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
// FILE:					SROLSTRM.CPP
//
// AUTHOR					Scott McCarty
//
// DESCRIPTION:		Manages interaction with OLE streams.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <windows.h>
#include "ushort.h"
#include <ole.h>
#include <a_alm.h>
#include <intdebug.h>

#include "srole.h"
#include "ole_obj.h"
#include "srolstrm.h"
#include "sroleobj.h"

////////////////////////////////////////////////////////////////
//
// Local statics
//
////////////////////////////////////////////////////////////////

DWORD ALMCALLBACK _export	SROleStreamGet(
	LPOLESTREAM							pStream,
	void FAR*								pBuf,
	DWORD										dwSize
	);

DWORD ALMCALLBACK _export	SROleStreamPut(
	LPOLESTREAM							pStream,
	OLE_CONST void FAR*			pBuf,
	DWORD										dwSize
	);

static OLESTREAMVTBL		vtblStream					=		{
																									SROleStreamGet,
																									SROleStreamPut
																								};

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SROleStreamPut
//
// DESC:			
//
////////////////////////////////////////////////////////////////

DWORD ALMCALLBACK _export	SROleStreamGet(
	LPOLESTREAM							pStreamAux,
	void FAR*								pBuf,
	DWORD										dwSize
	)
	{
	SROleStreamAux*					pStream		= (SROleStreamAux*) pStreamAux;

	return pStream->m_Owner->Get(pBuf, dwSize);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SROleStreamPut
//
// DESC:			
//
////////////////////////////////////////////////////////////////

DWORD ALMCALLBACK _export	SROleStreamPut(
	LPOLESTREAM							pStreamAux,
	OLE_CONST void FAR*			pBuf,
	DWORD										dwSize
	)
	{
	SROleStreamAux*					pStream		= (SROleStreamAux*) pStreamAux;

	return pStream->m_Owner->Put(pBuf, dwSize);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleStream constructor
//
// DESC:			Initializes a new SROleStream
//
////////////////////////////////////////////////////////////////

SROleStream::SROleStream(
	SROleClientObj*		pObj,
	BOOL							fRead
	) :
		m_Object(pObj),
		m_Reading(fRead),
		m_Data(NULL)
	{
	I_ASSERT(pObj != NULL);

	m_Stream.m_StreamVtbl = &vtblStream;
	m_Stream.m_Owner      = this;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleStream destructor
//
// DESC:			Cleans up an SROleStream object.
//
////////////////////////////////////////////////////////////////

SROleStream::~SROleStream(
	VOID
	)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleStream::Init
//
// DESC:			Verifies internal state for stream I/O.  If we are reading,
//						then # of data blocks is checked; if we are writing, we resize
//						the data block.
//
////////////////////////////////////////////////////////////////

BOOL		SROleStream::Init(
	DWORD			dwMinSize
	)
	{
	BOOL					fOk				= FALSE;
	SROLEObject*	psrObject;

	psrObject = (SROLEObject*) AObjGetRuntimeInfo(m_Object->ObjectId());

	I_ASSERT(psrObject != NULL);

	if (m_Reading)
		fOk = ((m_Data = psrObject->LockDataBlock()) != NULL);

	else												// writing
		fOk = ((m_Data = psrObject->CreateDataBlock(dwMinSize)) != NULL);

	return fOk;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleStream::Term
//
// DESC:			Cleans up after an Init() operation has been done.
//
////////////////////////////////////////////////////////////////

VOID		SROleStream::Term(
	VOID
	)
	{
	SROLEObject*	psrObject;

	psrObject = (SROLEObject*) AObjGetRuntimeInfo(m_Object->ObjectId());

	I_ASSERT(psrObject != NULL);

	psrObject->UnlockDataBlock();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleStream::Get
//
// DESC:			Reads data from the AppWare memory blocks and copies
//						them to the supplied buffer.  The number of bytes actually
//						copied is returned.
//
////////////////////////////////////////////////////////////////

DWORD		SROleStream::Get(
	VOID*			pBuf,
	DWORD			dwSize
	)
	{
	I_ASSERT(pBuf != NULL && dwSize > 0);

#if WINVER >= 0x030a
	hmemcpy(pBuf, m_Data, dwSize);

	m_Data = (VOID*) (((char _huge*) m_Data) + dwSize);

#else

#error

#endif		// WINVER >= 0x030a

	return dwSize;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleStream::Put
//
// DESC:			Writes data to the AppWare memory blocks (copies
//						from the supplied buffer).  The number of bytes actually
//						copied is returned.
//
////////////////////////////////////////////////////////////////

DWORD		SROleStream::Put(
	const VOID*		pBuf,
	DWORD					dwSize
	)
	{
	I_ASSERT(pBuf != NULL && dwSize > 0);

#if WINVER >= 0x030a
	hmemcpy(m_Data, pBuf, dwSize);

	m_Data = (VOID*) (((char _huge*) m_Data) + dwSize);

#else

#error

#endif		// WINVER >= 0x030a

	return dwSize;
	}
