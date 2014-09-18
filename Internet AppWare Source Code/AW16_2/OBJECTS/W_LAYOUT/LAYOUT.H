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
// FILE:          "layout.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAYOUT_H
#define LAYOUT_H

// make sure the correct things are included ///////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef LAY_WND_H
#include "lay_wnd.h"
#endif

#ifndef LAY_SHAP_H
#include "lay_shap.h"
#endif

// LAYPREF_* ///////////////////////////////////////////////////////////////

#define LAYPREF_PRSTWNDPOSITIONS  0x0001
#define LAYPREF_PRSTTOOL          0x0002
#define LAYPREF_PRSTMODE          0x0004
#define LAYPREF_FULLINFOMODE      0x0008  // following use for new window only
#define LAYPREF_WYSIWYGMODE       0x0010
#define LAYPREF_SHOWGRID          0x0020
#define LAYPREF_SNAPTOGRID        0x0040
#define LAYPREF_REQFULLENCLOSURE  0x0080
#define LAYPREF_INCHES            0x0100
#define LAYPREF_MILLIMETERS       0x0200
#define LAYPREF_POINTS            0x0400
#define LAYPREF_UNITSMASK         (LAYPREF_INCHES | LAYPREF_MILLIMETERS | LAYPREF_POINTS)

// STATUS_* ////////////////////////////////////////////////////////////////

#define STATUS_HSCROLL            0x0001
#define STATUS_VSCROLL            0x0002
#define STATUS_SCROLLING          0x0004

#define STATUS_SCROLLMASK         (STATUS_HSCROLL | STATUS_VSCROLL)

////////////////////////////////////////////////////////////////////////////

class Layout

  {
  private:
    MEMBLOCKID    m_miWnd;            // a list of windows being edited
    short         m_nWnd;             // count of the windows being edited
    Window**      m_pWnd;             // pointer to the list of windows
    Window*       m_pWndActive;       // the current active window

    Shapes        m_shapes;           // a class to manage shape libraries

    HWND          m_hwndObjList;      // object list window handle
    HWND          m_hwndAlign;        // alignment palette window handle
    HWND          m_hwndSizes;        // sizes palette window handle

    HWND          m_hwndLayout;       // the Layout window handle
    short         m_fStatus;          // the layout window status
    WORD          m_fLayPref;         // preferences for the window layout
    POINT         m_ptDefGrid;        // the default size for the grid

    void          GetBoundsOfWindows(RECT& rcBounds, const POINT& ptViewportOrg);
    void          GetScrollRanges(int& nMinHorz, int& nMaxHorz, int& nPosHorz, int& nMinVert, int& nMaxVert, int& nPosVert);
    void          Scroll(UINT uMsg, WORD wScrollCode, int nPos);

    BOOL          HasHorzScroll()       {return (m_fStatus & STATUS_HSCROLL) ? TRUE : FALSE;}
    BOOL          HasVertScroll()       {return (m_fStatus & STATUS_VSCROLL) ? TRUE : FALSE;}
    BOOL          IsScrolling()         {return (m_fStatus & STATUS_SCROLLING) ? TRUE : FALSE;}
    void          SetHorzScroll(BOOL f) {if(f) m_fStatus |= STATUS_HSCROLL; else m_fStatus &= ~STATUS_HSCROLL;}
    void          SetVertScroll(BOOL f) {if(f) m_fStatus |= STATUS_VSCROLL; else m_fStatus &= ~STATUS_VSCROLL;}
    void          SetIsScrolling(BOOL f){if(f) m_fStatus |= STATUS_SCROLLING; else m_fStatus &= ~STATUS_SCROLLING;}

    friend LRESULT WINAPI AWLayoutWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  public:
                  Layout();
                 ~Layout();

                  Layout(const Layout&);
    void          operator=(const Layout&);

    inline HWND     WndHandle()                   {return m_hwndLayout;}
    inline Shapes*  Shapes()                      {return &m_shapes;}
    inline WORD     DefaultPreferences()          {return m_fLayPref;}
    inline short    DefaultGridWidth()            {return m_ptDefGrid.x;}
    inline short    DefaultGridHeight()           {return m_ptDefGrid.y;}
    inline Window*  ActiveWindow()                {return m_pWndActive;}
    inline BOOL     CurrUnitsInches()             {return (m_fLayPref & LAYPREF_INCHES)           ? TRUE : FALSE;}
    inline BOOL     CurrUnitsMillimeters()        {return (m_fLayPref & LAYPREF_MILLIMETERS)      ? TRUE : FALSE;}
    inline BOOL     CurrUnitsPoints()             {return (m_fLayPref & LAYPREF_POINTS)           ? TRUE : FALSE;}
    inline BOOL     PrstWndPositions()            {return (m_fLayPref & LAYPREF_PRSTWNDPOSITIONS) ? TRUE : FALSE;}
    inline BOOL     PrstTool()                    {return (m_fLayPref & LAYPREF_PRSTTOOL)         ? TRUE : FALSE;}
    inline BOOL     PrstMode()                    {return (m_fLayPref & LAYPREF_PRSTMODE)         ? TRUE : FALSE;}
    inline WORD     CurrentUnits()                {return m_fLayPref & LAYPREF_UNITSMASK;}

    inline void     SetPrstWndPositions(BOOL set) {if(set) m_fLayPref |= LAYPREF_PRSTWNDPOSITIONS; else m_fLayPref &= ~LAYPREF_PRSTWNDPOSITIONS;}
    inline void     SetPrstTool(BOOL set)         {if(set) m_fLayPref |= LAYPREF_PRSTTOOL;         else m_fLayPref &= ~LAYPREF_PRSTTOOL;}
    inline void     SetPrstMode(BOOL set)         {if(set) m_fLayPref |= LAYPREF_PRSTMODE;         else m_fLayPref &= ~LAYPREF_PRSTMODE;}
    inline void     SetFullInfoMode(BOOL set)     {if(set) m_fLayPref |= LAYPREF_FULLINFOMODE;     else m_fLayPref &= ~LAYPREF_FULLINFOMODE;}
    inline void     SetWYSIWYGMode(BOOL set)      {if(set) m_fLayPref |= LAYPREF_WYSIWYGMODE;      else m_fLayPref &= ~LAYPREF_WYSIWYGMODE;}
    inline void     SetReqFullEnclosure(BOOL set) {if(set) m_fLayPref |= LAYPREF_REQFULLENCLOSURE; else m_fLayPref &= ~LAYPREF_REQFULLENCLOSURE;}
    inline void     SetShowGrid(BOOL set)         {if(set) m_fLayPref |= LAYPREF_SHOWGRID;         else m_fLayPref &= ~LAYPREF_SHOWGRID;}
    inline void     SetSnapToGrid(BOOL set)       {if(set) m_fLayPref |= LAYPREF_SNAPTOGRID;       else m_fLayPref &= ~LAYPREF_SNAPTOGRID;}
    inline void     SetGrid(LPPOINT lppt)         {m_ptDefGrid.x = lppt->x; m_ptDefGrid.y = lppt->y;}
    inline void     SetCurrUnitsInches()          {m_fLayPref &= ~LAYPREF_UNITSMASK; m_fLayPref |= LAYPREF_INCHES;}
    inline void     SetCurrUnitsMillimeters()     {m_fLayPref &= ~LAYPREF_UNITSMASK; m_fLayPref |= LAYPREF_MILLIMETERS;}
    inline void     SetCurrUnitsPoints()          {m_fLayPref &= ~LAYPREF_UNITSMASK; m_fLayPref |= LAYPREF_POINTS;}

    void          InitPalettes();
    void          DestroyPalettes();
    void          OpenWindow(OBJECTID oiWnd);   // start editing window
    void          CloseWindow(OBJECTID oiWnd);  // stop editing window
    void          ActivateWindow(Window* pWnd);
    BOOL          Exit();
    void          EnablePalettes(BOOL fEnable);
    void          ShowAllPalettes(BOOL fShow);
    void          TogglePaletteVisibility(WPARAM wParam);
    void          InitMenuPopup(HMENU hmenuPopup, int nIndex, BOOL bSysmenu);
    void          SwitchToFrameMode(BOOL fNewShowFrames, BOOL fOldShowFrames);
    void          UpdateScrollRegion();
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LAYOUT_H
