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
 *	"scrp_rdlg.cpp: In here we will handle the dialogs for the scrp object.
 *	There are three dialogs, the scrp edit dialog, the Function Argument
 *	dialog and the Return Value dialog.
 *
 *	The scrp dialog allows the user to select a scrp throug the use of the
 *	Browser button, and once the scrp is selected and opened, it will
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
#include	<o_number.h>
#include	<o_text.h>
#include	<intdebug.h>

#include "scrp_bas.h"



extern	MEMBLOCKID	argsBlks[MAX_NUM_OFARGS];	// temp ptrs for objs data blocks.
extern	HSBI			SBIHandle;
extern	BOOL			setErrorInConsoleWin;		// true set error in console.
extern	int			errorIndex;						// error index.


/*******************************************************************
 *	Local function prototypes
 ******************************************************************/

BOOL initErrLogDlg(HWND);
BOOL errLstSelection(HWND, LONG);


/*******************************************************************
 *	External function prototypes
 ******************************************************************/

extern void Edit_Script(HWND, OBJECTID);			// edit scrip function.



/////////////////////////////////////////////////////////////////////////////
// errorLogDialogProc: Display the compiler error log.
////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export errorLogDialogProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
{
	BOOL	retVal = FALSE;

	switch (message) {
		case WM_INITDIALOG:
			retVal = initErrLogDlg(hDlg);
			break;

		case WM_COMMAND:
			switch (wParam) {
				case IDD_ERROR_LIST:	// list of function arguments.
					retVal = errLstSelection(hDlg, (long) lParam);
					break;

				case IDOK:
					setErrorInConsoleWin = FALSE;
					errorIndex = -1;
					EndDialog (hDlg, FALSE) ;
					break;
				}
	}
	return retVal ;
}			// errorLogDialogProc (HWND hDlg,...


/////////////////////////////////////////////////////////////////////////////
// initErrLogDlg: Initialized compiler error log dialog box.
/////////////////////////////////////////////////////////////////////////////

BOOL initErrLogDlg(HWND hDlg)
{
	int						len = 0;
	char						temp[SCRPNAMESIZE+100]; 		// fuck
	int						i;
	BOOL						retVal = TRUE;
	COMPILER_ERROR_LOG*	errorLogPtr;		// ptr to error log DB.
	SCRP_FUNCNAME*			scrpFuncName;		// scrp PTR to data0.
	int						index;
	char						sblErrTextBuff[255];
	long						tempVar;

	AUtlCenterDialog(hDlg, 0);

	scrpFuncName = (SCRP_FUNCNAME*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
	index = scrpFuncName->numOfErrorsInLog;
	AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
	if (index > 0) {
		errorLogPtr = (COMPILER_ERROR_LOG*)AMemLock(argsBlks[ERROR_LOG_DBINDEX]);
		for (i=0; i<index; ++i) {
			itoa(errorLogPtr->err, &temp[0], 10);
//			strcpy(&temp[0], "<Undefined>");
			len = strlen(temp);
			temp[len] = '\x09';					// tab to object field.
			len++;
//			itoa(errorLogPtr->severity, &temp[len], 10);
			if (errorLogPtr->severity == SBL_COMPILER_OK)
				strcpy(&temp[len], "OK");
			if (errorLogPtr->severity == SBL_COMPILER_WARNING)
				strcpy(&temp[len], "WARNING");
			if (errorLogPtr->severity == SBL_COMPILER_ERROR)
				strcpy(&temp[len], "ERROR");
//			strcpy(&temp[len], "<Undefined>");
			len = strlen(temp);
			temp[len] = '\x09';					// tab to object field.
			len++;
			itoa(errorLogPtr->lineNumber, &temp[len], 10);
			len = strlen(temp);
			temp[len] = '\x09';					// tab to object field.
			len++;
			SblGetPlainErrorText(errorLogPtr->err, &sblErrTextBuff[0],255);
			strcpy(&temp[len], &sblErrTextBuff[0]);
			tempVar = sizeof(COMPILER_ERROR_LOG);
//			errorLogPtr = errorLogPtr + sizeof(COMPILER_ERROR_LOG);
			errorLogPtr = (COMPILER_ERROR_LOG*)((long)errorLogPtr + tempVar);
				// add it to the argument list in the dialog.
			SendDlgItemMessage(hDlg, IDD_ERROR_LIST, LB_INSERTSTRING, i, (LONG)&temp);
		}			// for (i=0; i<index; ++i)...
		AMemUnlock(argsBlks[ERROR_LOG_DBINDEX]);
	}			// if (index > 0)...
	return retVal;
}			// initErrLogDlg(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  errLstSelection
// DESCRIPTION: An argument have been selected update the C type and the ALM
//		object type and name, pass value by ref or value radio buttons.
/////////////////////////////////////////////////////////////////////////////

BOOL errLstSelection(HWND hDlg, LONG lParam)
{
	long			selNumOfArgs, indexOBJ, lineNumber, tempVar;
	int				selectBuff[MAX_NUM_OFARGS];
	char			objectName[SCRPNAMESIZE];
	BOOL			done;
	BOOL			retVal = FALSE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.
	COMPILER_ERROR_LOG*	errorLogPtr;		// ptr to error log DB.

	switch (HIWORD (lParam)) {
		case CBN_DBLCLK:
			selNumOfArgs = SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELCOUNT, 0, 0);
			if (selNumOfArgs == 1) {
				SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, (int)selNumOfArgs, (LONG) &selectBuff);
				errorIndex = selectBuff[0];
				setErrorInConsoleWin = TRUE;
				Edit_Script(hDlg, 0L);
			}			// if (selNumOfArgs == 1)...
			break;

		case CBN_SELCHANGE:			// setup the object name lists.
			break;
	}
	return retVal ;
}			// errLstSelection(...

