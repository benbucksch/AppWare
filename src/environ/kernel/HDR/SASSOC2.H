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
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SASSOC2_H
#define SASSOC2_H

// SAssoc2:  Provides associations based on a two-key ID.
class SAssoc2
	{
	private:
		LONG						m_lSize;									// size of arrays
		LONG						m_lCount;									// count of associations
		LONG*						m_pId1;
		LONG*						m_pId2;
		VOID**					m_ppValue;

		VOID*						Find(LONG id1, LONG id2, LONG& lIndex);
		VOID*						Find(LONG id1, LONG& lIndex);

	public:
										SAssoc2(VOID);
									 ~SAssoc2(VOID);

		BOOL						Add(LONG id1, LONG id2, VOID* value);
		VOID*						Get(LONG id1, LONG id2);
		VOID*						Get(LONG id1, LONG* pId2, LONG* pKey);

		BOOL						Remove(LONG id1, LONG id2);
		BOOL						RemoveAll(LONG id1);
		BOOL						RemoveAll(VOID);
	};

#endif	// !SASSOC2_H
