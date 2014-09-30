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
// FILE:          memobj.h
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef MEMOBJ_H
#define MEMOBJ_H

////////////////////////////////////////////////////////////////
//
// Data #0 contains description information about the object.  This
// consists of
//
////////////////////////////////////////////////////////////////

typedef struct MemTestStatus
	{
	long		nDataCount;					// does NOT include data 0
	long		nTargetCount;				// # of datas to allocate (does not include #0)
	long		nDataAllocSize;			// # of datas to allocate at once
	long		nAllocSize;					// # of bytes to alloc for each data
	long		nFillValue;					// value to put in each byte
	BOOL		fFillIndex;					// fill with data #
	BOOL		fFillForever;				// fill until failure
	};

#define OTYPE_MEMTEST					ATYPE_MAKEID('m', 'e', 'm', 'T')

#endif
