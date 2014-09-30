////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          HIERLIST.CPP
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   Implements a hierarchical list of items window.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <memory.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>

#include <a_alm.h>
#include <intdebug.h>

#include "hierlist.h"
#include "xp_macro.h"

//////////////// Global variables
static char       s_szWndClass[]              = "ApwHierList";

//////////////// Class static variables
BOOL              SItemListWindow::s_fInitOK    = FALSE;
HINSTANCE         SItemListWindow::s_hInstance  = NULL;

//////////////// Local constants

const short       c_nItemHeightSpace            = 1;
const short       c_nDragDelta                  = 5;    //!!
const short       c_nRootXOffset                = 5;    //!!

// ****************************************************************
//
// SItemListWindow code
//
// ****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::SetMouseFlags
//
// DESCR:   Examines MK_* and returns appropriate flags.
//
////////////////////////////////////////////////////////////////

short   SItemListWindow::SetMouseFlags(
  POINT&  ptMouse,
  BOOL    fLeft,
  WPARAM  wParam,
  LPARAM  lParam
  )
  {
  short   fFlags;

  ptMouse.x = (COORDINATE)((short)LOWORD(lParam));
  ptMouse.y = (COORDINATE)((short)HIWORD(lParam));

  if (fLeft)
    fFlags = leftButton;
  else
    fFlags = rightButton;

  if (wParam & MK_CONTROL)
    fFlags |= ctrlKey;
  if (wParam & MK_SHIFT)
    fFlags |= shiftKey;

  return fFlags;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::WndProc
//
// DESCR:   This is the WindowProc for the list window.  It
//          forwards necessary messages on to the corresponding methods
//          on the object.
//
////////////////////////////////////////////////////////////////

LRESULT ALMAPI    SItemListWindow::WndProc(
  HWND    hWnd,
  UINT    nMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )
  {
  LRESULT             lRes        = 0;
  SItemListWindow*    pList;
  short               fFlags;

  pList = (SItemListWindow*) GetWindowLong(hWnd, 0);

  switch (nMsg)
    {
    case WM_CREATE:
      pList = (SItemListWindow*) (((LPCREATESTRUCT) lParam)->lpCreateParams);
      SetWindowLong(hWnd, 0, (LONG) pList);
      break;

    case WM_MOUSEACTIVATE:
      if (pList->m_lStatus & fDisabled)
        {
        MessageBeep(MB_OK);
        lRes = MA_NOACTIVATEANDEAT;
        }
      else
        {
        lRes = DefWindowProc(hWnd, nMsg, wParam, lParam);
        }
      break;

    case WM_SIZE:
      I_ASSERT(pList != NULL);
      lRes = HANDLE_WM_SIZE(hWnd, wParam, lParam, pList->Resize);
      break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
      {
      POINT   ptDown;

      I_ASSERT(pList != NULL);

      fFlags = SetMouseFlags(ptDown, (nMsg == WM_LBUTTONDOWN), wParam, lParam);

      lRes = pList->MouseDown(&ptDown, fFlags);
      break;
      }

    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
      {
      POINT   ptUp;

      I_ASSERT(pList != NULL);

      fFlags = SetMouseFlags(ptUp, (nMsg == WM_LBUTTONDBLCLK), wParam, lParam);

      lRes = pList->MouseDblClick(&ptUp, fFlags);
      break;
      }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
      {
      POINT   ptUp;

      I_ASSERT(pList != NULL);

      fFlags = SetMouseFlags(ptUp, (nMsg == WM_LBUTTONUP), wParam, lParam);

      lRes = pList->MouseUp(&ptUp, fFlags);
      break;
      }

    case WM_MOUSEMOVE:
      {
      POINT   ptMove;

      I_ASSERT(pList != NULL);

      fFlags = SetMouseFlags(ptMove, TRUE, wParam, lParam);

      lRes = pList->MouseMove(&ptMove, fFlags);
      break;
      }

    case WM_VSCROLL:
      I_ASSERT(pList != NULL);
      lRes = HANDLE_WM_VSCROLL(hWnd, wParam, lParam, pList->VScroll);
      break;

    case WM_PAINT:
      {
      PAINTSTRUCT   ps;
      HDC           hDC;

      I_ASSERT(pList != NULL);

      hDC = BeginPaint(hWnd, &ps);

      if (hDC)
        lRes = pList->Draw(hDC, &ps.rcPaint);

      EndPaint(hWnd, &ps);

      break;
      }

    case WM_SYSCOMMAND:
      if (wParam == SC_CLOSE)
        ShowWindow(hWnd, SW_HIDE);
      else
        lRes = DefWindowProc(hWnd, nMsg, wParam, lParam);

      break;

    default:
      lRes = DefWindowProc(hWnd, nMsg, wParam, lParam);
      break;
    }

  return lRes;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::Init
//
// DESCR:   Registers the hierarchical list window class with Windows
//
////////////////////////////////////////////////////////////////

BOOL    SItemListWindow::Init(    // Register class
  HINSTANCE   hInst
  )
  {
  WNDCLASS      wc;

  s_hInstance = hInst;

  wc.style          = CS_BYTEALIGNWINDOW | CS_DBLCLKS;
  wc.lpfnWndProc    = WndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = sizeof(SItemListWindow*);
  wc.hInstance      = s_hInstance;
  wc.hIcon          = NULL;
  wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
  wc.lpszMenuName   =  NULL;
  wc.lpszClassName  = s_szWndClass;

  s_fInitOK = RegisterClass(&wc);

  return s_fInitOK;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::SItemListWindow
//
// DESCR:   Initializes a new list item window.
//
////////////////////////////////////////////////////////////////

SItemListWindow::SItemListWindow(
  HWND            hOwner,
  LONG            lOptions,
  DWORD           dwCreateOptions
  ) :
    m_hListWindow(NULL),
    m_lStatus(0),
    m_lOptions(lOptions & ItemList_allowedOptions),
    m_pRootItem(NULL),
    m_pAnchorItem(NULL),
    m_pMouseDownItem(NULL),
    m_hDragCursor(NULL),
    m_hOldCursor(NULL),
    m_nFullCount(0),
    m_cxImages(0),
    m_cyImages(0),
    m_nImageSpace(0),
    m_nItemHeight(1),                   // so we don't divide by 0
    m_nMaxDisplay(0),
    m_nTopIndex(0),
    m_nMaxTopIndex(0),
    m_hFont(NULL),
    m_hBoldFont(NULL),
    m_nFontHeight(0),
    m_apDragTargets(NULL),
    m_apDragTargetCount(0),
    m_apDragTargetSize(0)
  {
  TEXTMETRIC  tm;
  HDC         hDC;

  ADBG_ASSERT_W(s_fInitOK, ;, "Creating an ItemListWindow when Init() failed.");

  if (s_fInitOK)
    {
    m_hListWindow = CreateWindow(s_szWndClass, NULL,
                                      dwCreateOptions,
                                      0, 0, 0, 0,
                                      hOwner, NULL, s_hInstance, this);

    I_ASSERT(m_hListWindow != NULL);

    if (m_hListWindow)
      {
      hDC = GetDC(NULL);              // get screen dc

      GetTextMetrics(hDC, &tm);

      m_nFontHeight = (short)(tm.tmHeight + tm.tmExternalLeading);
      m_nImageSpace = (short)tm.tmAveCharWidth;

      UpdateItemHeight();
      UpdateBoldFont(hDC);

      ReleaseDC(NULL, hDC);

      // Allocate root item
      m_pRootItem = new SListItem(this, 0);

      if (m_pRootItem)
        {
        if (m_lOptions & sortItems)
          m_pRootItem->SetSortable();

        m_pRootItem->VisitItems(SListItem::syncDraw, 0);
        m_pRootItem->Expand(TRUE);
        }
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::~SItemListWindow
//
// DESCR:   Cleans up a list window object.
//
////////////////////////////////////////////////////////////////

SItemListWindow::~SItemListWindow(
  void
  )
  {
  if (m_hFont)
    DeleteObject(m_hFont);

  if (m_hBoldFont)
    DeleteObject(m_hBoldFont);

  if (m_hListWindow)
    DestroyWindow(m_hListWindow);

  if (m_pRootItem)
    delete m_pRootItem;

  if (m_apDragTargets)
    delete [] (char*)m_apDragTargets;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::Flatten
//
// DESCR:   Flattens the hierarchy into a linearly ordered list.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::Flatten(
  void
  )
  {
  m_nFullCount = m_pRootItem->Flatten(-1);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::UpdateItemHeight
//
// DESCR:   Recalculates the item height of the items.  The item
//          height is the maximum of font height and image height.
//
////////////////////////////////////////////////////////////////

void  SItemListWindow::UpdateItemHeight(
  void
  )
  {
  if (m_nFontHeight > m_cyImages + 2 * c_nItemHeightSpace)
    m_nItemHeight = m_nFontHeight;
  else
    {
    // Add some spacing around it
    m_nItemHeight = (short)(m_cyImages + 2 * c_nItemHeightSpace);
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::UpdateBoldFont
//
// DESCR:   Creates a bold font from the one selected in the DC for
//          drawing items that have children.
//
////////////////////////////////////////////////////////////////

void  SItemListWindow::UpdateBoldFont(
  HDC   hDC
  )
  {
  HFONT     hOldFont;
  LOGFONT   lfFont;

  I_ASSERT(hDC != NULL);

  hOldFont = (HFONT) SelectObject(hDC, GetStockObject(SYSTEM_FONT));
  GetObject(hOldFont, sizeof(lfFont), &lfFont);
  SelectObject(hDC, hOldFont);

  lfFont.lfWeight = FW_BOLD;

  if (m_hBoldFont)
    DeleteObject(m_hBoldFont);

  m_hBoldFont = CreateFontIndirect(&lfFont);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::UpdateScroll
//
// DESCR:   Sets the scroll range and position based on the new count
//          and topindex.  The # of steps in the scrollbar will be
//          only the extra so that we always have a full window.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::UpdateScroll(
  void
  )
  {
  short     nPos;                   // new position of thumb.
  short     nDisplayed;             // number of full lines displayed
  int       nScrollMin;
  int       nScrollMax;
  BOOL      fNewRange;

  // If we have a partial line showing, subtract one from maxdisplay
  nDisplayed = m_nMaxDisplay;
  if (m_lStatus & partialLine)
    nDisplayed--;

  // The high value of the scrollbar is the difference between what we
  // can see and what we want to display
  m_nMaxTopIndex = (short)(m_nFullCount - nDisplayed);
  if (m_nMaxTopIndex < 0)
    m_nMaxTopIndex = 0;

  // Our current position is just the top index
  nPos = m_nTopIndex;

  // Sanity check:  if max top index is 0, then we can display everything, so
  // make sure top index is 0

  if (m_nMaxTopIndex == 0)
    m_nTopIndex = 0;

  GetScrollRange(m_hListWindow, SB_VERT, &nScrollMin, &nScrollMax);

  // I_ASSERT(nScrollMin == 0);   // this fails on the first Resize()

  fNewRange = (nScrollMax != m_nMaxTopIndex);

  SetScrollPos(m_hListWindow, SB_VERT, nPos, !fNewRange);

  if (fNewRange)
    SetScrollRange(m_hListWindow, SB_VERT, 0, m_nMaxTopIndex, TRUE);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::FindTargetWindow
//
// DESCR:   Searches the list of registered drag targets and returns
//          the index of it.  -1 is returned if the window is not
//          a registered target.
//
////////////////////////////////////////////////////////////////

short   SItemListWindow::FindTargetWindow(
  HWND    hTarget
  )
  {
  short   nIndex;

  for (nIndex = 0; nIndex < m_apDragTargetCount; nIndex++)
    {
    if (m_apDragTargets[nIndex] == hTarget)
      break;
    }

  if (nIndex >= m_apDragTargetCount)        // not found
    nIndex = -1;

  return nIndex;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::FindDragWindow
//
// DESCR:   Determines the window that the given point is over.  NULL
//          is returned if the window is not in the target drag list.
//          If the window the point is over has the useParent option on
//          it, then its parent window is used for the operations.  The
//          point passed in is modified to the equivalent client point
//          in the returned window.
//
////////////////////////////////////////////////////////////////

HWND    SItemListWindow::FindDragWindow(
  POINT*      ptOfWindow
  )
  {
  HWND        hDragWindow;
  RECT        rcClient;
  short       nDragIdx;

  I_ASSERT(ptOfWindow != NULL);

  ClientToScreen(m_hListWindow, ptOfWindow);
  hDragWindow = WindowFromPoint(*ptOfWindow);

  I_ASSERT(hDragWindow != NULL);

  if (hDragWindow == m_hListWindow)
    nDragIdx = 0;                       // let it through
  else
    {
    nDragIdx = FindTargetWindow(hDragWindow);

    // this assumes that the top-level window does not have attribute
    // useParent.
    while (nDragIdx != -1 && (m_apDragTargFlags[nDragIdx] & useParent))
      {
      hDragWindow = GetParent(hDragWindow);
      ADBG_ASSERT_E(hDragWindow != NULL, return NULL,
                      "useParent option requires a real HWND parent");

      nDragIdx = FindTargetWindow(hDragWindow);
      }
    }

  if (nDragIdx == -1)
    hDragWindow = NULL;
  else
    {
    ScreenToClient(hDragWindow, ptOfWindow);
    GetClientRect(hDragWindow, &rcClient);

    if (!PtInRect(&rcClient, *ptOfWindow))
      hDragWindow = NULL;
    }

  return hDragWindow;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::ChangeCursor
//
// DESCR:   Changes the drag cursor to the given one.  It destroys
//          the existing drag cursor if necessary and it remembers
//          the current cursor if this the first change since the
//          drag.
//
////////////////////////////////////////////////////////////////

void  SItemListWindow::ChangeCursor(
  HCURSOR   hNewCursor,
  BOOL      fDestroy
  )
  {
  HCURSOR   hOldCursor;
  
  hOldCursor = SetCursor(hNewCursor);

  if (hOldCursor != hNewCursor)
    {
    if (m_lStatus & destroyCursor)
      DestroyCursor(m_hDragCursor);

    if (m_hOldCursor == NULL)
      m_hOldCursor = hOldCursor;

    m_hDragCursor = hNewCursor;

    if (fDestroy)
      m_lStatus |= destroyCursor;
    else
      m_lStatus &= ~destroyCursor;
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::MouseDown
//
// DESCR:   Selects the item that the mouse is over.  The current algorithm
//          for selection and unselection is as follows:
//
//          If the shift key is down:
//            if there is an anchor item: unselect everything and
//              then select all between anchor and item.
//            if there is no anchor item: do nothing
//
//          Otherwise, if the shift key is not down, do the following:
//          If the user selects an unselected item, then:
//            if the ctrl key is down, then select that unselected item, else
//            if the ctrl key is NOT down, unselect everything, then select
//              that one.
//
//          If the user selects an already selected item, then:
//            if the ctrl key is down, then unselect that item on the
//              button up (since they might want to drag), else
//            if the ctrl key is NOT down, then unselect all items on
//              the button up (since they might want to drag all items).
//
//          Set the anchor if the shift key is NOT down.
//
////////////////////////////////////////////////////////////////

LRESULT   SItemListWindow::MouseDown(
  POINT*    ptDown,
  short     nFlags
  )
  {
  short         nFlat;
  SListItem*    pItem;
  BOOL          fSelected;
  BOOL          fCtrl;
  BOOL          fGreedy;
  BOOL          fShift;

  nFlat = MapYToFlatPosition((short)ptDown->y);

  pItem = m_pRootItem->FindItem(nFlat);

  // Record for later operations
  m_pMouseDownItem = pItem;

  if (pItem && ptDown->x >= pItem->LeftEdge())
    {
    m_ptDown = *ptDown;

    SetCapture(m_hListWindow);
    m_lStatus |= trackingMouse;

    fSelected = (pItem->Status() & SListItem::selected) == SListItem::selected;
    fCtrl     = (nFlags & ctrlKey)  == ctrlKey;
    fShift    = (nFlags & shiftKey) == shiftKey;
    fGreedy   = (pItem->Status() & SListItem::greedySelect) == SListItem::greedySelect;

    if (fShift)
      {
      if (m_pAnchorItem != NULL)    // anchor cannot be greedy, so this is okay
        {
        short       nAnchorFlat;
        short       nItemFlat;
        short       nLow;
        short       nHigh;
        SListItem*  pTemp;

        // This will cause unnecessary flash.  This should be
        // optimized eventually.
        m_pRootItem->VisitItems(SListItem::clearSelection, 0);  // invalidates
        
        nAnchorFlat = m_pAnchorItem->FlatPosition();
        nItemFlat   = pItem->FlatPosition();

        nLow  = min(nAnchorFlat, nItemFlat);
        nHigh = max(nAnchorFlat, nItemFlat);

        for (; nLow <= nHigh; nLow++)
          {
          pTemp = m_pRootItem->FindItem(nLow);

          pTemp->Select(TRUE);
          pTemp->Invalidate(FALSE);
          m_lStatus |= selectionChange;
          }

        // Can't have greedy items on
        m_pRootItem->VisitItems(SListItem::clearGreedy, 0);     // invalidates

        }
      }

    else if (!fSelected)
      {
      if (!fCtrl || fGreedy)          // unselect others
        m_pRootItem->VisitItems(SListItem::clearSelection, 0);  // invalidates
      else                            // unselect greedy items
        m_pRootItem->VisitItems(SListItem::clearGreedy, 0);     // invalidates
      
      if (pItem->Select(TRUE))
        m_lStatus |= selectionChange;
      pItem->Invalidate(FALSE);
      }
    else                              // possibly a drag will occur
      {
      if (fCtrl)                      // unselect current one if not dragged
        m_lStatus |= unselectOnUp;
      else
        m_lStatus |= unselectOthersOnUp;
      }

    if (!fShift)
      {
      // Cannot anchor on greedy items
      if (!fGreedy)
        m_pAnchorItem = pItem;
      else
        m_pAnchorItem = NULL;
      }
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::MouseDblClick
//
// DESCR:   Expands or contracts an item.  The item manipulated is the one
//          selected on the button down.  Any item pointed at during button
//          down will remain selected by the algorithm, so we can expand it
//          here.
//
////////////////////////////////////////////////////////////////

LRESULT   SItemListWindow::MouseDblClick(
  POINT*    ptDown,
  short     nFlags
  )
  {
  BOOL          fChanged    = TRUE;
  short         nTop;

  I_UNUSED(ptDown);
  I_UNUSED(nFlags);

  if (m_pMouseDownItem)
    {
    if (m_pMouseDownItem->Status() & SListItem::expanded)
      m_pMouseDownItem->Expand(FALSE);
    else if (m_pMouseDownItem->Status() & SListItem::hasChildren)
      m_pMouseDownItem->Expand(TRUE);
    else
      fChanged = FALSE;

    if (fChanged)
      {
      Flatten();                  //!! optimize:  check for a different count

      nTop = m_nTopIndex;
      UpdateScroll();

      if (nTop == m_nTopIndex)
        m_pMouseDownItem->Invalidate(TRUE);
      else
        InvalidateRect(m_hListWindow, NULL, TRUE);
      }
    else
      ItemDblClicked(m_pMouseDownItem);
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::MouseMove
//
// DESCR:   Handles mouse move actions.  If we are tracking the mouse, then
//          we check for a delta for the drag.
//
////////////////////////////////////////////////////////////////

LRESULT   SItemListWindow::MouseMove(
  POINT*    ptMove,
  short     nFlags
  )
  {
  BOOL      fDestroy;
  LONG      lCount;
  POINT     ptMouse;
  short     nMsg;
  HCURSOR   hNewCursor;
  HWND      hDragWindow;

  I_UNUSED(nFlags);

  if ((m_lOptions & allowDragDrop) && (m_lStatus & trackingMouse))
    {
    if (!(m_lStatus & (wasDragged | nonDraggable)))
      {
      if ((abs(m_ptDown.x - ptMove->x) > c_nDragDelta) ||
          (abs(m_ptDown.y - ptMove->y) > c_nDragDelta))
        {
        // Count # of selected items
        lCount = 0;
        m_pRootItem->VisitItems(SListItem::count, SListItem::expanded,
                                  (short) (SListItem::selected), &lCount);

        I_ASSERT(lCount > 0);

        if (lCount == 1)    // might be non-draggable
          {
          if ((m_pMouseDownItem == NULL) ||
              !(m_pMouseDownItem->Status() & SListItem::nonDraggable))
            m_lStatus |= (singleItemDrag | wasDragged);
          else
            m_lStatus |= nonDraggable;
          }
        else
          m_lStatus |= wasDragged;
        }
      }

    if (m_lStatus & wasDragged)
      {
      // Now check for drag targets
      ptMouse = *ptMove;

      hDragWindow = FindDragWindow(&ptMouse);

      if (hDragWindow)
        {
        nMsg = (short)((hDragWindow == m_hListWindow) ? sourceOfDrag : targetForDrag);

        if (m_lStatus & singleItemDrag)
          fDestroy = GetCursor(nMsg, m_pMouseDownItem, hNewCursor);
        else
          fDestroy = GetCursor(nMsg, NULL, hNewCursor);
        }
      else
        {
        fDestroy = GetCursor(invalidTarget, NULL, hNewCursor);
        }

      ChangeCursor(hNewCursor, fDestroy);
      }
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::MouseUp
//
// DESCR:   Handles mouse up actions.  If the user did not drag, then
//          this operation will complete the unselect operation started
//          during the MouseDown().
//
////////////////////////////////////////////////////////////////

LRESULT   SItemListWindow::MouseUp(
  POINT*    ptUp,
  short     nFlags
  )
  {
  POINT     ptDragUp;
  HWND      hDragWindow;

  I_UNUSED(nFlags);

  if (m_lStatus & trackingMouse)
    ReleaseCapture();

  if (m_lStatus & wasDragged)
    {
    I_ASSERT(m_hOldCursor != NULL);

    ChangeCursor(m_hOldCursor, FALSE);

    m_hDragCursor  = NULL;
    m_hOldCursor   = NULL;

    ptDragUp = *ptUp;

    hDragWindow = FindDragWindow(&ptDragUp);

    if (hDragWindow && (hDragWindow != m_hListWindow))
      {
      // Notify anyone whose interested in the actual drop operation
      DragRelease(hDragWindow, ptDragUp);
      }
    }

  else if (m_lStatus & unselectOnUp)
    {
    I_ASSERT(m_pMouseDownItem != NULL);

    if (m_pMouseDownItem->Select(FALSE))
      m_lStatus |= selectionChange;
    m_pMouseDownItem->Invalidate(FALSE);
    }

  else if (m_lStatus & unselectOthersOnUp)
    {
    I_ASSERT(m_pMouseDownItem != NULL);

    m_pRootItem->VisitItems(SListItem::clearSelection, 0);  // invalidates

    // re-select ourself
    if (m_pMouseDownItem->Select(TRUE))
      m_lStatus |= selectionChange;
    m_pMouseDownItem->Invalidate(FALSE);
    }

  if (m_lStatus & selectionChange)
    SelectionChange();

  // Clear out any temporary states
  m_lStatus &= ~(trackingMouse | unselectOnUp | unselectOthersOnUp |
                  wasDragged | destroyCursor | singleItemDrag |
                  selectionChange | nonDraggable);

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::Resize
//
// DESCR:   Recalculates the max # of items displayed, etc. for a resize.
//
////////////////////////////////////////////////////////////////

LRESULT   SItemListWindow::Resize(
  HWND      hwnd,
  UINT      state,
  int       cx,
  int       cy
  )
  {
  I_UNUSED(hwnd);
  I_UNUSED(state);
  I_UNUSED(cx);

  short     nTopIndex;

  m_nMaxDisplay = (short)(cy / ItemHeight());

  // Round up if any portion of a line is showing.
  if (cy % ItemHeight() != 0)
    {
    m_nMaxDisplay++;
    m_lStatus |= partialLine;           // remember this for UpdateScroll()
    }
  else
    {
    m_lStatus &= ~partialLine;
    }

  nTopIndex = m_nTopIndex;

  if (m_hListWindow)                    // we get a size before we return from createwindow()
    {
    // Record old top to see if we need to invalidate everything
    UpdateScroll();
    if (nTopIndex != m_nTopIndex)
      InvalidateRect(m_hListWindow, NULL, TRUE);
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::Draw
//
// DESCR:   Draws all items in the list that are in the invalid rect.
//
////////////////////////////////////////////////////////////////

LRESULT   SItemListWindow::Draw(
  HDC       hDC,
  RECT*     prcInvalid
  )
  {
  short     nFirstInvalid;
  short     nLastInvalid;
  HFONT     hOldFont;

  I_ASSERT(prcInvalid != NULL);
  I_ASSERT(prcInvalid->top <= prcInvalid->bottom);

  // Select in new font
  if (m_hFont)
    hOldFont = (HFONT) SelectObject(hDC, m_hFont);

  // Calculate rows that are invalid

  nFirstInvalid = MapYToFlatPosition((short)prcInvalid->top);
  nLastInvalid  = MapYToFlatPosition((short)prcInvalid->bottom);

  if (nLastInvalid > m_nFullCount)
    nLastInvalid = m_nFullCount;

  if (nFirstInvalid <= m_nFullCount)
    m_pRootItem->Draw(hDC, nFirstInvalid, nLastInvalid, prcInvalid);

  if (m_hFont && hOldFont)
    SelectObject(hDC, hOldFont);

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::VScroll
//
// DESCR:   Handles the WM_VSCROLL message by positioning the topIndex at
//          the appropriate place.  If possible, the window contents are
//          scrolled rather than invalidated for efficiency.
//
////////////////////////////////////////////////////////////////

LRESULT   SItemListWindow::VScroll(
  HWND      hwnd,
  HWND      hwndCtl,
  UINT      uCode,
  int       nPos
  )
  {
  I_UNUSED(hwnd);
  I_UNUSED(hwndCtl);

  short     nNewTop           = m_nTopIndex;
  short     dy;

  // For right now, just set top index and invalidate the whole window

  switch (uCode)
    {
    case SB_TOP:
      nNewTop = 0;
      break;

    case SB_BOTTOM:
      nNewTop = m_nMaxTopIndex;
      break;

    case SB_LINEDOWN:
      if (nNewTop < m_nMaxTopIndex)
        nNewTop++;
      break;

    case SB_LINEUP:
      if (nNewTop > 0)
        nNewTop--;
      break;

    case SB_PAGEDOWN:
      nNewTop += m_nMaxDisplay;
      if (nNewTop > m_nMaxTopIndex)
        nNewTop = m_nMaxTopIndex;
      break;

    case SB_PAGEUP:
      nNewTop -= m_nMaxDisplay;
      if (nNewTop < 0)
        nNewTop = 0;
      break;

    case SB_THUMBPOSITION:
      nNewTop = (short)nPos;
      break;

    case SB_THUMBTRACK:                         // too hard to keep up
    default:
      break;
    }

  if (nNewTop != m_nTopIndex)                   // we must change display
    {
    if (abs(nNewTop - m_nTopIndex) > m_nMaxDisplay)
      {
      InvalidateRect(m_hListWindow, NULL, TRUE);
      }

    else                                        // we just scroll window
      {
      dy = (short)(ItemHeight() * (m_nTopIndex - nNewTop));
      ScrollWindow(m_hListWindow, 0, dy, NULL, NULL);
      }

    m_nTopIndex = nNewTop;
    UpdateScroll();
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::GetCursor
//
// DESCR:   Default drag cursor handler.  A standard cursor is used.
//          A return of FALSE says that it should not be destroyed.
//
////////////////////////////////////////////////////////////////

BOOL    SItemListWindow::GetCursor(
  short       nMsg,
  SListItem*  pItem,        // NULL if multi
  HCURSOR&    hCursor
  )
  {
  I_UNUSED(pItem);

  switch (nMsg)
    {
    case sourceOfDrag:
      hCursor = LoadCursor(NULL, IDC_ICON);
      break;

    case targetForDrag:
      hCursor = LoadCursor(NULL, IDC_UPARROW);
      break;

    case invalidTarget:
      hCursor = LoadCursor(NULL, IDC_CROSS);
      break;
    }

  return FALSE;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::DragRelease
//
// DESCR:   Processes the mouse up message after a drag operation
//          has occurred.  For any real processing to happen, this
//          method must be overridden.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::DragRelease(
  HWND    hDroppedOn,               // window mouse was released over
  POINT   ptDropPoint               // relative to hDroppedOn
  )
  {
  I_UNUSED(hDroppedOn);
  I_UNUSED(ptDropPoint);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::SelectionChange
//
// DESCR:   Notifies the system that an item was selected or unselected
//          since the last button up.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::SelectionChange(
  void
  )
  {
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::ItemDblClicked
//
// DESCR:   Notifies the system that an item was double clicked.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::ItemDblClicked(
  SListItem*  pItem
  )
  {
  I_UNUSED(pItem);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::ItemIsVisible
//
// DESCR:   Returns TRUE if the flat position is visible in the window.
//
////////////////////////////////////////////////////////////////

BOOL    SItemListWindow::ItemIsVisible(
  short     nFlat
  )
  {
  return (nFlat >= m_nTopIndex && nFlat <= (m_nTopIndex + m_nMaxDisplay));
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::SetFont
//
// DESCR:   Changes the font that is used to display the item's title.  The
//          font should NOT be bold (currently) since that is a cue that
//          there are children under the item.
//
////////////////////////////////////////////////////////////////

BOOL    SItemListWindow::SetFont(
  LOGFONT*    pLogFont
  )
  {
  BOOL        fSet    = FALSE;
  HDC         hDC;
  TEXTMETRIC  tm;

  ADBG_ASSERT_E(pLogFont != NULL, return FALSE, "Invalid logfont in SetFont");
  ADBG_ASSERT_W(pLogFont->lfWeight < FW_BOLD, ;, "Bold font passed to Setfont");

  hDC = GetDC(NULL);

  if (m_hFont)
    {
    I_ASSERT(m_hBoldFont != NULL);

    DeleteObject(m_hFont);
    DeleteObject(m_hBoldFont);
    }

  m_hFont = CreateFontIndirect(pLogFont);

  GetTextMetrics(hDC, &tm);

  m_nFontHeight = (short)(tm.tmHeight + tm.tmExternalLeading);

  UpdateItemHeight();
  UpdateBoldFont(hDC);

  ReleaseDC(NULL, hDC);

  return fSet;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::MapFlatPositionToY
//
// DESCR:   Returns the item's position in pixels from the top of the window.
//          It returns -1 if the item is not visible.
//
////////////////////////////////////////////////////////////////

short   SItemListWindow::MapFlatPositionToY(
  short     nFlat
  )
  {
  short     nPixelPos;

  I_ASSERT(ItemHeight() != 0);

  if (ItemIsVisible(nFlat))
    nPixelPos = (short)((nFlat - m_nTopIndex) * ItemHeight());
  else
    nPixelPos = -1;

  return nPixelPos;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::MapYToFlatPosition
//
// DESCR:   Given a Y pixel position, return the flattened position that
//          it is within.
//
////////////////////////////////////////////////////////////////

short     SItemListWindow::MapYToFlatPosition(
  short     yCoord
  )
  {
  I_ASSERT(ItemHeight() != 0);
  I_ASSERT(yCoord >= 0);
//!! Add another debug test for <= client.bottom

  return (short)(yCoord / ItemHeight() + m_nTopIndex);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::AddItem
//
// DESCR:   Adds a new item to the list window.  The item can be added
//          at any point in the already-created hierarchy.  This is
//          accomplished by passing in an array of UIDs (i.e., LONGs)
//          and a count of the UIDs to the parent that the new item
//          should be inserted under.  To create a new parent, it new
//          item MUST have a unique UID at that level.
//
////////////////////////////////////////////////////////////////

BOOL    SItemListWindow::AddItem(
  SListItem*    pItem,
  LONG*         plPath,
  short         nPathLength
  )
  {
  SListItem*    pParent;              // parent item was added to
  BOOL          fAdded;
  short         nOldCount;            // flattened # of visible items before add

  ADBG_ASSERT_E(pItem != NULL, return FALSE, "pItem is NULL in AddItem");
  ADBG_ASSERT_E(nPathLength == 0 || plPath != NULL, return FALSE,
                "non-null pathlength with null path");

  I_ASSERT(m_pRootItem != NULL);

  fAdded = m_pRootItem->Add(pItem, plPath, nPathLength, pParent);

  if (fAdded)
    {
    // Record or test bitmap size
    if (pItem->Status() & SListItem::hasBitmap)
      {
      if (m_cxImages == 0)
        {
        m_cxImages = pItem->ImageWidth();
        m_cyImages = pItem->ImageHeight();

        UpdateItemHeight();
        }
      else
        {
        ADBG_ASSERT_W(m_cxImages == pItem->ImageWidth(), ;, "Inconsistent image size");
        ADBG_ASSERT_W(m_cyImages == pItem->ImageHeight(), ;, "Inconsistent image size");
        }
      }

    nOldCount = m_nFullCount;
    Flatten();                            //!! this might be optimized by
                                          //   flattening only from pParent on
    if (nOldCount != m_nFullCount)
      {
      if (ItemIsVisible(pParent->FlatPosition()))
        pParent->Invalidate(TRUE);          // invalidates from that on down

      UpdateScroll();
      }
    }

  return fAdded;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::ExpandAll
//
// DESCR:   Expands or collapses all items in the hierarchy.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::ExpandAll(
  BOOL    fExpand
  )
  {
  I_ASSERT(m_pRootItem != NULL);

  if (fExpand)
    m_pRootItem->VisitItems(SListItem::expandAll, 0);
  else
    m_pRootItem->VisitItems(SListItem::collapseAll, 0);

  Flatten();

  InvalidateRect(m_hListWindow, NULL, TRUE);

  UpdateScroll();
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::DeleteAll
//
// DESCR:   Removes all items from the hierarchy.  It preserves
//          the root node.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::DeleteAll(
  void
  )
  {
  I_ASSERT(m_pRootItem != NULL);

  m_pRootItem->DeleteChildren();

  InvalidateRect(m_hListWindow, NULL, TRUE);

  UpdateScroll();
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::AddDragTarget
//
// DESCR:   Adds another drag target window to the list of windows
//          to be sent messages during and after the drag.
//
////////////////////////////////////////////////////////////////

BOOL    SItemListWindow::AddDragTarget(
  HWND    hDragTarget,
  short   fDragTargetFlags
  )
  {
  BOOL    fAdded      = FALSE;
  short   nNewSize;
  HWND*   pNewHwndAry;
  short*  pNewFlagAry;

  ADBG_ASSERT_E(hDragTarget != NULL, return FALSE, "Invalid HWND in AddDragTarget");

  if (m_apDragTargetSize <= m_apDragTargetCount)        // grow array
    {
    nNewSize = (short)(((m_apDragTargetSize / 10) + 1) * 10);
    pNewHwndAry  = (HWND*)  new char[nNewSize * sizeof(HWND)];
    pNewFlagAry  = (short*) new char[nNewSize * sizeof(short)];

    if (pNewHwndAry && pNewFlagAry)
      {
      if (m_apDragTargets)
        {
        COPYMEMORY(pNewHwndAry, m_apDragTargets, sizeof(HWND) * m_apDragTargetSize);
        delete [] (char*)m_apDragTargets;
        }

      if (m_apDragTargFlags)
        {
        COPYMEMORY(pNewFlagAry, m_apDragTargFlags, sizeof(short) * m_apDragTargetSize);
        delete [] (char*)m_apDragTargFlags;
        }

      m_apDragTargets    = pNewHwndAry;
      m_apDragTargFlags  = pNewFlagAry;
      m_apDragTargetSize = nNewSize;
      }
    }

  if (m_apDragTargetCount < m_apDragTargetSize)   // add item
    {
    m_apDragTargets[m_apDragTargetCount]   = hDragTarget;
    m_apDragTargFlags[m_apDragTargetCount] = (short)(fDragTargetFlags & ItemList_allowedDragFlags);

    m_apDragTargetCount++;

    fAdded = TRUE;
    }

  return fAdded;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::RemoveDragTarget
//
// DESCR:   Removes the given drag target window from the list of windows
//          to be sent messages during and after the drag.
//
////////////////////////////////////////////////////////////////

BOOL    SItemListWindow::RemoveDragTarget(
  HWND    hDragTarget
  )
  {
  BOOL    fRemoved      = FALSE;
  short   nIdx;

  ADBG_ASSERT_E(hDragTarget != NULL, return FALSE, "Invalid HWND in RemoveDragTarget");

  for (nIdx = 0; nIdx < m_apDragTargetCount; nIdx++)
    {
    if (m_apDragTargets[nIdx] == hDragTarget)
      {
      m_apDragTargetCount--;
      m_apDragTargets[nIdx] = m_apDragTargets[m_apDragTargetCount];

      fRemoved = TRUE;
      }
    }

  return fRemoved;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::EnumItems
//
// DESCR:   Enumerates the items matching the given status.  The
//          enumeration does NOT descend into non-expanded items
//          since the flat position of those are incorrect.
//
////////////////////////////////////////////////////////////////

SListItem*    SItemListWindow::EnumItems(
  short&          nKey,                 // set to -1 at first
  short           nStatus
  )
  {
  SListItem*      pItem   = NULL;
  SListItem*      pSearch;

  if (nKey < 0)
    nKey = -1;

  for (nKey++; nKey < m_nFullCount; nKey++)
    {
    pSearch = m_pRootItem->FindItem(nKey);
    if (pSearch->Status() & nStatus)
      {
      pItem = pSearch;
      break;
      }
    }

  return pItem;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SItemListWindow::Enable
//
// DESCR:   Enables or disables the window.  If it is disabled,
//          all WM_MOUSEACTIVATE messages are eaten.
//
////////////////////////////////////////////////////////////////

void    SItemListWindow::Enable(
  BOOL    fEnable
  )
  {
  if (fEnable)
    {
    m_lStatus &= ~fDisabled;
    }
  else
    {
    m_lStatus |= fDisabled;
    }
  }

// ****************************************************************
//
// SListItem code
//
// ****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::SListItem
//
// DESCR:   Initializes a new list item.
//
////////////////////////////////////////////////////////////////

SListItem::SListItem(
  SItemListWindow*    pMaster,
  LONG                lUserType
  ) :
    m_pMaster(pMaster),
    m_pParent(NULL),
    m_lUserType(lUserType),
    m_fStatus(rootItem | exists),         // assume root until added
    m_lUID(0),
    m_nFlatPosition(0),
    m_apSubItems(NULL),
    m_apSubItemCount(0),
    m_apSubItemSize(0),
    m_pszTitle(NULL),
    m_hImage(NULL),
    m_cxImage(0),
    m_cyImage(0),
    m_nXOffset(0)
  {
  ADBG_ASSERT_E(pMaster != NULL, ;, "pMaster is null in SListItem ctor");
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::~SListItem
//
// DESCR:   Cleans up a list item.  We do NOT delete the bitmap here since
//          we do not own it (the user of the item list window does).
//
////////////////////////////////////////////////////////////////

SListItem::~SListItem(
  void
  )
  {
  if (m_pszTitle)
    delete [] m_pszTitle;

  DeleteChildren();

  delete [] m_apSubItems;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::DeleteChildren
//
// DESCR:   Deletes all children of this node.
//
////////////////////////////////////////////////////////////////

void    SListItem::DeleteChildren(
  void
  )
  {
  short     nIdx;

  for (nIdx = 0; nIdx < m_apSubItemCount; nIdx++)
    {
    delete m_apSubItems[nIdx];
    m_apSubItems[nIdx] = NULL;
    }

  m_apSubItemCount = 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::Sort
//
// DESCR:   Sorts the list of items alphabetically.  Speed is probably
//          not an issue, so a slow sort is okay.
//
////////////////////////////////////////////////////////////////

void    SListItem::Sort(        // Use a selection sort
  void
  )
  {
  short       nIdx1;            // current insert point
  short       nIdx2;            // current test point
  short       nMin;             // index of min element
  SListItem*  pMin;             // pointer to min element

  for (nIdx1 = 0; nIdx1 < m_apSubItemCount; nIdx1++)
    {
    // scan from nIdx1 + 1 looking for smallest
    nMin = nIdx1;
    pMin = m_apSubItems[nMin];

    for (nIdx2 = (short)(nIdx1 + 1); nIdx2 < m_apSubItemCount; nIdx2++)
      {
      if (lstrcmp(pMin->Title(), m_apSubItems[nIdx2]->Title()) > 0)
        {
        nMin = nIdx2;
        pMin = m_apSubItems[nMin];
        }
      }

    m_apSubItems[nMin]  = m_apSubItems[nIdx1];
    m_apSubItems[nIdx1] = pMin;
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::SetupDraw
//
// DESCR:   Calculates XOffset for all items.  It is assumed that
//          this will be called again if images or other associated
//          values change.
//
////////////////////////////////////////////////////////////////

void    SListItem::SetupDraw(
  void
  )
  {
  short     nParentTextLoc;

  if (Status() & rootItem)              // we still need to have reasonable XOffset
    {
    m_nXOffset = c_nRootXOffset;
    }

  else
    {
    nParentTextLoc = m_pParent->XOffset();

    if (m_hImage != NULL)
      nParentTextLoc += (short)(m_cxImage + m_pMaster->ImageSpace());
    else if (m_pParent->m_hImage == NULL)
      nParentTextLoc += m_pMaster->ImageSpace();

    m_nXOffset = nParentTextLoc;
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::LeftEdge
//
// DESCR:   Returns the left edge of the item.
//
////////////////////////////////////////////////////////////////

short     SListItem::LeftEdge(
  void
  )
  {
  short     nLeft;

  if (Image() != NULL && m_pParent != NULL)
    nLeft = m_pParent->XOffset();
  else
    nLeft = XOffset();

  return nLeft;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::SetTitle
//
// DESCR:   Copies the title into private memory.  This must be deleted in
//          the destructor.
//
////////////////////////////////////////////////////////////////

void    SListItem::SetTitle(
  char*   pszTitle
  )
  {
  size_t  nLen;

  ADBG_ASSERT_E(pszTitle != NULL, return, "Null Title passed to SetTitle");

  nLen = lstrlen(pszTitle);

  if ((m_pszTitle != NULL) && (nLen > lstrlen(m_pszTitle)))
    {
    delete [] m_pszTitle;
    m_pszTitle = NULL;
    }

  if (m_pszTitle == NULL)
    m_pszTitle = new char[nLen + 1];

  if (m_pszTitle != NULL)
    lstrcpy(m_pszTitle, pszTitle);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::SetImage
//
// DESCR:   Remembers the reference to the bitmap image.  The ownership
//          is NOT transferred, so the caller MUST delete the bitmap
//          when the window is no longer used.  NULL can be passed in
//          if the image should be removed.
//
////////////////////////////////////////////////////////////////

void    SListItem::SetImage(
  HBITMAP   hImage
  )
  {
  BITMAP    biInfo;

  ADBG_ASSERT_E(m_pParent == NULL, return, "Cannot set image after insertion into list");

  if (hImage == NULL)
    {
    m_hImage  = NULL;
    m_cxImage = 0;
    m_cyImage = 0;

    m_fStatus &= ~hasBitmap;
    }
  else
    {
    m_hImage = hImage;

    GetObject(m_hImage, sizeof(biInfo), &biInfo);

    m_cxImage = (short)biInfo.bmWidth;
    m_cyImage = (short)biInfo.bmHeight;

    m_fStatus |= hasBitmap;
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::Flatten
//
// DESCR:   Assigns a flattened position number to this item and to
//          all expanded items that it contains.
//
////////////////////////////////////////////////////////////////

short   SListItem::Flatten(
  short   nBase
  )
  {
#ifdef ADBG_DEBUG
  static short      s_nDepth  = 0;
#endif
  short             nIdx;

  ADBG_EVAL(
    s_nDepth++;
    if (s_nDepth > 50)
      ADbgPrint(ADBG_WARNING, 0, "Exceeded reasonable recursive depth");
    );

  m_nFlatPosition = nBase++;                  // "++" because we use one

  if (Status() & expanded)
    {
    for (nIdx = 0; nIdx < m_apSubItemCount; nIdx++)
      nBase = (m_apSubItems[nIdx])->Flatten(nBase);
    }

#ifdef ADBG_DEBUG
  s_nDepth--;
#endif

  return nBase;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::FindItem
//
// DESCR:   Searches for the item with the given flattened position.  NULL
//          is returned if not found.
//
////////////////////////////////////////////////////////////////

SListItem*    SListItem::FindItem(
  short         nFlatPos
  )
  {
  SListItem*    pItem   = NULL;
  short         nLow;
  short         nHigh;              // one above the last valid search entry
  short         nMid;
  short         nItemFlat;

  if (m_nFlatPosition < nFlatPos)   // don't look if key is before this one
    {
    // perform a binary search on the items
    nLow  = 0;
    nHigh = m_apSubItemCount;

    while (nLow < nHigh)
      {
      nMid = (short)((nLow + nHigh) / 2);

      nItemFlat = m_apSubItems[nMid]->FlatPosition();

      if (nItemFlat == nFlatPos)
        {
        pItem = m_apSubItems[nMid];
        break;
        }
      else if (nItemFlat < nFlatPos)
        nLow  = (short)(nMid + 1);
      else
        nHigh = nMid;
      }

    if (pItem == NULL)              // try descending into the subitems
      {
      I_ASSERT(nLow == nHigh);
      I_ASSERT(nLow <= m_apSubItemCount);

      // All items whose index is below nHigh (or nLow since nLow==nHigh)
      // have a position less than nFlatPos.  We still need to check all
      // of its children, however
      if (nLow > 0)
        pItem = m_apSubItems[nLow - 1]->FindItem(nFlatPos);
      }
    }

  return pItem;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::VisitItems
//
// DESCR:   Visits items in this portion of the hierarchy, whether
//          collapsed or not.  The message is sent to all children that
//          match the given status (all if status is 0).
//
////////////////////////////////////////////////////////////////

void CDECL    SListItem::VisitItems(
  short         nMsg,
  LONG          lStatus,
  ...
  )
  {
  short     nIdx;
  va_list   va;
  short     nStatus;
  LONG*     pCount;


  va_start(va, lStatus);

  switch (nMsg)
    {
    case clearSelection:              // clears and invalidates
      if ((Status() & selected))
        {
        m_fStatus &= ~selected;

        if (m_pMaster->ItemIsVisible(m_nFlatPosition))
          Invalidate(FALSE);
        }
      break;

    case clearGreedy:                 // clears and invalidates if greedy
      if ((Status() & (selected | greedySelect)) == (selected | greedySelect))
        {
        m_fStatus &= ~selected;

        if (m_pMaster->ItemIsVisible(m_nFlatPosition))
          Invalidate(FALSE);
        }
      break;

    case count:
      nStatus = va_arg(va, short);
      pCount  = va_arg(va, LONG*);

      I_ASSERT(pCount != 0);

      if (Status() & nStatus)
        (*pCount)++;
      break;

    case expandAll:
      Expand(TRUE);
      break;

    case collapseAll:
      Expand(FALSE);
      break;

    case syncDraw:
      SetupDraw();
      break;

    default:
      I_ASSERT(0);
      break;
    }

  if (lStatus == 0 || (Status() & lStatus))
    {
    for (nIdx = 0; nIdx < m_apSubItemCount; nIdx++)
      {
      switch (nMsg)
        {
        case count:
          m_apSubItems[nIdx]->VisitItems(nMsg, lStatus, nStatus, pCount);
          break;

        default:
          m_apSubItems[nIdx]->VisitItems(nMsg, lStatus);
          break;
        }
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::Select
//
// DESCR:   Selects or unselects the item.  It is NOT redrawn at this time.
//          TRUE is returned if the item has changed selection states.
//          ??!! Should it automatically invalidate?  Probably since it
//          knows if it has changed state.
//
////////////////////////////////////////////////////////////////

BOOL    SListItem::Select(
  BOOL    fSelect
  )
  {
  BOOL    fChanged    = TRUE;

  if (fSelect && !(Status() & selected))
    m_fStatus |= selected;
  else if (!fSelect && (Status() & selected))
    m_fStatus &= ~selected;
  else
    fChanged = FALSE;

  return fChanged;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::Expand
//
// DESCR:   Expands or collapses an item.  If the number of flattened
//          items changes, then TRUE is returned, else FALSE.
//
////////////////////////////////////////////////////////////////

BOOL    SListItem::Expand(
  BOOL      fExpand                         // FALSE => collapse
  )
  {
  BOOL      fChanged      = FALSE;

  if (Status() & rootItem)
    {
    if (fExpand)
      m_fStatus |= expanded;
    }

  else if (fExpand && !(Status() & expanded))
    {
    m_fStatus |= expanded;
    fChanged = TRUE;
    }

  else if (!fExpand && (Status() & expanded))
    {
    m_fStatus &= ~expanded;
    fChanged = TRUE;
    }

  return (fChanged && m_apSubItemCount != 0);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::Add
//
// DESCR:   Adds a new item to the list.  The items position can be
//          specified by a path of UIDs (where the UIDs are unique at
//          each level).  If this is the case, then nPath will be non-zero.
//
////////////////////////////////////////////////////////////////

BOOL  SListItem::Add(
  SListItem*    pItem,
  LONG*         plPath,
  short         nPathLength,
  SListItem*&   pAddedLevel         // filled in if added to 'this'
  )
  {
  BOOL          fAdded    = FALSE;
  short         nIdx;
  short         nNewSize;
  SListItem**   pNewAry;

  I_ASSERT(pItem != NULL);
  I_ASSERT(nPathLength >= 0);

  if (nPathLength > 0)              // doesn't belong at this level:  send down
    {
    I_ASSERT(plPath != NULL);
    I_ASSERT(*plPath != 0);

    // Find item to send it to.  The UID of the item will be at the
    // head of the UID path
    for (nIdx = 0; nIdx < m_apSubItemCount; nIdx++)
      {
      if (m_apSubItems[nIdx]->UID() == *plPath)
        {
        fAdded = m_apSubItems[nIdx]->Add(pItem, ++plPath, (short)(nPathLength - 1),
                                               pAddedLevel);
        break;
        }
      }

    ADBG_ASSERT_W(fAdded, ;, "Path not found in SListItem::Add");
    }
  else
    {
    if (m_apSubItemSize <= m_apSubItemCount)        // grow array
      {
      nNewSize = (short)(((m_apSubItemSize / 10) + 1) * 10);
      pNewAry  = (SListItem**) new char[nNewSize * sizeof(SListItem*)];

      if (pNewAry)
        {
        if (m_apSubItems)
          {
          COPYMEMORY(pNewAry, m_apSubItems, sizeof(SListItem*) * m_apSubItemSize);
          delete [] (char*)m_apSubItems;
          }

        m_apSubItems    = pNewAry;
        m_apSubItemSize = nNewSize;
        }
      }

    if (m_apSubItemCount + 1 <= m_apSubItemSize)    // add item
      {
      // The assumption is that the caller will do a Flatten() and
      // then check if the new count is different from the old count

      pItem->m_fStatus &= ~rootItem;
      pItem->m_pParent  = this;

      // Calculate x-offset for drawing
      pItem->SetupDraw();

      m_apSubItems[m_apSubItemCount++] = pItem;
      fAdded = TRUE;

      m_fStatus |= hasChildren;

      // Now re-sort
      if (Status() & sortItems)
        Sort();

#if 0
#ifdef ADBG_DEBUG
      // verify that all children either have or don't have a bitmap
      // While this is not necessary from a logic stance, if we don't
      // have consistency, then the display will be screwy.
      {
      BOOL    fHaveImage      = (m_apSubItems[0]->Image() != NULL);
      BOOL    fItemHasImage;
      short   nIdx;

      for (nIdx = 0; nIdx < m_apSubItemCount; nIdx++)
        {
        fItemHasImage = (m_apSubItems[nIdx]->Image() != NULL);
        if (fItemHasImage != fHaveImage)
          {
          ADbgPrint(ADBG_WARNING, 0, "Not all children consistent with image");
          break;
          }
        }
      }
#endif
#endif

      pAddedLevel = this;
      }
    }

  return fAdded;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::Invalidate
//
// DESCR:   Invalidates the area beginning at the 'this' flat position
//          to the bottom of the window.
//
////////////////////////////////////////////////////////////////

void    SListItem::Invalidate(
  BOOL    fOnDown                     // TRUE => from here down
  )
  {
  RECT      rcInvalid;

  GetClientRect(m_pMaster->Hwnd(), &rcInvalid);

  rcInvalid.top = m_pMaster->MapFlatPositionToY(m_nFlatPosition);

  if (!fOnDown)
    rcInvalid.bottom = rcInvalid.top + m_pMaster->ItemHeight();

  InvalidateRect(m_pMaster->Hwnd(), &rcInvalid, TRUE);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  SListItem::Draw
//
// DESCR:   Draws all items in the items list whose flat position is
//          within the given range.
//
////////////////////////////////////////////////////////////////

void    SListItem::Draw(
  HDC       hDC,
  short     nFirstFlat,
  short     nLastFlat,
  RECT*     pInvalidRect
  )
  {
  short     nIdx;
  short     nNextFlat;
  short     nY;
  int       nOldAlign;
  HFONT     hOldFont;

  // Always draw from the parent's text location.  Record our text location
  // for our children.  If we don't and our parent doesn't have an icon
  // then space over slightly.

  if (Status() & rootItem)                          // just pass to children
    ;

  else if (m_nFlatPosition < nFirstFlat ||          // item is before first
           m_nFlatPosition > nLastFlat  ||          // or after last
        !m_pMaster->ItemIsVisible(m_nFlatPosition)) // to draw or is not visible
    ;                                               // do nothing

  else
    {
    nY = m_pMaster->MapFlatPositionToY(m_nFlatPosition);

    nOldAlign = SetTextAlign(hDC, TA_LEFT | TA_TOP);

    if (m_hImage != NULL)
      {
      HDC       hBitmapDC   = CreateCompatibleDC(hDC);
      HBITMAP   hOldBitmap;

      if (hBitmapDC)
        {
        hOldBitmap = (HBITMAP) SelectObject(hBitmapDC, m_hImage);

        BitBlt(hDC, m_pParent->XOffset(), nY + c_nItemHeightSpace,
                    m_cxImage, m_cyImage, hBitmapDC, 0, 0, SRCCOPY);

        SelectObject(hBitmapDC, hOldBitmap);
        DeleteDC(hBitmapDC);
        }
      }

    if (Status() & hasChildren)
      hOldFont = (HFONT) SelectObject(hDC, m_pMaster->BoldFont());

    if (Status() & selected)        // set highlighted colors
      {
      SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
      SetBkColor(hDC,   GetSysColor(COLOR_HIGHLIGHT));
      }
    else
      {
      SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
      SetBkColor(hDC,   GetSysColor(COLOR_WINDOW));
      }

    // Now draw text.  Text is aligned so that baseline is at bottom of image.
    TextOut(hDC, XOffset(), nY + c_nItemHeightSpace,
              m_pszTitle, lstrlen(m_pszTitle));

    if (Status() & hasChildren)
      SelectObject(hDC, hOldFont);

    SetTextAlign(hDC, nOldAlign);
    }

  // Draw children only if expanded (if this item is not expanded, then
  // their flat positions are invalid).
  if (Status() & expanded)
    {
    for (nIdx = 0; nIdx < m_apSubItemCount; nIdx++)
      {
      if (nIdx + 1 < m_apSubItemCount)
        nNextFlat = m_apSubItems[nIdx + 1]->FlatPosition();
      else
        nNextFlat = (short)(nFirstFlat + 1);     // to force next 'if' true

      // if all of this child's flatpos are less than the first to display, then
      // don't display.  Don't display if nNextFlat is not visible either
      if (nNextFlat > nFirstFlat)// && m_pMaster->ItemIsVisible(nNextFlat))
        m_apSubItems[nIdx]->Draw(hDC, nFirstFlat, nLastFlat, pInvalidRect);
      }
    }
  }
