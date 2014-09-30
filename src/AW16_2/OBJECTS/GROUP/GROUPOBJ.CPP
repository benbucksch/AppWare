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

#include "groupobj.h"

/////////////////////////////////////////////////////////////////////////
//Function:    doValidateObjectIDs
//Description: Check whether the Object ID is validate or not
/////////////////////////////////////////////////////////////////////////
void doValidateObjectIDs (OBJECTID gObject, pAObjMessage theSystem)
{
	OBJECTID far *lpObjects ;
	int           nObjects, nObjectsRemaining ;
  int           i, j;
	BOOL          changed ;

	lpObjects = (OBJECTID far*) AObjLockData(gObject, OBJECT_INDEX) ;
	nObjects = (int) AObjGetDataSize (gObject, OBJECT_INDEX) /sizeof(OBJECTID) ;
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
	AObjResizeData( gObject, OBJECT_INDEX, nObjectsRemaining*sizeof(OBJECTID)) ;
}
/////////////////////////////////////////////////////////////////////////
//Function:    CheckObjID
//Description: Check whether the object ID  already exit in the subject
/////////////////////////////////////////////////////////////////////////
void CheckObjID(OBJECTID gObject)
{
	OBJECTID far *lpObjects ;
	int          nObjects, nObjectsRemaining ;
	int          i, j;

	lpObjects = (OBJECTID far*) AObjLockData(gObject, OBJECT_INDEX) ;
	nObjects =(int) AObjGetDataSize (gObject, OBJECT_INDEX) /sizeof(OBJECTID) ;
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
	AObjResizeData( gObject, OBJECT_INDEX, nObjectsRemaining*sizeof(OBJECTID)) ;
}
/////////////////////////////////////////////////////////////////////////
//Function:    GroupObjectProc
//Description: Call back function
/////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export GroupObjectProc (HWND hDlg, WORD message,
								 WORD wParam, LONG lParam)
	{
	OBJECTID hobjGroup ;
	WORD     Index ;
	OBJECTID theObj ;

	switch (message)
		{
		case WM_INITDIALOG:
			AUtlSetTitleFont(OTYPE_GROUP, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			hobjGroup = (OBJECTID)lParam ;
			InitDialog(hDlg, hobjGroup) ;
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
				case IDD_OBJECTSLIST:
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

				case IDD_FIELDSLIST:
					switch (HIWORD (lParam))
						{
						case LBN_SELCHANGE:
							 EnableWindow (GetDlgItem(hDlg, removeB), TRUE) ;
							 if (SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELCOUNT, 0, 0L)==1)
								EnableWindow (GetDlgItem(hDlg, editB), TRUE) ;
                             else
								EnableWindow (GetDlgItem(hDlg, editB), FALSE) ;
							 return TRUE ;

						case LBN_DBLCLK:
							 Index = SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETCURSEL, 0 ,0) ;
							 theObj =(OBJECTID) SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETITEMDATA, Index, 0) ;
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
					hobjGroup =(OBJECTID)GetWindowLong (hDlg, DWL_USER) ;
					saveObj (hDlg, hobjGroup) ;
					EndDialog (hDlg, TRUE) ;
					return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;

				case IDHELP:
					WinHelp (hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Grup) ;
                    return TRUE ;
				}
		}
	 return FALSE ;
	 }
/////////////////////////////////////////////////////////////////////////
//Function:    UpdateObjectList
//Description: Update the Available object list
/////////////////////////////////////////////////////////////////////////
void UpdateObjectList (HWND hDlg, WORD buttonID)
	{
	LONG      theGetType ;
	TYPEID    IDtype;

	// clear all strings from the list box
	SendDlgItemMessage(hDlg, IDD_OBJECTSLIST, LB_RESETCONTENT, 0, 0) ;
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
/////////////////////////////////////////////////////////////////////////
//Function:    DisplayObj
//Description: Display the text of the object
/////////////////////////////////////////////////////////////////////////
void DisplayObj(HWND hDlg, TYPEID IDtype)
	{
	 LONG      theGetObj ;
	 OBJECTID  objID ;
	 DWORD     objIndex ;
	 LPSTR     text ;

	 text = new char[256] ;
	 theGetObj = AOBJ_GETFIRSTKEY ;
	 while((objID=AObjGetNextObject(IDtype, &theGetObj))!= NULL)
		  {
		   AObjGetName(objID, text, 256) ;
		   objIndex = (DWORD)SendDlgItemMessage(hDlg, IDD_OBJECTSLIST, LB_ADDSTRING, 0, (long)text);
			 SendDlgItemMessage(hDlg, IDD_OBJECTSLIST, LB_SETITEMDATA, (WPARAM)objIndex, (long)objID) ;
		  }
	 delete (text) ;
	 return ;
	}
/////////////////////////////////////////////////////////////////////////
//Function:   InitDialog
//Description: Initialize the dialog box
/////////////////////////////////////////////////////////////////////////
void InitDialog ( HWND hDlg, OBJECTID gObject)
	{
	 LPOBJECTID     gObjPtr ;
	 DWORD           objIndex ;
	 char far*       lpszString ;
	 char         	 buffer[OBJECTNAMESIZE] ;
	 int             i ;

	 AObjGetName(gObject, buffer, OBJECTNAMESIZE) ;
	 SetWindowText(hDlg, buffer) ;

	 lpszString = new char[256] ;

	 // check the "Text" radio button
	 CheckRadioButton (hDlg, textB, otherB, textB) ;

	 // initialize the Object Selection list
	 UpdateObjectList (hDlg, textB) ;

	 // disable the remove and edit buttons
	 EnableWindow(GetDlgItem(hDlg, removeB), FALSE) ;
	 EnableWindow (GetDlgItem(hDlg, editB), FALSE) ;

	 // initialize the object list
	 SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_RESETCONTENT, 0, 0) ;
	 gObjPtr = (LPOBJECTID) AObjLockData(gObject, 0) ;
	 if (gObjPtr != NULL)
	 {
		for (i=0; i< AObjGetDataSize(gObject, 0)/sizeof(OBJECTID); i++)
		{
			AObjGetName(gObjPtr[i], lpszString, 255) ;
			objIndex = SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_ADDSTRING, 0, (long)lpszString) ;
			SendDlgItemMessage(hDlg,IDD_FIELDSLIST, LB_SETITEMDATA, (WPARAM)objIndex, (long)gObjPtr[i]) ;
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
	 char far  *lpszString ;
	 int       *SelectedIndexes ;
	 DWORD 	  	objectIndex ;
	 OBJECTID 	theObj ;
	 int		i ;

	 nObjects = (int) SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETSELCOUNT, 0, 0L) ;
	 if (nObjects == 0) return ;

	 SelectedIndexes = new int[nObjects] ;
	 SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETSELITEMS, (WPARAM)nObjects, (long)SelectedIndexes) ;
	 lpszString = new char[256] ;
	 for (i = 0; i < nObjects; i++)
		{
		 SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETTEXT, SelectedIndexes[i], (long) lpszString) ;
		 theObj =(OBJECTID)SendDlgItemMessage (hDlg, IDD_OBJECTSLIST, LB_GETITEMDATA, SelectedIndexes[i], 0) ;
		 objectIndex = (DWORD)SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_INSERTSTRING, -1, (long)lpszString) ;
		 SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_SETITEMDATA, (WPARAM)objectIndex, (long)theObj) ;
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
	 nObjects = SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELCOUNT, 0, 0L) ;
	 if (nObjects!=0 &&(0!=SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETCOUNT, 0, 0)))
	 {
		while(SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELITEMS, 1, (long) &Index) !=0)
	 	{
			SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_DELETESTRING, (WORD)Index, 0) ;
		}
		SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_SETCARETINDEX, 0, 0) ;
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
	 WORD Index ;
	 OBJECTID theObject ;

	 nObjects = SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELCOUNT, 0, 0L) ;
	 if (nObjects != 1) return ;

	 SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETSELITEMS, 1, (long)&Index) ;
	 theObject =(OBJECTID) SendDlgItemMessage(hDlg, IDD_FIELDSLIST, LB_GETITEMDATA, Index, 0L) ;
	 AObjEdit(theObject) ;
	 return ;
	}
/////////////////////////////////////////////////////////////////////////
//Function:    saveObj
//Description: save the data of Group object
/////////////////////////////////////////////////////////////////////////
void saveObj(HWND hDlg, OBJECTID gObject)
	{
	 LPOBJECTID gObjPtr ;
	 long        sizeObj ;
	 int         i ;

	 sizeObj = SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETCOUNT, 0, 0) ;

	 AObjResizeData(gObject, 0, sizeObj*sizeof(OBJECTID)) ;
	 gObjPtr = (LPOBJECTID) AObjLockData(gObject, 0) ;

	 if (gObjPtr != NULL)
		{
		for (i=0; i<sizeObj; i++)
			gObjPtr[i] =(OBJECTID) SendDlgItemMessage (hDlg, IDD_FIELDSLIST, LB_GETITEMDATA, (WORD) i, 0) ;

		AObjUnlockData(gObject, 0);
		}

	AObjSetData (gObject, 0, gObjPtr, sizeObj*sizeof(OBJECTID)) ;
	}


