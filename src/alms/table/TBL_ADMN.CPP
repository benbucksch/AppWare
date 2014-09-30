////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:  "tbl_admn.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#define   STRICT
#define   ASTRICT

// include files ///////////////////////////////////////////////////////////

#include  <windows.h>

#include  <a_alm.h>
#include  <op_wnd.h>
#include  <o_wnd.h>
#include  "intdebug.h"
#include  "tabl_hdr.h"
#include  "table.h"
#include  "tabl_row.h"
#include  "tbl_wlay.h"
class TABLE_WLAY;

// function prototypes /////////////////////////////////////////////////////

extern OpWndItemD* ALMAPI CreateTableVI(
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );
extern BOOL VerifyTable(AOBJECTID oid);
BOOL  Tabl_doCreate(AOBJECTID theObject);
BOOL  Tabl_doAssign(AOBJECTID dstObject, AOBJECTID srcObject);
BOOL  Tabl_exportObject(AOBJECTID theObject);
BOOL  Tabl_importObject(AOBJECTID theObject);
BOOL  Tabl_doVersionCheck(AOBJECTID theObject, long& dataVersion, long& status, char* errorMessage);

//extern  function prototypes /////////////////////////////////////////////////////

extern long Tabl_editObject(AOBJECTID  theObject);
extern long Tabl_getParameters(AOBJECTID theObject, AObjInfo* infoRecord);
extern void Tabl_validateObjectIDs(AOBJECTID theObject, pAObjMessage theSystem);
extern void Tabl_buildSketchSignals(AOBJECTID theObject, AMEMBLOCKID theSignalBlock);
extern void Tabl_InitDialogControls(HWND hDlg, AOBJECTID theObject);

int cxVS = GetSystemMetrics(SM_CXVSCROLL);
int cyVS = GetSystemMetrics(SM_CYVSCROLL);
int cxHS = GetSystemMetrics(SM_CXHSCROLL);
int cyHS = GetSystemMetrics(SM_CYHSCROLL);


// Tabl_Admn -- administration routine /////////////////////////////////////

long CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_Admn(
  AOBJECTID          theObject,
  pAObjMessage        theSystem)
  {
  long rVal = A_OK;
  switch((int)(theSystem->message1))
    {
    case AOBJ_CREATED:
      if (Tabl_doCreate((AOBJECTID)(theObject)) == FALSE)
        rVal = A_ERROR; // cannot create object
    case AOBJ_AWAKENED:
      {
      RUNTIMEINFO *rTime = (RUNTIMEINFO *)new RUNTIMEINFO(theObject);
      I_UNUSED(rTime);
      if (theSystem->message1 == AOBJ_AWAKENED)
        rVal = A_CONTINUE;
      break;
      }

    case AOBJ_DESTROYED:
    case AOBJ_ASLEEP:
      {
      RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);
      delete rTime;
      AObjSetRuntimeInfo(theObject, 0L);
      rVal = A_CONTINUE;
      }
      break;

    case AOBJ_ASSIGNOBJECT:
      {
      AOBJECTID dstID = theObject;
      AOBJECTID srcID = (AOBJECTID)theSystem->message3;
      if (Tabl_doAssign(dstID, srcID) == FALSE)
        rVal = A_ERROR;
      }
      break;

    case AOBJ_EXPORTOBJECT:
      VerifyTable(theObject);
      if (Tabl_exportObject((AOBJECTID)(theObject)) == FALSE)
        rVal = A_ERROR;
      break;

    case AOBJ_IMPORTOBJECT:
      if (Tabl_importObject((AOBJECTID)(theObject)) == FALSE)
        rVal = A_ERROR;
      break;

    case AOBJ_READ:
      if (Tabl_doVersionCheck((AOBJECTID)(theObject), theSystem->message3, theSystem->message4, theSystem->errorMessage) == FALSE)
        {
        rVal =  A_ERROR;
        AObjReportError   (theObject,
                           AOBJ_READ,
                           A_ERROR,
                           "table\tIncompatible Version",
                           0);
        }
        
      break;

    case AOBJ_EDITOBJECT:
      rVal = Tabl_editObject(theObject);
      break;

    case AOBJ_CHECKOBJECT:
      VerifyTable(theObject);
      rVal = Tabl_getParameters(theObject,(AObjInfo*)theSystem->message2);
      if ((rVal == A_ERROR) || (rVal == A_WARNING))
        {
        AObjReportError(theObject,
                        AOBJ_CHECKOBJECT,
                        rVal,
                        ((AObjInfo*)(theSystem->message2))->errorMessage,
                        0);
        }

      break;

    case AOBJ_VALIDATEOBJECTIDS:
      Tabl_validateObjectIDs(theObject, theSystem);
      break;

    case AOBJ_BUILDSIGNALS:
      Tabl_buildSketchSignals((AOBJECTID)theObject, (AMEMBLOCKID)(theSystem->message2));
      break;

    case AOBJ_QUERYPROTOCOL:
      {
      ApQueryInfo *pQueryInfo = (ApQueryInfo *) theSystem->message4;

      if (pQueryInfo->protocol == APROT_WND)  // if the window protocol is supported
        {
        pQueryInfo->pfnCreate = (APPCFN) CreateTableVI; // Entry point for WINDOW Object
        rVal = AP_SUPPORTED;
        }
      break;
      }
    default:
      break;
    }
  return rVal;
  }

// Tabl_doCreate ///////////////////////////////////////////////////////////

BOOL  Tabl_doCreate(
  AOBJECTID  theObject)

  {
  if(AObjSetDataCount(theObject, NUM_TABLEDATA) == FALSE)
    {
    // we need to warn the user that the object was not created properly
    return FALSE;
    }

  // call the TABLE_ATTRIBUTES constructor to set the default data for the object
  TABLE_ATTRIBUTES  table(theObject);
  return TRUE;
  }

// Tabl_doAssign ///////////////////////////////////////////////////////////

BOOL  Tabl_doAssign(
  AOBJECTID  dstObject,
  AOBJECTID  srcObject)

  {
  // if trying to assign an object to itself
  if (dstObject == srcObject)
    return FALSE; 

  //  make sure the versions for the source and destination are the current version
  TABLE             dstTable(dstObject);

  TABLE_ATTRIBUTES  srcAttrb(srcObject);  // called for version check
  COLUMN            srcColumn(srcObject);

  RUNTIMEINFO*      rtSrc = (RUNTIMEINFO*)AObjGetRuntimeInfo(srcObject);
  RUNTIMEINFO*      rtDst = (RUNTIMEINFO*)AObjGetRuntimeInfo(dstObject);
  ROW*              srcRow = rtSrc->GetRow();
  ROW*              dstRow = rtDst->GetRow();

  if(srcAttrb.Version()!=VERSION_OBJECT)
    return FALSE;

  if(!dstTable.AutoFill())
    {
    if(dstTable.Temporary())
      {
      // copy EVERYTHING if the destination is a temporary object
      long  lNumData  = AObjGetDataCount(srcObject);
      long  lDataLength;
      void* pData;
      long  i;
      
      AObjSetDataCount(dstObject, lNumData);
      for(i=0; i<lNumData; i++)
        {
        lDataLength = AObjGetDataSize(srcObject, i);
        pData = AObjLockData(srcObject, i);
        AObjSetData(dstObject, i, pData, lDataLength);
        AObjUnlockData(srcObject, i);
        }
      }
    // copy the cell information, but none of the attributes or column info
    else if(srcColumn.NumColumns()==dstTable.TotalNumCols())
      {
      // do AEVT_DIRECT copies of all the row data fields
      void* src  = AObjLockData(srcObject, DATA_ROWOFFSETS);
      long  size = AObjGetDataSize(srcObject, DATA_ROWOFFSETS);
      dstRow->FocalResizeData(dstObject, DATA_ROWOFFSETS, size, TRUE);
      void* dst  = AObjLockData(dstObject, DATA_ROWOFFSETS);
      if (src && dst)
        CopyMemory(dst, src, size);

      if (src)
        AObjUnlockData(srcObject, DATA_ROWOFFSETS);
      if (dst)
        AObjUnlockData(dstObject, DATA_ROWOFFSETS);

      src  = AObjLockData(srcObject, DATA_CELLDATA);
      size = AObjGetDataSize(srcObject, DATA_CELLDATA);
      dstRow->FocalResizeData(dstObject, DATA_CELLDATA, size, TRUE);
      dst = AObjLockData(dstObject, DATA_CELLDATA);

      if (src && dst)
        CopyMemory(dst, src, size);

      if (src)
        AObjUnlockData(srcObject, DATA_CELLDATA);
      if (dst)
        AObjUnlockData(dstObject, DATA_CELLDATA);

      src  = AObjLockData(srcObject, DATA_SELECTIONS);
      size = AObjGetDataSize(srcObject, DATA_SELECTIONS);
      dstRow->FocalResizeData(dstObject, DATA_SELECTIONS, size, TRUE);
      dst  = AObjLockData(dstObject, DATA_SELECTIONS);
      if (src && dst)
        CopyMemory(dst, src, size);
      if (src)
        AObjUnlockData(srcObject, DATA_SELECTIONS);
      if (dst)
        AObjUnlockData(dstObject, DATA_SELECTIONS);
      }
    else if(srcColumn.NumColumns()<dstTable.TotalNumCols())
      {
      int           numExtraCols = dstTable.TotalNumCols() - srcColumn.NumColumns();
      long          reqSpace = srcRow->DataLength() + srcRow->NumRows()*numExtraCols;
      dstRow->FocalResizeData(dstObject, DATA_CELLDATA, reqSpace, TRUE);
      char *    dst = (char *)AObjLockData(dstObject, DATA_CELLDATA);
      long          length;
      long          offset=0;
      dstRow->FocalResizeData(dstObject, DATA_ROWOFFSETS, sizeof(long)*srcRow->NumRows(), TRUE);
      long *    dstOff = (long *)AObjLockData(dstObject, DATA_ROWOFFSETS);

      for(long i=0; i<srcRow->NumRows(); i++)
        {
        length = srcRow->RowLength(i, FALSE, NULL) + numExtraCols;
        srcRow->GetRow(i, dst+offset, length, FALSE, FALSE, NULL);
        *(dstOff+i) = offset;
        offset+=length;
        }
      if (dstOff)
        AObjUnlockData(dstObject, DATA_ROWOFFSETS);

      void* src = AObjLockData(srcObject, DATA_SELECTIONS);
      long  size = AObjGetDataSize(srcObject, DATA_SELECTIONS);
      dstRow->FocalResizeData(dstObject, DATA_SELECTIONS, size, TRUE);
      void* dest = AObjLockData(dstObject, DATA_SELECTIONS);
      if (dest && src)
        CopyMemory(dest, src, size);
      if (src)
        AObjUnlockData(srcObject, DATA_SELECTIONS);
      if (dest)
        AObjUnlockData(dstObject, DATA_SELECTIONS);

      if (dst)
        AObjUnlockData(dstObject, DATA_CELLDATA);
      }
    else
      {
      // more source columns than destination columns -- hardest case
      dstRow->FocalResizeData(dstObject, DATA_CELLDATA, srcRow->DataLength(), TRUE);
      char *    dst =(char *) AObjLockData(dstObject, DATA_CELLDATA);

      long          length;
      long          offset=0;
      long *    dstOff;
      dstRow->FocalResizeData(dstObject, DATA_ROWOFFSETS, sizeof(long)*srcRow->NumRows(), TRUE);
      dstOff = (long *)AObjLockData(dstObject, DATA_ROWOFFSETS);

      for(long i=0; i<srcRow->NumRows(); i++)
        {
        *(dstOff+i) = offset;
        for(int j=0; j<dstTable.TotalNumCols(); j++)
          {
          length = lstrlen(srcRow->GetCell(i, j, FALSE, NULL))+1;
          CopyMemory(dst+offset, srcRow->GetCell(i, j, FALSE, NULL), length);
          offset+=length;
          }
        }

      if (dst)
        AObjUnlockData(dstObject, DATA_CELLDATA);

      dstRow->FocalResizeData(dstObject, DATA_CELLDATA, offset, TRUE);

      void* src = AObjLockData(srcObject, DATA_SELECTIONS);
      long  size = AObjGetDataSize(srcObject, DATA_SELECTIONS);
      dstRow->FocalResizeData(dstObject, DATA_SELECTIONS, size, TRUE);
      void* dest = AObjLockData(dstObject, DATA_SELECTIONS);
      if (dest && src)
        CopyMemory(dest, src, size);
      if (src)
        AObjUnlockData(srcObject, DATA_SELECTIONS);
      if (dest)
        AObjUnlockData(dstObject, DATA_SELECTIONS);
      if (dstOff)
        AObjUnlockData(dstObject, DATA_ROWOFFSETS);
      }

    dstTable.PostObjectChanged(CHANGED_NUMROWS);
    return TRUE;
    }
  else
    // no assignment will take place if the table is connected to a database
    return FALSE;
  }

// Tabl_exportObject ///////////////////////////////////////////////////////

BOOL  Tabl_exportObject(
  AOBJECTID  theObject)

  {
  TABLE_ATTRIBUTES  table(theObject);
  COLUMN            column(theObject);

  if(!table.Export())
    return FALSE;

  if(!column.Export())
    return FALSE;

  return TRUE;
  }

// Tabl_importObject ///////////////////////////////////////////////////////

BOOL  Tabl_importObject(
  AOBJECTID  theObject)

  {
  TABLE_ATTRIBUTES  table(theObject);
  COLUMN            column(theObject);

  if(!table.Import())
    return FALSE;

  if(!column.Import())
    return FALSE;

  return TRUE;
  }

// Tabl_doVersionCheck /////////////////////////////////////////////////////

BOOL Tabl_doVersionCheck(
  AOBJECTID  theObject,
  long&     dataVersion,
  long&     status,
  char*     errorMessage)

  {
  BOOL rVal = TRUE; // true := OK / FALSE := version Error
  int version = *(int*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
  AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);

  switch(version)
    {
    case 0:
      {
      // upgrade the object....
      TABLE_ATTRIBUTES  ta(theObject);
      }

    case VERSION_OBJECT1_0: // case 1:
      {
      TABLE_ATTRIBUTES  ta(theObject);    // resize DATA_ATTRIBUTES to be sizeof(TATTRIB)
      // status = versionOK;
      dataVersion = VERSION_OBJECT;
      // if it is an pre April 27 version (i.e. < version 2.0)
      // Set adhoc to be false
      ta.SetAdHoc(FALSE);
      }
      break;

    case VERSION_OBJECT:
      {
      dataVersion = VERSION_OBJECT;
      }
      break;

    default:
      // status = versionError;
      lstrcpy(errorMessage, "Table\talm_tabl.dll");
      rVal = FALSE;
      break;
    }

  I_UNUSED(status);
  return rVal;
  }

// GetSerVersion ///////////////////////////////////////////////////////////

long CALLBACK /*!!PORT!! was _far_ _pascal_*/  GetSerVersion(
  ATYPEID  theType)

  {
  if(theType==OTYPE_TABLE)
    return VERSION_DLL;
  else
    return 0L;
  }

////////////////////////////////////////////////////////////////////////////

/**************************************************************************
* Function: Table Visual Instance Creator
* Shaili Jain
* Aug. 30, 1993
* Description: A pointer to this function is passed back to the Window  Object
*              The Window Object calls this for every v-instance of the table
*              item.  The constructor to TABLE is called to create and init
*              a visual instance of Table's c++ class.  The methods in this
*              class provide the implentation for the Window Item Protocol
*
**************************************************************************/
OpWndItemD* ALMAPI CreateTableVI(
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  )
  {
  long statusFlag = 0L;
  TABLE       *table;
  TABLE_WLAY  *tableWLAY;
  BOOL isLayout = OWndIsLayoutTime();
  OpWndItemD *  defItem;
  I_UNUSED(special);

  if (isLayout)
    {
    tableWLAY = new TABLE_WLAY(oiItem, oiParent, rcItem, status);
    defItem = tableWLAY;
    }
  else
    {
    table = new TABLE(oiItem,oiParent, rcItem,(HWND)0, status);
    defItem = table;
    }

  if (isLayout)
    return tableWLAY;
  else
    {
    OWndSetItemUsesTab(defItem, TRUE);
    OWndSetItemUsesCursorKeys(defItem, TRUE);
    OWndSetItemUsesDefaultKey(defItem, TRUE);
    OWndSetItemUsesCancelKey(defItem, TRUE);
    OWndSetItemEnabled(defItem, TRUE);

    OWndSetItemActivatable(defItem,table->CanActivate());
    // set the status flags for the table
    statusFlag |= (SF_USESTAB | SF_ENTERTHIEFPROOF);
    statusFlag |= (SF_USESCURSORKEYS | SF_TABSTOP);
    if(table->CanActivate())
      statusFlag |= SF_CANACTIVATE;
    else
      statusFlag &= ~SF_CANACTIVATE;

    table->SetStatusFlag(statusFlag);

    return table;
    }
  }
