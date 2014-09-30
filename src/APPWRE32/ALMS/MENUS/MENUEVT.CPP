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
// FILE:					MENUEVT.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Menu object event handlers
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
#include <string.h>

#include "menuobj.h"
#include "hmenumgr.h"
#include "itemobj.h"
#include "itemlib.h"
#include "menulib.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateMenuItem
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void  UpdateMenuItems(
  AObjOwnerInfo   ownerInfo,
  AOBJECTID        itemID)
  {
  LPITEMINFO      pItemInfo;
  HMENUMGR*       hmenuMgr;
  long            itemStatus;
  char            title[MENU_MAX_TEXT+1];
  char            accelTitle[ACCEL_MAX_TEXT+1];
//  LPSTR           pAccelTitle = accelTitle;
//  LPSTR           pTitle = (LPSTR)title; 
  UINT            flags;

  flags = (MF_BYCOMMAND|MF_STRING);
  itemStatus = ItemGetStatus(itemID);

  if (itemStatus & ITEM_DISABLED)
    flags |= MF_GRAYED;
  else
    flags |= MF_ENABLED;

  if (itemStatus & ITEM_TOGGLE_TYPE)
    {
    if (itemStatus & ITEM_TOGGLED_ON)
      {
      ItemGetToggledTitle(itemID, title, MENU_MAX_TEXT+1);
      if (itemStatus & ITEM_SHOW_CHECKMARK)
        flags |= MF_CHECKED;
      }
    else
      OMenuGetTitle(itemID, title, MENU_MAX_TEXT+1);
    }
  else
    OMenuGetTitle(itemID, title, MENU_MAX_TEXT+1);

  // Now append command key text to menu title
  if ((pItemInfo = (LPITEMINFO)AObjLockData(itemID, ITEM_INFO_DATA)) != 0)
    {
    GetCommandKeyText(accelTitle, pItemInfo->accelKey, pItemInfo->accelModifier);
    strcat(title, accelTitle);
    AObjUnlockData(itemID, ITEM_INFO_DATA);
    }

  while (AObjGetNextOwner(OTYPE_MENU, itemID, &ownerInfo))
    {
    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(ownerInfo.ownerID);
    if (0 != hmenuMgr->GetMenuHandle())
      {
      ModifyMenu(hmenuMgr->GetMenuHandle(), (int)ownerInfo.ownerData,
                 flags, (int)ownerInfo.ownerData, title);
      }
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateSubMenus
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void static UpdateSubMenus(
  AObjOwnerInfo   ownerInfo,
  AOBJECTID        menuID)
  {
  HMENUMGR*       hmenuMgr;
//  long            itemStatus;
//  char            title[MENU_MAX_TEXT+1];
  UINT            flags;
  int             i;
  char            title[MENU_MAX_TEXT+1];
  LPMENUINFO      pMenuInfo;
  LPITEMENTRY     pItemEntry;
  
  flags = MF_BYPOSITION | MF_POPUP;
  if (TRUE == OMenuIsEnabled(menuID))
    flags |= MF_ENABLED;
  else
    flags |= MF_GRAYED;

  OMenuGetTitle(menuID, title, MENU_MAX_TEXT+1);

  while (AObjGetNextOwner(OTYPE_MENU, menuID, &ownerInfo))
    {
    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(ownerInfo.ownerID);
    if (0 != hmenuMgr->GetMenuHandle())
      {
      // Get position of sub menu
      pMenuInfo = (LPMENUINFO)AObjLockData(ownerInfo.ownerID, MENU_INFO_DATA);
      pItemEntry = (LPITEMENTRY)AObjLockData(ownerInfo.ownerID, MENU_ITEMLIST_DATA);
      for (i=0; i<pMenuInfo->itemCount; i++)
        {
        if (pItemEntry[i].id == (AOBJECTID)ownerInfo.ownerData)
          break;
        }
      AObjUnlockData(ownerInfo.ownerID, MENU_ITEMLIST_DATA);   // Release pItemEntry
      AObjUnlockData(ownerInfo.ownerID, MENU_INFO_DATA);       // Release pMenuInfo
      // i = position of submenu
      
			if (-1 == EnableMenuItem(hmenuMgr->GetMenuHandle(),i,flags))
        ADBG_PRINT_E("Menu Object Object Changed: EnableMenuItem failed");

      ModifyMenu(hmenuMgr->GetMenuHandle(), (UINT)i, flags, (UINT)i, title);
      AEvtPostStandard(ownerInfo.ownerID, AEVENT_OBJECTCHANGED, 0,
                       AEVT_ATTAIL|AEVT_KILLDUPLICATES);
      }
    }
  }
////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuStartup
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI MenuStartup(
  pAEvtInfo     pEvent,
  pAObjMessage  pSystem)
	{
	AOBJECTID	   menuID;
	LONG		     getkey;
  LPITEMENTRY  pItemEntry;
  LPMENUINFO   pMenuInfo;
  int          i;

  I_UNUSED(pEvent);
  I_UNUSED(pSystem);


  //initialize owner list

	getkey = AOBJ_GETFIRSTKEY;

	while( (menuID = AObjGetNextObject(OTYPE_MENU, &getkey)) != 0 )
		{
    pMenuInfo = (LPMENUINFO)AObjLockData(menuID, MENU_INFO_DATA);
    if ((pMenuInfo->itemCount > 0) &&
        (0 != (pItemEntry = (LPITEMENTRY)AObjLockData(menuID, MENU_ITEMLIST_DATA))))
      {
      for (i=0; i<pMenuInfo->itemCount; i++)
        {
        if (AObjCheckType(pItemEntry[i].id, OTYPE_MENUITEM))  // Menu item so register menu item id for OWNER DATA
          {
          AObjRegisterOwner(menuID, pItemEntry[i].id, pItemEntry[i].menuItemID);
          }
        else                                        // Sub menu so register object id for OWNER DATA
          {
          AObjRegisterOwner(menuID, pItemEntry[i].id, (int)pItemEntry[i].id);
          }
        }
      AObjUnlockData(menuID, MENU_ITEMLIST_DATA);
      }
    AObjUnlockData(menuID, MENU_INFO_DATA);
		}

	}

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuObjectChanged
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI MenuObjectChanged(
  pAEvtInfo     pEvent,
  pAObjMessage  pSystem)
  {
  AObjOwnerInfo   ownerInfo;
  AOBJECTID        itemID;

  I_UNUSED(pSystem);

  ownerInfo.key = AOBJ_GETFIRSTKEY;

  itemID = pEvent->targetObject;

  if (AObjCheckType(itemID, OTYPE_MENUITEM))
    {
    UpdateMenuItems(ownerInfo, itemID);
    }
  else if (AObjCheckType(itemID, OTYPE_MENU))
    {
    UpdateSubMenus(ownerInfo, itemID);
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EvtHandleCommand
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI EvtHandleCommand(
  pAEvtInfo     pEvent,
  pAObjMessage  pSystem)
  {
  short     itemID = (short) ((LPMENUSPARAMBLOCK)pEvent->details)->itemID;

  I_UNUSED(pSystem);

  MenuHandleCommand(itemID);

  }

