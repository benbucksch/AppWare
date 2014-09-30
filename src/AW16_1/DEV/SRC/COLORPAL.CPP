#include <almsys.h>
#include <windowsx.h>
#include <string.h>
#include "position.h"
#include "colorpal.h"
#include "iconedit.h"
#include "winlist.h"
#include "pixelwin.h"

#define COLORPALETTEWNDCLASS	"SERIUS_ICON_COLOR_PALETTE"

#define MAXHORIZCOUNT 9

LRESULT I_CALLBACK ColorPaletteWndProc(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////
// Allocate and initialize static class members
/////////////////////////////////////////////////////////////////////////////

HINSTANCE COLORPALETTE::hAppInstance = NULL;

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
	HINSTANCE					hAppInstance,
	HINSTANCE					hPrevInstance,
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

		hWndThis = CreateWindow(COLORPALETTEWNDCLASS,
									"ICON EDITOR",
									WS_CHILD|WS_BORDER,
									PALLEFT + (PALBOXWIDTH*(MAXHORIZCOUNT-horizCount)/2),
									PALTOP, PALBOXWIDTH*horizCount+1, PALHEIGHT,
									hWndParent,
									NULL,
									hAppInstance,
									(LPSTR)this);

		if (hWndThis)
			{
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
// FUNCTION....: UpdateColors
//
// CLASS.......: COLORPALETTE
//
// DESCRIPTION.: Update the color values for the palette entries based on the
//               icons color palette.
//
/////////////////////////////////////////////////////////////////////////////
void COLORPALETTE::UpdateColors(
	void)
	{
	short	x;

	for (x=0; x<horizCount-1; x++)
		{
		colors[x][0].rgb = pixelWindow->GetColor(colors[x][0].index);
		}

	for (x=0; x<horizCount-1; x++)
		{
		colors[x][1].rgb = pixelWindow->GetColor(colors[x][1].index);
		}

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
	wc.hInstance = hAppInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
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

		case WM_LBUTTONDOWN:
			x = LOWORD(lParam)/PALBOXWIDTH;
			y = HIWORD(lParam)/PALBOXHEIGHT;
			ICONEDITOR::SetCurrentColor(colors[x][y].index);
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
LRESULT I_CALLBACK ColorPaletteWndProc(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	COLORPALETTE far* colorPalette;
	LPCREATESTRUCT  lpcs;
	long						result;

  colorPalette =
			(COLORPALETTE far*)(COLORPALETTE::windowList->GetObject(hwnd));
	if (colorPalette)
		{
		result = colorPalette->WindowProc(hwnd, message, wParam, lParam);
		if (message == WM_DESTROY)
			{
			COLORPALETTE::windowList->Remove(hwnd);
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
				COLORPALETTE::windowList->Add(hwnd, (LPSTR)colorPalette);
				result = colorPalette->WindowProc(hwnd, message, wParam, lParam);
				}
			default:
				result = DefWindowProc(hwnd, message, wParam, lParam);
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

  hPen = GetStockPen(BLACK_PEN);
	if (hPen)
		{
		hOldPen = SelectPen(hDC, hPen);
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

		SelectPen(hDC, hOldPen);
		DeletePen(hPen);
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
				hOldBrush = SelectBrush(hDC, hBrush);
				hPen = CreatePen(PS_SOLID, 1, colors[x][y].rgb);
				hOldPen = SelectPen(hDC, hPen);

				Rectangle(hDC, PALBOXWIDTH*x, PALBOXHEIGHT*y, PALBOXWIDTH*(x+1)-1,
									PALBOXHEIGHT*(y+1)-1);

				SelectPen(hDC, hOldPen);
				SelectBrush(hDC, hOldBrush);

				DeleteBrush(hBrush);
				DeletePen(hPen);
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

	hPen = GetStockPen(WHITE_BRUSH);
	hBrush = GetStockBrush(WHITE_PEN);

	hOldPen = SelectPen(hDC, hPen);
	hOldBrush = SelectBrush(hDC, hBrush);

	Rectangle(hDC, left, top, PALBOXWIDTH+left-1, PALBOXHEIGHT+top-1);

	hPen = GetStockPen(BLACK_PEN);

	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);

	SetBkMode(hDC, TRANSPARENT);
	hFont = CreateFont(13, 0, 0, 0, 10, FALSE, FALSE, FALSE, ANSI_CHARSET,
	 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	 VARIABLE_PITCH|FF_SWISS, "Helv");
	hOldFont = SelectFont(hDC, hFont);
	SetTextAlign(hDC, TA_CENTER);
	textHeight = HIWORD(GetTextExtent(hDC, "STRING", 6));
	if (invertedFlag)
		TextOut(hDC, left+PALBOXWIDTH/2-1,
		 top+PALBOXHEIGHT/2-textHeight/2-1, "I", 1);
	else
		TextOut(hDC, left+PALBOXWIDTH/2-1,
		 top+PALBOXHEIGHT/2-textHeight/2-1, "S", 1);
	SetTextAlign(hDC, TA_LEFT|TA_TOP|TA_NOUPDATECP);
	SelectFont(hDC, hOldFont);
	DeleteFont(hFont);

	SelectPen(hDC, hOldPen);
	SelectBrush(hDC, hOldBrush);

	}

