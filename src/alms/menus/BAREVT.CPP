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
// FILE:					BAREVT.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:	  Bar Object event handlers
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>

#include <o_menus.h>
#include <o_wnd.h>

#include "menuobj.h"
#include "menulib.h"
#include "hmenumgr.h"
#include "barlib.h"

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarStartup
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI BarStartup(
  pAEvtInfo     pEvent,
  pAObjMessage  pSystem)
	{
	AOBJECTID	   barID;
	LONG		     getkey;
  LPMENUENTRY  pMenuEntry;
  LPBARINFO    pBarInfo;
  int          i;

  I_UNUSED(pEvent);
  I_UNUSED(pSystem);

  //initialize owner list

	getkey = AOBJ_GETFIRSTKEY;

	while( (barID = AObjGetNextObject(OTYPE_MENUBAR, &getkey)) != 0 )
		{
    pBarInfo = (LPBARINFO)AObjLockData(barID, BAR_INFO_DATA);
    if ((pBarInfo->menuCount > 0) &&
        (0 != (pMenuEntry = (LPMENUENTRY)AObjLockData(barID, BAR_MENULIST_DATA))))
      {
      for (i=0; i<pBarInfo->menuCount; i++)
        AObjRegisterOwner(barID, pMenuEntry[i].id, i);
      AObjUnlockData(barID, BAR_MENULIST_DATA);
      }
    AObjUnlockData(barID, BAR_INFO_DATA);
		}

	}

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarObjectChanged
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI BarObjectChanged(
  pAEvtInfo     pEvent,
  pAObjMessage  pSystem)
  {
  AObjOwnerInfo  ownerInfo;
  HMENUMGR*      hmenuMgr;
  AOBJECTID       menuID;
  UINT           flags;

  I_UNUSED(pSystem);

  ownerInfo.key = AOBJ_GETFIRSTKEY;

  menuID = pEvent->targetObject;

  if (AObjCheckType(menuID, OTYPE_MENU))
    {
    flags = MF_BYPOSITION;
    if (TRUE == OMenuIsEnabled(menuID))
      flags |= MF_ENABLED;
    else
      flags |= MF_GRAYED;

    while (AObjGetNextOwner(OTYPE_MENUBAR, menuID, &ownerInfo))
      {
      hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(ownerInfo.ownerID);
      if (0 != hmenuMgr->GetMenuHandle())
        {
				if (-1 == EnableMenuItem(hmenuMgr->GetMenuHandle(),
                                 (int)ownerInfo.ownerData, flags))
          ADBG_PRINT_E("Bar Object Object Changed: EnableMenuItem failed");
        OWndRedrawMenuBar(ownerInfo.ownerID);
        AEvtPostStandard(ownerInfo.ownerID, AEVENT_OBJECTCHANGED, 0,
         AEVT_ATTAIL|AEVT_KILLDUPLICATES);
        }
      }
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarGetBarHandle
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI EvtGetBarHandle(
  pAEvtInfo     pEvent,
  pAObjMessage  pSystem)
  {

  I_UNUSED(pSystem);

  ADBG_ASSERT_E(pEvent->details != 0, return,
   "Menus, EvtGetBarHandle: details pointer is null");

  ADBG_ASSERT_E(
   TRUE == AObjCheckType(((LPMENUSPARAMBLOCK)pEvent->details)->objID, OTYPE_MENUBAR),
   return, "Menus, EvtGetBarHandle: object is not a menu bar");

  ((LPMENUSPARAMBLOCK)pEvent->details)->hMenu =
   BarGetBarHandle(((LPMENUSPARAMBLOCK)pEvent->details)->objID);

  if ((pEvent->sourceObject) && AObjCheckType(pEvent->sourceObject, OTYPE_WINDOW))
    {   // Register accelerator keys for this menu bar
    BarRegisterAccelerators(((LPMENUSPARAMBLOCK)pEvent->details)->objID, pEvent->sourceObject);
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EvtReleaseBarHandle
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI EvtReleaseBarHandle(
  pAEvtInfo     pEvent,
  pAObjMessage  pSystem)
  {

  I_UNUSED(pSystem);

  ADBG_ASSERT_E(pEvent->details != 0, return,
   "Menus, EvtReleaseBarHandle: details pointer is null");

  ADBG_ASSERT_E(
   TRUE == AObjCheckType(((LPMENUSPARAMBLOCK)pEvent->details)->objID, OTYPE_MENUBAR),
   return, "Menus, EvtReleaseBarHandle: object is not a menu bar");

  BarReleaseBarHandle(((LPMENUSPARAMBLOCK)pEvent->details)->objID,
   ((LPMENUSPARAMBLOCK)pEvent->details)->hMenu);

  }

