#include "include.h"
#include "math.h"
#include "helpids.h"

#define I_SCROLL_LINE		  8
#define I_SCROLL_PAGE			64
//this is created in the WM_INITDIALOG message
EDIT_POLY::EDIT_POLY(
	AOBJECTID 								oiPoly,
	HWND										hwnd)
	{
	LPPOLY									poly;
	USHORT									us;
	size_t									size;
	LPBUTTON								pButtonSource;
	LPBUTTON  *					ppButtons;
	RECT										rcDrawWindow;

	m_oiStore = oiPoly;

	m_miNormal = AMemAllocate(0);
	CopyDatasToMemBlocks(m_miNormal, oiPoly, I_DATA_NORMAL);
	m_hBmpNormal = NULL;

	m_miHighlight = AMemAllocate(0);
	CopyDatasToMemBlocks(m_miHighlight, oiPoly, I_DATA_HIGHLIGHT);
	m_hBmpHighlight = NULL;

	m_hPaletteNormal = m_hPaletteHighlight = NULL;

	m_whichImage = I_IMAGE_NORMAL;

	CreateBitmapHandles();

	m_bTracking = FALSE;
	m_usButtonSelected = I_UNDEFINED_INDEX;

	m_hwndDialog = hwnd;

	//Create the draw window
	GetWindowRect( GetDlgItem(m_hwndDialog, OD_FRAME), &rcDrawWindow );
	MapWindowPoints(NULL, m_hwndDialog, (LPPOINT)&rcDrawWindow, 2);
	m_hwndDraw = CreateWindow(g_szDrawClassName, "", WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL, 
		rcDrawWindow.left, rcDrawWindow.top, rcDrawWindow.right - rcDrawWindow.left, rcDrawWindow.bottom - rcDrawWindow.top, 
		m_hwndDialog, (HMENU)OD_DRAWWINDOW, g_hInstance, this);

 	SetSizes();


	m_usNumPoints = I_UNDEFINED_INDEX;

	m_ptOffset.x = m_ptOffset.y = 0;

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	m_uidNext = poly->m_uidNext;

	m_usNumButtons = poly->GetNumberOfButtons();
	pButtonSource = (LPBUTTON)poly->m_aButtons;

	m_miButtonsPointer = AMemAllocate(sizeof(LPVOID) * NumButtons());
	if (NumButtons())
		{
		ppButtons	= (LPBUTTON  *)AMemLock(m_miButtonsPointer);
		I_VERIFY_POINTER(ppButtons, UnlockPoly(oiPoly);return);

		for (us=0; us<NumButtons(); us++)
			{
			size = pButtonSource->GetSize();
			ppButtons[us] = new (size) BUTTON(pButtonSource);
			ppButtons[us]->CreateRegionHandle();
			pButtonSource = (LPBUTTON)((LPSTR)pButtonSource + size);
			}
		AMemUnlock(m_miButtonsPointer);
		}

	UnlockPoly(oiPoly);
	}

//////////

EDIT_POLY::~EDIT_POLY()
	{
	LPBUTTON  *					ppButtons;
	USHORT									us;

	DeleteBitmapHandles();

	if (NumButtons())
		{
		if ( (ppButtons	= (LPBUTTON  *)AMemLock(m_miButtonsPointer)) != NULL)
			{
			for (us=0; us<NumButtons(); us++)
				{
				ppButtons[us]->DestroyRegionHandle();
				delete ppButtons[us];
				}
			}
		AMemUnlock(m_miButtonsPointer);
		}

	AMemFree(m_miButtonsPointer);

	}

//////////

BOOL EDIT_POLY::CreateBitmapHandles()
	{

	DeleteBitmapHandles();

	m_hPaletteNormal = CreateBitmapPalette(m_miNormal);
	m_hPaletteHighlight = CreateBitmapPalette(m_miHighlight);

	m_hBmpNormal = CreateBitmapHandle(m_hPaletteNormal, m_miNormal);
	m_hBmpHighlight = CreateBitmapHandle(m_hPaletteHighlight, m_miHighlight);
	
	return TRUE;
	}

//////////

void EDIT_POLY::DeleteBitmapHandles()
	{
	if (m_hBmpNormal)
		DeleteObject(m_hBmpNormal);

	if (m_hBmpHighlight)
		DeleteObject(m_hBmpHighlight);

	if (m_hPaletteNormal)
		DeleteObject(m_hPaletteNormal);

	if (m_hPaletteHighlight)
		DeleteObject(m_hPaletteHighlight);
		
	m_hPaletteNormal = m_hPaletteHighlight = NULL;
	m_hBmpHighlight = m_hBmpNormal = NULL;
	}
//////////

void EDIT_POLY::MouseDown(
	LPPOINT 								ptWindow, 
	WPARAM 									keysdown)
	{
	I_UNUSED(keysdown);

	ptWindow->x -= m_ptOffset.x;
	ptWindow->y -= m_ptOffset.y;

	switch(m_currentTool)
		{
		case I_TOOL_ARROW:
			if ( (SetCurrentSelectedButton(PointInWhichButton(ptWindow))) != I_UNDEFINED_INDEX)
				{	
				m_ptTracking[0] = *ptWindow;
				m_bTracking = TRUE;
				}
			EnableProperControls();
			break;
		
		case I_TOOL_RECT:
		case I_TOOL_ELLIPSE:
			m_usNumPoints = 2;
			m_ptTracking[0] = m_ptTracking[1] = *ptWindow;
		 	m_bTracking = TRUE;
			break;

		case I_TOOL_POLYGON:
			if (m_bTracking)
				{
				if (m_usNumPoints < I_MAX_POINTS_AT_EDIT)
					m_usNumPoints++;
				m_ptTracking[m_usNumPoints] = *ptWindow;
				break;
				}
		case I_TOOL_FREEHAND:
			m_ptTracking[0] = m_ptTracking[1] = *ptWindow;
			m_usNumPoints = 1;
		 	m_bTracking = TRUE;
			break;
		}

	}

///////////////

void EDIT_POLY::MouseMove(
	LPPOINT 								ptWindow, 
	WPARAM 									keysdown)
	{		
	HDC											hdc;

	I_UNUSED(keysdown);

	ptWindow->x -= m_ptOffset.x;
	ptWindow->y -= m_ptOffset.y;

	if (m_bTracking)
		{
		switch(m_currentTool)
			{
			case I_TOOL_ARROW:
				hdc = GetDC(m_hwndDraw);
				OffsetButton(hdc, m_usButtonSelected, ptWindow->x - m_ptTracking[0].x, ptWindow->y - m_ptTracking[0].y);
				m_ptTracking[0] = *ptWindow;
				ReleaseDC(m_hwndDraw, hdc);
				break;
				
			case I_TOOL_RECT:
			case I_TOOL_ELLIPSE:
				hdc = GetDC(m_hwndDraw);
				DrawTrackingPoints(hdc, FALSE);
				m_ptTracking[1] = *ptWindow;
				DrawTrackingPoints(hdc, FALSE);
				ReleaseDC(m_hwndDraw, hdc);
				break;
				
			case I_TOOL_POLYGON:
				hdc = GetDC(m_hwndDraw);
				DrawTrackingPoints(hdc, FALSE);
				m_ptTracking[m_usNumPoints] = *ptWindow;
				DrawTrackingPoints(hdc, FALSE);
				ReleaseDC(m_hwndDraw, hdc);
				break;

			case I_TOOL_FREEHAND:
				hdc = GetDC(m_hwndDraw);
				if (m_usNumPoints < I_MAX_POINTS_AT_EDIT)
					m_usNumPoints++;
				m_ptTracking[m_usNumPoints] = *ptWindow;
				DrawTrackingPoints(hdc, FALSE);
				ReleaseDC(m_hwndDraw, hdc);
				break;
			}

		}

	}

///////////////

void EDIT_POLY::MouseUp(
	LPPOINT									ptWindow, 
	WPARAM 									keysdown)
	{
	I_UNUSED(keysdown);
	I_UNUSED(ptWindow);

	ptWindow->x -= m_ptOffset.x;
	ptWindow->y -= m_ptOffset.y;

	switch(m_currentTool)
		{
		case I_TOOL_ARROW:
			if (m_bTracking)
				InvalidateRect(m_hwndDraw, NULL, TRUE);
			m_bTracking = FALSE;
			break;

		case I_TOOL_RECT:
		case I_TOOL_ELLIPSE:
		case I_TOOL_FREEHAND:
			if (m_bTracking)
				{
				InvalidateRect(m_hwndDraw, NULL, TRUE);
				m_bTracking = FALSE;
				CreateNewButton();
				}
			break;
		}

	}

//////////

void EDIT_POLY::MouseDoubleClick(
	LPPOINT 								ptWindow, 
	WPARAM 									keysdown)
	{
	USHORT									usIndex;

	I_UNUSED(keysdown);

	ptWindow->x -= m_ptOffset.x;
	ptWindow->y -= m_ptOffset.y;

	switch(m_currentTool)
		{
		case I_TOOL_ARROW:
			if ( (usIndex = PointInWhichButton(ptWindow)) != I_UNDEFINED_INDEX)
				ConfigureButton(usIndex);
			break;

		case I_TOOL_RECT:
		case I_TOOL_ELLIPSE: //do nothing
		case I_TOOL_FREEHAND:
			break;

		case I_TOOL_POLYGON:
			InvalidateRect(m_hwndDraw, NULL, TRUE);
			m_bTracking = FALSE;
			CreateNewButton();
			break;
		}

	}

//////

USHORT EDIT_POLY::PointInWhichButton(
	LPPOINT 								ptWindow)
	{
	LPBUTTON  *					ppButtons;
	USHORT									us;
	USHORT									rVal = I_UNDEFINED_INDEX;

	if (NumButtons())
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return I_UNDEFINED_INDEX);

		for(us=0; us<NumButtons() && rVal==I_UNDEFINED_INDEX; us++)
			{
			if (ppButtons[us]->PointInButton(ptWindow))
				rVal = us;
			}

		EndUsingButtons();
		}

	return rVal;
	}

//////

void EDIT_POLY::DrawTrackingPoints(
	HDC 										hdc, 
	BOOL 										bAll)
	{
	HPEN										hPen;
	HBRUSH									hBrush;
	int											oldROP;

	hPen = (HPEN)SelectObject(hdc, GetStockObject(BLACK_PEN));
	hBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
	oldROP = SetROP2(hdc, R2_NOTXORPEN);				

	switch(m_currentTool)
		{
		case I_TOOL_ARROW:
			I_FAILURE();
			break;
		case I_TOOL_RECT:
			Rectangle(hdc, m_ptTracking[0].x, m_ptTracking[0].y, m_ptTracking[1].x, m_ptTracking[1].y);
			break;
		case I_TOOL_ELLIPSE:
			Ellipse(hdc, m_ptTracking[0].x, m_ptTracking[0].y, m_ptTracking[1].x, m_ptTracking[1].y);
			break;
		case I_TOOL_POLYGON:
		case I_TOOL_FREEHAND:
			if (bAll)
				Polyline(hdc, m_ptTracking, m_usNumPoints);
			else if (m_usNumPoints)
				{
				///MoveToEx(hdc, m_ptTracking[m_usNumPoints-1].x, m_ptTracking[m_usNumPoints-1].y);
				MoveToEx(hdc, m_ptTracking[m_usNumPoints-1].x, m_ptTracking[m_usNumPoints-1].y, NULL);
				LineTo(hdc, m_ptTracking[m_usNumPoints].x, m_ptTracking[m_usNumPoints].y);
				}
			break;
		}

	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
	SetROP2(hdc, oldROP);				
	}

///////

USHORT EDIT_POLY::SetCurrentSelectedButton(
	USHORT									us)
	{
	LPBUTTON  *					ppButtons;

	if (us != m_usButtonSelected)
		{
		if (NumButtons())
			{
			ppButtons = BeginUsingButtons();
			I_VERIFY_POINTER(ppButtons, return I_UNDEFINED_INDEX);

		 	if (m_usButtonSelected < NumButtons())
		 		ppButtons[m_usButtonSelected]->InvalidateButton(m_hwndDraw, TRUE, &m_ptOffset);
		 
		 	if (us < NumButtons())
		 		ppButtons[us]->InvalidateButton(m_hwndDraw, TRUE, &m_ptOffset);

			EndUsingButtons();
			}

		m_usButtonSelected = us;
	 	}

	return m_usButtonSelected;
	}

//////

void EDIT_POLY::InitDialog()
	{
	char										szObjectName[80];

	AObjGetName(m_oiStore, szObjectName, 80);
  SetWindowText(m_hwndDialog, szObjectName);
	
	AUtlSetTitleFont(OTYPE_POLYBUTTON, NULL, GetDlgItem(m_hwndDialog, ST_OBJECTNAME) );

	//Check the proper 'which image' radio button
	///SendDlgItemMessage(m_hwndDialog, RB_NORMAL, BM_SETCHECK, m_whichImage == I_IMAGE_NORMAL, 0L);
	Button_SetCheck(GetDlgItem(m_hwndDialog, RB_NORMAL), m_whichImage == I_IMAGE_NORMAL);
	
	///SendDlgItemMessage(m_hwndDialog, RB_HIGHLIGHT, BM_SETCHECK, m_whichImage == I_IMAGE_HIGHLIGHT, 0L);
	Button_SetCheck(GetDlgItem(m_hwndDialog, RB_HIGHLIGHT), m_whichImage == I_IMAGE_HIGHLIGHT);

	//check the proper tool icon
	m_currentTool = I_TOOL_ARROW;
	///SendDlgItemMessage(m_hwndDialog, RB_TOOL_ARROW, BM_SETCHECK, 1, 0L);
	Button_SetCheck(GetDlgItem(m_hwndDialog, RB_TOOL_ARROW), 1);
	EnableProperControls();
	}

//////

void EDIT_POLY::EnableProperControls()
	{
	EnableWindow(GetDlgItem(m_hwndDialog, PB_CONFIGURE), m_usButtonSelected != I_UNDEFINED_INDEX);
	EnableWindow(GetDlgItem(m_hwndDialog, PB_DELETE), m_usButtonSelected != I_UNDEFINED_INDEX);
	EnableWindow(GetDlgItem(m_hwndDialog, PB_PASTE), IsClipboardFormatAvailable(CF_DIB)||IsClipboardFormatAvailable(CF_BITMAP) );
	}

//////

void EDIT_POLY::Store()
	{
	size_t									sizeTotal;
	LPBUTTON  *					ppButtons;// = NULL; to remove warning
	USHORT									us;

	sizeTotal	= sizeof(POLY);

	if (NumButtons())
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return);

		for(us=0; us<NumButtons(); us++)
			sizeTotal += ppButtons[us]->GetSize();

		new(sizeTotal, m_oiStore) POLY(m_oiStore, ppButtons, NumButtons(), m_uidNext, FALSE);
		EndUsingButtons();
		}
	else
		new(sizeTotal, m_oiStore) POLY(m_oiStore, NULL, 0, m_uidNext, FALSE);

	CopyMemBlocksToDatas(m_oiStore, I_DATA_NORMAL, m_miNormal);
	CopyMemBlocksToDatas(m_oiStore, I_DATA_HIGHLIGHT, m_miHighlight);
	}

//////

void EDIT_POLY::DialogResized()
	{
	return;
	}

//////

BOOL EDIT_POLY::SaveSizeInIniFile()
	{
	return FALSE;
	}

//////

BOOL EDIT_POLY::ReadSizeFromIniFile()
	{
	return FALSE;
	}

//////

void EDIT_POLY::SetWhichImage(
	WHICH_IMAGE 						w) 
	{
	HDC											hdc;

	if (m_whichImage != w)
		{
		m_whichImage = w; 
		m_ptOffset.x = m_ptOffset.y = 0;
		if (m_hwndDraw)
			{
			hdc = GetDC(m_hwndDraw);
			SetViewportOrgEx(hdc, 0, 0, NULL);
			ReleaseDC(m_hwndDraw, hdc);

			InvalidateRect(m_hwndDraw, NULL, TRUE);
			}
		}
	}

///////

void EDIT_POLY::SetCurrentTool(TOOL_TYPE tt)
	{
	switch(m_currentTool)
		{
		case I_TOOL_ARROW:				InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_ARROW), NULL, TRUE); break;
		case I_TOOL_RECT:					InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_RECT), NULL, TRUE); break;
		case I_TOOL_ELLIPSE:			InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_ELLIPSE), NULL, TRUE); break;
		case I_TOOL_POLYGON:			InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_POLYGON), NULL, TRUE); break;
		case I_TOOL_FREEHAND:			InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_FREEHAND), NULL, TRUE); break;
		}

	m_currentTool = tt;

	switch(m_currentTool)
		{
		case I_TOOL_ARROW:				InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_ARROW), NULL, TRUE); break;
		case I_TOOL_RECT:					InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_RECT), NULL, TRUE); break;
		case I_TOOL_ELLIPSE:			InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_ELLIPSE), NULL, TRUE); break;
		case I_TOOL_POLYGON:			InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_POLYGON), NULL, TRUE); break;
		case I_TOOL_FREEHAND:			InvalidateRect(GetDlgItem(m_hwndDialog, RB_TOOL_FREEHAND), NULL, TRUE); break;
		}

	}

//////

BOOL EDIT_POLY::Load()
	{
	BOOL										rVal = FALSE;
	
	if (LoadBitmapToMemblock(m_whichImage==I_IMAGE_NORMAL ? m_miNormal:m_miHighlight))
		{
	 	SetSizes();
		CreateBitmapHandles();
		InvalidateRect(m_hwndDraw, NULL, TRUE);
		rVal = TRUE;
		}
	return rVal;
	}

//////

BOOL EDIT_POLY::Paste()
	{
	BOOL										rVal = FALSE;
	
	if (CopyBitmapFromClipboardToMemblock(m_whichImage==I_IMAGE_NORMAL ? m_miNormal:m_miHighlight, m_hwndDialog))
		{
 		SetSizes();
		CreateBitmapHandles();
		InvalidateRect(m_hwndDraw, NULL, TRUE);
		rVal = TRUE;
		}
	return rVal;

	}

////////

void EDIT_POLY::SetSizes()
	{
	int											hScrollMax, vScrollMax;
	RECT										rcWindow;

	GetWindowRect(m_hwndDraw, &rcWindow);
	GetBitmapSize(m_whichImage==I_IMAGE_NORMAL ? m_miNormal:m_miHighlight, &m_ptBitmapSize);

	hScrollMax = max((long)0, m_ptBitmapSize.x - rcWindow.right + rcWindow.left);
	vScrollMax = max((long)0, m_ptBitmapSize.y - rcWindow.bottom + rcWindow.top);
	hScrollMax += GetSystemMetrics(SM_CXVSCROLL); 
	vScrollMax += GetSystemMetrics(SM_CYHSCROLL); 

	SetScrollRange(m_hwndDraw, SB_HORZ, 0, hScrollMax, TRUE);
	SetScrollRange(m_hwndDraw, SB_VERT, 0, vScrollMax, TRUE);

	m_ptOffset.x = m_ptOffset.y = 0;
	SetScrollPos(m_hwndDraw, SB_HORZ, m_ptOffset.x, TRUE);
	SetScrollPos(m_hwndDraw, SB_VERT, m_ptOffset.y, TRUE);
	}

///////

void EDIT_POLY::WmPaintImage(
	HDC 										hdc)
	{	
	HDC											hdcMem;
	HDC											hdcScreen;
	HBITMAP									hbmpOld;
	LPBUTTON  *					ppButtons;
	USHORT									us;
	HPALETTE								hPalOld;

	//Select in the palette
	if (GetCurrentPaletteHandle())
		{
		hPalOld = SelectPalette(hdc, GetCurrentPaletteHandle(), TRUE);
		RealizePalette(hdc);
		}

	// Draw the bitmap first 
	hdcScreen = GetDC(NULL);
	hdcMem = CreateCompatibleDC(hdcScreen);
	hbmpOld = (HBITMAP)SelectObject(hdcMem, m_whichImage==I_IMAGE_NORMAL ? m_hBmpNormal : m_hBmpHighlight);
	BitBlt(hdc, 0, 0, m_ptBitmapSize.x, m_ptBitmapSize.y, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmpOld);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	//Now, draw the buttons	
	if (NumButtons())
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return);

		for(us=0; us<NumButtons(); us++)
			{
			ppButtons[us]->FrameButton(hdc, NULL);
			if (us == m_usButtonSelected)
				ppButtons[us]->InvertButton(hdc, NULL);
			}
		EndUsingButtons();
		}

	//restore the old palette if we put in a new one
	if (GetCurrentPaletteHandle())
		{
		SelectPalette(hdc, hPalOld, TRUE);
		RealizePalette(hdc);
		}

	}

//////

void EDIT_POLY::WmVScroll(
	WORD									wScrollCode, 
	int 										iScrollPos)
	{
  int                     y = GetScrollPos(m_hwndDraw, SB_VERT);
  int                     yMin, yMax;
  RECT                    rcClient;
	HDC											hdc;

  // scroll based on window dimensions
  GetClientRect(m_hwndDraw, &rcClient);
  GetScrollRange(m_hwndDraw, SB_VERT, &yMin, &yMax);

  switch(wScrollCode)
    {
    case SB_TOP:					y = yMin;      										break;
    case SB_BOTTOM:				y = yMax;													break;
    case SB_LINEDOWN:			y = min(y + I_SCROLL_LINE, yMax);	break;
    case SB_LINEUP:				y = max(y - I_SCROLL_LINE, yMin);	break;
    case SB_PAGEDOWN:     y = min(y + I_SCROLL_PAGE, yMax);	break;
    case SB_PAGEUP:      	y = max(y - I_SCROLL_PAGE, yMin);	break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:y = iScrollPos;										break;
    }

  if(y != GetScrollPos(m_hwndDraw, SB_VERT))
    {
		hdc = GetDC(m_hwndDraw);

		SetViewportOrgEx(hdc, m_ptOffset.x, -y, NULL);

    SetScrollPos(m_hwndDraw, SB_VERT, y, TRUE);
    ScrollWindow(m_hwndDraw, 0, -(m_ptOffset.y + y), NULL, NULL);
    UpdateWindow(m_hwndDraw);
		ReleaseDC(m_hwndDraw, hdc);
		m_ptOffset.y = -y;
    }
	}

//////

void EDIT_POLY::WmHScroll(
	WORD	 									wScrollCode, 
	int 										iScrollPos)
	{
  int                     x = GetScrollPos(m_hwndDraw, SB_HORZ);
  int                     xMin, xMax;
  RECT                    rcClient;
	HDC											hdc;

  // scroll based on window dimensions
  GetClientRect(m_hwndDraw, &rcClient);
  GetScrollRange(m_hwndDraw, SB_HORZ, &xMin, &xMax);

  switch(wScrollCode)
    {
    case SB_TOP:					x = xMin;      											break;
    case SB_BOTTOM:				x = xMax;      											break;
    case SB_LINEDOWN:			x = min(x + I_SCROLL_LINE, xMax);		break;
    case SB_LINEUP:				x = max(x - I_SCROLL_LINE, xMin);		break;
    case SB_PAGEDOWN:			x = min(x + I_SCROLL_PAGE, xMax);		break;
    case SB_PAGEUP:				x = max(x - I_SCROLL_PAGE, xMin);		break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:x = iScrollPos;											break;
    }

  if(x != GetScrollPos(m_hwndDraw, SB_HORZ))
    {
		hdc = GetDC(m_hwndDraw);

		SetViewportOrgEx(hdc, -x, m_ptOffset.y, NULL);

    SetScrollPos(m_hwndDraw, SB_HORZ, x, TRUE);
    ScrollWindow(m_hwndDraw, -(m_ptOffset.x + x), 0, NULL, NULL);
    UpdateWindow(m_hwndDraw);
		ReleaseDC(m_hwndDraw, hdc);
		m_ptOffset.x = -x;
    }
	}

//////

BOOL EDIT_POLY::ConfigureButton(
	USHORT									usIndex)
	{
	LPBUTTON  *					ppButtons;
	BOOL										rVal = FALSE;

	if (usIndex < NumButtons())
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return FALSE);
		rVal = ppButtons[usIndex]->Configure(usIndex);
		EndUsingButtons();
		}

	return rVal;
	}


BOOL EDIT_POLY::OffsetButton(
	HDC											hdc,
	USHORT									usIndex, 
	int 										dx, 
	int 										dy)
	{
	LPBUTTON  *					ppButtons;
	BOOL										rVal = FALSE;

	if (usIndex < NumButtons())
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return FALSE);

		FrameButton(usIndex, hdc);
		ppButtons[usIndex]->OffsetButton(dx, dy);
		FrameButton(usIndex, hdc);

		EndUsingButtons();
		rVal = TRUE;
		}

	return rVal;
	}

BOOL EDIT_POLY::FrameButton(
	USHORT									usIndex,
	HDC											hdc)
	{
	LPBUTTON  *					ppButtons;
	BOOL										rVal = FALSE;

	if (usIndex < NumButtons())
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return FALSE);

		ppButtons[usIndex]->FrameButton(hdc, NULL);

		EndUsingButtons();
		rVal = TRUE;
		}

	return rVal;
	}


BOOL EDIT_POLY::InvalidateButton(
	USHORT 									usIndex, 
	BOOL 										bErase)
	{
	LPBUTTON  *					ppButtons;
	BOOL										rVal = FALSE;

	if (usIndex < NumButtons())
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return FALSE);

		ppButtons[usIndex]->InvalidateButton(m_hwndDraw, bErase, &m_ptOffset);

		EndUsingButtons();
		rVal = TRUE;
		}

	return rVal;
	}


//////

BOOL EDIT_POLY::CreateNewButton()
	{
	LPBUTTON  *					ppButtons;
	BOOL										bShouldCreate = FALSE;
	BOOL										rVal = FALSE;
	size_t									totalSize;

	switch (m_currentTool)
		{
		case I_TOOL_RECT:
		case I_TOOL_ELLIPSE:
			if ( abs(m_ptTracking[0].x - m_ptTracking[1].x) > 1  &&  abs(m_ptTracking[0].y - m_ptTracking[1].y) > 1)
				bShouldCreate = TRUE;
			break;

		case I_TOOL_POLYGON:
		case I_TOOL_FREEHAND:
			if (m_usNumPoints > 2)
				bShouldCreate = TRUE;
			break;
		}

	if (bShouldCreate && IncrementNumButtons())
		{
		ppButtons = (LPBUTTON  *)AMemLock(m_miButtonsPointer);
		I_VERIFY_POINTER(ppButtons, return FALSE);

		totalSize = sizeof(BUTTON) + sizeof(REGION) + m_usNumPoints * sizeof(POINT);

		ppButtons[NumButtons()-1] = new(totalSize) BUTTON(m_uidNext, m_currentTool, m_ptTracking, m_usNumPoints);
		ppButtons[NumButtons()-1]->CreateRegionHandle();
		ppButtons[NumButtons()-1]->InvalidateButton(m_hwndDraw, TRUE, &m_ptOffset);
		AMemUnlock(m_miButtonsPointer);
		rVal = TRUE;
		}

	return rVal;
	}

//////

BOOL EDIT_POLY::DeleteSelectedButton()
	{
	LPBUTTON  *					ppButtons;
	BOOL										rVal=FALSE;

	if (m_usButtonSelected < m_usNumButtons )
		{
		ppButtons = BeginUsingButtons();
		I_VERIFY_POINTER(ppButtons, return rVal);
		delete ppButtons[m_usButtonSelected];
		MoveMemory(&ppButtons[m_usButtonSelected], &ppButtons[m_usButtonSelected+1], (m_usNumButtons-m_usButtonSelected-1) * sizeof(LPVOID) );
		EndUsingButtons();
		m_usNumButtons--;
		AMemSetSize(m_miButtonsPointer, sizeof(LPVOID) * m_usNumButtons);
		rVal = TRUE;
		m_usButtonSelected = I_UNDEFINED_INDEX;
		EnableProperControls();
		}
	return rVal;
	}

///////

LPBUTTON  * EDIT_POLY::BeginUsingButtons()
	{
	return (LPBUTTON  *)AMemLock(m_miButtonsPointer);
	}

//////

BOOL EDIT_POLY::EndUsingButtons()
	{
	AMemUnlock(m_miButtonsPointer); 
	return TRUE;
	}

//////

BOOL EDIT_POLY::IncrementNumButtons()
	{
	m_usNumButtons++;
	AMemSetSize(m_miButtonsPointer, sizeof(LPVOID) * NumButtons());
	m_uidNext += I_INCREMENT_NEXT_UID;
	return TRUE;
	}

//////

BOOL EDIT_POLY::RegisterWindowClasses() //static member function
	{
	WNDCLASS								wc;

	wc.style = CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC) EDIT_POLY::PolybuttonDrawWindow;
	wc.cbClsExtra	 = 	0;
	wc.cbWndExtra =  sizeof(LPVOID);
	wc.hInstance = g_hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND+1;
	wc.lpszMenuName	= NULL;
	wc.lpszClassName = g_szDrawClassName;

	RegisterClass(&wc);

	return TRUE;
	}

BOOL EDIT_POLY::UnregisterWindowClasses() //static member function
	{
	UnregisterClass(g_szDrawClassName, g_hInstance);
	return TRUE;
	}


//////////////////////////////////////////////////////
// This is the callback function for the            //
// draw window in the edit dialog.  The draw window //
// is the window you draw the buttons in.           //
//////////////////////////////////////////////////////
#define SetDrawWindowEPPointer(hwnd, ep) (SetWindowLong(hwnd, 0, (LONG)ep))
#define GetDrawWindowEPPointer(hwnd) ((LPEDIT_POLY)GetWindowLong(hwnd, 0))

LRESULT CALLBACK /*!!PORT!! was _far_ _pascal_*/  EDIT_POLY::PolybuttonDrawWindow(
	HWND										hWnd, 
	UINT 										msg, 
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LPEDIT_POLY							ep;
	POINT										ptMouse;
	PAINTSTRUCT							ps;

	switch(msg)
		{
		case WM_CREATE:
			ep = (LPEDIT_POLY) ((LPCREATESTRUCT)lParam)->lpCreateParams;
			SetDrawWindowEPPointer(hWnd, ep);
			break;

		case WM_PAINT:
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			ep->WmPaintImage( BeginPaint(hWnd, &ps) );
			EndPaint(hWnd, &ps);
			return 0;

		case WM_VSCROLL: //!!PORT!!
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			///ep->WmVScroll(wParam, LOWORD(lParam)); 
			ep->WmVScroll(LOWORD(wParam), HIWORD(wParam));
			break;

		case WM_HSCROLL: //!!PORT!!
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			///ep->WmHScroll(wParam, LOWORD(lParam));
			ep->WmHScroll(LOWORD(wParam), HIWORD(wParam));
			break;

		case WM_LBUTTONDOWN:
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			ptMouse.x = LOWORD(lParam);
			ptMouse.y = HIWORD(lParam);
			ep->MouseDown(&ptMouse, wParam);
			if (ep->m_bTracking)
				SetCapture(hWnd);
			return 0;

		case WM_MOUSEMOVE:
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			ptMouse.x = LOWORD(lParam);
			ptMouse.y = HIWORD(lParam);
			ep->MouseMove(&ptMouse, wParam);
			return 0;

		case WM_LBUTTONUP:
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			ptMouse.x = LOWORD(lParam);
			ptMouse.y = HIWORD(lParam);
			ep->MouseUp(&ptMouse, wParam);
			if (!ep->m_bTracking)
				ReleaseCapture();
			return 0;

		case WM_LBUTTONDBLCLK:
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			ptMouse.x = LOWORD(lParam);
			ptMouse.y = HIWORD(lParam);
			ep->MouseDoubleClick(&ptMouse, wParam);
			if (!ep->m_bTracking)
				ReleaseCapture();
			return 0;

		case WM_CANCELMODE:
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			ep->m_bTracking = FALSE;
			ReleaseCapture();
			break;

		case WM_SETCURSOR:
			ep = GetDrawWindowEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			SetCursor(LoadCursor(NULL, ep->GetCurrentTool()==I_TOOL_ARROW ? IDC_ARROW:IDC_CROSS));
			return TRUE;

		}

	return DefWindowProc(hWnd, msg, wParam, lParam);
	}


//////

#define SetDialogEPPointer(hwnd, ep) (SetWindowLong(hwnd, DWL_USER, (LONG)ep) )
#define GetDialogEPPointer(hwnd) ((LPEDIT_POLY)GetWindowLong(hwnd, DWL_USER))

LRESULT CALLBACK /*!!PORT!! was _far_ _pascal_*/  EDIT_POLY::PolybuttonEditDlg(
	HWND 										hWnd, 
	UINT 										msg, 
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LPEDIT_POLY							ep;
	LPDRAWITEMSTRUCT				di;
	HICON										hIcon;
	BOOL										bSet;
	HDC											hdc;
	BOOL										rVal = FALSE;
	HPALETTE								hPalOld;

	switch(msg)
		{
		case WM_INITDIALOG:
			ep = new EDIT_POLY( (AOBJECTID)lParam, hWnd);
			SetDialogEPPointer(hWnd, ep);
			ep->InitDialog();
			break;

		case WM_DESTROY:
			ep = GetDialogEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			delete ep;
			return 0;

		case WM_DRAWITEM:
			ep = GetDialogEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			di = (LPDRAWITEMSTRUCT)lParam;
			switch (di->CtlID)
				{
				case RB_TOOL_ARROW:			bSet = ep->m_currentTool == I_TOOL_ARROW;		hIcon = LoadIcon(g_hInstance, "ICON_ARROW"); break;
				case RB_TOOL_RECT:			bSet = ep->m_currentTool == I_TOOL_RECT; 		hIcon = LoadIcon(g_hInstance, "ICON_SQUARE"); break;
				case RB_TOOL_ELLIPSE:		bSet = ep->m_currentTool == I_TOOL_ELLIPSE;	hIcon = LoadIcon(g_hInstance, "ICON_CIRCLE"); break;
				case RB_TOOL_POLYGON:		bSet = ep->m_currentTool == I_TOOL_POLYGON;	hIcon = LoadIcon(g_hInstance, "ICON_POLYGON"); break;
				case RB_TOOL_FREEHAND:	bSet = ep->m_currentTool == I_TOOL_FREEHAND;hIcon = LoadIcon(g_hInstance, "ICON_FREEHAND"); break;
				default: return FALSE;
				}
			DrawIcon(di->hDC, 0, 0, hIcon);
			DestroyIcon(hIcon);
			if (bSet || di->itemState & ODS_SELECTED)
				InvertRect(di->hDC, &di->rcItem);
			break;

		case WM_ACTIVATE: //!!PORT!!
			ep = GetDialogEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			ep->EnableProperControls();
			break;

		case WM_PALETTECHANGED:
			if ((HWND)wParam == hWnd)
				break;
			//else fall through
		case WM_QUERYNEWPALETTE:
			ep = GetDialogEPPointer(hWnd);
			if (ep->GetCurrentPaletteHandle())
				{
				hdc = GetDC(hWnd);
	  	  hPalOld = SelectPalette(hdc, ep->GetCurrentPaletteHandle(), FALSE);
	  	  rVal = RealizePalette(hdc);         
	    	SelectPalette(hdc, hPalOld, TRUE);
	  	  RealizePalette(hdc);
	  	  ReleaseDC(hWnd, hdc);
	 	  	if (rVal)                            
	    		InvalidateRect(hWnd, NULL, TRUE);
				}
      break;

		case WM_COMMAND: //!!PORT!!
			ep = GetDialogEPPointer(hWnd);
			I_VERIFY_POINTER(ep, break);
			///switch(wParam)
			switch (GET_WM_COMMAND_ID(wParam, lParam))
				{
				case PB_OK:
					ep->Store();
					EndDialog(hWnd, 1);
					break;
					
				case PB_CANCEL:
					EndDialog(hWnd, 0);
					break;

				case PB_HELP:
					WinHelp (hWnd, "awuspoly.hlp", HELP_CONTEXT, HELPID_OBJD) ;
					break;

				case PB_CONFIGURE:
					ep->ConfigureButton(); 
					break;

				case PB_DELETE:
					if (ep->DeleteSelectedButton())
						InvalidateRect(ep->m_hwndDraw, NULL, TRUE);
					break;

				case PB_LOAD:
					ep->Load();
					break;

				case PB_PASTE:
					ep->Paste();
					break;

				case RB_NORMAL:
					ep->SetWhichImage(I_IMAGE_NORMAL);
					InvalidateRect(ep->m_hwndDraw, NULL, TRUE);
					ep->SetSizes();
					break;
				case RB_HIGHLIGHT:
					ep->SetWhichImage(I_IMAGE_HIGHLIGHT);
					InvalidateRect(ep->m_hwndDraw, NULL, TRUE);
					ep->SetSizes();
					break;

				case RB_TOOL_ARROW:
					ep->SetCurrentTool(I_TOOL_ARROW);
					break;
				case RB_TOOL_RECT:
					ep->SetCurrentTool(I_TOOL_RECT);
					break;
				case RB_TOOL_ELLIPSE:
					ep->SetCurrentTool(I_TOOL_ELLIPSE);
					break;
				case RB_TOOL_POLYGON:
					ep->SetCurrentTool(I_TOOL_POLYGON);
					break;
				case RB_TOOL_FREEHAND:
					ep->SetCurrentTool(I_TOOL_FREEHAND);
					break;
				}
		}

									
  return rVal;         
	}
