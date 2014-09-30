///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
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
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"calc.cpp"
//
// Author:	Kevin Smart
//   Start:		July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	CALCULATION class functions for object management
//
//	jon	15-Dec-92 added GetNetxtVariable method in support of ownership in
//								event handlers.
//
////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <bwcc.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <math.h>

#include <helpids.h>
#include <o_number.h>

#include "calc_obj.h"
#include "strclass.h"
#include "calc.h"
#include "heap.h"
#include "listdata.h"
#include "calc_dlg.h"

#define 	MAX_EDIT	255
#define		DATACOUNT	4

////////////////////////////////////////////////////////////////////////////

char*	cnsts[]	= {"E",   "PI"};

#define num_cnsts	(sizeof(cnsts)/sizeof(cnsts[0]))

////////////////////////////////////////////////////////////////////////////

extern	GLOBAL	G;


////////////////////////////////////////////////////////////////
//
// FUNCTION:	::operator new and ::operator delete
//
// DESC:			Replaces the system-supplied new and delete operators.
//
////////////////////////////////////////////////////////////////

void*		operator new(
	size_t		s
	)
	{
	return MemAlloc(s);
	}

void		operator delete(
	void*			p
	)
	{
	MemFree(p);
	}
////////////////////////////////////////////////////////////////////////////

////////////////////Function Prototypes for this File///////////////////////


////////////////////////////////////////////////////////////////////////////

/* CALCULATION::CALCULATION(OBJECTID objID, BOOL wantExpression) // runtime constructor -- faster

{
	theObject	= objID;

	// variableOBJIDs is a list of object variables
	variableOBJIDs = (OBJECTID*)AObjLockData(theObject, 0L);


  // is a pointer to a STRING class that manages the ALM object
	variableNames = new STRING(theObject, 1L);

	// calcExpression is the expression string
	if(wantExpression){
		calcExpression = (char*)AObjLockData(theObject, 2L);
		}
	else
  	{
  	calcExpression = NULL;
		}

  // calcHeader keeps track of the auto-update flag, the result variable, and the count
  calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);
	if(calcHeader==NULL) {
    AObjResizeData(theObject, 3L, sizeof(CALCHEADER));
    calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);

    calcHeader->count			= 0;
    calcHeader->autoUpdate	= FALSE;
    calcHeader->resultID		= 0L;
    calcHeader->ver				= VERSION_OBJECT;

		}
} */

////////////////////////////////////////////////////////////////////////////

CALCULATION::CALCULATION(OBJECTID objID)

{

  theObject	= objID;

	if(AObjGetDataCount(theObject)==0) {
    AObjSetDataCount(theObject, DATACOUNT);
    }

  // variableOBJIDs is a list of object variables
  variableOBJIDs = (OBJECTID*)AObjLockData(theObject, 0L);


	// variableNames is a pointer to a string object that
	// is the list of aliases for the object variables
	variableNames = new STRING(theObject, 1L);


  // calcExpression is the expression string
  calcExpression = (char*)AObjLockData(theObject, 2L);
	if(calcExpression==NULL) {
		AObjResizeData(theObject, 2L, sizeof(char));
    calcExpression = (char*)AObjLockData(theObject, 2L);
    calcExpression[0] = '\0';

		}

  // calcHeader keeps track of the auto-update flag, the result variable, and the count
  calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);
  if(calcHeader==NULL) {
    AObjResizeData(theObject, 3L, sizeof(CALCHEADER));
    calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);

    calcHeader->count			= 0;
    calcHeader->autoUpdate	= FALSE;
    calcHeader->resultID		= 0L;
    calcHeader->ver				= VERSION_OBJECT;

		}
}

////////////////////////////////////////////////////////////////////////////

CALCULATION::~CALCULATION()

{

	delete variableNames;

  if(variableOBJIDs)
	  AObjUnlockData(theObject, 0L);

///	if (calcExpression)
///  	{
	AObjUnlockData(theObject, 2L);
///   }

	AObjUnlockData(theObject, 3L);

}

////////////////////////////////////////////////////////////////////////////

BOOL	CALCULATION::SetExpression(char* expr, long exprLen)
	{
	BOOL	returnVal;

	AObjUnlockData(theObject, 2L);
	returnVal = AObjSetData(theObject, 2L, expr, exprLen);
	calcExpression = (char*)AObjLockData(theObject, 2L);
  return returnVal;
  }

////////////////////////////////////////////////////////////////////////////
void	CALCULATION::OBJD_Evaluate(HWND hDlg)

{
	char				err[MAX_EDIT];
	long double	x;
	long				d0, d2, d3;
	void				*data4, *data6, *data7;

	if(CheckForErrors(hDlg, FALSE))
		return;

	d0 = AObjGetDataSize(theObject, 0L);
	d2 = AObjGetDataSize(theObject, 2L);
	d3 = AObjGetDataSize(theObject, 3L);

	if(!AObjSetDataCount(theObject, 8L)) {
		MessageBox(hDlg, "Unable to create temporary memory to evaluate expression.",
							 NULL, MB_OK | MB_ICONEXCLAMATION);
		goto UnlockDatas;
		}

	if((!variableOBJIDs && d0!=0L) || !calcExpression || !calcHeader) {
		MessageBox(hDlg, "Unable to create temporary memory to evaluate expression.",
							 NULL, MB_OK | MB_ICONEXCLAMATION);
		goto UnlockDatas;
		}

	AObjSetData(theObject, 4L, variableOBJIDs, d0);

	AObjSetData(theObject, 6L, calcExpression, d2);
	AObjSetData(theObject, 7L, calcHeader, d3);

	data4 = AObjLockData(theObject, 4L);
	data6 = AObjLockData(theObject, 6L);
	data7 = AObjLockData(theObject, 7L);

	if((!data4 && d0!=0L) || !data6 || !data7) {
		MessageBox(hDlg, "Unable to create temporary memory to evaluate expression.",
							 NULL, MB_OK | MB_ICONEXCLAMATION);
		goto SetDatas;
		}

	UpdateObject(hDlg, FALSE);
  if(!G.ParseError()) {
		BOOL	error = evaluate((const char*)calcExpression, x, err, theObject);
		if (!error) {
			sprintf(err, "Answer: %Lg", x);
			BWCCMessageBox(hDlg, err, "Calculation Object", MB_OK | MB_ICONINFORMATION);
			goto SetDatas;
			}
		else {
			MessageBox(hDlg, err, "Calculation Object", MB_OK | MB_ICONEXCLAMATION);
			}
		}

	SetDatas:

	if(data4)
  	{
    if(variableOBJIDs)
   	  AObjUnlockData(theObject, 0L);
		AObjSetData(theObject, 0L, data4, d0);
		variableOBJIDs = (OBJECTID*)AObjLockData(theObject, 0L);
    }
	if(data6)
  	{
		AObjUnlockData(theObject, 2L);
		AObjSetData(theObject, 2L, data6, d2);
		calcExpression = (char*)AObjLockData(theObject, 2L);
    }
	if(data7)
  	{
		AObjUnlockData(theObject, 3L);
		AObjSetData(theObject, 3L, data7, d3);
		calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);
    }

  UnlockDatas:
	AObjUnlockData(theObject, 4L);
	AObjUnlockData(theObject, 6L);
	AObjUnlockData(theObject, 7L);
	AObjSetDataCount(theObject, 4L);
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::InitDlgControls(HWND hDialog)

{
	SendDlgItemMessage(hDialog, IDD_EXPRESSION, WM_SETTEXT, 0, (long)calcExpression);
  FillResult(hDialog, calcHeader->resultID);
  CheckDlgButton(hDialog, IDD_AUTO_UPDATE, calcHeader->autoUpdate);
  FillObjects(hDialog);
  FillVariables(hDialog);

  UpdateAddButton(hDialog);
  UpdateRemoveButton(hDialog);
  UpdateEditButton(hDialog);
  UpdateAliasControl(hDialog);

  SetFocus(GetDlgItem(hDialog, IDD_OBJECTS));
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::FillResult(HWND hDialog, OBJECTID oid)

{
  TYPEID	typeID;
  long		getKey = AOBJ_GETFIRSTKEY; 
  char		name[MAX_EDIT+1];
  LRESULT	index;

  SendDlgItemMessage(hDialog, IDD_RESULT, CB_RESETCONTENT, 0, 0L);

	index = SendDlgItemMessage(hDialog, IDD_RESULT, CB_ADDSTRING, 0, (long)"(None)");
	SendDlgItemMessage(hDialog, IDD_RESULT, CB_SETITEMDATA, (WPARAM)index, 0L);
	if(oid==0L) {
		SendDlgItemMessage(hDialog, IDD_RESULT, CB_SETCURSEL, (WPARAM)index, 0L);
		}

	while((typeID=AObjGetNextObject(OTYPE_NUMBER, &getKey))!=0) {
    AObjGetName(typeID, name, MAX_EDIT);
    index = SendDlgItemMessage(hDialog, IDD_RESULT, CB_ADDSTRING, 0, (long)name);
		SendDlgItemMessage(hDialog, IDD_RESULT, CB_SETITEMDATA, (WPARAM)index, typeID);

    if(typeID==oid) {
			SendDlgItemMessage(hDialog, IDD_RESULT, CB_SETCURSEL, (WPARAM)index, 0L);
      }
    } 
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::FillObjects(HWND hDialog)

{
  TYPEID	typeID;
  long		getKey = AOBJ_GETFIRSTKEY;
  char		name[MAX_EDIT+1];
  LRESULT	index;

  SendDlgItemMessage(hDialog, IDD_OBJECTS, LB_RESETCONTENT, 0, 0L);

  while((typeID=AObjGetNextObject(OTYPE_NUMBER, &getKey))!=0) {
    AObjGetName(typeID, name, MAX_EDIT);
    index = SendDlgItemMessage(hDialog, IDD_OBJECTS, LB_ADDSTRING, 0, (long)name);
		SendDlgItemMessage(hDialog, IDD_OBJECTS, LB_SETITEMDATA, (WPARAM)index, typeID);
		}

}
///////////////////////////////////////////////////////////////////////////

void	CALCULATION::FillVariables(HWND hDialog)

{
  char		name[MAX_EDIT+1];
  LRESULT	index;
  LISTDATA*	ldPtr;

  if(variableOBJIDs==NULL) {
    // allocation error
    return;
    }


	if(variableNames==NULL) {
    // allocation error
    return;
    }

  SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_RESETCONTENT, 0, 0L);

  for(int i=0; i<calcHeader->count; i++) {
    AObjGetName(variableOBJIDs[i], name, MAX_EDIT);
    index = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_ADDSTRING, 0, (long)name);
    ldPtr = new LISTDATA;
    ldPtr->Init(variableOBJIDs[i], variableNames->Get(i));
		SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETITEMDATA, (WPARAM)index, (long)ldPtr);
    }
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::UpdateAddButton(HWND hDialog)

{
	WORD	i = SendDlgItemMessage(hDialog, IDD_OBJECTS, LB_GETSELCOUNT, 0, 0L);
	HWND  hAddButton	= GetDlgItem(hDialog, IDD_ADD);

  if(i>0) {
    EnableWindow(hAddButton, TRUE);
    }
  else {
    EnableWindow(hAddButton, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::UpdateEditButton(HWND hDialog)

{
  WORD	i = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELCOUNT, 0, 0L);
	HWND	hEditButton	= GetDlgItem(hDialog, IDD_EDIT);


  if(i==1) {
    EnableWindow(hEditButton, TRUE);
    }
  else {
    EnableWindow(hEditButton, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::UpdateRemoveButton(HWND hDialog)

{
  WORD	i = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELCOUNT, 0, 0L);
	HWND	hRemoveButton	= GetDlgItem(hDialog, IDD_REMOVE);

  if(i>0) {
    EnableWindow(hRemoveButton, TRUE);
    }
  else {
    EnableWindow(hRemoveButton, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::UpdateAliasControl(HWND hDialog)

{
  LISTDATA*	ld;
  WPARAM	index;

  WORD	i = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELCOUNT, 0, 0L);
	HWND  hAliasTitle	= GetDlgItem(hDialog, IDD_ALIAS_TITLE);
	HWND  hAlias	= GetDlgItem(hDialog, IDD_ALIAS);


  if(i==1) {
    SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELITEMS, 1, (long)&index);
    ld = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, index, 0L);

    if((LRESULT)ld!=LB_ERR) {
			SetDlgItemText(hDialog, IDD_ALIAS, ld->Alias());
      }

    EnableWindow(hAliasTitle, TRUE);
    EnableWindow(hAlias, TRUE);
    }
  else {
    SendMessage(hAlias, WM_SETTEXT, 0L, NULL);
    EnableWindow(hAliasTitle, FALSE);
    EnableWindow(hAlias, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////

BOOL	CALCULATION::CheckForErrors(HWND hDialog, BOOL checkAutoUpdate)

{
	int i, j;
	LRESULT		lresult;
  DWORD			count;
  LISTDATA*	ldPtr;
  LISTDATA*	ldPtr2;
  OBJECTID	resultID;
  BOOL			autoUpdate;

  // first check for errors and exit if there are errors
  count = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETCOUNT, 0, 0L);
  lresult = SendDlgItemMessage(hDialog, IDD_RESULT, CB_GETCURSEL, 0, 0L);
  if(lresult==CB_ERR) {
    resultID = 0L;
    }
  else {
    resultID = SendDlgItemMessage(hDialog, IDD_RESULT, CB_GETITEMDATA, (WPARAM)lresult, 0L);
    }
  autoUpdate = IsDlgButtonChecked(hDialog, IDD_AUTO_UPDATE);


	for(i=0; i<count; i++) {
		ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, i, 0L);

    // check to make sure alias is not a constant
		for(j=0; j<num_cnsts; j++) {
  	  if(!stricmp(ldPtr->Alias(), cnsts[j])) break;
  	  }  // end...for(j=0; j<num_cnsts; j++)

		if(j<num_cnsts) {
			char	szBuffer[256];
	  	sprintf(szBuffer, "The string \"%s\" is a reserved constant name.  Using that name will cause unpredictable results.", ldPtr->Alias());
			if(MessageBox(hDialog, szBuffer, "Warning", MB_ICONINFORMATION | MB_OKCANCEL)==IDCANCEL) {
				SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETSEL, FALSE, MAKELONG(-1, 0));
				SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETSEL, TRUE, MAKELONG(i, 0));
				SetFocus(GetDlgItem(hDialog, IDD_ALIAS));
				PostMessage(hDialog, WM_COMMAND, IDD_VARIABLES, MAKELONG(GetDlgItem(hDialog, IDD_VARIABLES), LBN_SELCHANGE));
        return TRUE;
				}
	    }

		// check for result in object list error
		if(!strlen(ldPtr->Alias())) {
			MessageBox(hDialog, "Object variables must have names.", NULL, MB_OK | MB_ICONEXCLAMATION);
			SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETSEL, FALSE, MAKELONG(-1, 0));
			SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETSEL, TRUE, MAKELONG(i, 0));
			SetFocus(GetDlgItem(hDialog, IDD_ALIAS));
			PostMessage(hDialog, WM_COMMAND, IDD_VARIABLES, MAKELONG(GetDlgItem(hDialog, IDD_VARIABLES), LBN_SELCHANGE));
			return TRUE;
      }

    if (checkAutoUpdate && ldPtr->TheObject()==resultID && autoUpdate) {
      MessageBox(hDialog, "Result object cannot be in the \"Expression Objects\" list when the \"Auto Update\" box is checked.",
		      		   NULL, MB_OK | MB_ICONEXCLAMATION );
			return TRUE;
      }
    }

  // check for "same alias" warning
	for(i=0; (int)i<((int)count-1); i++) {
    ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, i, 0L);
     
    for(int j=i+1; j<count; j++) {
			ldPtr2 = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, j, 0L);
       
      if(!strcmp(ldPtr->Alias(), ldPtr2->Alias())) {
			  // the aliases are the same!!
			  char*	buf = new char[80+lstrlen(ldPtr->Alias())];
			  wsprintf(buf, "Variable name \"%s\" is a duplicate.\nThis may cause unpredictable results.", ldPtr->Alias());
			  int MBret = MessageBox(hDialog, buf, "Warning", MB_OKCANCEL | MB_ICONINFORMATION);
        delete buf;

			  if(MBret==IDCANCEL) {
			    // return to the dialog box
			    SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETSEL, FALSE, MAKELONG(-1, 0));
			    SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETSEL, TRUE, MAKELONG(i, 0));
			    SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETSEL, TRUE, MAKELONG(j, 0));
			    PostMessage(hDialog, WM_COMMAND, IDD_VARIABLES, MAKELONG(GetDlgItem(hDialog, IDD_VARIABLES), LBN_SELCHANGE));
					return TRUE;
			    }
			  else {
			    // continue exiting
			    i=(WORD)count;
			    break;
			    }	
			  }
      }
		}

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::UpdateObject(HWND hDialog, BOOL test)

{
  WORD			i;
  LRESULT		lresult;
  DWORD			count;
  LISTDATA*	ldPtr;
  OBJECTID	resultID;
  BOOL			autoUpdate;

  // first check for errors and exit if there are errors
  count = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETCOUNT, 0, 0L);
  lresult = SendDlgItemMessage(hDialog, IDD_RESULT, CB_GETCURSEL, 0, 0L);
  if(lresult==CB_ERR) {
    resultID = 0L;
    }
  else {
    resultID = SendDlgItemMessage(hDialog, IDD_RESULT, CB_GETITEMDATA, (WPARAM)lresult, 0L);
    }
  autoUpdate = IsDlgButtonChecked(hDialog, IDD_AUTO_UPDATE);

	if(test) {
		G.SetParseError(CheckForErrors(hDialog, TRUE));
		if(G.ParseError())
    	return;
		} // end...if(test)

	// Data 3
  AObjUnlockData(theObject, 3L);
  AObjResizeData(theObject, 3L, sizeof(CALCHEADER));
  calcHeader = (CALCHEADER*)AObjLockData(theObject, 3L);
  if(calcHeader==NULL) {
    MessageBox(NULL, "OIP Memory Allocation Failure", NULL, MB_OK | MB_ICONEXCLAMATION );
    return;
    }

  calcHeader->resultID = resultID;
  calcHeader->autoUpdate = autoUpdate;
	calcHeader->count = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETCOUNT, 0, 0L);


  // Data 0 & Data 1

  if (variableNames == NULL) {
    return;
    }

	variableNames->Clear();

  if(variableOBJIDs)
	  AObjUnlockData(theObject, 0L);
	AObjResizeData(theObject, 0L, sizeof(long)*count);
  variableOBJIDs = (OBJECTID*)AObjLockData(theObject, 0L);
	calcHeader->count = count;

  if (!variableOBJIDs && count!=0) {
    return;
    }

  for(i=0; i<count; i++) {
    ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, i, 0L);
    variableOBJIDs[i] = ldPtr->TheObject();
    variableNames->Append(ldPtr->Alias());
    }

  // Data 2--store string
	i = SendDlgItemMessage(hDialog, IDD_EXPRESSION, WM_GETTEXTLENGTH, 0, 0L);
	AObjUnlockData(theObject, 2L);
	AObjResizeData(theObject, 2L, sizeof(char)*(i+1));
  calcExpression = (char*)AObjLockData(theObject, 2L);
	calcExpression[i] = 0;
  GetDlgItemText(hDialog, IDD_EXPRESSION, calcExpression, i+1);
	AObjUnlockData(theObject, 2L);
	AObjSetData(theObject, 2L, (void*)calcExpression, i+1);
	calcExpression = (char*)AObjLockData(theObject, 2L);
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::DeleteListData(HWND hDialog)

{
  LISTDATA*	ldPtr;

  DWORD count = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETCOUNT, 0, 0L);

  for(WORD i=0; i<count; i++) {
    ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, i, 0L);
    if(ldPtr!=NULL && (LRESULT)ldPtr!=LB_ERR) {
      delete ldPtr;
      }
    }
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::Add(HWND hDialog)

{
  DWORD		index;
  WORD*		intArray;
  OBJECTID	objid;
  LISTDATA*	ldPtr;
  char		name[MAX_EDIT+1];

  WORD count = SendDlgItemMessage(hDialog, IDD_OBJECTS, LB_GETSELCOUNT, 0, 0L);

  intArray = new WORD[count];
  if(intArray==NULL) {
    // allocation error
    return;
    }

  SendDlgItemMessage(hDialog, IDD_OBJECTS, LB_GETSELITEMS, count, (long)intArray);

  for(WORD i=0; i<count; i++) {
    objid = SendDlgItemMessage(hDialog, IDD_OBJECTS, LB_GETITEMDATA, intArray[i], 0L);

    ldPtr = (LISTDATA*)new LISTDATA(objid);
    AObjGetName(objid, name, MAX_EDIT);
    index = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_ADDSTRING, 0, (long)name);
		SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_SETITEMDATA, (WPARAM)index, (long)ldPtr);
    }

  delete intArray;
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::Remove(HWND hDialog)

{
  LISTDATA*	ldPtr;

  WORD count = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELCOUNT, 0, 0L);

  WORD*	intArray = new WORD[count];
  if(intArray==NULL) {
    // allocation error
    return;
    }

  SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELITEMS, count, (long)intArray);

  for(int i=count-1; i>=0; i--) {
    ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, intArray[i], 0L);
    SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_DELETESTRING, intArray[i], 0L);

    if(ldPtr!=NULL && (LRESULT)ldPtr!=LB_ERR) {
      delete ldPtr;
      }
    }
  delete intArray;
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::Edit(HWND hDialog)

{
  int		index;
  LISTDATA*	ldPtr;

  WORD	count = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELCOUNT, 0, 0L);

  if(count!=1) {
    return;
    }

  long	ret = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELITEMS, 1, (long)&index);

  if(ret==LB_ERR) {
    return;
    }

  ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, index, 0L);

  if(ldPtr==NULL || (LRESULT)ldPtr==LB_ERR) {
    return;
    }

  AObjEdit(ldPtr->TheObject());
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::Alias(HWND hDialog)

{
  char		name[MAX_EDIT];
  WORD 		intArray;
  LISTDATA*	ldPtr;

  WORD	count = SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELCOUNT, 0, 0L);
  if(count!=1) {
    return;
    }

  SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETSELITEMS, 1, (long)&intArray);
  ldPtr = (LISTDATA*)SendDlgItemMessage(hDialog, IDD_VARIABLES, LB_GETITEMDATA, intArray, 0L);

  if((DWORD)ldPtr==LB_ERR) {
    // an error occurred trying to get the LB item data
    return;
    }
	SendDlgItemMessage(hDialog, IDD_ALIAS, WM_GETTEXT, MAX_EDIT, (long)name);

	ldPtr->SetAlias(name);
	SetDlgItemText(hDialog, IDD_ALIAS, ldPtr->Alias());
}

////////////////////////////////////////////////////////////////////////////

void CALCULATION::DoAssign(
	OBJECTID		srcObject,
	OBJECTID		dstObject)
{
  long		dataPtrCnt;
  long 		ptrIndex;
  TYPEID	srcType,	dstType;

  srcType	= AObjGetType(srcObject);
  dstType	= AObjGetType(dstObject);

  if(srcObject==NULL || dstObject==NULL) {
    MessageBox(NULL, "Assign Failed; Null Object Value", NULL, MB_OK | MB_ICONEXCLAMATION);
    return;
    }

  if(srcType!=OTYPE_EXPRESSION || dstType!=OTYPE_EXPRESSION) {
    MessageBox(NULL, "Assign Failed; Invalid Object Type", NULL, MB_OK | MB_ICONEXCLAMATION);
    return;
    }

  dataPtrCnt = AObjGetDataCount(srcObject);
  for(ptrIndex=0; ptrIndex<dataPtrCnt; ptrIndex++) {
    CopyDataPtr(dstObject, srcObject, ptrIndex);
    }
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::ImportObject()

{
	long			expr_len;
	char			name[256];
	OBJECTID	oid;
  long			ret;

	if(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &calcHeader->ver, sizeof(int)) != AUPSF_NOERROR) {
		AUpsfLogComment("Defaulting to version 0.");
		calcHeader->ver = 0;
		}
	if(AUpsfImportField("AUTOUPD", 0L, AUPSF_NATV_BOOL, &calcHeader->autoUpdate, sizeof(BOOL)) != AUPSF_NOERROR) {
		AUpsfLogComment("Defaulting to not auto update.");
		calcHeader->autoUpdate = FALSE;
		}
	if(AUpsfFieldInfo("EXPRESS", 0L, AUPSF_NATV_STRING, NULL, &expr_len) == AUPSF_NOERROR) {
		// resize the expression string and assign it
		AObjUnlockData(theObject, 2L);
		AObjResizeData(theObject, 2L, expr_len);
		calcExpression = (char*)AObjLockData(theObject, 2L);
		AUpsfImportField("EXPRESS", 0L, AUPSF_NATV_STRING, calcExpression, expr_len);
		}

	calcHeader->count = AUpsfFieldCount("VARIABLE");

	// initialize the data
  if(variableOBJIDs)
	  AObjUnlockData(theObject, 0L);
	AObjResizeData(theObject, 0L, sizeof(long)*calcHeader->count);
	variableOBJIDs = (OBJECTID*)AObjLockData(theObject, 0L);
	variableNames->Clear();

  // do the assignments
	int	i = 0;
  ret = AUPSF_NOERROR;
	while (i<calcHeader->count && ret==AUPSF_NOERROR) {
		ret = AUpsfImportField("VARIABLE", i, AUPSF_NATV_OBJECTREF, &oid, sizeof(OBJECTID));
		if(ret != AUPSF_NOERROR) {
			AUpsfLogComment("Error importing variable--Quit importing list.");
			continue;
      }

		ret = AUpsfImportField("IDENTIFY", i, AUPSF_NATV_STRING, name, 256);
		if(ret != AUPSF_NOERROR) {
			AUpsfLogComment("Error importing variable itentifier--Quit importing list.");
			continue;
      }

		variableOBJIDs[i] = oid;
		variableNames->Append(name);

    i++;
		}

	// take care of the error possibility
	if(i<calcHeader->count) {
		// an import error occurred--fix the data
		calcHeader->count = i;

    if(variableOBJIDs)
	    AObjUnlockData(theObject, 0L);
		AObjResizeData(theObject, 0L, sizeof(long)*calcHeader->count);
		variableOBJIDs = (OBJECTID*)AObjLockData(theObject, 0L);
    }

	if(AUpsfImportField("RESULT", 0L, AUPSF_NATV_OBJECTREF, &calcHeader->resultID, sizeof(OBJECTID)) != AUPSF_NOERROR) {
		AUpsfLogComment("Defaulting to no result.");
		calcHeader->resultID = 0L;
		}

}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::ExportObject()

{
	// ignore errors--If an error, continue exporting
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &calcHeader->ver, sizeof(int));
	AUpsfExportField("AUTOUPD", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &calcHeader->autoUpdate, sizeof(BOOL));
	AUpsfExportField("EXPRESS", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, calcExpression, strlen(calcExpression));


	for(int i=0; i<calcHeader->count; i++) {
		AUpsfExportField("VARIABLE", i, AUPSF_NATV_OBJECTREF,
		  AUPSF_UNIV_OBJECTREF, &variableOBJIDs[i], sizeof(OBJECTID));
		AUpsfExportField("IDENTIFY", i, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
		  variableNames->Get(i), strlen(variableNames->Get(i)));
		}

	AUpsfExportField("RESULT", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &calcHeader->resultID, sizeof(OBJECTID));
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::CheckIDs()

{
  BOOL	valid;

	// data 0 & data 1

	for(int i=0; i<calcHeader->count; i++) {
    valid = AObjCheckType(variableOBJIDs[i], 0L);
    if(!valid) {
      // the object was deleted
      DataObjectDelete(variableOBJIDs, i);
      variableNames->Delete(i);
			calcHeader->count--;
      i--;
      }
    }

  // data 3
  valid = AObjCheckType(calcHeader->resultID, 0L);
  if(!valid) {
    calcHeader->resultID = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////

void	CALCULATION::Validate(pAObjMessage theSystem)

{
  BOOL	changed = FALSE;
  int		i;

  // data 0 & data 1

  for (i=0; i<calcHeader->count; i++) {
    changed = AObjValidateID(&variableOBJIDs[i], theSystem);
    if (changed && variableOBJIDs[i]==0L) {
      DataObjectDelete(variableOBJIDs, i);  // update the list
      variableNames->Delete(i);
      calcHeader->count--;
      i--;
      }
    }

	// data 3
  changed = AObjValidateID(&calcHeader->resultID, theSystem);
}

////////////////////////////////////////////////////////////////////////////

void CALCULATION::DataObjectDelete(OBJECTID* oidP, int i)

{
  while (i+1<calcHeader->count) {
    oidP[i] = oidP[i+1];
    i++;
    }

  if(variableOBJIDs)
	  AObjUnlockData(theObject, 0L);
	AObjResizeData(theObject, 0, sizeof(OBJECTID)*(calcHeader->count-1));
  variableOBJIDs = (OBJECTID*)AObjLockData(theObject, 0);
}

////////////////////////////////////////////////////////////////////////////

STRING* CALCULATION::GetVariableNames()

{
	return variableNames;
}
////////////////////////////////////////////////////////////////////////////

OBJECTID* CALCULATION::GetData0()

{
	return variableOBJIDs;
}
////////////////////////////////////////////////////////////////////////////

CALCHEADER* CALCULATION::GetData3()

{
	return calcHeader;
}
////////////////////////////////////////////////////////////////////////////

char* CALCULATION::Get(WORD index)  // return pointer to string

{
	return variableNames->Get(index);
}
////////////////////////////////////////////////////////////////////////////

void CALCULATION::CopyDataPtr(OBJECTID dstObj, OBJECTID srcObj, long index)

{
	if(index == 1L) // Data 1 is only modified by STRING class
		{
		variableNames->CopyDataPtr(dstObj, index);
		return;
		}

	long 		srcSize    = AObjGetDataSize(srcObj, index);
  LPVOID	srcPtr     = AObjLockData(srcObj, index);
  LPVOID 	dstPtr;

	AObjUnlockData(dstObj, index);
	AObjResizeData(dstObj, index, srcSize);
	dstPtr = AObjLockData(dstObj, index);

  if(srcPtr!=NULL && dstPtr!=NULL) {
    _fmemcpy(dstPtr, srcPtr, (size_t)srcSize);
    }

	AObjUnlockData(srcObj, index);
	AObjUnlockData(dstObj, index);
}

////////////////////////////////////////////////////////////////////////////

BOOL	CALCULATION::InVarList(OBJECTID numbObj)

{
  for(int i=0; i<calcHeader->count; i++) {
    if(numbObj==variableOBJIDs[i]) return TRUE;
    }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////

BOOL	CALCULATION::ValidExpression(char* errString)

{
	// calcExpression is the expression string
	//evaluate returns TRUE if there is an error

	long double	result;

	return !evaluate(calcExpression, result, errString, theObject, NULL, FALSE);
}

////////////////////////////////////////////////////////////////////////////
//   CALCULATION::GetNextVariable
//
// Fetches the next variable from our variable list.  Returns TRUE if
// there was a next variable, else FALSE.  Set the cookie to
// AOBJ_GETFIRSTKEY to start the enumeration, otherwise DON'T TOUCH IT!
////////////////////////////////////////////////////////////////////////////
BOOL CALCULATION::GetNextVariable(
  LPOBJECTID  next,
  LPLONG      cookie)
  {
  if (*cookie == AOBJ_GETFIRSTKEY)
    *cookie = 0;

  if (*cookie >= calcHeader->count)
    return FALSE;

  *next = variableOBJIDs[(int)*cookie];
  (*cookie)++;
  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////
