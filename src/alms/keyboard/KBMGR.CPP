// kbmgr.cpp -- MDRT for the serius keyboard
// 28-Oct-92 jon rewritten

#define STRICT
#define ASTRICT
#include <memory.h>
#include <a_alm.h>
#include <intdebug.h>
#include <ddeml.h>

#include "keyboard.h"

//////////////// Global variable declarations

extern HANDLE			g_hInstance;				// instance of this DLL

////////////////////////////////////////////////////////////////
//
// FUNCTION:	postKeySignal (static)
//
// DESC:			Posts the specified signal with the specified how, for each
//						permanent Keyboard object in this serius app.
//
////////////////////////////////////////////////////////////////

static VOID		postKeySignal(
	long					theSignal
	)
	{
	long        key				= AOBJ_GETFIRSTKEY;
	KBRECORD*		kbRec;
	AOBJECTID    objNow;

	while (0 != (objNow = AObjGetNextObject(OTYPE_KEYBOARD, &key)))
		{
    kbRec = (KBRECORD*) AObjLockData(objNow, DATA0);

		if (kbRec->active != 0)
			AEvtPostSignalAtTail(objNow, theSignal);

		AObjUnlockData(objNow, DATA0);
		}
	}

////////////////////////////////////////////////////////////////
//
// The following function will translate the virtual keyboard code to
// appropriate signals to map to the MAC signals.
// The signals in the 2 platforms for Dec'92
// release (version 1.0) were different.
////////////////////////////////////////////////////////////////

static long MapSignal(LONG oldSignalID)
  {
	long newSignalID;

	newSignalID = oldSignalID;

	switch (oldSignalID)
		  {
			case  0x0D:  newSignalID = 0; break;  // "Enter Key"
			case  0x20:  newSignalID = 2; break; // "Spacebar"
    	case  0x31:  newSignalID = 3; break; // "Upper 1 Key"
			case  0x32:  newSignalID = 4; break; // "Upper 2 Key"
			case  0x33:  newSignalID = 5; break; // "Upper 3 Key"
			case  0x34:  newSignalID = 6; break; // "Upper 4 Key"
			case  0x35:  newSignalID = 7; break; // "Upper 5 Key"
			case  0x36:  newSignalID = 8; break; // "Upper 6 Key"
      case  0x37:  newSignalID = 9; break; // "Upper 7 Key"
      
			case  0x38:  newSignalID = 10; break; // "Upper 8 Key"
      
			case  0x39:  newSignalID = 11; break; // "Upper 9 Key"
			case  0x30:  newSignalID = 12; break; // "Upper 0 Key"

			case  0x08:  newSignalID = 13; break; // "Backspace Key"       // m: "Delete Key"
			case  0x1B:  newSignalID = 14; break; // "Escape Key"

			case  0x70:  newSignalID = 15; break; // "F1 Key"
			case  0x71:  newSignalID = 16; break; // "F2 Key"
			case  0x72:  newSignalID = 17; break; // "F3 Key"
			case  0x73:  newSignalID = 18; break; // "F4 Key"
			case  0x74:  newSignalID = 19; break; // "F5 Key"
			case  0x75:  newSignalID = 20; break; // "F6 Key"
			case  0x76:  newSignalID = 21; break; // "F7 Key"
			case  0x77:  newSignalID = 22; break; // "F8 Key"
			case  0x78:  newSignalID = 23; break; // "F9 Key"
			case  0x79:  newSignalID = 24; break; // "F10 Key"
			case  0x7A:  newSignalID = 25; break; // "F11 Key"
			case  0x7B:  newSignalID = 26; break; // "F12 Key"
			case  0x7C:  newSignalID = 27; break; // "F13 Key"
			case  0x7D:  newSignalID = 28; break; // "F14 Key"
			case  0x7E: newSignalID =  29; break; // "F15 Key"
			/*
			case  0x7F: newSignalID = oldSignalID; break; // "F16 Key"             // w+
			case  0x80: newSignalID = oldSignalID; break; // "F17 Key"             // w+
			case  0x81: newSignalID = oldSignalID; break; // "F18 Key"             // w+
			case  0x82: newSignalID = oldSignalID; break; // "F19 Key"             // w+
			case  0x83: newSignalID = oldSignalID; break; // "F20 Key"             // w+
			case  0x84: newSignalID = oldSignalID; break; // "F21 Key"             // w+
			case  0x85: newSignalID = oldSignalID; break; // "F22 Key"             // w+
			case  0x86: newSignalID = oldSignalID; break; // "F23 Key"             // w+
			case  0x87: newSignalID = oldSignalID; break; // "F24 Key"             // w+
			*/

			case  0x41: newSignalID = 31; break; // "A Key"
			case  0x42: newSignalID = 32; break; // "B Key"
			case  0x43: newSignalID = 33; break; // "C Key"
			case  0x44: newSignalID = 34; break; // "D Key"
			case  0x45: newSignalID = 35; break; // "E Key"
			case  0x46: newSignalID = 36; break; // "F Key"
			case  0x47: newSignalID = 37; break; // "G Key"
			case  0x48: newSignalID = 38; break; // "H Key"
			case  0x49: newSignalID = 39; break; // "I Key"
			case  0x4A: newSignalID = 40; break; // "J Key"
			case  0x4B: newSignalID = 41; break; // "K Key"
			case  0x4C: newSignalID = 42; break; // "L Key"
			case  0x4D: newSignalID = 43; break; // "M Key"
			case  0x4E: newSignalID = 44; break; // "N Key"
			case  0x4F: newSignalID = 45; break; // "O Key"
			case  0x50: newSignalID = 46; break; // "P Key"
			case  0x51: newSignalID = 47; break; // "Q Key"
			case  0x52: newSignalID = 48; break; // "R Key"
			case  0x53: newSignalID = 49; break; // "S Key"
			case  0x54: newSignalID = 50; break; // "T Key"
			case  0x55: newSignalID = 51; break; // "U Key"
			case  0x56: newSignalID = 52; break; // "V Key"
			case  0x57: newSignalID = 53; break; // "W Key"
			case  0x58: newSignalID = 54; break; // "X Key"
			case  0x59: newSignalID = 55; break; // "Y Key"
			case  0x5A: newSignalID = 56; break; // "Z Key"

			case  0x2F: newSignalID = 57; break; // "Help Key"            // v?

          // *** NOTE: the order of these directional key signals
					// ***       is different than on serius/m.  For example: newSignalID = ; break; //
          // ***       the "Forward Delete Key" signal in serius/m
          // ***       appears in the midst of directional key signals.

			case  0x24: newSignalID = 58; break; // "Home Key"
			case  0x21: newSignalID = 59; break; // "Page Up Key"
			case  0x2E: newSignalID = 60; break; // "Delete Key"          // m: "Forward Delete Key" in Serius/M
			case  0x23: newSignalID = 61; break; // "End Key"

			case  0x22: newSignalID = 62; break; // "Page Down Key"

			case  0x25: newSignalID = 63; break; // "Left Arrow Key"
			case  0x27: newSignalID = 64; break; // "Right Arrow Key"
			case  0x26: newSignalID = 65; break; // "Up Arrow Key"
			case  0x28: newSignalID = 66; break; // "Down Arrow Key"

			
			case  0x2D: newSignalID = 68; break ;// "Insert Key"          // w+

			case  0x0C: newSignalID = 67; break; // "Clear Key"           // v?

        // -------------------------- // m+ "Numeric = Key"

			case  0x6F: newSignalID = 69; break; // "Numeric / Key"
			case  0x6A: newSignalID = 70; break; // "Numeric * Key"
			case  0x6D: newSignalID = 71; break; // "Numeric - Key"
			case  0x6B: newSignalID = 72; break; // "Numeric + Key"
			case  0x6E: newSignalID = 73; break; // "Numeric . Key"

			case  0x60: newSignalID = 74; break; // "Numeric 0 Key"
			case  0x61: newSignalID = 75; break; // "Numeric 1 Key"
			case  0x62: newSignalID = 76; break; // "Numeric 2 Key"
			case  0x63: newSignalID = 77; break; // "Numeric 3 Key"
			case  0x64: newSignalID = 78; break; // "Numeric 4 Key"
			case  0x65: newSignalID = 79; break; // "Numeric 5 Key"
			case  0x66: newSignalID = 80; break; // "Numeric 6 Key"
			case  0x67: newSignalID = 81; break; // "Numeric 7 Key"
			case  0x68: newSignalID = 82; break; // "Numeric 8 Key"
			case  0x69: newSignalID = 83; break; // "Numeric 9 Key"

			case  0x10: newSignalID = 85; break; // "Shift Key"
			case  0x11: newSignalID = 86; break; // "Control Key"
			case  0x12: newSignalID = 87; break; // "Alt Key"             // m: "Option Key"

        // -------------------------- // m+ "Command Key"

			case  0x09: newSignalID = 30; break; // "Tab Key"

 /**** Windows strays: ****/
      
			case  0x01: newSignalID = 90; break; // "Left Mouse Button"   // w+
			case  0x02: newSignalID = 91; break; // "Right Mouse Button"  // w+
			case  0x03: newSignalID = 92; break; // "Cancel Key"          // w+: newSignalID = ; break; // v?
			case  0x04: newSignalID = 93; break; // "Middle Mouse Button" // w+

			case  0x13: newSignalID = 94; break; // "Pause Key"           // w+
			case  0x14: newSignalID = 95; break; // "Caps Lock Key"       // w+

			case  0x29: newSignalID = 96; break; // "Select Key"          // w+: newSignalID = oldSignalID; break; // v?

			case  0x2B: newSignalID = 97; break; // "Execute Key"         // w+: newSignalID = ; break; // v?
			case  0x2C: newSignalID = 98; break; // "Print Screen Key"    // w+

			case  0x6C: newSignalID = 99; break; // "Numeric Separator Key"  // w+: newSignalID = ; break; // v?

			case  0x90: newSignalID = 100; break; // "Num Lock Key"        // w+
			case  0x91: newSignalID = 101; break; // "Scroll Lock Key"    }  // w+
      
      default: break;
      }

	return newSignalID;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	KeyMouseHook
//
// DESC:			Implements the WH_MOUSE hook.  This is looking for mouse
//						up and down.
//
////////////////////////////////////////////////////////////////

extern "C"
LRESULT ALMCALLBACK		KeyMouseHook(
	int			code,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	pKeyRuntimeInfo		pRunInfo			= (pKeyRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_KEYBOARD);

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
		switch (wParam)			// Do we worry about WM_NC* messages?
			{
			case WM_LBUTTONDOWN:
				postKeySignal(MapSignal(primaryDownSignalID));
				break;

			case WM_RBUTTONDOWN:
				postKeySignal(MapSignal(secondaryDownSignalID));
				break;

			case WM_MBUTTONDOWN:
				postKeySignal(MapSignal(middleDownSignalID));
				break;

			default:
				break;
			}
		}

	return CallNextHookEx(pRunInfo->hMouseHook, code, wParam, lParam);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	KeyKeyboardHook
//
// DESC:			Implements the WH_KEYBOARD hook.  This is looking for keyboard
//						up and down messages.
//
////////////////////////////////////////////////////////////////

extern "C"
LRESULT ALMCALLBACK		KeyKeyboardHook(
	int			code,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	pKeyRuntimeInfo		pRunInfo		= (pKeyRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_KEYBOARD);

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
		if (lParam & (1L << 31))			// Bit31 specifies pressed/released transition
			postKeySignal(MapSignal(wParam));
		}

	return CallNextHookEx(pRunInfo->hMouseHook, code, wParam, lParam);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HandleStartup
//
// DESC:			Creates a process-specific hook 
//
////////////////////////////////////////////////////////////////

extern "C"
VOID ALMCALLBACK		HandleStartup(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	HMODULE						hMod;
	HHOOK							hMouseHook;
	HHOOK							hKeyboardHook;
	DWORD						 	hCurThread;
	pKeyRuntimeInfo		pRunInfo;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	hCurThread = GetCurrentThreadId();

	// Because of a (semi) documented defect in Windows 3.1, we need to pass
	// the module instead of the instance to SetWindowsHookEx.  The module
	// can be retrieved via the (undocumented) use of GetModuleHandle().
	// This info was published in "Ask Dr. GUI".  THIS WON'T WORK WITH Win32!

	hMod = GetModuleHandle("ALM_KEYB.DLL");

	I_ASSERT(hMod != NULL);

	hMouseHook    = SetWindowsHookEx(WH_MOUSE,    (HOOKPROC) KeyMouseHook,
														hMod, hCurThread);

	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC) KeyKeyboardHook,
														hMod, hCurThread);

	if (hMouseHook    != NULL &&
			hKeyboardHook != NULL)
		{
		pRunInfo = new KeyRuntimeInfo;

		if (pRunInfo != NULL)
			{
			pRunInfo->fStatus       = 0x0;
			pRunInfo->hMouseHook    = hMouseHook;
			pRunInfo->hKeyboardHook = hKeyboardHook;

			ATypeSetRuntimeInfo(OTYPE_KEYBOARD, (LONG) pRunInfo);
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
// FUNCTION:	HandleShutDown
//
// DESC:			Uninstalls the task-specific hook that was installed
//						during HandleStartup.
//
////////////////////////////////////////////////////////////////

extern "C"
VOID ALMCALLBACK		HandleShutDown(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	pKeyRuntimeInfo		pRunInfo;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	pRunInfo = (pKeyRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_KEYBOARD);

	if (pRunInfo != NULL)
		{
		I_ASSERT(pRunInfo->hMouseHook    != NULL);
		I_ASSERT(pRunInfo->hKeyboardHook != NULL);

		UnhookWindowsHookEx(pRunInfo->hMouseHook);
		UnhookWindowsHookEx(pRunInfo->hKeyboardHook);
		}

	delete pRunInfo;
	}
