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
// FILE:					OIPUTILS.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		OIP Utility functions
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef OIPUTILS_H
#define OIPUTILS_H

LPOBJECTRECORD			GetObjectRecord(OBJECTID  theObject);
LPBASETYPE					GetObjectsTypeRecord(OBJECTID  theObject);
LPBASETYPE					GetTypeRecord(TYPEID  theTypeID);

VOID								DoObjectSendMessage(
											LONG				lMsg1,					// main message
											LONG&				lMsg2,					// extra info
											LONG&				lMsg3,					// extra info
											LONG&				lMsg4,					// extra info
											OBJECTID		oid,
											TYPEID			tid,
											BOOL 				forceToExeObj
											);


inline VOID					SendMessageToObject(
											LONG				lMsg1,					// main message
											LONG&				lMsg2,					// extra info
											LONG&				lMsg3,					// extra info
											LONG&				lMsg4,					// extra info
											OBJECTID		oid		= 0,
											TYPEID			tid		= 0
											)
	{
	//pass FALSE because we don't want to force the message to be sent
	//to the exe object.
	DoObjectSendMessage(lMsg1, lMsg2, lMsg3, lMsg4, oid, tid, FALSE);
	}

inline VOID		 			ForceMessageToAllExeObjects(
											LONG				lMsg1,					// main message
											LONG&				lMsg2,					// extra info
											LONG&				lMsg3,					// extra info
											LONG&				lMsg4					// extra info
											)
	{
	//The NULLs force the message to go to all objects and the TRUE
	//forces the message to go to exe objects.
	DoObjectSendMessage(lMsg1, lMsg2, lMsg3, lMsg4, NULL, NULL, TRUE);
	}

void 								CenterRectangle(LPRECT r1,	const LPRECT r2);

void 								CenterDialog( 
											HWND hDlg,
											HWND centeringHwnd,
											BOOL redraw);


#endif	// !OIPUTILS_H
