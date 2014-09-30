///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// dialog.cpp
// Text Builder
// Dialog Box Routines
//
// Robert Lundahl/ Edgar C. Jerez / Russ Graves
//
// Start Date 05/28/92
// End   Date 06/../92
///////////////////////////////////////////////////////////////

#include "tf_incld.h"


#define OBJECTNAMESIZE		35
#define IDD_OBJECT				201

#define	HELPFILE					"awustfct.hlp"

#define TFOK							1
#define TFCANCEL					2
#define TFHELP						998
#define TFOBJLIST     		101
#define TFADD         		102
#define TFREMOVE      		103
#define TFEDIT        		104
#define TFOBJVAR      		105
#define TFRESULT      		106
#define TFEXPRESS	  			108
#define TFALIAS       		109
#define TFTYPE		  			110
#define TFAUTO        		111
#define TFALIASTITLE  		112

extern GLOBAL vG;       // Global Container Class

long	GetLiteral ( STRING* strPtr, char*& space, WORD& length );
long 	GetCharacter ( char*& space );
int 	GetObject ( STRING* strPtr, char*& space );

//int 	latoi ( BYTE* s );
//void	Underscore ( char* p );
void 	SkipSpace ( char*& p );
void 	TrimSpace ( char* p );


///////////////////////////////////////////////////////////////
// TEXTFACT::TfDialog: Text Builder Dialog Box
//
///////////////////////////////////////////////////////////////

BOOL CALLBACK TEXTFACT::TfDialog ( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	TEXTFACT*	tf;

	switch ( message )
	{
	case WM_INITDIALOG:
		// subclass the edit control to limit input to alpha-numeric chars
		vG.lpfnOldEditControlProc = (FARPROC)GetWindowLong(GetDlgItem(hDlg, TFALIAS), GWL_WNDPROC);
		SetWindowLong(GetDlgItem(hDlg, TFALIAS), GWL_WNDPROC, (long)TEXTFACT::NewEditControlProc);
		SetProp(GetDlgItem(hDlg, TFALIAS), "Shift", FALSE);

		SetWindowLong(hDlg, vG.cbOffset, lParam);

    tf = (TEXTFACT*)lParam;

    tf->SetDialog(hDlg);

		if (!tf->SaveDatas())
    {
			EndDialog(hDlg, 0);
			return TRUE;
		}

		tf->InitDlgControl();
  	//AUtlSetTitleFont(OTYPE_TEXTMAKER, NULL, GetDlgItem(hDlg, IDD_OBJECT));
		AUtlCenterDialog(hDlg, 0);
		return TRUE;

	case WM_COMMAND: 
		tf = (TEXTFACT*)GetWindowLong(hDlg, vG.cbOffset);

		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case TFOK:
			SetFocus(GetDlgItem(hDlg, TFOK));
      if (!tf->CheckDuplicate())
      {
				tf->UpdateObject();
       	if (!vG.parseError)
				{
					tf->RestoreDatas(FALSE);
					tf->DeleteListData();
					EndDialog(hDlg, TRUE);
				}
      }
			return TRUE;

		case TFCANCEL:
			SetFocus(GetDlgItem(hDlg, TFCANCEL));
			tf->RestoreDatas(TRUE);
			tf->DeleteListData();
			EndDialog(hDlg, FALSE);
			return TRUE;

		case TFHELP:
			WinHelp(hDlg, "awustfct.hlp", HELP_CONTEXT, HELPID_OBJD);
			return TRUE;

		case TFADD:
			tf->Add();
			return TRUE;

		case TFREMOVE:
			tf->Remove();
			tf->UpdateRemoveButton();
			tf->UpdateEditButton();
			tf->UpdateAliasControl();
			return TRUE;

		case TFEDIT:
			tf->Edit();
			return TRUE;

		case TFALIAS:
			if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_KILLFOCUS)
				tf->Alias();
			return TRUE;

		case TFEXPRESS:
//			if (HIWORD(lParam) == EN_CHANGE);  // Check this at end dialog OK
			return TRUE;

		case TFOBJLIST:
			if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_SELCHANGE)
				tf->UpdateAddButton();

			if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK)
				tf->Add();
			return TRUE;

		case TFOBJVAR:
			if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_SELCHANGE)
			{
				tf->UpdateRemoveButton();
				tf->UpdateEditButton();
				tf->UpdateAliasControl();
			}

			if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK)
				tf->Edit();
			return TRUE;

		case TFTYPE:
			if (GET_WM_COMMAND_CMD(wParam, lParam) == CBN_SELCHANGE)
			{
				tf->FillType();
				tf->UpdateAddButton();
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}				// TEXTFACT::TfDialog...


///////////////////////////////////////////////////////////////
// TEXTFACT::InitDlgControl: Text Builder init Dialog Box
//
///////////////////////////////////////////////////////////////

void TEXTFACT::InitDlgControl ( void )
{
	char		name[maxEdit+1];
	char		szType[256];
//	BYTE**	ps;

	ATypeGetName(OTYPE_TEXTMAKER, szType, sizeof(szType));
	strcat(szType, ": ");
	AObjGetName(objectId, (LPSTR)name, maxEdit+1);
	strcat(szType, name);
	SetWindowText((HWND)hDialog, (LPCSTR)szType/*name*/);

	hType 					= GetDlgItem(hDialog, TFTYPE);
	hRemoveButton 	= GetDlgItem(hDialog, TFREMOVE);
	hAddButton      = GetDlgItem(hDialog, TFADD);
	hEditButton     = GetDlgItem(hDialog, TFEDIT);
	hAliasTitle			= GetDlgItem(hDialog, TFALIASTITLE);
	hAlias					= GetDlgItem(hDialog, TFALIAS);

	long		theGetKey = AOBJ_GETFIRSTKEY;
	ATYPEID	theType;
	int			index;
	BOOL		selected = FALSE;
	while ((theType = ATypeGetNext(&theGetKey))!=NULL)
	{
		if(AEvtCheckForMethod(theType, AEVENT_OBJECTTOTEXT))
		{
			ATypeGetName(theType, (LPSTR)name, maxEdit+1);
			index = ComboBox_AddString(GetDlgItem(hDialog, TFTYPE) ,name);
///			index = (int)SendDlgItemMessage(hDialog, TFTYPE, CB_ADDSTRING, 0, (LONG)name);
				ComboBox_SetItemData(GetDlgItem(hDialog, TFTYPE) ,index, theType);
///			SendDlgItemMessage(hDialog, TFTYPE, CB_SETITEMDATA, index, theType);

			if(theType == OTYPE_TEXT)
			{
				ComboBox_SetCurSel(GetDlgItem(hDialog, TFTYPE) ,index);
///				SendDlgItemMessage(hDialog, TFTYPE, CB_SETCURSEL, index, 0L);
        selected = TRUE;
			}
		}
	}

	if(!selected)
		ComboBox_SetCurSel(GetDlgItem(hDialog, TFTYPE) ,0);
///		SendDlgItemMessage(hDialog, TFTYPE, CB_SETCURSEL, 0, 0L);
	FillType();
	FillVar();
	FillResult(data4->resultId);
 	MakeExpress();
	CheckDlgButton(hDialog, TFAUTO, data4->autoUpdate);
	UpdateAddButton();
	UpdateRemoveButton();
	UpdateEditButton();
	UpdateAliasControl();
	SetFocus(hType);
}				// TEXTFACT::InitDlgControl...


///////////////////////////////////////////////////////////////
// TEXTFACT::SaveDatas:
//
///////////////////////////////////////////////////////////////

BOOL TEXTFACT::SaveDatas ( void )
{
	void	*data2, *data3, *data5, *data6;
	long	size2, size3;

	AObjSetDataCount(objectId, DATACOUNT+2L);

	size2 = AObjGetDataSize(objectId, 2L);
  size3 = AObjGetDataSize(objectId, 3L);

	data2 = (void*)AObjLockData(objectId, 2L);
  data3 = (void*)AObjLockData(objectId, 3L);

	data5 = (void*)AObjSetData(objectId, 5L, data2, size2);
	data6 = (void*)AObjSetData(objectId, 6L, data3, size3);

  AObjUnlockData(objectId, 2L);
	AObjUnlockData(objectId, 3L);

	if (data5 == NULL || data6 == NULL)
	{
		MessageBox(NULL, "Text Builder: AObjSetData Failed On Save Datas", NULL, MB_OK | MB_ICONEXCLAMATION );
    return 0;
	}

	return 1;
}				// TEXTFACT::SaveDatas...


///////////////////////////////////////////////////////////////
// TEXTFACT::RestoreDatas:
//
///////////////////////////////////////////////////////////////

BOOL TEXTFACT::RestoreDatas ( BOOL error )
{
	if (error)
    {
		void	*data2r, *data5, *data6;

		long	size5, size6;

		size5 = AObjGetDataSize(objectId, 5L);
  	size6 = AObjGetDataSize(objectId, 6L);

		data5 = (void*)AObjLockData(objectId, 5L);
		data6 = (void*)AObjLockData(objectId, 6L);

		AObjUnlockData(objectId, 2L);

		data2r = (void*)AObjSetData(objectId, 2L, data5, size5);

		strclass3->SetData(objectId, 3L, data6, size6);

		data2 = (TOKEN*)AObjLockData(objectId, 2L);

		AObjUnlockData(objectId, 5L);
		AObjUnlockData(objectId, 6L);

		if (data2r == NULL)
		{
			MessageBox(NULL, "Text Builder: AObjSetData Failed On Restore Datas", NULL, MB_OK | MB_ICONEXCLAMATION );
 	    return 0;
		}
	}
	AObjSetDataCount(objectId, DATACOUNT);
	return 1;
}				// TEXTFACT::RestoreDatas...


///////////////////////////////////////////////////////////////
// TEXTFACT::UpdateObject:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::UpdateObject ( void )
{
  BOOL				autoUpdate;
	int					i;
  LRESULT			lresult;
	DWORD				count;
  AOBJECTID		resultId;
	LISTDATA*		ldPtr;

	// Data 4
	lresult = ComboBox_GetCurSel(GetDlgItem(hDialog, TFRESULT));
///	lresult = SendDlgItemMessage(hDialog, TFRESULT, CB_GETCURSEL, 0, 0L);
  if (lresult == CB_ERR)
		resultId = 0L;
  else
		resultId = ComboBox_GetItemData(GetDlgItem(hDialog, TFRESULT), lresult);
///		resultId = SendDlgItemMessage(hDialog, TFRESULT, CB_GETITEMDATA, WPARAM(lresult), 0L);

	autoUpdate = IsDlgButtonChecked(hDialog, TFAUTO);

	// Data 0 & Data 1
	count = ListBox_GetCount(GetDlgItem(hDialog, TFOBJVAR));
///	count = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETCOUNT, 0, 0L);

	// Check Here For Circular Inclusion
	for (i = 0; i < count; i++)
	{
		ldPtr = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), i);
///		ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, i, 0L);
    if (!*ldPtr->alias)
		{
			MessageBox(hDialog, "Variable Name is zero length.", NULL, MB_OK | MB_ICONEXCLAMATION );
			ListBox_SetSel(GetDlgItem(hDialog, TFOBJVAR), FALSE, -1);
///			SendDlgItemMessage(hDialog, TFOBJVAR, LB_SETSEL, FALSE, MAKELONG(-1, 0));
			ListBox_SetSel(GetDlgItem(hDialog, TFOBJVAR), TRUE, i);
///			SendDlgItemMessage(hDialog, TFOBJVAR, LB_SETSEL, TRUE, MAKELONG(i, 0));
			PostMessage(hDialog, WM_COMMAND, TFOBJVAR, MAKELONG(NULL, LBN_SELCHANGE)); //!!PORT!!
			vG.parseError = TRUE;
			return;
		}

		if (autoUpdate && ldPtr->objid == resultId)
		{
			char	szBuffer[512];
			char	szName[80];
      AObjGetName(resultId, szName, 79L);
			wsprintf(szBuffer, "The object '%s' cannot be the result object because it is in the list of expression objects.  Either remove it from the expression object list or turn off Auto Update.", szName);
			MessageBox(hDialog, szBuffer, NULL, MB_OK | MB_ICONEXCLAMATION );
     	vG.parseError = TRUE;
			return;
		}
	}

	AObjUnlockData(objectId, 0L);
	data0 = (AOBJECTID*)AObjResizeData(objectId, 0L, sizeof(LONG)*(count+1) );
	if (data0 == NULL)
		return;
	data0 = (AOBJECTID*)AObjLockData(objectId, 0L);

	strclass1->Clear();
	for (i = 0; i < count; i++)
	{
	 	ldPtr = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), i);
///		ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, i, 0L);
		data0[i] = ldPtr->objid;
		strclass1->Append(ldPtr->alias);
	}
	data0[i] = NULL;

	// Data 2 & Data 3
	Express();

	// Data 4
	data4->autoUpdate = autoUpdate;
	data4->resultId = resultId;
}				// TEXTFACT::UpdateObject...


///////////////////////////////////////////////////////////////
// TEXTFACT::CheckDuplicate:
//
///////////////////////////////////////////////////////////////

BOOL TEXTFACT::CheckDuplicate ( void )
{
  LRESULT			count;
	LISTDATA*		ldPtr;
	LISTDATA*		ldPtr2;

	count = ListBox_GetCount(GetDlgItem(hDialog, TFOBJVAR));
///	count = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETCOUNT, 0, 0L);

	// check for same alias warning
	for(int i = 0; i < count-1; i++)
	{
		ldPtr = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), i);
///		ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, i, 0L);

		for(int j = i+1; j < count; j++)
		{
			ldPtr2 = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), j);
///			ldPtr2 = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, j, 0L);

			if(!lstrcmp((const char *)ldPtr->alias, (const char *)ldPtr2->alias))
			{
				char* buf = (char*)(new char[(int)(lstrlen((const char*)ldPtr->alias)+100)]);

				// the aliases are the same!
				wsprintf(buf, "Variable name '%s' is a duplicate name.\nThis may cause unpredictable results.", ldPtr->alias);
				int MBret = MessageBox(hDialog, buf, "Warning", MB_OKCANCEL | MB_ICONINFORMATION);

				delete buf;
				if(MBret == IDCANCEL)
				{
					ListBox_SetSel(GetDlgItem(hDialog, TFOBJVAR), FALSE, -1);
///					SendDlgItemMessage(hDialog, TFOBJVAR, LB_SETSEL, FALSE, MAKELONG(-1, 0));
					ListBox_SetSel(GetDlgItem(hDialog, TFOBJVAR), TRUE, i);
///					SendDlgItemMessage(hDialog, TFOBJVAR, LB_SETSEL, TRUE, MAKELONG(i, 0));
					ListBox_SetSel(GetDlgItem(hDialog, TFOBJVAR), TRUE, j);
///					SendDlgItemMessage(hDialog, TFOBJVAR, LB_SETSEL, TRUE, MAKELONG(j, 0));
					PostMessage(hDialog, WM_COMMAND, MAKELONG(TFOBJVAR, LBN_SELCHANGE), (LONG)GetDlgItem(hDialog, TFOBJVAR));
///					PostMessage(hDialog, WM_COMMAND, TFOBJVAR, MAKELONG(NULL, LBN_SELCHANGE)); //!!PORT!!

					return TRUE;
        }
				else
					return FALSE;
			}	
		}
    }
	return FALSE;
}				// TEXTFACT::CheckDuplicate...


///////////////////////////////////////////////////////////////
// TEXTFACT::Express: Store expression as tokens
//
///////////////////////////////////////////////////////////////

WORD TEXTFACT::Express ( void )
{
	char*		string;
  LRESULT	length;	

	length = SendDlgItemMessage(hDialog, TFEXPRESS, WM_GETTEXTLENGTH, 0, 0L)+1;

	string = new char[length+1];
	if (string == NULL)
		return FALSE;

	GetDlgItemText(hDialog, TFEXPRESS, (LPSTR)string, UINT(length));

	if ( Parse(string) == FALSE)
	{
		delete string;
		return FALSE;
	}

	delete string;
	return TRUE;
}				// TEXTFACT::Express...


///////////////////////////////////////////////////////////////
// TEXTFACT::DeleteListData:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::DeleteListData ( void )
{
	int					i;
	DWORD				count;
	LISTDATA*		ldPtr;

	count = ListBox_GetCount(GetDlgItem(hDialog, TFOBJVAR));
///	count = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETCOUNT, 0, 0L);

	for (i = 0; i < count; i++)
	{
		ldPtr = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), i);
///		ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, i, 0L);
		if (ldPtr != NULL)
			delete ldPtr;
	}
}				// TEXTFACT::DeleteListData...


///////////////////////////////////////////////////////////////
// TEXTFACT::UpdateAddButton:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::UpdateAddButton ( void )
{
	LRESULT	cnt;

	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJLIST));
///	cnt = SendDlgItemMessage(hDialog, TFOBJLIST, LB_GETSELCOUNT, 0, 0L);

	if (cnt)
		EnableWindow(hAddButton, TRUE);
	else
		EnableWindow(hAddButton, FALSE);
}				// TEXTFACT::UpdateAddButton...


///////////////////////////////////////////////////////////////
// TEXTFACT::UpdateRemoveButton:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::UpdateRemoveButton ( void )
{
	LRESULT	cnt;

	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJVAR));
///	cnt = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELCOUNT, 0, 0L);

	if (cnt)
		EnableWindow(hRemoveButton, TRUE);
	else
		EnableWindow(hRemoveButton, FALSE);
}				// TEXTFACT::UpdateRemoveButton...


///////////////////////////////////////////////////////////////
// TEXTFACT::UpdateEditButton:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::UpdateEditButton ( void )
{
	LRESULT	cnt;

	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJVAR));
///	cnt = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELCOUNT, 0, 0L);

	if (cnt == 1)
		EnableWindow(hEditButton, TRUE);
	else
		EnableWindow(hEditButton, FALSE);
}				// TEXTFACT::UpdateEditButton...


///////////////////////////////////////////////////////////////
// TEXTFACT::UpdateAliasControl:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::UpdateAliasControl ( void )
{
	LRESULT		cnt;
	int		 		intArray[2];
	LISTDATA*	ld;

	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJVAR));
///	cnt = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELCOUNT, 0, 0L);

	if (cnt == 1)
  {
		ListBox_GetSelItems(GetDlgItem(hDialog, TFOBJVAR), cnt, intArray);
///		SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELITEMS, maxEdit, (LONG)intArray);

		ld  = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), intArray[0]);
///		ld = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, intArray[0], 0L);
    if (LRESULT(ld) != LB_ERR)
			SetDlgItemText(hDialog, TFALIAS, (LPCSTR)ld->alias);

    EnableWindow(hAliasTitle, TRUE);
		EnableWindow(hAlias, TRUE);
  }
	else
  {
		SetDlgItemText(hDialog, TFALIAS, "");

		EnableWindow(hAliasTitle, FALSE);
		EnableWindow(hAlias, FALSE);
	}
}				// TEXTFACT::UpdateAliasControl...


///////////////////////////////////////////////////////////////
// TEXTFACT::FillType: Fill Object List
//
///////////////////////////////////////////////////////////////

void TEXTFACT::FillType ( void )
{
	char			name[maxEdit+1];
	LRESULT		index;
	ATYPEID		type;
	ATYPEID		objid;
	LONG			getkey;

	ListBox_ResetContent(GetDlgItem(hDialog, TFOBJLIST));
///	SendDlgItemMessage(hDialog, TFOBJLIST, LB_RESETCONTENT, 0, 0L);

	index = ComboBox_GetCurSel(GetDlgItem(hDialog, TFTYPE));
///	index = SendDlgItemMessage(hDialog, TFTYPE, CB_GETCURSEL, 0, 0L);
	if (index == CB_ERR)
  	return;

	type = ComboBox_GetItemData(GetDlgItem(hDialog, TFTYPE), index);
///	type = SendDlgItemMessage(hDialog, TFTYPE, CB_GETITEMDATA, (WPARAM)index, 0L);
	if (type == CB_ERR)
  	return;

	getkey = -1;
	while ( (objid = AObjGetNextObject(type, &getkey)) != 0)
	{
		AObjGetName(objid, (LPSTR)name, maxEdit);
		index = ListBox_AddString(GetDlgItem(hDialog, TFOBJLIST), name);
///		index = SendDlgItemMessage(hDialog, TFOBJLIST, LB_ADDSTRING, 0, LONG(name));
		ListBox_SetItemData(GetDlgItem(hDialog, TFOBJLIST), index, objid);
///		SendDlgItemMessage(hDialog, TFOBJLIST, LB_SETITEMDATA, WPARAM(index), objid);
	}
}				// TEXTFACT::FillType...


///////////////////////////////////////////////////////////////
// TEXTFACT::FillVar:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::FillVar ( void )
{
	char*				name;
	int					i;
	LRESULT			index;
	LISTDATA*		ldPtr;

	if (data0 == NULL)
		return;

	name = new char[maxEdit+1];
	if (name == NULL)
		return;

	for (i = 0; data0[i]; i++)  // this better be null terminated!
	{
		AObjGetName(data0[(int)i], (LPSTR)name, maxEdit);

		index = ListBox_AddString(GetDlgItem(hDialog, TFOBJVAR), name);
///		index = SendDlgItemMessage(hDialog, TFOBJVAR, LB_ADDSTRING, 0, LONG(name));

		ldPtr = new LISTDATA;
		ldPtr->Init(data0[i], strclass1->Get(i));
		ListBox_SetItemData(GetDlgItem(hDialog, TFOBJVAR), index, ldPtr);
///		SendDlgItemMessage(hDialog, TFOBJVAR, LB_SETITEMDATA, WPARAM(index), LONG(ldPtr) );
	}
	delete name;
}				// TEXTFACT::FillVar...


///////////////////////////////////////////////////////////////
// TEXTFACT::FillResult:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::FillResult ( AOBJECTID id )
{
	char			name[maxEdit+1];
	LRESULT		index;
	AOBJECTID	objid;
  ATYPEID		typeID;
	LONG			objectGetKey;
  LONG			typeGetKey;

	ComboBox_ResetContent(GetDlgItem(hDialog, TFRESULT));
///	SendDlgItemMessage(hDialog, TFRESULT, CB_RESETCONTENT, 0, 0L);

	index = ComboBox_AddString(GetDlgItem(hDialog, TFRESULT), "(None)");
///	index = SendDlgItemMessage(hDialog, TFRESULT, CB_ADDSTRING, 0, (LONG)"(None)");
	ComboBox_SetItemData(GetDlgItem(hDialog, TFRESULT), index, 0L);
///	SendDlgItemMessage(hDialog, TFRESULT, CB_SETITEMDATA, WPARAM(index), 0L);
	if (id == 0L)
		ComboBox_SetCurSel(GetDlgItem(hDialog, TFRESULT), index);
///		SendDlgItemMessage(hDialog, TFRESULT, CB_SETCURSEL, WPARAM(index), 0L);

	typeGetKey = AOBJ_GETFIRSTKEY;
	while((typeID = ATypeGetNext(&typeGetKey)) != NULL)
		{
		if(AEvtCheckForMethod(typeID, AEVENT_OBJECTTOTEXT))
			{
      objectGetKey = AOBJ_GETFIRSTKEY;
			while ( (objid = AObjGetNextObject(typeID, &objectGetKey)) != 0)
				{
				AObjGetName(objid, (LPSTR)name, maxEdit);
				index = ComboBox_AddString(GetDlgItem(hDialog, TFRESULT), name);
///				index = SendDlgItemMessage(hDialog, TFRESULT, CB_ADDSTRING, 0, LONG(name));
				ComboBox_SetItemData(GetDlgItem(hDialog, TFRESULT), index, objid);
///				SendDlgItemMessage(hDialog, TFRESULT, CB_SETITEMDATA, WPARAM(index), objid);

				if (objid == id)
					ComboBox_SetCurSel(GetDlgItem(hDialog, TFRESULT), index);
///					SendDlgItemMessage(hDialog, TFRESULT, CB_SETCURSEL, WPARAM(index), 0L);
				}
      }
    }
}				// TEXTFACT::FillResult...


///////////////////////////////////////////////////////////////
// TEXTFACT::MakeExpress:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::MakeExpress ( void )
{
	char*		express;
   int		len;

	if (data2 == NULL)
		return;

	express = new char[maxEdit*2];
	if (express == NULL)
		return;

	*express = 0;

	for ( int i = 0; data2[i].type != STOP; i++)
	{
		switch ( data2[i].type )
		{
		case LITERAL:
			MakeLiteral(express, data2[i].multi);
			break;

		case CHARACTER:
			MakeCharacter(express, data2[i].multi);
			break;

		case OBJECT:
			MakeObject(express, data2[i].multi);
			break;
		}
	}

	if (i > 0)
  {
		len = lstrlen((LPCSTR)express);
   	express[len-1] = 0;

		SetDlgItemText(hDialog, TFEXPRESS, (LPCSTR)express);
  }
	delete express;
}				// TEXTFACT::MakeExpress...


///////////////////////////////////////////////////////////////
// TEXTFACT::MakeLiteral:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::MakeLiteral ( char* express, LONG multi )
{
	lstrcat((LPSTR)express, "\"");
	lstrcat((LPSTR)express, (LPCSTR)strclass3->Get(UINT(multi)));
	lstrcat((LPSTR)express, "\"");
	lstrcat((LPSTR)express, "+");
}				// TEXTFACT::MakeLiteral...


///////////////////////////////////////////////////////////////
// TEXTFACT::MakeCharacter:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::MakeCharacter ( char* express, LONG multi )
{
	BYTE 	space[30];

	switch ( int(multi) )
	{
	case 9:
		lstrcat((LPSTR)express, "<TAB>+");
		break;

	case 10:
		lstrcat((LPSTR)express, "<LF>+");
		break;

	case 13:
		lstrcat((LPSTR)express, "<CR>+");
		break;

	case 27:
		lstrcat((LPSTR)express, "<ESC>+");
		break;

	default:
		wsprintf((LPSTR)space, "<%d>+", int(multi));
		lstrcat((LPSTR)express, (LPCSTR)space);
		break;
	}
}				// TEXTFACT::MakeCharacter...


///////////////////////////////////////////////////////////////
// TEXTFACT::MakeObject:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::MakeObject ( char* express, LONG multi )
{
	BOOL found = FALSE;

	// the index is not stored
	// it is derived by matching the object id from
	// the object list in data0 with multi
	// this is not needed during run time

	for (int i = 0; data0[i]; i++)
	{
		if (multi == data0[i])
    {
      found = TRUE;
			break;
    }
  }

	if (found)
  {
		lstrcat((LPSTR)express, (LPCSTR)strclass1->Get(i));
		lstrcat((LPSTR)express, "+");
	}
	else
		MessageBox(hDialog, "Alias Object Missing;\nCheck String", NULL, MB_OK | MB_ICONEXCLAMATION);
}				// TEXTFACT::MakeObject...


///////////////////////////////////////////////////////////////
// TEXTFACT::Add:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::Add ( void )
{
  char*				name;
	int					i;
	LRESULT			count;
	LRESULT     index;
	int*				intArray;
	AOBJECTID		objid;
	LISTDATA*   ldPtr;

	name = new char[maxEdit+1];
	if (name == NULL)
		return;

	count = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJLIST));
///	count = SendDlgItemMessage(hDialog, TFOBJLIST, LB_GETSELCOUNT, 0, 0L);
	if (count == 0)
  {
    delete name;
		return;
  }

	intArray = new int[count];
	if (intArray == NULL)
  {
    delete name;
		return;
  }

	ListBox_GetSelItems(GetDlgItem(hDialog, TFOBJLIST), count, intArray);
///	SendDlgItemMessage(hDialog, TFOBJLIST, LB_GETSELITEMS, WPARAM(count), LONG(intArray));

	for (i = 0; i < count; i++)
	{
		objid = ListBox_GetItemData(GetDlgItem(hDialog, TFOBJLIST), intArray[i]);
///		objid = SendDlgItemMessage(hDialog, TFOBJLIST, LB_GETITEMDATA, intArray[i], 0L);

		ldPtr = (LISTDATA*)new LISTDATA(objid);

		AObjGetName(objid, name, maxEdit);
		index = ListBox_AddString(GetDlgItem(hDialog, TFOBJVAR), name);
///		index = SendDlgItemMessage(hDialog, TFOBJVAR, LB_ADDSTRING, 0, long(name));
		ListBox_SetItemData(GetDlgItem(hDialog, TFOBJVAR), index, ldPtr);
///		SendDlgItemMessage(hDialog, TFOBJVAR, LB_SETITEMDATA, WPARAM(index), (LONG)ldPtr);
	}
	delete intArray;
	delete name;
}				// TEXTFACT::Add...


///////////////////////////////////////////////////////////////
// TEXTFACT::Remove:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::Remove ( void )
{
	LRESULT			count;
	int					i;
	int*				intArray;
	LISTDATA*   ldPtr;

	count = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJVAR));
///	count = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELCOUNT, 0, 0L);
	if (count == 0)
		return;

	intArray = new int[count];
	if (intArray == NULL)
		return;

	ListBox_GetSelItems(GetDlgItem(hDialog, TFOBJVAR), count, intArray);
///	SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELITEMS, WPARAM(count), LPARAM(intArray));

	for (i = (int)count-1; i >= 0; i--)
	{
		ldPtr = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), intArray[i]);
///		ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, intArray[i], 0L);

		ListBox_DeleteString(GetDlgItem(hDialog, TFOBJVAR), intArray[i]);
///		SendDlgItemMessage(hDialog, TFOBJVAR, LB_DELETESTRING, intArray[i], 0L);

		if (ldPtr != NULL)
			delete ldPtr;
	}
	delete intArray;
}				// TEXTFACT::Remove...


///////////////////////////////////////////////////////////////
// TEXTFACT::Edit:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::Edit ( void )
{
	LRESULT   count;
	LONG      ret;
	int    		intArray[2];
  LISTDATA*	ldPtr;

	count = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJVAR));
///	count = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELCOUNT, 0, 0L);

	if (count != 1)
		return;

	ret = ListBox_GetSelItems(GetDlgItem(hDialog, TFOBJVAR), count, intArray);
///	ret = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELITEMS, WPARAM(count), (LPARAM)&intArray);
	if (ret == LB_ERR)
		return;

	ldPtr = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), intArray[0]);
///	ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, intArray, 0L);
	if (LRESULT(ldPtr) != LB_ERR)
		AObjEdit(ldPtr->objid);   // first item
}				// TEXTFACT::Edit...


///////////////////////////////////////////////////////////////
// TEXTFACT::Alias:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::Alias ( void )
{
	char*			name;
	LRESULT		count;
	int 			intArray[2];
	LISTDATA* ldPtr;

	count = ListBox_GetSelCount(GetDlgItem(hDialog, TFOBJVAR));
///	count = SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELCOUNT, 0, 0L);
	if (count != 1)
		return;

	name = new char[maxEdit+1];
	if (name == NULL)
		return;

	ListBox_GetSelItems(GetDlgItem(hDialog, TFOBJVAR), count, intArray);
///	SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETSELITEMS, maxEdit, (LONG)intArray);
	ldPtr = (LISTDATA*)ListBox_GetItemData(GetDlgItem(hDialog, TFOBJVAR), intArray[0]);
///	ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, TFOBJVAR, LB_GETITEMDATA, intArray[0], 0L);
	GetDlgItemText(hDialog, TFALIAS, (LPSTR)name, maxEdit);
	ldPtr->SetAlias(name);

	delete name;
}				// TEXTFACT::Alias...


///////////////////////////////////////////////////////////////
// TEXTFACT::Parse: This must update the data immediately.
//
///////////////////////////////////////////////////////////////

BOOL TEXTFACT::Parse ( char* string, pAObjMessage sys, BOOL isImportTime )
{
  BOOL		error;
	int			i;
	char*		p;
  WORD		length;
	WORD		tokenCount;
	TOKEN*	tokenArray;
	TOKEN*	tkp;

	vG.parseError = FALSE;

	tokenArray 	= new TOKEN[maxEdit];
	if (tokenArray == NULL)
		return FALSE;

	strclass3->Clear();

	p = (char*)string;
    SkipSpace(p);

	tkp = tokenArray;
	tokenCount = 0;

	while (*p)
	{
    error = 0;
		switch (*p)
		{
		case '"':
			tkp->type = LITERAL;
			if ( (tkp->multi = GetLiteral(p, length, sys, isImportTime)) == -1)
				error = TRUE;

			tkp->length = length;
			break;

		case '<':
			tkp->type = CHARACTER;
			if ( (tkp->multi = GetCharacter(p, sys, isImportTime)) == -1)
				error = TRUE;

			tkp->length = 1;
			break;

		default:
			tkp->type = OBJECT;
			if ( (i = GetObject(p, sys, isImportTime)) == -1 )
				error = TRUE;

			tkp->length = 0L;
			tkp->multi = data0[i];
			break;
		}
    if (!error)
			tkp++;
		else
			if(sys)
			{
      	delete tokenArray;
				return FALSE;
      }

		if (++tokenCount == 255)
			break;
	}

	tkp->type = STOP;	// terminator
	tkp->multi = 0L;
	tokenCount++;

  AObjUnlockData(objectId, 2L); // Can't set data if locked, so release pointer
	AObjSetData(objectId, 2L, tokenArray, long(sizeof(TOKEN)*tokenCount));
	data2 = (TOKEN*)AObjLockData(objectId, 2L);  // Get pointer again

	delete tokenArray;
	return TRUE;
}				// TEXTFACT::Parse...


///////////////////////////////////////////////////////////////
// TEXTFACT::GetLiteral:
//
///////////////////////////////////////////////////////////////

long	TEXTFACT::GetLiteral ( char*& space, WORD& length, pAObjMessage sys, BOOL isImportTime )
{
	char*	pBuf;
  char*	pStart;
  char*	pEnd;
	UINT	i;
	long	index;

  if (*space != '"')
	{
		if(sys)
			AFuncReportResult(sys, 0L, "Missing Opening Quote In Literal Expression");
		else if(isImportTime)
			AUpsfLogComment("Missing Opening Quote In Literal Expression");
    else
			MessageBox(NULL, "Missing Opening Quote In Literal Expression", "Warning!", MB_OK | MB_ICONEXCLAMATION);
		vG.parseError = TRUE;
   	return -1L;
	}
	else
		space++;

  pStart = space;

	while (*space && *space != '"')
		space++;

	pEnd = space;

	if (*space != '"')
	{
		if(sys)
			AFuncReportResult(sys, 0L, "Missing Closing Quote In Literal Expression");
		else if(isImportTime)
			AUpsfLogComment("Missing Opening Quote In Literal Expression");
    else
			MessageBox(NULL, "Missing Closing Quote In Literal Expression", "Warning!", MB_OK | MB_ICONEXCLAMATION);
		vG.parseError = TRUE;
  	return -1L;
	}
	SkipSpace(++space);

	if (*space == '+')
		SkipSpace(++space);
	else
    if (*space)
	{
		if(sys)
			AFuncReportResult(sys, 0L, "Missing Expression Separator '+' In Literal Expression");
		else if(isImportTime)
			AUpsfLogComment("Missing Expression Separator '+' In Literal Expression");
    else
			MessageBox(NULL, "Missing Expression Separator '+' In Literal Expression", "Warning!", MB_OK | MB_ICONEXCLAMATION);
		vG.parseError = TRUE;
  	return -1L;
	}

	length = (WORD)(pEnd - pStart);

	index = -1L;
	if (length > 0)
    {
		pBuf = new char[length+1];

		for (i = 0; i < length; i++)
			pBuf[i] = pStart[i];
		pBuf[i] = 0;

		index = (long)strclass3->Append((BYTE*)pBuf);
    delete pBuf;
    }
	return index;
}				// TEXTFACT::GetLiteral...


///////////////////////////////////////////////////////////////
// TEXTFACT::GetCharacter:
//
///////////////////////////////////////////////////////////////

long TEXTFACT::GetCharacter ( char*& space, pAObjMessage sys, BOOL isImportTime )
{
	int		i;
	char*	p;
	char	s[maxEdit];
	char	character[50];
	long	c = -1L;

  // get character text
	for (i = 0; i < 49 && *space && *space != '+'; i++)
		character[i] = *space++;
	character[i] = 0;

	TrimSpace(character);			// remove trailing spaces

	if (*space == '+')
		SkipSpace(++space);
	else
	if (*space)
	{
		if(sys)
			AFuncReportResult(sys, 0L, "Missing Expression Separator '+' In Literal Expression");
		else if(isImportTime)
			AUpsfLogComment("Missing Expression Separator '+' In Literal Expression");
    else
			MessageBox(NULL, "Missing Expression Separator '+' In Literal Expression", "Warning!", MB_OK | MB_ICONEXCLAMATION);
		vG.parseError = TRUE;
   	return -1L;
	}

	for (p = character; *p; p++)
		*p = (char)toupper(*p);

	switch (character[1])
	{
		case 'C':  	// <cr>
      if (lstrcmp(character, "<CR>") == 0)
				c = 13L;
			break;

		case 'L':  	// <lf>
			if (lstrcmp(character, "<LF>") == 0)
				c = 10L;
			break;

		case 'T': 	// <tab>
			if (lstrcmp(character, "<TAB>") == 0)
				c = 9L;
			break;

		case 'E':  	// <esc>
			if (lstrcmp(character, "<ESC>") == 0)
				c = 27L;
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			c = strtol(character+1, NULL, 0);
			break;
	}

	if (c < 1 || c > 255)
	{
		wsprintf(s, "Invalid Character %s In Expression", character);
		if(sys)
			AFuncReportResult(sys, 0L, s);
		else if(isImportTime)
    	AUpsfLogComment(s);
    else
			MessageBox(NULL, s, NULL, MB_OK | MB_ICONEXCLAMATION);
    vG.parseError = TRUE;
		return -1L;
	}
	return c;
}				// TEXTFACT::GetCharacter...


///////////////////////////////////////////////////////////////
// TEXTFACT::GetObject:
//
///////////////////////////////////////////////////////////////

int TEXTFACT::GetObject ( char*& space, pAObjMessage sys, BOOL isImportTime )
{
  char	s[maxEdit];
  char*	alias;
	char	objectName[50];
	int		i;
	LONG	count;

	SkipSpace(space);

  // get object name
	for (i = 0 ; i < 49 && *space && *space != '+'; i++)
		objectName[i] = *space++;
	objectName[i] = 0;

  TrimSpace(objectName);

	if (*space == '+')
		SkipSpace(++space);
	else
	if (*space)
	{
		if(sys)
			AFuncReportResult(sys, 0L, "Missing Expression Separator '+' In Literal Expression");
		else if(isImportTime)
			AUpsfLogComment("Missing Expression Separator '+' In Literal Expression");
    else
			MessageBox(NULL, "Missing Expression Separator '+' In Literal Expression", "Warning!", MB_OK | MB_ICONEXCLAMATION);
		vG.parseError = TRUE;
   	return -1;
	}

	count = strclass1->Count();

	for ( i = 0; i < count; i++ )
	{
		alias = (char*)(strclass1->Get((UINT)i));
		if ( lstrcmp(alias, objectName) == 0 )
			return i;
	}

	vG.parseError = TRUE;
	wsprintf(s, "Invalid variable name '%s' in expression.", objectName);
	if(sys)
		AFuncReportResult(sys, 0L, s);
	else if(isImportTime)
  	AUpsfLogComment(s);
  else
		MessageBox(NULL, s, NULL, MB_OK | MB_ICONEXCLAMATION);

	return -1;
}				// TEXTFACT::GetObject...


///////////////////////////////////////////////////////////////
// TEXTFACT::DrawObjectName:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::DrawObjectName ( void )
{
	char	textbuf[35];

	ATypeGetName(OTYPE_TEXTMAKER, textbuf, 30);
	AUtlSetTitleFont(OTYPE_TEXTMAKER, textbuf, GetDlgItem(hDialog, IDD_OBJECT));
}				// TEXTFACT::DrawObjectName...


///////////////////////////////////////////////////////////////
// TEXTFACT::NewEditControlProc:
//
///////////////////////////////////////////////////////////////

long CALLBACK TEXTFACT::NewEditControlProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_DESTROY:
		RemoveProp(hWnd, "Shift");
		break;

	case WM_CHAR:
	{
		if(!(isalnum(wParam) || wParam=='_' || wParam=='\b'))
		{
    	MessageBeep(-1);
			return 0L;
		}
    break;
  }

	case WM_KEYDOWN:
	{
		if(wParam == VK_SHIFT)
			SetProp(hWnd, "Shift", (HANDLE)TRUE);
		else
		if(GetProp(hWnd, "Shift") && wParam == VK_INSERT)
		{
			if (OpenClipboard(hWnd))
			{
       	HANDLE hText;

				if((hText = GetClipboardData(CF_TEXT)) != NULL)
				{
					char* text = (char*)GlobalLock(hText);

					for (int i = 0; i < lstrlen(text); i++)
					{
						if (!(isalnum(text[i]) || text[i]=='_'))
						{
         			// don't paste if there is an invalid character
         			CloseClipboard();
         			GlobalUnlock(hText);
         			return 0L;
         		}
					} // end...for(int i=0; ...
					GlobalUnlock(hText);
				} // end...if((hText = ...
	     	CloseClipboard();
			}
		} // end...else if(G.Shift ...)
   	break;
	}

  case WM_KEYUP:
		if (wParam == VK_SHIFT)
			SetProp(hWnd, "Shift", FALSE);
		break;

	default:
		break;
	}
	return ((long) CallWindowProc((WNDPROC)vG.lpfnOldEditControlProc, hWnd, message, wParam, lParam));
}				// TEXTFACT::NewEditControlProc...


///////////////////////////////////////////////////////////////
// SkipSpace:
//
///////////////////////////////////////////////////////////////

void SkipSpace ( char*& p )
{
	while (*p && *p == ' ')
   	p++;
}				// SkipSpace:...


///////////////////////////////////////////////////////////////
// TrimSpace:
//
///////////////////////////////////////////////////////////////

void TrimSpace ( char* p )
{
	int 	i;

	i = lstrlen(p);

	while (i > 0 && p[i-1] == ' ')
   	i--;
	p[i] = 0;
}				// TrimSpace:...


///////////////////////////////////////////////////////////////
// Underscore:
//
///////////////////////////////////////////////////////////////
/*
void Underscore ( char* p )
{
	while (*p)
	{
		if (*p == ' ')
			*p = '_';
		p++;
	}
}				// Underscore:...
*/
