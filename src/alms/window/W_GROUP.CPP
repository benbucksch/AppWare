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
// FILE:          "w_group.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define OWND_DEFAULTIMPLEMENTATION
#include "w_includ.h"

#include <stdlib.h>
#include <limits.h>

#include <o_upsf.h>
#include <o_wndgrp.h>

// local function prototypes ///////////////////////////////////////////////

OpWndItemD* ALMAPI CreateGroupWndItem(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long lStatus, AMEMBLOCKID mExtraData);
void localCreateGrabHandle(AOBJECTID oiWindow, RECT* targetRect, RECT* sourceRect, int grabHandle);

// internal structure definitions //////////////////////////////////////////

/* structure for each item */
struct GWI_ITEM
  {
  long  uid;
  short grow;
  long  unused;
  };

/* file record... -- size is sizeof(short) + nCount*sizeof(GWI_ITEM) */
#define VERSION_GROUPITEM   0x0100
struct GWI_FILE
  {
  short     sVer;
  GWI_ITEM  item[1];
  };

/* GroupItems structure -- runtime data structure */
struct GroupItems
  {
  long        uid;  /* uid of the item */
  short       grow; /* original grow flags of the item */
  OpWndItemD* wi;   /* pointer to the item */
  };

/* Specialized  group for window layout */
class OpLayGroupWndItem
  : public OpGroupWndItem

  {
  private:
    OpWndItemD* m_wiPointIn;  // window item the cursor is in

  public:
                OpLayGroupWndItem(AOBJECTID oiSelf, AOBJECTID oiParent, LPRECT lprcItem, long status, AMEMBLOCKID miExtra);
               ~OpLayGroupWndItem();

    /* overridden OpGroupWndItem methods */
    BOOL ALMAPI PointInItem(LPPOINT lppt);
    BOOL ALMAPI ItemInRect(LPRECT lprc, BOOL fReqFullEnclosure);
  };


// ********************************************************************** //

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     MakeRealRect
//
//  DESCRIPTION:  This converts a rectangle so the left and top are smaller
//                than the right and bottom.
//
////////////////////////////////////////////////////////////////////////////

void MakeRealRect
  (
  LPRECT        lprcDst,
  const LPRECT  lprcSrc
  )

  {
  RECT  rc;

  rc.left   = (lprcSrc->left > lprcSrc->right)  ? lprcSrc->right  : lprcSrc->left;
  rc.top    = (lprcSrc->top  > lprcSrc->bottom) ? lprcSrc->bottom : lprcSrc->top;
  rc.right  = (lprcSrc->left > lprcSrc->right)  ? lprcSrc->left   : lprcSrc->right;
  rc.bottom = (lprcSrc->top  > lprcSrc->bottom) ? lprcSrc->top    : lprcSrc->bottom;

  CopyRect(lprcDst, &rc);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     ShapeLibAdmin
//
//  DESCRIPTION:  This function is called to get information about the group
//                class.  It is treated like a shape in terms of creation,
//                but it will be marked as a group type.
//
////////////////////////////////////////////////////////////////////////////

extern "C" LONG ALMAPI ShapeLibAdmin
  (
  UINT          nMsg,
  pAObjMessage  pMsg
  )

  {
  LONG  lRet;

  ADBG_ASSERT_E(pMsg != NULL, return A_ERROR, "Invalid pMsg");

  switch (nMsg)
    {
    case OSHP_QUERYLIBINFO:
      pMsg->message1 = (long)CreateGroupWndItem;
      pMsg->message2 = (long)AGROUP_LIBTYPEID;
      pMsg->message3 = 0L;
      lRet = A_OK;
      break;

    case OSHP_QUERYVERSIONANDID:
      pMsg->message2 = (long)VERSION_GROUPITEM;
      pMsg->message3 = (long)0;
      lRet = A_OK;
      break;

    default:
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     CreateGroupWndItem
//
//  DESCRIPTION:  This creates a group window item.
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* ALMAPI CreateGroupWndItem
  (
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  RECT*       rcItem,
  long        lStatus,
  AMEMBLOCKID  miExtraData
  )

  {
  OpWndItemD* wi;
  if(OWndIsLayoutTime())
    wi = new OpLayGroupWndItem(oiItem, oiParent, rcItem, lStatus, miExtraData);
  else
    wi = new OpGroupWndItem(oiItem, oiParent, rcItem, lStatus, miExtraData);

  return wi;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem Constructor
//
//  DESCRIPTION:  This function initializes a group item
//
////////////////////////////////////////////////////////////////////////////

OpGroupWndItem::OpGroupWndItem
  (
  AOBJECTID    oiSelf, 
  AOBJECTID    oiParent, 
  LPRECT      lprcItem, 
  long        status, 
  AMEMBLOCKID  miExtra
  ) : OpLiteWndItem(oiSelf, oiParent, lprcItem, status)

  {
  BOOL  fInit;

  m_fInit = FALSE;
  OWndLaySetItemIsGroup(this, TRUE);
  OWndSetItemActivatable(this, FALSE);
  OWndSetItemTransparent(this, TRUE);

  if(miExtra)
    {
    GWI_FILE* gwiFile = (GWI_FILE*)AMemLock(miExtra);
    int       i;

    if(gwiFile->sVer == VERSION_GROUPITEM)
      {
      m_nItems = (int)((AMemGetSize(miExtra)-sizeof(short))/sizeof(GWI_ITEM));
      m_miItems = AMemAllocate((long)m_nItems*sizeof(GroupItems));
      m_pItems = m_nItems ? (GroupItems*)AMemLock(m_miItems) : NULL;
      for(i=0; i<m_nItems; i++)
        {
        m_pItems[i].uid  = gwiFile->item[i].uid;
        m_pItems[i].grow = gwiFile->item[i].grow;
        m_pItems[i].wi   = NULL;
        }

      fInit = TRUE;
      }
    else
      fInit = FALSE;
    AMemUnlock(miExtra);
    }
  else
    fInit = FALSE;

  if(!fInit)
    {
    m_nItems = 0;
    m_miItems = AMemAllocate(0L);
    m_pItems = NULL;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem Destructor
//
//  DESCRIPTION:  This function cleans up a group item
//
////////////////////////////////////////////////////////////////////////////

OpGroupWndItem::~OpGroupWndItem
  (
  )

  {
  if(m_nItems>0)
    AMemUnlock(m_miItems);
  AMemFree(m_miItems);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::GetBoundingRect (private)
//
//  DESCRIPTION:  This function is called to determine the bounding rect
//                of a group.
//
////////////////////////////////////////////////////////////////////////////

void OpGroupWndItem::GetBoundingRect
  (
  )

  {
  int   i;
  RECT  rcBounds = {INT_MAX, INT_MAX, INT_MIN, INT_MIN};
  RECT  rcTemp;

  I_ASSERT(m_fInit);
  // get the total bounding rect of all contained window items
  for(i=0; i<m_nItems; i++)
    {
    MakeRealRect(&rcTemp, &m_pItems[i].wi->m_rcItem);
    if(rcBounds.left   > rcTemp.left)   rcBounds.left   = rcTemp.left;
    if(rcBounds.top    > rcTemp.top)    rcBounds.top    = rcTemp.top;
    if(rcBounds.right  < rcTemp.right)  rcBounds.right  = rcTemp.right;
    if(rcBounds.bottom < rcTemp.bottom) rcBounds.bottom = rcTemp.bottom;
    }

  CopyRect(&m_rcItem, &rcBounds);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::NumItems
//
//  DESCRIPTION:  This function returns the number of items -- it can also
//                count nested items.
//
////////////////////////////////////////////////////////////////////////////


int ALMAPI OpGwiNumItems
  (
  OpGroupWndItem* 				gwi,
  BOOL            				fCountNested
  )

  {
  int 									iRet;

  I_ASSERT(gwi->m_fInit);
  if(fCountNested)
    {
    iRet = 0;
    for(int i=0; i<gwi->m_nItems; i++)
      {
      if(OWndLayIsItemGroup(gwi->m_pItems[i].wi))
        iRet += ((OpGroupWndItem*)gwi->m_pItems[i].wi)->NumItems(TRUE);

      iRet ++;
      }
    }
  else
    iRet = gwi->m_nItems;

  return iRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::AllObjects
//
//  DESCRIPTION:  This function returns TRUE if all items within the group
//                are objects (not shapes).
//
////////////////////////////////////////////////////////////////////////////


BOOL ALMAPI OpGwiAllObjects
  (
  OpGroupWndItem* gwi
  )

  {
  BOOL  fRet = TRUE;

  I_ASSERT(gwi->m_fInit);
  for(int i=0; fRet && i<gwi->m_nItems; i++)
    {
    if(OWndLayIsItemShape(gwi->m_pItems[i].wi))
      fRet = FALSE;
    else if(OWndLayIsItemGroup(gwi->m_pItems[i].wi))
      fRet = ((OpGroupWndItem*)(gwi->m_pItems[i].wi))->AllObjects();
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Created
//
//  DESCRIPTION:  This notifies the group that it was just created (and all
//                other objects that it could contain have been created).
//                if fHasUIDs==TRUE the group has the list of items from the
//                create function (loaded from disk).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGwiCreated
  (
  OpGroupWndItem* gwi,
  OpWndItemD**    wi,
  int		           nCount
  )

  {
  int i, j;

  I_ASSERT(((gwi->m_nItems>0) && gwi->m_pItems) || (wi && (nCount>0)));

  if(gwi->m_nItems>0)
    {
    BOOL  fResize = FALSE;

    for(i=0; i<gwi->m_nItems; i++)
      {
      gwi->m_pItems[i].wi = OWndItemFromUID(gwi->m_oiParent, gwi->m_pItems[i].uid);
      if(!gwi->m_pItems[i].wi)
        {
        // move the other ones down
        for(j=i+1; j<gwi->m_nItems; j++)
          gwi->m_pItems[j-1] = gwi->m_pItems[j];

        // flag the list as needing resizing
        fResize = TRUE;
        i--;
        gwi->m_nItems--;
        }
      }

    if(fResize)
      {
      AMemUnlock(gwi->m_miItems);
      AMemSetSize(gwi->m_miItems, gwi->m_nItems*sizeof(GroupItems));
      gwi->m_pItems = (GroupItems*)AMemLock(gwi->m_miItems);
      }
    }
  else
    {
    gwi->m_nItems = nCount;
    AMemSetSize(gwi->m_miItems, gwi->m_nItems*sizeof(GroupItems));
    gwi->m_pItems = (GroupItems*)AMemLock(gwi->m_miItems);

    for(i=0; i<gwi->m_nItems; i++)
      {
      gwi->m_pItems[i].wi   = wi[i];
      gwi->m_pItems[i].grow = wi[i]->m_grow;
      gwi->m_pItems[i].uid  = OWndUIDFromItem(wi[i]);

      // clear the grow flags for the item -- restored if the group is released
      wi[i]->m_grow = 0;
      }
    }

  gwi->m_fInit = TRUE;

  // get the group rectangle
  gwi->GetBoundingRect();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::SetVisible
//
//  DESCRIPTION:  This function sets all items' visibility flags.
//
////////////////////////////////////////////////////////////////////////////


void ALMAPI OpGwiSetVisible
  (
  OpGroupWndItem* gwi,
  BOOL            fVisible,
  BOOL            fFlagOnly
  )

  {
  I_ASSERT(gwi->m_fInit);
  for(int i=0; i<gwi->m_nItems; i++)
    {
    if(fFlagOnly)
      OWndLaySetItemVisibleFlag(gwi->m_pItems[i].wi, fVisible);
    else
      OWndSetItemVisible(gwi->m_pItems[i].wi, fVisible);

    if(OWndLayIsItemGroup(gwi->m_pItems[i].wi))
      ((OpGroupWndItem*)(gwi->m_pItems[i].wi))->SetVisible(fVisible, fFlagOnly);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::GetNextItem
//
//  DESCRIPTION:  This function gets the next item in the list.
//
////////////////////////////////////////////////////////////////////////////


OpWndItemD* ALMAPI OpGwiGetNextItem
  (
  OpGroupWndItem* gwi,
  OpWndItemD*     wi,
  BOOL            fSkipGroups
  )

  {
  OpWndItemD* wiRet = NULL;
  int         i;

  I_ASSERT(gwi->m_fInit);
  if(wi)
    {
    for(i=0; (i<gwi->m_nItems) && (wiRet==NULL); i++)
      {
      if(!fSkipGroups && OWndLayIsItemGroup(gwi->m_pItems[i].wi))
        wiRet = ((OpGroupWndItem*)(gwi->m_pItems[i].wi))->GetNextItem(wi, fSkipGroups);
      else if(gwi->m_pItems[i].wi == wi)
        wiRet = ((gwi->m_nItems-1)>i) ? gwi->m_pItems[i+1].wi : NULL;
      }
    }
  else
    wiRet = (gwi->m_nItems>0) ? gwi->m_pItems[0].wi : NULL;

  return wiRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::GetPrevItem
//
//  DESCRIPTION:  This function gets the previous item in the list.
//
////////////////////////////////////////////////////////////////////////////


OpWndItemD* ALMAPI OpGwiGetPrevItem
  (
  OpGroupWndItem* gwi,
  OpWndItemD*     wi,
  BOOL            fSkipGroups
  )

  {
  OpWndItemD* wiRet = NULL;
  int         i;

  I_ASSERT(gwi->m_fInit);
  if(wi)
    {
    for(i=gwi->m_nItems-1; (i>=0) && (wiRet==NULL); i--)
      {
      if(!fSkipGroups && OWndLayIsItemGroup(gwi->m_pItems[i].wi))
        wiRet = ((OpGroupWndItem*)(gwi->m_pItems[i].wi))->GetPrevItem(wi, fSkipGroups);
      else if(gwi->m_pItems[i].wi == wi)
        wiRet = (i>0) ? gwi->m_pItems[i-1].wi : NULL;
      }
    }
  else
    wiRet = (gwi->m_nItems>0) ? gwi->m_pItems[gwi->m_nItems-1].wi : NULL;

  return wiRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ItemRecreated
//
//  DESCRIPTION:  This function updates the window item pointer
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGwiItemRecreated
  (
  OpGroupWndItem* gwi,
  OpWndItemD*     wiNew,
  OpWndItemD*     wiOld
  )

  {
  I_ASSERT(gwi->m_fInit);
  for(int i=0; i<gwi->m_nItems; i++)
    {
    if(gwi->m_pItems[i].wi == wiOld)
      {
      gwi->m_pItems[i].wi = wiNew;
      break;
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ItemDeleted
//
//  DESCRIPTION:  This function removes an item from the group and returns
//                the new number of items in the group.
//
////////////////////////////////////////////////////////////////////////////

short ALMAPI OpGwiItemDeleted
  (
  OpGroupWndItem* gwi,
  OpWndItemD*     wiOld
  )

  {
  I_ASSERT(gwi->m_fInit);

  int   i;
  int   nIdx  = -1;

  for(i=0; i<gwi->m_nItems; i++)
    {
    if(gwi->m_pItems[i].wi == wiOld)
      {
      nIdx = i;
      break;
      }
    }

  if(nIdx>=0)
    {
    // move the items in the list and resize the list
    for(i=nIdx+1; i<gwi->m_nItems; i++)
      gwi->m_pItems[i-1] = gwi->m_pItems[i];

    gwi->m_nItems--;
    AMemUnlock(gwi->m_miItems);
    AMemSetSize(gwi->m_miItems, gwi->m_nItems*sizeof(GroupItems));
    gwi->m_pItems = (GroupItems*)AMemLock(gwi->m_miItems);

    // get the new group rectangle
    gwi->GetBoundingRect();
    }

  return (short)gwi->m_nItems;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ItemAdded
//
//  DESCRIPTION:  This function adds an item from the group and returns
//                the new number of items in the group.
//
////////////////////////////////////////////////////////////////////////////

short ALMAPI OpGwiItemAdded
  (
  OpGroupWndItem* gwi,
  OpWndItemD*     wiNew
  )

  {
  I_ASSERT(gwi->m_fInit);

  int   i;
  int   nIdx  = -1;
  int   nPos  = 0;

  for(i=0; i<gwi->m_nItems; i++)
    {
    if(gwi->m_pItems[i].wi == wiNew)
      {
      nIdx = i;
      break;
      }
    else if(gwi->m_pItems[i].wi->m_index < wiNew->m_index)
      nPos++;
    }

  if(nIdx<0)  // add a previously non-existent item
    {
    gwi->m_nItems++;
    AMemUnlock(gwi->m_miItems);
    AMemSetSize(gwi->m_miItems, gwi->m_nItems*sizeof(GroupItems));
    gwi->m_pItems = (GroupItems*)AMemLock(gwi->m_miItems);

    // move the items in the list and resize the list
    for(i=(gwi->m_nItems-1); i>nPos; i--)
      gwi->m_pItems[i] = gwi->m_pItems[i-1];

    // add the item to the list
    gwi->m_pItems[nPos].wi   = wiNew;
    gwi->m_pItems[nPos].grow = wiNew->m_grow;
    gwi->m_pItems[nPos].uid  = OWndUIDFromItem(wiNew);
    wiNew->m_grow = 0;

    // get the new group rectangle
    gwi->GetBoundingRect();
    }

  return (short)gwi->m_nItems;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Ungrouped
//
//  DESCRIPTION:  This notifies the group that it will be deleted and the
//                items need to be restored to their original state.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGwiUngrouped
  (
  OpGroupWndItem* gwi
  )

  {
  I_ASSERT(gwi->m_fInit);
  for(int i=0; i<gwi->m_nItems; i++)
    gwi->m_pItems[i].wi->m_grow = gwi->m_pItems[i].grow;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Lock
//
//  DESCRIPTION:  This function sets all items' lock flags.
//
////////////////////////////////////////////////////////////////////////////


void ALMAPI OpGwiLock
  (
  OpGroupWndItem* gwi,
  BOOL            fLock
  )

  {
  I_ASSERT(gwi->m_fInit);
  for(int i=0; i<gwi->m_nItems; i++)
    {
    OWndLaySetItemLocked(gwi->m_pItems[i].wi, fLock);

    if(OWndLayIsItemGroup(gwi->m_pItems[i].wi))
      ((OpGroupWndItem*)(gwi->m_pItems[i].wi))->Lock(fLock);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Destroy
//
//  DESCRIPTION:  This function deletes the window item
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::QueryProtocol
//
//  DESCRIPTION:  This function fills a query info structure with the
//                protocol information for groups.
//
////////////////////////////////////////////////////////////////////////////

long ALMAPI OpGroupWndItem::QueryProtocol
  (
  ApQueryInfo*  pqi,
  APROTOCOLID    piProt
  )

  {
  LONG  rVal = A_OK;

  if(piProt==APROT_WND)
    {
    pqi->size      = sizeof(ApQueryInfo);
    pqi->object    = NULL;
    pqi->type      = AGROUP_LIBTYPEID;
    pqi->protocol  = APROT_WND;
    pqi->pfnCreate = (APPCFN)CreateGroupWndItem;
    }
  else
    rVal = A_NOTHANDLED;

  return rVal;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Object
//
//  DESCRIPTION:  This function returns the group type id.
//
////////////////////////////////////////////////////////////////////////////

AOBJECTID ALMAPI OpGroupWndItem::Object
  (
  )

  {
  return (AOBJECTID)AGROUP_LIBTYPEID;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Type
//
//  DESCRIPTION:  This function returns the group type id.
//
////////////////////////////////////////////////////////////////////////////

ATYPEID ALMAPI OpGroupWndItem::Type
  (
  )

  {
  return AGROUP_LIBTYPEID;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Show
//
//  DESCRIPTION:  This shows all items in the group.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Show
  (
  )

  {
  if(m_fInit)
    {
    for(int i=0; i<m_nItems; i++)
      OWndShowItem(m_pItems[i].wi);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Hide
//
//  DESCRIPTION:  This hides all items in the group
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Hide
  (
  )

  {
  if(m_fInit)
    {
    for(int i=0; i<m_nItems; i++)
      OWndHideItem(m_pItems[i].wi);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Move
//
//  DESCRIPTION:  This moves all items in the group.  Items rectangles are
//                sized propotionally to the group rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Move
  (
  LPRECT  lprc
  )

  {
  I_ASSERT(m_fInit);
  if(!EqualRect(lprc, &m_rcItem))
    {
    int     i;
    double  xGrow;
    double  yGrow;
    RECT    rc, rcTemp;
    RECT    rcBounds = {INT_MAX, INT_MAX, INT_MIN, INT_MIN};

    xGrow = (m_rcItem.right  != m_rcItem.left) ? (double)(lprc->right  - lprc->left) / (double)(m_rcItem.right  - m_rcItem.left) : 1.;
    yGrow = (m_rcItem.bottom != m_rcItem.top ) ? (double)(lprc->bottom - lprc->top ) / (double)(m_rcItem.bottom - m_rcItem.top ) : 1.;

    for(i=0; i<m_nItems; i++)
      {
      rc.left   = lprc->left + (int)(xGrow * (m_pItems[i].wi->m_rcItem.left   - m_rcItem.left) + 0.5);
      rc.right  = lprc->left + (int)(xGrow * (m_pItems[i].wi->m_rcItem.right  - m_rcItem.left) + 0.5);
      rc.top    = lprc->top  + (int)(yGrow * (m_pItems[i].wi->m_rcItem.top    - m_rcItem.top)  + 0.5);
      rc.bottom = lprc->top  + (int)(yGrow * (m_pItems[i].wi->m_rcItem.bottom - m_rcItem.top)  + 0.5);

      OWndMoveItem(m_pItems[i].wi, &rc);

      MakeRealRect(&rcTemp, &rc);
      if(rcBounds.left   > rcTemp.left)   rcBounds.left   = rcTemp.left;
      if(rcBounds.top    > rcTemp.top)    rcBounds.top    = rcTemp.top;
      if(rcBounds.right  < rcTemp.right)  rcBounds.right  = rcTemp.right;
      if(rcBounds.bottom < rcTemp.bottom) rcBounds.bottom = rcTemp.bottom;
      }

    // invalidate the group rectangle
    OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
    OWndInvalidateLogicalRect(m_oiParent, &rcBounds, FALSE);

    // update the item rectangle
    CopyRect(&m_rcItem, &rcBounds);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Enable
//
//  DESCRIPTION:  This en<dis>ables all items in the group.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Enable
  (
  BOOL  fEnable
  )

  {
  I_ASSERT(m_fInit);
  for(int i=0; i<m_nItems; i++)
    m_pItems[i].wi->Enable(fEnable);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::AdjustNewRect
//
//  DESCRIPTION:  This calls the AdjustNewRect for each item and then it
//                adjusts the rect for the group.
//
////////////////////////////////////////////////////////////////////////////

long ALMAPI OpGroupWndItem::AdjustNewRect
  (
  LPRECT  lprc,
  LONG    fAdjust
  )

  {
  long    lRet = 0L;
  int     i;
  double  xGrow;
  double  yGrow;
  RECT    rc;
  RECT    rcBounds = {INT_MAX, INT_MAX, INT_MIN, INT_MIN};
  RECT    rcTemp;
  WINDOW* wnd;

  wnd = LockWindowData(m_oiParent);
  I_VERIFY_POINTER(wnd, return 0);

  xGrow = (m_rcItem.right  != m_rcItem.left) ? (double)(lprc->right  - lprc->left) / (double)(m_rcItem.right  - m_rcItem.left) : 1.;
  yGrow = (m_rcItem.bottom != m_rcItem.top ) ? (double)(lprc->bottom - lprc->top ) / (double)(m_rcItem.bottom - m_rcItem.top ) : 1.;

  I_ASSERT(m_fInit);
  for(i=0; i<m_nItems; i++)
    {
    rc.left   = lprc->left + (int)(xGrow * (m_pItems[i].wi->m_rcItem.left   - m_rcItem.left) + 0.5);
    rc.right  = lprc->left + (int)(xGrow * (m_pItems[i].wi->m_rcItem.right  - m_rcItem.left) + 0.5);
    rc.top    = lprc->top  + (int)(yGrow * (m_pItems[i].wi->m_rcItem.top    - m_rcItem.top)  + 0.5);
    rc.bottom = lprc->top  + (int)(yGrow * (m_pItems[i].wi->m_rcItem.bottom - m_rcItem.top)  + 0.5);

    wnd->WndLPtoDP((LPPOINT)&rc, 2); //Make sure we are on even pixel boundries
    wnd->WndDPtoLP((LPPOINT)&rc, 2);

    lRet |= m_pItems[i].wi->AdjustNewRect(&rc, fAdjust);
    MakeRealRect(&rcTemp, &rc);
    if(rcBounds.left   > rcTemp.left)   rcBounds.left   = rcTemp.left;
    if(rcBounds.top    > rcTemp.top)    rcBounds.top    = rcTemp.top;
    if(rcBounds.right  < rcTemp.right)  rcBounds.right  = rcTemp.right;
    if(rcBounds.bottom < rcTemp.bottom) rcBounds.bottom = rcTemp.bottom;
    }

  UnlockWindowData(m_oiParent);

  CopyRect(lprc, &rcBounds);
  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::PointInItem
//
//  DESCRIPTION:  This always returns FALSE.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OpGroupWndItem::PointInItem
  (
  LPPOINT lppt
  )

  {
  I_UNUSED(lppt);

  return FALSE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ItemInRect
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OpGroupWndItem::ItemInRect
  (
  LPRECT  lprc,
  BOOL    fReqFullEnclosure
  )

  {
  I_UNUSED(lprc);
  I_UNUSED(fReqFullEnclosure);

  return FALSE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ItemInRegion
//
//  DESCRIPTION:  This will always return FALSE.  A group does not draw. 
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OpGroupWndItem::ItemInRegion
  (
  HRGN  hrgn,
  BOOL  bDeviceUnits
  )

  {
  I_UNUSED(hrgn);
  I_UNUSED(bDeviceUnits);

  return FALSE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::DrawTrackingRect
//
//  DESCRIPTION:  This uses the default processing
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::DrawTrackingRect
  (
  HDC   hdc,
  RECT* rect
  )

  {
  OpLiteWndItem::DrawTrackingRect(hdc, rect);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::DrawGrabHandles
//
//  DESCRIPTION:  This will draw the grab handles for the group.  The group
//                will have a single set of grab handles.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  int   i;

  for (i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    localCreateGrabHandle(m_oiParent, &rcGrab, &m_rcItem, i);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::InvalidateGrabHandles
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  int   i;

  for (i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    localCreateGrabHandle(m_oiParent, &rcGrab, &m_rcItem, i);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::PointInGrabHandle
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

long ALMAPI OpGroupWndItem::PointInGrabHandle
  (
  LPPOINT   lppt,
  HCURSOR*  phcur
  )

  {
  I_UNUSED(phcur);

  RECT  rcGrab;
  int   i;
  long  lRet = OWND_POINT_IN_NONE;

  for (i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    localCreateGrabHandle(m_oiParent, &rcGrab, &m_rcItem, i);
    if (PtInRect(&rcGrab, *lppt))
      {
      lRet = (long)i;
      break;
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::GetExtraData
//
//  DESCRIPTION:  This returns a memblock filled with a list of the items
//                uids and original grow flags.
//
////////////////////////////////////////////////////////////////////////////

AMEMBLOCKID ALMAPI OpGroupWndItem::GetExtraData
  (
  )

  {
  int         i;
  AMEMBLOCKID  miExtra = AMemAllocate(m_nItems*sizeof(GWI_ITEM) + sizeof(short));
  GWI_FILE*   gwiFile = (GWI_FILE*)AMemLock(miExtra);

  gwiFile->sVer = VERSION_GROUPITEM;
  for(i=0; i<m_nItems; i++)
    {
    gwiFile->item[i].uid    = m_pItems[i].uid;
    gwiFile->item[i].grow   = m_pItems[i].grow;
    gwiFile->item[i].unused = 0;
    }
  AMemUnlock(miExtra);

  return miExtra;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ExportExtraData
//
//  DESCRIPTION:  This exports a list of owned items.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::ExportExtraData
  (
  )

  {
  int i;

  ExportInt("NumItems", 0L, m_nItems);
  for(i=0; i<m_nItems; i++)
    ExportLong("ITEM-UID", i, m_pItems[i].uid);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ImportExtraData
//
//  DESCRIPTION:  This imports a list of owned items.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::ImportExtraData
  (
  )

  {
  int i;

  if(ImportInt("NumItems", 0L, &m_nItems) == AUPSF_NOERROR)
    {
    if(m_pItems)
      AMemUnlock(m_miItems);
    AMemSetSize(m_miItems, (long)m_nItems*sizeof(GroupItems));
    m_pItems = m_nItems ? (GroupItems*)AMemLock(m_miItems) : NULL;

    for(i=0; i<m_nItems; i++)
      {
      m_pItems[i].uid = 0;
      ImportLong("ITEM-UID", i, &m_pItems[i].uid);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ParentOpened
//
//  DESCRIPTION:  This does nothing.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::ParentOpened
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ParentClosing
//
//  DESCRIPTION:  This does nothing.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::ParentClosing
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Print
//
//  DESCRIPTION:  This does nothing.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Print
  (
  HDC   hdc,
  RECT* printRect, 
  int   pclPrinter
  )

  {
  I_UNUSED(hdc);
  I_UNUSED(printRect);
  I_UNUSED(pclPrinter);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::ScreenLocationChanged
//
//  DESCRIPTION:  This does nothing.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::ScreenLocationChanged
  (
  int causedBy
  )

  {
  I_UNUSED(causedBy);
  }

#ifdef I_AW_DEBUG

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Edit
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

LONG ALMAPI OpGroupWndItem::Edit
  (
  BOOL  fEditExtra
  )

  {
  I_UNUSED(fEditExtra);

  I_ASSERT(0);

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Cut
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Cut
  (
  )

  {
  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Copy
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Copy
  (
  )

  {
  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Paste
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Paste
  (
  )

  {
  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Undo
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Undo
  (
  )

  {
  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::DataChanged
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::DataChanged
  (
  long  details,
  long  modifiers,
  long  variation
  )

  {
  I_UNUSED(details);
  I_UNUSED(modifiers);
  I_UNUSED(variation);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::AccelKeyHit
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::AccelKeyHit
  (
  )

  {
  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::GetSelection
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OpGroupWndItem::GetSelection
  (
  long* start,
  long* count
  )

  {
  I_UNUSED(start);
  I_UNUSED(count);

  I_ASSERT(0);

  return FALSE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::SetSelection
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OpGroupWndItem::SetSelection
  (
  long  start,
  long  count
  )

  {
  I_UNUSED(start);
  I_UNUSED(count)

  I_ASSERT(0);

  return FALSE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::SetDefaultItem
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::SetDefaultItem
  (
  BOOL  set
  )

  {
  I_UNUSED(set);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::MouseEntry
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::MouseEntry
  (
  BOOL  inside
  )

  {
  I_UNUSED(inside);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Activate
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

long ALMAPI OpGroupWndItem::Activate
  (
  int   causedBy,
  BOOL  mustKeep
  )

  {
  I_UNUSED(causedBy);
  I_UNUSED(mustKeep);

  I_ASSERT(0);

  return OWND_ACTIVATION_PASS;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Deactivated
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Deactivated
  (
  )

  {
  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::RemoveItemFromRegion
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::RemoveItemFromRegion
  (
  HRGN  hrgn,
  BOOL  bDeviceUnits
  )

  {
  I_UNUSED(hrgn);
  I_UNUSED(bDeviceUnits);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::Draw
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::Draw
  (
  HDC   hdc,
  RECT* rcInvalid
  )

  {
  I_UNUSED(hdc);
  I_UNUSED(rcInvalid);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::KeyDown
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::KeyDown
  (
  int   vKey,
  char  ascii,
  long  keyData
  )

  {
  I_UNUSED(vKey);
  I_UNUSED(ascii);
  I_UNUSED(keyData);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::KeyUp
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::KeyUp
  (
  int   vKey,
  char  ascii,
  long  keyData
  )

  {
  I_UNUSED(vKey);
  I_UNUSED(ascii);
  I_UNUSED(keyData);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::MouseMove
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::MouseMove
  (
  POINT*  where,
  int     keysDown
  )

  {
  I_UNUSED(where);
  I_UNUSED(keysDown);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::MouseDown
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::MouseDown
  (
  POINT*  where,
  int     keysDown,
  int     button
  )

  {
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::DoubleClicked
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::DoubleClicked
  (
  POINT*  where,
  int     keysDown,
  int     button
  )

  {
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::MouseUp
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::MouseUp
  (
  POINT*  where,
  int     keysDown,
  int     button
  )

  {
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);

  I_ASSERT(0);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpGroupWndItem::CancelMode
//
//  DESCRIPTION:  This should never be called (or it's an error).
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI OpGroupWndItem::CancelMode
  (
  )

  {
  I_ASSERT(0);
  }

#endif
////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpLayGroupWndItem Constructor
//
//  DESCRIPTION:  This function initializes a group item
//
////////////////////////////////////////////////////////////////////////////

OpLayGroupWndItem::OpLayGroupWndItem
  (
  AOBJECTID    oiSelf, 
  AOBJECTID    oiParent, 
  LPRECT      lprcItem, 
  long        status, 
  AMEMBLOCKID  miExtra
  ) : OpGroupWndItem(oiSelf, oiParent, lprcItem, status, miExtra)

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpLayGroupWndItem Destructor
//
//  DESCRIPTION:  This function cleans up a layout group item
//
////////////////////////////////////////////////////////////////////////////

OpLayGroupWndItem::~OpLayGroupWndItem
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpLayGroupWndItem::PointInItem
//
//  DESCRIPTION:  This function returns TRUE if the point is in any of the
//                items.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OpLayGroupWndItem::PointInItem
  (
  LPPOINT lppt
  )

  {
  int   i;
  BOOL  fRet = FALSE;

  I_ASSERT(m_fInit);
  for(i=m_nItems-1; (i>=0) && !fRet; i--)
    fRet = m_pItems[i].wi->PointInItem(lppt);

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       OpLayGroupWndItem::ItemInRect
//
//  DESCRIPTION:  This calls the OpLiteWndItem default functionality.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OpLayGroupWndItem::ItemInRect
  (
  LPRECT  lprc,
  BOOL    fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    fRet = ((lprc->left   <= m_rcItem.left)
                  &&
            (lprc->top    <= m_rcItem.top)
                  &&
            (lprc->right  >= m_rcItem.right)
                  &&
            (lprc->bottom >= m_rcItem.bottom));
    }
  else
    {
    int i;

    I_ASSERT(m_fInit);
    fRet = FALSE;
    for(i=m_nItems-1; (i>=0) && !fRet; i--)
      fRet = m_pItems[i].wi->ItemInRect(lprc, FALSE);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
