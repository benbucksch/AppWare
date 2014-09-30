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
// FILE:          "lay_item.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAY_ITEM_H
#define LAY_ITEM_H

// make sure the correct things are included ///////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

#ifndef O_WNDGRP_H
#include <o_wndgrp.h>
#endif

#ifndef O_WNDLAY_H
#include <o_wndlay.h>
#endif

#ifndef LAY_PROT_H
#include "lay_prot.h"
#endif

// forward class definitions ///////////////////////////////////////////////

class Window;
class Items;

// FLAG defines for items //////////////////////////////////////////////////

#define FLAG_SELECTED             0x00000001L
#define FLAG_GROUPED              0x00000100L
#define FLAG_LOCKED               0x00000200L

#define FLAG_NEWOBJECT            0x00010000L
#define FLAG_OBJECTCREATED        0x00020000L

#define FLAG_NEEDSTOBEDRAWN       0x01000000L
#define FLAG_RECTANGLECHANGED     0x02000000L

#define FLAG_ADDEDOBJECT          (FLAG_NEWOBJECT | FLAG_SELECTED)
#define FLAG_CREATEDOBJECT        (FLAG_OBJECTCREATED | FLAG_ADDEDOBJECT)
#define FLAG_DRAWSELECTION        (FLAG_SELECTED)

// WND_ITEMS structure /////////////////////////////////////////////////////

struct WND_ITEMS

  {
  long        flags;          // selection and status flags -- FLAG_XXXX
  RECT        rcTrack;        // tracking rectangle
  OBJECTID    oiObj;          // object id for newly AObjCreate'd objects
  OpWndItemD* wi;             // window item pointer
  int         nGroup;         // index to the group item
  };

// special typedefs ////////////////////////////////////////////////////////

typedef OpWndItemD* (Items::* GetItemFunc)(OpWndItemD*, BOOL);

// Items class definition //////////////////////////////////////////////////

class Items
  : public Protocol

  {
  private:
    MEMBLOCKID    m_miItems;      // the list of window items
    WND_ITEMS*    m_pItems;       // pointer to the list of window items
    int           m_nItems;       // count of window items

    HWND          m_hwndParent;   // parent window for window items

    long          m_fTrack;       // tracking flags for mouse action
    POINT         m_ptStart;      // starting point for tracking
    RECT          m_rcLast;       // last selection rect drawn
    OpWndItemD*   m_wiPrimary;    // primary item being resized
    RECT          m_rcMaxMove;    // maximum the items can be moved
    int           m_nNext;        // next number for reordering

    short         m_nSel;         // number of selected items -- not items in a group
    RECT          m_rcSel;        // selection rectangle
		long					m_uidPaletteOwner;  //UID of the window item that owns the palette

    inline void   SetItemNeedsToBeDrawn(int idx, BOOL set)  {if(set) m_pItems[idx].flags |= FLAG_NEEDSTOBEDRAWN; else m_pItems[idx].flags &= ~FLAG_NEEDSTOBEDRAWN;}
    inline void   SetItemSelected(int idx, BOOL set)        {if(set) m_pItems[idx].flags |= FLAG_SELECTED;       else m_pItems[idx].flags &= ~FLAG_SELECTED;}
    inline void   SetItemGrouped(int idx, BOOL set)         {if(set) m_pItems[idx].flags |= FLAG_GROUPED;        else m_pItems[idx].flags &= ~FLAG_GROUPED;}
    inline void   SetItemLocked(int idx, BOOL set)          {OWndLaySetItemLocked(m_pItems[idx].wi, set);}

    inline BOOL   DoesItemNeedToBeDrawn(int idx)            {return (m_pItems[idx].flags & FLAG_NEEDSTOBEDRAWN) ? TRUE : FALSE;}
    inline BOOL   IsItemSelected(int idx)                   {return (m_pItems[idx].flags & FLAG_SELECTED)       ? TRUE : FALSE;}
    inline BOOL   IsItemGrouped(int idx)                    {return (m_pItems[idx].flags & FLAG_GROUPED)        ? TRUE : FALSE;}
    inline BOOL   IsItemLocked(int idx)                     {return OWndLayIsItemLocked(m_pItems[idx].wi);}

    OpWndItemD*   GetPrevItem(OpWndItemD* wi, BOOL fSkipGroups);
    OpWndItemD*   GetNextItem(OpWndItemD* wi, BOOL fSkipGroups);
    int           SelectItem(int idxItem, BOOL fSelect);
    int           GetSelectionBounds(int* left, int* top, int* right, int* bottom);
    int           GetMinMaxSizes(int* minW, int* maxW, int* minH, int* maxH);
    void          DeleteItem(OpWndItemD* wi);
    void          DeleteGroup(OpGroupWndItem* gwi);
    #ifdef AW_I_DEBUG
    void          ResetCtlIDs();
    #endif
    void          PaintFrames();
    void          PaintItemInfo(OpWndItemD* wi, HDC hdc, int idx);
    void          PaintItems();
    void          Track(LPPOINT lppt, WORD fwKeys, BOOL fDraw, BOOL fToggle);
    void          TrackResize(HDC hdc, LPPOINT lppt, WORD fwKeys, BOOL fDraw, BOOL fToggle);
    void          TrackMove(HDC hdc, LPPOINT lppt, BOOL fDraw, BOOL fToggle);
    void          DrawTrackingRects(OpWndItemD* wi, HDC hdc, LPRECT lprc, BOOL fDraw, BOOL fToggle);
    void          TrackSelect(HDC hdc, LPPOINT lppt, BOOL fDraw, BOOL fToggle);
    void          SelectItemsInRect(LPRECT lprc);
    void          MoveItems();
    void          SetTrackingRectsEmpty();
    long          GetStdCursor(OpWndItemD* wi, LPPOINT lppt, HCURSOR* phcur);
    int           SetZOrder(OpWndItemD* wi, int nIdx);
    OpWndItemD*   RecreateItem(OpWndItemD* wi);
    void          NudgeResizeItem(OpWndItemD* wi, WORD wVKey);
    void          Nudge(WORD wVKey);
    void          TabSelect(GetItemFunc pfnGetItem);

  public:
                  Items(Window* pWnd);
                 ~Items();

    inline void   SetParentWnd(HWND hwnd)   {m_hwndParent = hwnd;}

    void          CreateAll();
    void          DestroyAll();
    void          AddNewItems(OpWndItemD** awi, OBJECTID* aoiObj, LPRECT arc, BOOL* abCreated, LONG* fAdjust, int nItems);
    void          SelectAll(BOOL fSelect);
    void          SelectionCount(int* nSel, int* nGroups, int* nObjects);
    OpWndItemD*   ItemFromUID(long uid);
    OpWndItemD*   ItemFromPoint(LPPOINT lppt, BOOL fSkipGroups);
    int           NumItems(BOOL fCountNested);
    BOOL          EditItem(LPPOINT lppt, BOOL fEditExtra);
    void          DeleteSelObjects();
    void          DeleteSelItems();
    void          Update();
    void          Revert();
    void          Paint();
    void          HScroll(WORD wScrollCode, int nPos);
    void          VScroll(WORD wScrollCode, int nPos);
    long          ParentNotified(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void          ChangeFullInfo(BOOL fNew);
    void          RenameSelectedItem();

    void          AlignLeft();
    void          AlignRight();
    void          AlignTop();
    void          AlignBottom();
    void          CenterHorz();
    void          CenterVert();
    void          CenterHorzInWnd();
    void          CenterVertInWnd();
    void          DefaultHorz();
    void          DefaultVert();
    void          GrowHorz();
    void          GrowVert();
    void          ShrinkHorz();
    void          ShrinkVert();
    void          SpaceHorz();
    void          SpaceVert();
    void          PosAndSize();
    void          Attach(LPPOINT lppt);
    void          SendToBack();
    int           BringToFront();
    void          Group();
    void          Ungroup();
    void          Lock(LPPOINT lppt);

    void          MouseMove(LPPOINT lppt, HCURSOR* phcur, WORD fwKeys);
    void          LButtonDown(LPPOINT lppt, HCURSOR* phcur, WORD fwKeys);
    void          LButtonUp(LPPOINT lppt, HCURSOR* phcur, WORD fwKeys);
    void          RButtonDown(LPPOINT lppt, HCURSOR* phcur, WORD fwKeys);
    void          KeyDown(WORD wVKey, DWORD dwKeyData);
    void          KeyUp(WORD wVKey, DWORD dwKeyData);
    BOOL          StartReorder();
    void          Reorder(LPPOINT lppt, BOOL fSendToFront);
    void          GroupStop(LPPOINT lppt);
    void          TabStop(LPPOINT lppt);
    void          Visibility(LPPOINT lppt);
    void          ViewportChanged(LPPOINT lpptOld);

    static WORD   GetKeyFlags();
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LAY_ITEM_H
