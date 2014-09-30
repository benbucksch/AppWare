////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          SHP_ATTR.CPP
//
// AUTHOR:        Kevin Smart
//
// DESCRIPTION:   Contains implementation for shape attributes
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <bwcc.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <commdlg.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <op_wnd.h>

#include "o_upsf.h"
#include "shp_attr.h"
#include "tmp.h"

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// STATIC DEFINITIONS
//
////////////////////////////////////////////////////////////////////////////

COLORREF ColorAttrib::sm_clr[] =
        {
        0xFFFFFFFFL,        // default
        COLOR_TRANSPARENT,  // transparent
        0x00000000L,        // black          (black)
        0x00000080L,        // dark red       (maroon)
        0x00008000L,        // dark green     (green)
        0x00008080L,        // dark yellow    (olive)
        0x00800000L,        // dark blue      (navy)
        0x00800080L,        // dark purple    (purple)
        0x00808000L,        // dark cyan      (teal)
        0x00808080L,        // dark gray      (gray)
        0x00C0C0C0L,        // light gray     (silver)
        0x000000FFL,        // red            (red)
        0x0000FF00L,        // green          (lime)
        0x0000FFFFL,        // yellow         (yellow)
        0x00FF0000L,        // blue           (blue)
        0x00FF00FFL,        // purple         (fuschia)
        0x00FFFF00L,        // cyan           (aqua)
        0x00FFFFFFL         // white          (white)
        };

char* ColorAttrib::sm_szClrName[] =
        {
        "Default",
        "Transparent",
        "Black",
        "Maroon",
        "Green",
        "Olive",
        "Navy",
        "Purple",
        "Teal",
        "Gray",
        "Silver",
        "Red",
        "Lime",
        "Yellow",
        "Blue",
        "Fuschia",
        "Aqua",
        "White"
        };

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      Constructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ColorAttrib::ColorAttrib
  (
  COLORREF  clrFG,
  COLORREF  clrBG
  )

  {
  m_clrFG = clrFG;
  m_clrBG = clrBG;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ColorAttrib::~ColorAttrib
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      SetFGColor
//
// DESCRIPTION: This method will set the foreground color.  This will return
//              TRUE if the color actually changes.
//
////////////////////////////////////////////////////////////////////////////

BOOL ColorAttrib::SetFGColor
  (
  COLORREF  clrFG
  )

  {
  BOOL  fRet;

  if(m_clrFG==clrFG)
    fRet = FALSE;
  else
    {
    m_clrFG = clrFG;
    fRet = TRUE;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      SetBGColor
//
// DESCRIPTION: This method will set the background color.  This will return
//              TRUE if the color actually changes.
//
////////////////////////////////////////////////////////////////////////////

BOOL ColorAttrib::SetBGColor
  (
  COLORREF  clrBG
  )

  {
  BOOL  fRet;

  if(m_clrBG==clrBG)
    fRet = FALSE;
  else
    {
    m_clrBG = clrBG;
    fRet = TRUE;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      FGColor
//
// DESCRIPTION: This method will return the foreground color
//
////////////////////////////////////////////////////////////////////////////

COLORREF ColorAttrib::FGColor
  (
  )

  {
  COLORREF  clr;

  if(m_clrFG==COLOR_WINDOWFOREGROUND)
    clr = GetSysColor(COLOR_WINDOWTEXT);
  else
    clr = (m_clrFG & 0x00FFFFFFL);

  return clr;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      BGColor
//
// DESCRIPTION: This method will return the background color
//
////////////////////////////////////////////////////////////////////////////

COLORREF ColorAttrib::BGColor
  (
  )

  {
  COLORREF  clr;

  if((m_clrBG==COLOR_WINDOWBACKGROUND) || (m_clrBG==COLOR_TRANSPARENT))
    clr = GetSysColor(COLOR_WINDOW);
  else
    clr = (m_clrBG & 0x00FFFFFFL);

  return clr;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      Export
//
// DESCRIPTION: This will export the colors to the UPSF file.
//
////////////////////////////////////////////////////////////////////////////

void ColorAttrib::Export
  (
  )

  {
  ExportColor("FGCOLOR",  0L, FGColor());
  ExportBool ("FGTRANSP", 0L, IsFGTransparent());
  ExportBool ("FGDEFALT", 0L, IsFGDefault());
  ExportColor("BGCOLOR",  0L, BGColor());
  ExportBool ("BGTRANSP", 0L, IsBGTransparent());
  ExportBool ("BGDEFALT", 0L, IsBGDefault());
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      Import
//
// DESCRIPTION: This will import the colors from the UPSF file.
//
////////////////////////////////////////////////////////////////////////////

void ColorAttrib::Import
  (
  )

  {
  BOOL      fVal = FALSE;
  COLORREF  clr;

  if((ImportBool ("FGTRANSP", 0L, &fVal)==AUPSF_NOERROR) && fVal)
    SetFGTransparent();
  else if((ImportBool ("FGDEFALT", 0L, &fVal)==AUPSF_NOERROR) && fVal)
    SetFGDefault();
  else if(ImportColor("FGCOLOR",  0L, &clr)==AUPSF_NOERROR)
    SetFGColor(clr);

  fVal = FALSE;
  if((ImportBool ("BGTRANSP", 0L, &fVal)==AUPSF_NOERROR) && fVal)
    SetBGTransparent();
  else if((ImportBool ("BGDEFALT", 0L, &fVal)==AUPSF_NOERROR) && fVal)
    SetBGDefault();
  else if(ImportColor("BGCOLOR",  0L, &clr)==AUPSF_NOERROR)
    SetBGColor(clr);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      FillColorComboBox
//
// DESCRIPTION: This will fill in an owner draw combobox with the system
//              colors.
//
////////////////////////////////////////////////////////////////////////////

BOOL ColorAttrib::FillColorComboBox
  (
  HWND      hwndCombo,
  COLORREF  clr,
  BOOL      fFG,
  BOOL      fWantTransparent
  )

  {
  BOOL  fRet = TRUE;
  int   i, idx;
  int   nCount = sizeof(sm_clr) / sizeof(sm_clr[0]);

  // overwrite the default color
  sm_clr[0] = fFG ? COLOR_WINDOWFOREGROUND : COLOR_WINDOWBACKGROUND;

  SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0L);
  for(i=0; i<nCount; i++)
    {
    idx = i;
    if(fWantTransparent || i!=1)
      {
      if(!fWantTransparent && i>1)
        idx--;
      SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)sm_szClrName[i]);
      SendMessage(hwndCombo, CB_SETITEMDATA, (WPARAM)idx, (LPARAM)sm_clr[i]);
      if(sm_clr[i]==clr)
        SendMessage(hwndCombo, CB_SETCURSEL, (WPARAM)idx, 0L);
      }
    }

  if(SendMessage(hwndCombo, CB_GETCURSEL, 0, 0L)==CB_ERR)
    SendMessage(hwndCombo, CB_SETCURSEL, 0, 0L);

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      DrawColorComboBox
//
// DESCRIPTION: This will draw an item for a color combo box.
//
////////////////////////////////////////////////////////////////////////////

void ColorAttrib::DrawColorComboBox
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  HBRUSH    hbr, hbrHighlight;
  COLORREF  clr, clrOldFG, clrOldBG;
  RECT      rc;
  char      szClrName[25];

  clr = (COLORREF)SendDlgItemMessage(hwnd, lpdi->CtlID, CB_GETITEMDATA, lpdi->itemID, 0);
  if(clr==COLOR_WINDOWFOREGROUND)
    clr = GetSysColor(COLOR_WINDOWTEXT);
  else if((clr==COLOR_WINDOWBACKGROUND) || (clr==COLOR_TRANSPARENT))
    clr = GetSysColor(COLOR_WINDOW);

  hbr = CreateSolidBrush(clr);

  rc.top = lpdi->rcItem.top+1;
  rc.bottom = lpdi->rcItem.bottom-1;
  rc.left = lpdi->rcItem.left+1;
  rc.right = lpdi->rcItem.left+25;

  if (lpdi->itemState & (ODS_SELECTED|ODS_FOCUS))
    {
    hbrHighlight = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
    clrOldFG = SetTextColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
    clrOldBG = SetBkColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHT));
    }
  else
    {
    hbrHighlight = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    clrOldFG = SetTextColor(lpdi->hDC, GetSysColor(COLOR_WINDOWTEXT));
    clrOldBG = SetBkColor(lpdi->hDC, GetSysColor(COLOR_WINDOW));
    }

  FillRect(lpdi->hDC, &lpdi->rcItem, hbrHighlight );
  FillRect(lpdi->hDC, &rc, hbr);
  FrameRect(lpdi->hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

  rc.left = rc.right + 4;
  rc.top = lpdi->rcItem.top;
  rc.right = lpdi->rcItem.right;
  rc.bottom = lpdi->rcItem.bottom;

  SendDlgItemMessage(hwnd, lpdi->CtlID, CB_GETLBTEXT, lpdi->itemID, (LPARAM)szClrName);
  DrawText(lpdi->hDC, szClrName, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

  if (lpdi->itemState & ODS_FOCUS)
    DrawFocusRect(lpdi->hDC, &lpdi->rcItem);

  SetTextColor(lpdi->hDC, clrOldFG);
  SetBkColor(lpdi->hDC, clrOldBG);
  DeleteObject(hbr);
  DeleteObject(hbrHighlight);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ColorAttrib
//
// METHOD:      MeasureComboBox
//
// DESCRIPTION: This will measure an item for a color combo box.
//
////////////////////////////////////////////////////////////////////////////

void ColorAttrib::MeasureComboBox
  (
  HWND                hwnd,
  LPMEASUREITEMSTRUCT lpmi
  )

  {
  TEXTMETRIC  tm;
  HDC     hdc = GetDC(hwnd);
  HFONT   hfont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0L);
  HFONT   hfontOld = (HFONT)SelectObject(hdc, hfont);

  GetTextMetrics(hdc, &tm);
  SelectObject(hdc, hfontOld);
  ReleaseDC(hwnd, hdc) ;

  lpmi->itemHeight = tm.tmHeight+1;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// STATIC DEFINITIONS
//
////////////////////////////////////////////////////////////////////////////

int LineAttrib::sm_nLineStyle[] =
        {
        PS_SOLID,
        PS_DASH,
        PS_DOT,
        PS_DASHDOT,
        PS_DASHDOTDOT,
        PS_3DIN,
        PS_3DOUT,
        PS_NULL
        };

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      Constructor
//
// DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

LineAttrib::LineAttrib
  (
  COLORREF  clrFG,
  COLORREF  clrBG,
  int       nWidth,
  int       nStyle
  ) : ColorAttrib(clrFG, clrBG)

  {
  m_nWidth = nWidth;
  m_nStyle = nStyle;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will fill in line attributes based on stored
//              informaiton.
//
////////////////////////////////////////////////////////////////////////////

LineAttrib::LineAttrib
  (
  ATTR_LINE*  line
  ) : ColorAttrib()

  {
  if(line && (line->sVer==ATTR_VERSION))
    {
    SetFGColor(line->clrFG);
    SetBGColor(line->clrBG);
    m_nWidth = line->nWidth;
    m_nStyle = line->nStyle;
    }
  else
    {
    SetFGColor(COLOR_WINDOWFOREGROUND);
    SetBGColor(COLOR_TRANSPARENT);
    m_nWidth = 0;
    m_nStyle = PS_SOLID;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

LineAttrib::~LineAttrib
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      SetLineWidth
//
// DESCRIPTION: This method will return TRUE if the width changes.
//
////////////////////////////////////////////////////////////////////////////

BOOL LineAttrib::SetLineWidth
  (
  int nWidth
  )
  
  {
  BOOL  fRet;

  if(m_nWidth==nWidth)
    fRet = FALSE;
  else
    {
    m_nWidth = nWidth;
    fRet = TRUE;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      SetLineStyle
//
// DESCRIPTION: This method will return TRUE if the style changes.
//
////////////////////////////////////////////////////////////////////////////

BOOL LineAttrib::SetLineStyle
  (
  int nStyle
  )
  
  {
  BOOL  fRet;

  if(m_nStyle==nStyle)
    fRet = FALSE;
  else
    {
    m_nStyle = nStyle;
    fRet = TRUE;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      CreatePen
//
// DESCRIPTION: This method will create a pen based on the line attributes
//
////////////////////////////////////////////////////////////////////////////

HPEN LineAttrib::CreatePen
  (
  OBJECTID  oiWnd,
  HDC       hdc
  )

  {
  int   nStyle = m_nStyle;
  int   nWidth = m_nWidth;
  HPEN  hpenRet;

  if(nWidth>0)
    {
    if(oiWnd)
      ; // currently do nothing -- the window should give an API to set the value
    else if(hdc)
      nWidth = (double)nWidth  * (double)GetDeviceCaps(hdc, LOGPIXELSY) / 254. + 0.5;
    else
      {
      HDC hdcLocal = CreateDC("DISPLAY", NULL, NULL, NULL);
      nWidth = (double)nWidth  * (double)GetDeviceCaps(hdcLocal, LOGPIXELSY) / 254. + 0.5;
      DeleteDC(hdcLocal);
      }
    nStyle = PS_INSIDEFRAME;

    hpenRet = ::CreatePen(nStyle, nWidth, FGColor());
    }
  else
    {
    if((nStyle==PS_3DIN) || (nStyle==PS_3DOUT))
      hpenRet = NULL;
    else
      hpenRet = ::CreatePen(nStyle, nWidth, FGColor());
    }

  return hpenRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      DeletePen
//
// DESCRIPTION: This method will delete a pen
//
////////////////////////////////////////////////////////////////////////////

BOOL LineAttrib::DeletePen
  (
  HPEN  hpen
  )

  {
  return hpen ? ::DeleteObject(hpen) : TRUE;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      InitLineWidth
//
// DESCRIPTION: This will set the line width group for the line editing
//              dialog.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::InitLineWidth
  (
  HWND  hwnd,
  int   nWidth
  )

  {
  if(nWidth<0)
    nWidth = 0;

  CheckDlgButton(hwnd, RB_HAIRLINE, nWidth==0);
  CheckDlgButton(hwnd, RB_LINEWIDTH, nWidth!=0);
  SetItemValue(hwnd, ED_LINEWIDTH, nWidth);
  SetScrollRange(GetDlgItem(hwnd, SB_LINEWIDTH), SB_CTL, 0, 32767, FALSE);
  SetScrollPos(GetDlgItem(hwnd, SB_LINEWIDTH), SB_CTL, nWidth, TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      GetLineWidthFromDlg
//
// DESCRIPTION: This will retrieve the line width from group in the line 
//              editing dialog.
//
////////////////////////////////////////////////////////////////////////////

int LineAttrib::GetLineWidthFromDlg
  (
  HWND  hwnd
  )

  {
  int nWidth;

  if(IsDlgButtonChecked(hwnd, RB_HAIRLINE))
    nWidth = 0;
  else
    nWidth = GetItemValue(hwnd, ED_LINEWIDTH);

  return nWidth;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      SetEditFocus
//
// DESCRIPTION: This will set the focus to the edit field and select the
//              text.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::SetEditFocus
  (
  HWND  hwnd,
  UINT  idCtl,
  BOOL  fSelectAll
  )

  {
  HWND  hwndEdit = GetDlgItem(hwnd, idCtl);
  int   nWidth = /*GetLineWidthFromDlg(hwnd);*/GetItemValue(hwnd, idCtl);

  if(GetFocus()==hwndEdit)
    {
    if(fSelectAll)
      SendMessage(hwndEdit, EM_SETSEL, 0, MAKELPARAM(0, -1));
    }
  else
    {
    SetFocus(hwndEdit);
    SendMessage(hwndEdit, EM_SETSEL, 0, MAKELPARAM(0, -1));
    }


  EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
  EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      WidthVScrollMsg
//
// DESCRIPTION: This will adjust the width of the line from scroll messages.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::WidthVScrollMsg
  (
  HWND    hwnd,
  WPARAM  wParam,
  LPARAM  lParam,
  UINT    idSampleCtl
  )

  {
  int nWidth = GetItemValue(hwnd, ED_LINEWIDTH);
  int nWidthNew = nWidth;

  switch(wParam)
    {
    case SB_LINEDOWN:
      if(nWidthNew>0)
        nWidthNew--;
      break;

    case SB_LINEUP:
      if(nWidthNew<0x7FFF)
        nWidthNew++;
      break;

    case SB_PAGEDOWN:
      nWidthNew -= 10;
      if(nWidthNew<0)
        nWidthNew = 0;
      break;

    case SB_PAGEUP:
      nWidthNew += 10;
      if(nWidthNew<0)
        nWidthNew = 0x7FFF;
      break;

    case SB_THUMBPOSITION:
      nWidthNew = LOWORD(lParam);
      break;

    default:
      break;
    }

  if(nWidthNew!=nWidth)
    {
    SetItemValue(hwnd, ED_LINEWIDTH, nWidthNew);
    SetEditFocus(hwnd, ED_LINEWIDTH, TRUE);
    InvalidateRect(GetDlgItem(hwnd, idSampleCtl), NULL, FALSE);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      SetItemValue
//
// DESCRIPTION: This will convert an incoming value from tenths of a milimeter
//              to the current unit preference and set the control's text.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::SetItemValue
  (
  HWND  hwnd,
  UINT  idCtrl,
  int   nVal
  )

  {
  char          szBuffer[80];
  double        dVal;
  ATTR_LINEDLG* ldlg = (ATTR_LINEDLG*)GetWindowLong(hwnd, DWL_USER);

  EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nVal==0);
  EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nVal==0);

  if(ldlg->nCurrentUnits == UNITS_ENGLISH_IN)
    {
    dVal = (double)(nVal / 254.);
    sprintf(szBuffer, "%.3lf\"", dVal);
    }
  else if(ldlg->nCurrentUnits == UNITS_METRIC_MM)
    {
    dVal = (double)(nVal / 10.);
    sprintf(szBuffer, "%.1lf mm", dVal);
    }
  else // if(ldlg->nCurrentUnits == UNITS_POINTS_PT)
    {
    dVal = (double)(nVal / 254. * 72.);
    sprintf(szBuffer, "%.2lf pt", dVal);
    }

  SetDlgItemText(hwnd, idCtrl, szBuffer);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      GetItemValue
//
// DESCRIPTION: This will convert text from a control from the current units
//              to 0.1 millimeters and return the new value.
//
////////////////////////////////////////////////////////////////////////////

int LineAttrib::GetItemValue
  (
  HWND  hwnd,
  UINT  idCtrl
  )

  {
  char          szBuffer[80];
  double        dVal;
  int           nRet = 0;
  ATTR_LINEDLG* ldlg = (ATTR_LINEDLG*)GetWindowLong(hwnd, DWL_USER);

  GetDlgItemText(hwnd, idCtrl, szBuffer, 80);
  dVal = strtod(szBuffer, NULL);

  if(ldlg->nCurrentUnits == UNITS_ENGLISH_IN)
    dVal = dVal * 254. + .5;
  else if(ldlg->nCurrentUnits == UNITS_METRIC_MM)
    dVal = dVal * 10. + .5;
  else // if(ldlg->nCurrentUnits == UNITS_POINTS_PT)
    dVal = dVal * 254. / 72. + .5;

  if(dVal<0.)
    nRet = 0;
  if(dVal>(double)0x7FFF)
    nRet = 0x7FFF;
  else
    nRet = dVal;

  return nRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      FillLineStyleComboBox
//
// DESCRIPTION: This will fill an owner draw combo box with different line
//              styles.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::FillLineStyleComboBox
  (
  HWND  hwndCombo,
  int   nStyle
  )

  {
  int i;
  int nCount = sizeof(sm_nLineStyle) / sizeof(sm_nLineStyle[0]);

  SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0L);
  for(i=0; i<nCount; i++)
    {
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)"");
    SendMessage(hwndCombo, CB_SETITEMDATA, (WPARAM)i, (LPARAM)sm_nLineStyle[i]);
    if(sm_nLineStyle[i]==nStyle)
      SendMessage(hwndCombo, CB_SETCURSEL, (WPARAM)i, 0L);
    }

  if(SendMessage(hwndCombo, CB_GETCURSEL, 0, 0L)==CB_ERR)
    SendMessage(hwndCombo, CB_SETCURSEL, 0, 0L);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      DrawLineStyleComboBox
//
// DESCRIPTION: This will draw an owner draw combo box with different line
//              styles.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::DrawLineStyleComboBox
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  HBRUSH    hbrHighlight;
  COLORREF  clrOldFG, clrOldBG;
  int       nPenStyle = (int)SendDlgItemMessage(hwnd, lpdi->CtlID, CB_GETITEMDATA, lpdi->itemID, 0);
  int       nVertPos = lpdi->rcItem.top + ((lpdi->rcItem.bottom - lpdi->rcItem.top) / 2);
  HPEN      hpen, hpenOld;

  if (lpdi->itemState & (ODS_SELECTED|ODS_FOCUS))
    {
    hbrHighlight = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
    clrOldFG = SetTextColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
    clrOldBG = SetBkColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHT));
    }
  else
    {
    if((nPenStyle==PS_3DIN) || (nPenStyle==PS_3DOUT))
      hbrHighlight = CreateSolidBrush(RGB(192, 192, 192));
    else
      hbrHighlight = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

    clrOldFG = SetTextColor(lpdi->hDC, GetSysColor(COLOR_WINDOWTEXT));
    clrOldBG = SetBkColor(lpdi->hDC, GetSysColor(COLOR_WINDOW));
    }

  FillRect(lpdi->hDC, &lpdi->rcItem, hbrHighlight);
  DeleteObject(hbrHighlight);

  if(nPenStyle==PS_NULL)
    DrawText(lpdi->hDC, "NULL", -1, &lpdi->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  else if(nPenStyle==PS_3DIN)
    {
    hpen = ::CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    MoveTo(lpdi->hDC, lpdi->rcItem.left+2, nVertPos);
    LineTo(lpdi->hDC, lpdi->rcItem.right-2, nVertPos);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);

    nVertPos++;
    hpen = ::CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    MoveTo(lpdi->hDC, lpdi->rcItem.left+2, nVertPos);
    LineTo(lpdi->hDC, lpdi->rcItem.right-2, nVertPos);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);
    }
  else if(nPenStyle==PS_3DOUT)
    {
    hpen = ::CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    MoveTo(lpdi->hDC, lpdi->rcItem.left+2, nVertPos);
    LineTo(lpdi->hDC, lpdi->rcItem.right-2, nVertPos);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);

    nVertPos++;
    hpen = ::CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    MoveTo(lpdi->hDC, lpdi->rcItem.left+2, nVertPos);
    LineTo(lpdi->hDC, lpdi->rcItem.right-2, nVertPos);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);
    }
  else
    {
    hpen = ::CreatePen(nPenStyle, 0, GetSysColor((lpdi->itemState & (ODS_SELECTED|ODS_FOCUS)) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    MoveTo(lpdi->hDC, lpdi->rcItem.left+2, nVertPos);
    LineTo(lpdi->hDC, lpdi->rcItem.right-2, nVertPos);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);
    }

  if (lpdi->itemState & ODS_FOCUS)
    DrawFocusRect(lpdi->hDC, &lpdi->rcItem);

  SetTextColor(lpdi->hDC, clrOldFG);
  SetBkColor(lpdi->hDC, clrOldBG);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      DrawSampleLine
//
// DESCRIPTION: This will draw an owner draw button with the current selected
//              line style.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::DrawSampleLine
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  COLORREF  clrFG, clrBG, clrBGOld;
  int       nStyle, nWidth, nOldBkMode, y;
  LRESULT   lRes;
  HPEN      hpen, hpenOld;
  HBRUSH    hbr = CreateSolidBrush(RGB(192, 192, 192)); // light gray brush for Borland

  lRes = SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETCURSEL, 0, 0L);
  clrFG = (COLORREF)SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  if(clrFG==COLOR_WINDOWFOREGROUND)
    clrFG = GetSysColor(COLOR_WINDOWTEXT);
  lRes = SendDlgItemMessage(hwnd, CB_LINEBGCOLOR, CB_GETCURSEL, 0, 0L);
  clrBG = (COLORREF)SendDlgItemMessage(hwnd, CB_LINEBGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  if(clrBG==COLOR_WINDOWBACKGROUND)
    clrBG = GetSysColor(COLOR_WINDOW);
  lRes = SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETCURSEL, 0, 0L);
  nStyle = (int)SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  nWidth = GetLineWidthFromDlg(hwnd); // in tenths of a millimeter
  if(nWidth > 0)
    {
    // convert to screen coordinates
    nWidth = (double)nWidth  * (double)GetDeviceCaps(lpdi->hDC, LOGPIXELSY) / 254. + 0.5;
    nStyle = PS_SOLID;
    }

  FillRect(lpdi->hDC, &lpdi->rcItem, hbr);
  if(clrBG==COLOR_TRANSPARENT)
    nOldBkMode = SetBkMode(lpdi->hDC, TRANSPARENT);
  else
    {
    nOldBkMode = SetBkMode(lpdi->hDC, OPAQUE);
    clrBGOld = SetBkColor(lpdi->hDC, clrBG);
    }

  if(nStyle==PS_NULL)
    ;
  else if(nStyle==PS_3DIN)
    {
    hpen = ::CreatePen(PS_SOLID, 0, clrFG);
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    y = lpdi->rcItem.top + (lpdi->rcItem.bottom - lpdi->rcItem.top)/2;
    MoveTo(lpdi->hDC, lpdi->rcItem.left, y);
    LineTo(lpdi->hDC, lpdi->rcItem.right, y);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);

    y++;
    hpen = ::CreatePen(PS_SOLID, 0, clrBG);
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    MoveTo(lpdi->hDC, lpdi->rcItem.left, y);
    LineTo(lpdi->hDC, lpdi->rcItem.right, y);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);
    }
  else if(nStyle==PS_3DOUT)
    {
    hpen = ::CreatePen(PS_SOLID, 0, clrBG);
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    y = lpdi->rcItem.top + (lpdi->rcItem.bottom - lpdi->rcItem.top)/2;
    MoveTo(lpdi->hDC, lpdi->rcItem.left, y);
    LineTo(lpdi->hDC, lpdi->rcItem.right, y);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);

    y++;
    hpen = ::CreatePen(PS_SOLID, 0, clrFG);
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    MoveTo(lpdi->hDC, lpdi->rcItem.left, y);
    LineTo(lpdi->hDC, lpdi->rcItem.right, y);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);
    }
  else
    {
    hpen = ::CreatePen(nStyle, nWidth, clrFG);
    hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
    y = lpdi->rcItem.top + (lpdi->rcItem.bottom - lpdi->rcItem.top)/2;
    MoveTo(lpdi->hDC, lpdi->rcItem.left, y);
    LineTo(lpdi->hDC, lpdi->rcItem.right, y);
    SelectObject(lpdi->hDC, hpenOld);
    DeleteObject(hpen);
    }

  SetBkMode(lpdi->hDC, nOldBkMode);
  if(clrBG!=COLOR_TRANSPARENT)
    SetBkColor(lpdi->hDC, clrBGOld);

  DeleteObject(hbr);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      EditDlgProc
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

BOOL WINAPI _export LineAttrib::EditDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  LineAttrib* line;
  BOOL        fRet = FALSE;
  int         nWidth;

  switch(uMsg)
    {
    case WM_INITDIALOG:
      {
			AUtlCenterDialog(hwnd, 0);

      ATTR_LINEDLG* ldlg = (ATTR_LINEDLG*)lParam;
      line = ldlg->line;
      SetWindowLong(hwnd, DWL_USER, (long)ldlg);

      FillColorComboBox(GetDlgItem(hwnd, CB_LINEFGCOLOR), line->GetTrueFGColor(), TRUE, FALSE);
      FillColorComboBox(GetDlgItem(hwnd, CB_LINEBGCOLOR), line->GetTrueBGColor(), FALSE, TRUE);
      FillLineStyleComboBox(GetDlgItem(hwnd, CB_LINESTYLE), line->LineStyle());
      InitLineWidth(hwnd, line->LineWidth());

      if((line->LineStyle()==PS_3DIN) || (line->LineStyle()==PS_3DOUT))
        {
        SetDlgItemText(hwnd, ST_LINEFGCOLOR, "&Shadow:");
        SetDlgItemText(hwnd, ST_LINEBGCOLOR, "&Highlight:");
        }
      else
        {
        SetDlgItemText(hwnd, ST_LINEFGCOLOR, "&Foreground:");
        SetDlgItemText(hwnd, ST_LINEBGCOLOR, "&Background:");
        }
      EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), line->LineWidth()==0);
      EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), line->LineWidth()==0);

      if(ldlg->szTitle)
        SetWindowText(hwnd, ldlg->szTitle);
      if(!ldlg->fShowBGColor)
        ShowWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), SW_HIDE);
      fRet = TRUE;
      break;
      }

    case WM_DESTROY:
      break;

    case WM_MEASUREITEM:
      MeasureComboBox(hwnd, (LPMEASUREITEMSTRUCT)lParam);
      fRet = TRUE;
      break;

    case WM_DRAWITEM:
      {
      LPDRAWITEMSTRUCT  lpdi = (LPDRAWITEMSTRUCT)lParam;

      switch(lpdi->CtlID)
        {
        case CB_LINEFGCOLOR:
        case CB_LINEBGCOLOR:
          DrawColorComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case CB_LINESTYLE:
          DrawLineStyleComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case PB_SAMPLELINE:
          DrawSampleLine(hwnd, lpdi);
          fRet = TRUE;
          break;

        default:
          break;
        }
      }
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          {
          DWORD dwIndex;

          line = ((ATTR_LINEDLG*)GetWindowLong(hwnd, DWL_USER))->line;
          dwIndex = SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETCURSEL, 0, 0L);
          if(dwIndex!=CB_ERR)
            line->SetFGColor((COLORREF)SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETITEMDATA, (WPARAM)dwIndex, 0L));
          dwIndex = SendDlgItemMessage(hwnd, CB_LINEBGCOLOR, CB_GETCURSEL, 0, 0L);
          if(dwIndex!=CB_ERR)
            line->SetBGColor((COLORREF)SendDlgItemMessage(hwnd, CB_LINEBGCOLOR, CB_GETITEMDATA, (WPARAM)dwIndex, 0L));
          dwIndex = SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETCURSEL, 0, 0L);
          if(dwIndex!=CB_ERR)
            line->m_nStyle = (int)SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETITEMDATA, (WPARAM)dwIndex, 0L);
          line->SetLineWidth(GetLineWidthFromDlg(hwnd));
          EndDialog(hwnd, TRUE);
          fRet = TRUE;
          }
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          break;

        case CB_LINEFGCOLOR:
        case CB_LINEBGCOLOR:
          if(HIWORD(lParam) == CBN_SELCHANGE)
            InvalidateRect(GetDlgItem(hwnd, PB_SAMPLELINE), NULL, TRUE);
          break;

        case CB_LINESTYLE:
          if(HIWORD(lParam) == CBN_SELCHANGE)
            {
            int idx     = (int)SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETCURSEL, 0, 0L);
            int nStyle  = (int)SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETITEMDATA, (WPARAM)idx, 0L);

            if((nStyle==PS_3DIN) || (nStyle==PS_3DOUT))
              {
              SetDlgItemText(hwnd, ST_LINEFGCOLOR, "&Shadow:");
              SetDlgItemText(hwnd, ST_LINEBGCOLOR, "&Highlight:");
              }
            else
              {
              SetDlgItemText(hwnd, ST_LINEFGCOLOR, "&Foreground:");
              SetDlgItemText(hwnd, ST_LINEBGCOLOR, "&Background:");
              }

            InvalidateRect(GetDlgItem(hwnd, PB_SAMPLELINE), NULL, TRUE);
            }
          break;

        case RB_LINEWIDTH:
          SetEditFocus(hwnd, ED_LINEWIDTH, FALSE);
          // fall through
        case RB_HAIRLINE:
          nWidth = GetLineWidthFromDlg(hwnd);
          EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), nWidth==0);
          InvalidateRect(GetDlgItem(hwnd, PB_SAMPLELINE), NULL, TRUE);
          fRet = TRUE;
          break;

        case ED_LINEWIDTH:
          switch(HIWORD(lParam))
            {
            case EN_KILLFOCUS:
              SetItemValue(hwnd, wParam, GetItemValue(hwnd, wParam));
              InvalidateRect(GetDlgItem(hwnd, PB_SAMPLELINE), NULL, TRUE);
              fRet = TRUE;
              break;

            case EN_SETFOCUS:
              {
              int   nWidth = LineAttrib::GetItemValue(hwnd, ED_LINEWIDTH);

              EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
              EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);

              CheckDlgButton(hwnd, RB_LINEWIDTH, TRUE);
              CheckDlgButton(hwnd, RB_HAIRLINE, FALSE);
              InvalidateRect(GetDlgItem(hwnd, PB_SAMPLELINE), NULL, TRUE);
              fRet = TRUE;
              break;
              }

            default:
              break;
            }

          nWidth = GetLineWidthFromDlg(hwnd);
          EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), nWidth==0);
          break;

        default:
          break;
        }
      break;

    case WM_VSCROLL:
      WidthVScrollMsg(hwnd, wParam, lParam, PB_SAMPLELINE);

      nWidth = GetLineWidthFromDlg(hwnd);
      EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), nWidth==0);
      EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), nWidth==0);

      fRet = TRUE;
      break;

    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      EditLine
//
// DESCRIPTION: This method will edit the line attributes
//
////////////////////////////////////////////////////////////////////////////

BOOL LineAttrib::EditLine
  (
  HWND  hwndParent,
  short nCurrentUnits,
  LPSTR szTitle,
  BOOL  fShowBGColor
  )

  {
  ATTR_LINEDLG  ldlg = {szTitle, nCurrentUnits, fShowBGColor, this};

  return DialogBoxParam(GetHInstance(), ID(DLG_LINEEDIT), hwndParent, EditDlgProc, (long)&ldlg);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      Export
//
// DESCRIPTION: This method will export the current line attributes
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::Export
  (
  )

  {
  if(AUpsfCreateScope("LINESCOP", 0L) == AUPSF_NOERROR)
    {
    // export the color attributes
    ColorAttrib::Export();

    // export the line attributes
    ExportShort("WIDTH",    0L, m_nWidth);
    ExportShort("STYLE",    0L, m_nStyle);

    AUpsfCloseScope();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      Import
//
// DESCRIPTION: This method will import the current line attributes
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::Import
  (
  short sVer
  )

  {
  if(sVer < 0x0100)
    {
    short     sVal;
    COLORREF  clr;
    // set up the default pen attributes
    m_nWidth = 0;
    m_nStyle = PS_SOLID;

    // import the line attributes
    if(ImportShort("PENWIDTH", 0L, &sVal)==AUPSF_NOERROR)
      m_nWidth = MulDiv(sVal, 254, 96); // assume 96 dpi

    ImportShort("PENSTYLE", 0L, (short*)&m_nStyle);

    // import the color attributes -- old version
    if(AUpsfOpenScope("ATTRIBUT", 0L) == AUPSF_NOERROR)
      {
      if(ImportColor("FGCOLOR", 0L, &clr)==AUPSF_NOERROR)
        SetFGColor(clr);
      else
        SetFGDefault();

      if(ImportColor("BGCOLOR", 0L, &clr)==AUPSF_NOERROR)
        SetBGColor(clr);
      else
        SetBGDefault();

      AUpsfCloseScope();
      }
    }
  else
    {
    if(AUpsfOpenScope("LINESCOP", 0L) == AUPSF_NOERROR)
      {
      // import the color attributes
      ColorAttrib::Import();

      // set up the default pen attributes
      m_nWidth = 0;
      m_nStyle = PS_SOLID;

      // import the line attributes
      ImportShort("WIDTH", 0L, (short*)&m_nWidth);
      ImportShort("STYLE", 0L, (short*)&m_nStyle);

      AUpsfCloseScope();
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       LineAttrib
//
// METHOD:      PrepForFile
//
// DESCRIPTION: This method will fill in a file structure with the class
//              information.
//
////////////////////////////////////////////////////////////////////////////

void LineAttrib::PrepForFile
  (
  ATTR_LINE*  pLine
  )

  {
  pLine->sVer = ATTR_VERSION;
  pLine->clrFG = GetTrueFGColor();
  pLine->clrBG = GetTrueBGColor();
  pLine->nWidth = m_nWidth;
  pLine->nStyle = m_nStyle;
  }

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// STATIC DEFINITIONS
//
////////////////////////////////////////////////////////////////////////////

FillBrushInfo FillAttrib::sm_nFillPattern[] =
      {
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0, NULL, NULL},  // PAT_FOREGROUND
        {{0x22, 0x00, 0x88, 0x00, 0x22, 0x00, 0x88, 0x00}, 0, NULL, NULL},
        {{0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88}, 0, NULL, NULL},
        {{0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA}, 0, NULL, NULL},
        {{0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00}, 0, NULL, NULL},
        {{0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, 0, NULL, NULL},
        {{0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88}, 0, NULL, NULL},
        {{0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77}, 0, NULL, NULL},
        {{0x4E, 0xCF, 0xFC, 0xE4, 0x27, 0x3F, 0xF3, 0x72}, 0, NULL, NULL},
        {{0x7F, 0xEF, 0xFD, 0xDF, 0xFE, 0xF7, 0xBF, 0xFB}, 0, NULL, NULL},
        {{0x00, 0x77, 0x77, 0x77, 0x00, 0x77, 0x77, 0x77}, 0, NULL, NULL},
        {{0x00, 0x7F, 0x7F, 0x7F, 0x00, 0xF7, 0xF7, 0xF7}, 0, NULL, NULL},
        {{0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 0, NULL, NULL},
        {{0x7F, 0xBF, 0xDF, 0xFF, 0xFD, 0xFB, 0xF7, 0xFF}, 0, NULL, NULL},
        {{0x7D, 0xBB, 0xC6, 0xBB, 0x7D, 0xFE, 0xFE, 0xFE}, 0, NULL, NULL},
        {{0x07, 0x8B, 0xDD, 0xB8, 0x70, 0xE8, 0xDD, 0x8E}, 0, NULL, NULL},
        {{0xAA, 0x5F, 0xBF, 0xBF, 0xAA, 0xF5, 0xFB, 0xFB}, 0, NULL, NULL},
        {{0xDF, 0xAF, 0x77, 0x77, 0x77, 0x77, 0xFA, 0xFD}, 0, NULL, NULL},
        {{0x40, 0xFF, 0x40, 0x40, 0x4F, 0x4F, 0x4F, 0x4F}, 0, NULL, NULL},
        {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 0, NULL, NULL},  // PAT_BACKGROUND
        {{0xFF, 0xF7, 0xFF, 0x7F, 0xFF, 0xF7, 0xFF, 0x7F}, 0, NULL, NULL},
        {{0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x77}, 0, NULL, NULL},
        {{0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77}, 0, NULL, NULL},
        {{0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x55}, 0, NULL, NULL},
        {{0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00}, 0, NULL, NULL},
        {{0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77}, 0, NULL, NULL},
        {{0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00}, 0, NULL, NULL},
        {{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F}, 0, NULL, NULL},
        {{0xFF, 0x7F, 0xFF, 0x77, 0xFF, 0x7F, 0xFF, 0x55}, 0, NULL, NULL},
        {{0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00}, 0, NULL, NULL},
        {{0xF7, 0xE3, 0xDD, 0x3E, 0x7F, 0xFE, 0xFD, 0xFB}, 0, NULL, NULL},
        {{0xFF, 0x55, 0xFF, 0x77, 0xBE, 0xDD, 0xEB, 0x77}, 0, NULL, NULL},
        {{0xBF, 0x5F, 0xFF, 0xFF, 0xFB, 0xF5, 0xFF, 0xFF}, 0, NULL, NULL},
        {{0xFC, 0x7B, 0xB7, 0xCF, 0xF3, 0xFD, 0xFE, 0xFE}, 0, NULL, NULL},
        {{0x7F, 0x7F, 0xBE, 0xC1, 0xF7, 0xF7, 0xEB, 0x1C}, 0, NULL, NULL},
        {{0xEF, 0xDF, 0xAB, 0x55, 0x00, 0xFD, 0xFB, 0xF7}, 0, NULL, NULL},
        {{0x88, 0x76, 0x70, 0x70, 0x88, 0x67, 0x07, 0x07}, 0, NULL, NULL},
        {{0xF7, 0xEB, 0xD5, 0xAA, 0xD5, 0xEB, 0xF7, 0xFF}, 0, NULL, NULL},
        {{0xEB, 0xDD, 0xB6, 0x6B, 0xD6, 0x6D, 0xBB, 0xD7}, 0, NULL, NULL}
      };

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      Constructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

FillAttrib::FillAttrib
  (
  COLORREF  clrFG,
  COLORREF  clrBG,
  int       nPattern,
  int*      pattern
  ) : ColorAttrib(clrFG, clrBG)

  {
  m_nPattern = nPattern;
  if(pattern)
    {
    memcpy(m_fbUser.pattern, pattern, sizeof(int)*8);
    m_fbUser.nUseCount = 0;
    m_fbUser.hbmp = NULL;
    m_fbUser.hbr = NULL;
    }
  else
    m_fbUser = sm_nFillPattern[0];
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      Constructor based on a ATTR_FILL structure
//
// DESCRIPTION: This will initialize the FillAttributes class from some data
//              that has been saved.
//
////////////////////////////////////////////////////////////////////////////

FillAttrib::FillAttrib
  (
  ATTR_FILL*  fill
  ) : ColorAttrib()

  {
  if(fill && (fill->sVer==ATTR_VERSION))
    {
    SetFGColor(fill->clrFG);
    SetBGColor(fill->clrBG);
    m_nPattern = fill->nPattern;
    memcpy(m_fbUser.pattern, fill->pattern, sizeof(int)*8);
    m_fbUser.nUseCount = 0;
    m_fbUser.hbmp = NULL;
    m_fbUser.hbr = NULL;
    }
  else
    {
    SetFGColor(COLOR_WINDOWFOREGROUND);
    SetBGColor(COLOR_WINDOWBACKGROUND);
    m_nPattern = 0;
    m_fbUser = sm_nFillPattern[0];
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

FillAttrib::~FillAttrib
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      SetFillPattern
//
// DESCRIPTION: This method will set the fill pattern
//
////////////////////////////////////////////////////////////////////////////

BOOL FillAttrib::SetFillPattern
  (
  int   nPattern,
  int*  pattern
  )
  
  {
  BOOL  fRet;
  int   bits[8];
  int   i;

  if(pattern)
    {
    // the low order byte is all that is significant
    for(i=0; i<8; i++)
      bits[i] = pattern[i] & 0x00FF;
    }

  if(pattern && (nPattern<0))
    {
    int nCount = sizeof(sm_nFillPattern) / sizeof(sm_nFillPattern[0]);

    for(i=0; i<nCount; i++)
      {
      if(!memcmp(bits, sm_nFillPattern[i].pattern, sizeof(int)*8))
        {
        nPattern = i;
        break;
        }
      }
    }

  if(nPattern<0)
    {
    if(pattern && memcmp(pattern, m_fbUser.pattern, sizeof(int)*8))
      {
      // clean up the brush if it has been created
      if(m_nPattern<0)
        {
        I_ASSERT(m_fbUser.hbr);

        #ifdef AW_I_DEBUG
        BOOL  fCheck;

        if(m_fbUser.hbr)
          fCheck = DeleteObject(m_fbUser.hbr);
        I_ASSERT(fCheck);
        if(m_fbUser.hbmp)
          fCheck = DeleteObject(m_fbUser.hbmp);
        I_ASSERT(fCheck);
        #else
        if(m_fbUser.hbr)
          DeleteObject(m_fbUser.hbr);
        if(m_fbUser.hbmp)
          DeleteObject(m_fbUser.hbmp);
        #endif
        }

      m_nPattern = -1;
      memcpy(m_fbUser.pattern, bits, sizeof(int)*8);
      fRet = TRUE;
      }
    else
      fRet = FALSE;
    }
  else
    {
    if(m_nPattern == nPattern)
      fRet = FALSE;
    else
      {
      m_nPattern = nPattern;
      fRet = TRUE;
      }
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      CreateBrush
//
// DESCRIPTION: This method will create a brush based on the fill attributes
//
////////////////////////////////////////////////////////////////////////////

HBRUSH FillAttrib::CreateBrush
  (
  )

  {
  HBRUSH  hbr;

  if(m_nPattern<0)
    {
    I_ASSERT(!m_fbUser.hbr);

    m_fbUser.hbmp = CreateBitmap(8, 8, 1, 1, m_fbUser.pattern);
    m_fbUser.hbr = CreatePatternBrush(m_fbUser.hbmp);
    hbr = m_fbUser.hbr;
    }
  else
    {
    if(sm_nFillPattern[m_nPattern].nUseCount==0)
      {
      sm_nFillPattern[m_nPattern].hbmp = CreateBitmap(8, 8, 1, 1, sm_nFillPattern[m_nPattern].pattern);
      sm_nFillPattern[m_nPattern].hbr = CreatePatternBrush(sm_nFillPattern[m_nPattern].hbmp);
      }

    hbr = sm_nFillPattern[m_nPattern].hbr;
    sm_nFillPattern[m_nPattern].nUseCount++;
    }

  return hbr;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      DeleteBrush
//
// DESCRIPTION: This method will delete a brush
//
////////////////////////////////////////////////////////////////////////////

BOOL FillAttrib::DeleteBrush
  (
  HBRUSH  hbr
  )

  {
  BOOL  fRet = FALSE;

  if(m_fbUser.hbr && m_fbUser.hbmp)
    {
    #ifdef AW_I_DEBUG
    BOOL  fCheck;

    fCheck = DeleteObject(m_fbUser.hbmp);
    I_ASSERT(fCheck);
    fCheck = DeleteObject(m_fbUser.hbr);
    I_ASSERT(fCheck);
    #else
    DeleteObject(m_fbUser.hbmp);
    DeleteObject(m_fbUser.hbr);
    #endif
    m_fbUser.hbmp = NULL;
    m_fbUser.hbr = NULL;

    fRet = TRUE;
    }
  else
    {
    if(hbr==sm_nFillPattern[m_nPattern].hbr)
      {
      sm_nFillPattern[m_nPattern].nUseCount--;
      if(sm_nFillPattern[m_nPattern].nUseCount==0)
        {
        #ifdef AW_I_DEBUG
        BOOL  fCheck;

        fCheck = DeleteObject(sm_nFillPattern[m_nPattern].hbmp);
        I_ASSERT(fCheck);
        fCheck = DeleteObject(sm_nFillPattern[m_nPattern].hbr);
        I_ASSERT(fCheck);
        #else
        DeleteObject(sm_nFillPattern[m_nPattern].hbmp);
        DeleteObject(sm_nFillPattern[m_nPattern].hbr);
        #endif
        sm_nFillPattern[m_nPattern].hbmp = NULL;
        sm_nFillPattern[m_nPattern].hbr = NULL;
        }
      fRet = TRUE;
      }
    else
      {
      // deleting a brush but the current has changed
      int nCount = sizeof(sm_nFillPattern) / sizeof(sm_nFillPattern[0]);
      int i;

      for(i=0; (i<nCount) && (fRet==FALSE); i++)
        {
        if(hbr==sm_nFillPattern[i].hbr)
          {
          sm_nFillPattern[i].nUseCount--;
          if(sm_nFillPattern[i].nUseCount==0)
            {
            #ifdef AW_I_DEBUG
            BOOL  fCheck;

            fCheck = DeleteObject(sm_nFillPattern[i].hbmp);
            I_ASSERT(fCheck);
            fCheck = DeleteObject(sm_nFillPattern[i].hbr);
            I_ASSERT(fCheck);
            #else
            DeleteObject(sm_nFillPattern[i].hbmp);
            DeleteObject(sm_nFillPattern[i].hbr);
            #endif
            sm_nFillPattern[i].hbmp = NULL;
            sm_nFillPattern[i].hbr = NULL;
            }
          fRet = TRUE;
          }
        } // end for
      } // end else if(hbr
    } // end else if(m_nPattern<0)

  I_ASSERT(fRet);

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      GetFillPattern
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

int*  FillAttrib::GetFillPattern
  (
  int         nPattern,
  FillAttrib* fill
  )

  {
  int*  pRet;

  I_ASSERT(((nPattern>=0) && !fill) || ((nPattern<0) && fill));

  if((nPattern<0) && fill)
    {
    if(fill->m_nPattern > 0)
      pRet = sm_nFillPattern[fill->m_nPattern].pattern;
    else
      pRet = fill->m_fbUser.pattern;
    }
  else if(!fill)
    pRet = sm_nFillPattern[nPattern].pattern;
  else
    pRet = NULL;

  return pRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      FillPatternComboBox
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

void FillAttrib::FillPatternComboBox
  (
  HWND  hwndCombo,
  int   nPattern
  )
  
  {
  int i;
  int nCount = sizeof(sm_nFillPattern) / sizeof(sm_nFillPattern[0]);

  SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0L);
  for(i=0; i<nCount; i++)
    {
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)"");
    SendMessage(hwndCombo, CB_SETITEMDATA, (WPARAM)i, (LPARAM)i);
    if(i==nPattern)
      SendMessage(hwndCombo, CB_SETCURSEL, (WPARAM)i, 0L);
    }

  if(SendMessage(hwndCombo, CB_GETCURSEL, 0, 0L)==CB_ERR)
    SendMessage(hwndCombo, CB_SETCURSEL, 0, 0L);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      DrawPatternComboBox
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

void FillAttrib::DrawPatternComboBox
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  HBITMAP   hbmp;
  HBRUSH    hbr, hbrOld, hbrHighlight;
  HPEN      hpen, hpenOld;
  COLORREF  clrOldFG, clrOldBG;
  int       nPattern;

  nPattern = (int)SendDlgItemMessage(hwnd, lpdi->CtlID, CB_GETITEMDATA, lpdi->itemID, 0);
  hbmp = CreateBitmap(8, 8, 1, 1, sm_nFillPattern[nPattern].pattern);
  hbr = CreatePatternBrush(hbmp);

  if (lpdi->itemState & (ODS_SELECTED|ODS_FOCUS))
    {
    hbrHighlight = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
    clrOldFG = SetTextColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
    clrOldBG = SetBkColor(lpdi->hDC, GetSysColor(COLOR_HIGHLIGHT));
    }
  else
    {
    hbrHighlight = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    clrOldFG = SetTextColor(lpdi->hDC, GetSysColor(COLOR_WINDOWTEXT));
    clrOldBG = SetBkColor(lpdi->hDC, GetSysColor(COLOR_WINDOW));
    }

  hpen = (HPEN)GetStockObject(NULL_PEN);
  hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);
  SetBrushOrg(lpdi->hDC, lpdi->rcItem.left%8, lpdi->rcItem.top%8);
  hbrOld = (HBRUSH)SelectObject(lpdi->hDC, hbr);
  FillRect(lpdi->hDC, &lpdi->rcItem, hbrHighlight );
  Rectangle(lpdi->hDC, lpdi->rcItem.left+1, lpdi->rcItem.top+1, lpdi->rcItem.right-1, lpdi->rcItem.bottom-1);
  SelectObject(lpdi->hDC, hbrOld);
  SelectObject(lpdi->hDC, hpenOld);

  if (lpdi->itemState & ODS_FOCUS)
    DrawFocusRect(lpdi->hDC, &lpdi->rcItem);

  SetTextColor(lpdi->hDC, clrOldFG);
  SetBkColor(lpdi->hDC, clrOldBG);
  DeleteObject(hbr);
  DeleteObject(hbmp);
  DeleteObject(hbrHighlight);
  }


////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      DrawSampleFill
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

void FillAttrib::DrawSampleFill
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  COLORREF  clrFG, clrBG, clrBGOld, clrFGOld;
  int       nPattern;
  LRESULT   lRes;
  HBITMAP   hbmp;
  HBRUSH    hbr;

  lRes = SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETCURSEL, 0, 0L);
  clrFG = (COLORREF)SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  if(clrFG==COLOR_WINDOWFOREGROUND)
    clrFG = GetSysColor(COLOR_WINDOWTEXT);
  lRes = SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETCURSEL, 0, 0L);
  clrBG = (COLORREF)SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  if(clrBG==COLOR_WINDOWBACKGROUND)
    clrBG = GetSysColor(COLOR_WINDOW);
  lRes = SendDlgItemMessage(hwnd, CB_FILLPATTERN, CB_GETCURSEL, 0, 0L);
  nPattern = (int)SendDlgItemMessage(hwnd, CB_FILLPATTERN, CB_GETITEMDATA, (WPARAM)lRes, 0L);

  clrBGOld = SetBkColor(lpdi->hDC, clrBG);
  clrFGOld = SetTextColor(lpdi->hDC, clrFG);
  hbmp = CreateBitmap(8, 8, 1, 1, sm_nFillPattern[nPattern].pattern);
  hbr = CreatePatternBrush(hbmp);
  FillRect(lpdi->hDC, &lpdi->rcItem, hbr);
  DeleteObject(hbmp);
  DeleteObject(hbr);
  SetBkColor(lpdi->hDC, clrBGOld);
  SetTextColor(lpdi->hDC, clrFGOld);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      EditDlgProc
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

BOOL WINAPI _export FillAttrib::EditDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  FillAttrib* fill;
  BOOL        fRet = FALSE;

  switch(uMsg)
    {
    case WM_INITDIALOG:
      {
			AUtlCenterDialog(hwnd, 0);

      ATTR_FILLDLG* fdlg = (ATTR_FILLDLG*)lParam;
      fill = fdlg->fill;
      SetWindowLong(hwnd, DWL_USER, (long)fill);

      FillColorComboBox(GetDlgItem(hwnd, CB_FILLFGCOLOR), fill->GetTrueFGColor(), TRUE, FALSE);
      FillColorComboBox(GetDlgItem(hwnd, CB_FILLBGCOLOR), fill->GetTrueBGColor(), FALSE, FALSE);
      FillPatternComboBox(GetDlgItem(hwnd, CB_FILLPATTERN), fill->m_nPattern);

      if(fdlg->szTitle)
        SetWindowText(hwnd, fdlg->szTitle);
      fRet = TRUE;
      break;
      }

    case WM_DESTROY:
      break;

    case WM_MEASUREITEM:
      MeasureComboBox(hwnd, (LPMEASUREITEMSTRUCT)lParam);
      fRet = TRUE;
      break;

    case WM_DRAWITEM:
      {
      LPDRAWITEMSTRUCT  lpdi = (LPDRAWITEMSTRUCT)lParam;

      switch(lpdi->CtlID)
        {
        case CB_FILLFGCOLOR:
        case CB_FILLBGCOLOR:
          DrawColorComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case CB_FILLPATTERN:
          DrawPatternComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case PB_SAMPLEFILL:
          DrawSampleFill(hwnd, lpdi);
          fRet = TRUE;
          break;
        }
      }
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        case IDOK:
          {
          DWORD dwIndex;

          fill = (FillAttrib*)GetWindowLong(hwnd, DWL_USER);
          dwIndex = SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETCURSEL, 0, 0L);
          if(dwIndex!=CB_ERR)
            fill->SetFGColor((COLORREF)SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETITEMDATA, (WPARAM)dwIndex, 0L));
          dwIndex = SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETCURSEL, 0, 0L);
          if(dwIndex!=CB_ERR)
            fill->SetBGColor((COLORREF)SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETITEMDATA, (WPARAM)dwIndex, 0L));
          dwIndex = SendDlgItemMessage(hwnd, CB_FILLPATTERN, CB_GETCURSEL, 0, 0L);
          if(dwIndex!=CB_ERR)
            fill->m_nPattern = (int)dwIndex;
          EndDialog(hwnd, TRUE);
          fRet = TRUE;
          }
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          break;

        case CB_FILLFGCOLOR:
        case CB_FILLBGCOLOR:
        case CB_FILLPATTERN:
          if(HIWORD(lParam) == CBN_SELCHANGE)
            InvalidateRect(GetDlgItem(hwnd, PB_SAMPLEFILL), NULL, TRUE);
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
// CLASS:       FillAttrib
//
// METHOD:      EditFill
//
// DESCRIPTION: This method will edit the fill pattern
//
////////////////////////////////////////////////////////////////////////////

BOOL FillAttrib::EditFill
  (
  HWND  hwndParent,
  LPSTR szTitle
  )

  {
  ATTR_FILLDLG  fdlg = {szTitle, this};

  return DialogBoxParam(GetHInstance(), ID(DLG_FILLEDIT), hwndParent, EditDlgProc, (long)&fdlg);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      Export
//
// DESCRIPTION: This method will export the current fill attributes
//
////////////////////////////////////////////////////////////////////////////

void FillAttrib::Export
  (
  )

  {
  if(AUpsfCreateScope("FILLSCOP", 0L) == AUPSF_NOERROR)
    {
    HBRUSH  hbr;

    // export the color attributes
    ColorAttrib::Export();

    // export the fill attributes
    ExportShort("PATTERN", 0L, m_nPattern + 1);

    // export the brush....
    if((hbr = CreateBrush()) != NULL)
      {
      ExportBrush("PAT-BITS", 0L, hbr);
      DeleteBrush(hbr);
      }

    AUpsfCloseScope();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      Import
//
// DESCRIPTION: This method will import the current fill attributes
//
////////////////////////////////////////////////////////////////////////////

void FillAttrib::Import
  (
  short sVer
  )

  {
  if(sVer < 0x0100)
    {
    short     sVal = 0;
    BOOL      fVal = FALSE;
    COLORREF  clr;

    // import the fill attributes
    ImportShort("PATTERN", 0L, &sVal);
    m_nPattern = sVal - 1;

    ImportBool("ISSOLID", 0L, &fVal);
    if((m_nPattern < 0) && fVal)
      m_nPattern = 0;

    if(m_nPattern < 0)  // user pattern
      ImportBrush("BRUSH", 0L, (short *)m_fbUser.pattern);

    // import the color attributes -- old version
    if(AUpsfOpenScope("ATTRIBUT", 0L) == AUPSF_NOERROR)
      {
      if(ImportColor("FGCOLOR", 0L, &clr)==AUPSF_NOERROR)
        SetFGColor(clr);
      else
        SetFGDefault();

      if(ImportColor("BGCOLOR", 0L, &clr)==AUPSF_NOERROR)
        SetBGColor(clr);
      else
        SetBGDefault();

      AUpsfCloseScope();
      }
    }
  else
    {
    if(AUpsfOpenScope("FILLSCOP", 0L) == AUPSF_NOERROR)
      {
      short     sVal = 0;

      // import the color attributes
      ColorAttrib::Import();

      // import the fill attributes
      ImportShort("PATTERN", 0L, &sVal);
      m_nPattern = sVal - 1;

      if(m_nPattern < 0)  // user pattern
        ImportBrush("PAT-BITS", 0L, (short *)m_fbUser.pattern);

      AUpsfCloseScope();
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FillAttrib
//
// METHOD:      PrepForFile
//
// DESCRIPTION: This method will fill in a file structure with the class
//              information.
//
////////////////////////////////////////////////////////////////////////////

void FillAttrib::PrepForFile
  (
  ATTR_FILL*  pFill
  )

  {
  pFill->sVer = ATTR_VERSION;
  pFill->clrFG = GetTrueFGColor();
  pFill->clrBG = GetTrueBGColor();
  pFill->nPattern = m_nPattern;
  memcpy(pFill->pattern, m_fbUser.pattern, sizeof(int)*8);
  }

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      Constructor
//
// DESCRIPTION: This will initialize the FontAttributes
//
////////////////////////////////////////////////////////////////////////////

FontAttrib::FontAttrib
  (
  LOGFONT*  lf,
  COLORREF  clrFG,
  COLORREF  clrBG,
  int       nTwips
  ) : ColorAttrib(clrFG, clrBG)

  {
  if(lf)
    m_lfFont = *lf;
  else
    {
    HFONT hfont = (HFONT)GetStockObject(SYSTEM_FONT);
    GetObject(hfont, sizeof(LOGFONT), &m_lfFont);
    }

  m_nTwips = nTwips;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      Constructor
//
// DESCRIPTION: This will initialize the FontAttributes class from some data
//              that has been saved.
//
////////////////////////////////////////////////////////////////////////////

FontAttrib::FontAttrib
  (
  ATTR_FONT*  font
  ) : ColorAttrib()

  {
  if(font && (font->sVer==ATTR_VERSION))
    {
    SetFGColor(font->clrFG);
    SetBGColor(font->clrBG);
    m_lfFont = font->lfFont;
    m_nTwips = font->nTwips;
    }
  else
    {
    HFONT hfont = (HFONT)GetStockObject(SYSTEM_FONT);
    GetObject(hfont, sizeof(LOGFONT), &m_lfFont);

    SetFGColor(COLOR_WINDOWFOREGROUND);
    SetBGColor(COLOR_TRANSPARENT);
    m_nTwips = DEF_TWIPSIZE;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

FontAttrib::~FontAttrib
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      operator ==
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

int FontAttrib::operator==
  (
  FontAttrib& font
  )

  {
  int nRet;

  nRet = (m_nTwips==font.m_nTwips)
            &&
         (m_lfFont.lfEscapement==font.m_lfFont.lfEscapement)
            &&
         (m_lfFont.lfOrientation==font.m_lfFont.lfOrientation)
            &&
         (m_lfFont.lfWeight==font.m_lfFont.lfWeight)
            &&
         (m_lfFont.lfItalic==font.m_lfFont.lfItalic)
            &&
         (m_lfFont.lfUnderline==font.m_lfFont.lfUnderline)
            &&
         (m_lfFont.lfStrikeOut==font.m_lfFont.lfStrikeOut)
            &&
         (m_lfFont.lfCharSet==font.m_lfFont.lfCharSet)
            &&
         (m_lfFont.lfOutPrecision==font.m_lfFont.lfOutPrecision)
            &&
         (m_lfFont.lfClipPrecision==font.m_lfFont.lfClipPrecision)
            &&
         (m_lfFont.lfQuality==font.m_lfFont.lfQuality)
            &&
         (m_lfFont.lfPitchAndFamily==font.m_lfFont.lfPitchAndFamily)
            &&
         !strcmp(m_lfFont.lfFaceName, font.m_lfFont.lfFaceName)
            &&
         (GetTrueFGColor()==font.GetTrueFGColor())
            &&
         (GetTrueBGColor()==font.GetTrueBGColor());

  return nRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      operator =
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

FontAttrib& FontAttrib::operator=
  (
  FontAttrib& font
  )

  {
  if(this != &font)
    {
    m_nTwips = font.m_nTwips;
    m_lfFont = font.m_lfFont;
    SetFGColor(font.GetTrueFGColor());
    SetBGColor(font.GetTrueBGColor());
    }

  return *this;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      CreateFont
//
// DESCRIPTION: This method will create the font based on the FontAttributes
//
////////////////////////////////////////////////////////////////////////////

HFONT FontAttrib::CreateFont
  (
  OBJECTID  oiWnd,
  HDC       hdc
  )

  {
  int nLPperIn;

  if(oiWnd)
    nLPperIn = 254;
  else if(hdc)
    nLPperIn = GetDeviceCaps(hdc, LOGPIXELSY);
  else
    {
    HDC hdcLocal = CreateDC("DISPLAY", NULL, NULL, NULL);
    nLPperIn = GetDeviceCaps(hdcLocal, LOGPIXELSY);
    DeleteDC(hdcLocal);
    }

  m_lfFont.lfHeight = -(int)((((long)m_nTwips * (long)nLPperIn) + 720L) / 1440L);
  m_lfFont.lfWidth = 0;

  return AUtlCreateFontIndirect(&m_lfFont);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      DeleteFont
//
// DESCRIPTION: This method will delete a font
//
////////////////////////////////////////////////////////////////////////////

BOOL FontAttrib::DeleteFont
  (
  HFONT hfont
  )

  {
  return AUtlDeleteFont(hfont);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      EditFont
//
// DESCRIPTION: This method will open the common dialog font editor.
//
////////////////////////////////////////////////////////////////////////////

BOOL FontAttrib::EditFont
  (
  HWND  hwndParent,
  LPSTR szTitle,
  LPSTR szSample,
  BOOL  bShowBG,
  BOOL  bDoEffects,
  BOOL  bPrinterFonts
  )

  {
  HDC       hdcPrinter = NULL;
  HDC       hdc;
  char      printer[64];
  LPSTR     device, driver, output;
  DWORD     flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
  short     nDeciPoints;
  BOOL      fRet = FALSE;
  int       nLPperIn;
  COLORREF  clrFG = GetTrueFGColor();
  COLORREF  clrBG = GetTrueBGColor();

  if(bPrinterFonts)
    {
    GetProfileString("windows", "device", "", printer, 64);
    if ((device = strtok(printer, "," )) != 0
          && (driver = strtok(NULL, ", ")) !=0
          && (output = strtok(NULL, ", ")) !=0 )
      {
      hdcPrinter = CreateDC(driver, device, output, NULL);
      flags |= CF_PRINTERFONTS;
      }
    else
      flags &= ~CF_PRINTERFONTS;
    }

  if(bDoEffects)
    flags |= CF_EFFECTS;

  hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
  nLPperIn = GetDeviceCaps(hdc, LOGPIXELSY);
  DeleteDC(hdc);
  m_lfFont.lfHeight = -(int)((((long)m_nTwips * (long)nLPperIn) + 720L) / 1440L);

  if(AUtlDlgEditFont(
      hwndParent,
      szSample,
      szTitle,
      &m_lfFont,
      &nDeciPoints,
      TRUE,
      &clrFG,
      bShowBG ? &clrBG : NULL,
      bDoEffects,
      flags,
      (bPrinterFonts && hdcPrinter) ? TRUE : FALSE,
      hdcPrinter))
    {
    SetFGColor(clrFG);
    if(bShowBG)
      SetBGColor(clrBG);

    m_nTwips = 2*nDeciPoints;
    fRet = TRUE;
    }

  if(hdcPrinter)
    DeleteDC(hdcPrinter);

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      Export
//
// DESCRIPTION: This method will export the current font.
//
////////////////////////////////////////////////////////////////////////////

void FontAttrib::Export
  (
  )

  {
  if(AUpsfCreateScope("FONTSCOP", 0L) == AUPSF_NOERROR)
    {
    // export the color attributes
    ColorAttrib::Export();

    // export the font attributes
    ExportFont ("FONTNAME", 0L, m_lfFont.lfFaceName);
    ExportShort("HEIGHT",   0L, MulDiv(m_nTwips, 254, 1440));
    ExportBool ("WEIGHT",   0L, m_lfFont.lfWeight);
    ExportBool ("ITALIC",   0L, m_lfFont.lfItalic);
    ExportBool ("UNDRLINE", 0L, m_lfFont.lfUnderline);
    ExportBool ("STRIKOUT", 0L, m_lfFont.lfStrikeOut);

    AUpsfCloseScope();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      Import
//
// DESCRIPTION: This method will import the current font.
//
////////////////////////////////////////////////////////////////////////////

void FontAttrib::Import
  (
  short sVer
  )

  {
  if(sVer < 0x0100)
    {
    if(AUpsfOpenScope("ATTRIBUT", 0L) == AUPSF_NOERROR)
      {
      short     sVal;
      BOOL      fVal;
      COLORREF  clr;

      // import the color attributes -- old version
      if(ImportColor("FGCOLOR", 0L, &clr)==AUPSF_NOERROR)
        SetFGColor(clr);
      else
        SetFGDefault();

      if(ImportColor("BGCOLOR", 0L, &clr)==AUPSF_NOERROR)
        SetBGColor(clr);
      else
        SetBGTransparent();

      // import the font attributes -- old version
      ImportFont("FONTNAME", 0L, m_lfFont.lfFaceName);

      if(ImportShort("POINTSZ",   0L, &sVal)==AUPSF_NOERROR)
        m_nTwips = sVal * 2;

      if(ImportBool("BOLD",   0L, &fVal)==AUPSF_NOERROR)
        m_lfFont.lfWeight = fVal ? FW_BOLD : FW_NORMAL;
      else
        m_lfFont.lfWeight = FW_NORMAL;
      
      if(ImportBool("ITALIC",   0L, &fVal)==AUPSF_NOERROR)
        m_lfFont.lfItalic = (char)fVal;
      else
        m_lfFont.lfItalic = 0;

      if(ImportBool("UNDRLINE",   0L, &fVal)==AUPSF_NOERROR)
        m_lfFont.lfUnderline = (char)fVal;
      else
        m_lfFont.lfUnderline = 0;

      if(ImportBool("STRIKOUT",   0L, &fVal)==AUPSF_NOERROR)
        m_lfFont.lfStrikeOut = (char)fVal;
      else
        m_lfFont.lfStrikeOut = 0;

      AUpsfCloseScope();
      }
    }
  else
    {
    if(AUpsfOpenScope("FONTSCOP", 0L) == AUPSF_NOERROR)
      {
      short sVal;
      BOOL  fVal;

      // import the color attributes
      ColorAttrib::Import();

      // import the font attributes
      ImportFont("FONTNAME", 0L, m_lfFont.lfFaceName);

      if(ImportShort("HEIGHT",   0L, &sVal)==AUPSF_NOERROR)
        m_nTwips = MulDiv(sVal, 1440, 254);

      if(ImportShort("WEIGHT",   0L, (short*)&m_lfFont.lfWeight)!=AUPSF_NOERROR)
        m_lfFont.lfWeight = FW_NORMAL;
      
      if(ImportBool("ITALIC",   0L, &fVal)==AUPSF_NOERROR)
        m_lfFont.lfItalic = (char)fVal;
      else
        m_lfFont.lfItalic = 0;

      if(ImportBool("UNDRLINE",   0L, &fVal)==AUPSF_NOERROR)
        m_lfFont.lfUnderline = (char)fVal;
      else
        m_lfFont.lfUnderline = 0;

      if(ImportBool("STRIKOUT",   0L, &fVal)==AUPSF_NOERROR)
        m_lfFont.lfStrikeOut = (char)fVal;
      else
        m_lfFont.lfStrikeOut = 0;

      AUpsfCloseScope();
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       FontAttrib
//
// METHOD:      PrepForFile
//
// DESCRIPTION: This method will fill in a file structure with the class
//              information.
//
////////////////////////////////////////////////////////////////////////////

void FontAttrib::PrepForFile
  (
  ATTR_FONT*  pFont
  )

  {
  pFont->sVer = ATTR_VERSION;
  pFont->clrFG = GetTrueFGColor();
  pFont->clrBG = GetTrueBGColor();
  pFont->lfFont = m_lfFont;
  pFont->nTwips = m_nTwips;
  }

////////////////////////////////////////////////////////////////////////////
