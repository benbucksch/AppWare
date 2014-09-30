////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"conn.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <stdio.h>
#include <alloc.h>
#include <string.h>
#include <windowsx.h>
#include "com_dlg.h"
#include "commObj.h"

WNDPROC defOldEditProc ;
void HighlightAddedItem (HWND hDlg, int Index);
void ShowButtonStatus (HWND, BOOL) ;
void EnableCompareGroup (HWND, BOOL) ;
void SetChkBtnCombox (HWND, int, int, BOOL) ;
void ShowDlgItem(HWND, int, BOOL) ;
int  HexToDec (int) ;
void TextToHex (LPSTR, LPSTR, LPSTR, LONG) ;
void StringToHex (LPSTR lpBuffer, LPSTR lpText, char *Format, int step) ;
void DisplayListString (HWND hwnd, LPSTR string, char mark, int signalNo, int Index) ;
void SetCompareBits (HWND, int) ;
void EditSignalString (HWND, WPARAM) ;
void ClearStringEdit (HWND hwnd) ;
long ChangeHexToDec (char* lpDesc, char* lpSource) ;
LPSTR StrChr (LPSTR, char, int) ;
long StrSpn (LPSTR, LPSTR, int) ;
LPSTR StrCmp (LPSTR, LPSTR, int, int*) ;
EDIT_SIGNAL::EDIT_SIGNAL(AOBJECTID oiObj):CONN(oiObj)
{
	SIGNALINFO *signalInfo1, *signalInfo2 ;
	LPSTR       signalBlock1, signalBlock2 ;
	int        InfoSize, BlockSize ;

	marktable[TRUE]=ACTMARK ;
	marktable[FALSE] =DACTMARK ;
  signal = &data0->signal ;

	signalInfo1 = (SIGNALINFO*) AObjLockData (theObject, SGNLINFO) ;
	if (signalInfo1)
	  {
	  InfoSize = AObjGetDataSize (theObject, SGNLINFO) ;
		hSignalInfo = AMemAllocate (InfoSize) ;
		signalInfo2 = (SIGNALINFO*) AMemLock(hSignalInfo) ;
		if (signalInfo2)
		  {
			CopyMemory (signalInfo2, signalInfo1, InfoSize) ;
			AMemUnlock (hSignalInfo) ;
			}
		AObjUnlockData (theObject, SGNLINFO) ;
	  }
	else
	  hSignalInfo = NULL ;

	signalBlock1 = (LPSTR) AObjLockData (theObject, SGNLBLOCK) ;
	if (signalBlock1)
	  {
		BlockSize = AObjGetDataSize (theObject, SGNLBLOCK) ;
		hSignalBlock = AMemAllocate (BlockSize) ;
		signalBlock2 = (LPSTR) AMemLock (hSignalBlock) ;
		if (signalBlock2)
		  {
			CopyMemory (signalBlock2, signalBlock1, BlockSize) ;
			AMemUnlock (hSignalBlock) ;
			}
		AObjUnlockData (theObject, SGNLBLOCK) ;
		}
	else
		hSignalBlock = NULL ;
}
EDIT_SIGNAL::~EDIT_SIGNAL(void)
{
	if (hSignalInfo)
		AMemFree (hSignalInfo) ;
	if (hSignalBlock)
		AMemFree (hSignalBlock) ;
}
void EDIT_SIGNAL::ChangeViewMode (HWND hDlg, LONG wParam)
{
	SIGNALINFO* si, *signalInfo ;
	LPSTR       sb, signalBlock ;
	AMEMBLOCKID  BlockID ;
	LPSTR       lpStr ;
	char        Format[6]  ;
	int         step ;
	int         i ;
	int        nCounts ;
	int        *SelectedIndexes ;

	switch (wParam)
		{
		case idASCII:
			step= 0 ;	
    	currentView = SGNLASCII ; 	
			break ;	

		case idByte:	
			lstrcpy(Format, "%02X") ;	
			step = 1 ;	
    	currentView = SGNLBYTE ; 	
			break ;	

		case idWord:	
			lstrcpy (Format, "%02X") ;	
			step = 2 ;	
				currentView = SGNLWORD ;
				break ;

			case idLongword:
				lstrcpy (Format, "%02X") ;
				step = 4 ;
     		currentView = SGNLLONG ;
				break ;		 		 		 	
		}
	si = signalInfo = (SIGNALINFO*) AMemLock (hSignalInfo) ; //AObjLockData (theObject, SGNLINFO) ;
	sb = signalBlock = (LPSTR) AMemLock (hSignalBlock) ; //ObjLockData (theObject, SGNLBLOCK) ;
	if (si&&sb)
   	{
//		nCounts = (WORD)SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0, 0L);
//		nCounts = ListBox_GetSelCount(GetDlgItem(hDlg, idList));
		nCounts = SendDlgItemMessage(hDlg, idList, LB_GETSELCOUNT, 0L, 0L);


		if (nCounts >0) //Save the current selected items
			{
			SelectedIndexes = new int[nCounts] ;
//			SendDlgItemMessage(hDlg, idList, LB_GETSELITEMS, nCounts, (long)SelectedIndexes) ;
//			ListBox_GetSelItems(GetDlgItem(hDlg, idList), nCounts, SelectedIndexes) ;
			SendDlgItemMessage(hDlg, idList, LB_GETSELITEMS, (WPARAM)(int)nCounts, (LPARAM)(int *)SelectedIndexes) ;	

			}
//		SendDlgItemMessage (hDlg, idList, LB_RESETCONTENT, 0, 0L) ;
//		ListBox_ResetContent(GetDlgItem(hDlg, idList));
		SendDlgItemMessage (hDlg, idList, LB_RESETCONTENT, 0, 0L) ;

		BlockID = AMemAllocate (si->signalLen) ;
		for (i=0; i<signal->signalCount; i++, si++)
			{
			AMemSetSize (BlockID, 3*si->signalLen) ;
			lpStr = (LPSTR) AMemLock (BlockID) ;
			if (lpStr)
				{
				if (step) StringToHex (lpStr, (LPSTR)sb, Format, step) ;
				if (!step) CopyMemory(lpStr, (LPSTR)sb, si->signalLen) ;
				sb += si->signalLen ;
				DisplayListString (hDlg, lpStr, marktable[si->active], si->signalIndex+1, -1) ;
				AMemUnlock (BlockID) ;
				}
			}
		AMemFree (BlockID) ;
		 //Restore the current selected items
		if (nCounts>0&&SelectedIndexes)
		  {
			for (i=0; i<nCounts; i++)
			  {
//				SendDlgItemMessage (hDlg, idList, LB_SETSEL, TRUE, MAKELPARAM(SelectedIndexes[i],0)) ;
//				ListBox_SetSel(GetDlgItemMessage(hDlg, idList), TRUE, SelectedIndexes[i]);
				SendDlgItemMessage (hDlg, idList, LB_SETSEL, (WPARAM)TRUE, MAKELPARAM(SelectedIndexes[i],0)) ;
		  	}
			delete [] SelectedIndexes ;
			}
		}
	ClearStringEdit (hDlg) ;
	if (nCounts==1)
//		SendMessage (hDlg, WM_COMMAND, idList, MAKELPARAM (GetDlgItem (hDlg, idList), CBN_SELCHANGE)) ; 
		SendMessage (hDlg, WM_COMMAND, MAKEWPARAM(idList, CBN_SELCHANGE), (LPARAM)GetDlgItem (hDlg, idList)) ; 
	if (signalInfo)
		AMemUnlock (hSignalInfo) ;
	if (signalBlock)
		AMemUnlock(hSignalBlock) ;
}
//
//
//
BOOL EDIT_SIGNAL::InitSignalDialog(HWND hwnd)
{
	char buffer[OBJECTNAMESIZE] ;

	AObjGetName(theObject, buffer, OBJECTNAMESIZE) ;
	SetWindowText(hwnd, buffer) ;
	AUtlSetTitleFont(OTYPE_CONN, NULL, GetDlgItem(hwnd, IDD_OBJECT));
	
	CheckRadioButton (hwnd, idASCII, idLongword, idASCII) ;
	currentView = SGNLASCII ;

    //Set signal list and current signal
  SetSignalList (hwnd) ;

  //Set check button
  SetComboxContent (hwnd, idFoundStrCon) ;
  SetComboxContent (hwnd, idFoundPosCon) ;
	SetComboxContent (hwnd, idInterceptCon) ;
	SetSignalCheckButton(hwnd) ;

	defOldEditProc = (WNDPROC) GetWindowLong(GetDlgItem(hwnd, idInsString), GWL_WNDPROC) ;
	SetWindowLong (GetDlgItem(hwnd, idInsString), GWL_WNDPROC, (long)EditProc) ;
	ShowButtonStatus (hwnd, FALSE) ;

  return TRUE ;
}
void EDIT_SIGNAL::SetComboxContent (HWND hwnd, int idCombox)
{
  ATYPEID   theTypeId ;
  long     theGetKey=-1 ;
  AOBJECTID oiObject ;
  AOBJECTID oiCombox ;
  int     index ;
  char    *objName ;

  if (idCombox == idFoundPosCon)
    {
    theTypeId = OTYPE_NUMBER ;
    oiCombox = signal->FoundIndex ;
    }
  if (idCombox == idFoundStrCon)
    {
    theTypeId = OTYPE_TEXT ;
    oiCombox = signal->FoundString ;
    }
  if (idCombox == idInterceptCon)
    {
    theTypeId = OTYPE_TEXT ;
    oiCombox = signal->InteText ;
    }

  objName = new char[255] ;
  lstrcpy(objName, "(None)") ;
//  index = (WORD)SendDlgItemMessage (hwnd, idCombox, CB_INSERTSTRING, 0, (long) objName) ;
    ComboBox_InsertString(GetDlgItem(hwnd, idCombox), 0, (long) objName) ;

//  SendDlgItemMessage (hwnd, idCombox, CB_SETITEMDATA, 0, 0) ;
    ComboBox_SetItemData(GetDlgItem(hwnd, idCombox), 0, 0);

	while ((oiObject=AObjGetNextObject(theTypeId, &theGetKey))!=NULL)
    {
    AObjGetName (oiObject, objName, 255) ;
//  index = (WORD)SendDlgItemMessage(hwnd, idCombox, CB_INSERTSTRING, -1, (long)objName) ;
    index = ComboBox_InsertString(GetDlgItem(hwnd, idCombox), -1, (long)objName) ;
//  SendDlgItemMessage (hwnd, idCombox, CB_SETITEMDATA, index, (long)oiObject) ;
    ComboBox_SetItemData(GetDlgItem(hwnd, idCombox), index, (long)oiObject) ;
    if (oiCombox == oiObject)
      {
//     SendDlgItemMessage (hwnd, idCombox, CB_SETCURSEL, index, 0) ;
       ComboBox_SetCurSel(GetDlgItem(hwnd, idCombox), index);
      }
		}

  if (oiCombox ==NULL)
//   SendDlgItemMessage (hwnd, idCombox, CB_SETCURSEL, 0, 0) ;
     ComboBox_SetCurSel(GetDlgItem(hwnd, idCombox), 0) ;

  delete []objName ;
}
void EDIT_SIGNAL::SetSignalCheckButton(HWND hwnd)
{
  SetChkBtnCombox (hwnd, idFoundStr, idFoundStrCon, signal->checkBox&FOUNDSTR) ;
	SetChkBtnCombox (hwnd, idFoundPos, idFoundPosCon, signal->checkBox&FOUNDPOS) ;
	SetChkBtnCombox (hwnd, idIntercept, idInterceptCon, signal->checkBox&INTERCEPT) ;
      
  if (signal->checkBox & INTERCEPT)
    {
   	ShowDlgItem(hwnd, idFoundPos, TRUE) ;
		if (signal->checkBox&FOUNDPOS)
     	ShowDlgItem(hwnd, idFoundPosCon, TRUE) ;
    }
    else
    {
    ShowDlgItem(hwnd, idFoundPos, FALSE) ;
    ShowDlgItem(hwnd, idFoundPosCon, FALSE) ;
    } 
}
void EDIT_SIGNAL::SetSignalList (HWND hDlg)
{
	SIGNALINFO *si,*signalInfo ;
  LPSTR       sb, signalBlock ;
	long        len ;
	int         i ;

	if (signal->signalCount>0)
		{
		si = signalInfo  = (SIGNALINFO*)AMemLock (hSignalInfo) ; //AObjLockData (theObject, SGNLINFO) ;
    sb = signalBlock = (LPSTR) AMemLock (hSignalBlock) ; //AObjLockData (theObject, SGNLBLOCK) ;
		if (si&&sb)
			{
			for (i=0; i<signal->signalCount; i++, si++)
				{
				len=si->signalLen ;
				DisplayListString (hDlg, sb, marktable[si->active], si->signalIndex+1, -1) ;
        SetCompareBits (hDlg, si->CompareBits?idLowerseven:idAlleight) ;
				sb +=(int)len ;
				}
			}
		if (signalInfo)
			AMemUnlock (hSignalInfo) ;
		if (signalBlock)
			AMemUnlock (hSignalBlock) ;
		}
	else
		SetCompareBits (hDlg, idAlleight) ;
	
}
//
//
//
void EDIT_SIGNAL::AddStringToList(HWND hDlg)
{
	int	        signalCount ;
	int         numChars ;
	char*       strs ;
	HWND        hEdit = GetDlgItem (hDlg, idInsString) ;

  signalCount = signal->signalCount  ;
	if (signalCount<249)    //if it is over 250, the over part of signal cannot be display on the screen.
	  {
		numChars = (int)Edit_GetTextLength (hEdit) ;
		if (numChars>250)
		  {
		 	numChars=250 ;
			MessageBox (hDlg, "The length of signal string can not greater 250.", "Warning", MB_OK) ;
			}
		strs = new char [++numChars] ;
		if (strs)
  		{
			Edit_GetText (hEdit, (LPSTR)strs, numChars) ; 
			signalCount++ ;
			signal->signalCount = signalCount ;
			int Index = FindAddPosition (signalCount) ;
			DisplayListString (hDlg, strs, marktable[numChars>1?TRUE:FALSE], Index+1, Index) ;
			//signal->signalCount = signalCount ;
			numChars = UpdateSignalBlock (strs, Index) ;
			UpdateSignalInfo (hDlg, Index, numChars) ;
			delete[] strs ;
			HighlightAddedItem (hDlg, Index) ;
			}
	  }
	else
		MessageBeep(-1) ;
}
int EDIT_SIGNAL::UpdateSignalBlock (char* buffer, int Index)
{
	LPSTR signalBlock ;
    char *string;      
	int	  signalCount=signal->signalCount ;
  	int  blockSize= 0, blockSize1=0 ; 
	int  numChars = lstrlen(buffer)+1 ;
	int   i ;

	string = new char[(numChars+1)/2];
	if (currentView!=SGNLASCII)
	  {
		numChars = ChangeHexToDec (string, buffer)+1;
		lstrcpy (buffer, string) ;
		}
	if (hSignalBlock)
		blockSize = AMemGetSize (hSignalBlock ) ; 
	else
		hSignalBlock = AMemAllocate (numChars) ;
	if (AMemSetSize (hSignalBlock, blockSize+numChars))
		{
		signalBlock = (LPSTR) AMemLock (hSignalBlock) ; 
		if (signalBlock)
		  {
		  for (i=0;i<Index; i++ )
		    {
				signalBlock +=_fstrlen (signalBlock)+1 ;
				blockSize1+=_fstrlen (signalBlock)+1 ;
		    }
			if (Index!=signalCount-1)
				MoveMemory (signalBlock+numChars, signalBlock, (DWORD)(blockSize-blockSize1)) ;
			MoveMemory(signalBlock, buffer, (DWORD)numChars) ;
			AMemUnlock (hSignalBlock) ;
   		}
		}
	if (string)
  	delete[]string ; 	

	return (numChars);
}
int EDIT_SIGNAL::FindAddPosition (long signalNum)
{
	SIGNALINFO *signalInfo ;
	SIGNALINFO *tmpSignalInfo ;
	int         signalIndex = 0 ;
	int         i;

	if (hSignalInfo==NULL)
		hSignalInfo = AMemAllocate (sizeof (SIGNALINFO)) ;

	if (AMemSetSize (hSignalInfo, signalNum*sizeof(SIGNALINFO)))
   	{
		signalInfo = (SIGNALINFO*) AMemLock (hSignalInfo) ; 
		if (signalInfo)
		  {
			for (i=0; i<signalNum-1; i++)
			  {
				if (signalInfo->signalIndex !=signalIndex)
					break;
				signalIndex++ ;
				tmpSignalInfo = signalInfo++ ;
				}
			if(i!=signalNum-1)
				MoveMemory (tmpSignalInfo+1, tmpSignalInfo, (DWORD)(signalNum-i)*sizeof(SIGNALINFO)) ;
			AMemUnlock (hSignalInfo) ;
			}
		}
	return(signalIndex);
}
void EDIT_SIGNAL::UpdateSignalInfo (HWND hDlg,int signalIndex, long numChars)
{
	SIGNALINFO *signalInfo ;

	signalInfo = (SIGNALINFO*) AMemLock (hSignalInfo) ;
	if (signalInfo)
	  {
			signalInfo+=signalIndex ;
			if (signalInfo)
			  {
	   		signalInfo->signalIndex = signalIndex ;
		 		signalInfo->signalLen = numChars ;
		 		signalInfo->active = numChars>1?TRUE:FALSE ;
		 		signalInfo->CompareBits = IsDlgButtonChecked (hDlg, idAlleight)? EIGHT: SEVENT;
				}
			AMemUnlock (hSignalInfo) ;
	  }
}
//
//
//
void EDIT_SIGNAL::ChangeStringInList(HWND hDlg)
{
	SIGNALINFO *signalInfo ;
	LPSTR str;
    LPSTR lpTempBlock, signalBlock ;
	int   signalCount ;
	int	  oldStrSize, newStrSize ; 
	int   oldBlockSize, newBlockSize, Size1=0 ;
	int   i ;
	int   Index ;

	if ((signalCount=signal->signalCount)==0) return ;
//	Index = ListBox_GetCurSel(GetDlgItem(hDlg, idList)) ; 
	Index = SendDlgItemMessage(hDlg, idList, LB_GETCURSEL, 0, 0L);

	newStrSize = Edit_GetTextLength (GetDlgItem(hDlg, idInsString)) ;
	if (newStrSize>250)
	  {
		newStrSize = 250 ;
		MessageBox (hDlg, "The length of signal string can not greater 250.", "Warning", MB_OK) ;
		}
	str = (LPSTR) farmalloc (++newStrSize) ;
	Edit_GetText (GetDlgItem (hDlg, idInsString), str, newStrSize) ;

	oldBlockSize = AMemGetSize (hSignalBlock) ;
	signalInfo = (SIGNALINFO*) AMemLock (hSignalInfo) ; 
	if (signalInfo)
	  {
	  for (i=0;i<Index; i++, signalInfo++)
			Size1 +=signalInfo->signalLen ;
		oldStrSize = signalInfo->signalLen ;
		signalInfo->signalLen = newStrSize ;
		signalInfo->active = newStrSize>1?(oldStrSize>1?signalInfo->active:TRUE):FALSE ;
		AMemUnlock (hSignalInfo) ;
	  }  
 	newBlockSize =oldBlockSize + newStrSize - oldStrSize ;
	signalBlock =(LPSTR) AMemLock (hSignalBlock) ;; 
	lpTempBlock = (LPSTR) new char [oldBlockSize] ;
  CopyMemory (lpTempBlock, signalBlock, oldBlockSize) ;
	AMemUnlock (hSignalBlock) ; 
	if (lpTempBlock)
		{
		if(AMemSetSize (hSignalBlock, newBlockSize))
 			{
			signalBlock = (LPSTR) AMemLock (hSignalBlock) ; //AObjLockData (theObject, SGNLBLOCK) ;
			if (signalBlock)
   			{
				CopyMemory (signalBlock, lpTempBlock, Size1) ;
				CopyMemory (signalBlock+Size1, str, newStrSize) ;
   			if (Index != signalCount-1)
					CopyMemory (signalBlock+Size1+newStrSize, lpTempBlock+Size1+oldStrSize, oldBlockSize-Size1-oldStrSize) ;
//				ListBox_DeleteString (GetDlgItem(hDlg, idList), Index) ;
				SendDlgItemMessage(hDlg, idList, LB_DELETESTRING, (WPARAM)Index, 0L);

				DisplayListString (hDlg, str, marktable[signalInfo->active], Index+1, Index) ;
				AMemUnlock (hSignalBlock) ;
				}
			}
   	delete[] lpTempBlock ;
		}
	if (str) farfree(str) ;
//	SendDlgItemMessage (hDlg, idList, LB_SETSEL, TRUE, MAKELPARAM(Index,0)) ;
//	ListBox_SetSel(GetDlgItem(hDlg, idList), TRUE, Index) ;
	SendDlgItemMessage (hDlg, idList, LB_SETSEL, TRUE, MAKELPARAM(Index,0)) ;

//	SendMessage (hDlg, WM_COMMAND, idList, MAKELPARAM (GetDlgItem (hDlg, idList), CBN_SELCHANGE)) ; 
	SendMessage (hDlg, WM_COMMAND, MAKEWPARAM(idList, CBN_SELCHANGE), (LPARAM)GetDlgItem (hDlg, idList)); 
}
//
//
//
void EDIT_SIGNAL::ActivateSignal (HWND hDlg, int idItem) 
{
	SIGNALINFO *signalInfo ;
	LPSTR str ;
	int   index ;
  	int   numChars ;
	BOOL active= (idItem==idActivate)?TRUE:FALSE ; ;

//	index = (WORD)SendDlgItemMessage (hDlg, idList, LB_GETCURSEL, 0, 0) ;
//  index = ListBox_GetCurSel(GetDlgItem(hDlg, idList));
	index = SendDlgItemMessage (hDlg, idList, LB_GETCURSEL, 0L, 0L) ;

	if (index==LB_ERR) 
		return ;

	signalInfo = (SIGNALINFO*) AMemLock (hSignalInfo) ; 
	if (signalInfo)
  	{
		signalInfo+=index ;
		active = (idItem==idList)?(signalInfo->active?FALSE:TRUE):active ;
		if (signalInfo->active != active)
			{
			signalInfo->active = active ;
//			numChars = SendDlgItemMessage (hDlg, idList, LB_GETTEXTLEN, index, 0L) ;
//			numChars = ListBox_GetTextLen(GetDlgItem(hDlg, idList), index) ;
			numChars = SendDlgItemMessage (hDlg, idList, LB_GETTEXTLEN, index, 0L) ;

			str =(LPSTR) new char[numChars] ;
//			SendDlgItemMessage (hDlg, idList, LB_GETTEXT, index, (long)str) ; 
//			ListBox_GetText(GetDlgItem(hDlg, idList), index, (long)str) ; 
			SendDlgItemMessage (hDlg, idList, LB_GETTEXT, index, (LPARAM)(LPCTSTR)str) ; 

//			SendDlgItemMessage (hDlg, idList, LB_DELETESTRING, index, 0L) ;
//			ListBox_DeleteString(GetDlgItem(hDlg, idList), index) ;
			SendDlgItemMessage (hDlg, idList, LB_DELETESTRING, index, 0L) ;

    	*str = marktable[active] ;
//			SendDlgItemMessage (hDlg, idList, LB_INSERTSTRING, index, (long)str) ;
//			ListBox_InsertString(GetDlgItem(hDlg, idList), index, (long)str) ;
			SendDlgItemMessage (hDlg, idList, LB_INSERTSTRING, index, (LPARAM)str) ;

//			SendDlgItemMessage (hDlg, idList, LB_SETSEL, TRUE, MAKELPARAM(index,0)) ;
//			ListBox_SetSel(GetDlgItem (hDlg, idList), TRUE, index) ;
			SendDlgItemMessage (hDlg, idList, LB_SETSEL, TRUE, MAKELPARAM(index,0)) ;

//			SendMessage (hDlg, WM_COMMAND, idList, MAKELPARAM (GetDlgItem(hDlg, idList), CBN_SELCHANGE)) ;
			SendMessage (hDlg, WM_COMMAND, MAKEWPARAM(idList, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, idList)) ; 
			if (active)
				{
				EnableWindow (GetDlgItem(hDlg, idActivate), FALSE);
				EnableWindow (GetDlgItem(hDlg, idDeactivate),TRUE) ;
				}
			else
			  {
			  EnableWindow (GetDlgItem(hDlg, idActivate), TRUE);
				EnableWindow (GetDlgItem(hDlg, idDeactivate),FALSE) ;
			  }
			delete[] str ;
	 	}
		AMemUnlock (hSignalInfo) ; 
		}
}
//
//
//
void EDIT_SIGNAL::SetDisplayText (HWND hDlg, LPSTR lpStr)
{
	int  len = _fstrlen (lpStr) ;
	int  step = currentView/2 ;

	if (len)
  	{
		AMEMBLOCKID BlockID = AMemAllocate (3*len+1) ;
		LPSTR lpPtr = (LPSTR) AMemLock (BlockID) ;
		if (step) StringToHex (lpPtr, lpStr, "%02X", step) ;
		if (!step) CopyMemory(lpPtr, lpStr, len+1) ;
		SetDlgItemText (hDlg, idInsString, lpPtr);
		AMemUnlock (BlockID) ;
		AMemFree (BlockID) ;
		}
	else
		SetDlgItemText (hDlg, idInsString, lpStr) ;
}
//
//
//
void EDIT_SIGNAL::DisplayString(HWND hDlg)
{
	SIGNALINFO *signalInfo ;
  LPSTR       signalBlock ;
	int  i ;
	long BlockSize = 0 ;
	long nCounts ;

//	nCounts = SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT,0, 0L) ;
//	nCounts = ListBox_GetSelCount(GetDlgItem(hDlg, idList));
	nCounts = SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0L, 0L) ;

	if (nCounts!=0)
	  {
		ShowButtonStatus (hDlg, TRUE) ;
		EnableWindow (GetDlgItem(hDlg, idRemove), TRUE) ;
		EnableWindow (GetDlgItem(hDlg, idChange), TRUE) ;
		}
	if (nCounts!= 1)
		{
		ShowButtonStatus (hDlg, FALSE) ;
		return ;
		}

//	int Index = ListBox_GetCurSel (GetDlgItem(hDlg, idList)) ;
	int Index = SendDlgItemMessage(hDlg, idList, LB_GETCURSEL, 0L, 0L);

	signalInfo = (SIGNALINFO*) AMemLock (hSignalInfo) ;
	if (signalInfo)
	  {
	  for (i=0; i<Index; i++,signalInfo++)
   		BlockSize +=signalInfo->signalLen ;
		CheckRadioButton(hDlg, idAlleight, idLowerseven,signalInfo->CompareBits?idLowerseven:idAlleight) ;
		if (signalInfo->signalLen>1)
		  {
			EnableWindow (GetDlgItem(hDlg, idActivate), signalInfo->active^TRUE);
			EnableWindow (GetDlgItem(hDlg, idDeactivate),signalInfo->active) ;
			}
		else
		  {
		  EnableWindow (GetDlgItem(hDlg, idActivate), FALSE);
			EnableWindow (GetDlgItem(hDlg, idDeactivate),FALSE) ;
		  }

		AMemUnlock (hSignalInfo) ;
	  }
  signalBlock = (LPSTR) AMemLock (hSignalBlock) ; 
	if (signalBlock)
  	{
		signalBlock +=(long)BlockSize ;
		SetDisplayText (hDlg, signalBlock) ;
		AMemUnlock (hSignalBlock) ;
		}
	SetFocus (GetDlgItem (hDlg, idInsString)) ;
//	SendMessage (hDlg, WM_COMMAND, idInsString, MAKELPARAM(GetDlgItem (hDlg, idInsString), EN_SETFOCUS)) ; 
	SendMessage (hDlg, WM_COMMAND, MAKEWPARAM(idInsString, EN_SETFOCUS), (LPARAM)GetDlgItem (hDlg, idInsString)) ; 
	return ;
}
//
//
//
void EDIT_SIGNAL::RemoveStringsFromList (HWND hDlg)
{
	SIGNALINFO *si ;
	LPSTR       sb ;
	int        nCounts ;
	int         BlockSize=0, oldBlockSize ;
	int        newSignalCounts= 0, oldSignalCounts ;
  long        index ;
  int        *SelectedIndexes ;
	WORD        i, j;

//  nCounts = (WORD)SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0, 0L);
//	nCounts = ListBox_GetSelCount (GetDlgItem(hDlg, idList)) ;
    nCounts = SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0L, 0L);

	if (nCounts >0)
		{
    //Remove the selected strings from the list
		SelectedIndexes = new int[nCounts] ;
//		nCounts = SendDlgItemMessage(hDlg, idList, LB_GETSELITEMS, nCounts, (long)SelectedIndexes) ;
//		nCounts = ListBox_GetSelItems(GetDlgItem(hDlg, idList), nCounts, (long)SelectedIndexes) ;
		nCounts = SendDlgItemMessage(hDlg, idList, LB_GETSELITEMS, nCounts, (LPARAM)(int *)SelectedIndexes) ;

//		while(SendDlgItemMessage (hDlg, idList, LB_GETSELITEMS, 1, (long) &index) !=0)
//		while(ListBox_GetSelItems(GetDlgItem(hDlg, idList), 1, (long) &index) !=0)
		while(SendDlgItemMessage (hDlg, idList, LB_GETSELITEMS, 1, (LPARAM)(int *) &index) !=0)
			{
//			SendDlgItemMessage (hDlg, idList, LB_DELETESTRING, (WORD)index, 0) ;
//			ListBox_DeleteString(GetDlgItem(hDlg, idList), index);
			SendDlgItemMessage (hDlg, idList, LB_DELETESTRING, (WPARAM)index, 0L) ;
			}
    //Handle the string data block
		oldSignalCounts = signal->signalCount ;
		si = (SIGNALINFO*) AMemLock (hSignalInfo) ; 
		sb = (LPSTR) AMemLock (hSignalBlock) ;      
		oldBlockSize = AMemGetSize (hSignalBlock) ; 
		if (si)
			{
			if (sb)
      	{
				for (i=0,j=0; i<oldSignalCounts; i++)
					{
					int Index ;
					if (j==nCounts)							  
						Index = -1 ;
					else
   					Index = SelectedIndexes [j] ;
					if (i!=Index)
						{
						BlockSize += si->signalLen ;
						sb +=si->signalLen ;
       			si++ ;
       			newSignalCounts++ ;
						}
   				else
						{
						if (Index != oldSignalCounts-1)
							{
							MoveMemory(sb, sb+si->signalLen, (DWORD)(oldBlockSize-BlockSize-si->signalLen)) ;
							MoveMemory(si,  si+1, (DWORD)((oldSignalCounts-newSignalCounts-1)*sizeof (SIGNALINFO))) ;
							j++ ;
							}
   					}
					}
				AMemUnlock (hSignalBlock) ;
				AMemSetSize (hSignalBlock, BlockSize) ;
				}
			AMemUnlock (hSignalInfo) ;
			AMemLock (hSignalInfo) ;
			AMemUnlock (hSignalInfo) ;
			AMemSetSize (hSignalInfo, newSignalCounts*sizeof(SIGNALINFO)) ;
			signal->signalCount = newSignalCounts ;
			}
		delete []SelectedIndexes ;
		}
	ClearStringEdit (hDlg) ;
	ShowButtonStatus (hDlg, FALSE);
	return ;                                               
}
//
//
//
void EDIT_SIGNAL::SaveCompareBits (HWND hDlg, int idButton)
{
	long nCount ;
	int  Index ;
  SIGNALINFO *signalInfo ;

	signalInfo = (SIGNALINFO*) AMemLock (hSignalInfo) ; //AObjLockData (theObject, SGNLINFO) ;
  if (signalInfo)
    {
	//  nCount = SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0, 0);
//		nCount = ListBox_GetSelCount(GetDlgItem (hDlg, idList)) ;
		nCount = SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0L, 0L);
		if (nCount==1)
    	{
	// SendDlgItemMessage (hDlg, idList, LB_GETSELITEMS, 1, (long)&Index) ;
//			ListBox_GetSelItems(GetDlgItem(hDlg, idList),1, &Index) ; 
	   SendDlgItemMessage (hDlg, idList, LB_GETSELITEMS, (WPARAM)1, (LPARAM)(int *)&Index) ;
  		signalInfo +=Index  ; 
			signalInfo->CompareBits = (idButton==idAlleight)?EIGHT:SEVENT ;
			}
    AMemUnlock (hSignalInfo) ; //AObjUnlockData (theObject, SGNLINFO) ;
    }
	return ;
}
BOOL EDIT_SIGNAL::ChangeSignalCheckButton (HWND hwnd, int idItem)
{
	BYTE     checkByte ;
	BOOL     check ;

  check = IsDlgButtonChecked (hwnd, idItem) ;
  switch (idItem)
    {
    case idFoundStr:
    	checkByte = FOUNDSTR ;
      break ;
    case idFoundPos:
      checkByte = FOUNDPOS ;
      break ;
    case idIntercept:
      checkByte = INTERCEPT ;
      break ;
    }
  if (check)
    signal->checkBox |=checkByte ;
  else
    signal->checkBox ^=checkByte ;

	SetSignalCheckButton (hwnd) ;

  return TRUE ;
}
void EDIT_SIGNAL::ChangeComboBoxContent (HWND hwnd, int idItem)
{
  int     index ;
  AOBJECTID oiObj ;

//  index = (WORD)SendDlgItemMessage (hwnd, idItem, CB_GETCURSEL, 0, 0) ;
  index = ComboBox_GetCurSel(GetDlgItem(hwnd, idItem));

//  oiObj = (AOBJECTID)SendDlgItemMessage (hwnd, idItem, CB_GETITEMDATA, index, 0) ;
  oiObj = (AOBJECTID)ComboBox_GetItemData(GetDlgItem(hwnd, idItem), index) ;

  switch (idItem)
    {
    case idFoundStrCon:
      signal->FoundString = oiObj ;
      break ;

    case idFoundPosCon:
      signal->FoundIndex = oiObj ;
      break ;
    case idInterceptCon:
      signal->InteText = oiObj ;
      break ;
    } 
}
void EDIT_SIGNAL::SaveSignalString (void) 
{
	SIGNALINFO *signalInfo ;
  LPSTR signalBlock ;

	signalInfo = (SIGNALINFO*)AMemLock (hSignalInfo) ;
	if (signalInfo)
	  {
	  AObjSetData (theObject, SGNLINFO, signalInfo, AMemGetSize(hSignalInfo)) ;
		AMemUnlock (hSignalInfo) ;
	  }
	signalBlock = (LPSTR) AMemLock (hSignalBlock) ;
	if (signalBlock)
	  {
		AObjSetData (theObject, SGNLBLOCK, signalBlock, AMemGetSize (hSignalBlock)) ;
		AMemUnlock (hSignalBlock) ;
		}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void EDIT_SIGNAL::ConfigureDialog (HWND hDlg)
{
	BOOL  ok ;
	DATA0 *tmpdata0 ;

	tmpdata0 = new DATA0 ;
	if(tmpdata0)
		*tmpdata0 = *data0 ;
	ok = DialogBoxParam(hInstance, "Connection" , hDlg, (DLGPROC)CntObjProc, (LPARAM)this) ;
	if(!ok)
		*data0 = *tmpdata0 ;
	if (tmpdata0)
		delete tmpdata0 ;
}
///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
void HighlightAddedItem (HWND hDlg, int Index)
{
	int selectedIndex ;
	while(SendDlgItemMessage (hDlg, idList, LB_GETSELITEMS, 1, (LPARAM)(int *)&selectedIndex) !=0)
	 	{
	 	SendDlgItemMessage (hDlg, idList, LB_SETSEL, FALSE, MAKELPARAM (selectedIndex, 0)) ;
	 	}
	SendDlgItemMessage (hDlg, idList, LB_SETSEL, TRUE, MAKELPARAM(Index,0)) ;
	SendMessage (hDlg, WM_COMMAND, MAKEWPARAM(idList, CBN_SELCHANGE), (LPARAM)GetDlgItem (hDlg, idList)) ; 

}
void ShowButtonStatus (HWND hDlg, BOOL display)
{	
	long nCounts ;
  nCounts = SendDlgItemMessage (hDlg, idList, LB_GETSELCOUNT, 0L, 0L) ;

	//Disable the buttons
	EnableWindow (GetDlgItem (hDlg, idChange), display) ;
	if (nCounts==0)
		EnableWindow (GetDlgItem (hDlg, idRemove), FALSE) ;
	else
		EnableWindow (GetDlgItem (hDlg, idRemove), TRUE) ;
	if (!display)
	  {
	  EnableWindow (GetDlgItem (hDlg, idActivate), FALSE) ;
		EnableWindow (GetDlgItem (hDlg, idDeactivate), FALSE) ;
		}
		
	//Hide the CompareBits stuffs
	EnableCompareGroup (hDlg, display) ;
}
void EnableCompareGroup (HWND hDlg, BOOL display)
{
	EnableWindow (GetDlgItem (hDlg, idAlleight), display) ;
	EnableWindow (GetDlgItem (hDlg, idLowerseven), display) ;
}
void SetChkBtnCombox (HWND hwnd, int idChkBtn, int idCombox, BOOL Status)
{
	CheckDlgButton (hwnd, idChkBtn, Status) ;
  ShowDlgItem (hwnd, idCombox, Status) ; 
}
void ShowDlgItem(HWND hwnd, int IdItem, BOOL status)
{
	HWND hwndItem = GetDlgItem (hwnd, IdItem) ;

  EnableWindow(hwndItem, status) ;
  UpdateWindow (hwndItem) ;
}

void DisplayListString (HWND hwnd, LPSTR string, char mark, int signalNo, int Index)
{
	char *displaystr ;
	int  len = _fstrlen(string);

	displaystr = new char[len+10] ;
  if (len)
		wsprintf(displaystr,"%c%d%c%s", mark, signalNo, ':',string) ;
	else
  	wsprintf(displaystr,"%c%d", mark, signalNo) ;
	SendDlgItemMessage (hwnd, idList, LB_INSERTSTRING, (WPARAM)Index, (LPARAM)displaystr) ;

	delete[]displaystr ;
}
void SetCompareBits (HWND hwnd, int idButton)
{
	CheckRadioButton (hwnd, idAlleight, idLowerseven, idButton) ;
}
void EditSignalString (HWND hDlg, WPARAM wParam)
{
	if (HIWORD(wParam)==EN_SETFOCUS)
	  {																								
		Edit_SetSel(GetDlgItem (hDlg, idInsString), 0, -1) ; //EM_SETSEL
		EnableCompareGroup (hDlg, TRUE) ;
		}
}
void ClearStringEdit (HWND hDlg)
{
	Edit_SetSel(GetDlgItem(hDlg, idInsString), 0, -1) ;  //EM_SETSEL
	SendDlgItemMessage (hDlg, idInsString, WM_CUT, 0, 0) ;
}
long ChangeHexToDec (char* lpDesc, char* lpSource)
{
	WORD i=0;
	int  currChar;
  int  nextChar;
	int  Decimal ;
	char c ;

	while(*lpSource!='\0')
    {
		if (*lpSource!=' ')
    	{
			currChar = HexToDec(*lpSource) ;
			while (*++lpSource==' ')
				{
				}
			if (*lpSource!='\0')
      	{
				nextChar = HexToDec(*lpSource++) ;
   	 		Decimal = 16*currChar + nextChar ;
      	c= (char) Decimal ;
				lpDesc[i++] = c ;
				}
			else
				break ;
			}
		else
			++lpSource ;
    }
		lpDesc[i] = '\0' ;
  return (lstrlen (lpDesc)) ;
}
void TextToHex (LPSTR lpBuffer, LPSTR lpText, LPSTR lpFormat, LONG/* len*/)
{
	char c ;
  char tbuf[80] ;

  //wsprintf (lpBuffer, lpFormat, (unsigned char)*lpText++) ;
	*lpBuffer = '\0' ;
  while((c=*lpText++) !='\0')
		{
    wsprintf((char*)tbuf, lpFormat, c);
    strcat(lpBuffer, (char *)tbuf);
    }
}
//
//
//
int HexToDec (int c)
{
  (char)c>'9'?c=c+9:c ;
	return (c&0x0F) ;
}
//
//
//
void StringToHex (LPSTR lpBuffer, LPSTR lpText, char *Format, int step)
{
	int i;
  char c ;
  char tbuf[80] ;

  i=0 ;
	//wsprintf (lpBuffer, Format, *lpText++) ;
	*lpBuffer = '\0' ;
  while((c=*lpText++) !='\0')
		{
    i++ ;
    wsprintf((char*)tbuf, Format, c);
		strcat(lpBuffer, (char *)tbuf);
		if (i==step)
			{
			i=0 ;
      strcat (lpBuffer, " ") ;
      }
    }
}
//////////////////////////////////////////////////////////////////////////
//function: StrChr
//Param1:
//Param2:
//Param3:   compareBits means that compare all eight bits (1) or lower sevent
//          bits of a char.
//////////////////////////////////////////////////////////////////////////
LPSTR StrChr (LPSTR str, char c, int compareBits)
{
	while(*str)
		{
		if (compareBits)
    	{
			if (*str==c)
				break ;
			}
		else
			{
			if ((*str|0x7F)==(c|0x7F))
      	break ;
      }
    str++ ;
		}

  return str ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
long StrSpn (LPSTR str1, LPSTR str2, int compareBits)
{
	long length = 0 ;

	while(*str1)
		{
		if (!*str2)
			break ;
		if (compareBits)
    	{
			if(*str1!=*str2)
				break ;
			}
		else
			{
			if((*str1|0x7F)!=(*str2|0x7F))
      	break ;
      }
		str1++ ;
		str2++ ;
		length++ ;	
		}
 return length ;
}
//////////////////////////////////////////////////////////////////////////
//*result = 1 ; str2 is substring of str1, return vaule points the first same
//              of str2 in str1.
//*result = -1; a part of str2 is same as str1, return value points the first
//              same char of str2 in str1.               ;
//*result = 0 ; there is not same part int str1, str2. return NULL pointer.
//////////////////////////////////////////////////////////////////////////
LPSTR StrCmp (LPSTR str1, LPSTR str2, int compareBits, int* result)
{
	LPSTR p ;
	int length ;
	 
	*result = 1 ;
	p = str1 ;
	p = StrChr (p,*str2, compareBits) ;

	if (p)
		{ 
		length = StrSpn (p, str2, compareBits) ;
		if (length==_fstrlen(str2))
			{
      *result = 1 ;
			}
		else if(length ==_fstrlen(p))
			{
      *result = -1 ;
			}
		else
    	*result = 0 ;
		}
	else
		{
		p = NULL ;
		*result = 0 ;
    }

  return p ;
}
BOOL ALMCALLBACK ConnectionDialog (
	HWND   hDlg,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
{
	EDIT_SIGNAL* es ;
	WORD	wID;
	WORD	wNotify;

	switch (message)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hDlg, 0);
			es = (EDIT_SIGNAL*) lParam ;
			SetWindowLong (hDlg, DWL_USER, lParam) ;
			es->InitSignalDialog(hDlg) ;
			return FALSE ;

    case WM_COMMAND: 
		wID = LOWORD(wParam);
		wNotify = HIWORD(wParam);
		  es = (EDIT_SIGNAL *)GetWindowLong (hDlg, DWL_USER) ;
		 	switch(wID)
       	{
        case idASCII:
        case idByte:
        case idWord:
        case idLongword:
					es->ChangeViewMode (hDlg, wParam) ;
					return FALSE ;

        case idAdd:
          es->AddStringToList (hDlg) ;
          return FALSE ;

        case idChange:
				  es->ChangeStringInList(hDlg) ;
          return FALSE;

				case idRemove:
					es->RemoveStringsFromList (hDlg) ;
					return FALSE ;

				case idActivate:
          es->ActivateSignal (hDlg, idActivate) ;
					return FALSE ;

				case idDeactivate:
				 	es->ActivateSignal (hDlg, idDeactivate) ;
					return FALSE ;

        case idList:
          switch(wNotify)
            {
						case CBN_SELCHANGE:
              es->DisplayString (hDlg) ;
              break;
						case CBN_DBLCLK:
							es->ActivateSignal (hDlg, idList) ; 
							break;
            }
						return FALSE ;

				case idInsString:
        	EditSignalString (hDlg, wParam) ;
					return FALSE ;

				case idAlleight:
				case idLowerseven:
				 	es->SaveCompareBits (hDlg, wParam) ;
					return FALSE ;

        case idFoundStr:
        case idFoundPos:
        case idIntercept:
					es->ChangeSignalCheckButton (hDlg, wParam) ;
          return FALSE ;

        case idFoundStrCon:
        case idFoundPosCon:
        case idInterceptCon:
					es->ChangeComboBoxContent (hDlg, wParam) ;
          return FALSE ;

        case idConfigure:
					es->ConfigureDialog (hDlg) ;
					return FALSE ;

				case IDOK:
					es->SaveSignalString () ;
					EndDialog (hDlg, TRUE) ;
					return FALSE ;

        case IDCANCEL:
				 	EndDialog (hDlg, FALSE) ;
					return TRUE ;

				}
        return FALSE ;
			}
       return FALSE ;
}
//
// Callback function 
//
long ALMCALLBACK EditProc (HWND hwnd, WORD message, WPARAM wParam, LPARAM lParam)
{
	int viewMode ;

	switch (message)
		{
		case WM_CHAR:
			viewMode = IsDlgButtonChecked (GetParent (hwnd), idASCII) ;
			if (viewMode) break ;
			if ((wParam==VK_SPACE)||(wParam==VK_BACK)||(wParam ==VK_INSERT)||(wParam==VK_DELETE)||(wParam<=VK_DOWN&&wParam>=VK_PRIOR)||
				(wParam>=48&&wParam<=57)||(wParam>=65&&wParam<=70)||(wParam>=97&&wParam<=102)) break;
			MessageBeep (-1) ;
			return 0 ;
		}

	return CallWindowProc (defOldEditProc, hwnd, message, wParam, lParam) ;
}
//
