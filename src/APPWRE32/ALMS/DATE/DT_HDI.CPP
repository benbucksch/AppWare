/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE: dt_hdi.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Hwnd'd date items
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "dt_incld.h"
#include <windowsx.h>


LRESULT CALLBACK AppwareDateObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HwndDateItem::HwndDateItem(AOBJECTID	oiItem,	AOBJECTID oiParent, RECT* rcItem, long	status) :	 OpHwndWndItem(oiItem, oiParent, rcItem, status)
{
	LPDate date;

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "HwndDateItem::HwndDateItem", ;);

	date->CopyAttributes(&m_attributes);
	m_selection = date->SelectsAll() ? MAKELONG(0,-1) : 0L;

	OWndSetItemActivatable(this, date->IsActivatable());
	OWndSetItemEnabled(this, TRUE);
	OWndSetItemTransparent(this, FALSE);
	OWndSetItemUsesCursorKeys(this, TRUE);
	OWndSetItemUsesTab(this, FALSE);
	OWndSetItemUsesDefaultKey(this, FALSE);
	OWndSetItemUsesCancelKey(this, FALSE);

	UnlockDateObject(Object());
}

void ALMAPI HwndDateItem::Destroy()
{
  delete this;
}

void ALMAPI HwndDateItem::DataChanged(long details,	long modifiers, long variation)
{

	I_UNUSED(details);	
	I_UNUSED(modifiers);	
	I_UNUSED(variation);

	if (m_hwndItem && m_hwndItem != (HWND)modifiers) // No need to change the one that triggered the change!!
		ForceRedraw();

}


void ALMAPI HwndDateItem::ParentOpened()
{
	LPDate date;
	HDC		 hdc;

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "HwndDateItem::ParentOpened", return);

	m_hwndItem = OWndCreateWindowEx(
					this,
					0,
					g_dateObjectClassName,
					date->GetDateStringPtr(),
					date->GetCreateWindowStyle(),
					g_hInstance,
					this);

	UnlockDateObject(Object());

	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::ParentOpened", ;);

	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrCreateBrush();
	m_attributes.AttrSetFont(m_hwndItem);

}

void ALMAPI HwndDateItem::ParentClosing()
{

	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::ParentClosing", return);

	m_attributes.AttrDeleteFont();
	m_attributes.AttrDeleteBrush();
	DestroyWindow(m_hwndItem);
	m_hwndItem = 0;
}

BOOL ALMAPI HwndDateItem::GetSelection(
	int *									start,
	int * 									count)
	{
	*start = LOWORD(m_selection);
	*count = HIWORD(m_selection) - *start;
	(*start)++; //cause its one based

  return TRUE;
	}


BOOL ALMAPI HwndDateItem::SetSelection(
	int										start,
	int 										count)
	{
	int											s, e;

	start--;		 //cause its one-based
	if (start<0)	//make sure start is valid
		start = 0;

	if (count < 0)
		{ //a negitive count just moves the start back
		count = -count;
		if (count > start)
			count = start;
		start -= count;
		}

	s = start;
  	e = s + count;

	m_selection = MAKELONG ((short)s, (short)e);

	if (IsWindow(m_hwndItem))
		SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection); 

  return TRUE;
	}



long ALMAPI HwndDateItem::Activate(int causedBy,	BOOL	mustKeep)
{
	LPDate	date;

  I_UNUSED(mustKeep);

	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::Activate", return OWND_ACTIVATION_PASS);

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "HwndDateItem::Activate", return OWND_ACTIVATION_KEPT);

	if (causedBy != OWND_ACTIVATED_BY_MOUSE)
 	{
		if (date->SelectsAll() )
//			SendMessage(m_hwndItem, EM_SETSEL, 0, MAKELONG (0, -1) );
			Edit_SetSel(m_hwndItem, 0, MAKELONG(0, -1));
		else
//			SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection);
			Edit_SetSel(m_hwndItem, 0, m_selection);

	}

	if (GetFocus() != m_hwndItem)
		SetFocus(m_hwndItem);


	UnlockDateObject(Object());

	return OWND_ACTIVATION_KEPT;
}

void ALMAPI HwndDateItem::Deactivated()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::Deactivated", return);

	//Get the position of the cursor
//	m_selection = SendMessage(m_hwndItem, EM_GETSEL, 0, 0L); 
	m_selection = Edit_GetSel(m_hwndItem);

}

LRESULT ALMAPI HwndDateItem::ParentNotification(HWND hwnd, UINT	message, WPARAM	wParam, LPARAM lParam)
{
  LPARAM rVal;

	switch(message)
	{
    	case WM_COMMAND: 
		{
			WORD  nc = GET_WM_COMMAND_CMD(wParam, lParam);

//			switch(HIWORD(lParam))
			switch(nc)
			{
				case EN_SETFOCUS:
					AEvtPostSignalAtTail(Object(), DT_SignalActivated);
	  				break;

				case EN_KILLFOCUS:
          			MaybePostChanged();
          			AEvtPostSignalAtTail(Object(), DT_SignalDeactivated);
  					break;
			}
      		rVal = DefWindowProc(hwnd, message, wParam, lParam);
			break;
		}

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

/*****************************************************/
/* Function called by the wndProc  									 */
/*****************************************************/
LRESULT HwndDateItem::WmSetCursor(HWND hwnd, UINT	message, WPARAM wParam,	LPARAM lParam)
{
	LPDate date;
	BOOL	 isEditable;

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "HwndDateItem::WmSetCursor", return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam));

	isEditable = date->IsEditable();
	UnlockDateObject(Object());

	if (!isEditable)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
}

LRESULT HwndDateItem::WmEraseBkgnd(HWND	hwnd,	UINT message,	WPARAM wParam, LPARAM lParam)
{
	HDC	 hdc;
	RECT rect;

	I_UNUSED(message);
	I_UNUSED(lParam);
	
	hdc = (HDC)wParam;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, m_attributes.AttrGetBrushHandle() );

	return TRUE;
}

LRESULT HwndDateItem::WmChar(HWND	hwnd,	UINT message,	WPARAM wParam, LPARAM	lParam)
{
  LRESULT	 rVal;


	if (wParam == VK_RETURN)
  {
    MaybePostChanged();
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

void HwndDateItem::MaybePostChanged
  (
  )

  {
  LPDate  date = LockDateObject(Object());

  I_ASSERT_DATE(date, "HwndDateItem::MaybePostChanged", return);

  if(!date->HasChanged())
    {
    int   nLen    = GetWindowTextLength(m_hwndItem);
    char* szText  = new char[nLen + 1];

    if(szText)
      {
      GetWindowText(m_hwndItem, szText, nLen);
      if(lstrcmp(date->GetDateStringPtr(), szText))
        SetWindowText(m_hwndItem, date->GetDateStringPtr());

      delete szText;
      }
    }
     
  date->MaybePostChangedStuff();
  UnlockDateObject(Object());
  }

/*****************************************************/
/*  This is the callback function for the 					 */
/*  subclassed edit control													 */
//  for the Date object  													 */
/*****************************************************/
LRESULT CALLBACK AppwareDateObjectProc(HWND	hwnd,	UINT message,	WPARAM wParam, LPARAM	lParam)
{
	HwndDateItem*		hdi;
  LPCREATESTRUCT	cr;

	switch(message)
	{
		case WM_CREATE:
			cr = (LPCREATESTRUCT)lParam;
			hdi =(HwndDateItem*)cr->lpCreateParams;
			SetThisPointer(hwnd, hdi);
			break;

		case WM_ERASEBKGND:
			hdi = GetThisPointer(hwnd);
			I_ASSERT_HDI(hdi, "AppwareTextObjectProc", break);
			return hdi->WmEraseBkgnd(hwnd, message, wParam, lParam);

		case WM_CHAR:
			hdi = GetThisPointer(hwnd);
			I_ASSERT_HDI(hdi, "AppwareTextObjectProc", break);
			return hdi->WmChar(hwnd, message, wParam, lParam);
	}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
}

void ALMAPI HwndDateItem::Cut()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::Cut", return);

	SendMessage(m_hwndItem, WM_CUT, 0, 0L);
	UpdateIfModified();
}

/////////////////////////////////

void ALMAPI HwndDateItem::Copy()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::Copy", return);

	SendMessage(m_hwndItem, WM_COPY, 0, 0L);
}

/////////////////////////////////

void ALMAPI HwndDateItem::Paste()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::Paste", return);

	SendMessage(m_hwndItem, WM_PASTE, 0, 0L);
	UpdateIfModified();
}

/////////////////////////////////

void ALMAPI HwndDateItem::Undo()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndDateItem::Undo", return);

	SendMessage(m_hwndItem, EM_UNDO, 0, 0L);
	UpdateIfModified();
}

static void PclDrawRect(HDC	hdc, LPRECT 	rect)
{
	PRECT_STRUCT				prect;

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


void ALMAPI HwndDateItem::Print(HDC	hdc, 	RECT*	printRect, int pclPrinter)
{
	ATTRIBUTES							attPrinter(&m_attributes);
	LPDate                  date;
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

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "HwndDateItem::Print", return);

	hBrush = CreateSolidBrush(back);
	hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
	attPrinter.AttrCreateFont(hdc);
	hFontOld = attPrinter.AttrSelectFont(hdc);
	oldFore	= SetTextColor(hdc, fore);
	oldBack = SetBkColor(hdc, back);
	oldMode = SetBkMode(hdc, TRANSPARENT);
	
	if (date->HasFrame())
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

	drawFlag = date->GetDrawDateStyle();

  //Draw the text
	DrawText(hdc, date->GetDateStringPtr(), -1, printRect, drawFlag);

  //Restore the hdc
	SetBkMode(hdc, oldMode);
	SetBkColor(hdc, oldBack);
	SetTextColor(hdc, oldFore);
	SelectObject(hdc, hFontOld);
	attPrinter.AttrDeleteFont();
	SelectObject(hdc, hBrushOld);
	DeleteObject(hBrush);

	UnlockDateObject(Object());
}

//////////////////////////////////////
// This function will check and see if the edit control
// has been modified.  If it has, it will post the necessary
// signals and return TRUE.  Otherwise, it will return FALSE
//////////////////////////////////////
BOOL HwndDateItem::UpdateIfModified()
{
	LPDate	 date;
	char		 str[DT_STRING_LEN];
	BOOL		 rVal = FALSE;

//	if (SendMessage(m_hwndItem, EM_GETMODIFY, 0, 0L))
	if (Edit_GetModify(m_hwndItem))
	{
		SendMessage(m_hwndItem, EM_SETMODIFY, 0, 0L);  //Clear the modified bit
		Edit_SetModify(m_hwndItem, 0);    //Clear the modified bit

		GetWindowText(m_hwndItem, str, DT_STRING_LEN-1);
		
		date = LockDateObject(Object());
		I_ASSERT_DATE(date, "HwndDateItem::UpdateIfModified", return FALSE);
		date->SetDateValue(str, DONTPOSTSGN, DONTBLANKOUT);
		UnlockDateObject(Object());

		rVal = TRUE;
	}

	return rVal;
}	 

void HwndDateItem::ChangeFont(LPATTRIBUTES newAttr,	long changeFlag)
{
	HDC	 hdc;

	if (m_hwndItem)
	{ //If our window handle is created, the font and brush handles will be too.
		hdc = GetDC(m_hwndItem);
		m_attributes.AttrChangeFontIndirect(hdc, m_hwndItem, changeFlag, newAttr);
		ReleaseDC(m_hwndItem, hdc);
	}
	else
		m_attributes.AttrChangeFontIndirect(NULL, NULL, changeFlag, newAttr);
}

BOOL CALLBACK HwndDateItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag)
{
	((HwndDateItem *)hwi)->ChangeFont((LPATTRIBUTES)attrPtr, changeFlag);
	return TRUE;
}

void HwndDateItem::ForceRedraw()
{
	AMEMBLOCKID	miCurrentString;
	LPSTR				szCurrentString;
	int         len;
	LPDate      date;

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "HwndDateItem::ForceRedraw", return;);

	len = GetWindowTextLength(m_hwndItem)+1;
	miCurrentString = AMemAllocate(len);
	szCurrentString = (LPSTR)AMemLock(miCurrentString);
	GetWindowText(m_hwndItem, szCurrentString, len);

	if (lstrcmp(date->GetDateStringPtr(), szCurrentString) )
	{
		if (OWndGetActiveItem() == this)
		{
//			m_selection = SendMessage(m_hwndItem, EM_GETSEL, 0, 0L); 
			m_selection = Edit_GetSel(m_hwndItem);
			SetWindowText(m_hwndItem, date->GetDateStringPtr());
//			SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection);
			Edit_SetSel(m_hwndItem, 0, m_selection); 
		}
		else
			SetWindowText(m_hwndItem, date->GetDateStringPtr());
	}

	AMemUnlock(miCurrentString);
	AMemFree(miCurrentString);

	UnlockDateObject(Object());
}

BOOL CALLBACK HwndDateItemForceRedraw(OpWndItemD*	hwi, long	notused, long	notusedEither)
{
	I_UNUSED(notused);I_UNUSED(notusedEither);

	((HwndDateItem *)hwi)->ForceRedraw();
	return TRUE;
}

