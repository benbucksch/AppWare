//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////

#define		STRICT
#define		ASTRICT


#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <commdlg.h>


#include "a_alm.h"
#include "a_almutl.h"
#include "helpids.h"
#include "rad.h"
#include "dialogs.h"
#include "globals.h"
#include "o_button.h"

#define TMP_BUFFER_SIZE		40
///////////////////////////////////////////////////////

long EditRadioObject(
	AOBJECTID            id)
	{
	// FARPROC							proc = MakeProcInstance( (FARPROC)Rad_SetupDlg, (HINSTANCE)hInstance);
	int									i;
  long 								rVal;
	i = DialogBoxParam(
				(HINSTANCE)hInstance,
				MAKEINTRESOURCE(RADIO_SETUP_DIALOG),
				GetActiveWindow(),
				(DLGPROC)Rad_SetupDlg,
				(LONG) id);

	// FreeProcInstance(proc);
	if (i)
		rVal = A_MODIFIED;
	else if (i == -1)
		rVal = A_ERROR;
  else rVal = A_OK;
	return rVal;
	}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

LRESULT CALLBACK Rad_SetupDlg(
	HWND                    hWnd,
	UINT                    msg,
	WPARAM                  wParam,
	LPARAM                  lParam )
	{
	LPEDIT_RADIO						er;
	int											i, ni, nc;
	AOBJECTID								object;
	LPINT										intArray;
	char										objectName[80];

	switch(msg)
		{
		case WM_INITDIALOG:
			er = new EDIT_RADIO( (AOBJECTID) lParam);
			SetDialogPtr1(hWnd, (LPVOID) er);
			er->hWndDlg = hWnd;
			er->InitDialog();

			AObjGetName((AOBJECTID) lParam, objectName, 80);
			SetWindowText(hWnd, objectName);

			ATypeGetName(OTYPE_RADIOGROUP, objectName, 30);
			AUtlSetTitleFont(OTYPE_RADIOGROUP,	objectName,	GetDlgItem(hWnd, ST_OBJECTNAME));
			AUtlCenterDialog(hWnd, 0);

			return TRUE;

		case WM_DESTROY:
			er = (LPEDIT_RADIO)GetDialogPtr1(hWnd);
			delete er;
			RemoveDialogPtr1(hWnd);
			return FALSE;

		case WM_COMMAND: 
			nc = GET_WM_COMMAND_CMD(wParam, lParam);	// notification command
			er = (LPEDIT_RADIO)GetDialogPtr1(hWnd);
			switch(GET_WM_COMMAND_ID(wParam, lParam))
				{
				case LB_ITEMS:
					if (nc != LBN_DBLCLK)
						{
						er->ListsChanged();
						break;
						}
					//else fall through
				case PB_EDIT:
					SendDlgItemMessage(hWnd, LB_ITEMS, LB_GETSELITEMS, 1, (LONG)&i);
					object = (AOBJECTID) SendDlgItemMessage(hWnd, LB_ITEMS, LB_GETITEMDATA, i, 0L);
					AObjEdit(object);
					er->ListsChanged();
					break;

				case LB_OBJECTS:
					if (nc != LBN_DBLCLK)
						{
						er->ListsChanged();
						break;
            }
					//Fall through to PB_ADD

				case PB_ADD:
					ni = (int)SendDlgItemMessage(hWnd, LB_OBJECTS, LB_GETSELCOUNT, 0, 0);
					intArray = (LPINT) farmalloc(sizeof(int)*ni);
					SendDlgItemMessage(hWnd, LB_OBJECTS, LB_GETSELITEMS, ni, (LONG)intArray);
					for(i=0; i<ni; i++)
						{
						object = (AOBJECTID) SendDlgItemMessage(hWnd, LB_OBJECTS, LB_GETITEMDATA, intArray[i], 0L);
						er->AddItem(object);
						}
					farfree(intArray);
					er->FillItemsList();
					er->ListsChanged();
					break;

				case PB_REMOVE:
					ni =(int) SendDlgItemMessage(hWnd, LB_ITEMS, LB_GETSELCOUNT, 0, 0);
					intArray = (LPINT) farmalloc(sizeof(int)*ni);
					SendDlgItemMessage(hWnd, LB_ITEMS, LB_GETSELITEMS, ni, (LONG)intArray);
					for(i=ni-1; i>=0; i--)
						er->RemoveItem(intArray[i]);
					farfree(intArray);
					er->FillItemsList();
					er->ListsChanged();
					break;

				case PB_OK:
					er->Store();
					EndDialog(hWnd, TRUE);
					break;
				case PB_CANCEL:
					EndDialog(hWnd, FALSE);
					break;

				case IDHELP:
					WinHelp(hWnd, "awusradg.hlp", HELP_CONTEXT, HELPID_OBJD);
					break;
				}
			break;
		}

	return FALSE;
	}

/////////////////////////////////////////////
EDIT_RADIO::EDIT_RADIO(
	AOBJECTID 	id)
	{
	LONG			size = AObjGetDataSize(id, RADIO_ITEMS_INFO);
	LPSTR     lpItems;
	numItems = (int)size/sizeof(AOBJECTID);
	itemsMID = AMemAllocate(numItems*sizeof(AOBJECTID));
	items = (LPOBJECTID)AMemLock(itemsMID);
	lpItems = (LPSTR) AObjLockData(id, RADIO_ITEMS_INFO);
	if (lpItems)
    {
		CopyMemory(items,(LPSTR) lpItems, (int)size);
		AObjUnlockData(id, RADIO_ITEMS_INFO);
		}
	AMemUnlock(itemsMID);
	editID = id;
	}

////////////////////////////////////////////

EDIT_RADIO::~EDIT_RADIO()
	{
	AMemFree(itemsMID);
	}

/////////////////////////////////////////////

void EDIT_RADIO::InitDialog()
	{
	FillObjectsList();
	FillItemsList();
	ListsChanged();
	}


void EDIT_RADIO::ListsChanged()
	{

	int				itemSelCount;

	itemSelCount = (int)SendDlgItemMessage(hWndDlg, LB_ITEMS, LB_GETSELCOUNT, 0, 0);

	EnableWindow( GetDlgItem(hWndDlg, PB_EDIT), itemSelCount == 1 ? TRUE:FALSE);
	EnableWindow( GetDlgItem(hWndDlg, PB_REMOVE), itemSelCount ? TRUE:FALSE);

	EnableWindow( GetDlgItem(hWndDlg, PB_ADD),
		SendDlgItemMessage(hWndDlg, LB_OBJECTS, LB_GETSELCOUNT, 0, 0) ? TRUE:FALSE);
  }

/////////////////////////////////////////////

BOOL EDIT_RADIO::AddItem(
	AOBJECTID				id)
	{
	items = (LPOBJECTID)AMemLock(itemsMID);
	if (Object_in_items_array(id, items, numItems) == -1)
		{
		items = (LPOBJECTID)AMemUnlock(itemsMID);
		AMemSetSize(itemsMID, (numItems+1)*sizeof(AOBJECTID) );
		items =(LPOBJECTID) AMemLock(itemsMID);
		items[numItems] = id;
		numItems++;
		}
	AMemUnlock(itemsMID);
	return TRUE;
	}

/////////////////////////////////////////////

BOOL EDIT_RADIO::RemoveItem(
	int							idx)
	{
	numItems--;
	items = (LPOBJECTID)AMemLock(itemsMID);
	EraseItemFromArray(idx, sizeof(AOBJECTID), numItems, items);
	AMemUnlock(itemsMID);
	AMemSetSize(itemsMID, sizeof(AOBJECTID) * numItems);
	// items = (LPOBJECTID) AMemLock(itemsMID);
	return TRUE;
	}

/////////////////////////////////////////////

void EDIT_RADIO::Store()
	{
	items = (LPOBJECTID)AMemLock(itemsMID);
	AObjSetData(editID, RADIO_ITEMS_INFO, items, sizeof(AOBJECTID) * numItems);
	AMemUnlock(itemsMID);
	}

/////////////////////////////////////////////

void EDIT_RADIO::FillObjectsList()
	{
	AOBJECTID  		object;
	LONG      		cookie = AOBJ_GETFIRSTKEY;
	int       		i = 0;
	char					tmpBuffer[TMP_BUFFER_SIZE+1];

	SendDlgItemMessage(hWndDlg, LB_OBJECTS, LB_RESETCONTENT, 0, 0L);
	while ( (object = AObjGetNextObject((ATYPEID)OTYPE_BUTTON, &cookie)) != 0)
		{
		if (OButnGetType(object) == OBUTN_RADIO)
			{
			AObjGetName(object, tmpBuffer, TMP_BUFFER_SIZE);
			SendDlgItemMessage(hWndDlg, LB_OBJECTS, LB_ADDSTRING, 0, (LONG)tmpBuffer);
			SendDlgItemMessage(hWndDlg, LB_OBJECTS, LB_SETITEMDATA, i, (LONG)object);
			i++;
			}
		}

	}

/////////////////////////////////////////////

void EDIT_RADIO::FillItemsList()
	{
	char                    tmpBuffer[TMP_BUFFER_SIZE+1];
	int                     i;

	SendDlgItemMessage(hWndDlg, LB_ITEMS, LB_RESETCONTENT, 0, 0L);
	items = (LPOBJECTID)AMemLock(itemsMID);
	for(i=0; i<numItems; i++)
		{
		AObjGetName(items[i], tmpBuffer, TMP_BUFFER_SIZE);
		SendDlgItemMessage(hWndDlg, LB_ITEMS, LB_ADDSTRING, 0, (LONG)tmpBuffer);
		SendDlgItemMessage(hWndDlg, LB_ITEMS, LB_SETITEMDATA, i, (LONG)items[i]);
		}
	items = (LPOBJECTID)AMemUnlock(itemsMID);

	}

/////////////////////////////////////////////










