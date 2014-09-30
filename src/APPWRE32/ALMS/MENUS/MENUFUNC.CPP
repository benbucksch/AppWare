/*//////////////////////////////////////////////////////////////
//
//          Copyright 1993, Novell, Inc.  All rights reserved
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
//--------------------------------------------------------------
//
// FILE:          MENUFUNC.CPP
//
// AUTHOR:        Brian Brown
//
// DESCRIPTION:   Menu function implementations
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include  <windows.h>
#include  <a_alm.h>
#include  <intdebug.h>
#include  <string.h>
#include  <o_number.h>
#include  <o_text.h>
#include  <o_menus.h>
#include  <o_wnd.h>
#include  <o_wndlay.h>

#include  "menuobj.h"
#include  "itemobj.h"
#include  "itemlib.h"
#include  "menulib.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetItemTextIdx
//
// DESCRIPTION.:  get item text by menu object and index.
//
// RETURNS:       true if index is in range, else false if out of range
//
/////////////////////////////////////////////////////////////////////////////
static BOOL GetItemTextIdx(
  AOBJECTID  menuObject,
  AOBJECTID  numberObject,
  LPSTR     buffer,
  short     bufferSize)
  {
  short        itemIndex;
  LPMENUINFO   pMenuInfo;
  LPITEMENTRY  pItemEntry;
  LPSTR        pText;
  BOOL         indexGood = TRUE;

  itemIndex = (short)ONmbrGetInteger(numberObject);

  pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);

  if ((itemIndex<=pMenuInfo->itemCount) && (itemIndex > 0))
    {
    itemIndex--; //convert to zero based index
    pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
    if ((pItemEntry[itemIndex].status & ITEMENTRY_TEXTONLY) ==
        ITEMENTRY_TEXTONLY)
      {
      pText = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntry[itemIndex].id);
      if (pText != 0)
        {
        lstrcpyn(buffer, pText, bufferSize);
        buffer[bufferSize-1] = '\0';
        AMemUnlock((AMEMBLOCKID)pItemEntry[itemIndex].id);
        }
      else
        buffer[0] = '\0';
      }
    else
      ItemGetTitle(pItemEntry[itemIndex].id, buffer, bufferSize);
    AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
    }
  else
    {
    indexGood = FALSE;
    buffer[0] = '\0';
    }
    

  AObjUnlockData(menuObject, MENU_INFO_DATA);

  return indexGood;
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuCountItems
//
// DESCRIPTION.: param 1 - Menu object, required.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuCountItems(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  WORD          count;
  AOBJECTID      menuObject;
  AOBJECTID      numberObject;
  LPMENUINFO    pMenuInfo;

  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1, OTYPE_MENU);

  if (menuObject == NULL)
    {
    ADBG_PRINT_E("Count Menu Items: could not get first parameter");
    AFuncReportResult(sys, 1, "");
    }
  else
    {
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
    count = pMenuInfo->itemCount;
    AObjUnlockData(menuObject, MENU_INFO_DATA);
    numberObject = AObjCreate(OTYPE_NUMBER);
    if (numberObject == NULL)
      {
      ADBG_PRINT_E("Count Menu Items: Cannot create number object for output");
      AFuncReportResult(sys, 1, "");
      }
    else
      {
      ONmbrSetInteger(numberObject, count, TRUE);
      AFuncSetParameter(1, numberObject);
      AFuncReportResult(sys, 1, "");
      }
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuDisable
//
// DESCRIPTION.: param 1 - Menu object, required.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuDisable(
  pAEvtInfo     evt,
  pAObjMessage  sys )
  {
  LPMENUINFO  pMenuInfo;
  AOBJECTID    menuObject;

  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1, OTYPE_MENU);

  if (menuObject == NULL)
    {
    ADBG_PRINT_E("Disable Menu: could not get first parameter");
    }
  else
    {
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
    pMenuInfo->status |= MENU_DISABLED;
    AObjUnlockData(menuObject, MENU_INFO_DATA);
    AEvtPostStandard(menuObject, AEVENT_OBJECTCHANGED, 0,
     AEVT_ATTAIL|AEVT_KILLDUPLICATES);
    }

  AFuncReportResult(sys, 1, 0);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuEnable
//
// DESCRIPTION.: param 1 - Menu object, required.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuEnable(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  LPMENUINFO  pMenuInfo;
  AOBJECTID    menuObject;

  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1, OTYPE_MENU);

  if (menuObject == NULL)
    {
    ADBG_PRINT_E("Enable Menu: could not get first parameter");
    }
  else
    {
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
    pMenuInfo->status &= (~MENU_DISABLED);
    AObjUnlockData(menuObject, MENU_INFO_DATA);
    AEvtPostStandard(menuObject, AEVENT_OBJECTCHANGED, 0,
     AEVT_ATTAIL|AEVT_KILLDUPLICATES);
    }

  AFuncReportResult(sys, 1, 0);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuGetItemTitle
//
// DESCRIPTION.: param 1 - Menu or item object, required.
//               param 2 - index, optional if item is supplied.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuGetItemTitle(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  AOBJECTID  menuObject;
  AOBJECTID  itemObject;
  AOBJECTID  numberObject;
  AOBJECTID  textObject;
  char      buffer[MENU_MAX_TEXT+1];
  BOOL      goodParams = TRUE;
  BOOL      goodIndex = TRUE;

  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1, OTYPE_MENU);
  if (0 == menuObject)
    itemObject = AFuncGetTypedParameter(1, OTYPE_MENUITEM);

  if ((menuObject == 0) && (itemObject == 0))
    {
    ADBG_PRINT_E("Get Item Title: could not get first parameter");
    goodParams = FALSE;
    }

  if (0 != menuObject)
    {
    numberObject = AFuncGetTypedParameter(2, OTYPE_NUMBER);
    if (0 == numberObject)
      {
      ADBG_PRINT_E("Get Item Title: could not get second parameter");
      goodParams = FALSE;
      }
    }

  if (TRUE == goodParams)
    {
    if (0 != menuObject)
      {
      if (numberObject == 0)
        MenuGetTitle(menuObject, buffer, MENU_MAX_TEXT+1);
      else
        goodIndex = GetItemTextIdx(menuObject, numberObject, buffer, MENU_MAX_TEXT+1);
      }
    else
      ItemGetTitle(itemObject, buffer, MENU_MAX_TEXT+1);
    }
  else
    buffer[0] = '\0';

    //set the output
  textObject = AObjCreate(OTYPE_TEXT);
  if (textObject == 0)
    {
    ADBG_PRINT_E("Get Item Text: Cannot create text object for output");
    }
  else if(goodIndex)
    {
    OTextSetValueFromBuffer(textObject, buffer);
    AFuncSetParameter(1, textObject);
    }

  if (goodIndex)
    AFuncReportResult(sys, 1, 0); // Flow = "Done"
  else
    AFuncReportResult(sys, 2, 0); // Flow = "Out of range"

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuInsertItem
//
// DESCRIPTION.: param 1 - Menu object, required.
//               param 2 - item object or text object.
//               param 3 - number object, index for insertion, optional
//                         if omitted add at end
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuInsertItem(
  pAEvtInfo       evt,
  pAObjMessage    sys)
  {
  short           index;
  AOBJECTID        menuObject;
  AOBJECTID        itemObject;
//  AOBJECTID        textObject = 0;
  AOBJECTID        numberObject;
  AOBJECTID        windowObject;
  AOBJECTID        menuBarObject;
  AObjOwnerInfo   ownerInfo;
//  pAObjOwnerInfo  pMenuOwnerInfo = MenuOwnerInfo;
  AMEMBLOCKID      textBlock;
  LPMENUINFO      pMenuInfo;
  LPITEMINFO      pItemInfo;
  LPSTR           pTextBlock;
  LONG            getKey;
  BOOL            goodParams = TRUE;
  BOOL            objectIsItem;
  char            accelKey;
  
  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1L, OTYPE_MENU);

  if (menuObject == 0)
    {
    ADBG_PRINT_E("Insert Menu Item: unable to get first parameter");
    goodParams = FALSE;
    }

  itemObject = AFuncGetParameter(2L);

  /*
  if (0 == itemObject)
    textObject = AFuncGetTypedParameter(2L, OTYPE_TEXT);

  if ((0 == textObject) && (0 == itemObject))
    {
    ADBG_PRINT_E("Insert Menu Item: unable to get second parameter");
    goodParams = FALSE;
    }
  */
  if (AObjGetType(itemObject) == OTYPE_MENUITEM)
    objectIsItem = TRUE;
  else
    objectIsItem = FALSE;
    
  numberObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);

  if (TRUE == goodParams)
    {
    //determine index for new item
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
    if (0 != numberObject)
      {
      index = (short)ONmbrGetInteger(numberObject);
      if ((index < 1) || (index > pMenuInfo->itemCount))
        index = pMenuInfo->itemCount;  //out of range
      else
        index--;  //convert to zero based
      }
    else
      index = pMenuInfo->itemCount;
    AObjUnlockData(menuObject, MENU_INFO_DATA);

    //add item
    if (objectIsItem)
      {
      MenuAddObjectItem(menuObject, itemObject, index);
      // Register this menu as the owner of this menuitem.
      pItemInfo = (LPITEMINFO)AObjLockData(itemObject, ITEM_INFO_DATA);
      AObjRegisterOwner(menuObject, itemObject, pItemInfo->itemID);

      // Now we need to register the accelerator key
      if (pItemInfo->accelKey != NULL)
        {
        ownerInfo.key = AOBJ_GETFIRSTKEY;
        while (AObjGetNextOwner(OTYPE_MENUBAR, menuObject, &ownerInfo)) 
          {
          // iterate through all menubars that own this menu
          getKey = AOBJ_GETFIRSTKEY;
          menuBarObject = ownerInfo.ownerID;
          while (0 != (windowObject = AObjGetNextObject(OTYPE_WINDOW, &getKey)))
            {
            if (menuBarObject == OWndLayGetMenuBarID(windowObject)) // find which windows use this menubar
              {
              accelKey = pItemInfo->accelKey;
              if ((accelKey > 0) && (accelKey < 27))
              accelKey += (char)64;     // Needs to be ASCII value
              OWndSetMenuAccelKey(windowObject, pItemInfo->itemID, 
                           accelKey, pItemInfo->accelModifier);
              }             
            }
          }                 
        }
      AObjUnlockData(itemObject, ITEM_INFO_DATA);
      }
    else
      {
      textBlock = AMemAllocate(0);
      if (AObjConvertToText(itemObject, textBlock) == FALSE)
        {
        // initialize textBlock to a space
        AMemSetSize(textBlock, 2);
        pTextBlock = (LPSTR)AMemLock(textBlock);
        lstrcpy(pTextBlock, " ");
        AMemUnlock(textBlock);
        }
      MenuAddTextItem(menuObject, textBlock, index);
      }
    }

  AFuncReportResult(sys, 1, 0);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuDeleteItem
//
// DESCRIPTION.: param 1 - Menu object, required.
//               param 2 - item object or number object containing one based
//                         index.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuDeleteItem(
  pAEvtInfo       evt,
  pAObjMessage    sys)
  {
  short           index;
  AOBJECTID        menuObject;
  AOBJECTID        itemObject;
  AOBJECTID        numberObject = 0;
  AOBJECTID        windowObject;
  AOBJECTID        menuBarObject;
  LPMENUINFO      pMenuInfo;
  LPITEMENTRY     pItemEntry;
  LPITEMINFO      pItemInfo;
  AObjOwnerInfo   ownerInfo;
  BOOL            goodParams = TRUE;
  BOOL            goodIndex = TRUE;
  LONG            getKey;
  UINT            itemID;
  char            accelKey;
  short           i;

  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1L, OTYPE_MENU);

  if (menuObject == 0)
    {
    ADBG_PRINT_E("Delete Menu Item: unable to get first parameter");
    goodParams = FALSE;
    }

  itemObject = AFuncGetParameter(2L);

  if (0 == itemObject)
    {
    ADBG_PRINT_E("Delete Menu Item: unable to get second parameter");
    goodParams = FALSE;
    }
  else if (OTYPE_MENUITEM != AObjGetType(itemObject))
    {
    itemObject = 0;
    numberObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
    if (0 == numberObject)
      {
      ADBG_PRINT_E("Delete Menu Item: unable to get second parameter");
      goodParams = FALSE;
      }
    }

  if (TRUE == goodParams)
    {
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
    if (0 != numberObject)
      {
      index = (short)ONmbrGetInteger(numberObject);
      if ((index < 1) || (index > pMenuInfo->itemCount))
        goodIndex = FALSE;  //out of range
      else
        index--;   //convert to zero based
      }
    else
      {
      index = -1;
      pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
      if (0 != pItemEntry)
        {
        for (i = 0; i < pMenuInfo->itemCount; i++)
          {
          if ((pItemEntry[i].status & ITEMENTRY_MENUITEM) &&
              (pItemEntry[i].id == itemObject))
            {
            index = i;
            itemID = pItemEntry[i].menuItemID;
            break;
            }
          }
        AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
        }
      }

    if ((-1 != index) & (goodIndex))
      {
      MenuDeleteItem(menuObject, index);


      // Un-register the menu object as the owner of this menuitem
      if (0 != (pItemInfo = (LPITEMINFO)AObjLockData(itemObject, ITEM_INFO_DATA)))
        { // If text only or separator, then no item info.
        AObjUnregisterOwner(menuObject, itemObject, pItemInfo->itemID);
        
        // Now we need to un-register the accelerator key
        if (pItemInfo->accelKey != NULL)
          {
          ownerInfo.key = AOBJ_GETFIRSTKEY;
          while (AObjGetNextOwner(OTYPE_MENUBAR, menuObject, &ownerInfo)) 
            {
            // iterate through all menubars that own this menu
            getKey = AOBJ_GETFIRSTKEY;
            menuBarObject = ownerInfo.ownerID;
            while (0 != (windowObject = AObjGetNextObject(OTYPE_WINDOW, &getKey)))
              {
              if (menuBarObject == OWndLayGetMenuBarID(windowObject)) // find which windows use this menubar
                {
                accelKey ='\0';     // Set accel key to zero
                OWndSetMenuAccelKey(windowObject, (short)itemID, 
                             accelKey, accelKey);
                }             
              }
            }                 
          }
        AObjUnlockData(itemObject, ITEM_INFO_DATA);  
        }
      }
    AObjUnlockData(menuObject, MENU_INFO_DATA);
    }

  if (goodIndex)
    AFuncReportResult(sys, 1, 0); // Flow = "Done"
  else
    AFuncReportResult(sys, 2, 0); // Flow = "Out of range"

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuSetItemTitle
//
// DESCRIPTION.: Param 1 Menu Object or Menu Item Object
//               Param 2 Text Object
//               Param 3 Number Object, one based index, optional, must be
//                       supplied if param1 is a MenuObject.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuSetItemTitle(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  AOBJECTID        textObject;
  AOBJECTID        menuObject;
  AOBJECTID        numberObject;
  AOBJECTID        itemObject   = 0;
  LPMENUINFO      pMenuInfo;
  char            buffer[MENU_MAX_TEXT+1];
  BOOL            goodParams = TRUE;
  BOOL            goodIndex = TRUE;
  short           index;
  
  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1L, OTYPE_MENU);
  if (0 == menuObject)
    itemObject = AFuncGetTypedParameter(1L, OTYPE_MENUITEM);

  if ((0 == menuObject) && (0 == itemObject))
    {
    ADBG_PRINT_E("Set Item Title: unable to get first parameter");
    goodParams = FALSE;
    }

  textObject = AFuncGetTypedParameter(2L, OTYPE_TEXT);
  if (0 == textObject)
    {
    ADBG_PRINT_E("Set Item Title: unable to get second parameter");
    goodParams = FALSE;
    }

  if (0 != menuObject)
    {
    numberObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
    if (0 == numberObject)
      {
      ADBG_PRINT_E("Set Item Title: unable to get third parameter");
      goodParams = FALSE;
      }
    else
      {
      index = (short)ONmbrGetInteger(numberObject);
      pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
      if ((index < 1) || (index > pMenuInfo->itemCount))
        goodIndex = FALSE;
      AObjUnlockData(menuObject, MENU_INFO_DATA);
      }
    }

  if ((goodParams) && (goodIndex))
    {
    OTextGetToBuffer(textObject, buffer, MENU_MAX_TEXT+1);
    if (menuObject != 0)
      {
      MenuSetItemTitle(menuObject, buffer, index);
			// MenuSetItemTitle does post an OBJECTCHANGED event, so no need to post one here.
      }
    else
      {
      ItemSetTitle(itemObject, buffer);
 			// ItemSetTitle does not post an OBJECTCHANGED event, so we need to post one here.
      AEvtPostStandard(itemObject, AEVENT_OBJECTCHANGED, 0,
                       AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
      }

    }

  if (goodIndex)
    AFuncReportResult(sys, 1, "");  // Done
  else
    AFuncReportResult(sys, 2, "");  // Out of range

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuGetPopItem
//
// DESCRIPTION.: Param 1 Popup Menu Object
//                       supplied if param1 is a MenuObject.
//
//               output 1  index of currently selected item.
//               output 2  name of currently selected item.
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuGetPopItem(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  AOBJECTID      menuObject;
  AOBJECTID      numberObject;
  AOBJECTID      textObject;
  LPMENUINFO    pMenuInfo;
  LPITEMENTRY   pItemEntry;
  char          buffer[MENU_MAX_TEXT+1];
  LPSTR         pText;
  short         itemIndex;
  short         bufferSize = sizeof(buffer);
   
  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1, OTYPE_MENU);
  
  if (menuObject)
    {
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);

    // Get item index
    numberObject = AObjCreate(OTYPE_NUMBER);
    textObject = AObjCreate(OTYPE_TEXT);
    itemIndex = (short)(pMenuInfo->popIndex + 1);
    
    ONmbrSetInteger(numberObject, itemIndex, TRUE);
    AFuncSetParameter(1, numberObject);
    
    // Get item text
    if ((itemIndex<=pMenuInfo->itemCount) && (itemIndex > 0))
      {
      itemIndex--; //convert to zero based index
      pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
      if ((pItemEntry[itemIndex].status & ITEMENTRY_TEXTONLY) ==
          ITEMENTRY_TEXTONLY)
        {
        pText = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntry[itemIndex].id);
        if (pText != 0)
          {
          lstrcpyn(buffer, pText, bufferSize);
          buffer[bufferSize-1] = '\0';
          AMemUnlock((AMEMBLOCKID)pItemEntry[itemIndex].id);
          }
        else
          buffer[0] = '\0';
        }
      else
        ItemGetTitle(pItemEntry[itemIndex].id, buffer, bufferSize);
      AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
      }
    AObjUnlockData(menuObject, MENU_INFO_DATA);

    OTextSetValueFromBuffer(textObject, buffer);
    AFuncSetParameter(2, textObject);

    AFuncReportResult(sys, 1, "");
    
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_MenuSetPopItem
//
// DESCRIPTION.: Param 1  popup Menu Object
//               Param 2 Menu Item or Index 
//               Param 3 Menu Text
//
//               As per the 'Spec viewer' (the MAC), the priority is as follows:
//                  If Parameter 3 is specified, then it takes priority. 
//                  If Parameter 3 is not specified, then whichever of Parameter 2
//                  (Menu item or Menu index) is specified, defines the the new 
//                  current item.    MRG - 11 March 94
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_MenuSetPopItem(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  AOBJECTID      menuObject;
  AOBJECTID      numberObject;
  AOBJECTID      textObject;
  AOBJECTID      itemObject;
  LPMENUINFO    pMenuInfo;
  LPITEMENTRY   pItemEntry;
  short         itemIndex;
  LPSTR         pText;
  LPSTR         pBuffer = NULL;
  AMEMBLOCKID    textBlock;
  int           i;
  char          buffer[MENU_MAX_TEXT+1];
  short         bufferSize = sizeof(buffer);
  BOOL          goodIndex = TRUE;

  I_UNUSED(evt);

  menuObject = AFuncGetTypedParameter(1L, OTYPE_MENU);
  
//  textObject = AFuncGetParameter(3L);
  if ((NULL != (textObject = AFuncGetParameter(3L))) &&
     (OTYPE_TEXT == AObjGetType(textObject))) 
    {      // Use text object as input (1st priority) if available.

    // Get pointer to text from Text object.
    textBlock = AMemAllocate(0);
    OTextGetToMemblock(textObject, textBlock);
    pText = (char*)AMemLock(textBlock);
    
    // Get pointer to menu info
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
    if (0 != pMenuInfo)
      {
      pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
      if (NULL != pItemEntry)
        {
        for (i = 0; i < pMenuInfo->itemCount; i++)
          {
          if ((pItemEntry[i].status & ITEMENTRY_TEXTONLY) ==
              ITEMENTRY_TEXTONLY)
            {       // Text only entry, so get text 
            pBuffer = (LPSTR)AMemLock((AMEMBLOCKID)pItemEntry[i].id);
            if (NULL != pBuffer)
              {
              if (lstrcmp(pBuffer, pText) == 0)
                {
                pMenuInfo->popIndex = (WORD)i;
                break;
                }
              }
            }
          else if((pItemEntry[i].status & ITEMENTRY_MENUITEM) == ITEMENTRY_MENUITEM)
            {     // Menu item entry, so get text
            ItemGetTitle(pItemEntry[i].id, buffer, bufferSize);
            if (lstrcmp(buffer, pText) == 0)
              {
              pMenuInfo->popIndex = (WORD)i;
              break;
              }
            }
          }
        AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
        if (NULL != pBuffer)
          AMemUnlock((AMEMBLOCKID)pItemEntry[i].id);
        }  
      AObjUnlockData(menuObject, MENU_INFO_DATA);
      } 
    } 

  else if (( NULL != (numberObject = AFuncGetParameter(2L))) && 
         (OTYPE_NUMBER == AObjGetType(numberObject)))
    {   // use number object as input
    numberObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
    if (numberObject)
      {
      pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
      itemIndex = (short)ONmbrGetInteger(numberObject);
      if ((itemIndex<=pMenuInfo->itemCount) && (itemIndex > 0))
        pMenuInfo->popIndex = (WORD)(itemIndex - 1);
      else
        goodIndex = FALSE;
      AObjUnlockData(menuObject, MENU_INFO_DATA);
      }
    }

  else if (( NULL != (itemObject = AFuncGetParameter(2L))) && 
            (OTYPE_MENUITEM == AObjGetType(itemObject)))
    {    //  use menu item object as input
    pMenuInfo = (LPMENUINFO)AObjLockData(menuObject, MENU_INFO_DATA);
    if (0 != pMenuInfo)
      {
      pItemEntry = (LPITEMENTRY)AObjLockData(menuObject, MENU_ITEMLIST_DATA);
      for (i = 0; i < pMenuInfo->itemCount; i++)
        {
        if (pItemEntry[i].id == itemObject)
          {
          pMenuInfo->popIndex = (WORD)i;
          break;
          }
        }
      AObjUnlockData(menuObject, MENU_INFO_DATA);
      }
    AObjUnlockData(menuObject, MENU_ITEMLIST_DATA);
    }

  if (goodIndex)
    AFuncReportResult(sys, 1, "");  // Done
  else
    AFuncReportResult(sys, 2, "");  // Out of range

  // Send event -> data changed 
  AEvtPostStandard(menuObject, AEVENT_OBJECTCHANGED, 0, AEVT_ATTAIL|AEVT_KILLDUPLICATES);

  }

