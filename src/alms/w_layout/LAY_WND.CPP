////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "lay_wnd.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_wndlay.h>

#include "xp_macro.h"
#include "wlay.h"
#include "lay_wnd.h"
#include "lay_shap.h"
#include "layout.h"
#include "lay_dlg.h"
#include "rc\wlay_rsc.h"
#include "objlist.h"
#include "menudata.h"

// global variables ////////////////////////////////////////////////////////

extern HINSTANCE    g_hInstance;
extern const char*  c_szFrameClassName;
extern const char*  c_szClientClassName;
extern const char*  c_szMenuClassName;

Window*             g_pWnd = NULL;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window Constructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

Window::Window
  (
  AOBJECTID  oiWnd,
  Layout*   layout
  ) : WndData(oiWnd),
      m_items(this)

  {
  m_layout = layout;

  m_hwndWindow = NULL;
  m_hwndMenu   = NULL;
  m_hwndClient = NULL;

  if(AObjGetDataSize(m_oiWnd, OWNDLAY_LAYOUTDATA) != sizeof(WNDLAY_DATA))
    AObjResizeData(m_oiWnd, OWNDLAY_LAYOUTDATA, (long)sizeof(WNDLAY_DATA));

  m_pwndlaydata = (WNDLAY_DATA*)AObjLockData(m_oiWnd, OWNDLAY_LAYOUTDATA);
  if((m_pwndlaydata->lSignature != LAYOUT_SIGNATURE)
          ||
     (m_pwndlaydata->sVer != LAYOUT_VERSION))
    {
    // lose the old layout info (and new version info)
    m_pwndlaydata->lSignature = LAYOUT_SIGNATURE;
    m_pwndlaydata->sVer       = LAYOUT_VERSION;
    m_pwndlaydata->sPref      = DefaultPreferences();
    m_pwndlaydata->sGridX     = m_layout->DefaultGridWidth();
    m_pwndlaydata->sGridY     = m_layout->DefaultGridHeight();
    }
  
  m_fFullInfo = !DefWYSIWYGMode();
  SetPoint(&m_ptNext, 0, 0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window Destructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

Window::~Window
  (
  )

  {
  if(PrstMode())
    {
    if(m_fFullInfo)
      {
      m_pwndlaydata->sPref |= WNDPREF_FULLINFOMODE;
      m_pwndlaydata->sPref &= ~WNDPREF_WYSIWYGMODE;
      }
    else
      {
      m_pwndlaydata->sPref &= ~WNDPREF_FULLINFOMODE;
      m_pwndlaydata->sPref |= WNDPREF_WYSIWYGMODE;
      }
    }

  AObjUnlockData(m_oiWnd, OWNDLAY_LAYOUTDATA);
  }
  
////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::MoveClient (private)
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::MoveClient
  (
  )

  {
  RECT  rc;
  POINT pt;
  int   hMax, vMax;
  BOOL  fHorz             = FALSE;
  BOOL  fVert             = FALSE;
  POINT ptScrollBarSize;
  POINT ptScrollSize      = m_ptScrollSize;
  POINT ptViewportOrg;
  BOOL  fViewportChanged  = FALSE;

  I_ASSERT(m_hwndWindow && m_hwndClient);

  GetClientRect(m_hwndWindow, &rc);
  if(HasMenuBar())
    {
    RECT  rcMenu;
    GetClientRect(m_hwndMenu, &rcMenu);
    if(rcMenu.bottom != 0)
      rc.top += (rcMenu.bottom + 1);
    }

  MoveWindow(m_hwndClient, rc.left-1, rc.top-1, rc.right-rc.left+2, rc.bottom-rc.top+2, TRUE);

  // update the scroll bars for the client window
  SetPoint(&ptScrollBarSize, GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));
  OWndLStoDS(m_oiWnd, &ptScrollSize, 1);

  pt.x = rc.right - rc.left;
  pt.y = rc.bottom - rc.top;

  hMax = ptScrollSize.x - pt.x;
  vMax = ptScrollSize.y - pt.y;

  if(HasHorzScroll() && (hMax > 0))
    {
    vMax += ptScrollBarSize.y;
    fHorz = TRUE;
    }

  if(HasVertScroll() && (vMax > 0))
    {
    hMax += ptScrollBarSize.x;
    fVert = TRUE;
    if(!fHorz && HasHorzScroll() && (hMax > 0))
      {
      vMax += ptScrollBarSize.y;
      fHorz = TRUE;
      }
    }

  OWndLayGetViewportOrgEx(m_oiWnd, &ptViewportOrg);

  if(fHorz)
    {
    if((-ptViewportOrg.x + pt.x - (fVert ? ptScrollBarSize.x : 0)) > ptScrollSize.x)
      {
      ptViewportOrg.x = -ptScrollSize.x + pt.x - (fVert ? ptScrollBarSize.x : 0);
      fViewportChanged = TRUE;
      }
    }
  else if(ptViewportOrg.x != 0)
    {
    ptViewportOrg.x = 0;
    fViewportChanged = TRUE;
    }

  if(fVert)
    {
    if((-ptViewportOrg.y + pt.y - (fHorz ? ptScrollBarSize.y : 0)) > ptScrollSize.y)
      {
      ptViewportOrg.y = -ptScrollSize.y + pt.y - (fHorz ? ptScrollBarSize.y : 0);
      fViewportChanged = TRUE;
      }
    }
  else if(ptViewportOrg.y != 0)
    {
    ptViewportOrg.y = 0;
    fViewportChanged = TRUE;
    }

  if(fViewportChanged)
    {
    POINT ptOld;

    OWndLayGetViewportOrgEx(m_oiWnd, &ptOld);
    OWndLaySetViewportOrgEx(m_oiWnd, ptViewportOrg.x, ptViewportOrg.y, NULL);
    InvalidateRect(m_hwndClient, NULL, TRUE);
    Items()->ViewportChanged(&ptOld);
    }

  ShowScrollBar(m_hwndClient, SB_HORZ, fHorz);
  SetScrollRange(m_hwndClient, SB_HORZ, 0, fHorz ? hMax : 0, TRUE);
  SetScrollPos(m_hwndClient, SB_HORZ, fHorz ? -ptViewportOrg.x : 0, TRUE);
  
  ShowScrollBar(m_hwndClient, SB_VERT, fVert);
  SetScrollRange(m_hwndClient, SB_VERT, 0, fVert ? vMax : 0, TRUE);
  SetScrollPos(m_hwndClient, SB_VERT, fVert ? -ptViewportOrg.y : 0, TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::MoveMenu (private)
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::MoveMenu
  (
  )

  {
  RECT      rc;
  MenuData* pMenuData;
  int       nHeight;

  I_ASSERT(m_hwndWindow && m_hwndMenu);

  GetClientRect(m_hwndWindow, &rc);
  pMenuData = (MenuData*)GetWindowLong(m_hwndMenu, I_GWL_MENUDATA);
  I_ASSERT(pMenuData);

  if(pMenuData)
    {
    nHeight = pMenuData->Height(rc.right - rc.left);
    if(nHeight==0)
      nHeight--;
    MoveWindow(m_hwndMenu, rc.left-1, rc.top-1, rc.right-rc.left+2, nHeight+2, TRUE);
    }
  else
    MoveWindow(m_hwndMenu, rc.left-1, rc.top-1, rc.right-rc.left+2, GetSystemMetrics(SM_CYMENU)+2, TRUE);

  ShowWindow(m_hwndMenu, HasMenuBar() ? SW_SHOWNA : SW_HIDE);
  if(HasMenuBar())
    InvalidateRect(m_hwndMenu, NULL, TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::DefaultPreferences (private)
//
//  DESCRIPTION:  This method converts layout preferences to window prefs.
//
////////////////////////////////////////////////////////////////////////////

short Window::DefaultPreferences
  (
  )

  {
  WORD  wDefPref = m_layout->DefaultPreferences();
  short sPref = 0;

  if(wDefPref & LAYPREF_FULLINFOMODE)     sPref |= WNDPREF_FULLINFOMODE;
  if(wDefPref & LAYPREF_WYSIWYGMODE)      sPref |= WNDPREF_WYSIWYGMODE;
  if(wDefPref & LAYPREF_PRSTMODE)         sPref |= WNDPREF_PRSTMODE;
  if(wDefPref & LAYPREF_SHOWGRID)         sPref |= WNDPREF_SHOWGRID;
  if(wDefPref & LAYPREF_SNAPTOGRID)       sPref |= WNDPREF_SNAPTOGRID;
  if(wDefPref & LAYPREF_REQFULLENCLOSURE) sPref |= WNDPREF_REQFULLENCLOSURE;

  return sPref;
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      Window::CreateMenuWindow (private)
//
// Description: This function creates the sample menu window.
//
////////////////////////////////////////////////////////////////////////////

void Window::CreateMenuWindow
  (
  )

  {
  RECT  rc;

  GetClientRect(m_hwndWindow, &rc);

  m_hwndMenu = CreateWindow
    (
    c_szMenuClassName,
    NULL,
    WS_CHILD | WS_BORDER,
    rc.left-1,
    rc.top-1,
    rc.right-rc.left+2,
    GetSystemMetrics(SM_CYMENU)+2,
    m_hwndWindow,
    NULL,
    g_hInstance,
    this
    );

  ShowWindow(m_hwndMenu, HasMenuBar() ? SW_SHOW : SW_HIDE);
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      Window::CreateClientWindow (private)
//
// Description: This function creates the client window (for items).
//
////////////////////////////////////////////////////////////////////////////

void Window::CreateClientWindow
  (
  )

  {
  RECT  rc;
  HDC   hdc;

  GetClientRect(m_hwndWindow, &rc);

  // create the client window with scroll bars so they can be hidden
  // and shown without messing up Windows (thanks Bill Gates!)
  if(HasMenuBar())
    rc.top += (GetSystemMetrics(SM_CYMENU)+1);
  m_hwndClient = CreateWindow
    (
    c_szClientClassName,
    NULL,
    WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL,
    rc.left-1,
    rc.top-1,
    rc.right-rc.left+2,
    rc.bottom-rc.top+2,
    m_hwndWindow,
    NULL,
    g_hInstance,
    this
    );

  // hide/show the scroll bars as necessary
  ShowScrollBar(m_hwndClient, SB_HORZ, HasHorzScroll());
  ShowScrollBar(m_hwndClient, SB_VERT, HasVertScroll());

  // display the window
  ShowWindow(m_hwndClient, SW_SHOW);

  // the items need to know their parent window ... set the value
  OWndLaySetWindowHandle(m_oiWnd, m_hwndClient);
  m_items.SetParentWnd(m_hwndClient);

  // set the mapping mode of the window
  hdc = GetDC(m_hwndClient);
  OWndLaySetMappingMode(hdc, OWNDLAY_MM_INDEPENDANT);
  OWndLaySetWindowOrgEx(m_oiWnd, 0, 0, NULL);
  OWndLaySetViewportOrgEx(m_oiWnd, 0, 0, NULL);
  ReleaseDC(m_hwndClient, hdc);

  // set the client area as a drop target
  SetObjectListDragTarget(m_hwndClient);

  // set frame mode for the items
  m_items.ChangeFullInfo(DrawFrameMode());

  // create all the items
  m_items.CreateAll();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::FixWindowStyle (private)
//
//  DESCRIPTION:  This will adjust the window to fit the current style.
//
////////////////////////////////////////////////////////////////////////////

void Window::FixWindowStyle
  (
  )

  {
  DWORD dwExStyle     = m_GetWindowStyleEx();
  DWORD dwStyle       = m_GetWindowStyle();
  RECT  rc;
  POINT ptScrollSize  = m_ptScrollSize;
  int   nHeight;
  int   nWidth;
  HDC   hdc;

  CopyRect(&rc, &m_rcInitClient);
  OWndLStoDS(m_oiWnd, (LPPOINT)&rc, 2);

  // adjust the window for the scroll bars
  OWndLStoDS(m_oiWnd, &ptScrollSize, 1);
  nHeight = rc.bottom - rc.top;
  nWidth  = rc.right - rc.left;
  if((dwStyle & WS_VSCROLL) && (nHeight >= ptScrollSize.y))
    rc.right -= (GetSystemMetrics(SM_CXVSCROLL) - 1);
  if((dwStyle & WS_HSCROLL) && (nWidth >= ptScrollSize.x))
    rc.bottom -= (GetSystemMetrics(SM_CYHSCROLL) - 1);

  // adjust the window rectangle to make sure the client size is correct
  OWndLayAdjWindowRectEx(&rc, dwStyle, HasMenuBar() ? m_oiMenu : NULL, dwExStyle);
  dwStyle &= ~(WS_HSCROLL | WS_VSCROLL);

  // fix the window title and style.
  SetWindowText(m_hwndWindow, m_szTitle);
  SetWindowLong(m_hwndWindow, GWL_EXSTYLE, dwExStyle);
  SetWindowLong(m_hwndWindow, GWL_STYLE, dwStyle);

  // adjust the position to account for possible scrolling in the layout window
  hdc = GetDC(m_layout->WndHandle());
  LPtoDP(hdc, (LPPOINT)&rc, 2);
  ReleaseDC(m_layout->WndHandle(), hdc);

  // cause a redraw to take place
  SetWindowPos(m_hwndWindow, HWND_TOP, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER | SWP_DRAWFRAME);
  MoveClient();

  // adjust layout window scroll bars as necessary -- window size changed
  m_layout->UpdateScrollRegion();
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      Window::Open
//
// Description: This function creates and opens windows for the window object.
//
////////////////////////////////////////////////////////////////////////////

void Window::Open
  (
  )

  {
  DWORD dwExStyle     = m_GetWindowStyleEx();
  DWORD dwStyle       = m_GetWindowStyle();
  RECT  rc;
  short nCmdShow      = m_nInitCmdShow;
  POINT ptScrollSize  = m_ptScrollSize;
  int   nHeight;
  int   nWidth;

  CopyRect(&rc, &m_rcInitClient);
  OWndLStoDS(m_oiWnd, (LPPOINT)&rc, 2);

  // adjust the window for the scroll bars
  OWndLStoDS(m_oiWnd, &ptScrollSize, 1);
  nHeight = rc.bottom - rc.top;
  nWidth  = rc.right - rc.left;
  if((dwStyle & WS_VSCROLL) && (nHeight >= ptScrollSize.y))
    rc.right -= (GetSystemMetrics(SM_CXVSCROLL) - 1);
  if((dwStyle & WS_HSCROLL) && (nWidth >= ptScrollSize.x))
    rc.bottom -= (GetSystemMetrics(SM_CYHSCROLL) - 1);

  // adjust the window rectangle to make sure the client size is correct
  OWndLayAdjWindowRectEx(&rc, dwStyle, HasMenuBar() ? m_oiMenu : NULL, dwExStyle);

  // set a global variable for MicroSUCKs -- for WM_MINMAXINFO
  g_pWnd = this;

  // create the window without scroll bars.  The client window will
  // have the scroll bars.
  #ifdef AW_I_DEBUG
  HWND  hwnd =
  #endif  
  CreateWindowEx
    (
    dwExStyle,
    c_szFrameClassName,
    Title(),
    (dwStyle & ~(WS_VISIBLE | WS_VSCROLL | WS_HSCROLL)),
    rc.left,
    rc.top,
    rc.right-rc.left,
    rc.bottom-rc.top,
    m_layout->WndHandle(),
    NULL,
    g_hInstance,
    this
    );

  g_pWnd = NULL;

  #ifdef AW_I_DEBUG
  I_ASSERT(hwnd==m_hwndWindow);
  #endif

  if(nCmdShow != m_nInitCmdShow)
    m_nInitCmdShow = nCmdShow;

  ShowWindow(m_hwndWindow, (nCmdShow==SW_SHOWNORMAL) ? SW_SHOW : nCmdShow);
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      Window::Close
//
// Description: This function closes the windows for the window object.
//
////////////////////////////////////////////////////////////////////////////

void Window::Close
  (
  )

  {
  // restore the viewport and window origins for the window's data
  OWndLaySetWindowOrgEx(m_oiWnd, 0, 0, NULL);
  OWndLaySetViewportOrgEx(m_oiWnd, 0, 0, NULL);

  // destroy the items and the window
  m_items.DestroyAll();
  DestroyWindow(m_hwndWindow);

  m_hwndWindow = NULL;
  m_hwndClient = NULL;
  m_hwndMenu   = NULL;

  OWndLaySetWindowHandle(m_oiWnd, m_hwndClient);
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      Window::Dirty
//
// Description: This function determines if the window has chaged.  The
//              window changing sizes or moving will not immediately cause
//              the dirty flag to be set.  This routine will check and
//              update the starting window rectangle.
//
////////////////////////////////////////////////////////////////////////////

BOOL Window::Dirty
  (
  )

  {
  short nCmdShow;
  RECT  rcWnd;
  HDC   hdc;

  // get the initial window state
  if(IsZoomed(m_hwndWindow))
    nCmdShow = SW_SHOWMAXIMIZED;
  else if(IsIconic(m_hwndWindow))
    nCmdShow = SW_SHOWMINIMIZED;
  else
    nCmdShow = SW_SHOWNORMAL;

  if(nCmdShow != m_nInitCmdShow)
    m_fDirty = TRUE;

  // get the initial window size
  GetClientRect(m_hwndClient, &rcWnd);
  MapWindowPoints(m_hwndClient, m_layout->WndHandle(), (LPPOINT)&rcWnd, 2);

  // convert the window position to logical (scrolled) coordinates
  hdc = GetDC(m_layout->WndHandle());
  DPtoLP(hdc, (LPPOINT)&rcWnd, 2);
  ReleaseDC(m_layout->WndHandle(), hdc);

  OWndDStoLS(m_oiWnd, (LPPOINT)&rcWnd, 2);
  if(!EqualRect(&m_rcInitClient, &rcWnd))
    {
    CopyRect(&m_rcInitClient, &rcWnd);
    m_fDirty = TRUE;
    }

  return m_fDirty;
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      Window::FrameWndCreated
//
// Description: This function initializes the frame window (from WM_CREATE).
//
////////////////////////////////////////////////////////////////////////////

void Window::FrameWndCreated
  (
  HWND  hwnd
  )

  {
  HMENU hmenuSys;

  // save the frame window handle
  m_hwndWindow = hwnd;

  // draw the window as active if the parent is active
  if(GetActiveWindow()==GetParent(hwnd))
    SendMessage(hwnd, WM_NCACTIVATE, (WPARAM)TRUE, 0L);

  // strip the system menu of the last 4 items
  hmenuSys = GetSystemMenu(hwnd, FALSE);
  DeleteMenu(hmenuSys, 5, MF_BYPOSITION);
  DeleteMenu(hmenuSys, 5, MF_BYPOSITION);
  DeleteMenu(hmenuSys, 5, MF_BYPOSITION);
  DeleteMenu(hmenuSys, 5, MF_BYPOSITION);

  // create the child windows
  CreateMenuWindow();
  CreateClientWindow();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::SnapPointToGrid
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::SnapPointToGrid
  (
  LPPOINT lppt
  )

  {
  if(SnapToGrid())
    {
    POINT ptGrid;

    GetGrid(&ptGrid);
    if(ptGrid.x>1)
      lppt->x = (int)((lppt->x + (int)(ptGrid.x / 2)) / ptGrid.x) * ptGrid.x;

    if(ptGrid.y>1)
      lppt->y = (int)((lppt->y + (int)(ptGrid.y / 2)) / ptGrid.y) * ptGrid.y;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::Update
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::Update
  (
  )

  {
  // call Dirty() to get the window position and set the dirty flag if
  // something has changed.
  Dirty();

  // Update the window
  SaveWindowData();
  m_items.Update();

  m_nChanged++;
  m_fDirty = FALSE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::Revert
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::Revert
  (
  )

  {
  if(Dirty())
    {
    GetWindowData();
    m_items.Revert();
    FixWindowStyle();
    InvalidateRect(m_hwndWindow, NULL, TRUE);

    m_fDirty = FALSE;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::EditAttributes
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::EditAttributes
  (
  )

  {
  WNDSTYLE_DLG  wsd;

  lstrcpy(wsd.szTitle, m_szTitle);
  wsd.style = Style();
  wsd.oiMenu = m_oiMenu;

  m_layout->EnablePalettes(FALSE);
  if(DialogBoxParam(g_hInstance, ID(DLG_WNDSTYLE), m_layout->WndHandle(), OWLayWndStyleDlgProc, (LPARAM)&wsd))
    {
    // save the style changes
    lstrcpy(m_szTitle, wsd.szTitle);
    SetStyle(wsd.style);

    // update the menu bar
    if(m_oiMenu != wsd.oiMenu)
      {
      MenuData* pMenuData = (MenuData*)GetWindowLong(m_hwndMenu, I_GWL_MENUDATA);

      m_oiMenu = wsd.oiMenu;
      I_ASSERT(pMenuData);
      pMenuData->Update(m_oiMenu);
      }

    // get the current window position
    Dirty();
    m_fDirty = TRUE;
    FixWindowStyle();
    }
  m_layout->EnablePalettes(TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::EditBackground
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::EditBackground
  (
  )

  {
  m_layout->EnablePalettes(FALSE);
  if(m_fill.EditFill(m_layout->WndHandle(), "Edit Window Background"))
    {
    InvalidateRect(m_hwndClient, NULL, TRUE);
    m_fDirty = TRUE;
    }
  m_layout->EnablePalettes(TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::EditGrid
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::EditGrid
  (
  )

  {
  GRID_DLG  gd;

  GetGrid(&gd.ptGrid);
  gd.fUnits = m_layout->CurrentUnits();
  gd.oiWnd = m_oiWnd;
  gd.fShowGrid = ShowGrid();
  gd.fSnapToGrid = SnapToGrid();

  m_layout->EnablePalettes(FALSE);
  if(DialogBoxParam(g_hInstance, ID(DLG_GRID), m_layout->WndHandle(), OWLayGridDlgProc, (LPARAM)&gd))
    {
    // redraw the window for the grid points
    if(gd.fShowGrid || ShowGrid())
      InvalidateRect(m_hwndClient, NULL, TRUE);

    // set the default grid info
    m_layout->SetGrid(&gd.ptGrid);
    m_layout->SetShowGrid(gd.fShowGrid);
    m_layout->SetSnapToGrid(gd.fSnapToGrid);

    // set the grid info for this window
    SetGrid(&gd.ptGrid);
    SetShowGrid(gd.fShowGrid);
    SetSnapToGrid(gd.fSnapToGrid);

    // change the status bar (or sizes palette)
    }
  m_layout->EnablePalettes(TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::EditSize
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::EditSize
  (
  )

  {
  WNDSIZES_DLG  wsd;

  // get the current size and state of the window
  Dirty();

  // set the dialog variables
  wsd.style = Style();
  CopyRect(&wsd.rcClient, &m_rcInitClient);
  CopyPoint(&wsd.ptMin, &m_ptMinSize);
  CopyPoint(&wsd.ptMax, &m_ptMaxSize);
  CopyPoint(&wsd.ptScroll, &m_ptScrollSize);
  wsd.fLimit = m_fLimitMaxSize;
  wsd.fUnits = m_layout->CurrentUnits();

  m_layout->EnablePalettes(FALSE);
  if(DialogBoxParam(g_hInstance, ID(DLG_WNDSIZES), m_layout->WndHandle(), OWLayWndSizesDlgProc, (LPARAM)&wsd))
    {
    // save the changes
    CopyRect(&m_rcInitClient, &wsd.rcClient);
    CopyPoint(&m_ptMinSize, &wsd.ptMin);
    CopyPoint(&m_ptMaxSize, &wsd.ptMax);
    CopyPoint(&m_ptScrollSize, &wsd.ptScroll);
    m_fLimitMaxSize = wsd.fLimit;

    // update the object visually
    m_fDirty = TRUE;
    FixWindowStyle();
    }
  m_layout->EnablePalettes(TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::EditPreferences
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::EditPreferences
  (
  )

  {
  PREF_DLG  pd;

  pd.fLayPref = m_layout->CurrentUnits();
  if(m_layout->PrstWndPositions())
    pd.fLayPref |= LAYPREF_PRSTWNDPOSITIONS;
  if(m_layout->PrstTool())
    pd.fLayPref |= LAYPREF_PRSTTOOL;
  if(PrstMode())
    pd.fLayPref |= LAYPREF_PRSTMODE;
  else
    pd.fLayPref |= (short)(DefFullInfoMode() ? LAYPREF_FULLINFOMODE : LAYPREF_WYSIWYGMODE);

  if(ReqFullEnclosure())
    pd.fLayPref |= LAYPREF_REQFULLENCLOSURE;

  m_layout->EnablePalettes(FALSE);
  if(DialogBoxParam(g_hInstance, ID(DLG_PREFERENCES), m_layout->WndHandle(), OWLayPreferencesDlgProc, (LPARAM)&pd))
    {
    // set the preferences for the layout
    m_layout->SetPrstWndPositions((pd.fLayPref & LAYPREF_PRSTWNDPOSITIONS) ? TRUE : FALSE);
    m_layout->SetPrstTool((pd.fLayPref & LAYPREF_PRSTTOOL) ? TRUE : FALSE);
    m_layout->SetPrstMode((pd.fLayPref & LAYPREF_PRSTMODE) ? TRUE : FALSE);
    m_layout->SetFullInfoMode((pd.fLayPref & LAYPREF_FULLINFOMODE) ? TRUE : FALSE);
    m_layout->SetWYSIWYGMode((pd.fLayPref & LAYPREF_WYSIWYGMODE) ? TRUE : FALSE);
    m_layout->SetReqFullEnclosure((pd.fLayPref & LAYPREF_REQFULLENCLOSURE) ? TRUE : FALSE);
    if(pd.fLayPref & LAYPREF_INCHES)
      m_layout->SetCurrUnitsInches();
    else if(pd.fLayPref & LAYPREF_MILLIMETERS)
      m_layout->SetCurrUnitsMillimeters();
    else if(pd.fLayPref & LAYPREF_POINTS)
      m_layout->SetCurrUnitsPoints();

    // set the preferences for this window
    SetPrstMode((pd.fLayPref & LAYPREF_PRSTMODE) ? TRUE : FALSE);
    SetFullInfoMode((pd.fLayPref & LAYPREF_FULLINFOMODE) ? TRUE : FALSE);
    SetWYSIWYGMode((pd.fLayPref & LAYPREF_WYSIWYGMODE) ? TRUE : FALSE);
    SetReqFullEnclosure((pd.fLayPref & LAYPREF_REQFULLENCLOSURE) ? TRUE : FALSE);

    m_fDirty = TRUE;
    }
  m_layout->EnablePalettes(TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::ToggleFullInfo
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::ToggleFullInfo
  (
  )

  {
  if(m_fFullInfo)
    {
    m_fFullInfo = FALSE;
    if(!(m_layout->Shapes())->FullInfoTool())
      {
      m_items.ChangeFullInfo(FALSE);
      InvalidateRect(m_hwndClient, NULL, TRUE);
      }
    }
  else
    {
    m_fFullInfo = TRUE;
    if(!(m_layout->Shapes())->FullInfoTool())
      {
      m_items.ChangeFullInfo(TRUE);
      InvalidateRect(m_hwndClient, NULL, TRUE);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::DrawFrameMode
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

BOOL Window::DrawFrameMode
  (
  )

  {
  return (FullInfoMode() || (m_layout->Shapes())->FullInfoTool());
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::DrawGroupsMode
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

BOOL Window::DrawGroupsMode
  (
  )

  {
  short nTool = (m_layout->Shapes())->CurrentTool();

  return ((nTool==OSHP_STDTOOL_ANCHOR)
              ||
          (nTool==OSHP_STDTOOL_ARROW)
              ||
          (nTool==OSHP_STDTOOL_LOCK)
              ||
          (nTool==OSHP_STDTOOL_REORDER)
              ||
          (nTool==OSHP_STDTOOL_VISIBILITY));
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::DrawGrid
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::DrawGrid
  (
  HDC     hdc,
  LPRECT  lprc
  )

  {
  // draw the grid points as required
  if(ShowGrid())
    {
    POINT pt, ptGrid;
    GetGrid(&pt);
    GetGrid(&ptGrid);
    OWndLStoDS(m_oiWnd, &pt, 1);

    if(pt.x>1 && pt.y>1)
      {
      COLORREF  clr = RGB(0, 0, 0); // BLACK
      int       nOldROP = SetROP2(hdc, R2_NOT);
      int       i, j;

      i=(int)(lprc->top / ptGrid.y) * ptGrid.y;
      while(i < lprc->bottom)
        {
        j=(int)(lprc->left / ptGrid.x) * ptGrid.x;
        while(j < lprc->right)
          {
          SetPixel(hdc, j, i, clr);

          j += ptGrid.x;
          }

        i += ptGrid.y;
        }

      SetROP2(hdc, nOldROP);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::SwitchToFrameMode
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::SwitchToFrameMode
  (
  BOOL  fNewShowFrames,
  BOOL  fOldShowFrames
  )

  {
  if(fNewShowFrames || (fNewShowFrames != fOldShowFrames))
    InvalidateRect(m_hwndClient, NULL, TRUE);

  if(!FullInfoMode())
    m_items.ChangeFullInfo(fNewShowFrames);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::GetNextAddPoint
//
//  DESCRIPTION:  This will get a point which will work for double-click
//                adding items.
//
////////////////////////////////////////////////////////////////////////////

POINT Window::GetNextAddPoint
  (
  )

  {
  POINT pt;
  POINT ptDefault = {ITEM_DEFAULT_WIDTH, ITEM_DEFAULT_HEIGHT};
  RECT  rcClient;

  if(SnapToGrid())
    {
    POINT ptGrid;
    int   nSize;

    GetGrid(&ptGrid);

    if(ptGrid.x > 1)
      {
      nSize = 0;
      while(nSize < ptDefault.x)
        nSize += ptGrid.x;
      ptDefault.x = nSize;
      }

    if(ptGrid.y > 1)
      {
      nSize = 0;
      while(nSize < ptDefault.y)
        nSize += ptGrid.y;
      ptDefault.y = nSize;
      }
    }
  OWndLStoDS(m_oiWnd, &ptDefault, 1);

  // snap the point to the grid
  OWndDPtoLP(m_oiWnd, &m_ptNext, 1);
  SnapPointToGrid(&m_ptNext);
  OWndLPtoDP(m_oiWnd, &m_ptNext, 1);

  // make sure the drop point is visible
  GetClientRect(m_hwndClient, &rcClient);
  if(m_ptNext.y >= rcClient.bottom)
    {
    m_ptNext.y = 0;
    m_ptNext.x += ptDefault.x;
    }
  if(m_ptNext.x >= rcClient.right)
    m_ptNext.x = 0;

  // copy the drop point
  CopyPoint(&pt, &m_ptNext);

  // update the next drop point
  m_ptNext.y += ptDefault.y;

  return pt;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::PaintMenuWindow
//
//  DESCRIPTION:  This will paint the menu window.  This will draw a sample
//                menu.  This will change for future releases.
//
////////////////////////////////////////////////////////////////////////////

void Window::PaintMenuWindow
  (
  HWND  hwnd
  )

  {
  PAINTSTRUCT ps;
  HDC         hdc       = BeginPaint(hwnd, &ps);
  HBRUSH      hbr       = CreateSolidBrush(GetSysColor(COLOR_MENU));
  HBRUSH      hbrOld    = (HBRUSH)SelectObject(hdc, hbr);
  HPEN        hpen      = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_MENU));
  HPEN        hpenOld   = (HPEN)SelectObject(hdc, hpen);
  RECT        rc;
  POINT       pt;
  COLORREF    clrBG     = SetBkColor(hdc, GetSysColor(COLOR_MENU));
  COLORREF    clrFG     = SetTextColor(hdc, GetSysColor(COLOR_MENUTEXT));
  HFONT       hfont     = (HFONT)GetStockObject(SYSTEM_FONT); // no need to delete stock objects
  HFONT       hfontOld  = (HFONT)SelectObject(hdc, hfont);
  MenuData*   pMenuData = (MenuData*)GetWindowLong(hwnd, I_GWL_MENUDATA);

  GetClientRect(hwnd, &rc);
  pt.x = rc.right;
  pt.y = rc.bottom;

  // draw the sample menu
  if(pMenuData)
    {
    int   i       = 0;
    int   nCount  = pMenuData->NumMenus();
    BOOL  fFirst  = TRUE;

    rc.right = pMenuData->LeftOffset();
    rc.bottom = rc.top + GetSystemMetrics(SM_CYMENU);
    rc.top--;

    while(i < nCount)
      {
      rc.left = rc.right;
      rc.right = rc.left + pMenuData->TitleWidth(i);

      if(fFirst)
        {
        fFirst = FALSE;
        DrawText(hdc, pMenuData->Title(i), -1, &rc, DT_SINGLELINE | DT_VCENTER);
        i++;
        }
      else if(rc.right <= pt.x)
        {
        DrawText(hdc, pMenuData->Title(i), -1, &rc, DT_SINGLELINE | DT_VCENTER);
        i++;
        }
      else
        {
        fFirst = TRUE;

        rc.right = pMenuData->LeftOffset();
        rc.top = rc.bottom;
        rc.bottom = rc.top + GetSystemMetrics(SM_CYMENU) + 1;
        }
      }
    }

  // restore the device context to the original state
  SelectObject(hdc, hfontOld);
  SetBkColor(hdc, clrBG);
  SetTextColor(hdc, clrFG);
  SelectObject(hdc, hpenOld);
  DeleteObject(hpen);
  SelectObject(hdc, hbrOld);
  DeleteObject(hbr);
  EndPaint(hwnd, &ps);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::EraseBackground
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::EraseBackground
  (
  HDC hdc
  )

  {
  HBRUSH    hbr       = m_fill.m_CreateBrush();
  COLORREF  clrFG     = m_fill.FGColor();
  COLORREF  clrFGOld  = SetTextColor(hdc, clrFG);
  COLORREF  clrBG     = m_fill.BGColor();
  COLORREF  clrBGOld  = SetBkColor(hdc, clrBG);
  RECT      rc;
  POINT     ptOrg     = {0, 0};

  // get the update rect BEFORE doing the other stuff or this will NOT work!
  GetUpdateRect(m_hwndClient, &rc, FALSE);

  // convert the top left corner of the window to screen device points.
  OWndLPtoDP(m_oiWnd, &ptOrg, 1);
  MapWindowPoints(m_hwndClient, HWND_DESKTOP, &ptOrg, 1);

  // set the brush origin as the (0, 0) position -- which may not be visible
  UnrealizeObject(hbr);
  SetBrushOrgEx(hdc, ptOrg.x, ptOrg.y, NULL);
  FillRect(hdc, &rc, hbr);

  // draw the background grid as necessary
  DrawGrid(hdc, &rc);

  // restore the DC
  SetTextColor(hdc, clrFGOld);
  SetBkColor(hdc, clrBGOld);
  m_fill.m_DeleteBrush(hbr);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::Size
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::Size
  (
  HWND  hwnd,
  UINT  uState,
  int   nWidth,
  int   nHeight
  )

  {
  I_UNUSED(hwnd);
  I_UNUSED(nWidth);
  I_UNUSED(nHeight);

  I_ASSERT(m_hwndClient && m_hwndWindow && m_hwndMenu);

  switch(uState)
    {
    case SIZE_MAXIMIZED:
    case SIZE_RESTORED:
      MoveMenu();
      MoveClient();
      break;

    case SIZE_MINIMIZED:
    default:
      break;
    }

  // adjust layout window scroll bars as necessary
  m_layout->UpdateScrollRegion();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::Move
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::Move
  (
  HWND  hwnd,
  int   xPos,
  int   yPos
  )

  {
  I_UNUSED(hwnd);
  I_UNUSED(xPos);
  I_UNUSED(yPos);

  I_ASSERT(m_hwndClient && m_hwndWindow && m_hwndMenu);

  if(!IsIconic(m_hwndWindow))
    {
    MoveMenu();
    MoveClient();
    }

  // adjust layout window scroll bars as necessary
  m_layout->UpdateScrollRegion();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Window::GetMinMaxInfo
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Window::GetMinMaxInfo
  (
  MINMAXINFO* lpmmi
  )

  {
  POINT ptMax;
  POINT ptMin;
  RECT  rc;

  GetClientRect(m_layout->WndHandle(), &rc);
  CopyPoint(&ptMax, &m_ptMaxSize);
  OWndLStoDS(m_oiWnd, &ptMax, 1);
  CopyPoint(&ptMin, &m_ptMinSize);
  OWndLStoDS(m_oiWnd, &ptMin, 1);

  if(HasDialogFrame())
    {
    ptMax.x   += (2*GetSystemMetrics(SM_CXDLGFRAME));
    ptMax.y   += (2*GetSystemMetrics(SM_CYDLGFRAME));
    ptMin.x   += (2*GetSystemMetrics(SM_CXDLGFRAME));
    ptMin.y   += (2*GetSystemMetrics(SM_CYDLGFRAME));
    rc.right  += (2*GetSystemMetrics(SM_CXDLGFRAME));
    rc.bottom += (2*GetSystemMetrics(SM_CYDLGFRAME));

    SetPoint(&lpmmi->ptMaxPosition, -GetSystemMetrics(SM_CXDLGFRAME), -GetSystemMetrics(SM_CYDLGFRAME));
    }
  else if(HasResizeBorder())
    {
    ptMax.x   += (2*GetSystemMetrics(SM_CXFRAME));
    ptMax.y   += (2*GetSystemMetrics(SM_CYFRAME));
    ptMin.x   += (2*GetSystemMetrics(SM_CXFRAME));
    ptMin.y   += (2*GetSystemMetrics(SM_CYFRAME));
    rc.right  += (2*GetSystemMetrics(SM_CXFRAME));
    rc.bottom += (2*GetSystemMetrics(SM_CYFRAME));

    SetPoint(&lpmmi->ptMaxPosition, -GetSystemMetrics(SM_CXFRAME), -GetSystemMetrics(SM_CYFRAME));
    }
  else
    {
    ptMax.x   += 2;
    ptMax.y   += 2;
    ptMin.x   += 2;
    ptMin.y   += 2;
    rc.right  += 2;
    rc.bottom += 2;

    SetPoint(&lpmmi->ptMaxPosition, -1, -1);
    }
  if(HasVertScroll())
    {
    ptMax.x += (GetSystemMetrics(SM_CXVSCROLL)-1);
    ptMin.x += (GetSystemMetrics(SM_CXVSCROLL)-1);
    }
  if(HasHorzScroll())
    {
    ptMax.y += (GetSystemMetrics(SM_CYHSCROLL)-1);
    ptMin.y += (GetSystemMetrics(SM_CYHSCROLL)-1);
    }
  if(HasTitleBar())
    {
    ptMax.y += (GetSystemMetrics(SM_CYCAPTION)-1);
    ptMin.y += (GetSystemMetrics(SM_CYCAPTION)-1);
    }

  CopyPoint(&lpmmi->ptMinTrackSize, &ptMin);

  if(m_fLimitMaxSize)
    {
    CopyPoint(&lpmmi->ptMaxTrackSize, &ptMax);
    SetPoint(&lpmmi->ptMaxSize, nMin(ptMax.x, rc.right), nMin(ptMax.y, rc.bottom));
    }
  else
    {
    SetPoint(&lpmmi->ptMaxSize, rc.right, rc.bottom);
    SetPoint(&lpmmi->ptMaxTrackSize, 0x7FFF, 0x7FFF);
    }
  }

////////////////////////////////////////////////////////////////////////////
