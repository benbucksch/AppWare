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
//This file contains 13 functions of the Movie section
//These functions are:
//      1       Movie_LoadMovie         (Load Movie)             !!!
//      2       Movie_IsMovieDone       (Movie Done?)            !!!
//      3       Movie_IsMovieLoaded     (Movie Loaded?)          !!!
//      4       Movie_IsMoviePlaying    (MoviePlaying?)          !!!
//      5       Movie_PlayMovie         (Play Movie)             !!!
//      6       Movie_PlayPreview       (Play Preview)           !!!
//      7       Movie_PlayUntilDone     (Play Until Done)        (not supported)
//      8       Movie_PrerollMovie      (Preroll Movie)          !!!
//      9       Movie_IsQuickTime       (Quick Time?)            !!!
//     10       Movie_SaveMovie         (Save Movie)             (not supported)
//     11       Movie_ShowMoviePoster   (Show Movie Poster)      !!!
//     12       Movie_StopMovie         (Stop Movie)             !!!
//     13       Movie_UpdateMovie       (Update Movie)           !!!
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
extern HINSTANCE hInstance;
extern BOOL GetMovieFile(HWND hDlg,AOBJECTID oiMovie, MovieFile  *lpmfMovie);

//////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_LoadMovie
// Description:
//   Opens Movie File with name passed in Input parameter.
//   If Input parameter is not passed the standat Dialog Box opened
// Inputs:
//   Full PathName of the movie. This parameter is optional 
////////////////////////////////////////////////////////////////////////// 
void ALMAPI  Movie_LoadMovie(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiPathName;          //ID of the TEXT Object contains path to the Movie
  AOBJECTID oiMovie;             //ID of the Temporary Object created to set Output Parameter
  AMEMBLOCKID  miBuffer;         //ID of the memory block
  MOVIERUNPTR lpMovieRunRec;
  UINT BufLen;
  char *path;
  MovieFile mfMovie;
  long how;
  ATYPEID theType;
  OSErr err;
  int cmp = 0;

  oiPathName = AFuncGetTypedParameter(1,OTYPE_TEXT);
  oiMovie = AFuncGetOutput(1,&theType);

  if (!oiMovie || (theType != OTYPE_MOVIE))
  {
    theSystem->message1 = 2;
    return;
  }

  if (oiPathName)
  {
    BufLen = OTextGetLength(oiPathName);
    miBuffer = AMemAllocate(BufLen + 1);
    path =(char *) AMemLock(miBuffer);
    OTextGetToBuffer(oiPathName,path,BufLen + 1);
    AMemUnlock(miBuffer);
  }

  if (oiPathName)
    cmp = strcmpi(path,"");

  if (cmp)
  {
    err = OpenMovieFile(path, &mfMovie, OF_READ);
    if (err != noErr)
    {
      theSystem->message1 = 2;     //Open File failed
      return;
    }
    else
    {
      MOVIEDEVPTR lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
      lpMovieDevRec->mfMovie = mfMovie;
      lstrcpy(lpMovieDevRec->mfMovieName, path);
      AObjUnlockData(oiMovie,MovieDev);
    }
  }
  else              
  {
    if (!GetMovieFile(GetActiveWindow(),oiMovie,&mfMovie))
    {
      theSystem->message1 = 1;     //Open File Canceled
      return;
    }
  }


  //Set output parameter
  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  if (lpMovieRunRec)
  {
    lpMovieRunRec->mMovie = NULL;
    lpMovieRunRec->Picture = NULL;
    lpMovieRunRec->bIsMovieDone = FALSE;
    lpMovieRunRec->ActiveInstance = NULL;
    lpMovieRunRec->bIsMoviePlaying = FALSE;
    lpMovieRunRec->LastClickTime = NULL;
    lpMovieRunRec->lfxRate = 0;
    lpMovieRunRec->sfxVolume = 0;
  }
  else
  {
      theSystem->message1 = 2;     //Is not done
      return;
  }

  GetMovieFromFile(mfMovie,oiMovie); //Movie File is closed in this function

  AObjUnlockData(oiMovie,MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtLoadMovie, AEvtGetObjectChangedPriority(how) );

  AEvtPostSignalAtMark(oiMovie, sChanged);

  theSystem->message1 = 1;         //After loading

  return;
}


//////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_IsMovieDone
// Description:
//   Generates oneof two signals based on whether the movie is"Done", i.e.
//   reached its last frame.
// Inputs:
//   Movie Object
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  Movie_IsMovieDone(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIERUNPTR lpMovieRunRec;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  if (oiMovie)
  {
    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    if (lpMovieRunRec)
    {
      if (lpMovieRunRec->bIsMovieDone)
      {
        theSystem->message1 = 1;     //Is done
      }
      else
        theSystem->message1 = 2;     //Is not done
    }
    else
    {
      theSystem->message1 = 2;     //Is not done
    }
  }

  AObjUnlockData(oiMovie,MovieRun);
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_IsMovieLoaded
// Description:
//   Generates oneof two signals based on whether there is a movie in the specified Movie object.
// Inputs:
//   Movie Object
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  Movie_IsMovieLoaded(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIERUNPTR lpMovieRunRec;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  if (oiMovie)
  {
    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    if (lpMovieRunRec)
    {
      if (lpMovieRunRec->mMovie)
      {
        theSystem->message1 = 1;     //Is loaded
      }
      else
        theSystem->message1 = 2;     //Is not loaded
    }
    else
    {
      theSystem->message1 = 2;       //Is not loaded
    }
  }

  AObjUnlockData(oiMovie,MovieRun);
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_IsMoviePlaying
// Description:
//   Generates oneof two signals based on whether the movie is currently playing.
// Inputs:
//   Movie Object
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  Movie_IsMoviePlaying(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIERUNPTR lpMovieRunRec;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  if (oiMovie)
  {
    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    if (lpMovieRunRec)
    {
      if (lpMovieRunRec->bIsMoviePlaying)
      {
        theSystem->message1 = 1;     //Is Playing
      }
      else
        theSystem->message1 = 2;     //Is not playing
    }
    else
    {
      theSystem->message1 = 2;     //Is not playing
    }
  }

  AObjUnlockData(oiMovie,MovieRun);
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_PlayMovie
// Description:
//   Starts the Movie in the specified Movie Object. Once the movie has begun playing, the After starting
//   signal flow is issued to call the next function, if any, in the chain. The movie will continue play
//   even while other processing is going on.   
// Inputs:
//   1. Movie Object
//   2. Rate - Optional
//   3. Volume - Optional               
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  Movie_PlayMovie(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiRate;
  AOBJECTID oiVolume;
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  LONG how;
  long /*double*/ Volume;
  LFIXED lfxRate;
  SFIXED sfxVolume;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiRate = AFuncGetTypedParameter(2,OTYPE_NUMBER);
  oiVolume = AFuncGetTypedParameter(3,OTYPE_NUMBER);

  if (oiMovie)
  {
    lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
    if (!lpMovieDevRec)
    {
      theSystem->message1 = 2;                        //Failed
      return;
    }

    AObjUnlockData(oiMovie,MovieDev);

    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    if (!lpMovieRunRec)
    {
      theSystem->message1 = 2;                        //Failed
      return;
    }

    mMovie = lpMovieRunRec->mMovie;
    AObjUnlockData(oiMovie,MovieRun);

    if (!mMovie) {
      theSystem->message1 = 2;                        //Failed
      return;
    }

  }
  else
  {
    MessageBox(NULL,"Movie Object is not passed",NULL,MB_OK);
    theSystem->message1 = 2;                                      //Failed
    return;
  }

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);

  if (oiRate)
  {
    long double ld;
    WORD        wInt;
    WORD        wFrac;

    ONmbrGetReal(oiRate, &ld);
    wInt = (WORD)ld;
    wFrac = (WORD)((ld - wInt) * 0xFFFF);
    lfxRate = MAKELFIXED(wInt, wFrac);
  }
  else
  {
    lfxRate = GetMoviePreferredRate(mMovie);
  }
  lpMovieRunRec->lfxRate = lfxRate;

  if (oiVolume)
  {
      Volume = ONmbrGetInteger(oiVolume);
      sfxVolume = MAKESFIXED((int)Volume,0);
  }
  else
  {
    sfxVolume = GetMoviePreferredVolume(mMovie);
  }
  lpMovieRunRec->sfxVolume = sfxVolume;
  AObjUnlockData(oiMovie, MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie,AEVENT_OBJECTCHANGED, EvtPlayMovie, AEvtGetObjectChangedPriority(how));
  AEvtPostSignalAtMark(oiMovie, sStarted);
  theSystem->message1 = 1;                 //After Starting

  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_PlayPreview
// Description:
//
// Inputs:
//   1. Movie Object
//   2. Rate - Optional
//   3. Volume - Optional               
//////////////////////////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI  Movie_PlayPreview(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  LONG how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  if (oiMovie)
  {
    lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
    if (!lpMovieDevRec)
    {
      theSystem->message1 = 2;                        //Failed
      return;
    }

    AObjUnlockData(oiMovie,MovieDev);

    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    if (!lpMovieRunRec)
    {
      theSystem->message1 = 2;                        //Failed
      return;
    }

    mMovie = lpMovieRunRec->mMovie;
    AObjUnlockData(oiMovie,MovieRun);

    if (!mMovie) {
      theSystem->message1 = 2;                        //Failed
      return;
    }
  }
  else
  {
    MessageBox(NULL,"Movie Object is not passed",NULL,MB_OK);
    theSystem->message1 = 2;                                      //Failed
    return;
  }

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);

  lpMovieRunRec->lfxRate = GetMoviePreferredRate(mMovie);
  lpMovieRunRec->sfxVolume = GetMoviePreferredVolume(mMovie);

  AObjUnlockData(oiMovie, MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie,AEVENT_OBJECTCHANGED, EvtPlayPreview, AEvtGetObjectChangedPriority(how));
  AEvtPostSignalAtMark(oiMovie, sStarted);
  theSystem->message1 = 1;                 //After Starting

  return;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_PrerollMovie
// Description:
//   Prepares a portion of a movie for playback, to enhance playback performance.
//   Playback performance can be improved if PrerollMovie is called prior to playing a movie.
// Inputs:
//   1. Movie Object
//   2. Rate - Optional
////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI  Movie_PrerollMovie(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  AOBJECTID oiRate;
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  LONG how;
  double Rate;
  LFIXED lfxRate;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiRate = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  if (oiMovie == 0)
  {
    MessageBox(NULL,"Movie Object is not passed",NULL,MB_OK);
    theSystem->message1 = 2;                                      //Failed
    return;
  }

  if (oiRate)
  {
    Rate = ONmbrGetInteger(oiRate);      //Rate is incremented/decremented by 1
    if (Rate < 0)
    {
      theSystem->message1 = 2;           //Failed. Play until done plays forward only
      return; 
    }
    lfxRate = MAKELFIXED(Rate,0);
  }
  else
  {
    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    mMovie = lpMovieRunRec->mMovie;
    AObjUnlockData(oiMovie, MovieRun);
    lfxRate = GetMoviePreferredRate(mMovie);
  }

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
  lpMovieRunRec->lfxRate = lfxRate;
  AObjUnlockData(oiMovie, MovieRun);

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtPrerollMovie, AEvtGetObjectChangedPriority(how) );
  theSystem->message1 = 1;                 //After Starting

  return;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_IsQuickTime
// Description:
////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI  Movie_IsQuickTime(pAEvtInfo, pAObjMessage theSystem)
{
   if (QTInstall) 
     theSystem->message1 = 1;
   else
     theSystem->message1 = 2;

  return;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_ShowMoviePoster
// Description:
//   Retrieves a movie's poster frame in the QuickTime for Windows picture format.
//   This function may be called whether a movie is playing or not.
//   The picture object returned is unusable by Windows directly.
//   To converting the image is using DrawPicture to display it at specified coordinates.
//   If no poster is available, the After showing signal is still issued.
// Inputs:
//   1. Movie Object
////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI  Movie_ShowMoviePoster(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  LONG how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  if (oiMovie == 0)
  {
    MessageBox(NULL,"Movie Object is not passed",NULL,MB_OK);
    theSystem->message1 = 1;                                      //Failed
    return;
  }

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtDrawPoster, AEvtGetObjectChangedPriority(how) );
  theSystem->message1 = 1;                 //After Starting

  return;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_StopMovie
// Description:
//   Stop the specified movie. The second signal, If failed, is issued if there is no movie in
//   the specified Movie object.
// Inputs:
//   1. Movie Object
////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI  Movie_StopMovie(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  LONG how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  if (oiMovie)
  {
    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);

    if (lpMovieRunRec)
    {
      mMovie = lpMovieRunRec->mMovie;
      if (!mMovie) {
        AObjUnlockData(oiMovie,MovieRun);
        theSystem->message1 = 2;                     //Failed
        return;
      }
    }
    else
    {
      theSystem->message1 = 2;                     //Failed
      return;
    }

    AObjUnlockData(oiMovie,MovieRun);
  }
  else
  {
    MessageBox(NULL,"Movie Object is not passed",NULL,MB_OK);
    theSystem->message1 = 2;                                      //Failed
    return;
  }
  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, EvtStopMovie, AEvtGetObjectChangedPriority(how) );
  AEvtPostSignalAtMark(oiMovie, sStopped);

  theSystem->message1 = 1;
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   Movie_UpdateMovie
// Description:
//   Stop the specified movie. The second signal, If failed, is issued if there is no movie in
//   the specified Movie object.
// Inputs:
//   1. Movie Object
////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI  Movie_UpdateMovie(pAEvtInfo, pAObjMessage theSystem)
{
  AOBJECTID oiMovie;             //ID of the Input Parameter
  LONG how;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  if (oiMovie == 0)
  {
    MessageBox(NULL,"Movie Object is not passed",NULL,MB_OK);
    theSystem->message1 = 2;                                      //Failed
    return;
  }

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiMovie, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how) );
  AEvtPostSignalAtMark(oiMovie, sViewChanged);

  theSystem->message1 = 1;
  return;
}


OSErr MOVIEVI::LoadMovie(const char *Path)
{
  MovieFile mfMovie;
  OSErr err;
  char str[80], errName[80];
      
  if (mMovie)
  {
    DisposeMovie(mMovie);
    mMovie = NULL;
  }

  if ((OpenMovieFile (Path, &mfMovie, OF_READ)) == noErr)
  {
    err = NewMovieFromFile (&mMovie, mfMovie, NULL, NULL,0, NULL);
    if (err)//(!mMovie)
    {
      LoadString(hInstance,(UINT)err,str,80);
      wsprintf(errName," NewMoviefromFile Failure\n\nQuickTime Error # %li\n\n%s",err,str);
      MessageBox (NULL,errName,NULL, MB_ICONHAND);
      return 0;
     }
     err = CloseMovieFile(mfMovie);
  }
  ::SetMovie(hPaintWnd, m_oiObject,mController,mMovie);

  return err;
}

void MOVIEVI::PlayMovie()
{
  MOVIERUNPTR lpMovieRunRec;
  BOOL ActSave;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);

  //Start Movie
  if (lpMovieRunRec->trSelDuration.value.dwLo == 0)
    MCDoAction(mController,mcActionSetPlaySelection,(LPVOID)FALSE);
  else
    MCDoAction(mController,mcActionSetPlaySelection,(LPVOID)TRUE);

  MCDoAction(mController, mcActionSetVolume, (LPVOID) lpMovieRunRec->sfxVolume);   //Set Volume
  ActSave = IsActivated();
  SetActivated(TRUE);
  MCDoAction(mController, mcActionPlay, (LPVOID) lpMovieRunRec->lfxRate);          //Play Movie

  SetActivated(ActSave);

  
  lpMovieRunRec->bIsMoviePlaying = TRUE;
  lpMovieRunRec->bIsMovieDone = FALSE;

  AObjUnlockData(m_oiObject, MovieRun);
}

void MOVIEVI::PlayPreview()
{
  MOVIERUNPTR lpMovieRunRec;
  BOOL ActSave;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);
  //Start Movie
  MCDoAction(mController,mcActionSetPlaySelection,(LPVOID)TRUE);
  MCDoAction(mController, mcActionSetVolume, (LPVOID) lpMovieRunRec->sfxVolume);   //Set Volume
  ActSave = IsActivated();
  SetActivated(TRUE);
  MCDoAction(mController, mcActionPlay, (LPVOID) lpMovieRunRec->lfxRate);          //Play Movie

  SetActivated(ActSave);

  lpMovieRunRec->bIsMoviePlaying = TRUE;
  lpMovieRunRec->bIsMovieDone = FALSE;
  AObjUnlockData(m_oiObject, MovieRun);

}


////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MOVIEVI::PrerollMovie
// Description:
//   for Visual Instance
////////////////////////////////////////////////////////////////////////////////////////////////
void MOVIEVI::PrerollMovie()
{
  MOVIERUNPTR lpMovieRunRec;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);

  ::PrerollMovie(mMovie,0,lpMovieRunRec->lfxRate);

  AObjUnlockData(m_oiObject, MovieRun);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Function:
//   MOVIEVI::ShowMoviePoster
// Description:
//   for Visual Instance
////////////////////////////////////////////////////////////////////////////////////////////////
void MOVIEVI::ShowMoviePoster()
{
  TimeValue tvLocation;
  TimeRecord trRecord;

  // Advance the movie to the poster frame
  tvLocation = GetMoviePosterTime (mMovie);
  trRecord.value.dwLo = tvLocation;
  trRecord.value.dwHi = 0;
  trRecord.scale = GetMovieTimeScale (mMovie);
  MCDoAction(mController, mcActionGoToTime, (LPVOID) &trRecord);
}


void MOVIEVI::StopMovie()
{
  MOVIERUNPTR lpMovieRunRec;
  BOOL ActSave;

  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(m_oiObject, MovieRun);

  ActSave = IsActivated();
  SetActivated(TRUE);

  MCDoAction(mController, mcActionPlay,0);
  lpMovieRunRec->bIsMoviePlaying = FALSE;
  SetActivated(ActSave);

  if (GetMovieDuration(mMovie) == MCGetCurrentTime(mController,NULL))
  {
    lpMovieRunRec->bIsMovieDone = TRUE;
    AEvtPostSignalAtMark(m_oiObject, sDone);
  }
  AObjUnlockData(m_oiObject, MovieRun);
}
