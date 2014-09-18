/* ****************************************************************
 *
 * Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.
 *
 * THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
 * TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
 * COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
 * CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 * TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
 * NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
 * AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
 * LIABILITY.
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
