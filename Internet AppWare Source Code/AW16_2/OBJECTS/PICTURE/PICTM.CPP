/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
// Date:		Aug.28, 1993
//
// AUTHOR:Shailendra Jain
//
// DESCRIPTION: PICTURE OBJECT
//
// CHANGES:
// ObjectToBitmap:
// BitmapToObject: In order to test these 2 event-handling, I have coded a
//								"GetSetPicture" function. Compile this DLL with PICTUREDEBUG defined
//								in order to access this function
//
//
//////////////////////////////////////////////////////////////*/


/*************************************************************************/
// to force strict compile
#define STRICT
#define ASTRICT
#define ADGB_DEBUG

#include "a_alm.h"
#include "windows.h"
#include <math.h>
#include <mem.h>
#include "stdlib.h"
#include "picture.h"
#include "print.h"

#define MEMFLAGS GMEM_MOVEABLE
// WIDTHBYTES takes # of bits in a scan line and rounds up to nearest
//  word.

#define WIDTHBYTES(bits)      (((bits) + 31) / 32 * 4)



extern HANDLE hBORDER ; // handle to border brush bitmap
extern HANDLE hInst;
extern WORD PaletteSize (VOID far *pv);
extern HPALETTE CreatePaletteHandle();
extern HPALETTE CreateDIBPaletteHandle(
	BITMAPINFOHEADER *lpInfo,
	MEMBLOCKID memID
	);
extern void DestroyPalette(HPALETTE hpal);



// function to compare the size of two rects
BOOL EqualSizeRect(LPRECT srcRect, LPRECT rect);



#if (WINVER < 0x030a)
extern void hmemcpy(void _huge*, const void _huge*, long);

#endif


#define DELETEALLHPICT -1 // flag to force deletion of all visual picture
                          // handles


/**********************************************************
*
*
*
**********************************************************/
LPLONG GetPictObject(OBJECTID objID)
	{
	LPLONG  lpContext = NULL;
	lpContext =  (LPLONG)AObjGetRuntimeInfo (objID);
	return lpContext;
	}

// BITMAP_To_UBit translation function /////////////////////////////////////

long	BITMAP_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	BITMAP		bitmap,
	HBITMAP		hBmp)

	{
	WORD	nColorBits = bitmap.bmBitsPixel*bitmap.bmPlanes;
	if(nColorBits!=1 && nColorBits!=4 && nColorBits!=8 && nColorBits!=24)
    return 0;


	long	ColorTableSize;
	if(nColorBits==24)
		{
		// 24-bit color -- no color table
		ColorTableSize = 0;
		}
	else
		ColorTableSize = pow(nColorBits, 2);

	univSize = sizeof(BITMAPINFOHEADER) + ColorTableSize*sizeof(RGBQUAD);
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == 0)
		return 0;

	BITMAPINFOHEADER* bmpIH = (BITMAPINFOHEADER*)GlobalLock(hUniv);
	if(!bmpIH)
  	return 0;

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
	if((hUniv = GlobalReAlloc(hUniv, univSize, MEMFLAGS)) == 0)
		return 0;
	bmpIH = (BITMAPINFOHEADER*)GlobalLock(hUniv);
	if(!bmpIH)
		return 0;

	void*	bits = (void*)((BYTE*)bmpIH + (size_t)(ColorTableSize*sizeof(RGBQUAD) + sizeof(BITMAPINFOHEADER)));

	 // fill in the bitmap data -- copies the bits automatically
	GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, (char*)bits, (LPBITMAPINFO)bmpIH, DIB_RGB_COLORS);

  // free the resources
  if(!hBmp)
	  DeleteObject(hBitmap);
	DeleteDC(hDC);

	GlobalUnlock(hUniv);
	return 1; 
	}

/////////////// hMetaFile_To_UBit translation function /////////////////////
//
// Borrowed from UPSF code
//
//////////////////////////////////////////////////////////////////////////// 
long	hMetaFile_To_UBit(
	HGLOBAL&	hUniv,
	long&			univSize,
	HMETAFILE	hMetaFile,
	long			natvLen)

	{
	if(natvLen != (long)sizeof(HMETAFILE))
    return 0;
		

	HDC	hDC = CreateIC("DISPLAY", NULL, NULL, NULL);
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES));
	SelectObject(hDCMem, hBitmap);

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
		return 0;
    }

	bitmap.bmBits = GlobalLock(hBits);
	if(!bitmap.bmBits)
		{
  	DeleteObject(hBitmap);
		return 0;
    }

	long ret = BITMAP_To_UBit(hUniv, univSize, bitmap, hBitmap);

	// clean up and exit
  DeleteObject(hBitmap);
	GlobalUnlock(hBits);
	GlobalFree(hBits);  
	return ret;
	}


/*************************************************************************
* Init the Picture Object.  Create a C++ class instance (PICT_OBJECT)
*	and save the object instance in the objectRuntime info
*
*
*
*************************************************************************/
void InitPictObject(OBJECTID objID)
	{
	LPLONG lpContext;
	PICTOBJECT *pObject;
	// for each picture Object create a PICTOBJECT instance
	// and stuff it in the object Runtime Datas
	// 8/22/93 maintenance release

	long rVal = AObjSetRuntimeInfo(objID, (LONG)0L);
	lpContext = GetPictObject(objID);
	pObject = (PICTOBJECT FAR*)lpContext;    // if contextH is NULL set PObject to NULL
	if (pObject == NULL )
		{
		pObject = new PICTOBJECT(objID,  NULL);
		AObjSetRuntimeInfo(objID, (LONG)pObject);
		}

	I_UNUSED(rVal);
	}




// function to compare the size of two rects
BOOL EqualSizeRect(LPRECT srcRect, LPRECT rect)
	{


	if ( ((srcRect->bottom - srcRect->top) == (rect->bottom - rect->top))
		&& ((srcRect->right - srcRect->left) == (rect->right - rect->left)) )
		return TRUE;

  return FALSE;
	}



// SKJ Added this on Mar. 29,1995
// to handle color printing

// =================================================================


//---------------------------------------------------------------------
//
// Function:   InitBitmapInfoHeader
//
// Purpose:    Does a "standard" initialization of a BITMAPINFOHEADER,
//             given the Width, Height, and Bits per Pixel for the
//             DIB.
//
//             By standard, I mean that all the relevant fields are set
//             to the specified values.  biSizeImage is computed, the
//             biCompression field is set to "no compression," and all
//             other fields are 0.
//
//             Note that DIBs only allow BitsPixel values of 1, 4, 8, or
//             24.  This routine makes sure that one of these values is
//             used (whichever is most appropriate for the specified
//             nBPP).
//
// Parms:      lpBmInfoHdr == Far pointer to a BITMAPINFOHEADER structure
//                            to be filled in.
//             dwWidth     == Width of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//             dwHeight    == Height of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//             nBPP        == Bits per Pixel for the DIB.
//
//
//---------------------------------------------------------------------

void InitBitmapInfoHeader (LPBITMAPINFOHEADER lpBmInfoHdr,
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

	 lpBmInfoHdr->biBitCount  = nBPP;
	 lpBmInfoHdr->biSizeImage = WIDTHBYTES (dwWidth * nBPP) * dwHeight;
}


//---------------------------------------------------------------------
//
// Function:   FindDIBBits
//
// Purpose:    Given a pointer to a DIB, returns a pointer to the
//             DIB's bitmap bits.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

LPSTR FindDIBBits (LPSTR lpbi)
{
   return (LPSTR)(lpbi + *(LPDWORD)lpbi + PaletteSize (lpbi));
}






//---------------------------------------------------------------------
//
// Function:   BitmapToDIB
//
// Purpose:    Given a device dependent bitmap and a palette, returns
//             a handle to global memory with a DIB spec in it.  The
//             DIB is rendered using the colors of the palette passed in.
//
//             Stolen almost verbatim from ShowDIB.
//
// Parms:      hBitmap == Handle to device dependent bitmap compatible
//                        with default screen display device.
//             hPal    == Palette to render the DDB with.  If it's NULL,
//                        use the default palette.
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

HANDLE BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal)
{
   BITMAP             Bitmap;
   BITMAPINFOHEADER   bmInfoHdr;
   LPBITMAPINFOHEADER lpbmInfoHdr;
   LPSTR              lpBits;
   HDC                hMemDC;
   HANDLE             hDIB;
   HPALETTE           hOldPal = NULL;

      // Do some setup -- make sure the Bitmap passed in is valid,
      //  get info on the bitmap (like its height, width, etc.),
      //  then setup a BITMAPINFOHEADER.

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


      // Now, we need a DC to hold our bitmap.  If the app passed us
      //  a palette, it should be selected into the DC.

   hMemDC       = GetDC (NULL);

   if (hPal)
      {
      hOldPal = SelectPalette (hMemDC, hPal, FALSE);
      RealizePalette (hMemDC);
      }

			// We're finally ready to get the DIB.  Call the driver and let
			//  it party on our bitmap.  It will fill in the color table,
			//  and bitmap bits of our global memory block.

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
			hDIB = NULL;
			}
	 else
			GlobalUnlock (hDIB);


			// Finally, clean up and return.

	 if (hOldPal)
			SelectPalette (hMemDC, hOldPal, FALSE);

	 ReleaseDC (NULL, hMemDC);

	 return hDIB;
}





//////////////////////////////////////////////////////
void PICTOBJECT::PrintPicture(
	HWND 		hWnd,
	HDC 		hItemDC,
	LPRECT 	lpRect,
	HANDLE *lpHPict
	)
	{
	HANDLE hPicture;

	if (GetPictType() == BITMAPPICT)
		{
		HWND hDIBWnd = hWnd;

		if (!hDIBWnd)
			 return ;

		// for a BITMAP picture, we need to save the old picture handle
		// and create a new one (because we have a new dc).  This bitmap is
		// then stretched to fit into the drawing rect.
		// At the end of printing we need to delete the new handle and
		// reinit to old handle
    HPALETTE hPal = CreateDIBPaletteHandle(&bmpInfoHdr, memID);
		hPicture = BitmapToDIB((HBITMAP)*lpHPict, hPal);

		BOOL        bUseBanding, bUse31APIs;
		WORD        wUnits;

		wUnits      = UNITS_PIXELS;
		bUseBanding = FALSE;  // cannot do banding because print object is calling
														// NEWFRAME Escape.
		bUse31APIs  = TRUE;

		if (hPicture)
			{
			DWORD dwError;
				// No Error check installed at this point.
				// SKJ Mar. 1, 1995
				// Fixed problems related to PS printer drivers.  The following function
				// Will Create a DIB and send it to the printer
				// Ref: DOCERR: Bad Printing Code Example in Guide to Programming
				// 			MSDNCD 10


			dwError = DIBPrint (    hItemDC,
																		hPicture,
																		lpRect,
																		wUnits,
																		SRCPAINT,
																		bUseBanding,
																		bUse31APIs);

			// ShowPrintError (hWnd, dwError);

			GlobalFree(hPicture);
			I_UNUSED(dwError);
			}

		DestroyPalette(hPal);
		}
	else
		{  // METAFILE
		DrawPicture (hWnd, hItemDC,hPicture,lpRect);
		}
	I_UNUSED(lpHPict);

	if (IsFramed())
	  {	
		HBRUSH 		hBrush = CreateSolidBrush(RGB(0,0,0)); // CreateHatchBrush(HS_CROSS,RGB(0,0,0));
					 
		FrameRect(hItemDC, lpRect, hBrush);
		DeleteObject(hBrush);
		}
	}

void PICTOBJECT::EditCut(HWND hWnd, LPRECT lpRect)
	{
	AEvtInfo   tEvent;

	HDC hItemDC  = GetDC(hWnd);
	int mode = GetMapMode(hItemDC);
	int sDC = SaveDC(hItemDC);
	mode = SetMapMode(hItemDC,MM_TEXT);

	HDC hDCMem = CreateCompatibleDC(hItemDC);
	int xLen  = lpRect->right - lpRect->left;
	int yLen  = lpRect->bottom - lpRect->top;
	HANDLE hBitmap = CreateCompatibleBitmap(hItemDC,xLen, yLen);
	
	if (hBitmap)
		{
		SelectObject(hDCMem, hBitmap);
		// remove the frame around
		if (pictData.bFrame)
			{	
			HBRUSH 		hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			FrameRect(hItemDC, lpRect, hBrush);
			DeleteObject(hBrush);
			}
		StretchBlt(hDCMem, 0, 0, xLen, yLen, hItemDC, lpRect->left, lpRect->top,
										 xLen, yLen, SRCCOPY);
		OpenClipboard(hWnd);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, hBitmap);

		CloseClipboard();
		// redraw the frame
		if (pictData.bFrame)
			{	
			HBRUSH 		hBrush = CreateSolidBrush(RGB(0,0,0)); // CreateHatchBrush(HS_CROSS,RGB(0,0,0));
					 
			FrameRect(hItemDC, lpRect, hBrush);
			DeleteObject(hBrush);
			}
		}
	CutCopyPict(IDM_CUT, hWnd);
	// Save the new runtime data into object datas
	SaveRuntimeData();
	DeleteDC(hDCMem);

	SetMapMode(hItemDC,mode);
	RestoreDC(hItemDC, sDC);
	ReleaseDC(hWnd, hItemDC);

	tEvent.sourceObject = objectID;
	tEvent.targetObject = tEvent.sourceObject;
	tEvent.what = AEVENT_OBJECTCHANGED;
	tEvent.when = 0 ;
	tEvent.details = RECREATEHANDLE;
	tEvent.modifiers =  RECREATEHANDLE;  // recreate all picture handles
	tEvent.how = AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
				
	AEvtPost(&tEvent);


	}

/***************************************************************************
*
*
*
*
***************************************************************************/
BOOL PICTOBJECT::EditPaste(HWND hWnd, LPRECT lpRect)
	{
	// Check to see if there is data on clipboard
	// skj, 3/21/93
	AEvtInfo tEvent;
	BOOL rVal = TRUE;
	BOOL clipData = FALSE;
	OpenClipboard(hWnd);
	if (IsClipboardFormatAvailable(CF_DIB)||
			IsClipboardFormatAvailable(CF_BITMAP)||
			IsClipboardFormatAvailable(CF_METAFILEPICT))
		{
		clipData = TRUE;

		}
	CloseClipboard();
	if (clipData == FALSE)
		return FALSE;
				
	// Paste image from Clipboard onto Picture Object
	// We do not need to reconstruct the picture handle as it
	// will be done in the PasteFromClip function in Pictdesc module
	
	HDC hItemDC = GetDC(hWnd);
	int mode = GetMapMode(hItemDC);
	int sDC = SaveDC(hItemDC);
	// 	mode = SetMapMode(hItemDC,MM_TEXT);

	// Erase the background Rect first
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = GetSysColor(COLOR_WINDOW);
	lb.lbHatch = HS_VERTICAL;
	HBRUSH hBrush = CreateBrushIndirect(&lb);
	FillRect(hItemDC,lpRect, hBrush);

	mode = SetMapMode(hItemDC,MM_TEXT);
	DeleteObject(hBrush);
	CutCopyPict(IDM_PASTE, hWnd);
	// Save the runtime data back into the datas before assigning
	SaveRuntimeData();

	SetMapMode(hItemDC,mode);
	RestoreDC(hItemDC, sDC);
	ReleaseDC(hWnd, hItemDC);

	bSave = TRUE;
	tEvent.sourceObject = objectID;
	tEvent.targetObject = objectID;
	tEvent.what = AEVENT_OBJECTCHANGED;
	tEvent.when = 0; //Tickcount;
	tEvent.details = 0;
	tEvent.modifiers =  RECREATEHANDLE;  // recreate all picture handles
	tEvent.how = AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
	// tEvent.private = 0;
	AEvtPost(&tEvent);


  return rVal;
	}

/***************************************************************************
*
*
*
*
***************************************************************************/
void PICTOBJECT::EditCopy(HWND hWnd, LPRECT lpRect)
	{
	HDC hItemDC  = GetDC(hWnd);
	int mode = GetMapMode(hItemDC);
	int sDC = SaveDC(hItemDC);
	mode = SetMapMode(hItemDC,MM_TEXT);

	HDC hDCMem = CreateCompatibleDC(hItemDC);
	int xLen  = lpRect->right - lpRect->left;
	int yLen  = lpRect->bottom - lpRect->top;
	HANDLE hBitmap = CreateCompatibleBitmap(hItemDC,xLen, yLen);
	if (hBitmap) {
		SelectObject(hDCMem, hBitmap);
		StretchBlt(hDCMem, 0, 0, xLen, yLen, hItemDC, lpRect->left, lpRect->top,
										 xLen, yLen, SRCCOPY);
		OpenClipboard(hWnd);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, hBitmap);
		CloseClipboard();
		}
	CutCopyPict(IDM_COPY,hWnd);

	DeleteDC(hDCMem);

	SetMapMode(hItemDC,mode);
	RestoreDC(hItemDC, sDC);

	ReleaseDC(hWnd, hItemDC);

	}


/*************************************************************************/
/* This is the MDRT routine for the Picture Object
   This is the common entry point for all the events from the shell
	 */
extern "C" void FAR PASCAL _export mdrtProc(
	pAEvtInfo theEvent,
	pAObjMessage      system) {

	OBJECTID  theObject;
	PICTOBJECT 	*pictObject = NULL;
	theObject = theEvent->targetObject;


	// for startup & shutdown events the target object = 0
	if (AObjCheckType(theObject,(TYPEID)OTYPE_PICTURE) || (theObject == 0))
	 	{
		// before startup is handled, this will be null
		pictObject = (PICTOBJECT *)GetPictObject(theObject);

		switch ((LONG)theEvent->what)  {
			// The following events are defined to provide data to and from
			// a picture object in BITMAP format. June 30, 1993
			case OPICT_EVT_OBJECTTOBITMAP: //(otoB)send the picture data in form of
													 // of a BITMAP
				{
				MEMBLOCKID bufferID = (MEMBLOCKID)theEvent->details;
				pictObject->ObjectToBitmap(bufferID, TRUE);
				return;
				}

			case OPICT_EVT_BITMAPTOOBJECT: //  (btoO):
				{
				MEMBLOCKID bufferID = (MEMBLOCKID)theEvent->details;
				pictObject->BitmapToObject(bufferID);
        }
				return;
			

		 	case AEVENT_OBJECTCHANGED:
				{
				AEvtPostSignalAtTail(theObject, PICTCHANGEDSIGNAL);
				return;
				}

		 	case AEVENT_STARTUP:
				{
				long i  = AOBJ_GETFIRSTKEY;
				OBJECTID pictID;

				// for all the picture object instances in your appl
				// loop thro and set Obj runtieInfo
				while ((BOOL)(pictID = AObjGetNextObject((TYPEID)OTYPE_PICTURE, (LONG *)&i)))
					{
					// create a runtime C++ class and store it in
					// obj runtime info
					// It will get deleted on the way out
					InitPictObject(pictID);
					}
				}
				break;

		 	case AEVENT_SHUTDOWN:
		 		{
				long i  = AOBJ_GETFIRSTKEY;
        PICTOBJECT *pictureObject;
				OBJECTID pictID;
				while ((BOOL)(pictID = AObjGetNextObject((TYPEID)OTYPE_PICTURE, (LONG *)&i)))
					{	
					pictureObject = (PICTOBJECT *)GetPictObject(pictID);
					// delete the temp file created for MEta operation
					if (pictureObject && (pictureObject->GetPictType() == METAPICT))
						{
						char fname[20];
						OFSTRUCT ofStruct;
						wsprintf(fname, "%d%d", pictID, GetCurrentTask());
			      OpenFile(fname, &ofStruct, OF_DELETE);
						}
					// SKJ delete this when you get the destroy message
					//delete (pictureObject);
					}
				}
		 		break;
		 	default:
				break;
		 	}  // eo msg switch
		}    // eo if (theObject == PICTURE object)
	I_UNUSED(system);

	}






#ifdef ADBG_DEBUG

extern "C" void FAR PASCAL _export GetSetPictData(
	pAEvtInfo							theEvent,
	pAObjMessage 					system) 
	{
  OBJECTID 						pObject, dObject;
	AEvtInfo               tEvent;

	pObject = AFuncGetParameter(1); // the picture object to load New picture into
	dObject = AFuncGetParameter(2); // the destinationPicture
	system->message1 = 1;

	if (AObjCheckType( pObject, OTYPE_PICTURE) &&
		 AObjCheckType( dObject, OTYPE_PICTURE))
		{

		MEMBLOCKID bufferID;

		bufferID = AMemAllocate(0);

		tEvent.sourceObject = pObject;
		tEvent.targetObject = pObject;
	  tEvent.what = OPICT_EVT_OBJECTTOBITMAP;
		tEvent.when = 0; //Tickcount;
		tEvent.how = AEVT_ATONCE | AEVT_DIRECT;
		tEvent.details = (long)bufferID;
		tEvent.modifiers =  0;  

		AEvtPost(&tEvent);

		if (AMemGetSize(bufferID))
      {
      // Send the received Bitmap to another picture object
		  tEvent.sourceObject = dObject;
		  tEvent.targetObject = dObject;
		  tEvent.what = OPICT_EVT_BITMAPTOOBJECT;
		  tEvent.when = 0; //Tickcount;
		  tEvent.how = AEVT_ATONCE | AEVT_DIRECT;
			tEvent.details = (long)bufferID;
		  tEvent.modifiers =  0;
			
		  AEvtPost(&tEvent);
			}
    

		AMemFree(bufferID);

		}
	else
		{
		lstrcpy(system->errorMessage, "Error while loading picture.");
		system->message1 = 0;
		}
	I_UNUSED(theEvent);
	}


#endif
