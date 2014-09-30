/*//////////////////////////////////////////////////////////////
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
// FILE:	dt_edit.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Edit date Dialog function
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "dt_incld.h"
#include "dt_dialg.h"

LRESULT far pascal _export DateEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*****************************************************/

LONG EditDateObject(OBJECTID  oi)
{
	BOOL	bEdited;

	bEdited = DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(DATE_SETUP_DIALOG),	GetActiveWindow(),	(DLGPROC)EditDate::DateEditDlg, (LONG) oi);

	if (bEdited)
		return A_MODIFIED;
	else
  	return A_OK;
}

/*****************************************************/
BOOL WINAPI _export EditDate::DateEditDlg(HWND  hwnd, UINT msg, WPARAM wParam,	LPARAM lParam )
{
	LPEditDate   editdate;
	int          nc;
	char         tmpStr[80];

	switch(msg)
	{
		case WM_INITDIALOG:
			editdate = new EditDate( (OBJECTID) lParam );
			SetWindowLong(hwnd, DWL_USER, (long)editdate);
			editdate->SetWindowHandle(hwnd);
			editdate->InitDialog();
			SetFocus( GetDlgItem(hwnd, ED_TITLE) );
			SendDlgItemMessage(hwnd, ED_TITLE, EM_SETSEL, 0, MAKELONG(0, -1));
			AUtlSetTitleFont(OTYPE_DATE, NULL, GetDlgItem(hwnd, ST_OBJECTNAME) );
			AUtlCenterDialog(hwnd, 0);
			return FALSE;

		case WM_DESTROY:
			editdate = (LPEditDate) GetWindowLong(hwnd, DWL_USER);
			delete editdate;
			return FALSE;

		case WM_COMMAND:
			editdate = (LPEditDate) GetWindowLong(hwnd, DWL_USER);
			nc = HIWORD(lParam);
			switch(wParam)
			{
				case RB_SHORTDATE:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
					{
						editdate->SetIsShortDate(TRUE);
						editdate->EnableProperControls();
          }
					break;

				case RB_LONGDATE:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
					{
						editdate->SetIsShortDate(FALSE);
						editdate->EnableProperControls();
          }
					break;

  			case RB_EDITABLE:
					if (nc == BN_CLICKED)
          {
						editdate->SetIsEditable(TRUE);
						editdate->SetIsKeepCurrent(FALSE);
						editdate->EnableProperControls();
          }
					break;

				case RB_KEEPCURRENT:
					if (nc == BN_CLICKED)
          {
						editdate->SetIsKeepCurrent(TRUE);
						editdate->SetIsEditable(FALSE);
						editdate->EnableProperControls();
          }
					break;

				case RB_NEITHER:
					if (nc == BN_CLICKED)
          {
						editdate->SetIsKeepCurrent(FALSE);
						editdate->SetIsEditable(FALSE);
						editdate->EnableProperControls();
          }
					break;

				case CB_SELECTALL:
					if (nc == BN_CLICKED)
         	{
						editdate->SetSelectsAll((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						editdate->EnableProperControls();
          }
					break;

				case CB_FRAME:
					if (nc == BN_CLICKED)
          {
						editdate->SetHasFrame((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						editdate->EnableProperControls();
          }
					break;

				case CB_ACTIVATABLE:
					if (nc == BN_CLICKED)
          {
						editdate->SetIsActivatable((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						editdate->EnableProperControls();
          }
					break;

				case CB_DISPLAYEMPTY:
					if (nc == BN_CLICKED)
					{
						editdate->SetDisplaysEmpty((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						editdate->EnableProperControls();
          }
					break;

				case ED_TITLE:
					if (nc == EN_KILLFOCUS)
          {
						if (editdate->IsShortDate())
							editdate->m_date = GetDlgEditShortDate(hwnd, ED_TITLE);
						else
							editdate->m_date = GetDlgEditLongDate(hwnd, ED_TITLE);
					}
					break;

				case PB_OK:  //cause title and format to get a killfocus..
					SetFocus((HWND)LOWORD(lParam)); 
					editdate->StoreAobj();
					EndDialog(hwnd, TRUE);
					break;

				case PB_CANCEL:
					EndDialog(hwnd, FALSE);
					break;

				case PB_FONTS:
					GetDlgItemText(hwnd, ED_TITLE, tmpStr, sizeof(tmpStr));
					editdate->EditFont(hwnd, tmpStr);
					break;

				case PB_HELP:
					//WinHelp(hwnd, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Text);
					break;
				}
			break;
		}

	return FALSE;
}

void EditDate::EnableProperControls()
{
	char tmpStr[80];

	EnableWindow(GetDlgItem(m_hwndDlg, CB_FRAME), IsActivatable() );
	EnableWindow(GetDlgItem(m_hwndDlg, CB_DISPLAYEMPTY), !IsKeepCurrent() );
	ShowWindow(GetDlgItem(m_hwndDlg, CB_SELECTALL), IsEditable());
	ShowWindow(GetDlgItem(m_hwndDlg, CB_CANACTIVATE), !IsEditable());

	if (IsShortDate())
	{
		FormatShortDate(m_date, tmpStr, sizeof(tmpStr) );
		SetDlgItemText(m_hwndDlg, RB_SHORTDATE, tmpStr);
		SetDlgItemText(m_hwndDlg, ED_TITLE, tmpStr);
	}
	else
	{
		FormatLongDate(m_date, tmpStr, sizeof(tmpStr) );
		SetDlgItemText(m_hwndDlg, RB_LONGDATE, tmpStr);
		SetDlgItemText(m_hwndDlg, ED_TITLE, tmpStr);
	}	

}

static BOOL IsButtonReallyChecked(
	HWND 										hwndDlg,
	int											idItem)
	{
	if (!IsWindowEnabled(GetDlgItem(hwndDlg, idItem)) )
		return FALSE;

	return (SendMessage(GetDlgItem(hwndDlg, idItem), BM_GETCHECK, 0, 0L) == 1);
	}

void EditDate::StoreAobj()
{
	LPDate dateSrc;

	SetSelectsAll(IsButtonReallyChecked(m_hwndDlg, CB_SELECTALL));
	SetHasFrame(IsButtonReallyChecked(m_hwndDlg, CB_FRAME));
	SetIsActivatable(IsButtonReallyChecked(m_hwndDlg, CB_ACTIVATABLE));
	SetDisplaysEmpty(IsButtonReallyChecked(m_hwndDlg, CB_DISPLAYEMPTY));

	dateSrc = LockDateObject(m_oiSelf);
	dateSrc->m_version 				= m_version;
	dateSrc->m_oiSelf 				= m_oiSelf;
	dateSrc->m_style 					= m_style;
	dateSrc->m_date           = m_date;
	_fmemcpy(&dateSrc->m_attributes, &m_attributes, sizeof(ATTRIBUTES) );

	dateSrc->FormatDate();

	UnlockDateObject(m_oiSelf);
}


void EditDate::InitDialog()
{
	char tmpStr[80];
	char 	objectName[80];

	AObjGetName(m_oiSelf, objectName, 80);
	SetWindowText(m_hwndDlg, objectName);

	if (IsShortDate()) 
	  SendDlgItemMessage(m_hwndDlg, RB_SHORTDATE, BM_SETCHECK, 1, 0L);
	else
	  SendDlgItemMessage(m_hwndDlg, RB_LONGDATE, BM_SETCHECK, 1, 0L);

	if (HasFrame())
		SendDlgItemMessage(m_hwndDlg, CB_FRAME, BM_SETCHECK, 1, 0L);
	if (SelectsAll())        		
		SendDlgItemMessage(m_hwndDlg, CB_SELECTALL, BM_SETCHECK, 1, 0L);
	if (IsActivatable())      	
		SendDlgItemMessage(m_hwndDlg, CB_ACTIVATABLE, BM_SETCHECK, 1, 0L);
	if (DisplaysEmpty())				
		SendDlgItemMessage(m_hwndDlg, CB_DISPLAYEMPTY, BM_SETCHECK, 1, 0L);
	if (IsEditable())						
		SendDlgItemMessage(m_hwndDlg, RB_EDITABLE, BM_SETCHECK, 1, 0L);
	else if(IsKeepCurrent())
		SendDlgItemMessage(m_hwndDlg, RB_KEEPCURRENT, BM_SETCHECK, 1, 0L);
	else
		SendDlgItemMessage(m_hwndDlg, RB_NEITHER, BM_SETCHECK, 1, 0L);

	FormatShortDate(m_date, tmpStr, sizeof(tmpStr) );
	SetDlgItemText(m_hwndDlg, RB_SHORTDATE, tmpStr);
	SetDlgItemText(m_hwndDlg, ED_TITLE, tmpStr);

	FormatLongDate(m_date, tmpStr, sizeof(tmpStr) );
	SetDlgItemText(m_hwndDlg, RB_LONGDATE, tmpStr);

	EnableProperControls();
};


EditDate::EditDate(OBJECTID	oi):Date(oi, FALSE)
{
	LPDate dateSrc;

	dateSrc = LockDateObject(oi);

	m_version	= dateSrc->m_version;
	m_oiSelf = dateSrc->m_oiSelf;
	m_style	= dateSrc->m_style;
	m_date = dateSrc->m_date;	
				
	_fmemcpy(&m_attributes, &dateSrc->m_attributes, sizeof(ATTRIBUTES) );

	UnlockDateObject(oi);

};

EditDate::~EditDate()
{
}

