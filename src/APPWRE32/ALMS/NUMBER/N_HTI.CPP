
////////////////////////////////////////////////////////
///  This file contains the code for hwnd'd number items
////////////////////////////////////////////////////////

#include "n_includ.h"

LRESULT CALLBACK AppwareNumberObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



////////////////////////////////////////////////////////////////

HwndNumberItem::HwndNumberItem(
	AOBJECTID								oiItem,
	AOBJECTID								oiParent,
	RECT*										rcItem,
	long										status) :	 OpHwndWndItem(oiItem, oiParent, rcItem, status)
	{
	LPNUMBER								num;

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "HwndNumberItem::HwndNumberItem", ;);

	num->CopyAttributes(&m_attributes);
	m_iSelectionStart = 0; 
	m_iSelectionEnd = num->SelectsAll() ? -1 : 0;

	OWndSetItemActivatable(this, num->IsActivatable());
	OWndSetItemEnabled(this, TRUE);
	OWndSetItemTransparent(this, FALSE);
	OWndSetItemUsesCursorKeys(this, TRUE);
	OWndSetItemUsesTab(this, FALSE);
	OWndSetItemUsesDefaultKey(this, FALSE);
	OWndSetItemUsesCancelKey(this, FALSE);

	UnlockNumberObject(Object());
	}

/////////////////////

void ALMAPI HwndNumberItem::Destroy()
	{
  delete this;
	}

/////////////////////////////////

void ALMAPI HwndNumberItem::DataChanged(
	long										details,
	long 										modifiers,
	long 										variation)
	{
	I_UNUSED(details);	I_UNUSED(modifiers);	I_UNUSED(variation);

	if (m_hwndItem && m_hwndItem != (HWND)modifiers) // No need to change the one that triggered the change!!
		ForceRedraw();

	}

/////////////////////////////////

void ALMAPI HwndNumberItem::ParentOpened()
	{
	LPNUMBER							num;
	HDC										hdc;

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "HwndNumberItem::ParentOpened", return);

	m_hwndItem = OWndCreateWindowEx(
					this,
					0,
					g_numberObjectClassName,
					num->GetStringPtr(),
					num->GetCreateWindowStyle(),
					g_hInstance,
					this);

	UnlockNumberObject(Object());

	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::ParentOpened", ;);

	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrCreateBrush();
	m_attributes.AttrSetFont(m_hwndItem);

	}

//////////////////////

void ALMAPI HwndNumberItem::ParentClosing()
	{

	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::ParentClosing", return);

	m_attributes.AttrDeleteFont();
	m_attributes.AttrDeleteBrush();
	DestroyWindow(m_hwndItem);
	m_hwndItem = 0;
  }

////////////////////////////

BOOL ALMAPI HwndNumberItem::GetSelection(
	LPINT 									start,
	LPINT 									count)
	{
	*start = m_iSelectionStart;
	*count = m_iSelectionEnd - m_iSelectionStart;
	(*start)++; //cause its one based

  return TRUE;
	}

////////////////////////

BOOL ALMAPI HwndNumberItem::SetSelection(
	int											start,
	int 										count)
	{
	start--;		 //cause its one-based
	if (start<0)	//make sure start is valid
		start = 0;

	if (count <0L)
		{ //a negitive count just moves the start back
		count = -count;
		if (count > start)
			count = start;
		start -= count;
		}

 	if (IsWindow(m_hwndItem))
		SendMessage(m_hwndItem, EM_SETSEL, start, start+count);

  return TRUE;
	}

////////////////////////////

long ALMAPI HwndNumberItem::Activate(
	int											causedBy,
	BOOL 										mustKeep)
	{
	LPNUMBER									num;

  I_UNUSED(mustKeep);

	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::Activate", return OWND_ACTIVATION_PASS);

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "HwndNumberItem::Activate", return OWND_ACTIVATION_KEPT);

	if (causedBy != OWND_ACTIVATED_BY_MOUSE)
  	{
		if (num->SelectsAll() )
			SendMessage(m_hwndItem, EM_SETSEL, 0, -1);
		else
			SendMessage(m_hwndItem, EM_SETSEL, m_iSelectionStart, m_iSelectionEnd); 
		}

	if (GetFocus() != m_hwndItem)
		SetFocus(m_hwndItem);

	UnlockNumberObject(Object());

	return OWND_ACTIVATION_KEPT;
	}

/////////////////////////////////

void ALMAPI HwndNumberItem::Deactivated()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::Deactivated", return);

	//Get the position of the cursor
	SendMessage(m_hwndItem, EM_GETSEL, (WPARAM)&m_iSelectionStart, (LPARAM)&m_iSelectionEnd);
	ForceRedraw();
	}

/////////////////////////////////
LRESULT ALMAPI HwndNumberItem::ParentNotification(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
	{
  LPNUMBER								num;
  LPARAM									rVal;

	switch(message)
		{
    case WM_COMMAND: 
			switch(HIWORD(wParam))
				{
				case EN_SETFOCUS:
					AEvtPostSignalAtTail(Object(), I_SIG_ACTIVATED);
					break;

				case EN_KILLFOCUS:
					num = LockNumberObject(Object());
					I_ASSERT_NUM(num, "HwndNumberItem::Notify", break);

					num->MaybePostChangedStuff();
					UnlockNumberObject(Object());

					AEvtPostSignalAtTail(Object(), I_SIG_DEACTIVATED);

					break;
				}
      rVal = DefWindowProc(hwnd, message, wParam, lParam);
			break;

		case WM_CTLCOLOREDIT:
			SetTextColor((HDC)wParam, m_attributes.AttrGetForeground());
			SetBkColor((HDC)wParam, m_attributes.AttrGetBackground());
			rVal = (LRESULT) m_attributes.AttrGetBrushHandle();
			break;

    default:
      rVal = DefWindowProc(hwnd, message, wParam, lParam);
			break;
		}

  return rVal;
  }

////////////////////////////

static void PclDrawRect(
	HDC											hdc,
	LPRECT									rect)
	{
	PRECT_STRUCT						prect;

	prect.prStyle = 0;

  // top line
	prect.prPosition.x = rect->left;
	prect.prPosition.y = rect->top;
	prect.prSize.x = rect->right - rect->left;
  prect.prSize.y = 1;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

  // left line
	prect.prPosition.x = rect->left;
	prect.prPosition.y = rect->top;
	prect.prSize.x = 1;
	prect.prSize.y = rect->bottom - rect->top;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

  // right line
	prect.prPosition.x = rect->right;
	prect.prPosition.y = rect->top;
	prect.prSize.x = 1;
	prect.prSize.y = rect->bottom - rect->top;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

  // bottom line
	prect.prPosition.x = rect->left;
	prect.prPosition.y = rect->bottom;
	prect.prSize.x = rect->right - rect->left;
  prect.prSize.y = 1;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
	}

/////////////////////////////////

void ALMAPI HwndNumberItem::Print(
	HDC 										hdc, 
	RECT* 									printRect, 
	int 										pclPrinter)
	{
	LPNUMBER								num;
	ATTRIBUTES							attPrinter(&m_attributes);
	COLORREF								fore, back, oldFore, oldBack;
	int											oldMode;
	HFONT										hFontOld;
	HBRUSH									hBrush, hBrushOld;
	unsigned int						drawFlag;
	POINT										ptDeflate;
	BOOL										bIsColor;

	bIsColor = GetDeviceCaps(hdc, NUMCOLORS) > 2;

	fore = bIsColor ? attPrinter.AttrGetForeground() : RGB(0,0,0); //black
	back = bIsColor ? attPrinter.AttrGetBackground() : RGB(255,255,255);//white

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "HwndNumberItem::Print", return);

	hBrush = CreateSolidBrush(back);
	hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
	attPrinter.AttrCreateFont(hdc);
	hFontOld = attPrinter.AttrSelectFont(hdc);
	oldFore	= SetTextColor(hdc, fore);
	oldBack = SetBkColor(hdc, back);
	oldMode = SetBkMode(hdc, TRANSPARENT);
	
	if (num->HasFrame())
		{
		if (pclPrinter) 
			PclDrawRect(hdc, printRect);
		else
			Rectangle(hdc, printRect->left, printRect->top, printRect->right, printRect->bottom);
		ptDeflate.x = MulDiv(GetDeviceCaps(hdc, LOGPIXELSX), 1, 16); //Shrink by 1/16 of an inch
		ptDeflate.y = MulDiv(GetDeviceCaps(hdc, LOGPIXELSY), 1, 16); //Shrink by 1/16 of an inch
		InflateRect(printRect, -ptDeflate.x, -ptDeflate.y);
		}
	else
		{//No frame
		FillRect(hdc, printRect, hBrush);
		}

	drawFlag = num->GetDrawTextStyle();

  //Draw the text
	DrawText(hdc, num->GetStringPtr(), -1, printRect, drawFlag);

  //Restore the hdc
	SetBkMode(hdc, oldMode);
	SetBkColor(hdc, oldBack);
	SetTextColor(hdc, oldFore);
	SelectObject(hdc, hFontOld);
	attPrinter.AttrDeleteFont();
	SelectObject(hdc, hBrushOld);
	DeleteObject(hBrush);

	UnlockNumberObject(Object());
	}

/////////////////////////////////

void ALMAPI HwndNumberItem::Cut()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::Cut", return);

	SendMessage(m_hwndItem, WM_CUT, 0, 0L);
	UpdateIfModified();
	}

/////////////////////////////////

void ALMAPI HwndNumberItem::Copy()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::Copy", return);

	SendMessage(m_hwndItem, WM_COPY, 0, 0L);
	}

/////////////////////////////////

void ALMAPI HwndNumberItem::Paste()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::Paste", return);

	SendMessage(m_hwndItem, WM_PASTE, 0, 0L);
	UpdateIfModified();
	}

/////////////////////////////////

void ALMAPI HwndNumberItem::Undo()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndNumberItem::Undo", return);

	SendMessage(m_hwndItem, EM_UNDO, 0, 0L);
	UpdateIfModified();
	}

//////////////////////////////////////
// This function will check and see if the edit control
// has been modified.  If it has, it will post the necessary
// signals and return TRUE.  Otherwise, it will return FALSE
//////////////////////////////////////
BOOL HwndNumberItem::UpdateIfModified()
	{
	LPNUMBER								num;
	char										str[I_STRING_LEN];
	BOOL										rVal = FALSE;

	if (SendMessage(m_hwndItem, EM_GETMODIFY, 0, 0L))
		{
		SendMessage(m_hwndItem, EM_SETMODIFY, 0, 0L);  //Clear the modified bit

		GetWindowText(m_hwndItem, str, I_STRING_LEN-1);
		
		num = LockNumberObject(Object());
		I_ASSERT_NUM(num, "HwndNumberItem::UpdateIfModified", return FALSE);
		num->SetValue(str, DONT_POST_SIGNALS, DONT_CHANGE_IF_PSEUDO, DONT_BLANK_IF_EMPTY_STRING);
		UnlockNumberObject(Object());

		AEvtPostSignalAtMark(Object(), I_SIG_EDITED);

		rVal = TRUE;
		}

	return rVal;
	}	 
	 
////////////////////////////////////////////////////////////////
/// Function called by the wndProc  ////////////////////////////
////////////////////////////////////////////////////////////////

LRESULT HwndNumberItem::WmSetCursor(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
	{
	LPNUMBER								num;
	BOOL										isEditable;

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "HwndNumberItem::WmSetCursor", return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam));

	isEditable = num->IsEditable();
	UnlockNumberObject(Object());

	if (!isEditable)
		{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
		}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
  }


//////////

LRESULT HwndNumberItem::WmEraseBkgnd(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
  {
	HDC											hdc;
	RECT										rect;

	I_UNUSED(message);I_UNUSED(lParam);
	
	hdc = (HDC)wParam;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, m_attributes.AttrGetBrushHandle() );

	return TRUE;
	}
//////////

LRESULT HwndNumberItem::WmChar(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
	{
  LPNUMBER								num;
  LRESULT									rVal;


	if (wParam == VK_RETURN)
  	{
		num = LockNumberObject(Object());
		I_ASSERT_NUM(num, "HwndNumberItem::WmChar", return 0L);
		num->MaybePostChangedStuff();
	  UnlockNumberObject(Object());
    rVal = 0;
    }
	else if (wParam == VK_TAB)
		rVal = 0;
	else
  	{
		rVal = CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
		UpdateIfModified();
    }

	return rVal;
	}

//////////////////////////////////////////////////////////////////////
//  This is the callback function for the subclassed edit control
//  for the number object  
//////////////////////////////////////////////////////////////////////

LRESULT CALLBACK AppwareNumberObjectProc(
	HWND										hwnd,
	UINT										message,
	WPARAM									wParam,
	LPARAM									lParam)
	{
	HwndNumberItem*					hni;
  LPCREATESTRUCT					cr;

	switch(message)
		{
		case WM_CREATE:
			cr = (LPCREATESTRUCT)lParam;
			hni =(HwndNumberItem*)cr->lpCreateParams;
			SetThisPointer(hwnd, hni);
			break;

		case WM_ERASEBKGND:
			hni = GetThisPointer(hwnd);
			I_ASSERT_HNI(hni, "AppwareTextObjectProc", break);
			return hni->WmEraseBkgnd(hwnd, message, wParam, lParam);

		case WM_CHAR:
			hni = GetThisPointer(hwnd);
			I_ASSERT_HNI(hni, "AppwareTextObjectProc", break);
			return hni->WmChar(hwnd, message, wParam, lParam);
		}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
	}



////////////////////////////////////////////////
void HwndNumberItem::ChangeFont(
	LPATTRIBUTES 						newAttr,
	long										changeFlag)
	{
	HDC											hdc;

	if (m_hwndItem)
		{ //If our window handle is created, the font and brush handles will be too.
		hdc = GetDC(m_hwndItem);
		m_attributes.AttrChangeFontIndirect(hdc, m_hwndItem, changeFlag, newAttr);
		ReleaseDC(m_hwndItem, hdc);
		}
	else
		m_attributes.AttrChangeFontIndirect(NULL, NULL, changeFlag, newAttr);
	}

////////////////////////////////////////////////
BOOL ALMAPI HwndNumberItemChangeFont(
	OpWndItemD* 						hwi, 
	long 										attrPtr, 
	long 										changeFlag)
	{
	((HwndNumberItem *)hwi)->ChangeFont((LPATTRIBUTES)attrPtr, changeFlag);
	return TRUE;
	}

////////////////

void HwndNumberItem::ForceRedraw()
	{
	LPNUMBER								num;
	char										str[I_STRING_LEN];

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "HwndNumberItem::ForceRedraw", return;);

	GetWindowText(m_hwndItem, str, I_STRING_LEN-1);

	if (lstrcmp(num->GetStringPtr(), str) )
		SetWindowText(m_hwndItem, num->GetStringPtr());

	UnlockNumberObject(Object());
	}

//////////////////////

BOOL ALMAPI HwndNumberItemForceRedraw(
	OpWndItemD* 						hwi, 
	long 										notused, 
	long 										notusedEither)
	{
	I_UNUSED(notused);I_UNUSED(notusedEither);

	((HwndNumberItem *)hwi)->ForceRedraw();
	return TRUE;
	}


