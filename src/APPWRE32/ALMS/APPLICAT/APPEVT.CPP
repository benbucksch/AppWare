/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					APPEVT.H
//
// AUTHOR:				Jon (original author)
//
// DESCRIPTION:		Application object event handlers.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include <intdebug.h>
#include <ddeml.h>

#include "app.h"

extern HINSTANCE		g_hDLL;

////////////////////////////////////////////////////////////////
//
// FUNCTION:	postAppSignal (static)
//
// DESC:			Posts the specified signal with the specified how, for each
//						permanent Application object in this serius app.
//
////////////////////////////////////////////////////////////////

static VOID		postAppSignal(
	long					theSignal,
	long					theHow
	)
	{
	long        key				= AOBJ_GETFIRSTKEY;
	AOBJECTID    objNow;

	while (0 != (objNow = AObjGetNextObject(OTYPE_APPLICATION, &key)))
		AEvtPostStandard(objNow, AEVENT_SIGNAL, theSignal, theHow);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	AppDoaActivateApp
//
// DESC:			Posts a suspended or resumed signal.
//
////////////////////////////////////////////////////////////////

extern "C"
VOID ALMCALLBACK		AppDoaActivateApp(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	DATA0 *data0;

	I_UNUSED(theSystem);

	if (theEvent->details)					// from WM_ACTIVATEAPP::wParam
		{
		// The message is the result of the application being activated.
		// Before posting a resume signal make sure the WM_ACTIVATEAPP message
		// wasn't the result of the application being executed.  If this is the
		// case, then don't send the resumedSignalID message.

		data0 = (DATA0 *) ATypeLockData(OTYPE_APPLICATION, 0L);
		ATypeUnlockData(OTYPE_APPLICATION, 0L);

		if (data0 && *data0 == STARTUP)
			{
			// Don't post the signal and delete the data since the message is
			// the result of the application first being executed.
			ATypeDeleteData(OTYPE_APPLICATION, 0L);
			}
		else
			{
			//post the signal
			postAppSignal(resumedSignalID, AEVT_ATTAIL | AEVT_KILLDUPLICATES);
			}
		}
	else
		postAppSignal(suspendedSignalID, AEVT_ATTAIL | AEVT_KILLDUPLICATES);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	AppMouseHook
//
// DESC:			Implements the WH_MOUSE hook.  This is looking for mouse
//						up and down.
//
////////////////////////////////////////////////////////////////

extern "C"
LRESULT ALMCALLBACK		AppMouseHook(
	int			code,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	pAppRuntimeInfo		pRunInfo		= (pAppRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_APPLICATION);
	int								iButton			= 0;
	BOOL							fDown;

	I_ASSERT(pRunInfo != NULL);

	/*
	If an application for Windows uses the Dynamic Data Exchange
	Management Library (DDEML) in addition to a message hook [for example,
	by calling SetWindowsHook() or SetWindowsHookEx()], it is possible
	that your hook procedure will receive messages that are intended for
	the DDEML libraries.

	For the DDEML libraries to work properly, you must make sure that your
	hook function does not forward on any messages that are intended for
	the DDEML libraries.
	If your hook procedure receives a code of type MSGF_DDEMGR, you should
	return FALSE instead of calling the CallNextHookEx() function.
  ---- Ref: Developer CD SKJ MAR. 24, 1994
	*/

	if (MSGF_DDEMGR == code)
  	return FALSE;
	else if (code < 0)
		{
		return CallNextHookEx(pRunInfo->hMouseHook, code, wParam, lParam);
		}

	else
		{
		switch (wParam)
			{
			// Do we worry about WM_NC* messages?

			// The order MUST be Left, Right, Middle.  This is a gross hack
			// that avoids testing again.  The fallthrough SHOULD occur.  Keep
			// in sync with APP_LEFTBUTTON, ... in app.h
			case WM_LBUTTONDOWN:	iButton++;
			case WM_RBUTTONDOWN:	iButton++;
			case WM_MBUTTONDOWN:
				postAppSignal(mouseDownSignalID, AEVT_ATMARK);
				fDown = TRUE;
				break;

			// The order MUST be Left, Right, Middle.  This is a gross hack
			// that avoids testing again.  The fallthrough SHOULD occur
			case WM_LBUTTONUP:	iButton++;
			case WM_RBUTTONUP:	iButton++;
			case WM_MBUTTONUP:
				postAppSignal(mouseUpSignalID, AEVT_ATMARK);
				fDown = FALSE;
				break;

			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
				postAppSignal(doubleClickSignalID, AEVT_ATMARK);
				iButton = -1;
				break;

			default:
				iButton = -1;
				break;
			}
		}

	if (iButton != -1)
		{
		if (fDown)
			{
			pRunInfo->mouseInfo |=  (1 << iButton);
			pRunInfo->lastMouse  = iButton;
			}
		else
			pRunInfo->mouseInfo &= ~(1 << iButton);
		}

	return CallNextHookEx(pRunInfo->hMouseHook, code, wParam, lParam);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	AppKeyboardHook
//
// DESC:			Implements the WH_KEYBOARD hook.  This is looking for keyboard
//						up and down messages.
//
////////////////////////////////////////////////////////////////

extern "C"
LRESULT ALMCALLBACK		AppKeyboardHook(
	int			code,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	pAppRuntimeInfo		pRunInfo		= (pAppRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_APPLICATION);

	I_ASSERT(pRunInfo != NULL);

	/*
	If an application for Windows uses the Dynamic Data Exchange
	Management Library (DDEML) in addition to a message hook [for example,
	by calling SetWindowsHook() or SetWindowsHookEx()], it is possible
	that your hook procedure will receive messages that are intended for
	the DDEML libraries.

	For the DDEML libraries to work properly, you must make sure that your
	hook function does not forward on any messages that are intended for
	the DDEML libraries.
	If your hook procedure receives a code of type MSGF_DDEMGR, you should
	return FALSE instead of calling the CallNextHookEx() function.
  ---- Ref: Developer CD SKJ MAR. 24, 1994
	*/

	if (MSGF_DDEMGR == code)
		return FALSE;
	else if (code < 0)
		{
		return CallNextHookEx(pRunInfo->hKeyboardHook, code, wParam, lParam);
		}

	else
		{
		if (lParam & (1L << 31))			// Bit31 specifies pressed/released transition
			postAppSignal(keyUpSignalID, AEVT_ATMARK);
		else
			postAppSignal(keyDownSignalID, AEVT_ATMARK);
		}

	return CallNextHookEx(pRunInfo->hKeyboardHook, code, wParam, lParam);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HandleStartup
//
// DESC:			Creates a task-specific hook 
//
////////////////////////////////////////////////////////////////

extern "C"
VOID ALMCALLBACK		HandleStartup(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	DATA0							data0					= STARTUP;	//set the data0 to signal startup
	HHOOK							hMouseHook;
	HHOOK							hKeyboardHook;
	DWORD							hCurThread;
	pAppRuntimeInfo		pRunInfo;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	// Create a temporary type data0 which signals
	// that a startup event was just processed.
	ATypeAddData(OTYPE_APPLICATION, &data0, sizeof(DATA0));
	postAppSignal(startupSignalID, AEVT_ATTAIL);

	hCurThread = GetCurrentThreadId();

#if 0	//!! assume WinNT and Windows95 don't have this defect
	// Because of a (semi) documented defect in Windows 3.1, we need to pass
	// the module instead of the instance to SetWindowsHookEx.  The module
	// can be retrieved via the (undocumented) use of GetModuleHandle().
	// This info was published in "Ask Dr. GUI".  THIS WON'T WORK WITH Win32!

	hMod = GetModuleHandle("ALM_SYSM.DLL");	//pass into SetWindowsHookEx

	I_ASSERT(hMod != NULL);
#endif	//!!

	hMouseHook    = SetWindowsHookEx(WH_MOUSE,    AppMouseHook,
														g_hDLL, hCurThread);

	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, AppKeyboardHook,
														g_hDLL, hCurThread);

	if (hMouseHook    != NULL &&
			hKeyboardHook != NULL)
		{
		pRunInfo = new AppRuntimeInfo;

		if (pRunInfo != NULL)
			{
			pRunInfo->fStatus       = 0x0;
			pRunInfo->mouseInfo     = 0x0;
			pRunInfo->hMouseHook    = hMouseHook;
			pRunInfo->hKeyboardHook = hKeyboardHook;

			ATypeSetRuntimeInfo(OTYPE_APPLICATION, (LONG) pRunInfo);
			}
		}

	else			// unhook from everything
		{
		if (hMouseHook != NULL)
			UnhookWindowsHookEx(hMouseHook);

		if (hKeyboardHook != NULL)
			UnhookWindowsHookEx(hKeyboardHook);
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HandleShutdown
//
// DESC:			Uninstalls the task-specific hook that was installed
//						during HandleStartup.
//
////////////////////////////////////////////////////////////////

extern "C"
VOID ALMCALLBACK		HandleShutdown(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	pAppRuntimeInfo		pRunInfo;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	pRunInfo = (pAppRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_APPLICATION);

	if (pRunInfo != NULL)
		{
		I_ASSERT(pRunInfo->hMouseHook    != NULL);
		I_ASSERT(pRunInfo->hKeyboardHook != NULL);

		UnhookWindowsHookEx(pRunInfo->hMouseHook);
		UnhookWindowsHookEx(pRunInfo->hKeyboardHook);
		}

	delete pRunInfo;
	}
