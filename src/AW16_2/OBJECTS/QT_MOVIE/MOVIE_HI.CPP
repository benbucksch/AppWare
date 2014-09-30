/*//////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
//  Written by DUNYE ALEXANDER, ECO High Tech Group, March 1994
//////////////////////////////////////////////////////////////*/

//////////////////////////////////////////////////////////////////////////////////////
//  File Name :
//    movie_hi.cpp
//
//  Description :
//    Methods for Visual Instance of a Handled Window Item of a Movie Object
//////////////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include <a_alm.h>
#include <o_wnd.h>

#include "moviedat.h"
#include "movieexp.h"
#include "movctrl.h"

extern HINSTANCE hInstance;
extern BOOL QTInstall;
extern VOID DrawWhiteRect(HDC hdc,int cxClient,int cyClient);
BOOL CALLBACK __export MyFilter (MovieController mController,UINT uAction, LPVOID lpParam, LONG lRefCon);
BOOL __export CALLBACK UpdateControllerInstance( HWND hwnd, LPARAM lParam );
BOOL ALMCALLBACK MuteMovieSound(OpWndItemD *pVI, long lParam1, long lParam2);
/////////////////////////////////////////////////////

extern char szMovVIWndName[];           //Movie Visual Instance Class Name
extern char szMovFrameWndName[];        //Movie Frame Window Class Name

OpWndItemD* ALMAPI  MovieVICreate(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long       /* special*/
  )
{
  MOVIEVI *MovieVI;

  MovieVI = new MOVIEVI(oiItem, oiParent, rcItem, status);
// Movie VI will be set to the extra data of the corresponding window  09.16.94
  return MovieVI;
}


MOVIEVI::MOVIEVI(
  OBJECTID oiItem,
  OBJECTID oiParent,
  RECT*    rcItem,
  long     status) : OpHwndWndItem(oiItem, oiParent, rcItem, status)

{
  mController = 0;
  mMovie      = NULL;
  hPaintWnd   = NULL;

  SetRect(&mapRect,0,0,0,0);

  OWndSetItemUsesTab(this, FALSE);
  OWndSetItemActivatable(this, TRUE);
  OWndSetItemEnabled(this, TRUE);
  OWndSetItemTransparent(this, TRUE);
  SetActivated(FALSE);
}


void ALMAPI MOVIEVI::Destroy(void)
{
  delete this;
}

void ALMAPI MOVIEVI::DataChanged(long details, long /*modifiers*/, long /*variation*/)
{
  MOVIEDEVPTR lpMovieDevRec;
  char Path[255];

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(m_oiObject, MovieDev);

  switch(details)
  {
    case EvtDefault:
      UpdateMovie(mMovie);
      break;

    case EvtDisplayMovie:
      break;

    case EvtRefocusMovie:
      break;

    case EvtDrawPoster:
      ShowMoviePoster();
      break;

    case EvtDrawMovie:
      break;

    case EvtDrawActBorder:
      DrawActBorder(lpMovieDevRec->bShowActBorder);
      break;

    case EvtDrawBadge:
      DrawBadge(lpMovieDevRec->bShowBadge);
      break;

    case EvtDrawBorder:
      DrawBorder(lpMovieDevRec->bShowBorder);
      break;

    case EvtDrawLogo:
      break;

    case EvtSetScale:
      break;

    case EvtSetScroll:
      break;

    case EvtLoadMovie:
      MCDoAction(mController,mcActionPlay,0);
      MuteSound();
      _fmemcpy(Path,lpMovieDevRec->mfMovieName,sizeof(Path));
      LoadMovie(Path);
      break;

    case EvtPlayMovie:
      PlayMovie();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtPlayPreview:
      PlayPreview();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtPrerollMovie:
      PrerollMovie();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtStopMovie:
      StopMovie();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtClearSelection:
      ClearSelection();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtSetSelection:
      SetSelDuration();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtSetSelectionTime:
      SetSelectionTime();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtGoToTime:
      GoToTime();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtSetLoopValue:
      SetLoopValue();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtCtrlHideSpeaker:
    case EvtCtrlShowSpeaker:
      ShowSpeaker(details);
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn

    case EvtCtrlHideSteppers:
    case EvtCtrlShowSteppers:
      ShowSteppers(details);
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn


    case EvtMuteSound:
      MuteSound();
      AObjUnlockData(m_oiObject, MovieDev);
      return;                           //Must not be redrawn
  }

  AObjUnlockData(m_oiObject, MovieDev);

  OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
}

void ALMAPI MOVIEVI::ParentOpened()
{
  MOVIEDEVPTR    lpMovieDevRec;
  RECT rcClient = m_rcItem;
  RECT rcFrame;

  long WinFlags = WS_CHILD;

  OWndLPtoDP(m_oiParent, (LPPOINT)&rcClient, 2);
  CopyRect(&rcFrame, &rcClient);
  InflateRect(&rcFrame, 1, 1);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(m_oiObject, MovieDev);
  if (lpMovieDevRec->bShowBorder)
    WinFlags |= WS_BORDER;
  else
    WinFlags &= ~WS_BORDER;

  AObjUnlockData(m_oiObject, MovieDev);

  m_hwndItem = CreateWindow(szMovVIWndName, NULL,
                       WinFlags,
                       rcClient.left,
                       rcClient.top,
                       rcClient.right - rcClient.left,
                       rcClient.bottom - rcClient.top,
                       m_hwndParent,
                       (HMENU)OWndGetItemControlID(this),
                       hInstance,
                       (LPVOID)this);

  hPaintWnd = m_hwndItem;
}

void ALMAPI MOVIEVI::ParentClosing()
{
  MOVIERUNPTR lpMovieRunRec;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject,MovieRun);
  lpMovieRunRec->LastClickTime = MCGetCurrentTime(mController,NULL);
  AObjUnlockData(m_oiObject,MovieRun);
  DestroyWindow(m_hwndItem);

  m_hwndItem = 0;
}


long ALMAPI MOVIEVI::Activate(int causedBy, BOOL mustKeep)
{
  SFIXED sfxVolume;
  MOVIEDEVPTR lpMovieDevRec;
  BOOL bShowActBorder;
  long i;
  OBJECTID oiMov;

  I_UNUSED(causedBy);
  I_UNUSED(mustKeep);

  if (isActivated)
    return OWND_ACTIVATION_KEPT;

  i = AOBJ_GETFIRSTKEY;
  //Mute sound for movies that are not active
  // must be loop for all movie instancies in the window
  //09.16.94

  while ((BOOL)(oiMov = AObjGetNextObject((TYPEID)OTYPE_MOVIE, (LONG *)&i)))
  {
    OWndCallEachVisualInstance(oiMov, MuteMovieSound, (LONG) this, 0);
  }


  SetFocus(m_hwndItem);
  SetActivated(TRUE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(m_oiObject,MovieDev);
  bShowActBorder = lpMovieDevRec->bShowActBorder;       //save the last active movie controller
  lpMovieDevRec->mController = mController;
  AObjUnlockData(m_oiObject,MovieDev);

// Enable Movie, Sound and Keyboard for active controller
  MCActivate(mController,m_hwndItem,TRUE);

  MCDoAction (mController, mcActionGetVolume, (LPVOID) &sfxVolume);
  sfxVolume = abs (sfxVolume);
  MCDoAction (mController, mcActionSetVolume, (LPVOID) sfxVolume);

  MCDoAction (mController, mcActionSetKeysEnabled, (LPVOID) TRUE);

  if (bShowActBorder)
    DrawActBorder(TRUE);

  return OWND_ACTIVATION_KEPT;
}

void ALMAPI MOVIEVI::Deactivated(void)
{
  MOVIEDEVPTR lpMovieDevRec;
  BOOL bShowActBorder;

  SetActivated(FALSE);
// Stop Movie

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(m_oiObject,MovieDev);
  bShowActBorder = lpMovieDevRec->bShowActBorder;
  AObjUnlockData(m_oiObject,MovieDev);


  MCActivate(mController,m_hwndItem,FALSE);


  MCDoAction (mController, mcActionSetKeysEnabled, (LPVOID) FALSE);

  if (bShowActBorder)
    DrawActBorder(FALSE);
  return;
}

void MOVIEVI::DrawActBorder(BOOL bShowActBorder)
{

  RECT ghostRect = m_rcItem;
  POINT inflatePt;
  HDC hdc;

  I_UNUSED(bShowActBorder);
  inflatePt.x = 2;
  inflatePt.y = 2;
  OWndLPtoDP(m_oiParent, (LPPOINT)&ghostRect, 2);
  InflateRect(&ghostRect, inflatePt.x, inflatePt.y);
  hdc = GetDC(m_hwndParent);
  int sDC = SaveDC(hdc);

  int mode = SetMapMode(hdc,MM_TEXT);
  DrawFocusRect(hdc,&ghostRect);
  SetMapMode(hdc, mode);
  RestoreDC(hdc, sDC);
  ReleaseDC(m_hwndParent, hdc);

}

void MOVIEVI::MuteSound(void)
{
  SFIXED sfxVolume;

  MCDoAction (mController, mcActionGetVolume, (LPVOID) &sfxVolume);
  sfxVolume =  - abs (sfxVolume);
  MCDoAction (mController, mcActionSetVolume, (LPVOID) sfxVolume);

}

void MOVIEVI::SetMovieToController(void)
{
  ::SetMovie(hPaintWnd, m_oiObject,mController, mMovie);
}


void MOVIEVI::DrawBadge(BOOL bShowBadge)
{
  MCDoAction (mController, mcActionSetUseBadge, (LPVOID)bShowBadge);
}

void MOVIEVI::DrawBorder(BOOL bShowBorder)
{
  long style;

  style = GetWindowLong(hPaintWnd,GWL_STYLE);
  if (bShowBorder)
    style |= WS_BORDER;
  else
    style &= ~WS_BORDER;

  SetWindowLong(hPaintWnd,GWL_STYLE,style);
  SetWindowPos(hPaintWnd,NULL, NULL, NULL, NULL, NULL, SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE);
}

void MOVIEVI::DrawLogo(BOOL bShowLogo)
{
  I_UNUSED(bShowLogo);
}


void MOVIEVI::ShowSpeaker(long details)
{
  long lFlags;

  if (details == EvtCtrlHideSpeaker)
  {
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags |= mcFlagSuppressSpeakerButton;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
  else
  {
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags &= ~mcFlagSuppressSpeakerButton;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
}


void MOVIEVI::ShowSteppers(long details)
{
  long lFlags;

  if (details == EvtCtrlHideSteppers)
  {
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags |= mcFlagSuppressStepButtons;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
  else
  {
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags &= ~mcFlagSuppressStepButtons;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// The filter function for Movie Controller
/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK __export MyFilter (MovieController mController,UINT uAction, LPVOID lpParam, LONG lRefCon)
{
  MOVIERUNPTR lpMovieRunRec;
  MOVIEDEVPTR lpMovieDevRec;
  LPMCTRLDEV  lpMCtrlDev;
  OBJECTID    oiController;
  TimeValue tvCurrent, tvDuration;
  MOVIEVI *MovieVI;
  BOOL bSelectable;
  BOOL rVal;
  long how;
  BOOL bValue;
  HWND hwnd;


      hwnd = (HWND) lRefCon;
      MovieVI = (MOVIEVI *)GetWindowLong(hwnd,0);

 switch (uAction)
  {
    case mcActionPlay:
      if (!MovieVI->IsActivated())
        return TRUE;
      lpMovieRunRec = (MOVIERUNPTR) AObjLockData((OBJECTID)MovieVI->GetMovieID(),MovieRun);
      if (lpParam == 0)       //Rate = 0 - Movie stopped
      {
        MCDoAction(mController,mcActionSetPlaySelection,(LPVOID)FALSE);
        lpMovieRunRec->bIsMoviePlaying = FALSE;
        AEvtPostSignalAtMark((OBJECTID)MovieVI->GetMovieID(), sStopped);
        how = AEVT_ATMARK;
        AEvtPostStandard((OBJECTID)MovieVI->GetMovieID(), AEVT_STOPMOVIE, 0, how);
      }
      else
      {
        lpMovieRunRec->bIsMoviePlaying = TRUE;
        lpMovieRunRec->bIsMovieDone = FALSE;
        AEvtPostSignalAtMark((OBJECTID)MovieVI->GetMovieID(), sStarted);
        how = AEVT_ATTAIL;
        AEvtPostStandard((OBJECTID)MovieVI->GetMovieID(), AEVT_STARTMOVIE, 0, how);
      }
      lpMovieRunRec->lfxRate =(LFIXED) lpParam;
      AObjUnlockData((OBJECTID)MovieVI->GetMovieID(),MovieRun);
      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData((OBJECTID)MovieVI->GetMovieID(),MovieDev);
      AEvtPostSignalAtTail(lpMovieDevRec->oiController, sActionClicked);
      AObjUnlockData((OBJECTID)MovieVI->GetMovieID(),MovieDev);
      rVal = FALSE;
      break;

    case mcActionDraw:
      AEvtPostSignalAtMark(MovieVI->GetMovieID(), sViewChanged);
      rVal = FALSE;
      break;

    case mcActionActivate:
      AEvtPostSignalAtMark(MovieVI->GetMovieID(), sActivated);
      rVal = FALSE;
      break;

    case mcActionDeactivate:
      AEvtPostSignalAtMark(MovieVI->GetMovieID(), sDeactivated);
      rVal = FALSE;
      break;

    case mcActionIdle:
      MCDoAction(mController,mcActionGetLooping,&bValue );
      if (bValue)
      {
        rVal = FALSE;
        break;
      }

      //save data for the last active movie only

      lpMovieRunRec = (MOVIERUNPTR) AObjLockData((OBJECTID)MovieVI->GetMovieID(),MovieRun);
      tvCurrent = MCGetCurrentTime(mController,NULL);
      if (MovieVI->IsActivated())
        lpMovieRunRec->LastClickTime = tvCurrent;
      tvDuration = GetMovieDuration(MCGetMovie(mController));
      MCDoAction(mController,mcActionGetPlayRate,(LPVOID) &(lpMovieRunRec->lfxRate));
      if (((tvCurrent == tvDuration) && (lpMovieRunRec->bIsMoviePlaying) && (lpMovieRunRec->lfxRate > 0)) ||
          ((tvCurrent == 0) && (lpMovieRunRec->bIsMoviePlaying) && (lpMovieRunRec->lfxRate < 0)))

      {
        if (MovieVI->IsActivated())
        {
          lpMovieRunRec->bIsMoviePlaying = FALSE;
          lpMovieRunRec->bIsMovieDone = TRUE;
        }
        AEvtPostSignalAtMark((OBJECTID)MovieVI->GetMovieID(), sDone);
      }
      AObjUnlockData(MovieVI->GetMovieID(),MovieRun);
      rVal = FALSE;
      break;

    case mcActionSetVolume:
      if (MovieVI->IsActivated())
      {
        lpMovieRunRec = (MOVIERUNPTR) AObjLockData((OBJECTID)MovieVI->GetMovieID(),MovieRun);
        lpMovieRunRec->sfxVolume = (SFIXED) lpParam;//0;
        AObjUnlockData((OBJECTID)MovieVI->GetMovieID(),MovieRun);
      }
      rVal = FALSE;
      break;

    case mcActionStep:
      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData((OBJECTID)MovieVI->GetMovieID(),MovieDev);
      AEvtPostSignalAtTail(lpMovieDevRec->oiController, sActionClicked);
      AObjUnlockData((OBJECTID)MovieVI->GetMovieID(),MovieDev);
      rVal = FALSE;
      break;

    case mcActionSetSelectionBegin:
    case mcActionSetSelectionDuration:
      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData((OBJECTID)MovieVI->GetMovieID(),MovieDev);
      oiController = lpMovieDevRec->oiController;
      if (oiController)
      {
        lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
        bSelectable = lpMCtrlDev->bSelectable;
        AObjLockData(oiController,MCtrlDev);
        if (!bSelectable)
        {
          rVal = TRUE;
          break;
        }
      }
      if (MovieVI->IsActivated())
      {
        lpMovieRunRec = (MOVIERUNPTR) AObjLockData((OBJECTID)MovieVI->GetMovieID(),MovieRun);
        if (uAction == mcActionSetSelectionBegin)
          _fmemcpy(&(lpMovieRunRec->trSelBegin),lpParam,sizeof(TimeRecord));
        else
          _fmemcpy(&(lpMovieRunRec->trSelDuration),lpParam,sizeof(TimeRecord));
        AObjUnlockData((OBJECTID)MovieVI->GetMovieID(),MovieRun);
      }

      AEvtPostSignalAtTail(lpMovieDevRec->oiController, sActionClicked);
      AObjUnlockData((OBJECTID)MovieVI->GetMovieID(),MovieDev);
      rVal = FALSE;
      break;

    default:
      return FALSE;
  }

  return rVal;
}

/////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovVIWndProc
// Description:
//   Window procedure of the Movie Visual Instance Window
//
////////////////////////////////////////////////////////////////////////////////
long FAR PASCAL _export MovVIWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
  MovieController mController;
  Movie mMovie = NULL;
  MOVIEDEVPTR lpMovieDevRec;
  LPMOVIESETREC lpMovieSetRec;
  OBJECTID oiMovie = 0;
  LPCREATESTRUCT lpc;
  PAINTSTRUCT ps;
  LPVOID lpv;
  RECT MovWndRect;    //Rectangle of the Movie Window
  RECT rcGrow;
  WORD DisplayMode;
  HDC hdc;
  MOVIEVI *MovieVI;
  long MovieFlags = mcNotVisible;
  BOOL bShowLogo;
  BOOL CtrlMsg = FALSE;
  char Path[255];
  BOOL bEdit;
  WNDENUMPROC lpfnUpdateCtrl;

  MovieVI =(MOVIEVI *) GetWindowLong(hwnd,0);
  if (MovieVI)
  {
    mController = MovieVI->GetMovieController();
    if (MCIsPlayerMessage (mController, hwnd, message, wParam, lParam))
      CtrlMsg = TRUE;
  }

  if (CtrlMsg)
    return 0;

  switch (message)
  {
    case WM_CREATE:
      lpc = (LPCREATESTRUCT) lParam;
      lpv = (lpc->lpCreateParams);
      MovieVI = (MOVIEVI *)lpv;
      oiMovie = MovieVI->GetMovieID();
      GetClientRect(hwnd,&MovWndRect);

      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

//////////////////////////////////////////////////////////////////////////////////////
//Today I must create Movie Controller with mcWithBadge flag and Scaled Display Mode,
//  because of Drawing problem in QuickTime
///////////////////////////////////////////////////////////////////////////////////////
//      lpMovieDevRec->bShowBadge = TRUE;          //!!!!!!!!!!!!!
//      lpMovieDevRec->DisplayMode = RB_SCALED;    //!!!!!!!!!!!!!
//////////////////////////////////////////////////////////////////////////////////////

      if (lpMovieDevRec->bShowBadge)
        MovieFlags |= mcWithBadge;
      else
        MovieFlags &= ~mcWithBadge;

      DisplayMode = lpMovieDevRec->DisplayMode;
      _fmemcpy(Path,lpMovieDevRec->mfMovieName,sizeof(Path));
      AObjUnlockData(oiMovie,MovieDev);

      mController = NewMovieController(NULL,&MovWndRect,MovieFlags,hwnd);

      ///////////////////////////////////////////////////////////////////
      MCDoAction (mController, mcActionGetFlags, &MovieFlags);
      MCDoAction (mController, mcActionSetFlags, (LPVOID)MovieFlags);
      ///////////////////////////////////////////////////////////////////

      MCDoAction(mController,mcActionSetFlags,(LPVOID)mcFlagsUseWindowPalette);

      SetRectEmpty(&rcGrow);
      MCDoAction(mController,mcActionSetGrowBoxBounds,(LPVOID)&rcGrow);
      MCSetActionFilter (mController, MyFilter, (LONG)hwnd);


      MovieVI->SetMovieController(mController);
      MovieVI->SetMovieWnd(hwnd);

      SetWindowLong(hwnd,0,(LONG)MovieVI);

      MovieVI->LoadMovie(Path);
      break;

    case WM_PAINT:
      MovieVI =(MOVIEVI *) GetWindowLong(hwnd,0);
      oiMovie = MovieVI->GetMovieID();
      lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);
      bEdit = lpMovieSetRec->bEdit;
      AObjUnlockData(oiMovie,MovieSet);

      if (bEdit)
        break;

      if (MovieVI)
      {
        mController = MovieVI->GetMovieController();
      }
      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      bShowLogo = lpMovieDevRec->bShowLogo;
      DisplayMode = lpMovieDevRec->DisplayMode;
      _fmemcpy(Path,lpMovieDevRec->mfMovieName,255);
      AObjUnlockData(oiMovie,MovieDev);

      mMovie = MovieVI->GetMovie();

      hdc = BeginPaint(hwnd,&ps);

      if (!mMovie)
      {
        GetClientRect(hwnd,&MovWndRect);
        ShowLogo(hwnd,hdc,oiMovie,bShowLogo,&MovWndRect,DisplayMode);
      }
      else
      {
        GetMovieBox(mMovie,&MovWndRect);  //May be GetClientRec ????????
        DrawMovie(hwnd,oiMovie,mController,mMovie,&MovWndRect,DisplayMode);
        lpfnUpdateCtrl = (WNDENUMPROC) MakeProcInstance((FARPROC)UpdateControllerInstance, hInstance);
        EnumChildWindows(hwnd,lpfnUpdateCtrl,(LPARAM)mController);
        FreeProcInstance((FARPROC)UpdateControllerInstance);
      }
      EndPaint(hwnd,&ps);
      if (MovieVI->IsActivated())
        MovieVI->DrawActBorder(TRUE);
      break;

    case WM_SETMOVIE:
      MovieVI = (MOVIEVI *) GetWindowLong(hwnd,0);

      if (MovieVI)
      {
        oiMovie = MovieVI->GetMovieID();
        mController = MovieVI->GetMovieController();
      }
      else
        break;

      mMovie = MovieVI->GetMovie();
      SetMovie(hwnd, oiMovie,mController,mMovie);
      break;

    case WM_LBUTTONDOWN:
      MovieVI = (MOVIEVI *) GetWindowLong(hwnd, 0);
      if (MovieVI)
        AEvtPostSignalAtMark(MovieVI->GetMovieID(), sSingleClick);
      break;

    case WM_LBUTTONDBLCLK:
      MovieVI = (MOVIEVI *) GetWindowLong(hwnd, 0);
      if (MovieVI)
        AEvtPostSignalAtMark(MovieVI->GetMovieID(), sDoubleClick);
      break;

    case WM_DESTROY:
      MovieVI = (MOVIEVI *) GetWindowLong(hwnd,0);

      MCDoAction (MovieVI->GetMovieController(), mcActionPlay, 0);
      DisposeMovieController(MovieVI->GetMovieController());
      DisposeMovie(MovieVI->GetMovie());
      MovieVI->SetMovie(NULL);
      break;


  }

  return (DefWindowProc(hwnd,message,wParam,lParam));
}


/////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovFrameWndProc
// Description:
//   Window procedure of the Movie Frame Window
//
////////////////////////////////////////////////////////////////////////////////
long FAR PASCAL _export MovFrameWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
  return (DefWindowProc(hwnd,message,wParam,lParam));
}


// Function: UpdateControllerInstance - Updates controller instance in the
//           movie VI Window
// --------------------------------------------------------------------
// Parameters: As required by Microsoft Windows
//
// Returns:    Always TRUE;
// --------------------------------------------------------------------
BOOL __export CALLBACK UpdateControllerInstance( HWND hwnd, LPARAM lParam )
{
    char            szBuffer[50];     // Temp buffer

    I_UNUSED(lParam);

    if( !GetClassName( hwnd, szBuffer, sizeof( szBuffer )))
      return TRUE;

    if (IsWindowVisible(hwnd))
    {
      ShowWindow(hwnd,SW_HIDE);
      ShowWindow(hwnd,SW_SHOW);

      UpdateWindow(hwnd);
    }

    return TRUE;
}


BOOL ALMCALLBACK MuteMovieSound(OpWndItemD *pVI, long lParam1, long lParam2)
{
  MOVIEVI *MovieVI;

  I_UNUSED(lParam2);

  MovieVI = (MOVIEVI *)pVI;


  if (MovieVI != ((MOVIEVI *)lParam1))
    MovieVI->MuteSound();

  return TRUE;
}
