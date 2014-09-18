///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// admin.cpp
// Menu Admininstration Routines
// Text Builder Object
//
//
//
//
//
///////////////////////////////////////////////////////////////

#include "tf_incld.h"
#include "heap.h"


////////////////////////////////////////////////////////////////
//
// FUNCTION:  ::operator new and ::operator delete
//
// DESC:      Replaces the system-supplied new and delete operators.
//
////////////////////////////////////////////////////////////////

void*   operator new(
  size_t    s
  )
  {
  return MemAlloc(s);
  }

void    operator delete(
  void*     p
  )
  {
  MemFree(p);
  }
////////////////////////////////////////////////////////////////////////////

GLOBAL  vG;   // Global Container Class
extern int _WinAllocFlag;

/*
extern "C"
{
void CopyDataPtr (OBJECTID dstObj, OBJECTID srcObj, long index );
LONG TFac_obj(OBJECTID  theObject, pAObjMessage theSystem);
  LONG FAR PASCAL _export Administration(OBJECTID theObject, pAObjMessage systemPtr);
  LONG FAR PASCAL _export GetSerVersion ( TYPEID theType );
}
*/

///////////////////////////////////////////////////////////////
// LibMain:
//
///////////////////////////////////////////////////////////////

int FAR PASCAL LibMain (
  HINSTANCE hInst,
  WORD /*wDataSeg*/,
  WORD wHeapSize,
  LPSTR /*lpszCmdLine*/ )
{

  _WinAllocFlag = GMEM_SHARE;
  vG.hInstance = hInst;
  if (wHeapSize > 0)
    UnlockData (0);
  vG.cbOffset = DWL_USER;
  return 1;
}       // int FAR PASCAL LibMain (...


///////////////////////////////////////////////////////////////
// Administration:
//
///////////////////////////////////////////////////////////////

LONG FAR PASCAL _export Administration(
  OBJECTID      theObject,
  pAObjMessage  systemPtr)
{
  TEXTFACT* tf;
  LONG      result;

  switch (systemPtr->message1)
  {
    case AOBJ_CREATED:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      if (!tf) // Run time info not stored to disk, so if tf = 0, then
               //   TEXTFACT object not created yet.
        {
        tf = (TEXTFACT*) new TEXTFACT(theObject);
        AObjSetRuntimeInfo(theObject, (long)tf);
        }
      result = A_OK;
      break;

    case AOBJ_AWAKENED:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      if (!tf)
        {
        tf = (TEXTFACT*) new TEXTFACT(theObject);
        AObjSetRuntimeInfo(theObject, (long)tf);
        }
      result = A_CONTINUE;
      break;

    case AOBJ_DESTROYED:
    case AOBJ_ASLEEP:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      delete tf;
      break;

    case AOBJ_ASSIGNOBJECT:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      result = (tf->DoAssign(theObject, systemPtr));
      break;

    case AOBJ_EXPORTOBJECT:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      result = (LONG)tf->ExportObject(systemPtr);
      break;

    case AOBJ_IMPORTOBJECT:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      result = (LONG)tf->ImportObject(systemPtr);
      break;

    case AOBJ_VALIDATEOBJECTIDS:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      if (!tf)
        {
        tf = (TEXTFACT*) new TEXTFACT(theObject);
        AObjSetRuntimeInfo(theObject, (long)tf);
        }
      tf->Validate(systemPtr);
      break;

    case AOBJ_READ:
      if(GetAppVersion(theObject)!= VERSION_OBJECT)
      {
        systemPtr->message4 = A_ERROR;
        AObjReportError(theObject, AOBJ_READ, A_ERROR,
                        "Text Builder\tser_tfct.dll",
                        (long)GetAppVersion(theObject));

        result = A_ERROR;
      }
      else {
        systemPtr->message4 = A_OK;
        result = A_OK;
      }
      break;

    case AOBJ_EDITOBJECT:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      tf->CheckObjectID();
      if(DialogBoxParam(vG.hInstance, "TFDIALOG", GetActiveWindow(), (DLGPROC)TEXTFACT::TfDialog, (LPARAM)tf))
        result = A_MODIFIED;
      else
        result = A_OK;
      break;

    case AOBJ_BUILDSIGNALS:
      MEMBLOCKID         signalBlock;
      pAObjSignalRecord  signalPtr;

      signalBlock = (MEMBLOCKID)systemPtr->message2;
      AMemSetSize(signalBlock, NUM_TEXTFACT_SIGNALS * sizeof(AObjSignalRecord));
      signalPtr = (pAObjSignalRecord)AMemLock(signalBlock);
      signalPtr[0].theID = SIGNAL_EXPUPDATE ;
      lstrcpy(signalPtr[0].theName, "Updated");
      signalPtr[1].theID = SIGNAL_VARCHANGED ;
      lstrcpy(signalPtr[1].theName, "Variable Changed");
      signalPtr[2].theID = SIGNAL_ERROR ;
      lstrcpy(signalPtr[2].theName, "Error");
      result = A_OK;
      break;

    case AOBJ_CHECKOBJECT:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(theObject);
      result = tf->ObjectHealthy((AObjInfo*)systemPtr->message2);
      break;

    default:
      result = A_NOTHANDLED;

  }       // switch (systemPtr->message1)...
  return result;
}       // Administration(...

///////////////////////////////////////////////////////////////
// GetVersion:  This function returns the version, which is
//              stored in Data4->version
//
///////////////////////////////////////////////////////////////

static int  GetAppVersion( OBJECTID theObject)

  {
  int         ver = 0;
  DATAFOUR*   Data4;

  Data4 = (DATAFOUR*)AObjLockData(theObject, 4L);
  ver = Data4->version;
  AObjUnlockData(theObject, 4L);
  return ver;

  }

///////////////////////////////////////////////////////////////
// TFac_obj:
//
///////////////////////////////////////////////////////////////

LONG TFac_obj(OBJECTID objid, pAObjMessage sys )
{
  TEXTFACT*   tf;
  LONG        result;

  switch ( int(sys->message1) )
  {
    case AOBJ_EDITOBJECT:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(objid);
      tf->CheckObjectID();
      DialogBoxParam(vG.hInstance, "TFDIALOG", tf->GetActive(), (DLGPROC)TEXTFACT::TfDialog, (LPARAM)tf);
      result = A_OK;
      break;

    case AOBJ_VALIDATEOBJECTIDS:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(objid);
      result = tf->Validate(sys);
      break;

    case AOBJ_BUILDSIGNALS:
      MEMBLOCKID         signalBlock;
      pAObjSignalRecord  signalPtr;

      signalBlock = (MEMBLOCKID)sys->message2;
      AMemSetSize(signalBlock, NUM_TEXTFACT_SIGNALS * sizeof(AObjSignalRecord));
      signalPtr = (pAObjSignalRecord)AMemLock(signalBlock);
      signalPtr[0].theID = SIGNAL_EXPUPDATE ;
      lstrcpy(signalPtr[0].theName, "Expression Updated");
      signalPtr[1].theID = SIGNAL_VARCHANGED ;
      lstrcpy(signalPtr[1].theName, "Variable Changed");
      signalPtr[2].theID = SIGNAL_ERROR ;
      lstrcpy(signalPtr[2].theName, "Error");
      result = A_OK;
      break;

    case AOBJ_CHECKOBJECT:
      tf = (TEXTFACT*)AObjGetRuntimeInfo(objid);
      result = tf->ObjectHealthy((AObjInfo*)sys->message2);
      break;
  }       // switch ( int(sys->message1) )...
  return (result);
//  AFuncReportResult(sys, 0, NULL);
}       // TFac_obj(...


///////////////////////////////////////////////////////////////
// GetSerVersion:
//
///////////////////////////////////////////////////////////////

LONG FAR PASCAL _export GetSerVersion(
  TYPEID  theType)

{
  if(theType==OTYPE_TEXTMAKER)
    return VERSION_DLL;
  else
    return 0L;
}       // GetSerVersion(...
