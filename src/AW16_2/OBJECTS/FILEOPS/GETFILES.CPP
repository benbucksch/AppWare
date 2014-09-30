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

int FAR PASCAL GetFilesDialogProc (HWND, UINT, UINT, LONG) ;
BOOL InitDialog(HWND hDlg) ;
BOOL ServiceDirectoryList(HWND hDlg, LPARAM lParam) ;
BOOL ServiceChosenList(HWND hDlg, LPARAM lParam) ;
BOOL AddFile(HWND hDlg) ;
BOOL RemoveFile(HWND hDlg) ;
BOOL WriteToOutputs(HWND hDlg) ;
void fillDirectoryList(HWND hDlg) ;
void GoToParentDirectory(HWND hDlg) ;
void GoToSubdirectory(HWND hDlg) ;
void AddFileToList(HWND hDlg) ;
void initChosenList(HWND hDlg) ;
void initDriveList(HWND hDlg) ;
BOOL ServiceDriveList(HWND hDlg, LPARAM lParam) ;

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
void FAR PASCAL _export GetFilesFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	if (DialogBox(hInstance, "GetFiles", GetActiveWindow(), (DLGPROC)GetFilesDialogProc) == TRUE)
		AFuncReportResult (theSystem, 1, "") ;
	else
		AFuncReportResult (theSystem, 2, "") ;
	return ;
	}

int FAR PASCAL _export GetFilesDialogProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			return InitDialog(hDlg) ;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCB_DRIVES:
					return ServiceDriveList(hDlg, lParam) ;

				case IDL_DIRECTORY:
					return (ServiceDirectoryList(hDlg, lParam)) ;

				case IDL_CHOSEN:
					return (ServiceChosenList(hDlg, lParam)) ;

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
	char buffer[MAXPATH] ;
	int firstTab = 512 ;

	SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_SETTABSTOPS, 1, (long)&firstTab) ;
	getcwd(buffer, sizeof(buffer) - MAXFILE - MAXEXT) ;
	initDriveList(hDlg) ;
	OBJECTID oiFilter = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
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
	char buffer[MAXPATH] ;

	save = getdisk() ;
	for (disk = 0; disk < 26; disk++)
		{
		setdisk(disk) ;
		if (disk == getdisk())
			{
			lstrcpy(buffer, "A:") ;
			buffer[0] += disk ;
			index = (int)SendDlgItemMessage(hDlg, IDCB_DRIVES, CB_ADDSTRING, 0, (long)(char far*)buffer) ;
			SendDlgItemMessage(hDlg, IDCB_DRIVES, CB_SETITEMDATA, index, disk) ;
			if (disk == save)
				SendDlgItemMessage(hDlg, IDCB_DRIVES, CB_SETCURSEL, index, 0) ;
			}
		}
	setdisk(save) ;
	}

BOOL ServiceDriveList(HWND hDlg, LPARAM lParam)
	{
	int index, disk, olddisk, length, i ;
	char buffer[MAXPATH+1], filter[14] ;

	if (HIWORD (lParam) != CBN_SELCHANGE)
		return TRUE ;

	index = (int)SendDlgItemMessage (hDlg, IDCB_DRIVES, CB_GETCURSEL, 0, 0L) ;
	disk = (int)SendDlgItemMessage(hDlg, IDCB_DRIVES, CB_GETITEMDATA, index, 0L) ;
	olddisk = getdisk() ;
	if (olddisk == disk)
		return TRUE ;
	GetDlgItemText(hDlg, IDS_PATH, buffer, MAXPATH-1) ;
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
	if (getcwd(buffer, MAXPATH) == NULL)
		{
		GetDlgItemText(hDlg, IDS_PATH, buffer, 2) ;
		setdisk(olddisk) ;
		index = (int)SendDlgItemMessage (hDlg, IDCB_DRIVES, CB_SELECTSTRING, 0, (long)(LPSTR)buffer) ;
		SendDlgItemMessage(hDlg, IDCB_DRIVES, CB_SETCURSEL, index, 0) ;
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
	OFSTRUCT openBuff ;
	OBJECTID oiPaths ;
	MEMBLOCKID memPaths ;
	char far *lpPaths ;
	char far *fullpath ;
	char listString[MAXPATH + 14], ext[MAXEXT] ;
	int i, length ;

	oiPaths = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	if (oiPaths == 0)
		return ;
	memPaths = AMemAllocate(0) ;
	OTextGetToMemblock(oiPaths, memPaths) ;
//	AObjConvertToText(oiPaths, memPaths) ;
	lpPaths = (char far*)AMemLock(memPaths) ;
	length = lstrlen(lpPaths) ;
	for (i = length ; i > 0; i--)
		{
		if ((lpPaths[i] == '\n') || (lpPaths[i] == '\r'))
			{
			lpPaths[i] = lpPaths[i-1] = 0 ;
			fullpath = &lpPaths[i+1] ;
			i-- ;
			if (OpenFile(fullpath, &openBuff, OF_EXIST) != HFILE_ERROR)
				{
				fnsplit(fullpath, NULL, NULL, listString, ext) ;
				lstrcat(listString, ext) ;
				lstrcat(listString, "\t") ;
				lstrcat(listString, fullpath) ;
				SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_INSERTSTRING, 0, (long)listString) ;
				}
			}
		}
	if (OpenFile(lpPaths, &openBuff, OF_EXIST) != HFILE_ERROR)
		{
		fnsplit(lpPaths, NULL, NULL, listString, ext) ;
		lstrcat(listString, ext) ;
		lstrcat(listString, "\t") ;
		lstrcat(listString, lpPaths) ;
		SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_INSERTSTRING, 0, (long)listString) ;
		}
	AMemUnlock(memPaths) ;
	AMemFree(memPaths) ;
	}

void fillDirectoryList(HWND hDlg)
	{
	char pathname[MAXPATH], filter[13] ;
	char fname[14] ;
	int done, nFiles, i, index, startOfFilter ;
	fileName *list ;
	struct ffblk ffBlk ;

	SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_RESETCONTENT, 0L, 0L) ;
	GetDlgItemText(hDlg, IDS_PATH, pathname, MAXPATH) ;
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
			index = (int)SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_ADDSTRING, 0L, (long)fname) ;
			SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_SETITEMDATA, index, 1L) ;
			}
		done = findnext(&ffBlk) ;
		}
	if (startOfFilter > 3)
		{
		index = (int)SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_INSERTSTRING, 0, (LONG)"..\\") ;
		SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_SETITEMDATA, index, 0L) ;
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
		index = (int)SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_INSERTSTRING, -1, (long)list[i].name) ;
		SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_SETITEMDATA, index, 2L) ;
		}
	delete [] list ;
	}

BOOL ServiceDirectoryList(HWND hDlg, LPARAM lParam)
	{
	int index, type ;

	index = (int)SendDlgItemMessage (hDlg, IDL_DIRECTORY, LB_GETCURSEL, 0, 0L) ;
	switch (HIWORD (lParam))
		{
		case LBN_SELCHANGE:
			EnableWindow(GetDlgItem(hDlg, IDD_ADD), (BOOL)(index != LB_ERR)) ;
			if (index != LB_ERR)
				{
				type = (int)SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_GETITEMDATA, index, 0L) ;
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
			type = (int)SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_GETITEMDATA, index, 0L) ;
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
	char pathname[MAXPATH], filter[13] ;
	int length, i ;

	GetDlgItemText(hDlg, IDS_PATH, pathname, MAXPATH-1) ;
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
	char subdirectory[13], pathname[MAXPATH], filter[13] ;
	int i, length, index ;

	index = (int)SendDlgItemMessage (hDlg, IDL_DIRECTORY, LB_GETCURSEL, 0, 0L) ;
	SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_GETTEXT, index, (LPARAM)subdirectory) ;
	GetDlgItemText(hDlg, IDS_PATH, pathname, MAXPATH-1) ;
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
	char filename[13], pathname[MAXPATH], listboxString[MAXPATH+13] ;
	int i, length, index ;

	index = (int)SendDlgItemMessage (hDlg, IDL_DIRECTORY, LB_GETCURSEL, 0, 0L) ;
	SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_GETTEXT, index, (long)filename) ;
	GetDlgItemText(hDlg, IDS_PATH, pathname, MAXPATH) ;
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
	SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_INSERTSTRING, -1, (long)listboxString) ;
	return ;
	}

BOOL ServiceChosenList(HWND hDlg, LPARAM lParam)
	{
	int index ;

	index = (int)SendDlgItemMessage (hDlg, IDL_CHOSEN, LB_GETCURSEL, 0, 0L) ;
	if (HIWORD (lParam) == LBN_SELCHANGE)
		EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), (BOOL)(index != LB_ERR)) ;
	return TRUE ;
	}

BOOL AddFile(HWND hDlg)
	{
	int index, type ;

	index = (int)SendDlgItemMessage (hDlg, IDL_DIRECTORY, LB_GETCURSEL, 0, 0L) ;
	if (index == LB_ERR)
		return TRUE ;
	type = (int)SendDlgItemMessage(hDlg, IDL_DIRECTORY, LB_GETITEMDATA, index, 0L) ;
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

	index = (int)SendDlgItemMessage (hDlg, IDL_CHOSEN, LB_GETCURSEL, 0, 0L) ;
	SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_DELETESTRING, index, 0L) ;
	SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_SETCURSEL, index, 0L) ;
	if(LB_ERR == SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_GETCURSEL, 0, 0L))
		{
		SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_SETCURSEL, index-1, 0L) ;
		if(LB_ERR == SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_GETCURSEL, 0, 0L))
			EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), FALSE) ;
		}
	return TRUE ;
	}

BOOL WriteToOutputs(HWND hDlg)
	{
	int i, j, nFiles, length, maxFiles ;
//	char far *lpPaths ;
//	char far *lpFiles ;
	char listString[MAXPATH + 14] ;

	nFiles = (int)SendDlgItemMessage (hDlg, IDL_CHOSEN, LB_GETCOUNT, 0, 0L) ;
	maxFiles = (int)(65536L / (MAXPATH + 2)) ;
	if (nFiles > maxFiles)
		nFiles = maxFiles ;
	LPSTR lpPaths = new char[nFiles * (MAXPATH + 2)+1] ;
	if (lpPaths == NULL)
		return TRUE ;
//	AObjResizeData(oiPaths, 1, ) ;
//	lpPaths = (char far*)AObjLockData(oiPaths, 1) ;
	LPSTR lpFiles = new char[nFiles*14 + 1] ;
	if (lpFiles == NULL)
		{
		delete [] lpPaths ;
		return TRUE ;
		}
//	AObjResizeData(oiFiles, 1, ) ;
//	lpFiles = (char far*)AObjLockData(oiFiles, 1) ;
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
		SendDlgItemMessage(hDlg, IDL_CHOSEN, LB_GETTEXT, i, (LPARAM)listString) ;
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
	OBJECTID oiPaths = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiPaths, lpPaths) ;
	AFuncSetParameter(1, oiPaths) ;

	OBJECTID oiFiles = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiFiles, lpFiles) ;
	AFuncSetParameter(2, oiFiles) ;
	return TRUE ;
	}

