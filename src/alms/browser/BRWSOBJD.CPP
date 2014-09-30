/////////////////////////////////////////////////////////////////////////////
// File Name:  BrwsObjD.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  8/01/92
//
// Description.: Object Definition Routine for the Serius Browser object:
////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <mem.h>
#include <a_alm.h>
//#include <seriusid.h>
#include <a_almutl.h>
#include <helpids.h>
#include <o_datb.h>
#include "browser.h"

BOOL InitDialog(HWND, AOBJECTID) ;
BOOL FieldList(HWND hDlg, WORD wNotify) ;
BOOL DatabaseList(HWND hDlg, WORD wNotify) ;
void ListDatabaseObjects( HWND hDlg, AOBJECTID oiBrowser) ;
void ListFieldObjects( HWND hDlg, AOBJECTID oiBrowser) ;

struct OBJECTSIGNALS {
	char * name ;
	int ID ;
	} ;

OBJECTSIGNALS signals[] = {
	{"Clear Fields", 0},
	{"Before Creating File", 1},
	{"Before Opening File", 2},
	{"Before Closing File", 3},
	{"Before Quitting", 15},
	{"Before Finding", 5},
	{"Before Power Finding", 6},
	{"Before Getting Previous", 7},
	{"Before Getting Next", 8},
	{"Before All Records", 9},
	{"Before Found Records", 10},
	{"Before Clearing Fields", 4},
	{"Before Adding", 12},
	{"Before Updating", 13},
	{"Before Deleting", 14},
	{"After Creating File", 21},
	{"After Opening File", 22},
	{"After Closing File", 23},
	{"After Finding", 25},
	{"After Power Finding", 26},
	{"After Getting Previous", 27},
	{"After Getting Next", 28},
	{"After All Records", 29},
	{"After Found Records", 30},
	{"After Clearing Fields", 24},
	{"After Adding", 32},
	{"After Updating", 33},
	{"After Deleting", 34} } ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doEditObject
// DESCRIPTION:  Calls the Browser's definition routine.
/////////////////////////////////////////////////////////////////////////////
LONG EditObject( AOBJECTID oiBrowser )
	{
	long size ;
	LPVOID lpSource, lpDestination ;

	int nCount = (int)AObjGetDataCount(oiBrowser) ;
	AObjSetDataCount(oiBrowser, 2 * nCount) ;
	for (int i = 0; i < nCount; i++)
		{
		size = AObjGetDataSize(oiBrowser, i) ;
		lpSource = AObjLockData(oiBrowser, i) ;
		AObjResizeData(oiBrowser, i+nCount, size) ;
		lpDestination = AObjLockData(oiBrowser, i+nCount) ;
		CopyMemory(lpDestination, lpSource, (int)size) ;
		AObjUnlockData(oiBrowser, i) ;
		AObjUnlockData(oiBrowser, i+nCount) ;
		}
	BOOL result = DialogBoxParam( hInstance, "Browser", GetActiveWindow(),
					(DLGPROC)EditObjectDialogProc, (LPARAM)oiBrowser) ;
	if (result == FALSE)
		{
		for (i = 0; i < nCount; i++)
			{
			size = AObjGetDataSize(oiBrowser, nCount + i) ;
			lpSource = AObjLockData(oiBrowser, nCount + i) ;
			AObjResizeData(oiBrowser, i, size) ;
			lpDestination = AObjLockData(oiBrowser, i) ;
			CopyMemory(lpDestination, lpSource, (int)size) ;
			AObjUnlockData(oiBrowser, nCount + i) ;
			AObjUnlockData(oiBrowser, i) ;
			}
		AObjSetDataCount(oiBrowser, nCount) ;
		return A_OK ;
		}
	AObjSetDataCount(oiBrowser, nCount) ;
	return A_MODIFIED ;
	}

LONG BuildSignals(pAObjMessage theSystem)
	{
	AMEMBLOCKID signalBlock ;
	AObjSignalRecord  *lpSignal ;
	int i, nSignals ;

	signalBlock = (AMEMBLOCKID)theSystem->message2 ;
	nSignals = sizeof(signals)/sizeof(signals[0]) ;
	AMemSetSize(signalBlock, nSignals*sizeof(AObjSignalRecord)) ;
	lpSignal = (AObjSignalRecord *)AMemLock(signalBlock) ;
	if (lpSignal == NULL)
		return A_ERROR ;
	for (i = 0; i < nSignals; i++)
		{
		lpSignal[i].theID = signals[i].ID ;
		lstrcpy(lpSignal[i].theName, signals[i].name) ;
		}
	AMemUnlock(signalBlock) ;
	return A_OK ;
	}

LONG EvaluateObject( AOBJECTID oiBrowser, pAObjMessage theSystem )
	{
	BrowserInfo  *lpBrowser ;

	pAObjInfo lpObjectInfo = (pAObjInfo)theSystem->message2 ;
	lpObjectInfo->typeOverhead = 8192 ; // for button pallette bitmap

	lpBrowser = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	VerifyDatabaseAndField(lpBrowser) ;
	AOBJECTID oiDatabase = lpBrowser->oiDatabase ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	if (oiDatabase == 0)
		{
		return AObjReportError(oiBrowser, AOBJ_CHECKOBJECT, A_WARNING,
										"No database object is assigned to the Browser.",
										0);
		}
	return A_OK ;
	}

LONG ValidateObjectIDs( AOBJECTID oiBrowser, pAObjMessage theSystem )
	{
	BrowserInfo  *lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	AObjValidateID(&lpBrowserInfo->oiDatabase, theSystem) ;
	AObjValidateID(&lpBrowserInfo->oiField, theSystem) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return A_OK ;
	}

BOOL CALLBACK EditObjectDialogProc (HWND hDlg, WORD message,
								 WPARAM wParam, LPARAM lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			return (InitDialog(hDlg, (AOBJECTID)lParam)) ;

		case WM_COMMAND: //!!PORT!!
			WORD wID = LOWORD(wParam) ;
			WORD wNotify = HIWORD(wParam) ;
			switch (wID)
				{
				case IDC_DATABASE:
					return (DatabaseList(hDlg, wNotify)) ;

				case IDC_FIELD:
					return (FieldList(hDlg, wNotify)) ;

				case IDHELP:
					WinHelp(hDlg, "awusbrws.hlp", HELP_CONTEXT, HELPID_OBJD) ;
					return TRUE ;

				case IDOK:
					EndDialog (hDlg, TRUE) ;
					return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;
				}
		}
	 return FALSE ;
	 }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DrawObjectTypeName
// DESCRIPTION: Draw the object type name next to the database icon
////////////////////////////////////////////////////////////////////////////
/*BOOL DrawObjectTypeName(HWND hDlg)
	{
	char textBuffer[30] ;
	HWND hText ;

	hText = GetDlgItem(hDlg, IDD_OBJECT) ;
	ATypeGetName(OTYPE_BROWSER, textBuffer, 30) ;
	serdlgDrawTitle(hText, textBuffer) ;
	return FALSE ;
	}
*/
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DatabaseList
// DESCRIPTION:  Services the database combo box.
/////////////////////////////////////////////////////////////////////////////
BOOL DatabaseList(HWND hDlg, WORD wNotify)
	{
	AOBJECTID oiBrowser ;
	BrowserInfo  * lpBrowserInfo ;
	int objectIndex ;
	HWND hDataBase = GetDlgItem (hDlg, IDC_DATABASE);

	if (wNotify != CBN_SELCHANGE)
		return FALSE ;

	oiBrowser = (AOBJECTID)GetWindowLong(hDlg, DWL_USER) ;
	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	//objectIndex = (int)SendDlgItemMessage (hDlg, IDC_DATABASE, CB_GETCURSEL, 0, 0) ;
	objectIndex = (int)ComboBox_GetCurSel(hDataBase);
	//lpBrowserInfo->oiDatabase = (AOBJECTID)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_GETITEMDATA, objectIndex, 0) ;
	lpBrowserInfo->oiDatabase = (AOBJECTID)ComboBox_GetItemData (hDataBase, objectIndex) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	ListFieldObjects(hDlg, oiBrowser) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FieldList
// DESCRIPTION:  Services the field list combo box.
/////////////////////////////////////////////////////////////////////////////
BOOL FieldList(HWND hDlg, WORD wNotify)
	{
	AOBJECTID oiBrowser ;
	BrowserInfo  * lpBrowserInfo ;
	int objectIndex ;
	HWND hField = GetDlgItem (hDlg, IDC_FIELD) ;

	if (wNotify != CBN_SELCHANGE)
		return FALSE ;

	oiBrowser = (AOBJECTID)GetWindowLong (hDlg, DWL_USER) ;
	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	//objectIndex = (int)SendDlgItemMessage (hDlg, IDC_FIELD, CB_GETCURSEL, 0, 0) ;
	//lpBrowserInfo->oiField = (AOBJECTID)SendDlgItemMessage (hDlg, IDC_FIELD, CB_GETITEMDATA, objectIndex, 0) ;
	objectIndex = (int)ComboBox_GetCurSel (hField) ;
	lpBrowserInfo->oiField = (AOBJECTID)ComboBox_GetItemData (hField, objectIndex) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return TRUE ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the import / export edit dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL InitDialog(HWND hDlg, AOBJECTID oiBrowser)
	{
	char name[OBJECTNAMESIZE] ;

	SetWindowLong(hDlg, DWL_USER, (long)oiBrowser) ; // save the object ID in the window extra bytes
	HWND hText = GetDlgItem(hDlg, IDD_OBJECT) ; // added 9/7/93
	AUtlSetTitleFont((ATYPEID)OTYPE_BROWSER, NULL, hText); // added 9/7/93
	AUtlCenterDialog(hDlg, 0);

	ListDatabaseObjects(hDlg, oiBrowser) ;
	ListFieldObjects(hDlg, oiBrowser) ;

	AObjGetName(oiBrowser, name, OBJECTNAMESIZE) ;
	SetWindowText(hDlg, name) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ListDatabaseObjects
// DESCRIPTION:  List the database objects in the database combo box.
/////////////////////////////////////////////////////////////////////////////
void ListDatabaseObjects( HWND hDlg, AOBJECTID oiBrowser)
	{
	long key ;
	int i, nObjects ;
	char buffer[OBJECTNAMESIZE] ;
	BrowserInfo  *lpBrowserInfo ;
	AOBJECTID oiToAdd ;
	int objectIndex ;
	HWND hDataBase = GetDlgItem (hDlg, IDC_DATABASE) ;

	key = AOBJ_GETFIRSTKEY ;
	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	while((oiToAdd = AObjGetNextObject(OTYPE_DATABASE, &key)) != NULL)
		{
		AObjGetName(oiToAdd, buffer, OBJECTNAMESIZE) ;
		//objectIndex = (int)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_ADDSTRING, 0, (long)buffer) ;
		//SendDlgItemMessage(hDlg, IDC_DATABASE, CB_SETITEMDATA, objectIndex, (long)oiToAdd) ;
		objectIndex = (int)ComboBox_AddString (hDataBase,buffer) ;
		ComboBox_SetItemData (hDataBase, objectIndex, (long)oiToAdd) ;
		}
	lstrcpy(buffer, "(None)") ;
	//objectIndex = (int)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_INSERTSTRING, 0, (long)buffer) ;
	//SendDlgItemMessage(hDlg, IDC_DATABASE, CB_SETITEMDATA, objectIndex, 0) ;
	objectIndex = (int)ComboBox_InsertString (hDataBase, 0, buffer) ;
	ComboBox_SetItemData (hDataBase, objectIndex, 0) ;
	//nObjects = (int)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_GETCOUNT, 0, 0) ;
	nObjects = (int)ComboBox_GetCount (hDataBase) ;
	for (i = 0; i < nObjects; i++)
		{
		//oiToAdd = (AOBJECTID)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_GETITEMDATA, i, 0) ;
		oiToAdd = (AOBJECTID)ComboBox_GetItemData (GetDlgItem(hDlg, IDC_DATABASE), i) ;
		if (lpBrowserInfo->oiDatabase == oiToAdd)
			{
			//SendDlgItemMessage(hDlg, IDC_DATABASE, CB_SETCURSEL, i, 0) ;
			ComboBox_SetCurSel (GetDlgItem(hDlg, IDC_DATABASE), i) ;
			break ;
			}
		}
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ListFieldObjects
// DESCRIPTION:  Lists the indexable Field Objects for the selected database object.
/////////////////////////////////////////////////////////////////////////////
void ListFieldObjects( HWND hDlg, AOBJECTID oiBrowser)
	{
	BOOL fieldFound ;
	char buffer[OBJECTNAMESIZE] ;
	BrowserInfo  *lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	//SendDlgItemMessage(hDlg, IDC_FIELD, CB_RESETCONTENT, 0, 0) ;
	ComboBox_ResetContent (GetDlgItem(hDlg, IDC_FIELD)) ;
	AOBJECTID oiDatabase = lpBrowserInfo->oiDatabase ;
	if (oiDatabase == 0)
		{
		EnableWindow(GetDlgItem(hDlg, IDC_FIELD), FALSE) ;
		EnableWindow(GetDlgItem(hDlg, IDS_FIELD), FALSE) ;
		lpBrowserInfo->oiField = 0 ;
		AObjUnlockData(oiBrowser, INFO_INDEX) ;
		return ;
		}
	EnableWindow(GetDlgItem(hDlg, IDC_FIELD), TRUE) ;
	EnableWindow(GetDlgItem(hDlg, IDS_FIELD), TRUE) ;

/*	lpObjects = (AOBJECTID *) AObjLockData(lpBrowserInfo->oiDatabase, OBJECTSINDEX) ;
	lpKeys = (IndexInfo *) AObjLockData(lpBrowserInfo->oiDatabase, KEYSINDEX) ;
	nObjects = AObjGetDataSize(lpBrowserInfo->oiDatabase, KEYSINDEX) / sizeof(IndexInfo) ;
	fieldFound = FALSE ;
	for (i = 0; i < nObjects; i++)
		{
		oiToAdd = lpObjects[lpKeys[i].field - 1] ;
		AObjGetName(oiToAdd, buffer, OBJECTNAMESIZE) ;
		objectIndex = SendDlgItemMessage(hDlg, IDC_FIELD, CB_INSERTSTRING, -1, (long)buffer) ;
		SendDlgItemMessage(hDlg, IDC_FIELD, CB_SETITEMDATA, objectIndex, oiToAdd) ;
		if (oiToAdd == lpBrowserInfo->oiField)
			fieldFound = TRUE ;
		}
	if (fieldFound == FALSE)
		lpBrowserInfo->oiField = 0 ;
*/
	AMEMBLOCKID keyObjectIDs ;
	keyObjectIDs = AMemAllocate(0) ;
	ODatbGetKeyObjectIDs(oiDatabase, keyObjectIDs) ;
	AOBJECTID * lpKeys = (AOBJECTID *)AMemLock(keyObjectIDs) ;
	int nKeys = (int)AMemGetSize(keyObjectIDs) / sizeof(AOBJECTID) ;
	for (int i = 0 ; i < nKeys; i++)
		{
		AObjGetName(lpKeys[i], buffer, sizeof(buffer)) ;
		//int objectIndex = (int)SendDlgItemMessage(hDlg, IDC_FIELD, CB_INSERTSTRING, -1, (long)(LPSTR)buffer) ;
		int objectIndex = (int)ComboBox_InsertString (GetDlgItem(hDlg, IDC_FIELD), -1, buffer) ; 
		ComboBox_SetItemData (GetDlgItem(hDlg, IDC_FIELD), objectIndex, lpKeys[i]) ;
		//SendDlgItemMessage(hDlg, IDC_FIELD, CB_SETITEMDATA, objectIndex, (long)lpKeys[i]) ;
		if (lpKeys[i] == lpBrowserInfo->oiField)
			{
			fieldFound = TRUE ;
			}
		}
	if (fieldFound == FALSE)
		lpBrowserInfo->oiField = 0 ;
	AMemUnlock(keyObjectIDs) ;
	AMemFree(keyObjectIDs) ;

	lstrcpy(buffer, "(Active Item)") ;
	//int objectIndex = (int)SendDlgItemMessage(hDlg, IDC_FIELD, CB_INSERTSTRING, 0, (long)buffer) ;
	int objectIndex = (int)ComboBox_InsertString (GetDlgItem (hDlg, IDC_FIELD), 0, buffer) ;
	ComboBox_SetItemData (GetDlgItem(hDlg, IDC_FIELD), objectIndex, 0) ;
	//SendDlgItemMessage(hDlg, IDC_FIELD, CB_SETITEMDATA, objectIndex, 0) ;

	//int nObjects = (int)SendDlgItemMessage(hDlg, IDC_FIELD, CB_GETCOUNT, 0, 0) ;
	int nObjects = (int)ComboBox_GetCount (GetDlgItem(hDlg, IDC_FIELD)) ;
	for (i = 0; i < nObjects; i++)
		{
		//AOBJECTID oiField = (AOBJECTID)SendDlgItemMessage(hDlg, IDC_FIELD, CB_GETITEMDATA, i, 0) ;
		AOBJECTID oiField = (AOBJECTID) ComboBox_GetItemData (GetDlgItem(hDlg, IDC_FIELD), i) ;
		if (lpBrowserInfo->oiField == oiField)
			{
			//SendDlgItemMessage(hDlg, IDC_FIELD, CB_SETCURSEL, i, 0) ;
			ComboBox_SetCurSel (GetDlgItem(hDlg, IDC_FIELD), i) ;
			break ;
			}
		}
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	}
