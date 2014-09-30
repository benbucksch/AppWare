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
 *	"dll_rdlg.cpp: In here we will handle the dialogs for the DLL object.
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
#include	<o_number.h>
#include	<o_text.h>
#include	<intdebug.h>

#include "dll.h"


extern HINSTANCE	hInstance;
extern MEMBLOCKID	objBlks[MAXNUMOFARGS];	// temp ptrs for objs data blocks.


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
	BOOL 				retVal = FALSE;
	DLL_RETARG*	ptr;
	OBJECTID		selObjID;	// object id associated with the c param type.
	int					CType;		// index to c param type.
	int					index;
	char				objName[DLLNAMESIZE];
		
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
					AMemFree(objBlks[(int)DLLRETPARAMINDEX]);
					objBlks[(int)DLLRETPARAMINDEX] = AMemAllocate(sizeof(DLL_RETARG));
					if (objBlks[(int)DLLRETPARAMINDEX]) {	// save changes into the edit area...
						ptr = (DLL_RETARG*)AMemLock(objBlks[(int)DLLRETPARAMINDEX]);
						index = (int)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_GETCURSEL, 0, 0);
						selObjID = (OBJECTID)SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_GETITEMDATA, index, 0L);
						SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_GETLBTEXT, index, (long)&objName);
						if (index != CB_ERR){
							ptr->theObject = selObjID;
						}			// if (index != CB_ERR...
						CType = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_GETCURSEL, 0, 0);
						SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_GETLBTEXT, CType, (long)&objName);
						if (CType != CB_ERR){
							ptr->paramType = CType;
						}			// if (index != CB_ERR...
						AMemUnlock(objBlks[(int)DLLRETPARAMINDEX]);
					}			// if (objBlks[DLLRETPARAMINDEX])...
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
// DESCRIPTION:  Initialized dll function argument dialog box.
/////////////////////////////////////////////////////////////////////////////

BOOL initRetArgDlg ( HWND hDlg, LONG lParam)
{
	int	 				theIndex;
	TYPEID			theType, selType = NULL;
	DLL_RETARG* ptr;
	int					CType;
  OBJECTID		selObjID;
	char				almName[DLLNAMESIZE];
	char				objName[DLLNAMESIZE];
	BOOL				done = FALSE;
  BOOL				retVal = TRUE;

  I_UNUSED(lParam);

	AUtlCenterDialog(hDlg, 0);

	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"short");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"unsigned short");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"long");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"unsigned long");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"float");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"double");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"long double");
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_ADDSTRING, 0, (long)"char far*");

	ptr = (DLL_RETARG*)AMemLock(objBlks[(int)DLLRETPARAMINDEX]);
	CType = ptr->paramType;
	selObjID = ptr->theObject;
	if (selObjID)
		selType = AObjGetType(selObjID);
	AMemUnlock(objBlks[(int)DLLRETPARAMINDEX]);

	if (CType == -1) {
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJTYPE), FALSE);	// disable alm type.
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJNAME), FALSE);	// disable alm name.
	}
	else {			// A return argument have been set...
		SendDlgItemMessage(hDlg, IDD_RETPARAMTYPE, CB_SETCURSEL, CType, 0L);
			// setup the object type lists.
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJTYPE), TRUE);	// alm type.
		EnableWindow(GetDlgItem(hDlg, IDD_RETOBJNAME), TRUE);	// alm name.
		setupObjTypeList(hDlg, CType);			// setup the alm type for selected C type.

			// edgar: need to verify that the ALM still exists, .
			// here we make the assumption that the ALM contains a name,
			// the return value is the same regardless, zero if the ALM
			// is not found or if it contains a name of zero length (blank).
		if (ATypeGetName(selType, (LPSTR)&almName, DLLNAMESIZE) > 0)
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
							AObjGetName(selObjID, (LPSTR)&objName, DLLNAMESIZE);
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
	char				temp[DLLNAMESIZE];
	BOOL				retVal = TRUE;

		// setup the object type lists.
	theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_RESETCONTENT, theIndex, 0L);
	theObjGetKey = AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
	while ((theType = ATypeGetNext(&theObjGetKey)) != 0 ) {
		if (CType < CTYPE_CHRFARPTR) {
			if (theType == OTYPE_NUMBER){
				ATypeGetName(theType, (LPSTR)&temp, DLLNAMESIZE);
				theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_ADDSTRING, 0, (long)&temp);
				theIndex = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_SETITEMDATA, theIndex, (long)theType);
			}
		}
		else if (theType == OTYPE_TEXT) {
			ATypeGetName(theType, (LPSTR)&temp, DLLNAMESIZE);
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
	char			almName[DLLNAMESIZE];
	OBJECTID	objID;
	long			theObjGetKey;
	char			objName[DLLNAMESIZE];
	int				theIndex, index;

	SendDlgItemMessage(hDlg, IDD_RETOBJNAME, CB_RESETCONTENT, 0, 0);
	index = (int)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_GETCURSEL, 0, 0);
	almType = (TYPEID)SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_GETITEMDATA, index, 0L);
	SendDlgItemMessage(hDlg, IDD_RETOBJTYPE, CB_GETLBTEXT, index, (long)&almName);
	if (index != CB_ERR){
		theObjGetKey = AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
		while ((objID = AObjGetNextObject(almType, (LPLONG) &theObjGetKey)) != 0 ) {
			AObjGetName(objID, (LPSTR)&objName, DLLNAMESIZE);
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
