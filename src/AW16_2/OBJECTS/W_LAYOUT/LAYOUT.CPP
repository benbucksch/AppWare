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
// FILE:          "layout.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <windows.h>
#include <bwcc.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_wndshp.h>

#include "wlay.h"
#include "lay_algn.h"
#include "lay_wnd.h"
#include "lay_shap.h"
#include "layout.h"
#include "rc\wlay_rsc.h"
#include "objlist.h"

// constants ///////////////////////////////////////////////////////////////

extern const char*  c_szAppBuilderIni;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Constructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

Layout::Layout
  (
  ) : m_shapes(this)

  {
  m_miWnd = AMemAllocate(0);
  m_nWnd = 0;
  m_pWnd = NULL;
  m_pWndActive = NULL;

  m_hwndObjList = NULL;
  m_hwndAlign = NULL;

  m_hwndLayout = NULL;
  m_fStatus = 0;

  // get the layout preferences from the 'appbldr.ini' file
  m_fLayPref = GetPrivateProfileInt("Layout", "Preferences",
                  (int)(LAYPREF_REQFULLENCLOSURE | LAYPREF_PRSTWNDPOSITIONS |
                        LAYPREF_INCHES),
                  c_szAppBuilderIni);

  m_ptDefGrid.x = GetPrivateProfileInt("Layout", "Grid Width", 0, c_szAppBuilderIni);
  m_ptDefGrid.y = GetPrivateProfileInt("Layout", "Grid Height", 0, c_szAppBuilderIni);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Destructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

Layout::~Layout
  (
  )

  {
  char  szBuffer[80];

  if(m_pWnd)
    AMemUnlock(m_miWnd);
  AMemFree(m_miWnd);

  // save the layout preferences to the 'appbldr.ini' file
  wsprintf(szBuffer, "%hi", m_fLayPref);
  WritePrivateProfileString("Layout", "Preferences", szBuffer, c_szAppBuilderIni);
  wsprintf(szBuffer, "%i", m_ptDefGrid.x);
  WritePrivateProfileString("Layout", "Grid Width", szBuffer, c_szAppBuilderIni);
  wsprintf(szBuffer, "%i", m_ptDefGrid.y);
  WritePrivateProfileString("Layout", "Grid Height", szBuffer, c_szAppBuilderIni);
  if(m_fLayPref & LAYPREF_PRSTTOOL)
    {
    TYPEID  tiTool = m_shapes.CurrentLib();
    short   nTool = (short)m_shapes.CurrentTool();

    wsprintf(szBuffer, "%li, %hi", (long)tiTool, nTool);
    WritePrivateProfileString("Layout", "Tool", szBuffer, c_szAppBuilderIni);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::InitPalettes
//
//  DESCRIPTION:  This method will create and open all the palettes
//
////////////////////////////////////////////////////////////////////////////

void Layout::InitPalettes
  (
  )

  {
  m_hwndAlign = InitAlignmentPalette(m_hwndLayout, this, PrstWndPositions());
  m_hwndObjList = InitObjectListPalette(m_hwndLayout, this, PrstWndPositions());

  m_shapes.InitPalettes();

  // change the tool if we have persistent tools.
  if(m_fLayPref & LAYPREF_PRSTTOOL)
    {
    char    szBuffer[80];
    char    szDefault[80];
    char*   szNext;
    TYPEID  tiShape;
    short   nTool;

    wsprintf(szDefault, "%li, %hi", 0L, OSHP_STDTOOL_ARROW);
    GetPrivateProfileString("Layout", "Tool", szDefault, szBuffer, 80, c_szAppBuilderIni);

    tiShape = (TYPEID)strtol(szBuffer, &szNext, 0);
    nTool = (short)strtol(++szNext, &szNext, 0);
    m_shapes.ChangeTool(tiShape, nTool);
    }    
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::DestroyPalettes
//
//  DESCRIPTION:  This method will destroy all the palettes
//
////////////////////////////////////////////////////////////////////////////

void Layout::DestroyPalettes
  (
  )

  {
  m_shapes.DestroyPalettes(PrstWndPositions());
  DestroyAlignmentPalette(PrstWndPositions());
  DestroyObjectListWindow(PrstWndPositions());

  m_hwndAlign = NULL;
  m_hwndObjList = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::OpenWindow
//
//  DESCRIPTION:  This method will open a window object for editing.
//
////////////////////////////////////////////////////////////////////////////

void Layout::OpenWindow
  (
  OBJECTID  oiWnd
  )

  {
  #ifdef AW_I_DEBUG
  // verify that the window is not already open
  int i;
  for(i=0; i<m_nWnd; i++)
    I_ASSERT(oiWnd != m_pWnd[i]->ObjectID());
  #endif

  if(m_pWnd)
    AMemUnlock(m_miWnd);

  m_nWnd++;
  AMemSetSize(m_miWnd, m_nWnd*sizeof(Window*));
  m_pWnd = (Window**)AMemLock(m_miWnd);
  I_ASSERT(m_pWnd);

  m_pWnd[m_nWnd-1] = new Window(oiWnd, this);
  I_ASSERT(m_pWnd[m_nWnd-1]);

  m_pWndActive = m_pWnd[m_nWnd-1];
  m_pWndActive->Open();

  // adjust scroll bars as necessary
  UpdateScrollRegion();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::CloseWindow
//
//  DESCRIPTION:  This method will close a window object to stop editing.
//
////////////////////////////////////////////////////////////////////////////

void Layout::CloseWindow
  (
  OBJECTID  oiWnd
  )

  {
  int   i = 0;

  for(i=0; i<m_nWnd; i++)
    {
    if(oiWnd==m_pWnd[i]->ObjectID())
      {
      int j;

      m_pWnd[i]->Close();
      if(m_pWnd[i]==m_pWndActive)
        {
        if(i<(m_nWnd-1))
          m_pWndActive = m_pWnd[i+1];
        else if(i>0)
          m_pWndActive = m_pWnd[0];
        else
          m_pWndActive = NULL;
        }

      // delete the memory and fix the list
      delete m_pWnd[i];
      m_nWnd--;
      for(j=i; j<m_nWnd; j++)
        m_pWnd[j] = m_pWnd[j+1];
      AMemUnlock(m_miWnd);
      AMemSetSize(m_miWnd, m_nWnd*sizeof(Window*));      
      if(m_nWnd>0)
        m_pWnd = (Window**)AMemLock(m_miWnd);

      // adjust scroll bars as necessary
      UpdateScrollRegion();

      // terminate the for loop
      break;
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::ActivateWindow
//
//  DESCRIPTION:  This method will activate the specified window.
//
////////////////////////////////////////////////////////////////////////////

void Layout::ActivateWindow
  (
  Window* pWnd
  )

  {
  if(pWnd != m_pWndActive)
    {
    for(int i=0; i<m_nWnd; i++)
      {
      if(pWnd==m_pWnd[i])
        {
        m_pWndActive->Activate(FALSE);
        m_pWndActive = pWnd;
        m_pWndActive->Activate(TRUE);
        break;
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::Exit
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

BOOL Layout::Exit
  (
  )

  {
  int   i;
  char  szBuffer[255];
  BOOL  fExit     = TRUE;
  int   nMBResult;

  for(i=0; (i<m_nWnd) && fExit; i++)
    {
    if(m_pWnd[i]->Dirty())
      {
      MessageBeep(MB_OK);
      wsprintf(szBuffer, "The window object \"%s\" has changed.\r\n\r\nSave changes before closing?", m_pWnd[i]->Title());
      nMBResult = BWCCMessageBox(m_hwndLayout, szBuffer, "Window Layout", MB_YESNOCANCEL | MB_ICONQUESTION);

      switch(nMBResult)
        {
        case IDYES:
          m_pWnd[i]->Update();
          break;

        case IDNO:
          break;

        case IDCANCEL:
        default:
          fExit = FALSE;
          break;
        }
      }

    if(fExit)
      {
      short nChanged = 0;

      for(i=0; i<m_nWnd; i++)
        {
        nChanged += m_pWnd[i]->NumSavedChanges();
        CloseWindow(m_pWnd[i]->ObjectID());
        }

      // close all the palettes before the layout window closes
      DestroyPalettes();

      // save the current layout window position
      if(PrstWndPositions())
        {
        RECT    rc;
        double  left, top, right, bottom;
        BOOL    fMax;

        if((fMax = IsZoomed(m_hwndLayout)) == TRUE)
          ShowWindow(m_hwndLayout, SW_RESTORE);
        GetWindowRect(m_hwndLayout, &rc);

        left   = 100. * (double)rc.left   / (double)GetSystemMetrics(SM_CXSCREEN);
        top    = 100. * (double)rc.top    / (double)GetSystemMetrics(SM_CYSCREEN);
        right  = 100. * (double)rc.right  / (double)GetSystemMetrics(SM_CXSCREEN);
        bottom = 100. * (double)rc.bottom / (double)GetSystemMetrics(SM_CYSCREEN);
        sprintf(szBuffer, "%.3lf, %.3lf, %.3lf, %.3lf, %i", left, top, right, bottom, (int)fMax);
        WritePrivateProfileString("Layout", "Startup Position", szBuffer, c_szAppBuilderIni);
        }

      // send the message to close the layout window
      PostMessage(m_hwndLayout, WM_ENDLAYOUT, (WPARAM)nChanged, 0L);
      }
    }

  return fExit;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::EnablePalettes
//
//  DESCRIPTION:  This method will enable or disable all the owned palettes.
//
////////////////////////////////////////////////////////////////////////////

void Layout::EnablePalettes
  (
  BOOL  fEnable
  )

  {
  EnableWindow(m_hwndObjList, fEnable);
  EnableWindow(m_hwndAlign, fEnable);
  m_shapes.EnablePalettes(fEnable);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::ShowAllPalettes
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Layout::ShowAllPalettes
  (
  BOOL  fShow
  )

  {
  ShowWindow(m_hwndAlign, fShow ? SW_SHOW : SW_HIDE);
  ShowWindow(m_hwndObjList, fShow ? SW_SHOW : SW_HIDE);

  m_shapes.ShowAllPalettes(fShow);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::TogglePaletteVisibility
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Layout::TogglePaletteVisibility
  (
  WPARAM  wParam
  )

  {
  switch(wParam)
    {
    case IDM_ALIGNMENT:
      ShowWindow(m_hwndAlign, IsWindowVisible(m_hwndAlign) ? SW_HIDE : SW_SHOW);
      break;

    case IDM_OBJECTLIST:
      ShowWindow(m_hwndObjList, IsWindowVisible(m_hwndObjList) ? SW_HIDE : SW_SHOW);
      break;

    case IDM_NAME:
    case IDM_SIZES:
    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::InitMenuPopup
//
//  DESCRIPTION:  This method will called to disable the correct menus when
//                the menus are popped up.
//
////////////////////////////////////////////////////////////////////////////

void Layout::InitMenuPopup
  (
  HMENU hmenuPopup,
  int   nIndex,
  BOOL  fSysMenu
  )

  {
  I_UNUSED(fSysMenu);

  int nTool;
  int nItems; // a group is a single item
  int nSel;   // number of items selected
  int nGroup; // number of groups selected
  int nObj;   // number of selected objects

  nTool = m_shapes.CurrentTool();
  nItems = m_pWndActive ? (m_pWndActive->Items())->NumItems(FALSE) : 0;
  if(m_pWndActive)
    (m_pWndActive->Items())->SelectionCount(&nSel, &nGroup, &nObj);
  else
    nSel = nGroup = nObj = 0;

  switch(nIndex)
    {
    case MENU_FILE:
      break;

    case MENU_EDIT:
      EnableMenuItem(hmenuPopup, IDM_UNDO,           MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_CUT,            MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_COPY,           MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_PASTE,          MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_DELETE,         MF_BYCOMMAND | (nSel>0)  ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_DUPLICATE,      MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_DELETEOBJECT,   MF_BYCOMMAND | ((nObj==nSel) && (nSel>0))                ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_OBJECTNAME,     MF_BYCOMMAND | ((nObj==1) && (nSel==1) && (nGroup==0))   ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_EDITITEM,       MF_BYCOMMAND | ((nSel==1) && (nGroup==0))                ? MF_ENABLED : MF_GRAYED);
      break;

    case MENU_POSITION:
      EnableMenuItem(hmenuPopup, IDM_ALIGNLEFT,       MF_BYCOMMAND | (nSel>1) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_ALIGNTOP,        MF_BYCOMMAND | (nSel>1) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_ALIGNRIGHT,      MF_BYCOMMAND | (nSel>1) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_ALIGNBOTTOM,     MF_BYCOMMAND | (nSel>1) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_CENTERHORZINWND, MF_BYCOMMAND | (nSel>0) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_CENTERVERTINWND, MF_BYCOMMAND | (nSel>0) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_CENTERHORZ,      MF_BYCOMMAND | (nSel>1) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_CENTERVERT,      MF_BYCOMMAND | (nSel>1) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_SPACEHORZ,       MF_BYCOMMAND | (nSel>2) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_SPACEVERT,       MF_BYCOMMAND | (nSel>2) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_SIZE,            MF_BYCOMMAND | (nSel>0) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_ATTACH,          MF_BYCOMMAND | (nSel>0) ? MF_ENABLED : MF_GRAYED);
      break;

    case MENU_ARRANGE:
      EnableMenuItem(hmenuPopup, IDM_SENDTOBACK,   MF_BYCOMMAND | ((nSel<nItems) && (nSel>0)) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_BRINGTOFRONT, MF_BYCOMMAND | ((nSel<nItems) && (nSel>0)) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_GROUP,        MF_BYCOMMAND | (nSel>1) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_UNGROUP,      MF_BYCOMMAND | ((nSel==1) && (nGroup==1)) ? MF_ENABLED : MF_GRAYED);
      break;

    case MENU_TOOLS:
      CheckMenuItem(hmenuPopup, IDM_FULLINFO,   MF_BYCOMMAND | (m_pWndActive->FullInfoMode()   ? MF_CHECKED : MF_UNCHECKED));
      CheckMenuItem(hmenuPopup, IDM_SELECTION,  MF_BYCOMMAND | (nTool==OSHP_STDTOOL_ARROW      ? MF_CHECKED : MF_UNCHECKED));
      CheckMenuItem(hmenuPopup, IDM_REORDER,    MF_BYCOMMAND | (nTool==OSHP_STDTOOL_REORDER    ? MF_CHECKED : MF_UNCHECKED));
      CheckMenuItem(hmenuPopup, IDM_TABSTOPS,   MF_BYCOMMAND | (nTool==OSHP_STDTOOL_TABSTOP    ? MF_CHECKED : MF_UNCHECKED));
      CheckMenuItem(hmenuPopup, IDM_GROUPSTOPS, MF_BYCOMMAND | (nTool==OSHP_STDTOOL_GROUPSTOP  ? MF_CHECKED : MF_UNCHECKED));
      CheckMenuItem(hmenuPopup, IDM_VISIBILITY, MF_BYCOMMAND | (nTool==OSHP_STDTOOL_VISIBILITY ? MF_CHECKED : MF_UNCHECKED));
      CheckMenuItem(hmenuPopup, IDM_LOCK,       MF_BYCOMMAND | (nTool==OSHP_STDTOOL_LOCK       ? MF_CHECKED : MF_UNCHECKED));
      CheckMenuItem(hmenuPopup, IDM_ANCHOR,     MF_BYCOMMAND | (nTool==OSHP_STDTOOL_ANCHOR     ? MF_CHECKED : MF_UNCHECKED));
      break;

    case MENU_PALETTES:
      InitPaletteMenuString(IsWindowVisible(m_hwndAlign), hmenuPopup, IDM_ALIGNMENT, "&Alignment");
//      InitPaletteMenuString(IsWindowVisible(m_hwndName), hmenuPopup, IDM_NAME, "&Name");
      EnableMenuItem(hmenuPopup, IDM_NAME, MF_BYCOMMAND | MF_GRAYED);
      InitPaletteMenuString(IsWindowVisible(m_hwndObjList), hmenuPopup, IDM_OBJECTLIST, "&Object List");
      m_shapes.InitPaletteMenu(hmenuPopup);
      break;

    case MENU_HELP:
      EnableMenuItem(hmenuPopup, IDM_HELPINDEX,       MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_HELPUSINGLAYOUT, MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(hmenuPopup, IDM_HELPABOUT,       MF_BYCOMMAND | MF_GRAYED);
      break;

    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::SwitchToFrameMode
//
//  DESCRIPTION:  This is called when there are tool changes.  This is
//                broadcast to all open window objects.
//
////////////////////////////////////////////////////////////////////////////

void Layout::SwitchToFrameMode
  (
  BOOL  fNewShowFrames,
  BOOL  fOldShowFrames
  )

  {
  int i;

  for(i=0; i<m_nWnd; i++)
    m_pWnd[i]->SwitchToFrameMode(fNewShowFrames, fOldShowFrames);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::GetBoundsOfWindows (private)
//
//  DESCRIPTION:  This is called to get the bounding rect of all child
//                windows in the Layout.
//
////////////////////////////////////////////////////////////////////////////

void Layout::GetBoundsOfWindows
  (
  RECT&         rcBounds,
  const POINT&  ptViewportOrg
  )

  {
  int   i;
  RECT  rcWnd;

  // give the worst case numbers to make sure the loop works correctly
  SetRect(&rcBounds, INT_MAX, INT_MAX, INT_MIN, INT_MIN);

  // get the bounding rectangle of all windows inside the layout
  for(i=0; i<m_nWnd; i++)
    {
    // get the window position in logical coordinates
    GetWindowRect(m_pWnd[i]->WndHandle(), &rcWnd);
    MapWindowPoints(HWND_DESKTOP, m_hwndLayout, (LPPOINT)&rcWnd, 2);
    rcWnd.left   -= ptViewportOrg.x;
    rcWnd.right  -= ptViewportOrg.x;
    rcWnd.top    -= ptViewportOrg.y;
    rcWnd.bottom -= ptViewportOrg.y;

    // update the bounding rectangle
    if(rcWnd.left   < rcBounds.left)
      rcBounds.left   = rcWnd.left;
    if(rcWnd.top    < rcBounds.top)
      rcBounds.top    = rcWnd.top;
    if(rcWnd.right  > rcBounds.right)
      rcBounds.right  = rcWnd.right;
    if(rcWnd.bottom > rcBounds.bottom)
      rcBounds.bottom = rcWnd.bottom;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::GetScrollRanges
//
//  DESCRIPTION:  This is called to get the necessary scroll ranges for the
//                layout window.
//
////////////////////////////////////////////////////////////////////////////

void Layout::GetScrollRanges
  (
  int&  nMinHorz,
  int&  nMaxHorz,
  int&  nPosHorz,
  int&  nMinVert,
  int&  nMaxVert,
  int&  nPosVert
  )

  {
  static int  nSBWidth  = GetSystemMetrics(SM_CXVSCROLL) - 1;
  static int  nSBHeight = GetSystemMetrics(SM_CYHSCROLL) - 1;

  RECT        rcBounds;
  RECT        rcLayClient;
  HDC         hdc;
  POINT       ptViewportOrg;
  BOOL        fHorz             = FALSE;
  BOOL        fVert             = FALSE;

  // get the current viewport origin -- adjust all rectangles by this value
  hdc = GetDC(m_hwndLayout);
  GetViewportOrgEx(hdc, &ptViewportOrg);
  ReleaseDC(m_hwndLayout, hdc);

  // get the bounding rectangle of all windows inside the layout
  GetBoundsOfWindows(rcBounds, ptViewportOrg);

  // get the client window size of the layout window (ignore the scroll bars)
  GetClientRect(m_hwndLayout, &rcLayClient);

  if(HasHorzScroll())
    rcLayClient.bottom += nSBHeight;
  if(HasVertScroll())
    rcLayClient.right  += nSBWidth;

  // determine if the horizontal scroll bar needs to be added
  if((ptViewportOrg.x != 0)
          ||
     (rcBounds.left   < rcLayClient.left)
          ||
     (rcBounds.right  > rcLayClient.right))
    {
    fHorz = TRUE;
    rcLayClient.bottom -= nSBHeight;
    }

  // determine if the vertical scroll bar should be added
  if((ptViewportOrg.y != 0)
          ||
     (rcBounds.top    < rcLayClient.top)
          ||
     (rcBounds.bottom > rcLayClient.bottom))
    {
    fVert = TRUE;
    rcLayClient.right -= nSBWidth;

    // Check to see if the horizontal scroll bar needs to be added, because
    // the layout window's client width has changed.
    if(!fHorz && (rcBounds.right  > rcLayClient.right))
      {
      fHorz = TRUE;
      rcLayClient.bottom -= nSBHeight;
      }
    }

  if(fHorz)
    {
    nMinHorz = nMin(rcBounds.left, nMin(0, -ptViewportOrg.x));
    nMaxHorz = nMax(-ptViewportOrg.x, nMax(rcLayClient.right, rcBounds.right) - rcLayClient.right);
    nPosHorz = -ptViewportOrg.x;
    }
  else
    nPosHorz = nMinHorz = nMaxHorz = 0;

  if(fVert)
    {
    nMinVert = nMin(rcBounds.top, nMin(0, -ptViewportOrg.y));
    nMaxVert = nMax(-ptViewportOrg.y, nMax(rcLayClient.bottom, rcBounds.bottom) - rcLayClient.bottom);
    nPosVert = -ptViewportOrg.y;
    }
  else
    nPosVert = nMinVert = nMaxVert = 0;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::UpdateScrollRegion
//
//  DESCRIPTION:  This is called when the required scroll size changes due
//                to a window changing size or moving, or the layout being
//                resized.  This function updates the scroll bar positions,
//                ranges, and visibiltiy.
//
////////////////////////////////////////////////////////////////////////////

void Layout::UpdateScrollRegion
  (
  )

  {
  if(!IsScrolling())
    {
    int nMinHorz;
    int nMaxHorz;
    int nPosHorz;
    int nMinVert;
    int nMaxVert;
    int nPosVert;

    // get the scroll ranges for the layout
    GetScrollRanges(nMinHorz, nMaxHorz, nPosHorz, nMinVert, nMaxVert, nPosVert);

    // set the horizontal scroll bar visibility, range and status
    SetHorzScroll(nMinHorz != nMaxHorz);
    ShowScrollBar(m_hwndLayout, SB_HORZ, nMinHorz != nMaxHorz);
    SetScrollRange(m_hwndLayout, SB_HORZ, nMinHorz, nMaxHorz, FALSE);
    SetScrollPos(m_hwndLayout, SB_HORZ, nPosHorz, TRUE);

    // set the vertical scroll bar visibility, range and status
    SetVertScroll(nMinVert != nMaxVert);
    ShowScrollBar(m_hwndLayout, SB_VERT, nMinVert != nMaxVert);
    SetScrollRange(m_hwndLayout, SB_VERT, nMinVert, nMaxVert, FALSE);
    SetScrollPos(m_hwndLayout, SB_VERT, nPosVert, TRUE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Layout::Scroll (private)
//
//  DESCRIPTION:  This is called when a scroll bar is used.
//
////////////////////////////////////////////////////////////////////////////

void Layout::Scroll
  (
  UINT  uMsg,
  WORD  wScrollCode,
  int   nPos
  )

  {
  int   fnBar         = (uMsg==WM_HSCROLL) ? SB_HORZ : SB_VERT;
  int   x             = GetScrollPos(m_hwndLayout, fnBar);
  int   extent;
  int   n_min, n_max;
  int   nLine, nPage;
  RECT  rcClient;

  GetClientRect(m_hwndLayout, &rcClient);
  extent = (uMsg==WM_HSCROLL) ? rcClient.right : rcClient.bottom;
  nPage = nMax(1, extent / 3);
  nLine = nMax(1, extent / 10);

  GetScrollRange(m_hwndLayout, fnBar, &n_min, &n_max);
  SetIsScrolling(TRUE);

  switch(wScrollCode)
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

    case SB_THUMBPOSITION:
      x = nPos;
      break;

    case SB_ENDSCROLL:
      SetIsScrolling(FALSE);
      UpdateScrollRegion();
      break;

    case SB_THUMBTRACK:
      break;

    default:
      break;
    }

  if(x != GetScrollPos(m_hwndLayout, fnBar))
    {
    HDC         hdc;
    POINT       ptViewportOrg;
 
    hdc = GetDC(m_hwndLayout);
    GetViewportOrgEx(hdc, &ptViewportOrg);
    if(uMsg==WM_HSCROLL)
      SetViewportOrgEx(hdc, -x, ptViewportOrg.y, NULL);
    else
      SetViewportOrgEx(hdc, ptViewportOrg.x, -x, NULL);
    ReleaseDC(m_hwndLayout, hdc);

    SetScrollPos(m_hwndLayout, fnBar, x, TRUE);
    if(uMsg==WM_HSCROLL)
      ScrollWindow(m_hwndLayout, -(x + ptViewportOrg.x), 0, NULL, NULL);
    else
      ScrollWindow(m_hwndLayout, 0, -(x + ptViewportOrg.y), NULL, NULL);
    UpdateWindow(m_hwndLayout);
    }
  }

////////////////////////////////////////////////////////////////////////////
