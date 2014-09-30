////////////////////////////////
// This file contains the global variables
// used by the button object
///////////////////////////////

#ifndef B_GLOBAL_H
#define B_GLOBAL_H

#ifdef I_MAIN
HINSTANCE						g_hInstance;
LPSTR								g_szButtonClassName = "AppwareButton";
WNDPROC							g_defButtonProc = NULL;
int 								g_iThisPointerOffset;
char								g_szBitmapPathName[256];
#else
extern HINSTANCE 		g_hInstance;
extern LPSTR 				g_szButtonClassName;
extern WNDPROC 			g_defButtonProc;
extern int 					g_iThisPointerOffset;
extern char					g_szBitmapPathName[];
#endif


#endif

