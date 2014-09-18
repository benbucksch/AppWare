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
// FILE:  "tbl_wlay.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#define   STRICT
#define   ASTRICT

#include  <windows.h>

#include  <a_alm.h>
#include  <op_wnd.h>
#include  <o_wnd.h>
#include  <intdebug.h>

// dfault sizes for buttons
#define I_DEFAULT_HEIGHT   300
#define I_DEFAULT_WIDTH    300
#define I_MIN_WIDTH        50
#define I_MIN_HEIGHT       50

#define I_DOT_SIZE         16 //1.6mm
#define I_DOT_SIZEH        8

extern int cxVS;
extern int cyVS;
extern int cxHS;
extern int cyHS;

#include  "tabl_hdr.h"
#include  "o_table.h"
#include  "table.h"
#include  "tbl_wlay.h"

// global variables ////////////////////////////////////////////////////////

extern GLOBAL G;
extern long Tbl_AdjustNewRect(RECT *rect,LONG adjustFlag);

#define LOPTR   "loPtr"
#define HIPTR   "hiPtr"
////////////////////////////////////////////////////////////////////////////
BOOL SetControlPtr(HWND hWnd, LPLONG thisPtr)
  {
  SetProp(hWnd, LOPTR, (HANDLE)LOWORD((DWORD)thisPtr));
  SetProp(hWnd, HIPTR, (HANDLE)HIWORD((DWORD)thisPtr));
  return TRUE;
  }

LONG GetControlPtr(HWND hWnd)
  {
  long rVal  = MAKELONG((UINT)GetProp(hWnd, LOPTR), (UINT)GetProp(hWnd, HIPTR));
  return rVal;
  }

extern "C" long FAR PASCAL _export VScrollWndProc (HWND hwnd,
  WORD message,
  WPARAM wParam,
  LPARAM lParam)
  {
  BOOL bCallDef = TRUE;
  WNDPROC DefWndProc = (WNDPROC)GetControlPtr(hwnd);

  // This should never happen, SKJ  WndProc is already stuffed in
  ADBG_ASSERT_E(DefWndProc, return 0L, "Invalid WndProc");

  switch (message)
    {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      bCallDef = IsWindowVisible(hwnd);
      if (bCallDef == FALSE) // i.e. if scroll bar is not visible
        {
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0L;
        }
      }
    default:
      break;  
    }

  return CallWindowProc ((WNDPROC)DefWndProc, hwnd, message, wParam, lParam) ;
  }


extern "C" long FAR PASCAL _export hScrollWndProc (HWND hwnd, WORD message, WPARAM wParam, LPARAM lParam)
  {
  WNDPROC DefWndProc = (WNDPROC)GetControlPtr(hwnd);
  ADBG_ASSERT_E(DefWndProc, return 0L, "Invalid WndProc");

  switch (message)
    {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      return 0L;
      }
    default:
      break;  
    }
  return CallWindowProc ((WNDPROC)DefWndProc, hwnd, message, wParam, lParam) ;
  }

// TABLE::TABLE ////////////////////////////////////////////////////////////

TABLE_WLAY::TABLE_WLAY(
  OBJECTID    objID,
  OBJECTID    oiParent,
  RECT*       rect,
  long        status)
  : OpLiteWndItem(objID,oiParent,rect,status)
  {
  HWND hwndParent = GetActiveWindow(); // the layout window
  table = new TABLE(objID,hwndParent, &m_rcItem);
  // set virtual class fields
  table->SetParentWnd(hwndParent);
  table->SetParentID((long)oiParent);
  table->m_oiParent = oiParent;
  }

// TABLE::~TABLE ///////////////////////////////////////////////////////////

TABLE_WLAY::~TABLE_WLAY(
  )
  {
  delete table;
  }

BOOL SubClassControl(HWND hWnd, long newProc)
  {
  BOOL rVal;
  if (hWnd == NULL)
    rVal = FALSE;
  else {
    WNDPROC defButtonProc = (WNDPROC)GetWindowLong(hWnd,  GWL_WNDPROC);
    SetControlPtr(hWnd, (LONG *)defButtonProc);
    SetWindowLong(hWnd,  GWL_WNDPROC, (long)newProc);
    }
  return rVal;
  }


/*************************************************************************
ParentOpened
*************************************************************************/
void ALMAPI TABLE_WLAY::ParentOpened(void)
  {
  // Put this code in the ParentShown function
  // store the DP rect
  table->visRect = m_rcItem;
  OWndLPtoDP(m_oiParent, (LPPOINT)&table->visRect, 2);

  table->CreateFonts();    // Create Runtime Fonts for the table
  table->InitFontMetrics();
  table->CreateScrollBars(m_hwndParent, FALSE);

  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(m_oiObject);
  rTime->m_fParentOpen = TRUE;
  }


/*************************************************************************
Hide
*************************************************************************/
void ALMAPI TABLE_WLAY::Hide(void)
  {
  if (table->VertScroll())
    {
    ShowWindow(table->hVertScroll, SW_HIDE);
    UpdateWindow(table->hVertScroll);
    }
  if (table->HorzScroll())
    {
    ShowWindow(table->hHorzScroll, SW_HIDE);
    UpdateWindow(table->hHorzScroll);
    }
  }


/*************************************************************************
Show
*************************************************************************/
void ALMAPI TABLE_WLAY::Show(void)
  {
  if (table->VertScroll())
    {
    ShowWindow(table->hVertScroll, SW_SHOW);
    UpdateWindow(table->hVertScroll);
    }
  if (table->HorzScroll())
    {
    ShowWindow(table->hHorzScroll, SW_SHOW);
    UpdateWindow(table->hHorzScroll);
    }
  }

/************************************************************************
ParentClosing
************************************************************************/
void ALMAPI TABLE_WLAY::ParentClosing(void)
  {
  table->DestroyScrollBars();
  table->DestroyFonts();    // Destroy Layout Fonts for the table

  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(m_oiObject);
  rTime->m_fParentOpen = FALSE;
  }

//    Destroy the table visual instance that was created with WIA_STARTUP //
void ALMAPI TABLE_WLAY::Destroy()
  {
  delete this;
  }

/////////////////////////////////////////////////////////////
// The AdjustNewRect Function calls this to make proper rect
// adjustment
/////////////////////////////////////////////////////////////
long localAdjustNewRect(
  OpWndItemD*             wi,
  RECT*                   newRect,
  LONG                    &adjustFlag)
  {
  long                    rVal = 0L;
  int                     tmp;
  int                     I_DEFAULT_THEIGHT = 2 * cyVS + cyHS;
  int                     I_DEFAULT_TWIDTH  = 2 * cxHS + cxVS;

  I_UNUSED(wi);

  if (newRect->bottom < 0 )
    {
    newRect->bottom = newRect->top + I_DEFAULT_THEIGHT;
    rVal |= (OWND_ADJRECT_CHANGED_B | OWND_ADJRECT_DEFAULT_H);
    }
  else if (newRect->bottom < newRect->top)
    {
    tmp = newRect->bottom;
    newRect->bottom = newRect->top;
    newRect->top = tmp;
    rVal |= OWND_ADJRECT_SWAPPED_TB;
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      { //if we swap the sides, we need to swap the anchor points
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_B;
      adjustFlag |= OWND_ADJRECT_ANCHOR_T;
      }
    else
      {
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_T;
      adjustFlag |= OWND_ADJRECT_ANCHOR_B;
      }
    }

  if (newRect->right < 0)
    {
    newRect->right = newRect->left + I_DEFAULT_TWIDTH;
    rVal |= (OWND_ADJRECT_CHANGED_R | OWND_ADJRECT_DEFAULT_W);
    }
  else if (newRect->right < newRect->left)
    {
    tmp = newRect->right;
    newRect->right = newRect->left;
    newRect->left = tmp;
    rVal |= OWND_ADJRECT_SWAPPED_LR;
    if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
      { //if we swap the sides, we need to swap the anchor points
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_R;
      adjustFlag |= OWND_ADJRECT_ANCHOR_L;
      }
    else
      {
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_L;
      adjustFlag |= OWND_ADJRECT_ANCHOR_R;
      }
    }

  //At this point, the rectangle is not flipped inside-out
  // Make sure it is a minimum height and width
  if (newRect->bottom - newRect->top < I_MIN_HEIGHT)
    {
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      {
      newRect->top = newRect->bottom - I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      newRect->bottom = newRect->top + I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_B;
      }
    }

  if (newRect->right - newRect->left < I_MIN_WIDTH)
    { 
    if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
      {
      newRect->left = newRect->right - I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      newRect->right = newRect->left + I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_R;
      }
    }

  return rVal;
  }

/////////////////////////////////////////////////////////////
// The AdjustNewRect Functions
/////////////////////////////////////////////////////////////
long ALMAPI TABLE_WLAY::AdjustNewRect(
  RECT*                   newRect,
  LONG                    adjustFlag)
  {
  long                    rVal = 0L;
  rVal = localAdjustNewRect(this, newRect, adjustFlag);
  table->visRect = *newRect;
  OWndLPtoDP(m_oiParent,(LPPOINT)&table->visRect, 2);
  
  // if the rect is too small clip to the minimum size
  if(table->VertScroll())
    {
    if ((table->visRect.right - table->visRect.left) < (2 * cxHS + cxVS))
      {
      if ( adjustFlag & OWND_ADJRECT_ANCHOR_R)
        table->visRect.left = table->visRect.right - (2 * cxHS + cxVS);
      else
        table->visRect.right = table->visRect.left + (2 * cxHS + cxVS);
      }
    }

  if(table->HorzScroll())
    {
    if((table->visRect.bottom - table->visRect.top) < (2 * cyVS + cyHS))
      {
      if ( adjustFlag & OWND_ADJRECT_ANCHOR_B)
        table->visRect.top = table->visRect.bottom - (2 * cyVS + cyHS);
      else
        table->visRect.bottom = table->visRect.top + (2 * cyVS + cyHS);;
      }
    }

  // reset the newRect
  *newRect = table->visRect;
  OWndDPtoLP(m_oiParent,(LPPOINT)newRect, 2);
  return rVal;
  }


/////////////////////////////////////////////////////////////
// The Move Function
/////////////////////////////////////////////////////////////
void ALMAPI TABLE_WLAY::Move(LPRECT newRect)
  {
  if (EqualRect(newRect, &(m_rcItem)) == FALSE)
      {
      OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
      m_rcItem = *newRect;
      table->visRect = *newRect;
      // store the DP rect
      OWndLPtoDP(m_oiParent, (LPPOINT)&table->visRect, 2);

      OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
      }
  }

////////////////////////////////////////////////////////////////////////
// Draw : Called during WLAYOUT time
///////////////////////////////////////////////////////////////////////
// Draw a sample table for the window layout ////////////////////////////

void ALMAPI TABLE_WLAY::Draw(HDC hDC, RECT *clipRect)
  {
  RECT tmpRect;
  BOOL bIntersect;
  bIntersect = IntersectRect(&tmpRect, clipRect, &m_rcItem);

  tmpRect = m_rcItem; // always draw to the entire
                      // visual rect

  if (bIntersect)
    {
    OWndLStoDS(m_oiParent, (LPPOINT)&tmpRect, 2);

    int saveDC = SaveDC(hDC);
    int mode = SetMapMode(hDC, MM_TEXT);
    table->WIDR_Draw(hDC, &tmpRect); //widr->rectPtr);
    SetMapMode(hDC, mode); // restore LOMETRIC mode
    RestoreDC(hDC, saveDC);

    if (table->VertScroll())
      {
      UpdateWindow(table->hVertScroll);
      }
    if (table->HorzScroll())
      {
      UpdateWindow(table->hHorzScroll);
      }
    }
  }
