/*//////////////////////////////////////////////////////////////
//
//											Copyright 1993, Novell, Inc.
//
//		<standard disclaimers here>
//
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//	7/22/93		Scott
//		Removed function of putting Windows messages into Serius queue.
//
//////////////////////////////////////////////////////////////*/

// EVENTQUE.CPP

  #include "almsys.h"
	#include "intalm.h"

  #include "eventque.h"
  #include "system.h"
  #include "exeevntl.h"
  #include "exerespl.h"
  #include "exefunc.h"
  #include "exeobjs.h"
  #include "exeflow.h"
  #include "rectypes.h"

  #include "eventqnd.i"
  #include "eventqms.i"
  #include "context.h"

  #include "tempobj.h"

#ifdef ADBG_DEBUG
	#include "oipdebug.h"
#endif
	#include "oiptime.h"

  // internal event!
  #define startupDone    MAKEID('s','u','d','N')

	long EVENTQUEUENODE::nodesNow   = 0;
  long EVENTQUEUENODE::nodesMax   = 0;
  long EVENTQUEUENODE::nodesTotal = 0;

////////////////////////////////////////////////////////////////////////
//
// public member functions
//
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
//
//    EVENTQUEUE::EVENTQUEUE (constructor)
//
//  Makes AEVT_SELF an open queue and nullifies all queue pointers.
//
//----------------------------------------------------------------------
EVENTQUEUE::EVENTQUEUE():
    functionStack(10)
	{
	this->Open();
	head = tail = mark = 0;
  headWaiter = 0;
  currentEventNodePtr = 0;

	gotBailOut = FALSE;

	EVENTQUEUENODE::nodesNow   = 0;
	EVENTQUEUENODE::nodesMax   = 0;
	EVENTQUEUENODE::nodesTotal = 0;
  }

//----------------------------------------------------------------------
//
//    EVENTQUEUE::~EVENTQUEUE (destructor)
//
//  Empties the main and wait queues.
//
//----------------------------------------------------------------------
EVENTQUEUE::~EVENTQUEUE()
	{
	LPEVENTQUEUENODE p, q;

 // kill the current node

	delete currentEventNodePtr;

 // empty the main queue

	p = head;
	while(p)
		{
		q = p->next;
		delete p;
		p = q;
		}

 // empty the wait queue

	p = headWaiter;
	while(p)
		{
		q = p->next;
		delete p;
		p = q;
		}
	}

//----------------------------------------------------------------------
//
//    EVENTQUEUE::KillIf
//
//  Scan the specified queue, removing all nodes fingered by the 'how'
//  field of the hitMan.
//
//----------------------------------------------------------------------
BOOL EVENTQUEUE::KillIf(
	LPEVENTQUEUENODE  theHitMan,
	LPEVENTQUEUENODE   & theQueueHead)
	{
	LPEVENTQUEUENODE  p;
	LPEVENTQUEUENODE  prevp = NULL;
	BOOL              queueDidChange = FALSE;

	LONG myHow = theHitMan->event.how;

	if (!(myHow & validKillBitsMask))
		return FALSE;

 // cruise the specified queue, looking for duplicates

	for (p = theQueueHead ; p ; )
		{
		BOOL              skipIt;
		LPEVENTQUEUENODE  dyingNode;

		if (myHow & AEVT_KILLWHAT && p->event.what != theHitMan->event.what)
			skipIt = TRUE;
		else if (myHow & AEVT_KILLTARGET && p->event.targetObject != theHitMan->event.targetObject)
			skipIt = TRUE;
		else if (myHow & AEVT_KILLDETAILS && p->event.details != theHitMan->event.details)
			skipIt = TRUE;
		else if (myHow & AEVT_KILLSOURCE && p->event.sourceObject != theHitMan->event.sourceObject)
			skipIt = TRUE;
		else if (myHow & AEVT_KILLMODIFIERS && p->event.modifiers != theHitMan->event.modifiers)
			skipIt = TRUE;
		else if (myHow & AEVT_KILLWHEN && p->event.when != theHitMan->event.when)
			skipIt = TRUE;
		else if (myHow & AEVT_KILLVARIATION && p->event.variation != theHitMan->event.variation)
			skipIt = TRUE;
		else
			skipIt = FALSE;

		if (skipIt)
			{
		 // *** this node does not match, try the next one ***
			prevp = p;
			p = p->next;
			continue;
			}

	 // *** this node matches -- remove it from the list ***

	 // save address of the dying node
		dyingNode = p;

	ADBG_EVAL(PrintKilledEvent(&dyingNode->event, dyingNode->nodeID));

	 // move on to the next node
		p = p->next;

		if (!prevp)
			{  // *** it's the head node that is dying ***

			theQueueHead = p;

		 // if it's the event queue, avoid dangling pointers
			if (theQueueHead == this->head)
				{
				//if mark is pointing to the head, set it to zero.  Just as if
				//we had poppep an event off.
				if (mark == dyingNode)
					mark = 0;

				if (tail == dyingNode)
					tail = p;

				markStack.Replace(dyingNode, p);
				}

			}
		else
			{  // *** somewhere beyond the head ***

			prevp->next = p;

		 // if it's the event queue, avoid dangling pointers
			if (theQueueHead == this->head)
				{
				if (mark == dyingNode)
					mark = prevp;

				if (tail == dyingNode)
					tail = prevp;

				markStack.Replace(dyingNode, prevp);
				}
			}

		delete dyingNode;
		queueDidChange = TRUE;

		} // queue cruise

	return queueDidChange;
	}

//----------------------------------------------------------------------
//
//    EVENTQUEUE::Pop
//
//  Removes and returns the next serius event from the queue.  If
//  any events in the wait queue have become eligible, push them
//  onto the main queue before we pop an element off.
//
//----------------------------------------------------------------------
pAEvtInfo  EVENTQUEUE::Pop()
	{

 // push expired waiters onto the main queue

	while (headWaiter && (headWaiter->event.when) <= AAppGetTime())
		{
		LPEVENTQUEUENODE dyingWaiter = headWaiter;

		headWaiter = headWaiter->next;
		this->Push(&(dyingWaiter->event));
		delete dyingWaiter;
		}

 // delete the last event that was pop'd ... remember the current event

	delete this->currentEventNodePtr;
	currentEventNodePtr = head;

 // make sure we have work to do...

	if (!currentEventNodePtr)
		return 0;

 // update queue pointers

	head = head->next;
	if (!head)
		tail = 0;

	mark = 0;

 ADBG_EVAL(PrintEventInfo(&currentEventNodePtr->event, currentEventNodePtr->nodeID));
 
 // hand the AEvtInfo back
	return &(currentEventNodePtr->event);
	}

//----------------------------------------------------------------------
//
//    EVENTQUEUE::WaitQueueHead
//	Returns the first eventinfo on the wait queue.
//
//----------------------------------------------------------------------
AEvtInfo*  EVENTQUEUE::WaitQueueHead(
	void
	)
	{
	AEvtInfo*		pEvent;

	if (headWaiter != NULL)
		pEvent = &headWaiter->event;
	else
		pEvent = NULL;

	return pEvent;
	}

//----------------------------------------------------------------------
//
//    EVENTQUEUE::HandleEvent
//
// Dispatches the passed event.  A non-zero return indicates that we
// are at a Stop point in the debugger.
//
//----------------------------------------------------------------------
BOOL EVENTQUEUE::HandleEvent(
	pAEvtInfo theEvent)
	{
	switch (theEvent->what)
		{
		case AEVENT_SIGNAL:
				HandleSignal(theEvent);
				break;

		case AEVENT_CALLFUNCTION:
				CallFunction(theEvent);
				break;

		case AEVENT_PAUSEPROGRAM:
				{
				LPAPPCONTEXT context = pTheSystem->GetCurrentContext();
				if (theEvent->details)
					{
					BOOL ans;

					//make sure the flow still has a stop set since the pause event
					//may have been pushed before a previous debugging stop was hit
					//where the flow stop was cleared.
					if(((LPEXEFLOW)theEvent->details)->DoStop() || TestSingleStepFlag())
						{
            ClearSingleStepFlag();
						context->SetIsAtStop();
						ans = context->EnterDebugger(theEvent->details);
						context->ClearIsAtStop();
						}
					else
						ans = TRUE; //signal that we should continue

					return ans;
          }
        else
          {
					// -- it's a user stop
					ADbgPrint(ADBG_ERROR, 0, "");
//					ADBG_ASSERT_E(0,,"");
          }

        break;
				}

    case startupDone:
        break;

    default:
        CallMethod(theEvent, NULL);
        break;
    }

  return TRUE;
  }

//----------------------------------------------------------------------
//
//    EVENTQUEUE::Push
//
//  Places the specified serius event onto the main queue.  If the
//  event is scheduled to happen in the future, place it onto the
//  wait queue.  Returns FALSE if the event is not pushed.
//
//----------------------------------------------------------------------
BOOL EVENTQUEUE::Push(
  pAEvtInfo  theEvent,
	LONG            theFlags)
  {
  LPEVENTQUEUENODE  pNode;
  LONG              myHow = theEvent->how;

  //--------------------------------------------------------------------
  // Validate the source and target objects.
  //
  // 1. Don't validate for Serius Function calls because
  //    some functions, e.g. Subroutine stuff, use the
  //    source/target object fields of theEvent to pass
  //    other info...
  //
  // 2. Return immediately, without posting, if this event
  //    is an AEVENT_OBJECTCHANGED for a temporary object.
  //
  // 3. Return immediately, without posting, if this event
  //    is a AEVENT_SIGNAL for a temporary object.
  //--------------------------------------------------------------------

  if (theEvent->what != AEVENT_CALLFUNCTION)
    {
    // Validate target object ID
    if (theEvent->targetObject)
			{
      if (TEMPOBJECT::GetObject(theEvent->targetObject))
        {
        // Ignore posting of AEVENT_OBJECTCHANGED for temporary objects!
        if (theEvent->what == AEVENT_OBJECTCHANGED
              ||
            theEvent->what == AEVENT_SIGNAL)
          {
          return FALSE;
          }
        }
      else if (!GetSeriusObject(theEvent->targetObject))
        {
        // target is not a Serius object (temp or real)
        char  myBuf[100];
        wsprintf(myBuf, "%lx is an invalid target object ID", theEvent->targetObject);
        MessageBox(NULL,
                   (LPSTR)myBuf,
                   "Serius (internal error) -- EVENTQUEUE::Push",
                   MB_OK | MB_ICONSTOP | MB_TASKMODAL);
        return FALSE;
        }
      }

#ifdef ADBG_DEBUG
    // Validate source object ID
    if (theEvent->sourceObject
        && !GetSeriusObject(theEvent->sourceObject)
        && !TEMPOBJECT::GetObject(theEvent->sourceObject))
      {
      // target is not a Serius object (temp or real)
      char  myBuf[100];
      wsprintf(myBuf, "%lx is an invalid source object ID", theEvent->targetObject);
      MessageBox(NULL,
                 myBuf,
                 "Internal Error -- EVENTQUEUE::Push",
                 MB_OK | MB_ICONSTOP | MB_TASKMODAL);
      return FALSE;
      }
#endif
    }

 // do some validation on the how field

#ifdef ADBG_DEBUG
  if (myHow & bogusHowBits)
    {
    char  myBuf[100];
    wsprintf(myBuf, "%lx is an invalid mask for 'how'", myHow);
    MessageBox(NULL,
               (LPSTR)myBuf,
               "Internal Error -- EVENTQUEUE::Push",
               MB_OK | MB_ICONSTOP | MB_TASKMODAL);
    return FALSE;
    }
#endif


//----- AEVT_ATONCE processing -----//

  if (myHow & AEVT_ATONCE)
    {
    LPEVENTQUEUENODE   holdCurrent;
	 /////////////////////////////////////////////////////////////////////
   // Remember & reset the queue mark.  See also use of the markStack
   // in EVENTQUEUE::killIf function.
   /////////////////////////////////////////////////////////////////////
    markStack.Push(mark);
    mark = 0;

   /////////////////////////////////////////////////////////////////////
   // Remember and reset the event node we are in the midst of
   // processing.  See also, EVENTQUEUE::Pop and notice we call Pop()
   // in the AEVT_ATONCE processing loop (things that make you go hmmmmm).
   /////////////////////////////////////////////////////////////////////
    holdCurrent = currentEventNodePtr;
    currentEventNodePtr = 0;

   // Delimit the end of this AEVT_ATONCE processing
    AEvtInfo     tombstone;
    tombstone.sourceObject = 0;
    tombstone.targetObject = 0;
    tombstone.what         = AEVENT_IMMEDIATETAIL;
    tombstone.details      = 0;  // *** (LONG)holdCurrent;
    tombstone.modifiers    = 0;
    tombstone.how          = AEVT_ATMARK;
    tombstone.when         = AAppGetTime();
		this->Push(&tombstone);  // recurse

   // push the mark back, yet again
    mark = 0;

   // Process this event and all AEVT_ATMARK events issued from it
    AEvtInfo     eventNow = *theEvent;
    pAEvtInfo  pEvent = &eventNow;

    //------------------------------------------------------------------
    // DO NOT REMOVE! These two lines are essential to ensure that
    // Project|Reset of a stop of an AEVT_ATONCE signal works!  Apparently,
    // 'this' (the EVENTQUEUE) has been smashed by the time we get
    // back here in the case of a Project|Reset.  It's disgusting
    // but it works.  To eliminate this problem we need to probably
    // totally re-engineer Stop-point processing.  This would involve
    // changes in MDICLASS, EVENTQUEUE and OIPEXE.CPP.
    //------------------------------------------------------------------ 
    jmp_buf        jumper;
    CopyMemory(jumper, this->bailOut, sizeof(jumper));

    while (pEvent && pEvent->what != AEVENT_IMMEDIATETAIL)
      {
      if (!this->HandleEvent(pEvent))
        // They did a Project|Reset while at a Stop.
        // Unwind the stack back to the invocation.
        {
        if(!pTheSystem->GetCurrentContext()->IsMicroApp())
         longjmp(jumper, 1);
				}

      pEvent = this->Pop();
      }

   // Recover the queue mark
    mark = markStack.Pop();

   // Restore the event node we were in the midst of processing
    delete currentEventNodePtr;
		currentEventNodePtr = holdCurrent;

   // Bye now
    return TRUE;
    }

 // default posting protocol is AEVT_ATTAIL

  if (!(myHow & (AEVT_ATTAIL | AEVT_ATMARK | AEVT_ATONCE)))
    myHow |= AEVT_ATTAIL;

 // ignore OS or AEVT_ATTAIL events if the queue is closed, ie, it's shutdown time

  if ( !(this->IsOpen()) && (myHow & AEVT_ATTAIL) )
    return TRUE;

 // close the queue on SHUTDOWN

  if (theEvent->what == AEVENT_SHUTDOWN)
    this->Close();

 // fill in the node

  pNode = new EVENTQUEUENODE;

  pNode->event = *theEvent;
  pNode->event.how = myHow;

  pNode->flags = theFlags;


//----- scheduled event processing -----//

 // see if this event is a waiter, ie, it is scheduled to run in the future

  if (theEvent->when > AAppGetTime())
    {

   // do any killing required
    if (myHow & validKillBitsMask)
      {
      KillIf(pNode, headWaiter);
      if (myHow & AEVT_KILLALLQUEUES)
        {
        KillIf(pNode, head);
        }
      if (myHow & AEVT_KILLSELF)
        {
        delete pNode;
        return TRUE;
				}
      }

   // find the correct place to insert this waiter
    LPEVENTQUEUENODE pTest;
    LPEVENTQUEUENODE pPrev = 0;
    for (
          pTest = headWaiter ;
          pTest && pTest->event.when <= theEvent->when ;
          pTest = pTest->next
        )
      {
      pPrev = pTest;
      }

   // waiter points to next (chronological) waiter
    pNode->next = pTest;

   // link waiter in (towards now)
    if (pPrev)
      pPrev->next = pNode;
    else
      headWaiter = pNode;

	 // bye
    return TRUE;
    }

//----- being here means we're actually posting to the event queue -----//

 // carry out any requested hit contract

  if (myHow & validKillBitsMask)
    {
    KillIf(pNode, head);
    if (myHow & AEVT_KILLALLQUEUES)
      {
      KillIf(pNode, headWaiter);
      }
    if (myHow & AEVT_KILLSELF)
      {
      delete pNode;
      return TRUE;
      }
    }


 // AEVT_ATTAIL is easy...

  if (myHow & AEVT_ATTAIL)
    {
		ADBG_EVAL(PrintEventPush(&pNode->event, pNode->nodeID));

    if (!head)       //empty queue
		  {
      head = tail = pNode;
      mark = 0;
      return TRUE;
      }
    else
      {
      tail->next = pNode;
      tail = pNode;
      return TRUE;
      }
    }

 // ...AEVT_ATMARK is hard.

  if (myHow & AEVT_ATMARK)
    {
		ADBG_EVAL(PrintEventPush(&pNode->event, pNode->nodeID));

    if (!head)       //empty queue
		  {
      mark = head = tail = pNode;
      return TRUE;
      }
    else if (!mark)  // mark is before the head, ie, we just popped an event
      {
      pNode->next = head;
      head = pNode;
      mark = head;
      }
    else             // insert after mark, move mark to node just inserted
      {
      pNode->next = mark->next;
      mark->next = pNode;
      mark = pNode;
      if (pNode->next == 0)  //if pNode is the tail, update the tail pointer
        tail = pNode;
      }
    return TRUE;
    }

 // ?? hmmm, bad posting protocol -- should never happen!
  return TRUE;

  }

//----------------------------------------------------------------------
//
//    EVENTQUEUE::CallMethod
//
//  Invokes the event handler for each object that responds to the
//  the specified event.  If <how> has the 'AEVT_SELF' and 'AEVT_DIRECT' bits
//  both set, the <targetObject> and <sourceObject> are the only two
//  objects that will receive this event (in that order).  If just
//  the 'AEVT_DIRECT' bit is set, only the targetObject will receive the
//  event.  If just the 'AEVT_SELF' bit is set, only the sourceObject
//  will receive the event.  Guarantees that <sourceObject> will
//  always be the last to have its event handler invoked.  Returns
//  FALSE if no event handlers were invoked.  Called from the OIP
//  function AEvtCallMethod() and the public member function named
//  this->HandleEvent().
//
//----------------------------------------------------------------------
BOOL  EVENTQUEUE::CallMethod(
  pAEvtInfo  theEvent,
  TYPEID          theType)
  {
  AObjMessage       sysRec;
  LPEXERESPONSELIST  pResponseList;
	LPEXERESPONSE      pResponseNow;
  METHODPROC         selfMethod = 0;

#ifdef AW_I_TIME
	LONG	nStartTime = HiResTime();
	LONG	nTotalTime;
	LONG	nResponseTime;
#endif

  BOOL  calledSomething = FALSE;

  TYPEID  sourceType;
  TYPEID  targetType;

  LONG    idx;

  // Ditch if no one listens for this event
	pResponseList = eventList -> GetResponseList(theEvent->what);
  if (!pResponseList)
    goto Bye;

  // Get the object type of the source object
  sourceType = theEvent->sourceObject
                 ? AObjGetType(theEvent->sourceObject)
                 : 0;

  //--------------------------------------------------------------------
  // There are three major cases
  //
  //   Case 1: AEVT_DIRECT bit is set
  //   Case 2: AEVT_SELF bit is set, AEVT_DIRECT bit isn't
  //   Case 3: not AEVT_SELF or AEVT_DIRECT
  //--------------------------------------------------------------------

  if ((theEvent->how & AEVT_DIRECT) == AEVT_DIRECT)
    {

    // *** Case 1: AEVT_DIRECT bit is set ***

    if (theType)
      targetType = theType;
    else
      targetType = theEvent->targetObject
                     ? AObjGetType(theEvent->targetObject)
                     : 0;

    if (targetType)
      {
      pResponseNow = pResponseList->FindNode(targetType);
      if (pResponseNow)
        {
#ifdef AW_I_TIME
				nResponseTime = HiResTime();
#endif

        MakeTheCall(pResponseNow->GetMethod(), theEvent, &sysRec);

#ifdef AW_I_TIME
				nResponseTime = HiResTime() - nResponseTime;
				AddHandlerTime((EVENTID) pResponseNow->GetTypeID(), nResponseTime);
#endif
        calledSomething = TRUE;
        }
      }

    if ((theEvent->how & AEVT_SELF) == AEVT_SELF
           &&
        targetType != sourceType)
      {
      pResponseNow = pResponseList->FindNode(sourceType);
      if (pResponseNow)
        selfMethod = pResponseNow->GetMethod();
      }
    }
  else if ((theEvent->how & AEVT_SELF) == AEVT_SELF)
    {

    // *** Case 2: AEVT_SELF bit is set, AEVT_DIRECT bit isn't ***

    pResponseNow = pResponseList->FindNode(sourceType);
    if (pResponseNow)
      selfMethod = pResponseNow->GetMethod();
    }
  else
    {

    // *** Case 3: not AEVT_SELF or AEVT_DIRECT ***

    // Call the method for all responders, source object last
    for(idx = 0 ; 0 != (pResponseNow = pResponseList->IdxGetNode(idx)) ; idx++)
      {
      sysRec.message1 = 0;
      sysRec.errorMessage[0] = '\0';

      // Ensure the source object is called up last
      if ((!selfMethod)
            &&
          (sourceType)
            &&
          (pResponseNow->GetTypeID() == sourceType))
        {
        selfMethod = pResponseNow->GetMethod();
        }
      else
        {
#ifdef AW_I_TIME
				nResponseTime = HiResTime();
#endif

        MakeTheCall(pResponseNow->GetMethod(), theEvent, &sysRec);

#ifdef AW_I_TIME
				nResponseTime = HiResTime() - nResponseTime;
				AddHandlerTime((EVENTID) pResponseNow->GetTypeID(), nResponseTime);
#endif
        calledSomething = TRUE;
				if (sysRec.message1 == 1)
					break;
				}
			}
    }

	if (selfMethod)
		{
#ifdef AW_I_TIME
		nResponseTime = HiResTime();
#endif

    MakeTheCall(selfMethod, theEvent, &sysRec);

#ifdef AW_I_TIME
		nResponseTime = HiResTime() - nResponseTime;
		AddHandlerTime(MAKEEVENTID('0','0','0','0'), nResponseTime);
#endif

    calledSomething = TRUE;
		}

Bye:

  if (theEvent->what == AEVENT_STARTUP)
    {
    // Give objects a chance to get a window up before
    // we shove the icon out there...
    AEvtPostStandard(0, startupDone, 0, AEVT_ATTAIL);
    }

#ifdef AW_I_TIME
	nTotalTime = HiResTime() - nStartTime;
	AddHandlerTime(theEvent->what, nTotalTime);
#endif

	return calledSomething;
	}


//----------------------------------------------------------------------
//
//    EVENTQUEUE::CheckForMethod
//
//  Returns TRUE if the specified object type knows how to respond
//  to the event specified in the selector.  Based on inner loop of
//  EVENTQUEUE::CallMethod.
//
//----------------------------------------------------------------------
BOOL  EVENTQUEUE::CheckForMethod(
  TYPEID   theType,
  EVENTID  selector)
	{
	LPEXERESPONSELIST  pResponseList;

	pResponseList = eventList -> GetResponseList(selector);

	if (!pResponseList)
		return FALSE;  // no one listens for that event

  return pResponseList->FindNode(theType) ? TRUE : FALSE;
	}


//----------------------------------------------------------------------
//
//    EVENTQUEUE::HandleSignal
//
//  Handles a signal from an object in the object list.  Posts a
//  'AEVENT_CALLFUNCTION' event to the queue for each function that is
//  bound to the specified signal.
//
//----------------------------------------------------------------------
BOOL EVENTQUEUE::HandleSignal(
  pAEvtInfo  theEvent)
	{
  return this->FollowFlow(
        (LPEXEOBJINST)GetObject((SERUID)theEvent->sourceObject),
                           theEvent->details,
                           theEvent->sourceObject);
	}


//----------------------------------------------------------------------
//
//    EVENTQUEUE::CallFunction
//
//  Executes the specified function and posts 'AEVENT_CALLFUNCTION' events
//  for all functions on the specified flow index.
//
//----------------------------------------------------------------------
BOOL  EVENTQUEUE::CallFunction(
  pAEvtInfo  theEvent)
	{
  LPEXEFLOW      exeFlow;
  AObjMessage   sysRec;
  LPEXEFUNCINST  thisFunction;
  SERUID         tempUID;

  // Remember the current function (for oip<Get,Set>Parameter)
  exeFlow = (LPEXEFLOW)theEvent->details;
  thisFunction = exeFlow->GetFlowTargetPtr();

  tempUID.ptr = thisFunction;
  this->functionStack.Push(tempUID);

  // Call that function
  MakeTheCall(thisFunction->GetMethodAddr(), theEvent, &sysRec);

  this->functionStack.Pop();

  // Post a call to all of the functions that hang off the spec'd flow
  return FollowFlow(thisFunction, sysRec.message1, 0L);

	}


//----------------------------------------------------------------------
//
//    EVENTQUEUE::FollowFlow
//
//  Posts a 'AEVENT_CALLFUNCTION' event for each function on the specified
//  flow index out of the specified flow source, which is an Object
//  or a Function.  Assumes that theSourceObject == 0 indicates a flow
//  rather than a signal.
//
//----------------------------------------------------------------------
BOOL EVENTQUEUE::FollowFlow(
  LPEXEFLOWMGR theFlowSource,
  LONG         theFlowIdx,
  OBJECTID     theSourceObject)
  {
  LPEXEFLOW    exeFlow;
  int          key;
  AEvtInfo  herEvent;

 // make sure we have a valid flow source pointer... might be
 // bad if the source objectID of a signal event is garbage
  if (!theFlowSource)
    return FALSE;

 // make sure a flow is positive -- signals can be anything
  if (theSourceObject == 0L && theFlowIdx <= 0)
    return FALSE;

 // start filling in an event record to use
  herEvent.sourceObject = theSourceObject;
  herEvent.targetObject = 0;
  herEvent.how          = AEVT_ATMARK;
  herEvent.modifiers    = 0;
  herEvent.variation    = 0;

 // loop over each
  key = 0;
  while (0 != (exeFlow = (theFlowSource->GetNextFlow(theFlowIdx, key))))
    {
    herEvent.details = (LONG)exeFlow;

   // is this a Stop (ie, breakpoint) ?
    if (TestSingleStepFlag() || (this->isDebugging && exeFlow->DoStop()))
      {
      herEvent.what = AEVENT_PAUSEPROGRAM;
      herEvent.when = AAppGetTime();
      this->Push(&herEvent);
      
      //Clear the context's SingleStepFlag but don't clear the queue's 
      //SingleStepFlag since we may need it later.
      pTheSystem->GetCurrentContext()->ClearSingleStepFlag();
      }

    herEvent.what = AEVENT_CALLFUNCTION;
    herEvent.when = AAppGetTime();
    this->Push(&herEvent);
    }

  return TRUE;
  }

///////////////////////////////////////////////////////////////////////
//
// private member functions
//
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// (private) MakeTheCall
//
// Call the indicated serius method with the passed event and system
// records.  Returns FALSE if the method returned an error string.
//---------------------------------------------------------------------
BOOL EVENTQUEUE::MakeTheCall(
	METHODPROC     theRoutine,
	pAEvtInfo theEvent,
	pAObjMessage      theSystem)
	{
	AEvtInfo    passedEvent;  // copy of theEvent for oip user to muck

	// Initialize the system record
	theSystem->message1 = 0;
	theSystem->message2 = 0;
	theSystem->message3 = 0;
	theSystem->message4 = 0;
	theSystem->errorMessage[0] = '\0';

	// Call'em up
	passedEvent = *theEvent;
	(theRoutine) (&passedEvent, theSystem);

	// Show any error message
	if (theSystem->errorMessage[0] != '\0')
		{
		MessageBox(NULL,
							 (LPSTR) &(theSystem->errorMessage),
							 "Warning",
							 MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL);
		return FALSE;
		}

	return TRUE;
	}
