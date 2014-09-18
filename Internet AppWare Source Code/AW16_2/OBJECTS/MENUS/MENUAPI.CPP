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
// FILE:				  MENUAPI.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		External api functions
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <string.h>

#include "a_alm.h"
#include "menuobj.h"
#include "o_menus.h"
#include "barlib.h"
#include "itemlib.h"
#include "menulib.h"
#include "barlib.h"
#include "hmenumgr.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuGetTitle
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" short ALMAPI OMenuGetTitle(
  OBJECTID  objID,
  LPSTR     buffer,
  short     bufferSize)
  {
  TYPEID  objType;
  short   result = 0;

  objType = AObjGetType(objID);

  if (OTYPE_MENU == objType)
    result = MenuGetTitle(objID, buffer, bufferSize);
  else if (OTYPE_MENUITEM == objType)
    result = ItemGetTitle(objID, buffer, bufferSize);
  else
    {
    ADBG_EVAL(
      ADBG_PRINT_E("OMenuGetTitle: Object type not Menu or Menu Item");
      );
    result = -1;
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuIsPopup
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" BOOL ALMAPI OMenuIsPopup(
  OBJECTID  objID)
  {
  LPMENUINFO  pMenuInfo;
  BOOL        result;

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

  if ((pMenuInfo->status&MENU_POPUP) == MENU_POPUP)
    result = TRUE;
  else
    result = FALSE;

  AObjUnlockData(objID, MENU_INFO_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuIsEnabled
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" BOOL ALMAPI OMenuIsEnabled(
  OBJECTID objID)
  {
  LPMENUINFO  pMenuInfo;
  BOOL        result;

  ADBG_ASSERT_E(AObjGetType(objID)==OTYPE_MENU, return FALSE,
    "Fatal Exit: OMenuIsEnabled, Object is not a Menu.");

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

  if ((pMenuInfo->status&MENU_DISABLED) == MENU_DISABLED)
    result = FALSE;
  else
    result = TRUE;

  AObjUnlockData(objID, MENU_INFO_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuAddItems
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" BOOL ALMAPI OMenuAddItems(
  OBJECTID    ownerID,
  OBJECTID    menuID,
  MEMBLOCKID  itemList,
  short       position)
  {
  LPITEMSPEC  pItemSpec;
  short       count;
  short       i;
  BOOL        result = TRUE;

  ADBG_ASSERT_E(AObjGetType(menuID)==OTYPE_MENU, return FALSE,
   (LPSTR)"OMenuAddItems: Menu Object ID is of wrong type or invalid.");

  pItemSpec = (LPITEMSPEC)AMemLock(itemList);

  if (NULL != pItemSpec)
    {
    ADBG_ASSERT_E(AMemGetSize(itemList)%sizeof(ITEMSPEC)==0, return FALSE,
      (LPSTR)"OMenuAddItems: Memory Block for item list is invalid size.");
    count = AMemGetSize(itemList)/sizeof(ITEMSPEC);
    for (i=0; i<count; i++)
      {
      MenuAddForeignItem(ownerID, menuID, pItemSpec+i, position+i);
      }
    AMemUnlock(itemList);
    }
  else
    {
    result = FALSE;
    ADBG_PRINT_W((LPSTR)"OMenuAddItems: Lock of itemList failed");
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuAddSubmenu
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" OBJECTID ALMAPI OMenuAddSubmenu(
  OBJECTID    ownerID,
  OBJECTID    menuID,
  MEMBLOCKID  itemList,
  short       position)
  {
  LPITEMSPEC  pItemSpec;
  short       count;
  short       i;
  OBJECTID    subMenuID = 0;

  ADBG_ASSERT_E(AObjGetType(menuID)==OTYPE_MENU, return FALSE,
   (LPSTR)"OMenuAddSubmenu: Menu Object ID is of wrong type or invalid.");

  pItemSpec = (LPITEMSPEC)AMemLock(itemList);

  if (NULL != pItemSpec)
    {
    ADBG_ASSERT_E(AMemGetSize(itemList)%sizeof(ITEMSPEC)==0, return FALSE,
      (LPSTR)"OMenuAddSubmenu: Memory Block for item list is invalid size.");
    subMenuID = MenuAddSubMenu(menuID, pItemSpec, position);
    count = AMemGetSize(itemList)/sizeof(ITEMSPEC);
    for (i=1; i<count; i++)
      {
      MenuAddForeignItem(ownerID, subMenuID, pItemSpec+i, i-1);
      }
    AMemUnlock(itemList);
    }
  else
    {
    ADBG_PRINT_W((LPSTR)"OMenuAddSubmenu: Lock of itemList failed");
    }

  return subMenuID;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuDeleteSubmenu
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" BOOL ALMAPI OMenuDeleteSubmenu(
	OBJECTID  menuID,    
  OBJECTID  submenuID)
  {
  BOOL  result;

  ADBG_ASSERT_E(AObjGetType(menuID)==OTYPE_MENU, return FALSE,
   (LPSTR)"OMenuDeleteSubmenu: Menu Object ID is of wrong type or invalid.");

  ADBG_ASSERT_E(AObjGetType(submenuID)==OTYPE_MENU, return FALSE,
   (LPSTR)"OMenuDeleteSubmenu: Submenu Object ID is of wrong type or invalid.");

  result = MenuDeleteSubMenu(menuID, submenuID);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuDeleteItems
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" BOOL ALMAPI OMenuDeleteItems(
  OBJECTID    menuID,
  MEMBLOCKID  itemList)
  {
  LPITEMSPEC  pItemSpec;
  short       count;
  short       i;
  BOOL        result = TRUE;

  ADBG_ASSERT_E(AObjGetType(menuID)==OTYPE_MENU, return FALSE,
   (LPSTR)"OMenuDeleteItems: Menu Object ID is of wrong type or invalid.");

  pItemSpec = (LPITEMSPEC)AMemLock(itemList);

  if (NULL != pItemSpec)
    {
    ADBG_ASSERT_E(AMemGetSize(itemList)%sizeof(ITEMSPEC)==0, return FALSE,
      (LPSTR)"OMenuDeleteItems: Memory Block for item list is invalid size.");
    count = AMemGetSize(itemList)/sizeof(ITEMSPEC);
    for (i=0; i<count; i++)
      {
      MenuDeleteForeignItem(menuID, pItemSpec+i);
      }
    AMemUnlock(itemList);
    }
  else
    {
    result = FALSE;
    ADBG_PRINT_W((LPSTR)"OMenuDeleteItems: Lock of itemList failed");
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OMenuDeletePosition
//
// DESCRIPTION.: This API deletes a specified number of foreign items 
//               beginning at the specified position, whether it is a menu
//               item or a submenu. It returns the number of items
//               actually deleted.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" short ALMAPI OMenuDeletePosition(
  OBJECTID    menuID,
  short       position,
  short       count)
  {
  OBJECTID    submenuID;
  LPMENUINFO  pMenuInfo;
//  LPITEMSPEC  pItemSpec;
  LPITEMENTRY pItemEntry;
//  BOOL        result = FALSE;
  BOOL        bTemp;
//  MEMBLOCKID  itemList;
  short       itemsDeleted = 0;
  int         i;
  HMENU       hSubMenu;
  HMENUMGR    hParentMenuMgr(AObjGetRuntimeInfo(menuID));

  pMenuInfo = (LPMENUINFO)AObjLockData(menuID, MENU_INFO_DATA);
  
  if ((pMenuInfo->itemCount-1 < position) || (position < 0))
    {
    ADBG_PRINT_E("OMenuDeletePosition: Index out of range.");
    AObjUnlockData(menuID, MENU_INFO_DATA);
    return FALSE;
    }
    
  if (pMenuInfo->itemCount < (position + count))
    count = (pMenuInfo->itemCount - position);   // don't delete more items than exist.
  
  pItemEntry = (LPITEMENTRY)AObjLockData(menuID, MENU_ITEMLIST_DATA);
  
  for (i = position; i < (count + position); i++)
    {
    if (pItemEntry[i].status & ITEMENTRY_FOREIGN)  // Foreign item
      {
      if(hParentMenuMgr.GetMenuHandle() != 0)
        {
        bTemp = DeleteMenu(hParentMenuMgr.GetMenuHandle(), i, MF_BYPOSITION);
        ADBG_ASSERT_E(bTemp == TRUE, , "OMenuDeletePosition: Call to DeleteMenu() failed.");
        }
      pItemEntry[i].status |= ITEMENTRY_DELETEENTRY;
      itemsDeleted++;
      }

    else if (pItemEntry[position].status & ITEMENTRY_FOREIGNMENU)  // Foreign Submenu
      {
      submenuID = pItemEntry[i].id;
      if(hParentMenuMgr.GetMenuHandle() != 0)
        {
        hSubMenu = GetSubMenu(hParentMenuMgr.GetMenuHandle(), i);
        RemoveMenu(hParentMenuMgr.GetMenuHandle(), i, MF_BYPOSITION);
        MenuReleaseMenuHandle(submenuID, hSubMenu);
        AObjDestroy(submenuID);
        }
      pItemEntry[i].status |= ITEMENTRY_DELETEENTRY;
      itemsDeleted++;
      }    
    }
    
  pMenuInfo->itemCount -= DeleteItemEntries(pItemEntry, pMenuInfo->itemCount);
    
  AObjUnlockData(menuID, MENU_ITEMLIST_DATA);
  AObjResizeData(menuID, MENU_ITEMLIST_DATA, pMenuInfo->itemCount * sizeof(ITEMENTRY));
  AObjUnlockData(menuID, MENU_INFO_DATA);
    
  return itemsDeleted;    

  }
  
