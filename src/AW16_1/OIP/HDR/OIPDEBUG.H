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
// FILE:					OIPDEBUG_H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Contains internal-only interface into debug code.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef OIPDEBUG_H
#define OIPDEBUG_H

#ifdef ADBG_DEBUG

VOID						PrintObjDataInfo(OBJECTID theObject, LONG dataIndex);
VOID						DeleteObjDataInfo(OBJECTID theObject);

VOID						PrintEventInfo(pAEvtInfo, long);		// AppWare event being processed
VOID						PrintEventPush(pAEvtInfo, long);		// AppWare event being pushed
VOID						PrintMsgInfo(LPMSG);					// Window's MSG being processed
VOID						PrintKilledEvent(pAEvtInfo, long); //AppWare being killed by KillDuplicates, etc
#endif

#endif	// !OIPDEBUG_H
