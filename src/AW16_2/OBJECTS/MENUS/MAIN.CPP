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
// DESCRIPTION:		LibMain for the ALM_MENU.DLL
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>

#include "modinfo.h"

extern int _WinAllocFlag;

HINSTANCE	 MODULEINFO::hInstance = 0;
char       MODULEINFO::testWndClassName[] = "AppWareMenuBarTestWindow";

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: LibMain
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" int FAR PASCAL LibMain(
  HINSTANCE hInst,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine)
  {

	I_UNUSED(wDataSeg);
	I_UNUSED(lpszCmdLine);

	_WinAllocFlag = GMEM_SHARE;

	MODULEINFO::hInstance = hInst;

	if (wHeapSize > 0)
			UnlockData(0);

	return 1;

  }

