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
// FILE:          "sbar_evnt.cpp"
//
// AUTHOR:
//
// DESCRIPTION:   Event handling functions for the SCROLLBAR object
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define ASTRICT
#define STRICT

#include <stdlib.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>      // window item protocol.
#include <o_wnd.h>       // window object API.
#include <o_number.h>

#include "sbar_hdr.h"
#include "scroll.h"

// Sbar_objectChanged //////////////////////////////////////////////////////

void far pascal _export Sbar_objectChanged
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )
  
  {
  I_UNUSED(pMsg);

  OBJECTID      targetID = pEvent->targetObject;
  AObjOwnerInfo ownerInfo;
  SCROLLBAR*    sbar;

  ownerInfo.key = AOBJ_GETFIRSTKEY; // Get each scrollBar that owns object.
  while (AObjGetNextOwner(scrollBarTypeID, targetID, &ownerInfo))
    {
    sbar = (SCROLLBAR*) AObjGetRuntimeInfo(ownerInfo.ownerID);
    if(sbar->PositionObject()==targetID)
      sbar->SetPosition((long) ONmbrGetInteger(sbar->PositionObject()));
    }
  }


// Sbar_startupEvent ///////////////////////////////////////////////////////

void far pascal _export Sbar_startupEvent
  (
  pAEvtInfo     pEvent,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvent);
  I_UNUSED(pMsg);

  OBJECTID    sbarID;
  long        sbarKey = AOBJ_GETFIRSTKEY;
  SCROLLBAR*  sbar;

  while((sbarID = AObjGetNextObject(scrollBarTypeID, &sbarKey))!=NULL)
    {
    sbar = (SCROLLBAR*) AObjGetRuntimeInfo(sbarID);

    if (sbar != NULL)
    if(sbar->PositionObject())
      { // register position obj for obj changes
      AObjRegisterOwner(sbarID, sbar->PositionObject(), 0L);
      sbar->SetPosition((long) ONmbrGetInteger(sbar->PositionObject()));
      }
    }
  }

// Sbar_MaybePostSignals ///////////////////////////////////////////////////

extern "C" void far pascal _export Sbar_MaybePostSignals
  (
  pAEvtInfo       pEvent,
  pAObjMessage    pMsg
  )

  {
  I_UNUSED(pMsg);

  SCROLLBAR*  sbar          = (SCROLLBAR*)pEvent->details;
  long        lNewPosition  = pEvent->modifiers;
  long        lidSignal     = pEvent->variation;

  if(sbar && sbar->SetPosition(lNewPosition))
    {
    AEvtPostStandard(sbar->ObjectID(), AEVENT_SIGNAL, lidSignal, AEVT_ATONCE);
    AEvtPostStandard(sbar->ObjectID(), AEVENT_SIGNAL, SIGNAL_CHANGED, scrollModifiers);
    }
  }

////////////////////////////////////////////////////////////////////////////
