#define STRICT
#define ASTRICT

#include<a_alm.h>
#include <windowsx.h>
#include "movctrl.h"
#include "moviedat.h"
#include <stdio.h>
#include <qtw.h>

extern HINSTANCE hInstance;
extern MovieFile mfMovie;
extern char szCtrlVIWndName[];
///////////////////////////////////////////////////////////////////////////////////
//  Function:
//    MovVIWndProc
//  Description:
//    Procedure of the Movie Window that must be created in the Movie Dialog Box
//////////////////////////////////////////////////////////////////////////////////

OpWndItemD* ALMAPI  MCtrlVICreate(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  )
{
  MCONTROLLERVI* mctrl_vi;
  I_UNUSED(special);

  mctrl_vi = new MCONTROLLERVI(oiItem,oiParent,rcItem,status);
  AObjSetRuntimeInfo(oiItem,(LONG)mctrl_vi);
  return mctrl_vi;
}

MCONTROLLERVI::MCONTROLLERVI(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status): OpHwndWndItem(oiItem,oiParent,rcItem,status)

{
  LPMCTRLDEV lpCtrlDev;
  MOVIERUNPTR lpMovieRun;
  
  NewMovieFromFile (&mMovie, mfMovie, NULL, NULL, 0, NULL);

  lpCtrlDev = (LPMCTRLDEV) AObjLockData(m_oiObject,MCtrlDev);
  {
    oiMovie = lpCtrlDev->oiMovie;   //Movie Object associated with the Controller Object
    if (oiMovie)
    {
      //Set Movie of the Movie object to the controller object
      lpMovieRun = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
      if ((lpMovieRun->mMovie))
	mMovie = lpMovieRun->mMovie;
    }
  }
  AObjUnlockData(m_oiObject,MCtrlDev);

  SetRect(&mapRect,0,0,0,0);

  OWndSetItemUsesTab(this, TRUE);
  OWndSetItemActivatable(this,TRUE);//lpMCtrlDev->canActivate);
  OWndSetItemTransparent(this, FALSE);
}
                              	

MCONTROLLERVI::~MCONTROLLERVI(void)
{
  DisposeMovie(mMovie);
  DisposeMovieController(mController);
}

void ALMAPI MCONTROLLERVI::Destroy(void)
{
  DisposeMovie(mMovie);
  DisposeMovieController(mController);
  delete this;
}

void ALMAPI MCONTROLLERVI::DataChanged(long details, long modifiers, long variation)
{
  I_UNUSED(details);
  I_UNUSED(variation);
  I_UNUSED(modifiers);
}


void ALMAPI MCONTROLLERVI::ParentOpened()
{
  RECT rcClient = m_rcItem;
  RECT rcGrowBox, MCtrlRect;
  static DWORD WinFlags = WS_CHILD | WS_VISIBLE; // | WS_BORDER;
  int height, width;

  OWndLPtoDP(m_oiParent, (LPPOINT)&rcClient, 2);

  m_hwndItem = CreateWindow(szCtrlVIWndName,NULL,
		       WinFlags,
		       rcClient.left,
		       rcClient.top,
		       rcClient.right - rcClient.left,
		       rcClient.bottom - rcClient.top,
		       m_hwndParent,
		       (HMENU)OWndGetItemControlID(this),
		       hInstance,
		       (LPVOID)m_oiObject);
  SetMovieActive(mMovie,TRUE);
  ShowWindow(m_hwndItem,SW_SHOW);
  UpdateWindow(m_hwndItem);
}

void ALMAPI MCONTROLLERVI::ParentClosing()
{
  DisposeMovieController(mController);
  DestroyWindow(m_hwndItem);
  m_hwndItem = 0;
}

long ALMAPI MCONTROLLERVI::Activate(int causedBy, BOOL mustKeep)
{
  return 0L;
}

void ALMAPI MCONTROLLERVI::Deactivated(void)
{
}

/////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovVIWndProc
// Description: 
//   Window procedure of the Movie Visual Instance Window
//
////////////////////////////////////////////////////////////////////////////////
long FAR PASCAL _export CtrlVIWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
  OBJECTID  oiController;
  OBJECTID oiMovie;
  LPCREATESTRUCT lpc;
  PAINTSTRUCT ps;
  LPVOID lpv;
  RECT CtrlWndRect, rcMovie;
  POINT ptMovie;
  static MovieController mController;
  Movie mMovie;
  MCONTROLLERVI *mCtrlVI;
  MOVIEVI *MovieVI;
  HDC hdc;
  HWND hMovWnd;

  if (MCIsPlayerMessage (mController, hwnd, message, wParam, lParam))
    return 0;

  switch (message)
  {
    case WM_CREATE:
      lpc = (LPCREATESTRUCT) lParam;
      lpv = (lpc->lpCreateParams);
      oiController = (OBJECTID)lpv;
      SetWindowLong(hwnd,DWL_USER,(LONG)oiController);
      mCtrlVI = (MCONTROLLERVI *) AObjGetRuntimeInfo(oiController);
      mMovie = mCtrlVI->GetControllerMovie();

      hMovWnd = hwnd;
      
      GetClientRect(hwnd,&CtrlWndRect);
      mController = NewMovieController(NULL,&rcMovie,/*mcTopLeftMovie + mcScaleMovieToFit*/0,hwnd);
      GetMovieBox(mMovie,&rcMovie);
      OffsetRect(&rcMovie,-rcMovie.right,-rcMovie.bottom);

      ptMovie.x = rcMovie.left;
      ptMovie.y = rcMovie.top;


      MCSetMovie(mController,mMovie,hMovWnd,ptMovie);
      MCSetControllerAttached(mController,FALSE);
      mCtrlVI->SetMovieController(mController);
      MCSetControllerBoundsRect(mController,&CtrlWndRect);
      break;

    case WM_PAINT:
      hdc = BeginPaint(hwnd,&ps);
      EndPaint(hwnd,&ps);
      break;

    case WM_SIZE:
    case WM_MOVE:
      oiController = (OBJECTID) GetWindowLong(hwnd,DWL_USER);
      mCtrlVI = (MCONTROLLERVI *) AObjGetRuntimeInfo(oiController);
      mController = mCtrlVI->GetMovieController();
      mMovie = mCtrlVI->GetControllerMovie();
      hdc = GetDC(hwnd);
      GetClientRect(hwnd,&CtrlWndRect);
      GetMovieBox(mMovie,&rcMovie);
      OffsetRect(&rcMovie,-rcMovie.right,-rcMovie.bottom);

      MCSetControllerBoundsRect(mController,&CtrlWndRect);
      MCPositionController(mController,&rcMovie,&CtrlWndRect,0);
      ReleaseDC(hwnd,hdc);
      break;
  }
  return DefWindowProc(hwnd,message,wParam,lParam);
}