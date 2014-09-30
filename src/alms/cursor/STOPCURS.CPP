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



extern InitCursors(AOBJECTID);
extern HANDLE hlastHiddenCursor;
extern hArrowCursor;


void StopAnimation(LPCURSOROBJ lpObject)
	{
	long t;
	AOBJECTID CursorObject;
	BOOL bNoMoreAnimation = TRUE;
	// To stop animation set the TIMER ID = 0
	// I check for this before animating
	lpObject->timerID = 0;
	t =  AOBJ_GETFIRSTKEY;
	while ((BOOL)(CursorObject = AObjGetNextObject((ATYPEID)OTYPE_CURSOR, &t))) {

		LPCURSOROBJ lpCursorData = (LPCURSOROBJ) AObjLockData(CursorObject,CURSOROBJDATA);

		if (lpCursorData->timerID)
			bNoMoreAnimation = FALSE;
		AObjUnlockData(CursorObject,CURSOROBJDATA);

		}

	if (bNoMoreAnimation)  // if no cursors are being animated
		{										 // show the ArrowCursor
		// AEvtCallMethod(CursorObject, windowID, OTYPE_WINDOW, OCURS_EVT_SETTHECURSOR, (LONG)hArrowCursor);
		OWndSetCursor((HICON)hArrowCursor, TRUE);
		SetCursor((HICON)hArrowCursor);
		while (ShowCursor(TRUE) < 0)
			 ;
		 }
	}



extern "C" void CALLBACK StopCursor(
	pAEvtInfo TheEvent,
	pAObjMessage      TheSystem)
	{

	AOBJECTID CursorObject;
	LPCURSOROBJ  Data;




	if (TheSystem->message1 == 0)
		{
		CursorObject = AFuncGetParameter(1);
		if (AObjCheckType((AOBJECTID)CursorObject, (ATYPEID)OTYPE_CURSOR)) {

			Data = (LPCURSOROBJ)(AObjLockData(CursorObject, CURSOROBJDATA));
			if (Data->FrameCount > 0)  {
				StopAnimation(Data);				// InitCursor(Data);
				}
			AEvtPostSignalAtTail(Data->objectID, CursorStopSignal);
			TheSystem->message1 = 1;
			AObjUnlockData(CursorObject,CURSOROBJDATA);

			}
		else
			{ // stop all cursor ANimation

		 	long i  = AOBJ_GETFIRSTKEY;
			while ((BOOL)(CursorObject = AObjGetNextObject((ATYPEID)OTYPE_CURSOR, (LONG *)&i))) {

				LPCURSOROBJ lpCursorData = (LPCURSOROBJ) AObjLockData(CursorObject,CURSOROBJDATA);

			 	lpCursorData->timerID = 0;

			 	AObjUnlockData(CursorObject,CURSOROBJDATA);

			 	}
		 	// AEvtCallMethod(CursorObject, windowID, OTYPE_WINDOW, OCURS_EVT_SETTHECURSOR, (LONG)hArrowCursor);
		 	OWndSetCursor((HICON)hArrowCursor, TRUE);
			SetCursor((HICON)hArrowCursor);
		 	while (ShowCursor(TRUE) < 0)
			 ;

		 	}

		}

	I_UNUSED(TheEvent);
 	}



