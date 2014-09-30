/* ****************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * **************************************************************** */

#ifndef A_ALMUTL_H
#define A_ALMUTL_H

#ifdef __cplusplus
extern "C" {
#endif

/* ****************************************************************
 * Resource Management
 * **************************************************************** */

// Return Values from Utility Resource Management Functions
#define AUTL_HANDLENOTCREATED  0  // Failed to create Resource Handle 
#define AUTL_HANDLEDELETED     1  // Deleted Resource Handle
#define AUTL_HANDLEINUSE       2  // Handle is in Use


HFONT ALMAPI  AUtlCreateFontIndirect(LOGFONT * lf);
HFONT ALMAPI  AUtlCreateFont(int lfHeight, int lfWidth,
                             int lfEscapement, int lfOrientation,
                             int lfWeight, DWORD lfItalic,
                             DWORD lfUnderline, DWORD lfStrikeOut,
                             DWORD lfCharSet, DWORD lfOutPrecision,
                             DWORD lfClipPrecision, DWORD lfQuality,
                             DWORD lfPitchAndFamily, LPCSTR lfFaceName);

short ALMAPI    AUtlDeleteFont(HFONT hFont);
 

HBRUSH ALMAPI   AUtlCreateBrushIndirect(LOGBRUSH * lb);
HBRUSH ALMAPI   AUtlCreateSolidBrush(COLORREF clRef);

short ALMAPI    AUtlDeleteBrush(HBRUSH hBrush);


/* ****************************************************************
 * Utility functions for dialogs
 * **************************************************************** */

void ALMAPI     AUtlSetTitleFont(ATYPEID tType, LPCSTR pszText, HWND hCtl);
void ALMAPI     AUtlCenterDialog(HWND hDlg, UINT nReserved);

BOOL ALMAPI     AUtlDlgEditFont(HWND hWndOwner, LPCSTR displayString,
                                LPCSTR dialogTitle, LOGFONT * lf,
                                int * deciPointSize, BOOL doColors,
                                COLORREF * fgColor, COLORREF * bgColor,
                                BOOL doEffects, DWORD flags,
                                BOOL printerFonts, HDC hPrinterDC);


/********************************************************************
 *     Registry Access Functions for Win32                        
 ********************************************************************/

typedef enum 	{ AUTL_KEY_APEBUILDER=0,
								AUTL_KEY_ALMBUILDER=1,
								AUTL_KEY_ALMS=2,					
								AUTL_KEY_DIRECTORIES=3,
								AUTL_KEY_APPSETTINGS=4,
								AUTL_KEY_FONTTRANS=5,
								AUTL_KEY_DEBUG=6} AUTL_SUBKEY_TYPE;



BOOL ALMAPI		 	AUtlWriteKeyLong(
										HKEY subKey,
										LPCSTR name,
										long value);

long ALMAPI		 	AUtlGetKeyLong(
										HKEY subKey, 
										LPCSTR name, 
										long defValue);

BOOL ALMAPI		 	AUtlWriteKeyString(
										HKEY subKey,
	 									LPCSTR name,
										LPCSTR data);

int ALMAPI 			AUtlGetKeyString(
										HKEY subKey,
	 									LPCSTR name,
	 									LPSTR buffer,
	 									WORD bufSize,
										LPCSTR defValue);

/* sysKey should be	one of Microsoft's predefined reserved key values. *
 * resturns ERROR_SUCCESS or an error value                            */
HKEY				ALMAPI AUtlOpenProductSubKey( HKEY sysKey, 
																					AUTL_SUBKEY_TYPE subKeyType);

/* hKey should be a key opened by AUtlOpenProductSubKey */                     
LONG				ALMAPI AUtlCloseProductSubKey( HKEY hKey );


/////////////////////////////////////////////////////////////////
//Operating system information.

#define AUTL_OS_UNKNOWN		1
#define AUTL_OS_WINNT			2
#define AUTL_OS_CHICAGO		3

WORD ALMAPI AUtlGetOSVersion();




#ifdef __cplusplus
}
#endif

#endif  /* A_ALMUTL_H */
