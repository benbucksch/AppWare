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
// DESCRIPTION:   Notification Object Administration Function,
//                NOTIFY class functions, and LibMain.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>

#include "notify.h"

// Global variables ////////////////////////////////////////////////////////

HINSTANCE   g_hinstDLL;

////////////////////////////////////////////////////////////////////////////

extern "C" int far pascal LibMain
  (
  HINSTANCE hInstance,
  WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine
  )

  {
  I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

  g_hinstDLL = hInstance;
  if(wHeapSize>0)
    UnlockData(0);
  return 1;
  }

////////////////////////////////////////////////////////////////////////////

extern "C" long ALMAPI NotifyAdmn
  (
  OBJECTID      oiNotify,
  pAObjMessage  pMsg
  )

  {
  Notify* notify;
  long    lRet;

  switch((int)pMsg->message1)
    {
    case AOBJ_CREATED:
    case AOBJ_AWAKENED:
      notify = new(oiNotify) Notify(oiNotify);
      I_ASSERT(notify);
      AObjSetRuntimeInfo(oiNotify, (long)notify);
      lRet = (pMsg->message1 == AOBJ_AWAKENED) ? A_CONTINUE : A_OK;
      break;

    case AOBJ_DESTROYED:
    case AOBJ_ASLEEP:
      notify = (Notify*)AObjGetRuntimeInfo(oiNotify);
      I_ASSERT(notify);
      delete notify;
      AObjSetRuntimeInfo(oiNotify, (long)NULL);
      lRet = A_CONTINUE;
      break;

    case AOBJ_CHECKOBJECT:
    case AOBJ_READ:
      lRet = A_OK;
      break;

    default :
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

void* Notify::operator new
  (
  size_t    size,
  OBJECTID  oiNotify
  )

  {
  // allocate the memory out of the object data
  AObjSetDataCount(oiNotify, 1L);
  AObjResizeData(oiNotify, 0L, (long)size);

  return AObjLockData(oiNotify, 0L);
  }

////////////////////////////////////////////////////////////////////////////

void Notify::operator delete
  (
  void*     p
  )

  {
  if (p)
    {
    AObjUnlockData(((Notify*)p)->m_oiSelf, 0L);
    AObjSetDataCount(((Notify*)p)->m_oiSelf, 0L);
    }
  }

////////////////////////////////////////////////////////////////////////////

Notify::Notify
  (
  OBJECTID  oiNtfy
  )

  {
  I_ASSERT(oiNtfy);

  m_oiSelf      = oiNtfy;
  m_sVer        = VER_OBJECT;
  m_fNotify     = FALSE;
  m_hwndFlash   = NULL;
  }

////////////////////////////////////////////////////////////////////////////
