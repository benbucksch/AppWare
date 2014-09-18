#define STRICT
#define ASTRICT

#include<a_alm.h>
#include <windowsx.h>
#include "movctrl.h"
#include <stdio.h>
#include <qtw.h>

extern MovieFile mfMovie;

OpWndItemD* ALMAPI  MCtrlCreate(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  )
{
  MCONTROLLER_VINST* mctrl_vi;
  I_UNUSED(special);

  mctrl_vi = new MCONTROLLER_VINST(oiItem,oiParent,rcItem,status);

  return mctrl_vi;
}
                                	
MCONTROLLER_VINST::MCONTROLLER_VINST(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status): OpLiteWndItem(oiItem,oiParent,rcItem,status)

{
  LPMCTRLDEV lpMCtrlDev = (LPMCTRLDEV) AObjLockData(m_oiObject,0);
  SetRect(&mapRect,0,0,0,0);

  OWndSetItemActivatable(this,lpMCtrlDev->canActivate);
  AObjUnlockData(m_oiObject,0);
}
                              	

MCONTROLLER_VINST::~MCONTROLLER_VINST(void)
{
}

void ALMAPI MCONTROLLER_VINST::Destroy(void)
{
  DisposeMovieController(mController);
  delete this;
}

void ALMAPI MCONTROLLER_VINST::DataChanged(long details, long modifiers, long variation)
{
  MessageBox(NULL,"DataChanged",NULL,MB_OK);
}

long ALMAPI MCONTROLLER_VINST::Activate(int causedBy, BOOL mustKeep)
{
  MessageBox(NULL,"Activate",NULL,MB_OK);
}

void ALMAPI MCONTROLLER_VINST::Deactivated(void)
{
  MessageBox(NULL,"Deactivate",NULL,MB_OK);
}

void ALMAPI MCONTROLLER_VINST::Draw(HDC hdc, RECT* invalidRect)
{
  LPMCTRLDEV lpMCtrlDev = (LPMCTRLDEV) AObjLockData(m_oiObject,0);

  int mode = GetMapMode(hdc);
  int sDC = SaveDC(hdc);

  Movie mMovie;
//  static MovieController mController;
  RECT MCtrlRect, rcClient;
  POINT inflatePt;
  char str[255];
  long lMCInfoFlags;

  I_UNUSED(invalidRect);

  mapRect = MCtrlRect = m_rcItem;
  mode = SetMapMode(hdc,MM_TEXT);

  OWndLPtoDP(m_oiParent,(LPPOINT)&mapRect,2);
  
  inflatePt.x = -1; //inflate y-dimension of the Rectangle  by -1 
  InflateRect(&MCtrlRect,inflatePt.x,0);
  DisposeMovieController(mController);
  NewMovieFromFile (&mMovie, mfMovie, NULL, NULL, 0, NULL);
  mController = NewMovieController(mMovie,&MCtrlRect,mcNotVisible,m_hwndParent);

  MCDoAction (mController, mcActionGetFlags, &lMCInfoFlags);

//  MessageBox(NULL,"Yaaaah",NULL,MB_OK);
                                                         
  if (lpMCtrlDev->bSpeaker) {
//    MessageBox(NULL,"speaker true",NULL,MB_OK);
    lMCInfoFlags &= ~mcFlagSuppressSpeakerButton;
  }
  else {
//    MessageBox(NULL,"speaker false",NULL,MB_OK);
    lMCInfoFlags |= mcFlagSuppressSpeakerButton;
  }

//  MCDoAction(mController,mcActionSetFlags,&lMCInfoFlags);

//  MCDoAction (mController, mcActionGetFlags, &lMCInfoFlags);

  if (lpMCtrlDev->bStepper) {
//    MessageBox(NULL,"stepper true",NULL,MB_OK);
    lMCInfoFlags &= ~mcFlagSuppressStepButtons;
  }
  else {
//    MessageBox(NULL,"stepper false",NULL,MB_OK);
    lMCInfoFlags |= mcFlagSuppressStepButtons;
  }

  MCDoAction(mController,mcActionSetFlags,&lMCInfoFlags);


  MCSetControllerAttached(mController,FALSE);
  MCDoAction (mController, mcActionPlay, 0);
  SetMovieActive (mMovie, FALSE);
  MCSetControllerBoundsRect(mController,&MCtrlRect);
  MCPositionController(mController,&mapRect,&mapRect,0);

  DisposeMovie(mMovie);



  AObjUnlockData(m_oiObject,MCtrlDev);
  MCSetVisible(mController,TRUE);

  SetMapMode(hdc,mode);
  RestoreDC(hdc, sDC);

  I_UNUSED(invalidRect);

}

void ALMAPI MCONTROLLER_VINST::MouseDown(POINT* where, int keysDown, int button)
{
  MessageBox(NULL,"Mouse Down",NULL,MB_OK);
}

void ALMAPI MCONTROLLER_VINST::MouseMove(POINT* where,  int keysDown)
{
  MessageBox(NULL,"Mouse Move",NULL,MB_OK);
}

void ALMAPI MCONTROLLER_VINST::MouseUp(POINT* where, int keysDown, int button)
{
  MessageBox(NULL,"Mouse Up",NULL,MB_OK);
}

void ALMAPI MCONTROLLER_VINST::KeyDown(int vKey, char ascii, long keyData)
{
  MessageBox(NULL,"Key Down",NULL,MB_OK);
}

void DoubleClicked(POINT* where, int keysDown, int button)
{
  MessageBox(NULL,"Double Clicked",NULL,MB_OK);
}