/////////////////////////////////////////////////////////////////////////////
// File Name:  FileObjD.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  4/22/92
//
// Description.: Object Definition Routine for the Serius File object:
////////////////////////////////////////////////////////////////////////////

#include <windows.h>
//#include <mem.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>
#include <o_pict.h>
#include "fileobj.h"

#define OTYPE_SOUND MAKEID('S','o','u','n')
#define HELPID_OBJD_File 0

BOOL InitDialog(HWND, long) ;
BOOL AddItem(HWND) ;
BOOL RemoveItem(HWND) ;
BOOL EditItem(HWND) ;
BOOL UpdateFieldName(HWND) ;
void UpdateAvailableList (HWND hDlg, WORD buttonID) ;
BOOL ServiceSelectedList(HWND hDlg, LONG lParam) ;
BOOL ServiceAvailableList(HWND hDlg, LONG lParam) ;
BOOL UpdateObject(HWND) ;
BOOL UpdateDefaultExtention(HWND hDlg) ;

LONG EvaluateObject( OBJECTID oiFile, pAObjMessage )
	{
	OBJECTID far *lpObjects ;

	lpObjects = (OBJECTID far*)AObjLockData( oiFile, OBJECT_INDEX ) ;
	int nObjects = (int)AObjGetDataSize (oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ;
	int nObjectsRemaining = nObjects ;
	for (int i = nObjects-1; i >= 0; i--)
		{
		if (!AObjCheckType(lpObjects[i], 0))
			{
			nObjectsRemaining-- ;
			for (int j = i; j < nObjectsRemaining; j++)
				{
				lpObjects[j] = lpObjects[j+1] ; // shift objects
				}
			}
		}
	if (lpObjects)
		AObjUnlockData( oiFile, OBJECT_INDEX ) ;
	AObjResizeData(oiFile, OBJECT_INDEX, nObjectsRemaining*sizeof(OBJECTID)) ;
	return A_OK ;
	}

LONG ValidateObjectIDs( OBJECTID oiFile, pAObjMessage theSystem )
	{
	OBJECTID far *lpObjects ;
	BOOL changed ;

	lpObjects = (OBJECTID far*)AObjLockData( oiFile, OBJECT_INDEX ) ;
	int nObjects = (int)AObjGetDataSize (oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ;
	int nObjectsRemaining = nObjects ;
	for (int i = nObjects-1; i >= 0; i--)
		{
		changed = AObjValidateID(&lpObjects[i], theSystem) ;
		if (changed && (lpObjects[i] == 0))
			{
			nObjectsRemaining-- ;
			for (int j = i; j < nObjectsRemaining; j++)
				{
				lpObjects[j] = lpObjects[j+1] ; // shift objects
				}
			}
		}
	if (lpObjects)
		AObjUnlockData( oiFile, OBJECT_INDEX ) ;
	AObjResizeData(oiFile, OBJECT_INDEX, nObjectsRemaining*sizeof(OBJECTID)) ;
	return A_OK ;
	}

BOOL FAR PASCAL _export EditObjectDialogProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
	{
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

				case IDE_DEFEXT:
					if (HIWORD(lParam) == EN_KILLFOCUS)
						return (UpdateDefaultExtention(hDlg)) ;
					return FALSE ;

				case IDHELP:
					WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_File) ;
					return TRUE ;

				case IDOK:
					return (UpdateObject(hDlg)) ;

				case IDCANCEL:
					EndDialog (hDlg, (int)A_OK) ;
					return TRUE ;
				}
		}
	 return FALSE ;
	 }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  UpdateFieldName
// DESCRIPTION:  Updates the field name list in the Import/Export object's
//	data to reflect an editted object field name.  This function is called
//	when the programmer exits the "Field Name" edit control.
/////////////////////////////////////////////////////////////////////////////
BOOL UpdateDefaultExtention(HWND hDlg)
	{
	OBJECTID oiFile ;
	char buffer[4] ;
	FileInfo far *lpFileInfo ;

	GetDlgItemText (hDlg, IDE_DEFEXT, buffer, sizeof(buffer)) ;
	oiFile = GetWindowLong(hDlg, DWL_USER) ;
	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ;
	lstrcpy(lpFileInfo->lpstrDefExt, buffer) ;
	AObjUnlockData(oiFile, INFO_INDEX);
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  UpdateObject
// DESCRIPTION:  Updates the File object's selected object list.
/////////////////////////////////////////////////////////////////////////////
BOOL  UpdateObject(HWND hDlg)
	{
	DWORD nObjects ;
	OBJECTID far * lpObjects ;
	int i ;

	OBJECTID oiFile = GetWindowLong (hDlg, DWL_USER) ;
	FileInfo far* lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ;
	GetDlgItemText(hDlg, IDE_DEFEXT, lpFileInfo->lpstrDefExt, 4) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	nObjects = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0L) ;
	AObjResizeData (oiFile, OBJECT_INDEX, nObjects*sizeof(OBJECTID)) ;
	lpObjects = (OBJECTID far*)AObjLockData (oiFile, OBJECT_INDEX) ;
	if (lpObjects)
		for (i = 0; i < nObjects; i++)
			lpObjects[i] = SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETITEMDATA, i, 0);

	AObjUnlockData (oiFile, OBJECT_INDEX) ;
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
				}
			else // otherwise, disable the "Edit" button
				{
				EnableWindow(GetDlgItem(hDlg, IDD_EDIT ), FALSE) ;
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
// FUNCTION:  UpdateAvailableList
// DESCRIPTION:  Updates the object's listed in the "Availble Objects"
//	list box.
/////////////////////////////////////////////////////////////////////////////
void UpdateAvailableList (HWND hDlg, WORD buttonID)
	{
	char buffer[OBJECTNAMESIZE] ;
	LONG typeID ;
	OBJECTID oiToAdd ;

	// clear all strings from the list box
	SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_RESETCONTENT, 0, 0) ; // clear the list of its previous contents
	EnableWindow(GetDlgItem(hDlg, IDD_ADD), FALSE) ; // disable the "ADD" button

	// if "other" object types are selected
	if (buttonID == IDD_OTHER)
		{
		long typeIndex = AOBJ_GETFIRSTKEY ;
		while ((typeID = ATypeGetNext (&typeIndex)) != 0)
			{
			if (typeID != OTYPE_TEXT &&
				typeID != OTYPE_NUMBER &&
				typeID != OTYPE_PICTURE &&
				typeID != OTYPE_SOUND )
				{
				long key = AOBJ_GETFIRSTKEY ;
				while((oiToAdd = AObjGetNextObject(typeID, &key)) != 0)
					{
					AObjGetName(oiToAdd, buffer, OBJECTNAMESIZE) ;
					int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_ADDSTRING, 0, (long)(LPSTR)buffer) ;
					SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, objectIndex, oiToAdd) ;
					}
				}
			}
		return ;
		}

	// if "text", "number", "picture", or "sound" object types are selected
	switch (buttonID)
		{
		case IDD_TEXT:
			typeID = OTYPE_TEXT ;
			break ;
		case IDD_NUMBER:
			typeID = OTYPE_NUMBER ;
			break ;
		case IDD_PICTURE:
			typeID = OTYPE_PICTURE ;
			break ;
		case IDD_SOUND:
			typeID = OTYPE_SOUND ;
			break ;
		}
	long key = AOBJ_GETFIRSTKEY ;
	while((oiToAdd = AObjGetNextObject(typeID, &key)) != NULL)
		{
		AObjGetName(oiToAdd, buffer, OBJECTNAMESIZE) ;
		int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_ADDSTRING, 0, (long)(LPSTR)buffer) ;
		SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, objectIndex, oiToAdd) ;
		}
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the import / export edit dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL InitDialog ( HWND hDlg, OBJECTID oiFile)
	{
	char buffer[OBJECTNAMESIZE] ;
	OBJECTID far* lpObjects ;
	FileInfo far *lpFileInfo ;

	SetWindowLong(hDlg, DWL_USER, oiFile) ; // save the object ID in the window extra bytes
	HWND hText = GetDlgItem(hDlg, IDD_OBJECT) ; // added 9/8/93
	AUtlSetTitleFont(OTYPE_FILE, NULL, hText); // added 9/8/93
	AUtlCenterDialog(hDlg, 0);

	AObjGetName(oiFile, buffer, OBJECTNAMESIZE) ;
	SetWindowText(hDlg, buffer) ; // sets the dialog box caption text

	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ;
	SetDlgItemText(hDlg, IDE_DEFEXT, lpFileInfo->lpstrDefExt) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	CheckRadioButton (hDlg, IDD_TEXT, IDD_OTHER, IDD_TEXT) ; // check the "Text" radio button
	UpdateAvailableList (hDlg, IDD_TEXT) ; // initialize the available objects list

	 // initialize the Import Export selected objects list
	lpObjects = (OBJECTID far*)AObjLockData(oiFile, OBJECT_INDEX) ; // identify the objects participating in the Import/Export object
	if (lpObjects == NULL) return TRUE ; // no objects in the list
	int nObjects = (int)AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ;
	for(int i = 0; i < nObjects; i++)
		{
		if (AObjCheckType(lpObjects[i], 0) == 0)
			continue ;
		AObjGetName(lpObjects[i], buffer, OBJECTNAMESIZE) ;  // get the name of each participating object
		int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_SELECTED, LB_ADDSTRING, 0, (long)buffer) ;
		SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, objectIndex, lpObjects[i]) ;
		}
	AObjUnlockData(oiFile, OBJECT_INDEX) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  AddItem
// DESCRIPTION:  Adds the objects selected from the "Available Objects" list
// 	to the "Selected Objects" list.
/////////////////////////////////////////////////////////////////////////////
BOOL AddItem (HWND hDlg)
	{
	char buffer[OBJECTNAMESIZE] ;
	int far* SelectedIndexes ;

	int nObjects = (int)SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELCOUNT, 0, 0L) ; // number of objects selected in the "Available Objects" list box

	if ((SelectedIndexes = new int[nObjects]) == NULL) // initialize buffer to store the indexes of selected
		{
		MessageBox(hDlg, "Insufficient Memory", NULL, MB_OK) ;
		return FALSE ;
		}
	SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	for (int i = 0; i < nObjects; i++)
		{
		SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETTEXT, SelectedIndexes[i], (long)(LPSTR)buffer) ;
		OBJECTID oiObjectToAdd = SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_GETITEMDATA, SelectedIndexes[i], 0) ;
		int nCheckForDuplicates = (int)SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0) ;
		for (int j = 0; j < nCheckForDuplicates; j++)
			if (oiObjectToAdd == SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, j, 0))
				continue ;

		int objectIndex = (int)SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING, -1, (long)(LPSTR)buffer) ;
		SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, objectIndex, oiObjectToAdd) ;
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
	int far* SelectedIndexes ;

	int nObjects = (int)SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0L) ;
	SelectedIndexes = new int[nObjects] ;
	SendDlgItemMessage (hDlg, IDL_SELECTED, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	for (int i = nObjects ; i > 0; i--)
		{
		SendDlgItemMessage (hDlg, IDL_SELECTED, LB_DELETESTRING, SelectedIndexes[i-1], 0L) ;
		}
	SendDlgItemMessage (hDlg, IDL_SELECTED, LB_SETCARETINDEX, 0,0) ;
	EnableWindow (GetDlgItem(hDlg, IDD_REMOVE), FALSE) ;
	EnableWindow (GetDlgItem(hDlg, IDD_EDIT), FALSE) ;
	SetFocus (GetDlgItem (hDlg, IDL_SELECTED)) ;
	delete(SelectedIndexes) ;
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

