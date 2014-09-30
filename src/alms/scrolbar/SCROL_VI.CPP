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
// FILE:          "scrol_vi.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is the main file for the Scroll Bar visual instance
//                code.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>

#include "scroll.h"
#include "scrol_vi.h"

// Global variables ////////////////////////////////////////////////////////

extern HINSTANCE  g_hinstDLL;
int               ScrollVI::sm_cxVS = GetSystemMetrics(SM_CXVSCROLL);
int               ScrollVI::sm_cyHS = GetSystemMetrics(SM_CYHSCROLL);
WNDPROC           ScrollVI::sm_pfnSBarWndProc = NULL;

// constants ///////////////////////////////////////////////////////////////

const char        k_szAPWScrollBar[]  = "APWScrollBar";
const char        k_szScrollBar[]     = "ScrollBar";

////////////////////////////////////////////////////////////////////////////
//
//  Function:     CreateScrollBarVI
//
//  Description:  This function will create a scroll bar visual instance.
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* ALMAPI CreateScrollBarVI
  (
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  LPRECT      rcItem,
  long        lStatus,
  AMEMBLOCKID  miExtra
  )

  {
  I_UNUSED(miExtra);

  return new ScrollVI(oiItem, oiParent, rcItem, lStatus);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::ScrollVI
//
//  Description:  This function will create a scroll bar visual instance.
//
////////////////////////////////////////////////////////////////////////////

ScrollVI::ScrollVI
  (
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  LPRECT      prcItem,
  long        lStatus
  ) : OpHwndWndItem(oiItem, oiParent, prcItem, lStatus),
      m_pScroll((Scroll*)AObjGetRuntimeInfo(m_oiObject))
  {
  OWndSetItemActivatable(this, m_pScroll->CanActivate());
  OWndSetItemEnabled(this, m_pScroll->Enabled());
  OWndSetItemTransparent(this, FALSE);
  OWndSetItemUsesCursorKeys(this, TRUE);
  OWndSetItemUsesTab(this, FALSE);
  OWndSetItemUsesDefaultKey(this, FALSE);
  OWndSetItemUsesCancelKey(this, FALSE);

  AdjustNewRect(&m_rcItem, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::~ScrollVI
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

ScrollVI::~ScrollVI
  (
  )

  {
  }


////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::AdjustNewRect
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

LONG ALMAPI ScrollVI::AdjustNewRect
  (
  LPRECT  lprcNew,
  long    fAdjust
  )

  {
  LONG    lRet = 0L;
  int     nTmp;
  POINT   sBarSize = {sm_cxVS, sm_cyHS};

  OWndDStoLS(m_oiParent, &sBarSize, 1) ;
  if (fAdjust & OWND_ADJRECT_DEFAULT_H)
    {
    if(fAdjust & OWND_ADJRECT_ANCHOR_B)
      {
      if(m_pScroll->Vertical())
        lprcNew->top = lprcNew->bottom - 4*sBarSize.y ;
      else
        lprcNew->top = lprcNew->bottom - sBarSize.y ;
      lRet |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      if(m_pScroll->Vertical())
        lprcNew->bottom = lprcNew->top + 4*sBarSize.y ;
      else
        lprcNew->bottom = lprcNew->top + sBarSize.y ;
      lRet |= OWND_ADJRECT_CHANGED_B;
      }
    }
  if (lprcNew->bottom < lprcNew->top)
    {
    nTmp = lprcNew->bottom;
    lprcNew->bottom = lprcNew->top;
    lprcNew->top = nTmp;
    lRet |= OWND_ADJRECT_SWAPPED_TB;
    if (fAdjust & OWND_ADJRECT_ANCHOR_B)
      { //if we swap the sides, we need to swap the anchor points
      fAdjust &= ~OWND_ADJRECT_ANCHOR_B;
      fAdjust |= OWND_ADJRECT_ANCHOR_T;
      }
    else
      {
      fAdjust &= ~OWND_ADJRECT_ANCHOR_T;
      fAdjust |= OWND_ADJRECT_ANCHOR_B;
      }
    }     //else if (lprcNew->bottom < lprcNew->top)...

  if (fAdjust & OWND_ADJRECT_DEFAULT_W)
    {
    if(fAdjust & OWND_ADJRECT_ANCHOR_B)
      {
      if(m_pScroll->Vertical())
        lprcNew->left = lprcNew->right - sBarSize.x ;
      else
        lprcNew->left = lprcNew->right - 4*sBarSize.x ;
      lRet |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      if(m_pScroll->Vertical())
        lprcNew->right = lprcNew->left + sBarSize.x ;
      else
        lprcNew->right = lprcNew->left + 4*sBarSize.x ;
      lRet |= OWND_ADJRECT_CHANGED_R;
      }
    }
  if (lprcNew->right < lprcNew->left)
    {
    nTmp = lprcNew->right;
    lprcNew->right = lprcNew->left;
    lprcNew->left = nTmp;
    lRet |= OWND_ADJRECT_SWAPPED_LR;
    if (fAdjust & OWND_ADJRECT_ANCHOR_R)
      { //if we swap the sides, we need to swap the anchor points
      fAdjust &= ~OWND_ADJRECT_ANCHOR_R;
      fAdjust |= OWND_ADJRECT_ANCHOR_L;
      }
    else
      {
      fAdjust &= ~OWND_ADJRECT_ANCHOR_L;
      fAdjust |= OWND_ADJRECT_ANCHOR_R;
      }
    }     // else if (lprcNew->right < lprcNew->left)...

  //At this point, the rectangle is not flipped inside-out

  // Make sure it is the right width and height
  if (m_pScroll->DefaultSize() && m_pScroll->Horizontal() &&
      ((lprcNew->bottom - lprcNew->top) != sBarSize.y))
    {
    if (fAdjust & OWND_ADJRECT_ANCHOR_B)
      {
      lprcNew->top = lprcNew->bottom - sBarSize.y ;
      lRet |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      lprcNew->bottom = lprcNew->top + sBarSize.y ;
      lRet |= OWND_ADJRECT_CHANGED_B;
      }
    }     // if (m_pScroll->DefaultSize() &&...

  if (m_pScroll->DefaultSize() && m_pScroll->Vertical() &&
      ((lprcNew->right - lprcNew->left) != sBarSize.x))
    {
    if (fAdjust & OWND_ADJRECT_ANCHOR_R)
      {
      lprcNew->left = lprcNew->right - sBarSize.x;
      lRet |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      lprcNew->right = lprcNew->left + sBarSize.x;
      lRet |= OWND_ADJRECT_CHANGED_R;
      }
    }     // if (m_pScroll->DefaultSize() &&...

  // Set minimum width for scrollbars.
  if(m_pScroll->Horizontal() && ((lprcNew->right - lprcNew->left) < (2*sBarSize.x)))
    {
    if (fAdjust & OWND_ADJRECT_ANCHOR_R)
      {
      lprcNew->left = lprcNew->right - 2*sBarSize.x;
      lRet |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      lprcNew->right = lprcNew->left + 2*sBarSize.x;
      lRet |= OWND_ADJRECT_CHANGED_R;
      }
    }
  else if(m_pScroll->Vertical() && ((lprcNew->right - lprcNew->left) < (sBarSize.x/2)))
    {
    if (fAdjust & OWND_ADJRECT_ANCHOR_R)
      {
      lprcNew->left = lprcNew->right - sBarSize.x/2;
      lRet |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      lprcNew->right = lprcNew->left + sBarSize.x/2;
      lRet |= OWND_ADJRECT_CHANGED_R;
      }
    }

  if (m_pScroll->Vertical() && ((lprcNew->bottom - lprcNew->top) < (2*sBarSize.y)))
    {
    if (fAdjust & OWND_ADJRECT_ANCHOR_B)
      {
      lprcNew->top = lprcNew->bottom - 2*sBarSize.y;
      lRet |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      lprcNew->bottom = lprcNew->top + 2*sBarSize.y;
      lRet |= OWND_ADJRECT_CHANGED_B;
      }
    }
  else if(m_pScroll->Horizontal() && ((lprcNew->bottom - lprcNew->top) < (sBarSize.y/2)))
    {
    if (fAdjust & OWND_ADJRECT_ANCHOR_B)
      {
      lprcNew->top = lprcNew->bottom - sBarSize.x/2;
      lRet |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      lprcNew->bottom = lprcNew->top + sBarSize.x/2;
      lRet |= OWND_ADJRECT_CHANGED_B;
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::Destroy
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ScrollVI::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::DataChanged
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ScrollVI::DataChanged
  (
  long  lDetails,
  long  lModifiers,
  long  lVariation
  )

  {
  I_UNUSED(lDetails);
  I_UNUSED(lModifiers);
  I_UNUSED(lVariation);

  UpdateScrollPosition();
  UpdateScrollRange();
  UpdateEnabledState();
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::ParentOpened
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ScrollVI::ParentOpened
  (
  )

  {
  DWORD   dwStyle = WS_CHILD | (m_pScroll->Vertical() ? SBS_VERT : SBS_HORZ);
  #ifdef AW_I_DEBUG
  HWND    hwnd =
  #endif

  OWndCreateWindowEx(this, 0L, k_szAPWScrollBar, NULL, dwStyle, g_hinstDLL, this);
  I_ASSERT(m_hwndItem == hwnd);

  UpdateScrollRange();
  EnableWindow(m_hwndItem, m_pScroll->Enabled());
  if(m_pScroll->Enabled())
    UpdateScrollPosition();
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::ParentClosing
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ScrollVI::ParentClosing
  (
  )

  {
  DestroyWindow(m_hwndItem);
  m_hwndItem = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::ParentNotification
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

LRESULT ALMAPI ScrollVI::ParentNotification
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  LRESULT lRet;

  switch(uMsg)
    {
    case WM_HSCROLL: //!!PORT!!
      HANDLE_WM_HSCROLL(hwnd, wParam, lParam, this->ScrollMsg); //!!PORT!!
      lRet = 0L;
      break;

    case WM_VSCROLL: //!!PORT!!
      HANDLE_WM_VSCROLL(hwnd, wParam, lParam, this->ScrollMsg); //!!PORT!!
      lRet = 0L;
      break;

    default:
      lRet = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::InitDLL -- static
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ScrollVI::InitDLL
  (
  )

  {
  WNDCLASS  wc;

  GetClassInfo(NULL, k_szScrollBar, &wc);
  wc.style &= ~CS_GLOBALCLASS;

  sm_pfnSBarWndProc = wc.lpfnWndProc;
  wc.lpfnWndProc = ScrollVI::WndProc;
  wc.lpszClassName = k_szAPWScrollBar;
  wc.hInstance = g_hinstDLL;
  RegisterClass(&wc);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::FreeDLL -- static
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ScrollVI::FreeDLL
  (
  )

  {
  UnregisterClass(k_szAPWScrollBar, g_hinstDLL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::WndProc -- private, static
//
//  Description:  This function is the wnd proc for the scroll bar.
//
////////////////////////////////////////////////////////////////////////////

LRESULT ALMCALLBACK ScrollVI::WndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  switch(uMsg)
    {
    case WM_CREATE:
      if(((CREATESTRUCT*)lParam)->lpCreateParams)
        ((ScrollVI*)(((CREATESTRUCT*)lParam)->lpCreateParams))->m_hwndItem = hwnd;
      break;

    default:
      break;
    }

  return CallWindowProc(sm_pfnSBarWndProc, hwnd, uMsg, wParam, lParam);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::ScrollMsg -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ScrollVI::ScrollMsg
  (
  HWND  hwnd,
  HWND  hwndCtl,
  UINT  uCode,
  int   nPos
  )

  {
  I_UNUSED(hwnd);
  I_UNUSED(hwndCtl);

  BOOL            fPost = TRUE;
  PostSignalsEvt  pse   = {m_pScroll, 0L, 0L};
  AEvtInfo        evt   = {m_oiObject, m_oiObject, OSBAR_EVT_MAYBEPOSTSIGNALS,
                           (long)&pse, 0L, 0L, AEVT_ATONCE, 0L};

  switch(uCode)
    {
    case SB_BOTTOM:
      pse.lPosition = m_pScroll->Maximum();
      pse.lidSignal = k_lSignalBottom;
      break;

    case SB_ENDSCROLL:
      fPost = FALSE;
      break;

    case SB_LINEDOWN:
      if(m_pScroll->MinLTMax())
        pse.lPosition = m_pScroll->Position() + m_pScroll->LineScroll();
      else
        pse.lPosition = m_pScroll->Position() - m_pScroll->LineScroll();
      pse.lidSignal = k_lSignalLineDown;
      break;

    case SB_LINEUP:
      if(m_pScroll->MinLTMax())
        pse.lPosition = m_pScroll->Position() - m_pScroll->LineScroll();
      else
        pse.lPosition = m_pScroll->Position() + m_pScroll->LineScroll();
      pse.lidSignal = k_lSignalLineUp;
      break;

    case SB_PAGEDOWN:
      if(m_pScroll->MinLTMax())
        pse.lPosition = m_pScroll->Position() + m_pScroll->PageScroll();
      else
        pse.lPosition = m_pScroll->Position() - m_pScroll->PageScroll();
      pse.lidSignal = k_lSignalPageDown;
      break;

    case SB_PAGEUP:
      if(m_pScroll->MinLTMax())
        pse.lPosition = m_pScroll->Position() - m_pScroll->PageScroll();
      else
        pse.lPosition = m_pScroll->Position() + m_pScroll->PageScroll();
      pse.lidSignal = k_lSignalPageUp;
      break;

    case SB_THUMBPOSITION:
      pse.lPosition = VIPosToSBarPos(nPos);
      pse.lidSignal = k_lSignalThumbPosition;
      break;

    case SB_THUMBTRACK:
      pse.lPosition = VIPosToSBarPos(nPos);
      pse.lidSignal = k_lSignalThumbTrack;
      break;

    case SB_TOP:
      pse.lPosition = m_pScroll->Minimum();
      pse.lidSignal = k_lSignalTop;
      break;

    default:
      fPost = FALSE;
      break;
    } // switch(uCode) ...

  if(fPost)
    {
    AEvtPost(&evt);
    UpdateScrollPosition();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::VIPosToSBarPos -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long ScrollVI::VIPosToSBarPos
  (
  int nPos
  )

  {
  long  lDiv = 1L;
  long  lRet;

  while((Abs(m_pScroll->Maximum() - m_pScroll->Minimum()) / lDiv) > 0x00007FFFL)
    lDiv++;

  if((int)(Abs(m_pScroll->Maximum() - m_pScroll->Minimum()) / lDiv) == nPos)
    lRet = m_pScroll->Maximum();
  else if(m_pScroll->MinLTMax())
    lRet = m_pScroll->Minimum() + (lDiv * nPos);
  else
    lRet = m_pScroll->Minimum() - (lDiv * nPos);

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::SBarPosToVIPos -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

int ScrollVI::SBarPosToVIPos
  (
  )

  {
  int   nRet;
  long  lDiv = 1L;

  while((Abs(m_pScroll->Maximum() - m_pScroll->Minimum()) / lDiv) > 0x00007FFFL)
    lDiv++;

  if(m_pScroll->MinLTMax())
    nRet = (int)((m_pScroll->Position() - m_pScroll->Minimum()) / lDiv);
  else
    nRet = (int)(VIMin() - ((m_pScroll->Position() - m_pScroll->Maximum()) / lDiv));

  return nRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::VIMin -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

int ScrollVI::VIMin
  (
  )

  {
  int nRet;

  if(m_pScroll->MinLTMax())
    nRet = 0;
  else
    {
    long  lDiv = 1L;

    while(((m_pScroll->Minimum() - m_pScroll->Maximum()) / lDiv) > 0x00007FFFL)
      lDiv++;

    nRet = (int)((m_pScroll->Minimum() - m_pScroll->Maximum()) / lDiv);
    }

  return nRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::VIMax -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

int ScrollVI::VIMax
  (
  )

  {
  int nRet;

  if(!m_pScroll->MinLTMax())
    nRet = 0;
  else
    {
    long  lDiv = 1L;

    while(((m_pScroll->Maximum() - m_pScroll->Minimum()) / lDiv) > 0x00007FFFL)
      lDiv++;

    nRet = (int)((m_pScroll->Maximum() - m_pScroll->Minimum()) / lDiv);
    }

  return nRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::UpdateScrollPosition -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ScrollVI::UpdateScrollPosition
  (
  )

  {
  if(SBarPosToVIPos() != GetScrollPos(m_hwndItem, SB_CTL))
    SetScrollPos(m_hwndItem, SB_CTL, SBarPosToVIPos(), m_pScroll->Enabled());
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::UpdateScrollRange -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ScrollVI::UpdateScrollRange
  (
  )

  {
  int     nMin, nMax;

  GetScrollRange(m_hwndItem, SB_CTL, &nMin, &nMax);

  if(nMin!=VIMin() || nMax!=VIMax())
    {
    if(m_pScroll->MinLTMax())
      SetScrollRange(m_hwndItem, SB_CTL, VIMin(), VIMax(), FALSE);
    else
      SetScrollRange(m_hwndItem, SB_CTL, VIMax(), VIMin(), FALSE);
    SetScrollPos(m_hwndItem, SB_CTL, SBarPosToVIPos(), m_pScroll->Enabled());
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       ScrollVI::UpdateEnabledState -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void ScrollVI::UpdateEnabledState
  (
  )

  {
  EnableWindow(m_hwndItem, m_pScroll->Enabled());
  }

////////////////////////////////////////////////////////////////////////////
