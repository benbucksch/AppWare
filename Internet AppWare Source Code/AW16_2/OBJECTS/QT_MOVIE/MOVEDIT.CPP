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

//////////////////////////////////////////////////////////////////////////////////////////////
//This file contains 18 functions of the Movie Display section
//These functions are:
//      1       MovieEditing_ClearSelection             (Clear Selection)               !!!
//      2       MovieEditing_ClipboardToMovie           (Clipboard to Movie)            (not supported, in Win)
//      3       MovieEditing_CopyToMovie                (Copy to Movie)                 (not supported, in Win)
//      4       MovieEditing_CutToMovie                 (Cut to Movie)                  (not supported, in Win)
//      5       MovieEditing_DisableEditing             (Disable Editing)               (not supported, in Win)
//      6       MovieEditing_IsEditable                 (Editable?)                     (not supported, in Win)
//      7       MovieEditing_EnableEditing              (Enable Editing)                (not supported, in Win)
//      8       MovieEditing_GetSelDuration             (Get Sel Duration)              !!!
//      9       MovieEditing_GetSelTime                 (Get Sel Time)                  !!!
//     10       MovieEditing_InsertEmptyTime            (Insert Empty Time)             (not supported, in Win)
//     11       MovieEditing_MovieToClipBoard           (Movie to Clipboard)            (not supported, in Win)
//     12       MovieEditing_MovieToSelection           (Movie to Selection)            (not supported, in Win)
//     13       MovieEditing_OverlaySelection           (Overlay Selection)             (not supported, in Win)
//     14       MovieEditing_ScaleSelection             (Scale Selection)               !!!
//     15       MovieEditing_SelectionToSelection       (Selection to Selection)        (not supported, in Win)
//     16       MovieEditing_SetSelDuration             (Set Sel Duration)              !!!
//     17       MovieEditing_SetSelTime                 (Set Sel Time)                  !!!
//     18       MovieEditing_SetUndo                    (Set Undo)                      (not supported, in Win)
//////////////////////////////////////////////////////////////////////////////////////////////

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
//  MovieEditing_ClearSelection
// Description:
//   Deletes the selected segment of the movie
// Inputs:
//   Movie Object
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieEditing_ClearSelection(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  MOVIERUNPTR lpMovieRunRec;
  MOVIEDEVPTR lpMovieDevRec;
  Movie mMovie;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    if(lpMovieDevRec)
      AObjUnlockData(oiMovie,MovieDev);
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;
    
  if (!lpMovieDevRec)
  {
    AObjUnlockData(oiMovie,MovieDev);
    theSystem->message1 = 2;               //Failed
    return;
  }

  //Clear Selection
  lpMovieRunRec->trSelBegin.value.dwLo = (DWORD) -1;
  lpMovieRunRec->trSelBegin.value.dwHi = 0;
  lpMovieRunRec->trSelBegin.scale = GetMovieTimeScale (mMovie);
  lpMovieRunRec->trSelBegin.base = TIMEBASE_DEFAULT;
  lpMovieRunRec->trSelDuration.value.dwLo = 0;
  lpMovieRunRec->trSelDuration.value.dwHi = 0;
  lpMovieRunRec->trSelDuration.scale = GetMovieTimeScale (mMovie);
  lpMovieRunRec->trSelDuration.base = NULL;

  AObjUnlockData(oiMovie,MovieDev);
  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtClearSelection, AEvtGetObjectChangedPriority(how) );
  AEvtPostSignalAtMark(oiMovie, sViewChanged);

  theSystem->message1 = 1;
  return;
}


//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieEditing_GetSelDuration
// Description:
//   Deletes the selected segment of the movie
// Inputs:
//   Movie Object
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieEditing_GetSelDuration(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiDuration;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TimeValue tvDuration;
  long double Duration;

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
  tvDuration = lpMovieRunRec->trSelDuration.value.dwLo;
  Duration = ((long double)tvDuration) / GetMovieTimeScale(mMovie);

  AObjUnlockData(oiMovie,MovieRun);

  ONmbrSetReal(oiDuration,Duration,TRUE);

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
//   MovieEditing_GetSelTime
// Description:
//   Deletes the selected segment of the movie
// Inputs:
//   Movie Object
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieEditing_GetSelTime(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiTime;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TimeValue tvStart;
  long double Time;

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
  tvStart =  lpMovieRunRec->trSelBegin.value.dwLo;
  if (tvStart == (DWORD)-1)
    tvStart = 0;

  Time = tvStart / GetMovieTimeScale(mMovie);
  AObjUnlockData(oiMovie,MovieRun);

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
//   MovieEditing_ScaleSelection
// Description:
//   Deletes the selected segment of the movie
// Inputs:
//   1. Movie Object
//   2. Time Scale
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieEditing_ScaleSelection(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiTimeScale;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TimeValue tvDuration;
  long double TimeScale;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiTimeScale = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;

  tvDuration = lpMovieRunRec->trSelDuration.value.dwLo;

  ONmbrGetReal(oiTimeScale,&TimeScale);

  if (TimeScale == 1)
  {
    theSystem->message1 = 1;
    return;
  }

  tvDuration *= TimeScale;

  lpMovieRunRec->trSelDuration.value.dwLo = tvDuration;
  lpMovieRunRec->trSelDuration.value.dwHi = 0;
  lpMovieRunRec->trSelDuration.scale = GetMovieTimeScale (mMovie);
  lpMovieRunRec->trSelDuration.base = TIMEBASE_DEFAULT;

  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtSetSelection, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieEditing_SetSelDuration
// Description:
//   Deletes the selected segment of the movie
// Inputs:
//   1. Movie Object
//   2. Duration
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieEditing_SetSelDuration(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiDuration;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TimeValue tvDuration;
  long double Duration;
  long how;

  //Retrieve input parameter
  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiDuration = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (!lpMovieRunRec)
  {
    theSystem->message1 = 2;               //Failed
    return;
  }

  mMovie = lpMovieRunRec->mMovie;

  ONmbrGetReal(oiDuration,&Duration);

  tvDuration = Duration * GetMovieTimeScale (mMovie);;

  lpMovieRunRec->trSelDuration.value.dwLo = tvDuration;
  lpMovieRunRec->trSelDuration.value.dwHi = 0;
  lpMovieRunRec->trSelDuration.scale = GetMovieTimeScale (mMovie);
  lpMovieRunRec->trSelDuration.base = TIMEBASE_DEFAULT;

  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtSetSelection, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;

  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieEditing_SetSelTime
// Description:
//   Deletes the selected segment of the movie
// Inputs:
//   1. Movie Object
//   2. Duration
// Outputs:
//   None
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieEditing_SetSelTime(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;
  OBJECTID oiTime;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TimeValue tvStart;
  long double StartTime;
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

  ONmbrGetReal(oiTime,&StartTime);

  tvStart = StartTime * GetMovieTimeScale (mMovie);;

  lpMovieRunRec->trSelBegin.value.dwLo = tvStart;
  lpMovieRunRec->trSelBegin.value.dwHi = 0;
  lpMovieRunRec->trSelBegin.scale = GetMovieTimeScale (mMovie);
  lpMovieRunRec->trSelBegin.base = TIMEBASE_DEFAULT;

  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtSetSelectionTime, AEvtGetObjectChangedPriority(how) );

  theSystem->message1 = 1;

  return;
}


void MOVIEVI::ClearSelection()
{
  MOVIERUNPTR lpMovieRunRec;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);

  MCDoAction (mController, mcActionSetSelectionBegin, (LPVOID) &(lpMovieRunRec->trSelBegin));
  MCDoAction (mController, mcActionSetSelectionDuration, (LPVOID) &(lpMovieRunRec->trSelDuration));

  AObjUnlockData(m_oiObject, MovieRun);
}


void MOVIEVI::SetSelDuration()
{
  MOVIERUNPTR lpMovieRunRec;
  TimeRecord trRecord;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);

  trRecord.value.dwLo = lpMovieRunRec->trSelDuration.value.dwLo;
  trRecord.value.dwHi = lpMovieRunRec->trSelDuration.value.dwHi;
  trRecord.scale = lpMovieRunRec->trSelDuration.scale;
  trRecord.base = lpMovieRunRec->trSelDuration.base;

  MCDoAction (mController, mcActionSetSelectionDuration, (LPVOID) &trRecord);
  AObjUnlockData(m_oiObject, MovieRun);
}


void MOVIEVI::SetSelectionTime()
{
  MOVIERUNPTR lpMovieRunRec;
  TimeRecord trRecord;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);

  trRecord.value.dwLo = lpMovieRunRec->trSelBegin.value.dwLo;
  trRecord.value.dwHi = lpMovieRunRec->trSelBegin.value.dwHi;
  trRecord.scale = lpMovieRunRec->trSelBegin.scale;
  trRecord.base = lpMovieRunRec->trSelBegin.base;

  MCDoAction (mController, mcActionSetSelectionBegin, (LPVOID) &trRecord);
  AObjUnlockData(m_oiObject, MovieRun);
}
