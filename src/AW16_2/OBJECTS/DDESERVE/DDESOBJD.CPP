/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//                                                                                           
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
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
// AUTHOR:
//
// DESCRIPTION:
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

#include "ddemlsv.h"
#include "stdlib.h"
#include "ddesobjd.h"
#include "o_pict.h"


#define N_SIGFORMATS	1

// Record used to temp. store main list strings during DELETION and UPDATE calls
struct DELRECORD
  {
  char        pszTopic[MAXNAME];
  char        pszItem[MAXNAME];
  FORMATINFO  format;
  OBJECTID    dataSrcOID;
  };
extern ATOM   CF_ALMOBJ;

// DDEML Initialize
extern  int far DoDialog(  LPCSTR lpTemplateName,  FARPROC lpDlgProc, OBJECTID objID);


typedef struct
  {
  TYPEID      typeID;
  char        name[MAXNAME];
  } TYPEINFO;


FORMATINFO sigFormats[N_SIGFORMATS] = {{1, CFTEXT}};


 /****************************************************************************
 *                                                                          *
 *  FUNCTION   : DoDialog()                                                 *
 *                                                                          *
 *  DESCRIPTION: Generic dialog invocation routine.  Handles procInstance   *
 *               stuff, and param passing.                 									*
 *  RETURNS    : result of dialog procedure.                                *
 *                                                                          *
 ****************************************************************************/
 int far DoDialog(
	 LPCSTR lpTemplateName,
	 FARPROC lpDlgProc,
	 OBJECTID objID)
	 {
	 WORD wRet;
   HWND hwndParent = GetActiveWindow();
	 wRet = DialogBoxParam((HINSTANCE)hInst, lpTemplateName, hwndParent, (DLGPROC)lpDlgProc, (LONG)objID);
	 return wRet;

   }

LONG GetDlgItemLong(
	HWND hwnd,
	WORD id,
	BOOL *pfTranslated,
	BOOL fSigned)
	{
  char szT[20];

  if (!GetDlgItemText(hwnd, id, szT, 20)) {
  	if (pfTranslated != NULL) {
    	*pfTranslated = FALSE;
      }
    return(0L);
    }
  if (pfTranslated != NULL) {
  	*pfTranslated = TRUE;
		}
	I_UNUSED(fSigned);

  return(atol(szT));
	}


VOID SetDlgItemLong(
	HWND hwnd,
	WORD id,
	LONG l,
	BOOL fSigned)
	{
  char szT[20];

  ltoa(l, szT, 10);
	SetDlgItemText(hwnd, id, szT);
	I_UNUSED(fSigned);
	}


/**************************************************************************
*
* 	This function will return the id of the clipboard formats
*
***************************************************************************/
ATOM GetFormatID(char *formatStr)
  {
  if (lstrcmpi(formatStr, CFTEXT) == 0)
    return CF_TEXT;
  else if (lstrcmpi(formatStr, CFBITMAP) == 0)
    return CF_BITMAP;
  else if (lstrcmpi(formatStr, CFMETAFILE) == 0)
    return CF_METAFILEPICT;
  else 
    return aFormats[CFORMATS].atom;  // CF_ALMOBJ
	}


/**************************************************************************
*
* 	
*
***************************************************************************/
void NewTopic(MEMBLOCKID tlID)
	{
  I_UNUSED(tlID);
  }

/****************************************************************************
* Function:     SetComboItem
*
* Description:  This functions will add a string to a combo box and set it as
*               current selection
*               If the string being added is empty it will select the first item
*
* Return:       index of the string added
*
*****************************************************************************/
int SetComboItem(HWND hWnd, int CtrlID, LPSTR itemStr)
  {
  int   index = 0;
  HWND  hCtrl = GetDlgItem(hWnd, CtrlID);

  if ((itemStr[0] != '\0') &&
	 ((index = (int)SendMessage(hCtrl, CB_FINDSTRINGEXACT,-1, (LONG)(LPSTR)itemStr)) == CB_ERR) )
    {
		index = (int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)itemStr);
    if (index == CB_ERR)
      index = 0;
    }
  SendMessage(hCtrl, CB_SETCURSEL, index, 0L);
  return index;
  }


/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetService(
  LPSTR name)
  {
  if (strcmp(name, "") == 0) // empty string
    return FALSE;
  if (strcmp(ddes->ddesService.Service, name) != 0)
    {
    strncpy((LPSTR)ddes->ddesService.Service,(LPSTR) name, MAX_NAME - 1);
    ddes->ddesService.Service[MAX_NAME - 1] = '\0';
    return TRUE;
    }
  else
  // The return value is used to set bitflags to be checked before Adding items
  // to main list.  So a non-empty edit box should return true even if it is 
  // same as the current string
    return TRUE; // FALSE; return true even if string is same ? SKJ
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

/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetTopic(LPSTR name) {
  if (strcmp(name, "") == 0) // empty string
    return FALSE;
  if (strcmp(ddes->defTopic, name) != 0)
    {
    strncpy((LPSTR)ddes->defTopic,(LPSTR) name, MAX_NAME - 1);
    ddes->defTopic[MAX_NAME - 1] = '\0';
    return TRUE;
    }
  else
    return TRUE; //  FALSE; return true even if string is same ? SKJ
  }

/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetItem(LPSTR name) {
  if (strcmp(name, "") == 0) // empty string
    return FALSE;
  if (strcmp(ddes->defItem, name) != 0)
    {
    strncpy((LPSTR)ddes->defItem,(LPSTR) name, MAX_NAME);
    ddes->defItem[MAX_NAME - 1] = '\0';
    return TRUE;
    }
  else
    return TRUE; // FALSE; return true even if string is same ? SKJ
  }

/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetFormat(LPSTR name) {
  if (strcmp(name, "") == 0) // empty string
    return FALSE;
  if (strcmp(ddes->defFormat.name, name) != 0)
    {
    strncpy((LPSTR)ddes->defFormat.name,(LPSTR) name, MAX_NAME);
    ddes->defFormat.name[MAX_NAME - 1] = '\0';
    return TRUE;
    }
  else
    return TRUE;// FALSE; return true even if string is same ? SKJ
  }


/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetObjType(TYPEID typeID) {
  if (typeID == 0) // empty string
    return FALSE;
  if (ddes->defObjectType != typeID)
    {
    ddes->defObjectType = typeID;
    return TRUE;
    }
  else
    return TRUE; // FALSE; return true even if string is same ? SKJ
  }

/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetObject(OBJECTID objID) {
  if (objID == 0)
    return FALSE;
  if (ddes->defDataSrcOID != objID)
    {
    ddes->defDataSrcOID = objID;
    return TRUE;
    }
  else
    return TRUE; // FALSE; return true even if string is same ? SKJ
  }

/**************************************************************************
*
* 	
*
***************************************************************************/
int OBJDCLASS::SetUpdateTime(long Time)
  {
	if (ddes->defUpdateTime == Time)
    return 0;
  else {
    ddes->defUpdateTime = Time;
    return 1;
    }
  }



/****************************************************************************
*                                                                          *
*  METHOD    : BuildFormatList()                                           *
*                                                                          *
*  DESCRIPTION    : Build A DDE Item List
*               
*  RETURNS    :                                                            *
*                                                                          *
****************************************************************************/
int OBJDCLASS::BuildFormatList()
  {
	int i;
  HWND hCtrl = GetDlgItem(hDlg, IDFORMAT);
  int curindex;
  // if there are no valid formats, then allow the user to choose from
  // any of the predetermined list of formats.  The error check in this
  // will not be done till runtime
  SendMessage(hCtrl, CB_RESETCONTENT, 0,0);

  for (i = 0; i < CFORMATS; i++)
    {
    
		curindex = (int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)aFormats[i].name);
    SendMessage(hCtrl, CB_SETITEMDATA, curindex,aFormats[i].atom);
    }

	curindex = (int) SendMessage(hCtrl,CB_SELECTSTRING, 0, (long) ddes->defFormat.name);
	if (curindex == CB_ERR) {
    curindex  = 0;
    // select the first string 
		SendMessage(hCtrl, CB_SETCURSEL, curindex, 0L);
		SendMessage(hCtrl,CB_GETLBTEXT, curindex, (long) ddes->defFormat.name); // set the new def
		}

	ATOM formatType = (int)SendMessage(hCtrl, CB_GETITEMDATA, curindex, 0L);
	ddes->defFormat.atom = formatType;
	SetAddFlag(DDES_ADDEDFORMAT);
	// bCanAdd |= DDES_ADDEDFORMAT;  // added correct service
  // Build the object list from the chosen type
  BuildObjTypeList(formatType);

  return curindex;

	}

/***************************************************************************
Description: In Response to PUSHING OK button
*            Create all topic related signals
****************************************************************************/
int OBJDCLASS::CreateSignalsFromMainList()
	{
	int nTopics = ddes->ddesService.nTopics;
	DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
	char *mainStr = new char[4 * MAXNAME];
	int curTopicIndex, curItemIndex;

	ddes->nTopicSignals = 0; // create a new sig list from MAINLIST items

	for (curTopicIndex = 1; curTopicIndex < nTopics; curTopicIndex++) // skip tbe SYSTEM topic
		{
		LPSTR szTopic = new char[strlen(lpTopicList[curTopicIndex].pszTopic) + 1];
		strcpy(szTopic, lpTopicList[curTopicIndex].pszTopic);\
		DDESITEMLIST *	lpItemList;
    // Create topic related signals (CONV START/CONV STOP)
		ddes->AddTopicSignal(szTopic, TRUE);

		_fmemset(mainStr, 0,4 * MAXNAME);
		lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[curTopicIndex].itemListID);

		for (curItemIndex = 0; curItemIndex <  lpTopicList[curTopicIndex].nItems; curItemIndex++)
      {
			strcpy(mainStr, szTopic);
			strcat(mainStr, ":");
			    
    	strcat(mainStr, lpItemList[curItemIndex].pszItem);
    	strcat(mainStr, ":");

    	strcat(mainStr, lpItemList[curItemIndex].format.name);
			ddes->AddTopicSignal(mainStr, FALSE);
			}
		AMemUnlock(lpTopicList[curTopicIndex].itemListID);
		delete szTopic;
		}

	AMemUnlock(ddes->ddesService.topicListID);
	delete mainStr;


  
	return ddes->nTopicSignals;
	}






/***************************************************************************
Description: Display error to describe why item cannot be added to mainlist 
*            
****************************************************************************/
UINT DisplayErr_MainList(HWND hDlg, long bCanAdd)
	{
  UINT idErrStr;
	if ((bCanAdd & DDES_ADDEDSERVICE) == 0)  						// 0000000000000001
		idErrStr = IDS_ADDEDSERVICE;
	else if ((bCanAdd & DDES_ADDEDTOPIC) == 0)      			// 0000000000000010
			idErrStr = IDS_ADDEDTOPIC;
	else if ( (bCanAdd & DDES_ADDEDITEM) == 0)          	// 0000000000000100
			idErrStr = IDS_ADDEDITEM;
	else if ( (bCanAdd & DDES_ADDEDFORMAT) == 0)         // 0000000000001000
			idErrStr = IDS_ADDEDFORMAT;
	else if ( (bCanAdd & DDES_ADDEDOBJECT) ==0)         // 0000000000010000
			idErrStr = IDS_ADDEDOBJECT;
  else idErrStr = 0;

	if (idErrStr)
		{
    char *errBuf = new char[80];
		LoadString(hInst, idErrStr, errBuf, 80);
		MessageBox(hDlg,errBuf, "ERROR IN ADDING ITEM", MB_OK);
		delete errBuf;
		}

  return idErrStr;

  }


/***************************************************************************
Description: In Response to PUSHING ADD button
*            Get the current strings from all controls and add to MAIN LISt
****************************************************************************/
int OBJDCLASS::AddMainList() {
	int j;
	int curTopicIndex = DDES_TOPICNOTFOUND;
  int curItemIndex =  DDES_ITEMNOTFOUND;
  long topicSize = AMemGetSize(ddes->ddesService.topicListID);

  DDESTOPICLIST *lpTopicList;
  DDESITEMLIST *lpItemList;
  int rVal;
	char *ObjectName = new char[MAXNAME];

  // if there are some empty transaction param. fields then we cannot add to the main list
	if ((bCanAdd & DDES_ADDEDALL) != DDES_ADDEDALL) {
    DisplayErr_MainList(hDlg, bCanAdd);
		delete ObjectName;
    return 0;
    }
  if ( (curTopicIndex = ddes->IsNewTopic(ddes->defTopic)) == DDES_TOPICNOTFOUND) // -1
    {  // add new topic
    AMemSetSize(ddes->ddesService.topicListID, topicSize + sizeof(DDESTOPICLIST));
    curTopicIndex = ddes->ddesService.nTopics;
    ddes->ddesService.nTopics += 1;
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
    strcpy(lpTopicList[curTopicIndex].pszTopic, ddes->defTopic);
		lpTopicList[curTopicIndex].itemListID = AMemAllocate(sizeof(DDESITEMLIST));
    rVal = DDES_NEWITEM;
    curItemIndex = 0; // first Item
    lpTopicList[curTopicIndex].nItems = 1;
    AMemUnlock(ddes->ddesService.topicListID);

    // add to list of topic signals
    //ddes->AddTopicSignal(ddes->defTopic, TRUE);

    }
  else
    {
		lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
    lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[curTopicIndex].itemListID);
    curItemIndex = DDES_ITEMNOTFOUND;
    for (j = 0; j <lpTopicList[curTopicIndex].nItems; j++)
      {
      if ((strcmp(lpItemList[j].pszItem, ddes->defItem) == 0) &&
        (strcmp(lpItemList[j].format.name, ddes->defFormat.name) == 0) &&
        (lpItemList[j].dataSrcOID == ddes->defDataSrcOID))
        curItemIndex = j;
      }

    AMemUnlock(lpTopicList[curTopicIndex].itemListID);
    
    // if no matching item was found, then add a new one
    // else return NOOP
    if (curItemIndex == DDES_ITEMNOTFOUND)
      {
      long itemSize = AMemGetSize(lpTopicList[curTopicIndex].itemListID);
      AMemSetSize(  lpTopicList[curTopicIndex].itemListID, itemSize + sizeof(DDESITEMLIST));
      curItemIndex = lpTopicList[curTopicIndex].nItems++;
      rVal = DDES_NEWITEM;
      }
    else
      rVal = DDES_ITEMALREADYEXISTS;

    AMemUnlock(ddes->ddesService.topicListID);
    }

  if (rVal == DDES_NEWITEM)
    {
		char *mainStr = new char[4 * MAXNAME];
    _fmemset(mainStr, 0,4 * MAXNAME);
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
    lpItemList = (DDESITEMLIST *)AMemLock(  lpTopicList[curTopicIndex].itemListID);

		strcpy(lpTopicList[curTopicIndex].pszTopic, ddes->defTopic);
    strcpy(lpItemList[curItemIndex].pszItem, ddes->defItem);
    strcpy(lpItemList[curItemIndex].format.name, ddes->defFormat.name);
    lpItemList[curItemIndex].format.atom = ddes->defFormat.atom;


    lpItemList[curItemIndex].dataSrcOID = ddes->defDataSrcOID;
    lpItemList[curItemIndex].updateTime = ddes->defUpdateTime;

    strcat(mainStr, lpTopicList[curTopicIndex].pszTopic);
    strcat(mainStr, ":");
    
    strcat(mainStr, lpItemList[curItemIndex].pszItem);
    strcat(mainStr, ":");

    strcat(mainStr, lpItemList[curItemIndex].format.name);

		//ddes->AddTopicSignal(mainStr, FALSE);

    strcat(mainStr, ":");


		if (AObjGetName(ddes->defDataSrcOID, ObjectName, MAXNAME) == 0)
			ObjectName[0] = '\0';
    strcat(mainStr, ObjectName);

    // select the last added string
    SetMainList(hDlg, mainStr, lpItemList[curItemIndex].dataSrcOID);


    AMemUnlock( lpTopicList[curTopicIndex].itemListID);
		AMemUnlock(ddes->ddesService.topicListID);
		delete mainStr;
    }

	delete ObjectName;
  return curTopicIndex;

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
  DELRECORD     *delRec = new DELRECORD[ddes->nUserSignals];
	// empty the MainList
	HWND	hCtrl = GetDlgItem(hDlg,IDSIGNALLIST);
	SendMessage(hCtrl, LB_RESETCONTENT, 0, 0L);

  for (i = 0; i < ddes->nUserSignals; i++)
    {
    // select the last added string
    if (AObjCheckType(ddes->userSignals[i].rcvrOID,0L) )
      SetSignalList(ddes->userSignals[i].name, ddes->userSignals[i].rcvrOID);
    else {
      strcpy(delRec[i].pszTopic, ddes->userSignals[i].name);
      delRec[i].dataSrcOID = ddes->userSignals[i].rcvrOID;
      delCount++;
      }
    }
  // delete all the delete oid references
  for (i = 0; i < delCount; i++)
    {
    DeleteSignalList(delRec[i].pszTopic, delRec[i].dataSrcOID);
		}

	// if there is a def. string then select it.
	
	if (strcmp(defSignal.name, "") == 0)
		SendMessage(hCtrl, LB_SELECTSTRING, -1, (long)defSignal.name);

	// SendMessage(hCtrl, LB_SETCURSEL, 0, 0L);
	delete delRec;
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
  curSignal->rcvrOID = (OBJECTID)SendMessage(GetDlgItem(hDlg, IDSIGNALLIST), LB_GETITEMDATA, index, 0);

  delete mainStr;
  return TRUE;
  }


/***************************************************************************
*
* Description:  In Response to PUSHING ADD button
*               Add Signal to Signal List
*
****************************************************************************/
int OBJDCLASS::AddSignalList(
  SIGNALINFO newSignal)
  {
	int   curSigIndex = DDES_SIGNALNOTFOUND;

  // if there are some empty transaction param. fields then we cannot add to the main list
	if ((bCanAdd & DDES_CANADDSIGNAL) != DDES_CANADDSIGNAL)
    return 0;

  if ( (curSigIndex = ddes->IsNewSignal(newSignal)) == DDES_SIGNALNOTFOUND) // -1
    {  // add new Signal
		SIGNALINFO *tmpSig = new SIGNALINFO[ddes->nUserSignals + 1]; // increment signal count and reallocate
		_fmemcpy((LPSTR)tmpSig, (LPSTR)ddes->userSignals, sizeof(SIGNALINFO) * (ddes->nUserSignals));
    tmpSig[ddes->nUserSignals] = newSignal; // copy the new signal
    ddes->nUserSignals++;
		delete ddes->userSignals;
    ddes->userSignals = tmpSig;
    
		char *mainStr = new char[MAXNAME];
    strcpy(mainStr, newSignal.name);
    // select the last added string
		SetSignalList(mainStr, newSignal.rcvrOID);
		delete mainStr;
		}

	return curSigIndex;
	}

/***************************************************************************
*
* Description:  In Response to PUSHING DELETE button
*               Deleete Signal Entry
*
****************************************************************************/
int OBJDCLASS::DeleteSignalList(
  LPSTR     pszSignal,
  OBJECTID  defRcvrOID
  )
  {
	int j = 0;
	SIGNALINFO delSignal;
	strcpy(delSignal.name, pszSignal);
  delSignal.rcvrOID = defRcvrOID;
  int curSigIndex = DDES_SIGNALNOTFOUND;

  if ( (curSigIndex = ddes->IsNewSignal(delSignal)) == DDES_SIGNALNOTFOUND) // -1
    {  
    return DDES_SIGNALNOTFOUND;
    }
  else
    {

    for (j = 0; j <ddes->nUserSignals; j++)
      {
      if (lstrcmp(ddes->userSignals[j].name, delSignal.name) == 0)
        curSigIndex = j;
      }
    if (curSigIndex != DDES_SIGNALNOTFOUND)
      ddes->RemoveSignal(curSigIndex);
    }

  I_UNUSED(pszSignal);
  I_UNUSED(defRcvrOID);

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
	int curSigIndex = DDES_SIGNALNOTFOUND;
	if ((bCanAdd & DDES_CANADDSIGNAL) != DDES_CANADDSIGNAL)
    return -1;
  SIGNALINFO curSignal;

  if (GetFromSignalList(&curSignal) == FALSE)
    return DDES_SIGNALNOTFOUND;

	if ( (curSigIndex = ddes->IsNewSignal(curSignal)) == DDES_SIGNALNOTFOUND) // -1
		{
		ADBG_PRINT_W("Err in Signal Selection");
		return curSigIndex;
		}
  else
    {
    curSigIndex = DDES_SIGNALNOTFOUND;
    DeleteSignalList(curSignal.name, curSignal.rcvrOID);
    // Insert New Topic, item etc   (Take values from the def-Fields i.e. defTopic etc)
    AddSignalList(defSignal);
    }
  return curSigIndex;  // the item deleted
  }




/***************************************************************************
*	Description:	In Response to PUSHING ADD button
*            		Get the current strings from all controls and add to MAIN LISt
*
****************************************************************************/
int OBJDCLASS::AddMainList(
  LPSTR defTopic,
  LPSTR defItem,
  LPSTR defFormat,
  OBJECTID defDataSrcOID,
  long    defUpdateTime)
	{
	int j;
	int curTopicIndex = DDES_TOPICNOTFOUND;
  int curItemIndex =  DDES_ITEMNOTFOUND;
  // HWND hCtrl = GetDlgItem(hDlg, IDMAINLIST);
  long topicSize = AMemGetSize(ddes->ddesService.topicListID);

  DDESTOPICLIST *lpTopicList; //  = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
  DDESITEMLIST *lpItemList;
  int rVal;
	char ObjectName[MAXNAME];

  // if there are some empty transaction param. fields then we cannot add to the main list
	if ((bCanAdd & DDES_ADDEDALL) != DDES_ADDEDALL)
    return 0;

  if ( (curTopicIndex = ddes->IsNewTopic(defTopic)) == DDES_TOPICNOTFOUND) // -1
    {  // add new topic
    AMemSetSize(ddes->ddesService.topicListID, topicSize + sizeof(DDESTOPICLIST));
    curTopicIndex = ddes->ddesService.nTopics;
    ddes->ddesService.nTopics += 1;
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
    strcpy(lpTopicList[curTopicIndex].pszTopic, defTopic);
    lpTopicList[curTopicIndex].itemListID = AMemAllocate(sizeof(DDESITEMLIST));
    rVal = DDES_NEWITEM;
    curItemIndex = 0; // first Item
    lpTopicList[curTopicIndex].nItems = 1;
    AMemUnlock(ddes->ddesService.topicListID);
    }
  else
    {
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
    lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[curTopicIndex].itemListID);
    curItemIndex = DDES_ITEMNOTFOUND;
    for (j = 0; j <lpTopicList[curTopicIndex].nItems; j++)
      {
      if ((strcmp(lpItemList[j].pszItem, defItem) == 0) &&
        (strcmp(lpItemList[j].format.name, defFormat) == 0) &&
        (lpItemList[j].dataSrcOID == defDataSrcOID))
        curItemIndex = j;
      }

    AMemUnlock(lpTopicList[curTopicIndex].itemListID);
    
    // if no matching item was found, then add a new one
    // else return NOOP
    if (curItemIndex == DDES_ITEMNOTFOUND)
      {
      long itemSize = AMemGetSize(lpTopicList[curTopicIndex].itemListID);
      AMemSetSize(  lpTopicList[curTopicIndex].itemListID, itemSize + sizeof(DDESITEMLIST));
      curItemIndex = lpTopicList[curTopicIndex].nItems++;
      rVal = DDES_NEWITEM;
      }
    else
      rVal = DDES_ITEMALREADYEXISTS;

    AMemUnlock(ddes->ddesService.topicListID);
    }

  if (rVal == DDES_NEWITEM)
    {
		char *mainStr = new char[4 * MAXNAME];
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
    lpItemList = (DDESITEMLIST *)AMemLock(  lpTopicList[curTopicIndex].itemListID);

    strcpy(lpTopicList[curTopicIndex].pszTopic, defTopic);
    strcpy(lpItemList[curItemIndex].pszItem, defItem);
    strcpy(lpItemList[curItemIndex].format.name, defFormat);
    lpItemList[curItemIndex].dataSrcOID = defDataSrcOID;
    lpItemList[curItemIndex].updateTime = defUpdateTime;


    mainStr[0] = '\0';
    strcat(mainStr, lpTopicList[curTopicIndex].pszTopic);
    strcat(mainStr, ":");
    strcat(mainStr, lpItemList[curItemIndex].pszItem);
    strcat(mainStr, ":");
    strcat(mainStr, lpItemList[curItemIndex].format.name);
		strcat(mainStr, ":");

		if (AObjGetName(ddes->defDataSrcOID, ObjectName, MAXNAME) == 0)
			ObjectName[0] = '\0';

    strcat(mainStr, ObjectName);

    // select the last added string
    SetMainList(hDlg, mainStr, lpItemList[curItemIndex].dataSrcOID);

    AMemUnlock( lpTopicList[curTopicIndex].itemListID);
		AMemUnlock(ddes->ddesService.topicListID);

		delete mainStr;
    }

  return curTopicIndex;

  }

/***************************************************************************
*
*	Description: In Response to PUSHING DELETE button
*
****************************************************************************/
int OBJDCLASS::DeleteMainList(
  LPSTR defTopic,
  LPSTR defItem,
  LPSTR defFormat,
  OBJECTID defDataSrcOID
  )
  {
  BOOL bDeleteAllItems = FALSE; // if all items under a topic are to be deleted
  BOOL bDeleteOneItem = FALSE;  // if only specific item is to be deleted
  int j;
  int curTopicIndex = DDES_TOPICNOTFOUND;
  int curItemIndex =  DDES_ITEMNOTFOUND;
  long topicSize = AMemGetSize(ddes->ddesService.topicListID);
  DDESTOPICLIST *lpTopicList; //  = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
  DDESITEMLIST *lpItemList;


	if ((bCanAdd & DDES_ADDEDALL) == DDES_ADDEDALL)
    bDeleteOneItem = TRUE;
  else if (bCanAdd & DDES_ADDEDTOPIC)
    bDeleteAllItems = TRUE;


  if ( (curTopicIndex = ddes->IsNewTopic(defTopic)) == DDES_TOPICNOTFOUND) // -1
    {  
    return DDES_TOPICNOTFOUND;
    }
  else
    {
		if (bDeleteAllItems)
      ddes->RemoveItem(curTopicIndex, DDES_REMOVEALLITEMS); // remove all items under this topic
    else
      {
      lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
      lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[curTopicIndex].itemListID);
      curItemIndex = DDES_ITEMNOTFOUND;
      for (j = 0; j <lpTopicList[curTopicIndex].nItems; j++)
        {
        if ((strcmp(lpItemList[j].pszItem, defItem) == 0) &&
          (strcmp(lpItemList[j].format.name, defFormat) == 0) &&
          (lpItemList[j].dataSrcOID == defDataSrcOID))
          curItemIndex = j;
        }

      AMemUnlock(ddes->ddesService.topicListID);
      AMemUnlock(lpTopicList[curTopicIndex].itemListID);

      if (curItemIndex != DDES_ITEMNOTFOUND)
        ddes->RemoveItem(curTopicIndex, curItemIndex);
      }
    }
  I_UNUSED(topicSize);
  I_UNUSED(bDeleteOneItem);

  return curItemIndex;  // the item deleted
  }


/***************************************************************************
*
*	Description: In Response to PUSHING DELETE button
*
****************************************************************************/
int OBJDCLASS::DeleteMainList()
  {
  BOOL bDeleteAllItems = FALSE; // if all items under a topic are to be deleted
  BOOL bDeleteOneItem = FALSE;  // if only specific item is to be deleted
  int j;
  int curTopicIndex = DDES_TOPICNOTFOUND;
  int curItemIndex =  DDES_ITEMNOTFOUND;
  long topicSize = AMemGetSize(ddes->ddesService.topicListID);
  DDESTOPICLIST *lpTopicList; //  = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
  DDESITEMLIST *lpItemList;


  if ((bCanAdd & DDES_ADDEDALL) == DDES_ADDEDALL)
    bDeleteOneItem = TRUE;
  else if (bCanAdd & DDES_ADDEDTOPIC)
    bDeleteAllItems = TRUE;


  if ( (curTopicIndex = ddes->IsNewTopic(ddes->defTopic)) == DDES_TOPICNOTFOUND) // -1
    {  
    return DDES_TOPICNOTFOUND;
    }
  else
    {
    if (bDeleteAllItems)
      ddes->RemoveItem(curTopicIndex, DDES_REMOVEALLITEMS); // remove all items under this topic
    else
      {
      lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
      lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[curTopicIndex].itemListID);
      curItemIndex = DDES_ITEMNOTFOUND;
      for (j = 0; j <lpTopicList[curTopicIndex].nItems; j++)
        {
        if ((strcmp(lpItemList[j].pszItem, ddes->defItem) == 0) &&
          (strcmp(lpItemList[j].format.name, ddes->defFormat.name) == 0) &&
          (lpItemList[j].dataSrcOID == ddes->defDataSrcOID))
          curItemIndex = j;
        }

      AMemUnlock(ddes->ddesService.topicListID);
      AMemUnlock(lpTopicList[curTopicIndex].itemListID);

      if (curItemIndex != DDES_ITEMNOTFOUND)
        ddes->RemoveItem(curTopicIndex, curItemIndex);
      }
    }
  I_UNUSED(topicSize);
  I_UNUSED(bDeleteOneItem);

  return curItemIndex;  // the item deleted
  }

/***************************************************************************
*
*	Description: In Response to PUSHING UPDATE button
*
****************************************************************************/
int OBJDCLASS::UpdateMainList() {
	int curTopicIndex = DDES_TOPICNOTFOUND;
  int curItemIndex =  DDES_ITEMNOTFOUND;
  long topicSize = AMemGetSize(ddes->ddesService.topicListID);
  DDESTOPICLIST *lpTopicList; //  = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
  DDESITEMLIST *lpItemList;
	int rVal;


  if ((bCanAdd & DDES_ADDEDALL) != DDES_ADDEDALL)
    return -1;

  char *Topic = new char[MAXNAME];
  char *Item    = new char[MAXNAME];
  char *Format  = new char[MAXNAME];
  OBJECTID ObjectID;

	if (GetFromMainList(Topic, Item, Format, &ObjectID) == FALSE)
    {
		delete Topic;
  	delete Item; 
  	delete Format; 
		return DDES_TOPICNOTFOUND;

    }
  if ( (curTopicIndex = ddes->IsNewTopic(Topic)) == DDES_TOPICNOTFOUND) // -1
    {  
    rVal =  DDES_TOPICNOTFOUND;
    }
  else
    {
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
    lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[curTopicIndex].itemListID);
    curItemIndex = DDES_ITEMNOTFOUND;
    // Remove Current Topic, item etc from MainList
    DeleteMainList(Topic, Item, Format, ObjectID);
    // Insert New Topic, item etc   (Take values from the def-Fields i.e. defTopic etc)
    AddMainList();
		AMemUnlock(ddes->ddesService.topicListID);
    AMemUnlock(lpTopicList[curTopicIndex].itemListID);
    }

  I_UNUSED(topicSize);
	delete Topic;
  delete Item; 
  delete Format; 

	I_UNUSED(rVal);
  I_UNUSED(lpItemList);
  return curItemIndex;  // the item deleted
  }



/***************************************************************************
*
*	Description: Set the contents of the various edit controls with the selected 
*            string from the MAIN list Box
****************************************************************************/
int OBJDCLASS::SetMainCtrls(int index)
  {
  char *mainStr = new char[4 *MAXNAME];
  HWND hCtrl = GetDlgItem(hDlg, IDMAINLIST);
  LPSTR lpTab;
  int len;
	char *Topic 	 = new char[MAXNAME];
  char *Item		= new char[MAXNAME];
	FORMATINFO Format;
	if (index > CB_ERR)
    {
    SendMessage(hCtrl, LB_GETTEXT, index, (LONG)mainStr);
    lpTab = strchr(mainStr, ']');
    len = strlen(mainStr) - strlen(lpTab);
    strncpy(Topic, mainStr, len);
    Topic[len] = '\0';
    mainStr = lpTab;

    lpTab = strchr(mainStr, ']');
    len = strlen(mainStr) - strlen(lpTab);
    strncpy(Item, mainStr, len);
    Item[len] = '\0';
    mainStr = lpTab;
  
    lpTab = strchr(mainStr, ']');
    len = strlen(mainStr) - strlen(lpTab);
    strncpy(Format.name, mainStr, len);
    Format.atom = GetFormatID(Format.name);
    Format.name[len] = '\0';
    mainStr = lpTab;

    OBJECTID objID = (OBJECTID) SendMessage(hCtrl, LB_GETITEMDATA, index, 0L);
    TYPEID typeID = AObjGetType(objID);
    ChangeName(Topic, IDTOPIC);
    ChangeName(Item, IDITEM);
    ChangeName((LPSTR)&Format, IDFORMAT);
    ChangeName((LPSTR)&typeID, IDOBJTYPE);
    ChangeName((LPSTR)&objID, IDOBJECT);

    }
  else {
    // Disable all controls
    // set Text as first selection
    // SKJ, Do I need to disable controls, or should I just check for bCanAdd before adding

		}

	delete Topic;
	delete Item;
	delete mainStr;

  return 1;
  }
/****************************************************************************
* Function:     SetMainList
*
* Description:  This functions will add a string to the main list box and set it as
*               current selection.
*               If the string being added is empty it will select the first item
*               The ObjectID is added as the itemData
*
* Return:       index of the string added
*
*****************************************************************************/

int OBJDCLASS::SetMainList(HWND hWnd, LPSTR itemStr, OBJECTID objID)
  {

  int index = 0;
  HWND hCtrl = GetDlgItem(hWnd, IDMAINLIST);

  if (strcmp(itemStr, "") == 0)
    return -1;
	char *Topic = new char[MAXNAME];
	char *Format = new char[MAXNAME];
	char *Item	= new char[MAXNAME];

  if ((itemStr[0] != '\0') &&
	 ((index = (int)SendMessage(hCtrl, LB_FINDSTRINGEXACT,-1, (LONG)(LPSTR)itemStr)) == CB_ERR) )
    {
		index = (int)SendMessage(hCtrl, LB_ADDSTRING, 0, (LONG)(LPSTR)itemStr);
    
    if (index == CB_ERR)
      index = 0;
    else
      SendMessage(hCtrl, LB_SETITEMDATA, index, (LONG)objID);
		}
	SendMessage(hCtrl, LB_SETCURSEL, index, 0L);

	// Enable the controls based on selection
	CheckForSystem(index);

	delete Topic;
	delete Format;
	delete Item;

  return index;
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

int OBJDCLASS::SetSignalList(LPSTR itemStr, OBJECTID oid)
  {
  int index = 0;
  HWND hCtrl = GetDlgItem(hDlg, IDSIGNALLIST);

  if (strcmp(itemStr, "") == 0)
    return -1;

  if ((itemStr[0] != '\0') &&
	 ((index = (int)SendMessage(hCtrl, LB_FINDSTRINGEXACT,-1, (LONG)(LPSTR)itemStr)) == CB_ERR) )
    {
		index = (int)SendMessage(hCtrl, LB_ADDSTRING, 0, (LONG)(LPSTR)itemStr);
    
    if (index == CB_ERR)
      index = 0;
    else
      SendMessage(hCtrl, LB_SETITEMDATA, index, (LONG)oid);
    }
  SendMessage(hCtrl, LB_SETCURSEL, index, 0L);
  return index;
  }



/****************************************************************************
* Function:     BuildDefMainStr
*
* Description:  This functions will create a main list string from the def topic,
*               item , format, oid
* Return:
*
*****************************************************************************/

BOOL OBJDCLASS:: BuildDefMainStr(LPSTR mainStr)
  {
  mainStr[0]='\0';

  if (strcmp(ddes->defTopic, "") == 0) 
    return FALSE;
  char *name = new char[MAXNAME];

  strcat(mainStr, ddes->defTopic);
  strcat(mainStr, ":");
  strcat(mainStr, ddes->defItem);
  strcat(mainStr, ":");
  strcat(mainStr, ddes->defFormat.name);
  strcat(mainStr, ":");
  if (strcmp(ddes->defTopic, SZDDESYS_TOPIC) == 0)
    {
    strcpy(name, "N/A");
    }
	else {
		if ( AObjGetName(ddes->defDataSrcOID, name, MAXNAME) == 0)
			name[0] = '\0';
		}
  strcat(mainStr, name);
  delete name;
  return TRUE;
  }


/****************************************************************************
* Function:     BuildMainList
*
* Description:  This function will build a list TOPIC:ITEM:FORMAT:OBJECT
*               during the INITDIALOG processing
*
*****************************************************************************/
void OBJDCLASS::BuildMainList()
  {
  int           i,j;
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddes->ddesService.topicListID);
  DDESITEMLIST  *lpItemList;
  MEMBLOCKID    delID = AMemAllocate(0);
  DELRECORD     *delRec;
  int           delCount = 0;
  char          *mainStr = new char[(MAXNAME *4) + 3] ;
  char          *name   = new char[MAXNAME];
  // empty the MainList
  SendMessage(GetDlgItem(hDlg,IDMAINLIST), LB_RESETCONTENT, 0, 0L);

  for (i = 0; i < ddes->ddesService.nTopics; i++)
    {
    lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[i].itemListID);
    for (j = 0; j <lpTopicList[i].nItems; j++)
      {
    
      // if it is not the service topic and the oid is not valid
      if ((strcmp(lpTopicList[i].pszTopic, SZDDESYS_TOPIC) != 0)
        && (AObjCheckType(lpItemList[j].dataSrcOID, 0) == FALSE))
        {
        AMemSetSize(delID , AMemGetSize(delID) + sizeof(DELRECORD));
        delRec = (DELRECORD *) AMemLock(delID);
        lstrcpy(delRec[delCount].pszTopic, lpTopicList[i].pszTopic);
        lstrcpy(delRec[delCount].pszItem, lpItemList[j].pszItem);
        delRec[delCount].format = lpItemList[j].format;
        delRec[delCount].dataSrcOID = lpItemList[j].dataSrcOID;
        delCount++;
        continue;
        }

      _fmemset(mainStr, 0,MAXNAME *4 + 3);

      mainStr[0]='\0';
      strcat(mainStr, lpTopicList[i].pszTopic);
      strcat(mainStr, ":");
      strcat(mainStr, lpItemList[j].pszItem);
      strcat(mainStr, ":");
      strcat(mainStr, lpItemList[j].format.name);
      strcat(mainStr, ":");
      if (strcmp(lpTopicList[i].pszTopic, SZDDESYS_TOPIC) == 0)
        {
        strcpy(name, "N/A");
        }
      else {
				if (AObjGetName(lpItemList[j].dataSrcOID, name, MAXNAME) == 0)
					name[0] = '\0';

        }
      strcat(mainStr, name);
      // select the last added string
      SetMainList(hDlg, mainStr, lpItemList[j].dataSrcOID);
      }
    AMemUnlock(lpTopicList[i].itemListID);
    }
  AMemUnlock(ddes->ddesService.topicListID);

  

  // delete all the delete oid references
  for (i = 0; i < delCount; i++)
    {
    DeleteMainList(delRec[i].pszTopic, delRec[i].pszItem, delRec[i].format.name,delRec[i].dataSrcOID);
    }

  // After adding all the strings, select the def string in the list or the
  // first item in the MAIN list
  BuildDefMainStr(mainStr);
	int curIndex = (int)SendMessage(GetDlgItem(hDlg, IDMAINLIST),LB_SELECTSTRING, 0, (long) mainStr);
  if (curIndex == LB_ERR) {
    curIndex  = 0;
    // select the first string 
		SendMessage(GetDlgItem(hDlg, IDMAINLIST), LB_SETCURSEL, curIndex, 0L);

    }
  // Enable the ctrls
  CheckForSystem(curIndex);

  delete mainStr;
  delete name;
  I_UNUSED(lpTopicList);
  I_UNUSED(lpItemList);

  }

/***************************************************************************
*                                                                          *
*  METHOD    :  BuildObjTypeList()                                         *
*                                                                          *
*  DESCRIPTION    : Builds a Serius Object Type list from a chosen format                          *
*                                                                          *
*  RETURNS    :                                                            *
*                                                                          *
****************************************************************************/
int OBJDCLASS::BuildObjTypeList(ATOM formatID)
	{
  
  long typecount = ATypeGetCount();
	TYPEINFO *typeList = (TYPEINFO *) new TYPEINFO[(int)typecount];
	long int i;
  TYPEID osType;
  HWND hTypeWnd = GetDlgItem(hDlg, IDOBJTYPE);
  int  curindex = 0;
  char  name[MAXNAME];
  typecount = 0;

  switch (formatID)
    {

    case CF_TEXT:
      {
			i = AOBJ_GETFIRSTKEY;
			char *name = new char[80];

			while ((osType = ATypeGetNext(&i)) != (TYPEID)0L) {
				ATypeGetName(osType, name, 20);
				if (!AEvtCheckForMethod(osType, AEVENT_TEXTTOOBJECT))
          continue;
        strncpy(typeList[(int)typecount].name, name, MAX_NAME);
        typeList[(int)typecount].name[MAX_NAME - 1] = '\0';
        typeList[(int)typecount].typeID = osType;
				typecount++;
				}

			delete name;
			}
			break;

    case CF_BITMAP:
      {
			i = AOBJ_GETFIRSTKEY;
			char *name = new char[80];

      while ((osType = ATypeGetNext(&i)) != (TYPEID)0L) {
		 
        ATypeGetName(osType, name, 20);


        if (!AEvtCheckForMethod(osType, (EVENTID) OPICT_EVT_BITMAPTOOBJECT)) // bitmapToObject
          continue;

        strncpy(typeList[(int)typecount].name, name, MAX_NAME);
        typeList[(int)typecount].name[MAX_NAME - 1] = '\0';
        typeList[(int)typecount].typeID = osType;

        typecount++;
				}
			delete name;
			}
			break;

    case CF_METAFILEPICT:
      {
			i = AOBJ_GETFIRSTKEY;
      char	*name = new char[80];
      while ((osType = ATypeGetNext(&i)) != (TYPEID) 0L)
        {
				ATypeGetName(osType, name, 20);
				if (!AEvtCheckForMethod(osType, OPICT_EVT_METAFILETOOBJECT)) //   metafileToObject))
          continue;
				strncpy(typeList[(int)typecount].name, name, MAX_NAME);
				typeList[(int)typecount].name[MAX_NAME - 1] = '0';
				typeList[(int)typecount].typeID = osType;
				typecount++;
        }
			delete name;

      }
      break;
    
    // case aFormat[CFORMATS- 1]: // almObject
    default:
      {
      if (formatID != CF_ALMOBJ)
        return 0;
			i = AOBJ_GETFIRSTKEY;
			char *name = new char[80];

      while ((osType = ATypeGetNext(&i)) != (TYPEID) 0L) {

        ATypeGetName(osType, name, 20);

				strncpy(typeList[(int)typecount].name, name, MAX_NAME);
				typeList[(int)typecount].name[MAX_NAME - 1] = '0';
				typeList[(int)typecount].typeID = osType;

        typecount++;
        }
			delete name;
			}
      break;
		}
	SendMessage(hTypeWnd, CB_RESETCONTENT, 0,0);
	for (i = 0; i < typecount; i++)
    {
		if (SendMessage(hTypeWnd, CB_FINDSTRINGEXACT, -1,(LONG)(LPSTR)typeList[(int)i].name) == CB_ERR)
      {
			curindex = (int)SendMessage(hTypeWnd, CB_ADDSTRING, 0,(LONG)(LPSTR)typeList[(int)i].name);
			SendMessage(hTypeWnd, CB_SETITEMDATA, curindex, (long)typeList[(int)i].typeID);
      }
    }

	if (ddes->defObjectType != 0L)
		{
		ATypeGetName(ddes->defObjectType, name, MAXNAME);
		curindex = (int)SendMessage(hTypeWnd,CB_SELECTSTRING, 0, (long) name);
		}
	else {
		name[0] = '\0';
		curindex = CB_ERR;
    }
  if (curindex == CB_ERR) {
		curindex  = 0;
    // select the first string 
		SendMessage(hTypeWnd, CB_SETCURSEL, curindex, 0L);

    }

	// Get the TYPE id of the current Type selected
  osType = (TYPEID)SendMessage(hTypeWnd,CB_GETITEMDATA, curindex,0);
	// Build the object list from the chosen type
  BuildObjectList(osType);
  SetObjType(osType);
  delete typeList;
	return 0;
  }

/***********************************************************************************************
*
*
*
***********************************************************************************************/
BOOL OBJDCLASS::SetCtrlsFromMainLst()
  {
	if (GetFromMainList(ddes->defTopic, ddes->defItem, ddes->defFormat.name, &ddes->defDataSrcOID) == FALSE)
		return FALSE;
	char *objectName = new char[MAXNAME];
	char *objTypeName = new char[MAXNAME];

	SetDlgItemText(hDlg, IDTOPIC, ddes->defTopic);
  SetDlgItemText(hDlg, IDITEM, ddes->defItem);

	SetComboItem(hDlg, IDFORMAT, ddes->defFormat.name);
	SetAddFlag(DDES_ADDEDTOPIC|DDES_ADDEDITEM|DDES_ADDEDFORMAT);

  // BuildObjTypeList(oType);
  ddes->defObjectType = AObjGetType(ddes->defDataSrcOID);

  // in case of "SYSTEM" topics
  if (strcmp(ddes->defTopic, SZDDESYS_TOPIC) == 0)
    {
    strcpy(objTypeName, "N/A");
		strcpy(objectName, "N/A");
		ResetAddFlag(DDES_ADDEDOBJECT);

    }
  else {
		ATypeGetName(ddes->defObjectType, objTypeName, MAXNAME);
		if (AObjGetName(ddes->defDataSrcOID, objectName, MAXNAME) == 0)
			objectName[0] = '\0';

		//Type check
		if (AObjCheckType(ddes->defDataSrcOID, 0) == TRUE)
			SetAddFlag(DDES_ADDEDOBJECT);
		}

  SetComboItem(hDlg, IDOBJTYPE, objTypeName);
  SetComboItem(hDlg, IDOBJECT, objectName);
  delete objectName;
  delete objTypeName;
  return TRUE;
  }

/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SetCtrlsFromSignalList()
  {
	TYPEID  objType;
  if (GetFromSignalList(&defSignal) == FALSE)
    return FALSE;
	char *objectName = new char[MAXNAME];
	char *objTypeName = new char[MAXNAME];

  SetDlgItemText(hDlg, IDSIGNAL, defSignal.name);
  objType = AObjGetType(defSignal.rcvrOID);
  ATypeGetName(objType, objTypeName, MAXNAME);
	if (AObjGetName(defSignal.rcvrOID, objectName, MAXNAME) == 0)
		objectName[0] = '\0';

  SetComboItem(hDlg, IDOBJTYPE, objTypeName);
	SetComboItem(hDlg, IDOBJECT, objectName);

	ddes->defObjectType = objType;
	ddes->defDataSrcOID = defSignal.rcvrOID;

	delete objectName;
  delete objTypeName;
  return TRUE;
  }

/***************************************************************************
*  METHOD    :  BuildObjectList()                                          *
*                                                                          *
*  DESCRIPTION    : Builds a Serius Object  list from a chosen type        *                         *
*                                                                          *
*  RETURNS    :                                                            *
*                                                                          *
****************************************************************************/
int OBJDCLASS::BuildObjectList(TYPEID osType)
  {                                         
	long typecount = ATypeGetCount();
	long objCount = AObjGetObjectCount(osType);
  char  objName[MAXNAME];

  long int i;
  int ctrlID = IDOBJECT;
  HWND hObjWnd = GetDlgItem(hDlg, IDOBJECT);
  int curindex;
  OBJECTID oid = 0;


  // if only type is DDEC object
  if ((typecount <= 1) || (objCount == 0))
    {
		SendDlgItemMessage(hDlg, ctrlID, CB_RESETCONTENT, 0,0);
		// added correct service
		ResetAddFlag(DDES_ADDEDOBJECT);
		// bCanAdd &= ~DDES_ADDEDOBJECT;  

    // EnableWindow(hObjWnd, FALSE );
    return FALSE;
    }
  else {
    // EnableWindow(hObjWnd, TRUE );
    }
  // Only display objects of the TYPE that the
  // user chooses.
  
  // Only change it if the selection is different from old selection
  // or if there is no current selection
	curindex = (int)SendMessage(hObjWnd, CB_GETCURSEL, 0,0);

  if ((osType != ddes->defObjectType) || (curindex == CB_ERR))
    {
    BOOL bNoObject = TRUE;  // no object of this type
    OBJECTID objRec;
    SendDlgItemMessage(hDlg, ctrlID, CB_RESETCONTENT, 0,0);

    ddes->defObjectType = osType;
    i = AOBJ_GETFIRSTKEY;


    curindex = 0;
    while ((objRec = AObjGetNextObject(osType, &i)) != (OBJECTID)0){
			if (AObjGetName(objRec, objName, MAXNAME) == 0)
				objName[0] = '\0';

			curindex = (int)SendDlgItemMessage(hDlg, ctrlID, CB_ADDSTRING, 0, (LONG)(LPSTR)objName);
      SendDlgItemMessage(hDlg, ctrlID, CB_SETITEMDATA, curindex,(long)objRec);
      bNoObject = FALSE;
      }

		if (bNoObject)
			ResetAddFlag(DDES_ADDEDOBJECT);  // added correct service
    else {
      curindex = 0;
			if (AObjGetName(ddes->defDataSrcOID, objName, MAXNAME) == 0)
				objName[0] = '\0';

			curindex = (int)SendMessage(hObjWnd,CB_SELECTSTRING, 0, (long) objName);
      if (curindex == CB_ERR) {
        curindex  = 0;
        // select the first string 
				if (SendMessage(hObjWnd, CB_SETCURSEL, curindex, 0L) != LB_ERR)
					SetAddFlag(DDES_ADDEDOBJECT);  // added correct service
				}
			else
				SetAddFlag(DDES_ADDEDOBJECT);  // added correct service
				// bCanAdd |= DDES_ADDEDOBJECT;  // added correct service
      }


    // Get the TYPE id of the current Type selected
    oid = (OBJECTID)SendMessage(hObjWnd,CB_GETITEMDATA, curindex,0);

    SetObject(oid);
    // Build the object list from the chosen type

		}
	else {
    
		int index = (int)SendMessage(hObjWnd, CB_FINDSTRINGEXACT,0,(LONG)"N/A");
    if (index != CB_ERR)
			SendMessage(hObjWnd, CB_DELETESTRING, index, 0);
    }

  return 0;
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
	LPDEFOBJDDATA lpDefData; // =  (LPDEFOBJDDATA)AObjLockData(objID, data3_DEFVALS);
  TYPEID  typeID = 0;
  // Init DDE Client Call Back;
  // serviceCount = 1;
  hDlg = hWnd;
  topicCount = 1;
  formatCount = 0;
  objTypeCount = 0;;
  objectCount = 0;

  // this flag should be init to 0 to start
  bCanAdd = 0;

  ddes = (DDES *)new DDES(objID);


	defSignal.name[0] = '\0';
	defSignal.sigID = 0;
	if (ddes->nUserSignals) {
		defSignal.rcvrOID = ddes->userSignals[0].rcvrOID;
		strcpy(defSignal.name, ddes->userSignals[0].name);
		}
  else
  	defSignal.rcvrOID =   0L;

  ddes->bDisplayErr = FALSE;
	ChangeName(ddes->ddesService.Service, IDSERVICE);

	// The first item in the service list will be a userdefined string
  lpDefData =  (LPDEFOBJDDATA)AObjLockData(objID, data3_DEFVALS);
  // if the old src oid is valid
  if (AObjCheckType(ddes->defDataSrcOID, 0))
    {
    typeID = AObjGetType(lpDefData->defDataSrcOID);
    ATypeGetName(typeID, typeName,  MAXNAME);
		if (AObjGetName(ddes->defDataSrcOID, objName,  MAXNAME) == 0)
			objName[0] = '\0';

    ChangeName((LPSTR)&typeID, IDOBJTYPE);
    ChangeName((LPSTR)&(lpDefData->defDataSrcOID), IDOBJECT);
    SetComboItem (hWnd, IDOBJTYPE, typeName);
    SetComboItem (hWnd, IDOBJECT, objName);
    }

  // Enable all the controls
  ChangeName(lpDefData->defTopic, IDTOPIC);
  ChangeName(lpDefData->defItem, IDITEM);
  ChangeName((LPSTR)&(lpDefData->defFormat), IDFORMAT);
  ChangeName((LPSTR)&(lpDefData->defUpdateTime), IDTIME);
  AObjUnlockData(objID, data3_DEFVALS);

  SetWindowText(GetDlgItem(hWnd, IDSERVICE),ddes->ddesService.Service);
  SetWindowText(GetDlgItem(hWnd, IDTOPIC),  ddes->defTopic);
  SetWindowText(GetDlgItem(hWnd, IDITEM),   ddes->defItem);
  SetComboItem (hWnd, IDFORMAT, ddes->defFormat.name);
	SetDlgItemInt(hWnd, IDTIME,  (int)lpDefData->defUpdateTime, FALSE);


  // create slot to hold OBJDCLASS ptr
  AObjGetDataCount(objID);
  // Set the runtime info slot to hold the OBJDCLASS ptr
  AObjSetRuntimeInfo(objID, (LONG)this);
	BuildFormatList(); // add the def list of formats to format control;
	BuildMainList();

	// Init the controls based on current selection
	SetCtrlsFromMainLst();
	delete typeName;
	delete objName;
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
	OBJECTID oid = ddes->ddesOID;
	delete ddes; // delete the DDES class instanced
	// Set the runtime info slot to hold NULL
	AObjSetRuntimeInfo(oid, (LONG)NULL);
  ddes = NULL;
  }



/*
int DoTransaction(LPSTR itemStr, MEMBLOCKID txtID)
  {
  // SKJ Build a List with the returned data
  pxact->hszItem = DdeCreateStringHandle(idInst,itemStr, NULL);
  dde->ProcessTransaction();
  if (dde->pxact->ret)
    {
    GetTextData((HDDEDATA)dde->pxact->ret, txtID);
  
    }
  else
    AMemSetSize(txtID, 0);

  DdeFreeStringHandle(idInst, dde->pxact->hszItem);

  return dde->pxact->ret;;
  }
*/

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
    if ((objd == NULL) || (!objd->IsDDE()))
      return NULL;
    }
  else if (msg == WM_INITDIALOG)
    {
    objd = new OBJDCLASS(hwnd , theObject);
    }
  I_UNUSED(lpPtr);
  return (LPLONG)objd;
  }


/**********************************************************************************
* Used to Build the Language List during the Init the Options Dlg box
*
*
***********************************************************************************/
void BuildLanguage(HWND hwnd, OBJDCLASS *objd)
	{
  int i;
	int size = sizeof(langList) / sizeof(LANGUAGELIST);
  int index;
	for (i = 0; i < size; i++)
		{

		index = SetComboItem(hwnd,IDEF_LANG, langList[i].Language);
		(int)SendMessage(GetDlgItem(hwnd, IDEF_LANG), CB_SETITEMDATA,index, langList[i].id);


		}
	I_UNUSED(objd);
	}

/**********************************************************************************
* Used to Build the Code Page List during the Init the Options Dlg box
*
*
***********************************************************************************/
void BuildCodePage(HWND hWnd, OBJDCLASS *objd)
	{
	int i;
	int size = sizeof(kbCode) / sizeof(KBCPAGE);
	int index;

	for (i = 0; i < size; i++)
		{

		index = SetComboItem(hWnd,IDEF_CODEPAGE, kbCode[i].name);
		(int)SendMessage(GetDlgItem(hWnd, IDEF_CODEPAGE), CB_SETITEMDATA,index, kbCode[i].id);


		}

	I_UNUSED(objd);
	}

/**********************************************************************************
* Used to Build the Coountry List during the Init the Options Dlg box
*
*
***********************************************************************************/
void BuildCountry(HWND hwnd, OBJDCLASS *objd)
	{
	int i;
	int size = sizeof(countryList) / sizeof(COUNTRYLIST);
  int index;
	for (i = 0; i < size; i++)
		{

		index = SetComboItem(hwnd,IDEF_COUNTRY, countryList[i].Country);
		(int)SendMessage(GetDlgItem(hwnd, IDEF_COUNTRY), CB_SETITEMDATA,index, countryList[i].id);

		}
	
 	I_UNUSED(objd);
	}



/**************************************************************************
*
* 	
*
***************************************************************************/
void SetOptions(OBJDCLASS *objd, int ctrlID, long val)
	{

	DDES *dde = objd->ddes;

	switch (ctrlID)
		{
		case IDEF_SECURITY:
			{
      dde->CCFilter.dwSecurity = val;
			}
			break;

		case IDEF_COUNTRY:
			{
			dde->CCFilter.wCountryID = (UINT)val;
			}
			break;

		case IDEF_LANG:
			{
			dde->CCFilter.dwLangID = (long)val;
			}
			break;

		case IDEF_CODEPAGE:
			{
			dde->CCFilter.iCodePage = (int)val;
			}
			break;
		case IDDEFAULT:
			dde->bDefContext = (BOOL)val;
			break;

		default:

			;

    }
	}



/**************************************************************************
*
* 	
*
***************************************************************************/
void SetOptionsStr(OBJDCLASS *objd, int ctrlID, LPSTR lpStr)
	{
	DDES *dde = objd->ddes;
	switch (ctrlID)
		{
		case IDEF_COUNTRY:
			{
			strncpy(dde->Country, lpStr, MAX_NAME);
			dde->Country[MAX_NAME - 1] = '\0';
			}
			break;

		case IDEF_LANG:
			{
			strncpy(dde->Language, lpStr, MAX_NAME);
			dde->Language[MAX_NAME - 1] = '\0';
			}
			break;

		case IDEF_CODEPAGE:
			{
			strncpy(dde->CodePage, lpStr, MAX_NAME);
			dde->CodePage[MAX_NAME - 1]  = '\0';
			}
			break;

    default:
			break;

    }
	}


/**************************************************************************
*
* 	
*
***************************************************************************/
LPSTR GetOptionsStr(OBJDCLASS *objd, int ctrlID)
	{
	DDES *dde = objd->ddes;

	switch (ctrlID)
		{
		
		case IDEF_COUNTRY:
			{
			return dde->Country;
			}
		
		case IDEF_LANG:
			{
			return dde->Language;
			}
		
		case IDEF_CODEPAGE:
			{
			return dde->CodePage;
			}
		
    default:
			return 0;

		}
	
	}

/**************************************************************************
*
* 	
*
***************************************************************************/
long int GetOptions(OBJDCLASS *objd, int ctrlID)
	{
  DDES *dde = objd->ddes;
	switch (ctrlID)
		{
		case IDDEFAULT:
			{
			long rVal =  (long)dde->bDefContext;
			return rVal;
			}

		case IDEF_SECURITY:
			{
      return dde->CCFilter.dwSecurity;
			}
     
		case IDEF_COUNTRY:
			{
			return dde->CCFilter.wCountryID;
			}

		case IDEF_LANG:
			{
			return dde->CCFilter.dwLangID;
			}

		case IDEF_CODEPAGE:
			{
			return (long int)dde->CCFilter.iCodePage;
			}

		default:

	    return 0;

		}

  }

/**********************************************************************************
* Init the Options Dlg box
*
*
*
***********************************************************************************/
void InitOptions(HWND hWnd, OBJDCLASS *objd)
	{
  BOOL defVal;
	BuildCountry(hWnd, objd);
	BuildLanguage(hWnd, objd);
	BuildCodePage(hWnd, objd);


	SetDlgItemInt(hWnd, IDEF_SECURITY, (UINT)GetOptions(objd, IDEF_SECURITY), TRUE);
	SetComboItem(hWnd, IDEF_COUNTRY, GetOptionsStr(objd, IDEF_COUNTRY));
	SetComboItem(hWnd, IDEF_LANG, GetOptionsStr(objd, IDEF_LANG));
	SetComboItem(hWnd, IDEF_CODEPAGE, GetOptionsStr(objd, IDEF_CODEPAGE));

	defVal = GetOptions(objd, IDDEFAULT) ? TRUE:FALSE;
	EnableWindow(GetDlgItem(hWnd, IDEF_COUNTRY), !defVal);
	EnableWindow(GetDlgItem(hWnd, IDEF_LANG),!defVal);
	EnableWindow(GetDlgItem(hWnd, IDEF_CODEPAGE),!defVal);
	EnableWindow(GetDlgItem(hWnd, IDEF_SECURITY),!defVal);

	(int)SendMessage(GetDlgItem(hWnd, IDDEFAULT), BM_SETCHECK, (WORD)defVal, 0);

  }

/**********************************************************************************
* Save FROM the Options Dlg box
*
*
*
***********************************************************************************/

void SaveOptions(HWND hWnd, OBJDCLASS *objd)
	{
	int val;
	BOOL flag;
	char lpStr[MAXNAME];

	// the IDDEFAULT field is already set
	if (objd->ddes->bDefContext)
		{
		// Set the default CCFilter (CONV. context)
		_fmemset(&(objd->ddes->CCFilter), 0, sizeof(CONVCONTEXT));
		GetProfileString("intl", "sCountry", "United States", objd->ddes->Country, MAXNAME);
		GetProfileString("intl", "sLanguage", "U.S. English",objd->ddes->Language, MAXNAME);
		strcpy(objd->ddes->Language, "Modern English");
		strcpy(objd->ddes->CodePage,"ANSI");
		objd->ddes->CCFilter.iCodePage  = CP_WINANSI;
		objd->ddes->CCFilter.wCountryID = GetProfileInt("intl", "iCountry", 1);
		objd->ddes->CCFilter.dwLangID = atol(objd->ddes->Language);
		objd->ddes->CCFilter.cb = sizeof(CONVCONTEXT);
		objd->ddes->CCFilter.dwSecurity = 0;
		}
  else {
		SetOptions(objd, IDEF_SECURITY,GetDlgItemInt(hWnd, IDEF_SECURITY, &flag, FALSE));
		GetWindowText(GetDlgItem(hWnd, IDEF_COUNTRY), lpStr, MAXNAME);
		val = (int)SendMessage(GetDlgItem(hWnd, IDEF_COUNTRY), CB_GETCURSEL, 0,0L);
		SetOptions(objd, IDEF_COUNTRY, (LONG)(int)SendMessage(GetDlgItem(hWnd, IDEF_COUNTRY), CB_GETITEMDATA, val, 0));
		SetOptionsStr(objd, IDEF_COUNTRY, lpStr);
		GetWindowText(GetDlgItem(hWnd, IDEF_LANG), lpStr, MAXNAME);
		val = (int)SendMessage(GetDlgItem(hWnd, IDEF_LANG), CB_GETCURSEL, 0,0L);
		SetOptions(objd, IDEF_LANG,  (LONG)(int)SendMessage(GetDlgItem(hWnd, IDEF_LANG), CB_GETITEMDATA, val, 0));
		SetOptionsStr(objd, IDEF_LANG, lpStr);
		GetWindowText(GetDlgItem(hWnd, IDEF_CODEPAGE), lpStr, MAXNAME);
		val = (int)SendMessage(GetDlgItem(hWnd, IDEF_CODEPAGE), CB_GETCURSEL, 0,0L);
		SetOptions(objd, IDEF_CODEPAGE,  (LONG)(int)SendMessage(GetDlgItem(hWnd, IDEF_CODEPAGE), CB_GETITEMDATA, val, 0));
		SetOptionsStr(objd, IDEF_CODEPAGE, lpStr);
    }

	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :     OptionsDialogBox                                       *
 *                                                                          *
 *  PURPOSE    :     To get users input on  DDE Conversation Options                                                       *
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
extern "C" int FAR PASCAL _export OptionsDlgProc (
  HWND          hwnd,
  WORD          msg,
  WORD          wParam,
  LONG          lParam)
  {
    static OBJECTID theObject;
    static OBJDCLASS *objd;
		static BOOL bDlgHasFocus = TRUE;

    switch (msg){
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
					char title[42];
          objd = (OBJDCLASS *) lParam;
					// SetWindowText(GetDlgItem(hwnd, IDSERVICE), objd->Service());
					ATypeGetName(OTYPE_DDES, (LPSTR)title, 32);
          strcat(title, " Options");
					AUtlSetTitleFont((TYPEID)OTYPE_DDES,  title,  GetDlgItem(hwnd, IDTITLE));

					// SKJ Nov. 2, 1994 Set Title
					AObjGetName((OBJECTID)objd->ddes->ddesOID, (LPSTR)title, 32);
					SetWindowText(hwnd, (LPSTR)title);

					AUtlCenterDialog(hwnd, 0);
					InitOptions(hwnd , objd);
          }
          break;

        case WM_COMMAND:
            switch (wParam){
                case IDDEFAULT: // choose the default conv context
                  {
                  if (HIWORD(lParam) == BN_CLICKED)
                    {
										BOOL newVal;
										BOOL oldVal = GetOptions(objd, IDDEFAULT)?TRUE:FALSE;
										newVal = !oldVal;

										SetOptions(objd, IDDEFAULT, (long)newVal);
                  
                    EnableWindow(GetDlgItem(hwnd, IDEF_COUNTRY), !newVal);
                    EnableWindow(GetDlgItem(hwnd, IDEF_LANG),!newVal);
                    EnableWindow(GetDlgItem(hwnd, IDEF_CODEPAGE),!newVal);
                    EnableWindow(GetDlgItem(hwnd, IDEF_SECURITY),!newVal);

                    }

                  }

                  return FALSE;

								case IDOK:
                	{
									SaveOptions(hwnd, objd);
									EndDialog(hwnd, TRUE);
                  }
									break;
                case IDCANCEL:
                  EndDialog(hwnd, 0);
                  break;

                default:
                    return FALSE;
            }
            break;

        default:
            return(FALSE);
    }

  I_UNUSED(theObject);
  return TRUE;
  }


/****************************************************************************
*                                                                          *
*  METHOD    : BuildFormatListForSignals()                                 *
*                                                                          *
*  DESCRIPTION    : Build A FORMAT list for signals               
*  RETURNS    :                                                            *
*                                                                          *
****************************************************************************/
int OBJDCLASS::BuildFormatListForSignals()
	{
	int i;
  HWND hCtrl = GetDlgItem(hDlg, IDFORMAT);
  int curindex;
  // if there are no valid formats, then allow the user to choose from
  // any of the predetermined list of formats.  The error check in this
  // will not be done till runtime
  SendMessage(hCtrl, CB_RESETCONTENT, 0,0);

	for (i = 0; i < N_SIGFORMATS; i++)
    {
    
		curindex = (int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)sigFormats[i].name);
    SendMessage(hCtrl, CB_SETITEMDATA, curindex,sigFormats[i].atom);
    }

	curindex = (int) SendMessage(hCtrl,CB_SELECTSTRING, 0, (long) ddes->defFormat.name);
	if (curindex == CB_ERR) {
    curindex  = 0;
    // select the first string 
		SendMessage(hCtrl, CB_SETCURSEL, curindex, 0L);
		SendMessage(hCtrl,CB_GETLBTEXT, curindex, (long) ddes->defFormat.name); // set the new def
		}

	ATOM formatType = (int)SendMessage(hCtrl, CB_GETITEMDATA, curindex, 0L);
	SetAddFlag(DDES_ADDEDFORMAT);
	// bCanAdd |= DDES_ADDEDFORMAT;  // added correct service
  // Build the object list from the chosen type
  BuildObjTypeList(formatType);

  return curindex;

	}


/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::InitSigCtrls()
  {

  if (defSignal.rcvrOID && (AObjCheckType(defSignal.rcvrOID, 0)) )
		{
		char typeName[MAXNAME];
    char objName[MAXNAME];
    TYPEID typeID = AObjGetType(defSignal.rcvrOID);
    ATypeGetName(typeID, typeName,  MAXNAME);
		if (AObjGetName(defSignal.rcvrOID, objName,  MAXNAME) == 0)
			objName[0] = '\0';


		ddes->defFormat.atom = CF_TEXT;
		strcpy(ddes->defFormat.name, CFTEXT);
		ddes->defObjectType = typeID;  // save the object type

		ChangeName((LPSTR)&typeID, IDOBJTYPE);
    ChangeName((LPSTR)&(defSignal.rcvrOID), IDOBJECT);
    SetWindowText(GetDlgItem(hDlg, IDSIGNAL),defSignal.name);
    SetComboItem (hDlg, IDOBJTYPE, typeName);
		SetComboItem (hDlg, IDOBJECT, objName);
		SetAddFlag(DDES_ADDEDSIGNAL);
		SetAddFlag(DDES_ADDEDSIGOBJECT); // object addded

		}

	BuildFormatListForSignals();
	BuildSignalList();

	if (SendDlgItemMessage(hDlg, IDOBJECT, CB_GETCURSEL, 0, 0L) != CB_ERR)
		SetAddFlag(DDES_ADDEDSIGOBJECT); // object addded

	defSignal.rcvrOID = ddes->defDataSrcOID;
  return FALSE;
  }

/**************************************************************************
*
* 	
*
***************************************************************************/
BOOL OBJDCLASS::SaveSysHelp()
	{
	HWND hEdit = GetDlgItem(hDlg, IDHELPDATA);
	int textSize = GetWindowTextLength(hEdit);
	AMemSetSize(ddes->helpMemID, textSize + 1);
	LPSTR lpHelp = (LPSTR)AMemLock(ddes->helpMemID);
	if (lpHelp)
   {
		GetWindowText(hEdit, lpHelp, textSize + 1);
		lpHelp[textSize] = '\0';
		AMemUnlock(ddes->helpMemID);
		}
	return TRUE;
	}


/**************************************************************************
*
* 	
*
***************************************************************************/
void OBJDCLASS::InitSysHelp()
	{
	HWND hEdit = GetDlgItem(hDlg, IDHELPDATA);
	// int textSize = AMemGetSize(ddes->helpMemID);
	LPSTR lpHelp = (LPSTR)AMemLock(ddes->helpMemID);
  if (lpHelp) {
		SetWindowText(hEdit, lpHelp);
		AMemUnlock(ddes->helpMemID);
		}
	}

/**************************************************************************
*
* check to see if any system topic:items have been selected
*
***************************************************************************/
BOOL OBJDCLASS::CheckForSystem(int curSel)
  {
  BOOL bEnable = FALSE;
	OBJECTID oid = 0;
	if (curSel == LB_ERR)
    {
		bEnable = TRUE;
    }
	else if (curSel >- 0)
    {
		oid = (OBJECTID)SendMessage(GetDlgItem(hDlg, IDMAINLIST), LB_GETITEMDATA, curSel, 0L);
		}

  

  if (oid || bEnable)
		{

		EnableWindow(GetDlgItem(hDlg, IDFORMAT), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDOBJTYPE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDOBJECT), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDADD), TRUE);
		// Enable the DELETE and UPDATE buttons only when selection is not system
		if (oid)
      {
    	EnableWindow(GetDlgItem(hDlg, IDDELETE), TRUE);
    	EnableWindow(GetDlgItem(hDlg, IDUPDATE), TRUE);
      }

		// Builf Format List
		HWND hObjCtrl =  GetDlgItem(hDlg, IDFORMAT);
		int curIndex = (int)SendMessage(hObjCtrl, CB_GETCURSEL, 0, 0L);
		ATOM formatType = (int)SendMessage(hObjCtrl, CB_GETITEMDATA, curIndex, 0L);

		// Build the object list from the chosen type
		BuildObjTypeList(formatType);

		hObjCtrl =  GetDlgItem(hDlg, IDOBJTYPE);
		curIndex = (int)SendMessage(hObjCtrl, CB_GETCURSEL, 0, 0L);
		if (curIndex != CB_ERR)
      {
      char buf[MAXNAME];
			SendMessage(hObjCtrl, CB_GETLBTEXT, curIndex, (LONG)(LPSTR)buf);
			if (strcmp(buf, "N/A") == 0)   // set the 1st item
				{
				SendMessage(hObjCtrl, CB_SETCURSEL, 0, 0L);
        }
			}


    hObjCtrl = GetDlgItem(hDlg, IDOBJECT);
		curIndex =(int) SendMessage(hObjCtrl, CB_GETCURSEL, 0, 0L);

    if (curIndex != CB_ERR)
      {
			char buf[MAXNAME];
			SendMessage(hObjCtrl, CB_GETLBTEXT, curIndex, (LONG)(LPSTR)buf);
			if (strcmp(buf, "N/A") == 0)
				{
				// set the 1st item
				SendMessage(hObjCtrl, CB_SETCURSEL, 0, 0L);
				}
			SetAddFlag(DDES_ADDEDOBJECT);
			}

    hObjCtrl =  GetDlgItem(hDlg, IDOBJTYPE);
		curIndex = (int)SendMessage(hObjCtrl, CB_GETCURSEL, 0, 0L);
    if (curIndex != CB_ERR)
      {
			char buf[MAXNAME];
			SendMessage(hObjCtrl, CB_GETLBTEXT, curIndex, (LONG)(LPSTR)buf);
			if (strcmp(buf, "N/A") == 0)   // set the 1st item
				SendMessage(hObjCtrl, CB_SETCURSEL, 0, 0L);

      }

		return TRUE;
    }
  else if (oid == 0) // if a system item is selected, it will not have any itemdata
		{
		EnableWindow(GetDlgItem(hDlg, IDFORMAT), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDOBJTYPE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDOBJECT), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDADD), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDDELETE), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDUPDATE), FALSE);
    return FALSE;
    }
  return FALSE;
	}



/***********************************************************************
* 
*	Save the old Signals
*
************************************************************************/
void SaveOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **lpOldSigs)
	{
	objd->ddes->CheckSignals();
	*nOldSigs = objd->ddes->nUserSignals;
	*lpOldSigs = (SIGNALINFO *)new char[sizeof(SIGNALINFO) * (objd->ddes->nUserSignals)];
	_fmemcpy((LPSTR)*lpOldSigs, (LPSTR)objd->ddes->userSignals, *nOldSigs * sizeof(SIGNALINFO));
	}

/***********************************************************************
* 
*	Reset the old Signals
*
************************************************************************/
void ResetOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **lpOldSigs)
 	{
	objd->ddes->nUserSignals = *nOldSigs;
	delete objd->ddes->userSignals;
	objd->ddes->userSignals = *lpOldSigs;
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
			objd = (OBJDCLASS *) lParam;
			lpOldSig = NULL; // init to null
			objd->hDlg = hwnd;
			SaveOldSigs(objd, &nOldSigs, &lpOldSig);
			ATypeGetName(OTYPE_DDES, (LPSTR)title, 32);
      strcat(title, " Signals");
      AUtlSetTitleFont((TYPEID)OTYPE_DDES,  title,  GetDlgItem(hwnd, IDTITLE) );
			AUtlCenterDialog(hwnd, 0);

			// SKJ Nov. 2, 1994 Set Title
			AObjGetName((OBJECTID)objd->ddes->ddesOID, (LPSTR)title, 32);
			SetWindowText(hwnd, (LPSTR)title);


			objd->bCanAdd = 0;  // set all add flags to 0
      objd->InitSigCtrls();
			return FALSE;
      }

    case WM_COMMAND:
      {

      switch (wParam)
        {
        case IDOK:
					objd->ddes->SetSave(DDES_SAVESIGS);
          if (lpOldSig)
						delete lpOldSig; // remove the old copy of signals
					EndDialog(hwnd, 1);
          break;

				case IDCANCEL:      {

					objd->ddes->ResetSave(DDES_SAVESIGS);
					ResetOldSigs(objd, &nOldSigs, &lpOldSig); // reset from old copy of signals
					EndDialog(hwnd, 0);
					break;
					}

        case IDADD:
          {
          objd->AddSignalList(objd->defSignal);
          bHandled = TRUE;
          }
          break;

				case IDDELETE:  {
					if (objd->DeleteSignalList(objd->defSignal.name, objd->defSignal.rcvrOID) != DDES_SIGNALNOTFOUND)
						{
						objd->BuildSignalList();
						objd->SetCtrlsFromSignalList(); // take the new selecttion and set the ctrls
						}
						bHandled = TRUE;

          }
          break;

        case IDUPDATE:
          objd->UpdateSignalList();
          objd->BuildSignalList();
          bHandled = TRUE;
          break;

        case IDFORMAT:
          {
          // if the user has edited the name of the service, then add that as user
          // defined string
					if ((HIWORD(lParam) == CBN_KILLFOCUS) || (HIWORD(lParam) == CBN_CLOSEUP))
						{
				 	 	FORMATINFO fInfo;
            HWND hCtrl = GetDlgItem(hwnd,  wParam);
						int index = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);
            SendMessage(hCtrl, CB_GETLBTEXT, index, (LONG) (LPSTR)fInfo.name);
						fInfo.atom  = (int)SendMessage(hCtrl, CB_GETITEMDATA, index, 0);
						// if no matching string is found then add this string as user
            // defined
						if (objd->ChangeName((LPSTR)&fInfo, wParam )) // if name is diff from old
							{
							int index = (int)SendMessage(GetDlgItem(hwnd, IDOBJECT), CB_GETCURSEL,0,0L);
							if (index != CB_ERR)
            		{
            		// Get the ObjectID of the item selected
            		// or the TypeID
								LONG oID = SendMessage(GetDlgItem(hwnd, IDOBJECT), CB_GETITEMDATA, index,0);
								objd->defSignal.rcvrOID = (OBJECTID)oID;
								objd->SetAddFlag(DDES_ADDEDSIGOBJECT);
								}
							}
						}
					bHandled = TRUE;
          }
          break;

        case  IDOBJTYPE:
				case  IDOBJECT:
          {
          // if the selected item in the listbox has changed i.e. if we
          // have a new column selected, then set all the controls in
          // the dlg template to reflect those changes.
          if ((HIWORD(lParam) == CBN_CLOSEUP) || (HIWORD(lParam) == CBN_SELCHANGE))
            {
						HWND hCtrl = GetDlgItem(hwnd,  wParam);
						int index = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);
						if (index != CB_ERR)
            	{
            	// Get the ObjectID of the item selected
            	// or the TypeID
							LONG oID = SendMessage(hCtrl, CB_GETITEMDATA, index,0);
							// if name is diff from old
							if (objd->ChangeName((LPSTR)&oID, wParam))
								{
								if (wParam == IDOBJTYPE)
									{
									LONG oID = SendMessage(GetDlgItem(hwnd,  IDOBJECT), CB_GETITEMDATA, index,0);
									objd->defSignal.rcvrOID = (OBJECTID)oID;
									}
								else
									{
									objd->defSignal.rcvrOID = (OBJECTID)oID;
									}
								}
							objd->SetAddFlag(DDES_ADDEDSIGOBJECT);
							}
						}
          bHandled = TRUE;
          }
          break;

        case IDSIGNAL:
          {
          // if the server name has changed, then reset the system, topic, item lists
          if ((HIWORD(lParam) == EN_KILLFOCUS) && (bDlgHasFocus == TRUE))
            {
	 					char name[MAXNAME];
            GetWindowText(GetDlgItem(hwnd,  wParam), name, MAXNAME);
            objd->ChangeName(name, wParam);
						objd->SetAddFlag(DDES_ADDEDSIGNAL);
            bHandled = TRUE;
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




/****************************************************************************
 *                                                                          *
 *  FUNCTION   :     SysHelpDlgProc                                         *
 *                                                                          *
 *  PURPOSE    :     To allow users to create a help text file              *
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
extern "C" int FAR PASCAL _export SysHelpDlgProc (
  HWND          hwnd,
  WORD          msg,
  WORD          wParam,
  LONG          lParam)
  {
  static OBJECTID theObject;
  static OBJDCLASS *objd;
  static BOOL bDlgHasFocus = TRUE;
  BOOL   bHandled = FALSE;

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
			char title[MAXNAME];
      objd = (OBJDCLASS *) lParam;
			objd->hDlg = hwnd;
			sprintf(title, "DDE Server %s::Help", objd->Service());
			AUtlSetTitleFont((TYPEID)OTYPE_DDES,  title,  GetDlgItem(hwnd, IDTITLE) );

			// SKJ Nov. 2, 1994 Set Title
			AObjGetName((OBJECTID)objd->ddes->ddesOID, (LPSTR)title, 32);
			SetWindowText(hwnd, (LPSTR)title);


			AUtlCenterDialog(hwnd, 0);
			objd->InitSysHelp();
			return FALSE;
      }

    case WM_COMMAND:
      {

      switch (wParam)
        {
        case IDOK:
					objd->SaveSysHelp();
					EndDialog(hwnd, 1);
          break;

        case IDCANCEL:
					EndDialog(hwnd, 0);
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

/**************************************************************************
*
* 
*
***************************************************************************/
BOOL OBJDCLASS::EnableTopic(
  HWND hwnd,
  BOOL bEnable)
  {

	// bCanAdd is a combination of bits.  When all the bits are set we can add this string to
  // MainList

  if (bEnable)
		{
		SetAddFlag(DDES_ADDEDSERVICE | DDES_ADDEDFORMAT);
		}
  else
		{
		ResetAddFlag(DDES_ADDEDSERVICE | DDES_ADDEDFORMAT);
		}

	// EnableWindow(GetDlgItem(hwnd, IDSYSHELP), bEnable);
	I_UNUSED(hwnd);
  return bEnable;
  }

/**************************************************************************
*
* 
*
***************************************************************************/
BOOL OBJDCLASS::EnableItems(HWND hwnd, BOOL bEnable)
  {
  // EnableWindow(GetDlgItem(hwnd, IDITEM), bEnable);
  // bCanAdd is a combination of bits.  When all the bits are set we can add this string to
  // MainList
  if (bEnable)
		{
		SetAddFlag(DDES_ADDEDTOPIC);
		// bCanAdd |= DDES_ADDEDTOPIC;  // added correct service
    }
	else
		ResetAddFlag(DDES_ADDEDTOPIC);
		// bCanAdd &= ~DDES_ADDEDTOPIC;  // added correct service
    
	I_UNUSED(hwnd);
  return bEnable;
  }

/**************************************************************************
*
* 
*
***************************************************************************/
BOOL OBJDCLASS::EnableFormat(HWND hwnd, BOOL bEnable)
  {
  // EnableWindow(GetDlgItem(hDlg, IDFORMAT), bEnable);
  // bCanAdd is a combination of bits.  When all the bits are set we can add this string to
  // MainList
  if (bEnable)
		{
		SetAddFlag(DDES_ADDEDITEM);
		// bCanAdd |= DDES_ADDEDITEM;  // added correct service
    }
	else {
		ResetAddFlag(DDES_ADDEDITEM);
		// bCanAdd &= ~DDES_ADDEDITEM;
    }
  I_UNUSED(hwnd);
  return bEnable;
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
  
    case IDSERVICE:
      {
      return EnableTopic(hDlg, SetService(name));
      }

    case IDTOPIC: {
      return EnableItems(hDlg, SetTopic(name));
      }

    case IDITEM: {
      return EnableFormat(hDlg, SetItem(name));
      }
      
    case IDFORMAT:
      {
      LPLONG lVal = (LPLONG)name;
      FORMATINFO *fmtInfo = (FORMATINFO *)lVal;

      if (SetFormat(fmtInfo->name))
        {
  
        // EnableWindow(GetDlgItem(hDlg, IDOBJTYPE), TRUE);
        // EnableWindow(GetDlgItem(hDlg, IDOBJECT), TRUE);
        // get the type id of this format
        // int curindex = SendMessage(hCtrl, CB_GETCURSEL,0,0L);
        // Get the TYPE id of the current Type selected
        // int formatType = SendDlgItemMessage(hDlg,ctrlID, CB_GETITEMDATA, curindex,0);
        ddes->defFormat.atom = fmtInfo->atom; // set the format id
        BuildObjTypeList(fmtInfo->atom);
        return 1;
        }
      else
        return 0;
      }

    case IDOBJTYPE:
			{
			LPLONG lVal = (LPLONG)name;
			TYPEID typeID = (TYPEID)*lVal;
			if (typeID)
      	{
				rVal = 1;
				BuildObjectList(typeID);
     	 	SetObjType(typeID);
				}
			else
      	rVal = 0;
			}
      return rVal;;

    case IDOBJECT:
      {
      LPLONG lVal = (LPLONG)name;
			// int curindex = SendMessage(hCtrl, CB_GETCURSEL,0,0L);
      // Get the ObjectID of the item selected
      OBJECTID objectID;
      objectID = (OBJECTID)*lVal;
      SetObject(objectID);
      // bCanAdd is a combination of bits.  When all the bits are set we can add this string to
      // MainList
			if (objectID)  {
				SetAddFlag(DDES_ADDEDOBJECT);
				// bCanAdd |= DDES_ADDEDOBJECT;  // added correct service
        rVal = 1;
        }
			else {
				ResetAddFlag(DDES_ADDEDITEM);
				// bCanAdd &= ~DDES_ADDEDOBJECT;
        rVal = 0;
        }
      }
      break;
    case IDTIME:
      {
      LPLONG lVal = (LPLONG)name;
      SetUpdateTime((LONG)*lVal);
      return 1;
      }

    case IDSIGNAL:
      {
      SetSignal(name);
      if (strcmp(name, "") == 0) {
        ResetAddFlag(DDES_ADDEDSIGNAL);
        rVal = 1;
        }
      else {
        SetAddFlag(DDES_ADDEDSIGNAL);
        rVal = 0;
        }
      }
      break;

    default: return 0;
    }
  I_UNUSED(hCtrl);
  return rVal;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :      ProcessSubDialog                                      *
 *                                                                          *
 *  DESCRIPTION    :  Calls Dialog handlers for different attributes        *
 *										1. SIGNALS																						*
 *										2. SYSHELP                                            *
 *										3. CONTEXT                                            *
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/

int OBJDCLASS::ProcessSubDialog(int dlgType)
	{
	int rVal 	= dlgType;
	HWND hwnd = hDlg;;
	// before jumnping into signal, options or sysHelp dlg box, save the context
	WORD saveState = bCanAdd;
  OBJECTID tmpObject = ddes->defDataSrcOID;
  FORMATINFO tmpFormat = ddes->defFormat;
  TYPEID tmpObjType = ddes->defObjectType;

	switch (dlgType)
  	{
		case IDSIGNALS: {
			DoDialog(MAKEINTRESOURCE(DLGSIGNALS), (FARPROC)SignalsDlgProc, (OBJECTID) this);
      break;
		case IDSYSHELP:
			DoDialog(MAKEINTRESOURCE(DLGSYSHELP), (FARPROC)SysHelpDlgProc, (OBJECTID) this);
			break;
			}
		case IDCONTEXT:
    	{
			DoDialog(MAKEINTRESOURCE(DLGOPTIONS), (FARPROC)OptionsDlgProc, (OBJECTID) this);
      break;
			}
		default:
      rVal = FALSE;
		}

	// Restore the context
  hDlg = hwnd;
	bCanAdd = saveState;
  ddes->defObjectType = tmpObjType;
  ddes->defFormat = tmpFormat;
	ddes->defDataSrcOID = tmpObject;
	return rVal;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :      ServerDlgProc                                         *
 *                                                                          *
 *  DESCRIPTION    :  Dialog Callback for DDE server OBJD handling          *                                          
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
extern "C" int FAR PASCAL _export ServerDlgProc(
  HWND          hwnd,
  register WORD msg,
  register WORD wParam,
  LONG          lParam)
  {
  static OBJECTID theObject;
	OBJDCLASS *objd;
  BOOL bHandled = FALSE;
  static BOOL bDlgHasFocus = TRUE;

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
			SendDlgItemMessage(hwnd, IDSERVICE, CB_LIMITTEXT, MAX_NAME, 0);
      SendDlgItemMessage(hwnd, IDTOPIC, CB_LIMITTEXT, MAX_NAME, 0);
      SendDlgItemMessage(hwnd, IDFORMAT, CB_LIMITTEXT, MAX_NAME, 0);
      SendDlgItemMessage(hwnd, IDOBJECT, CB_LIMITTEXT, MAX_NAME, 0);
      SendDlgItemMessage(hwnd, IDOBJTYPE, CB_LIMITTEXT, MAX_NAME, 0);
			AUtlSetTitleFont((TYPEID)OTYPE_DDES,  0,  GetDlgItem(hwnd, IDTITLE));

			// SKJ Nov. 2, 1994 Set Title
			AObjGetName((OBJECTID)objd->ddes->ddesOID, (LPSTR)title, 32);
			SetWindowText(hwnd, (LPSTR)title);


			AUtlCenterDialog(hwnd, 0);
      bHandled = TRUE;
      }
      break;
    case WM_COMMAND:
      objd = (OBJDCLASS*)GetOBJDPtr(hwnd , theObject ,msg);
      if (!objd)
        {
        bHandled = FALSE;
        break;
        }
      switch (wParam)
        {
        
        case IDADD:
          objd->AddMainList();
          bHandled = TRUE;
          break;
        case IDDELETE:
          objd->DeleteMainList(); // take the strings from def controls
					objd->BuildMainList();
          objd->SetCtrlsFromMainLst(); // take the new selecttion and set the ctrls
          bHandled = TRUE;
          break;
        case IDUPDATE:
          objd->UpdateMainList();
          objd->BuildMainList();
          bHandled = TRUE;
          break;
				case IDOK: {
					objd->CreateSignalsFromMainList();
					objd->ddes->SetSave(DDES_SAVEOBJD); // set the bSave flag
					delete objd;                // delete the objd class instance

					EndDialog(hwnd, 1);
          bHandled = TRUE;
          return TRUE;
					}

        case IDCANCEL:
          {
          delete objd;                // delete the objd class instance

          EndDialog(hwnd, 0);
          bHandled = TRUE;
          return TRUE;
					}
        case IDHELP:
          {
          MessageBox(hwnd, "Help Not Implemented", "DDEC HELP", MB_OK);
          bHandled = TRUE;
          }
          break;
        case IDSERVICE:
        case IDTOPIC:
        case IDITEM:
          {
          // if the server name has changed, then reset the system, topic, item lists
          if ((HIWORD(lParam) == EN_KILLFOCUS) && (bDlgHasFocus == TRUE))
            {
         
            char name[MAXNAME];
            GetWindowText(GetDlgItem(hwnd,  wParam), name, MAXNAME);
						objd->ChangeName(name, wParam);
            // objd->SetServerName(hwnd, name); // if name is diff from old
            bHandled = TRUE;
						if (wParam == IDTOPIC)
							{
              // if the topic name has changed, then set the ctrls to be active again
							// simulate a new selection
							if (strcmp(name, SZDDESYS_TOPIC) != 0)
								objd->CheckForSystem(LB_ERR); // #define CB_ERRSPACE         (-2)
							else // if a system string was typed in
								objd->CheckForSystem(LB_ERRSPACE); // #define CB_ERRSPACE         (-2)
                
							}

						}
					}
          break;

        case IDFORMAT:
          {
 					// if the user has edited the name of the service, then add that as user
          // defined string
          if (HIWORD(lParam) == CBN_KILLFOCUS)
            {
            FORMATINFO fInfo;
            GetDlgItemText(hwnd, wParam,fInfo.name, MAXNAME);
						int index = (int)SendMessage(GetDlgItem(hwnd, IDFORMAT), CB_GETCURSEL,0,0L);
						fInfo.atom  = (int)SendMessage(GetDlgItem(hwnd, IDFORMAT), CB_GETITEMDATA, index, 0);
            // if no matching string is found then add this string as user
            // defined
            objd->ChangeName((LPSTR)&fInfo, wParam ); // if name is diff from old
            } // if the server name has changed, then reset the system, topic, item lists
          else if (HIWORD(lParam) == CBN_CLOSEUP)
            {
            FORMATINFO fInfo;
            HWND hCtrl = GetDlgItem(hwnd,  wParam);
						int index = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);
            SendMessage(hCtrl, CB_GETLBTEXT, index, (LONG) (LPSTR)fInfo.name);
						fInfo.atom  = (int)SendMessage(GetDlgItem(hwnd, IDFORMAT), CB_GETITEMDATA, index, 0);
            objd->ChangeName((LPSTR)&fInfo, wParam); // if name is diff from old
            }
          bHandled = TRUE;
          }
          break;

        case IDMAINLIST: 
          if (HIWORD(lParam) == LBN_SELCHANGE)
            {
            objd->SetCtrlsFromMainLst();
            // check to see if any system topic:items have been selected
            int curSel = (int)SendDlgItemMessage(hwnd, IDMAINLIST, LB_GETCURSEL, 0L, 0);
						objd->CheckForSystem(curSel);
            bHandled = TRUE;
            }
            break;

        case IDOBJTYPE:
        case IDOBJECT:
          {
					// if the selected item in the listbox has changed i.e. if we
          // have a new column selected, then set all the controls in
          // the dlg template to reflect those changes.
          if ((HIWORD(lParam) == CBN_CLOSEUP) || (HIWORD(lParam) == CBN_SELCHANGE))
            {
						HWND hCtrl = GetDlgItem(hwnd,  wParam);
						int index = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);

						if (index != CB_ERR)
            	{
            	// Get the ObjectID of the item selected
            	// or the TypeID
            	LONG oID = SendMessage(hCtrl, CB_GETITEMDATA, index,0);
							objd->ChangeName((LPSTR)&oID, wParam); // if name is diff from old
							}
            }
					bHandled = TRUE;
          }
          break;

				case IDSIGNALS:
				case IDSYSHELP:
				case IDCONTEXT:
					{
					objd->ProcessSubDialog(wParam);
					bHandled = TRUE;
					}
					break;

				default:
					bHandled = FALSE;

				}
      break;  // end of case WM_COMMAND
    }  // end of main Switch
  return bHandled;
  }


/**************************************************************************
*
*  This function is called to edit the CLIENT OBJECT
*
***************************************************************************/

long FAR PASCAL EditServerObject(OBJECTID theObject)
	{
  long rVal = A_OK;
  // DoDialog(MAKEINTRESOURCE(IDDDECLIENT), (FARPROC)ClientDlgProc, theObject);
	long i = DoDialog(MAKEINTRESOURCE(DDESERVER), (FARPROC)ServerDlgProc, theObject);
	if (i == -1)
		rVal= A_ERROR;
	else if (i == 1)
		rVal = A_MODIFIED;
	else rVal = A_OK;
	return rVal;

  }




