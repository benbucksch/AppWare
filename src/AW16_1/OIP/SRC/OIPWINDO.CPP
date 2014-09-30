/*//////////////////////////////////////////////////////////////
//
//											Copyright 1994, Novell, Inc.
//
//		<standard disclaimers here>
//
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//	7/22/93		Scott
//		Changed WM_ACTIVATE so that it posts the AEVENT_OS_ACTIVATE event instead of
//		emulating an OS event.
//
//////////////////////////////////////////////////////////////*/

#include <string.h>
#include "almsys.h"
#include "intdebug.h"

#include "system.h"
#include "context.h"


//----------------------------------------------------------------------
//   AAppGetIcon
//
// Returns the handle to the master icon.
//----------------------------------------------------------------------
extern "C"
HICON ALMAPI	AAppGetIcon()
  {
  return pTheSystem->GetCurrentContext()->GetMasterIcon();
  }

//----------------------------------------------------------------------
// AAppDefWndProc
//
// Handles top-level window messages and converts some of them into
// AppWare events.
//----------------------------------------------------------------------
extern "C"
void ALMAPI		AAppDefWndProc(
	HWND					hWnd,
	UINT					uMsg,
	WPARAM				wParam,
	LPARAM				lParam
	)
	{
	AEvtInfo			eEvent;

	I_UNUSED(hWnd);
	I_UNUSED(lParam);

	switch (uMsg)
		{
		case WM_ACTIVATEAPP:
			eEvent.sourceObject   =
			eEvent.targetObject = NULL;
			eEvent.what           = AEVENT_OS_ACTIVATEAPP;
			eEvent.details        = wParam;								// TRUE => we're being activated
			eEvent.modifiers      = 
			eEvent.variation    = 0;
			eEvent.when           = AAppGetTime();
			eEvent.how            = AEVT_ATTAIL;

			AEvtPost(&eEvent);
			// Now we need to post a no-op message so we get out of the
			// block on the GetMessage() in oipexe.cpp
			PostAppMessage(GetCurrentTask(), WM_NULL, 0, 0);
			break;


		/*** The WM_CANCELMODE message is sent to inform a window to cancel any internal mode.
		This message is sent to the focus window when a dialog
		box or message box is displayed, giving the focus window the opportunity to
		cancel modes such as mouse capture.  **/

		case WM_CANCELMODE:
			eEvent.sourceObject   =
			eEvent.targetObject = NULL;
			eEvent.what           = AEVENT_OS_CANCELMODE;
			eEvent.details        = 0;							
			eEvent.modifiers      =
			eEvent.variation    = 0;
			eEvent.when           = AAppGetTime();
			eEvent.how            = AEVT_ATTAIL;

			AEvtPost(&eEvent);
			// Now we need to post a no-op message so we get out of the
			// block on the GetMessage() in oipexe.cpp
			PostAppMessage(GetCurrentTask(), WM_NULL, 0, 0);
			break;

//		case WM_COMPACTING:				// ALMs should free memory
//			break;

//		case WM_DEVMODECHANGED:		// Reinitialize device mode settings
//			lpszDev = (LPSTR) lParam;
//			break;

//		case WM_ENDSESSION:				// ending session (is this the same as shutdown)
//			fEnding = (BOOL) wParam;
//			break;

//		case WM_POWER:
//			fwPowerEvt = wParam;
//			break;

//		case WM_QUERYENDSESSION:	// SPECIAL CASE:  need to post ATONCE
//			break;

//		case WM_SPOOLERSTATUS:		// information only
//			fwJobStatus = wParam;
//			cJobsLeft = LOWORD(lParam);
//			break;

//		case WM_SYSCOLORCHANGE:		// delete and recreate brushes
//			break;

//		case WM_TIMECHANGE:
//			break;

//		case WM_WININICHANGE:
//			pszSection = (LPSTR) lParam;
//			break;

		default:
			break;
		}
	}
