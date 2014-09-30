#include "b_includ.h"


////////////////////////////	

CHECK_VI::CHECK_VI(
	AOBJECTID 								oiItem, 
	AOBJECTID 								oiParent, 
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

	SetAccelKey();

	UnlockButtonObject(Object());
	}

////////////////////////////	

void ALMAPI CHECK_VI::Destroy()
	{
	I_ASSERT(!m_hwndItem);
	delete this;
	}

///////////////////////////

void ALMAPI CHECK_VI::DataChanged(
	long										details, 
	long 										modifiers, 
	long 										variation)
	{
	LPBUTTON								but;

	I_UNUSED(details);I_UNUSED(modifiers);I_UNUSED(variation);

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	but->SetControlsText(m_hwndItem);
	but->SetCheckState(m_hwndItem);

	UnlockButtonObject(Object());
	}

////////////////////////////

void ALMAPI CHECK_VI::ParentOpened(void)
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
	
	SendMessage(m_hwndItem, BM_SETCHECK, but->IsButtonSelected(), 0L);

	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrSetFont(m_hwndItem);
	m_attributes.AttrCreateBrush();

	UnlockButtonObject(Object());
	}


////////////////////////////

void ALMAPI CHECK_VI::ParentClosing(void)
	{

	I_ASSERT_WINDOW(m_hwndItem, return);

	m_attributes.AttrDeleteFont();
	m_attributes.AttrDeleteBrush();
	DestroyWindow(m_hwndItem);
	m_hwndItem = 0;
	}

////////////////////////////

void ALMAPI CHECK_VI::AccelKeyHit(void)
	{	
	SendMessage(m_hwndItem, WM_LBUTTONDOWN, 0, 0L);
	SendMessage(m_hwndItem, WM_LBUTTONUP, 0, 0L);
	}

////////////////////////////

LRESULT ALMAPI CHECK_VI::ParentNotification(
	HWND										hwnd, 
	UINT 										message, 
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LRESULT									rVal;
	LPBUTTON								but;

	I_UNUSED(hwnd);

	if (message == WM_COMMAND  &&  HIWORD(wParam) == BN_CLICKED) //!!PORT!!
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return 0L);

		but->SetButtonSelected((BOOL)SendMessage(m_hwndItem, BM_GETCHECK, 0, 0L));
		
		if (m_bDoubleClicked)
			but->PostDoubleClickedSignal();
		else
			but->PostPushedSignal();

		rVal = 0L;
		}

	else if (message == WM_CTLCOLORBTN) //!!PORT!!
		{
		SetTextColor((HDC)wParam, m_attributes.AttrGetForeground());
		SetBkColor((HDC)wParam, m_attributes.AttrGetBackground());
		rVal = (LRESULT) m_attributes.AttrGetBrushHandle();
		}
	else
		rVal = DefWindowProc(hwnd, message, wParam, lParam);

  return rVal;
	}

////////////////

void ALMAPI CHECK_VI::Print(
	HDC 										hdc, 
	LPRECT 									rcPrint, 
	int 										pclPrinter)
	{
	COLORREF								clrFore, clrBack;
	POINT										dpi, ptBoxSize;
	RECT										rcBox;
	HPEN										hPen;
	HBRUSH									hBrush;
	HFONT										hFont;	 
	LPBUTTON								but;

	I_UNUSED(pclPrinter);

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	dpi.x = GetDeviceCaps(hdc, LOGPIXELSX);
	dpi.y = GetDeviceCaps(hdc, LOGPIXELSY);

	if (GetDeviceCaps(hdc, NUMCOLORS) > 2)
		{ 
		clrFore = m_attributes.AttrGetForeground();
		clrBack = m_attributes.AttrGetBackground();
		}
	else
		{
		clrFore = RGB(0,0,0); //black
		clrBack = RGB(255,255,255);//white
		}

	//Calculate the size of the box.  Make it 13/72nds of an inch. Why ask why?
	ptBoxSize.x = MulDiv(13, dpi.x, 72);
	ptBoxSize.y = MulDiv(13, dpi.y, 72);

	if (ptBoxSize.y < rcPrint->bottom - rcPrint->top  &&  ptBoxSize.x < rcPrint->right - rcPrint->left)
		{	//Its big enough to print in

		//Center the box in the print rectangle
		rcBox.top = (rcPrint->top + rcPrint->bottom - ptBoxSize.y) / 2;
 		rcBox.left = rcPrint->left;
		rcBox.bottom = rcBox.top + ptBoxSize.y + 1;
		rcBox.right = rcBox.left + ptBoxSize.x + 1;

		hPen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, clrFore));
		hBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH) );

		Rectangle(hdc, rcBox.left, rcBox.top, rcBox.right, rcBox.bottom);

		SelectObject(hdc, hBrush);//Select in the old (no need to delete stock objects)

		if (but->IsButtonSelected())
			{
			MoveToEx(hdc, rcBox.left, rcBox.top, NULL);
			LineTo(hdc, rcBox.right, rcBox.bottom);
			MoveToEx(hdc, rcBox.right, rcBox.top, NULL);
			LineTo(hdc, rcBox.left, rcBox.bottom);
			}

		hPen = (HPEN)SelectObject(hdc, hPen); //Select in the old pen
		DeleteObject(hPen);  //and delete the one we created

		///Now, lets draw the text
		clrFore = SetTextColor(hdc, clrFore);
		clrBack = SetBkColor(hdc, clrBack);

		m_attributes.AttrCreateFont(hdc);	
		hFont = m_attributes.AttrSelectFont(hdc);		

		rcPrint->left += ptBoxSize.x + MulDiv(6, dpi.x , 72); //indent in so the text isn't on the ellipse

		DrawText(hdc, but->GetTitle(), -1, rcPrint, DT_LEFT|DT_VCENTER|DT_SINGLELINE);  //draw the text

		SelectObject(hdc, hFont); //Select in the old font
		m_attributes.AttrDeleteFont(); //Delete the one we created

		//Restore the colors
		SetTextColor(hdc, clrFore);
		SetBkColor(hdc, clrBack);
		}

	UnlockButtonObject(Object());
	}

////////

BOOL ALMAPI CHECK_VI::AppearanceChanged(
	CHECK_VI *							vi,
	long 										fwWhatChanged,
	long 										)
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

///////////////////////

BOOL CHECK_VI::SetAccelKey()
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
