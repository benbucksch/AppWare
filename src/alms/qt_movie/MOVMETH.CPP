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

#define STRICT
#define ASTRICT

#include <string.h>
#include <math.h>
#include <windows.h>

#include <a_alm.h>
#include <o_wnd.h>
#include <op_wnd.h>

#include "qtw.h"
#include "moviedat.h"
#include "movieexp.h"

////////////////////////////////////////////////////////////////////////////
// Function:
//  mdrtProc
//
// Description:
//  This is the MDRT routine for the Movie Object
//  This is the common entry point for all the events from the shell
///////////////////////////////////////////////////////////////////////////
extern "C" void CALLBACK /*!!PORT!! was _far_ _pascal_*/  mdrtProc(pAEvtInfo theEvent, pAObjMessage)
{
  AOBJECTID  theObject;
  AOBJECTID oiMovie;
  MOVIERUNPTR lpMovieRunRec;
  long how;
  BOOL bIsMovieDone;
  long i;
  static BOOL evt = TRUE;

  theObject = theEvent->targetObject;

  // for startup & shutdown events the target object = 0
  if (AObjCheckType(theObject,(ATYPEID)OTYPE_MOVIE) || (theObject == 0))
  {
    switch ((LONG)theEvent->what)
    {
      case AEVENT_OBJECTCHANGED:
        AEvtPostSignalAtTail(theObject, sChanged);
        return;

     case AEVENT_STARTUP:
       break;

     case AEVENT_SHUTDOWN:
       i = AOBJ_GETFIRSTKEY;
       //Close all movie files that were open at runtime
       while ((BOOL)(oiMovie = AObjGetNextObject((ATYPEID)OTYPE_MOVIE, (LONG *)&i)))
       {
         lpMovieRunRec = (MOVIERUNPTR) AObjLockData(oiMovie,MovieRun);
         lpMovieRunRec->LastClickTime = 0;
         AObjUnlockData(oiMovie,MovieRun);
       }
       break;

     case AEVT_STARTMOVIE:
       break;

     case AEVT_STOPMOVIE:
       break;

     case AEVT_PLAYMOVIE:
       lpMovieRunRec = (MOVIERUNPTR) AObjLockData(theObject,MovieRun);
         bIsMovieDone = lpMovieRunRec->bIsMovieDone;
       AObjUnlockData(theObject,MovieRun);
       if (!bIsMovieDone)
       {
         if (evt)
         {
           evt = FALSE;
           how = AEVT_ATTAIL | AEVT_KILLALLQUEUES | AEVT_SELF;
         }
         else
         {
           evt = TRUE;
           how = AEVT_ATMARK | AEVT_KILLALLQUEUES | AEVT_SELF;
         }
         AEvtPostStandard(theObject, AEVT_PLAYMOVIE, 0, how);
       }

       break;

     default:
       break;
    }    
  }
//  I_UNUSED(system);

}

