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
 *	"dll_adlg.cpp: In here we will handle the argument dialogs for 
 *	the DLL object. There are three dialogs, the DLL edit dialog,
 *	the Function Argument dialog and the Return Value dialog.
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

#include <a_alm.h>
#include <a_almutl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

#include	<a_almutl.h>
#include	<op_wnd.h>
#include	<o_wnd.h>
#include	<o_number.h>
#include	<o_text.h>
#include	<intdebug.h>

#include "dll.h"


extern HINSTANCE	hInstance;
extern AMEMBLOCKID	objBlks[MAXNUMOFARGS];	// tmp ptrs to objs data blocks.
extern LONG				numberOfArgs;		// Arguments in function call + 2.


/*******************************************************************
 *	Local function prototypes
 ******************************************************************/

BOOL argLstSelection(HWND hDlg, LONG lParam);
BOOL deleteArg ( HWND hDlg, AOBJECTID oiDLL);
BOOL insertArg ( HWND hDlg, AOBJECTID oiDLL);
BOOL updateArg ( HWND hDlg, AOBJECTID oiDLL);
BOOL initArgumentDlg ( HWND hDlg, AOBJECTID oiDLL);
BOOL initArgList(HWND hDlg);
LPSTR getCTypeName(int cType);
BOOL selectObjType ( HWND hDlg, LONG lParam);
BOOL selectParamType ( HWND hDlg, LONG lParam);
BOOL saveArgumentsBlks();
BOOL setUnsetConst(HWND hDlg);
BOOL setupArgTypeObjList(HWND hDlg);
BOOL setupArgObjNameList ( HWND hDlg, int CType);
void conVertConstToString(char* , long double);

AMEMBLOCKID			argsBlks[MAXNUMOFARGS];	// temp ptr array for parameter blocks.
LONG							nmbrOfArgs;		// number of parameters defined.
DLL_ARGUMENTS*	srcBlkPtr;
DLL_ARGUMENTS*	destBlkPtr;
int							argBlkLen;


////////////////////////////////////////////////////////////////////////////
// FUNCTION:  EditArgumentDialogProc
// DESCRIPTION: handle the argument dialog.
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  EditArgumentDialogProc(HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
{
	BOOL	retVal = FALSE;

	switch (message) {
		case WM_INITDIALOG:
			retVal = (initArgumentDlg(hDlg, (AOBJECTID)lParam));
			break;

		case WM_COMMAND: //!!PORT!!
			switch (wParam) {
				case IDD_ARGUMENTLST:	// list of function arguments.
					retVal = argLstSelection(hDlg, (long) lParam);
					break;

				case IDD_INSERT:			// insert argument in list.
					retVal = (insertArg(hDlg, (AOBJECTID)lParam));
					break;

				case IDD_UPDATE:			// update, modify argument in list.
					retVal = (updateArg(hDlg, (AOBJECTID)lParam));
					break;

				case IDD_DELETE:		  // delete argument argument from list.
					retVal = (deleteArg(hDlg, (AOBJECTID)lParam));
					break;

				case IDD_PARAMTYPE:		// C parameter type.
					retVal = (selectParamType(hDlg, (long)lParam));
					break;

				case IDD_OBJECTTYPE:		// ALM object types list.
					retVal = (selectObjType(hDlg, (long)lParam));
					break;

				case IDD_OBJECTNAME:		// ALM object names list.
					retVal = TRUE;
					break;

				case IDD_OBJECTCONST:	// parameter is a constant.
					retVal = setUnsetConst(hDlg);
					break;

				case IDD_OBJECTVALUE:	// value for constant parameter.
					retVal = TRUE;
					break;

				case IDOK:
					retVal = saveArgumentsBlks();		// save new function arguments.
					EndDialog (hDlg, FALSE);
					break;

				case IDCANCEL:
					EndDialog (hDlg, FALSE);
					retVal = TRUE;
					break;
			}				// switch (wParam)...
	}				// switch (message)...
	 return retVal ;
}			// EditArgumentDialogProc(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  argLstSelection
// DESCRIPTION: An argument have been selected update the C type and the ALM
//		object type and name, pass value by ref or value radio buttons.
/////////////////////////////////////////////////////////////////////////////

BOOL argLstSelection(HWND hDlg, LONG lParam)
{
	long			selNumOfArgs, indexOBJ;
	int				selectBuff[1];
	int				i;
	char			objectName[DLLNAMESIZE];
	BOOL			done;
	BOOL			retVal = FALSE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	switch (HIWORD (lParam)) {
		case CBN_SELCHANGE:			// setup the object name lists.
			selNumOfArgs = SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELCOUNT, 0, 0);
			if (selNumOfArgs == 1) {
				SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, (int)selNumOfArgs, (LONG) &selectBuff);
				i = selectBuff[0];
				srcBlkPtr = (DLL_ARGUMENTS*)AMemLock(argsBlks[i]);	// current argument.
				if (srcBlkPtr->constant) {
					CheckDlgButton(hDlg, IDD_OBJECTCONST, TRUE);	// constant.
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), FALSE);		// could update.
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), FALSE);	// could delete.
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), TRUE);		// could update.
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), FALSE);		// could update.
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), FALSE);		// could delete.
					SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_SETCURSEL, -1, 0L);
					SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_SETCURSEL, -1, 0L);
					conVertConstToString(&(constValStr[0]), srcBlkPtr->constVal);
					SetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &(constValStr[0]));	// enable and set the value.
				}
				else {
					if (srcBlkPtr->paramType == CTYPE_CHRFARPTR) {			// text can only be passed by referance.
						EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), FALSE);		// disable it.
						EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), FALSE);	// disable it.
						CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, FALSE);	// byValue radio button.
						CheckDlgButton(hDlg, IDD_OBJECTBYREF, TRUE);		// byRef radio button.
					}
          else {
						EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), TRUE);
						EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), TRUE);
					}
					CheckDlgButton(hDlg, IDD_OBJECTCONST, FALSE);							// disable the constant value input.
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), TRUE);
					SetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) "");
					EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), FALSE);
				}
				if (srcBlkPtr->byValue) {
					CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, TRUE);	// byValue radio button.
					CheckDlgButton(hDlg, IDD_OBJECTBYREF, FALSE);		// byRef radio button.
				}
				else {
					CheckDlgButton(hDlg, IDD_OBJECTBYREF, TRUE);		// byRef radio button.
					CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, FALSE);	// byValue radio button.
				}
				SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_SETCURSEL, srcBlkPtr->paramType, 0L);
				retVal = setupArgObjNameList(hDlg, srcBlkPtr->paramType);			// setup the alm type for selected C type.
				retVal = setupArgTypeObjList(hDlg);	// alm name list for selected alm type
				if (srcBlkPtr->theArgObject)
					AObjGetName(srcBlkPtr->theArgObject, &objectName[0], DLLNAMESIZE);	// select the object id.
				done = FALSE;
        indexOBJ = -1;
				while(!done) {
					indexOBJ = SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_SELECTSTRING, (int)indexOBJ, (long)&objectName[0]);
					if ((AOBJECTID)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_GETITEMDATA, (int)indexOBJ, 0L ) == srcBlkPtr->theArgObject ) {
						SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_SETCURSEL, (int)indexOBJ, 0L);
						done = TRUE;			// we found it.
					}
					if (indexOBJ == CB_ERR)
          	done = TRUE;			// we didn't find it.
				}			// while(!done)...
				EnableWindow(GetDlgItem(hDlg, IDD_UPDATE), TRUE);	// could update.
				EnableWindow(GetDlgItem(hDlg, IDD_DELETE), TRUE);	// could delete.
			}				// if (index == 1)...

			else {			// none or multiple items are selected. 
				EnableWindow(GetDlgItem(hDlg, IDD_UPDATE), FALSE);	// don't allow to update.
				if (selNumOfArgs == 0)			// no items selected.
					EnableWindow(GetDlgItem(hDlg, IDD_DELETE), FALSE);	// nothing to delete.
				else			// multiple items are selected.
					EnableWindow(GetDlgItem(hDlg, IDD_DELETE), TRUE);		// multiple deletion.
				SetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) "");
				SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_SETCURSEL, -1, 0L);
				SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_SETCURSEL, -1, 0L);
				SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_SETCURSEL, -1, 0L);
				CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, FALSE);	// byValue radio button.
				CheckDlgButton(hDlg, IDD_OBJECTBYREF, FALSE);		// byRef radio button.
				CheckDlgButton(hDlg, IDD_OBJECTCONST, FALSE);		// constant.
			}				// if (selNumOfArgs > 1)...
			retVal = TRUE;
	}
	return retVal ;
}			// argLstSelection(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  initArgumentDlg
// DESCRIPTION:  Initialized dll function argument dialog. Save current
//			  arguments into a temp area, for editing.
//
/////////////////////////////////////////////////////////////////////////////

BOOL initArgumentDlg(HWND hDlg, AOBJECTID oiDLL)
{
	int			i;
	int			theTab[1];
	BOOL		retVal;

  I_UNUSED(oiDLL);

	AUtlCenterDialog(hDlg, 0);

	EnableWindow(GetDlgItem(hDlg, IDD_INSERT), TRUE);
	for (i=0; i<NUMOFTYPES; i++)
	SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_ADDSTRING, 0, (long)getCTypeName(i));
	theTab[0] = 56;
	SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_SETTABSTOPS, 1, (LONG) &theTab[0]);
	EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), FALSE);	// could update.
	EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), FALSE);	// alm type.
	EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), FALSE);	// alm name.
	CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, TRUE);

			// copy parameter blocks to local edit area.
	nmbrOfArgs = numberOfArgs-2;			// skip DATA 0, DATA 1.
	argBlkLen = sizeof(DLL_ARGUMENTS);
	for (i=0; i<nmbrOfArgs; ++i) {		// copy data blocks to temp area.
		argsBlks[i] = AMemAllocate(argBlkLen);
		if(argsBlks[i]) {
			srcBlkPtr = (DLL_ARGUMENTS*)AMemLock(objBlks[(int)(i+DLLPARAMINDEX)]);	// current argument.
			destBlkPtr = (DLL_ARGUMENTS*)AMemLock(argsBlks[i]);	// local edit area.
			CopyMemory(destBlkPtr, srcBlkPtr, argBlkLen);
			AMemUnlock(argsBlks[i]);
			AMemUnlock(objBlks[(int)(i+DLLPARAMINDEX)]);
		}
		else
			retVal = FALSE;			// not abled to allocate temp memory for editing.
	}			// for (i=0; i<nmbrOfArgs; ++i)...
	retVal = initArgList(hDlg);
	return retVal;
}			// initArgumentDlg(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  initArgList
// DESCRIPTION: Init the argument list.
/////////////////////////////////////////////////////////////////////////////

BOOL initArgList(HWND hDlg)
{
	int				len = 0;
	char			temp[DLLNAMESIZE];
	char			objName[DLLNAMESIZE];
	int				CType;
	int				i;
	BOOL			aConst;
  BOOL			retVal = TRUE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	for (i=0; i<nmbrOfArgs; ++i) {
		if(argsBlks[i]) {
			srcBlkPtr = (DLL_ARGUMENTS*)AMemLock(argsBlks[i]);
			CType = srcBlkPtr->paramType;				// get CType.
			if (CType >= 0) {
				lstrcpy((char*)&temp[0], (char*)getCTypeName(CType));
				len = lstrlen((char*)&temp);
				aConst = srcBlkPtr->constant;
				if (aConst) {						// is it a constant?
					temp[len] = '\x09';		// yes - tab to object field.
					len++;
					conVertConstToString(&(constValStr[0]), srcBlkPtr->constVal);
					lstrcpy(&temp[len], (LPSTR)&(constValStr[0]));
				}
				if (!aConst) {					// not a constant, add object name.
					temp[len] = '\x09';		// tab to object field.
					len++;
					if (srcBlkPtr->theArgObject)
						AObjGetName(srcBlkPtr->theArgObject, (LPSTR)&objName, DLLNAMESIZE);
					lstrcpy(&temp[len], &objName[0]);
					len = lstrlen(temp);
				}			// if (!aConst)...
			}			// if (CType >= 0)...
			else {				// must be an undefined arguments...
      	lstrcpy(&temp[0], "<Undefined>");			// initialize it...
				len = lstrlen(temp);
				temp[len] = '\x09';					// tab to object field.
				len++;
				aConst = (BOOL)SendDlgItemMessage(hDlg, IDD_OBJECTCONST, BM_GETCHECK, 0, 0);
				srcBlkPtr->constant = aConst;
				if (aConst) {				// is it a constant?
					GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &(constValStr[0]), ONMBR_MAX_TEXT_LENGTH);
					srcBlkPtr->constVal = _strtold(&constValStr[0], NULL);
					GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &temp[len], ONMBR_MAX_TEXT_LENGTH);
					srcBlkPtr->byValue = IsDlgButtonChecked(hDlg, IDD_OBJECTBYVALUE);
				}
				else
					lstrcpy(&temp[len], "<No ALM>");
    	}			// else {...  undefined argument

				// add it to the argument list in the dialog.
			SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_INSERTSTRING, i, (LONG)&temp);
			AMemUnlock(argsBlks[i]);
		}			// if(argsBlks[index])...
	}			// for (i=0; i<nmbrOfArgs; ++i)...
	return retVal;
}			// initArgList(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  getCTypeName
// DESCRIPTION: Return a string to identify the C type.
/////////////////////////////////////////////////////////////////////////////

LPSTR getCTypeName(int cType)
{
	LPSTR	theNamePtr;

	switch (cType) {
		case CTYPE_SHORT:
				theNamePtr = "short";
			break;

		case CTYPE_USHORT:
				theNamePtr = "unsigned short";
			break;

		case CTYPE_LONG:
				theNamePtr = "long";
			break;

		case CTYPE_ULONG:
				theNamePtr = "unsigned long";
			break;

		case CTYPE_FLOAT:
				theNamePtr = "float";
			break;

		case CTYPE_DOUBLE:
				theNamePtr = "double";
			break;

		case CTYPE_LDOUBLE:
				theNamePtr = "long double";
			break;

		case CTYPE_CHRFARPTR:
				theNamePtr = "char *";
			break;

		default:
				theNamePtr = "shit";
			break;
	}			// switch (cType)...
	return theNamePtr;
}			// getCTypeName(...

 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  selectParamType
// DESCRIPTION: C type paramater selected in combo box.
/////////////////////////////////////////////////////////////////////////////

BOOL selectParamType(HWND hDlg, long lParam)
{
	int			CType;
	BOOL		retVal = TRUE;

	switch (HIWORD (lParam)) {
		case CBN_SELCHANGE:			// setup the object name lists.
			EnableWindow(GetDlgItem(hDlg, IDD_INSERT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), TRUE);	// enable alm type.
			EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), TRUE);	// enable alm name.
			CType = (int)SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_GETCURSEL, 0, 0);
			retVal = setupArgObjNameList(hDlg, CType);			// setup the alm type for selected C type.
			retVal = setupArgTypeObjList(hDlg);	// alm name list for selected alm type.
			if (CType == CTYPE_CHRFARPTR) {			// text can only be passed by referance.
				EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), FALSE);		// could update.
				EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), FALSE);	// could delete.
				CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, FALSE);						// byValue radio button.
				CheckDlgButton(hDlg, IDD_OBJECTBYREF, TRUE);							// byRef radio button.
				EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), FALSE);		// disable the constant input val.
				CheckDlgButton(hDlg, IDD_OBJECTCONST, FALSE);							// uncheck the constant check box.
			}
			else {
				EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), TRUE);
			}
			break;
	}
	return retVal;
}			// selectParamType()


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  selectObjType
// DESCRIPTION: Setup the object name list for the selected object type.
/////////////////////////////////////////////////////////////////////////////

BOOL selectObjType ( HWND hDlg, LONG lParam)
{
	BOOL			retVal = FALSE;

	switch (HIWORD (lParam)) {
		case CBN_SELCHANGE:			// setup the object name lists.
			EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), TRUE);	// alm name.
			setupArgTypeObjList(hDlg);	// alm name list for selected alm type
			retVal = TRUE;
      break;
	}
	return retVal ;
}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:	setupArgObjNameList
// DESCRIPTION:  Setup the list of ALM types available for selected C type.
/////////////////////////////////////////////////////////////////////////////

BOOL setupArgObjNameList(HWND hDlg, int CType)
{
	int	 				theIndex = 0;			// select ist item in list.
	ATYPEID			theType;
	long				theObjGetKey;
	char				temp[DLLNAMESIZE];
	BOOL				retVal = TRUE;

		// setup the object type lists.
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_RESETCONTENT, theIndex, 0L);
	theObjGetKey = (long)AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
	while ((theType = ATypeGetNext(&theObjGetKey)) != 0 ) {
		if (CType < CTYPE_CHRFARPTR) {
			if (theType == OTYPE_NUMBER){
				ATypeGetName(theType, (LPSTR)&temp, DLLNAMESIZE);
				theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_ADDSTRING, 0, (long)&temp);
				theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_SETITEMDATA, theIndex, (long)theType);
			}
		}
		else if (theType == OTYPE_TEXT) {
			ATypeGetName(theType, (LPSTR)&temp, DLLNAMESIZE);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_ADDSTRING, 0, (long)&temp);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_SETITEMDATA, theIndex, (long)theType);
     }
	}					// while (theOBJ !=0)...
  theIndex = 0;
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_SETCURSEL, theIndex, 0L);
	return retVal ;
}			// setupArgObjNameList(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION: setupArgTypeObjList
// DESCRIPTION:  Will setup a list of objects in the object names combo box
//				for the selected ALM type/name.
//
/////////////////////////////////////////////////////////////////////////////

BOOL setupArgTypeObjList(HWND hDlg)
{
	BOOL			retVal = TRUE;
	ATYPEID		almType;
	char			almName[DLLNAMESIZE];
	AOBJECTID	objID;
	long			theObjGetKey;
	char			objName[DLLNAMESIZE];
	int				theIndex, index;

	SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_RESETCONTENT, 0, 0);
	index = (int)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_GETCURSEL, 0, 0);
	almType = (ATYPEID)SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_GETITEMDATA, index, 0L);
	SendDlgItemMessage(hDlg, IDD_OBJECTTYPE, CB_GETLBTEXT, index, (long)&almName);
	if (index != CB_ERR){
		theObjGetKey = AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
		while ((objID = AObjGetNextObject(almType, (LPLONG) &theObjGetKey)) != 0 ) {
			if (objID)
				AObjGetName(objID, (LPSTR)&objName, DLLNAMESIZE);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_ADDSTRING, 0, (long)&objName);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_SETITEMDATA, theIndex, (long)objID);
		}					// while (objID !=0)...
	}			// if (index != CB_ERR...
	theIndex = 0;
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_SETCURSEL, theIndex, 0L);
	return retVal;
}			// setupArgTypeObjList(HWND hDlg...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  updateArg
// DESCRIPTION: Update a dll function argument.
/////////////////////////////////////////////////////////////////////////////

BOOL updateArg( HWND hDlg, AOBJECTID oiDLL)
{
	int				len = 0;
	char			temp[DLLNAMESIZE];
  int				CType;
	int				index=0L, index2=0L;
	BOOL			aConst;
  BOOL			retVal = TRUE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	I_UNUSED(oiDLL);
	SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, 1, (LONG) &index);
	if(argsBlks[index]) {
		srcBlkPtr = (DLL_ARGUMENTS*)AMemLock(argsBlks[index]);
		CType = (int)SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_GETCURSEL, 0, 0);
		srcBlkPtr->paramType = CType;				// save the CType.
		if (CType != CB_ERR)
			len = (int)SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_GETLBTEXT, CType, (LONG) &temp);

		aConst = (BOOL)SendDlgItemMessage(hDlg, IDD_OBJECTCONST, BM_GETCHECK, 0, 0);
		srcBlkPtr->constant = aConst;			// save constant bool value.
		if (aConst) {				// If contant, then get the constant value.
			temp[len] = '\x09';					// tab to object field.
			len++;
			GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &(constValStr[0]), ONMBR_MAX_TEXT_LENGTH);
			srcBlkPtr->constVal = _strtold(&constValStr[0], NULL);
			GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &temp[len], ONMBR_MAX_TEXT_LENGTH);
		}
		else {		// not a constant, associate selected ALM with argument.
			index2 = (int)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_GETCURSEL, 0, 0);
			if (index2 != CB_ERR){
				temp[len] = '\x09';					// tab to object field.
				len++;
				SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_GETLBTEXT, index2, (LONG) &temp[len]);
				len = lstrlen(temp);
				srcBlkPtr->theArgObject = (AOBJECTID)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_GETITEMDATA, index2, 0L );
				srcBlkPtr->byValue = IsDlgButtonChecked(hDlg, IDD_OBJECTBYVALUE);
			}
		}			// if (!aConst)... else...		
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_INSERTSTRING, index, (LONG)&temp);	// add to argument list.
		index++;
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_DELETESTRING, index, 0);
		AMemUnlock(argsBlks[index]);
	}			// if(argsBlks[index])...
	return retVal;
}			// updateArg(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  insertArg
// DESCRIPTION:  Insert dll function argument.
/////////////////////////////////////////////////////////////////////////////

BOOL insertArg( HWND hDlg, AOBJECTID oiDLL)
{
	int				len = 0;
	char			temp[DLLNAMESIZE];
	int				CType, index=0L, index2=0L;
	int				i, offset;
	BOOL			aConst;
  BOOL			retVal = TRUE;
	char			constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	I_UNUSED(oiDLL);  	
	temp[0] = '\0';			// initialize it...  	
	if (SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELCOUNT, 0L, 0L) > 0) {
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, 1, (LONG) &index);
		offset = (int)(nmbrOfArgs - index + 1);
		for (i=0; i<offset; ++i)
			argsBlks[(int)(nmbrOfArgs-i+1)] = argsBlks[(int)(nmbrOfArgs-i)];	// move data blocks up.
  }
	else
		index = (int)nmbrOfArgs;							// increment the number of args.

	argsBlks[index] = AMemAllocate(argBlkLen);		// insert new
	if(argsBlks[index]) {
		srcBlkPtr = (DLL_ARGUMENTS*)AMemLock(argsBlks[index]);
		CType = (int)SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_GETCURSEL, 0, 0);
		srcBlkPtr->paramType = CType;				// save the CType.

		if (CType != CB_ERR) {
			len = (int)SendDlgItemMessage(hDlg, IDD_PARAMTYPE, CB_GETLBTEXT, CType, (LONG) &temp);
			aConst = (BOOL)SendDlgItemMessage(hDlg, IDD_OBJECTCONST, BM_GETCHECK, 0, 0);
			srcBlkPtr->constant = aConst;
			if (aConst) {				// is it a constant?
				temp[len] = '\x09';					// tab to object field.
				len++;
				GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &(constValStr[0]), ONMBR_MAX_TEXT_LENGTH);
				srcBlkPtr->constVal = _strtold(&constValStr[0], NULL);
				GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &temp[len], ONMBR_MAX_TEXT_LENGTH);
				srcBlkPtr->byValue = IsDlgButtonChecked(hDlg, IDD_OBJECTBYVALUE);
				srcBlkPtr->theArgObject = NULL;
			}
			else {		// not a constant, associate selected ALM with argument.
				index2 = (int)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_GETCURSEL, 0, 0);
				if (index2 != CB_ERR){
					temp[len] = '\x09';					// tab to object field.
					len++;
					SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_GETLBTEXT, index2, (LONG) &temp[len]);
					len = lstrlen(temp);
					srcBlkPtr->theArgObject = (AOBJECTID)SendDlgItemMessage(hDlg, IDD_OBJECTNAME, CB_GETITEMDATA, index2, 0L );
					srcBlkPtr->byValue = IsDlgButtonChecked(hDlg, IDD_OBJECTBYVALUE);
				}
			}			// if (aConst)... else ...
		}			// if (CType != CB_ERR)...
		else {				// must be an undefined arguments...
			lstrcpy(&temp[0], "<Undefined>");			// initialize it...
			len = lstrlen(temp);
			temp[len] = '\x09';					// tab to object field.
			len++;
			aConst = (BOOL)SendDlgItemMessage(hDlg, IDD_OBJECTCONST, BM_GETCHECK, 0, 0);
			srcBlkPtr->constant = aConst;
			if (aConst) {				// is it a constant?
				GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &(constValStr[0]), ONMBR_MAX_TEXT_LENGTH);
				srcBlkPtr->constVal = _strtold(&constValStr[0], NULL);
				GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &temp[len], ONMBR_MAX_TEXT_LENGTH);
				srcBlkPtr->byValue = IsDlgButtonChecked(hDlg, IDD_OBJECTBYVALUE);
			}
			else
				lstrcpy(&temp[len], "<No ALM>");
		}			// else {...  undefined argument
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_INSERTSTRING, index, (LONG)&temp);	// add to argument list.
		nmbrOfArgs++;							// increment the number of args.
		AMemUnlock(argsBlks[index]);
	}			// if(argsBlks[index])...
	else
		retVal = FALSE;			// couldn't allocate temp memory for editing.
	return retVal;
}			// insertArg(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  deleteArg
// DESCRIPTION:  delete selected argument from dll function argument list.
/////////////////////////////////////////////////////////////////////////////

BOOL deleteArg( HWND hDlg, AOBJECTID oiDLL)
{
	int	selNumOfArgs;
	int		selectBuff[MAXNUMOFARGS];
	int		i, j;

  I_UNUSED(oiDLL);
	selNumOfArgs = (int)SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELCOUNT, 0, 0);
	if (selNumOfArgs > 0) {
		SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_GETSELITEMS, selNumOfArgs, (LONG) &selectBuff);
		for (i = 0; i < selNumOfArgs; i++) {
			SendDlgItemMessage(hDlg, IDD_ARGUMENTLST, LB_DELETESTRING, selectBuff[i]-i, 0);
			AMemFree(argsBlks[selectBuff[i]]);			// free up mem block id.
			argsBlks[selectBuff[i]] = 0;
		}			// for (i = selNumOfArgs-1...
		for (i=0; i < nmbrOfArgs; ++i) {		// delete arguments.
			if (!argsBlks[i])
				for (j=i; j < nmbrOfArgs; ++j)
					if (argsBlks[j]) {
						argsBlks[i] = argsBlks[j];
						argsBlks[j] = 0;
						break;
          }
		}			// for (i=0; i<nmbrOfArgs; ++i)...
		nmbrOfArgs = nmbrOfArgs - selNumOfArgs;
	}				// if (index = -1)...
	return TRUE ;
}			// deleteArg(...

 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  saveArgumentsBlks
// DESCRIPTION: here all we do is to copy the memblock id for the new
//			argument over the old one, if an old one existed then we free
//			the old memblock id.
/////////////////////////////////////////////////////////////////////////////

BOOL saveArgumentsBlks()
{
	BOOL	retVal = TRUE;
	int i;

	for (i=0; i<nmbrOfArgs; ++i) {		// copy new arguments.
  	if (objBlks[i+2])
			AMemFree(objBlks[i+2]);
		objBlks[i+2] = argsBlks[i];
	}			// for (i=0; i<nmbrOfArgs; ++i)...
	numberOfArgs = nmbrOfArgs + 2;
	return retVal;
}			// saveArgumentsBlks(...

 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  setUnsetConst
// DESCRIPTION: The constant check box have been selected or desected.  Set
//							the correct setting for the dialog control items.
/////////////////////////////////////////////////////////////////////////////

BOOL setUnsetConst(HWND hDlg)
{
	if(SendDlgItemMessage(hDlg, IDD_OBJECTCONST, BM_GETCHECK, 0, 0)) {
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), FALSE);		// could update.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), FALSE);	// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), FALSE);		// could update.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), FALSE);		// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), TRUE);		// could update.
		CheckDlgButton(hDlg, IDD_OBJECTBYVALUE, TRUE);						// byValue radio button.
		CheckDlgButton(hDlg, IDD_OBJECTBYREF, FALSE);							// byRef radio button.
	}
	else {
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYREF), TRUE);		// could update.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTBYVALUE), TRUE);	// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTTYPE), TRUE);			// could update.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTNAME), TRUE);			// could delete.
		EnableWindow(GetDlgItem(hDlg, IDD_OBJECTVALUE), FALSE);		// could update.
	}
	return TRUE;
}			// setUnsetConst(...

 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  conVertConstToString
// DESCRIPTION: Convert a long double to a string value.
/////////////////////////////////////////////////////////////////////////////

void conVertConstToString(char* theStr, long double theValue)

{
	AOBJECTID	theTextObjID = 0;

//	theTextObjID = AObjCreate(OTYPE_NUMBER);
//	ONmbrSetType(theTextObjID, ONMBR_REAL);
//	ONmbrSetReal(theTextObjID, theValue, TRUE);
	ONmbrFormatReal(theTextObjID, theValue, theStr, ONMBR_MAX_TEXT_LENGTH);

//	ONmbrConvertToString(theTextObjID, theStr, ONMBR_MAX_TEXT_LENGTH);
//	AObjDestroy(theTextObjID);
}			// conVertConstToString(...
