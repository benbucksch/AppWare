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
// FILE:					OLEMOD.CPP
//
// OWNER:					Scott McCarty
//
// DESCRIPTION:		DLL initialization and termination.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <stdlib.h>
#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>


#include "srole.h"
#include "srolemod.h"

//////////////// Globals

////////////////////////////////////////////////////////////////
//
// FUNCTION:	LibMain/*!!PORT!! Replace with DllEntryPoint */
//
// DESC:			Initializes the AppWare OLE DLL.
//
////////////////////////////////////////////////////////////////

extern "C"
int WINAPI		LibMain/*!!PORT!! Replace with DllEntryPoint */(
	HINSTANCE				hInstance,
	WORD						wDataSeg,
	WORD						wHeapSize,
	LPSTR						lpszCmdLine
	)
	{
	I_UNUSED(wDataSeg);
	I_UNUSED(lpszCmdLine);

	SROleModule::Init(hInstance);

	if (wHeapSize > 0)
		UnlockData(0);

	return 1;	// the dll was initialized properly
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	WEP/*!!PORT!! Replace with DllEntryPoint */
//
// DESC:			Cleans up the DLL.
//
////////////////////////////////////////////////////////////////

extern "C"
int ALMCALLBACK		WEP/*!!PORT!! Replace with DllEntryPoint */(
	int							nParameter
	)
	{
	I_UNUSED(nParameter);
	
	SROleModule::Cleanup();

	return 1;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	::operator new and ::operator delete
//
// DESC:			Replaces the system-supplied new and delete operators.
//
////////////////////////////////////////////////////////////////

void*		operator new(
	size_t		s
	)
	{
	return MemAlloc(s);
	}

void		operator delete(
	void*			p
	)
	{
	MemFree(p);
	}
