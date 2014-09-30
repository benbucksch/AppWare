////////////////////////////////
// This file contains the global variables
// used by the number object
///////////////////////////////

#ifndef N_GLOBAL_H
#define N_GLOBAL_H

#ifdef I_MAIN
HINSTANCE						g_hInstance;
LPSTR								g_numberObjectClassName = "AppwareNumber";
WNDPROC							g_defEditProc = NULL;
int 								g_iThisPointerOffset;
char								g_szNumberParts[100]; //Parts of a number. Loaded from a string res
char								g_chCurrency; //Loaded from win.ini
char								g_chThouSep;	//Loaded from win.ini
char								g_chDecSep; 	//Loaded from win.ini
LPSTR								g_szIntegerNumberFormat = "#########0";
NFORM_INFO      		g_integerNumberFormatInfo;
LPSTR								g_szRealNumberFormat= "#,###,###,##0.0###";
NFORM_INFO      		g_realNumberFormatInfo;
#else
extern HINSTANCE 		g_hInstance;
extern LPSTR 				g_numberObjectClassName;
extern WNDPROC 			g_defEditProc;
extern int 					g_iThisPointerOffset;
extern char					g_szNumberParts[];
extern char					g_chCurrency;
extern char					g_chThouSep;
extern char					g_chDecSep;
extern LPSTR				g_szIntegerNumberFormat;
extern NFORM_INFO   g_integerNumberFormatInfo;
extern LPSTR				g_szRealNumberFormat;
extern NFORM_INFO   g_realNumberFormatInfo;
#endif


#endif

