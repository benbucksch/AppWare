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
// FILE:          "lay_wnd.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAY_WND_H
#define LAY_WND_H

// make sure the correct things are included ///////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef LAY_WDAT_H
#include "lay_wdat.h"
#endif

#ifndef LAY_ITEM_H
#include "lay_item.h"
#endif

// forward class definition ////////////////////////////////////////////////

class Layout;

// defines specific to the window object ///////////////////////////////////

#define WNDPREF_FULLINFOMODE      0x0001
#define WNDPREF_WYSIWYGMODE       0x0002
#define WNDPREF_PRSTMODE          0x0004
#define WNDPREF_SHOWGRID          0x0008
#define WNDPREF_SNAPTOGRID        0x0010
#define WNDPREF_REQFULLENCLOSURE  0x0020

// WNDLAY_DATA struct //////////////////////////////////////////////////////

struct WNDLAY_DATA

  {
  long  lSignature;               // signature for the window layout data
  short sVer;                     // version stamp for the Novell layout
  
  short sPref;                    // window preferences -- see WNDPREF_*
  short sGridX;                   // the grid for the window
  short sGridY;                   // the grid for the window

  // layering stuff
  };

// Window class definition /////////////////////////////////////////////////

class Window
  : public WndData

  {
  private:
    Layout*       m_layout;       // pointer to the parent layout
    WNDLAY_DATA*  m_pwndlaydata;  // layout specific data
    Items         m_items;

    HWND          m_hwndWindow;   // handle to the window object window
    HWND          m_hwndMenu;     // handle to the menu window
    HWND          m_hwndClient;   // handle to the client window

    BOOL          m_fFullInfo;    // flag for full info mode

    POINT         m_ptNext;       // next location for double-click added item

    // private methods
    void          MoveClient();
    void          MoveMenu();
    short         DefaultPreferences();
    void          CreateMenuWindow();
    void          CreateClientWindow();
    void          FixWindowStyle();

  public:
                      Window(AOBJECTID oiWnd, Layout* layout);
                     ~Window();

    inline AOBJECTID   ObjectID()                    {return m_oiWnd;}
    inline HWND       WndHandle()                   {return m_hwndWindow;}
    inline HWND       ClientWnd()                   {return m_hwndClient;}
    inline Layout*    GetLayout()                   {return m_layout;}
    inline Items*     Items()                       {return &m_items;}
    inline short      NumSavedChanges()             {return m_nChanged;}
    inline void       GetGrid(LPPOINT lppt)         {lppt->x = m_pwndlaydata->sGridX; lppt->y = m_pwndlaydata->sGridY;}
    inline void       SetGrid(LPPOINT lppt)         {m_pwndlaydata->sGridX = (short)lppt->x; m_pwndlaydata->sGridY = (short)lppt->y;}
    inline BOOL       PrstMode()                    {return (m_pwndlaydata->sPref & WNDPREF_PRSTMODE)         ? TRUE  : FALSE;}
    inline BOOL       DefFullInfoMode()             {return (m_pwndlaydata->sPref & WNDPREF_FULLINFOMODE)     ? TRUE  : FALSE;}
    inline BOOL       DefWYSIWYGMode()              {return (m_pwndlaydata->sPref & WNDPREF_FULLINFOMODE)     ? FALSE : TRUE;}
    inline BOOL       ShowGrid()                    {return (m_pwndlaydata->sPref & WNDPREF_SHOWGRID)         ? TRUE  : FALSE;}
    inline BOOL       SnapToGrid()                  {return (m_pwndlaydata->sPref & WNDPREF_SNAPTOGRID)       ? TRUE  : FALSE;}
    inline BOOL       ReqFullEnclosure()            {return (m_pwndlaydata->sPref & WNDPREF_REQFULLENCLOSURE) ? TRUE  : FALSE;}
    inline void       SetPrstMode(BOOL set)         {if(set) m_pwndlaydata->sPref |= WNDPREF_PRSTMODE;         else m_pwndlaydata->sPref &= ~WNDPREF_PRSTMODE;}
    inline void       SetFullInfoMode(BOOL set)     {if(set) m_pwndlaydata->sPref |= WNDPREF_FULLINFOMODE;     else m_pwndlaydata->sPref &= ~WNDPREF_FULLINFOMODE;}
    inline void       SetWYSIWYGMode(BOOL set)      {if(set) m_pwndlaydata->sPref |= WNDPREF_WYSIWYGMODE;      else m_pwndlaydata->sPref &= ~WNDPREF_WYSIWYGMODE;}
    inline void       SetShowGrid(BOOL set)         {if(set) m_pwndlaydata->sPref |= WNDPREF_SHOWGRID;         else m_pwndlaydata->sPref &= ~WNDPREF_SHOWGRID;}
    inline void       SetSnapToGrid(BOOL set)       {if(set) m_pwndlaydata->sPref |= WNDPREF_SNAPTOGRID;       else m_pwndlaydata->sPref &= ~WNDPREF_SNAPTOGRID;}
    inline void       SetReqFullEnclosure(BOOL set) {if(set) m_pwndlaydata->sPref |= WNDPREF_REQFULLENCLOSURE; else m_pwndlaydata->sPref &= ~WNDPREF_REQFULLENCLOSURE;}

    inline BOOL       FullInfoMode()                {return m_fFullInfo;}
    inline BOOL       WYSIWYGMode()                 {return !m_fFullInfo;}
    inline void       SetDirtyFlag()                {m_fDirty = TRUE;}
    inline LPCSTR     Title()                       {return m_szTitle;}

    void              Open();
    void              Close();
    BOOL              Dirty();
    void              Activate(BOOL fActivate)      {;}
    void              FrameWndCreated(HWND hwnd);
    void              SnapPointToGrid(LPPOINT lppt);
    void              Update();
    void              Revert();
    void              EditAttributes();
    void              EditBackground();
    void              EditGrid();
    void              EditSize();
    void              EditPreferences();
    void              ToggleFullInfo();
    BOOL              DrawFrameMode();
    BOOL              DrawGroupsMode();
    void              DrawGrid(HDC hdc, LPRECT lprc);
    void              SwitchToFrameMode(BOOL fNewShowFrames, BOOL fOldShowFrames);
    POINT             GetNextAddPoint();

    // window message processing
    void              PaintMenuWindow(HWND hwnd);
    void              EraseBackground(HDC hdc);
    void              Size(HWND hwnd, UINT uState, int nWidth, int nHeight);
    void              Move(HWND hwnd, int xPos, int yPos);
    void              GetMinMaxInfo(MINMAXINFO* lpmmi);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LAY_WND_H
