/////////////////////////////////////////////////////////////////////////////
// File Name:  GroupObj.cpp
// 		Copyright 1989-92 Serius Corporation
// Author:  Sibai Li
// Date:  5/13/92
//
// Description.: Dialog Routine for the Serius Group object:
////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <windows.h>
#include <memory.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>
#include <o_pict.h>
#include <o_list.h>
#include <helpids.h>
#include "bundle.h"

void doValidateObjectIDs (OBJECTID bObject, pAObjMessage theSystem)
{
	LPOBJECTID  lpObjects ;
	WORD        nObjects, nObjectsRemaining ;
	BOOL        changed ;
	int         i, j ;

	lpObjects = (OBJECTID far*) AObjLockData(bObject, OBJECT_INDEX) ;
	nObjects = (WORD)AObjGetDataSize (bObject, OBJECT_INDEX) /sizeof(OBJECTID) ;
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
	AObjResizeData( bObject, OBJECT_INDEX, nObjectsRemaining*sizeof(OBJECTID)) ;
}
void CheckObjID(OBJECTID bObject)
{
	OBJECTID far *lpObjects ;
	int           nObjects, i, j, nObjectsRemaining ;

	lpObjects = (OBJECTID far*) AObjLockData(bObject, OBJECT_INDEX) ;
	nObjects = (int)AObjGetDataSize (bObject, OBJECT_INDEX) /sizeof(OBJECTID) ;
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

	AObjResizeData( bObject, OBJECT_INDEX, nObjectsRemaining*sizeof(OBJECTID)) ;
}


BOOL CALLBACK _export BundleObjectProc (HWND hDlg, WORD message,
										  WORD wParam, LONG lParam)
	{
  OBJECTID hobjBundle ;
	WORD     Index ;
	OBJECTID theObj ;

	switch (message)
		{
		case WM_INITDIALOG:
			AUtlSetTitleFont(OTYPE_BUNDLE, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
			SetWindowLong (hDlg, DWL_USER, lParam) ;
			hobjBundle = (OBJECTID)lParam ;
			InitDialog(hDlg, hobjBundle) ;
			return FALSE ;

		case WM_COMMAND:
			if (wParam >= textB && wParam <= otherB)
				{
				CheckRadioButton (hDlg, textB, otherB, wParam) ;
				UpdateObjectList(hDlg, wParam) ;
				return TRUE ;
				}

			switch (wParam)
				{
				case AVAILABLE_OBJ:
					switch (HIWORD (lParam))
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
					switch (HIWORD (lParam))
						{
						case LBN_SELCHANGE:
							 EnableWindow (GetDlgItem(hDlg, removeB), TRUE) ;
							 if (SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETSELCOUNT, 0, 0L)==1)
								EnableWindow (GetDlgItem(hDlg, editB), TRUE) ;
                             else
								EnableWindow (GetDlgItem(hDlg, editB), FALSE) ;
							 return TRUE ;

						case LBN_DBLCLK:
							 Index = SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETCURSEL, 0 ,0) ;
							 theObj = (OBJECTID) SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETITEMDATA, Index, 0) ;
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
          hobjBundle = (OBJECTID) GetWindowLong (hDlg, DWL_USER) ;
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
	TYPEID    IDtype;

	// clear all strings from the list box
	SendDlgItemMessage(hDlg, AVAILABLE_OBJ, LB_RESETCONTENT, 0, 0) ;
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
void DisplayObj(HWND hDlg, TYPEID IDtype)
	{
	 LONG    theGetObj ;
	 OBJECTID  objID ;
	 DWORD     objIndex ;
	 LPSTR     text ;

	 text = new char[256] ;
	 theGetObj = AOBJ_GETFIRSTKEY  ;
	 while((objID=AObjGetNextObject(IDtype,&theGetObj))!= NULL)
		  {
		   AObjGetName(objID, text, 256) ;
		   objIndex = SendDlgItemMessage(hDlg, AVAILABLE_OBJ, LB_ADDSTRING, 0, (long)text);
		   SendDlgItemMessage(hDlg, AVAILABLE_OBJ, LB_SETITEMDATA, (WPARAM)objIndex, (LPARAM)objID) ;
		  }
	 delete (text) ;
	 return ;
	}

void InitDialog ( HWND hDlg, OBJECTID bObject)
	{
	 LPOBJECTID     bObjPtr ;
	 DWORD           objIndex ;
	 char far*       lpszString ;
	 char         	 buffer[OBJECTNAMESIZE] ;
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
	 SendDlgItemMessage(hDlg, SELECTED_OBJ, LB_RESETCONTENT, 0, 0) ;
	 bObjPtr = (LPOBJECTID) AObjLockData(bObject, 0) ;

	 if (bObjPtr != NULL)
		{
		for (i=0; i< AObjGetDataSize(bObject, 0)/sizeof(OBJECTID); i++)
			{
			AObjGetName(bObjPtr[i], lpszString, 256) ;
			objIndex = SendDlgItemMessage(hDlg, SELECTED_OBJ, LB_ADDSTRING, 0, (long)lpszString) ;
			SendDlgItemMessage(hDlg,SELECTED_OBJ, LB_SETITEMDATA, (WPARAM)objIndex, (LPARAM)bObjPtr[i]) ;
			}
		delete (lpszString) ;

		AObjUnlockData(bObject, 0) ;
		}

	 return ;
	}

void AddItem (HWND hDlg)
	{
	 OBJECTID  objID ;
	 WORD      nObjects ;
	 char far* lpszString ;
	 int far*  SelectedIndexes ;
	 DWORD     objectIndex ;
	 int       i ;

	 nObjects = SendDlgItemMessage (hDlg, AVAILABLE_OBJ, LB_GETSELCOUNT, 0, 0L) ;
	 if (nObjects == 0 ) return ;

	 SelectedIndexes = new int[nObjects] ;
	 SendDlgItemMessage (hDlg, AVAILABLE_OBJ, LB_GETSELITEMS, nObjects, (long)SelectedIndexes) ;
	 lpszString = new char[256] ;

	 for (i = 0; i < nObjects; i++)
		{
		 objID = (OBJECTID) SendDlgItemMessage (hDlg, AVAILABLE_OBJ, LB_GETITEMDATA, SelectedIndexes[i], 0L) ;
		 SendDlgItemMessage (hDlg, AVAILABLE_OBJ, LB_GETTEXT, SelectedIndexes[i], (long) lpszString) ;
		 objectIndex = SendDlgItemMessage(hDlg, SELECTED_OBJ, LB_INSERTSTRING, -1, (long)lpszString) ;
		 SendDlgItemMessage(hDlg, SELECTED_OBJ, LB_SETITEMDATA, (WPARAM)objectIndex, (LPARAM)objID) ;
		}
	 delete(SelectedIndexes) ;
	 delete(lpszString) ;
	 return ;
	}

void RemoveItem(HWND hDlg)
	{
	 long nObjects ;
	 long Index ;

	 EnableWindow(GetDlgItem(hDlg, removeB), FALSE) ;
	 EnableWindow(GetDlgItem(hDlg, editB), FALSE) ;
	 nObjects = SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETSELCOUNT, 0, 0L) ;
	 if (nObjects!=0 &&(0!=SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETCOUNT, 0, 0)))
	 {
	 	while(SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETSELITEMS, 1, (long) &Index) !=0)
	 	{
			SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_DELETESTRING, (WORD)Index, 0) ;
		}
		SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_SETCARETINDEX, 0, 0) ;
     }
	 SetFocus(GetDlgItem (hDlg,SELECTED_OBJ)) ;
    }
void EditItem(HWND hDlg)
	{
	 long nObjects ;
	 long Index ;
	 OBJECTID theObject ;

	 nObjects = SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETSELCOUNT, 0, 0L) ;
	 if (nObjects != 1) return ;

	 SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETSELITEMS, 1, (long)&Index) ;
	 theObject =(OBJECTID) SendDlgItemMessage(hDlg, SELECTED_OBJ, LB_GETITEMDATA, (WPARAM)Index, 0L) ;
	 AObjEdit(theObject) ;
	 return ;
	}
//
//saveObj -- save the data of Group object
//
void saveObj(HWND hDlg, OBJECTID bObject)
	{
	LPOBJECTID bObjPtr ;
	long        sizeObj ;
	int         i ;

	sizeObj = SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETCOUNT, 0, 0) ;

	AObjResizeData(bObject, 0, sizeObj*sizeof(OBJECTID)) ;
	bObjPtr = (LPOBJECTID) AObjLockData(bObject, 0) ;
	for (i=0; i<sizeObj; i++)
		bObjPtr[i] =(OBJECTID) SendDlgItemMessage (hDlg, SELECTED_OBJ, LB_GETITEMDATA, (WORD) i, 0) ;

	if (bObjPtr != NULL)
		AObjUnlockData(bObject, 0) ;
	AObjSetData (bObject, 0, bObjPtr, sizeObj*sizeof(OBJECTID)) ;

	return ;
	}

