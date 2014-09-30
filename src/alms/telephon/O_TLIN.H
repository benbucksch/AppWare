/*************************************************************************
**	File Name:
**		O_TLIN.H			Copyright 1994 Novell Inc.
**
**	Description:
**		AppWare Telephony Services TLine Object Construction Kit header.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

//	Include files

#include <windows.h>
#include <a_alm.h>
#include <a_almutl.h>
#include "acs.h"
#include "csta.h"

//	Exported functions

#ifdef __cplusplus
extern "C" {
#endif

//	AppWare (CSTA) TLine Object Construction Kit API

BOOL ALMCALLBACK OTLinSetDeviceID(OBJECTID oiTLine, LPSTR szDeviceID);
BOOL ALMCALLBACK OTLinGetDeviceID(OBJECTID oiTLine, LPSTR szDeviceID, LONG bufLen);
BOOL ALMCALLBACK OTLinSetLineLetter(OBJECTID oiTLine, char cLineLetter);
char ALMCALLBACK OTLinGetLineLetter(OBJECTID oiTLine);
BOOL ALMCALLBACK OTLinSetCallID(OBJECTID oiTLine, LONG lCallID);
BOOL ALMCALLBACK OTLinGetCallID(OBJECTID OTLine, LONG * lpCallID);
BOOL ALMCALLBACK OTLinSetLocalState(OBJECTID OTLine, LCS lConnState);
LCS ALMCALLBACK OTLinGetLocalState(OBJECTID OTLine);
BOOL ALMCALLBACK OTLinSetActiveLine(OBJECTID oiTLine);
BOOL ALMCALLBACK OTLinSetCallInEffect(OBJECTID oiTLine, BOOL fState);

LONG ALMCALLBACK OTLinGetNumCxtns(OBJECTID OTLine);
MEMBLOCKID ALMCALLBACK OTLinGetNextCxtn(OBJECTID oiTLine, LONG * lKey);
LONG ALMCALLBACK OTLinAddCxtn(OBJECTID OTLine, MEMBLOCKID mTCxn);
MEMBLOCKID ALMCALLBACK OTLinFindCxtn(OBJECTID OTLine, char * lpszDeviceID);
MEMBLOCKID ALMCALLBACK OTLinRemoveCxtn(OBJECTID OTLine, char * lpszDeviceID);
BOOL ALMCALLBACK OTLinMerge2Calls(OBJECTID OTLinDst, OBJECTID OTLinSrc, LONG lNewCallID);

MEMBLOCKID ALMCALLBACK OTCxnCreate(void);
BOOL ALMCALLBACK OTCxnDestroy(MEMBLOCKID mTCxtn);
BOOL ALMCALLBACK OTCxnSetDeviceID(MEMBLOCKID mTCxtn, char * lpszDeviceID);
BOOL ALMCALLBACK OTCxnGetDeviceID(MEMBLOCKID mTCxtn, char * lpszDeviceID);
BOOL ALMCALLBACK OTCxnSetState(MEMBLOCKID mTCxtn, LCS lcs);
LCS ALMCALLBACK OTCxnGetState(MEMBLOCKID mTCxtn);
BOOL ALMCALLBACK OTCxnSetComment(MEMBLOCKID mTCxtn, char * lpszComment);
BOOL ALMCALLBACK OTCxnGetComment(MEMBLOCKID mTCxtn, char * lpszComment, LONG size);

#ifdef __cplusplus
}
#endif

