/////////////////////////////////////////////////////////////////////////////
// File Name:  ImExConf.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:
//
// Description.:
//	Contains the dialog boxes procedures for configuring the export and
//	import characteristics of the Serius import / export object.
// Notes :
// Microsoft windows list boxes may not contain more than 64K items.
// Therefore, attempting to insert more than 64K items will have
// unpredicatable results
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commdlg.h>
#include <dos.h>
#include <mem.h>
#include <io.h>
#include <a_alm.h>
#include <a_almutl.h>
//#include <wip.h>
#include <o_text.h>
#include <o_time.h>
#include <o_number.h>
#include <o_date.h>
#include "imexobj.h"
#include "imexdlg.h"

BOOL InitDlgBox (HWND hDlg, OBJECTID oiImportExport, int) ;
BOOL TieField   (HWND hDlg) ;
BOOL TieAllField(HWND hDLg) ;
BOOL AddField   (HWND hDlg) ;
BOOL AddNumField(HWND hDLg) ;
BOOL UntieField (HWND hDlg) ;
BOOL RemoveField(HWND) ;
BOOL InsertField(HWND) ;
BOOL ReadFirstRecord(HWND) ;
BOOL CheckItem (HWND hDlg, unsigned int ) ;
BOOL ServiceApplicationList(HWND hDlg, LONG lParam) ;
BOOL ServiceFileList(HWND hDlg, LONG lParam) ;
void TieButtonsTest(HWND hDlg) ;
void UpdateObject (HWND hDlg, long type) ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ConfigImportDlgProc
// DESCRIPTION:  The dialog callback routine for the Import configuration
//  dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ConfigImportDlgProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			return InitDlgBox(hDlg, (OBJECTID) lParam, 3) ;

		case WM_COMMAND:
			if (wParam == IDD_TABS || wParam == IDD_COMMAS)
				{
				CheckRadioButton (hDlg, IDD_TABS, IDD_COMMAS, wParam) ;
				return 1;
				}
			switch (wParam)
				{
				case IDL_APPLICATION:
					return ServiceApplicationList(hDlg, lParam) ;
				case IDL_FILE:
					return ServiceFileList(hDlg, lParam) ;
				case IDD_TIE:
					return TieField (hDlg) ;
				case IDD_TIEALL:
					return TieAllField (hDlg) ;
				case IDD_UNTIE:
					return UntieField (hDlg) ;
				case IDD_ADD:
					return AddField (hDlg) ;
				case IDD_ADDNUM:
					return AddNumField (hDlg) ;
				case IDD_REMOVE:
					return RemoveField (hDlg) ;
				case IDD_INSERT:
					return InsertField (hDlg) ;
				case IDD_FIRSTRECORD:
					return ReadFirstRecord (hDlg) ;
				case IDOK:
					UpdateObject(hDlg, IMP_OBJ_INDEX);
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
// FUNCTION:  ConfigExportDlgProc
// DESCRIPTION:  The dialog callback routine for the Export configuration
//  dialog box.
/////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ConfigExportDlgProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			return InitDlgBox(hDlg, (OBJECTID) lParam, 4) ;

		case WM_COMMAND:
			if (wParam == IDD_TABS || wParam == IDD_COMMAS)
				{
				CheckRadioButton (hDlg, IDD_TABS, IDD_COMMAS, wParam) ;
				return TRUE;
				}
			switch (wParam)
				{
				case IDL_APPLICATION:
					return ServiceApplicationList(hDlg, lParam) ;
				case IDL_FILE:
					return ServiceFileList(hDlg, lParam) ;
				case IDD_TIE:
					return TieField (hDlg) ;
				case IDD_TIEALL:
					return TieAllField (hDlg) ;
				case IDD_UNTIE:
					return UntieField (hDlg) ;
				case IDD_ADD:
					return AddField (hDlg) ;
				case IDD_ADDNUM:
					return AddNumField (hDlg) ;
				case IDD_REMOVE:
					return RemoveField (hDlg) ;
				case IDD_INSERT:
					return InsertField (hDlg) ;
				case IDOK:
					UpdateObject(hDlg, EXP_OBJ_INDEX);
					EndDialog (hDlg, TRUE) ;
					return TRUE ;
				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return FALSE ;
				}
		}
	 return FALSE ;
	 }

//////////////////////////////////////////////////////////////////
// LoadFieldList
// Description:  Called by ReadFirstRecord().  Reads the field names
//  from the first line of the selected file into the field's list box.
/////////////////////////////////////////////////////////////////
void LoadFieldList(HWND hDlg, int hFile)
	{
	char buffer[256] ;
	char strField[MAXFIELDSIZE+1] ;
	char string[sizeof(strField) + LEADSIZE] ;
	BOOL EndOfLine, EndOfField ;
	int mark, bytes, field, j ;
	char currentChar, delimiter ;

	bytes = _lread(hFile, buffer, sizeof(buffer)) ;
	if (bytes <= 0) return ;
	EndOfLine = FALSE ;
	mark = 0 ;
	field = 0 ;
	if (IsDlgButtonChecked(hDlg, IDD_COMMAS))
		delimiter = ',' ;
	else
		delimiter = '\t' ;

	SendDlgItemMessage(hDlg, IDL_FILE, LB_RESETCONTENT, 0, 0L) ;
	while (!EndOfLine)
		{
		j = 0 ;
		EndOfField = FALSE ;
		while (!EndOfField)
			{
			currentChar = buffer[mark++] ;
			if (mark == sizeof(buffer))
				{
				bytes = _lread(hFile, buffer, sizeof(buffer)) ;
				mark = 0 ;
				if (bytes <= 0) return ;
				}
			EndOfLine = (currentChar == VK_RETURN) ;
			EndOfField = (currentChar == delimiter || EndOfLine) ;
			if (j < MAXFIELDSIZE && !EndOfField)
				strField[j++] = currentChar ;
			}
		strField[j] = '\0' ;
		wsprintf(string, "%d ->(%s)", field+1, strField) ;
		SendDlgItemMessage(hDlg, IDL_FILE, LB_INSERTSTRING, field, (long) string) ;
		SendDlgItemMessage(hDlg, IDL_FILE, LB_SETITEMDATA, field, 0) ;
		field++ ;
		}
	}

//////////////////////////////////////////////////////////////////
// TieButtonsTest
// Description:  Enables and Disables Tie, Tie All, and Untie buttons
//////////////////////////////////////////////////////////////////
void TieButtonsTest(HWND hDlg)
	{
	DWORD nApplication ; // number of objects selected in the Available list box.
	DWORD nFile ; // number of fields selected in the File list box.
	int i ;
	BOOL bTest ;

	// TIE BUTTON
	nFile = SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0) ;
	nApplication = SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETCURSEL, 0, 0) ;
	bTest = ((nApplication != LB_ERR) && (nFile == 1)) ;
	EnableWindow(GetDlgItem(hDlg, IDD_TIE), bTest) ;

	// TIEALL BUTTON
	nApplication = SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETCOUNT, 0, 0) ;
	nFile = SendDlgItemMessage (hDlg, IDL_FILE, LB_GETCOUNT, 0, 0) ;
	bTest = (nApplication && nFile) ;
	EnableWindow(GetDlgItem(hDlg, IDD_TIEALL), bTest) ;

	// UNTIE BUTTON
	bTest = FALSE ;
	for (i = 0; i < nFile; i++)
		if (SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSEL, i, 0))  // is this item selected ?
			if (SendDlgItemMessage (hDlg, IDL_FILE, LB_GETITEMDATA, i, 0)) // is this item tied ?
				{
				bTest = TRUE ;
				break ;
				}
	EnableWindow(GetDlgItem(hDlg, IDD_UNTIE), bTest) ;
	}

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

static
UINT		ALMCALLBACK OpenFileHook(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	UINT		nRes;

	switch (nMsg)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hDlg, 0);
			break;

		default:
			nRes = 0;
			break;
		}

	return nRes;
	}

//////////////////////////////////////////////////////////////////
// ReadFirstRecord
// Description:  Read the field names in the first line of the selected file
/////////////////////////////////////////////////////////////////
BOOL ReadFirstRecord(HWND hDlg)
	{
	OBJECTID oiImportExport ; // ID to the File object
	ImpExpInfo far* lpInfo ;
	OPENFILENAME ofn ;
	char customFilter[40] ;
	int hFile ;

	oiImportExport = GetWindowLong(hDlg, DWL_USER) ; // get the object reference the File object
	lpInfo = (ImpExpInfo far*)AObjLockData(oiImportExport, INFO_INDEX) ; // get the file information for this record

	// Set all structure members to zero
	_fmemset (&ofn, 0, sizeof(OPENFILENAME)) ;
	ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
	ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
	ofn.lpstrFilter = lpstrFilter ; // filter strings
	ofn.nFilterIndex = lpInfo->nFilterIndex ; // selects the first filter in the szFilter buffer
	ofn.lpstrFile = lpInfo->lpstrFile ;
	ofn.nMaxFile = sizeof (lpInfo->lpstrFile) ; // should be at least 256
	ofn.lpstrTitle = "Open" ;
	ofn.Flags =  OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLEHOOK;
	ofn.lpfnHook = OpenFileHook;
	wsprintf(customFilter,"[*.%s] *.%s", lpInfo->lpstrDefExt, lpInfo->lpstrDefExt) ;
	customFilter[lstrlen(customFilter)+1] = '\0' ;
	customFilter[lstrlen(lpInfo->lpstrDefExt)+4] = '\0' ;
	ofn.lpstrCustomFilter = customFilter ;
	ofn.nMaxCustFilter = sizeof(customFilter) ;
	ofn.lpstrDefExt = lpInfo->lpstrDefExt ;

	if (GetOpenFileName (&ofn) == FALSE)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		return TRUE ;
		}
	lpInfo->nFilterIndex = (int)ofn.nFilterIndex  ; // saves the first filter in the szFilter buffer

	if ((hFile = _lopen(lpInfo->lpstrFile, 0)) == -1)
		{
		MessageBox(hDlg, "File Could Not Be Opened", NULL, MB_OK) ;
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		return TRUE ;
		}

	LoadFieldList(hDlg, hFile) ;

	_lclose(hFile) ;
	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	return TRUE ;
	}

//////////////////////////////////////////////////////////////////
// UpdateObject
// Description:  Saves the contents of the file list in the object's datas
/////////////////////////////////////////////////////////////////
void UpdateObject(HWND hDlg, long datablock)
	{
	OBJECTID oiImportExport ;
	OBJECTID far *lpFields ;
	ImpExpInfo far *lpImpExpInfo ;
	ConfigInfo far *lpConfigInfo ;

	oiImportExport = GetWindowLong(hDlg, DWL_USER) ;
	int nFields = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETCOUNT, 0, 0) ;
	AObjResizeData (oiImportExport, datablock, nFields * sizeof(OBJECTID)) ;
	lpFields = (OBJECTID far*)AObjLockData (oiImportExport, datablock) ;
	for (int i = 0; i < nFields; i++)
		{
		lpFields[i] = SendDlgItemMessage (hDlg, IDL_FILE, LB_GETITEMDATA, i, 0) ;
		}
	if (lpFields)
		AObjUnlockData (oiImportExport, datablock) ;
	lpImpExpInfo = (ImpExpInfo far*)AObjLockData(oiImportExport, INFO_INDEX) ;
	if (datablock == IMP_OBJ_INDEX)
		lpConfigInfo = &lpImpExpInfo->ImportConfig ;
	else
		lpConfigInfo = &lpImpExpInfo->ExportConfig ;
	if (IsDlgButtonChecked(hDlg, IDD_COMMAS))
		lpConfigInfo->Delimiter = ',' ;
	else
		lpConfigInfo->Delimiter = '\t' ;
	lpConfigInfo->ShowStatus = IsDlgButtonChecked(hDlg, IDD_SHOW) ;
	lpConfigInfo->Heading = IsDlgButtonChecked(hDlg, IDD_IGNORE) ;
	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	}

//////////////////////////////////////////////////////////////////
// Function: InitDlgBox
// Description:
/////////////////////////////////////////////////////////////////
BOOL InitDlgBox(HWND hDlg, OBJECTID oiImportExport, int datablock)
	{
	OBJECTID far *lpObjects ;
	ImpExpInfo far *lpImpExpInfo ;
	ConfigInfo far *lpConfigInfo ;
	LPSTR lpszNames ;
	char buffer[FIELDNAMESIZE + LEADSIZE] ;
	OBJECTID far *lpFields ;
    int fieldIndex ;

	AUtlCenterDialog(hDlg, 0);
	SetWindowLong(hDlg, DWL_USER, oiImportExport) ; // save the object ID in the window extra bytes
	lpImpExpInfo = (ImpExpInfo far*) AObjLockData (oiImportExport, INFO_INDEX) ;
	if (datablock == IMP_OBJ_INDEX)
		lpConfigInfo = &lpImpExpInfo->ImportConfig ;
	else
		lpConfigInfo = &lpImpExpInfo->ExportConfig ;
	AObjUnlockData (oiImportExport, INFO_INDEX) ;

	// select the "Tab" or "Commas" radio button
	if (lpConfigInfo->Delimiter == VK_TAB )
		CheckRadioButton (hDlg, IDD_TABS, IDD_COMMAS, IDD_TABS) ;
	else
		CheckRadioButton (hDlg, IDD_TABS, IDD_COMMAS, IDD_COMMAS) ;

	// check the "Show status" and "Ignore First Line(field names)" button
	CheckDlgButton (hDlg, IDD_SHOW, lpConfigInfo->ShowStatus) ;
	CheckDlgButton (hDlg, IDD_IGNORE, lpConfigInfo->Heading) ;

	// initialize the "Application Fields" list box
	int nObjects = (int)AObjGetDataSize(oiImportExport, OBJECT_INDEX) / sizeof(OBJECTID) ;
	if (nObjects <= 0)
		return TRUE ;

	lpObjects = (OBJECTID far*) AObjLockData (oiImportExport, OBJECT_INDEX) ;
	lpszNames = (LPSTR) AObjLockData (oiImportExport, NAMES_INDEX) ;
	int j = 0 ;
	for (int i = 0; i < nObjects; i++)
		{
		int fieldIndex = (int)SendDlgItemMessage(hDlg, IDL_APPLICATION, LB_ADDSTRING, 0, (long) &lpszNames[j]) ;
		SendDlgItemMessage(hDlg, IDL_APPLICATION, LB_SETITEMDATA, fieldIndex, lpObjects[i]) ;
		if (i < nObjects)
			while (lpszNames[j++]) ;
		}

	// initialize the "Fields in File" list box
	nObjects = (int)AObjGetDataSize(oiImportExport, datablock) / sizeof(OBJECTID) ;
	if (nObjects <= 0)
		{
		if (lpObjects)
			AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
		if (lpszNames)
			AObjUnlockData (oiImportExport, NAMES_INDEX) ;
		return TRUE ;
		}
	lpFields = (OBJECTID far*) AObjLockData(oiImportExport, datablock) ;
	for (i = 0; i < nObjects; i++)
		{
		int index = (int)GetFieldIndex(oiImportExport, lpFields[i]) ;
		if (index != -1L)
			{
			for (int k = 0, j = 0; k < index; k++)
				{
				while (lpszNames[j++]) ;
				}
			wsprintf(buffer, "%d ->%s", i+1, &lpszNames[j]) ;
			fieldIndex = (int)SendDlgItemMessage(hDlg, IDL_FILE, LB_INSERTSTRING, -1, (long) buffer) ;
			SendDlgItemMessage(hDlg, IDL_FILE, LB_SETITEMDATA, fieldIndex, lpObjects[index]) ;
			}
		else
			{
			wsprintf(buffer, "%d ->", i+1 ) ;
			fieldIndex = (int)SendDlgItemMessage(hDlg, IDL_FILE, LB_INSERTSTRING, -1, (long) buffer) ;
			SendDlgItemMessage(hDlg, IDL_FILE, LB_SETITEMDATA, fieldIndex, 0) ;
			}
		}
	if (lpObjects)
		AObjUnlockData (oiImportExport, OBJECT_INDEX) ;
	if (lpszNames)
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
	if (lpFields)
		AObjUnlockData(oiImportExport, datablock) ;
	TieButtonsTest(hDlg) ;
	return TRUE ;
	}

//////////////////////////////////////////////////////////////////
// Function: GetFieldIndex
// Description:
/////////////////////////////////////////////////////////////////
long GetFieldIndex(OBJECTID oiImportExport, OBJECTID oiField)
	{
	OBJECTID far* lpObjects ;

	if (oiField == 0)
		return -1L ;

	int nObjects = (int)AObjGetDataSize(oiImportExport, OBJECT_INDEX) / sizeof(OBJECTID) ;
	lpObjects = (OBJECTID far*)AObjLockData(oiImportExport, OBJECT_INDEX) ;
	if (lpObjects)
		AObjUnlockData(oiImportExport, OBJECT_INDEX) ;
	for (int i = 0; i < nObjects; i++)
		if (lpObjects[i] == oiField)
			return i ;
	return -1L ;
	}
//////////////////////////////////////////////////////////////////
// Function: ServiceFileList
// Description:
/////////////////////////////////////////////////////////////////
BOOL ServiceFileList(HWND hDlg, LONG lParam)
	{
	int nFile ; // number of fields selected in the File list box.
	BOOL bTest ;
	int fieldIndex ;
	OBJECTID objectID ;

	switch (HIWORD (lParam))
		{
		case LBN_SELCHANGE:
			nFile = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0) ;

			bTest = (nFile == 1) ;
			EnableWindow(GetDlgItem(hDlg, IDD_INSERT), bTest) ;

			bTest = (nFile > 0) ;
			EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), bTest) ;

			TieButtonsTest(hDlg) ;
			return TRUE ;

		case LBN_DBLCLK:
			nFile = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0) ;
			if (nFile != 1)
				return TRUE ;

			SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELITEMS, 1, (long)(int far*)&fieldIndex) ;
			objectID = SendDlgItemMessage (hDlg, IDL_FILE, LB_GETITEMDATA, fieldIndex, 0L) ;
			if (objectID)
				UntieField (hDlg) ;
			else if (SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETCURSEL, 0, 0) != LB_ERR)
				TieField (hDlg) ;
		 }
	return TRUE ;
	}
//////////////////////////////////////////////////////////////////
// Function: ServiceApplicationList
// Description:
/////////////////////////////////////////////////////////////////
BOOL ServiceApplicationList(HWND hDlg, LONG lParam)
	{
	int nApplication ; // number of objects selected in the Available list box.
	int nFile ; // number of fields selected in the File list box.
	BOOL bTest ;

	switch (HIWORD (lParam))
		{
		case LBN_SELCHANGE:
			TieButtonsTest(hDlg) ;
			return TRUE ;

		case LBN_DBLCLK:
			nFile = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0) ;
			nApplication = (int)SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETCURSEL, 0, 0) ;
			bTest = ((nApplication != LB_ERR) && (nFile == 1)) ;
			if (bTest)
			   TieField (hDlg) ;
			return TRUE ;
		}
	return FALSE ;
	}

//////////////////////////////////////////////////////////////////
// Function: TieField
// Description:
/////////////////////////////////////////////////////////////////
BOOL TieField (HWND hDlg)
	{
	DWORD oiToAdd ;
	int fieldIndex ;
	char buffer[FIELDNAMESIZE] ;
	char fieldString[FIELDNAMESIZE + LEADSIZE] ;
	int nObjects ; // number of objects

	//verify that only one item is selected in the application field list
//	nObjects = SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETSELCOUNT, 0, 0L) ;
//	if (nObjects != 1) return TRUE ;
	if (SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETCURSEL, 0, 0) == LB_ERR)
		return TRUE ;

//	SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETSELITEMS, 1, (long)(int far*)&fieldIndex) ;
	fieldIndex = (int)SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETCURSEL, 0, 0) ;
	oiToAdd = SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETITEMDATA, fieldIndex, 0L) ;
	SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETTEXT, fieldIndex, (long)buffer) ;

	//verify than only one field is selected in the file field list
	nObjects = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0L) ;
	if (nObjects != 1) return TRUE ;

	SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELITEMS, 1, (long)(int far*)&fieldIndex) ;
	SendDlgItemMessage (hDlg, IDL_FILE, LB_DELETESTRING, fieldIndex, 0) ; // delete the currently selected field
	wsprintf (fieldString, "%d ->%s", fieldIndex+1, buffer) ;

	SendDlgItemMessage (hDlg, IDL_FILE, LB_INSERTSTRING, fieldIndex, (long)fieldString) ;
	SendDlgItemMessage (hDlg, IDL_FILE, LB_SETITEMDATA, fieldIndex, oiToAdd) ;
	SendDlgItemMessage (hDlg, IDL_FILE, LB_SETSEL, TRUE, fieldIndex) ;

	TieButtonsTest(hDlg) ;
	return TRUE ;
	}

//////////////////////////////////////////////////////////////////
// Function: UntieField
// Description:
/////////////////////////////////////////////////////////////////
BOOL UntieField (HWND hDlg)
	{
	char buffer[LEADSIZE] ;
	int far* SelectedIndexes ; // pointer to an integer array of list box indexes

	int nObjects = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0L) ;
	SelectedIndexes =  new int[nObjects] ;
	SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	for (int i = 0; i < nObjects; i++)
		{
		SendDlgItemMessage (hDlg, IDL_FILE, LB_DELETESTRING, SelectedIndexes[i], 0L);
		wsprintf(buffer, "%d ->", SelectedIndexes[i]+1) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_INSERTSTRING, SelectedIndexes[i], (long) buffer) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_SETITEMDATA, SelectedIndexes[i], 0) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_SETSEL, TRUE, SelectedIndexes[i]) ;
		}
	delete (SelectedIndexes) ;

	TieButtonsTest(hDlg) ;
	return TRUE ;
	}
//////////////////////////////////////////////////////////////////
// Function: TieAllField
// Description: Tie all of the fields in the file to the application
/////////////////////////////////////////////////////////////////
BOOL TieAllField (HWND hDlg)
	{
	OBJECTID oiToAdd ;
	char buffer[FIELDNAMESIZE] ;
	char fieldString[FIELDNAMESIZE + LEADSIZE] ;

	int appFields = (int)SendDlgItemMessage ( hDlg, IDL_APPLICATION, LB_GETCOUNT, 0, 0);
	int fileFields = (int)SendDlgItemMessage ( hDlg, IDL_FILE, LB_GETCOUNT, 0, 0);
	SendDlgItemMessage (hDlg, IDL_FILE, LB_RESETCONTENT, 0, 0) ;

//	lMaxFields = (lAppFields > lFileFields) ? lAppFields:lFileFields ;
//	lMinFields = (lAppFields < lFileFields) ? lAppFields:lFileFields ;
	for (int i = 0; i < fileFields; i++)
		{
		if (i < appFields)
			{
			SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETTEXT, i, (long)buffer) ;
			oiToAdd = SendDlgItemMessage (hDlg, IDL_APPLICATION, LB_GETITEMDATA, i, 0L) ;
			wsprintf (fieldString, "%d ->%s", i+1, buffer) ;
			}
		else
			{
			wsprintf(fieldString, "%d ->", i+1) ;
			oiToAdd = 0 ;
			}
		SendDlgItemMessage (hDlg, IDL_FILE, LB_INSERTSTRING, i, (long)fieldString) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_SETITEMDATA, i, oiToAdd) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_SETSEL, TRUE, i) ;
		}
	fileFields = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0) ;
	EnableWindow(GetDlgItem(hDlg, IDD_INSERT), (BOOL)(fileFields == 1)) ;
	EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), (BOOL)(fileFields)) ;
	TieButtonsTest(hDlg) ;
	return TRUE ;
	}

//////////////////////////////////////////////////////////////////
// Function: AddField
// Description: Add a number into the field in the file
//////////////////////////////////////////////////////////////////
BOOL AddField (HWND hDlg)
	{
	char buffer[LEADSIZE] ;
	long nObjects ; // number of objects

	nObjects = SendDlgItemMessage(hDlg, IDL_FILE, LB_GETCOUNT, 0, 0);
	if (nObjects >= 256)
		{
		MessageBox(hDlg, "The maximum number of fields is 256", NULL, MB_OK) ;
		return TRUE ;
		}
	wsprintf(buffer, "%ld ->", nObjects + 1) ;
	SendDlgItemMessage (hDlg, IDL_FILE, LB_SETSEL, FALSE, -1) ;
	int fieldIndex = (int)SendDlgItemMessage(hDlg, IDL_FILE, LB_INSERTSTRING, -1, (long)buffer) ;
	SendDlgItemMessage(hDlg, IDL_FILE, LB_SETITEMDATA, fieldIndex, 0) ;
	SendDlgItemMessage(hDlg, IDL_FILE, LB_SETSEL, TRUE, fieldIndex) ;
	EnableWindow(GetDlgItem(hDlg, IDD_INSERT), TRUE) ;
	EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), TRUE) ;
	TieButtonsTest(hDlg) ;
	return TRUE ;
	}

//////////////////////////////////////////////////////////////////
// Function: AddNumField
// Description: Add a number into the field in the file
//////////////////////////////////////////////////////////////////
BOOL AddNumField (HWND hDlg)
	{
	char buffer[LEADSIZE] ;
	long nObjects ; // number of objects
	int i, number, nFields ;

	nFields = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETCOUNT, 0, 0L);
	if (nFields >= 256)
		{
		MessageBox(hDlg, "The maximum number of fields is 256", NULL, MB_OK) ;
		return TRUE ;
		}
	number = DialogBox(hInstance, "addnumtofield", hDlg, (FARPROC)EditAddDlgBox) ;
	if (number <= 0)
		return TRUE ;
	if ((number+nFields) >= 256)
		{
		MessageBox(hDlg, "The maximum number of fields is 256", NULL, MB_OK) ;
		number = 256 - nFields ;
		}
	SendDlgItemMessage (hDlg, IDL_FILE, LB_SETSEL, FALSE, -1) ;
	nObjects = SendDlgItemMessage(hDlg, IDL_FILE, LB_GETCOUNT, 0, 0);
	for (i = 1; i <= number; i++)
		{
		wsprintf(buffer, "%d ->", nObjects+i) ;
		int fieldIndex = (int)SendDlgItemMessage(hDlg, IDL_FILE, LB_INSERTSTRING, -1, (long) (char far*)buffer) ;
		SendDlgItemMessage(hDlg, IDL_FILE, LB_SETITEMDATA, fieldIndex, 0) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_SETSEL, TRUE, fieldIndex) ;
		}
	nFields = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0) ;
	EnableWindow(GetDlgItem(hDlg, IDD_INSERT), (BOOL)(nFields == 1)) ;
	EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), (BOOL)(nFields)) ;
	TieButtonsTest(hDlg) ;
	return TRUE ;
	}

//////////////////////////////////////////////////////////////////
// Function: EditAddDlgBox
// Description:
//////////////////////////////////////////////////////////////////
int FAR PASCAL _export EditAddDlgBox(HWND hDlg, WORD message, WORD wParam, LONG)
	{
	switch (message)
		{
		case WM_COMMAND:
			 switch (wParam)
				{
				case WM_INITDIALOG:
					AUtlCenterDialog(hDlg, 0);
					return TRUE;

				case IDOK:
          {
					int AddNumber = (int)GetDlgItemInt(hDlg, IDE_NUM, NULL, TRUE) ;
					EndDialog (hDlg, AddNumber) ;
					return TRUE ;
          }
				case IDCANCEL:
					EndDialog (hDlg, 0) ;
					return TRUE ;
				}
		}
	return FALSE ;
	}

//////////////////////////////////////////////////////////////////
// Function: RemoveField
// Description: Remove a field from the file list box
//////////////////////////////////////////////////////////////////
BOOL RemoveField (HWND hDlg)
	{
	OBJECTID objectID ;
	int far* SelectedIndexes ; // pointer to an integer array of list box indexes
	char buffer[FIELDNAMESIZE + LEADSIZE] ;
	char fieldString[FIELDNAMESIZE + LEADSIZE] ;
	int i, j ;

	int nFields = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0L);
	SelectedIndexes = new int[nFields] ;
	SendDlgItemMessage(hDlg, IDL_FILE, LB_GETSELITEMS, nFields, (long)SelectedIndexes) ;
	for (i = nFields-1; i >= 0; i--)
		{
		SendDlgItemMessage (hDlg, IDL_FILE, LB_DELETESTRING, SelectedIndexes[i], 0L);
		}
	nFields = (int)SendDlgItemMessage(hDlg, IDL_FILE, LB_GETCOUNT, 0, 0);
	for (i = SelectedIndexes[0]; i < nFields; i++)
		{
		SendDlgItemMessage (hDlg, IDL_FILE, LB_GETTEXT, i, (long) (char far*)buffer) ;
		objectID = SendDlgItemMessage (hDlg, IDL_FILE, LB_GETITEMDATA, i, 0L) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_DELETESTRING, i, 0L);
		for (j = 0; buffer[j] != NULL && buffer[j] !='-'; j++) ;
		if (buffer[i] == NULL)
			wsprintf(fieldString, "%d ->", i+1) ;
		else
			wsprintf(fieldString, "%d ->%s", i+1, &buffer[j+2]);
		SendDlgItemMessage (hDlg, IDL_FILE, LB_INSERTSTRING, i, (long)fieldString);
		SendDlgItemMessage (hDlg, IDL_FILE, LB_SETITEMDATA, i, objectID);
		}
	SendDlgItemMessage(hDlg, IDL_FILE, LB_SETCURSEL, SelectedIndexes[0], 0) ;
	delete [] (SelectedIndexes) ;
	EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), FALSE) ;
	EnableWindow(GetDlgItem(hDlg, IDD_INSERT), FALSE) ;
	TieButtonsTest(hDlg) ;
	return TRUE ;
	}

//////////////////////////////////////////////////////////////////
// Function: InsertField
// Description: Insert a field into the file list box
//////////////////////////////////////////////////////////////////
BOOL InsertField(HWND hDlg)
	{
	OBJECTID objectID ;
	long nFields ; // number of objects or selected objected
	int fieldIndex ;
	char buffer[FIELDNAMESIZE + LEADSIZE] ;
	char fieldString[FIELDNAMESIZE + LEADSIZE] ;
	int i, j ;

	nFields = SendDlgItemMessage (hDlg, IDL_FILE, LB_GETCOUNT, 0, 0L);
	if (nFields >= 256)
		{
		MessageBox(hDlg, "The maximum number of fields is 256", NULL, MB_OK) ;
		return TRUE ;
		}

	if (SendDlgItemMessage (hDlg, IDL_FILE, LB_GETSELCOUNT, 0, 0L) != 1)
		return TRUE ;

	SendDlgItemMessage(hDlg, IDL_FILE, LB_GETSELITEMS, 1, (long)(int far*)&fieldIndex) ;
	wsprintf(fieldString, "%d ->", fieldIndex+1) ;
	i = (int)SendDlgItemMessage (hDlg, IDL_FILE, LB_INSERTSTRING, fieldIndex, (long) (char far*)fieldString) ;
	SendDlgItemMessage (hDlg, IDL_FILE, LB_SETITEMDATA, i, 0);
	SendDlgItemMessage (hDlg, IDL_FILE, LB_SETSEL, TRUE, fieldIndex) ;
	nFields = SendDlgItemMessage(hDlg, IDL_FILE, LB_GETCOUNT, 0, 0);
	for (i = fieldIndex+1; i < nFields; i++)
		{
		SendDlgItemMessage (hDlg, IDL_FILE, LB_GETTEXT, i, (long) (char far*)buffer) ;
		objectID = SendDlgItemMessage (hDlg, IDL_FILE, LB_GETITEMDATA, i, 0L) ;
		SendDlgItemMessage (hDlg, IDL_FILE, LB_DELETESTRING, i, 0L);
		for (j = 0; buffer[j] != NULL && buffer[j] !='-'; j++) ;
		if (buffer[j] == NULL)
			wsprintf(fieldString, "%d ->", i+1) ;
		else
			wsprintf(fieldString, "%d ->%s", i+1, &buffer[j+2]);
		SendDlgItemMessage (hDlg, IDL_FILE, LB_INSERTSTRING, i, (long) (char far*)fieldString);
		SendDlgItemMessage (hDlg, IDL_FILE, LB_SETITEMDATA, i, objectID);
		}
	TieButtonsTest(hDlg) ;
	return TRUE ;
	}

