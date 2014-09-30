////////////////////////////////
// This file contains the global variables
// used by the polybutton object
///////////////////////////////
	 
#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef I_MAIN
HINSTANCE						g_hInstance;
LPSTR								g_szDrawClassName = "EDIT_POLYDrawWindow";
char								g_szBitmapPathName[256];
#else
extern HINSTANCE 		g_hInstance;
extern LPSTR				g_szDrawClassName;
extern char					g_szBitmapPathName[];
#endif


#endif

