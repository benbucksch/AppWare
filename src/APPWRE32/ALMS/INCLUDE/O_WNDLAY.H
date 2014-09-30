/* ***********************************************************************
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
 * *********************************************************************** */

#ifndef O_WNDLAY_H
#define O_WNDLAY_H

#define OWNDLAY_MM_DEPENDANT              100
#define OWNDLAY_MM_INDEPENDANT            101

#define OWNDLAY_ITEM_ID_OFFSET            1000

#define OWNDLAY_ITEM_GROW_TOP             1
#define OWNDLAY_ITEM_GROW_LEFT            2
#define OWNDLAY_ITEM_GROW_BOTTOM          4
#define OWNDLAY_ITEM_GROW_RIGHT           8

#define OWNDLAY_SYSCOLOR_FG               0xFE000000L
#define OWNDLAY_SYSCOLOR_BG               0xFF000000L

#define OWNDLAY_LAYOUTDATA                1L

typedef enum {OWND_PALETTE_MODE_COMPROMISE=0, OWND_PALETTE_MODE_ORDER=1} OWND_PALETTE_MODE; //Used for OWndLay<Get,Set>PaletteMode

#ifdef __cplusplus
extern "C" {
#endif

//Creates all existing window items in the window.  Resizes the array
//of items, and returns the number of items created.  
int ALMAPI OWndLayCreateAllExistingItems(AOBJECTID oiWindow, AMEMBLOCKID miArray);


//Save the items in the array.  numItems is the number of items in the array to save.  
BOOL ALMAPI OWndLaySaveItems(AOBJECTID oiWindow, AMEMBLOCKID miArray, int numItems);


//Creates a new window item visual instance.  Returns the visual instance record.
//If the item is a shape, oiItem is the typeid of the shape library (and szShapeDllName is valid)
//Sets the next UID
OpWndItemD * ALMAPI OWndLayCreateNewItem(AOBJECTID oiWindow, AOBJECTID oiItem, PWICreateFN createFunction, LPRECT lprcItem, AMEMBLOCKID miExtra, LPSTR szShapeDllName);


//Creates a new group visual instance.  Returns the visual instance record.
//Sets the next UID.
OpWndItemD * ALMAPI OWndLayCreateNewGroup(AOBJECTID oiWnd, AMEMBLOCKID miItems, int nCount);


//Destroys a single item.  
BOOL ALMAPI OWndLayDestroyItem(OpWndItemD * wi);

//Recreate an existing item
OpWndItemD * ALMAPI OWndLayRecreateExistingItem(OpWndItemD * wiOld);


//Destroys all items in the array and resizes the array to 0.
BOOL ALMAPI OWndLayDestroyAllExistingItems(AMEMBLOCKID miArray, int numItems);

//////////////////////////////////////////////////
// Functions to get and set the window attributes
/////////////////////////////////////////////////

int ALMAPI    OWndLayGetTitle(AOBJECTID oiWindow, LPSTR szBufffer, int len);
void ALMAPI   OWndLaySetTitle(AOBJECTID oiWindow, LPCSTR szTitle);
BOOL ALMAPI   OWndLayHasTitleBar(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasTitleBar(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasSysmenu(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasSysmenu(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasMaxbutton(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasMaxbutton(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasMinbutton(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasMinbutton(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasHScroll(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasHScroll(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasVScroll(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasVScroll(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasResizeFrame(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasResizeFrame(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasDialogFrame(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasDialogFrame(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayIsModal(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetIsModal(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayIsPopup(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetIsPopup(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayIsInitVisible(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetIsInitVisible(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayDrawsSmooth(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetDrawsSmooth(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayQuitsOnClose(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetQuitsOnClose(AOBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasMaxSize(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetHasMaxSize(AOBJECTID oiWindow, BOOL set);

void ALMAPI   OWndLayGetInitialClientRect(AOBJECTID oiWindow, LPRECT rect);
void ALMAPI   OWndLaySetInitialClientRect(AOBJECTID oiWindow, const LPRECT rect);
int ALMAPI    OWndLayGetInitialWindowState(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetInitialWindowState(AOBJECTID oiWindow, int nCmdShow);
POINT ALMAPI  OWndLayGetMaxSize(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetMaxSize(AOBJECTID oiWindow, POINT maxSize);
POINT ALMAPI  OWndLayGetMinSize(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetMinSize(AOBJECTID oiWindow, POINT minSize);
POINT ALMAPI  OWndLayGetScrollSize(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetScrollSize(AOBJECTID oiWindow, POINT scrollSize);
AOBJECTID ALMAPI OWndLayGetMenuBarID(AOBJECTID oiWindow);
void ALMAPI   OWndLaySetMenuBarID(AOBJECTID oiWindow, AOBJECTID oiMenu);

void ALMAPI   OWndLayGetColorsAndPattern(AOBJECTID oiWindow, COLORREF  * pclrCBackground, COLORREF  * pclrCForeground,
                                         COLORREF  * pclrBWBackground, COLORREF  * pclrBWForeground,
                                         LPWORD ptrPattern, UINT cbSize);
void ALMAPI   OWndLaySetColorsAndPattern(AOBJECTID oiWindow, COLORREF clrCBackground, COLORREF clrCForeground,
                                         COLORREF clrBWBackground, COLORREF clrBWForeground,
                                         LPWORD ptrPattern, UINT cbSize);

/***************************************************************************
 *  Functions to set item status/attribute flags
 **************************************************************************/

void ALMAPI   OWndLaySetItemTabstop(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemTabstop(OpWndItemD * wi);
void ALMAPI   OWndLaySetItemGroupstop(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemGroupstop(OpWndItemD * wi);
void ALMAPI   OWndLaySetItemGrowflag(OpWndItemD * wi, short growflag);
short ALMAPI   OWndLayGetItemGrowflag(OpWndItemD * wi);
void ALMAPI   OWndLaySetItemLocked(OpWndItemD * wi, BOOL fLocked);
BOOL ALMAPI   OWndLayIsItemLocked(OpWndItemD * wi);
void ALMAPI   OWndLaySetItemVisibleFlag(OpWndItemD * wi, BOOL fSet);
void ALMAPI   OWndLaySetItemIsGroup(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemGroup(OpWndItemD * wi);
void ALMAPI   OWndLaySetItemIsShape(OpWndItemD * wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemShape(OpWndItemD * wi);

/***************************************************************************
 *  Overridden Windows functions for the Layout.
 **************************************************************************/

void ALMAPI   OWndLaySetMappingMode(HDC hdc, int mappingMode);  //OWNDLAY_MM_DEPENDANT or OWNDLAY_MM_INDEPENDANT
void ALMAPI   OWndLaySetWindowHandle(AOBJECTID oiWindow, HWND hwnd);
BOOL ALMAPI   OWndLaySetViewportOrgEx(AOBJECTID oiWindow, int nX, int nY, LPPOINT lppt);
BOOL ALMAPI   OWndLayGetViewportOrgEx(AOBJECTID oiWindow, LPPOINT lppt);
BOOL ALMAPI   OWndLaySetWindowOrgEx(AOBJECTID oiWindow, int nX, int nY, LPPOINT lppt);
BOOL ALMAPI   OWndLayGetWindowOrgEx(AOBJECTID oiWindow, LPPOINT lppt);

void ALMAPI   OWndLayAdjWindowRectEx(LPRECT lprc, DWORD dwStyle,
                                     AOBJECTID oiMenuBar, DWORD dwExStyle);

#ifdef __cplusplus
}
#endif


#endif
