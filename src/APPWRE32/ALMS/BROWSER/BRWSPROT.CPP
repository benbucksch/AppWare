/////////////////////////////////////////////////////////////////////////////
// File Name:  BrwsProt.CPP
//      Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  8/01/92
//
// Description.:
//  Contains the Window Item Protocol functions for the Browser object:
/////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <a_alm.h>
#include "browser.h"

#define NOTSORTABLE 0
#define INDEXED 1
#define SORTABLE 2

#define B_NEW       0
#define B_OPEN      1
#define B_CLOSE     2
#define B_QUIT      3
#define B_FIND      4
#define B_POWER     5
#define B_PREV      6
#define B_NEXT      7
#define B_ALL       8
#define B_FOUND     9
#define B_TEXT      10
#define B_CLEAR     11
#define B_ADD       12
#define B_UPDATE    13
#define B_DELETE    14

#define BWIDTH      54
#define BHEIGHT     38

#define PALETTEWIDTH 125
#define PALETTEHEIGHT 299

void SetFileFlag(AOBJECTID oiBrowser, BOOL flag) ;
BOOL IsFile(AOBJECTID oiBrowser) ;
void ClearCancelledFlag(AOBJECTID oiBrowser) ;
void ClearCancelledFlag(AOBJECTID oiBrowser) ;
BOOL WasCancelled(AOBJECTID oiBrowser) ;
AOBJECTID GetIndexedFieldObject(AOBJECTID oiBrowser) ;
BOOL SendUrgentSignal(AOBJECTID oiBrowser, long signal) ;

int COLUMN[] = { 7, 65 } ;
int ROW[] = { 6, 48, 94, 136, 178, 194, 214, 256 } ;

BrowserButton button[] = {
	{{COLUMN[0], ROW[0], BWIDTH, BHEIGHT}, "New"},
	{{COLUMN[1], ROW[0], BWIDTH, BHEIGHT}, "Open"},
	{{COLUMN[0], ROW[1], BWIDTH, BHEIGHT}, "Close"},
	{{COLUMN[1], ROW[1], BWIDTH, BHEIGHT}, "Quit"},
	{{COLUMN[0], ROW[2], BWIDTH, BHEIGHT}, "Find ?"},
	{{COLUMN[1], ROW[2], BWIDTH, BHEIGHT}, "Find ??"},
	{{COLUMN[0], ROW[3], BWIDTH, BHEIGHT}, "Prev"},
	{{COLUMN[1], ROW[3], BWIDTH, BHEIGHT}, "Next"},
	{{COLUMN[0], ROW[4], BWIDTH, 12}, "All"},
	{{COLUMN[1], ROW[4], BWIDTH, 12}, "Found"},
	{{COLUMN[0], ROW[5], 111, 12}, "Text"},
	{{COLUMN[0], ROW[6], BWIDTH, BHEIGHT}, "Clear"},
	{{COLUMN[1], ROW[6], BWIDTH, BHEIGHT}, "Add"},
	{{COLUMN[0], ROW[7], BWIDTH, BHEIGHT}, "Update"},
	{{COLUMN[1], ROW[7], BWIDTH, BHEIGHT}, "Delete" }
	} ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  CreateBrowser
// DESCRIPTION:  Instantiates a new BrowserVI class
/////////////////////////////////////////////////////////////////////////////
OpWndItemD* CALLBACK /*!!PORT!! was _far_ _pascal_*/  BrowserCreate(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status, long )
	{
	return new BrowserVI(oiItem, oiParent, rcItem, status) ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  BrowserVI
// DESCRIPTION:  BrowserVI class constructor
/////////////////////////////////////////////////////////////////////////////
BrowserVI::BrowserVI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status)
			: OpLiteWndItem(oiItem, oiParent, rcItem, status)
	{
	BrowserInfo * lpBrowser = (BrowserInfo *)AObjLockData(oiItem, 0) ;
	m_oiDatabase = lpBrowser->oiDatabase ;
	AObjUnlockData(oiItem, 0) ;

	POINT browserSize ;
	browserSize.x = PALETTEWIDTH ;
	browserSize.y = PALETTEHEIGHT ;
	OWndDStoLS(m_oiParent, &browserSize, 1) ;

	m_rcItem.right = m_rcItem.left + browserSize.x ; // makes browser non sizable
	m_rcItem.bottom = m_rcItem.top + browserSize.y ; // makes browser non sizable
	m_index = 1 ;
	m_button = -1 ;
	m_depressed = FALSE ;
	if (OWndIsLayoutTime())
		lstrcpy(m_buffer, "Layout Time") ;
	else
        lstrcpy(m_buffer, "File Closed") ;
	SetRect(&m_rcDepressed,0,0,0,0) ;
	m_beingTracked = FALSE ;
	m_oiLastFieldIndex = 0 ;

	OWndSetItemEnabled(this, TRUE);
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  AdjustNewRect
// DESCRIPTION:  The Browser Pallette size in unchangable so this function
//		always resets the newRect size and returns FALSE
/////////////////////////////////////////////////////////////////////////////
long ALMAPI BrowserVI::AdjustNewRect(RECT* newRect, LONG adjustFlag)
	{
	long rVal = 0L;
	int	tmp;

	POINT browserSize ;
	browserSize.x = PALETTEWIDTH ;
	browserSize.y = PALETTEHEIGHT ;
	OWndDStoLS(m_oiParent, &browserSize, 1) ;

	if (adjustFlag & OWND_ADJRECT_DEFAULT_H)
		{
    if(adjustFlag & OWND_ADJRECT_ANCHOR_B)
      {
      newRect->top = newRect->bottom - browserSize.y;
      rVal |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      newRect->bottom = newRect->top + browserSize.y;
      rVal |= OWND_ADJRECT_CHANGED_B;
      }
		}
	else if (newRect->bottom < newRect->top)
		{
		tmp = newRect->bottom;
		newRect->bottom = newRect->top;
		newRect->top = tmp;
		rVal |= OWND_ADJRECT_SWAPPED_TB;
		if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
			{ //if we swap the sides, we need to swap the anchor points
			adjustFlag &= ~OWND_ADJRECT_ANCHOR_B;
			adjustFlag |= OWND_ADJRECT_ANCHOR_T;
			}
		else
			{
			adjustFlag &= ~OWND_ADJRECT_ANCHOR_T;
			adjustFlag |= OWND_ADJRECT_ANCHOR_B;
			}
		}

	if (adjustFlag & OWND_ADJRECT_DEFAULT_W)
		{
		rVal |= (OWND_ADJRECT_CHANGED_R | OWND_ADJRECT_DEFAULT_W);
    if(adjustFlag & OWND_ADJRECT_ANCHOR_R)
      {
      newRect->left = newRect->right - browserSize.x;
      rVal |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      newRect->right = newRect->left + browserSize.x;
      rVal |= OWND_ADJRECT_CHANGED_R;
      }
		}
	else if (newRect->right < newRect->left)
		{
		tmp = newRect->right;
		newRect->right = newRect->left;
		newRect->left = tmp;
		rVal |= OWND_ADJRECT_SWAPPED_LR;
		if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
			{ //if we swap the sides, we need to swap the anchor points
			adjustFlag &= ~OWND_ADJRECT_ANCHOR_R;
			adjustFlag |= OWND_ADJRECT_ANCHOR_L;
			}
		else
			{
			adjustFlag &= ~OWND_ADJRECT_ANCHOR_L;
			adjustFlag |= OWND_ADJRECT_ANCHOR_R;
			}
		}

  //At this point, the rectangle is not flipped inside-out

	// Make sure it is the right width and height
	if (newRect->bottom - newRect->top != browserSize.y)
		{
		if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
			{
			newRect->top = newRect->bottom - browserSize.y ;
			rVal |= OWND_ADJRECT_CHANGED_T;
			}
		else
			{
			newRect->bottom = newRect->top + browserSize.y ;
			rVal |= OWND_ADJRECT_CHANGED_B;
			}
		}

	if (newRect->right - newRect->left != browserSize.x)
		{
		if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
			{
			newRect->left = newRect->right - browserSize.x;
			rVal |= OWND_ADJRECT_CHANGED_L;
			}
		else
			{
			newRect->right = newRect->left + browserSize.x;
			rVal |= OWND_ADJRECT_CHANGED_R;
			}
		}
	return rVal;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  Draw
// DESCRIPTION:  Draws the Browser Pallette.
/////////////////////////////////////////////////////////////////////////////
void ALMAPI BrowserVI::Draw(HDC hdc, RECT*)
	{
	SIZE windowExt ;
	SIZE viewportExt ;
	int width = m_rcItem.right - m_rcItem.left ;
	int height = m_rcItem.bottom - m_rcItem.top ;

	HDC hdcMem = CreateCompatibleDC (hdc);
	int mapMode = GetMapMode(hdc) ;
	SetMapMode(hdcMem, mapMode) ;
	//DWORD windowExt = GetWindowExtEx(hdc) ;
	//SetWindowExtEx(hdcMem, LOWORD(windowExt), HIWORD(windowExt)) ;
	//DWORD viewportExt = GetViewportExtEx(hdc) ;
	//SetViewportExtEx(hdcMem, LOWORD(viewportExt), HIWORD(viewportExt)) ;
	if (GetWindowExtEx(hdc, &windowExt))
		{
		SetWindowExtEx(hdcMem, windowExt.cx,windowExt.cy, NULL) ;
		GetViewportExtEx (hdc, &viewportExt) ;
		SetViewportExtEx (hdcMem, viewportExt.cx, viewportExt.cy, NULL) ;
		}

	// Draw the Browser button palette
	SelectObject (hdcMem, hBitmap) ;
	BitBlt (hdc, m_rcItem.left, m_rcItem.top, width, height,
			hdcMem, 0, 0, SRCCOPY) ;

	// Draw "All" and "Found" buttons
	int highlight = B_FOUND ;
	if (IsFile(m_oiObject))
		highlight = B_ALL ;
	RECT rcButton = button[highlight].rect ;
	OWndDStoLS(m_oiParent, (POINT*)&rcButton, 2) ;
	BitBlt (hdc,
			m_rcItem.left + rcButton.left,
			m_rcItem.top + rcButton.top,
			rcButton.right,
			rcButton.bottom,
			hdcMem, rcButton.left,
			rcButton.top, NOTSRCCOPY) ;
	// Draw the button that has been pressed
	if (m_depressed)
		{
		rcButton = button[m_button].rect ;
		OWndDStoLS(m_oiParent, (POINT*)&rcButton, 2) ;
		BitBlt (hdc,
				m_rcItem.left + rcButton.left,
				m_rcItem.top + rcButton.top,
				rcButton.right,
				rcButton.bottom,
				hdcMem, rcButton.left, rcButton.top, NOTSRCCOPY) ;
		}
	// Draw Text
	POINT fontSize ;
	fontSize.x = -8 ;
	fontSize.y = 0 ;
	OWndDStoLS(m_oiParent, &fontSize, 1) ;
	HFONT hfont = CreateFont(fontSize.x, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "arial");
	HFONT hfontOld = (HFONT) SelectObject(hdc, hfont);

	RECT rect ;
	rcButton = button[B_TEXT].rect ;
	OWndDStoLS(m_oiParent, (POINT*)&rcButton, 2) ;
	rect.left = m_rcItem.left + rcButton.left + 5 ;
	rect.top = m_rcItem.top + rcButton.top + 1 ;
	rect.right = m_rcItem.right ;
	rect.bottom = rect.top + rcButton.bottom - 2 ;

	DrawText(hdc, m_buffer, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER) ;

	SelectObject(hdc, hfontOld);
	DeleteObject(hfont);

	DeleteDC (hdcMem);
	return ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  MouseDown
// DESCRIPTION:
/////////////////////////////////////////////////////////////////////////////
void ALMAPI BrowserVI::MouseDown(POINT* where, int, int )
	{
// temporarily remove the focus from an editable object
// so that the object will be updated.
	HWND hWndFocus = SetFocus(NULL) ;
	if (hWndFocus)
		SetFocus(hWndFocus) ;

	if (m_depressed == TRUE) // if another button is already depressed, release it
		ReleaseButton() ;

	POINT mouse ;
	mouse.x = where->x - m_rcItem.left ; // horizontal position of cursor in palette
	mouse.y = where->y - m_rcItem.top ; // vertical position of cursor in palette
	int nButtons = sizeof(button)/sizeof(button[0]) ;
	OWndLStoDS(m_oiParent, &mouse, 1) ;
	for (m_button = 0 ; m_button <  nButtons ; m_button++)
		{
		if (mouse.x <= (button[m_button].rect.left + button[m_button].rect.right))
			if (mouse.y <= (button[m_button].rect.top + button[m_button].rect.bottom))
				break ;
		}
	if (m_button == nButtons)
		{
		m_button = -1 ;
		return ; // no button pushed
		}

	if (m_button == B_TEXT)
		return ;

	else if (m_button == B_ALL || m_button == B_FOUND)  // all or found button
		{
		SendUrgentSignal(m_oiObject, m_button+1) ;
		SetFileFlag(m_oiObject, (m_button == B_ALL)) ;
		SetScope() ;
		AEvtPostSignalAtMark(m_oiObject, m_button+21) ;
		}
	else
		{
		// save button rectangle in window coordinates
		CopyRect(&m_rcDepressed, &button[m_button].rect) ;
		OWndDStoLS (m_oiParent, (POINT*)&m_rcDepressed, 2) ;

		m_rcDepressed.left += m_rcItem.left ;
		m_rcDepressed.top += m_rcItem.top ;
		m_rcDepressed.right += m_rcDepressed.left ;
		m_rcDepressed.bottom += m_rcDepressed.top ;

		m_beingTracked = TRUE ;
		OWndSetCaptureItem(this) ;
		DepressButton() ;
		}
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  MouseMove
// DESCRIPTION:  Handles mouse move notification messages.
/////////////////////////////////////////////////////////////////////////////
void ALMAPI BrowserVI::MouseMove (POINT* where, int keysDown)
	{
	if (!m_beingTracked)
		return ;  // return immediately if we are not in track mode
	if ((keysDown & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) == 0)
		{  // if the mouse buttons have been released
		m_beingTracked = FALSE ;
		OWndReleaseCapture() ;
		ReleaseButton() ;
		return ;
		}
	if (PtInRect(&m_rcDepressed, *where))
		{
		if (!m_depressed) // if the mouse is in the button
			DepressButton() ; // if the button is not depressed
		}
	else
		{
		if (m_depressed) // if the mouse is not in the button
			ReleaseButton() ;  // if the button is not depressed
		}
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  MouseUp
// DESCRIPTION:  Part of the new window item protocol.
/////////////////////////////////////////////////////////////////////////////
void ALMAPI BrowserVI::MouseUp(POINT* where, int, int )
	{
	m_beingTracked = FALSE ;
	OWndReleaseCapture() ;
	if (PtInRect(&m_rcDepressed, *where))
		{
		ReleaseButton() ;
		CallDatabaseMethods() ;
		WriteText() ;
		}
	else
		ReleaseButton() ;
	}
///////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DepressButton
// DESCRIPTION:  Inverts the browser button.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::DepressButton()
       	{
	if (m_button == -1 || m_depressed)
		return ;
//	HDC hdc = GetDC(m_hwndParent) ;
//	HDC hdcMem = CreateCompatibleDC (hdc);
//	int mapMode = GetMapMode(hdc) ;
// 	SetMapMode(hdcMem, mapMode) ;
//	DWORD windowExt = GetWindowExt(hdc) ;
//	SetWindowExt(hdcMem, LOWORD(windowExt), HIWORD(windowExt)) ;
//	DWORD viewportExt = GetViewportExt(hdc) ;
//	SetViewportExt(hdcMem, LOWORD(viewportExt), HIWORD(viewportExt)) ;
//
//	SelectObject (hdcMem, hBitmap) ;

//	RECT rcButton = button[m_button].rect ;
//	OWndDStoLS(m_oiParent, (POINT*)&rcButton, 2) ;
//	BitBlt (hdc,
//			m_rcItem.left + rcButton.left,
//			m_rcItem.top + rcButton.top,
//			rcButton.right,
//			rcButton.bottom,
//			hdcMem, rcButton.left, rcButton.top, NOTSRCCOPY) ;
	m_depressed = TRUE ;
//	rcButton.top    += m_rcItem.top;
//    rcButton.left   += m_rcItem.left;
//	rcButton.bottom += m_rcItem.top;
//    rcButton.right  += m_rcItem.left;
	RECT rect = m_rcItem ;
	OWndLStoDS(m_oiParent, (POINT*)&rect, 2) ;
	rect.top += button[m_button].rect.top ;
	rect.left += button[m_button].rect.left ;
	rect.bottom = rect.top + button[m_button].rect.bottom ;
	rect.right = rect.left + button[m_button].rect.left + button[m_button].rect.right ;
	OWndDStoLS(m_oiParent, (POINT*)&rect, 2) ;
	OWndInvalidateLogicalRect(m_oiParent, &rect, FALSE) ;
//	OWndInvalidateLogicalRect(m_oiParent, &rcButton, FALSE);
    UpdateWindow(m_hwndParent);
//	ReleaseDC (m_hwndParent, hdc) ;
//	DeleteDC (hdcMem);
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ReleaseButton
// DESCRIPTION:  Inverts the browser button.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::ReleaseButton()
	{
	if (m_button == -1 || !m_depressed)
		return ;
//	HDC hdc = GetDC(m_hwndParent) ;
//	HDC hdcMem = CreateCompatibleDC (hdc);
//	int mapMode = GetMapMode(hdc) ;
//	SetMapMode(hdcMem, mapMode) ;
//	DWORD windowExt = GetWindowExt(hdc) ;
//	SetWindowExt(hdcMem, LOWORD(windowExt), HIWORD(windowExt)) ;
//	DWORD viewportExt = GetViewportExt(hdc) ;
//	SetViewportExt(hdcMem, LOWORD(viewportExt), HIWORD(viewportExt)) ;
//
//	SelectObject (hdcMem, hBitmap) ;
//
//	RECT rcButton = button[m_button].rect ;
//	OWndDStoLS(m_oiParent, (POINT*)&rcButton, 2) ;
//	BitBlt (hdc,
//			m_rcItem.left + rcButton.left,
//			m_rcItem.top + rcButton.top,
//			rcButton.right,
//			rcButton.bottom,
//			hdcMem, rcButton.left, rcButton.top, SRCCOPY) ;
	m_depressed = FALSE ;
//	rcButton.top    += m_rcItem.top;
//    rcButton.left   += m_rcItem.left;
//	rcButton.bottom += m_rcItem.top;
//    rcButton.right  += m_rcItem.left;
//	OWndInvalidateLogicalRect(m_oiParent, &rcButton, FALSE);
	RECT rect = m_rcItem ;
	OWndLStoDS(m_oiParent, (POINT*)&rect, 2) ;
	rect.top += button[m_button].rect.top ;
	rect.left += button[m_button].rect.left ;
	rect.bottom = rect.top + button[m_button].rect.bottom ;
	rect.right = rect.left + button[m_button].rect.left + button[m_button].rect.right ;
	OWndDStoLS(m_oiParent, (POINT*)&rect, 2) ;
	OWndInvalidateLogicalRect(m_oiParent, &rect, FALSE) ;
	UpdateWindow(m_hwndParent);
//	ReleaseDC (m_hwndParent, hdc) ;
//	DeleteDC (hdcMem);
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  SetScope
// DESCRIPTION:  Sets either the "All" or "Found" button.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::SetScope()
	{
	m_index = 0 ;
	if (!m_oiDatabase)
		return ;

	if (IsFile(m_oiObject))
		AllRefresh() ;
	else
		FoundRefresh() ;
	WriteText() ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  AllRefresh
// DESCRIPTION:  Refreshes the text window and all objects connected to the
//      currently locked record.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::AllRefresh()
	{
	RECT rect = m_rcItem ;
	OWndLStoDS(m_oiParent, (POINT*)&rect, 2) ;
	rect.top += button[B_ALL].rect.top ;
	rect.left += button[B_ALL].rect.left ;
	rect.bottom = rect.top + button[B_FOUND].rect.bottom ;
	rect.right = rect.left + button[B_FOUND].rect.left + button[B_FOUND].rect.right ;
	OWndDStoLS(m_oiParent, (POINT*)&rect, 2) ;
	OWndInvalidateLogicalRect(m_oiParent, &rect, FALSE) ;

	if (!ODatbIsFileOpen(m_oiDatabase))
	{ lstrcpy(m_buffer, "File Closed") ;
      return ;
	} // if

	ODatbMethodParams Params ;
	long recordCount = ODatbGetRecCount(m_oiDatabase) ;
	if (!ODatbIsRecLock(m_oiDatabase))
		{
		wsprintf(m_buffer, "At - of %ld", recordCount) ;
		return ;
		}
	Params.obj[0] = m_oiDatabase ;
	GetSortableActiveField(Params.obj[1]) ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)RecordIndexEvent, (long)(ODatbMethodParams *)&Params) ;
	m_index = (long)Params.obj[2] ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetRecordEvent, (long)(ODatbMethodParams *)&Params) ;
	wsprintf(m_buffer, "At %ld of %ld", m_index, recordCount) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FoundRefresh
// DESCRIPTION:  Refreshes the text window and all objects connected to the
//      currently locked record.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::FoundRefresh()
	{
	RECT rect = m_rcItem ;
	OWndLStoDS(m_oiParent, (POINT*)&rect, 2) ;
	rect.top += button[B_ALL].rect.top ;
	rect.left += button[B_ALL].rect.left ;
	rect.bottom = rect.top + button[B_FOUND].rect.bottom ;
	rect.right = rect.left + button[B_FOUND].rect.left + button[B_FOUND].rect.right ;
	OWndDStoLS(m_oiParent, (POINT*)&rect, 2) ;
	OWndInvalidateLogicalRect(m_oiParent, &rect, FALSE) ;

	ODatbMethodParams Params ;
	long foundCount = ODatbGetFoundCount(m_oiDatabase) ;
	if (!ODatbIsRecLock(m_oiDatabase))
		{
		wsprintf(m_buffer, "At - of %ld", foundCount) ;
		return ;
		}
	m_index = ODatbGetFoundIndex(m_oiDatabase) + 1 ;
	if (m_index == 0)
		{
		wsprintf(m_buffer, "At - of %ld", foundCount ) ;
		return ;
		}
	Params.obj[0] = m_oiDatabase ;
	Params.obj[1] = (AOBJECTID)m_index ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetFoundRecordEvent, (long)(ODatbMethodParams *)&Params) ;
	wsprintf(m_buffer, "At %ld of %ld", m_index, foundCount ) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  WriteText
// DESCRIPTION:  Writes text to the browser button palette.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::WriteText()
	{
	RECT rect = m_rcItem ;
	OWndLStoDS(m_oiParent, (POINT*)&rect, 2) ;
	rect.left += button[B_TEXT].rect.left ;
	rect.top += button[B_TEXT].rect.top ;
	rect.right = rect.left + button[B_TEXT].rect.right ;
	rect.bottom = rect.top + button[B_TEXT].rect.bottom ;
	OWndDStoLS(m_oiParent, (POINT*)&rect, 2) ;
	OWndInvalidateLogicalRect(m_oiParent, &rect, FALSE) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  CallDatabaseMethods
// DESCRIPTION:  Calls the appropriate Database methods.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::CallDatabaseMethods()
	{
	ODatbMethodParams Params ;

	int signal = m_button + 1 ;
	if (signal > B_TEXT) signal-- ;
	if (m_button == B_QUIT) signal = 15 ;
	if (m_button == B_CLEAR) signal = 4 ;

	ClearCancelledFlag(m_oiObject) ;
	SendUrgentSignal(m_oiObject, signal) ;
	BOOL isFile = IsFile(m_oiObject) ;

	if (WasCancelled(m_oiObject)) // the Cancel function was called
		return ;
	if (m_button == B_QUIT)
		{
		AEvtPostStandard(m_oiDatabase, AEVENT_SHUTDOWN, 0, AEVT_ATTAIL) ;
		return ;
		}
	if (m_oiDatabase == 0)
		{
		if ( m_button != B_CLEAR )
			{
                        MessageBeep(0);
			AEvtPostSignalAtMark(m_oiObject, signal+20) ;
			return ;
			}
		}
	else
		{
		Params.obj[0] = m_oiDatabase ;
		if (ODatbIsFileOpen(m_oiDatabase) == FALSE)
			if (m_button > B_OPEN && m_button != B_CLEAR )
				{
                                MessageBeep(0);
				AEvtPostSignalAtMark(m_oiObject, signal+20) ;
				return ;
				}
		}
	Params.flowIdx = 0 ;
	switch (m_button)
		{
		case B_NEW :
			Params.obj[1] = 0 ;
			AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)NewFileEvent, (long)(ODatbMethodParams *)&Params) ;
			if(Params.flowIdx != 1)
				return ;  // if the user canceled the new file dialog box
			break ;
		case B_OPEN :
			AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)OpenFileEvent, (long)(ODatbMethodParams *)&Params) ;
			if(Params.flowIdx != 1)
				return ;  // if the user canceled the open file dialog box
			break ;
		case B_CLOSE :
			AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)CloseFileEvent, (long)(ODatbMethodParams *)&Params) ;
			break ;
		case B_FIND :
			doFind(&Params) ;
			break ;
		case B_POWER :
			if (isFile)
				{
				SetFileFlag(m_oiObject, FALSE) ;
				SetScope() ;
				}
			AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)PowerSearchEvent, (long)(ODatbMethodParams *)&Params) ;
			m_index = 0 ;
			break ;
		case B_PREV :
			if(isFile)
				doPreviousFile(&Params) ;
			else
				doPreviousFound(&Params) ;
			break ;
		case B_NEXT :
			if(isFile)
				doNextFile(&Params) ;
			else
				doNextFound(&Params) ;
			break ;
		case B_CLEAR :
			AEvtPostSignalAtMark(m_oiObject, 0) ;
			break ;
		case B_ADD :
			doAdd(&Params) ;
			break ;
		case B_UPDATE :
			doUpdate(&Params) ;
			break ;
		case B_DELETE :
			if (isFile)
				doDelete(&Params) ;
			else
				doDeleteFound(&Params) ;
			break ;
		}
	UpdateText(Params.flowIdx) ;
	SendUrgentSignal(m_oiObject, signal + 20) ;
	switch (m_button)
		{
		case B_NEW :
		case B_OPEN :
		case B_CLOSE :
		case B_DELETE :
			SendUrgentSignal(m_oiObject, 0) ; // Send the "Clear Fields" signal
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doFind
// DESCRIPTION:  Finds matching records in the database file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doFind(ODatbMethodParams  *Params)
	{
	Params->flowIdx = 0 ;
	int flags = GetSortableActiveField(Params->obj[1]) ;
	if (flags == NOTSORTABLE)
		{
		MessageBox(m_hwndParent, "The database cannot be searched on the active window item.", NULL, MB_OK) ;
		return ;
		}
	if (IsFile(m_oiObject))
		{
		SetFileFlag(m_oiObject, FALSE) ;
		SetScope() ;
		}
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)FindRecordsEvent, (long)Params) ;
	switch(Params->flowIdx)
		{
		case 1:
			m_index = 1 ;
			wsprintf(m_buffer, "At 1 of %ld", ODatbGetFoundCount(m_oiDatabase)) ;
			break ;
		case 2:
			m_index = 0 ;
			lstrcpy(m_buffer, "No records found.") ;
			break ;
		case 3:
			m_index = 1 ;
			lstrcpy(m_buffer, "The found record is busy.") ;
			break ;
		}
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doAdd
// DESCRIPTION:  Adds a record to a database file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doAdd(ODatbMethodParams  *Params)
	{
	Params->obj[0] = m_oiDatabase ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)AddRecordEvent, (long)Params) ;
	if (Params->flowIdx != 1)
		return ;

	if (!IsFile(m_oiObject))
		{
		SetFileFlag(m_oiObject, TRUE) ;
		SetScope() ;
		}
	GetSortableActiveField(Params->obj[1]) ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)RecordIndexEvent, (long)Params) ;
	m_index = (long) Params->obj[2] ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doUpdate
// DESCRIPTION:  Updates a record from a database file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doUpdate(ODatbMethodParams  *Params)
	{
	if (ODatbIsRecLock(m_oiDatabase) == FALSE)
		{
		MessageBox(m_hwndParent, "No record has been selected for updating", NULL, MB_OK) ;
		return ;
		}
	Params->obj[0] = m_oiDatabase ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)UpdateRecordEvent, (long)Params) ;

	GetSortableActiveField(Params->obj[1]) ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)RecordIndexEvent, (long)Params) ;
	m_index = (long)Params->obj[2] ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doDelete
// DESCRIPTION:  Deletes a record from a database file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doDelete(ODatbMethodParams  *Params)
	{
	if (ODatbIsRecLock(m_oiDatabase) == FALSE)
		{
		MessageBox(m_hwndParent, "No record has been selected for deletion", NULL, MB_OK) ;
		return ;
		}
	Params->obj[0] = m_oiDatabase ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)DeleteRecordEvent, (long)Params) ;

	long recordCount = ODatbGetRecCount(m_oiDatabase) ;
	if (m_index > recordCount)
		m_index = recordCount ;

	if ((Params->flowIdx != 1) || (m_index == 0))
		return ;

	GetSortableActiveField(Params->obj[1]) ;
	Params->obj[2] = (AOBJECTID)m_index ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetRecordEvent, (long)Params) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doDeleteFound
// DESCRIPTION:  Deletes a record from a database file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doDeleteFound(ODatbMethodParams  *Params)
	{
	if (ODatbIsRecLock(m_oiDatabase) == FALSE)
		{
		MessageBox(m_hwndParent, "No record has been selected for deletion", NULL, MB_OK) ;
		return ;
		}
	Params->obj[0] = m_oiDatabase ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)DeleteRecordEvent, (long)Params) ;
	if (Params->flowIdx == 2)
		{
		MessageBox(m_hwndParent, "Record not deleted", NULL, MB_OK) ;
		return ;
		}
	Params->obj[1] = (AOBJECTID)m_index ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)RemoveFoundEvent, (long)Params) ;

	long foundCount = ODatbGetFoundCount(m_oiDatabase) ;
	if (m_index > foundCount)
		m_index = foundCount ;

	if ((Params->flowIdx != 1) || (m_index == 0))
		return ;

	Params->obj[1] = (AOBJECTID)m_index ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetFoundRecordEvent, (long)Params) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  UpdateText
// DESCRIPTION:  Updates Browser text.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::UpdateText(int flowIdx)
	{
	long total ;
	if (m_oiDatabase)
		{
		if (IsFile(m_oiObject))
			total = ODatbGetRecCount(m_oiDatabase) ;
		else
			total = ODatbGetFoundCount(m_oiDatabase) ;
		}

	switch (m_button)
		{
		case B_NEW :
		case B_OPEN :
			if (flowIdx == 1)
				{
				m_index = 0 ;
				wsprintf(m_buffer, "At - of %ld", total) ;
				}
			else
				lstrcpy(m_buffer, "") ;
			return ;
		case B_PREV :
		case B_NEXT :
			switch(flowIdx)
				{
				case 1:
					wsprintf(m_buffer, "At %ld of %ld", m_index, total) ;
					break ;
				case 2:
					wsprintf(m_buffer, "Record %ld is gone.", m_index) ;
					break ;
				case 3:
					wsprintf(m_buffer, "Record %ld is busy.", m_index) ;
					break ;
				}
			return ;
		case B_POWER :
		case B_ADD :
		case B_UPDATE :
		case B_DELETE :
			if (m_index > 0 && flowIdx == 1)
				wsprintf(m_buffer, "At %ld of %ld", m_index, total) ;
			else if (flowIdx > 0)
				wsprintf(m_buffer, "At - of %ld", total) ;
			break ;
		case B_CLOSE :
			lstrcpy(m_buffer, "File Closed") ;
			break ;
		}
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doPreviousFile
// DESCRIPTION:  Retrieves the previous record from the file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doPreviousFile(ODatbMethodParams  *Params)
	{
	AOBJECTID oiField ;

	int flags = GetSortableActiveField(oiField) ;
	if (flags != INDEXED)
		{
		MessageBox(m_hwndParent,"The database is not indexed on the currently active window item.", NULL, MB_OK) ;
		return ;
		}
	Params->obj[0] = m_oiDatabase ;
	Params->obj[1] = oiField ;
	if (GetIndexedFieldObject(m_oiObject) == 0)
		{
		if (oiField != m_oiLastFieldIndex)
			{
			if (m_index != 0)
				{   // the active field has changed which would cause the index to change too
				AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)RecordIndexEvent, (long)Params) ;
				m_index = (long)Params->obj[2] ;
				}
			m_oiLastFieldIndex = oiField ;
			}
		}
	if (m_index <= 1)  // check limits
		{
		MessageBeep(0);
                return;
		}

	Params->obj[2] = (AOBJECTID)--m_index ;

	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetRecordEvent, (long)Params) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doNextFile
// DESCRIPTION:  Retrieves the next record from the file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doNextFile(ODatbMethodParams  *Params)
	{
	AOBJECTID oiField ;
	long total ;

	int flags = GetSortableActiveField(oiField) ;
	if (flags != INDEXED)
		{
		MessageBox(m_hwndParent, "The database is not indexed on the currently active window item.", NULL, MB_OK) ;
		return ;
		}
	Params->obj[0] = m_oiDatabase ;
	Params->obj[1] =  oiField ;
	if (GetIndexedFieldObject(m_oiObject) == 0)
		{
		if (oiField != m_oiLastFieldIndex)
			{
			if (m_index != 0)
				{   // the active field has changed which would cause the index to change too
				AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)RecordIndexEvent, (long)Params) ;
				m_index = (long)Params->obj[2] ;
				}
			m_oiLastFieldIndex = oiField ;
			}
		}
	total = ODatbGetRecCount(m_oiDatabase) ;
	if (m_index >= total)  // check limits
		{
		MessageBeep(0);
                return;
		}

	Params->obj[2] = (AOBJECTID)++m_index ;
	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetRecordEvent, (long)Params) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doPreviousFound
// DESCRIPTION:  Retrieves the previous record from the file.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doPreviousFound(ODatbMethodParams  *Params)
	{
	Params->obj[0] = m_oiDatabase ;
	if (m_index <= 1)  // check limits
        	{
		MessageBeep(0);
		return;
                } 
	Params->obj[1] = (AOBJECTID)--m_index ;

	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetFoundRecordEvent, (long)Params) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doNextFound
// DESCRIPTION:  Retrieves the next record from the found list.
/////////////////////////////////////////////////////////////////////////////
void BrowserVI::doNextFound(ODatbMethodParams  *Params)
	{
	long total = ODatbGetFoundCount(m_oiDatabase) ;
	Params->obj[0] = m_oiDatabase ;
	if (m_index >= total)  // check limits
		{
		MessageBeep(0);
                return;
		}
	Params->obj[1] = (AOBJECTID)++m_index ;

	AEvtCallMethod(0, m_oiDatabase, 0, (AEVENTID)GetFoundRecordEvent, (long)Params) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetSortableActiveField
// DESCRIPTION:  Returns the active field if it is an indexed or sortable
// database field.
///////////////////////////////////////////////////////////////////////////
int BrowserVI::GetSortableActiveField(AOBJECTID& oiObject)
	{
	oiObject = GetIndexedFieldObject(m_oiObject) ;
	if (oiObject) // if an indexed field has been selected at design time
		return INDEXED ;

	OpWndItemD* lpWndItem = OWndGetActiveItem() ;
	if (lpWndItem == NULL)
		{
		oiObject = 0 ;
		return NOTSORTABLE ;
		}

	AOBJECTID oiActiveObject = lpWndItem->m_oiObject ;

	// Is the active object an indexed field of the database?
	AMEMBLOCKID keyObjectIDs = AMemAllocate(0) ;
	ODatbGetKeyObjectIDs(m_oiDatabase, keyObjectIDs) ;
	int nKeys = (int)AMemGetSize(keyObjectIDs) / sizeof(AOBJECTID) ;
	AOBJECTID * lpKeys = (AOBJECTID *)AMemLock(keyObjectIDs) ;
	for (int i = 0; i < nKeys; i++)
		if (lpKeys[i] == oiActiveObject)
			{
			oiObject = oiActiveObject ;
			break ;
			}
	AMemUnlock(keyObjectIDs) ;
	AMemFree(keyObjectIDs) ;
	if (oiObject != 0)
		return INDEXED ;

	// Is the active object a sortable field of the database?
	AMEMBLOCKID fieldObjectIDs = AMemAllocate(0) ;
	ODatbGetFieldObjectIDs(m_oiDatabase, fieldObjectIDs) ;
	int nFields = (int)AMemGetSize(fieldObjectIDs) / sizeof(AOBJECTID) ;
	AOBJECTID * lpFields = (AOBJECTID *)AMemLock(fieldObjectIDs) ;
	for (i = 0; i < nFields; i++)
		if (lpFields[i] == oiActiveObject)
			{
			oiObject = oiActiveObject ;
			break ;
			}
	AMemUnlock(fieldObjectIDs) ;
	AMemFree(fieldObjectIDs) ;
	if (oiObject != 0)
		{
		ODatbKey key ;
		key.type = ODATB_KEYQUERY;
		AEvtCallMethod(m_oiDatabase, oiObject, 0, ODATB_EVT_GETKEY, (long) &key);
		if (key.type != ODATB_KEYQUERY)
			return SORTABLE ;
		oiObject = 0 ;
		}
	return NOTSORTABLE ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION: SetFileFlag
// DESCRIPTION:  Sets the Browser's file flag
/////////////////////////////////////////////////////////////////////////////
void SetFileFlag(AOBJECTID oiBrowser, BOOL flag = TRUE)
	{
	BrowserInfo * lpBrowser = (BrowserInfo *)AObjLockData(oiBrowser, 0) ;
	lpBrowser->file = flag ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION: IsFile
// DESCRIPTION:  Determines whether the browser is working on a found list
//		or the entire database file.
/////////////////////////////////////////////////////////////////////////////
BOOL IsFile(AOBJECTID oiBrowser)
	{
	BrowserInfo * lpBrowser = (BrowserInfo *)AObjLockData(oiBrowser, 0) ;
	BOOL isFile = lpBrowser->file ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return isFile ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION: SetCancelledFlag
// DESCRIPTION:  Determines whether the browser was cancelled.
/////////////////////////////////////////////////////////////////////////////
void ClearCancelledFlag(AOBJECTID oiBrowser)
	{
	BrowserInfo * lpBrowser = (BrowserInfo *)AObjLockData(oiBrowser, 0) ;
	lpBrowser->cancel = FALSE ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION: WasCancelled
// DESCRIPTION:  Determines whether the browser was cancelled.
/////////////////////////////////////////////////////////////////////////////
BOOL WasCancelled(AOBJECTID oiBrowser)
	{
	BrowserInfo * lpBrowser = (BrowserInfo *)AObjLockData(oiBrowser, 0) ;
	BOOL wasCancelled = lpBrowser->cancel ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return wasCancelled ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION: GetIndexedFieldObject
// DESCRIPTION:  Returns the Browser's indexed field object.
/////////////////////////////////////////////////////////////////////////////
AOBJECTID GetIndexedFieldObject(AOBJECTID oiBrowser)
	{
	BrowserInfo * lpBrowser = (BrowserInfo *)AObjLockData(oiBrowser, 0) ;
	AOBJECTID oiField = lpBrowser->oiField ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return oiField ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  SendUrgentSignal
// DESCRIPTION:  Posts a signal at once.
///////////////////////////////////////////////////////////////////////////
BOOL SendUrgentSignal(AOBJECTID oiObject, long signal)
	{
	AEvtInfo event ;

	event.sourceObject = oiObject ;
	event.targetObject = oiObject ;
	event.when = AAppGetTime() ;
	event.how = AEVT_ATONCE ;
	event.what = AEVENT_SIGNAL ;
	event.details = signal ;
	return AEvtPost(&event) ;
	}
