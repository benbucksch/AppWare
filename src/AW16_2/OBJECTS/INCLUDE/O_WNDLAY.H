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

#ifdef __cplusplus
extern "C" {
#endif

//Creates all existing window items in the window.  Resizes the array
//of items, and returns the number of items created.  
int ALMAPI OWndLayCreateAllExistingItems(OBJECTID oiWindow, MEMBLOCKID miArray);


//Save the items in the array.  numItems is the number of items in the array to save.  
BOOL ALMAPI OWndLaySaveItems(OBJECTID oiWindow, MEMBLOCKID miArray, int numItems);


//Creates a new window item visual instance.  Returns the visual instance record.
//If the item is a shape, oiItem is the typeid of the shape library (and szShapeDllName is valid)
//Sets the next UID
OpWndItemD FAR* ALMAPI OWndLayCreateNewItem(OBJECTID oiWindow, OBJECTID oiItem, PWICreateFN createFunction, LPRECT lprcItem, MEMBLOCKID miExtra, LPSTR szShapeDllName);


//Creates a new group visual instance.  Returns the visual instance record.
//Sets the next UID.
OpWndItemD FAR* ALMAPI OWndLayCreateNewGroup(OBJECTID oiWnd, MEMBLOCKID miItems, int nCount);


//Destroys a single item.  
BOOL ALMAPI OWndLayDestroyItem(OpWndItemD FAR* wi);

//Recreate an existing item
OpWndItemD FAR* ALMAPI OWndLayRecreateExistingItem(OpWndItemD FAR* wiOld);


//Destroys all items in the array and resizes the array to 0.
BOOL ALMAPI OWndLayDestroyAllExistingItems(MEMBLOCKID miArray, int numItems);

//////////////////////////////////////////////////
// Functions to get and set the window attributes
/////////////////////////////////////////////////

int ALMAPI    OWndLayGetTitle(OBJECTID oiWindow, LPSTR szBufffer, int len);
void ALMAPI   OWndLaySetTitle(OBJECTID oiWindow, LPCSTR szTitle);
BOOL ALMAPI   OWndLayHasTitleBar(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasTitleBar(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasSysmenu(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasSysmenu(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasMaxbutton(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasMaxbutton(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasMinbutton(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasMinbutton(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasHScroll(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasHScroll(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasVScroll(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasVScroll(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasResizeFrame(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasResizeFrame(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasDialogFrame(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasDialogFrame(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayIsModal(OBJECTID oiWindow);
void ALMAPI   OWndLaySetIsModal(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayIsPopup(OBJECTID oiWindow);
void ALMAPI   OWndLaySetIsPopup(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayIsInitVisible(OBJECTID oiWindow);
void ALMAPI   OWndLaySetIsInitVisible(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayDrawsSmooth(OBJECTID oiWindow);
void ALMAPI   OWndLaySetDrawsSmooth(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayQuitsOnClose(OBJECTID oiWindow);
void ALMAPI   OWndLaySetQuitsOnClose(OBJECTID oiWindow, BOOL set);
BOOL ALMAPI   OWndLayHasMaxSize(OBJECTID oiWindow);
void ALMAPI   OWndLaySetHasMaxSize(OBJECTID oiWindow, BOOL set);

void ALMAPI   OWndLayGetInitialClientRect(OBJECTID oiWindow, LPRECT rect);
void ALMAPI   OWndLaySetInitialClientRect(OBJECTID oiWindow, const LPRECT rect);
int ALMAPI    OWndLayGetInitialWindowState(OBJECTID oiWindow);
void ALMAPI   OWndLaySetInitialWindowState(OBJECTID oiWindow, int nCmdShow);
POINT ALMAPI  OWndLayGetMaxSize(OBJECTID oiWindow);
void ALMAPI   OWndLaySetMaxSize(OBJECTID oiWindow, POINT maxSize);
POINT ALMAPI  OWndLayGetMinSize(OBJECTID oiWindow);
void ALMAPI   OWndLaySetMinSize(OBJECTID oiWindow, POINT minSize);
POINT ALMAPI  OWndLayGetScrollSize(OBJECTID oiWindow);
void ALMAPI   OWndLaySetScrollSize(OBJECTID oiWindow, POINT scrollSize);
OBJECTID ALMAPI OWndLayGetMenubarID(OBJECTID oiWindow);
void ALMAPI   OWndLaySetMenuBarID(OBJECTID oiWindow, OBJECTID oiMenu);

void ALMAPI   OWndLayGetColorsAndPattern(OBJECTID oiWindow, COLORREF FAR* pclrCBackground, COLORREF FAR* pclrCForeground,
                                         COLORREF FAR* pclrBWBackground, COLORREF FAR* pclrBWForeground,
                                         short FAR* ptrPattern, UINT cbSize);
void ALMAPI   OWndLaySetColorsAndPattern(OBJECTID oiWindow, COLORREF clrCBackground, COLORREF clrCForeground,
                                         COLORREF clrBWBackground, COLORREF clrBWForeground,
                                         const short FAR* ptrPattern, UINT cbSize);

void ALMAPI		OWndLaySetPaletteOwner(OBJECTID oiWindow, LONG uidOwner);
LONG ALMAPI		OWndLayGetPaletteOwner(OBJECTID oiWindow);

/***************************************************************************
 *  Functions to set item status/attribute flags
 **************************************************************************/

void ALMAPI   OWndLaySetItemTabstop(OpWndItemD FAR* wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemTabstop(OpWndItemD FAR* wi);
void ALMAPI   OWndLaySetItemGroupstop(OpWndItemD FAR* wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemGroupstop(OpWndItemD FAR* wi);
void ALMAPI   OWndLaySetItemGrowflag(OpWndItemD FAR* wi, int growflag);
int  ALMAPI   OWndLayGetItemGrowflag(OpWndItemD FAR* wi);
void ALMAPI   OWndLaySetItemLocked(OpWndItemD FAR* wi, BOOL fLocked);
BOOL ALMAPI   OWndLayIsItemLocked(OpWndItemD FAR* wi);
void ALMAPI   OWndLaySetItemVisibleFlag(OpWndItemD FAR* wi, BOOL fSet);
void ALMAPI   OWndLaySetItemIsGroup(OpWndItemD FAR* wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemGroup(OpWndItemD FAR* wi);
void ALMAPI   OWndLaySetItemIsShape(OpWndItemD FAR* wi, BOOL set);
BOOL ALMAPI   OWndLayIsItemShape(OpWndItemD FAR* wi);

/***************************************************************************
 *  Overridden Windows functions for the Layout.
 **************************************************************************/

void ALMAPI   OWndLaySetMappingMode(HDC hdc, int mappingMode);  //OWNDLAY_MM_DEPENDANT or OWNDLAY_MM_INDEPENDANT
void ALMAPI   OWndLaySetWindowHandle(OBJECTID oiWindow, HWND hwnd);
BOOL ALMAPI   OWndLaySetViewportOrgEx(OBJECTID oiWindow, int nX, int nY, LPPOINT lppt);
BOOL ALMAPI   OWndLayGetViewportOrgEx(OBJECTID oiWindow, LPPOINT lppt);
BOOL ALMAPI   OWndLaySetWindowOrgEx(OBJECTID oiWindow, int nX, int nY, LPPOINT lppt);
BOOL ALMAPI   OWndLayGetWindowOrgEx(OBJECTID oiWindow, LPPOINT lppt);

void ALMAPI   OWndLayAdjWindowRectEx(LPRECT lprc, DWORD dwStyle,
                                     OBJECTID oiMenuBar, DWORD dwExStyle);

#ifdef __cplusplus
}
#endif


#endif
