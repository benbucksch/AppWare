/*************************************************************************
**	File Name:
**		TLINVISU.CPP			Copyright 1994 Novell Inc.
**
**	Description:
**		The Visual Instance methods for the Telephony Line Window Object.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

#define STRICT
#define ASTRICT

#include <string.h>
#include <a_alm.h>
#include <o_text.h>
#include <windowsx.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"
#include "tlinvisu.h"
#include "o_tlin.h"
#include "o_tdev.h"

static void DrawBitmap(HDC hdc, HBITMAP hBitmap, short xOrigin,
	short yOrigin, short xSize, short ySize, short xDest, short yDest);
static void LoadListBox(HWND hwndList, OBJECTID oiTLine);

/*************************************************************************
**	Function:
**		TLineCreate 				(AppWare Bus)
**
**	Description:
**		A pointer to this function is passed to the AppWare Bus
**		when the TLine Window Object's Administration routine
**		gets the AOBJ_QUERYPROTOCOL message and the protocol type is
**		APROT_WND.
**
**			pQueryInfo->pfnCreate = (APPCFN)TLineCreate;
**
**		This function is called:
**			During window layout at 'Design Time'
**			During the windows object's startup event
**				response at 'Run Time'
**
**		Note: This function is much more complicated for Window
**			items written in 'C' because the 'Virtual Table'
**			containing pointers to all of the window item protocol
**			functions must be built manually.
*/

OpWndItemD * ALMAPI TLineCreate(OBJECTID oiItem, OBJECTID oiParent,
	RECT * rcItem, long status, long /* special */ ) {
	return new TLineVI(oiItem, oiParent, rcItem, status);
}

/*************************************************************************
**	Function:
**		TLineVI (Constructor)		(AppWare Bus)
**
**	Description:
**		Constructor for the TLine Window Object's Visual Instance
**		class. This routine is called from TLineCreate or anytime
**		that a new instance of this class needs to be initialized.
**		The base class of this hwnd'd object is OpHwndWndItem.
*/

TLineVI::TLineVI(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem,
	long  status) : OpHwndWndItem(oiItem, oiParent, rcItem, status) {
	hBmpBtns = LoadBitmap(hInstance, (LPCSTR)"Buttons");
	hBmpLgts = LoadBitmap(hInstance, (LPCSTR)"Lights");
	OWndSetItemActivatable(this, TRUE);
}


/*************************************************************************
**	Function:
**		Destroy 					(AppWare Bus)
**
**	Description:
**		Destroy function for this visual instance.
*/

void ALMAPI TLineVI::Destroy(void) {
	DeleteObject(hBmpBtns);
	DeleteObject(hBmpLgts);
	delete this;
	return;
}

/*************************************************************************
**	Function:
**		DataChanged 				(AppWare Bus)
**
**	Description:
**		Called by the parent Window Object when the TLine Object's
**		appearance has changed. A better implimentation would pass
**		information about what changed in the details, modifiers, or
**		variation fields of the 'objectChanged' event.
*/

#pragma argsused
void ALMAPI TLineVI::DataChanged(long details, long modifiers, long variation) {
	RECT rect;

	if (details & TLINE_REDLED) {
		rect.top = 4; rect.left = 2; rect.bottom = 16; rect.right = 14;
		InvalidateRect(m_hwndItem, &rect, FALSE);
	}
	if (details & TLINE_GREENLED) {
		rect.top = 16; rect.left = 2; rect.bottom = 28; rect.right = 14;
		InvalidateRect(m_hwndItem, &rect, FALSE);
	}
	if (details & TLINE_LINELETTER) {
		rect.top = 3; rect.left = 19; rect.bottom = 16; rect.right = 9;
		InvalidateRect(m_hwndItem, NULL, TRUE);
	}
	if (details & TLINE_DEVICEID) {
		GetClientRect(m_hwndItem, &rect);
		rect.top = 3; rect.left = 30; rect.bottom = 16; rect.right -= 20;
		InvalidateRect(m_hwndItem, NULL, TRUE);
	}
	if (details & TLINE_CALLID) {
		GetClientRect(m_hwndItem, &rect);
		rect.top = 16; rect.left = 19; rect.bottom = 28; rect.right -= 20;
		InvalidateRect(m_hwndItem, NULL, TRUE);
	}
	return;
}

/*************************************************************************
**	Function:
**		Activate					(AppWare Bus)
**
**	Description:
**		Called by the parent Window Object when this visual instance of
**		the TLine Object is to be activated (typically in response to
**		the tab key or a mouse click). Activation is indicated by a black
**		frame around the object (in the case of this object).
*/

long ALMAPI TLineVI::Activate(int, BOOL) {
	HBRUSH	hBrush;
	RECT	rcClient;
	HPEN	hPen;
	HDC 	hDC;

//	Post a "TLine Activated signal"
	AEvtPostSignalAtTail(m_oiObject, s_TLineActivated);

//	Create a pen to draw a black, hollow frame around the object
	hDC = GetDC(m_hwndItem);
	hBrush = SelectBrush(hDC, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
	hPen = SelectPen(hDC, (HPEN)GetStockObject(BLACK_PEN));
	GetClientRect(m_hwndItem, &rcClient);
	Rectangle(hDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

//	Deselect the brush and pen from the display context before releasing it
	SelectBrush(hDC, hBrush);
	SelectPen(hDC, hPen);
	ReleaseDC(m_hwndItem, hDC);

//	Call SetFocus to get keyboard input
	SetFocus(m_hwndItem);
	return OWND_ACTIVATION_KEPT;
}

/*************************************************************************
**	Function:
**		Deactivate					(AppWare Bus)
**
**	Description:
**		Called by the parent Window Object when this visual instance of
**		the TLine Object is to be deactivated (typically in response
**		to the tab key or a mouse click). Deactivation is indicated by
**		matching the frame color to the background of the TLine box.
*/

void ALMAPI TLineVI::Deactivated(void) {
	HBRUSH	hBrush;
	RECT	rcClient;
	HPEN	hPen;
	HDC 	hDC;

//	Post a "TLine Deactivated signal"
	AEvtPostSignalAtTail(m_oiObject, s_TLineDeactivated);

//	Hide Drop-down box if it's dropped-down
	if (fDropped) {
		fDropped = FALSE;
		ShowWindow(hwndList, SW_HIDE);
	}

//	Draw a white, hollow frame around the object
	hDC = GetDC(m_hwndItem);
	hBrush = SelectBrush(hDC, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
	hPen = SelectPen(hDC, (HPEN)GetStockObject(WHITE_PEN));
	GetClientRect(m_hwndItem, &rcClient);
	Rectangle(hDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

//	Deselect the brush and pen from the display context before releasing it
	SelectBrush(hDC, hBrush);
	SelectPen(hDC, hPen);
	ReleaseDC(m_hwndItem, hDC);
	return;
}

/*************************************************************************
**	Function:
**		Draw						(AppWare Bus)
**
**	Description:
**		Called by TLineWndProc in response to the WM_PAINT message.
**		For non-hwnd'd window items, the parent window object will call
**		their Draw routines when the parent window object determines that
**		they need to be drawn. Non-hwnd'd window items will use their
**		m_rcItem data member to determine the drawing location in the
**		parent window object rather than rcClient.
*/

void TLineVI::Draw(void) {
	LPTLINE 	lpTLine;
	PAINTSTRUCT ps;
	HBRUSH		hBrush;
	RECT		rcClient, rcLight, rcText;
	HPEN		hPen;
	HFONT		hFont;
	HDC 		hDC;
	char		szBuf[64];

//	Get a handle to the display context
	hDC = BeginPaint(m_hwndItem, &ps);

//	Get the client rectangle area
	GetClientRect(m_hwndItem, &rcClient);

//	Draw the TLine Object's frame
	hBrush = SelectBrush(hDC, (HBRUSH)GetStockObject(WHITE_BRUSH));
	hPen = SelectPen(hDC, (HPEN)GetStockObject(BLACK_PEN));

//	If this TLine Object has the focus then draw it's focus frame in black.
	if (OWndGetActiveItem() == this)
		Rectangle(hDC, rcClient.left, rcClient.top, rcClient.right,
			rcClient.bottom);

//	Fill the rectangle with white
	SelectPen(hDC, (HPEN)GetStockObject(WHITE_PEN));
	Rectangle(hDC, rcClient.left + 1, rcClient.top + 1,
		rcClient.right - 1, 28);

//	Draw the TLine Object's inner frame in black
	SelectPen(hDC, (HPEN)GetStockObject(BLACK_PEN));
	MoveToEx(hDC, 1, 27); 	// don't ask me why this has to be 27!!!
	LineTo(hDC, 1,	1);
	LineTo(hDC, rcClient.right - 2,  1);
	LineTo(hDC, rcClient.right - 2, 28);

//	Draw internal lines
	MoveToEx(hDC, 2, 2);
	LineTo(hDC, rcClient.right - 2, 2);
	MoveToEx(hDC, 2, 3);
	LineTo(hDC, rcClient.right - 2, 3);
	MoveToEx(hDC, 14, 4);
	LineTo(hDC, 14, 28);
	MoveToEx(hDC, 15, 4);
	LineTo(hDC, 15, 28);
	MoveToEx(hDC, 16, 4);
	LineTo(hDC, 16, 28);
	MoveToEx(hDC, 17, 4);
	LineTo(hDC, 17, 28);

//	Restore original brush
	SelectBrush(hDC, hBrush);

//	Get a pointer to the object's data block
	lpTLine = (LPTLINE)AObjLockData(m_oiObject, 0);

//	Draw LED displays according to display flags in data structure
	DrawBitmap(hDC, hBmpLgts, (lpTLine->fRedLEDActv && lpTLine->fActiveLine)?
		0 : 12,  0, 12, 12, 2, 4);
	DrawBitmap(hDC, hBmpLgts, (lpTLine->fGreenLEDActv && lpTLine->fCallInEffect)?
		0 : 12, 12, 12, 12, 2, 16);

//	Draw drop down button on right end of the TLine Object
	DrawBitmap(hDC, hBmpBtns, fButtonPressed? 18 : 0, 0, 18, 25, rcClient.right - 20, 3);

//	Select background mode, text color, and font
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(0, 0, 0));
	hFont = SelectFont(hDC, (HFONT)GetStockObject(SYSTEM_FONT));

	TextOut(hDC, 19, 3, (LPCSTR)lpTLine->szLineChar, 1);
	*szBuf = 0;
	if (lpTLine->oiPhone)
		OTextGetToBuffer(lpTLine->oiPhone, szBuf, sizeof(szBuf));
	TextOut(hDC, 30, 3, (LPCSTR)szBuf, lstrlen(szBuf));

	SelectFont(hDC, (HFONT)GetStockObject(ANSI_VAR_FONT));

	TextOut(hDC, 19, 16, (LPCSTR)"CID:", 4);
	if (lpTLine->lCallID)
		wsprintf(szBuf, "%ld", lpTLine->lCallID);
	else
		lstrcpy(szBuf, "(No Call)");

	TextOut(hDC, 19 + LOWORD(GetTextExtentPoint32(hDC, (LPCSTR)"CID: ", 5)), 16,
		(LPCSTR)szBuf, lstrlen(szBuf));

	AObjUnlockData(m_oiObject, 0);

//	Reselect original brush, font, and pen in hDC
	SelectFont(hDC, hFont);
	SelectPen(hDC, hPen);

	EndPaint(m_hwndItem, &ps);
	return;
}

/*************************************************************************
**	Function:
**		ParentOpened				(AppWare Bus)
**
**	Description:
**		Called by the parent window object when that window opens.
**		The window used to display this visual instance of the TLine
**		Object and receive WM_XXX messages is created.
*/

void ALMAPI TLineVI::ParentOpened() {
	m_hwndItem = OWndCreateWindowEx(this, 0, TLINE_CLASSNAME, "", WS_CHILD,
		hInstance, this);
	return;
}

/*************************************************************************
**	Function:
**		ParentClosing				(AppWare Bus)
**
**	Description:
**		Called by the parent window object when that window is about to
**		be closed.
*/

void ALMAPI TLineVI::ParentClosing() {
	DestroyWindow(m_hwndItem);
	m_hwndItem = NULL;
	return;
}

/*************************************************************************
**	Function:
**		TLineWndProc				(Windows Kernal)
**
**	Description:
**		This is the TLine Window's callback procedure, which responds
**		to Windows messages such as WM_CREATE and WM_PAINT.
*/

LONG CALLBACK TLineWndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam) {
	CREATESTRUCT  *	cs;
	TLineVI  *		vi;
	RECT				rc;
	HDC 				hDC;

	switch(msg) {
		case WM_CREATE:
			cs = (CREATESTRUCT  *)lParam;
			vi = (TLineVI  *)cs->lpCreateParams;
			vi->m_hwndItem = hwnd;
			vi->nListHeight = cs->cy - 29;
			vi->fDropped = FALSE;
			vi->fButtonPressed = FALSE;
			SetWindowLong(hwnd, 0, (long)vi);
			GetWindowRect(hwnd, &rc);

		//	Create the drop down list box as a child of this window so
		//	the child window will record where to send parent messages.
		//	Then reset the window to a child of the desktop so that it
		//	may be seen beyond the boundaries of the "parent" window.

			vi->hwndList = CreateWindowEx(0, "ComboLBox", NULL, 
				WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_VSCROLL |
				LBS_NOTIFY | LBS_HASSTRINGS | 0x8000,	/* don't ask! */  
				rc.left, rc.top + 29, rc.right - rc.left - 2,
				vi->nListHeight, hwnd, (HMENU)0x350, hInstance, 0);

			SetParent(vi->hwndList, HWND_DESKTOP);
			SetWindowPos(hwnd, 0, 0, 0, cs->cx, 29, SWP_NOACTIVATE |
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW);
			break;

		case WM_LBUTTONDOWN:
			vi = (TLineVI  *)GetWindowLong(hwnd, 0);
			SetCapture(hwnd);
			GetClientRect(hwnd, &rc);
			OTLinSetActiveLine(vi->m_oiObject);
			if (LOWORD(lParam) >= rc.right - 20) {	//	Button Pressed
				vi->fButtonPressed = TRUE;
				hDC = GetDC(hwnd);
				DrawBitmap(hDC, vi->hBmpBtns, 18, 0, 18, 25, rc.right - 20, 3);
				ReleaseDC(hwnd, hDC);
			}
			break;

		case WM_LBUTTONUP:
			vi = (TLineVI  *)GetWindowLong(hwnd, 0);
			ReleaseCapture();
			GetClientRect(hwnd, &rc);
			if (vi->fButtonPressed) {
				hDC = GetDC(hwnd);
				DrawBitmap(hDC, vi->hBmpBtns, 0, 0, 18, 25, rc.right - 20, 3);
				ReleaseDC(hwnd, hDC);
			}
			vi->fButtonPressed = FALSE;
			if (LOWORD(lParam) >= rc.right - 20 &&
				LOWORD(lParam) <= rc.right &&
				HIWORD(lParam) >= rc.top &&
				HIWORD(lParam) <= rc.bottom) {		//	Do Button Stuff
				GetWindowRect(hwnd, &rc);
				vi->fDropped = !vi->fDropped;
				if (vi->fDropped) {
					LoadListBox(vi->hwndList, vi->m_oiObject);
					SetWindowPos(vi->hwndList, HWND_TOPMOST,
						rc.left, rc.top + 28, 0, 0, SWP_NOSIZE);
					ShowWindow(vi->hwndList, SW_SHOW);
				}
				else
					SetWindowPos(vi->hwndList, HWND_NOTOPMOST, 0, 0, 0, 0,
						SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
			}
			break;

		case WM_ACTIVATE: //!!PORT!!
			vi = (TLineVI  *)GetWindowLong(hwnd, 0);
			SetActiveWindow(vi->hwndList);
			break;

		case WM_COMMAND: //!!PORT!!
			switch(wParam) {
				case 0x350:							// listbox
					switch (HIWORD(lParam)) {
						case LBN_DBLCLK:
						case LBN_ERRSPACE:
						case LBN_KILLFOCUS:
						case LBN_SELCANCEL:
						case LBN_SELCHANGE:
						case LBN_SETFOCUS:
							break;
					}
					break;
			}
			break;

		case WM_PAINT:
			vi = (TLineVI  *)GetWindowLong(hwnd, 0);
			vi->Draw();
			break;

		case WM_DESTROY:
			vi = (TLineVI  *)GetWindowLong(hwnd, 0);
			SendMessage(vi->hwndList, WM_CLOSE, 0, 0L);
			break;

        default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
    return 0;
}


/*************************************************************************
**	Function:
**		TLineLBWndProc				(Windows Kernal)
**
**	Description:
**		This is the TLine Window's List Box callback procedure
*/

LONG CALLBACK TLineLBWndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam) {
	CREATESTRUCT  *	cs;
	HWND				hwndOwner, hwndList;
	RECT				rc;

	switch(msg) {
		case WM_CREATE:
			cs = (CREATESTRUCT  *)lParam;
			hwndOwner = (HWND)cs->lpCreateParams;
			SetWindowLong(hwnd, 0, (long)hwndOwner);	//	Save Owner (WI)

			hwndList = CreateWindow("ListBox", NULL,
				LBS_USETABSTOPS | LBS_NOTIFY | LBS_HASSTRINGS |
				WS_CHILD | WS_VSCROLL | WS_VISIBLE,
				0, 0, cs->cx, cs->cy,
				hwnd, (HMENU)0, hInstance, NULL);

			SetWindowLong(hwnd, 4, (long)hwndList);		//	Save Child (LB)

		//	Resize listbox frame (this) to fit integrally sized listbox
			GetWindowRect(hwndList, &rc);
			MoveWindow(hwnd, cs->x, cs->y, cs->cx, rc.bottom - rc.top, FALSE);
			break;

		case WM_SETFOCUS:
            hwndList = (HWND)GetWindowLong(hwnd, 4);
			SetWindowPos(hwndList, hwnd,
				0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetFocus(hwndList);
			break;

		case WM_COMMAND: //!!PORT!!
			switch(wParam) {
				case 0: {			//	pass listbox notification to owner
					hwndOwner = (HWND)GetWindowLong(hwnd, 0);
					SendMessage(hwndOwner, msg, wParam, lParam);
					break;
				}
			}
			break;

		case LB_ADDSTRING:		// 	pass relevant LB commands to listbox
		case LB_DELETESTRING:
		case LB_FINDSTRING:
		case LB_GETCOUNT:
		case LB_GETCURSEL:
		case LB_GETITEMDATA:
		case LB_GETSEL:
		case LB_GETSELCOUNT:
		case LB_GETSELITEMS:
		case LB_GETTEXT:
		case LB_GETTEXTLEN:
		case LB_GETTOPINDEX:
		case LB_INSERTSTRING:
		case LB_RESETCONTENT:
		case LB_SELECTSTRING:
		case LB_SELITEMRANGE:
		case LB_SETCARETINDEX:
		case LB_SETCURSEL:
		case LB_SETITEMDATA:
		case LB_SETSEL:		
		case LB_SETTOPINDEX:	//	then return the return value to owner
			hwndList = (HWND)GetWindowLong(hwnd, 4);
			return SendMessage(hwndList, msg, wParam, lParam);

		default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		DrawBitmap					(Anywhere)
**
**	Description:
**		This function draws a bitmap on the screen.
*/

static void DrawBitmap(HDC hdc, HBITMAP hBitmap, short xOrigin,
	short yOrigin, short xSize, short ySize, short xDest, short yDest) {

	BITMAP	bm;
	HDC 	hdcMem;
	DWORD	dwSize;
	POINT	ptSize, ptOrg;

	hdcMem = CreateCompatibleDC(hdc);
	SelectObject(hdcMem, hBitmap);
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);
	ptSize.x = xSize;
	ptSize.y = ySize;
	DPtoLP(hdc, &ptSize, 1);
	ptOrg.x = xOrigin;
	ptOrg.y = yOrigin;
	DPtoLP(hdcMem, &ptOrg, 1);

	BitBlt(hdc, xDest, yDest, ptSize.x, ptSize.y,
		hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);

	DeleteDC(hdcMem);
	return;
}

                       
static void LoadListBox(HWND hwndList, OBJECTID oiTLine) {
    MEMBLOCKID	mTCxtn;
	DeviceID_t	lpszDeviceID;
    LONG		lKey = TLINE_FIRSTCXTN;
	char		lpszComment[30];
	char		buffer[40];
	int			index;

	SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
	mTCxtn = OTLinGetNextCxtn(oiTLine, &lKey);
    while (mTCxtn) {
		OTCxnGetComment(mTCxtn, lpszComment, 30);
        OTCxnGetDeviceID(mTCxtn, lpszDeviceID);
        wsprintf(buffer, "%s\t%s", lpszDeviceID, lpszComment);
//		index = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)buffer);
//		SendMessage(hwndList, LB_SETITEMDATA, index, (LPARAM)mTCxtn);
        mTCxtn = OTLinGetNextCxtn(oiTLine, &lKey);
	}
	return;
}
