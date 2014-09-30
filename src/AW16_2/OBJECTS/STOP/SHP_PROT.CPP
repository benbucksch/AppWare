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
#include <bwcc.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>

#include "heap.h"
#include "shp_attr.h"
#include "shp_prot.h"
#include "shapepal.h"
#include "shapeutl.h"
#include "tmp.h"

// externs /////////////////////////////////////////////////////////////////

extern TYPEID   g_StdShapeLibType;

// ****************************************************************
//
// Overridden operator new and delete so we don't eat up selectors
// with small C++ objects.
//
// ****************************************************************

void*   operator new(
  size_t    s
  )
  {
  return MemAlloc(s);
  }

void    operator delete(
  void*     p
  )
  {
  if (p)
    MemFree(p);
  }

// ****************************************************************

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
  ) : OpLiteWndItem(oiItem, oiParent, rcItem, lStatus)

  {
  m_nShapeType = nShapeType;
  OWndSetItemIsShape(this, TRUE);
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
  HPEN          hpen, hpenOld;
  COLORREF      clrFGPen;
  COLORREF      clrFG, clrFGOld;
  COLORREF      clrBG, clrBGOld;
  int           nStyle;
  int           nWidth;
  HBITMAP       hbmp;
  int           nPattern;

  // Draw the background for Borland dialog
  FillRect(lpdi->hDC, &lpdi->rcItem, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

  // Create and select the pen
  lRes = SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETCURSEL, 0, 0L);
  clrFGPen = (COLORREF)SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  if(clrFGPen==COLOR_WINDOWFOREGROUND)
    clrFGPen = GetSysColor(COLOR_WINDOWTEXT);
  lRes = SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETCURSEL, 0, 0L);
  nStyle = (int)SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  nWidth = LineAttrib::GetLineWidthFromDlg(hwnd); // in tenths of a millimeter
  if(nWidth > 0)
    {
    // convert to screen coordinates
    nWidth = (double)nWidth  * (double)GetDeviceCaps(lpdi->hDC, LOGPIXELSY) / 254. + 0.5;
    nStyle = PS_INSIDEFRAME;
    }
  hpen = CreatePen(nStyle, nWidth, clrFGPen);
  hpenOld = (HPEN)SelectObject(lpdi->hDC, hpen);

  // Set the colors and mode
  lRes = SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETCURSEL, 0, 0L);
  clrFG = (COLORREF)SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  if(clrFG==COLOR_WINDOWFOREGROUND)
    clrFG = GetSysColor(COLOR_WINDOWTEXT);
  lRes = SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETCURSEL, 0, 0L);
  clrBG = (COLORREF)SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  if(clrBG==COLOR_WINDOWBACKGROUND)
    clrBG = GetSysColor(COLOR_WINDOW);
  clrFGOld = SetTextColor(lpdi->hDC, clrFG);
  clrBGOld = SetBkColor(lpdi->hDC, clrBG);

  // Create and select the brush
  lRes = SendDlgItemMessage(hwnd, CB_FILLPATTERN, CB_GETCURSEL, 0, 0L);
  nPattern = (int)SendDlgItemMessage(hwnd, CB_FILLPATTERN, CB_GETITEMDATA, (WPARAM)lRes, 0);
  hbmp = CreateBitmap(8, 8, 1, 1, FillAttrib::GetFillPattern(nPattern));
  hbr = CreatePatternBrush(hbmp);
  hbrOld = (HBRUSH)SelectObject(lpdi->hDC, hbr);

  // have the actual shape draw the sample
  shape->DrawSample(hwnd, lpdi);

  // clean up the pen, brush, colors, etc.
  SelectObject(lpdi->hDC, hpenOld);
  DeleteObject(hpen);
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
  lRes = SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETCURSEL, 0, 0L);
  line.clrFG = (COLORREF)SendDlgItemMessage(hwnd, CB_LINEFGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  lRes = SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETCURSEL, 0, 0L);
  line.nStyle = (int)SendDlgItemMessage(hwnd, CB_LINESTYLE, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  line.nWidth = LineAttrib::GetLineWidthFromDlg(hwnd);

  // update the fill attributes based on the dialog
  lRes = SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETCURSEL, 0, 0L);
  fill.clrFG = (COLORREF)SendDlgItemMessage(hwnd, CB_FILLFGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  lRes = SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETCURSEL, 0, 0L);
  fill.clrBG = (COLORREF)SendDlgItemMessage(hwnd, CB_FILLBGCOLOR, CB_GETITEMDATA, (WPARAM)lRes, 0L);
  lRes = SendDlgItemMessage(hwnd, CB_FILLPATTERN, CB_GETCURSEL, 0, 0L);
  fill.nPattern = (int)SendDlgItemMessage(hwnd, CB_FILLPATTERN, CB_GETITEMDATA, (WPARAM)lRes, 0L);

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

BOOL WINAPI _export ShpItemFill::FilledShapeDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL          fRet = FALSE;
  ShpItemFill*  shape;

  switch(uMsg)
    {
    case WM_INITDIALOG:
      {
      SHPFILL_EDIT* shpedit = (SHPFILL_EDIT*)lParam;
      ATTR_FILL     fill;
      ATTR_LINE     line;
      shape = shpedit->shape;
      SetWindowLong(hwnd, DWL_USER, (long)shpedit);
      shape->GetAttributes(&line, &fill);

      ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_LINEFGCOLOR), line.clrFG, TRUE, FALSE);
      LineAttrib::FillLineStyleComboBox(GetDlgItem(hwnd, CB_LINESTYLE), line.nStyle);
      LineAttrib::InitLineWidth(hwnd, line.nWidth);

      ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_FILLFGCOLOR), fill.clrFG, TRUE, FALSE);
      ColorAttrib::FillColorComboBox(GetDlgItem(hwnd, CB_FILLBGCOLOR), fill.clrBG, FALSE, FALSE);
      FillAttrib::FillPatternComboBox(GetDlgItem(hwnd, CB_FILLPATTERN), fill.nPattern);

      if(shpedit->szTitle)
        SetWindowText(hwnd, shpedit->szTitle);
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
      switch(wParam)
        {
        case IDOK:
          EndDialog(hwnd, SaveNewAttributes(hwnd));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
          break;

        case CB_LINEFGCOLOR:
        case CB_FILLFGCOLOR:
        case CB_FILLBGCOLOR:
        case CB_LINESTYLE:
        case CB_FILLPATTERN:
          if(HIWORD(lParam) == CBN_SELCHANGE)
            InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
          break;

        case RB_LINEWIDTH:
          LineAttrib::SetEditFocus(hwnd, ED_LINEWIDTH, FALSE);
          InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
          fRet = TRUE;
          break;

        case RB_HAIRLINE:
          EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), TRUE);
          EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), TRUE);
          InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
          fRet = TRUE;
          break;

        case ED_LINEWIDTH:
          switch(HIWORD(lParam))
            {
            case EN_KILLFOCUS:
              LineAttrib::SetItemValue(hwnd, wParam, LineAttrib::GetItemValue(hwnd, wParam));
              InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
              fRet = TRUE;
              break;

            case EN_SETFOCUS:
              {
              int   nWidth = LineAttrib::GetItemValue(hwnd, ED_LINEWIDTH);

              EnableWindow(GetDlgItem(hwnd, CB_LINESTYLE), nWidth==0);
              EnableWindow(GetDlgItem(hwnd, ST_LINESTYLE), nWidth==0);

              CheckDlgButton(hwnd, RB_LINEWIDTH, TRUE);
              CheckDlgButton(hwnd, RB_HAIRLINE, FALSE);
              InvalidateRect(GetDlgItem(hwnd, PB_SAMPLE), NULL, TRUE);
              fRet = TRUE;
              break;
              }

            default:
              break;
            }
          break;

        default:
          break;
        }
      break;

    case WM_VSCROLL:
      LineAttrib::WidthVScrollMsg(hwnd, wParam, lParam, PB_SAMPLE);
      fRet = TRUE;
      break;

    default:
      break;
    }

  return fRet;
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
  strncpy(lpszName, "Filled Rectangle", nBufLen);
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
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  I_UNUSED(hwnd);

  Rectangle(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom);
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
  HPEN      hpenOld;
  COLORREF  clrFGOld, clrBGOld;
  RECT      rc;

  UnrealizeObject(m_hbr);
  CopyRect(&rc, &m_rcItem);
  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 1);
  MapWindowPoints(m_hwndParent, HWND_DESKTOP, (LPPOINT)&rc, 1);
  SetBrushOrg(hdc, rc.left, rc.top);

  hbrOld = (HBRUSH)SelectObject(hdc, m_hbr);
  hpenOld = (HPEN)SelectObject(hdc, m_hpen);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  Rectangle(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
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
  m_hpen = m_line.CreatePen(m_oiParent);
  m_hbr = m_fill.CreateBrush();
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
  m_line.DeletePen(m_hpen);
  m_hpen = NULL;
  m_fill.DeleteBrush(m_hbr);
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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
  strncpy(lpszName, "Rectangle", nBufLen);
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

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
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
  m_hpen = m_line.CreatePen(m_oiParent);
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
  m_line.DeletePen(m_hpen);
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  fRet = PtInRect(&m_rcItem, *lppt);
  if(fRet)
    {
    POINT ptSize = {DOT_SIZE, DOT_SIZE};
    int   nDeflate;
    RECT  rc;

    OWndDStoLS(m_oiParent, &ptSize, 1);
    CopyRect(&rc, &m_rcItem);
    nDeflate = -nMax(ptSize.x, m_line.LineWidth());
    InflateRect(&rc, nDeflate, nDeflate);
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
  strncpy(lpszName, "Filled Ellipse/Circle", nBufLen);
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
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  I_UNUSED(hwnd);

  Ellipse(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom);
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
  HPEN      hpenOld;
  COLORREF  clrFGOld, clrBGOld;
  RECT      rc;

  UnrealizeObject(m_hbr);
  CopyRect(&rc, &m_rcItem);
  OWndLPtoDP(m_oiParent, (LPPOINT)&rc, 1);
  MapWindowPoints(m_hwndParent, HWND_DESKTOP, (LPPOINT)&rc, 1);
  SetBrushOrg(hdc, rc.left, rc.top);

  hbrOld = (HBRUSH)SelectObject(hdc, m_hbr);
  hpenOld = (HPEN)SelectObject(hdc, m_hpen);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  Ellipse(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
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
  m_hpen = m_line.CreatePen(m_oiParent);
  m_hbr = m_fill.CreateBrush();
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
  m_line.DeletePen(m_hpen);
  m_hpen = NULL;
  m_fill.DeleteBrush(m_hbr);
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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
  strncpy(lpszName, "Ellipse/Circle", nBufLen);
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
  HPEN      hpenOld = (HPEN)SelectObject(hdc, m_hpen);
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

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
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
  m_hpen = m_line.CreatePen(m_oiParent);
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
  m_line.DeletePen(m_hpen);
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  fRet = PointInEllipse(pt, &m_rcItem);
  if(fRet)
    {
    RECT  rcTemp;
    POINT ptSize = {DOT_SIZE, DOT_SIZE};
    int   nDeflate;

    OWndDStoLS(m_oiParent, &ptSize, 1);
    nDeflate = -nMax(ptSize.x, m_line.LineWidth());
    CopyRect(&rcTemp, &m_rcItem);
    InflateRect(&rcTemp, nDeflate, nDeflate);

    fRet = !PointInEllipse(pt, &rcTemp);
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

    OWndDStoLS(m_oiParent, &ptSize, 1);
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
    lppt->x = (lprc->right - lprc->left)/5;
    lppt->y = (lprc->bottom - lprc->top)/5;
    (lppt->x<lppt->y) ? (lppt->y=lppt->x) : (lppt->x=lppt->y);
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
  int           xRad,
  int           yRad
  ) : ShpItemFill(oiItem, oiParent, rcItem, lStatus, SHP_ROUNDRECTFILL),
      m_line(line),
      m_fill(fill)

  {
  m_ptRadius.x = xRad;
  m_ptRadius.y = yRad;

  OWndSetItemTransparent(this, TRUE);
  m_hbr = NULL;
  m_hpen = NULL;
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
  strncpy(lpszName, "Filled Rounded Rectangle", nBufLen);
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
  LPDRAWITEMSTRUCT  lpdi
  )

  {
  I_UNUSED(hwnd);

  POINT ptRadius = {-1, -1};  // have the roundness be based on rectangle size
  POINT pt;

  FixRoundRectRadius(ptRadius, &lpdi->rcItem, &pt);

  RoundRect(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, lpdi->rcItem.right, lpdi->rcItem.bottom, pt.x, pt.y);
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
  shape->ptRadius   = m_ptRadius;

  AMemUnlock(mi);

  return mi;
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
  SetBrushOrg(hdc, rc.left, rc.top);

  hbrOld = (HBRUSH)SelectObject(hdc, m_hbr);
  hpenOld = (HPEN)SelectObject(hdc, m_hpen);
  clrBGOld = SetBkColor(hdc, m_fill.BGColor());
  clrFGOld = SetTextColor(hdc, m_fill.FGColor());

  RoundRect(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, pt.x, pt.y);

  SetBkColor(hdc, clrBGOld);
  SetTextColor(hdc, clrFGOld);
  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
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
  m_hpen = m_line.CreatePen(m_oiParent);
  m_hbr = m_fill.CreateBrush();
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
  m_line.DeletePen(m_hpen);
  m_hpen = NULL;
  m_fill.DeleteBrush(m_hbr);
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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
  int           xRad,
  int           yRad
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_ROUNDRECT),
      m_line(line)

  {
  m_ptRadius.x = xRad;
  m_ptRadius.y = yRad;

  OWndSetItemTransparent(this, TRUE);
  m_hpen = NULL;
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
  strncpy(lpszName, "Rounded Rectangle", nBufLen);
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
  shape->ptRadius   = m_ptRadius;

  AMemUnlock(mi);

  return mi;
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
  HPEN      hpenOld = (HPEN)SelectObject(hdc, m_hpen);
  int       nOldBkMode;
  COLORREF  clrOldBG;
  POINT     pt;

  if(m_line.IsBGTransparent())
    nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  else
    clrOldBG = SetBkColor(hdc, m_line.BGColor());

  FixRoundRectRadius(m_ptRadius, &m_rcItem, &pt);
  RoundRect(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, pt.x, pt.y);

  if(m_line.IsBGTransparent())
    SetBkMode(hdc, nOldBkMode);
  else
    SetBkColor(hdc, clrOldBG);

  SelectObject(hdc, hbrOld);
  SelectObject(hdc, hpenOld);
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
  m_hpen = m_line.CreatePen(m_oiParent);
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
  m_line.DeletePen(m_hpen);
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

    OWndDStoLS(m_oiParent, &ptSize, 1);
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
  POINT ptSize = {DOT_SIZE, DOT_SIZE};
  POINT ptRR;
  RECT  rcTemp;
  HRGN  hrgnOutside;
  HRGN  hrgnInside;
  HRGN  hrgnRRect;
  int   nDeflate;

  FixRoundRectRadius(m_ptRadius, &m_rcItem, &ptRR);
  hrgnOutside = CreateRoundRectRgn(m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom, ptRR.x, ptRR.y);

  OWndDStoLS(m_oiParent, &ptSize, 1);
  nDeflate = -nMax(ptSize.x, m_line.LineWidth());
  CopyRect(&rcTemp, &m_rcItem);
  InflateRect(&rcTemp, nDeflate, nDeflate);
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
  strncpy(lpszName, "Line", nBufLen);
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

  int       nOldBkMode;
  COLORREF  clrBGOld;
  HPEN      hpenOld = (HPEN)SelectObject(hdc, m_hpen);

  if(m_line.IsBGTransparent())
    nOldBkMode = SetBkMode(hdc, TRANSPARENT);
  else
    clrBGOld = SetBkColor(hdc, m_line.BGColor());

  MoveTo(hdc, m_rcItem.left,  m_rcItem.top);
  LineTo(hdc, m_rcItem.right, m_rcItem.bottom);

  if(m_line.IsBGTransparent())
    SetBkMode(hdc, nOldBkMode);
  else
    SetBkColor(hdc, clrBGOld);
  SelectObject(hdc, hpenOld);
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
  MoveTo(hdc, rect->left,  rect->top);
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
  m_hpen = m_line.CreatePen(m_oiParent);
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
  m_line.DeletePen(m_hpen);
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

  OWndDStoLS(m_oiParent, &ptDot, 1);

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
            (lprc->bottom >= (nMin(m_rcItem.top,  m_rcItem.bottom) + nExtra)));
    }
  else
    {
    POINT   ptDot = {DOT_SIZE, DOT_SIZE};

    OWndDStoLS(m_oiParent, &ptDot, 1);

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

  OWndDStoLS(m_oiParent, &pt, 1);
  
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

  OWndDStoLS(m_oiParent, &pt, 1);
  
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  if(newRect->right<0)
    {
    newRect->right = newRect->left;
    lAdjNewRect |= (OWND_ADJRECT_CHANGED_R | OWND_ADJRECT_DEFAULT_W);
    }

  if(newRect->bottom<0)
    {
    newRect->bottom = newRect->top;
    lAdjNewRect |= (OWND_ADJRECT_CHANGED_B | OWND_ADJRECT_DEFAULT_H);
    }

  // force a minimum length....!
  // currently the minimum length is the size of the diagonal of a grab handle.
  // this may need to change.....
  OWndDStoLS(m_oiParent, &ptSize, 1);
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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
  I_ASSERT(m_hpen);

  LOGPEN  lp;
  RECT    rc;

  GetObject(m_hpen, sizeof(LOGPEN), &lp);
  MakeRealRect(&rc, &m_rcItem);
  InflateRect(&rc, (lp.lopnWidth.x + 1)/2, (lp.lopnWidth.x + 1)/2);
  OWndInvalidateLogicalRect(m_oiParent, &rc, fErase);
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
  UINT          nAlign,
  int           nTextLen,
  char*         pText,
  BOOL          fEditText
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_TEXT),
      m_font(font)

  {
  m_miText = AMemAllocate(nTextLen);
  m_nTextLen = nTextLen;
  m_nAlign = nAlign;
  m_pText = (char*)AMemLock(m_miText);
  if(m_pText)
    strcpy(m_pText, pText);

  if(fEditText)
    Edit(TRUE);

  OWndSetItemTransparent(this, TRUE);
  m_hfont = NULL;
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

BOOL CALLBACK _export ShpText::TextEntryDlgProc
  (
  HWND    hDlg,
  UINT    nMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL        fHandled  = TRUE;
  HWND        hEdit;
  ShpText*    pShape;
  RECT        rcWindow;
  int         nScreenX;
  int         nScreenY;
  int         nDlgWidth;
  int         nDlgHeight;
  FontAttrib* pFont;

  switch (nMsg)
    {
    case WM_INITDIALOG:
      {
      ATTR_FONT font;

      SetWindowLong(hDlg, DWL_USER, lParam);

      pShape = (ShpText*)lParam;
      hEdit = GetDlgItem(hDlg, DLG_TEXT_ENTRY);
      SetWindowText(hEdit, pShape->m_pText);
      pShape->m_font.PrepForFile(&font);
      pFont = new FontAttrib(&font);
      SetProp(hDlg, "loFont", (HANDLE)LOWORD((long)pFont));
      SetProp(hDlg, "hiFont", (HANDLE)HIWORD((long)pFont));

      SendMessage(hEdit, EM_SETSEL, TRUE, MAKELONG(0, -1));   // select all
      SetFocus(hEdit);

      // Get size of stuff to center dialog
      GetWindowRect(hDlg, &rcWindow);
      nDlgWidth  = rcWindow.right  - rcWindow.left;
      nDlgHeight = rcWindow.bottom - rcWindow.top;
      nScreenX = GetSystemMetrics(SM_CXSCREEN);
      nScreenY = GetSystemMetrics(SM_CYSCREEN);

      // Now center
      nScreenX = (nScreenX - nDlgWidth) / 2;
      nScreenY = (nScreenY - nDlgHeight) / 2;
      MoveWindow(hDlg, nScreenX, nScreenY, nDlgWidth, nDlgHeight, FALSE);

      fHandled = FALSE;
      }
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDOK:
          {
          BOOL  fChanged = FALSE;
          char* szText;
          int   nLen;

          pShape = (ShpText*)GetWindowLong(hDlg, DWL_USER);
          pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hDlg, "loFont"), (WORD)GetProp(hDlg, "hiFont"));

          hEdit  = GetDlgItem(hDlg, DLG_TEXT_ENTRY);
          nLen = GetWindowTextLength(hEdit) + 1;
          szText = new char[nLen];
          GetWindowText(hEdit, szText, nLen);

          // check to see if there was a change
          fChanged = !strcmp(szText, pShape->m_pText) || (*pFont == pShape->m_font);

          // copy the new data if there was a change
          if(fChanged)
            {
            pShape->m_nTextLen = (short)nLen;
            AMemUnlock(pShape->m_miText);
            AMemSetSize(pShape->m_miText, pShape->m_nTextLen);
            pShape->m_pText = (char*)AMemLock(pShape->m_miText);
            strcpy(pShape->m_pText, szText);
            pShape->m_font = *pFont;
            }

          // free the memory and end the dialog
          delete szText;
          delete pFont;
          EndDialog(hDlg, fChanged);
          }
          break;

        case IDCANCEL:
          pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hDlg, "loFont"), (WORD)GetProp(hDlg, "hiFont"));
          delete pFont;
          EndDialog(hDlg, FALSE);
          break;

        case DLG_FONT:
          {
          char  szText[50];

          pFont = (FontAttrib*)MAKELONG((WORD)GetProp(hDlg, "loFont"), (WORD)GetProp(hDlg, "hiFont"));
          hEdit  = GetDlgItem(hDlg, DLG_TEXT_ENTRY);
          GetWindowText(hEdit, szText, 50);
          pFont->EditFont(hDlg, "Edit Font", szText, FALSE, TRUE, FALSE);
          break;
          }

        default:
          break;
        }
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
  strncpy(lpszName, "Static Text", nBufLen);
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
  strcpy(shape->szText, m_pText);

  AMemUnlock(mi);

  return mi;
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
  m_hfont = m_font.CreateFont(m_oiParent);
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
  m_font.DeleteFont(m_hfont);
  m_hfont = NULL;
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
  int           nTextLen,
  char*         pText
  ) : ShpItem(oiItem, oiParent, rcItem, lStatus, SHP_GROUPBOX),
      m_line(line),
      m_font(font)

  {
  m_miText = AMemAllocate(nTextLen);
  m_nTextLen = nTextLen;
  m_pText = (char*)AMemLock(m_miText);
  if(m_pText)
    strcpy(m_pText, pText);

  OWndSetItemTransparent(this, TRUE);
  m_hfont = NULL;
  m_hpen = NULL;
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
  strncpy(lpszName, "Group Box", nBufLen);
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
  strcpy(shape->szText, m_pText);

  AMemUnlock(mi);

  return mi;
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
  m_hpen = m_line.CreatePen(m_oiParent);
  m_hfont = m_font.CreateFont(m_oiParent);
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
  m_font.DeleteFont(m_hfont);
  m_hfont = NULL;
  m_line.DeletePen(m_hpen);
  m_hpen = NULL;
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

  OWndDStoLS(m_oiParent, &ptSize, 1);

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

  OWndDStoLS(m_oiParent, &ptSize, 1);

  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    CreateGrabHandle(i, &rcGrab, &m_rcItem, &ptSize);
    OWndInvalidateLogicalRect(m_oiParent, &rcGrab, fErase);
    }
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
  strncpy(lpszName, "Bitmap", nBufLen);
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
