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
// FILE:          "lay_prot.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Protocol class methods
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>

#include "wlay.h"
#include "lay_prot.h"
#include "lay_wnd.h"

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol constructor
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

Protocol::Protocol
  (
  Window* pWnd
  )

  {
  m_pWnd = pWnd;
  m_oiWnd = pWnd->ObjectID();
  m_fFrames = FALSE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol destructor
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

Protocol::~Protocol
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::CreateGrabHandle (private)
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void Protocol::CreateGrabHandle
  (
  RECT*       rcGrabHandle,
  const RECT* rcItem,
  int         nGrabHandle  // the POINT_IN_?? constants
  )

  {
  static int  I_DOT_SIZE  = 16; //1.6mm

  int   i;

  CopyRect(rcGrabHandle, rcItem);

  switch(nGrabHandle)
    {
    case OWND_POINT_IN_TL:
      SetRect(rcGrabHandle, rcGrabHandle->left, rcGrabHandle->top, rcGrabHandle->left+I_DOT_SIZE, rcGrabHandle->top+I_DOT_SIZE);
      break;

    case OWND_POINT_IN_TR:
      SetRect(rcGrabHandle, rcGrabHandle->right-I_DOT_SIZE, rcGrabHandle->top, rcGrabHandle->right, rcGrabHandle->top+I_DOT_SIZE);
      break;

    case OWND_POINT_IN_BL:
      SetRect(rcGrabHandle, rcGrabHandle->left, rcGrabHandle->bottom-I_DOT_SIZE, rcGrabHandle->left+I_DOT_SIZE, rcGrabHandle->bottom);
      break;

    case OWND_POINT_IN_BR:
      SetRect(rcGrabHandle, rcGrabHandle->right-I_DOT_SIZE, rcGrabHandle->bottom-I_DOT_SIZE, rcGrabHandle->right, rcGrabHandle->bottom);
      break;

    case OWND_POINT_IN_T:
      i = (rcGrabHandle->right + rcGrabHandle->left - I_DOT_SIZE)/2; //horz start for top & bottom
      SetRect(rcGrabHandle, i, rcGrabHandle->top, i+I_DOT_SIZE, rcGrabHandle->top+I_DOT_SIZE);
      break;

    case OWND_POINT_IN_L:
      i = (rcGrabHandle->bottom + rcGrabHandle->top - I_DOT_SIZE)/2; //vert start for left & right
      SetRect(rcGrabHandle, rcGrabHandle->left, i, rcGrabHandle->left+I_DOT_SIZE, i+I_DOT_SIZE);
      break;

    case OWND_POINT_IN_B:
      i = (rcGrabHandle->right + rcGrabHandle->left - I_DOT_SIZE)/2; //horz start for top & bottom
      SetRect(rcGrabHandle, i, rcGrabHandle->bottom-I_DOT_SIZE, i+I_DOT_SIZE, rcGrabHandle->bottom);
      break;

    case OWND_POINT_IN_R:
      i = (rcGrabHandle->bottom + rcGrabHandle->top - I_DOT_SIZE)/2; //vert start for left & right
      SetRect(rcGrabHandle, rcGrabHandle->right-I_DOT_SIZE, i, rcGrabHandle->right, i + I_DOT_SIZE);
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::PointInItem
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL Protocol::PointInItem
  (
  OpWndItemD* wi,
  LPPOINT     lppt
  )

  {
  BOOL  fRet;

  if(m_fFrames)
    {
    RECT  rcItem;

    MakeRealRect(&rcItem, &wi->m_rcItem);
    fRet = PtInRect(&rcItem, *lppt);
    }
  else if(OWndIsItemVisible(wi))
    fRet = wi->PointInItem(lppt);
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::ItemInRect
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL Protocol::ItemInRect
  (
  OpWndItemD* wi,
  LPRECT      lprc,
  BOOL        fReqFullEnclosure
  )

  {
  BOOL  fRet;
  RECT  rcItem;

  if(m_fFrames)
    {
    if(fReqFullEnclosure)
      {
      MakeRealRect(&rcItem, &wi->m_rcItem);

      fRet =  (lprc->left   <= rcItem.left)
                     &&
              (lprc->top    <= rcItem.top)
                     &&
              (lprc->right  >= rcItem.right)
                     &&
              (lprc->bottom >= rcItem.bottom);
      }
    else
      {
      RECT  rcIntr;
      RECT  rcTemp;

      MakeRealRect(&rcItem, &wi->m_rcItem);
      CopyRect(&rcTemp, lprc);

      if (rcTemp.left == rcTemp.right) rcTemp.right++;
      if (rcTemp.top == rcTemp.bottom) rcTemp.bottom++;

      fRet = IntersectRect(&rcIntr, &rcTemp, &rcItem);
      }
    }
  else
    fRet = wi->ItemInRect(lprc, fReqFullEnclosure);

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::PointInGrabHandle
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

long Protocol::PointInGrabHandle
  (
  OpWndItemD* wi,
  LPPOINT     lppt,
  HCURSOR*    hcursor
  )

  {
  long  lRet;

  if(m_fFrames)
    {
    RECT  rcItem;
    RECT  rcGrabHandle;
    int   i;
    BOOL  fSwapTB = wi->m_rcItem.bottom < wi->m_rcItem.top;
    BOOL  fSwapLR = wi->m_rcItem.right < wi->m_rcItem.left;

    MakeRealRect(&rcItem, &wi->m_rcItem);
    lRet = OWND_POINT_IN_NONE;
    for(i=OWND_POINT_IN_TL; (i<=OWND_POINT_IN_R) && (lRet==OWND_POINT_IN_NONE); i++)
      {
      CreateGrabHandle(&rcGrabHandle, &rcItem, i);
      if (PtInRect(&rcGrabHandle, *lppt) )
        lRet = i;
      }

    if(fSwapTB && fSwapLR)
      {
      switch((int)lRet)
        {
        case OWND_POINT_IN_TL:
          lRet = OWND_POINT_IN_BR;
          break;

        case OWND_POINT_IN_TR:
          lRet = OWND_POINT_IN_BL;
          break;

        case OWND_POINT_IN_BL:
          lRet = OWND_POINT_IN_TR;
          break;

        case OWND_POINT_IN_BR:
          lRet = OWND_POINT_IN_TL;
          break;

        case OWND_POINT_IN_T:
          lRet = OWND_POINT_IN_B;
          break;

        case OWND_POINT_IN_L:
          lRet = OWND_POINT_IN_R;
          break;

        case OWND_POINT_IN_B:
          lRet = OWND_POINT_IN_T;
          break;

        case OWND_POINT_IN_R:
          lRet = OWND_POINT_IN_L;
          break;
        }
      }
    else if(fSwapTB)
      {
      switch((int)lRet)
        {
        case OWND_POINT_IN_TL:
          lRet = OWND_POINT_IN_BL;
          *hcursor = LoadCursor(NULL, IDC_SIZENWSE);
          break;

        case OWND_POINT_IN_TR:
          lRet = OWND_POINT_IN_BR;
          *hcursor = LoadCursor(NULL, IDC_SIZENESW);
          break;

        case OWND_POINT_IN_BL:
          lRet = OWND_POINT_IN_TL;
          *hcursor = LoadCursor(NULL, IDC_SIZENESW);
          break;

        case OWND_POINT_IN_BR:
          lRet = OWND_POINT_IN_TR;
          *hcursor = LoadCursor(NULL, IDC_SIZENWSE);
          break;

        case OWND_POINT_IN_T:
          lRet = OWND_POINT_IN_B;
          break;

        case OWND_POINT_IN_B:
          lRet = OWND_POINT_IN_T;
          break;
        }
      }
    else if(fSwapLR)
      {
      switch((int)lRet)
        {
        case OWND_POINT_IN_TL:
          lRet = OWND_POINT_IN_TR;
          *hcursor = LoadCursor(NULL, IDC_SIZENWSE);
          break;

        case OWND_POINT_IN_TR:
          lRet = OWND_POINT_IN_TL;
          *hcursor = LoadCursor(NULL, IDC_SIZENESW);
          break;

        case OWND_POINT_IN_BL:
          lRet = OWND_POINT_IN_BR;
          *hcursor = LoadCursor(NULL, IDC_SIZENESW);
          break;

        case OWND_POINT_IN_BR:
          lRet = OWND_POINT_IN_BL;
          *hcursor = LoadCursor(NULL, IDC_SIZENWSE);
          break;

        case OWND_POINT_IN_L:
          lRet = OWND_POINT_IN_R;
          break;

        case OWND_POINT_IN_R:
          lRet = OWND_POINT_IN_L;
          break;
        }
      }
    }
  else
    lRet = wi->PointInGrabHandle(lppt, hcursor);

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::InvalidateGrabHandles
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void Protocol::InvalidateGrabHandles
  (
  OpWndItemD* wi,
  BOOL        fErase
  )

  {
  if(m_fFrames)
    {
    RECT  rcItem;
    RECT  rcInvalid;
    int   i;

    MakeRealRect(&rcItem, &wi->m_rcItem);

    for (i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
      {
      CreateGrabHandle(&rcInvalid, &rcItem, i);
      OWndInvalidateLogicalRect(m_oiWnd, &rcInvalid, fErase);
      }
    }
  else
    wi->InvalidateGrabHandles(fErase);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::DrawGrabHandles
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void Protocol::DrawGrabHandles
  (
  OpWndItemD* wi,
  HDC         hdc
  )

  {
  if(m_fFrames)
    {
    RECT  rcItem;
    RECT  rcGrabHandle;
    int   i;

    MakeRealRect(&rcItem, &wi->m_rcItem);

    for (i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
      {
      CreateGrabHandle(&rcGrabHandle, &rcItem, i);
      Rectangle(hdc, rcGrabHandle.left, rcGrabHandle.top, rcGrabHandle.right, rcGrabHandle.bottom);
      }
    }
  else
    wi->DrawGrabHandles(hdc);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::DrawTrackingRect
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void Protocol::DrawTrackingRect
  (
  OpWndItemD* wi,
  HDC         hdc,
  LPRECT      lprc
  )

  {
  if(m_fFrames)
    {
    HBRUSH  hbrOld = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RECT    rc;

    MakeRealRect(&rc, lprc);
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

    SelectObject(hdc, hbrOld);
    }
  else
    wi->DrawTrackingRect(hdc, lprc);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::AdjustNewRect
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

long Protocol::AdjustNewRect
  (
  OpWndItemD* wi,
  LPRECT      lprc,
  long        fAdjust
  )

  {
  // make sure the borders are on pixel boundaries
  OWndLPtoDP(m_oiWnd, (LPPOINT)lprc, 2);
  OWndDPtoLP(m_oiWnd, (LPPOINT)lprc, 2);
  
  return wi->AdjustNewRect(lprc, fAdjust);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::Move
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void Protocol::Move
  (
  OpWndItemD* wi,
  LPRECT      lprc
  )

  {
  // make sure the borders are on pixel boundaries
  OWndLPtoDP(m_oiWnd, (LPPOINT)lprc, 2);
  OWndDPtoLP(m_oiWnd, (LPPOINT)lprc, 2);

  if(m_fFrames)
    {
    RECT  rcOld;

    CopyRect(&rcOld, &wi->m_rcItem);
    InvalidateGrabHandles(wi, TRUE);
    wi->Move(lprc);
    InvalidateGrabHandles(wi, FALSE);
    if(OWndIsHwndItem(wi) && !EqualRect(&rcOld, &wi->m_rcItem))
      {
      OWndInvalidateLogicalRect(m_oiWnd, &rcOld, TRUE);
      OWndInvalidateLogicalRect(m_oiWnd, &wi->m_rcItem, TRUE);
      }
    }
  else
    {
    wi->InvalidateGrabHandles(TRUE);
    wi->Move(lprc);
    wi->InvalidateGrabHandles(FALSE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Protocol::InvalidateItem
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void Protocol::InvalidateItem
  (
  OpWndItemD* wi,
  BOOL        fErase
  )

  {
  if(m_fFrames)
    {
    RECT  rcItem;

    MakeRealRect(&rcItem, &wi->m_rcItem);
    InflateRect(&rcItem, 16, 16);
    OWndInvalidateLogicalRect(m_oiWnd, &rcItem, fErase);
    }
  else
    wi->InvalidateItem(fErase);
  }

////////////////////////////////////////////////////////////////////////////
