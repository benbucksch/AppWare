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
// FILE:          "lay_algn.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   WINDOW LAYOUT ALIGNMENT PALETTE CONTROL FUNCTIONS
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>

#include "wlay.h"
#include "palette.h"
#include "lay_algn.h"
#include "lay_wnd.h"
#include "lay_item.h"
#include "layout.h"
#include "rc\wlay_rsc.h"

// externs /////////////////////////////////////////////////////////////////

extern HINSTANCE            g_hInstance;
extern const char*          c_szAppBuilderIni;

// consts //////////////////////////////////////////////////////////////////

const char*                 c_szAlignmentCaption = "";

// statics /////////////////////////////////////////////////////////////////

static AlignmentPalette*    s_pAlignPalette;

// internal function prototypes ////////////////////////////////////////////

HWND CreateAlignmentPalette(POINT ptStart, HWND hWnd, Layout* pLayout);

// InitAlignmentPalette ////////////////////////////////////////////////////

HWND InitAlignmentPalette
  (
  HWND    hwndOwner,
  Layout* pLayout,
  BOOL    fPrstPos
  )
  
  {
  char    szBuffer[80];
  char    szDefault[80];
  POINT   pt;
  BOOL    fShow;
  double  x, y;
  HWND    hwndRet;

  // create the alignment palette
  x = 100.* (double)(GetSystemMetrics(SM_CXSCREEN) - TOOL_WIDTH - ALIGNMENT_WIDTH) / (double)GetSystemMetrics(SM_CXSCREEN);
  y = 0.;
  sprintf(szDefault, "%lf, %lf, %hi", x, y, 1);
  if(fPrstPos)
    GetPrivateProfileString("Layout Palettes", "Alignment", szDefault, szBuffer, 80, c_szAppBuilderIni);
  else
    lstrcpy(szBuffer, szDefault);
  GetPalettePositions(szBuffer, ALIGNMENT_WIDTH, ALIGNMENT_HEIGHT + GetSystemMetrics(SM_CYCAPTION), &pt, &fShow);
  hwndRet = CreateAlignmentPalette(pt, hwndOwner, pLayout);

  ShowWindow(hwndRet, fShow ? SW_SHOWNA : SW_HIDE);
  return hwndRet;
  }

// RegisterPaletteWindow ///////////////////////////////////////////////////

void RegisterPaletteWindow
  (
  )

  {
  PaletteWindow::Init(g_hInstance);
  }


// UnegisterPaletteWindow ///////////////////////////////////////////////////

void UnregisterPaletteWindow
  (
  )

  {
  PaletteWindow::Cleanup();
  }

// AlignmentPalette::AlignmentPalette//////////////////////////////////////

AlignmentPalette::AlignmentPalette(
  Layout* pLayout,
  HWND    hOwner
  ) :
    m_pLayout(pLayout),
    PaletteWindow(PaletteWindow::popupPalette | PaletteWindow::titleBar, hOwner)
  {
  }

// AlignmentPalette::~AlignmentPalette/////////////////////////////////////

AlignmentPalette::~AlignmentPalette(
  void
  )
  {
  }

// AlignmentPalette::AddButton ////////////////////////////////////////////

void  AlignmentPalette::AddButton(
  UINT    nID,
  char*   szInPrompt,
  DWORD   helpID
  )
  {
  PaletteItem*      pItem;
  HBITMAP           hImage;

  hImage = LoadBitmap(g_hInstance, ID(nID));

  pItem  = new PaletteItem(this, nID, 0, PAL_PUSHBUTTON);

  pItem->SetImage(hImage);
  pItem->AddPrompt(szInPrompt);
  pItem->AddHelpID(helpID);
  AddItem(pItem);
  }

// AlignmentPalette::InitPalette /////////////////////////////////////////

void  AlignmentPalette::InitPalette(
  void
  )
  {
  // Add all the buttons here
  SetColumns(2);
  SetUnitSize(24, 24, 1);     // resolution is 1 column width

  AddButton(ALIGNLEFT, "Align Left", ALIGNLEFT_HELPID);
  AddButton(ALIGNRIGHT, "Align Right", ALIGNRIGHT_HELPID); 
  AddButton(ALIGNTOP, "Align Top", ALIGNTOP_HELPID);
  AddButton(ALIGNBOTTOM, "Align Bottom", ALIGNBOTTOM_HELPID);
  AddButton(CENTERHORZ, "Center Horizontally", CENTERHORZ_HELPID);
  AddButton(CENTERVERT, "Center Vertically", CENTERVERT_HELPID);
  AddButton(CENTERGROUPHORZ, "Center Group Horizontally", CENTERGROUPHORZ_HELPID);
  AddButton(CENTERGROUPVERT, "Center Group Vertically", CENTERGROUPVERT_HELPID);
  AddButton(GROWWIDE, "Grow to Widest", GROWWIDE_HELPID);
  AddButton(GROWTALL, "Grow to Tallest", GROWTALL_HELPID);
  AddButton(SHRINKTHIN, "Shrink to Thinnest", SHRINKTHIN_HELPID);
  AddButton(SHRINKSHORT, "Shrink to Shortest", SHRINKSHORT_HELPID);		
  AddButton(SPACEVERT, "Space Vertically", SPACEVERT_HELPID);
  AddButton(SPACEHORZ, "Space Horizontally", SPACEHORZ_HELPID);
//  AddButton(ALIGNTOOL_GRID, "Grid Tool", GRID_HELPID);
//  AddButton(ALIGNTOOL_MOVESIZE, "Position/Size", POSITIONSIZE_HELPID);

  EndAdd();
  }

// AlignmentPalette::Closing ////////////////////////////////////////////

void  AlignmentPalette::Closing(
  void
  )
  {
  ShowWindow(Hwnd(), SW_HIDE);
  }

// AlignmentPalette::ItemChosen /////////////////////////////////////////

void  AlignmentPalette::ItemChosen(
  PaletteItem*    pItem
  )
  {
  Window*         pWnd = ((AlignmentPalette*) pItem->Owner())->m_pLayout->ActiveWindow();
  Items*          pItems = pWnd->Items();

  switch (pItem->ItemID())
    {
    case ALIGNLEFT:
      pItems->AlignLeft();
      break;

    case ALIGNRIGHT:
      pItems->AlignRight();
      break;

    case ALIGNTOP:
      pItems->AlignTop();
      break;

    case ALIGNBOTTOM:
      pItems->AlignBottom();
      break;

    case CENTERHORZ:
      pItems->CenterHorz();
      break;

    case CENTERVERT:
      pItems->CenterVert();
      break;
		  
    case CENTERGROUPHORZ:
      pItems->CenterHorzInWnd();
      break;

    case CENTERGROUPVERT:
      pItems->CenterVertInWnd();
      break;

    case GROWWIDE:
      pItems->GrowHorz();
      break;

    case GROWTALL:
      pItems->GrowVert();
      break;

    case SHRINKTHIN:
      pItems->ShrinkHorz();
      break;

    case SHRINKSHORT:
      pItems->ShrinkVert();
      break;

    case SPACEHORZ:
      pItems->SpaceHorz();
      break;

    case SPACEVERT:
      pItems->SpaceVert();
      break;

/*
    case ALIGNTOOL_GRID:
      pWnd->EditGrid();
      break;

    case ALIGNTOOL_MOVESIZE:
      pItems->PosAndSize();
      break;
*/
    }

  SetActiveWindow(m_pLayout->WndHandle());
  }

// CreateAlignmentPalette //////////////////////////////////////////////////

HWND  CreateAlignmentPalette
  (
  POINT   ptStart,
  HWND    hWnd,
  Layout* pLayout
  )

  {
  HWND    hPalette    = NULL;

  if (s_pAlignPalette != NULL)
    DestroyAlignmentPalette(FALSE);

  s_pAlignPalette = new AlignmentPalette(pLayout, hWnd);

  if (s_pAlignPalette)
    {
    s_pAlignPalette->InitPalette();
    hPalette = s_pAlignPalette->Hwnd();
    SetWindowText(hPalette, c_szAlignmentCaption);
    SetWindowPos(hPalette, NULL, ptStart.x, ptStart.y, 0, 0,
                    SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE);
    }

  return hPalette;
  }

// DestroyAlignmentPalette //////////////////////////////////////////////////

void  DestroyAlignmentPalette
  (
  BOOL  fPrstPos
  )
  {
  if (s_pAlignPalette != NULL)
    {
    if(fPrstPos)
      {
      RECT    rc;
      double  x, y;
      char    szBuffer[80];
      short   nVisible = (short)(IsWindowVisible(s_pAlignPalette->Hwnd()) ? 1 : 0);

      GetWindowRect(s_pAlignPalette->Hwnd(), &rc);

      x = 100. * (double)rc.left / (double)GetSystemMetrics(SM_CXSCREEN);
      y = 100. * (double)rc.top  / (double)GetSystemMetrics(SM_CYSCREEN);
      sprintf(szBuffer, "%.3lf, %.3lf, %hi", x, y, nVisible);
      WritePrivateProfileString("Layout Palettes", "Alignment", szBuffer, c_szAppBuilderIni);
      }

    delete s_pAlignPalette;
    s_pAlignPalette = NULL;
    }
  }

// EnableAlignmentPalette //////////////////////////////////////////////////

void  EnableAlignmentPalette
  (
  BOOL    fEnable
  )
  {
  if (s_pAlignPalette != NULL)
    s_pAlignPalette->Enable(fEnable);
  }

////////////////////////////////////////////////////////////////////////////
