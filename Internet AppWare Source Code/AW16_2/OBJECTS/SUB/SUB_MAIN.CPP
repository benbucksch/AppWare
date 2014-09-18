////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "sub_main.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>
#include <string.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>

#include "sub.h"
#include "sub_rc.h"

// globals and externs /////////////////////////////////////////////////////

HINSTANCE   g_hinst;

#ifdef __BORLANDC__
extern int  _WinAllocFlag;
#endif

// local function prototypes ///////////////////////////////////////////////

void InitSubDialog(HWND hwnd);
BOOL UpdateSubroutine(HWND hwnd);

////////////////////////////////////////////////////////////////////////////

int far pascal LibMain
  (
  HINSTANCE hinst,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine
  )

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  g_hinst = hinst;

  #ifdef __BORLANDC__
  _WinAllocFlag = GMEM_SHARE;
  #endif

  if(wHeapSize>0)
    UnlockData(0);

  return 1;
  }

////////////////////////////////////////////////////////////////////////////

extern "C" long ALMCALLBACK SubroutineAdmn
  (
  OBJECTID        oiSub,
  pAObjMessage    pMsg
  )

  {
  LONG          lRet;
  Subroutine*   pSub;
  short*        pVer;

  switch(pMsg->message1)
    {
    case AOBJ_AWAKENED:
    case AOBJ_CREATED:
      pSub = new Subroutine(oiSub);
      if(pSub &&  pSub->Valid())
        {
        AObjSetRuntimeInfo(oiSub, (long)pSub);
        lRet = (pMsg->message1 == AOBJ_AWAKENED) ? A_CONTINUE : A_OK;
        }
      else
        {
        AObjSetRuntimeInfo(oiSub, (long)NULL);
        lRet = A_ERROR;
        }
      break;

    case AOBJ_ASLEEP:
    case AOBJ_DESTROYED:
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      delete pSub;
      AObjSetRuntimeInfo(oiSub, (long)NULL);
      lRet = A_CONTINUE;
      break;

    case AOBJ_ASSIGNOBJECT:
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      lRet = pSub->Assign((Subroutine*)AObjGetRuntimeInfo((OBJECTID)(pMsg->message3)));
      break;

    case AOBJ_BUILDSIGNALS:
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      lRet = pSub->BuildSignals((MEMBLOCKID)pMsg->message2);
      break;

    case AOBJ_CHECKOBJECT:
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      lRet = pSub->Check();
      break;

    case AOBJ_EDITOBJECT:
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      lRet = pSub->Edit();
      break;      

    case AOBJ_EXPORTOBJECT :
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      lRet = pSub->Export();
      break;

    case AOBJ_IMPORTOBJECT :
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      lRet = pSub->Import();
      break;

    case AOBJ_READ:
      pVer = (short*)AObjLockData(oiSub, DATA_SUBDATA);
      if(pVer && (*pVer > VERSION_SUBOBJECT))
        {
        lRet = A_ERROR;
        AObjReportError(oiSub, AOBJ_READ, A_ERROR, "Subroutine Object\talm_subr.dll", VERSION_SUBOBJECT);
        }
      else if(pVer && (*pVer < VERSION_SUBOBJECT))
        {
        *pVer = VERSION_SUBOBJECT;

        // adjust the data size
        AObjUnlockData(oiSub, DATA_SUBDATA);
        AObjResizeData(oiSub, DATA_SUBDATA, sizeof(SubData));
        pVer = NULL;

        // set the project's dirty flag
        lRet = A_WARNING;
        }
      else
        lRet = A_OK;

      if(pVer)
        AObjUnlockData(oiSub, DATA_SUBDATA);
      break;

    case AOBJ_VALIDATEOBJECTIDS:
      pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
      lRet = pSub->ValidateIDs(pMsg);
      break;

    default:
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
}

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export SubDlg
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL        fRet;

  switch(uMsg)
    {
    case WM_INITDIALOG:
      AUtlSetTitleFont(OTYPE_SUB, NULL, GetDlgItem(hwnd, ST_TYPENAME));
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitSubDialog(hwnd);
      fRet = TRUE;
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, UpdateSubroutine(hwnd));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          fRet = TRUE;
          break;

        default :
          fRet = FALSE;
          break;
        }
      break;

    default:
      fRet = FALSE;
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

void InitSubDialog
  (
  HWND  hwnd
  )

  {
  Subroutine*   pSub = (Subroutine*)GetWindowLong(hwnd, DWL_USER);
  char          szBuffer[256];

  // set the dialog title
  AObjGetName(pSub->ObjectID(), szBuffer, 256);
  SetWindowText(hwnd, szBuffer);

  CheckDlgButton(hwnd, RB_MANUALRETURN, !pSub->AutoReturn());
  CheckDlgButton(hwnd, RB_AUTORETURN, pSub->AutoReturn());
  }

////////////////////////////////////////////////////////////////////////////

BOOL UpdateSubroutine
  (
  HWND  hwnd
  )

  {
  Subroutine*   pSub = (Subroutine*)GetWindowLong(hwnd, DWL_USER);

  return pSub->SetAutoReturn(IsDlgButtonChecked(hwnd, RB_AUTORETURN));
  }

////////////////////////////////////////////////////////////////////////////
