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
// FILE:				OLE_DATA.CPP
//
// AUTHOR:			Scott McCarty
//
// DESCRIPTION:	Manages OLE configuration data.
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
#include "srolefun.h"
#include "srolemod.h"
#include "sroleapp.h"
#include "sroledoc.h"
#include "sroleobj.h"
#include "ole_obj.h"
#include "ole_data.h"					// data block definitions

////////////////////////////////////////////////////////////////
//
// METHOD:		OLECFGDATA::SizeNeeded
//
// DESC:			Returns the minimum # of bytes that the object will
//						need to hold the given data.
//
////////////////////////////////////////////////////////////////

WORD		OLECFGDATA::SizeNeeded(
	LPSTR				pszObjectType,						// in:  ASCIIZ object type
	LPSTR				pszLinkFile,							// in:  ASCIIZ link file name
	LPSTR				pszLinkItem								// in:  ASCIIZ link item name
	)
	{
	WORD					nLen	= sizeof(OLECFGDATA) - 1;			// "- 1" because of m_cData[1]

	nLen += ((pszObjectType != NULL) ? (lstrlen(pszObjectType) + 1) : 0);
	nLen += ((pszLinkFile   != NULL) ? (lstrlen(pszLinkFile) + 1) : 0);
	nLen += ((pszLinkItem   != NULL) ? (lstrlen(pszLinkItem) + 1) : 0);

	return nLen;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLECFGDATA::operator new
//
// DESC:			Locks the objects CFGBLOCK and returns a pointer to it.
//
////////////////////////////////////////////////////////////////

VOID*		OLECFGDATA::operator new(
	size_t				s,
	SROLEObject*	pThis
	)
	{
	OBJECTID			oid		= pThis->ObjectId();
	VOID*					pMem;

	I_UNUSED(s);

	if ((AObjGetDataCount(oid) > 0) &&
			(AObjGetDataSize(oid, OLEDATA_CFGBLOCK) > 0))
		pMem = AObjLockData(oid, OLEDATA_CFGBLOCK);
	else
		pMem = NULL;

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLECFGDATA::Init
//
// DESC:			Initializes the given field to point to the string.
//
////////////////////////////////////////////////////////////////

VOID	OLECFGDATA::Init(
	LPSTR			pszString,								// in:   string to initialize it
	WORD&			rOffset,									// mod:  offset of free space
	LPSTR&		pszEnd										// mod:  pointer to free space
	)
	{
	if (pszString != NULL)
		{
		lstrcpy(pszEnd, pszString);
		rOffset  = (WORD) (pszEnd - (LPSTR) this);
		pszEnd  += (lstrlen(pszString) + 1);
		}
	else
		rOffset = 0;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLECFGDATA::Init
//
// DESC:			This initializes the object with the given values.
//
////////////////////////////////////////////////////////////////

VOID	OLECFGDATA::Init(
	OLEOPTIONS	oOptions,								// in:  options
	LPSTR				pszObjectType,					// in:  ASCIIZ object type
	LPSTR				pszLinkFile,						// in:  ASCIIZ link file name
	LPSTR				pszLinkItem						// in:  ASCIIZ link item name
	)
	{
	LPSTR				pszEnd;

	m_Version     = OLEOBJ_VERSION;
	m_fOptions    = oOptions;
	m_oMenuObject = 0;

	// We assume that we have enough memory to copy all the text items in.
	// The object should have been allocated with memory size at least what
	// was returned with OLECFGDATA::SizeNeeded()

	I_ASSERT((oOptions & ~OLEOPTIONSALL) == 0);									// no options outside allowed
	I_ASSERT((oOptions & (SOLE_EMBEDDED | SOLE_LINKED)) !=
				 (SOLE_EMBEDDED | SOLE_LINKED));										// can't have both
	I_ASSERT((oOptions & (SOLE_EMBEDDED | SOLE_LINKED)) != 0);	// need one

	// If embedded, then linkfile and linkitem should be NULL
	// If linked, then linkfile cannot be NULL
	I_ASSERT((oOptions & SOLE_EMBEDDED) ? (pszLinkFile == NULL &&
																				pszLinkItem == NULL) :
																			(pszLinkFile != NULL));

	pszEnd = (LPSTR) &m_cData;

	Init(pszObjectType, m_pszObjectType, pszEnd);
	Init(pszLinkFile,   m_pszLinkFile,   pszEnd);
	Init(pszLinkItem,   m_pszLinkItem,   pszEnd);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLECFGDATA::Destroy
//
// DESC:			Unlocks the OLEDATA_CFGBLOCK and then deletes the object.
//
////////////////////////////////////////////////////////////////

VOID	OLECFGDATA::Destroy(
	SROLEObject*		pThis
	)
	{
	AObjUnlockData(pThis->ObjectId(), OLEDATA_CFGBLOCK);

	delete this;
	}

