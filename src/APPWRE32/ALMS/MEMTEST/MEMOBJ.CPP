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
// FILE:          memobj.cpp
//
// AUTHOR:        Scott McCarty
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
#include "memobj.h"

////////////////

HINSTANCE     g_hDLL;

////////////////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	g_hDLL = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}


////////////////////////////////////////////////////////////////////////////

extern "C"
LONG ALMCALLBACK	MemTestAdmin
  (
  AOBJECTID      oiMem,
  pAObjMessage  pMsg
  )

  {
  LONG      lRet		= A_NOTHANDLED;

  switch(pMsg->message1)
    {
    case AOBJ_CREATED:
			AObjSetDataCount(oiMem, 1);
			I_ASSERT(AObjGetDataCount(oiMem) == 1);
			AObjResizeData(oiMem, 0, sizeof(MemTestStatus));
			I_ASSERT(AObjGetDataSize(oiMem, 0) == sizeof(MemTestStatus));
			break;

    case AOBJ_AWAKENED:
    case AOBJ_ASLEEP:
    case AOBJ_DESTROYED:
      break;

    case AOBJ_ASSIGNOBJECT:
			lRet = A_ERROR;
      break;

    case AOBJ_BUILDSIGNALS:
      break;

    case AOBJ_CHECKOBJECT:
      break;

    case AOBJ_EDITOBJECT:
      break;

    case AOBJ_EXPORTOBJECT:
      break;

    case AOBJ_IMPORTOBJECT:
      break;

    case AOBJ_READ:
			lRet = A_OK;
      break;

    case AOBJ_VALIDATEOBJECTIDS:
      break;
      
    default:
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
  }

