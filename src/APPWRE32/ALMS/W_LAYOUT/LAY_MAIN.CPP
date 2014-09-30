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
// FILE:          "lay_main.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   WINDOW LAYOUT ALM main file for LibMain
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdlib.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_wndshp.h>
//#include <defines.h>

#include "xp_macro.h"
#include "wlay.h"
#include "objlist.h"
#include "menudata.h"
#include "lay_algn.h"
#include "lay_dlg.h"
#include "lay_wnd.h"
#include "lay_shap.h"
#include "layout.h"
#include "balloon.h"


#include "rc\wlay_rsc.h"

// global variables ////////////////////////////////////////////////////////

#ifdef __BORLANDC__
extern int    _WinAllocFlag;
#endif

HINSTANCE     g_hInstance;

HBITMAP       g_hbmpArrow;
HBITMAP       g_hbmpReorder;
HBITMAP       g_hbmpTab;
HBITMAP       g_hbmpGroup;
HBITMAP       g_hbmpLock;
HBITMAP       g_hbmpVisibility;
HBITMAP       g_hbmpAnchor;
HBITMAP       g_hbmpSelectAll;
HBITMAP       g_hbmpWindowBG;

HCURSOR       g_hcurNmbr12;
HCURSOR       g_hcurNmbr21;
HCURSOR       g_hcurTab;
HCURSOR       g_hcurGroup;
HCURSOR       g_hcurLock;
HCURSOR       g_hcurVisibility;
HCURSOR       g_hcurAnchor;

HICON         g_hiconWnd;

GETPLACEPROC  g_lpfnGetWindowPlacement = 0;

const char*   c_szAppBuilderIni        = "appware.ini";
const char*   c_szAppWareIni           = "appware.ini";
const char*   c_szLayoutTitle          = "Window Layout";
const char*   c_szLayoutClassName      = "WLayLayout";
const char*   c_szFrameClassName       = "WLayFrameWnd";
const char*   c_szClientClassName      = "WLayClientWnd";
const char*   c_szMenuClassName        = "WLayMenuWnd";
const char*   c_szSampleClassName      = "WLaySampleWnd";

#ifdef AW_I_DEBUG
HINSTANCE     g_hinstShape;
#endif

// function prototypes /////////////////////////////////////////////////////

void InitializeDLL(HINSTANCE hinstDLL);
void CleanUpDLL();
void RegisterLayoutWindow();

HWND CreateLayoutWindow(HMENU hmenu, Layout* pLayout);

void RegisterFrameWindow();
void RegisterClientWindow();
void RegisterMenuWindow();
void RegisterSampleWindow();

extern "C" BOOL ALMAPI AAppShowThreadWindows(BOOL fShow, AMEMBLOCKID miWnd);
struct ShowThreadWnds
  {
  int   nNumWnds;
  HWND  hwnd[];
  };

#if defined(__FLAT__)

// DllEntryPoint ///////////////////////////////////////////////////////////

extern "C" BOOL WINAPI DllEntryPoint
  (
  HINSTANCE hinstDLL,
  DWORD     fdwReason,
  LPVOID    pReserved
  )

  {
  I_UNUSED(pReserved);

  switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      InitializeDLL(hinstDLL);
      break;

    case DLL_PROCESS_DETACH:
      CleanUpDLL();
      break;

    default:
      break;
    }

  return TRUE;
  }

#else

// LibMain /////////////////////////////////////////////////////////////////

int WINAPI LibMain
  (
  HINSTANCE hInstance,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine
  )

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  #ifdef __BORLANDC__
  // use this magical line to make the 'new' operator work in Borland C++!?
  _WinAllocFlag = GMEM_SHARE;
  #endif

  InitializeDLL(hInstance);

  if(wHeapSize>0)
    UnlockData(0);

  return 1; // the dll was initialized properly
  }

// WEP /////////////////////////////////////////////////////////////////////

int WINAPI WEP
  (
  int nParameter
  )

  {
  I_UNUSED(nParameter);

  CleanUpDLL();

  return 1;
  }
#endif  // #if defined(__FLAT__) .... #else

////////////////////////////////////////////////////////////////////////////
//
// Function:    InitializeDLL
//
// Description: This function initializes the DLL and loads global variables.
//
////////////////////////////////////////////////////////////////////////////

void InitializeDLL
  (
  HINSTANCE hinstDLL
  )

  {
  // initialize the global variables
  g_hInstance      = hinstDLL;

  g_hbmpArrow      = LoadBitmap(g_hInstance, ID(STDTOOL_ARROW));
  g_hbmpReorder    = LoadBitmap(g_hInstance, ID(STDTOOL_REORDER));
  g_hbmpTab        = LoadBitmap(g_hInstance, ID(STDTOOL_TABSTOP));
  g_hbmpGroup      = LoadBitmap(g_hInstance, ID(STDTOOL_GROUPSTOP));
  g_hbmpLock       = LoadBitmap(g_hInstance, ID(STDTOOL_LOCK));
  g_hbmpVisibility = LoadBitmap(g_hInstance, ID(STDTOOL_VISIBILITY));
  g_hbmpAnchor     = LoadBitmap(g_hInstance, ID(STDTOOL_ANCHOR));
  g_hbmpSelectAll  = LoadBitmap(g_hInstance, ID(STDTOOL_SELECTALL));
  g_hbmpWindowBG   = LoadBitmap(g_hInstance, ID(STDTOOL_BACKGROUND));

  g_hcurNmbr12     = LoadCursor(g_hInstance, ID(CUR_NMBR_12));
  g_hcurNmbr21     = LoadCursor(g_hInstance, ID(CUR_NMBR_21));
  g_hcurTab        = LoadCursor(g_hInstance, ID(CUR_TABTOOL));
  g_hcurGroup      = LoadCursor(g_hInstance, ID(CUR_GROUPTOOL));
  g_hcurLock       = LoadCursor(g_hInstance, ID(CUR_LOCKTOOL));
  g_hcurVisibility = LoadCursor(g_hInstance, ID(CUR_VISBTOOL));
  g_hcurAnchor     = LoadCursor(g_hInstance, ID(CUR_ANCHOR));

  g_hiconWnd       = LoadIcon(g_hInstance, ID(ICON_LAYOUT));

  WndData::CreatePalette();   // create the default palette

  // register the window classes
  RegisterLayoutWindow();     // the layout window
  RegisterFrameWindow();
  RegisterClientWindow();
  RegisterMenuWindow();
  RegisterPaletteWindow();    // the alignment palette
  RegisterObjectPalette();    // the object list palette
  RegisterSampleWindow();

  #ifdef AW_I_DEBUG
  g_hinstShape = LoadLibrary("AW32SHAP.DLL");
  #endif
  }

////////////////////////////////////////////////////////////////////////////
//
// Function:    CleanUpDLL
//
// Description: This function frees any remaning allocated resources.
//
////////////////////////////////////////////////////////////////////////////

void CleanUpDLL
  (
  )

  {
  // Free the std tool bitmaps
  DeleteObject(g_hbmpArrow);
  DeleteObject(g_hbmpReorder);
  DeleteObject(g_hbmpTab);
  DeleteObject(g_hbmpGroup);
  DeleteObject(g_hbmpLock);
  DeleteObject(g_hbmpVisibility);
  DeleteObject(g_hbmpAnchor);
  DeleteObject(g_hbmpSelectAll);
  DeleteObject(g_hbmpWindowBG);

  // free the cursors
  DestroyCursor(g_hcurNmbr12);
  DestroyCursor(g_hcurNmbr21);
  DestroyCursor(g_hcurTab);
  DestroyCursor(g_hcurGroup);
  DestroyCursor(g_hcurLock);
  DestroyCursor(g_hcurVisibility);
  DestroyCursor(g_hcurAnchor);

  DestroyIcon(g_hiconWnd);

  WndData::DestroyPalette();

  UnregisterPaletteWindow();

  #ifdef AW_I_DEBUG
  FreeLibrary(g_hinstShape);
  #endif
  }

////////////////////////////////////////////////////////////////////////////
//
// Function:    RegisterLayoutWindow
//
// Description: This function registers the layout window class.
//
////////////////////////////////////////////////////////////////////////////

void RegisterLayoutWindow
  (
  )

  {
  WNDCLASS  wc;

  wc.style = CS_OWNDC | CS_DBLCLKS;
  wc.lpfnWndProc = AWLayoutWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(Layout*);
  wc.hInstance = g_hInstance;
  wc.hIcon = g_hiconWnd;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = c_szLayoutClassName;
  RegisterClass(&wc);
  }


////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    CreateLayoutWindow
//
// DESCRIPTION: Creates the layout editing window.
//
////////////////////////////////////////////////////////////////////////////

HWND  CreateLayoutWindow
  (
  HMENU   hmenu,
  Layout* pLayout
  )

  {
  HWND    hwnd;
  double  left, top, right, bottom;
  int     fMax;
  RECT    rc;
  HDC     hdc;

  if(pLayout->PrstWndPositions())
    {
    char  szDefault[80];
    char  szBuffer[80];
    char* szNext;

    wsprintf(szDefault, "0., 0., 100., 100., 0");
    GetPrivateProfileString("Layout", "Startup Position", szDefault, szBuffer, 80, c_szAppBuilderIni);

    left   = strtod(szBuffer, &szNext);
    top    = strtod(++szNext, &szNext);
    right  = strtod(++szNext, &szNext);
    bottom = strtod(++szNext, &szNext);
    fMax   = (int)strtol(++szNext, &szNext, 0);
    }
  else
    {
    left   = 0.;
    top    = 0.;
    right  = 100.;
    bottom = 100.;
    fMax   = 0;
    }

  rc.left   = (int)(left   * (double)GetSystemMetrics(SM_CXSCREEN) / 100.);
  rc.top    = (int)(top    * (double)GetSystemMetrics(SM_CYSCREEN) / 100.);
  rc.right  = (int)(right  * (double)GetSystemMetrics(SM_CXSCREEN) / 100.);
  rc.bottom = (int)(bottom * (double)GetSystemMetrics(SM_CYSCREEN) / 100.);

  if(rc.left < 0)
    rc.left = 0;
  if(rc.top < 0)
    rc.top = 0;
  if(rc.right > GetSystemMetrics(SM_CXSCREEN))
    rc.right = GetSystemMetrics(SM_CXSCREEN);
  if(rc.bottom > GetSystemMetrics(SM_CYSCREEN))
    rc.bottom = GetSystemMetrics(SM_CYSCREEN);

  hwnd = CreateWindow(
    c_szLayoutClassName,
    c_szLayoutTitle,
    WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
    rc.left,
    rc.top,
    rc.right-rc.left,
    rc.bottom-rc.top,
    HWND_DESKTOP,
    hmenu,
    g_hInstance,
    (void*)pLayout);

  // hide the scroll bars
  ShowScrollBar(hwnd, SB_HORZ, FALSE);
  ShowScrollBar(hwnd, SB_VERT, FALSE);

  // set the window and viewport origin
  hdc = GetDC(hwnd);
  SetWindowOrgEx(hdc, 0, 0, NULL);
  SetViewportOrgEx(hdc, 0, 0, NULL);
  ReleaseDC(hwnd, hdc);

  // show the layout window
  ShowWindow(hwnd, fMax ? SW_SHOWMAXIMIZED : SW_SHOW);

  return hwnd;
  }

////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    OWndEditLayout
//
// DESCRIPTION: Starts editing the specified window object.
//
////////////////////////////////////////////////////////////////////////////

extern "C" long ALMAPI OWndEditLayout
  (
  AOBJECTID  oiWnd
  )

  {
  long        lRet;
  Layout*     layout      = new Layout();

  HMENU       hmenu       = LoadMenu(g_hInstance, ID(MENU_LAYOUT));
  HACCEL      hAccel      = LoadAccelerators(g_hInstance, ID(ACCEL_LAYOUT));
  HWND        hwndLayout;
  MSG         msg;
  HCURSOR     hcur;
  AMEMBLOCKID  miWnd       = AMemAllocate(0L);


  hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
  AAppShowThreadWindows(FALSE, miWnd);
  hwndLayout = CreateLayoutWindow(hmenu, layout);

  layout->InitPalettes();
  layout->OpenWindow(oiWnd);
  SetCursor(hcur);

  // make my own message loop so the window will act like a dialog....
  while(GetMessage(&msg, NULL, 0, 0))
    {
    if((msg.hwnd==hwndLayout) && (msg.message==WM_ENDLAYOUT))
      {
      if(msg.wParam>0)
        lRet = A_MODIFIED;
      else
        lRet = A_OK;

      break;
      }

    if(!TranslateAccelerator(hwndLayout, hAccel, &msg))
      {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      }
    }   // end while(GetMessage ...)

  // release and destroy the Layout window menu
  SetMenu(hwndLayout, NULL);
  DestroyMenu(hmenu);

  // destroy the layout and enable the VAB environment
  AAppShowThreadWindows(TRUE, miWnd);
  AMemFree(miWnd);

  Balloon_ShutdownServer();

  DestroyWindow(hwndLayout);
  delete layout;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// Function:    RegisterFrameWindow
//
// Description: This function registers the "ALM_WLayFrameWnd" window class.
//
////////////////////////////////////////////////////////////////////////////

void RegisterFrameWindow()
  {
  WNDCLASS  wc;

  wc.style = CS_DBLCLKS;
  wc.lpfnWndProc = AWFrameWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(Window*);
  wc.hInstance = g_hInstance;
  wc.hIcon = g_hiconWnd;
  wc.hCursor = NULL;;
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = c_szFrameClassName;
  RegisterClass(&wc);
  }

////////////////////////////////////////////////////////////////////////////
//
// Function:    RegisterClientWindow
//
// Description: This function registers the "WLayClientWnd" window class.
//
////////////////////////////////////////////////////////////////////////////

void RegisterClientWindow
  (
  )

  {
  WNDCLASS  wc;

  wc.style = CS_OWNDC | CS_DBLCLKS;
  wc.lpfnWndProc = AWClientWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(Window*) + sizeof(Shapes*);
  wc.hInstance = g_hInstance;
  wc.hIcon = NULL;
  wc.hCursor = NULL;;
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = c_szClientClassName;
  RegisterClass(&wc);
  }

////////////////////////////////////////////////////////////////////////////
//
// Function:    RegisterMenuWindow
//
// Description: This function registers the "ALM_WLayMenuWnd" window class.
//
////////////////////////////////////////////////////////////////////////////

void RegisterMenuWindow
  (
  )

  {
  WNDCLASS  wc;

  wc.style = CS_DBLCLKS;
  wc.lpfnWndProc = AWMenuWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(Window*) + sizeof(MenuData*);
  wc.hInstance = g_hInstance;
  wc.hIcon = NULL;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_MENU+1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = c_szMenuClassName;
  RegisterClass(&wc);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     RegisterSampleWindow
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void RegisterSampleWindow
  (
  )

  {
  WNDCLASS  wc;

  wc.style = 0;
  wc.lpfnWndProc = OWLaySampleWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = g_hInstance;
  wc.hIcon = NULL;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = c_szSampleClassName;

  RegisterClass(&wc);
  }

////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    OWndItemFromUID
//
// DESCRIPTION: Returns a pointer to the window item.  (Based on UID).
//
////////////////////////////////////////////////////////////////////////////

extern "C" OpWndItemD* ALMAPI OWndItemFromUID
  (
  HWND  hwnd,
  long  uid
  )

  {
  Items*  pItems = ((Window*)GetWindowLong(hwnd, I_GWL_WINDOW))->Items();

  return pItems->ItemFromUID(uid);
  }

////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    HideAllThreadWindows
//
// DESCRIPTION: This will be called with each thread window as they are
//              enumerated.
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMCALLBACK HideAllThreadWindows
  (
  HWND    hwnd,
  LPARAM  lParam
  )

  {
  BOOL  fRet;

  if(IsWindowVisible(hwnd))
    {
    AMEMBLOCKID      miWnd = (AMEMBLOCKID)lParam;
    ShowThreadWnds* pSTW  = (ShowThreadWnds*)AMemLock(miWnd);

    // make sure there is enough space allocated in the array
    if(pSTW && (((pSTW->nNumWnds + 1) % 10) == 0))
        { // grow the list size by 10.
        AMemUnlock(miWnd);
        if(AMemSetSize(miWnd, sizeof(int)+ (pSTW->nNumWnds + 11)*sizeof(HWND)))
          pSTW = (ShowThreadWnds*)AMemLock(miWnd);
        else
          pSTW = NULL;
        }

    if(pSTW)
      {
      // hide the window
      ShowWindow(hwnd, SW_HIDE);

      // store the window handle in the memblock
      pSTW->hwnd[pSTW->nNumWnds] = hwnd;

      // increment the number of windows stored in the memblock
      pSTW->nNumWnds++;
      AMemUnlock(miWnd);
      }

    fRet = (pSTW != NULL);
    }
  else
    fRet = TRUE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    AAppShowThreadWindows
//
// DESCRIPTION: This will either show or hide all the windows of the current
//              task.  It will create a list of windows in ascending order
//              on the way the windows were hidden.  When the windows are
//              shown, the list is traversed in the reverse direction.
//
////////////////////////////////////////////////////////////////////////////

extern "C" BOOL ALMAPI AAppShowThreadWindows
  (
  BOOL        fShow,
  AMEMBLOCKID  miWnd
  )

  {
  BOOL            fRet;
  ShowThreadWnds* pSTW;

  if(miWnd)
    {
    if(fShow)
      {
      int i;

      pSTW = (ShowThreadWnds*)AMemLock(miWnd);
      if(pSTW)
        {
        for(i=pSTW->nNumWnds-1; i>=0; i--)
          ShowWindow(pSTW->hwnd[i], SW_SHOW);

        AMemUnlock(miWnd);
        }

      fRet = (pSTW != NULL);
      }
    else  // hide all the visible task windows
      {
      if(AMemSetSize(miWnd, sizeof(int)+ 10*sizeof(HWND)))
        {
        pSTW = (ShowThreadWnds*)AMemLock(miWnd);
        if(pSTW)
          {
          pSTW->nNumWnds = 0;
          AMemUnlock(miWnd);

          fRet = EnumThreadWindows(GetCurrentThreadId(), HideAllThreadWindows, (LPARAM)miWnd);
          }
        else
          fRet = FALSE;        
        }
      else
        fRet = FALSE;        
      }
    }
  else
    fRet = FALSE;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
