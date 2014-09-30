#define 	STRICT
#define  	ASTRICT

#include <windows.h>
#include <commDlg.h>
#include <mem.h>
#include <string.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_number.h>
#include "scale.h"

#if WINVER < 0x030a
void	CopyMemory(void _huge* dest, const void _huge* source, long size)

{
	BYTE huge*	srcPtr = (BYTE huge*)source;
	BYTE huge*	dstPtr = (BYTE huge*)dest;

	WORD	srcOffset;
	WORD	dstOffset;
	DWORD	copycount;

	while(size > 0L) {
		srcOffset = LOWORD((long)srcPtr);
		dstOffset = LOWORD((long)dstPtr);

		copycount = 0x00010000 - (srcOffset>dstOffset ? srcOffset : dstOffset);
		if(copycount > size)
			copycount = size;
		if(copycount > 0x0000FFFF)
			copycount = 0x0000FFFF;

		CopyMemory((BYTE*)dstPtr, (BYTE*)srcPtr, copycount);
		srcPtr	+= copycount;
		dstPtr	+= copycount;
		size	-= copycount;
		}
}
#endif
WORD DIBNumColors (LPSTR lpbi)
{
   WORD wBitCount;
   DWORD dwClrUsed;

   dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

   if (dwClrUsed)
		 return (WORD) dwClrUsed;

   wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;

   switch (wBitCount)
			{
      case 1:
         return 2;

      case 4:
         return 16;

      case 8:
         return 256;

      default:
				 return 0;
      }
}

WORD PaletteSize (LPSTR lpbi)
{
   return (WORD)(DIBNumColors (lpbi) * sizeof (RGBQUAD));
}
LPSTR FindDIBBits (LPSTR lpbi)
{
   return (lpbi +(WORD)*(LPDWORD)lpbi + PaletteSize (lpbi));
}
void InitBitmapInfoHeader (
	LPBITMAPINFOHEADER lpBmInfoHdr,
  DWORD dwWidth,
  DWORD dwHeight,
  int nBPP)
{
   _fmemset (lpBmInfoHdr, 0, sizeof (BITMAPINFOHEADER));

   lpBmInfoHdr->biSize      = sizeof (BITMAPINFOHEADER);
   lpBmInfoHdr->biWidth     = dwWidth;
   lpBmInfoHdr->biHeight    = dwHeight;
   lpBmInfoHdr->biPlanes    = 1;

   if (nBPP <= 1)
      nBPP = 1;
   else if (nBPP <= 4)
      nBPP = 4;
   else if (nBPP <= 8)
      nBPP = 8;
   else
      nBPP = 24;

   lpBmInfoHdr->biBitCount  = (WORD)nBPP;
   lpBmInfoHdr->biSizeImage = WIDTHBYTES (dwWidth * nBPP) * dwHeight;
}
HANDLE BitmapToDIB (HBITMAP hBitmap, LPPOINT size) 
{
   BITMAP             Bitmap;
   BITMAPINFOHEADER   bmInfoHdr;
	 LPBITMAPINFOHEADER lpbmInfoHdr;
	 LPSTR              lpBits;
   HDC                hMemDC;
   HANDLE             hDIB;

   if (!hBitmap)
      return NULL;

   if (!GetObject (hBitmap, sizeof (Bitmap), (LPSTR) &Bitmap))
      return NULL;

   InitBitmapInfoHeader (&bmInfoHdr, 
                         Bitmap.bmWidth, 
                         Bitmap.bmHeight, 
                         Bitmap.bmPlanes * Bitmap.bmBitsPixel);


      // Now allocate memory for the DIB.  Then, set the BITMAPINFOHEADER
      //  into this memory, and find out where the bitmap bits go.

   hDIB = GlobalAlloc (GHND, sizeof (BITMAPINFOHEADER) + 
						 PaletteSize ((LPSTR) &bmInfoHdr) + bmInfoHdr.biSizeImage);

   if (!hDIB)
      return NULL;

   lpbmInfoHdr  = (LPBITMAPINFOHEADER) GlobalLock (hDIB);
   *lpbmInfoHdr = bmInfoHdr;
   lpBits       = FindDIBBits ((LPSTR) lpbmInfoHdr);

   hMemDC       = GetDC (NULL);

   if (!GetDIBits (hMemDC, 
                   hBitmap, 
                   0, 
                   Bitmap.bmHeight, 
									 lpBits,
									 (LPBITMAPINFO) lpbmInfoHdr,
                   DIB_RGB_COLORS))
   	{
    GlobalUnlock (hDIB);
    GlobalFree (hDIB);
    }
	 else
		{
		size->x = (int)lpbmInfoHdr->biWidth ;
		size->y = (int)lpbmInfoHdr->biHeight ;
		GlobalUnlock (hDIB);
    }

   ReleaseDC (NULL, hMemDC);

   return (hDIB) ;
}
//
//
//
HANDLE LoadBitmapFromClipboard(HWND hwnd, HANDLE hDIB, LPPOINT size)
{
  HBITMAP theBmp ;

	if (OpenClipboard (hwnd))
		{
		theBmp = (HBITMAP)GetClipboardData (CF_BITMAP) ;
		if (theBmp )
			{
			if (hDIB)
      	GlobalFree(hDIB) ;
			hDIB = (HANDLE) BitmapToDIB(theBmp, size) ;
			}
		CloseClipboard () ;
    }
  return (hDIB) ;
}
//
//
//
HANDLE LoadBitmapFromFile(HWND hwnd, HANDLE hDIB, LPPOINT size)
{
	char  fileName[256] ;
	BOOL   ok ; 

	ok = OpenDlg(hwnd, fileName) ;
	if (ok&&*fileName)
		{
		if (hDIB)
			GlobalFree(hDIB) ;
		hDIB =ReadDIB(fileName, size);
    }
  return (hDIB) ;	
}

//////////////////////////////////////////////////

static
UINT		ALMCALLBACK OpenFileHook(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	/*wParam*/,
	LPARAM	/*lParam*/		   
	)
	{
	UINT		nRes;

	switch (nMsg)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hDlg, 0);
			break;

		default:
			nRes = 0;
			break;
		}

	return nRes;
	}

BOOL OpenDlg (HWND hwnd, char *fileName)
{
	OPENFILENAME    ofn;

	FillMemory((LPVOID)&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = NULL;
	ofn.lpstrFilter = "Bitmaps (*.BMP)\0*.BMP\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	fileName[0] = 0;		/* pass in NULL */
	ofn.lpstrFile = (LPSTR)fileName;
	ofn.nMaxFile = 128;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrDefExt = NULL;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_ENABLEHOOK;
	ofn.lpfnHook = OpenFileHook;

	return(GetOpenFileName((LPOPENFILENAME)&ofn));

}
/****************************************************************************
 *									    *
 *  FUNCTION   : ReadDIB(char *)						    *
 *									    *
 *  PURPOSE    : Reads a DIB from a file, obtains a handle to it's          *
 *		 BITMAPINFO struct. and loads the DIB.                      *
 *									    *
 *  RETURNS    : TRUE  - DIB loads ok					    *
 *		 FALSE - otherwise					    *
 *									    *
 ****************************************************************************/
HANDLE ReadDIB (char* fileName, LPPOINT size)
{
	HANDLE             fh ; 
  HANDLE             hDIBInfo=NULL;
	LPBITMAPINFOHEADER lpbi;
	BITMAPFILEHEADER   bf;
	WORD		 nNumColors;
	char     str[255] ;
	WORD     offBits ;
	DWORD    bytes ;
	/* Open the file and get a handle to it's BITMAPINFO */

	//fh = CreateFile (fileName, &of, OF_READ);
	fh = CreateFile(
									fileName,
									GENERIC_READ, 
									0,
									NULL, 
									OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL,
									NULL);	
	if (fh==INVALID_HANDLE_VALUE)
		{
		//wsprintf(str,"Can't open file '%ls'", (LPSTR)achFileName);
		//MessageBox(hWnd, str, "Error", MB_ICONSTOP | MB_OK);
		return NULL;
		}
	hDIBInfo = GlobalAlloc(
								GMEM_MOVEABLE,
								(DWORD)(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)));
	if (!hDIBInfo)
		return NULL ;

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIBInfo);

	/* read the BITMAPFILEHEADER */
	ReadFile (fh, (LPSTR)&bf,sizeof(bf), &bytes, NULL) ; 
	if (sizeof (bf) != bytes)
		goto ErrExit;

	if (bf.bfType != 0x4d42)	/* 'BM' */
	{
		wsprintf(str,"This is not a bitmap file.\nCan't load this file");
		MessageBox(NULL, str, "Error", MB_ICONSTOP | MB_OK);
		goto ErrExit;
	}

	ReadFile (fh, (LPSTR)&lpbi, sizeof(BITMAPINFOHEADER), &bytes, NULL) ;
	if (sizeof(BITMAPINFOHEADER) != bytes)
		goto ErrExit;

	/* !!!!! for now, don't even deal with CORE headers */
	if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
		goto ErrExit;

	if (!(nNumColors = (WORD)lpbi->biClrUsed))
	{
	/* no color table for 24-bit, default size otherwise */
	if (lpbi->biBitCount != 24)
		nNumColors = 1 << lpbi->biBitCount;	/* standard size table */
	}

	/*	fill in some default values if they are zero */
	if (lpbi->biClrUsed == 0)
		lpbi->biClrUsed = (DWORD)nNumColors;

	if (lpbi->biSizeImage == 0)
	{
	lpbi->biSizeImage = ((((lpbi->biWidth * (DWORD)lpbi->biBitCount) + 31) & ~31) >> 3)
			 * lpbi->biHeight;
	}

	/* get a proper-sized buffer for header, color table and bits */
	GlobalUnlock(hDIBInfo);
	hDIBInfo = GlobalReAlloc(hDIBInfo, lpbi->biSize +
							nNumColors * sizeof(RGBQUAD) +
							lpbi->biSizeImage, 0);
	if (!hDIBInfo)	/* can't resize buffer for loading */
		return NULL ; //goto ErrExit2;

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIBInfo);

	/* read the color table */
	ReadFile (fh, (LPSTR)(lpbi)+(WORD)lpbi->biSize,(UINT)(nNumColors * sizeof(RGBQUAD)), NULL, NULL);

	/* offset to the bits from start of DIB header */
	offBits = (WORD)lpbi->biSize + nNumColors * sizeof(RGBQUAD);

	if (bf.bfOffBits != 0L)
		{
		SetFilePointer(fh,bf.bfOffBits,0,0);
		}
	if (lpbi->biSizeImage == lread(fh, (LPSTR)lpbi + offBits, lpbi->biSizeImage))
		{
		size->x = (int)lpbi->biWidth ;
		size->y = (int)lpbi->biHeight ;
		GlobalUnlock (hDIBInfo) ;
		CloseHandle(fh) ;
    return (hDIBInfo) ;
		}

ErrExit:
	CloseHandle(fh);
	GlobalUnlock(hDIBInfo);
	GlobalFree (hDIBInfo) ;
	return NULL ;

}

void PaintDIBitmap(HDC hDC, HANDLE hDIB, LPRECT lpRect) //HANDLE hDIB, LPRECT lpRect)
{
	HDC                hMemDC ;
	HBITMAP            hDDBitmap=NULL, hOldBitmap ;
	LPBITMAPINFOHEADER lpInfo ;
  int                offBits ;

	lpInfo = (LPBITMAPINFOHEADER) GlobalLock(hDIB);
	if (lpInfo)
  	{
		offBits = (WORD)lpInfo->biSize + PaletteSize((LPSTR)lpInfo);

		hMemDC = CreateCompatibleDC(hDC);
		hDDBitmap = CreateCompatibleBitmap(
									hDC, 
									(WORD)lpInfo->biWidth,
									(WORD)lpInfo->biHeight);
		SetDIBits(hDC,
							hDDBitmap,
							0,
							(WORD)lpInfo->biHeight, 
							(LPSTR)lpInfo + offBits, 
							(LPBITMAPINFO)lpInfo,
							DIB_RGB_COLORS);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, (HBITMAP)hDDBitmap);
		BitBlt (hDC,
						lpRect->left,
						lpRect->top, 
						lpRect->right-lpRect->left, //(WORD)lpInfo->biWidth,
						lpRect->bottom-lpRect->top, //(WORD)lpInfo->biHeight,
						hMemDC,
						0,
						0,
						SRCCOPY);
		SelectObject (hMemDC, hOldBitmap) ;
		DeleteObject (hDDBitmap) ;
		DeleteDC (hMemDC) ;
		GlobalUnlock (hDIB) ;
		}
	//return (hDDBitmap) ;
}
DWORD  lread (HANDLE fh, VOID  *pv, DWORD ul)
{
	DWORD     ulT = ul;
	BYTE     *hp = (BYTE*) pv;
	DWORD     bytes ;

	while (ul > MAXREAD) {
	ReadFile (fh, (LPSTR)hp, MAXREAD, &bytes, NULL) ;
	if (bytes != MAXREAD)
		return 0;
	ul -= MAXREAD;
	hp += MAXREAD;
	}
	ReadFile(fh, (LPSTR)hp, (WORD)ul, &bytes, NULL) ;
	if(bytes != (WORD)ul)
		return 0;
	return ulT;
}

HANDLE ObjectToDIBitmap (AOBJECTID oiObj, LPRECT lprect, int ScaleOrInd)
{
	LPBITMAPINFOHEADER lpInfo, lpTmp ;
	long size ;
  HANDLE hDIB = NULL ;

	SetRect (lprect, 0,0,0,0) ;
	lpInfo = (LPBITMAPINFOHEADER)AObjLockData (oiObj, ScaleOrInd) ;
	if (lpInfo)
		{
		size = AObjGetDataSize (oiObj, ScaleOrInd) ;
		hDIB = GlobalAlloc (GHND, size) ;
		if (hDIB)
			{
			lpTmp =(LPBITMAPINFOHEADER) GlobalLock (hDIB) ;
			if (lpTmp)
      	{
				CopyMemory(lpTmp, lpInfo, size) ;
				lprect->right = (int)lpInfo->biWidth ;
				lprect->bottom = (int)lpInfo->biHeight ;
        GlobalUnlock (hDIB) ;
				}
			else
				{
				GlobalUnlock (hDIB) ;
				GlobalFree (hDIB) ;
				hDIB = NULL ;
        }
			}
		AObjUnlockData (oiObj, ScaleOrInd) ;
		}

  return (hDIB) ;
}
void DIBitmapToObject (AOBJECTID oiObj, HANDLE hDIB, int ScaleOrInd)
{
	LPSTR lpInfo ;

	if (hDIB)
		{
		lpInfo = (LPSTR) GlobalLock (hDIB) ;
		if (lpInfo)
    	{
			AObjSetData(oiObj, ScaleOrInd, lpInfo, GlobalSize(hDIB)) ;
			GlobalUnlock (hDIB) ;
			}
		}				 	
}

BOOL CheckInBitmap (RECT rect, POINT mousepos, LPPOINT mouseInBitmap)
{
	BOOL rel = FALSE ;
	mouseInBitmap->x = 0 ;
	mouseInBitmap->y = 0 ;

	if (PtInRect(&rect, mousepos))
		{
		mouseInBitmap->x = mousepos.x - rect.left;
		mouseInBitmap->y = mousepos.y - rect.top;
    rel = TRUE ;
		}
	return rel ;
}

void DrawFrame(HWND hwnd, RECT frameRect, RECT oldframe)
	{
	LOGPEN   lp ; //= {PS_DOT, 1, 1, RGB(0,0,0)} ;
	LOGBRUSH lb ;
  HBRUSH 	 hBrush, hOldBrush ;
  HPEN     hPen, hOldPen ;
	HDC      hdc ;     

	hdc= GetDC(hwnd);
	SetROP2 (hdc, R2_XORPEN) ;
	lp.lopnStyle = PS_DOT;
	lp.lopnWidth.x = 1 ;
	lp.lopnWidth.y = 1 ;
	lp.lopnColor = RGB(0,0,0) ;
	hPen = CreatePenIndirect (&lp) ;
	hOldPen = (HPEN)SelectObject (hdc, (HPEN)hPen) ;
	lb.lbStyle = BS_HOLLOW ;
	lb.lbColor = 0 ;
  lb.lbHatch = 0 ;
  hBrush = CreateBrushIndirect (&lb) ;
	hOldBrush = (HBRUSH)SelectObject (hdc, (HBRUSH)hBrush) ;
	if (!IsRectEmpty(&oldframe))
		Rectangle (hdc, oldframe.left, oldframe.top, oldframe.right, oldframe.bottom) ;
	Rectangle (hdc, frameRect.left, frameRect.top, frameRect.right, frameRect.bottom) ;
	SelectObject (hdc, hOldPen) ;
  SelectObject (hdc, hOldBrush) ;
	DeleteObject (hPen) ;
	DeleteObject (hBrush) ;
	ReleaseDC(hwnd, hdc) ;                                                            
	}                          
void DrawIndText (
	HDC  hdc, //HWND hwnd,
	RECT sRect, // Rectangle of the scale
	RECT iRect, // rectangle of the indicator
	int status)
{
	//HDC  hdc ;
	char text[10] ;

	//write max or min text
	if (status == 0)
		lstrcpy (text, "max") ;
	else
		lstrcpy (text, "min") ;

	//hdc = GetDC (hwnd) ;
	SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT) ) ;
	SetBkMode (hdc, TRANSPARENT) ;
	SetTextColor (hdc, RGB(0,0,0)) ;

	if (iRect.bottom > sRect.bottom)
		TextOut (hdc, iRect.left, iRect.bottom, text, lstrlen (text)) ;
	else
		TextOut (hdc, iRect.left, iRect.top-15, text, lstrlen (text)) ;

	DeleteObject (GetStockObject (SYSTEM_FIXED_FONT)) ;
	//ReleaseDC (hwnd, hdc) ;
}

BOOL CorrectRect(LPRECT newRect, LPRECT layRect)
{
	BOOL rVel = FALSE ;

	OffsetRect(newRect, layRect->left, layRect->top) ;
	if (newRect->right>layRect->right)
		newRect->right = layRect->right ;
	if (newRect->bottom>layRect->bottom)
		newRect->bottom = layRect->bottom ;

	if (newRect->left<newRect->right&&newRect->top<newRect->bottom)
		rVel = TRUE ;

	return rVel ;
}
double long GetValue (AOBJECTID theObject)
{
	double long	theValue ;

	if (AObjCheckType (theObject, OTYPE_NUMBER))
		{
		if (ONmbrGetType(theObject) == ONMBR_INTEGER)
    	theValue = (double long)ONmbrGetInteger(theObject);
		else
			{
			ONmbrGetReal(theObject, &theValue);
      }
		}
	else
		theValue = 0.;

  return theValue ;
}
void SetValue (AOBJECTID numObject, long double theValue)
{
	long double			tmpValue ;

	if (AObjCheckType (numObject, OTYPE_NUMBER))
		{
		if (ONmbrGetType(numObject) == ONMBR_INTEGER)
			{
			if (ONmbrGetInteger(numObject) != theValue) 
				{
				ONmbrSetInteger(numObject, theValue, FALSE);
				//AEvtPostStandard (numObject, AEVENT_OBJECTCHANGED, 0L, AEVT_ATMARK) ;
				}
			}
		else
			{
			ONmbrGetReal(numObject, &tmpValue);
			if (tmpValue != theValue) // heNum->value.l != (long) theValue)
				{
				ONmbrSetReal(numObject, theValue, FALSE);
				}
      }
		AEvtPostStandard (numObject, AEVENT_OBJECTCHANGED, 0L, AEVT_ATTAIL) ;
    }
}

HBITMAP CreateInvertBitmap(HANDLE hDIB, HBITMAP hMaskBitmap)
{
	HBITMAP hInvertBitmap, oldBitmap1, oldBitmap2 ;
  BITMAP  bm ;
	HDC hdcSrc = CreateCompatibleDC (NULL) ;
	HDC hdcDes = CreateCompatibleDC (NULL) ;
	HBITMAP hBitmap ;
	LPBITMAPINFOHEADER lpInfo ;

	HDC hdc = CreateDC ("DISPLAY", NULL, NULL, NULL) ;
	lpInfo = (LPBITMAPINFOHEADER) GlobalLock(hDIB);
	if (lpInfo)
  	{
		int offBits = (WORD)lpInfo->biSize + PaletteSize((LPSTR)lpInfo);
		hBitmap = CreateDIBitmap (hdc, //hDC,
		 													lpInfo,
		 													CBM_INIT,
		 													(LPSTR)lpInfo + offBits,
		 													(LPBITMAPINFO)lpInfo,
		 													DIB_RGB_COLORS) ;
  	GlobalUnlock (hDIB) ;
		}

	GetObject(hMaskBitmap, sizeof(BITMAP), (LPSTR) &bm) ;
	hInvertBitmap= CreateCompatibleBitmap(
										hdc,
										bm.bmWidth,    //indicator size
										bm.bmHeight) ; //..

	oldBitmap1 = (HBITMAP)SelectObject (hdcDes, (HBITMAP)hInvertBitmap);
	oldBitmap2 = (HBITMAP)SelectObject (hdcSrc, (HBITMAP)hMaskBitmap) ;
	BitBlt (hdcDes,
          0,
          0,
          bm.bmWidth,
          bm.bmHeight,
          hdcSrc,
					0,
					0,
					WHITENESS );
  BitBlt( hdcDes,
          0,
          0,
          bm.bmWidth,
          bm.bmHeight,
					hdcSrc,
					0,
					0,
					SRCINVERT );

	oldBitmap2 = (HBITMAP)SelectObject(hdcSrc, (HBITMAP)hBitmap);
	BitBlt (hdcDes,
					0,
          0,
					(WORD)bm.bmWidth,
					(WORD)bm.bmHeight,
					hdcSrc,
					0,
					0,
					SRCAND);
  SelectObject (hdcDes, oldBitmap1) ;
	SelectObject (hdcSrc, oldBitmap2) ;
	DeleteDC (hdcDes) ;
	DeleteDC (hdcSrc) ;
	DeleteDC (hdc) ;

	DeleteObject (hBitmap) ;

	return (hInvertBitmap) ;
}
void DoTransparent (HDC hdc, HBITMAP hMaskBitmap, HBITMAP hInvertBitmap, LPRECT lpRect)
{
	HBITMAP oldBitmap ;

	HDC hdcMem = CreateCompatibleDC( hdc) ;

  oldBitmap = (HBITMAP)SelectObject (hdcMem, (HBITMAP)hMaskBitmap) ;
	BitBlt(hdc,
				 lpRect->left,
				 lpRect->top,
				 lpRect->right-lpRect->left,
				 lpRect->bottom-lpRect->top,
         hdcMem, 0,0, SRCAND) ;

  SelectObject (hdcMem, hInvertBitmap) ;
	BitBlt(hdc,
				 lpRect->left,
				 lpRect->top,
				 lpRect->right-lpRect->left,
				 lpRect->bottom-lpRect->top,
				 hdcMem, 0,0, SRCINVERT) ;

	SelectObject (hdcMem, oldBitmap) ;
	DeleteDC (hdcMem) ;
}
