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
 *	"scrp_adlg.cpp: In here we will handle the argument dialogs for
 *	the scrp object. There are three dialogs, the scrp edit dialog,
 *	the Function Argument dialog and the Return Value dialog.
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

#include <a_alm.h>
#include <a_almutl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <io.h>

#include	<a_almutl.h>
#include	<op_wnd.h>
#include	<o_wnd.h>
#include	<o_number.h>
#include	<o_text.h>
#include	<intdebug.h>

#include "scrp_bas.h"

#define scrpTempFile "ecjtest0.sbl"


extern HINSTANCE	hInstance;
extern SBL_LPMEMBER_INFO	typeMemberInfoPtr;	// ptr to structure member info.
extern int			numberOfMembers;			// number of members in structure.
extern MEMBLOCKID	typeMemberInfoBlkID;		// mem block ID for member infos.
extern OBJECTID		theObjectID;
LONG numberOfDataBlks;		// number of data blocks in object.
HWND theArgumentDlg;


/*******************************************************************
 *	Local function prototypes
 ******************************************************************/

BOOL argLstSelection(HWND, LONG);
BOOL deleteArg (HWND, OBJECTID);
BOOL insertArg (HWND, OBJECTID);
BOOL updateArg (HWND, OBJECTID);
BOOL initArgumentDlg (HWND, OBJECTID);
BOOL initArgList(HWND);
LPSTR getscrpTypeName(int);
BOOL saveArgumentsBlks(OBJECTID);
BOOL setUnsetConst(HWND);
void conVertConstToString(char* , long double);
void Edit_Script(HWND, OBJECTID);			// edit scrip function.
LONG Compile_Script(OBJECTID);
extern BOOL setGlobalVariables();
extern BOOL setStructureMembers();
VOID registerAllGlobals();
LONG cnvrt_to_file(OBJECTID, BOOL);
LONG cnvrt_from_file(OBJECTID);
void goHandleSBIEditMessage(WORD, WORD, LONG);


/*******************************************************************
 *	Call back prototypes.
 ******************************************************************/

SBL_STATUS DLLENTRY Compiler_ErrorProc(SBL_WORD, SBL_STATUS, SBL_SHORT, SBL_LPSTR, SBL_LPSTR);
SBL_SHORT DLLENTRY Compiler_ImplicitProc(SBL_LPSTR);
void DLLENTRY Compiler_RegVarsProc();
	// ptr's to the call back functions.
SBL_ERRPROC pErrorProc = &Compiler_ErrorProc;
SBL_IMPLICITPROC pImplicitProc = &Compiler_ImplicitProc;
SBL_CALLBACKPROC pCallbackProc = &Compiler_RegVarsProc;



MEMBLOCKID			argsBlks[MAX_NUM_OFARGS];	// temp ptr array for parameter blocks.
LONG					nmbrOfArgs;		// params include return, skip DATA 0,1,2.
SCRP_ARGUMENTS*	srcBlkPtr;
SCRP_ARGUMENTS*	destBlkPtr;
int					argBlkLen;
int					selectedParam;				// selected parameter in parameter list.
OBJECTID				compilingObject;			// id of object being compile.

SCRP_OBJECT*		scrpObj;


/**************************************************************************
 *	SBL basic init structure...
 *************************************************************************/

SBI_INIT		SBLInit;				// initialize structure.
SBI_LPINIT	pSBLInit;
HSBI			SBIHandle = NULL;
extern HSBL	SBLHandle;
BOOL			setErrorInConsoleWin = FALSE;		// true set error in console.
int			errorIndex = -1;						// error index.


////////////////////////////////////////////////////////////////////////////
// FUNCTION:  EditArgumentDialogProc
// DESCRIPTION: handle the argument dialog.
////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export EditArgumentDialogProc(HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
{
	BOOL					retVal = FALSE;
	OBJECTID				objID;
	SCRP_FUNCNAME*		scrpFuncName;		// scrp PTR to data0.

	switch (message) {
		case WM_INITDIALOG:
			retVal = (initArgumentDlg(hDlg, (OBJECTID)lParam));
			break;

		case WM_COMMAND:
			switch (wParam) {
				case IDD_ARGUMENTLST:	// list of function arguments.
					retVal = argLstSelection(hDlg, (long) lParam);
					break;

				case IDD_INSERT:			// insert argument in list.
					retVal = (insertArg(hDlg, (OBJECTID)lParam));
					break;

				case IDD_DELETE:		  // delete argument argument from list.
					retVal = (deleteArg(hDlg, (OBJECTID)lParam));
					break;

				case IDD_OPENSCRIPT:
					objID = (OBJECTID)GetWindowLong(hDlg, DWL_USER);
					Edit_Script(hDlg, objID);
					break;

				case IDD_ERRORLOG:
					retVal = DialogBoxParam(hInstance, "ERRORLOG", hDlg,
						(DLGPROC)errorLogDialogProc, (long)lParam);
					break;

				case IDD_DEBUGGER:
					scrpFuncName = (SCRP_FUNCNAME*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
					if(SendDlgItemMessage(hDlg, IDD_DEBUGGER, BM_GETCHECK, 0, 0))
						scrpFuncName->debuggEnable = TRUE;
					else
						scrpFuncName->debuggEnable = FALSE;
					AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
					break;

				case IDD_GLOBALVARS:
					scrpFuncName = (SCRP_FUNCNAME*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
					if(SendDlgItemMessage(hDlg, IDD_GLOBALVARS, BM_GETCHECK, 0, 0))
						scrpFuncName->globalScript = TRUE;
					else
						scrpFuncName->globalScript = FALSE;
					AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
					break;

				case IDD_SCRIPT:
					scrpFuncName = (SCRP_FUNCNAME*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
					scrpFuncName->function = TRUE;
					scrpFuncName->structure = FALSE;
					scrpFuncName->variables = FALSE;
					ShowWindow(GetDlgItem(hDlg, IDD_OPENSCRIPT), SW_SHOW);
					ShowWindow(GetDlgItem(hDlg, IDD_ERRORLOG), SW_SHOW);
					ShowWindow(GetDlgItem(hDlg, IDD_DEBUGGER), SW_SHOW);
					AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
			      SetWindowText(hDlg, "Basic Script");
					SetDlgItemText(hDlg, 150, "Basic");
					break;

				case IDD_STRUCTURE:
					scrpFuncName = (SCRP_FUNCNAME*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
					scrpFuncName->structure = TRUE;
					scrpFuncName->function = FALSE;
					scrpFuncName->variables = FALSE;
					ShowWindow(GetDlgItem(hDlg, IDD_OPENSCRIPT), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDD_ERRORLOG), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDD_DEBUGGER), SW_HIDE);
					AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
					SetWindowText(hDlg, "Structure");
					SetDlgItemText(hDlg, 150, "Structure");
					break;

				case IDD_VARIABLES:
					scrpFuncName = (SCRP_FUNCNAME*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
					scrpFuncName->variables = TRUE;
					scrpFuncName->structure = FALSE;
					scrpFuncName->function = FALSE;
					ShowWindow(GetDlgItem(hDlg, IDD_OPENSCRIPT), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDD_ERRORLOG), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDD_DEBUGGER), SW_HIDE);
					AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
					SetWindowText(hDlg, "Variables");
					SetDlgItemText(hDlg, 150, "Variables");
					break;

				case IDOK:
					objID = (OBJECTID)GetWindowLong(hDlg, DWL_USER);
					retVal = cnvrt_from_file(objID);						// save the script.
					retVal = saveArgumentsBlks(objID);		// save args, script.
					if(SBIHandle != NULL) {
						SBITerm(SBIHandle);
						SBIHandle = NULL;
						SBLHandle = NULL;
						unlink ("ecjtest0.sbl");
					}
					scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(objID, SCRP_INFO_DBINDEX);
					AObjGetName(objID, &scrpFuncName->scrpFunction[0], SBL_CCH_MAX_NAME);
					AObjUnlockData(objID, SCRP_INFO_DBINDEX);
					EndDialog (hDlg, FALSE);
					break;

				case IDCANCEL:
					if(SBIHandle != NULL) {
						SBITerm(SBIHandle);
						SBIHandle = NULL;
						SBLHandle = NULL;
					}
					EndDialog (hDlg, FALSE);
					retVal = TRUE;
					break;
			}				// switch (wParam)...

		default:
			if(SBIHandle != NULL)
				goHandleSBIEditMessage(message, wParam, lParam);
			break;
	}				// switch (message)...
	 return retVal ;
}			// EditArgumentDialogProc(...


/////////////////////////////////////////////////////////////////////////////
// FUNCION: goHandleSBIEditMessage
// DESCRIPTION: Go process ans SBI editor messages.  If the user
//				clicked in the compiler error list, the error info is set in the
//				sbi editor console window.
//
/////////////////////////////////////////////////////////////////////////////

void goHandleSBIEditMessage(WORD message, WORD wParam, LONG lParam)
{
	MSG						msg;
	char						temp[SCRPNAMESIZE+100]; 		// fuck
	int						len = 0;
	COMPILER_ERROR_LOG*	errorLogPtr;		// ptr to error log DB.
	char						sblErrTextBuff[255];
	long						tempVar;
	SBL_ULONG				lineCharOffset, nextLineCharOffset;
	SBL_LONG					editSelLine = 0;

	if (setErrorInConsoleWin) {
		errorLogPtr = (COMPILER_ERROR_LOG*)AMemLock(argsBlks[ERROR_LOG_DBINDEX]);
		tempVar = sizeof(COMPILER_ERROR_LOG);
		errorLogPtr = (COMPILER_ERROR_LOG*)((long)errorLogPtr + tempVar*errorIndex);
		len = 0;

		strcpy(&temp[len], "LINE:");
		len = strlen(temp);
		temp[len] = '\x09';					// tab to object field.
		len++;
		itoa(errorLogPtr->lineNumber, &temp[len], 10);
		len = strlen(temp);
		temp[len] = '\x09';					// tab to object field.
		len++;
		temp[len] = '\x09';					// tab to object field.
		len++;

		if (errorLogPtr->severity == SBL_COMPILER_OK)
			strcpy(&temp[len], "OK:     ");
		if (errorLogPtr->severity == SBL_COMPILER_WARNING)
			strcpy(&temp[len], "WARNING:");
		if (errorLogPtr->severity == SBL_COMPILER_ERROR)
			strcpy(&temp[len], "ERROR:  ");
		len = strlen(temp);
		temp[len] = '\x09';					// tab to object field.
		len++;

		itoa(errorLogPtr->err, &temp[len], 10);
		len = strlen(temp);
		temp[len] = '\x09';					// tab to object field.
		len++;
		temp[len] = '\x09';					// tab to object field.
		len++;

		if (errorLogPtr->severity == SBL_COMPILER_ERROR)
			strcpy(&temp[len], "DESCRIPTION:");
		len = strlen(temp);
		temp[len] = '\x09';					// tab to object field.
		len++;

		SblGetPlainErrorText(errorLogPtr->err, &sblErrTextBuff[0],255);
		strcpy(&temp[len], &sblErrTextBuff[0]);

		len = strlen(temp);
		temp[len] = '\x0d';					// CR.
		len++;
		temp[len] = '\x0a';					// LF.
		len++;
		temp[len] = '\0';						// terminate it...

		AMemUnlock(argsBlks[ERROR_LOG_DBINDEX]);

		lineCharOffset = SBIEditControl(SBIHandle,
											 SBIEC_LINEINDEX,
											 0,
											 errorLogPtr->lineNumber-1);
		nextLineCharOffset = SBIEditControl(SBIHandle,
											 SBIEC_LINEINDEX,
											 0,
											 errorLogPtr->lineNumber);

		if (SBICmdQuery(SBIHandle, SBI_CMD_CONSOLE) == MF_UNCHECKED)
			SBICmdExec(SBIHandle, SBI_CMD_CONSOLE);
		else {
			SBICmdExec(SBIHandle, SBI_CMD_CONSOLE);
			SBICmdExec(SBIHandle, SBI_CMD_CONSOLE);
		}
		SBIEditControl(SBIHandle, SBIEC_APPENDCONSOLE, 0, (SBL_LONG)&temp);

		editSelLine = MAKELONG(lineCharOffset, nextLineCharOffset-1);
//		editSelLine = MAKELONG(nextLineCharOffset, lineCharOffset);
		nextLineCharOffset = SBIEditControl(SBIHandle,
											 SBIEC_SETSEL,
											 0,
											 editSelLine);

		setErrorInConsoleWin = FALSE;
	}			// if (setErrorInConsoleWin...

	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	if(!SBIPreTrans(&msg)) {
//					TranslateMessage(&msg);    /* Translates virtual key codes       */
//					DispatchMessage(&msg);     /* Dispatches message to window       */
	}
}			// goHandleSBIEditMessage(WORD wParam, LONG lParam...

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  argLstSelection
// DESCRIPTION: An argument have been selected update the C type and the ALM
//		object type and name, pass value by ref or value radio buttons.
/////////////////////////////////////////////////////////////////////////////

BOOL argLstSelection(HWND hDlg, LONG lParam)
{
	long			selNumOfArgs, indexOBJ;
	int				selectBuff[MAX_NUM_OFARGS];
	int				i;
	char			objectName[SCRPNAMESIZE];
	BOOL			done;
	BOOL			retVal = FALSE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	switch (HIWORD (lParam)) {
		case CBN_DBLCLK:
			selNumOfArgs = SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELCOUNT, 0, 0);
			if (selNumOfArgs == 1) {
				SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, (int)selNumOfArgs, (LONG) &selectBuff);
				selectedParam = selectBuff[0];
//				srcBlkPtr = (SCRP_ARGUMENTS*)AMemLock(argsBlks[selectedParam]);	// current argument.
				retVal = DialogBoxParam(hInstance, "RETURNVAL", hDlg,
					(DLGPROC)EditReturnValDialogProc, (long)lParam);
				retVal = (updateArg(hDlg, (OBJECTID)lParam));
			}			// if (selNumOfArgs == 1)...
			break;

		case CBN_SELCHANGE:			// setup the object name lists.
			selNumOfArgs = SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELCOUNT, 0, 0);
			SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, (int)selNumOfArgs, (LONG) &selectBuff);
			i = selectBuff[0];
			if (selNumOfArgs == 1) {
				if (i==SCRPRETPARAMDLGINDEX)
					EnableWindow(GetDlgItem(hDlg, IDD_DELETE), FALSE);	// can't delete return parameter.
				else
					EnableWindow(GetDlgItem(hDlg, IDD_DELETE), TRUE);	// allow deletion.
			}				// if (index == 1)...

			else {			// none or multiple items are selected.
				if (selNumOfArgs == 0)			// no items selected.
					EnableWindow(GetDlgItem(hDlg, IDD_DELETE), FALSE);	// nothing to delete.
				else			// multiple items are selected.
					if (i==SCRPRETPARAMDLGINDEX)
						EnableWindow(GetDlgItem(hDlg, IDD_DELETE), FALSE);	// can't delete return parameter.
					else
						EnableWindow(GetDlgItem(hDlg, IDD_DELETE), TRUE);	// allow deletion.
			}				// if (selNumOfArgs > 1)...
			retVal = TRUE;
	}
	return retVal ;
}			// argLstSelection(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  initArgumentDlg
// DESCRIPTION:  Initialized scrp function argument dialog. Save current
//			  arguments into a temp area, for editing.
//
/////////////////////////////////////////////////////////////////////////////

BOOL initArgumentDlg(HWND hDlg, OBJECTID oiScrp)
{
	int			i;
	int			theTab[1];
	BOOL		retVal;
	LONG		dataBlkLen;
	SCRP_FUNCNAME*		scrpFuncName;		// scrp PTR to data0.


	I_UNUSED(oiScrp);

	theArgumentDlg = hDlg;			// save the dialog handle to allow update from the parameter setting dialog.
	AUtlSetTitleFont((const TYPEID__*)scrpTypeID, NULL, GetDlgItem(hDlg, 150));
	AUtlCenterDialog(hDlg, 0);
	SetWindowLong(hDlg, DWL_USER, (long)oiScrp);		// save ptr to object.

	EnableWindow(GetDlgItem(hDlg, IDD_INSERT), TRUE);			// enable the insert button.
	theTab[0] = 56;
	SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_SETTABSTOPS, 1, (LONG) &theTab[0]);

			// copy parameter blocks to local edit area.
	numberOfDataBlks = AObjGetDataCount(oiScrp);
				// skip DATA 0,1,2 remember that DATA3 is return parameter.
	nmbrOfArgs = numberOfDataBlks-SCRP_PARAM_DBINDEX;

	argBlkLen = sizeof(SCRP_ARGUMENTS);
	for (i=0; i<numberOfDataBlks; ++i) {		// copy data blocks to temp area.
		if (i==0)									// TEMP TEMP
			dataBlkLen = sizeof(SCRP_FUNCNAME);				// TEMP TEMP
		else     									// TEMP TEMP
			dataBlkLen = AObjGetDataSize(oiScrp, i);

		argsBlks[i] = AMemAllocate(dataBlkLen);
		if(argsBlks[i]) {
			destBlkPtr = (SCRP_ARGUMENTS*)AMemLock(argsBlks[i]);	// local edit area.
			if (destBlkPtr !=NULL ) {
				srcBlkPtr = (SCRP_ARGUMENTS*)AObjLockData(oiScrp, i);	// get Data Block.
				memcpy(destBlkPtr, srcBlkPtr, dataBlkLen);
				AMemUnlock(argsBlks[i]);
				AObjUnlockData(oiScrp,i);
			}			// if (srcBlkPtr !=NULL...
		}
		else
			retVal = FALSE;			// not abled to allocate temp memory for editing.
	}			// for (i=0; i<numberOfDataBlks; ++i)...


	scrpFuncName = (SCRP_FUNCNAME*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
	if(scrpFuncName->debuggEnable)
		CheckDlgButton(hDlg, IDD_DEBUGGER, TRUE);
	else
		CheckDlgButton(hDlg, IDD_DEBUGGER, FALSE);
	if(scrpFuncName->globalScript)
		CheckDlgButton(hDlg, IDD_GLOBALVARS, TRUE);
	else
		CheckDlgButton(hDlg, IDD_GLOBALVARS, FALSE);

	if(scrpFuncName->function) {
		CheckDlgButton(hDlg, IDD_SCRIPT, TRUE);
		CheckDlgButton(hDlg, IDD_STRUCTURE, FALSE);
	}
	else {
		CheckDlgButton(hDlg, IDD_SCRIPT, FALSE);
		if(scrpFuncName->structure) {
			CheckDlgButton(hDlg, IDD_STRUCTURE, TRUE);
			CheckDlgButton(hDlg, IDD_VARIABLES, FALSE);
			SetWindowText(hDlg, "Structure");
			SetDlgItemText(hDlg, 150, "Structure");
		}
		else {
			CheckDlgButton(hDlg, IDD_VARIABLES, TRUE);
			CheckDlgButton(hDlg, IDD_STRUCTURE, FALSE);
			SetWindowText(hDlg, "Variables");
			SetDlgItemText(hDlg, 150, "Variables");
		}

		ShowWindow(GetDlgItem(hDlg, IDD_OPENSCRIPT), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDD_ERRORLOG), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDD_DEBUGGER), SW_HIDE);
//		AUtlSetTitleFont((const TYPEID__*)scrpTypeID, NULL, GetDlgItem(hDlg, 150));
	}
	AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
	retVal = initArgList(hDlg);
	return retVal;
}			// initArgumentDlg(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  initArgList
// DESCRIPTION: Init the argument list.
/////////////////////////////////////////////////////////////////////////////

BOOL initArgList(HWND hDlg)
{
	int			len = 0;
	char			temp[SCRPNAMESIZE+100]; 		// fuck
	char			objName[SCRPNAMESIZE];
	int			scrpType;
	int			i;
	BOOL			aConst;
	BOOL			retVal = TRUE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	for (i=SCRP_PARAM_DBINDEX; i<nmbrOfArgs+SCRP_PARAM_DBINDEX; ++i) {
		if(argsBlks[i]) {
			srcBlkPtr = (SCRP_ARGUMENTS*)AMemLock(argsBlks[i]);
			scrpType = srcBlkPtr->paramType;				// get scrpType.
			if (scrpType >= 0) {
				strcpy((char*)&temp[0], (char*)getscrpTypeName(scrpType));
				len = strlen((char*)&temp);
				aConst = srcBlkPtr->constant;
				if (aConst) {						// is it a constant?
					temp[len] = '\x09';		// yes - tab to object field.
					len++;
					conVertConstToString(&(constValStr[0]), srcBlkPtr->constVal);
					strcpy(&temp[len], (LPSTR)&(constValStr[0]));
				}
				if (!aConst) {					// not a constant, add object name.

					temp[len] = '\x09';					// tab to object field.
					len++;
   	         TYPEID objType;
					if (srcBlkPtr->theArgObject) {
						objType = AObjGetType(srcBlkPtr->theArgObject);
						ATypeGetName(objType, (LPSTR)&temp[len], SCRPNAMESIZE);
//						ltoa((long)objType,&temp[len],10);		// convert type to text, for display.
					}
					else
						strcpy(&temp[len], "<Undefined>");			// no ALM name...

					len = strlen(temp);
					temp[len] = '\x09';		// tab to object field.
					len++;
					if (srcBlkPtr->theArgObject)
						AObjGetName(srcBlkPtr->theArgObject, (LPSTR)&objName, SCRPNAMESIZE);
					strcpy(&temp[len], &objName[0]);
					len = strlen(temp);
				}			// if (!aConst)...
			}			// if (scrpType >= 0)...
			else {				// must be an undefined argument...
      		strcpy(&temp[0], "<Undefined>");			// initialize it...
				len = strlen(temp);
				temp[len] = '\x09';					// tab to object field.
				len++;
				srcBlkPtr->constant = FALSE;
				strcpy(&temp[len], "<No ALM>");
				len = strlen(temp);
				temp[len] = '\x09';					// tab to object field.
				len++;
				strcpy(&temp[len], "<Undefined>");			// initialize it...
			}			// else {...  undefined argument

				// add it to the argument list in the dialog.
			SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_INSERTSTRING, i-SCRP_PARAM_DBINDEX, (LONG)&temp);
			AMemUnlock(argsBlks[i]);
		}			// if(argsBlks[index])...
	}			// for (i=1; i<nmbrOfArgs+SCRP_PARAM_DBINDEX; ++i)...
	return retVal;
}			// initArgList(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  getscrpTypeName
// DESCRIPTION: Return a string to identify the C type.
/////////////////////////////////////////////////////////////////////////////

LPSTR getscrpTypeName(int scrpType)
{
	LPSTR	theNamePtr;

	switch (scrpType) {
		case BASTYPE_INTEGER:
				theNamePtr = "Integer";
			break;

//		case CTYPE_USHORT:
//				theNamePtr = "unsigned short";
//			break;

		case BASTYPE_LONG:
				theNamePtr = "Long";
			break;

//		case CTYPE_ULONG:
//				theNamePtr = "unsigned long";
//			break;

		case BASTYPE_SINGLE:
				theNamePtr = "Single";
			break;

		case BASTYPE_DOUBLE:
				theNamePtr = "Double";
			break;

		case BASTYPE_CURRENCY:
				theNamePtr = "Currency";
			break;

		case BASTYPE_FIXEDSTR:
				theNamePtr = "Fixed String";			// fixed strings can not be of zero lenght
			break;

		case BASTYPE_DYNAMICSTR:
				theNamePtr = "Dynamic String";		// the lenght of a dynamic string is 0..32,767.
			break;

		default:
				theNamePtr = "shit";
			break;
	}			// switch (scrpType)...
	return theNamePtr;
}			// getscrpTypeName(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  updateArg
// DESCRIPTION: Update a scrp function argument.
/////////////////////////////////////////////////////////////////////////////

BOOL updateArg( HWND hDlg, OBJECTID oiScrp)
{
	int	len = 0;
	char	temp[200];
	int	scrpType;
	int	index=0L;
	BOOL	aConst;
	BOOL	retVal = TRUE;
	char	constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	I_UNUSED(oiScrp);
	SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, 1, (LONG) &index);
	index = index + SCRP_PARAM_DBINDEX;	// skip over first 3 data blocks.
	if(argsBlks[index]) {
		srcBlkPtr = (SCRP_ARGUMENTS*)AMemLock(argsBlks[index]);
		strcpy((char *)&temp, (char *)getscrpTypeName(srcBlkPtr->paramType));
		len = strlen(temp);
		if (srcBlkPtr->constant) {				// If contant, then get the constant value.
			temp[len] = '\x09';					// tab to object field.
			len++;
			ltoa(srcBlkPtr->constVal,&temp[len],10);		// convert constant to text, for display.
			len = strlen(temp);
		}
		else {		// not a constant, associate selected ALM with argument.
			if (srcBlkPtr->theArgObject) {				// get the name of the ALM.
				temp[len] = '\x09';					// tab to object field.
				len++;
            TYPEID objType;
				if (srcBlkPtr->theArgObject) {
					objType = AObjGetType(srcBlkPtr->theArgObject);
					ATypeGetName(objType, (LPSTR)&temp[len], SCRPNAMESIZE);
//					ltoa((long)objType,&temp[len],10);		// convert type to text, for display.
				}
				else
					strcpy(&temp[len], "<Undefined>");			// no ALM name...

				len = strlen((char*)&temp);
				temp[len] = '\x09';					// tab to object field.
				len++;
				AObjGetName(srcBlkPtr->theArgObject, (LPSTR)&temp[len], SCRPNAMESIZE);
				len = strlen(temp);
			}
		}			// if (!aConst)... else...
		index--;
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_INSERTSTRING, index-2, (LONG)&temp);	// add to argument list.
		index++;
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_DELETESTRING, index-2, 0);
		AMemUnlock(argsBlks[index]);
	}			// if(argsBlks[index])...
	return retVal;
}			// updateArg(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  insertArg
// DESCRIPTION:  Insert scrp function argument.
/////////////////////////////////////////////////////////////////////////////

BOOL insertArg( HWND hDlg, OBJECTID oiScrp)
{
	int			len = 0;
	char			temp[SCRPNAMESIZE];
	int			scrpType, index=0L;
	int			i, offset;
	BOOL			aConst;
	BOOL			retVal = TRUE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	I_UNUSED(oiScrp);
	temp[0] = '\0';			// initialize it...
	SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, 1, (LONG) &index);
	if (index == 0)
		index = nmbrOfArgs + SCRP_PARAM_DBINDEX;		// no selection.
	else {
		index = index + SCRP_PARAM_DBINDEX;	// skip over first 3 data blocks.
		offset = (int)(nmbrOfArgs + SCRP_PARAM_DBINDEX - index);
		for (i=0; i<offset; ++i)
			argsBlks[(int)(nmbrOfArgs-i+SCRP_PARAM_DBINDEX)] = argsBlks[(int)(nmbrOfArgs-i+2)];	// move data blocks up.
	}
	argsBlks[index] = 0;

	argsBlks[index] = AMemAllocate(argBlkLen);		// insert new
	if(argsBlks[index]) {
		srcBlkPtr = (SCRP_ARGUMENTS*)AMemLock(argsBlks[index]);
		srcBlkPtr->constant = FALSE;
		srcBlkPtr->theArgObject = 0L;
		srcBlkPtr->paramType = -1;
		srcBlkPtr->constVal = 0;
		srcBlkPtr->byValue = FALSE;

		strcpy(&temp[0], "<Undefined>");			// initialize it...
		len = strlen(temp);
		temp[len] = '\x09';					// tab to object field.
		len++;
		strcpy(&temp[len], "<No ALM>");
		len = strlen(temp);

		temp[len] = '\x09';					// tab to object field.
		len++;
		strcpy(&temp[len], "<Undefined>");


		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_INSERTSTRING, index-SCRP_PARAM_DBINDEX, (LONG)&temp);	// add to argument list.
		nmbrOfArgs++;							// increment the number of args.
		AMemUnlock(argsBlks[index]);
	}			// if(argsBlks[index])...
	else
		retVal = FALSE;			// couldn't allocate temp memory for editing.
	return retVal;
}			// insertArg(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  deleteArg
// DESCRIPTION:  delete selected argument from scrp function argument list.
/////////////////////////////////////////////////////////////////////////////

BOOL deleteArg( HWND hDlg, OBJECTID oiScrp)
{
	int	selNumOfArgs;
	int		selectBuff[MAX_NUM_OFARGS];
	int		i, j;

	I_UNUSED(oiScrp);
	selNumOfArgs = (int)SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELCOUNT, 0, 0);
	if (selNumOfArgs > 0) {
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, selNumOfArgs, (LONG) &selectBuff);
		for (i = 0; i < selNumOfArgs; i++) {
			SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_DELETESTRING, selectBuff[i]-i, 0);
			AMemFree(argsBlks[selectBuff[i]+SCRP_PARAM_DBINDEX]);			// free up mem block id.
			argsBlks[SCRP_PARAM_DBINDEX+selectBuff[i]] = 0;
		}			// for (i = selNumOfArgs-1...
		for (i=0; i < nmbrOfArgs+SCRP_PARAM_DBINDEX; ++i) {		// delete arguments.
			if (!argsBlks[i])
				for (j=i; j < nmbrOfArgs+SCRP_PARAM_DBINDEX; ++j)
					if (argsBlks[j]) {
						argsBlks[i] = argsBlks[j];
						argsBlks[j] = 0;
						break;
			 }
		}			// for (i=0; i<nmbrOfArgs; ++i)...
		nmbrOfArgs = nmbrOfArgs - selNumOfArgs;
		EnableWindow(GetDlgItem(hDlg, IDD_DELETE), FALSE);	// nothing to delete.
	}				// if (selNumOfArgs > 1)...
	return TRUE ;
}			// deleteArg(...

/*
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  setUnsetConst
// DESCRIPTION: The constant check box have been selected or desected.  Set
//							the correct setting for the dialog control items.
/////////////////////////////////////////////////////////////////////////////

BOOL setUnsetConst(HWND hDlg)
{
	if(SendDlgItemMessage(hDlg, IDD_OBJECTCONST, BM_GETCHECK, 0, 0)) {
//		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), FALSE);		// could update.
//		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), FALSE);	// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), FALSE);		// could update.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), FALSE);		// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), TRUE);		// could update.
//		CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, TRUE);						// byValue radio button.
//		CheckDlgButton(hDlg, IDD_OBJECTBYREF, FALSE);							// byRef radio button.
	}
	else {
//		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), TRUE);		// could update.
//		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), TRUE);	// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), TRUE);			// could update.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), TRUE);			// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), FALSE);		// could update.
	}
	return TRUE;
}			// setUnsetConst(...
*/

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  conVertConstToString
// DESCRIPTION: Convert a long double to a string value.
/////////////////////////////////////////////////////////////////////////////

void conVertConstToString(char* theStr, long double theValue)

{
	OBJECTID	theTextObjID = 0;

//	theTextObjID = AObjCreate(OTYPE_NUMBER);
//	ONmbrSetType(theTextObjID, ONMBR_REAL);
//	ONmbrSetReal(theTextObjID, theValue, TRUE);
	ONmbrFormatReal(theTextObjID, theValue, theStr, ONMBR_MAX_TEXT_LENGTH);

//	ONmbrConvertToString(theTextObjID, theStr, ONMBR_MAX_TEXT_LENGTH);
//	AObjDestroy(theTextObjID);
}			// conVertConstToString(...






////////////////////////////////////////////////////////////////////////////
// saveArgumentsBlks: update the data in the scrp object, Ok was pressed
//	in the scrp edit dialog.
///////////////////////////////////////////////////////////////////////////

BOOL saveArgumentsBlks(OBJECTID oiScrp)
{
	BOOL				retVal = TRUE;
	char				scrpName[SCRPNAMESIZE];
	char				scrpFunctName[FUNCNAMESIZE];
	OBJECTID			objID;
	int				i;
	long				oldDataCount;
	SCRP_FUNCNAME*	theScrpFuncNamePtr;
	char*				memBlkPtr;

	oldDataCount = AObjGetDataCount(oiScrp);
	for (i=0; i<oldDataCount; ++i)			// delete all data blocks.
		AObjDeleteData(oiScrp, 0L);
	oldDataCount = AObjGetDataCount(oiScrp);
	AObjSetDataCount(oiScrp, MIN_DATABLKS-1);

	memBlkPtr = (char*)AMemLock(argsBlks[SCRP_INFO_DBINDEX]);
	theScrpFuncNamePtr = (SCRP_FUNCNAME*)memBlkPtr;
	i = AMemGetSize(argsBlks[SCRP_INFO_DBINDEX]);
	AObjUnlockData(oiScrp,SCRP_INFO_DBINDEX);

	if(memBlkPtr != NULL) {
		if(!AObjSetData(oiScrp, SCRP_INFO_DBINDEX, theScrpFuncNamePtr, i))
			retVal = FALSE;
		AMemUnlock(argsBlks[SCRP_INFO_DBINDEX]);
		AMemFree(argsBlks[SCRP_INFO_DBINDEX]);
	}
	argsBlks[SCRP_INFO_DBINDEX] = NULL;

	memBlkPtr = (char*)AMemLock(argsBlks[ERROR_LOG_DBINDEX]);
	if(memBlkPtr != NULL) {
		if(!AObjSetData(oiScrp, ERROR_LOG_DBINDEX, memBlkPtr, AMemGetSize(argsBlks[ERROR_LOG_DBINDEX])))
			retVal = FALSE;
		AMemUnlock(argsBlks[ERROR_LOG_DBINDEX]);
		AMemFree(argsBlks[ERROR_LOG_DBINDEX]);
	}
	argsBlks[ERROR_LOG_DBINDEX] = NULL;

	long ecj = AMemGetSize(argsBlks[SCRP_TEXT_DBINDEX]);
	memBlkPtr = (char*)AMemLock(argsBlks[SCRP_TEXT_DBINDEX]);
	if(memBlkPtr != NULL) {
		if(!AObjSetData(oiScrp, SCRP_TEXT_DBINDEX, memBlkPtr, AMemGetSize(argsBlks[SCRP_TEXT_DBINDEX])))
			retVal = FALSE;
		AMemUnlock(argsBlks[SCRP_TEXT_DBINDEX]);
		AMemFree(argsBlks[SCRP_TEXT_DBINDEX]);
	}
	argsBlks[SCRP_TEXT_DBINDEX] = NULL;

	for (i=SCRP_PARAM_DBINDEX; i<nmbrOfArgs+SCRP_PARAM_DBINDEX; ++i) {
		memBlkPtr = (char*)AMemLock(argsBlks[i]);	// Save arguments, in data blocks.
		if(memBlkPtr != NULL) {
			if(!AObjAddData(oiScrp, memBlkPtr, AMemGetSize(argsBlks[i])))
				retVal = FALSE;
			AMemUnlock(argsBlks[i]);
			AMemFree(argsBlks[i]);
			argsBlks[i] = NULL;
		}
		else
			retVal = FALSE;			// not abled to access temp data block.
	}			// for (i=0; numberOfArgs...
	oldDataCount = AObjGetDataCount(oiScrp);
	return retVal;
}			// saveArgumentsBlks(...


////////////////////////////////////////////////////////////////////////////
// Edit_Script: Responsible for registering any callback functions.
//			Here we get the pcode, if any from,
//			from the object data blocks, we convert this pcode into text,
//			and save in a temp text file (BasTmp99.sbl), the basic editor is
//			invoked with the temp file.  During the edit session the user can
//			change the script.  Once the edit session is
//			terminated, the text file is converted into PCode, the pcode is
//			save in the object data blocks.
//				Inputs		remarks
//					HWND		handle to the parent dialog.
//					OBJECTID	the id for the basic object.
//
//				return value
//					(none)
//
///////////////////////////////////////////////////////////////////////////

void Edit_Script(HWND parentWin, OBJECTID oiScrp)

{
	MSG	msg;
	HWND	theEditWin;
	BOOL	notDone = TRUE;
	UINT	closeMsg;
	SBI_STATE	theSBIState;

	closeMsg = WM_CLOSE;

	if(SBIHandle == NULL) {
		pSBLInit = &SBLInit;
		SBIDefaults(pSBLInit, sizeof(SBLInit));
		SBLInit.hWndParent = parentWin;
		strcpy(SBLInit.szWndName, "AppWare Basic Script Language");
		SBIInit(pSBLInit, sizeof(SBLInit), &SBIHandle);	// Create SBI instance.
	}			// if(SBIHandle != NULL...
	SBLHandle = SBIhSbl(SBIHandle);
	if (SBLHandle != NULL){
		if (cnvrt_to_file(oiScrp, FALSE)) {
			SBIEdit(SBIHandle, "ecjtest0.sbl");
//				SBIEdit(SBIHandle, scrpTempFile);
// check to see if file was modified.
		}		// 	if (cnvrt_to_file(...
	}		// if (SBLHandle != NULL...
}		// Edit_Script(...


////////////////////////////////////////////////////////////////////////////
// Compile_Scripts: Call when user compiles the AppWare project.  We are
//			call to compile a script.  If the compiler ends with errors, the
//			user is taken into the editor to fix the errors.
//
//				Inputs			remarks
//					OBJECTID		the id for the basic object.
//
//				return value
//					SCRPERR		script did not compiled.
//					SCRPNOERR	script compiled ok.
//
///////////////////////////////////////////////////////////////////////////

LONG Compile_Scripts()

{
//	int	 			theIndex = 0;			// select ist item in list.
	TYPEID			theType;
	long				theObjGetKey;
	char				temp[SCRPNAMESIZE];
	BOOL				retVal = TRUE;
	OBJECTID oiScrp;

	theObjGetKey = (long)AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
	while ((oiScrp = AObjGetNextObject((TYPEID)scrpTypeID, (LPLONG) &theObjGetKey)) != 0 ) {
		Compile_Script(oiScrp);		// compile it.
	}					// while (theOBJ !=0)...
	return retVal ;

}			// Compile_Scripts(...


////////////////////////////////////////////////////////////////////////////
// Compile_Script: Call when user compiles the AppWare project.  We are
//			call to compile a script.  If the compiler ends with errors, the
//			user is taken into the editor to fix the errors.
//
//				Inputs			remarks
//					OBJECTID		the id for the basic object.
//
//				return value
//					SCRPERR		script did not compiled.
//					SCRPNOERR	script compiled ok.
//
///////////////////////////////////////////////////////////////////////////

LONG Compile_Script(OBJECTID oiScrp)

{
	MSG msg;
	LONG retVal = A_OK;
	SCRP_FUNCNAME*			scrpFuncName;		// scrp PTR to data0.
	SBL_STATUS				status = 0;

	compilingObject = oiScrp;
//	pSBLInit = &SBLInit;
//	SBIDefaults(pSBLInit, sizeof(SBLInit));
//	SBLInit.hWndParent = GetActiveWindow();
//	strcpy(SBLInit.szWndName, "AppWare Basic Script Language");



	if(SBIHandle == NULL) {
		pSBLInit = &SBLInit;
		SBIDefaults(pSBLInit, sizeof(SBLInit));
		SBLInit.hWndParent = GetActiveWindow();
		strcpy(SBLInit.szWndName, "AppWare Basic Script Language");
		SBIInit(pSBLInit, sizeof(SBLInit), &SBIHandle);	// Create SBI instance.
		SBLHandle = SBIhSbl(SBIHandle);
		registerAllGlobals();
	}			// if(SBIHandle != NULL...
//		registerAllGlobals();
	scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(compilingObject, SCRP_INFO_DBINDEX);
	if (scrpFuncName->function)
//	if (!SBIInit(pSBLInit, sizeof(SBLInit), &SBIHandle)){	// Create SBI instance.
//		SBLHandle = SBIhSbl(SBIHandle);
		if (SBLHandle != NULL){
			if (cnvrt_to_file(oiScrp, TRUE)){
				scrpFuncName->numOfErrorsInLog = 0;
				AObjResizeData(compilingObject, ERROR_LOG_DBINDEX, 0);
						// get the name of the script alm, will be use to run function.
				AObjGetName(compilingObject, &scrpFuncName->scrpFunction[0], SBL_CCH_MAX_NAME);

				AObjUnlockData(compilingObject, SCRP_INFO_DBINDEX);
				status = SblCompile(SBLHandle, "ecjtest0.sbl", NULL, pErrorProc, pImplicitProc,
								"-ZA-W1", pCallbackProc);
				if (status != 0)
					retVal = A_ERROR;
				else
					retVal = A_OK;
				unlink ("ecjtest0.sbl");
			}			// if(cnvrt_to_file(...
		}		// if (SBLHandle != NULL...
//		SBITerm(SBIHandle);
//		SBIHandle = NULL;
//		SBLHandle = NULL;
//	}		// if (SBIInit(...
	return retVal;
}			// Compile_Script(...


////////////////////////////////////////////////////////////////////////////
// registerAllGlobals: Will register all global variables and structures .
//
//				Inputs			remarks
//
//				return value
//
///////////////////////////////////////////////////////////////////////////

VOID registerAllGlobals()
{
	SBL_DATATYPE		dataType;
	SBL_STATUS 			status;
	SCRP_FUNCNAME*		scrpFuncName;		// scrp PTR to data0.
	long					theObjGetKey;
//	OBJECTID		theObjectID;
	BOOL					retVal;

	theObjGetKey = (long)AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
	while ((theObjectID = AObjGetNextObject((TYPEID)scrpTypeID, (LPLONG) &theObjGetKey)) != 0 ) {
		scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(theObjectID, SCRP_INFO_DBINDEX);
		if (scrpFuncName->variables)
			retVal = setGlobalVariables();
		if (scrpFuncName->structure) {
			setStructureMembers();			// go setup the elememnts of the structure.
			typeMemberInfoPtr = (SBL_LPMEMBER_INFO)AMemLock(typeMemberInfoBlkID);
			dataType = SblRegisterType(SBLHandle, &scrpFuncName->scrpFunction[0], numberOfMembers, typeMemberInfoPtr);
			if (SBL_DT_TYPE_OF(dataType) == SBL_DATATYPE_ILLEGAL) {
				int defErr;
				defErr = SBL_DT_INDEX_OF(dataType);
			}
			else {
				status = SblRegisterVar(SBLHandle, &scrpFuncName->scrpFunction[0], dataType, SBL_SCOPE_GLOBAL);
			}
			AObjUnlockData(theObjectID, SCRP_INFO_DBINDEX);
		}			// if (scrpFuncName->structure)...
	}					// while (theOBJ !=0)...
}			// registerAllGlobals(...


////////////////////////////////////////////////////////////////////////////
// cnvrt_to_file: Will conver the pcode in the object's datablocks to
//			text, it will saveit in the script temp file (scrpTempFile).
//
//				Inputs			remarks
//					OBJECTID		the id for the basic object.
//					BOOL			TRUE - if compiling a script.
//
//				return value
//					SCRPERR		could no convert.
//					SCRPNOERR	script convert it.
//
///////////////////////////////////////////////////////////////////////////

LONG cnvrt_to_file(OBJECTID oiScrp, BOOL compiling)

{
#define			maxCharsToWrite 65535		// max chars to be write out.

	LONG					retVal = TRUE;			// return value
	WORD					scriptBuffSize=0;		// len of text to write out.
	SBL_LPBYTE			scriptTextPtr;			// ptr to the script text.
	SBL_HBFILE			fileHandle;				// handle to sbl temp file.
	long					fileSize;				// size of script file.

	if (compiling)
		fileSize = AObjGetDataSize(oiScrp, SCRP_TEXT_DBINDEX);
	else
		fileSize = AMemGetSize(argsBlks[SCRP_TEXT_DBINDEX]);
	if (fileSize > maxCharsToWrite)
		scriptBuffSize = maxCharsToWrite;
	else
		scriptBuffSize = fileSize;
//	fileHandle = SblFSCreatFile(SBLHandle, "ecjtest0.sbl",OF_READWRITE , fileSize);
	fileHandle = SblFSCreatFile(SBLHandle, "ecjtest0.sbl",0 , fileSize);
	if (fileHandle != SBL_E_OPEN_FILE) {
		if (compiling)
			scriptTextPtr = (SBL_LPBYTE)AObjLockData(oiScrp, SCRP_TEXT_DBINDEX);
		else
			scriptTextPtr = (SBL_LPBYTE)AMemLock(argsBlks[SCRP_TEXT_DBINDEX]);
		while (fileSize > 0) {
			scriptBuffSize = SblFSWriteFile(fileHandle, scriptTextPtr, scriptBuffSize);
			fileSize = fileSize - scriptBuffSize;	// remainder to be read in.
			if (fileSize > maxCharsToWrite)
				scriptBuffSize = maxCharsToWrite;
			else
				scriptBuffSize = fileSize;
			scriptTextPtr = (SBL_LPBYTE)((long)scriptTextPtr + scriptBuffSize);
		}			// while (fileSize >= scriptBuffSize)...

		if (compiling)
			AObjUnlockData(oiScrp, SCRP_TEXT_DBINDEX);
		else
			AMemUnlock(argsBlks[SCRP_TEXT_DBINDEX]);
		if (SblFSCloseFile(fileHandle))
			retVal = FALSE;
	}			// if (fileHandle != SBL_E_OPEN_FILE...
	else
		retVal = FALSE;

	return retVal;
}			// cnvrt_to_file(...


////////////////////////////////////////////////////////////////////////////
// cnvrt_from_file: Will conver from the scrpTempFile to pcode and will
//			save it in the object's datablocks.  First we delete all data
//			blocks, if any and then we save the ffile in new datablocks.
//
//				Inputs		remarks
//					OBJECTID		the id for the basic object.
//
//				return value
//					SCRPERR		could no convert.
//					SCRPNOERR	script convert it.
//
///////////////////////////////////////////////////////////////////////////

LONG cnvrt_from_file(OBJECTID oiScrp)

{
#define			maxCharsReadIn 65535			// max chars to be read in.

	WORD					scriptBuffSize=0;				// len of text in data block.
	SBL_LPBYTE			scriptTextPtr;			// ptr to the script text.
	SBL_HBFILE			fileHandle;				// handle to sbl temp file.
	LONG					retVal = TRUE;			// return value.
	long					fileSize;				// size of script file.
	SBL_WORD				bytesToRead;			// number of bytes to read in.

	HFILE tmpFileH = open("ecjtest0.sbl", OF_READ);
	if (tmpFileH != -1)     {
		fileSize = filelength(tmpFileH);		// get size of temp file.
		close(tmpFileH);
		fileHandle = SblFSOpenFile(SBLHandle, "ecjtest0.sbl",OF_READ);	// read-only.
		if (fileHandle != SBL_E_OPEN_FILE) {
			if (AMemSetSize(argsBlks[SCRP_TEXT_DBINDEX], fileSize)) {
				scriptTextPtr = (SBL_LPBYTE)AMemLock(argsBlks[SCRP_TEXT_DBINDEX]);
				bytesToRead = maxCharsReadIn;
				bytesToRead = 100;
				while (fileSize > 0) {
					scriptBuffSize = SblFSReadFile(fileHandle, scriptTextPtr, bytesToRead);
					fileSize = fileSize - scriptBuffSize;	// remainder to be read in.
					scriptTextPtr = (SBL_LPBYTE)((long)scriptTextPtr + scriptBuffSize);
				}			// while (fileSize >= scriptBuffSize)...
				AMemUnlock(argsBlks[SCRP_TEXT_DBINDEX]);
			}			// if (AObjResizeData(...
			else
				retVal = FALSE;
			if (SblFSCloseFile(fileHandle))
				retVal = FALSE;
			unlink ("ecjtest0.sbl");
		}			// if (fileHandle != SBL_E_OPEN_FILE...
		else
			retVal = FALSE;
	}			// if(tmpFile...
	else
		retVal = FALSE;
	return retVal;
}			// cnvrt_from_file(...


////////////////////////////////////////////////////////////////////////////
// Debug_Script_CB: Will get invoked when a debug session takes place.  The
//			debugger get initiated through user control.
//				Inputs		remarks
//
//				return value
//					(none)
//
///////////////////////////////////////////////////////////////////////////

void Debug_Script_CB()

{

}			// Debug_Script_CB(...


////////////////////////////////////////////////////////////////////////////
// Compiler_ErrorProc: Invoked when an error is reported by the compiler.
//
//				Inputs		remarks
//					severity,
//					err,
//					line,
//					pszFile,
//					pszErr
//
//				return value
//					(none)
//
///////////////////////////////////////////////////////////////////////////

SBL_STATUS DLLENTRY Compiler_ErrorProc(
	SBL_WORD severity,
	SBL_STATUS err,
	SBL_SHORT line,
	SBL_LPSTR pszFile,
	SBL_LPSTR pszErr)

{
	COMPILER_ERROR_LOG*	errorLogPtr;		// ptr to error log DB.
	COMPILER_ERROR_LOG	errorToLog;
	SCRP_FUNCNAME*			scrpFuncName;		// scrp PTR to data0.
	LONG						newDBSize;
	SBL_STATUS				retVal = 0;			// default: continue comilation.
	int						index;

	scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(compilingObject, SCRP_INFO_DBINDEX);
	index = scrpFuncName->numOfErrorsInLog;
	if (index < MAX_ERRORS) {
		newDBSize = AObjGetDataSize(compilingObject, ERROR_LOG_DBINDEX)+sizeof(COMPILER_ERROR_LOG);
		if (AObjResizeData(compilingObject, ERROR_LOG_DBINDEX, newDBSize)) {
			(scrpFuncName->numOfErrorsInLog)++;		// increment error count count
			errorLogPtr = (COMPILER_ERROR_LOG*)AObjLockData(compilingObject, ERROR_LOG_DBINDEX);
			errorLogPtr = (COMPILER_ERROR_LOG*)((long)errorLogPtr + (index * sizeof(COMPILER_ERROR_LOG)));
			errorLogPtr->severity = severity;
			errorLogPtr->err = err;
			errorLogPtr->lineNumber = line;
			AObjUnlockData(compilingObject, ERROR_LOG_DBINDEX);
			if (index == MAX_ERRORS-1)
				retVal = 1;			// max number of errors.
		};			// if (AObjResizeData(...
	}			// if (index < MAX_ERRORS)...
	else
		retVal = 1;
	AObjUnlockData(compilingObject, SCRP_INFO_DBINDEX);
	return retVal;
}			// Compiler_ErrorProc(...


////////////////////////////////////////////////////////////////////////////
// Compiler_ImplicitProc: Is invoked when the compiler detects a referance
//			to an undefined procedure, possibly in another module.
//
//				Inputs				remarks
//					pszProcName		name of unknown procedure.
//
//				return value
//					retVal			will return one of the following:
//										SBL_COMPILER_OK, SBL_COMPILER_WARNING or
//										SBL_COMPILER_ERROR.
//
///////////////////////////////////////////////////////////////////////////

SBL_SHORT DLLENTRY Compiler_ImplicitProc(SBL_LPSTR pszProcName)

{
	SBL_SHORT	retVal;

	return retVal;
}			// Compiler_ImplicitProc(...


////////////////////////////////////////////////////////////////////////////
// Compiler_RegVarsProc: Is invoked to register script variables for the
//			compiler.  Called at the beginning of the compile.
//
//				Inputs				remarks
//					pszProcName		name of unknown procedure.
//
//				return value
//					(none)
//
///////////////////////////////////////////////////////////////////////////

void DLLENTRY Compiler_RegVarsProc()

{

}			// Compiler_RegVarsProc(...






