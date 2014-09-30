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

////////////////////////////////////////////////////////////////////
//This file contains 13 functions of the Movie Display section
//These functions are:
//                      c++ function name       AppWare function name   Support !- win/mac
//      1       MovieDisplay_IsActBorder        (Activation Border?)    !!!
//      2       MovieDisplay_IsBadge            (Badge?)                !!!                      
//      3       MovieDisplay_IsBorder           (Border?)               !!!             
//      4       MovieDisplay_GetDisplayMode     (Get Display Mode)      !!!          
//      5       MovieDisplay_GetRotation        (Get Rotation)          (not supported, in Win)
//      6       MovieDisplay_GetScale           (Get Scale)             !!!
//      7       MovieDisplay_GetScroll          (Get Scroll)            !!!
//      8       MovieDisplay_GetVolume          (Get Volume)            !!!
//      9       MovieDisplay_HideActBorder      (Hide Act. Border)      !!!
//     10       MovieDisplay_HideBadge          (Hide Badge)            !!!
//     11       MovieDisplay_HideBorder         (Hide Border)           !!!
//     12       MovieDisplay_HideLogo           (Hide Logo)             !!!
//     13       MovieDisplay_IsLogo             (Logo?)                 !!!
//     14       MovieDisplay_SetDisplayMode     (Set DisplayMode)       !!!
//     15       MovieDisplay_SetLanguage        (Set Language)          (not supported, in Win)
//     16       MovieDisplay_SetRotation        (Set Rotation)          (not supported, in Win)
//     17       MovieDisplay_SetScale           (Set Scale)             !!!
//     18       MovieDisplay_SetScroll          (Set Scroll)            !!!
//     19       MovieDisplay_SetVolume          (Set Volume)            !!!
//     20       MovieDisplay_ShowActBorder      (Show Act. Border)      !!!
//     21       MovieDisplay_ShowBadge          (Show Badge)            !!!
//     22       MovieDisplay_ShowBorder         (Show Border)           !!!
//     23       MovieDisplay_ShowLogo           (Show Logo)             !!!
/////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>

#include "qtw.h"
#include "moviedat.h"
#include "movieexp.h"

extern BOOL QTInstall;

//extern void DrawActBorder(HWND, BOOL);

//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_IsActivationBorder
// Description:
//   Determines whether the activation border is to be displayed
//   when the movie is activated.
// Inputs:
//   Movie Object
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_IsActBorder(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Temporary Object created to set Output Parameter
  MOVIEDEVPTR lpMovieDevRec;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

  if (lpMovieDevRec->bShowActBorder)
    theSystem->message1 = 1;                 //Is shown
  else
    theSystem->message1 = 2;                 //Is hidden
     
  AObjUnlockData(oiMovie,MovieDev);

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_IsBadge
// Description:
//   Determines whether the movie badge is to be displayed
//   when the movie stops.
// Inputs:
//   Movie Object
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_IsBadge(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Temporary Object created to set Output Parameter
  MOVIEDEVPTR lpMovieDevRec;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

  if (lpMovieDevRec->bShowBadge)
    theSystem->message1 = 1;                 //Is shown
  else
    theSystem->message1 = 2;                 //Is hidden
     
  AObjUnlockData(oiMovie,MovieDev);

  return;
}


//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_IsBorder
// Description:
//   Determines whether the  movie has a frame.
// Inputs:
//   Movie Object
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_IsBorder(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Temporary Object created to set Output Parameter
  MOVIEDEVPTR lpMovieDevRec;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

  if (lpMovieDevRec->bShowBorder)
    theSystem->message1 = 1;                 //Is shown
  else
    theSystem->message1 = 2;                 //Is hidden
     
  AObjUnlockData(oiMovie,MovieDev);

  return;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_GetDisplayMode
// Description:
//   Return a whole number indicating the movie's display mode. There are four modes:
//      1: Centered
//      2: Sized
//      3: Scaled
//      4: Cropped   
// Inputs:
//   1. Movie Object
// Outputs:
//   1. Mode - Number Object;   
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_GetDisplayMode(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiMode;
  MOVIEDEVPTR lpMovieDevRec;

  WORD Mode;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiMode = AObjCreate(OTYPE_NUMBER);

  if (oiMovie)
  {
    lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

    if (lpMovieDevRec)
    {
      Mode = lpMovieDevRec->DisplayMode - 1004;  // Base point of the Display mode is RB_CENTERED = 1004
      AObjUnlockData(oiMovie,MovieDev);
    }
    else
    {
      theSystem->message1 = 2;                        //Failed
      return;
    }

    ONmbrSetInteger(oiMode,(LONG)Mode,TRUE);
    
    if (AFuncSetParameter(1,oiMode))
      theSystem->message1 = 1;                          //After getting
    else
      theSystem->message1 = 2;                          //Failed
  }
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_GetScale
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   2. Scale - Real number
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_GetScale(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiScale;
  LPMOVIEFRAMEREC lpMovieFrameRec;
  double Scale;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiScale = AObjCreate(OTYPE_NUMBER);

  lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
  Scale = lpMovieFrameRec->Scale;
  AObjUnlockData(oiMovie,MovieFrame);

  ONmbrSetReal(oiScale,Scale,TRUE);

  if (AFuncSetParameter(1,oiScale))
    theSystem->message1 = 1;                          //After getting
  else
    theSystem->message1 = 2;

  return;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_GetScroll
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   2. Scroll - Real number
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_GetScroll(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiHScroll;
  AOBJECTID oiVScroll;
  LPMOVIEFRAMEREC lpMovieFrameRec;
  long HScroll, VScroll;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiHScroll = AObjCreate(OTYPE_NUMBER);
  oiVScroll = AObjCreate(OTYPE_NUMBER);

  lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
  HScroll = lpMovieFrameRec->HScroll;
  VScroll = lpMovieFrameRec->VScroll;
  AObjUnlockData(oiMovie,MovieFrame);

  ONmbrSetInteger(oiHScroll,HScroll,TRUE);
  ONmbrSetInteger(oiVScroll,VScroll,TRUE);

  if (!AFuncSetParameter(1,oiHScroll))
  {
    theSystem->message1 = 2;
    return;
  }

  if (!AFuncSetParameter(2,oiVScroll))
  {
    theSystem->message1 = 2;
    return;
  }

  theSystem->message1 = 1;
  return;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_GetVolume
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   2. Volume - Whole number
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_GetVolume(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiVolume;
  MOVIEDEVPTR lpMovieDevRec;
  SFIXED sfxVolume;
  long Volume;
  MovieController mController;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  if (!oiMovie)                               //Invalid parameter
  {
    theSystem->message1 = 2;                  //Failed
    return;
  }
  oiVolume = AObjCreate(OTYPE_NUMBER);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  mController = lpMovieDevRec->mController;
  AObjUnlockData(oiMovie,MovieDev);

  if (!mController)
  {
    theSystem->message1 = 2;                  //Failed
    return;
  }

  // Get the movie's volume
  MCDoAction (mController, mcActionGetVolume,(LPVOID) &sfxVolume);
  Volume = (LONG) HIBYTE(sfxVolume);
  
  ONmbrSetInteger(oiVolume,(LONG)Volume,TRUE);

  AFuncSetParameter(1,oiVolume);
  theSystem->message1 = 1;

  return;

}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_HideActBorder
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_HideActBorder(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowActBorder = FALSE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawActBorder, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_HideBadge
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_HideBadge(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowBadge = FALSE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawBadge, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_HideBorder
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_HideBorder(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowBorder = FALSE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawBorder, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_HideLogo
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_HideLogo(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowLogo = FALSE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawLogo, AEvtGetObjectChangedPriority(how));

  theSystem->message1 = 1;                 //After Hiding

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_IsLogo
// Description:
//
// Inputs:
//   Movie Object
// Outputs:
//   None 
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_IsLogo(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Temporary Object created to set Output Parameter
  MOVIEDEVPTR lpMovieDevRec;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

  if (lpMovieDevRec->bShowLogo)
    theSystem->message1 = 1;                 //Is shown
  else
    theSystem->message1 = 2;                 //Is hidden
     
  AObjUnlockData(oiMovie,MovieDev);

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_SetDisplayMode
// Description:
//   Sets a whole number indicating the movie's display mode. There are four modes:
//      1: Centered
//      2: Sized
//      3: Scaled
//      4: Cropped   
// Inputs:
//   1. Movie Object
//   2. Value
// Outputs:
//   None       
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_SetDisplayMode(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiMode;
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  WORD Mode;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiMode = AFuncGetParameter(2);

  Mode = ONmbrGetInteger(oiMode);
  if ((Mode <= 0) || (Mode > 4))
    Mode = 2;

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

  if (lpMovieDevRec)
  {
    lpMovieDevRec->DisplayMode = Mode + 1004;  // Base point of the Display mode is RB_CENTERED = 1004

  }
  else
  {
    theSystem->message1 = 2;                        //Failed
    return;
  }
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDisplayMovie, AEvtGetObjectChangedPriority(how));

  theSystem->message1 = 1;                          //After getting

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_SetScale
// Description:
//
// Inputs:
//   1. Movie Object
//   2. Scale - Whole number
// Output:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_SetScale(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiScale;
  LPMOVIEFRAMEREC lpMovieFrameRec;
  MOVIERUNPTR lpMovieRunRec;
  MOVIEDEVPTR lpMovieDevRec;
  WORD DisplayMode;
  long double Scale;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiScale = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  ONmbrGetReal(oiScale,&Scale);

  if (Scale <= 0)
  {
    theSystem->message1 = 2;
    return;
  }


  lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
    lpMovieFrameRec->Scale = Scale;
  AObjUnlockData(oiMovie,MovieFrame);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  DisplayMode = lpMovieDevRec->DisplayMode;
  AObjUnlockData(oiMovie, MovieDev);

  if (DisplayMode != 1008)
  {
    AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how));
    theSystem->message1 = 1;
    return;
  }
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtSetScale, AEvtGetObjectChangedPriority(how));

  theSystem->message1 = 1;
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_SetScroll
// Description:
//
// Inputs:
//   1. Movie Object
//   2. HScroll - Whole number
//   3. VScroll - Whole number
// Output:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_SetScroll(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiHScroll;
  AOBJECTID oiVScroll;
  LPMOVIEFRAMEREC lpMovieFrameRec;
  MOVIERUNPTR lpMovieRunRec;
  long HScroll, VScroll;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiHScroll = AFuncGetTypedParameter(2,OTYPE_NUMBER);
  oiVScroll = AFuncGetTypedParameter(3,OTYPE_NUMBER);

  HScroll = ONmbrGetInteger(oiHScroll);
  VScroll = ONmbrGetInteger(oiVScroll);

  lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(oiMovie,MovieFrame);
  lpMovieFrameRec->HScroll =(int) HScroll;
  lpMovieFrameRec->VScroll =(int) VScroll;
  AObjUnlockData(oiMovie,MovieFrame);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtSetScroll, AEvtGetObjectChangedPriority(how));

  theSystem->message1 = 1;
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_SetVolume
// Description:
//
// Inputs:
//   1. Movie Object
//   2. Volume
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_SetVolume(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiVolume;
  MOVIERUNPTR lpMovieRunRec;
  MOVIEDEVPTR lpMovieDevRec;
  SFIXED sfxVolume;
  long double Volume;
  double VolInt, VolFrac;
  long how;
  MovieController mController;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiVolume = AFuncGetParameter(2);

  ONmbrGetReal(oiVolume,&Volume);
  VolFrac = modf(Volume,&VolInt);
  sfxVolume = MAKESFIXED(VolInt,VolFrac);


  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  mController = lpMovieDevRec->mController;
  AObjUnlockData(oiMovie,MovieDev);

  if (!mController)
  {
    theSystem->message1 = 2;                  //Failed
    return;
  }

  // Get the movie's volume
  MCDoAction (mController, mcActionSetVolume,(LPVOID) &sfxVolume);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how));

  theSystem->message1 = 1;

  return;

}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_ShowActBorder
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_ShowActBorder(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowActBorder = TRUE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawActBorder, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_ShowBadge
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_ShowBadge(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowBadge = TRUE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawBadge, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_ShowBorder
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_ShowBorder(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowBorder = TRUE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawBorder, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;                 //After Hiding

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MovieDisplay_ShowLogo
// Description:
//
// Inputs:
//   1. Movie Object
// Outputs:
//   None
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieDisplay_ShowLogo(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieDevRec->bShowLogo = TRUE;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec->bIsMoviePlaying)
    how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  else
    how = AEVT_ATTAIL | AEVT_KILLDUPLICATES;

  AObjUnlockData(oiMovie, MovieRun);

  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawLogo, AEvtGetObjectChangedPriority(how));

  theSystem->message1 = 1;                 //After Hiding

  return;
}
