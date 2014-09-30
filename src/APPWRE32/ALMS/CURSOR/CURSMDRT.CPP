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
#include "op_wnd.h"
#include "o_wnd.h"
#include "cursor.h"
#define CURSORMDRT


extern HANDLE hInst;
extern HANDLE hArrowCursor;

extern int CursorAnimate(AOBJECTID, int);
HANDLE hlastHiddenCursor= 0;
extern HCURSOR MakeCursor(HDC hDC, HICON hIcon, SERIUSICON sI);



// this function stops all the current Animation (s)
BOOL StopAllCursorAnim() 
  { // stop all cursor ANimation
	AOBJECTID CursorObject;
	BOOL rVal = FALSE;
	long i  = AOBJ_GETFIRSTKEY;


	i =  AOBJ_GETFIRSTKEY;
	while ((CursorObject = AObjGetNextObject((ATYPEID)OTYPE_CURSOR, (LONG *)&i)) != 0L){

	  LPCURSOROBJ lpCursorData = (LPCURSOROBJ) AObjLockData(CursorObject,CURSOROBJDATA);

		lpCursorData->timerID = 0;

    // unlock the previously acquired pointer
		AObjUnlockData(CursorObject,CURSOROBJDATA);

    rVal = TRUE;
		}
	return rVal;
	}



  
void DoShutdown()
	{
	long Index;
	AOBJECTID CursorObject;
	LPCURSOROBJ Data;
	//	for (Index = 0; Index <= oipGetObjectCount(OTYPE_CURSOR); Index++)
	Index = AOBJ_GETFIRSTKEY;
	while (( CursorObject = AObjGetNextObject((ATYPEID)OTYPE_CURSOR, &Index)) != 0L)
		{
		Data = (LPCURSOROBJ)(AObjLockData(CursorObject, CURSOROBJDATA));

		for (int i = 0; i < Data->FrameCount; i++) {
			DestroyCursor((HICON)Data->lpHCursors[ i]);
			}
		Data->timerID = 0;  // Stop all timers
		// unlock the previously acquired pointer
		AObjUnlockData(CursorObject,CURSOROBJDATA);

		}

	}


void BuildCursors(AOBJECTID theObject, HANDLE *hSIcons, HANDLE *hCursors){
	LPCURSOROBJ lpCursorData = (LPCURSOROBJ) AObjLockData(theObject, CURSOROBJDATA);
	HDC hDC;
	HWND hWnd = GetActiveWindow(); //  oipGetMasterWindow();
	int i;
	hDC = GetDC(hWnd);
	LPSERIUSICON lpIcon = (LPSERIUSICON)AObjLockData(theObject, SERIUSICONSDATA);
	int size = (int)AObjGetDataSize(theObject, CURSOROBJDATA);

	if (size < (sizeof(CURSOROBJ) + sizeof(HANDLE) * lpCursorData->FrameCount))
		{
		// unlock the previously acquired pointer
		AObjUnlockData(theObject,CURSOROBJDATA);
		AObjResizeData(theObject, CURSOROBJDATA, sizeof(CURSOROBJ) + sizeof(HANDLE) * lpCursorData->FrameCount);
		lpCursorData = (CURSOROBJ *)AObjLockData(theObject,CURSOROBJDATA);

    }
	// allocate a buffer to hold all the handles
	*hSIcons = GlobalAlloc(GMEM_FIXED, (lpCursorData->FrameCount) * sizeof(SERIUSICON));
	*hCursors = GlobalAlloc(GMEM_FIXED, lpCursorData->FrameCount * sizeof(HANDLE));

	LPSERIUSICON lpoldSeriusIcon = (LPSERIUSICON) GlobalLock(*hSIcons);
	LPHANDLE      lpCursors = (LPHANDLE) GlobalLock(*hCursors);

	hArrowCursor = LoadCursor(NULL, IDC_ARROW);
	for (i = 0; i < lpCursorData->FrameCount; i++) {
		// Copy icons from the Data to the Global Memory block
		lpoldSeriusIcon[i] = (SERIUSICON) *(lpIcon + i);

		lpCursorData->lpHCursors[ i] = MakeCursor(hDC, (HICON)hArrowCursor, lpoldSeriusIcon[i]); // store the handle in the global memory
		lpCursors[i] = lpCursorData->lpHCursors[i];

		}


	GlobalUnlock(*hSIcons);
	GlobalUnlock(*hCursors);


	ReleaseDC(hWnd, hDC);

	// unlock the previously acquired pointer
	if (lpCursorData)
		AObjUnlockData(theObject,CURSOROBJDATA);

	if (lpIcon)
		AObjUnlockData(theObject, SERIUSICONSDATA);
	}




extern "C" void CALLBACK mdrtPROC (
	pAEvtInfo 				theEvent,
	pAObjMessage      theSystem)
	{
	AOBJECTID CursorObject; // = theEvent->targetObject;

	if (theSystem->message1 == 0)
		{
		switch ((long)theEvent->what)
		 {
		 case AEVENT_STARTUP:
       {
			 HANDLE hSeriusIcons;
			 HANDLE hCursors;
			 BOOL isFirst;
			 long t =  AOBJ_GETFIRSTKEY;
			 while ((CursorObject = AObjGetNextObject((ATYPEID)OTYPE_CURSOR, &t)) != 0L)
				 {
			 
				 AObjSetDataCount(CursorObject, TMPDATA + 1);
				 AObjResizeData(CursorObject, TMPDATA, sizeof(HANDLE));
				 isFirst = FALSE;
					 
				 BuildCursors(CursorObject,&hCursors, &hSeriusIcons );
				 GlobalFree(hCursors);
				 GlobalFree(hSeriusIcons);
				 I_UNUSED(isFirst);

				 }
			 }
			 break;
		 case OCURS_EVT_STARTANIMATION: {
			 CursorObject = theEvent->targetObject;
			 LPCURSOROBJ lpCursor = (LPCURSOROBJ)AObjLockData((AOBJECTID)CursorObject, CURSOROBJDATA);
			 // if the timer has not been stopped
			 if (lpCursor->timerID) {
				 CursorAnimate(CursorObject, lpCursor->timerID);
				 theEvent->when = AAppGetTime() + lpCursor->delayTime;
				 theEvent->how  = AEVT_ATMARK;
				 AEvtPost(theEvent);
				 }
			 AObjUnlockData((AOBJECTID)CursorObject, CURSOROBJDATA);

			 }
			 break;
		 case OCURS_EVT_KILLANIMATION:
		 	 break;
		 case AEVENT_SHUTDOWN: {
			 DoShutdown();
			 // This event is sent to WINDOW object so that I can set the
			 // cursor for all WINDOW OBJECTs back to the ARROW
			 SetCursor((HICON)hArrowCursor);
			 while (ShowCursor(TRUE) < 0)
				 ;
			 // AEvtCallMethod(CursorObject, windowID, OTYPE_WINDOW, OCURS_EVT_SETTHECURSOR, (LONG)hArrowCurs);
			 OWndSetCursor((HICON)hArrowCursor, TRUE);
			 }
			 break;

		 
		 default:
			 break;
		
		 }
	 }
 }

