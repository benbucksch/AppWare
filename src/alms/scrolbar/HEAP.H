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
// FILE:					HEAP.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Interface to the heap manager.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef HEAP_H
#define HEAP_H

//////////////// 

#define	HP_CONSTANTSIZE			0x1					// all allocations will be same size
#define HP_DELAYEDCOALESCE	0x2					// coalesce when no memory
#define HP_USEBESTFIT				0x4					// use best fit instead of first fit
#define HP_TASKOWNED				0x8					// owned by task, rather than DLL

#define HP_ZEROINIT					0x1

LONG		HeapCreate(LONG lPageSize, WORD fFlags);
BOOL		HeapDestroy(LONG hHeap);
BOOL		HeapCheck(LONG hHeap);

VOID*		HeapAlloc(LONG hHeap, WORD wBytes, WORD fFlags);
BOOL		HeapFree(LONG hHeap, VOID*);
BOOL		HeapOwns(LONG hHeap, VOID*);


//////////////// More friendly interface that doesn't talk to individual heaps
//////////////// Defaults are:  HP_ZEROINIT only

VOID*		MemAlloc(WORD wBytes);
BOOL		MemFree(VOID*);

#endif	// !HEAP_H
