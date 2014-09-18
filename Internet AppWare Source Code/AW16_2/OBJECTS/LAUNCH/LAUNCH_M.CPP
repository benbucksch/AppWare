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
// FILE:          "launch_m.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   LibMain, WEP, Launcher ALM entry points,
//                and Launcher object editing functions.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_text.h>

#include "launcher.h"
#include "launc_rc.h"
#include "opendlg.h"

// Global Variables ////////////////////////////////////////////////////////

HINSTANCE   g_hinstDLL;

#ifdef __BORLANDC__
extern int  _WinAllocFlag;
#endif

////////////////////////////////////////////////////////////////////////////

extern "C" int FAR PASCAL LibMain
  (
  HINSTANCE hInstance,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine
  )

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  g_hinstDLL = hInstance;

  #ifdef __BORLANDC__
  _WinAllocFlag = GMEM_SHARE;
  #endif

  if(wHeapSize>0)
    UnlockData(0);

  return 1;
  }
  
////////////////////////////////////////////////////////////////////////////

int FAR PASCAL WEP
  (
  int nParameter
  )
  
  {
  I_UNUSED(nParameter);
  
  return 1;
  }

////////////////////////////////////////////////////////////////////////////

extern "C" long ALMCALLBACK LaunchAdmn
  (
  OBJECTID      oiLaunch,
  pAObjMessage  pMsg
  )

  {
  Launcher* pLaunch;
  short*    pVer;
  long      lRet;

  switch(pMsg->message1)
    {
    case AOBJ_CREATED:
    case AOBJ_AWAKENED:
      pLaunch = new Launcher(oiLaunch);
      if(pLaunch &&  pLaunch->Valid())
        {
        AObjSetRuntimeInfo(oiLaunch, (long)pLaunch);
        lRet = (pMsg->message1 == AOBJ_AWAKENED) ? A_CONTINUE : A_OK;
        }
      else
        {
        AObjSetRuntimeInfo(oiLaunch, (long)NULL);
        lRet = A_ERROR;
        }
      break;

    case AOBJ_DESTROYED:
    case AOBJ_ASLEEP:
      pLaunch = (Launcher*)AObjGetRuntimeInfo(oiLaunch);
      I_ASSERT(pLaunch);
      delete pLaunch;
      AObjSetRuntimeInfo(oiLaunch, (long)NULL);
      lRet = A_CONTINUE;
      break;


    case AOBJ_ASSIGNOBJECT : 
      pLaunch = (Launcher*)AObjGetRuntimeInfo(oiLaunch);
      I_ASSERT(pLaunch);
      lRet = pLaunch->Assign((Launcher*)AObjGetRuntimeInfo((OBJECTID)pMsg->message3));
      break;

    case AOBJ_CHECKOBJECT:
      lRet = A_OK;  // never any compile errors or warnings
      break;

    case AOBJ_EDITOBJECT :
      pLaunch = (Launcher*)AObjGetRuntimeInfo(oiLaunch);
      I_ASSERT(pLaunch);
      lRet = pLaunch->Edit();
      break;

    case AOBJ_EXPORTOBJECT : 
      pLaunch = (Launcher*)AObjGetRuntimeInfo(oiLaunch);
      I_ASSERT(pLaunch);
      lRet = pLaunch->Export();
      break;

    case AOBJ_IMPORTOBJECT :
      pLaunch = (Launcher*)AObjGetRuntimeInfo(oiLaunch);
      I_ASSERT(pLaunch);
      lRet = pLaunch->Import();
      break;

    case AOBJ_READ:   // done before AOBJ_AWAKENED
      pVer = (short*)AObjLockData(oiLaunch, DATA_VERSION);
      if(pVer && (*pVer != VERSION_LAUNCHOBJECT))
        {
        if(*pVer < VERSION_LAUNCHOBJECT)
          {
          lRet = A_WARNING;
          *pVer = VERSION_LAUNCHOBJECT;
          }
        else if(*pVer > VERSION_LAUNCHOBJECT)
          {
          lRet = A_ERROR;
          AObjReportError(oiLaunch, AOBJ_READ, A_ERROR, "Launcher\talm_appl.dll", VERSION_LAUNCHOBJECT);
          }
        }
      else
        lRet = A_OK;

      if(pVer)
        AObjUnlockData(oiLaunch, DATA_VERSION);
      break;

    default:
      lRet = A_NOTHANDLED;
      break;
    }
    
  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//  
//  Function:     LaunchApplication --- ALM Function
//
//  Description:  This will cause the application specified in the
//                launcher object to start executing.
//
//  ALM Function Info:
//    inputs:
//      1.  Launcher (launcher object)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after launching (even if error)
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK LaunchApplication
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);
  
  OBJECTID  oiLaunch = AFuncGetTypedParameter(1L, OTYPE_LAUNCHER);

  if(!oiLaunch)
    AFuncReportResult(pMsg, 1L, "The input to the \"Launch Application\" function must be a Launcher Object.");
  else
    {
    Launcher* pLaunch = (Launcher*)AObjGetRuntimeInfo(oiLaunch);
    
    AFuncReportResult(pMsg, pLaunch->LaunchApplication(pMsg->errorMessage, sizeof(pMsg->errorMessage)), NULL);
    }
  }

////////////////////////////////////////////////////////////////////////////
//  
//  Function:     SetApplication --- ALM Function
//
//  Description:  This will set the default application for the launcher
//                object.  This will be the application run from
//                LaunchApplication.
//
//  ALM Function Info:
//    inputs:
//      1.  Launcher (launcher object)
//      2.  Command Line (optional text object)
//
//    outputs:
//      none
//
//    flows:
//      1.  Followed after the application has been set
//      2.  Followed if the user cancels the dialog (or an error occurs).
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK SetApplication
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);
  
  OBJECTID  oiLaunch = AFuncGetTypedParameter(1L, OTYPE_LAUNCHER);
  OBJECTID  oiText   = AFuncGetTypedParameter(2L, OTYPE_TEXT);

  if(!oiLaunch)
    AFuncReportResult(pMsg, 2L, "The first input to the \"Set Application\" function must be a Launcher Object.");
  else
    {
    Launcher* pLaunch = (Launcher*)AObjGetRuntimeInfo(oiLaunch);

    if(!oiText)
      AFuncReportResult(pMsg, (pLaunch->Edit(TRUE) == A_MODIFIED) ? 1L : 2L, NULL);
    else
      {
      pLaunch->SetApplication(oiText);
      AFuncReportResult(pMsg, 1L, NULL);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////

void InitLauncherEditDlg
  (
  HWND  hwnd
  )
  
  {
  EditLauncher* pEL = (EditLauncher*)GetWindowLong(hwnd, DWL_USER);

  if(!pEL->fRuntime)
    {
    char  szBuffer[256];
    
    AUtlSetTitleFont(OTYPE_LAUNCHER, NULL, GetDlgItem(hwnd, ST_OBJECT));
		AUtlCenterDialog(hwnd, 0);
    AObjGetName(pEL->pLaunch->ObjectID(), szBuffer, sizeof(szBuffer));
    SetWindowText(hwnd, szBuffer);
    }
    
  SetDlgItemText(hwnd, ED_CMDLINE,  pEL->pLaunch->CommandLine());
  SetDlgItemText(hwnd, ED_STARTDIR, pEL->pLaunch->StartDirectory());

  CheckDlgButton(hwnd, XB_RUNMIN,   pEL->pLaunch->RunMinimized());
  }
  
////////////////////////////////////////////////////////////////////////////

int UpdateLauncher
  (
  HWND  hwnd
  )
  
  {
  EditLauncher* pEL = (EditLauncher*)GetWindowLong(hwnd, DWL_USER);
  char          szBuffer[256];
  int           fRes;

  GetDlgItemText(hwnd, ED_CMDLINE, szBuffer, sizeof(szBuffer));
  fRes = pEL->pLaunch->SetCommandLine(szBuffer);

  GetDlgItemText(hwnd, ED_STARTDIR, szBuffer, sizeof(szBuffer));
  fRes = pEL->pLaunch->SetStartDirectory(szBuffer) || fRes;

  fRes = pEL->pLaunch->SetRunMinimized(IsDlgButtonChecked(hwnd, XB_RUNMIN)) || fRes;
  
  return fRes ? (int)A_MODIFIED : (int)A_OK;
  }
  
////////////////////////////////////////////////////////////////////////////

void BrowseButtonPressed
  (
  HWND  hwnd
  )
  
  {
  EditLauncher* pEL = (EditLauncher*)GetWindowLong(hwnd, DWL_USER);	
  OpenDialog  od(hwnd, !pEL->fRuntime);
  char        szBuffer[256];
  int         nLen = GetDlgItemText(hwnd, ED_STARTDIR, szBuffer, sizeof(szBuffer));

  if(nLen>0)
    od.SetInitDir(szBuffer);

  if(od.FileDlg())
    {
    SetDlgItemText(hwnd, ED_CMDLINE, od.FullPath());
    if(nLen==0)
      {
      strcpy(szBuffer, od.FullPath());
      szBuffer[od.FileOffset()==3 ? 3 : od.FileOffset()-1] = 0;
      SetDlgItemText(hwnd, ED_STARTDIR, szBuffer);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export LauncherEditDlg
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )
  
  {
  BOOL          fRet      = FALSE;
  
  switch(uMsg)
    {
    case WM_INITDIALOG:
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitLauncherEditDlg(hwnd);
      SetFocus(GetDlgItem(hwnd, ED_CMDLINE));
      SendDlgItemMessage(hwnd, ED_CMDLINE, EM_SETSEL, TRUE, MAKELONG(0, -1));
      fRet = FALSE;
      break;
      
    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, UpdateLauncher(hwnd));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, (int)A_WARNING);
          fRet = TRUE;
          break;

        case PB_BROWSE:
          BrowseButtonPressed(hwnd);
          fRet = TRUE;
          break;
          
        case IDHELP:
          fRet = TRUE;
          break;

        default:
          fRet = FALSE;
          break;
        }
      break;

    default:
      fRet = FALSE;
    }
  
  return fRet;
  }
  
////////////////////////////////////////////////////////////////////////////
