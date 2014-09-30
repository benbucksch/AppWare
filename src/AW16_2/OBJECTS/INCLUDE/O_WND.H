/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_WND_H
#define O_WND_H

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

/* ***********************************************************************
 * TYPEID for Window Object
 * *********************************************************************** */

#define OTYPE_WINDOW                  MAKETYPEID('W', 'i', 'n', 'o')

/* ***********************************************************************
 * Typedefs and constant definitions
 * *********************************************************************** */

/* Accelerator key specifiers */
#define OWND_SHIFT                    0x01
#define OWND_CTRL                     0x02
#define OWND_ALT                      0x04

/* Conversion defines */
#define OWND_TMM_PER_INCH             254.0
#define OWND_INCH_PER_TMM             (1.0/254.0)

/* Prototype for create function for Window Item Protocol objects */
typedef BOOL (ALMAPI* CALLVIPROC)(OpWndItemD far*, long, long);

/* ***********************************************************************
 * Window Object APIs
 * *********************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif

HWND     ALMAPI   OWndGetHandle(OBJECTID oiWnd);
BOOL     ALMAPI   OWndSimulateAccelKey(OBJECTID windowID, TYPEID type,
                                       char vKey, char modifier);
HCURSOR  ALMAPI   OWndSetCursor(HCURSOR hCursor, BOOL master);
HCURSOR  ALMAPI   OWndReleaseCursor(void);
OBJECTID ALMAPI   OWndGetActiveWindowObject(void);
BOOL     ALMAPI   OWndActivateWindow(OBJECTID id);
void     ALMAPI   OWndGetClientRect(OBJECTID oiWindow, LPRECT rect);
BOOL     ALMAPI   OWndIsLayoutTime(void);
void     ALMAPI   OWndDPtoLP(OBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndLPtoDP(OBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndDStoLS(OBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndLStoDS(OBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndInvalidateLogicalRect(OBJECTID oiWindow,
                                            const LPRECT rcInvalid, BOOL bErase);
void     ALMAPI   OWndValidateLogicalRect(OBJECTID oiWindow,
                                          const LPRECT rcValid);

BOOL     ALMAPI		OWndGetPalette(OBJECTID oiWindow, MEMBLOCKID miPalette);
BOOL 		 ALMAPI 	OWndSetPalette(OpWndItemD* wi, PALETTEENTRY* pPaletteEntries, WORD wNumPaletteEntries);
BOOL 		 ALMAPI 	OWndQueryCanSetPalette(OpWndItemD* wi);
/* Menu handling */
OBJECTID ALMAPI   OWndGetActiveMenuBar(void); 
BOOL     ALMAPI   OWndShowMenuBar(OBJECTID oiMenu);
BOOL     ALMAPI   OWndHideMenuBar(OBJECTID oiMenu);
BOOL     ALMAPI   OWndSetMenuBar(OBJECTID oiMenu, OBJECTID oiWindow); 
BOOL     ALMAPI   OWndRedrawMenuBar(OBJECTID oiMenu); 
BOOL 	   ALMAPI		OWndSetMenuAccelKey(OBJECTID oiWindow, short itemID, char vKey, char modifier);
                                                        

/* ***********************************************************************
 * Window Item APIs
 * *********************************************************************** */

/* HwndItem window manipulation */
HWND ALMAPI   OWndCreateWindowEx(OpHwndWndItemD far* hwi, DWORD dwExStyle,
                                 LPCSTR szClassName, LPCSTR szWindowName,
                                 DWORD dwStyle, HINSTANCE hinst, void FAR* lpvCreateParams);
BOOL ALMAPI   OWndMoveWindow(OpHwndWndItemD far* hwi, const LPRECT newRect,
                             BOOL repaint);

/* Mouse capture APIs */
OpWndItemD far* ALMAPI  OWndSetCaptureItem(OpWndItemD far* wi);
void            ALMAPI  OWndReleaseCapture(void);
OpWndItemD far* ALMAPI  OWndGetCaptureItem(void);

/* Visual instance traversal */
BOOL ALMAPI   OWndCallEachVisualInstance(OBJECTID targetObject, CALLVIPROC proc, long lParam1, long lParam2);

/* Visual instance attributes */
BOOL ALMAPI   OWndSetItemAccelKey(OpWndItemD far* wi, char vKey, char modifier);
void ALMAPI   OWndSetItemVisible(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndIsItemVisible(OpWndItemD far* wi);
void ALMAPI   OWndSetItemEnabled(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndIsItemEnabled(OpWndItemD far* wi);
void ALMAPI   OWndSetItemActivatable(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndIsItemActivatable(OpWndItemD far* wi);
void ALMAPI   OWndSetItemTransparent(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndIsItemTransparent(OpWndItemD far* wi);
void ALMAPI   OWndSetItemUsesCursorKeys(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseCursorKeys(OpWndItemD far* wi);
void ALMAPI   OWndSetItemUsesTab(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseTab(OpWndItemD far* wi);
void ALMAPI   OWndSetItemUsesDefaultKey(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseDefaultKey(OpWndItemD far* wi);
void ALMAPI   OWndSetItemUsesCancelKey(OpWndItemD far* wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseCancelKey(OpWndItemD far* wi);
void ALMAPI   OWndSetDefaultItem(OpWndItemD far* wi);
BOOL ALMAPI   OWndIsItemDefault(OpWndItemD far* wi);
void ALMAPI   OWndSetCancelItem(OpWndItemD far* wi);
BOOL ALMAPI   OWndIsItemCancel(OpWndItemD far* wi);
BOOL ALMAPI   OWndIsItemObject(OpWndItemD far* wi);
BOOL ALMAPI   OWndIsLiteItem(OpWndItemD far* wi);
BOOL ALMAPI   OWndIsHwndItem(OpWndItemD far* wi);

/* Miscellaneous */
OpWndItemD far* ALMAPI  OWndSetActiveItem(OpWndItemD far* wi);
OpWndItemD far* ALMAPI  OWndGetActiveItem(void);
OpWndItemD far* ALMAPI  OWndGetNextItem(OpWndItemD far* wi, OBJECTID windowID);
OpWndItemD far* ALMAPI  OWndGetPreviousItem(OpWndItemD far* wi, OBJECTID windowID);
WORD            ALMAPI  OWndGetItemControlID(OpHwndWndItemD far* hwi);

HFONT ALMAPI OWndCreateFontTmm(HDC hdcTarget, int nHeightInTmm, int nWidthInTmm, int nEscapement, int nOrientation,
                               int fnWeight, BYTE fbItalic, BYTE fbUnderline, BYTE fbStrikeOut, BYTE fbCharSet,
                               BYTE fbOutputPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily,
                               LPSTR lpszFace);
BOOL ALMAPI OWndDeleteFont(HFONT hfont);

OBJECTID ALMAPI OWndWindowObjectFromHwnd(HWND hwnd);

BOOL ALMAPI OWndHideItem(OpWndItemD far* wi); /* return TRUE if the item visible, then hidden */
BOOL ALMAPI OWndShowItem(OpWndItemD far* wi); /* return TRUE if the item invisible, then shown */
BOOL ALMAPI OWndMoveItem(OpWndItemD far* wi, LPRECT rcNew); /* return TRUE if the item was moved to a new location */
LONG ALMAPI OWndUIDFromItem(OpWndItemD far* wi);
OpWndItemD far* ALMAPI OWndItemFromUID(OBJECTID oiWnd, LONG uid);

#ifdef __cplusplus
}
#endif

#endif
