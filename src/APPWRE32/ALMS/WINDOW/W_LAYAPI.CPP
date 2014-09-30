
#include "w_includ.h"

#include "o_wndgrp.h"
#include "menudata.h"


// local function prototypes ///////////////////////////////////////////////

OpWndItemD* ALMAPI CreateGroupWndItem(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long lStatus, AMEMBLOCKID mExtraData);

////////////////////////////////////////////////////////////////////////////

//Creates all existing window items in the window.  Resizes the array
//of items, and returns the number of items created.  
extern "C" int ALMAPI OWndLayCreateAllExistingItems(
  AOBJECTID                oiWindow,
  AMEMBLOCKID              miArray)
  {
  WINDOW*                 wnd;
  int                     rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return 0);

  rVal = wnd->CreateExistingItems(miArray);

  UnlockWindowData(oiWindow);
  return rVal;
  }

//Save the items in the array.  numItems is the number of items in the array to save.  
extern "C" BOOL ALMAPI OWndLaySaveItems(
  AOBJECTID                oiWindow,
  AMEMBLOCKID              miArray,
  int                   	numItems)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);

  rVal = wnd->PutItemsIntoDatas(miArray, numItems);
  UnlockWindowData(oiWindow);
  return rVal;
  }


//Creates a new window item visual instance.  Returns the visual instance record.
//If the item is a shape, oiItem is the typeid of the shape library, and bIsShape=TRUE
//Sets the next UID
extern "C" OpWndItemD* ALMAPI OWndLayCreateNewItem(
  AOBJECTID                oiWindow,
  AOBJECTID                oiItem,
  PWICreateFN             createFunction,
  LPRECT                  lprcItem,
  AMEMBLOCKID              miExtra,
  LPSTR                   szShapeDllName)
  {
  WINDOW*                 wnd;
  OpWndItemD*             wi;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return NULL);

  wi = wnd->CreateNewItem(oiItem, createFunction, lprcItem, miExtra, szShapeDllName);
  UnlockWindowData(oiWindow);

  return wi;
  }


//Creates a new group item visual instance.  Returns the visual instance record.
extern "C" OpWndItemD* ALMAPI OWndLayCreateNewGroup(
  AOBJECTID                oiWnd,
  AMEMBLOCKID              miItems,
  int                     nCount)
  {
  WINDOW*                 wnd;
  OpGroupWndItem*         gwi;
  OpWndItemD**            awi   = (OpWndItemD**)AMemLock(miItems);
  RECT                    rc    = {0, 0, 0, 0};

  wnd = LockWindowData(oiWnd);
  I_VERIFY_POINTER(wnd, return NULL);

  I_ASSERT(nCount>0 && awi);

  gwi = (OpGroupWndItem*)wnd->CreateNewItem((AOBJECTID)AGROUP_LIBTYPEID, CreateGroupWndItem, &rc, NULL, "ALM_WND.DLL");
  UnlockWindowData(oiWnd);

  gwi->Created(awi, nCount);
  AMemUnlock(miItems);

  return gwi;
  }


//Recreates an existing item.  Destroys the old item. Return a pointer to the new item
extern "C" OpWndItemD* ALMAPI OWndLayRecreateExistingItem(
  OpWndItemD*             wiOld)
  {
  AOBJECTID                oiParent = wiOld->m_oiParent;
  WINDOW*                 wnd;
  OpWndItemD*             wiNew;

  wnd = LockWindowData(oiParent);
  I_VERIFY_POINTER(wnd, return NULL);

  wiNew = wnd->RecreateExistingItem(wiOld);
  UnlockWindowData(oiParent);

  return wiNew;
  }


//Destroys a single item.  
extern "C" BOOL ALMAPI OWndLayDestroyItem(
  OpWndItemD*             wi)
  {
  wi->Destroy();
  return TRUE;
  }


//Destroys all items in the array and resizes the array to 0.
extern "C" BOOL ALMAPI OWndLayDestroyAllExistingItems(
  AMEMBLOCKID              miArray,
  int                     numItems)
  {
  return WINDOW::DestroyAllItems(miArray, numItems);
  }

//////////////////////////////////////////////////
// Functions to get and set the window attributes
/////////////////////////////////////////////////

extern "C" int ALMAPI OWndLayGetTitle(
  AOBJECTID                oiWindow,
  LPSTR                   szBuffer,
  int                     len)
  {
  WINDOW*                 wnd;
  int                     rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return 0);
  rVal = wnd->GetTitle(szBuffer, len);                  
  UnlockWindowData(oiWindow);
  return rVal;
  }


extern "C" void ALMAPI OWndLaySetTitle(
  AOBJECTID                oiWindow,
  LPCSTR                  szTitle)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetTitle(szTitle);
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasTitleBar(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return TRUE;);
  rVal = wnd->HasTitleBar();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasTitleBar(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasTitleBar(set);
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasSysmenu(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return TRUE);
  rVal = wnd->HasSysMenu();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasSysmenu(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasSysMenu(set);                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasMaxbutton(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return TRUE);
  rVal = wnd->HasMaxButton();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasMaxbutton(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasMaxButton(set);                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasMinbutton(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return TRUE);
  rVal = wnd->HasMinButton();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasMinbutton(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasMinButton(set);                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasHScroll(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->HasHorzScroll();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasHScroll(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasHorzScroll(set);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasVScroll(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->HasVertScroll();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasVScroll(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasVertScroll(set);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasResizeFrame(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return TRUE);
  rVal = wnd->HasResizeFrame();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasResizeFrame(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasResizeFrame(set);                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayHasDialogFrame(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->HasDialogFrame();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasDialogFrame(AOBJECTID oiWindow, BOOL set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasDialogFrame(set);                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayIsModal(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->IsModal();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetIsModal(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetIsModal(set);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayIsPopup(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->IsPopup();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetIsPopup(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetIsPopup(set);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayIsInitVisible(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return TRUE);
  rVal = wnd->IsInitVisible();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetIsInitVisible(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetIsInitVisible(set);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" BOOL ALMAPI OWndLayDrawsSmooth(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->DrawsSmooth();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetDrawsSmooth(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetDrawsSmooth(set);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

BOOL ALMAPI OWndLayQuitsOnClose(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->QuitsOnClose();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

void ALMAPI OWndLaySetQuitsOnClose(
  AOBJECTID                oiWindow, 
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetQuitsOnClose(set);                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////


extern "C" BOOL ALMAPI OWndLayHasMaxSize(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);
  rVal = wnd->HasMaxSize();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetHasMaxSize(
  AOBJECTID                oiWindow,
  BOOL                    set)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetHasMaxSize(set);                  
  UnlockWindowData(oiWindow);
  }


/////////////////////////

extern "C" void ALMAPI OWndLayGetInitialClientRect(
  AOBJECTID                oiWindow,
  LPRECT                  rect)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  *rect = wnd->GetInitialClientRect();                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetInitialClientRect(
  AOBJECTID                oiWindow,
  const LPRECT            rect)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetInitialClientRect(rect);                  
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" int ALMAPI OWndLayGetInitialWindowState(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  int                     rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return 0);
  rVal = wnd->GetInitialWindowState();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetInitialWindowState(
  AOBJECTID                oiWindow,
  int                     nCmdShow)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetInitialWindowState(nCmdShow);
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" POINT ALMAPI OWndLayGetMaxSize(AOBJECTID oiWindow)
  {
  WINDOW*                 wnd;
  POINT                   rVal = {0,0};

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return rVal);
  rVal = wnd->GetMaxSize();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetMaxSize(
  AOBJECTID                oiWindow,
  POINT                   maxSize)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetMaxSize(maxSize);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" POINT ALMAPI OWndLayGetMinSize(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  POINT                   rVal = {0,0};

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return rVal);
  rVal = wnd->GetMinSize();                 
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetMinSize(
  AOBJECTID                oiWindow,
  POINT                   minSize)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetMinSize(minSize);               
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" POINT ALMAPI OWndLayGetScrollSize(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  POINT                   rVal = {0,0};

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return rVal);
  rVal = wnd->GetScrollSize();                  
  UnlockWindowData(oiWindow);
  return rVal;
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetScrollSize(
  AOBJECTID                oiWindow,
  POINT                   scrollSize)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetScrollSize(scrollSize);                 
  UnlockWindowData(oiWindow);
  }

/////////////////////////

extern "C" AOBJECTID ALMAPI OWndLayGetMenuBarID(
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  AOBJECTID                rVal;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return 0);
  rVal = wnd->GetMenuObject();                  
  UnlockWindowData(oiWindow);
  if (!AObjCheckType(rVal, 0))
    rVal=0;
  return rVal;
  }
                                
/////////////////////////

extern "C" void ALMAPI OWndLaySetMenuBarID(
  AOBJECTID                oiWindow,
  AOBJECTID                oiMenu)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->AssignMenuObject(oiMenu);
  UnlockWindowData(oiWindow);
  }

///////////////////////////

extern "C" void ALMAPI OWndLayGetColorsAndPattern(
  AOBJECTID                oiWindow, 
  COLORREF  *          		pclrCBackground, 
  COLORREF  *          		pclrCForeground,
  COLORREF  *          		pclrBWBackground, 
  COLORREF  *          		pclrBWForeground,
  LPWORD              		ptrPattern, 
  UINT                  	cbSize)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->GetColorsAndPattern(pclrCBackground, pclrCForeground, pclrBWBackground, pclrBWForeground, ptrPattern, cbSize);
  UnlockWindowData(oiWindow);
  }

////////

extern "C" void ALMAPI OWndLaySetColorsAndPattern(
  AOBJECTID                oiWindow, 
  COLORREF                clrCBackground, 
  COLORREF                clrCForeground,
  COLORREF                clrBWBackground, 
  COLORREF                clrBWForeground,
  LPWORD                  ptrPattern, 
  UINT                    cbSize)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->SetColorsAndPattern(clrCBackground, clrCForeground, clrBWBackground, clrBWForeground, ptrPattern, cbSize);
  UnlockWindowData(oiWindow);
  }

///////////////////////////

extern "C" void ALMAPI OWndLaySetItemTabstop(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemTabstop(wi, set);
  }

extern "C" BOOL ALMAPI  OWndLayIsItemTabstop(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemTabstop(wi);
  }

/////////////////////////

extern "C" void ALMAPI  OWndLaySetItemGroupstop(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemGroupstop(wi, set);
  }

/////////////////////////

extern "C" BOOL ALMAPI  OWndLayIsItemGroupstop(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemGroupstop(wi);
  }

///////////////////////////

extern "C" void ALMAPI OWndLaySetItemGrowflag(
  OpWndItemD*             wi, 
  short                   growflag)
  {
  WINDOWAPI::SetItemGrowflag(wi, growflag);
  }

///////////////////////////

extern "C" short ALMAPI OWndLayGetItemGrowflag(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::GetItemGrowflag(wi);
  }

///////////////////////////

extern "C" void ALMAPI OWndLaySetItemLocked(
  OpWndItemD*             wi, 
  BOOL                    fLocked)
  {
  WINDOWAPI::SetItemLocked(wi, fLocked);
  }

///////////////////////////

extern "C" BOOL ALMAPI OWndLayIsItemLocked(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemLocked(wi);
  }

/////////////////////////////////////////////////

extern "C" void ALMAPI OWndLaySetItemIsShape(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemIsShape(wi, set);
  }

/////////

extern "C" BOOL ALMAPI OWndLayIsItemShape(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemShape(wi);
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetItemIsGroup(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemIsGroup(wi, set);
  }

/////////

extern "C" BOOL ALMAPI OWndLayIsItemGroup(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemGroup(wi);
  }

/////////

extern "C" void ALMAPI OWndLaySetItemVisibleFlag(
  OpWndItemD*             wi,
  BOOL                    fSet)
  {
  WINDOWAPI::SetItemVisible(wi, fSet);
  }

/////////////////////////

extern "C" void ALMAPI  OWndLaySetMappingMode(
  HDC                     hdc,
  int                     mapMode)
  {
  POINT                   wExt, vExt;

  if (mapMode == OWNDLAY_MM_DEPENDANT)
    {
    SetMapMode(hdc, MM_TEXT);
    }
  else if (mapMode == OWNDLAY_MM_INDEPENDANT)
    {
    WINDOW::CalculateMappingModeExtents(mapMode, &wExt, &vExt);

    SetMapMode(hdc, MM_ANISOTROPIC);                     
    
    SetWindowExtEx(hdc, wExt.x, wExt.y, NULL);   /*Must be called before SetViewportExt*/
    SetViewportExtEx(hdc, vExt.x, vExt.y, NULL);
    }
  ADBG_EVAL(
  else
    {
    ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INCORECT_MAPPING_MODE);
    }
    );
  }

/////////////////////////

extern "C" void ALMAPI OWndLaySetWindowHandle(
  AOBJECTID                oiWindow,
  HWND                    hwnd)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);

  wnd->SetWindowHandle(hwnd);

  UnlockWindowData(oiWindow);
  }

//////////////////////////

extern "C" BOOL ALMAPI OWndLaySetViewportOrgEx(
  AOBJECTID                oiWindow,
  int                     nX,
  int                     nY,
  LPPOINT                 lppt)
  {
  WINDOW*                 wnd;
  BOOL                    fRet;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);

  fRet = wnd->m_SetViewportOrgEx(nX, nY, lppt);

  UnlockWindowData(oiWindow);
  return fRet;
  }

//////////////////////////

extern "C" BOOL ALMAPI OWndLayGetViewportOrgEx(
  AOBJECTID                oiWindow,
  LPPOINT                 lppt)
  {
  WINDOW*                 wnd;
  BOOL                    fRet;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);

  fRet = wnd->m_GetViewportOrgEx(lppt);

  UnlockWindowData(oiWindow);
  return fRet;
  }

//////////////////////////

extern "C" BOOL ALMAPI OWndLaySetWindowOrgEx(
  AOBJECTID                oiWindow,
  int                     nX,
  int                     nY,
  LPPOINT                 lppt)
  {
  WINDOW*                 wnd;
  BOOL                    fRet;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);

  fRet = wnd->m_SetWindowOrgEx(nX, nY, lppt);

  UnlockWindowData(oiWindow);
  return fRet;
  }

//////////////////////////

extern "C" BOOL ALMAPI OWndLayGetWindowOrgEx(
  AOBJECTID                oiWindow,
  LPPOINT                 lppt)
  {
  WINDOW*                 wnd;
  BOOL                    fRet;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);

  fRet = wnd->m_GetWindowOrgEx(lppt);

  UnlockWindowData(oiWindow);
  return fRet;
  }

//////////////////////////
// This is a fixed version of the Windows API AdjustWindowRectEx.  The
// scroll bars and menu are now properly considered.

void ALMAPI OWndLayAdjWindowRectEx(
  LPRECT                  lprc,
  DWORD                   dwStyle,
  AOBJECTID                oiMenuBar,
  DWORD                   dwExStyle
  )
  {
  MenuData  menu(oiMenuBar);
  int       nWidth            = lprc->right - lprc->left;
  int       nHeight           = menu.Height(nWidth);

  AdjustWindowRectEx(lprc, dwStyle, FALSE, dwExStyle);
  if(dwStyle & WS_VSCROLL)
    lprc->right += (GetSystemMetrics(SM_CXVSCROLL) - 1);
  if(dwStyle & WS_HSCROLL)
    lprc->bottom += (GetSystemMetrics(SM_CYHSCROLL) - 1);
  if(nHeight>0)
    lprc->top -= (nHeight+1);
  }


