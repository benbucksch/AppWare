/*//////////////////////////////////////////////////////////////
//
//					CopyrigSght 1993, Novell, Inc.  All rights reserved
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
// FILE:  	DLGPROC.CPP	
//
// AUTHOR:  Shaili Jain
//
// DESCRIPTION:   Handles Objd Code
//
// CHANGES:
//
//
//////////////////////////////////////////////////////////////*/
/**********************************************************
* object: ALM_CURSOR 
* Module: dlgproc.cpp
*	Description:  Dlg Proc for OBJECT EDIT
* Aug. 23, 1993 : Tailored code to adjust to new APPWARE
*									implementation
*
**********************************************************/
#define		STRICT
#define		ASTRICT

#ifndef DWL_MSGRESULT
#define DWL_MSGRESULT 0
#endif

#include "windows.h"
#include "fileopen.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "a_alm.h"
#include "a_almutl.h"
#include "helpids.h"
#include "memclass.h"

// #include "memclass.h"

#include "iconedit.h"   // Hdr file for ICON EDITOR
#include "pixelwin.h"
#include "commdlg.h"
#include "position.h"
#include "cursor.h"
#include "cursorDlg.h"

#define OBJECTNAMESIZE  80
extern int iScanLineBytes;
extern int mScanLineBytes;


BOOL bICONLOADED;
// POSITION to place ICONEDITOR window
int dlgleft;
int dlgtop;

BOOL bNewFrameAdded = FALSE;
HANDLE hArrowCursor;
HANDLE hWaitCursor;

// ext. of CTRLs in Window Units
int xCtrlExt;
int yCtrlExt;

BOOL bNotSCROLL = TRUE;

#define _DEMO
// turn the style on
#define SetWindowStyle(hWnd, Style) \
	 SetWindowLong(hWnd, GWL_STYLE, Style|GetWindowLong(hWnd, GWL_STYLE));

// turn the style off
#define ResetWindowStyle(hWnd, Style) \
	 SetWindowLong(hWnd, GWL_STYLE, ~Style&GetWindowLong(hWnd, GWL_STYLE));


#define ENABLECHILDREN (0x80000000L)
#define DISABLECHILDREN (0x00000000L)

#define FRAMELEFT PIXELWNDLEFT+PIXELWNDWIDTH-BUTTON2WIDTH+18


extern 	HWND hpixelWnd;

int 		nIconEdited = 0;
extern 	HANDLE hInst;

// Variables used during Cursor editing
// No REENTRANCY issue here, because we can only be editing one cursor at a time
ICONEDITOR 		*iconEditor;
SERIUSICON 		seriusIcon;
LPHCURSORS 		lpHCursors 	= 0;
HANDLE   	 		hHCursors 	= 0;
HSICON    		hSIcons 		= 0;
LPHSICONS 		lpHSIcons 	= 0;

// prototypes
void 		SaveOldIconInDB(HWND hWnd, int FrameIndex, int nIconEdited);
extern 	void BuildCursors(OBJECTID theObject, HANDLE *handle, HANDLE *cursH);
void 		FlipIcon(BYTE *lpBit);
HCURSOR MakeCursor(HDC hDC, HICON hIcon, SERIUSICON serIcon);
long 		FAR PASCAL _export MainWndProc(HWND hWnd, WORD message, WORD wParam, DWORD lParam);
void 		SaveIconInDB(HWND hDlg, int i, CURSOROBJ *cursorObj);
void 		DelIconInDB(HWND hDLG, int index);
LPLONG 	GetOBJDPtr(HWND hWnd);
extern "C" int FAR PASCAL _export CursorProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam);

HANDLE Gbl_hCursor;  // holds handle to the current Cursor
#define MAX_TYPE 100;

#ifdef OIP
	OSTYPE *typeList;
	HANDLE hTypeList;
#endif




/****** CODE FOR TESTING THE CURSOR ANIMATION *******/


extern "C" WORD FAR PASCAL _export testTimerProc(
	HWND  hWnd,
	WORD  wMsg,
	int   ID,
	DWORD time)
	{
	static int  nCount = 0;
	HICON      hCursor;
	int         index = 0;
	CURSOROBJ		*cursorObj;

	if (hWnd == NULL)
    return 0;
	cursorObj = (LPCURSOROBJ)GetOBJDPtr(hWnd);
	if ((!cursorObj) || (!cursorObj->FrameCount))
		return 0;

	lpHCursors = (LPHCURSORS)GlobalLock(hHCursors);

	index = nCount % cursorObj->FrameCount;
	hCursor = *(lpHCursors + index);
	nCount++;

	SetCursor((HICON)hCursor);

	Gbl_hCursor = hCursor;

	GlobalUnlock(hHCursors);

	I_UNUSED(wMsg);
	I_UNUSED(ID);
	I_UNUSED(time);

	return 1;

	}



static void StartAnim(
  HWND 				hWnd,
	LPCURSOROBJ lpCursor)
	{
	if (lpCursor->FrameCount <= 0)
		return;

	// if the user has input -ve time delay, set it to 1 sec
	if (lpCursor->delayTime <= 0)
		lpCursor->delayTime = 60;

	unsigned int millideltaTime = (1000 / 60) * lpCursor->delayTime  ;  // convert 1/60 sec to millisecond

	SetClassWord(iconEditor->hWndIconEditor, GCW_HCURSOR, NULL);
	if (SetTimer(hWnd, (int)(lpCursor->timerID), millideltaTime, (TIMERPROC)testTimerProc) == 0)
		lpCursor->timerID = 0;

	}


static void KillAnim(
	HWND 				hWnd,
	LPCURSOROBJ lpObject)
	{

	KillTimer(hWnd, lpObject->timerID);

	SetClassWord(iconEditor->hWndIconEditor, GCW_HCURSOR, (WORD)hArrowCursor);

	lpObject->timerID = 0;

	Gbl_hCursor = hArrowCursor;

	}



///////////////////////////////////////////////////
BOOL InitializeFOpen(OPENFILENAME FAR*ofn, HWND ghWnd)
	{

	static char szDirName[256];
 	static char szFile[256] = "\0";
 	char szFileTitle[256];
 	char szFilter[256];
	LPSTR lpext;
  BOOL 	rVal = FALSE;
 	OPENFILENAME opn;
 	int size = 0;
	szFileTitle[0] = '\0';
  szDirName[0] = '\0';

	memset((LPVOID)&opn, 0, sizeof(OPENFILENAME));

	memset(szFilter , 0, 256);
	lstrcpy(szFilter, "Cursor File (*.cur) *.cur");
	lpext = strchr(szFilter, ')');
	size = strlen(szFilter);

	lpext[1] = '\0';
	szFilter[size + 1] = '\0';
	szDirName[0] = '\0';
	if (szFile[0] == '\0')
		{
		GetSystemDirectory(szDirName, sizeof(szDirName));
		strcpy(szFile, "*.cur");

		}
	else
		{
		LPSTR lpOld;
		LPSTR lpTmp = new char[strlen(szFile) + 1];
		strcpy(lpTmp, szFile);
		lpOld = lpTmp;
		while ((lpTmp = strchr(lpTmp, '\\')) != NULL)
			{
			lpOld = lpTmp;

			lpTmp++;
			}
		szFile[strlen(szFile) - strlen(lpOld)] = '\0';

		strcpy(szDirName, szFile);
		strcpy(szFile, "*.cur");
		delete lpTmp;
		}

	opn.lStructSize       = sizeof(OPENFILENAME);
	opn.hwndOwner         = (HWND)ghWnd;
	// opn.hInstance         = (HANDLE)NULL;
	opn.lpstrFilter       = szFilter;
	// opn.lpstrCustomFilter = (LPSTR)NULL;
	// opn.nMaxCustFilter    = 0L;
	opn.nFilterIndex      = 1L;
	opn.lpstrFile         = szFile;
	opn.nMaxFile          = (DWORD)sizeof(szFile);
	opn.lpstrFileTitle    = szFileTitle;
	opn.nMaxFileTitle     = 80; //MAXFILETITLELEN;
	opn.lpstrInitialDir   = szDirName; //gszBuffer;
	opn.lpstrTitle        = (LPSTR)"LOAD CURSOR";
	opn.Flags             = OFN_PATHMUSTEXIST |
				OFN_FILEMUSTEXIST |  OFN_NOCHANGEDIR |
				OFN_SHOWHELP;


	rVal = GetOpenFileName((OPENFILENAME FAR*) &opn);
	_fmemcpy((LPSTR) ofn , (LPSTR)&opn, sizeof(OPENFILENAME));

  return rVal;
	}



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
HCURSOR MakeFlipCursor(
	HDC        /*hDC*/,
	HICON      /*hIcon*/,
	SERIUSICON seriusIcon)
	{
	int i,j;
	BYTE BMPBits[MASKARRAYSIZE], ZEROBits[MASKARRAYSIZE]; // for BMPBits should we use IMAGEARRAYSIZE

	for (i=0; i< 128; i++)
		ZEROBits[i] = 0;
	j = MASKARRAYSIZE ;

	for (i = 0; i < MASKARRAYSIZE; i++) {
		BMPBits[i] = seriusIcon.bwIcon.imageArray[i];
		j--;
		}
	// Invert Image
	int size = 32;
	int x, y;

	for (y = 0; y < 32; y++) {
		for (x = 0; x < 32; x++) {
		 int pixelsPerByte = 8;   // should be 2? for colored icons
		 int imageIndex = ((size - 1 - y)*iScanLineBytes)+(x/pixelsPerByte);

		 int newImageIndex = (y * iScanLineBytes)+(x/pixelsPerByte);

		 BMPBits[newImageIndex] = seriusIcon.bwIcon.imageArray[imageIndex];
		 }
		}
	j = MASKARRAYSIZE;
	for (i=0; i< MASKARRAYSIZE; i++) {
		ZEROBits[i] = seriusIcon.bwIcon.maskArray[i];
		j--;
		}
	return CreateCursor((HINSTANCE)hInst,
					 seriusIcon.bwCursor.cursorImage.HotSpot.xHotSpot,
					 seriusIcon.bwCursor.cursorImage.HotSpot.yHotSpot,
					 32,32,
					 (LPSTR)BMPBits,
					 (LPSTR)ZEROBits);

	}




HCURSOR MakeCursor(
	HDC        /*hDC*/,
	HICON      /*hIcon*/,
	SERIUSICON seriusIcon)
	{
	int i,j;
	BYTE BMPBits[MASKARRAYSIZE];
	BYTE ZEROBits[MASKARRAYSIZE]; // for BMPBits should we use IMAGEARRAYSIZE

	for (i=0; i< 128; i++)
		ZEROBits[i] = 0;

	j = MASKARRAYSIZE ;

  // XOR Bits
	for (i = 0; i < MASKARRAYSIZE; i++)
		{
		BMPBits[i] = seriusIcon.bwIcon.imageArray[i];
		j--;
		}

	// Invert Image

	j = MASKARRAYSIZE;

  // AND BITS
	for (i=0; i< MASKARRAYSIZE; i++)
		{
		ZEROBits[i] = seriusIcon.bwIcon.maskArray[i];
		j--;
		}

	FlipIcon(BMPBits);
	FlipIcon(ZEROBits);

	return CreateCursor((HINSTANCE)hInst,
					 seriusIcon.bwCursor.cursorImage.HotSpot.xHotSpot,
					 seriusIcon.bwCursor.cursorImage.HotSpot.yHotSpot,
					 32,32,
					 (LPSTR)ZEROBits,
					 (LPSTR)BMPBits);

 
	}


/****************************************************************************

		FUNCTION: GetOBJDPtr

		PURPOSE: Returns the C++ object pointer assoc. with this dlg window

****************************************************************************/
LPLONG GetOBJDPtr(HWND hWnd) {
	LPLONG cursorObj;
	OBJECTID theObject = (OBJECTID)GetWindowLong(hWnd, DWL_USER);
  if (theObject)
		cursorObj =  (LPLONG)AObjGetRuntimeInfo (theObject);
	else
		cursorObj = NULL;

  return cursorObj;
	}

/****************************************************************************

		FUNCTION: SetOBJDPtr

		PURPOSE: Sets a C++ object pointer assoc. with this dlg window

****************************************************************************/

LONG SetOBJDPtr(HWND hWnd, OBJECTID theObject, LONG cursorObj) {

	long rVal = AObjSetRuntimeInfo(theObject, (LONG)cursorObj);
	SetWindowLong(hWnd, DWL_USER, (LONG)theObject);
	return rVal;
	}


/****************************************************************************

		FUNCTION: CursorProc(HWND, unsigned, WORD, LONG)

		PURPOSE: Let user select a file, and return.  Open code not provided.

****************************************************************************/

extern "C" int FAR PASCAL _export CursorProc(   
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam)
	{
	// object ID of the Cursor being edited
	OBJECTID theObject = (OBJECTID)GetWindowLong(hDlg, DWL_USER);
	BOOL bProcessed = FALSE;
	SERIUSICON huge *lpIcon;
	char lstr[25];
	LPCURSOROBJ lpData;
	struct CURSOROBJ *cursorObj = NULL;

	if (message != WM_INITDIALOG) {
		cursorObj = (LPCURSOROBJ) GetOBJDPtr(hDlg);
		}

	switch (message) {
		case WM_INITDIALOG: {
		 	char timetxt[40];
		 	char title[OBJECTNAMESIZE];
			HDC hDC = GetDC(hDlg);
			cursorObj = (LPCURSOROBJ)new char[sizeof(CURSOROBJ)];
		 	AObjGetName((OBJECTID)lParam, (LPSTR)title, OBJECTNAMESIZE);
		 	SetWindowText(hDlg, (LPSTR)title);
		 	hArrowCursor = LoadCursor(NULL, IDC_ARROW);
		 	Gbl_hCursor = hArrowCursor;
		 	theObject = (OBJECTID)lParam;
			// copy the contents of the data to cursorObj
			lpData = (LPCURSOROBJ)AObjLockData(theObject, CURSOROBJDATA);
			_fmemcpy(cursorObj, lpData, sizeof(CURSOROBJ));
			AObjUnlockData(theObject, CURSOROBJDATA);

			if (cursorObj->FrameCount){
			 	SERIUSICON blankIcon;
			 	int i;
			 	BuildCursors(theObject, &hSIcons,&hHCursors );
				nIconEdited = cursorObj->FrameCount;
			 	lpHSIcons = (LPHSICONS) GlobalLock(hSIcons);
			 	cursorObj->nCurrentFrame = 1; // set it the First FRAME
			 	bNewFrameAdded = FALSE;
			 	seriusIcon = *(lpHSIcons + 0 );  // show the last frame
		 		iconEditor = new ICONEDITOR(hDlg, hInst, 0, &seriusIcon, BLACKANDWHITE);
			 	seriusIcon = *(lpHSIcons + 0);  // show the last frame
 				iconEditor->pixelWindow->ReplaceIcon((LPSERIUSICON)(&seriusIcon));
			 	iconEditor->pixelWindow->Redraw();
			 	SetScrollPos(hDlg, SB_VERT, cursorObj->nCurrentFrame, TRUE);
		 		EnableWindow(iconEditor->hDelButton, TRUE);
			 	if (cursorObj->FrameCount > 1) {
					SetScrollRange(iconEditor->pixelWindow->hWndPixelWindow,
						 SB_VERT, 1, cursorObj->FrameCount, TRUE);
				 	SetScrollPos(iconEditor->pixelWindow->hWndPixelWindow,
						 SB_VERT,cursorObj->nCurrentFrame , TRUE);
				 	}
				// EnableWindow(iconEditor->hDelButton, FALSE);
			 	lpHCursors = (LPHCURSORS)GlobalLock(hHCursors);
			 	for (i = 0; i < cursorObj->FrameCount; i++)
					*(lpHCursors + i) = MakeCursor(hDC, (HICON)hArrowCursor, *(lpHSIcons + i)); // store the handle in the global memory
				GlobalUnlock(hHCursors);
			 	GlobalUnlock(hSIcons);
			 	}
		 	else {
			 	SERIUSICON blankIcon;
			 	_fmemset((LPSTR)cursorObj,0, sizeof(CURSOROBJ));
				// when first starting, set the Frame Count to be 1
			 	nIconEdited = 1;
			 	cursorObj->delayTime = 60;  // 1 sec
			 	cursorObj->nCurrentFrame = 1;
			 	cursorObj->FrameCount = 1;
			 	cursorObj->objectID = theObject;
			 	_fmemcpy(&seriusIcon, &blankIcon, sizeof(SERIUSICON));

			 	hHCursors = GlobalAlloc( GMEM_MOVEABLE, nIconEdited * sizeof(HCURSOR));

			 	// if wew start for the first time
				bNewFrameAdded = TRUE;
				iconEditor = new ICONEDITOR(hDlg, hInst, 0, &seriusIcon, BLACKANDWHITE);
			 	//Start w/ a clear Frame
			 	iconEditor->pixelWindow->ClearIcon();
			 	iconEditor->pixelWindow->Redraw();
			 	// Init the HOT SPOT
			 	seriusIcon.bwCursor.cursorImage.HotSpot.xHotSpot = 0;
			 	seriusIcon.bwCursor.cursorImage.HotSpot.yHotSpot = 0;
			 	EnableWindow(iconEditor->hDelButton, FALSE);
			 	}
			wsprintf((LPSTR)timetxt, "%d", cursorObj->delayTime);
		 	SetWindowText(iconEditor->hDelayTimeEdit, (LPSTR)timetxt);
			// Store cursorObj in  the runtime-Info Data of the object
			SetOBJDPtr(hDlg, theObject, (LONG)cursorObj);
			ReleaseDC(hDlg, hDC);
			ATypeGetName((TYPEID)OTYPE_CURSOR, (LPSTR)title, 32);
			AUtlSetTitleFont((TYPEID)OTYPE_CURSOR, title, GetDlgItem(hDlg, ID_OBJECT));

	 		wsprintf((LPSTR)lstr, "%d of %d",cursorObj->nCurrentFrame, cursorObj->FrameCount);
	 		SetWindowText(iconEditor->hNFrames, (LPSTR)lstr);
	 		return TRUE;

		 	}
		case WM_PAINT: {
			return FALSE;
			}

		case WM_LBUTTONDOWN: {
			if (!cursorObj)
				break;
			if (cursorObj->timerID) {
				KillAnim(hDlg, cursorObj);
				EnableWindow(GetDlgItem(hDlg, ID_TEST), TRUE);
      	}
			}
			break;
		case WM_SETCURSOR: {
			if (cursorObj->timerID) {
				SetWindowLong(hDlg, DWL_MSGRESULT, MAKELONG(TRUE, 0));
				SetCursor((HICON)Gbl_hCursor);
				return TRUE;
				}
			else {
				SetWindowLong(hDlg, DWL_MSGRESULT, MAKELONG(FALSE, 0));
				}
			}
			break;

		case WM_COMMAND:
			SendMessage(iconEditor->hWndIconEditor, WM_COMMAND, wParam, lParam);

			switch (wParam)
			  {
				case IDHELP: {
					WinHelp(hDlg, "vabhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Acur);
				 	break;
					}
				case ID_TEST : {
					if (cursorObj->timerID == 0) {
						char timer[20];
						GetWindowText(iconEditor->hDelayTimeEdit, (LPSTR)timer, 10);

						cursorObj->delayTime = atoi(timer);
						cursorObj->timerID = 1; // use 1 as ID
						// NEED TO SAVE THE LAST FRAME             
						if (bNewFrameAdded) {
							SaveIconInDB(hDlg, cursorObj->nCurrentFrame, cursorObj);
							bNewFrameAdded = FALSE;
							}
						else
							// Save the OLD FRAME
							SaveOldIconInDB(hDlg, cursorObj->nCurrentFrame, cursorObj->FrameCount);
						StartAnim(hDlg, cursorObj);
						EnableWindow(GetDlgItem(hDlg, ID_TEST), FALSE);
						}
					else
						KillAnim(hDlg, cursorObj);
					cursorObj->bcursorJustSet = TRUE;
					}
					break;
				case DELAYID:
				 	return TRUE;

				case IDOK: {
					long size = 0L;
					int i;
					char timer[11];
					LPCURSOROBJ lpData;

					if (cursorObj->timerID)
						KillAnim(hDlg, cursorObj);
					GetWindowText(iconEditor->hDelayTimeEdit, (LPSTR)timer, 10);

					cursorObj->delayTime = atoi(timer);

					if (cursorObj->delayTime <= 0)
						cursorObj->delayTime = 60; // set it to 1 sec

					// NEED TO SAVE THE LAST FRAME
					if (bNewFrameAdded) {
						SaveIconInDB(hDlg, cursorObj->nCurrentFrame, cursorObj);
						bNewFrameAdded = FALSE;
						}
					else
						// Save the OLD FRAME
						SaveOldIconInDB(hDlg, cursorObj->nCurrentFrame, cursorObj->FrameCount);

					bNewFrameAdded = FALSE;
					size = GlobalSize(hHCursors); // to hold all the Handle to Cursors
					size = cursorObj->FrameCount;
					lpHCursors = (LPHCURSORS)GlobalLock(hHCursors);
					for (i= 0; i < size; i++) {
						DestroyCursor(*(lpHCursors + i));
						}
					AObjResizeData(theObject, CURSOROBJDATA, size + sizeof(CURSOROBJ));
					lpData = (LPCURSOROBJ)AObjLockData(theObject, CURSOROBJDATA);
					_fmemcpy(lpData, cursorObj, sizeof(CURSOROBJ));
					AObjUnlockData(theObject, CURSOROBJDATA);

					size = cursorObj->FrameCount;
					AObjResizeData(theObject, SERIUSICONSDATA, size * sizeof(SERIUSICON));
					lpIcon = (SERIUSICON *)AObjLockData(theObject, SERIUSICONSDATA);
					lpHSIcons = (LPSERIUSICON)GlobalLock(hSIcons);
					for (i = 0; i < size; i++) {
						SERIUSICON *tmpIcon = (SERIUSICON *)(lpIcon + i);
						SERIUSICON *serIcon = (SERIUSICON *)(lpHSIcons + i);
						_fmemcpy((LPSTR)(tmpIcon), (LPSTR)(serIcon), sizeof(SERIUSICON));
						}
					AObjUnlockData(theObject, SERIUSICONSDATA);

					// Remove the Memory block to seriusIcons and HCursors
					GlobalUnlock(hSIcons);
					GlobalFree(hSIcons);

					GlobalUnlock(hHCursors);
					GlobalFree(hHCursors);
					delete cursorObj;
					cursorObj = NULL;
					// Store cursorObj in  the runtime-Info Data of the object
					SetOBJDPtr(hDlg, theObject, (LONG)cursorObj);

					EndDialog(hDlg, 1);
					return (TRUE);

					}
				case IDCANCEL:
					if (cursorObj->timerID)
						KillAnim(hDlg, cursorObj);

					cursorObj->FrameCount = 0;
					delete cursorObj;
					cursorObj = NULL;
					// Store cursorObj in  the runtime-Info Data of the object
					SetOBJDPtr(hDlg, theObject, (LONG)cursorObj);

					EndDialog(hDlg, NULL);
					return (FALSE);

				case LOADICONID: {
					OPENFILENAME ofn;
					char szFile[256];
					int FILE_HANDLE = HFILE_ERROR;
					SERIUSICON tmpIcon ;
					szFile[0] = '\0';
					ofn.lpstrFile         = szFile;

					if (InitializeFOpen(&ofn, hDlg))
						{
						FILE_HANDLE = _lopen(ofn.lpstrFile, OF_READ);
						}

					if (FILE_HANDLE != HFILE_ERROR)
						{
						iconEditor->OpenIconFile(FILE_HANDLE);
						_lclose(FILE_HANDLE);
						}

					bICONLOADED = TRUE;
					break;
					}

				case NEWID:
					{
					char tmp[15];
					SERIUSICON blankIcon;
					HOTSPOT oldHotSpot;

					if (cursorObj->FrameCount > 39) {
						MessageBox(hDlg, "More than 40 Frames are not allowed","WARNING",  MB_OK);
						break;
						}

					oldHotSpot =  seriusIcon.bwCursor.cursorImage.HotSpot;
					SaveIconInDB(hDlg, cursorObj->nCurrentFrame, cursorObj);
					cursorObj->nCurrentFrame =++cursorObj->FrameCount;
					if (cursorObj->FrameCount >1) {
						SetScrollRange(hpixelWnd,
							SB_VERT, 1, cursorObj->FrameCount, TRUE);
						SetScrollPos(hpixelWnd, SB_VERT, cursorObj->FrameCount, TRUE);
						EnableWindow(iconEditor->hDelButton, TRUE);
						}
					bNewFrameAdded = TRUE;
					wsprintf(tmp, "%d of %d",cursorObj->nCurrentFrame, cursorObj->FrameCount);
					SetWindowText(iconEditor->hNFrames,(LPSTR)tmp);
					// init the hot spot for the new cursor
					seriusIcon.bwCursor.cursorImage.HotSpot = oldHotSpot;
					break;
					}
				case DELICONID:
					{
					char tmp[15];
					int indexToDelete = cursorObj->nCurrentFrame ;
					int nIconEdited = cursorObj->FrameCount;
					int newPos;
					int newFrame;
					/* Two Possibilities -> # of Frames >  1
																	# of Frames == 1
					*/

					if (indexToDelete == 1)
						{
						if (nIconEdited == 1)
              newFrame = 1;
						else
							newFrame = 2;
            newPos = 1;
						}
					else {
						newFrame = indexToDelete - 1;
            newPos = newFrame;
            }

          
					// if we are editing
					if (nIconEdited > 1)
						nIconEdited--;
					if (nIconEdited == 1)
            {
						EnableWindow(iconEditor->hDelButton, FALSE);
						}

					// The scroll range and position will be affected if we successfully
          // delete a cursor frame.
					SetScrollRange(hpixelWnd, //iconEditor->pixelWindow->hWndPixelWindow,
						SB_VERT, 1, nIconEdited , TRUE);

					if (nIconEdited >= 1) {
						lpHSIcons = (LPHSICONS) GlobalLock(hSIcons);
						seriusIcon = *(lpHSIcons + newFrame - 1); // REplace with new Icon
						GlobalUnlock(hSIcons);
						iconEditor->pixelWindow->ReplaceIcon((LPSERIUSICON)(&seriusIcon));
						iconEditor->pixelWindow->Redraw();
						SetScrollPos(iconEditor->pixelWindow->hWndPixelWindow, SB_VERT, newPos, TRUE);
						}
					if (!bNewFrameAdded) {
						DelIconInDB(hDlg, indexToDelete); //1 based index
						bNewFrameAdded = FALSE;
						}
					ShowScrollBar(hpixelWnd, SB_VERT, TRUE);
					cursorObj->nCurrentFrame = newPos;
					cursorObj->FrameCount = nIconEdited;
					// SKJ Nov.3, 1993
					// cursorObj->nCurrentFrame = cursorObj->FrameCount;
					wsprintf(tmp, "%d of %d",cursorObj->nCurrentFrame, cursorObj->FrameCount);
					SetWindowText(iconEditor->hNFrames,(LPSTR)tmp);
					break;
					}

				} // eof switch (wm_command)
		break;

		} // eof switch

	return bProcessed;
	}

// Flip the Icon Image
void FlipIcon(BYTE *pIArray) {
	// Invert Image
	int size = PIXELCOUNT;
	int x, y;
	BYTE BMPBits[MASKARRAYSIZE]; 	// for BMPBits should we use IMAGEARRAYSIZE
	int pixelsPerByte = 8;   			// should be 2? for colored icons
	iScanLineBytes  = ((size/pixelsPerByte/sizeof(long)) +
			 (((size/pixelsPerByte)%sizeof(long)==0)?0:1))*sizeof(long);

	for (y = 0; y < 32; y++) {
	 for (x = 0; x < 32; x++) {

		 int imageIndex = ((size - 1 - y)*iScanLineBytes)+(x/pixelsPerByte);

		 int newImageIndex = (y * iScanLineBytes)+(x/pixelsPerByte);

		 BMPBits[newImageIndex] = pIArray[imageIndex];
		 }
	 }
	_fmemcpy(pIArray, BMPBits, MASKARRAYSIZE);

	}

// Function to save Handle to newly created ICON

void SaveIconInDB(HWND hWnd, int FrameIndex,
	CURSOROBJ *cursorObj) {
 	HANDLE tmpHandle = 0;
 	LPHCURSORS tmpPtr;
 	SERIUSICON FAR *tmpSPtr;
 	int i;
 	HDC hDC;
 	int nIconEdited = cursorObj->FrameCount;
 	SERIUSICON blankIcon;

 	int index;

 	// invert the image
	// FlipIcon(&seriusIcon);

 	if (iconEditor->theIcon)
		iconEditor->pixelWindow->SaveIcon(iconEditor->theIcon);

 	index = FrameIndex;
 	if (hSIcons)
	 	lpHSIcons = (LPHSICONS)GlobalLock(hSIcons);
 	if (FrameIndex == nIconEdited) {
	 	tmpHandle = GlobalAlloc(GMEM_MOVEABLE, (nIconEdited)* sizeof(SERIUSICON));
	 	tmpSPtr = (SERIUSICON FAR*)GlobalLock(tmpHandle);
	 	i = (int)GlobalSize(tmpHandle);
	 	for (i = 0; i < nIconEdited - 1; i++)
		 	*(tmpSPtr+i) = *(lpHSIcons + i);

	 	GlobalUnlock(hSIcons);
	 	GlobalFree(hSIcons);
	 	hSIcons = tmpHandle;
	 	lpHSIcons = tmpSPtr;
	 	index = nIconEdited;

	 	}



	// copy the new cursor
 	_fmemcpy((void far*)(lpHSIcons + index - 1),(void far*)&seriusIcon, sizeof(SERIUSICON));



 	if (hSIcons)
	 	GlobalUnlock(hSIcons); //hSIcons);   // Unlock the last lock

 	if (hHCursors)
	 	lpHCursors = (LPHCURSORS)GlobalLock(hHCursors);

 	index = FrameIndex;
 	if ( FrameIndex == nIconEdited) {
	 	tmpHandle = GlobalAlloc(GMEM_MOVEABLE, nIconEdited * sizeof(HCURSOR));
	 	tmpPtr = (LPHCURSORS)GlobalLock(tmpHandle);
	 	for (i = 0; i < nIconEdited - 1; i++)
		 	*(tmpPtr+i) = *(lpHCursors + i);


	 	GlobalUnlock(hHCursors);
	 	GlobalFree(hHCursors);
	 	hHCursors = tmpHandle;
	 	lpHCursors = tmpPtr;

	 	index = nIconEdited;
	 	}
 	hDC = GetDC(hWnd);
	*(lpHCursors + index -1) = MakeCursor(hDC, (HICON)hArrowCursor, seriusIcon); // store the handle in the global memory

 	if (hHCursors)
	 	GlobalUnlock(hHCursors);  // Unlock the handle
 	ReleaseDC(hWnd, hDC);
 	bICONLOADED = FALSE;
	}


// Save the Cursor at an old spot
// Called when we are simply scrolling along

void SaveOldIconInDB(HWND hWnd, int FrameIndex, int nIconEdited)
	{
	HDC hDC;
	int index;

	// invert the image
	// FlipIcon(&seriusIcon);

	if (iconEditor->theIcon)
		iconEditor->pixelWindow->SaveIcon(iconEditor->theIcon);


	if ((hSIcons) && (hHCursors)) {
		lpHSIcons = (LPHSICONS)GlobalLock(hSIcons);
		}
	else
		return;
	// copy the new cursor at the old spot

	_fmemcpy((void far*)(lpHSIcons + FrameIndex - 1),(void far*)&seriusIcon, sizeof(SERIUSICON));

 	if (hSIcons)
	 	GlobalUnlock(hSIcons); //hSIcons);   // Unlock the last lock

	if (hHCursors)
	 	lpHCursors = (LPHCURSORS)GlobalLock(hHCursors);

 	index = FrameIndex;
 	hDC = GetDC(hWnd);

	*(lpHCursors + FrameIndex -1) = MakeCursor(hDC, (HICON)hArrowCursor, seriusIcon); // store the handle in the global memory

 	if (hHCursors)
	 	GlobalUnlock(hHCursors);  // Unlock the handle
 	ReleaseDC(hWnd, hDC);

 	I_UNUSED(index);
 	I_UNUSED(nIconEdited);
 	}



// Function to save Handle to newly created ICON

void DelIconInDB(HWND hWnd, int FrameIndex) {
 	HANDLE tmpHandle;
 	LPHCURSORS tmpPtr;
 	int i;
 	HANDLE tmpSIHandle;
 	LPHSICONS tmpSIPtr;
 	LPCURSOROBJ	cursorObj = (LPCURSOROBJ)GetOBJDPtr(hWnd);
 	lpHSIcons = (LPHSICONS)GlobalLock(hSIcons);
 	tmpHandle = GlobalAlloc(GMEM_MOVEABLE, (cursorObj->FrameCount - 1)* sizeof(HCURSOR));
 	tmpPtr = (LPHCURSORS)GlobalLock(tmpHandle);

 	tmpSIHandle = GlobalAlloc(GMEM_MOVEABLE, (cursorObj->FrameCount)* sizeof(SERIUSICON));
 	tmpSIPtr = (LPHSICONS)GlobalLock(tmpSIHandle);


	for (i = 0; i < cursorObj->FrameCount; i++)
		{
	 	if (i == FrameIndex - 1){ // 0 based index

			lpHSIcons ++;
		 	}
	 	else  {
			*(tmpSIPtr ++) = *(lpHSIcons ++);
		 	}
		}

	lpHCursors = (LPHCURSORS)GlobalLock(hHCursors);

	for (i = 0; i < cursorObj->FrameCount; i++) {
		if (i == FrameIndex - 1){ // 0 based index
		 	lpHCursors ++;
			}
	 	else  {
			*(tmpPtr++) = *(lpHCursors++);
			}
		}
	GlobalUnlock(hHCursors);

 	GlobalFree(hHCursors);
 	GlobalUnlock(hSIcons);
 	GlobalFree(hSIcons);
	hSIcons = tmpSIHandle;
 	hHCursors = tmpHandle;
	GlobalUnlock(hSIcons);
 	GlobalUnlock(hHCursors);
 	I_UNUSED(hWnd);
	}

