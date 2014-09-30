//////////////////////////////////////////
/// This file contains the member functions
///  for the class: WINDOW
//////////////////////////////////////////
#define I_MAIN

#include "w_includ.h"

extern int _WinAllocFlag;

int far pascal _export HaveWeBeenLoadedYet()
	{
	static BOOL							beenHere = FALSE;
	BOOL										rVal;

	rVal = beenHere;
	beenHere = TRUE;
	return rVal;
	}	

///////////////////////////////

extern "C" int far pascal LibMain (
	HINSTANCE 							hIns,
	WORD 										wDataSeg,
	WORD 										cbHeapSize,
	LPSTR										lpCmdLine)
	{
	WNDCLASS								wndclass;

	I_UNUSED(wDataSeg);
	I_UNUSED(cbHeapSize);
	I_UNUSED(lpCmdLine);

	if (HaveWeBeenLoadedYet() )
		return 0;

	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = hIns;


		/**********************************************************************
		 *** N O T E !!!
		 ***  The CS_OWNDDC bit is set so the ViewportOrigin remains intact
		 ***  after ReleaseDC.
		 **********************************************************************/

	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndclass.lpfnWndProc   = WndCallbackProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = sizeof(OBJECTID);
	wndclass.hInstance     = g_hInstance;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = NULL;
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = g_windowObjectClassName;
	RegisterClass( &wndclass );


	return TRUE;
	}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

extern "C" int far pascal _export WEP(
	int											nParameter)
	{
	I_UNUSED(nParameter);

	UnregisterClass(g_windowObjectClassName, g_hInstance);
	return 1;
  }
