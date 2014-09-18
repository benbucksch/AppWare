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
// FILE:          SHP_PROT.CPP
//
// AUTHOR:        Kevin Smart
//
// DESCRIPTION:   Contains implementation for the shape window items
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <string.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <bwcc.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_wndlay.h>

#include "o_upsf.h"
#include "shp_attr.h"
#include "shp_prot.h"
#include "shapepal.h"
#include "shapeutl.h"
#include "tmp.h"
#include "xp_macro.h"

#if !defined(__FLAT__)
#include "heap.h"
#endif

// externs /////////////////////////////////////////////////////////////////

extern TYPEID   g_StdShapeLibType;

#if !defined(__FLAT__)

// ********************************************************************** //
//
// Overridden operator new and delete so we don't eat up selectors
// with small C++ objects.
//
// ********************************************************************** //

void* operator new
  (
  size_t    s
  )

  {
  return MemAlloc(s);
  }

void operator delete
  (
  void*     p
  )

  {
  if (p)
    MemFree(p);
  }

// ********************************************************************** //

#endif

// inline functions ////////////////////////////////////////////////////////

inline nMax
  (
  int n1,
  int n2
  )

  {
  return (n1>n2) ? n1 : n2;
  }

inline nMin
  (
  int n1,
  int n2
  )

  {
  return (n1<n2) ? n1 : n2;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItem
//
// METHOD:      Constructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpItem::ShpItem
  (
  OBJECTID  oiItem,
  OBJECTID  oiParent,
  RECT*     rcItem,
  long      lStatus,
  short     nShapeType
  ) : OpLiteWndItem(oiItem, oiParent, rcItem, lStatus),
      m_nShapeType(nShapeType),
      m_fStatus(0)

  {
  OWndLaySetItemIsShape(this, TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItem
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpItem::~ShpItem
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItem
//
// METHOD:      SizeToShape
//
// DESCRIPTION: This will do nothing.  The text item will override this to
//              automatically size itself upon creation.
//
////////////////////////////////////////////////////////////////////////////

void ShpItem::SizeToShape
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItem
//
// METHOD:      QueryProtocol
//
// DESCRIPTION: This is the overriden QueryProtocol for all shapes
//
////////////////////////////////////////////////////////////////////////////

LONG ALMAPI ShpItem::QueryProtocol
  (
  pApQueryInfo  pqi,
  PROTOCOLID    piProt
  )

  {
  LONG  rVal = A_OK;

  if(piProt==APROT_WND)
    {
    pqi->size      = sizeof(ApQueryInfo);
    pqi->object    = NULL;
    pqi->type      = g_StdShapeLibType;
    pqi->protocol  = APROT_WND;
    pqi->pfnCreate = (APPCFN)ShapePalette::ShapeCreate;
    }
  else
    rVal = A_NOTHANDLED;

  return rVal;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItem
//
// METHOD:      Type
//
// DESCRIPTION: This is the overriden Type method for all shapes
//
////////////////////////////////////////////////////////////////////////////

TYPEID ALMAPI ShpItem::Type
  (
  )

  {
  return  g_StdShapeLibType;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItemFill
//
// METHOD:      Constructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpItemFill::ShpItemFill
  (
  OBJECTID  oiItem,
  OBJECTID  oiParent,
  RECT*     rcItem,
  long      lStatus,
  short     nShapeType
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, nShapeType)

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItemFill
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpItemFill::~ShpItemFill
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItemFill
// 
// METHOD:      LaunchDialog
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

BOOL ShpItemFill::LaunchDialog
  (
  HWND  hwndParent,
  LPSTR szTitle
  )

  {
  SHPFILL_EDIT  shpedit = {szTitle, ShapePalette::GetCurrentUnits(), this};

  return DialogBoxParam(GetHInstance(), ID(DLG_FILLSHAPE), hwndParent, FilledShapeDlgProc, (long)&shpedit);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItemFill
//
// METHOD:      DrawSampleBox
//
// DESCRIPTION: This is the method to draw the sample box.  This sets up
//              the brush and pen and draws the background.
//
////////////////////////////////////////////////////////////////////////////

void ShpItemFill::DrawSampleBox
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  ShpItemFill*  shape = ((SHPFILL_EDIT*)GetWindowLong(hwnd, DWL_USER))->shape;
  LRESULT       lRes;
  HBRUSH        hbr, hbrOld;
  COLORREF      clrFGPen, clrBGPen;
  COLORREF      clrFG, clrFGOld;
  COLORREF      clrBG, clrBGOld;
  short         nStyle;
  short         nWidth;
  HBITMAP       hbmp;
  short         nPattern;

  // Draw the background for Borland dialog
  FillRect(lpdi->hDC, &lpdi->rcItem, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

  // Create and select the pen
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEFGCOLOR));
  clrFGPen = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEFGCOLOR), lRes);
  if(clrFGPen==COLOR_WINDOWFOREGROUND)
    clrFGPen = GetSysColor(COLOR_WINDOWTEXT);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEBGCOLOR));
  clrBGPen = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEBGCOLOR), lRes);
  if(clrBGPen==COLOR_WINDOWBACKGROUND)
    clrBGPen = GetSysColor(COLOR_WINDOW);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
  nStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);
  nWidth = LineAttrib::GetLineWidthFromDlg(hwnd); // in tenths of a millimeter
  if(nWidth > 0)
    {
    // convert to screen coordinates
    nWidth = (double)nWidth  * (double)GetDeviceCaps(lpdi->hDC, LOGPIXELSY) / 254. + 0.5;
    nStyle = (short)PS_INSIDEFRAME;
    }

  // Set the colors and mode
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_FILLFGCOLOR));
  clrFG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_FILLFGCOLOR), lRes);
  if(clrFG==COLOR_WINDOWFOREGROUND)
    clrFG = GetSysColor(COLOR_WINDOWTEXT);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_FILLBGCOLOR));
  clrBG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_FILLBGCOLOR), lRes);
  if(clrBG==COLOR_WINDOWBACKGROUND)
    clrBG = GetSysColor(COLOR_WINDOW);
  clrFGOld = SetTextColor(lpdi->hDC, clrFG);
  clrBGOld = SetBkColor(lpdi->hDC, clrBG);

  // Create and select the brush
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_FILLPATTERN));
  nPattern = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_FILLPATTERN), lRes);
  hbmp = CreateBitmap(8, 8, 1, 1, FillAttrib::GetFillPattern(nPattern));
  hbr = CreatePatternBrush(hbmp);
  hbrOld = (HBRUSH)SelectObject(lpdi->hDC, hbr);

  // have the actual shape draw the sample
  shape->DrawSample(hwnd, lpdi, clrFGPen, clrBGPen, nWidth, nStyle);

  // clean up the pen, brush, colors, etc.
  SelectObject(lpdi->hDC, hbrOld);
  DeleteObject(hbr);
  DeleteObject(hbmp);
  SetTextColor(lpdi->hDC, clrFGOld);
  SetBkColor(lpdi->hDC, clrBGOld);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItemFill
//
// METHOD:      SaveNewAttributes
//
// DESCRIPTION: This will save the new attributes to the shape.
//
////////////////////////////////////////////////////////////////////////////

BOOL ShpItemFill::SaveNewAttributes
  (
  HWND  hwnd
  )

  {
  ShpItemFill*  shape = ((SHPFILL_EDIT*)GetWindowLong(hwnd, DWL_USER))->shape;
  ATTR_FILL     fill;
  ATTR_LINE     line;
  LRESULT       lRes;

  // initialize to the old
  shape->GetAttributes(&line, &fill);

  // update the line attributes based on the dialog
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEFGCOLOR));
  line.clrFG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEFGCOLOR), lRes);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEBGCOLOR));
  line.clrBG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEBGCOLOR), lRes);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
  line.nStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);
  line.nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);

  // update the fill attributes based on the dialog
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_FILLFGCOLOR));
  fill.clrFG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_FILLFGCOLOR), lRes);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_FILLBGCOLOR));
  fill.clrBG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_FILLBGCOLOR), lRes);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_FILLPATTERN));
  fill.nPattern = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_FILLPATTERN), lRes);

  return shape->SetAttributes(&line, &fill);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItemFill
//
// METHOD:      FilledShapeDlgProc
//
// DESCRIPTION: This is the dialog proc for the filled shapes.
//
////////////////////////////////////////////////////////////////////////////

BOOL WINAPI EXPORT ShpItemFill::FilledShapeDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL          fRet = FALSE;
  ShpItemFill*  shape;
  short         nWidth;
  long          lRes;
  short         nStyle;

  switch(uMsg)
    {
    case WM_INITDIALOG:
      {
      AUtlCenterDialog(hwnd, 0);

      SHPFILL_EDIT* shpedit = (SHPFILL_EDIT*)lParam;
      ATTR_FILL     fill;
      ATTR_LINE     line;
      shape = shpedit->shape;
      SetWindowLong(hwnd, DWL_USER, (long)shpedit);
      shape->GetAttributes(&line, &fill);

      ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_LINEFGCOLOR), line.clrFG, TRUE, FALSE);
      ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_LINEBGCOLOR), line.clrBG, FALSE, FALSE);
      LineAttrib::FillLineStyleComboBox(GetDlgItem(hwnd, CB_LINESTYLE), line.nStyle);
      LineAttrib::InitLineWidth(hwnd, line.nWidth);

      ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_FILLFGCOLOR), fill.clrFG, TRUE, FALSE);
      ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_FILLBGCOLOR), fill.clrBG, FALSE, FALSE);
      FillAttrib::FillPatternComboBox(GetDlgItem(hwnd, CB_FILLPATTERN), fill.nPattern);

      if((line.nStyle==PS_3DIN) || (line.nStyle==PS_3DOUT))
        {
        SetDlgItemText(hwnd, ST_LINEFGCOLOR, "Shadow:");
        SetDlgItemText(hwnd, ST_LINEBGCOLOR, "Highlight:");
        EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), line.nWidth==0);
        EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), line.nWidth==0);
        }
      else
        {
        SetDlgItemText(hwnd, ST_LINEFGCOLOR, "Line Color:");
        SetDlgItemText(hwnd, ST_LINEBGCOLOR, "Line Background:");
        EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), FALSE);
        EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), FALSE);
        }

      if(shpedit->szTitle)
        SetWindowText(hwnd, shpedit->szTitle);

      fRet = TRUE;  // focus was not set.
      break;
      }

    case WM_DESTROY:
      break;

    case WM_MEASUREITEM:
      ColorAttrib::MeasureComboBox(hwnd, (LPMEASUREITEMSTRUCT)lParam);
      fRet = TRUE;
      break;

    case WM_DRAWITEM:
      {
      LPDRAWITEMSTRUCT  lpdi = (LPDRAWITEMSTRUCT)lParam;

      switch(lpdi->CtlID)
        {
        case CB_LINEFGCOLOR:
        case CB_LINEBGCOLOR:
        case CB_FILLFGCOLOR:
        case CB_FILLBGCOLOR:
          ColorAttrib::DrawColorComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case CB_LINESTYLE:
          LineAttrib::DrawLineStyleComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case CB_FILLPATTERN:
          FillAttrib::DrawPatternComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case PB_SAMPLE:
          DrawSampleBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        default:
          break;
        }
      }
      break;

    case WM_COMMAND:
      HANDLE_WM_COMMAND(hwnd, wParam, lParam, FilledShapeCommand);
      fRet = TRUE;
      break;

    case WM_VSCROLL:
      HANDLE_WM_VSCROLL_EX(hwnd, wParam, lParam, PB_SAMPLE, LineAttrib::WidthVScrollMsg);
      lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
      nStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);
      nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);
      EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)) && (nWidth==0));
      EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)) && (nWidth==0));
      fRet = TRUE;
      break;

    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpItemFill
//
// METHOD:      FilledShapeCommand
//
// DESCRIPTION: This is the message handler for the WM_COMMAND message.
//
////////////////////////////////////////////////////////////////////////////

void ShpItemFill::FilledShapeCommand
  (
  HWND  hwnd,
  int   id,
  HWND  hwndCtl,
  UINT  uNtfyCode
  )

  {
  I_UNUSED(hwndCtl);

  long  lRes;
  short nStyle;
  short nWidth;

  switch(id)
    {
    case IDOK:
      EndDialog(hwnd, SaveNewAttributes(hwnd));
      break;

    case IDCANCEL:
      EndDialog(hwnd, FALSE);
      break;

    case IDHELP:
      break;

    case CB_LINESTYLE:
      if(uNtfyCode == CBN_SELCHANGE)
        {
        lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
        nStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);
        nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);

        if((nStyle==PS_3DIN) || (nStyle==PS_3DOUT))
          {
          SetDlgItemText(hwnd, ST_LINEFGCOLOR, "Shadow:");
          SetDlgItemText(hwnd, ST_LINEBGCOLOR, "Highlight:");
          EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), nWidth==0);
          }
        else
          {
          SetDlgItemText(hwnd, ST_LINEFGCOLOR, "Line Color:");
          SetDlgItemText(hwnd, ST_LINEBGCOLOR, "Line Background:");
          EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), FALSE);
          EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), FALSE);
          }

        InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
        }
      break;

    case CB_LINEFGCOLOR:
    case CB_LINEBGCOLOR:
      if(uNtfyCode == CBN_SELCHANGE)
        InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
      break;

    case CB_FILLFGCOLOR:
    case CB_FILLBGCOLOR:
    case CB_FILLPATTERN:
      if(uNtfyCode == CBN_SELCHANGE)
        InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
      break;

    case RB_LINEWIDTH:
      LineAttrib::SetEditFocus(hwnd, ED_LINEWIDTH, FALSE);
      // fall through
    case RB_HAIRLINE:
      lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
      nStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);
      nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);

      EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
      EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);
      EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)) && (nWidth==0));
      EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)) && (nWidth==0));
      InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
      break;

    case ED_LINEWIDTH:
      switch(uNtfyCode)
        {
        case EN_KILLFOCUS:
          LineAttrib::SetItemValue(hwnd, id, LineAttrib::GetItemValue(hwnd, id));
          InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
          break;

        case EN_SETFOCUS:
          nWidth = LineAttrib::GetItemValue(hwnd, ED_LINEWIDTH);
          EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);

          CheckDlgButton(hwnd, RB_LINEWIDTH, TRUE);
          CheckDlgButton(hwnd, RB_HAIRLINE, FALSE);
          InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
          break;

        default:
          break;
        }

      lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
      nStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);
      nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);
      EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)) && (nWidth==0));
      EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)) && (nWidth==0));
      break;

    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a filled rectangle based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpRectangleFill::ShpRectangleFill
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_LINE*    line,
  ATTR_FILL*    fill
  ) : ShpItemFill(oiItem, oiParent, rcItem, lStatus, SHP_RECTFILL),
      m_line(line),
      m_fill(fill)

  {
  m_hbr = NULL;
  m_hpen = NULL;
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpRectangleFill::~ShpRectangleFill
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpRectangleFill::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Filled Rectangle", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      GetAttributes
//
// DESCRIPTION: This will set the fill and pen attributes for the editing
//              dialog.
//
////////////////////////////////////////////////////////////////////////////

void ShpRectangleFill::GetAttributes
  (
  ATTR_LINE*  line,
  ATTR_FILL*  fill
  )

  {
  m_line.PrepForFile(line);
  m_fill.PrepForFile(fill);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      SetAttributes
//
// DESCRIPTION: This will update the fill and pen attributes from the
//              editing dialog.
//
////////////////////////////////////////////////////////////////////////////

BOOL ShpRectangleFill::SetAttributes
  (
  const ATTR_LINE*  line,
  const ATTR_FILL*  fill
  )

  {
  BOOL  fRet;

  fRet = m_line.SetFGColor(line->clrFG);
  fRet = m_line.SetBGColor(line->clrBG)         || fRet;
  fRet = m_line.SetLineWidth(line->nWidth)      || fRet;
  fRet = m_line.SetLineStyle(line->nStyle)      || fRet;
  fRet = m_fill.SetFillPattern(fill->nPattern)  || fRet;
  fRet = m_fill.SetFGColor(fill->clrFG)         || fRet;
  fRet = m_fill.SetBGColor(fill->clrBG)         || fRet;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      DrawSample
//
// DESCRIPTION: This will draw a sample rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ShpRectangleFill::DrawSample
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi,
  COLORREF          clrFGPen,
  COLORREF          clrBGPen, 
  short             nWidth, 
  short             nStyle
  )

  {
  I_UNUSED(hwnd);

  HPEN  hpenOld;

  if((nWidth==0) && ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)))
    { // draw a sample 3d shape
    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nStyle==PS_3DIN) ? clrBGPen : clrFGPen));
    Rectangle(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom);
    DeleteObject(SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nStyle==PS_3DIN) ? clrFGPen : clrBGPen)));
    MoveToEx(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.bottom-1, NULL);
    LineTo(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top);
    LineTo(lpdi->hDC, lpdi->rcItem.right-1, lpdi->rcItem.top);
    DeleteObject(SelectObject(lpdi->hDC, hpenOld));
    }
  else
    {
    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(nStyle, nWidth, clrFGPen));
    Rectangle(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom);
    DeleteObject(SelectObject(lpdi->hDC, hpenOld));
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI  ShpRectangleFill::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI  ShpRectangleFill::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  rVal;

  if(LaunchDialog(GetActiveWindow(), "Edit Filled Rectangle"))
    rVal = A_MODIFIED;
  else
    rVal = A_OK;

  return rVal;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpRectangleFill::GetExtraData
  (
  )

  {
  MEMBLOCKID      mi = AMemAllocate(sizeof(SHAPE_RECTFILL));
  SHAPE_RECTFILL* shape = (SHAPE_RECTFILL*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_RECTFILL;
  m_line.PrepForFile(&shape->line);
  m_fill.PrepForFile(&shape->fill);

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a filled rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangleFill::ExportExtraData
  (
  )

  {
  m_line.Export();
  m_fill.Export();
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a filled rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangleFill::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);
  m_fill.Import(sVer);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      Draw3D
//
// DESCRIPTION: Draws this shape 3-dimensionally.
//
////////////////////////////////////////////////////////////////

void ShpRectangleFill::Draw3D
  (
  HDC hdc
  )
  
  {
  RECT  rc = m_rcItem;
  HPEN  hpenOld;

  hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
  Rectangle(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
  rc.right--;
  rc.bottom--;
  OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
  DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

  if(m_line.Is3DIn())
    {
    // highlight
    MoveToEx(hdc, rc.right, rc.top, NULL);
    LineTo(hdc, rc.right,   rc.bottom);
    LineTo(hdc, rc.left,    rc.bottom);
    }
  else
    {
    // highlight
    MoveToEx(hdc, rc.left, rc.bottom, NULL);
    LineTo(hdc, rc.left,   rc.top);
    LineTo(hdc, rc.right,  rc.top);
    }

  DeleteObject(SelectObject(hdc, hpenOld));
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI  ShpRectangleFill::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  HBRUSH    hbrOld;
  COLORREF  clrFGOld, clrBGOld;
  RECT      rc;

  UnrealizeObject(m_hbr);
  CopyRect(&rc, &m_rcItem);
  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 1);
  MapWindowPoints(m_hwndParent, HWND_DESKTOP, (LPPOINT)&rc, 1);
  SetBrushOrgEx(hdc, rc.left, rc.top, NULL);

  hbrOld = (HBRUSH)SelectObject(hdc, m_hbr);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  if(m_line.IsLine3D())
    Draw3D(hdc);
  else
    {
    HPEN  hpenOld = (HPEN)SelectObject(hdc, m_hpen);

    Rectangle(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    SelectObject(hdc, hpenOld);
    }

  SelectObject(hdc, hbrOld);
  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangleFill::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  m_hbr = m_fill.m_CreateBrush();
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangleFill::ParentClosing
  (
  )

  {
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  m_fill.m_DeleteBrush(m_hbr);
  m_hbr = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: This will draw the grab handles with no lines
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangleFill::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: This is the overriden InvalidateGrabHandles that will
//              not draw a bounding rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangleFill::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangleFill
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangleFill::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  HBRUSH    hbr = m_fill.m_CreateBrush();
  HPEN      hpen = m_line.m_CreatePen(NULL, hdc);
  HBRUSH    hbrOld;
  HPEN      hpenOld;
  COLORREF  clrFGOld, clrBGOld;
  RECT      rc;

  UnrealizeObject(hbr);
  CopyRect(&rc, lprcPrint);
  SetBrushOrgEx(hdc, rc.left, rc.top, NULL);

  hbrOld = (HBRUSH)SelectObject(hdc, hbr);
  hpenOld = (HPEN)SelectObject(hdc, hpen);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  m_fill.m_DeleteBrush(hbr);
  m_line.m_DeletePen(hpen);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a rectangle based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpRectangle::ShpRectangle
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_LINE*    line
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_RECT),
      m_line(line)

  {
  OWndSetItemTransparent(this, TRUE);
  m_hpen = NULL;
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpRectangle::~ShpRectangle
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpRectangle::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Rectangle", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI  ShpRectangle::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI  ShpRectangle::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  rVal;

  if(m_line.EditLine(GetActiveWindow(), ShapePalette::GetCurrentUnits(), "Edit Rectangle"))
    rVal = A_MODIFIED;
  else
    rVal = A_OK;

  return rVal;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpRectangle::GetExtraData
  (
  )

  {
  MEMBLOCKID  mi = AMemAllocate(sizeof(SHAPE_RECT));
  SHAPE_RECT* shape = (SHAPE_RECT*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_RECT;
  m_line.PrepForFile(&shape->line);

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangle::ExportExtraData
  (
  )

  {
  m_line.Export();
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangle::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      Draw3D
//
// DESCRIPTION: Draws this shape 3-dimensionally.
//
////////////////////////////////////////////////////////////////

void ShpRectangle::Draw3D
  (
  HDC hdc
  )
  
  {
  RECT  rc = m_rcItem;
  HPEN  hpenOld;

  if(m_line.Is3DIn())
    {
    // shadow
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.right--;
    rc.bottom--;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

    // highlight
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left++;
    rc.top++;
    rc.right++;
    rc.bottom++;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  else
    {
    // shadow
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left++;
    rc.top++;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

    // highlight
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left--;
    rc.top--;
    rc.right--;
    rc.bottom--;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI  ShpRectangle::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HBRUSH    hbrOld  = (HBRUSH)SelectObject(hdc, hbr);

  if(m_line.IsLine3D())
    Draw3D(hdc);
  else
    {
    HPEN      hpenOld = (HPEN)SelectObject(hdc, m_hpen);
    int       nOldBkMode;
    COLORREF  clrOldBG;

    if(m_line.IsBGTransparent())
      nOldBkMode = SetBkMode(hdc, TRANSPARENT);
    else
      clrOldBG = SetBkColor(hdc, m_line.BGColor());

    Rectangle(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

    if(m_line.IsBGTransparent())
      SetBkMode(hdc, nOldBkMode);
    else
      SetBkColor(hdc, clrOldBG);

    SelectObject(hdc, hpenOld);
    }

  SelectObject(hdc, hbrOld);
  DeleteObject(hbr);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangle::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangle::ParentClosing
  (
  )

  {
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: This will draw the grab handles with no lines
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangle::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: This is the overriden InvalidateGrabHandles that will
//              not draw a bounding rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangle::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      PointInItem
//
// DESCRIPTION: This will return TRUE if the point is in the item.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpRectangle::PointInItem
  (
  POINT*  lppt
  )

  {
  BOOL  fRet;
  int   nDeflate    = m_line.LineWidth();
  RECT  rc;

  CopyRect(&rc, &m_rcItem);
  if(nDeflate < 2*DOT_SIZE)
    {
    if(nDeflate < DOT_SIZE)
      nDeflate = DOT_SIZE;

    InflateRect(&rc, 2*DOT_SIZE-nDeflate, 2*DOT_SIZE-nDeflate);
    }

  fRet = PtInRect(&rc, *lppt);
  if(fRet)
    {
    CopyRect(&rc, &m_rcItem);
    InflateRect(&rc, -nDeflate, -nDeflate);
    fRet = !PtInRect(&rc, *lppt);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      ItemInRect
//
// DESCRIPTION: This will return TRUE if the item is inside the rectangle.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpRectangle::ItemInRect
  (
  RECT* lprc,
  BOOL  fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    fRet = ((lprc->left   <= m_rcItem.left)
                 &&
            (lprc->top    <= m_rcItem.top)
                 &&
            (lprc->right  >= m_rcItem.right)
                 &&
            (lprc->bottom >= m_rcItem.bottom));
    }
  else
    {
    int   nWidth = nMax(1, m_line.LineWidth());
    int   nHeight = nWidth;
    RECT  rc1, rc2, rc3, rc4, rcIntr;

    nWidth  = nMin(nWidth,  (m_rcItem.right  - m_rcItem.left)/2);
    nHeight = nMin(nHeight, (m_rcItem.bottom - m_rcItem.top )/2);

    SetRect(&rc1, m_rcItem.left, m_rcItem.top, m_rcItem.left + nWidth, m_rcItem.bottom);
    SetRect(&rc2, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top + nHeight);
    SetRect(&rc3, m_rcItem.right - nWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    SetRect(&rc4, m_rcItem.left, m_rcItem.bottom - nHeight, m_rcItem.right, m_rcItem.bottom);

    fRet = IntersectRect(&rcIntr, &rc1, lprc)
                ||
           IntersectRect(&rcIntr, &rc2, lprc)
                ||
           IntersectRect(&rcIntr, &rc3, lprc)
                ||
           IntersectRect(&rcIntr, &rc4, lprc);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRectangle
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRectangle::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HPEN      hpen = m_line.m_CreatePen(NULL, hdc);
  HBRUSH    hbrOld;
  HPEN      hpenOld;
  int       nOldBkMode;
  COLORREF  clrOldBG;

  if(m_line.IsBGTransparent())
    nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  else
    clrOldBG = SetBkColor(hdc, m_line.BGColor());

  hbrOld = (HBRUSH)SelectObject(hdc, hbr);
  hpenOld = (HPEN)SelectObject(hdc, hpen);

  Rectangle(hdc, lprcPrint->left, lprcPrint->top, lprcPrint->right, lprcPrint->bottom);

  if(m_line.IsBGTransparent())
    SetBkMode(hdc, nOldBkMode);
  else
    SetBkColor(hdc, clrOldBG);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
  DeleteObject(hbr);
  m_line.m_DeletePen(hpen);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a filled ellipse based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpEllipseFill::ShpEllipseFill
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_LINE*    line,
  ATTR_FILL*    fill
  ) : ShpItemFill(oiItem, oiParent, rcItem, lStatus, SHP_ELLIPSEFILL),
      m_line(line),
      m_fill(fill)

  {
  OWndSetItemTransparent(this, TRUE);
  m_hbr = NULL;
  m_hpen = NULL;
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpEllipseFill::~ShpEllipseFill
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpEllipseFill::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Filled Ellipse/Circle", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      GetAttributes
//
// DESCRIPTION: This will set the fill and pen attributes for the editing
//              dialog.
//
////////////////////////////////////////////////////////////////////////////

void ShpEllipseFill::GetAttributes
  (
  ATTR_LINE*  line,
  ATTR_FILL*  fill
  )

  {
  m_line.PrepForFile(line);
  m_fill.PrepForFile(fill);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      SetAttributes
//
// DESCRIPTION: This will update the fill and pen attributes from the
//              editing dialog.
//
////////////////////////////////////////////////////////////////////////////

BOOL ShpEllipseFill::SetAttributes
  (
  const ATTR_LINE*  line,
  const ATTR_FILL*  fill
  )

  {
  BOOL  fRet;

  fRet = m_line.SetFGColor(line->clrFG);
  fRet = m_line.SetBGColor(line->clrBG)         || fRet;
  fRet = m_line.SetLineWidth(line->nWidth)      || fRet;
  fRet = m_line.SetLineStyle(line->nStyle)      || fRet;
  fRet = m_fill.SetFillPattern(fill->nPattern)  || fRet;
  fRet = m_fill.SetFGColor(fill->clrFG)         || fRet;
  fRet = m_fill.SetBGColor(fill->clrBG)         || fRet;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      DrawSample
//
// DESCRIPTION: This will draw a sample ellipse.
//
////////////////////////////////////////////////////////////////////////////

void ShpEllipseFill::DrawSample
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi,
  COLORREF          clrFGPen,
  COLORREF          clrBGPen, 
  short             nWidth, 
  short             nStyle
  )

  {
  I_UNUSED(hwnd);

  HPEN  hpenOld;

  if((nWidth==0) && ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)))
    { // draw a sample 3d shape
    POINT ptLB, ptRT;

    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nStyle==PS_3DIN) ? clrBGPen : clrFGPen));
    Ellipse(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom);
    DeleteObject(SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nStyle==PS_3DIN) ? clrFGPen : clrBGPen)));

    Ellipse3DShadowPoints(&lpdi->rcItem, &ptLB, &ptRT);
    Arc(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom, ptRT.x, ptRT.y, ptLB.x, ptLB.y);

    DeleteObject(SelectObject(lpdi->hDC, hpenOld));
    }
  else
    {
    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(nStyle, nWidth, clrFGPen));
    Ellipse(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom);
    DeleteObject(SelectObject(lpdi->hDC, hpenOld));
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpEllipseFill::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  rVal;

  if(LaunchDialog(GetActiveWindow(), "Edit Filled Ellipse/Circle"))
    rVal = A_MODIFIED;
  else
    rVal = A_OK;

  return rVal;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpEllipseFill::GetExtraData
  (
  )

  {
  MEMBLOCKID          mi = AMemAllocate(sizeof(SHAPE_ELLIPSEFILL));
  SHAPE_ELLIPSEFILL*  shape = (SHAPE_ELLIPSEFILL*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_ELLIPSEFILL;
  m_line.PrepForFile(&shape->line);
  m_fill.PrepForFile(&shape->fill);

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a filled ellipse
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::ExportExtraData
  (
  )

  {
  m_line.Export();
  m_fill.Export();
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a filled ellipse
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);
  m_fill.Import(sVer);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      Draw3D
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ShpEllipseFill::Draw3D
  (
  HDC hdc
  )

  {
  HPEN  hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
  POINT ptLB, ptRT;

  Ellipse(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
  Ellipse3DShadowPoints(&m_rcItem, &ptLB, &ptRT);
  DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

  if(m_line.Is3DIn())
    Arc(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, ptLB.x, ptLB.y, ptRT.x, ptRT.y);
  else
    Arc(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, ptRT.x, ptRT.y, ptLB.x, ptLB.y);

  DeleteObject(SelectObject(hdc, hpenOld));
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  HBRUSH    hbrOld;
  COLORREF  clrFGOld, clrBGOld;
  RECT      rc;

  UnrealizeObject(m_hbr);
  CopyRect(&rc, &m_rcItem);
  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 1);
  MapWindowPoints(m_hwndParent, HWND_DESKTOP, (LPPOINT)&rc, 1);
  SetBrushOrgEx(hdc, rc.left, rc.top, NULL);

  hbrOld = (HBRUSH)SelectObject(hdc, m_hbr);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  if(m_line.IsLine3D())
    Draw3D(hdc);
  else
    {
    HPEN  hpenOld = (HPEN)SelectObject(hdc, m_hpen);

    Ellipse(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    SelectObject(hdc, hpenOld);
    }

  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  SelectObject(hdc, hbrOld);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      DrawTrackingRect
//
// DESCRIPTION: This method draws the tracking shape for the shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::DrawTrackingRect
  (
  HDC   hdc,
  RECT* rect
  )

  {
  Arc(hdc, rect->left, rect->top, rect->right, rect->bottom, 0, 0, 0, 0);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  m_hbr = m_fill.m_CreateBrush();
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::ParentClosing
  (
  )

  {
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  m_fill.m_DeleteBrush(m_hbr);
  m_hbr = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: This will draw the grab handles with no lines
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: This is the overriden InvalidateGrabHandles that will
//              not draw a bounding rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      PointInItem
//
// DESCRIPTION: This will return TRUE if cursor is inside the ellipse.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpEllipseFill::PointInItem
  (
  POINT*  pt
  )

  {
  return PointInEllipse(pt, &m_rcItem);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      ItemInRect
//
// DESCRIPTION: This will return TRUE if the ellipse is inside the rectangle.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpEllipseFill::ItemInRect
  (
  RECT* lprc,
  BOOL  fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    fRet = ((lprc->left   <= m_rcItem.left)
                 &&
            (lprc->top    <= m_rcItem.top)
                 &&
            (lprc->right  >= m_rcItem.right)
                 &&
            (lprc->bottom >= m_rcItem.bottom));
    }
  else
    {
    HRGN  hrgn = CreateEllipticRgnIndirect(&m_rcItem);
    fRet = RectInRegion(hrgn, lprc);
    DeleteObject(hrgn);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipseFill
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipseFill::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  HBRUSH    hbr = m_fill.m_CreateBrush();
  HPEN      hpen = m_line.m_CreatePen(NULL, hdc);
  HBRUSH    hbrOld;
  HPEN      hpenOld;
  COLORREF  clrFGOld, clrBGOld;
  RECT      rc;

  UnrealizeObject(hbr);
  CopyRect(&rc, lprcPrint);
  SetBrushOrgEx(hdc, rc.left, rc.top, NULL);

  hbrOld = (HBRUSH)SelectObject(hdc, hbr);
  hpenOld = (HPEN)SelectObject(hdc, hpen);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  m_fill.m_DeleteBrush(hbr);
  m_line.m_DeletePen(hpen);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize an ellipse based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpEllipse::ShpEllipse
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_LINE*    line
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_ELLIPSE),
      m_line(line)

  {
  OWndSetItemTransparent(this, TRUE);
  m_hpen = NULL;
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpEllipse::~ShpEllipse
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpEllipse::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Ellipse/Circle", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpEllipse::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  rVal;

  if(m_line.EditLine(GetActiveWindow(), ShapePalette::GetCurrentUnits(), "Edit Ellipse/Circle"))
    rVal = A_MODIFIED;
  else
    rVal = A_OK;

  return rVal;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpEllipse::GetExtraData
  (
  )

  {
  MEMBLOCKID      mi = AMemAllocate(sizeof(SHAPE_ELLIPSE));
  SHAPE_ELLIPSE*  shape = (SHAPE_ELLIPSE*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_ELLIPSE;
  m_line.PrepForFile(&shape->line);

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports an ellipse
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::ExportExtraData
  (
  )

  {
  m_line.Export();
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports an ellipse
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      Draw3D
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ShpEllipse::Draw3D
  (
  HDC   hdc
  )

  {
  RECT  rc = m_rcItem;
  HPEN  hpenOld;

  if(m_line.Is3DIn())
    {
    // shadow
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.right--;
    rc.bottom--;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

    // highlight
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left++;
    rc.top++;
    rc.right++;
    rc.bottom++;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  else
    {
    // shadow
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left++;
    rc.top++;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

    // highlight
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left--;
    rc.top--;
    rc.right--;
    rc.bottom--;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HBRUSH    hbrOld  = (HBRUSH)SelectObject(hdc, hbr);

  if(m_line.IsLine3D())
    Draw3D(hdc);
  else
    {
    HPEN      hpenOld     = (HPEN)SelectObject(hdc, m_hpen);
    int       nOldBkMode;
    COLORREF  clrOldBG;

    if(m_line.IsBGTransparent())
      nOldBkMode = SetBkMode(hdc, TRANSPARENT);
    else
      clrOldBG = SetBkColor(hdc, m_line.BGColor());

    Ellipse(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

    if(m_line.IsBGTransparent())
      SetBkMode(hdc, nOldBkMode);
    else
      SetBkColor(hdc, clrOldBG);

    SelectObject(hdc, hpenOld);
    }

  SelectObject(hdc, hbrOld);
  DeleteObject(hbr);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      DrawTrackingRect
//
// DESCRIPTION: This method draws the tracking shape for the shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::DrawTrackingRect
  (
  HDC   hdc,
  RECT* rect
  )

  {
  Arc(hdc, rect->left, rect->top, rect->right, rect->bottom, 0, 0, 0, 0);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::ParentClosing
  (
  )

  {
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: This will draw the grab handles with no lines
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: This is the overriden InvalidateGrabHandles that will
//              not draw a bounding rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      PointInItem
//
// DESCRIPTION: This will return TRUE if cursor is inside the ellipse.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpEllipse::PointInItem
  (
  POINT*  pt
  )

  {
  BOOL  fRet;
  int   nDeflate    = m_line.LineWidth();
  RECT  rc;

  CopyRect(&rc, &m_rcItem);
  if(nDeflate < 2*DOT_SIZE)
    {
    if(nDeflate < DOT_SIZE)
      nDeflate = DOT_SIZE;

    InflateRect(&rc, 2*DOT_SIZE-nDeflate, 2*DOT_SIZE-nDeflate);
    }

  fRet = PointInEllipse(pt, &rc);
  if(fRet)
    {
    CopyRect(&rc, &m_rcItem);
    InflateRect(&rc, -nDeflate, -nDeflate);
    fRet = !PointInEllipse(pt, &rc);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      ItemInRect
//
// DESCRIPTION: This will return TRUE if the ellipse is inside the rectangle.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpEllipse::ItemInRect
  (
  RECT* lprc,
  BOOL  fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    fRet = ((lprc->left   <= m_rcItem.left)
                 &&
            (lprc->top    <= m_rcItem.top)
                 &&
            (lprc->right  >= m_rcItem.right)
                 &&
            (lprc->bottom >= m_rcItem.bottom));
    }
  else
    {
    RECT  rcTemp;
    POINT ptSize = {DOT_SIZE, DOT_SIZE};
    int   nDeflate;
    HRGN  hrgnOutside;
    HRGN  hrgnInside;
    HRGN  hrgnEllipse;

    hrgnOutside = CreateEllipticRgnIndirect(&m_rcItem);

    nDeflate = -nMax(ptSize.x, m_line.LineWidth());
    CopyRect(&rcTemp, &m_rcItem);
    InflateRect(&rcTemp, nDeflate, nDeflate);
    hrgnInside = CreateEllipticRgnIndirect(&rcTemp);

    hrgnEllipse = CreateRectRgn(0, 0, 0, 0);
    CombineRgn(hrgnEllipse, hrgnOutside, hrgnInside, RGN_DIFF);

    fRet = RectInRegion(hrgnEllipse, lprc);

    DeleteObject(hrgnOutside);
    DeleteObject(hrgnInside);
    DeleteObject(hrgnEllipse);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    FixRoundRectRadius
//
// DESCRIPTION: Converts rounded rectangle radii to a point.
//
////////////////////////////////////////////////////////////////////////////

void FixRoundRectRadius
  (
  POINT       ptRadius,
  const RECT* lprc,
  LPPOINT     lppt
  )

  {
  // if the both radii are negative, then the radius is 1/5 the minimum of
  // the width or height of the rectangle.

  if((ptRadius.x<0) && (ptRadius.y<0))
    {
    lppt->x = (lprc->right - lprc->left)/4;
    lppt->y = (lprc->bottom - lprc->top)/4;
    (lppt->x<lppt->y) ? (lppt->y = lppt->x) : (lppt->x = lppt->y);
    }
  else if(ptRadius.x<0)
    lppt->x = lppt->y = ptRadius.y;
  else if(ptRadius.y<0)
    lppt->x = lppt->y = ptRadius.x;
  else
    {
    lppt->x = ptRadius.x;
    lppt->y = ptRadius.y;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpEllipse
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpEllipse::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HPEN      hpen = m_line.m_CreatePen(NULL, hdc);
  HBRUSH    hbrOld;
  HPEN      hpenOld;
  int       nOldBkMode;
  COLORREF  clrOldBG;

  if(m_line.IsBGTransparent())
    nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  else
    clrOldBG = SetBkColor(hdc, m_line.BGColor());

  hbrOld = (HBRUSH)SelectObject(hdc, hbr);
  hpenOld = (HPEN)SelectObject(hdc, hpen);

  Ellipse(hdc, lprcPrint->left, lprcPrint->top, lprcPrint->right, lprcPrint->bottom);

  if(m_line.IsBGTransparent())
    SetBkMode(hdc, nOldBkMode);
  else
    SetBkColor(hdc, clrOldBG);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
  DeleteObject(hbr);
  m_line.m_DeletePen(hpen);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a filled round rectangle based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpRoundRectFill::ShpRoundRectFill
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_LINE*    line,
  ATTR_FILL*    fill,
  short         xRad,
  short         yRad
  ) : ShpItemFill(oiItem, oiParent, rcItem, lStatus, SHP_ROUNDRECTFILL),
      m_line(line),
      m_fill(fill),
      m_hbr(NULL),
      m_hpen(NULL)

  {
  m_ptRadius.x = xRad;
  m_ptRadius.y = yRad;

  OWndSetItemTransparent(this, TRUE);
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpRoundRectFill::~ShpRoundRectFill
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpRoundRectFill::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Filled Rounded Rectangle", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      GetAttributes
//
// DESCRIPTION: This will set the fill and pen attributes for the editing
//              dialog.
//
////////////////////////////////////////////////////////////////////////////

void ShpRoundRectFill::GetAttributes
  (
  ATTR_LINE*  line,
  ATTR_FILL*  fill
  )

  {
  m_line.PrepForFile(line);
  m_fill.PrepForFile(fill);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      SetAttributes
//
// DESCRIPTION: This will update the fill and pen attributes from the
//              editing dialog.
//
////////////////////////////////////////////////////////////////////////////

BOOL ShpRoundRectFill::SetAttributes
  (
  const ATTR_LINE*  line,
  const ATTR_FILL*  fill
  )

  {
  BOOL  fRet;

  fRet = m_line.SetFGColor(line->clrFG);
  fRet = m_line.SetBGColor(line->clrBG)         || fRet;
  fRet = m_line.SetLineWidth(line->nWidth)      || fRet;
  fRet = m_line.SetLineStyle(line->nStyle)      || fRet;
  fRet = m_fill.SetFillPattern(fill->nPattern)  || fRet;
  fRet = m_fill.SetFGColor(fill->clrFG)         || fRet;
  fRet = m_fill.SetBGColor(fill->clrBG)         || fRet;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      DrawSample
//
// DESCRIPTION: This will draw a sample round rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ShpRoundRectFill::DrawSample
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi,
  COLORREF          clrFGPen,
  COLORREF          clrBGPen, 
  short             nWidth, 
  short             nStyle
  )

  {
  I_UNUSED(hwnd);

  POINT ptRadius = {-1, -1};  // have the roundness be based on rectangle size
  POINT pt;
  HPEN  hpenOld;

  FixRoundRectRadius(ptRadius, &lpdi->rcItem, &pt);

  if((nWidth==0) && ((nStyle==PS_3DIN) || (nStyle==PS_3DOUT)))
    { // draw a sample 3d shape
    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nStyle==PS_3DIN) ? clrBGPen : clrFGPen));
    RoundRect(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom, pt.x, pt.y);
    DeleteObject(SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nStyle==PS_3DIN) ? clrFGPen : clrBGPen)));
    RoundRectFrameTL(lpdi->hDC, &lpdi->rcItem, &pt);
    DeleteObject(SelectObject(lpdi->hDC, hpenOld));
    }
  else
    {
    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(nStyle, nWidth, clrFGPen));
    RoundRect(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom, pt.x, pt.y);
    DeleteObject(SelectObject(lpdi->hDC, hpenOld));
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpRoundRectFill::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  rVal;

  if(LaunchDialog(GetActiveWindow(), "Edit Filled Rounded Rectangle"))
    rVal = A_MODIFIED;
  else
    rVal = A_OK;

  return rVal;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpRoundRectFill::GetExtraData
  (
  )

  {
  MEMBLOCKID            mi = AMemAllocate(sizeof(SHAPE_ROUNDRECTFILL));
  SHAPE_ROUNDRECTFILL*  shape = (SHAPE_ROUNDRECTFILL*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_ROUNDRECTFILL;
  m_line.PrepForFile(&shape->line);
  m_fill.PrepForFile(&shape->fill);
  shape->xRad       = (short)m_ptRadius.x;
  shape->yRad       = (short)m_ptRadius.y;

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a filled rounded rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::ExportExtraData
  (
  )

  {
  m_line.Export();
  m_fill.Export();

  ExportPoint("PTRADIUS", 0L, &m_ptRadius);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a filled rounded rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);
  m_fill.Import(sVer);

  m_ptRadius.x = m_ptRadius.y = -1;
  ImportPoint("PTRADIUS", 0L, &m_ptRadius);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      Draw3D
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ShpRoundRectFill::Draw3D
  (
  HDC     hdc,
  LPPOINT lppt
  )

  {
  HPEN      hpenOld;
  COLORREF  clrLT = m_line.Is3DIn() ? m_line.FGColor() : m_line.BGColor();
  COLORREF  clrRB = m_line.Is3DIn() ? m_line.BGColor() : m_line.FGColor();

  hpenOld = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
  RoundRect(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, lppt->x, lppt->y);
  SelectObject(hdc, hpenOld);

  RoundRect3DFrame(hdc, &m_rcItem, lppt, clrLT, clrRB);
/*
  //  -- this should be used as soon as the RoundRectFrameTL function is fixed    
  hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.FGColor() : m_line.BGColor()));
  RoundRect(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, lppt->x, lppt->y);
  DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.BGColor() : m_line.FGColor())));
  RoundRectFrameTL(hdc, &m_rcItem, lppt);
  DeleteObject(SelectObject(hdc, hpenOld));
*/
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  HBRUSH    hbrOld;
  HPEN      hpenOld;
  COLORREF  clrFGOld, clrBGOld;
  POINT     pt;
  RECT      rc;

  FixRoundRectRadius(m_ptRadius, &m_rcItem, &pt);

  UnrealizeObject(m_hbr);
  CopyRect(&rc, &m_rcItem);
  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 1);
  MapWindowPoints(m_hwndParent, HWND_DESKTOP, (LPPOINT)&rc, 1);
  SetBrushOrgEx(hdc, rc.left, rc.top, NULL);

  hbrOld = (HBRUSH)SelectObject(hdc, m_hbr);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  if(m_line.IsLine3D())
    Draw3D(hdc, &pt);
  else
    {
    hpenOld = (HPEN)SelectObject(hdc, m_hpen);
    RoundRect(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, pt.x, pt.y);
    SelectObject(hdc, hpenOld);
    }

  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  SelectObject(hdc, hbrOld);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      DrawTrackingRect
//
// DESCRIPTION: This method draws the tracking shape for the shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::DrawTrackingRect
  (
  HDC   hdc,
  RECT* rect
  )

  {
  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HBRUSH    hbrOld  = (HBRUSH)SelectObject(hdc, hbr);
  POINT     pt;

  FixRoundRectRadius(m_ptRadius, rect, &pt);
  RoundRect(hdc, rect->left, rect->top, rect->right, rect->bottom, pt.x, pt.y);

  SelectObject(hdc, hbrOld);
  DeleteObject(hbr);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  m_hbr = m_fill.m_CreateBrush();
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::ParentClosing
  (
  )

  {
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  m_fill.m_DeleteBrush(m_hbr);
  m_hbr = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: This will draw the grab handles with no lines
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: This is the overriden InvalidateGrabHandles that will
//              not draw a bounding rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      ItemInRect
//
// DESCRIPTION: This will return TRUE if the ellipse is inside the rectangle.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpRoundRectFill::ItemInRect
  (
  RECT* lprc,
  BOOL  fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    fRet = ((lprc->left   <= m_rcItem.left)
                 &&
            (lprc->top    <= m_rcItem.top)
                 &&
            (lprc->right  >= m_rcItem.right)
                 &&
            (lprc->bottom >= m_rcItem.bottom));
    }
  else
    {
    HRGN  hrgn;
    POINT pt;

    FixRoundRectRadius(m_ptRadius, &m_rcItem, &pt);
    hrgn = CreateRoundRectRgn(m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, pt.x, pt.y);
    fRet = RectInRegion(hrgn, lprc);

    DeleteObject(hrgn);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRectFill
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRectFill::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  HBRUSH    hbr = m_fill.m_CreateBrush();
  HPEN      hpen = m_line.m_CreatePen(NULL, hdc);
  HBRUSH    hbrOld;
  HPEN      hpenOld;
  COLORREF  clrFGOld, clrBGOld;
  RECT      rc;
  POINT     pt;

  UnrealizeObject(hbr);
  CopyRect(&rc, lprcPrint);
  SetBrushOrgEx(hdc, rc.left, rc.top, NULL);

  hbrOld = (HBRUSH)SelectObject(hdc, hbr);
  hpenOld = (HPEN)SelectObject(hdc, hpen);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  FixRoundRectRadius(m_ptRadius, lprcPrint, &pt);
  RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, pt.x, pt.y);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  m_fill.m_DeleteBrush(hbr);
  m_line.m_DeletePen(hpen);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a round rectangle based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpRoundRect::ShpRoundRect
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_LINE*    line,
  short         xRad,
  short         yRad
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_ROUNDRECT),
      m_line(line),
      m_hpen(NULL)

  {
  m_ptRadius.x = xRad;
  m_ptRadius.y = yRad;

  OWndSetItemTransparent(this, TRUE);
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpRoundRect::~ShpRoundRect
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpRoundRect::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Rounded Rectangle", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpRoundRect::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  rVal;

  if(m_line.EditLine(GetActiveWindow(), ShapePalette::GetCurrentUnits(), "Edit Rounded Rectangle"))
    rVal = A_MODIFIED;
  else
    rVal = A_OK;

  return rVal;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpRoundRect::GetExtraData
  (
  )

  {
  MEMBLOCKID        mi = AMemAllocate(sizeof(SHAPE_ROUNDRECT));
  SHAPE_ROUNDRECT*  shape = (SHAPE_ROUNDRECT*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_ROUNDRECT;
  m_line.PrepForFile(&shape->line);
  shape->xRad       = (short)m_ptRadius.x;
  shape->yRad       = (short)m_ptRadius.y;

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a rounded rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::ExportExtraData
  (
  )

  {
  m_line.Export();

  ExportPoint("PTRADIUS", 0L, &m_ptRadius);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a rounded rectangle
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);

  m_ptRadius.x = m_ptRadius.y = -1;
  ImportPoint("PTRADIUS", 0L, &m_ptRadius);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      Draw3D
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ShpRoundRect::Draw3D
  (
  HDC     hdc,
  LPPOINT lppt
  )

  {
  RECT  rc = m_rcItem;
  HPEN  hpenOld;

  if(m_line.Is3DIn())
    {
    // shadow
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.right--;
    rc.bottom--;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, lppt->x, lppt->y);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

    // highlight
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left++;
    rc.top++;
    rc.right++;
    rc.bottom++;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, lppt->x, lppt->y);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  else
    {
    // shadow
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left++;
    rc.top++;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.FGColor()));
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, lppt->x, lppt->y);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.BGColor())));

    // highlight
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    rc.left--;
    rc.top--;
    rc.right--;
    rc.bottom--;
    OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, lppt->x, lppt->y);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HBRUSH    hbrOld  = (HBRUSH)SelectObject(hdc, hbr);
  POINT     pt;

  FixRoundRectRadius(m_ptRadius, &m_rcItem, &pt);
  if(m_line.IsLine3D())
    Draw3D(hdc, &pt);
  else
    {
    HPEN      hpenOld     = (HPEN)SelectObject(hdc, m_hpen);
    int       nOldBkMode  = m_line.IsBGTransparent() ? SetBkMode(hdc, TRANSPARENT) : 0;
    COLORREF  clrOldBG    = !m_line.IsBGTransparent() ? SetBkColor(hdc, m_line.BGColor()) : 0;

    RoundRect(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, pt.x, pt.y);

    if(m_line.IsBGTransparent())
      SetBkMode(hdc, nOldBkMode);
    else
      SetBkColor(hdc, clrOldBG);

    SelectObject(hdc, hpenOld);
    }

  SelectObject(hdc, hbrOld);
  DeleteObject(hbr);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      DrawTrackingRect
//
// DESCRIPTION: This method draws the tracking shape for the shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::DrawTrackingRect
  (
  HDC   hdc,
  RECT* rect
  )

  {
  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HBRUSH    hbrOld  = (HBRUSH)SelectObject(hdc, hbr);
  POINT     pt;

  FixRoundRectRadius(m_ptRadius, rect, &pt);
  RoundRect(hdc, rect->left, rect->top, rect->right, rect->bottom, pt.x, pt.y);

  SelectObject(hdc, hbrOld);
  DeleteObject(hbr);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::ParentClosing
  (
  )

  {
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRountRect
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: This will draw the grab handles with no lines
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: This is the overriden InvalidateGrabHandles that will
//              not draw a bounding rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      ItemInRect
//
// DESCRIPTION: This will return TRUE if the round rect is inside the rectangle.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpRoundRect::ItemInRect
  (
  RECT* lprc,
  BOOL  fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    fRet = ((lprc->left   <= m_rcItem.left)
                 &&
            (lprc->top    <= m_rcItem.top)
                 &&
            (lprc->right  >= m_rcItem.right)
                 &&
            (lprc->bottom >= m_rcItem.bottom));
    }
  else
    {
    RECT  rcTemp;
    POINT ptSize = {DOT_SIZE, DOT_SIZE};
    int   nDeflate;
    HRGN  hrgnOutside;
    HRGN  hrgnInside;
    HRGN  hrgnRRect;
    POINT pt;

    FixRoundRectRadius(m_ptRadius, &m_rcItem, &pt);
    hrgnOutside = CreateRoundRectRgn(m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, pt.x, pt.y);

    nDeflate = -nMax(ptSize.x, m_line.LineWidth());
    CopyRect(&rcTemp, &m_rcItem);
    InflateRect(&rcTemp, nDeflate, nDeflate);
    FixRoundRectRadius(m_ptRadius, &rcTemp, &pt);
    hrgnInside = CreateRoundRectRgn(rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, pt.x, pt.y);

    hrgnRRect = CreateRectRgn(0, 0, 0, 0);
    CombineRgn(hrgnRRect, hrgnOutside, hrgnInside, RGN_DIFF);

    fRet = RectInRegion(hrgnRRect, lprc);

    DeleteObject(hrgnOutside);
    DeleteObject(hrgnInside);
    DeleteObject(hrgnRRect);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      PointInItem
//
// DESCRIPTION: This will return TRUE if the point is in the round rect.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpRoundRect::PointInItem
  (
  POINT*  pt
  )

  {
  BOOL  fRet;
  POINT ptRR;
  RECT  rcTemp;
  HRGN  hrgnOutside;
  HRGN  hrgnInside;
  HRGN  hrgnRRect;
  int   nDeflate    = m_line.LineWidth();

  FixRoundRectRadius(m_ptRadius, &m_rcItem, &ptRR);

  CopyRect(&rcTemp, &m_rcItem);
  if(nDeflate < 2*DOT_SIZE)
    {
    if(nDeflate < DOT_SIZE)
      nDeflate = DOT_SIZE;

    InflateRect(&rcTemp, 2*DOT_SIZE-nDeflate, 2*DOT_SIZE-nDeflate);
    }

  hrgnOutside = CreateRoundRectRgn(rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, ptRR.x, ptRR.y);

  CopyRect(&rcTemp, &m_rcItem);
  InflateRect(&rcTemp, -nDeflate, -nDeflate);
  FixRoundRectRadius(m_ptRadius, &rcTemp, &ptRR);
  hrgnInside = CreateRoundRectRgn(rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, ptRR.x, ptRR.y);

  hrgnRRect = CreateRectRgn(0, 0, 0, 0);
  CombineRgn(hrgnRRect, hrgnOutside, hrgnInside, RGN_DIFF);

  fRet = PtInRegion(hrgnRRect, pt->x, pt->y);

  DeleteObject(hrgnOutside);
  DeleteObject(hrgnInside);
  DeleteObject(hrgnRRect);

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpRoundRect
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpRoundRect::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  LOGBRUSH  lb = {BS_HOLLOW, 0L, 0};
  HBRUSH    hbr = CreateBrushIndirect(&lb);
  HPEN      hpen = m_line.m_CreatePen(NULL, hdc);
  HBRUSH    hbrOld;
  HPEN      hpenOld;
  int       nOldBkMode;
  COLORREF  clrOldBG;
  POINT     pt;

  if(m_line.IsBGTransparent())
    nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  else
    clrOldBG = SetBkColor(hdc, m_line.BGColor());

  hbrOld = (HBRUSH)SelectObject(hdc, hbr);
  hpenOld = (HPEN)SelectObject(hdc, hpen);

  FixRoundRectRadius(m_ptRadius, lprcPrint, &pt);
  RoundRect(hdc, lprcPrint->left, lprcPrint->top, lprcPrint->right, lprcPrint->bottom, pt.x, pt.y);

  if(m_line.IsBGTransparent())
    SetBkMode(hdc, nOldBkMode);
  else
    SetBkColor(hdc, clrOldBG);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
  DeleteObject(hbr);
  m_line.m_DeletePen(hpen);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a line based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpLine::ShpLine
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_LINE*    line
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_LINE),
      m_line(line)

  {
  OWndSetItemTransparent(this, TRUE);
  m_hpen = NULL;
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpLine::~ShpLine
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpLine::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Line", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpLine::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  rVal;

  if(m_line.EditLine(GetActiveWindow(), ShapePalette::GetCurrentUnits(), "Edit Line"))
    rVal = A_MODIFIED;
  else
    rVal = A_OK;

  return rVal;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpLine::GetExtraData
  (
  )

  {
  MEMBLOCKID  mi = AMemAllocate(sizeof(SHAPE_LINE));
  SHAPE_LINE* shape = (SHAPE_LINE*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_LINE;
  m_line.PrepForFile(&shape->line);

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a line
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::ExportExtraData
  (
  )

  {
  m_line.Export();
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a line
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Draw3D
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ShpLine::Draw3D
  (
  HDC hdc
  )

  {
  HPEN  hpenOld;
  RECT  rc        = m_rcItem;

  hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.FGColor() : m_line.BGColor()));
  MoveToEx(hdc, rc.left, rc.top, NULL);
  LineTo(hdc, rc.right, rc.bottom);

  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
  rc.left++;
  rc.top++;
  rc.right++;
  rc.bottom++;
  OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);
  DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.BGColor() : m_line.FGColor())));

  MoveToEx(hdc, rc.left, rc.top, NULL);
  LineTo(hdc, rc.right, rc.bottom);

  DeleteObject(SelectObject(hdc, hpenOld));
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  if(m_line.IsLine3D())
    Draw3D(hdc);
  else
    {
    HPEN      hpenOld = (HPEN)SelectObject(hdc, m_hpen);
    int       nOldBkMode;
    COLORREF  clrBGOld;

    if(m_line.IsBGTransparent())
      nOldBkMode = SetBkMode(hdc, TRANSPARENT);
    else
      clrBGOld = SetBkColor(hdc, m_line.BGColor());

    MoveToEx(hdc, m_rcItem.left, m_rcItem.top, NULL);
    LineTo(hdc, m_rcItem.right, m_rcItem.bottom);

    if(m_line.IsBGTransparent())
      SetBkMode(hdc, nOldBkMode);
    else
      SetBkColor(hdc, clrBGOld);

    SelectObject(hdc, hpenOld);
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      DrawTrackingRect
//
// DESCRIPTION: This method draws the tracking shape for the shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::DrawTrackingRect
  (
  HDC   hdc,
  RECT* rect
  )

  {
  MoveToEx(hdc, rect->left, rect->top, NULL);
  LineTo(hdc, rect->right, rect->bottom);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::ParentClosing
  (
  )

  {
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      PointInItem
//
// DESCRIPTION: This method will determine if the given point is
//              in the item.
//
////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpLine::PointInItem
  (
  POINT*  pt
  )

  {
  POINT   ptDot = {DOT_SIZE, DOT_SIZE};

  return PtOnLine(m_rcItem, *pt, nMax(m_line.LineWidth(), ptDot.x)/2);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      ItemInRect
//
// DESCRIPTION: This will return TRUE if the given rectangle
//              intersects the line.
//
////////////////////////////////////////////////////////////////


BOOL ALMAPI ShpLine::ItemInRect
  (
  RECT* lprc,
  BOOL  fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    int nExtra = (m_line.LineWidth()+1)/2;
    fRet = ((lprc->left   <= (nMin(m_rcItem.left, m_rcItem.right)  + nExtra))
                   &&
            (lprc->top    <= (nMin(m_rcItem.top,  m_rcItem.bottom) + nExtra))
                   &&
            (lprc->right  >= (nMax(m_rcItem.left, m_rcItem.right)  + nExtra))
                   &&
            (lprc->bottom >= (nMax(m_rcItem.top,  m_rcItem.bottom) + nExtra)));
    }
  else
    {
    POINT   ptDot = {DOT_SIZE, DOT_SIZE};

    fRet = RectAndLineIntersect(*lprc, m_rcItem, nMax(ptDot.x, m_line.LineWidth())/2);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: Draws the grab handles for the line.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rc;
  POINT pt = {DOT_SIZE, DOT_SIZE};

  CreateCenteredGrabHandle(&rc, (LPPOINT)&m_rcItem.left, &pt);
  Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
  CreateCenteredGrabHandle(&rc, (LPPOINT)&m_rcItem.right, &pt);
  Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: Invalidates the grab handles to erase them.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rc;
  POINT pt = {DOT_SIZE, DOT_SIZE};

  CreateCenteredGrabHandle(&rc, (LPPOINT)&m_rcItem.left, &pt);
  OWndInvalidateLogicalRect(m_oiParent, &rc, fErase);
  CreateCenteredGrabHandle(&rc, (LPPOINT)&m_rcItem.right, &pt);
  OWndInvalidateLogicalRect(m_oiParent, &rc, fErase);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      PointInGrabHandle
//
// DESCRIPTION: Returns the grab handle the cursor is in (and
//              optionally sets the cursor).
//
////////////////////////////////////////////////////////////////

long ALMAPI  ShpLine::PointInGrabHandle
  (
  POINT*    pt,
  HCURSOR*  phCursor
  )

  {
  long    lGrabHandle;
  RECT    r;
  POINT   ptSize = {DOT_SIZE, DOT_SIZE};

  CreateCenteredGrabHandle(&r, (LPPOINT)&m_rcItem.left, &ptSize);

  if (PtInRect(&r, *pt))
    lGrabHandle = OWND_POINT_IN_TL;
  else
    {
    CreateCenteredGrabHandle(&r, (LPPOINT)&m_rcItem.right, &ptSize);

    if (PtInRect(&r, *pt))
      lGrabHandle = OWND_POINT_IN_BR;
    else
      lGrabHandle = OWND_POINT_IN_NONE;
    }

  if(lGrabHandle!=OWND_POINT_IN_NONE)
    *phCursor = LoadCursor(GetHInstance(), ID(CUR_LINEMOVE));
  else
    *phCursor = NULL;

  return lGrabHandle;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      AdjustNewRect
//
// DESCRIPTION: This method will adjust a rectangle if it will
//              not be proper for the line.
//
////////////////////////////////////////////////////////////////

long ALMAPI  ShpLine::AdjustNewRect
  (
  RECT* newRect,
  LONG  adjustFlag
  )

  {
  long    lAdjNewRect = 0;
  int     x, y;
  double  dLen, dMin;
  POINT   ptSize = {DOT_SIZE, DOT_SIZE};

  if(adjustFlag & OWND_ADJRECT_DEFAULT_W)
    lAdjNewRect |= OWND_ADJRECT_NODEFAULT_W;

  if(adjustFlag & OWND_ADJRECT_DEFAULT_H)
    lAdjNewRect |= OWND_ADJRECT_NODEFAULT_H;

  // force a minimum length....!
  // currently the minimum length is the size of the diagonal of a grab handle.
  // this may need to change.....
  dMin = sqrt((long)ptSize.x*(long)ptSize.x + (long)ptSize.y*(long)ptSize.y);

  x = newRect->right - newRect->left;
  y = newRect->bottom - newRect->top;
  dLen = sqrt((long)x*(long)x + (long)y*(long)y);

  if(dLen < dMin)
    {
    // increase the length, but preserve the line angle
    int xNew, yNew;

    // avoid division by zero
    if(dLen==0.)
      {
      xNew = (int)((dMin / sqrt(2)) + 0.5);
      yNew = (int)((dMin / sqrt(2)) + 0.5);
      }
    else
      {
      xNew = (int)((x * dMin / dLen) + ((x>0) ? 0.5 : -0.5));
      yNew = (int)((y * dMin / dLen) + ((y>0) ? 0.5 : -0.5));
      }

    // adjust the rectangle based on the flags
    if(adjustFlag & OWND_ADJRECT_ANCHOR_T)
      { // adjust the bottom ....
      newRect->bottom = newRect->top + yNew;
      lAdjNewRect |= OWND_ADJRECT_CHANGED_B;
      }
    else
      { // adjust the top ....
      newRect->top = newRect->bottom - yNew;
      lAdjNewRect |= OWND_ADJRECT_CHANGED_T;
      }

    if(adjustFlag & OWND_ADJRECT_ANCHOR_L)
      { // adjust the right ....
      newRect->right = newRect->left + xNew;
      lAdjNewRect |= OWND_ADJRECT_CHANGED_R;
      }
    else
      { // adjust the left ....
      newRect->left = newRect->right - xNew;
      lAdjNewRect |= OWND_ADJRECT_CHANGED_L;
      }
    }

  return lAdjNewRect;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Move
//
// DESCRIPTION: This will move the line and cause it to redraw.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::Move
  (
  RECT* newRect
  )

  {
  RECT  rc;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};

  MakeRealRect(&rc, &m_rcItem);
  InflateRect(&rc, (m_line.LineWidth() + ptSize.x + 1)/2, (m_line.LineWidth() + ptSize.y + 1)/2);
  OWndInvalidateLogicalRect(m_oiParent, &rc, TRUE); //Invalidate the old
  CopyRect(&m_rcItem, newRect);                     //Copy the new

  MakeRealRect(&rc, &m_rcItem);
  InflateRect(&rc, (m_line.LineWidth() + ptSize.x + 1)/2, (m_line.LineWidth() + ptSize.y + 1)/2);
  OWndInvalidateLogicalRect(m_oiParent, &rc, TRUE); //Invalidate the new
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      ItemInRegion
//
// DESCRIPTION: This will return TRUE if the line is in the
//              specified region.
//
////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpLine::ItemInRegion
  (
  HRGN  hRegion,
  BOOL  bDeviceUnits
  )

  {
  RECT  rc;
  POINT pt1 = {2, 2};
  POINT pt2 = {(m_line.LineWidth()+1)/2, (m_line.LineWidth()+1)/2};

  MakeRealRect(&rc, &m_rcItem);

  if (bDeviceUnits)
    {
    OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
    OWndLStoDS(m_oiParent, &pt2, 1);
    }
  else
    OWndDStoLS(m_oiParent, &pt1, 1);

  InflateRect(&rc, nMax(pt1.x, pt2.x), nMax(pt1.y, pt2.y));

  return RectInRegion(hRegion, &rc);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      RemoveItemFromRegion
//
// DESCRIPTION: This will remove the line from the region.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::RemoveItemFromRegion
  (
  HRGN  hRegion,
  BOOL  bDeviceUnits
  )

  {
  I_UNUSED(hRegion);
  I_UNUSED(bDeviceUnits);

  // the line will never be removed from the region.
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      InvalidateItem
//
// DESCRIPTION: This will cause the line to be redrawn.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::InvalidateItem
  (
  BOOL  fErase
  )

  {
  RECT  rc;
  POINT pt    = {2, 2};

  MakeRealRect(&rc, &m_rcItem);
  OWndDStoLS(m_oiParent, &pt, 1);
  if((rc.right-rc.left) < pt.x)
    rc.right+=pt.x;
  if((rc.bottom-rc.top) < pt.y)
    rc.bottom+=pt.y;

  if(m_hpen)
    { // not 3D -- get pen width from the pen handle
    LOGPEN  lp;

    GetObject(m_hpen, sizeof(LOGPEN), &lp);
    InflateRect(&rc, (lp.lopnWidth.x + 1)/2, (lp.lopnWidth.x + 1)/2);
    }
  else  // a 3D pen
    InflateRect(&rc, pt.x, pt.y);

  OWndInvalidateLogicalRect(m_oiParent, &rc, fErase);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpLine
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpLine::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  HPEN      hpen        = m_line.m_CreatePen(NULL, hdc);
  HPEN      hpenOld     = (HPEN)SelectObject(hdc, hpen);
  int       nOldBkMode;
  COLORREF  clrOldBG;

  if(m_line.IsBGTransparent())
    nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  else
    clrOldBG = SetBkColor(hdc, m_line.BGColor());

  MoveToEx(hdc, lprcPrint->left, lprcPrint->top, NULL);
  LineTo(hdc, lprcPrint->right, lprcPrint->bottom);

  if(m_line.IsBGTransparent())
    SetBkMode(hdc, nOldBkMode);
  else
    SetBkColor(hdc, clrOldBG);

  SelectObject(hdc, hpenOld);
  m_line.m_DeletePen(hpen);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      Constructor
//
// DESCRIPTION: The constructor will initialize text based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpText::ShpText
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_FONT*    font,
  WORD          nAlign,
  short         nTextLen,
  char*         pText,
  BOOL          fEditText
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_TEXT),
      m_font(font),
      m_nAlign(nAlign),
      m_nTextLen(nTextLen),
      m_hfont(NULL)

  {
  m_miText = AMemAllocate(nTextLen);
  m_pText = (char*)AMemLock(m_miText);
  if(m_pText)
    {
    char  vKey;
    char  modifier;

    lstrcpy(m_pText, pText);
    if(AccelKeyFromString(m_pText, &vKey, &modifier))
      OWndSetItemAccelKey(this, vKey, modifier);
    }

  OWndSetItemTransparent(this, TRUE);

  if(fEditText)
    {
    SetConstructor(TRUE);
    Edit(TRUE);
    SetConstructor(FALSE);
    }
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpText::~ShpText
  (
  )

  {
  if(m_pText)
    AMemUnlock(m_miText);

  AMemFree(m_miText);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      TextEntryDlgProc
//
// DESCRIPTION: Dialog procedure for entering text.
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EXPORT ShpText::TextEntryDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL        fHandled;
  HWND        hEdit;
  ShpText*    pShape;
  FontAttrib* pFont;

  switch (uMsg)
    {
    case WM_INITDIALOG:
      {
      AUtlCenterDialog(hwnd, 0);

      ATTR_FONT font;

      SetWindowLong(hwnd, DWL_USER, lParam);

      pShape = (ShpText*)lParam;
      hEdit = GetDlgItem(hwnd, ED_TEXT);
      SetWindowText(hEdit, pShape->m_pText);
      pShape->m_font.PrepForFile(&font);
      pFont = new FontAttrib(&font);
      SetProp(hwnd, "loFont", (HANDLE)LOWORD((long)pFont));
      SetProp(hwnd, "hiFont", (HANDLE)HIWORD((long)pFont));

      Edit_SetSel(hEdit, 0, -1);
      SetFocus(hEdit);

      fHandled = FALSE;
      }
      break;

    case WM_COMMAND:
      HANDLE_WM_COMMAND(hwnd, wParam, lParam, ShpText::TextEntryCommand);
      fHandled = TRUE;
      break;

    default:
      fHandled = FALSE;
      break;
    }

  return fHandled;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      TextEntryCommand
//
// DESCRIPTION: This will handle the WM_COMMAND message in the dialog.
//
////////////////////////////////////////////////////////////////////////////

void ShpText::TextEntryCommand
  (
  HWND  hwnd,
  int   idCtl,
  HWND  hwndCtl,
  UINT  uNtfyCode
  )

  {
  I_UNUSED(hwndCtl);
  I_UNUSED(uNtfyCode);

  ShpText*    pShape;
  FontAttrib* pFont;
  HWND        hEdit;

  switch (idCtl)
    {
    case IDOK:
      {
      BOOL  fChanged;
      char* szText;
      int   nLen;

      pShape = (ShpText*)GetWindowLong(hwnd, DWL_USER);
      pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));

      hEdit  = GetDlgItem(hwnd, ED_TEXT);
      nLen = GetWindowTextLength(hEdit) + 1;
      szText = new char[nLen];
      GetWindowText(hEdit, szText, nLen);

      // check to see if there was a change
      fChanged = lstrcmp(szText, pShape->m_pText) || !(*pFont == pShape->m_font);

      // copy the new data if there was a change
      if(fChanged)
        {
        pShape->m_nTextLen = (short)nLen;
        AMemUnlock(pShape->m_miText);
        AMemSetSize(pShape->m_miText, pShape->m_nTextLen);
        pShape->m_pText = (char*)AMemLock(pShape->m_miText);
        lstrcpy(pShape->m_pText, szText);
        pShape->m_font = *pFont;
        }

      // free the memory and end the dialog
      delete [] szText;
      delete pFont;
      RemoveProp(hwnd, "loFont");
      RemoveProp(hwnd, "hiFont");
      EndDialog(hwnd, fChanged);
      }
      break;

    case IDCANCEL:
      pShape = (ShpText*)GetWindowLong(hwnd, DWL_USER);
      if(pShape->InConstructor())
        pShape->SetCancelCreate(TRUE);
      pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));
      delete pFont;
      RemoveProp(hwnd, "loFont");
      RemoveProp(hwnd, "hiFont");
      EndDialog(hwnd, FALSE);
      break;

    case PB_FONT:
      {
      char  szText[50];

      pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));
      hEdit  = GetDlgItem(hwnd, ED_TEXT);
      GetWindowText(hEdit, szText, 50);
      pFont->EditFont(hwnd, "Edit Font", szText, FALSE, TRUE, FALSE);
      break;
      }

    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      SizeToShape
//
// DESCRIPTION: This will cause the original bounding rectangle of the text
//              item to exactly enclose the text.
//
////////////////////////////////////////////////////////////////////////////

void ShpText::SizeToShape
  (
  )

  {
  HDC   hdc = GetDC(m_hwndParent);
  HFONT hfontOld = (HFONT) SelectObject(hdc, m_hfont);
  RECT  rc;

  // Now calculate bounding box for convenience for the user
  CopyRect(&rc, &m_rcItem);
  DrawText(hdc, m_pText, -1, &rc, m_nAlign | DT_SINGLELINE | DT_CALCRECT);

  // make sure the rectangle is big enough ... add one pixel in each direction
  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 2);
  rc.right++;
  rc.bottom++;
  OWndDPtoLP(m_oiParent, (LPPOINT)&rc, 2);

  // set the item rectangle the proper way!
  AdjustNewRect(&rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
  Move(&rc);

  SelectObject(hdc, hfontOld);
  ReleaseDC(m_hwndParent, hdc);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpText::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Static Text", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpText::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpText::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG  lRet = A_OK;

  if(DialogBoxParam(GetHInstance(), ID(DLG_TEXTENTRY), GetActiveWindow(), TextEntryDlgProc, (LPARAM)this))
    lRet = A_MODIFIED;

  return lRet;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpText::GetExtraData
  (
  )

  {
  MEMBLOCKID  mi = AMemAllocate(sizeof(SHAPE_TEXT)+m_nTextLen);
  SHAPE_TEXT* shape = (SHAPE_TEXT*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_TEXT;
  m_font.PrepForFile(&shape->font);
  shape->nTextLen   = m_nTextLen;
  shape->nAlign     = m_nAlign;
  lstrcpy(shape->szText, m_pText);

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports static text
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpText::ExportExtraData
  (
  )

  {
  short nAlign = (short)(m_nAlign & (DT_LEFT | DT_CENTER | DT_RIGHT));

  m_font.Export();

  ExportText ("STRING",   0L, m_pText, m_nTextLen - 1);
  ExportShort("ALIGN",    0L, (short)((nAlign==DT_LEFT) ? 0 : ((nAlign==DT_CENTER) ? 1 : 2)));
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports static text
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpText::ImportExtraData
  (
  )

  {
  short sVer = 0;
  short sVal = DT_LEFT;

  ImportShort("VERSION", 0L, &sVer);

  m_font.Import(sVer);

  ImportShort("ALIGN", 0L, &sVal);
  m_nAlign &= ~(DT_LEFT | DT_CENTER | DT_RIGHT);
  m_nAlign |= (WORD)((sVal==0) ? DT_LEFT : ((sVal==1) ? DT_CENTER : DT_RIGHT));

  if(m_pText)
    AMemUnlock(m_miText);

  if(sVer<0x0100)
    {
    short nPad = 0;
    short nLen = 1;
    char* pText;

    ImportText ("NAME", 0L, m_miText);

    // replace all '&' characters with '&&'.
    m_pText = pText = (char*)AMemLock(m_miText);
    while(*pText)
      {
      nLen++;
      if(*pText == '&')
        nPad++;
      pText++;
      }

    if(nPad > 0)
      {
      MEMBLOCKID  mi      = AMemAllocate(nLen);
      char*       p       = (char*)AMemLock(mi);
      short       nOffset = 0;

      lstrcpy(p, m_pText);
      AMemUnlock(m_miText);
      AMemSetSize(m_miText, nLen + nPad);
      m_pText = (char*)AMemLock(m_miText);

      while(*p)
        {
        I_ASSERT(nOffset < (nLen + nPad - 1));
        *(m_pText + nOffset++) = *p;
        if(*p == '&')
          {
          I_ASSERT(nOffset < (nLen + nPad - 1));
          *(m_pText + nOffset++) = *p;
          }
        p++;
        }
      I_ASSERT(nOffset < (nLen + nPad));
      *(m_pText + nOffset) = *p;

      AMemUnlock(mi);
      AMemFree(mi);
      AMemUnlock(m_miText);
      }
    }
  else
    ImportText ("STRING", 0L, m_miText);

  m_pText    = (char*)AMemLock(m_miText);
  m_nTextLen = (short)AMemGetSize(m_miText);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpText::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  HFONT     hfontOld = (HFONT)SelectObject(hdc, m_hfont);
  int       nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  COLORREF  clrOldFG = SetTextColor(hdc, m_font.FGColor());

  DrawText(hdc, m_pText, -1, &m_rcItem, m_nAlign);

  SelectObject(hdc, hfontOld);
  SetBkMode(hdc, nOldBkMode);
  SetTextColor(hdc, clrOldFG);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpText::ParentOpened
  (
  )

  {
  m_hfont = m_font.m_CreateFont(m_oiParent);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpText::ParentClosing
  (
  )

  {
  m_font.m_DeleteFont(m_hfont);
  m_hfont = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpText::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  HFONT     hfont       = m_font.m_CreateFont(NULL, hdc);
  HFONT     hfontOld = (HFONT)SelectObject(hdc, hfont);
  int       nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  COLORREF  clrOldFG = SetTextColor(hdc, m_font.FGColor());

  DrawText(hdc, m_pText, -1, lprcPrint, m_nAlign);

  SelectObject(hdc, hfontOld);
  SetBkMode(hdc, nOldBkMode);
  SetTextColor(hdc, clrOldFG);
  m_font.m_DeleteFont(hfont);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpText
//
// METHOD:      AccelKeyHit
//
// DESCRIPTION: This will be called when the text's accelerator key is hit.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpText::AccelKeyHit
  (
  )

  {
  OWndSetActiveItem(this);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a group box based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpGroupBox::ShpGroupBox
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  ATTR_FONT*    font,
  ATTR_LINE*    line,
  short         nTextLen,
  char*         pText
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_GROUPBOX),
      m_line(line),
      m_font(font),
      m_nTextLen(nTextLen),
      m_hfont(NULL),
      m_hpen(NULL)

  {
  m_miText = AMemAllocate(nTextLen);
  m_pText = (char*)AMemLock(m_miText);
  if(m_pText)
    {
    char  vKey;
    char  modifier;

    lstrcpy(m_pText, pText);
    if(AccelKeyFromString(m_pText, &vKey, &modifier))
      OWndSetItemAccelKey(this, vKey, modifier);
    }

  OWndSetItemTransparent(this, TRUE);
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpGroupBox::~ShpGroupBox
  (
  )

  {
  if(m_pText)
    AMemUnlock(m_miText);

  AMemFree(m_miText);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      InitDialog
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

void ShpGroupBox::InitDialog
  (
  HWND  hwnd
  )

  {
  ATTR_LINE   line;
  ATTR_FONT   font;
  FontAttrib* pFont;

  // set up the line portion of the dialog
  m_line.PrepForFile(&line);
  ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_LINEFGCOLOR), line.clrFG, TRUE, FALSE);
  ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_LINEBGCOLOR), line.clrBG, FALSE, TRUE);
  LineAttrib::FillLineStyleComboBox(GetDlgItem(hwnd, CB_LINESTYLE), line.nStyle);
  LineAttrib::InitLineWidth(hwnd, line.nWidth);

  if((line.nStyle==PS_3DIN) || (line.nStyle==PS_3DOUT))
    {
    SetDlgItemText(hwnd, ST_LINEFGCOLOR, "&Shadow:");
    SetDlgItemText(hwnd, ST_LINEBGCOLOR, "&Highlight:");
    }
  else
    {
    SetDlgItemText(hwnd, ST_LINEFGCOLOR, "&Foreground:");
    SetDlgItemText(hwnd, ST_LINEBGCOLOR, "&Background:");
    }
  EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), line.nWidth==0);
  EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), line.nWidth==0);

  // set up the text portion of the dialog
  SetDlgItemText(hwnd, ED_TEXT, m_pText);
  m_font.PrepForFile(&font);
  pFont = new FontAttrib(&font);
  SetProp(hwnd, "loFont", (HANDLE)LOWORD((long)pFont));
  SetProp(hwnd, "hiFont", (HANDLE)HIWORD((long)pFont));
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      SaveChanges
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

BOOL  ShpGroupBox::SaveChanges
  (
  HWND  hwnd
  )

  {
  FontAttrib*   pFont     = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));
  ShpGroupBox*  pGroup    = ((GROUPBOX_EDIT*)GetWindowLong(hwnd, DWL_USER))->pGroup;
  short         nLen      = (short)(GetWindowTextLength(GetDlgItem(hwnd, ED_TEXT)) + 1);
  char*         szText    = new char[nLen];
  BOOL          fChanged;
  DWORD         dwIndex;

  // check to see if there was a change in the text or font
  GetDlgItemText(hwnd, ED_TEXT, szText, nLen);
  fChanged = lstrcmp(szText, pGroup->m_pText) || !(*pFont == pGroup->m_font);

  // copy the new data if there was a change in the text stuff
  if(fChanged)
    {
    pGroup->m_nTextLen = (short)nLen;
    AMemUnlock(pGroup->m_miText);
    AMemSetSize(pGroup->m_miText, pGroup->m_nTextLen);
    pGroup->m_pText = (char*)AMemLock(pGroup->m_miText);
    lstrcpy(pGroup->m_pText, szText);
    pGroup->m_font = *pFont;
    }
  delete [] szText;

  // update the line style and see if anything changed
  dwIndex = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEFGCOLOR));
  if(dwIndex!=(DWORD)CB_ERR)
    fChanged |= pGroup->m_line.SetFGColor((COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEFGCOLOR), dwIndex));
  dwIndex = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEBGCOLOR));
  if(dwIndex!=(DWORD)CB_ERR)
    fChanged |= pGroup->m_line.SetBGColor((COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEBGCOLOR), dwIndex));
  dwIndex = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
  if(dwIndex!=(DWORD)CB_ERR)
    fChanged |= pGroup->m_line.SetLineStyle((short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), dwIndex));
  fChanged |= pGroup->m_line.SetLineWidth(LineAttrib::GetLineWidthFromDlg(hwnd));

  return fChanged;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      DrawSample
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

void ShpGroupBox::DrawSample
  (
  HWND              hwnd,
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  FontAttrib* pFont   = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));
  short       nLen    = (short)(GetWindowTextLength(GetDlgItem(hwnd, ED_TEXT)) + 1);
  char*       szText  = new char[nLen];
  HFONT       hfontOld;
  TEXTMETRIC  tm;
  RECT        rcText;
  RECT        rcTextExt = {0, 0, 0, 0};
  RECT        rcFrame;
  int         nLT, nRT;
  LRESULT     lRes;
  short       nLineStyle, nLineWidth, tmmLineWidth;
  int         n1_2Width;
  int         nBkModeOld;
  COLORREF    clrLineFG, clrLineBG;
  COLORREF    clrOld;
  HPEN        hpenOld;

  // get the current title
  GetDlgItemText(hwnd, ED_TEXT, szText, nLen);

  // fill the background with light gray to match Borland dialog
  FillRect(lpdi->hDC, &lpdi->rcItem, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

  // get the current line information
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEFGCOLOR));
  if(lRes!=CB_ERR)
    clrLineFG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEFGCOLOR), lRes);
  if(clrLineFG == COLOR_WINDOWFOREGROUND)
    clrLineFG = GetSysColor(COLOR_WINDOWTEXT);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINEBGCOLOR));
  if(lRes!=CB_ERR)
    clrLineBG = (COLORREF)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINEBGCOLOR), lRes);
  if(clrLineBG == COLOR_WINDOWBACKGROUND)
    clrLineBG = GetSysColor(COLOR_WINDOW);
  lRes = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
  if(lRes!=CB_ERR)
    nLineStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);
  tmmLineWidth = LineAttrib::GetLineWidthFromDlg(hwnd);
  nLineWidth = (short)MulDiv(tmmLineWidth, GetDeviceCaps(lpdi->hDC, LOGPIXELSX), 254);
  if(tmmLineWidth>0)
    nLineStyle = PS_SOLID;
  n1_2Width = (nLineWidth + 1)/2;

  // get the specific rectangles for drawing efficiently
  hfontOld = (HFONT)SelectObject(lpdi->hDC, pFont->m_CreateFont());
  GetTextMetrics(lpdi->hDC, &tm);
  DrawText(lpdi->hDC, szText, nLen-1, &rcTextExt, DT_SINGLELINE | DT_CALCRECT);

  rcText.left   = lpdi->rcItem.left + nLineWidth + 2 * tm.tmAveCharWidth;
  rcText.top    = lpdi->rcItem.top  + ((rcTextExt.bottom < nLineWidth) ? (nLineWidth - rcTextExt.bottom)/2 : 0);
  rcText.right  = rcText.left + rcTextExt.right + tm.tmAveCharWidth/2;
  rcText.bottom = rcText.top  + rcTextExt.bottom;

  if(rcText.right > (lpdi->rcItem.right - nLineWidth - 3*tm.tmAveCharWidth/2))
    rcText.right = lpdi->rcItem.right - nLineWidth - 3*tm.tmAveCharWidth/2;

  CopyRect(&rcFrame, &lpdi->rcItem);
  rcFrame.left   += n1_2Width;
  rcFrame.top    += (rcTextExt.bottom < nLineWidth) ? n1_2Width : rcTextExt.bottom/2;
  rcFrame.right  -= (n1_2Width + 1);
  rcFrame.bottom -= (n1_2Width + 1);

  nLT = rcFrame.left + tm.tmAveCharWidth;
  nRT = rcText.right + tm.tmAveCharWidth/2 + n1_2Width;

  nBkModeOld = SetBkMode(lpdi->hDC, TRANSPARENT);
  clrOld = SetTextColor(lpdi->hDC, pFont->FGColor());
  DrawText(lpdi->hDC, szText, nLen-1, &rcText, DT_SINGLELINE);
  SetTextColor(lpdi->hDC, clrOld);
  pFont->m_DeleteFont((HFONT)SelectObject(lpdi->hDC, hfontOld));

  if((nLineWidth==0) && ((nLineStyle==PS_3DIN) || (nLineStyle==PS_3DOUT)))
    { // draw a 3D group box
    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nLineStyle==PS_3DIN) ? clrLineBG : clrLineFG));

    MoveToEx(lpdi->hDC, nLT + 1,        rcFrame.top + 1, NULL);
    LineTo(lpdi->hDC, rcFrame.left + 1, rcFrame.top + 1);
    LineTo(lpdi->hDC, rcFrame.left + 1, rcFrame.bottom);
    LineTo(lpdi->hDC, rcFrame.right,    rcFrame.bottom);
    LineTo(lpdi->hDC, rcFrame.right,    rcFrame.top + 1);
    LineTo(lpdi->hDC, nRT + 1,          rcFrame.top + 1);
    DeleteObject(SelectObject(lpdi->hDC, CreatePen(PS_SOLID, 0, (nLineStyle==PS_3DIN) ? clrLineFG : clrLineBG)));

    MoveToEx(lpdi->hDC, nLT,             rcFrame.top, NULL);
    LineTo(lpdi->hDC, rcFrame.left,      rcFrame.top);
    LineTo(lpdi->hDC, rcFrame.left,      rcFrame.bottom - 1);
    LineTo(lpdi->hDC, rcFrame.right - 1, rcFrame.bottom - 1);
    LineTo(lpdi->hDC, rcFrame.right - 1, rcFrame.top);
    LineTo(lpdi->hDC, nRT,               rcFrame.top);
    DeleteObject(SelectObject(lpdi->hDC, hpenOld));
    }
  else
    { // draw a standard group box
    COLORREF  clrBGOld;

    if(clrLineBG!=COLOR_TRANSPARENT)
      {
      SetBkMode(lpdi->hDC, OPAQUE);
      clrBGOld = SetBkColor(lpdi->hDC, clrLineBG);
      }

    hpenOld = (HPEN)SelectObject(lpdi->hDC, CreatePen(nLineStyle, nLineWidth, clrLineFG));

    MoveToEx(lpdi->hDC, nLT,         rcFrame.top, NULL);
    LineTo(lpdi->hDC, rcFrame.left,  rcFrame.top);
    LineTo(lpdi->hDC, rcFrame.left,  rcFrame.bottom);
    LineTo(lpdi->hDC, rcFrame.right, rcFrame.bottom);
    LineTo(lpdi->hDC, rcFrame.right, rcFrame.top);
    LineTo(lpdi->hDC, nRT,           rcFrame.top);

    DeleteObject(SelectObject(lpdi->hDC, hpenOld));

    if(clrLineBG!=COLOR_TRANSPARENT)
      SetBkColor(lpdi->hDC, clrBGOld);
    }

  SetBkMode(lpdi->hDC, nBkModeOld);

  delete [] szText;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      EditDlgProc
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EXPORT ShpGroupBox::EditDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL  fRet = FALSE;
  short nWidth;

  switch(uMsg)
    {
    case WM_INITDIALOG:
      AUtlCenterDialog(hwnd, 0);

      SetWindowLong(hwnd, DWL_USER, lParam);
      ((GROUPBOX_EDIT*)lParam)->pGroup->InitDialog(hwnd);
      Edit_SetSel(GetDlgItem(hwnd, ED_TEXT), 0, -1);
      SetFocus(GetDlgItem(hwnd, ED_TEXT));
      fRet = FALSE;
      break;

    case WM_DRAWITEM:
      {
      LPDRAWITEMSTRUCT  lpdi = (LPDRAWITEMSTRUCT)lParam;

      switch(lpdi->CtlID)
        {
        case CB_LINEFGCOLOR:
        case CB_LINEBGCOLOR:
          ColorAttrib::DrawColorComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case CB_LINESTYLE:
          LineAttrib::DrawLineStyleComboBox(hwnd, lpdi);
          fRet = TRUE;
          break;

        case PB_SAMPLE:
          DrawSample(hwnd, lpdi);
          break;

        default:
          break;
        }
      }
      break;

    case WM_MEASUREITEM:
      ColorAttrib::MeasureComboBox(hwnd, (LPMEASUREITEMSTRUCT)lParam);
      fRet = TRUE;
      break;

    case WM_COMMAND:
      HANDLE_WM_COMMAND(hwnd, wParam, lParam, Command);
      fRet = TRUE;
      break;

    case WM_VSCROLL:
      HANDLE_WM_VSCROLL_EX(hwnd, wParam, lParam, PB_SAMPLE, LineAttrib::WidthVScrollMsg);
      nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);
      EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), nWidth==0);
      EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), nWidth==0);
      fRet = TRUE;
      break;

    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Command
//
// DESCRIPTION: This method handles the WM_COMMAND message.
//
////////////////////////////////////////////////////////////////

void ShpGroupBox::Command
  (
  HWND  hwnd,
  int   idCtl,
  HWND  hwndCtl,
  UINT  uNtfyCode
  )

  {
  I_UNUSED(hwndCtl);

  FontAttrib* pFont;
  short       nWidth;

  switch(idCtl)
    {
    case IDOK:
      pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));
      EndDialog(hwnd, SaveChanges(hwnd));
      delete pFont;
      RemoveProp(hwnd, "loFont");
      RemoveProp(hwnd, "hiFont");
      break;

    case IDCANCEL:
      pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));
      delete pFont;
      RemoveProp(hwnd, "loFont");
      RemoveProp(hwnd, "hiFont");
      EndDialog(hwnd, FALSE);
      break;

    case IDHELP:
      break;

    case ED_TEXT:
      if(uNtfyCode == EN_CHANGE)
        InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
      break;

    case PB_FONT:
      {
      char  szText[50];

      pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hwnd, "loFont"), (WORD)GetProp(hwnd, "hiFont"));
      GetDlgItemText(hwnd, ED_TEXT, szText, 50);
      pFont->EditFont(hwnd, "Edit Group Box Font", szText, FALSE, TRUE, FALSE);
      InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
      break;
      }

    case CB_LINESTYLE:
      if(uNtfyCode == CBN_SELCHANGE)
        {
        LRESULT lRes   = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_LINESTYLE));
        short   nStyle = (short)ComboBox_GetItemData(GetDlgItem(hwnd, CB_LINESTYLE), lRes);

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

        InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
        }
      break;

    case CB_LINEFGCOLOR:
    case CB_LINEBGCOLOR:
      if(uNtfyCode == CBN_SELCHANGE)
        InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
      break;

    case RB_LINEWIDTH:
      LineAttrib::SetEditFocus(hwnd, ED_LINEWIDTH, FALSE);
      // fall through
    case RB_HAIRLINE:
      nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);
      EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
      EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);
      EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), nWidth==0);
      EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), nWidth==0);
      InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
      break;

    case ED_LINEWIDTH:
      switch(uNtfyCode)
        {
        case EN_KILLFOCUS:
          LineAttrib::SetItemValue(hwnd, idCtl, LineAttrib::GetItemValue(hwnd, idCtl));
          InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
          break;

        case EN_SETFOCUS:
          nWidth = LineAttrib::GetItemValue(hwnd, ED_LINEWIDTH);
          EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, CB_LINEBGCOLOR), nWidth==0);
          EnableWindow(GetDlgItem(hwnd, ST_LINEBGCOLOR), nWidth==0);

          CheckDlgButton(hwnd, RB_LINEWIDTH, TRUE);
          CheckDlgButton(hwnd, RB_HAIRLINE, FALSE);
          InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
          break;

        default:
          break;
        }
      break;

    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      InitDrawValues
//
// DESCRIPTION: This method initializes the values that are used
//              in drawing the shape.
//
////////////////////////////////////////////////////////////////

void ShpGroupBox::InitDrawValues
  (
  )

  {
  HDC         hdc;
  TEXTMETRIC  tm;
  HFONT       hfontOld;
  RECT        rcTextExt = {0, 0, 0, 0};
  int         n1_2Width = (m_line.LineWidth()+1)/2;
  // get the specific rectangles for drawing efficiently
  hdc = GetDC(m_hwndParent);
  hfontOld = (HFONT)SelectObject(hdc, m_hfont);
  GetTextMetrics(hdc, &tm);
  DrawText(hdc, m_pText, m_nTextLen-1, &rcTextExt, DT_SINGLELINE | DT_CALCRECT);
  SelectObject(hdc, hfontOld);
  ReleaseDC(m_hwndParent, hdc);

  m_rcText.left   = m_rcItem.left + m_line.LineWidth() + 2*tm.tmAveCharWidth;
  m_rcText.top    = m_rcItem.top  + ((rcTextExt.bottom < m_line.LineWidth()) ? (m_line.LineWidth() - rcTextExt.bottom)/2 : 0);
  m_rcText.right  = m_rcText.left + rcTextExt.right + tm.tmAveCharWidth/2;
  m_rcText.bottom = m_rcText.top  + rcTextExt.bottom;

  if(m_rcText.right > (m_rcItem.right - m_line.LineWidth() - 3*tm.tmAveCharWidth/2))
    m_rcText.right = m_rcItem.right - m_line.LineWidth() - 3*tm.tmAveCharWidth/2;

  CopyRect(&m_rcFrame, &m_rcItem);
  OWndLPtoDP(m_oiParent, (LPPOINT)&m_rcFrame, 2);
  if(m_line.IsLine3D())
    {
    m_rcFrame.right  -= 2;
    m_rcFrame.bottom -= 2;
    }
  else
    {
    m_rcFrame.right--;
    m_rcFrame.bottom--;
    }
  OWndDPtoLP(m_oiParent, (LPPOINT)&m_rcFrame, 2);
  m_rcFrame.left   += n1_2Width;
  m_rcFrame.top    += (rcTextExt.bottom < m_line.LineWidth()) ? n1_2Width : rcTextExt.bottom/2;
  m_rcFrame.right  -= n1_2Width;
  m_rcFrame.bottom -= n1_2Width;

  // make sure the frame is on even pixel boundaries
  OWndLPtoDP(m_oiParent, (LPPOINT)&m_rcFrame, 2);
  OWndDPtoLP(m_oiParent, (LPPOINT)&m_rcFrame, 2);

  m_nLT = m_rcFrame.left + tm.tmAveCharWidth;
  m_nRT = m_rcText.right + tm.tmAveCharWidth/2 + n1_2Width;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpGroupBox::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Group Box", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpGroupBox::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  LONG          lRet = A_OK;
  GROUPBOX_EDIT ge = {this, ShapePalette::GetCurrentUnits()};

  if(DialogBoxParam(GetHInstance(), ID(DLG_GROUPBOX), GetActiveWindow(), EditDlgProc, (LPARAM)&ge))
    lRet = A_MODIFIED;

  return lRet;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpGroupBox::GetExtraData
  (
  )

  {
  MEMBLOCKID      mi = AMemAllocate(sizeof(SHAPE_GROUPBOX)+m_nTextLen);
  SHAPE_GROUPBOX* shape = (SHAPE_GROUPBOX*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_GROUPBOX;
  m_font.PrepForFile(&shape->font);
  m_line.PrepForFile(&shape->line);
  shape->nTextLen   = m_nTextLen;
  lstrcpy(shape->szText, m_pText);

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a group box
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::ExportExtraData
  (
  )

  {
  m_line.Export();
  m_font.Export();

  ExportText ("STRING",   0L, m_pText, m_nTextLen - 1);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a group box
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::ImportExtraData
  (
  )

  {
  short sVer = 0;

  ImportShort("VERSION", 0L, &sVer);

  m_line.Import(sVer);
  m_font.Import(sVer);

  if(m_pText)
    AMemUnlock(m_miText);

  if(sVer==0)
    ImportText ("NAME",     0L, m_miText);
  else
    ImportText ("STRING",   0L, m_miText);

  m_pText    = (char*)AMemLock(m_miText);
  m_nTextLen = (short)AMemGetSize(m_miText);
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(invalidRect);

  HFONT       hfontOld;
  int         nBkModeOld;
  COLORREF    clrOld;
  HPEN        hpenOld;

  hfontOld = (HFONT)SelectObject(hdc, m_hfont);
  nBkModeOld = SetBkMode(hdc, TRANSPARENT);
  clrOld = SetTextColor(hdc, m_font.FGColor());
  DrawText(hdc, m_pText, m_nTextLen-1, &m_rcText, DT_SINGLELINE);
  SetTextColor(hdc, clrOld);
  SelectObject(hdc, hfontOld);

  if(m_line.IsLine3D())
    { // draw a 3D group box
    POINT ptOne = {1, 1};

    OWndDStoLS(m_oiParent, &ptOne, 1);

    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.BGColor() : m_line.FGColor()));

    MoveToEx(hdc, m_nLT + ptOne.x,         m_rcFrame.top + ptOne.y, NULL);
    LineTo(hdc, m_rcFrame.left + ptOne.x,  m_rcFrame.top + ptOne.y);
    LineTo(hdc, m_rcFrame.left + ptOne.x,  m_rcFrame.bottom + ptOne.y);
    LineTo(hdc, m_rcFrame.right + ptOne.x, m_rcFrame.bottom + ptOne.y);
    LineTo(hdc, m_rcFrame.right + ptOne.x, m_rcFrame.top + ptOne.y);
    LineTo(hdc, m_nRT + ptOne.x,           m_rcFrame.top + ptOne.y);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.FGColor() : m_line.BGColor())));

    MoveToEx(hdc, m_nLT,         m_rcFrame.top, NULL);
    LineTo(hdc, m_rcFrame.left,  m_rcFrame.top);
    LineTo(hdc, m_rcFrame.left,  m_rcFrame.bottom);
    LineTo(hdc, m_rcFrame.right, m_rcFrame.bottom);
    LineTo(hdc, m_rcFrame.right, m_rcFrame.top);
    LineTo(hdc, m_nRT,           m_rcFrame.top);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  else
    { // draw a standard group box
    COLORREF  clrBGOld;

    if(!m_line.IsBGTransparent())
      {
      SetBkMode(hdc, OPAQUE);
      clrBGOld = SetBkColor(hdc, m_line.BGColor());
      }

    hpenOld = (HPEN)SelectObject(hdc, m_hpen);

    MoveToEx(hdc, m_nLT,         m_rcFrame.top, NULL);
    LineTo(hdc, m_rcFrame.left,  m_rcFrame.top);
    LineTo(hdc, m_rcFrame.left,  m_rcFrame.bottom);
    LineTo(hdc, m_rcFrame.right, m_rcFrame.bottom);
    LineTo(hdc, m_rcFrame.right, m_rcFrame.top);
    LineTo(hdc, m_nRT,           m_rcFrame.top);

    SelectObject(hdc, hpenOld);
    if(!m_line.IsBGTransparent())
      SetBkColor(hdc, clrBGOld);
    }

  SetBkMode(hdc, nBkModeOld);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::ParentOpened
  (
  )

  {
  m_hpen = m_line.m_CreatePen(m_oiParent);
  m_hfont = m_font.m_CreateFont(m_oiParent);

  InitDrawValues();
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::ParentClosing
  (
  )

  {
  m_font.m_DeleteFont(m_hfont);
  m_hfont = NULL;
  m_line.m_DeletePen(m_hpen);
  m_hpen = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Move
//
// DESCRIPTION: This will move the group box
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::Move
  (
  RECT* rcNew
  )

  {
  // use the default functionality
  OpLiteWndItem::Move(rcNew);

  // figure the new rectangles
  InitDrawValues();
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      DrawGrabHandles
//
// DESCRIPTION: This will draw the grab handles with no lines
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::DrawGrabHandles
  (
  HDC hdc
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    Rectangle(hdc, rcGrab.left, rcGrab.top, rcGrab.right, rcGrab.bottom);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      InvalidateGrabHandles
//
// DESCRIPTION: This is the overriden InvalidateGrabHandles that will
//              not draw a bounding rectangle.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::InvalidateGrabHandles
  (
  BOOL  fErase
  )

  {
  RECT  rcGrab;
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  int   i;

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      PointInItem
//
// DESCRIPTION: This will return TRUE if the point is in the item.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpGroupBox::PointInItem
  (
  POINT*  lppt
  )

  {
  BOOL  fRet;
  int   nDeflate    = m_line.LineWidth();
  RECT  rc;

  CopyRect(&rc, &m_rcItem);
  if(nDeflate < 2*DOT_SIZE)
    {
    if(nDeflate < DOT_SIZE)
      nDeflate = DOT_SIZE;

    InflateRect(&rc, 2*DOT_SIZE-nDeflate, 2*DOT_SIZE-nDeflate);
    }

  fRet = PtInRect(&rc, *lppt);
  if(fRet)
    {
    int   nDefTop;

    CopyRect(&rc, &m_rcItem);
    nDefTop = nMax(nDeflate, MulDiv(m_font.FontHeight(), 254, 1440));
    rc.left   += nDeflate;
    rc.top    += nDefTop;
    rc.right  -= nDeflate;
    rc.bottom -= nDeflate;
    fRet = !PtInRect(&rc, *lppt);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      ItemInRect
//
// DESCRIPTION: This will return TRUE if the item is inside the rectangle.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI ShpGroupBox::ItemInRect
  (
  RECT* lprc,
  BOOL  fReqFullEnclosure
  )

  {
  BOOL  fRet;

  if(fReqFullEnclosure)
    {
    fRet = ((lprc->left   <= m_rcItem.left)
                 &&
            (lprc->top    <= m_rcItem.top)
                 &&
            (lprc->right  >= m_rcItem.right)
                 &&
            (lprc->bottom >= m_rcItem.bottom));
    }
  else
    {
    int   nWidth = nMax(1, m_line.LineWidth());
    int   nHeight = nWidth;
    int   nTopHeight = nMax(nHeight, MulDiv(m_font.FontHeight(), 254, 1440));
    RECT  rc1, rc2, rc3, rc4, rcIntr;

    nWidth  = nMin(nWidth,  (m_rcItem.right  - m_rcItem.left)/2);
    nHeight = nMin(nHeight, (m_rcItem.bottom - m_rcItem.top )/2);

    SetRect(&rc1, m_rcItem.left, m_rcItem.top, m_rcItem.left + nWidth, m_rcItem.bottom);
    SetRect(&rc2, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top + nTopHeight);
    SetRect(&rc3, m_rcItem.right - nWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    SetRect(&rc4, m_rcItem.left, m_rcItem.bottom - nHeight, m_rcItem.right, m_rcItem.bottom);

    fRet = IntersectRect(&rcIntr, &rc1, lprc)
                ||
           IntersectRect(&rcIntr, &rc2, lprc)
                ||
           IntersectRect(&rcIntr, &rc3, lprc)
                ||
           IntersectRect(&rcIntr, &rc4, lprc);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(nPCLMode);

  HFONT       hfontOld;
  TEXTMETRIC  tm;
  RECT        rcText;
  RECT        rcTextExt = {0, 0, 0, 0};
  RECT        rcFrame;
  int         nLT, nRT;
  short       nLineWidth;
  int         n1_2Width;
  int         nBkModeOld;
  COLORREF    clrOld;
  HPEN        hpenOld;

  // get the current line attributes
  nLineWidth = (short)MulDiv(m_line.LineWidth(), GetDeviceCaps(hdc, LOGPIXELSX), 254);
  n1_2Width = (nLineWidth + 1)/2;

  // get the specific rectangles for drawing efficiently
  hfontOld = (HFONT)SelectObject(hdc, m_font.m_CreateFont(NULL, hdc));
  GetTextMetrics(hdc, &tm);
  DrawText(hdc, m_pText, m_nTextLen-1, &rcTextExt, DT_SINGLELINE | DT_CALCRECT);

  rcText.left   = lprcPrint->left + nLineWidth + 2 * tm.tmAveCharWidth;
  rcText.top    = lprcPrint->top  + ((rcTextExt.bottom < nLineWidth) ? (nLineWidth - rcTextExt.bottom)/2 : 0);
  rcText.right  = rcText.left + rcTextExt.right + tm.tmAveCharWidth/2;
  rcText.bottom = rcText.top  + rcTextExt.bottom;

  if(rcText.right > (lprcPrint->right - nLineWidth - 3*tm.tmAveCharWidth/2))
    rcText.right = lprcPrint->right - nLineWidth - 3*tm.tmAveCharWidth/2;

  CopyRect(&rcFrame, lprcPrint);
  rcFrame.left   += n1_2Width;
  rcFrame.top    += (rcTextExt.bottom < nLineWidth) ? n1_2Width : rcTextExt.bottom/2;
  rcFrame.right  -= (n1_2Width + 1);
  rcFrame.bottom -= (n1_2Width + 1);

  nLT = rcFrame.left + tm.tmAveCharWidth;
  nRT = rcText.right + tm.tmAveCharWidth/2 + n1_2Width;

  nBkModeOld = SetBkMode(hdc, TRANSPARENT);
  clrOld = SetTextColor(hdc, m_font.FGColor());
  DrawText(hdc, m_pText, m_nTextLen-1, &rcText, DT_SINGLELINE);
  SetTextColor(hdc, clrOld);
  m_font.m_DeleteFont((HFONT)SelectObject(hdc, hfontOld));

  if(m_line.IsLine3D())
    { // draw a 3D group box
    hpenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.BGColor() : m_line.FGColor()));

    MoveToEx(hdc, nLT + 1,        rcFrame.top + 1, NULL);
    LineTo(hdc, rcFrame.left + 1, rcFrame.top + 1);
    LineTo(hdc, rcFrame.left + 1, rcFrame.bottom);
    LineTo(hdc, rcFrame.right,    rcFrame.bottom);
    LineTo(hdc, rcFrame.right,    rcFrame.top + 1);
    LineTo(hdc, nRT + 1,          rcFrame.top + 1);
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, m_line.Is3DIn() ? m_line.FGColor() : m_line.BGColor())));

    MoveToEx(hdc, nLT,             rcFrame.top, NULL);
    LineTo(hdc, rcFrame.left,      rcFrame.top);
    LineTo(hdc, rcFrame.left,      rcFrame.bottom - 1);
    LineTo(hdc, rcFrame.right - 1, rcFrame.bottom - 1);
    LineTo(hdc, rcFrame.right - 1, rcFrame.top);
    LineTo(hdc, nRT,               rcFrame.top);
    DeleteObject(SelectObject(hdc, hpenOld));
    }
  else
    { // draw a standard group box
    COLORREF  clrBGOld;
    short     nLineStyle;

    if(m_line.IsBGTransparent());
      {
      SetBkMode(hdc, OPAQUE);
      clrBGOld = SetBkColor(hdc, m_line.BGColor());
      }
    nLineStyle = (short)((m_line.LineWidth()>0) ? PS_SOLID : m_line.LineStyle());

    hpenOld = (HPEN)SelectObject(hdc, CreatePen(nLineStyle, nLineWidth, m_line.FGColor()));

    MoveToEx(hdc, nLT,         rcFrame.top, NULL);
    LineTo(hdc, rcFrame.left,  rcFrame.top);
    LineTo(hdc, rcFrame.left,  rcFrame.bottom);
    LineTo(hdc, rcFrame.right, rcFrame.bottom);
    LineTo(hdc, rcFrame.right, rcFrame.top);
    LineTo(hdc, nRT,           rcFrame.top);

    DeleteObject(SelectObject(hdc, hpenOld));

    if(m_line.IsBGTransparent());
      SetBkColor(hdc, clrBGOld);
    }

  SetBkMode(hdc, nBkModeOld);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpGroupBox
//
// METHOD:      AccelKeyHit
//
// DESCRIPTION: This will be called when the text's accelerator key is hit.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpGroupBox::AccelKeyHit
  (
  )

  {
  OWndSetActiveItem(this);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      Constructor based on a MemBlockID
//
// DESCRIPTION: The constructor will initialize a bitmap based on
//              a default from the miExtra field.
//
////////////////////////////////////////////////////////////////////////////

ShpBitmap::ShpBitmap
  (
  OBJECTID      oiItem,
  OBJECTID      oiParent,
  RECT*         rcItem,
  long          lStatus,
  MEMBLOCKID    miExtra
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_BITMAP)

  {
  I_UNUSED(miExtra);

  m_hbmp = NULL;
  } 

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      Destructor
//
// DESCRIPTION: 
//
////////////////////////////////////////////////////////////////////////////

ShpBitmap::~ShpBitmap
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      ShapeName
//
// DESCRIPTION: This will fill a buffer the the shape's name.
//
////////////////////////////////////////////////////////////////////////////

void ShpBitmap::ShapeName
  (
  LPSTR lpszName,
  int   nBufLen
  )

  {
  lstrcpyn(lpszName, "Bitmap", nBufLen);
  lpszName[nBufLen-1] = '\0';
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      Destroy
//
// DESCRIPTION: Deletes the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpBitmap::Destroy
  (
  )

  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      Edit
//
// DESCRIPTION: Edits the visual instance for this shape.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI ShpBitmap::Edit
  (
  BOOL  bEditExtra
  )

  {
  I_UNUSED(bEditExtra);

  return A_OK;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      GetExtraData
//
// DESCRIPTION: Creates a memory block with the visual instance
//              specific information for this shape.
//
////////////////////////////////////////////////////////////////

MEMBLOCKID ALMAPI ShpBitmap::GetExtraData
  (
  )

  {
  MEMBLOCKID    mi = AMemAllocate(sizeof(SHAPE_BITMAP));
  SHAPE_BITMAP* shape = (SHAPE_BITMAP*)AMemLock(mi);

  shape->base.sVer  = SHP_VERSION;
  shape->base.sType = SHP_BITMAP;

  AMemUnlock(mi);

  return mi;
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      ExportExtraData
//
// DESCRIPTION: Exports a bitmap
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpBitmap::ExportExtraData
  (
  )

  {
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      ImportExtraData
//
// DESCRIPTION: Imports a bitmap
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpBitmap::ImportExtraData
  (
  )

  {
  }

////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      Draw
//
// DESCRIPTION: Draws this shape.
//
////////////////////////////////////////////////////////////////

void ALMAPI ShpBitmap::Draw
  (
  HDC   hdc,
  RECT* invalidRect
  )

  {
  I_UNUSED(hdc);
  I_UNUSED(invalidRect);
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      ParentOpened
//
// DESCRIPTION: Method called when the GDI resources need to be created.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpBitmap::ParentOpened
  (
  )

  {
  // create the bitmap handle.
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      ParentClosing
//
// DESCRIPTION: Method called when the GDI resources need to be freed.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpBitmap::ParentClosing
  (
  )

  {
  // free the bitmap
  }

////////////////////////////////////////////////////////////////////////////
//
// CLASS:       ShpBitmap
//
// METHOD:      Print
//
// DESCRIPTION: This will cause the item to draw to the printer DC.
//
////////////////////////////////////////////////////////////////////////////

void ALMAPI ShpBitmap::Print
  (
  HDC   hdc,
  RECT* lprcPrint,
  int   nPCLMode
  )

  {
  I_UNUSED(hdc);
  I_UNUSED(lprcPrint);
  I_UNUSED(nPCLMode);
  }

////////////////////////////////////////////////////////////////////////////
