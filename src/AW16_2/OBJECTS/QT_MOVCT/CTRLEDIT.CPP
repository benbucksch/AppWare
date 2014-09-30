/////////////////////////////////////////////////////////////////////////////
// File Name:  MovieControllerEdit.CPP
// 		Copyright 1993 Novell Inc.
// Description.:
//	The AppWare Movie Controller object's Object Definition Routine used to edit the
//		Movie Controller object's data.
/////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>

#include "movctrl.h"
#include "moviedat.h"
#include "qtw.h"

extern HINSTANCE hInstance;
extern MovieFile mfMovie;
Movie mMovie;
MovieController mController;
long lMCInfoFlags;
TimeValue tvDuration;
HWND hCtrlBox;
extern QTInstall;
//extern QTDrive;

HBITMAP hSpeaker,
	hArea,
	hAreaSel,
	hSteppers,
	hBmpOld;

BOOL FAR PASCAL _export EditCtrlDlgProc (HWND hDlg, UINT message,
					     UINT wParam, LONG lParam);
BOOL InitDialog(HWND, OBJECTID theObject);
BOOL UpdateObject(HWND);

LONG EditObject( OBJECTID theObject )
{
  BOOL result = DialogBoxParam( hInstance, "CtrlDlg", GetActiveWindow(),
		  (DLGPROC)EditCtrlDlgProc, (LONG)theObject) ;
  if (result == FALSE)
    return A_OK;

  return A_MODIFIED;
}

/////////////////////////////////////////////////////////////////////
//  EditObjectDialogProc
//  Used to edit the data members of the Loan object at design time
/////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export EditCtrlDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
  RECT MCtrlRect;

  switch (message)
  {
    case WM_INITDIALOG:
      return InitDialog(hDlg, (OBJECTID)lParam) ;

    case WM_COMMAND:
      switch (wParam)
      {
	case IDOK:
	  return UpdateObject(hDlg) ;

	case IDCANCEL:
	  EndDialog (hDlg, FALSE) ;
          return TRUE;

	case IDHELP:
	  WinHelp(hDlg,"c:\\appware\\help\\apwhelp.hlp",HELP_CONTEXT,0);
	  return TRUE;

	case CB_SHOWSPEAKER:
	case CB_ALLOWSELECTIONS:
	case CB_SHOWSTEPPERS:
	  GetClientRect(hCtrlBox,&MCtrlRect);
	  InvalidateRect(hCtrlBox,&MCtrlRect,TRUE);

	  return TRUE;

        case CB_DRAWINCOLOR:
	  return TRUE;
      }
  }
  return FALSE ;
}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the Loan Object's edit dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL InitDialog ( HWND hDlg, OBJECTID theObject)
{
  HWND hWnd;
  char name[255];
  char title[20];
  LPMCTRLDEV lpMCtrlDev;
  RECT MCtrlRect, rcClient;
  char buffer[40] ;
  char *nonestr = "None";
  OBJECTID oiToAdd, oiMovieSave ;
  int objectIndex ;
  long key;
  HDC hdc;
  HWND hIcnCtl;
  HICON hIcn;
  BITMAP bm;
  int shift, height;

  SetWindowLong(hDlg, DWL_USER, (LONG)theObject); // save the object ID in the window extra bytes
  AObjGetName(theObject,(LPSTR)name,80);
  SetWindowText(hDlg,(LPSTR)name);

  ATypeGetName((TYPEID)OTYPE_CONTROLLER,(LPSTR)title,32);
  AUtlSetTitleFont(OTYPE_CONTROLLER, NULL, GetDlgItem(hDlg, ST_MCTRLNAME));

  hIcnCtl = GetDlgItem(hDlg, MovieIconID);
  hdc = GetDC(hIcnCtl);
  hIcn = LoadIcon(hInstance,"MovieIcon");
  DrawIcon(hdc,0,0,hIcn);
  ReleaseDC(hIcnCtl,hdc);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(theObject, MCtrlDev);

  //Set Controller
  SendDlgItemMessage(hDlg,CB_SHOWSPEAKER , BM_SETCHECK, lpMCtrlDev->bSpeaker, 0L);
  SendDlgItemMessage(hDlg,CB_SHOWSTEPPERS , BM_SETCHECK, lpMCtrlDev->bStepper, 0L);

  SendDlgItemMessage(hDlg,CB_ALLOWSELECTIONS , BM_SETCHECK, lpMCtrlDev->bSelectable, 0L);
  SendDlgItemMessage(hDlg,CB_DRAWINCOLOR , BM_SETCHECK, lpMCtrlDev->bColor, 0L);


  //Set Controller Static control pre-sizes ------------------------------------
  hWnd = GetDlgItem(hDlg,105);   //BorShade
  GetWindowRect(hWnd,&MCtrlRect);
  MapWindowPoints(NULL,hDlg,(LPPOINT)&MCtrlRect,2);
  GetClientRect(hDlg,&rcClient);
  SetRect(&MCtrlRect,MCtrlRect.left,MCtrlRect.bottom,MCtrlRect.right,rcClient.bottom);

  hCtrlBox = CreateWindow("ControlBox",NULL,WS_CHILD,
				MCtrlRect.left,MCtrlRect.top,
				MCtrlRect.right-MCtrlRect.left,
				MCtrlRect.bottom-MCtrlRect.top,
				hDlg,NULL,hInstance,(LPVOID)theObject);

 // Resize Controller static box//////////////////////////////////////
  GetObject (hSpeaker, sizeof (BITMAP), (LPSTR) &bm);
  height = MCtrlRect.bottom - MCtrlRect.top;
  shift = (height - bm.bmHeight) / 2;
  MCtrlRect.top += shift;
  MCtrlRect.bottom -= shift;

  MoveWindow(hCtrlBox,MCtrlRect.left,MCtrlRect.top,
		MCtrlRect.right - MCtrlRect.left,
		MCtrlRect.bottom - MCtrlRect.top,TRUE);

  ShowWindow(hCtrlBox,SW_SHOW);

  oiMovieSave = lpMCtrlDev->oiMovie;   //!!!!!!!!

  lpMCtrlDev->oiMovie = 0;             //!!!!!!!!

  lpMCtrlDev->oiMovieShared = 0;       //!!!!!!!!
  lpMCtrlDev->bMovieAlias = FALSE;     //!!!!!!!!

  //Set List of Movies if any
  strcpy(buffer,nonestr);

  //Set None string to the ListBox
  objectIndex = (int)SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_ADDSTRING, 0, (long)buffer);
  SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_SETITEMDATA, objectIndex, 0L);

  //Set Movie Objects are plced in the current subject to the List Box
  key = AOBJ_GETFIRSTKEY ;
  while((oiToAdd = AObjGetNextObject(OTYPE_MOVIE, &key)) != 0)
  {
    AObjGetName(oiToAdd, buffer, 40) ;
    objectIndex = (int)SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_ADDSTRING, 0, (long)buffer);
    SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_SETITEMDATA, objectIndex, (long)oiToAdd);
    if (oiToAdd == oiMovieSave)
    {
      lpMCtrlDev->oiMovie = oiMovieSave;
      SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_SETCURSEL, objectIndex, 0) ;
    }
  }

  if (!lpMCtrlDev->oiMovie)
    SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_SETCURSEL, 0, 0) ;

  AObjUnlockData(theObject,MCtrlDev);

  return TRUE ;
}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  UpdateObject
// DESCRIPTION:  Updates the Loan object's data.
/////////////////////////////////////////////////////////////////////////////
BOOL  UpdateObject(HWND hDlg)
{
  OBJECTID theObject ;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  MOVIEDEVPTR lpMovieDevRec;
  int index;

  theObject = (OBJECTID)GetWindowLong (hDlg, DWL_USER) ;
  if (!theObject)
  {
    EndDialog(hDlg,FALSE);
    return TRUE;
  }

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(theObject, MCtrlDev);

  index = (int)SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_GETCURSEL, 0, 0) ;

  if (index == CB_ERR)
     lpMCtrlDev->oiMovie = 0 ;
  else
    oiMovie = (OBJECTID)SendDlgItemMessage(hDlg, CB_MOVIEASSIGNED, CB_GETITEMDATA, index, 0) ;

  if (lpMCtrlDev->oiMovie)
  {
    lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(lpMCtrlDev->oiMovie,MovieDev);
    lpMovieDevRec->oiController = 0;
    AObjUnlockData(lpMCtrlDev->oiMovie,MovieDev);
  }

  lpMCtrlDev->oiMovie = oiMovie;
  lpMCtrlDev->oiMovieShared = oiMovie;       //
  if (oiMovie)
  {
    lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
    lpMovieDevRec->oiController = theObject;
    if (lpMovieDevRec->oiSelf != oiMovie)
    {
      lpMovieDevRec->bShared = TRUE;
      lpMCtrlDev->oiMovieShared = lpMovieDevRec->oiSelf;   //refer to original Movie object
      lpMCtrlDev->bMovieAlias = TRUE;                           //Assigned Movie is an alias object
    }
    AObjUnlockData(oiMovie,MovieDev);
  }

  lpMCtrlDev->bSpeaker = (int)SendDlgItemMessage(hDlg,CB_SHOWSPEAKER , BM_GETCHECK, 0L, 0L);
  lpMCtrlDev->bSelectable = (int)SendDlgItemMessage(hDlg,CB_ALLOWSELECTIONS , BM_GETCHECK, 0L, 0L);
  lpMCtrlDev->bStepper = (int)SendDlgItemMessage(hDlg,CB_SHOWSTEPPERS , BM_GETCHECK, 0L, 0L);
  lpMCtrlDev->bColor = (int)SendDlgItemMessage(hDlg,CB_DRAWINCOLOR , BM_GETCHECK, 0L, 0L);
  AObjUnlockData(theObject,MCtrlDev);
  EndDialog (hDlg, TRUE) ;
  return TRUE ;
}

void DrawController(OBJECTID oiController, HWND hCtrlBox, HDC hdc)
{
  HBITMAP  hSpeaker,
	   hPlayArea,
	   hAreaSel,
	   hSteppers,
	   hBmpOld;


  BITMAP bm;
  HDC hdcMemory;
  RECT rcBMP;
  RECT MCtrlRect;
  int shift, height;

  LPMCTRLDEV lpMCtrlDev;

  GetClientRect(hCtrlBox,&MCtrlRect);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController, MCtrlDev);

  hSpeaker = LoadBitmap(hInstance,"Speaker");
  hPlayArea = LoadBitmap(hInstance,"PlayArea");
  hAreaSel = LoadBitmap(hInstance,"AreaSel");
  hSteppers = LoadBitmap(hInstance,"Steppers");

  hdcMemory = CreateCompatibleDC(hdc);

// Resize Controller static box//////////////////////////////////////
  GetObject (hSpeaker, sizeof (BITMAP), (LPSTR) &bm);
//  width = MCtrlRect.right - MCtrlRect.left;
  height = MCtrlRect.bottom - MCtrlRect.top;
  shift = (height - bm.bmHeight) / 2;
  MCtrlRect.top += shift;
  MCtrlRect.bottom -= shift;
  height = MCtrlRect.bottom - MCtrlRect.top;

//  MoveWindow(hCtrlBox,MCtrlRect.left,MCtrlRect.top,width,height,TRUE);
/////////////////////////////////////////////////////////////////////////

// Draw Controller speaker
  if (lpMCtrlDev->bSpeaker)
  {

    hBmpOld = (HBITMAP)SelectObject(hdcMemory, hSpeaker);
    SetRect(&rcBMP, 0, 0, bm.bmWidth, bm.bmHeight);

    StretchBlt(hdc,rcBMP.left,rcBMP.top,rcBMP.right,rcBMP.bottom,
    hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

    SelectObject(hdcMemory,hBmpOld);
    DeleteObject(hBmpOld);

  }
// Draw Controller play/stop button

// Draw Controller scroll area

// Draw Steppers
  if (lpMCtrlDev->bStepper)
  {

  }
  DeleteObject(hSpeaker);
  DeleteObject(hPlayArea);
  DeleteObject(hAreaSel);
  DeleteObject(hSteppers);

  DeleteDC(hdcMemory);

  AObjUnlockData(oiController, MCtrlDev);
}

/////////////////////////////////////////////////////////////////////////////////
//Function:
//  Window procedure of the Controller Box
//
////////////////////////////////////////////////////////////////////////////////
long FAR PASCAL _export CtrlBoxProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
//  LPCREATESTRUCT lpc;
  PAINTSTRUCT ps;
//  LPVOID lpv;
//  OBJECTID oiController;
  HDC hdc;

  static BITMAP bm;
  static HDC hdcMemory;
  static RECT rcBMP;
  static RECT MCtrlRect;
  int PlayBegin = 0, PlayEnd = 0;

  switch (message)
  {
    case WM_CREATE:
//      lpc = (LPCREATESTRUCT) lParam;
//      lpv = (lpc->lpCreateParams);
//      oiController = (OBJECTID)lpv;

      hSpeaker = LoadBitmap(hInstance,"Speaker");
      hAreaSel = LoadBitmap(hInstance,"AreaSel");
      hArea = LoadBitmap(hInstance,"Area");
      hSteppers = LoadBitmap(hInstance,"Steppers");
      break;

    case WM_PAINT:
      hdc = BeginPaint(hwnd,&ps);
      hdcMemory = CreateCompatibleDC(hdc);
      GetClientRect(hwnd,&MCtrlRect);
      if (SendDlgItemMessage(GetParent(hwnd),CB_SHOWSPEAKER , BM_GETCHECK, 0, 0L))
      {
	GetObject (hSpeaker, sizeof (BITMAP), (LPSTR) &bm);
	hBmpOld = (HBITMAP)SelectObject(hdcMemory, hSpeaker);
       //	SetRect(&rcBMP, 0, 0, bm.bmWidth, bm.bmHeight);

	StretchBlt(hdc,0,0,bm.bmWidth, bm.bmHeight,
	hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	SelectObject(hdcMemory,hBmpOld);
	DeleteObject(hBmpOld);
	PlayBegin = bm.bmWidth;
      }

      //Draw Steppers
      if (SendDlgItemMessage(GetParent(hwnd),CB_SHOWSTEPPERS , BM_GETCHECK, 0, 0L))
      {
	GetObject (hSteppers, sizeof (BITMAP), (LPSTR) &bm);
	hBmpOld = (HBITMAP)SelectObject(hdcMemory, hSteppers);
	SetRect(&rcBMP, MCtrlRect.right-bm.bmWidth, 0, bm.bmWidth, bm.bmHeight);
	StretchBlt(hdc,rcBMP.left,rcBMP.top,rcBMP.right,rcBMP.bottom,
	hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	SelectObject(hdcMemory,hBmpOld);
	DeleteObject(hBmpOld);
	PlayEnd = MCtrlRect.right - bm.bmWidth;
      }
      else
	PlayEnd = MCtrlRect.right;

      //Draw Controller play/stop button
      if (SendDlgItemMessage(GetParent(hwnd),CB_ALLOWSELECTIONS , BM_GETCHECK, 0, 0L))
      {
	GetObject (hAreaSel, sizeof (BITMAP), (LPSTR) &bm);
	hBmpOld = (HBITMAP)SelectObject(hdcMemory, hAreaSel);
      }
      else
      {
	GetObject (hArea, sizeof (BITMAP), (LPSTR) &bm);
	hBmpOld = (HBITMAP)SelectObject(hdcMemory, hArea);
      }
      SetRect(&rcBMP, PlayBegin, 0, PlayEnd-PlayBegin, bm.bmHeight);

      StretchBlt(hdc,rcBMP.left,rcBMP.top,rcBMP.right,rcBMP.bottom,
      hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

      SelectObject(hdcMemory,hBmpOld);
      DeleteObject(hBmpOld);

      DeleteDC(hdcMemory);
      EndPaint(hwnd,&ps);
      break;

    case WM_DESTROY:
      DeleteObject(hSpeaker);
      DeleteObject(hArea);
      DeleteObject(hAreaSel);
      DeleteObject(hSteppers);
      break;
  }
  return (DefWindowProc(hwnd,message,wParam,lParam));
}
