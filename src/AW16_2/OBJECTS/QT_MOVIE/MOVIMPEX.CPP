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
//This file contains 2 functions of the Movie Import/Export section
//These functions are:
//      1       MovieImpExp_MovieToPict         (Load Movie)             !!!
//      2       MovieImpExp_PosterToPict        (Movie Done?)            !!!
/////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <o_number.h>
#include <o_pict.h>

#include "qtw.h"
#include "moviedat.h"
#include "movieexp.h"

MEMBLOCKID CopyHandle (HANDLE h);

//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieImpExp_MovieToPict
// Description:
//   Converts a movie frame to a BMP image wich you can pass to a Picture object.
//   
// Inputs:
//   1. Movie
//   2. Time - Real number      
////////////////////////////////////////////////////////////////////////// 
extern "C" void ALMAPI _export MovieImpExp_MovieToPict(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;             //ID of the Temporary Object created to set Output Parameter
  OBJECTID oiPictOut;
  OBJECTID oiTime;
  MEMBLOCKID  miDIB;         //ID of the memory block
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TYPEID theType;
  PicHandle phPicture;
  DIBHandle hdPicture;
  AEvtInfo MovieToPictEvt;
  pAEvtInfo  pMovieToPictEvt = &MovieToPictEvt;
  TimeValue tvTime;
  TimeScale tsScale;
  long double Time;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);
  oiTime = AFuncGetTypedParameter(2,OTYPE_NUMBER);

  if (oiMovie)
  {
    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    mMovie = lpMovieRunRec->mMovie;
    AObjUnlockData(oiMovie,MovieRun);
  }
  else
  {
    theSystem->message1 = 2;     //Open File failed
    return;
  }

  if (oiTime)
  {
    ONmbrGetReal(oiTime,&Time);
    tsScale = GetMovieTimeScale(mMovie);
    tvTime = (LONG)(Time * tsScale);
  }
  else
  {
    theSystem->message1 = 2;     //Open File failed
    return;
  }


  phPicture = GetMoviePict(mMovie,tvTime);
  hdPicture = PictureToDIB(phPicture);
  miDIB = CopyHandle(hdPicture);

  oiPictOut = AFuncGetOutput(1,&theType);

  if (oiPictOut && (AObjCheckType(oiPictOut,OTYPE_PICTURE)))
  {
    pMovieToPictEvt->details = (long)miDIB;
    pMovieToPictEvt->sourceObject = oiMovie;
    pMovieToPictEvt->targetObject = oiPictOut;
    pMovieToPictEvt->what = AEVT_BITMAPTOOBJECT;
    pMovieToPictEvt->how =  AEVT_ATONCE | AEVT_DIRECT;

    AEvtPost(pMovieToPictEvt);
  }

  GlobalFree(hdPicture);
  AMemFree(miDIB);

  theSystem->message1 = 1;
  return;
}

//////////////////////////////////////////////////////////////////////////
// Function:
//   MovieImpExp_PosterToPict
// Description:
//   Returns a current poster as a Picture object.
//   
// Inputs:
//   1. Movie
//////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI _export MovieImpExp_PosterToPict(pAEvtInfo, pAObjMessage theSystem)
{
  OBJECTID oiMovie;             //ID of the Temporary Object created to set Output Parameter
  OBJECTID oiPictOut;
  MEMBLOCKID  miDIB;         //ID of the memory block
  MOVIERUNPTR lpMovieRunRec;
  Movie mMovie;
  TYPEID theType;
  PicHandle phPicture;
  DIBHandle hdPicture;
  AEvtInfo MovieToPictEvt;
  pAEvtInfo  pMovieToPictEvt = &MovieToPictEvt;

  oiMovie = AFuncGetTypedParameter(1,OTYPE_MOVIE);

  if (oiMovie)
  {
    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    mMovie = lpMovieRunRec->mMovie;
    AObjUnlockData(oiMovie,MovieRun);
  }
  else
  {
    theSystem->message1 = 2;     //Open File failed
    return;
  }

  phPicture = GetMoviePosterPict(mMovie);
  hdPicture = PictureToDIB(phPicture);
  miDIB = CopyHandle(hdPicture);

  oiPictOut = AFuncGetOutput(1,&theType);

  if (oiPictOut && (AObjCheckType(oiPictOut,OTYPE_PICTURE)))
  {
    pMovieToPictEvt->details = (long)miDIB;
    pMovieToPictEvt->sourceObject = oiMovie;
    pMovieToPictEvt->targetObject = oiPictOut;
    pMovieToPictEvt->what = AEVT_BITMAPTOOBJECT;
    pMovieToPictEvt->how =  AEVT_ATONCE | AEVT_DIRECT;

    AEvtPost(pMovieToPictEvt);
  }

  GlobalFree(hdPicture);
  AMemFree(miDIB);

  theSystem->message1 = 1;
  return;
}


MEMBLOCKID CopyHandle (HANDLE h)
{
   BYTE huge   *lpCopy;
   BYTE huge   *lp;
   MEMBLOCKID  miCopy;
   DWORD       dwLen;

   if (!h)
      return NULL;

   dwLen = GlobalSize (h);

   miCopy = AMemAllocate(dwLen);

   if (miCopy)
   {
      lpCopy = (BYTE huge *)AMemLock(miCopy);
      lp     = (BYTE huge *)GlobalLock (h);

      while (dwLen--) 
         *lpCopy++ = *lp++;

      AMemUnlock(miCopy);
      GlobalUnlock (h);
//      AMemFree(miCopy);  memory is freed outside this procedure
   }

   return miCopy;
   }
