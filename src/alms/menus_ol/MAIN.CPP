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
// FILE:					MAIN.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		DllEntryPoint for the ALM_MENU.DLL
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
//#include <winnt.h>
#include <a_alm.h>
#include <intdebug.h>

#include "modinfo.h"

/// extern int _WinAllocFlag;

HINSTANCE	 MODULEINFO::hInstance = 0;
char       MODULEINFO::testWndClassName[] = "AppWareMenuBarTestWindow";

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DllEntryPoint
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI		DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD				dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

///	_WinAllocFlag = GMEM_SHARE;

	MODULEINFO::hInstance = hInstDLL;

	return TRUE;
	}


////////////////////////////////////////////////////////////////////////////



