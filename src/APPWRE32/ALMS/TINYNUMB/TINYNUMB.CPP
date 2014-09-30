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
// FILE:          "tinynumb.cpp"
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
#include <o_number.h>
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
long ALMCALLBACK	TinyNumbAdmin
	(
	AOBJECTID			oiNumb,
	pAObjMessage	pMsg
	)
	{
	LONG			lRet		= A_NOTHANDLED;

	switch (pMsg->message1)
		{
		case AOBJ_CREATED:
			AObjSetDataCount(oiNumb, 1);
			AObjResizeData(oiNumb, 0, sizeof(long));
			lRet = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			{
			//message3 is source
			LONG*		pSrc;
			LONG*		pDst;

			pSrc = (LONG*) AObjLockData((AOBJECTID) pMsg->message3, 0);
			pDst = (LONG*) AObjLockData(oiNumb, 0);

			*pDst = *pSrc;

			AObjUnlockData(oiNumb, 0);
			AObjUnlockData((AOBJECTID) pMsg->message3, 0);
			break;
			}

		case AOBJ_READ:
			lRet = A_OK;
			break;

		case AOBJ_ASLEEP:
		case AOBJ_AWAKENED:
		case AOBJ_BUILDSIGNALS:
		case AOBJ_CHECKOBJECT:
		case AOBJ_DESTROYED:
		case AOBJ_EDITOBJECT:
		case AOBJ_EXPORTOBJECT:
		case AOBJ_IMPORTOBJECT:
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
long ALMAPI		ONmbrGetInteger(
	AOBJECTID			o
	)
	{
	LONG*					pNumb = (LONG*) AObjLockData(o, 0);
	LONG					nValue;

	nValue = *pNumb;

	AObjUnlockData(o, 0);

	return nValue;
	}

////////////////////////////////////////////////////////////////

extern "C"
long ALMAPI		ONmbrSetInteger(
	AOBJECTID			o,
	LONG					l,
	BOOL					b
	)
	{
	LONG*					pNumb = (LONG*) AObjLockData(o, 0);

	I_UNUSED(b);

	*pNumb = l;

	AObjUnlockData(o, 0);

	return l;
	}

////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK	TinyNumbObjToText(
	pAEvtInfo					pEvent,
	pAObjMessage			pSystem
	)
	{
	AMEMBLOCKID				mbNumb	= (AMEMBLOCKID) pEvent->details;
	AOBJECTID					oiNumb	= pEvent->targetObject;
	LONG*							pValue;
	char*							pDest;

	I_UNUSED(pSystem);

	AMemSetSize(mbNumb, 20);
	pDest  = (char*) AMemLock(mbNumb);
	pValue = (LONG*) AObjLockData(oiNumb, 0);

	wsprintf(pDest, "%ld", *pValue);

	AObjUnlockData(oiNumb, 0);
	AMemUnlock(mbNumb);
	}

////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK	TinyNumbTextToObj(
	pAEvtInfo					pEvent,
	pAObjMessage			pSystem
	)
	{
	AMEMBLOCKID				mbNumb	= (AMEMBLOCKID) pEvent->details;
	AOBJECTID					oiNumb	= pEvent->targetObject;
	LONG*							pValue;
	char*							pSrc;
	LONG							nValue;
	LONG							nMult;

	I_UNUSED(pSystem);

	pSrc  = (char*) AMemLock(mbNumb);
	pValue = (LONG*) AObjLockData(oiNumb, 0);

	if (*pSrc == '-')
		{
		nMult = -1;
		pSrc++;
		}
	else if (*pSrc == '+')
		{
		nMult = +1;
		pSrc++;
		}
	else
		nMult = 1;

	nValue = 0;


	while (*pSrc != '\0')
		{
		nValue = 10 * nValue + (*pSrc - '0');
		pSrc++;
		}

	nValue *= nMult;

	*pValue = nValue;

	AObjUnlockData(oiNumb, 0);
	AMemUnlock(mbNumb);
	}
