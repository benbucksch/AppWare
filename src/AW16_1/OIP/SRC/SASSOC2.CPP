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

#include <memory.h>
#include <windows.h>

#include "sassoc2.h"

#define ASC_GROWSIZE			20			// size to increment arrays by

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc2 constructor
//
// DESCR:			Initializes a new two-key association object.
//
////////////////////////////////////////////////////////////////

SAssoc2::SAssoc2(
	VOID
	) :
		m_lSize(0),
		m_lCount(0),
		m_pId1(NULL),
		m_pId2(NULL),
		m_ppValue(NULL)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc2 destructor
//
// DESCR:			Frees memory used by the association object.
//
////////////////////////////////////////////////////////////////

SAssoc2::~SAssoc2(
	VOID
	)
	{
	RemoveAll();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc::Find
//
// DESCR:			Searches the association for the two-key entry.  The
//						value associated with the keys is returned, and the index
//						they are found at is modified.
//
////////////////////////////////////////////////////////////////

VOID*		SAssoc2::Find(
	LONG			id1,
	LONG			id2,
	LONG&			lIndex
	)
	{
	VOID*			pValue		= NULL;

	for (lIndex = 0; lIndex < m_lCount; lIndex++)
		{
		if (m_pId1[lIndex]   == id1 &&
				m_pId2[lIndex]   == id2)
			{
			pValue = m_ppValue[lIndex];
			break;
			}
		}

	return pValue;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc::Find
//
// DESCR:			Searches the association for the one-key entry.  The
//						value associated with the key is returned, and the index
//						they are found at is modified.
//
////////////////////////////////////////////////////////////////

VOID*		SAssoc2::Find(
	LONG			id1,
	LONG&			lIndex
	)
	{
	VOID*			pValue		= NULL;

	// DON'T set lIndex to 0!
	for (; lIndex < m_lCount; lIndex++)
		{
		if (m_pId1[lIndex] == id1)
			{
			pValue = m_ppValue[lIndex];
			break;
			}
		}

	return pValue;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc2::Add
//
// DESCR:			Adds a new association to the two-key assoc.  If the
//						two-id key exists, FALSE is returned.  NULL cannot be
//						passed as the value to associate.
//
////////////////////////////////////////////////////////////////

BOOL	SAssoc2::Add(
	LONG			id1,
	LONG			id2,
	VOID*			pValue
	)
	{
	BOOL			fAdded			= FALSE;
	LONG*			pId1New;
	LONG*			pId2New;
	VOID**		ppValue;
	LONG			lNewSize;

	if (pValue != NULL &&						// don't add NULLs (because of Get() return value
			Get(id1, id2) == NULL)			// not already there
		{
		if (m_lCount >= m_lSize)
			{
			lNewSize = ((m_lSize / ASC_GROWSIZE) + 1) * ASC_GROWSIZE;

			pId1New = new LONG[lNewSize];
			pId2New = new LONG[lNewSize];
			ppValue = new VOID*[lNewSize];

			if (pId1New != NULL &&
					pId2New != NULL &&
					ppValue != NULL)
				{
				if (m_pId1 != NULL)				// assume that we have all from this test
					{
					memcpy(pId1New, m_pId1,    m_lSize * sizeof(LONG));
					memcpy(pId2New, m_pId2,    m_lSize * sizeof(LONG));
					memcpy(ppValue, m_ppValue, m_lSize * sizeof(VOID*));

					delete m_pId1;
					delete m_pId2;
					delete m_ppValue;
					}

				m_pId1   = pId1New;
				m_pId2   = pId2New;
				m_ppValue = ppValue;
				m_lSize  = lNewSize;
				}
			}

		if (m_lCount < m_lSize)
			{
			m_pId1[m_lCount]   = id1;
			m_pId2[m_lCount]   = id2;
			m_ppValue[m_lCount] = pValue;
			m_lCount++;
			fAdded = TRUE;
			}
		}

	return fAdded;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc2::Get
//
// DESCR:			Looks up the two-id key and returns the value that was
//						previously associated with it.
//
////////////////////////////////////////////////////////////////

VOID*		SAssoc2::Get(
	LONG			id1,
	LONG			id2
	)
	{
	LONG			lIndex;

	return Find(id1, id2, lIndex);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc2::Get
//
// DESCR:			Returns the next entry that has the given first key.
//						To get the first entry, *pIndex should be 0.
//
////////////////////////////////////////////////////////////////

VOID*		SAssoc2::Get(
	LONG			id1,
	LONG*			pId2,
	LONG*			pIndex
	)
	{
	LONG			lIndex		= *pIndex;
	VOID*			pValue;

	pValue	= Find(id1, lIndex);

	if (pId2 != NULL)
		*pId2 = m_pId2[lIndex];

	*pIndex = lIndex + 1;

	return pValue;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc2::Remove
//
// DESCR:			Removes the entry with the two-id key.  TRUE is returned
//						for successful deletion.  The value is NOT deleted, so Get
//						should be called before this to free up memory used by the
//						value.
//
////////////////////////////////////////////////////////////////

BOOL	SAssoc2::Remove(
	LONG			id1,
	LONG			id2
	)
	{
	LONG			lIndex;
	VOID*			pValue;

	pValue = Find(id1, id2, lIndex);

	if (pValue != NULL)
		{
		m_pId1[lIndex]    = m_pId1[m_lCount - 1];
		m_pId2[lIndex]    = m_pId2[m_lCount - 1];
		m_ppValue[lIndex] = m_ppValue[m_lCount - 1];
		m_lCount--;
		}

	return (pValue != NULL);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SAssoc2::RemoveAll
//
// DESCR:			Removes all entries whose first key is the given one.  The
//						values associated with the keys are not deleted.  TRUE is
//						returned if at least 1 entry is deleted.
//
////////////////////////////////////////////////////////////////

BOOL	SAssoc2::RemoveAll(
	LONG			id1
	)
	{
	BOOL			fDeleted	= FALSE;
	LONG			lIndex		= 0;

	for (;lIndex < m_lCount;)
		{
		if (Find(id1, lIndex) != NULL)
			{
			m_pId1[lIndex]   = m_pId1[m_lCount - 1];
			m_pId2[lIndex]   = m_pId2[m_lCount - 1];
			m_ppValue[lIndex] = m_ppValue[m_lCount - 1];
			m_lCount--;

			fDeleted = TRUE;
			}
		else
			break;
		}

	return fDeleted;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SAssoc2::RemoveAll
//
// DESCR:		Removes all entries in the association.  The memory pointed to
//					by the values of the associations is NOT freed.
//
////////////////////////////////////////////////////////////////

BOOL	SAssoc2::RemoveAll(
	VOID
	)
	{
	m_lCount = 0;

	delete m_pId1;
	delete m_pId2;
	delete m_ppValue;

	return TRUE;
	}

