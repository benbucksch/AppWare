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
// FILE:  "tbl_objd.cpp"
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

#include  <windows.h>
#include  <stdlib.h>
#include  <stdio.h>
#include  <mem.h>
#include  <ctype.h>

#include  <a_alm.h>
#include  <a_almutl.h>
#include  <op_wnd.h>
#include  <o_wnd.h>
#include  <o_text.h>
#include  <o_number.h>
#include  <o_time.h>
#include  <o_date.h>
#include  <op_dview.h>
#include  <commdlg.h>
#include  <intdebug.h>

#include  <helpids.h>

#include  "tabl_hdr.h"
#include  "tbl_atrb.h"
#include  "tbl_clmn.h"
#include  "tbl_objd.h"
#include  "tabl_rsc.h"
#include  "o_table.h"

// define the help id if it is not already defined /////////////////////////

#ifndef HELPID_DLG_TABLEOPTIONS
#define HELPID_DLG_TABLEOPTIONS   3003
#endif

// global variable container class /////////////////////////////////////////

extern GLOBAL   G;

// function prototypes /////////////////////////////////////////////////////

long  Tabl_editObject(OBJECTID  theObject);
long  Tabl_getParameters(OBJECTID theObject, AObjInfo* infoRecord);
void  Tabl_validateObjectIDs(OBJECTID theObject, pAObjMessage theSystem);
void  Tabl_buildSketchSignals(OBJECTID theObject, MEMBLOCKID theSignalBlock);
void  Tabl_InitDialogControls(HWND hDlg, OBJECTID theObject);

////////////////////////////////////////////////////////////
// Verify Table ---> Check for deleted oid refs.
// Return: TRUE --> if all oids are OK
//         FALSE --> else
////////////////////////////////////////////////////////////
BOOL VerifyTable(OBJECTID oid)
  {
  BOOL        bAllOIDS = TRUE;
  BOOL        valid;
  OBJECTID    colObj;
  TABLE_OBJD  table(oid);

  // check the column objects -- if the column is tied to an object
  for(int i=0; i<table.NumColumns(); i++)
    {
    colObj = table.GetColumnObjectID(i);

    // go to the next iteration if the column is not tied to an object
    if(colObj == NULL)
      continue;

    // check the object ID for the columns that are tied to an object
    valid = AObjCheckType(colObj, 0L);
    if(!valid)
      {
      // delete the object -- the object has apparently been deleted
      table.DeleteColumn(i);
      i--;
      bAllOIDS = FALSE;
      }
    }

  // check the TABLE_ATTRIBUTE row object
  OBJECTID  rowObj = table.RowObject();
  valid = AObjCheckType(rowObj, 0L);
  if(!valid)   {
    table.SetRowObject(NULL);
    bAllOIDS = FALSE;
    }

  return bAllOIDS;  // all oids OK == TRUE
                    // else FALSE
  }
// Tabl_editObject /////////////////////////////////////////////////////////

long Tabl_editObject(
  OBJECTID  theObject)

  {
  long        rval;
  int         dlgID;
  HWND        hActiveWnd;

  VerifyTable(theObject);
  TABLE_OBJD  table(theObject);

  dlgID = TABLEDIALOG;
  hActiveWnd = GetActiveWindow();
  rval = DialogBoxParam(
    G.hInstance,
    MAKEINTRESOURCE(dlgID),
    hActiveWnd,
    (DLGPROC)Tabl_DialogProc,
    (LPARAM)&table);

  I_UNUSED(dlgID);

  if (rval == -1)
    return A_ERROR;
  else if (rval)
    return A_MODIFIED;
  else return A_OK;
  }


// Tabl_getParameters //////////////////////////////////////////////////////

long Tabl_getParameters(
  OBJECTID          theObject,
  AObjInfo* infoRecord)

  {
  long errorCode;
  // check the version and do the update as necessary
  TABLE_ATTRIBUTES  tAttrib(theObject);

  // a table object will always be valid -- if there are no columns give a warning
  if(AObjGetDataSize(theObject, DATA_COLUMN) < sizeof(TCOLS)
      ||
     (tAttrib.AutoFill() && !tAttrib.DataSource() && !tAttrib.RowIndexObject()))
    {
    errorCode = A_WARNING;
    if(AObjGetDataSize(theObject, DATA_COLUMN) < sizeof(TCOLS)
          &&
       (tAttrib.AutoFill() && !tAttrib.DataSource() && !tAttrib.RowIndexObject()))
      {
      lstrcpy(infoRecord->errorMessage, "There are no columns in the table.\r\n\r\nAn auto fill table without a Data Source Object needs to have a Row Index Object specified.");
      }
    else if(tAttrib.AutoFill() && !tAttrib.DataSource() && !tAttrib.RowIndexObject())
      lstrcpy(infoRecord->errorMessage, "An auto fill table without a Data Source Object needs to have a Row Index Object specified.");
    else
      errorCode = A_OK;
    }
  else
    errorCode = A_OK;
  return errorCode;
  }

// Tabl_validateObjectIDs //////////////////////////////////////////////////

void Tabl_validateObjectIDs(
  OBJECTID  theObject,
  pAObjMessage  theSystem)

  {
  BOOL      changed;

  // validate the connected row object
  TABLE_ATTRIBUTES  table_attrib(theObject);
  OBJECTID  rowObj = table_attrib.RowIndexObject();
  AObjValidateID(&rowObj, theSystem);
  table_attrib.SetRowIndexObject(rowObj);

  // validate the data source object
  OBJECTID  dataSourceObj = table_attrib.DataSource();
  AObjValidateID(&dataSourceObj, theSystem);
  table_attrib.SetDataSource(dataSourceObj);

  // validate the indexed field object
  OBJECTID  idxFieldObj = table_attrib.IndexedField();
  AObjValidateID(&idxFieldObj, theSystem);
  table_attrib.SetIndexedField(idxFieldObj);

  // validate the columns -- update the object ids (if any)
  COLUMN    cols(theObject);
  OBJECTID  colObj;
  for(int i=0; i<cols.NumColumns(); i++)
    {
    colObj = cols.GetObjectID(i);

    // go to the next iteration if the column is not tied to an object
    if(colObj == NULL)
      continue;

    // check the object ID for the columns that are tied to an object
    changed = AObjValidateID(&colObj, theSystem);
    if(changed && !colObj)
      {
      // delete the object -- the object has apparently been deleted
      cols.DeleteColumn(i);
      i--;
      }
    else
      cols.SetObjectID(i, colObj);
    }
  }

// Tabl_buildSketchSignals /////////////////////////////////////////////////

void Tabl_buildSketchSignals(
  OBJECTID    theObject,
  MEMBLOCKID  theSignalBlock)

  {
  TABLE_ATTRIBUTES  tableAttributes(theObject);
  pAObjSignalRecord   theSignals;
  int  numSignals = tableAttributes.NumSignals();

  // if the table is an AUTOFILL w/ autoColumn (adhoc), then remove the  Needs row signal
  if (tableAttributes.AutoFill() && tableAttributes.AdHoc())
    numSignals -= 1;

  AMemSetSize(theSignalBlock, (long)sizeof(AObjSignalRecord)*tableAttributes.NumSignals());
  theSignals = (pAObjSignalRecord)AMemLock(theSignalBlock);

  // check the return value -- make sure we have a valid pointer
  if(!theSignals)
    return;

  int i=0;
  theSignals[i].theID = SIGNAL_ROWCLICKED;
  lstrcpy(theSignals[i].theName, SIGNAME_ROWCLICKED);

  theSignals[++i].theID = SIGNAL_ROWDOUBLECLICKED;
  lstrcpy(theSignals[i].theName, SIGNAME_ROWDOUBLECLICKED);

  theSignals[++i].theID = SIGNAL_SWITCHCOLUMNS;
  lstrcpy(theSignals[i].theName, SIGNAME_SWITCHCOLUMNS);

  theSignals[++i].theID = SIGNAL_SELECTIONCHANGED;
  lstrcpy(theSignals[i].theName, SIGNAME_SELECTIONCHANGED);

  if (tableAttributes.AutoFill())
    {
    if (tableAttributes.AdHoc() == FALSE)
      {
      theSignals[++i].theID = SIGNAL_NEEDSROW;
      lstrcpy(theSignals[i].theName, SIGNAME_NEEDSROW);
      }
    theSignals[++i].theID = SIGNAL_TABLEAUTOFILLED;
    lstrcpy(theSignals[i].theName, SIGNAME_TABLEAUTOFILLED);

    theSignals[++i].theID = SIGNAL_STARTINGAUTOFILL;
    lstrcpy(theSignals[i].theName, SIGNAME_STARTINGAUTOFILL);
    }

  if(tableAttributes.CanActivate())
    {
    theSignals[++i].theID = SIGNAL_RETURNPRESSED;
    lstrcpy(theSignals[i].theName, SIGNAME_RETURNPRESSED);

    theSignals[++i].theID = SIGNAL_DELETEPRESSED;
    lstrcpy(theSignals[i].theName, SIGNAME_DELETEPRESSED);

    theSignals[++i].theID = SIGNAL_TABLEACTIVATED;
    lstrcpy(theSignals[i].theName, SIGNAME_TABLEACTIVATED);

    theSignals[++i].theID = SIGNAL_TABLEDEACTIVATED;
    lstrcpy(theSignals[i].theName, SIGNAME_TABLEDEACTIVATED);

    if(!tableAttributes.AutoFill())
      {
      theSignals[++i].theID = SIGNAL_TABLEEDITED;
      lstrcpy(theSignals[i].theName, SIGNAME_TABLEEDITED);

      theSignals[++i].theID = SIGNAL_STARTEDITING;
      lstrcpy(theSignals[i].theName, SIGNAME_STARTEDITING);
  
      theSignals[++i].theID = SIGNAL_STOPEDITING;
      lstrcpy(theSignals[i].theName, SIGNAME_STOPEDITING);
      }
    }

  AMemUnlock(theSignalBlock);
  AMemSetSize(theSignalBlock, (long)sizeof(AObjSignalRecord)*(i+1));
  }

// Tabl_DialogProc /////////////////////////////////////////////////////////

BOOL far pascal _export Tabl_DialogProc(
  HWND    hDlg,
  UINT    message,
  WPARAM  wParam,
  LPARAM  lParam)

  {
  TABLE_OBJD*   tableInfo;

  switch(message)
    {
    case WM_INITDIALOG:
      {
      // write the object name to the title bar
      char  objName[255];
      char  title[255];
      tableInfo = (TABLE_OBJD*)lParam;
      AObjGetName(tableInfo->GetObject(), objName, 255);
      SetWindowText(hDlg, objName);
      SetWindowLong(hDlg, G.cbByteOffset, (long)tableInfo);

      // initialize the dialog
      SetProp(hDlg, "lastType", NULL);
      SetProp(hDlg, "numSel", (HANDLE)0);
      SetProp(hDlg, "sameTitleFont", (HANDLE)FALSE);
      SetProp(hDlg, "sameCellFont", (HANDLE)FALSE);
      SetProp(hDlg, "sameTitleFG", (HANDLE)FALSE);
      SetProp(hDlg, "sameCellFG", (HANDLE)FALSE);
      SetProp(hDlg, "sameTitleBG", (HANDLE)FALSE);
      SetProp(hDlg, "sameCellBG", (HANDLE)FALSE);
      tableInfo->InitDialogControls(hDlg);

      // TITLE on dlg box
      ATypeGetName(OTYPE_TABLE, title, 34);
      AUtlSetTitleFont(OTYPE_TABLE, title, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);

      return TRUE;
      }

    case WM_DESTROY:
      RemoveProp(hDlg, "lastType");
      RemoveProp(hDlg, "numSel");
      RemoveProp(hDlg, "sameTitleFont");
      RemoveProp(hDlg, "sameCellFont");
      RemoveProp(hDlg, "sameTitleFG");
      RemoveProp(hDlg, "sameCellFG");
      RemoveProp(hDlg, "sameTitleBG");
      RemoveProp(hDlg, "sameCellBG");
      break;

    case WM_COMMAND:
      tableInfo = (TABLE_OBJD*)GetWindowLong(hDlg, G.cbByteOffset);
      switch(wParam)
        {
        // buttons on right side of dialog
        case IDOK:
          SetFocus(GetDlgItem(hDlg, IDOK));
          tableInfo->UpdateObjectData();
          EndDialog(hDlg, TRUE);
          return TRUE;

        case IDCANCEL:
          SetFocus(GetDlgItem(hDlg, IDCANCEL));
          EndDialog(hDlg, FALSE);
          return TRUE;

        case IDD_OPTIONS:
          {
          // Save the last state of all the object-type check boxes
          if(!DialogBoxParam(G.hInstance, MAKEINTRESOURCE(TABLEATTRIBUTES), hDlg, (DLGPROC)Tabl_AttribProc, (LPARAM)tableInfo))
            return TRUE;

          if(tableInfo->TABLE_ATTRIBUTES::AutoFill())
            {
            if (tableInfo->TABLE_ATTRIBUTES::AdHoc())
              {
              EnableWindow(GetDlgItem(hDlg, IDD_SOURCEFIELDS), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_TIME), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_DATE), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_ALL), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_TEXT), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_NUMBER), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_AVAILOBJS), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_COLUMNS), FALSE);
              }
            else
              {
              EnableWindow(GetDlgItem(hDlg, IDD_TIME), TRUE);
              EnableWindow(GetDlgItem(hDlg, IDD_DATE), TRUE);
              EnableWindow(GetDlgItem(hDlg, IDD_ALL),TRUE);
              EnableWindow(GetDlgItem(hDlg, IDD_TEXT), TRUE);
              EnableWindow(GetDlgItem(hDlg, IDD_NUMBER), TRUE);
              EnableWindow(GetDlgItem(hDlg, IDD_AVAILOBJS), TRUE);
              EnableWindow(GetDlgItem(hDlg, IDD_COLUMNS), TRUE);


              if(IsDlgButtonChecked(hDlg, IDD_ALL))
                {
                CheckDlgButton(hDlg, IDD_ALL, FALSE);
                CheckDlgButton(hDlg, IDD_SOURCEFIELDS, TRUE);
                tableInfo->FillAvailableObjects(hDlg, IDD_SOURCEFIELDS);
                }
              else if(IsDlgButtonChecked(hDlg, IDD_SOURCEFIELDS))
                {
                SetProp(hDlg, "lastType", (HANDLE)IDD_ALL);
                tableInfo->FillAvailableObjects(hDlg, IDD_SOURCEFIELDS);
                }
              EnableWindow(GetDlgItem(hDlg, IDD_SOURCEFIELDS), TRUE);
              }
            }
          else
            {
            EnableWindow(GetDlgItem(hDlg, IDD_TIME), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_DATE), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_ALL),TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_TEXT), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_NUMBER), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_AVAILOBJS), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDD_COLUMNS), TRUE);

            if(IsDlgButtonChecked(hDlg, IDD_SOURCEFIELDS))
              {
              CheckDlgButton(hDlg, IDD_ALL, TRUE);
              CheckDlgButton(hDlg, IDD_SOURCEFIELDS, FALSE);
              tableInfo->FillAvailableObjects(hDlg, IDD_ALL);
              }

            EnableWindow(GetDlgItem(hDlg, IDD_SOURCEFIELDS), FALSE);
            }

          int   numSel = (int)GetProp(hDlg, "numSel");

          if (tableInfo->TABLE_ATTRIBUTES::AdHoc())
            {
            ShowWindow(GetDlgItem(hDlg, IDD_DEFCOLUMNATTRIB), SW_SHOW);
            }
          else {
            if (numSel <= 0)
              ShowWindow(GetDlgItem(hDlg, IDD_DEFCOLUMNATTRIB), SW_SHOW);
            else {
              ShowWindow(GetDlgItem(hDlg, IDD_DEFCOLUMNATTRIB), SW_HIDE);
              ShowWindow(GetDlgItem(hDlg, IDD_SELCOLUMNATTRIB), SW_SHOW);
              }
            }
          return TRUE;
          }

        case IDHELP:
          WinHelp(hDlg, SERIUS_HELPFILE, HELP_CONTEXT, HELPID_OBJD_Tabl);
          return TRUE;

        // object type radio buttons
        case IDD_ALL:
        case IDD_TEXT:
        case IDD_NUMBER:
        case IDD_DATE:
        case IDD_TIME:
        case IDD_SOURCEFIELDS:
          tableInfo->FillAvailableObjects(hDlg, wParam);
          tableInfo->SetEnabledState(hDlg);
          return TRUE;

        // title alignment radio buttons
        case IDD_TITLELEFT:
        case IDD_TITLECENTER:
        case IDD_TITLERIGHT:
          tableInfo->ChangeTitleAlignmentRB(hDlg, wParam, TRUE);
          return TRUE;

        // cell alignment radio buttons
        case IDD_LEFT:
        case IDD_CENTER:
        case IDD_RIGHT:
          tableInfo->ChangeAlignmentRadioButton(hDlg, wParam, TRUE);
          return TRUE;

        // width radio buttons
        case IDD_RBPIXELS:
        case IDD_RBCHARACTERS:
        case IDD_RBINCHES:
          tableInfo->ChangeWidthRadioButton(hDlg, wParam);
          return TRUE;

        // editable check box
        case IDD_EDITABLE:
          tableInfo->EditableHit(hDlg);
          return TRUE;

        // more push buttons
        case IDD_ADD:
          tableInfo->Add(hDlg);
          return TRUE;

        case IDD_REMOVE:
          tableInfo->Remove(hDlg);
          return TRUE;

        case IDD_EDIT:
          tableInfo->Edit(hDlg);
          return TRUE;

        // list boxes
        case IDD_AVAILOBJS:
        case IDD_COLUMNS:
          return tableInfo->ListBoxCommand(hDlg, wParam, lParam);

        // edit controls
        case IDD_TITLE:
        case IDD_PIXELS:
        case IDD_CHARACTERS:
        case IDD_INCHES:
          return tableInfo->EditControlCommand(hDlg, wParam, lParam);

        // column font push buttons
        case IDD_TITLEFONT:
        case IDD_CELLFONT:
          return tableInfo->FontDialog(hDlg, wParam);

        default:
          break;
        }
      break;

    default:
      break;
    }

  return FALSE;
  }

// TABLE_OBJD::TABLE_OBJD -- constructor ////////////////////////////////////

TABLE_OBJD::TABLE_OBJD(
  OBJECTID  objID)
  : TABLE_ATTRIBUTES(objID, &tblAttrib),
    COLUMN(objID, hCols, numCols)

  {
  theObject = objID;
  } 

// TABLE_OBJD::InitDialogControls //////////////////////////////////////////

void TABLE_OBJD::InitDialogControls(
  HWND      hDlg)

  {
  // default to selecting the ALL or SOURCE FIELDS radio button
  WPARAM  radButt = IDD_ALL;
  if(TABLE_ATTRIBUTES::AutoFill())
    radButt = IDD_SOURCEFIELDS;

  CheckDlgButton(hDlg, radButt, TRUE);
  FillAvailableObjects(hDlg, radButt);
  FillColumnsInTable(hDlg);
  ChangeWidthRadioButton(hDlg, TABLE_ATTRIBUTES::LastWidthRB());
  SetEnabledState(hDlg);
  EnableWindow(GetDlgItem(hDlg, IDD_SOURCEFIELDS), TABLE_ATTRIBUTES::AutoFill() && !(TABLE_ATTRIBUTES::AdHoc()));
  }

// TABLE_OBJD::SetAddEnableState ///////////////////////////////////////////

void TABLE_OBJD::SetAddEnableState(
  HWND  hDlg)

  {
  int availSelCount = (int)SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_GETSELCOUNT, 0, 0L);
  EnableWindow(GetDlgItem(hDlg, IDD_ADD), (availSelCount>0) && !AdHoc());
  }

// TABLE_OBJD::SetEnabledState /////////////////////////////////////////////

void TABLE_OBJD::SetEnabledState(
  HWND  hDlg)

  {
  SetAddEnableState(hDlg);

  int   numSel = (int)GetProp(hDlg, "numSel");

  if(TABLE_ATTRIBUTES::AutoFill())
    {
    if (TABLE_ATTRIBUTES::AdHoc())
      {
      EnableWindow(GetDlgItem(hDlg, IDD_SOURCEFIELDS), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_TIME), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_DATE), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_ALL), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_TEXT), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_NUMBER), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_AVAILOBJS), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDD_COLUMNS), FALSE);
      }
    }

  // only display title if there is a single selection
  if(numSel!=1)
    SetDlgItemText(hDlg, IDD_TITLE, NULL);

  // windows for 1 selection only
  EnableWindow(GetDlgItem(hDlg, IDD_EDIT), (numSel==1) && !AdHoc());
  EnableWindow(GetDlgItem(hDlg, IDD_TITLE), (numSel==1) || (numSel==0));
  EnableWindow(GetDlgItem(hDlg, IDD_STATICTITLE), (numSel==1) || (numSel==0));

  ShowWindow(GetDlgItem(hDlg, IDD_SELCOLUMNATTRIB), (numSel>0)?SW_SHOW:SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDD_DEFCOLUMNATTRIB), (numSel<=0)?SW_SHOW:SW_HIDE); // SKJ
    
  // window for any selection
  EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), (numSel>0) && !AdHoc());
  EnableWindow(GetDlgItem(hDlg, IDD_TITLEALIGNMENT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_TITLELEFT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_TITLECENTER), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_TITLERIGHT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_ALIGNMENT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_LEFT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_CENTER), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_RIGHT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_EDITABLE), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_TITLEFONT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_CELLFONT), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_WIDTH), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_RBPIXELS), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_PIXELS), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_RBCHARACTERS), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_CHARACTERS), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_RBINCHES), (numSel>=0));
  EnableWindow(GetDlgItem(hDlg, IDD_INCHES), (numSel>=0));
  }

// TABLE_OBJD::UpdateObjectData ////////////////////////////////////////////

void TABLE_OBJD::UpdateObjectData(
  )

  {
  // copy the column data to the object data
  if(hCols)
    {
    TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);
    AObjResizeData(theObject, DATA_COLUMN, (long)numCols*sizeof(TCOLS));
    TCOLS*  objPtr = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
    hmemcpy(objPtr, colPtr, (long)numCols*sizeof(TCOLS));
    GlobalUnlock(hCols);
    AObjUnlockData(theObject, DATA_COLUMN);
    }
  else
    AObjResizeData(theObject, DATA_COLUMN, 0L);

  // copy the table attribute data to the object data
  AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
  TATTRIB*  tattribPtr = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
  memcpy(tattribPtr, &tblAttrib, sizeof(TATTRIB));
  AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
  }

// TABLE_OBJD::FillColumnsInTable //////////////////////////////////////////

void TABLE_OBJD::FillColumnsInTable(
  HWND  hDlg)

  {
  // give a blank string for inserting at the end
  SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_INSERTSTRING, 0, (LPARAM)"");

  // make sure there are some columns -- at least a valid memory handle
  if(!hCols)
    return;

  // fill in the columns list box   
  TCOLS*    colPtr = (TCOLS*)GlobalLock(hCols);
  OBJECTID  colObj;
  for(int i=0; i<numCols; i++)
    {
    char  szBuffer[32];
    if((colObj = COLUMN::ObjectID(i, colPtr))!=NULL)
      {
      AObjGetName(colObj, szBuffer, 32);
      SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_INSERTSTRING, i, (LPARAM)szBuffer);
      }
    else
      SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_INSERTSTRING, i, (LPARAM)COLUMN::Title(i, colPtr));
    }
  GlobalUnlock(hCols);
  }

// TABLE_OBJD::FillAvailableObjects /////////////////////////////////////////

void  TABLE_OBJD::FillAvailableObjects(
  HWND    hDlg,
  WPARAM  wParam)

  {
  TYPEID    availableType;
  long      theGetKey = AOBJ_GETFIRSTKEY;
  OBJECTID  anObject;
  char      szBuffer[80];
  int       index;

  if((WPARAM)GetProp(hDlg, "lastType")==wParam)
    return;
  else
    SetProp(hDlg, "lastType", (HANDLE)wParam);

  // clear any current objects
  SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_RESETCONTENT, 0, 0L);

  // determine which type to list
  switch(wParam)
    {
    case IDD_TEXT:
      availableType = OTYPE_TEXT;
      break;

    case IDD_NUMBER:
      availableType = OTYPE_NUMBER;
      break;

    case IDD_DATE:
      availableType = OTYPE_DATE;
      break;

    case IDD_TIME:
      availableType = OTYPE_TIME;
      break;

    case IDD_SOURCEFIELDS:
      {
      if(TABLE_ATTRIBUTES::DataSource())
        {
        MEMBLOCKID  theBlock = AMemAllocate(0L);
        AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, autoTableGetFieldObjects, (long)theBlock);
        int         numObj = (int)AMemGetSize(theBlock)/sizeof(OBJECTID);
        OBJECTID*   objs = (OBJECTID*)AMemLock(theBlock);
        TYPEID      theType;
        if(objs)
          {
          for(int i=0; i<numObj; i++)
            {
            theType = AObjGetType(objs[i]);
            if(AEvtCheckForMethod(theType, AEVENT_OBJECTTOTEXT))
              {
              AObjGetName(objs[i], szBuffer, 80);
              index = (int)SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
              if(index==LB_ERR)
                break;
              SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_SETITEMDATA, (WPARAM)index, (LPARAM)objs[i]);
              }
            }

          AMemUnlock(theBlock);
          }
        AMemFree(theBlock);
        }
      else
        {
        availableType = NULL;
        break;
        }
      return;
      }

    default:
      availableType = NULL;
      break;
    }

  if(availableType)
    {
    while((anObject=AObjGetNextObject(availableType, &theGetKey))!=NULL)
      {
      // add anObject to the list box
      AObjGetName(anObject, szBuffer, 80);
      index = (int)SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
      if(index==LB_ERR)
        break;
      SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_SETITEMDATA, (WPARAM)index, (LPARAM)anObject); 
      }
    }
  else  // IDD_ALL
    {
    // list all objects that support AObjConvertToText
    TYPEID  aType = NULL;
    while((aType = ATypeGetNext(&theGetKey))!=NULL)
      {
      if(AEvtCheckForMethod(aType, AEVENT_OBJECTTOTEXT))
        {
        long  cookie = AOBJ_GETFIRSTKEY;

        while((anObject=AObjGetNextObject(aType, &cookie))!=NULL)
          {
          // add anObject to the list box
          AObjGetName(anObject, szBuffer, 80);
          index = (int)SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
          if(index==LB_ERR)
            break;
          SendDlgItemMessage(hDlg, IDD_AVAILOBJS, LB_SETITEMDATA, (WPARAM)index, (LPARAM)anObject); 
          } // end...while((anObject=AObjGetNextObject(....
        } // end...if(AEvtCheckForMethod(....
      } // end....while((aType = ATypeGetNext(....
    } // end...if(availableType)...else
  } // end...TABLE_OBJD::FillAvailableObjects

// TABLE_OBJD::EditableHit /////////////////////////////////////////////////

void TABLE_OBJD::EditableHit(
  HWND  hDlg)

  {
  int numSel = (int)GetProp(hDlg, "numSel");
  if(numSel<0)
    return;

  BOOL  newState = !IsDlgButtonChecked(hDlg, IDD_EDITABLE);

  if (numSel == 0)
    {
    CheckDlgButton(hDlg, IDD_EDITABLE, newState);
    COLUMN::SetEditable(0, (TCOLS *)&tblAttrib.defColumn, newState);
    }
  else {
    int*  selList = new int[numSel];
    if(!selList)
      return;

    SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, (WPARAM)numSel, (LPARAM)selList);

    CheckDlgButton(hDlg, IDD_EDITABLE, newState);

    // modify the editable field for each selected object
    TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);

    for(int i=0; i<numSel; i++)
      COLUMN::SetEditable(selList[i], colPtr, newState);
    GlobalUnlock(hCols);
    }
  }

// TABLE_OBJD::ChangeTitleAlignmentRB //////////////////////////////////////

void TABLE_OBJD::ChangeTitleAlignmentRB(
  HWND    hDlg,
  WPARAM  wParam,
  BOOL    changeData)

  {
  // make sure only one of the radio buttons is selected.
  if(wParam!=IDD_TITLELEFT)
    CheckDlgButton(hDlg, IDD_TITLELEFT, FALSE);
  else
    CheckDlgButton(hDlg, IDD_TITLELEFT, TRUE);

  if(wParam != IDD_TITLECENTER)
    CheckDlgButton(hDlg, IDD_TITLECENTER, FALSE);
  else
    CheckDlgButton(hDlg, IDD_TITLECENTER, TRUE);

  if(wParam != IDD_TITLERIGHT)
    CheckDlgButton(hDlg, IDD_TITLERIGHT, FALSE);
  else
    CheckDlgButton(hDlg, IDD_TITLERIGHT, TRUE);

  if(changeData)
    {
    TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);
    int     numSel = (int)GetProp(hDlg, "numSel");
    int*    selList = new int[numSel];

    if (numSel  == 0)
      COLUMN::SetTitleAlignment(0, &tblAttrib.defColumn, (char)(wParam - 500));
    
    for(int i=0; i<numSel; i++)
      COLUMN::SetTitleAlignment(selList[i], colPtr, (char)(wParam - 500));

    GlobalUnlock(hCols);
    delete selList;
    }
  }

// TABLE_OBJD::ChangeAlignmentRadioButton //////////////////////////////////

void TABLE_OBJD::ChangeAlignmentRadioButton(
  HWND    hDlg,
  WPARAM  wParam,
  BOOL    changeData)

  {
  // make sure only one of the radio buttons is selected.
  if(wParam!=IDD_LEFT)
    CheckDlgButton(hDlg, IDD_LEFT, FALSE);
  else
    CheckDlgButton(hDlg, IDD_LEFT, TRUE);

  if(wParam != IDD_CENTER)
    CheckDlgButton(hDlg, IDD_CENTER, FALSE);
  else
    CheckDlgButton(hDlg, IDD_CENTER, TRUE);

  if(wParam != IDD_RIGHT)
    CheckDlgButton(hDlg, IDD_RIGHT, FALSE);
  else
    CheckDlgButton(hDlg, IDD_RIGHT, TRUE);

  if(changeData)
    {
    TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);
    int     numSel = (int)GetProp(hDlg, "numSel");

    int*    selList = new int[numSel];

    if (numSel == 0)
      COLUMN::SetAlignment(0, &tblAttrib.defColumn, wParam);
 
    for(int i=0; i<numSel; i++)
      COLUMN::SetAlignment(selList[i], colPtr, wParam);

    GlobalUnlock(hCols);
    if (selList)
      delete selList;
    }
  }

// TABLE_OBJD::ChangeWidthRadioButton //////////////////////////////////////

void TABLE_OBJD::ChangeWidthRadioButton(
  HWND    hDlg,
  WPARAM  wParam)

  {
  char    szBuffer[80];
  int     numSel = (int)GetProp(hDlg, "numSel");
  BOOL    sameWidth = TRUE;
  BOOL    sameCharWidth = TRUE;
  if(numSel<0)
    return;
  if(!hCols && (numSel!=0))
    return;

  TABLE_ATTRIBUTES::SetLastWidthRB(wParam);

  int*    selList;
  TCOLS*  colPtr;

  selList = new int[numSel];
  colPtr = hCols ? (TCOLS*)GlobalLock(hCols) : NULL;
  SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, numSel, (LPARAM)selList);

  //determine if the cells are the same width
  int firstPixel;
  int firstChar;
  
  if (numSel == 0)
    {
    //determine if the cells are the same width
    firstPixel = COLUMN::Pixels(0, &tblAttrib.defColumn);
    firstChar = (int)(100 * COLUMN::Characters(0, &tblAttrib.defColumn) + 0.5);
    if(firstPixel != COLUMN::Pixels(0, &tblAttrib.defColumn))
      sameWidth = FALSE;
    if(firstChar != (int)(100 * COLUMN::Characters(0, &tblAttrib.defColumn) + 0.5))
      sameCharWidth = FALSE;

    }
  else
    {
    //determine if the cells are the same width
    firstPixel = COLUMN::Pixels(selList[0], colPtr);
    firstChar = (int)(100 * COLUMN::Characters(selList[0], colPtr) + 0.5);
    }

  for(int i=1; i<numSel && (sameWidth || sameCharWidth); i++)
    {
    if(firstPixel != COLUMN::Pixels(selList[i], colPtr))
      sameWidth = FALSE;
    if(firstChar != (int)(100 * COLUMN::Characters(selList[i], colPtr) + 0.5))
      sameCharWidth = FALSE;
    }

  // make sure only one of the radio buttons is selected.
  if(wParam != IDD_RBPIXELS)
    {
    ShowWindow(GetDlgItem(hDlg, IDD_PIXELS), SW_HIDE);
    if(IsDlgButtonChecked(hDlg, IDD_RBPIXELS))
      CheckDlgButton(hDlg, IDD_RBPIXELS, FALSE);
    }
  else
    {
    CheckDlgButton(hDlg, IDD_RBPIXELS, TRUE);
    if(sameWidth)
      {
      if (numSel == 0)
        sprintf(szBuffer, "%i", COLUMN::Pixels(0, &tblAttrib.defColumn));
      else
        sprintf(szBuffer, "%i", COLUMN::Pixels(selList[0], colPtr));
      SetDlgItemText(hDlg, IDD_PIXELS, szBuffer);
      }
    else
      SetDlgItemText(hDlg, IDD_PIXELS, NULL);
    ShowWindow(GetDlgItem(hDlg, IDD_PIXELS), SW_SHOW);
    }

  if(wParam != IDD_RBCHARACTERS)
    {
    ShowWindow(GetDlgItem(hDlg, IDD_CHARACTERS), SW_HIDE);
    if(IsDlgButtonChecked(hDlg, IDD_RBCHARACTERS))
      CheckDlgButton(hDlg, IDD_RBCHARACTERS, FALSE);
    }
  else
    {
    CheckDlgButton(hDlg, IDD_RBCHARACTERS, TRUE);
    if(sameCharWidth)
      {
      if (numSel == 0)
        sprintf(szBuffer, "%5.2lf", COLUMN::Characters(0, &tblAttrib.defColumn));
      else
        sprintf(szBuffer, "%5.2lf", COLUMN::Characters(selList[0], colPtr));
      SetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer);
      }
    else
      SetDlgItemText(hDlg, IDD_CHARACTERS, NULL);
    ShowWindow(GetDlgItem(hDlg, IDD_CHARACTERS), SW_SHOW);
    }

  if(wParam != IDD_RBINCHES)
    {
    ShowWindow(GetDlgItem(hDlg, IDD_INCHES), SW_HIDE);
    if(IsDlgButtonChecked(hDlg, IDD_RBINCHES))
      CheckDlgButton(hDlg, IDD_RBINCHES, FALSE);
    }
  else
    {
    CheckDlgButton(hDlg, IDD_RBINCHES, TRUE);
    if(sameWidth)
      {
      if (numSel == 0)
        sprintf(szBuffer, "%5.2lf", COLUMN::Inches(0, &tblAttrib.defColumn));
      else
        sprintf(szBuffer, "%5.2lf", COLUMN::Inches(selList[0], colPtr));
      SetDlgItemText(hDlg, IDD_INCHES, szBuffer);
      }
    else
      SetDlgItemText(hDlg, IDD_INCHES, NULL);
    ShowWindow(GetDlgItem(hDlg, IDD_INCHES), SW_SHOW);
    }

  if(hCols)
    GlobalUnlock(hCols);
  delete selList;
  }

// TABLE_OBJD::Add /////////////////////////////////////////////////////////

void TABLE_OBJD::Add(
  HWND  hDlg)

  {
  HWND  hAvailObjs = GetDlgItem(hDlg, IDD_AVAILOBJS);
  int   numAvailSel = (int)SendMessage(hAvailObjs, LB_GETSELCOUNT, 0, 0L);
  if(numAvailSel<1)
    return;

  // allocate enough space to get the list of objects to add
  int*  selList = new int[numAvailSel];
  if(!selList)
    return;

  OBJECTID  objID;
  int       index;
  int       colNum;
  TCOLS*    colPtr;
  int       first;

  if(SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, 1, (LPARAM)&first)<1L)
    first = (int)SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETCOUNT, 0, 0L)-1;

  SendMessage(hAvailObjs, LB_GETSELITEMS, numAvailSel, (LPARAM)selList);
  for(int i=0; i<numAvailSel; i++)
    {
    objID = (OBJECTID)SendMessage(hAvailObjs, LB_GETITEMDATA, (WPARAM)selList[i], 0L);
    if((colNum = AddColumn(objID, hCols, numCols, first+i,&(tblAttrib.defColumn)))<0)
      break;

    // when we add a string to the column structure, the position should be
    // the same as the list box positioning.
    colPtr = (TCOLS*)GlobalLock(hCols); 
    index = (int)SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_INSERTSTRING, (WPARAM)(first+i), (LPARAM)COLUMN::Title(first+i, colPtr));
    GlobalUnlock(hCols);
    if(colNum!=index)
      {
      SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_DELETESTRING, (WPARAM)(first+i), 0L);
      break;
      }
    }

  delete selList;
  }

// TABLE_OBJD::Remove //////////////////////////////////////////////////////

void TABLE_OBJD::Remove(
  HWND  hDlg)

  {
  HWND  hColObjs = GetDlgItem(hDlg, IDD_COLUMNS);
  int   numColSel = (int)GetProp(hDlg, "numSel");
  if(numColSel<1)
    return;

  // allocate enough space to get the list of objects to delete
  int*  selList = new int[numColSel];
  if(!selList)
    return;
  SendMessage(hColObjs, LB_GETSELITEMS, numColSel, (LPARAM)selList);

  // delete in reverse order to preserve the numbering....
  for(int i=numColSel-1; i>=0; i--)
    {
    COLUMN::DeleteColumn(selList[i], hCols, numCols);
    SendMessage(hColObjs, LB_DELETESTRING, (WPARAM)selList[i], 0L);
    }

  delete selList;

  // update the buttons
  SendMessage(hDlg, WM_COMMAND, IDD_COLUMNS, MAKELONG((UINT)hColObjs, (UINT)LBN_SELCHANGE));
  }

// TABLE_OBJD::Edit ////////////////////////////////////////////////////////

LONG TABLE_OBJD::Edit(
  HWND  hDlg)

  {
  if((int)GetProp(hDlg, "numSel") != 1)
    return FALSE;

  int index;
  SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, 1, (LPARAM)&index);
  return AObjEdit(COLUMN::GetObjectID(index, hCols));
  }

// TABLE_OBJD::ListBoxCommand //////////////////////////////////////////////

BOOL TABLE_OBJD::ListBoxCommand(
  HWND    hDlg,
  WPARAM  wParam,
  LPARAM  lParam)

  {
  switch(wParam)
    {
    case IDD_AVAILOBJS:
      switch(HIWORD(lParam))
        {
        case LBN_SELCHANGE:
          SetAddEnableState(hDlg);
          return TRUE;

        case LBN_DBLCLK:
          Add(hDlg);
          return TRUE;
        }
      break;

    case IDD_COLUMNS:
      switch(HIWORD(lParam))                                      
        {
        case LBN_SELCHANGE:
          {
          int*  sel = NULL;
          int   numSel = (int)SendMessage((HWND)LOWORD(lParam), LB_GETSELCOUNT, 0, 0L);
          if(numSel==0)
            goto afterCheck;

          // decrement numSel if the blank (last item) is selected
          sel = new int[numSel];
          SendMessage((HWND)LOWORD(lParam), LB_GETSELITEMS, numSel, (LPARAM)sel);
          if(sel[numSel-1]==((int)SendMessage((HWND)LOWORD(lParam), LB_GETCOUNT, 0, 0L)-1))
            numSel--;
          delete sel;

          afterCheck:
          SetProp(hDlg, "numSel", (HANDLE)numSel);
          SetColumnAttributes(hDlg);
          SetEnabledState(hDlg);
          return TRUE;
          }

        case LBN_DBLCLK:
          Edit(hDlg);
          return TRUE;
        }
      break;

    default:
      break;
    }

  return FALSE;
  }

// TABLE_OBJD::EditControlCommand //////////////////////////////////////////

BOOL TABLE_OBJD::EditControlCommand(
  HWND    hDlg,
  WPARAM  wParam,
  LPARAM  lParam)

  {
  switch(wParam)
    {
    case IDD_TITLE:
      switch(HIWORD(lParam))
        {
        case EN_KILLFOCUS:
          {
          int numSel = (int)GetProp(hDlg, "numSel");

          if (numSel == 0)
            {
            GetDlgItemText(hDlg, IDD_TITLE, COLUMN::Title(0, &tblAttrib.defColumn), COLUMN_TITLEBUFSIZE);
            }
          else if(numSel!=1)
            break;
          else
            {
            int     index;
            TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);
            SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, 1, (LPARAM)&index);
            GetDlgItemText(hDlg, IDD_TITLE, COLUMN::Title(index, colPtr), COLUMN_TITLEBUFSIZE);
            GlobalUnlock(hCols);
            }
          return TRUE;
          }

        default:
          break;
        }
      break;

    case IDD_PIXELS:
      switch(HIWORD(lParam))
        {
        case EN_KILLFOCUS:
          {
          int     numSel = (int)GetProp(hDlg, "numSel");
          char    szBuffer[20];
          int     numPixels;

          GetDlgItemText(hDlg, IDD_PIXELS, szBuffer, 20);
          numPixels = (int)strtol(szBuffer, NULL, 10);
          if(numPixels==0 && szBuffer[0]!='0')
            {
            // input error -- don't reset to zero!!!!
            ChangeWidthRadioButton(hDlg, IDD_RBPIXELS);
            return TRUE;
            }

          TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);
          int*    selList = new int[numSel];
          SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, numSel, (LPARAM)selList);

          if (numSel == 0)
            {
            COLUMN::SetPixels(0, &tblAttrib.defColumn, numPixels);
            sprintf(szBuffer, "%i", COLUMN::Pixels(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_PIXELS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Characters(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Inches(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_INCHES, szBuffer);

            }
          else {
            for(int i=0; i<numSel; i++)
              COLUMN::SetPixels(selList[i], colPtr, numPixels);

            sprintf(szBuffer, "%i", COLUMN::Pixels(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_PIXELS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Characters(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Inches(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_INCHES, szBuffer);
            }
          GlobalUnlock(hCols);
          delete selList;
          return TRUE;
          }

        default:
          break;
        }
      break;

    case IDD_CHARACTERS:
      switch(HIWORD(lParam))
        {
        case EN_KILLFOCUS:
          {
          int     numSel = (int)GetProp(hDlg, "numSel");
          char    szBuffer[20];
          double  numChars;

          GetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer, 20);
          numChars = strtod(szBuffer, NULL);
          if(numChars==0.)
            {
            int idx=0;
            while(szBuffer[idx] && isspace(szBuffer[idx]))
              idx++;

            if(!(szBuffer[idx]=='0' || szBuffer[idx]=='.'))
              {
              // input error -- don't reset to zero!!!!
              ChangeWidthRadioButton(hDlg, IDD_RBCHARACTERS);
              return TRUE;
              }
            }

          if (numSel == 0)
            {
            COLUMN::SetCharacters(0, &tblAttrib.defColumn, numChars);

            sprintf(szBuffer, "%i", COLUMN::Pixels(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_PIXELS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Characters(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Inches(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_INCHES, szBuffer);
            }
          else
            {
            TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);
            int*    selList = new int[numSel];
            SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, numSel, (LPARAM)selList);

            for(int i=0; i<numSel; i++)
              COLUMN::SetCharacters(selList[i], colPtr, numChars);

            sprintf(szBuffer, "%i", COLUMN::Pixels(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_PIXELS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Characters(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Inches(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_INCHES, szBuffer);
            GlobalUnlock(hCols);
            delete selList;

            }
          return TRUE;
          }

        default:
          break;
        }
      break;

    case IDD_INCHES:
      switch(HIWORD(lParam))
        {
        case EN_KILLFOCUS:
          {
          int     numSel = (int)GetProp(hDlg, "numSel");
          char    szBuffer[20];
          double  numInches;

          GetDlgItemText(hDlg, IDD_INCHES, szBuffer, 20);
          numInches = (double)strtod(szBuffer, NULL);
          if(numInches==0.)
            {
            int idx=0;
            while(szBuffer[idx] && isspace(szBuffer[idx]))
              idx++;

            if(!(szBuffer[idx]=='0' || szBuffer[idx]=='.'))
              {
              // input error -- don't reset to zero!!!!
              ChangeWidthRadioButton(hDlg, IDD_RBINCHES);
              return TRUE;
              }
            }

          if (numSel == 0)
            {
            COLUMN::SetInches(0, &tblAttrib.defColumn, numInches);

            sprintf(szBuffer, "%i", COLUMN::Pixels(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_PIXELS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Characters(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Inches(0, &tblAttrib.defColumn));
            SetDlgItemText(hDlg, IDD_INCHES, szBuffer);
            }
          else {
            TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);
            int*    selList = new int[numSel];
            SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, numSel, (LPARAM)selList);

            for(int i=0; i<numSel; i++)
              COLUMN::SetInches(selList[i], colPtr, numInches);

            sprintf(szBuffer, "%i", COLUMN::Pixels(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_PIXELS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Characters(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_CHARACTERS, szBuffer);
            sprintf(szBuffer, "%5.2lf", COLUMN::Inches(selList[0], colPtr));
            SetDlgItemText(hDlg, IDD_INCHES, szBuffer);

            GlobalUnlock(hCols);
            delete selList;
            }
          return TRUE;
          }

        default:
          break;
        }
      break;

    default:
      break;
    }

  return FALSE;
  }

// TABLE_OBJD::SetColumnAttributes /////////////////////////////////////////

void TABLE_OBJD::SetColumnAttributes(
  HWND  hDlg)

  {
  int numSel = (int)GetProp(hDlg, "numSel");
  if(numSel<0)
    return;

  // Setting the Default attributes
  if (numSel == 0)
    {
    // set the title
    SetDlgItemText(hDlg, IDD_TITLE, COLUMN::Title(0, &tblAttrib.defColumn));

    // set the alignment
    ChangeTitleAlignmentRB(hDlg, (WPARAM)COLUMN::TitleAlignment(0, &tblAttrib.defColumn)+500, FALSE);
    ChangeAlignmentRadioButton(hDlg, COLUMN::Alignment(0, &tblAttrib.defColumn), FALSE);

    // set the editable check box
    CheckDlgButton(hDlg, IDD_EDITABLE, (COLUMN::Editable(0, &tblAttrib.defColumn) ? TRUE : FALSE));

    // set the width field
    ChangeWidthRadioButton(hDlg, TABLE_ATTRIBUTES::LastWidthRB());

    // set the font properties
    SetProp(hDlg, "sameTitleFont", (HANDLE)TRUE);
    SetProp(hDlg, "sameCellFont", (HANDLE)TRUE);
    SetProp(hDlg, "sameTitleFG", (HANDLE)TRUE);
    SetProp(hDlg, "sameCellFG", (HANDLE)TRUE);
    SetProp(hDlg, "sameTitleBG", (HANDLE)TRUE);
    SetProp(hDlg, "sameCellBG", (HANDLE)TRUE);

    return;
    }
  // get the list of columns
  int*  selList = new int[numSel];
  if(!selList)
    return;
  SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, numSel, (LPARAM)selList);

  TCOLS*  colPtr = (TCOLS*)GlobalLock(hCols);

  if(numSel>1)
    {
    int   i;
    BOOL  same;

    // set the title
    SetDlgItemText(hDlg, IDD_TITLE, NULL);

    // set the title alignment radio buttons
    same = TRUE;
    char  firstTitleAlign = COLUMN::TitleAlignment(selList[0], colPtr);
    for(i=1; i<numSel; i++)
      {
      if(firstTitleAlign != COLUMN::TitleAlignment(selList[i], colPtr))
        {
        same = FALSE;
        break;
        }
      }
    if(same)
      ChangeTitleAlignmentRB(hDlg, (WPARAM)COLUMN::TitleAlignment(selList[0], colPtr)+500, FALSE);
    else
      ChangeTitleAlignmentRB(hDlg, NULL, FALSE);

    // set the cell alignment radio buttons
    same = TRUE;
    int firstAlignment = COLUMN::Alignment(selList[0], colPtr);
    for(i=1; i<numSel; i++)
      {
      if(firstAlignment != COLUMN::Alignment(selList[i], colPtr))
        {
        same = FALSE;
        break;
        }
      }
    if(same)
      ChangeAlignmentRadioButton(hDlg, COLUMN::Alignment(selList[0], colPtr), FALSE);
    else
      ChangeAlignmentRadioButton(hDlg, NULL, FALSE);

    // set the editable check box
    same = TRUE;
    BOOL  firstEditable = COLUMN::Editable(selList[0], colPtr);
    for(i=1; i<numSel; i++)
      {
      if(firstEditable != COLUMN::Editable(selList[i], colPtr))
        {
        same = FALSE;
        break;
        }
      }
    if(same)
      CheckDlgButton(hDlg, IDD_EDITABLE, (firstEditable ? TRUE : FALSE));
    else
      // check the third state
      CheckDlgButton(hDlg, IDD_EDITABLE, 2);

    // set the width field
    same = TRUE;
    int firstPixels = COLUMN::Pixels(selList[0], colPtr);
    for(i=1; i<numSel; i++)
      {
      if(firstPixels != COLUMN::Pixels(selList[i], colPtr))
        {
        same = FALSE;
        break;
        }
      }
    ChangeWidthRadioButton(hDlg, TABLE_ATTRIBUTES::LastWidthRB());

    // check to see if all the title fonts are the same
    same = TRUE;
    LOGFONT*  firstLF = COLUMN::TitleFont(selList[0], colPtr, 0, 0);
    LOGFONT*  currLF;
    for(i=1; i<numSel; i++)
      {
      if(firstLF
          &&
         ((currLF = COLUMN::TitleFont(selList[i], colPtr, 0, 0))!=NULL))
        {
        if(lfcmp(firstLF, currLF))
          {
          same = FALSE;
          break;
          }
        }
      else
        {
        same = FALSE;
        break;
        }
      }
    SetProp(hDlg, "sameTitleFont", (HANDLE)same);

    // check to see if all the title foreground colors are the same
    same = TRUE;
    COLORREF  firstColor = COLUMN::TitleFGColor(selList[0], colPtr);
    COLORREF  currColor;  
    for(i=1; i<numSel; i++)
      {
      currColor = COLUMN::TitleFGColor(selList[i], colPtr);
      if(currColor != firstColor)
        {
        same = FALSE;
        break;
        }
      }
    SetProp(hDlg, "sameTitleFG", (HANDLE)same);

    // check to see if all the title background colors are the same
    same = TRUE;
    firstColor = COLUMN::TitleBGColor(selList[0], colPtr);
    for(i=1; i<numSel; i++)
      {
      currColor = COLUMN::TitleBGColor(selList[i], colPtr);
      if(currColor != firstColor)
        {
        same = FALSE;
        break;
        }
      }
    SetProp(hDlg, "sameTitleBG", (HANDLE)same);

    // check to see if all the cell fonts are the same
    same = TRUE;
    firstLF = COLUMN::CellFont(selList[0], colPtr, 0, 0);
    for(i=1; i<numSel; i++)
      {
      if(firstLF
          &&
         ((currLF = COLUMN::CellFont(selList[i], colPtr, 0, 0))!=NULL))
        {
        if(lfcmp(firstLF, currLF))
          {
          same = FALSE;
          break;
          }
        }
      else
        {
        same = FALSE;
        break;
        }
      }
    SetProp(hDlg, "sameCellFont", (HANDLE)same);

    // check to see if all the cell foreground colors are the same
    same = TRUE;
    firstColor = COLUMN::CellFGColor(selList[0], colPtr);
    for(i=1; i<numSel; i++)
      {
      currColor = COLUMN::CellFGColor(selList[i], colPtr);
      if(currColor != firstColor)
        {
        same = FALSE;
        break;
        }
      }
    SetProp(hDlg, "sameCellFG", (HANDLE)same);

    // check to see if all the cell background colors are the same
    same = TRUE;
    firstColor = COLUMN::CellBGColor(selList[0], colPtr);
    for(i=1; i<numSel; i++)
      {
      currColor = COLUMN::CellBGColor(selList[i], colPtr);
      if(currColor != firstColor)
        {
        same = FALSE;
        break;
        }
      }
    SetProp(hDlg, "sameCellBG", (HANDLE)same);
    }
  else
    {
    // set the title
    SetDlgItemText(hDlg, IDD_TITLE, COLUMN::Title(selList[0], colPtr));

    // set the alignment
    ChangeTitleAlignmentRB(hDlg, (WPARAM)COLUMN::TitleAlignment(selList[0], colPtr)+500, FALSE);
    ChangeAlignmentRadioButton(hDlg, COLUMN::Alignment(selList[0], colPtr), FALSE);

    // set the editable check box
    CheckDlgButton(hDlg, IDD_EDITABLE, (COLUMN::Editable(selList[0], colPtr) ? TRUE : FALSE));

    // set the width field
    ChangeWidthRadioButton(hDlg, TABLE_ATTRIBUTES::LastWidthRB());

    // set the font properties
    SetProp(hDlg, "sameTitleFont", (HANDLE)TRUE);
    SetProp(hDlg, "sameCellFont", (HANDLE)TRUE);
    SetProp(hDlg, "sameTitleFG", (HANDLE)TRUE);
    SetProp(hDlg, "sameCellFG", (HANDLE)TRUE);
    SetProp(hDlg, "sameTitleBG", (HANDLE)TRUE);
    SetProp(hDlg, "sameCellBG", (HANDLE)TRUE);
    }

  GlobalUnlock(hCols);
  delete selList;
  }

// TABLE_OBJD::FontDialog //////////////////////////////////////////////////

BOOL TABLE_OBJD::FontDialog(
  HWND    hDlg,
  WPARAM  wParam)

  {
  int       numSel = (int)GetProp(hDlg, "numSel");
  if(numSel<0)
    return FALSE;
  int*      selList = new int[numSel];

  SendDlgItemMessage(hDlg, IDD_COLUMNS, LB_GETSELITEMS, numSel, (LPARAM)selList);

  LOGFONT   lf;
  COLORREF  fgColor;
  COLORREF  bgColor;
  short     deciPointSize;
  DWORD     flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST;
  TCOLS*    colPtr = (TCOLS*)GlobalLock(hCols);

  switch(wParam)
    {
    case IDD_TITLEFONT:
      {
      BOOL  sameTitleFont = (BOOL)GetProp(hDlg, "sameTitleFont");
      BOOL  sameTitleFG = (BOOL)GetProp(hDlg, "sameTitleFG");
      BOOL  sameTitleBG = (BOOL)GetProp(hDlg, "sameTitleBG");

      // initialize the logfont structure
      if (numSel == 0)
        lf = *COLUMN::TitleFont(0, &tblAttrib.defColumn, VGA_RESOLUTION, 0);
      else if (sameTitleFont)
        {
        HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
        if (numSel == 0)
          lf = *COLUMN::TitleFont(0, &tblAttrib.defColumn, VGA_RESOLUTION, 0);
        else 
          lf = *COLUMN::TitleFont(selList[0], colPtr, VGA_RESOLUTION, 0);
        GetProfileString("FontSubstitutes", lf.lfFaceName, lf.lfFaceName, lf.lfFaceName, LF_FACESIZE);
        DeleteDC(hDC);
        }
      else {
        memset(&lf, 0, sizeof(LOGFONT));
        }

      // initialize the colors
      if (numSel == 0)
        fgColor = COLUMN::TitleFGColor(0, &tblAttrib.defColumn);
      else if(sameTitleFG)
        {
        fgColor = COLUMN::TitleFGColor(selList[0], colPtr);
        }
      else 
        fgColor = WINDOW_FOREGROUND;

      if (numSel == 0)
        bgColor = COLUMN::TitleBGColor(0, &tblAttrib.defColumn);
      else if(sameTitleBG)
        {
        bgColor = COLUMN::TitleBGColor(selList[0], colPtr);
        }
      else
        bgColor = WINDOW_BACKGROUND;


      // call the font dialog
      if ( AUtlDlgEditFont(hDlg, "AaBbCcXxYyZz", "Title Font", &lf, &deciPointSize, TRUE, &fgColor, &bgColor, TRUE, flags, FALSE, NULL))
        {
        if (numSel == 0)
          {
          COLUMN::SetTitleFont(0, &tblAttrib.defColumn, &lf, deciPointSize);
          COLUMN::SetTitleFGColor(0, &tblAttrib.defColumn, fgColor);
          COLUMN::SetTitleBGColor(0, &tblAttrib.defColumn, bgColor);
          }
          
        // update the title font for all selected members
        for(int i=0; i<numSel; i++)
          {
          COLUMN::SetTitleFont(selList[i], colPtr, &lf, deciPointSize);
          COLUMN::SetTitleFGColor(selList[i], colPtr, fgColor);
          COLUMN::SetTitleBGColor(selList[i], colPtr, bgColor);
          }

        SetProp(hDlg, "sameTitleFont", (HANDLE)TRUE);
        SetProp(hDlg, "sameTitleFG", (HANDLE)TRUE);
        SetProp(hDlg, "sameTitleBG", (HANDLE)TRUE);
        }
      }
      break;

    case IDD_CELLFONT:
      {
      BOOL  sameCellFont = (BOOL)GetProp(hDlg, "sameCellFont");
      BOOL  sameCellFG = (BOOL)GetProp(hDlg, "sameCellFG");
      BOOL  sameCellBG = (BOOL)GetProp(hDlg, "sameCellBG");

      // initialize the logfont structure
      if (numSel == 0)
        lf = *COLUMN::CellFont(0, &tblAttrib.defColumn, VGA_RESOLUTION, 0);
      else if(sameCellFont)
        {
        HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
        lf = *COLUMN::CellFont(selList[0], colPtr, VGA_RESOLUTION, 0);
        GetProfileString("FontSubstitutes", lf.lfFaceName, lf.lfFaceName, lf.lfFaceName, LF_FACESIZE);
        DeleteDC(hDC);
        }
      else
        memset(&lf, 0, sizeof(LOGFONT));

      // initialize the colors
      if (numSel == 0)
        fgColor = COLUMN::CellFGColor(0, &tblAttrib.defColumn);
      else if(sameCellFG)
        {
        fgColor = COLUMN::CellFGColor(selList[0], colPtr);
        }
      else
        fgColor = WINDOW_FOREGROUND;


      if (numSel == 0)
        bgColor = COLUMN::CellBGColor(0, &tblAttrib.defColumn);
      else if (sameCellBG)
        {
        bgColor = COLUMN::CellBGColor(selList[0], colPtr);
        }
      else
        bgColor = WINDOW_BACKGROUND;

      if(AUtlDlgEditFont(hDlg, "AaBbCcXxYyZz", "Cell Font", &lf, &deciPointSize, TRUE, &fgColor, &bgColor, TRUE, flags, FALSE, NULL))
        {
        // update the cell font for Default Column April 29, 1994
        if (numSel == 0)
          {
          COLUMN::SetCellFont(0, &tblAttrib.defColumn, &lf, deciPointSize);
          COLUMN::SetCellFGColor(0, &tblAttrib.defColumn, fgColor);
          COLUMN::SetCellBGColor(0, &tblAttrib.defColumn, bgColor);
          }

        // update the cell font for all selected members
        for(int i=0; i<numSel; i++)
          {
          COLUMN::SetCellFont(selList[i], colPtr, &lf, deciPointSize);
          COLUMN::SetCellFGColor(selList[i], colPtr, fgColor);
          COLUMN::SetCellBGColor(selList[i], colPtr, bgColor);
          }

        SetProp(hDlg, "sameCellFont", (HANDLE)TRUE);
        SetProp(hDlg, "sameCellFG", (HANDLE)TRUE);
        SetProp(hDlg, "sameCellBG", (HANDLE)TRUE);

        // if currently looking at character width -- adjust the width as necessary
        if(IsDlgButtonChecked(hDlg, IDD_RBCHARACTERS))
          EditControlCommand(hDlg, IDD_CHARACTERS, MAKELPARAM((WORD)GetDlgItem(hDlg, IDD_CHARACTERS), EN_KILLFOCUS));
        }
      }
      break;

    default:
      delete selList;
      GlobalUnlock(hCols);
      return FALSE;
    }

  delete selList;
  GlobalUnlock(hCols);
  return TRUE;
  }

// TABLE_OBJD::GetColumnObjectID ///////////////////////////////////////////

OBJECTID TABLE_OBJD::GetColumnObjectID(
  int index)

  {
  TCOLS*    colPtr = (TCOLS*)GlobalLock(hCols);
  OBJECTID  colObj = COLUMN::ObjectID(index, colPtr);
  GlobalUnlock(hCols);
  return colObj;
  }

// TABLE_OBJD::SetColumnObjectID ///////////////////////////////////////////

void TABLE_OBJD::SetColumnObjectID(
  int       index,
  OBJECTID  colObj)

  {
  TCOLS*    colPtr = (TCOLS*)GlobalLock(hCols);
  COLUMN::SetObjectID(index, colPtr, colObj);
  GlobalUnlock(hCols);
  }

// Tabl_AttribProc /////////////////////////////////////////////////////////

BOOL far pascal _export Tabl_AttribProc(
  HWND    hDlg,
  UINT    message,
  WPARAM  wParam,
  LPARAM  lParam)

  {
  TABLE_OBJD* tableInfo;

  switch(message)
    {
    case WM_INITDIALOG:
      {
      // store the data in the window extra bytes
      tableInfo = (TABLE_OBJD*)lParam;
      SetWindowLong(hDlg, G.cbByteOffset, (long)tableInfo);

      // write the object name to the title bar
      char  objName[255];
      AObjGetName(tableInfo->GetObject(), objName, 255);
      SetWindowText(hDlg, objName);

      // initialize the controls
      tableInfo->InitAttributeControls(hDlg);

      AUtlSetTitleFont(OTYPE_TABLE, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
      break;
      }

    case WM_PAINT:
      return 0; //serdlgDrawTitle(GetDlgItem(hDlg, IDD_OBJECT), "Table Options");

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          tableInfo = (TABLE_OBJD*)GetWindowLong(hDlg, G.cbByteOffset);
          tableInfo->UpdateTableAttributes(hDlg);
          EndDialog(hDlg, TRUE);
          return TRUE;

        case IDCANCEL:
          EndDialog(hDlg, FALSE);
          return TRUE;

        case IDHELP:
          WinHelp(hDlg, SERIUS_HELPFILE, HELP_CONTEXT, HELPID_DLG_TABLEOPTIONS);
          return TRUE;

        case IDD_AUTOFILL:
          tableInfo = (TABLE_OBJD*)GetWindowLong(hDlg, G.cbByteOffset);
          tableInfo->EnableComboBoxes(hDlg);
        case IDD_CANACTIVATE:
          EnableWindow(GetDlgItem(hDlg, IDD_SHOWCELLSEL), !IsDlgButtonChecked(hDlg, IDD_AUTOFILL) && IsDlgButtonChecked(hDlg, IDD_CANACTIVATE));
          EnableWindow(GetDlgItem(hDlg, IDD_CLAIRVOYANTSEL), !IsDlgButtonChecked(hDlg, IDD_AUTOFILL) && IsDlgButtonChecked(hDlg, IDD_CANACTIVATE));
          return TRUE;


        case IDD_ADHOC:
          tableInfo = (TABLE_OBJD*)GetWindowLong(hDlg, G.cbByteOffset);
          tableInfo->FillDataSource(hDlg);
          tableInfo->EnableComboBoxes(hDlg);

          return TRUE;
        
        case IDD_DATASOURCE:
          if(HIWORD(lParam)==CBN_SELCHANGE)
            {
            tableInfo = (TABLE_OBJD*)GetWindowLong(hDlg, G.cbByteOffset);
            tableInfo->FillIndexedField(hDlg);
            tableInfo->EnableComboBoxes(hDlg);
            return TRUE;
            }
          else
            return FALSE;

        case IDD_COLUMNLINES:
        case IDD_SHOWTITLES:
          EnableWindow(GetDlgItem(hDlg, IDD_CANRESIZE), IsDlgButtonChecked(hDlg, IDD_SHOWTITLES) && IsDlgButtonChecked(hDlg, IDD_COLUMNLINES));
          EnableWindow(GetDlgItem(hDlg, IDD_SHOWCOLSEL), IsDlgButtonChecked(hDlg, IDD_SHOWTITLES));
          return TRUE;

        default:
          break;
        }
      break;

    default:
      break;
    }

  return FALSE;
  }

// TABLE_OBJD::InitAttributeControls ///////////////////////////////////////

void TABLE_OBJD::InitAttributeControls(
  HWND  hDlg)

  {
  // set the fixed columns number
  char  szBuffer[80];
  wsprintf(szBuffer, "%i", NumFixedColumns());
  SetDlgItemText(hDlg, IDD_FIXFIRST, szBuffer);

  // set the number of buffered rows
  wsprintf(szBuffer, "%i", MaxStore());
  SetDlgItemText(hDlg, IDD_NUMBUFFERED, szBuffer);

  // set the checks for the styles
  CheckDlgButton(hDlg, IDD_AUTOFILL, AutoFill());
  CheckDlgButton(hDlg, IDD_ADHOC, AdHoc());
  CheckDlgButton(hDlg, IDD_MULTISELECT, MultiSelect());
  CheckDlgButton(hDlg, IDD_SHOWTITLES, ShowTitles());
  CheckDlgButton(hDlg, IDD_CANACTIVATE, CanActivate());
  CheckDlgButton(hDlg, IDD_ROWLINES, RowLines());
  CheckDlgButton(hDlg, IDD_COLUMNLINES, ColumnLines());
  CheckDlgButton(hDlg, IDD_VSCROLL, VertScroll());
  CheckDlgButton(hDlg, IDD_HSCROLL, HorzScroll());
  CheckDlgButton(hDlg, IDD_CANRESIZE, CanResize());
  CheckDlgButton(hDlg, IDD_SHOWCOLSEL, ShowSelection());
  CheckDlgButton(hDlg, IDD_SHOWCELLSEL, ShowActiveCell());
  CheckDlgButton(hDlg, IDD_CLAIRVOYANTSEL, Clairvoyant());

  EnableWindow(GetDlgItem(hDlg, IDD_CANRESIZE), ColumnLines() && ShowTitles());
  EnableWindow(GetDlgItem(hDlg, IDD_SHOWCOLSEL), ShowTitles());
  EnableWindow(GetDlgItem(hDlg, IDD_SHOWCELLSEL), !AutoFill() && CanActivate());
  EnableWindow(GetDlgItem(hDlg, IDD_CLAIRVOYANTSEL), !AutoFill() && CanActivate());

  FillDataSource(hDlg);
  FillIndexedField(hDlg);
  FillRowIndex(hDlg);
  EnableComboBoxes(hDlg);
  }

// TABLE_OBJD::FillDataSource //////////////////////////////////////////////

void TABLE_OBJD::FillDataSource(
  HWND  hDlg)

  {
  char      szBuffer[80];
  int       index;
  long      theGetKey = AOBJ_GETFIRSTKEY;
  long      cookie = AOBJ_GETFIRSTKEY;
  OBJECTID  aObject;
  TYPEID    aType;
  BOOL      setSelection = FALSE;
  BOOL      bAdHoc   = IsDlgButtonChecked(hDlg, IDD_ADHOC);

  SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_RESETCONTENT, 0, (LPARAM)0L);
  index = (int)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_ADDSTRING, 0, (LPARAM)"(None)");
  SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_SETITEMDATA, index, NULL);
  while((aType = ATypeGetNext(&cookie))!=NULL)
    {
    // if Adhoc is checked than only fill in the adhoc data sources
    if (bAdHoc)
      {
      if(!AEvtCheckForMethod(aType, AEVENT_DVIEWGETDATA))
        continue;
      }
    else { // else fill the dataSource list w/ all autofill sources
      if(!AEvtCheckForMethod(aType, autoTableReadRecords))
        continue;
      }

    theGetKey = AOBJ_GETFIRSTKEY;
    while((aObject = AObjGetNextObject(aType, &theGetKey))!=NULL)
      {
      AObjGetName(aObject, szBuffer, 80);
      index = (int)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
      SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_SETITEMDATA, index, (LPARAM)aObject);

      if(aObject==DataSource())
        {
        SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_SETCURSEL, (WPARAM)index, 0L);
        setSelection = TRUE;
        }
      }
    }
  if(!setSelection)
    SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_SETCURSEL, 0, 0L);
  }

// TABLE_OBJD::FillIndexedField ////////////////////////////////////////////

void TABLE_OBJD::FillIndexedField(
  HWND  hDlg)

  {
  char      szBuffer[80];
  OBJECTID  aObject;
  BOOL      setSelection = FALSE;

  int       index = (int)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_GETCURSEL, 0, 0L);
  aObject = (OBJECTID)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_GETITEMDATA, index, 0L);

  // clear any existing items
  SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_RESETCONTENT, 0, 0L);

  if(aObject)
    {
    // add the "(None)" field
    index = (int)SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_ADDSTRING, 0, (LPARAM)"(None)");
    SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_SETITEMDATA, index, NULL);

    MEMBLOCKID  hFields = AMemAllocate(0L);
    AEvtCallMethod(theObject, aObject, NULL, autoTableGetIndexableFields, (LONG)hFields);
    int         numFields = (int)AMemGetSize(hFields)/sizeof(OBJECTID);
    OBJECTID*   fieldObjs = (OBJECTID*)AMemLock(hFields);
    if(fieldObjs)
      {
      for(int i=0; i<numFields; i++)
        {
        AObjGetName(fieldObjs[i], szBuffer, 80);
        index = (int)SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_ADDSTRING, 0, (LPARAM)szBuffer);
        SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_SETITEMDATA, index, (LPARAM)fieldObjs[i]);

        if(fieldObjs[i]==IndexedField())
          {
          SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_SETCURSEL, (WPARAM)index, 0L);
          setSelection = TRUE;
          }
        }
      AMemUnlock(hFields);

      }
    AMemFree(hFields);

    // make sure a selection is made
    if(!setSelection)
      SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_SETCURSEL, 0, 0L);
    }
  }

// TABLE_OBJD::FillRowIndex ////////////////////////////////////////////////

void TABLE_OBJD::FillRowIndex(
  HWND  hDlg)

  {
  char      szBuffer[80];
  OBJECTID  aObject;
  long      theGetKey = AOBJ_GETFIRSTKEY;
  BOOL      setSelection = FALSE;

  int       index = (int)SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_ADDSTRING, 0, (LPARAM)"(None)");
  SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_SETITEMDATA, index, NULL);
  while((aObject = AObjGetNextObject(OTYPE_NUMBER, &theGetKey))!=NULL)
    {
    AObjGetName(aObject, szBuffer, 80);
    index = (int)SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_ADDSTRING, 0, (LPARAM)szBuffer);
    SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_SETITEMDATA, index, (LPARAM)aObject);

    if(aObject==RowIndexObject())
      {
      SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_SETCURSEL, (WPARAM)index, 0L);
      setSelection = TRUE;
      }
    }
  if(!setSelection)
    SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_SETCURSEL, 0, 0L);
  }

// TABLE_OBJD::EnableComboBoxes ////////////////////////////////////////////

void TABLE_OBJD::EnableComboBoxes(
  HWND  hDlg)

  {
  int       index = (int)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_GETCURSEL, 0, 0L);
  OBJECTID  aObject = (OBJECTID)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_GETITEMDATA, index, 0L);
  BOOL      autoFill = IsDlgButtonChecked(hDlg, IDD_AUTOFILL);
  BOOL      bAdHoc   = IsDlgButtonChecked(hDlg, IDD_ADHOC);

  // If the Autofill is unchecked, always uncheck ADhoc box
  if (autoFill == FALSE)
    CheckDlgButton(hDlg, IDD_ADHOC, FALSE);

  EnableWindow(GetDlgItem(hDlg, IDD_INDEXEDFIELD), (aObject && autoFill && !bAdHoc) ? TRUE : FALSE);
  EnableWindow(GetDlgItem(hDlg, IDD_INDEXEDFIELDSTATIC), (aObject && autoFill && !bAdHoc) ? TRUE : FALSE);
  EnableWindow(GetDlgItem(hDlg, IDD_DATASOURCESTATIC), IsDlgButtonChecked(hDlg, IDD_AUTOFILL));
  EnableWindow(GetDlgItem(hDlg, IDD_DATASOURCE), IsDlgButtonChecked(hDlg, IDD_AUTOFILL));
  EnableWindow(GetDlgItem(hDlg, IDD_ROWINDEXSTATIC), autoFill);
  EnableWindow(GetDlgItem(hDlg, IDD_ROWINDEX), autoFill);
  EnableWindow(GetDlgItem(hDlg, IDD_ADHOC), autoFill);

  // number of buffered auto-fill rows
  EnableWindow(GetDlgItem(hDlg, IDD_NUMBUFFEREDSTATIC), autoFill);
  EnableWindow(GetDlgItem(hDlg, IDD_NUMBUFFERED), autoFill);
  }

// TABLE_OBJD::UpdateTableAttributes ///////////////////////////////////////

void TABLE_OBJD::UpdateTableAttributes(
  HWND  hDlg)

  {
  // set from the edit controls
  char  szBuffer[20];
  GetDlgItemText(hDlg, IDD_FIXFIRST, szBuffer, 19);
  SetNumFixedColumns((int)strtol(szBuffer, NULL, 0));

  GetDlgItemText(hDlg, IDD_NUMBUFFERED, szBuffer, 19);
  SetMaxStore((int)strtol(szBuffer, NULL, 0));

  // set from the check boxes
  SetAutoFill(IsDlgButtonChecked(hDlg, IDD_AUTOFILL));
  SetAdHoc(IsDlgButtonChecked(hDlg, IDD_ADHOC));
  SetMultiSelect(IsDlgButtonChecked(hDlg, IDD_MULTISELECT));
  SetShowTitles(IsDlgButtonChecked(hDlg, IDD_SHOWTITLES));
  SetCanActivate(IsDlgButtonChecked(hDlg, IDD_CANACTIVATE));
  SetRowLines(IsDlgButtonChecked(hDlg, IDD_ROWLINES));
  SetColumnLines(IsDlgButtonChecked(hDlg, IDD_COLUMNLINES));
  SetVertScroll(IsDlgButtonChecked(hDlg, IDD_VSCROLL));
  SetHorzScroll(IsDlgButtonChecked(hDlg, IDD_HSCROLL));
  SetCanResize(IsDlgButtonChecked(hDlg, IDD_CANRESIZE));
  SetShowSelection(IsDlgButtonChecked(hDlg, IDD_SHOWCOLSEL));
  SetShowActiveCell(IsDlgButtonChecked(hDlg, IDD_SHOWCELLSEL));
  SetClairvoyant(IsDlgButtonChecked(hDlg, IDD_CLAIRVOYANTSEL));

  // set data source object from the combo box
  int index;
  index = (int)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_GETCURSEL, 0, 0L);
  if(index==CB_ERR)
    SetDataSource(NULL);
  else
    SetDataSource((OBJECTID)SendDlgItemMessage(hDlg, IDD_DATASOURCE, CB_GETITEMDATA, (WPARAM)index, 0L));

  // set indexed field connection from the combo box
  index = (int)SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_GETCURSEL, 0, 0L);
  if(index==CB_ERR)
    SetIndexedField(NULL);
  else
    SetIndexedField((OBJECTID)SendDlgItemMessage(hDlg, IDD_INDEXEDFIELD, CB_GETITEMDATA, (WPARAM)index, 0L));

  // set row index connection from the combo box
  index = (int)SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_GETCURSEL, 0, 0L);
  if(index==CB_ERR)
    SetRowIndexObject(NULL);
  else
    SetRowIndexObject((OBJECTID)SendDlgItemMessage(hDlg, IDD_ROWINDEX, CB_GETITEMDATA, (WPARAM)index, 0L));
  }

// TestTableWndProc ////////////////////////////////////////////////////////
//
//long far pascal _export TestTableWndProc(
//  HWND    hWnd,
//  UINT    message,
//  WPARAM  wParam,
//  LPARAM  lParam)
//
//  {
//  }
//
////////////////////////////////////////////////////////////////////////////
