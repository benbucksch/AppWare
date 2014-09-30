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
// FILE:          "lay_item.cpp"
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

#include <stdlib.h>
#include <limits.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_wndlay.h>
#include <o_wndgrp.h>

#include "xp_macro.h"
#include "wlay.h"
#include "lay_dlg.h"
#include "lay_wnd.h"
#include "lay_shap.h"
#include "layout.h"
#include "rc\wlay_rsc.h"
#include "objlist.h"

// defines /////////////////////////////////////////////////////////////////

#define MAX_NAMELENGTH    64

// externs /////////////////////////////////////////////////////////////////

extern HINSTANCE  g_hInstance;

// struct SPACEEVENLY //////////////////////////////////////////////////////

typedef struct
  {
  OpWndItemD* wi;
  int         nStart;
  } SPACEEVENLY;

// fcmpSpaceEvenly /////////////////////////////////////////////////////////

int fcmpSpaceEvenly
  (
  const void* pse1,
  const void* pse2
  )

  {
  return ((SPACEEVENLY*)pse1)->nStart-((SPACEEVENLY*)pse2)->nStart;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items Constructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

Items::Items
  (
  Window* pWnd
  ) : Protocol(pWnd)

  {
  m_hwndParent = NULL;

  // initialize the items information
  m_nItems = 0;
  m_miItems = AMemAllocate(0L);
  m_pItems = NULL;

  // initialize the tracking flags
  m_fTrack = 0;
  m_wiPrimary = NULL;
  SetPointEmpty(&m_ptStart);
  SetRectEmpty(&m_rcLast);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items Destructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

Items::~Items
  (
  )

  {
  I_ASSERT(m_nItems==0);

  if(m_nItems)
    AMemUnlock(m_miItems);

  AMemFree(m_miItems);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GetPrevItem (private)
//
//  DESCRIPTION:  This function gets the previous item in the list
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* Items::GetPrevItem
  (
  OpWndItemD* wi,
  BOOL        fSkipGroups
  )

  {
  OpWndItemD* wiRet;
  int         idx;

  if(wi)
    {
    idx = wi->m_index;

    if(fSkipGroups && OWndLayIsItemGroup(wi))
      {
      idx -= ((OpGroupWndItem*)wi)->NumItems(TRUE);
      I_ASSERT((idx>=0) && (idx<m_nItems));
      wiRet = (idx<=0) ? NULL : m_pItems[idx-1].wi;
      }
    else if(!fSkipGroups && OWndLayIsItemGroup(wi))
      {
      idx--;
      while((idx>=0) && OWndLayIsItemGroup(m_pItems[idx].wi))
        idx--;

      I_ASSERT((idx>=0) && (idx<m_nItems));
      wiRet = (idx<0) ? NULL : m_pItems[idx].wi;
      }
    else
      {
      I_ASSERT((idx>=0) && (idx<m_nItems));
      wiRet = (idx<=0) ? NULL : m_pItems[idx-1].wi;
      }
    }
  else if(m_nItems)
    {
    idx = m_nItems - 1;
    if(!fSkipGroups)
      {
      while((idx>=0) && OWndLayIsItemGroup(m_pItems[idx].wi))
        idx--;
      }

    wiRet = (idx<0) ? NULL : m_pItems[idx].wi;
    }
  else
    wiRet = NULL;

  return wiRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GetNextItem (private)
//
//  DESCRIPTION:  This function gets the next item in the list
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* Items::GetNextItem
  (
  OpWndItemD* wi,
  BOOL        fSkipGroups
  )

  {
  OpWndItemD* wiRet;

  if(wi)
    {
    int idx = wi->m_index;

    I_ASSERT((idx>=0) && (idx<m_nItems));
    if(idx==(m_nItems-1))
      wiRet = NULL;
    else if(fSkipGroups && IsItemGrouped(idx+1))
      {
      idx++;
      while(IsItemGrouped(idx))
        idx = m_pItems[idx].nGroup;

      I_ASSERT(idx<m_nItems);
      wiRet = m_pItems[idx].wi;
      }
    else if(!fSkipGroups && OWndLayIsItemGroup(m_pItems[idx+1].wi))
      {
      idx++;
      while((idx<m_nItems) && OWndLayIsItemGroup(m_pItems[idx].wi))
        idx++;

      wiRet = (idx<m_nItems) ? m_pItems[idx].wi : NULL;
      }
    else
      wiRet = m_pItems[idx+1].wi;
    }
  else if(m_nItems)
    {
    if(fSkipGroups && IsItemGrouped(0))
      {
      int idx = 0;
      while((idx<m_nItems) && IsItemGrouped(idx))
        idx = m_pItems[idx].nGroup;

      I_ASSERT(idx<m_nItems);
      wiRet = m_pItems[idx].wi;
      }
    else
      wiRet = m_pItems[0].wi;
    }
  else
    wiRet = NULL;

  return wiRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SelectItem (private)
//
//  DESCRIPTION:  This selects the specified item (or the item's group) and
//                returns the index of the item that was selected.
//
////////////////////////////////////////////////////////////////////////////

int Items::SelectItem
  (
  int   idxItem,
  BOOL  fSelect
  )

  {
  int i = idxItem;

  // skip to the group if the item is grouped -- allow nested groups
  while(m_pItems[i].nGroup > 0)
    {
    I_ASSERT(IsItemGrouped(i));
    i = m_pItems[i].nGroup;
    I_ASSERT(i<m_nItems);
    }

  // verify the current item is not a group
  I_ASSERT(!IsItemGrouped(i));

  // do the selection
  if(!fSelect && IsItemSelected(i))
    {
    m_pItems[i].flags &= ~FLAG_SELECTED;
    m_nSel--;
    InvalidateGrabHandles(m_pItems[i].wi, TRUE);
    }
  else if(fSelect  && !IsItemSelected(i) && !IsItemLocked(i) && (m_fFrames || OWndIsItemVisible(m_pItems[i].wi)))
    {
    m_pItems[i].flags |= FLAG_SELECTED;
    m_nSel++;
    InvalidateGrabHandles(m_pItems[i].wi, FALSE);
    }

  return i;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GetSelectionBounds (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

int Items::GetSelectionBounds
  (
  COORDINATE* left,
  COORDINATE* top,
  COORDINATE* right,
  COORDINATE* bottom
  )

  {
  OpWndItemD* wi = NULL;
  int         nSel = 0;
  RECT        rc;

  I_ASSERT(left || top || right || bottom);

  if(left)   *left   = INT_MAX;
  if(top)    *top    = INT_MAX;
  if(right)  *right  = INT_MIN;
  if(bottom) *bottom = INT_MIN;

  // get the bounding rectangle of the selections
  while((wi=GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      nSel++;

      MakeRealRect(&rc, &wi->m_rcItem);

      if(left   && (rc.left   < *left  ))
        *left   = rc.left;
      if(top    && (rc.top    < *top   ))
        *top    = rc.top;
      if(right  && (rc.right  > *right ))
        *right  = rc.right;
      if(bottom && (rc.bottom > *bottom))
        *bottom = rc.bottom;
      }
    }

  return nSel;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GetMinMaxSizes (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

int Items::GetMinMaxSizes
  (
  COORDINATE* minW,
  COORDINATE* maxW,
  COORDINATE* minH,
  COORDINATE* maxH
  )

  {
  OpWndItemD* wi = NULL;
  int         nSel = 0;
  RECT        rc;

  I_ASSERT(minW || maxW || minH || maxH);

  if(minW) *minW = INT_MAX;
  if(maxW) *maxW = INT_MIN;
  if(minH) *minH = INT_MAX;
  if(maxH) *maxH = INT_MIN;

  // get the extreme widths and heights
  while((wi=GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      nSel++;

      MakeRealRect(&rc, &wi->m_rcItem);

      if(minW && ((rc.right - rc.left) < *minW))
        *minW = rc.right - rc.left;
      if(maxW && ((rc.right - rc.left) > *maxW))
        *maxW = rc.right - rc.left;
      if(minH && ((rc.bottom - rc.top) < *minH))
        *minH = rc.bottom - rc.top;
      if(maxH && ((rc.bottom - rc.top) > *maxH))
        *maxH = rc.bottom - rc.top;
      }
    }

  return nSel;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DeleteItem (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DeleteItem
  (
  OpWndItemD* wi
  )

  {
  I_ASSERT(wi);

  int   i, j;
  int   idx       = wi->m_index;
  int   nItems    = OWndLayIsItemGroup(wi) ? ((OpGroupWndItem*)wi)->NumItems(TRUE) + 1 : 1;
  BOOL  fGrouped  = IsItemGrouped(idx);
  BOOL  nGroup    = m_pItems[idx].nGroup - nItems;

  // delete the item(s)
  for(i=nItems-1; i>=0; i--)
    {
    if(OWndIsHwndItem(m_pItems[idx-i].wi))
      UnsubclassHwndItem(((OpHwndWndItemD*)m_pItems[idx-i].wi)->m_hwndItem);

    if(IsItemSelected(idx-i))
      {
      m_nSel--;
      InvalidateGrabHandles(m_pItems[idx-i].wi, TRUE);
      }

    if(m_fFrames)
      {
      RECT  rc;
      POINT pt = {1, 1};

      OWndDStoLS(m_oiWnd, &pt, 1);
      MakeRealRect(&rc, &m_pItems[idx-i].wi->m_rcItem);

      if(rc.left==rc.right)
        rc.right += pt.x;
      if(rc.top==rc.bottom)
        rc.bottom += pt.y;

      OWndInvalidateLogicalRect(m_oiWnd, &rc, TRUE);
      }
    else if(OWndIsLiteItem(m_pItems[idx-i].wi) && OWndIsItemVisible(m_pItems[idx-i].wi))
      InvalidateItem(m_pItems[idx-i].wi, TRUE);

    m_pItems[idx-i].wi->ParentClosing();
    OWndLayDestroyItem(m_pItems[idx-i].wi);
    }

  // fix the list -- make sure the indexes and ctl ids are correct
  for(j = idx + 1; j<m_nItems; j++)
    {
    m_pItems[j-nItems] = m_pItems[j];
    m_pItems[j-nItems].wi->m_index = j - nItems;

    if(OWndIsHwndItem(m_pItems[j-nItems].wi))
      SetItemCtlID(((OpHwndWndItemD*)m_pItems[j-nItems].wi)->m_hwndItem, m_pItems[j-nItems].wi->m_index + OWNDLAY_ITEM_ID_OFFSET);

    // fix the group index
    if(m_pItems[j-nItems].nGroup>0)
      m_pItems[j-nItems].nGroup -= nItems;
    }

  // resize the list of items
  m_nItems  -= nItems;
  AMemUnlock(m_miItems);
  AMemSetSize(m_miItems, m_nItems*sizeof(WND_ITEMS));
  m_pItems = (WND_ITEMS*)AMemLock(m_miItems);

  if(fGrouped)
    {
    // notify the group that an item was deleted
    I_ASSERT(nGroup>0);

    OpWndItemD* wiTmp       = NULL;
    int         nGroupItems;

    if(IsItemSelected(nGroup))
      InvalidateGrabHandles(m_pItems[nGroup].wi, TRUE);

    nGroupItems = ((OpGroupWndItem*)m_pItems[nGroup].wi)->ItemDeleted(wi);

    // fix the group index for all items in the group
    while((wiTmp = ((OpGroupWndItem*)m_pItems[nGroup].wi)->GetNextItem(wiTmp, TRUE)) != NULL)
      m_pItems[wiTmp->m_index].nGroup = nGroup;

    // do the necessary ungrouping -- handle nested problems...
    if(nGroupItems<2)
      DeleteGroup((OpGroupWndItem*)m_pItems[nGroup].wi);
    else if(IsItemSelected(idx-nItems))
      InvalidateGrabHandles(m_pItems[nGroup].wi, FALSE);
    }

  m_pWnd->SetDirtyFlag();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DeleteGroup (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DeleteGroup
  (
  OpGroupWndItem* gwi
  )

  {
  I_ASSERT(gwi && OWndLayIsItemGroup(gwi));

  int   i;
  int   j         = 0;
  int   nItems    = gwi->NumItems(FALSE);
  int   idx       = gwi->m_index;
  int   nGroup    = m_pItems[idx].nGroup;
  BOOL  fGrouped  = IsItemGrouped(idx);

  // invalidate the group item
  if(IsItemSelected(idx))
    InvalidateGrabHandles(gwi, TRUE);
  if(fGrouped && IsItemSelected(nGroup))
    InvalidateGrabHandles(m_pItems[nGroup].wi, TRUE);
  if(m_fFrames)
    InvalidateItem(gwi, TRUE);

  // notify the group that it will be destroyed and needs to restore its items
  gwi->Ungrouped();
  if(fGrouped)
    ((OpGroupWndItem*)m_pItems[nGroup].wi)->ItemDeleted(gwi);

  for(i=1; i<=nItems; i++)
    {
    I_ASSERT((idx-i-j) >= 0);

    // ungroup the item
    SetItemGrouped(idx-i-j, fGrouped);
    m_pItems[idx-i-j].nGroup = nGroup;

    if(fGrouped)
      ((OpGroupWndItem*)m_pItems[nGroup].wi)->ItemAdded(m_pItems[idx-i-j].wi);

    // calculate the extra offset if the item is a group
    if(OWndLayIsItemGroup(m_pItems[idx-i-j].wi))
      j += ((OpGroupWndItem*)m_pItems[idx-i-j].wi)->NumItems(TRUE);
    }

  // delete item and fix the list -- make sure the indexes and ctl ids are correct
  OWndLayDestroyItem(gwi);

  for(j = idx + 1; j<m_nItems; j++)
    {
    m_pItems[j-1] = m_pItems[j];
    m_pItems[j-1].wi->m_index = j - 1;

    if(OWndIsHwndItem(m_pItems[j-1].wi))
      SetItemCtlID(((OpHwndWndItemD*)m_pItems[j-1].wi)->m_hwndItem, m_pItems[j-1].wi->m_index + OWNDLAY_ITEM_ID_OFFSET);

    // redraw the item to fix the item index
    if(m_fFrames)
      InvalidateItem(m_pItems[j-1].wi, TRUE);

    // fix the group index
    if(m_pItems[j-1].nGroup>0)
      m_pItems[j-1].nGroup--;
    }

  // resize the list of items
  m_nItems--;
  AMemUnlock(m_miItems);
  AMemSetSize(m_miItems, m_nItems*sizeof(WND_ITEMS));
  m_pItems = (WND_ITEMS*)AMemLock(m_miItems);

  // fix the group pointers for nested groups
  if(fGrouped)
    {
    OpWndItemD* wi = NULL;
    while((wi = ((OpGroupWndItem*)m_pItems[nGroup-1].wi)->GetNextItem(wi, TRUE)) != NULL)
      m_pItems[wi->m_index].nGroup = nGroup - 1;

    if(IsItemSelected(nGroup-1))
      InvalidateGrabHandles(m_pItems[nGroup-1].wi, FALSE);
    }
  }

#ifdef AW_I_DEBUG
////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ResetCtlIDs (private)
//
//  DESCRIPTION:  This will make sure that all the control ids are the
//                required id.
//
////////////////////////////////////////////////////////////////////////////

void Items::ResetCtlIDs
  (
  )

  {
  OpWndItemD* wi = NULL;
  int         idCtl;

  while((wi = GetNextItem(wi, FALSE)) != NULL)
    {
    if(OWndIsHwndItem(wi))
      {
      idCtl = GetItemCtlID(((OpHwndWndItemD*)wi)->m_hwndItem);
      if(idCtl != (wi->m_index + OWNDLAY_ITEM_ID_OFFSET))
        {
        I_ASSERT(0);
        SetItemCtlID(((OpHwndWndItemD*)wi)->m_hwndItem, wi->m_index + OWNDLAY_ITEM_ID_OFFSET);
        }
      } // OWndIsHwndItem
    } // while
  }
#endif

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Track (private)
//
//  DESCRIPTION:  This will process the mouse moves when the items are
//                tracking.
//
////////////////////////////////////////////////////////////////////////////

void Items::Track
  (
  LPPOINT lppt,
  int     fwKeys,
  BOOL    fDraw,
  BOOL    fToggle
  )

  {
  I_ASSERT(m_fTrack && lppt && (fToggle || fDraw));

  HDC     hdc           = GetDC(m_hwndParent);
  HPEN    hpen          = CreatePen(PS_DOT, 1, COLOR_WINDOWTEXT);
  HPEN    hpenOld       = (HPEN)SelectObject(hdc, hpen);
  HBRUSH  hbrOld        = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
  int     nOldDrawMode  = SetROP2(hdc, R2_NOTXORPEN);

  if(m_fTrack & TRACK_RESIZEMASK)
    TrackResize(hdc, lppt, fwKeys, fDraw, fToggle);
  else if(m_fTrack & TRACK_MOVE)
    TrackMove(hdc, lppt, fDraw, fToggle);
  else if(m_fTrack & TRACK_SELECT)
    TrackSelect(hdc, lppt, fDraw, fToggle);

  SelectObject(hdc, hpenOld);
  DeleteObject(hpen);
  SelectObject(hdc, hbrOld);
  SetROP2(hdc, nOldDrawMode);
  ReleaseDC(m_hwndParent, hdc);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::TrackResize (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::TrackResize
  (
  HDC     hdc,
  LPPOINT lppt,
  int     fwKeys,
  BOOL    fDraw,
  BOOL    fToggle
  )

  {
  I_UNUSED(fwKeys);

  I_ASSERT(m_wiPrimary);
  POINT       ptDelta     = {lppt->x - m_ptStart.x, lppt->y - m_ptStart.y};
  RECT        rc, rcTrack;
  double      dX, dY;
  POINT       pt          = {0, 0};
  OpWndItemD* wi          = NULL;

  CopyRect(&rc, &m_wiPrimary->m_rcItem);
  switch((int)(m_fTrack & TRACK_RESIZEMASK))
    {
    case (int)TRACK_RESIZEL:
      dX = (rc.right==rc.left) ? 1. : (double)(rc.right - rc.left - ptDelta.x) / (double)(rc.right - rc.left);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.x = (wi==m_wiPrimary) ? rc.left + ptDelta.x : wi->m_rcItem.right - dX * (wi->m_rcItem.right - wi->m_rcItem.left);
          m_pWnd->SnapPointToGrid(&pt);

          rcTrack.left = (pt.x>0) ? pt.x : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_R | OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_B);
          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    case (int)TRACK_RESIZET:
      dY = (rc.bottom==rc.top) ? 1. : (double)(rc.bottom - rc.top - ptDelta.y) / (double)(rc.bottom - rc.top);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.y = (wi==m_wiPrimary) ? rc.top + ptDelta.y : wi->m_rcItem.bottom - dY * (wi->m_rcItem.bottom - wi->m_rcItem.top);
          m_pWnd->SnapPointToGrid(&pt);
          rcTrack.top = (pt.y>0) ? pt.y : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_R | OWND_ADJRECT_ANCHOR_B);
          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    case (int)TRACK_RESIZER:
      dX = (rc.right==rc.left) ? 1. : (double)(rc.right + ptDelta.x - rc.left) / (double)(rc.right - rc.left);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.x = (wi==m_wiPrimary) ? rc.right + ptDelta.x : wi->m_rcItem.left + dX * (wi->m_rcItem.right - wi->m_rcItem.left);
          m_pWnd->SnapPointToGrid(&pt);
          rcTrack.right = (pt.x>0) ? pt.x : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_B);

          if(nMin(rcTrack.left, rcTrack.right) < 0)
            {
            rcTrack.right = rcTrack.left = nMax(rcTrack.left, rcTrack.right);
            AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_B);
            }

          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    case (int)TRACK_RESIZEB:
      dY = (rc.bottom==rc.top) ? 1. : (double)(rc.bottom + ptDelta.y - rc.top) / (double)(rc.bottom - rc.top);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.y = (wi==m_wiPrimary) ? rc.bottom + ptDelta.y : wi->m_rcItem.top + dY * (wi->m_rcItem.bottom - wi->m_rcItem.top);
          m_pWnd->SnapPointToGrid(&pt);
          rcTrack.bottom = (pt.y>0) ? pt.y : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_R);

          if(nMin(rcTrack.top, rcTrack.bottom) < 0)
            {
            rcTrack.bottom = rcTrack.top = nMax(rcTrack.top, rcTrack.bottom);
            AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_R);
            }

          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    case (int)TRACK_RESIZETL:
      dX = (rc.right==rc.left) ? 1. : (double)(rc.right - rc.left - ptDelta.x) / (double)(rc.right - rc.left);
      dY = (rc.bottom==rc.top) ? 1. : (double)(rc.bottom - rc.top - ptDelta.y) / (double)(rc.bottom - rc.top);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.x = (wi==m_wiPrimary) ? rc.left + ptDelta.x : wi->m_rcItem.right - dX * (wi->m_rcItem.right - wi->m_rcItem.left);
          pt.y = (wi==m_wiPrimary) ? rc.top + ptDelta.y : wi->m_rcItem.bottom - dY * (wi->m_rcItem.bottom - wi->m_rcItem.top);
          m_pWnd->SnapPointToGrid(&pt);

          rcTrack.left = (pt.x>0) ? pt.x : 0;
          rcTrack.top = (pt.y>0) ? pt.y : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_R | OWND_ADJRECT_ANCHOR_B);
          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    case (int)TRACK_RESIZEBL:
      dX = (rc.right==rc.left) ? 1. : (double)(rc.right - rc.left - ptDelta.x) / (double)(rc.right - rc.left);
      dY = (rc.bottom==rc.top) ? 1. : (double)(rc.bottom + ptDelta.y - rc.top) / (double)(rc.bottom - rc.top);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.x = (wi==m_wiPrimary) ? rc.left + ptDelta.x : wi->m_rcItem.right - dX * (wi->m_rcItem.right - wi->m_rcItem.left);
          pt.y = (wi==m_wiPrimary) ? rc.bottom + ptDelta.y : wi->m_rcItem.top + dY * (wi->m_rcItem.bottom - wi->m_rcItem.top);
          m_pWnd->SnapPointToGrid(&pt);
          rcTrack.left = (pt.x>0) ? pt.x : 0;
          rcTrack.bottom = (pt.y>0) ? pt.y : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_R | OWND_ADJRECT_ANCHOR_T);

          if(nMin(rcTrack.top, rcTrack.bottom) < 0)
            {
            rcTrack.bottom = rcTrack.top = nMax(rcTrack.top, rcTrack.bottom);
            AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_R | OWND_ADJRECT_ANCHOR_T);
            }

          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    case (int)TRACK_RESIZETR:
      dX = (rc.right==rc.left) ? 1. : (double)(rc.right + ptDelta.x - rc.left) / (double)(rc.right - rc.left);
      dY = (rc.bottom==rc.top) ? 1. : (double)(rc.bottom - rc.top - ptDelta.y) / (double)(rc.bottom - rc.top);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.x = (wi==m_wiPrimary) ? rc.right + ptDelta.x : wi->m_rcItem.left + dX * (wi->m_rcItem.right - wi->m_rcItem.left);
          pt.y = (wi==m_wiPrimary) ? rc.top + ptDelta.y : wi->m_rcItem.bottom - dY * (wi->m_rcItem.bottom - wi->m_rcItem.top);
          m_pWnd->SnapPointToGrid(&pt);
          rcTrack.right = (pt.x>0) ? pt.x : 0;
          rcTrack.top = (pt.y>0) ? pt.y : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_B);

          if(nMin(rcTrack.left, rcTrack.right) < 0)
            {
            rcTrack.right = rcTrack.left = nMax(rcTrack.left, rcTrack.right);
            AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_B);
            }

          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    case (int)TRACK_RESIZEBR:
      dX = (rc.right==rc.left) ? 1. : (double)(rc.right + ptDelta.x - rc.left) / (double)(rc.right - rc.left);
      dY = (rc.bottom==rc.top) ? 1. : (double)(rc.bottom + ptDelta.y - rc.top) / (double)(rc.bottom - rc.top);
      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          CopyRect(&rcTrack, &wi->m_rcItem);
          pt.x = (wi==m_wiPrimary) ? rc.right + ptDelta.x : wi->m_rcItem.left + dX * (wi->m_rcItem.right - wi->m_rcItem.left);
          pt.y = (wi==m_wiPrimary) ? rc.bottom + ptDelta.y : wi->m_rcItem.top + dY * (wi->m_rcItem.bottom - wi->m_rcItem.top);
          m_pWnd->SnapPointToGrid(&pt);

          rcTrack.right = (pt.x>0) ? pt.x : 0;
          rcTrack.bottom = (pt.y>0) ? pt.y : 0;

          AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);

          if(nMin(rcTrack.top, rcTrack.bottom) < 0)
            {
            rcTrack.bottom = rcTrack.top = nMax(rcTrack.top, rcTrack.bottom);
            AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
            }
          if(nMin(rcTrack.left, rcTrack.right) < 0)
            {
            rcTrack.right = rcTrack.left = nMax(rcTrack.left, rcTrack.right);
            AdjustNewRect(wi, &rcTrack, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
            }

          DrawTrackingRects(wi, hdc, &rcTrack, fDraw, fToggle);
          }
        }
      break;

    default:
      I_ASSERT(0);
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::TrackMove (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::TrackMove
  (
  HDC     hdc,
  LPPOINT lppt,
  BOOL    fDraw,
  BOOL    fToggle
  )

  {
  OpWndItemD* wi        = NULL;
  POINT       ptDelta;
  POINT       pt;
  RECT        rc;

  m_pWnd->SnapPointToGrid(lppt);
  SetPoint(&ptDelta, m_rcMaxMove.left + lppt->x - m_ptStart.x, m_rcMaxMove.top + lppt->y - m_ptStart.y);
  if(ptDelta.x < 0)
    ptDelta.x = 0;
  if(ptDelta.y < 0)
    ptDelta.y = 0;
  m_pWnd->SnapPointToGrid(&ptDelta);
  ptDelta.x -= m_rcMaxMove.left;
  ptDelta.y -= m_rcMaxMove.top;

  while((wi = GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      pt.x = wi->m_rcItem.left + ptDelta.x;
      pt.y = wi->m_rcItem.top  + ptDelta.y;
      SetRect(&rc, pt.x, pt.y, wi->m_rcItem.right + ptDelta.x, wi->m_rcItem.bottom + ptDelta.y);

      AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
      DrawTrackingRects(wi, hdc, &rc, fDraw, fToggle);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DrawTrackingRects (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DrawTrackingRects
  (
  OpWndItemD* wi,
  HDC         hdc,
  LPRECT      lprc,
  BOOL        fDraw,
  BOOL        fToggle
  )

  {
  I_ASSERT(wi);

  if((fToggle && !fDraw) || (fDraw && !fToggle) || !EqualRect(lprc, &m_pItems[wi->m_index].rcTrack))
    {
    // remove the old tracking rect
    if(fToggle)
      {
      DrawTrackingRect(wi, hdc, &m_pItems[wi->m_index].rcTrack);
      if(!fDraw)
        CopyRect(&m_pItems[wi->m_index].rcTrack, lprc);
      }

    // draw the new tracking rect
    if(fDraw)
      {
      DrawTrackingRect(wi, hdc, lprc);
      CopyRect(&m_pItems[wi->m_index].rcTrack, lprc);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::TrackSelect (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::TrackSelect
  (
  HDC     hdc,
  LPPOINT lppt,
  BOOL    fDraw,
  BOOL    fToggle
  )

  {
  RECT  rc = {m_ptStart.x, m_ptStart.y, lppt->x, lppt->y};

  MakeRealRect(&rc, &rc);
  if((fToggle && !fDraw) || (fDraw && !fToggle) || !EqualRect(&rc, &m_rcLast))
    {
    // remove the old tracking rect
    if(fToggle)
      {
      Rectangle(hdc, m_rcLast.left, m_rcLast.top, m_rcLast.right, m_rcLast.bottom);
      if(!fDraw)
        CopyRect(&m_rcLast, &rc);
      }

    // draw the new tracking rect
    if(fDraw)
      {
      Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
      CopyRect(&m_rcLast, &rc);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SelectItemsInRect (private)
//
//  DESCRIPTION:  This will select all items in the specified rectangle.
//
////////////////////////////////////////////////////////////////////////////

void Items::SelectItemsInRect
  (
  LPRECT  lprc
  )

  {
  OpWndItemD* wi = NULL;

  while((wi = GetNextItem(wi, TRUE)) != NULL)
    {
    if(!OWndLayIsItemLocked(wi) && ItemInRect(wi, lprc, m_pWnd->ReqFullEnclosure()))
      SelectItem(wi->m_index, TRUE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::MoveItems (private)
//
//  DESCRIPTION:  This will move all selected items to their last tracking
//                rect size.
//
////////////////////////////////////////////////////////////////////////////

void Items::MoveItems
  (
  )

  {
  OpWndItemD* wi = NULL;

  while((wi = GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      if(!EqualRect(&m_pItems[wi->m_index].rcTrack, &wi->m_rcItem))
        {
        Move(wi, &m_pItems[wi->m_index].rcTrack);
        m_pWnd->SetDirtyFlag();
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SetTrackingRectsEmpty (private)
//
//  DESCRIPTION:  This will set all tracking rects to {0,0,0,0}.
//
////////////////////////////////////////////////////////////////////////////

void Items::SetTrackingRectsEmpty
  (
  )

  {
  for(int i=0; i<m_nItems; i++)
    SetRectEmpty(&m_pItems[i].rcTrack);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GetStdCursor (private)
//
//  DESCRIPTION:  This will get the standard cursor for the item at the
//                specified point.
//
////////////////////////////////////////////////////////////////////////////

long Items::GetStdCursor
  (
  OpWndItemD* wi,
  LPPOINT     lppt,
  HCURSOR*    phcur
  )

  {
  HCURSOR hcur = NULL;
  long    lRet;

  if(!IsItemSelected(wi->m_index))
    {
    if(PointInItem(wi, lppt))
      {
      lRet = OWND_POINT_IN_ITEM;
      *phcur = LoadCursor(NULL, IDC_ARROW);
      }
    else
      lRet = OWND_POINT_IN_NONE;
    }
  else if((lRet = PointInGrabHandle(wi, lppt, &hcur))!=OWND_POINT_IN_NONE)
    {
    if(!hcur)
      {
      switch(lRet)
        {
        case OWND_POINT_IN_TL:
        case OWND_POINT_IN_BR:
          *phcur = LoadCursor(NULL, IDC_SIZENWSE);
          break;

        case OWND_POINT_IN_TR:
        case OWND_POINT_IN_BL:
          *phcur = LoadCursor(NULL, IDC_SIZENESW);
          break;

        case OWND_POINT_IN_T:
        case OWND_POINT_IN_B:
          *phcur = LoadCursor(NULL, IDC_SIZENS);
          break;

        case OWND_POINT_IN_L:
        case OWND_POINT_IN_R:
          *phcur = LoadCursor(NULL, IDC_SIZEWE);
          break;

        default:
          I_ASSERT(0);
          break;
        }
      }
    else
      *phcur = hcur;
    }
  else if(PointInItem(wi, lppt))
    {
    lRet = OWND_POINT_IN_ITEM;
    *phcur = LoadCursor(NULL, IDC_SIZE);
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SetZOrder (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

int Items::SetZOrder
  (
  OpWndItemD* wi,
  int         nIdx
  )

  {
  AMEMBLOCKID  mi;
  WND_ITEMS*  pwi;
  int         nItems = 0;
  int         i;
  int         nGroupAdjust;

  // make sure we are working on non-owned items
  I_ASSERT(!IsItemGrouped(wi->m_index));

  if(nIdx != wi->m_index)
    {
    // allocate enough memory for all the items to be moved
    nItems = OWndLayIsItemGroup(wi) ? ((OpGroupWndItem*)wi)->NumItems(TRUE) + 1 : 1;
    mi = AMemAllocate(sizeof(WND_ITEMS)*nItems);
    pwi = (WND_ITEMS*)AMemLock(mi);
    I_ASSERT(pwi);
    }

  // save the items being moved -- loop for groups
  for(i=0; i<nItems; i++)
    pwi[nItems - 1 - i] = m_pItems[wi->m_index - i];

  // make sure you aren't moving something to screw up a group...
  I_ASSERT((nIdx <= (wi->m_index - nItems + 1)) || nIdx >= wi->m_index);

  if(nIdx < (wi->m_index - nItems + 1))
    {
    // the first item in the group will be placed at nIdx.  The actual item
    // will be offset by the number of items in the group.

    // move the lower items to replace the item being moved.
    for(i=(wi->m_index-nItems); i>=nIdx; i--)
      {
      // fix the group index
      if(m_pItems[i].nGroup > 0)
        {
        m_pItems[i].nGroup += nItems;
        I_ASSERT(OWndLayIsItemGroup(m_pItems[m_pItems[i].nGroup].wi));
        }

      // fix the item index and control id
      m_pItems[i].wi->m_index += nItems;
      if(OWndIsHwndItem(m_pItems[i].wi))
        SetItemCtlID(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem, m_pItems[i].wi->m_index + OWNDLAY_ITEM_ID_OFFSET);

      // move and redraw the item
      InvalidateItem(m_pItems[i].wi, TRUE);
      m_pItems[i + nItems] = m_pItems[i];
      I_ASSERT((i + nItems) == m_pItems[i+nItems].wi->m_index);
      }

    // put the saved items in the proper location
    nGroupAdjust = wi->m_index - nItems - nIdx + 1;
    for(i=(nItems-1); i>=0; i--)
      {
      // move and redraw the item
      m_pItems[i + nIdx] = pwi[i];
      InvalidateItem(m_pItems[i + nIdx].wi, TRUE);

      // fix the item index
      m_pItems[i + nIdx].wi->m_index = i + nIdx;
      if(OWndIsHwndItem(m_pItems[i + nIdx].wi))
        SetItemCtlID(((OpHwndWndItemD*)m_pItems[i + nIdx].wi)->m_hwndItem, i + nIdx + OWNDLAY_ITEM_ID_OFFSET);

      // fix the group index
      if(m_pItems[i + nIdx].nGroup > 0)
        {
        m_pItems[i + nIdx].nGroup -= nGroupAdjust;
        I_ASSERT(OWndLayIsItemGroup(m_pItems[m_pItems[i + nIdx].nGroup].wi));
        }
      }

    m_pWnd->SetDirtyFlag();
    }
  else if(nIdx > wi->m_index)
    {
    // the actual group item will appear at the nIdx position.  The other
    // items will preceed the group item.

    // move the top items down
    for(i=(wi->m_index + 1); i<=nIdx; i++)
      {
      // fix the group index
      if(m_pItems[i].nGroup > 0)
        {
        I_ASSERT(OWndLayIsItemGroup(m_pItems[m_pItems[i].nGroup].wi));
        m_pItems[i].nGroup -= nItems;
        }

      // fix the item index
      m_pItems[i].wi->m_index -= nItems;
      if(OWndIsHwndItem(m_pItems[i].wi))
        SetItemCtlID(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem, m_pItems[i].wi->m_index + OWNDLAY_ITEM_ID_OFFSET);

      // move and redraw the item
      InvalidateItem(m_pItems[i].wi, TRUE);
      m_pItems[i - nItems] = m_pItems[i];
      I_ASSERT((i - nItems) == m_pItems[i-nItems].wi->m_index);
      }

    // move the saved items up
    nGroupAdjust = nIdx - wi->m_index;
    for(i=(1 - nItems); i<=0; i++)
      {
      // move and redraw the item
      m_pItems[nIdx + i] = pwi[i + nItems - 1];
      InvalidateItem(m_pItems[nIdx + i].wi, TRUE);

      // fix the item index
      m_pItems[nIdx + i].wi->m_index = nIdx + i;
      if(OWndIsHwndItem(m_pItems[nIdx + i].wi))
        SetItemCtlID(((OpHwndWndItemD*)m_pItems[nIdx + i].wi)->m_hwndItem, nIdx + i + OWNDLAY_ITEM_ID_OFFSET);

      // fix the group index
      if(m_pItems[nIdx + i].nGroup > 0)
        m_pItems[nIdx + i].nGroup += nGroupAdjust;
      }

    // verify the group indexes are correct
    #ifdef AW_I_DEBUG
    for(i=(nIdx-nItems+1); i<=nIdx; i++)
      {
      if(m_pItems[i].nGroup > 0)
        I_ASSERT(OWndLayIsItemGroup(m_pItems[m_pItems[i].nGroup].wi));
      }
    #endif

    m_pWnd->SetDirtyFlag();
    }

  if(nItems)
    {
    // free up the memory used for moving items
    AMemUnlock(mi);
    AMemFree(mi);
    }

  return nItems;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::RecreateItem (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* Items::RecreateItem
  (
  OpWndItemD* wi
  )

  {
  OpWndItemD* wiRet;
  OpWndItemD* wiOld = wi;
  int         nIdx = wi->m_index;

  if(OWndIsHwndItem(wi))
    {
    ADBG_ASSERT_RES_E(((OpHwndWndItemD*)wi)->m_hwndItem, ;, g_hInstance, ERROR_INVALIDHWND);
    UnsubclassHwndItem(((OpHwndWndItemD*)wi)->m_hwndItem);
    }
  InvalidateItem(wi, TRUE); // Erase the item
  wi->ParentClosing();      // close it out

  // Recreate the visual instance
  wiRet = wi = OWndLayRecreateExistingItem(wi);

  if(wiRet)
    {
    m_pItems[wi->m_index].wi = wi;

    // update the group, if any
    if(IsItemGrouped(wi->m_index))
      ((OpGroupWndItem*)m_pItems[m_pItems[wi->m_index].nGroup].wi)->ItemRecreated(wi, wiOld);

    AdjustNewRect(wi, &wi->m_rcItem, OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_L);
    wi->ParentOpened(); //Open it up

    (!m_fFrames || OWndIsItemVisible(wi)) ? wi->Show() : wi->Hide();

    if(OWndIsHwndItem(wi))
      {
      ADBG_ASSERT_RES_E(((OpHwndWndItemD*)wi)->m_hwndItem, ;, g_hInstance, ERROR_INVALIDHWND);
      SubclassHwndItem(((OpHwndWndItemD*)wi)->m_hwndItem);
      }//OWndIsHwndItem

    // set the tab stop status
    if(!OWndIsItemActivatable(wiRet))
      OWndLaySetItemTabstop(wiRet, FALSE);
    }
  else
    { // item was changed so it is no longer a window item
    BOOL  fGrouped = IsItemGrouped(nIdx);
    int   nGroup   = m_pItems[nIdx].nGroup - 1;

    for(int i = nIdx + 1; i<m_nItems; i++)
      {
      m_pItems[i-1] = m_pItems[i];
      m_pItems[i-1].wi->m_index = i - 1;

      if(OWndIsHwndItem(m_pItems[i-1].wi))
        SetItemCtlID(((OpHwndWndItemD*)m_pItems[i-1].wi)->m_hwndItem, m_pItems[i-1].wi->m_index + OWNDLAY_ITEM_ID_OFFSET);

      // fix the group index
      if(m_pItems[i-1].nGroup>0)
        m_pItems[i-1].nGroup--;
      }

    // resize the list of items
    m_nItems--;
    AMemUnlock(m_miItems);
    AMemSetSize(m_miItems, m_nItems*sizeof(WND_ITEMS));
    m_pItems = (WND_ITEMS*)AMemLock(m_miItems);

    if(fGrouped)
      {
      I_ASSERT(nGroup>0);
      OpWndItemD* wiTmp   = NULL;
      int         nItems;

      if(IsItemSelected(nGroup))
        InvalidateGrabHandles(m_pItems[nGroup].wi, TRUE);
      nItems = ((OpGroupWndItem*)m_pItems[nGroup].wi)->ItemDeleted(wiOld);

      while((wiTmp=((OpGroupWndItem*)m_pItems[nGroup].wi)->GetNextItem(wiTmp, TRUE))!=NULL)
        m_pItems[wiTmp->m_index].nGroup = nGroup;

      // do the necessary ungrouping -- handle nested problems...
      if(nItems<2)
        DeleteGroup((OpGroupWndItem*)m_pItems[nGroup].wi);
      else if(IsItemSelected(nGroup))
        InvalidateGrabHandles(m_pItems[nGroup].wi, FALSE);
      }

    m_pWnd->SetDirtyFlag();
    }

  return wiRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::NudgeResizeItem (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::NudgeResizeItem
  (
  OpWndItemD* wi,
  int         wVKey
  )

  {
  RECT  rc = wi->m_rcItem;

  OWndLPtoDP(wi->m_oiParent, (LPPOINT)&rc, 2);

  switch(wVKey)
    {
    case VK_UP:
      rc.bottom--;
      break;

    case VK_DOWN:
      rc.bottom++;
      break;

    case VK_LEFT:
      rc.right--;
      break;

    case VK_RIGHT:
      rc.right++;
      break;

    default:
      I_ASSERT(0);
      break;
    }

  OWndDPtoLP(wi->m_oiParent, (LPPOINT)&rc, 2);
  AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
  if(!EqualRect(&rc, &wi->m_rcItem))
    {
    Move(wi, &rc);
    m_pWnd->SetDirtyFlag();
    UpdateWindow(m_hwndParent);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Nudge (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Nudge
  (
  int wVKey
  )

  {
  OpWndItemD**  awi       = new OpWndItemD*[m_nSel];
  int           i         = 0;
  OpWndItemD*   wi        = NULL;
  RECT          rc        = {0x7FFF, 0x7FFF, 0, 0};
  BOOL          fMoved    = FALSE;

  // get a list of the items and the selection rectangle
  while((i < m_nSel) && ((wi = GetNextItem(wi, TRUE)) != NULL))
    {
    if(IsItemSelected(wi->m_index))
      {
      // adjust the selection bounds
      if(wi->m_rcItem.left < rc.left)
        rc.left = wi->m_rcItem.left;
      if(wi->m_rcItem.top < rc.top)
        rc.top = wi->m_rcItem.top;
      if(wi->m_rcItem.right > rc.right)
        rc.right = wi->m_rcItem.right;
      if(wi->m_rcItem.bottom > rc.bottom)
        rc.bottom = wi->m_rcItem.bottom;

      // save the item for quick reference
      awi[i] = wi;
      i++;
      }
    }

  I_ASSERT((i==m_nSel) && (m_nSel>0) && wi);

  // moving item(s)
  i = 0;
  switch(wVKey)
    {
    case VK_UP:
      if(rc.top > 0)
        {
        while(i<m_nSel)
          {
          CopyRect(&rc, &awi[i]->m_rcItem);
          OWndLPtoDP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
          rc.top--;
          rc.bottom--;
          OWndDPtoLP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
          AdjustNewRect(awi[i], &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
          Move(awi[i], &rc);
          i++;
          }
        fMoved = TRUE;
        }
      break;

    case VK_DOWN:
      while(i<m_nSel)
        {
        CopyRect(&rc, &awi[i]->m_rcItem);
        OWndLPtoDP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
        rc.top++;
        rc.bottom++;
        OWndDPtoLP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
        AdjustNewRect(awi[i], &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        Move(awi[i], &rc);
        i++;
        }
      fMoved = TRUE;
      break;

    case VK_LEFT:
      if(rc.left > 0)
        {
        while(i<m_nSel)
          {
          CopyRect(&rc, &awi[i]->m_rcItem);
          OWndLPtoDP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
          rc.left--;
          rc.right--;
          OWndDPtoLP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
          AdjustNewRect(awi[i], &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
          Move(awi[i], &rc);
          i++;
          }
        fMoved = TRUE;
        }
      break;

    case VK_RIGHT:
      while(i<m_nSel)
        {
        CopyRect(&rc, &awi[i]->m_rcItem);
        OWndLPtoDP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
        rc.left++;
        rc.right++;
        OWndDPtoLP(awi[i]->m_oiParent, (LPPOINT)&rc, 2);
        AdjustNewRect(awi[i], &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        Move(awi[i], &rc);
        i++;
        }
      fMoved = TRUE;
      break;

    default:
      I_ASSERT(0);
      break;
    }

  delete [] awi;

  if(fMoved)
    {
    m_pWnd->SetDirtyFlag();
    UpdateWindow(m_hwndParent);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::TabSelect (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::TabSelect
  (
  GetItemFunc   pfnGetItem
  )

  {
  OpWndItemD* wi          = NULL;
  OpWndItemD* wiFirst     = NULL;
  BOOL        fFoundFirst = (m_nSel==0);

  // get the item to be selected
  while((wi = (this->*pfnGetItem)(wi, TRUE)) != NULL)
    {
    if(fFoundFirst)
      {
      if(!IsItemLocked(wi->m_index) && (m_fFrames || OWndIsItemVisible(wi)))
        break;
      }
    else
      {
      if(IsItemSelected(wi->m_index))
        {
        if(m_nSel>1)
          break;
        else
          {
          wiFirst = wi;
          fFoundFirst = TRUE;
          }
        }
      }
    }

  // wrap around
  if(!wi && wiFirst)
    {
    while((wi = (this->*pfnGetItem)(wi, TRUE)) != wiFirst)
      {
      if(!IsItemLocked(wi->m_index) && (m_fFrames || OWndIsItemVisible(wi)))
        break;
      }
    }

  if(wi && (wi!=wiFirst))
    {
    SelectAll(FALSE);
    SelectItem(wi->m_index, TRUE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::CreateAll
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::CreateAll
  (
  )

  {
  AMEMBLOCKID    miTemp;
  OpWndItemD**  pwi;
  RECT          rcItem;
  int           i, j, k;

  // create all the window items
  miTemp = AMemAllocate(0L);
  m_nItems = OWndLayCreateAllExistingItems(m_oiWnd, miTemp);

  if(m_pItems)
    AMemUnlock(m_miItems);
  AMemSetSize(m_miItems, m_nItems*sizeof(WND_ITEMS));

  if(m_nItems>0)
    {
    pwi = (OpWndItemD**)AMemLock(miTemp);
    m_pItems = (WND_ITEMS*)AMemLock(m_miItems);

    I_ASSERT(pwi);
    I_ASSERT(m_pItems);

    // First pass to set up the whole array -- because of groups.
    for(i=0; i<m_nItems; i++)
      {
      m_pItems[i].flags = 0L;
      SetRectEmpty(&m_pItems[i].rcTrack);
      m_pItems[i].oiObj = pwi[i]->Object();
      m_pItems[i].nGroup = -1;
      m_pItems[i].wi = pwi[i];
      m_pItems[i].wi->m_index = i;
      }

    // Second pass to make sure the groups are constructed correctly and
    // the item has had the correct methods called.
    for(i=0; i<m_nItems; i++)
      {
      // set up the items that are grouped
      if(OWndLayIsItemGroup(m_pItems[i].wi))
        {
        // tell the group that it has been created
        ((OpGroupWndItem*)m_pItems[i].wi)->Created();

        if(((OpGroupWndItem*)m_pItems[i].wi)->NumItems(FALSE)<=1)
          {
          // need to kill the group if it doesn't have at least two items.
          // notify the group that it will be destroyed and needs to restore its items
          ((OpGroupWndItem*)m_pItems[i].wi)->Ungrouped();

          // move the item list down -- copy over the current group item.
          for(j=i+1; j<m_nItems; j++)
            {
            m_pItems[j-1] = m_pItems[j];
            m_pItems[j-1].wi->m_index = j-1;
            }

          // resize the item list
          m_nItems--;
          AMemUnlock(m_miItems);
          AMemSetSize(m_miItems, m_nItems*sizeof(WND_ITEMS));
          m_pItems = (m_nItems>0) ? (WND_ITEMS*)AMemLock(m_miItems) : NULL;

          // decrement the current loop index -- try this item again.
          i--;

          // re-evaluate the for ... methods don't need to be called on item.
          continue;
          }
        else
          {
          k = 0;  // offset for nested groups
          for(j=1; j<=((OpGroupWndItem*)m_pItems[i].wi)->NumItems(FALSE); j++)
            {
            I_ASSERT((i-j-k) >= 0);

            SetItemGrouped(i-j-k, TRUE);
            m_pItems[i-j-k].nGroup = i;
            if(OWndLayIsItemGroup(m_pItems[i-j-k].wi))
              k += ((OpGroupWndItem*)m_pItems[i-j-k].wi)->NumItems(TRUE);
            }
          }
        }

      // make sure that the item has the AdjustNewRect method called
      CopyRect(&rcItem, &m_pItems[i].wi->m_rcItem);
      AdjustNewRect(m_pItems[i].wi, &rcItem, OWND_ADJRECT_ANCHOR_T | OWND_ADJRECT_ANCHOR_L);
      CopyRect(&m_pItems[i].wi->m_rcItem, &rcItem);

      // initialize the item's parent
      m_pItems[i].wi->m_hwndParent = m_hwndParent;
      m_pItems[i].wi->ParentOpened();

      // show (or hide) the item and cause a redraw
      (!m_fFrames && OWndIsItemVisible(m_pItems[i].wi))
        ? m_pItems[i].wi->Show() : m_pItems[i].wi->Hide();

      if(OWndIsHwndItem(m_pItems[i].wi))
        {
        ADBG_ASSERT_RES_E(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem, continue, g_hInstance, ERROR_INVALIDHWND);

        SubclassHwndItem(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem);

        // verify that the correct control id was used -- should use ADBG_ASSERT_XXX
        I_ASSERT(GetItemCtlID(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem)
                    ==
                 (m_pItems[i].wi->m_index + OWNDLAY_ITEM_ID_OFFSET));
        }

      // validate the shape library, if necessary
      if(OWndLayIsItemShape(m_pItems[i].wi))
        m_pWnd->GetLayout()->Shapes()->ValidateShapeLib((ATYPEID)m_pItems[i].wi->Object());
      }

    AMemUnlock(miTemp);
    }
  else
    m_pItems = NULL;

  AMemFree(miTemp);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DestroyAll
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DestroyAll
  (
  )

  {
  if(m_nItems>0)
    {
    int           i;
    AMEMBLOCKID    miItems;
    OpWndItemD**  pItems;

    // destroy the current list of items
    miItems = AMemAllocate(m_nItems*sizeof(OpWndItemD*));
    pItems = (OpWndItemD**)AMemLock(miItems);

    // fill a list to have the window object delete them all
    for(i=0; i<m_nItems; i++)
      {
      pItems[i] = m_pItems[i].wi;
      if(OWndIsHwndItem(pItems[i]))
        {
        if(((OpHwndWndItemD*)pItems[i])->m_hwndItem)
          UnsubclassHwndItem(((OpHwndWndItemD*)pItems[i])->m_hwndItem);
        #ifdef ADBG_DEBUG
        else
          ADbgPrintRes(ADBG_ERROR, pItems[i]->Type(), g_hInstance, ERROR_INVALIDHWND);
        #endif
        }
      pItems[i]->ParentClosing();
      }

    AMemUnlock(miItems);
    OWndLayDestroyAllExistingItems(miItems, m_nItems);
    AMemFree(miItems);

    m_nItems = 0;
    m_nSel = 0;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::AddNewItems
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::AddNewItems
  (
  OpWndItemD**  awi,
  AOBJECTID*     aoiObj,
  LPRECT        arc,
  BOOL*         abCreated,
  LONG*         fAdjust,
  int           nItems
  )

  {
  I_ASSERT(awi);

  int i, start, stop;

  // unselect all items so the new item(s) will be the only one(s) selected
  SelectAll(FALSE);
  I_ASSERT(m_nSel==0);

  // resize the list of items
  AMemUnlock(m_miItems);
  AMemSetSize(m_miItems, (m_nItems+nItems)*sizeof(WND_ITEMS));
  m_pItems = (WND_ITEMS*)AMemLock(m_miItems);
  I_ASSERT(m_pItems);

  // update layout members
  start = m_nItems;
  stop = m_nItems + nItems;
  for(i = start; i<stop; i++)
    {
    m_nSel++;

    SetRectEmpty(&m_pItems[i].rcTrack);
    m_pItems[i].flags = abCreated ? (abCreated[i-start] ? FLAG_CREATEDOBJECT : FLAG_ADDEDOBJECT) : FLAG_ADDEDOBJECT;
    m_pItems[i].oiObj = aoiObj ? aoiObj[i-start] : awi[i-start]->Object();
    m_pItems[i].wi = awi[i-start];
    m_pItems[i].nGroup = -1;

    // set the item flags
    OWndSetItemVisible(m_pItems[i].wi, TRUE);
    OWndLaySetItemTabstop(m_pItems[i].wi, OWndIsItemActivatable(m_pItems[i].wi));
    OWndLaySetItemGroupstop(m_pItems[i].wi, !OWndIsItemActivatable(m_pItems[i].wi));

    // increment the item count for the ParentNotified calls (when ParentOpened is called)
    m_nItems++;

    // show the new item
    m_pItems[i].wi->m_index = i;
    m_pItems[i].wi->m_hwndParent = m_hwndParent;
    m_pItems[i].wi->ParentOpened();
    if(arc)
      {
      AdjustNewRect(m_pItems[i].wi, &arc[i-start], fAdjust ? fAdjust[i-start] : OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
      Move(m_pItems[i].wi, &arc[i-start]);
      }
    m_fFrames ? m_pItems[i].wi->Hide() : m_pItems[i].wi->Show();

    if(OWndIsHwndItem(m_pItems[i].wi))
      {
      ADBG_ASSERT_RES_E(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem, continue, g_hInstance, ERROR_INVALIDHWND);
      SubclassHwndItem(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem);

      // verify that the correct control id was used
      I_ASSERT(GetItemCtlID(((OpHwndWndItemD*)m_pItems[i].wi)->m_hwndItem)
                  ==
               (m_pItems[i].wi->m_index + OWNDLAY_ITEM_ID_OFFSET));
      }

    if(OWndLayIsItemGroup(m_pItems[i].wi))
      {
      if(m_fFrames)
        InvalidateItem(m_pItems[i].wi, FALSE);
      InvalidateGrabHandles(m_pItems[i].wi, FALSE);
      }
    }

  // set the dirty flag
  if(nItems>0)
    m_pWnd->SetDirtyFlag();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SelectAll
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::SelectAll
  (
  BOOL  fSelect
  )

  {
  OpWndItemD* wi = NULL;

  while((wi = GetNextItem(wi, TRUE)) != NULL)
    SelectItem(wi->m_index, fSelect);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SelectionCount
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::SelectionCount
  (
  int*  nSel,
  int*  nGroups,
  int*  nObjects
  )

  {
  OpWndItemD* wi = NULL;

  I_ASSERT(nSel || nGroups || nObjects);

  if(nSel)     *nSel     = 0; // total number of selections
  if(nGroups)  *nGroups  = 0; // number of selections that are groups
  if(nObjects) *nObjects = 0; // number of selections that are all objects

  while((wi = GetNextItem(wi, TRUE)) != NULL)
    {
    // count the selection ....
    if(IsItemSelected(wi->m_index))
      {
      if(nSel)
        *nSel = *nSel + 1;

      if(OWndLayIsItemGroup(wi))
        {
        if(nGroups)
          *nGroups = *nGroups + 1;

        if(nObjects && ((OpGroupWndItem*)wi)->AllObjects())
          *nObjects  = *nObjects + 1;
        }
      else if(nObjects && !OWndLayIsItemShape(wi))
        *nObjects  = *nObjects + 1;
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ItemFromUID
//
//  DESCRIPTION:  Returns a pointer to the specified item
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* Items::ItemFromUID
  (
  long  uid
  )

  {
  int         i;
  OpWndItemD* wiRet = NULL;

  for(i=0; i<m_nItems; i++)
    {
    if(m_pItems[i].wi->m_uidItem == uid)
      {
      wiRet = m_pItems[i].wi;
      break;
      }
    }

  return wiRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ItemFromPoint
//
//  DESCRIPTION:  This returns a pointer to the item under the specified
//                point.  This can skip groups.
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* Items::ItemFromPoint
  (
  LPPOINT lppt,
  BOOL    fSkipGroups
  )

  {
  OpWndItemD* wi = NULL;

  while((wi = GetPrevItem(wi, fSkipGroups)) != NULL)
    {
    if(fSkipGroups || !OWndLayIsItemGroup(wi))
      {
      if(PointInItem(wi, lppt))
        break;
      }
    }

  return wi;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::NumItems
//
//  DESCRIPTION:  This returns the number of items (and perhaps sub-items).
//
////////////////////////////////////////////////////////////////////////////

int Items::NumItems
  (
  BOOL  fCountNested
  )

  {
  int         nRet = 0;
  OpWndItemD* wi = NULL;

  if(fCountNested)
    nRet = m_nItems;
  else
    {
    while((wi = GetPrevItem(wi, TRUE)) != NULL)
      nRet++;
    }

  return nRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::EditItem
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

BOOL Items::EditItem
  (
  LPPOINT lppt,
  BOOL    fEditExtra
  )

  {
  BOOL        fRet = FALSE;
  OpWndItemD* wi = NULL;

  if(lppt)
    wi = ItemFromPoint(lppt, FALSE);
  else
    {
    #ifdef AW_I_DEBUG
    int nSel, nGroups;
    SelectionCount(&nSel, &nGroups, NULL);
    I_ASSERT(nSel==1 && nGroups==0);
    #endif

    while((wi=GetPrevItem(wi, TRUE))!=NULL)
      {
      if(IsItemSelected(wi->m_index))
        break;
      }
    }

  if(wi)
    {
    I_ASSERT(!OWndLayIsItemGroup(wi));

    m_pWnd->GetLayout()->EnablePalettes(FALSE);
    if(wi->Edit(fEditExtra)==A_MODIFIED)
      {
      BOOL  fIsShape = OWndLayIsItemShape(wi);

      if(!fIsShape)
        {
        OpWndItemD* wiTmp = NULL;

        while((wiTmp=GetNextItem(wiTmp, FALSE))!=NULL)
          {
          if((wi!=wiTmp) && (wiTmp->Object()==wi->Object()))
            wiTmp = RecreateItem(wiTmp);
          }
        }

      wi = RecreateItem(wi);
      if(!wi)
        ReloadObjectListWindow();
      fRet = TRUE;
      m_pWnd->SetDirtyFlag();
      }
    m_pWnd->GetLayout()->EnablePalettes(TRUE);
    }
  else if(lppt)
    m_pWnd->EditAttributes();

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DeleteSelObjects
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DeleteSelObjects
  (
  )

  {
  OpWndItemD* wi = NULL;
  OpWndItemD* wiTmp;
  AOBJECTID    oi;

  while((wi = GetNextItem(wi, FALSE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      // destroy all window items associated with the object
      if(OWndIsItemObject(wi))
        {
        oi = wi->Object();

        wiTmp = NULL;
        while((wiTmp = GetNextItem(wiTmp, FALSE)) != NULL)
          {
          if((wi!=wiTmp) && (oi==wiTmp->Object()))
            {
            InvalidateItem(wiTmp, TRUE);
            wiTmp = GetPrevItem(wiTmp, FALSE);
            DeleteItem(GetNextItem(wiTmp, FALSE));
            }
          }

        // delete the item
        wi = GetPrevItem(wi, FALSE);
        DeleteItem(GetNextItem(wi, FALSE));
        AObjDestroy(oi);
        ReloadObjectListWindow();
        }
      }
    }

  // refresh the object list

  #ifdef AW_I_DEBUG
  ResetCtlIDs();
  #endif
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DeleteSelItems
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DeleteSelItems
  (
  )

  {
  OpWndItemD* wi = NULL;

  while((wi = GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      // delete the item
      wi = GetPrevItem(wi, TRUE);
      DeleteItem(GetNextItem(wi, TRUE));
      }
    }

  #ifdef AW_I_DEBUG
  ResetCtlIDs();
  #endif
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Update
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Update
  (
  )

  {
  AMEMBLOCKID    miItems = AMemAllocate(m_nItems*sizeof(OpWndItemD*));
  int           i;
  OpWndItemD**  pItems = (OpWndItemD**)AMemLock(miItems);

  // update the flags and oiObj fields for all objects -- created objects will stick
  for(i=0; i<m_nItems; i++)
    {
    m_pItems[i].flags &= ~FLAG_OBJECTCREATED;
    pItems[i] = m_pItems[i].wi;
    }

  AMemUnlock(miItems);
  OWndLaySaveItems(m_oiWnd, miItems, m_nItems);
  AMemFree(miItems);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Revert
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Revert
  (
  )

  {
  DestroyAll();
  InvalidateRect(m_hwndParent, NULL, TRUE);
  CreateAll();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Paint
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Paint
  (
  )

  {
  I_ASSERT(m_hwndParent == m_pWnd->ClientWnd());

  if(IsIconic(m_pWnd->WndHandle()))
    DefWindowProc(m_pWnd->WndHandle(), WM_PAINT, 0, 0L);
  else
    {
    POINT pt;
    int   fwKeys    = GetKeyFlags();

    GetCursorPos(&pt);
    MapWindowPoints(HWND_DESKTOP, m_hwndParent, &pt, 1);
    OWndDPtoLP(m_oiWnd, &pt, 1);
    m_pWnd->SnapPointToGrid(&pt);

    if(m_pWnd->GetLayout()->Shapes()->IsTracking())
      m_pWnd->GetLayout()->Shapes()->TrackShape(m_pWnd, m_pWnd->ClientWnd(), &pt, FALSE, TRUE);
    else if(m_fTrack && !(m_fTrack & TRACK_WAITFORDELTA))
      Track(&pt, fwKeys, FALSE, TRUE);

    m_fFrames ? PaintFrames() : PaintItems();

    if(m_pWnd->GetLayout()->Shapes()->IsTracking())
      m_pWnd->GetLayout()->Shapes()->TrackShape(m_pWnd, m_pWnd->ClientWnd(), &pt, FALSE, TRUE);
    else if(m_fTrack && !(m_fTrack & TRACK_WAITFORDELTA))
      Track(&pt, fwKeys, FALSE, TRUE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::PaintFrames (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::PaintFrames
  (
  )

  {
  OpWndItemD* wi;
  HRGN        hrgnUpdate;
  PAINTSTRUCT ps;
  BOOL        MicrosoftSucksErrorFlag = FALSE;
  RECT        rcTemp;
  LOGFONT     lf;
  POINT       pt                      = {5, 11};
  HFONT       hfont;
  HFONT       hfontOld;
  BOOL        fShowGroups             = m_pWnd->DrawGroupsMode();
  int         i                       = 0;

  OWndDStoLS(m_oiWnd, &pt, 1);

  hrgnUpdate = CreateRectRgn(0,0,0,0);

  if(GetUpdateRgn(m_hwndParent, hrgnUpdate, TRUE) == SIMPLEREGION)
    MicrosoftSucksErrorFlag = TRUE;

  BeginPaint(m_hwndParent, &ps);
  if (MicrosoftSucksErrorFlag)
    {
    CopyRect(&rcTemp, &ps.rcPaint);
    OWndLPtoDP(m_oiWnd, (LPPOINT)&rcTemp, 2);
    SetRectRgn(hrgnUpdate, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
    }

  // set up the font for the item information
  lf.lfHeight = pt.y;
  lf.lfWidth = 0;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;
  lf.lfWeight = FW_NORMAL;
  lf.lfItalic = 0;
  lf.lfUnderline = 0;
  lf.lfStrikeOut = 0;
  lf.lfCharSet = ANSI_CHARSET;
  lf.lfOutPrecision = OUT_STRING_PRECIS;
  lf.lfClipPrecision = CLIP_STROKE_PRECIS;
  lf.lfQuality = DEFAULT_QUALITY;
  lf.lfPitchAndFamily = FF_SWISS;
  lstrcpy(lf.lfFaceName, "Small Fonts");
  hfont = CreateFontIndirect(&lf);
  hfontOld = (HFONT)SelectObject(ps.hdc, hfont);

  SetPoint(&pt, 1, 1);
  OWndDStoLS(m_oiWnd, &pt, 1);
  wi = NULL;
  while((wi = GetNextItem(wi, fShowGroups)) != NULL)
    {
    MakeRealRect(&rcTemp, &wi->m_rcItem);
    OWndLPtoDP(m_oiWnd, (LPPOINT)&rcTemp, 2);
    i++;

    if(rcTemp.right == rcTemp.left)
      rcTemp.right++;
    if(rcTemp.bottom == rcTemp.top)
      rcTemp.bottom++;

    if(RectInRegion(hrgnUpdate, &rcTemp))
      {
      PaintItemInfo(wi, ps.hdc, i);

      if(IsItemSelected(wi->m_index))
        DrawGrabHandles(wi, ps.hdc);
      }
    }

  // free everything up and finish painting
  SelectObject(ps.hdc, hfontOld);
  DeleteObject(hfont);
  DeleteObject(hrgnUpdate);
  EndPaint(m_hwndParent, &ps);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::PaintItemInfo (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::PaintItemInfo
  (
  OpWndItemD* wi,
  HDC         hdc,
  int         idx
  )

  {
  I_ASSERT(wi);

  char  szBuffer[256];
  POINT pt = {1, 1};
  RECT  rc;
  char* szVisible = OWndIsItemVisible(wi) ? "Visible" : "Invisible";
  char* szTab = OWndLayIsItemTabstop(wi) ? "Tab Stop" : "No Tab Stop";
  char* szGroup = OWndLayIsItemGroupstop(wi) ? "Group Stop" : "No Group Stop";
  char* szLock = OWndLayIsItemLocked(wi) ? "Locked" : "Unlocked";
  char  szName[MAX_NAMELENGTH];

  if(OWndLayIsItemShape(wi))
    (m_pWnd->GetLayout()->Shapes())->GetShapeName(wi, szName, MAX_NAMELENGTH);
  else if (OWndLayIsItemGroup(wi))
    wsprintf(szName, "Group of %hi items.", ((OpGroupWndItem*)wi)->NumItems(FALSE));
  else
    AObjGetName(wi->Object(), szName, MAX_NAMELENGTH);

  switch((m_pWnd->GetLayout()->Shapes())->CurrentTool())
    {
    case OSHP_STDTOOL_TABSTOP:
      wsprintf(szBuffer, "%s\r\n%i  %s\r\n%s\r\n%s\r\n%s", szTab, idx, szName, szVisible, szLock, szGroup);
      break;

    case OSHP_STDTOOL_GROUPSTOP:
      wsprintf(szBuffer, "%s\r\n%i  %s\r\n%s\r\n%s\r\n%s", szGroup, idx, szName, szVisible, szLock, szTab);
      break;

    case OSHP_STDTOOL_LOCK:
      wsprintf(szBuffer, "%s\r\n%i  %s\r\n%s\r\n%s\r\n%s", szLock, idx, szName, szVisible, szTab, szGroup);
      break;

    case OSHP_STDTOOL_VISIBILITY:
      wsprintf(szBuffer, "%s\r\n%i  %s\r\n%s\r\n%s\r\n%s", szVisible, idx, szName, szLock, szTab, szGroup);
      break;

    case OSHP_STDTOOL_REORDER:
    default:
      wsprintf(szBuffer, "%i  %s\r\n%s\r\n%s\r\n%s\r\n%s", idx, szName, szVisible, szLock, szTab, szGroup);
      break;
    }

  // get the item rect and fix it so the left and top are to the left and top of the right bottom.
  MakeRealRect(&rc, &wi->m_rcItem);

  // force at least a one pixel width and height
  OWndDStoLS(m_oiWnd, &pt, 1);
  if((rc.right - rc.left) < pt.x)
    rc.right = rc.left + pt.x;
  if((rc.bottom - rc.top) < pt.y)
    rc.bottom = rc.top + pt.y;

  Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

  // draw the text with at least a pixel border
  InflateRect(&rc, -2*pt.x, -2*pt.y);
  DrawText(hdc, szBuffer, -1, &rc, DT_LEFT);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::PaintItems (private)
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::PaintItems
  (
  )

  {
  OpWndItemD* wi = NULL;
  HRGN        hrgnUpdate;
  HRGN        hrgnInvalid;
  PAINTSTRUCT ps;
  BOOL        MicrosoftSucksErrorFlag = FALSE;  // Microsoft ALWAYS sucks, but the flag needs to be false sometimes.
  BOOL        fFullWindow             = FALSE;
  RECT        rcInvalid;
  RECT        rcTemp;
  HPALETTE    hpalOld;
  int         nRgnRes;
  int         i;

  hrgnUpdate = CreateRectRgn(0,0,0,0);
  hrgnInvalid = CreateRectRgn(0,0,0,0);

  nRgnRes = GetUpdateRgn(m_hwndParent, hrgnUpdate, TRUE);
  if((nRgnRes == SIMPLEREGION))
    MicrosoftSucksErrorFlag = TRUE;

  BeginPaint(m_hwndParent, &ps);
  CopyRect(&rcInvalid, &ps.rcPaint);

  if (MicrosoftSucksErrorFlag)
    {
    OWndLPtoDP(m_oiWnd, (LPPOINT)&rcInvalid, 2);
    SetRectRgn(hrgnUpdate, rcInvalid.left, rcInvalid.top, rcInvalid.right, rcInvalid.bottom);
    OWndDPtoLP(m_oiWnd, (LPPOINT)&rcInvalid, 2);

    GetClientRect(m_hwndParent, &rcTemp);
    OWndDPtoLP(m_oiWnd, (LPPOINT)&rcTemp, 2);
    if(EqualRect(&rcTemp, &rcInvalid))
      fFullWindow = TRUE;
    }

  if(CombineRgn(hrgnInvalid, hrgnUpdate, NULL, RGN_COPY) == ERROR)
    {
    DeleteObject(hrgnInvalid);
    hrgnInvalid = NULL;
    }

  // Calculate the invalid items and set them to need to be drawn
  while((wi = GetPrevItem(wi, FALSE)) != NULL)
    {
    if(OWndIsItemVisible(wi) && wi->ItemInRegion(hrgnUpdate, TRUE))
      {
      SetItemNeedsToBeDrawn(wi->m_index, TRUE);

      if(!OWndIsItemTransparent(wi))
        wi->RemoveItemFromRegion(hrgnUpdate, TRUE);

      if(fFullWindow && OWndIsHwndItem(wi))
        InvalidateRect(((OpHwndWndItemD*)wi)->m_hwndItem, NULL, TRUE);
      }
    }
  EndPaint(m_hwndParent, &ps);

  // Now, draw the items that need to be drawn
  HDC hdc = GetDC(m_hwndParent);
  if(hrgnInvalid)
    SelectClipRgn(hdc, hrgnInvalid);

  if(WndData::GetPaletteHandle())
    {
    hpalOld = SelectPalette(hdc, WndData::GetPaletteHandle(), TRUE);
    RealizePalette(hdc);
    }

  for(i=0; i<m_nItems; i++)
    {
    wi = m_pItems[i].wi;

    if(OWndIsItemVisible(wi))
      {
      if(OWndIsLiteItem(wi))
        {
        if(DoesItemNeedToBeDrawn(wi->m_index))
          {
          IntersectRect(&rcTemp, &wi->m_rcItem, &rcInvalid);
          ((OpLiteWndItemD*)wi)->Draw(hdc, &rcTemp);
          SetItemNeedsToBeDrawn(wi->m_index, FALSE);
          }
        }
      else
        UpdateWindow(((OpHwndWndItemD*)wi)->m_hwndItem);

      if(IsItemSelected(wi->m_index))
        DrawGrabHandles(wi, hdc);
      }
    }

  if (WndData::GetPaletteHandle())
    {
    SelectPalette(hdc, hpalOld, TRUE);
    RealizePalette(hdc);
    }

  SelectClipRgn(hdc, NULL);
  ReleaseDC(m_hwndParent, hdc);
  DeleteObject(hrgnUpdate);
  if(hrgnInvalid)
    DeleteObject(hrgnInvalid);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::HScroll
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::HScroll
  (
  HWND  hwnd,
  HWND  hwndCtl,
  UINT  uCode,
  int   nPos
  )

  {
  I_UNUSED(hwnd);
  I_UNUSED(hwndCtl);

  int   x               = GetScrollPos(m_hwndParent, SB_HORZ);
  int   n_min, n_max;
  int   nLine, nPage;
  RECT  rcClient;

  GetClientRect(m_hwndParent, &rcClient);
  nPage = nMax(1, rcClient.right / 2);
  nLine = nMax(1, rcClient.right / 5);
  GetScrollRange(m_hwndParent, SB_HORZ, &n_min, &n_max);

  switch(uCode)
    {
    case SB_TOP:
      x = n_min;
      break;

    case SB_BOTTOM:
      x = n_max;
      break;

    case SB_LINEDOWN:
      x = nMin(x + nLine, n_max);
      break;

    case SB_LINEUP:
      x = nMax(x - nLine, n_min);
      break;

    case SB_PAGEDOWN:
      x = nMin(x + nPage, n_max);
      break;

    case SB_PAGEUP:
      x = nMax(x - nPage, n_min);
      break;

    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
      x = nPos;
      break;

    default:
      break;
    }

  if(x != GetScrollPos(m_hwndParent, SB_HORZ))
    {
    OpWndItemD* wi              = NULL;
    POINT       ptViewportOrg;
    POINT       ptScroll;

    OWndLayGetViewportOrgEx(m_oiWnd, &ptViewportOrg);
    OWndLaySetViewportOrgEx(m_oiWnd, -x, ptViewportOrg.y, NULL);

    SetScrollPos(m_hwndParent, SB_HORZ, x, TRUE);
    SetPoint(&ptScroll, x + ptViewportOrg.x, 0);
    OWndDStoLS(m_oiWnd, &ptScroll, 1);
    ScrollWindow(m_hwndParent, -ptScroll.x, -ptScroll.y, NULL, NULL);
    UpdateWindow(m_hwndParent);

    while((wi = GetNextItem(wi, FALSE)) != NULL)
      wi->ScreenLocationChanged(OWND_LOC_HSCROLL);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::VScroll
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::VScroll
  (
  HWND  hwnd,
  HWND  hwndCtl,
  UINT  uCode,
  int   nPos
  )

  {
  I_UNUSED(hwnd);
  I_UNUSED(hwndCtl);

  int   y               = GetScrollPos(m_hwndParent, SB_VERT);
  int   n_min, n_max;
  int   nLine, nPage;
  RECT  rcClient;

  GetClientRect(m_hwndParent, &rcClient);
  nPage = nMax(1, rcClient.bottom / 2);
  nLine = nMax(1, rcClient.bottom / 5);
  GetScrollRange(m_hwndParent, SB_VERT, &n_min, &n_max);

  switch(uCode)
    {
    case SB_TOP:
      y = n_min;
      break;

    case SB_BOTTOM:
      y = n_max;
      break;

    case SB_LINEDOWN:
      y = nMin(y + nLine, n_max);
      break;

    case SB_LINEUP:
      y = nMax(y - nLine, n_min);
      break;

    case SB_PAGEDOWN:
      y = nMin(y + nPage, n_max);
      break;

    case SB_PAGEUP:
      y = nMax(y - nPage, n_min);
      break;

    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
      y = nPos;
      break;

    default:
      break;
    }

  if(y != GetScrollPos(m_hwndParent, SB_VERT))
    {
    OpWndItemD* wi              = NULL;
    POINT       ptViewportOrg;
    POINT       ptScroll;

    OWndLayGetViewportOrgEx(m_oiWnd, &ptViewportOrg);
    OWndLaySetViewportOrgEx(m_oiWnd, ptViewportOrg.x, -y, NULL);

    SetScrollPos(m_hwndParent, SB_VERT, y, TRUE);
    SetPoint(&ptScroll, 0, y + ptViewportOrg.y);
    OWndDStoLS(m_oiWnd, &ptScroll, 1);
    ScrollWindow(m_hwndParent, -ptScroll.x, -ptScroll.y, NULL, NULL);
    UpdateWindow(m_hwndParent);

    while((wi = GetNextItem(wi, FALSE)) != NULL)
      wi->ScreenLocationChanged(OWND_LOC_VSCROLL);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ParentNotified
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

long Items::ParentNotified
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  OpWndItemD* wi;
  BOOL        fHandled  = FALSE;
  int         idCtl     = -1;
  int         idx;
  long        lRet;
  #ifdef ADBG_DEBUG
  char        szBuffer[25];
  #endif

  switch(uMsg)
    {
    case WM_DELETEITEM:
      ADBG_EVAL(lstrcpy(szBuffer, "WM_DELETEITEM");)
      idCtl = (int)wParam;
      idx = idCtl - OWNDLAY_ITEM_ID_OFFSET;
      break;

    case WM_COMPAREITEM:
      ADBG_EVAL(lstrcpy(szBuffer, "WM_COMPAREITEM");)
      idCtl = (int)wParam;
      idx = idCtl - OWNDLAY_ITEM_ID_OFFSET;
      break;

    case WM_MEASUREITEM:
      ADBG_EVAL(lstrcpy(szBuffer, "WM_MEASUREITEM");)
      idCtl = (int)wParam;
      idx = idCtl - OWNDLAY_ITEM_ID_OFFSET;
      break;

    case WM_DRAWITEM:
      ADBG_EVAL(lstrcpy(szBuffer, "WM_DRAWITEM");)
      idCtl = (int)wParam;
      idx = idCtl - OWNDLAY_ITEM_ID_OFFSET;
      break;

    #if defined(__FLAT__)
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
      ADBG_EVAL(lstrcpy(szBuffer, "WM_CTLCOLORxxx");)
      idCtl = GetItemCtlID((HWND)lParam);
    #else
    case WM_CTLCOLOR:
      ADBG_EVAL(lstrcpy(szBuffer, "WM_CTLCOLOR");)
      idCtl = GetItemCtlID((HWND)LOWORD(lParam));
    #endif
      idx = idCtl - OWNDLAY_ITEM_ID_OFFSET;
      break;
 

    default:
      ADBG_EVAL(wsprintf(szBuffer, "WM_??? (%#04hx)", uMsg);)
      idx = -1;
      break;
    }

  // call the ParentNotification method
  if(idx>=0 && idx<m_nItems)
    {
    wi = m_pItems[idx].wi;

    if(OWndIsHwndItem(wi))
      {
      #ifdef ADBG_DEBUG
      if(idCtl != GetItemCtlID(((OpHwndWndItemD*)wi)->m_hwndItem))
        {
        long  type = (long)wi->Type();
        char  c1 = (char)HIBYTE(HIWORD(type));
        char  c2 = (char)LOBYTE(HIWORD(type));
        char  c3 = (char)HIBYTE(LOWORD(type));
        char  c4 = (char)LOBYTE(LOWORD(type));

        ADbgPrint(ADBG_ERROR, 0,
          "Type \"%c%c%c%c\":  Invalid control ID in message %s.",
          c1, c2, c3, c4, szBuffer);
        }
      #endif

      fHandled = TRUE;
      lRet = ((OpHwndWndItemD*)wi)->ParentNotification(hwnd, uMsg, wParam, lParam);
      }
    }

  if(!fHandled)
    {
    lRet = DefWindowProc(hwnd, uMsg, wParam, lParam);

    #ifdef ADBG_DEBUG
    long  type = (idx>=0 && idx<m_nItems) ? (long)wi->Type() : AMAKEID('?', '?', '?', '?');
    char  c1 = (char)HIBYTE(HIWORD(type));
    char  c2 = (char)LOBYTE(HIWORD(type));
    char  c3 = (char)HIBYTE(LOWORD(type));
    char  c4 = (char)LOBYTE(LOWORD(type));


    ADbgPrint(ADBG_ERROR, 0,
      "Type \"%c%c%c%c\":  Invalid control ID in message %s.",
      c1, c2, c3, c4, szBuffer);
    #endif
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ChangeFullInfo
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::ChangeFullInfo
  (
  BOOL  fNew
  )

  {
  OpWndItemD* wi = NULL;
  BOOL        fVisible;

  if(fNew && !m_fFrames)
    {
    m_fFrames = TRUE;

    while((wi=GetNextItem(wi, FALSE)) != NULL)
      {
      if(OWndIsItemVisible(wi))
        wi->Hide();
      }
    }
  else if(!fNew && m_fFrames)
    {
    m_fFrames = FALSE;

    while((wi=GetNextItem(wi, FALSE)) != NULL)
      {
      fVisible = OWndIsItemVisible(wi);

      if(fVisible)
        wi->Show();

      if(IsItemSelected(wi->m_index) && (!fVisible || IsItemLocked(wi->m_index)))
        SelectItem(wi->m_index, FALSE);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::RenameSelectedItem
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::RenameSelectedItem
  (
  )

  {
  OpWndItemD* wi = NULL;

  while((wi=GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      I_ASSERT(!OWndLayIsItemGroup(wi));
      I_ASSERT(!OWndLayIsItemShape(wi));

      RENAME_DLG  rnd = {wi->Object()};

      m_pWnd->GetLayout()->EnablePalettes(FALSE);
      if(DialogBoxParam(g_hInstance, ID(DLG_RENAMEOBJ), m_pWnd->GetLayout()->WndHandle(), OWLayRenameDlgProc, (long)&rnd))
        {
        // renaming an object is not undoable, so don't set the dirty flag
        if(m_fFrames)
          InvalidateItem(wi, TRUE);
        }
      m_pWnd->GetLayout()->EnablePalettes(TRUE);

      break;
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::AlignLeft
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::AlignLeft
  (
  )

  {
  COORDINATE  left;
  int         nSel = GetSelectionBounds(&left, NULL, NULL, NULL);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // move to the left
        rc.top    = wi->m_rcItem.top;
        rc.bottom = wi->m_rcItem.bottom;
        if(wi->m_rcItem.left < wi->m_rcItem.right)
          {
          rc.left  = left;
          rc.right = left + (wi->m_rcItem.right - wi->m_rcItem.left);
          }
        else
          {
          rc.left  = left + (wi->m_rcItem.left - wi->m_rcItem.right);
          rc.right = left;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::AlignRight
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::AlignRight
  (
  )

  {
  COORDINATE  right;
  int         nSel = GetSelectionBounds(NULL, NULL, &right, NULL);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // move to the right
        rc.top    = wi->m_rcItem.top;
        rc.bottom = wi->m_rcItem.bottom;
        if(wi->m_rcItem.left < wi->m_rcItem.right)
          {
          rc.left  = right - (wi->m_rcItem.right - wi->m_rcItem.left);;
          rc.right = right;
          }
        else
          {
          rc.left  = right;
          rc.right = right - (wi->m_rcItem.left - wi->m_rcItem.right);
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_R | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::AlignTop
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::AlignTop
  (
  )

  {
  COORDINATE  top;
  int         nSel = GetSelectionBounds(NULL, &top, NULL, NULL);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // move to the top
        rc.left  = wi->m_rcItem.left;
        rc.right = wi->m_rcItem.right;
        if(wi->m_rcItem.top < wi->m_rcItem.bottom)
          {
          rc.top    = top;
          rc.bottom = top + (wi->m_rcItem.bottom - wi->m_rcItem.top);
          }
        else
          {
          rc.top    = top + (wi->m_rcItem.top - wi->m_rcItem.bottom);
          rc.bottom = top;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::AlignBottom
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::AlignBottom
  (
  )

  {
  COORDINATE  bottom;
  int         nSel = GetSelectionBounds(NULL, NULL, NULL, &bottom);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // move to the bottom
        rc.left  = wi->m_rcItem.left;
        rc.right = wi->m_rcItem.right;
        if(wi->m_rcItem.top < wi->m_rcItem.bottom)
          {
          rc.top    = bottom - (wi->m_rcItem.bottom - wi->m_rcItem.top);
          rc.bottom = bottom;
          }
        else
          {
          rc.top    = bottom;
          rc.bottom = bottom - (wi->m_rcItem.top - wi->m_rcItem.bottom);
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_B);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::CenterHorz
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::CenterHorz
  (
  )

  {
  COORDINATE  left, right;
  int         nSel = GetSelectionBounds(&left, NULL, &right, NULL);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;
    int         width;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        width = abs(wi->m_rcItem.right - wi->m_rcItem.left);

        // center horizontally
        rc.top    = wi->m_rcItem.top;
        rc.bottom = wi->m_rcItem.bottom;
        if(wi->m_rcItem.left <= wi->m_rcItem.right)
          {
          rc.left  = (right - width + left)/2;
          rc.right = rc.left + width;
          }
        else
          {
          rc.right = (right - width + left)/2;
          rc.left  = rc.right + width;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::CenterVert
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::CenterVert
  (
  )

  {
  COORDINATE  top, bottom;
  int         nSel = GetSelectionBounds(NULL, &top, NULL, &bottom);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;
    int         height;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        height = abs(wi->m_rcItem.bottom - wi->m_rcItem.top);

        // center vertically
        rc.left  = wi->m_rcItem.left;
        rc.right = wi->m_rcItem.right;
        if(wi->m_rcItem.top <= wi->m_rcItem.bottom)
          {
          rc.top    = (bottom - height + top)/2;
          rc.bottom = rc.top + height;
          }
        else
          {
          rc.bottom = (bottom - height + top)/2;
          rc.top    = rc.bottom + height;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::CenterHorzInWnd
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::CenterHorzInWnd
  (
  )

  {
  COORDINATE  left, right;
  int         nSel;
  int         x;
  int         y;
  RECT        rcWnd;
  int         delta;

  nSel = GetSelectionBounds(&left, NULL, &right, NULL);
  x = m_pWnd->HasHorzScroll() ? GetScrollPos(m_hwndParent, SB_HORZ) : 0;
  y = m_pWnd->HasVertScroll() ? GetScrollPos(m_hwndParent, SB_VERT) : 0;

  GetClientRect(m_hwndParent, &rcWnd);
  OWndDStoLS(m_oiWnd, (LPPOINT)&rcWnd, 2);
  rcWnd.left   += x;
  rcWnd.top    += y;
  rcWnd.right  += x;
  rcWnd.bottom += y;

  delta = (rcWnd.right + rcWnd.left - right - left)/2;

  if(nSel>0)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        rc.left   = wi->m_rcItem.left + delta;
        rc.top    = wi->m_rcItem.top;
        rc.right  = wi->m_rcItem.right + delta;
        rc.bottom = wi->m_rcItem.bottom;

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::CenterVertInWnd
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::CenterVertInWnd
  (
  )

  {
  COORDINATE  top, bottom;
  int         nSel;
  int         x;
  int         y;
  RECT        rcWnd;
  int         delta;

  nSel = GetSelectionBounds(NULL, &top, NULL, &bottom);
  x = m_pWnd->HasHorzScroll() ? GetScrollPos(m_hwndParent, SB_HORZ) : 0;
  y = m_pWnd->HasVertScroll() ? GetScrollPos(m_hwndParent, SB_VERT) : 0;

  GetClientRect(m_hwndParent, &rcWnd);
  OWndDStoLS(m_oiWnd, (LPPOINT)&rcWnd, 2);
  rcWnd.left   += x;
  rcWnd.top    += y;
  rcWnd.right  += x;
  rcWnd.bottom += y;

  delta = (rcWnd.bottom + rcWnd.top - bottom - top)/2;

  if(nSel>0)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        rc.left   = wi->m_rcItem.left;
        rc.top    = wi->m_rcItem.top + delta;
        rc.right  = wi->m_rcItem.right;
        rc.bottom = wi->m_rcItem.bottom + delta;

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DefaultHorz
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DefaultHorz
  (
  )

  {
  OpWndItemD* wi = NULL;
  RECT        rc;

  while((wi=GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      CopyRect(&rc, &wi->m_rcItem);

      AdjustNewRect(wi, &rc, OWND_ADJRECT_DEFAULT_W | OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
      if(!EqualRect(&rc, &wi->m_rcItem))
        {
        Move(wi, &rc);
        m_pWnd->SetDirtyFlag();
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::DefaultVert
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::DefaultVert
  (
  )

  {
  OpWndItemD* wi = NULL;
  RECT        rc;

  while((wi=GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      CopyRect(&rc, &wi->m_rcItem);

      AdjustNewRect(wi, &rc, OWND_ADJRECT_DEFAULT_H | OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
      if(!EqualRect(&rc, &wi->m_rcItem))
        {
        Move(wi, &rc);
        m_pWnd->SetDirtyFlag();
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GrowHorz
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::GrowHorz
  (
  )

  {
  COORDINATE  width;
  int         nSel = GetMinMaxSizes(NULL, &width, NULL, NULL);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // adjust the item width
        rc.top    = wi->m_rcItem.top;
        rc.bottom = wi->m_rcItem.bottom;
        if(wi->m_rcItem.left <= wi->m_rcItem.right)
          {
          rc.left  = wi->m_rcItem.left;
          rc.right = rc.left + width;
          }
        else
          {
          rc.right = wi->m_rcItem.right;
          rc.left  = rc.right + width;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GrowVert
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::GrowVert
  (
  )

  {
  COORDINATE  height;
  int         nSel = GetMinMaxSizes(NULL, NULL, NULL, &height);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // adjust the item height
        rc.left  = wi->m_rcItem.left;
        rc.right = wi->m_rcItem.right;
        if(wi->m_rcItem.top <= wi->m_rcItem.top)
          {
          rc.top    = wi->m_rcItem.top;
          rc.bottom = rc.top + height;
          }
        else
          {
          rc.bottom = wi->m_rcItem.bottom;
          rc.top    = rc.bottom + height;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ShrinkHorz
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::ShrinkHorz
  (
  )

  {
  COORDINATE  width;
  int         nSel = GetMinMaxSizes(&width, NULL, NULL, NULL);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // adjust the item width
        rc.top    = wi->m_rcItem.top;
        rc.bottom = wi->m_rcItem.bottom;
        if(wi->m_rcItem.left <= wi->m_rcItem.right)
          {
          rc.left  = wi->m_rcItem.left;
          rc.right = rc.left + width;
          }
        else
          {
          rc.right = wi->m_rcItem.right;
          rc.left  = rc.right + width;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ShrinkVert
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::ShrinkVert
  (
  )

  {
  COORDINATE  height;
  int         nSel = GetMinMaxSizes(NULL, NULL, &height, NULL);

  if(nSel>1)
    {
    OpWndItemD* wi = NULL;
    RECT        rc;

    while((wi=GetNextItem(wi, TRUE)) != NULL)
      {
      if(IsItemSelected(wi->m_index))
        {
        // adjust the item height
        rc.left  = wi->m_rcItem.left;
        rc.right = wi->m_rcItem.right;
        if(wi->m_rcItem.top <= wi->m_rcItem.top)
          {
          rc.top    = wi->m_rcItem.top;
          rc.bottom = rc.top + height;
          }
        else
          {
          rc.bottom = wi->m_rcItem.bottom;
          rc.top    = rc.bottom + height;
          }

        AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
        if(!EqualRect(&rc, &wi->m_rcItem))
          {
          Move(wi, &rc);
          m_pWnd->SetDirtyFlag();
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SpaceHorz
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::SpaceHorz
  (
  )

  {
  AMEMBLOCKID    miSE  = AMemAllocate(5*sizeof(SPACEEVENLY));
  SPACEEVENLY*  pse   = (SPACEEVENLY*)AMemLock(miSE);

  OpWndItemD*   wi    = NULL;
  int           left  = INT_MAX;
  int           right = INT_MIN;
  int           width = 0;
  int           nSel  = 0;

  I_ASSERT(pse);

  // get the bounding rectangle and fill the memblock
  while((wi=GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      if(((nSel+1) % 5) == 0)
        {
        AMemUnlock(miSE);
        AMemSetSize(miSE, (nSel+6)*sizeof(SPACEEVENLY));
        pse = (SPACEEVENLY*)AMemLock(miSE);
        I_ASSERT(pse);
        }

      pse[nSel].wi     = wi;
      pse[nSel].nStart = nMin(wi->m_rcItem.left, wi->m_rcItem.right);
      width += abs(wi->m_rcItem.right - wi->m_rcItem.left);

      left  = nMin(left,  nMin(wi->m_rcItem.left, wi->m_rcItem.right));
      right = nMax(right, nMax(wi->m_rcItem.left, wi->m_rcItem.right));

      nSel++;
      }
    }

  // there must be at least three items for this operation to make sense
  if(nSel>2)
    {
    int   i;
    int   space;
    int   delta;
    int   newLeft;
    RECT  rc;

    // sort the items visually and compute the space between each item
    qsort(pse, (size_t)nSel, sizeof(SPACEEVENLY), fcmpSpaceEvenly);
    space = (right - left - width)/(nSel - 1);

    // do the actual moving of items
    newLeft = left;
    for(i=0; i<nSel; i++)
      {
      CopyRect(&rc, &pse[i].wi->m_rcItem);
      delta = newLeft - nMin(rc.left, rc.right);
      if(newLeft < left)
        delta = left - nMin(rc.left, rc.right);
      if((nMax(rc.left, rc.right) + delta) > right)
        delta = right - nMax(rc.left, rc.right);

      rc.left  += delta;
      rc.right += delta;

      AdjustNewRect(pse[i].wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
      if(!EqualRect(&rc, &pse[i].wi->m_rcItem))
        {
        Move(pse[i].wi, &rc);
        m_pWnd->SetDirtyFlag();
        }

      newLeft = nMax(rc.left, rc.right) + space;
      }
    }

  AMemUnlock(miSE);
  AMemFree(miSE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SpaceVert
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::SpaceVert
  (
  )

  {
  AMEMBLOCKID    miSE    = AMemAllocate(5*sizeof(SPACEEVENLY));
  SPACEEVENLY*  pse     = (SPACEEVENLY*)AMemLock(miSE);

  OpWndItemD*   wi      = NULL;
  int           top     = INT_MAX;
  int           bottom  = INT_MIN;
  int           height  = 0;
  int           nSel    = 0;

  I_ASSERT(pse);

  // get the bounding rectangle and fill the memblock
  while((wi=GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      if(((nSel+1) % 5) == 0)
        {
        AMemUnlock(miSE);
        AMemSetSize(miSE, (nSel+6)*sizeof(SPACEEVENLY));
        pse = (SPACEEVENLY*)AMemLock(miSE);
        I_ASSERT(pse);
        }

      pse[nSel].wi     = wi;
      pse[nSel].nStart = nMin(wi->m_rcItem.bottom, wi->m_rcItem.top);
      height += abs(wi->m_rcItem.bottom - wi->m_rcItem.top);

      top    = nMin(top,    nMin(wi->m_rcItem.bottom, wi->m_rcItem.top));
      bottom = nMax(bottom, nMax(wi->m_rcItem.bottom, wi->m_rcItem.top));

      nSel++;
      }
    }

  // there must be at least three items for this operation to make sense
  if(nSel>2)
    {
    int   i;
    int   space;
    int   delta;
    int   newTop;
    RECT  rc;

    // sort the items visually and compute the space between each item
    qsort(pse, (size_t)nSel, sizeof(SPACEEVENLY), fcmpSpaceEvenly);
    space = (bottom - top - height)/(nSel - 1);

    // do the actual moving of items
    newTop = top;
    for(i=0; i<nSel; i++)
      {
      CopyRect(&rc, &pse[i].wi->m_rcItem);
      delta = newTop - nMin(rc.top, rc.bottom);
      if(newTop < top)
        delta = top - nMin(rc.top, rc.bottom);
      if((nMax(rc.top, rc.bottom) + delta) > bottom)
        delta = bottom - nMax(rc.top, rc.bottom);

      rc.top    += delta;
      rc.bottom += delta;

      AdjustNewRect(pse[i].wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
      if(!EqualRect(&rc, &pse[i].wi->m_rcItem))
        {
        Move(pse[i].wi, &rc);
        m_pWnd->SetDirtyFlag();
        }

      newTop = nMax(rc.top, rc.bottom) + space;
      }
    }

  AMemUnlock(miSE);
  AMemFree(miSE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::PosAndSize
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::PosAndSize
  (
  )

  {
  int         nSel    = 0;
  OpWndItemD* wi      = NULL;
  OpWndItemD* wiFirst = NULL;

  // count the selections and get the first item
  while((wi = GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      if(!wiFirst)
        wiFirst = wi;

      nSel++;
      }
    }

  if(nSel >= 1)
    {
    POSSIZE_DLG psd = {m_pWnd->GetLayout()->CurrentUnits(), nSel, wiFirst, this};

    m_pWnd->GetLayout()->EnablePalettes(FALSE);
    DialogBoxParam(g_hInstance, ID(DLG_ITEMSIZES), m_pWnd->WndHandle(), OWLayItemSizesDlgProc, (LPARAM)&psd);
    m_pWnd->GetLayout()->EnablePalettes(TRUE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Attach
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Attach
  (
  LPPOINT lppt
  )

  {
  int         nAnchorFlags = 0;
  BOOL        fDifferent = FALSE;
  char        szTitle[255];
  OpWndItemD* wi = lppt ? ItemFromPoint(lppt, TRUE) : NULL;
  int         nSel = 0;

  if(wi)
    {
    char  szItem[80];

    // get the anchor flags for the hit object
    int tmp = OWndLayGetItemGrowflag(wi);
    if(tmp & OWNDLAY_ITEM_GROW_LEFT)
      nAnchorFlags |= ANCHOR_LEFT;
    if(tmp & OWNDLAY_ITEM_GROW_TOP)
      nAnchorFlags |= ANCHOR_TOP;
    if(tmp & OWNDLAY_ITEM_GROW_RIGHT)
      nAnchorFlags |= ANCHOR_RIGHT;
    if(tmp & OWNDLAY_ITEM_GROW_BOTTOM)
      nAnchorFlags |= ANCHOR_BOTTOM;

    // get the item name
    if(OWndLayIsItemShape(wi))
      (m_pWnd->GetLayout()->Shapes())->GetShapeName(wi, szItem, 80);
    else if(OWndLayIsItemGroup(wi))
      wsprintf(szItem, "Group");
    else
      AObjGetName(wi->Object(), szItem, 80);
    wsprintf(szTitle, "Attach Item \"%s\"", szItem);
    }
  else
    {
    // anchor for all the selected items
    OpWndItemD* wiTmp   = NULL;
    BOOL        fFirst  = TRUE;
    int         nFirstFlags;

    // change the window title to be reasonable for the selection
    lstrcpy(szTitle, "Attach Selected Items");

    while((wiTmp = GetNextItem(wiTmp, TRUE)) != NULL)
      {
      if(IsItemSelected(wiTmp->m_index))
        {
        if(fFirst)
          {
          fFirst = FALSE;
          nFirstFlags = OWndLayGetItemGrowflag(wiTmp);
          }
        else if(nFirstFlags != OWndLayGetItemGrowflag(wiTmp))
          fDifferent = TRUE;

        nSel++;
        }
      }

    if(!fDifferent)
      {
      if(nFirstFlags & OWNDLAY_ITEM_GROW_LEFT)
        nAnchorFlags |= ANCHOR_LEFT;
      if(nFirstFlags & OWNDLAY_ITEM_GROW_TOP)
        nAnchorFlags |= ANCHOR_TOP;
      if(nFirstFlags & OWNDLAY_ITEM_GROW_RIGHT)
        nAnchorFlags |= ANCHOR_RIGHT;
      if(nFirstFlags & OWNDLAY_ITEM_GROW_BOTTOM)
        nAnchorFlags |= ANCHOR_BOTTOM;
      }
    }

  if((nSel>0) || wi)
    {
    AnchorItem  ai(nAnchorFlags, nSel>1, szTitle);

    m_pWnd->GetLayout()->EnablePalettes(FALSE);
    if(DialogBoxParam(g_hInstance, ID(DLG_ATTACH), m_pWnd->GetLayout()->WndHandle(), AnchorItem::DlgProc, (long)&ai))
      {
      int   tmp       = ai.GetAnchorFlags();
      short nNewFlags = 0;

      if(tmp & ANCHOR_LEFT)
        nNewFlags |= OWNDLAY_ITEM_GROW_LEFT;
      if(tmp & ANCHOR_TOP)
        nNewFlags |= OWNDLAY_ITEM_GROW_TOP;
      if(tmp & ANCHOR_RIGHT)
        nNewFlags |= OWNDLAY_ITEM_GROW_RIGHT;
      if(tmp & ANCHOR_BOTTOM)
        nNewFlags |= OWNDLAY_ITEM_GROW_BOTTOM;

      if(wi)
        OWndLaySetItemGrowflag(wi, nNewFlags);
      else
        {
        while((wi=GetNextItem(wi, TRUE)) != NULL)
          {
          if(IsItemSelected(wi->m_index))
            OWndLaySetItemGrowflag(wi, nNewFlags);
          }
        }

      m_pWnd->SetDirtyFlag();
      }
    m_pWnd->GetLayout()->EnablePalettes(TRUE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::SendToBack
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::SendToBack
  (
  )

  {
  OpWndItemD*   wi    = NULL;
  OpWndItemD*   wiEnd = NULL;

  while(((wi = GetPrevItem(wi, TRUE)) != NULL) && (!wiEnd || wi!=wiEnd))
    {
    if(wiEnd && (wi==wiEnd))
      break;

    if(IsItemSelected(wi->m_index))
      {
      if(!wiEnd)
        wiEnd = wi;

      wi = GetNextItem(wi, TRUE);
      SetZOrder(GetPrevItem(wi, TRUE), 0);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::BringToFront
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

int Items::BringToFront
  (
  )

  {
  int           nSel  = 0;
  OpWndItemD*   wi    = NULL;
  OpWndItemD*   wiEnd = NULL;

  while(((wi = GetNextItem(wi, TRUE)) != NULL) && (!wiEnd || wi!=wiEnd))
    {
    if(IsItemSelected(wi->m_index))
      {
      if(!wiEnd)
        wiEnd = wi;

      wi = GetPrevItem(wi, TRUE);
      SetZOrder(GetNextItem(wi, TRUE), m_nItems-1);
      nSel++;
      }
    }

  return nSel;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Group
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Group
  (
  )

  {
  int             nSel;
  AMEMBLOCKID      miItems;
  OpWndItemD**    awi;
  OpGroupWndItem* gwi;
  int             nItems;
  OpWndItemD*     wi        = NULL;

  // bring all selected items to the front
  nItems = nSel = BringToFront();

  // copy all the items to the list for the group
  miItems = AMemAllocate(nSel * sizeof(OpWndItemD*));
  awi = (OpWndItemD**)AMemLock(miItems);
  while((nSel>0) && ((wi = GetPrevItem(wi, TRUE)) != NULL))
    {
    I_ASSERT(IsItemSelected(wi->m_index));

    nSel--;
    awi[nSel] = wi;
    }
  I_ASSERT(nSel==0);
  AMemUnlock(miItems);

  gwi = (OpGroupWndItem*)OWndLayCreateNewGroup(m_oiWnd, miItems, nItems);
  AddNewItems((OpWndItemD**)&gwi, NULL, NULL, NULL, NULL, 1);

  wi = NULL;
  while((wi = gwi->GetNextItem(wi, TRUE)) != NULL)
    {
    m_pItems[wi->m_index].nGroup = gwi->m_index;
    SetItemGrouped(wi->m_index, TRUE);
    }

  AMemFree(miItems);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Ungroup
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Ungroup
  (
  )

  {
  OpWndItemD* wi = NULL;

  while((wi = GetNextItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index) && OWndLayIsItemGroup(wi))
      {
      int   i;
      int   j       = 0;
      int   nItems  = ((OpGroupWndItem*)wi)->NumItems(FALSE);
      int   idx     = wi->m_index;

      // notify the group that it will be destroyed and needs to restore its items
      ((OpGroupWndItem*)wi)->Ungrouped();

      for(i=1; i<=nItems; i++)
        {
        I_ASSERT((idx-i-j) >= 0);

        // ungroup the item
        SetItemGrouped(idx-i-j, FALSE);
        m_pItems[idx-i-j].nGroup = -1;

        // select the item
        if(!IsItemLocked(idx-i-j) && OWndIsItemVisible(m_pItems[idx-i-j].wi))
          {
          m_pItems[idx-i-j].flags |= FLAG_SELECTED;
          m_nSel++;
          InvalidateGrabHandles(m_pItems[idx-i-j].wi, FALSE);
          }

        // calculate the extra offset if the item is a group
        if(OWndLayIsItemGroup(m_pItems[idx-i-j].wi))
          j += ((OpGroupWndItem*)m_pItems[idx-i-j].wi)->NumItems(TRUE);
        }

      // delete item and fix the list -- make sure the indexes and ctl ids are correct
      InvalidateGrabHandles(wi, TRUE);
      m_nSel--;
      if(m_fFrames)
        InvalidateItem(wi, TRUE);
      OWndLayDestroyItem(wi);
      for(j = idx + 1; j<m_nItems; j++)
        {
        m_pItems[j-1] = m_pItems[j];
        m_pItems[j-1].wi->m_index = j - 1;

        if(OWndIsHwndItem(m_pItems[j-1].wi))
          SetItemCtlID(((OpHwndWndItemD*)m_pItems[j-1].wi)->m_hwndItem, m_pItems[j-1].wi->m_index + OWNDLAY_ITEM_ID_OFFSET);

        // redraw the item to fix the item index
        if(m_fFrames)
          InvalidateItem(m_pItems[j-1].wi, TRUE);

        // fix the group index
        if(m_pItems[j-1].nGroup>0)
          m_pItems[j-1].nGroup--;
        }

      // resize the list of items
      m_nItems--;
      AMemUnlock(m_miItems);
      AMemSetSize(m_miItems, m_nItems*sizeof(WND_ITEMS));
      m_pItems = (WND_ITEMS*)AMemLock(m_miItems);

      m_pWnd->SetDirtyFlag();
      break;
      }
    #ifdef AW_I_DEBUG
    else if(IsItemSelected(wi->m_index))
      I_ASSERT(0);
    #endif
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Lock
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Lock
  (
  LPPOINT lppt
  )

  {
  OpWndItemD* wi = ItemFromPoint(lppt, TRUE);

  if(wi)
    {
    BOOL  fLock = !OWndLayIsItemLocked(wi);

    OWndLaySetItemLocked(wi, fLock);

    if(OWndLayIsItemGroup(wi))
      ((OpGroupWndItem*)wi)->Lock(fLock);

    if(fLock && IsItemSelected(wi->m_index))
      SelectItem(wi->m_index, FALSE);

    InvalidateItem(wi, TRUE);
    m_pWnd->SetDirtyFlag();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::MouseMove
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::MouseMove
  (
  LPPOINT   lppt,
  HCURSOR*  phcur,
  int       fwKeys
  )

  {
  if(m_fTrack & (TRACK_RESIZEMASK | TRACK_MOVE | TRACK_SELECT))
    {
    I_ASSERT(!(m_fTrack & ~(TRACK_RESIZEMASK | TRACK_MOVE |
                            TRACK_SELECT | TRACK_WAITFORDELTA)));

    // just do the tracking.  There is no need to change the cursor
    if(m_fTrack & TRACK_WAITFORDELTA)
      {
      if((abs(lppt->x - m_ptStart.x) >= TRACK_DELTA)
                ||
         (abs(lppt->y - m_ptStart.y) >= TRACK_DELTA))
        {
        m_fTrack &= ~TRACK_WAITFORDELTA;
        }
      }

    if(!(m_fTrack & TRACK_WAITFORDELTA))
      {
      SetPoint(lppt, (lppt->x>0) ? lppt->x : 0, (lppt->y>0) ? lppt->y : 0);
      Track(lppt, fwKeys, TRUE, TRUE);
      }
    }
  else
    {
    I_ASSERT(!m_fTrack);

    OpWndItemD* wi      = NULL;
    long        lFound  = OWND_POINT_IN_NONE;

    while(((wi = GetPrevItem(wi, TRUE)) != NULL) && (lFound==OWND_POINT_IN_NONE))
      {
      lFound = GetStdCursor(wi, lppt, phcur);
      }

    if(lFound==OWND_POINT_IN_NONE)
      *phcur = LoadCursor(NULL, IDC_ARROW);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::LButtonDown
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::LButtonDown
  (
  LPPOINT   lppt,
  HCURSOR*  phcur,
  int       fwKeys
  )

  {
  I_ASSERT(!m_fTrack);

  OpWndItemD* wi  = NULL;
  POINT       pt  = *lppt;
  HCURSOR     hcur;

  m_pWnd->SnapPointToGrid(lppt);

  while((wi = GetPrevItem(wi, TRUE)) != NULL)
    {
    if(IsItemSelected(wi->m_index))
      {
      long  lRet = GetStdCursor(wi, &pt, &hcur);

      if(lRet!=OWND_POINT_IN_NONE)
        {
        // the point is somewhere in the item
        if((fwKeys & MK_SHIFT) && (lRet==OWND_POINT_IN_ITEM))
          {
          // unselect and exit
          SelectItem(wi->m_index, FALSE);
          *phcur = LoadCursor(NULL, IDC_ARROW);
          }
        else
          {
          // start tracking
          *phcur = hcur;
          CopyPoint(&m_ptStart, lppt);
          SetCapture(m_hwndParent);
          if(lRet==OWND_POINT_IN_ITEM)
            {
            m_fTrack = TRACK_MOVE | TRACK_WAITFORDELTA;
            GetSelectionBounds(&m_rcMaxMove.left, &m_rcMaxMove.top, NULL, NULL);
            }
          else
            {
            I_ASSERT((lRet>OWND_POINT_IN_NONE) && (lRet<=OWND_POINT_IN_R));
            m_fTrack = lRet | TRACK_WAITFORDELTA;
            m_wiPrimary = wi;
            }
          }
        break;
        }
      }
    else if(!IsItemLocked(wi->m_index) && PointInItem(wi, &pt))
      {
      #ifdef AW_I_DEBUG
      long  lRet =
      #endif
      GetStdCursor(wi, &pt, &hcur);
      I_ASSERT(lRet!=OWND_POINT_IN_NONE);

      // select the current item
      if(!(fwKeys & MK_SHIFT))
        SelectAll(FALSE);
      SelectItem(wi->m_index, TRUE);

      // always start moving a previously unselected item
      CopyPoint(&m_ptStart, lppt);
      SetCapture(m_hwndParent);
      m_fTrack = TRACK_MOVE | TRACK_WAITFORDELTA;
      GetSelectionBounds(&m_rcMaxMove.left, &m_rcMaxMove.top, NULL, NULL);

      // make sure we are using the move cursor
      *phcur = LoadCursor(NULL, IDC_SIZE);
      break;
      }
    }

  if(!wi)
    {
    if(!(fwKeys & MK_SHIFT))
      SelectAll(FALSE);

    CopyPoint(&m_ptStart, lppt);
    SetCapture(m_hwndParent);
    m_fTrack = TRACK_SELECT | TRACK_WAITFORDELTA;
    *phcur = LoadCursor(NULL, IDC_ARROW);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::LButtonUp
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::LButtonUp
  (
  LPPOINT   lppt,
  HCURSOR*  phcur,
  int       fwKeys
  )

  {
  I_UNUSED(phcur);

  if(m_fTrack & TRACK_WAITFORDELTA)
    {
    m_fTrack = 0;
    ReleaseCapture();
    SetPointEmpty(&m_ptStart);
    }
  else if(m_fTrack & TRACK_SELECT)
    {
    Track(lppt, fwKeys, FALSE, TRUE);
    SelectItemsInRect(&m_rcLast);
    m_fTrack = 0;
    ReleaseCapture();
    SetPointEmpty(&m_ptStart);
    SetRectEmpty(&m_rcLast);
    }
  else if((m_fTrack & TRACK_MOVE) || (m_fTrack & TRACK_RESIZEMASK))
    {
    Track(lppt, fwKeys, FALSE, TRUE);
    MoveItems();
    m_fTrack = 0;
    m_wiPrimary = NULL;
    ReleaseCapture();
    SetPointEmpty(&m_ptStart);
    SetTrackingRectsEmpty();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::RButtonDown
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::RButtonDown
  (
  LPPOINT   lppt,
  HCURSOR*  phcur,
  int       fwKeys
  )

  {
  if(m_fTrack & TRACK_WAITFORDELTA)
    {
    m_fTrack = 0;
    ReleaseCapture();
    SetPointEmpty(&m_ptStart);
    MouseMove(lppt, phcur, fwKeys);
    }
  else if(m_fTrack & TRACK_SELECT)
    {
    Track(lppt, fwKeys, FALSE, TRUE);
    m_fTrack = 0;
    ReleaseCapture();
    SetPointEmpty(&m_ptStart);
    SetRectEmpty(&m_rcLast);
    MouseMove(lppt, phcur, fwKeys);
    }
  else if((m_fTrack & TRACK_MOVE) || (m_fTrack & TRACK_RESIZEMASK))
    {
    Track(lppt, fwKeys, FALSE, TRUE);
    m_fTrack = 0;
    m_wiPrimary = NULL;
    ReleaseCapture();
    SetPointEmpty(&m_ptStart);
    SetTrackingRectsEmpty();
    MouseMove(lppt, phcur, fwKeys);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::KeyDown
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::KeyDown
  (
  int   wVKey,
  DWORD dwKeyData
  )

  {
  I_UNUSED(dwKeyData);

  int fwKeys  = GetKeyFlags();

  if(!m_fTrack
          &&
     (m_nSel>=1)
          &&
     ((wVKey==VK_UP) || (wVKey==VK_DOWN) ||(wVKey==VK_LEFT) ||(wVKey==VK_RIGHT)))
    {
    if((m_nSel==1) && (fwKeys & MK_SHIFT))
      {
      // resizing a single selection -- unless it is a group
      OpWndItemD* wi = NULL;

      while((wi = GetNextItem(wi, TRUE)) != NULL)
        {
        if(IsItemSelected(wi->m_index))
          {
          if(OWndLayIsItemGroup(wi))
            wi = NULL;

          break;
          }
        }

      if(wi)
        NudgeResizeItem(wi, wVKey);
      }
    else if(!(fwKeys & (MK_CONTROL | MK_SHIFT)))
      Nudge(wVKey);
    }
  else if((wVKey==VK_TAB) && (m_nItems>0))
    TabSelect((fwKeys & MK_SHIFT) ? &Items::GetPrevItem : &Items::GetNextItem);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::KeyUp
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::KeyUp
  (
  int   wVKey,
  DWORD dwKeyData
  )

  {
  I_UNUSED(wVKey);
  I_UNUSED(dwKeyData);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::StartReorder
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

BOOL Items::StartReorder
  (
  )

  {
  m_nNext = 0;

  return (NumItems(FALSE)>1);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Reorder
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Reorder
  (
  LPPOINT lppt,
  BOOL    fSendToFront
  )

  {
  OpWndItemD* wi = ItemFromPoint(lppt, TRUE);

  I_ASSERT(NumItems(FALSE)>1);

  if(wi && fSendToFront)
    {
    SetZOrder(wi, m_nItems-1);
    m_nNext = 0;
    }
  else if(wi && (wi->m_index == m_nNext))
    m_nNext += OWndLayIsItemGroup(wi) ? ((OpGroupWndItem*)wi)->NumItems(TRUE) : 1;
  else if(wi && (wi->m_index > m_nNext))
    {
    #ifdef AW_I_DEBUG
    int nInc = SetZOrder(wi, m_nNext);
    I_ASSERT(nInc>0);
    m_nNext += nInc;
    #else
    m_nNext += SetZOrder(wi, m_nNext);
    #endif
    }

  if(m_nNext==m_nItems)
    m_pWnd->GetLayout()->Shapes()->ChangeTool(NULL, OSHP_STDTOOL_ARROW);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::GroupStop
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::GroupStop
  (
  LPPOINT lppt
  )

  {
  OpWndItemD* wi = ItemFromPoint(lppt, FALSE);

  if(wi)
    {
    OWndLaySetItemGroupstop(wi, !OWndLayIsItemGroupstop(wi));
    InvalidateItem(wi, TRUE);
    m_pWnd->SetDirtyFlag();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::TabStop
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::TabStop
  (
  LPPOINT lppt
  )

  {
  OpWndItemD* wi = ItemFromPoint(lppt, FALSE);

  if(wi && OWndIsItemActivatable(wi))
    {
    OWndLaySetItemTabstop(wi, !OWndLayIsItemTabstop(wi));
    InvalidateItem(wi, TRUE);
    m_pWnd->SetDirtyFlag();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::Visibility
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::Visibility
  (
  LPPOINT lppt
  )

  {
  OpWndItemD* wi = ItemFromPoint(lppt, TRUE);

  if(wi)
    {
    BOOL  fVisible = !OWndIsItemVisible(wi);
    OWndLaySetItemVisibleFlag(wi, fVisible);

    if(OWndLayIsItemGroup(wi))
      ((OpGroupWndItem*)wi)->SetVisible(fVisible, TRUE);

    InvalidateItem(wi, TRUE);
    m_pWnd->SetDirtyFlag();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ViewportChanged
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void Items::ViewportChanged
  (
  LPPOINT lpptOld
  )

  {
  POINT ptNew;
  int   nFlag = 0;


  OWndLayGetViewportOrgEx(m_oiWnd, &ptNew);
  nFlag |= (lpptOld->x!=ptNew.x) ? OWND_LOC_HSCROLL : 0;
  nFlag |= (lpptOld->y!=ptNew.y) ? OWND_LOC_VSCROLL : 0;

  if(nFlag)
    {
    OpWndItemD* wi = NULL;
    POINT       ptScroll = {ptNew.x - lpptOld->x, ptNew.y - lpptOld->y};

    OWndDStoLS(m_oiWnd, &ptScroll, 1);
    ScrollWindow(m_hwndParent, ptScroll.x, ptScroll.y, NULL, NULL);

    while((wi = GetNextItem(wi, FALSE)) != NULL)
      wi->ScreenLocationChanged(nFlag);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Items::ViewportChanged
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

int Items::GetKeyFlags
  (
  )

  {
  int fwKeys  = 0;

  if(GetKeyState(VK_SHIFT)   < 0)
    fwKeys |= MK_SHIFT;
  if(GetKeyState(VK_CONTROL) < 0)
    fwKeys |= MK_CONTROL;
  if(GetKeyState(VK_LBUTTON) < 0)
    fwKeys |= MK_LBUTTON;
  if(GetKeyState(VK_MBUTTON) < 0)
    fwKeys |= MK_MBUTTON;
  if(GetKeyState(VK_RBUTTON) < 0)
    fwKeys |= MK_RBUTTON;

  return fwKeys;
  }

////////////////////////////////////////////////////////////////////////////
