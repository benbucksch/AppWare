/////////////////////////////////////////////////////////////////////////////
// File Name:  GetFiles.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  4/22/92
//
// Description.:
//	File operation functions
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mem.h>
#include <dir.h>
#include <dos.h>
#include <ctype.h>
#include <a_alm.h>
//#include <serdlg.h>
//#include <seriusID.h>
//#include <wip.h>
#include <o_text.h>
#include "flop.h"

int CALLBACK GetFilesDialogProc (HWND, UINT, UINT, LONG) ;
BOOL InitDialog(HWND hDlg) ;
BOOL ServiceDirectoryList(HWND hDlg, WPARAM wParam, LPARAM lParam) ;
BOOL ServiceChosenList(HWND hDlg, WPARAM wParam, LPARAM lParam) ;
BOOL AddFile(HWND hDlg) ;
BOOL RemoveFile(HWND hDlg) ;
BOOL WriteToOutputs(HWND hDlg) ;
void fillDirectoryList(HWND hDlg) ;
void GoToParentDirectory(HWND hDlg) ;
void GoToSubdirectory(HWND hDlg) ;
void AddFileToList(HWND hDlg) ;
void initChosenList(HWND hDlg) ;
void initDriveList(HWND hDlg) ;
BOOL ServiceDriveList(HWND hDlg, WPARAM wParam, LPARAM lParam) ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetFilesFunc (fOgf)
// DESCRIPTION: Call this funntion to display a dialog box listing files
//  which the user may select.  The results are returned in lists.  Default
//	files can be passed to this function.
// INPUT 1 (opt Text Object): Return delimited list of file types
// INPUT 2 (opt Text Object): Return delimited list of full pathnames for initial list
// OUTPUT 1 (opt Text Object): Return delimited list of selected full pathnames
// OUTPUT 2 (opt Text Object): Return delimited list of pathnames
// FLOW 1: Followed after the files have been selected.
// FLOW 2: Followed if the user cancels the selection dialog.
/////////////////////////////////////////////////////////////////////////////
void CALLBACK GetFilesFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	if (DialogBox(hInstance, "GetFiles", GetActiveWindow(), (DLGPROC)GetFilesDialogProc) == TRUE)
		AFuncReportResult (theSystem, 1, "") ;
	else
		AFuncReportResult (theSystem, 2, "") ;
	return ;
	}

int CALLBACK GetFilesDialogProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			return InitDialog(hDlg) ;

		case WM_COMMAND: 
			switch (GET_WM_COMMAND_ID(wParam, lParam))
				{
				case IDCB_DRIVES:
					return ServiceDriveList(hDlg, wParam, lParam) ;

				case IDL_DIRECTORY:
					return (ServiceDirectoryList(hDlg, wParam, lParam)) ;

				case IDL_CHOSEN:
					return (ServiceChosenList(hDlg, wParam, lParam)) ;

				case IDD_ADD:
					return (AddFile(hDlg)) ;

				case IDD_REMOVE:
					return (RemoveFile(hDlg)) ;

				case IDOK:
					WriteToOutputs(hDlg) ;
					EndDialog (hDlg, TRUE) ;
					return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;
				}
		}
	 return FALSE ;
	 }

BOOL InitDialog(HWND hDlg)
	{
	char buffer[MAX_PATH] ;
	int firstTab = 512 ;

	ListBox_SetTabStops(GetDlgItem(hDlg, IDL_CHOSEN), 1, &firstTab);
	getcwd(buffer, sizeof(buffer) - MAXFILE - MAXEXT) ;
	initDriveList(hDlg) ;
	AOBJECTID oiFilter = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
	if (oiFilter == 0)
		lstrcat(buffer, "\\*.*") ;
	else
		{
		char filter[MAXFILE + MAXEXT] ;
		OTextGetToBuffer(oiFilter, filter, sizeof(filter)) ;
		char fileName[MAXFILE] ;
		char extension[MAXEXT] ;
		fnsplit(filter, NULL, NULL, fileName, extension) ;
		lstrcat(buffer, "\\") ;
		lstrcat(buffer, fileName) ;
		lstrcat(buffer, extension) ;
		if (filter[0] == 0)
			lstrcat(buffer, "*.*") ;
		}
	SetDlgItemText(hDlg, IDS_PATH, buffer) ;
	fillDirectoryList(hDlg) ;
	initChosenList(hDlg) ;
	return TRUE ;
	}

void initDriveList(HWND hDlg)
	{
	int disk, save, index ;
	char buffer[MAX_PATH] ;

	save = getdisk() ;
	for (disk = 0; disk < 26; disk++)
		{
		setdisk(disk) ;
		if (disk == getdisk())
			{
			lstrcpy(buffer, "A:") ;
			buffer[0] = (char)(buffer + disk) ;
			index = (int)ComboBox_AddString(GetDlgItem(hDlg, IDCB_DRIVES), buffer);
			ComboBox_SetItemData(GetDlgItem(hDlg, IDCB_DRIVES), index, disk);
			if (disk == save)
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDCB_DRIVES), index);
			}
		}
	setdisk(save) ;
	}

BOOL ServiceDriveList(HWND hDlg, WPARAM wParam, LPARAM /*lParam*/)
	{
	int index, disk, olddisk, length, i ;
	char buffer[MAX_PATH+1], filter[14] ;

	if (GET_WM_COMMAND_CMD(wParam, lParam) != CBN_SELCHANGE)
		return TRUE ;

	index = (int)ComboBox_GetCurSel(GetDlgItem(hDlg, IDCB_DRIVES));
	disk = 	(int)ComboBox_GetItemData(GetDlgItem(hDlg, IDCB_DRIVES), index);
	olddisk = getdisk() ;
	if (olddisk == disk)
		return TRUE ;
	GetDlgItemText(hDlg, IDS_PATH, buffer, MAX_PATH-1) ;
	length = lstrlen(buffer) ;
	for (i = length; i > 1; i--) // find the filter string
		{
		if (buffer[i] == '\\')
			{
			lstrcpy(filter, &buffer[i+1]) ;
			break ;
			}
		}
	if (i < 2)
		lstrcpy(filter, "*.*") ;

	setdisk(disk) ;
	if (getcwd(buffer, MAX_PATH) == NULL)
		{
		GetDlgItemText(hDlg, IDS_PATH, buffer, 2) ;
		setdisk(olddisk) ;
		index = (int)ComboBox_SelectString(GetDlgItem(hDlg, IDCB_DRIVES), 0, buffer);
		ComboBox_SetCurSel(GetDlgItem(hDlg, IDCB_DRIVES), index);
		return TRUE ;
		}
	length = lstrlen(buffer) ;
	if (buffer[length-1] != '\\')
		lstrcat(buffer, "\\") ;
	lstrcat(buffer, filter) ;
	SetDlgItemText(hDlg, IDS_PATH, buffer) ;
	fillDirectoryList(hDlg) ;
	return TRUE ;
	}

void initChosenList(HWND hDlg)
	{
	AOBJECTID oiPaths ;
	AMEMBLOCKID memPaths ;
	char  *lpPaths ;
	char  *fullpath ;
	char listString[MAX_PATH + 14], ext[MAXEXT] ;
	int i, length ;

	oiPaths = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	if (oiPaths == 0)
		return ;
	memPaths = AMemAllocate(0) ;
	OTextGetToMemblock(oiPaths, memPaths) ;
//	AObjConvertToText(oiPaths, memPaths) ;
	lpPaths = (char *)AMemLock(memPaths) ;
	length = lstrlen(lpPaths) ;
	for (i = length ; i > 0; i--)
		{
		if ((lpPaths[i] == '\n') || (lpPaths[i] == '\r'))
			{
			lpPaths[i] = lpPaths[i-1] = 0 ;
			fullpath = &lpPaths[i+1] ;
			i-- ;
			if (CreateFile(fullpath, 0, 0, NULL, OPEN_EXISTING, 
										 FILE_ATTRIBUTE_NORMAL, NULL) != INVALID_HANDLE_VALUE)
				{
				fnsplit(fullpath, NULL, NULL, listString, ext) ;
				lstrcat(listString, ext) ;
				lstrcat(listString, "\t") ;
				lstrcat(listString, fullpath) ;
				ListBox_InsertString(GetDlgItem(hDlg, IDL_CHOSEN), 0, listString);
				}
			}
		}
	if (CreateFile(lpPaths, 0, 0, NULL, OPEN_EXISTING, 
								 FILE_ATTRIBUTE_NORMAL, NULL) != INVALID_HANDLE_VALUE)
		{
		fnsplit(lpPaths, NULL, NULL, listString, ext) ;
		lstrcat(listString, ext) ;
		lstrcat(listString, "\t") ;
		lstrcat(listString, lpPaths) ;
		ListBox_InsertString(GetDlgItem(hDlg, IDL_CHOSEN), 0, listString);
		}
	AMemUnlock(memPaths) ;
	AMemFree(memPaths) ;
	}

void fillDirectoryList(HWND hDlg)
	{
	char pathname[MAX_PATH], filter[13] ;
	char fname[14] ;
	int done, nFiles, i, index, startOfFilter ;
	fileName *list ;
	struct ffblk ffBlk ;

	ListBox_ResetContent(GetDlgItem(hDlg, IDL_DIRECTORY));
	GetDlgItemText(hDlg, IDS_PATH, pathname, MAX_PATH) ;
	startOfFilter = lstrlen(pathname) ;
	while (pathname[startOfFilter] != '\\')
		startOfFilter-- ;
	startOfFilter++ ;
	lstrcpy(filter, &pathname[startOfFilter]) ;
	lstrcpy(&pathname[startOfFilter], "*.*") ;
	done = findfirst(pathname, &ffBlk, FA_DIREC) ;
	while (!done)
		{
		if ((ffBlk.ff_attrib & FA_DIREC) && (ffBlk.ff_name[0] != '.'))
			{
			lstrcpy(fname, ffBlk.ff_name) ;
			lstrcat(fname, "\\") ;
			index = (int)ListBox_AddString(GetDlgItem(hDlg, IDL_DIRECTORY), fname);
			ListBox_SetItemData(GetDlgItem(hDlg, IDL_DIRECTORY), index, 1L);
			}
		done = findnext(&ffBlk) ;
		}
	if (startOfFilter > 3)
		{
		index = (int)ListBox_InsertString(GetDlgItem(hDlg, IDL_DIRECTORY), 0, "..\\");
		ListBox_SetItemData(GetDlgItem(hDlg, IDL_DIRECTORY), index, 0L);
		}
	lstrcpy(&pathname[startOfFilter], filter) ;
	nFiles = 0 ;
	done = findfirst(pathname, &ffBlk, FA_DIREC) ;
	while (!done)
		{
		if(!(ffBlk.ff_attrib & FA_DIREC))
			nFiles++ ;
		done = findnext(&ffBlk) ;
		}
	list = (fileName *)new char[nFiles*sizeof(fileName)] ;
	if (list == NULL) return ;
	i = 0 ;
	done = findfirst(pathname, &ffBlk, 0) ;
	while (!done && i < nFiles)
		{
		lstrcpy(list[i++].name, ffBlk.ff_name) ;
		done = findnext(&ffBlk) ;
		}
	qsort((void*)list, nFiles, sizeof(fileName), sort_function) ;
	for (i = 0; i < nFiles; i++)
		{
		if(list[i].name[0] == '.') // skip directories . and ..
			continue ;
		index = (int)ListBox_InsertString(GetDlgItem(hDlg, IDL_DIRECTORY), -1, list[i].name);
		ListBox_SetItemData(GetDlgItem(hDlg, IDL_DIRECTORY), index, 2L);
		}
	delete [] list ;
	}

BOOL ServiceDirectoryList(HWND hDlg, WPARAM wParam, LPARAM /*lParam*/)
	{
	int index, type ;

	index = (int)ListBox_GetCurSel(GetDlgItem(hDlg, IDL_DIRECTORY));
	switch (GET_WM_COMMAND_CMD(wParam, lParam))
		{
		case LBN_SELCHANGE:
			EnableWindow(GetDlgItem(hDlg, IDD_ADD), (BOOL)(index != LB_ERR)) ;
			if (index != LB_ERR)
				{
				type = (int)ListBox_GetItemData(GetDlgItem(hDlg, IDL_DIRECTORY), index);
				if (type == 2)
					SetDlgItemText(hDlg, IDD_ADD, "&Add") ;
				else
					SetDlgItemText(hDlg, IDD_ADD, "&Open");
				}
			else // if no items are selected, disable the "Add" button
				SetDlgItemText(hDlg, IDD_ADD, "&Add") ;
			return TRUE;

		case LBN_DBLCLK:
			if (index == LB_ERR)
				return FALSE ;
			type = (int)ListBox_GetItemData(GetDlgItem(hDlg, IDL_DIRECTORY), index);
			switch (type)
				{
				case 0: // step back one directory level
					GoToParentDirectory(hDlg) ;
					break ;
				case 1:
					GoToSubdirectory(hDlg) ;
					break ;
				case 2:
					AddFileToList(hDlg) ;
					break ;
				}
			return TRUE;
		}
	return FALSE ;
	}

void GoToParentDirectory(HWND hDlg)
	{
	char pathname[MAX_PATH], filter[13] ;
	int length, i ;

	GetDlgItemText(hDlg, IDS_PATH, pathname, MAX_PATH-1) ;
	length = lstrlen(pathname) ;
	for (i = length; i > 2; i--)
		{
		if (pathname[i] == '\\')
			{
			lstrcpy(filter, &pathname[i+1]) ;
			break ;
			}
		}
	i-- ;
	for ( ; i > 1; i--)
		{
		if (pathname[i] == '\\')
			{
			lstrcpy(&pathname[i+1], filter) ;
			SetDlgItemText(hDlg, IDS_PATH, pathname) ;
			fillDirectoryList(hDlg) ;
			break ;
			}
		}
	return ;
	}

void GoToSubdirectory(HWND hDlg)
	{
	char subdirectory[13], pathname[MAX_PATH], filter[13] ;
	int i, length, index ;

	index = (int)ListBox_GetCurSel(GetDlgItem(hDlg, IDL_DIRECTORY));
	ListBox_GetText(GetDlgItem(hDlg, IDL_DIRECTORY), index, subdirectory);
	GetDlgItemText(hDlg, IDS_PATH, pathname, MAX_PATH-1) ;
	length = lstrlen(pathname) ;
	for (i = length; i > 1; i--)
		{
		if (pathname[i] == '\\')
			break ;
		}
	if (i < 2)
		return ;
	lstrcpy(filter, &pathname[i+1]) ;
	lstrcpy(&pathname[i+1], subdirectory) ;
	lstrcat(pathname, filter) ;
	SetDlgItemText(hDlg, IDS_PATH, pathname) ;
	fillDirectoryList(hDlg) ;
	return ;
	}

void AddFileToList(HWND hDlg)
	{
	char filename[13], pathname[MAX_PATH], listboxString[MAX_PATH+13] ;
	int i, length, index ;

	index = (int)ListBox_GetCurSel(GetDlgItem(hDlg, IDL_DIRECTORY));
	ListBox_GetText(GetDlgItem(hDlg, IDL_DIRECTORY), index, filename);
	GetDlgItemText(hDlg, IDS_PATH, pathname, MAX_PATH) ;
	length = lstrlen(pathname) ;
	for (i = length; i > 1; i--)
		{
		if (pathname[i] == '\\')
			break ;
		}
	if (i < 2)
		return ;
	lstrcpy(&pathname[i+1], filename) ;
	lstrcpy(listboxString, filename) ;
	lstrcat(listboxString, "\t") ;
	lstrcat(listboxString, pathname) ;
	ListBox_InsertString(GetDlgItem(hDlg, IDL_CHOSEN), -1, listboxString);
	return ;
	}

BOOL ServiceChosenList(HWND hDlg, WPARAM wParam, LPARAM /*lParam*/)
	{
	int index ;

	index = (int)ListBox_GetCurSel(GetDlgItem(hDlg, IDL_CHOSEN));
	if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_SELCHANGE)
		EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), (BOOL)(index != LB_ERR)) ;
	return TRUE ;
	}

BOOL AddFile(HWND hDlg)
	{
	int index, type ;

	index = (int)ListBox_GetCurSel(GetDlgItem(hDlg, IDL_DIRECTORY));
	if (index == LB_ERR)
		return TRUE ;
	type = (int)ListBox_GetItemData(GetDlgItem(hDlg, IDL_DIRECTORY), index);
	switch (type)
		{
		case 0: // step back one directory level
			GoToParentDirectory(hDlg) ;
			break ;
		case 1:
			GoToSubdirectory(hDlg) ;
			break ;
		case 2:
			AddFileToList(hDlg) ;
			break ;
		}
	return TRUE;
	}

BOOL RemoveFile(HWND hDlg)
	{
	int index ;

	index = (int)ListBox_GetCurSel(GetDlgItem(hDlg, IDL_CHOSEN));
	ListBox_DeleteString(GetDlgItem(hDlg, IDL_CHOSEN), index);
	ListBox_SetCurSel(GetDlgItem(hDlg, IDL_CHOSEN), index);
	if (LB_ERR == ListBox_GetCurSel(GetDlgItem(hDlg, IDL_CHOSEN)))
		{
		ListBox_SetCurSel(GetDlgItem(hDlg, IDL_CHOSEN), index-1);
		if (LB_ERR == ListBox_GetCurSel(GetDlgItem(hDlg, IDL_CHOSEN)))
			EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), FALSE) ;
		}
	return TRUE ;
	}

BOOL WriteToOutputs(HWND hDlg)
	{
	int i, j, nFiles, length, maxFiles ;
	char listString[MAX_PATH + 14] ;

	nFiles = (int)ListBox_GetCount(GetDlgItem(hDlg, IDL_CHOSEN));
	maxFiles = (int)(65536L / (MAX_PATH + 2)) ;
	if (nFiles > maxFiles)
		nFiles = maxFiles ;
	LPSTR lpPaths = new char[nFiles * (MAX_PATH + 2)+1] ;
	if (lpPaths == NULL)
		return TRUE ;
//	AObjResizeData(oiPaths, 1, ) ;
//	lpPaths = (char *)AObjLockData(oiPaths, 1) ;
	LPSTR lpFiles = new char[nFiles*14 + 1] ;
	if (lpFiles == NULL)
		{
		delete [] lpPaths ;
		return TRUE ;
		}
//	AObjResizeData(oiFiles, 1, ) ;
//	lpFiles = (char *)AObjLockData(oiFiles, 1) ;
//	if ((lpPaths == NULL) || (lpFiles == NULL))
//		return TRUE ;
	lpPaths[0] = lpFiles[0] = 0 ;
	for (i = 0; i < nFiles; i++)
		{
		if (i != 0)
			{
			lstrcat(lpFiles, "\r\n") ;
			lstrcat(lpPaths, "\r\n") ;
			}
		ListBox_GetText(GetDlgItem(hDlg, IDL_CHOSEN), i, listString);
		length = lstrlen(listString) ;
		for (j = 0; j < length; j++)
			{
			if (listString[j] == '\t')
				{
				listString[j] = 0 ;
				lstrcat(lpFiles, listString) ;
				lstrcat(lpPaths, &listString[j+1]) ;
				break ;
				}
			}
		}
	AOBJECTID oiPaths = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiPaths, lpPaths) ;
	AFuncSetParameter(1, oiPaths) ;

	AOBJECTID oiFiles = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiFiles, lpFiles) ;
	AFuncSetParameter(2, oiFiles) ;
	return TRUE ;
	}

