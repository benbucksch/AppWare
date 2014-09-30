////////////////////////////////////////////////////
/// This file contains the mouse & key hook functions
/// for the Window Object
////////////////////////////////////////////////////

#include "w_includ.h"

void ComboboxKludgeFunction(
	HWND										hwnd)
	{
	WINDOWRUNTIME*					wr = GetWindowRuntime();
	AOBJECTID								oiWindow;
	WINDOW*									wnd;
	OpWndItemD*							wi;

	I_VERIFY_POINTER(wr, return );

	//first, go up untill we get the parent window
	while(hwnd &&  GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD)
		hwnd = GetParent(hwnd);

	if (hwnd  &&  ((oiWindow = wr->GetWindowObjectFromHwnd(hwnd)) != 0) )
		{// if the parent window is one of our window objects
		wnd = LockWindowData(oiWindow);
		I_VERIFY_POINTER(wnd, return);

		wi = NULL;
		while( (wi = wnd->GetNextItem(wi, FALSE)) != NULL)
			{
			if (WINDOWAPI::IsHwndItem(wi)  &&  IsWindow( ((OpHwndWndItemD*)wi)->m_hwndItem) )
				SendMessage( ((OpHwndWndItemD*)wi)->m_hwndItem, CB_SHOWDROPDOWN, FALSE, 0L);
			}

		UnlockWindowData(oiWindow);
		}

	}

/////////////////////////////////////////////////////

LRESULT ALMCALLBACK WindowMouseHook(
	int											code,
	WPARAM									wParam,
	LPARAM									lParam)
	{
	WINDOWRUNTIME*					wr = GetWindowRuntime();

	I_VERIFY_POINTER(wr, return 0);

	if (code >=0)
  	{
		if (wParam == WM_MOUSEMOVE  ||  wParam == WM_NCMOUSEMOVE)
			wr->MouseMoveHook( ((MOUSEHOOKSTRUCT*)lParam)->pt	);
		else if (wParam == WM_LBUTTONDOWN)
			wr->MouseDownHook( ((MOUSEHOOKSTRUCT*)lParam)->hwnd, ((MOUSEHOOKSTRUCT*)lParam)->pt);

		if (wParam == WM_LBUTTONDOWN  ||  wParam == WM_NCLBUTTONDOWN)
			ComboboxKludgeFunction( ((MOUSEHOOKSTRUCT*)lParam)->hwnd);
		}
	
	return CallNextHookEx(wr->GetNextMouseHook(), code, wParam, lParam);
	}

////////////////////////////////////////////////////

void WINDOWRUNTIME::MouseMoveHook(
	POINT 									pt)
	{
	WINDOW*									wnd;
	AOBJECTID								oiWindow;
  OpWndItemD*							wi = NULL;

	if ( (oiWindow = WindowObjectFromPoint(pt)) != 0 )
		{
		wnd = LockWindowData(oiWindow);
		I_VERIFY_POINTER(wnd, return);
		wi = wnd->ItemFromScreenPoint(pt);
	  UnlockWindowData(oiWindow);
		}

	if (wi != m_wiMouseIn) //The mouse is in a different item
		{
		if (m_wiMouseIn)
			{
			m_wiMouseIn->MouseEntry(FALSE);
			AEvtPostSignalAtTail(m_wiMouseIn->m_oiParent, m_wiMouseIn->m_uidItem + I_SIG_OUTOFFSET);
			}

		if (wi)
			{
			wi->MouseEntry(TRUE);
			AEvtPostSignalAtTail(wi->m_oiParent, wi->m_uidItem + I_SIG_INOFFSET);
			}

		m_wiMouseIn = wi;
		}

	}

//////////////////////////////////////////////////////////

void WINDOWRUNTIME::MouseDownHook(
	HWND 										hwndTarget,
	POINT 									pt)
	{
	AOBJECTID								oiWindow;
	WINDOW*									wnd;
	OpWndItemD*							wi;

	if ( (oiWindow = GetWindowObjectFromHwnd(hwndTarget)) != 0)
		{//The mouse down is for a lite item, or no item
		wnd = LockWindowData(oiWindow);
		I_VERIFY_POINTER(wnd, return);

		wi = wnd->ItemFromScreenPoint(pt);
		if (wi && wi != wnd->GetActiveItem() && WINDOWAPI::IsItemActivatable(wi))
			wnd->SetActiveItem(wi, OWND_ACTIVATED_BY_MOUSE, TRUE);

		UnlockWindowData(oiWindow);
		}
	else
  	{
		wi = GetItemFromHwnd(hwndTarget);  //Gets a hwnd'd window item only

		if (wi &&  WINDOWAPI::IsItemActivatable(wi))
			{
			wnd = LockWindowData(wi->m_oiParent);
			I_VERIFY_POINTER(wnd, return);

			if (wi != wnd->GetActiveItem())
				wnd->SetActiveItem(wi, OWND_ACTIVATED_BY_MOUSE, TRUE);

			UnlockWindowData(wi->m_oiParent);
			}
		}

	}

//////////////////////////////////////////////////////////

LRESULT ALMCALLBACK WindowKeyHook(
	int											code,
	WPARAM									wParam,
	LPARAM									lParam)
	{
	WINDOWRUNTIME*					wr = GetWindowRuntime();
	LRESULT									rVal = 0;	//returning 0 will let the message live

	I_VERIFY_POINTER(wr, return 0);

	//From the SDK help file. topic:KeyboardProc.  
	//Bit	Meaning
	// 29	Specifies the context code. The value is 1 if the ALT key is held down while the key is pressed;
	// 30	Specifies the previous key state. The value is 1 if the key is down before the message is sent
	// 31	Specifies the key-transition state. The value is 1 if the key is being released

	if (wr->IsHwndAWindowObject(GetActiveWindow()) )
		{
		if (code >= 0 && !(lParam & 0x80000000L/*bit31*/) )
			{
			switch (wParam)
				{
				case VK_RETURN: 
					rVal = wr->EnterKeyHook(wParam, lParam); 
					break;

				case VK_ESCAPE: 
					if (!(lParam & 0x20000000L/*bit29*/)  &&  !IsCtrlKeyDown())
						rVal = wr->EscKeyHook(wParam, lParam); 
					break;

				case VK_TAB: 
				if (!(lParam & 0x20000000L/*bit29*/))
					rVal = wr->TabKeyHook(); 
				break;

				case VK_LEFT:
				case VK_UP:
				case VK_RIGHT:
				case VK_DOWN: 
					rVal = wr->CursorKeyHook(wParam); 
					break;

				default: 
					rVal = wr->CheckForAccelKey(LOWORD(wParam), wr->GetActiveWindowObject() ); 
					break;
				}
	    }
		}

	if (rVal)
		return rVal;
	else
		return CallNextHookEx(wr->GetNextKeyHook(), code, wParam, lParam);
	}

//////////////////////////////////////

LPARAM WINDOWRUNTIME::CheckForAccelKey(
	char										vKey,
	AOBJECTID								oiWindow)
	{
	WINDOW*									wnd;
	LPARAM									rVal;
	char										modifier;

	if (!oiWindow ||  !vKey)
		return FALSE;

	modifier = 0;

	if (IsShiftKeyDown() )	modifier |= OWND_SHIFT;
	if (IsCtrlKeyDown() )	  modifier |= OWND_CTRL;
	if (IsAltKeyDown() )		modifier |= OWND_ALT;

	if ( (modifier & (OWND_CTRL | OWND_ALT)) == 0)
		{//neither CTRL nor ALT is down
		if (vKey >= 'A'  &&  vKey <= 'Z')
			return FALSE; //key A-Z without ALT or CTRL is not an accel key
		if (vKey >= '0'  &&  vKey <= '9')
			return FALSE; //key 0-9 without ALT or CTRL is not an accel key
		if (vKey == VK_SPACE)
			return FALSE; //space key without ALT or CTRL is not an accel key
		}

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return 0);

	rVal = wnd->SendAccelKey(wnd->GetActiveItem(), 0, vKey, modifier);

	UnlockWindowData(oiWindow);
  return rVal;
  }

///////////////////////////////////////

LRESULT WINDOWRUNTIME::EnterKeyHook(
	WPARAM									wParam,
	LPARAM 									lParam)
	{
	WINDOW*									wnd;
  OpWndItemD*							wi;
	HWND										hwndItem;
	BOOL										keyDown;
  LPARAM									rVal = 0;
	AOBJECTID								oi;

	if ( (oi = m_oiActive) == 0)
		return 0;

	wnd = LockWindowData(oi);
	I_VERIFY_POINTER(wnd, return 0);

	if (wnd->ShouldEnterKeyBeStolen())
  	{
		keyDown = lParam & 0x80000000L/*bit 31*/ ? FALSE:TRUE;
		wi = wnd->GetDefaultItem();
		if (wi)
    	{
			if ( WINDOWAPI::IsLiteItem(wi) )
				{
				if (keyDown)
					((OpLiteWndItemD*)wi)->KeyDown(wParam, 0, lParam);
				else
					((OpLiteWndItemD*)wi)->KeyUp(wParam, 0, lParam);

				rVal = 1; //Stop futher processing
				}

			else if (WINDOWAPI::IsHwndItem(wi) )
				{
				hwndItem = ((OpHwndWndItemD*)wi)->m_hwndItem;
				I_VERIFY_WINDOW(hwndItem, return 0);
				SendMessage(hwndItem, keyDown ? WM_KEYDOWN:WM_KEYUP, wParam, lParam);
				rVal = 1; //Stop futher processing
				}
			}

		ADBG_EVAL(else ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_UNKNOWN_WI_TYPE, "WINDOWRUNTIME::EnterKeyHook");)
		}


	UnlockWindowData(oi);
	return rVal;
	}


//////////////////////////////////////

LRESULT WINDOWRUNTIME::EscKeyHook(
	WPARAM									wParam,
	LPARAM 									lParam)
	{
	WINDOW*									wnd;
  OpWndItemD*							wi;
	HWND										hwndItem;
	BOOL										keyDown;
  LPARAM									rVal = 0;
	AOBJECTID								oi;

	if ( (oi = m_oiActive) == 0)
		return 0;

	wnd = LockWindowData(oi);
	I_VERIFY_POINTER(wnd, return 0);

	if (wnd->ShouldEscKeyBeStolen())
		{
		keyDown = lParam & 0x80000000L/*bit 31*/ ? FALSE:TRUE;
		wi = wnd->GetCancelItem();
		if (wi)
			{
			if (WINDOWAPI::IsLiteItem(wi) )
				{
				if (keyDown)
					((OpLiteWndItemD*)wi)->KeyDown(wParam, 0, lParam);
				else
					((OpLiteWndItemD*)wi)->KeyUp(wParam, 0, lParam);
    	
				rVal = 1; //Stop futher processing
				}

			else if (wi && WINDOWAPI::IsHwndItem(wi) )
				{
				hwndItem = ((OpHwndWndItemD*)wi)->m_hwndItem;
				I_VERIFY_WINDOW(hwndItem, return 0);
				SendMessage(hwndItem, keyDown ? WM_KEYDOWN:WM_KEYUP, wParam, lParam);
				rVal = 1; //Stop futher processing
				}
      }

		ADBG_EVAL(else ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_UNKNOWN_WI_TYPE, "WINDOWRUNTIME::EscKeyHook");)
		}


	UnlockWindowData(oi);
	return rVal;
	}

///////////////////

LRESULT WINDOWRUNTIME::TabKeyHook()
	{
	WINDOW*									wnd;
  OpWndItemD*							wi;
  LPARAM									rVal = 0;
	AOBJECTID								oi;

	if ( (oi = m_oiActive) == 0)
		return 0;

	wnd = LockWindowData(oi);
	I_VERIFY_POINTER(wnd, return 0);

	if (wnd->ShouldTabKeyBeStolen())
		{
		wi = wnd->GetActiveItem();
		if (wi)
			wi = IsShiftKeyDown() ? wnd->GetPrevTabstop(wi) : wnd->GetNextTabstop(wi);

		if (wi)
     	{
			wnd->SetActiveItem(wi, OWND_ACTIVATED_BY_TAB, IsShiftKeyDown()?FALSE:TRUE);
			rVal = 1;
			}
		}

	UnlockWindowData(oi);
  return rVal;
  }

///////////////////////////////

LRESULT WINDOWRUNTIME::CursorKeyHook(
	WPARAM									wParam)
	{
	WINDOW*									wnd;
  OpWndItemD*							wi;
  LPARAM									rVal = 0;
	AOBJECTID								oi;

	if ( (oi = m_oiActive) == 0)
		return 0;

	wnd = LockWindowData(oi);
	I_VERIFY_POINTER(wnd, return 0);

	if (wnd->ShouldCursorKeysBeStolen())
		{
		wi = wnd->GetActiveItem();
		if (wi)
    	{
			wi = (wParam == VK_LEFT || wParam == VK_UP) ? wnd->GetPrevActivatableItemWithinGroup(wi) : wnd->GetNextActivatableItemWithinGroup(wi);
			if (wi)
    		{
				wnd->SetActiveItem(wi, OWND_ACTIVATED_BY_CURSORKEYS);
				rVal = 1;
				}
			}
		}

	UnlockWindowData(oi);
  return rVal;
	}



///////////////////////////////////////////////////////////////////////////////

void WINDOWRUNTIME::InstallHooks()
	{
	
	m_hNextMouseHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)WindowMouseHook, NULL, GetCurrentThreadId());
	I_ASSERT_HOOK(m_hNextMouseHook, "Mouse");

	m_hNextKeyHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)WindowKeyHook, NULL, GetCurrentThreadId());
	I_ASSERT_HOOK(m_hNextKeyHook, "Keyboard");
	}

////////////////////////////////////////

void WINDOWRUNTIME::RemoveHooks()
	{
	if (m_hNextMouseHook != NULL)
		UnhookWindowsHookEx(m_hNextMouseHook); 

	if (m_hNextKeyHook != NULL)
		UnhookWindowsHookEx(m_hNextKeyHook);
	}


