//////////////////////////////////////////////////////////////////////////////////////
//This file contains 13 functions of the Movie Controller object
//These functions are:
//      1	Controller_AllowSelecting 	(Allow Selecting)	!!!
//      2	Controller_AssignMovie		(Assign Movie)		!!!  
//      3	Controller_ClearMovie		(Clear Movie)		!!!  
//      4	Controller_DisallowSelecting	(Disallow Selecting)	!!!
//      5	Controller_GetCtrlMovie		(Get Controllers Movie)	!!!
//      6	Controller_GetMoviesCtrl	(Get Movies Controller)	!!!
//      7	Controller_HideSpeaker		(Hide Speaker)		!!!
//      8	Controller_HideSteppers		(HideSteppers)		!!!
//      9	Controller_IsSelectingEnabled	(Selecting Enabled?)	!!!
//     10	Controller_ShowSpeaker		(Show Speaker)		!!!
//     11 	Controller_ShowSteppers		(Show Steppers)		!!!
//     12	Controller_IsSpeakerVisible	(Speaker Visible?)	!!!
//     13	Controller_IsSteppersVisible	(Stepper Visible?)	!!!
//////////////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdio.h>
#include <mem.h>
#include <windows.h>
#include <windowsx.h>

#include <a_alm.h>

#include "movctrl.h"
#include "moviedat.h"
#include "qtw.h"


LONG (FAR PASCAL *gopa)(OBJECTID, pAObjMessage);

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_AllowSelecting
// Description:
//
// Inputs:
//   Controller
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_AllowSelecting(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  LPMCTRLDEV lpMCtrlDev;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  lpMCtrlDev->bSelectable = TRUE;
  AObjUnlockData(oiController,MCtrlDev);

  theSystem->message1 = 1;
  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_AssignMovie
// Description:
//
// Inputs:
//   1. Controller
//   2. Movie
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_AssignMovie(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  MOVIEDEVPTR lpMovieDevRec;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);
  oiMovie = AFuncGetTypedParameter(2,OTYPE_MOVIE);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);

  lpMCtrlDev->oiMovie = oiMovie;
  lpMCtrlDev->oiMovieShared = oiMovie;       //
  if (oiMovie)
  {
    lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
    lpMovieDevRec->oiController = oiController;
    if (lpMovieDevRec->oiSelf != oiMovie)
    {
      lpMovieDevRec->bShared = TRUE;
      lpMCtrlDev->oiMovieShared = lpMovieDevRec->oiSelf;   //refer to original Movie object
      lpMCtrlDev->bMovieAlias = TRUE;                           //Assigned Movie is an alias object
    }
    AObjUnlockData(oiMovie,MovieDev);
  }

  AObjUnlockData(oiController,MCtrlDev);

  theSystem->message1 = 1;
  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_ClearMovie
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_ClearMovie(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  MOVIEDEVPTR lpMovieDevRec;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  if (lpMCtrlDev)
  {
    if (lpMCtrlDev->oiMovie)
    {
      oiMovie = lpMCtrlDev->oiMovie;
      lpMCtrlDev->oiMovie = 0;
      lpMCtrlDev->oiMovieShared = 0;
      AObjUnlockData(oiController,MCtrlDev);

      lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      lpMovieDevRec->oiController = 0;
      AObjUnlockData(oiMovie,MovieDev);
    }
  }
  theSystem->message1 = 1;
  return;

}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_DisallowSelecting
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_DisallowSelecting(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  LPMCTRLDEV lpMCtrlDev;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  lpMCtrlDev->bSelectable = FALSE;
  AObjUnlockData(oiController,MCtrlDev);

  theSystem->message1 = 1;
  return;

}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_GetCtrlMovie
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   1.Movie 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_GetCtrlMovie(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  TYPEID type;
  AObjMessage theSys;
  HINSTANCE libInst;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);
  oiMovie = AFuncGetOutput(1,&type);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  if (oiMovie != lpMCtrlDev->oiMovie)
  {
    theSys.message3 = (LONG)lpMCtrlDev->oiMovie;
    libInst = LoadLibrary("alm_mov.dll");
    (FARPROC) gopa = GetProcAddress(libInst,"AssignObject");
    (gopa)(oiMovie,&theSys);
/*
    MovieVI = (MOVIEVI *) AObjGetRuntimeInfo(oiMovie);
    if (MovieVI)
    {
      hwnd = MovieVI->GetMovieWnd();
      I_UNUSED(hwnd);
    }
*/
    FreeLibrary(libInst);
  }
  AObjUnlockData(oiController,MCtrlDev);

  theSystem->message1 = 1;
  return;

}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_GetMoviesCtrl
// Description:
//
// Inputs:
//   1. Movie
// Outputs:
//   1.Controller
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_GetMoviesCtrl(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  MOVIEDEVPTR lpMovieDevRec;
  AObjMessage theSys;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiController = AObjCreate(OTYPE_CONTROLLER);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  theSys.message3 =(LONG) lpMovieDevRec->oiController;
  AssignObject(oiController,&theSys);
  AObjUnlockData(oiMovie,MovieDev);

  AFuncSetParameter(1,oiController);
  theSystem->message1 = 1;
  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_HideSpeaker
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_HideSpeaker(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  long how;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  oiMovie = lpMCtrlDev->oiMovie;
  lpMCtrlDev->bSpeaker = FALSE;
  AObjUnlockData(oiController,MCtrlDev);

/*
  MovieVI = (MOVIEVI *) AObjGetRuntimeInfo(oiMovie);
  if (MovieVI)
  {
    mController = MovieVI->GetMovieController();
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags |= mcFlagSuppressSpeakerButton;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
*/

  how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AEvtPostStandard(oiController, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how) );
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtCtrlHideSpeaker, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;

}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_HideSteppers
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_HideSteppers(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  long how;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  oiMovie = lpMCtrlDev->oiMovie;
  lpMCtrlDev->bStepper = FALSE;
  AObjUnlockData(oiController,MCtrlDev);
/*
  MovieVI = (MOVIEVI *) AObjGetRuntimeInfo(oiMovie);
  if (MovieVI)
  {
    mController = MovieVI->GetMovieController();
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags |= mcFlagSuppressStepButtons;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
*/
  how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AEvtPostStandard(oiController, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how) );
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtCtrlHideSteppers, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_IsSelectingEnabled
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_IsSelectingEnabled(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  LPMCTRLDEV lpMCtrlDev;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);

  if (lpMCtrlDev->bSelectable)
    theSystem->message1 = 1;
  else
    theSystem->message1 = 2;

  AObjUnlockData(oiController,MCtrlDev);

  return;

}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_ShowSpeaker
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_ShowSpeaker(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  long how;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  oiMovie = lpMCtrlDev->oiMovie;
  lpMCtrlDev->bSpeaker = TRUE;
  AObjUnlockData(oiController,MCtrlDev);
/*
  MovieVI = (MOVIEVI *) AObjGetRuntimeInfo(oiMovie);
  if (MovieVI)
  {
    mController = MovieVI->GetMovieController();
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags &= ~mcFlagSuppressSpeakerButton;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
*/
  how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AEvtPostStandard(oiController, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how) );
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtCtrlShowSpeaker, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;


}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_ShowSteppers
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_ShowSteppers(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  OBJECTID oiMovie;
  LPMCTRLDEV lpMCtrlDev;
  long how;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
  oiMovie = lpMCtrlDev->oiMovie;
  lpMCtrlDev->bStepper = TRUE;
  AObjUnlockData(oiController,MCtrlDev);
/*
  MovieVI = (MOVIEVI *) AObjGetRuntimeInfo(oiMovie);
  if (MovieVI)
  {
    mController = MovieVI->GetMovieController();
    MCDoAction(mController,mcActionGetFlags,&lFlags);
    lFlags &= ~mcFlagSuppressStepButtons;
    MCDoAction(mController,mcActionSetFlags,(LPVOID)lFlags);
  }
*/
  how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AEvtPostStandard(oiController, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how) );
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtCtrlShowSteppers, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;

}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_IsSpeakerVisible
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_IsSpeakerVisible(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  LPMCTRLDEV lpMCtrlDev;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);

  if (lpMCtrlDev->bSpeaker)
    theSystem->message1 = 1;
  else
    theSystem->message1 = 2;

  AObjUnlockData(oiController,MCtrlDev);

  return;

}

//////////////////////////////////////////////////////////////////////////
// Function:
//   Controller_IsStepperVisible
// Description:
//
// Inputs:
//   1. Controller
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export Controller_IsSteppersVisible(pAEvtInfo theEvent, pAObjMessage theSystem)
{
  I_UNUSED(theEvent);

  OBJECTID oiController;
  LPMCTRLDEV lpMCtrlDev;

  oiController = AFuncGetTypedParameter(1,OTYPE_CONTROLLER);

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);

  if (lpMCtrlDev->bStepper)
    theSystem->message1 = 1;
  else
    theSystem->message1 = 2;

  AObjUnlockData(oiController,MCtrlDev);

  return;
}

