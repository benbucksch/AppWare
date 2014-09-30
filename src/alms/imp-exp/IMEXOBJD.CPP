/////////////////////////////////////////////////////////////////////////////
// File Name:  ImExOBJD.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:
//
// Description.:
//	Contains the object definition routines and dialog callback
//	functions associated with the Serius Import/Export object:
//		StatusDlgProc: Dialog Procedure for the status dialog box
//		LibMain/*!!PORT!! Replace with DllEntryPoint */: saves the instance handle of the DLL
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <dos.h>
#include <memory.h>
#include <io.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_time.h>
#include <o_number.h>
#include <o_date.h>
#include <helpids.h>
#include "imexobj.h"
#include "imexdlg.h"

//void WindowShow(HWND, int, int, BOOL);
BOOL InitDialog(HWND, long) ;
BOOL AddItem(HWND) ;
BOOL RemoveItem(HWND) ;
BOOL EditItem(HWND) ;
BOOL UpdateFieldName(HWND) ;
BOOL ServiceSelectedList(HWND hDlg, WPARAM wParam) ;
BOOL ServiceAvailableList(HWND hDlg, WPARAM wParam) ;
BOOL UpdateObject(HWND) ;
//BOOL DrawObjectTypeName(HWND hDlg) ;

void GetFieldNameText( HWND hDlg, int index ) ;
void UpdateAvailableList (HWND hDlg, WORD buttonID) ;
int DeleteFieldName(AOBJECTID oiImportExport, int index) ;
int AddFieldName(AOBJECTID oiImportExport, int index) ;
int ChangeFieldName(AOBJECTID oiImportExport, int index, LPSTR lpNewName) ;
void AddObjectID(AOBJECTID oiImportExport, AOBJECTID oiField, int index) ;
void DeleteObjectID(AOBJECTID oiImportExport, int index) ;
void RemoveInvalidObjects( AOBJECTID oiImportExport ) ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  RemoveInvalidObjects
// DESCRIPTION:  Removes invalid objects from the Import-Export object's datas.
/////////////////////////////////////////////////////////////////////////////
void RemoveInvalidObjects( AOBJECTID oiImportExport )
	{
	AOBJECTID  *lpObjects ;
	long nObjects, i, j, nObjectsRemaining, datablock ;

	for (datablock = OBJECT_INDEX; datablock <= EXP_OBJ_INDEX; datablock++)
		{
		if (datablock == NAMES_INDEX)
			continue ;

		lpObjects = (AOBJECTID *)AObjLockData( oiImportExport, datablock ) ;
		nObjects = AObjGetDataSize (oiImportExport, datablock) / sizeof(AOBJECTID) ;
		nObjectsRemaining = nObjects ;
		for (i = nObjects-1; i >= 0; i--)
			{
			if (AObjCheckType(lpObjects[(int)i], 0) == 0)
				{
				if (datablock == OBJECT_INDEX)
					{
					DeleteFieldName(oiImportExport, (int)i) ;
					nObjectsRemaining-- ;
					for (j = i; j < nObjectsRemaining; j++)
						{
						lpObjects[(int)j] = lpObjects[(int)j+1] ; // shift objects
						}
					}
				}
			}
		if (lpObjects != NULL)
			AObjUnlockData( oiImportExport, datablock ) ;
		if (datablock == OBJECT_INDEX)
			AObjResizeData(oiImportExport, datablock, nObjectsRemaining*sizeof(AOBJECTID)) ;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doEditObject
// DESCRIPTION:  Calls the Import-Export object's definition routine.
/////////////////////////////////////////////////////////////////////////////
LONG doEditObject( AOBJECTID oiImportExport, pAObjMessage )
	{
	long size ;
	LPVOID lpSource, lpDestination ;

	int count = (int)AObjGetDataCount(oiImportExport) ;
	AObjSetDataCount(oiImportExport, 2 * count) ;
	for (int i = 0; i < count; i++)
		{
		size = AObjGetDataSize(oiImportExport, i) ;
		AObjResizeData(oiImportExport, i+count, size) ;
		if (size > 0)
			{
			lpSource = AObjLockData(oiImportExport, i) ;
			lpDestination = AObjLockData(oiImportExport, i+count) ;
			CopyMemory(lpDestination, lpSource, (int)size) ;
			AObjUnlockData(oiImportExport, i) ;
			AObjUnlockData(oiImportExport, i+count) ;
			}
		}
	RemoveInvalidObjects( oiImportExport ) ;
	LONG result = DialogBoxParam( hInstance, "ImportExport", GetActiveWindow(),
			(DLGPROC)EditObjectDialogProc, oiImportExport);
	if (result != A_MODIFIED)
		for (i = 0; i < count; i++) // Canceled out of dialog box
			{
			size = AObjGetDataSize(oiImportExport, count + i) ;
			AObjResizeData(oiImportExport, i, size) ;
			if (size > 0)
				{
				lpSource = AObjLockData(oiImportExport, count + i) ;
				lpDestination = AObjLockData(oiImportExport, i) ;
				CopyMemory(lpDestination, lpSource, (int)size) ;
				AObjUnlockData(oiImportExport, count + i) ;
				AObjUnlockData(oiImportExport, i) ;
				}
			}
	AObjSetDataCount(oiImportExport, count) ;
	return result ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doValidateObjectIDs
// DESCRIPTION:  Updates the object IDs stored in the object list
/////////////////////////////////////////////////////////////////////////////
LONG doValidateObjectIDs( AOBJECTID oiImportExport, pAObjMessage theSystem )
	{
	AOBJECTID  *lpObjects ;
	long nObjects, i ;

	// service datablock 1
	lpObjects = (AOBJECTID *)AObjLockData( oiImportExport, OBJECT_INDEX ) ;
	nObjects = AObjGetDataSize (oiImportExport, OBJECT_INDEX) / sizeof(AOBJECTID) ;
	for (i = nObjects-1; i >= 0; i--)
		AObjValidateID(&lpObjects[(int)i], theSystem) ;
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, OBJECT_INDEX ) ;

	// service datablock 2
	lpObjects = (AOBJECTID *)AObjLockData( oiImportExport, IMP_OBJ_INDEX ) ;
	nObjects = AObjGetDataSize (oiImportExport, IMP_OBJ_INDEX) / sizeof(AOBJECTID) ;
	for (i = nObjects-1; i >= 0; i--)
		AObjValidateID(&lpObjects[(int)i], theSystem) ;
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, IMP_OBJ_INDEX ) ;

	// service datablock 3
	lpObjects = (AOBJECTID *)AObjLockData( oiImportExport, EXP_OBJ_INDEX ) ;
	nObjects = AObjGetDataSize (oiImportExport, EXP_OBJ_INDEX) / sizeof(AOBJECTID) ;
	for (i = nObjects-1; i >= 0; i--)
		AObjValidateID(&lpObjects[(int)i], theSystem) ;
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, EXP_OBJ_INDEX ) ;
	return A_OK ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doEvaluateObject
// DESCRIPTION:  Verifies the existence of the object IDs stored in the
// 	object lists prior to compilation.
/////////////////////////////////////////////////////////////////////////////
LONG doEvaluateObject( AOBJECTID oiImportExport, pAObjMessage /*theSystem*/)
	{
//	pAObjInfo lpObjectInfo ;

	RemoveInvalidObjects( oiImportExport ) ;
//	lpObjectInfo = (pAObjInfo)theSystem->message2 ;

	if (!AObjGetDataSize (oiImportExport, OBJECT_INDEX))
		{
		return AObjReportError(oiImportExport, AOBJ_CHECKOBJECT, A_WARNING,
														"No field objects have been selected for the Import/Export object.",
														0);
		}
	return A_OK ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  EditObjectDialogProc
// DESCRIPTION:  The dialog callback routine for the Import/Export
//	object's edit dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EditObjectDialogProc (
	HWND hDlg, 
	UINT message,
	WPARAM wParam, 
	LPARAM lParam)
	{
	AOBJECTID oiImportExport ;
	switch (message)
		{
		case WM_INITDIALOG:
			return (InitDialog(hDlg, lParam)) ;

		case WM_COMMAND: //!!PORT!!
			WORD id = LOWORD (wParam) ;
			if (id >= IDD_TEXT && id <= IDD_OTHER) // radio buttons
				{
				CheckRadioButton (hDlg, IDD_TEXT, IDD_OTHER, id) ;
				UpdateAvailableList(hDlg, id) ;
				return TRUE ;
				}

			switch (id)
				{
				case IDL_AVAILABLE:
					return (ServiceAvailableList(hDlg, wParam)) ;

				case IDL_SELECTED:
					return (ServiceSelectedList(hDlg, wParam)) ;

				case IDD_ADD:
					return (AddItem(hDlg)) ;

				case IDD_REMOVE:
					return (RemoveItem(hDlg)) ;

				case IDD_EDIT:
					return (EditItem(hDlg)) ;

				case IDD_IMPORTCONF:
					oiImportExport = GetWindowLong (hDlg, DWL_USER) ;
					return DialogBoxParam(hInstance, "ImportConfiguration" , hDlg, (DLGPROC)ConfigImportDlgProc, oiImportExport) ;

				case IDD_EXPORTCONF:
					oiImportExport = GetWindowLong (hDlg, DWL_USER) ;
					return DialogBoxParam(hInstance, "ExportConfiguration" , hDlg, (DLGPROC)ConfigExportDlgProc, oiImportExport) ;

				case IDE_FIELDNAME:
					if (HIWORD(wParam) == EN_KILLFOCUS)
						return (UpdateFieldName(hDlg)) ;
					return FALSE ;

				case IDHELP:
					WinHelp(hDlg, "awusimex.hlp", HELP_CONTEXT, HELPID_OBJD) ;
					return TRUE ;

				case IDOK:
					EndDialog (hDlg, (int)A_MODIFIED) ;
					return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, (int)A_OK) ;
					return TRUE ;
				}
		}
	 return FALSE ;
	 }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  UpdateObject
// DESCRIPTION:  Updates the Import Export object's selected object list.
/////////////////////////////////////////////////////////////////////////////
BOOL  UpdateObject(HWND hDlg)
	{
	DWORD nObjects ;
	AOBJECTID  * lpObjects ;
	AOBJECTID oiImportExport ;
	int i ;
	HWND     hwndCtl = GetDlgItem(hDlg, IDL_SELECTED) ;

  nObjects = ListBox_GetCount (hwndCtl) ; //SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0L) ;
	oiImportExport = GetWindowLong (hDlg, DWL_USER) ;
	AObjResizeData (oiImportExport, OBJECT_INDEX, nObjects*sizeof(AOBJECTID)) ;
	lpObjects = (AOBJECTID *)AObjLockData (oiImportExport, OBJECT_INDEX) ;
	if (lpObjects)
		{
		for (i = 0; i < nObjects; i++)
			lpObjects[i] = ListBox_GetItemData (hwndCtl, i) ;
		//	lpObjects[i] = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETITEMDATA, i, 0);

		AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
		}
	EndDialog (hDlg, (int)A_MODIFIED) ;
	return TRUE ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ServiceAvailableList
// DESCRIPTION:  Service single or double mouse clicks in the available
//	list box.
/////////////////////////////////////////////////////////////////////////////
BOOL ServiceAvailableList(HWND hDlg, WPARAM wParam)
	{
	DWORD nObjects ; // number of objects selected from the "Available" list box
	switch (HIWORD (wParam))
		{
		case LBN_SELCHANGE:
			nObjects = ListBox_GetSelCount (GetDlgItem(hDlg, IDL_AVAILABLE)) ;
			//nObjects = SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELCOUNT, 0, 0L) ;
			if (nObjects)  // if one or more items are selected, enable the "Add" button
				EnableWindow(GetDlgItem(hDlg, IDD_ADD), TRUE) ;
			else // if no items are selected, disable the "Add" button
				EnableWindow(GetDlgItem(hDlg, IDD_ADD), FALSE) ;
			return TRUE;

		case LBN_DBLCLK:
			AddItem(hDlg) ;
			return TRUE;
		}
	return FALSE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ServiceSelectedList
// DESCRIPTION:  Service single or double mouse clicks in the selected
//	list box.
/////////////////////////////////////////////////////////////////////////////
BOOL ServiceSelectedList(HWND hDlg, WPARAM wParam)
	{
	DWORD nObjects ;
	int objectIndex ;

	//nObjects = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0L) ;
	nObjects = ListBox_GetSelCount (GetDlgItem(hDlg, IDL_SELECTED)) ;
	switch (HIWORD (wParam))
		{
		case LBN_SELCHANGE:
			if (nObjects) // if any objects are selected, enable the "Remove" button
				 EnableWindow (GetDlgItem(hDlg, IDD_REMOVE), TRUE) ;
			else // if no objects are selected, disable the "Remove" button
				 EnableWindow (GetDlgItem(hDlg, IDD_REMOVE), FALSE) ;

			if( nObjects == 1) // if only one object is selected, enable the "Edit" button
				{
				EnableWindow( GetDlgItem(hDlg, IDD_EDIT ), TRUE) ;
				ListBox_GetSelItems (GetDlgItem(hDlg, IDL_SELECTED), 1, &objectIndex) ;
				//SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long)(int *) &objectIndex) ;
				GetFieldNameText (hDlg, objectIndex) ;
				EnableWindow( GetDlgItem(hDlg, IDS_FIELDNAME ), TRUE) ;
				EnableWindow( GetDlgItem(hDlg, IDE_FIELDNAME ), TRUE) ;
				}
			else // otherwise, disable the "Edit" button and hide the "Field Name" controls
				{
				EnableWindow(GetDlgItem(hDlg, IDD_EDIT ), FALSE) ;
				EnableWindow( GetDlgItem(hDlg, IDS_FIELDNAME ), FALSE) ;
//				WindowShow(hDlg, IDS_FIELDNAME, SW_HIDE, FALSE) ;
				EnableWindow( GetDlgItem(hDlg, IDE_FIELDNAME ), FALSE) ;
				SetDlgItemText(hDlg, IDE_FIELDNAME, "") ;
//				WindowShow(hDlg, IDE_FIELDNAME, SW_HIDE, FALSE) ;
				}
			return TRUE ;

		case LBN_DBLCLK:
			if (nObjects == 1)
				{
				EditItem(hDlg) ;
				}
			return TRUE ;
		}
	return FALSE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetFieldNameText
// DESCRIPTION:  Writes the indexed object's field name to the "field name"
//	edit control.
/////////////////////////////////////////////////////////////////////////////
void GetFieldNameText( HWND hDlg, int index )
	{
	AOBJECTID oiImportExport ;
	LPSTR lpszNames ;
	int i, j ;

	oiImportExport = (AOBJECTID) GetWindowLong( hDlg, DWL_USER ) ;
	lpszNames = (char *) AObjLockData (oiImportExport, NAMES_INDEX) ;
	for (i = 0, j = 0; i < index; i++) // locate the beginning of the indexed field name
		while (lpszNames[j++]) ;
	SetDlgItemText (hDlg, IDE_FIELDNAME, &lpszNames[j]) ;
	if (lpszNames != NULL)
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  WindowShow
// DESCRIPTION:  Hides or reveals the dialog box control.
/////////////////////////////////////////////////////////////////////////////
void WindowShow(HWND hDlg, int ItemID, int Show, BOOL Status)
	{
	HWND hCtrl ;

	hCtrl = GetDlgItem(hDlg, ItemID) ;
	EnableWindow(hCtrl, Status) ;
	ShowWindow(hCtrl, Show) ;
	UpdateWindow(hCtrl) ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  UpdateAvailableList
// DESCRIPTION:  Updates the object's listed in the "Availble Objects"
//	list box.
/////////////////////////////////////////////////////////////////////////////
void UpdateAvailableList (HWND hDlg, WORD buttonID)
	{
	char buffer[OBJECTNAMESIZE] ;
	LONG typeID ;
	LONG key ;
	AOBJECTID oiToAdd ;
	LONG typeIndex ;
	HWND hwndCtl = GetDlgItem(hDlg, IDL_AVAILABLE) ;

	// clear all strings from the list box
	//SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_RESETCONTENT, 0, 0) ; // clear the list of its previous contents
	ListBox_ResetContent (GetDlgItem(hDlg, IDL_AVAILABLE)) ;
	EnableWindow(GetDlgItem(hDlg, IDD_ADD), FALSE) ; // disable the "ADD" button

	// if "other" object types are selected
	if (buttonID == IDD_OTHER)
		{
		typeIndex = AOBJ_GETFIRSTKEY ;
		while ((typeID = ATypeGetNext (&typeIndex)) != 0)
			{
			if (typeID == OTYPE_TEXT ||	typeID == OTYPE_NUMBER ||
				typeID == OTYPE_DATE || typeID == OTYPE_TIME )
					continue ;
			if (!AEvtCheckForMethod(typeID, AEVENT_OBJECTTOTEXT))
					continue ;
			key = AOBJ_GETFIRSTKEY ;
			while((oiToAdd = AObjGetNextObject(typeID, &key)) != 0)
				{
				AObjGetName(oiToAdd, buffer, OBJECTNAMESIZE) ;
				//int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_ADDSTRING, 0, (long)buffer) ;
				//SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, objectIndex, oiToAdd) ;
				int objectIndex = (int)ListBox_AddString (hwndCtl, buffer) ;
				ListBox_SetItemData (hwndCtl, objectIndex, oiToAdd) ;
				}
			}
		return ;
		}

	// if "text", "number", "date", or "time" object types are selected
	switch (buttonID)
		{
		case IDD_TEXT:
			typeID = OTYPE_TEXT ;
			break ;
		case IDD_NUMBER:
			typeID = OTYPE_NUMBER ;
			break ;
		case IDD_DATE:
			typeID = OTYPE_DATE ;
			break ;
		case IDD_TIME:
			typeID = OTYPE_TIME ;
			break ;
		}
	key = AOBJ_GETFIRSTKEY ;
	while((oiToAdd = AObjGetNextObject(typeID, &key)) != NULL)
		{
		AObjGetName(oiToAdd, buffer, OBJECTNAMESIZE) ;
		int objectIndex = (int)ListBox_AddString (hwndCtl, buffer) ;
		ListBox_SetItemData (hwndCtl, objectIndex, oiToAdd) ;
		//int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_ADDSTRING, 0, (long)buffer) ;
		//SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, objectIndex, oiToAdd) ;
		}
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the import / export edit dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL InitDialog ( HWND hDlg, AOBJECTID oiImportExport)
	{
	char buffer[OBJECTNAMESIZE] ;
	LPOBJECTID lpObjects ;
	int nObjects, i, objectIndex ;

	SetWindowLong(hDlg, DWL_USER, oiImportExport) ; // save the object ID in the window extra bytes
	HWND hText = GetDlgItem(hDlg, IDD_OBJECT) ; // added 9/7/93
	AUtlSetTitleFont(OTYPE_IMPORTEXPORT, NULL, hText); // added 9/7/93
	AUtlCenterDialog(hDlg, 0);

	AObjGetName(oiImportExport, buffer, OBJECTNAMESIZE) ;
	SetWindowText(hDlg, buffer) ;

	CheckRadioButton (hDlg, IDD_TEXT, IDD_OTHER, IDD_TEXT) ; // check the "Text" radio button
	UpdateAvailableList (hDlg, IDD_TEXT) ; // initialize the available objects list

	 // initialize the Import Export selected objects list
	lpObjects = (LPOBJECTID)AObjLockData(oiImportExport, OBJECT_INDEX) ; // identify the objects participating in the Import/Export object
	if (lpObjects == NULL) return TRUE ; // no objects in the list
	nObjects = (int)AObjGetDataSize(oiImportExport, OBJECT_INDEX) / sizeof(AOBJECTID) ;
	for(i = 0; i < nObjects; i++)
		{
		AObjGetName(lpObjects[i], buffer, OBJECTNAMESIZE) ;  // get the name of each participating object
		HWND hwndCtl = GetDlgItem (hDlg, IDL_SELECTED) ;
		objectIndex = (int)ListBox_AddString(hwndCtl, buffer) ;
    ListBox_SetItemData (hwndCtl, objectIndex, lpObjects[i]) ;
		//objectIndex = (int)SendDlgItemMessage(hDlg, IDL_SELECTED, LB_ADDSTRING, 0, (long)(LPSTR)buffer) ;
		//SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, objectIndex, lpObjects[i]) ;
		}
	AObjUnlockData(oiImportExport, OBJECT_INDEX) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  AddItem
// DESCRIPTION:  Adds the objects selected from the "Available Objects" list
// 	to the "Selected Objects" list.
/////////////////////////////////////////////////////////////////////////////
BOOL AddItem (HWND hDlg)
	{
	AOBJECTID oiObjectToAdd, oiImportExport ;
	long nCheckForDuplicates ;
	char buffer[OBJECTNAMESIZE] ;
	int * SelectedIndexes ;
	HWND hAvailable = GetDlgItem (hDlg, IDL_AVAILABLE) ;
	HWND hSelected  = GetDlgItem (hDlg, IDL_SELECTED) ;

	//int nObjects = (int)SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELCOUNT, 0, 0L) ; // number of objects selected in the "Available Objects" list box
	int nObjects = (int)ListBox_GetSelCount (hAvailable) ;

	if ((SelectedIndexes = new int[(int)nObjects]) == NULL) // initialize buffer to store the indexes of selected
		{
		MessageBox(hDlg, "Insufficient Memory", NULL, MB_OK) ;
		return FALSE ;
		}
	ListBox_GetSelItems (hAvailable, nObjects, SelectedIndexes) ;
	//SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	for (int i = 0; i < nObjects; i++)
		{
		ListBox_GetText (hAvailable, SelectedIndexes[i], buffer) ;
		//SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETTEXT, SelectedIndexes[i], (long)(LPSTR)buffer) ;
		oiObjectToAdd = ListBox_GetItemData (hAvailable, SelectedIndexes[i]) ;
		nCheckForDuplicates = ListBox_GetCount (hSelected) ;
		//oiObjectToAdd = SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_GETITEMDATA, SelectedIndexes[i], 0) ;
		//nCheckForDuplicates = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0) ;
		for (int j = 0; j < nCheckForDuplicates; j++)
			if (oiObjectToAdd == ListBox_GetItemData (hSelected, j))
			//if (oiObjectToAdd == SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, j, 0))
				continue ;
		int objectIndex = (int)ListBox_InsertString (hSelected, -1, buffer) ;
		ListBox_SetItemData (hSelected, objectIndex, oiObjectToAdd) ;
		//int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING, -1, (long)(LPSTR)buffer) ;
		//SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, objectIndex, oiObjectToAdd) ;
		AddObjectID(oiImportExport, oiObjectToAdd, (int)objectIndex) ;
		AddFieldName(oiImportExport, (int)objectIndex) ;
		}
	delete [] (SelectedIndexes) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  RemoveItem
// DESCRIPTION:  Removes the objects selected from the "Selected Objects" list.
/////////////////////////////////////////////////////////////////////////////
BOOL RemoveItem(HWND hDlg)
	{
	AOBJECTID oiImportExport ;
	int * SelectedIndexes ;
	int i ;
	HWND hwndCtl = GetDlgItem (hDlg, IDL_SELECTED) ;

	//int nObjects = (int)SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0L) ;
	int nObjects = (int) ListBox_GetSelCount (hwndCtl) ;
	SelectedIndexes = new int[(int)nObjects] ;
	ListBox_GetSelItems (hwndCtl, nObjects, SelectedIndexes) ;
	//SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	for (i = (int)nObjects-1 ; i >= 0; i--)
		{
		DeleteFieldName(oiImportExport, SelectedIndexes[i]) ;
		DeleteObjectID(oiImportExport, SelectedIndexes[i]) ;
		ListBox_DeleteString (hwndCtl, SelectedIndexes[i]) ;
		//SendDlgItemMessage (hDlg, IDL_SELECTED, LB_DELETESTRING, SelectedIndexes[i], 0L) ;
		}
	delete [] (SelectedIndexes) ;

	EnableWindow(GetDlgItem(hDlg, IDD_EDIT ), FALSE) ; // disable the edit button
	EnableWindow(GetDlgItem(hDlg, IDD_REMOVE ), FALSE) ; // disable the remove button
	EnableWindow( GetDlgItem(hDlg, IDS_FIELDNAME ), FALSE) ;
	EnableWindow( GetDlgItem(hDlg, IDE_FIELDNAME ), FALSE) ;
	SetDlgItemText( hDlg, IDE_FIELDNAME, "") ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  EditItem
// DESCRIPTION:  Edits an object from the "Selected Objects" list box.
/////////////////////////////////////////////////////////////////////////////
BOOL EditItem(HWND hDlg)
	{
	AOBJECTID oiToEdit ;
	long nObjects ;
	int objectIndex ;
	HWND hwndCtl = GetDlgItem(hDlg, IDL_SELECTED) ;

	nObjects = ListBox_GetSelCount (hwndCtl) ;
	//nObjects = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0L) ;
	if (nObjects != 1) return TRUE ;
	ListBox_GetSelItems (hwndCtl, 1, &objectIndex) ;
	oiToEdit = ListBox_GetItemData (hwndCtl, objectIndex) ;
//	SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long)(int *) &objectIndex) ;
//	oiToEdit = SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, objectIndex, 0) ;
	AObjEdit(oiToEdit) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  UpdateFieldName
// DESCRIPTION:  Updates the field name list in the Import/Export object's
//	data to reflect an editted object field name.  This function is called
//	when the programmer exits the "Field Name" edit control.
/////////////////////////////////////////////////////////////////////////////
BOOL UpdateFieldName(HWND hDlg)
	{
	AOBJECTID oiImportExport ;
	int index ;
	char * lpszString ;

	lpszString = new char[FIELDNAMESIZE] ;
	GetDlgItemText (hDlg, IDE_FIELDNAME, lpszString, FIELDNAMESIZE) ;
	ListBox_GetSelItems (GetDlgItem (hDlg, IDL_SELECTED), 1, &index) ;
	//SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long)(LPSTR)&index) ;
	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	ChangeFieldName (oiImportExport, index, lpszString) ;
	delete [] (lpszString) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DeleteObjectID
// DESCRIPTION:  Deletes an Object ID from the Import/Export object's object list.
/////////////////////////////////////////////////////////////////////////////
void DeleteObjectID(AOBJECTID oiImportExport, int index)
	{
	AOBJECTID  *lpObjects ;
	long nObjects ;
	int i ;

	nObjects = AObjGetDataSize(oiImportExport, OBJECT_INDEX) / sizeof(AOBJECTID) ;
	lpObjects = (AOBJECTID *) AObjLockData (oiImportExport, OBJECT_INDEX) ;

	if (index > nObjects) return ;

	for (i = index; i < nObjects-1; i++)
		{
		lpObjects[i] = lpObjects[i+1] ;
		}
	if (lpObjects != NULL)
		AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
	AObjResizeData(oiImportExport, OBJECT_INDEX, sizeof(AOBJECTID)*(nObjects-1)) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DeleteFieldName
// DESCRIPTION:  Deletes a field name from the Import/Export object's field name list.
/////////////////////////////////////////////////////////////////////////////
DeleteFieldName(AOBJECTID oiImportExport, int index)
	{
	char  *lpTemp ;

	LPSTR lpszNames = (char *)AObjLockData(oiImportExport, NAMES_INDEX) ;
	int j = 0 ;
	for (int i = 0; i < index; i++) // locate the beginning of the indexed field name
		while (lpszNames[j++]) ;

	int nOldDataSize = (int)AObjGetDataSize (oiImportExport, NAMES_INDEX) ; // size of the field names data
	int nOldStrLength = lstrlen(&lpszNames[j]) + 1;
	int nNewDataSize = nOldDataSize - nOldStrLength ;
	if ((lpTemp = new char[nNewDataSize]) == NULL) // create a temporary copy of the field names data
		{
		if (lpszNames != NULL)
			AObjUnlockData (oiImportExport, NAMES_INDEX) ;
		return 0 ; // indicates OUT OF MEMORY
		}
	CopyMemory (lpTemp, lpszNames, j) ; // copy the list up to the new name
	CopyMemory (&lpTemp[j], &lpszNames[j+nOldStrLength], nNewDataSize - j) ; // copy the list after the field name
	if (lpszNames != NULL)
		AObjUnlockData(oiImportExport, NAMES_INDEX) ;
	AObjResizeData(oiImportExport, NAMES_INDEX, nNewDataSize) ; // resize data block
	if (lpszNames != NULL)
		{
		lpszNames = (char *)AObjLockData(oiImportExport, NAMES_INDEX) ; // resize data block
		CopyMemory(lpszNames, lpTemp, nNewDataSize) ; // save new data
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
		}
	delete [] lpTemp ; // delete the temporary copy
	return 1 ; // indicates OKAY
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  AddObjectID
// DESCRIPTION:  Adds an object ID to the Import/Export object ID list.
/////////////////////////////////////////////////////////////////////////////
void AddObjectID(AOBJECTID oiImportExport, AOBJECTID oiField, int index)
	{
	int Size = (int)AObjGetDataSize(oiImportExport, OBJECT_INDEX) ;
	Size += sizeof(AOBJECTID) ;
	AObjResizeData (oiImportExport, OBJECT_INDEX, Size) ;
	AOBJECTID * lpObjects = (AOBJECTID *) AObjLockData (oiImportExport, OBJECT_INDEX) ;
	int nObjects = Size / sizeof(AOBJECTID) ;
	if (index > nObjects)
		index = nObjects ;

	for (int i = index; i < nObjects - 1; i++)
		{
		lpObjects[i+1] = lpObjects[i] ;
		}
	lpObjects[index] = oiField ;
	AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  AddFieldName
// DESCRIPTION:  Adds a field name to the Import/Export object's
//		field name list.  When a new field name is added it is always the
//		object name.  The field name may be changed after the object is added.
/////////////////////////////////////////////////////////////////////////////
AddFieldName(AOBJECTID oiImportExport, int index)
	{
	char  *lpTemp ;
	char ObjectName[OBJECTNAMESIZE] ;

	LPSTR lpszNames = (LPSTR)AObjLockData (oiImportExport, NAMES_INDEX) ;
	int j = 0 ;
	for (int i = 0; i < index; i++) // locate the beginning of the indexed field name
		while (lpszNames[j++]) ;

	int nOldDataSize = (int)AObjGetDataSize (oiImportExport, NAMES_INDEX) ; // size of the field names data
	AOBJECTID * lpObjects = (AOBJECTID *) AObjLockData (oiImportExport, OBJECT_INDEX) ;
	AOBJECTID oiField = 0 ;
	if (lpObjects != NULL)
		{
		oiField = lpObjects[index] ;
		AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
		}
	int nNewStrLength = 1 + (int)AObjGetName(oiField, ObjectName, OBJECTNAMESIZE) ; // object name
	int nNewDataSize = nOldDataSize + nNewStrLength ;
	if ((lpTemp = new char[(int)nNewDataSize]) == NULL) // create a temporary copy of the field names data
		return 0 ; // indicates OUT OF MEMORY
	CopyMemory (lpTemp, lpszNames, j) ; // copy the list up to the new name
	CopyMemory (&lpTemp[j], ObjectName, nNewStrLength) ; // write the new name
	CopyMemory (&lpTemp[j+nNewStrLength], &lpszNames[j], (int)nOldDataSize - j) ; // copy the list after the field name
	if (lpszNames != NULL)
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	AObjResizeData(oiImportExport, NAMES_INDEX, nNewDataSize) ; // resize data block
	lpszNames = (char *) AObjLockData (oiImportExport, NAMES_INDEX) ;
	CopyMemory(lpszNames, lpTemp, (int)nNewDataSize) ; // save new data
	AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	delete [] lpTemp ; // delete the temporary copy
	return 1 ; // indicates OKAY
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ChangeFieldName
// DESCRIPTION:  Changes a field name in the Import/Export object's
//		field name list.
/////////////////////////////////////////////////////////////////////////////
ChangeFieldName(AOBJECTID oiImportExport, int index, LPSTR lpNewName)
	{
	char  *lpTemp ;

	LPSTR lpszNames = (LPSTR)AObjLockData (oiImportExport, NAMES_INDEX) ;
	int j = 0 ;
	for (int i = 0; i < index; i++) // locate the beginning of the indexed field name
		while (lpszNames[j++]) ;

	if (!lstrcmpi(&lpszNames[j], lpNewName))
		return 1 ; // if the new and old names are identical no work is required

	int nOldDataSize = (int)AObjGetDataSize (oiImportExport, NAMES_INDEX) ; // size of the field names data
	int nNewStrLength = lstrlen(lpNewName) + 1;
	int nOldStrLength = lstrlen(&lpszNames[j]) + 1;
	int nNewDataSize = nOldDataSize - nOldStrLength + nNewStrLength ;
	if ((lpTemp = new char[(int)nNewDataSize]) == NULL) // create a temporary copy of the field names data
		{
		if (lpszNames != NULL)
			AObjUnlockData (oiImportExport, NAMES_INDEX) ;
		return 0 ; // indicates OUT OF MEMORY
		}
	CopyMemory (lpTemp, lpszNames, j) ; // copy the list up to the new name
	CopyMemory (&lpTemp[j], lpNewName, nNewStrLength) ; // write the new name
	CopyMemory (&lpTemp[j+nNewStrLength], &lpszNames[j+nOldStrLength], (int)(nOldDataSize - (j + nOldStrLength))) ; // copy the list after the field name
	if (lpszNames != NULL)
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	AObjResizeData(oiImportExport, NAMES_INDEX, nNewDataSize) ; // resize data block
	lpszNames = (LPSTR) AObjLockData (oiImportExport, NAMES_INDEX) ;
	CopyMemory(lpszNames, lpTemp, (int)nNewDataSize) ; // save new data
	if (lpszNames != NULL)
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	delete [] lpTemp ; // delete the temporary copy
	return 1 ; // indicates OKAY
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DrawStatusBar
// DESCRIPTION:  Draw Status Bar in the Import/Export status dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL DrawStatusBar(HWND hDlg, BOOL /*RefreshBackground*/)
	{
	AOBJECTID oiImportExport ;
	ImpExpInfo  *lpImpExpInfo ;
	float fraction ;
	HDC hdc ;
	RECT rect ;
	HWND hBar ;
	HPEN hPen ;
	HBRUSH hBrush ;

	hBar = GetDlgItem(hDlg, IDD_BAR) ;
	hdc = GetDC(hBar) ;
	GetClientRect(hBar, &rect) ;
	hPen = (HPEN)SelectObject( hdc, GetStockObject(NULL_PEN)) ;
	hBrush = (HBRUSH)SelectObject( hdc, GetStockObject(GRAY_BRUSH)) ;

	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	lpImpExpInfo = (ImpExpInfo  *) AObjLockData(oiImportExport, INFO_INDEX) ;
	if (lpImpExpInfo->Status == O_IMPORTING)
		{
		if (lpImpExpInfo->FileSize > 0)
			fraction = (lpImpExpInfo->lpRuntimeInfo->wMark
				+ lpImpExpInfo->lpRuntimeInfo->lBlockBytes)
				/ (lpImpExpInfo->FileSize * 1.0) ;
		else
			fraction = 0.0 ;
		}
	else
		{
		if (lpImpExpInfo->lpRuntimeInfo->finalStatus > 0)
			fraction = lpImpExpInfo->lpRuntimeInfo->currentStatus
				/ (lpImpExpInfo->lpRuntimeInfo->finalStatus * 1.0) ;
		else
			fraction = 0.0 ;
		}
	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	if (fraction < 0.0)
		fraction = 0.0 ;
	else if (fraction > 1.0)
		fraction = 1.0 ;

	Rectangle(hdc, rect.left, rect.top, rect.right*fraction+1, rect.bottom+1) ;

	SelectObject(hdc, hBrush) ;
	SelectObject(hdc, hPen) ;
	ReleaseDC(hBar, hdc) ;
	return FALSE ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  StatusDlgProc
// DESCRIPTION:  Callback routine for the Import/Export status dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK StatusDlgProc (
	HWND hDlg, 
	UINT message,
	WPARAM wParam, 
	LPARAM lParam)
	{
	AOBJECTID oiImportExport ;
	ImpExpInfo  *lpImpExpInfo ;
	HWND hBar ;

	switch (message)
		{
		case WM_INITDIALOG:
			oiImportExport = lParam ;
			lpImpExpInfo = (ImpExpInfo *) AObjLockData (oiImportExport, 0) ;
			if (lpImpExpInfo->Status == O_IMPORTING)
				{
				SetWindowText(hDlg, "Import Status") ;
				SetDlgItemText(hDlg, ID_TEXT, "Records Imported =") ;
				}
			else
				{
				SetWindowText(hDlg, "Export Status") ;
				SetDlgItemText(hDlg, ID_TEXT, "Records Exported =") ;
				}
			AObjUnlockData (oiImportExport, 0) ;
			SetWindowLong(hDlg, DWL_USER, oiImportExport) ;

			SetDlgItemInt(hDlg, ID_RECORDS, 0, FALSE) ;
			return TRUE ;

		case WM_USER :
			SetDlgItemInt(hDlg, ID_RECORDS, LOWORD(lParam), FALSE) ;
			DrawStatusBar(hDlg, FALSE) ;
			return TRUE ;

		case WM_PAINT :
			hBar = GetDlgItem(hDlg, IDD_BAR) ;
			InvalidateRect(hBar, NULL, TRUE) ;
			UpdateWindow(hBar) ;
			return DrawStatusBar(hDlg, TRUE) ;

		case WM_COMMAND : //!!PORT!!
			switch (LOWORD(wParam))
				{
				case IDCANCEL:
					oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
					lpImpExpInfo = (ImpExpInfo *) AObjLockData (oiImportExport, 0) ;
					lpImpExpInfo->lpRuntimeInfo->Abort = TRUE ;
					AObjUnlockData (oiImportExport, 0) ;
					return TRUE ;
				}
		}
		return FALSE ;
	}
