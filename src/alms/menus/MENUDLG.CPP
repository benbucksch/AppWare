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
#include <windowsx.h>

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
#include "helpids.h"


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

  cnt = (WORD)ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

  if (cnt == 1)
    {
	ret = ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), cnt, indexp);
    if (ret == 1)
			{
			ret = ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), *indexp);
      if ((ret != LB_ERR) &&
          (ret != SEPARATORID) &&
          (ret != BLANKID))
			  AObjEdit( AOBJECTID(ret) );
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
  AOBJECTID  objID)
  {
	LRESULT     index;
	LONG        objkey;
	AOBJECTID    tempID;
  char        buffer[MENU_MAX_TEXT+1];


	ListBox_ResetContent(GetDlgItem(hDlg, LB_SOURCE));

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
				index = ListBox_AddString(GetDlgItem(hDlg, LB_SOURCE), buffer);
			  ListBox_SetItemData(GetDlgItem(hDlg, LB_SOURCE), index, tempID);
			  }
      }
		}
	else // show items by default
		{
		objkey = AOBJ_GETFIRSTKEY;
		while ((tempID = AObjGetNextObject(OTYPE_MENUITEM, &objkey)) != 0)
			{
      ItemGetTitle(tempID, buffer, MENU_MAX_TEXT+1);
			index =	ListBox_AddString(GetDlgItem(hDlg, LB_SOURCE), buffer);
			ListBox_SetItemData(GetDlgItem(hDlg, LB_SOURCE), index, tempID);
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

	if (0 < ListBox_GetSelCount(GetDlgItem(hDlg, LB_SOURCE)))
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

	cnt = ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

		//make sure there are more items than just the blank item
	if (cnt == 1)
		{
			//make sure the only selected item is not the blank item
		ret = ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), 1, &index);

			if(ret != LB_ERR)
				ret =  ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), index);

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
	int				cnt;
	int			  indexp;
	LRESULT 	ret;

	cnt =  ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

	if (cnt == 1)
	  {
		ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), cnt, &indexp);
		ret = ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), indexp);
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

	cnt = ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));

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
	int       	srcSelectCount;
  int       	insertionIndex;
  int*      	pSrcSelections;
	AOBJECTID  	tempID;
  long        ret;

	asTitle = IsDlgButtonChecked(hDlg, BTN_DISPLAYTITLE);

	srcSelectCount =	ListBox_GetSelCount(GetDlgItem(hDlg, LB_SOURCE));

	if (srcSelectCount > 0)
		{
		pSrcSelections = new int[(unsigned int)srcSelectCount];
		ListBox_GetSelItems(GetDlgItem(hDlg, LB_SOURCE), srcSelectCount, pSrcSelections);

    // get the first selected item in destination list if it exits
		ret = ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), 1, &insertionIndex);

    if (ret != 1)
      {// insert before the dummy empty entry
      insertionIndex = ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));
      insertionIndex -= 1;
      }

		//clear all selections
		ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), FALSE, -1);

		for (i = 0; i < srcSelectCount; i++)
		  {
			tempID = (AOBJECTID)ListBox_GetItemData(GetDlgItem(hDlg, LB_SOURCE), pSrcSelections[i]);
			if (asTitle)
				{
				OMenuGetTitle(tempID, text, MENU_MAX_TEXT);
				}
			else
				{
				AObjGetName(tempID, (LPSTR)text, MENU_MAX_TEXT);
				}

			ListBox_InsertString(GetDlgItem(hDlg, LB_DESTINATION), insertionIndex, text);
			ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), insertionIndex, tempID);
      insertionIndex++;
			}

		ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, insertionIndex);

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
  int 	index;
	int	 	insertionIndex;
	LONG  ret;

	ret = ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), 1, &insertionIndex);

  if (ret != 1)
    {// insert before the dummy empty entry
    insertionIndex = ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));
    insertionIndex -= 1;
    }

	index = ListBox_InsertString(GetDlgItem(hDlg, LB_DESTINATION), insertionIndex, SEPARATORTEXT);

	ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), index, SEPARATORID);

	ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, index+1);

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
	int        	count;
	int*       	indexp;
	int        	tempID;
	int					newCount;

	count = ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

	if (count > 0)
    {
		indexp = new int[count];
		ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), count, indexp);
		for (i = count-1; i >= 0; i--)
			{
			tempID = ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), indexp[i]);
			if (tempID != BLANKID)
				ListBox_DeleteString(GetDlgItem(hDlg, LB_DESTINATION), indexp[i]);
			}

		//If the total count is one then that item must be the blank so select it
		//Get the new count of the items in the list
		newCount = ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));
		if(newCount == 1)
			{
			//-1 means to select everything which is OK since the blank is the
			//only item.
			ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, -1);
			}
		else
			{
			//highlight the item after the last deleted item.
			//We delete count items and we want to place it 1 item after the
			//last selected item so subtract count and add 1 to the index of the
			//last selected item.
			ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, indexp[count-1] - count + 1);
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
  AOBJECTID  objID)
  {
	char        text[MENU_MAX_TEXT+1];
	LRESULT     index;
	LONG        objkey;
  AOBJECTID    tempID;

	if (IsDlgButtonChecked(hDlg, BTN_SHOWMENUS) )
		{
		ListBox_ResetContent(GetDlgItem(hDlg, LB_SOURCE));
    objkey = AOBJ_GETFIRSTKEY;
		// Insert Right Listbox Menus
		while ((tempID = AObjGetNextObject(OTYPE_MENU, &objkey)) != 0 )
			{
			// a menu cannot contain itself nor popup menus
			if ((tempID != objID) && (OMenuIsPopup(tempID) == FALSE))
        {
				AObjGetName(tempID, (LPSTR)text, MENU_MAX_TEXT);
				index =	ListBox_AddString(GetDlgItem(hDlg, LB_SOURCE), text);
				ListBox_SetItemData(GetDlgItem(hDlg, LB_SOURCE), index, tempID);
				}
		  }
    }
	else
		{
		ListBox_ResetContent(GetDlgItem(hDlg, LB_SOURCE));
		objkey = AOBJ_GETFIRSTKEY;
		while ((tempID = AObjGetNextObject(OTYPE_MENUITEM, (LONG  *)&objkey)) != 0 )
			{
			AObjGetName(tempID, (LPSTR)text, MENU_MAX_TEXT);
			index = ListBox_AddString(GetDlgItem(hDlg, LB_SOURCE), text);
			ListBox_SetItemData(GetDlgItem(hDlg, LB_SOURCE), index, tempID);
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
  AOBJECTID  objID)
  {

	ListBox_ResetContent(GetDlgItem(hDlg, LB_SOURCE));
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
  AOBJECTID  objID)
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
			if (pItemEntry[i].id != (AOBJECTID)SEPARATORID)
				{
				AObjGetName(pItemEntry[i].id, text, MENU_MAX_TEXT);
				index = ListBox_AddString(GetDlgItem(hDlg, LB_DESTINATION), text);
				ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), index, pItemEntry[i].id);
				}
			else
				{
				//Don't call InsertSeparator here since the blank item hasn't
				//been added yet.
				index = ListBox_AddString(GetDlgItem(hDlg, LB_DESTINATION), SEPARATORTEXT);
				ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), index, SEPARATORID);
				}
			}
    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
		}

	//Insert a blank item at end of list
	index = ListBox_AddString(GetDlgItem(hDlg, LB_DESTINATION), "");
	ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), index, BLANKID);

	//Set the selection to the blank item
	ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, index);

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
	AOBJECTID*  objidp;
	int*       oldSelection;
	int        selCount = 0;

	cnt = ListBox_GetCount(GetDlgItem(hDlg, cntrlID));

	if (cnt > 0)
    {
	  objidp  = new AOBJECTID[(int)cnt];

		// save objid and delete items
		if(saveSelection)
			{
			selCount = (short)ListBox_GetSelCount(GetDlgItem(hDlg, cntrlID));
			if(selCount)
				{
				oldSelection = (int*)new int[selCount];
				ListBox_GetSelItems(GetDlgItem(hDlg, cntrlID), selCount, oldSelection);
				}
		  }

		for (i = 0; i < cnt; i++)
		  objidp[i] = (AOBJECTID)ListBox_GetItemData(GetDlgItem(hDlg, cntrlID), i);

		ListBox_ResetContent(GetDlgItem(hDlg, cntrlID));

		usingName = IsDlgButtonChecked(hDlg, BTN_DISPLAYNAME);

		if (usingName)
		  {
			// add items with object name
			for (i = 0; i < cnt; i++)
				{
				if (objidp[i] == (AOBJECTID)SEPARATORID)
					{
					index = ListBox_AddString(GetDlgItem(hDlg, cntrlID), SEPARATORTEXT);
					ListBox_SetItemData(GetDlgItem(hDlg, cntrlID), index, objidp[i]);
					}
				else if(objidp[i] != (AOBJECTID)BLANKID)
					{
					AObjGetName(objidp[i], (LPSTR)text, MENU_MAX_TEXT);
					index = ListBox_AddString(GetDlgItem(hDlg, cntrlID), text);
					ListBox_SetItemData(GetDlgItem(hDlg, cntrlID), index, objidp[i]);
					}
				}
		  }
		else
		  {
				// add items with object title
			for (i = 0; i < cnt; i++)
				{
			  if (objidp[i] == (AOBJECTID)SEPARATORID)
			    {
				  index = ListBox_AddString(GetDlgItem(hDlg, cntrlID), SEPARATORTEXT);
				  ListBox_SetItemData(GetDlgItem(hDlg, cntrlID), index, objidp[i]);
					}
			  else if(objidp[i] != (AOBJECTID)BLANKID)
					{
          OMenuGetTitle(objidp[i], text, MENU_MAX_TEXT+1);
					index = ListBox_AddString(GetDlgItem(hDlg, cntrlID), text);
				  ListBox_SetItemData(GetDlgItem(hDlg, cntrlID), index, objidp[i]);
					}
				}
		  }

		delete objidp;

    }

	//if its the left listbox add a blank item
	if(cntrlID == LB_DESTINATION)
		{
		index = ListBox_AddString(GetDlgItem(hDlg, cntrlID), "");
		ListBox_SetItemData(GetDlgItem(hDlg, cntrlID), index, BLANKID);

		//now select the blank item if we're not saving the selection state
		if(!saveSelection)
			ListBox_SetSel(GetDlgItem(hDlg, cntrlID), TRUE, index);
		}

	if(saveSelection && selCount > 0)
		{
		for(i=0;i<selCount;i++)
			{
			ListBox_SetSel(GetDlgItem(hDlg, cntrlID), TRUE, oldSelection[i]);
///				MAKELPARAM(oldSelection[i], 0));
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
  AOBJECTID  objID)
  {
	WORD         cnt;
  WORD         i;
  LPITEMENTRY  pItemEntry;

	cnt = (WORD)ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));

	//subtract 1 from the count since the last item in the list is the
	//blank item;
	cnt--;

  if (cnt > 0)
    {
   	AObjResizeData(objID, MENU_ITEMLIST_DATA, (LONG)(sizeof(ITEMENTRY)*(cnt)));

    pItemEntry = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);

	  for (i = 0; i < cnt; i++)
		  {
		  pItemEntry[i].id = (AOBJECTID)
       ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), i);

		  if ((AOBJECTID)SEPARATORID == pItemEntry[i].id)
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
  AOBJECTID  objID )
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

	index = ComboBox_GetCurSel(GetDlgItem(hDlg, CB_TEXTTO));
	if (index == CB_ERR)
		pMenuInfo->sendText = 0L;
	else
		pMenuInfo->sendText = (AOBJECTID)ComboBox_GetItemData(GetDlgItem(hDlg, CB_TEXTTO), index);

	index = ComboBox_GetCurSel(GetDlgItem(hDlg, CB_INDEXTO));
	if (index == CB_ERR)
		pMenuInfo->sendIndex = 0L;
	else
		pMenuInfo->sendIndex = (AOBJECTID)ComboBox_GetItemData(GetDlgItem(hDlg, CB_INDEXTO), index);

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
  AOBJECTID    objid;

  // "Send Text To" Combo Box
	index =	ComboBox_AddString(GetDlgItem(hDlg, CB_TEXTTO), "(None)");

	ComboBox_SetItemData(GetDlgItem(hDlg, CB_TEXTTO), index, 0L);

  objkey = AOBJ_GETFIRSTKEY;
	while ( (objid = AObjGetNextObject(OTYPE_TEXT, &objkey)) != 0 )
    {
    AObjGetName(objid, (LPSTR)text, MENU_MAX_TEXT);
    index =	ComboBox_InsertString(GetDlgItem(hDlg, CB_TEXTTO), -1, text);
		ComboBox_SetItemData(GetDlgItem(hDlg, CB_TEXTTO), index, objid);
   }

  // "Send Index To" Combo Box

	index =	ComboBox_AddString(GetDlgItem(hDlg, CB_INDEXTO), "(None)");
	ComboBox_SetItemData(GetDlgItem(hDlg, CB_INDEXTO), index, 0L);

  objkey = AOBJ_GETFIRSTKEY;
	while ( (objid = AObjGetNextObject(OTYPE_NUMBER, &objkey)) != 0 )
    {
    AObjGetName(objid, (LPSTR)text, MENU_MAX_TEXT);
		index = ComboBox_InsertString(GetDlgItem(hDlg, CB_INDEXTO), -1, text);
    ComboBox_SetItemData(GetDlgItem(hDlg, CB_INDEXTO), index, objid);
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
  AOBJECTID  cursel)
  {
	WORD      index;
	WORD	    count;
///	LRESULT     count;
	AOBJECTID  obj;

  count = (WORD)ComboBox_GetCount(GetDlgItem(hDlg, cntrlId));

  if (count != 0)
    {
    for (index = 0; index < count; index++)
      {
      obj = (AOBJECTID)ComboBox_GetItemData(GetDlgItem(hDlg, cntrlId), index);
      if (cursel == obj)
		    {
        ComboBox_SetCurSel(GetDlgItem(hDlg, cntrlId), index);
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
  AOBJECTID objID)
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

	Edit_SetSel(GetDlgItem(hDlg, SLE_TITLE), 0, -1);

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
  AOBJECTID  objID)
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
BOOL CALLBACK  EditMenuDialogProc(
  HWND    hDlg,
  UINT    message,
  WPARAM  wParam,
  LPARAM  lParam)
  {
  BOOL      result;
  AOBJECTID  objID;

	switch ( message )
		{
		case WM_INITDIALOG:
			SetWindowLong(hDlg, DWL_USER, lParam);
      InitDialog(hDlg, (AOBJECTID)lParam);
	    AUtlSetTitleFont(OTYPE_MENU, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
  		result = FALSE;
      break;

		case WM_COMMAND: //!!PORT!!
			objID = (AOBJECTID)GetWindowLong(hDlg, DWL_USER);
			switch ( GET_WM_COMMAND_ID(wParam, lParam) )
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
					WinHelp (hDlg, "awusmenu.hlp", HELP_CONTEXT, HELPID_OBJD) ;
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
					if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK)
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
			    if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK)
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
  AOBJECTID  objID)
  {
  long result = A_OK;

	if (TRUE == DialogBoxParam(MODULEINFO::hInstance, "DLGMENU", GetActiveWindow(),
       EditMenuDialogProc, (LPARAM)objID))
    result = A_MODIFIED;

  return result;

  }
