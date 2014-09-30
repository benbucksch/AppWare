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


HFONT ALMAPI  AUtlCreateFontIndirect(LOGFONT FAR* lf);
HFONT ALMAPI  AUtlCreateFont(short lfHeight, short lfWidth,
                             short lfEscapement, short lfOrientation,
                             short lfWeight, BYTE lfItalic,
                             BYTE lfUnderline, BYTE lfStrikeOut,
                             BYTE lfCharSet, BYTE lfOutPrecision,
                             BYTE lfClipPrecision, BYTE lfQuality,
                             BYTE lfPitchAndFamily, LPSTR lfFaceName);

short ALMAPI    AUtlDeleteFont(HFONT hFont);
 

HBRUSH ALMAPI   AUtlCreateBrushIndirect(LOGBRUSH FAR* lb);
HBRUSH ALMAPI   AUtlCreateSolidBrush(COLORREF clRef);

short ALMAPI    AUtlDeleteBrush(HBRUSH hBrush);


/* ****************************************************************
 * Utility functions for dialogs
 * **************************************************************** */

void ALMAPI     AUtlSetTitleFont(TYPEID tType, LPSTR pszText, HWND hCtl);
void ALMAPI     AUtlCenterDialog(HWND hDlg, UINT nReserved);

BOOL ALMAPI     AUtlDlgEditFont(HWND hWndOwner, LPCSTR displayString,
                                LPCSTR dialogTitle, LOGFONT FAR* lf,
                                short FAR* deciPointSize, BOOL doColors,
                                COLORREF FAR* fgColor, COLORREF FAR* bgColor,
                                BOOL doEffects, DWORD flags,
                                BOOL printerFonts, HDC hPrinterDC);


#ifdef __cplusplus
}
#endif

#endif  /* A_ALMUTL_H */
