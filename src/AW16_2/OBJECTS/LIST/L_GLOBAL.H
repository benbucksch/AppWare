////////////////////////////////
// This file contains the global variables
// used by the list object
///////////////////////////////

#ifndef L_GLOBAL_H
#define L_GLOBAL_H

#ifdef I_MAIN
HINSTANCE						g_hInstance;
LPSTR								g_listboxObjectClassName = "AppwareListbox";
LPSTR								g_comboboxObjectClassName = "AppwareCombobox";
WNDPROC							g_defListboxProc = NULL;
WNDPROC							g_defComboboxProc = NULL;
WNDPROC							g_defComboboxEditProc = NULL;
WNDPROC							g_defComboboxListProc = NULL;
int 								g_iListboxThisPointerOffset;
int 								g_iComboboxThisPointerOffset;
UINT								g_uDoubleClickTime = 0;
POINT								g_ptDoubleClick = {0,0};

#else
extern HINSTANCE		g_hInstance;
extern LPSTR				g_listboxObjectClassName;
extern LPSTR				g_comboboxObjectClassName;
extern WNDPROC			g_defListboxProc;
extern WNDPROC			g_defComboboxProc;
extern WNDPROC			g_defComboboxEditProc;
extern WNDPROC			g_defComboboxListProc;
extern int 					g_iListboxThisPointerOffset;
extern int 					g_iComboboxThisPointerOffset;
extern UINT					g_uDoubleClickTime;
extern POINT				g_ptDoubleClick;

#endif


#endif

