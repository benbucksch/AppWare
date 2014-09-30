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
// FILE:          "scrol_vi.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is the header file for the Scroll Bar visual
//                instance code.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SCROL_VI_H
#define SCROL_VI_H

// necessary include files /////////////////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

// ScrollVI class definition ///////////////////////////////////////////////

class Scroll;

class ScrollVI
  : public OpHwndWndItem

  {
  public:
                    ScrollVI(AOBJECTID oiItem, AOBJECTID oiParent, LPRECT prcItem, long lStatus);
                   ~ScrollVI();

    // various visual instance routines for Window Items.
    LONG ALMAPI     AdjustNewRect(LPRECT lprcNew, long fAdjust);
    void ALMAPI     Destroy();
    void ALMAPI     DataChanged(long lDetails, long lModifiers, long lVariation);
    void ALMAPI     ParentOpened();
    void ALMAPI     ParentClosing();
    LRESULT ALMAPI  ParentNotification(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // interface for 
    static void     InitDLL();
    static void     FreeDLL();

  private:
    Scroll*         m_pScroll;

    static WNDPROC  sm_pfnSBarWndProc;
    static LRESULT ALMCALLBACK  WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static int      sm_cxVS;
    static int      sm_cyHS;

    // other visual instance routines for scroll object.
    void            ScrollMsg(HWND hwnd, HWND hwndCtl, UINT uCode, int nPos);
    long            VIPosToSBarPos(int nPos);
    int             SBarPosToVIPos();
    int             VIMin();
    int             VIMax();
    void            UpdateScrollPosition();
    void            UpdateScrollRange();
    void            UpdateEnabledState();
  };

////////////////////////////////////////////////////////////////////////////
#endif // #ifndef SCROL_VI_H
