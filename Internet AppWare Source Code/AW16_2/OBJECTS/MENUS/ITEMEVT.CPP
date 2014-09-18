/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					ITEMEVT.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:   Item Object event handlers
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT


#include <windows.h>

#include "a_alm.h"
#include "itemobj.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemObjectToText
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void ItemObjectToText ( pAEvtInfo theEvent, pAObjMessage /*theSystem*/ )
  {
	LPSTR			  szTitle;
	MEMBLOCKID	textMID = (MEMBLOCKID)theEvent->details;
	LPSTR			  szText;

  szTitle = (LPSTR)AObjLockData(theEvent->targetObject, ITEM_TITLE_DATA);

	AMemSetSize(textMID, lstrlen(szTitle)+1);

	szText = (char*)AMemLock(textMID);

	lstrcpy(szText, szTitle);

  AMemUnlock(textMID);

  AObjUnlockData(theEvent->targetObject, ITEM_TITLE_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemTextToObject
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void ItemTextToObject(
  pAEvtInfo    theEvent,
  pAObjMessage /*theSystem*/)
  {
	MEMBLOCKID  textMID = (MEMBLOCKID)theEvent->details;
	LPSTR			  text;

	text = (char*)AMemLock(textMID);

	AObjSetData(theEvent->targetObject, ITEM_TITLE_DATA, text, lstrlen(text)+1);

  AMemUnlock(textMID);

	AEvtPostStandard(theEvent->targetObject, AEVENT_OBJECTCHANGED,
   0, AEvtGetObjectChangedPriority(AEVT_KILLPARTICULARS));

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemEvent
//
// DESCRIPTION.: Event handler for menu item events
//
////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI ItemEvent(
  pAEvtInfo     eventPtr,
  pAObjMessage  systemPtr)
  {

	if (eventPtr->what == AEVENT_OBJECTTOTEXT)
	  {
		ItemObjectToText(eventPtr, systemPtr);
    }

	else if (eventPtr->what == AEVENT_TEXTTOOBJECT)
	  {
    ItemTextToObject(eventPtr, systemPtr);
		return;
    }

  }

