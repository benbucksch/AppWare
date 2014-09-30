#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_menus.h>

#include "modinfo.h"

#define TEST_WINDOW  "MENU_TEST_WINDOW"

static g_registerCount = 0;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Register
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
LRESULT  CALLBACK _export WndProc(
  HWND hWnd,
  UINT iMessage,
  WPARAM wParam,
  LPARAM lParam )
  {
  BOOL            msgHandled = TRUE;
  LRESULT         result     = 0;
  LPCREATESTRUCT  pCreateStruct;
  MENUSPARAMBLOCK params;

  switch (iMessage)
    {
    case WM_CREATE:
      pCreateStruct = (LPCREATESTRUCT)lParam;
      SetWindowLong(hWnd, 0, (long)pCreateStruct->lpCreateParams);
      break;

    case WM_COMMAND:
      if (LOWORD(lParam) == 0)
        {
        params.objID = (OBJECTID)GetWindowLong(hWnd, 0);
        params.itemID = wParam;
        AEvtCallMethod(0, params.objID, 0, OEVENT_HANDLECOMMAND,
         (long)(LPMENUSPARAMBLOCK)&params);
        }
      else
        msgHandled = FALSE;
      break;

    default:
      msgHandled = FALSE;
      break;

    }

  if (FALSE == msgHandled)
    result = DefWindowProc( hWnd, iMessage, wParam, lParam );

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Register
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
HWND CreateAndShow(
  OBJECTID  objID)
  {
  HWND    hWnd;
        // Pass 'this' pointer in lpParam of CreateWindow().

  hWnd = CreateWindow(TEST_WINDOW,
            "",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            NULL,
            NULL,
            MODULEINFO::hInstance,
            (void far *)objID);

  if (hWnd)
    {
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
    }

  return hWnd;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemDisable
//
// DESCRIPTION.: Param 1 - Menu Bar Object
//
////////////////////////////////////////////////////////////////////////////
extern "C" void FAR PASCAL _export CreateTestWindow(
  pAEvtInfo    evt,
  pAObjMessage sys)
  {
  BOOL             goodParams = TRUE;
	OBJECTID	       barID;
  HWND             hWnd;
  HMENU            hMenu;
  MENUSPARAMBLOCK  params;


  (void far*)evt;

	barID = AFuncGetTypedParameter(1, OTYPE_MENUBAR);

	if (barID == 0)
    goodParams = FALSE;

  ADBG_EVAL(
	  if (FALSE == goodParams)
		  ADbgPrint(ADBG_WARNING, 0, "Bad Parameter: Item Disable");
    );

  if (TRUE == goodParams)
    {
    if (0 != (hWnd = CreateAndShow(barID)))
      {
      params.objID = barID;
      AEvtCallMethod(0, barID, 0, OEVENT_GETMENUHANDLE,
       (long)(LPMENUSPARAMBLOCK)&params);
      hMenu = params.hMenu;
      SetMenu(hWnd, hMenu);
      DrawMenuBar(hWnd);
      }
    }

 	AFuncReportResult(sys, 1, "");

  }



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: handleStartUp
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
extern "C" void FAR PASCAL _export handleStartUp(
  pAEvtInfo    evt,
  pAObjMessage sys)
  {
  WNDCLASS wndclass;   // Structure used to register Windows class.

  if (g_registerCount == 0)
    {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = ::WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = sizeof(OBJECTID);
    wndclass.hInstance     = MODULEINFO::hInstance;
    wndclass.hIcon         = 0;
    wndclass.hCursor       = 0;
    wndclass.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = TEST_WINDOW;

    if (RegisterClass(&wndclass))
      g_registerCount++;
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: handleShutDown
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
extern "C" void FAR PASCAL _export handleShutDown(
  pAEvtInfo    evt,
  pAObjMessage sys)
  {

  if (g_registerCount > 0)
    {
    g_registerCount--;
    if (g_registerCount == 0)
      UnregisterClass(TEST_WINDOW, MODULEINFO::hInstance);
    }

  }
