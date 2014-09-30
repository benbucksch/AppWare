
#include "w_includ.h"        
#include "a_almutl.h"

typedef OpWndItemD* (pascal  *ITEMFROMUID)(HWND hwnd, long UID);

extern ITEMFROMUID  g_lpfnItemFromUID;
extern HINSTANCE    g_hinstLayout;

/////////////////////////////////////////////
//  OWndCallEachVisualInstance
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndCallEachVisualInstance(
  AOBJECTID                targetObject,
  CALLVIPROC              proc,
  long                    lParam1,
  long                    lParam2)
  {
  AObjOwnerInfo           ownerInfo;
  OpWndItemD*             wi;

  // Get each visual instance of the item that is in a window object
  ownerInfo.key = AOBJ_GETFIRSTKEY;
  while (AObjGetNextOwner(OTYPE_WINDOW, targetObject, &ownerInfo))
    {
    wi = (OpWndItemD*)ownerInfo.ownerData;
    
    if (! (proc)(wi, lParam1, lParam2) )
      return FALSE;
    }

  return TRUE;
  }


/////////////////////////////////////////////
//  OWndGetHandle
////////////////////////////////////////////
extern "C" HWND ALMAPI OWndGetHandle(
  AOBJECTID                oiWnd)
  {
  HWND                    hwndRet;
  WINDOW*                 pWnd = LockWindowData(oiWnd);

  I_VERIFY_POINTER(pWnd, return NULL);

  if(pWnd)
    {
    hwndRet = pWnd->GetWindowHandle();
    UnlockWindowData(oiWnd);
    }
  else
    hwndRet = NULL;

  return hwndRet;
  }


/////////////////////////////////////////////
//  OWndIsWindowColor
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndIsWindowColor()
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();

  I_VERIFY_POINTER(wr, return TRUE;);

  return wr->IsColor();
  }


/////////////////////////////////////////////
//  OWndSimulateAccelKey
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndSimulateAccelKey(
  AOBJECTID                oiWindow,
  ATYPEID                  type,
  char                    vKey,
  char                    modifier)
  {
  BOOL                    rVal = FALSE;
  WINDOW*                 wnd;

	if (!AAppIsDesignTime() && vKey && modifier) 
		{
	  wnd = LockWindowData(oiWindow);
  	I_VERIFY_POINTER(wnd, return FALSE);

	  rVal = wnd->SendAccelKey(wnd->GetActiveItem(), type, vKey, modifier);
  	UnlockWindowData(oiWindow);
		}

  return rVal;
}


/////////////////////////////////////////////
//  OWndSetCaptureItem
////////////////////////////////////////////
extern "C" OpWndItemD* ALMAPI OWndSetCaptureItem(
  OpWndItemD*             wi)
  {
  WINDOWRUNTIME*          wr;
	OpWndItemD*							rVal = NULL;

	if ( !AAppIsDesignTime() )
		{
  	wr = GetWindowRuntime();

		I_VERIFY_POINTER(wr, return NULL);
		I_VERIFY_POINTER(wi, return NULL);

		rVal = wr->SetCaptureItem(wi);
		}

	return rVal;
  }

/////////////////////////////////////////////
//  OWndReleaseCapture
////////////////////////////////////////////
extern "C" void ALMAPI OWndReleaseCapture()
  {
  WINDOWRUNTIME*          wr;

	if ( !AAppIsDesignTime() )
		{
  	wr = GetWindowRuntime();
	  I_VERIFY_POINTER(wr, return;);
		
	  wr->ReleaseCapture();
		}
  }


/////////////////////////////////////////////
//  OWndGetCaptureItem
////////////////////////////////////////////
extern "C" OpWndItemD* ALMAPI OWndGetCaptureItem()
  {
  WINDOWRUNTIME*          wr;
	OpWndItemD*							rVal = NULL;

	if ( !AAppIsDesignTime() )
		{
  	wr = GetWindowRuntime();
	  I_VERIFY_POINTER(wr, return NULL);

  	rVal = wr->GetCaptureItem();
		}

	return rVal;
  }


/////////////////////////////////////////////
//  OWndSetCursor
////////////////////////////////////////////
extern "C" HCURSOR ALMAPI OWndSetCursor(
  HCURSOR                 hCursor,
  BOOL                    master)
  {
  WINDOWRUNTIME*          wr;
	HCURSOR									rVal = NULL;

	if ( !AAppIsDesignTime() )
		{
	  wr = GetWindowRuntime();
  	I_VERIFY_POINTER(wr, return NULL);

		rVal = wr->SetCursor(hCursor, master);
		}

	return rVal;
  }


////////////////////////////////////////////
//   OWndReleaseCursor
////////////////////////////////////////////
extern "C" HCURSOR ALMAPI OWndReleaseCursor()
  {
  WINDOWRUNTIME*          wr;
	HCURSOR									rVal = NULL;
  
	if ( !AAppIsDesignTime() )
		{
	  wr = GetWindowRuntime();
	  I_VERIFY_POINTER(wr, return NULL);

	  rVal = wr->ReleaseCursor();
		}

	return rVal;
  }


////////////////////////////////////////////
//  OWndGetActiveoiWindow
////////////////////////////////////////////
extern "C" AOBJECTID ALMAPI OWndGetActiveWindowObject()
  {
  WINDOWRUNTIME*          wr;
	AOBJECTID								rVal = 0L;

	if ( !AAppIsDesignTime() )
		{
	  wr = GetWindowRuntime();
	  I_VERIFY_POINTER(wr, return 0);

	  rVal = wr->GetActiveWindowObject();
		}

	return rVal;
  }


////////////////////////////////////////////
//  OWndActivateWindow
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndActivateWindow(
  AOBJECTID                oi)
  {
  WINDOWRUNTIME*          wr;
	BOOL										rVal = FALSE;

	if ( !AAppIsDesignTime() )
		{
	  wr = GetWindowRuntime();
	  I_VERIFY_POINTER(wr, return FALSE);
	  I_VERIFY_TYPEID(oi, OTYPE_WINDOW, return FALSE);

	  rVal = wr->ActivateWindow(oi);
		}

	return rVal;
  }


////////////////////////////////////////////
// OWndSetActiveItem
////////////////////////////////////////////
extern "C" OpWndItemD* ALMAPI OWndSetActiveItem(
  OpWndItemD*             wi)
  {
  WINDOW*                 wnd;
  OpWndItemD*             rVal = NULL;

  I_VERIFY_POINTER(wi, return NULL);

	if ( !AAppIsDesignTime() )
		{
  	wnd = LockWindowData(wi->m_oiParent);
  	I_VERIFY_POINTER(wnd, return NULL);

  	rVal = wnd->SetActiveItem(wi, OWND_ACTIVATED_BY_OTHER);
  	UnlockWindowData(wi->m_oiParent);
		}

  return rVal;
  }



////////////////////////////////////////////
// Returns the active item in the active window
////////////////////////////////////////////
extern "C" OpWndItemD* ALMAPI OWndGetActiveItem()
  {
  WINDOWRUNTIME*          wr;
  WINDOW*                 wnd;
  OpWndItemD*             rVal = NULL;
  AOBJECTID                oiWindow;

	if ( !AAppIsDesignTime() )
		{
  	wr = GetWindowRuntime();
	  I_VERIFY_POINTER(wr, return NULL);

	  if ( (oiWindow = wr->GetActiveWindowObject()) == 0)
	    rVal = NULL; //No active window
	  else
	    {
	    wnd = LockWindowData(oiWindow);
	    I_VERIFY_POINTER(wnd, return NULL);
	    rVal = wnd->GetActiveItem();
	    UnlockWindowData(oiWindow);
	    }
		}

  return rVal;
  }



////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" OpWndItemD* ALMAPI OWndGetNextItem(
  OpWndItemD*             wi,
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  OpWndItemD*             rVal;

  if (wi)
    oiWindow = wi->m_oiParent;

  I_VERIFY_TYPEID(oiWindow, OTYPE_WINDOW, return NULL);

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return NULL);

  rVal = wnd->GetNextItem(wi, FALSE);
  UnlockWindowData(oiWindow);
  return rVal;
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" OpWndItemD* ALMAPI OWndGetPreviousItem(
  OpWndItemD*             wi,
  AOBJECTID                oiWindow)
  {
  WINDOW*                 wnd;
  OpWndItemD*             rVal;

  if (wi)
    oiWindow = wi->m_oiParent;

  I_VERIFY_TYPEID(oiWindow, OTYPE_WINDOW, return NULL);

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return NULL);

  rVal = wnd->GetPrevItem(wi, FALSE);
  UnlockWindowData(oiWindow);
  return rVal;
  }



////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndSetItemAccelKey(
  OpWndItemD*             wi,
  char                    vKey,
  char                    modifier)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  I_VERIFY_POINTER(wi, return FALSE);
  wnd = LockWindowData(wi->m_oiParent);
  I_VERIFY_POINTER(wnd, return FALSE);

  rVal = wnd->SetItemAccelKey(wi, vKey, modifier);
  UnlockWindowData(wi->m_oiParent);
  return rVal;
  }
////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndSetMenuAccelKey(
  AOBJECTID                oiWindow,
  short                   menuID,
  char                    vKey,
  char                    modifier)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  I_VERIFY_TYPEID(oiWindow, OTYPE_WINDOW,  return FALSE);

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);

  rVal = wnd->SetMenuAccelKey(menuID, vKey, modifier);

  UnlockWindowData(oiWindow);
  return rVal;
  }

////////////////////////////////////////////
////////////////////////////////////////////
static void MakeSureItemIsNotActive(
  OpWndItemD*             wi)
  {
  AOBJECTID                oiParent;
  WINDOW*                 wnd;
    
  oiParent = wi->m_oiParent;  //store in temp variable incase wi becomes NULL
  wnd = LockWindowData(oiParent);
  I_VERIFY_POINTER(wnd, return);

  if (wi == wnd->GetActiveItem())
    wnd->SetActiveItem(wnd->GetNextTabstop(wi), OWND_ACTIVATED_BY_TAB);

  UnlockWindowData(oiParent);
  }


////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemVisible(
  OpWndItemD*             wi,
  BOOL                    set)
  {
	if (set)
		OWndShowItem(wi);
	else
		OWndHideItem(wi);
  }


////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndIsItemVisible(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemVisible(wi);
  }



////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemEnabled(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemEnabled(wi, set);

  if (set)
    wi->Enable(TRUE);
  else
    {
    MakeSureItemIsNotActive(wi);
    wi->Enable(FALSE);
    }
  }


////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndIsItemEnabled(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemEnabled(wi);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemActivatable(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemActivatable(wi, set);

  if (!set)
    MakeSureItemIsNotActive(wi);
  }


////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndIsItemActivatable(
  OpWndItemD*             wi)
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();
	BOOL										rVal = FALSE;

	if (wr  &&  wi)
		rVal = WINDOWAPI::IsItemActivatable(wi, wr->IsLayoutTime() ? DONT_CHECK_ENABLE_AND_VISIBLE_BIT:CHECK_ENABLE_AND_VISIBLE_BIT);

  return rVal;
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemTransparent(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemTransparent(wi, set);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndIsItemTransparent(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsItemTransparent(wi);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemUsesCursorKeys(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemUsesCursorKeys(wi, set);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndDoesItemUseCursorKeys(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::DoesItemUseCursorKeys(wi);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemUsesTab(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemUsesTab(wi, set);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndDoesItemUseTab(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::DoesItemUseTab(wi);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemUsesDefaultKey(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemUsesDefaultKey(wi, set);
  }


////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndDoesItemUseDefaultKey(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::DoesItemUseDefaultKey(wi);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetItemUsesCancelKey(
  OpWndItemD*             wi,
  BOOL                    set)
  {
  WINDOWAPI::SetItemUsesCancelKey(wi, set);
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndDoesItemUseCancelKey(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::DoesItemUseCancelKey(wi);
  }



////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetDefaultItem(
  OpWndItemD*             wi)
  {
  WINDOW*                 wnd;

  I_VERIFY_POINTER(wi, return);
  wnd = LockWindowData(wi->m_oiParent);
  I_VERIFY_POINTER(wnd, return);

  wnd->SetDefaultItem(wi);
  UnlockWindowData(wi->m_oiParent);
  return;
  }


////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndIsItemDefault(
  OpWndItemD*             wi)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  I_VERIFY_POINTER(wi, return FALSE);
  wnd = LockWindowData(wi->m_oiParent);
  I_VERIFY_POINTER(wnd, return FALSE);

  rVal = (wnd->GetDefaultItem() == wi);
  UnlockWindowData(wi->m_oiParent);
  return rVal;
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" void ALMAPI OWndSetCancelItem(
  OpWndItemD*             wi)
  {
  WINDOW*                 wnd;

  I_VERIFY_POINTER(wi, return);
  wnd = LockWindowData(wi->m_oiParent);
  I_VERIFY_POINTER(wnd, return);

  wnd->SetCancelItem(wi);
  UnlockWindowData(wi->m_oiParent);
  return;
  }

////////////////////////////////////////////
//
////////////////////////////////////////////
extern "C" BOOL ALMAPI OWndIsItemCancel(
  OpWndItemD*             wi)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  I_VERIFY_POINTER(wi, return FALSE);
  wnd = LockWindowData(wi->m_oiParent);
  I_VERIFY_POINTER(wnd, return FALSE);

  rVal = (wnd->GetCancelItem() == wi);
  UnlockWindowData(wi->m_oiParent);
  return rVal;
  }

////////////////////////////////////////////////
///
///////////////////////

extern "C" BOOL ALMAPI OWndIsLiteItem(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsLiteItem(wi);
  }

////////////

extern "C" BOOL ALMAPI OWndIsHwndItem(
  OpWndItemD*             wi)
  {
  return WINDOWAPI::IsHwndItem(wi);
  }

//////////////////////////////////////////////////

extern "C" DWORD ALMAPI OWndGetItemControlID(
  OpHwndWndItemD*         hwi)
  {
  I_VERIFY_WI_HWND(hwi, ;);

  return WINDOWAPI::GetItemIndex(hwi) + I_ITEM_ID_OFFSET;
  }

/////////////////////////////////////////////////

extern "C" void ALMAPI OWndGetClientRect(
  AOBJECTID                oiWindow,
  LPRECT                  rect)
  {
  WINDOW*                 wnd = LockWindowData(oiWindow);
  HWND                    hwnd;

  I_VERIFY_POINTER(wnd, return);

  if((hwnd = wnd->GetWindowHandle()) != NULL)
    {
    GetClientRect(hwnd, rect);
    wnd->WndDStoLS((LPPOINT)rect, 2);
    }
  else
    {
    *rect = wnd->GetInitialClientRect();

    rect->right  -= rect->left;
    rect->left    = 0;
    rect->bottom -= rect->top;
    rect->top     = 0;
    }

  UnlockWindowData(oiWindow);
  }

/////////////////////////////////////////////////

extern "C" BOOL ALMAPI OWndIsLayoutTime()
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();

  I_VERIFY_POINTER(wr, return TRUE);

  return wr->IsLayoutTime();
  }

/////////////////////////////////////////////////

extern "C" void ALMAPI OWndDPtoLP(
  AOBJECTID                oiWindow,
  LPPOINT                 points,
  short                   numPoints)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->WndDPtoLP(points, numPoints);
  UnlockWindowData(oiWindow);
  }

/////////////////////////////////////////////////

extern "C" void ALMAPI OWndLPtoDP(
  AOBJECTID                oiWindow,
  LPPOINT                 points,
  short                   numPoints)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->WndLPtoDP(points, numPoints);
  UnlockWindowData(oiWindow);
  }

/////////////////////////////////////////////////

extern "C" void ALMAPI OWndDStoLS(
  AOBJECTID                oiWindow,
  LPPOINT                 points,
  short                   numPoints)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->WndDStoLS(points, numPoints);
  UnlockWindowData(oiWindow);
  }

/////////////////////////////////////////////////

extern "C" void ALMAPI OWndLStoDS(
  AOBJECTID                oiWindow,
  LPPOINT                 points,
  short                   numPoints)
  {
  WINDOW*                 wnd;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);
  wnd->WndLStoDS(points, numPoints);
  UnlockWindowData(oiWindow);
  }

//////////////////////////

extern "C" void ALMAPI OWndInvalidateLogicalRect(
  AOBJECTID                oiWindow,
  const LPRECT            rcInvalid,
  BOOL                    bErase)
  {
  WINDOW*                 wnd;
  RECT                    tmpRect;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);

  if (wnd->GetWindowHandle())
    {
    CopyRect(&tmpRect, rcInvalid);
    wnd->WndLPtoDP((LPPOINT)&tmpRect, 2);

    InvalidateRect(wnd->GetWindowHandle(), &tmpRect, bErase);
    }

  UnlockWindowData(oiWindow);
  }

///////////////////////////

void ALMAPI OWndValidateLogicalRect(
  AOBJECTID                oiWindow,
  const LPRECT            rcValid)
  {
  WINDOW*                 wnd;
  RECT                    tmpRect;

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return);

  if (wnd->GetWindowHandle())
    {
    CopyRect(&tmpRect, rcValid);
    wnd->WndLPtoDP((LPPOINT)&tmpRect, 2);

    ValidateRect(wnd->GetWindowHandle(), &tmpRect);
    }

  UnlockWindowData(oiWindow);
  }

//////////////////

BOOL ALMAPI OWndGetPalette(
	AOBJECTID 								oiWindow,
	AMEMBLOCKID							miPalette)
	{
  WINDOW*                 wnd;
	BOOL										rVal;

  I_VERIFY_TYPEID(oiWindow, OTYPE_WINDOW, return FALSE);

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return FALSE);

	rVal = wnd->CopyLogPalette(miPalette);

	UnlockWindowData(oiWindow);

	return rVal;
	}

//////////////////

BOOL ALMAPI OWndReconstructPalette(
	AOBJECTID 								oiWindow)
	{
	I_UNUSED(oiWindow);
	return FALSE;
  //I_VERIFY_TYPEID(oiWindow, OTYPE_WINDOW, return FALSE);
	}

//////////////////

HWND ALMAPI OWndCreateWindowEx(
  OpHwndWndItemD*         hwi,
  DWORD                   dwExStyle,
  LPCSTR                  szClassName,
  LPCSTR                  szWindowName,
  DWORD                   dwStyle,
  HINSTANCE               hinst,
  void *               lpvCreateParams)
  {
  RECT                    tmpRect;

  I_VERIFY_WI_HWND(hwi, return 0);

  CopyRect(&tmpRect, &hwi->m_rcItem);
  OWndLPtoDP(hwi->m_oiParent, (LPPOINT)&tmpRect, 2);

  hwi->m_hwndItem = CreateWindowEx(
    dwExStyle,
    szClassName,
    szWindowName,
    dwStyle,
    tmpRect.left,
    tmpRect.top,
    tmpRect.right - tmpRect.left,
    tmpRect.bottom - tmpRect.top,
    hwi->m_hwndParent,
    (HMENU)OWndGetItemControlID(hwi),
    hinst,
    lpvCreateParams);

  return hwi->m_hwndItem;
  }

//////////////////////////////

BOOL ALMAPI OWndMoveWindow(
  OpHwndWndItemD*         hwi,
  const LPRECT            newRect,
  BOOL                    repaint)
  {
  RECT                    tmpRect;
  BOOL                    rVal;

  I_VERIFY_WI_HWND(hwi, return FALSE);

  CopyRect(&hwi->m_rcItem, newRect);
  CopyRect(&tmpRect, &hwi->m_rcItem);
  OWndLPtoDP(hwi->m_oiParent, (LPPOINT)&tmpRect, 2);

  rVal = MoveWindow(hwi->m_hwndItem, tmpRect.left, tmpRect.top, tmpRect.right-tmpRect.left, tmpRect.bottom-tmpRect.top, repaint); 

  return rVal;
  }


/////////////////////////////////////////////////////////////
/// Menu APIs ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////
AOBJECTID ALMAPI OWndGetActiveMenuBar(void) //Returns the Object id for the menu bar in the active window.
  {
  WINDOWRUNTIME*          wr;
  WINDOW*                 wnd;
  AOBJECTID                rVal = 0L;
  AOBJECTID                oiWindow;
  
  wr = GetWindowRuntime();
  I_VERIFY_POINTER(wr, return 0);
  
  oiWindow = wr->GetActiveWindowObject();
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return 0);

    rVal = wnd->GetMenuObject();

    UnlockWindowData(oiWindow);
    }

  return rVal;
  }

////////////////////

BOOL ALMAPI OWndShowMenuBar(               //  Displays the menu bar in all windows that contain the specified menubar. 
  AOBJECTID                oiMenu)  // If oiMenu is 0, then show the menu bar in the active window.
  {
  AOBJECTID                oiWindow;
  LONG                    cookie = AOBJ_GETFIRSTKEY;
  WINDOWRUNTIME*          wr;          
  WINDOW*                 wnd;

  if (oiMenu)
    {
    while ( (oiWindow = AObjGetNextObject(OTYPE_WINDOW, &cookie)) != 0)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, continue);
      if (wnd->IsWindowOpen() &&  wnd->GetMenuObject() == oiMenu)
        wnd->SetMenuHandle();
      UnlockWindowData(oiWindow);
      }
    }
  else
    {
    wr = GetWindowRuntime();
    I_VERIFY_POINTER(wr, return FALSE);
    oiWindow = wr->GetActiveWindowObject();
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return FALSE);
      if (wnd->IsWindowOpen() &&  wnd->GetMenuObject() )
        wnd->SetMenuHandle();
      UnlockWindowData(oiWindow);
      }
    }

  return TRUE;
  }
                                  
///////////////////////////////

BOOL ALMAPI OWndHideMenuBar(                // Hides the menu bar in all windows that contain the specified menubar.  
  AOBJECTID                oiMenu)   // If oiMenu is 0, then hides the menu bar in the active window.
  {
  AOBJECTID                oiWindow;
  LONG                    cookie = AOBJ_GETFIRSTKEY;
  WINDOWRUNTIME*          wr;
  WINDOW*                 wnd;

  if (oiMenu)
    {
    while ( (oiWindow = AObjGetNextObject(OTYPE_WINDOW, &cookie)) != 0)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, continue);
      if (wnd->IsWindowOpen() &&  wnd->GetMenuObject() == oiMenu)
        wnd->ReleaseMenuHandle();
      UnlockWindowData(oiWindow);
      }
    }
  else
    {
    wr = GetWindowRuntime();
    I_VERIFY_POINTER(wr, return FALSE);
    oiWindow = wr->GetActiveWindowObject();
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return FALSE);
      if (wnd->IsWindowOpen() &&  wnd->GetMenuObject() )
        wnd->ReleaseMenuHandle();
      UnlockWindowData(oiWindow);
      }
    }

  return TRUE;
  }

////////////////////////

BOOL ALMAPI OWndSetMenuBar(               // Sets the menubar for the specified window.  if no window is specified then use the active
  AOBJECTID                oiMenu,   //  window.  Redraw the menu bar if necessary.
  AOBJECTID                oiWindow) 
  {
  WINDOWRUNTIME*          wr;
  BOOL                    rVal=FALSE;
  WINDOW*                 wnd;

  if (!oiWindow)
    {
    wr = GetWindowRuntime();
    I_VERIFY_POINTER(wr, return FALSE);   

    oiWindow = wr->GetActiveWindowObject();
    }

  if(oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return FALSE);  

    wnd->ReleaseMenuHandle();
    wnd->AssignMenuObject(oiMenu);
    wnd->SetMenuHandle(); 

    UnlockWindowData(oiWindow);
    rVal = TRUE;
    } 

  return rVal;
  }
                                    


BOOL ALMAPI OWndRedrawMenuBar(      //Redraws the menu in all windows that contain it
  AOBJECTID                oiMenu)
  {
  BOOL                    rVal=FALSE;
  AOBJECTID                oiWindow;
  WINDOW*                 wnd;
  LONG                    cookie = AOBJ_GETFIRSTKEY;

  if (oiMenu)
    {
    while ( (oiWindow = AObjGetNextObject(OTYPE_WINDOW, &cookie)) != 0)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, continue);
      if (wnd->IsWindowOpen() &&  wnd->GetMenuObject() == oiMenu)
        wnd->RedrawMenu();
      UnlockWindowData(oiWindow);
      }
    rVal=TRUE;
    }

  return rVal;
  }
                                      

/////////////////////////////////////////////////////////

HFONT ALMAPI OWndCreateFontTmm(
  HDC                     hdcTarget, 
  int                     nHeightInTmm, 
  int                     nWidthInTmm, 
  int                     nEscapement, 
  int                     nOrientation,
  int                     fnWeight, 
  DWORD                   fdwItalic, 
  DWORD                   fdwUnderline, 
  DWORD                   fdwStrikeOut, 
  DWORD                   fdwCharSet,
  DWORD                   fdwOutputPrecision, 
  DWORD                   fdwClipPrecision, 
  DWORD                   fdwQuality, 
  DWORD                   fdwPitchAndFamily,
  LPSTR                   lpszFace)
  {
  double                  flFontSize, flFontWidth;
  SIZE                    windowExt, viewportExt;
                            
  GetWindowExtEx(hdcTarget, &windowExt);
  GetViewportExtEx(hdcTarget, &viewportExt);

  flFontSize = (double)nHeightInTmm/*tmm*/ * 
                OWND_INCH_PER_TMM * 
                (double)GetDeviceCaps(hdcTarget, LOGPIXELSY) /*pixels per inch*/ * 
                (double)windowExt.cy /*units*/ /
                (double)viewportExt.cy /*pixel*/;
  flFontSize += flFontSize > 0.0 ? 0.5 : -0.5; //round

  flFontWidth =  (double)nWidthInTmm/*tmm*/ * 
                OWND_INCH_PER_TMM * 
                (double)GetDeviceCaps(hdcTarget, LOGPIXELSY) /*pixels per inch*/ * 
                (double)windowExt.cy /*units*/ /
                (double)viewportExt.cy /*pixel*/;
  flFontWidth += flFontWidth > 0.0 ? 0.5 : -0.5; //round


  return (HFONT)AUtlCreateFont( (int)flFontSize, (int)flFontWidth, 
                  nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet,
                  fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace);

  }


///////////

BOOL ALMAPI OWndDeleteFont(
  HFONT                   hFont)
  {
  AUtlDeleteFont(hFont);
  return TRUE;
  }

//////////////////////////////////////

AOBJECTID ALMAPI OWndWindowObjectFromHwnd(
  HWND                    hwnd)
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();

  I_VERIFY_POINTER(wr, return 0L);

  return wr->GetWindowObjectFromHwnd(hwnd);
  }

//////////////////////////////////////////

BOOL ALMAPI OWndHideItem(
  OpWndItemD*             wi)
  {
  BOOL                    rVal=FALSE;
  WINDOW*                 wnd;

  if (wi  &&  WINDOWAPI::IsItemVisible(wi))
    {
  	WINDOWAPI::SetItemVisible(wi, FALSE);
    MakeSureItemIsNotActive(wi);

    if (!WINDOWAPI::IsItemTransparent(wi))
      {
      wnd = LockWindowData(wi->m_oiParent);
      I_VERIFY_POINTER(wnd, return rVal);
      wnd->SetEraseRegion();
      UnlockWindowData(wi->m_oiParent);
      }
    wi->Hide();
    rVal = TRUE;
    }
  return rVal;
  }

////////////////////////

BOOL ALMAPI OWndShowItem(
  OpWndItemD*             wi)
  {
  BOOL                    rVal=FALSE;
  WINDOW*                 wnd;

  if (wi  &&  !WINDOWAPI::IsItemVisible(wi))
    {
    WINDOWAPI::SetItemVisible(wi, TRUE);
    wnd = LockWindowData(wi->m_oiParent);
    I_VERIFY_POINTER(wnd, return rVal);

    if (!WINDOWAPI::IsItemTransparent(wi))
      wnd->SetEraseRegion();

    wi->Show();

		if (wnd->GetActiveItem() == NULL  &&  WINDOWAPI::IsItemActivatable(wi) )
			wnd->SetActiveItem(wi, OWND_ACTIVATED_BY_OTHER);			

    UnlockWindowData(wi->m_oiParent);

    rVal = TRUE;
    }
  return rVal;
  }

/////////////////////////////////////////////////

BOOL ALMAPI OWndMoveItem(
  OpWndItemD*             wi,
  LPRECT                  rcNew)
  {
  BOOL                    rVal=FALSE;
  WINDOW*                 wnd;
  WINDOWRUNTIME*          wr;

  wr = GetWindowRuntime();
  I_VERIFY_POINTER(wr, return rVal);

  if (wi && !EqualRect(&wi->m_rcItem, rcNew) )
    {
    wnd = LockWindowData(wi->m_oiParent);
    I_VERIFY_POINTER(wnd, return rVal);

    wnd->WndLPtoDP((LPPOINT)rcNew, 2); //Make sure we are on even pixel boundries
    wnd->WndDPtoLP((LPPOINT)rcNew, 2);

    wi->AdjustNewRect(rcNew, OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_L);

    if (!EqualRect(&wi->m_rcItem, rcNew) )
      { //are the rects still unequal?
      wi->Move(rcNew);

      if (!WINDOWAPI::IsItemTransparent(wi)
              &&
          WINDOWAPI::IsItemVisible(wi)
              &&
          !wr->IsLayoutTime())
        {
        wnd->SetEraseRegion();
        }

      rVal = TRUE;
      }
    UnlockWindowData(wi->m_oiParent);
    }
  return rVal;
  }


/////////////////////////////////////////////////

extern "C" BOOL ALMAPI OWndIsItemObject(
  OpWndItemD*             wi)
  {
  return !WINDOWAPI::IsItemGroup(wi) && !WINDOWAPI::IsItemShape(wi);
  }

/////////////////////////

extern "C" LONG ALMAPI OWndUIDFromItem(
  OpWndItemD*             wi)
  {
  LONG                    rVal;

  if(wi)
    rVal = WINDOWAPI::UIDFromItem(wi);
  else
    rVal = 0L;

  return rVal;
  }

////////////////////////////////////////////

extern "C" OpWndItemD* ALMAPI OWndItemFromUID(
  AOBJECTID                oiWnd,
  LONG                    uid)
  {
  OpWndItemD*             rVal;
  WINDOW*                 wnd = LockWindowData(oiWnd);

  I_VERIFY_POINTER(wnd, return NULL);

  if(OWndIsLayoutTime())
    {
    I_ASSERT(g_hinstLayout);
    if(!g_lpfnItemFromUID)
      g_lpfnItemFromUID = (ITEMFROMUID)GetProcAddress(g_hinstLayout, "OWndItemFromUID");
    I_ASSERT(g_lpfnItemFromUID);
    rVal = g_lpfnItemFromUID ? g_lpfnItemFromUID(wnd->GetWindowHandle(), uid) : NULL;
    }
  else
    rVal = wnd->GetItemByUID(uid);

  UnlockWindowData(oiWnd);
  return rVal;
  }
