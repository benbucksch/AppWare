//////////////////////////////////////////
/// This file contains the member functions
///  for the class: WINDOW
//////////////////////////////////////////

#include "w_includ.h"
#include "o_wndgrp.h"

int g_defaultPattern[8]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

///////////////////////////////////

WINDOW::WINDOW(
  OBJECTID                id)
  {
  HDC                     hdc;
  POINT                   screenSize;

  memset(this, 0, sizeof(WINDOW) );
  strcpy(m_szTitle, I_NOTHING_STRING);
  m_style = I_DEFAULT_WINDOW_STYLE;

  AssignMappingMode(OWNDLAY_MM_INDEPENDANT);

  m_nInitialCmdShow = SW_SHOWNORMAL;
  m_ptlScrollSize.x = 3048;
  m_ptlScrollSize.y = 2032;
  m_ptlMinSize.x = 100;
  m_ptlMinSize.y = 100;
  m_ptlMaxSize.x = 3048;
  m_ptlMaxSize.y = 2032;
  m_version = I_THE_VERSION;
  m_oiSelf = id;
  m_nItems = 0;
  m_uidNext = 100;

  m_bwBackground = m_clrBackground = I_WINDOW_BACKGROUND;
  m_bwForeground = m_clrForeground = I_WINDOW_FOREGROUND;
  _fmemcpy(m_aPattern, g_defaultPattern, sizeof(m_aPattern));

  //Figure out the default window size based upon the size of the display
  screenSize.x = GetSystemMetrics(SM_CXFULLSCREEN);
  screenSize.y = GetSystemMetrics(SM_CYFULLSCREEN);
  //Convert to tmm
  hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
  screenSize.x = MulDiv(screenSize.x, 12700, 50 * GetDeviceCaps(hdc, LOGPIXELSX) );
  screenSize.y = MulDiv(screenSize.y, 12700, 50 * GetDeviceCaps(hdc, LOGPIXELSY) );
  DeleteDC(hdc);

  //From the screen size, create a reasonable default initial size
  m_rclInitialClient.left   = (int)(screenSize.x * 0.05);
  m_rclInitialClient.top    = (int)(screenSize.y * 0.10);
  m_rclInitialClient.right  = screenSize.x - 4*m_rclInitialClient.left;
  m_rclInitialClient.bottom = screenSize.y - 2*m_rclInitialClient.top;

	m_uidPaletteOwner = 0L;
	memcpy(m_aPaletteEntries, g_def_palette, sizeof(PALETTEENTRY) * I_NUM_PALETTE_ENTRIES);
	m_wNumPaletteEntries = I_NUM_PALETTE_ENTRIES;
  }


LPVOID WINDOW::operator new(
  size_t                  size,
  OBJECTID                id)
  {
  AObjSetDataCount(id, I_DATA_ITEMS_OFFSET);
  AObjResizeData(id, I_DATA_WINDOW, size);
  return AObjLockData(id, I_DATA_WINDOW);
  }

void WINDOW::operator delete(
  LPVOID                  ptr)
  {
  AObjUnlockData( ((WINDOW*)ptr)->m_oiSelf, I_DATA_WINDOW);
  }




/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void WINDOW::SetTitle(
  LPCSTR                  newTitle)
  {
  lstrcpy(m_szTitle, newTitle);

  if(m_hwnd)
    SetWindowText(m_hwnd, newTitle); //no need to PostObjectChanged
  }

int WINDOW::GetTitle(
  LPSTR                   buffer,
  int                     len)
  {
  if (lstrcmp(m_szTitle, I_NOTHING_STRING))
    _fstrncpy(buffer, m_szTitle, len);
  else
    AObjGetName(m_oiSelf, buffer, len);

  buffer[len-1]=0;
  return lstrlen(buffer);
  }

void WINDOW::ValidateTitle()
  {
  if (!lstrcmp(m_szTitle, I_NOTHING_STRING))
    AObjGetName(m_oiSelf, m_szTitle, I_TITLE_LENGTH);
  }


/////////////////////////////////////////////////////////////////
//  This function will set the active item
//  for the window (m_wiActive), and if the window is active,
//  it will activate the item.
//  It returns the active item for the window (m_wiActive)
/////////////////////////////////////////////////////////////////
OpWndItemD* WINDOW::SetActiveItem(
  OpWndItemD*             wi,
  int                     causedBy,
  BOOL                    bForward) //DEFAULT=TRUE)
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();

  I_VERIFY_POINTER(wr, return wi);

  if (m_oiSelf == wr->GetActiveWindowObject() )
    m_wiActive = ActivateItem(wi, causedBy, bForward);
  else
    m_wiActive = wi;

  return m_wiActive;
  }

////////////////////////////////////////////////////////
//  This function assumes that the
//  item is in the active window.
//  It returns the actual item activated.
//  It stores the active it in m_wiActive.
////////////////////////////////////////////////////////
OpWndItemD* WINDOW::ActivateItem(
  OpWndItemD*             wi,
  int                     causedBy,
  BOOL                    bForward) //defaults to TRUE
  {
  BOOL                    b;
  OpWndItemD              *wiPrevActive, *wiOrg;
  BOOL                    mustKeep=FALSE;

  if (wi  &&  !WINDOWAPI::IsItemActivatable(wi))
    wi = bForward ? GetNextMatchingItem(wi, I_SF_ACTIVATABLE) : GetPrevMatchingItem(wi, I_SF_ACTIVATABLE);

  wiOrg = wi;

  if(wi)
    {
    while (wi  &&  wi->Activate(causedBy, mustKeep) == OWND_ACTIVATION_PASS  &&  !mustKeep)
      {
      wi = GetNextMatchingItemWithinGroup(wi, I_SF_ACTIVATABLE);
      if (wi == wiOrg  ||  wi == m_wiActive)
        mustKeep = TRUE;
      }
    }

  wiPrevActive = m_wiActive;
  m_wiActive = wi; //Store the new activate item

  if (wiPrevActive && wiPrevActive != wi)
    wiPrevActive->Deactivated(); //Deactivate the old

  if (m_wiActive)
    {
    if (WINDOWAPI::IsLiteItem(m_wiActive))  //If it's a lite item set the focus to the window object
      SetFocus(m_hwnd);

    b = (WINDOWAPI::DoesItemUseDefaultKey(m_wiActive) && m_wiActive != m_wiDefault)? FALSE : TRUE;
    if (m_wiDefault)
      m_wiDefault->SetDefaultItem(b);
    m_bDefaultOn = b;

    m_bCancelOn = WINDOWAPI::DoesItemUseCancelKey(m_wiActive) ? FALSE : TRUE;
    }
  else //no active item
     SetFocus(m_hwnd);

  return wi;
  }


void WINDOW::ActivateItemCauseWindowWas()
  {
  if (!m_wiActive)
    m_wiActive = GetNextActivatableItem(NULL);

  if (m_wiActive  &&  !WINDOWAPI::IsItemActivatable(m_wiActive))
    m_wiActive = GetNextTabstop(GetPrevItem( GetNextMatchingItem(m_wiActive, I_SF_GROUPSTOP), TRUE));

  if (m_wiActive)
    m_wiActive = ActivateItem(m_wiActive, OWND_ACTIVATED_BY_WINDOW);
  }

////////////////////////////////////////////////

void WINDOW::DeactivateItemCauseWindowWas()
  {
  if (m_wiActive)
    m_wiActive->Deactivated();
  }


////////////////////////////////////
//
////////////////////////////////////
BOOL WINDOW::SendAccelKey(
  OpWndItemD*             wi,       //The key to start at
  TYPEID                  type,
  char                    vKey,
  char                    modifier)
  {
  OpWndItemD*             wiStopAt;
  LPMENU_ACCEL_INFO       menuAccel;
  unsigned short          uNumMenuAccel, u;
  BOOL                    rVal=FALSE;
  MENUSPARAMBLOCK         menuBlock;

  //Check the table first
  if ( (m_accelKeysUsed[vKey] & modifier) != modifier)
    return FALSE; //not in the table

  if (m_miMenuAccelKeys  &&  m_accelKeysUsed[vKey] & I_MODIFIER_FOR_MENU  &&  !type)
    {
    //see if the key is for a menu object
    uNumMenuAccel = (unsigned short)(AMemGetSize(m_miMenuAccelKeys) / sizeof(MENU_ACCEL_INFO));
    if (uNumMenuAccel)
      {
      menuAccel = (LPMENU_ACCEL_INFO)AMemLock(m_miMenuAccelKeys);
      I_VERIFY_POINTER(menuAccel, return FALSE);
      for(u=0; u<uNumMenuAccel && !rVal; u++)
        {
        if (menuAccel[u].vKey == vKey  &&  menuAccel[u].modifier == modifier)
          {
          menuBlock.objID = 0;
          menuBlock.hMenu = 0;
          menuBlock.itemID = menuAccel[u].itemID;
          AEvtCallMethod(m_oiSelf, 0, OTYPE_MENU, OEVENT_HANDLECOMMAND, (LONG)&menuBlock);
          rVal = TRUE;
          }
        }
      AMemUnlock(m_miMenuAccelKeys);
      }
    }


  if (!rVal)
    {
    if (!wi) //If we started out with a NULL item...
      wiStopAt = GetPrevItem(NULL, TRUE); //.. stop at the last item in the window
    else
      wiStopAt = wi; //stop at the item passed in

    while ( (wi = GetNextItem(wi, TRUE)) != NULL  &&  !rVal)
      {
      if ( (!type  ||  type == wi->Type() )  &&
            (vKey == wi->m_accelVKey) &&
            (modifier == wi->m_accelModifier) &&
            WINDOWAPI::IsItemEnabled(wi) )
        {
        wi->AccelKeyHit();
        rVal = TRUE;
        }

      if (wi == wiStopAt)
        break; //Got to the end.  Not found
      }

    }

  return rVal;
  }


////////////////////////////////////////
//
////////////////////////////////////////
void WINDOW::RebuildAccelTable(
  char                    vKey)
  {
  OpWndItemD*             wiTmp=NULL;
  LPMENU_ACCEL_INFO       menuAccel;
  unsigned short          uNumMenuAccel, u;

  //fill in for the items
  while( (wiTmp = GetNextItem(wiTmp, FALSE)) != NULL)
    {
    if (wiTmp->m_accelVKey == vKey)
      m_accelKeysUsed[vKey] |= wiTmp->m_accelModifier;
    }

  //fill in for the menus
  if (m_miMenuAccelKeys)
    {
    uNumMenuAccel = (unsigned short)(AMemGetSize(m_miMenuAccelKeys) / sizeof(MENU_ACCEL_INFO));
    if (uNumMenuAccel)
      {
      menuAccel = (LPMENU_ACCEL_INFO)AMemLock(m_miMenuAccelKeys);
      I_VERIFY_POINTER(menuAccel, return);
      for(u=0; u<uNumMenuAccel; u++)
        {
        if (menuAccel[u].vKey == vKey)
          m_accelKeysUsed[vKey] |= menuAccel[u].modifier | I_MODIFIER_FOR_MENU;
        }
      AMemUnlock(m_miMenuAccelKeys);
      }
    }

  }

////////////////////////////////////////

BOOL WINDOW::SetItemAccelKey(
  OpWndItemD*             wi,
  char                    vKey,
  char                    modifier)
  {
  char                    vKeyToRebuild=0;

  if (wi->m_accelVKey)
    {
    vKeyToRebuild = wi->m_accelVKey;
    m_accelKeysUsed[wi->m_accelVKey] = 0; //The item already has an accel key -- blank it out from the table
    }

  //// Update the table entry and assign the new values
  m_accelKeysUsed[vKey] |= modifier;
  wi->m_accelVKey = vKey;
  wi->m_accelModifier = modifier;

  if (vKeyToRebuild)
    RebuildAccelTable(vKeyToRebuild);

  return TRUE;
  }

//////////////////////////////////////////////
BOOL WINDOW::SetMenuAccelKey(
  short                   itemID,
  char                    vKey,
  char                    modifier)
  {
  unsigned short          uNumMenuAccel, u;
  LPMENU_ACCEL_INFO       menuAccel;
  char                    vKeyToRebuild=0;

  if (!m_miMenuAccelKeys)
    m_miMenuAccelKeys = AMemAllocate(0);

  uNumMenuAccel = (unsigned short)(AMemGetSize(m_miMenuAccelKeys) / sizeof(MENU_ACCEL_INFO));
  AMemSetSize(m_miMenuAccelKeys,  (uNumMenuAccel+1)*sizeof(MENU_ACCEL_INFO));

  menuAccel = (LPMENU_ACCEL_INFO)AMemLock(m_miMenuAccelKeys);
  I_VERIFY_POINTER(menuAccel, return FALSE);

  for(u=0; u<uNumMenuAccel; u++)
    {
    if (menuAccel[u].itemID == itemID)
      {
      vKeyToRebuild = menuAccel[u].vKey;
      m_accelKeysUsed[menuAccel[u].vKey] = 0; //The item already has an accel key -- blank it out from the table
      break;
      }
    }

  menuAccel[u].itemID   = itemID;
  menuAccel[u].vKey     = vKey;
  menuAccel[u].modifier = modifier;
  m_accelKeysUsed[menuAccel[u].vKey] |= modifier | I_MODIFIER_FOR_MENU;

  if (vKeyToRebuild)
    RebuildAccelTable(vKeyToRebuild);
  else //we added a new entry
    uNumMenuAccel++;

  AMemUnlock(m_miMenuAccelKeys);
  AMemSetSize(m_miMenuAccelKeys, sizeof(MENU_ACCEL_INFO) * uNumMenuAccel);
  return TRUE;
  }

////////////////////////////////////////
//
////////////////////////////////////////
OpHwndWndItemD* WINDOW::GetItemByControlID(
  int                     controlID)
  {
  int                     index = controlID - I_ITEM_ID_OFFSET;

  MakeSureTheVisualInstancesAreCreated();

  if (index >=0  &&  index < m_nItems)
    {
    I_VERIFY_WI_HWND(m_aptrWndItem[index], return NULL);
    return (OpHwndWndItemD*)m_aptrWndItem[index];
    }
  else
    {
    ADBG_EVAL( ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WI_IDX_RANGE, "WINDOW::GetItemByControlID"));
    return NULL;
    }
  }

//////////////////////////////////////////////////
//////////////////////////////////////////////////

OpWndItemD* WINDOW::GetItemByObjectID(
  OBJECTID                id)//defaut=NULL;
  {
  OpWndItemD*             wi;

  MakeSureTheVisualInstancesAreCreated();

  wi = NULL;

  while( (wi=GetNextItem(wi, FALSE)) != NULL)
    {
    if (wi->Object() == id)
      return wi;
    }

  return NULL;
  }


/////////////////////////////////////

OpWndItemD* WINDOW::GetItemByUID(
  LONG                    uid)
  {
  OpWndItemD*             wi;

  MakeSureTheVisualInstancesAreCreated();

  wi=NULL;
  while ( (wi=GetNextItem(wi, FALSE)) != NULL)
    {
    if (wi->m_uidItem == uid)
      return wi;
    }

  return NULL;
  }

////////////////////////////////////
////////////////////////////////////

OpWndItemD* WINDOW::GetNextItem(
  OpWndItemD*             wi,
  BOOL                    wrap)
  {
  int                     nextItemIdx;

  MakeSureTheVisualInstancesAreCreated();

  if(m_nItems==0)
    return NULL;

  if (!wi) // return the first item in the window
    return m_aptrWndItem[0];

  I_ASSERT_WI_IN_WND(wi, this, return NULL);

  nextItemIdx = wi->m_index+1;
  if (nextItemIdx >= m_nItems)
    {
    if (wrap)
      return m_aptrWndItem[0];
    else
      return NULL;
    }

  return m_aptrWndItem[nextItemIdx];
  }

////////////////////////////////////////

OpWndItemD* WINDOW::GetPrevItem(
  OpWndItemD*             wi,
  BOOL                    wrap)
  {
  int                     prevItemIdx;

  MakeSureTheVisualInstancesAreCreated();

  if(m_nItems==0)
    return NULL;

  if (!wi) //return the last item in the window
    return m_aptrWndItem[m_nItems-1];

  I_ASSERT_WI_IN_WND(wi, this, return NULL);

  prevItemIdx = wi->m_index-1;
  if (prevItemIdx < 0)
    {
    if (wrap)
      return m_aptrWndItem[m_nItems-1];
    else
      return NULL;
    }

  return m_aptrWndItem[prevItemIdx];
  }

/////////////////////////////////////
/////////////////////////////////////

OpWndItemD* WINDOW::GetNextItemWithinGroup(
  OpWndItemD*             wi) //wraps within group
  {
  OpWndItemD*             wiStartOfGroup;
  OpWndItemD*             wiEndOfGroup;

  MakeSureTheVisualInstancesAreCreated();

  if(m_nItems==0)
    return NULL;

  if (!wi) // return the first item in the window
    return m_aptrWndItem[0];

  wiStartOfGroup = GetPrevMatchingItem( GetNextItem(wi, TRUE), I_SF_GROUPSTOP);
  wiEndOfGroup = GetPrevItem( GetNextMatchingItem(wi, I_SF_GROUPSTOP), TRUE);

  if (wi == wiEndOfGroup)
    return wiStartOfGroup;
  else
    return GetNextItem(wi, TRUE);
  }

/////////////////////////////////////

OpWndItemD* WINDOW::GetPrevItemWithinGroup(
  OpWndItemD*             wi) //wraps within group
  {
  OpWndItemD*             wiStartOfGroup;
  OpWndItemD*             wiEndOfGroup;

  MakeSureTheVisualInstancesAreCreated();

  if (!wi) //return the last item in the window
    return m_aptrWndItem[m_nItems-1];

  wiStartOfGroup = GetPrevMatchingItem( GetNextItem(wi, TRUE), I_SF_GROUPSTOP);
  wiEndOfGroup = GetPrevItem( GetNextMatchingItem(wi, I_SF_GROUPSTOP), TRUE);

  if (wi == wiStartOfGroup)
    return wiEndOfGroup;
  else
    return GetPrevItem(wi, TRUE);
  }

/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////

OpWndItemD* WINDOW::GetNextMatchingItem(
  OpWndItemD*             wi,
  LONG                    statusBits)
  {
  OpWndItemD*             wiStopAt;

  if (!wi) //If we started out with a NULL item...
    wiStopAt = GetPrevItem(NULL, TRUE); //.. stop at the last item in the window
  else
    wiStopAt = wi; //stop at the item passed in


  wi = GetNextItem(wi, TRUE);

  while(wi && wi != wiStopAt)
    {
    if ( (wi->m_status & statusBits) == statusBits)
      return wi; //found a item that matches

    wi = GetNextItem(wi, TRUE);
    }

  // If we are here, we didn't a match
  // before we got to the stoping point.

  if (wi  &&  (wi->m_status & statusBits) == statusBits)
    return wi; //The item we started with
  else
    return NULL; //Couldn't find an match
  }

/////////////////////////////////////

OpWndItemD* WINDOW::GetPrevMatchingItem(
  OpWndItemD*             wi,
  LONG                    statusBits)
  {
  OpWndItemD*             wiStopAt;

  if (!wi) //If we started out with a NULL item...
    wiStopAt = GetNextItem(NULL, TRUE); //.. stop at the first item in the window
  else
    wiStopAt = wi; //stop at the item passed in

  wi = GetPrevItem(wi, TRUE);

  while(wi && wi != wiStopAt)
    {
    if ( (wi->m_status & statusBits) == statusBits)
      return wi; //found a item that matches

    wi = GetPrevItem(wi, TRUE);
    }

  // If we are here, we didn't a match
  // before we got to the stoping point.

  if (wi  &&  (wi->m_status & statusBits) == statusBits)
    return wi; //The item we started with
  else
    return NULL; //Couldn't find a match
  }

///////////////////////////////////////////////////
///////////////////////////////////////////////////

OpWndItemD* WINDOW::GetNextMatchingItemWithinGroup(
  OpWndItemD*             wi,
  LONG                    statusBits)
  {
  OpWndItemD*             wiStopAt;

  if (!wi) //If we started out with a NULL item...
    wiStopAt = GetPrevItemWithinGroup(NULL); //.. stop at the last item in the window
  else
    wiStopAt = wi; //stop at the item passed in


  wi = GetNextItemWithinGroup(wi);

  while(wi && wi != wiStopAt)
    {
    if ( (wi->m_status & statusBits) == statusBits)
      return wi; //found a item that matches

    wi = GetNextItemWithinGroup(wi);
    }

  // If we are here, we didn't a match
  // before we got to the stoping point.

  if (wi  &&  (wi->m_status & statusBits) == statusBits)
    return wi; //The item we started with
  else
    return NULL; //Couldn't find an match
  }

/////////////////////////////////////

OpWndItemD* WINDOW::GetPrevMatchingItemWithinGroup(
  OpWndItemD*             wi,
  LONG                    statusBits)
  {
  OpWndItemD*             wiStopAt;

  if (!wi) //If we started out with a NULL item...
    wiStopAt = GetNextItemWithinGroup(NULL); //.. stop at the first item in the window
  else
    wiStopAt = wi; //stop at the item passed in

  wi = GetPrevItemWithinGroup(wi);

  while(wi && wi != wiStopAt)
    {
    if ( (wi->m_status & statusBits) == statusBits)
      return wi; //found a item that matches

    wi = GetPrevItemWithinGroup(wi);
    }

  // If we are here, we didn't a match
  // before we got to the stoping point.

  if (wi  &&  (wi->m_status & statusBits) == statusBits)
    return wi; //The item we started with
  else
    return NULL; //Couldn't find a match
  }

/////////////////////////////////////
////////////////////////////////////
////////////////////////////////////
void WINDOW::BringToTop()
  {

  I_VERIFY_WINDOW(m_hwnd, return);

	SetActiveWindow(m_hwnd);
  }

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////

void WINDOW::Startup(
  OBJECTID                oiWindow)
  {
  DWORD                   dwStyle;
  int                     nHeight, nWidth;
  POINT                   ptScrollSize;

  m_oiSelf = oiWindow;

  m_hwnd = 0;
  _fmemset(m_accelKeysUsed, 0, 256);

  m_bVisualInstancesCreated = FALSE;
  m_miWindItems = 0;
  m_miMenuAccelKeys = 0;
  m_nItems = 0;
  m_aptrWndItem = NULL;
  m_hbrushBackground = 0;

  m_wiActive = m_wiDefault = m_wiCancel = NULL;
  m_bDefaultOn = m_bCancelOn = FALSE;
  m_hrgnErase = NULL;

  SetWindowIsClosing(FALSE);
  SetWindowIsOpening(FALSE);
  SetWindowIsIconic(FALSE);

  //Figure out the window rect based upon the desired client rect
  CopyRect(&m_rclCurrentClient, &m_rclInitialClient);
  CopyRect(&m_rcdCurrentWindow, &m_rclInitialClient);
  WndLStoDS( (LPPOINT)&m_rcdCurrentWindow, 2); //Convert to device coords

  dwStyle = GetNativeStyle();

  // adjust the window for the scroll bars
  ptScrollSize = m_ptlScrollSize;
  WndLStoDS(&ptScrollSize, 1);
  nHeight = m_rcdCurrentWindow.bottom - m_rcdCurrentWindow.top;
  nWidth  = m_rcdCurrentWindow.right - m_rcdCurrentWindow.left;
  if((dwStyle & WS_VSCROLL) && (nHeight >= ptScrollSize.y))
    m_rcdCurrentWindow.right -= (GetSystemMetrics(SM_CXVSCROLL) - 1);
  if((dwStyle & WS_HSCROLL) && (nWidth >= ptScrollSize.x))
    m_rcdCurrentWindow.bottom -= (GetSystemMetrics(SM_CYHSCROLL) - 1);

  // get the new current window size
  OWndLayAdjWindowRectEx(&m_rcdCurrentWindow, dwStyle, HasMenuBar() ? m_oiMenu : NULL, GetNativeExStyle());

  if (IsInitVisible())
    AEvtPostStandard(m_oiSelf, OWND_EVT_OPENWINDOWOBJECT, 0L, AEVT_ATTAIL);
  }

//////////////////////////////////////////////////
BOOL WINDOW::MakeSureTheVisualInstancesAreCreated()
  {
  OpWndItemD*             wi;
  RECT                    rcItem;

  if (m_bVisualInstancesCreated)
    return FALSE;

  m_miWindItems = AMemAllocate(0L);
  m_nItems = CreateExistingItems(m_miWindItems);
  m_aptrWndItem = (OpWndItemD**)AMemLock(m_miWindItems);

  m_bVisualInstancesCreated = TRUE;

  wi = NULL;
  while( (wi=GetNextItem(wi, FALSE)) != NULL)
    {
    // notify groups that they have been created
    if(WINDOWAPI::IsItemGroup(wi))
      ((OpGroupWndItem*)wi)->Created();

    if(OWndIsItemObject(wi)) // Register Ownership if it is a real object
      AObjRegisterOwner(m_oiSelf, wi->Object(), (LONG)wi);

    CopyRect(&rcItem, &wi->m_rcItem);
    wi->AdjustNewRect(&rcItem, OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_L);
    CopyRect(&wi->m_rcItem, &rcItem);
    }

  return TRUE;
  }

//////////////////////////////////////////////////
//////////////////////////////////////////////////
void WINDOW::Shutdown()
  {

  if (IsWindowOpen())
    Close();

  if (m_miMenuAccelKeys)
    AMemFree(m_miMenuAccelKeys);

  if (AreTheVisualInstancesCreated())
    {
    PutItemsIntoDatas(m_miWindItems, m_nItems);
    AMemUnlock(m_miWindItems);
    DestroyAllItems(m_miWindItems, m_nItems);
    AMemFree(m_miWindItems);
    }
  }

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

long WINDOW::GetNativeStyle()
  {
  LONG      							s = WS_BORDER | WS_POPUP;

  if (m_style & I_WF_TITLEBAR)      s |= WS_CAPTION;
  if (m_style & I_WF_SYSMENU)       s |= WS_SYSMENU;
  if (m_style & I_WF_MAXBUTTON)     s |= WS_MAXIMIZEBOX;
  if (m_style & I_WF_MINBUTTON)     s |= WS_MINIMIZEBOX;
  if (m_style & I_WF_HORZSCROLL)    s |= WS_HSCROLL;
  if (m_style & I_WF_VERTSCROLL)    s |= WS_VSCROLL;
  if (m_style & I_WF_RESIZE)        s |= WS_THICKFRAME;

  if (m_style & I_WF_DIALOGFRAME)
    s &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);

  return s;
  }

////////////////////////////////////////////////

long WINDOW::GetNativeExStyle()
  {
  LONG      s = 0;

  if (m_style & I_WF_DIALOGFRAME)   s |= WS_EX_DLGMODALFRAME;
  if (m_style & I_WF_POPUP)         s |= WS_EX_TOPMOST;

  return s;
  }

////////////////////////////////////////////////

void WINDOW::RemoveTitleBar()
  {
  LONG  s = GetWindowLong(m_hwnd, GWL_STYLE);
  //This whole function would be unnessary in MicroSoft
  //wasn't brain dead

  s &= ~WS_DLGFRAME;
  SetWindowLong(m_hwnd, GWL_STYLE, s);
  }

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void WINDOW::CreateTheWindow()
  {
  HMENU                   hSysmenu;
  HDC                     hdc;
  WINDOWRUNTIME*          wr = GetWindowRuntime();
	HWND										hwndParent=NULL;

	I_VERIFY_POINTER(wr, return);

	if (IsModal())
		hwndParent = wr->GetActiveWindowObjectHandle();

  m_hwnd = CreateWindowEx(
          GetNativeExStyle(),
          g_windowObjectClassName,
          m_szTitle,
          GetNativeStyle(),
          m_rcdCurrentWindow.left,
          m_rcdCurrentWindow.top,
          m_rcdCurrentWindow.right - m_rcdCurrentWindow.left,
          m_rcdCurrentWindow.bottom - m_rcdCurrentWindow.top,
          hwndParent,
          NULL,
          g_hInstance,
          (LPVOID)m_oiSelf);

  if (!HasTitleBar()) //MicroSoft SUCKS!!
    RemoveTitleBar();

  if (HasSysMenu())
    {
    hSysmenu = GetSystemMenu(m_hwnd, FALSE);

    if (!HasMaxButton() )
      EnableMenuItem(hSysmenu, SC_MAXIMIZE, MF_GRAYED|MF_BYCOMMAND);
    if (!HasMinButton() )
      EnableMenuItem(hSysmenu, SC_MINIMIZE, MF_GRAYED|MF_BYCOMMAND);
    if (!HasResizeFrame())
      EnableMenuItem(hSysmenu, SC_SIZE, MF_GRAYED|MF_BYCOMMAND);
    }

  if (HasMenuBar())
    SetMenuHandle();

  hdc = GetDC(m_hwnd);
  this->SetMappingMode(hdc);
  m_SetWindowOrgEx(0, 0, NULL);
  m_SetViewportOrgEx(0, 0, NULL);
  ValidateScrollBars();
  ReleaseDC(m_hwnd, hdc);
  }


//////////////////////////

void WINDOW::CalculateMappingModeExtents(
  int                     mode,
  LPPOINT                 windowExt,
  LPPOINT                 viewportExt)
  {
  HDC                     hdcDisplay;


  if (mode == OWNDLAY_MM_DEPENDANT)
    viewportExt->x = viewportExt->y = windowExt->x = windowExt->y = 1;
  else if (mode == OWNDLAY_MM_INDEPENDANT)
    {
    hdcDisplay = CreateDC("DISPLAY", NULL, NULL, NULL);

    windowExt->x = 12700;
    windowExt->y = 12700;

    I_ASSERT_LONG_RANGE( 50L * GetDeviceCaps(hdcDisplay, LOGPIXELSX), ;);
    I_ASSERT_LONG_RANGE( 50L * GetDeviceCaps(hdcDisplay, LOGPIXELSY), ;);

    viewportExt->x = 50 * GetDeviceCaps(hdcDisplay, LOGPIXELSX);
    viewportExt->y = 50 * GetDeviceCaps(hdcDisplay, LOGPIXELSY);

    DeleteDC(hdcDisplay);
    }
  else
    {
    ADBG_EVAL(ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INCORECT_MAPPING_MODE));
    }
  }


void WINDOW::AssignMappingMode(
  int                     newMode)
  {
  m_mappingMode = newMode;

  CalculateMappingModeExtents(m_mappingMode, &m_ptWindowExt, &m_ptViewportExt);
  }

/////////////////////

void WINDOW::SetMappingMode(
  HDC                     hdc)
  {

  if (m_mappingMode == OWNDLAY_MM_DEPENDANT)
    SetMapMode(hdc, MM_TEXT);
  else if (m_mappingMode == OWNDLAY_MM_INDEPENDANT)
    {
    SetMapMode(hdc, MM_ANISOTROPIC);
    SetWindowExt(hdc, m_ptWindowExt.x, m_ptWindowExt.y);   /*Must be called before SetViewportExt*/
    SetViewportExt(hdc, m_ptViewportExt.x, m_ptViewportExt.y);
    }
  else
    {
    ADBG_EVAL( ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INCORECT_MAPPING_MODE));
    }

  }

/////////////

void WINDOW::WndDPtoLP(
  LPPOINT                 pt,
  int                     numPoints)
  {
  int                     i;

  for(i=0; i<numPoints; i++,pt++)
    {
    pt->x = MulDiv(pt->x - m_ptViewportOrg.x, m_ptWindowExt.x, m_ptViewportExt.x) + m_ptWindowOrg.x;
    pt->y = MulDiv(pt->y - m_ptViewportOrg.y, m_ptWindowExt.y, m_ptViewportExt.y) + m_ptWindowOrg.y;
    }
  }


/////////////

void WINDOW::WndLPtoDP(
  LPPOINT                 pt,
  int                     numPoints)
  {
  int                     i;

  for(i=0; i<numPoints; i++,pt++)
    {
    pt->x = MulDiv(pt->x - m_ptWindowOrg.x, m_ptViewportExt.x, m_ptWindowExt.x) + m_ptViewportOrg.x;
    pt->y = MulDiv(pt->y - m_ptWindowOrg.y, m_ptViewportExt.y, m_ptWindowExt.y) + m_ptViewportOrg.y;
    }
  }


/////////////

void WINDOW::WndDStoLS(
  LPPOINT                 pt,
  int                     numPoints)
  {
  int                     i;

  for(i=0; i<numPoints; i++,pt++)
    {
    pt->x = MulDiv(pt->x, m_ptWindowExt.x, m_ptViewportExt.x) + m_ptWindowOrg.x;
    pt->y = MulDiv(pt->y, m_ptWindowExt.y, m_ptViewportExt.y) + m_ptWindowOrg.y;
    }
  }


/////////////

void WINDOW::WndLStoDS(
  LPPOINT                 pt,
  int                     numPoints)
  {
  int                     i;

  for(i=0; i<numPoints; i++,pt++)
    {
    pt->x = MulDiv(pt->x - m_ptWindowOrg.x, m_ptViewportExt.x, m_ptWindowExt.x);
    pt->y = MulDiv(pt->y - m_ptWindowOrg.y, m_ptViewportExt.y, m_ptWindowExt.y);
    }
  }


////////////////////////

void WINDOW::CreateBackgroundBrushAndColors()
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();
  HBITMAP                 hBmp;

  FreeBackgroundBrushAndColors();

  if (!wr || wr->IsColor())
    {
    m_foreground = (m_clrForeground == I_WINDOW_FOREGROUND ? GetSysColor(COLOR_WINDOWTEXT) : m_clrForeground);
    m_background = (m_clrBackground == I_WINDOW_BACKGROUND ?  GetSysColor(COLOR_WINDOW) : m_clrBackground);
    }
  else
    {
    m_foreground = (m_bwForeground == I_WINDOW_FOREGROUND ? GetSysColor(COLOR_WINDOWTEXT) : m_bwForeground);
    m_background = (m_bwBackground == I_WINDOW_BACKGROUND ? GetSysColor(COLOR_WINDOW) : m_bwBackground);
    }

  hBmp = CreateBitmap(8, 8, 1, 1, m_aPattern);
  m_hbrushBackground = CreatePatternBrush(hBmp);
  DeleteObject(hBmp);
  }

//////////

void WINDOW::FreeBackgroundBrushAndColors()
  {
  if (m_hbrushBackground)
    DeleteObject(m_hbrushBackground);
  m_hbrushBackground = 0;
  }

//////////

void WINDOW::Open()
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();
  OpWndItemD*             wi;

	I_VERIFY_POINTER(wr, return);

  if ( IsModal() || !wr->GetModalWindowObject() || wr->GetModalWindowObject() == m_oiSelf)
    {

    if (!IsWindowOpen() )
      {
      SetWindowIsOpening(TRUE);

      CreateTheWindow(); //Creates it not visible

      CreatePaletteHandle();
      CreateBackgroundBrushAndColors(); //This needs to be done as a result of WM_WININICHANGED too!

      wi = NULL;
      while( (wi = GetNextItem(wi, FALSE)) != NULL)
        {
        wi->m_hwndParent = m_hwnd;
        wi->ParentOpened();
        if (WINDOWAPI::IsItemVisible(wi))
          wi->Show();
        }

      ShowWindow(m_hwnd, SW_SHOW);

      AEvtPostSignalAtTail(m_oiSelf, I_SIG_OPENED);
      SetWindowIsOpening(FALSE);
      }

    else  //the window is open already
      {
      ShowWindow(m_hwnd, SW_RESTORE);
      SetActiveWindow(m_hwnd);
      }

    } //IsModal() || .......

  }

//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
void WINDOW::Close()
  {
  OpWndItemD*             wi;

  I_VERIFY_WINDOW(m_hwnd, return);

  SetWindowIsClosing(TRUE);

  Hide();

  wi = NULL;
  while ( (wi = GetNextItem(wi, FALSE)) != NULL)
    {
    wi->ParentClosing();
    wi->m_hwndParent = 0;
    }

  if (HasMenuBar())
    ReleaseMenuHandle();

  RemoveEraseRegion();
  FreeBackgroundBrushAndColors();
  DeletePaletteHandle();

  DestroyWindow(m_hwnd);
  m_hwnd = 0;

  AEvtPostSignalAtTail(m_oiSelf, I_SIG_CLOSED);

  if (QuitsOnClose())
    AAppPostShutdown(FALSE);

  SetWindowIsClosing(FALSE);

  }

/////////////////////////////////////////////////////

void WINDOW::Hide()
  {
  if (m_hwnd)
    ShowWindow(m_hwnd, SW_HIDE);
  }


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

//////////////////////////////////////////
OpWndItemD* WINDOW::ItemFromLogicalPoint(
  POINT                   pt)       //pt is in client logical coords
  {
  OpWndItemD*             wi=NULL;

  while( (wi=GetPrevItem(wi, FALSE)) != NULL)
    if (WINDOWAPI::IsItemVisible(wi)  &&  wi->PointInItem(&pt) )
      return wi;

  return NULL;
  }

////////////////////////

OpWndItemD* WINDOW::ItemFromScreenPoint(
  POINT                   pt)  //pt is in screen coords
  {
  HDC                     hdc;

  ScreenToClient(m_hwnd, &pt);
  hdc = GetDC(m_hwnd);
  DPtoLP(hdc, &pt, 1);
  ReleaseDC(m_hwnd, hdc);

  return ItemFromLogicalPoint(pt);
  }

////////////////////////


BOOL WINDOW::Erase(
  HDC                     hdc)
  {
  RECT                    rc;
  POINT                   ptOrigin;
  COLORREF                oldFore;
  COLORREF                oldBack;
  RECT                    tmpClientRect;

  GetClientRect(m_hwnd, &tmpClientRect);
  WndDStoLS((LPPOINT)&tmpClientRect, 2);

  if (!EqualRect(&m_rclCurrentClient, &tmpClientRect))
    SetEraseRegion();

  if(m_hrgnErase)
    {
    // get the update rect BEFORE doing the other stuff or this will NOT work!
    GetUpdateRect(m_hwnd, &rc, FALSE);

    // convert the top left corner of the window to screen device points.
    ptOrigin.x = ptOrigin.y = 0;
    WndLPtoDP(&ptOrigin, 1);
    MapWindowPoints(m_hwnd, HWND_DESKTOP, &ptOrigin, 1);
    // set the brush origin as the (0, 0) position -- which may not be visible
    UnrealizeObject(m_hbrushBackground);
    SetBrushOrg(hdc, ptOrigin.x, ptOrigin.y);

    oldFore = SetTextColor(hdc, m_foreground);  //bitmap brushes use the fore and back color
    oldBack = SetBkColor(hdc, m_background);

    FillRgn(hdc, m_hrgnErase, m_hbrushBackground);

    SetTextColor(hdc, oldFore);
    SetBkColor(hdc, oldBack);
    return TRUE;
    }
  else  //no erase region handle
    return FALSE; //this will cause the default action to take place -- OS will just fill rect
  }

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////

void WINDOW::Paint()
  {
  OpWndItemD*         wi;
  HRGN                updateRegion;
  PAINTSTRUCT         ps;
  BOOL                MicrosoftSucksErrorFlag;
  RECT                tmpRect;
  HPALETTE            hPalOld;

  updateRegion = CreateRectRgn(0,0,0,0);

  if ( GetUpdateRgn(m_hwnd, updateRegion, TRUE) == SIMPLEREGION)
    MicrosoftSucksErrorFlag = TRUE;

  BeginPaint(m_hwnd, &ps);
  if (MicrosoftSucksErrorFlag)
    { //Thanks ASSHOLES at Microsoft....
    CopyRect(&tmpRect, &ps.rcPaint);
    WndLPtoDP((LPPOINT)&tmpRect, 2);
    SetRectRgn(updateRegion, tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom);
    }

  if (m_hPalette)
    {
    hPalOld = SelectPalette(ps.hdc, m_hPalette, TRUE);
    RealizePalette(ps.hdc);
    }

  // Calculate the invalid items and set them to need to be drawn
  wi = NULL;
  while ( (wi=GetPrevItem(wi, FALSE)) != NULL)
    {
    if ( WINDOWAPI::IsLiteItem(wi)  &&  WINDOWAPI::IsItemVisible(wi)  &&  wi->ItemInRegion(updateRegion, TRUE))
      {
      WINDOWAPI::SetItemNeedsToBeDrawn(wi, TRUE);

      if (!WINDOWAPI::IsItemTransparent(wi))
        wi->RemoveItemFromRegion(updateRegion, TRUE);
      }
    }


  // Now, draw the items that need to be drawn
  wi = NULL;
  while ( (wi=GetNextItem(wi, FALSE)) != NULL)
    {
    if ( WINDOWAPI::IsLiteItem(wi))
      {
      if (WINDOWAPI::IsItemVisible(wi)  &&  WINDOWAPI::DoesItemNeedToBeDrawn(wi))
        ((OpLiteWndItemD*)wi)->Draw(ps.hdc, &ps.rcPaint);
      WINDOWAPI::SetItemNeedsToBeDrawn(wi, FALSE);
      }
    else
      UpdateWindow(((OpHwndWndItemD*)wi)->m_hwndItem);

    }

  DeleteObject(updateRegion);

  if (m_hPalette)
    {
    SelectPalette(ps.hdc, hPalOld, TRUE);
    RealizePalette(ps.hdc);
    }

  EndPaint(m_hwnd, &ps);
  }

/////////////////////////////////////////////////////
void WINDOW::PaintIcon()
  {
  PAINTSTRUCT             ps;
  int                     x, y;
  RECT                    rcIconicWindow;

  BeginPaint(m_hwnd, &ps);

  SetMapMode(ps.hdc, MM_TEXT); //Set maping mode to text so drawicon will work

  DefWindowProc(m_hwnd, WM_ICONERASEBKGND, (WPARAM)ps.hdc, 0L); //Called to erase the border area of the iconic window

  GetClientRect(m_hwnd, &rcIconicWindow);  //Center the icon
  x = (rcIconicWindow.right - GetSystemMetrics(SM_CXICON))/2;
  y = (rcIconicWindow.bottom - GetSystemMetrics(SM_CYICON))/2;

  DrawIcon(ps.hdc, x, y, GetIcon() );

  SetMappingMode(ps.hdc);

  EndPaint(m_hwnd, &ps);
  }
////////////////////////////////////////////////////

HICON WINDOW::GetIcon()
  {
  return AAppGetIcon();
  }

////////////////////////////////////////////////////


void WINDOW::ResizeWindowItems(
  const LPRECT            rclNewSize)
  {
  BOOL                    changed;
  int                     vDif, hDif;
  OpWndItemD*             wi;
  RECT                    newItemRect;

  I_ASSERT(!WindowIsOpening() &&  !WindowIsClosing() );

  vDif = (rclNewSize->bottom - m_rclCurrentClient.bottom) - (rclNewSize->top - m_rclCurrentClient.top);
  hDif = (rclNewSize->right - m_rclCurrentClient.right) - (rclNewSize->left - m_rclCurrentClient.left);

  wi = NULL;
  while ( (wi = GetNextItem(wi, FALSE)) != NULL)
    {
    changed=FALSE;
    newItemRect = wi->m_rcItem;

    if (vDif && (wi->m_grow & I_GROW_TOP))    {newItemRect.top += vDif; changed = TRUE;}
    if (vDif && (wi->m_grow & I_GROW_BOTTOM)) {newItemRect.bottom += vDif;  changed = TRUE;}
    if (hDif && (wi->m_grow & I_GROW_RIGHT))  {newItemRect.right += hDif; changed = TRUE;}
    if (hDif && (wi->m_grow & I_GROW_LEFT))   {newItemRect.left += hDif;  changed = TRUE;}

    if (changed)
      OWndMoveItem(wi, &newItemRect);
    }

  }

//////////////////////////

void WINDOW::SetEraseRegion()
  {
  RECT          rclTmpClient;
  HRGN          hRgnItem, hRgnTmp;
  OpWndItemD*   wi;
  BOOL          bCombineSucess;

  RemoveEraseRegion(); //Remove the old eraseRegion

  if (m_hwnd)
    {
    GetClientRect(m_hwnd, &rclTmpClient);
    WndDStoLS( (LPPOINT)&rclTmpClient, 2);
    rclTmpClient.bottom = max(rclTmpClient.bottom, m_ptlScrollSize.y);
    rclTmpClient.right = max(rclTmpClient.right, m_ptlScrollSize.x);

    m_hrgnErase = CreateRectRgnIndirect(&rclTmpClient);

    wi = NULL;
    while( (wi=GetNextItem(wi, FALSE)) != NULL  &&  m_hrgnErase)
      {
      if (!WINDOWAPI::IsItemTransparent(wi) && WINDOWAPI::IsItemVisible(wi) )
        {
        hRgnTmp = CreateRectRgn(0,0,0,0);
        hRgnItem = CreateRectRgnIndirect(&wi->m_rcItem);
        bCombineSucess = FALSE;

        if (hRgnTmp  &&  hRgnItem)
          {
          if (CombineRgn(hRgnTmp, m_hrgnErase, hRgnItem, RGN_DIFF) != ERROR  &&
              CombineRgn(m_hrgnErase, hRgnTmp, 0, RGN_COPY) != ERROR)
            bCombineSucess = TRUE;
          }

        //Delete the temporary regions
        if (hRgnTmp)
          DeleteObject(hRgnTmp);
        if (hRgnItem)
          DeleteObject(hRgnItem);

        if (!bCombineSucess)
          RemoveEraseRegion(); //this will cause the while loop to break
        }
      }//while
    } //if (m_hwnd)

  }

//////////////////////////

void WINDOW::RemoveEraseRegion()
  {
  if (m_hrgnErase)
    {
    DeleteObject(m_hrgnErase);
    m_hrgnErase = NULL;
    }

  }

//////////////////////////

BOOL WINDOW::m_SetWindowOrgEx(
  int                     xOrigin,
  int                     yOrigin,
  LPPOINT                 lppt)
  {
  BOOL                    fRet;
  POINT                   pt;
  HDC                     hdc = GetDC(m_hwnd);

  fRet = SetWindowOrgEx(hdc, xOrigin, yOrigin, &pt);
  I_ASSERT(fRet && (pt.x == m_ptWindowOrg.x) && (pt.y == m_ptWindowOrg.y));

  ReleaseDC(m_hwnd, hdc);

  if(lppt)
    {
    lppt->x = m_ptWindowOrg.x;
    lppt->y = m_ptWindowOrg.y;
    }

  m_ptWindowOrg.x = xOrigin;
  m_ptWindowOrg.y = yOrigin;

  return fRet;
  }

//////////////////////////

BOOL WINDOW::m_SetViewportOrgEx(
  int                     xOrigin,
  int                     yOrigin,
  LPPOINT                 lppt)
  {
  BOOL                    fRet;
  POINT                   pt;
  HDC                     hdc = GetDC(m_hwnd);

  fRet = SetViewportOrgEx(hdc, xOrigin, yOrigin, &pt);
  I_ASSERT(fRet && (pt.x == m_ptViewportOrg.x  &&  pt.y == m_ptViewportOrg.y));

  ReleaseDC(m_hwnd, hdc);

  if(lppt)
    {
    lppt->x = m_ptViewportOrg.x;
    lppt->y = m_ptViewportOrg.y;
    }

  m_ptViewportOrg.x = xOrigin;
  m_ptViewportOrg.y = yOrigin;

  return fRet;
  }

/////////////////////////

void WINDOW::RestorePortOrigins()
  {
  HDC                     hdc;

  hdc = GetDC(m_hwnd);
  SetViewportOrg(hdc, m_ptViewportOrg.x, m_ptViewportOrg.y);
  SetWindowOrg(hdc, m_ptWindowOrg.x, m_ptWindowOrg.y);
  ReleaseDC(m_hwnd, hdc);
  }

//////////////////////////

void WINDOW::ValidateScrollBars()
  {
  RECT                    rc;
  POINT                   ptOrigin;
  POINT                   ptScroll          = m_ptlScrollSize;
  POINT                   ptSBarSize        = {GetSystemMetrics(SM_CXVSCROLL)-1, GetSystemMetrics(SM_CYHSCROLL)-1};
  int                     hMax, vMax;
  BOOL                    fHorz             = FALSE;
  BOOL                    fVert             = FALSE;
  BOOL                    fViewportChanged  = FALSE;

  // the scroll area is all in device units
  WndLStoDS(&ptScroll, 1);

  // get the client rectangle (neglect space lost to scroll bars)
  GetClientRect(m_hwnd, &rc);
  if(HScrollVisible())
    rc.bottom += ptSBarSize.y;
  if(VScrollVisible())
    rc.right  += ptSBarSize.x;

  hMax = ptScroll.x - (rc.right - rc.left);
  vMax = ptScroll.y - (rc.bottom - rc.top);

  if(HasHorzScroll() && (hMax > 0))
    {
    vMax += ptSBarSize.y;
    fHorz = TRUE;
    }

  if(HasVertScroll() && (vMax > 0))
    {
    hMax += ptSBarSize.x;
    fVert = TRUE;

    if(!fHorz && HasHorzScroll() && (hMax > 0))
      {
      vMax += ptSBarSize.y;
      fHorz = TRUE;
      }
    }

  m_GetViewportOrgEx(&ptOrigin);

  if(fHorz)
    {
    if((-ptOrigin.x + rc.right - rc.left - (fVert ? ptSBarSize.x : 0)) > ptScroll.x)
      {
      ptOrigin.x = -ptScroll.x + rc.right - rc.left - (fVert ? ptSBarSize.x : 0);
      fViewportChanged = TRUE;
      }
    }
  else if(ptOrigin.x != 0)
    {
    ptOrigin.x = 0;
    fViewportChanged = TRUE;
    }

  if(fVert)
    {
    if((-ptOrigin.y + rc.bottom - rc.top - (fHorz ? ptSBarSize.y : 0)) > ptScroll.y)
      {
      ptOrigin.y = -ptScroll.y + rc.bottom - rc.top - (fHorz ? ptSBarSize.y : 0);
      fViewportChanged = TRUE;
      }
    }
  else if(ptOrigin.y != 0)
    {
    ptOrigin.y = 0;
    fViewportChanged = TRUE;
    }

  if(fViewportChanged)
    {
    POINT ptOld;
    int   nFlag = 0;

    m_SetViewportOrgEx(ptOrigin.x, ptOrigin.y, &ptOld);
    InvalidateRect(m_hwnd, NULL, TRUE);

    // scroll the window and notify the items they have been scrolled
    nFlag |= (ptOld.x!=ptOrigin.x) ? OWND_LOC_HSCROLL : 0;
    nFlag |= (ptOld.y!=ptOrigin.y) ? OWND_LOC_VSCROLL : 0;

    if(nFlag)
      {
      OpWndItemD* wi = NULL;
      POINT       ptScrollWnd = {ptOrigin.x - ptOld.x, ptOrigin.y - ptOld.y};

      WndDStoLS(&ptScrollWnd, 1);
      ScrollWindow(m_hwnd, ptScrollWnd.x, ptScrollWnd.y, NULL, NULL);
      UpdateWindow(m_hwnd);

      while((wi = GetNextItem(wi, FALSE)) != NULL)
        wi->ScreenLocationChanged(nFlag);
      }
    }

  SetHScrollVisible(fHorz);
  ShowScrollBar(m_hwnd, SB_HORZ, fHorz);
  SetScrollRange(m_hwnd, SB_HORZ, 0, fHorz ? hMax : 0, TRUE);
  SetScrollPos(m_hwnd, SB_HORZ, fHorz ? -ptOrigin.x : 0, TRUE);

  SetVScrollVisible(fVert);
  ShowScrollBar(m_hwnd, SB_VERT, fVert);
  SetScrollRange(m_hwnd, SB_VERT, 0, fVert ? vMax : 0, TRUE);
  SetScrollPos(m_hwnd, SB_VERT, fVert ? -ptOrigin.y : 0, TRUE);
  }

//////////////////////////////////////////

void WINDOW::DoWindowHScroll(
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  int                     x = GetScrollPos(m_hwnd, SB_HORZ);
  int                     xMin, xMax;
  int                     nLine, nPage;
  RECT                    rcClient;

  // scroll based on window dimensions
  GetClientRect(m_hwnd, &rcClient);
  nPage = max(1, rcClient.right/2);
  nLine = max(1, rcClient.right/5);
  GetScrollRange(m_hwnd, SB_HORZ, &xMin, &xMax);

  switch(wParam)
    {
    case SB_TOP:
      x = xMin;
      break;

    case SB_BOTTOM:
      x = xMax;
      break;

    case SB_LINEDOWN:
      x = min(x + nLine, xMax);
      break;

    case SB_LINEUP:
      x = max(x - nLine, xMin);
      break;

    case SB_PAGEDOWN:
      x = min(x + nPage, xMax);
      break;

    case SB_PAGEUP:
      x = max(x - nPage, xMin);
      break;

    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
      x = LOWORD(lParam);
      break;

    default:
      break;
    }

  if(x != GetScrollPos(m_hwnd, SB_HORZ))
    {
    OpWndItemD* wi        = NULL;
    POINT       ptOrigin;
    POINT       ptScroll;

    m_GetViewportOrgEx(&ptOrigin);
    m_SetViewportOrgEx(-x, ptOrigin.y, NULL);

    SetScrollPos(m_hwnd, SB_HORZ, x, TRUE);
    ptScroll.x = ptOrigin.x + x;
    ptScroll.y = 0;
    WndDStoLS(&ptScroll, 1);
    ScrollWindow(m_hwnd, -ptScroll.x, -ptScroll.y, NULL, NULL);
    UpdateWindow(m_hwnd);

    while ( (wi = GetNextItem(wi, FALSE)) != NULL)
      wi->ScreenLocationChanged(OWND_LOC_HSCROLL);
    }
  }

////////////////////////////////////////

void WINDOW::DoWindowVScroll(
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  int                     y = GetScrollPos(m_hwnd, SB_VERT);
  int                     yMin, yMax;
  int                     nLine, nPage;
  RECT                    rcClient;

  // scroll based on window dimensions
  GetClientRect(m_hwnd, &rcClient);
  nPage = max(1, rcClient.bottom/2);
  nLine = max(1, rcClient.bottom/5);
  GetScrollRange(m_hwnd, SB_VERT, &yMin, &yMax);

  switch(wParam)
    {
    case SB_TOP:
      y = yMin;
      break;

    case SB_BOTTOM:
      y = yMax;
      break;

    case SB_LINEDOWN:
      y = min(y + nLine, yMax);
      break;

    case SB_LINEUP:
      y = max(y - nLine, yMin);
      break;

    case SB_PAGEDOWN:
      y = min(y + nPage, yMax);
      break;

    case SB_PAGEUP:
      y = max(y - nPage, yMin);
      break;

    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
      y = LOWORD(lParam);
      break;

    default:
      break;
    }

  if(y != GetScrollPos(m_hwnd, SB_VERT))
    {
    OpWndItemD* wi        = NULL;
    POINT       ptOrigin;
    POINT       ptScroll;

    m_GetViewportOrgEx(&ptOrigin);
    m_SetViewportOrgEx(ptOrigin.x, -y, NULL);

    SetScrollPos(m_hwnd, SB_VERT, y, TRUE);
    ptScroll.x = 0;
    ptScroll.y = ptOrigin.y + y;
    WndDStoLS(&ptScroll, 1);
    ScrollWindow(m_hwnd, -ptScroll.x, -ptScroll.y, NULL, NULL);
    UpdateWindow(m_hwnd);

    while ( (wi = GetNextItem(wi, FALSE)) != NULL)
      wi->ScreenLocationChanged(OWND_LOC_VSCROLL);
    }
  }


//////////////////////////////////////////
void WINDOW::Center(
  OBJECTID                oiCenterIn)
  {
  WINDOW*                 wndCenterIn;
  RECT                    rcCenterIn;
  int                     dxT, dyT, dxC, dyC;

  if (oiCenterIn)
    {
    wndCenterIn = LockWindowData(oiCenterIn);
    I_VERIFY_POINTER(wndCenterIn, return);
    CopyRect(&rcCenterIn, &wndCenterIn->m_rcdCurrentWindow);
    UnlockWindowData(oiCenterIn);
    }
  else
    GetWindowRect(GetDesktopWindow(), &rcCenterIn);


  dxT = m_rcdCurrentWindow.right - m_rcdCurrentWindow.left;
  dyT = m_rcdCurrentWindow.bottom - m_rcdCurrentWindow.top;
  dxC = rcCenterIn.right - rcCenterIn.left;
  dyC = rcCenterIn.bottom - rcCenterIn.top;

  m_rcdCurrentWindow.right = (dxT + dxC)/2 + rcCenterIn.left;
  m_rcdCurrentWindow.bottom = (dyT + dyC)/2 + rcCenterIn.top;

  m_rcdCurrentWindow.left = m_rcdCurrentWindow.right - dxT;
  m_rcdCurrentWindow.top = m_rcdCurrentWindow.bottom - dyT;

  if (m_hwnd)
    MoveWindow(m_hwnd, m_rcdCurrentWindow.left, m_rcdCurrentWindow.top, m_rcdCurrentWindow.right-m_rcdCurrentWindow.left, m_rcdCurrentWindow.bottom-m_rcdCurrentWindow.top, TRUE);
  }


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// WINDOW::MoveItemToBack //////////////////////////////////////////////////

void WINDOW::MoveItemToBack
  (
  OpWndItemD* wi
  )

  {
  OpWndItemD* wiTemp = wi;
  int         idxWI = wi->m_index;


  // do the index reorders
  while((wiTemp = GetNextItem(wiTemp, FALSE)) != NULL)   //MakeSureTheVisualInstancesAreCreated() is not
    wiTemp->m_index--;                                   //necessary, cause GetNextItem calls it

  // do the actual pointer reorder
  idxWI = wi->m_index;
  while(idxWI<(m_nItems-1))
    {
    m_aptrWndItem[idxWI] = m_aptrWndItem[idxWI+1];
    idxWI++;
    }
  wi->m_index = m_nItems - 1;
  m_aptrWndItem[m_nItems-1] = wi;
  }

// WINDOW::MoveItemToFront /////////////////////////////////////////////////

void WINDOW::MoveItemToFront
  (
  OpWndItemD* wi
  )

  {
  OpWndItemD* wiTemp = wi;
  int         idxWI = wi->m_index;

  // do the index reorders
  while((wiTemp = GetPrevItem(wiTemp, FALSE)) != NULL) //MakeSureTheVisualInstancesAreCreated() is not
    wiTemp->m_index++;                                 //necessary, cause GetPrevItem calls it

  // do the actual pointer reorder
  idxWI = wi->m_index;
  while(idxWI>0)
    {
    m_aptrWndItem[idxWI] = m_aptrWndItem[idxWI-1];
    idxWI--;
    }
  wi->m_index = 0;
  m_aptrWndItem[0] = wi;
  }

// WINDOW::ReorderItem /////////////////////////////////////////////////////

void WINDOW::ReorderItem
  (
  OpWndItemD* wi,
  int         nIndex
  )

  {
  OpWndItemD* wiTemp = wi;
  int         idxWI = wi->m_index;

  if(nIndex>idxWI)
    {
    // do the index reorders
    while(((wiTemp = GetNextItem(wiTemp, FALSE)) != NULL) && (wiTemp->m_index<=idxWI))
      wiTemp->m_index--;

    // do the actual pointer reorder
    idxWI = wi->m_index;
    while(idxWI<nIndex)
      {
      m_aptrWndItem[idxWI] = m_aptrWndItem[idxWI+1];      //MakeSureTheVisualInstancesAreCreated() is not
      idxWI++;                                            //necessary, cause GetNextItem calls it
      }
    wi->m_index = nIndex;
    m_aptrWndItem[nIndex] = wi;
    }
  else
    {
    // do the index reorders
    while(((wiTemp = GetPrevItem(wiTemp, FALSE)) != NULL) && (wiTemp->m_index>=idxWI))
      wiTemp->m_index++;

    // do the actual pointer reorder
    idxWI = wi->m_index;
    while(idxWI>nIndex)
      {
      m_aptrWndItem[idxWI] = m_aptrWndItem[idxWI-1];
      idxWI++;
      }
    wi->m_index = nIndex;
    m_aptrWndItem[nIndex] = wi;
    }
  }


// WINDOW::SetItemParent ///////////////////////////////////////////////////

void WINDOW::SetItemParent
  (
  OpWndItemD* wi,
  HWND        hwndParent
  )

  {
  wi->m_hwndParent = hwndParent;
  }

// WINDOW::SetAllParents ///////////////////////////////////////////////////

void WINDOW::SetAllParents
  (
  HWND  hwndParent
  )

  {
  OpWndItemD* wi = NULL;

  while((wi=GetNextItem(wi, FALSE))!=NULL)
    wi->m_hwndParent = hwndParent;
  }

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
long WINDOW::ValidateObjectIDs(
  OBJECTID                oiNew,      //The new OBJECTID for the window object
  pAObjMessage            theSystem)
  {
  WNDITEM_DATA*           wid;
  int                     i;
  BOOL                    bItemGood;

  AObjValidateID(&m_oiSelf, theSystem);
  m_oiSelf = oiNew;

  if (m_oiMenu)
    AObjValidateID(&m_oiMenu, theSystem);

  m_nItems = (int)( AObjGetDataCount(m_oiSelf) - I_DATA_ITEMS_OFFSET);
  i = 0;
  while(i<m_nItems)
    {
    wid = LockItemData(m_oiSelf, i);
    I_VERIFY_POINTER(wid, continue);
    bItemGood = wid->Validate(theSystem);
    UnlockItemData(m_oiSelf, i);

    if (!bItemGood)         // if the items no longer exists
      RemoveItemDataBlock(i); // decrements m_nItems
    else
      i++;
    }

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////

long WINDOW::CheckObject()
  {
  WNDITEM_DATA*           wid;
  BOOL                    bItemGood;
  int                     i;

  ValidateTitle(); //Make sure we have a valid title -- if not, get it from the object name

  m_nItems = (int)( AObjGetDataCount(m_oiSelf) - I_DATA_ITEMS_OFFSET);

  i = 0;
  while(i<m_nItems)
    {
    wid = LockItemData(m_oiSelf, i);
    I_VERIFY_POINTER(wid, continue);
    bItemGood = wid->Check();
    UnlockItemData(m_oiSelf, i);

    if (bItemGood)
      i++;
    else
      {
      RemoveItemDataBlock(i); // decrements m_nItems
      ADBG_EVAL( ADbgPrint(ADBG_ERROR, 0, "Deleted an item"));
      }

    }

  return A_OK;
  }


long WINDOW::Read()
  {
  AssignMappingMode(m_mappingMode); //will set the windowExt and viewPortExt

  if (m_version == 200)
    {
    m_bwBackground = m_clrBackground = I_WINDOW_BACKGROUND;
    m_bwForeground = m_clrForeground = I_WINDOW_FOREGROUND;
    _fmemcpy(m_aPattern, g_defaultPattern, sizeof(m_aPattern));
    m_version = 201;
    }

	if (m_version == 201)
		{								
		m_uidPaletteOwner = 0L;
		memcpy(m_aPaletteEntries, g_def_palette, sizeof(PALETTEENTRY) * I_NUM_PALETTE_ENTRIES);
		m_wNumPaletteEntries = I_NUM_PALETTE_ENTRIES;
    m_version = 202;
		}

  return A_OK;
  }
////////////////////////////////////////////////////////////////////////////

long WINDOW::BuildSignals(
  MEMBLOCKID              miSignal)
  {
  int                     i;
  long                    signalNum, numSignals;
  AObjSignalRecord huge * signal;
  WNDITEM_DATA*           wid;
  char                    objectName[80];


  m_nItems = (int)( AObjGetDataCount(m_oiSelf) - I_DATA_ITEMS_OFFSET);

  numSignals = ((long)m_nItems * 2L) + I_SIG_NUM_BASE_SIGNALS;

  AMemSetSize(miSignal, numSignals * (long)sizeof(AObjSignalRecord));
  signal = (pAObjSignalRecord)AMemLock(miSignal);
  I_VERIFY_POINTER(signal, return A_ERROR);

  signalNum=0;

  signal[signalNum].theID = I_SIG_SELECTED;
  lstrcpy(signal[signalNum++].theName, "Selected");

  signal[signalNum].theID = I_SIG_OPENED;
  lstrcpy(signal[signalNum++].theName, "Opened");

  signal[signalNum].theID = I_SIG_CLOSED;
  lstrcpy(signal[signalNum++].theName, "Closed");

  signal[signalNum].theID = I_SIG_MOVED;
  lstrcpy(signal[signalNum++].theName, "Moved");

  signal[signalNum].theID = I_SIG_RESIZED;
  lstrcpy(signal[signalNum++].theName, "Resized");

  signal[signalNum].theID = I_SIG_ICONIFIED;
  lstrcpy(signal[signalNum++].theName, "Iconified");

  signal[signalNum].theID = I_SIG_ICONRESTORED;
  lstrcpy(signal[signalNum++].theName, "Icon Restored");

  signal[signalNum].theID = I_SIG_ABOUTTOCLOSE;
  lstrcpy(signal[signalNum++].theName, "Before User Closes");


  for (i=0; i<m_nItems; i++)
    {
    wid = LockItemData(m_oiSelf, i);
    I_VERIFY_POINTER(wid, continue);

    wid->GetName(i, objectName, sizeof(objectName)-1);

    signal[signalNum].theID = wid->GetUID() + I_SIG_INOFFSET;
    wsprintf(signal[signalNum++].theName, "Cursor in %s", objectName);

    signal[signalNum].theID = wid->GetUID() + I_SIG_OUTOFFSET;
    wsprintf(signal[signalNum++].theName, "Cursor out of %s", objectName);

    UnlockItemData(m_oiSelf, i);
    }

  AMemUnlock(miSignal);

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
/// Removes an items data block and decrements m_nItems
void WINDOW::RemoveItemDataBlock(
  int                     itemNumber)
  {

  I_ASSERT_ITEM_NUMBER(this, itemNumber, return);

  AObjDeleteData(m_oiSelf, itemNumber+I_DATA_ITEMS_OFFSET);
  m_nItems--;
  }

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

//Creates all items in datas.  Resizes the array. return number created
int WINDOW::CreateExistingItems(
  MEMBLOCKID              miArray)
  {
  int                     idxFileItem, idxMemItem, numFileItems, numMemItems;
  WNDITEM_DATA*           wid;
  OpWndItemD**            array;

  numFileItems = (int)(AObjGetDataCount(m_oiSelf) - I_DATA_ITEMS_OFFSET);
  AMemSetSize(miArray, numFileItems * sizeof(LPVOID));  //Set to the size if all item are still good
  array = (OpWndItemD**)AMemLock(miArray);

  numMemItems = numFileItems;
  idxFileItem = idxMemItem = 0;

  while (idxFileItem < numFileItems)
    {
    array[idxMemItem] = NULL;
    wid = LockItemData(m_oiSelf, idxFileItem);
    I_VERIFY_POINTER(wid, continue);

    array[idxMemItem] = wid->CreateVI(m_oiSelf, GetItemDataSize(m_oiSelf, idxFileItem), idxMemItem);

    UnlockItemData(m_oiSelf, idxFileItem);

    if (array[idxMemItem])
      idxMemItem++;
    else/// The item for some reason can no longer live in the window
      numMemItems--;

    idxFileItem++;
    }

  AMemUnlock(miArray);
  AMemSetSize(miArray, numMemItems * sizeof(LPVOID));  //Set to the size of the actual number of good items

  return numMemItems;
  }

//////////////////////////////////////////////
////////////////////////////
//Creates a new item.  Does NOT add it to any array. itemIndex is -1..
//Increments m_uidNext
OpWndItemD* WINDOW::CreateNewItem(
  OBJECTID                oiItem,
  PWICreateFN             createFunction,
  LPRECT                  lprcItem,
  MEMBLOCKID              miExtra,
  LPSTR                   szShapeDllName)
  {
  OpWndItemD*             wi;
  WINDOWRUNTIME*          wr = GetWindowRuntime();
  long                    status = I_SF_VISIBLE;
  PWICreateFN             shapeCreateFunction;
  #ifdef AW_I_DEBUG
  BOOL                    bIsShape=FALSE;
  #endif

  I_VERIFY_POINTER(wr, return NULL);


  if (szShapeDllName && *szShapeDllName)
    {
    #ifdef AW_I_DEBUG
    bIsShape=TRUE;
    #endif
    shapeCreateFunction = wr->GetShapeCreateAndAddToList(szShapeDllName, (TYPEID)oiItem);
    if (!createFunction)
      createFunction = shapeCreateFunction;
    else if (shapeCreateFunction != createFunction)
      ADBG_EVAL( ADbgPrintRes(ADBG_WARNING, 0, g_hInstance, ERR_SHAPE_CREATEFN_MISMATCH, __FILE__, __LINE__));
    }

  if (createFunction)
    {
    wi = createFunction(oiItem, m_oiSelf, lprcItem, status, miExtra);
    if (wi)
      {
      wi->m_grow            = 0;
      wi->m_uidItem         = m_uidNext;
      wi->m_accelVKey       = 0;
      wi->m_accelModifier   = 0;
      wi->m_index           = -1;

      #ifdef AW_I_DEBUG
      if(bIsShape)
        I_ASSERT(WINDOWAPI::IsItemShape(wi) || WINDOWAPI::IsItemGroup(wi));
      else
        I_ASSERT(!WINDOWAPI::IsItemShape(wi) && !WINDOWAPI::IsItemGroup(wi));
      #endif

      m_uidNext += 10L;
      }
    }

  return wi;
  }

////////////////////////////
//Recreate an existing item
OpWndItemD* WINDOW::RecreateExistingItem(
  OpWndItemD*             wiIn)
  {
  OpWndItemD*             wiOut = NULL;
  WINDOWRUNTIME*          wr = GetWindowRuntime();
  MEMBLOCKID              miExtra;
  ApQueryInfo             qi;

  I_VERIFY_POINTER(wr, return NULL);

  if (wiIn->QueryProtocol(&qi, APROT_WND) != AP_NOTSUPPORTED)
    {
    miExtra = wiIn->GetExtraData();

    wiOut = ((PWICreateFN)qi.pfnCreate)(wiIn->Object(), m_oiSelf, &wiIn->m_rcItem, wiIn->m_status, miExtra);
    if (wiOut)
      {
      wiOut->m_hwndParent       = wiIn->m_hwndParent;
      wiOut->m_grow             = wiIn->m_grow;
      wiOut->m_uidItem          = wiIn->m_uidItem;
      wiOut->m_accelVKey        = wiIn->m_accelVKey;
      wiOut->m_accelModifier    = wiIn->m_accelModifier;
      wiOut->m_index            = wiIn->m_index;
      }

    if(miExtra)
        AMemFree(miExtra);

    wiIn->Destroy();
    }
  else
    {
    I_PROTOCOL_NOT_SUPPORTED(wiIn->Type());
    }

  return wiOut;
  }


//////////////////////////////////////////////
//////////////////////////////////////////////
BOOL WINDOW::PutItemsIntoDatas(
  MEMBLOCKID              miArray,
  int                     numItems)
  {
  WNDITEM_DATA*           widSource;
  WNDITEM_DATA*           widTarget;
  OpWndItemD**            array;
  MEMBLOCKID              miExtra;
  LONG                    itemDataSize;
  LPSTR                   ptr;
  LPVOID                  extra;
  int                     i, j;
  WINDOWRUNTIME*          wr = GetWindowRuntime();
  BOOL                    fRet;

  if(wr)
    { // save the items
    array = (OpWndItemD**)AMemLock(miArray);
    AObjSetDataCount(m_oiSelf, numItems+I_DATA_ITEMS_OFFSET);

    j=0;
    for (i=0; i<numItems; i++)
      {
      if(array[i+j])
        {
        widSource = new WNDITEM_DATA(array[i+j]);
        miExtra = array[i+j]->GetExtraData();  //For the extra vi stuff

        itemDataSize = sizeof(WNDITEM_DATA);
        if (miExtra)
          itemDataSize += AMemGetSize(miExtra);

        SetItemDataSize(m_oiSelf, i, itemDataSize);
        widTarget = LockItemData(m_oiSelf, i);
        I_VERIFY_POINTER(widTarget, AMemUnlock(miArray);return FALSE);

        hmemcpy(widTarget, widSource, sizeof(WNDITEM_DATA) );

        delete widSource;

        if (miExtra)
          {
          ptr = (LPSTR)widTarget + sizeof(WNDITEM_DATA);
          extra = AMemLock(miExtra);
          hmemcpy(ptr, extra, AMemGetSize(miExtra));
          AMemUnlock(miExtra);
          AMemFree(miExtra);
          }

        UnlockItemData(m_oiSelf, i);
        }
      else
        {
        j++;
        i--;
        numItems--;
        AObjSetDataCount(m_oiSelf, numItems+I_DATA_ITEMS_OFFSET);
        }
      }

    AMemUnlock(miArray);
    fRet = TRUE;
    }
  else  // saving at shutdown or design time
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////
//Destroys all items in the array and resizes the array to 0.
BOOL WINDOW::DestroyAllItems(
  MEMBLOCKID              miArray,
  int                     numItems)
  {
  OpWndItemD**            array;
  int                     i;

  array = (OpWndItemD**)AMemLock(miArray);

  for (i=0; i<numItems; i++)
    array[i]->Destroy();

  AMemUnlock(miArray);
  AMemSetSize(miArray, 0);
  return TRUE;
  }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
BOOL WINDOW::SetMenuHandle() //uses m_oiMenu--returns true for sucess--releases current hmenu if it exists
  {
  MENUSPARAMBLOCK         menuBlock;

  I_VERIFY_WINDOW(GetWindowHandle(), return FALSE);

  ReleaseMenuHandle();

  menuBlock.objID = GetMenuObject();
  menuBlock.hMenu = 0;
  menuBlock.itemID = 0;

  AEvtCallMethod(m_oiSelf, GetMenuObject(), 0, OEVENT_GETMENUHANDLE, (LONG)&menuBlock);
  if (menuBlock.hMenu)
    SetMenu(GetWindowHandle(), menuBlock.hMenu);

  return TRUE;
  }

///////////////////////////////////////////////////

BOOL WINDOW::ReleaseMenuHandle() //Release the hmenu associtated with m_hwnd if one exists
  {
  MENUSPARAMBLOCK         menuBlock;
  HMENU                   hmenu;

  I_VERIFY_WINDOW(GetWindowHandle(), return FALSE);

  hmenu  = GetMenu(GetWindowHandle());

  SetMenu(GetWindowHandle(), NULL);

  if (hmenu)
    {
    menuBlock.objID = GetMenuObject();
    menuBlock.hMenu = hmenu;
    menuBlock.itemID = 0;

    AEvtCallMethod(m_oiSelf, GetMenuObject(), 0, OEVENT_RELEASEMENUHANDLE, (LONG)&menuBlock);
    }

  return TRUE;
  }

BOOL WINDOW::RedrawMenu()
  {
  I_VERIFY_WINDOW(GetWindowHandle(), return FALSE);

  DrawMenuBar(GetWindowHandle());
  return TRUE;
  }

/////////////////////////////////////

void WINDOW::GetColorsAndPattern(
  COLORREF far *          pclrCBackground,
  COLORREF far *          pclrCForeground,
  COLORREF far *          pclrBWBackground,
  COLORREF far *          pclrBWForeground,
  short*                  ptrPattern,
  UINT                    cbSize)
  {
  if (pclrCBackground)
    *pclrCBackground = m_clrBackground;
  if (pclrCForeground)
    *pclrCForeground = m_clrForeground;

  if (pclrBWBackground)
    *pclrBWBackground = m_bwBackground;
  if (pclrBWForeground)
    *pclrBWForeground = m_bwForeground;

  if (ptrPattern)
    _fmemcpy(ptrPattern, m_aPattern, max( sizeof(m_aPattern), cbSize) );
  }

/////////////////////////////////////

void WINDOW::SetColorsAndPattern(
  COLORREF                clrCBackground,
  COLORREF                clrCForeground,
  COLORREF                clrBWBackground,
  COLORREF                clrBWForeground,
  const short*            ptrPattern,
  UINT                    cbSize)
  {
  m_clrBackground =  clrCBackground ;
  m_clrForeground =  clrCForeground ;

  m_bwBackground =  clrBWBackground;
  m_bwForeground =  clrBWForeground;

  if (ptrPattern)
    _fmemcpy(m_aPattern, ptrPattern, max( sizeof(m_aPattern), cbSize) );
  }

////////////////////////////////////////

BOOL WINDOW::Print(
  LPPIDR_BLOCK            pidr)
  {
  OpWndItemD*             wi;
  RECT                    rcItemPrint;
  int                     leftOffset, topOffset;

  if (!pidr->numPages) //Not just asking for the number of pages
    {
    MakeSureTheVisualInstancesAreCreated();

    leftOffset = pidr->rect.left;
    topOffset = pidr->rect.top;

    wi = NULL;
    while( (wi=GetNextItem(wi, FALSE)) != NULL)
      {
      CopyRect(&rcItemPrint, &wi->m_rcItem);
      rcItemPrint.top     = topOffset  + MulDiv(rcItemPrint.top,    pidr->DPI.y, OWND_TMM_PER_INCH);
      rcItemPrint.bottom  = topOffset  + MulDiv(rcItemPrint.bottom, pidr->DPI.y, OWND_TMM_PER_INCH);
      rcItemPrint.left    = leftOffset + MulDiv(rcItemPrint.left,   pidr->DPI.x, OWND_TMM_PER_INCH);
      rcItemPrint.right   = leftOffset + MulDiv(rcItemPrint.right,  pidr->DPI.x, OWND_TMM_PER_INCH);
      if (WINDOWAPI::IsItemVisible(wi))
        wi->Print(pidr->hDC, &rcItemPrint, pidr->pclPrinter);
      }
    }

  pidr->toPage = 1;

  return TRUE;
  }

////////////////////////////////////

BOOL WINDOW::CreatePaletteHandle()
  {
  MEMBLOCKID              miPalette;
  LOGPALETTE*             palette;
  HDC                     hdc;
  int                     iSizePalette;

  m_hPalette = NULL;

  //See if we are on a 256 color machine
  hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
  iSizePalette = GetDeviceCaps(hdc, SIZEPALETTE);
  DeleteDC(hdc);
  if (iSizePalette != 256)
    return FALSE;

  ////////////////////

  miPalette = AMemAllocate(sizeof(LOGPALETTE) + sizeof(g_def_palette) );
  palette = (LOGPALETTE*)AMemLock(miPalette);
  I_VERIFY_POINTER(palette, return FALSE);

  palette->palVersion = 0x300;
  palette->palNumEntries = m_wNumPaletteEntries;
  memcpy(&palette->palPalEntry, m_aPaletteEntries, m_wNumPaletteEntries * sizeof(PALETTEENTRY));

  m_hPalette = CreatePalette(palette);

  AMemUnlock(miPalette);
  AMemFree(miPalette);

  return m_hPalette != NULL ? TRUE : FALSE;
  }

////////////////////////////////////

BOOL WINDOW::DeletePaletteHandle()
  {
  BOOL                    rVal = FALSE;

  if (m_hPalette)
    {
    DeleteObject(m_hPalette);
    m_hPalette = 0;
    rVal = TRUE;
    }

  return rVal;
  }

////////////////////////////////////

void WINDOW::GetPaletteEntries(
	PALETTEENTRY * 					pPalEntries, 
	WORD 										wNumEntries)
	{
	memcpy(pPalEntries, m_aPaletteEntries, min(I_NUM_PALETTE_ENTRIES, wNumEntries) * sizeof(PALETTEENTRY) );
	}

////////////////////////////////////

BOOL WINDOW::SetPaletteEntries(
	PALETTEENTRY * 					pPalEntries, 
	WORD 										wNumEntries)
	{
	HDC											hdc;
	HPALETTE								hPalOld;
  LPARAM                  rVal=0L;


	m_wNumPaletteEntries = min(I_NUM_PALETTE_ENTRIES, wNumEntries);
	memcpy(m_aPaletteEntries, pPalEntries, m_wNumPaletteEntries * sizeof(PALETTEENTRY) );

  if (m_hPalette)
		{
		DeletePaletteHandle();
		CreatePaletteHandle();

		hdc = GetDC(m_hwnd);
	  hPalOld = SelectPalette(hdc, m_hPalette, FALSE);
	  rVal = RealizePalette(hdc);         
	  SelectPalette(hdc, hPalOld, TRUE);
	  RealizePalette(hdc);
	  ReleaseDC(m_hwnd, hdc);
		}

	if (m_hwnd && rVal)
		InvalidateRect(m_hwnd, NULL, TRUE);
	
	return TRUE;
	}


////////////////////////////////////
////////////////////////////////////

#ifdef ADBG_DEBUG
WINDOW* WINDOW::LockWindowData
  (
  OBJECTID  oi
  )

  {
  OWndRuntimeInfo*  wri = (OWndRuntimeInfo*)AObjGetRuntimeInfo(oi);

  if(wri)
    wri->nLockCount++;
  else
    {
    wri = new OWndRuntimeInfo;

    wri->pWnd       = ((WINDOW*)AObjLockData(oi, I_DATA_WINDOW));
    wri->nLockCount = 1;

    I_VERIFY_POINTER(wri->pWnd, return NULL);

    if (wri->pWnd->m_oiSelf != oi)
      {
      wri->pWnd->m_oiSelf = oi;
      ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_SELFID_MISMATCH);
      }

    AObjSetRuntimeInfo(oi, (long)wri);
    }

  return wri->pWnd;
  }

void WINDOW::UnlockWindowData
  (
  OBJECTID  oi
  )

  {
  OWndRuntimeInfo*  wri = (OWndRuntimeInfo*)AObjGetRuntimeInfo(oi);

  I_VERIFY_POINTER(wri, ;);
  if(wri)
    {
    wri->nLockCount--;
    I_ASSERT(wri->nLockCount>=0);
    if(wri->nLockCount<=0)
      {
      AObjUnlockData(oi, I_DATA_WINDOW);
      delete wri;

      AObjSetRuntimeInfo(oi, (long)NULL);
      }
    }
  }

#endif
