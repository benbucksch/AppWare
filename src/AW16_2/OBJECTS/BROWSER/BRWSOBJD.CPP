/////////////////////////////////////////////////////////////////////////////
// File Name:  BrwsObjD.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  8/01/92
//
// Description.: Object Definition Routine for the Serius Browser object:
////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <mem.h>
#include <a_alm.h>
//#include <seriusid.h>
#include <a_almutl.h>
#include <helpids.h>
#include <o_datb.h>
#include "browser.h"

BOOL InitDialog(HWND, OBJECTID) ;
BOOL FieldList(HWND hDlg, long lParam) ;
BOOL DatabaseList(HWND hDlg, long lParam) ;
void ListDatabaseObjects( HWND hDlg, OBJECTID oiBrowser) ;
void ListFieldObjects( HWND hDlg, OBJECTID oiBrowser) ;

struct OBJECTSIGNALS {
	char far* name ;
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
LONG EditObject( OBJECTID oiBrowser )
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
		_fmemcpy(lpDestination, lpSource, (int)size) ;
		AObjUnlockData(oiBrowser, i) ;
		AObjUnlockData(oiBrowser, i+nCount) ;
		}
	BOOL result = DialogBoxParam( hInstance, "Browser", GetActiveWindow(),
					(FARPROC)EditObjectDialogProc, (long)oiBrowser) ;
	if (result == FALSE)
		{
		for (i = 0; i < nCount; i++)
			{
			size = AObjGetDataSize(oiBrowser, nCount + i) ;
			lpSource = AObjLockData(oiBrowser, nCount + i) ;
			AObjResizeData(oiBrowser, i, size) ;
			lpDestination = AObjLockData(oiBrowser, i) ;
			_fmemcpy(lpDestination, lpSource, (int)size) ;
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
	MEMBLOCKID signalBlock ;
	AObjSignalRecord far *lpSignal ;
	int i, nSignals ;

	signalBlock = (MEMBLOCKID)theSystem->message2 ;
	nSignals = sizeof(signals)/sizeof(signals[0]) ;
	AMemSetSize(signalBlock, nSignals*sizeof(AObjSignalRecord)) ;
	lpSignal = (AObjSignalRecord far*)AMemLock(signalBlock) ;
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

LONG EvaluateObject( OBJECTID oiBrowser, pAObjMessage theSystem )
	{
	BrowserInfo far *lpBrowser ;

	pAObjInfo lpObjectInfo = (pAObjInfo)theSystem->message2 ;
	lpObjectInfo->typeOverhead = 8192 ; // for button pallette bitmap

	lpBrowser = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	VerifyDatabaseAndField(lpBrowser) ;
	OBJECTID oiDatabase = lpBrowser->oiDatabase ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	if (oiDatabase == 0)
		{
		return AObjReportError(oiBrowser, AOBJ_CHECKOBJECT, A_WARNING,
										"No database object is assigned to the Browser.",
										0);
		}
	return A_OK ;
	}

LONG ValidateObjectIDs( OBJECTID oiBrowser, pAObjMessage theSystem )
	{
	BrowserInfo far *lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	AObjValidateID(&lpBrowserInfo->oiDatabase, theSystem) ;
	AObjValidateID(&lpBrowserInfo->oiField, theSystem) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return A_OK ;
	}

BOOL FAR PASCAL _export EditObjectDialogProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			return (InitDialog(hDlg, (OBJECTID)lParam)) ;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDC_DATABASE:
					return (DatabaseList(hDlg, lParam)) ;

				case IDC_FIELD:
					return (FieldList(hDlg, lParam)) ;

				case IDHELP:
					WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Brws) ;
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
BOOL DatabaseList(HWND hDlg, long lParam)
	{
	OBJECTID oiBrowser ;
	BrowserInfo far * lpBrowserInfo ;
	int objectIndex ;

	if (HIWORD (lParam) != CBN_SELCHANGE)
		return FALSE ;

	oiBrowser = (OBJECTID)GetWindowLong(hDlg, DWL_USER) ;
	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	objectIndex = (int)SendDlgItemMessage (hDlg, IDC_DATABASE, CB_GETCURSEL, 0, 0) ;
	lpBrowserInfo->oiDatabase = (OBJECTID)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_GETITEMDATA, objectIndex, 0) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	ListFieldObjects(hDlg, oiBrowser) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FieldList
// DESCRIPTION:  Services the field list combo box.
/////////////////////////////////////////////////////////////////////////////
BOOL FieldList(HWND hDlg, long lParam)
	{
	OBJECTID oiBrowser ;
	BrowserInfo far * lpBrowserInfo ;
	int objectIndex ;

	if (HIWORD (lParam) != CBN_SELCHANGE)
		return FALSE ;

	oiBrowser = (OBJECTID)GetWindowLong (hDlg, DWL_USER) ;
	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	objectIndex = (int)SendDlgItemMessage (hDlg, IDC_FIELD, CB_GETCURSEL, 0, 0) ;
	lpBrowserInfo->oiField = (OBJECTID)SendDlgItemMessage (hDlg, IDC_FIELD, CB_GETITEMDATA, objectIndex, 0) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return TRUE ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the import / export edit dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL InitDialog(HWND hDlg, OBJECTID oiBrowser)
	{
	char name[OBJECTNAMESIZE] ;

	SetWindowLong(hDlg, DWL_USER, (long)oiBrowser) ; // save the object ID in the window extra bytes
	HWND hText = GetDlgItem(hDlg, IDD_OBJECT) ; // added 9/7/93
	AUtlSetTitleFont((TYPEID)OTYPE_BROWSER, NULL, hText); // added 9/7/93
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
void ListDatabaseObjects( HWND hDlg, OBJECTID oiBrowser)
	{
	long key ;
	int i, nObjects ;
	char buffer[OBJECTNAMESIZE] ;
	BrowserInfo far *lpBrowserInfo ;
	OBJECTID oiToAdd ;
	int objectIndex ;

	key = AOBJ_GETFIRSTKEY ;
	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	while((oiToAdd = AObjGetNextObject(OTYPE_DATABASE, &key)) != NULL)
		{
		AObjGetName(oiToAdd, buffer, OBJECTNAMESIZE) ;
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage(hDlg, IDC_DATABASE, CB_SETITEMDATA, objectIndex, (long)oiToAdd) ;
		}
	lstrcpy(buffer, "(None)") ;
	objectIndex = (int)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_INSERTSTRING, 0, (long)buffer) ;
	SendDlgItemMessage(hDlg, IDC_DATABASE, CB_SETITEMDATA, objectIndex, 0) ;

	nObjects = (int)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_GETCOUNT, 0, 0) ;
	for (i = 0; i < nObjects; i++)
		{
		oiToAdd = (OBJECTID)SendDlgItemMessage(hDlg, IDC_DATABASE, CB_GETITEMDATA, i, 0) ;
		if (lpBrowserInfo->oiDatabase == oiToAdd)
			{
			SendDlgItemMessage(hDlg, IDC_DATABASE, CB_SETCURSEL, i, 0) ;
			break ;
			}
		}
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ListFieldObjects
// DESCRIPTION:  Lists the indexable Field Objects for the selected database object.
/////////////////////////////////////////////////////////////////////////////
void ListFieldObjects( HWND hDlg, OBJECTID oiBrowser)
	{
	BOOL fieldFound ;
	char buffer[OBJECTNAMESIZE] ;
	BrowserInfo far *lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	SendDlgItemMessage(hDlg, IDC_FIELD, CB_RESETCONTENT, 0, 0) ;
	OBJECTID oiDatabase = lpBrowserInfo->oiDatabase ;
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

/*	lpObjects = (OBJECTID far*) AObjLockData(lpBrowserInfo->oiDatabase, OBJECTSINDEX) ;
	lpKeys = (IndexInfo far*) AObjLockData(lpBrowserInfo->oiDatabase, KEYSINDEX) ;
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
	MEMBLOCKID keyObjectIDs ;
	keyObjectIDs = AMemAllocate(0) ;
	ODatbGetKeyObjectIDs(oiDatabase, keyObjectIDs) ;
	OBJECTID far* lpKeys = (OBJECTID far*)AMemLock(keyObjectIDs) ;
	int nKeys = (int)AMemGetSize(keyObjectIDs) / sizeof(OBJECTID) ;
	for (int i = 0 ; i < nKeys; i++)
		{
		AObjGetName(lpKeys[i], buffer, sizeof(buffer)) ;
		int objectIndex = (int)SendDlgItemMessage(hDlg, IDC_FIELD, CB_INSERTSTRING, -1, (long)(LPSTR)buffer) ;
		SendDlgItemMessage(hDlg, IDC_FIELD, CB_SETITEMDATA, objectIndex, (long)lpKeys[i]) ;
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
	int objectIndex = (int)SendDlgItemMessage(hDlg, IDC_FIELD, CB_INSERTSTRING, 0, (long)buffer) ;
	SendDlgItemMessage(hDlg, IDC_FIELD, CB_SETITEMDATA, objectIndex, 0) ;

	int nObjects = (int)SendDlgItemMessage(hDlg, IDC_FIELD, CB_GETCOUNT, 0, 0) ;
	for (i = 0; i < nObjects; i++)
		{
		OBJECTID oiField = (OBJECTID)SendDlgItemMessage(hDlg, IDC_FIELD, CB_GETITEMDATA, i, 0) ;
		if (lpBrowserInfo->oiField == oiField)
			{
			SendDlgItemMessage(hDlg, IDC_FIELD, CB_SETCURSEL, i, 0) ;
			break ;
			}
		}
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	}
