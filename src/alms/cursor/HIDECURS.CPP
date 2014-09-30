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
extern HANDLE hlastHiddenCursor;

extern HANDLE hInst;


extern "C" void CALLBACK HideCursor(
	pAEvtInfo theEvent,
	pAObjMessage TheSystem) {

	if (TheSystem->message1 == 0)
		{
		long i = AOBJ_GETFIRSTKEY;

		// AOBJECTID windowID = AObjGetNextObject(OTYPE_WINDOW, (LONG *)&i);
		i  = AOBJ_GETFIRSTKEY;
		AOBJECTID cursorID = AObjGetNextObject((ATYPEID)OTYPE_CURSOR, (LONG *)&i);
		if (cursorID > 0) {
		  LPHANDLE lpCursor = (LPHANDLE)AObjLockData(cursorID, TMPDATA);
			hlastHiddenCursor = *lpCursor;
			AObjUnlockData(cursorID, TMPDATA);

			SetCursor(NULL);

			// set the ShowCounter to < 0
			while (ShowCursor(FALSE) >= 0)
				;

			TheSystem->message1 = 1;
			}
		else { // 3/22/93
			TheSystem->message1 = 0;
			lstrcpy(TheSystem->errorMessage, "Need a Cursor Object in Project.");


			}
		}

	I_UNUSED(theEvent);
	}
