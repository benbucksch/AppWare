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

/////////////////////////////////////////////////////////////////////////////
// File Name:  MoviEdit.CPP
//              Copyright 1993 Novell Inc.
// Description.:
//      The AppWare Movie object's Object Definition Routine used to edit the
//              Movie object's data.
/////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <windows.h>
#include <commdlg.h>
#include <math.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <o_wnd.h>

#include "qtw.h"
#include "moviedlg.h"
#include "moviedat.h"
#include "movieexp.h"
#include "movctrl.h"

typedef struct 
{
  MovieController mController;
  Movie mMovie;
  AOBJECTID oiMovie;

} STATIC_STRUCT;

typedef struct
{
  AOBJECTID oiMovie;
  HWND hMovWnd;
  MOVIEDEVREC OldMovieDevRec;

} DLGSTRUCT;

extern HINSTANCE hInstance;
extern char szMovWndName[];
extern QTInstall;

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  EditMovieDialogProc (HWND hDlg, UINT message,UINT wParam, LONG lParam);
BOOL GetMovieFile(HWND hDlg,AOBJECTID oiMovie, MovieFile  *lpmfMovie);
GetMovieFromFile(MovieFile mfMovie,AOBJECTID oiMovie);
void CalcMovieRect(HWND hDlg, RECT *lprc);
void SetMovie(HWND hMovWnd,AOBJECTID oiMovie,MovieController mController,Movie mMovie);
void DrawMovie(HWND, AOBJECTID, MovieController, Movie, RECT  *,WORD DisplayMode);
void DrawActBorder(HWND, BOOL);
long CALLBACK /*!!PORT!! was _far_ _pascal_*/  MovWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);
HWND CreateMovieWindow(HWND hParent, DWORD WinFlags, RECT rcClient, STATIC_STRUCT *st);

LONG EditObject(AOBJECTID oiMovie)
{
  LPMOVIESETREC lpMovieSetRec;
  DLGSTRUCT *dlgst;

  lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);
    lpMovieSetRec->bEdit = TRUE;
  AObjUnlockData(oiMovie,MovieSet);

  dlgst = new DLGSTRUCT;
  dlgst->oiMovie = oiMovie;
  dlgst->hMovWnd = 0;

  BOOL result = DialogBoxParam( hInstance, "MovieDialog", GetActiveWindow(),
                               (DLGPROC)EditMovieDialogProc, (LONG)dlgst) ;
 
  lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);
    lpMovieSetRec->bEdit = FALSE;
  AObjUnlockData(oiMovie,MovieSet);

  if (result == FALSE)
    return A_OK;

  return A_MODIFIED;
}
  
/////////////////////////////////////////////////////////////////////
//  EditObjectDialogProc
//  Used to edit the data members of the Loan object at design time
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  EditMovieDialogProc (HWND hDlg, UINT message,
                                             UINT wParam, LONG lParam)
{
  LPMOVIESETREC lpMovieSetRec;
  MOVIERUNPTR lpMovieRunRec;
  MOVIEDEVPTR lpMovieDevRec;
//  MOVIEDEVREC OldMovieDevRec;
  AOBJECTID oiMovie;
  DWORD WinFlags = WS_CHILD | WS_VISIBLE;
  STATIC_STRUCT *st;
  HWND hMovWnd = 0;       //Handle of the Movie Window

  char name[255];
  char title[32];
  HWND hIcnCtl;
  HICON hIcn;
  HDC hdc;
  RECT rcMovWnd;
  DLGSTRUCT *dlgst;

  switch (message)
    {
    case WM_INITDIALOG:
      dlgst = (DLGSTRUCT *)lParam;
      oiMovie = dlgst->oiMovie;
      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      CopyMemory(&(dlgst->OldMovieDevRec),lpMovieDevRec,sizeof(MOVIEDEVREC));
      AObjUnlockData(oiMovie,MovieDev);

      AObjGetName(oiMovie,(LPSTR)name,80);
      SetWindowText(hDlg,(LPSTR)name);

      ATypeGetName((ATYPEID)OTYPE_MOVIE,(LPSTR)title,32);
      AUtlSetTitleFont(OTYPE_MOVIE, NULL, GetDlgItem(hDlg, ST_OBJECTNAME));

      hIcnCtl = GetDlgItem(hDlg, MovieIconID);
      hdc = GetDC(hIcnCtl);
      hIcn = LoadIcon(hInstance,"MovieIcon");
      DrawIcon(hdc,0,0,hIcn);
      ReleaseDC(hIcnCtl,hdc);

      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie, MovieDev);

      CheckRadioButton(hDlg,RB_CENTERED,RB_CROPPED,lpMovieDevRec->DisplayMode);

      //Set Display Options
      SendDlgItemMessage(hDlg,CB_BORDER , BM_SETCHECK, lpMovieDevRec->bShowBorder, 0L);
      SendDlgItemMessage(hDlg,CB_ACTBORDER , BM_SETCHECK, lpMovieDevRec->bShowActBorder, 0L);

      //Prevent Badge Drawing today because of controller problems
      //  lpMovieDevRec->bShowBadge = FALSE;
      SendDlgItemMessage(hDlg,CB_BADGE , BM_SETCHECK, lpMovieDevRec->bShowBadge, 0L);
      if ((lpMovieDevRec->DisplayMode == RB_CENTERED) ||
           (lpMovieDevRec->DisplayMode == RB_SIZED)   ||
           (lpMovieDevRec->bShowController))

        EnableWindow(GetDlgItem(hDlg,CB_BADGE),FALSE);
      else
        EnableWindow(GetDlgItem(hDlg,CB_BADGE),TRUE);

      //Set Edit Options
      SendDlgItemMessage(hDlg,CB_EDITABLE , BM_SETCHECK, FALSE, 0L);  //This option is not available yet
      EnableWindow(GetDlgItem(hDlg,CB_EDITABLE),FALSE);
      SendDlgItemMessage(hDlg,CB_SHOWLOGO , BM_SETCHECK, lpMovieDevRec->bShowLogo, 0L);
      SendDlgItemMessage(hDlg,CB_AUTOSTOP , BM_SETCHECK, lpMovieDevRec->bAutoStop, 0L);

      //Set controller visibility
      SendDlgItemMessage(hDlg,CB_SHOWCONTROLLER , BM_SETCHECK, lpMovieDevRec->bShowController, 0L);  //This option is not available yet
 
      CopyMemory(name,lpMovieDevRec->mfMovieName,sizeof(name));

      if (lpMovieDevRec->bShowBorder)
        WinFlags |= WS_BORDER;
      else
        WinFlags &= ~WS_BORDER;

      AObjUnlockData(oiMovie,MovieDev);

      //Create Window for Movie Displaying

      CalcMovieRect(hDlg, &rcMovWnd);

      if (!hMovWnd)
      {
        st = new STATIC_STRUCT;
        st->mController = 0;
        st->mMovie = NULL;
        st->oiMovie = oiMovie;

        hMovWnd = CreateMovieWindow(hDlg,WinFlags, rcMovWnd, st);
      }
      if (!hMovWnd)
        return FALSE;

      lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);
      lpMovieSetRec->bSaved = FALSE;
      AObjUnlockData(oiMovie,MovieSet);

      InvalidateRect(hMovWnd,NULL,TRUE);

      dlgst->hMovWnd = hMovWnd;

      SetWindowLong(hDlg,  DWL_USER, (LONG)dlgst);

      return TRUE ;

    case WM_COMMAND: //!!PORT!!
      switch (wParam)
      {
        case IDOK:
          dlgst = (DLGSTRUCT *)GetWindowLong(hDlg, DWL_USER);
          oiMovie = dlgst->oiMovie;
          hMovWnd = dlgst->hMovWnd;

          SendMessage(hMovWnd,message,wParam,lParam);       //new
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);

          //Set bFlags
          if (IsDlgButtonChecked(hDlg,RB_CENTERED)) {
            lpMovieDevRec->DisplayMode = RB_CENTERED;
          }
          else if (IsDlgButtonChecked(hDlg,RB_SIZED)) {
            lpMovieDevRec->DisplayMode = RB_SIZED;
          }
          else if (IsDlgButtonChecked(hDlg,RB_SCALED)) {
            lpMovieDevRec->DisplayMode = RB_SCALED;
          }
          else
            lpMovieDevRec->DisplayMode = RB_CROPPED;

          //Set Display Options
          lpMovieDevRec->bShowBorder = (BOOL) SendDlgItemMessage(hDlg,CB_BORDER , BM_GETCHECK, 0L, 0L);
          lpMovieDevRec->bShowActBorder = (BOOL) SendDlgItemMessage(hDlg,CB_ACTBORDER , BM_GETCHECK, 0L, 0L);
          lpMovieDevRec->bShowBadge = (BOOL)SendDlgItemMessage(hDlg,CB_BADGE , BM_GETCHECK, 0L, 0L);

          //Set Edit Options
          lpMovieDevRec->bEditable = (BOOL) SendDlgItemMessage(hDlg,CB_EDITABLE , BM_GETCHECK, 0L, 0L);
          lpMovieDevRec->bShowLogo = (BOOL) SendDlgItemMessage(hDlg,CB_SHOWLOGO , BM_GETCHECK, 0L, 0L);
          lpMovieDevRec->bAutoStop = (BOOL) SendDlgItemMessage(hDlg,CB_AUTOSTOP , BM_GETCHECK, 0L, 0L);

          //Set controller visibility
          lpMovieDevRec->bShowController = (BOOL) SendDlgItemMessage(hDlg,CB_SHOWCONTROLLER , BM_GETCHECK, 0, 0L);  //This option is not available yet


          lpMovieSetRec->bSaved = TRUE;
          ////////////////////////////////////////////////////////////////
          lpMovieSetRec->mMovie = NULL;

          AObjUnlockData(oiMovie,MovieSet);
          AObjUnlockData(oiMovie,MovieDev);
          delete dlgst;
          EndDialog(hDlg, TRUE);
          hMovWnd = NULL;
          return TRUE;

        case IDCANCEL:
          dlgst = (DLGSTRUCT *)GetWindowLong(hDlg, DWL_USER);
          oiMovie = dlgst->oiMovie;
          hMovWnd = dlgst->hMovWnd;
          lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);
          lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          CopyMemory(lpMovieDevRec,&(dlgst->OldMovieDevRec),sizeof(MOVIEDEVREC));
          if (lpMovieSetRec->bSaved)
          {
            lpMovieSetRec->bSaved = FALSE;
            lpMovieDevRec->mfMovie = lpMovieSetRec->mfMovie;
            CopyMemory(lpMovieDevRec->mfMovieName,lpMovieSetRec->mfMovieName,256);
            lpMovieRunRec->mMovie = lpMovieSetRec->mMovie;
          }
          AObjUnlockData(oiMovie,MovieDev);
          AObjUnlockData(oiMovie,MovieSet);
          AObjUnlockData(oiMovie,MovieRun);
          DestroyWindow(hMovWnd);
          hMovWnd = NULL;
          SendMessage(hMovWnd,message,wParam,lParam);  
          delete dlgst;
          EndDialog (hDlg, FALSE);
          return TRUE ;

        case IDHELP:
          WinHelp(hDlg,"apwhelp.hlp",HELP_CONTEXT,0);
          return TRUE;

        case CB_BORDER :
          dlgst = (DLGSTRUCT *)GetWindowLong(hDlg, DWL_USER);
          oiMovie = dlgst->oiMovie;
          hMovWnd = dlgst->hMovWnd;
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          lpMovieDevRec->bShowBorder = !(lpMovieDevRec->bShowBorder);
          if (lpMovieDevRec->bShowBorder)
            WinFlags |= WS_BORDER;
          else
            WinFlags &= ~WS_BORDER;
          AObjUnlockData(oiMovie,MovieDev);
          WinFlags = SetWindowLong(hMovWnd,GWL_STYLE,WinFlags);
          ShowWindow(hMovWnd,SW_HIDE);
          ShowWindow(hMovWnd,SW_SHOWNORMAL);
          InvalidateRect(hMovWnd,NULL,TRUE);
          break;

        case CB_ACTBORDER :
          dlgst =(DLGSTRUCT *) GetWindowLong(hDlg, DWL_USER);
          oiMovie = dlgst->oiMovie;
          hMovWnd = dlgst->hMovWnd;
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          lpMovieDevRec->bShowActBorder = !(lpMovieDevRec->bShowActBorder);
          DrawActBorder(hDlg,lpMovieDevRec->bShowActBorder);
          AObjUnlockData(oiMovie,MovieDev);
          break;

        case CB_SHOWCONTROLLER:
          dlgst =(DLGSTRUCT *) GetWindowLong(hDlg, DWL_USER);
          oiMovie = dlgst->oiMovie;
          hMovWnd = dlgst->hMovWnd;
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          lpMovieDevRec->bShowController = !(lpMovieDevRec->bShowController);
          if (lpMovieDevRec->bShowController)
          {
            lpMovieDevRec->bShowBadge = FALSE;
            SendDlgItemMessage(hDlg,CB_BADGE , BM_SETCHECK, lpMovieDevRec->bShowBadge, 0L);
            MCDoAction (lpMovieDevRec->mController, mcActionSetUseBadge,
                    (LPVOID)FALSE);

            EnableWindow(GetDlgItem(hDlg,CB_BADGE),FALSE);
          }
          else
          {
            if ((lpMovieDevRec->DisplayMode == RB_SIZED) || (lpMovieDevRec->DisplayMode == RB_CENTERED))
              EnableWindow(GetDlgItem(GetParent(hDlg),CB_BADGE),FALSE);
            else
              EnableWindow(GetDlgItem(hDlg,CB_BADGE),TRUE);
          }
          lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
          lpMovieRunRec->LastClickTime = MCGetCurrentTime(lpMovieDevRec->mController,NULL);
          AObjUnlockData(oiMovie,MovieRun);
          AObjUnlockData(oiMovie,MovieDev);
          InvalidateRect(hMovWnd,NULL,TRUE);
          break;

        case PB_SETMOVIE:
        case PB_CLEARMOVIE :
        case CB_BADGE:
        case CB_SHOWLOGO:
          dlgst =(DLGSTRUCT *) GetWindowLong(hDlg, DWL_USER);
          oiMovie = dlgst->oiMovie;
          hMovWnd = dlgst->hMovWnd;
          SendMessage(hMovWnd, WM_COMMAND, wParam, lParam); //!!PORT!!
          break;

        case RB_CENTERED:
        case RB_SIZED:
        case RB_SCALED:
        case RB_CROPPED:
          dlgst =(DLGSTRUCT *) GetWindowLong(hDlg, DWL_USER);
          oiMovie = dlgst->oiMovie;
          hMovWnd = dlgst->hMovWnd;
          SendMessage(hMovWnd, WM_COMMAND, wParam, lParam); //!!PORT!!
          ShowWindow(hMovWnd,SW_HIDE);
          ShowWindow(hMovWnd,SW_SHOWNORMAL);

          InvalidateRect(hMovWnd,NULL,TRUE);
          break;
      }
    }
  return FALSE;
}


HWND CreateMovieWindow(HWND hParent, DWORD WinFlags, RECT rcClient, STATIC_STRUCT *st)
{
  return CreateWindow(  szMovWndName,NULL,
                        WinFlags,
                        rcClient.left,
                        rcClient.top,
                        rcClient.right - rcClient.left,
                        rcClient.bottom - rcClient.top,
                        hParent,
                        NULL,
                        hInstance,
                        (LPVOID)st);

}

//////////////////////////////////////////////////////////////////////////
// FUNCTION:
//   
// DESCRIPTION:
/////////////////////////////////////////////////////////////////////////
BOOL GetMovieFile(HWND hDlg,AOBJECTID oiMovie, MovieFile  *lpmfMovie)
{
  OPENFILENAME ofn;
  MOVIEDEVPTR lpMovieDevRec;
  char szDirName [255];
  char szFile [255];
  char szFileTitle [255];
  char errName[255];
  char str[80];
  OSErr err;


  I_UNUSED(hDlg);

  FillMemory (&ofn, 0, sizeof (ofn));
  ofn.lStructSize = sizeof (ofn);
  ofn.hwndOwner = GetActiveWindow();    //hDlg;
  ofn.lpstrFilter = "Movies (*.mov)\0*.mov\0\0";
  ofn.nFilterIndex = 1;

      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      CopyMemory(szFile,lpMovieDevRec->mfMovieName,sizeof(szDirName));
      AObjUnlockData(oiMovie,MovieDev);

  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof (szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof (szFileTitle);
  ofn.lpstrInitialDir = getcwd (szDirName, sizeof (szDirName));
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


  if (GetOpenFileName (&ofn))
  {
    if (!QTInstall)
      return FALSE;

    if (OpenMovieFile (ofn.lpstrFile, lpmfMovie, OF_READ) == noErr)
    {
      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      lpMovieDevRec->mfMovie = *lpmfMovie;
      CopyMemory(lpMovieDevRec->mfMovieName,ofn.lpstrFile,sizeof(szDirName));
      AObjUnlockData(oiMovie,MovieDev);

      err = (GetMoviesError());
      if (err)
      {
        LoadString(hInstance,(UINT)err,str,80);
        wsprintf(errName,"Open MovieFile Failure\n\nQuickTime Error # %li\n\n%s",err,str);
        MessageBox (NULL,errName,NULL, MB_ICONHAND);
      }
      return TRUE;
    }
  }
  else
    return FALSE;

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// FUNCTION:
//   
// DESCRIPTION:
/////////////////////////////////////////////////////////////////////////

void CalcMovieRect(HWND hDlg, RECT *lprc)
{
  HWND hPaintWnd;
  RECT rc1;

// Calculate initial positions of controllers
  hPaintWnd = GetDlgItem(hDlg,ST_MOVIERECT);
//Absolute Coordinates of the White Rectangle Control
  GetWindowRect(hPaintWnd,&rc1);
  MapWindowPoints(NULL, hDlg, (LPPOINT) &rc1, 2);
  InflateRect(&rc1,-15,-15);
  CopyRect(lprc, &rc1);
}

//////////////////////////////////////////////////////////////////////////
// FUNCTION:
//   
// DESCRIPTION:
/////////////////////////////////////////////////////////////////////////
int GetMovieFromFile(MovieFile mfMovie, AOBJECTID oiMovie)
{
  MOVIERUNPTR lpMovieRunRec;
  LPMOVIEFRAMEREC lpMovieFrameRec;
  Movie mMovie;
  OSErr err;
  char errName[255];
  char str[80];
// Extract instance of the movie

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  mMovie = lpMovieRunRec->mMovie;
  if (mMovie)
    DisposeMovie(mMovie);
  lpMovieRunRec->mMovie = NULL;
  AObjUnlockData(oiMovie,MovieRun);

  err = NewMovieFromFile (&mMovie, mfMovie, NULL, NULL,0, NULL);
  
  if (err)//(!mMovie)
  {
    LoadString(hInstance,(UINT)err,str,80);
    wsprintf(errName," NewMoviefromFile Failure\n\nQuickTime Error # %li\n\n%s",err,str);
    MessageBox (NULL,errName,NULL, MB_ICONHAND);
    return 0;
   }
  err = CloseMovieFile(mfMovie); 

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    lpMovieRunRec->mMovie = mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
  GetMovieBox(mMovie,&(lpMovieFrameRec->SetMovieRect));

  AObjUnlockData(oiMovie,MovieFrame);
  return 1;
}

//////////////////////////////////////////////////////////////////////////
// FUNCTION:
//   SetMovie
// DESCRIPTION:
/////////////////////////////////////////////////////////////////////////
void SetMovie(HWND hMovWnd,
               AOBJECTID oiMovie,
               MovieController mController,
               Movie mMovie)
{
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  POINT ptMovie;
  RECT rcMovie;
  LFIXED lfxRate;
  TimeRecord trRecord;
  if (!mMovie)   //Movie wasn't set
    return;

  GetMovieBox(mMovie,&rcMovie);
  ptMovie.x = -rcMovie.right;
  ptMovie.y = -rcMovie.bottom;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  trRecord.value.dwLo = lpMovieRunRec->LastClickTime;
  trRecord.value.dwHi = 0;
  trRecord.scale = GetMovieTimeScale (mMovie);
  lfxRate = lpMovieRunRec->lfxRate;
  AObjUnlockData(oiMovie,MovieRun);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->mController = mController;
  AObjUnlockData(oiMovie,MovieDev);

  SetRectEmpty(&rcMovie);
  SetMovieBox(mMovie,&rcMovie);

  MCSetMovie (mController, mMovie, hMovWnd, ptMovie);
  MCDoAction (mController, mcActionPlay, (LPVOID)lfxRate);


  MCDoAction(mController, mcActionGoToTime,(LPVOID) &trRecord);

  return;
}

//////////////////////////////////////////////////////////////////////////
// FUNCTION:
//   DrawMovie
// DESCRIPTION:
/////////////////////////////////////////////////////////////////////////
void DrawMovie(HWND hMovWnd,
               AOBJECTID oiMovie,
               MovieController mController,
               Movie mMovie,
               RECT  * lprcMovieBox,
               WORD DisplayMode)
{
  MOVIEDEVPTR lpMovieDevRec;
  LPMOVIEFRAMEREC lpMovieFrameRec;
  RECT rcMovie, rcMov, rcCtrlBounds, rcClient;
  BOOL bShowController;
  int dx, dy;
  double Scale;

  if (!mMovie)   //Movie wasn't set
    return;

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  bShowController = lpMovieDevRec->bShowController;
  AObjUnlockData(oiMovie,MovieDev);

  if (bShowController)
  {
    MCSetVisible(mController,TRUE);
    MCSetControllerAttached(mController,TRUE);
  }
  else
  {
    MCSetVisible(mController,FALSE);
    MCSetControllerAttached(mController,FALSE);
  }

  switch (DisplayMode)
  {
    case RB_CENTERED :
    ////////////////////////////////////////
    // Centered
    ////////////////////////////////////////
      GetClientRect(hMovWnd,&rcClient);
      lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
      CopyRect(&rcMovie,&(lpMovieFrameRec->SetMovieRect));
      
      dx = (rcClient.right - rcMovie.right)/2;
      dy = (rcClient.bottom - rcMovie.bottom)/2;
      OffsetRect(&rcMovie,dx,dy);
      SetMovieBox(mMovie,&rcMovie);
      CopyRect(lprcMovieBox,&rcMovie);

      AObjUnlockData(oiMovie,MovieFrame);

      MCPositionController(mController,lprcMovieBox,&rcMovie/*rcCtrlBounds*/,0);
      /////////////////////////////////////////////////////////////////

      break;

    case RB_SIZED :
    /////////////////////////////////////////
    // Sized
    /////////////////////////////////////////                      
      GetClientRect(hMovWnd,&rcClient);
      lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
      CopyRect(&rcMovie,&(lpMovieFrameRec->SetMovieRect));

      SetMovieBox(mMovie,&rcMovie);
      CopyRect(lprcMovieBox,&rcClient);
      MCGetControllerBoundsRect(mController,&rcCtrlBounds);

      CopyRect(&(lpMovieFrameRec->RunMovieRect),lprcMovieBox);
      AObjUnlockData(oiMovie,MovieFrame);

      MCPositionController(mController,lprcMovieBox,&rcCtrlBounds,0);
    ////////////////////////////////////////

      break;

    case RB_SCALED :
    ////////////////////////////////////////
    // Scaled
    ////////////////////////////////////////
      lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
      GetClientRect(hMovWnd,&rcMovie);
      SetMovieBox(mMovie,&rcMovie);
      CopyRect(lprcMovieBox,&rcMovie);
      if (bShowController)
      {
        CopyRect(&(lpMovieFrameRec->RunMovieRect),lprcMovieBox);

        MCPositionController(mController,lprcMovieBox,&rcMovie,mcScaleMovieToFit);
      }
      else
      {
        CopyRect(&(lpMovieFrameRec->RunMovieRect),&rcMovie);   

        MCPositionController(mController,NULL,&rcMovie,mcScaleMovieToFit);
      }
      AObjUnlockData(oiMovie,MovieFrame);
    ////////////////////////////////////////

      break;

    case RB_CROPPED :
    ////////////////////////////////////////
    // Cropped
    ////////////////////////////////////////
      GetClientRect(hMovWnd,&rcClient);
      lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
      Scale = lpMovieFrameRec->Scale;
      CopyRect(&rcMovie,&(lpMovieFrameRec->SetMovieRect));
      rcMovie.right *= Scale;
      rcMovie.bottom *= Scale;
      CopyRect(&rcMov,&rcMovie);

      SetMovieBox(mMovie,&rcMovie);
      OffsetRect(&rcMov,lpMovieFrameRec->HScroll,lpMovieFrameRec->VScroll);
      CopyRect(lprcMovieBox,&rcMov);
      MCGetControllerBoundsRect(mController,&rcCtrlBounds);

      CopyRect(&(lpMovieFrameRec->RunMovieRect),&rcMov);   

      AObjUnlockData(oiMovie,MovieFrame);

      MCPositionController(mController,&rcMov,&rcCtrlBounds,mcTopLeftMovie);
    ////////////////////////////////////////

      break;

  }
  SetMovieActive (mMovie, TRUE);
  return;
}

void DrawActBorder(HWND hwnd, BOOL bShowActBorder)
{
  RECT ghostRect;
  HBRUSH hBrush;
  POINT inflatePt;
  HDC hdc;
  HWND hw;

  if (bShowActBorder)
  {
    hw = GetDlgItem(hwnd,ST_MOVIERECT);
    hBrush = CreateSolidBrush(BLACK_BRUSH);
    inflatePt.x = -10;
    inflatePt.y = -10;
    GetClientRect(hw,&ghostRect);
    InflateRect(&ghostRect, inflatePt.x, inflatePt.y);
    hdc = GetDC(hw);
    int sDC = SaveDC(hdc);
    int mode = SetMapMode(hdc,MM_TEXT);
    FrameRect(hdc, &ghostRect, hBrush);
    InflateRect(&ghostRect, -1, -1);
    FrameRect(hdc, &ghostRect, hBrush);
    SetMapMode(hdc, mode);
    RestoreDC(hdc, sDC);
    if (hBrush)
      DeleteObject(hBrush);
    ReleaseDC(hw, hdc);
  }
  else
  {
    hw = GetDlgItem(hwnd,ST_MOVIERECT);
    hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    inflatePt.x = -10;
    inflatePt.y = -10;
    GetClientRect(hw,&ghostRect);
    InflateRect(&ghostRect, inflatePt.x, inflatePt.y);
    hdc = GetDC(hw);
    int sDC = SaveDC(hdc);
    int mode = SetMapMode(hdc,MM_TEXT);
    FrameRect(hdc, &ghostRect, hBrush);
    InflateRect(&ghostRect, -1, -1);
    FrameRect(hdc, &ghostRect, hBrush);
    SetMapMode(hdc, mode);
    RestoreDC(hdc, sDC);
    if (hBrush)
      DeleteObject(hBrush);
    ReleaseDC(hw, hdc);

  }

  return;
}

//////////////////////////////////////////////////
// Draw Badge Image on the Logo Picture
//////////////////////////////////////////////////
void DrawBadge(HWND hwnd, HDC hdc, AOBJECTID oiMovie ,RECT rc)
{
  HDC hdcMemory;
  RECT rcClient, rcBadge;
  HBITMAP hBmp, hBmpOld;
  BITMAP bm;
  MOVIEDEVPTR lpMovieDevRec;
  BOOL bShowBadge;

  lpMovieDevRec = (MOVIEDEVPTR)AObjLockData(oiMovie,MovieDev);
  bShowBadge = lpMovieDevRec->bShowBadge;
  AObjUnlockData(oiMovie,MovieDev);

  if (!bShowBadge)
    return;

  hBmp = LoadBitmap(hInstance,MAKEINTRESOURCE(BADGE));
  GetClientRect(hwnd,&rcClient);
  
  hdcMemory = CreateCompatibleDC(hdc);
  hBmpOld = (HBITMAP)SelectObject(hdcMemory, hBmp);

  GetObject (hBmp, sizeof (BITMAP), (LPSTR) &bm);

  rcBadge.left = rc.left * 0.2 + 5;
  rcBadge.top = rc.bottom * 0.8;
  rcBadge.right = rcBadge.left + bm.bmWidth;
  rcBadge.bottom = rcBadge.top + bm.bmHeight;

  BitBlt(hdc,rcBadge.left,rcBadge.top,rcBadge.right,rcBadge.bottom,
    hdcMemory, 0, 0, SRCCOPY);

  SelectObject(hdcMemory, hBmpOld);

  DeleteDC(hdcMemory);

  DeleteObject(hBmp);
}

VOID DrawWhiteRect(HDC hdc,int cxClient,int cyClient)
   {
   HBRUSH hBrushFirst;
   HPEN   hPenFirst;
   // Clear DC by drawing white rectangle on it
   hPenFirst = (HPEN)SelectObject(hdc,(HPEN)WHITE_PEN);
   hBrushFirst = (HBRUSH)SelectObject(hdc,(HBRUSH)WHITE_BRUSH);
   Rectangle(hdc,0,0,cxClient,cyClient);
   SelectObject(hdc,hBrushFirst);
   SelectObject(hdc,hPenFirst);
   }


void ShowLogo(HWND hwnd, HDC hdc, AOBJECTID oiMovie, BOOL bShowLogo, RECT *rcClient, WORD DisplayMode)
{
  HDC hdcMemory;
  DWORD Alpha, Beta;
  RECT rcBMP, rcPict;
  HBITMAP hBmp, hBmpOld;
  BITMAP bm;

  if (!bShowLogo)
    return;

  hBmp = LoadBitmap(hInstance,MAKEINTRESOURCE(QTLOGO));
  if (!hBmp)
    return;

  hdcMemory = CreateCompatibleDC(hdc);
  hBmpOld = (HBITMAP)SelectObject(hdcMemory, hBmp);

  GetObject (hBmp, sizeof (BITMAP), (LPSTR) &bm);

  SetRect(&rcPict,0,0,bm.bmWidth,bm.bmHeight);

//  ratio = bm.bmHeight / bm.bmWidth;                   // aspect ration of the picture
  Alpha = bm.bmWidth / rcClient->right;         // PictureWidth / WindowWidth
  Beta = bm.bmHeight / rcClient->bottom;        // PictureHeight / WindowHeight

  switch (DisplayMode)
  {
    case RB_CENTERED:
      rcBMP.left = (rcClient->right - bm.bmWidth*1.2) / 2;
      rcBMP.top = (rcClient->bottom - bm.bmHeight*1.2) / 2;
      rcBMP.right = bm.bmWidth*1.2;   //width
      rcBMP.bottom = bm.bmHeight*1.2; //height

      break;

    case RB_SIZED:
      if ((Alpha <= 1) && (Beta <= 1))
      {
        rcBMP.left = (rcClient->right - bm.bmWidth) / 2;
        rcBMP.top = (rcClient->bottom - bm.bmHeight) / 2;
        rcBMP.right = bm.bmWidth;   //width
        rcBMP.bottom = bm.bmHeight; //height
      }
      else if (Alpha > Beta) {
      //Compress by width
        rcBMP.left = (int) (rcClient->right - bm.bmWidth / Alpha) / 2;
        rcBMP.top = (int) (rcClient->bottom - bm.bmHeight / Alpha) / 2;
        rcBMP.right = (int) (bm.bmWidth / Alpha);   //width
        rcBMP.bottom = (int) (bm.bmHeight / Alpha); //height
      }
      else {
      //Compress by height
        rcBMP.left = (int) (rcClient->right - bm.bmWidth / Beta) / 2;
        rcBMP.top = (int)(rcClient->bottom - bm.bmHeight / Beta) / 2;
        rcBMP.right =(int) (bm.bmWidth / Beta);   //width
        rcBMP.bottom =(int) (bm.bmHeight / Beta); //height
      }

      break;

    case RB_SCALED:
      CopyRect(&rcBMP,rcClient);
      break;

    case RB_CROPPED:
      rcBMP.left = 0;
      rcBMP.top = 0;
      rcBMP.right = bm.bmWidth;
      rcBMP.bottom = bm.bmHeight;

      break;
  }

  StretchBlt(hdc,rcBMP.left,rcBMP.top,rcBMP.right,rcBMP.bottom,
    hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

  SelectObject(hdcMemory,hBmpOld);
  DeleteDC(hdcMemory);

  DeleteObject(hBmp);

  DrawBadge(hwnd, hdc, oiMovie, rcBMP);
}


/////////////////////////////////////////////////////////////////////////////////
//Function:
//  Window procedure of the Movie Window
//
////////////////////////////////////////////////////////////////////////////////
long CALLBACK /*!!PORT!! was _far_ _pascal_*/  MovWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
  STATIC_STRUCT *st;
  MovieController mController = 0;
  MovieFile mfMovie;
  Movie mMovie = NULL;
  MOVIEDEVPTR lpMovieDevRec;
  LPMOVIESETREC lpMovieSetRec;
  MOVIERUNPTR lpMovieRunRec;
  AOBJECTID oiMovie = 0;
  LPCREATESTRUCT lpc;
  PAINTSTRUCT ps;
  LPVOID lpv;
  RECT MovWndRect;    //Rectangle of the Movie Window
  long MovieFlags = mcNotVisible;
  WORD DisplayMode;
  BOOL bShowLogo, bShowBadge;
  BOOL bShowActBorder;
  HDC hdc;
  RECT rcGrow;
  char Path[255], str[80], errName[80];
  OSErr err;

  //retrieve all "static" data
  st = (STATIC_STRUCT *)GetWindowLong(hwnd,0);
  if (st != 0)
    mController = st->mController;

  if (QTInstall && mController)
    if (MCIsPlayerMessage (mController, hwnd, message, wParam, lParam))
      return 0;     

  switch (message)
  {
    case WM_CREATE:
      lpc = (LPCREATESTRUCT) lParam;
      lpv = (lpc->lpCreateParams);
      st = (STATIC_STRUCT *)lpv;
      oiMovie = st->oiMovie;

      GetClientRect(hwnd,&MovWndRect);

      if (SendDlgItemMessage(GetParent(hwnd),CB_BADGE , BM_GETCHECK, 0L, 0L))
        MovieFlags |= mcWithBadge;
      else
        MovieFlags &= ~mcWithBadge;

      if (QTInstall) {         //04.04.94
        mController = NewMovieController(NULL,&MovWndRect,MovieFlags,hwnd);
        SetRectEmpty(&rcGrow);
        MCDoAction(mController,mcActionSetGrowBoxBounds,(LPVOID)&rcGrow);
      }

      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      bShowBadge = lpMovieDevRec->bShowBadge;
      CopyMemory(Path,lpMovieDevRec->mfMovieName,sizeof(Path));  
      AObjUnlockData(oiMovie,MovieDev);

      if (!bShowBadge)
        MCDoAction(mController,mcActionSetFlags,(LPVOID)mcFlagsUseWindowPalette);

      lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
      mMovie = lpMovieRunRec->mMovie;
      AObjUnlockData(oiMovie,MovieRun);

      if ((err = OpenMovieFile (Path, &mfMovie, OF_READ)) == noErr)
      {
            err = NewMovieFromFile (&mMovie, mfMovie, NULL, NULL,0, NULL);
            if (err)//(!mMovie)
            {
              LoadString(hInstance,(UINT)err,str,80);
              wsprintf(errName," NewMovieFromFile Failure\n\nQuickTime Error # %li\n\n%s",err,str);
              MessageBox (NULL,errName,NULL, MB_ICONHAND);
              return 0;
            }
         CloseMovieFile(mfMovie);
      }

      SetMovie(hwnd, oiMovie,mController,mMovie);
      //Stop Movie
      MCDoAction(mController,mcActionPlay,0);

      st->mController = mController;
      st->mMovie = mMovie;
     // st->oiMovie = oiMovie;         already set outside the window
      SetWindowLong(hwnd,0,(LONG)st);
      
      break;

    case WM_PAINT:
      st = (STATIC_STRUCT *)GetWindowLong(hwnd, 0);
      oiMovie = st->oiMovie;
      mMovie = st->mMovie;
      mController = st->mController;

      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      DisplayMode = lpMovieDevRec->DisplayMode;
      bShowLogo = lpMovieDevRec->bShowLogo;
      bShowActBorder = lpMovieDevRec->bShowActBorder;
      bShowBadge = lpMovieDevRec->bShowBadge;
      AObjUnlockData(oiMovie,MovieDev);

      DrawActBorder(GetParent(hwnd),bShowActBorder);
      GetClientRect(hwnd,&MovWndRect);

      hdc = BeginPaint(hwnd,&ps);                                                        

      if (!mMovie) 
      {
        ShowLogo(hwnd,hdc,oiMovie,bShowLogo,&MovWndRect,DisplayMode);
        EndPaint(hwnd,&ps);
        break;
      }
      else
         DrawMovie(hwnd,oiMovie,mController,mMovie,&MovWndRect,DisplayMode); //new

      EndPaint(hwnd,&ps);

      break;

    case WM_DESTROY:
      st = (STATIC_STRUCT *)GetWindowLong(hwnd, 0);
      oiMovie = st->oiMovie;
      mMovie = st->mMovie;
      mController = st->mController;

      MCDoAction (mController, mcActionPlay, 0);
      MCDoAction (mController, mcActionSetVolume, (LPVOID) 0);

      lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
      lpMovieRunRec->LastClickTime = MCGetCurrentTime(mController,NULL);
      AObjUnlockData(oiMovie,MovieRun);
      DisposeMovie(mMovie);
      DisposeMovieController(mController);
      delete st;
      SetWindowLong(hwnd,0,(LONG) 0);     //need to prevent crash at the next window message
      mController = 0;
      mMovie = 0;
      break;


    case WM_COMMAND: //!!PORT!!
      st = (STATIC_STRUCT *)GetWindowLong(hwnd, 0);
      oiMovie = st->oiMovie;
      mMovie = st->mMovie;
      mController = st->mController;

      switch (wParam)
      {
        case IDOK:
          
          if (!mMovie)
          {
            lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
            lpMovieDevRec->mfMovieName[0] = '\0';
            AObjUnlockData(oiMovie,MovieDev);
            lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
            DisposeMovie(lpMovieRunRec->mMovie);
            lpMovieRunRec->mMovie = NULL;
            AObjUnlockData(oiMovie,MovieRun);
          }
          break;

        case IDCANCEL:
          break;

        case PB_SETMOVIE:
          //Get existing Movie File if any for closing it
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          mfMovie = lpMovieDevRec->mfMovie;
          CopyMemory(Path,lpMovieDevRec->mfMovieName,255);
          AObjUnlockData(oiMovie,MovieDev);
          //if Movie file was already set for this object close it

          lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);
          lpMovieSetRec->mfMovie = mfMovie;
          CopyMemory(lpMovieSetRec->mfMovieName,Path,255);
          lpMovieSetRec->mMovie = mMovie;
          AObjUnlockData(oiMovie,MovieSet);

          if (GetMovieFile(hwnd,oiMovie,&mfMovie))
          {
            lpMovieSetRec = (LPMOVIESETREC) AObjLockData(oiMovie,MovieSet);
            lpMovieSetRec->bSaved = TRUE;
            AObjUnlockData(oiMovie,MovieSet);

            if (mMovie)
            {
              DisposeMovie(mMovie);
              mMovie = NULL;
            }
            
            err = NewMovieFromFile (&mMovie, mfMovie, NULL, NULL,0, NULL);
            if (err)//(!mMovie)
            {
              LoadString(hInstance,(UINT)err,str,80);
              wsprintf(errName," NewMoviefromFile Failure\n\nQuickTime Error # %li\n\n%s",err,str);
              MessageBox (NULL,errName,NULL, MB_ICONHAND);
              return 0;
             }
            CloseMovieFile(mfMovie); 
            
            st->mMovie = mMovie;     //update movie in st structure

            lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
            lpMovieDevRec->mfMovie = mfMovie;
            bShowBadge = lpMovieDevRec->bShowBadge;
            AObjUnlockData(oiMovie,MovieDev);
            SetMovie(hwnd, oiMovie,mController,mMovie);
            if (!bShowBadge)
              MCDoAction(mController,mcActionSetFlags,(LPVOID)mcFlagsUseWindowPalette);
            InvalidateRect(hwnd,NULL,TRUE);
          }
          break;

        case PB_CLEARMOVIE :
        //Clear Movie from the Movie Object
          DisposeMovie(mMovie);
          mMovie = NULL;               
          st->mMovie = mMovie;          //Update movie in the st structure

          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          if (lpMovieDevRec->bShowController)
            MCSetVisible(mController,FALSE);
          AObjUnlockData(oiMovie,MovieDev);
          ShowWindow(hwnd,SW_HIDE);
          ShowWindow(hwnd,SW_SHOWNORMAL);
          InvalidateRect(hwnd,NULL,TRUE);
        
          break;

        case CB_BADGE:
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          lpMovieDevRec->bShowBadge = !(lpMovieDevRec->bShowBadge);
          AObjUnlockData(oiMovie,MovieDev);

          if (!mMovie)
            InvalidateRect(hwnd,NULL,TRUE);                               

          MCDoAction (mController, mcActionSetUseBadge,
                      (LPVOID)SendDlgItemMessage(GetParent(hwnd),CB_BADGE , BM_GETCHECK, 0L, 0L));
          break;

        case RB_CENTERED:
        case RB_SIZED:
        case RB_SCALED:
        case RB_CROPPED:
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          bShowBadge = lpMovieDevRec->bShowBadge;
          lpMovieDevRec->DisplayMode = (WORD)wParam;
          if ((lpMovieDevRec->DisplayMode == RB_CENTERED) || (lpMovieDevRec->DisplayMode == RB_SIZED))
          {
            lpMovieDevRec->bShowBadge = FALSE;
            SendDlgItemMessage(GetParent(hwnd),CB_BADGE , BM_SETCHECK, lpMovieDevRec->bShowBadge, 0L);
            EnableWindow(GetDlgItem(GetParent(hwnd),CB_BADGE),FALSE);
            MCDoAction (mController, mcActionSetUseBadge,
                      (LPVOID)SendDlgItemMessage(GetParent(hwnd),CB_BADGE , BM_GETCHECK, 0L, 0L));

          }
          else if (!lpMovieDevRec->bShowController)
          {
            EnableWindow(GetDlgItem(GetParent(hwnd),CB_BADGE),TRUE);
            SendDlgItemMessage(GetParent(hwnd),CB_BADGE , BM_SETCHECK, lpMovieDevRec->bShowBadge, 0L);
            MCDoAction (mController, mcActionSetUseBadge,
                      (LPVOID)SendDlgItemMessage(GetParent(hwnd),CB_BADGE , BM_GETCHECK, 0L, 0L));
          }
          AObjUnlockData(oiMovie,MovieDev);
          break;

        case CB_SHOWLOGO:
          lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
          lpMovieDevRec->bShowLogo = !(lpMovieDevRec->bShowLogo);
          AObjUnlockData(oiMovie,MovieDev);
          InvalidateRect(hwnd,NULL,TRUE);

          break;
      }
  }
  return (DefWindowProc(hwnd,message,wParam,lParam));
}
