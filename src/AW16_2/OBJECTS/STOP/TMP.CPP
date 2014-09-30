/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
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

#define ASTRICT
#define STRICT

#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>
#include <o_wndshp.h>

#include "palette.h"

//
//
// This file contains temporary stuff and stuff that is subject to change.
//
//

HINSTANCE		g_hInst;

extern int	_WinAllocFlag;

////////////////////////////////////////////////////////////////

void			InitPaletteSys(
	HINSTANCE		hInst
	)
	{
	g_hInst = hInst;
	PaletteWindow::Init(hInst);
	}

void			CleanPaletteSys(
	void
	)
	{
	PaletteWindow::Cleanup();
	}

////////////////////////////////////////////////////////////////

HINSTANCE					GetHInstance(
	void
	)
	{
	return g_hInst;
	}

////////////////////////////////////////////////////////////////

extern "C"
int ALMCALLBACK		LibMain(
	HINSTANCE				hInstance,
	WORD						wDataSeg,
	WORD						wHeapSize,
	LPSTR						lpszCmdLine
	)
	{
	I_UNUSED(wDataSeg);
	I_UNUSED(wHeapSize);
	I_UNUSED(lpszCmdLine);

	InitPaletteSys(hInstance);

#ifdef __BORLANDC__
	// Use this magical line to make the 'new' operator work!?
	_WinAllocFlag = GMEM_SHARE;
#endif

	if (wHeapSize > 0)
		UnlockData(0);

	return 1;	// the dll was initialized properly
	}

////////////////////////////////////////////////////////////////

extern "C"
int ALMCALLBACK		WEP(
	int							nParameter
	)
	{
	I_UNUSED(nParameter);

	CleanPaletteSys();

	return 1;
	}
