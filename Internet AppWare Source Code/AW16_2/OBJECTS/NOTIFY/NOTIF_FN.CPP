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
// FILE:          "notify.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Notification Object Functions and Methods
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <string.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_wnd.h>
#include <o_text.h>

#include  "notify.h"

////////////////////////////////////////////////////////////////////////////
//  AlertNotify
//
//      inputs:
//        1.  Message (Text Object)
//
//      outputs:
//        none
//
//      flows:
//        1.  After alerting
//
//      description:
//        This function brings up a MessageBox with an OK button.
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI AlertNotify
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID    oiText  = AFuncGetTypedParameter(1L, OTYPE_TEXT);
  OBJECTID    oiTitle = AFuncGetTypedParameter(2L, OTYPE_TEXT);
  MEMBLOCKID  miText;
  MEMBLOCKID  miTitle;
  char*       pText  = NULL;
  char*       pTitle = NULL;

  if(!oiText)
    {
    AFuncReportResult(pMsg, 1L, "Alert Notify--The first input must be a Text Object.");
    return;
    }

  miText  = AMemAllocate(0L);

  if(AObjConvertToText(oiText, miText)
       &&
     ((pText=(char*)AMemLock(miText))!=NULL))
    {
    miTitle = AMemAllocate(0L);

    if(oiTitle)
      {
      AObjConvertToText(oiTitle, miTitle);
      pTitle = (char*)AMemLock(miTitle);
      }
               
    MessageBox(NULL, pText, pTitle ? pTitle : "Alert", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);

    if(pTitle)
      AMemUnlock(miTitle);
    AMemFree(miText);

    AFuncReportResult(pMsg, 1L, NULL); 
    }
  else
    AFuncReportResult(pMsg, 1L, "Alert Notify -- Could not get the text of the object.");

  if(pText)
    AMemUnlock(miText);
  AMemFree(miText);
  }


////////////////////////////////////////////////////////////////////////////
//  StartNotify
//
//      inputs:
//        1.  Window (Optional)
//        2.  Notification
//
//      outputs:
//        none
//
//      flows:
//        1.  After starting
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI StartNotify
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiNotify = AFuncGetTypedParameter(1L, OTYPE_NOTIFY);

  if(oiNotify)
    {
    Notify* notify = (Notify*)AObjGetRuntimeInfo(oiNotify);

    I_ASSERT(notify);

    if(!notify->DoNotify())
      {
      AEvtInfo  event;
      OBJECTID  oiWnd = AFuncGetTypedParameter(2L, OTYPE_WINDOW);

      notify->SetNotify(TRUE);
      notify->SetWindow(oiWnd ? OWndGetHandle(oiWnd) : NULL);

      // sent the flash event
      event.sourceObject  = oiNotify;
      event.targetObject  = oiNotify;
      event.what          = OEVT_NOTIFY_FLASH;
      event.details       = 0L;
      event.when          = 0L;
      event.how           = AEVT_ATTAIL | AEVT_DIRECT;
      event.variation     = 0L;
      AEvtPost(&event);
      }

    AFuncReportResult(pMsg, 1L, NULL);
    }
  else
    AFuncReportResult(pMsg, 1L, "The \"Notification\" input was invalid.");
  }

////////////////////////////////////////////////////////////////////////////
//  StopNotify
//
//      inputs:
//        1.  Notification
//
//      outputs:
//        none
//
//      flows:
//        1.  After stopping
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI StopNotify
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiNotify = AFuncGetTypedParameter(1L, OTYPE_NOTIFY);

  if(oiNotify)
    {
    Notify* notify  = (Notify*)AObjGetRuntimeInfo(oiNotify);

    I_ASSERT(notify);

    if(notify->DoNotify())
      {
      notify->SetNotify(FALSE);
      FlashWindow(notify->GetWindow(), FALSE);
      }

    AFuncReportResult(pMsg, 1L, NULL);
    }
  else
    AFuncReportResult(pMsg, 1L, "The \"Notification\" input was invalid.");
  }

////////////////////////////////////////////////////////////////////////////
// FlashNotify method -- flashes the window
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI FlashNotify
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  OBJECTID  oiNotify = pEvt->targetObject;

  if(AObjCheckType(oiNotify, OTYPE_NOTIFY))
    {
    Notify* notify = (Notify*)AObjGetRuntimeInfo(oiNotify);

    I_ASSERT(notify);

    if(notify->DoNotify())
      {
      AEvtInfo  event;

      FlashWindow(notify->GetWindow(), TRUE);

      event.sourceObject  = oiNotify;
      event.targetObject  = oiNotify;
      event.what          = OEVT_NOTIFY_FLASH;
      event.details       = 0L;
      event.when          = AAppGetTime() + 30L;
      event.how           = AEVT_KILLDUPLICATES | AEVT_ATTAIL | AEVT_DIRECT;
      event.variation     = 0L;

      AEvtPost(&event);
      }
    }

  AFuncReportResult(pMsg, 1L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//  Shutdown Event handler--clears all notifications
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI ShutDownEvent
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);
  I_UNUSED(pMsg);

  long      lGetKey   = AOBJ_GETFIRSTKEY;
  OBJECTID  oiNotify;
  Notify*   notify;

  while((oiNotify=AObjGetNextObject(OTYPE_NOTIFY, &lGetKey))!=NULL)
    {
    notify = (Notify*)AObjGetRuntimeInfo(oiNotify);

    I_ASSERT(notify);

    notify->SetNotify(FALSE);
    FlashWindow(notify->GetWindow(), FALSE);
    }
  }

////////////////////////////////////////////////////////////////////////////
