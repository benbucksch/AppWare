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
// FILE: tm_hti.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Hwnd'd time items
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "tm_incld.h"
#include <windowsx.h>

LRESULT CALLBACK AppwareTimeObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HwndTimeItem::HwndTimeItem(AOBJECTID	oiItem,	AOBJECTID oiParent, RECT* rcItem, long	status) :	 OpHwndWndItem(oiItem, oiParent, rcItem, status)
{
	LPTime time;

	time = LockTimeObject(Object());
	I_ASSERT_TIME(time, "HwndTimeItem::HwndTimeItem", ;);

	time->CopyAttributes(&m_attributes);
	m_selection = time->SelectsAll() ? MAKELONG(0,-1) : 0L;

	time->FormatTM();

	OWndSetItemActivatable(this, time->IsActivatable());
	OWndSetItemEnabled(this, TRUE);
	OWndSetItemTransparent(this, FALSE);
	OWndSetItemUsesCursorKeys(this, TRUE);
	OWndSetItemUsesTab(this, FALSE);
	OWndSetItemUsesDefaultKey(this, FALSE);
	OWndSetItemUsesCancelKey(this, FALSE);

	UnlockTimeObject(Object());
}

void ALMAPI HwndTimeItem::Destroy()
{
  delete this;
}

void ALMAPI HwndTimeItem::DataChanged(long details,	long modifiers, long variation)
{
	I_UNUSED(details);	
	I_UNUSED(modifiers);	
	I_UNUSED(variation);

	if (m_hwndItem && m_hwndItem != (HWND)modifiers) // No need to change the one that triggered the change!!
		ForceRedraw();
}


void ALMAPI HwndTimeItem::ParentOpened()
{
	LPTime time;
	HDC		 hdc;

	time = LockTimeObject(Object());
	I_ASSERT_TIME(time, "HwndTimeItem::ParentOpened", return);

	m_hwndItem = OWndCreateWindowEx(
					this,
					0,
					g_timeObjectClassName,
					time->GetTimeStringPtr(),
					time->GetCreateWindowStyle(),
					g_hInstance,
					this);

	UnlockTimeObject(Object());

	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::ParentOpened", ;);

	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrCreateBrush();
	m_attributes.AttrSetFont(m_hwndItem);

}

void ALMAPI HwndTimeItem::ParentClosing()
{

	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::ParentClosing", return);

	m_attributes.AttrDeleteFont();
	m_attributes.AttrDeleteBrush();
	DestroyWindow(m_hwndItem);
	m_hwndItem = 0;
}

BOOL ALMAPI HwndTimeItem::GetSelection(
	int * 									start,
	int * 									count)
	{
	*start = LOWORD(m_selection);
	*count = HIWORD(m_selection) - *start;
	(*start)++; //cause its one based

  return TRUE;
	}


BOOL ALMAPI HwndTimeItem::SetSelection(
	int										start,
	int 										count)
	{
	int										s, e;

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

	s = start;
  	e = s + count;

	m_selection = MAKELONG ((short)s, (short)e);

	if (IsWindow(m_hwndItem))
//		SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection); 
		Edit_SetSel(m_hwndItem, 0, m_selection);

  return TRUE;
	}


long ALMAPI HwndTimeItem::Activate(int causedBy,	BOOL	mustKeep)
{
	LPTime	time;

  I_UNUSED(mustKeep);

	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::Activate", return OWND_ACTIVATION_PASS);

	time = LockTimeObject(Object());
	I_ASSERT_TIME(time, "HwndTimeItem::Activate", return OWND_ACTIVATION_KEPT);

	if (causedBy != OWND_ACTIVATED_BY_MOUSE)
 	{
		if (time->SelectsAll() )
//			SendMessage(m_hwndItem, EM_SETSEL, 0, MAKELONG (0, -1) ); 
			Edit_SetSel(m_hwndItem, 0, MAKELONG(0, -1));

		else
//			SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection); 
			Edit_SetSel(m_hwndItem, 0, m_selection);
	}

	if (GetFocus() != m_hwndItem)
		SetFocus(m_hwndItem);

	UnlockTimeObject(Object());

	return OWND_ACTIVATION_KEPT;
}

void ALMAPI HwndTimeItem::Deactivated()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::Deactivated", return);

	//Get the position of the cursor
//	m_selection = SendMessage(m_hwndItem, EM_GETSEL, 0, 0L); 
	m_selection = Edit_GetSel(m_hwndItem);

}

LRESULT ALMAPI HwndTimeItem::ParentNotification(HWND hwnd, UINT	message, WPARAM	wParam, LPARAM lParam)
{
  LPARAM rVal;

	switch(message)
	{
    	case WM_COMMAND: 
		{
			WORD nc = GET_WM_COMMAND_CMD(wParam, lParam);

			switch(nc)
			{
				case EN_SETFOCUS:
					AEvtPostSignalAtTail(Object(), TM_SignalActivated);
	  				break;

				case EN_KILLFOCUS:
			        MaybePostChanged();
					AEvtPostSignalAtTail(Object(), TM_SignalDeactivated);
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
LRESULT HwndTimeItem::WmChar(HWND	hwnd,	UINT message,	WPARAM wParam, LPARAM	lParam)
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


LRESULT HwndTimeItem::WmSetCursor(HWND hwnd, UINT	message, WPARAM wParam,	LPARAM lParam)
{
	LPTime time;
	BOOL	 isEditable;

	time = LockTimeObject(Object());
	I_ASSERT_TIME(time, "HwndTimeItem::WmSetCursor", return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam));

	isEditable = time->IsEditable();
	UnlockTimeObject(Object());

	if (!isEditable)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
}

LRESULT HwndTimeItem::WmEraseBkgnd(HWND	hwnd,	UINT message,	WPARAM wParam, LPARAM lParam)
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

void HwndTimeItem::MaybePostChanged
  (
  )

  {
  LPTime time = LockTimeObject(Object());
  I_ASSERT_TIME(time, "HwndTimeItem::MaybePostChanged", return);

  if(!time->HasChanged())
    {
    int   nLen    = GetWindowTextLength(m_hwndItem);
    char* szText  = new char[nLen + 1];

    if(szText)
      {
      GetWindowText(m_hwndItem, szText, nLen);
      if(lstrcmp(time->GetTimeStringPtr(), szText))
        SetWindowText(m_hwndItem, time->GetTimeStringPtr());

      delete szText;
      }
    }

  time->MaybePostChangedStuff();
  UnlockTimeObject(Object());
  }

/*****************************************************/
/*  This is the callback function for the 					 */
/*  subclassed edit control													 */
//  for the number object  													 */
/*****************************************************/
LRESULT CALLBACK AppwareTimeObjectProc(HWND	hwnd,	UINT message,	WPARAM wParam, LPARAM	lParam)
{
	HwndTimeItem*		hni;
  LPCREATESTRUCT	cr;

	switch(message)
	{
		case WM_CREATE:
			cr = (LPCREATESTRUCT)lParam;
			hni =(HwndTimeItem*)cr->lpCreateParams;
			SetThisPointer(hwnd, hni);
			break;

		case WM_ERASEBKGND:
			hni = GetThisPointer(hwnd);
			I_ASSERT_HTI(hni, "AppwareTextObjectProc", break);
			return hni->WmEraseBkgnd(hwnd, message, wParam, lParam);

		case WM_CHAR:
			hni = GetThisPointer(hwnd);
			I_ASSERT_HTI(hni, "AppwareTextObjectProc", break);
			return hni->WmChar(hwnd, message, wParam, lParam);
	}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
}

void ALMAPI HwndTimeItem::Cut()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::Cut", return);

	SendMessage(m_hwndItem, WM_CUT, 0, 0L);
	UpdateIfModified();
}

/////////////////////////////////

void ALMAPI HwndTimeItem::Copy()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::Copy", return);

	SendMessage(m_hwndItem, WM_COPY, 0, 0L);
}

/////////////////////////////////

void ALMAPI HwndTimeItem::Paste()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::Paste", return);

	SendMessage(m_hwndItem, WM_PASTE, 0, 0L);
	UpdateIfModified();
}

/////////////////////////////////

void ALMAPI HwndTimeItem::Undo()
{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTimeItem::Undo", return);

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


void ALMAPI HwndTimeItem::Print(HDC	hdc, 	RECT*	printRect, int pclPrinter)
{
	ATTRIBUTES							attPrinter(&m_attributes);
	LPTime                  time;
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

	time = LockTimeObject(Object());
	I_ASSERT_TIME(time, "HwndTimeItem::Print", return);

	hBrush = CreateSolidBrush(back);
	hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
	attPrinter.AttrCreateFont(hdc);
	hFontOld = attPrinter.AttrSelectFont(hdc);
	oldFore	= SetTextColor(hdc, fore);
	oldBack = SetBkColor(hdc, back);
	oldMode = SetBkMode(hdc, TRANSPARENT);
	
	if (time->HasFrame())
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

	drawFlag = time->GetDrawTimeStyle();

  //Draw the text
	DrawText(hdc, time->GetTimeStringPtr(), -1, printRect, drawFlag);

  //Restore the hdc
	SetBkMode(hdc, oldMode);
	SetBkColor(hdc, oldBack);
	SetTextColor(hdc, oldFore);
	SelectObject(hdc, hFontOld);
	attPrinter.AttrDeleteFont();
	SelectObject(hdc, hBrushOld);
	DeleteObject(hBrush);

	UnlockTimeObject(Object());
}


//////////////////////////////////////
// This function will check and see if the edit control
// has been modified.  If it has, it will post the necessary
// signals and return TRUE.  Otherwise, it will return FALSE
//////////////////////////////////////
BOOL HwndTimeItem::UpdateIfModified()
{
	LPTime	 time;
	char		 str[TM_STRING_LEN];
	BOOL		 rVal = FALSE;

	if (SendMessage(m_hwndItem, EM_GETMODIFY, 0, 0L))
	{
		SendMessage(m_hwndItem, EM_SETMODIFY, 0, 0L);  //Clear the modified bit

		GetWindowText(m_hwndItem, str, TM_STRING_LEN-1);
		
		time = LockTimeObject(Object());
		I_ASSERT_TIME(time, "HwndTimeItem::UpTimeIfModified", return FALSE);
		time->SetTimeValue(str, DONTPOSTSGN, DONTBLANKOUT);
		UnlockTimeObject(Object());

		rVal = TRUE;
	}

	return rVal;
}	 
	 
void HwndTimeItem::ChangeFont(LPATTRIBUTES newAttr,	long changeFlag)
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

BOOL CALLBACK HwndTimeItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag)
{
	((HwndTimeItem *)hwi)->ChangeFont((LPATTRIBUTES)attrPtr, changeFlag);
	return TRUE;
}

void HwndTimeItem::ForceRedraw()
{
	LPTime 	time;
	AMEMBLOCKID							miCurrentString;
	LPSTR										szCurrentString;
	int                     len;

	time = LockTimeObject(Object());
	I_ASSERT_TIME(time, "HwndTimeItem::ForceRedraw", return;);

	len = GetWindowTextLength(m_hwndItem)+1;
	miCurrentString = AMemAllocate(len);
	szCurrentString = (LPSTR)AMemLock(miCurrentString);
	GetWindowText(m_hwndItem, szCurrentString, len);

	if (lstrcmp(time->GetTimeStringPtr(), szCurrentString) )
	{
		if (OWndGetActiveItem() == this)
		{
//			m_selection = SendMessage(m_hwndItem, EM_GETSEL, 0, 0L); 
			m_selection = Edit_GetSel(m_hwndItem);

			SetWindowText(m_hwndItem, time->GetTimeStringPtr());

//			SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection); 
			Edit_SetSel(m_hwndItem, 0, m_selection);

		}
		else
			SetWindowText(m_hwndItem, time->GetTimeStringPtr());
	}

	AMemUnlock(miCurrentString);
	AMemFree(miCurrentString);

	UnlockTimeObject(Object());
}

BOOL CALLBACK HwndTimeItemForceRedraw(OpWndItemD*	hwi, long	notused, long	notusedEither)
{
	I_UNUSED(notused);I_UNUSED(notusedEither);

	((HwndTimeItem *)hwi)->ForceRedraw();
	return TRUE;
}

