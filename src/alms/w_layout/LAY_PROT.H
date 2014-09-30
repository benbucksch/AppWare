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
// FILE:          "lay_prot.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAY_PROT_H
#define LAY_PROT_H

// make sure the correct things are included ///////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

// forward class definitions ///////////////////////////////////////////////

class Window;

////////////////////////////////////////////////////////////////////////////

class Protocol

  {
  private:
    void            CreateGrabHandle(RECT* rcGrabHandle, const RECT* rcItem, int nGrabHandle);

  protected:
    Window*         m_pWnd;         // pointer to window class
    AOBJECTID        m_oiWnd;        // window object id
    BOOL            m_fFrames;      // flag for knowing if the items are frames

  public:
                    Protocol(Window* pWnd);
                   ~Protocol();

    inline Window*  GetWindow()     {return m_pWnd;}

    BOOL            PointInItem(OpWndItemD* wi, LPPOINT lppt);
    BOOL            ItemInRect(OpWndItemD* wi, LPRECT lprc, BOOL fReqFullEnclosure);
    long            PointInGrabHandle(OpWndItemD* wi, LPPOINT lppt, HCURSOR* hcursor);
    void            InvalidateGrabHandles(OpWndItemD* wi, BOOL fErase);
    void            DrawGrabHandles(OpWndItemD* wi, HDC hdc);
    void            DrawTrackingRect(OpWndItemD* wi, HDC hdc, LPRECT lprc);
    long            AdjustNewRect(OpWndItemD* wi, LPRECT lprc, long fAdjust);
    void            Move(OpWndItemD* wi, LPRECT lprc);
    void            InvalidateItem(OpWndItemD* wi, BOOL fErase);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LAY_PROT_H
