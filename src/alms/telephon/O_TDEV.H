/*************************************************************************
**	File Name:
**		O_TDEV.H			Copyright 1994 Novell Inc.
**
**	Description:
**		AppWare Telephony Services TDEV Object Construction Kit header.
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

#define TDEV_FIRSTLINE	  0L

//	Exported functions

#ifdef __cplusplus
extern "C" {
#endif

//	AppWare (TDEV) Object Construction Kit API

BOOL ALMCALLBACK OTDevSetStreamHandle(OBJECTID oiTDevc, ACS acsHandle);
ACS  ALMCALLBACK OTDevGetStreamHandle(OBJECTID oiTDevc);
BOOL ALMCALLBACK OTDevSetSendQSize(OBJECTID oiTDevc, LONG sendQSize);
LONG ALMCALLBACK OTDevGetSendQSize(OBJECTID oiTDevc);
BOOL ALMCALLBACK OTDevSetRecvQSize(OBJECTID oiTDevc, LONG recvQSize);
LONG ALMCALLBACK OTDevGetRecvQSize(OBJECTID oiTDevc);
BOOL ALMCALLBACK OTDevSetServerID(OBJECTID oiTDevc, LPSTR szServerID);
BOOL ALMCALLBACK OTDevGetServerID(OBJECTID oiTDevc, LPSTR szServerID, LONG bufLen);
BOOL ALMCALLBACK OTDevSetLoginID(OBJECTID oiTDevc, LPSTR szLoginID);
BOOL ALMCALLBACK OTDevGetLoginID(OBJECTID oiTDevc, LPSTR szLoginID, LONG bufLen);
BOOL ALMCALLBACK OTDevSetMonitorDevice(OBJECTID oiTDevc, LPSTR szExtension);
BOOL ALMCALLBACK OTDevGetMonitorDevice(OBJECTID oiTDevc, LPSTR szExtension, LONG bufLen);
BOOL ALMCALLBACK OTDevSetPassword(OBJECTID oiTDevc, LPSTR szPassword);
BOOL ALMCALLBACK OTDevGetPassword(OBJECTID oiTDevc, LPSTR szPassword, LONG bufLen);
LONG ALMCALLBACK OTDevGetNumLines(OBJECTID oiTDevc);
OBJECTID ALMCALLBACK OTDevGetNextLine(OBJECTID oiTDevc, LONG * lKey);
LONG ALMCALLBACK OTDevFindLine(OBJECTID oiTDevc, LONG lCallID);
OBJECTID ALMCALLBACK OTDevGetActiveLine(OBJECTID oiTDevc);

#ifdef __cplusplus
}
#endif

