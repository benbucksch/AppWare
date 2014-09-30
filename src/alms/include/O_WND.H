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
 * ATYPEID for Window Object
 * *********************************************************************** */

#define OTYPE_WINDOW                  ATYPE_MAKEID('W', 'i', 'n', 'o')

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
typedef BOOL (ALMAPI* CALLVIPROC)(OpWndItemD *, long, long);

/* ***********************************************************************
 * Window Object APIs
 * *********************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif

HWND     ALMAPI   OWndGetHandle(AOBJECTID oiWnd);
BOOL     ALMAPI   OWndSimulateAccelKey(AOBJECTID windowID, ATYPEID type,
                                       char vKey, char modifier);
HCURSOR  ALMAPI   OWndSetCursor(HCURSOR hCursor, BOOL master);
HCURSOR  ALMAPI   OWndReleaseCursor(void);
AOBJECTID ALMAPI   OWndGetActiveWindowObject(void);
BOOL     ALMAPI   OWndActivateWindow(AOBJECTID id);
void     ALMAPI   OWndGetClientRect(AOBJECTID oiWindow, LPRECT rect);
BOOL     ALMAPI   OWndIsLayoutTime(void);
void     ALMAPI   OWndDPtoLP(AOBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndLPtoDP(AOBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndDStoLS(AOBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndLStoDS(AOBJECTID oiWindow, LPPOINT points, short numPoints);
void     ALMAPI   OWndInvalidateLogicalRect(AOBJECTID oiWindow,
                                            const LPRECT rcInvalid, BOOL bErase);
void     ALMAPI   OWndValidateLogicalRect(AOBJECTID oiWindow,
                                          const LPRECT rcValid);

BOOL     ALMAPI		OWndGetPalette(AOBJECTID oiWindow, AMEMBLOCKID miPalette);
BOOL     ALMAPI		OWndReconstructPalette(AOBJECTID oiWindow);

/* Menu handling */
AOBJECTID ALMAPI   OWndGetActiveMenuBar(void); 
BOOL     ALMAPI   OWndShowMenuBar(AOBJECTID oiMenu);
BOOL     ALMAPI   OWndHideMenuBar(AOBJECTID oiMenu);
BOOL     ALMAPI   OWndSetMenuBar(AOBJECTID oiMenu, AOBJECTID oiWindow); 
BOOL     ALMAPI   OWndRedrawMenuBar(AOBJECTID oiMenu); 
BOOL 	   ALMAPI		OWndSetMenuAccelKey(AOBJECTID oiWindow, short itemID, char vKey, char modifier);
                                                        

/* ***********************************************************************
 * Window Item APIs
 * *********************************************************************** */

/* HwndItem window manipulation */
HWND ALMAPI   OWndCreateWindowEx(OpHwndWndItemD * hwi, DWORD dwExStyle,
                                 LPCSTR szClassName, LPCSTR szWindowName,
                                 DWORD dwStyle, HINSTANCE hinst, void * lpvCreateParams);
BOOL ALMAPI   OWndMoveWindow(OpHwndWndItemD * hwi, const LPRECT newRect,
                             BOOL repaint);

/* Mouse capture APIs */
OpWndItemD * ALMAPI  OWndSetCaptureItem(OpWndItemD * wi);
void            ALMAPI  OWndReleaseCapture(void);
OpWndItemD * ALMAPI  OWndGetCaptureItem(void);

/* Visual instance traversal */
BOOL ALMAPI   OWndCallEachVisualInstance(AOBJECTID targetObject, CALLVIPROC proc, long lParam1, long lParam2);

/* Visual instance attributes */
BOOL ALMAPI   OWndSetItemAccelKey(OpWndItemD * wi, char vKey, char modifier);
void ALMAPI   OWndSetItemVisible(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndIsItemVisible(OpWndItemD * wi);
void ALMAPI   OWndSetItemEnabled(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndIsItemEnabled(OpWndItemD * wi);
void ALMAPI   OWndSetItemActivatable(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndIsItemActivatable(OpWndItemD * wi);
void ALMAPI   OWndSetItemTransparent(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndIsItemTransparent(OpWndItemD * wi);
void ALMAPI   OWndSetItemUsesCursorKeys(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseCursorKeys(OpWndItemD * wi);
void ALMAPI   OWndSetItemUsesTab(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseTab(OpWndItemD * wi);
void ALMAPI   OWndSetItemUsesDefaultKey(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseDefaultKey(OpWndItemD * wi);
void ALMAPI   OWndSetItemUsesCancelKey(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndDoesItemUseCancelKey(OpWndItemD * wi);
void ALMAPI   OWndSetDefaultItem(OpWndItemD * wi);
BOOL ALMAPI   OWndIsItemDefault(OpWndItemD * wi);
void ALMAPI   OWndSetCancelItem(OpWndItemD * wi);
BOOL ALMAPI   OWndIsItemCancel(OpWndItemD * wi);
BOOL ALMAPI   OWndIsItemObject(OpWndItemD * wi);
BOOL ALMAPI   OWndIsLiteItem(OpWndItemD * wi);
BOOL ALMAPI   OWndIsHwndItem(OpWndItemD * wi);

/* Miscellaneous */
OpWndItemD * ALMAPI  OWndSetActiveItem(OpWndItemD * wi);
OpWndItemD * ALMAPI  OWndGetActiveItem(void);
OpWndItemD * ALMAPI  OWndGetNextItem(OpWndItemD * wi, AOBJECTID windowID);
OpWndItemD * ALMAPI  OWndGetPreviousItem(OpWndItemD * wi, AOBJECTID windowID);
DWORD        ALMAPI  OWndGetItemControlID(OpHwndWndItemD * hwi);

HFONT ALMAPI OWndCreateFontTmm(HDC hdcTarget, int nHeightInTmm, int nWidthInTmm, int nEscapement, int nOrientation,
                               int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet,
                               DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily,
                               LPSTR lpszFace);
BOOL ALMAPI OWndDeleteFont(HFONT hfont);

AOBJECTID ALMAPI OWndWindowObjectFromHwnd(HWND hwnd);

BOOL ALMAPI OWndHideItem(OpWndItemD * wi); /* return TRUE if the item visible, then hidden */
BOOL ALMAPI OWndShowItem(OpWndItemD * wi); /* return TRUE if the item invisible, then shown */
BOOL ALMAPI OWndMoveItem(OpWndItemD * wi, LPRECT rcNew); /* return TRUE if the item was moved to a new location */
LONG ALMAPI OWndUIDFromItem(OpWndItemD * wi);
OpWndItemD * ALMAPI OWndItemFromUID(AOBJECTID oiWnd, LONG uid);

#ifdef __cplusplus
}
#endif

#endif
