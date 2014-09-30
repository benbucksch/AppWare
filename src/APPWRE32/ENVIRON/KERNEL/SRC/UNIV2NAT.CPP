////////////////////////////////////////////////////////////////////////////
//	UPSF Translator functions
//		"univ2nat.cpp"
//
//	(c) Serius Corporation, 1989-1992
//
//	Kevin J. Smart
//		start:		10/16/92
//		finish:		10/xx/92
//
////////////////////////////////////////////////////////////////////////////

////////////
// 8/9/95  Changed lstrcpyn to strncpy.  lstrcpyn is behaving differently than expected.
////////////

#include	<stdlib.h>
#include	"almsys.h"
#include	"upsf_cls.h"
#include  "a_almutl.h"
// max inline function /////////////////////////////////////////////////////

#ifdef max
#undef max
#endif
inline size_t	max(size_t arg1, size_t arg2)

{
	return (arg1>arg2) ? arg1 : arg2;
}

// min inline function /////////////////////////////////////////////////////

#ifdef min
#undef min
#endif
inline size_t	min(long arg1, size_t arg2)

{
	return ((size_t)arg1<arg2) ? (size_t)arg1 : arg2;
}

// UnivToNative translator /////////////////////////////////////////////////

long	UPSF::UnivToNative(long univ,		void* univPtr,		long univLen,
												 long native,	void* nativePtr,	long nativeLen)

{
  if(nativeLen<0L)
    return AUPSF_SIZEERROR;

	if(univ==native)
		return Uxxx_To_Nxxx(univPtr, univLen, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_NUMBER)
		return UNum_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_INTEGER)
		return UInt_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_TEXT)
		return UTxt_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_BITMAP)
		return UBit_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_OBJECTREF)
		return URef_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_COLOR)
		return UClr_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_POINT)
  	return UPnt_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_RECT)
  	return URct_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else if(univ==AUPSF_UNIV_FONT)
  	return UFon_To_xxxx(univPtr, univLen, native, nativePtr, nativeLen);
	else
		return AUPSF_CANTCONVERT;
}

// xxxx_To_xxxx translation function ///////////////////////////////////////

long	UPSF::Uxxx_To_Nxxx	(void* univPtr,	long univLen,	void* natvPtr, long natvLen)

{
	if(univLen!=natvLen)
		return AUPSF_SIZEERROR;
	else {
		CopyMemory(natvPtr, univPtr, univLen);
		return AUPSF_NOERROR;
    }
}

// UNum_To_xxxx translation function ///////////////////////////////////////

long	UPSF::UNum_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
  *((char*)univPtr + univLen) = '\0';

	if(natv == AUPSF_NATV_BYTE) {  	
		if(natvLen!=sizeof(BYTE))
			return AUPSF_SIZEERROR;
		else
			*(BYTE*)natvPtr = UNum_To_Byte(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_CHAR) {
		if(natvLen!=sizeof(char))
			return AUPSF_SIZEERROR;
		else
			*(char*)natvPtr = UNum_To_Char(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_WORD) {
		if(natvLen!=sizeof(WORD))
			return AUPSF_SIZEERROR;
		else
			*(WORD*)natvPtr = UNum_To_Word(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_SHORT) {
		if(natvLen!=sizeof(short))
			return AUPSF_SIZEERROR;
		else
			*(short*)natvPtr = UNum_To_Short(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_DWORD) {
		if(natvLen!=sizeof(DWORD))
			return AUPSF_SIZEERROR;
		else
			*(DWORD*)natvPtr = UNum_To_DWord(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_LONG) {
		if(natvLen!=sizeof(LONG))
			return AUPSF_SIZEERROR;
		else
			*(long*)natvPtr = UNum_To_Long(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_FLOAT) {
		if(natvLen!=sizeof(float))
			return AUPSF_SIZEERROR;
		else
			*(float*)natvPtr = UNum_To_Float(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_DOUBLE) {
		if(natvLen!=sizeof(double))
			return AUPSF_SIZEERROR;
		else
			*(double*)natvPtr = UNum_To_Double(univPtr, univLen);
		}
	else if(natv == AUPSF_NATV_LONGDOUBLE) {
		if(natvLen!=sizeof(long double))
			return AUPSF_SIZEERROR;
		else
			*(long double*)natvPtr = UNum_To_LDouble(univPtr, univLen);
		}
	else
		return AUPSF_CANTCONVERT;

	return AUPSF_NOERROR;
}

// UNum_To_Byte translation function ///////////////////////////////////////

BYTE	UPSF::UNum_To_Byte	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (BYTE)strtoul(szBuffer, NULL, 0);
}

// UNum_To_Char translation function ///////////////////////////////////////

char	UPSF::UNum_To_Char	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (char)strtol(szBuffer, NULL, 0);
}

// UNum_To_Word translation function ///////////////////////////////////////

WORD	UPSF::UNum_To_Word	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (WORD)strtoul(szBuffer, NULL, 0);
}

// UNum_To_Short translation function //////////////////////////////////////

short	UPSF::UNum_To_Short	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (short)strtol(szBuffer, NULL, 0);
}

// UNum_To_DWord translation function //////////////////////////////////////

DWORD	UPSF::UNum_To_DWord	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (DWORD)strtoul(szBuffer, NULL, 0);
}

// UNum_To_Long translation function ///////////////////////////////////////

long	UPSF::UNum_To_Long	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (long)strtol(szBuffer, NULL, 0);
}

// UNum_To_Float translation function //////////////////////////////////////

float	UPSF::UNum_To_Float	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (float)strtod(szBuffer, NULL);
}

// UNum_To_Double translation function /////////////////////////////////////

double	UPSF::UNum_To_Double	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	return (double)strtod(szBuffer, NULL);
}

// UNum_To_LDouble translation function ////////////////////////////////////

long double	UPSF::UNum_To_LDouble	(void*	univPtr,	long	univLen)

{
	char	szBuffer[80];

	strncpy(szBuffer, (char*)univPtr, min(univLen, 79));
	szBuffer[min(univLen, 79)] = 0;

	//TJW - Since this function is not widely used, hopefully
	//		translating to a double will suffice.
	return (long double)strtod(szBuffer, NULL);
}

// UInt_To_xxxx translation function ///////////////////////////////////////

long	UPSF::UInt_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	if(univLen != sizeof(DWORD)) {
		// a AUPSF_UNIV_INTEGER is length sizeof(DWORD)!!!!
		return AUPSF_SIZEERROR;
		}
	else if(natv==AUPSF_NATV_BYTE) {
		if(natvLen!=sizeof(BYTE))
			return AUPSF_SIZEERROR;
		else
			*(BYTE*)natvPtr = (BYTE)(*(DWORD*)univPtr);
		}
	else if(natv==AUPSF_NATV_CHAR) {
		if(natvLen!=sizeof(char))
			return AUPSF_SIZEERROR;
		else
			*(char*)natvPtr = (char)(*(long*)univPtr);
		}
	else if(natv==AUPSF_NATV_WORD) {
		if(natvLen!=sizeof(WORD))
			return AUPSF_SIZEERROR;
		else
			*(WORD*)natvPtr = (WORD)(*(DWORD*)univPtr);
		}
	else if(natv==AUPSF_NATV_SHORT) {
		if(natvLen!=sizeof(short))
			return AUPSF_SIZEERROR;
		else
			*(short*)natvPtr = (short)(*(long*)univPtr);
		}
	else if(natv==AUPSF_NATV_BOOL) {
		if(natvLen!=sizeof(BOOL))
			return AUPSF_SIZEERROR;
		else
			*(BOOL*)natvPtr = (BOOL)(*(DWORD*)univPtr);
		}
	else if(natv==AUPSF_NATV_DWORD) {
		if(natvLen!=sizeof(DWORD))
			return AUPSF_SIZEERROR;
		else
			*(DWORD*)natvPtr = *(DWORD*)univPtr;
		}
	else if(natv==AUPSF_NATV_LONG) {
		if(natvLen!=sizeof(long))
			return AUPSF_SIZEERROR;
		else
			*(long*)natvPtr = *(long*)univPtr;
		}
	else
		return AUPSF_CANTCONVERT;

	return AUPSF_NOERROR;
}

// UTxt_To_xxxx translation function ///////////////////////////////////////

long	UPSF::UTxt_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	long	retValue;

	if(natv==AUPSF_NATV_STRING) {
  	// returns number of bytes copied (positive) or an error (negative)
		retValue = UTxt_To_String(natvPtr, natvLen, univPtr, univLen);
		}
	else
		return AUPSF_CANTCONVERT;

	if(retValue >= 0L)
		return AUPSF_NOERROR;
	else
  	return retValue;
}

// UTxt_To_String translation function /////////////////////////////////////

long	UPSF::UTxt_To_String	(void*	natvPtr,	long	natvLen,	void*	univPtr,	long	univLen)

{
	long				retValue = AUPSF_NOERROR;

	long				count = 0L;
	long				charNum = 0L;
	BYTE *			univBuf = (BYTE *)univPtr;

	while(charNum<univLen) {
		if(*univBuf==0x0D)
			count++;
    univBuf++;
		charNum++;
		}

  // if the buffer isn't long enough return AUPSF_SIZEERROR (but copy first natvLen bytes)
	if(natvLen < (univLen + count + 1L))
		retValue = AUPSF_SIZEERROR;

	BYTE *	natvBuf = (BYTE *)natvPtr;
  univBuf = (BYTE *)univPtr;
	charNum = 0L;
	count = 0L;

	// copy the buffer (or until the natvBuf is full)
	while((charNum < univLen) && ((charNum+count) < (natvLen-1L))) {
		if(*univBuf==0x0D) {
    	// convert cr to cr-lf (mac to pc string format)
			*natvBuf = 0x0D;
      natvBuf++;
			count++;
			if((charNum+count) < (natvLen-1L))
				*natvBuf = 0x0A;
			}
		else
			*natvBuf = *univBuf;
		natvBuf++;
		univBuf++;
		charNum++;
		} // end...while

	// null-terminate the string
	*natvBuf = 0x00;

	if(retValue == AUPSF_SIZEERROR)
		return retValue; // return the error
	else
		return (charNum+count+1L); // return the length of the final string
}

// UBit_To_xxxx translation function ///////////////////////////////////////

long	UPSF::UBit_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	if(natv==AUPSF_NATV_BITMAP)
  	return UBit_To_Bitmap(univPtr, univLen, natvPtr, natvLen);
	else if(natv==AUPSF_NATV_BITMAPINFO)
		return UBit_To_BitmapInfo(univPtr, univLen, natvPtr, natvLen);
	else if(natv==AUPSF_NATV_BITMAPCORE)
		return UBit_To_BitmapCore(univPtr, univLen, natvPtr, natvLen);
	else
		return AUPSF_CANTCONVERT;
}

// UBit_To_Bitmap translation function /////////////////////////////////////

long	UPSF::UBit_To_Bitmap(void* univPtr, long /*univLen*/, void* natvPtr, long natvLen)

{
	BITMAPINFOHEADER* bmpIH = (BITMAPINFOHEADER*)univPtr;
	if(!bmpIH)
		return AUPSF_MEMORYERROR;

	HDC	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	if(!hDC) {
		return AUPSF_MEMORYERROR;
		}

	HBITMAP hBitmap = CreateBitmap((int)bmpIH->biWidth, (int)bmpIH->biHeight, bmpIH->biPlanes, bmpIH->biBitCount, NULL);
	if(!hBitmap) {
		DeleteDC(hDC);
		return AUPSF_MEMORYERROR;
		}

	long	ColorTableSize = 0L;
	if(bmpIH->biBitCount*bmpIH->biPlanes != 24)
		ColorTableSize = pow2((WORD)(bmpIH->biBitCount*bmpIH->biPlanes));

	void*	bits = (BYTE*)bmpIH + (size_t)(sizeof(BITMAPINFOHEADER) + ColorTableSize*sizeof(RGBQUAD));

	// set the bitmap
	SetDIBits(hDC, hBitmap, 0, bmpIH->biHeight, (char*)bits, (LPBITMAPINFO)bmpIH, DIB_RGB_COLORS);

	DeleteDC(hDC);

	// make sure the buffer is large enough for step one...
	if(natvLen < sizeof(BITMAP)) {
		DeleteObject(hBitmap);
		return AUPSF_SIZEERROR;
		}

	// get the bitmap information
	BITMAP*	bitmap = (BITMAP*)natvPtr;
	GetObject(hBitmap, sizeof(BITMAP), (char*)bitmap);

	// make sure the buffer is large enough for the bits and header
	long	reqSize = (long)sizeof(BITMAP)+(long)bitmap->bmWidthBytes*(long)bitmap->bmHeight*(long)bitmap->bmPlanes;
	if(natvLen < reqSize) {
		DeleteObject(hBitmap);
		return AUPSF_SIZEERROR;
		}

	// get the bits
	GetBitmapBits(hBitmap, (long)bitmap->bmPlanes*(long)bitmap->bmWidthBytes*(long)bitmap->bmHeight, (BYTE*)natvPtr+sizeof(BITMAP));

	DeleteObject(hBitmap);

	return AUPSF_NOERROR;
}

// UBit_To_BitmapInfo translation function /////////////////////////////////

long	UPSF::UBit_To_BitmapInfo(void* univPtr, long univLen, void* natvPtr, long natvLen)

{
	// this windows format is the universal bitmap format
	if(natvLen != univLen)
		return AUPSF_SIZEERROR;
	else
		CopyMemory(natvPtr, univPtr, univLen);

	return AUPSF_NOERROR;
}

// UBit_To_BitmapCore translation function /////////////////////////////////

long	UPSF::UBit_To_BitmapCore(void* univPtr, long univLen, void* natvPtr, long natvLen)

{
	BITMAPCOREHEADER*	bmpCH = (BITMAPCOREHEADER*)natvPtr;
	BITMAPINFOHEADER*	bmpIH = (BITMAPINFOHEADER*)univPtr;
	if(bmpIH->biCompression!=BI_RGB)
		return AUPSF_CANTCONVERT; // can only convert non-compressed bitmaps

	WORD	bitCount = bmpIH->biBitCount;

	if(!(bitCount==1 || bitCount==4 || bitCount==8 || bitCount==24))
		return AUPSF_CANTCONVERT;

	long	ColorTableSize;
  long	BitmapSize;
  long	NatvSize;
	if(bitCount==24) {
		// 24-bit color -- no color table
    ColorTableSize = 0;
		}
	else
		ColorTableSize = pow2(bitCount);

	BitmapSize = univLen - (long)sizeof(BITMAPINFOHEADER) - ColorTableSize*sizeof(RGBQUAD);
	NatvSize = BitmapSize + ColorTableSize*sizeof(RGBTRIPLE) + (long)sizeof(BITMAPCOREHEADER);
	if(natvLen < NatvSize)
  	return AUPSF_SIZEERROR;

	// store the bitmap core header
  bmpCH->bcSize = bmpIH->biSize;
	bmpCH->bcWidth = (WORD)bmpIH->biWidth;
	bmpCH->bcHeight = (WORD)bmpIH->biHeight;
	bmpCH->bcPlanes = bmpIH->biPlanes;
	bmpCH->bcBitCount = bmpIH->biBitCount;

  // store the color table
	RGBTRIPLE*	rgbTriple = (RGBTRIPLE*)((BYTE*)natvPtr + sizeof(BITMAPCOREHEADER));
  RGBQUAD*		rgbQuad = (RGBQUAD*)((BYTE*)univPtr + sizeof(BITMAPINFOHEADER));
	for(int i=0; i<ColorTableSize; i++) {
		rgbTriple[i].rgbtBlue =	rgbQuad[i].rgbBlue;
		rgbTriple[i].rgbtGreen = rgbQuad[i].rgbGreen;
    rgbTriple[i].rgbtRed = rgbQuad[i].rgbRed;
		}

	// store the bitmap
	void*	UnivBitmapBits = (void*)((BYTE*)univPtr + (size_t)(sizeof(BITMAPINFOHEADER) + ColorTableSize*sizeof(RGBQUAD)));
	void*	NatvBitmapBits = (void*)((BYTE*)natvPtr + (size_t)(sizeof(BITMAPCOREHEADER) + ColorTableSize*sizeof(RGBTRIPLE)));
	CopyMemory(NatvBitmapBits, UnivBitmapBits, BitmapSize);

	return AUPSF_NOERROR;
}

// URef_To_xxxx translation function ///////////////////////////////////////

long	UPSF::URef_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	if(univLen != sizeof(long))
		return AUPSF_SIZEERROR;
	else if(natv==NATV_OBJECTID) {
		if(natvLen!=sizeof(long))
			return AUPSF_SIZEERROR;
		else
			*(long*)natvPtr = URef_To_ObjID(univPtr);
		}
	else if(natv==AUPSF_NATV_OBJECTREF) {
		if(natvLen!=sizeof(AOBJECTID))
			return AUPSF_SIZEERROR;
		else {
			long	natvID;
			long	retValue = URef_To_Object(univPtr, natvID);
			if(retValue != AUPSF_NOERROR)
				return retValue;
			else
				*(AOBJECTID*)natvPtr = natvID;
			}
		}
	else
		return AUPSF_CANTCONVERT;

	return AUPSF_NOERROR;
}

// URef_To_ObjID translation function //////////////////////////////////////

long	UPSF::URef_To_ObjID	(void*	univPtr)

{
	return *(long*)univPtr;
}

// URef_To_Object translation function /////////////////////////////////////

AOBJECTID	UPSF::URef_To_Object	(void*	univPtr, AOBJECTID& natvID)

{
	if(*(long*)univPtr==0L) {
		// make sure that the NULL object returns as NULL rather than an error
  	natvID = 0L;
		return AUPSF_NOERROR;
    }
  else
		return GetNativeID(*(long*)univPtr, natvID);
}

// GetNativeID conversion routine //////////////////////////////////////////

long	UPSF::GetNativeID(long upsfID, AOBJECTID& natvID)

{
	IDLIST*	idlist = (IDLIST*)GlobalLock(hIDList);
	if(numIDs!=0L && idlist) {
		long i = 0L;
		while (i<numIDs) {
			if(upsfID == idlist[(size_t)i].univID) {
				natvID = idlist[(size_t)i].nativeID;
        GlobalUnlock(hIDList);
				return AUPSF_NOERROR;
				}
			i++;
      }
		}
	else if(!idlist && numIDs!=0L) {
  	GlobalUnlock(hIDList);
		return AUPSF_MEMORYERROR;
    }

  GlobalUnlock(hIDList);
	return AUPSF_INVALIDOBJECT;
}

// UClr_To_xxxx translation function ///////////////////////////////////////

long	UPSF::UClr_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	if(univLen != (sizeof(short)*3))
		return AUPSF_SIZEERROR;
	else if(natv==AUPSF_NATV_RGBCOLOR) {
		if(natvLen != sizeof(COLORREF))
			return AUPSF_SIZEERROR;
		else
			*(COLORREF*)natvPtr = UClr_To_RGBColor(univPtr);
		}
	else
		return AUPSF_CANTCONVERT;

	return AUPSF_NOERROR;
}

// UClr_To_RGBColor translation function ///////////////////////////////////

COLORREF	UPSF::UClr_To_RGBColor	(void*	univPtr)

{
	BYTE	Red		= *((BYTE*)univPtr+1);
	BYTE	Green	= *((BYTE*)univPtr+3);
	BYTE	Blue	= *((BYTE*)univPtr+5);

  return	RGB(Red, Green, Blue);
}

// UPnt_To_xxxx //////////////////////////////////////////////////////////// 

long	UPSF::UPnt_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	if(univLen != (sizeof(long)*2))
		return AUPSF_SIZEERROR;
	else if(natv==AUPSF_NATV_POINT) {
		if(natvLen != sizeof(POINT))
			return AUPSF_SIZEERROR;
		else
			*(POINT*)natvPtr = UPnt_To_Point(univPtr);
		}
	else
		return AUPSF_CANTCONVERT;

  return AUPSF_NOERROR;
}

// UPnt_To_Point translation function //////////////////////////////////////

POINT	UPSF::UPnt_To_Point	(void*	univPtr)

{
	long	x = *((long*)univPtr+0);
	long	y = *((long*)univPtr+1);

	POINT	point = {(int)x, (int)y};

	return point;
}

// URct_To_xxxx translation function ///////////////////////////////////////

long	UPSF::URct_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	if(univLen != (sizeof(long)*4))
		return AUPSF_SIZEERROR;
	else if(natv==AUPSF_NATV_RECT) {
		if(natvLen != sizeof(RECT))
			return AUPSF_SIZEERROR;
		else
			*(RECT*)natvPtr = URct_To_Rect(univPtr);
		}
	else
		return AUPSF_CANTCONVERT;

  return AUPSF_NOERROR;
}

// URct_To_Rect translation function ///////////////////////////////////////

RECT	UPSF::URct_To_Rect	(void*	univPtr)

{
	long	left		= *((long*)univPtr+0);
	long	top			= *((long*)univPtr+1);
	long	right		= *((long*)univPtr+2);
	long	bottom	= *((long*)univPtr+3);

	RECT	rect = {(int)left, (int)top, (int)right, (int)bottom};

	return rect;
}

// UFon_To_xxxx translation function ///////////////////////////////////////
long	UPSF::UFon_To_xxxx	(void* univPtr,	long univLen,	long natv, void* natvPtr,	long natvLen)

{
	long	retValue;

	if(natv==AUPSF_NATV_FONT) {
		if(natvLen < LF_FACESIZE)
			return AUPSF_SIZEERROR;
		else
			retValue = UFon_To_Font(natvPtr, univPtr, univLen);
		}
	else
		return AUPSF_CANTCONVERT;

  if(retValue >= 0)
		return AUPSF_NOERROR;
	else
		return retValue;
}

// UFon_To_Font translation function ///////////////////////////////////////

long	UPSF::UFon_To_Font	(void* natvPtr,	void*	univPtr, long univLen)

{
	numFonts = 0L;
	HKEY fontKey = NULL;
	hFonts = GlobalAlloc(MEMFLAGS, 1L);
	if(!hFonts)
		return AUPSF_MEMORYERROR;

	// get an information context for the display
	HDC	hDC = CreateIC("DISPLAY", NULL, NULL, NULL);
	if(!hDC) {
		hFonts = GlobalFree(hFonts);
		return AUPSF_MEMORYERROR;
		}

	// get a listing of the fonts for the current display
	if(!EnumFonts(hDC, (LPCSTR) NULL, (OLDFONTENUMPROC) EnumAllFaces, (LPARAM) this)) {
		hFonts = GlobalFree(hFonts);
		DeleteDC(hDC);
		return AUPSF_MEMORYERROR;
		}
  // free the IC for the display
  DeleteDC(hDC);

	// copy the font name to a local buffer
	HGLOBAL	hUniv = GlobalAlloc(MEMFLAGS, univLen+1L);
	if(!hUniv) {
		hFonts = GlobalFree(hFonts);
		return AUPSF_MEMORYERROR;
    }

	char*	univFace = (char*)GlobalLock(hUniv);
	if(!hUniv) {
		hFonts = GlobalFree(hFonts);
		GlobalFree(hUniv);
		return AUPSF_MEMORYERROR;
		}

	// store the universal face name in the local, null-terminated buffer
	CopyMemory(univFace, univPtr, univLen);
	univFace[(size_t)univLen]='\0';
  // make sure the buffer isn't too long
	if(univLen>=LF_FACESIZE)
		univFace[LF_FACESIZE-1] = 0;

	// check to see if the univFace is in the display-supported font list
  FACES*	fontArray = (FACES*)GlobalLock(hFonts);
  BOOL    fFound = FALSE;
	for(int i=0; (i<numFonts) && !fFound; i++) {
		if(!strcmp(fontArray[i].faceName, univFace)) {
			// there has been a match!!
			lstrcpy((char*)natvPtr, univFace);
			fFound = TRUE;
			}
		}

  if(!fFound)
    {
  	// check to see if the univFace is in the "FontSubstitutes" section of win.ini
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\FontSubstitues",
    	NULL, KEY_ALL_ACCESS, &fontKey) == ERROR_SUCCESS)
      {
      AUtlGetKeyString(fontKey, univFace, (char*)natvPtr, LF_FACESIZE, "");
      RegCloseKey(fontKey);
      }
    else
    	((char*)natvPtr)[0] = '\0';
    
  	if(lstrlen((char*)natvPtr)==0)
      {
      fontKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_FONTTRANS);
      
    	// Get the conversion from the appbldr.ini file or default to "System"
    	AUtlGetKeyString(fontKey, univFace, (char*)natvPtr, LF_FACESIZE, "System");
      
      if(fontKey)
      	AUtlCloseProductSubKey(fontKey);
      }

    // Specify the font conversion in the log file
    char  szComment[255];
    wsprintf(szComment, "Substituting font \"%s\" for font \"%s.\"", natvPtr, univFace);
    LogComment(szComment);
    }

  // Clean up memory and exit
  GlobalUnlock(hFonts);
	hFonts = GlobalFree(hFonts);
	GlobalUnlock(hUniv);
	GlobalFree(hUniv);

	return (long)lstrlen((char*)natvPtr);
}

////////////////////////////////////////////////////////////////////////////

int CALLBACK	EnumAllFaces(const ENUMLOGFONT* lf,
									const NEWTEXTMETRIC* ,
									int ,
									LPARAM lData)

{
	UPSF*		upsf	= (UPSF*) lData;

	if((upsf->hFonts = GlobalReAlloc(upsf->hFonts,
															(long)sizeof(FACES)*(upsf->numFonts+1L), MEMFLAGS)) == NULL)
		return 0;

	FACES*	fontArray = (FACES*)GlobalLock(upsf->hFonts);
	if(!fontArray)
  	return 0;

	// copy the face name and increment the number of fonts
	lstrcpy (fontArray[(size_t)(upsf->numFonts)].faceName, lf->elfLogFont.lfFaceName);
	upsf->numFonts++;

  GlobalUnlock(upsf->hFonts);
	return 1 ;
}

////////////////////////////////////////////////////////////////////////////
