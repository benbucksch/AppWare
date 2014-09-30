/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define		STRICT
#define		ASTRICT

#include <windows.h>
#include <string.h>
#include "position.h"
#include "colorpal.h"
#include "iconedit.h"
#include "winlist.h"

#define COLORPALETTEWNDCLASS	"SERIUS_ICON_COLOR_PALETTE"

#define MAXHORIZCOUNT 9


extern HANDLE Gbl_hCursor;
extern int xCtrlExt;
extern int yCtrlExt;


long FAR PASCAL _export ColorPaletteWndProc(
	HWND   hWnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////
// Allocate and initialize static class members
/////////////////////////////////////////////////////////////////////////////

HANDLE COLORPALETTE::hAppInstance = NULL;

WINDOWLIST far* COLORPALETTE::windowList = NULL;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: COLORPALETTE
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

COLORPALETTE::COLORPALETTE(
	HWND	 						hWndParent,
	HANDLE 						hAppInstance,
	HANDLE 						hPrevInstance,
	ICONEDITOR far*		iconEditor,
	PIXELWINDOW far*	pixelWindow,
	short int					colorCount)
	{
	BOOL	status;
	COLORPALETTE::hAppInstance = hAppInstance;

	if (!hPrevInstance && !windowList)
		status = registerWindowClass();
	else
		status = TRUE;

	if (!windowList)
		windowList = new WINDOWLIST();

	if (status)
		{
		this->hWndParent = hWndParent;
		horizCount = colorCount/2 + 1;

		hWndThis = CreateWindow((LPSTR)COLORPALETTEWNDCLASS,
									(LPSTR)"ICON EDITOR",
									WS_CHILD|WS_BORDER,
									PALLEFT + BUTTON2WIDTH / 4,
									PALTOP, PALBOXWIDTH*horizCount+1, PALHEIGHT,
									hWndParent,
									NULL,
									(HINSTANCE)hAppInstance,
									(LPSTR)this);

		if (hWndThis)
			{
			this->iconEditor = iconEditor;
			this->pixelWindow = pixelWindow;

			InitColorInfo(colorCount);

			UpdateIndices();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitColorInfo
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void COLORPALETTE::InitColorInfo(
	short int	colorCount)
	{

	if (colorCount == 16)
		{
		colors[0][0].rgb = RGB(0x00, 0x00, 0x00);
		colors[1][0].rgb = RGB(0x00, 0x00, 0xFF);
		colors[2][0].rgb = RGB(0x00, 0xFF, 0x00);
		colors[3][0].rgb = RGB(0xFF, 0x00, 0x00);
		colors[4][0].rgb = RGB(0x00, 0x00, 0x80);
		colors[5][0].rgb = RGB(0x00, 0x80, 0x00);
		colors[6][0].rgb = RGB(0x80, 0x00, 0x00);
		colors[7][0].rgb = RGB(0x80, 0x80, 0x80);
		colors[7][1].rgb = RGB(0xC0, 0xC0, 0xC0);
		colors[6][1].rgb = RGB(0x80, 0x80, 0x00);
		colors[5][1].rgb = RGB(0x00, 0x80, 0x80);
		colors[4][1].rgb = RGB(0x80, 0x00, 0x80);
		colors[3][1].rgb = RGB(0xFF, 0xFF, 0x00);
		colors[2][1].rgb = RGB(0x00, 0xFF, 0xFF);
		colors[1][1].rgb = RGB(0xFF, 0x00, 0xFF);
		colors[0][1].rgb = RGB(0xFF, 0xFF, 0xFF);
		}
	else if (colorCount == 2)
		{
		colors[0][0].rgb = RGB(0x00, 0x00, 0x00);
		colors[0][1].rgb = RGB(0xFF, 0xFF, 0xFF);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateIndices
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void COLORPALETTE::UpdateIndices(
	void)
	{
	short	x;

	for (x=0; x<horizCount-1; x++)
		{
		colors[x][0].index = pixelWindow->GetColorIndex(colors[x][0].rgb);
		}

	for (x=0; x<horizCount-1; x++)
		{
		colors[x][1].index = pixelWindow->GetColorIndex(colors[x][1].rgb);
		}

	colors[horizCount-1][0].index = SCREENCOLOR;
	colors[horizCount-1][1].index = INVERTEDCOLOR;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~COLORPALETTE (destructor)
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

COLORPALETTE::~COLORPALETTE(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: registerWindowClass
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL COLORPALETTE::registerWindowClass(
	void
	){
	WNDCLASS  	wc;
	BOOL				status;

	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc = ColorPaletteWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(COLORPALETTE far*);
	wc.hInstance = (HINSTANCE)hAppInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = COLORPALETTEWNDCLASS;

	status = RegisterClass((LPWNDCLASS)(&wc));

	return(status);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: WindowProc
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

long COLORPALETTE::WindowProc(
	HWND 				hWnd,
	unsigned 		message,
	WORD 				wParam,
	LONG 				lParam
	){
	BOOL 				msgHandled = TRUE;
	short int		x, y;

	switch (message)
		{
		case WM_PAINT:
			Draw();
			break;

		case WM_SETCURSOR: {
			SetCursor((HICON)Gbl_hCursor);
			return TRUE;
			}
		case WM_LBUTTONDOWN:
			x = LOWORD(lParam)/PALBOXWIDTH;
			y = HIWORD(lParam)/PALBOXHEIGHT;
			iconEditor->SetCurrentColor(colors[x][y].index);
      // Send it to the parent window
			SendMessage(GetParent(hWnd), WM_LBUTTONDOWN, wParam, lParam);
			break;

		case WM_RBUTTONDOWN:
			x = LOWORD(lParam)/PALBOXWIDTH;
			y = HIWORD(lParam)/PALBOXHEIGHT;
			pixelWindow->SetBackgroundColor(colors[x][y].index);
			break;

		case WM_MOUSEMOVE:
			break;

		default:
			msgHandled = FALSE;
		}

	if (msgHandled)
		return (0L);
	else
		return DefWindowProc(hWnd, message, wParam, lParam);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ColorPaletteWndProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: Assumes the static windowList pointer in  class is
//								initialized.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
long FAR PASCAL _export ColorPaletteWndProc(
	HWND   hWnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	COLORPALETTE far* colorPalette;
	LPCREATESTRUCT  lpcs;
	long						result;

	colorPalette =
			(COLORPALETTE far*)(COLORPALETTE::windowList->GetObject(hWnd));

	if (colorPalette != 0)
		{
		result = colorPalette->WindowProc(hWnd, message, wParam, lParam);
		if (message == WM_DESTROY)
			{
			COLORPALETTE::windowList->Remove(hWnd);
			delete colorPalette;
			}
		}
	else
		switch (message)
			{
			case WM_CREATE:
				{
				lpcs = (LPCREATESTRUCT) lParam;
				colorPalette = (COLORPALETTE far*)(lpcs->lpCreateParams);
				COLORPALETTE::windowList->Add(hWnd, (LPSTR)colorPalette);
				result = colorPalette->WindowProc(hWnd, message, wParam, lParam);
				}
			default:
				result = DefWindowProc(hWnd, message, wParam, lParam);
			}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Show
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void COLORPALETTE::Show(
	void
	){

	ShowWindow(hWndThis, SW_SHOW);
	UpdateWindow(hWndThis);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Draw
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void COLORPALETTE::Draw(
	void)
	{
	PAINTSTRUCT	paintStruct;
	HBRUSH 			hBrush;
	HPEN				hPen;
	HPEN				hOldPen;
	HBRUSH			hOldBrush;
	HDC					hDC;
	short int		x,y;

	hDC = BeginPaint(hWndThis, (LPPAINTSTRUCT)(&paintStruct));

	if (0 != (hPen = (HPEN)GetStockObject(BLACK_PEN)))
		{
		hOldPen = (HPEN)SelectObject(hDC, hPen);
		for (x=1; x<horizCount; x++)
			{
			MoveTo(hDC, PALBOXWIDTH*x-1, 0);
			LineTo(hDC, PALBOXWIDTH*x-1, 2*PALBOXWIDTH);
			}
		for (y=1; y<2; y++)
			{
			MoveTo(hDC, 0, PALBOXHEIGHT*y-1);
			LineTo(hDC, PALBOXWIDTH*horizCount, PALBOXHEIGHT*y-1);
			}

		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);
		}

	for (y=0; y<2; y++)
		for (x=0; x<horizCount; x++)
			{
			if (colors[x][y].index == SCREENCOLOR)
				DrawScreenColorBox(hDC, PALBOXWIDTH*x, PALBOXHEIGHT*y, FALSE);
			else if (colors[x][y].index == INVERTEDCOLOR)
				DrawScreenColorBox(hDC, PALBOXWIDTH*x, PALBOXHEIGHT*y, TRUE);
			else
				{
				hBrush = CreateSolidBrush(colors[x][y].rgb);
				hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
				hPen = CreatePen(PS_SOLID, 1, colors[x][y].rgb);
				hOldPen = (HPEN)SelectObject(hDC, hPen);

				Rectangle(hDC, PALBOXWIDTH*x, PALBOXHEIGHT*y, PALBOXWIDTH*(x+1)-1,
									PALBOXHEIGHT*(y+1)-1);

				SelectObject(hDC, hOldPen);
				SelectObject(hDC, hOldBrush);

				DeleteObject(hBrush);
				DeleteObject(hPen);
				}
			}

	EndPaint(hWndThis, (LPPAINTSTRUCT)(&paintStruct));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawScreenColorBox
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void COLORPALETTE::DrawScreenColorBox(
	HDC			hDC,
	short		left,
	short		top,
	BOOL		invertedFlag)
	{
	HPEN  	hPen;
	HPEN		hOldPen;
	HBRUSH	hBrush;
	HBRUSH	hOldBrush;
	HFONT		hFont;
	HFONT		hOldFont;
	short		textHeight;

	hPen = (HPEN)GetStockObject(WHITE_BRUSH);
	hBrush = (HBRUSH)GetStockObject(WHITE_PEN);

	hOldPen = (HPEN)SelectObject(hDC, hPen);
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, left, top, PALBOXWIDTH+left-1, PALBOXHEIGHT+top-1);

	hPen = (HPEN)GetStockObject(BLACK_PEN);

	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);

	SetBkMode(hDC, TRANSPARENT);
	hFont = CreateFont(13, 0, 0, 0, 10, FALSE, FALSE, FALSE, ANSI_CHARSET,
	 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	 VARIABLE_PITCH|FF_SWISS, "Helv");
	hOldFont = (HFONT)SelectObject(hDC, hFont);
	SetTextAlign(hDC, TA_CENTER);
	textHeight = HIWORD(GetTextExtent(hDC, "STRING", 6));
	if (invertedFlag)
		TextOut(hDC, left+PALBOXWIDTH/2-1,
		 top+PALBOXHEIGHT/2-textHeight/2-1, "I", 1);
	else
		TextOut(hDC, left+PALBOXWIDTH/2-1,
		 top+PALBOXHEIGHT/2-textHeight/2-1, "S", 1);
	SetTextAlign(hDC, TA_LEFT|TA_TOP|TA_NOUPDATECP);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);

	}

