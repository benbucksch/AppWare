#include "include.h"
#include "math.h"

//////////

template <class T> inline T WIDTHBYTES( T bits)
	{
	return ((bits + (T)31) / (T)32) * (T)4;
	}

/////////////////////////////////////

HBITMAP BMP_DATA::CreateMemBitmap(
	HPALETTE 								hPalette)
	{
	HDC											hdcScreen, hdcMem;
	HBITMAP									hBitmap, hBitmapOld;
	long										tableSize;
	BYTE huge *							tmpBMP;

	if (!hPalette)
		hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	hdcScreen = GetDC(NULL);
	hdcMem = CreateCompatibleDC(hdcScreen);

	hPalette = SelectPalette(hdcMem, hPalette, FALSE);
	RealizePalette(hdcMem);

	hBitmap = CreateCompatibleBitmap(hdcScreen, (int)m_bmpIH.biWidth, (int)m_bmpIH.biHeight);
	hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	tableSize = CalcNumberOfTableEntries(m_bmpIH.biBitCount * m_bmpIH.biPlanes) * sizeof(RGBQUAD);
	tmpBMP = (BYTE huge *)m_rgbQuadAndBits + tableSize;

	//Would SetDIBits be better???
	StretchDIBits(hdcMem,
							0, 0, (int)m_bmpIH.biWidth, (int)m_bmpIH.biHeight,
							0, 0, (int)m_bmpIH.biWidth, (int)m_bmpIH.biHeight,
							tmpBMP, (LPBITMAPINFO)&m_bmpIH,
							DIB_RGB_COLORS, SRCCOPY);

	SelectObject(hdcMem, hBitmapOld);
	SelectPalette(hdcMem, hPalette, FALSE);

	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

  return hBitmap;
	}


///////////////////////////////////////

///////////////////////////////////////

BOOL BMP_DATA::GetBestPaletteForBitmap(
	MEMBLOCKID 							miPalette)
	{
	int											nTableEntries, i;
	LOGPALETTE*							palette;
	RGBQUAD*								rgbBits;
	HDC											hdc;
	int											iSizePalette;
	BOOL										rVal = FALSE;

	//See if we are on a 256 color machine
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	iSizePalette = GetDeviceCaps(hdc, SIZEPALETTE);
	DeleteDC(hdc);
	if (iSizePalette == 256)
		{
		nTableEntries = CalcNumberOfTableEntries(m_bmpIH.biBitCount * m_bmpIH.biPlanes);
		
		if (nTableEntries) //REMEMBER -- 16 color bitmaps can have a palette that is not the same as Windows palette
			{
			if (AMemSetSize(miPalette, sizeof(LOGPALETTE) + nTableEntries * sizeof(PALETTEENTRY) ))
				{
				palette = (LOGPALETTE*)AMemLock(miPalette);
				I_VERIFY_POINTER(palette, return NULL);

				rgbBits = (RGBQUAD*)m_rgbQuadAndBits;

				palette->palVersion = 0x300;
				palette->palNumEntries = nTableEntries;
				for(i=0; i<nTableEntries; i++, rgbBits++)
					{
					palette->palPalEntry[i].peRed		= rgbBits->rgbRed;
					palette->palPalEntry[i].peGreen	= rgbBits->rgbGreen;
					palette->palPalEntry[i].peBlue	= rgbBits->rgbBlue;
					palette->palPalEntry[i].peFlags	= NULL;
					}

				AMemUnlock(miPalette);
				rVal = TRUE;
				}
			}
		}

	return rVal;
	}

///////////////////////////////////////

HPALETTE BMP_DATA::CreatePaletteForBitmap()
	{
	HPALETTE								hPalette = NULL;
	MEMBLOCKID							miPalette;
	LOGPALETTE*							palette;

	miPalette = AMemAllocate(1);

	if (GetBestPaletteForBitmap(miPalette))
		{
		palette = (LOGPALETTE*)AMemLock(miPalette);
		I_VERIFY_POINTER(palette, return NULL);

		hPalette = CreatePalette(palette);

		AMemUnlock(miPalette);
		AMemFree(miPalette);
		}

	return hPalette;
	}


///////////////////////////////////////
/// Name: BMP_DATA::Export
/// Purpose: UPSF Exports the bitmap
/// Parameters: szFieldName -- the name of the UPSF field
LONG BMP_DATA::Export(
	LPSTR										szFieldName)
	{

	//Thanks to MicroSucks, this field is not alway set.  We, therefore, need to calculate it.
	m_bmpIH.biSizeImage = WIDTHBYTES((long)m_bmpIH.biWidth * (long)m_bmpIH.biBitCount * (long)m_bmpIH.biPlanes) * m_bmpIH.biHeight;

	AUpsfExportField(szFieldName, 0L, AUPSF_NATV_BITMAPINFO, AUPSF_UNIV_BITMAP, &m_bmpIH, 
		m_bmpIH.biSizeImage + sizeof(BITMAPINFOHEADER) + CalcNumberOfTableEntries(m_bmpIH.biBitCount * m_bmpIH.biPlanes) * sizeof(RGBQUAD)  );

	return A_OK;
	}

///////////////////////////////////////

LONG BMP_DATA::Import(
	LPSTR										szFieldName,
	LONG										lBmpSize)
	{
	short										sNumColorBits;
	long										lTableSize;

	AUpsfImportField(szFieldName, 0L, AUPSF_NATV_BITMAPINFO, &m_bmpIH, lBmpSize);

	sNumColorBits = m_bmpIH.biBitCount * m_bmpIH.biPlanes;

	if(sNumColorBits!=24)
		lTableSize = (0x00000001 << sNumColorBits) * sizeof(RGBQUAD);
	else
		lTableSize = 0;

	m_bmpFH.bfType				= 0x4d42; //atoi("BM");
	m_bmpFH.bfSize				= sizeof(BITMAPFILEHEADER) + lBmpSize;
	m_bmpFH.bfReserved1		= 0;
	m_bmpFH.bfReserved2		= 0;
	m_bmpFH.bfOffBits			= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lTableSize;

	return A_OK;
	}


              
              
              
              
    
  	