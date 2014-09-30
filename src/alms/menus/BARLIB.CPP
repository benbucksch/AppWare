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
// FILE:					BARLIB.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Utility routines for menu bar objects.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>

#include "a_alm.h"
#include "barobj.h"
#include "itemobj.h"
#include "menuobj.h"
#include "menulib.h"
#include "o_menus.h"
#include "o_wnd.h"
#include "hmenumgr.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DeleteMenuEntries
//
// DESCRIPTION.: Deletes menu entries marked for deletion and compacts the
//               the remaining entries.  Does not resize the data because it
//               is locked.  Returns the number of entries deleted.
//
////////////////////////////////////////////////////////////////////////////
short DeleteMenuEntries(
  LPMENUENTRY pMenuEntryList,
  short       currentCount)
  {
  short  deleteCount = 0;
  short  i;

  for (i=0; i<currentCount; i++)
    {
    if (deleteCount > 0)
      pMenuEntryList[i-deleteCount] = pMenuEntryList[i];
    if ((pMenuEntryList[i].status & ITEMENTRY_DELETEENTRY) ==
                                                     ITEMENTRY_DELETEENTRY)
      deleteCount++;
    }

  return deleteCount;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarDestroyMenuBar
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void BarDestroyMenuBar(
  AOBJECTID  objID,
  HMENU     hMenuBar)
  {
  HMENU        hMenu;
  LPMENUENTRY  pMenuList;
  LPBARINFO    pBarInfo;
  short        i;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	if (pBarInfo->menuCount > 0)
		{
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
		for (i=0; i<pBarInfo->menuCount; i++)
			{
      hMenu = GetSubMenu(hMenuBar, 0);
      RemoveMenu(hMenuBar, 0, MF_BYPOSITION);
      MenuReleaseMenuHandle(pMenuList[i].id, hMenu);
      }
    AObjUnlockData(objID, BAR_MENULIST_DATA);
		}

  DestroyMenu(hMenuBar);

  AObjUnlockData(objID, BAR_INFO_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarCreateMenuBar
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
HMENU BarCreateMenuBar(
  AOBJECTID  objID)
  {
  HMENU        hMenuBar;
  HMENU        hMenu;
  LPMENUENTRY  pMenuList;
  LPBARINFO    pBarInfo;
  short        i;
  UINT         flags;
  char         title[BAR_MAX_TEXT+1];

  hMenuBar = CreateMenu();

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	if (pBarInfo->menuCount > 0)
		{
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
		for (i=0; i<pBarInfo->menuCount; i++)
			{
      hMenu = MenuGetMenuHandle(pMenuList[i].id);
      OMenuGetTitle(pMenuList[i].id, title, MENU_MAX_TEXT+1);
      if (hMenu != 0)
        {
        flags = MF_POPUP;
        if ((pBarInfo->status & BAR_DISABLED) ||
            (FALSE == OMenuIsEnabled(pMenuList[i].id)))
          flags |= MF_GRAYED;
        AppendMenu(hMenuBar, flags, (UINT)hMenu, title);
        }
			}
    AObjUnlockData(objID, BAR_MENULIST_DATA);
		}

  AObjUnlockData(objID, BAR_INFO_DATA);

  return hMenuBar;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarHandleCommand
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
/* BOOL BarHandleCommand(
  AOBJECTID  objID,
  short     itemID)
  {
  LPMENUENTRY  pMenuList;
  LPBARINFO    pBarInfo;
  short        i;
  BOOL         result = FALSE;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	if (pBarInfo->menuCount > 0)
		{
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
		for (i=0; i<pBarInfo->menuCount; i++)
			{
      result = MenuHandleCommand(pMenuList[i].id, itemID);
      if (result)
        break;
			}
    AObjUnlockData(objID, BAR_MENULIST_DATA);
		}

  AObjUnlockData(objID, BAR_INFO_DATA);

  return result;

  }  */


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarGetBarHandle
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
HMENU BarGetBarHandle(
  AOBJECTID  barID)
  {
  HMENUMGR*  hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(barID);

  if (hmenuMgr->GetMenuHandle() == 0)
    hmenuMgr->SetMenuHandle(BarCreateMenuBar(barID));

  hmenuMgr->IncrementUseCount();

///  AObjSetRuntimeInfo(barID, hmenuMgr);

  return hmenuMgr->GetMenuHandle();

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarReleaseBarHandle
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void BarReleaseBarHandle(
  AOBJECTID  objID,
  HMENU     hMenu)
  {
  HMENUMGR*  hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);

  if (hMenu != 0)
    {
    ADBG_ASSERT_E(hmenuMgr->GetMenuHandle() == hMenu, return,
     "BarReleaseBarHandle: Menu bar handle does not match.");
    }

  ADBG_ASSERT_E(hmenuMgr->GetMenuHandle() != 0, return,
   "BarReleaseBarHandle: Menu bar handle is zero.");

  if (0 == hmenuMgr->DecrementUseCount())
    {
    BarDestroyMenuBar(objID, hmenuMgr->GetMenuHandle());
    hmenuMgr->SetMenuHandle(0);
    }

///  AObjSetRuntimeInfo(objID, hmenuMgr);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarRegisterAccelerators
//
// DESCRIPTION.: Register accelerator keys for each menuitem
//
/////////////////////////////////////////////////////////////////////////////
void BarRegisterAccelerators(
  AOBJECTID      barObjID,
  AOBJECTID      winObjID)
  {
  LPBARINFO     pBarInfo;
  LPMENUENTRY   pMenuList;
  LPMENUINFO    pMenuInfo;
  LPITEMENTRY   pItemEntryList;
  LPITEMINFO    pItemInfo;
  short         i;
  short         j;
  char          accelKey;

  pBarInfo = (LPBARINFO)AObjLockData(barObjID, BAR_INFO_DATA);

	if (pBarInfo->menuCount > 0)
		{
    pMenuList = (LPMENUENTRY)AObjLockData(barObjID, BAR_MENULIST_DATA);
		for (i=0; i<pBarInfo->menuCount; i++)
			{
      pMenuInfo = (LPMENUINFO)AObjLockData(pMenuList[i].id, MENU_INFO_DATA);
      if (pMenuInfo->itemCount > 0)
        {
        pItemEntryList = (LPITEMENTRY)AObjLockData(pMenuList[i].id, MENU_ITEMLIST_DATA);
        for (j=0; j<pMenuInfo->itemCount; j++)
          {
          if (pItemEntryList[j].status & ITEMENTRY_MENUITEM)
            {
            pItemInfo = (LPITEMINFO)AObjLockData(pItemEntryList[j].id, ITEM_INFO_DATA);
            if (pItemInfo->accelKey != NULL)
              {
              accelKey = pItemInfo->accelKey;
              if ((accelKey > 0) && (accelKey < 27))
                accelKey += (char)64;     // Needs to be ASCII value
              OWndSetMenuAccelKey(winObjID, (short)pItemEntryList[j].menuItemID, 
                                  accelKey, pItemInfo->accelModifier);
              }
            AObjUnlockData(pItemEntryList[j].id, ITEM_INFO_DATA);
            }
          }
        AObjUnlockData(pMenuList[i].id, MENU_ITEMLIST_DATA);
        }
      AObjUnlockData(pMenuList[i].id, MENU_INFO_DATA);
			}
    AObjUnlockData(barObjID, BAR_MENULIST_DATA);
		}

  AObjUnlockData(barObjID, BAR_INFO_DATA);
  }
  
