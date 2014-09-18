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
// FILE:          "sbr_main.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is the main file for the Scroll Bar object.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_number.h>

#if !defined(__FLAT__)
#include "heap.h"
#endif

#include "scroll.h"
#include "scrol_vi.h"

// Global variables ////////////////////////////////////////////////////////

HINSTANCE   g_hinstDLL;

#if !defined(__FLAT__)
// ****************************************************************
//
// Overridden operator new and delete so we don't eat up selectors
// with small C++ objects.
//
// ****************************************************************

void*   operator new(
  size_t    s
  )
  {
  return MemAlloc(s);
  }

void    operator delete(
  void*     p
  )
  {
  if (p)
    MemFree(p);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     LibMain
//
//  DESCRIPTION:  This is the module that gets called when the DLL is first
//                loaded into memory.  All global initialization should be
//                done here.
//
////////////////////////////////////////////////////////////////////////////

extern "C" int WINAPI LibMain
  (
  HINSTANCE hinstDLL,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine
  )

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  g_hinstDLL = hinstDLL;

  #ifdef __BORLANDC__
  extern int  _WinAllocFlag;
  _WinAllocFlag = GMEM_SHARE;
  #endif

  if(wHeapSize>0)
    UnlockData(0);

  ScrollVI::InitDLL();

  return 1;
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     WEP
//
//  DESCRIPTION:  This is the Windows Exit Procedure (WEP).  This is called
//                when the DLL is being removed from memory.  All handles
//                and resources should be released after this is called.
//
////////////////////////////////////////////////////////////////////////////

int WINAPI WEP
  (
  int nParameter
  )

  {
  I_UNUSED(nParameter);

  ScrollVI::FreeDLL();

  return 1;
  }

#else

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     DllEntryPoint
//
//  DESCRIPTION:  This is the module that gets called when the DLL is first
//                loaded into memory or freed from memory.  All global
//                initialization should be de-allocation should be done here.
//
////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllEntryPoint
  (
  HINSTANCE hinstDLL,
  DWORD     fdwReason,
  LPVOID    pReserved
  )

  {
  I_UNUSED(pReserved);

  switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
      g_hinstDLL = hinstDLL;
      ScrollVI::InitDLL();
      break;

    case DLL_PROCESS_DETACH:
    case DLL_THREAD_DETACH:
      ScrollVI::FreeDLL();
      break;

    default:
      I_ASSERT(0);
      break;
    }

  return TRUE;
  }

#endif
////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     ScrollAdmn
//
//  DESCRIPTION:  This is the administration function for the Scroll object.
//                This is called with various messages and requests.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

extern "C" long ALMCALLBACK ScrollAdmn
  (
  OBJECTID      oiScroll,
  pAObjMessage  pMsg
  )

  {
  Scroll*   pScroll;
  long      lRet;

  switch(pMsg->message1)
    {
    case AOBJ_AWAKENED:   // object receives this message after being read from disk
      pMsg->message4 = 0;
      // fall through
    case AOBJ_CREATED:    // sent when the AppWare object is first created
      pScroll = new Scroll(oiScroll, (pMsg->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE);
      if(pScroll && pScroll->Valid())
        {
        AObjSetRuntimeInfo(oiScroll, (long)pScroll);
        lRet = (pMsg->message1 == AOBJ_AWAKENED) ? A_CONTINUE : A_OK;
        }
      else
        {
        delete pScroll;
        AObjSetRuntimeInfo(oiScroll, (long)NULL);
        lRet = A_ERROR;
        }
      break;

    case AOBJ_DESTROYED:  // sent when object is destroyed or delete
    case AOBJ_ASLEEP:     // sent when worksheet is closed or application terminates
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      delete pScroll;
      AObjSetRuntimeInfo(oiScroll, (long)NULL);
      lRet = A_CONTINUE;
      break;

    case AOBJ_ASSIGNOBJECT:   // set an object to the same state as another object.
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      if(Scroll::CheckVersion((OBJECTID)pMsg->message3) != A_ERROR)
        {
        if(AObjGetDataCount((OBJECTID)pMsg->message3) != 0)
          pScroll->InitializeFromData();

        lRet = pScroll->Assign((Scroll*)AObjGetRuntimeInfo((OBJECTID)pMsg->message3));
        }
      else
        lRet = A_ERROR;
      break;

    case AOBJ_BUILDSIGNALS:   // system is asking for signal information
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->BuildSignals((MEMBLOCKID)pMsg->message2);
      break;

    case AOBJ_CHECKOBJECT:    // object being checked for errors
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->Check();
      break;

    case AOBJ_EDITOBJECT:     // object is being requested to edit
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->Edit();
      break;

    case AOBJ_EXPORTOBJECT:   // export object to UPSF file
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->Export();
      break;

    case AOBJ_IMPORTOBJECT:   // import the object from UPSF file
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->Import();
      break;

    case AOBJ_POSTWRITE:      // the object has finished writing to file
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->PostWrite();
      break;

    case AOBJ_PREWRITE:       // prepare object to write to file
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->PreWrite();
      break;

    case AOBJ_QUERYPROTOCOL:  // does the object/type support a given protocol
      lRet = Scroll::QueryProtocol(oiScroll, (pApQueryInfo)pMsg->message4);
      break;

    case AOBJ_READ:           /* object data read from disk -- check version
                               * and upgrade as necessary.  This is done
                               * before AOBJ_AWAKENED message is sent.
                               */
      lRet = Scroll::CheckVersion(oiScroll);
      if(lRet == A_ERROR)
        AObjReportError(oiScroll, AOBJ_READ, A_ERROR, "Scroll Bar\talm_sbar.dll", k_sVersion);
      break;

    case AOBJ_REPORTREQUIREDFILES:  // Report any files this ALM needs
      lRet = Scroll::ReportRequiredFiles(pMsg);
      break;

    case AOBJ_VALIDATEOBJECTIDS:    // validate all Object IDs this object holds
      pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);
      I_ASSERT(pScroll);
      lRet = pScroll->ValidateIDs(pMsg);
      break;

    default:
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
  }

// Disable ////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK Disable
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);
  Scroll*   pScroll;

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The input must be a Scroll Bar object for the 'Disable Scroll Bar' function.");
  else
    {
    pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);

    if(pScroll->Enabled())
      {
      pScroll->SetEnabled(FALSE);
      AEvtPostStandard(pScroll->ObjectID(), AEVENT_OBJECTCHANGED, 0, AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL));
      }
    AFuncReportResult(pMsg, 1L, NULL);
    }
  }

// Enable /////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK Enable
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID oiScroll = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);
  Scroll*  pScroll;

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The input must be a Scroll Bar object for the 'Enable Scroll Bar' function.");
  else {
    pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);
    if(!pScroll->Enabled())
      {
      pScroll->SetEnabled(TRUE);
      AEvtPostStandard(pScroll->ObjectID(), AEVENT_OBJECTCHANGED, 0, AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL));
      }
    AFuncReportResult(pMsg, 1L, NULL);
    }
  }

// GetIncrement ///////////////////////////////////////////////////////

extern "C" void ALMCALLBACK GetIncrement
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The input must be a Scroll Bar object for the 'Get Scroll Bar Increment' function.");
  else
    {
    OBJECTID  oiLine = AObjCreate(OTYPE_NUMBER);
    OBJECTID  oiPage = AObjCreate(OTYPE_NUMBER);
    Scroll*   pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);
    ONmbrSetInteger(oiLine, pScroll->LineScroll(), TRUE);
    ONmbrSetInteger(oiPage, pScroll->PageScroll(), TRUE);
    AFuncSetParameter(1L, oiLine);
    AFuncSetParameter(2L, oiPage);
    AFuncReportResult(pMsg, 1L, NULL);
    }
  }

// GetPosition ////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK GetPosition
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The input must be a Scroll Bar object for the 'Get Scroll Position' function.");
  else
    {
    OBJECTID  oiPosition = AObjCreate(OTYPE_NUMBER);
    Scroll*   pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);
    ONmbrSetInteger(oiPosition, pScroll->Position(), TRUE);
    AFuncSetParameter(1L, oiPosition);
    AFuncReportResult(pMsg, 1L, NULL);
    }
  }

// GetRange ///////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK GetRange
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The input must be a Scroll Bar object for the 'Get Scroll Range' function.");
  else
    {
    OBJECTID  oiMin = AObjCreate(OTYPE_NUMBER);
    OBJECTID  oiMax = AObjCreate(OTYPE_NUMBER);
    Scroll*   pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);
    ONmbrSetInteger(oiMin, pScroll->Minimum(), TRUE);
    ONmbrSetInteger(oiMax, pScroll->Maximum(), TRUE);
    AFuncSetParameter(1L, oiMin);
    AFuncSetParameter(2L, oiMax);
    AFuncReportResult(pMsg, 1L, NULL);
    }
  }

// IsEnabled //////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK IsEnabled
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The input must be a Scroll Bar object for the 'Is Enabled' function.");
  else
    {
    Scroll* pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);
    if(pScroll->Enabled())
      AFuncReportResult(pMsg, 1L, NULL);
    else
      AFuncReportResult(pMsg, 2L, NULL);
    }
  }


// SetIncrement ///////////////////////////////////////////////////////

extern "C" void ALMCALLBACK SetIncrement
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll  = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);
  OBJECTID  oiLine    = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID  oiPage    = AFuncGetTypedParameter(3L, OTYPE_NUMBER);

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The first input must be a Scroll Bar object for the 'Set Scroll Bar Increment' function.");
  else
    {
    BOOL    fError = FALSE;
    Scroll* pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);

    if(oiLine)
      {
      long  lLine = ONmbrGetInteger(oiLine);
      if(Abs(lLine) <= Abs(pScroll->Maximum() - pScroll->Minimum()))
        pScroll->SetLineScroll(lLine);
      else
        fError = TRUE;
      }
    if(oiPage)
      {
      long  lPage = ONmbrGetInteger(oiPage);
      if(Abs(lPage) <= Abs(pScroll->Maximum() - pScroll->Minimum()))
        pScroll->SetPageScroll(lPage);
      else
        fError = TRUE;
      }

    if(fError)
      AFuncReportResult(pMsg, 2L, NULL);
    else
      AFuncReportResult(pMsg, 1L, NULL);
    }       // else {...
  }

// SetPosition ////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK SetPosition
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);
  OBJECTID  oiPosition = AFuncGetTypedParameter(2L, OTYPE_NUMBER);

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The first input must be a Scroll Bar object for the 'Set Scroll Position' function.");
  else if(!oiPosition)
    AFuncReportResult(pMsg, 0L, "The second input must be a Number object for the 'Set Scroll Position' function.");
  else
    {
    Scroll* pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);
    long    lPos = ONmbrGetInteger(oiPosition);

    if(pScroll->Minimum() < pScroll->Maximum())
      {
      if((lPos < pScroll->Minimum()) || (lPos > pScroll->Maximum()))
        AFuncReportResult(pMsg, 2L, NULL);
      else
        {
        pScroll->SetPosition(lPos);
        AFuncReportResult(pMsg, 1L, NULL);
        }
      }
    else if(pScroll->Minimum() > pScroll->Maximum())
      {
      if((lPos > pScroll->Minimum()) || (lPos < pScroll->Maximum()))
        AFuncReportResult(pMsg, 2L, NULL);
      else
        {
        pScroll->SetPosition(lPos);
        AFuncReportResult(pMsg, 1L, NULL);
        }
      }
    }       // else {...
  }

// SetRange ///////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK SetRange
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OBJECTID  oiScroll  = AFuncGetTypedParameter(1L, OTYPE_SCROLLBAR);
  OBJECTID  oiMin     = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID  oiMax     = AFuncGetTypedParameter(3L, OTYPE_NUMBER);

  if(!oiScroll)
    AFuncReportResult(pMsg, 0L, "The first input must be a Scroll Bar object for the 'Set Scroll Range' function.");
  else
    {
    Scroll* pScroll = (Scroll*) AObjGetRuntimeInfo(oiScroll);
    long    lMin    = oiMin ? ONmbrGetInteger(oiMin) : pScroll->Minimum();
    long    lMax    = oiMax ? ONmbrGetInteger(oiMax) : pScroll->Maximum();

    if(pScroll->SetMinimum(lMin) || pScroll->SetMaximum(lMax))
      {
      if(lMin < lMax)
        {
        if(pScroll->Position() < lMin)
          pScroll->SetPosition(lMin);
        else if(pScroll->Position() > lMax)
          pScroll->SetPosition(lMax);
        }
      else
        {
        if(pScroll->Position() < lMax)
          pScroll->SetPosition(lMax);
        else if(pScroll->Position() > lMin)
          pScroll->SetPosition(lMin);
        }
      }

    if(Abs(pScroll->LineScroll()) > Abs(lMax - lMin))
      pScroll->SetLineScroll(lMax - lMin);

    if(Abs(pScroll->PageScroll()) > Abs(lMax - lMin))
      pScroll->SetPageScroll(lMax - lMin);

    AEvtPostStandard(pScroll->ObjectID(), AEVENT_OBJECTCHANGED, 0, AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL));
    AFuncReportResult(pMsg, 1L, NULL);
    }
  }

// ObjectChanged //////////////////////////////////////////////////////

extern "C" void ALMCALLBACK ObjectChanged
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )
  
  {
  I_UNUSED(pMsg);

  OBJECTID      oiTarget = pEvt->targetObject;
  AObjOwnerInfo ownerInfo;
  Scroll*       pScroll;

  ownerInfo.key = AOBJ_GETFIRSTKEY; // Get each Scroll Bar that owns object.
  while (AObjGetNextOwner(OTYPE_SCROLLBAR, oiTarget, &ownerInfo))
    {
    pScroll = (Scroll*) AObjGetRuntimeInfo(ownerInfo.ownerID);
    if(pScroll && (pScroll->PositionObject() == oiTarget))
      pScroll->SetPosition(ONmbrGetInteger(pScroll->PositionObject()));
    }
  }


// Startup ///////////////////////////////////////////////////////

extern "C" void ALMCALLBACK Startup
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);
  I_UNUSED(pMsg);

  OBJECTID  oiScroll;
  long      lScrollKey = AOBJ_GETFIRSTKEY;
  Scroll*   pScroll;

  while(NULL != (oiScroll = AObjGetNextObject(OTYPE_SCROLLBAR, &lScrollKey)))
    {
    pScroll = (Scroll*)AObjGetRuntimeInfo(oiScroll);

    if(pScroll && pScroll->PositionObject())
      { // register position obj for obj changes
      AObjRegisterOwner(oiScroll, pScroll->PositionObject(), 0L);
      pScroll->SetPosition((long) ONmbrGetInteger(pScroll->PositionObject()));
      }
    }
  }

// MaybePostSignals ///////////////////////////////////////////////////

extern "C" void ALMCALLBACK MaybePostSignals
  (
  pAEvtInfo       pEvt,
  pAObjMessage    pMsg
  )

  {
  I_UNUSED(pMsg);

  PostSignalsEvt*   pse = (PostSignalsEvt*)pEvt->details;

  if(pse && pse->pScroll)
    {
    if(pse->lidSignal == k_lSignalThumbPosition)
      {
      AEvtPostStandard(pse->pScroll->ObjectID(), AEVENT_SIGNAL, pse->lidSignal, AEVT_ATONCE);
      if(pse->pScroll->SetPosition(pse->lPosition))
        AEvtPostStandard(pse->pScroll->ObjectID(), AEVENT_SIGNAL, k_lSignalChanged, AEVT_KILLPARTICULARS | AEVT_ATTAIL);
      }
    else if(pse->pScroll->SetPosition(pse->lPosition))
      {
      AEvtPostStandard(pse->pScroll->ObjectID(), AEVENT_SIGNAL, pse->lidSignal, AEVT_ATONCE);
      AEvtPostStandard(pse->pScroll->ObjectID(), AEVENT_SIGNAL, k_lSignalChanged, AEVT_KILLPARTICULARS | AEVT_ATTAIL);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
