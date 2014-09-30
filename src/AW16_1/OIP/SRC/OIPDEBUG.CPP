/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					OIPDEBUG.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Contains debug code for ALM DLL.  The functions implemented
//								in this module are declared in A_ALM.H and in INTDEBUG.H
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "almsys.h"
#include "intdebug.h"				// internal debug help
#include "system.h"

#include "object.h"
#include "sassoc2.h"				// two-key associative map
#include "oipdebug.h"

//needed so I_ASSERT will work
I_ENABLE_ASSERT; 

#ifdef ADBG_DEBUG

// Undef the debug macros so we can call the real procedures
#undef AObjLockData
#undef AObjUnlockData
extern "C" {
LPVOID  ALMAPI  AObjLockData      (OBJECTID  theObject,
                                    LONG      dataIndex);
BOOL    ALMAPI  AObjUnlockData    (OBJECTID  theObject,
                                    LONG      dataIndex);
}

#define DBG_MAXFILTERS		20
#define DBG_BREAKONINFO				0x1
#define DBG_BREAKONWARN				0x2
#define DBG_BREAKONERR				0x4
#define DBG_BREAKONUNKNOWN		0x8

#define DBG_TRACEEVENTS				0x1
#define DBG_TRACEWMSGS				0x2

#ifdef DEMO
static char g_szIniFile[]           = "APPWDEMO.INI";
#else
static char		g_szIniFile[]					= "APPWARE.INI";	// private .INI file name
#endif

static char		g_szIniDebug[]				= "Debug";				// Section name
static char		g_szIniLockLife[]			= "MaxLockLife";	// # of locks/unlocks before complaining
static char		g_szIniLockCount[]		= "MaxLockCount";	// max lock count before msg
static char		g_szIniTrace[]				= "TraceLevel";		// lowest printable message (0 = info)
static char		g_szIniBreak[]				= "BreakOn";			// "Abort,Break,Ignore" on {INFO,WARN,...}
static char		g_szIniFilter[]				= "TypeFilter";		// Print messages only for these types
static char		g_szIniTraceEvents[]	= "TraceEvents";	// Print out event info at runtime
static char		g_szIniTraceWMsgs[]		= "TraceWindowMsgs";// Print Windows MSG info at runtime

static WORD		g_wBreakOpt			= 0;
static LONG		g_nMaxLockLife	= -1;
static LONG		g_nMaxLockCount	= -1;
static WORD		g_wTraceOpt			= 0;
static LPSTR	g_szTypes[]			= {
																	"Info:  ",
																	"Warn:  ",
																	"Err:   ",
																	"???:   "
																};

typedef struct _DATALOCKINFO
	{
	LONG						lLine;						// < 0 implies unlock instead of lock
	_DATALOCKINFO*	pNext;						// next locker of (object,dataindex)
	char						szFile[13];				// 8.3 format for reduced memory usage
	}		DATALOCKINFO;

static SAssoc2*		g_pObjLock			= NULL;
static BOOL				g_fReadIni			= FALSE;
static int				g_nFilter				= ADBG_INFO;		// lowest level to print
static int				g_nFilterCount	= 0;
static TYPEID			g_tidFilters[DBG_MAXFILTERS];

typedef struct
	{
	LPSTR		pszName;
	WORD		wID;
	}		WMSGMAP;

static WMSGMAP*		g_pWindowsMsgMap;
static WORD				g_nMsgMapSize;
static WORD				g_nMsgMapCount;
static BOOL				g_fMapInitialized;

#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		ReadIniDebug
//
// DESCRIPTION:	Initializes filter information for ADbgPrint from the
//							.ini file.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
static VOID		ReadIniDebug(
	int&					nFilter,
	int&					nFilterCount,
	TYPEID*				tidFilters
	)
	{
	BOOL					fEnd					= FALSE;
	LPSTR					pScan;
	LPSTR					pType;
	char					szTypes[256];
	TYPEID				tid;

	g_nMaxLockLife  = GetPrivateProfileInt(g_szIniDebug, g_szIniLockLife,
																				-1, g_szIniFile);
	g_nMaxLockCount = GetPrivateProfileInt(g_szIniDebug, g_szIniLockCount,
																				-1, g_szIniFile);
	g_wBreakOpt     = GetPrivateProfileInt(g_szIniDebug, g_szIniBreak,
																				 0,  g_szIniFile);
	nFilter         = GetPrivateProfileInt(g_szIniDebug, g_szIniTrace,
																				 0,  g_szIniFile);

	if (GetPrivateProfileInt(g_szIniDebug, g_szIniTraceEvents,
																				0, g_szIniFile))
		g_wTraceOpt |= DBG_TRACEEVENTS;

	if (GetPrivateProfileInt(g_szIniDebug, g_szIniTraceWMsgs,
																				0, g_szIniFile))
		g_wTraceOpt |= DBG_TRACEWMSGS;


	// If we read garbage from the .INI for maxlocklife, then ignore it
	if (g_nMaxLockLife == 0)
		g_nMaxLockLife = -1;

	// If we read garbage from the .INI for maxlockcount, then ignore it
	if (g_nMaxLockCount == 0)
		g_nMaxLockCount = -1;

	GetPrivateProfileString(g_szIniDebug, g_szIniFilter,
													"", szTypes, sizeof(szTypes), g_szIniFile);

	for (pType = szTypes;;)
		{
		for (pScan = pType; *pScan != '\0' && *pScan != ';'; pScan++)
			;

		if (pScan == pType)						// hit \0
			break;
		else if (*pScan == '\0')			// valid end of string
			fEnd = TRUE;
		else													// middle of string
			*pScan = '\0';

		if (strlen(pType) != 4)				// TYPEIDs must be 4 ASCII chars
			{
			// Don't use DebugOutput() because we don't want this formatted
			OutputDebugString("Invalid TYPEID in ");
			OutputDebugString(g_szIniFile);
			OutputDebugString(":  ");
			OutputDebugString(pType);
			OutputDebugString("\r\n");
			}
		else
			{
			tid = MAKEID(pType[0], pType[1], pType[2], pType[3]);
			tidFilters[nFilterCount++] = tid;
			}

		if (fEnd)
			break;
		else if (nFilterCount < DBG_MAXFILTERS)
			pType = pScan + 1;
		else
			{
			OutputDebugString("Too many filter types in ");
			OutputDebugString(g_szIniFile);
			OutputDebugString(":  maximum = ");

			itoa(DBG_MAXFILTERS, szTypes, 10);
			OutputDebugString(szTypes);
			OutputDebugString("\r\n");
			break;
			}
		}
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		InitFromIni
//
// DESCRIPTION:	Reads debug information from the .INI file if it hasn't
//							been done already.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
static VOID InitFromIni(
	VOID
	)
	{
	// Read .ini debug information
	if (!g_fReadIni)
		{
		g_fReadIni = TRUE;
		ReadIniDebug(g_nFilter, g_nFilterCount, g_tidFilters);
		}
	}
#endif	// ADBG_DEBUG

////////////////////////////////////////////////////////////////
//
// FUNCTION:		InFilterList
//
// DESCRIPTION:	Returns TRUE if the type is in the filter list.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
BOOL	InFilterList(
	TYPEID		tid,
	TYPEID*		tidList,
	int				nTidCount
	)
	{
	for (; nTidCount >= 0; nTidCount--)
		{
		if (tid == tidList[nTidCount])
			break;
		}

	return (nTidCount >= 0);
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		FilterMessage
//
// DESCRIPTION:	Returns TRUE if the message should NOT be printed.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
static BOOL		FilterMessage(
	int						nType,
	TYPEID				tType
	)
	{
	BOOL		fFilter;

	if (nType >= g_nFilter &&
			(tType == 0 ||
				g_nFilterCount == 0 ||
				InFilterList(tType, g_tidFilters, g_nFilterCount)))
		fFilter = FALSE;
	else
		fFilter = TRUE;

	return fFilter;
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		ADbgPrintInt
//
// DESCRIPTION:	Prints out the given message via OutputDebugString if
//							the system has been compiled with ADBG_DEBUG.  The main use of
//							this will be to issue warnings 
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
static VOID		ADbgPrintInt(
	BOOL					fBreak,						// TRUE => DebugBreak() if appropriate
	int						nType,						// ADBG_E_{INFO,WARNING,ERROR}
	TYPEID				tType,						// current message is for given type
	LPSTR					pszFmt,						// message to print to debug monitor
	va_list				vaList						// va_start,end done elsewhere
	)
	{
	char					szMsg[256];

	I_UNUSED(tType);

	InitFromIni();

	if (!FilterMessage(nType, tType))
		{
		I_ASSERT(ADBG_INFO <= nType && nType <= ADBG_ERROR);

		if (nType < ADBG_INFO || nType > ADBG_ERROR)
			nType = ADBG_UNKNOWN;

		OutputDebugString(g_szTypes[nType]);

		wvsprintf(szMsg, pszFmt, vaList);

		OutputDebugString(szMsg);
		OutputDebugString("\r\n");

		if (fBreak &&
				((nType == ADBG_INFO    && (g_wBreakOpt & DBG_BREAKONINFO))  ||
				 (nType == ADBG_WARNING && (g_wBreakOpt & DBG_BREAKONWARN))  ||
				 (nType == ADBG_ERROR   && (g_wBreakOpt & DBG_BREAKONERR))   ||
				 (nType == ADBG_UNKNOWN && (g_wBreakOpt & DBG_BREAKONUNKNOWN))))
			DebugBreak();
		}
	}
#endif	

////////////////////////////////////////////////////////////////
//
// FUNCTION:		ADbgPrintAux
//
// DESCRIPTION:	Prints out the formatted message via ADbgPrintInt(), but
//							will NOT cause a break.
//
////////////////////////////////////////////////////////////////

static void	_cdecl	ADbgPrintAux(
	int						nType,						// ADBG_E_{INFO,WARNING,ERROR}
	TYPEID				tType,						// current message is for given type
	LPSTR					pszFmt,						// message to print to debug monitor
	...
	)
	{
#ifdef ADBG_DEBUG
	va_list				vaList;

	va_start(vaList, pszFmt);

	ADbgPrintInt(FALSE, nType, tType, pszFmt, vaList);

	va_end(vaList);

#else
	I_UNUSED(nType);
	I_UNUSED(tType);
	I_UNUSED(pszFmt);
#endif
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:		ADbgPrint
//
// DESCRIPTION:	Prints out the given message via OutputDebugString if
//							the system has been compiled with ADBG_DEBUG.  The main use of
//							this will be to issue warnings 
//
////////////////////////////////////////////////////////////////

extern "C"
short	ALMCAPI	ADbgPrint(
	short					nType,						// ADBG_E_{INFO,WARNING,ERROR}
	TYPEID				tType,						// current message is for given type
	LPSTR					pszFmt,						// message to print to debug monitor
	...
	)
	{
#ifdef ADBG_DEBUG
	va_list				vaList;

	va_start(vaList, pszFmt);

	ADbgPrintInt(TRUE, nType, tType, pszFmt, vaList);

	va_end(vaList);

#else
	I_UNUSED(nType);
	I_UNUSED(tType);
	I_UNUSED(pszFmt);
#endif

	return 0;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:		ADbgPrintRes
//
// DESCRIPTION:	Prints out the given message via OutputDebugString.  The
//							format string is retrieved from the string resource id
//							and HINSTANCE.  The format of the message is:
//
//							<module> <errcode>:  <message>
//
//							Both <module> and <errcode> are derived from the parameters:
//							module name is expected to be in the 0'th string resource
//							in hinst.
//
////////////////////////////////////////////////////////////////

extern "C"
short ALMCAPI	ADbgPrintRes(
	short					nType,
	TYPEID				tType,
	HINSTANCE			hInst,
	WORD					idRes,
	...
	)
	{
#ifdef ADBG_DEBUG
	char					szFmt[256];
	va_list				vaList;

	InitFromIni();

	if (!FilterMessage(nType, tType))
		{
		va_start(vaList, szFmt);

		// Issue "<module> <errcode>:  " stuff
		LoadString(hInst, 0, szFmt, sizeof(szFmt));			// 0 is module name
		OutputDebugString(szFmt);
		wsprintf(szFmt, " %d:  ", idRes);
		OutputDebugString(szFmt);

		// Now output real message
		LoadString(hInst, idRes, szFmt, sizeof(szFmt));

		ADbgPrintInt(TRUE, nType, tType, szFmt, vaList);

		va_end(vaList);
		}

#else
	I_UNUSED(nType);
	I_UNUSED(tType);
	I_UNUSED(hInst);
	I_UNUSED(idRes);
#endif

	return 0;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:		ADbgInternalPrint
//
// DESCRIPTION:	Reports an internal system error.  These errors are NOT
//							generated by bad information passed in from outside OIP, but
//							are a result of bad logic within OIP.
//
////////////////////////////////////////////////////////////////

extern "C"
int ALMAPI		ADbgInternalPrint(
	LPSTR					lpszExpr,
	LPSTR					lpszFile,
	LONG					lLine
	)
	{
#ifdef AW_I_DEBUG
	char					szMsg[256];

	if (lpszExpr)
		wsprintf(szMsg, "File %s, Line %ld, Expr %s",
						lpszFile, lLine, lpszExpr);
	else
		wsprintf(szMsg, "File %s, Line %ld",
						lpszFile, lLine);

	MessageBox(NULL, szMsg, "Assertion failure", MB_OK);
#else
	I_UNUSED(lpszExpr);
	I_UNUSED(lpszFile);
	I_UNUSED(lLine);
#endif

	return 0;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:		AddObjDataInfo
//
// DESCRIPTION:	Records the file,line that requested the object data for
//							debug purposes.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
static VOID		AddObjDataInfo(
	OBJECTID				theObject,
	LONG						dataIndex,
	LPSTR						lpszFile,
	LONG						lLine
	)
	{
	DATALOCKINFO*		pInfo;
	DATALOCKINFO*		pNewInfo;
	LPSTR						pszScan;
	LONG						lLockLife;
	LPSTR						pszMsg;

	if (g_pObjLock == NULL)
		g_pObjLock = new SAssoc2();

	if (g_pObjLock != NULL)						// for safety sake
		{
		pInfo = (DATALOCKINFO*) g_pObjLock->Get(theObject, dataIndex);

		if ((pNewInfo = new DATALOCKINFO) != NULL)
			{
			pNewInfo->lLine = lLine;
			pNewInfo->pNext = NULL;

			if (lpszFile && *lpszFile != '\0')
				{
				pszScan = lpszFile + strlen(lpszFile) - 1;
				for (; *pszScan != '\\' &&
							 *pszScan != '/'  &&
							 pszScan  >= lpszFile; pszScan--)
					;

				pszScan++;
				strncpy(pNewInfo->szFile, pszScan, sizeof(pNewInfo->szFile));
				}
			else
				pNewInfo->szFile[0] = '\0';

			if (pInfo == NULL)						// first in list
				{
				if (!g_pObjLock->Add(theObject, dataIndex, pNewInfo))
					delete pNewInfo;
				}
			else													// link in at tail
				{
				lLockLife = 0;
				for (; pInfo->pNext != NULL; pInfo = pInfo->pNext)
					lLockLife++;

				pInfo->pNext = pNewInfo;

				if (g_nMaxLockLife != -1 && lLockLife >= g_nMaxLockLife)
					{
					if ((lLockLife % g_nMaxLockLife) == 0)
						{
						PrintObjDataInfo(theObject, dataIndex);

						if (lLockLife == g_nMaxLockLife)
							pszMsg = "Exceeding MaxLockLife:  object %ld, data %ld; locklife = %ld";
						else
							pszMsg = "Greatly exceeding MaxLockLife:  object %ld, data %ld; locklife = %ld";

						ADbgPrint(ADBG_WARNING, 0, pszMsg, theObject, dataIndex, lLockLife);
						}
					}
				}
			}
		}
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		DeleteObjDataInfo
//
// DESCRIPTION:	Deletes all object info
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
static VOID		DeleteObjDataInfo(
	OBJECTID			theObject,
	LONG					dataIndex
	)
	{
	DATALOCKINFO*	pInfo;
	DATALOCKINFO*	pNext;

	if (g_pObjLock != NULL)
		{
		if ((pInfo = (DATALOCKINFO*) g_pObjLock->Get(theObject, dataIndex)) != NULL)
			{
			for (; pInfo != NULL; pInfo = pNext)
				{
				pNext = pInfo->pNext;
				delete pInfo;
				}

			g_pObjLock->Remove(theObject, dataIndex);
			}
		else
			ADbgPrint(ADBG_WARNING, 0, "No locking info in DeleteObjDataInfo");
		}
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		DeleteObjDataInfo
//
// DESCRIPTION:	Deletes all object data info on all data indices.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
VOID		DeleteObjDataInfo(
	OBJECTID			theObject
	)
	{
	DATALOCKINFO*	pInfo;
	DATALOCKINFO*	pNext;
	LONG					key;

	if (g_pObjLock != NULL)
		{
		key = 0;

		for (;;)
			{
			pInfo = (DATALOCKINFO*) g_pObjLock->Get(theObject, NULL, &key);
			if (pInfo == NULL)
				break;

			for (; pInfo != NULL; pInfo = pNext)
				{
				pNext = pInfo->pNext;
				delete pInfo;
				}
			}

		g_pObjLock->RemoveAll(theObject);
		}
	else
		ADbgPrint(ADBG_WARNING, 0, "No locking info in DeleteObjDataInfo");
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		PrintObjDataInfo
//
// DESCRIPTION:	Prints the object info to the debug monitor.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
VOID		PrintObjDataInfo(
	OBJECTID			theObject,
	LONG					dataIndex
	)
	{
	BOOL						fHeader		= FALSE;
	DATALOCKINFO*		pInfo;

	if (g_pObjLock != NULL)
		{
		if ((pInfo = (DATALOCKINFO*) g_pObjLock->Get(theObject, dataIndex)) != NULL)
			{
			for (; pInfo != NULL; pInfo = pInfo->pNext)
				{
				if (!fHeader)
					{
					ADbgPrintAux(ADBG_WARNING, 0, "Lock info for object %ld, data %ld:  ",
												theObject, dataIndex);
					fHeader = TRUE;
					}

				ADbgPrintAux(ADBG_WARNING, 0, "\tFile %s, line %ld:  %s",
											pInfo->szFile, labs(pInfo->lLine),
											(pInfo->lLine > 0) ? "Locked" : "Unlocked");
				}
			}
		else
			ADbgPrint(ADBG_WARNING, 0, "No locking info in PrintObjDataInfo");
		}
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		AObjLockDataDbg
//
// DESCRIPTION:	Records the file,line that requested the object data for
//							debug purposes.
//
////////////////////////////////////////////////////////////////

extern "C"
LPVOID ALMAPI		AObjLockDataDbg(
	OBJECTID				theObject,
	LONG						dataIndex,
	LPSTR						lpszFile,
	LONG						lLine
	)
	{
	LPVOID					pData;

	pData = AObjLockData(theObject, dataIndex); 


#ifdef ADBG_DEBUG
	if (pData != NULL)
		{
		LPOBJECTRECORD	pObject;

		InitFromIni();

		pTheSystem->GetCurrentContext();

		AddObjDataInfo(theObject, dataIndex, lpszFile, lLine);

		pObject = GetObjectRecord(theObject);

		I_ASSERT(pObject != NULL);

		if (g_nMaxLockCount >= 0 &&
					pObject->GetDataLockCount(dataIndex) > g_nMaxLockCount)
			{
			PrintObjDataInfo(theObject, dataIndex);

			ADbgPrint(ADBG_WARNING, 0,
											"Exceeded MaxLockCount in object %ld, data %ld",
											theObject, dataIndex);
			}
		}
#endif

	return pData;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:		AObjUnlockDataDbg
//
// DESCRIPTION:	Records where an object data was unlocked.  If the lock
//							count goes to zero, then all locking information is
//							deleted.
//
////////////////////////////////////////////////////////////////

extern "C"
BOOL ALMAPI		AObjUnlockDataDbg(
	OBJECTID				theObject,
	LONG						dataIndex,
	LPSTR						lpszFile,
	LONG						lLine
	)
	{
	BOOL						fUnlocked;

	fUnlocked = AObjUnlockData(theObject, dataIndex);

#ifdef ADBG_DEBUG
	if (fUnlocked)
		{
		LPOBJECTRECORD	pObject;

		pTheSystem->GetCurrentContext();

		pObject = GetObjectRecord(theObject);

		I_ASSERT(pObject != NULL);

		if (pObject->GetDataLockCount(dataIndex) == 0)
			DeleteObjDataInfo(theObject, dataIndex);
		else
			// Record the negative line number to denote it was unlocked.
			AddObjDataInfo(theObject, dataIndex, lpszFile, -lLine);
		}
#endif

	return fUnlocked;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:		PrintEventInfo
//
// DESCRIPTION:	Prints out trace information.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
VOID		PrintEventInfo(
	pAEvtInfo		pEvent,
	long				nodeID
	)
	{
	InitFromIni();

	if (g_wTraceOpt & DBG_TRACEEVENTS)
		ADbgPrint(ADBG_INFO, 0, "Dispatch event '%c%c%c%c': %ld -> %ld, %s, ID = %li",
												(char) (pEvent->what >> 24),
												(char) (pEvent->what >> 16),
												(char) (pEvent->what >>  8),
												(char) (pEvent->what      ),
												pEvent->sourceObject,
												pEvent->targetObject,
												(!(pEvent->how & AEVT_ATMARK)) ? "TAIL" : "MARK",
												nodeID);
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		PrintEventPush
//
// DESCRIPTION:	Prints out trace information when the event is pushed.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
VOID		PrintEventPush(
	pAEvtInfo		pEvent,
	long nodeID
	)
	{
	InitFromIni();

	if (g_wTraceOpt & DBG_TRACEEVENTS)
		ADbgPrint(ADBG_INFO, 0, "Pushing event  '%c%c%c%c': %ld -> %ld, %s, ID = %li",
												(char) (pEvent->what >> 24),
												(char) (pEvent->what >> 16),
												(char) (pEvent->what >>  8),
												(char) (pEvent->what      ),
												pEvent->sourceObject,
												pEvent->targetObject,
												(!(pEvent->how & AEVT_ATMARK)) ? "TAIL" : "MARK",
												nodeID);
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		PrintKilledEvent
//
// DESCRIPTION:	Prints out trace information.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
VOID		PrintKilledEvent(
	pAEvtInfo		pEvent,
	long				nodeID
	)
	{
	InitFromIni();

	if (g_wTraceOpt & DBG_TRACEEVENTS)
		ADbgPrint(ADBG_INFO, 0, "Killed event '%c%c%c%c': %ld -> %ld, %s, ID = %li",
												(char) (pEvent->what >> 24),
												(char) (pEvent->what >> 16),
												(char) (pEvent->what >>  8),
												(char) (pEvent->what      ),
												pEvent->sourceObject,
												pEvent->targetObject,
												(!(pEvent->how & AEVT_ATMARK)) ? "TAIL" : "MARK",
												nodeID);
	}
#endif


////////////////////////////////////////////////////////////////
//
// FUNCTION:		PrintMsgInfo
//
// DESCRIPTION:	Prints out Windows MSG information.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG

void		InitWMsgMap(void);
LPSTR		MapMsg(WORD);

VOID		PrintMsgInfo(
	LPMSG			pMsg
	)
	{
	InitFromIni();

	if (g_wTraceOpt & DBG_TRACEWMSGS)
		{
		InitWMsgMap();
		ADbgPrint(ADBG_INFO, 0, "Windows Msg:  0x%04x  %s", pMsg->message,
									MapMsg(pMsg->message));
		}
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		MapCmp
//
// DESCRIPTION:	Compares two elements in the map.  Returns -1, 0, +1
//							according to the comparison results.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
int	_CType _export	MapCmp(
	const void*		key,
	const void*		elem
	)
	{
	WMSGMAP*			pKey		= (WMSGMAP*) key;
	WMSGMAP*			pElem		= (WMSGMAP*) elem;
	int						nRet;

	if (pKey->wID < pElem->wID)
		nRet = -1;
	else if (pKey->wID > pElem->wID)
		nRet = 1;
	else
		nRet = 0;

	return nRet;
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		AddWindowsMsg
//
// DESCRIPTION:	Adds one (LPSTR, WORD) pair to the list of translatable
//							messages.  After all messages are added, this function
//							should be called with NULL as the string to sort it.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
void	AddWindowsMsg(
	LPSTR			pMsg,
	WORD			wMsg
	)
	{
	if (pMsg == NULL)			// sort
		{
		qsort(g_pWindowsMsgMap, g_nMsgMapCount, sizeof(WMSGMAP), MapCmp);
		}
	else
		{
		WMSGMAP*		pNewMap;
		WORD				nNewSize;

		if (g_nMsgMapCount + 1 >= g_nMsgMapSize)
			{
			nNewSize = g_nMsgMapSize + 50;
			pNewMap  = new WMSGMAP[nNewSize];

			if (pNewMap)
				{
				if (g_pWindowsMsgMap)
					{
					memcpy(pNewMap, g_pWindowsMsgMap, g_nMsgMapCount * sizeof(WMSGMAP));
					delete g_pWindowsMsgMap;
					}
				g_nMsgMapSize = nNewSize;
				g_pWindowsMsgMap = pNewMap;
				}
			}

		if (g_nMsgMapCount + 1 < g_nMsgMapSize)
			{
			g_pWindowsMsgMap[g_nMsgMapCount].pszName = pMsg;
			g_pWindowsMsgMap[g_nMsgMapCount].wID     = wMsg;
			g_nMsgMapCount++;
			}
		}
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		MapMsg
//
// DESCRIPTION:	Translates from a UINT wMsg to an LPSTR that is the
//							WM_* string.  "(unknown)" is returned if not found.
//
////////////////////////////////////////////////////////////////

#ifdef ADBG_DEBUG
LPSTR		MapMsg(
	WORD		wMsg
	)
	{
	static WMSGMAP	key;
	WMSGMAP*				pElem;

	key.pszName = "(unknown)";
	key.wID     = wMsg;

	pElem = (WMSGMAP*) bsearch(&key, g_pWindowsMsgMap, g_nMsgMapCount,
															sizeof(WMSGMAP), MapCmp);

	if (pElem)
		key.pszName = pElem->pszName;

	return key.pszName;
	}
#endif

////////////////////////////////////////////////////////////////
//
// FUNCTION:		InitWMsgMap
//
// DESCRIPTION:	Adds all WM_* messages to the translation table for
//							debug printing capability.
//
////////////////////////////////////////////////////////////////

#define ADDWMSG(m,n)		AddWindowsMsg(#m, n)

#ifdef ADBG_DEBUG
void	InitWMsgMap(
	void
	)
	{
	if ((g_wTraceOpt & DBG_TRACEWMSGS) && !g_fMapInitialized)
		{
		ADDWMSG(WM_ACTIVATE,					0x0006);
		ADDWMSG(WM_ACTIVATEAPP,				0x001C);
		ADDWMSG(WM_ASKCBFORMATNAME,		0x030C);
		ADDWMSG(WM_CANCELMODE,				0x001F);
		ADDWMSG(WM_CHANGECBCHAIN,			0x030D);
		ADDWMSG(WM_CHAR,							0x0102);
		ADDWMSG(WM_CHARTOITEM,				0x002F);
		ADDWMSG(WM_CHILDACTIVATE,			0x0022);
		ADDWMSG(WM_CLEAR,							0x0303);
		ADDWMSG(WM_CLOSE,							0x0010);
		ADDWMSG(WM_COALESCE_FIRST,		0x0390);
		ADDWMSG(WM_COALESCE_LAST,			0x039F);
		ADDWMSG(WM_COMMAND,						0x0111);
		ADDWMSG(WM_COMMNOTIFY,				0x0044);
		ADDWMSG(WM_COMPACTING,				0x0041);
		ADDWMSG(WM_COMPAREITEM,				0x0039);
		ADDWMSG(WM_COPY,							0x0301);
		ADDWMSG(WM_CREATE,						0x0001);
		ADDWMSG(WM_CTLCOLOR,					0x0019);
		ADDWMSG(WM_CUT,								0x0300);
		ADDWMSG(WM_DEADCHAR,					0x0103);
		ADDWMSG(WM_DELETEITEM,				0x002D);
		ADDWMSG(WM_DESTROY,						0x0002);
		ADDWMSG(WM_DESTROYCLIPBOARD,	0x0307);
		ADDWMSG(WM_DEVMODECHANGE,			0x001B);
		ADDWMSG(WM_DRAWCLIPBOARD,			0x0308);
		ADDWMSG(WM_DRAWITEM,					0x002B);
		ADDWMSG(WM_DROPFILES,					0x0233);
		ADDWMSG(WM_ENABLE,						0x000A);
		ADDWMSG(WM_ENDSESSION,				0x0016);
		ADDWMSG(WM_ENTERIDLE,					0x0121);
		ADDWMSG(WM_ERASEBKGND,				0x0014);
		ADDWMSG(WM_FONTCHANGE,				0x001D);
		ADDWMSG(WM_GETDLGCODE,				0x0087);
		ADDWMSG(WM_GETFONT,						0x0031);
		ADDWMSG(WM_GETMINMAXINFO,			0x0024);
		ADDWMSG(WM_GETTEXT,						0x000D);
		ADDWMSG(WM_GETTEXTLENGTH,			0x000E);
		ADDWMSG(WM_HSCROLL,						0x0114);
		ADDWMSG(WM_HSCROLLCLIPBOARD,	0x030E);
		ADDWMSG(WM_ICONERASEBKGND,		0x0027);
		ADDWMSG(WM_INITDIALOG,				0x0110);
		ADDWMSG(WM_INITMENU,					0x0116);
		ADDWMSG(WM_INITMENUPOPUP,			0x0117);
		ADDWMSG(WM_KEYDOWN,						0x0100);
		ADDWMSG(WM_KEYFIRST,					0x0100);
		ADDWMSG(WM_KEYLAST,						0x0108);
		ADDWMSG(WM_KEYUP,							0x0101);
		ADDWMSG(WM_KILLFOCUS,					0x0008);
		ADDWMSG(WM_LBUTTONDBLCLK,			0x0203);
		ADDWMSG(WM_LBUTTONDOWN,				0x0201);
		ADDWMSG(WM_LBUTTONUP,					0x0202);
		ADDWMSG(WM_MBUTTONDBLCLK,			0x0209);
		ADDWMSG(WM_MBUTTONDOWN,				0x0207);
		ADDWMSG(WM_MBUTTONUP,					0x0208);
		ADDWMSG(WM_MDIACTIVATE,				0x0222);
		ADDWMSG(WM_MDICASCADE,				0x0227);
		ADDWMSG(WM_MDICREATE,					0x0220);
		ADDWMSG(WM_MDIDESTROY,				0x0221);
		ADDWMSG(WM_MDIGETACTIVE,			0x0229);
		ADDWMSG(WM_MDIICONARRANGE,		0x0228);
		ADDWMSG(WM_MDIMAXIMIZE,				0x0225);
		ADDWMSG(WM_MDINEXT,						0x0224);
		ADDWMSG(WM_MDIRESTORE,				0x0223);
		ADDWMSG(WM_MDISETMENU,				0x0230);
		ADDWMSG(WM_MDITILE,						0x0226);
		ADDWMSG(WM_MEASUREITEM,				0x002C);
		ADDWMSG(WM_MENUCHAR,					0x0120);
		ADDWMSG(WM_MENUSELECT,				0x011F);
		ADDWMSG(WM_MOUSEACTIVATE,			0x0021);
		ADDWMSG(WM_MOUSEFIRST,				0x0200);
		ADDWMSG(WM_MOUSELAST,					0x0209);
		ADDWMSG(WM_MOUSEMOVE,					0x0200);
		ADDWMSG(WM_MOVE,							0x0003);
		ADDWMSG(WM_NCACTIVATE,				0x0086);
		ADDWMSG(WM_NCCALCSIZE,				0x0083);
		ADDWMSG(WM_NCCREATE,					0x0081);
		ADDWMSG(WM_NCDESTROY,					0x0082);
		ADDWMSG(WM_NCHITTEST,					0x0084);
		ADDWMSG(WM_NCLBUTTONDBLCLK,		0x00A3);
		ADDWMSG(WM_NCLBUTTONDOWN,			0x00A1);
		ADDWMSG(WM_NCLBUTTONUP,				0x00A2);
		ADDWMSG(WM_NCMBUTTONDBLCLK,		0x00A9);
		ADDWMSG(WM_NCMBUTTONDOWN,			0x00A7);
		ADDWMSG(WM_NCMBUTTONUP,				0x00A8);
		ADDWMSG(WM_NCMOUSEMOVE,				0x00A0);
		ADDWMSG(WM_NCPAINT,						0x0085);
		ADDWMSG(WM_NCRBUTTONDBLCLK,		0x00A6);
		ADDWMSG(WM_NCRBUTTONDOWN,			0x00A4);
		ADDWMSG(WM_NCRBUTTONUP,				0x00A5);
		ADDWMSG(WM_NEXTDLGCTL,				0x0028);
		ADDWMSG(WM_NULL,							0x0000);
		ADDWMSG(WM_PAINT,							0x000F);
		ADDWMSG(WM_PAINTCLIPBOARD,		0x0309);
		ADDWMSG(WM_PALETTECHANGED,		0x0311);
		ADDWMSG(WM_PALETTEISCHANGING,	0x0310);
		ADDWMSG(WM_PARENTNOTIFY,			0x0210);
		ADDWMSG(WM_PASTE,							0x0302);
		ADDWMSG(WM_PENWINFIRST,				0x0380);
		ADDWMSG(WM_PENWINLAST,				0x038F);
		ADDWMSG(WM_POWER,							0x0048);
		ADDWMSG(WM_QUERYDRAGICON,			0x0037);
		ADDWMSG(WM_QUERYENDSESSION,		0x0011);
		ADDWMSG(WM_QUERYNEWPALETTE,		0x030F);
		ADDWMSG(WM_QUERYOPEN,					0x0013);
		ADDWMSG(WM_QUEUESYNC,					0x0023);
		ADDWMSG(WM_QUIT,							0x0012);
		ADDWMSG(WM_RBUTTONDBLCLK,			0x0206);
		ADDWMSG(WM_RBUTTONDOWN,				0x0204);
		ADDWMSG(WM_RBUTTONUP,					0x0205);
		ADDWMSG(WM_RENDERALLFORMATS,	0x0306);
		ADDWMSG(WM_RENDERFORMAT,			0x0305);
		ADDWMSG(WM_SETCURSOR,					0x0020);
		ADDWMSG(WM_SETFOCUS,					0x0007);
		ADDWMSG(WM_SETFONT,						0x0030);
		ADDWMSG(WM_SETREDRAW,					0x000B);
		ADDWMSG(WM_SETTEXT,						0x000C);
		ADDWMSG(WM_SHOWWINDOW,				0x0018);
		ADDWMSG(WM_SIZE,							0x0005);
		ADDWMSG(WM_SIZECLIPBOARD,			0x030B);
		ADDWMSG(WM_SPOOLERSTATUS,			0x002A);
		ADDWMSG(WM_SYSCHAR,						0x0106);
		ADDWMSG(WM_SYSCOLORCHANGE,		0x0015);
		ADDWMSG(WM_SYSCOMMAND,				0x0112);
		ADDWMSG(WM_SYSDEADCHAR,				0x0107);
		ADDWMSG(WM_SYSKEYDOWN,				0x0104);
		ADDWMSG(WM_SYSKEYUP,					0x0105);
		ADDWMSG(WM_SYSTEMERROR,				0x0017);
		ADDWMSG(WM_TIMECHANGE,				0x001E);
		ADDWMSG(WM_TIMER,							0x0113);
		ADDWMSG(WM_UNDO,							0x0304);
		ADDWMSG(WM_USER,							0x0400);
		ADDWMSG(WM_VKEYTOITEM,				0x002E);
		ADDWMSG(WM_VSCROLL,						0x0115);
		ADDWMSG(WM_VSCROLLCLIPBOARD,	0x030A);
		ADDWMSG(WM_WINDOWPOSCHANGED,	0x0047);
		ADDWMSG(WM_WINDOWPOSCHANGING,	0x0046);
		ADDWMSG(WM_WININICHANGE,			0x001A);

		AddWindowsMsg(NULL, 0);				// tells it to sort

		g_fMapInitialized = TRUE;
		}
	}
#endif // ADBG_DEBUG

