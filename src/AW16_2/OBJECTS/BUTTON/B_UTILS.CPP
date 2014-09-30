#include "b_include.h"
#include "math.h"

#define RGB_BTNFACE       GetSysColor( COLOR_BTNFACE )
#define RGB_BTNSHADOW     GetSysColor( COLOR_BTNSHADOW )
#define RGB_BTNHIGHLIGHT  GetSysColor( COLOR_BTNHIGHLIGHT )
#define RGB_WHITE         RGB( 255, 255, 255 )
#define RGB_BLACK         RGB( 0, 0, 0 )

#define PSDPxax 0x00B8074AL


template <class T> inline T WIDTHBYTES( T bits)
	{
	return ((bits + (T)31) / (T)32) * (T)4;
	}


////////////////////////////////////
/// This function figures out the table size of a 
///  bitmap based on the number of color bits.
////////////////////////////////////

int CalcNumberOfTableEntries(
	int 										nColorBits)
	{
	if (nColorBits <=1)
		return 2;

	if (nColorBits <=4)
		return 16;

	if (nColorBits <=8)
		 return 256;
	
	return 0;
	}

///////////////////////////////////////////////////
// This file contains utility functions that the 
// button design-time editing uses
///////////////////////////////////////////////

void huge_memset(
	void huge 							*dest,
	char 										c,
	long 										len)
	{
	long										numCopy;
	long										destOffset;

	do
		{
		destOffset =  0x10000L - LOWORD((long)dest);
		numCopy = min(destOffset, len);
		numCopy = min(numCopy, 0xFFFFL);

		_fmemset( (LPVOID)dest, c, (unsigned)numCopy);

		len -= numCopy;
		(BYTE huge *)dest += numCopy;
		}
	while(len>0);

  }

//////////

void huge_memmove(
	void huge 							*dest, 
	void huge 							*src, 
	long 										len)
	{
	long										numCopy;
	long										srcOffset, destOffset;		

	do
		{
		destOffset = 0x10000L - LOWORD((long)dest);
		srcOffset =  0x10000L - LOWORD((long)src);
		numCopy = min(destOffset, srcOffset);
    numCopy = min( numCopy, 0xFFFFL);
		numCopy = min(numCopy, len);

    memmove( (LPVOID)dest, (LPVOID)src, (unsigned)numCopy);

		len -= numCopy;
		(BYTE huge *)dest += numCopy;
		(BYTE huge *)src += numCopy;
		}
	while(len>0);
	}

////////////////////////////////

long CopyBitmapHandleToMemblock(
	MEMBLOCKID							mi,
	HBITMAP									hBitmap,
	HPALETTE								hPalette)
	{
	LPBMP_DATA							bd;
	HDC											hdc;
	BITMAP									bitmap;
	char huge*							bits;
	int											nColorBits;
	unsigned long						tableSize;
	long										bmpSize, memblockSize;

	if (!hPalette)
		hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	::GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);

	nColorBits = bitmap.bmBitsPixel*bitmap.bmPlanes;
	nColorBits = nColorBits <=1 ? 1 : nColorBits <=4 ? 4 : nColorBits <= 8 ? 8 : 24;
	tableSize = CalcNumberOfTableEntries(nColorBits) * sizeof(RGBQUAD);

	memblockSize = sizeof(BYTE) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize;
	AMemSetSize(mi, memblockSize);

	bd = (BMP_DATA far*)AMemLock(mi);

	bd->m_bmpIH.biWidth					= bitmap.bmWidth;
	bd->m_bmpIH.biHeight				= bitmap.bmHeight;
	bd->m_bmpIH.biPlanes				= 1;
	bd->m_bmpIH.biSize					= sizeof(BITMAPINFOHEADER);
	bd->m_bmpIH.biBitCount			= nColorBits;
	bd->m_bmpIH.biCompression		= BI_RGB;
	bd->m_bmpIH.biXPelsPerMeter	= 0;
	bd->m_bmpIH.biYPelsPerMeter	= 0;
	bd->m_bmpIH.biSizeImage			= WIDTHBYTES((DWORD)bitmap.bmWidth * nColorBits) * bitmap.bmHeight;
	bd->m_bmpIH.biClrUsed				= 0;
	bd->m_bmpIH.biClrImportant	= 0;

	bmpSize = bd->m_bmpIH.biSizeImage;

	// resize the data without losing information
	AMemUnlock(mi);
	memblockSize = sizeof(BYTE) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize + bmpSize;
	AMemSetSize(mi, memblockSize);

	bd = (BMP_DATA far*)AMemLock(mi);

	bits = (char huge*)bd->m_rgbQuadAndBits + tableSize;
	huge_memset(bits, 0, bmpSize);

	hdc = GetDC(NULL);

	hPalette = SelectPalette(hdc, hPalette, FALSE);
	RealizePalette(hdc);


	// fill in the bitmap data
	GetDIBits(hdc, hBitmap, 0, bitmap.bmHeight, (LPSTR)bits,
						(LPBITMAPINFO)&bd->m_bmpIH, DIB_RGB_COLORS);


	bd->m_bmpFH.bfType			= 0x4d42; //atoi("BM");
	bd->m_bmpFH.bfSize			= sizeof(BITMAPFILEHEADER) +
																sizeof(BITMAPINFOHEADER) +
																tableSize + bmpSize;
	bd->m_bmpFH.bfReserved1	= 0;
	bd->m_bmpFH.bfReserved2	= 0;
	bd->m_bmpFH.bfOffBits		= sizeof(BITMAPFILEHEADER) +
																sizeof(BITMAPINFOHEADER) +
																tableSize;

	AMemUnlock(mi);

	SelectPalette(hdc, hPalette, FALSE);
	ReleaseDC(NULL, hdc);

	return memblockSize;
	}

/////////////////////////////

long CopyMemBlocks(
	MEMBLOCKID							miDest,
	MEMBLOCKID							miSource)
	{
	LONG										lSize;
	void huge								*dest;
	void huge								*src;

	lSize = AMemGetSize(miSource);
	AMemSetSize(miDest, lSize);

	if (lSize)
		{

		dest = (void huge *)AMemLock(miDest);
		src = (void huge *)AMemLock(miSource);

		hmemcpy(dest, src, lSize);

		AMemUnlock(miDest);
		AMemUnlock(miSource);
		}

	return lSize;
	}

/////////////////////////////////////

long CopyDatasToMemBlocks(
	MEMBLOCKID							miDest,
	OBJECTID								oiSource,
	LONG										idxSource)
	{
	LONG										lSize;
	void huge								*dest;
	void huge								*src;

	lSize = AObjGetDataSize(oiSource, idxSource);
	AMemSetSize(miDest, lSize);

	if (lSize)
		{
		src = (void huge *)AObjLockData(oiSource, idxSource);
	  dest = (void huge *)AMemLock(miDest);

		hmemcpy(dest, src, lSize);

		AObjUnlockData(oiSource, idxSource);
		AMemUnlock(miDest);
		}

	return lSize;
	}

/////////////////////////////////

long CopyMemBlocksToDatas(
	OBJECTID								oiDest,
	LONG										idxDest,
	MEMBLOCKID							miSource)
	{
	LONG										lSize;
	void huge								*dest;
	void huge								*src;

	lSize = AMemGetSize(miSource);
	AObjResizeData(oiDest, idxDest, lSize);

	if (lSize)
		{
		dest = (void huge *)AObjLockData(oiDest, idxDest);
		src = (void huge *)AMemLock(miSource);

		hmemcpy(dest, src, lSize);

		AObjUnlockData(oiDest, idxDest);
		AMemUnlock(miSource);
		}

	return lSize;
	}


//////////////////////////////////////////////////////
// Functions to create a default normal bitmaps //////
//////////////////////////////////////////////////////

HBITMAP CreateDefaultNormalBitmap()
	{
	HDC											hdcScreen, hdcMem;
	POINT										size;
	HBITMAP									hBitmap, hBitmapOld;
	RECT										rect;
	HBRUSH									hBrush;
	HFONT										hFont;
	COLORREF								backColor, foreColor;
  int											backMode;

	size.x = I_DEFAULT_BM_WIDTH;
	size.y = I_DEFAULT_BM_HEIGHT;

	hdcScreen = GetDC(NULL);
	hdcMem = CreateCompatibleDC(hdcScreen);

	hBitmap = CreateCompatibleBitmap(hdcScreen, size.x, size.y);
	hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	SetRect(&rect, 0, 0, size.x, size.y);

	hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(hdcMem, &rect, hBrush);
	DeleteObject(hBrush);

	hFont = (HFONT)GetStockObject(SYSTEM_FONT);
	hFont = (HFONT)SelectObject(hdcMem, hFont);
	backColor = SetBkColor(hdcMem, GetSysColor(COLOR_WINDOW) );
	foreColor = SetTextColor(hdcMem, GetSysColor(COLOR_WINDOWTEXT) );
	backMode = SetBkMode(hdcMem, OPAQUE);

	DrawText(hdcMem, "Bitmap", -1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	SelectObject(hdcMem, hFont);
	SetBkColor(hdcMem, backColor);
	SetTextColor(hdcMem, foreColor);
	SetBkMode(hdcMem, backMode);

	SelectObject(hdcMem, hBitmapOld); //hBitmap must not be selected into a DC

	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	return hBitmap;
	}


void CreateDefaultNormalBitmap(
	MEMBLOCKID							miNormal)
	{
	HBITMAP									hBitmap;

	hBitmap = CreateDefaultNormalBitmap();
	CopyBitmapHandleToMemblock(miNormal, hBitmap, NULL);
	DeleteObject(hBitmap);
	}


//////////////////////////////////////////////////////
// Functions to create a default focused bitmaps /////
//////////////////////////////////////////////////////

HBITMAP CreateDefaultFocusedBitmap(
	HPALETTE 								hPalette,
	HBITMAP 								hbmpNormal)
	{
	HDC											hdcScreen, hdcMemSource, hdcMemDest;
 	HBITMAP									hbmpDest, hbmpOldSource, hbmpOldDest;
	HPALETTE								hPalOldDest, hPalOldSource;
 	RECT										rect;
 	BITMAP									bitmap;
 
 	GetObject(hbmpNormal, sizeof(BITMAP), &bitmap);			 
 
	if (!hPalette)
		hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

 	hdcScreen = GetDC(NULL);
	hdcMemSource = CreateCompatibleDC(hdcScreen);
	hdcMemDest = CreateCompatibleDC(hdcScreen);

	hPalOldSource = SelectPalette(hdcMemSource, hPalette, FALSE);
	hPalOldDest = SelectPalette(hdcMemDest, hPalette, FALSE);
	RealizePalette(hdcMemSource);
	RealizePalette(hdcMemDest);
 
	hbmpDest = CreateCompatibleBitmap(hdcScreen, bitmap.bmWidth, bitmap.bmHeight);
		
	hbmpOldSource = (HBITMAP)SelectObject(hdcMemSource, hbmpNormal);
	hbmpOldDest = (HBITMAP)SelectObject(hdcMemDest, hbmpDest);

	BitBlt(hdcMemDest, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMemSource, 0, 0, SRCCOPY);

	SetRect(&rect, 0, 0, bitmap.bmWidth, bitmap.bmHeight);
	FrameRect(hdcMemDest, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	SelectObject(hdcMemDest, hbmpOldDest);
	SelectObject(hdcMemSource, hbmpOldSource);

	SelectPalette(hdcMemSource, hPalOldSource, FALSE);
	SelectPalette(hdcMemDest, hPalOldDest, FALSE);

	DeleteDC(hdcMemDest);
	DeleteDC(hdcMemSource);

	ReleaseDC(NULL, hdcScreen);

	return hbmpDest;
	}

/////////

HBITMAP CreateDefaultFocusedBitmap(
	HPALETTE 								hPalette,
	MEMBLOCKID 							miNormal)
	{
	LPBMP_DATA							bd;
	HBITMAP									hBitmapNormal, hBitmapFocused;
	POINT										ptSize;

	bd = (LPBMP_DATA)AMemLock(miNormal);
	I_ASSERT_POINTER(bd, return NULL);

	bd->GetSize(&ptSize, I_UNDERTIMINED);
	hBitmapNormal = bd->CreateMemBitmap(hPalette, &ptSize, I_UNDERTIMINED);

	AMemUnlock(miNormal);

	hBitmapFocused = CreateDefaultFocusedBitmap(hPalette, hBitmapNormal);

	DeleteObject(hBitmapNormal);

	return hBitmapFocused;
	}

///////////////////////

void CreateDefaultFocusedBitmap(
	HPALETTE 								hPalette,
	MEMBLOCKID							miNormal,
	MEMBLOCKID							miFocused)
	{
	HBITMAP									hbmpFocused;

	hbmpFocused = CreateDefaultFocusedBitmap(hPalette, miNormal);
	CopyBitmapHandleToMemblock(miFocused, hbmpFocused, hPalette);
	DeleteObject(hbmpFocused);
	}

//////////////////////////////////////////////////////
// Functions to create a default pressed bitmaps /////
//////////////////////////////////////////////////////

HBITMAP CreateDefaultPressedBitmap(
	HPALETTE 								hPalette,
	HBITMAP 								hbmpNormal)
	{
	HDC											hdcScreen, hdcMemSource, hdcMemDest;
 	HBITMAP									hbmpDest, hbmpOldSource, hbmpOldDest;
	HPALETTE								hPalOldDest, hPalOldSource;
 	RECT										rect;
 	BITMAP									bitmap;
 
 	GetObject(hbmpNormal, sizeof(BITMAP), &bitmap);			 
 
	if (!hPalette)
		hPalette = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

 	hdcScreen = GetDC(NULL);
	hdcMemSource = CreateCompatibleDC(hdcScreen);
	hdcMemDest = CreateCompatibleDC(hdcScreen);

	hPalOldSource = SelectPalette(hdcMemSource, hPalette, FALSE);
	hPalOldDest = SelectPalette(hdcMemDest, hPalette, FALSE);
	RealizePalette(hdcMemSource);
	RealizePalette(hdcMemDest);
 
	hbmpDest = CreateCompatibleBitmap(hdcScreen, bitmap.bmWidth, bitmap.bmHeight);
		
	hbmpOldSource = (HBITMAP)SelectObject(hdcMemSource, hbmpNormal);
	hbmpOldDest = (HBITMAP)SelectObject(hdcMemDest, hbmpDest);

	SetRect(&rect, 0, 0, bitmap.bmWidth, bitmap.bmHeight);
	FillRect(hdcMemDest, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	//InvertRect(hdcMemDest, &rect);

	BitBlt(hdcMemDest, 2, 2, bitmap.bmWidth-2, bitmap.bmHeight-2, hdcMemSource, 0, 0, SRCCOPY);

	SelectObject(hdcMemDest, hbmpOldDest);
	SelectObject(hdcMemSource, hbmpOldSource);

	SelectPalette(hdcMemSource, hPalOldSource, FALSE);
	SelectPalette(hdcMemDest, hPalOldDest, FALSE);

	DeleteDC(hdcMemDest);
	DeleteDC(hdcMemSource);

	ReleaseDC(NULL, hdcScreen);

	return hbmpDest;
	}

//////////

HBITMAP CreateDefaultPressedBitmap(
	HPALETTE 								hPalette,
	MEMBLOCKID 							miNormal)
	{
	LPBMP_DATA							bdNormal;
	HBITMAP									hbmpNormal, hbmpPressed; 
	POINT										ptSize;

	bdNormal = (LPBMP_DATA)AMemLock(miNormal);
	I_ASSERT_POINTER(bdNormal, return NULL);

	bdNormal->GetSize(&ptSize, I_UNDERTIMINED);
	hbmpNormal = bdNormal->CreateMemBitmap(hPalette, &ptSize, I_UNDERTIMINED);

	AMemUnlock(miNormal);

	hbmpPressed = CreateDefaultPressedBitmap(hPalette, hbmpNormal);

	DeleteObject(hbmpNormal);

	return hbmpPressed;
	}

///////

void CreateDefaultPressedBitmap(
	HPALETTE 								hPalette,
	MEMBLOCKID							miNormal,
	MEMBLOCKID							miPressed)
	{
	HBITMAP									hbmpPressed;

	hbmpPressed = CreateDefaultPressedBitmap(hPalette, miNormal);
	CopyBitmapHandleToMemblock(miPressed, hbmpPressed, hPalette);
	DeleteObject(hbmpPressed);
	}

//////////////////////////////////////////////////////
// Functions to create a default grayed bitmaps /////
//////////////////////////////////////////////////////
HBITMAP CreateDefaultGrayedBitmap(
	HPALETTE 								hPalette,
	HBITMAP 								hbmpNormal)
	{
  BITMAP   								bmInfo;
  HBITMAP  								hbmOld, hbmShadow, hbmHighlight, hbmDisable;
  HBRUSH   								hbrPat;
  HDC      								hdcScreen, hColorDC, hMonoDC ;

   // We essentially need to create 2 monochrome bitmaps:  one defining the 
   // "highlight" area on the bitmap, and one defining the "shadow"
   // area, which is how we make the resulting bitmap look 3-D.  Both
   // the highlight and shadow bitmaps are created by offsetting
   // calls to BitBlt() using various ROP codes.  hbmDisable is our 
   // result bitmap.

	I_UNUSED(hPalette);

	hbmDisable = NULL ;

	hdcScreen = GetDC(NULL);
	hMonoDC = CreateCompatibleDC(hdcScreen);
	hColorDC = CreateCompatibleDC(hdcScreen);

  if (hMonoDC && hColorDC)
	   {
      // create the monochrome and color bitmaps and
      // necessary DCs

      GetObject( hbmpNormal, sizeof( BITMAP ), (LPSTR) &bmInfo ) ;
      hbmShadow =
         CreateBitmap( bmInfo.bmWidth, bmInfo.bmHeight, 1, 1, NULL ) ;
      hbmHighlight =
         CreateBitmap( bmInfo.bmWidth, bmInfo.bmHeight, 1, 1, NULL ) ;
      hbmDisable =
         CreateCompatibleBitmap( hdcScreen, bmInfo.bmWidth, bmInfo.bmHeight ) ;

      hbmOld = (HBITMAP)SelectObject( hColorDC, hbmpNormal ) ;

      // set background color of bitmap for mono conversion

      SetBkColor( hColorDC, GetPixel( hColorDC, 0, 0 ) ) ;

      // Create the shadow bitmap.

      hbmShadow = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmShadow ) ;
      PatBlt( hMonoDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, WHITENESS ) ;
      BitBlt( hMonoDC, 0, 0, bmInfo.bmWidth - 1, bmInfo.bmHeight - 1,
              hColorDC, 1, 1, SRCCOPY ) ;
      BitBlt( hMonoDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight,
              hColorDC, 0, 0, MERGEPAINT ) ;

      hbmShadow = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmShadow ) ;

      // create the highlight bitmap

      hbmHighlight = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmHighlight ) ;
      BitBlt( hMonoDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight,
              hColorDC, 0, 0, SRCCOPY ) ;
      BitBlt( hMonoDC, 0, 0, bmInfo.bmWidth- 1, bmInfo.bmHeight - 1,
              hColorDC, 1, 1, MERGEPAINT ) ;
      hbmHighlight = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmHighlight ) ;

      // select old bitmap into color DC

      SelectObject( hColorDC, hbmOld ) ;

      // clear the background for the disabled bitmap

      SelectObject( hColorDC, hbmDisable ) ;

      hbrPat = CreateSolidBrush( RGB_BTNFACE ) ;
      hbrPat = (HBRUSH)SelectObject( hColorDC, hbrPat ) ;
      PatBlt( hColorDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, PATCOPY ) ;
      DeleteObject( SelectObject( hColorDC, hbrPat ) ) ;
      SetBkColor( hColorDC, RGB_WHITE ) ;
      SetTextColor( hColorDC, RGB_BLACK ) ;

      // blt the highlight edge

      hbrPat = CreateSolidBrush( RGB_BTNHIGHLIGHT ) ;
      hbrPat = (HBRUSH)SelectObject( hColorDC, hbrPat ) ;
      hbmHighlight = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmHighlight ) ;
      BitBlt( hColorDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight,
              hMonoDC, 0, 0, PSDPxax ) ;
      DeleteObject( SelectObject( hColorDC, hbrPat ) ) ;
      hbmHighlight = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmHighlight ) ;

      // blt the shadow edge

      hbrPat = CreateSolidBrush( RGB_BTNSHADOW ) ;
      hbrPat = (HBRUSH)SelectObject( hColorDC, hbrPat ) ;
      hbmShadow = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmShadow ) ;
      BitBlt( hColorDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight,
              hMonoDC, 0, 0, PSDPxax ) ;
      DeleteObject( SelectObject( hColorDC, hbrPat ) ) ;
      hbmShadow = (HBITMAP)SelectObject( hMonoDC, (HGDIOBJ) hbmShadow ) ;

      // select old bitmap into color DC - hbmColor now
      // contains the "tinfoil'd" bitmap

      SelectObject( hColorDC, hbmOld ) ;

      DeleteObject( (HGDIOBJ) hbmShadow ) ;
      DeleteObject( (HGDIOBJ) hbmHighlight ) ;
	   }

   if (hMonoDC)
      DeleteDC( hMonoDC );

   if (hColorDC)
      DeleteDC( hColorDC );

	ReleaseDC(NULL, hdcScreen);

	return hbmDisable;
	}

///////////////

HBITMAP CreateDefaultGrayedBitmap(
	HPALETTE 								hPalette,
	MEMBLOCKID 							miNormal)
	{
	LPBMP_DATA							bdNormal;
	HBITMAP									hbmpNormal, hbmpGrayed; 
	POINT										ptSize;

	bdNormal = (LPBMP_DATA)AMemLock(miNormal);
	I_ASSERT_POINTER(bdNormal, return NULL);

	bdNormal->GetSize(&ptSize, I_UNDERTIMINED);
	hbmpNormal = bdNormal->CreateMemBitmap(hPalette, &ptSize, I_UNDERTIMINED);

	AMemUnlock(miNormal);

	hbmpGrayed = CreateDefaultGrayedBitmap(hPalette, hbmpNormal);

	DeleteObject(hbmpNormal);

	return hbmpGrayed;
	}

//////////

void CreateDefaultGrayedBitmap(
	HPALETTE 								hPalette,
	MEMBLOCKID 							miNormal, 
	MEMBLOCKID 							miGrayed)
	{
	HBITMAP									hbmpGrayed;

	hbmpGrayed = CreateDefaultGrayedBitmap(hPalette, miNormal);
	CopyBitmapHandleToMemblock(miGrayed, hbmpGrayed, hPalette);
	DeleteObject(hbmpGrayed);
	}
