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
// FILE:					MENUDLG.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Menu object editing routines.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>

#include "a_alm.h"
#include "a_almutl.h"
#include "o_menus.h"
#include "o_text.h"
#include "o_number.h"

#include "modinfo.h"
#include "menudlg.h"
#include "menuobj.h"
#include "menulib.h"
#include "itemlib.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void EditObject(
  HWND  hDlg)
  {
  WORD        cnt;
  LRESULT     ret;
  WORD        indexp[1];

  cnt = (WORD)SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELCOUNT, 0, 0L);

  if (cnt == 1)
    {
		ret = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELITEMS, cnt, (LONG)indexp);
    if (ret == 1)
			{
			ret = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETITEMDATA, *indexp, 0L);
      if ((ret != LB_ERR) &&
          (ret != SEPARATORID) &&
          (ret != BLANKID))
			  AObjEdit( OBJECTID(ret) );
			}
		}
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: LBMenuItemAsName
//
// DESCRIPTION.: returns number of items in list
//
/////////////////////////////////////////////////////////////////////////////
static void LbMenuItemAsTitle(
  HWND      hDlg,
  OBJECTID  objID)
  {
	LRESULT     index;
	LONG        objkey;
	OBJECTID    tempID;
  char        buffer[MENU_MAX_TEXT+1];


	SendDlgItemMessage(hDlg, LB_SOURCE, LB_RESETCONTENT, 0, 0L);

	if (IsDlgButtonChecked(hDlg, BTN_SHOWMENUS))
		{
		objkey = AOBJ_GETFIRSTKEY;
				// Insert Right Listbox Menus
		while ((tempID = AObjGetNextObject(OTYPE_MENU, (LONG  *)&objkey)) != 0 )
			{
			// a menu cannot contain itself nor a popup
			if ((tempID != objID) && (FALSE == OMenuIsPopup(objID)))
        {
				MenuGetTitle(tempID, buffer, MENU_MAX_TEXT+1);
				index = SendDlgItemMessage(hDlg, LB_SOURCE, LB_ADDSTRING, 0,
         (LONG)(LPSTR)buffer);
			  SendDlgItemMessage(hDlg, LB_SOURCE, LB_SETITEMDATA, (WPARAM)index,
         (LPARAM)tempID);
			  }
      }
		}
	else // show items by default
		{
		objkey = AOBJ_GETFIRSTKEY;
		while ((tempID = AObjGetNextObject(OTYPE_MENUITEM, &objkey)) != 0)
			{
      ItemGetTitle(tempID, buffer, MENU_MAX_TEXT+1);
			index =
       SendDlgItemMessage(hDlg, LB_SOURCE, LB_ADDSTRING, 0, (long)buffer);
			SendDlgItemMessage(hDlg, LB_SOURCE, LB_SETITEMDATA, (WPARAM)index, (LPARAM)tempID);
			}
		}

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateAddButton
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateAddButton(
  HWND  hDlg)
  {

	if (0 < SendDlgItemMessage(hDlg, LB_SOURCE, LB_GETSELCOUNT, 0, 0L))
		EnableWindow(GetDlgItem(hDlg, BTN_ADD), TRUE);
	else
		EnableWindow(GetDlgItem(hDlg, BTN_ADD), FALSE);

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  UpdateRemoveButton
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateRemoveButton(
  HWND  hDlg)
  {
	LRESULT    cnt;
	LONG ret;
	int index;

	cnt = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELCOUNT, 0, 0L);

		//make sure there are more items than just the blank item
	if (cnt == 1)
		{
			//make sure the only selected item is not the blank item
		ret = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELITEMS, 1,
		 (LPARAM)&index);

			if(ret != LB_ERR)
				ret = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETITEMDATA, index,
					0L);

			if(ret == BLANKID)
			 {                 	//We can't check here to make sure that ret!=LB_ERR
													//since LB_ERR has the same value as Robert's
													//stupid separator ID's and we want the remove
													//button to be enabled if it's a separator id.
													//Therefore, if an error occurs, the program will
													//think its a separtor ID.  Cool, huh?
				EnableWindow(GetDlgItem(hDlg, BTN_REMOVE), FALSE);
				}
			else
				EnableWindow(GetDlgItem(hDlg, BTN_REMOVE), TRUE);
			}
		else if(cnt > 1)
			EnableWindow(GetDlgItem(hDlg, BTN_REMOVE), TRUE);
		else
				EnableWindow(GetDlgItem(hDlg, BTN_REMOVE), FALSE);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateEditButton
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateEditButton(
  HWND  hDlg)
  {
	LRESULT		cnt;
	WORD		  indexp[1];
	LRESULT 	ret;

	cnt = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELCOUNT, 0, 0L);

	if (cnt == 1)
	  {
		SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELITEMS, (WPARAM)cnt, (LONG)indexp);
		ret = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETITEMDATA,
     indexp[0], (LPARAM)0L);
		if (ret == SEPARATORID  || ret == BLANKID)
			EnableWindow(GetDlgItem(hDlg, BTN_EDIT), FALSE);
		else
			EnableWindow(GetDlgItem(hDlg, BTN_EDIT), TRUE);
		}
	else
		EnableWindow(GetDlgItem(hDlg, BTN_EDIT), FALSE);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateSeparateButton
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateSeparateButton(
  HWND  hDlg)
  {
	LRESULT		cnt;

	cnt = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETCOUNT, 0, 0L);

	if (cnt >0)
		EnableWindow(GetDlgItem(hDlg, BTN_SEPARATOR), TRUE);
	else
		EnableWindow(GetDlgItem(hDlg, BTN_SEPARATOR), FALSE);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  InsertObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InsertObject(
  HWND  hDlg)
  {
	BOOL        asTitle;
	char        text[MENU_MAX_TEXT+1];
	short       i;
	short       srcSelectCount;
  short       insertionIndex;
  short*      pSrcSelections;
	OBJECTID    tempID;
  long        ret;

	asTitle = IsDlgButtonChecked(hDlg, BTN_DISPLAYTITLE);

	srcSelectCount =
   (WORD)SendDlgItemMessage(hDlg, LB_SOURCE, LB_GETSELCOUNT, 0, 0L);

	if (srcSelectCount > 0)
		{
		pSrcSelections = new short[(unsigned int)srcSelectCount];
		SendDlgItemMessage(hDlg, LB_SOURCE, LB_GETSELITEMS, srcSelectCount,
     (LONG)pSrcSelections);

    // get the first selected item in destination list if it exits
		ret = SendDlgItemMessage(hDlg, LB_DESTINATION,
     LB_GETSELITEMS, 1, (long)(short far*)&insertionIndex);

    if (ret != 1)
      {// insert before the dummy empty entry
      insertionIndex =
       (WORD)SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETCOUNT, 0, 0L);
      insertionIndex -= 1;
      }

		//clear all selections
		SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETSEL, FALSE, -1);

		for (i = 0; i < srcSelectCount; i++)
		  {
			tempID = (OBJECTID)SendDlgItemMessage(hDlg, LB_SOURCE, LB_GETITEMDATA,
       pSrcSelections[i], 0L);
			if (asTitle)
				{
				OMenuGetTitle(tempID, text, MENU_MAX_TEXT);
				}
			else
				{
				AObjGetName(tempID, (LPSTR)text, MENU_MAX_TEXT);
				}

			SendDlgItemMessage(hDlg, LB_DESTINATION,LB_INSERTSTRING,
       insertionIndex, (LONG)text);
			SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETITEMDATA,
       insertionIndex, (LPARAM)tempID);
      insertionIndex++;
			}

		SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETSEL, TRUE,
			MAKELPARAM(insertionIndex, 0));

		delete pSrcSelections;

    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InsertSeparator
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InsertSeparator(
  HWND  hDlg)
  {
  short index;
	short insertionIndex;
	LONG  ret;

	ret = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELITEMS, 1,
   (long)(short far*)&insertionIndex);

  if (ret != 1)
    {// insert before the dummy empty entry
    insertionIndex =
     (short)SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETCOUNT, 0, 0L);
    insertionIndex -= 1;
    }

	index = (WORD)SendDlgItemMessage(hDlg, LB_DESTINATION, LB_INSERTSTRING,
	 insertionIndex, (LONG)SEPARATORTEXT);

	SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETITEMDATA, index, SEPARATORID);

	SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETSEL, TRUE,
	 MAKELPARAM(index+1, 0));

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DeleteObjects
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DeleteObjects(
  HWND  hDlg)
  {
	int         i;
	WORD        count;
	WORD*       indexp;
	LONG        tempID;
	WORD				newCount;

	count = (WORD) SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELCOUNT, 0, 0L);

	if (count > 0)
    {
		indexp = new WORD[count];
		SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETSELITEMS, count, (LONG)indexp);
		for (i = count-1; i >= 0; i--)
			{
			tempID = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETITEMDATA,
       indexp[i],0L);
			if (tempID != BLANKID)
				SendDlgItemMessage(hDlg, LB_DESTINATION, LB_DELETESTRING, indexp[i], 0L);
			}

		//If the total count is one then that item must be the blank so select it
		//Get the new count of the items in the list
		newCount = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETCOUNT, 0, 0L);
		if(newCount == 1)
			{
			//-1 means to select everything which is OK since the blank is the
			//only item.
			SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETSEL, TRUE, -1);
			}
		else
			{
			//highlight the item after the last deleted item.
			//We delete count items and we want to place it 1 item after the
			//last selected item so subtract count and add 1 to the index of the
			//last selected item.
			SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETSEL, TRUE,
				MAKELPARAM(indexp[count-1] - count + 1, 0));
			}
    }

	delete indexp;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: LBMenuItemAsName
//
// DESCRIPTION.: returns number of items in list
//
/////////////////////////////////////////////////////////////////////////////
static void LbMenuItemAsName(
  HWND      hDlg,
  OBJECTID  objID)
  {
	char        text[MENU_MAX_TEXT+1];
	LRESULT     index;
	LONG        objkey;
  OBJECTID    tempID;

	if (IsDlgButtonChecked(hDlg, BTN_SHOWMENUS) )
		{
		SendDlgItemMessage(hDlg, LB_SOURCE, LB_RESETCONTENT, 0, 0L);
    objkey = AOBJ_GETFIRSTKEY;
		// Insert Right Listbox Menus
		while ((tempID = AObjGetNextObject(OTYPE_MENU, &objkey)) != 0 )
			{
			// a menu cannot contain itself nor popup menus
			if ((tempID != objID) && (OMenuIsPopup(tempID) == FALSE))
        {
				AObjGetName(tempID, (LPSTR)text, MENU_MAX_TEXT);
				index =
         SendDlgItemMessage(hDlg, LB_SOURCE, LB_ADDSTRING, 0, (LONG)text);
				SendDlgItemMessage(hDlg, LB_SOURCE, LB_SETITEMDATA, (WPARAM)index,
         (LPARAM)tempID);
				}
		  }
    }
	else
		{
		SendDlgItemMessage(hDlg, LB_SOURCE, LB_RESETCONTENT, 0, 0L);
		objkey = AOBJ_GETFIRSTKEY;
		while ((tempID = AObjGetNextObject(OTYPE_MENUITEM, (LONG  *)&objkey)) != 0 )
			{
			AObjGetName(tempID, (LPSTR)text, MENU_MAX_TEXT);
			index = SendDlgItemMessage(hDlg, LB_SOURCE, LB_ADDSTRING, 0, (LONG)text);
			SendDlgItemMessage(hDlg, LB_SOURCE, LB_SETITEMDATA, (WPARAM)index,
       (LPARAM)tempID);
			}
		}

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: LbContentUpdate
//
// DESCRIPTION.: returns number of items in list
//
/////////////////////////////////////////////////////////////////////////////
static void LbContentUpdate(
  HWND      hDlg,
  OBJECTID  objID)
  {

	SendDlgItemMessage(hDlg, LB_SOURCE, LB_RESETCONTENT, 0, 0L);

  if (IsDlgButtonChecked(hDlg, BTN_DISPLAYNAME))
    LbMenuItemAsName(hDlg, objID);
  else
    LbMenuItemAsTitle(hDlg, objID);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitObjectList
//
// DESCRIPTION.: returns number of items in list
//
/////////////////////////////////////////////////////////////////////////////
static void InitObjectList(
  HWND      hDlg,
  OBJECTID  objID)
  {
	int          i;
	char         text[MENU_MAX_TEXT+1];
	LRESULT      index;
  LPITEMENTRY  pItemEntry;
  LPMENUINFO   pMenuInfo;

	// Right ListBox - always as ObjectName at start
	LbMenuItemAsName(hDlg, objID);

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

	// Insert Left ListBox Items

  if (pMenuInfo->itemCount > 0)
		{
    pItemEntry = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);
		for (i = 0; i < pMenuInfo->itemCount; i++)
		  {
			if (pItemEntry[i].id != (OBJECTID)SEPARATORID)
				{
				AObjGetName(pItemEntry[i].id, text, MENU_MAX_TEXT);
				index = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_ADDSTRING,
         0, (LONG)text);
				SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETITEMDATA,
          (WPARAM)index, (LPARAM)pItemEntry[i].id);
				}
			else
				{
				//Don't call InsertSeparator here since the blank item hasn't
				//been added yet.
				index = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_ADDSTRING,
         0, (long)SEPARATORTEXT);
				SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETITEMDATA,
         (WPARAM)index, SEPARATORID);
				}
			}
    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
		}

	//Insert a blank item at end of list
	index = SendDlgItemMessage(hDlg, LB_DESTINATION, LB_ADDSTRING, 0, (LONG)"");
	SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETITEMDATA, (WPARAM)index, (LPARAM)BLANKID);

	//Set the selection to the blank item
	SendDlgItemMessage(hDlg, LB_DESTINATION, LB_SETSEL, TRUE,
 	 MAKELPARAM(index, 0));

  AObjUnlockData(objID, MENU_INFO_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: LbTitleUpdate
//
// DESCRIPTION.: returns number of items in list
//
/////////////////////////////////////////////////////////////////////////////
static void LbTitleUpdate(
  HWND  hDlg,
  int   cntrlID,
  BOOL  saveSelection)
  {
	BOOL       usingName;
	char       text[MENU_MAX_TEXT+1];
 	WORD       i;
	LRESULT    cnt;
	LRESULT    index;
	OBJECTID*  objidp;
	int*       oldSelection;
	int        selCount = 0;

	cnt = SendDlgItemMessage(hDlg, cntrlID, LB_GETCOUNT, 0, 0L);

	if (cnt > 0)
    {
	  objidp  = new OBJECTID[(int)cnt];

		// save objid and delete items
		if(saveSelection)
			{
			selCount = (short)SendDlgItemMessage(hDlg, cntrlID, LB_GETSELCOUNT, 0, 0L);
			if(selCount)
				{
				oldSelection = (int*)new int[selCount];
				SendDlgItemMessage(hDlg, cntrlID, LB_GETSELITEMS,selCount,
					(long)oldSelection);
				}
		  }

		for (i = 0; i < cnt; i++)
		  objidp[i] = (OBJECTID)SendDlgItemMessage(hDlg, cntrlID, LB_GETITEMDATA, i, 0L);

		SendDlgItemMessage(hDlg, cntrlID, LB_RESETCONTENT, 0, 0L);

		usingName = IsDlgButtonChecked(hDlg, BTN_DISPLAYNAME);

		if (usingName)
		  {
			// add items with object name
			for (i = 0; i < cnt; i++)
				{
				if (objidp[i] == (OBJECTID)SEPARATORID)
					{
					index = SendDlgItemMessage(hDlg, cntrlID, LB_ADDSTRING, 0,
           (LONG)SEPARATORTEXT);
					SendDlgItemMessage(hDlg, cntrlID, LB_SETITEMDATA, (WPARAM)index,
           (LPARAM)objidp[i]);
					}
				else if(objidp[i] != (OBJECTID)BLANKID)
					{
					AObjGetName(objidp[i], (LPSTR)text, MENU_MAX_TEXT);
					index = SendDlgItemMessage(hDlg, cntrlID, LB_ADDSTRING, 0, (LONG)text);
					SendDlgItemMessage(hDlg, cntrlID, LB_SETITEMDATA, (WPARAM)index,
           (LPARAM)objidp[i]);
					}
				}
		  }
		else
		  {
				// add items with object title
			for (i = 0; i < cnt; i++)
				{
			  if (objidp[i] == (OBJECTID)SEPARATORID)
			    {
				  index = SendDlgItemMessage(hDlg, cntrlID, LB_ADDSTRING, 0, (LONG)SEPARATORTEXT);
				  SendDlgItemMessage(hDlg, cntrlID, LB_SETITEMDATA, (WPARAM)index,
           (LPARAM)objidp[i]);
					}
			  else if(objidp[i] != (OBJECTID)BLANKID)
					{
          OMenuGetTitle(objidp[i], text, MENU_MAX_TEXT+1);
					index = SendDlgItemMessage(hDlg, cntrlID, LB_ADDSTRING, 0, (LONG)text);
				  SendDlgItemMessage(hDlg, cntrlID, LB_SETITEMDATA, (WPARAM)index,
           (LPARAM)objidp[i]);
					}
				}
		  }

		delete objidp;

    }

	//if its the left listbox add a blank item
	if(cntrlID == LB_DESTINATION)
		{
		index = SendDlgItemMessage(hDlg, cntrlID, LB_ADDSTRING, 0, (LONG)"");
		SendDlgItemMessage(hDlg, cntrlID, LB_SETITEMDATA, (WPARAM)index,
			(LPARAM)BLANKID);

		//now select the blank item if we're not saving the selection state
		if(!saveSelection)
			SendDlgItemMessage(hDlg, cntrlID, LB_SETSEL, TRUE,
				MAKELPARAM(index, 0));
		}

	if(saveSelection && selCount > 0)
		{
		for(i=0;i<selCount;i++)
			{
			SendDlgItemMessage(hDlg, cntrlID, LB_SETSEL, TRUE,
				MAKELPARAM(oldSelection[i], 0));
			}
		delete oldSelection;
		}

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateItemList
//
// DESCRIPTION.: returns number of items in list
//
/////////////////////////////////////////////////////////////////////////////
static short UpdateItemList(
  HWND      hDlg,
  OBJECTID  objID)
  {
	WORD         cnt;
  WORD         i;
  LPITEMENTRY  pItemEntry;

	cnt = (WORD)SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETCOUNT, 0, 0L);

	//subtract 1 from the count since the last item in the list is the
	//blank item;
	cnt--;

  if (cnt > 0)
    {
   	AObjResizeData(objID, MENU_ITEMLIST_DATA, (LONG)(sizeof(ITEMENTRY)*(cnt)));

    pItemEntry = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);

	  for (i = 0; i < cnt; i++)
		  {
		  pItemEntry[i].id = (OBJECTID)
       SendDlgItemMessage(hDlg, LB_DESTINATION, LB_GETITEMDATA, i, 0L);

		  if ((OBJECTID)SEPARATORID == pItemEntry[i].id)
        {
        pItemEntry[i].status = ITEMENTRY_SEPARATOR;
        pItemEntry[i].menuItemID = 0;
        }
      else if (OTYPE_MENU == AObjGetType(pItemEntry[i].id))
        {
        pItemEntry[i].status = ITEMENTRY_MENU;
        pItemEntry[i].menuItemID = (int)pItemEntry[i].id;
        }
      else
        {
        pItemEntry[i].menuItemID = MenuGenerateItemID();
        pItemEntry[i].status = ITEMENTRY_MENUITEM;
        }
		  }

    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
    }

  return cnt;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateObject(
  HWND      hDlg,
  OBJECTID  objID )
  {
	char        text[MENU_MAX_TEXT+1];
	LRESULT     index;
  LPMENUINFO  pMenuInfo;

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

	pMenuInfo->status &= (~MENU_NEVER_EDITED);

	if (IsDlgButtonChecked(hDlg, BTN_DISABLED))
    pMenuInfo->status |= MENU_DISABLED;
  else
    pMenuInfo->status &= (~MENU_DISABLED);

	if (IsDlgButtonChecked(hDlg, BTN_POPUP))
		pMenuInfo->status |= MENU_POPUP;
  else
    pMenuInfo->status &= (~MENU_POPUP);

	index = SendDlgItemMessage(hDlg, CB_TEXTTO, CB_GETCURSEL, 0, 0L);
	if (index == CB_ERR)
		pMenuInfo->sendText = 0L;
	else
		pMenuInfo->sendText = (OBJECTID)
     SendDlgItemMessage(hDlg, CB_TEXTTO, CB_GETITEMDATA, WPARAM(index), 0L);

	index = SendDlgItemMessage(hDlg, CB_INDEXTO, CB_GETCURSEL, 0, 0L);
	if (index == CB_ERR)
		pMenuInfo->sendIndex = 0L;
	else
		pMenuInfo->sendIndex = (OBJECTID)SendDlgItemMessage(hDlg, CB_INDEXTO,
     CB_GETITEMDATA, WPARAM(index), 0L);

  pMenuInfo->itemCount = UpdateItemList(hDlg, objID);

	GetDlgItemText(hDlg, SLE_TITLE, (LPSTR)text, MENU_MAX_TEXT);
	AObjSetData(objID, MENU_TITLE_DATA, LPSTR(text), lstrlen(text)+1);

  AObjUnlockData(objID, MENU_INFO_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FillComboBoxes
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void FillComboBoxes(
  HWND  hDlg)
  {
  BYTE        text[MENU_MAX_TEXT+1];
  LRESULT     index;
  LONG        objkey;
  OBJECTID    objid;

  // "Send Text To" Combo Box
	index =
   SendDlgItemMessage(hDlg, CB_TEXTTO, CB_ADDSTRING, 0, (LONG)"(None)");

	SendDlgItemMessage(hDlg, CB_TEXTTO, CB_SETITEMDATA, (WPARAM)index, 0L);

  objkey = AOBJ_GETFIRSTKEY;
	while ( (objid = AObjGetNextObject(OTYPE_TEXT, &objkey)) != 0 )
    {
    AObjGetName(objid, (LPSTR)text, MENU_MAX_TEXT);
    index =
     SendDlgItemMessage(hDlg, CB_TEXTTO, CB_INSERTSTRING, -1, (LONG)text);
		SendDlgItemMessage(hDlg, CB_TEXTTO, CB_SETITEMDATA, (WPARAM)index,
     (LPARAM)objid);
    }

  // "Send Index To" Combo Box

	index =
   SendDlgItemMessage(hDlg, CB_INDEXTO, CB_ADDSTRING, 0, (LONG)"(None)");
	SendDlgItemMessage(hDlg, CB_INDEXTO, CB_SETITEMDATA, (WPARAM)index, 0L);

  objkey = AOBJ_GETFIRSTKEY;
	while ( (objid = AObjGetNextObject(OTYPE_NUMBER, &objkey)) != 0 )
    {
    AObjGetName(objid, (LPSTR)text, MENU_MAX_TEXT);
		index = SendDlgItemMessage(hDlg, CB_INDEXTO, CB_INSERTSTRING, -1, (LONG)text);
    SendDlgItemMessage(hDlg, CB_INDEXTO, CB_SETITEMDATA, (WPARAM)index,
     (LPARAM)objid);
    }

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SelectComboBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void SelectComboBox(
  HWND      hDlg,
  WORD      cntrlId,
  OBJECTID  cursel)
  {
	WORD        index;
	LRESULT     count;
	OBJECTID    obj;

  count = SendDlgItemMessage(hDlg, cntrlId, CB_GETCOUNT, 0, 0L);

  if (count != 0)
    {
    for (index = 0; index < count; index++)
      {
      obj = (OBJECTID)SendDlgItemMessage(hDlg, cntrlId, CB_GETITEMDATA,
       index, 0L);
      if (cursel == obj)
		    {
        SendDlgItemMessage(hDlg, cntrlId, CB_SETCURSEL, index, 0L);
        break;
        }
      }
    }

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDlgControl
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL InitDlgControl(
  HWND     hDlg,
  OBJECTID objID)
  {
  char        text[MENU_MAX_TEXT+1];
  LPMENUINFO  pMenuInfo;
  LPSTR       pTitle;

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

	AObjGetName(objID, text, MENU_MAX_TEXT);

	SetWindowText(hDlg, text);

	if ((pMenuInfo->status & MENU_NEVER_EDITED) == MENU_NEVER_EDITED)
    {
		AObjGetName(objID, text, MENU_MAX_TEXT);
		AObjSetData(objID, MENU_TITLE_DATA, text, (LONG)lstrlen(text)+1);
    pMenuInfo->status & (~MENU_NEVER_EDITED);
		}

  CheckRadioButton(hDlg, BTN_DISPLAYNAME, BTN_DISPLAYTITLE, BTN_DISPLAYNAME);
  CheckRadioButton(hDlg, BTN_SHOWMENUS, BTN_SHOWITEMS, BTN_SHOWITEMS);

  if ((pMenuInfo->status & MENU_DISABLED) == MENU_DISABLED)
    CheckRadioButton(hDlg, BTN_ENABLED, BTN_DISABLED, BTN_DISABLED);
  else
    CheckRadioButton(hDlg, BTN_ENABLED, BTN_DISABLED, BTN_ENABLED);

  if ((pMenuInfo->status & MENU_POPUP) == MENU_POPUP)
 	  CheckRadioButton(hDlg, BTN_NORMAL, BTN_POPUP, BTN_POPUP);
  else
 	  CheckRadioButton(hDlg, BTN_NORMAL, BTN_POPUP, BTN_NORMAL);

  pTitle = (LPSTR)AObjLockData(objID, MENU_TITLE_DATA);
  if (pTitle)
    {
		SetDlgItemText(hDlg, SLE_TITLE, pTitle);
    AObjUnlockData(objID, MENU_TITLE_DATA);
    }

  FillComboBoxes(hDlg);

	SelectComboBox(hDlg, CB_TEXTTO,  pMenuInfo->sendText);
	SelectComboBox(hDlg, CB_INDEXTO, pMenuInfo->sendIndex);

	SetFocus(GetDlgItem(hDlg, SLE_TITLE));

	SendDlgItemMessage(hDlg, SLE_TITLE, EM_SETSEL, 0, MAKELONG(0, -1));

  //return false to set proper return value from dialog proc because of the
  //SetFocus call above

  AObjUnlockData(objID, MENU_INFO_DATA);

  return FALSE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDialog
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL InitDialog(
  HWND      hDlg,
  OBJECTID  objID)
  {
  BOOL  result;

	result = InitDlgControl(hDlg, objID);
	InitObjectList(hDlg, objID);
	UpdateAddButton(hDlg);
	UpdateRemoveButton(hDlg);
	UpdateEditButton(hDlg);
	UpdateSeparateButton(hDlg);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditMenuDialogProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK _export EditMenuDialogProc(
  HWND    hDlg,
  UINT    message,
  WPARAM  wParam,
  LPARAM  lParam)
  {
  BOOL      result;
  OBJECTID  objID;

	switch ( message )
		{
		case WM_INITDIALOG:
			SetWindowLong(hDlg, DWL_USER, lParam);
      InitDialog(hDlg, (OBJECTID)lParam);
	    AUtlSetTitleFont(OTYPE_MENU, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
  		result = FALSE;
      break;

		case WM_COMMAND:
			objID = (OBJECTID)GetWindowLong(hDlg, DWL_USER);
			switch ( wParam )
				{
				case IDOK:
					SetFocus(GetDlgItem(hDlg, IDOK));
					UpdateObject(hDlg, objID);
					EndDialog(hDlg, TRUE);
					result = TRUE;
          break;

				case IDCANCEL:
					SetFocus(GetDlgItem(hDlg, IDCANCEL));
					EndDialog(hDlg, FALSE);
					result = TRUE;
          break;

				case IDHELP:
//					WinHelp (hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Menu) ;
					result = TRUE;
          break;

				case BTN_DISPLAYNAME:
				case BTN_DISPLAYTITLE:
					LbTitleUpdate(hDlg, LB_SOURCE, TRUE);
					LbTitleUpdate(hDlg, LB_DESTINATION, TRUE);
					result = TRUE;
          break;

				case BTN_SHOWITEMS:
				case BTN_SHOWMENUS:
					LbContentUpdate(hDlg,objID);
					UpdateAddButton(hDlg);
					result = TRUE;
          break;

				case BTN_ADD:
				  InsertObject(hDlg);
					UpdateAddButton(hDlg);
					UpdateSeparateButton(hDlg);
			    result = TRUE;
          break;

				case BTN_REMOVE:
					DeleteObjects(hDlg);
					UpdateEditButton(hDlg);
					UpdateRemoveButton(hDlg);
					UpdateSeparateButton(hDlg);
					result = TRUE;
          break;

				case BTN_EDIT:
					EditObject(hDlg);
          LbTitleUpdate(hDlg, LB_SOURCE, TRUE);
					LbTitleUpdate(hDlg, LB_DESTINATION, TRUE);
					result = TRUE;
          break;

				case BTN_SEPARATOR:
					InsertSeparator(hDlg);
					result = TRUE;
          break;

		    case LB_SOURCE:
		 			UpdateAddButton(hDlg);
					if (HIWORD(lParam) == LBN_DBLCLK)
						{
						InsertObject(hDlg);
				    UpdateAddButton(hDlg);
						UpdateSeparateButton(hDlg);
			      }
					result = TRUE;
          break;
          
				case LB_DESTINATION:
					UpdateRemoveButton(hDlg);
					UpdateEditButton(hDlg);
			    if (HIWORD(lParam) == LBN_DBLCLK)
			      {
						EditObject(hDlg);
				    LbTitleUpdate(hDlg, LB_SOURCE, TRUE);
						LbTitleUpdate(hDlg, LB_DESTINATION, TRUE);
						}
			    result = TRUE;
          break;

        default:
          result = FALSE;
				}
			break;

    default:
      result = FALSE;
      break;

	  }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditMenuObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
long EditMenuObject(
  OBJECTID  objID)
  {
  long result = A_OK;

	if (TRUE == DialogBoxParam(MODULEINFO::hInstance, "DLGMENU", GetActiveWindow(),
       EditMenuDialogProc, (LPARAM)objID))
    result = A_MODIFIED;

  return result;

  }