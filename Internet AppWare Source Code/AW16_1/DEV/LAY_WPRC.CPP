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
// FILE:          "lay_wprc.cpp"
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

#include <windows.h>
#include <windowsx.h>
#include <bwcc.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_wndshp.h>

#include "wlay.h"
#include "lay_shap.h"
#include "lay_wnd.h"
#include "layout.h"
#include "rc\wlay_rsc.h"
#include "menudata.h"

// externs /////////////////////////////////////////////////////////////////

extern HINSTANCE    g_hInstance;
extern Window*      g_pWnd;

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     AWLayoutWndProc
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK EXPORT AWLayoutWndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL    fHandled = FALSE;
  LRESULT lRet;
  Layout* pLayout = (Layout*)GetWindowLong(hwnd, I_GWL_LAYOUT);

  switch(uMsg)
    {
    case WM_CREATE:
      pLayout = (Layout*)(((CREATESTRUCT*)lParam)->lpCreateParams);
      SetWindowLong(hwnd, I_GWL_LAYOUT, (long)pLayout);
      pLayout->m_hwndLayout = hwnd;
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_SYSCOMMAND:
      if((wParam & 0xFFF0)==SC_CLOSE)
        {
        FORWARD_WM_COMMAND(hwnd, IDM_EXIT, 0, 0, PostMessage);
        fHandled = TRUE;
        lRet = 0L;
        break;
        }
      break;

    case WM_QUERYENDSESSION:
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(hwnd, "Window layout is still active.\r\n\r\nQuit the window layout before quitting Windows.", "Window Layout", MB_TASKMODAL | MB_OK | MB_ICONEXCLAMATION);
      lRet = 0;
      fHandled = TRUE;
      break;

    case WM_KEYDOWN:
      (pLayout->m_shapes).KeyDown(pLayout->m_pWndActive, hwnd, (int)wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYUP:
      (pLayout->m_shapes).KeyUp(pLayout->m_pWndActive, hwnd, (int)wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_INITMENUPOPUP:
      HANDLE_WM_INITMENUPOPUP(hwnd, wParam, lParam, pLayout->InitMenuPopup);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_PALETTECHANGED:
      if ((pLayout->ActiveWindow()) && ((HWND)wParam==pLayout->ActiveWindow()->ClientWnd()))
        break;
      // else fall through
    case WM_QUERYNEWPALETTE:
      if(!IsIconic(hwnd) && pLayout->ActiveWindow() && WndData::GetPaletteHandle())
        {
        HDC       hdc     = GetDC(pLayout->ActiveWindow()->ClientWnd());
        HPALETTE  hpalOld = SelectPalette(hdc, WndData::GetPaletteHandle(), FALSE);
        lRet = RealizePalette(hdc);         
        SelectPalette(hdc, hpalOld, TRUE);
        RealizePalette(hdc);
        ReleaseDC(pLayout->ActiveWindow()->ClientWnd(), hdc);

        if(lRet)                            
          InvalidateRect(pLayout->ActiveWindow()->ClientWnd(), NULL, TRUE);
        fHandled = TRUE;
        }
      break;

    case WM_HSCROLL:
      HANDLE_WM_HSCROLL(hwnd, wParam, lParam, pLayout->HScroll);
      fHandled = TRUE;
      lRet = 0;
      break;

    case WM_VSCROLL:
      HANDLE_WM_VSCROLL(hwnd, wParam, lParam, pLayout->VScroll);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_SIZE:
      pLayout->UpdateScrollRegion();
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_COMMAND:
      HANDLE_WM_COMMAND(hwnd, wParam, lParam, pLayout->Command);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_NCACTIVATE:
      if(!IsIconic(hwnd) && pLayout->m_pWndActive && pLayout->m_pWndActive->WndHandle())
        SendMessage(pLayout->m_pWndActive->WndHandle(), uMsg, wParam, lParam);
      break;

    default:
      break;
    }

  if(!fHandled)
    lRet = DefWindowProc(hwnd, uMsg, wParam, lParam);

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     AWFrameWndProc
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK EXPORT AWFrameWndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  LRESULT lRet;
  BOOL    fHandled;
  Window* pWnd;

  switch (uMsg)
    {
    case WM_CREATE:
      pWnd = (Window*)(((CREATESTRUCT*)lParam)->lpCreateParams);
      SetWindowLong(hwnd, I_GWL_WINDOW, (long)pWnd);
      pWnd->FrameWndCreated(hwnd);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_SIZE:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      I_ASSERT(pWnd);
      HANDLE_WM_SIZE(hwnd, wParam, lParam, pWnd->Size);
      fHandled = FALSE;
      break;

    case WM_MOVE:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      I_ASSERT(pWnd);
      HANDLE_WM_MOVE(hwnd, wParam, lParam, pWnd->Move);
      fHandled = FALSE;
      break;

    case WM_GETMINMAXINFO:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      if(!pWnd) // this comes before WM_CREATE ... thanks MicroSUCKS
        pWnd = g_pWnd;
      I_ASSERT(pWnd);
      pWnd->GetMinMaxInfo((MINMAXINFO*)lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYDOWN:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      I_ASSERT(pWnd);
      pWnd->GetLayout()->Shapes()->KeyDown(pWnd, pWnd->ClientWnd(), (int)wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYUP:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      I_ASSERT(pWnd);
      pWnd->GetLayout()->Shapes()->KeyUp(pWnd, pWnd->ClientWnd(), (int)wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_INITMENUPOPUP:
      I_ASSERT(GetSystemMenu(hwnd, FALSE) == (HMENU)wParam);
      EnableMenuItem((HMENU)wParam, 3, MF_BYPOSITION | MF_GRAYED);
      EnableMenuItem((HMENU)wParam, 4, MF_BYPOSITION | MF_GRAYED);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_SYSCOMMAND:
      switch(wParam & 0xFFF0)
        {
        case SC_MAXIMIZE:
        case SC_MINIMIZE:
          fHandled = TRUE;
          lRet = 0L;
          break;

        default:
          fHandled = FALSE;
          break;
        }
      break;

    default:
      fHandled = FALSE;
      break;
    }

  return fHandled ? lRet : DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     AWClientWndProc
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK EXPORT AWClientWndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  LRESULT lRet;
  BOOL    fHandled;
  Window* pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
  Shapes* pShapes = (Shapes*)GetWindowLong(hwnd, I_GWL_SHAPES);

  switch (uMsg)
    {
    case WM_CREATE:
      pWnd = (Window*)(((CREATESTRUCT*)lParam)->lpCreateParams);
      pShapes = (pWnd->GetLayout())->Shapes();
      SetWindowLong(hwnd, I_GWL_WINDOW, (long)pWnd);
      SetWindowLong(hwnd, I_GWL_SHAPES, (long)pShapes);
      fHandled = TRUE;
      lRet = 0L;
      break;

    // the following messages are processed by the Shapes class
    case WM_MOUSEMOVE:
      pShapes->MouseMove(pWnd, hwnd, (int)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_LBUTTONDOWN:
      pShapes->MouseLButtonDown(pWnd, hwnd, (int)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_LBUTTONDBLCLK:
      pShapes->MouseLButtonDblClk(pWnd, hwnd, (int)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_LBUTTONUP:
      pShapes->MouseLButtonUp(pWnd, hwnd, (int)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_RBUTTONDOWN:
      pShapes->MouseRButtonDown(pWnd, hwnd, (int)wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYDOWN:
      pShapes->KeyDown(pWnd, hwnd, (int)wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYUP:
      pShapes->KeyUp(pWnd, hwnd, (int)wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;


    // the following messages are handled by the Window class
    case WM_ERASEBKGND:
      pWnd->EraseBackground((HDC)wParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_PAINT:
      (pWnd->Items())->Paint();
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_HSCROLL:
      HANDLE_WM_HSCROLL(hwnd, wParam, lParam, (pWnd->Items())->HScroll);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_VSCROLL:
      HANDLE_WM_VSCROLL(hwnd, wParam, lParam, (pWnd->Items())->VScroll);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_DELETEITEM:
    case WM_COMPAREITEM:
    case WM_MEASUREITEM:
    case WM_DRAWITEM:
    #if defined(__FLAT__)
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    #else
    case WM_CTLCOLOR:
    #endif
      lRet = (pWnd->Items())->ParentNotified(hwnd, uMsg, wParam, lParam);
      fHandled = TRUE;
      break;

    default:
      fHandled = FALSE;
      break;
    }

  return fHandled ? lRet : DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     AWMenuWndProc
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK EXPORT AWMenuWndProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  LRESULT           lRet;
  BOOL              fHandled;
  Window*           pWnd;
  MenuData*         pMenuData;

  switch (uMsg)
    {
    case WM_CREATE:
      pWnd = (Window*)(((CREATESTRUCT*)lParam)->lpCreateParams);
      SetWindowLong(hwnd, I_GWL_WINDOW, (long)pWnd);
      pMenuData = new MenuData(pWnd->MenuID());
      SetWindowLong(hwnd, I_GWL_MENUDATA, (long)pMenuData);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_DESTROY:
      pMenuData = (MenuData*)GetWindowLong(hwnd, I_GWL_MENUDATA);
      delete pMenuData;
      SetWindowLong(hwnd, I_GWL_MENUDATA, (long)NULL);
      break;

    case WM_PAINT:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      pWnd->PaintMenuWindow(hwnd);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_LBUTTONDBLCLK:
      pMenuData = (MenuData*)GetWindowLong(hwnd, I_GWL_MENUDATA);
      pMenuData->Edit();
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      pWnd->Move(hwnd, 0, 0);
      InvalidateRect(hwnd, NULL, TRUE);
      fHandled = TRUE;
      lRet = 0L;
      break;

    default:
      fHandled = FALSE;
      break;
    }

  return fHandled ? lRet : DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

////////////////////////////////////////////////////////////////////////////
