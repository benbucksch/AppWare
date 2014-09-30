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
#include <windowsx.h>
#include <memory.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>
#include <o_pict.h>
#include <o_list.h>
#include <helpids.h>

#include "groupobj.h"

/////////////////////////////////////////////////////////////////////////
//Function:    doValidateObjectIDs
//Description: Check whether the Object ID is validate or not
/////////////////////////////////////////////////////////////////////////
void doValidateObjectIDs (AOBJECTID gObject, pAObjMessage theSystem)
{
	AOBJECTID  *lpObjects ;
	int           nObjects, nObjectsRemaining ;
  int           i, j;
	BOOL          changed ;

	lpObjects = (AOBJECTID *) AObjLockData(gObject, OBJECT_INDEX) ;
	nObjects = (int) AObjGetDataSize (gObject, OBJECT_INDEX) /sizeof(AOBJECTID) ;
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
		AObjUnlockData(gObject, OBJECT_INDEX) ;
	AObjResizeData( gObject, OBJECT_INDEX, nObjectsRemaining*sizeof(AOBJECTID)) ;
}
/////////////////////////////////////////////////////////////////////////
//Function:    CheckObjID
//Description: Check whether the object ID  already exit in the subject
/////////////////////////////////////////////////////////////////////////
void CheckObjID(AOBJECTID gObject)
{
	AOBJECTID  *lpObjects ;
	int          nObjects, nObjectsRemaining ;
	int          i, j;

	lpObjects = (AOBJECTID *) AObjLockData(gObject, OBJECT_INDEX) ;
	nObjects =(int) AObjGetDataSize (gObject, OBJECT_INDEX) /sizeof(AOBJECTID) ;
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
		AObjUnlockData(gObject, OBJECT_INDEX) ;
	AObjResizeData( gObject, OBJECT_INDEX, nObjectsRemaining*sizeof(AOBJECTID)) ;
}
/////////////////////////////////////////////////////////////////////////
//Function:    GroupObjectProc
//Description: Call back function
/////////////////////////////////////////////////////////////////////////
extern "C"
BOOL CALLBACK GroupObjectProc (HWND hDlg, UINT message,
								 WPARAM wParam, LPARAM lParam)
	{
	AOBJECTID hobjGroup ;
	int      Index ;
	AOBJECTID theObj ;

	switch (message)
		{
		case WM_INITDIALOG:
			//AUtlSetTitleFont(OTYPE_GROUP, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			hobjGroup = (AOBJECTID)lParam ;
			InitDialog(hDlg, hobjGroup) ;
			return FALSE ;

		case WM_COMMAND: 
			if (LOWORD(wParam) >= textB && LOWORD(wParam) <= otherB)
				{
				CheckRadioButton (hDlg, textB, otherB, LOWORD(wParam)) ;
				UpdateObjectList(hDlg, LOWORD(wParam)) ;
				return TRUE ;
				}

			switch (LOWORD(wParam))
				{
				case IDD_OBJECTSLIST:
					//switch (HIWORD (lParam))
					switch(HIWORD(wParam))
						{
						case LBN_SELCHANGE:
							EnableWindow(GetDlgItem(hDlg, addB), TRUE) ;
							return TRUE ;

						case LBN_DBLCLK:
							AddItem(hDlg) ;
							return TRUE ;
						}
					break ;

				case IDD_FIELDSLIST:
					switch (HIWORD (wParam))
						{						  
						case LBN_SELCHANGE:
							 EnableWindow (GetDlgItem(hDlg, removeB), TRUE) ;
							 //if (SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELCOUNT, 0, 0L)==1)
							 if (ListBox_GetSelCount (GetDlgItem(hDlg, IDD_FIELDSLIST))==1)
								EnableWindow (GetDlgItem(hDlg, editB), TRUE) ;
                             else
								EnableWindow (GetDlgItem(hDlg, editB), FALSE) ;
							 return TRUE ;

						case LBN_DBLCLK:
							 Index = (int)ListBox_GetCurSel(GetDlgItem(hDlg, IDD_FIELDSLIST)) ;
							 theObj = (AOBJECTID)ListBox_GetItemData(GetDlgItem (hDlg,IDD_FIELDSLIST), Index) ; 
							 //theObj =(AOBJECTID) SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETITEMDATA, Index, 0) ;
							 return (AObjEdit(theObj) == A_MODIFIED);
						}
					break ;

				case addB:
					AddItem(hDlg) ;
					//EnableWindow (GetDlgItem(hDlg, addB), FALSE) ;
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
					hobjGroup =(AOBJECTID)GetWindowLong (hDlg, DWL_USER) ;
					saveObj (hDlg, hobjGroup) ;
					EndDialog (hDlg, TRUE) ;
					return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;

				case IDHELP:
					WinHelp (hDlg, "awusgrup.hlp", HELP_CONTEXT, HELPID_OBJD) ;
                    return TRUE ;
				}
		}
	 return FALSE ;
	 }
/////////////////////////////////////////////////////////////////////////
//Function:    UpdateObjectList
//Description: Update the Available object list
/////////////////////////////////////////////////////////////////////////
void UpdateObjectList (HWND hDlg, int buttonID)
	{
	LONG      theGetType ;
	ATYPEID    IDtype;

	// clear all strings from the list box
	//SendDlgItemMessage(hDlg, IDD_OBJECTSLIST, LB_RESETCONTENT, 0, 0) ;
	ListBox_ResetContent (GetDlgItem (hDlg, IDD_OBJECTSLIST)) ;
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
			 	IDtype!=OTYPE_LIST)
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
/////////////////////////////////////////////////////////////////////////
//Function:    DisplayObj
//Description: Display the text of the object
/////////////////////////////////////////////////////////////////////////
void DisplayObj(HWND hDlg, ATYPEID IDtype)
	{
	 LONG      theGetObj ;
	 AOBJECTID  objID ;
	 int       objIndex ;
	 LPSTR     text ;

	 text = new char[256] ;
	 theGetObj = AOBJ_GETFIRSTKEY ;
	 while((objID=AObjGetNextObject(IDtype, &theGetObj))!= NULL)
		  {
		   AObjGetName(objID, text, 256) ;
		   //objIndex = (DWORD)SendDlgItemMessage(hDlg, IDD_OBJECTSLIST, LB_ADDSTRING, 0, (long)text);
			 objIndex = (int)ListBox_AddString (GetDlgItem(hDlg, IDD_OBJECTSLIST), (LPCSTR)text) ;
			 ListBox_SetItemData (GetDlgItem (hDlg, IDD_OBJECTSLIST), objIndex, (long)objID) ;
			 //SendDlgItemMessage(hDlg, IDD_OBJECTSLIST, LB_SETITEMDATA, (WPARAM)objIndex, (long)objID) ;
		  }
	 delete (text) ;
	 return ;
	}
/////////////////////////////////////////////////////////////////////////
//Function:   InitDialog
//Description: Initialize the dialog box
/////////////////////////////////////////////////////////////////////////
void InitDialog ( HWND hDlg, AOBJECTID gObject)
	{
	 LPOBJECTID     gObjPtr ;
	 DWORD           objIndex ;
	 char *       lpszString ;
	 char         	 buffer[OBJECTNAMESIZE] ;
	 char			szType[256];
	 int             i ;

	 ATypeGetName(OTYPE_GROUP, szType, sizeof(szType));
	 strcat(szType, ": ");
	 AObjGetName(gObject, buffer, OBJECTNAMESIZE) ;
	 strcat(szType, buffer);
	 SetWindowText(hDlg, szType/*buffer*/) ;

	 lpszString = new char[256] ;

	 // check the "Text" radio button
	 CheckRadioButton (hDlg, textB, otherB, textB) ;

	 // initialize the Object Selection list
	 UpdateObjectList (hDlg, textB) ;

	 // disable the remove and edit buttons
	 EnableWindow(GetDlgItem(hDlg, removeB), FALSE) ;
	 EnableWindow (GetDlgItem(hDlg, editB), FALSE) ;

	 // initialize the object list
	 //SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_RESETCONTENT, 0, 0) ;
	 ListBox_ResetContent (GetDlgItem(hDlg, IDD_FIELDSLIST)) ;
	 gObjPtr = (LPOBJECTID) AObjLockData(gObject, 0) ;
	 if (gObjPtr != NULL)
	 {
		for (i=0; i< AObjGetDataSize(gObject, 0)/sizeof(AOBJECTID); i++)
		{
			AObjGetName(gObjPtr[i], lpszString, 255) ;
			//objIndex = SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_ADDSTRING, 0, (long)lpszString) ;
			//SendDlgItemMessage(hDlg,IDD_FIELDSLIST, LB_SETITEMDATA, (WPARAM)objIndex, (long)gObjPtr[i]) ;
			objIndex = ListBox_AddString (GetDlgItem (hDlg, IDD_FIELDSLIST), lpszString) ;
			ListBox_SetItemData (GetDlgItem (hDlg, IDD_FIELDSLIST), objIndex, gObjPtr[i]) ;
		}

		AObjUnlockData(gObject, 0);
	 }
	 delete (lpszString) ;
	 return  ;
	}
/////////////////////////////////////////////////////////////////////////
//Function:    AddItem
//Description: Add a item to Selected object list
/////////////////////////////////////////////////////////////////////////
void AddItem (HWND hDlg)
	{
	 int  		  nObjects ;
	 char   *lpszString ;
	 int       *SelectedIndexes ;
	 DWORD 	  	objectIndex ;
	 AOBJECTID 	theObj ;
	 int		i ;

	 //nObjects = (int) SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETSELCOUNT, 0, 0L) ;
	 nObjects = ListBox_GetSelCount (GetDlgItem(hDlg, IDD_OBJECTSLIST)) ;
	 if (nObjects == 0) return ;

	 SelectedIndexes = new int[nObjects] ;
	 //SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETSELITEMS, (WPARAM)nObjects, (long)SelectedIndexes) ;
	 ListBox_GetSelItems (GetDlgItem (hDlg, IDD_OBJECTSLIST), nObjects, SelectedIndexes) ;
	 lpszString = new char[256] ;
	 for (i = 0; i < nObjects; i++)
		{
		 //SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETTEXT, SelectedIndexes[i], (long) lpszString) ;
		 //theObj =(AOBJECTID)SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETITEMDATA, SelectedIndexes[i], 0) ;
		 //objectIndex = (DWORD)SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_INSERTSTRING, -1, (long)lpszString) ;
		 //SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_SETITEMDATA, (WPARAM)objectIndex, (long)theObj) ;
		 ListBox_GetText (GetDlgItem (hDlg, IDD_OBJECTSLIST),SelectedIndexes[i], lpszString) ;
		 theObj = (AOBJECTID)ListBox_GetItemData (GetDlgItem (hDlg, IDD_OBJECTSLIST),SelectedIndexes[i]) ;
		 objectIndex = ListBox_InsertString (GetDlgItem (hDlg, IDD_FIELDSLIST), -1, lpszString) ;
		 ListBox_SetItemData (GetDlgItem(hDlg, IDD_FIELDSLIST), objectIndex, theObj) ;
		}
	 delete(SelectedIndexes) ;
	 delete(lpszString) ;
	 return ;
	}
/////////////////////////////////////////////////////////////////////////
//Function:    RemoveItem
//Description: remove a item from selected object list
/////////////////////////////////////////////////////////////////////////
void RemoveItem(HWND hDlg)
	{
	 long nObjects ;
	 long Index ;

	 EnableWindow(GetDlgItem(hDlg, removeB), FALSE) ;
	 EnableWindow(GetDlgItem(hDlg, editB), FALSE) ;
	 /*nObjects = SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELCOUNT, 0, 0L) ;
	 if (nObjects!=0 &&(0!=SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETCOUNT, 0, 0)))
	 {
		while(SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELITEMS, 1, (long) &Index) !=0)
	 	{
			SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_DELETESTRING, (WORD)Index, 0) ;
		}
		SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_SETCARETINDEX, 0, 0) ;
     }*/
	 nObjects = ListBox_GetSelCount (GetDlgItem(hDlg, IDD_FIELDSLIST)) ;
	 if(nObjects!=0&&(0!=ListBox_GetCount(GetDlgItem(hDlg, IDD_FIELDSLIST))))
		{
		while(ListBox_GetSelItems (GetDlgItem(hDlg,IDD_FIELDSLIST),1, &Index)!=0)
			{
			ListBox_DeleteString (GetDlgItem(hDlg, IDD_FIELDSLIST), Index) ;
			}
		ListBox_SetCaretIndex (GetDlgItem(hDlg, IDD_FIELDSLIST),0) ;
		}
	 SetFocus(GetDlgItem (hDlg,IDD_FIELDSLIST)) ;
	}
/////////////////////////////////////////////////////////////////////////
//Function:    EditItem
//Description: Edit a item in Selected Object list
/////////////////////////////////////////////////////////////////////////
void EditItem(HWND hDlg)
	{
	 long nObjects ;
	 int  Index ;
	 AOBJECTID theObject ;

	 //nObjects = SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELCOUNT, 0, 0L) ;
	 nObjects = ListBox_GetSelCount (GetDlgItem(hDlg, IDD_FIELDSLIST)) ;
	 if (nObjects != 1) return ;

	 //SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELITEMS, 1, (long)&Index) ;
	 //theObject =(AOBJECTID) SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_GETITEMDATA, Index, 0L) ;
	 ListBox_GetSelItems (GetDlgItem(hDlg, IDD_FIELDSLIST),1, &Index) ;
	 theObject = (AOBJECTID) ListBox_GetItemData (GetDlgItem(hDlg, IDD_FIELDSLIST), Index) ;
	 AObjEdit(theObject) ;
	 return ;
	}
/////////////////////////////////////////////////////////////////////////
//Function:    saveObj
//Description: save the data of Group object
/////////////////////////////////////////////////////////////////////////
void saveObj(HWND hDlg, AOBJECTID gObject)
	{
	 LPOBJECTID gObjPtr ;
	 long        sizeObj ;
	 int         i ;

	 //sizeObj = SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETCOUNT, 0, 0) ;
	 sizeObj = ListBox_GetCount (GetDlgItem (hDlg, IDD_FIELDSLIST)) ;

	 AObjResizeData(gObject, 0, sizeObj*sizeof(AOBJECTID)) ;
	 gObjPtr = (LPOBJECTID) AObjLockData(gObject, 0) ;

	 if (gObjPtr != NULL)
		{
		for (i=0; i<sizeObj; i++)
			//gObjPtr[i] =(AOBJECTID) SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETITEMDATA, (WORD) i, 0) ;
			gObjPtr[i] = (AOBJECTID) ListBox_GetItemData (GetDlgItem(hDlg, IDD_FIELDSLIST), i) ;

		AObjUnlockData(gObject, 0);
		}

	AObjSetData (gObject, 0, gObjPtr, sizeObj*sizeof(AOBJECTID)) ;
	}


