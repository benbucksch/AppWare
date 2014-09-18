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
//This file contains 14 functions of the Movie Metrics section
//These functions are:
//      1       MovieMetrics_FrameToTime        (Frame to Time)                 !!!
//      2       MovieMetrics_GetFrame           (Get Frame)                     !!!
//      3       MovieMetrics_GetLoopValue       (Get Loop Value)                !!!
//      4       MovieMetrics_GetPosterTime      (Get Poster Time)               !!!
//      5       MovieMetrics_GetRate            (Get Rate)                      !!!
//      6       MovieMetrics_GetTime            (Get Time)                      !!!
//      7       MovieMetrics_GoToBeginning      (Go To Begining)                !!!
//      8       MovieMetrics_GoToEnd            (Go To End)                     !!!
//      9       MovieMetrics_GoToFrame          (Go To Frame)                   !!!
//     10       MovieMetrics_GoToTime           (Go To Time)                    !!!
//     11       MovieMetrics_SetLoopValue       (Set Loop Value)                !!!
//     12       MovieMetrics_SetPosterTime      (Set Poster Time)               (not supported, in Win)
//     13       MovieMetrics_SetRate            (Set Rate)                      (not supported, in Win)
//     14       MovieMetrics_TimeToFrame        (Time To Frame)                 !!!
//////////////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>

#include "qtw.h"
#include "moviedat.h"
#include "movieexp.h"

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_FrameToTime
// Description:
//   Reports the elapsed time (in seconds) to a specified frame
// Inputs:
//   1. Movie Object
//   2. Frame
// Outputs:
//   1. Time
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_FrameToTime(pAEvtInfo, pAObjMessage theSystem)
{
//This function cannot be released now

  OBJECTID oiMovie;
  OBJECTID oiFrame;    
  OBJECTID oiTime;
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  LONG Frame;
  long double Time;
  Movie mMovie;
  MovieController mController;
  TimeScale tsScale;
  TimeValue Duration;
  long double v1, v2;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  if (!oiMovie)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  oiFrame= AFuncGetTypedParameter(2,OTYPE_NUMBER);
  if (!oiFrame)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  oiTime = AObjCreate(OTYPE_NUMBER);
  if (!oiTime)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  if (!lpMovieDevRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mController = lpMovieDevRec->mController;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieDevRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  MCGetCurrentTime(mController,&tsScale);
  Frame = ONmbrGetInteger(oiFrame);
  v1 = (long double)Frame;
  v2 = (long double)tsScale;

  Duration = GetMovieDuration(mMovie);

  if (Frame > Duration)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  Time = v1 / v2;

  if (ONmbrSetReal(oiTime,(long double)Time,TRUE) == ONMBR_RANGE_ERR)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  if (!AFuncSetParameter(1,oiTime))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GetFrame
// Description:
//   Returns the frame in the specified movie at the moment the function is called.
// Inputs:
//   Movie Object
// Outputs:
//   Frame (Integer Number)
///////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GetFrame(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiFrame;
  MOVIERUNPTR lpMovieRunRec;
  TimeValue tvTime;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiFrame = AObjCreate(OTYPE_NUMBER);
  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  tvTime = lpMovieRunRec->LastClickTime;
  AObjUnlockData(oiMovie,MovieRun);

  ONmbrSetInteger(oiFrame,(LONG)tvTime,TRUE);

  if (!AFuncSetParameter(1,oiFrame))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GetLoopValue
// Description:
//   Reports the loop value of the specified movie.
// Inputs:
//   Movie Object
// Outputs:
//   1. Value - Integer Number
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GetLoopValue(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiValue;                 //ID of the Temporary Object created to set Output Parameter 1
  MOVIERUNPTR lpMovieRunRec;
  long Value;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiValue = AObjCreate(OTYPE_NUMBER);
  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  Value = lpMovieRunRec->LoopValue;
  AObjUnlockData(oiMovie,MovieRun);

  ONmbrSetInteger(oiValue,Value,TRUE);

  if (!AFuncSetParameter(1,oiValue))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GetPosterTime
// Description:
//   Returns the elapsed time to the frame of the current poster.
// Inputs:
//   Movie Object
// Outputs:
//   1. Time - Real Number
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GetPosterTime(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiTime;    //ID of the Temporary Object created to set Output Parameter 1
  MOVIERUNPTR lpMovieRunRec;
  long double Time;
  TimeValue tvPoster;    //Time value of the poster of the specified movie
  Movie mMovie;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiTime = AObjCreate(OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  tvPoster = GetMoviePosterTime(mMovie);
  Time = tvPoster / GetMovieTimeScale(mMovie);   //Poster Time in seconds

  ONmbrSetReal(oiTime,Time,TRUE);                //Set output parameter

  if (!AFuncSetParameter(1,oiTime))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GetRate
// Description:
//   Returns a floating point number indicating the rate at wich the movie playing.
// Inputs:
//   Movie Object
// Outputs:
//   1. Rate - Real Number
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GetRate(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiRate;    //ID of the Temporary Object created to set Output Parameter 1
  MOVIEDEVPTR lpMovieDevRec;
  LFIXED lfxRate;
  double Rate;          //Duration expressed in seconds
  char RateStr[80];
  MovieController mController;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiRate = AObjCreate(OTYPE_NUMBER);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  if (!lpMovieDevRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mController = lpMovieDevRec->mController;
  AObjUnlockData(oiMovie,MovieDev);

  MCDoAction(mController,mcActionGetPlayRate,(LPVOID) &lfxRate);
  wsprintf(RateStr,"%d.%d",HIWORD(lfxRate),LOWORD(lfxRate));
  Rate = atof(RateStr);

  ONmbrSetReal(oiRate,Rate,TRUE);

  if (!AFuncSetParameter(1,oiRate))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GetTime
// Description:
//   Returns the tiem elapsed time in the specified movie at the moment the function is called.
// Inputs:
//   Movie Object
// Outputs:
//   None
///////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GetTime(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiTime;
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  MovieController mController;
  TimeValue tvTime;
  TimeScale tsScale;
  double Time, v1, v2;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiTime = AObjCreate(OTYPE_NUMBER);

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  if (!lpMovieDevRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mController = lpMovieDevRec->mController;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie, MovieRun);
  tvTime = lpMovieRunRec->LastClickTime;
  AObjUnlockData(oiMovie,MovieRun);
  MCGetCurrentTime(mController,&tsScale);
  v1 = (double)tvTime;
  v2 = (double)tsScale;

  Time = v1 / v2;

  ONmbrSetReal(oiTime,Time,TRUE);

  if (!AFuncSetParameter(1,oiTime))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GoToBeginning
// Description:
//   Position the specified movie at its first frame.
// Inputs:
//   Movie Object
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GoToBeginning(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  MOVIERUNPTR lpMovieRunRec;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  lpMovieRunRec->LastClickTime = 0;
  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtGoToTime, AEvtGetObjectChangedPriority(how));
  theSystem->message1 = 1;                 //After changing position

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GoToEnd
// Description:
//   Position the specified movie at its last frame.
// Inputs:
//   Movie Object
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GoToEnd(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  lpMovieRunRec->LastClickTime = GetMovieDuration(mMovie);
  AObjUnlockData(oiMovie,MovieRun);
  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtGoToTime, AEvtGetObjectChangedPriority(how));
  theSystem->message1 = 1;                 //After changing position

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GoToFrame
// Description:
//   Position the specified movie at specified frame frame.
// Inputs:
//   1. Movie Object
//   2. Frame 
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GoToFrame(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiFrame;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TimeValue Duration;
  long Frame;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiFrame = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;

  Frame = ONmbrGetInteger(oiFrame);
  Duration = GetMovieDuration(mMovie);

  if (Frame > Duration)
  {
    theSystem->message1 = 2;
    AObjUnlockData(oiMovie,MovieRun);
    return;
  }

  lpMovieRunRec->LastClickTime = Frame;
  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtGoToTime, AEvtGetObjectChangedPriority(how));
  theSystem->message1 = 1;                 //After changing position

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_GoToTime
// Description:
//   Position the specified movie at specified frame frame.
// Inputs:
//   1. Movie Object
//   2. Time (in seconds)
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_GoToTime(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiTime;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TimeRecord trRecord;
  TimeScale tsScale;
  TimeValue Duration;
  long double Time;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiTime = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;

  ONmbrGetReal(oiTime,&Time);

  tsScale = GetMovieTimeScale(mMovie);
 
  trRecord.value.dwLo = (LONG) Time * tsScale;
  trRecord.value.dwHi = 0;
  trRecord.scale = tsScale;
  
  Duration = GetMovieDuration(mMovie);

  if (Duration < trRecord.value.dwLo)
  {
    theSystem->message1 = 2;
    AObjUnlockData(oiMovie,MovieRun);
    return;
  }

  lpMovieRunRec->LastClickTime = (LONG) Time * tsScale;
  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtGoToTime, AEvtGetObjectChangedPriority(how));
  theSystem->message1 = 1;                 //After changing position

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_SetLoopValue
// Description:
//   Sets the loop value of the specified movie.
// Inputs:
//   1. Movie
//   2. Value - Integer Number
// Output
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_SetLoopValue(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiValue;                 //ID of the Temporary Object created to set Output Parameter 1
  MOVIERUNPTR lpMovieRunRec;
  long Value;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiValue = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  if (!oiValue)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  Value = ONmbrGetInteger(oiValue);
  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  lpMovieRunRec->LoopValue = Value;
  AObjUnlockData(oiMovie, MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtSetLoopValue, AEvtGetObjectChangedPriority(how));

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieMetrics_TimeToFrame
// Description:
//   Reports the elapsed time (in seconds) to a specified frame
// Inputs:
//   1. Movie Object
//   2. Time
// Outputs:
//   1. Frame
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieMetrics_TimeToFrame(pAEvtInfo, pAObjMessage theSystem)
{
//This function cannot be released now

  OBJECTID oiMovie;
  OBJECTID oiFrame;    
  OBJECTID oiTime;
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long double Frame;
  long double Time;
  Movie mMovie;
  MovieController mController;
  TimeScale tsScale;
  TimeValue Duration;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  if (!oiMovie)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  oiTime= AFuncGetTypedParameter(2,OTYPE_NUMBER);
  if (!oiTime)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  oiFrame = AObjCreate(OTYPE_NUMBER);
//  oiFrame = AFuncGetOutput(1,&Type);

  if (!oiFrame)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  if (!lpMovieDevRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mController = lpMovieDevRec->mController;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  MCGetCurrentTime(mController,&tsScale);
  ONmbrGetReal(oiTime, &Time);

  Frame = Time * tsScale;

  Duration = GetMovieDuration(mMovie);

  if (Frame > Duration)
  {
    theSystem->message1 = 2;
    return;
  }

  if (ONmbrSetReal(oiFrame,Frame,TRUE) == ONMBR_RANGE_ERR)
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  if (!AFuncSetParameter(1,oiFrame))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

void MOVIEVI::GoToTime()
{
  MOVIERUNPTR lpMovieRunRec;
  TimeRecord trRecord;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);
    trRecord.value.dwLo = lpMovieRunRec->LastClickTime;
    trRecord.value.dwHi = 0;
    trRecord.scale = GetMovieTimeScale(mMovie);
  AObjUnlockData(m_oiObject,MovieRun);

  MCDoAction(mController, mcActionGoToTime, (LPVOID) &trRecord);

}

void MOVIEVI::SetLoopValue()
{
  MOVIERUNPTR lpMovieRunRec;
  BOOL bValue;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);
  switch(lpMovieRunRec->LoopValue)
  {
    case 0:
      MCDoAction(mController,mcActionSetLooping,(LPVOID) FALSE);
      MCDoAction(mController,mcActionSetLoopIsPalindrome,(LPVOID) FALSE);

      break;

    case 1:
      MCDoAction(mController,mcActionGetLoopIsPalindrome,&bValue );
      if (bValue)
        MCDoAction(mController,mcActionSetLoopIsPalindrome,(LPVOID) FALSE);
      MCDoAction(mController,mcActionSetLooping,(LPVOID) TRUE);
      break;

    case 2:
      MCDoAction(mController,mcActionSetLooping,(LPVOID) TRUE);
      MCDoAction(mController,mcActionSetLoopIsPalindrome,(LPVOID) TRUE);
      break;
  }
  AObjUnlockData(m_oiObject,MovieRun);
}