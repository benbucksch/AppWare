////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "tinytext.cpp"
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>

//////////////// Globals

HINSTANCE		g_hDLL;

////////////////////////////////////////////////////////////////////////////


extern "C"
BOOL WINAPI		DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	g_hDLL = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}


////////////////////////////////////////////////////////////////////////////

extern "C"
long ALMCALLBACK	TinyTextAdmin
	(
	AOBJECTID			oiText,
	pAObjMessage	pMsg
	)
	{
	LONG			lRet		= A_NOTHANDLED;

	switch(pMsg->message1)
		{
		case AOBJ_CREATED:
			AObjSetDataCount(oiText, 1);
			lRet = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			{
			char*			pSource;
			char*			pDest;

			pSource = (char*) AObjLockData((AOBJECTID) pMsg->message3, 0);

			// source is message3
			AObjResizeData(oiText, 0, lstrlen(pSource) + 1);
			pDest = (char*) AObjLockData(oiText, 0);

			lstrcpy(pDest, pSource);

			AObjUnlockData(oiText, 0);
			AObjUnlockData((AOBJECTID) pMsg->message3, 0);
			break;
			}

		case AOBJ_ASLEEP:
		case AOBJ_AWAKENED:
		case AOBJ_BUILDSIGNALS:
		case AOBJ_CHECKOBJECT:
		case AOBJ_DESTROYED:
		case AOBJ_EDITOBJECT:
		case AOBJ_EXPORTOBJECT:
		case AOBJ_IMPORTOBJECT:
		case AOBJ_READ:
		case AOBJ_VALIDATEOBJECTIDS:
			break;
			
		default:
			lRet = A_NOTHANDLED;
			break;
		}

	return lRet;
	}

////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK	TinyTextObjToText(
	pAEvtInfo					pEvent,
	pAObjMessage			pSystem
	)
	{
	AMEMBLOCKID				mbText	= (AMEMBLOCKID) pEvent->details;
	AOBJECTID					oiText	= pEvent->targetObject;
	char*							pSource;
	char*							pDest;

	I_UNUSED(pSystem);

	pSource = (char*) AObjLockData(oiText, 0);

	if (pSource != NULL)
		{
		AMemSetSize(mbText, lstrlen(pSource) + 1);
		pDest = (char*) AMemLock(mbText);

		lstrcpy(pDest, pSource);

		AMemUnlock(mbText);
		AObjUnlockData(oiText, 0);
		}
	}

////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK	TinyTextTextToObj(
	pAEvtInfo					pEvent,
	pAObjMessage			pSystem
	)
	{
	AMEMBLOCKID				mbText	= (AMEMBLOCKID) pEvent->details;
	AOBJECTID					oiText	= pEvent->targetObject;
	char*							pSource;
	char*							pDest;

	I_UNUSED(pSystem);

	pSource = (char*) AMemLock(mbText);

	if (pSource != NULL)
		{
		AObjResizeData(oiText, 0, lstrlen(pSource) + 1);

		pDest = (char*) AObjLockData(oiText, 0);

		lstrcpy(pDest, pSource);

		AObjUnlockData(oiText, 0);
		AMemUnlock(mbText);
		}
	}
