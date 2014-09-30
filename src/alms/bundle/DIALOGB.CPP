/////////////////////////////////////////////////////////////////////////////
// File Name:  GroupObj.cpp
//              Copyright 1989-92 Serius Corporation
// Author:  Sibai Li
// Date:  5/13/92
//
// Description.: Dialog Routine for the Serius Group object:
////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <windows.h>
#include <windowsx.h>
#include <memory.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>
#include <o_pict.h>
#include <o_list.h>
#include <helpids.h>
#include "bundle.h"

void doValidateObjectIDs (AOBJECTID bObject, pAObjMessage theSystem)
{
	LPOBJECTID  lpObjects ;
	WORD        nObjects, nObjectsRemaining ;
	BOOL        changed ;
	int         i, j ;

	lpObjects = (AOBJECTID *) AObjLockData(bObject, OBJECT_INDEX) ;
	nObjects = (WORD)AObjGetDataSize (bObject, OBJECT_INDEX) /sizeof(AOBJECTID) ;
	nObjectsRemaining = nObjects ;
	for (i=nObjects-1; i>=0; i--)
	{
		changed = AObjValidateID(&lpObjects[i], theSystem) ;
		if (changed && (lpObjects[i]==0))
		{
			nObjectsRemaining-- ;
			for(j=i; j<nObjectsRemaining; j++)
			{
				lpObjects[j] = lpObjects[j+1] ; //shift objects
			}
		}
	}

	if (lpObjects != NULL)
		AObjUnlockData(bObject, OBJECT_INDEX) ;
	AObjResizeData( bObject, OBJECT_INDEX, nObjectsRemaining*sizeof(AOBJECTID)) ;
}
void CheckObjID(AOBJECTID bObject)
{
	AOBJECTID  *lpObjects ;
	int           nObjects, i, j, nObjectsRemaining ;

	lpObjects = (AOBJECTID *) AObjLockData(bObject, OBJECT_INDEX) ;
	nObjects = (int)AObjGetDataSize (bObject, OBJECT_INDEX) /sizeof(AOBJECTID) ;
	nObjectsRemaining = nObjects ;
	for (i=nObjects-1; i>=0; i--)
	{
		if (!AObjCheckType (lpObjects[i], 0))
		{
			nObjectsRemaining-- ;
			for(j=i; j<nObjectsRemaining; j++)
			{
				lpObjects[j] = lpObjects[j+1] ; //shift objects
			}
		}
	}

	if (lpObjects != NULL)
		AObjUnlockData(bObject, OBJECT_INDEX) ;

	AObjResizeData( bObject, OBJECT_INDEX, nObjectsRemaining*sizeof(AOBJECTID)) ;
}


BOOL CALLBACK  BundleObjectProc (HWND hDlg, WORD message,
										  WORD wParam, LONG lParam)
	{
  AOBJECTID hobjBundle ;
	WORD     Index ;
	AOBJECTID theObj ;
	WORD     wID, wNotify;

	switch (message)
		{
		case WM_INITDIALOG:
			AUtlSetTitleFont(OTYPE_BUNDLE, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
			SetWindowLong (hDlg, DWL_USER, lParam) ;
			hobjBundle = (AOBJECTID)lParam ;
			InitDialog(hDlg, hobjBundle) ;
			return FALSE ;

		case WM_COMMAND: //!!PORT!! //!!PORT!!
			wID     = LOWORD(wParam) ;
			wNotify = HIWORD(wParam) ;
			if (wID >= textB && wID <= otherB)
				{
				CheckRadioButton (hDlg, textB, otherB, wID) ;
				UpdateObjectList(hDlg, wID) ;
				return TRUE ;
				}

			switch (wID)
				{
				case AVAILABLE_OBJ:
					switch (wNotify)
						{
						case LBN_SELCHANGE:
							EnableWindow(GetDlgItem(hDlg, addB), TRUE) ;
							return TRUE ;

						case LBN_DBLCLK:
							AddItem(hDlg) ;
							return TRUE ;
						}
					break ;

				case SELECTED_OBJ:
					switch (wNotify)
						{
						case LBN_SELCHANGE:
							EnableWindow (GetDlgItem(hDlg, removeB), TRUE) ;
						  if (ListBox_GetSelCount (GetDlgItem(hDlg, SELECTED_OBJ))==1)
								EnableWindow (GetDlgItem(hDlg, editB), TRUE) ;
	      else
								EnableWindow (GetDlgItem(hDlg, editB), FALSE) ;
							 return TRUE ;

						case LBN_DBLCLK:
							 Index = ListBox_GetCurSel (GetDlgItem (hDlg, SELECTED_OBJ)) ;
							 theObj = (AOBJECTID)ListBox_GetItemData (GetDlgItem (hDlg, SELECTED_OBJ),Index) ;
							 return (AObjEdit(theObj) == A_MODIFIED);
						}
					break ;

				case addB:
					AddItem(hDlg) ;
					return TRUE ;

				case removeB:
					RemoveItem(hDlg) ;
					return TRUE ;

				case editB:
					EditItem(hDlg) ;
					return TRUE ;

				case IDOK:
					// save the File object's default extention
					// save the typeIDs for each object to be stored
	  hobjBundle = (AOBJECTID) GetWindowLong (hDlg, DWL_USER) ;
					saveObj (hDlg, hobjBundle) ;
					EndDialog (hDlg, TRUE) ;
					return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;

				case IDHELP:
					WinHelp (hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Bndl) ;
					return TRUE ;
				}
		}
	 return FALSE ;
	 }

void UpdateObjectList (HWND hDlg, WORD buttonID)
	{
	LONG      theGetType ;
	ATYPEID    IDtype;

	// clear all strings from the list box
	ListBox_ResetContent (GetDlgItem(hDlg, AVAILABLE_OBJ)) ;
	EnableWindow(GetDlgItem(hDlg, addB), FALSE) ;

	// if "other" object types are selected
	if(buttonID == otherB)
	   {
		theGetType = AOBJ_GETFIRSTKEY ;
		while((IDtype=ATypeGetNext(&theGetType)) !=0)
			{
			 if(IDtype!=OTYPE_TEXT &&
				IDtype!=OTYPE_NUMBER &&
				IDtype!=OTYPE_PICTURE &&
				IDtype!=OTYPE_LIST )
				DisplayObj(hDlg, IDtype) ;
			}
		return ;
	   }

	//if"text","number","picture","time","date" object types are selected
	switch(buttonID)
		{
		 case textB:
			  IDtype = OTYPE_TEXT ;
			  break ;

		 case numberB:
			  IDtype = OTYPE_NUMBER;
			  break ;

		 case pictureB:
			  IDtype = OTYPE_PICTURE;
			  break;

		 case listB:
			  IDtype = OTYPE_LIST ;                                                                  
			  break;
		}
	DisplayObj(hDlg, IDtype) ;
	return ;
	}

//Display the text of the object
void DisplayObj(HWND hDlg, ATYPEID IDtype)
	{
	 LONG    theGetObj ;
	 AOBJECTID  objID ;
	 DWORD     objIndex ;
	 LPSTR     text ;

	 text = new char[256] ;
	 theGetObj = AOBJ_GETFIRSTKEY  ;
	 while((objID=AObjGetNextObject(IDtype,&theGetObj))!= NULL)
		{
		AObjGetName(objID, text, 256) ;
		objIndex = ListBox_AddString (GetDlgItem(hDlg, AVAILABLE_OBJ),text) ; 
		ListBox_SetItemData (GetDlgItem (hDlg, AVAILABLE_OBJ), objIndex, objID) ; 
		}
	 delete (text) ;
	 return ;
	}

void InitDialog ( HWND hDlg, AOBJECTID bObject)
	{
	 LPOBJECTID     bObjPtr ;
	 DWORD           objIndex ;
	 char *       lpszString ;
	 char            buffer[OBJECTNAMESIZE] ;
	 int             i;

	 AObjGetName(bObject, buffer, OBJECTNAMESIZE) ;
	 SetWindowText(hDlg, buffer) ;

	 lpszString = new char[256] ;

	 // check the "Text" radio button
	 CheckRadioButton (hDlg, textB, otherB, textB) ;

	 // initialize the Object Selection list
	 UpdateObjectList (hDlg, textB) ;

	 // disable the remove and edit buttons
	 EnableWindow(GetDlgItem(hDlg, removeB), FALSE) ;
	 EnableWindow(GetDlgItem(hDlg, editB),   FALSE) ;

	 // initialize the object list
	 ListBox_ResetContent (GetDlgItem(hDlg, SELECTED_OBJ)) ;
	 bObjPtr = (LPOBJECTID) AObjLockData(bObject, 0) ;

	 if (bObjPtr != NULL)
		{
		for (i=0; i< AObjGetDataSize(bObject, 0)/sizeof(AOBJECTID); i++)
			{
			AObjGetName(bObjPtr[i], lpszString, 256) ;
			objIndex = ListBox_AddString (GetDlgItem (hDlg,SELECTED_OBJ), lpszString) ;
			ListBox_SetItemData (GetDlgItem (hDlg, SELECTED_OBJ), objIndex, bObjPtr[i]) ;
			}
		delete (lpszString) ;

		AObjUnlockData(bObject, 0) ;
		}

	 return ;
	}

void AddItem (HWND hDlg)
	{
	 AOBJECTID  objID ;
	 WORD      nObjects ;
	 char * lpszString ;
	 int *  SelectedIndexes ;
	 DWORD     objectIndex ;
	 int       i ;
	 HWND   hCtrlAvailableObj=GetDlgItem(hDlg, AVAILABLE_OBJ) ;
	 HWND   hCtrlSelectedObj=GetDlgItem(hDlg, SELECTED_OBJ);

	 nObjects = ListBox_GetSelCount (hCtrlAvailableObj) ;
	 if (nObjects == 0 ) return ;

	 SelectedIndexes = new int[nObjects] ;
	 ListBox_GetSelItems (hCtrlAvailableObj, nObjects, SelectedIndexes) ;
	 lpszString = new char[256] ;

	 for (i = 0; i < nObjects; i++)
		{
		objID = (AOBJECTID) ListBox_GetItemData (hCtrlAvailableObj, SelectedIndexes[i]) ;
		ListBox_GetText (hCtrlAvailableObj, SelectedIndexes[i], lpszString) ;
		ListBox_InsertString (hCtrlSelectedObj, -1, lpszString) ;
		ListBox_SetItemData (hCtrlSelectedObj, objectIndex, objID) ;
		}
	 delete(SelectedIndexes) ;
	 delete(lpszString) ;
	 return ;
	}

void RemoveItem(HWND hDlg)
	{
	 long nObjects ;
	 long Index ;
	 HWND   hCtrlSelectedObj=GetDlgItem(hDlg, SELECTED_OBJ);

	 EnableWindow(GetDlgItem(hDlg, removeB), FALSE) ;
	 EnableWindow(GetDlgItem(hDlg, editB), FALSE) ;
	 nObjects = ListBox_GetSelCount (hCtrlSelectedObj) ;
	 if (nObjects!=0&&(0!=ListBox_GetCount(hCtrlSelectedObj)))
		{
		while (ListBox_GetSelItems(hCtrlSelectedObj, 1, &Index))
		  {
			ListBox_DeleteString (hCtrlSelectedObj, Index) ;
		  }
		ListBox_SetCaretIndex (hCtrlSelectedObj, 0) ;
		}
	 SetFocus(GetDlgItem (hDlg,SELECTED_OBJ)) ;
}
void EditItem(HWND hDlg)
	{
	long nObjects ;
	long Index ;
	AOBJECTID theObject ;
	HWND   hCtrlSelectedObj=GetDlgItem(hDlg, SELECTED_OBJ) ;

	nObjects = ListBox_GetSelCount (hCtrlSelectedObj) ;
	nObjects = SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETSELCOUNT, 0, 0L) ;
	if (nObjects != 1) return ;

	ListBox_GetSelItems (hCtrlSelectedObj, 1, &Index) ;
	theObject = (AOBJECTID)ListBox_GetItemData (hCtrlSelectedObj, Index) ;
	AObjEdit(theObject) ;
	return ;
}
//
//saveObj -- save the data of Group object
//
void saveObj(HWND hDlg, AOBJECTID bObject)
	{
	LPOBJECTID  bObjPtr ;
	long        sizeObj ;
	int         i ;
	HWND        hCtrlSelectedObj=GetDlgItem(hDlg, SELECTED_OBJ) ;

	sizeObj = ListBox_GetCount (hCtrlSelectedObj) ;

	AObjResizeData(bObject, 0, sizeObj*sizeof(AOBJECTID)) ;
	bObjPtr = (LPOBJECTID) AObjLockData(bObject, 0) ;
	for (i=0; i<sizeObj; i++)
		bObjPtr[i] = (AOBJECTID) ListBox_GetItemData (hCtrlSelectedObj, 1) ;
	if (bObjPtr != NULL)
		AObjUnlockData(bObject, 0) ;
	AObjSetData (bObject, 0, bObjPtr, sizeObj*sizeof(AOBJECTID)) ;

	return ;
	}

