/*//////////////////////////////////////////////////////////////
//
//					Copyright 1994, Novell, Inc.  All rights reserved
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
// FILE:					TEST.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Prints out Admin messages as they are received.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include <a_alm.h>
#include <o_number.h>

extern int	_WinAllocFlag;

////////////////////////////////////////////////////////////////////////////

int CALLBACK /*!!PORT!! was _far_ _pascal_*/	LibMain/*!!PORT!! Replace with DllEntryPoint *//*!!PORT!! Replace with DllEntryPoint */(
	HANDLE		hInstance,
	WORD			wDataSeg,
	WORD			wHeapSize,
	LPSTR			lpszCmdLine
	)
	{
	(void) hInstance;
	(void) wDataSeg;
	(void) lpszCmdLine;

  _WinAllocFlag = GMEM_SHARE;

  if (wHeapSize > 0)
		UnlockData(0);

  return 1;
	}

////////////////////////////////////////////////////////////////////////////

extern "C"
LONG ALMCALLBACK	TestAdmn(
	AOBJECTID			theObject,
	pAObjMessage	theSystem
	)
	{
	LONG			lRet				= A_OK;
	char			szMsg[128];
	char			szOut[128];

	switch (theSystem->message1)
		{
		case AOBJ_CREATED:
			lstrcpy(szMsg, "AOBJ_CREATED(%ld) called\r\n");

			if (AObjSetDataCount(theObject, 1))
				AObjResizeData(theObject, 1, 10);
			break;

		case AOBJ_AWAKENED:
			lstrcpy(szMsg, "AOBJ_AWAKENED(%ld) called\r\n");
			lRet = A_CONTINUE;
			break;

		case AOBJ_ASLEEP:
			lstrcpy(szMsg, "AOBJ_ASLEEP(%ld) called\r\n");
			lRet = A_CONTINUE;
			break;

		case AOBJ_DESTROYED:
			lstrcpy(szMsg, "AOBJ_DESTROYED(%ld) called\r\n");
			lRet = A_CONTINUE;
			break;

		case AOBJ_READ:
			lstrcpy(szMsg, "AOBJ_READ(%ld) called\r\n");
			break;

		case AOBJ_POSTWRITE:
			lstrcpy(szMsg, "AOBJ_POSTWRITE(%ld) called\r\n");
			lRet = A_CONTINUE;
			break;

		case AOBJ_PREWRITE:
			lstrcpy(szMsg, "AOBJ_PREWRITE(%ld) called\r\n");
			lRet = A_CONTINUE;
			break;

		case AOBJ_QUERYPROTOCOL:
			lstrcpy(szMsg, "AOBJ_QUERYPROTOCOL(%ld) called\r\n");
			break;

		case AOBJ_ASSIGNOBJECT:
			lstrcpy(szMsg, "AOBJ_ASSIGNOBJECT(%ld) called\r\n");
			break;

		case AOBJ_CHECKOBJECT:
			lstrcpy(szMsg, "AOBJ_CHECKOBJECT(%ld) called\r\n");
			break;

		case AOBJ_EXPORTOBJECT:
			lstrcpy(szMsg, "AOBJ_EXPORTOBJECT(%ld) called\r\n");
			break;

		case AOBJ_IMPORTOBJECT:
			lstrcpy(szMsg, "AOBJ_IMPORTOBJECT(%ld) called\r\n");
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			lstrcpy(szMsg, "AOBJ_VALIDATEOBJECTIDS(%ld) called\r\n");
			break;

		case AOBJ_BUILDSIGNALS:
			lstrcpy(szMsg, "AOBJ_BUILDSIGNALS(%ld) called\r\n");
			break;

		case AOBJ_EDITOBJECT:
			lstrcpy(szMsg, "AOBJ_EDITOBJECT(%ld) called\r\n");
			break;

		case AOBJ_REPORTREQUIREDFILES:
			lstrcpy(szMsg, "AOBJ_REPORTREQUIREDFILES(%ld) called\r\n");
			break;

		default:
			lstrcpy(szMsg, "Default(%ld) called\r\n");
			break;
		}

	wsprintf(szOut, szMsg, theObject);
	OutputDebugString(szOut);

	return lRet;
}


////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK DoNothing(
	pAEvtInfo				pEvent,
	pAObjMessage		pSystem
	)
	{
	pSystem->message1 = 1;						// done
	}

////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK MemUsage(
	pAEvtInfo				pEvent,
	pAObjMessage		pSystem
	)
	{
	AOBJECTID				oiNumber;
	LONG						nMemUsage;
	LONG						lTypeKey;
	LONG						lObjKey;
	ATYPEID					tType;
	AOBJECTID				oObj;
	LONG						nDataCount;

	lTypeKey  = -1;
	nMemUsage = 0;

	while ((tType = ATypeGetNext(&lTypeKey)) != 0)
		{
		lObjKey = -1;
		while ((oObj = AObjGetNextObject(tType, &lObjKey)) != 0)
			{
			nDataCount = AObjGetDataCount(oObj);

			while (nDataCount > 0)
				{
				nDataCount--;
				nMemUsage += AObjGetDataSize(oObj, nDataCount);
				}
			}
		}

	oiNumber = AObjCreate(OTYPE_NUMBER);
	ONmbrSetInteger(oiNumber, nMemUsage, TRUE);
	AFuncSetParameter(1, oiNumber);

	pSystem->message1 = 1;						// done
	}
