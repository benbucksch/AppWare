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
// FILE:          "sub_fn.cpp"
//
// AUTHOR:
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

#include "sub.h"

////////////////////////////////////////////////////////////////////////////
//  CallSubroutine
//
//      inputs:
//        1.  Subroutine  (subroutine object)
//        2.  Object 1  (optional object--any type)
//        3.  Object 2  (optional object--any type)
//        4.  Object 3  (optional object--any type)
//
//      outputs:
//        none
//
//      flows:
//        1.  After calling
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK CallSubroutine
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  OBJECTID    oiSub = AFuncGetTypedParameter(1L, OTYPE_SUB);
  OBJECTID    oi1   = AFuncGetParameter (2L);
  OBJECTID    oi2   = AFuncGetParameter (3L);
  OBJECTID    oi3   = AFuncGetParameter (4L);

  if(oiSub)
    {
    Subroutine* pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);
    AEvtInfo    event;

    if(pSub)
      {
      if((pEvent->targetObject == (OBJECTID)OTYPE_SUB) || pSub->AutoReturn())
        {
        // continue the flow--either returning from sub. or auto-return sub.
        AFuncReportResult(pMsg, 1L, NULL);
        }
      else
        {
        // stop the flow until a RETURN from subroutine is called
        AFuncReportResult(pMsg, 0L, NULL);
        }

      if(pEvent->targetObject != (OBJECTID)OTYPE_SUB)
        {
        pSub->Called();
        pSub->SetParameters(oi1, oi2, oi3);
   
        AEvtPostStandard(oiSub, AEVENT_SIGNAL, SIGNAL_SUBCALLED, AEVT_ATMARK);

        if(!pSub->AutoReturn())
          {
          // Save the call function event, but overwrite the targetObject
          // so we can tell how the function is being called.
          event = *pEvent;
          event.targetObject = (OBJECTID)OTYPE_SUB;
          pSub->SetEvent(&event);
          }  // end...if(!pSub->AutoReturn())
        }  // end...if(pEvent->targetObject!=OTYPE_SUB)
      else
        {
        event = pSub->Event();
        event.what = OSUB_EVT_BAD;
        pSub->SetEvent(&event);
        }
      }  // end...if(pSub)
    else
      AFuncReportResult(pMsg, 1L, "Critical memory error.");
    }  // end...if(oiSub)
  else
    AFuncReportResult(pMsg, 1L, "Invalid input to Call Subroutine function");
  }

////////////////////////////////////////////////////////////////////////////
//  FetchParameters
//
//      inputs:
//        1.  Subroutine  (subroutine object)
//
//      outputs:
//        1.  Object 1  (optional object--any type)
//        2.  Object 2  (optional object--any type)
//        3.  Object 3  (optional object--any type)
//
//      flows:
//        1.  After fetching
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK FetchParameters
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  OBJECTID  oiSub = AFuncGetTypedParameter(1L, OTYPE_SUB);

  if(oiSub)
    {
    Subroutine* pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);

    if(pSub)
      {
      OBJECTID  oi1, oi2, oi3;

      // set the output parameters to whatever was stored
      pSub->GetParameters(&oi1, &oi2, &oi3);
      AFuncSetParameter(1L, oi1);
      AFuncSetParameter(2L, oi2);
      AFuncSetParameter(3L, oi3);
      }
    }

  AFuncReportResult(pMsg, 1L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//  Return
//
//      inputs:
//        1.  Subroutine  (subroutine object)
//        2.  Object 1  (optional object--any type)
//        3.  Object 2  (optional object--any type)
//        4.  Object 3  (optional object--any type)
//
//      outputs:
//        none
//
//      flows:
//        none
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK Return
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);

  OBJECTID  oiSub = AFuncGetTypedParameter(1L, OTYPE_SUB);
  OBJECTID  oi1 = AFuncGetParameter (2L);
  OBJECTID  oi2 = AFuncGetParameter (3L);
  OBJECTID  oi3 = AFuncGetParameter (4L);

  if(oiSub)
    {
    Subroutine* pSub = (Subroutine*)AObjGetRuntimeInfo(oiSub);

    if(pSub && pSub->WasCalled())
      {
      pSub->SetParameters(oi1, oi2, oi3);
      pSub->Returned();

      if(!pSub->AutoReturn())
        {
        AEvtInfo  event = pSub->Event();

        if(event.what!= OSUB_EVT_BAD)
          AEvtPost(&event);
        }

      AFuncReportResult(pMsg, 0L, NULL);
      }  // end...if(pSub && pSub->WasCalled())
    else if(pSub && !pSub->WasCalled())
      AFuncReportResult(pMsg, 0L,
        "Subroutine ALM:\r\nReturn was called without first calling Call Subroutine.");
    else
      AFuncReportResult(pMsg, 0L, NULL);
    }
  else
    AFuncReportResult(pMsg, 0L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
