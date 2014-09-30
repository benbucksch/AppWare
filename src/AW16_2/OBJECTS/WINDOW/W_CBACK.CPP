//////////////////////////////////////////
/// This file contains the window objects 
///  callback function
//////////////////////////////////////////

#include "w_includ.h"


LRESULT WINDOWRUNTIME::WmCreate(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  WINDOW*                 wnd;
  OBJECTID                oiWindow;
  LPCREATESTRUCT          cr;

  cr = (LPCREATESTRUCT)lParam;
  oiWindow = (OBJECTID)cr->lpCreateParams;

  StoreIDinExtraBytes(hwnd, oiWindow);

  wnd = LockWindowData(oiWindow);
  I_VERIFY_POINTER(wnd, return -1);

  wnd->SetWindowHandle(hwnd);
  wnd->SetEraseRegion();
  wnd->SetHScrollVisible(wnd->HasHorzScroll());
  wnd->SetVScrollVisible(wnd->HasVertScroll());

  UnlockWindowData(oiWindow);

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

/////////////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmActivate(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  HTASK                   activeTask, currentTask;

  if (IsAppActive())
    {//App is active and activate a non-window object window (eg dialog box)
    if (!IsHwndAWindowObject(hwnd))
      return DefWindowProc(hwnd, message, wParam, lParam);
    }

  oiWindow = GetIDfromExtraBytes(hwnd);
  wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return 0);

  if (wParam)
    { // It's going active!
    m_oiModal = wnd->IsModal() ? oiWindow : 0;
    m_oiActive = oiWindow;

    wnd->ActivateItemCauseWindowWas();

    AEvtPostSignalAtTail(oiWindow, I_SIG_SELECTED);
    }
  else
    {// It's going inactive!
    currentTask = GetCurrentTask();
    activeTask = GetWindowTask((HWND)LOWORD(lParam));

    if (activeTask == currentTask  &&  m_oiModal == oiWindow)
			m_oiModal = 0;

		m_oiActive = 0;

		if (m_wiCapture  &&  WINDOWAPI::IsLiteItem(m_wiCapture))
			{
			((OpLiteWndItemD*)m_wiCapture)->CancelMode();
			OWndReleaseCapture();
			}
		else if (m_wiCapture  &&  WINDOWAPI::IsHwndItem(m_wiCapture))
			{
			SendMessage( ((OpHwndWndItemD*)m_wiCapture)->m_hwndItem, WM_CANCELMODE, 0, 0L);
			OWndReleaseCapture();
			}

    wnd->DeactivateItemCauseWindowWas();
		}

  UnlockWindowData(oiWindow);
  return 0;
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmNcButton(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;

  oiWindow = GetIDfromExtraBytes(hwnd);

  if (m_oiModal && m_oiModal != oiWindow)
    return FALSE;  //Trying to activate the non-modal window
  else
    return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmMouseActivate(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;

  I_UNUSED(hwnd);I_UNUSED(message);I_UNUSED(wParam);I_UNUSED(lParam);

  oiWindow = GetIDfromExtraBytes(hwnd);

  if (m_oiModal && m_oiModal != oiWindow) //Clicked on a window other than the modal window
    {
    if (!m_bAppIsActive)  //trying to re-activate the app.  clicked on other than the modal
      ActivateWindow(m_oiModal);
    else
      MessageBeep(0);

    return MA_NOACTIVATEANDEAT;
    }
  else
    return MA_ACTIVATE;
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmSysCommand(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    switch(wParam)
      {
      case SC_CLOSE:
        AEvtPostSignalAtMark(oiWindow, I_SIG_ABOUTTOCLOSE);
        break;

      case SC_RESTORE:
      case SC_MAXIMIZE:
        wnd = LockWindowData(oiWindow);
        I_VERIFY_POINTER(wnd, break);
        if (wnd->WindowIsIconic())
          {
          AEvtPostSignalAtTail(oiWindow, I_SIG_ICONRESTORED);
          wnd->SetWindowIsIconic(FALSE);
          wnd->RestorePortOrigins();
          }
        UnlockWindowData(oiWindow);
        break;

      case SC_MINIMIZE:
        wnd = LockWindowData(oiWindow);
        I_VERIFY_POINTER(wnd, break);
        AEvtPostSignalAtTail(oiWindow, I_SIG_ICONIFIED);
        wnd->SetWindowIsIconic(TRUE);
        UnlockWindowData(oiWindow);
        break;

      }

    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmClose(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;

  I_UNUSED(hwnd);I_UNUSED(message);I_UNUSED(wParam);I_UNUSED(lParam);

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    AEvtPostStandard(oiWindow, OWND_EVT_CLOSEWINDOWOBJECT, (long)oiWindow, AEVT_ATMARK);

   return 0;
   }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmGetMinMaxInfo(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  LPPOINT                 ptPtr = (LPPOINT)lParam;
  POINT                   pt;
  RECT                    rect;

  if (!ptPtr)
    return DefWindowProc(hwnd, message, wParam, lParam);

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam) );

    ptPtr[3] = wnd->GetMinSize();
    wnd->WndLStoDS(&ptPtr[3], 1);
                                    
    if (wnd->HasMaxSize())
      {
      ptPtr[4] = wnd->GetMaxSize();
      wnd->WndLStoDS(&ptPtr[4], 1);
      }
    else 
      {
      POINT   ptScrollSize;
      DWORD   dwStyle           = wnd->GetNativeStyle();
      int     nHeight, nWidth;

      CopyRect(&rect, &(wnd->GetInitialClientRect()) );
      wnd->WndLStoDS((LPPOINT)&rect, 2); //Convert to device coords

      ptScrollSize = wnd->GetScrollSize();
      wnd->WndLStoDS(&ptScrollSize, 1);  //Convert to device coords

      // adjust the size for the scroll bars
      nHeight = rect.bottom - rect.top;
      nWidth  = rect.right - rect.left;
      if((dwStyle & WS_VSCROLL) && (nHeight >= ptScrollSize.y))
        rect.right -= (GetSystemMetrics(SM_CXVSCROLL) - 1);
      if((dwStyle & WS_HSCROLL) && (nWidth >= ptScrollSize.x))
        rect.bottom -= (GetSystemMetrics(SM_CYHSCROLL) - 1);

      OWndLayAdjWindowRectEx(&rect, dwStyle, wnd->HasMenuBar() ? wnd->GetMenuObject() : NULL, wnd->GetNativeExStyle());

      pt.y = rect.bottom - rect.top;
      pt.x = rect.right - rect.left;

      ptPtr[4].y = max(pt.y, ptPtr[4].y);
      ptPtr[4].x = max(pt.x, ptPtr[4].x);
      }

    UnlockWindowData(oiWindow);
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmSetCursor(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  HWND                    hWndWindow;

  I_UNUSED(hwnd);I_UNUSED(message);I_UNUSED(wParam);I_UNUSED(lParam);

  if ( LOWORD(lParam) == HTCLIENT)
    {
    oiWindow = GetIDfromExtraBytes(hwnd);
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam) );
      hWndWindow= wnd->GetWindowHandle();
      UnlockWindowData(oiWindow);

      if (!m_bArrowCursor || (HWND)wParam == hWndWindow)
        {
        ::SetCursor(m_hCursor);
        return TRUE;
        }
      }
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }


//////////////////////////////////////////////////////


LRESULT WINDOWRUNTIME::WmMove(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  RECT                    rect, beforeMoveRect;
  OpWndItemD*             wi;


  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
    
    if (!wnd->WindowIsOpening() && !wnd->WindowIsClosing() && !IsIconic(hwnd))
      {
      GetWindowRect(hwnd, &rect);
      beforeMoveRect = wnd->GetCurrentWindowRect();

      if ((rect.top != beforeMoveRect.top) ||(rect.left != beforeMoveRect.left))
        {
				AEvtPostSignalAtTail(oiWindow, I_SIG_MOVED);
        wi=NULL;
        while ( (wi = wnd->GetNextItem(wi, FALSE)) != NULL)
          wi->ScreenLocationChanged(OWND_LOC_MOVED);
        }

      wnd->SetCurrentWindowRect(&rect);
      }
    UnlockWindowData(oiWindow);
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmSize(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  RECT                    rect, beforeSizeRect;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

    //A WM_SIZE is sent when the window is being destroyed.  We can ignore this one.
    if (!wnd->WindowIsClosing() && !IsIconic(hwnd) )
      {
      GetClientRect(hwnd, &rect);
      wnd->WndDPtoLP((LPPOINT)&rect, 2);
      rect.bottom -= rect.top;
      rect.right -= rect.left;
      rect.top = rect.left = 0;
      
      if (!wnd->WindowIsOpening())
        {
        beforeSizeRect = wnd->GetCurrentClientRect();

        if ((rect.bottom - rect.top != beforeSizeRect.bottom - beforeSizeRect.top)
            ||(rect.right - rect.left != beforeSizeRect.right - beforeSizeRect.left) )
            {
            AEvtPostSignalAtTail(oiWindow, I_SIG_RESIZED);

            wnd->ResizeWindowItems(&rect); //Tmp rect is passed--it is compared with wo->clientRect
            }
        }

      wnd->SetCurrentClientRect(&rect);
      wnd->ValidateScrollBars();//Now that clientRect is updated, we can validate the scroll bars

      GetWindowRect(hwnd, &rect);
      wnd->SetCurrentWindowRect(&rect);
      }

    UnlockWindowData(oiWindow);
    }

  return DefWindowProc(hwnd, message, wParam, lParam);

  }
//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmEraseBkgnd(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  LPARAM                  rVal;

  // wo->Paint will take care of the background for icons...
  if (IsIconic(hwnd))
    return TRUE;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

    if (wnd->Erase((HDC)wParam))
      rVal = TRUE;
    else
      rVal = DefWindowProc(hwnd, message, wParam, lParam);
    UnlockWindowData(oiWindow);
    }
  else 
    rVal = DefWindowProc(hwnd, message, wParam, lParam);

  return rVal;
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmPaint(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  LPARAM                  rVal;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

    if (IsIconic(hwnd))
      wnd->PaintIcon();
    else
      wnd->Paint();
    rVal = 0;
    UnlockWindowData(oiWindow);
    }
  else 
    rVal = DefWindowProc(hwnd, message, wParam, lParam);

  return rVal;

  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmQueryNewPalette(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
	HDC											hdc;
	HPALETTE								hPalOld;
  LPARAM                  rVal=0L;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

		if (wnd->GetPaletteHandle())
			{
			hdc = GetDC(hwnd);
	    hPalOld = SelectPalette(hdc, wnd->GetPaletteHandle(), FALSE);
	    rVal = RealizePalette(hdc);         
	    SelectPalette(hdc, hPalOld, TRUE);
	    RealizePalette(hdc);
	    ReleaseDC(hwnd, hdc);

	    if (rVal)                            
	    	InvalidateRect(hwnd, NULL, TRUE);
			}
		else
			rVal = DefWindowProc(hwnd, message, wParam, lParam);	
    
    UnlockWindowData(oiWindow); 
		}
	else 
		rVal = DefWindowProc(hwnd, message, wParam, lParam);	
	
	return rVal;
	}

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmMenuChar(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;

  // Give up if we are already in the Menu Manager
  if (LOWORD(lParam))
    return DefWindowProc(hwnd, message, wParam, lParam);

  // There is no menu action going on, see if it's one of our
  // hot keys.

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    if ( CheckForAccelKey(wParam, oiWindow) )
      return 0x00010000L;  // Return 1 in hiword so Windows won't beep
    else
      return 0L;
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmCommand(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OpHwndWndItemD*         hwi;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  MENUSPARAMBLOCK         menuBlock;
  LRESULT                 rVal=0L;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

    if ( LOWORD(lParam))
      {
      hwi = wnd->GetItemByControlID(wParam);
      if (hwi) //we know what control it is for
        rVal = hwi->ParentNotification(hwnd, message, wParam, lParam);
      }
    else
      {
      menuBlock.objID = 0;
      menuBlock.hMenu = 0;
      menuBlock.itemID = wParam;
      AEvtCallMethod(oiWindow, 0, OTYPE_MENU, OEVENT_HANDLECOMMAND, (LONG)&menuBlock);
      rVal=0L;
      }

    UnlockWindowData(oiWindow);
    }

  return rVal;
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmMenuSelect(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  MENUSPARAMBLOCK         menuBlock;
  LRESULT                 rVal=0L;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

		wnd->SetMenuDown(LOWORD(lParam) != 0xFFFF);	 //means the menu got closed

    menuBlock.objID = wnd->GetMenuObject();
    menuBlock.hMenu = 0;
    menuBlock.itemID = wParam;
    AEvtCallMethod(oiWindow, wnd->GetMenuObject(), 0, OEVENT_HANDLESELECT, (LONG)&menuBlock);
    rVal=0L;

    UnlockWindowData(oiWindow);
    }

  return rVal;
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmVScroll(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  LRESULT                 rVal = 0L;
  OpHwndWndItemD*         hwi;
  int                     controlID;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

    if (HIWORD(lParam)) //Is it for a item
      {  
      controlID = GetWindowWord((HWND)HIWORD(lParam), GWW_ID);
      hwi = wnd->GetItemByControlID(controlID);
      if (hwi) //we know what control it is for
        rVal = hwi->ParentNotification(hwnd, message, wParam, lParam);
      else
        rVal = DefWindowProc(hwnd, message, wParam, lParam);
      }
    else //It is for the window
      wnd->DoWindowVScroll(wParam, lParam);

    UnlockWindowData(oiWindow);
    }
  else
    rVal = DefWindowProc(hwnd, message, wParam, lParam);

  return rVal;
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmHScroll(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  LRESULT                 rVal = 0L;
  OpHwndWndItemD*         hwi;
  int                     controlID;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));

    if (HIWORD(lParam)) //Is it for a item
      {  
      controlID = GetWindowWord((HWND)HIWORD(lParam), GWW_ID);
      hwi = wnd->GetItemByControlID(controlID);
      if (hwi) //we know what control it is for
        rVal = hwi->ParentNotification(hwnd, message, wParam, lParam);
      else
        rVal = DefWindowProc(hwnd, message, wParam, lParam);
      }
    else //It is for the window
      wnd->DoWindowHScroll(wParam, lParam);

    UnlockWindowData(oiWindow);
    }
  else
    rVal = DefWindowProc(hwnd, message, wParam, lParam);

  return rVal;
  }

///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmCompareItem(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OpHwndWndItemD*         hwi;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
    hwi = wnd->GetItemByControlID(wParam);
    UnlockWindowData(oiWindow);
    if (hwi) //we know what control it is for
      return hwi->ParentNotification(hwnd, message, wParam, lParam);
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmMeasureItem(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  OpHwndWndItemD*         hwi;
  LPMEASUREITEMSTRUCT     mis = (LPMEASUREITEMSTRUCT)lParam;

  if (mis->CtlType == ODT_BUTTON || mis->CtlType == ODT_COMBOBOX || mis->CtlType == ODT_LISTBOX)
    {
    oiWindow = GetIDfromExtraBytes(hwnd);
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
      hwi = wnd->GetItemByControlID(wParam);
      UnlockWindowData(oiWindow);

      if (hwi)
        return hwi->ParentNotification(hwnd, message, wParam, lParam);
      }
    }
  else
    {
    //// It is for a menu
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmDrawItem(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  LPDRAWITEMSTRUCT        dis = (LPDRAWITEMSTRUCT)lParam;
  OpHwndWndItemD*         hwi;

  if (dis->CtlType == ODT_BUTTON || dis->CtlType == ODT_COMBOBOX || dis->CtlType == ODT_LISTBOX)
    {
    oiWindow = GetIDfromExtraBytes(hwnd);
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
      hwi = wnd->GetItemByControlID(wParam);
      UnlockWindowData(oiWindow);

      if (hwi)
        return hwi->ParentNotification(hwnd, message, wParam, lParam);
      }
    }
  else
    {
    //// It is for a menu
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmCtlColor(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  HWND                    hwndChild = (HWND)LOWORD(lParam);
  OpHwndWndItemD*         hwi;
  int                     controlID;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;

  if ( hwndChild)
    {
    controlID = GetWindowWord(hwndChild, GWW_ID);
    oiWindow = GetIDfromExtraBytes(hwnd);
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
      hwi = wnd->GetItemByControlID(controlID);
      UnlockWindowData(oiWindow);
      if (hwi) //we know what control it is for
        return hwi->ParentNotification(hwnd, message, wParam, lParam);
      }
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

////////////////////////////////////

LRESULT WINDOWRUNTIME::WmVKeyToItem(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  HWND                    hwndChild = (HWND)LOWORD(lParam);
  OpHwndWndItemD*         hwi;
  int                     controlID;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;

  if ( hwndChild)
    {
    controlID = GetWindowWord(hwndChild, GWW_ID);
    oiWindow = GetIDfromExtraBytes(hwnd);
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
      hwi = wnd->GetItemByControlID(controlID);
      UnlockWindowData(oiWindow);
      if (hwi) //we know what control it is for
        return hwi->ParentNotification(hwnd, message, wParam, lParam);
      }
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

////////////////////////////////

LRESULT WINDOWRUNTIME::WmCharToItem(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  HWND                    hwndChild = (HWND)LOWORD(lParam);
  OpHwndWndItemD*         hwi;
  int                     controlID;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;

  if ( hwndChild)
    {
    controlID = GetWindowWord(hwndChild, GWW_ID);
    oiWindow = GetIDfromExtraBytes(hwnd);
    if (oiWindow)
      {
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
      hwi = wnd->GetItemByControlID(controlID);
      UnlockWindowData(oiWindow);
      if (hwi) //we know what control it is for
        return hwi->ParentNotification(hwnd, message, wParam, lParam);
      }
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

////////////////////////////////

LRESULT WINDOWRUNTIME::WmDeleteItem(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OpHwndWndItemD*         hwi;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
    hwi = wnd->GetItemByControlID(wParam);
    UnlockWindowData(oiWindow);
    if (hwi) //we know what control it is for
      return hwi->ParentNotification(hwnd, message, wParam, lParam);
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////
///////////////////////////////////////////////////////
char RemoveAsciiKeyFromQue(
  HWND                    hwndTarget) //for this window
  {
  MSG                     msg;

 // be sure to PM_NOYIELD so we don't lose our context!

  if (PeekMessage(&msg, hwndTarget, WM_CHAR, WM_CHAR, PM_REMOVE|PM_NOYIELD))
    return (char)(msg.wParam & 0x00ff);

  if (PeekMessage(&msg, hwndTarget, WM_SYSCHAR, WM_SYSCHAR, PM_REMOVE | PM_NOYIELD))
    return (char)(msg.wParam & 0x00ff);

  return 0;
  }

////////

LRESULT WINDOWRUNTIME::WmKeyDown(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  OpLiteWndItemD*         lwi;
  char                    asciiKey;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
    lwi = (OpLiteWndItemD*)wnd->GetActiveItem();
    UnlockWindowData(oiWindow);

    if (lwi) //Possible to have to active item
      { 
      I_VERIFY_WI_LITE(lwi, return DefWindowProc(hwnd, message, wParam, lParam));

      asciiKey = RemoveAsciiKeyFromQue(hwnd);
      lwi->KeyDown(wParam, asciiKey, lParam);
      return 0L;
      }
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmKeyUp(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  OpLiteWndItemD*         lwi;
  char                    asciiKey;

  oiWindow = GetIDfromExtraBytes(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return DefWindowProc(hwnd, message, wParam, lParam));
    lwi = (OpLiteWndItemD*)wnd->GetActiveItem();
    UnlockWindowData(oiWindow);

    if (lwi) //Possible to have to active item
      { 
      I_VERIFY_WI_LITE(lwi, return DefWindowProc(hwnd, message, wParam, lParam));
    
      asciiKey = RemoveAsciiKeyFromQue(hwnd);
      lwi->KeyUp(wParam, asciiKey, lParam);
      return 0L;
      }
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

///////////////////////////////////////////////
///////////////////////////////////////////////


LRESULT WINDOWRUNTIME::WmMouseMove(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  POINT                   pt;
  OpLiteWndItemD*         lwi;
  HDC                     hdc;

  pt = MAKEPOINT(lParam);
  hdc = GetDC(hwnd);
  DPtoLP(hdc, &pt, 1);
  ReleaseDC(hwnd, hdc);

  lwi = GetLiteItemFromPointOrCapture(hwnd, pt);
  if (lwi  &&  WINDOWAPI::IsItemEnabled(lwi) )
    {
    lwi->MouseMove(&pt, wParam);
    return 0; 
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmMouseDown(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  int                     button;
  POINT                   pt;
  OpLiteWndItemD*         lwi;
  HDC                     hdc;

  pt = MAKEPOINT(lParam);
  hdc = GetDC(hwnd);
  DPtoLP(hdc, &pt, 1);
  ReleaseDC(hwnd, hdc);

  lwi = GetLiteItemFromPointOrCapture(hwnd, pt);
  if (lwi  &&  WINDOWAPI::IsItemEnabled(lwi) )
    {
    button = message == WM_LBUTTONDOWN ? MK_LBUTTON : message == WM_RBUTTONDOWN ? MK_RBUTTON : MK_MBUTTON;
    lwi->MouseDown(&pt, wParam, button);
    return 0; 
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmMouseDblClicked(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  int                     button;
  POINT                   pt;
  OpLiteWndItemD*         lwi;
  HDC                     hdc;

  pt = MAKEPOINT(lParam);
  hdc = GetDC(hwnd);
  DPtoLP(hdc, &pt, 1);
  ReleaseDC(hwnd, hdc);

  lwi = GetLiteItemFromPointOrCapture(hwnd, pt);
  if (lwi  &&  WINDOWAPI::IsItemEnabled(lwi) )
    {
    button = message == WM_LBUTTONDBLCLK ? MK_LBUTTON : message == WM_RBUTTONDBLCLK ? MK_RBUTTON : MK_MBUTTON;
    lwi->DoubleClicked(&pt, wParam, button);
    return 0;
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

/////////////////////////////////////////////////////

LRESULT WINDOWRUNTIME::WmMouseUp(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  int                     button;
  POINT                   pt;
  OpLiteWndItemD*         lwi;
  HDC                     hdc;

  pt = MAKEPOINT(lParam);
  hdc = GetDC(hwnd);
  DPtoLP(hdc, &pt, 1);
  ReleaseDC(hwnd, hdc);

  lwi = GetLiteItemFromPointOrCapture(hwnd, pt);
  if (lwi  &&  WINDOWAPI::IsItemEnabled(lwi) )
    {
    button = message == WM_LBUTTONUP ? MK_LBUTTON : message == WM_RBUTTONUP ? MK_RBUTTON : MK_MBUTTON;
    lwi->MouseUp(&pt, wParam, button);
    return 0;
    }

  return DefWindowProc(hwnd, message, wParam, lParam);
  }

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

LRESULT far pascal _export WndCallbackProc(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  WINDOWRUNTIME*          wr;
  LRESULT                 rVal = 0;

  switch(message)
    {
    case WM_CREATE:
      if ( (wr = GetWindowRuntime()) != NULL)
				{
	      rVal = wr->WmCreate(hwnd, message, wParam, lParam);
	      wr->SetZOrderList();
				}
			else
				rVal = -1; //this will terminate the creation of the window
      break;

    case WM_ACTIVATE:
      if ( (wr = GetWindowRuntime()) != NULL)
				{
	      wr->SetZOrderList();
	      rVal = wr->WmActivate(hwnd, message, wParam, lParam);
				}
      break;

    case WM_NCLBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCRBUTTONDBLCLK:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmNcButton(hwnd, message, wParam, lParam);
      break;

    case WM_MOUSEACTIVATE:
      if ( (wr = GetWindowRuntime()) != NULL)
				rVal = wr->WmMouseActivate(hwnd, message, wParam, lParam);
			else
				rVal = MA_NOACTIVATEANDEAT;  //don't active the window, and eat the mouse message
      break;

    case WM_SYSCOMMAND:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmSysCommand(hwnd, message, wParam, lParam);
      break;

    case WM_CLOSE:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmClose(hwnd, message, wParam, lParam);
      break;

    case WM_GETMINMAXINFO:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmGetMinMaxInfo(hwnd, message, wParam, lParam);
      break;

    case WM_SETCURSOR:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmSetCursor(hwnd, message, wParam, lParam);
      break;

    case WM_MOVE:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmMove(hwnd, message, wParam, lParam);
      break;

    case WM_SIZE:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmSize(hwnd, message, wParam, lParam);
      break;

    case WM_ERASEBKGND:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmEraseBkgnd(hwnd, message, wParam, lParam);
      break;

    case WM_QUERYDRAGICON:
      rVal = (LRESULT)AAppGetIcon();
      break;

    case WM_PAINT:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmPaint(hwnd, message, wParam, lParam);
      break;

		case WM_PALETTECHANGED:
			if ((HWND)wParam == hwnd)
				break;
			//else fall through
		case WM_QUERYNEWPALETTE:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmQueryNewPalette(hwnd, message, wParam, lParam);
      break;

    case WM_MENUCHAR:
      wr = GetWindowRuntime();
      rVal = wr->WmMenuChar(hwnd, message, wParam, lParam);
      break;

    case WM_COMMAND:
      wr = GetWindowRuntime();
      rVal = wr->WmCommand(hwnd, message, wParam, lParam);
      break;

    case WM_MENUSELECT:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmMenuSelect(hwnd, message, wParam, lParam);
			else
				rVal = 1; //to close the current menu
      break;

    case WM_VSCROLL:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmVScroll(hwnd, message, wParam, lParam);
      break;

    case WM_HSCROLL:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmHScroll(hwnd, message, wParam, lParam);
      break;


    case WM_VKEYTOITEM:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmVKeyToItem(hwnd, message, wParam, lParam);
			else
				rVal = -2;  //to indicate that all processing has been done -- not true, but if wr==NULL, we can't do anything anyway
      break;

    case WM_CHARTOITEM:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmCharToItem(hwnd, message, wParam, lParam);
			else
				rVal = -2;  //to indicate that all processing has been done -- not true, but if wr==NULL, we can't do anything anyway
      break;

    case WM_COMPAREITEM:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmCompareItem(hwnd, message, wParam, lParam);
      break;

    case WM_MEASUREITEM:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmMeasureItem(hwnd, message, wParam, lParam);
      break;

    case WM_DRAWITEM:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmDrawItem(hwnd, message, wParam, lParam);
      break;

    case WM_CTLCOLOR:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmCtlColor(hwnd, message, wParam, lParam);
      break;

    case WM_DELETEITEM:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmDeleteItem(hwnd, message, wParam, lParam);
      break;

    case WM_KEYUP:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmKeyUp(hwnd, message, wParam, lParam);
      break;

    case WM_KEYDOWN:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmKeyDown(hwnd, message, wParam, lParam);
      break;

    case WM_MOUSEMOVE:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmMouseMove(hwnd, message, wParam, lParam);
      break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmMouseDown(hwnd, message, wParam, lParam);
      break;

    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmMouseDblClicked(hwnd, message, wParam, lParam);
      break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      if ( (wr = GetWindowRuntime()) != NULL)
	      rVal = wr->WmMouseUp(hwnd, message, wParam, lParam);
      break;

    default:
      rVal = DefWindowProc(hwnd, message, wParam, lParam);
      break;

    }

	AAppDefWndProc(hwnd, message, wParam, lParam);

  return rVal;
  }


