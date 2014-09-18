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
// FILE:					SROLEFUN.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Utility functions used by the AppWare OLE Object module.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SROLEFUN_H
#define SROLEFUN_H

////////////////////////////////////////////////////////////////
//
// Function Prototypes:
//
////////////////////////////////////////////////////////////////

SROleClientDoc*	SRCurrentDoc(VOID);

VOID						SRGetText(OBJECTID, LPSTR, int);	// fills in buffer
LONG						SRGetNumber(OBJECTID);						// returns value as a LONG

#endif	// !SROLEFUN_H
