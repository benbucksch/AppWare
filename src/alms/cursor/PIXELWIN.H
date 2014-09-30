#if !defined( __PIXELWIN_H )
#define __PIXELWIN_H

#include "winlist.h"
#include "sericon.h"

#define INCREMENTBACKGROUND -3
#define DECREMENTBACKGROUND	-4
#define SCROLLWIDTH     17

class PIXELWINDOW{

	private:

	static WINDOWLIST *	windowList;
	static HANDLE  			hAppInstance;

	HWND	   			 			hWndParent;


	int									size;
	WORD								pixelsPerByte;
	int									imageScanLineBytes;	//# bytes in zero padded scan line
	GLOBALHANDLE				hImageArray;
	BYTE *							pImageArray;
	int									maskScanLineBytes;
	GLOBALHANDLE				hMaskArray;
	BYTE *							pMaskArray;

	BITMAPINFOHEADER		imageDIBinfoHdr;		//these two fields must remain
	RGBQUAD							imageColors[16];    //adjacent and in this order

	BITMAPINFOHEADER		maskDIBinfoHdr;			//these two fields must remain
	RGBQUAD							maskColors[2];      //adjacent and in this order

	BYTE								pixelMask[8];

	HBITMAP							hBitmap;
	HBITMAP							hSmallS;
	HBITMAP							hSmallI;
	HPALETTE						hPalette;
	int       					currentColor;
	int       					backgroundColor;
	int      						startX;
	int       					startY;
	int      						prevX;
	int      						prevY;
	BOOL								doingDrag;
	BOOL								dirtyFlag;

	BOOL registerWindowClass(void);

	void DragLine(
		int	x1,
		int	y1,
		int x2,
		int y2);

	void RestoreLine(
		int	x1,
		int	y1,
		int x2,
		int y2);

	void Line(
		int	x1,
		int	y1,
		int x2,
		int y2);

	void UpdatePixel(
		int	x,
		int	y,
		int currentColor,
		HDC	hDC,
		HDC	hBitmapDC);

	void RestorePixel(
		int	x,
		int	y,
		HDC	hDC);

	void Draw(void);

	void DrawScreenPixels(
		HDC	hDC);

	void DrawIconBackground(void);

	void CreateLogicalPalette(void);

	void InitPixelMask(void);


public:
	HWND  		       hWndPixelWindow;

	PIXELWINDOW(
		HWND	 	 	hWndMain,
		HANDLE 		hAppInstance,
		HANDLE 		hPrevInstance,
		int				size,
		WORD			pixelsPerByte);

	~PIXELWINDOW(void);

	long WindowProc(
		HWND 			hWnd,
		unsigned 	message,
		WPARAM 		wParam,
		LPARAM		lParam);

	void Show(void);

	void UpdateIcon(
		BOOL	drawIconBackground);

	UINT GetColorIndex(
		COLORREF	rgb);

	COLORREF GetColor(
		int index);

	void SetCurrentColor(
		UINT	colorIndex);

	void SetBackgroundColor(
		UINT	colorIndex);

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


	friend long CALLBACK PixelWindowWndProc(
		HWND 			hWnd,
		UINT		 	message,
		WPARAM		wParam,
		LPARAM		lParam);

	};

#endif
