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

LRESULT CALLBACK _export AWLayoutWndProc
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
        PostMessage(hwnd, WM_COMMAND, IDM_EXIT, 0L);
        fHandled = TRUE;
        lRet = 0L;
        break;
        }
      break;

    case WM_QUERYENDSESSION:
      MessageBeep(MB_ICONEXCLAMATION);
      BWCCMessageBox(hwnd, "Window layout is still active.\r\n\r\nQuit the window layout before quitting Windows.", "Window Layout", MB_TASKMODAL | MB_OK | MB_ICONEXCLAMATION);
      lRet = 0;
      fHandled = TRUE;
      break;

    case WM_KEYDOWN:
      (pLayout->m_shapes).KeyDown(pLayout->m_pWndActive, hwnd, wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYUP:
      (pLayout->m_shapes).KeyUp(pLayout->m_pWndActive, hwnd, wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_INITMENUPOPUP:
      pLayout->InitMenuPopup((HMENU)wParam, (int)LOWORD(lParam), (BOOL)HIWORD(lParam));
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
    case WM_VSCROLL:
      pLayout->Scroll(uMsg, wParam, (int)LOWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_SIZE:
      pLayout->UpdateScrollRegion();
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_COMMAND:
      switch(wParam)
        {
        // file menu
        case IDM_UPDATE:
          (pLayout->m_pWndActive)->Update();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_REVERT:
          (pLayout->m_pWndActive)->Revert();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_EXIT:
          pLayout->Exit();
          fHandled = TRUE;
          lRet = 0L;
          break;

        // edit menu
        case IDM_UNDO:
        case IDM_REDO:
        case IDM_CUT:
        case IDM_COPY:
        case IDM_PASTE:
          break;

        case IDM_DELETE:
          (pLayout->m_pWndActive->Items())->DeleteSelItems();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_DUPLICATE:
          break;

        case IDM_WNDATTRIBUTES:
          (pLayout->m_pWndActive)->EditAttributes();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_WNDBACKGROUND:
          (pLayout->m_pWndActive)->EditBackground();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_WNDSIZES:
          (pLayout->m_pWndActive)->EditSize();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_LAYPREFERENCES:
          (pLayout->m_pWndActive)->EditPreferences();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_DELETEOBJECT:
          (pLayout->m_pWndActive->Items())->DeleteSelObjects();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_OBJECTNAME:
          (pLayout->m_pWndActive->Items())->RenameSelectedItem();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_EDITITEM:
          (pLayout->m_pWndActive->Items())->EditItem(NULL, TRUE);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_SELECTALL:
          (pLayout->m_pWndActive->Items())->SelectAll(TRUE);
          fHandled = TRUE;
          lRet = 0L;
          break;


        // position menu
        case IDM_ALIGNLEFT:
          (pLayout->m_pWndActive->Items())->AlignLeft();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_ALIGNTOP:                              
          (pLayout->m_pWndActive->Items())->AlignTop();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_ALIGNRIGHT:                    
          (pLayout->m_pWndActive->Items())->AlignRight();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_ALIGNBOTTOM:
          (pLayout->m_pWndActive->Items())->AlignBottom();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_CENTERHORZINWND:
          (pLayout->m_pWndActive->Items())->CenterHorzInWnd();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_CENTERVERTINWND:
          (pLayout->m_pWndActive->Items())->CenterVertInWnd();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_CENTERHORZ:
          (pLayout->m_pWndActive->Items())->CenterHorz();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_CENTERVERT:
          (pLayout->m_pWndActive->Items())->CenterVert();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_SPACEHORZ:
          (pLayout->m_pWndActive->Items())->SpaceHorz();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_SPACEVERT:
          (pLayout->m_pWndActive->Items())->SpaceVert();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_GRID:
          (pLayout->m_pWndActive)->EditGrid();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_SIZE:
          (pLayout->m_pWndActive->Items())->PosAndSize();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_ATTACH:
          (pLayout->m_pWndActive->Items())->Attach(NULL);
          fHandled = TRUE;
          lRet = 0L;
          break;

        // arrange menu
        case IDM_SENDTOBACK:
          (pLayout->m_pWndActive->Items())->SendToBack();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_BRINGTOFRONT:
          (pLayout->m_pWndActive->Items())->BringToFront();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_GROUP:
          (pLayout->m_pWndActive->Items())->Group();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_UNGROUP:
          (pLayout->m_pWndActive->Items())->Ungroup();
          fHandled = TRUE;
          lRet = 0L;
          break;

        // tool menu
        case IDM_FULLINFO:
          (pLayout->m_pWndActive)->ToggleFullInfo();
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_SELECTION:
          pLayout->m_shapes.ChangeTool(NULL, OSHP_STDTOOL_ARROW);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_REORDER:
          pLayout->m_shapes.ChangeTool(NULL, OSHP_STDTOOL_REORDER);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_TABSTOPS:
          pLayout->m_shapes.ChangeTool(NULL, OSHP_STDTOOL_TABSTOP);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_GROUPSTOPS:
          pLayout->m_shapes.ChangeTool(NULL, OSHP_STDTOOL_GROUPSTOP);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_VISIBILITY:
          pLayout->m_shapes.ChangeTool(NULL, OSHP_STDTOOL_VISIBILITY);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_LOCK:
          pLayout->m_shapes.ChangeTool(NULL, OSHP_STDTOOL_LOCK);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_ANCHOR:
          pLayout->m_shapes.ChangeTool(NULL, OSHP_STDTOOL_ANCHOR);
          fHandled = TRUE;
          lRet = 0L;
          break;

        // palettes menu
        case IDM_HIDEALL:
        case IDM_SHOWALL:
          pLayout->ShowAllPalettes(wParam==IDM_SHOWALL);
          fHandled = TRUE;
          lRet = 0L;
          break;

        case IDM_ALIGNMENT:
        case IDM_NAME:
        case IDM_OBJECTLIST:
        case IDM_SIZES:
          pLayout->TogglePaletteVisibility(wParam);
          fHandled = TRUE;
          lRet = 0L;
          break;

        // help menu
        case IDM_HELPINDEX:
        case IDM_HELPUSINGLAYOUT:
        case IDM_HELPABOUT:
          break;

        // user palatte
        default:
          pLayout->m_shapes.MenuPicked(wParam);
          fHandled = TRUE;
          lRet = 0L;
          break;
        }
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

LRESULT CALLBACK _export AWFrameWndProc
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
      pWnd->Size(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
      fHandled = FALSE;
      break;

    case WM_MOVE:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      I_ASSERT(pWnd);
      pWnd->Move((int)LOWORD(lParam), (int)HIWORD(lParam));
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
      pWnd->GetLayout()->Shapes()->KeyDown(pWnd, pWnd->ClientWnd(), wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYUP:
      pWnd = (Window*)GetWindowLong(hwnd, I_GWL_WINDOW);
      I_ASSERT(pWnd);
      pWnd->GetLayout()->Shapes()->KeyUp(pWnd, pWnd->ClientWnd(), wParam, lParam);
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

LRESULT CALLBACK _export AWClientWndProc
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
      pShapes->MouseMove(pWnd, hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_LBUTTONDOWN:
      pShapes->MouseLButtonDown(pWnd, hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_LBUTTONDBLCLK:
      pShapes->MouseLButtonDblClk(pWnd, hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_LBUTTONUP:
      pShapes->MouseLButtonUp(pWnd, hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_RBUTTONDOWN:
      pShapes->MouseRButtonDown(pWnd, hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYDOWN:
      pShapes->KeyDown(pWnd, hwnd, wParam, lParam);
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_KEYUP:
      pShapes->KeyUp(pWnd, hwnd, wParam, lParam);
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
      (pWnd->Items())->HScroll(wParam, LOWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_VSCROLL:
      (pWnd->Items())->VScroll(wParam, LOWORD(lParam));
      fHandled = TRUE;
      lRet = 0L;
      break;

    case WM_DELETEITEM:
    case WM_COMPAREITEM:
    case WM_MEASUREITEM:
    case WM_DRAWITEM:
    case WM_CTLCOLOR:
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

LRESULT CALLBACK _export AWMenuWndProc
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
      pWnd->Move(0, 0);
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
