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
// FILE:					MENULIB.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Menu object utility functions
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <string.h>

#include  "a_alm.h"

#include  "menuobj.h"
#include  "menulib.h"
#include  "hmenumgr.h"
#include  "o_menus.h"
#include  "itemobj.h"
#include  "itemlib.h"
#include  "o_wnd.h"
#include  "o_number.h"
#include  "o_text.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuGenerateItemID
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
short MenuGenerateItemID(
  void)
  {
  LPMENUTYPEINFO  pTypeInfo;
  short           itemID;

  pTypeInfo = (LPMENUTYPEINFO)ATypeLockData(OTYPE_MENU, MENUTYPE_INFO_DATA);

  if (0 != pTypeInfo)
    {
    pTypeInfo->idSeed++;
    itemID = (short)pTypeInfo->idSeed;
    ATypeUnlockData(OTYPE_MENU, MENUTYPE_INFO_DATA);
    }
  else
    itemID = 0;

  return itemID;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuCheckObjectIDs
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void MenuCheckObjectIDs(
  AOBJECTID  objID)
  {
	BOOL	       valid;
	int		       i;
  LPITEMENTRY  pItemEntryList;
  LPMENUINFO   pMenuInfo;

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

	if (pMenuInfo->sendText)
	  {
		valid = AObjCheckType(pMenuInfo->sendText, 0L);
		if (!valid)
			pMenuInfo->sendText = 0L;
	  }

	if (pMenuInfo->sendIndex)
		{
		valid = AObjCheckType(pMenuInfo->sendIndex, 0L);
		if (!valid)
			pMenuInfo->sendIndex = 0L;
	  }
  // check ids in object list

  if (pMenuInfo->itemCount > 0)
    {
    pItemEntryList = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);
   	for (i = 0; i < pMenuInfo->itemCount; i++)
	  {
		if (pItemEntryList[i].status & (ITEMENTRY_MENUITEM|ITEMENTRY_MENU))
      {
		  valid = AObjCheckType(pItemEntryList[i].id, 0L);
		  if (!valid)
			  {
			  pItemEntryList[i].status |= ITEMENTRY_DELETEENTRY;
		    }
      }
    }
    pMenuInfo->itemCount -=
     DeleteItemEntries(pItemEntryList, pMenuInfo->itemCount);
    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
    AObjResizeData(objID, MENU_ITEMLIST_DATA,
     pMenuInfo->itemCount*sizeof(ITEMENTRY));
	  }

  AObjUnlockData(objID, MENU_INFO_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DeleteItemEntries
//
// DESCRIPTION.: Deletes item entries marked for deletion and compacts the
//               the remaining entries.  Does not resize the data because it
//               is locked.  Returns the number of entries deleted.
//
////////////////////////////////////////////////////////////////////////////
short DeleteItemEntries(
  LPITEMENTRY pItemEntryList,
  short       currentCount)
  {
  short  deleteCount = 0;
  short  i;

  for (i=0; i<currentCount; i++)
    {
    if (deleteCount > 0)
      pItemEntryList[i-deleteCount] = pItemEntryList[i];
    if ((pItemEntryList[i].status & ITEMENTRY_DELETEENTRY) ==
                                                     ITEMENTRY_DELETEENTRY)
      deleteCount++;
    }

  return deleteCount;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuBuildMenu
//
// DESCRIPTION.: Builds a popup menu
//
////////////////////////////////////////////////////////////////////////////
HMENU MenuBuildMenu(
	AOBJECTID  objID)
	{
	LPITEMENTRY  pItemEntry;
	LPMENUINFO   pMenuInfo;
  	LPITEMINFO   pItemInfo;
	HMENU        hMenu;
	UINT         flags;
	char         title[MENU_MAX_TEXT+1];
  	char         accelTitle[ACCEL_MAX_TEXT+1];
  	LPSTR        pAccelTitle; 
	LPSTR        pTitle = 0;
	int          i;
	UINT         itemMenuID;
	short        itemStatus;


	pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

	hMenu = CreatePopupMenu();

	ADBG_ASSERT_E(hMenu!=0, return 0,
	 "MenuBuildMenu: Could not Create Popup menu");

	if (pMenuInfo->itemCount > 0)
		{
		pItemEntry = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);
		for (i = 0; i < pMenuInfo->itemCount; i++)
			{
      if (pItemEntry[i].status & ITEMENTRY_MENUITEM)
        {
        pItemInfo = (LPITEMINFO)AObjLockData(pItemEntry[i].id, ITEM_INFO_DATA);
        }
      else
        {
        pItemInfo = 0;
        }  
        
			if ((pItemEntry[i].status & ITEMENTRY_SEPARATOR) ==
					ITEMENTRY_SEPARATOR)
				{
				flags = MF_SEPARATOR;
				itemMenuID = 0;
				pTitle = 0;
				}
			else if (pItemEntry[i].status & (ITEMENTRY_MENU|ITEMENTRY_FOREIGNMENU))
				{
				flags = MF_POPUP|MF_STRING;
				if (FALSE == OMenuIsEnabled(pItemEntry[i].id))
					flags |= MF_GRAYED;
				else
					flags |= MF_ENABLED;
				itemMenuID = (UINT)MenuGetMenuHandle(pItemEntry[i].id);
        pItemEntry[i].menuItemID = itemMenuID; 
				OMenuGetTitle(pItemEntry[i].id, title, MENU_MAX_TEXT+1);
        pTitle = (LPSTR)title;
				}
			else if ((pItemEntry[i].status & ITEMENTRY_TEXTONLY) ==
							 ITEMENTRY_TEXTONLY)
				{
				flags = MF_STRING|MF_ENABLED;
				itemMenuID = pItemEntry[i].menuItemID;
				pTitle = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntry[i].id);
				}
			else if ((pItemEntry[i].status & ITEMENTRY_MENUITEM) ==
							 ITEMENTRY_MENUITEM)
				{
				flags = MF_STRING;
				itemStatus = (short)ItemGetStatus(pItemEntry[i].id);
				if (itemStatus & ITEM_DISABLED)
					flags |= MF_GRAYED;
				else
					flags |= MF_ENABLED;
				if (itemStatus & ITEM_TOGGLE_TYPE)
					{
					if (itemStatus & ITEM_TOGGLED_ON)
						{
						ItemGetToggledTitle(pItemEntry[i].id, title, MENU_MAX_TEXT+1);
						if (itemStatus & ITEM_SHOW_CHECKMARK)
							flags |= MF_CHECKED;
						}
					else
						OMenuGetTitle(pItemEntry[i].id, title, MENU_MAX_TEXT+1);
					}
				else
					OMenuGetTitle(pItemEntry[i].id, title, MENU_MAX_TEXT+1);
        pAccelTitle = GetCommandKeyText(accelTitle, pItemInfo->accelKey, pItemInfo->accelModifier);
 				pTitle = strcat(title, pAccelTitle);
				itemMenuID = pItemEntry[i].menuItemID;
				}
			else if (pItemEntry[i].status & ITEMENTRY_FOREIGN)
				{
				flags = MF_STRING;
				if (pItemEntry[i].status & ITEMENTRY_DISABLED)
					flags |= MF_GRAYED;
				else
					flags |= MF_ENABLED;
				pTitle = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntry[i].id);
				ADBG_ASSERT_E(pTitle != 0, ,
				 "MenuBuildMenu: Invalid memblock id for foreign item");

				//The memory block contains the owner object id so increment pTitle
				//past the id to the title string.

				if (pTitle != 0)
					{
					pTitle += sizeof(AOBJECTID);
					}
				itemMenuID = pItemEntry[i].menuItemID;
				}
			AppendMenu(hMenu, flags, itemMenuID, pTitle);
			if (pItemEntry[i].status & (ITEMENTRY_TEXTONLY|ITEMENTRY_FOREIGN))
				AMemUnlock((AMEMBLOCKID)pItemEntry[i].id);
 
      if (pItemInfo)    // If menu entry is an item (not a submenu), then it is locked.
        AObjUnlockData(pItemEntry[i].id, ITEM_INFO_DATA);
			}
		AObjUnlockData(objID, MENU_ITEMLIST_DATA);
		}

	AObjUnlockData(objID, MENU_INFO_DATA);

	return hMenu;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetCommandKeyText
//
// DESCRIPTION.: Generates the text for the accelerator key
//               
//
////////////////////////////////////////////////////////////////////////////
LPSTR GetCommandKeyText(
  char  buffer[ACCEL_MAX_TEXT+1],
  char  accelKey, 
  char  accelModifier)
  {
  buffer[0] = '\t';   // Tab before Command Key Text
  buffer[1] = '\0';
  
  if (accelModifier & OWND_SHIFT)
    lstrcat(buffer, "Shift+");

  if(accelModifier & OWND_CTRL)
    lstrcat(buffer, "Ctrl+");

  lstrcat(buffer, g_cmdKeyToken[accelKey]);

  return buffer;
  }
  
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuGetMenuHandle
//
// DESCRIPTION.: Creates the menu if it does not exists.  Increments use
//               count by 1.  Returns the handle to the menu.
//
////////////////////////////////////////////////////////////////////////////
HMENU MenuGetMenuHandle(
  AOBJECTID  objID)
  {
 
  HMENUMGR*  hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);

  if (hmenuMgr->GetMenuHandle() == 0)
		hmenuMgr->SetMenuHandle(MenuBuildMenu(objID));

  hmenuMgr->IncrementUseCount();

  return hmenuMgr->GetMenuHandle();

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuReturnMenuHandle
//
// DESCRIPTION.: Returns the handle to the menu.
//
////////////////////////////////////////////////////////////////////////////
HMENU MenuReturnMenuHandle(
  AOBJECTID  objID)
  {
 
  HMENUMGR*  hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);

  if (hmenuMgr->GetMenuHandle() == 0)
		return 0;

  return hmenuMgr->GetMenuHandle();

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuReleaseMenuHandle
//
// DESCRIPTION.: Decrements use count by one. Deletes menu if use count
//               goes to zero.
//
////////////////////////////////////////////////////////////////////////////
BOOL MenuReleaseMenuHandle(
  AOBJECTID      objID,
  HMENU         hMenu)
  {
  
  LPITEMENTRY   pItemEntry;
  LPMENUINFO    pMenuInfo;
  HMENU         hSubMenu;
  int           i;
  int           iPosition;
  
  HMENUMGR*  hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);
  HMENUMGR*  subMenuHmenuMgr;

  ADBG_ASSERT_E(hmenuMgr->GetMenuHandle() == hMenu, return FALSE,
   "MenuReleaseMenuHandle: menu handle does not match");

  ADBG_ASSERT_E(hmenuMgr->GetMenuHandle() != 0, return FALSE,
   "MenuReleaseMenuHandle: Menu Handle is zero.");

  if (0 == hmenuMgr->DecrementUseCount())
    {
    //iterate through menu items and remove any items that are menus
    //and call MenuReleaseMenuHandle for those menus
	  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);
  	if (pMenuInfo->itemCount > 0)
  		{
      pItemEntry = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);
      iPosition = 0;   // sub menu position pointer
  		for (i=0; i<pMenuInfo->itemCount; i++)
        {
        if (pItemEntry[i].status & (ITEMENTRY_MENU|ITEMENTRY_FOREIGNMENU))
          {
          RemoveMenu(hMenu, iPosition, MF_BYPOSITION);
          subMenuHmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(pItemEntry[i].id);
				  hSubMenu = subMenuHmenuMgr->GetMenuHandle();
          MenuReleaseMenuHandle(pItemEntry[i].id, hSubMenu);
          // Note : don't increment iPosition because all other entries are moved down
          //        by RemoveMenu.
          }
        else
          {
          // If not a submenu, then increment to the next position.
          iPosition++;
          }
        }
      AObjUnlockData(objID, MENU_ITEMLIST_DATA);
      }
    AObjUnlockData(objID, MENU_INFO_DATA);  
    DestroyMenu(hmenuMgr->GetMenuHandle());
		hmenuMgr->SetMenuHandle(0);
    }

  return TRUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuSetTitle
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL MenuSetTitle(
  AOBJECTID  menuID,
  LPSTR     pTitle)
  {

  return AObjSetData(menuID, MENU_TITLE_DATA, (LPSTR)pTitle,
   lstrlen(pTitle)+1);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuGetTitle
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
short MenuGetTitle(
  AOBJECTID  objID,
  LPSTR     buffer,
  short     bufferSize)
  {
  LPMENUINFO  pMenuInfo;
  LPSTR       pTitle;
  short       result;

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

  if ((pMenuInfo->status & MENU_NEVER_EDITED) == MENU_NEVER_EDITED)
    {
	  if (0 == AObjGetName(objID, buffer, bufferSize))
      {
      pTitle = (LPSTR)AObjLockData(objID, MENU_TITLE_DATA);
      if (pTitle != 0)
        {
        lstrcpyn(buffer, pTitle, bufferSize-1);
        buffer[bufferSize-1] = '\0';
        AObjUnlockData(objID, MENU_TITLE_DATA);
        }
      else
        buffer[0] = '\0';
      }
    }
  else
    {
    pTitle = (LPSTR)AObjLockData(objID, MENU_TITLE_DATA);
    if (pTitle != 0)
      {
      lstrcpyn(buffer, pTitle, bufferSize-1);
      buffer[bufferSize-1] = '\0';
      AObjUnlockData(objID, MENU_TITLE_DATA);
      }
    else
      buffer[0] = '\0';
    }

  result = (short)lstrlen(buffer);

  AObjUnlockData(objID, MENU_INFO_DATA);

  return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuSetDisabledFlag
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL MenuSetDisabledFlag(
  AOBJECTID  menuID)
  {
  LPMENUINFO  pMenuInfo;
  BOOL        result = TRUE;

  pMenuInfo = (LPMENUINFO)AObjLockData(menuID, MENU_INFO_DATA);

  if (pMenuInfo != 0)
    {
    pMenuInfo->status |= MENU_DISABLED;
    }
  else
    {
    ADBG_PRINT_E("MenuSetDisabledFlag: Lock of MENU_INFO_DATA failed");
    result = FALSE;
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuGetStatus
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
long MenuGetStatus(
  AOBJECTID  menuID)
  {
  LPMENUINFO  pMenuInfo;
  long        status;

  pMenuInfo = (LPMENUINFO)AObjLockData(menuID, MENU_INFO_DATA);

  status = pMenuInfo->status;

  AObjUnlockData(menuID, MENU_INFO_DATA);


  return status;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateMenuItems
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void MenuUpdateMenuItems(
  AOBJECTID  itemID,
  LPSTR     newText,
  UINT      flags)
  {
  AObjOwnerInfo ownerInfo;
  HMENUMGR*    	hmenuMgr;

  ownerInfo.key = AOBJ_GETFIRSTKEY;

  flags |= (MF_BYCOMMAND|MF_STRING);

  while (AObjGetNextOwner(OTYPE_MENU, itemID, &ownerInfo))
    {
    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(ownerInfo.ownerID);
    if (0 != hmenuMgr->GetMenuHandle())
      ModifyMenu(hmenuMgr->GetMenuHandle(), (int)ownerInfo.ownerData,
       flags, (int)ownerInfo.ownerData, newText);
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: postItemSignal
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static void postItemSignal(
  AOBJECTID  itemID,
  short     entryStatus)
  {
  long        itemStatus;
  char        title[MENU_MAX_TEXT+1];
	UINT        flags;
  char        accelText[ACCEL_MAX_TEXT+1];
  LPITEMINFO  pItemInfo;
  
    if ((entryStatus & ITEMENTRY_MENUITEM) == ITEMENTRY_MENUITEM)
    {
    itemStatus = ItemGetStatus(itemID);
    if (itemStatus & ITEM_TOGGLE_TYPE)
      {
      flags = 0;
      if (itemStatus & ITEM_TOGGLED_ON)
        {
        ItemClearToggledOnFlag(itemID);
        ItemGetTitle(itemID, title, MENU_MAX_TEXT+1);
        AEvtPostSignalAtMark(itemID, SIGNAL_ITMTOGOFF);
        }
      else
        {
        ItemSetToggledOnFlag(itemID);
        ItemGetToggledTitle(itemID, title, MENU_MAX_TEXT+1);
        if (itemStatus & ITEM_SHOW_CHECKMARK)
          flags |= MF_CHECKED;
        AEvtPostSignalAtMark(itemID, SIGNAL_ITMTOGON);
        }
      pItemInfo = (LPITEMINFO)AObjLockData(itemID, ITEM_INFO_DATA);
      GetCommandKeyText(accelText, pItemInfo->accelKey, pItemInfo->accelModifier);
      strcat(title, accelText);  
      MenuUpdateMenuItems(itemID, title, flags);
      AObjUnlockData(itemID, MENU_INFO_DATA);
      }
    AEvtPostSignalAtMark(itemID, SIGNAL_ITMCHOSEN);
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: NotifyOwner
//
// DESCRIPTION.: Helper Routine for MenuHandleCommand. Sends a message to
//               the owner object of a foreign menu item to let it know that
//               the item was selected.
//
////////////////////////////////////////////////////////////////////////////
static void NotifyOwner(
  AOBJECTID    menuID,
  LPITEMENTRY pItemEntry)
  {
  LPOBJECTID  pOwnerID;

  pOwnerID = (LPOBJECTID)AMemLock((AMEMBLOCKID)pItemEntry->id);

  if (pOwnerID != 0)
    {
    AEvtCallMethod(menuID, *pOwnerID, (ATYPEID)0, OEVENT_MENUITEMCHOSEN,
     pItemEntry->menuItemID);
    AMemUnlock((AMEMBLOCKID)pItemEntry->id);
    }
  else
    {
    ADBG_PRINT_E((LPSTR)"MenuLib - NotifyOwner(): Unsuccessful lock of memblock");
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuHandleCommand
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL MenuHandleCommand(
  short     itemID)
  {
	AOBJECTID     menuObjID;
	AOBJECTID     itemObjID;
  LONG         getKey = AOBJ_GETFIRSTKEY;
	BOOL	       result = FALSE;

  while (0 != (menuObjID = AObjGetNextObject(OTYPE_MENU, &getKey)))
    {
    if (NULL != (itemObjID =MenuSearchEntryList(menuObjID, itemID)))
      {
      MenuSendText(menuObjID, itemObjID);
      AEvtPostSignalAtMark(menuObjID, SIGNAL_ITMCHOSEN);
      result = TRUE;
      break;
      }
    }

  return  result;

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuSearchEntryList
//
// DESCRIPTION.:  Searches all the menuitems in an Item Entry List looking 
//                for a match to itemID.
//
//  INPUTS.....:  objID  - The object ID of the menu object.
//                itemID - The item ID of the menu item we're searching for.
//
//                Returns the Object ID of the menu item.
//
////////////////////////////////////////////////////////////////////////////
AOBJECTID MenuSearchEntryList(
	AOBJECTID    objID,
  short        itemID)
  {
	AOBJECTID    result = NULL;
	int		       i;
  LPITEMENTRY  pItemEntryList;
  LPMENUINFO   pMenuInfo;
  LPITEMINFO   pItemInfo;
  
  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

  if (pMenuInfo->itemCount > 0)
    {
    pItemEntryList = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);
   	for (i = 0; i < pMenuInfo->itemCount; i++)
      {
      if (pItemEntryList[i].status & ITEMENTRY_MENUITEM)
       {
		    if (pItemEntryList[i].menuItemID == itemID)
          {
          // If item is disabled, don't post message. (Only happens with accelerator keys)
          pItemInfo = (LPITEMINFO)AObjLockData(pItemEntryList[i].id, ITEM_INFO_DATA);
          if (0 == (pItemInfo->status & ITEM_DISABLED))
            {
            postItemSignal(pItemEntryList[i].id, pItemEntryList[i].status);
            result = pItemEntryList[i].id;
            if (pMenuInfo->status & MENU_POPUP)
              pMenuInfo->popIndex = WORD(i);
            }
          AObjUnlockData(pItemEntryList[i].id, ITEM_INFO_DATA);
          break;  
          }
        }    
      else if (pItemEntryList[i].status & ITEMENTRY_TEXTONLY)
        {
		    if (pItemEntryList[i].menuItemID == itemID)
          {
          postItemSignal(pItemEntryList[i].id, pItemEntryList[i].status);
          result = pItemEntryList[i].id;
          if (pMenuInfo->status & MENU_POPUP)
            pMenuInfo->popIndex = WORD(i);
          break;
          }    
        }
      else if (pItemEntryList[i].status & ITEMENTRY_FOREIGN)   // Entry is a foreign item
        {
  	    if (pItemEntryList[i].menuItemID == itemID)
          {
          postItemSignal(pItemEntryList[i].id, pItemEntryList[i].status);
          NotifyOwner(objID, &pItemEntryList[i]);
          result = pItemEntryList[i].id;
          if (pMenuInfo->status & MENU_POPUP)
            pMenuInfo->popIndex = WORD(i);
          break;     
          }
        }                 
      else if (pItemEntryList[i].status & ITEMENTRY_FOREIGNMENU)    // Entry is a foreign menu, so search through its list
        {
        if (NULL != (result = MenuSearchEntryList(pItemEntryList[i].id, itemID)))
          break;
        }   
	    }
    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
    }
  AObjUnlockData(objID, MENU_INFO_DATA);

  return  result;    

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuAddEntryToItemList
//
// DESCRIPTION.:  Adds an uninitialized entry to the item list and returns a
//                pointer to the item.
//
// SIDE-EFFECTS:  Leaves a lock on the MENU_ITEMLIST_DATA. This lock must
//                be removed by the caller.
//
////////////////////////////////////////////////////////////////////////////
static LPITEMENTRY MenuAddEntryToItemList(
  AOBJECTID  menuID,
  short     index)
  {
  LPMENUINFO   pMenuInfo;
  LPITEMENTRY  pItemEntry;
  short        i;

  pMenuInfo = (LPMENUINFO)AObjLockData(menuID, MENU_INFO_DATA);

  pMenuInfo->itemCount++;

  AObjResizeData(menuID, MENU_ITEMLIST_DATA,
   pMenuInfo->itemCount * sizeof(ITEMENTRY));

	pItemEntry = (LPITEMENTRY)AObjLockData(menuID, MENU_ITEMLIST_DATA);

  //shift entries down one if need be
  for (i = (short)(pMenuInfo->itemCount-1); i > index; i--)
    pItemEntry[i] = pItemEntry[i-1];

  AObjUnlockData(menuID, MENU_INFO_DATA);

  return (pItemEntry + index);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuAddObjectItem
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void MenuAddObjectItem(
  AOBJECTID  menuObject,
  AOBJECTID  itemObject,
  short     index)
  {
  LPITEMENTRY  pItemEntry;
  LPITEMINFO   pItemInfo;
  HMENUMGR*    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(menuObject);
  char         title[MENU_MAX_TEXT+1];
  char         accelTitle[ACCEL_MAX_TEXT+1];
  LPSTR        pAccelTitle;
  LPSTR        pTitle; 
  long         itemStatus;
  UINT         flags;

  pItemEntry = MenuAddEntryToItemList(menuObject, index);
  pItemInfo = (LPITEMINFO)AObjLockData(itemObject, ITEM_INFO_DATA);
  
  pItemEntry->status = ITEMENTRY_MENUITEM;
  pItemEntry->id = itemObject;
  
  pItemEntry->menuItemID = pItemInfo->itemID = MenuGenerateItemID();

  if (hmenuMgr->GetMenuHandle() != 0)
    {
    flags = MF_STRING|MF_BYPOSITION;
    itemStatus = ItemGetStatus(itemObject);
    if (itemStatus & ITEM_DISABLED)
      flags |= MF_GRAYED;
    else
      flags |= MF_ENABLED;
    if (itemStatus & ITEM_TOGGLE_TYPE)
      {
      if (itemStatus & ITEM_TOGGLED_ON)
        {
        ItemGetToggledTitle(itemObject, title, MENU_MAX_TEXT+1);
        if (itemStatus & ITEM_SHOW_CHECKMARK)
          flags |= MF_CHECKED;
        }
      else
        OMenuGetTitle(itemObject, title, MENU_MAX_TEXT+1);
      }
    else
      OMenuGetTitle(itemObject, title, MENU_MAX_TEXT+1);
      
    // Now append command key text to menu title
    pAccelTitle = GetCommandKeyText(accelTitle, pItemInfo->accelKey, pItemInfo->accelModifier);
  	pTitle = strcat(title, pAccelTitle);

    InsertMenu(hmenuMgr->GetMenuHandle(), index, flags, pItemEntry->menuItemID,
     pTitle);
    }

  AObjUnlockData(itemObject, ITEM_INFO_DATA);
  AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);  //locked by MenuAddEntryToItemList

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuExtractTitle
//
// DESCRIPTION.: Extracts the first title from a null terminated string.
//               \r\n is the delimiter. NULL is returned if this string is 
//               the last title in the string, otherwise the address of the  
//               next title (inputBuffer) is returned. 
//
////////////////////////////////////////////////////////////////////////////
LPSTR MenuExtractTitle(
	LPSTR     inputBuffer,
	LPSTR     outputBuffer)
	{
  short   bufferLength = (short)lstrlen(inputBuffer);
  short   titleLength;
  LPSTR   ptr;

  strnset(outputBuffer, '\0', MENU_MAX_TEXT+1);  // Make sure title is NULL terminated
  if (bufferLength != 0)
    {
    if (NULL ==(ptr = strstr(inputBuffer, "\r\n")))
      {
      lstrcpy(outputBuffer, inputBuffer);
      return(NULL);
      }
    else
      {
      titleLength = (short)(ptr - inputBuffer);
      lstrcpyn(outputBuffer, inputBuffer, titleLength);
      outputBuffer[titleLength] = '\0'; // Make sure string is null terminated.
      if ((titleLength + 2) >= bufferLength)
        return(NULL);   // CR/LF terminates last string.
      else
        return(inputBuffer + titleLength + 2);  // Point to the beginning of the next title.
      }
    }
  else
    {
    lstrcpy(outputBuffer, "");
    return(NULL);
    }
  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuAddTextItem
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void MenuAddTextItem(
	AOBJECTID     menuObject,
	AMEMBLOCKID   textBlock,
	short        index)
	{
  AMEMBLOCKID   bufferBlock;
	LPITEMENTRY  pItemEntry;
	HMENUMGR*    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(menuObject);
  char         title[MENU_MAX_TEXT+1];
	LPSTR        pTitle = title;
  LPSTR        pBuffer;
  LPSTR        pTextBuffer;
  LPSTR        pThisTitle;
  LPSTR        pNextTitle;

  pBuffer = (LPSTR)AMemLock(textBlock);

  if (NULL != pBuffer)
    pThisTitle = pBuffer;
  else
    pThisTitle = NULL;
           
  while (NULL != pThisTitle)
    {
    pNextTitle = MenuExtractTitle(pThisTitle, title);

  	pItemEntry = MenuAddEntryToItemList(menuObject, index);

    if (lstrlen(pTitle) == 0)
      {
    	pItemEntry->status = ITEMENTRY_SEPARATOR;
  		InsertMenu(hmenuMgr->GetMenuHandle(), index, MF_SEPARATOR|MF_BYPOSITION,
  		           pItemEntry->menuItemID, (LPSTR)pTitle);
    	AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);  //locked by MenuAddEntryToItemList
//  	  pItemEntry = MenuAddEntryToItemList(menuObject, index);
      }

    else
      {
    	pItemEntry->status = ITEMENTRY_TEXTONLY;

    	bufferBlock = AMemAllocate(lstrlen(pTitle)+1);
      pTextBuffer = (LPSTR)AMemLock(bufferBlock);
      lstrcpy(pTextBuffer, pTitle);
    	pItemEntry->id = (AOBJECTID)bufferBlock;
      AMemUnlock(bufferBlock);

    	pItemEntry->menuItemID = MenuGenerateItemID();

    	if (hmenuMgr->GetMenuHandle() != 0)
    		InsertMenu(hmenuMgr->GetMenuHandle(), index, MF_STRING|MF_BYPOSITION,
    		           pItemEntry->menuItemID, (LPSTR)pTitle);

    	AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);  //locked by MenuAddEntryToItemList
      }
    pThisTitle = pNextTitle;
    index ++;
    }

  if (pBuffer != 0)
  	AMemUnlock(textBlock);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuAddForeignItem
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void MenuAddForeignItem(
  AOBJECTID    ownerObject,
  AOBJECTID    menuObject,
  LPITEMSPEC  pItemSpec,
  short       index)
  {
  LPITEMENTRY  pItemEntry;
  HMENUMGR*    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(menuObject);
  LPSTR        pBuffer;
  UINT         flags;

	pItemEntry = MenuAddEntryToItemList(menuObject, index);

	pItemEntry->status = ITEMENTRY_FOREIGN;
	if ((pItemSpec->status & OMENU_DISABLED) == OMENU_DISABLED)
		pItemEntry->status |= ITEMENTRY_DISABLED;

	//allocate memory block large enough to hold the owner object id followed
	//by the text for the menu item.

	pItemEntry->id = (AOBJECTID)AMemAllocate(
	 sizeof(AOBJECTID) + lstrlen(pItemSpec->itemText) + sizeof(char));

	pBuffer = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntry->id);

	if (pBuffer != 0)
		{
		*(LPOBJECTID)pBuffer = ownerObject;
		pBuffer += sizeof(AOBJECTID);
		lstrcpy(pBuffer, pItemSpec->itemText);
		AMemUnlock((AMEMBLOCKID)pItemEntry->id);
		}
	else
		{														                                             
		ADBG_PRINT_E((LPSTR)"MenuAddForeignItem: Unsuccessful lock of memblock");
		}

	pItemEntry->menuItemID = pItemSpec->itemID = MenuGenerateItemID();

	if (hmenuMgr->GetMenuHandle() != 0)
		{
		flags = MF_STRING|MF_BYPOSITION;
    if ((pItemEntry->status & ITEMENTRY_DISABLED) == ITEMENTRY_DISABLED)
      {
      flags |= MF_GRAYED;
      }
    InsertMenu(hmenuMgr->GetMenuHandle(), index, flags,
     pItemEntry->menuItemID, pBuffer);
    }

  AObjUnlockData(menuObject, MENU_ITEMLIST_DATA); //locked by MenuAddEntry to Item List

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuAddSubMenu
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
AOBJECTID MenuAddSubMenu(
  AOBJECTID    menuObject,
  LPITEMSPEC  pItemSpec,
  short       index)
  {
  LPITEMENTRY  pItemEntry;
  HMENUMGR*    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(menuObject);
  AOBJECTID     newMenuObject;
  HMENU        hSubMenu;
  UINT         flags;

  pItemEntry = MenuAddEntryToItemList(menuObject, index);

  pItemEntry->status = ITEMENTRY_FOREIGNMENU;

  newMenuObject = pItemEntry->id = AObjCreate(OTYPE_MENU);

  if ((pItemSpec->status & OMENU_DISABLED) == OMENU_DISABLED)
    MenuSetDisabledFlag(newMenuObject);

  MenuSetTitle(newMenuObject, pItemSpec->itemText);

  if (hmenuMgr->GetMenuHandle() != 0)
    {
    HMENUMGR*  newHmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(newMenuObject);
    hSubMenu = CreatePopupMenu();
    newHmenuMgr->SetMenuHandle(hSubMenu);
    newHmenuMgr->IncrementUseCount();
    pItemEntry->menuItemID = (UINT)hSubMenu;
    flags = MF_POPUP|MF_STRING|MF_BYPOSITION;
    if (FALSE == OMenuIsEnabled(pItemEntry->id))
      flags |= MF_GRAYED;
    else
      flags |= MF_ENABLED;
    InsertMenu(hmenuMgr->GetMenuHandle(), index, flags, (UINT)hSubMenu,
     pItemSpec->itemText);
    }
  else
    {
    pItemEntry->menuItemID = 0;
    }

  AObjUnlockData(menuObject, MENU_ITEMLIST_DATA); //locked by MenuAddEntry to Item List

  return newMenuObject;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuDeleteItem
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void MenuDeleteItem(
  AOBJECTID  menuObject,
  short     index)
  {
  LPMENUINFO  pMenuInfo;
  LPITEMENTRY pItemEntry;
  HMENUMGR*   hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(menuObject);
  int         i;

  pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);

  if ((pMenuInfo->itemCount-1 < index) || (index < 0))
    ADBG_PRINT_E("MenuDeleteItem: index out of range.");
  else
    {
    pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
    if (0 != pItemEntry)
      {
      if (pItemEntry[index].status & ITEMENTRY_TEXTONLY)
        AMemFree((AMEMBLOCKID)pItemEntry[index].id);
      else if (pItemEntry[index].status & ITEMENTRY_MENU)
        MenuReleaseMenuHandle(pItemEntry[index].id, 0);
      for (i=index+1; i<pMenuInfo->itemCount; i++)
        pItemEntry[i-1] = pItemEntry[i];
      AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
      pMenuInfo->itemCount--;
      AObjResizeData(menuObject, MENU_ITEMLIST_DATA,
       pMenuInfo->itemCount * sizeof(ITEMENTRY));
      if (hmenuMgr->GetMenuHandle() != 0)
        RemoveMenu(hmenuMgr->GetMenuHandle(), index, MF_BYPOSITION);
      }

    }

  AObjUnlockData(menuObject, MENU_INFO_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuSetItemTitle
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void MenuSetItemTitle(
  AOBJECTID  menuObject,
  LPSTR     pTitle,
  short     itemIndex)
  {
  LPMENUINFO   pMenuInfo;
  LPITEMENTRY  pItemEntry;
  LPSTR        pMemBlock;
  HMENUMGR*    hmenuMgr;
  UINT         flags = MF_BYPOSITION;

  pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
  itemIndex --;

  if ((pMenuInfo->itemCount-1 < itemIndex) || (itemIndex < 0))
    ADBG_PRINT_E("MenuSetItemTitle: index out of range.");
  else
    {
    pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
    if (0 != pItemEntry)
      {
      if (pItemEntry[itemIndex].status & ITEMENTRY_MENUITEM )
        {
        ItemSetTitle(pItemEntry[itemIndex].id, pTitle);
        AEvtPostStandard(pItemEntry[itemIndex].id, AEVENT_OBJECTCHANGED, 0,
                        AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
        }
      else if (pItemEntry[itemIndex].status & ITEMENTRY_MENU)
        {
        MenuSetTitle(pItemEntry[itemIndex].id, pTitle);
        AEvtPostStandard(pItemEntry[itemIndex].id, AEVENT_OBJECTCHANGED, 0,
                        AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
        }
      else if (pItemEntry[itemIndex].status & ITEMENTRY_TEXTONLY)
        {
        if (AMemSetSize((AMEMBLOCKID)pItemEntry[itemIndex].id, lstrlen(pTitle)+1))
          { 
          pMemBlock = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntry[itemIndex].id);
          lstrcpy(pMemBlock, pTitle);
          AMemUnlock((AMEMBLOCKID)pItemEntry[itemIndex].id);
          hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(menuObject);
          ModifyMenu(hmenuMgr->GetMenuHandle(), (UINT)itemIndex, flags, (UINT)itemIndex, pTitle);
          }
        }
      AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
      }
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuDeleteForeignItem
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL MenuDeleteForeignItem(
  AOBJECTID    menuObject,
  LPITEMSPEC  pItemSpec)
  {
  LPITEMENTRY  pItemEntry;
  LPMENUINFO   pMenuInfo;
  int          i;
  HMENUMGR*    hParentMenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(menuObject);
  BOOL         result = FALSE;
  BOOL         bTemp;


  pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);

  ADBG_ASSERT_E(pMenuInfo!=0, return FALSE,
   "MenuDeleteForeignItem: Could not lock MENU_INFO_DATA");

  if (pMenuInfo->itemCount > 0)
		{
    pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
    ADBG_ASSERT_E(pItemEntry!=0, return FALSE,
     "MenuDeletForeignItem: Could not lock MENU_ITEMLIST_DATA");
		for (i = 0; i < pMenuInfo->itemCount; i++)
		  {
			if ((pItemEntry[i].status & ITEMENTRY_FOREIGN) &&
          (pItemEntry[i].menuItemID == pItemSpec->itemID))
				{
        pItemEntry[i].status |= ITEMENTRY_DELETEENTRY;
        result = TRUE;
        if (hParentMenuMgr->GetMenuHandle() != 0)
          {
          bTemp = DeleteMenu(hParentMenuMgr->GetMenuHandle(), i, MF_BYPOSITION);
          ADBG_ASSERT_E(bTemp == TRUE, ,
           "MenuDeletForeignItem: Call to DeleteMenu() failed.");
          }
        break;
				}
      }
    pMenuInfo->itemCount -=
     DeleteItemEntries(pItemEntry, pMenuInfo->itemCount);
    AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
    AObjResizeData(menuObject, MENU_ITEMLIST_DATA,
     pMenuInfo->itemCount*sizeof(ITEMENTRY));
		}

  AObjUnlockData(menuObject, MENU_INFO_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuDeleteSubMenu
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL MenuDeleteSubMenu(
  AOBJECTID    parentMenu,
  AOBJECTID    subMenu)
  {
  LPITEMENTRY  pItemEntry;
  LPMENUINFO   pMenuInfo;
  HMENU        hSubMenu;
  int          i;
  HMENUMGR*    hParentMenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(parentMenu);
  BOOL         result = FALSE;

  pMenuInfo = (LPMENUINFO)AObjLockData(parentMenu, MENU_INFO_DATA);

  ADBG_ASSERT_E(pMenuInfo!=0, return FALSE,
   "MenuDeletSubMenu: Could not lock MENU_INFO_DATA");


  if (pMenuInfo->itemCount > 0)
		{
    pItemEntry = (LPITEMENTRY)AObjLockData(parentMenu, MENU_ITEMLIST_DATA);
    ADBG_ASSERT_E(pItemEntry!=0, return FALSE,
     "MenuDeletSubMenu: Could not lock MENU_ITEMLIST_DATA");
		for (i = 0; i < pMenuInfo->itemCount; i++)
		  {
			if ((pItemEntry[i].status & ITEMENTRY_FOREIGNMENU) &&
          (pItemEntry[i].id == subMenu))
				{
        pItemEntry[i].status |= ITEMENTRY_DELETEENTRY;
        result = TRUE;
        if (hParentMenuMgr->GetMenuHandle() != 0)
          {
          hSubMenu = GetSubMenu(hParentMenuMgr->GetMenuHandle(), i);
          RemoveMenu(hParentMenuMgr->GetMenuHandle(), i, MF_BYPOSITION);
          MenuReleaseMenuHandle(subMenu, hSubMenu);
          }
				}
      }
    pMenuInfo->itemCount -=
     DeleteItemEntries(pItemEntry, pMenuInfo->itemCount);
    AObjUnlockData(parentMenu, MENU_ITEMLIST_DATA);
    AObjResizeData(parentMenu, MENU_ITEMLIST_DATA,
     pMenuInfo->itemCount*sizeof(ITEMENTRY));
		}

  AObjUnlockData(parentMenu, MENU_INFO_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MenuSendText
//
// DESCRIPTION.:  Sends the text 
//                for a match to itemID.
//
//  INPUTS.....:  objID  - The object ID of the menu object.
//                itemID - The item ID of the menu item selected.
//
////////////////////////////////////////////////////////////////////////////
BOOL MenuSendText(
  AOBJECTID    menuObjID,
  AOBJECTID    itemObjID)
  {
	BOOL	      result = FALSE;
  char        buffer[MENU_MAX_TEXT + 1];
	int		      i;
  AOBJECTID    textID; 
  AOBJECTID    numberID; 
  LPMENUINFO  pMenuInfo;
  LPITEMENTRY pItemEntryList;
  LPSTR       pText;

  pMenuInfo = (LPMENUINFO)AObjLockData(menuObjID, MENU_INFO_DATA);

  numberID = pMenuInfo->sendIndex;
  textID = pMenuInfo->sendText;
  
  if ((NULL == numberID ) && (NULL == textID))
    {
    // Don't need to send Index or Text, so return
    AObjUnlockData(menuObjID, MENU_INFO_DATA);
    return result;
    }

  // Search EntryList for proper menu item
  pItemEntryList = (LPITEMENTRY)AObjLockData(menuObjID, MENU_ITEMLIST_DATA);
 	for (i = 0; i < pMenuInfo->itemCount; i++)
    {
    if (pItemEntryList[i].status & (ITEMENTRY_TEXTONLY|ITEMENTRY_MENUITEM))
      {
      if (pItemEntryList[i].id == itemObjID)
      break;
      }
    }
    
  // i = index of menu item  

  if (NULL != numberID)   // Send Index ?
    {
    // Send index (i) to numberID
    ONmbrSetInteger(numberID, (long)(i+1), TRUE);
    }
  
  if (NULL != textID)    // Send Number ?
    {
    if ((pItemEntryList[i].status & ITEMENTRY_TEXTONLY) ==
        ITEMENTRY_TEXTONLY)
      {
      pText = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntryList[i].id);
      if (pText != 0)
        {
        lstrcpyn(buffer, pText, MENU_MAX_TEXT+1);
        buffer[MENU_MAX_TEXT] = '\0';
        AMemUnlock((AMEMBLOCKID)pItemEntryList[i].id);
        }
      else
        buffer[0] = '\0';
      }
    else
	    ItemGetTitle(itemObjID, buffer, MENU_MAX_TEXT+1);
    // Send item name (buffer) to textID
    OTextSetValueFromBuffer(textID, buffer);
    }   

  if (pItemEntryList)
    AObjUnlockData(menuObjID, MENU_ITEMLIST_DATA);

  AObjUnlockData(menuObjID, MENU_INFO_DATA);
  
  return result;
  
  }
