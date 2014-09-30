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
// File Name:  MovAdmn.CPP
//              Copyright 1993 Novell Inc.
// Description.:
//      The Administration Routine for the AppWare Movie object.
/////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <mem.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <a_alm.h>

#include "moviedat.h"
#include "movieexp.h"
#include "movctrl.h"

extern BOOL QTInstall;

LONG CreateObject(AOBJECTID oiMovie,pAObjMessage theSystem) ;
LONG DestroyObject(AOBJECTID oiMovie, pAObjMessage theSystem );
LONG CheckObject( AOBJECTID oiMovie, pAObjMessage theSystem );
LONG ValidateObjectIDs( AOBJECTID oiMovie, pAObjMessage theSystem );
LONG BuildSignals(pAObjMessage theSystem) ;
LONG AssignObject(AOBJECTID oiDestination, pAObjMessage theSystem);
LONG ExportObject(AOBJECTID oiMovie);
LONG ImportObject(AOBJECTID oiMovie);
LONG CheckVersion(AOBJECTID oiMovie);
void InitMovieObject(AOBJECTID objID);

extern LONG EditObject(AOBJECTID oiMovie);
LONG ObjectAwakened(AOBJECTID oiMovie);
LONG ObjectAsleep(AOBJECTID oiMovie);

OpWndItemD* ALMAPI  AMovieCreate(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status,long special);

const char * const signals[] = {
        {"Movie Changed"},
        {"Done"},
        {"Single Click"},
        {"Double Click"},
        {"Started"},
        {"Stopped"},
        {"View Changed"},
        {"Activated"},
        {"Deactivated"}
        };

/////////////////////////////////////////////////////////////////////
// Function:  AdministrationRoutine
// Description:  Creates the and initializes the data structure for
//   a Movie Object when the AOBJ_CREATED message is received
/////////////////////////////////////////////////////////////////////
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/  MovieAdministration(AOBJECTID oiMovie, pAObjMessage theSystem)
{
  pApQueryInfo pQueryInfo;
  long rVal = A_NOTHANDLED;

  switch (theSystem->message1)
  {
    case AOBJ_CREATED:
      return CreateObject(oiMovie,theSystem) ;

    case AOBJ_DESTROYED:
      return DestroyObject(oiMovie,theSystem);

    case AOBJ_EDITOBJECT:
      return EditObject(oiMovie) ;

    case AOBJ_CHECKOBJECT:
      return CheckObject (oiMovie, theSystem) ;

    case AOBJ_QUERYPROTOCOL:
      pQueryInfo = (pApQueryInfo) theSystem->message4;

      if (pQueryInfo->protocol == APROT_WND)  // if the window protocol is supported
      {
        rVal = AP_SUPPORTED;
        pQueryInfo->pfnCreate = (APPCFN) MovieVICreate; // create function for movie instance
      }
      return rVal;

    case AOBJ_VALIDATEOBJECTIDS :
      return ValidateObjectIDs(oiMovie, theSystem) ;

    case AOBJ_BUILDSIGNALS:
      return BuildSignals(theSystem) ;

    case AOBJ_ASSIGNOBJECT:
      return AssignObject(oiMovie, theSystem) ;

    case AOBJ_IMPORTOBJECT :
      return ImportObject(oiMovie) ;

    case AOBJ_EXPORTOBJECT :
      return ExportObject(oiMovie) ;

    case AOBJ_AWAKENED :
      return ObjectAwakened(oiMovie);

    case AOBJ_READ :
      return CheckVersion(oiMovie) ;

    case AOBJ_ASLEEP :
      return ObjectAsleep(oiMovie);

    case AOBJ_POSTWRITE:
      break;

    case AOBJ_PREWRITE:
      break;
  }
  return A_NOTHANDLED ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  CreateObject
//  Description:  Creates a Movie object with default values
////////////////////////////////////////////////////////////////////////
LONG CreateObject(AOBJECTID oiMovie, pAObjMessage)
{
  AOBJECTID  dObject;
  char      errorString[255];
  long rVal = A_OK;
  OSErr err;
  errorString[0] = '\0';

  MOVIEDEVPTR lpMovieDevData;
  MOVIERUNPTR lpMovieRunRec;
  LPMOVIESETREC lpMovieSetRec;
  LPMOVIEFRAMEREC lpMovieFrameRec;

  if (QTInstall)
  {
    err = EnterMovies();
    if (err != noErr)
    {
      MessageBox (NULL, "EnterMovies failure", NULL, MB_OK);
      return 0;
    }
  }

  dObject = (AOBJECTID)oiMovie;
  AObjSetDataCount(dObject, NumOfDataBlocks);

  //Set Data Block #0
  AObjResizeData(dObject, MovieDev, sizeof(MOVIEDEVREC));
  lpMovieDevData = (MOVIEDEVPTR) AObjLockData(dObject, MovieDev);
    lpMovieDevData->version = MovieDataVersion;
    lpMovieDevData->mfMovie = NULL;
    setmem(lpMovieDevData->mfMovieName,255,0);      //empty string
    lpMovieDevData->mController = NULL;
    lpMovieDevData->oiController = 0;
    lpMovieDevData->DisplayMode = RB_SIZED;  // Crop, fitToScreen, OrigProp, etc
    lpMovieDevData->bShowBadge = FALSE;      //it must be FALSE for Sized mode because of the bug in QuickTime
    lpMovieDevData->bShowBorder = TRUE;
    lpMovieDevData->bShowActBorder = TRUE;
    lpMovieDevData->bEditable = TRUE;
    lpMovieDevData->bShowLogo = TRUE;
    lpMovieDevData->bAutoStop = TRUE;
    lpMovieDevData->bShowController = TRUE;
    ////////////////////////////////////////////////////////////////
    //Design time data
    lpMovieDevData->oiSelf = dObject;
    lpMovieDevData->bShared = FALSE;
  AObjUnlockData(dObject, MovieDev);

  //Set Data Block #1
  AObjResizeData(dObject, MovieRun, sizeof(MOVIERUNREC));
  lpMovieRunRec = (MOVIERUNPTR) AObjLockData(dObject,MovieRun);
    lpMovieRunRec->mMovie = NULL;
    lpMovieRunRec->Picture = NULL;
    lpMovieRunRec->bIsMovieDone = FALSE;
    lpMovieRunRec->ActiveInstance = NULL;
    lpMovieRunRec->bIsMoviePlaying = FALSE;
    lpMovieRunRec->LastClickTime = NULL;
    lpMovieRunRec->lfxRate = 0;
    lpMovieRunRec->sfxVolume = 0;

  //Clear Selection
  lpMovieRunRec->trSelBegin.value.dwLo = (DWORD) -1;
  lpMovieRunRec->trSelBegin.value.dwHi = 0;
  lpMovieRunRec->trSelBegin.scale = 0;
  lpMovieRunRec->trSelBegin.base = TIMEBASE_DEFAULT;
  lpMovieRunRec->trSelBegin.value.dwLo = 0;
  lpMovieRunRec->trSelBegin.base = NULL;

  lpMovieRunRec->trSelDuration.value.dwLo = 0;
  lpMovieRunRec->trSelDuration.value.dwHi = 0;
  lpMovieRunRec->trSelDuration.scale = 0;
  lpMovieRunRec->trSelDuration.base = TIMEBASE_DEFAULT;
  lpMovieRunRec->trSelDuration.value.dwLo = 0;
  lpMovieRunRec->trSelDuration.base = NULL;

  lpMovieRunRec->LoopValue = 0;
  AObjUnlockData(dObject,MovieRun);

  //Set Data Block #2
  AObjResizeData(dObject, MovieSet, sizeof(MOVIESETREC));
  lpMovieSetRec = (LPMOVIESETREC) AObjLockData(dObject,MovieSet);
    lpMovieSetRec->mMovie = NULL;
    lpMovieSetRec->mfMovie = NULL;
    lpMovieSetRec->mController = NULL;
    lpMovieSetRec->bSaved = FALSE;
    lpMovieSetRec->bEdit = FALSE;
  AObjUnlockData(dObject,MovieSet);

  AObjResizeData(dObject, MovieMooV, 0); // data #3       ???
  AObjResizeData(dObject, MoviePICT, 0); // data #4       ???

  //Set Data Block #5 - Movie Rectangles
  AObjResizeData(dObject, MovieFrame, sizeof(MOVIEFRAMEREC));
  lpMovieFrameRec = (LPMOVIEFRAMEREC) AObjLockData(dObject,MovieFrame);
    SetRectEmpty(&(lpMovieFrameRec->SetMovieRect));
    SetRectEmpty(&(lpMovieFrameRec->RunMovieRect));
    SetRectEmpty(&(lpMovieFrameRec->Frame));
    SetRectEmpty(&(lpMovieFrameRec->ActFrame));
    lpMovieFrameRec->HScroll = 0;
    lpMovieFrameRec->VScroll = 0;
    lpMovieFrameRec->Scale = 1;
  AObjUnlockData(dObject,MovieFrame);

  AObjResizeData(dObject, MovieCallBack, 0); // data #6   ???

  return rVal ;
}

/////////////////////////////////////////////////////////////////////
//  DestroyObject
//  Updates Movie Object when it is to be destroyed
///////////////////////////////////////////////////////////////////// 
LONG DestroyObject(AOBJECTID oiMovie, pAObjMessage)
{
  AOBJECTID oiController;
  MOVIEDEVPTR lpMovieDevRec;
  LPMCTRLDEV lpMCtrlDevRec;

  lpMovieDevRec =(MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  if (lpMovieDevRec)
  {
    oiController = lpMovieDevRec->oiController;
    if (oiController)
    {
      lpMCtrlDevRec = (LPMCTRLDEV) AObjLockData(oiController,MCtrlDev);
      lpMCtrlDevRec->oiMovie = 0;
      lpMCtrlDevRec->oiMovieShared = 0;
      AObjUnlockData(oiController,MCtrlDev);
    }
    oiController = 0;
  }
  AObjUnlockData(oiMovie,MovieDev);

  return A_OK;
}

/////////////////////////////////////////////////////////////////////
//  CheckObject
//  Verifies that the Movie object is ready to be compiled.
/////////////////////////////////////////////////////////////////////
LONG CheckObject( AOBJECTID, pAObjMessage)
{
  long rVal = A_OK;

  return rVal ;
}

/////////////////////////////////////////////////////////////////////
//  ValidateObjectIDs
//  Store Object ID of a Controller Object
/////////////////////////////////////////////////////////////////////
LONG ValidateObjectIDs( AOBJECTID oiMovie, pAObjMessage theSystem )
{
  //Movie Object doesn't contain any object ID
  MOVIEDEVPTR lpMovieDevRec;

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);
  AObjValidateID(&lpMovieDevRec->oiController,theSystem);
  AObjUnlockData(oiMovie,MovieDev);

  return A_OK ;
}

/////////////////////////////////////////////////////////////////////
//  BuildSignals
//  Builds an array of Movie object signal names.
/////////////////////////////////////////////////////////////////////
LONG BuildSignals(pAObjMessage theSystem)
{
  AMEMBLOCKID signalBlock ;
  AObjSignalRecord  *lpSignal ;
  int i, nSignals ;

  // a memory block handle is passed to you
  signalBlock = (AMEMBLOCKID)theSystem->message2 ;
  // determine the number of signals you generate
  nSignals = sizeof(signals)/sizeof(signals[0]) ;
  // size the memory block based on the number of signals you generate
  AMemSetSize(signalBlock, nSignals*sizeof(AObjSignalRecord)) ;
  // get a pointer to the memory block
  lpSignal = (AObjSignalRecord *)AMemLock(signalBlock) ;
  for (i = 0; i < nSignals; i++)
  {
    lpSignal[i].theID = i ;  // the signal number
    lstrcpy(lpSignal[i].theName, signals[i]) ;  // the signal name
  }
  AMemUnlock(signalBlock) ;
  return A_OK ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  AssignObject
//  Description:  
////////////////////////////////////////////////////////////////////////
LONG AssignObject(AOBJECTID oiDestination, pAObjMessage theSystem)
{
  AOBJECTID oiSource ;
  MOVIEDEVPTR pSourceDev, pDestDev;
  MOVIERUNPTR pSourceRun, pDestRun;
  LPMOVIESETREC pSourceSet, pDestSet;
  LPMOVIEFRAMEREC pSourceFrame, pDestFrame;
  long how;

  oiSource = (AOBJECTID) theSystem->message3 ; // source object ID

  pDestDev = (MOVIEDEVPTR) AObjLockData(oiDestination, MovieDev) ;
  pSourceDev = (MOVIEDEVPTR) AObjLockData(oiSource, MovieDev) ;
  pDestDev->version =  pSourceDev->version;

  CopyMemory(pDestDev->mfMovieName,pSourceDev->mfMovieName,sizeof(pDestDev->mfMovieName));

//  pDestDev->oiController =  pSourceDev->oiController;
  pDestDev->DisplayMode =  pSourceDev->DisplayMode;
  pDestDev->bShowBorder =  pSourceDev->bShowBorder;
  pDestDev->bShowBadge =  pSourceDev->bShowBadge;
  pDestDev->bShowActBorder =  pSourceDev->bShowActBorder;
  pDestDev->bEditable =  pSourceDev->bEditable;
  pDestDev->bShowLogo =  pSourceDev->bShowLogo;
  pDestDev->bAutoStop =  pSourceDev->bAutoStop;
  pDestDev->bShowController =  pSourceDev->bShowController;
  CopyRect(&(pDestDev->rcBorder),&pSourceDev->rcBorder);
  CopyRect(&(pDestDev->rcActBorder),&pSourceDev->rcActBorder);
  
  if (pDestDev->mfMovieName[0] != '\0')
  {
    if (OpenMovieFile (pDestDev->mfMovieName, &pDestDev->mfMovie, OF_READ) != noErr)
    {
      return A_ERROR;
    }

    GetMovieFromFile(pDestDev->mfMovie,oiDestination); //Movie File is closed in this function
  }

  AObjUnlockData(oiSource, MovieDev) ;
  AObjUnlockData(oiDestination, MovieDev) ;
  
  pDestRun = (MOVIERUNPTR) AObjLockData(oiDestination, MovieRun) ;
  pSourceRun = (MOVIERUNPTR) AObjLockData(oiSource, MovieRun) ;

  if (oiSource < 0)       //theSystem->message4 < 0)            //Temporary object
  {
    pDestRun->Picture = pSourceRun->Picture;
    pDestRun->bIsMovieDone = pSourceRun->bIsMovieDone;
    pDestRun->bIsMoviePlaying = pSourceRun->bIsMoviePlaying;
    pDestRun->LastClickTime = pSourceRun->LastClickTime;
    pDestRun->lfxRate = pSourceRun->lfxRate;
    pDestRun->sfxVolume = pSourceRun->sfxVolume;
    pDestRun->LoopValue = pSourceRun->LoopValue;
  }
  else
  {
    pDestRun->LastClickTime = pSourceRun->LastClickTime;
  }

  AObjUnlockData(oiSource, MovieRun) ;
  AObjUnlockData(oiDestination, MovieRun) ;

  pDestSet = (LPMOVIESETREC) AObjLockData(oiDestination, MovieSet) ;
  pSourceSet = (LPMOVIESETREC) AObjLockData(oiSource, MovieSet) ;

  CopyMemory(pDestSet,pSourceSet,sizeof(MOVIESETREC));

  AObjUnlockData(oiSource, MovieSet);
  AObjUnlockData(oiDestination, MovieSet) ;

  pDestFrame = (LPMOVIEFRAMEREC) AObjLockData(oiDestination, MovieFrame) ;
  pSourceFrame = (LPMOVIEFRAMEREC) AObjLockData(oiSource, MovieFrame) ;

  CopyMemory(pDestFrame,pSourceFrame,sizeof(MOVIEFRAMEREC));

  AObjUnlockData(oiSource, MovieFrame) ;
  AObjUnlockData(oiDestination, MovieFrame) ;

  how = AEVT_ATMARK | AEVT_KILLDUPLICATES;
  AEvtPostStandard(oiDestination, AEVENT_OBJECTCHANGED, EvtLoadMovie, AEvtGetObjectChangedPriority(how) );

  AEvtPostSignalAtMark(oiDestination, sChanged);

  return A_OK ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  ExportObject
//  Description:  Exports the Loan object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ExportObject(AOBJECTID oiMovie)
{
  MOVIEDEVPTR lpMovieDevRec;

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

  if (lpMovieDevRec == NULL)
    return A_ERROR;

  AUpsfExportField("mfMovieName", 0, AUPSF_NATV_STRING, AUPSF_UNIV_STRING, &lpMovieDevRec->mfMovieName, 255) ;
  AUpsfExportField("DisplayMode", 0, AUPSF_NATV_WORD, AUPSF_UNIV_INTEGER, &lpMovieDevRec->DisplayMode, sizeof(WORD)) ;
  AUpsfExportField("bShowBorder", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMovieDevRec->bShowBorder, sizeof(BOOL)) ;
  AUpsfExportField("bShowBadge", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMovieDevRec->bShowBadge, sizeof(BOOL)) ;
  AUpsfExportField("bShowActBorder", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMovieDevRec->bShowActBorder, sizeof(BOOL)) ;
  AUpsfExportField("bEditable", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMovieDevRec->bEditable, sizeof(BOOL)) ;
  AUpsfExportField("bShowLogo", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMovieDevRec->bShowLogo, sizeof(BOOL)) ;
  AUpsfExportField("bAutoStop", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMovieDevRec->bAutoStop, sizeof(BOOL)) ;

  AObjUnlockData(oiMovie,MovieDev);
  return A_OK ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  ImportObject
//  Description:  Imports the Movie object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ImportObject(AOBJECTID oiMovie)
{
  MOVIEDEVPTR lpMovieDevRec;

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie,MovieDev);

  if (lpMovieDevRec == NULL)
    return A_ERROR;

  AUpsfImportField("mfMovieName", 0, AUPSF_NATV_STRING, &lpMovieDevRec->mfMovieName, 255) ;
  AUpsfImportField("DisplayMode", 0, AUPSF_NATV_WORD, &lpMovieDevRec->DisplayMode, sizeof(WORD)) ;
  AUpsfImportField("bShowBorder", 0, AUPSF_NATV_BOOL, &lpMovieDevRec->bShowBorder, sizeof(BOOL)) ;
  AUpsfImportField("bShowBadge", 0, AUPSF_NATV_BOOL, &lpMovieDevRec->bShowBadge, sizeof(BOOL)) ;
  AUpsfImportField("bShowActBorder", 0, AUPSF_NATV_BOOL, &lpMovieDevRec->bShowActBorder, sizeof(BOOL)) ;
  AUpsfImportField("bEditable", 0, AUPSF_NATV_BOOL, &lpMovieDevRec->bEditable, sizeof(BOOL)) ;
  AUpsfImportField("bShowLogo", 0, AUPSF_NATV_BOOL, &lpMovieDevRec->bShowLogo, sizeof(BOOL)) ;
  AUpsfImportField("bAutoStop", 0, AUPSF_NATV_BOOL, &lpMovieDevRec->bAutoStop, sizeof(BOOL)) ;

  AObjUnlockData(oiMovie,MovieDev);
  return A_OK ;
}

LONG CheckVersion(AOBJECTID)
{
  return A_OK ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  ObjectAwakened
//  Description:  Load the movie into the object if ti was set
////////////////////////////////////////////////////////////////////////
LONG ObjectAwakened(AOBJECTID oiMovie)
{
  MovieFile mfMovie;
  char      name[32];   
  char      Path[255];
  char      errMsg[255]; 
  long rVal = A_CONTINUE;
  OSErr err;
  MOVIEDEVPTR lpMovieDevRec;
  MOVIERUNPTR lpMovieRunRec;

  if (!QTInstall)
    return 0;

  err = EnterMovies();
  if (err != noErr)
  {
    MessageBox (NULL, "EnterMovies failure", NULL, MB_OK);
    return 0;
  }

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie, MovieDev);
  CopyMemory(Path,lpMovieDevRec->mfMovieName,sizeof(Path));
  AObjUnlockData(oiMovie, MovieDev);

  if (Path[0] == '\0')
    return rVal;

  if (OpenMovieFile (Path, &mfMovie, OF_READ) == noErr)
  {
    GetMovieFromFile(mfMovie,oiMovie);
  }
  else {
    AObjGetName(oiMovie,(LPSTR)name,32);
    strcat(Path," not found");
    wsprintf(errMsg,"%s : %s",name,Path);
    MessageBox(NULL,errMsg,"Warning",MB_ICONEXCLAMATION | MB_OK);
    lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie, MovieDev);
    lpMovieDevRec->mfMovieName[0] = '\0';
    AObjUnlockData(oiMovie, MovieDev);

    lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
    lpMovieRunRec->mMovie = NULL;
    AObjUnlockData(oiMovie,MovieRun);
  }
  return rVal;
}

////////////////////////////////////////////////////////////////////////
//  Function:  ObjectAsleep
//  Description:  
////////////////////////////////////////////////////////////////////////
LONG ObjectAsleep(AOBJECTID oiMovie)
{
  long rVal = A_CONTINUE;
  MOVIEDEVPTR lpMovieDevRec;

  lpMovieDevRec = (MOVIEDEVPTR) AObjLockData(oiMovie, MovieDev);
    lpMovieDevRec->mController = NULL;
  AObjUnlockData(oiMovie, MovieDev);

  ExitMovies();
  return rVal;
}
