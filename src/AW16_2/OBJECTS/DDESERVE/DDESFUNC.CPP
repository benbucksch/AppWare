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
// FILE:				DDESFUNC.cpp	
//
// AUTHOR:      Shaili Jain	
//
// DESCRIPTION:	This module contains appbldr-level functions
//
// CHANGES:
//
// 
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_wnd.h>

#include "ddemlsv.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
extern LPLONG GetRunTimePtr(OBJECTID theObject, BOOL bIsStartUp );
extern unsigned int fmtLink;

//----------------------------------------------------------------------
//  gen_Copy -- copy Link Information to clipboard
//----------------------------------------------------------------------
extern "C" void ALMCALLBACK ddes_CopyLink(
	pAEvtInfo /*-- theEvent --*/,
  pAObjMessage      theSystem)
	{
	theSystem->message1 = 2;
	OpWndItemD *VItem = OWndGetActiveItem();
	if (VItem == NULL)
    {
		return;
		}
	OBJECTID 		activeOID = VItem->Object();
	long 				curindex = -1;
	OBJECTID 		ddesOID = 0;

	while ( (ddesOID = AObjGetNextObject(OTYPE_DDES, &curindex)) != 0L)
		{
		MEMBLOCKID topicID = AMemAllocate(0);
		MEMBLOCKID itemID = AMemAllocate(0);
		long size = 0;

		int nService, nTopic, nItem;
		LPSTR lpTopic, lpItem;
		MEMBLOCKID clipDataID = AMemAllocate(0);
		LPSTR lpClip;

		DDES *ddes = (DDES *)GetRunTimePtr(ddesOID, FALSE);
		if (ddes->GetConvInfoFromOID(topicID, itemID, activeOID) == FALSE)
			{
			AMemFree(topicID);
			AMemFree(itemID);
			continue;
      }

		lpTopic  = (LPSTR) AMemLock(topicID);
		lpItem 	 = (LPSTR) AMemLock(itemID);

		size = AMemGetSize(topicID) + AMemGetSize(itemID) + strlen(ddes->GetddesService().Service);
		AMemSetSize(clipDataID, size + 3) ; // service\0topic\0item\0\0
		lpClip = (LPSTR)AMemLock(clipDataID);
		_fmemset(lpClip, 0, size + 3);

		strcpy(lpClip, ddes->GetddesService().Service);
		nService = strlen(lpClip);
		lpClip[nService] = '\t';

		
		strcat(lpClip, lpTopic);
		nTopic = strlen(lpClip);
		lpClip[nTopic] = '\t';

		strcat(lpClip, lpItem);
		nItem = strlen(lpClip);
		lpClip[nItem + 1] = '\0';

		if (OpenClipboard(VItem->m_hwndParent))
			{
			HANDLE hData = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, nItem + 2);
			LPSTR	 lpData =(LPSTR)GlobalLock(hData);
			strcpy(lpData, lpClip);
			lpData[nService] = '\0';
			lpData[nTopic] = '\0';

			GlobalUnlock(hData);

			SetClipboardData((unsigned int)fmtLink, hData);
			CloseClipboard();
			theSystem->message1 = 1;
			}

		AMemUnlock(topicID);
		AMemUnlock(itemID);
		AMemFree(topicID);
		AMemFree(itemID);

		break;
		}

	// AEvtPostStandard((OBJECTID)0, OGEN_EVT_CUT, 0, AEVT_ATMARK);
	}


