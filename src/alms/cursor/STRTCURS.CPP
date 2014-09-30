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
#include "cursor.h"
#include "op_wnd.h"
#include "o_wnd.h"
extern HANDLE hlastHiddenCursor;
extern HANDLE hArrowCursor;

AEvtInfo RepeatEvent;
static AOBJECTID GBLobjectID;


BOOL CALLBACK TaskWindowCallBack(
	HWND hWnd,
	LONG lhCursor) {
  HWND hMouseWnd;  // Window that contains the Mouse
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	hMouseWnd = WindowFromPoint(pt);
	if (
	  (hWnd == hMouseWnd) ||
		IsChild(hWnd, hMouseWnd)
		)
		{
		ScreenToClient(hWnd, &pt);

		GetClientRect(hWnd, &rect);

		if (PtInRect(&rect, pt))   { // Show cursor only in Client Area
//			SetCursor((HICON)LOWORD(lhCursor));
			SetCursor((HCURSOR)lhCursor);

			// save the handle in the Global Cursor
//			hlastHiddenCursor = (HICON)LOWORD(lhCursor);
			hlastHiddenCursor = (HANDLE)lhCursor;
	

			
      }
		return FALSE;
		}
	return TRUE;
	}


int CursorAnimate(
	AOBJECTID CursorObject,
	int ID
	) {
	static int nCount = 0;

	HANDLE hCursor;
	int index;
//	HANDLE hTask;
	DWORD	threadID;


	LPCURSOROBJ lpCursorObj = (LPCURSOROBJ)AObjLockData((AOBJECTID)CursorObject, CURSOROBJDATA);
	if (!lpCursorObj)
		return 0;
	index = nCount % lpCursorObj->FrameCount;
	hCursor = *(lpCursorObj->lpHCursors + index);
	nCount++;
	if (index == lpCursorObj->FrameCount - 1)
		nCount = 0;

	// unlock the previous DATA ptr
	AObjUnlockData((AOBJECTID)CursorObject, CURSOROBJDATA);

	// set the cursor to this till the next cursor comes thro'

	/*AEvtCallMethod(CursorObject, windowID, OTYPE_WINDOW, OCURS_EVT_SETTHECURSOR, (LONG)hCursor);*/
	OWndSetCursor((HICON)hCursor, TRUE);   // MASTER CURSOR


//	hTask = GetCurrentTask();
	threadID = GetCurrentThreadId();

	// Enumerate all the windows assoc. with this task and see if the cursor
	// is within any of these windows.
	// If so, then animate the cursor else do not
//	EnumThreadWindows((HTASK)hTask, (WNDENUMPROC)TaskWindowCallBack, (long)hCursor);
	EnumThreadWindows(threadID, (WNDENUMPROC)TaskWindowCallBack, (long)hCursor);

	LPHANDLE lpCursor = (LPHANDLE)AObjLockData((AOBJECTID)CursorObject, TMPDATA);
	*lpCursor = hlastHiddenCursor;
	// unlock the previous DATA ptr
	AObjUnlockData((AOBJECTID)CursorObject, TMPDATA);

		;
	I_UNUSED(ID);
	return 1;

	}


void StartAnim(
	LPCURSOROBJ lpCursor,
	pAObjMessage theSystem)
	{
	HANDLE   hCursor;
	if (lpCursor->FrameCount <= 0)
		return;
	GBLobjectID = lpCursor->objectID;
//	lpCursor->timerID = (int)GetCurrentTask() + (int)lpCursor->objectID;
	lpCursor->timerID = (int)GetCurrentThreadId() + (int)lpCursor->objectID;

	// save the handle in the Global Cursor
	hlastHiddenCursor = hArrowCursor;
	LPHANDLE lpHandle = (LPHANDLE)AObjLockData(lpCursor->objectID, TMPDATA);
	*lpHandle = hlastHiddenCursor;
		// unlock the previous DATA ptr
	AObjUnlockData(lpCursor->objectID, TMPDATA);

	// This event is sent to WINDOW object so that I can set the
	// cursor to all WINDOW OBJECTS
	// AEvtCallMethod(lpCursor->objectID, windowID, OTYPE_WINDOW, HideTheCursor, (LONG)0);
	SetCursor(NULL); // <-- TO HIDE THE CURSOR

	// AEvtCallMethod(lpCursor->objectID, windowID, OTYPE_WINDOW, OCURS_EVT_SETTHECURSOR, (LONG)0);
	OWndSetCursor(0, FALSE);
	while (ShowCursor(FALSE) < 0)
		;


	// Start by atleast showing the first frame
	hCursor = *(lpCursor->lpHCursors + 0);
	SetCursor((HICON)hCursor);


	CursorAnimate(lpCursor->objectID, lpCursor->timerID);
	// RepeatEvent.sourceObject = lpCursor->objectID;
	RepeatEvent.what = (AEVENTID)OCURS_EVT_STARTANIMATION;
	RepeatEvent.targetObject = lpCursor->objectID ;
	RepeatEvent.when = AAppGetTime() + lpCursor->delayTime;
	RepeatEvent.how  |= AEVT_ATMARK | AEVT_DIRECT;
	AEvtPost(&RepeatEvent);



	if (lpCursor->timerID) {
		theSystem->message1 = 1;
		AEvtPostSignalAtTail(lpCursor->objectID, CursorStartSignal);
		}
	else
		AFuncReportResult(theSystem, 0, "Cannot Start TIMER!");
	}

extern "C" void CALLBACK StartCursor(
	pAEvtInfo theEvent,
	pAObjMessage TheSystem) {

	if (TheSystem->message1 == 0)
		{

		AOBJECTID objectID;
		RepeatEvent = *theEvent;
		if (( objectID = AFuncGetTypedParameter(1, (ATYPEID)OTYPE_CURSOR)) != 0L)
			{


			LPCURSOROBJ Data = (LPCURSOROBJ)AObjLockData(objectID, DATA1);
			Data->objectID = objectID;
			if (Data->FrameCount)  {
				// unlock before you call the StartAnim routine
			  AObjUnlockData(objectID, DATA1);
				StartAnim(Data, TheSystem);
				}
			else
				AObjUnlockData(objectID, DATA1);

			}
		else
		 AFuncReportResult(TheSystem, 0, "No Cursor parameter passed!");

		}
	}
