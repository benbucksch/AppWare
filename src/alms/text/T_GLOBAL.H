////////////////////////////////
// This file contains the global variables
// used by the text object

///////////////////////////////

#ifndef T_GLOBAL_H
#define T_GLOBAL_H

#ifdef I_MAIN
HINSTANCE						g_hInstance;
LPSTR								g_textObjectClassName = "AppwareText";
WNDPROC							g_defEditProc = NULL;
int 								g_iThisPointerOffset;
#else
extern HINSTANCE 		g_hInstance;
extern LPSTR 				g_textObjectClassName;
extern WNDPROC 			g_defEditProc;
extern int 					g_iThisPointerOffset;
#endif


#endif

