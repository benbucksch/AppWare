/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR: Shaili Jain
//
// DESCRIPTION:  March 21, 1994
//
// CHANGES:
//
// 
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT
#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>                                                                   
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "commdlg.h"

#define SIGOBJD

#include "stdlib.h"

#include "signal.h"
#include "sigsobjd.h"


#define N_SIGFORMATS	1


 /****************************************************************************
 *                                                                          *
 *  FUNCTION   : DoDialog()                                                 *
 *                                                                          *
 *  DESCRIPTION: Generic dialog invocation routine.  Handles procInstance   *
 *               stuff, and param passing.                 									*
 *  RETURNS    : result of dialog procedure.                                *
 *                                                                          *
 ****************************************************************************/
WORD far DoDialog(
	 LPCSTR lpTemplateName,
	 FARPROC lpDlgProc,
	 OBJECTID objID)
	 {
	 WORD wRet;
   HWND hwndParent = GetActiveWindow();
	 wRet = DialogBoxParam((HINSTANCE)hInst, lpTemplateName, hwndParent, (DLGPROC)lpDlgProc, (LONG)objID);
	 return wRet;

   }

extern "C" int FAR PASCAL _export SignalsDlgProc (
  HWND          hwnd,
  WORD          msg,
  WORD          wParam,
	LONG          lParam);

/**************************************************************************
*
*  This function is called to edit the CLIENT OBJECT
*
***************************************************************************/

int FAR PASCAL EditServerObject(OBJECTID theObject)
  {
	return DoDialog(MAKEINTRESOURCE(DLGSIGNALS), (FARPROC)SignalsDlgProc, (OBJECTID)theObject);
  }




/***********************************************************************
* 
*	Save the old Signals
*
************************************************************************/
void SaveOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **lpOldSigs)
	{
	objd->sigs->CheckSignals();
	*nOldSigs = objd->sigs->m_nUserSignals;
	*lpOldSigs = (SIGNALINFO *)new char[sizeof(SIGNALINFO) * (objd->sigs->m_nUserSignals)];
	_fmemcpy((LPSTR)*lpOldSigs, (LPSTR)objd->sigs->m_userSignals, *nOldSigs * sizeof(SIGNALINFO));
	}

/***********************************************************************
* 
*	Reset the old Signals
*
************************************************************************/
void ResetOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **lpOldSigs)
 	{
	objd->sigs->m_nUserSignals = *nOldSigs;
	delete objd->sigs->m_userSignals;
	objd->sigs->m_userSignals = *lpOldSigs;
	}




/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetSignal(
  LPSTR name)
  {
  if (strcmp(name, "") == 0) // empty string
		return FALSE;
  if (strcmp(defSignal.name, name) != 0)
    {
    strncpy((LPSTR)defSignal.name,(LPSTR) name, MAX_NAME - 1);
    return TRUE;
    }
  else
  // The return value is used to set bitflags to be checked before Adding items
  // to main list.  So a non-empty edit box should return true even if it is 
  // same as the current string
    return TRUE; // FALSE; return true even if string is same ? SKJ
  }



/**********************************************************************
* Function:     ChangeName
* Description:  This function will set the name of service, topic, or item
*               based on the ctrlID parameter. If the service name changes
*               it will change the list of topics and items
*
*
***********************************************************************/
int OBJDCLASS::ChangeName(LPSTR name, int ctrlID)
  {
  int rVal = 0;;
  HWND hCtrl = GetDlgItem(hDlg, ctrlID);


  switch (ctrlID)
    {
  
		case IDSIGNAL:
      {
      SetSignal(name);
			if (strcmp(name, "") == 0) {
				ResetAddFlag(SIGS_ADDEDSIGNAL);
        rVal = 1;
        }
      else {
				SetAddFlag(SIGS_ADDEDSIGNAL);
        rVal = 0;
        }
      }
      break;

    default: return 0;
    }
  I_UNUSED(hCtrl);
  return rVal;
	}


/*************************************************************************
* Find if the string has any alpha char in it
*
*
*************************************************************************/
BOOL IsAlphaString(LPSTR lpsz)
	{
	BOOL rVal = FALSE;
	LPSTR	lpszNon;
	for (lpszNon = lpsz; ((*lpszNon != NULL) && !(rVal = IsCharAlpha(*lpszNon)));
		lpszNon = AnsiNext(lpszNon));
	return rVal;
  }


/*****************************************************************************************
*
*		Get all the strings from the MAIN LIST
*
****************************************************************************************/
BOOL OBJDCLASS::GetFromMainList(
  LPSTR Topic,
  LPSTR Item,
  LPSTR Format,
  OBJECTID *ObjectID)
  {

  char *mainStr = new char[4 * MAXNAME];
  int index = (int)SendMessage(GetDlgItem(hDlg, IDMAINLIST), LB_GETCURSEL, 0, 0L);

  if (index == LB_ERR)
    return FALSE;
  SendMessage(GetDlgItem(hDlg, IDMAINLIST), LB_GETTEXT, index, (LPARAM)(LPSTR)mainStr);

  LPSTR tmpStr = strchr(mainStr, ':');
  tmpStr[0] = '\0';  // set 0th pos to null and increment ptr
  tmpStr++;
  strncpy(Topic, mainStr, MAXNAME);
  mainStr = tmpStr;

  tmpStr = strchr(mainStr, ':');
  tmpStr[0]='\0';  // set 0th pos to null and increment ptr
  tmpStr++;
  strncpy(Item, mainStr, MAXNAME);
  mainStr = tmpStr;

  tmpStr = strchr(mainStr, ':');
  tmpStr[0]='\0';  // set 0th pos to null and increment ptr
  strncpy(Format, mainStr, MAXNAME);
  mainStr = ++tmpStr;

  *ObjectID = (OBJECTID)SendMessage(GetDlgItem(hDlg, IDMAINLIST), LB_GETITEMDATA, index, 0);
  delete mainStr;
  return TRUE;
  }


/******** Methods for Signal Dlg box *********************/




/****************************************************************************
* Function:     BuildSignalList
*
* Description:  This function will build a list Signals
*               during the INITDIALOG processing
*
*****************************************************************************/
void OBJDCLASS::BuildSignalList()
  {
  int           i;
  int           delCount = 0;
	// empty the MainList
	HWND	hCtrl = GetDlgItem(hDlg,IDSIGNALLIST);
	SendMessage(hCtrl, LB_RESETCONTENT, 0, 0L);

	for (i = 0; i < sigs->m_nUserSignals; i++)
    {
    // select the last added string
		SetSignalList(sigs->m_userSignals[i].name);
		}

	// if there is a def. string then select it.
	if (strcmp(defSignal.name, "") != 0)
  	{
		SendMessage(hCtrl, LB_SELECTSTRING, -1, (long)defSignal.name);
		PostMessage(GetDlgItem(hDlg, IDSIGNAL), EM_SETSEL, 0, MAKELONG(0, -1));

		}

	I_UNUSED(delCount);
	}


/****************************************************************************
* Function:     SetSignalList
*
* Description:  This functions will add a string to the signal list box and set it as
*               current selection.
*               If the string being added is empty it will select the first item
*               The ObjectID is added as the itemData
*
* Return:       index of the string added
*
*****************************************************************************/

int OBJDCLASS::SetSignalList(LPSTR itemStr)
  {
  int index = 0;
  HWND hCtrl = GetDlgItem(hDlg, IDSIGNALLIST);

  if (strcmp(itemStr, "") == 0)
    return -1;

	if (itemStr[0] != '\0') 		{
		index = (int)SendMessage(hCtrl, LB_ADDSTRING, 0, (LONG)(LPSTR)itemStr);
    
    if (index == CB_ERR)
      index = 0;
    else
      SendMessage(hCtrl, LB_SETITEMDATA, index, (LONG)oid);
    }
  SendMessage(hCtrl, LB_SETCURSEL, index, 0L);
  return index;
  }



/*****************************************************************************************
*
* Get all the strings from thE SIGNAL LIST
*
****************************************************************************************/
BOOL OBJDCLASS::GetFromSignalList(
  SIGNALINFO *curSignal)
  {

  char *mainStr = new char[1 * MAXNAME];
  int index = (int)SendMessage(GetDlgItem(hDlg, IDSIGNALLIST), LB_GETCURSEL, 0, 0L);
  if (index == LB_ERR)
    return FALSE;

  SendMessage(GetDlgItem(hDlg, IDSIGNALLIST), LB_GETTEXT, index, (LPARAM)(LPSTR)mainStr);
  strncpy(curSignal->name, mainStr, MAXNAME);

  delete mainStr;
  return TRUE;
	}


/***************************************************************************
*
* Description:  In Response to PUSHING UPDATE button
*               Keep a list of update signals (old and update names)
*
****************************************************************************/
BOOL OBJDCLASS::KeepUpdatedSignals(LPSTR oldName, LPSTR upName)
	{
	int numUpdates = AMemGetSize(sigs->m_upListID) / sizeof(UPDATESIGNAL);
	AMemSetSize(sigs->m_upListID,(numUpdates + 1) * sizeof(UPDATESIGNAL));
	LPUPDATESIGNAL lpUpdate = (LPUPDATESIGNAL)AMemLock(sigs->m_upListID);
	strcpy(lpUpdate[numUpdates].oldName, oldName);
	strcpy(lpUpdate[numUpdates].updName, upName);
  AMemUnlock(sigs->m_upListID);
	return TRUE;
	}

/***************************************************************************
*
* Description:  In Response to PUSHING ADD button
*               Add Signal to Signal List
*
****************************************************************************/
int OBJDCLASS::AddSignalList(
	SIGNALINFO newSignal,
	BOOL       bUpdate)
  {
	int   curSigIndex = SIGS_SIGNALNOTFOUND;

  // if there are some empty transaction param. fields then we cannot add to the main list
	if ((bCanAdd & SIGS_CANADDSIGNAL) != SIGS_CANADDSIGNAL)
    return 0;

	if ( (curSigIndex = sigs->IsNewSignal(newSignal)) == SIGS_SIGNALNOTFOUND) // -1
    {  // add new Signal
		SIGNALINFO *tmpSig = new SIGNALINFO[sigs->m_nUserSignals + 1]; // increment signal count and reallocate
		_fmemcpy((LPSTR)tmpSig, (LPSTR)sigs->m_userSignals, sizeof(SIGNALINFO) * (sigs->m_nUserSignals));

		if (IsAlphaString(newSignal.name))
			newSignal.bisAlphaSignal = SIG_ISALPHA;
    else
			newSignal.bisAlphaSignal = SIG_ISINT;

		if (bUpdate)
			newSignal.bisAlphaSignal |= SIG_ISUPDATED;
      
		tmpSig[sigs->m_nUserSignals] = newSignal; // copy the new signal
		sigs->m_nUserSignals++;
		delete sigs->m_userSignals;
		sigs->m_userSignals = tmpSig;
    
		char *mainStr = new char[MAXNAME];
    strcpy(mainStr, newSignal.name);
    // select the last added string
		SetSignalList(mainStr);
		delete mainStr;
		}

	return curSigIndex;
	}

/***************************************************************************
*
* Description:  In Response to PUSHING DELETE button
*               Delete Signal Entry
*
****************************************************************************/
int OBJDCLASS::DeleteSignalList(
	LPSTR     pszSignal
	)
  {
	int j = 0;
	SIGNALINFO delSignal;
	strcpy(delSignal.name, pszSignal);
	int curSigIndex = SIGS_SIGNALNOTFOUND;

	if ( (curSigIndex = sigs->IsNewSignal(delSignal)) == SIGS_SIGNALNOTFOUND) // -1
    {  
		return SIGS_SIGNALNOTFOUND;
    }
  else
    {

		for (j = 0; j <sigs->m_nUserSignals; j++)
      {
			if (lstrcmp(sigs->m_userSignals[j].name, delSignal.name) == 0)
        curSigIndex = j;
      }
		if (curSigIndex != SIGS_SIGNALNOTFOUND)
			sigs->RemoveSignal(curSigIndex);
    }

  I_UNUSED(pszSignal);

  return curSigIndex;  // the item deleted
  }


/***************************************************************************
*
* Description:  In Response to PUSHING UPDATE button
*               Update Signal in the List
*          
****************************************************************************/
int OBJDCLASS::UpdateSignalList()
  {
	int curSigIndex = SIGS_SIGNALNOTFOUND;
	if ((bCanAdd & SIGS_CANADDSIGNAL) != SIGS_CANADDSIGNAL)
    return -1;
  SIGNALINFO curSignal;

  if (GetFromSignalList(&curSignal) == FALSE)
		return SIGS_SIGNALNOTFOUND;

  // If the new updated signal name already exists, do not allow for update
	curSigIndex = sigs->IsNewSignal(defSignal);
	if (curSigIndex != SIGS_SIGNALNOTFOUND)
  	{
		MessageBox(hDlg, "Signal Already Exists", "ERROR: UPDATE", MB_OK);
		// SetFocus(GetDlgItem(hDlg, IDSIGNAL) );
		return -1;
    }
  
	if ( (curSigIndex = sigs->IsNewSignal(curSignal)) == SIGS_SIGNALNOTFOUND) // -1
		{
		ADBG_ASSERT_I(curSigIndex != SIGS_SIGNALNOTFOUND, return curSigIndex, "Err in Signal Selection");

    ;
    }
  else
    {
    curSigIndex = SIGS_SIGNALNOTFOUND;
		DeleteSignalList(curSignal.name);
    // Insert New Topic, item etc   (Take values from the def-Fields i.e. defTopic etc)
		AddSignalList(defSignal, TRUE);
		KeepUpdatedSignals(curSignal.name, defSignal.name);
		curSigIndex = 0;
		}
  return curSigIndex;  // the item deleted
  }







/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::InitSigCtrls()
  {
	SetWindowText(GetDlgItem(hDlg, IDSIGNAL),defSignal.name);
	SetAddFlag(SIGS_ADDEDSIGNAL);
	SetAddFlag(SIGS_ADDEDSIGOBJECT); // object addded

	BuildSignalList();
	return FALSE;
	}

/****************************************************************************
 *  METHOD     	: OBJDCLASS()                                             	*
 *                                                                          *
 *  DESCRIPTION	: Constructor                                   						*               
 *
 *  RETURNS    	:                                                           *
 *                                                                          *
 ****************************************************************************/
OBJDCLASS::OBJDCLASS(HWND hWnd, OBJECTID objID)
  {

	char *typeName = new char[MAXNAME];
	char *objName	 = new char[MAXNAME];
	LPDEFOBJDDATA lpDefData = (LPDEFOBJDDATA)AObjLockData(objID, data1_DEFVALS);
	TYPEID  typeID = 0;

  // serviceCount = 1;
  hDlg = hWnd;
	oid = objID;

  // this flag should be init to 0 to start
  bCanAdd = 0;
	
	sigs = (SIGS *)new SIGS(objID);

  defSignal = lpDefData->defSignal;
	if (sigs->m_nUserSignals) {
		;
		}

	sigs->m_bDisplayErr = FALSE;

	AObjUnlockData(objID, data1_DEFVALS);

	// Set the runtime info slot to hold the OBJDCLASS ptr
  AObjSetRuntimeInfo(objID, (LONG)this);
	delete typeName;
	delete objName;
	I_UNUSED(typeID);
	I_UNUSED(lpDefData);

	}

/****************************************************************************
 *                                                                          *
 *  MEWTHOD    : ~OBJDCLASS()                                                 *
 *                                                                          *
 *  DESCRIPTION    : Destructor
 *               
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
OBJDCLASS::~OBJDCLASS()
	{
	if (sigs->m_bSave & SIGS_SAVEOBJD) // only after OK'd OBJD edit
		{

		LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(oid, data1_DEFVALS);
    lpDefData->defSignal = defSignal;
		AObjUnlockData(oid, data1_DEFVALS);
		}

	delete sigs;	 	// delete the SIGS class instanced
									// Set the runtime info slot to hold NULL
	AObjSetRuntimeInfo(oid, (LONG)NULL);
	sigs = NULL;
  }




/*********************************************************************
*	FUNCTION:  		GetOBJDPtr
*	DESCRIPTION:  This function will create a C++ ptr during INITDIAOG msg
            		and stuff it in the object datas.  For subsequent calls
*           		it returns this pointer
*
*
**********************************************************************/

LPLONG GetOBJDPtr(HWND hwnd, OBJECTID theObject, WORD msg )
  {
  OBJDCLASS *objd = NULL;
  LONG lpPtr;
  if (msg != WM_INITDIALOG)
    {
    lpPtr = (LONG)AObjGetRuntimeInfo(theObject);
    objd = (OBJDCLASS *)lpPtr;
    // I seem to get a KILL focus on editcontrols even after cancel/Ok
		if ((objd == NULL))
      return NULL;
    }
  else if (msg == WM_INITDIALOG)
    {
    objd = new OBJDCLASS(hwnd , theObject);
    }
  I_UNUSED(lpPtr);
  return (LPLONG)objd;
  }

/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetCtrlsFromSignalList()
  {
	TYPEID  objType = 0;
  if (GetFromSignalList(&defSignal) == FALSE)
    return FALSE;
	char *objectName = new char[MAXNAME];
	char *objTypeName = new char[MAXNAME];

  SetDlgItemText(hDlg, IDSIGNAL, defSignal.name);
	// Select the SIGNAL text box
	SetFocus(GetDlgItem(hDlg, IDSIGNAL));
	PostMessage(GetDlgItem(hDlg, IDSIGNAL), EM_SETSEL, 0, MAKELONG(0, -1));

	delete objectName;
	delete objTypeName;
	I_UNUSED(objType);
  return TRUE;
	}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   :     SignalsDlgProc                                         *
 *                                                                          *
 *  PURPOSE    :     To allow users to construct a list of server           *
 *								   userSignals  																					*
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
extern "C" int FAR PASCAL _export SignalsDlgProc (
  HWND          hwnd,
  WORD          msg,
  WORD          wParam,
  LONG          lParam)
  {
  static OBJECTID theObject;
  static OBJDCLASS *objd;
  static BOOL bDlgHasFocus = TRUE;
  BOOL   bHandled = FALSE;
	static int    nOldSigs;
	static SIGNALINFO *lpOldSig;

  switch (msg)
    {
    case WM_ACTIVATEAPP:
      {
      if (wParam == 0)
        {
        bDlgHasFocus = FALSE;
        return FALSE;
        }
      else
        {
        bDlgHasFocus = TRUE;
        return FALSE;
        }
      }

    case WM_INITDIALOG:
      {
			char title[40];
			theObject = (OBJECTID) lParam;
      objd = (OBJDCLASS*)GetOBJDPtr(hwnd, theObject ,msg);
      if (!objd)
        break;


			lpOldSig = NULL; // init to null
			objd->hDlg = hwnd;
			SaveOldSigs(objd, &nOldSigs, &lpOldSig);
			ATypeGetName(OTYPE_SIGS, (LPSTR)title, 32);
			AUtlSetTitleFont((TYPEID)OTYPE_SIGS, NULL,  GetDlgItem(hwnd, IDTITLE) );
			AUtlCenterDialog(hwnd, 0);
      // SKJ Nov. 2, 1994 Set Title
			AObjGetName((OBJECTID)lParam, (LPSTR)title, 32);
			SetWindowText(hwnd, (LPSTR)title);

			objd->bCanAdd = 0;  // set all add flags to 0
			objd->InitSigCtrls();
			SetFocus(GetDlgItem(hwnd, IDSIGNAL));
			return FALSE;
      }

    case WM_COMMAND:
      {
			objd = (OBJDCLASS*)GetOBJDPtr(hwnd , theObject ,msg);
      if (!objd)
      	{
        bHandled = FALSE;
        break;
				}

      switch (wParam)
				{
				case IDOK:
					objd->sigs->SetSave(SIGS_SAVESIGS);
					if (lpOldSig)
						delete lpOldSig; // remove the old copy of signals

					objd->sigs->SetSave(SIGS_SAVEOBJD); // set the bSave flag
					objd->sigs->SetIsAlphaSignal();
          delete objd;
					EndDialog(hwnd, 1);
          break;

				case IDCANCEL:      {

					objd->sigs->ResetSave(SIGS_SAVESIGS);
					ResetOldSigs(objd, &nOldSigs, &lpOldSig); // reset from old copy of signals
					delete objd;
					EndDialog(hwnd, 0);
					break;
					}

        case IDADD:
					{
					char *name = new char[MAXNAME];
					GetWindowText(GetDlgItem(hwnd,  IDSIGNAL), name, MAXNAME);
					if ((strcmpi(name, SIGSZ_DEFAULT) == 0) || (strcmpi(name, SIGSZ_ANY) == 0))
						{      
						MessageBox(hwnd, "RESERVED SIGNAL NAMES: '<DEFAULT>' and '<ANY>' ","ERROR - SIGNAL", MB_OK);
						}
					else if (strcmp(name, "") == 0)
						{
						MessageBox(hwnd, "INVALID SIGNAL NAME","ERROR - SIGNAL", MB_OK);
						}
					else {
						objd->ChangeName(name, IDSIGNAL); // if name is dif. from last one
						objd->SetAddFlag(SIGS_ADDEDSIGNAL);
						objd->AddSignalList(objd->defSignal, FALSE);
							
						}

					PostMessage(GetDlgItem(hwnd, IDSIGNAL), EM_SETSEL, 0, MAKELONG(0, -1));
					SetFocus(GetDlgItem(hwnd, IDSIGNAL));
					// PostMessage(GetDlgItem(hwnd, IDSIGNAL), WM_SETSEL, 0, MAKELONG(0, -1));

					bHandled = TRUE;
					delete name;

					}
          break;

				case IDDELETE:  {
					if (objd->DeleteSignalList(objd->defSignal.name) != SIGS_SIGNALNOTFOUND)
						{
						objd->BuildSignalList();
						objd->SetCtrlsFromSignalList(); // take the new selecttion and set the ctrls
						}
						bHandled = TRUE;

          }
          break;

        case IDUPDATE:
					if (objd->UpdateSignalList() != -1)
          	{
						objd->BuildSignalList();
						}
					SetFocus(GetDlgItem(hwnd, IDSIGNAL));
          bHandled = TRUE;
          break;

				case IDSIGNAL:
					{
          // When EDIT ctrl receives the focus, set the ADD button to be def
					if (HIWORD(lParam) == EN_SETFOCUS)
						{
            SendMessage(hwnd, DM_SETDEFID, IDADD, 0);
						SendDlgItemMessage(hwnd, IDADD, BM_SETSTYLE,(WPARAM) BS_DEFPUSHBUTTON, TRUE);
						}
		
          if ((HIWORD(lParam) == EN_KILLFOCUS) && (bDlgHasFocus == TRUE))
            {
						char *name = new char[MAXNAME];
            GetWindowText(GetDlgItem(hwnd,  wParam), name, MAXNAME);
            objd->ChangeName(name, wParam);
						objd->SetAddFlag(SIGS_ADDEDSIGNAL);
						bHandled = TRUE;
						SendMessage(hwnd, DM_SETDEFID, IDOK, 0);
						SendDlgItemMessage(hwnd, IDOK, BM_SETSTYLE,(WPARAM) BS_DEFPUSHBUTTON, TRUE);
						delete name;
            }
          break;
          }

				case IDSIGNALLIST:
          {
          if (HIWORD(lParam) == LBN_SELCHANGE)
            {
						objd->SetCtrlsFromSignalList();
		
            bHandled = TRUE;
            }
          }
					break;
				case IDHELP:
          {
					MessageBox(hwnd, "Help Not Implemented", "SIGNAL HELP", MB_OK);
          bHandled = TRUE;
          }
					break;
        default:
          return FALSE;
        }  // eo switch
      }  // eo of WM_COMMAND
    default:
      bHandled = FALSE;
    }  // eo of Switch

  I_UNUSED(theObject);
  return bHandled;
  }


