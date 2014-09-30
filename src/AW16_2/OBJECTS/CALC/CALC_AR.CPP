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

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:  Essentials
// Object:          Calculation
//
// Filename:  "calc_ar.cpp"
//
// Author:  Kevin Smart
//   Start:   July 13, 1992
//   Finish:  July xx, 1992
//
// Description:
//  Calculation Object Administration Routine and DLL entry point
//
////////////////////////////////////////////////////////////////////////////

#include  <a_alm.h>
#include  <intdebug.h>
#include  <string.h>

#include  "calc_obj.h"
#include  "strclass.h"
#include  "calc.h"
#include  "calc_dlg.h"

////////////////////////////Global Container////////////////////////////////

GLOBAL      G;
extern int  _WinAllocFlag;

////////////////////////////////////////////////////////////////////////////

int far pascal LibMain(
  HINSTANCE hInstance,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine)

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  G.hInst = hInstance;
  _WinAllocFlag = GMEM_SHARE;

  if(wHeapSize>0)
    UnlockData(0);

  return 1;
  }

////////////////////////////////////////////////////////////////////////////
static int  GetVersion(
  OBJECTID  theObject)
  {
  int         version = 0;
  CALCHEADER*   Data3;

  Data3 = (CALCHEADER*)AObjLockData(theObject, 3L);
  version = Data3->ver;
  AObjUnlockData(theObject, 3L);
  return version;

  }
////////////////////////////////////////////////////////////////////////////
extern "C" {
  long ALMCALLBACK CalcAdmn (
    OBJECTID        theObject,
    pAObjMessage    theSystem)

    {
    LONG                lRet;
    CALCULATION*        calc;
    MEMBLOCKID          theBlock;
    pAObjSignalRecord   theArray;

    switch(theSystem->message1)
      {
      case AOBJ_CREATED :
        calc = new CALCULATION(theObject);
        I_ASSERT(calc);
        AObjSetRuntimeInfo(theObject, (long)calc);
        lRet  = A_OK;
        break;

      case AOBJ_AWAKENED :
        calc = new CALCULATION(theObject);
        I_ASSERT(calc);
        AObjSetRuntimeInfo(theObject, (long)calc);
        lRet = A_CONTINUE;
        break;

      case AOBJ_DESTROYED :
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        delete calc;
        G.DeleteGlobalData();
        lRet  = A_OK;
        break;

      case AOBJ_ASLEEP :
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        delete calc;
        G.DeleteGlobalData();
        lRet  = A_CONTINUE;
        break;

      case AOBJ_ASSIGNOBJECT :
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        calc->DoAssign(theObject, (OBJECTID) (theSystem->message3));
        lRet  = A_OK;
        break;
  
      case AOBJ_IMPORTOBJECT :
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        calc->ImportObject();
        lRet  = A_OK;
        break;
  
      case AOBJ_EXPORTOBJECT :
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        calc->ExportObject();
        lRet  = A_OK;
        break;

      case AOBJ_READ:
        if(GetVersion(theObject)!=VERSION_OBJECT)
          {
          lRet = AObjReportError(theObject, AOBJ_READ, A_ERROR,
                          "Calculation\talm_calc.dll", VERSION_OBJECT);
          }
        else
          {
          lRet = A_OK;  
          }
        break;

      case AOBJ_EDITOBJECT :
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        calc->CheckIDs();
        if (DialogBoxParam(G.hInst, "CALCDLG", GetActiveWindow(),
          (FARPROC)CalcDlg,(LPARAM)calc))
          { 
          lRet = A_MODIFIED;
          }
         else
          {
          lRet = A_OK;
          }
        break;

      case AOBJ_VALIDATEOBJECTIDS:
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        calc->Validate(theSystem);
        lRet = A_OK;
        break;

      case AOBJ_BUILDSIGNALS:
        theBlock = (MEMBLOCKID)theSystem->message2;
        AMemSetSize(theBlock, SIGNALCOUNT*sizeof(AObjSignalRecord));
        theArray = (pAObjSignalRecord)AMemLock(theBlock);

        theArray[calcObjectUpdated].theID = calcObjectUpdated;
        lstrcpy(theArray[calcObjectUpdated].theName, "Calculated");

        theArray[calcVariableChanged].theID = calcVariableChanged;
        lstrcpy(theArray[calcVariableChanged].theName, "Variable Changed");

        theArray[calcExpressionError].theID = calcExpressionError;
        lstrcpy(theArray[calcExpressionError].theName, "Error In Expression");
        lRet = A_OK;
        break;

      case AOBJ_CHECKOBJECT : {
        calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);
        I_ASSERT(calc);
        char  errString[255];

        lRet = A_OK;
        if (calc->Result() == 0) {
          lRet = AObjReportError(theObject, AOBJ_CHECKOBJECT, A_WARNING,
                      "Calculation object should have a result object specified.",
                      0);
          }
        else if(!calc->ValidExpression(errString)) {
          lRet = AObjReportError(theObject, AOBJ_CHECKOBJECT, A_WARNING,
                    errString, 0);
          }
        }
        break;

      default:
        lRet = A_NOTHANDLED;
        break;
      }

    return lRet;
    }
  }
// GetSerVersion ///////////////////////////////////////////////////////////
/*extern "C" {
  long far pascal _export GetSerVersion(
    TYPEID  theType)

    {
    if(theType==OTYPE_EXPRESSION)
      return VERSION_DLL;
    else
      return 0L;
    }
  }
 */
////////////////////////////////////////////////////////////////////////////
