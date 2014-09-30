#include "b_includ.h"
#include "math.h"

//////////

template <class T> inline T WIDTHBYTES( T bits)
	{
	return ((bits + (T)31) / (T)32) * (T)4;
	}

/////////////////////////////////////

BOOL BMP_DATA::Draw(
	HDC											hdc,
	LPRECT 									rect,
	BITMAP_STYLE	 					bmpStyle)
	{
	POINT 									ptSize;
	long										tableSize;
	BYTE *							    tmpBMP;

	ptSize.x = rect->right - rect->left;
	ptSize.y = rect->bottom - rect->top;

	GetSize(&ptSize, bmpStyle);

	tableSize = CalcNumberOfTableEntries(m_bmpIH.biBitCount * m_bmpIH.biPlanes) * sizeof(RGBQUAD);

	tmpBMP = (BYTE *)m_rgbQuadAndBits + tableSize;

	return (BOOL)StretchDIBits(hdc,
							rect->left, rect->top, ptSize.x, ptSize.y,
							0, 0, (int)m_bmpIH.biWidth, (int)m_bmpIH.biHeight,
							tmpBMP, (LPBITMAPINFO)&m_bmpIH,
							DIB_RGB_COLORS, SRCCOPY);
	}

/////////////////////////////////////

void BMP_DATA::GetSize(
	LPPOINT									ptSize,
	BITMAP_STYLE	 					bmpStyle)
	{

	if (bmpStyle == I_CROP)
		{
		ptSize->x = min(ptSize->x, m_bmpIH.biWidth);
		ptSize->y = min(ptSize->y, m_bmpIH.biHeight);
		}

	else if (bmpStyle == I_ORIGPROP)
		{
		if (m_bmpIH.biWidth  &&  m_bmpIH.biWidth < ptSize->x)
    	{
			ptSize->x /= (int)m_bmpIH.biWidth;
			ptSize->x *= (int)m_bmpIH.biWidth;
			}
		if (m_bmpIH.biHeight  &&  m_bmpIH.biHeight < ptSize->y)
    	{
			ptSize->y /= (int)m_bmpIH.biHeight;
			ptSize->y *= (int)m_bmpIH.biHeight;
      }
		}

	else if (bmpStyle == I_UNDERTIMINED)
		{
		ptSize->x = (int)m_bmpIH.biWidth;
		ptSize->y = (int)m_bmpIH.biHeight;
		}

	}

///////////////

HBITMAP BMP_DATA::CreateMemBitmap(
	HPALETTE 								hPalette,
	LPPOINT									ptSize,
	BITMAP_STYLE 						bmpStyle)
	{
	HDC											hdcScreen, hdcMem;
	HBITMAP									hBitmap, hBitmapOld;
	RECT										rect;

	if (!hPalette)
		hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	GetSize(ptSize, bmpStyle);

	SetRect(&rect, 0, 0, ptSize->x, ptSize->y);

	hdcScreen = GetDC(NULL);
	hdcMem = CreateCompatibleDC(hdcScreen);

	hPalette = SelectPalette(hdcMem, hPalette, FALSE);
	RealizePalette(hdcMem);

	hBitmap = CreateCompatibleBitmap(hdcScreen, ptSize->x, ptSize->y);
  
	hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	Draw(hdcMem, &rect, bmpStyle);

	SelectObject(hdcMem, hBitmapOld);
	SelectPalette(hdcMem, hPalette, FALSE);

	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

  return hBitmap;
	}


///////////////////////////////////////

WORD BMP_DATA::GetPaletteForBitmap(
	AMEMBLOCKID							miPalette)
	{
	WORD										wTableEntries, i;
	RGBQUAD*								rgbBits;
	LOGPALETTE							*palette;

	wTableEntries = CalcNumberOfTableEntries(m_bmpIH.biBitCount * m_bmpIH.biPlanes);
		
	if (wTableEntries) //REMEMBER -- 16 color bitmaps can have a palette that is not the same as Windows palette
		{
		rgbBits = (RGBQUAD*)m_rgbQuadAndBits;

		AMemSetSize(miPalette, sizeof(LOGPALETTE) + wTableEntries * sizeof(PALETTEENTRY) );
		palette = (LOGPALETTE*)AMemLock(miPalette);
		I_VERIFY_POINTER(palette, return NULL);

		palette->palVersion = 0x300;
		palette->palNumEntries = wTableEntries;
		for(i=0; i<wTableEntries; i++, rgbBits++)
			{
			palette->palPalEntry[i].peRed		= rgbBits->rgbRed;
			palette->palPalEntry[i].peGreen	= rgbBits->rgbGreen;
			palette->palPalEntry[i].peBlue	= rgbBits->rgbBlue;
			palette->palPalEntry[i].peFlags	= NULL;
			}

		AMemUnlock(miPalette);
		}

	return wTableEntries;
	}

///////////////////////////////////////

HPALETTE BMP_DATA::CreatePaletteForBitmap()
	{
	HPALETTE								hPalette = NULL;
	AMEMBLOCKID							miPalette;
	LOGPALETTE*							palette;
	HDC											hdc;
	int											iSizePalette;

	//See if we are on a 256 color machine
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	iSizePalette = GetDeviceCaps(hdc, SIZEPALETTE);
	DeleteDC(hdc);
	if (iSizePalette == 256)
		{
		miPalette = AMemAllocate(0);

		if ( GetPaletteForBitmap(miPalette) )
			{
			palette = (LOGPALETTE*)AMemLock(miPalette);
			I_VERIFY_POINTER(palette, return NULL);
			hPalette = CreatePalette(palette);
			AMemUnlock(miPalette);
			}

		AMemFree(miPalette);
		}

	return hPalette;
	}


///////////////////////////////////////

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
	int										nNumColorBits;
	long										lTableSize;

	AUpsfImportField(szFieldName, 0L, AUPSF_NATV_BITMAPINFO, &m_bmpIH, lBmpSize);

	nNumColorBits = m_bmpIH.biBitCount * m_bmpIH.biPlanes;

	if(nNumColorBits!=24)
		lTableSize = (0x00000001 << nNumColorBits) * sizeof(RGBQUAD);
	else
		lTableSize = 0;

	m_bmpFH.bfType				= 0x4d42; //atoi("BM");
	m_bmpFH.bfSize				= sizeof(BITMAPFILEHEADER) + lBmpSize;
	m_bmpFH.bfReserved1		= 0;
	m_bmpFH.bfReserved2		= 0;
	m_bmpFH.bfOffBits			= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lTableSize;

	return A_OK;
	}
