////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  	Loop			
//
// Filename:	"loop_fm.cpp"
//
// Author:	Kevin Smart
//   Start:		August  7, 1992
//   Finish:	August  7, 1992
//
// Description:
//	Loop Object Functions
//	  1.  StartLoop
//	  2.  AbortLoop
//
//	Loop Object Methods
//	  1.  LoopMethod
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<o_number.h>
#include	"loop.h"

////////////////////////////Global Container////////////////////////////////

extern GLOBAL	G;

////////////////////////////////////////////////////////////////////////////
//  StartLoop
//
//      inputs:
//        1.  Loop		(loop object)
//			  2.  Length	(optional number object)
//			  3.  Start		(optional number object)
//
//      outputs:
//        none
//
//      flows:
//        1.  After Looping
////////////////////////////////////////////////////////////////////////////
#pragma argsused        // disable warning:  "Parameter '' never used"
void ALMCALLBACK	StartLoop	(pAEvtInfo theEvent,
																	 pAObjMessage      theSystem)

{
  OBJECTID	loopObj  = AFuncGetTypedParameter(1L, OTYPE_LOOP);
  OBJECTID	lenObj	 = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID	startObj = AFuncGetTypedParameter(3L, OTYPE_NUMBER);

  if(loopObj) {
		LOOP	loop(loopObj, TRUE);

		if(theEvent->targetObject==OTYPE_LOOP || !loop.IsMultiTask()) {
			// continue the flow
			AFuncReportResult(theSystem, 1L, NULL);
			}
		else {
			// stop the flow until the loop is finished
			AFuncReportResult(theSystem, 0L, NULL);
      }

    if(theEvent->targetObject != OTYPE_LOOP) {
	    // update the start and length fields as necessary
    	if(lenObj) {
				loop.SetLength(ONmbrGetInteger(lenObj));
      	}

    	if(startObj) {
				loop.SetStart(ONmbrGetInteger(startObj));
      	}

			// store the event to continue the flow when finishing the loop (multi-task only)
			if(loop.IsMultiTask()) {
				AEvtInfo	event = *theEvent;
				event.targetObject = OTYPE_LOOP;
				loop.SetEvent(&event);
        }

			// send the event to do the looping
			loop.SetValue((loop.GetLength()>0) ? (loop.GetStart()-1L) : (loop.GetStart()+1L));
    	AEvtPostStandard(loopObj, doLoop, 0L, AEVT_ATMARK);
			} // end...if(theEvent->targetObject != OTYPE_LOOP)
		else {
			AEvtInfo	event = loop.GetEvent();
			event.targetObject = OTYPE_LOOP;
			event.what = badEvent;
			loop.SetEvent(&event);
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
#pragma argsused        // disable warning:  "Parameter '' never used"
void ALMCALLBACK	AbortLoop	(pAEvtInfo theEvent,
																	 pAObjMessage      theSystem)

{
  OBJECTID	loopObj  = AFuncGetTypedParameter(1L, OTYPE_LOOP);

  if(loopObj) {
    LOOP	loop(loopObj, TRUE);

    // send the event to stop the looping
		AEvtPostStandard(loopObj, doLoop, 1L, AEVT_KILLDUPLICATES | AEVT_KILLSELF);

		if(loop.IsMultiTask()) {
			// send the "After Looping" flow
			AEvtInfo	event = loop.GetEvent();
			if(event.what!=badEvent) {
				event.when = AAppGetTime();
				event.how	 = AEVT_ATTAIL;
//SFM				loop.SetEvent(&event);
				AEvtPost(&event);
				} // end...if(event.what!=badEvent)
      } // end...if(loop.IsMultiTask())
    } // end...if(loopObj)

  AFuncReportResult(theSystem, 1L, NULL);
}

////////////////////////////////////////////////////////////////////////////
// LoopMethod--function that starts the loop and loops Length times
////////////////////////////////////////////////////////////////////////////
#pragma argsused        // disable warning:  "Parameter '' never used"
void ALMCALLBACK	LoopMethod	(pAEvtInfo theEvent,
																		 pAObjMessage      theSystem)

{
  if(theEvent->details) return;

  OBJECTID	loopObj = theEvent->targetObject;
  if(AObjGetType(loopObj)!=OTYPE_LOOP) return;

  LOOP	loop(loopObj, TRUE);
  loop.NextValue();

  if(loop.doNext()) {
    if(loop.GetConnect()) {
      OBJECTID tmpObj = AObjCreate(OTYPE_NUMBER);

			if (tmpObj)
				{
				ONmbrSetInteger(tmpObj, loop.GetValue(), FALSE);
				AObjAssign(tmpObj, loop.GetConnect());
				}
			AObjDestroy(tmpObj);
			}

		if(loop.IsMultiTask()) {
			AEvtPostStandard(loopObj, AEVENT_SIGNAL, EACH_ITERATION, AEVT_ATMARK);
			AEvtPostStandard(loopObj, doLoop, 0L, AEVT_ATTAIL);
			}
    else {
			AEvtPostStandard(loopObj, AEVENT_SIGNAL, EACH_ITERATION, AEVT_ATMARK);
			AEvtPostStandard(loopObj, doLoop, 0L, AEVT_ATMARK);
			}
		}
	else {
		// return back to the loop function and follow the "After Looping" flow
		if(loop.IsMultiTask()) {
			// send the "After Looping" flow
			AEvtInfo	event = loop.GetEvent();
			if(event.what!=badEvent) {
				event.when = AAppGetTime();
				event.how	 = AEVT_ATTAIL;
//SFM				loop.SetEvent(&event);
				AEvtPost(&event);
				} // end...if(event.what==AEVENT_CALLFUNCTION)
			} // end...if(loop.IsMultiTask())
		}

  AFuncReportResult(theSystem, 0L, NULL);
}

////////////////////////////////////////////////////////////////////////////
