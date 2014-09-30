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
// FILE:          "mci.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

// include files ///////////////////////////////////////////////////////////

#include <mem.h>
#include <windows.h>
#include <mmsystem.h>

#include <a_alm.h>
#include <intdebug.h>

#include "mci.h"

// global variables ////////////////////////////////////////////////////////

extern HINSTANCE          g_hinstDLL;
static const char* const  k_szCallbackWndClass = "AW_MCI_CallbackWnd";

// Class variable initialization ///////////////////////////////////////////

HWND MediaControlInterface::sm_hwndCallback  = NULL;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::MediaControlInterface()
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

MediaControlInterface::MediaControlInterface
  (
  ) : LinkedList(),
      m_dwLastNtfyCmd(0L),
      m_dwDeviceID(0)

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::~MediaControlInterface()
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

MediaControlInterface::~MediaControlInterface
  (
  )

  {
  if((this == sm_pHead) && (this == sm_pTail) && sm_hwndCallback)
    DestroyCallbackWindow();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Break(DWORD, int, HWND)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Break
  (
  DWORD dwFlags,
  int   nVirtKey,
  HWND  hwndBreak
  )

  {
  BreakMCI  bMCI;
  bMCI.nVirtKey  = nVirtKey;
  bMCI.hwndBreak = hwndBreak;

  return CallCommand(dwFlags, MCI_BREAK, &bMCI);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::SysInfo(DWORD, LPMCI_SYSINFO_PARMS)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::SysInfo
  (
  DWORD       dwFlags,
  SysInfoMCI* pSysInfo
  )

  {
  return CallCommand(dwFlags, MCI_SYSINFO, pSysInfo);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Close(BOOL)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Close
  (
  BOOL  fWait
  )

  {
  DWORD dwRet = CallGenericCommand(fWait, MCI_CLOSE);

  if(0 == dwRet)
    m_dwDeviceID = 0;

  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::GetDevCaps
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::GetDevCaps
  (
  BOOL    fWait,
  DWORD   dwItem,
  DWORD&  dwReturn
  )

  {
  GetDevCapsMCI   gdc;
  DWORD           dwFlags = MCI_GETDEVCAPS_ITEM | (fWait ? MCI_WAIT : MCI_NOTIFY);
  DWORD           dwRet;

  gdc.dwItem = dwItem;

  dwRet = CallCommand(dwFlags, MCI_GETDEVCAPS, &gdc);
  if(0 == dwRet)
    dwReturn = gdc.dwReturn;
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Info
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Info
  (
  DWORD dwFlags,
  LPSTR szProdInfo,
  DWORD dwSize
  )

  {
  InfoMCI info;

  info.lpstrReturn = szProdInfo;
  info.dwRetSize   = dwSize;

  return CallCommand(dwFlags, MCI_INFO, &info);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Open
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Open
  (
  DWORD     dwFlags,
  OpenMCI*  pOpen
  )

  {
  long  dwRet = CallCommand(dwFlags, MCI_OPEN, pOpen);

  if(0 == dwRet)
    m_dwDeviceID = pOpen->wDeviceID;

  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Status
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Status
  (
  DWORD   dwFlags,
  DWORD   dwItem,
  DWORD   dwTrack,
  DWORD&  dwReturn
  )

  {
  StatusMCI status;
  DWORD     dwRet;
  status.dwItem   = dwItem;
  status.dwTrack  = dwTrack;

  dwRet = CallCommand(dwFlags, MCI_STATUS, &status);
  if(0 == dwRet)
    dwReturn = status.dwReturn;
  return dwRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Load
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Load
  (
  DWORD     dwFlags,
  LoadMCI*  pLoad
  )

  {
  return CallCommand(dwFlags, MCI_LOAD, pLoad);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Pause
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Pause
  (
  BOOL  fWait
  )

  {
  return CallGenericCommand(fWait, MCI_PAUSE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Play
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Play
  (
  DWORD     dwFlags,
  PlayMCI*  pPlay
  )

  {
  return CallCommand(dwFlags, MCI_PLAY, pPlay);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Record
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Record
  (
  DWORD dwFlags,
  DWORD dwFrom,
  DWORD dwTo
  )

  {
  RecordMCI record;
  record.dwFrom = dwFrom;
  record.dwTo   = dwTo;

  return CallCommand(dwFlags, MCI_RECORD, &record);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Resume
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Resume
  (
  BOOL  fWait
  )

  {
  return CallGenericCommand(fWait, MCI_RESUME);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Save
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Save
  (
  DWORD     dwFlags,
  SaveMCI*  pSave
  )

  {
  return CallCommand(dwFlags, MCI_SAVE, pSave);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Seek
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Seek
  (
  DWORD dwFlags,
  DWORD dwTo
  )

  {
  SeekMCI seek;
  seek.dwTo = dwTo;

  return CallCommand(dwFlags, MCI_SEEK, &seek);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Set
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Set
  (
  DWORD   dwFlags,
  SetMCI* pSet
  )

  {
  return CallCommand(dwFlags, MCI_SET, pSet);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Step
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Step
  (
  DWORD       dwFlags,
  GenericMCI* pStep
  )

  {
  return CallCommand(dwFlags, MCI_STEP, pStep);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Stop
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Stop
  (
  BOOL  fWait
  )

  {
  return CallGenericCommand(fWait, MCI_STOP);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Copy
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Copy
  (
  BOOL  fWait
  )

  {
  return CallGenericCommand(fWait, MCI_COPY);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Cue
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Cue
  (
  BOOL  fWait
  )

  {
  return CallGenericCommand(fWait, MCI_CUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Cut
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Cut
  (
  BOOL  fWait
  )

  {
  return CallGenericCommand(fWait, MCI_CUT);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Paste
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::Paste
  (
  BOOL  fWait
  )

  {
  return CallGenericCommand(fWait, MCI_PASTE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::Notify
//
//  DESCRIPTION:  Default implementation does nothing.
//
////////////////////////////////////////////////////////////////////////////

void MediaControlInterface::Notify
  (
  UINT  uNotifyCode
  )

  {
  I_UNUSED(uNotifyCode);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::CallGenericCommand(BOOL, DWORD)
//                                              --- private
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::CallGenericCommand
  (
  BOOL  fWait,
  DWORD dwCmd
  )

  {
  GenericMCI  gMCI(fWait ? 0 : CallbackWnd());
  DWORD       dwFlags = fWait ? MCI_WAIT : MCI_NOTIFY;

  if(!fWait)
    m_dwLastNtfyCmd = dwCmd;

  return mciSendCommand(m_dwDeviceID, (UINT)dwCmd, dwFlags, (DWORD)&gMCI);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::CallCommand
//                              (DWORD, GenericMCI*, DWORD) --- private
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

DWORD MediaControlInterface::CallCommand
  (
  DWORD       dwFlags,
  DWORD       dwCmd,
  GenericMCI* pGeneric
  )

  {
  if(dwFlags & MCI_NOTIFY)
    {
    pGeneric->dwCallback = (DWORD)CallbackWnd();
    m_dwLastNtfyCmd = dwCmd;
    }

  return mciSendCommand(m_dwDeviceID, (UINT)dwCmd, dwFlags, (DWORD)pGeneric);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::MapDeviceIDToMCI(UINT)
//                                              --- private, static
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

MediaControlInterface* MediaControlInterface::MapDeviceIDToMCI
  (
  UINT  dwID
  )

  {
  MediaControlInterface*  pMCI = (MediaControlInterface*)sm_pHead;

  while(pMCI && (pMCI->m_dwDeviceID != dwID))
    pMCI = (MediaControlInterface*)pMCI->m_pNext;

  return pMCI;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::CallbackWndProc
//                         (HWND, UINT, WPARAM, LPARAM) --- private, static
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long ALMCALLBACK MediaControlInterface::CallbackWndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  long                    lRet;
  BOOL                    fHandled;
  MediaControlInterface*  pMCI;

  switch (uMsg)
    {
    case MM_MCINOTIFY:
      pMCI = MapDeviceIDToMCI((UINT)LOWORD(lParam));
      if(pMCI)
        {
        pMCI->Notify((UINT)wParam);
        lRet = 0;
        fHandled = TRUE;
        }
      break;

    default:
      fHandled = FALSE;
      break;
    }

  if(!fHandled)
    lRet = DefWindowProc(hwnd, uMsg, wParam, lParam);

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::CallbackWnd()
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

HWND MediaControlInterface::CallbackWnd
  (
  ) const

  {
  if(!sm_hwndCallback)
    ((MediaControlInterface*)this)->CreateCallbackWindow();

  I_ASSERT(sm_hwndCallback);

  return sm_hwndCallback;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::DestroyCallbackWindow() --- private
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void MediaControlInterface::DestroyCallbackWindow
  (
  )

  {
  if(sm_hwndCallback)
    {
    // destroy the window
    DestroyWindow(sm_hwndCallback);
    sm_hwndCallback = NULL;

    // unregister the class with Windows
    UnregisterClass(k_szCallbackWndClass, g_hinstDLL);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MediaControlInterface::CreateCallbackWindow() --- private
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void MediaControlInterface::CreateCallbackWindow
  (
  )

  {
  WNDCLASS  wc;

  wc.style          = 0;
  wc.lpfnWndProc    = CallbackWndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hInstance      = g_hinstDLL;
  wc.hIcon          = NULL;
  wc.hCursor        = NULL;
  wc.hbrBackground  = NULL;
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = k_szCallbackWndClass;

  RegisterClass(&wc);

  sm_hwndCallback = CreateWindow(k_szCallbackWndClass, NULL, WS_POPUPWINDOW,
                                 0, 0, 0, 0, HWND_DESKTOP, NULL,
                                 g_hinstDLL, NULL);
  }

////////////////////////////////////////////////////////////////////////////
