/*************************************************************************
**	File Name:
**		TLINMDRT.CPP	Copyright 1994 Novell Inc.
**
**	Description:
**      The text to object and object to text conversion routines for
**		the StopLight Window Objecct.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

#define STRICT
#define ASTRICT

#include <a_alm.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"

#ifdef __cplusplus
extern "C" {
#endif

void ALMCALLBACK Stp_ObjectToText(pAEvtInfo theEvent, pAObjMessage theSystem);
void ALMCALLBACK Stp_TextToObject(pAEvtInfo theEvent, pAObjMessage theSystem);

#ifdef __cplusplus
}
#endif

/*************************************************************************
**	Function:
**		Stp_ObjectToText			(AppWare Bus)
**
**	Description:
**		The ObjectToText conversion method for the StopLight
**		Window Object.
*/

#pragma argsused
void ALMCALLBACK Stp_ObjectToText(pAEvtInfo theEvent, pAObjMessage theSystem) {
	LPSTOPLIGHT	lpStp = (LPSTOPLIGHT)AObjLockData(theEvent->targetObject, 0);
	MEMBLOCKID	textMID = (MEMBLOCKID) theEvent->details;
	LPSTR		text;

	switch(lpStp->color) {
		case red:
			AMemSetSize(textMID, 4);
			text = (LPSTR)AMemLock(textMID);
			lstrcpy(text, "red");
			break;

		case yellow:
			AMemSetSize(textMID, 7);
			text = (LPSTR)AMemLock(textMID);
			lstrcpy(text, "yellow");
			break;

		case green:
			AMemSetSize(textMID, 6);
			text = (LPSTR)AMemLock(textMID);
			lstrcpy(text, "green");
			break;
    }
    AMemUnlock(textMID);
	AObjUnlockData(theEvent->targetObject, 0);
	return;
}

/*************************************************************************
**	Function:
**		Stp_TextToObject			(AppWare Bus)
**
**	Description:
**		The TextToObject conversion method for the StopLight
**		Window Object.
*/

#pragma argsused
void ALMCALLBACK Stp_TextToObject(pAEvtInfo theEvent, pAObjMessage theSystem) {
	LPSTOPLIGHT	lpStp	= (LPSTOPLIGHT)AObjLockData(theEvent->targetObject, 0);
	MEMBLOCKID	textMID	= (MEMBLOCKID) theEvent->details;
	LPSTR		text	= (LPSTR)AMemLock(textMID);

	switch(text[0] | 0x20) {	//	Change to lower case
		case 'r':
			ChangeStopLightColor(theEvent->targetObject, lpStp, red);
			break;

		case 'y':
			ChangeStopLightColor(theEvent->targetObject, lpStp, yellow);
			break;

		case 'g':
			ChangeStopLightColor(theEvent->targetObject, lpStp, green);
			break;
	}
    AMemUnlock(textMID);
	AObjUnlockData(theEvent->targetObject, 0);
	return;
}

