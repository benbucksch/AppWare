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
// FILE:					SRASSOC.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Defines an association class that will associate
//								two LONG values.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SRASSOC_H
#define SRASSOC_H

////////////////////////////////////////////////////////////////
//
// CLASS:		SRAssoc
//
// DESC:		Implements an association between two 4-byte quantities.  The
//					left half is a key for the pair, but either value can be used
//					for searching.
//
////////////////////////////////////////////////////////////////

class SRAssoc
	{
	private:
		LONG*						m_Assoc;									// (long, long) assoc
		int							m_nCount;									// # of filled elements (in LONGs)
		int							m_nSize;									// size of memory (in LONGs)

		int							Find(LONG, BOOL);					// finds in left or right; returns index

	public:
										SRAssoc(VOID);
									 ~SRAssoc(VOID);

		BOOL						Add(LONG, LONG);					// (L,R) pair

		BOOL						RemoveL(LONG);						// find by L
		BOOL						RemoveR(LONG);						// find by R

		LONG						FindL(LONG l);						// return R
		LONG						FindR(LONG l);						// return L
	};

#endif	// !SRASSOC_H
