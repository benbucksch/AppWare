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
//This file contains 8 functions of the Movie Display section
//These functions are:
//      1       MovieInfo_GetDates              (Get Dates)                     !!!
//      2       MovieInfo_GetDuration           (Get Duration)                  !!!
//      3       MovieInfo_GetMaxScroll          (Get Max Scroll)                !!!
//      4       MovieInfo_GetPreferredRate      (Get Preferred Rate)            !!!
//      5       MovieInfo_GetPreferredVolume    (Get Preferred Volume)          !!!
//      6       MovieInfo_GetTotalFrames        (Get Total Frames)              !!!
//      7       MovieInfo_IsSoundTrack          (Sound Track?)                  !!!
//      8       MovieInfo_IsVideoTrack          (Video Track?)                  !!!
/////////////////////////////////////////////////////////////////////

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
//  MovieInfo_GetDates
// Description:
//   Reports the dates the movie was created and modified.
// Inputs:
//   Movie Object
// Outputs:
//   1. Creation Date
//   2. Modification Date
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_GetDates(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  AOBJECTID oiCreationDate;    //ID of the Temporary Object created to set Output Parameter 1
  AOBJECTID oiModificationDate;    //ID of the Temporary Object created to set Output Parameter 2
  MOVIERUNPTR lpMovieRunRec;
  LONG lCreationDateTime;
  LONG lModificationDateTime;
  Movie mMovie;
  char buffer[80];
  char *ch;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiCreationDate = AObjCreate(OTYPE_TEXT);
  oiModificationDate = AObjCreate(OTYPE_TEXT);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  lCreationDateTime = GetMovieCreationTime (mMovie);
  wsprintf (buffer, "%s", ctime(&lCreationDateTime));
  ch = strchr(buffer,'\n');
  *ch = '\0';
  if (!OTextSetValueFromBuffer(oiCreationDate,buffer))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  lModificationDateTime = GetMovieModificationTime (mMovie);
  wsprintf (buffer, "%s", ctime(&lModificationDateTime));
  ch = strchr(buffer,'\n');
  *ch = '\0';

  if (!OTextSetValueFromBuffer(oiModificationDate,buffer))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  if (!AFuncSetParameter(1,oiCreationDate))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  if (!AFuncSetParameter(2,oiModificationDate))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieInfo_GetDuration
// Description:
//   Reports the length in seconds (a floating point number) of the specified movie.
// Inputs:
//   Movie Object
// Outputs:
//   1. Duration - Real Number
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_GetDuration(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  AOBJECTID oiDuration;    //ID of the Temporary Object created to set Output Parameter 1
  MOVIERUNPTR lpMovieRunRec;
  long double lDuration;          //Duration expressed in seconds
  TimeValue Duration;      //Number of units in the movie
  Movie mMovie;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiDuration = AObjCreate(OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  Duration = GetMovieDuration(mMovie);                //Number of units in the movie

  lDuration = Duration / GetMovieTimeScale(mMovie);   //Duration expressed in seconds

  ONmbrSetReal(oiDuration,lDuration,TRUE);

  if (!AFuncSetParameter(1,oiDuration))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieInfo_GetMaxScroll
// Description:
//   Reports the preferred rate ofthe movie as a floating point number.
// Inputs:
//   Movie Object
// Outputs:
//   1. Max H Scroll - Whole Number
//   2. Max V Scroll - Whole Number  
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_GetMaxScroll(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  AOBJECTID oiMaxHScroll;    //ID of the Temporary Object created to set Output Parameter 1
  AOBJECTID oiMaxVScroll;    //ID of the Temporary Object created to set Output Parameter 2
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  long MaxHScroll, MaxVScroll;
  Movie mMovie;
  RECT rcMovie, rcClient;
  WORD DisplayMode;
  MOVIEVI *MovieVI;
  HWND hWnd;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiMaxHScroll = AObjCreate(OTYPE_NUMBER);
  oiMaxVScroll = AObjCreate(OTYPE_NUMBER);

  MovieVI = (MOVIEVI *) AObjGetRuntimeInfo(oiMovie);
  if (!MovieVI)                            //There is no visual instance
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  hWnd = MovieVI->GetMovieWnd();                //Movie Window
  GetClientRect(hWnd,&rcClient);     //Window dimensions

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  if (!lpMovieDevRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  DisplayMode = lpMovieDevRec->DisplayMode;
  AObjUnlockData(oiMovie,MovieDev);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  GetMovieBox(mMovie,&rcMovie);                //Preferred rate of the movie

  if (DisplayMode != 1008)                 //RB_CROPPED
  {
    MaxHScroll = 0;
    MaxVScroll = 0;
  }
  else
  {
    MaxHScroll = rcClient.right - rcMovie.right;
    MaxVScroll = rcClient.bottom - rcMovie.bottom;
  }

  ONmbrSetInteger(oiMaxHScroll,MaxHScroll,TRUE);
  ONmbrSetInteger(oiMaxVScroll,MaxVScroll,TRUE);

  if (!AFuncSetParameter(1,oiMaxHScroll))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  if (!AFuncSetParameter(2,oiMaxVScroll))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieInfo_GetPreferredRate
// Description:
//   Reports the preferred rate ofthe movie as a floating point number.
// Inputs:
//   Movie Object
// Outputs:
//   1. Preferred Rate - Real Number
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_GetPreferredRate(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  AOBJECTID oiPrefRate;    //ID of the Temporary Object created to set Output Parameter 1
  MOVIERUNPTR lpMovieRunRec;
  long double PrefRate;
  LFIXED lfxRate;      //Number of units in the movie
  Movie mMovie;
  char RateStr[80];

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiPrefRate = AObjCreate(OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  lfxRate = GetMoviePreferredRate(mMovie);                //Preferred rate of the movie

  wsprintf(RateStr,"%d.%d",HIWORD(lfxRate),LOWORD(lfxRate));
  PrefRate = atof(RateStr);

  ONmbrSetReal(oiPrefRate,PrefRate,TRUE);

  if (!AFuncSetParameter(1,oiPrefRate))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieInfo_GetPreferredVolume
// Description:
//   Reports the preferred rate ofthe movie as a floating point number.
// Inputs:
//   Movie Object
// Outputs:
//   1. Preferred Volume - Whole Number
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_GetPreferredVolume(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  AOBJECTID oiPrefVolume;    //ID of the Temporary Object created to set Output Parameter 1
  MOVIERUNPTR lpMovieRunRec;
  SFIXED sfxVolume;      //Number of units in the movie
  Movie mMovie;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiPrefVolume = AObjCreate(OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  sfxVolume = GetMoviePreferredVolume(mMovie);                //Number of units in the movie

//  wsprintf(VolumeStr,"%d.%d",HIBYTE(sfxVolume),LOBYTE(sfxVolume));
//  PrefVolume = atof(VolumeStr);

  ONmbrSetReal(oiPrefVolume,sfxVolume,TRUE);

  if (!AFuncSetParameter(1,oiPrefVolume))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieInfo_GetTotalFrames
// Description:
//   Reports the length in seconds (a floating point number) of the specified movie.
// Inputs:
//   Movie Object
// Outputs:
//   1. Count - Whole Number
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_GetTotalFrames(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  AOBJECTID oiDuration;    //ID of the Temporary Object created to set Output Parameter 1
  MOVIERUNPTR lpMovieRunRec;
  TimeValue Duration;      //Number of units in the movie
  Movie mMovie;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  oiDuration = AObjCreate(OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  Duration = GetMovieDuration(mMovie);                //Number of units in the movie

  ONmbrSetInteger(oiDuration,Duration,TRUE);

  if (!AFuncSetParameter(1,oiDuration))
  {
    theSystem->message1 = 2;                     //Failed
    return;
  }

  theSystem->message1 = 1;                       //After getting

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieInfo_IsSoundTrack
// Description:
//   Determines whether the movie has a sound trck.
// Inputs:
//   Movie Object
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_IsSoundTrack(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  SoundDescription sdSoundInfo;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  sdSoundInfo.descSize = sizeof(SoundDescription);

  if (GetSoundInfo(mMovie,&sdSoundInfo) == noSoundTrackInMovie)
    theSystem->message1 = 2;                                    //Has No Sound
  else                                                          
    theSystem->message1 = 1;                                    //Has Sound
  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//  MovieInfo_IsVideoTrack
// Description:
//   Determines whether the movie has a video track.
// Inputs:
//   Movie Object
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  MovieInfo_IsVideoTrack(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  ImageDescription idVideoInfo;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
  AObjUnlockData(oiMovie,MovieRun);

  idVideoInfo.idSize = sizeof(ImageDescription);

  if (GetVideoInfo(mMovie,&idVideoInfo) == noVideoTrackInMovie)
    theSystem->message1 = 2;                                    //Has No Video
  else                                                          
    theSystem->message1 = 1;                                    //Has Video
  return;
}

