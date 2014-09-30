#include <math.h>
#include "almsys.h"
#include <windowsx.h>
#include "intdebug.h"

#include "pixelwin.h"
#include "colorpal.h"
#include "winlist.h"
#include "sericon.h"

#define PIXELWINDOWWNDCLASS	"APPWARE_PIXEL_WINDOW"

#define COLORPIXELSPERBYTE	2
#define BWPIXELSPERBYTE			8
#define MASKPIXELSPERBYTE		8
#define BITSPERBYTE					8

LRESULT CALLBACK PixelWindowWndProc(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////
// Allocate and initialize static class members
/////////////////////////////////////////////////////////////////////////////

HINSTANCE PIXELWINDOW::hAppInstance = NULL;

WINDOWLIST* PIXELWINDOW::windowList = NULL;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: PIXELWINDOW
//
// CLASS.......: PIXELWINDOW
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

PIXELWINDOW::PIXELWINDOW(
	HWND	 				hWndParent,
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	short int			size,
	short int			pixelsPerByte)
	{
	BOOL						status;
	short 					arraySize;
	short 					i;

	PIXELWINDOW::hAppInstance = hAppInstance;

	if (!hPrevInstance && !windowList)
		status = registerWindowClass();
	else
		status = TRUE;

	if (!windowList)
		windowList = new WINDOWLIST();

	if (status)
		{
		this->hWndParent = hWndParent;

		hWndPixelWindow = CreateWindow((LPSTR)PIXELWINDOWWNDCLASS,
									(LPSTR)"ICON EDITOR",
									WS_CHILD|WS_BORDER,
									PIXELWNDLEFT, PIXELWNDTOP, PIXELWNDWIDTH, PIXELWNDHEIGHT,
									hWndParent,
									NULL,
									hAppInstance,
									(LPSTR)this);

		if (hWndPixelWindow)
			{
			hSmallS = LoadBitmap(hAppInstance, "SMALLS");
			hSmallI = LoadBitmap(hAppInstance, "SMALLI");

			this->size = size;
			this->pixelsPerByte = pixelsPerByte;
			doingDrag = FALSE;

			//calculate array size so each scan line ends on a LONG boundary

			imageScanLineBytes = (short int)(((size/pixelsPerByte/sizeof(long)) +
			 (((size/pixelsPerByte)%sizeof(long)==0)?0:1))*sizeof(long));

			arraySize = (short)(imageScanLineBytes*size);
			pImageArray = (LPBYTE)GlobalAlloc(GMEM_FIXED, arraySize);

			//set everything to white assuming white is at color table entry 15
			//for color icons and entry 1 for b&w icons.

			for (i=0; i<arraySize; i++)
				pImageArray[i] = 0xFF;

			imageDIBinfoHdr.biSize = (long)sizeof(BITMAPINFOHEADER);
			imageDIBinfoHdr.biWidth = size;
			imageDIBinfoHdr.biHeight = size;
			imageDIBinfoHdr.biPlanes = 1;
			imageDIBinfoHdr.biBitCount = (WORD)(BITSPERBYTE/pixelsPerByte);
			imageDIBinfoHdr.biCompression = 0L;
			imageDIBinfoHdr.biSizeImage = 0L;
			imageDIBinfoHdr.biXPelsPerMeter = 0L;
			imageDIBinfoHdr.biYPelsPerMeter = 0L;
			imageDIBinfoHdr.biClrUsed = 0L;
			imageDIBinfoHdr.biClrImportant = 0L;

			imageColors[0].rgbRed = 0x00;
			imageColors[0].rgbGreen = 0x00;
			imageColors[0].rgbBlue = 0x00;
			imageColors[0].rgbReserved = 0;

			if (pixelsPerByte == COLORPIXELSPERBYTE)
				{
				backgroundColor = 8;
				imageColors[1].rgbRed = 0x00;
				imageColors[1].rgbGreen = 0x00;
				imageColors[1].rgbBlue = 0xFF;
				imageColors[1].rgbReserved = 0;

				imageColors[2].rgbRed = 0x00;
				imageColors[2].rgbGreen = 0xFF;
				imageColors[2].rgbBlue = 0x00;
				imageColors[2].rgbReserved = 0;

				imageColors[3].rgbRed = 0xFF;
				imageColors[3].rgbGreen = 0x00;
				imageColors[3].rgbBlue = 0x00;
				imageColors[3].rgbReserved = 0;

				imageColors[4].rgbRed = 0x00;
				imageColors[4].rgbGreen = 0x00;
				imageColors[4].rgbBlue = 0x80;
				imageColors[4].rgbReserved = 0;

				imageColors[5].rgbRed = 0x00;
				imageColors[5].rgbGreen = 0x80;
				imageColors[5].rgbBlue = 0x00;
				imageColors[5].rgbReserved = 0;

				imageColors[6].rgbRed = 0x80;
				imageColors[6].rgbGreen = 0x00;
				imageColors[6].rgbBlue = 0x00;
				imageColors[6].rgbReserved = 0;

				imageColors[7].rgbRed = 0x80;
				imageColors[7].rgbGreen = 0x80;
				imageColors[7].rgbBlue = 0x80;
				imageColors[7].rgbReserved = 0;

				imageColors[8].rgbRed = 0xC0;
				imageColors[8].rgbGreen = 0xC0;
				imageColors[8].rgbBlue = 0xC0;
				imageColors[8].rgbReserved = 0;

				imageColors[9].rgbRed = 0x80;
				imageColors[9].rgbGreen = 0x80;
				imageColors[9].rgbBlue = 0x00;
				imageColors[9].rgbReserved = 0;

				imageColors[10].rgbRed = 0x00;
				imageColors[10].rgbGreen = 0x80;
				imageColors[10].rgbBlue = 0x80;
				imageColors[10].rgbReserved = 0;

				imageColors[11].rgbRed = 0x80;
				imageColors[11].rgbGreen = 0x00;
				imageColors[11].rgbBlue = 0x80;
				imageColors[11].rgbReserved = 0;

				imageColors[12].rgbRed = 0xFF;
				imageColors[12].rgbGreen = 0xFF;
				imageColors[12].rgbBlue = 0x00;
				imageColors[12].rgbReserved = 0;

				imageColors[13].rgbRed = 0x00;
				imageColors[13].rgbGreen = 0xFF;
				imageColors[13].rgbBlue = 0xFF;
				imageColors[13].rgbReserved = 0;

				imageColors[14].rgbRed = 0xFF;
				imageColors[14].rgbGreen = 0x00;
				imageColors[14].rgbBlue = 0xFF;
				imageColors[14].rgbReserved = 0;

				imageColors[15].rgbRed = 0xFF;
				imageColors[15].rgbGreen = 0xFF;
				imageColors[15].rgbBlue = 0xFF;
				imageColors[15].rgbReserved = 0;
				}
			else
				{
				backgroundColor = 1;
				imageColors[1].rgbRed = 0xFF;
				imageColors[1].rgbGreen = 0xFF;
				imageColors[1].rgbBlue = 0xFF;
				imageColors[1].rgbReserved = 0;
				}


			maskScanLineBytes = (short int)(((size/MASKPIXELSPERBYTE/sizeof(long)) +
			 (((size/MASKPIXELSPERBYTE)%sizeof(long)==0)?0:1))*sizeof(long));

			arraySize = (short)(maskScanLineBytes*size);
			pMaskArray = (LPBYTE) GlobalAlloc(GMEM_FIXED, arraySize);

			for (i=0; i<arraySize; i++)				//set mask to opaque
				pMaskArray[i] = 0x00;

			maskDIBinfoHdr.biSize = (long)sizeof(BITMAPINFOHEADER);
			maskDIBinfoHdr.biWidth = size;
			maskDIBinfoHdr.biHeight = size;
			maskDIBinfoHdr.biPlanes = 1;
			maskDIBinfoHdr.biBitCount = BITSPERBYTE/MASKPIXELSPERBYTE;
			maskDIBinfoHdr.biCompression = 0L;
			maskDIBinfoHdr.biSizeImage = 0L;
			maskDIBinfoHdr.biXPelsPerMeter = 0L;
			maskDIBinfoHdr.biYPelsPerMeter = 0L;
			maskDIBinfoHdr.biClrUsed = 0L;
			maskDIBinfoHdr.biClrImportant = 0L;

			maskColors[0].rgbRed = 0;
			maskColors[0].rgbGreen = 0;
			maskColors[0].rgbBlue = 0;
			maskColors[0].rgbReserved = 0;
			maskColors[1].rgbRed = 0xFF;
			maskColors[1].rgbGreen = 0xFF;
			maskColors[1].rgbBlue = 0xFF;
			maskColors[1].rgbReserved = 0;

			hPalette = 0;
			CreateLogicalPalette();

			InitPixelMask();

			currentColor = 0;

			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitPixelMask
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::InitPixelMask(
	void)
	{

	if (pixelsPerByte == COLORPIXELSPERBYTE)
		{
		pixelMask[0] = 0X0F;
		pixelMask[1] = 0XF0;
		}
	else
		{
		pixelMask[0] = (BYTE)~0X80;
		pixelMask[1] = (BYTE)~0X40;
		pixelMask[2] = (BYTE)~0X20;
		pixelMask[3] = (BYTE)~0X10;
		pixelMask[4] = (BYTE)~0X08;
		pixelMask[5] = (BYTE)~0X04;
		pixelMask[6] = (BYTE)~0X02;
		pixelMask[7] = (BYTE)~0X01;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CreateLogicalPalette
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::CreateLogicalPalette(
	void)
	{
	LPLOGPALETTE 	 	pPalStruct;
	short						i;
	unsigned short  colorCount;

	colorCount = (unsigned short) (0X01 << (BITSPERBYTE/pixelsPerByte));

	if (hPalette != 0)
		DeleteObject(hPalette);

	pPalStruct = (LPLOGPALETTE) GlobalAlloc(GMEM_FIXED,
											sizeof(LOGPALETTE)+colorCount*sizeof(PALETTEENTRY));

	pPalStruct->palVersion = 0x300;
	pPalStruct->palNumEntries = colorCount;

	for (i=0; i<colorCount; i++)
		{
		pPalStruct->palPalEntry[i].peRed = imageColors[i].rgbRed;
		pPalStruct->palPalEntry[i].peGreen = imageColors[i].rgbGreen;
		pPalStruct->palPalEntry[i].peBlue = imageColors[i].rgbBlue;
		pPalStruct->palPalEntry[i].peFlags = 0;
		}

	hPalette = CreatePalette(pPalStruct);

	GlobalFree((HGLOBAL) pPalStruct);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~PIXELWINDOW (destructor)
//
// CLASS.......: PIXELWINDOW
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

PIXELWINDOW::~PIXELWINDOW(
	void)
	{

	GlobalFree((HGLOBAL) pMaskArray);

	GlobalFree((HGLOBAL) pImageArray);

	DeleteObject(hSmallI);
	DeleteObject(hSmallS);
	DeleteObject(hPalette);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: registerWindowClass
//
// CLASS.......: PIXELWINDOW
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
BOOL PIXELWINDOW::registerWindowClass(
	void
	){
	WNDCLASS  	wc;
	BOOL				status;

	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc = PixelWindowWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(PIXELWINDOW *);
	wc.hInstance = hAppInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = PIXELWINDOWWNDCLASS;

	status = RegisterClass(&wc);

	return(status);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: WindowProc
//
// CLASS.......: PIXELWINDOW
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

LRESULT PIXELWINDOW::WindowProc(
	HWND 				hWnd,
	UINT		 		message,
	WPARAM			wParam,
	LPARAM			lParam
	){
	BOOL 				msgHandled = TRUE;
	short int		newX;
	short int		newY;
	RECT				rectClient;

	switch (message)
		{
		case WM_PAINT:
			Draw();
			break;

		case WM_LBUTTONDOWN:
			prevX = startX = (short)((short)LOWORD(lParam)/PIXELWIDTH);
			prevY = startY = (short)((short)HIWORD(lParam)/PIXELHEIGHT);
			doingDrag = TRUE;
			dirtyFlag = TRUE;
			SetCapture(hWndPixelWindow);
			DragLine(startX, startY, startX, startY);
			break;

		case WM_MOUSEMOVE:
			if ((wParam & MK_LBUTTON) && doingDrag)
				{
				GetClientRect(hWndPixelWindow, &rectClient);
				newX = (short)((short)LOWORD(lParam)/PIXELWIDTH);
				newY = (short)((short)HIWORD(lParam)/PIXELHEIGHT);
				if (newX < rectClient.left/PIXELWIDTH)
					newX = (short int)(rectClient.left/PIXELWIDTH);
				else if (newX > rectClient.right/PIXELWIDTH)
					newX = (short int)(rectClient.right/PIXELWIDTH);
				if (newY < rectClient.top/PIXELHEIGHT)
					newY = (short int)(rectClient.top/PIXELHEIGHT);
				else if (newY > rectClient.bottom/PIXELHEIGHT)
					newY = (short int)(rectClient.bottom/PIXELHEIGHT);
				if (newX!=prevX || newY!=prevY)
					{
					RestoreLine(startX, startY, prevX, prevY);
					DragLine(startX, startY, prevX=newX, prevY=newY);
					}
				}
			else
				msgHandled = FALSE;
			break;

		case WM_LBUTTONUP:
			if (doingDrag)
				{
				Line(startX, startY, prevX, prevY);
				doingDrag = FALSE;
				ReleaseCapture();
				}
			else
				msgHandled = FALSE;
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
// FUNCTION....: PixelWindowWndProc
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
LRESULT CALLBACK PixelWindowWndProc(
	HWND    hwnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam)
	{
	PIXELWINDOW*		 pixelWindow;
	LPCREATESTRUCT  lpcs;
	LRESULT						result;

  pixelWindow =
    (PIXELWINDOW *)(PIXELWINDOW::windowList->GetObject(hwnd));
	if (pixelWindow)
		{
		result = pixelWindow->WindowProc(hwnd, message, wParam, lParam);
		if (message == WM_DESTROY)
			{
			PIXELWINDOW::windowList->Remove(hwnd);
			delete pixelWindow;
			}
		}
	else
		switch (message)
			{
			case WM_CREATE:
				{
				lpcs = (LPCREATESTRUCT) lParam;
				pixelWindow = (PIXELWINDOW *)(lpcs->lpCreateParams);
				PIXELWINDOW::windowList->Add(hwnd, (LPSTR)pixelWindow);
				result = pixelWindow->WindowProc(hwnd, message, wParam, lParam);
				break;
				}
			default:
				result = DefWindowProc(hwnd, message, wParam, lParam);
			}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Show
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::Show(
	void
	){

	ShowWindow(hWndPixelWindow, SW_SHOW);
	UpdateWindow(hWndPixelWindow);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Draw
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::Draw(
	void)
	{
	PAINTSTRUCT	paintStruct;
	HPEN				hPen;
	HPEN				hOldPen;
	HDC					hDC;
	short int		i;

	hDC = BeginPaint(hWndPixelWindow, (LPPAINTSTRUCT)(&paintStruct));

	StretchDIBits(hDC, 0, 0, size*PIXELWIDTH, size*PIXELWIDTH, 0, 0,
	 size, size, (LPSTR)pImageArray, (LPBITMAPINFO)&imageDIBinfoHdr,
	 DIB_RGB_COLORS, SRCCOPY);

  hPen = GetStockPen(BLACK_PEN);
	if (hPen)
		{
		hOldPen = SelectPen(hDC, hPen);
		for (i=1; i<size; i++)
			 {
			 I_VERIFY(MoveToEx(hDC, PIXELWIDTH*i-1, 0, NULL));
			 LineTo(hDC, PIXELWIDTH*i-1, PIXELHEIGHT*size);
			 I_VERIFY(MoveToEx(hDC, 0, PIXELHEIGHT*i-1, NULL));
			 LineTo(hDC, PIXELWIDTH*size, PIXELHEIGHT*i-1);
			 }
		SelectPen(hDC, hOldPen);
		DeletePen(hPen);
		}

	DrawScreenPixels(hDC);

	DrawIconBackground();

	EndPaint(hWndPixelWindow, (LPPAINTSTRUCT)(&paintStruct));

//	UpdateIcon(TRUE);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawScreenPixels
//
// CLASS.......: PIXELWINDOW
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

void PIXELWINDOW::DrawScreenPixels(
	HDC	hDC)
	{
	short			x;
	short			y;
	short 		maskIndex;
	short 		imageIndex;
	BYTE			imageValue;
	HDC				hBitmapDC;
	HBITMAP		hOldBitmap;
	short			top;
	short			left;

	for (x=0; x<size; x++)
		for(y=0; y<size; y++)
			{
			maskIndex = (short)(((size-1-y)*maskScanLineBytes)+(x/MASKPIXELSPERBYTE));
			if (pMaskArray[maskIndex] & (0x80u>>(x%MASKPIXELSPERBYTE)))
				{
				imageIndex = (short)(((size-1-y)*imageScanLineBytes)+(x/pixelsPerByte));
				if (x%2)
					imageValue = (BYTE)(pImageArray[imageIndex] & 0x0F);
				else
					imageValue = (BYTE)(pImageArray[imageIndex]&0xF0);
				hBitmapDC = CreateCompatibleDC(hDC);
				if (imageValue == 0)
					hOldBitmap = SelectBitmap(hBitmapDC, hSmallS);
				else
					hOldBitmap = SelectBitmap(hBitmapDC, hSmallI);
				top = (short)(PIXELWIDTH * y);
				left = (short)(PIXELWIDTH * x);
				BitBlt(hDC, left, top, PIXELWIDTH-1, PIXELWIDTH-1, hBitmapDC,
				 0, 0, SRCCOPY);
				SelectBitmap(hBitmapDC, hOldBitmap);
				DeleteDC(hBitmapDC);
				}
			}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawIconBackground
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::DrawIconBackground(
	void)
	{
	HBRUSH 			hBrush;
	HPEN				hPen;
	HPEN				hOldPen;
	HBRUSH			hOldBrush;
	HDC					hDC;
	RGBQUAD			rgb;

	hDC = GetDC(hWndParent);

	rgb = imageColors[backgroundColor];
	hBrush = CreateSolidBrush(RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
	hPen = GetStockPen(BLACK_PEN);

	hOldPen = SelectPen(hDC, hPen);
	hOldBrush = SelectBrush(hDC, hBrush);

	Rectangle(hDC, ICONBACKLEFT, ICONBACKTOP,
						ICONBACKLEFT+ICONBACKWIDTH, ICONBACKTOP+ICONBACKWIDTH);

	SelectPen(hDC, hOldPen);
	SelectBrush(hDC, hOldBrush);

	DeleteBrush(hBrush);

	ReleaseDC(hWndParent, hDC);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetColorIndex
//
// CLASS.......: PIXELWINDOW
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
short PIXELWINDOW::GetColorIndex(
	COLORREF		rgb)
	{

	return (short)GetNearestPaletteIndex(hPalette, rgb);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCurrentColor
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::SetCurrentColor(
	short	colorIndex)
	{

	currentColor = colorIndex;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetBackgroundColor
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::SetBackgroundColor(
	short	colorIndex)
	{
	short int	colorMax;

	if (pixelsPerByte == COLORPIXELSPERBYTE)
		colorMax = 15;
	else
		colorMax = 1;

	if (colorIndex == DECREMENTBACKGROUND)
		backgroundColor = (short int)(backgroundColor==0 ? colorMax : backgroundColor-1);
	else if (colorIndex == INCREMENTBACKGROUND)
		backgroundColor = (short int)(backgroundColor==colorMax ? 0 : backgroundColor+1);
	else
		backgroundColor = colorIndex;

	UpdateIcon(TRUE);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Line
//
// CLASS.......: PIXELWINDOW
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

void PIXELWINDOW::Line(
	short int	x1,
	short int	y1,
	short int x2,
	short int y2)
	{
	short int dx;
	short int dy;
	short int x;
	short int y;
	double 		tmp;
	HDC				hDC;
	HBRUSH 		hBrush;
	HBRUSH		hOldBrush;
	HPEN			hPen;
	HPEN			hOldPen;
	HBITMAP		hOldBitmap;
	HDC				hBitmapDC;
	RGBQUAD		rgb;

	hDC = GetDC(hWndPixelWindow);

	if (currentColor == SCREENCOLOR)
		{
		hBitmapDC = CreateCompatibleDC(hDC);
		hOldBitmap = SelectBitmap(hBitmapDC, hSmallS);
		}
	else if (currentColor == INVERTEDCOLOR)
		{
		hBitmapDC = CreateCompatibleDC(hDC);
		hOldBitmap = SelectBitmap(hBitmapDC, hSmallI);
		}
	else
		{
		rgb = imageColors[currentColor];
		hBrush = CreateSolidBrush(RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
		hOldBrush = SelectBrush(hDC, hBrush);
		hPen = CreatePen(PS_SOLID, 1, RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
		hOldPen = SelectPen(hDC, hPen);
		}

	dx = (short int)(x2-x1);
	dy = (short int)(y2-y1);

	UpdatePixel(x1, y1, currentColor, hDC, hBitmapDC);

	if (abs(dx) > abs(dy))		//vary x
		{
		if (x2>=x1)
			for (x=(short int)(x1+1); x<x2; x++)
				{
				tmp =	(double)((x2-x)*dy)/(double)dx;
				if (floor(tmp)+0.5>tmp)
					y = (short int)(y2 - (short)floor(tmp));
				else
					y = (short int)(y2 - (short)ceil(tmp));
				UpdatePixel(x, y, currentColor, hDC, hBitmapDC);
				}
		else
			for (x=(short int)(x1-1); x>x2; x--)
				{
				tmp =	(double)((x2-x)*dy)/(double)dx;
				if (floor(tmp)+0.5>tmp)
					y = (short int)(y2 - (short)floor(tmp));
				else
					y = (short int)(y2 - (short)ceil(tmp));
				UpdatePixel(x, y, currentColor, hDC, hBitmapDC);
				}
		}
	else											//vary y
		{
		if (y2>=y1)
			for (y=(short int)(y1+1); y<y2; y++)
				{
				tmp =	(double)((y2-y)*dx)/(double)dy;
				if (floor(tmp)+0.5>tmp)
					x = (short int)(x2 - (short)floor(tmp));
				else
					x = (short int)(x2 - (short)ceil(tmp));
				UpdatePixel(x, y, currentColor, hDC, hBitmapDC);
				}
		else
			for (y=(short int)(y1-1); y>y2; y--)
				{
				tmp =	(double)((y2-y)*dx)/(double)dy;
				if (floor(tmp)+0.5>tmp)
					x = (short int)(x2 - (short)floor(tmp));
				else
					x = (short int)(x2 - (short)ceil(tmp));
				UpdatePixel(x, y, currentColor, hDC, hBitmapDC);
				}
		}

	if ((x1!=x2) || (y1!=y2))
		UpdatePixel(x2, y2, currentColor, hDC, hBitmapDC);

	if ((currentColor == SCREENCOLOR) || (currentColor == INVERTEDCOLOR))
		{
		SelectBitmap(hBitmapDC, hOldBitmap);
		DeleteDC(hBitmapDC);
		}
	else
		{
		SelectPen(hDC, hOldPen);
		SelectBrush(hDC, hOldBrush);
		DeleteBrush(hBrush);
		DeletePen(hPen);
		}

	ReleaseDC(hWndPixelWindow, hDC);

  UpdateIcon(TRUE);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DragLine
//
// CLASS.......: PIXELWINDOW
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

void PIXELWINDOW::DragLine(
	short int	x1,
	short int	y1,
	short int x2,
	short int y2)
	{
	short int x;
	short int y;
	short int dx;
	short int dy;
	double tmp;
	HDC				hDC;
	HBRUSH 		hBrush;
	HBRUSH		hOldBrush;
	HPEN			hPen;
	HPEN			hOldPen;
  // int       currentROP;
	RGBQUAD		rgb;

	hDC = GetDC(hWndPixelWindow);

	rgb = imageColors[currentColor];
	hBrush = CreateSolidBrush(RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
	hOldBrush = SelectBrush(hDC, hBrush);
	hPen = CreatePen(PS_SOLID, 1, RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
	hOldPen = SelectPen(hDC, hPen);

  // hBrush = GetStockObject(BLACK_BRUSH);
  // hOldBrush = SelectObject(hDC, hBrush);
  // hPen = GetStockObject(BLACK_PEN);
  // hOldPen = SelectObject(hDC, hPen);

  // currentROP = GetROP2(hDC);
  // SetROP2(hDC, R2_NOTXORPEN);

	Rectangle(hDC, PIXELWIDTH*x1, PIXELHEIGHT*y1, PIXELWIDTH*(x1+1)-1,
	 PIXELHEIGHT*(y1+1)-1);

	dx = (short int)(x2-x1);
	dy = (short int)(y2-y1);

	if (abs(dx) > abs(dy))		//vary x
		{
		if (x2>=x1)
			for (x=(short int)(x1+1); x<x2; x++)
				{
				tmp =	(double)((x2-x)*dy)/(double)dx;
				if (floor(tmp)+0.5>tmp)
					y = (short int)(y2 - (short)floor(tmp));
				else
					y = (short int)(y2 - (short)ceil(tmp));
				Rectangle(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH*(x+1)-1,
				 PIXELHEIGHT*(y+1)-1);
				}
		else
			for (x=(short int)(x1-1); x>x2; x--)
				{
				tmp =	(double)((x2-x)*dy)/(double)dx;
				if (floor(tmp)+0.5>tmp)
					y = (short int)(y2 - (short)floor(tmp));
				else
					y = (short int)(y2 - (short)ceil(tmp));
				Rectangle(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH*(x+1)-1,
				 PIXELHEIGHT*(y+1)-1);
				}
		}
	else											//vary y
		{
		if (y2>=y1)
			for (y=(short int)(y1+1); y<y2; y++)
				{
				tmp =	(double)((y2-y)*dx)/(double)dy;
				if (floor(tmp)+0.5>tmp)
					x = (short int)(x2 - (short)floor(tmp));
				else
					x = (short int)(x2 - (short)ceil(tmp));
				Rectangle(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH*(x+1)-1,
				 PIXELHEIGHT*(y+1)-1);
				}
		else
			for (y=(short int)(y1-1); y>y2; y--)
				{
				tmp =	(double)((y2-y)*dx)/(double)dy;
				if (floor(tmp)+0.5>tmp)
					x = (short int)(x2 - (short)floor(tmp));
				else
					x = (short int)(x2 - (short)ceil(tmp));
				Rectangle(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH*(x+1)-1,
				 PIXELHEIGHT*(y+1)-1);
				}
		}

	if ((x1!=x2) || (y1!=y2))
		Rectangle(hDC, PIXELWIDTH*x2, PIXELHEIGHT*y2, PIXELWIDTH*(x2+1)-1,
		 PIXELHEIGHT*(y2+1)-1);

  // SetROP2(hDC, currentROP);
	SelectPen(hDC, hOldPen);
	SelectBrush(hDC, hOldBrush);

	DeleteBrush(hBrush);
	DeletePen(hPen);

	ReleaseDC(hWndPixelWindow, hDC);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RestoreLine
//
// CLASS.......: PIXELWINDOW
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

void PIXELWINDOW::RestoreLine(
	short int	x1,
	short int	y1,
	short int x2,
	short int y2)
	{
	short int x;
	short int y;
	short int dx;
	short int dy;
	double tmp;
	HDC		hDC;

	hDC = GetDC(hWndPixelWindow);

	RestorePixel(x1,y1,hDC);

	dx = (short int)(x2-x1);
	dy = (short int)(y2-y1);

	if (abs(dx) > abs(dy))		//vary x
		{
		if (x2>=x1)
			for (x=(short int)(x1+1); x<x2; x++)
				{
				tmp =	(double)((x2-x)*dy)/(double)dx;
				if (floor(tmp)+0.5>tmp)
					y = (short int)(y2 - (short)floor(tmp));
				else
					y = (short int)(y2 - (short)ceil(tmp));
				RestorePixel(x, y, hDC);
				}
		else
			for (x=(short int)(x1-1); x>x2; x--)
				{
				tmp =	(double)((x2-x)*dy)/(double)dx;
				if (floor(tmp)+0.5>tmp)
					y = (short int)(y2 - (short)floor(tmp));
				else
					y = (short int)(y2 - (short)ceil(tmp));
				RestorePixel(x, y, hDC);
				}
		}
	else											//vary y
		{
		if (y2>=y1)
			for (y=(short int)(y1+1); y<y2; y++)
				{
				tmp =	(double)((y2-y)*dx)/(double)dy;
				if (floor(tmp)+0.5>tmp)
					x = (short int)(x2 - (short)floor(tmp));
				else
					x = (short int)(x2 - (short)ceil(tmp));
				RestorePixel(x, y, hDC);
				}
		else
			for (y=(short int)(y1-1); y>y2; y--)
				{
				tmp =	(double)((y2-y)*dx)/(double)dy;
				if (floor(tmp)+0.5>tmp)
					x = (short int)(x2 - (short)floor(tmp));
				else
					x = (short int)(x2 - (short)ceil(tmp));
				RestorePixel(x, y, hDC);
				}
		}

	if ((x1!=x2) || (y1!=y2))
		RestorePixel(x2, y2, hDC);


	ReleaseDC(hWndPixelWindow, hDC);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RestorePixel
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::RestorePixel(
	short			x,
	short			y,
	HDC				hDC)
	{
	short					 	imageIndex;
	short						maskIndex;
	HBRUSH 					hBrush;
	HBRUSH					hOldBrush;
	HPEN						hPen;
	HPEN						hOldPen;
	RGBQUAD					rgb;
	unsigned short 	imageColor;
	HDC							hBitmapDC;
	HBITMAP					hOldBitmap;

	imageIndex = (short)(((size-1-y)*imageScanLineBytes)+(x/pixelsPerByte));

	maskIndex = (short)(((size-1-y)*maskScanLineBytes)+(x/8)); //8 pixels per byte

	imageColor = (unsigned short)((pImageArray[imageIndex] & ~pixelMask[x%pixelsPerByte]) >>
		 ((pixelsPerByte-1-x%pixelsPerByte)*BITSPERBYTE/pixelsPerByte));

	if (pMaskArray[maskIndex] & (0x80u>>(x%MASKPIXELSPERBYTE)))
		{
		hBitmapDC = CreateCompatibleDC(hDC);
		if (imageColor == 0)		//screen color
			hOldBitmap = SelectBitmap(hBitmapDC, hSmallS);
		else										//inverse screen color
			hOldBitmap = SelectBitmap(hBitmapDC, hSmallI);
		BitBlt(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH-1, PIXELWIDTH-1,
		 hBitmapDC, 0, 0, SRCCOPY);
		SelectBitmap(hBitmapDC, hOldBitmap);
		DeleteDC(hBitmapDC);
		}
	else
		{
		rgb = imageColors[imageColor];
		hBrush = CreateSolidBrush(RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
		hOldBrush = SelectBrush(hDC, hBrush);
		hPen = CreatePen(PS_SOLID, 1, RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue));
		hOldPen = SelectPen(hDC, hPen);
		Rectangle(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH*(x+1)-1,
		 PIXELHEIGHT*(y+1)-1);

		SelectPen(hDC, hOldPen);
		SelectBrush(hDC, hOldBrush);

		DeleteBrush(hBrush);
		DeletePen(hPen);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdatePixel
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::UpdatePixel(
	short			x,
	short			y,
	short			currentColor,
	HDC				hDC,
	HDC				hBitmapDC)
	{
	short			imageIndex;
	short			maskIndex;

	imageIndex = (short)(((size-1-y)*imageScanLineBytes)+(x/pixelsPerByte));

	maskIndex = (short)(((size-1-y)*maskScanLineBytes)+(x/8)); //8 pixels per byte

	if ((currentColor == SCREENCOLOR) || (currentColor == INVERTEDCOLOR))
		{
		BitBlt(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH-1, PIXELWIDTH-1,
		 hBitmapDC, 0, 0, SRCCOPY);
		pMaskArray[maskIndex] |= (BYTE)(0x80u>>(x%8));
		if (currentColor == SCREENCOLOR)
			currentColor = 0X00;
		else
			{
			if (pixelsPerByte == COLORPIXELSPERBYTE)
				currentColor = 0X0F;
			else
				currentColor = 0X01;
			}
		}
	else
		{
		Rectangle(hDC, PIXELWIDTH*x, PIXELHEIGHT*y, PIXELWIDTH*(x+1)-1,
		 PIXELHEIGHT*(y+1)-1);
		pMaskArray[maskIndex] &= (BYTE)(0xFF7F>>(x%8));
		}

	pImageArray[imageIndex] =
	 (BYTE)((pImageArray[imageIndex] & pixelMask[x%pixelsPerByte]) |
	 (currentColor <<
	 ((pixelsPerByte-1-x%pixelsPerByte)*BITSPERBYTE/pixelsPerByte)));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateIcon
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::UpdateIcon(
	BOOL	drawBackgroundFlag)
	{
	HDC						hDC;
	HDC						hIconDC;
	HBITMAP				hIconBitmap;
	HBITMAP				hOldBitmap;

	if (drawBackgroundFlag)
		DrawIconBackground();

	hDC = GetDC(hWndParent);

	hIconDC = CreateCompatibleDC(hDC);

	hIconBitmap = CreateCompatibleBitmap(hDC, size, size);


	//bit blit mask

	SetDIBits(hDC, hIconBitmap, 0, size,	 (LPSTR)pMaskArray,
	 (LPBITMAPINFO)&maskDIBinfoHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, ICONLEFT, ICONTOP, size, size, hIconDC, 0, 0, SRCAND);

	SelectBitmap(hIconDC, hOldBitmap);


	//bit blit image

	SetDIBits(hDC, hIconBitmap, 0, size,	 (LPSTR)pImageArray,
	 (LPBITMAPINFO)&imageDIBinfoHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, ICONLEFT, ICONTOP, size, size, hIconDC, 0, 0, SRCINVERT);

	SelectBitmap(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteBitmap(hIconBitmap);

	ReleaseDC(hWndParent, hDC);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ReplaceIcon
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::ReplaceIcon(
	LPSERIUSICON		theIcon)
	{
	int							byteCount;
	int							i;

	if (pixelsPerByte  == COLORPIXELSPERBYTE)			//color icon
		{
		byteCount = imageScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			pImageArray[i] = theIcon->colorIcon.imageArray[i];
		byteCount = maskScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			pMaskArray[i] = theIcon->colorIcon.maskArray[i];
		for (i=0; i<16; i++)
			imageColors[i] = theIcon->colorIcon.imageColors[i];
		}
	else if (pixelsPerByte ==	BWPIXELSPERBYTE)
		{
		byteCount = imageScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			pImageArray[i] = theIcon->bwIcon.imageArray[i];
		byteCount = maskScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			pMaskArray[i] = theIcon->bwIcon.maskArray[i];
		for (i=0; i<2; i++)
			imageColors[i] = theIcon->bwIcon.imageColors[i];
		}

	CreateLogicalPalette();

	dirtyFlag = TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SaveIcon
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::SaveIcon(
	LPSERIUSICON		theIcon)
	{
	int							byteCount;
	int							i;

	if (pixelsPerByte  == COLORPIXELSPERBYTE)			//color icon
		{
		byteCount = imageScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			theIcon->colorIcon.imageArray[i] = pImageArray[i];
		byteCount = maskScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			theIcon->colorIcon.maskArray[i] = pMaskArray[i];
		for (i=0; i<16; i++)
			theIcon->colorIcon.imageColors[i] = imageColors[i];
		}
	else if (pixelsPerByte ==	BWPIXELSPERBYTE)
		{
		byteCount = imageScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			theIcon->bwIcon.imageArray[i] = pImageArray[i];
		byteCount = maskScanLineBytes*size;
		for (i=0; i<byteCount; i++)
			theIcon->bwIcon.maskArray[i] = pMaskArray[i];
		for (i=0; i<2; i++)
			theIcon->bwIcon.imageColors[i] = imageColors[i];
		}

	dirtyFlag = FALSE;

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetColor
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
COLORREF PIXELWINDOW::GetColor(
	short index)
	{
	RGBQUAD	 rgb;

	if ((index>=0) && (index<=15))
		{
		rgb = imageColors[index];
		return RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue);
		}
	else
		return RGB(0xFF, 0xFF, 0xFF);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SaveToFile
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::SaveToFile(
	LPSTR	fileName)
	{
	HANDLE		hFile;
	DWORD							nBytesWritten;

  hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL,
											CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE)
		{
		WriteFile(hFile, &imageDIBinfoHdr,
						sizeof(imageDIBinfoHdr)+16*sizeof(RGBQUAD), &nBytesWritten, NULL);
		WriteFile(hFile, pImageArray, 512, &nBytesWritten, NULL);
		WriteFile(hFile, pMaskArray, 128, &nBytesWritten, NULL);
		CloseHandle(hFile);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ClearIcon
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::ClearIcon(
	void)
	{
	long int	arraySize;
	int				i;

	arraySize = imageScanLineBytes*size;

	for (i=0; i<arraySize; i++)				//set every thing to white
		pImageArray[i] = 0xFF;

	arraySize = maskScanLineBytes*size;

	for (i=0; i<arraySize; i++)				//set every thing to white
		pMaskArray[i] = 0x00;

	dirtyFlag = TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Redraw
//
// CLASS.......: PIXELWINDOW
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
void PIXELWINDOW::Redraw(
	void)
	{
	InvalidateRect(hWndPixelWindow, NULL, FALSE);

	UpdateWindow(hWndPixelWindow);

	UpdateIcon(TRUE);
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: IsDirty
//
// CLASS.......: PIXELWINDOW
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
BOOL PIXELWINDOW::IsDirty(
	void)
	{

	return dirtyFlag;

	}
