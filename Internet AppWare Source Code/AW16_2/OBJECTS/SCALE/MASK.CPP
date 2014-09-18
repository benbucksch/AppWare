
/////////////////////////////////////////////////////////////////////////////
// File Name:  mask.CPP
// 		Copyright 1989-92 Serius Corporation
// Date: 7/25/92
//
// Description.:
//	Takes a handle to a bitmap of any size and resolution and creates a
//	mask bitmap.  White remains white, all other colors are translated
//	to black.
/////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <windows.h>
#include <a_alm.h>
#include "scale.h"
#define WHITE  1  // transparent part of the mask
#define BLACK  0  // opaque part of the mask

HBITMAP CreateMaskBitmap(HANDLE hDIB)
	{
	HDC                hdc ;
	LPBITMAPINFOHEADER lpInfo ;
	LPBITMAPINFO 			 lpbmiInd, lpbmiMask ;
	LPSTR              lpBitsInd,lpBits, lpBitsMask ;
	HBITMAP            hbmpMask ;
	int bit, byte ;
	WORD i, j ;
	long colors, colorIndex, whiteIndex ;

	lpInfo = (LPBITMAPINFOHEADER) GlobalLock(hDIB);
	if (lpInfo)
		{
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL) ;
		WORD offBits = (WORD)lpInfo->biSize + PaletteSize((LPSTR)lpInfo);
		lpbmiInd = (LPBITMAPINFO)lpInfo ;
		lpBitsInd = (LPSTR)lpInfo+offBits  ;

		colors= 1 ;
		colors = colors << (lpInfo->biBitCount) ; // determine the number of colors used by the indicator bitmap
		whiteIndex = -1 ;
		if (colors > 256)
			{
			colors = 0 ; // if the bitmap supports 24-bit color, the bmiColors field is NULL
			whiteIndex = 0x00FFFFFFL ;
			}

		for (i = 0; i < colors; i++) // determine which color index represents white
			{
			if (lpbmiInd->bmiColors[i].rgbRed == 0xFF &&
				lpbmiInd->bmiColors[i].rgbGreen == 0xFF &&
				lpbmiInd->bmiColors[i].rgbBlue == 0xFF )
				{
				whiteIndex = i ;
				break ;
				}
			}

		// initialize the BITMAPINFO structure for the 2 color mask bitmap
		lpbmiMask = (BITMAPINFO far*) new char[sizeof(BITMAPINFOHEADER) + 2*sizeof(RGBQUAD)] ;
		InitBitmapInfoHeader (&lpbmiMask->bmiHeader, lpInfo->biWidth, lpInfo->biHeight, 1) ;
		lpbmiMask->bmiColors[BLACK].rgbRed = 0 ;  // 0 = black
		lpbmiMask->bmiColors[BLACK].rgbGreen = 0 ;
		lpbmiMask->bmiColors[BLACK].rgbBlue = 0 ;
		lpbmiMask->bmiColors[WHITE].rgbRed = 0xFF ;  // 1 = white
		lpbmiMask->bmiColors[WHITE].rgbGreen = 0xFF ;
		lpbmiMask->bmiColors[WHITE].rgbBlue = 0xFF ;
		hbmpMask = CreateDIBitmap (hdc, (BITMAPINFOHEADER far*)lpbmiMask, 0,
				NULL, lpbmiMask, DIB_RGB_COLORS) ;

		i = (WORD)(((lpInfo->biWidth + 7)/8 + 3) / 4) * 4 ; // bytes per scan line;
		lpBitsMask = new char[i] ; // i must be a multiple of 4 bytes
		for (i = 0; i < lpInfo->biHeight; i++)
			{
			lpBits = lpBitsInd+i*((((WORD)lpInfo->biWidth*lpInfo->biBitCount +7)/8 + 3)/4) *4 ;
			for (j = 0; j < lpInfo->biWidth; j++)
				{
				byte = j / 8 ;
				bit =  128 >> (j % 8) ; // 128, 64, 32, 16, ... 2, 1
				switch (lpInfo->biBitCount)
					{
					case 1 :  // monochrome
						colorIndex = lpBits[byte] ; //lpBitsInd[byte] ;
						colorIndex = (colorIndex & bit)>>(7-j%8) ;
						break ;
					case 4 :  // 16 colors
						colorIndex = lpBits[j/2] ; //lpBitsInd[j/2] ;
						if (j % 2)
							colorIndex = colorIndex & 0x0F ;
						else
							colorIndex = ( colorIndex & 0xF0 ) >> 4 ;
						break ;
					case 8 :  // 256 colors
						colorIndex = lpBits[j]&0xFF ; //lpBitsInd[j] ;
						break ;
					case 24 : // 2**24 colors
						colorIndex = (*(long*)(lpBits + 3*j)) & 0x00FFFFFFL ;
						break ;
					}
				if (bit == 128)
					lpBitsMask[byte] = 0 ;
				if (colorIndex == whiteIndex)
					{
					lpBitsMask[byte] += bit ; // the white parts of the mask will be transparent
					}
				}
			SetDIBits( hdc, hbmpMask, i, 1, lpBitsMask, lpbmiMask, DIB_RGB_COLORS) ;
			}

		delete [] lpBitsMask ;
		delete [] lpbmiMask ;
		DeleteDC (hdc) ;
 		GlobalUnlock (hDIB) ;
		}
	return hbmpMask ;
	}
