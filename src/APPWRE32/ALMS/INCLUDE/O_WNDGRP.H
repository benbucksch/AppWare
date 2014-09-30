/***************************************************************************
 *
 *  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
 *
 *  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
 *  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
 *  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
 *  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
 *  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
 *  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
 *  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
 *  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
 *  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
 *  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
 *  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
 *  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
 *
 *--------------------------------------------------------------------------
 *
 * FILE:          "o_wndgrp.h"
 *
 * AUTHOR:        Kevin J. Smart
 *
 * DESCRIPTION:   Header file for the group window item.  The group window
 *                Item is used to do certain operations as a group such as
 *                show, hide, move, adjust new rect, draw grab handles, etc.
 *                By making this a window item, the group behavior can be
 *                shown at run time (for attached items).
 *
 * CHANGES:
 *
 **************************************************************************/

#ifndef O_WNDGRP_H      /* prevent multiple includes */
#define O_WNDGRP_H

/* make sure necessary files are included */
#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

/* group library type id */
#define AGROUP_LIBTYPEID          ATYPE_MAKEID('G', 'w', 'i', ' ')

/* forward class definition */
class OpGroupWndItem;
struct GroupItems;

/* OpGroupItem method entry points */
extern "C"
  {
  int ALMAPI            	OpGwiNumItems(OpGroupWndItem *, BOOL);
  BOOL ALMAPI             OpGwiAllObjects(OpGroupWndItem *); /* check to see if all items are objects */
  void ALMAPI             OpGwiCreated(OpGroupWndItem *, OpWndItemD * *, int);
  void ALMAPI             OpGwiSetVisible(OpGroupWndItem *, BOOL, BOOL);
  OpWndItemD * ALMAPI  OpGwiGetPrevItem(OpGroupWndItem *, OpWndItemD *, BOOL);
  OpWndItemD * ALMAPI  OpGwiGetNextItem(OpGroupWndItem *, OpWndItemD *, BOOL);
  void ALMAPI             OpGwiItemRecreated(OpGroupWndItem *, OpWndItemD *, OpWndItemD *);
  short ALMAPI            OpGwiItemDeleted(OpGroupWndItem *, OpWndItemD *);
  short ALMAPI            OpGwiItemAdded(OpGroupWndItem *, OpWndItemD *);
  void ALMAPI             OpGwiUngrouped(OpGroupWndItem *);
  void ALMAPI             OpGwiLock(OpGroupWndItem *, BOOL);
  }

/* OpGroupItem class definition */
class OpGroupWndItem
  : public OpLiteWndItem
  {
  protected:
    int           	m_nItems;   /* number of items in the group */
    AMEMBLOCKID      m_miItems;  /* the item list is an array */
    GroupItems*     m_pItems;   /* pointer to the item records */
    BOOL            m_fInit;    /* flag indicating initialization status */

    void            GetBoundingRect();

    friend int ALMAPI OpGwiNumItems(OpGroupWndItem *, BOOL);
    friend BOOL ALMAPI OpGwiAllObjects(OpGroupWndItem *);
    friend void ALMAPI OpGwiCreated(OpGroupWndItem *, OpWndItemD * *, int);
    friend void ALMAPI OpGwiSetVisible(OpGroupWndItem *, BOOL, BOOL);
    friend OpWndItemD * ALMAPI OpGwiGetPrevItem(OpGroupWndItem *, OpWndItemD *, BOOL);
    friend OpWndItemD * ALMAPI OpGwiGetNextItem(OpGroupWndItem *, OpWndItemD *, BOOL);
    friend void ALMAPI OpGwiItemRecreated(OpGroupWndItem *, OpWndItemD *, OpWndItemD *);
    friend short ALMAPI OpGwiItemDeleted(OpGroupWndItem *, OpWndItemD *);
    friend short ALMAPI OpGwiItemAdded(OpGroupWndItem *, OpWndItemD *);
    friend void ALMAPI OpGwiUngrouped(OpGroupWndItem *);
    friend void ALMAPI OpGwiLock(OpGroupWndItem *, BOOL);

  public:
                    OpGroupWndItem(AOBJECTID oiSelf, AOBJECTID oiParent, LPRECT lprcItem, long status, AMEMBLOCKID miExtra);
                   ~OpGroupWndItem();

    /* public methods for the layout */
    int	           	NumItems(BOOL fCountNested);
    BOOL            AllObjects();
    void            Created(OpWndItemD * * awi=NULL, int nCount=0);
    void            SetVisible(BOOL fVisible, BOOL fFlagOnly);
    OpWndItemD * GetPrevItem(OpWndItemD * wi, BOOL fSkipGroups);
    OpWndItemD * GetNextItem(OpWndItemD * wi, BOOL fSkipGroups);
    void            ItemRecreated(OpWndItemD * wiNew, OpWndItemD * wiOld);
    short           ItemDeleted(OpWndItemD * wiOld);
    short           ItemAdded(OpWndItemD * wiNew);
    void            Ungrouped();
    void            Lock(BOOL fLock);

    /* overridden ApObjectD methods */
    virtual void       ALMAPI Destroy();
    virtual long       ALMAPI QueryProtocol(ApQueryInfo * pqi, APROTOCOLID piProt);
    virtual AOBJECTID   ALMAPI Object();
    virtual ATYPEID     ALMAPI Type();

    /* overridden OpWndItemD methods */
    virtual void       ALMAPI Show();
    virtual void       ALMAPI Hide();
    virtual void       ALMAPI Move(LPRECT lprc);
    virtual void       ALMAPI Enable(BOOL fEnable);
    virtual long       ALMAPI AdjustNewRect(LPRECT lprcNew, LONG fAdjust);
    virtual BOOL       ALMAPI PointInItem(LPPOINT lppt);
    virtual BOOL       ALMAPI ItemInRect(LPRECT lprc, BOOL fReqFullEnclosure);
    virtual BOOL       ALMAPI ItemInRegion(HRGN hrgn, BOOL bDeviceUnits);
    virtual void       ALMAPI DrawTrackingRect(HDC hdc, LPRECT rect);
    virtual void       ALMAPI DrawGrabHandles(HDC hdc);
    virtual void       ALMAPI InvalidateGrabHandles(BOOL fErase);
    virtual long       ALMAPI PointInGrabHandle(LPPOINT lppt, HCURSOR * phcur);
    virtual AMEMBLOCKID ALMAPI GetExtraData();
    virtual void       ALMAPI ExportExtraData();
    virtual void       ALMAPI ImportExtraData();

    /* methods that will do nothing */
    virtual void       ALMAPI ParentOpened();
    virtual void       ALMAPI ParentClosing();
    virtual void       ALMAPI Print(HDC hdc, LPRECT printRect, int pclPrinter);
    virtual void       ALMAPI ScreenLocationChanged(int causedBy);

    #ifdef I_AW_DEBUG
    /* methods that should never be called */
    virtual LONG       ALMAPI Edit(BOOL fEditExtra);
    virtual void       ALMAPI Cut();
    virtual void       ALMAPI Copy();
    virtual void       ALMAPI Paste();
    virtual void       ALMAPI Undo();
    virtual void       ALMAPI DataChanged(long details, long modifiers, long variation);
    virtual void       ALMAPI AccelKeyHit();
    virtual BOOL       ALMAPI GetSelection(long * start, long * count);
    virtual BOOL       ALMAPI SetSelection(long start, long count);
    virtual void       ALMAPI SetDefaultItem(BOOL set);
    virtual void       ALMAPI MouseEntry(BOOL inside);
    virtual long       ALMAPI Activate(int causedBy, BOOL mustKeep);
    virtual void       ALMAPI Deactivated();
    virtual void       ALMAPI RemoveItemFromRegion(HRGN hrgn, BOOL bDeviceUnits);
    virtual void       ALMAPI Draw(HDC hdc, LPRECT rcInvalid);
    virtual void       ALMAPI KeyDown(int vKey, char ascii, long keyData);
    virtual void       ALMAPI KeyUp(int vKey, char ascii, long keyData);
    virtual void       ALMAPI MouseMove(LPPOINT where, int keysDown);
    virtual void       ALMAPI MouseDown(LPPOINT where, int keysDown, int button);
    virtual void       ALMAPI DoubleClicked(LPPOINT where, int keysDown, int button);
    virtual void       ALMAPI MouseUp(LPPOINT where, int keysDown, int button);
    virtual void       ALMAPI CancelMode();
    #endif
  };


inline int OpGroupWndItem::NumItems
  (
  BOOL  fCountNested
  )
  
  {
  return OpGwiNumItems(this, fCountNested);
  }

inline BOOL OpGroupWndItem::AllObjects
  (
  )
  
  {
  return OpGwiAllObjects(this);
  }

inline void OpGroupWndItem::Created
  (
  OpWndItemD * *  	awi,
  int               nCount
  )
  
  {
  OpGwiCreated(this, awi, nCount);
  }

inline void OpGroupWndItem::SetVisible
  (
  BOOL  fVisible,
  BOOL  fFlagOnly
  )
  
  {
  OpGwiSetVisible(this, fVisible, fFlagOnly);
  }

inline OpWndItemD * OpGroupWndItem::GetPrevItem
  (
  OpWndItemD * wi,
  BOOL            fSkipGroups
  )

  {
  return OpGwiGetPrevItem(this, wi, fSkipGroups);
  }

inline OpWndItemD * OpGroupWndItem::GetNextItem
  (
  OpWndItemD * wi,
  BOOL            fSkipGroups
  )

  {
  return OpGwiGetNextItem(this, wi, fSkipGroups);
  }

inline void OpGroupWndItem::ItemRecreated
  (
  OpWndItemD * wiNew,
  OpWndItemD * wiOld
  )

  {
  OpGwiItemRecreated(this, wiNew, wiOld);
  }

inline short OpGroupWndItem::ItemDeleted
  (
  OpWndItemD * wiOld
  )

  {
  return OpGwiItemDeleted(this, wiOld);
  }

inline short OpGroupWndItem::ItemAdded
  (
  OpWndItemD * wiNew
  )

  {
  return OpGwiItemAdded(this, wiNew);
  }

inline void OpGroupWndItem::Ungrouped
  (
  )

  {
  OpGwiUngrouped(this);
  }

inline void OpGroupWndItem::Lock
  (
  BOOL  fLock
  )

  {
  OpGwiLock(this, fLock);
  }
#endif  /* !O_WNDGRP_H */
