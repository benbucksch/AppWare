///////////////////////////////////////////////////////////////
//
//	(c) 1993, 1994 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
//	"dll_main.cpp"
//
// DLL ALM entry point.
//
////////////////////////////////////////////////////////////////////////////

#define ASTRICT
#define STRICT

#include <a_alm.h>
#include <stdlib.h>
#include <string.h>
#include <intdebug.h>

#include "dll.h"


/******************************************************************
*	global variables
******************************************************************/

HINSTANCE	hInstance;
extern int _WinAllocFlag;


/******************************************************************
*	LibMain/*!!PORT!! Replace with DllEntryPoint */: DLL entry point.
******************************************************************/

int CALLBACK /*!!PORT!! was _far_ _pascal_*/  LibMain/*!!PORT!! Replace with DllEntryPoint */ (
	HINSTANCE		hInst,
	WORD 				wDataSeg,
	WORD 				wHeapSize,
	LPSTR				lpCmdLine)
{
	WNDCLASS    wndclass;
	I_UNUSED(wDataSeg);
	I_UNUSED(lpCmdLine);

#ifdef	__BORLANDC__
	_WinAllocFlag = GMEM_SHARE ;
#endif

	hInstance = hInst;
	if ( wHeapSize > 0 )
		UnlockData(0);
	GetClassInfo(NULL, MAKEINTRESOURCE(32770L), &wndclass) ;
	wndclass.style &= ~CS_GLOBALCLASS ;
	wndclass.hInstance = hInstance ;
	wndclass.lpszClassName = "DLLDialog" ;
	RegisterClass (&wndclass) ;
	return TRUE;					// DLL is initialized properly.
}			// LibMain/*!!PORT!! Replace with DllEntryPoint */...
