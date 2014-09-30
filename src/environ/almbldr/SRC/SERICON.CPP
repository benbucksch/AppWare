#include "almsys.h"
#include <windowsx.h>

#include "sericon.h"

#define COLOR_BLACK	0

#define COLORPIXELSPERBYTE	2
#define BWPIXELSPERBYTE			8

#define COLORSCANLINEBYTES	16
#define BWSCANLINEBYTES			4

#define ICONSIZE			32		//32 by 32 pixels

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
		InitPixels((short)i,0);
		InitPixels((short)i,31);
		InitPixels(0,(short)i);
		InitPixels(31,(short)i);
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
	HANDLE		fileHandle,
	short			colorCount)
	{
	DWORD			nBytesRead;

	if (colorCount == 16)
		{
		ReadFile(fileHandle, &colorIcon.imageHdr, sizeof(BITMAPINFOHEADER)+
		 16*sizeof(RGBQUAD)+IMAGEARRAYSIZE, &nBytesRead, NULL);

		ReadFile(fileHandle, (LPSTR)&colorIcon.maskArray, MASKARRAYSIZE, &nBytesRead, NULL);

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
		ReadFile(fileHandle, (LPSTR)&bwIcon.imageHdr, sizeof(BITMAPINFOHEADER)+
		 2*sizeof(RGBQUAD)+MASKARRAYSIZE, &nBytesRead, NULL);

		ReadFile(fileHandle, (LPSTR)&bwIcon.maskArray, MASKARRAYSIZE, &nBytesRead, NULL);

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

	imageIndex = (short)(((ICONSIZE-1-y)*COLORSCANLINEBYTES)+(x/COLORPIXELSPERBYTE));

	maskIndex = (short)(((ICONSIZE-1-y)*BWSCANLINEBYTES)+(x/BWPIXELSPERBYTE));

	colorIcon.maskArray[maskIndex] &= (short)(0xFF7F>>(x%BWPIXELSPERBYTE));

	bwIcon.imageArray[maskIndex] &= (short)(0xFF7F>>(x%BWPIXELSPERBYTE));
	bwIcon.maskArray[maskIndex] &= (short)(0xFF7F>>(x%BWPIXELSPERBYTE));

	if (x%2)
		colorIcon.imageArray[imageIndex] =
		 (short)((colorIcon.imageArray[imageIndex]&0xF0)|COLOR_BLACK);
	else
		colorIcon.imageArray[imageIndex] =
		 (short)((colorIcon.imageArray[imageIndex]&0x0F)|(COLOR_BLACK<<4));

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

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)colorIcon.maskArray,
	 (LPBITMAPINFO)&colorIcon.maskHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCAND);

	SelectBitmap(hIconDC, hOldBitmap);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)colorIcon.imageArray,
	 (LPBITMAPINFO)&colorIcon.imageHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCINVERT);

	SelectBitmap(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteBitmap(hIconBitmap);

	}

//copies the color mask onto hDC
void SERIUSICON::DrawColorMask(
	HDC	hDC,
	int	x,
	int y)
	{
	HDC						hIconDC;
	HBITMAP				hIconBitmap;
	HBITMAP				hOldBitmap;

	hIconDC = CreateCompatibleDC(hDC);

	hIconBitmap = CreateCompatibleBitmap(hDC, ICONSIZE, ICONSIZE);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)colorIcon.maskArray,
	 (LPBITMAPINFO)&colorIcon.maskHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCCOPY);

	SelectBitmap(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteBitmap(hIconBitmap);

	}

//copies the color image bitmap onto hDC
void SERIUSICON::DrawColorImage(
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

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCCOPY);

	SelectBitmap(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteBitmap(hIconBitmap);

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

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)bwIcon.maskArray,
	 (LPBITMAPINFO)&bwIcon.maskHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCAND);

	SelectBitmap(hIconDC, hOldBitmap);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)bwIcon.imageArray,
	 (LPBITMAPINFO)&bwIcon.imageHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCINVERT);

	SelectBitmap(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteBitmap(hIconBitmap);

	}

//copies the BW mask onto hDC
void SERIUSICON::DrawBWMask(
	HDC	hDC,
	int	x,
	int y)
	{
	HDC						hIconDC;
	HBITMAP				hIconBitmap;
	HBITMAP				hOldBitmap;

	hIconDC = CreateCompatibleDC(hDC);

	hIconBitmap = CreateCompatibleBitmap(hDC, ICONSIZE, ICONSIZE);

	SetDIBits(hDC, hIconBitmap, 0, ICONSIZE,	 (LPSTR)bwIcon.maskArray,
	 (LPBITMAPINFO)&bwIcon.maskHdr, DIB_RGB_COLORS);

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCCOPY);

	SelectBitmap(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteBitmap(hIconBitmap);

	}

//copies the BW image bitmap onto hDC
void SERIUSICON::DrawBWImage(
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

	hOldBitmap = SelectBitmap(hIconDC, hIconBitmap);

	BitBlt(hDC, x, y, ICONSIZE, ICONSIZE, hIconDC, 0, 0, SRCCOPY);

	SelectBitmap(hIconDC, hOldBitmap);

	DeleteDC(hIconDC);

	DeleteBitmap(hIconBitmap);

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Write
//
// CLASS.......: SERIUSICON
//
// DESCRIPTION.:  Writes the serius icon to a file formated as a Windows
//                .ico file.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSICON::Write(
	HANDLE		hFile)
	{
	ICOFILEHDR   fileHdr;
	ICODIRENTRY  dirEntry;
	DWORD				nBytesRead;
	DWORD				nBytesWritten;

	//initialize and write file header

	fileHdr.icoReserved = 0;
	fileHdr.icoResourceType = 1;  //value for an icon resource type
	fileHdr.icoResourceCount = 2;

	WriteFile(hFile, &fileHdr, sizeof(fileHdr), &nBytesWritten, NULL);

	//write dir entry for color icon first

	dirEntry.width = 32;
	dirEntry.height = 32;
	dirEntry.colorCount = 16;
	dirEntry.reserved1 = 0;
	dirEntry.reserved2 = 0;
	dirEntry.reserved3 = 0;
	dirEntry.icoDIBSize =
	 sizeof(BITMAPINFOHEADER) + 16*sizeof(RGBQUAD) + IMAGEARRAYSIZE +
	 MASKARRAYSIZE;  //744;
	dirEntry.icoDIBOffset = sizeof(fileHdr) + 2*sizeof(dirEntry);

	WriteFile(hFile, &dirEntry, sizeof(dirEntry), &nBytesWritten, NULL);

	//write dir entry for b&w icon second

	dirEntry.width = 32;
	dirEntry.height = 32;
	dirEntry.colorCount = 2;
	dirEntry.reserved1 = 0;
	dirEntry.reserved2 = 0;
	dirEntry.reserved3 = 0;
	dirEntry.icoDIBOffset += dirEntry.icoDIBSize;
	dirEntry.icoDIBSize =
	 sizeof(BITMAPINFOHEADER) + 2*sizeof(RGBQUAD) + 2*MASKARRAYSIZE;  //744;

	ReadFile(hFile, &dirEntry, sizeof(dirEntry), &nBytesRead, NULL);

	//write the color icon

	colorIcon.imageHdr.biHeight = 64;  //64 to account for the mask

	WriteFile(hFile, (char *)&colorIcon.imageHdr, sizeof(colorIcon.imageHdr), &nBytesWritten, NULL);
	WriteFile(hFile, (char *)&colorIcon.imageColors, sizeof(colorIcon.imageColors), &nBytesWritten, NULL);
	WriteFile(hFile, (char *)&colorIcon.imageArray, sizeof(colorIcon.imageArray), &nBytesWritten, NULL);
	WriteFile(hFile, (char *)&colorIcon.maskArray, sizeof(colorIcon.maskArray), &nBytesWritten, NULL);

	colorIcon.imageHdr.biHeight = 32;

	//write the b&w icon

	bwIcon.imageHdr.biHeight = 64;  //64 to account for the mask

	WriteFile(hFile, (char *)&bwIcon.imageHdr, sizeof(bwIcon.imageHdr), &nBytesWritten, NULL);
	WriteFile(hFile, (char *)&bwIcon.imageColors, sizeof(bwIcon.imageColors), &nBytesWritten, NULL);
	WriteFile(hFile, (char *)&bwIcon.imageArray, sizeof(bwIcon.imageArray), &nBytesWritten, NULL);
	WriteFile(hFile, (char *)&bwIcon.maskArray, sizeof(bwIcon.maskArray), &nBytesWritten, NULL);

	bwIcon.imageHdr.biHeight = 32;

	return TRUE;

	}

