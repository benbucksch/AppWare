////////////////////////////////////////////////////////////////////////////
//	UPSF Native To Universal Translator functions
//		"nat2univ.cpp"
//
//	(c) Serius Corporation, 1989-1992
//
//	Kevin J. Smart
//		start:		10/19/92
//		finish:		10/xx/92
//
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "almsys.h"
#include <windowsx.h>

#include	"upsf_cls.h"

// NativeToUniv translator /////////////////////////////////////////////////

long	UPSF::NativeToUniv(
	long			native,
	LPCVOID		nativePtr,
	long			nativeLen,
	long			univ,
	HGLOBAL&	hUniv,
	long&			univSize)

	{
	// check for invalid NULL pointers
	if(!nativePtr && nativeLen)
		return AUPSF_MEMORYERROR;

	if(nativeLen < 0L)
  	return AUPSF_SIZEERROR;

	if(univ==native)
		return Nxxx_To_Uxxx(hUniv, univSize, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_NUMBER)
		return xxxx_To_UNum(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_INTEGER)
		return xxxx_To_UInt(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_TEXT)
		return xxxx_To_UTxt(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_BITMAP)
		return xxxx_To_UBit(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_OBJECTREF)
		return xxxx_To_URef(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_COLOR)
		return xxxx_To_UClr(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_POINT)
		return xxxx_To_UPnt(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_RECT)
		return xxxx_To_URct(hUniv, univSize, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_FONT)
		return xxxx_To_UFon(hUniv, univSize, native, nativePtr, nativeLen);
	else
		return AUPSF_CANTCONVERT;
	}

// xxxx_To_xxxx translation function ///////////////////////////////////////

long	UPSF::Nxxx_To_Uxxx(
	HGLOBAL&	hUniv,
	long&			univSize,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	univSize = natvLen;
	if((hUniv = GlobalReAlloc(hUniv, natvLen, MEMFLAGS))==NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, natvPtr, natvLen);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// xxxx_To_UNum translation function ///////////////////////////////////////

long	UPSF::xxxx_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	if(natv==AUPSF_NATV_BYTE)
		{
		if(natvLen != sizeof(BYTE))
			return AUPSF_SIZEERROR;
		else
			return Byte_To_UNum(hUniv, univSize, *(BYTE*)natvPtr);
		}
	else if(natv==AUPSF_NATV_CHAR)
		{
		if(natvLen != sizeof(char))
			return AUPSF_SIZEERROR;
		else
			return Char_To_UNum(hUniv, univSize, *(char*)natvPtr);
		}
	else if(natv==AUPSF_NATV_WORD)
		{
		if(natvLen != sizeof(WORD))
			return AUPSF_SIZEERROR;
		else
			return Word_To_UNum(hUniv, univSize, *(WORD*)natvPtr);
		}
	else if(natv==AUPSF_NATV_SHORT)
		{
		if(natvLen != sizeof(short))
			return AUPSF_SIZEERROR;
		else
			return Short_To_UNum(hUniv, univSize, *(short*)natvPtr);
		}
	else if(natv==AUPSF_NATV_DWORD)
		{
		if(natvLen != sizeof(DWORD))
			return AUPSF_SIZEERROR;
		else
			return DWord_To_UNum(hUniv, univSize, *(DWORD*)natvPtr);
		}
	else if(natv==AUPSF_NATV_LONG)
		{
		if(natvLen != sizeof(long))
			return AUPSF_SIZEERROR;
		else
			return Long_To_UNum(hUniv, univSize, *(long*)natvPtr);
		}
	else if(natv==AUPSF_NATV_FLOAT)
		{
		if(natvLen != sizeof(float))
			return AUPSF_SIZEERROR;
		else
			return Float_To_UNum(hUniv, univSize, *(float*)natvPtr);
		}
	else if(natv==AUPSF_NATV_DOUBLE)
		{
		if(natvLen != sizeof(double))
			return AUPSF_SIZEERROR;
		else
			return Double_To_UNum(hUniv, univSize, *(double*)natvPtr);
		}
	else if(natv==AUPSF_NATV_LONGDOUBLE)
		{
		if(natvLen != sizeof(long double))
			return AUPSF_SIZEERROR;
		else
			return LDouble_To_UNum(hUniv, univSize, *(long double*)natvPtr);
		}
	else
		return AUPSF_CANTCONVERT;
	}

// Byte_To_UNum translation function ///////////////////////////////////////

long	UPSF::Byte_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	BYTE			byte)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%hu", (WORD)byte);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// Char_To_UNum translation function ///////////////////////////////////////

long	UPSF::Char_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	char			Char)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%hi", (int)Char);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// Word_To_UNum translation function ///////////////////////////////////////

long	UPSF::Word_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	WORD			word)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%hu", word);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// Short_To_UNum translation function //////////////////////////////////////

long	UPSF::Short_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	short			Short)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%hi", Short);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// DWord_To_UNum translation function //////////////////////////////////////

long	UPSF::DWord_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	DWORD			dword)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%lu", dword);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// Long_To_UNum translation function ///////////////////////////////////////

long	UPSF::Long_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			Long)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%li", Long);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// Float_To_UNum translation function //////////////////////////////////////

long	UPSF::Float_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	float			Float)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%.10g", Float);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
  GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// Double_To_UNum translation function /////////////////////////////////////

long	UPSF::Double_To_UNum(
	HGLOBAL&	hUniv,
	long&			univSize,
	double		Double)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%.18lg", Double);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// LDouble_To_UNum translation function ////////////////////////////////////

long	UPSF::LDouble_To_UNum(
	HGLOBAL&		hUniv,
	long&				univSize,
	long double	LDouble)

	{
	char	szBuffer[80];

	sprintf(szBuffer, "%.20Lg", LDouble);
	univSize = lstrlen(szBuffer);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, szBuffer, univSize);
  GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// xxxx_To_UInt translation function ///////////////////////////////////////

long	UPSF::xxxx_To_UInt(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	univSize = sizeof(DWORD);
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	DWORD*	univPtr = (DWORD*)GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	long	retVal = AUPSF_NOERROR;

	if(natv==AUPSF_NATV_BYTE)
		{
		if(natvLen != sizeof(BYTE))
			retVal = AUPSF_SIZEERROR;
		else
			*univPtr = (DWORD)(*(BYTE*)natvPtr);
		}
	else if(natv==AUPSF_NATV_CHAR)
		{
		if(natvLen != sizeof(char))
			retVal = AUPSF_SIZEERROR;
		else
			*univPtr = (DWORD)(*(char*)natvPtr);
		}
	else if(natv==AUPSF_NATV_WORD)
		{
		if(natvLen != sizeof(WORD))
			retVal = AUPSF_SIZEERROR;
		else
			*univPtr = (DWORD)(*(WORD*)natvPtr);
		}
	else if(natv==AUPSF_NATV_SHORT)
		{
		if(natvLen != sizeof(short))
			retVal = AUPSF_SIZEERROR;
		else
			*univPtr = (DWORD)(*(short*)natvPtr);
		}
	else if(natv==AUPSF_NATV_DWORD)
		{
		if(natvLen != sizeof(DWORD))
			retVal = AUPSF_SIZEERROR;
		else
			*univPtr = (DWORD)(*(DWORD*)natvPtr);
		}
	else if(natv==AUPSF_NATV_LONG)
		{
		if(natvLen != sizeof(long))
			retVal = AUPSF_SIZEERROR;
		else
			*univPtr = (DWORD)(*(long*)natvPtr);
		}
	else if(natv==AUPSF_NATV_BOOL)
		{
		if(natvLen != sizeof(BOOL))
			retVal = AUPSF_SIZEERROR;
		else
    	*univPtr = (DWORD)(*(BOOL*)natvPtr);
		}
	else
		retVal = AUPSF_CANTCONVERT;

  GlobalUnlock(hUniv);
	return retVal;
	}

// xxxx_To_UTxt translation function ///////////////////////////////////////

long	UPSF::xxxx_To_UTxt(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	long	retValue;

	if(natv==AUPSF_NATV_STRING)
		retValue = String_To_UTxt(hUniv, univSize, (char *)natvPtr, natvLen);
	else
		retValue = AUPSF_CANTCONVERT;

	if(retValue >= 0L)
		return AUPSF_NOERROR;
	else
  	return retValue;
	}

// String_To_UTxt translation function /////////////////////////////////////

long	UPSF::String_To_UTxt(
	HGLOBAL&		hUniv,
	long&				univSize,
	char *			string,
	long				length)

	{
	// replace cr-lf with cr only
	long				count = 0L;
	long				charNum = 0L;
  char *			natvStr = string;

  // count the number of cr-lf combinations
	while(charNum<length)
		{
		if(*natvStr==0x0D && natvStr[1]==0x0A)
			{
			count++;
      natvStr++;
			charNum++;
			}
    natvStr++;
		charNum++;
		}

	// set the universal buffer size
	univSize = length - count;
  if(univSize > 0L)
		if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
			return AUPSF_MEMORYERROR;

	natvStr = string;
	BYTE *	univStr = NULL;
	if(univSize > 0L)
		{
		univStr = (BYTE *)GlobalLock(hUniv);
		if(!univStr)
			return AUPSF_MEMORYERROR;
    }

	charNum = 0L;
	count = 0L;
  // copy the string to the universal buffer changing cr-lf to cr
	while(charNum < length)
		{
		if(*natvStr==0x0D && natvStr[1]==0x0A)
			{
			*univStr = 0x0D;
      natvStr++;
			charNum++;
			count++;
			}
		else
			*univStr = *natvStr;
		natvStr++;
    univStr++;
		charNum++;
		} // end...while

  if(univSize > 0L)
		GlobalUnlock(hUniv);
	return (length-count);
	}

// xxxx_To_UBit translation function ///////////////////////////////////////

long	UPSF::xxxx_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	if(natv==AUPSF_NATV_BITMAP)
		return Bitmap_To_UBit(hUniv, univSize, natvPtr, natvLen);
	else if(natv==AUPSF_NATV_BITMAPINFO)
		return BitmapInfo_To_UBit(hUniv, univSize, natvPtr, natvLen);
	else if(natv==AUPSF_NATV_BITMAPCORE)
		return BitmapCore_To_UBit(hUniv, univSize, natvPtr, natvLen);
	else if(natv==AUPSF_NATV_HBITMAP)
		return hBitmap_To_UBit(hUniv, univSize, (HBITMAP)(*(HBITMAP*)natvPtr), natvLen);
	else if(natv==AUPSF_NATV_HBRUSH)
		return hBrush_To_UBit(hUniv, univSize, (HBRUSH)(*(HBRUSH*)natvPtr), natvLen);
	else if(natv==AUPSF_NATV_METAFILE)
		return hMetaFile_To_UBit(hUniv, univSize, (HMETAFILE)(*(HMETAFILE*)natvPtr), natvLen);
	else
		return AUPSF_CANTCONVERT;
	}

// BITMAP_To_UBit translation function /////////////////////////////////////

long	UPSF::BITMAP_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	BITMAP		bitmap,
	HBITMAP		hBmp)

	{
	WORD	nColorBits = (WORD)(bitmap.bmBitsPixel*bitmap.bmPlanes);
	if(nColorBits!=1 && nColorBits!=4 && nColorBits!=8 && nColorBits!=24)
		return AUPSF_CANTCONVERT;

	long	ColorTableSize;
	if(nColorBits==24)
		{
		// 24-bit color -- no color table
		ColorTableSize = 0;
		}
	else
		ColorTableSize = pow2(nColorBits);

	univSize = sizeof(BITMAPINFOHEADER) + ColorTableSize*sizeof(RGBQUAD);
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	BITMAPINFOHEADER* bmpIH = (BITMAPINFOHEADER*)GlobalLock(hUniv);
	if(!bmpIH)
  	return AUPSF_MEMORYERROR;

	bmpIH->biWidth					= bitmap.bmWidth;
	bmpIH->biHeight					= bitmap.bmHeight;
	bmpIH->biPlanes					= 1;
	bmpIH->biSize						= sizeof(BITMAPINFOHEADER);
	bmpIH->biBitCount				= nColorBits;
	bmpIH->biCompression		= BI_RGB;

	bmpIH->biXPelsPerMeter	= 0;
	bmpIH->biYPelsPerMeter	= 0;

	bmpIH->biSizeImage			= 0;
	bmpIH->biClrUsed				= 0;
	bmpIH->biClrImportant		= 0;

  // get some handles for bitmap conversion
	HDC			hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	HBITMAP hBitmap;
  if(!hBmp)
		hBitmap = CreateBitmapIndirect(&bitmap);
	else
  	hBitmap = hBmp;

	// fill in the BITMAPINFO structure -- fills in the color table
	GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, NULL, (LPBITMAPINFO)bmpIH, DIB_RGB_COLORS);

  // check to see if the video driver messed up!
	if(bmpIH->biSizeImage==0)
		bmpIH->biSizeImage = (((DWORD)bitmap.bmWidth * nColorBits + 31)/8) * bitmap.bmHeight;

	long	BitmapSize = bmpIH->biSizeImage;
  GlobalUnlock(hUniv);

	// resize the universal memory without losing information
  univSize += BitmapSize;
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;
	bmpIH = (BITMAPINFOHEADER*)GlobalLock(hUniv);
	if(!bmpIH)
  	return AUPSF_MEMORYERROR;

	void*	bits = (void*)((BYTE*)bmpIH + (size_t)(ColorTableSize*sizeof(RGBQUAD) + sizeof(BITMAPINFOHEADER)));

	 // fill in the bitmap data -- copies the bits automatically
	GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, (char*)bits, (LPBITMAPINFO)bmpIH, DIB_RGB_COLORS);

  // free the resources
  if(!hBmp)
	  DeleteObject(hBitmap);
	DeleteDC(hDC);

	GlobalUnlock(hUniv);
  return AUPSF_NOERROR;
	}

// Bitmap_To_UBit translation function /////////////////////////////////////

long	UPSF::Bitmap_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	LPCVOID		natvPtr,
	long			/*natvLen*/)

	{
	BITMAP	bitmap = *(BITMAP*)natvPtr;
	bitmap.bmBits = (BYTE*)natvPtr + sizeof(BITMAP);

	return BITMAP_To_UBit(hUniv, univSize, bitmap);
	}

// BitmapInfo_To_UBit translation function /////////////////////////////////

long	UPSF::BitmapInfo_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	// this windows bitmap format is the UPSF bitmap format
  univSize = natvLen;
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	CopyMemory(univPtr, natvPtr, natvLen);

	GlobalUnlock(hUniv);
  return AUPSF_NOERROR;
	}

// BitmapCore_To_UBit translation function /////////////////////////////////

long	UPSF::BitmapCore_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	BITMAPCOREHEADER*	bmpCH = (BITMAPCOREHEADER*)natvPtr;
	WORD	bitCount = bmpCH->bcBitCount;

	if(!(bitCount==1 || bitCount==4 || bitCount==8 || bitCount==24))
		return AUPSF_CANTCONVERT;

	long	ColorTableSize;
  long	BitmapSize;
	if(bitCount==24)
		{
		// 24-bit color -- no color table
    ColorTableSize = 0;
		}
	else
		ColorTableSize = pow2(bitCount);

	BitmapSize = natvLen - (long)sizeof(BITMAPCOREHEADER) - ColorTableSize*sizeof(RGBTRIPLE);
	univSize = BitmapSize + ColorTableSize*sizeof(RGBQUAD) + (long)sizeof(BITMAPINFOHEADER);

	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;
	void*	univPtr = GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

  // store the bitmap info header
	BITMAPINFOHEADER*	bmpIH = (BITMAPINFOHEADER*)univPtr;
	bmpIH->biSize						= sizeof(BITMAPINFOHEADER);
	bmpIH->biWidth					= bmpCH->bcWidth;
	bmpIH->biHeight					= bmpCH->bcHeight;
	bmpIH->biPlanes					= bmpCH->bcPlanes;
	bmpIH->biBitCount				= bmpCH->bcBitCount;
	bmpIH->biCompression		= BI_RGB;

	bmpIH->biXPelsPerMeter	= 0;
	bmpIH->biYPelsPerMeter	= 0;

	bmpIH->biSizeImage			= 0;
	bmpIH->biClrUsed				= 0;
	bmpIH->biClrImportant		= 0;

  // store the color table
	RGBTRIPLE*	rgbTriple = (RGBTRIPLE*)((BYTE*)natvPtr + sizeof(BITMAPCOREHEADER));
	RGBQUAD*		rgbQuad = (RGBQUAD*)((BYTE*)univPtr + sizeof(BITMAPINFOHEADER));
	for(int i=0; i<ColorTableSize; i++)
		{
		rgbQuad[i].rgbBlue = rgbTriple[i].rgbtBlue;
		rgbQuad[i].rgbGreen = rgbTriple[i].rgbtGreen;
		rgbQuad[i].rgbRed = rgbTriple[i].rgbtRed;
		rgbQuad[i].rgbReserved = 0; 
		}

	// store the bitmap
	void*	UnivBitmapBits = (void*)((BYTE*)univPtr + (size_t)(sizeof(BITMAPINFOHEADER) + ColorTableSize*sizeof(RGBQUAD)));
	void*	NatvBitmapBits = (void*)((BYTE*)natvPtr + (size_t)(sizeof(BITMAPCOREHEADER) + ColorTableSize*sizeof(RGBTRIPLE)));
	CopyMemory(UnivBitmapBits, NatvBitmapBits, BitmapSize);

	GlobalUnlock(hUniv);
	return AUPSF_NOERROR;
	}

// hBitmap_To_UBit translation function ////////////////////////////////////

long	UPSF::hBitmap_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	HBITMAP		hBitmap,
	long			natvLen)

	{
	if(natvLen != (long)sizeof(HBITMAP))
		return AUPSF_SIZEERROR;

	BITMAP	bitmap;
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);

  long		bitmapSize = (long)bitmap.bmHeight * (long)bitmap.bmWidthBytes * (long)bitmap.bmPlanes;
	HGLOBAL	hBits = GlobalAlloc(MEMFLAGS, bitmapSize);
	if(!hBits)
		return AUPSF_MEMORYERROR;

	bitmap.bmBits = GlobalLock(hBits);
	if(!bitmap.bmBits)
		return AUPSF_MEMORYERROR;

	long ret = BITMAP_To_UBit(hUniv, univSize, bitmap, hBitmap);

  // clean up and exit
	GlobalUnlock(hBits);
	GlobalFree(hBits);  
  return ret;
	}

// hBrush_To_UBit translation function /////////////////////////////////////

long	UPSF::hBrush_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	HBRUSH		hBrush,
	long			natvLen)

	{
	if(natvLen != (long)sizeof(HBRUSH))
		return AUPSF_SIZEERROR;

	HDC	hDC = CreateIC("DISPLAY", NULL, NULL, NULL);
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateBitmap(BRUSH_WIDTH, BRUSH_HEIGHT, 1, 1, NULL);
	SelectObject(hDCMem, hBitmap);
	HBRUSH	hBrushOld = SelectBrush(hDCMem, hBrush);

  // draw the brush in the region
	RECT	rect = {0, 0, BRUSH_WIDTH, BRUSH_HEIGHT};
  FillRect(hDCMem, &rect, hBrush);

	SelectBrush(hDCMem, hBrushOld);
	DeleteDC(hDCMem);
	DeleteDC(hDC);

	// we are left with the bitmap with the brush pattern
	BITMAP	bitmap;
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);

  long		bitmapSize = (long)bitmap.bmHeight * (long)bitmap.bmWidthBytes * (long)bitmap.bmPlanes;
	HGLOBAL	hBits = GlobalAlloc(MEMFLAGS, bitmapSize);
	if(!hBits)
		{
  	DeleteObject(hBitmap);
		return AUPSF_MEMORYERROR;
    }

	bitmap.bmBits = GlobalLock(hBits);
	if(!bitmap.bmBits)
		{
  	DeleteObject(hBitmap);
		return AUPSF_MEMORYERROR;
    }

	long ret = BITMAP_To_UBit(hUniv, univSize, bitmap, hBitmap);

	// clean up and exit
  DeleteObject(hBitmap);
	GlobalUnlock(hBits);
	GlobalFree(hBits);  
	return ret;
	}

// hMetaFile_To_UBit translation function //////////////////////////////////

long	UPSF::hMetaFile_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	HMETAFILE	hMetaFile,
	long			natvLen)

	{
	if(natvLen != (long)sizeof(HMETAFILE))
		return AUPSF_SIZEERROR;

	HDC	hDC = CreateIC("DISPLAY", NULL, NULL, NULL);
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES));
	SelectBitmap(hDCMem, hBitmap);

	// play the metafile
  PlayMetaFile(hDCMem, hMetaFile);

	DeleteDC(hDCMem);
	DeleteDC(hDC);

	// we are left with the bitmap with the meta file
	BITMAP	bitmap;
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);

  long		bitmapSize = (long)bitmap.bmHeight * (long)bitmap.bmWidthBytes * (long)bitmap.bmPlanes;
	HGLOBAL	hBits = GlobalAlloc(MEMFLAGS, bitmapSize);
	if(!hBits)
		{
  	DeleteObject(hBitmap);
		return AUPSF_MEMORYERROR;
    }

	bitmap.bmBits = GlobalLock(hBits);
	if(!bitmap.bmBits)
		{
  	DeleteObject(hBitmap);
		return AUPSF_MEMORYERROR;
    }

	long ret = BITMAP_To_UBit(hUniv, univSize, bitmap, hBitmap);

	// clean up and exit
  DeleteObject(hBitmap);
	GlobalUnlock(hBits);
	GlobalFree(hBits);  
	return ret;
	}

// xxxx_To_URef translation function ///////////////////////////////////////

long	UPSF::xxxx_To_URef(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	univSize = sizeof(long);
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

  long retValue = AUPSF_NOERROR;

	long*	univPtr = (long*)GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	if(natv==NATV_OBJECTID)
		{
		if(natvLen != sizeof(long))
			retValue = AUPSF_SIZEERROR;
		else
			*univPtr = *(long*)natvPtr;
		}
	else if(natv==AUPSF_NATV_OBJECTREF)
		{
		if(natvLen != sizeof(AOBJECTID))
			retValue = AUPSF_SIZEERROR;
		else
    	*univPtr = *(AOBJECTID*)natvPtr;
		}
	else
		retValue = AUPSF_CANTCONVERT;

	GlobalUnlock(hUniv);
	return retValue;
	}

// xxxx_To_UClr translation function ///////////////////////////////////////

long	UPSF::xxxx_To_UClr(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	univSize = sizeof(short)*3L;
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	short*	univPtr = (short*)GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

  long	retValue;

	if(natv==AUPSF_NATV_RGBCOLOR)
		{
		if(natvLen != sizeof(COLORREF))
			retValue = AUPSF_SIZEERROR;
    else
			retValue = RGBColor_To_UClr(univPtr, *(COLORREF*)natvPtr);
    }
	else
		retValue = AUPSF_CANTCONVERT;

	GlobalUnlock(hUniv);
	return retValue;
	}
	
// RGBColor_To_UClr translation function ///////////////////////////////////

long	UPSF::RGBColor_To_UClr(
	short*		univPtr,
	COLORREF	rgbColor)

	{
	// put the red in the upper byte of univPtr[0]
	univPtr[0] = (short) (GetRValue(rgbColor) << 8);
	// put the green in the upper byte univPtr[1]
	univPtr[1] = (short) (GetGValue(rgbColor) << 8);
	// put the blue in the upper byte univPtr[2]
	univPtr[2] = (short) (GetBValue(rgbColor) << 8);

  return AUPSF_NOERROR;
	}

// xxxx_To_UPnt translation function ///////////////////////////////////////

long	UPSF::xxxx_To_UPnt(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	univSize = sizeof(long)*2L;
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	long*	univPtr = (long*)GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

  long	retValue;

	if(natv==AUPSF_NATV_POINT)
		{
		if(natvLen != sizeof(POINT))
			retValue = AUPSF_SIZEERROR;
    else
			retValue = Point_To_UPnt(univPtr, *(POINT*)natvPtr);
    }
	else
		retValue = AUPSF_CANTCONVERT;

	GlobalUnlock(hUniv);
  return retValue;
	}

// Point_To_UPnt translation function //////////////////////////////////////

long	UPSF::Point_To_UPnt(
	long*	univPtr,
	POINT	point)

	{
	// store point.x in univPtr[0]
	univPtr[0] = (long)point.x;
	// store point.y in univPtr[1]
	univPtr[1] = (long)point.y;

	return AUPSF_NOERROR; 
	}

// xxxx_To_URct translation function ///////////////////////////////////////

long	UPSF::xxxx_To_URct(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	univSize = sizeof(long)*4L;
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	long*	univPtr = (long*)GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

  long	retValue;

	if(natv==AUPSF_NATV_RECT)
		{
		if(natvLen != sizeof(RECT))
			retValue = AUPSF_SIZEERROR;
    else
			retValue = Rect_To_URct(univPtr, *(RECT*)natvPtr);
    }
	else
		retValue = AUPSF_CANTCONVERT;

	GlobalUnlock(hUniv);
  return retValue;
	}

// Rect_To_URct translation function ///////////////////////////////////////

long	UPSF::Rect_To_URct(
	long*	univPtr,
	RECT	rect)

	{
	// store rect.left in univPtr[0]
	univPtr[0] = (long)rect.left;
	// store rect.top in univPtr[1]
	univPtr[1] = (long)rect.top;
	// store rect.right in univPtr[2]
	univPtr[2] = (long)rect.right;
	// store rect.bottom in univPtr[3]
	univPtr[3] = (long)rect.bottom;

	return AUPSF_NOERROR; 
	}

// xxxx_To_UFon translation function ///////////////////////////////////////

long	UPSF::xxxx_To_UFon(
	HGLOBAL&	hUniv,
	long&			univSize,
	long			natv,
	LPCVOID		natvPtr,
	long			natvLen)

	{
	univSize = natvLen;
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == NULL)
		return AUPSF_MEMORYERROR;

	long*	univPtr = (long*)GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	long	retValue = AUPSF_NOERROR;

	if(natv==AUPSF_NATV_FONT)
		{
		// just store the font name--each platform will convert on import
		// the attributes will be stored separately
		CopyMemory(univPtr, natvPtr, natvLen);
		}
	else
		retValue = AUPSF_CANTCONVERT;

	GlobalUnlock(hUniv);
	return retValue;
	}

////////////////////////////////////////////////////////////////////////////
