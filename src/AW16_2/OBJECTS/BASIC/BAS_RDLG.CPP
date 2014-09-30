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


extern		HINSTANCE	hInstance;
extern		MEMBLOCKID	argsBlks[MAX_NUM_OFARGS];	// temp ptrs for objs data blocks.
extern int	selectedParam;				// selected parameter in parameter list.
extern HWND	theArgumentDlg;


/*******************************************************************
 *	Local function prototypes
 ******************************************************************/

BOOL initRetArgDlg (HWND hDlg, LONG lParam);
BOOL handleRetParamType(HWND hDlg	, LONG lParam);
BOOL handleRetObjectType(HWND hDlg, LONG lParam);
BOOL handleRetObjectName(HWND hDlg, LONG lParam);
BOOL setupTypeObjList(HWND hDlg);
BOOL setupObjTypeList ( HWND hDlg, int CType);


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  EditReturnValDialogProc
// DESCRIPTION: handle the argument dialog.
////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export EditReturnValDialogProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
{
	BOOL 					retVal = FALSE;
	BOOL					aConst;
	SCRP_ARGUMENTS*	ptr;
	OBJECTID				selObjID;	// object id associated with the c param type.
	int					CType;		// index to c param type.
	int					index;
	char					objName[SCRPNAMESIZE];
	char					constValStr[ONMBR_MAX_TEXT_LENGTH];		// contain string  of long double.

	switch (message) {
		case WM_INITDIALOG:
			retVal = (initRetArgDlg(hDlg, lParam));
			break;

		case WM_COMMAND:
			switch (wParam) {
				case IDD_RETPARAMTYPE:
					retVal = handleRetParamType(hDlg, lParam);
					break;

				case IDD_RETOBJTYPE:
					retVal = handleRetObjectType(hDlg, lParam);
					break;

				case IDD_RETOBJNAME:
					retVal = handleRetObjectName(hDlg, lParam);
					break;

				case IDOK:
					AMemFree(argsBlks[(int)selectedParam+SCRP_PARAM_DBINDEX]);
					argsBlks[(int)selectedParam+SCRP_PARAM_DBINDEX] = AMemAllocate(sizeof(SCRP_ARGUMENTS));
					if (argsBlks[(int)selectedParam+SCRP_PARAM_DBINDEX]) {	// save changes into the edit area...
						ptr = (SCRP_ARGUMENTS*)AMemLock(argsBlks[(int)selectedParam+SCRP_PARAM_DBINDEX]);
						index = (int)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_GETCURSEL, 0, 0);
						selObjID = (OBJECTID)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_GETITEMDATA, index, 0L);
						SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_GETLBTEXT, index, (long)&objName);
						if (index != CB_ERR){
							ptr->theArgObject = selObjID;
							AObjGetName(selObjID, &ptr->theArgObjectName[0], SBL_CCH_MAX_NAME);
						}			// if (index != CB_ERR...

						CType = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_GETCURSEL, 0, 0);
						SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_GETLBTEXT, CType, (long)&objName);
						if (CType != CB_ERR){
							ptr->paramType = CType;
						}			// if (index != CB_ERR...

						aConst = (BOOL)SendDlgItemMessage(hDlg, IDD_OBJECTCONST, BM_GETCHECK, 0, 0);
						ptr->constant = aConst;			// save constant bool value.
						if (aConst) {				// If contant, then get the constant value.
							GetDlgItemText(hDlg, IDD_OBJECTVALUE, (LPSTR) &(constValStr[0]), ONMBR_MAX_TEXT_LENGTH);
							ptr->constVal = _strtold(&constValStr[0], NULL);
						}			// if (aConst)...

						AMemUnlock(argsBlks[(int)selectedParam+SCRP_PARAM_DBINDEX]);
//ecj						SendMessage(theArgumentDlg, IDD_UPDATE, 0,0);
					}			// if (argsBlks[selectedParam+SCRP_PARAM_DBINDEX])...
					else
						retVal = FALSE;	// not abled to allocate temp memory for editing.
					EndDialog (hDlg, FALSE) ;
					break;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					break;
				}
	}
	return retVal ;
}			// EditReturnValDialogProc (HWND hDlg,...

 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  initRetArgDlg
// DESCRIPTION:  Initialized scrp function argument dialog box.
/////////////////////////////////////////////////////////////////////////////

BOOL initRetArgDlg ( HWND hDlg, LONG lParam)
{
	int	 				theIndex;
	TYPEID				theType, selType = NULL;
	SCRP_ARGUMENTS*	ptr;
	int					CType;
	OBJECTID				selObjID;
	char					almName[SCRPNAMESIZE];
	char					objName[SCRPNAMESIZE];
	BOOL					done = FALSE;
	BOOL					retVal = TRUE;

	I_UNUSED(lParam);

	AUtlCenterDialog(hDlg, 0);

	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"Integer");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"long");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"Single");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"Double");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"Currency");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"Fixed String");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"Dynamic String");
//	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"char far*");

	ptr = (SCRP_ARGUMENTS*)AMemLock(argsBlks[(int)selectedParam+SCRP_PARAM_DBINDEX]);
	CType = ptr->paramType;
	selObjID = ptr->theArgObject;
	if (selObjID)
		selType = AObjGetType(selObjID);
	AMemUnlock(argsBlks[(int)selectedParam+SCRP_PARAM_DBINDEX]);

	if (CType == -1) {
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJTYPE), FALSE);	// disable alm type.
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJNAME), FALSE);	// disable alm name.
	}
	else {			// the argument have been set...
		SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_SETCURSEL, CType, 0L);
			// setup the object type lists.
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJTYPE), TRUE);	// alm type.
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJNAME), TRUE);	// alm name.
		setupObjTypeList(hDlg, CType);			// setup the alm type for selected C type.

			// edgar: need to verify that the ALM still exists, .
			// here we make the assumption that the ALM contains a name,
			// the return value is the same regardless, zero if the ALM
			// is not found or if it contains a name of zero length (blank).
		if (ATypeGetName(selType, (LPSTR)&almName, SCRPNAMESIZE) > 0)
			if (selObjID) {			  	// select the ALM and object name.
				theIndex = -1;
				done = FALSE;					// initialize it.
	   	 	while (!done) {
					theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_FINDSTRING, theIndex, (long)&almName);
					if (theIndex != CB_ERR) {
						theType = (TYPEID)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_GETITEMDATA, theIndex, 0L);
						if (theType == selType) {		// we found it, select ALM and obj.
							theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_SETCURSEL, theIndex, 0L);
							retVal = setupTypeObjList(hDlg);		// setup objs name CB list.
							AObjGetName(selObjID, (LPSTR)&objName, SCRPNAMESIZE);
							theIndex = -1;				// init for search.
							while (!done) {			// set the selected object.
								theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_FINDSTRING, theIndex, (long)&objName);
								if (theIndex != CB_ERR) {
									if(selObjID == (OBJECTID)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_GETITEMDATA, theIndex, 0L)){
											// select it.
										theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_SETCURSEL, theIndex, 0L);
										done = TRUE;
									}			// if(selObjID ==...
								}			// if (theIndex != CB_ERR)...
								else
    	  			  	done = TRUE;
							}			// while (!done)...
						}			// if (theType == selType)...
					}			// if (theIndex != CB_ERR)...
					else
        		done = TRUE;
				}			// while (!done)...
			}			// if (selObjID)...
	}			// else {  A return argument have been set...
	return retVal ;
}			// initRetArgDlg (...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  handleRetParamType
// DESCRIPTION:  Will handle the parameter C type.  When the user changes
//				the selection or selects a C type it will be saved or modified in
//				the temp area.
//
/////////////////////////////////////////////////////////////////////////////

BOOL handleRetParamType(HWND hDlg	, LONG lParam)
{
	BOOL	retVal = TRUE;
  int		CType;

	switch (HIWORD (lParam)) {
		case CBN_SELCHANGE:
			EnableWindow(GetDlgItem(hDlg, IDD_RETOBJTYPE), TRUE);	// alm type.
			CType = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_GETCURSEL, 0, 0);
			setupObjTypeList(hDlg, CType);			// setup the alm type for selected C type.
			EnableWindow(GetDlgItem(hDlg, IDD_RETOBJNAME), TRUE);	// enable alm name.
			retVal = setupTypeObjList(hDlg);	// alm name list for selected alm type
			break;
	}
	return retVal;
}			// handleRetParamType(HWND hDlg...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION: handleRetObjectType
// DESCRIPTION:  Will handle the parameter C type.  When the user changes
//				the selection or selects a C type it will be saved or modified in
//				the temp area.
//
/////////////////////////////////////////////////////////////////////////////

BOOL handleRetObjectType(HWND hDlg, LONG lParam)
{
	BOOL			retVal = TRUE;

	switch (HIWORD (lParam)) {
		case CBN_SELCHANGE:
			EnableWindow(GetDlgItem(hDlg, IDD_RETOBJNAME), TRUE);	// alm name.
			retVal = setupTypeObjList(hDlg);	// alm name list for selected alm type
			break;
	}
	return retVal;
}			// handleRetObjectType(HWND hDlg...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:	setupObjTypeList
// DESCRIPTION:  Setup the list of ALM types available for selected C type.
/////////////////////////////////////////////////////////////////////////////

BOOL setupObjTypeList ( HWND hDlg, int CType)
{
	int	 				theIndex = 0;			// select ist item in list.
	TYPEID			theType;
	long				theObjGetKey;
	char				temp[SCRPNAMESIZE];
	BOOL				retVal = TRUE;

		// setup the object type lists.
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_RESETCONTENT, theIndex, 0L);
	theObjGetKey = AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
	while ((theType = ATypeGetNext(&theObjGetKey)) != 0 ) {
		if (CType < BASTYPE_FIXEDSTR) {
			if (theType == OTYPE_NUMBER){
				ATypeGetName(theType, (LPSTR)&temp, SCRPNAMESIZE);
				theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_ADDSTRING, 0, (long)&temp);
				theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_SETITEMDATA, theIndex, (long)theType);
			}
		}
		else if (theType == OTYPE_TEXT) {
			ATypeGetName(theType, (LPSTR)&temp, SCRPNAMESIZE);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_ADDSTRING, 0, (long)&temp);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_SETITEMDATA, theIndex, (long)theType);
     }
	}					// while (theOBJ !=0)...
  theIndex = 0;
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_SETCURSEL, theIndex, 0L);
	return retVal ;
}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION: setupTypeObjList
// DESCRIPTION:  Will setup a list of objects in the object names combo box
//				for the selected ALM type/name.
//
/////////////////////////////////////////////////////////////////////////////

BOOL setupTypeObjList(HWND hDlg)
{
	BOOL			retVal = TRUE;
	TYPEID		almType;
	char			almName[SCRPNAMESIZE];
	OBJECTID	objID;
	long			theObjGetKey;
	char			objName[SCRPNAMESIZE];
	int				theIndex, index;

	SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_RESETCONTENT, 0, 0);
	index = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_GETCURSEL, 0, 0);
	almType = (TYPEID)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_GETITEMDATA, index, 0L);
	SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_GETLBTEXT, index, (long)&almName);
	if (index != CB_ERR){
		theObjGetKey = AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
		while ((objID = AObjGetNextObject(almType, (LPLONG) &theObjGetKey)) != 0 ) {
			AObjGetName(objID, (LPSTR)&objName, SCRPNAMESIZE);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_ADDSTRING, 0, (long)&objName);
			theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_SETITEMDATA, theIndex, (long)objID);
		}					// while (objID !=0)...
	}			// if (index != CB_ERR...
	theIndex = 0;
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_SETCURSEL, theIndex, 0L);
	return retVal;
}			// setupTypeObjList(HWND hDlg...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION: handleRetObjectName
// DESCRIPTION:  Will handle the selected ALM.  When the user changes
//				the selection or selects an ALM it will be saved or modified in
//				the temp area.
//
/////////////////////////////////////////////////////////////////////////////

BOOL handleRetObjectName(HWND hDlg, LONG lParam)
{
	BOOL retVal = TRUE;

  I_UNUSED(hDlg);
	switch (HIWORD (lParam)) {
		case CBN_KILLFOCUS:
			break;

		case CBN_SELCHANGE:
			break;
	}
	return retVal;
}			// handleRetObjectName(HWND hDlg...

/* end of return argument dialog......*/
