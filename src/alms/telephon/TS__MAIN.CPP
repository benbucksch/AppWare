/*************************************************************************
**	File Name:
**		TS__MAIN.CPP	Copyright 1994 Novell Inc.
**
**	Description:
**      The main program module for the StopLight Window Object.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

#define STRICT
#define ASTRICT

//	Module include references

#include <windows.h>
#include <string.h>
#include <a_alm.h>
#include <a_almutl.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"
#include "tlinvisu.h"
#include "o_tdev.h"
#include "o_tlin.h"

//	Module externed and un-mangled function prototypes

#ifdef __cplusplus
extern "C" {
#endif

int CALLBACK LibMain/*!!PORT!! Replace with DllEntryPoint */(HINSTANCE, WORD, WORD, LPSTR);

#ifdef __cplusplus
}
#endif

//	Module globals

HINSTANCE hInstance;

//	Module external references

extern int _WinAllocFlag;

//	Module code

/*************************************************************************
**	Function:
**		LibMain/*!!PORT!! Replace with DllEntryPoint */						(AppWare Bus)
**
**	Description:
**		Called when a library is initially loaded.
*/

int CALLBACK LibMain/*!!PORT!! Replace with DllEntryPoint */(HINSTANCE hInst, WORD, WORD wHeapSize, LPSTR) {
	WNDCLASS wc;

#ifdef __BORLANDC__
	_WinAllocFlag = GMEM_SHARE;
#endif

	hInstance = hInst;

	if (wHeapSize != 0)
		UnlockData(0);

//	Register class for TLine Window Object.

	wc.style			= 0;
	wc.lpfnWndProc 		= TLineWndProc;
	wc.cbClsExtra 		= 0;
	wc.cbWndExtra		= sizeof(TLineVI  *);
	wc.hInstance 		= hInstance;
	wc.hIcon 			= 0;
	wc.hCursor 			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground 	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName 	= 0;
	wc.lpszClassName 	= TLINE_CLASSNAME;

    RegisterClass(&wc);

//	Register class for TLine Window Object List Box Child Window.

	wc.style 			= CS_SAVEBITS;
	wc.lpfnWndProc 		= TLineLBWndProc;
	wc.cbClsExtra 		= 0;
	wc.cbWndExtra 		= sizeof(LONG) * 2;
	wc.hInstance 		= hInstance;
	wc.hIcon			= 0;
	wc.hCursor 			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= 0;
	wc.lpszMenuName 	= 0;
	wc.lpszClassName 	= TLINE_LB_CLASSNAME;

    RegisterClass(&wc);

	// register window class for TSAPI event handler window

	wc.style 			= 0;
	wc.lpfnWndProc 		= CSTAEventHandler;
	wc.cbClsExtra 		= 0;
	wc.cbWndExtra 		= sizeof(OBJECTID);
	wc.hInstance 		= hInstance;
	wc.hIcon 			= 0;
	wc.hCursor 			= 0;
	wc.hbrBackground	= 0;
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= CSTA_EVENTCLASSNAME;

	RegisterClass(&wc);

	return 1;
}
