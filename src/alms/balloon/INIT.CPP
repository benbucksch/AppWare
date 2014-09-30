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
// FILE:          "init.cpp"
//
// AUTHOR:        Lee E. Thayer
//
// DESCRIPTION:   Balloon server initialization and entry point
//
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>

#include "server.h"


HINSTANCE  hInstance;
WNDCLASS  	wc;

const char*	szBalloonClass = "BalloonClass";			


extern "C" BOOL WINAPI DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD				dwReason,
	LPVOID				pReserved)
{
	I_UNUSED(pReserved);

	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			hInstance = hInstDLL;
			break;

		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}


//***************************************************************
//
//	Windows class registration for balloon class
//
//***************************************************************
int RegisterBalloonClass()
{
	static BYTE	fRegistered = FALSE;


	// Only register once
	if(fRegistered)
		return TRUE;

	wc.style         = NULL;
  	wc.lpfnWndProc   = BalloonWndProc;
  	wc.cbClsExtra    = 0;
  	wc.cbWndExtra    = sizeof(Balloon*);
  	wc.hInstance     = hInstance;
  	wc.hIcon         = NULL;
  	wc.hCursor       = NULL;
  	wc.hbrBackground = NULL;
  	wc.lpszMenuName  = NULL;
  	wc.lpszClassName = szBalloonClass;

	if(RegisterClass(&wc))
		fRegistered = TRUE;

	return fRegistered;
}


void UnregisterBalloonClass()
{
	UnregisterClass(szBalloonClass, hInstance);
}
