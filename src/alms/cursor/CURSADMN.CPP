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
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define		STRICT
#define		ASTRICT

#include "a_alm.h"
#include "mem.h"
#include "iconedit.h"
#include "cursor.h"
#include "cursordl.h"
// #include "upsf.h"

extern int _WinAllocFlag;
#define CURSORSCOPE "cursorScope"
#define FRAME       "frame"
#define NATV_BWCURSOR AMAKEID('U','B','W', 'I')
#define UNIV_BWCURSOR NATV_BWCURSOR

#define MACCURSORSIZE 	(16 * (16 * 4)) / 8	// 16 * 16 pixel bmp with 4 bits per pixel

HANDLE hInst;
extern SERIUSICON seriusIcon;

extern "C" int CALLBACK CursorProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam);


long EditCursors(AOBJECTID theObject ) ;

// This a struct used to import Mac Cursor Images
struct MACCURSORBMP {
	BITMAPINFOHEADER bmpInfo;
	RGBQUAD          rgbQuad[16];          // 4 bits per pixel
	BYTE             bits[MACCURSORSIZE]; // rgb table and image
	};

// This a struct used to import MS Cursor Images
struct CURSORBMP {
	BITMAPINFOHEADER bmpInfo;
	RGBQUAD          rgbQuad[2];         
	BYTE             bits[MASKARRAYSIZE]; // rgb table and image

  };

LPSTR SourcePtr, DestinationPtr;
AOBJECTID Source, Destination;
pAEvtInfo theEvent;
pAObjMessage      TheSystem;
LPSTR  Data;
// CursorData Data;


////////////////////////////////////////////////////////////////////////////
// 	MakeCursorFromBMP :
//	This function takes a Mac BMP and creates a MS monochromatic bmp from it.
//  It is called to create an And and Xor image of a cursor
////////////////////////////////////////////////////////////////////////////
BOOL MakeCursorFromBMP(
	MACCURSORBMP macBMP,
	CURSORBMP *cursor)
	{
	HBITMAP copyBmp;
	HDC hDC = GetDC(NULL);

	// Finally, create a monochrome DDB, and put the DIB into it.
	// SetDIBits does smart color conversion.
	// select the old palette back into the DC
	
  copyBmp = CreateBitmap(16, 16, 1, 1, NULL);

	SetDIBits(hDC,copyBmp, 0, 16, // lpbi->biHeight,
						(LPSTR)&(macBMP.bits),
						(LPBITMAPINFO)&macBMP, DIB_RGB_COLORS);
	HBITMAP newBmp =	CreateBitmap(32, 32, 1, 1, NULL);
	HBITMAP oldBmp1, oldBmp2;
	HDC hMemDC1 = CreateCompatibleDC (hDC) ;
	HDC hMemDC2 = CreateCompatibleDC (hDC) ;
	oldBmp1 = (HBITMAP)SelectObject (hMemDC1, copyBmp); // device dependent copy of BMP
	oldBmp2 = (HBITMAP)SelectObject (hMemDC2, newBmp) ;
	StretchBlt(hMemDC2, 0, 0, 32, 32, hMemDC1, 0,0,(int)16, 16,SRCCOPY);
	SelectObject (hMemDC1, oldBmp2) ;
	SelectObject (hMemDC2, oldBmp1) ;
	DeleteDC(hMemDC1);
  DeleteDC(hMemDC2);
	BITMAP bitmap1;

	GetObject(newBmp, sizeof(BITMAP), &bitmap1);
	GetBitmapBits(newBmp, bitmap1.bmWidthBytes * bitmap1.bmHeight, cursor->bits);
	
	DeleteObject(copyBmp);
	DeleteObject(newBmp);

	ReleaseDC(NULL, hDC);
	return TRUE;
	}

////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////
int CreateIconBitmaps(LPSERIUSICON lpIcon,
	CURSORBMP *andBMP, CURSORBMP *xorBMP)
	{
	CopyMemory(&andBMP->bmpInfo, &lpIcon->bwIcon.maskHdr, sizeof(BITMAPINFOHEADER));
	CopyMemory(andBMP->rgbQuad,lpIcon->bwIcon.maskColors, sizeof(RGBQUAD) * 2);
	CopyMemory(andBMP->bits, lpIcon->bwIcon.maskArray, MASKARRAYSIZE);

	CopyMemory(&xorBMP->bmpInfo, &lpIcon->bwIcon.imageHdr, sizeof(BITMAPINFOHEADER));
	CopyMemory(xorBMP->rgbQuad,lpIcon->bwIcon.imageColors, sizeof(RGBQUAD) * 2);
	CopyMemory(xorBMP->bits, lpIcon->bwIcon.imageArray, MASKARRAYSIZE);

	return 1;
	}


/////////////////////////////////////////////////////////////////////////
// Fucntion:  ImportObject  
// 						Called to handle the AObj_Import event
//
//////////////////////////////////////////////////////////////////////////
void ImportObject(AOBJECTID objectID)
	{

	BITMAPINFOHEADER stdbmpInfo;
	RGBQUAD       stdRGBQuad[2];

	stdbmpInfo.biSize =   40;
	stdbmpInfo.biWidth =  32;
  stdbmpInfo.biHeight = 32;
	stdbmpInfo.biPlanes = 1;
	stdbmpInfo.biBitCount = 1;
	stdbmpInfo.biCompression = 0;
	stdbmpInfo.biSizeImage = MASKARRAYSIZE ;
	stdbmpInfo.biXPelsPerMeter = 0;
	stdbmpInfo.biYPelsPerMeter = 0;
	stdbmpInfo.biClrUsed = 0;
	stdbmpInfo.biClrImportant = 0;

	stdRGBQuad[0].rgbGreen = 0;
	stdRGBQuad[0].rgbBlue = 0;
	stdRGBQuad[0].rgbRed = 0;

	stdRGBQuad[1].rgbGreen = 0xff;
	stdRGBQuad[1].rgbBlue = 0xff;
	stdRGBQuad[1].rgbRed = 0xff;

	LPCURSOROBJ		 lpCursor = (LPCURSOROBJ)AObjLockData(objectID, CURSOROBJDATA);
	LPSERIUSICON   lpIcon;

	int i;
	long milliDelay;
	if (!AUpsfOpenScope(CURSORSCOPE, 0))
		{
//		ATYPEID  uType;
//		long size;

		AUpsfImportField("VERSION", 0,AUPSF_NATV_INT, &(lpCursor->version), sizeof(int));

		// If importing from an older project, then delay is in 100msec units
		// else it is in 1/60 sec units
		// if (AUpsfFieldInfo("DELAY", 0, AUPSF_NATV_INT, (LONG *)&uType,(LPLONG)&size);

		if (AUpsfImportField("DELAYTIX", 0,AUPSF_NATV_INT, &milliDelay, sizeof(int)) == AUPSF_NOERROR)
			{
			
			lpCursor->delayTime =(int) (milliDelay); // import in 1/60 sec units
			}
		else {
			AUpsfImportField("DELAY", 0,AUPSF_NATV_INT, &milliDelay, sizeof(int));
			lpCursor->delayTime =(int) (milliDelay * 60) / 10; // import in 100 msec units

      }
    	
		AUpsfImportField("FRAMECOUNT", 0,AUPSF_NATV_INT, &(lpCursor->FrameCount), sizeof(int));
		AUpsfCloseScope();
		}
	
	AObjResizeData(objectID, SERIUSICONSDATA, lpCursor->FrameCount * sizeof(SERIUSICON));
	lpIcon = (SERIUSICON *)AObjLockData(objectID, SERIUSICONSDATA);

	for (i = 0; i < lpCursor->FrameCount; i++) {
		if (!AUpsfOpenScope("FRAME", i)){
			SERIUSICON *tmpIcon  = (SERIUSICON *) new SERIUSICON(); //(lpIcon + i);
			long size = sizeof(CURSORBMP);
			CURSORBMP andBMP;
			CURSORBMP xorBMP;
			ATYPEID  uType;

      // Hot spots
			AUpsfImportField("HOTSPOT", 0,AUPSF_NATV_INT, &tmpIcon->bwCursor.cursorImage.HotSpot.xHotSpot, sizeof(int));
			AUpsfImportField("HOTSPOT", 1,AUPSF_NATV_INT, &tmpIcon->bwCursor.cursorImage.HotSpot.yHotSpot, sizeof(int));
			AUpsfFieldInfo("ANDCURSOR", 0, AUPSF_NATV_BITMAPINFO, (LONG *)&uType,(LPLONG)&size);

			// MAC UPSFs a monochromatic bitmap as 4 bits/pixel bitmap
			// GO FIGURE ! June 25, 1993
			if (size != sizeof(CURSORBMP)) // if being imported from MAC
				{
				
				MACCURSORBMP macAndBMP;
				AUpsfImportField("ANDCURSOR", 0,AUPSF_NATV_BITMAPINFO, &macAndBMP, size);
				andBMP.bmpInfo  = stdbmpInfo;
				andBMP.rgbQuad[0] = stdRGBQuad[0];
				andBMP.rgbQuad[1] = stdRGBQuad[1];
				_fmemset(andBMP.bits, 0, MACCURSORSIZE);
				MakeCursorFromBMP(macAndBMP, &andBMP);
				}
			else
			  {				                   // 
			  AUpsfImportField("ANDCURSOR", 0,AUPSF_NATV_BITMAPINFO, &andBMP, size);
				}
			
			AUpsfFieldInfo("XORCURSOR", 0, AUPSF_NATV_BITMAPINFO, (LONG *)&uType,(LPLONG)&size);

			if (size != sizeof(CURSORBMP)) // if being imported from MAC
				{			
				MACCURSORBMP macXorBMP;
				AUpsfImportField("XORCURSOR", 0,AUPSF_NATV_BITMAPINFO, &macXorBMP, size);
				xorBMP.bmpInfo  = stdbmpInfo;
				xorBMP.rgbQuad[0] =  stdRGBQuad[0];
				xorBMP.rgbQuad[1] =  stdRGBQuad[1];
				_fmemset(xorBMP.bits, 0, MASKARRAYSIZE);
				MakeCursorFromBMP(macXorBMP, &xorBMP);
				}
			else
				AUpsfImportField("XORCURSOR", 0,AUPSF_NATV_BITMAPINFO, &xorBMP, size);

			CopyMemory(&(tmpIcon->bwIcon.maskHdr),  &(andBMP.bmpInfo) , sizeof(BITMAPINFOHEADER));
			CopyMemory(tmpIcon->bwIcon.maskColors,andBMP.rgbQuad, sizeof(RGBQUAD) * 2);
			CopyMemory(tmpIcon->bwIcon.maskArray, andBMP.bits, MASKARRAYSIZE);
			CopyMemory(&(tmpIcon->bwIcon.imageHdr),  &(xorBMP.bmpInfo),  sizeof(BITMAPINFOHEADER));
			CopyMemory(tmpIcon->bwIcon.imageColors,xorBMP.rgbQuad,sizeof(RGBQUAD) * 2);
			CopyMemory(tmpIcon->bwIcon.imageArray, xorBMP.bits, MASKARRAYSIZE);
			SERIUSICON *serIcon = (SERIUSICON *)(lpIcon + i);
			CopyMemory((LPSTR)(serIcon), (LPSTR)(tmpIcon), sizeof(SERIUSICON));
			delete tmpIcon;
			AUpsfCloseScope();
			}
		}

	// unlock prvious Data POINTERS
	if (lpIcon)
		AObjUnlockData(objectID, SERIUSICONSDATA);

	if (lpCursor)
		AObjUnlockData(objectID, CURSOROBJDATA);
	}


//////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////
void ExportObject(AOBJECTID objectID) {
	LPCURSOROBJ		 lpCursor = (LPCURSOROBJ)AObjLockData(objectID, CURSOROBJDATA);
	LPSERIUSICON   lpIcon;
	int i;
	if (!AUpsfCreateScope(CURSORSCOPE, 0))
		{
		long lmilliDelay;
		AUpsfExportField("VERSION", 0,AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &lpCursor->version, sizeof(int));
		lmilliDelay = (long)lpCursor->delayTime; // Export unit 1/60 of a second
		AUpsfExportField("DELAYTIX", 0,AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &lmilliDelay, sizeof(int));
		AUpsfExportField("FRAMECOUNT", 0,AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &lpCursor->FrameCount, sizeof(int));
		AUpsfCloseScope();
		}
	lpIcon = (SERIUSICON *)AObjLockData(objectID, SERIUSICONSDATA);
	for (i = 0; i < lpCursor->FrameCount; i++)
		{
		if (!AUpsfCreateScope("FRAME", i))
			{
		  int              imageSize = sizeof(CURSORBMP);

			CURSORBMP        *lpAndBMP = (CURSORBMP *)new char[imageSize];
			CURSORBMP        *lpXorBMP = (CURSORBMP *)new char[imageSize];
			
			SERIUSICON *tmpIcon = (SERIUSICON *)(lpIcon + i);

			// for cross platform export
			// create the and and Xor bitmap
			CreateIconBitmaps(tmpIcon, lpAndBMP, lpXorBMP);

			 // Hot spots
			AUpsfExportField("HOTSPOT", 0,AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &(tmpIcon->bwCursor.cursorImage.HotSpot.xHotSpot), sizeof(int));
			AUpsfExportField("HOTSPOT", 1,AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &(tmpIcon->bwCursor.cursorImage.HotSpot.yHotSpot), sizeof(int));


			// AND Mask bits
			AUpsfExportField("ANDCURSOR", 0,AUPSF_NATV_BITMAPINFO, AUPSF_UNIV_BITMAP, lpAndBMP, imageSize);
      // XOR Mask bits
			AUpsfExportField("XORCURSOR", 0,AUPSF_NATV_BITMAPINFO, AUPSF_UNIV_BITMAP, lpXorBMP, imageSize);
   
			AUpsfCloseScope();

			delete lpAndBMP;
			delete lpXorBMP;
			}
		}
	// unlock prvious Data POINTERS
	AObjUnlockData(objectID, SERIUSICONSDATA);
	AObjUnlockData(objectID, CURSOROBJDATA);
	}

///////////////////////////////////////////////////////////////

void InitData(AOBJECTID oid)
	{
	CURSOROBJ *Data ; //CursorData Data;
	AOBJECTID TheObject;

	TheObject = oid;
	AObjSetDataCount(TheObject, 2);

	AObjResizeData(TheObject, 0, sizeof(CURSOROBJ));
	Data = (CURSOROBJ *)(AObjLockData(TheObject, 0));
	_fmemset(Data, 0, sizeof(CURSOROBJ));
	AObjUnlockData(TheObject, 0);
	Data->version = DATAVersion; // April 13, 1993
	}


/////////////////////////////////////////////////////////////////

void StopAnimation()
	{
	;
	}


/////////////////////////////////////////////////////////////////
//  Cursor ADMN proc
//
/////////////////////////////////////////////////////////////////
extern "C" long CALLBACK admnProc(
	AOBJECTID					theObject,
	pAObjMessage       theSystem)
	{
	AMEMBLOCKID  SignalID;
	pAObjSignalRecord SignalHandle;
	char ErrorMessage[256];
	ErrorMessage[0] = '\0';

	long rVal = A_OK;

	switch (theSystem->message1) {
		case AOBJ_CREATED:
			InitData(theObject);
			break;

		case AOBJ_READ: // version Check
			{
			// this is to correct projects that do not have any version set
			// in windows 1.0 release 4/13/93 (DLLVersion == 1)
			// this will be removed when I release a new CURSOR dll
			LPCURSOROBJ lpCursor = (CURSOROBJ *)AObjLockData(theObject, CURSOROBJDATA);
			if (lpCursor->version != DATAVersion)
			  lpCursor->version = DATAVersion;

			AObjUnlockData(theObject, CURSOROBJDATA);
		 // 	theSystem->message4 = versionOK;
			theSystem->message3 = (LONG)DATAVersion;

			rVal = A_OK;
			}
			break;

		case AOBJ_ASSIGNOBJECT:
			{
			// Appware change --> oid reversed
			Destination = theObject;
			Source = (AOBJECTID)theSystem->message3;
			if (!(AObjCheckType(Source, (ATYPEID)OTYPE_CURSOR)))
				{
				rVal = A_NOTHANDLED; // source is not cursor
				break; // exit out of this event handling
			
        }
			else if (AObjGetType(Source) == AObjGetType(Destination))
				{
				Data = (LPSTR)AObjLockData(Destination, 0);

				SourcePtr = (LPSTR)AObjLockData(Source, 0);

				// correct the vesion number , April 13, 1993
				((LPCURSOROBJ)Data)->version = DATAVersion;
				((LPCURSOROBJ)SourcePtr)->version = DATAVersion;

				AObjUnlockData(Destination, 0);

				AObjResizeData(Destination, 0, AObjGetDataSize(Source, 0));
				DestinationPtr = (LPSTR)AObjLockData(Destination, 0);

				if (DestinationPtr)
					{
					CopyMemory(DestinationPtr , SourcePtr, (int)AObjGetDataSize(Source, 0));
					}
				AObjUnlockData(Destination, 0);
				rVal = A_OK;
				}
			}
			break;

		case AOBJ_IMPORTOBJECT:
			ImportObject(theObject);
      rVal = A_OK;
			break;

		case AOBJ_EXPORTOBJECT:
			ExportObject(theObject);
			rVal = A_OK;
			break;

		case AOBJ_CHECKOBJECT: {      // getParameters
			rVal = A_OK; // ObjectHealthy
			}
			break;

		case AOBJ_EDITOBJECT:
			rVal = EditCursors(theObject);
			break;

		case AOBJ_VALIDATEOBJECTIDS: {
			LPCURSOROBJ lpCur = (CURSOROBJ *)AObjLockData(theObject, CURSOROBJDATA);
			AObjValidateID( &(lpCur->objectID), theSystem);
			AObjUnlockData(theObject, CURSOROBJDATA);
			}
			break;

		case AOBJ_BUILDSIGNALS:

			SignalID = (AMEMBLOCKID)theSystem->message2;
			AMemSetSize(SignalID, sizeof(AObjSignalRecord) * 2);
			SignalHandle = (pAObjSignalRecord)AMemLock(SignalID);

			SignalHandle[0].theID = 0;
			lstrcpy(SignalHandle[0].theName, "Started");
			SignalHandle[1].theID = 1;
			lstrcpy(SignalHandle[1].theName,"Stopped");
			AMemUnlock(SignalID);
			break;

	 default:
	 		rVal = A_NOTHANDLED;
			break;
	 }

 // AFuncReportResult(theSystem, 0, ErrorMessage);
 return rVal; // A_OK, A_NOTHANDLED or A_ERROR
 }

////////////////////////////////////////////////////////////////////////
// this function returns the dllVersion of this Object. It is
// used by the shell to check for incompatibilty
long  GetSerVersion(
	ATYPEID objectType)
	{
	if (objectType == (ATYPEID)OTYPE_CURSOR)
		return DLLVersion;
	else
		return -1L;
	}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long EditCursors(
	AOBJECTID theObject )
	{
	HWND hWnd = GetActiveWindow();
	seriusIcon.bwIcon.imageArray[0] = NOT_SAVED;

	if (DialogBoxParam((HINSTANCE)hInst, 
							MAKEINTRESOURCE(DIALOG_1), 
							hWnd, 
							(DLGPROC)CursorProc, 
							(LONG)theObject) == TRUE)

		return A_MODIFIED;
	else
	    return A_OK;
	}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI   DllEntryPoint(
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
			break;

		case DLL_PROCESS_DETACH:
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

