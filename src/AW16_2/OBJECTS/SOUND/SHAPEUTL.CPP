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
// FILE:          SHAPEUTL.CPP
//
// AUTHOR:        Scott McCarty (converted from old Window Shapes)
//
// DESCRIPTION:   Utility functions for shapes
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define ASTRICT
#define STRICT

#include <math.h>
#include <stdlib.h>
#include <windows.h>

#include <a_alm.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <intdebug.h>

#include "shapeutl.h"

////////////////////////////////////////////////////////////////
//
// FUNCTION:  CreateCenteredGrabHandle
//
// DESCR:     Creates a grab handle around a point of size lpptSize
//
////////////////////////////////////////////////////////////////

void CreateCenteredGrabHandle(
  LPRECT        lprcHandle,
  const POINT*  lpptRef,
  const POINT*  lpptSize
  )
  {
  lprcHandle->left   = lpptRef->x - lpptSize->x/2;
  lprcHandle->top    = lpptRef->y - lpptSize->y/2;
  lprcHandle->right  = lpptRef->x + lpptSize->x/2;
  lprcHandle->bottom = lpptRef->y + lpptSize->y/2;
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

static int  ccw(
  POINT       p0,
  POINT       p1,
  POINT       p2
  )
  {
  long      dx1, dx2, dy1, dy2;

  dx1 = p1.x - p0.x;  dy1 = p1.y - p0.y;
  dx2 = p2.x - p0.x;  dy2 = p2.y - p0.y;

  if (dx1*dy2 > dy1*dx2)
    return 1;

  if (dx1*dy2 < dy1*dx2)
    return -1;

  if ((dx1*dx2 < 0) || (dy1*dy2 < 0))
    return -1;

  if ((dx1*dx1+dy1*dy1) < (dx2*dx2+dy2*dy2))
    return 1;

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

static BOOL   LinesIntersect(
  POINT         line1Pt1,
  POINT         line1Pt2,
  POINT         line2Pt1,
  POINT         line2Pt2
  )
  {
  return  (( ccw(line1Pt1, line1Pt2, line2Pt1) *
             ccw(line1Pt1, line1Pt2, line2Pt2)) <=0)
          &&
          (( ccw(line2Pt1, line2Pt2, line1Pt1) *
             ccw(line2Pt1, line2Pt2, line1Pt2)) <=0);

  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

void  MakeRealRect(
  RECT*   target,
  RECT*   source
  )
  {
  target->top    = min(source->top, source->bottom);
  target->bottom = max(source->top, source->bottom);
  target->left   = min(source->left, source->right);
  target->right  = max(source->left, source->right);
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

void DrawFrame(
  HDC       hdc,
  RECT      r
  )
  {
  if (r.top == r.bottom) {r.top--; r.bottom++;}
  if (r.left == r.right) {r.left--; r.right++;}

  MoveToEx(hdc, min(r.left, r.right), min(r.top, r.bottom), NULL);

  //The -1 will make it act like the Rectangle function
  LineTo(hdc, min(r.left, r.right), max(r.top, r.bottom) - 1 );
  LineTo(hdc, max(r.left, r.right) - 1, max(r.top, r.bottom) - 1 );
  LineTo(hdc, max(r.left, r.right) - 1, min(r.top, r.bottom) );
  LineTo(hdc, min(r.left, r.right), min(r.top, r.bottom) );
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

void CreateGrabHandle
  (
  int           nGrabHandle,
  LPRECT        lprcHandle,
  const RECT*   lprcItem,
  const POINT*  lpptSize
  )

  {
  int i;
  
  switch(nGrabHandle)
    {
    case OWND_POINT_IN_TL:
      SetRect(lprcHandle, lprcItem->left, lprcItem->top, lprcItem->left+lpptSize->x, lprcItem->top+lpptSize->y);
      break;

    case OWND_POINT_IN_TR:
      SetRect(lprcHandle, lprcItem->right-lpptSize->x, lprcItem->top, lprcItem->right, lprcItem->top+lpptSize->y);
      break;

    case OWND_POINT_IN_BL:
      SetRect(lprcHandle, lprcItem->left, lprcItem->bottom-lpptSize->y, lprcItem->left+lpptSize->x, lprcItem->bottom);
      break;

    case OWND_POINT_IN_BR:
      SetRect(lprcHandle, lprcItem->right-lpptSize->x, lprcItem->bottom-lpptSize->y, lprcItem->right, lprcItem->bottom);
      break;

    case OWND_POINT_IN_T:
      i = (lprcItem->right + lprcItem->left - lpptSize->x)/2; //horz start for top & bottom
      SetRect(lprcHandle, i, lprcItem->top, i+lpptSize->x, lprcItem->top+lpptSize->y);
      break;

    case OWND_POINT_IN_R:
      i = (lprcItem->bottom + lprcItem->top - lpptSize->y)/2; //vert start for left & right
      SetRect(lprcHandle, lprcItem->right-lpptSize->x, i, lprcItem->right, i+lpptSize->y);
      break;

    case OWND_POINT_IN_L:
      i = (lprcItem->bottom + lprcItem->top - lpptSize->y)/2; //vert start for left & right
      SetRect(lprcHandle, lprcItem->left, i, lprcItem->left+lpptSize->x, i+lpptSize->y);
      break;

    case OWND_POINT_IN_B:
      i = (lprcItem->right + lprcItem->left - lpptSize->x)/2; //horz start for top & bottom
      SetRect(lprcHandle, i, lprcItem->bottom-lpptSize->y, i+lpptSize->x, lprcItem->bottom);
      break;
    }
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

BOOL    PtOnLine
  (
  RECT      rcLine,
  POINT     pt,
  int       leeway
  )
  {
  // this routine will determine if a point is within a distance of
  // leeway from the line.
  //                         .
  //                     d2 /
  //                dC     / dB
  //                      x  d1
  //                  d3 /  `.
  //                    /    
  //                   .  dA
  //
  // We want to get the distance from the point to the line (d1).  If
  // the distance is less than leeway, then the point is on the line.
  //
  // d3 = (dC^2 + dB^2 - dA^2)/(2 * dC)
  // d2 = dC - d3
  // d1 = sqrt(dB^2 - d3^2)
  //
  // For the point to be within leeway from the line segment, the following
  // statements must be true.
  // 1) abs(d2) + abs(d3) <= dC + 2*leeway;
  // 2) d1 <= leeway
  BOOL    fRet;

  if((rcLine.left==rcLine.right) && (rcLine.top==rcLine.bottom))
    {
    // The line has collapsed to a point.  Make sure the distance
    // from the line is less than leeway.
    long  d;

    d = (long)(rcLine.bottom - pt.y) * (long)(rcLine.bottom - pt.y) +
        (long)(rcLine.right - pt.x) * (long)(rcLine.right - pt.x);

    if(sqrt(d) <= (double)leeway)
      fRet = TRUE;
    else
      fRet = FALSE;
    }
  else
    {
    double  d1, d2, d3, d3_2;
    double  dC;
    long    dA2, dB2, dC2;

    dA2   = (long)(rcLine.bottom - pt.y) * (long)(rcLine.bottom - pt.y) +
            (long)(rcLine.right - pt.x) * (long)(rcLine.right - pt.x);

    dB2   = (long)(rcLine.top - pt.y) * (long)(rcLine.top - pt.y) +
            (long)(rcLine.left - pt.x) * (long)(rcLine.left - pt.x);

    dC2   = (long)(rcLine.top - rcLine.bottom) * (long)(rcLine.top - rcLine.bottom) +
            (long)(rcLine.left - rcLine.right) * (long)(rcLine.left - rcLine.right);

    dC    = sqrt(dC2);

    d3    = (dC2 + dB2 - dA2)/(2. * dC);
    d3_2  = d3 * d3;
    d2    = dC - d3;
    d1    = (dB2 > d3_2) ? sqrt(dB2 - d3_2) : 0.;

    if((d1 <= (double)leeway)
          &&
       ((fabs(d2) + fabs(d3)) <= (dC + 2*leeway)))
      fRet = TRUE;
    else
      fRet = FALSE;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

BOOL  RectAndLineIntersect(
  RECT      rect,
  RECT      line,
  int       leeway
  )
  {
  POINT     rectlinePt1, rectlinePt2;
  POINT     linePt1, linePt2;
  POINT     pt;
  RECT      rcLine;

  linePt1.x = line.left; linePt1.y = line.top;
  linePt2.x = line.right; linePt2.y = line.bottom;

  //If either end point is in the rect
  if (PtInRect(&rect, linePt1))
    return TRUE;

  if (PtInRect(&rect, linePt2))
    return TRUE;

  // Next see if the line crosses any of the rectangles lines

  //tl - tr
  rectlinePt1.x = rect.left;  rectlinePt1.y = rect.top;
  rectlinePt2.x = rect.right; rectlinePt2.y = rect.top;
  if ( LinesIntersect(linePt1, linePt2, rectlinePt1, rectlinePt2) )
    return TRUE;

  //bl - br
  rectlinePt1.x = rect.left;  rectlinePt1.y = rect.bottom;
  rectlinePt2.x = rect.right; rectlinePt2.y = rect.bottom;
  if ( LinesIntersect(linePt1, linePt2, rectlinePt1, rectlinePt2) )
    return TRUE;

  //tl - bl
  rectlinePt1.x = rect.left;  rectlinePt1.y = rect.top;
  rectlinePt2.x = rect.left;  rectlinePt2.y = rect.bottom;
  if ( LinesIntersect(linePt1, linePt2, rectlinePt1, rectlinePt2) )
    return TRUE;

  //tr - br
  rectlinePt1.x = rect.right; rectlinePt1.y = rect.top;
  rectlinePt2.x = rect.right; rectlinePt2.y = rect.bottom;
  if ( LinesIntersect(linePt1, linePt2, rectlinePt1, rectlinePt2) )
    return TRUE;

  // next see if the rectangle corners are within "leeway" of the line
  pt.x = rect.left;
  pt.y = rect.top;
  if(PtOnLine(line, pt, leeway))
    return TRUE;

  pt.x = rect.right;
  if(PtOnLine(line, pt, leeway))
    return TRUE;

  pt.y = rect.bottom;
  if(PtOnLine(line, pt, leeway))
    return TRUE;

  pt.x = rect.left;
  if(PtOnLine(line, pt, leeway))
    return TRUE;

  // next see if the line ends are within "leeway" of the rectangle edges
  pt.x = line.left;
  pt.y = line.top;
  SetRect(&rcLine, rect.left, rect.top, rect.left, rect.bottom);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  SetRect(&rcLine, rect.left, rect.top, rect.right, rect.top);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  SetRect(&rcLine, rect.right, rect.top, rect.right, rect.bottom);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  SetRect(&rcLine, rect.left, rect.bottom, rect.right, rect.bottom);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  pt.x = line.right;
  pt.y = line.bottom;
  SetRect(&rcLine, rect.left, rect.top, rect.left, rect.bottom);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  SetRect(&rcLine, rect.left, rect.top, rect.right, rect.top);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  SetRect(&rcLine, rect.right, rect.top, rect.right, rect.bottom);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  SetRect(&rcLine, rect.left, rect.bottom, rect.right, rect.bottom);
  if(PtOnLine(rcLine, pt, leeway))
    return TRUE;

  return FALSE;
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
// DESCR:
//
////////////////////////////////////////////////////////////////

BOOL  PointInEllipse
  (
  LPPOINT lppt,
  LPRECT  lprcEllipse
  )

  {
  BOOL  fRet;
  double  a = (lprcEllipse->right - lprcEllipse->left) / 2.;
  double  b = (lprcEllipse->bottom - lprcEllipse->top) / 2.;
  double  c = sqrt(fabs(a*a - b*b));

  // distance from the ellipse focii to the point -- for an ellipse
  // (d1 + d2) <= 2*max(a, b)
  double  d1;
  double  d2;

  if(a>b)
    {
    d1 = sqrt(pow(lppt->x - (lprcEllipse->left + a - c), 2) + pow(lppt->y - (lprcEllipse->top + b), 2));
    d2 = sqrt(pow(lppt->x - (lprcEllipse->left + a + c), 2) + pow(lppt->y - (lprcEllipse->top + b), 2));

    fRet = ((d1 + d2) <= (2*a));
    }
  else if(b>a)
    {
    d1 = sqrt(pow(lppt->x - (lprcEllipse->left + a), 2) + pow(lppt->y - (lprcEllipse->top + b - c), 2));
    d2 = sqrt(pow(lppt->x - (lprcEllipse->left + a), 2) + pow(lppt->y - (lprcEllipse->top + b + c), 2));

    fRet = ((d1 + d2) <= (2*b));
    }
  else  // the ellipse is a circle
    {
    d1 = sqrt(pow(lppt->x - (lprcEllipse->left + a), 2) + pow(lppt->y - (lprcEllipse->top + a), 2));
    fRet = (d1 <= a);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  Ellipse3DShadowPoints
//
// DESCR:     This function will give the points where the ellipse
//            has a slope of -1.
//
////////////////////////////////////////////////////////////////

void Ellipse3DShadowPoints
  (
  const LPRECT  lprcEllipse,
  LPPOINT       lpptLB,
  LPPOINT       lpptRT
  )

  {
  double  a = (lprcEllipse->right - lprcEllipse->left) / 2.;
  double  b = (lprcEllipse->bottom - lprcEllipse->top) / 2.;
  double  c = sqrt(fabs(a*a + b*b));

  I_ASSERT(a!=0. || b!=0.);

  if(lpptLB)
    {
    lpptLB->x = (int)(lprcEllipse->left + a - a*a/c - 0.5);
    lpptLB->y = (int)(lprcEllipse->top  + b + b*b/c - 0.5);
    }
  if(lpptRT)
    {
    lpptRT->x = (int)(lprcEllipse->left + a + a*a/c + 0.5);
    lpptRT->y = (int)(lprcEllipse->top  + b - b*b/c + 0.5);
    }
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  RoundRect3DFrame
//
// DESCR:     This function will draw a 3D rounded rectangle
//            frame.
//
////////////////////////////////////////////////////////////////

void RoundRect3DFrame
  (
  HDC           hdc,
  const LPRECT  lprc,   // rectangle
  LPPOINT       lppt,   // rounded corners
  COLORREF      clrLT,  // top left color
  COLORREF      clrRB   // bottom right color
  )

  {
  // this function will draw the RoundRect frame in two sections.
  // The top left section will be drawn in one color and the
  // bottom right will be drawn in a differnt color.
  POINT ptZero        = {0, 0};
  POINT ptOne         = {1, 1};
  RECT  rcEllipse;
  RECT  rc            = *lprc;
  POINT ptLB, ptRT;
  HPEN  hpenOld       = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, clrLT));

  DPtoLP(hdc, &ptZero, 1);
  DPtoLP(hdc, &ptOne, 1);
  ptOne.x -= ptZero.x;
  ptOne.y -= ptZero.y;

  // make sure the ellipse for the rounded corners is an odd number of pixels
  LPtoDP(hdc, lppt, 1);
  if(!(lppt->x % 2))
    lppt->x++;
  if(!(lppt->y % 2))
    lppt->y++;
  DPtoLP(hdc, lppt, 1);

  // fix the rectangle to make the MoveToEx and LineTo work correctly
  LPtoDP(hdc, (LPPOINT)&rc, 2);
  rc.right--;
  rc.bottom--;
  DPtoLP(hdc, (LPPOINT)&rc, 2);

  // draw the left top part
  // bottom left partial ellipse
  rcEllipse.left   = lprc->left;
  rcEllipse.top    = lprc->bottom - lppt->y;
  rcEllipse.right  = lprc->left + lppt->x;
  rcEllipse.bottom = lprc->bottom;
  Ellipse3DShadowPoints(&rcEllipse, &ptLB, NULL);
  Arc(hdc, rcEllipse.left, rcEllipse.top, rcEllipse.right, rcEllipse.bottom, lprc->left, lprc->bottom - lppt->y/2, ptLB.x, ptLB.y);

  // left line
  MoveToEx(hdc, lprc->left, lprc->bottom - lppt->y/2 + ptOne.y, NULL);
  LineTo(hdc, lprc->left, lprc->top + lppt->y/2 - ptOne.y);

  // top left ellipse
  Arc(hdc, lprc->left, lprc->top, lprc->left + lppt->x, lprc->top + lppt->y, lprc->left + lppt->x/2, lprc->top, lprc->left, lprc->top + lppt->y/2);

  // top line
  MoveToEx(hdc, lprc->left + lppt->x/2 - ptOne.x, lprc->top, NULL);
  LineTo(hdc, lprc->right - lppt->x/2 + ptOne.x, lprc->top);

  // top right partial ellipse
  rcEllipse.left   = lprc->right - lppt->x;
  rcEllipse.top    = lprc->top;
  rcEllipse.right  = lprc->right;
  rcEllipse.bottom = lprc->top + lppt->y;
  Ellipse3DShadowPoints(&rcEllipse, NULL, &ptRT);
  Arc(hdc, rcEllipse.left, rcEllipse.top, rcEllipse.right, rcEllipse.bottom, ptRT.x, ptRT.y, lprc->right - lppt->x/2, lprc->top);
  DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, clrRB)));

  // draw the bottom right section of the rounded rectangle
  // top right partial ellipse -- same ellipse and end point as before
  Arc(hdc, rcEllipse.left, rcEllipse.top, rcEllipse.right, rcEllipse.bottom, lprc->right, lprc->top + lppt->y/2, ptRT.x, ptRT.y);

  // right line
  MoveToEx(hdc, rc.right, lprc->top + lppt->y/2 - ptOne.y, NULL);
  LineTo(hdc, rc.right, lprc->bottom - lppt->y/2 + ptOne.y);

  // bottom right ellipse
  Arc(hdc, lprc->right - lppt->x, lprc->bottom - lppt->y, lprc->right, lprc->bottom, lprc->right - lppt->x/2, lprc->bottom, lprc->right, lprc->bottom - lppt->y/2);

  // bottom line
  MoveToEx(hdc, lprc->right - lppt->x/2 + ptOne.x, rc.bottom, NULL);
  LineTo(hdc, lprc->left + lppt->x/2 - ptOne.x, rc.bottom);

  // bottom left ellipse -- ptLB as before -- still valid
  rcEllipse.left   = lprc->left;
  rcEllipse.top    = lprc->bottom - lppt->y;
  rcEllipse.right  = lprc->left + lppt->x;
  rcEllipse.bottom = lprc->bottom;
  Arc(hdc, rcEllipse.left, rcEllipse.top, rcEllipse.right, rcEllipse.bottom, ptLB.x, ptLB.y, lprc->left + lppt->x/2, lprc->bottom);

  DeleteObject(SelectObject(hdc, hpenOld));
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  RoundRectFrameTL
//
// DESCR:     This function will draw the top left frame of a
//            rounded rectangle.
//
////////////////////////////////////////////////////////////////

void RoundRectFrameTL
  (
  HDC           hdc,
  const LPRECT  lprc,   // rectangle
  LPPOINT       lppt    // rounded corners
  )

  {
  // this function will draw the RoundRect frame in two sections.
  // The top left section will be drawn in one color and the
  // bottom right will be drawn in a differnt color.
  POINT ptZero        = {0, 0};
  POINT ptOne         = {1, 1};
  RECT  rcEllipse;
  POINT ptLB, ptRT;

  DPtoLP(hdc, &ptZero, 1);
  DPtoLP(hdc, &ptOne, 1);
  ptOne.x -= ptZero.x;
  ptOne.y -= ptZero.y;

  // make sure the ellipse for the rounded corners is an odd number of pixels
  LPtoDP(hdc, lppt, 1);
  if(!(lppt->x % 2))
    lppt->x++;
  if(!(lppt->y % 2))
    lppt->y++;
  DPtoLP(hdc, lppt, 1);

  // draw the left top part
  // bottom left partial ellipse
  rcEllipse.left   = lprc->left;
  rcEllipse.top    = lprc->bottom - lppt->y;
  rcEllipse.right  = lprc->left + lppt->x;
  rcEllipse.bottom = lprc->bottom;
  Ellipse3DShadowPoints(&rcEllipse, &ptLB, NULL);
  Arc(hdc, rcEllipse.left, rcEllipse.top, rcEllipse.right, rcEllipse.bottom, lprc->left, lprc->bottom - lppt->y/2, ptLB.x, ptLB.y);

  // left line
  MoveToEx(hdc, lprc->left, lprc->bottom - lppt->y/2 + ptOne.y, NULL);
  LineTo(hdc, lprc->left, lprc->top + lppt->y/2 - ptOne.y);

  // top left ellipse
  Arc(hdc, lprc->left, lprc->top, lprc->left + lppt->x, lprc->top + lppt->y, lprc->left + lppt->x/2, lprc->top, lprc->left, lprc->top + lppt->y/2);

  // top line
  MoveToEx(hdc, lprc->left + lppt->x/2 - ptOne.x, lprc->top, NULL);
  LineTo(hdc, lprc->right - lppt->x/2 + ptOne.x, lprc->top);

  // top right partial ellipse
  rcEllipse.left   = lprc->right - lppt->x;
  rcEllipse.top    = lprc->top;
  rcEllipse.right  = lprc->right;
  rcEllipse.bottom = lprc->top + lppt->y;
  Ellipse3DShadowPoints(&rcEllipse, NULL, &ptRT);
  Arc(hdc, rcEllipse.left, rcEllipse.top, rcEllipse.right, rcEllipse.bottom, ptRT.x, ptRT.y, lprc->right - lppt->x/2, lprc->top);
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  AccelKeyFromString
//
// DESCR:     This function will retrieve the accelerator key
//            from the specified string.
//
////////////////////////////////////////////////////////////////

BOOL AccelKeyFromString
  (
  LPCSTR  string,
  char*   vKey,
  char*   modifier
  )

  {
  I_ASSERT(vKey && modifier && string);

  BOOL  fRet = FALSE;

  if(vKey && modifier && string)
    {
    int i = 0;
    while(*(string+i) && *(string+i+1))
      {
      if(*(string+i)=='&')
        {
        if(*(string+i+1)=='&')
          i+=2;
        else
          {
          *modifier = OWND_ALT;
          *vKey     = *(string+i+1);
          fRet      = TRUE;
          }
        }
      i++;
      }
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////
