#define		STRICT
#define		ASTRICT

#include "windows.h"
#include "sericon.h"
#include "mem.h"
#include "io.h"

#define COLOR_BLACK	0

#define COLORPIXELSPERBYTE	2
#define BWPIXELSPERBYTE			8

#define COLORSCANLINEBYTES	16
#define BWSCANLINEBYTES			4

#define ICONSIZE			32		//32 by 32 pixels

extern SERIUSICON seriusIcon;

struct ICOFILEHDR
	{
	WORD	icoReserved;
	WORD	icoResourceType;
	WORD	icoResourceCount;
	};

struct ICODIRENTRY
	{
	BYTE		width;
	BYTE		height;
	BYTE		colorCount;
	BYTE		reserved1;
	WORD		reserved2;
	WORD		reserved3;
	DWORD		icoDIBSize;
	DWORD		icoDIBOffset;
	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	SERIUSICON
//
// CLASS.......: 	SERIUSICON
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
SERIUSICON::SERIUSICON(
	void)
	{
	int	i;

	for (i=0; i<IMAGEARRAYSIZE; i++)				//set every thing to white
		colorIcon.imageArray[i] = 0xFF;

	colorIcon.imageHdr.biSize = (long)sizeof(BITMAPINFOHEADER);
	colorIcon.imageHdr.biWidth = 32L;
	colorIcon.imageHdr.biHeight = 32L;
	colorIcon.imageHdr.biPlanes = 1;
	colorIcon.imageHdr.biBitCount = 4;
	colorIcon.imageHdr.biCompression = 0L;
	colorIcon.imageHdr.biSizeImage = 0L;
	colorIcon.imageHdr.biXPelsPerMeter = 0L;
	colorIcon.imageHdr.biYPelsPerMeter = 0L;
	colorIcon.imageHdr.biClrUsed = 0L;
	colorIcon.imageHdr.biClrImportant = 0L;

	colorIcon.imageColors[0].rgbRed = 0x00;
	colorIcon.imageColors[0].rgbGreen = 0x00;
	colorIcon.imageColors[0].rgbBlue = 0x00;
	colorIcon.imageColors[0].rgbReserved = 0;

	colorIcon.imageColors[1].rgbRed = 0x00;
	colorIcon.imageColors[1].rgbGreen = 0x00;
	colorIcon.imageColors[1].rgbBlue = 0xFF;
	colorIcon.imageColors[1].rgbReserved = 0;

	colorIcon.imageColors[2].rgbRed = 0x00;
	colorIcon.imageColors[2].rgbGreen = 0xFF;
	colorIcon.imageColors[2].rgbBlue = 0x00;
	colorIcon.imageColors[2].rgbReserved = 0;

	colorIcon.imageColors[3].rgbRed = 0xFF;
	colorIcon.imageColors[3].rgbGreen = 0x00;
	colorIcon.imageColors[3].rgbBlue = 0x00;
	colorIcon.imageColors[3].rgbReserved = 0;

	colorIcon.imageColors[4].rgbRed = 0x00;
	colorIcon.imageColors[4].rgbGreen = 0x00;
	colorIcon.imageColors[4].rgbBlue = 0x80;
	colorIcon.imageColors[4].rgbReserved = 0;

	colorIcon.imageColors[5].rgbRed = 0x00;
	colorIcon.imageColors[5].rgbGreen = 0x80;
	colorIcon.imageColors[5].rgbBlue = 0x00;
	colorIcon.imageColors[5].rgbReserved = 0;

	colorIcon.imageColors[6].rgbRed = 0x80;
	colorIcon.imageColors[6].rgbGreen = 0x00;
	colorIcon.imageColors[6].rgbBlue = 0x00;
	colorIcon.imageColors[6].rgbReserved = 0;

	colorIcon.imageColors[7].rgbRed = 0x80;
	colorIcon.imageColors[7].rgbGreen = 0x80;
	colorIcon.imageColors[7].rgbBlue = 0x80;
	colorIcon.imageColors[7].rgbReserved = 0;

	colorIcon.imageColors[8].rgbRed = 0xC0;
	colorIcon.imageColors[8].rgbGreen = 0xC0;
	colorIcon.imageColors[8].rgbBlue = 0xC0;
	colorIcon.imageColors[8].rgbReserved = 0;

	colorIcon.imageColors[9].rgbRed = 0x80;
	colorIcon.imageColors[9].rgbGreen = 0x80;
	colorIcon.imageColors[9].rgbBlue = 0x00;
	colorIcon.imageColors[9].rgbReserved = 0;

	colorIcon.imageColors[10].rgbRed = 0x00;
	colorIcon.imageColors[10].rgbGreen = 0x80;
	colorIcon.imageColors[10].rgbBlue = 0x80;
	colorIcon.imageColors[10].rgbReserved = 0;

	colorIcon.imageColors[11].rgbRed = 0x80;
	colorIcon.imageColors[11].rgbGreen = 0x00;
	colorIcon.imageColors[11].rgbBlue = 0x80;
	colorIcon.imageColors[11].rgbReserved = 0;

	colorIcon.imageColors[12].rgbRed = 0xFF;
	colorIcon.imageColors[12].rgbGreen = 0xFF;
	colorIcon.imageColors[12].rgbBlue = 0x00;
	colorIcon.imageColors[12].rgbReserved = 0;

	colorIcon.imageColors[13].rgbRed = 0x00;
	colorIcon.imageColors[13].rgbGreen = 0xFF;
	colorIcon.imageColors[13].rgbBlue = 0xFF;
	colorIcon.imageColors[13].rgbReserved = 0;

	colorIcon.imageColors[14].rgbRed = 0xFF;
	colorIcon.imageColors[14].rgbGreen = 0x00;
	colorIcon.imageColors[14].rgbBlue = 0xFF;
	colorIcon.imageColors[14].rgbReserved = 0;

	colorIcon.imageColors[15].rgbRed = 0xFF;
	colorIcon.imageColors[15].rgbGreen = 0xFF;
	colorIcon.imageColors[15].rgbBlue = 0xFF;
	colorIcon.imageColors[15].rgbReserved = 0;

	for (i=0; i<MASKARRAYSIZE; i++)				//set every thing to white
		colorIcon.maskArray[i] = 0x00;

	colorIcon.maskHdr.biSize = (long)sizeof(BITMAPINFOHEADER);
	colorIcon.maskHdr.biWidth = 32L;
	colorIcon.maskHdr.biHeight = 32L;
	colorIcon.maskHdr.biPlanes = 1;
	colorIcon.maskHdr.biBitCount = 1;
	colorIcon.maskHdr.biCompression = 0L;
	colorIcon.maskHdr.biSizeImage = 0L;
	colorIcon.maskHdr.biXPelsPerMeter = 0L;
	colorIcon.maskHdr.biYPelsPerMeter = 0L;
	colorIcon.maskHdr.biClrUsed = 0L;
	colorIcon.maskHdr.biClrImportant = 0L;

	colorIcon.maskColors[0].rgbRed = 0;
	colorIcon.maskColors[0].rgbGreen = 0;
	colorIcon.maskColors[0].rgbBlue = 0;
	colorIcon.maskColors[0].rgbReserved = 0;
	colorIcon.maskColors[1].rgbRed = 0xFF;
	colorIcon.maskColors[1].rgbGreen = 0xFF;
	colorIcon.maskColors[1].rgbBlue = 0xFF;
	colorIcon.maskColors[1].rgbReserved = 0;

	for (i=0; i<MASKARRAYSIZE; i++)				//set every thing to white
		bwIcon.imageArray[i] = 0xFF;

	bwIcon.imageHdr.biSize = (long)sizeof(BITMAPINFOHEADER);
	bwIcon.imageHdr.biWidth = 32L;
	bwIcon.imageHdr.biHeight = 32L;
	bwIcon.imageHdr.biPlanes = 1;
	bwIcon.imageHdr.biBitCount = 1;
	bwIcon.imageHdr.biCompression = 0L;
	bwIcon.imageHdr.biSizeImage = 0L;
	bwIcon.imageHdr.biXPelsPerMeter = 0L;
	bwIcon.imageHdr.biYPelsPerMeter = 0L;
	bwIcon.imageHdr.biClrUsed = 0L;
	bwIcon.imageHdr.biClrImportant = 0L;

	bwIcon.imageColors[0].rgbRed = 0x00;
	bwIcon.imageColors[0].rgbGreen = 0x00;
	bwIcon.imageColors[0].rgbBlue = 0x00;
	bwIcon.imageColors[0].rgbReserved = 0;

	bwIcon.imageColors[1].rgbRed = 0xFF;
	bwIcon.imageColors[1].rgbGreen = 0xFF;
	bwIcon.imageColors[1].rgbBlue = 0xFF;
	bwIcon.imageColors[1].rgbReserved = 0;

	for (i=0; i<MASKARRAYSIZE; i++)				//set every thing to white
		bwIcon.maskArray[i] = 0x00;

	bwIcon.maskHdr.biSize = (long)sizeof(BITMAPINFOHEADER);
	bwIcon.maskHdr.biWidth = 32L;
	bwIcon.maskHdr.biHeight = 32L;
	bwIcon.maskHdr.biPlanes = 1;
	bwIcon.maskHdr.biBitCount = 1;
	bwIcon.maskHdr.biCompression = 0L;
	bwIcon.maskHdr.biSizeImage = 0L;
	bwIcon.maskHdr.biXPelsPerMeter = 0L;
	bwIcon.maskHdr.biYPelsPerMeter = 0L;
	bwIcon.maskHdr.biClrUsed = 0L;
	bwIcon.maskHdr.biClrImportant = 0L;

	bwIcon.maskColors[0].rgbRed = 0;
	bwIcon.maskColors[0].rgbGreen = 0;
	bwIcon.maskColors[0].rgbBlue = 0;
	bwIcon.maskColors[0].rgbReserved = 0;
	bwIcon.maskColors[1].rgbRed = 0xFF;
	bwIcon.maskColors[1].rgbGreen = 0xFF;
	bwIcon.maskColors[1].rgbBlue = 0xFF;
	bwIcon.maskColors[1].rgbReserved = 0;

	for (i=0; i<32; i++)
		{
		InitPixels(i,0);
		InitPixels(i,31);
		InitPixels(0,i);
		InitPixels(31,i);
		}




	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSICON
//
// CLASS.......: SERIUSICON
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
SERIUSICON::SERIUSICON(
	int		fileHandle,
	short	colorCount)
	{
	if (colorCount == 16)
		{
		_lread(fileHandle, (LPSTR)&colorIcon, sizeof(BITMAPINFOHEADER)+
		 16*sizeof(RGBQUAD)+IMAGEARRAYSIZE);

		_lread(fileHandle, (LPSTR)&colorIcon.maskArray, MASKARRAYSIZE);

		colorIcon.maskHdr.biSize = (long)sizeof(BITMAPINFOHEADER);
		colorIcon.maskHdr.biWidth = 32L;
		colorIcon.maskHdr.biHeight = 32L;
		colorIcon.maskHdr.biPlanes = 1;
		colorIcon.maskHdr.biBitCount = 4;
		colorIcon.maskHdr.biCompression = 0L;
		colorIcon.maskHdr.biSizeImage = 0L;
		colorIcon.maskHdr.biXPelsPerMeter = 0L;
		colorIcon.maskHdr.biYPelsPerMeter = 0L;
		colorIcon.maskHdr.biClrUsed = 0L;
		colorIcon.maskHdr.biClrImportant = 0L;

		colorIcon.maskColors[0].rgbRed = 0;
		colorIcon.maskColors[0].rgbGreen = 0;
		colorIcon.maskColors[0].rgbBlue = 0;
		colorIcon.maskColors[0].rgbReserved = 0;
		colorIcon.maskColors[1].rgbRed = 0xFF;
		colorIcon.maskColors[1].rgbGreen = 0xFF;
		colorIcon.maskColors[1].rgbBlue = 0xFF;
		colorIcon.maskColors[1].rgbReserved = 0;
		}
	else if (colorCount == 2)
		{
		int dirSize;
		int i;
		CURSORENTRY cursorEntry;

		_lread(fileHandle, (LPSTR)&(bwCursor.cursorHdr), sizeof(WORD) * 3);

		dirSize = bwCursor.cursorHdr.curResourceCount;
		hrsrcDir = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, sizeof(CURSORENTRY) * dirSize);
		bwCursor.cursorHdr.CursorEntry = (LPRESOURCEDIR)GlobalLock(hrsrcDir);


	_lread(fileHandle, (LPSTR) &cursorEntry, sizeof(CURSORENTRY) * dirSize);

	_fmemcpy((LPSTR)bwCursor.cursorHdr.CursorEntry, (LPSTR)&cursorEntry, sizeof(CURSORENTRY));

	_lread(fileHandle, (LPSTR) &bwCursor.cursorImage, sizeof(HOTSPOT));



	_lread(fileHandle, (LPSTR)&bwCursor.cursorImage.dibInfo, sizeof(BITMAPINFOHEADER));

	_lread(fileHandle, (LPSTR)&bwCursor.cursorImage.imageColors,  2*sizeof(RGBQUAD));


	_lread(fileHandle, (LPSTR)bwCursor.cursorImage.bXorMask, MASKARRAYSIZE);
	_lread(fileHandle, (LPSTR)bwCursor.cursorImage.bAndMask, MASKARRAYSIZE);


	bwCursor.cursorImage.imageColors[0].rgbRed = 0x00;
	bwCursor.cursorImage.imageColors[0].rgbGreen = 0x00;
	bwCursor.cursorImage.imageColors[0].rgbBlue = 0x00;
	bwCursor.cursorImage.imageColors[0].rgbReserved = 0;

	bwCursor.cursorImage.imageColors[1].rgbRed = 0xFF;
	bwCursor.cursorImage.imageColors[1].rgbGreen = 0xFF;
	bwCursor.cursorImage.imageColors[1].rgbBlue = 0xFF;
	bwCursor.cursorImage.imageColors[1].rgbReserved = 0;

	dirSize = MASKARRAYSIZE;
	for (i=0; i<dirSize; i++)				//set every thing to white
		bwIcon.imageArray[i] = bwCursor.cursorImage.bAndMask[i];

	memcpy(&bwIcon.imageHdr, &bwCursor.cursorImage.dibInfo, sizeof(BITMAPINFOHEADER));
	memcpy(&bwIcon.imageColors, &bwCursor.cursorImage.imageColors, sizeof(RGBQUAD));


	for (i=0; i<dirSize; i++)				//set every thing to white
		bwIcon.maskArray[i] = bwCursor.cursorImage.bXorMask[i];
	memcpy(&bwIcon.maskHdr, &bwCursor.cursorImage.dibInfo, sizeof(BITMAPINFOHEADER));
	memcpy(&bwIcon.maskColors, &bwCursor.cursorImage.imageColors, sizeof(RGBQUAD));
	}

}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitPixels
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
void SERIUSICON::InitPixels(
	short			x,
	short			y)
	{
	short			imageIndex;
	short			maskIndex;

	imageIndex = ((ICONSIZE-1-y)*COLORSCANLINEBYTES)+(x/COLORPIXELSPERBYTE);

	maskIndex = ((ICONSIZE-1-y)*BWSCANLINEBYTES)+(x/BWPIXELSPERBYTE);

	colorIcon.maskArray[maskIndex] &= (0xFF7F>>(x%BWPIXELSPERBYTE));

	bwIcon.imageArray[maskIndex] &= (0xFF7F>>(x%BWPIXELSPERBYTE));
	bwIcon.maskArray[maskIndex] &= (0xFF7F>>(x%BWPIXELSPERBYTE));

       //	bwCursor.imageArray[maskIndex] &= (0xFF7F>>(x%BWPIXELSPERBYTE));
       //	bwCursor.maskArray[maskIndex] &= (0xFF7F>>(x%BWPIXELSPERBYTE));

	if (x%2)
		colorIcon.imageArray[imageIndex] =
		 (colorIcon.imageArray[imageIndex]&0xF0)|COLOR_BLACK;
	else
		colorIcon.imageArray[imageIndex] =
		 (colorIcon.imageArray[imageIndex]&0x0F)|(COLOR_BLACK<<4);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawColor
//
// CLASS.......: SERIUSICON
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
void SERIUSICON::DrawColor(
	HDC	hDC,
	int	x,
	int y)
	{
	HDC						hIconDC;
	HBITMAP				hIconBitmap;
	HBITMAP				hOldBitmap;

	hIconDC = CreateCompatibleDC(hDC);

	hIconBitmap = CreateCompatibleBitmap(hDC, ICONSIZE, ICONSIZE);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)colorIcon.imageArray,
	 (LPBITMAPINFO)&colorIcon.imageHdr, DIB_RGB_COLORS);

	hOldBitmap = (HBITMAP)SelectObject(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCAND);

	SelectObject(hIconDC, hOldBitmap);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)colorIcon.maskArray,
	 (LPBITMAPINFO)&colorIcon.maskHdr, DIB_RGB_COLORS);

	hOldBitmap = (HBITMAP)SelectObject(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCINVERT);

	SelectObject(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteObject(hIconBitmap);

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawBW
//
// CLASS.......: SERIUSICON
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
void SERIUSICON::DrawBW(
	HDC	hDC,
	int	x,
	int y)
	{
	HDC						hIconDC;
	HBITMAP				hIconBitmap;
	HBITMAP				hOldBitmap;

	hIconDC = CreateCompatibleDC(hDC);

	hIconBitmap = CreateCompatibleBitmap(hDC, ICONSIZE, ICONSIZE);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)bwIcon.imageArray,
	 (LPBITMAPINFO)&bwIcon.imageHdr, DIB_RGB_COLORS);

	hOldBitmap = (HBITMAP)SelectObject(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCAND);

	SelectObject(hIconDC, hOldBitmap);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)bwIcon.maskArray,
	 (LPBITMAPINFO)&bwIcon.maskHdr, DIB_RGB_COLORS);

	hOldBitmap = (HBITMAP)SelectObject(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCINVERT);

	SelectObject(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteObject(hIconBitmap);

	}

