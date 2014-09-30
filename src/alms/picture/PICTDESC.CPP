/*//////////////////////////////////////////////////////////////
//
//                                      Copyright 1993, Novell, Inc.  All rights reserved
//
//      THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//      PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//      ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//      A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//      THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//      ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//      WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//      REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//      COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//      WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//      EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//      THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
// To test this desc. module, I have added some code to simulate as
// a working windows environment.  These added code pieces are encapsulated
// in #ifdef DEMO blocks.                          
// Also, till I have a working version of the OIP and shell, I have ifdef'd
// all the code related to these modules.  June, 1992
// 
//
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT

#define PERR_ROLLBACK                   0x0001
#define PERR_SETEMPTY                   0x0002



#include <a_alm.h>
#include <a_almutl.h>
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <intdebug.h>
#include <helpids.h>
#include "picture.h"

#define MS_BMTYPE                       0x4d42

HANDLE hBORDER = NULL; // handle to border brush bitmap
WORD		instCount = 0;
extern int _WinAllocFlag;
/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)    								(((i) + 31) / 32 * 4)

#define PIXELWIDTH                      8
#define ENABLECHILDREN  								(0x80000000L)
#define DISABLECHILDREN 								(0x00000000L)
#define MAX_TYPE                      	100
#define DUMMYHANDLE             				1
#define MAXPALETTE              				256
#define _DEMO
// turn the style on
#define SetWindowStyle(hWnd, Style) \
   SetWindowLong(hWnd, GWL_STYLE, Style|GetWindowLong(hWnd, GWL_STYLE));
// turn the style off
#define ResetWindowStyle(hWnd, Style) \
	 SetWindowLong(hWnd, GWL_STYLE, ~Style&GetWindowLong(hWnd, GWL_STYLE));



extern LPLONG GetPictObject(AOBJECTID objID);

typedef struct {
    DWORD   key;
    HANDLE  hmf;
    RECT    bbox;
    WORD    inch;
    DWORD   reserved;
    WORD    checksum;
    } PLACEABLEMETAHEADER;



BOOL CALLBACK PictureDlgProc(    // hDlg, message, wParam, lParam)
  HWND hDlg,
	UINT message,
  WPARAM wParam,
  LONG lParam);


HBITMAP DrawPictWindow(PICTOBJECT *pO);
HANDLE  ReadBitmap(int);

HINSTANCE               hInst;



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

//////////////////////////////////////////////////////////////////
// Palette:  A LogPalette structure that is used by picture during  
//           object editing.  This is a generalized palette shared 
//           across different pictures
//////////////////////////////////////////////////////////////////

// usage count
static int Gn_hPal = 0;
struct WndPalette
  {
  WORD          palVersion;
  WORD          palNumEntries;
  PALETTEENTRY  palPalEntry[256];
	};

static HPALETTE G_hPal;

WndPalette G_logpalette =
	{
	0x0300,
  256,
  {{0xFF, 0xFF, 0xFF, 0}, {0x00, 0x00, 0x00, 0}, {0x77, 0x77, 0x77, 0}, {0xFF, 0x00, 0x00, 0},
   {0x00, 0xFF, 0x00, 0}, {0x00, 0x00, 0xFF, 0}, {0xFF, 0xFF, 0x00, 0}, {0xFF, 0x00, 0xFF, 0},
   {0x00, 0xFF, 0xFF, 0}, {0xBB, 0xBB, 0xBB, 0}, {0xFF, 0x99, 0x00, 0}, {0x66, 0x99, 0x00, 0},
   {0x00, 0x66, 0xFF, 0}, {0xFF, 0xFF, 0x99, 0}, {0xFF, 0x99, 0xFF, 0}, {0x66, 0xFF, 0x99, 0},
   {0x22, 0x22, 0x22, 0}, {0xCC, 0x00, 0x00, 0}, {0x66, 0xCC, 0x00, 0}, {0x66, 0xCC, 0xFF, 0},
   {0xFF, 0xCC, 0x66, 0}, {0xCC, 0x99, 0xFF, 0}, {0xCC, 0xFF, 0xFF, 0}, {0x88, 0x88, 0x88, 0},
   {0xFF, 0x66, 0x33, 0}, {0x00, 0xCC, 0x33, 0}, {0x00, 0x00, 0x77, 0}, {0xFF, 0xFF, 0x66, 0},
   {0xFF, 0x99, 0xCC, 0}, {0x99, 0xFF, 0xCC, 0}, {0x44, 0x44, 0x44, 0}, {0xFF, 0x33, 0x00, 0},
   {0x99, 0xFF, 0x33, 0}, {0x66, 0x99, 0xFF, 0}, {0xFF, 0xCC, 0x00, 0}, {0xCC, 0x00, 0x99, 0},
   {0x99, 0xFF, 0xFF, 0}, {0xAA, 0xAA, 0xAA, 0}, {0xCC, 0x66, 0x00, 0}, {0x66, 0x99, 0x33, 0},
   {0xCC, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0x00, 0}, {0xFF, 0xCC, 0xFF, 0}, {0x99, 0x99, 0xFF, 0},
   {0x55, 0x55, 0x55, 0}, {0xDD, 0x00, 0x00, 0}, {0x00, 0xDD, 0x00, 0}, {0x00, 0x00, 0xBB, 0},
   {0xFF, 0xFF, 0x33, 0}, {0xFF, 0xCC, 0xCC, 0}, {0x33, 0xFF, 0xCC, 0}, {0xEE, 0xEE, 0xEE, 0},
   {0x88, 0x00, 0x00, 0}, {0x00, 0x55, 0x00, 0}, {0x00, 0x00, 0x55, 0}, {0xCC, 0xCC, 0x00, 0},
   {0xCC, 0x33, 0xCC, 0}, {0x33, 0xFF, 0xFF, 0}, {0x11, 0x11, 0x11, 0}, {0xFF, 0x33, 0x66, 0},
   {0x33, 0xCC, 0x33, 0}, {0x00, 0x66, 0xCC, 0}, {0xFF, 0xCC, 0x33, 0}, {0x99, 0x00, 0x66, 0},
   {0x66, 0x99, 0xCC, 0}, {0xDD, 0xDD, 0xDD, 0}, {0xAA, 0x00, 0x00, 0}, {0x00, 0x77, 0x00, 0},
   {0x00, 0x00, 0x88, 0}, {0xCC, 0xFF, 0x33, 0}, {0xCC, 0x00, 0xCC, 0}, {0x99, 0xCC, 0xFF, 0},
   {0xCC, 0xCC, 0xCC, 0}, {0xFF, 0x33, 0x33, 0}, {0x00, 0xCC, 0x00, 0}, {0x33, 0x33, 0xFF, 0},
   {0xCC, 0xCC, 0x33, 0}, {0xFF, 0x00, 0xCC, 0}, {0x33, 0x99, 0xCC, 0}, {0x99, 0x99, 0x99, 0},
   {0xFF, 0x66, 0x00, 0}, {0x66, 0xFF, 0x00, 0}, {0x33, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0x66, 0},
   {0x66, 0x66, 0x99, 0}, {0x66, 0xCC, 0xCC, 0}, {0x66, 0x66, 0x66, 0}, {0x44, 0x00, 0x00, 0},
   {0x00, 0x22, 0x00, 0}, {0x00, 0x00, 0x33, 0}, {0x99, 0x99, 0x00, 0}, {0x99, 0x66, 0x99, 0},
   {0x66, 0x66, 0xCC, 0}, {0x99, 0xCC, 0xCC, 0}, {0xEE, 0x00, 0x00, 0}, {0x00, 0xEE, 0x00, 0},
   {0x00, 0x00, 0xEE, 0}, {0xFF, 0xFF, 0xCC, 0}, {0xFF, 0x33, 0xFF, 0}, {0x00, 0xFF, 0xCC, 0},
   {0x99, 0x99, 0xCC, 0}, {0xFF, 0x00, 0x99, 0}, {0x00, 0x88, 0x00, 0}, {0x66, 0x33, 0xFF, 0},
   {0xCC, 0x99, 0x33, 0}, {0xCC, 0x33, 0xFF, 0}, {0x66, 0xFF, 0xCC, 0}, {0x33, 0x33, 0x33, 0},
   {0xBB, 0x00, 0x00, 0}, {0x00, 0x66, 0x00, 0}, {0x00, 0x00, 0x44, 0}, {0xCC, 0x99, 0x66, 0},
   {0xFF, 0x66, 0x99, 0}, {0x33, 0xCC, 0xFF, 0}, {0x99, 0x99, 0x66, 0}, {0xFF, 0x00, 0x33, 0},
   {0x99, 0xCC, 0x00, 0}, {0x00, 0x00, 0xAA, 0}, {0xCC, 0xFF, 0x99, 0}, {0xCC, 0x33, 0x99, 0},
   {0x66, 0x66, 0xFF, 0}, {0x99, 0x66, 0x66, 0}, {0x55, 0x00, 0x00, 0}, {0x00, 0x11, 0x00, 0},
   {0x33, 0x00, 0x66, 0}, {0xFF, 0xCC, 0x99, 0}, {0xCC, 0x66, 0x99, 0}, {0x00, 0xFF, 0x99, 0},
   {0xCC, 0x99, 0xCC, 0}, {0xFF, 0x00, 0x66, 0}, {0x33, 0xCC, 0x00, 0}, {0x00, 0x00, 0xCC, 0},
   {0x99, 0xFF, 0x00, 0}, {0x99, 0x33, 0x66, 0}, {0x00, 0xCC, 0xFF, 0}, {0x66, 0x99, 0x99, 0},
   {0xCC, 0x00, 0x33, 0}, {0x00, 0x99, 0x00, 0}, {0x66, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0xCC, 0},
   {0xCC, 0x33, 0x66, 0}, {0x99, 0xFF, 0x99, 0}, {0xCC, 0xCC, 0xFF, 0}, {0xCC, 0x33, 0x00, 0},
   {0x00, 0xFF, 0x33, 0}, {0x00, 0x33, 0xFF, 0}, {0xFF, 0x99, 0x66, 0}, {0xCC, 0x00, 0x66, 0},
   {0x66, 0xFF, 0xFF, 0}, {0x33, 0x66, 0x66, 0}, {0x99, 0x00, 0x00, 0}, {0x99, 0xFF, 0x66, 0},
   {0x33, 0x99, 0xFF, 0}, {0xCC, 0xCC, 0x66, 0}, {0xFF, 0x33, 0xCC, 0}, {0x00, 0x99, 0xCC, 0},
   {0xCC, 0x99, 0x99, 0}, {0x77, 0x00, 0x00, 0}, {0x00, 0x33, 0x00, 0}, {0x33, 0x00, 0xCC, 0},
   {0xCC, 0x99, 0x00, 0}, {0xCC, 0x66, 0xCC, 0}, {0x00, 0x99, 0xFF, 0}, {0x66, 0x33, 0x33, 0},
   {0xCC, 0x33, 0x33, 0}, {0x33, 0x66, 0x00, 0}, {0x00, 0x00, 0x22, 0}, {0xCC, 0xCC, 0x99, 0},
   {0xCC, 0x66, 0xFF, 0}, {0x00, 0xCC, 0xCC, 0}, {0x66, 0x66, 0x33, 0}, {0x22, 0x00, 0x00, 0},
   {0x33, 0xFF, 0x00, 0}, {0x00, 0x33, 0x99, 0}, {0x99, 0x66, 0x00, 0}, {0xFF, 0x66, 0xFF, 0},
   {0x33, 0xCC, 0xCC, 0}, {0x99, 0xCC, 0x99, 0}, {0xFF, 0x33, 0x99, 0}, {0x33, 0xFF, 0x66, 0},
   {0x00, 0x00, 0x11, 0}, {0xFF, 0x99, 0x33, 0}, {0x99, 0x33, 0x99, 0}, {0x66, 0xCC, 0x99, 0},
   {0x99, 0xCC, 0x66, 0}, {0x99, 0x00, 0x33, 0}, {0x00, 0x44, 0x00, 0}, {0x33, 0x66, 0xFF, 0},
   {0x99, 0x99, 0x33, 0}, {0xFF, 0x66, 0xCC, 0}, {0x33, 0xFF, 0x99, 0}, {0x33, 0x99, 0x99, 0},
   {0x33, 0x00, 0x00, 0}, {0x00, 0xFF, 0x66, 0}, {0x33, 0x00, 0x99, 0}, {0x99, 0x66, 0x33, 0},
   {0x99, 0x33, 0xCC, 0}, {0x33, 0xCC, 0x99, 0}, {0x99, 0x66, 0xCC, 0}, {0x99, 0x33, 0x00, 0},
   {0x00, 0x99, 0x33, 0}, {0x00, 0x00, 0x99, 0}, {0xCC, 0x66, 0x33, 0}, {0x99, 0x00, 0x99, 0},
   {0x33, 0x66, 0xCC, 0}, {0x33, 0x33, 0x66, 0}, {0xFF, 0x66, 0x66, 0}, {0x00, 0x66, 0x33, 0},
   {0x66, 0x00, 0xCC, 0}, {0x66, 0xFF, 0x33, 0}, {0xFF, 0x99, 0x99, 0}, {0x00, 0x66, 0x66, 0},
   {0x33, 0x99, 0x00, 0}, {0x33, 0x33, 0x99, 0}, {0x66, 0xFF, 0x66, 0}, {0x33, 0x00, 0x33, 0},
   {0xCC, 0x66, 0x66, 0}, {0x00, 0xAA, 0x00, 0}, {0x00, 0x33, 0xCC, 0}, {0x66, 0x33, 0x00, 0},
   {0x33, 0x99, 0x66, 0}, {0x66, 0x00, 0x00, 0}, {0x00, 0xBB, 0x00, 0}, {0x66, 0x00, 0x99, 0},
   {0x33, 0xFF, 0x33, 0}, {0x66, 0x33, 0xCC, 0}, {0x66, 0x00, 0x33, 0}, {0x33, 0x33, 0x00, 0},
   {0x99, 0x00, 0xCC, 0}, {0x00, 0x99, 0x66, 0}, {0x00, 0x00, 0x66, 0}, {0x66, 0xCC, 0x66, 0},
   {0x66, 0x00, 0x66, 0}, {0x99, 0x66, 0xFF, 0}, {0x33, 0x33, 0xCC, 0}, {0x00, 0x99, 0x99, 0},
   {0x99, 0x33, 0x33, 0}, {0x00, 0x00, 0xDD, 0}, {0x99, 0xCC, 0x33, 0}, {0x66, 0x33, 0x66, 0},
   {0x11, 0x00, 0x00, 0}, {0x33, 0x66, 0x99, 0}, {0x66, 0x66, 0x00, 0}, {0x99, 0x00, 0xFF, 0},
   {0x00, 0x33, 0x33, 0}, {0x33, 0x99, 0x33, 0}, {0x99, 0x33, 0xFF, 0}, {0x00, 0xCC, 0x99, 0},
   {0x00, 0x33, 0x66, 0}, {0x66, 0xCC, 0x33, 0}, {0x66, 0x99, 0x66, 0}, {0x66, 0x33, 0x99, 0},
	 {0x33, 0xCC, 0x66, 0}, {0x33, 0x66, 0x33, 0}, {0x00, 0xCC, 0x66, 0}, {0x00, 0x66, 0x99, 0}}
	 };


////////////////////////////////////////////////////////////////////////////
//
// Method:      PICTOBJECT::CreatePalette  (static)
//
// Description: This function will create the default palette for the window
//              object.
//
////////////////////////////////////////////////////////////////////////////

HPALETTE CreatePaletteHandle()
	{
	HPALETTE hpal;
	if (Gn_hPal == 0)
		{
		HDC hdc         = CreateDC("DISPLAY", NULL, NULL, NULL);
		int nPalSize    = GetDeviceCaps(hdc, SIZEPALETTE);
		hpal = (nPalSize==256) ? ::CreatePalette((LOGPALETTE*)&G_logpalette) : NULL;
		DeleteDC(hdc);
		}
	else {
		hpal = G_hPal;
		ADBG_ASSERT_W(G_hPal, ;, "Palette already exists");
    }

	Gn_hPal += 1;
	return hpal;
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      WndData::DestroyPalette  (static)
//
// Description: This function will create the default palette for the window
//              object.
//
////////////////////////////////////////////////////////////////////////////

void DestroyPalette(HPALETTE hpal)
	{
	if (Gn_hPal > 0)
		Gn_hPal--;
	else if (Gn_hPal == 0)
    return;

	if((hpal) && (Gn_hPal == 0))
    {
    DeleteObject(hpal);
		}
  }


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibNumColors(VOID  * lv)                                *
 *                                                                          *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : The number of colors in the DIB.                           *
 *                                                                          *
 ****************************************************************************/
WORD DibNumColors (VOID  *lv)
  {
  int                 bits;
  LPBITMAPINFOHEADER  lpbi;
  LPBITMAPCOREHEADER  lpbc;

	lpbi = ((LPBITMAPINFOHEADER)lv);
	lpbc = ((LPBITMAPCOREHEADER)lv);

  /*  With the BITMAPINFO format headers, the size of the palette
  *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
  *  is dependent on the bits per pixel ( = 2 raised to the power of
  *  bits/pixel).
	*/
	if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
		{
		if (lpbi->biClrUsed != 0)
	    return (WORD)lpbi->biClrUsed;
		bits = lpbi->biBitCount * lpbi->biPlanes;
    }
  else
		bits = lpbc->bcBitCount * lpbc->bcPlanes; ;



  switch (bits){
		case 1:
			return 2;
		case 4:
			return 16;
		case 8:
			return 256;
		default:
			/* A 24 bitcount DIB has no color table */
		return 0;
    }
	}


/****************************************************************************

		FUNCTION: GetOBJDPtr

		PURPOSE: Returns the C++ object pointer assoc. with this dlg window

****************************************************************************/
LPLONG GetOBJDPtr(HWND hWnd)
	{
  
	LPLONG cursorObj;
	AOBJECTID theObject = (AOBJECTID)GetWindowLong(hWnd, DWL_USER);
	cursorObj =  (LPLONG)AObjGetRuntimeInfo (theObject);
  return cursorObj;
	}

/****************************************************************************

		FUNCTION: SetOBJDPtr

		PURPOSE: Sets a C++ object pointer assoc. with this dlg window

****************************************************************************/

LONG SetOBJDPtr(HWND hWnd, AOBJECTID theObject, LONG cursorObj)
	{

	long rVal = AObjSetRuntimeInfo(theObject, (LONG)cursorObj);
	SetWindowLong(hWnd, DWL_USER, (LONG)theObject);
	return rVal;
	}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  PaletteSize(VOID  * lv)                                *
 *                                                                          *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *                is of the BITMAPCOREHEADER type, the number of colors is  *
 *                multiplied by 3 to give the palette size, otherwise the   *
 *                number of colors is multiplied by 4.                                                          *
 *                                                                          *
 *  RETURNS    :  Palette size in number of bytes.                          *
 *                                                                          *
 ****************************************************************************/
WORD PaletteSize (VOID  *lv)

	{
	LPBITMAPINFOHEADER lpbi;
  WORD               NumColors;

	lpbi      = (LPBITMAPINFOHEADER)lv;
  NumColors = DibNumColors(lpbi);

  if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
		return (WORD)(NumColors * sizeof(RGBTRIPLE));
  else
		return (WORD)(NumColors * sizeof(RGBQUAD));
	}

/////////////////////////////////////////////////////////////
// Function to Handle MEM ALLOCATION failure
//              - Rollback to older state of picture datas
//              - Set the picture image to empty
/////////////////////////////////////////////////////////////

int PICTOBJECT::pErrMemLock(int errType)

	{
	switch (errType) {
		// not implemented yet
		case PERR_ROLLBACK: 
			{
			}
			break;
		case PERR_SETEMPTY:
    default:
			{
			// if it is a bitmap picture, set the image to be empty
			if (pictData.pictType == BITMAPPICT)
				{
				ZeroMemory((LPSTR)&bmpFileHdr, sizeof(BITMAPFILEHEADER));
				ZeroMemory(&bmpInfoHdr, sizeof(BITMAPINFOHEADER));
				ImageSize = 0;
				tableSize = 0;
				AMemSetSize(memID, 0);
				rgbQuadAndBits = NULL;
				AMemSetSize(memID, 0);
				}
			else {
				AMemSetSize(memID, 0); 
				}
			}
    }
	return 1;
	}

/////////////////////////////////////////////////////////////
// Function to delete all picture Handles during objd time
/////////////////////////////////////////////////////////////

int PICTOBJECT::DeletePHandle(HANDLE hPict)
	{

	// by the time object change comes thro' I do not know what the
	// previous picture-type was.  I can save a field 'oldType' but
	// this will work juist fine.
	int rVal = 1;  // 1: bitmap deleted 2: MetaFile deleted 0: none deleted
	if (hPict == 0)
    rVal = 0;
	else {
		if (DeleteObject(hPict) == FALSE)
			{
			if (DeleteEnhMetaFile((HENHMETAFILE)hPict))
				rVal = 2;
			else
				rVal = 0;
			}
		}
	return rVal;
	}





/**********************************************************
PICTOBJECT::InitObjFieldsFromDatas()
This method will init the fields of the C++ class from object
datas

**********************************************************/
void PICTOBJECT::InitObjFieldsFromDatas()
	{
	LPSTR                   lpData = (LPSTR) AObjLockData(objectID, PICTDATA0);
	int oldDataSize = (int)AObjGetDataSize(objectID, PICTDATA0);
	int newDataSize = sizeof(PICTDATA);

	// check added for version 2.0 April 1, 1993
	if ((newDataSize != oldDataSize) ||
		( ((PICTDATA *)lpData)->version != DATAVersion))
		{
		AObjUnlockData(objectID, PICTDATA0);
		AObjResizeData(objectID, PICTDATA0, sizeof(PICTDATA));
		lpData = (LPSTR) AObjLockData(objectID, PICTDATA0);
		((PICTDATA*)lpData)->bFrame = FALSE;
		((PICTDATA *)lpData)->version = DATAVersion;

    }

	pictData =  *(PICTDATA  *)lpData;
	AObjUnlockData(objectID, PICTDATA0);

	// Initialize the PictData field to
	// the data passed in.
	if (pictData.pictType == BITMAPPICT)
		{
		BMP                  *lpBMP;

		metaBits = NULL;
		// if nothing is saved in Object datas #1
		// then break;
		if ((ImageSize = AObjGetDataSize(objectID, 1)) == 0)
			{
			ZeroMemory(&bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			return;
			}
		lpBMP = (BMP *)AObjLockData(objectID,BMPMETADATA);

		if (pictData.bClipboard)
			{
			CopyMemory((LPSTR)&bmpFileHdr,(LPSTR)&lpBMP->bmpFileHdr, sizeof(BITMAPFILEHEADER));
			CopyMemory(&bmpInfoHdr,(LPSTR) &lpBMP->bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			tableSize = PaletteSize(&(lpBMP->bmpInfoHdr));
			int nColorBits =  (int)(bmpInfoHdr.biBitCount * bmpInfoHdr.biPlanes);
			ImageSize = bmpFileHdr.bfSize - bmpFileHdr.bfOffBits;
			ImageSize = WIDTHBYTES((DWORD)bmpInfoHdr.biWidth* nColorBits) * bmpInfoHdr.biHeight;
			AMemSetSize(memID, ImageSize + tableSize);
			rgbQuadAndBits = (BYTE*)AMemLock(memID);
			if (rgbQuadAndBits == NULL)
				{
				pErrMemLock(PERR_SETEMPTY);
				}
      else {
				CopyMemory(rgbQuadAndBits,lpBMP->rgbQuadAndBits, ImageSize + tableSize);
				AMemUnlock(memID);
				}
			}
		else
			{
			long                    hugesize;
			CopyMemory((LPSTR)&bmpFileHdr,(LPSTR)&lpBMP->bmpFileHdr, sizeof(BITMAPFILEHEADER));
			CopyMemory(&bmpInfoHdr,(LPSTR) &lpBMP->bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			ImageSize = bmpFileHdr.bfSize - bmpFileHdr.bfOffBits;;
			tableSize = PaletteSize(&bmpInfoHdr); //  * sizeof(RGBQUAD);
			AMemSetSize(memID, ImageSize + tableSize);
			rgbQuadAndBits = (BYTE*)AMemLock(memID);

			if (rgbQuadAndBits)
				{
				hugesize = ImageSize;
				CopyMemory((LPSTR)rgbQuadAndBits, lpBMP->rgbQuadAndBits, (size_t)tableSize);
				BYTE *lpImage = (BYTE *)(lpBMP->rgbQuadAndBits + (int)tableSize);
				BYTE *tmpBits = (BYTE *)(rgbQuadAndBits + (int)tableSize);
				// copy the data to a local memory buffer--being careful with large blocks
				CopyMemory(tmpBits,lpImage, hugesize);
				AMemUnlock(memID);
				}
			else {
				pErrMemLock(PERR_SETEMPTY);
				}
			}

		AObjUnlockData(objectID,BMPMETADATA);
		}
	else {
		BYTE *              lpMeta;
		// if nothing is saved in Object datas #1
		// then break;
		if ((ImageSize = AObjGetDataSize(objectID, 1)) == 0)
				return;

		AMemSetSize(memID, ImageSize);
		metaBits = (BYTE *)AMemLock(memID);
		if (metaBits)
			{
			lpMeta = (BYTE *)AObjLockData(objectID,BMPMETADATA);
			CopyMemory(metaBits, lpMeta, ImageSize);
			AObjUnlockData(objectID,BMPMETADATA);
			AMemUnlock(memID);
			}
		else
			pErrMemLock(PERR_SETEMPTY);

		}
	}

/*************************************************************************
*       Function:                       SavePictFile
*       Description:    Called from SavePict function
*
*
*************************************************************************/
int PICTOBJECT::SavePictFile(
	LPSTR fName)

	{
	int rVal;
	// skj, 3-21-93 init from the datas
	// create a new pict object and delete it on the way out AUG 12, 1993
	// pObject = new PICTOBJECT(pictObject, NULL);

	if (ImageSize <= 0)
		{
		return 0;
    }

	if (pictData.pictType == METAPICT) 
		{
		// Create a MetaFile Handle from the info in
		// in the Datas
		HANDLE metaFile;
		DWORD cb;
    
    metaFile = CreateFile(fName, GENERIC_READ|GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (metaFile == INVALID_HANDLE_VALUE)
			rVal = 0;
		else 
			{    	
			rVal = (int)WriteFile(metaFile, (LPSTR)metaBits,(size_t)ImageSize, &cb, NULL);
        
			CloseHandle(metaFile);
			}
		}
	else {
		DWORD cb;
		HANDLE bitFile;

    bitFile = CreateFile(fName, GENERIC_READ|GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		
		if (bitFile == INVALID_HANDLE_VALUE)
			rVal = 0;
		else {
			rVal = (int)WriteFile(bitFile, (LPSTR) &(bmpFileHdr), sizeof(BITMAPFILEHEADER), &cb, NULL);
        
			rVal = WriteFile(bitFile, (LPSTR)&(bmpInfoHdr), sizeof(BITMAPINFOHEADER), &cb, NULL);
			long fileSize = tableSize + ImageSize;
			rVal = (int)WriteFile(bitFile, (LPSTR)rgbQuadAndBits, (size_t)fileSize, &cb, NULL);
			ADBG_ASSERT_E(rVal != -1, ;, "ERROR in Writing BMP to File");
			CloseHandle(bitFile);
		 }
		}

	return rVal;
  }

/***************************************************************************
// Function:  To Cut, Copy and Paste Picture Data to/from the CLipBoard
***************************************************************************/
void PICTOBJECT::CutCopyPict( int flag, HWND hWnd)

	{
	if (GetPictType() == BITMAPPICT)
		{
		// Save the BMP into DATA#2 so as to be able to do UNDO
    
		long size = tableSize + ImageSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		if (flag == IDM_PASTE) {
       
			// delete old picture handle
      // during object change
			PasteFromClipboard(hWnd); // this function will not create a
																	 // new handle
																	 // we will create a new handle during
				   // objectChange handling
			bSave = TRUE;
			}
		else if (flag == IDM_CUT) {
			bSave = TRUE;
			ImageSize = 0; // Set the Image to empty
			}
		I_UNUSED(size);
		}
	else {
		if (flag == IDM_PASTE) {
			PasteFromClipboard(hWnd);
			bSave = TRUE;
			}
		else if (flag == IDM_CUT)
			{
			ImageSize = 0;
			bSave = TRUE;
			}
		}
	}


/*************************************************************************
* Function:             BitmapToObject
* Description:  This function will set the datas of the picture object
*                           in a DIB format.  It will convert a METAFILE type picture to
*                                                               BMP type
*                                                       
* date:
*
*
**************************************************************************/
void PICTOBJECT::BitmapToObject(AMEMBLOCKID bufferID)

	{
	AEvtInfo tEvent;
	// Offset into the datas to point at the BITMAPINFOHEADER, followed by the
	// rgbQuadInfo and the bits
	// destination object needs to deleted and recreated on the next
	// entry to MDRT
	PICTOBJECT *destObject =  (PICTOBJECT *)GetPictObject(objectID);
	BMP *curPict;
	BITMAPFILEHEADER bmpFileHdr;
	BITMAPINFOHEADER bInfo;
	int tableSize;
	PICTDATA        *pictData = (PICTDATA *)AObjLockData(objectID, PICTDATA0);
	LPSTR lpBuffer;
	BYTE *lpImage;

	long size = AMemGetSize(bufferID);
  // if wrong format
	if (size == 0)
		{
		AObjUnlockData(objectID, PICTDATA0);
		return ;
    }
	lpBuffer = (LPSTR)AMemLock(bufferID);
	// if lpBuffer is NULL then ignore
	if (lpBuffer)
		{
		// Size the bmpmetaData to hold the passed in image and bitmapfileheader structure.
    // Data  --> BITMAPFILEHEADER
		// Image --> BITMAPINFOHEADER
		//                       --> RGBQUADANDBITS     --> RGBTABLE
		//                                                                                                --> BITS
		// NOV. 9, 1993
		AObjResizeData(objectID, BMPMETADATA, size + sizeof(BITMAPFILEHEADER));
		lpImage = (BYTE *)AObjLockData(objectID, BMPMETADATA);
		curPict  = (BMP  *)(lpImage);
		// take precautions for huge mem copy
		CopyMemory((LPSTR)&(curPict->bmpInfoHdr), lpBuffer, size);
		AMemUnlock(bufferID);
		bmpFileHdr.bfType = MS_BMTYPE; // 0x4d = M, 0x42 = 'B' "BM"
		bmpFileHdr.bfSize = sizeof(BITMAPFILEHEADER) + size;
		bmpFileHdr.bfReserved1 = 0;
		bmpFileHdr.bfReserved2 = 0;
		CopyMemory((LPSTR)&bInfo, (LPSTR)&curPict->bmpInfoHdr, sizeof(BITMAPINFOHEADER));
		tableSize = PaletteSize(&bInfo);
	
		bmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize;
		CopyMemory(&curPict->bmpFileHdr, &bmpFileHdr, sizeof(BITMAPFILEHEADER));
		pictData->pictType = BITMAPPICT;
		AObjUnlockData(objectID, BMPMETADATA);
		destObject->InitObjFieldsFromDatas();

		// Post Object Changed
		tEvent.sourceObject = objectID;
		tEvent.targetObject = objectID;
		tEvent.what = AEVENT_OBJECTCHANGED;
		tEvent.when = 0; //Tickcount;
		tEvent.how = AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
		tEvent.details = RECREATEHANDLE;
		tEvent.modifiers =  RECREATEHANDLE;  // recreate all picture handles

		AEvtPost(&tEvent);
		}

	AObjUnlockData(objectID, PICTDATA0);
	}


/*************************************************************************
* Function:    	ObjectToPicture
* Description:  This function will return the datas of the picture object
*               in a DIB format.  Currently a METAFILE type picture does not return
*     					anything.
* date:
*
*
**************************************************************************/
int PICTOBJECT::ObjectToBitmap(
	AMEMBLOCKID bufferID,
	BOOL bReturnAsBitmap) // bool indicating whether we want to return
													 // a metafile as bitmap
	{
	// Offset into the datas to point at the BITMAPINFOHEADER, followed by the
	// rgbQuadInfo and the bits

	BYTE * lpBuffer;
  int rVal;
	PICTDATA        *pictData = (PICTDATA *)AObjLockData(objectID, PICTDATA0);

	if (pictData->pictType == BITMAPPICT)
    {
		BYTE        *lpImage = (BYTE  *) AObjLockData(objectID, BMPMETADATA);
		BMP  *curPict = (BMP  *)(lpImage);
		long size = AObjGetDataSize(objectID, BMPMETADATA);
		size -= sizeof(BITMAPFILEHEADER);
		AMemSetSize(bufferID, size);
		lpBuffer = (BYTE *)AMemLock(bufferID);
		if (lpBuffer)
      {
			CopyMemory(lpBuffer, &curPict->bmpInfoHdr,size );
			rVal = 1;
			AMemUnlock(bufferID);
			}
		else
			AMemSetSize(bufferID, 0); // reset the buffer to 0

		if (lpImage)
		  AObjUnlockData(objectID, BMPMETADATA);
		}
	else
	  {
	   // currently I do not allow for conversion of a BITMAP
		 // to a METAFILE, because I do not have a window context
		 // render this metafile in (as a bitmap).
			
		if (bReturnAsBitmap)
			{
			rVal = 0;
      /*
			// Create a Bitmap from the META file handle. The metafile handle is
		  // for the MasterWindow DC
      long size;
		  HWND hitemWnd = oipGetMasterWindow();
		  HDC hDC;
		  lpBuffer = (LPSTR)AMemLock(bufferID);

		  HGLOBAL hBit = GlobalAlloc(1, GMEM_MOVEABLE);
		  PICTOBJECT *pExpObject = new PICTOBJECT((AOBJECTID) theObject, NULL);
		
		  // pVinst: mem block allocation
		  pExpObject->pVInst = (PICTURE_VINST *) new char[sizeof(PICTURE_VINST)];

		  pExpObject->pVInst->hPict = NULL;
	    pExpObject->bClientRect = TRUE;

		  hitemWnd = oipGetMasterWindow();

		  // set the flag so as to create a new handle
		  // Create a METAFILE handle.  The handle will be created to the
		  // DC of the MASTER oip window.  The picture is rendered in the
		  // entire area.
		  pExpObject->pVInst->hPaintWnd = hitemWnd;
		  pExpObject->pictData.bImageSaved = TRUE;
			hDC = GetDC(hitemWnd);
	    pExpObject->CreatePictH(hDC);
		  ReleaseDC(hitemWnd, hDC);


		  hEnhMetaFile_To_UBit(hBit, size,pExpObject->pVInst->hPict, sizeof(HANDLE));

		  // SKJ - destroy the picture handle
		  if (pExpObject->pVInst->hPict)
		    DeletePictHandle(pExpObject);

			delete pExpObject->pVInst;
			delete pExpObject;
     
		  AMemSetSize(bufferID, size);
		  lpBuffer = (LPSTR)AMemLock(bufferID);

		  LPSTR lpBits = GlobalLock(hBit);
      CopyMemory(lpBuffer, lpBits, size);
			GlobalUnlock(hBit);
			GlobalFree(hBit);
			*/
			}
		else {  // return as Metafile

			BYTE        *lpImage = (BYTE  *) AObjLockData(objectID, BMPMETADATA);
			long size = AObjGetDataSize(objectID, BMPMETADATA);
			AMemSetSize(bufferID,size );
			lpBuffer = (BYTE *) AMemLock(bufferID);
			if (lpBuffer)
				{
				CopyMemory(lpBuffer, lpImage, size);
				AMemUnlock(bufferID);
				rVal = 1;
				}
			else {
				AMemSetSize(bufferID, 0);
				rVal = 0;
				}

			AObjUnlockData(objectID, BMPMETADATA);
			}
		}

	AObjUnlockData(objectID, PICTDATA0);
	return rVal;
	}

/*************************************************************************
* Function:             SaveRunTimeData
* Description:  This function will write the fields of the PICTOBJECT class
*               instance to the datas of its picture object
* date:
*
*
**************************************************************************/
int PICTOBJECT::SaveRuntimeData()
	{
	BMP                  *curPict;
	
	// copy required info from the C++ class instance onto the serius
	// OBJECT datas.

	if (bSave == FALSE)
		{
		return 0;
		}

	// Object datas #1 should already be properly set
	// Code to copy Datas#1 from the Class instance onto to the Object Datas
	// get a ptr to #1 datas in Picture Object

	// Maybe put into the Destructor
	if (pictData.pictType == BITMAPPICT)
		{
		if (ImageSize > 0)
			{
			long                                    size = tableSize + ImageSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			AObjResizeData(objectID, BMPMETADATA, size);
			BYTE                *lpImage = (BYTE  *)AObjLockData(objectID, BMPMETADATA);
      
			curPict = (BMP  *)lpImage;

			// Copy the bitmap info to data#1
			CopyMemory((LPSTR)&curPict->bmpFileHdr, &bmpFileHdr, sizeof(BITMAPFILEHEADER));
			CopyMemory((LPSTR)&curPict->bmpInfoHdr, &bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			CopyMemory( (LPSTR)(curPict->rgbQuadAndBits), (LPSTR)rgbQuadAndBits,       tableSize);

			long            hugesize = ImageSize;
			lpImage = (BYTE  *)(curPict->rgbQuadAndBits + (int) tableSize);
			BYTE  *tmpBits = (BYTE *)((BYTE *)rgbQuadAndBits + tableSize);

			// copy the data to a local memory buffer--being careful with large blocks
			CopyMemory(lpImage, tmpBits, hugesize);
			AObjUnlockData(objectID, BMPMETADATA);

			}
		else
			{
			AObjResizeData(objectID, BMPMETADATA, 0);
			}
		}
	else
		{
		if (ImageSize > 0)
			{
			AObjResizeData(objectID, 1, ImageSize);
			BYTE * lpObjData = (BYTE  *)AObjLockData(objectID, BMPMETADATA);
			CopyMemory(lpObjData, metaBits, ImageSize);
			AObjUnlockData(objectID, BMPMETADATA);
			}
		else
			{
			AObjResizeData(objectID, BMPMETADATA, 0);
			}
		}

	AObjSetData(objectID, PICTDATA0, (LPSTR)&pictData, sizeof(PICTDATA));
	return 1; // saved
	}

/*************************************************************************
* Function:             ~PICTOBJECT
* Description:  Destructor for PICTOBJECT class
*
*
**************************************************************************/

PICTOBJECT::~PICTOBJECT()
	{
	BMP                  *curPict;
	// copy required info from the C++ class instance onto the serius
	// OBJECT datas.
	if (bSave == FALSE)
		{
		AMemFree(memID);
		return;
		}

	// Object datas #1 should already be properly set
	// Code to copy Datas#1 from the Class instance onto to the Object Datas
	// get a ptr to #1 datas in Picture Object

	// Maybe put into the Destructor
	if (pictData.pictType == BITMAPPICT)
		{
		if (ImageSize > 0)
			{
			long                                    size = tableSize + ImageSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			AObjResizeData(objectID, BMPMETADATA, size);
			BYTE                *lpImage = (BYTE  *)AObjLockData(objectID, BMPMETADATA);
      
			curPict = (BMP  *)lpImage;
			// Copy the bitmap info to data#1
			CopyMemory((LPSTR)&curPict->bmpFileHdr, &bmpFileHdr, sizeof(BITMAPFILEHEADER));
			CopyMemory((LPSTR)&curPict->bmpInfoHdr, &bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			CopyMemory( (LPSTR)&(curPict->rgbQuadAndBits), (LPSTR)rgbQuadAndBits,      tableSize);
			long            hugesize = ImageSize;
			lpImage = (BYTE *)(curPict->rgbQuadAndBits + (int) tableSize);
			BYTE  *tmpBits = (BYTE *)((LPSTR)rgbQuadAndBits + (int) tableSize);
			// copy the data to a local memory buffer--being careful with large blocks
			CopyMemory(lpImage, tmpBits, hugesize);
			AObjUnlockData(objectID, BMPMETADATA);
			}
		else
			{
			AObjResizeData(objectID, BMPMETADATA, 0);
			}
		}
	else
		{
		if (ImageSize > 0)
			{
			AObjResizeData(objectID, 1, ImageSize);
			BYTE * lpObjData = (BYTE *)AObjLockData(objectID, BMPMETADATA);
			CopyMemory(lpObjData, metaBits, ImageSize);
			AObjUnlockData(objectID, BMPMETADATA);
			}
		else
			{
			AObjResizeData(objectID, BMPMETADATA, 0);
			}
		}
	AObjSetData(objectID, PICTDATA0, (LPSTR)&pictData, sizeof(PICTDATA));
	// Free the SERIUS BLOCK that holds the BITMAP/ METAFILE during
	// buildTime
	AMemFree(memID);
	}

/*************************************************************************
* Function:             SaveMetaData
* Description:  This function will write metafile data to the Object datas
*
*
**************************************************************************/
struct TEMPHEADER
	{
  WORD w1;
  WORD w2;
  WORD w3;
  WORD w4;
  };

#define PLACEABLE 1
#define NORMAL		2
#define ENHANCED	3

int PICTOBJECT::SaveMetaData(
	int             fromWhere,  // flag indicating where to save METAFILE from -- i.e. CLIPBOARD or a FILE on disk
	LPSTR           fname)
	{
	int          			rVal = 1;
  DWORD 						cb;
  TEMPHEADER        th;
  int               type=0;
   
	if (fromWhere == SAVE_FROM_FILE)
		{
		HANDLE                     FILE_HANDLE;
		ENHMETAHEADER              metaHdr;
		HENHMETAFILE               enhMf=NULL;
    
    
    FILE_HANDLE = CreateFile(fname, GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (FILE_HANDLE == INVALID_HANDLE_VALUE)
      {
			rVal = 0;                                                       
			ImageSize = 0;
      return rVal;
      }
      
    //determine what kind of meta file we have
    //
    I_VERIFY(ReadFile(FILE_HANDLE, &th, sizeof(th), &cb, NULL));
    if(*((DWORD*)&th)	== 0x9ac0cdd7L)
    	type = PLACEABLE;
    else if(*((DWORD*)&th) == EMR_HEADER)
    	type = ENHANCED;
    else if((th.w1 == 0 || th.w1 == 1) && (th.w3 == 0x300))
    	type = NORMAL;
    
    CloseHandle(FILE_HANDLE);
      
    if(!type)
    	{
      ImageSize = 0;
      return 0;
      }
      
		// if it is a placeable metafile, then strip out 22 bytes before
		// trying to read in the file
		if (type == PLACEABLE || type == NORMAL)
			{
			//convert the standard metafile to the enhanced version
      HMETAFILE hmf = GetMetaFile(fname);
      I_ASSERT(hmf);
      METAHEADER* mh = (METAHEADER*)GlobalLock(hmf);
      LPSTR bufPtr = (LPSTR)GlobalAlloc(GPTR, mh->mtSize * 2);
      GetMetaFileBitsEx(hmf, mh->mtSize * 2, (LPVOID)bufPtr);
      enhMf = SetWinMetaFileBits(mh->mtSize * 2, (CONST BYTE*)bufPtr, NULL, NULL);
      I_ASSERT(enhMf);
      GlobalFree(bufPtr);
      GlobalUnlock(hmf);
      DeleteMetaFile(hmf);
			}
		else if(type == ENHANCED)
			{
      enhMf = GetEnhMetaFile(fname);
			I_ASSERT(enhMf);
			}
    else
    	I_ASSERT(FALSE);
      
    if(enhMf)
    	{
    	GetEnhMetaFileHeader(enhMf, sizeof(metaHdr), &metaHdr);
    	  
			AMemSetSize(memID, metaHdr.nSize); // new char[ImageSize];
			metaBits = (BYTE *)AMemLock(memID);
		
    	GetEnhMetaFileBits(enhMf, metaHdr.nSize, metaBits);
    
			ImageSize = metaHdr.nSize;
			AMemUnlock(memID);
			rVal = 1;
     	}
		}
  else
  	I_ASSERT(FALSE);
	return rVal;
	}


/***************************************************************************
*       Function: SaveBMP
*
*       Description: This function will save a BMP to object Datas
*
****************************************************************************/
int PICTOBJECT::SaveBMP(
	int   		                 fromWhere,
	HANDLE                     fileHandle)
	{
	int rVal = 0;
  DWORD cb;

	int nColorBits;
	if (fromWhere == SAVE_FROM_FILE)
		{
		int		fileindex;
		long  hugesize;
		ReadFile(fileHandle, (LPSTR)&(bmpFileHdr), sizeof(BITMAPFILEHEADER), &cb, NULL);
		if (bmpFileHdr.bfType != MS_BMTYPE) // 0x4d = M, 0x42 = B
			{
			ImageSize = 0;
			// if not right format then leave the old bitmap
			return 0;
      }
		fileindex = sizeof(BITMAPFILEHEADER);
		SetFilePointer(fileHandle, fileindex, NULL, FILE_BEGIN);
		ReadFile(fileHandle, (LPSTR)&(bmpInfoHdr), sizeof(BITMAPINFOHEADER), &cb, NULL);
		fileindex = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		SetFilePointer(fileHandle, fileindex, NULL, FILE_BEGIN);
		if(bmpInfoHdr.biBitCount<=8)
			{
			tableSize = PaletteSize(&bmpInfoHdr);
			}
		else
		tableSize = 0;

		fileindex =  (int) bmpFileHdr.bfOffBits;
		ImageSize =   bmpFileHdr.bfSize - fileindex;

		// certain drivers are not stuffing in right info. into
		// the BMPFileHeader
		long nColorBits = bmpInfoHdr.biBitCount * bmpInfoHdr.biPlanes;
		ImageSize = WIDTHBYTES((DWORD)bmpInfoHdr.biWidth* nColorBits) * bmpInfoHdr.biHeight;

		bmpInfoHdr.biSizeImage = ImageSize;
		bmpFileHdr.bfSize =  ImageSize + fileindex;

    hugesize = ImageSize;
		AMemSetSize(memID, tableSize + ImageSize); 
		rgbQuadAndBits = (BYTE *)AMemLock(memID);

		if (rgbQuadAndBits)
      {
			ReadFile(fileHandle, rgbQuadAndBits, tableSize, &cb, NULL);
			char * tmpBMP = (char *)(rgbQuadAndBits + tableSize);
			ReadFile(fileHandle, (char *)tmpBMP, (int)hugesize, &cb, NULL);
			AMemUnlock(memID);
			rVal = 1;
			}
		else {
			pErrMemLock(PERR_SETEMPTY);
			rVal = 0;
			}
		}
	else  // else if from a clipboard
		{
		HBITMAP                 hbm;
    BOOL        bfreePal = FALSE;

		if (IsClipboardFormatAvailable(CF_DIB))                 // SKJ, Aug. 3, 1994 -> to handle palette in clipboard
			{
			HANDLE hDIB =  (HANDLE)GetClipboardData(CF_DIB);
			BITMAP         	bmap;
			LPSTR           bits;
			HDC  					 	hDC;
			HPALETTE  			hOldPal;
			BYTE            *  lpCB;
			LPBITMAPINFO 		lpbmInfo;

      int numColors;
			ImageSize = GlobalSize(hDIB);
			if (ImageSize == 0)
				return 0;
			lpCB      = (BYTE *)GlobalLock(hDIB);
			lpbmInfo  = (LPBITMAPINFO )lpCB;
			// SKJ, the planes info should be gotten from the prev. GetDIBits call
			// 3/15/93
			CopyMemory(&bmpInfoHdr, lpbmInfo, sizeof(BITMAPINFOHEADER));

			tableSize = PaletteSize(&(lpbmInfo->bmiHeader));
			ImageSize = lpbmInfo->bmiHeader.biSizeImage;
			AMemSetSize(memID, ImageSize + tableSize); //new char[ImageSize + sizeof(BITMAP)];
			rgbQuadAndBits = (BYTE *)AMemLock(memID);
			if (rgbQuadAndBits)
				{
				CopyMemory(rgbQuadAndBits, (lpCB + sizeof(BITMAPINFOHEADER)), ImageSize + tableSize);
				pictData.pictType = BITMAPPICT;
				pictData.bClipboard= TRUE;

				bmpFileHdr.bfType = MS_BMTYPE; 				// 0x4d = M, 0x42 = 'B'
				bmpFileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ImageSize;
				bmpFileHdr.bfReserved1 = 0;
				bmpFileHdr.bfReserved2 = 0;
				bmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize;
				AMemUnlock(memID);
				}

			GlobalUnlock(hDIB);
			rVal = 1;
			}
		else if (IsClipboardFormatAvailable(CF_BITMAP))
			{
			HPALETTE  hPalCB = (HPALETTE)GetClipboardData(CF_PALETTE); // if to be saved from CLIPBOARD
			hbm = (HBITMAP)GetClipboardData(CF_BITMAP); // if to be saved from CLIPBOARD
			// SKJ Aug. 3, 1994
			HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
			if (!hPalCB)
				{
				int nPalSize    = GetDeviceCaps(hDC, SIZEPALETTE);
				hPalCB = (nPalSize==256) ? ::CreatePalette((LOGPALETTE*)&G_logpalette) : NULL;
				bfreePal = TRUE;
				}

			BITMAP 					bmap;
			LPSTR       		bits;
			LPBITMAPINFO  	lpbmInfo;
			HPALETTE        hOldPal;
			int 						numColors;
			AMEMBLOCKID			bmiMemID;
			::GetObject(hbm, sizeof(BITMAP), (LPSTR)&bmap);

			nColorBits = bmap.bmPlanes * bmap.bmBitsPixel;
			nColorBits = nColorBits <=1 ? 1 : nColorBits <=4 ? 4 : nColorBits <= 8 ? 8 : 24;
			tableSize = CalcNumberOfTableEntries(nColorBits) * sizeof(RGBQUAD);
			ImageSize = WIDTHBYTES((DWORD)bmap.bmWidth * nColorBits) * bmap.bmHeight;
			bmiMemID = AMemAllocate(sizeof(BITMAPINFO) + (int)tableSize);
			lpbmInfo  = (LPBITMAPINFO)AMemLock(bmiMemID);

			lpbmInfo->bmiHeader.biWidth = bmap.bmWidth;
			lpbmInfo->bmiHeader.biHeight = bmap.bmHeight;
			lpbmInfo->bmiHeader.biPlanes = 1;
			lpbmInfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
			lpbmInfo->bmiHeader.biBitCount = nColorBits;
			lpbmInfo->bmiHeader.biCompression = BI_RGB;
			lpbmInfo->bmiHeader.biXPelsPerMeter = 0;
			lpbmInfo->bmiHeader.biYPelsPerMeter = 0;
			lpbmInfo->bmiHeader.biSizeImage  = ImageSize;
			lpbmInfo->bmiHeader.biClrUsed    = 0; // numColors;
			lpbmInfo->bmiHeader.biClrImportant = 0; // numColors;;

			if (hPalCB)
				{
				hOldPal = SelectPalette(hDC, hPalCB, FALSE);
				RealizePalette(hDC);
				}

			AMEMBLOCKID      bitsID = AMemAllocate(ImageSize); // to allow for huge ptr
			bits = (LPSTR)AMemLock(bitsID);
			GetDIBits(hDC, hbm, 0, bmap.bmHeight, (LPSTR)bits, lpbmInfo, DIB_RGB_COLORS);

			AMemSetSize(memID, ImageSize + tableSize); //new char[ImageSize + sizeof(BITMAP)];
			rgbQuadAndBits = (BYTE *)AMemLock(memID);

			if (rgbQuadAndBits)
				{
				CopyMemory(&bmpInfoHdr, &(lpbmInfo->bmiHeader), sizeof(BITMAPINFOHEADER));
				CopyMemory(rgbQuadAndBits, lpbmInfo->bmiColors, tableSize);

				pictData.pictType = BITMAPPICT;
				pictData.bClipboard= TRUE;
				long        		hugesize = ImageSize;
				char *      lphugebits = (char *)bits;
				char *      tmpBMP = (char *)(rgbQuadAndBits + tableSize);

				// copy the data to a local memory buffer--being careful with large blocks
				CopyMemory(tmpBMP, lphugebits, hugesize);
				CopyMemory((LPSTR)rgbQuadAndBits, lpbmInfo->bmiColors, tableSize);

				bmpFileHdr.bfType = MS_BMTYPE; // 0x4d = M, 0x42 = 'B'
				bmpFileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize + ImageSize;
				bmpFileHdr.bfReserved1 = 0;
				bmpFileHdr.bfReserved2 = 0;
				bmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize;


				AMemUnlock(bmiMemID);
				AMemFree(bmiMemID);
				AMemUnlock(memID);

				}
			else
				{
				pErrMemLock(PERR_SETEMPTY);
				}

			AMemUnlock(bitsID);
			AMemFree(bitsID);
			rVal = 1;

			// SKJ Aug. 3, 1994
			if (hPalCB)
				{
				SelectPalette(hDC, hOldPal, FALSE);
				if (bfreePal)
					DeleteObject(hPalCB);
				}
			DeleteDC(hDC);
			}
		else
			{     // cannot paste a METAFILE
			rVal = NULL;
      }
    }    // end of If SAVE_FROM_CLIP

	return rVal;
	}

/***************************************************************************
*       Function: StretchBitmap
*
*       Description:  This function will create a bitmap of size (as laid out in the
*                                                               Window
*
***************************************************************************/
HANDLE PICTOBJECT::StretchBitmap(HDC hDC, RECT* lpDrawRect)
	{
	HANDLE 				hPicture;
	POINT     		ptSize;
	RECT          rect;
  int           xClient, yClient;
	POINT       	srcSize;
	BYTE   	*lpbits;
	BYTE *   	bmi;
	HDC         	hMemDC1;
	HDC         	hMemDC2;

	long bmpInfoSize = sizeof(BITMAPINFOHEADER) + (int)tableSize;
	lpbits = (BYTE *)(rgbQuadAndBits + tableSize);
	AMEMBLOCKID bmiMI = AMemAllocate(bmpInfoSize);
	bmi = (BYTE *)AMemLock(bmiMI);

	// bmi = (LPSTR)new char[(int)bmpInfoSize];
	CopyMemory((LPSTR)(bmi), (LPSTR)&bmpInfoHdr, sizeof(BITMAPINFOHEADER));
	CopyMemory((LPSTR)(bmi + sizeof(BITMAPINFOHEADER)), rgbQuadAndBits, tableSize);

	rect = *lpDrawRect;  // else drawRect was init by the WINDOW OBJECT
	xClient = rect.right - rect.left;
	yClient = rect.bottom - rect.top;

	if (ImageSize > 0) {
		if (pictData.pictStyle == SIZETOFITID)
			{
			ptSize.x = xClient;
		  ptSize.y = yClient;
			srcSize.x = (int)bmpInfoHdr.biWidth;
			srcSize.y = (int)bmpInfoHdr.biHeight;
			}
	  else if (pictData.pictStyle == ORIGPROPID)
			{
			float           prop;
			ptSize.x = xClient;
			ptSize.y = yClient;
			prop = (float)bmpInfoHdr.biWidth / (float)bmpInfoHdr.biHeight;
		  ptSize.y = ptSize.x / prop;
			if (ptSize.y > yClient)
			  {
			  ptSize.y = yClient;
			  ptSize.x = ptSize.y * prop; //ptSize.x / prop;
			  }
			srcSize.x = (int) bmpInfoHdr.biWidth;
			srcSize.y = (int)bmpInfoHdr.biHeight;

			}
	  else   // CROP
			{
			POINT mapBMP;
			mapBMP.x = (int) bmpInfoHdr.biWidth;
			mapBMP.y = (int)bmpInfoHdr.biHeight;
			srcSize.x = ptSize.x =  (int)((xClient < mapBMP.x) ? xClient : mapBMP.x);
			srcSize.y = ptSize.y =  (int)((yClient < mapBMP.y) ? yClient: mapBMP.y);

			}

		// Save the height and width of bitmap in device units
		pictData.height = ptSize.y;
		pictData.width = ptSize.x ;
		HBITMAP copyBmp  = CreateCompatibleBitmap (hDC, (int)bmpInfoHdr.biWidth, (int)bmpInfoHdr.biHeight) ; // Create a device -dependent bitmap
		// select the old palette back into the DC
		SetDIBits( hDC, copyBmp, 0,(int) bmpInfoHdr.biHeight, lpbits, (BITMAPINFO *)bmi , DIB_RGB_COLORS); // bUsePal?DIB_PAL_COLORS:DIB_RGB_COLORS) ;
		HBITMAP newBmp = CreateCompatibleBitmap(hDC, ptSize.x, ptSize.y);
		HBITMAP oldBmp1, oldBmp2;

		hMemDC1 = CreateCompatibleDC (hDC) ;
		int nPalSize    = GetDeviceCaps(hMemDC1, SIZEPALETTE);
		HPALETTE hpal1 = (nPalSize==256) ? ::CreatePalette((LOGPALETTE*)&G_logpalette) : NULL;
		HPALETTE hOldPal1 = 0;
		if (hpal1)
			{
			hOldPal1 = SelectPalette(hMemDC1, hpal1, FALSE);
			RealizePalette(hMemDC1);
			}

		hMemDC2 = CreateCompatibleDC (hDC) ;
		nPalSize    = GetDeviceCaps(hMemDC2, SIZEPALETTE);
		HPALETTE hpal2 = (nPalSize==256) ? ::CreatePalette((LOGPALETTE*)&G_logpalette) : NULL;
		HPALETTE hOldPal2 = 0;
		if (hpal2)
			{
			hOldPal2 = SelectPalette(hMemDC2, hpal2, FALSE);
			RealizePalette(hMemDC2);
			}

		oldBmp1 = (HBITMAP)SelectObject (hMemDC1, copyBmp); // device dependent copy of BMP
		oldBmp2 = (HBITMAP)SelectObject (hMemDC2, newBmp) ;
		StretchBlt(hMemDC2, 0, 0, ptSize.x, ptSize.y, hMemDC1, 0,0,(int)srcSize.x, (int)srcSize.y,
			SRCCOPY);
		SelectObject (hMemDC1, oldBmp2) ;
		SelectObject (hMemDC2, oldBmp1) ;

		if (hpal1)
			{
			SelectPalette(hMemDC1, hOldPal1, FALSE);
			DeleteObject(hpal1);
			}

		if (hpal2)
			{
			SelectPalette(hMemDC2, hOldPal2, FALSE);
			DeleteObject(hpal2);
			}

		DeleteDC(hMemDC1);
    DeleteDC(hMemDC2);
		DeleteObject(copyBmp);
		hPicture = newBmp;
		}

	AMemUnlock(bmiMI);
	AMemFree(bmiMI);
	// delete bmi;
	I_UNUSED(srcSize);
	return hPicture;
	}


/**************************************************************************



***************************************************************************/
HANDLE PICTOBJECT::CreatePictH(
	HDC                                                     hDC,
	LPRECT                                          lpRect)
	{
	HANDLE                                          hPicture = NULL;
	DWORD cb;
	if (!pictData.bImageSaved)
		return NULL;
	if (hPicture == NULL)
		{
		if (pictData.pictType == BITMAPPICT)
			{
			if ((rgbQuadAndBits == NULL)  || (ImageSize == 0)){
				hPicture  = NULL;
				return NULL;
				}
			// In Case of a Bitmap from ClipBoard we have tableSize = sizeof(BITMAP)
			// else it is the size of the RGBQUAD table
			// rgbQuadandBits contain this info followed by the BITS for image.
			if (pictData.bClipboard)
				{
				LPBITMAPINFO            lpbmInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO) + (int)tableSize];
				CopyMemory(lpbmInfo, &bmpInfoHdr, sizeof(BITMAPINFOHEADER));
				CopyMemory(lpbmInfo->bmiColors, rgbQuadAndBits, (int)tableSize);
				}
      // stretch the bitmap
			hPicture = StretchBitmap(hDC, lpRect);
			}
		else    // METAFILE
			{
			// Create a MetaFile Handle from the info in
			// in the Datas
			HANDLE                             metaFile;
			char                    fname[50];
			long      rVal;
			OFSTRUCT        ofStruct;
			if (ImageSize == 0)
				{
				return NULL;
				}
			HANDLE          hMemMeta = GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, ImageSize);
			BYTE *lpMetaData = (BYTE *)GlobalLock(hMemMeta);
			CopyMemory((LPSTR)lpMetaData, (LPSTR)metaBits, ImageSize);
			hPicture = SetEnhMetaFileBits(ImageSize, lpMetaData);   // ??
			GlobalUnlock(hMemMeta);
			GlobalFree(hMemMeta);
			// create a temporary meta file 'temp<task>'
			wsprintf(fname, "%u%u", objectID, GetCurrentProcess());
    	metaFile = CreateFile(fname, GENERIC_READ|GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			rVal = (long)WriteFile(metaFile, (LPSTR)metaBits, ImageSize, &cb, NULL);
			ADBG_ASSERT_E(rVal != FALSE, ;, "ERROR in Writing Meta to File");

			CloseHandle(metaFile);
			hPicture   = GetMetaFile(fname);  //
			I_UNUSED(rVal);
			}
		}
	return hPicture;
	}


/***************************************************************************
*
*
*
***************************************************************************/
void PICTOBJECT::DrawPicture (
	HWND                                    hWndParent,
	HDC                                             hDC,
	HANDLE                          hPict,
	RECT*                                   lpDrawRect)
	{

	if (hPict == NULL)
		return ;

	if (pictData.pictType == BITMAPPICT)
		{
    RECT rect;
		rect = *lpDrawRect;
		HDC hMemDC = CreateCompatibleDC(hDC);
		HANDLE hOld = SelectObject(hMemDC, hPict);
		// SKJ, added this to map from logical to Pixels in LOMETRIC mode
		// LPtoDP(hDC, (LPPOINT)&rect,2);
		BitBlt(hDC, rect.left,rect.top, rect.right - rect.left,rect.bottom - rect.top,
						hMemDC, 0,0, SRCCOPY);
		
	  //Don't delete the bitmap -- we'll need it again
		SelectObject(hMemDC, hOld);
		DeleteDC(hMemDC);
		}
	else
		{
		LoadMetaFile(hWndParent, hDC,lpDrawRect, hPict);
		}
	}


/***************************************************************************
*       Function: OpenFileHook
*
*       Centers dialog over VAB window.
*
***************************************************************************/

static
UINT		ALMCALLBACK OpenFileHook(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	wParam,
	LPARAM	lParam
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

/***************************************************************************
*       Function: InitializeFOpen
*
*       Calls CommDlg to use the FILEOPEN function.
*       Lets the user choose the BITMAP they want to load
*
***************************************************************************/
static char szFile[256] = "\0";
char szFilter[256];

int PICTOBJECT::InitializeFOpen(
	HWND                                    ghWnd)
	{
	char                                    *szDirName = new char[256];
	static char     szFileTitle[256];
	char          *sztmpFile  = new char[256];
	int                                     rVal = 1;

	HANDLE                                     FILE_HANDLE;
	OPENFILENAME    opn;
	BOOL          bRIGHTFORMAT = TRUE;
  LPSTR         lpext;
	int           size;

	ZeroMemory((LPVOID)&opn, sizeof(OPENFILENAME));
	if (pictData.pictType == BITMAPPICT)
		{
		lstrcpy(szFilter, "BITMAP FILES (*.bmp) *.bmp");
		lstrcpy(sztmpFile, "*.bmp");
		}
	else if (pictData.pictType == METAPICT)
		{
		lstrcpy(szFilter, "META FILES (*.wmf) *.wmf");
		lstrcpy(sztmpFile, "*.wmf");
		}
	
	lpext = strchr(szFilter, ')');
	size = lstrlen(szFilter);
	lpext[1] = '\0';
	szFilter[size + 1] = '\0';
	szDirName[0] = '\0';

  if (szFile[0] == '\0')
		{
		GetSystemDirectory(szDirName, sizeof(szDirName));
		lstrcpy(szFile, sztmpFile);
		}
	else
		{
		LPSTR lpOld;
		LPSTR lpTmp = new char[lstrlen(szFile) + 1];

		lstrcpy(lpTmp, szFile);
		lpOld = lpTmp;

		while ((lpTmp = strchr(lpTmp, '\\')) != NULL)
			{
			lpOld = lpTmp;
			lpTmp++;
			}

		szFile[lstrlen(szFile) - lstrlen(lpOld)] = '\0';
		lstrcpy(szDirName, szFile);
		lstrcpy(szFile, sztmpFile);

		delete lpTmp;
		}

	opn.lStructSize       = sizeof(OPENFILENAME);
	opn.hwndOwner         = (HWND)ghWnd;
	opn.lpstrFilter       = szFilter;
	opn.nFilterIndex      = 1L;
	opn.lpstrFile         = szFile;
	opn.nMaxFile          = (DWORD)sizeof(szFile);
	opn.lpstrFileTitle    = szFileTitle;
	opn.nMaxFileTitle     = 80; //MAXFILETITLELEN;
	opn.lpstrInitialDir   = szDirName; //gszBuffer;
	opn.lpstrTitle        = (LPSTR)"Load Picture";
	opn.Flags             = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY
													| OFN_NOCHANGEDIR | OFN_ENABLEHOOK;
	opn.lpfnHook          = OpenFileHook;

	rVal = GetOpenFileName(&opn);

	if (rVal == 0)
	{
		delete szDirName;
		delete sztmpFile;
		return 0;
		}
	else
		pictData.bImageSaved = TRUE;

	if (pictData.pictType == BITMAPPICT)
		{
    FILE_HANDLE = CreateFile(opn.lpstrFile, GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
      
		if (FILE_HANDLE == INVALID_HANDLE_VALUE) {
			rVal = 0;
			bRIGHTFORMAT = FALSE;
      }
		else
			{
			// Save the BMP into the Object Datas
			if (SaveBMP(SAVE_FROM_FILE, FILE_HANDLE)) {
	rVal = 1;
	;
				}
			else {
				MessageBox(ghWnd, "   Wrong BMP format   ","Error : PICTURE OBJECT", MB_OK);
				rVal = 0;
				bRIGHTFORMAT = FALSE;
				}
	    CloseHandle(FILE_HANDLE);
	    }
		}
	else
		{
		// Save the MetaFile into the Object datas
		if (SaveMetaData(SAVE_FROM_FILE, opn.lpstrFile )) {
	rVal = 1;
				;
				}
		else {
			MessageBox(ghWnd, "    Wrong Meta File format    ","Error : PICTURE OBJECT", MB_OK);
      rVal = 0;
			bRIGHTFORMAT = FALSE;
			}
		}
	I_UNUSED(bRIGHTFORMAT);

	delete sztmpFile;
	delete szDirName;
	return rVal;
	}

/***************************************************************************
*
*
*
***************************************************************************/
LRESULT CALLBACK  PictWndProc(
	HWND                                                            hWnd,
	UINT                                                message,
	WPARAM                                                            wParam,
	LPARAM                                                            lParam)
	{

	switch (message)
		{
		case WM_PAINT:
			{
      PAINTSTRUCT       ps;
			char                                    title[32];

			ATypeGetName((ATYPEID)OTYPE_PICTURE, (LPSTR)title, 32);
			BeginPaint(hWnd, &ps);
			AUtlSetTitleFont((ATYPEID)OTYPE_PICTURE, NULL, GetDlgItem(hWnd, ID_OBJECT));

			// serdlgDrawTitle(GetDlgItem(hWnd, ID_OBJECT), (LPSTR)title);
			EndPaint(hWnd, &ps);
			break;
			}

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

    default:
      return (DefWindowProc(hWnd, message, wParam, lParam));
		  }

  return (NULL);
  }


int PICTOBJECT::PasteFromClipboard(HWND hParent)
	{
	HANDLE                  newH;  // Handle to The Clipboard Data
	int        rVal = 0;
	OpenClipboard(hParent);

	if (SaveBMP(SAVE_FROM_CLIPBOARD, 0))
		{
		rVal = 1;
		}
	else if ((newH = GetClipboardData(CF_METAFILEPICT)) != NULL)
		{
		pictData.pictType = METAPICT;
		pictData.bClipboard= TRUE;

		SaveMetaData(SAVE_FROM_CLIPBOARD, (LPSTR)NULL );
	
    rVal = 1;
		}

	pictData.bImageSaved = TRUE;

	CloseClipboard();

	I_UNUSED(newH);
	return rVal;

	}



/***************************************************************************
*CONSTRUCTOR for PICTOBJECT Class
*The fields are init. from the Object datas
*BMPFILEHEADER field is build as needed
***************************************************************************/

PICTOBJECT::PICTOBJECT(
	AOBJECTID                objID,
	HWND        hWND)
	{
	LPSTR   lpData = (LPSTR) AObjLockData(objID, PICTDATA0);
	int     oldDataSize = (int)AObjGetDataSize(objID, PICTDATA0);
	int     newDataSize = sizeof(PICTDATA);

	// check added for version 2.0 April 1, 1993
	if ((newDataSize != oldDataSize) ||
		 ( ((PICTDATA *)lpData)->version != DATAVersion))
		{
		AObjUnlockData(objID, PICTDATA0);
		AObjResizeData(objID, PICTDATA0, sizeof(PICTDATA));
		lpData = (LPSTR) AObjLockData(objID, PICTDATA0);

		((PICTDATA*)lpData)->bFrame = FALSE;
		((PICTDATA *)lpData)->version = DATAVersion;

    }

	ZeroMemory(this, sizeof(PICTOBJECT));
	pictData =  *(PICTDATA  *)lpData;
	// Initialize the PictData field to
	// the data passed in.
	objectID = objID;
	memID = AMemAllocate(0);

	bClientRect = FALSE;
	if (pictData.pictType == BITMAPPICT)
		{
		BMP                  *lpBMP;
		metaBits = NULL;
		// if nothing is saved in Object datas #1
		// then break;
		if ((ImageSize = AObjGetDataSize(objID, 1)) == 0)
			{
			ZeroMemory(&bmpInfoHdr,sizeof(BITMAPINFOHEADER));
			AObjUnlockData(objID, PICTDATA0);
			return;
			}
		lpBMP = (BMP *)AObjLockData(objID,BMPMETADATA);

		if (pictData.bClipboard)
			{
			CopyMemory((LPSTR)&bmpFileHdr,(LPSTR)&lpBMP->bmpFileHdr, sizeof(BITMAPFILEHEADER));
			CopyMemory(&bmpInfoHdr,(LPSTR) &lpBMP->bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			tableSize = PaletteSize(&(lpBMP->bmpInfoHdr));

			// tableSize = pow(2, tableSize) * sizeof(RGBQUAD);   // size of RGB table

			int nColorBits =  (int)(bmpInfoHdr.biBitCount * bmpInfoHdr.biPlanes);
			// ImageSize = (bmpInfoHdr.biWidth / 8) * bmpInfoHdr.biHeight * ImageSize;
			ImageSize = bmpFileHdr.bfSize - bmpFileHdr.bfOffBits;
			ImageSize = WIDTHBYTES((DWORD)bmpInfoHdr.biWidth* nColorBits) * bmpInfoHdr.biHeight;
			AMemSetSize(memID, ImageSize + tableSize);
			rgbQuadAndBits = (BYTE *)AMemLock(memID);
			if (rgbQuadAndBits)
	{
				CopyMemory(rgbQuadAndBits,lpBMP->rgbQuadAndBits, ImageSize + tableSize);
				AMemUnlock(memID);
				}
			else {
				pErrMemLock(PERR_SETEMPTY);
				}
			}
		else
			{
			CopyMemory((LPSTR)&bmpFileHdr,(LPSTR)&lpBMP->bmpFileHdr, sizeof(BITMAPFILEHEADER));
			CopyMemory(&bmpInfoHdr,(LPSTR) &lpBMP->bmpInfoHdr, sizeof(BITMAPINFOHEADER));

			ImageSize = bmpFileHdr.bfSize - bmpFileHdr.bfOffBits;;
			// tableSize = bmpInfoHdr.biBitCount;
			tableSize = PaletteSize(&bmpInfoHdr); //  * sizeof(RGBQUAD);
			// tableSize = pow(2, tableSize) * sizeof(RGBQUAD);   // size of RGB table

			AMemSetSize(memID, ImageSize + tableSize);
			rgbQuadAndBits = (BYTE *)AMemLock(memID);

			if (rgbQuadAndBits)
	{
				long                    hugesize = ImageSize;
				CopyMemory((LPSTR)rgbQuadAndBits,(LPSTR) lpBMP->rgbQuadAndBits, (int)tableSize);
				BYTE *lpImage = (BYTE *)(lpBMP->rgbQuadAndBits + (int)tableSize);
				BYTE *tmpBits = (BYTE *)(rgbQuadAndBits + tableSize);
				// copy the data to a local memory buffer--being careful with large blocks
				CopyMemory(tmpBits,lpImage, hugesize);
				AMemUnlock(memID);
				}
			else
				{
				pErrMemLock(PERR_SETEMPTY);
				}
			}

		AObjUnlockData(objID,BMPMETADATA);
		}
	else {
		BYTE *              lpMeta;
		// if nothing is saved in Object datas #1
		// then break;

		if ((ImageSize = AObjGetDataSize(objID, 1)) == 0)
			{
			AObjUnlockData(objID, PICTDATA0);
			return;
      }
		AMemSetSize(memID, ImageSize); // new char[ImageSize];
		metaBits = (BYTE *)AMemLock(memID);
		if (metaBits)
      {
			lpMeta = (BYTE*)AObjLockData(objID,BMPMETADATA);
			CopyMemory(metaBits, lpMeta, ImageSize);
			AMemUnlock(memID);
			AObjUnlockData(objID,BMPMETADATA);
			}
		else{
			ImageSize = 0;
			AMemSetSize(memID, 0); // new char[ImageSize];
			}
		}
	AObjUnlockData(objID, PICTDATA0);
	I_UNUSED(hWND);
	}


HANDLE PICTOBJECT::LoadMetaFile(
	HWND                            hPaintWnd,
	HDC                             hDC,
	LPRECT                  lpDrawRect,
	HANDLE                  hPict)
	{
	RECT            rect;
	int             xClient;
  int           yClient;
	HDC                     thDC = hDC;
  WORD                  	oldMode;
  SIZE                   oldWExt, oldVExt;
	int                     debug = 1;
	POINT	  			          oldOrg;
	POINT 	     						oldVOrg;
	POINT                   point;


	/* if (bClientRect)
		{
		lprect = NULL;
		rect = *lpDrawRect;
    }
	else
		{
		rect = *lpDrawRect;  // else drawRect was init by the WINDOW OBJECT
    lprect = &rect;
		}  */
	rect = *lpDrawRect;  // else drawRect was init by the WINDOW OBJECT
	// lprect = &rect;

  xClient = rect.right - rect.left;
  yClient = rect.bottom - rect.top;

  point.x = rect.left;
  point.y = rect.top;

	LPtoDP(thDC, &point, 1);
	//  DPtoLP(thDC, &point, 1);

  // oldOrg = SetWindowOrgEx(thDC, point.x, point.y);

	
	if (debug)
		{
		oldMode = SetMapMode(thDC, MM_ANISOTROPIC);
    SetWindowExtEx(thDC, 100, 100, &oldWExt);
    SetViewportExtEx(thDC,xClient, yClient, &oldVExt);
 	  SetViewportOrgEx(thDC, point.x, point.y, &oldVOrg);
   }
  else
    oldMode = MM_TEXT;


  DPtoLP(thDC, &point, 1);
  SetWindowOrgEx(thDC, -point.x, -point.y, &oldOrg);

  /*
	if (bClientRect)
		{
    
		InvalidateRect(hPaintWnd, lprect, TRUE);
		UpdateWindow(hPaintWnd);
		} */

	PlayEnhMetaFile(thDC, (HENHMETAFILE) hPict, &rect);

  if(debug)
  	{
  	SetMapMode(thDC, oldMode);
	  SetWindowExtEx(thDC, oldWExt.cx, oldWExt.cy, NULL);
  	SetViewportExtEx(thDC, oldVExt.cx, oldVExt.cy, NULL);
		SetViewportOrgEx(thDC, oldVOrg.x, oldVOrg.y, NULL);
		}
    
	SetWindowOrgEx(thDC, oldOrg.x, oldOrg.y, NULL);

	I_UNUSED(hPaintWnd);
	return hPict;

	}




#pragma argsused
extern "C" BOOL WINAPI		DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	hInst = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
    	hInst = hInstDLL;
			if(!instCount)
				hBORDER = LoadBitmap(hInst, MAKEINTRESOURCE(BORDERID));
			
			instCount++;
			break;

		case DLL_PROCESS_DETACH:
			instCount--;
			if(!instCount)
      	{
				DeleteObject(hBORDER); // free the border bitmap
        hBORDER=NULL;
        }
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}



/****************************************************************************

		FUNCTION: PictureDlgProc(HWND, unsigned, WORD, LONG)

    PURPOSE: Let user select a file, and return.  Open code not provided.

****************************************************************************/

BOOL CALLBACK  PictureDlgProc(    // hDlg, message, wParam, lParam)
	HWND                            hDlg,
	UINT                message,
	WPARAM                            wParam,
	LPARAM                            lParam)
	{
	BOOL                            bProcessed = FALSE;
	PICTOBJECT                      *pObject;
	static HWND             hPaintWnd;
	static HANDLE           hPict = 0;
	static RECT                     ClientRect; // rect of picture window
	static AOBJECTID theObject;

	if (message != WM_INITDIALOG)
		{
		pObject = (PICTOBJECT*)GetOBJDPtr(hDlg);
		}

	switch (message)
		{
		case WM_INITDIALOG:
			{
			HWND      hitemWnd;
			char                    *name = new char[255];
			HDC       hDC;
			char                    title[20];
			theObject = (AOBJECTID)lParam;
			AObjGetName((AOBJECTID)lParam, (LPSTR)name, 80);
			SetWindowText(hDlg, (LPSTR)name);
			hitemWnd =  GetDlgItem(hDlg, PICTUREID);

			// pObject = new PICTOBJECT((AOBJECTID) lParam, hitemWnd);
			pObject = (PICTOBJECT*) AObjGetRuntimeInfo(theObject);
			// For a Dialog Box the hPaintWnd is init to
			// the static control handle in the description dlg template
			hPaintWnd = hitemWnd;
			GetClientRect(hPaintWnd, &ClientRect);


			// set the flag so as to create a new handle 3/26/93
			pObject->SetbSave(TRUE);
			hDC = GetDC(hitemWnd);
      hPict = 0;
			G_hPal = CreatePaletteHandle();  // Init the global Palette Handle
			ReleaseDC(hitemWnd, hDC);
			
			if (pObject->GetPictType() == BITMAPPICT)
      	Button_SetCheck(GetDlgItem(hDlg, BITMAPID), 1);
			else {
				Button_SetCheck(GetDlgItem(hDlg, METAFILEID), 1);

				EnableWindow(GetDlgItem(hDlg, ORIGPROPID), FALSE);
				EnableWindow(GetDlgItem(hDlg, CROPID), FALSE);
				}
			Button_SetCheck(GetDlgItem(hDlg, pObject->GetPictStyle()), 1);
			if (pObject->IsSelectable())
      	Button_SetCheck(GetDlgItem(hDlg, SELECTABLEID), 1);

      // SKJ, maintenance release March 23'93
			if (pObject->IsFramed())
				Button_SetCheck(GetDlgItem(hDlg,BORDERID), 1);
			// Copy the PICTURE object into the object datas
			SetOBJDPtr(hDlg, theObject, (LONG)pObject);
			ATypeGetName((ATYPEID)OTYPE_PICTURE, (LPSTR)title, 32);
			AUtlSetTitleFont((ATYPEID)OTYPE_PICTURE, title,  GetDlgItem(hDlg, ID_OBJECT));
			AUtlCenterDialog(hDlg, 0);
      delete name;
			}
	    return TRUE;

		case WM_DRAWITEM:
			{
			LPDRAWITEMSTRUCT di;

			/* Set the cursor to the hourglass and save the previous cursor. */
			HICON hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

			if (wParam == PICTUREID)
				{

				di = (LPDRAWITEMSTRUCT)lParam;
				HDC      hDC=   di->hDC;
				HPALETTE                hpalOld;
				// if a picture exists then delete the handle and
			// recreate the handle to the Picture           
				// skj, 3.24.93
				// Now, draw the items that need to be drawn
				if (G_hPal != 0)
					{
					hpalOld = ::SelectPalette(hDC,G_hPal, TRUE);
					RealizePalette(hDC);  
					}

				if (!hPict)
					{
					hPict = pObject->CreatePictH(hDC, &ClientRect);
					}

				if (hPict) {
					pObject->DrawPicture(hPaintWnd, hDC,hPict, &ClientRect);
					}
				else {  // else just paint the background
					LOGBRUSH lb;
					lb.lbStyle = BS_SOLID;
					lb.lbColor = GetSysColor(COLOR_WINDOW);
					lb.lbHatch = HS_VERTICAL;

					HBRUSH hBrush = CreateBrushIndirect(&lb);
					FillRect(hDC, &ClientRect, hBrush);
					DeleteObject(hBrush);

					break; //return;
					}
				
				if (G_hPal)
					{
					::SelectPalette(hDC, hpalOld, TRUE);
				  RealizePalette(hDC);
					}
				}

			/* Restore the previous cursor. */
			SetCursor(hcurSave);

      }
			break;

		case WM_PALETTECHANGED:
			// if the palette was realized by this window itself, then ignore it
			if ((HWND) wParam == hDlg)
	return 0;

    /* Otherwise, fall through to WM_QUERYNEWPALETTE. */

    case WM_QUERYNEWPALETTE:

      /*
       * If realizing the palette causes the palette to change,
       * redraw completely.
			 */
			// else fall through
      if (G_hPal)
				{
				HDC       hdc     = GetDC(hPaintWnd);
	HPALETTE  hpalOld = SelectPalette(hdc, G_hPal, FALSE);
				BOOL      lRet = RealizePalette(hdc);
	SelectPalette(hdc, hpalOld, TRUE);
	RealizePalette(hdc);
	ReleaseDC(hPaintWnd, hdc);

	if(lRet)                            
	  InvalidateRect(hPaintWnd, NULL, TRUE);
				bProcessed = TRUE;
	}
			break;

		case WM_COMMAND: //!!PORT!!
			switch (GET_WM_COMMAND_ID(wParam, lParam))
				{
				case IDHELP:
					WinHelp(hDlg, "vabhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Pict);
					break;

				case PASTEID:
					{
					HDC             hDC;
					HANDLE oldhPict = hPict;
					InvalidateRect(hPaintWnd, NULL, TRUE);
					hDC = GetDC(hPaintWnd);
					if (pObject->PasteFromClipboard(hPaintWnd))
						{
	    // delete old handle
						if (DeleteObject(oldhPict) == FALSE)
							DeleteMetaFile((HMETAFILE)oldhPict);
						hPict = NULL;
						}
					UpdateWindow(hPaintWnd);
					// pObject->DrawPicture(hPaintWnd, hDC, hPict, &ClientRect);
					ReleaseDC(hPaintWnd, hDC);
					}
					break;

				case LOADFILEID: {
					if (SendDlgItemMessage(hDlg, BITMAPID, BM_GETCHECK, 0, 0L))
	    pObject->SetPictType(BITMAPPICT);
					else																	
						pObject->SetPictType(METAPICT);

					// SKJ 3-29-93
					if (hPict)
						pObject->DeletePHandle(hPict);

	  // Create a new picture handle only if we successfully load in a new file
					if (pObject->InitializeFOpen(hDlg))
						{
						pObject->SetbClipboard(FALSE);
					hPict = NULL;
						InvalidateRect(hPaintWnd, NULL, TRUE);
						UpdateWindow(hPaintWnd);
						}
					break;
					}

				case SELECTABLEID :
					break;

				case BORDERID:
					break;

				case BITMAPID:
					{
					if ( Button_GetCheck(GetDlgItem(hDlg, GET_WM_COMMAND_ID(wParam, lParam))))
						{
						// if already a bitmap type then return;
						if (pObject->GetPictType() == BITMAPPICT)
						  break;

						EnableWindow(GetDlgItem(hDlg, ORIGPROPID), TRUE);
						EnableWindow(GetDlgItem(hDlg, CROPID), TRUE);
						if (hPict != NULL)
							DeleteMetaFile((HMETAFILE)hPict);
						hPict = NULL;
						// Initialise the ImageSize to 0
						pObject->SetImageSize(0);
						pObject->SetPictType( BITMAPPICT);
						InvalidateRect(hPaintWnd, NULL, TRUE);
						}
					}
					break;

				case METAFILEID  : {
					if ( Button_GetCheck(GetDlgItem(hDlg, GET_WM_COMMAND_ID(wParam, lParam))))
						{
						// if already a bitmap type then return;
						if (pObject->GetPictType() == METAPICT)
						  break;

					
						if (hPict != NULL)
							DeleteObject(hPict);
						hPict = NULL;

						// Initialise the ImageSize to 0
						pObject->SetImageSize(0);

						InvalidateRect(hPaintWnd, NULL, TRUE);
						Button_SetCheck(GetDlgItem(hDlg, pObject->GetPictStyle()), 0);
						pObject->SetPictStyle(SIZETOFITID);
						pObject->SetPictType(METAPICT);
						Button_SetCheck(GetDlgItem(hDlg, SIZETOFITID), 1);
						EnableWindow(GetDlgItem(hDlg, ORIGPROPID), FALSE);
						EnableWindow(GetDlgItem(hDlg, CROPID), FALSE);
						}
					}
				break;

				case CROPID:
				case ORIGPROPID:
				case SIZETOFITID:
					{
					if ( Button_GetCheck(GetDlgItem(hDlg, GET_WM_COMMAND_ID(wParam, lParam))))
						{
						int     oldpictStyle = pObject->GetPictStyle();

						pObject->SetPictStyle( GET_WM_COMMAND_ID(wParam, lParam));
						if (pObject->GetPictStyle() != oldpictStyle)
							{
							pObject->DeletePHandle(hPict);
							hPict = NULL;
							InvalidateRect(hPaintWnd, NULL, TRUE);
							UpdateWindow(hPaintWnd);
							}
						}
					}
					break;

				case PICTUREID:
					break;

				case IDOK:
					{
					char                    fname[50];
					// Save Bitmap or MetaFile in Object Data
					pObject->SetSelectable((BOOL)Button_GetCheck(GetDlgItem(hDlg, SELECTABLEID)));
					pObject->SetFrame((BOOL)Button_GetCheck(GetDlgItem(hDlg, BORDERID)));

					// SKJ - destroy the picture handle
					if (hPict)
						pObject->DeletePHandle(hPict);

					if (pObject->GetImageSize() == 0) {
						hPict = 0;
						}
					// So that we can save the picture datas
					// in the DESTRUCTOR,
					pObject->SetbSave(TRUE);
					// delete the temp file created for MEta operation
					if (pObject->GetPictType() == METAPICT)
	    {
						wsprintf(fname, "%d%d", theObject, GetCurrentProcess());
			      DeleteFile(fname);
						}
	  // Save c++ obj to datas
					pObject->SaveRuntimeData();
					// Destroy the palette
					DestroyPalette(G_hPal);
					// delete pObject; // delete during sleep
					EndDialog(hDlg, TRUE);
					return (TRUE);
					}

				case IDCANCEL:
					{
					pObject->SetbSave(FALSE);
					// SKJ - destroy the picture handle
					if (hPict)
						pObject->DeletePHandle(hPict);
					// delete the temp file created for MEta operation
					if (pObject->GetPictType() == METAPICT)
						{
						char fname[20];
			      wsprintf(fname, "SER%d", GetCurrentProcess());
			      I_VERIFY(DeleteFile(fname));
						}
					// Rollback to old datas without saving
					// Init the C++ object with the old datas
					pObject->InitObjFieldsFromDatas();

					// Destroy the palette
					DestroyPalette(G_hPal);

					EndDialog(hDlg, FALSE);
					return (FALSE);
					}
				}
				break;
		}
	return bProcessed;
	}



