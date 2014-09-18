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
//		LibMain: saves the instance handle of the DLL
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <dos.h>
#include <mem.h>
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
BOOL ServiceSelectedList(HWND hDlg, LONG lParam) ;
BOOL ServiceAvailableList(HWND hDlg, LONG lParam) ;
BOOL UpdateObject(HWND) ;
//BOOL DrawObjectTypeName(HWND hDlg) ;

void GetFieldNameText( HWND hDlg, int index ) ;
void UpdateAvailableList (HWND hDlg, WORD buttonID) ;
int DeleteFieldName(OBJECTID oiImportExport, int index) ;
int AddFieldName(OBJECTID oiImportExport, int index) ;
int ChangeFieldName(OBJECTID oiImportExport, int index, LPSTR lpNewName) ;
void AddObjectID(OBJECTID oiImportExport, OBJECTID oiField, int index) ;
void DeleteObjectID(OBJECTID oiImportExport, int index) ;
void RemoveInvalidObjects( OBJECTID oiImportExport ) ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  RemoveInvalidObjects
// DESCRIPTION:  Removes invalid objects from the Import-Export object's datas.
/////////////////////////////////////////////////////////////////////////////
void RemoveInvalidObjects( OBJECTID oiImportExport )
	{
	OBJECTID far *lpObjects ;
	long nObjects, i, j, nObjectsRemaining, datablock ;

	for (datablock = OBJECT_INDEX; datablock <= EXP_OBJ_INDEX; datablock++)
		{
		if (datablock == NAMES_INDEX)
			continue ;

		lpObjects = (OBJECTID far*)AObjLockData( oiImportExport, datablock ) ;
		nObjects = AObjGetDataSize (oiImportExport, datablock) / sizeof(OBJECTID) ;
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
			AObjResizeData(oiImportExport, datablock, nObjectsRemaining*sizeof(OBJECTID)) ;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  doEditObject
// DESCRIPTION:  Calls the Import-Export object's definition routine.
/////////////////////////////////////////////////////////////////////////////
LONG doEditObject( OBJECTID oiImportExport, pAObjMessage )
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
			_fmemcpy(lpDestination, lpSource, (int)size) ;
			AObjUnlockData(oiImportExport, i) ;
			AObjUnlockData(oiImportExport, i+count) ;
			}
		}
	RemoveInvalidObjects( oiImportExport ) ;
	LONG result = DialogBoxParam( hInstance, "ImportExport", GetActiveWindow(),
			(FARPROC)EditObjectDialogProc, oiImportExport);
	if (result != A_MODIFIED)
		for (i = 0; i < count; i++) // Canceled out of dialog box
			{
			size = AObjGetDataSize(oiImportExport, count + i) ;
			AObjResizeData(oiImportExport, i, size) ;
			if (size > 0)
				{
				lpSource = AObjLockData(oiImportExport, count + i) ;
				lpDestination = AObjLockData(oiImportExport, i) ;
				_fmemcpy(lpDestination, lpSource, (int)size) ;
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
LONG doValidateObjectIDs( OBJECTID oiImportExport, pAObjMessage theSystem )
	{
	OBJECTID far *lpObjects ;
	long nObjects, i ;

	// service datablock 1
	lpObjects = (OBJECTID far*)AObjLockData( oiImportExport, OBJECT_INDEX ) ;
	nObjects = AObjGetDataSize (oiImportExport, OBJECT_INDEX) / sizeof(OBJECTID) ;
	for (i = nObjects-1; i >= 0; i--)
		AObjValidateID(&lpObjects[(int)i], theSystem) ;
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, OBJECT_INDEX ) ;

	// service datablock 2
	lpObjects = (OBJECTID far*)AObjLockData( oiImportExport, IMP_OBJ_INDEX ) ;
	nObjects = AObjGetDataSize (oiImportExport, IMP_OBJ_INDEX) / sizeof(OBJECTID) ;
	for (i = nObjects-1; i >= 0; i--)
		AObjValidateID(&lpObjects[(int)i], theSystem) ;
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, IMP_OBJ_INDEX ) ;

	// service datablock 3
	lpObjects = (OBJECTID far*)AObjLockData( oiImportExport, EXP_OBJ_INDEX ) ;
	nObjects = AObjGetDataSize (oiImportExport, EXP_OBJ_INDEX) / sizeof(OBJECTID) ;
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
LONG doEvaluateObject( OBJECTID oiImportExport, pAObjMessage /*theSystem*/)
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
BOOL FAR PASCAL _export EditObjectDialogProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
	{
	OBJECTID oiImportExport ;
	switch (message)
		{
		case WM_INITDIALOG:
			return (InitDialog(hDlg, lParam)) ;

		case WM_COMMAND:
			if (wParam >= IDD_TEXT && wParam <= IDD_OTHER) // radio buttons
				{
				CheckRadioButton (hDlg, IDD_TEXT, IDD_OTHER, wParam) ;
				UpdateAvailableList(hDlg, wParam) ;
				return TRUE ;
				}

			switch (wParam)
				{
				case IDL_AVAILABLE:
					return (ServiceAvailableList(hDlg, lParam)) ;

				case IDL_SELECTED:
					return (ServiceSelectedList(hDlg, lParam)) ;

				case IDD_ADD:
					return (AddItem(hDlg)) ;

				case IDD_REMOVE:
					return (RemoveItem(hDlg)) ;

				case IDD_EDIT:
					return (EditItem(hDlg)) ;

				case IDD_IMPORTCONF:
					oiImportExport = GetWindowLong (hDlg, DWL_USER) ;
					return DialogBoxParam(hInstance, "ImportConfiguration" , hDlg, (FARPROC)ConfigImportDlgProc, oiImportExport) ;

				case IDD_EXPORTCONF:
					oiImportExport = GetWindowLong (hDlg, DWL_USER) ;
					return DialogBoxParam(hInstance, "ExportConfiguration" , hDlg, (FARPROC)ConfigExportDlgProc, oiImportExport) ;

				case IDE_FIELDNAME:
					if (HIWORD(lParam) == EN_KILLFOCUS)
						return (UpdateFieldName(hDlg)) ;
					return FALSE ;

				case IDHELP:
					WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Imex) ;
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
	OBJECTID far * lpObjects ;
	OBJECTID oiImportExport ;
	int i ;

	nObjects = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0L) ;
	oiImportExport = GetWindowLong (hDlg, DWL_USER) ;
	AObjResizeData (oiImportExport, OBJECT_INDEX, nObjects*sizeof(OBJECTID)) ;
	lpObjects = (OBJECTID far*)AObjLockData (oiImportExport, OBJECT_INDEX) ;
	if (lpObjects)
		{
		for (i = 0; i < nObjects; i++)
			lpObjects[i] = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETITEMDATA, i, 0);

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
BOOL ServiceAvailableList(HWND hDlg, LONG lParam)
	{
	DWORD nObjects ; // number of objects selected from the "Available" list box
	switch (HIWORD (lParam))
		{
		case LBN_SELCHANGE:
			nObjects = SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELCOUNT, 0, 0L) ;
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
BOOL ServiceSelectedList(HWND hDlg, LONG lParam)
	{
	DWORD nObjects ;
	int objectIndex ;

	nObjects = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0L) ;
	switch (HIWORD (lParam))
		{
		case LBN_SELCHANGE:
			if (nObjects) // if any objects are selected, enable the "Remove" button
				 EnableWindow (GetDlgItem(hDlg, IDD_REMOVE), TRUE) ;
			else // if no objects are selected, disable the "Remove" button
				 EnableWindow (GetDlgItem(hDlg, IDD_REMOVE), FALSE) ;

			if( nObjects == 1) // if only one object is selected, enable the "Edit" button
				{
				EnableWindow( GetDlgItem(hDlg, IDD_EDIT ), TRUE) ;
				SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long)(int far*) &objectIndex) ;
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
	OBJECTID oiImportExport ;
	LPSTR lpszNames ;
	int i, j ;

	oiImportExport = (OBJECTID) GetWindowLong( hDlg, DWL_USER ) ;
	lpszNames = (char far*) AObjLockData (oiImportExport, NAMES_INDEX) ;
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
	OBJECTID oiToAdd ;
	LONG typeIndex ;

	// clear all strings from the list box
	SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_RESETCONTENT, 0, 0) ; // clear the list of its previous contents
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
				int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_ADDSTRING, 0, (long)buffer) ;
				SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, objectIndex, oiToAdd) ;
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
		int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, objectIndex, oiToAdd) ;
		}
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the import / export edit dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL InitDialog ( HWND hDlg, OBJECTID oiImportExport)
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
	nObjects = (int)AObjGetDataSize(oiImportExport, OBJECT_INDEX) / sizeof(OBJECTID) ;
	for(i = 0; i < nObjects; i++)
		{
		AObjGetName(lpObjects[i], buffer, OBJECTNAMESIZE) ;  // get the name of each participating object
		objectIndex = (int)SendDlgItemMessage(hDlg, IDL_SELECTED, LB_ADDSTRING, 0, (long)(LPSTR)buffer) ;
		SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, objectIndex, lpObjects[i]) ;
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
	OBJECTID oiObjectToAdd, oiImportExport ;
	long nCheckForDuplicates ;
	char buffer[OBJECTNAMESIZE] ;
	int far* SelectedIndexes ;

	int nObjects = (int)SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELCOUNT, 0, 0L) ; // number of objects selected in the "Available Objects" list box

	if ((SelectedIndexes = new int[(int)nObjects]) == NULL) // initialize buffer to store the indexes of selected
		{
		MessageBox(hDlg, "Insufficient Memory", NULL, MB_OK) ;
		return FALSE ;
		}
	SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	for (int i = 0; i < nObjects; i++)
		{
		SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETTEXT, SelectedIndexes[i], (long)(LPSTR)buffer) ;
		oiObjectToAdd = SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_GETITEMDATA, SelectedIndexes[i], 0) ;
		nCheckForDuplicates = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0) ;
		for (int j = 0; j < nCheckForDuplicates; j++)
			if (oiObjectToAdd == SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, j, 0))
				continue ;

		int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING, -1, (long)(LPSTR)buffer) ;
		SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, objectIndex, oiObjectToAdd) ;
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
	OBJECTID oiImportExport ;
	int far* SelectedIndexes ;
	int i ;

	int nObjects = (int)SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0L) ;
	SelectedIndexes = new int[(int)nObjects] ;
	SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	for (i = (int)nObjects-1 ; i >= 0; i--)
		{
		DeleteFieldName(oiImportExport, SelectedIndexes[i]) ;
		DeleteObjectID(oiImportExport, SelectedIndexes[i]) ;
		SendDlgItemMessage (hDlg, IDL_SELECTED, LB_DELETESTRING, SelectedIndexes[i], 0L) ;
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
	OBJECTID oiToEdit ;
	long nObjects ;
	int objectIndex ;

	nObjects = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0L) ;
	if (nObjects != 1) return TRUE ;
	SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long)(int far*) &objectIndex) ;
	oiToEdit = SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, objectIndex, 0) ;
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
	OBJECTID oiImportExport ;
	int index ;
	char far* lpszString ;

	lpszString = new char[FIELDNAMESIZE] ;
	GetDlgItemText (hDlg, IDE_FIELDNAME, lpszString, FIELDNAMESIZE) ;
	SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long)(LPSTR)&index) ;
	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	ChangeFieldName (oiImportExport, index, lpszString) ;
	delete [] (lpszString) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DeleteObjectID
// DESCRIPTION:  Deletes an Object ID from the Import/Export object's object list.
/////////////////////////////////////////////////////////////////////////////
void DeleteObjectID(OBJECTID oiImportExport, int index)
	{
	OBJECTID far *lpObjects ;
	long nObjects ;
	int i ;

	nObjects = AObjGetDataSize(oiImportExport, OBJECT_INDEX) / sizeof(OBJECTID) ;
	lpObjects = (OBJECTID far*) AObjLockData (oiImportExport, OBJECT_INDEX) ;

	if (index > nObjects) return ;

	for (i = index; i < nObjects-1; i++)
		{
		lpObjects[i] = lpObjects[i+1] ;
		}
	if (lpObjects != NULL)
		AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
	AObjResizeData(oiImportExport, OBJECT_INDEX, sizeof(OBJECTID)*(nObjects-1)) ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DeleteFieldName
// DESCRIPTION:  Deletes a field name from the Import/Export object's field name list.
/////////////////////////////////////////////////////////////////////////////
DeleteFieldName(OBJECTID oiImportExport, int index)
	{
	char far *lpTemp ;

	LPSTR lpszNames = (char far*)AObjLockData(oiImportExport, NAMES_INDEX) ;
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
	_fmemcpy (lpTemp, lpszNames, j) ; // copy the list up to the new name
	_fmemcpy (&lpTemp[j], &lpszNames[j+nOldStrLength], nNewDataSize - j) ; // copy the list after the field name
	if (lpszNames != NULL)
		AObjUnlockData(oiImportExport, NAMES_INDEX) ;
	AObjResizeData(oiImportExport, NAMES_INDEX, nNewDataSize) ; // resize data block
	if (lpszNames != NULL)
		{
		lpszNames = (char far*)AObjLockData(oiImportExport, NAMES_INDEX) ; // resize data block
		_fmemcpy(lpszNames, lpTemp, nNewDataSize) ; // save new data
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
		}
	delete [] lpTemp ; // delete the temporary copy
	return 1 ; // indicates OKAY
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  AddObjectID
// DESCRIPTION:  Adds an object ID to the Import/Export object ID list.
/////////////////////////////////////////////////////////////////////////////
void AddObjectID(OBJECTID oiImportExport, OBJECTID oiField, int index)
	{
	int Size = (int)AObjGetDataSize(oiImportExport, OBJECT_INDEX) ;
	Size += sizeof(OBJECTID) ;
	AObjResizeData (oiImportExport, OBJECT_INDEX, Size) ;
	OBJECTID far* lpObjects = (OBJECTID far*) AObjLockData (oiImportExport, OBJECT_INDEX) ;
	int nObjects = Size / sizeof(OBJECTID) ;
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
AddFieldName(OBJECTID oiImportExport, int index)
	{
	char far *lpTemp ;
	char ObjectName[OBJECTNAMESIZE] ;

	LPSTR lpszNames = (LPSTR)AObjLockData (oiImportExport, NAMES_INDEX) ;
	int j = 0 ;
	for (int i = 0; i < index; i++) // locate the beginning of the indexed field name
		while (lpszNames[j++]) ;

	int nOldDataSize = (int)AObjGetDataSize (oiImportExport, NAMES_INDEX) ; // size of the field names data
	OBJECTID far* lpObjects = (OBJECTID far*) AObjLockData (oiImportExport, OBJECT_INDEX) ;
	OBJECTID oiField = 0 ;
	if (lpObjects != NULL)
		{
		oiField = lpObjects[index] ;
		AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
		}
	int nNewStrLength = 1 + (int)AObjGetName(oiField, ObjectName, OBJECTNAMESIZE) ; // object name
	int nNewDataSize = nOldDataSize + nNewStrLength ;
	if ((lpTemp = new char[(int)nNewDataSize]) == NULL) // create a temporary copy of the field names data
		return 0 ; // indicates OUT OF MEMORY
	_fmemcpy (lpTemp, lpszNames, j) ; // copy the list up to the new name
	_fmemcpy (&lpTemp[j], ObjectName, nNewStrLength) ; // write the new name
	_fmemcpy (&lpTemp[j+nNewStrLength], &lpszNames[j], (int)nOldDataSize - j) ; // copy the list after the field name
	if (lpszNames != NULL)
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	AObjResizeData(oiImportExport, NAMES_INDEX, nNewDataSize) ; // resize data block
	lpszNames = (char far*) AObjLockData (oiImportExport, NAMES_INDEX) ;
	_fmemcpy(lpszNames, lpTemp, (int)nNewDataSize) ; // save new data
	AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	delete [] lpTemp ; // delete the temporary copy
	return 1 ; // indicates OKAY
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ChangeFieldName
// DESCRIPTION:  Changes a field name in the Import/Export object's
//		field name list.
/////////////////////////////////////////////////////////////////////////////
ChangeFieldName(OBJECTID oiImportExport, int index, LPSTR lpNewName)
	{
	char far *lpTemp ;

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
	_fmemcpy (lpTemp, lpszNames, j) ; // copy the list up to the new name
	_fmemcpy (&lpTemp[j], lpNewName, nNewStrLength) ; // write the new name
	_fmemcpy (&lpTemp[j+nNewStrLength], &lpszNames[j+nOldStrLength], (int)(nOldDataSize - (j + nOldStrLength))) ; // copy the list after the field name
	if (lpszNames != NULL)
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	AObjResizeData(oiImportExport, NAMES_INDEX, nNewDataSize) ; // resize data block
	lpszNames = (LPSTR) AObjLockData (oiImportExport, NAMES_INDEX) ;
	_fmemcpy(lpszNames, lpTemp, (int)nNewDataSize) ; // save new data
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
	OBJECTID oiImportExport ;
	ImpExpInfo far *lpImpExpInfo ;
	float fraction ;
	HDC hdc ;
	RECT rect ;
	HWND hBar ;
	HPEN hPen ;
	HBRUSH hBrush ;

	hBar = GetDlgItem(hDlg, IDD_BAR) ;
	hdc = GetDC(hBar) ;
	GetClientRect(hBar, &rect) ;
	hPen = SelectObject( hdc, GetStockObject(NULL_PEN)) ;
	hBrush = SelectObject( hdc, GetStockObject(GRAY_BRUSH)) ;

	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	lpImpExpInfo = (ImpExpInfo far *) AObjLockData(oiImportExport, INFO_INDEX) ;
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
BOOL FAR PASCAL _export StatusDlgProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
	{
	OBJECTID oiImportExport ;
	ImpExpInfo far *lpImpExpInfo ;
	HWND hBar ;

	switch (message)
		{
		case WM_INITDIALOG:
			oiImportExport = lParam ;
			lpImpExpInfo = (ImpExpInfo far*) AObjLockData (oiImportExport, 0) ;
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

		case WM_COMMAND :
			switch (wParam)
				{
				case IDCANCEL:
					oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
					lpImpExpInfo = (ImpExpInfo far*) AObjLockData (oiImportExport, 0) ;
					lpImpExpInfo->lpRuntimeInfo->Abort = TRUE ;
					AObjUnlockData (oiImportExport, 0) ;
					return TRUE ;
				}
		}
		return FALSE ;
	}
