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
// FILE:					BARDLG.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Edit object routines for menu bar
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
#include "barobj.h"
#include "barlib.h"
#include "hmenumgr.h"
#include "menulib.h"
#include "modinfo.h"
#include "helpids.h"

// Bar Dialog
#define BTN_ADD         101
#define BTN_REMOVE      102
#define BTN_EDIT        108

#define BTN_ENABLE      103
#define BTN_DISABLE     104

#define BTN_SHOWNAMES   109
#define BTN_SHOWTITLES  110

#define LB_DESTINATION  105     // Added Menus
#define LB_SOURCE       106

#define IDHELP          999

#define IDD_OBJECT		  201


#define BLANKID         0

#define TEST_WINDOW_HEIGHT 70

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateObject(
  HWND      hDlg,
  AOBJECTID  objID)
  {
	WORD         i;
	WORD         cnt;
	LPBARINFO    pBarInfo;
	LPMENUENTRY  pMenuList;
	RECT		     rect;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);
	// save size of test window
	GetWindowRect(pBarInfo->hTestWindow, &rect);

	pBarInfo->testWinHz = rect.right - rect.left + 1;

	if (TRUE == IsDlgButtonChecked(hDlg, BTN_DISABLE))
    pBarInfo->status |= BAR_DISABLED;
  else
    pBarInfo->status &= (~BAR_DISABLED);

	cnt = (WORD)ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));

	//subtract one off the count since the last item in the list is a blank
	//item.
	cnt--;

	pBarInfo->menuCount = cnt;

	AObjResizeData(objID, BAR_MENULIST_DATA, cnt*sizeof(MENUENTRY));
  	pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);

  if (pMenuList != 0)
    {
  	for (i = 0; i < cnt; i++)
	  {
	  pMenuList[i].id = (AOBJECTID)ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), i);
      pMenuList[i].status = 0;
	  }
    AObjUnlockData(objID, BAR_MENULIST_DATA);
    }

  AObjUnlockData(objID, BAR_INFO_DATA);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateListBoxEntry
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateListBoxEntry(
  HWND  hDlg,
  WORD  ctrlID,
  UINT  index)
  {
  long        isSelected;
  char        text[BAR_MAX_TEXT+1];
  AOBJECTID    menuID;

  menuID =
   (AOBJECTID)ListBox_GetItemData(GetDlgItem(hDlg, ctrlID), index);

  if (menuID != BLANKID)
    {
    isSelected = ListBox_GetSel(GetDlgItem(hDlg, ctrlID), index);

		ListBox_DeleteString(GetDlgItem(hDlg, ctrlID), index);

		if (IsDlgButtonChecked(hDlg, BTN_SHOWTITLES))
			OMenuGetTitle(menuID, text, BAR_MAX_TEXT);
		else
			AObjGetName(menuID, text, BAR_MAX_TEXT);

		ListBox_InsertString(GetDlgItem(hDlg, ctrlID), index, text);

		ListBox_SetItemData(GetDlgItem(hDlg, ctrlID), index, menuID);

    if (isSelected != 0)
			ListBox_SetSel(GetDlgItem(hDlg, ctrlID), TRUE, index);
    }

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateLbText
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateLbText(
  HWND  hDlg,
  WORD  ctrlID)
  {
	UINT        i;
	WORD        cnt;

	cnt = (WORD)ListBox_GetCount(GetDlgItem(hDlg, ctrlID));

	for (i = 0; i < cnt; i++)
    {
    UpdateListBoxEntry(hDlg, ctrlID, i);
	  }

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CreateTestMenu
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static HMENU CreateTestMenu(
  HWND      hDlg,
  AOBJECTID  objID)
  {
  short        count;
  short        i;
  AOBJECTID     menuID;
  HMENU        hMenuBar;
  HMENU        hMenu;
  UINT         flags;
  char         title[BAR_MAX_TEXT+1];
	HMENUMGR*		 hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);

  if (0 == (hMenuBar = hmenuMgr->GetMenuHandle()))
    {
    hMenuBar = CreateMenu();
    hmenuMgr->SetMenuHandle(hMenuBar);

		count = (short)ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));
	  //subtract one off the count since the last item in the list is a blank
	  //item.
	  count--;

	  for (i = 0; i < count; i++)
		  {
		  menuID = (AOBJECTID)ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), i);
      hMenu = MenuGetMenuHandle(menuID);
      OMenuGetTitle(menuID, title, BAR_MAX_TEXT+1);
      if (hMenu != 0)
        {
        flags = MF_POPUP|MF_STRING;
        if (FALSE == OMenuIsEnabled(menuID))
          flags |= MF_GRAYED;
        AppendMenu(hMenuBar, flags, (UINT)hMenu, title);
        }
      }
    }

  hmenuMgr->IncrementUseCount();

  return hMenuBar;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateTestMenu
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateTestMenu(
  HWND      hDlg,
  AOBJECTID  objID)
  {
  LPBARINFO   pBarInfo;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

  SetMenu(pBarInfo->hTestWindow, CreateTestMenu(hDlg, objID));

  DrawMenuBar(pBarInfo->hTestWindow);

  AObjUnlockData(objID, BAR_INFO_DATA);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DestroyTestMenu
//	                                                               
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DestroyTestMenu(
  HWND      hDlg,
  AOBJECTID  objID)
  {
  LPBARINFO    pBarInfo;
  short        count;
  short        i;
  AOBJECTID     menuID;
  HMENU        hMenuBar;
  HMENU        hMenu;
  HMENUMGR*    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);

  hMenuBar = hmenuMgr->GetMenuHandle();

  if (hMenuBar != 0)
    {
    pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

    SetMenu(pBarInfo->hTestWindow, 0);

	  count = (short)ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));

	  //subtract one off the count since the last item in the list is a blank
	  //item.
	  count--;

	  for (i = 0; i < count; i++)
		  {
		  menuID = (AOBJECTID)ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), i);
      hMenu = GetSubMenu(hMenuBar, 0);
      ADBG_ASSERT_E(hMenu!=0, return, "Menu bar DestroyTestMenu: Out of submenus");
      RemoveMenu(hMenuBar, 0, MF_BYPOSITION);
      MenuReleaseMenuHandle(menuID, hMenu);
      }

    if (0 == hmenuMgr->DecrementUseCount())
      {
      hmenuMgr->SetMenuHandle(0);
      DestroyMenu(hMenuBar);
      }


    AObjUnlockData(objID, BAR_INFO_DATA);
    }
  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditObject
//
// DESCRIPTION.:
//						                        
/////////////////////////////////////////////////////////////////////////////
static void EditObject(
  HWND      hDlg,
  AOBJECTID  objID)
  {
	WORD        cnt;
	LRESULT     ret;
	WORD        indexp[1];
  LPBARINFO   pBarInfo;

	cnt = (WORD)ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

	if (cnt == 1)
		{
		ret = ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), cnt, indexp );

		if (ret == 1)
			{
			ret = ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), *indexp);
			if (ret != LB_ERR)
				{
        pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);
				ShowWindow(pBarInfo->hTestWindow, SW_HIDE);
				AObjEdit(AOBJECTID(ret));
				ShowWindow(pBarInfo->hTestWindow, SW_SHOW);
				SetFocus(hDlg);
        AObjUnlockData(objID, BAR_INFO_DATA);
			  }
			}
		}

  DestroyTestMenu(hDlg, objID);
	UpdateTestMenu(hDlg, objID);

	//TRUE means to save the selection state
	UpdateLbText(hDlg, LB_SOURCE);
	UpdateListBoxEntry(hDlg, LB_DESTINATION, *indexp);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InsertObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InsertObject(
  HWND  hDlg)
  {
	BOOL        asTitle;
	char        text[BAR_MAX_TEXT+1];
	int	       	i;
	int		      srcSelectCount;
  int       	insertionIndex;
  int*      	pSrcSelections;
	AOBJECTID    tempID;
  long        ret;

	asTitle = IsDlgButtonChecked(hDlg, BTN_SHOWTITLES);

	srcSelectCount = (WORD)ListBox_GetSelCount(GetDlgItem(hDlg, LB_SOURCE));

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
				OMenuGetTitle(tempID, text, BAR_MAX_TEXT);
				}
			else
				{
				AObjGetName(tempID, text, BAR_MAX_TEXT);
				}

			ListBox_InsertString(GetDlgItem(hDlg, LB_DESTINATION), insertionIndex, text);
			ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), insertionIndex, tempID);
      insertionIndex++;
			}

		ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, insertionIndex);

		delete pSrcSelections;

    }

  }

////////////////////////////////////////////////////////////////////////////
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
	LONG        ret;
	WORD				newCount;

	count = (WORD) ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

	if (count == 0)
		return;

	indexp = new WORD[count];

	ret = ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), count, indexp);

		// This should never happen
	if (ret == LB_ERR)
		return;

		// must delete backwards
	for (i = count-1; i >= 0; i--)
		{
		ret = ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), indexp[i]);
			//make sure not to delete the blank item
		if (ret != BLANKID)
			{
			ListBox_DeleteString(GetDlgItem(hDlg, LB_DESTINATION), indexp[i]);
			}
		}

		//If the total count is one then that item must be the blank so select it
	newCount = (WORD)ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));

	if (newCount == 1)
		{
			//-1 means to select everything which is OK since the blank is the
			//only item.
			ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, -1);
		}
	else
		{
			//highlight the item after the last deleted item
			//We delete count items and we want to place it 1 item after the
			//last selected item so subtract count and add 1 to the index of the
			//last selected item.
		ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, indexp[count-1] -count + 1);
		}

	delete indexp;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateAddButton
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateAddButton(
  HWND  hDlg)
  {
	LRESULT  cnt;

	cnt = ListBox_GetSelCount(GetDlgItem(hDlg, LB_SOURCE));

	if (cnt)
		EnableWindow(GetDlgItem(hDlg, BTN_ADD), TRUE);
	else
		EnableWindow(GetDlgItem(hDlg, BTN_ADD), FALSE);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateRemoveButton
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateRemoveButton(
  HWND  hDlg)
  {
  HWND     hButton;
	LRESULT  selcnt;
	UINT     cnt;
  BOOL     enableFlag;

	selcnt = ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

  hButton = GetDlgItem(hDlg, BTN_REMOVE);

	if(selcnt == 1)
		{
		//make sure it's not the blank item which is selected
    cnt = (UINT)ListBox_GetCount(GetDlgItem(hDlg, LB_DESTINATION));
		if (0 == ListBox_GetSel(GetDlgItem(hDlg, LB_DESTINATION), cnt-1))
 		  enableFlag = TRUE;
    else
      enableFlag = FALSE;
    }
	else if (selcnt > 1)
    {
		enableFlag = TRUE;
    }
	else
    {
		enableFlag = FALSE;
    }

	EnableWindow(hButton, enableFlag);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateEditButton
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateEditButton(
  HWND  hDlg)
	{
  HWND     hEditButton;
	LRESULT  cnt;
	int      index;
	long     ret;

	cnt = ListBox_GetSelCount(GetDlgItem(hDlg, LB_DESTINATION));

  hEditButton = GetDlgItem(hDlg, BTN_EDIT);

	if (cnt == 1)
		{
		ListBox_GetSelItems(GetDlgItem(hDlg, LB_DESTINATION), 1, &index);

		ret = ListBox_GetItemData(GetDlgItem(hDlg, LB_DESTINATION), index);
		if (ret == BLANKID)
			EnableWindow(hEditButton, FALSE);
		else
			EnableWindow(hEditButton, TRUE);
		}
	else
    {
		EnableWindow(hEditButton, FALSE);
    }

	}


////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DestroyTestWindow
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DestroyTestWindow(
  HWND      hDlg,
  AOBJECTID  objID)
  {
  LPBARINFO   pBarInfo;

	DestroyTestMenu(hDlg, objID);

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	DestroyWindow(pBarInfo->hTestWindow);

  AObjUnlockData(objID, BAR_INFO_DATA);

  }


////////////////////////////////////////////////////////////////////////////
// FUNCTION....: TestWndProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
LONG CALLBACK TestWndProc(HWND hwnd, WORD msg, WORD wParam, LONG lParam)
  {

	switch(msg)
		{
		case WM_SIZE:
					return 0;
		}

	return DefWindowProc(hwnd, msg, wParam, lParam);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RegisterTestWindow
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL RegisterTestWindow(
  void)
  {
	WNDCLASS wndclass;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC)TestWndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = MODULEINFO::hInstance;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = MODULEINFO::testWndClassName;

	if (!RegisterClass (&wndclass))
    return FALSE;
  else
    return TRUE;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CreateTestWindow
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void CreateTestWindow(
  HWND      hDlg,
  AOBJECTID  objID)
  {
  static BOOL classIsRegistered;
	RECT	      rect;
  LPBARINFO   pBarInfo;

  if (!classIsRegistered)
    classIsRegistered = RegisterTestWindow();

  ADBG_ASSERT_E(classIsRegistered == TRUE, return,
    "Register of Menu Test Window Class failed");

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	GetWindowRect(hDlg, &rect);

	pBarInfo->hTestWindow = CreateWindow(
            MODULEINFO::testWndClassName,    // window class name
					  "Test Menu Bar",                 // window caption
						WS_OVERLAPPED | WS_CAPTION |
						WS_THICKFRAME,                   // window style
						rect.left,                       // initial x position
			      rect.top-90,                     // initial y position
			      pBarInfo->testWinHz,             // initial x size
						TEST_WINDOW_HEIGHT,              // initial y size
						hDlg,                            // parent window handle
						0    ,                           // window menu handle
						MODULEINFO::hInstance,           // program instance handle
						NULL);                           // creation parameters

  SetMenu(pBarInfo->hTestWindow, CreateTestMenu(hDlg, objID));

  ShowWindow(pBarInfo->hTestWindow, SW_SHOW);

  UpdateWindow(pBarInfo->hTestWindow);

  DrawMenuBar(pBarInfo->hTestWindow);

  AObjUnlockData(objID, BAR_INFO_DATA);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitObjectList
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InitObjectList(
  HWND      hDlg,
  AOBJECTID  objID)
  {
	char          text[BAR_MAX_TEXT+1];
	int           index;
	LONG          objkey;
	AOBJECTID      menuID;
  LPMENUENTRY   pMenuList;
  LPBARINFO     pBarInfo;
  short         i;

	objkey = AOBJ_GETFIRSTKEY;

		// Insert Right Listbox Items
	while ((menuID = AObjGetNextObject(OTYPE_MENU, &objkey)) != 0)
		{
		if(FALSE == OMenuIsPopup(menuID))
			{
			AObjGetName(menuID, text, BAR_MAX_TEXT);
			index = (int)ListBox_AddString(GetDlgItem(hDlg, LB_SOURCE), text);
			ListBox_SetItemData(GetDlgItem(hDlg, LB_SOURCE), index, menuID);
			}
		}

	// Insert Left ListBox Items

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

  // Cleanup, by deleting all menu items that are now popup menus
  pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
	for (i = 0; i < pBarInfo->menuCount; i++)
 	 {
	//mark popups to be deleted from the bars list
 	 if (TRUE == OMenuIsPopup(pMenuList[i].id))
 	  {
       pMenuList[i].status |= ITEMENTRY_DELETEENTRY;
 	  }
 	 }
 
  pBarInfo->menuCount -=
   DeleteMenuEntries(pMenuList, pBarInfo->menuCount); // do the actual deleting
  if (pMenuList)
    AObjUnlockData(objID, BAR_MENULIST_DATA);
  AObjResizeData(objID, BAR_MENULIST_DATA,
   pBarInfo->menuCount*sizeof(MENUENTRY));
  // Cleanup ...   
 
 	if (pBarInfo->menuCount > 0)
		{
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
		for (i = 0; i < pBarInfo->menuCount; i++)
			 {
			 AObjGetName(pMenuList[i].id, text, BAR_MAX_TEXT);
			 index = (int)ListBox_AddString(GetDlgItem(hDlg, LB_DESTINATION), text);
			 ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), index, pMenuList[i].id);
 			 }
    if (pMenuList)
      AObjUnlockData(objID, BAR_MENULIST_DATA);
		}

  AObjUnlockData(objID, BAR_INFO_DATA);

	//Insert and select a blank item
	index = (UINT)ListBox_AddString(GetDlgItem(hDlg, LB_DESTINATION), "");
	ListBox_SetItemData(GetDlgItem(hDlg, LB_DESTINATION), index, BLANKID);
	ListBox_SetSel(GetDlgItem(hDlg, LB_DESTINATION), TRUE, index);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDlgControl
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InitDlgControl(
  HWND      hDlg,
  AOBJECTID  objID)
  {
	RECT 	     rect;
	char 	     objName[BAR_MAX_TEXT];
  LPBARINFO  pBarInfo;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	AObjGetName(objID, objName, BAR_MAX_TEXT);

	SetWindowText(hDlg, objName);

	GetWindowRect(hDlg, &rect);

	pBarInfo->testWinHz = rect.right - rect.left + 1;

		// Set check box and radio button

  CheckRadioButton(hDlg, BTN_SHOWNAMES, BTN_SHOWTITLES, BTN_SHOWNAMES);

	if ((pBarInfo->status & BAR_DISABLED) == BAR_DISABLED)
	  CheckRadioButton(hDlg, BTN_ENABLE, BTN_DISABLE, BTN_DISABLE);
  else
	  CheckRadioButton(hDlg, BTN_ENABLE, BTN_DISABLE, BTN_ENABLE);

  AObjUnlockData(objID, BAR_INFO_DATA);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDialog
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL InitDialog(
  HWND      hDlg,
  AOBJECTID  objID)
  {

	InitDlgControl(hDlg, objID);

  InitObjectList(hDlg, objID);

	DestroyTestWindow(hDlg, objID);	 

  CreateTestWindow(hDlg, objID);

  UpdateAddButton(hDlg);

	UpdateRemoveButton(hDlg);

	UpdateEditButton(hDlg);

  //return false because we do a set focus
  return FALSE;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BAROBJ::MenuBarDlg
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK MenuBarDlgProc(
  HWND 		hDlg,
  UINT 		message,
  WPARAM 	wParam,
  LPARAM 	lParam)
  {
  AOBJECTID  objID;
  BOOL      result;

  switch(message)
    {
    case WM_INITDIALOG:
		  AUtlSetTitleFont(OTYPE_MENUBAR, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
      SetWindowLong(hDlg, DWL_USER, lParam);
      result = InitDialog(hDlg, (AOBJECTID)lParam);
      break;

	  case WM_COMMAND: 
      objID = (AOBJECTID)GetWindowLong(hDlg, DWL_USER);
      switch(GET_WM_COMMAND_ID(wParam, lParam))
        {
				case IDOK:
			    SetFocus(GetDlgItem(hDlg, IDOK));
			    UpdateObject(hDlg, objID);
			    DestroyTestWindow(hDlg, objID);
			    EndDialog(hDlg, (int)A_MODIFIED);
			    result = TRUE;
          break;

        case IDCANCEL:
			    SetFocus(GetDlgItem(hDlg, IDCANCEL));
			    DestroyTestWindow(hDlg, objID);
					EndDialog(hDlg, (int)A_OK);
			    result = TRUE;
          break;

		    case IDHELP:
					WinHelp (hDlg, "awusmenu.hlp", HELP_CONTEXT, HELPID_OBJD + 1) ;
		    	result = TRUE;
          break;

				case BTN_SHOWTITLES:
				case BTN_SHOWNAMES:
						//TRUE means to save the selection state
					UpdateLbText(hDlg, LB_SOURCE);
					UpdateLbText(hDlg, LB_DESTINATION);
			    result  = TRUE;
          break;

				case BTN_ADD:
          DestroyTestMenu(hDlg, objID);
					InsertObject(hDlg);
					UpdateAddButton(hDlg);
					UpdateTestMenu(hDlg, objID);
			    result = TRUE;
          break;

				case BTN_REMOVE:
          DestroyTestMenu(hDlg, objID);
					DeleteObjects(hDlg);
					UpdateEditButton(hDlg);
					UpdateRemoveButton(hDlg);
					UpdateTestMenu(hDlg, objID);
			    result = TRUE;
          break;

				case BTN_EDIT:
					EditObject(hDlg, objID);
			    result = TRUE;
          break;

				case LB_SOURCE:
					UpdateAddButton(hDlg);
					if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK)
						{
            DestroyTestMenu(hDlg, objID);
						InsertObject(hDlg);
				    UpdateTestMenu(hDlg, objID);
						}
			    result = TRUE;
          break;

				case LB_DESTINATION:
					UpdateRemoveButton(hDlg);
					UpdateEditButton(hDlg);
					if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK)
						{
						EditObject(hDlg, objID);
						}
			    result = TRUE;
          break;

        default:
          result = FALSE;
          break;

  			}
		    break;

    default:
      result = FALSE;
      break;

      }


	return result;

  }


////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditMenuBarObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
long EditMenuBarObject(
  AOBJECTID theObject)
  {
  long result;

  result = DialogBoxParam(MODULEINFO::hInstance, "DLGBAR",
   GetActiveWindow(), (DLGPROC)MenuBarDlgProc, (LPARAM)theObject);

  return result;

  }
