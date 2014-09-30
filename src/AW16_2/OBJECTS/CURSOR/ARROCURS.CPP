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

extern HANDLE hInst;
extern HANDLE hlastHiddenCursor;

extern BOOL StopAllCursorAnim();
extern "C" void FAR PASCAL _export ArrowCursor(
	pAEvtInfo theEvent,
	pAObjMessage TheSystem) {

	if (TheSystem->message1 == 0)
		{
		HANDLE hCursor  = LoadCursor(NULL, IDC_ARROW);  // (IDC_WAIT)
		long i  = AOBJ_GETFIRSTKEY;

		OBJECTID cursorID = AObjGetNextObject((TYPEID)OTYPE_CURSOR, (LONG *)&i);
		

		StopAllCursorAnim();
		// This event is sent to WINDOW object so that I can set the
		// cursor to all WINDOW OBJECTS
		// AEvtCallMethod(cursorID, windowID, OTYPE_WINDOW, OCURS_EVT_SETTHECURSOR, (LONG)hCursor);
		OWndSetCursor((HICON)hCursor, TRUE);
		SetCursor((HICON)hCursor);
		// save the handle in the Global Cursor
		hlastHiddenCursor = hCursor;
		LPHANDLE lpCursor = (LPHANDLE)AObjLockData(cursorID, TMPDATA);

		if (lpCursor)  // if there is a cursor object in the project
			 *lpCursor = hlastHiddenCursor;
	
		AObjUnlockData(cursorID, TMPDATA);

		while (ShowCursor(TRUE)< 0)     // show till display count is 0
				;
			TheSystem->message1 = 1;
		}

	I_UNUSED(theEvent);

	}
