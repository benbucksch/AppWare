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
// FILE:  "tbl_main.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#define   STRICT
#define   ASTRICT

#include  <windows.h>

#include  <a_alm.h>
#include  <op_wnd.h>
#include  <o_wnd.h>

#include  "tabl_hdr.h"
#include  "tabl_rsc.h"
#include  "table.h"

// Declare Global variables ////////////////////////////////////////////////

GLOBAL      G;
extern int  _WinAllocFlag;

// GLOBAL -- initialization ////////////////////////////////////////////////

char* GLOBAL::CRLF = "\r\n";

// Tabl_WndProc ////////////////////////////////////////////////////////////

long far pascal _export Tabl_WndProc(
  HWND    hWnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam)

  {
  TABLE*  table;

  switch(uMsg)
    {
    case WM_CREATE:  // stuff handle into m_hwndParent
      {
      CREATESTRUCT *cs  = (CREATESTRUCT *)lParam;
      table = (TABLE *)cs->lpCreateParams;
      table->m_hwndItem = hWnd;
      }
      break;

    case WM_VSCROLL:
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->VScroll(wParam, lParam);
      return 0L;

    case WM_HSCROLL:
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->HScroll(wParam, lParam);
      return 0L;

    case WM_PAINT:
      {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->Tabl_windItemDraw(ps.hdc, &ps.rcPaint);
      EndPaint(hWnd, &ps);
      return 0L;
      }

    /* The following msgs have been added for new impelmentation of APPWARE.
    These messages are no longer part of the WIP protocol.  As a result all
    hwnd Window Item (non-lite) have to handle these events in their callbacks */
    case EDITM_KEYDOWN: // posted by the edit control
    case WM_KEYDOWN:
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->Tabl_windItemKey(wParam, lParam);
      break;

    case WM_MOUSEMOVE:
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->Tabl_windItemMouseMove(wParam, lParam);
      break;
    case WM_LBUTTONUP:
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->Tabl_windMouseUp(wParam, lParam);
      break;
    case WM_LBUTTONDOWN:
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->Tabl_windItemHit(wParam, lParam);
      break;
    case WM_LBUTTONDBLCLK: {
      table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
      table->Tabl_windItemDoubleClick(wParam, lParam);
      break;
      }
    }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

// LibMain -- DLL entry point //////////////////////////////////////////////
//    this will Initialize some global variables.

int far pascal _export LibMain (
  HINSTANCE hInstance,
  WORD      /*wDataSeg*/,
  WORD      wHeapSize,
  LPSTR     /*lpszCmdLine*/)

  {
  // initialize the necessary global variables
  G.hInstance = hInstance;
  G.cbByteOffset = DWL_USER;
  G.hResizeCursor = LoadCursor(G.hInstance, MAKEINTRESOURCE(RESIZECURSOR));
  G.hBitmap = LoadBitmap(G.hInstance, MAKEINTRESOURCE(BORDERBITMAP));

  WNDCLASS  wc;
  wc.style          = CS_DBLCLKS;
  wc.lpfnWndProc    = (WNDPROC)Tabl_WndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hInstance      = G.hInstance;
  wc.hIcon          = NULL;
  wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground  = NULL;
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = "SeriusTable";
  RegisterClass(&wc);

  // use this magical line to make the 'new' operator work!?
  _WinAllocFlag = GMEM_SHARE;

  if(wHeapSize>0)
    UnlockData(0);

  return 1; // the dll was initialized properly
  }

// WEP -- DLL exit point ///////////////////////////////////////////////////

int far pascal _export WEP(
  int /*nParameter*/)

  {
  DestroyCursor(G.hResizeCursor);
  DeleteObject(G.hBitmap);
  UnregisterClass("SeriusTable", G.hInstance);
  return 1;
  }

////////////////////////////////////////////////////////////////////////////
