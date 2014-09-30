////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "loopmain.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_number.h>

#include "loop.h"

////////////////////////////Global Container////////////////////////////////

HINSTANCE     g_hinst;

#ifdef __BORLANDC__
extern int    _WinAllocFlag;
#endif

////////////////////////////////////////////////////////////////////////////

int CALLBACK LibMain
  (
  HINSTANCE   hinst,
  WORD        wDataSeg,
  WORD        wHeapSize,
  LPSTR       lpszCmdLine
  )

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  g_hinst = hinst;

  #ifdef __BORLANDC__
  _WinAllocFlag = GMEM_SHARE;
  #endif

  if(wHeapSize>0)
    UnlockData(0);

  return 1;
  }

////////////////////////////////////////////////////////////////////////////

extern "C" long ALMCALLBACK LoopAdmn
  (
  OBJECTID      oiLoop,
  pAObjMessage  pMsg
  )

  {
  LONG      lRet;
  short*    pVer;
  Loop*     pLoop;

  switch(pMsg->message1)
    {
    case AOBJ_AWAKENED:
    case AOBJ_CREATED:
      pLoop = new Loop(oiLoop);
      if(pLoop && pLoop->Valid())
        {
        AObjSetRuntimeInfo(oiLoop, (long)pLoop);
        lRet = (pMsg->message1 == AOBJ_AWAKENED) ? A_CONTINUE : A_OK;
        }
      else
        {
        AObjSetRuntimeInfo(oiLoop, (long)NULL);
        lRet = A_ERROR;
        }
      break;

    case AOBJ_ASLEEP:
    case AOBJ_DESTROYED:
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      delete pLoop;
      AObjSetRuntimeInfo(oiLoop, (long)NULL);
      lRet = A_CONTINUE;
      break;

    case AOBJ_ASSIGNOBJECT :
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      lRet = pLoop->Assign((Loop*)AObjGetRuntimeInfo((OBJECTID)(pMsg->message3)));
      break;

    case AOBJ_BUILDSIGNALS:
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      lRet = pLoop->BuildSignals((MEMBLOCKID)pMsg->message2);
      break;

    case AOBJ_CHECKOBJECT:
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      lRet = pLoop->Check();
      if(lRet == A_WARNING)
        {
        AObjReportError(oiLoop, AOBJ_CHECKOBJECT, A_WARNING,
          "The sum of the loop starting value and length is out of range.",
          0L);
        }
      break;

    case AOBJ_EDITOBJECT:
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      lRet = pLoop->Edit();
      break;

    case AOBJ_EXPORTOBJECT:
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      lRet = pLoop->Export();
      break;

    case AOBJ_IMPORTOBJECT:
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      lRet = pLoop->Import();
      break;

    case AOBJ_READ:
      pVer = (short*)AObjLockData(oiLoop, DATA_LOOPDATA);
      if(pVer && (*pVer > VERSION_LOOPOBJECT))
        {
        lRet = A_ERROR;
        AObjReportError(oiLoop, AOBJ_READ, A_ERROR, "Loop Object\talm_loop.dll", VERSION_LOOPOBJECT);
        }
      else if(pVer && (*pVer < VERSION_LOOPOBJECT))
        {
        *pVer = VERSION_LOOPOBJECT;

        // set the project's dirty flag
        lRet = A_WARNING;
        }
      else
        lRet = A_OK;

      if(pVer)
        AObjUnlockData(oiLoop, DATA_LOOPDATA);
      break;

    case AOBJ_VALIDATEOBJECTIDS:
      pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);
      pLoop->ValidateIDs(pMsg);
      lRet = A_OK;
      break;
      
    default:
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//  StartLoop
//
//      inputs:
//        1.  Loop    (loop object)
//        2.  Length  (optional number object)
//        3.  Start   (optional number object)
//
//      outputs:
//        none
//
//      flows:
//        1.  After Looping
//
//      comments:
//        This function is called in two different ways.  First, it is called
//        by the Bus like every other function.  Second, it is called to 
//        notify the user that the multi-tasking loop is done.  In the second
//        case, Start Loop does nothing except continue to the next function
//        in the chain.
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK StartLoop
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  OBJECTID  oiLoop  = AFuncGetTypedParameter(1L, OTYPE_LOOP);

  if(oiLoop)
    {
    Loop* pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);

    if(!pLoop)
      AFuncReportResult(pMsg, 1L, "Loop data has been corrupted.");
    else if(pLoop->IsLooping())
      {
      AFuncReportResult(pMsg, 1L,
        "The loop is currently running.  The loop must be aborted before it can be restarted.");
      }
    else if(pEvent->targetObject != (OBJECTID)OTYPE_LOOP)
      {
      OBJECTID  oiLength  = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
      OBJECTID  oiStart   = AFuncGetTypedParameter(3L, OTYPE_NUMBER);

      // update the start and length fields as necessary
      if(oiLength)
        pLoop->SetLength(ONmbrGetInteger(oiLength));

      if(oiStart)
        pLoop->SetStartValue(ONmbrGetInteger(oiStart));

      if(pLoop->Check() == A_WARNING)
        {
        AFuncReportResult(pMsg, 1L,
          "The sum of the loop starting value and length is out of range.");
        }
      else
        {
        if(pLoop->MultiTask())
          {
          // store the event to continue the flow when finishing the loop
          AEvtInfo  event = *pEvent;

          event.targetObject = (OBJECTID)OTYPE_LOOP;
          event.when         = 0;
          event.how          = AEVT_ATTAIL;
          pLoop->SetEvent(&event);

          // break the flow until the loop is finished
          AFuncReportResult(pMsg, 0L, NULL);
          }
        else
          {
          // follow the "Done" flow
          AFuncReportResult(pMsg, 1L, NULL);
          }

        // send the event to do the looping
        pLoop->StartLooping();
        pLoop->SetCurrentIndex(pLoop->StartValue());
        AEvtPostStandard(oiLoop, OLOOP_EVT_EACHITERATION, 0L, AEVT_ATMARK);
        }
      } // end...if(pEvent->targetObject != OTYPE_LOOP)
    else
      {
      AEvtInfo  event = pLoop->Event();

      // make sure the stored event is BAD ... can't use again
      event.targetObject  = (OBJECTID)OTYPE_LOOP;
      event.what          = OLOOP_EVT_BAD;
      pLoop->SetEvent(&event);

      // follow the "Done" flow
      AFuncReportResult(pMsg, 1L, NULL);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//  AbortLoop
//
//      inputs:
//        1.  Loop
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after aborting
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK AbortLoop
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  OBJECTID  oiLoop = AFuncGetTypedParameter(1L, OTYPE_LOOP);

  if(oiLoop)
    {
    Loop* pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);

    // send the event to stop the looping
    AEvtPostStandard(oiLoop, OLOOP_EVT_EACHITERATION, 1L, AEVT_KILLDUPLICATES | AEVT_KILLSELF);

    if(pLoop)
      {
      pLoop->EndLooping();

      if(pLoop->MultiTask())
        {
        // call the Start Loop function to quit looping and follow the "Done" flow
        AEvtInfo  event = pLoop->Event();

        if((EVENTID)event.what != OLOOP_EVT_BAD)
          AEvtPost(&event);
        } // end...if(pLoop->MultiTask())
      } // end...if(pLoop)
    } // end...if(oiLoop)

  AFuncReportResult(pMsg, 1L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
// LoopMethod--function that starts the loop and loops Length times
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK LoopMethod
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  OBJECTID  oiLoop = pEvent->targetObject;
  Loop*     pLoop = (Loop*)AObjGetRuntimeInfo(oiLoop);

  if(pLoop)
    {
    // set the index object
    if(pLoop->ConnectionObject())
      ONmbrSetInteger(pLoop->ConnectionObject(), pLoop->CurrentIndex(), FALSE);

    // send the "Each iteration" signal
    if(pLoop->Length()!=0)
      AEvtPostStandard(oiLoop, AEVENT_SIGNAL, SIGNAL_EACHITERATION, AEVT_ATMARK);

    // in(de)crement the current loop index
    pLoop->NextValue();

    // continue looping or quit
    if(pLoop->DoNext() && (pLoop->Length()!=0))
      AEvtPostStandard(oiLoop, OLOOP_EVT_EACHITERATION, 0L, pLoop->MultiTask() ? AEVT_ATTAIL : AEVT_ATMARK);
    else
      {
      pLoop->EndLooping();
      if(pLoop->MultiTask())
        {
        // call the Start Loop function to quit looping and follow the "Done" flow
        AEvtInfo  event = pLoop->Event();

        if((EVENTID)event.what != OLOOP_EVT_BAD)
          AEvtPost(&event);
        } // end...if(pLoop->MultiTask())
      }
    }

  AFuncReportResult(pMsg, 0L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
