//////////////////////////////////////////
/// This file contains the member functions
///  for the class: WINDOW
//////////////////////////////////////////
#define I_MAIN

#include "w_includ.h"

///////////////////////////////

BOOL WINAPI DllEntryPoint(
	HINSTANCE								hinstDLL,	// handle of DLL module 
  DWORD  									fdwReason,	// reason for calling function 
  LPVOID  								lpvReserved) 	// reserved 
	{
	WNDCLASS								wndclass;

	I_UNUSED(lpvReserved); 

	if (fdwReason == DLL_PROCESS_ATTACH)
		{
		g_hInstance = hinstDLL;

		//N O T E -- The CS_OWNDDC bit is set so the ViewportOrigin remains intact after ReleaseDC.
		wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
		wndclass.lpfnWndProc   = WndCallbackProc;
		wndclass.cbClsExtra    = 0;
		wndclass.cbWndExtra    = sizeof(AOBJECTID);
		wndclass.hInstance     = g_hInstance;
		wndclass.hIcon         = NULL;
		wndclass.hCursor       = NULL;
		wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wndclass.lpszMenuName  = NULL;
		wndclass.lpszClassName = g_windowObjectClassName;
		RegisterClass( &wndclass );
		}

	else if (fdwReason == DLL_PROCESS_DETACH)
		{
		UnregisterClass(g_windowObjectClassName, g_hInstance);
		}

	return TRUE;
	}





