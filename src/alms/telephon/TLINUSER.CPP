/*************************************************************************
**	File Name:
**		TLINUSER.CPP	Copyright 1994 Novell Inc.
**
**	Description:
**		The AppWare Line object's user support functions.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

//	Include files

#include <windows.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_wnd.h>
#include <o_text.h>
#include <o_number.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"
#include "o_tlin.h"
#include "tlinvisu.h"

/*************************************************************************
**	Notes:
**		The relationship between a TLine Object and the CSTA Telephony
**		services model is as follows: A TLine Object is directly
**		equivalent to a CSTA call. Attributes include Call ID and Call
**		State.
**
**			"Call behavior, including establishment and release,
**			can be observed and manipulated by the API. During some
**			phases of the call (e.g. establishment and release) the
**			call is not completely formed and there may be only a
**			single device involved.  In many operations, such as
**			conference and transfer, one device in a call is
**			replaced with another device or two calls are merged
**			into a single call. In these situations, the API call
**			object is maintained as long as the telecommunications
**			relationship remains across each operation..."
**								TSAPI Specification, Section 3.2.5.2
**
**		The number of TLine Objects in a CSTA Stream Object is limited
**		to TDEVC_MAXLINES.
**
**		Local Connection States:
**			CS_NULL 					CS_INITIATE
**			CS_ALERTING 				CS_CONNECT
**			CS_HOLD 					CS_QUEUED
**			CS_FAIL
**
**	----------------------------------------------------------------------
**
**	Function:
**		OTLinSetDeviceID
**
**	Description:
**		TLine data structure access method to set the device ID.
*/

BOOL ALMCALLBACK OTLinSetDeviceID(OBJECTID oiTLine, LPSTR szDeviceID) {
	LPTLINE 	lpTLine;
	AEvtInfo	event;
	BOOL		rc = FALSE;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		rc = (BOOL)OTextSetValueFromBuffer(lpTLine->oiPhone, szDeviceID);
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		event.sourceObject	= event.targetObject = oiTLine;
		event.what			= AEVENT_OBJECTCHANGED;
		event.how			= AEvtGetObjectChangedPriority(AEVT_ATTAIL);
		event.when			= AAppGetTime();
		event.details		= TLINE_DEVICEID;
		AEvtPost(&event);
	}
	return rc;
}

/*************************************************************************
**	Function:
**		OTLinGetDeviceID
**
**	Description:
**		TLine data structure access method to retrieve the device ID.
*/

BOOL ALMCALLBACK OTLinGetDeviceID(OBJECTID oiTLine, LPSTR szDeviceID, LONG bufLen) {
	LPTLINE lpTLine;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		OTextGetToBuffer(lpTLine->oiPhone, szDeviceID, bufLen);
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTLinSetLineLetter
**
**	Description:
**		Set the line letter for this line object.
*/

BOOL ALMCALLBACK OTLinSetLineLetter(OBJECTID oiTLine, char cLineLetter) {
	LPTLINE 	lpTLine;
	AEvtInfo	event;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		*lpTLine->szLineChar = cLineLetter;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		event.sourceObject	= event.targetObject = oiTLine;
		event.what			= AEVENT_OBJECTCHANGED;
		event.how			= AEvtGetObjectChangedPriority(AEVT_ATTAIL);
		event.when			= AAppGetTime();
		event.details		= TLINE_LINELETTER;
		AEvtPost(&event);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTLinGetLineLetter
**
**	Description:
**		Retrieves the line letter for this TLine Object.
*/

char ALMCALLBACK OTLinGetLineLetter(OBJECTID oiTLine) {
	LPTLINE lpTLine;
	char	cLineLetter = 0;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		cLineLetter = *lpTLine->szLineChar;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
	}
	return cLineLetter;
}

/*************************************************************************
**	Function:
**		OTLinSetCallID
**
**	Description:
**		Set the call ID for this TLine Object.
*/

BOOL ALMCALLBACK OTLinSetCallID(OBJECTID oiTLine, LONG lCallID) {
	LPTLINE 	lpTLine;
	AEvtInfo	event;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		lpTLine->lCallID = lCallID;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		event.sourceObject	= event.targetObject = oiTLine;
		event.what			= AEVENT_OBJECTCHANGED;
		event.how			= AEvtGetObjectChangedPriority(AEVT_ATTAIL);
		event.when			= AAppGetTime();
		event.details		= TLINE_CALLID;
		AEvtPost(&event);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTLinGetCallID
**
**	Description:
**		Retrieves the call ID for this TLine Object.
*/

BOOL ALMCALLBACK OTLinGetCallID(OBJECTID oiTLine, LONG * lpCallID) {
	LPTLINE lpTLine;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		*lpCallID = lpTLine->lCallID;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTLinSetLocalState
**
**	Description:
**		Sets the local state indicator for this TLine Object.
*/

BOOL ALMCALLBACK OTLinSetLocalState(OBJECTID oiTLine, LCS lConnState) {
	LPTLINE lpTLine;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		lpTLine->lConnState = lConnState;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTLinGetLocalState
**
**	Description:
**		Retrieves the local state indicator for this call block.
*/

LCS ALMCALLBACK OTLinGetLocalState(OBJECTID oiTLine) {
	LPTLINE lpTLine;
	LCS 	lcs = CS_NULL;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		lcs = lpTLine->lConnState;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
	}
	return lcs;
}

/*************************************************************************
**	Function:
**		OTLinSetActiveLine
**
**	Description:
**		Sets the active line indicator (red LED) on the given TLine.
*/

BOOL ALMCALLBACK OTLinSetActiveLine(OBJECTID oiTLine) {
	LPTDEVICE	lpTDevice;
	LPTLINE 	lpTLine, lpTLineTmp;
	AEvtInfo	event;
	int 		i;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		lpTDevice = (LPTDEVICE)AObjLockData(lpTLine->oiOwner, TDEVC_MAINBLK);
		if (lpTDevice) {
			for (i = 0; i < lpTDevice->lNumLines; i++) {
				lpTLineTmp = (LPTLINE)AObjLockData(lpTDevice->oiLines[i], TLINE_MAINBLK);
				if (lpTLineTmp) {
					if (lpTLineTmp->fActiveLine) {
						lpTLineTmp->fActiveLine = FALSE;
						event.sourceObject	= event.targetObject = lpTDevice->oiLines[i];
						event.what			= AEVENT_OBJECTCHANGED;
						event.how			= AEvtGetObjectChangedPriority(AEVT_ATTAIL);
						event.when			= AAppGetTime();
						event.details		= TLINE_REDLED;
						AEvtPost(&event);
					}
					AObjUnlockData(lpTDevice->oiLines[i], TLINE_MAINBLK);
				}
			}
			AObjUnlockData(lpTLine->oiOwner, TDEVC_MAINBLK);
		}
		lpTLine->fActiveLine = TRUE;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		event.sourceObject	= event.targetObject = oiTLine;
		event.what			= AEVENT_OBJECTCHANGED;
		event.how			= AEvtGetObjectChangedPriority(AEVT_ATTAIL);
		event.when			= AAppGetTime();
		event.details		= TLINE_REDLED;
		AEvtPost(&event);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTLinSetCallInEffect
**
**	Description:
**		Sets the call in effect indicator (green LED) on the given TLine.
*/

BOOL ALMCALLBACK OTLinSetCallInEffect(OBJECTID oiTLine, BOOL fState) {
	LPTLINE lpTLine;
	AEvtInfo	event;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		lpTLine->fCallInEffect = fState;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		event.sourceObject	= event.targetObject = oiTLine;
		event.what			= AEVENT_OBJECTCHANGED;
		event.how			= AEvtGetObjectChangedPriority(AEVT_ATTAIL);
		event.when			= AAppGetTime();
		event.details		= TLINE_GREENLED;
		AEvtPost(&event);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTLinGetNumCxtns
**
**	Description:
**		Retrieves the current number of connection blocks in this TLine
**		Object's connection block list, including the local connection.
**		Thus, a call which is being established will have a single
**		connection (local only) and will return 1, while a call which
**		has been established will have two connections (one local and
**		one remote) and will return 2.
*/

LONG ALMCALLBACK OTLinGetNumCxtns(OBJECTID oiTLine) {
	LPTLINE lpTLine;
	LONG	lNumCxtns;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		lNumCxtns = lpTLine->lNumCxtns;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
	}
	return lNumCxtns;
}

/*************************************************************************
**	Function:
**		OTLinGetNextCxtn
**
**	Description:
**		Iterative function which returns the next connection block
**		handle in the remote connection block list for this call block.
**		mCxtn is the iterator key for this function. Initialize it to
**		TLINE_FIRSTCXTN and use it over again until the function returns
**		FALSE.
*/

MEMBLOCKID ALMCALLBACK OTLinGetNextCxtn(OBJECTID oiTLine, LONG * lKey) {
	LPTLINE 	lpTLine;
	MEMBLOCKID	mReturn = (MEMBLOCKID)0;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		if (lpTLine->lNumCxtns)
			if (++(*lKey) < lpTLine->lNumCxtns)
				mReturn = lpTLine->mTCxtns[(UINT)*lKey];
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
	}
	return mReturn;
}

/*************************************************************************
**	Function:
**		OTLinAddCxtn
**
**	Description:
**		Adds a connection block to the TLine Object's list of connection
**		blocks.
*/

LONG ALMCALLBACK OTLinAddCxtn(OBJECTID oiTLine, MEMBLOCKID mTCxtn) {
	LPTLINE lpTLine;
	LPTCXTN lpTCxtn;
	LONG	lKey = -1L;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		if (lpTLine->lNumCxtns <= TLINE_MAXCXTNS) {
			lKey = lpTLine->lNumCxtns++;
			lpTLine->mTCxtns[(UINT)lKey] = mTCxtn;
			lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
			if (lpTCxtn) {
				lpTCxtn->oiOwningLine = oiTLine;
				AMemUnlock(mTCxtn);
			}
		}
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
	}
	return lKey;		// return index of lkey or -1L for error
}

/*************************************************************************
**	Function:
**		OTLinFindCxtn
**
**	Description:
**		Searches for and return the block ID of a connection with a
**		matching device ID string in the connection block list of a
**		TLine Object.
*/

MEMBLOCKID ALMCALLBACK OTLinFindCxtn(OBJECTID oiTLine, char * lpszDeviceID) {
	LPTLINE 	lpTLine;
	LPTCXTN 	lpTCxtn;
	MEMBLOCKID	mResult = (MEMBLOCKID)0;
	int 		i;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		for (i = 0; i < lpTLine->lNumCxtns; i++) {
			lpTCxtn = (LPTCXTN)AMemUnlock(lpTLine->mTCxtns[i]);
			if (lpTCxtn) {
				if (!_fstricmp(lpTCxtn->didDeviceID, lpszDeviceID)) {
					AMemUnlock(lpTLine->mTCxtns[i]);
					mResult = lpTLine->mTCxtns[i];
					break;
				}
				AMemUnlock(lpTLine->mTCxtns[i]);
			}
		}
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
	}
	return mResult;
}

/*************************************************************************
**	Function:
**		OTLinRemoveCxtn
**
**	Description:
**		Removes a connection block from the connection block list of a
**		TLine Object and returns its memblock ID. Since these memblocks
**		were created dynamically with a call to OTLinCreateCxtn, they
**		should be destroyed with a call to OTLinDestroyCxtn by the user
**		when he or she is finished with them. Returns 0 in case of an
**		error.
*/

MEMBLOCKID ALMCALLBACK OTLinRemoveCxtn(OBJECTID oiTLine, char * lpszDeviceID) {
	LPTLINE 	lpTLine;
	LPTCXTN 	lpTCxtn;
	MEMBLOCKID	mResult = (MEMBLOCKID)0;
	int 		i;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		for (i = 0; i < lpTLine->lNumCxtns; i++) {
			lpTCxtn = (LPTCXTN)AMemLock(lpTLine->mTCxtns[i]);
			if (lpTCxtn) {
				if (!_fstricmp(lpTCxtn->didDeviceID, lpszDeviceID)) {
					mResult = lpTLine->mTCxtns[i];
					lpTCxtn->oiOwningLine = (OBJECTID)0;
					AMemUnlock(lpTLine->mTCxtns[i]);
					CopyMemory(&lpTLine->mTCxtns[i], &lpTLine->mTCxtns[i+1],
						(size_t)(sizeof(MEMBLOCKID) * (--lpTLine->lNumCxtns - i)));
					break;
				}
				AMemUnlock(lpTLine->mTCxtns[i]);
			}
		}
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
	}
	return mResult;
}

/*************************************************************************
**	Function:
**		OTLinMerge2Calls
**
**	Description:
**		Appends the connection list in the source TLine Object to the
**		connection list in the destination TLine Object and reassigns the
**		new call ID to the destination TLine Object and the new owning
**		TLine Object ID to the new connections in the destination TLine
**		Object. This allows the user to quickly merge two conference or
**		regular calls into one conference call TLine Object.
*/

BOOL ALMCALLBACK OTLinMerge2Calls(OBJECTID oiTLineDst, OBJECTID oiTLineSrc, LONG lNewCallID) {
	LPTLINE lpTLineSrc;
	LPTLINE lpTLineDst;
	LPTCXTN lpTCxtn;
	LONG	lSCxtns;
	LONG	lDCxtns;
	BOOL	fResult = FALSE;
	int 	i;

	lpTLineSrc = (LPTLINE)AObjLockData(oiTLineSrc, TLINE_MAINBLK);
	if (lpTLineSrc) {
		lpTLineDst = (LPTLINE)AObjLockData(oiTLineDst, TLINE_MAINBLK);
		if (lpTLineDst) {
			lSCxtns = lpTLineSrc->lNumCxtns;
			lDCxtns = lpTLineDst->lNumCxtns;
			if (lSCxtns && lDCxtns && lSCxtns + lDCxtns <= TLINE_MAXCXTNS) {
				CopyMemory(&lpTLineDst->mTCxtns[(UINT)lDCxtns],
					&lpTLineSrc->mTCxtns[0], (size_t)lSCxtns);
				lpTLineDst->lNumCxtns += lpTLineSrc->lNumCxtns;
				lpTLineSrc->lNumCxtns = 0;
				lpTLineDst->lCallID = lNewCallID;
				for (i = 0; i < lpTLineDst->lNumCxtns; i++) {
					lpTCxtn = (LPTCXTN)AMemLock(lpTLineDst->mTCxtns[i]);
					if (lpTCxtn) {
						lpTCxtn->oiOwningLine = oiTLineDst;
						AMemUnlock(lpTLineDst->mTCxtns[i]);
					}
				}
				fResult = TRUE;
			}
			AObjUnlockData(oiTLineDst, TLINE_MAINBLK);
		}
		AObjUnlockData(oiTLineSrc, TLINE_MAINBLK);
	}
	return fResult;
}

/*************************************************************************
**	Notes:
**		The relationship between a connection block and the CSTA Telephony
**		services model is as follows:  A connection block is directly
**		equivalent to a CSTA connection.  Attributes include Connection
**		ID and Connection State.
**
**			"The Connection is a relationship between a call and a
**			device...CONNECTION IDENTIFIER -- a combination of the Call
**			Identifier and Device Identifier.  For a call there
**			are as many Connection identifiers as there are associated
**			devices, and for a device there are as many Connection
**			identifiers as there are associated calls."
**								TSAPI Specification, Section 3.2.5.3
**
**		The number of connection blocks in a call block is unlimited and
**		they are dynamically created and destroyed at runtime only.
**
**	----------------------------------------------------------------------
**
**	Function:
**		OTCxnCreate
**
**	Description:
**		Creates a new connection block.
*/

MEMBLOCKID ALMCALLBACK OTCxnCreate(void) {
	MEMBLOCKID	mTCxtn;
	LPTCXTN 	lpTCxtn;

	mTCxtn = AMemAllocate(sizeof(TCXTN));
	if (mTCxtn) {
		lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
		_fmemset(lpTCxtn, 0, sizeof(TCXTN));
		AMemUnlock(mTCxtn);
	}
	return mTCxtn;
}

/*************************************************************************
**	Function:
**		OTCxnDestroy
**
**	Description:
**		Destroys a connection block.
*/

BOOL ALMCALLBACK OTCxnDestroy(MEMBLOCKID mTCxtn) {
	return AMemFree(mTCxtn);
}

/*************************************************************************
**	Function:
**		OTCxnSetDeviceID
**
**	Description:
**		Sets the device ID string for the given connection block.
*/

BOOL ALMCALLBACK OTCxnSetDeviceID(MEMBLOCKID mTCxtn, char * lpszDeviceID) {
	LPTCXTN lpTCxtn;

	lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
	if (lpTCxtn) {
		CopyMemory(lpTCxtn->didDeviceID, lpszDeviceID, sizeof(DID));
		AMemUnlock(mTCxtn);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTCxnGetDeviceID
**
**	Description:
**		Returns the device ID string for the given connection block.
*/

BOOL ALMCALLBACK OTCxnGetDeviceID(MEMBLOCKID mTCxtn, char * lpszDeviceID) {
	LPTCXTN lpTCxtn;

	lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
	if (lpTCxtn) {
		CopyMemory(lpszDeviceID, &lpTCxtn->didDeviceID, sizeof(DID));
		AMemUnlock(mTCxtn);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTCxnSetState
**
**	Description:
**		Sets the connection state indicator of a connection block.
*/

BOOL ALMCALLBACK OTCxnSetState(MEMBLOCKID mTCxtn, LCS lcs) {
	LPTCXTN lpTCxtn;

	lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
	if (lpTCxtn) {
		lpTCxtn->lConnState = lcs;
		AMemUnlock(mTCxtn);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTCxnGetState
**
**	Description:
**		Returns the connection state indicator of a connection block.
*/

LCS ALMCALLBACK OTCxnGetState(MEMBLOCKID mTCxtn) {
	LPTCXTN lpTCxtn;
	LCS 	lcs = CS_NULL;

	lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
	if (lpTCxtn) {
		lcs = lpTCxtn->lConnState;
		AMemUnlock(mTCxtn);
	}
	return lcs;
}

/*************************************************************************
**	Function:
**		OTCxnSetComment
**
**	Description:
**		Sets the comment field of a connection block.
*/

BOOL ALMCALLBACK OTCxnSetComment(MEMBLOCKID mTCxtn, char * lpszComment) {
	LPTCXTN lpTCxtn;

	lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
	if (lpTCxtn) {
		lstrcpyn(lpTCxtn->szComment, lpszComment, TCXTN_MAXCMNT);
		AMemUnlock(mTCxtn);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTCxnGetComment
**
**	Description:
**		Returns the comment field of a connection block.
*/

BOOL ALMCALLBACK OTCxnGetComment(MEMBLOCKID mTCxtn, char * lpszComment, LONG size) {
	LPTCXTN lpTCxtn;

	lpTCxtn = (LPTCXTN)AMemLock(mTCxtn);
	if (lpTCxtn) {
		lstrcpyn(lpszComment, lpTCxtn->szComment, (size_t)size);
		lpszComment[(UINT)size - 1] = 0;
		AMemUnlock(mTCxtn);
		return TRUE;
	}
	return FALSE;
}

