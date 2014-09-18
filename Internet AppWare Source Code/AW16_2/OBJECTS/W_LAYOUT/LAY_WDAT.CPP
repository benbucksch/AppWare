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
// FILE:          "lay_wdat.cpp"
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

#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wndlay.h>
#include <o_wnd.h>

#include "lay_wdat.h"

// static member definition ////////////////////////////////////////////////

WndPalette WndData::sm_palette =
  { 
  0x0300,
  256,
  {{0xFF, 0xFF, 0xFF, 0}, {0x00, 0x00, 0x00, 0}, {0x77, 0x77, 0x77, 0}, {0xFF, 0x00, 0x00, 0},
   {0x00, 0xFF, 0x00, 0}, {0x00, 0x00, 0xFF, 0}, {0xFF, 0xFF, 0x00, 0}, {0xFF, 0x00, 0xFF, 0},
   {0x00, 0xFF, 0xFF, 0}, {0xBB, 0xBB, 0xBB, 0}, {0xFF, 0x99, 0x00, 0}, {0x66, 0x99, 0x00, 0},
   {0x00, 0x66, 0xFF, 0}, {0xFF, 0xFF, 0x99, 0}, {0xFF, 0x99, 0xFF, 0}, {0x66, 0xFF, 0x99, 0},
   {0x22, 0x22, 0x22, 0}, {0xCC, 0x00, 0x00, 0}, {0x66, 0xCC, 0x00, 0}, {0x66, 0xCC, 0xFF, 0},
   {0xFF, 0xCC, 0x66, 0}, {0xCC, 0x99, 0xFF, 0}, {0xCC, 0xFF, 0xFF, 0}, {0x88, 0x88, 0x88, 0},
   {0xFF, 0x66, 0x33, 0}, {0x00, 0xCC, 0x33, 0}, {0x00, 0x00, 0x77, 0}, {0xFF, 0xFF, 0x66, 0},
   {0xFF, 0x99, 0xCC, 0}, {0x99, 0xFF, 0xCC, 0}, {0x44, 0x44, 0x44, 0}, {0xFF, 0x33, 0x00, 0},
   {0x99, 0xFF, 0x33, 0}, {0x66, 0x99, 0xFF, 0}, {0xFF, 0xCC, 0x00, 0}, {0xCC, 0x00, 0x99, 0},
   {0x99, 0xFF, 0xFF, 0}, {0xAA, 0xAA, 0xAA, 0}, {0xCC, 0x66, 0x00, 0}, {0x66, 0x99, 0x33, 0},
   {0xCC, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0x00, 0}, {0xFF, 0xCC, 0xFF, 0}, {0x99, 0x99, 0xFF, 0},
   {0x55, 0x55, 0x55, 0}, {0xDD, 0x00, 0x00, 0}, {0x00, 0xDD, 0x00, 0}, {0x00, 0x00, 0xBB, 0},
   {0xFF, 0xFF, 0x33, 0}, {0xFF, 0xCC, 0xCC, 0}, {0x33, 0xFF, 0xCC, 0}, {0xEE, 0xEE, 0xEE, 0},
   {0x88, 0x00, 0x00, 0}, {0x00, 0x55, 0x00, 0}, {0x00, 0x00, 0x55, 0}, {0xCC, 0xCC, 0x00, 0},
   {0xCC, 0x33, 0xCC, 0}, {0x33, 0xFF, 0xFF, 0}, {0x11, 0x11, 0x11, 0}, {0xFF, 0x33, 0x66, 0},
   {0x33, 0xCC, 0x33, 0}, {0x00, 0x66, 0xCC, 0}, {0xFF, 0xCC, 0x33, 0}, {0x99, 0x00, 0x66, 0},
   {0x66, 0x99, 0xCC, 0}, {0xDD, 0xDD, 0xDD, 0}, {0xAA, 0x00, 0x00, 0}, {0x00, 0x77, 0x00, 0},
   {0x00, 0x00, 0x88, 0}, {0xCC, 0xFF, 0x33, 0}, {0xCC, 0x00, 0xCC, 0}, {0x99, 0xCC, 0xFF, 0},
   {0xCC, 0xCC, 0xCC, 0}, {0xFF, 0x33, 0x33, 0}, {0x00, 0xCC, 0x00, 0}, {0x33, 0x33, 0xFF, 0},
   {0xCC, 0xCC, 0x33, 0}, {0xFF, 0x00, 0xCC, 0}, {0x33, 0x99, 0xCC, 0}, {0x99, 0x99, 0x99, 0},
   {0xFF, 0x66, 0x00, 0}, {0x66, 0xFF, 0x00, 0}, {0x33, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0x66, 0},
   {0x66, 0x66, 0x99, 0}, {0x66, 0xCC, 0xCC, 0}, {0x66, 0x66, 0x66, 0}, {0x44, 0x00, 0x00, 0},
   {0x00, 0x22, 0x00, 0}, {0x00, 0x00, 0x33, 0}, {0x99, 0x99, 0x00, 0}, {0x99, 0x66, 0x99, 0},
   {0x66, 0x66, 0xCC, 0}, {0x99, 0xCC, 0xCC, 0}, {0xEE, 0x00, 0x00, 0}, {0x00, 0xEE, 0x00, 0},
   {0x00, 0x00, 0xEE, 0}, {0xFF, 0xFF, 0xCC, 0}, {0xFF, 0x33, 0xFF, 0}, {0x00, 0xFF, 0xCC, 0},
   {0x99, 0x99, 0xCC, 0}, {0xFF, 0x00, 0x99, 0}, {0x00, 0x88, 0x00, 0}, {0x66, 0x33, 0xFF, 0},
   {0xCC, 0x99, 0x33, 0}, {0xCC, 0x33, 0xFF, 0}, {0x66, 0xFF, 0xCC, 0}, {0x33, 0x33, 0x33, 0},
   {0xBB, 0x00, 0x00, 0}, {0x00, 0x66, 0x00, 0}, {0x00, 0x00, 0x44, 0}, {0xCC, 0x99, 0x66, 0},
   {0xFF, 0x66, 0x99, 0}, {0x33, 0xCC, 0xFF, 0}, {0x99, 0x99, 0x66, 0}, {0xFF, 0x00, 0x33, 0},
   {0x99, 0xCC, 0x00, 0}, {0x00, 0x00, 0xAA, 0}, {0xCC, 0xFF, 0x99, 0}, {0xCC, 0x33, 0x99, 0},
   {0x66, 0x66, 0xFF, 0}, {0x99, 0x66, 0x66, 0}, {0x55, 0x00, 0x00, 0}, {0x00, 0x11, 0x00, 0},
   {0x33, 0x00, 0x66, 0}, {0xFF, 0xCC, 0x99, 0}, {0xCC, 0x66, 0x99, 0}, {0x00, 0xFF, 0x99, 0},
   {0xCC, 0x99, 0xCC, 0}, {0xFF, 0x00, 0x66, 0}, {0x33, 0xCC, 0x00, 0}, {0x00, 0x00, 0xCC, 0},
   {0x99, 0xFF, 0x00, 0}, {0x99, 0x33, 0x66, 0}, {0x00, 0xCC, 0xFF, 0}, {0x66, 0x99, 0x99, 0},
   {0xCC, 0x00, 0x33, 0}, {0x00, 0x99, 0x00, 0}, {0x66, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0xCC, 0},
   {0xCC, 0x33, 0x66, 0}, {0x99, 0xFF, 0x99, 0}, {0xCC, 0xCC, 0xFF, 0}, {0xCC, 0x33, 0x00, 0},
   {0x00, 0xFF, 0x33, 0}, {0x00, 0x33, 0xFF, 0}, {0xFF, 0x99, 0x66, 0}, {0xCC, 0x00, 0x66, 0},
   {0x66, 0xFF, 0xFF, 0}, {0x33, 0x66, 0x66, 0}, {0x99, 0x00, 0x00, 0}, {0x99, 0xFF, 0x66, 0},
   {0x33, 0x99, 0xFF, 0}, {0xCC, 0xCC, 0x66, 0}, {0xFF, 0x33, 0xCC, 0}, {0x00, 0x99, 0xCC, 0},
   {0xCC, 0x99, 0x99, 0}, {0x77, 0x00, 0x00, 0}, {0x00, 0x33, 0x00, 0}, {0x33, 0x00, 0xCC, 0},
   {0xCC, 0x99, 0x00, 0}, {0xCC, 0x66, 0xCC, 0}, {0x00, 0x99, 0xFF, 0}, {0x66, 0x33, 0x33, 0},
   {0xCC, 0x33, 0x33, 0}, {0x33, 0x66, 0x00, 0}, {0x00, 0x00, 0x22, 0}, {0xCC, 0xCC, 0x99, 0},
   {0xCC, 0x66, 0xFF, 0}, {0x00, 0xCC, 0xCC, 0}, {0x66, 0x66, 0x33, 0}, {0x22, 0x00, 0x00, 0},
   {0x33, 0xFF, 0x00, 0}, {0x00, 0x33, 0x99, 0}, {0x99, 0x66, 0x00, 0}, {0xFF, 0x66, 0xFF, 0},
   {0x33, 0xCC, 0xCC, 0}, {0x99, 0xCC, 0x99, 0}, {0xFF, 0x33, 0x99, 0}, {0x33, 0xFF, 0x66, 0},
   {0x00, 0x00, 0x11, 0}, {0xFF, 0x99, 0x33, 0}, {0x99, 0x33, 0x99, 0}, {0x66, 0xCC, 0x99, 0},
   {0x99, 0xCC, 0x66, 0}, {0x99, 0x00, 0x33, 0}, {0x00, 0x44, 0x00, 0}, {0x33, 0x66, 0xFF, 0},
   {0x99, 0x99, 0x33, 0}, {0xFF, 0x66, 0xCC, 0}, {0x33, 0xFF, 0x99, 0}, {0x33, 0x99, 0x99, 0},
   {0x33, 0x00, 0x00, 0}, {0x00, 0xFF, 0x66, 0}, {0x33, 0x00, 0x99, 0}, {0x99, 0x66, 0x33, 0},
   {0x99, 0x33, 0xCC, 0}, {0x33, 0xCC, 0x99, 0}, {0x99, 0x66, 0xCC, 0}, {0x99, 0x33, 0x00, 0},
   {0x00, 0x99, 0x33, 0}, {0x00, 0x00, 0x99, 0}, {0xCC, 0x66, 0x33, 0}, {0x99, 0x00, 0x99, 0},
   {0x33, 0x66, 0xCC, 0}, {0x33, 0x33, 0x66, 0}, {0xFF, 0x66, 0x66, 0}, {0x00, 0x66, 0x33, 0},
   {0x66, 0x00, 0xCC, 0}, {0x66, 0xFF, 0x33, 0}, {0xFF, 0x99, 0x99, 0}, {0x00, 0x66, 0x66, 0},
   {0x33, 0x99, 0x00, 0}, {0x33, 0x33, 0x99, 0}, {0x66, 0xFF, 0x66, 0}, {0x33, 0x00, 0x33, 0},
   {0xCC, 0x66, 0x66, 0}, {0x00, 0xAA, 0x00, 0}, {0x00, 0x33, 0xCC, 0}, {0x66, 0x33, 0x00, 0},
   {0x33, 0x99, 0x66, 0}, {0x66, 0x00, 0x00, 0}, {0x00, 0xBB, 0x00, 0}, {0x66, 0x00, 0x99, 0},
   {0x33, 0xFF, 0x33, 0}, {0x66, 0x33, 0xCC, 0}, {0x66, 0x00, 0x33, 0}, {0x33, 0x33, 0x00, 0},
   {0x99, 0x00, 0xCC, 0}, {0x00, 0x99, 0x66, 0}, {0x00, 0x00, 0x66, 0}, {0x66, 0xCC, 0x66, 0},
   {0x66, 0x00, 0x66, 0}, {0x99, 0x66, 0xFF, 0}, {0x33, 0x33, 0xCC, 0}, {0x00, 0x99, 0x99, 0},
   {0x99, 0x33, 0x33, 0}, {0x00, 0x00, 0xDD, 0}, {0x99, 0xCC, 0x33, 0}, {0x66, 0x33, 0x66, 0},
   {0x11, 0x00, 0x00, 0}, {0x33, 0x66, 0x99, 0}, {0x66, 0x66, 0x00, 0}, {0x99, 0x00, 0xFF, 0},
   {0x00, 0x33, 0x33, 0}, {0x33, 0x99, 0x33, 0}, {0x99, 0x33, 0xFF, 0}, {0x00, 0xCC, 0x99, 0},
   {0x00, 0x33, 0x66, 0}, {0x66, 0xCC, 0x33, 0}, {0x66, 0x99, 0x66, 0}, {0x66, 0x33, 0x99, 0},
   {0x33, 0xCC, 0x66, 0}, {0x33, 0x66, 0x33, 0}, {0x00, 0xCC, 0x66, 0}, {0x00, 0x66, 0x99, 0}}
  };

HPALETTE WndData::sm_hpal = 0;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       WndData Constructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

WndData::WndData
  (
  OBJECTID  oiWnd
  )

  {
  m_oiWnd    = oiWnd;
  m_fDirty   = FALSE;
  m_nChanged = 0;

  GetWindowData();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       WndData Destructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

WndData::~WndData
  (
  )

  {
  }
  
////////////////////////////////////////////////////////////////////////////
//
// Method:      WndData::GetWindowData (protected)
//
// Description: This function gets the window object data in a useful form
//
////////////////////////////////////////////////////////////////////////////

void WndData::GetWindowData
  (
  )

  {
  COLORREF  clrFG, clrBG;
  int       pattern[8];

  OWndLayGetTitle(m_oiWnd, m_szTitle, 256);
  OWndLayGetInitialClientRect(m_oiWnd, &m_rcInitClient);

  // fix the initial client to avoid rounding errors
  OWndLStoDS(m_oiWnd, (LPPOINT)&m_rcInitClient, 2);
  OWndDStoLS(m_oiWnd, (LPPOINT)&m_rcInitClient, 2);

  m_style = 0L;
  if(OWndLayHasTitleBar   (m_oiWnd))  m_style |= WF_TITLEBAR;
  if(OWndLayHasSysmenu    (m_oiWnd))  m_style |= WF_SYSMENU;
  if(OWndLayHasMaxbutton  (m_oiWnd))  m_style |= WF_MAXBUTTON;
  if(OWndLayHasMinbutton  (m_oiWnd))  m_style |= WF_MINBUTTON;
  if(OWndLayHasHScroll    (m_oiWnd))  m_style |= WF_HORZSCROLL;
  if(OWndLayHasVScroll    (m_oiWnd))  m_style |= WF_VERTSCROLL;
  if(OWndLayHasResizeFrame(m_oiWnd))  m_style |= WF_RESIZEBORDER;
  if(OWndLayHasDialogFrame(m_oiWnd))  m_style |= WF_DIALOGFRAME;
  if(OWndLayIsModal       (m_oiWnd))  m_style |= WF_MODAL;
  if(OWndLayIsPopup       (m_oiWnd))  m_style |= WF_ALWAYSONTOP;
  if(OWndLayIsInitVisible (m_oiWnd))  m_style |= WF_INITVISIBLE;
  if(OWndLayDrawsSmooth   (m_oiWnd))  m_style |= WF_DRAWSMOOTH;
  if(OWndLayQuitsOnClose  (m_oiWnd))  m_style |= WF_QUITONCLOSE;

  m_nInitCmdShow  = OWndLayGetInitialWindowState(m_oiWnd);
  m_fLimitMaxSize = OWndLayHasMaxSize(m_oiWnd);
  m_ptMaxSize     = OWndLayGetMaxSize(m_oiWnd);
  m_ptMinSize     = OWndLayGetMinSize(m_oiWnd);
  m_ptScrollSize  = OWndLayGetScrollSize(m_oiWnd);
  m_oiMenu        = OWndLayGetMenubarID(m_oiWnd);

  OWndLayGetColorsAndPattern(m_oiWnd, &clrBG, &clrFG, NULL, NULL, (short *)pattern, sizeof(pattern));
  m_fill.SetFGColor(clrFG);
  m_fill.SetBGColor(clrBG);
  m_fill.SetFillPattern(-1, pattern);

  // make sure that the window is in a valid original state
  if(!((m_nInitCmdShow==SW_SHOWNORMAL)
               ||
       (m_nInitCmdShow==SW_SHOWMAXIMIZED)
               ||
       (m_nInitCmdShow==SW_SHOWMINIMIZED)))
    {
    m_nInitCmdShow = SW_SHOWNORMAL;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      WndData::SaveWindowData (protected)
//
// Description: This function saves the window object data.
//
////////////////////////////////////////////////////////////////////////////

void WndData::SaveWindowData
  (
  )

  {
  COLORREF  clrFG, clrBG;
  int       pattern[8];

  OWndLaySetTitle             (m_oiWnd,  m_szTitle);
  OWndLaySetInitialClientRect (m_oiWnd, &m_rcInitClient);
  OWndLaySetHasTitleBar       (m_oiWnd, (m_style & WF_TITLEBAR)      ? TRUE : FALSE);
  OWndLaySetHasSysmenu        (m_oiWnd, (m_style & WF_SYSMENU)       ? TRUE : FALSE);
  OWndLaySetHasMaxbutton      (m_oiWnd, (m_style & WF_MAXBUTTON)     ? TRUE : FALSE);
  OWndLaySetHasMinbutton      (m_oiWnd, (m_style & WF_MINBUTTON)     ? TRUE : FALSE);
  OWndLaySetHasHScroll        (m_oiWnd, (m_style & WF_HORZSCROLL)    ? TRUE : FALSE);
  OWndLaySetHasVScroll        (m_oiWnd, (m_style & WF_VERTSCROLL)    ? TRUE : FALSE);
  OWndLaySetHasResizeFrame    (m_oiWnd, (m_style & WF_RESIZEBORDER)  ? TRUE : FALSE);
  OWndLaySetHasDialogFrame    (m_oiWnd, (m_style & WF_DIALOGFRAME)   ? TRUE : FALSE);
  OWndLaySetIsModal           (m_oiWnd, (m_style & WF_MODAL)         ? TRUE : FALSE);
  OWndLaySetIsPopup           (m_oiWnd, (m_style & WF_ALWAYSONTOP)   ? TRUE : FALSE);
  OWndLaySetIsInitVisible     (m_oiWnd, (m_style & WF_INITVISIBLE)   ? TRUE : FALSE);
  OWndLaySetDrawsSmooth       (m_oiWnd, (m_style & WF_DRAWSMOOTH)    ? TRUE : FALSE);
  OWndLaySetQuitsOnClose      (m_oiWnd, (m_style & WF_QUITONCLOSE)   ? TRUE : FALSE);
  OWndLaySetInitialWindowState(m_oiWnd,  m_nInitCmdShow);
  OWndLaySetMaxSize           (m_oiWnd,  m_ptMaxSize);
  OWndLaySetHasMaxSize        (m_oiWnd,  m_fLimitMaxSize);
  OWndLaySetMinSize           (m_oiWnd,  m_ptMinSize);
  OWndLaySetScrollSize        (m_oiWnd,  m_ptScrollSize);
  OWndLaySetMenuBarID         (m_oiWnd,  m_oiMenu);

  // save the colors and pattern
  clrFG = m_fill.GetTrueFGColor();
  clrBG = m_fill.GetTrueBGColor();
  memcpy(pattern, FillAttrib::GetFillPattern(-1, &m_fill), sizeof(pattern));
  OWndLaySetColorsAndPattern(m_oiWnd, clrBG, clrFG, clrBG, clrFG, (short *)pattern, sizeof(pattern));
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      WndData::GetWindowStyle (protected)
//
// Description: This function converts the window style bits to the Windows
//              style flags.  Note:  Scroll bars are ignored because they
//              are part of the client window.
//
////////////////////////////////////////////////////////////////////////////

DWORD WndData::GetWindowStyle
  (
  )

  {
  DWORD dwStyle = WS_VISIBLE | WS_BORDER | WS_CHILD;

  if (m_style & WF_TITLEBAR)     dwStyle |= WS_CAPTION;
  if (m_style & WF_SYSMENU)      dwStyle |= WS_SYSMENU;
  if (m_style & WF_MAXBUTTON)    dwStyle |= WS_MAXIMIZEBOX;
  if (m_style & WF_MINBUTTON)    dwStyle |= WS_MINIMIZEBOX;
  if (m_style & WF_HORZSCROLL)   dwStyle |= WS_HSCROLL;
  if (m_style & WF_VERTSCROLL)   dwStyle |= WS_VSCROLL;
  if (m_style & WF_RESIZEBORDER) dwStyle |= WS_THICKFRAME;

  if(m_style & WF_DIALOGFRAME)
    dwStyle &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);

  return dwStyle;
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      WndData::GetWindowStyleEx (protected)
//
// Description: This function converts the window style bits to the Windows
//              extended style flags.
//
////////////////////////////////////////////////////////////////////////////

DWORD WndData::GetWindowStyleEx
  (
  )

  {
  DWORD dwExStyle;

  if (m_style & WF_DIALOGFRAME)
    dwExStyle = WS_EX_DLGMODALFRAME;
  else
    dwExStyle = 0L;

  return dwExStyle;
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      WndData::CreatePalette  (static)
//
// Description: This function will create the default palette for the window
//              object.
//
////////////////////////////////////////////////////////////////////////////

void WndData::CreatePalette
  (
  )

  {
  HDC hdc         = CreateDC("DISPLAY", NULL, NULL, NULL);
  int nPalSize    = GetDeviceCaps(hdc, SIZEPALETTE);

  sm_hpal = (nPalSize==256) ? ::CreatePalette((LOGPALETTE*)&sm_palette) : NULL;

  DeleteDC(hdc);
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      WndData::DestroyPalette  (static)
//
// Description: This function will create the default palette for the window
//              object.
//
////////////////////////////////////////////////////////////////////////////

void WndData::DestroyPalette
  (
  )

  {
  if(sm_hpal)
    {
    DeleteObject(sm_hpal);
    sm_hpal = NULL;
    }
  }

////////////////////////////////////////////////////////////////////////////
