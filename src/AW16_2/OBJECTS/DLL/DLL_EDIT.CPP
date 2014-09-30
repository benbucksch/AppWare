///////////////////////////////////////////////////////////////
//
//	(c) 1993, 1994 Novell, Inc.  All rights reserved.
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
//////////////////////////////////////////////////////////////*/

/********************************************************************
 *
 *	"dll_edit.cpp: In here we will handle the dialogs for the DLL object.
 *	There are three dialogs, the DLL edit dialog, the Function Argument
 *	dialog and the Return Value dialog.
 *
 *	The DLL dialog allows the user to select a DLL throug the use of the
 *	Browser button, and once the DLL is selected and opened, it will
 *	display a list of external functions which the user can use to
 *	select the desired function.
 *
 *	The Return value dialog will allow the user to setup the return
 *	Value to the appropiate C type and mapp it to the appropiate
 *	appware ALM.
 *
 *
 *******************************************************************/

#define ASTRICT
#define STRICT

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mem.h>
#include <bwcc.h>
#include <helpids.h>

#include	<a_alm.h>
#include	<a_almutl.h>
#include	<op_wnd.h>
#include	<o_wnd.h>
#include	<intdebug.h>

#include "dll.h"


extern HINSTANCE	hInstance;


/*******************************************************************
 *	Local function prototypes
 ******************************************************************/

BOOL InitDialog(HWND, DLL_OBJECT*) ;
BOOL handleDLLName(HWND hDlg, LONG lParam);


DLL_OBJECT*	DLLObj;
OBJECTID oiDLL;

LONG		numberOfArgs;				// number of arguments in function call + 2.
MEMBLOCKID	objBlks[100];			// temp ptr array for the object data blocks.
char*		memBlkPtr;
char*		oldMemBlkPtr;
LONG		dataBlkLen;
LPVOID	dataBlockPtr;


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  EditObjectDialogProc
// DESCRIPTION: handle the dll dialog.
////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export EditObjectDialogProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
{
	char					szBuffer[DLLNAMESIZE];
	BOOL					retVal = FALSE;					// return value.
//	DLL_FUNCNAME*	theDLLFuncNamePtr;
	HFONT	hFont;
	OPENDLG	od(hDlg);
	int		len;

	switch (message)
	{
		case WM_INITDIALOG:
			oiDLL = (OBJECTID)lParam;
			DLLObj = (DLL_OBJECT*)AObjGetRuntimeInfo(oiDLL);
			retVal = (InitDialog(hDlg, DLLObj));
			break;

		case WM_COMMAND:
			switch (wParam)
			{
				case IDD_DLLNAME:
					retVal = handleDLLName(hDlg, (long)lParam);
					break;

				case IDD_DLLFUNCNAME:
					break;

				case IDD_RETURNVAL:
					DialogBoxParam(hInstance, "RETURNVAL", GetActiveWindow(),
							(DLGPROC)EditReturnValDialogProc, (long)lParam);
					break;

				case IDD_ARGUMENTS:
					DialogBoxParam(hInstance, "ARGUMENTS", GetActiveWindow(),
							(DLGPROC)EditArgumentDialogProc, (long)lParam);
					break;

				case IDD_BROWSER:
					len = GetDlgItemText(hDlg, IDD_DLLNAME, szBuffer, DLLNAMESIZE);

					if(len>0)
						od.SetInitDir(szBuffer);
					if(od.FileDlg()) {
						SetDlgItemText(hDlg, IDD_DLLNAME, od.GetName());
					}
					break;

				case IDD_DLLCTYPE:
					break;

				case IDD_DLLPASCALTYPE:
					break;

				case IDHELP:
					WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Brws) ;
					break;

				case IDOK:
					DLLObj = (DLL_OBJECT*)GetWindowLong(hDlg, DWL_USER);
					DLLObj->UpdateData(hDlg);
					hFont = (HFONT)SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, WM_GETFONT, 0, 0L);
 	        DeleteObject(hFont);
					EndDialog(hDlg, TRUE);
					break;

				case IDCANCEL:
					hFont = (HFONT)SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, WM_GETFONT, 0, 0L);
					DeleteObject(hFont);
					EndDialog (hDlg, FALSE) ;
					retVal = FALSE ;
					break;
			}			// switch (wParam)...
	}			// switch (message)...
	 return retVal;
}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  InitDialog
// DESCRIPTION:  Initialized the DLL dialog box, and copy the data
//							blocks D0..Dn to a temp area for editing, the memblock ID's
//							are saved in the array objBlks[].
//
//	return value:
//		TRUE - the data block where copied to a temp area for editing.
//		FALSE - could not allocate temp memory for the data blocks.
/////////////////////////////////////////////////////////////////////////////

BOOL InitDialog ( HWND hDlg, DLL_OBJECT* DLLObj)
{
	char			buffer[DLLNAMESIZE];
	OBJECTID	objID;
	int				i;
	HFONT			hFont;
	BOOL			retVal = TRUE;				// return value.
	int ii;
	DLL_FUNCNAME*	theDLLFuncNamePtr;

	AUtlSetTitleFont((const TYPEID__*)dllTypeID, NULL, GetDlgItem(hDlg, 150));
	AUtlCenterDialog(hDlg, 0);
  SetWindowLong(hDlg, DWL_USER, (long)DLLObj);		// save ptr to object.
	objID = DLLObj->theObjectID();
	AObjGetName(objID, buffer, DLLNAMESIZE);
	SetWindowText(hDlg, buffer);			// sets the dialog box caption text
	hFont = CreateFont(-10,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
	SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	EnableWindow(GetDlgItem(hDlg, IDD_DLLFUNCNAME), FALSE);	// alm type.
	EnableWindow(GetDlgItem(hDlg, IDD_DLLCTYPE), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDD_DLLPASCALTYPE), FALSE);

	numberOfArgs = AObjGetDataCount(objID);
	for (i=0; i<numberOfArgs; ++i) {		// copy data blocks to temp area.
		dataBlkLen = AObjGetDataSize(objID, i);
		objBlks[i] = AMemAllocate(dataBlkLen);
    if(objBlks[i]) {
			dataBlockPtr = AObjLockData(objID, i);
			memBlkPtr = (char*)AMemLock(objBlks[i]);
			memcpy(memBlkPtr, dataBlockPtr, (size_t)dataBlkLen);
			AObjUnlockData(objID, i);
			AMemUnlock(objBlks[i]);
		}
		else
			retVal = FALSE;			// not abled to allocate temp memory for editing.
	}			// for (i=0; numberOfArgs...

	theDLLFuncNamePtr = (DLL_FUNCNAME*)AMemLock(objBlks[0]);
	if(theDLLFuncNamePtr != NULL) {	// init DLL and function name.
		SetDlgItemText(hDlg, IDD_DLLNAME, theDLLFuncNamePtr->DLLName);
		ii = (int)SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_SELECTSTRING, -1, (long)theDLLFuncNamePtr->DLLFunction);
		SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_SETCURSEL, ii, 0L);
		if (theDLLFuncNamePtr->languageType == C_FUNCTION)
			CheckDlgButton(hDlg, IDD_DLLCTYPE, TRUE);
		else
			CheckDlgButton(hDlg, IDD_DLLPASCALTYPE, TRUE);

		AMemUnlock(objBlks[0]);
	}
	return retVal;
}         // InitDialog ( HWND hDlg,...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  handleDLLName
// DESCRIPTION:  Will handle the dll name entered by the user in the
//               DLL edit dialog.
//
/////////////////////////////////////////////////////////////////////////////

BOOL handleDLLName(HWND hDlg, LONG lParam)
{
	char	szBuffer[DLLNAMESIZE];
	int		len;
	BOOL	retVal = TRUE;					// return value.

	switch (HIWORD (lParam)) {
		case EN_KILLFOCUS:
			break;

    case EN_CHANGE:
			len = GetDlgItemText(hDlg, IDD_DLLNAME, &szBuffer[0], DLLNAMESIZE-1);
	  //		len = SendDlgItemMessage(hDlg, IDD_DLLNAME, EM_GETLINE, 0, (LONG)&szBuffer[0]);
			if (len > DLLNAMESIZE)
				len = DLLNAMESIZE-1;
			szBuffer[len] = '\0';              // Terminate the name.
			DLLObj->openDLLFile(hDlg, &szBuffer[0]);
			break;
	}
	return retVal;
}


////////////////////////////////////////////////////////////////////////////
// DLL_OBJECT::UpdateData: update the data in the dll object, Ok was pressed
//	in the DLL edit dialog.
///////////////////////////////////////////////////////////////////////////

void	DLL_OBJECT::UpdateData(
	HWND	hDlg)
{
	char					dllName[DLLNAMESIZE];
	char					dllFunctName[FUNCNAMESIZE];
	OBJECTID			objID;
  int						i;
	long					oldDataCount;
	DLL_FUNCNAME*	theDLLFuncNamePtr;
	int						len, index;

	objID = theObjectID();
	oldDataCount = AObjGetDataCount(objID);
	for (i=0; i<oldDataCount; ++i)			// delete all data blocks.
		AObjDeleteData(objID, 0L);

			// set data0 fields...
	theDLLFuncNamePtr = (DLL_FUNCNAME*)AMemLock(objBlks[0]);
	if(theDLLFuncNamePtr != NULL) {	// init DLL and function name.
		len = GetDlgItemText(hDlg, IDD_DLLNAME, dllName, DLLNAMESIZE);
//		len = SendDlgItemMessage(hDlg, IDD_DLLNAME, EM_GETLINE, 0, (LONG)dllName);
		dllName[len] = '\0';              // Terminate the name.
		index = (int)SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_GETCURSEL, 0, 0);
		SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_GETLBTEXT, index, (long)dllFunctName);
		strcpy(theDLLFuncNamePtr->DLLName, dllName);
		strcpy(theDLLFuncNamePtr->DLLFunction, dllFunctName);
		if (IsDlgButtonChecked(hDlg, IDD_DLLCTYPE))
			theDLLFuncNamePtr->languageType = (int)C_FUNCTION;
		else
			theDLLFuncNamePtr->languageType = (int)PASCAL_FUNCTION;
		AMemUnlock(objBlks[0]);
	}

	for (i=0; i<numberOfArgs; ++i) {		// copy data blocks to temp area.
		memBlkPtr = (char*)AMemLock(objBlks[i]);
		if(memBlkPtr != NULL) {
			AObjAddData(objID, memBlkPtr, AMemGetSize(objBlks[i]));
//			if (!AObjAddData(objID, memBlkPtr, AMemGetSize(objBlks[i])))
//				retVal = FALSE;			// not abled to add data block to object.
			AMemUnlock(objBlks[i]);
			AMemFree(objBlks[i]);
			objBlks[i] = NULL;
		}
//		else
//			retVal = FALSE;			// not abled to access temp data block.
	}			// for (i=0; numberOfArgs...
//	return retVal;
}			// DLL_OBJECT::UpdateData(...


////////////////////////////////////////////////////////////////////////////

char* OPENDLG::filter = "Programs\0*.dll\0";
char* OPENDLG::title  = "Browse";


//////////////////////////////////////////////////

static
UINT		ALMCALLBACK OpenFileHook(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	UINT		nRes;

	I_UNUSED(wParam);
	I_UNUSED(lParam);
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

////////////////////////////////////////////////////////////////////////////

OPENDLG::OPENDLG(
	HWND	hWnd)

{
	filename[0]	= '\0';
	file_title[0]	= '\0';

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize 		= sizeof(OPENFILENAME);
	ofn.hwndOwner 			= hWnd;
	ofn.lpstrFilter 		= filter;
	ofn.nFilterIndex 		= 1;
	ofn.lpstrFile 			= filename;
	ofn.nMaxFile 				= 256;
	ofn.lpstrTitle			= title;
	ofn.lpstrFileTitle	= file_title;
	ofn.nMaxFileTitle		= 13;
	ofn.Flags						= OFN_HIDEREADONLY |
												OFN_FILEMUSTEXIST |
												OFN_NOCHANGEDIR |
												OFN_PATHMUSTEXIST |
												OFN_ENABLEHOOK;
	ofn.lpfnHook        = OpenFileHook;
}


////////////////////////////////////////////////////////////////////////////

BOOL	OPENDLG::FileDlg(
	char*	p)
{
	if (p != NULL)
		ofn.lpstrFilter = p;
	return GetOpenFileName((LPOPENFILENAME)&ofn);
}

////////////////////////////////////////////////////////////////////////////
