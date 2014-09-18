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
// FILE:          "lay_util.cpp"
//
// AUTHOR:
//
// DESCRIPTION:   utility functions for the layout....
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdlib.h>
#include <windows.h>

#include "objlist.h"

// local function prototypes ///////////////////////////////////////////////

LRESULT CALLBACK _export HwndItemWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     GetPalettePositions
//
//  DESCRIPTION:  This converts a string "%lf, %lf, %hi" to a width, height,
//                and visibility state.
//
////////////////////////////////////////////////////////////////////////////

void GetPalettePositions
  (
  LPSTR   szBuffer,
  int     width,
  int     height,
  LPPOINT lppt,
  BOOL*   lpbShow
  )

  {
  double  x, y;
  char*   szEntry;

  x = strtod(szBuffer, &szEntry);
  y = strtod(++szEntry, &szEntry);
  *lpbShow   = (int)strtol(++szEntry, &szEntry, 0);

  lppt->x = (int)(x * GetSystemMetrics(SM_CXSCREEN)/100.);
  if((lppt->x+width) > GetSystemMetrics(SM_CXSCREEN))
    lppt->x = GetSystemMetrics(SM_CXSCREEN) - width;
  if(lppt->x<0)
    lppt->x = 0;

  lppt->y = (int)(y * GetSystemMetrics(SM_CYSCREEN)/100.);
  if((lppt->y+height) > GetSystemMetrics(SM_CYSCREEN))
    lppt->y = GetSystemMetrics(SM_CYSCREEN) - height;
  if(lppt->y<0)
    lppt->y = 0;
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     InitPaletteMenuString
//
//  DESCRIPTION:  This toggles the Hide and Show string for the palettes.
//
////////////////////////////////////////////////////////////////////////////

void InitPaletteMenuString
  (
  BOOL    fVisible,
  HMENU   hmenu,
  UINT    nMenuCmd,
  LPCSTR  lpcszPalName
  )

  {
  char  szBuffer[80];

  wsprintf(szBuffer, fVisible ? "Hide %s" : "Show %s", lpcszPalName);
  ModifyMenu(hmenu, nMenuCmd, MF_BYCOMMAND | MF_STRING, nMenuCmd, szBuffer);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     SubclassWindow
//
//  DESCRIPTION:  This function will replace the wnd proc of a window with
//                "dead" wnd proc for window layout or the original wnd proc.
//
////////////////////////////////////////////////////////////////////////////

void SubclassWindow
  (
  HWND  hwnd,
  BOOL  bUseNew
  )

  {
  WNDPROC lpfnProc;

  if(bUseNew)
    {
    lpfnProc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
    SetProp(hwnd, "AWloPtr", (HANDLE)LOWORD(lpfnProc));
    SetProp(hwnd, "AWhiPtr", (HANDLE)HIWORD(lpfnProc));
    SetWindowLong(hwnd, GWL_WNDPROC, (long)HwndItemWndProc);
    SetObjectListDragIgnore(hwnd);
    }
  else
    {
    lpfnProc = (WNDPROC)MAKELONG((WORD)GetProp(hwnd, "AWloPtr"), (WORD)GetProp(hwnd, "AWhiPtr"));
    SetWindowLong(hwnd, GWL_WNDPROC, (long)lpfnProc);
    RemoveProp(hwnd, "AWloPtr");
    RemoveProp(hwnd, "AWhiPtr");
    RemoveObjectListDragIgnore(hwnd);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     SubclassAllWindows
//
//  DESCRIPTION:  This function will replace the wnd proc for all child
//                windows for hwnd window items.
//
////////////////////////////////////////////////////////////////////////////

void SubclassAllWindows
  (
  HWND  hwnd,
  BOOL  bUseNew
  )

  {
  HWND  hwndCurrent = hwnd;

  while(hwndCurrent)
    {
    SubclassWindow(hwndCurrent, bUseNew);
    SubclassAllWindows(GetWindow(hwndCurrent, GW_CHILD), bUseNew);

    hwndCurrent = GetWindow(hwndCurrent, GW_HWNDNEXT);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     SubclassHwndItem
//
//  DESCRIPTION:  This function will replace the wnd proc of an hwnd item
//                so the window item is dead to mouse clicks and moves.
//
////////////////////////////////////////////////////////////////////////////

void SubclassHwndItem
  (
  HWND  hwndItem
  )

  {
  SubclassWindow(hwndItem, TRUE);
  SubclassAllWindows(GetWindow(hwndItem, GW_CHILD), TRUE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     UnsubclassHwndItem
//
//  DESCRIPTION:  This function will restores the wnd proc of an hwnd item.
//
////////////////////////////////////////////////////////////////////////////

// UnsubclassHwndItem //////////////////////////////////////////////////////

void UnsubclassHwndItem
  (
  HWND  hwndItem
  )

  {
  SubclassWindow(hwndItem, FALSE);
  SubclassAllWindows(GetWindow(hwndItem, GW_CHILD), FALSE);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     HwndItemWndProc
//
//  DESCRIPTION:  This is the new WndProc for hwnd items in the layout.
//                All mouse messages are reflected to the parent.
//
////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK _export HwndItemWndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  WNDPROC lpfnOld;
  POINT   pt;

  switch(uMsg)
    {
    case WM_MOUSEACTIVATE:
      return SendMessage(GetParent(hwnd), uMsg, (WPARAM)GetParent(hwnd), lParam);

    case WM_SETCURSOR:
      return 0L;


    case WM_NCMOUSEMOVE:
      uMsg = WM_MOUSEMOVE;
      break;

    case WM_NCLBUTTONDBLCLK:
      uMsg = WM_LBUTTONDBLCLK;
      break;

    case WM_NCLBUTTONDOWN:
      uMsg = WM_LBUTTONDOWN;
      break;

    case WM_NCLBUTTONUP:
      uMsg = WM_LBUTTONUP;
      break;

    case WM_NCMBUTTONDBLCLK:
      uMsg = WM_MBUTTONDBLCLK;
      break;

    case WM_NCMBUTTONDOWN:
      uMsg = WM_MBUTTONDOWN;
      break;

    case WM_NCMBUTTONUP:
      uMsg = WM_MBUTTONUP;
      break;

    case WM_NCRBUTTONDBLCLK:
      uMsg = WM_RBUTTONDBLCLK;
      break;

    case WM_NCRBUTTONDOWN:
      uMsg = WM_RBUTTONDOWN;
      break;

    case WM_NCRBUTTONUP:
      uMsg = WM_RBUTTONUP;
      break;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
      pt = MAKEPOINT(lParam);
      MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);
      lParam = MAKELPARAM(pt.x, pt.y);
      return SendMessage(GetParent(hwnd), uMsg, wParam, lParam);

    default:
      lpfnOld = (WNDPROC)MAKELONG((WORD)GetProp(hwnd, "AWloPtr"), (WORD)GetProp(hwnd, "AWhiPtr"));
      return CallWindowProc(lpfnOld, hwnd, uMsg, wParam, lParam);
    }

  pt = MAKEPOINT(lParam);
  MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), &pt, 1);
  lParam = MAKELPARAM(pt.x, pt.y);
  wParam = 0;
  if(GetKeyState(VK_CONTROL)<0)
    wParam |= MK_CONTROL;
  if(GetKeyState(VK_LBUTTON)<0)
    wParam |= VK_LBUTTON;
  if(GetKeyState(VK_MBUTTON)<0)
    wParam |= VK_MBUTTON;
  if(GetKeyState(VK_RBUTTON)<0)
    wParam |= VK_RBUTTON;
  if(GetKeyState(VK_SHIFT)<0)
    wParam |= MK_SHIFT;
  return SendMessage(GetParent(hwnd), uMsg, wParam, lParam);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     MakeRealRect
//
//  DESCRIPTION:  This converts a rectangle so the left and top are smaller
//                than the right and bottom.
//
////////////////////////////////////////////////////////////////////////////

void MakeRealRect
  (
  LPRECT  lprcDst,
  LPRECT  lprcSrc
  )

  {
  int left   = (lprcSrc->left > lprcSrc->right)  ? lprcSrc->right  : lprcSrc->left;
  int top    = (lprcSrc->top  > lprcSrc->bottom) ? lprcSrc->bottom : lprcSrc->top;
  int right  = (lprcSrc->left > lprcSrc->right)  ? lprcSrc->left   : lprcSrc->right;
  int bottom = (lprcSrc->top  > lprcSrc->bottom) ? lprcSrc->top    : lprcSrc->bottom;

  SetRect(lprcDst, left, top, right, bottom);
  }

////////////////////////////////////////////////////////////////////////////
