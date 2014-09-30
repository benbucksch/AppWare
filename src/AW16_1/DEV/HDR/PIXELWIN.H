#if !defined( __PIXELWIN_H )
#define __PIXELWIN_H

#include "winlist.h"
#include "classdef.h"

#define INCREMENTBACKGROUND -3
#define DECREMENTBACKGROUND	-4

class PIXELWINDOW:public MEMORY{

  friend long I_CALLBACK PixelWindowWndProc(
	              HWND    hwnd,
	              UINT    message,
	              WPARAM  wParam,
	              LPARAM  lParam);

	private:

	static WINDOWLIST*			windowList;
	static HINSTANCE				hAppInstance;

	HWND	   			 					hWndParent;
	HWND					 					hWndPixelWindow;

	short int								size;
	short int								pixelsPerByte;
	short int								imageScanLineBytes;	//# bytes in zero padded scan line
	GLOBALHANDLE						hImageArray;
	BYTE FAR*								pImageArray;
	short int								maskScanLineBytes;
	GLOBALHANDLE						hMaskArray;
	BYTE FAR *							pMaskArray;

	BITMAPINFOHEADER				imageDIBinfoHdr;		//these two fields must remain
	RGBQUAD									imageColors[16];    //adjacent and in this order

	BITMAPINFOHEADER				maskDIBinfoHdr;			//these two fields must remain
	RGBQUAD									maskColors[2];      //adjacent and in this order

	BYTE										pixelMask[8];

	HBITMAP									hBitmap;
	HBITMAP									hSmallS;
	HBITMAP									hSmallI;
	HPALETTE								hPalette;
	short int								currentColor;
	short int								backgroundColor;
	short int								startX;
	short int 							startY;
	short int								prevX;
	short int								prevY;
	BOOL										doingDrag;
	BOOL										dirtyFlag;

	BOOL registerWindowClass(void);

	void DragLine(
		short int	x1,
		short int	y1,
		short int x2,
		short int y2);

	void RestoreLine(
		short int	x1,
		short int	y1,
		short int x2,
		short int y2);

	void Line(
		short int	x1,
		short int	y1,
		short int x2,
		short int y2);

	void UpdatePixel(
		short			x,
		short			y,
		short			currentColor,
		HDC				hDC,
		HDC				hBitmapDC);

	void RestorePixel(
		short			x,
		short			y,
		HDC				hDC);

	void Draw(void);

	void DrawScreenPixels(
		HDC	hDC);

	void DrawIconBackground(void);

	void CreateLogicalPalette(void);

	void InitPixelMask(void);


public:

	PIXELWINDOW(
		HWND	  	hWndMain,
		HINSTANCE	hAppInstance,
		HINSTANCE	hPrevInstance,
		short int	size,
		short int	pixelsPerByte);

	~PIXELWINDOW(void);

	LRESULT WindowProc(
		HWND 			hWnd,
		UINT		 	message,
		WPARAM		wParam,
		LPARAM		lParam);

	void Show(void);

	void UpdateIcon(
		BOOL	drawIconBackground);

	short GetColorIndex(
		COLORREF	rgb);

	COLORREF GetColor(
		short index);

	void SetCurrentColor(
		short	colorIndex);

	void SetBackgroundColor(
		short	colorIndex);

	void ReplaceIcon(
		LPSERIUSICON	iconInfo);

	void SaveIcon(
		LPSERIUSICON	iconInfo);

	void SaveToFile(
		LPSTR	fileName);

	void ClearIcon(
		void);

	void Redraw(
		void);

	BOOL IsDirty(
		void);


	friend long I_CALLBACK PixelWindowWndProc(
		HWND 			hWnd,
		unsigned 	message,
		WORD 			wParam,
		LONG 			lParam);

	};

#endif
