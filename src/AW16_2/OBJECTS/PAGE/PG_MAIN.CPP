///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:  Essentials
// Object:        Page
//
// Filename:  "pg_main.cpp"
//
// Description:
//  Page Object Mian Routine 
//
////////////////////////////////////////////////////////////////
#define PG_MAIN

#include <windows.h>
#include <mem.h>
#include "a_alm.h"
#include "pg_glob.h"


extern int _WinAllocFlag;

extern "C" int far pascal LibMain(
	HINSTANCE hInst,
	WORD	/*wDataSeg*/,
	WORD	wHeapSize,
	LPSTR	/*lpszCmdLine*/ )

	{
	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = (HINSTANCE)hInst;

	if (wHeapSize > 0)
	  UnlockData (0);

	return 1;
	}


