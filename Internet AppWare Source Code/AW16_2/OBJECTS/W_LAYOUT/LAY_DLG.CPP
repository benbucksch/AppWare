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
// FILE:          "lay_dlg.cpp"
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_menus.h>

#include "wlay.h"
#include "lay_dlg.h"
#include "layout.h"
#include "rc\wlay_rsc.h"
#include "lay_item.h"
#include "objlist.h"

// externs /////////////////////////////////////////////////////////////////

extern HINSTANCE    g_hInstance;
extern const char*  c_szSampleClassName;
extern const char*  c_szAppBuilderIni;

const char* c_szSampleWindow = "SampleWindow";

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     SetItemValue
//
//  DESCRIPTION:  The incoming value (nVal) is in .1 milimeter units this
//                will convert the value and set the ctrl's text.
//
////////////////////////////////////////////////////////////////////////////

void SetItemValue
  (
  HWND  hDlg,
  UINT  idCtrl,
  int   nVal,
  WORD  fUnits
  )

  {
  char    szBuffer[80];
  double  dVal;

  if(fUnits & LAYPREF_INCHES)
    {
    dVal = (double)(nVal / 254.);
    sprintf(szBuffer, "%.3lf\"", dVal);
    }
  else if(fUnits & LAYPREF_MILLIMETERS)
    {
    dVal = (double)(nVal / 10.);
    sprintf(szBuffer, "%.1lf mm", dVal);
    }
  else // points
    {
    dVal = (double)(nVal / 254. * 72.);
    sprintf(szBuffer, "%.2lf pt", dVal);
    }

  SetDlgItemText(hDlg, idCtrl, szBuffer);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     GetItemValue
//
//  DESCRIPTION:  The value stored by the control's text is in the current
//                units.  This will convert that to .1 mm units.
//
////////////////////////////////////////////////////////////////////////////

int GetItemValue
  (
  HWND  hDlg,
  UINT  idCtrl,
  WORD  fUnits
  )

  {
  char    szBuffer[80];
  double  dVal;
  int     rVal = 0;

  GetDlgItemText(hDlg, idCtrl, szBuffer, 80);
  dVal = strtod(szBuffer, NULL);

  if(fUnits & LAYPREF_INCHES)
    rVal = dVal * 254. + .5;
  else if(fUnits & LAYPREF_MILLIMETERS)
    rVal = dVal * 10. + .5;
  else // points
    rVal = dVal * 254. / 72. + .5;

  return rVal;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     OWLayGridDlgProc
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export OWLayGridDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL      fRet = FALSE;
  GRID_DLG* gd;

  switch(uMsg)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitGridDlg(hwnd);
      SetFocus(GetDlgItem(hwnd, IDOK));
      fRet = FALSE;
      break;

    case WM_COMMAND:
      gd = (GRID_DLG*)GetWindowLong(hwnd, DWL_USER);
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, UpdateGrid(hwnd));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case PB_RESETGRID:
          SetItemValue(hwnd, ED_GRIDWIDTH, 0, gd->fUnits);
          SetItemValue(hwnd, ED_GRIDHEIGHT, 0, gd->fUnits);
          EnableWindow(GetDlgItem(hwnd, XB_SHOWGRID), FALSE);
          EnableWindow(GetDlgItem(hwnd, XB_SNAPTOGRID), FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          break;

        case ED_GRIDWIDTH:
        case ED_GRIDHEIGHT:
          if((HIWORD(lParam)==EN_KILLFOCUS) || (HIWORD(lParam)==EN_CHANGE))
            {
            POINT pt;
            pt.x = GetItemValue(hwnd, wParam, gd->fUnits);
            pt.y = GetItemValue(hwnd, (wParam==ED_GRIDWIDTH) ? ED_GRIDHEIGHT : ED_GRIDWIDTH, gd->fUnits);
          
            if(HIWORD(lParam)==EN_KILLFOCUS)
              SetItemValue(hwnd, wParam, (pt.x>=0) ? pt.x : 0, gd->fUnits);

            OWndLStoDS(gd->oiWnd, &pt, 1);
            EnableWindow(GetDlgItem(hwnd, XB_SHOWGRID), (pt.x<=1 || pt.y<=1) ? FALSE : TRUE);
            EnableWindow(GetDlgItem(hwnd, XB_SNAPTOGRID), (pt.x<=1 || pt.y<=1) ? FALSE : TRUE);
            fRet = TRUE;
            }
          break;
        }
      break;

    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     InitGridDlg
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void InitGridDlg
  (
  HWND  hDlg
  )

  {
  GRID_DLG* gd = (GRID_DLG*)GetWindowLong(hDlg, DWL_USER);
  POINT     pt;

  // show the grid settings
  SetItemValue(hDlg, ED_GRIDWIDTH, gd->ptGrid.x, gd->fUnits);
  SetItemValue(hDlg, ED_GRIDHEIGHT, gd->ptGrid.y, gd->fUnits);
  CheckDlgButton(hDlg, XB_SHOWGRID, gd->fShowGrid);
  CheckDlgButton(hDlg, XB_SNAPTOGRID, gd->fSnapToGrid);

  CopyPoint(&pt, &gd->ptGrid);
  OWndLStoDS(gd->oiWnd, &pt, 1);
  EnableWindow(GetDlgItem(hDlg, XB_SHOWGRID), (pt.x<=1 || pt.y<=1) ? FALSE : TRUE);
  EnableWindow(GetDlgItem(hDlg, XB_SNAPTOGRID), (pt.x<=1 || pt.y<=1) ? FALSE : TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     UpdateGrid
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL UpdateGrid
  (
  HWND  hDlg
  )

  {
  BOOL      fRet = FALSE;
  GRID_DLG* gd = (GRID_DLG*)GetWindowLong(hDlg, DWL_USER);
  int       nVal;

  nVal = GetItemValue(hDlg, ED_GRIDWIDTH, gd->fUnits);
  if(nVal!=gd->ptGrid.x)
    {
    gd->ptGrid.x = nVal;
    fRet = TRUE;
    }

  nVal = GetItemValue(hDlg, ED_GRIDHEIGHT, gd->fUnits);
  if(nVal!=gd->ptGrid.y)
    {
    gd->ptGrid.y = nVal;
    fRet = TRUE;
    }

  if((IsDlgButtonChecked(hDlg, XB_SHOWGRID) ? TRUE : FALSE) != gd->fShowGrid)
    {
    gd->fShowGrid = !gd->fShowGrid;
    fRet = TRUE;
    }

  if((IsDlgButtonChecked(hDlg, XB_SNAPTOGRID) ? TRUE : FALSE) != gd->fSnapToGrid)
    {
    gd->fSnapToGrid = !gd->fSnapToGrid;
    fRet = TRUE;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Static data:  AnchorItem::sm_bmp
//
//  Description:
//**************************************************************************
//   To make the logic simplier for the attach dialog, all the different
// states are represented in the sm_bmp member.  The index into the array
// is a number from the four state flags for attaching.  BMP_???? means the
// bitmap with the left (the first position) either attached (X) or not
// attached (O).  The BMP_(Left)(Top)(Right)(Bottom) must be in sequential
// order for the logic to work.
//   To indicate that a state is not used, I have defaulted the use count
// to 1.  This will always cause the use count for invalid states be be one
// higher than valid states.  This will also force the invalid bitmaps to
// never be loaded or deleted.
//**************************************************************************
//
////////////////////////////////////////////////////////////////////////////

ATTACHBMP AnchorItem::sm_bmp[] =
  {
    {NULL, NULL, 0},  // BMP_OOOO
    {NULL, NULL, 0},  // BMP_OOOX
    {NULL, NULL, 0},  // BMP_OOXO
    {NULL, NULL, 0},  // BMP_OOXX
    {NULL, NULL, 1},  // BMP_OXOO   // unused
    {NULL, NULL, 0},  // BMP_OXOX
    {NULL, NULL, 1},  // BMP_OXXO   // unused
    {NULL, NULL, 0},  // BMP_OXXX
    {NULL, NULL, 1},  // BMP_XOOO   // unused
    {NULL, NULL, 1},  // BMP_XOOX   // unused
    {NULL, NULL, 0},  // BMP_XOXO
    {NULL, NULL, 0},  // BMP_XOXX
    {NULL, NULL, 1},  // BMP_XXOO   // unused
    {NULL, NULL, 1},  // BMP_XXOX   // unused
    {NULL, NULL, 1},  // BMP_XXXO   // unused
    {NULL, NULL, 0}   // BMP_XXXX
  };

////////////////////////////////////////////////////////////////////////////
//
//  Method:       AnchorItem constructor
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

AnchorItem::AnchorItem
  (
  int   nOrigState,
  BOOL  fMultipleItems,
  char* szTitle
  )

  {
  int i;
  int nCount = sizeof(sm_bmp) / sizeof(sm_bmp[0]);

  m_fMultipleItems = fMultipleItems;
  m_nOriginalState = nOrigState;
  m_nCurrentState = nOrigState;
  m_szTitle = szTitle;

  // load the bitmaps
  for(i=0; i<nCount; i++)
    {
    if(sm_bmp[i].nUseCount==0)
      {
      sm_bmp[i].hbmpUp = LoadBitmap(g_hInstance, MAKEINTRESOURCE(BMP_OOOO + BMP_NORMAL + i));
      sm_bmp[i].hbmpDown = LoadBitmap(g_hInstance, MAKEINTRESOURCE(BMP_OOOO + BMP_PUSHED + i));
      }

    // the use count always needs to be incremented -- even if it is not loaded.
    sm_bmp[i].nUseCount++;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       AnchorItem destructor
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

AnchorItem::~AnchorItem
  (
  )

  {
  int i;
  int nCount = sizeof(sm_bmp) / sizeof(sm_bmp[0]);

  // free the bitmaps
  for(i=0; i<nCount; i++)
    {
    if(sm_bmp[i].nUseCount>0)
      sm_bmp[i].nUseCount--;

    if(sm_bmp[i].nUseCount==0)
      {
      if(sm_bmp[i].hbmpUp)
        {
        DeleteObject(sm_bmp[i].hbmpUp);
        sm_bmp[i].hbmpUp = NULL;
        }

      if(sm_bmp[i].hbmpDown)
        {
        DeleteObject(sm_bmp[i].hbmpDown);
        sm_bmp[i].hbmpDown = NULL;
        }
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       AnchorItem::DlgProc
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

BOOL WINAPI AnchorItem::DlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  AnchorItem* ai;
  BOOL        fRet = FALSE;
  switch(uMsg)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
      ai = (AnchorItem*)lParam;
      SetWindowLong(hwnd, DWL_USER, lParam);
      ai->InitDialog(hwnd);
      fRet = TRUE;
      break;

    case WM_DRAWITEM:
      switch(wParam)
        {
        case PB_CURRENTATTACH:
          ai = (AnchorItem*)GetWindowLong(hwnd, DWL_USER);
          ai->DrawCurrentSample((LPDRAWITEMSTRUCT)lParam);
          fRet = TRUE;
          break;

        case BMP_OOOO:
        case BMP_OOOX:
        case BMP_OOXO:
        case BMP_OOXX:
        case BMP_OXOO:  // unused
        case BMP_OXOX:
        case BMP_OXXO:  // unused
        case BMP_OXXX:
        case BMP_XOOO:  // unused
        case BMP_XOOX:  // unused
        case BMP_XOXO:
        case BMP_XOXX:
        case BMP_XXOO:  // unused
        case BMP_XXOX:  // unused
        case BMP_XXXO:  // unused
        case BMP_XXXX:
          ai = (AnchorItem*)GetWindowLong(hwnd, DWL_USER);
          ai->DrawAnchorBitmap((int)wParam - BMP_OOOO, (LPDRAWITEMSTRUCT)lParam);
          fRet = TRUE;
          break;

        default:
          break;
        }
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          ai = (AnchorItem*)GetWindowLong(hwnd, DWL_USER);
          EndDialog(hwnd, ai->m_fMultipleItems || (ai->m_nOriginalState!=ai->m_nCurrentState));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          fRet = TRUE;
          break;

        case BMP_OOOO:
        case BMP_OOOX:
        case BMP_OOXO:
        case BMP_OOXX:
        case BMP_OXOO:  // unused
        case BMP_OXOX:
        case BMP_OXXO:  // unused
        case BMP_OXXX:
        case BMP_XOOO:  // unused
        case BMP_XOOX:  // unused
        case BMP_XOXO:
        case BMP_XOXX:
        case BMP_XXOO:  // unused
        case BMP_XXOX:  // unused
        case BMP_XXXO:  // unused
        case BMP_XXXX:
          ai = (AnchorItem*)GetWindowLong(hwnd, DWL_USER);
          if(((int)wParam - BMP_OOOO) != ai->m_nCurrentState)
            {
            InvalidateRect(GetDlgItem(hwnd, PB_CURRENTATTACH), NULL, FALSE);
            InvalidateRect(GetDlgItem(hwnd, BMP_OOOO + ai->m_nCurrentState), NULL, FALSE);
            InvalidateRect(GetDlgItem(hwnd, wParam), NULL, FALSE);
            ai->m_nCurrentState = (int)wParam - BMP_OOOO;
            }
          fRet = TRUE;
          break;
        }
      break;

    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       AnchorItem::InitDialog
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void AnchorItem::InitDialog
  (
  HWND hwnd
  )

  {
  // to make sure that we don't try to move an invalid window, we get a
  // legitmate use count for the bitmaps.  We are always guaranteed that
  // BMP_OOOO is legitmate, so we use that count to make sure only valid
  // windows are resized.

  int   i;
  int   nLegitimate = sm_bmp[0].nUseCount;
  HWND  hwndItem;
  RECT  rc;

  for(i=BMP_OOOO; i<=BMP_XXXX; i++)
    {
    if(nLegitimate==sm_bmp[i-BMP_OOOO].nUseCount)
      {
      hwndItem = GetDlgItem(hwnd, i);
      GetClientRect(hwndItem, &rc);
      MapWindowPoints(hwndItem, hwnd, (LPPOINT)&rc, 1);
      MoveWindow(hwndItem, rc.left, rc.top, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
      }
    }

  if(m_szTitle)
    SetWindowText(hwnd, m_szTitle);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       AnchorItem::DrawAnchorBitmap
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void AnchorItem::DrawAnchorBitmap
  (
  int               nItem,
  LPDRAWITEMSTRUCT  lpdis
  )

  {
  HDC     hdcMem = CreateCompatibleDC(lpdis->hDC);
  HBITMAP hbmpOld;

  if(nItem==m_nCurrentState)
    hbmpOld = (HBITMAP)SelectObject(hdcMem, sm_bmp[nItem].hbmpDown);
  else if(lpdis->itemState & ODS_SELECTED)
    hbmpOld = (HBITMAP)SelectObject(hdcMem, sm_bmp[nItem].hbmpDown);
  else if(lpdis->itemState & ODS_FOCUS  )
    hbmpOld = (HBITMAP)SelectObject(hdcMem, sm_bmp[nItem].hbmpUp);
  else
    hbmpOld = (HBITMAP)SelectObject(hdcMem, sm_bmp[nItem].hbmpUp);

  BitBlt(lpdis->hDC, 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, hdcMem, 0, 0, SRCCOPY);

  SelectObject(hdcMem, hbmpOld);
  DeleteDC(hdcMem);
  }
  
////////////////////////////////////////////////////////////////////////////
//
//  Method:       AnchorItem::DrawCurrentSample
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void AnchorItem::DrawCurrentSample
  (
  LPDRAWITEMSTRUCT  lpdis
  )

  {
  HDC     hdcMem = CreateCompatibleDC(lpdis->hDC);
  HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, sm_bmp[m_nCurrentState].hbmpDown);

  FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
  BitBlt(lpdis->hDC, 0, 0, BUTTON_WINDOW_WIDTH, BUTTON_WINDOW_HEIGHT,
         hdcMem, BUTTON_WINDOWOFFSET_LEFT, BUTTON_WINDOWOFFSET_TOP, SRCCOPY);

  SelectObject(hdcMem, hbmpOld);
  DeleteDC(hdcMem);
  }
  
////////////////////////////////////////////////////////////////////////////
//
//  Function:     OWLayPreferencesDlgProc
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export OWLayPreferencesDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL  fRet = FALSE;

  switch(uMsg)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitPreferencesDlg(hwnd);
      SetFocus(GetDlgItem(hwnd, IDOK));
      fRet = FALSE;
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, UpdatePreferences(hwnd));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          fRet = FALSE;
          break;

        default:
          fRet = FALSE;
          break;
        }
      break;

    default:
      fRet = FALSE;
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     InitPreferencesDlg
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void InitPreferencesDlg
  (
  HWND  hDlg
  )

  {
  PREF_DLG* pd = (PREF_DLG*)GetWindowLong(hDlg, DWL_USER);

  CheckDlgButton(hDlg, XB_PRSTWNDPOSITIONS, (pd->fLayPref & LAYPREF_PRSTWNDPOSITIONS) ? TRUE  : FALSE);

  CheckDlgButton(hDlg, XB_REQFULLENCLOSURE, (pd->fLayPref & LAYPREF_REQFULLENCLOSURE) ? TRUE  : FALSE);

  CheckDlgButton(hDlg, RB_SELECTIONTOOL,    (pd->fLayPref & LAYPREF_PRSTTOOL)         ? FALSE : TRUE);
  CheckDlgButton(hDlg, RB_PRSTTOOL,         (pd->fLayPref & LAYPREF_PRSTTOOL)         ? TRUE  : FALSE);

  if(pd->fLayPref & LAYPREF_PRSTMODE)
    {
    CheckDlgButton(hDlg, RB_FULLINFOMODE,   FALSE);
    CheckDlgButton(hDlg, RB_WYSIWYGMODE,    FALSE);
    CheckDlgButton(hDlg, RB_PRSTMODE,       TRUE);
    }
  else
    {
    CheckDlgButton(hDlg, RB_FULLINFOMODE,   (pd->fLayPref & LAYPREF_FULLINFOMODE)     ? TRUE  : FALSE);
    CheckDlgButton(hDlg, RB_WYSIWYGMODE,    (pd->fLayPref & LAYPREF_WYSIWYGMODE)      ? TRUE  : FALSE);
    CheckDlgButton(hDlg, RB_PRSTMODE,       FALSE);
    }

  CheckDlgButton(hDlg, RB_INCHES,           (pd->fLayPref & LAYPREF_INCHES)           ? TRUE  : FALSE);
  CheckDlgButton(hDlg, RB_MILLIMETERS,      (pd->fLayPref & LAYPREF_MILLIMETERS)      ? TRUE  : FALSE);
  CheckDlgButton(hDlg, RB_POINTS,           (pd->fLayPref & LAYPREF_POINTS)           ? TRUE  : FALSE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     UpdatePreferences
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL UpdatePreferences
  (
  HWND  hDlg
  )

  {
  PREF_DLG* pd = (PREF_DLG*)GetWindowLong(hDlg, DWL_USER);
  BOOL      rVal = FALSE;

  // check box for persistent window positions
  if((IsDlgButtonChecked(hDlg, XB_PRSTWNDPOSITIONS) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_PRSTWNDPOSITIONS) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_PRSTWNDPOSITIONS)
      pd->fLayPref &= ~LAYPREF_PRSTWNDPOSITIONS;
    else
      pd->fLayPref |= LAYPREF_PRSTWNDPOSITIONS;
    rVal = TRUE;
    }

  // check box for full enclosure requirement
  if((IsDlgButtonChecked(hDlg, XB_REQFULLENCLOSURE) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_REQFULLENCLOSURE) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_REQFULLENCLOSURE)
      pd->fLayPref &= ~LAYPREF_REQFULLENCLOSURE;
    else
      pd->fLayPref |= LAYPREF_REQFULLENCLOSURE;
    rVal = TRUE;
    }

  // radio group for tool
  if((IsDlgButtonChecked(hDlg, RB_PRSTTOOL) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_PRSTTOOL) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_PRSTTOOL)
      pd->fLayPref &= ~LAYPREF_PRSTTOOL;
    else
      pd->fLayPref |= LAYPREF_PRSTTOOL;
    rVal = TRUE;
    }

  // radio group for info mode
  if((IsDlgButtonChecked(hDlg, RB_FULLINFOMODE) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_FULLINFOMODE) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_FULLINFOMODE)
      pd->fLayPref &= ~LAYPREF_FULLINFOMODE;
    else
      pd->fLayPref |= LAYPREF_FULLINFOMODE;
    rVal = TRUE;
    }
  if((IsDlgButtonChecked(hDlg, RB_WYSIWYGMODE) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_WYSIWYGMODE) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_WYSIWYGMODE)
      pd->fLayPref &= ~LAYPREF_WYSIWYGMODE;
    else
      pd->fLayPref |= LAYPREF_WYSIWYGMODE;
    rVal = TRUE;
    }
  if((IsDlgButtonChecked(hDlg, RB_PRSTMODE) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_PRSTMODE) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_PRSTMODE)
      pd->fLayPref &= ~LAYPREF_PRSTMODE;
    else
      pd->fLayPref |= LAYPREF_PRSTMODE;
    rVal = TRUE;
    }

  // radio group for units
  if((IsDlgButtonChecked(hDlg, RB_INCHES) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_INCHES) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_INCHES)
      pd->fLayPref &= ~LAYPREF_INCHES;
    else
      pd->fLayPref |= LAYPREF_INCHES;
    rVal = TRUE;
    }
  if((IsDlgButtonChecked(hDlg, RB_MILLIMETERS) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_MILLIMETERS) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_MILLIMETERS)
      pd->fLayPref &= ~LAYPREF_MILLIMETERS;
    else
      pd->fLayPref |= LAYPREF_MILLIMETERS;
    rVal = TRUE;
    }
  if((IsDlgButtonChecked(hDlg, RB_POINTS) ? TRUE : FALSE) != ((pd->fLayPref & LAYPREF_POINTS) ? TRUE : FALSE))
    {
    if(pd->fLayPref & LAYPREF_POINTS)
      pd->fLayPref &= ~LAYPREF_POINTS;
    else
      pd->fLayPref |= LAYPREF_POINTS;
    rVal = TRUE;
    }


  return rVal;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     OWLayItemSizesDlgProc
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export OWLayItemSizesDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL  fRet;

  switch(uMsg)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitPosAndSizeDlg(hwnd);
      fRet = TRUE;
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          UpdatePosAndSize(hwnd);
          EndDialog(hwnd, TRUE);
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          fRet = FALSE;
          break;

        case ED_HPOSITION:
        case ED_WIDTH:
        case ED_VPOSITION:
        case ED_HEIGHT:
          if(HIWORD(lParam)==EN_KILLFOCUS)
            {
            POSSIZE_DLG*  psd = (POSSIZE_DLG*)GetWindowLong(hwnd, DWL_USER);
            SetItemValue(hwnd, wParam, GetItemValue(hwnd, wParam, psd->fUnits), psd->fUnits);
            fRet = TRUE;
            }
          else
            fRet = FALSE;
          break;

        case RB_HNOCHANGE:
        case RB_HSHRINK:
        case RB_HGROW:
        case RB_HDEFAULT:
          EnableWindow(GetDlgItem(hwnd, ST_HPOSITION), FALSE);
          EnableWindow(GetDlgItem(hwnd, ST_WIDTH),     FALSE);
          EnableWindow(GetDlgItem(hwnd, ED_HPOSITION), FALSE);
          EnableWindow(GetDlgItem(hwnd, ED_WIDTH),     FALSE);
          break;

        case RB_HENTER:
          EnableWindow(GetDlgItem(hwnd, ST_HPOSITION), TRUE);
          EnableWindow(GetDlgItem(hwnd, ST_WIDTH),     TRUE);
          EnableWindow(GetDlgItem(hwnd, ED_HPOSITION), TRUE);
          EnableWindow(GetDlgItem(hwnd, ED_WIDTH),     TRUE);
          break;

        case RB_VNOCHANGE:
        case RB_VSHRINK:
        case RB_VGROW:
        case RB_VDEFAULT:
          EnableWindow(GetDlgItem(hwnd, ST_VPOSITION), FALSE);
          EnableWindow(GetDlgItem(hwnd, ST_HEIGHT),    FALSE);
          EnableWindow(GetDlgItem(hwnd, ED_VPOSITION), FALSE);
          EnableWindow(GetDlgItem(hwnd, ED_HEIGHT),    FALSE);
          break;

        case RB_VENTER:
          EnableWindow(GetDlgItem(hwnd, ST_VPOSITION), TRUE);
          EnableWindow(GetDlgItem(hwnd, ST_HEIGHT),    TRUE);
          EnableWindow(GetDlgItem(hwnd, ED_VPOSITION), TRUE);
          EnableWindow(GetDlgItem(hwnd, ED_HEIGHT),    TRUE);
          break;


        default:
          fRet = FALSE;
          break;
        }
      break;

    default:
      fRet = FALSE;
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     InitPosAndSizeDlg
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void InitPosAndSizeDlg
  (
  HWND  hDlg
  )
  
  {
  POSSIZE_DLG*  psd = (POSSIZE_DLG*)GetWindowLong(hDlg, DWL_USER);

  if(psd->nSelections==1)
    {
    CheckDlgButton(hDlg, RB_HENTER, TRUE);
    CheckDlgButton(hDlg, RB_VENTER, TRUE);
    EnableWindow(GetDlgItem(hDlg, RB_HSHRINK), FALSE);
    EnableWindow(GetDlgItem(hDlg, RB_HGROW),   FALSE);
    EnableWindow(GetDlgItem(hDlg, RB_VSHRINK), FALSE);
    EnableWindow(GetDlgItem(hDlg, RB_VGROW),   FALSE);

    I_ASSERT(psd->wi);
    SetItemValue(hDlg, ED_HPOSITION, psd->wi->m_rcItem.left, psd->fUnits);
    SetItemValue(hDlg, ED_VPOSITION, psd->wi->m_rcItem.top, psd->fUnits);
    SetItemValue(hDlg, ED_WIDTH, psd->wi->m_rcItem.right - psd->wi->m_rcItem.left, psd->fUnits);
    SetItemValue(hDlg, ED_HEIGHT, psd->wi->m_rcItem.bottom - psd->wi->m_rcItem.top, psd->fUnits);
    }
  else
    {
    int idHRB = GetPrivateProfileInt("Sizes Dialog", "Horizontal", RB_HNOCHANGE, c_szAppBuilderIni);
    int idVRB = GetPrivateProfileInt("Sizes Dialog", "Vertical", RB_VNOCHANGE, c_szAppBuilderIni);

    if((idHRB != RB_HNOCHANGE)
            &&
       (idHRB != RB_HSHRINK)
            &&
       (idHRB != RB_HGROW)
            &&
       (idHRB != RB_HDEFAULT))
      {
      idHRB = RB_HNOCHANGE;
      }

    if((idVRB != RB_VNOCHANGE)
            &&
       (idVRB != RB_VSHRINK)
            &&
       (idVRB != RB_VGROW)
            &&
       (idVRB != RB_VDEFAULT))
      {
      idVRB = RB_VNOCHANGE;
      }

    CheckDlgButton(hDlg, idHRB, TRUE);
    CheckDlgButton(hDlg, idVRB, TRUE);

    EnableWindow(GetDlgItem(hDlg, RB_HENTER), FALSE);
    EnableWindow(GetDlgItem(hDlg, RB_VENTER), FALSE);

    EnableWindow(GetDlgItem(hDlg, ED_HPOSITION), FALSE);
    EnableWindow(GetDlgItem(hDlg, ED_WIDTH),     FALSE);
    EnableWindow(GetDlgItem(hDlg, ST_HPOSITION), FALSE);
    EnableWindow(GetDlgItem(hDlg, ST_WIDTH),     FALSE);

    EnableWindow(GetDlgItem(hDlg, ED_VPOSITION), FALSE);
    EnableWindow(GetDlgItem(hDlg, ED_HEIGHT),    FALSE);
    EnableWindow(GetDlgItem(hDlg, ST_VPOSITION), FALSE);
    EnableWindow(GetDlgItem(hDlg, ST_HEIGHT),    FALSE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     UpdatePosAndSize
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void UpdatePosAndSize
  (
  HWND  hDlg
  )
  
  {
  POSSIZE_DLG*  psd = (POSSIZE_DLG*)GetWindowLong(hDlg, DWL_USER);
  RECT          rc;
  int           idHRB, idVRB;


  I_ASSERT(psd->wi);
  CopyRect(&rc, &(psd->wi->m_rcItem));

  if(IsDlgButtonChecked(hDlg, RB_HSHRINK))
    {
    idHRB = RB_HSHRINK;
    psd->pItems->ShrinkHorz();
    }
  else if(IsDlgButtonChecked(hDlg, RB_HGROW))
    {
    idHRB = RB_HGROW;
    psd->pItems->GrowHorz();
    }
  else if(IsDlgButtonChecked(hDlg, RB_HDEFAULT))
    {
    idHRB = RB_HDEFAULT;
    psd->pItems->DefaultHorz();
    }
  else if(IsDlgButtonChecked(hDlg, RB_HENTER))
    {
    I_ASSERT(psd->nSelections==1);

    rc.left = GetItemValue(hDlg, ED_HPOSITION, psd->fUnits);
    rc.right = rc.left + GetItemValue(hDlg, ED_WIDTH, psd->fUnits);
    }
  else
    idHRB = RB_HNOCHANGE;

  if(IsDlgButtonChecked(hDlg, RB_VSHRINK))
    {
    idVRB = RB_VSHRINK;
    psd->pItems->ShrinkVert();
    }
  else if(IsDlgButtonChecked(hDlg, RB_VGROW))
    {
    idVRB = RB_VGROW;
    psd->pItems->GrowVert();
    }
  else if(IsDlgButtonChecked(hDlg, RB_VDEFAULT))
    {
    idVRB = RB_VDEFAULT;
    psd->pItems->DefaultVert();
    }
  else if(IsDlgButtonChecked(hDlg, RB_VENTER))
    {
    I_ASSERT(psd->nSelections==1);

    rc.top = GetItemValue(hDlg, ED_VPOSITION, psd->fUnits);
    rc.bottom = rc.top + GetItemValue(hDlg, ED_HEIGHT, psd->fUnits);
    }
  else
    idVRB = RB_VNOCHANGE;

  if((psd->nSelections==1) && !EqualRect(&rc, &psd->wi->m_rcItem))
    {
    psd->pItems->AdjustNewRect(psd->wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
    psd->pItems->Move(psd->wi, &rc);
    psd->pItems->GetWindow()->SetDirtyFlag();
    }

  if(psd->nSelections!=1)
    {
    char  szBuffer[25];

    wsprintf(szBuffer, "%i", idHRB);
    WritePrivateProfileString("Sizes Dialog", "Horizontal", szBuffer, c_szAppBuilderIni);

    wsprintf(szBuffer, "%i", idVRB);
    WritePrivateProfileString("Sizes Dialog", "Vertical", szBuffer, c_szAppBuilderIni);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     OWLayRenameDlgProc
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export OWLayRenameDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL  fRet = FALSE;

  switch(uMsg)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitRenameDlg(hwnd);
      fRet = FALSE;
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, UpdateName(hwnd));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          fRet = TRUE;
          break;

        default:
          break;
        }
      break;
  
    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     InitRenameDlg
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void InitRenameDlg
  (
  HWND  hDlg
  )
  
  {
  RENAME_DLG* rnd = (RENAME_DLG*)GetWindowLong(hDlg, DWL_USER);
  long        nLen;
  char        szName[MAX_NAMELEN];
  TYPEID      ti;

  // limit the amount of text that can be entered
  SendDlgItemMessage(hDlg, ED_OBJECTNAME, EM_LIMITTEXT, MAX_NAMELEN-1, 0L);

  nLen = AObjGetName(rnd->oiItem, szName, MAX_NAMELEN);
  if(nLen>MAX_NAMELEN)
    {
    I_ASSERT(0);
    }
  else
    {
    SetDlgItemText(hDlg, ST_OBJECTNAME, szName);
    SetDlgItemText(hDlg, ED_OBJECTNAME, szName);
    }

  ti = AObjGetType(rnd->oiItem);
  I_ASSERT(ti);
  nLen = ATypeGetName(ti, szName, MAX_NAMELEN);
  if(nLen>MAX_NAMELEN)
    {
    I_ASSERT(0);
    }
  else
    SetDlgItemText(hDlg, ST_TYPENAME, szName);

  SetFocus(GetDlgItem(hDlg, ED_OBJECTNAME));
  SendDlgItemMessage(hDlg, ED_OBJECTNAME, EM_SETSEL, 1, MAKELPARAM(0, -1));
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     UpdateName
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL UpdateName
  (
  HWND  hDlg
  )
  
  {
  RENAME_DLG* rnd           = (RENAME_DLG*)GetWindowLong(hDlg, DWL_USER);
  char        szName[MAX_NAMELEN];
  char        szDlgName[MAX_NAMELEN];
  BOOL        fChanged      = FALSE;
  #ifdef AW_I_DEBUG
  int         nLen          = (int)
  #endif

  AObjGetName(rnd->oiItem, szName, MAX_NAMELEN);
  I_ASSERT(nLen<=MAX_NAMELEN);

  GetDlgItemText(hDlg, ED_OBJECTNAME, szDlgName, MAX_NAMELEN);
  szDlgName[MAX_NAMELEN-1] = '\0';
  if(strcmp(szName, szDlgName))
    {
    fChanged = TRUE;
    AObjSetName(rnd->oiItem, szDlgName);
    ReloadObjectListWindow();
    }

  return fChanged;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     OWLayWndSizesDlgProc
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export OWLayWndSizesDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL  fRet = FALSE;

  switch(uMsg)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitSizesDlg(hwnd);
      fRet = FALSE;
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, UpdateSizes(hwnd));
          return TRUE;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          return TRUE;

        case IDHELP:
          break;

        case RB_LIMITED:
        case RB_UNLIMITED:
          if(IsDlgButtonChecked(hwnd, RB_LIMITED))
            {
            EnableWindow(GetDlgItem(hwnd, ST_MAXHEIGHT), TRUE);
            EnableWindow(GetDlgItem(hwnd, ED_MAXHEIGHT), TRUE);
            EnableWindow(GetDlgItem(hwnd, ST_MAXWIDTH), TRUE);
            EnableWindow(GetDlgItem(hwnd, ED_MAXWIDTH), TRUE);
            }
          else
            {
            EnableWindow(GetDlgItem(hwnd, ST_MAXHEIGHT), FALSE);
            EnableWindow(GetDlgItem(hwnd, ED_MAXHEIGHT), FALSE);
            EnableWindow(GetDlgItem(hwnd, ST_MAXWIDTH), FALSE);
            EnableWindow(GetDlgItem(hwnd, ED_MAXWIDTH), FALSE);
            }
          return TRUE;

        case ED_LEFT:
        case ED_TOP:
        case ED_RIGHT:
        case ED_BOTTOM:
        case ED_MINHEIGHT:
        case ED_MINWIDTH:
        case ED_MAXHEIGHT:
        case ED_MAXWIDTH:
        case ED_SCROLLHEIGHT:
        case ED_SCROLLWIDTH:
          if(HIWORD(lParam)==EN_KILLFOCUS)
            {
            WNDSIZES_DLG* wsd = (WNDSIZES_DLG*)GetWindowLong(hwnd, DWL_USER);
            SetItemValue(hwnd, wParam, GetItemValue(hwnd, wParam, wsd->fUnits), wsd->fUnits);
            return TRUE;
            }
          break;

        default:
          break;
        }
      break;

    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     InitSizesDlg
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

void InitSizesDlg
  (
  HWND  hDlg
  )

  {
  WNDSIZES_DLG* wsd = (WNDSIZES_DLG*)GetWindowLong(hDlg, DWL_USER);

  // set the startup location group
  SetItemValue(hDlg, ED_LEFT,   wsd->rcClient.left,   wsd->fUnits);
  SetItemValue(hDlg, ED_TOP,    wsd->rcClient.top,    wsd->fUnits);
  SetItemValue(hDlg, ED_RIGHT,  wsd->rcClient.right,  wsd->fUnits);
  SetItemValue(hDlg, ED_BOTTOM, wsd->rcClient.bottom, wsd->fUnits);

  // set the minimum size group
  SetItemValue(hDlg, ED_MINHEIGHT, wsd->ptMin.y, wsd->fUnits);
  SetItemValue(hDlg, ED_MINWIDTH,  wsd->ptMin.x, wsd->fUnits);

  // set the maximum size group
  SetItemValue(hDlg, ED_MAXHEIGHT, wsd->ptMax.y, wsd->fUnits);
  SetItemValue(hDlg, ED_MAXWIDTH,  wsd->ptMax.x, wsd->fUnits);
  if(wsd->fLimit)
    CheckDlgButton(hDlg, RB_LIMITED, TRUE);
  else
    {
    CheckDlgButton(hDlg, RB_UNLIMITED, TRUE);
    EnableWindow(GetDlgItem(hDlg, ST_MAXHEIGHT), FALSE);
    EnableWindow(GetDlgItem(hDlg, ED_MAXHEIGHT), FALSE);
    EnableWindow(GetDlgItem(hDlg, ST_MAXWIDTH), FALSE);
    EnableWindow(GetDlgItem(hDlg, ED_MAXWIDTH), FALSE);
    }

  // set the scroll area group
  SetItemValue(hDlg, ED_SCROLLHEIGHT, wsd->ptScroll.y, wsd->fUnits);
  SetItemValue(hDlg, ED_SCROLLWIDTH,  wsd->ptScroll.x, wsd->fUnits);

  if(!(wsd->style & WF_HORZSCROLL))
    {
    EnableWindow(GetDlgItem(hDlg, ST_SCROLLWIDTH), FALSE);
    EnableWindow(GetDlgItem(hDlg, ED_SCROLLWIDTH), FALSE);
    }
  if(!(wsd->style & WF_VERTSCROLL))
    {
    EnableWindow(GetDlgItem(hDlg, ST_SCROLLHEIGHT), FALSE);
    EnableWindow(GetDlgItem(hDlg, ED_SCROLLHEIGHT), FALSE);
    if(!(wsd->style & WF_HORZSCROLL))
      EnableWindow(GetDlgItem(hDlg, PB_ENCLOSEITEMS), FALSE);
    }

  SetFocus(GetDlgItem(hDlg, ED_LEFT));
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     UpdateSizes
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL UpdateSizes
  (
  HWND  hDlg
  )

  {
  WNDSIZES_DLG* wsd = (WNDSIZES_DLG*)GetWindowLong(hDlg, DWL_USER);
  BOOL          fChanged = FALSE;
  RECT          rc;
  POINT         pt;

  // get the startup location group
  rc.left   = GetItemValue(hDlg, ED_LEFT,   wsd->fUnits);
  rc.top    = GetItemValue(hDlg, ED_TOP,    wsd->fUnits);
  rc.right  = GetItemValue(hDlg, ED_RIGHT,  wsd->fUnits);
  rc.bottom = GetItemValue(hDlg, ED_BOTTOM, wsd->fUnits);
  if(!EqualRect(&wsd->rcClient, &rc))
    {
    fChanged = TRUE;
    CopyRect(&wsd->rcClient, &rc);
    }

  // get the minimum size group
  pt.y = GetItemValue(hDlg, ED_MINHEIGHT, wsd->fUnits);
  pt.x = GetItemValue(hDlg, ED_MINWIDTH,  wsd->fUnits);
  if(!EqualPoint(&wsd->ptMin, &pt))
    {
    fChanged = TRUE;
    CopyPoint(&wsd->ptMin, &pt);
    }

  // get the maximum size group
  pt.y = GetItemValue(hDlg, ED_MAXHEIGHT, wsd->fUnits);
  pt.x = GetItemValue(hDlg, ED_MAXWIDTH,  wsd->fUnits);
  if(!EqualPoint(&wsd->ptMax, &pt))
    {
    fChanged = TRUE;
    CopyPoint(&wsd->ptMax, &pt);
    }
  if(wsd->fLimit != IsDlgButtonChecked(hDlg, RB_LIMITED))
    {
    fChanged = TRUE;
    wsd->fLimit = IsDlgButtonChecked(hDlg, RB_LIMITED);
    }

  // get the scroll area group
  pt.y = GetItemValue(hDlg, ED_SCROLLHEIGHT, wsd->fUnits);
  pt.x = GetItemValue(hDlg, ED_SCROLLWIDTH,  wsd->fUnits);
  if(!EqualPoint(&wsd->ptScroll, &pt))
    {
    fChanged = TRUE;
    CopyPoint(&wsd->ptScroll, &pt);
    }

  return fChanged;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Function:     OWLayWndStyleDlgProc
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export OWLayWndStyleDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL    fRet = FALSE;
  HWND    hwndSample;

  switch(uMsg)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitStyleDlg(hwnd);
      fRet = TRUE;
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, UpdateWndStyle(hwnd));
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          break;

        case IDHELP:
          break;

        case XB_TITLEBAR:
        case XB_SYSTEMMENU:
        case XB_MAXIMIZEBUTTON:
        case XB_MINIMIZEBUTTON:
        case XB_HORIZONTALSCROLL:
        case XB_VERTICALSCROLL:
        case XB_RESIZEBORDER:
        case XB_DIALOGFRAME:
        case ED_WINDOWTITLE:
          DrawSampleWindow(hwnd);
          fRet = TRUE;
          break;

        default:
          break;
        }
      break;

    case WM_NCACTIVATE:
      hwndSample = (HWND)GetProp(hwnd, c_szSampleWindow);
      if(hwndSample)
        SendMessage(hwndSample, uMsg, wParam, lParam);
      break;

    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     InitStyleDlg
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void InitStyleDlg
  (
  HWND  hDlg
  )

  {
  WNDSTYLE_DLG* wsd = (WNDSTYLE_DLG*)GetWindowLong(hDlg, DWL_USER);

  SetDlgItemText(hDlg, ED_WINDOWTITLE, wsd->szTitle);

  FillMenuBarList(hDlg);

  CheckDlgButton(hDlg, XB_TITLEBAR,         (wsd->style & WF_TITLEBAR)      ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_SYSTEMMENU,       (wsd->style & WF_SYSMENU)       ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_MAXIMIZEBUTTON,   (wsd->style & WF_MAXBUTTON)     ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_MINIMIZEBUTTON,   (wsd->style & WF_MINBUTTON)     ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_HORIZONTALSCROLL, (wsd->style & WF_HORZSCROLL)    ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_VERTICALSCROLL,   (wsd->style & WF_VERTSCROLL)    ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_RESIZEBORDER,     (wsd->style & WF_RESIZEBORDER)  ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_DIALOGFRAME,      (wsd->style & WF_DIALOGFRAME)   ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_MODAL,            (wsd->style & WF_MODAL)         ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_ALWAYSONTOP,      (wsd->style & WF_ALWAYSONTOP)   ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_INITIALLYVISIBLE, (wsd->style & WF_INITVISIBLE)   ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_DRAWSMOOTHLY,     (wsd->style & WF_DRAWSMOOTH)    ? TRUE : FALSE);
  CheckDlgButton(hDlg, XB_QUITONCLOSE,      (wsd->style & WF_QUITONCLOSE)   ? TRUE : FALSE);

  DrawSampleWindow(hDlg);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     FillMenuBarList
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void FillMenuBarList
  (
  HWND  hDlg
  )

  {
  WNDSTYLE_DLG* wsd = (WNDSTYLE_DLG*)GetWindowLong(hDlg, DWL_USER);
  long          lGetTypeKey = AOBJ_GETFIRSTKEY;
  long          lGetObjKey;
  TYPEID        ti;
  OBJECTID      oi;
  char          szName[50];
  long          idx;

  // add the "(None)" string to the list
  idx = SendDlgItemMessage(hDlg, CB_MENUBAR, CB_ADDSTRING, (WPARAM)0, (LPARAM)"(None)");
  if(idx==CB_ERR || idx==CB_ERRSPACE)
    return;
  SendDlgItemMessage(hDlg, CB_MENUBAR, CB_SETITEMDATA, (WPARAM)idx, (LPARAM)0L);
  if(wsd->oiMenu==NULL)
    SendDlgItemMessage(hDlg, CB_MENUBAR, CB_SETCURSEL, (WPARAM)idx, 0L);

  while((ti=ATypeGetNext(&lGetTypeKey))!=NULL)
    {
    if(AEvtCheckForMethod(ti, OEVENT_GETMENUHANDLE))
      {
      lGetObjKey = AOBJ_GETFIRSTKEY;
      while((oi=AObjGetNextObject(ti, &lGetObjKey))!=NULL)
        {
        AObjGetName(oi, szName, 50);
        idx = SendDlgItemMessage(hDlg, CB_MENUBAR, CB_ADDSTRING, (WPARAM)0, (LPARAM)szName);
        if(idx==CB_ERR || idx==CB_ERRSPACE)
          continue;

        SendDlgItemMessage(hDlg, CB_MENUBAR, CB_SETITEMDATA, (WPARAM)idx, (LPARAM)oi);
        if(wsd->oiMenu==oi)
          SendDlgItemMessage(hDlg, CB_MENUBAR, CB_SETCURSEL, (WPARAM)idx, 0L);
        }
      }
    }

  idx = SendDlgItemMessage(hDlg, CB_MENUBAR, CB_GETCURSEL, 0, 0L);
  if(idx==CB_ERR)
    { // no item selected -- selecte the "(None)" string
    SendDlgItemMessage(hDlg, CB_MENUBAR, CB_SETCURSEL, 0, 0L);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     DrawSampleWindow
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void DrawSampleWindow
  (
  HWND  hDlg
  )

  {
  DWORD dwExStyle = 0L;
  DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER;
  RECT  rc;
  char  szTitle[80];
  HWND  hwndSampleWindow = (HWND)GetProp(hDlg, c_szSampleWindow);
  if(hwndSampleWindow)
    DestroyWindow(hwndSampleWindow);

  // get the current window style
  if(IsDlgButtonChecked(hDlg, XB_TITLEBAR))
    dwStyle |= WS_CAPTION;
  if(IsDlgButtonChecked(hDlg, XB_SYSTEMMENU))
    dwStyle |= WS_SYSMENU;
  if(IsDlgButtonChecked(hDlg, XB_MAXIMIZEBUTTON))
    dwStyle |= WS_MAXIMIZEBOX;
  if(IsDlgButtonChecked(hDlg, XB_MINIMIZEBUTTON))
    dwStyle |= WS_MINIMIZEBOX;
  if(IsDlgButtonChecked(hDlg, XB_HORIZONTALSCROLL))
    dwStyle |= WS_HSCROLL;
  if(IsDlgButtonChecked(hDlg, XB_VERTICALSCROLL))
    dwStyle |= WS_VSCROLL;
  if(IsDlgButtonChecked(hDlg, XB_RESIZEBORDER))
    dwStyle |= WS_THICKFRAME;
  if(IsDlgButtonChecked(hDlg, XB_DIALOGFRAME))
    {
    dwExStyle |= WS_EX_DLGMODALFRAME;
    dwStyle &= ~(WS_HSCROLL | WS_VSCROLL);
    EnableWindow(GetDlgItem(hDlg, ST_MENUBAR), FALSE);
    EnableWindow(GetDlgItem(hDlg, CB_MENUBAR), FALSE);
    }
  else
    {
    EnableWindow(GetDlgItem(hDlg, ST_MENUBAR), TRUE);
    EnableWindow(GetDlgItem(hDlg, CB_MENUBAR), TRUE);
    }

  // create the sample window
  GetClientRect(GetDlgItem(hDlg, ST_SAMPLEWINDOW), &rc);
  GetDlgItemText(hDlg, ED_WINDOWTITLE, szTitle, 80);

  hwndSampleWindow = CreateWindowEx
    (
    dwExStyle,
    c_szSampleClassName,
    szTitle,
    dwStyle,
    rc.left,
    rc.top,
    rc.right-rc.left,
    rc.bottom-rc.top,
    GetDlgItem(hDlg, ST_SAMPLEWINDOW),
    NULL,
    g_hInstance,
    NULL
    );
  SendMessage(hwndSampleWindow, WM_NCACTIVATE, TRUE, 0L);

  // store the sample window handle for future use
  SetProp(hDlg, c_szSampleWindow, hwndSampleWindow);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     UpdateWndStyle
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

BOOL UpdateWndStyle
  (
  HWND  hDlg
  )

  {
  OBJECTID      oiMenu;
  BOOL          bChanged = FALSE;
  char          szTitle[256];
  long          idx;
  WNDSTYLE_DLG* wsd = (WNDSTYLE_DLG*)GetWindowLong(hDlg, DWL_USER);

  GetDlgItemText(hDlg, ED_WINDOWTITLE, szTitle, 256);
  idx = SendDlgItemMessage(hDlg, CB_MENUBAR, CB_GETCURSEL, 0, 0L);
  if(idx!=CB_ERR)
    {
    oiMenu = (OBJECTID)SendDlgItemMessage(hDlg, CB_MENUBAR, CB_GETITEMDATA, (WPARAM)idx, 0L);
    if((long)oiMenu == CB_ERR)
      oiMenu = NULL;
    }

  if((((wsd->style & WF_TITLEBAR) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_TITLEBAR))
        ||
     (((wsd->style & WF_SYSMENU) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_SYSTEMMENU))
        ||
     (((wsd->style & WF_MAXBUTTON) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_MAXIMIZEBUTTON))
        ||
     (((wsd->style & WF_MINBUTTON) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_MINIMIZEBUTTON))
        ||
     (((wsd->style & WF_HORZSCROLL) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_HORIZONTALSCROLL))
        ||
     (((wsd->style & WF_VERTSCROLL) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_VERTICALSCROLL))
        ||
     (((wsd->style & WF_RESIZEBORDER) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_RESIZEBORDER))
        ||
     (((wsd->style & WF_DIALOGFRAME) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_DIALOGFRAME))
        ||
     (((wsd->style & WF_MODAL) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_MODAL))
        ||
     (((wsd->style & WF_ALWAYSONTOP) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_ALWAYSONTOP))
        ||
     (((wsd->style & WF_INITVISIBLE) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_INITIALLYVISIBLE))
        ||
     (((wsd->style & WF_DRAWSMOOTH) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_DRAWSMOOTHLY))
        ||
     (((wsd->style & WF_QUITONCLOSE) ? TRUE : FALSE)!=IsDlgButtonChecked(hDlg, XB_QUITONCLOSE))
        ||
     (strcmp(szTitle, wsd->szTitle))
        ||
     (oiMenu!=wsd->oiMenu))
    {
    bChanged = TRUE;
    }

  if(bChanged)
    {
    wsd->style = 0L;
    if(IsDlgButtonChecked(hDlg, XB_TITLEBAR))         wsd->style |= WF_TITLEBAR;
    if(IsDlgButtonChecked(hDlg, XB_SYSTEMMENU))       wsd->style |= WF_SYSMENU;
    if(IsDlgButtonChecked(hDlg, XB_MAXIMIZEBUTTON))   wsd->style |= WF_MAXBUTTON;
    if(IsDlgButtonChecked(hDlg, XB_MINIMIZEBUTTON))   wsd->style |= WF_MINBUTTON;
    if(IsDlgButtonChecked(hDlg, XB_HORIZONTALSCROLL)) wsd->style |= WF_HORZSCROLL;
    if(IsDlgButtonChecked(hDlg, XB_VERTICALSCROLL))   wsd->style |= WF_VERTSCROLL;
    if(IsDlgButtonChecked(hDlg, XB_RESIZEBORDER))     wsd->style |= WF_RESIZEBORDER;
    if(IsDlgButtonChecked(hDlg, XB_DIALOGFRAME))      wsd->style |= WF_DIALOGFRAME;
    if(IsDlgButtonChecked(hDlg, XB_MODAL))            wsd->style |= WF_MODAL;
    if(IsDlgButtonChecked(hDlg, XB_ALWAYSONTOP))      wsd->style |= WF_ALWAYSONTOP;
    if(IsDlgButtonChecked(hDlg, XB_INITIALLYVISIBLE)) wsd->style |= WF_INITVISIBLE;
    if(IsDlgButtonChecked(hDlg, XB_DRAWSMOOTHLY))     wsd->style |= WF_DRAWSMOOTH;
    if(IsDlgButtonChecked(hDlg, XB_QUITONCLOSE))      wsd->style |= WF_QUITONCLOSE;

    strncpy(wsd->szTitle, szTitle, 255);
    wsd->szTitle[255]=0;

    wsd->oiMenu = oiMenu;
    }

  DestroyWindow((HWND)GetProp(hDlg, c_szSampleWindow));
  RemoveProp(hDlg, c_szSampleWindow);

  return bChanged;
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     PaintSampleWindow
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void PaintSampleWindow
  (
  HWND  hwnd
  )

  {
  PAINTSTRUCT   ps;
  RECT          rc;
  HDC           hdc      = BeginPaint(hwnd, &ps);
  int           nOldMode = SetBkMode(hdc, TRANSPARENT);
  COLORREF      clrOldFG = SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

  // draw the sample text in the window
  GetClientRect(hwnd, &rc);
  DrawText(hdc, "Sample Window", -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

  SetBkMode(hdc, nOldMode);
  SetTextColor(hdc, clrOldFG);
  EndPaint(hwnd, &ps);
  }

// OWLaySampleWindowProc ///////////////////////////////////////////////////

LRESULT CALLBACK _export OWLaySampleWndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL    fHandled;
  LRESULT lRet;

  switch (uMsg)
    {
    case WM_PAINT:
      PaintSampleWindow(hwnd);
      lRet = 0L;
      fHandled = TRUE;
      break;

    case WM_SETCURSOR:
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      lRet = 0L;
      fHandled = TRUE;
      break;

    case WM_MOUSEACTIVATE:
      lRet = MA_ACTIVATEANDEAT;
      fHandled = TRUE;
      break;

    default:
      fHandled = FALSE;
      break;
    }

  return fHandled ? lRet : DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

////////////////////////////////////////////////////////////////////////////
