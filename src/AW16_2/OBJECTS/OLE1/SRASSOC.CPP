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
// FILE:					SRASSOC.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Maintains (long, long) associations
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <string.h>
#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>

#include "srole.h"
#include "srassoc.h"

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc constructor
//
// DESC:			Initializes a new SRAssoc instance.
//
////////////////////////////////////////////////////////////////

SRAssoc::SRAssoc(
	VOID
	) :
		m_Assoc(NULL),
		m_nCount(0),
		m_nSize(0)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc destructor
//
// DESC:			Cleans up a SRAssoc instance.
//
////////////////////////////////////////////////////////////////

SRAssoc::~SRAssoc(
	VOID
	)
	{
	if (m_Assoc)
		delete m_Assoc;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc::Find
//
// DESC:			Finds the given element.  The index returned is the pair index
//						(as compared to the LONG index).
//
////////////////////////////////////////////////////////////////

int		SRAssoc::Find(
	LONG			lValue,									// in:  value to search for
	BOOL			fLeft										// in:  search in left or right
	)
	{
	int				nIdx;
	int				nMax;
	int				nOff;

	if (fLeft)
		nOff = 0;
	else
		nOff = 1;

	nMax = m_nCount / 2;

	for (nIdx = 0; nIdx < nMax; nIdx++)
		{
		if (m_Assoc[nIdx * 2 + nOff] == lValue)
			break;
		}

	if (nIdx >= nMax)														// not found
		nIdx = -1;

	return nIdx;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc::Add
//
// DESC:			Adds a new (long,long) assocation.
//
////////////////////////////////////////////////////////////////

BOOL	SRAssoc::Add(
	LONG				lLeft,								// in:  left half of association
	LONG				lRight								// in:  right half of association
	)
	{
	BOOL				fAdded			= FALSE;
	LONG*				pNewAssoc;
	int					nNewSize;

	if (m_Assoc == NULL || (m_nCount + 2 >= m_nSize))			// no mem or not enough
		{
		nNewSize = m_nSize + 10;														// 5 more associations

		pNewAssoc = new LONG[nNewSize];

		if (pNewAssoc != NULL)
			{
			if (m_Assoc != NULL)														// copy over existing assoc
				{
				memcpy(pNewAssoc, m_Assoc, sizeof(LONG) * m_nCount);
				delete m_Assoc;
				}

			m_Assoc = pNewAssoc;
			m_nSize = nNewSize;
			}
		}

	if (m_nCount + 2 < m_nSize)
		{
		m_Assoc[m_nCount++] = lLeft;
		m_Assoc[m_nCount++] = lRight;

		fAdded = TRUE;
		}

	return fAdded;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc::RemoveL
//
// DESC:			Removes the association where the left value is the given
//						value.
//
////////////////////////////////////////////////////////////////

BOOL		SRAssoc::RemoveL(
	LONG			lLeft											// in:  value of left side
	)
	{
	BOOL			fRemoved	= FALSE;
	int				nIdx			= Find(lLeft, TRUE) * 2;

	if (nIdx >= 0)
		{
		m_nCount -= 2;								// do this before the memmove
		memmove(m_Assoc + nIdx, m_Assoc + (nIdx + 2),
						sizeof(LONG) * (m_nCount - nIdx));

		fRemoved = TRUE;
		}

	return fRemoved;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc::RemoveR
//
// DESC:			Removes the association where the right value is the given
//						value.
//
////////////////////////////////////////////////////////////////

BOOL		SRAssoc::RemoveR(
	LONG			lRight										// in:  value of right side
	)
	{
	BOOL			fRemoved	= FALSE;
	int				nIdx			= Find(lRight, FALSE) * 2;

	if (nIdx >= 0)
		{
		m_nCount -= 2;								// do this before the memmove
		memmove(m_Assoc + nIdx, m_Assoc + (nIdx + 2),
						sizeof(LONG) * (m_nCount - nIdx));

		fRemoved = TRUE;
		}

	return fRemoved;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc::FindL
//
// DESC:			Returns the right side of the association whose left side
//						is given by lLeft.  0 is returned if there is no association.
//
////////////////////////////////////////////////////////////////

LONG		SRAssoc::FindL(
	LONG			lLeft											// in:  value to find
	)
	{
	LONG			lRight		= 0;
	int				nIdx			= Find(lLeft, TRUE);

	if (nIdx >= 0)
		{
		I_ASSERT(m_Assoc != NULL);

		lRight = m_Assoc[nIdx * 2 + 1];
		}

	return lRight;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SRAssoc::FindR
//
// DESC:			Returns the left side of the association whose right side
//						is given by lRight.  0 is returned if there is no association.
//						If the right value is not unique, the first association found
//						is returned.
//
////////////////////////////////////////////////////////////////

LONG		SRAssoc::FindR(
	LONG			lRight									// in:  value to find
	)
	{
	LONG			lLeft		= 0;
	int				nIdx		= Find(lRight, FALSE);

	if (nIdx >= 0)
		{
		I_ASSERT(m_Assoc != NULL);

		lLeft = m_Assoc[nIdx * 2];
		}

	return lLeft;
	}
