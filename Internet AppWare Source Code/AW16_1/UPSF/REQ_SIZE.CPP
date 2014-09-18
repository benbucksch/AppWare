////////////////////////////////////////////////////////////////////////////
//	UPSF Functions to get Required Sizes
//		"req_size.cpp"
//
//	(c) Serius Corporation, 1989-1992
//
//	Kevin J. Smart
//		start:		11/04/92
//		finish:		11/xx/92
//
////////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include "almsys.h"

#include "upsf_cls.h"

////////////////////////////////////////////////////////////////////////////

long	UPSF::RequiredSize	(long ntype, long utype, long usize, HGLOBAL hUniv)

{
	if(ntype==utype) // no conversion necessary
		return usize;
	else if(utype==AUPSF_UNIV_NUMBER)
		return Req_Size_For_UNum(ntype);
	else if(utype==AUPSF_UNIV_INTEGER)
		return Req_Size_For_UInt(ntype);
	else if(utype==AUPSF_UNIV_TEXT)
		return Req_Size_For_UTxt(ntype, usize, hUniv);
	else if(utype==AUPSF_UNIV_BITMAP)
		return Req_Size_For_UBit(ntype, usize, hUniv);
	else if(utype==AUPSF_UNIV_OBJECTREF)
		return Req_Size_For_URef(ntype);
	else if(utype==AUPSF_UNIV_COLOR)
		return Req_Size_For_UClr(ntype);
	else if(utype==AUPSF_UNIV_POINT)
		return Req_Size_For_UPnt(ntype);
	else if(utype==AUPSF_UNIV_RECT)
		return Req_Size_For_URct(ntype);
	else if(utype==AUPSF_UNIV_FONT)
		return Req_Size_For_UFon(ntype);
	else // conversion not supported
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_UNum(long natvType)

{
	if(natvType==AUPSF_NATV_BYTE)
		return (long)sizeof(BYTE);
	else if(natvType==AUPSF_NATV_CHAR)
		return (long)sizeof(char);
	else if(natvType==AUPSF_NATV_WORD)
		return (long)sizeof(WORD);
	else if(natvType==AUPSF_NATV_SHORT)
		return (long)sizeof(short);
	else if(natvType==AUPSF_NATV_DWORD)
		return (long)sizeof(DWORD);
	else if(natvType==AUPSF_NATV_LONG)
		return (long)sizeof(long);
	else if(natvType==AUPSF_NATV_FLOAT)
		return (long)sizeof(float);
	else if(natvType==AUPSF_NATV_DOUBLE)
		return (long)sizeof(double);
	else if(natvType==AUPSF_NATV_LONGDOUBLE)
		return (long)sizeof(long double);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_UInt(long natvType)

{
	if(natvType==AUPSF_NATV_BYTE)
		return (long)sizeof(BYTE);
	else if(natvType==AUPSF_NATV_CHAR)
		return (long)sizeof(char);
	else if(natvType==AUPSF_NATV_WORD)
		return (long)sizeof(WORD);
	else if(natvType==AUPSF_NATV_SHORT)
		return (long)sizeof(short);
	else if(natvType==AUPSF_NATV_DWORD)
		return (long)sizeof(DWORD);
	else if(natvType==AUPSF_NATV_LONG)
		return (long)sizeof(long);
	else if(natvType==AUPSF_NATV_BOOL)
		return (long)sizeof(BOOL);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_UTxt(long natvType, long univLen, HGLOBAL hUniv)

{
	if(natvType==AUPSF_NATV_STRING)
  	return Get_Req_String_Size(univLen, hUniv);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Get_Req_String_Size(long univLen, HGLOBAL hUniv)

{
	BYTE huge*	univPtr = NULL;
  if(univLen > 0L) {
		univPtr = (BYTE huge*)GlobalLock(hUniv);
		if(!univPtr)
			return AUPSF_MEMORYERROR;
    }

	long	count = 0L;
	long	charNum = 0L;
	while(charNum < univLen) {
		// count the number of cr's - will be replaced with cr-lf
		if(*univPtr==0x0D)
			count++;
		univPtr++;
		charNum++;
		}

  if(univLen > 0L)
		GlobalUnlock(hUniv);
	return (univLen + count + 1L);
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_UBit(long natvType, long univLen, HGLOBAL hUniv)

{
	if(natvType==AUPSF_NATV_BITMAP)
		return Get_Req_Bitmap_Size(univLen, hUniv);
	else if(natvType==AUPSF_NATV_BITMAPINFO)
		return univLen; // this is the universal standard type
	else if(natvType==AUPSF_NATV_BITMAPCORE)
		return Get_Req_BitmapCore_Size(univLen, hUniv);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Get_Req_Bitmap_Size(long /*univLen*/, HGLOBAL hUniv)

{
	BITMAPINFOHEADER* bmpIH = (BITMAPINFOHEADER*)GlobalLock(hUniv);
	if(!bmpIH)
		return AUPSF_MEMORYERROR;

	HDC	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	if(!hDC) {
		GlobalUnlock(hUniv);
		return AUPSF_MEMORYERROR;
		}

	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, (int)bmpIH->biWidth, (int)bmpIH->biHeight);
	if(!hBitmap) {
		GlobalUnlock(hUniv);
		DeleteDC(hDC);
		return AUPSF_MEMORYERROR;
		}

	long	ColorTableSize = 0L;
	if(bmpIH->biBitCount*bmpIH->biPlanes != 24)
		ColorTableSize = pow2(bmpIH->biBitCount*bmpIH->biPlanes);

	void*	bits = (BYTE*)bmpIH + (size_t)(sizeof(BITMAPINFOHEADER) + ColorTableSize*sizeof(RGBQUAD));

	// set the bitmap
	SetDIBits(hDC, hBitmap, 0, bmpIH->biHeight, (char*)bits, (LPBITMAPINFO)bmpIH, DIB_RGB_COLORS);

	// get the bitmap information
	BITMAP	bitmap;
	GetObject(hBitmap, sizeof(BITMAP), (char*)&bitmap);

	DeleteObject(hBitmap);
	DeleteDC(hDC);

	GlobalUnlock(hUniv);
	return ((long)sizeof(BITMAP)+(long)bitmap.bmWidthBytes*(long)bitmap.bmHeight*(long)bitmap.bmPlanes);
}

// function to determine the required native size //////////////////////////

long	UPSF::Get_Req_BitmapCore_Size(long univLen, HGLOBAL hUniv)

{
	BITMAPINFOHEADER* univPtr = (BITMAPINFOHEADER*)GlobalLock(hUniv);
	if(!univPtr)
		return AUPSF_MEMORYERROR;

	if(univPtr->biBitCount==24) { // there is no color table with 24-bit color
  	GlobalUnlock(hUniv);
		return (univLen + sizeof(BITMAPCOREHEADER) - sizeof(BITMAPINFOHEADER));
    }

	long	ColorTableSize = pow2(univPtr->biBitCount*univPtr->biPlanes);
	GlobalUnlock(hUniv);
	return (univLen + sizeof(BITMAPCOREHEADER) + ColorTableSize*sizeof(RGBTRIPLE) - sizeof(BITMAPINFOHEADER) - ColorTableSize*sizeof(RGBQUAD));
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_URef(long natvType)

{
	if(natvType==NATV_OBJECTID || natvType==AUPSF_NATV_OBJECTREF)
		return sizeof(OBJECTID);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_UClr(long natvType)

{
	if(natvType==AUPSF_NATV_RGBCOLOR)
		return sizeof(COLORREF);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_UPnt(long natvType)

{
	if(natvType==AUPSF_NATV_POINT)
		return sizeof(POINT);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_URct(long natvType)

{
	if(natvType==AUPSF_NATV_RECT)
		return sizeof(RECT);
	else
		return AUPSF_CANTCONVERT;
}

// function to determine the required native size //////////////////////////

long	UPSF::Req_Size_For_UFon(long natvType)

{
	if(natvType==AUPSF_NATV_FONT)
		return (long)LF_FACESIZE; // this is the maximum required size.
	else
  	return AUPSF_CANTCONVERT;
}

////////////////////////////////////////////////////////////////////////////
