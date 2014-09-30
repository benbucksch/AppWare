#include "include.h"
#include "commdlg.h"
#include "math.h"

#define RGB_BTNFACE       GetSysColor( COLOR_BTNFACE )
#define RGB_BTNSHADOW     GetSysColor( COLOR_BTNSHADOW )
#define RGB_BTNHIGHLIGHT  GetSysColor( COLOR_BTNHIGHLIGHT )
#define RGB_WHITE         RGB( 255, 255, 255 )
#define RGB_BLACK         RGB( 0, 0, 0 )

#define PSDPxax 0x00B8074AL


void PostObjectChanged(
	AOBJECTID 								oi, 
	USHORT 									usButton,						//defaults to I_UNDEFINED_INDEX
	USHORT 									usOtherButton)			//defaults to I_UNDEFINED_INDEX
	{
	AEvtInfo								event;

	event.sourceObject = event.targetObject = oi;
	event.what = AEVENT_OBJECTCHANGED;
	event.details   = (long)usButton;
	event.modifiers = (long)usOtherButton;
	event.variation = 0L;
	event.when = 0L;
	event.how = AEvtGetObjectChangedPriority(AEVT_KILLALLQUEUES | AEVT_KILLWHAT | AEVT_KILLSOURCE);

	AEvtPost(&event);
	}



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
	void 			 							*dest,
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
		(BYTE *)dest += numCopy;
		}
	while(len>0);

  }

//////////

void huge_memmove(
	void  							*dest, 
	void  							*src, 
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

    MoveMemory( (LPVOID)dest, (LPVOID)src, (unsigned)numCopy);

		len -= numCopy;
		(BYTE *)dest += numCopy;
		(BYTE *)src += numCopy;
		}
	while(len>0);
	}

////////////////////////////////

long CopyBitmapHandleToMemblock(
	AMEMBLOCKID							mi,
	HBITMAP									hBitmap,
	HPALETTE								hPalette)
	{
	LPBMP_DATA							bd;
	HDC											hdc;
	BITMAP									bitmap;
	char 		*							bits;
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

	bd = (BMP_DATA *)AMemLock(mi);

	bd->m_bmpIH.biWidth					= bitmap.bmWidth;
	bd->m_bmpIH.biHeight				= bitmap.bmHeight;
	bd->m_bmpIH.biPlanes				= 1;
	bd->m_bmpIH.biSize					= sizeof(BITMAPINFOHEADER);
	bd->m_bmpIH.biBitCount			= (SHORT)nColorBits; // casted to short to eliminate warning
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

	bd = (BMP_DATA *)AMemLock(mi);

	bits = (char *)bd->m_rgbQuadAndBits + tableSize;
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
	AMEMBLOCKID							miDest,
	AMEMBLOCKID							miSource)
	{
	LONG										lSize;
	void 								*dest;
	void 								*src;

	lSize = AMemGetSize(miSource);
	AMemSetSize(miDest, lSize);

	if (lSize)
		{

		dest = (void *)AMemLock(miDest);
		src = (void *)AMemLock(miSource);

		CopyMemory(dest, src, lSize);

		AMemUnlock(miDest);
		AMemUnlock(miSource);
		}

	return lSize;
	}

/////////////////////////////////////

long CopyDatasToMemBlocks(
	AMEMBLOCKID							miDest,
	AOBJECTID								oiSource,
	LONG										idxSource)
	{
	LONG										lSize;
	void 								*dest;
	void 								*src;

	lSize = AObjGetDataSize(oiSource, idxSource);
	AMemSetSize(miDest, lSize);

	if (lSize)
		{
		src = (void *)AObjLockData(oiSource, idxSource);
	  dest = (void *)AMemLock(miDest);

		CopyMemory(dest, src, lSize);

		AObjUnlockData(oiSource, idxSource);
		AMemUnlock(miDest);
		}

	return lSize;
	}

/////////////////////////////////

long CopyMemBlocksToDatas(
	AOBJECTID								oiDest,
	LONG										idxDest,
	AMEMBLOCKID							miSource)
	{
	LONG										lSize;
	void 								*dest;
	void 								*src;

	lSize = AMemGetSize(miSource);
	AObjResizeData(oiDest, idxDest, lSize);

	if (lSize)
		{
		dest = (void  *)AObjLockData(oiDest, idxDest);
		src = (void  *)AMemLock(miSource);

		CopyMemory(dest, src, lSize);

		AObjUnlockData(oiDest, idxDest);
		AMemUnlock(miSource);
		}

	return lSize;
	}

//////

long CopyDatas(
	AOBJECTID 								oiDest, 
	LONG 										idxDest, 
	AOBJECTID 								oiSource, 
	LONG 										idxSource)
	{
	LONG										lSize;
	void 								*dest;
	void 								*src;

	lSize = AObjGetDataSize(oiSource, idxSource);
	AObjResizeData(oiDest, idxDest, lSize);

	if (lSize)
		{
		dest = (void *)AObjLockData(oiDest, idxDest);
		src = (void *)AObjLockData(oiSource, idxSource);

		CopyMemory(dest, src, lSize);

		AObjUnlockData(oiDest, idxDest);
		AObjUnlockData(oiSource, idxSource);
		}

	return lSize;
	}

////////

BOOL CopyBitmapFromClipboardToMemblock(
	AMEMBLOCKID 							miBitmap, 
	HWND 										hwnd)
	{
	HBITMAP									hBitmapClipboard;
	HPALETTE								hPalette;
  BOOL										rVal = FALSE;
	HANDLE									hDib;
	long										lHandleSize;
	BYTE  *							ptrDest;
	BYTE  *							ptrSource;

	if ( OpenClipboard(hwnd) )
  	{
		if (IsClipboardFormatAvailable(CF_DIB))
			{
			hDib = GetClipboardData(CF_DIB);
			lHandleSize = GlobalSize(hDib);
			if (AMemSetSize(miBitmap, lHandleSize + sizeof(BITMAPFILEHEADER) ))
				{
				ptrSource = (BYTE *)GlobalLock(hDib);
				I_VERIFY_POINTER(ptrSource, return FALSE);

				ptrDest = (BYTE *)AMemLock(miBitmap);
				I_VERIFY_POINTER(ptrDest, GlobalUnlock(hDib);return FALSE);

				ptrDest += sizeof(BITMAPFILEHEADER);		 //cause hDib contains a bitminfoheader followed by the bits
				CopyMemory(ptrDest, ptrSource, lHandleSize);

				GlobalUnlock(hDib);
				AMemUnlock(miBitmap);

				rVal = TRUE;
				}
			}

		else if (IsClipboardFormatAvailable(CF_BITMAP))
			{
			hBitmapClipboard = (HBITMAP)GetClipboardData(CF_BITMAP);
			hPalette = (HPALETTE)GetClipboardData(CF_PALETTE);
			CopyBitmapHandleToMemblock(miBitmap, hBitmapClipboard, hPalette);
			rVal = TRUE;
			}

		CloseClipboard();
		}

	return rVal;
	}

////////
BOOL LoadBitmapToMemblock(
	AMEMBLOCKID 							miBitmap)
	{
	OPENFILENAME						ofn;
	char 		 								szFullFileName[256];
	LPBMP_DATA							bmpData;
	HANDLE									hFile;
	DWORD										dwSize, dwRead;
  short										type;
	BOOL										rVal = FALSE;

	szFullFileName[0] = 0;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize			 = sizeof(OPENFILENAME);
	ofn.hwndOwner				 = GetActiveWindow();
 	ofn.lpstrFilter			 = "Bitmaps\0*.bmp\0All Files\0*.*\0";
	ofn.nFilterIndex		 = 1;
	ofn.lpstrFile				 = szFullFileName;
	ofn.nMaxFile				 = sizeof(szFullFileName);
	ofn.lpstrInitialDir	 = g_szBitmapPathName;
	ofn.lpstrTitle			 = "Load Bitmap";	
	ofn.lpstrDefExt			 = "bmp";
  ofn.Flags						 = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

	if (GetOpenFileName(&ofn))
		{
		hFile = CreateFile(szFullFileName, GENERIC_READ, 0, NULL, 
											 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
	  	{
			dwSize = SetFilePointer (hFile, 0L, NULL, FILE_END) ;
			SetFilePointer (hFile, 0L, NULL, FILE_BEGIN) ;

	    //make sure its really a bitmap
			ReadFile(hFile, (LPSTR)&type, sizeof(short), &dwRead, NULL);
			if (type == 0x4d42) //"BM"
	    	{
				SetFilePointer(hFile, 0L, NULL, FILE_BEGIN);	// reset the file pointer

				AMemSetSize(miBitmap, dwSize + sizeof(BYTE));
				bmpData = (LPBMP_DATA)AMemLock(miBitmap);

				ReadFile(hFile, bmpData, dwSize, &dwRead, NULL);
				AMemUnlock(miBitmap);
				rVal = TRUE;
				}

			CloseHandle(hFile);
			}
		}

	return rVal;
	}

////////

HBITMAP CreateBitmapHandle(
	HPALETTE								hPalette, 
	AMEMBLOCKID 							miBitmap)
	{
	LPBMP_DATA							bd;
	HBITMAP									rVal = NULL;

	if (AMemGetSize(miBitmap))
		{
		bd = (LPBMP_DATA)AMemLock(miBitmap);
		I_VERIFY_POINTER(bd, return NULL);

		rVal = bd->CreateMemBitmap(hPalette);

		AMemUnlock(miBitmap);
		}

	return rVal;
	}

////////

HBITMAP CreateBitmapHandle(
	HPALETTE								hPalette, 
	AOBJECTID 								oiData, 
	long 										lIdxData)
	{
	LPBMP_DATA							bd;
	HBITMAP									rVal = NULL;

	if (AObjGetDataSize(oiData, lIdxData))
		{
		bd = (LPBMP_DATA)AObjLockData(oiData, lIdxData);
		I_VERIFY_POINTER(bd, return NULL);

		rVal = bd->CreateMemBitmap(hPalette);

		AObjUnlockData(oiData, lIdxData);
		}

	return rVal;
	}

////////

HPALETTE CreateBitmapPalette(
	AMEMBLOCKID 							miBitmap)
	{
	LPBMP_DATA							bd;
	HPALETTE								rVal = NULL;

	if (AMemGetSize(miBitmap))
		{
		bd = (LPBMP_DATA)AMemLock(miBitmap);
		I_VERIFY_POINTER(bd, return NULL);

		rVal = bd->CreatePaletteForBitmap();

		AMemUnlock(miBitmap);
		}

	return rVal;
	}

////////


void GetBitmapSize(
	AMEMBLOCKID 							miBitmap, 
	LPPOINT 								pPtSize)
	{
	LPBMP_DATA							bd;

	if (AMemGetSize(miBitmap))
		{
		bd = (LPBMP_DATA)AMemLock(miBitmap);
		I_VERIFY_POINTER(bd, pPtSize->x = pPtSize->y = 0; return);

		pPtSize->x = (int)bd->m_bmpIH.biWidth;
		pPtSize->y = (int)bd->m_bmpIH.biHeight;

		AMemUnlock(miBitmap);
		}
	else
		pPtSize->x = pPtSize->y = 0;
		
	}
