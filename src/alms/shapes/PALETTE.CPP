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
// FILE:          PALETTE.CPP
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   Contains code for displaying a palette of buttons.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define ASTRICT
#define STRICT

#include <memory.h>
#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>
//#include <defines.h>


#include "bttncur.h"
#include "palette.h"
#include "xp_macro.h"
#include "balloon.h"


//////////////// Global variables
static char       s_szWndClass[]              = "ApwPalette";

//////////////// Class static variables
BOOL              PaletteWindow::s_fInitOK    = FALSE;
HINSTANCE         PaletteWindow::s_hInstance  = NULL;

//////////////// Internal constants
const int         c_nBorder   = 2;          // two pixels on each side of face

// ****************************************************************
//
// PaletteWindow code
//
// ****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::Init
//
//
////////////////////////////////////////////////////////////////

BOOL    PaletteWindow::Init(    // Register class
  HINSTANCE   hInst
  )
  {
  WNDCLASS      wc;

  s_hInstance = hInst;

  wc.style          = CS_BYTEALIGNWINDOW;
  wc.lpfnWndProc    = WndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = sizeof(PaletteWindow*);
  wc.hInstance      = s_hInstance;
  wc.hIcon          = NULL;
  wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground  = (HBRUSH) GetStockObject(GRAY_BRUSH);
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = s_szWndClass;

  s_fInitOK = (RegisterClass(&wc) && ToolButtonInit());

  return s_fInitOK;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::Cleanup
//
// DESCR:   Cleans up the palette code.
//
////////////////////////////////////////////////////////////////

void    PaletteWindow::Cleanup(
  void
  )
  {
  ToolButtonFree();
  UnregisterClass(s_szWndClass, s_hInstance);

  s_fInitOK = FALSE;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::WndProc
//
// DESCR:   This is the WindowProc for the palette window.  It just
//          forwards necessary messages on to the corresponding methods
//          on the object.
//
////////////////////////////////////////////////////////////////

LRESULT ALMAPI    PaletteWindow::WndProc(
  HWND    hWnd,
  UINT    nMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )
  {
  LRESULT           lRes        = 0;
  PaletteWindow*    pPalette;


  pPalette = (PaletteWindow*) GetWindowLong(hWnd, 0);

  switch (nMsg)
    {
    case WM_CREATE:
      pPalette = (PaletteWindow*) (((LPCREATESTRUCT) lParam)->lpCreateParams);
      SetWindowLong(hWnd, 0, (LONG) pPalette);
      break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
      {
      POINT   pDown;

      pDown.x = (COORDINATE)((short)LOWORD(lParam));
      pDown.y = (COORDINATE)((short)HIWORD(lParam));

      pPalette->MouseDown(&pDown, (short)((nMsg == WM_LBUTTONDOWN) ? leftButton : rightButton));
      break;
      }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
      {
      POINT   pUp;

      pUp.x = (COORDINATE)((short)LOWORD(lParam));
      pUp.y = (COORDINATE)((short)HIWORD(lParam));

      pPalette->MouseUp(&pUp, (short)((nMsg == WM_LBUTTONUP) ? leftButton : rightButton));
      break;
      }

    case WM_MOUSEMOVE:
      {
      POINT   pMove;

      pMove.x = (COORDINATE)((short)LOWORD(lParam));
      pMove.y = (COORDINATE)((short)HIWORD(lParam));

      pPalette->MouseMove(&pMove);
      break;
      }

    case WM_CANCELMODE:
      pPalette->CancelMode();
      break;

    case WM_PAINT:
      {
      PAINTSTRUCT   ps;
      HDC           hDC;

      hDC = BeginPaint(hWnd, &ps);

      if (hDC)
        pPalette->Draw(hDC, &ps.rcPaint);

      EndPaint(hWnd, &ps);

      break;
      }

    case WM_SYSCOMMAND:
      if (wParam == SC_CLOSE)
        pPalette->Closing();
      else
        lRes = DefWindowProc(hWnd, nMsg, wParam, lParam);
      break;

//    case WM_ERASEBKGND:
//      lRes = TRUE;            // don't erase background
//      break;

	case BALLOON_REQUESTPROMPT:	
	{
		POINT	pMove;
		pMove.x = BALLOON_POINT_X(lParam);
		pMove.y = BALLOON_POINT_Y(lParam);
     	pPalette->ReturnPrompt(wParam, &pMove);
		break;
	}


    default:
      lRes = DefWindowProc(hWnd, nMsg, wParam, lParam);
      break;
    }

  return lRes;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::PaletteWindow
//
// DESCR:   Initializes a new palette window object.
//
////////////////////////////////////////////////////////////////

PaletteWindow::PaletteWindow(
  LONG      lOptions,
  HWND      hOwner
  ) :
    m_hPalette(NULL),
    m_lStatus(0),
    m_lOptions(lOptions & Palette_allowedOptions),
    m_apItems(NULL),
    m_apItemCount(0),
    m_apItemSize(0),
    m_nCurrentRow(0),
    m_nCurrentCol(0),
    m_cxUnit(0),
    m_cyUnit(0),
    m_nUnitRes(1),
    m_nCols(0),
    m_pCurrentItem(NULL)
  {
  DWORD     dwStyle   = WS_BORDER;

  ADBG_ASSERT_W(s_fInitOK, ;, "Creating a palette when Init() failed.");

  if (s_fInitOK)
    {
    if (m_lOptions & titleBar)
      dwStyle |= (WS_CAPTION | WS_SYSMENU);

    if (m_lOptions & popupPalette)
      dwStyle |= WS_POPUP;
    else
      dwStyle |= WS_CHILD;

    m_hPalette = CreateWindow(s_szWndClass, NULL, dwStyle,
                                      0, 0, 0, 0,
                                      hOwner, NULL, s_hInstance, this);

	Balloon_RegisterClient(m_hPalette);

    if (m_hPalette && (lOptions & titleBar))    // remove nonsensical items
      {
      HMENU   hSysMenu  = GetSystemMenu(m_hPalette, FALSE);

      // Keep positions 0 and 6 (MOVE and CLOSE); delete in reverse order
      DeleteMenu(hSysMenu, 8, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 7, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 5, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 4, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 3, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 2, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 0, MF_BYPOSITION);
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::~PaletteWindow
//
// DESCR:   Destroys a palette window:  deletes all palette items and
//          the actual Window's window.
//
////////////////////////////////////////////////////////////////

PaletteWindow::~PaletteWindow(
  void
  )
  {
  short     idx;

  if (m_hPalette)
    {
    for (idx = 0; idx < m_apItemCount; idx++)
      delete m_apItems[idx];

    delete m_apItems;               // NULL is okay here

    if (m_hPalette)
      {
      DestroyWindow(m_hPalette);
      m_hPalette = NULL;
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::ItemChosen
//
// DESCR:   Default hander for item chosen action.
//
////////////////////////////////////////////////////////////////

void    PaletteWindow::ItemChosen(
  PaletteItem*    pItem
  )
  {
  I_UNUSED(pItem);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::Closing
//
// DESCR:   Default hander for the closing action.
//
////////////////////////////////////////////////////////////////

void    PaletteWindow::Closing(
  void
  )
  {
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::SetUnitSize
//
// DESCR:   Sets the size of a cell in pixels.  The size is used to
//          place each item as it is added.  The resolution of the coordinate
//          system is set also.  The resolution determines to what part of
//          the column or row an item's size can be set to.  The value
//          passed in is the reciprocal of the resolution.  E.g., if "2"
//          is passed in, then the size of the items will be specified
//          in 1/2 column&row units.
//
////////////////////////////////////////////////////////////////

void    PaletteWindow::SetUnitSize(
  short   nUnitX,
  short   nUnitY,
  short   nUnitRes                // == 1
  )
  {
  m_cxUnit = nUnitX;
  m_cyUnit = nUnitY;

  m_nUnitRes = nUnitRes;
  }


////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::ReturnPrompt
//
// DESCR:   When the balloon server request a prompt 
//			 this method will ascertain which item the mouse
//			 is over and extract the appropriate prompt and
//			 return it to the balloon server.
//
////////////////////////////////////////////////////////////////
void PaletteWindow::ReturnPrompt(WPARAM wParam, POINT *pMove)
{
	char	*prompt;
  	int   	idx;


	m_pCurrentItem = NULL;

	for (idx = 0; idx < m_apItemCount; idx++)
	{
		if (m_apItems[idx]->PointInItem(pMove))
		{
			m_pCurrentItem = m_apItems[idx];
			break;
		}
	}

	if (m_pCurrentItem)
	{
		prompt = m_pCurrentItem->GetPrompt();
		SendMessage((HWND)wParam, BALLOON_UPDATEPROMPT, (WPARAM)prompt, NULL);
	}
}			



////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::AddItem
//
// DESCR:   Adds a new item to the palette.  Items are added left
//          to right, top to bottom, with automatic wrapping from
//          row n to row n+1.
//
////////////////////////////////////////////////////////////////

BOOL    PaletteWindow::AddItem(
  PaletteItem*    pItem
  )
  {
  BOOL            fAdded  = FALSE;
  PaletteItem**   pNewAry;
  short           nNewSize;
  RECT            rcNew;
  short           nItemWidth;
  short           nItemHeight;

  ADBG_ASSERT_E(m_cxUnit != 0, return FALSE;, "Unit size not set up for palette");
  ADBG_ASSERT_E(pItem != NULL, return FALSE;, "AddItem requires an item");
  ADBG_ASSERT_E(!(m_lStatus & fEndAddCalled), return FALSE, "EndAdd() called");

  if (m_apItemSize <= m_apItemCount)        // grow array
    {
    nNewSize = (short)(((m_apItemSize / 10) + 1) * 10);
    pNewAry  = (PaletteItem**) new char[nNewSize * sizeof(PaletteItem*)];

    if (pNewAry)
      {
      if (m_apItems)
        {
        COPYMEMORY(pNewAry, m_apItems, sizeof(PaletteItem*) * m_apItemSize);
        delete (char*) m_apItems;
        }

      m_apItems    = pNewAry;
      m_apItemSize = nNewSize;
      }
    }

  if (m_apItemCount + 1 <= m_apItemSize)
    {
    pItem->ItemSize(nItemWidth, nItemHeight);   // in unitres
    if (nItemWidth == 0)
      nItemWidth = m_nUnitRes;

    if (nItemHeight == 0)
      nItemHeight = m_nUnitRes;

    ADBG_ASSERT_E(nItemWidth / m_nUnitRes <= m_nCols, ;, "Item is wider than palette");

    if (m_nCurrentCol + nItemWidth > m_nCols * m_nUnitRes)
      {
      ADBG_PRINT_W("Item goes past right edge:  placing at first of new row");
      m_nCurrentCol = 0;
      m_nCurrentRow++;
      }

    rcNew.left = MulDiv(m_nCurrentCol, m_cxUnit, m_nUnitRes);
    rcNew.top  = MulDiv(m_nCurrentRow, m_cyUnit, m_nUnitRes);

    m_nCurrentCol += nItemWidth;

    if (m_nCurrentCol >= m_nCols * m_nUnitRes)
      {
      m_nCurrentCol  = 0;
      m_nCurrentRow += nItemHeight;
      }

    rcNew.right  = rcNew.left + MulDiv(m_cxUnit, nItemWidth, m_nUnitRes);
    rcNew.bottom = rcNew.top  + MulDiv(m_cyUnit, nItemHeight, m_nUnitRes);
    pItem->SetPosition(&rcNew);

    m_apItems[m_apItemCount++] = pItem;
    fAdded = TRUE;
    }

  return fAdded;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::EndAdd
//
// DESCR:   Adjusts the size of the window to be large enough to hold
//          all the items, but no larger.
//
////////////////////////////////////////////////////////////////

void    PaletteWindow::EndAdd(
  void
  )
  {
  RECT    rcClient;

  m_lStatus |= fEndAddCalled;

  if (m_apItemCount > 0)
    {
    rcClient.left   = 0;
    rcClient.top    = 0;
    rcClient.right  = m_cxUnit * m_nCols;
    rcClient.bottom = MulDiv(m_cyUnit, m_nCurrentRow, m_nUnitRes);

    AdjustWindowRect(&rcClient,
                          WS_CAPTION | WS_POPUP | WS_BORDER | WS_SYSMENU,
                          FALSE);

    SetWindowPos(m_hPalette, NULL, 0, 0,
                  rcClient.right - rcClient.left,
                  rcClient.bottom - rcClient.top,
                  SWP_NOZORDER | SWP_NOACTIVATE);
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::SetItemState
//
// DESCR:   Searches for the given item and changes its state to the
//          given one.  Semantics of radio group are applied.
//
////////////////////////////////////////////////////////////////

void    PaletteWindow::SetItemState(
  short   nToolId,
  BOOL    fState
  )
  {
  PaletteItem*    pItem;
  short           idx;

  for (idx = 0; idx < m_apItemCount; idx++)
    {
    pItem = m_apItems[idx];

    if (pItem->ItemID() == nToolId)
      {
      pItem->SetState(fState);

      if (fState && pItem->GroupID())
        ChangeRadioButton(pItem);     
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::Enable
//
// DESCR:   Enables or disables the palette window.
//
////////////////////////////////////////////////////////////////

void    PaletteWindow::Enable(
  BOOL    fEnable
  )
  {
  I_ASSERT(m_hPalette);

  EnableWindow(m_hPalette, fEnable);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::CancelMode
//
// DESCR:   This is sent by windows when the capture is about to be taken
//          away by a dialog or message box.  This method will perform a
//          MouseUp() action with inItem FALSE.  This will redraw the
//          item in the original state.
//
////////////////////////////////////////////////////////////////

LRESULT   PaletteWindow::CancelMode(
  void
  )
  { 
  if (m_lStatus & fTrackingMouse)
    {
    I_ASSERT(m_pCurrentItem != NULL);

    m_pCurrentItem->MouseUp(FALSE);

    m_lStatus &= ~fTrackingMouse;
    }

  m_pCurrentItem = NULL;

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::MouseDown
//
// DESCR:   Handles the mouse down condition by searching on all the
//          items.  If the item has an action on mouse up, then we must
//          track the mouse using SetCapture.  If the action is on mouse
//          down, then we notify the palette owner.
//
////////////////////////////////////////////////////////////////

LRESULT   PaletteWindow::MouseDown(
  POINT*    pDown,
  short     nButton
  )
  { 
  int       idx;
  LONG      lStyle;

  if (nButton == leftButton)
    {
    m_pCurrentItem = NULL;

    for (idx = 0; idx < m_apItemCount; idx++)
      {
      if (m_apItems[idx]->PointInItem(pDown))
        {
        m_pCurrentItem = m_apItems[idx];
        break;
        }
      }

    if (m_pCurrentItem)
      {
      m_pCurrentItem->MouseDown();
      m_lStatus |= fMouseIn;

      lStyle = m_pCurrentItem->Style();

      if (lStyle & PaletteItem::actionOnMouseUp)
        {
        // need to capture mouse
        SetCapture(m_hPalette);
        m_lStatus |= fTrackingMouse;
        }

      if (lStyle & PaletteItem::actionOnMouseDown)
        ItemChosen(m_pCurrentItem);
      }
    }
  else if (nButton == rightButton)
    {
    m_pCurrentItem = NULL;

    for (idx = 0; idx < m_apItemCount; idx++)
      {
      if (m_apItems[idx]->PointInItem(pDown))
        {
        m_pCurrentItem = m_apItems[idx];
        break;
        }
      }

    if (m_pCurrentItem)
      {
		 m_pCurrentItem->fPress = FALSE;
	     SetCapture(m_hPalette);
        m_lStatus |= fTrackingMouse;
      }
    }


  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::MouseMove
//
// DESCR:   If the mouse is being tracked, then MouseIn and MouseOut calls
//          are generated appropriately.
//
////////////////////////////////////////////////////////////////

LRESULT   PaletteWindow::MouseMove(
  POINT*    pMove
  )
  { 
  if (m_lStatus & fTrackingMouse)
    {
    I_ASSERT(m_pCurrentItem != NULL);

    if (m_lStatus & fMouseIn)
      {
      if (!m_pCurrentItem->PointInItem(pMove))
        {
        m_lStatus &= ~fMouseIn;
        m_pCurrentItem->MouseOut();
        }
      }
    else
      {
      if (m_pCurrentItem->PointInItem(pMove))
        {
			if(m_pCurrentItem->fPress)
			{
		        m_lStatus |= fMouseIn;
		        m_pCurrentItem->MouseIn();
			}
        }
      }
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::MouseUp
//
// DESCR:   If the mouse is being tracked, then a MouseUp() is sent
//          to the item.  If the item is now on and is a radio button,
//          then any other buttons in the group are turned off.
//
////////////////////////////////////////////////////////////////

LRESULT   PaletteWindow::MouseUp(
  POINT*    pUp,
  short     nButton
  )
  { 
  BOOL      fInItem;


  if (nButton == leftButton)
    {
    if (m_lStatus & fTrackingMouse)
      {
      I_ASSERT(m_pCurrentItem != NULL);
      I_ASSERT(m_pCurrentItem->Style() & PaletteItem::actionOnMouseUp);

      m_lStatus &= ~fTrackingMouse;
      ReleaseCapture();

      fInItem = m_pCurrentItem->PointInItem(pUp);

      m_pCurrentItem->MouseUp(fInItem);

      if (fInItem)
        {
        ChangeRadioButton(m_pCurrentItem);
        ItemChosen(m_pCurrentItem);
        }
      }

    m_lStatus &= ~fMouseIn;
    m_pCurrentItem = NULL;
    }
  else if (nButton == rightButton)
    {
    if (m_lStatus & fTrackingMouse)
      {
      I_ASSERT(m_pCurrentItem != NULL);

      m_lStatus &= ~fTrackingMouse;
      ReleaseCapture();

      fInItem = m_pCurrentItem->PointInItem(pUp);

	   m_pCurrentItem->fPress = TRUE;

      if (fInItem)
		   m_pCurrentItem->ShowHelp();
      }

    m_lStatus &= ~fMouseIn;
    m_pCurrentItem = NULL;
    }

  return 0;
  }


////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::ShowHelp
//
// DESCR:   Get the help item that is associated with 
//          the help ID that is associated with this bitmap.
//
////////////////////////////////////////////////////////////////
void PaletteItem::ShowHelp()
{
	WinHelp(m_pOwner->Hwnd(), 
			"awuswnd.hlp", 
			HELP_CONTEXT, 
			dwHelpID);
}


////////////////////////////////////////////////////////////////
//
// METHOD:    PaletteWindow::Draw
//
// DESCR:     Draws all items whose rectangle intersects the invalid
//            region.
//
////////////////////////////////////////////////////////////////

LRESULT   PaletteWindow::Draw(
  HDC       hDC,
  RECT*     prcInvalid
  )
  { 
  RECT      rcIntersect;
  short     idx;

  for (idx = 0; idx < m_apItemCount; idx++)
    {
    if (IntersectRect(&rcIntersect,
                      prcInvalid,
                      m_apItems[idx]->Rectangle()))
      m_apItems[idx]->Draw(hDC);
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteWindow::ChangeRadioButton
//
// DESCR:   Turns on the given radio button.  It will turn off any
//          other buttons in that radio group.
//
////////////////////////////////////////////////////////////////

void  PaletteWindow::ChangeRadioButton(
  PaletteItem*    pItem
  )
  {
  UINT      nGroup;
  short     idx;

  nGroup = pItem->GroupID();

  if (nGroup != 0)                      // turn off any that are down
    {
    for (idx = 0; idx < m_apItemCount; idx++)
      {
      if (m_apItems[idx] != pItem           &&
          m_apItems[idx]->GroupID() == nGroup)
        {
        if (m_apItems[idx]->State())
          m_apItems[idx]->SetState(FALSE);
        }
      }
    }
  }

// ****************************************************************
//
// PaletteItem code
//
// ****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::PaletteItem
//
// DESCR:   Initializes a new palette item object.
//
////////////////////////////////////////////////////////////////

PaletteItem::PaletteItem(
  PaletteWindow*  pOwner,
  UINT            nID,
  UINT            gID,
  LONG            lStyle
  ) :
    m_pOwner(pOwner),
    m_lStyle(lStyle),
    m_nID(nID),
    m_gID(gID),
    m_fState(FALSE),
    m_hBitmap(NULL),
    m_cxBitmap(0),
    m_cyBitmap(0),
    m_cxWidth(0),
    m_cyHeight(0),
	 fPress(TRUE)
  {
  m_rcItem.left = m_rcItem.right = m_rcItem.top = m_rcItem.bottom = 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::~PaletteItem
//
// DESCR:   Destroys a palette item:  deletes the bitmap
//
////////////////////////////////////////////////////////////////

PaletteItem::~PaletteItem(
  void
  )
  {
  if (m_hBitmap && !(m_lStyle & doesntOwnBitmap))
    {
    DeleteObject(m_hBitmap);
    m_hBitmap = NULL;
    }

  if (szPrompt)
	 delete szPrompt;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::DrawBitmap
//
// DESCR:   Draws the bitmap into the HDC using the Microsoft-supplied
//          tool drawing routines that accompany the book
//          "The Window Interface:  An Application Design Guide".  We
//          do not currently take advantage of all the different states
//          of the buttons (only up and down).
//
////////////////////////////////////////////////////////////////

void    PaletteItem::DrawBitmap(
  HDC     hDC,
  UINT    uState
  )
  {
  UIToolButtonDraw(hDC, m_rcItem.left, m_rcItem.top,
                    m_rcItem.right  - m_rcItem.left + 1,
                    m_rcItem.bottom - m_rcItem.top + 1,
                    m_hBitmap, m_cxBitmap, m_cyBitmap, 0,
                    uState);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::DrawItem
//
// DESCR:   Draws the item into the given HDC with the given state.
//          This is the lowest-level draw that all other Draw()s call.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::DrawItem(
  HDC     hDC,
  BOOL    fState
  )
  {
  DrawBitmap(hDC, (fState) ? ATTRIBUTEBUTTON_MOUSEDOWN : ATTRIBUTEBUTTON_UP);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::DrawByState
//
// DESCR:   Draws the item into the palette window with the given state.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::DrawByState(
  BOOL    fState
  )
  {
  HDC     hDC;

  hDC = GetDC(m_pOwner->Hwnd());

  if (hDC)
    {
    DrawItem(hDC, fState);
    ReleaseDC(m_pOwner->Hwnd(), hDC);
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::GetNewState
//
// DESCR:   Determines what the new state of the item would be if the
//          mouse were to be released within its rectangle.
//
////////////////////////////////////////////////////////////////

BOOL    PaletteItem::GetNewState(
  void
  )
  {
  BOOL    fNewState;

  if (m_lStyle & toggleWhenActionDone)
    fNewState = !m_fState;
  else if (m_lStyle & turnOnWhenActionDone)
    fNewState = TRUE;
  else if (m_lStyle & turnOffWhenActionDone)
    fNewState = FALSE;
  else
    fNewState = m_fState;

  return fNewState;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::GetImageState
//
// DESCR:   Determines what the image state should be (on or off) during
//          a mouse tracking operation.
//
////////////////////////////////////////////////////////////////

BOOL    PaletteItem::GetImageState(
  void
  )
  {
  BOOL    fImageState;

  if (m_lStyle & toggleImage)
    fImageState = !m_fState;
  else
    fImageState = m_fState;

  return fImageState;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::ChangeState
//
// DESCR:   Changes the state of the item to the one passed in (on or off).
//
////////////////////////////////////////////////////////////////

void    PaletteItem::ChangeState(
  BOOL    fNewState
  )
  {
  m_fState = fNewState;
  DrawByState(m_fState);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::Draw
//
// DESCR:   Draws the item into the HDC.  This is called during a repaint
//          operation, so the current state is the one used during the draw.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::Draw(
  HDC     hDC
  )
  {
  DrawItem(hDC, m_fState);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::PointInItem
//
// DESCR:   Returns TRUE if the point is within the item.
//
////////////////////////////////////////////////////////////////

BOOL    PaletteItem::PointInItem(
  POINT*    pPoint
  )
  {
  return PtInRect(&m_rcItem, *pPoint);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::MouseDown
//
// DESCR:   Handles the mouse going down in the item.  Since items are
//          only concerned about internal state and image, no posting
//          or notification is generated here.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::MouseDown(
  void
  )
  {
  BOOL      fNewState;
  BOOL      fImageState;

  fNewState   = GetNewState();
  fImageState = GetImageState();

  if (fNewState != m_fState)
    DrawByState(fNewState);
  else if (fImageState != m_fState)
    DrawByState(fImageState);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::MouseUp
//
// DESCR:   If the mouse goes up in the item, then the item changes to
//          the new state.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::MouseUp(
  BOOL      fInItem
  )
  {
  BOOL      fNewState;

  if (fInItem)
    {
    fNewState = GetNewState();
    ChangeState(fNewState);
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::MouseIn
//
// DESCR:   Handles the case where, during tracking of the mouse, the
//          mouse moves back into the item's rectangle.  This turns out to
//          be logically equivalent to a MouseDown().
//
////////////////////////////////////////////////////////////////

void    PaletteItem::MouseIn(
  void
  )
  {
  MouseDown();
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::MouseOut
//
// DESCR:   Handles the case where, during tracking of the mouse, the
//          mouse move out of the item's rectangle.  The item just
//          draws itself in its current state.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::MouseOut(
  void
  )
  {
  DrawByState(m_fState);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::SetState
//
// DESCR:   Changes the internal state to the given one.  All image
//          changes are performed.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::SetState(
  BOOL    fState
  )
  {
  ChangeState(fState);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::SetPosition
//
// DESCR:   Changes the display position of the item.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::SetPosition(
  RECT*   prcItem
  )
  {
  m_rcItem.left   = prcItem->left;
  m_rcItem.bottom = prcItem->bottom;
  m_rcItem.right  = prcItem->right;
  m_rcItem.top    = prcItem->top;
  }


////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::AddHelpID
//
// DESCR:   Associates a helpID with and item.
//
////////////////////////////////////////////////////////////////
void PaletteItem::AddHelpID(DWORD helpID)
{
	if(helpID)
		dwHelpID = helpID;
}


////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::AddPrompt
//
// DESCR:   Associates a prompt with a bitmap.  This is used
//          for balloon prompt support.
//
////////////////////////////////////////////////////////////////
void PaletteItem::AddPrompt(char *szInPrompt)
{
	szPrompt = new char[strlen(szInPrompt)];
	strcpy(szPrompt, szInPrompt);
}


////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::GetPrompt
//
// DESCR:   Get the prompt associated with the bitmap.
//          This is used for balloon prompt support.
//
////////////////////////////////////////////////////////////////
char* PaletteItem::GetPrompt()
{
	return szPrompt;
}



////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::SetImage
//
// DESCR:   Accepts an image bitmap to be displayed as the item face.
//          The item's right and bottom are adjusted accordingly.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::SetImage(
  HBITMAP   hImage
  )
  {
  BITMAP    biInfo;

  ADBG_ASSERT_E(hImage != NULL, return, "Invalid hImage in SetImage");
  ADBG_ASSERT_W(m_hBitmap == NULL, return, "Item already has image");

  if (m_hBitmap != NULL)
    DeleteObject(m_hBitmap);

  m_hBitmap = hImage;

  GetObject(m_hBitmap, sizeof(biInfo), &biInfo);

  m_cxBitmap = (short)biInfo.bmWidth;
  m_cyBitmap = (short)biInfo.bmHeight;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::SetSize
//
// DESCR:   Sets the size of the item in parent units.  This lets the
//          parent know the size of the item.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::SetSize(
  short   cx,                   // in parent units
  short   cy
  )
  {
  I_ASSERT(cx > 0 && cy > 0);

  m_cxWidth  = cx;
  m_cyHeight = cy;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  PaletteItem::ItemSize
//
// DESCR:   Returns the size of the item in parent units.
//
////////////////////////////////////////////////////////////////

void    PaletteItem::ItemSize(
  short&    cx,
  short&    cy
  )
  {
  cx = m_cxWidth;
  cy = m_cyHeight;
  }
