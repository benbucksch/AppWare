/*//////////////////////////////////////////////////////////////
//
//											Copyright 1993, Novell, Inc.
//
//		<standard disclaimers here>
//
//
//--------------------------------------------------------------
//
// FILE:					OIPEVNTS.CPP
//
// AUTHOR:
//
// DESCRIPTION:		OIP event-oriented calls:
//     AEvtCallMethod
//     oipCallTypeMethod
//     AEvtCheckForMethod
//     AEvtPostSignalAtTail
//     AEvtPost
//     AEvtPostSignalAtMark
//     AEvtPostStandard
//
// CHANGES:
//	7/22/93		Scott
//		Removed oipEmulateOSEvent procedure.
//  jon 20-Oct-92 AEvtPostStandard now zeroes modifiers and variation
//  jon 10-Jun-92 created
//
//////////////////////////////////////////////////////////////*/


  #include <string.h>

  #include "almsys.h"
	#include "intalm.h"

  #include "eventque.h"
  #include "system.h"
  #include "kernel.h"
  #include "project.h"
	#include "subject.h"
	#include "exetypes.h"
	#include "objtype.h"
	#include "objdesc.h"
  #include "context.h"

extern "C" {

//----------------------------------------------------------------------
//   AEvtPost
//
// Posts an event record to the shell's queue for processing.
//----------------------------------------------------------------------
BOOL ALMAPI	AEvtPost (pAEvtInfo theEvent)
  {
  if (pTheSystem->IsRuntime())
    {
    theEvent->variation = 0;
    return pTheSystem->GetCurrentContext()->GetEventQueue()->Push(theEvent);
    }
  else
    return FALSE;
  }


//----------------------------------------------------------------------
//   AEvtPostStandard
//
// Posts an event in a "standard" fashion.
//----------------------------------------------------------------------
BOOL  ALMAPI	 AEvtPostStandard (OBJECTID  theObject,
                                   EVENTID   what,
                                   LONG      details,
                                   LONG      how)
  {
  AEvtInfo  event;

  event.sourceObject  = theObject;
  event.targetObject  = theObject;
  event.what          = what;
  event.details       = details;
  event.modifiers     = 0;
  event.when          = AAppGetTime();
  event.how           = how;
  event.variation     = 0;

  return AEvtPost(&event);
  }


//----------------------------------------------------------------------
//   AEvtPostSignalAtTail
//
// Posts a signal event into the queue at tail.
//----------------------------------------------------------------------
BOOL ALMAPI	AEvtPostSignalAtTail (OBJECTID theObject,
                                     LONG     theSignal)
  {
  AEvtInfo  event;

  event.sourceObject  = theObject;
  event.targetObject  = theObject;
  event.when          = AAppGetTime();
  event.how           = AEVT_ATTAIL;
  event.what          = AEVENT_SIGNAL;
  event.details       = theSignal;

  return AEvtPost(&event);
  }


//----------------------------------------------------------------------
//   AEvtPostSignalAtMark
//
// Posts a signal event into the queue at mark.
//----------------------------------------------------------------------
BOOL ALMAPI	AEvtPostSignalAtMark (OBJECTID theObject,
                                     LONG     theSignal)
  {
  AEvtInfo  event;

  event.sourceObject  = theObject;
  event.targetObject  = theObject;
  event.when          = AAppGetTime();
  event.how           = AEVT_ATMARK;
  event.what          = AEVENT_SIGNAL;
  event.details       = theSignal;

  return AEvtPost(&event);
  }


//----------------------------------------------------------------------
//   AEvtCallMethod
//
// "Direct" message send. The selector must be an event id that the
// type of the target object has registered to listen to.
//----------------------------------------------------------------------
BOOL ALMAPI AEvtCallMethod (
	OBJECTID sourceObject,
	OBJECTID targetObject,
	TYPEID   targetType,
	EVENTID  selector,
	LONG     methodParam)
	{
  LPRESPONSE   pResponse;
  BOOL         result = FALSE;
	LPOBJTYPE    pObjType=NULL;
	int          idx;
	HINSTANCE    hDLL;
  METHODPROC   methProc;
	PROJECT far* pProject;

  pTheSystem->GetCurrentContext();

  if (targetType)
    {
    if (targetObject)
			if (!AObjCheckType(targetObject, targetType))
        result = FALSE;
    }
  else  // no target type spec'd
    if (!targetObject)
      result = FALSE;
    else
			targetType = AObjGetType(targetObject);

  AEvtInfo  event;

  event.sourceObject = sourceObject;
  event.targetObject = targetObject;
  event.what = selector;
  event.when = AAppGetTime();
  event.how = AEVT_DIRECT;
  event.variation = 0;
  event.modifiers = 0;
  event.details = methodParam;

	if (pTheSystem->IsRuntime())
		{
		result = pTheSystem->GetCurrentContext()
						 -> GetEventQueue()
						 -> CallMethod(&event, targetType);
		}
	else
		{
		pProject = (PROJECT far*)GetObject(pTheSystem->GetFileObject()->GetHeadRec());

		idx=0;
		while(0 != (pObjType = (LPOBJTYPE)GetObject(pProject->typeBlock, idx)))
			if(pObjType->typeID == targetType)
				break;

		if (0 != pObjType)
			{
			pResponse = pObjType->pObjDesc->responses.pGetResponse(selector);
			if (pResponse != 0)
				{
				if ((hDLL = LoadLibrary(pResponse->GetDll())) < HINSTANCE_ERROR)
					{
#ifdef DEBUG
					MessageBox(NULL, "Can't load object RESPONSE DLL", "CALLMETHOD",
					 MB_OK);
#endif
					}
				else if (0 != (methProc = (METHODPROC)GetProcAddress(hDLL,
								 pResponse->pGetFunction())))
					{
					AObjMessage theSystem;

					theSystem.message1 = 0;
					theSystem.message2 = 0;
					theSystem.message3 = 0;
					theSystem.message4 = 0;
					theSystem.errorMessage[0] = '\0';

					methProc(&event, &theSystem);

					if (theSystem.errorMessage[0] != '\0')
						MessageBox(NULL, (LPSTR) &(theSystem.errorMessage), "ERROR",
						 MB_ICONEXCLAMATION | MB_OK);
					else
						result = TRUE;
					FreeLibrary(hDLL);
					}
				else
					{
#ifdef DEBUG
					MessageBox(NULL, "Can't Get METHOD address", "CALLMETHOD", MB_OK);
#endif
					FreeLibrary(hDLL);
					}
				}
			else
				result = FALSE;
			}
		}

	return result;

	}


//----------------------------------------------------------------------
//   AEvtCheckForMethod
//
// See if the specified type listens for the specified event.
//----------------------------------------------------------------------
BOOL ALMAPI	AEvtCheckForMethod (TYPEID  theType,
																	 EVENTID selector)
	{
	LPOBJTYPE   pObjType;
	LPSUBTYPE		pSubType;
	RUNTIMEUID  objTypesBlock;
	int         idx;

	if (pTheSystem->IsRuntime())
		{
		return pTheSystem->GetCurrentContext()
						 -> GetEventQueue()
						 -> CheckForMethod(theType, selector);
		}
	else
		{
		pTheSystem->GetCurrentContext();

		objTypesBlock =
		 ((PROJECT far*)GetObject(pTheSystem->GetFileObject()->GetHeadRec()))
			->activeSubject->theSubTypes;

		idx = 0;
		while (0 != (pSubType = (LPSUBTYPE)GetObject(objTypesBlock, idx)))
			{
			if (pSubType->typeID == theType)
				{
				pObjType = (LPOBJTYPE)GetObject(pSubType->objTypeUID);
				if (0 != pObjType->pObjDesc->responses.pGetResponse(selector))
					return TRUE;
				else
					return FALSE;
				}
			}
		}

	return FALSE;

	}

} // extern "C"
