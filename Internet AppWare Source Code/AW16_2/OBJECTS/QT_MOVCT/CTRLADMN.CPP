
/////////////////////////////////////////////////////////////////////////////
// File Name:  MovieCPP
// 		Copyright 1993 Novell Inc.
// Description.:
//	The Administration Routine for the AppWare Loan object.
/////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <mem.h>
#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>

#include "movctrl.h"
#include "moviedat.h"

extern QTInstall;

LONG CreateObject(OBJECTID theObject) ;
LONG DestroyObject(OBJECTID theObject);
LONG CheckObject( OBJECTID theObject, pAObjMessage theSystem );
LONG ValidateObjectIDs( OBJECTID theObject, pAObjMessage theSystem );
LONG BuildSignals(pAObjMessage theSystem) ;
LONG AssignObject(OBJECTID oiDestination, pAObjMessage theSystem);
LONG ExportObject(OBJECTID theObject);
LONG ImportObject(OBJECTID theObject);
LONG CheckVersion(OBJECTID theObject);

extern LONG EditObject(OBJECTID theObject);

extern OpWndItemD* ALMAPI  MCtrlVICreate(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status,long special);

char far* signals[] = {
  {"Volume Clicked"},
  {"Action Clicked"},
  {"Scroll Area Clicked"},
  {"Stepper Clicked"}
  };

/////////////////////////////////////////////////////////////////////
// Function:  AdministrationRoutine
// Description:  Creates the and initializes the data structure for
//   a Movie Object when the AOBJ_CREATED message is received
/////////////////////////////////////////////////////////////////////
LONG FAR PASCAL _export CtrlAdministration(OBJECTID theObject, pAObjMessage theSystem)
{
  LPMCTRLDEV  lpMCtrlDev;
  LPMCTRLRUN  lpMCtrlRun;

  switch (theSystem->message1)
  {
    case AOBJ_CREATED:
//return CreateObject(theObject) ;

      AObjSetDataCount(theObject,MCTRLDATABLOCKS);
      AObjResizeData(theObject,MCtrlDev,sizeof(MCTRLDEV));
      lpMCtrlDev = (LPMCTRLDEV)AObjLockData(theObject,MCtrlDev);
      lpMCtrlDev->oiMovie = 0;
      lpMCtrlDev->bSpeaker = TRUE;
      lpMCtrlDev->bStepper = TRUE;
      lpMCtrlDev->bSelectable = TRUE;
      lpMCtrlDev->bColor = TRUE;
      lpMCtrlDev->canActivate = TRUE;
//Data used at design time
      lpMCtrlDev->oiMovieShared = 0;
      lpMCtrlDev->bMovieAlias = FALSE;
      AObjUnlockData(theObject,MCtrlDev);

      AObjResizeData(theObject,MCtrlRun,sizeof(MCTRLRUN));
      lpMCtrlRun = (LPMCTRLRUN) AObjLockData(theObject,MCtrlRun);
      setmem(lpMCtrlRun,sizeof(LPMCTRLRUN),0);
      AObjUnlockData(theObject,MCtrlRun);
      return A_OK;

    case AOBJ_DESTROYED:
      return DestroyObject(theObject);

    case AOBJ_EDITOBJECT:
      return EditObject(theObject) ;

    case AOBJ_CHECKOBJECT:
      return CheckObject (theObject, theSystem) ;

    case AOBJ_VALIDATEOBJECTIDS :
      return ValidateObjectIDs(theObject, theSystem) ;

    case AOBJ_BUILDSIGNALS:
      return BuildSignals(theSystem) ;

    case AOBJ_ASSIGNOBJECT:
      return AssignObject(theObject, theSystem) ;

    case AOBJ_IMPORTOBJECT :
      return ImportObject(theObject) ;

    case AOBJ_EXPORTOBJECT :
      return ExportObject(theObject) ;

    case AOBJ_READ :
      return CheckVersion(theObject) ;
  }
  return A_NOTHANDLED ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  CreateObject
//  Description:  Creates a Movie object with default values
////////////////////////////////////////////////////////////////////////
LONG CreateObject(OBJECTID theObject)
  {
    I_UNUSED(theObject);

    return A_OK ;
  }

LONG DestroyObject(OBJECTID theObject)
{
  LPMCTRLDEV lpMCtrlDev;
  MOVIEDEVPTR lpMovieDevRec;
  OBJECTID oiMovie;

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(theObject,MCtrlDev);
  oiMovie = lpMCtrlDev->oiMovieShared;
  AObjUnlockData(theObject,MCtrlDev);

  if (oiMovie)
  {
    lpMovieDevRec = (MOVIEDEVPTR)AObjLockData(oiMovie,MovieDev);
    lpMovieDevRec->oiController = 0;
    AObjUnlockData(oiMovie,MovieDev);
  }
  return A_OK;
}
/////////////////////////////////////////////////////////////////////
//  CheckObject
//  Verifies that the Movie Controller object is associated with a movie.
/////////////////////////////////////////////////////////////////////
LONG CheckObject( OBJECTID theObject, pAObjMessage theSystem )
{
  I_UNUSED(theObject);
  I_UNUSED(theSystem);

  long rVal = A_OK;

  return rVal ;
}

/////////////////////////////////////////////////////////////////////
//  ValidateObjectIDs
//  Updates the stored Object ID oiMovie.
/////////////////////////////////////////////////////////////////////
LONG ValidateObjectIDs( OBJECTID theObject, pAObjMessage theSystem )
{
  LPMCTRLDEV lpMCtrlDev;

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(theObject,MCtrlDev);
  AObjValidateID(&lpMCtrlDev->oiMovie,theSystem);
  AObjUnlockData(theObject,MCtrlDev);

  return A_OK ;
}

/////////////////////////////////////////////////////////////////////
//  BuildSignals
//  Builds an array of Movie Controller object signal names.
/////////////////////////////////////////////////////////////////////
LONG BuildSignals(pAObjMessage theSystem)
{
  MEMBLOCKID signalBlock ;
  AObjSignalRecord far *lpSignal ;
  int i, nSignals ;

  // a memory block handle is passed to you
  signalBlock = (MEMBLOCKID)theSystem->message2 ;
  // determine the number of signals you generate
  nSignals = sizeof(signals)/sizeof(signals[0]) ;
  // size the memory block based on the number of signals you generate
  AMemSetSize(signalBlock, nSignals*sizeof(AObjSignalRecord)) ;
  // get a pointer to the memory block
  lpSignal = (AObjSignalRecord far*)AMemLock(signalBlock) ;
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
//    Copies Data form the source Movie Controller object to the destination one
////////////////////////////////////////////////////////////////////////
LONG AssignObject(OBJECTID oiDestination, pAObjMessage theSystem)
{
  OBJECTID oiSource ;
  LPMCTRLDEV lpSource, lpDestination;

  oiSource = (OBJECTID) theSystem->message3 ; // source object ID
  lpDestination = (LPMCTRLDEV) AObjLockData(oiDestination, 0) ;
  lpSource = (LPMCTRLDEV) AObjLockData(oiSource, 0) ;
  _fmemcpy(lpDestination,lpSource,sizeof(MCTRLDEV));
  AObjUnlockData(oiSource, 0) ;
  AObjUnlockData(oiDestination, 0) ;

  return A_OK ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  ExportObject
//  Description:  Exports the Movie Controller object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ExportObject(OBJECTID theObject)
{
  LPMCTRLDEV lpMCtrlDev;

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(theObject,MCtrlDev);

  if (lpMCtrlDev == NULL)
    return A_ERROR;

  AUpsfExportField("oiMovie", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpMCtrlDev->oiMovie,sizeof(OBJECTID)) ;
  AUpsfExportField("bSpeaker", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMCtrlDev->bSpeaker, sizeof(BOOL)) ;
  AUpsfExportField("bStepper", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMCtrlDev->bStepper, sizeof(BOOL)) ;
  AUpsfExportField("bSelectable", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMCtrlDev->bSelectable, sizeof(BOOL)) ;
  AUpsfExportField("bColor", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMCtrlDev->bColor, sizeof(BOOL)) ;
  AUpsfExportField("canActivate", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpMCtrlDev->canActivate, sizeof(BOOL)) ;

  AObjUnlockData(theObject,MCtrlDev);
  return A_OK ;
}

////////////////////////////////////////////////////////////////////////
//  Function:  ImportObject
//  Description:  Imports the Movie Controller object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ImportObject(OBJECTID theObject)
{
  LPMCTRLDEV lpMCtrlDev;

  lpMCtrlDev = (LPMCTRLDEV) AObjLockData(theObject,MCtrlDev);

  if (lpMCtrlDev == NULL)
    return A_ERROR;

  AUpsfImportField("oiMovie", 0, AUPSF_NATV_OBJECTREF, &lpMCtrlDev->oiMovie,sizeof(OBJECTID));
  AUpsfImportField("bSpeaker", 0, AUPSF_NATV_BOOL, &lpMCtrlDev->bSpeaker, sizeof(BOOL));
  AUpsfImportField("bStepper", 0, AUPSF_NATV_BOOL, &lpMCtrlDev->bStepper, sizeof(BOOL));
  AUpsfImportField("bSelectable", 0, AUPSF_NATV_BOOL, &lpMCtrlDev->bSelectable, sizeof(BOOL));
  AUpsfImportField("bColor", 0, AUPSF_NATV_BOOL, &lpMCtrlDev->bColor, sizeof(BOOL));
  AUpsfImportField("canActivate", 0, AUPSF_NATV_BOOL, &lpMCtrlDev->canActivate, sizeof(BOOL));

  AObjUnlockData(theObject,MCtrlDev);

  return A_OK ;
}

LONG CheckVersion(OBJECTID theObject)
{
  I_UNUSED(theObject);

  return A_OK ;
}

