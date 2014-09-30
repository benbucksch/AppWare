#include "b_include.h"


////////////////////////////

PUSH_VI::PUSH_VI(
	OBJECTID 								oiItem, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status):HWND_VI(oiItem, oiParent, rcItem, status)
	{
	LPBUTTON								but;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	but->CopyAttributes(&m_attributes);
	but->MakeSureButtonHasTitle();

	OWndSetItemActivatable(this, TRUE);
	OWndSetItemEnabled(this, but->IsButtonEnabled() );
	if (but->IsButtonDefault())
		OWndSetDefaultItem(this);
	if (but->IsButtonCancel())
		OWndSetCancelItem(this);

	OWndSetItemUsesDefaultKey(this, TRUE);

	SetAccelKey();

	UnlockButtonObject(Object());
	}

///////////////////////////

void ALMAPI PUSH_VI::Destroy()
	{
	I_ASSERT(!m_hwndItem);
	delete this;
	}

///////////////////////////

void ALMAPI PUSH_VI::DataChanged(
	long										details, 
	long 										modifiers, 
	long 										variation)
	{
	LPBUTTON								but;
	
	I_UNUSED(details);I_UNUSED(modifiers);I_UNUSED(variation);

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	but->SetControlsText(m_hwndItem);

	UnlockButtonObject(Object());
	}

////////////////////////////

void ALMAPI PUSH_VI::ParentOpened(void)
	{
	LPBUTTON								but;
	HDC											hdc;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	m_hwndItem = OWndCreateWindowEx(
								this,
								0,
								g_szButtonClassName,
								but->GetTitle(),
								but->GetCreateWindowStyleBits(),
								g_hInstance,
								this);

	I_ASSERT_WINDOW(m_hwndItem, return);
	
	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrSetFont(m_hwndItem);

	UnlockButtonObject(Object());
	}

////////////////////////////

void ALMAPI PUSH_VI::ParentClosing(void)
	{
	I_ASSERT_WINDOW(m_hwndItem, return);

	m_attributes.AttrDeleteFont();
	DestroyWindow(m_hwndItem);
	m_hwndItem = 0;
	}

////////////////////////////

void ALMAPI PUSH_VI::AccelKeyHit(void)
	{
	SendMessage(m_hwndItem, WM_LBUTTONDOWN, 0, 0L);
	SendMessage(m_hwndItem, WM_LBUTTONUP, 0, 0L);
	}

////////////////////////////

void ALMAPI PUSH_VI::SetDefaultItem(
	BOOL 										set)
	{		 
	LONG										s;

	I_ASSERT_WINDOW(m_hwndItem, return);

	s = GetWindowLong(m_hwndItem, GWL_STYLE);

	if (set)	s |= BS_DEFPUSHBUTTON;
	else			s &= ~BS_DEFPUSHBUTTON;

	SetWindowLong(m_hwndItem, GWL_STYLE, s);
	InvalidateRect(m_hwndItem, NULL, TRUE);
	UpdateWindow(m_hwndItem);
	}

////////////////////////////

long ALMAPI PUSH_VI::Activate(
	int 										causedBy, 
	BOOL 										mustKeep)
	{
	this->SetDefaultItem(TRUE);
	return OpHwndWndItem::Activate(causedBy, mustKeep);
	}

////////////////////////////

void ALMAPI PUSH_VI::Deactivated()
	{
	this->SetDefaultItem(FALSE);
	OpHwndWndItem::Deactivated();
	}

////////////////////////////

LRESULT ALMAPI PUSH_VI::ParentNotification(
	HWND 										hwnd, 
	UINT 										message, 
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LPBUTTON								but;
	LRESULT									rVal;
	
	I_UNUSED(hwnd); I_UNUSED(wParam);

	if (message == WM_COMMAND  &&  HIWORD(lParam) == BN_CLICKED)
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return 0L);

		if (m_bDoubleClicked)
			but->PostDoubleClickedSignal();
		else
			but->PostPushedSignal();

		UnlockButtonObject(Object());
		rVal = 0L;
		}

	else
		rVal = DefWindowProc(hwnd, message, wParam, lParam);

  return rVal;
	}

////////////////

BOOL ALMAPI PUSH_VI::AppearanceChanged(
	PUSH_VI *								vi,
	long 										fwWhatChanged,
	long										)
	{
	LPBUTTON								but;

	if (vi->m_hwndItem)
		{
		but = LockButtonObject(vi->Object());
		I_ASSERT_POINTER(but, return TRUE);

		if (fwWhatChanged & I_APPEARANCE_CHANGED_TITLE)
			{
			but->SetControlsText(vi->m_hwndItem);
			vi->SetAccelKey();
			}

		if (fwWhatChanged & I_APPEARANCE_CHANGED_ENABLE)
			OWndSetItemEnabled(vi, but->IsButtonEnabled() );

		UnlockButtonObject(vi->Object());
		}

	return TRUE;
	}

///////////////////

BOOL PUSH_VI::SetAccelKey()
	{
	LPBUTTON								but;
	BOOL										rVal=FALSE;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return FALSE);

	if (but->GetAccelVKey())
		rVal = OWndSetItemAccelKey(this, but->GetAccelVKey(), but->GetAccelMKey());

	UnlockButtonObject(Object());

	return rVal;
	}


////////////////////////////////////////////
// This is the callback proc for the sub-classed button
// control.  It is used by PUSH, CHECK, and RADIO button types
////////////////////////////////////////////////

LRESULT far pascal _export AppwareButtonObjectProc(
	HWND										hwnd,
	UINT										message,
	WPARAM									wParam,
	LPARAM									lParam)
	{
	HWND_VI*								wi;
	LPCREATESTRUCT					cr;
	LPBUTTON								but;

	switch(message)
		{
		case WM_CREATE:
			cr = (LPCREATESTRUCT)lParam;
			wi =(HWND_VI*)cr->lpCreateParams;
			SetThisPointer(hwnd, wi);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			wi = GetThisPointer(hwnd);
			I_ASSERT_POINTER(wi, break);

			but = LockButtonObject(wi->Object());
			I_ASSERT_POINTER(but, break);

			if ( (wParam == VK_RETURN   &&  but->GetButtonType()==OBUTN_PUSH)	 || 
				(wParam == VK_ESCAPE   &&  OWndIsItemCancel(wi)) )
				{
				wParam = VK_SPACE;
				if (message == WM_KEYDOWN)
					OWndSetActiveItem(wi);
				}

			if (message == WM_KEYUP &&  isalnum(wParam))
				OWndSimulateAccelKey(wi->m_oiParent, OTYPE_BUTTON, wParam, OWND_ALT);

			wi->m_bDoubleClicked = FALSE;
			UnlockButtonObject(wi->Object());
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			wi = GetThisPointer(hwnd);
			I_ASSERT_POINTER(wi, break);

			wi->m_bDoubleClicked = (message == WM_LBUTTONDBLCLK ? TRUE : FALSE);

			but = LockButtonObject(wi->Object());
			I_ASSERT_POINTER(but, break);

			but->PostTouchedSignal();
			but->BeginTrackingButton(wi);

			UnlockButtonObject(wi->Object());
			break;

			


		case WM_KILLFOCUS:
		case WM_CANCELMODE:
		case WM_LBUTTONUP:
			wi = GetThisPointer(hwnd);
			I_ASSERT_POINTER(wi, break);

			but = LockButtonObject(wi->Object());
			I_ASSERT_POINTER(but, break);

			if (but->IsBeingTracked())
				{
				but->PostMouseUpSignal();
				but->EndTrackingButton();
				}

			UnlockButtonObject(wi->Object());
			break;

		}

	return CallWindowProc(g_defButtonProc, hwnd, message, wParam, lParam);
	}

