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
// FILE:  			ddecobjd.cpp
//
// AUTHOR:      Shaili Jain
//
// DESCRIPTION: OBJD Class implementation to handle AOBJ_EDIT 
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

// to force strict compile
#define STRICT
#define ASTRICT


/***************************************************************************
 *                                                                        *
 *  MODULE      :   ddecobjd.cpp                    	                      *
 *                                                                        *
 *  DESCRIPTION :   Contains all dialog procedures and related functions  *
 *								  used during OBJ Description
 *                                                                        *
 ***************************************************************************/
#include "ddemlcl.h"
#include "a_almutl.h"
#include <stdlib.h>                                               
#include <string.h>
#include <stdio.h>
#include "commdlg.h"
#include "objd.h"
#include "runtime.h"



extern ATOM GetFormatID(char *formatStr);
/***************************************************************************
// Get the language string from Language code
***************************************************************************/
void GetLanguageString(LPSTR lpStr)
	{
	int i;
	int size =  sizeof(langList) / sizeof(LANGUAGELIST);

	for (i = 0; i < size ; i++)
		{
		;
		}
	I_UNUSED(lpStr);
	}


/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description:  																															*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
BOOL EnablePaste(HWND hwnd)
	{
	BOOL bPasteLink = FALSE;
  HANDLE hClipData;
	// check to see if there is clipboard data
	if (OpenClipboard(hwnd))
	  {
		if ((hClipData = GetClipboardData(fmtLink)) != NULL) // if there is data
		  {
			bPasteLink = TRUE;	
			}
		CloseClipboard();
    }
	if (bPasteLink)
	  EnableWindow(GetDlgItem(hwnd, IDPASTELINK), TRUE);
	else
		EnableWindow(GetDlgItem(hwnd, IDPASTELINK), FALSE);
	I_UNUSED(hClipData);
	return bPasteLink;
  }

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description:  																															*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
int IsInFormatList(LPSTR fName)
	{
	int i;
  for (i =0; i < MAXFORMATS; i++)
		{
		if (lstrcmpi(SFormats[i].name, fName) == 0)
			return SFormats[i].typeID;
		}
	return SDDE_NOMATCHINGFORMAT;

	}


/****************************************************************************
* Function: 		SetComboItem
*
* Description:  This functions will add a string to a combo box and set it as
*								current selection
*								If the string being added is empty it will select the first item
*
* Return:       index of the string added
*
*****************************************************************************/

int SetComboItem(HWND hWnd, int CtrlID, LPSTR itemStr)
	{
	int index = 0;
	HWND hCtrl = GetDlgItem(hWnd, CtrlID);

  if ((itemStr[0] != '\0') &&
	 ((index = (int)SendMessage(hCtrl, CB_FINDSTRINGEXACT,-1, (LONG)(LPSTR)itemStr)) == CB_ERR) )
    {
		index = (int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)itemStr);
		if (index == CB_ERR)
			index = 0;
    }

  
	(int)SendMessage(hCtrl, CB_SETCURSEL, index, 0L);

	return index;
	}

/**********************************************************************
* Function:  		ChangeName
* Description:  This function will set the name of service, topic, or item
*               based on the ctrlID parameter. If the service name changes
*								it will change the list of topics and items
*
*
***********************************************************************/
int OBJDCLASS::ChangeName(HWND hWnd, LPSTR name, int ctrlID)
	{
	HWND hCtrl = GetDlgItem(hWnd, ctrlID);

	switch (ctrlID)
		{
		case IDSERVICE:       {
			if (SetService(name))
				{
				// Build a new Topic and Item list
				GetSystemInfo(hWnd);
				return 1;
        }
			return 0;
      }
		case IDTOPIC:
			return SetTopic(name);

		case IDITEM:
			return SetItem(name);

		case IDFORMAT:
			{
			SetFormat(name);
			// get the type id of this format
			int curindex = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);
			// Get the TYPE id of the current Type selected
			int formatType = (int)SendDlgItemMessage(hWnd, ctrlID, CB_GETITEMDATA, curindex,0);
			BuildObjTypeList(hWnd, formatType);
			}
			return 0;

		case IDOBJTYPE:
			{
			int curindex = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);
			// Get the ObjectID of the item selected
			TYPEID typeID = (TYPEID)SendDlgItemMessage(hWnd, ctrlID, CB_GETITEMDATA, curindex,0);
			BuildObjectList(hWnd, typeID);
			SetObjType(typeID);
			}
			return 0;
		case IDOBJECT:
			{
			int curindex = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);
			// Get the ObjectID of the item selected
			OBJECTID objectID = (OBJECTID)SendDlgItemMessage(hWnd, ctrlID, CB_GETITEMDATA, curindex,0);

			SetObject(objectID);

			}
			return 0;
    default: return 0;
		}


	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description:  																															*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
BOOL UninitializeDDE()
	{
	HWND hwnd = GetActiveWindow();
	DWORD idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);

	int rVal  = DdeUninitialize(idInst);

  
  #ifdef DEBUG
	if (rVal == 0)
		MessageBox(hwnd,(LPSTR)"DDEML UNINITIALIZE ERR", "DDE MSG", MB_OK);
	else {
		MessageBox(hwnd,(LPSTR)"DDEML UNINITIALIZE SUCCESSFUL", "DDE MSG", MB_OK);

    }
	#endif
	I_UNUSED(hwnd);
	return rVal;
	}


/******************************************************************************
* Function:  InitialiseDDE                                                    *
*                                                                             *
* Description:  																															*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
BOOL InitializeDDE()
	{
	
	DWORD idInst = 0L;  // init to 0
	int rVal = DdeInitialize(&idInst, (PFNCALLBACK)(FARPROC)DdeCallback, APPCLASS_STANDARD, 0L);
	if (rVal == DMLERR_NO_ERROR)
		{
		ATypeSetRuntimeInfo(OTYPE_DDEC, (LONG)idInst);
		}
	else
		{
		ATypeSetRuntimeInfo(OTYPE_DDEC, 0);
		}



	#ifdef DEBUG
	HWND hwnd = GetActiveWindow();
	switch (rVal)
		{
		case DMLERR_DLL_USAGE:
			MessageBox(hwnd,(LPSTR)"DMLERR_DLL_USAGE", "DDE_INIT_ERROR", MB_OK);
			return 0;
		case DMLERR_INVALIDPARAMETER:
		 	MessageBox(hwnd , "DMLERR_INVALIDPARAM", "DDE INIT ERROR", MB_OK);
      return 0;
		case DMLERR_NO_ERROR:
			MessageBox(hwnd , "DMLERR_NO_ERROR", "DDE INIT ERROR", MB_OK);
			break;
		case DMLERR_SYS_ERROR:
			MessageBox(hwnd , "DMLERR_SYS_ERROR", "DDE INIT ERROR", MB_OK);
			return 0;
		}
	#endif



  CCFilter.iCodePage = CP_WINANSI;
	RegisterClipboardFormat("Link");
	I_UNUSED(rVal);
	return TRUE;


  }

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description:  																															*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/

int OBJDCLASS::DoTransaction(LPSTR itemStr, MEMBLOCKID txtID)
	{
	// SKJ Build a List with the returned data
	dde->pxact->hszItem = DdeCreateStringHandle(dde->idInst,itemStr, NULL);
	dde->ProcessTransaction();
	if (dde->pxact->ret)
    {
		GetTextData((HDDEDATA)dde->pxact->ret, txtID);
	
		}
	else
		AMemSetSize(txtID, 0);

	DdeFreeStringHandle(dde->idInst, dde->pxact->hszItem);

	return (int)dde->pxact->ret;;
	}

/***************************************************************************
*
	Function: 		GetSystemInfo
*
* Description:  This function will start a dde conversation with a given
*               service and 'SYSTEM' topic.  It will then carry out
*	              transaction (REQUEST) with the following items:
*               1. ITEM_TOPICS:   build list of topics   
*               2. ITEM_ITEMLIST: build list of items for the chosen service
								3. ITEM_FORMAT:   build list of supported formats
*
****************************************************************************/
int OBJDCLASS::GetSystemInfo(HWND hwnd)
	{
	WORD error;
	MEMBLOCKID txtID;
	BOOL bOldDefCC;
	char topicName[MAXNAME];

 // 	_fmemset(&CCFilter, 0, sizeof(CONVCONTEXT));
 // 	CCFilter.cb = 16;
 // 	CCFilter.wCountryID = 1004;
	lstrcpy(topicName,SZDDESYS_TOPIC);
	HSZ 	hszApp = DdeCreateStringHandle((DWORD)dde->idInst,dde->convInfo.Service , 0);
	HSZ 	hszTopic = DdeCreateStringHandle((DWORD)dde->idInst,topicName , 0);

	// Start a conversation with Server under System Topic
	// for system probe use default conv context
	bOldDefCC = dde->bDefContext;
	dde->bDefContext = TRUE;

	if (!(dde->convInfo.hConv = dde->CreateConv(hszApp, hszTopic,FALSE, &error)))
		{
		dde->MPError(hwnd, MB_OK, IDS_DDEMLERR, (LPSTR)Error2String(error));
		DdeFreeStringHandle(dde->idInst, hszApp);
		DdeFreeStringHandle(dde->idInst, hszTopic);

		// SKJ Build a List with the returned data
	  SetDDETopicList(hwnd, 0);
   	SetDDEFormatList(hwnd, 0);
		SetDDEItemList(hwnd, 0);
	  dde->bDefContext = bOldDefCC;
    return  0;
		}

	txtID = AMemAllocate(0);
	dde->pxact->fsOptions = DefOptions;
	dde->pxact->ulTimeout = DEFTIMEOUT;
	dde->pxact->wFmt = CF_TEXT;
	dde->pxact->wType = XTYP_REQUEST;
	dde->pxact->hDdeData = 0;
	// now start the transaction
	// Get a list of all formats
	
	DoTransaction( SZDDESYS_ITEM_TOPICS, txtID);
	SetDDETopicList(hwnd, txtID);
	
	DoTransaction(SZDDESYS_ITEM_FORMATS, txtID);
	SetDDEFormatList(hwnd,  txtID);	
	
	DoTransaction( SZDDE_ITEM_ITEMLIST, txtID);
	SetDDEItemList(hwnd, txtID);

	dde->Disconnect();
	AMemFree(txtID);
	DdeFreeStringHandle(dde->idInst, hszApp);
	DdeFreeStringHandle(dde->idInst, hszTopic);

	dde->bDefContext = bOldDefCC;
	return 1;
	
	}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : Error2String()                                             *
 *                                                                          *
 *  PURPOSE    : Converts an error code to a string and returns a pointer   *
 *               to that string.  The string is valid until the next call   *
 *               to this function.                                          *
 *                                                                          *
 ****************************************************************************/
LPSTR Error2String(
	WORD error)
	{
		static char szErr[40];
    static char *e2s[] = {
        "Advacktimeout"              ,
        "Busy"                       ,
        "Dataacktimeout"             ,
        "Dll_not_initialized"        ,
        "Dll_usage"                  ,
        "Execacktimeout"             ,
        "Invalidparameter"           ,
        "Low Memory warning"         ,
        "Memory_error"               ,
        "Notprocessed"               ,
        "No_conv_established"        ,
        "Pokeacktimeout"             ,
        "Postmsg_failed"             ,
        "Reentrancy"                 ,
        "Server_died"                ,
        "Sys_error"                  ,
        "Unadvacktimeout"            ,
        "Unfound_queue_id"           ,
    };
    if (!error) {
      lstrcpy(szErr, "0");
			}
		else if (error > DMLERR_LAST || error < DMLERR_FIRST) { 
      lstrcpy(szErr, "???");
			}
		else {
      lstrcpy(szErr, e2s[error - DMLERR_FIRST]);
			}
    return(szErr);
}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : MPError ( hwnd, flags, id, ...)                            *
 *                                                                          *
 *  PURPOSE    : Flashes a Message Box to the user. The format string is    *
 *               taken from the STRINGTABLE.                                *
 *                                                                          *
 *  RETURNS    : Returns value returned by MessageBox() to the caller.      *
 *                                                                          *
 ****************************************************************************/
short FAR CDECL DDEC::MPError(HWND hwnd,WORD bFlags,WORD id,...)
{
    char sz[160];
		char szFmt[128];
		if (bDisplayErr) // even if DEBUG flag is set, we may not want to disp
										 // certain errs
		  {  
      LoadString (hInst, id, szFmt, sizeof (szFmt));
      wvsprintf (sz, szFmt, (LPSTR)(&id + 1));
      LoadString (hInst, IDS_APPNAME, szFmt, sizeof (szFmt));
			return MessageBox (hwnd, sz, szFmt, bFlags);
			}
    else
     return 0; 
	

}



/***************************************************************************
*
	Function: 		DDECTest
*
* Description:  This function will start a dde conversation with a given
*               service and topic.  It will then carry out
*	              transaction (REQUEST) with the chosen item:
* Output Value: It will display a message box with the result of the
*               dde operations
****************************************************************************/
int OBJDCLASS::DDECTest(HWND hwnd)
	{
	WORD error;
	int rVal = 0;
	MEMBLOCKID textID = AMemAllocate(0);
	HSZ 	hszApp = DdeCreateStringHandle((DWORD)dde->idInst,dde->convInfo.Service , 0);
	HSZ 	hszTopic = DdeCreateStringHandle(dde->idInst,dde->convInfo.Topic , 0);

	// Start a conversation with Server under System Topic
	if (!(dde->convInfo.hConv = dde->CreateConv(hszApp, hszTopic,FALSE, &error)))
		{
		// failed conversation
		// transaction cannot be attempted
		dde->MPError(hwnd, MB_OK, IDS_DDEMLERR, (LPSTR)Error2String(error));
		DdeFreeStringHandle(dde->idInst, hszApp);
		DdeFreeStringHandle(dde->idInst, hszTopic);
    return  0;
    }

  dde->pxact->fsOptions = DefOptions;
	dde->pxact->ulTimeout = DEFTIMEOUT;
	dde->pxact->wFmt = GetFormatID(dde->defFormat);
	dde->pxact->wType = XTYP_REQUEST;
	dde->pxact->hDdeData = 0;
	// now start the transaction
	// Get a list of all formats

	// dde->pxact->hszItem = DdeCreateStringHandle(dde->idInst, dde->defItem, NULL);
	DoTransaction(dde->defItem, textID);
	if (dde->pxact->ret)
		{
		// Successful conversation and transaction processing
		dde->MPError(hwnd, MB_OK, IDS_TRANSNERR, dde->convInfo.Service, dde->convInfo.Topic, dde->defItem);
    // stop this transaction
		dde->pxact->wType = XTYP_ADVSTOP;
		DoTransaction(dde->defItem, textID);
		}
	else {
		// successful conversation but failed transaction
		// Successful conversation and transaction processing
		dde->MPError(hwnd, MB_OK, IDS_TRANSERR, dde->convInfo.Service, dde->convInfo.Topic, dde->defItem, (LPSTR)(LPSTR)Error2String(DdeGetLastError(dde->idInst)));
		}

	dde->Disconnect();
	DdeFreeStringHandle(dde->idInst, hszApp);
	DdeFreeStringHandle(dde->idInst, hszTopic);
	AMemFree(textID);
	I_UNUSED(rVal);
	return 1;
	
	}

/**************************************************************************
* Funtion:  SetUndefItemInCombo
* This function will add a string to a combo box and make it the current
* selection.  It will always place the new item in place of the 'undefined'
* item (position 0).
*
*
*
**************************************************************************/
int SetUndefItemInCombo(HWND hCtrl, LPSTR str)
	{
	int index;
    
	if ((index = (int)SendMessage(hCtrl, CB_FINDSTRINGEXACT,-1, (LONG)(LPSTR)str)) == CB_ERR)
		{
		(int)SendMessage(hCtrl, CB_DELETESTRING, INDEX_USERDEFSTR,0L); // remove the userdef string
		index = (int)SendMessage(hCtrl, CB_INSERTSTRING, INDEX_USERDEFSTR, (LONG)(LPSTR)str);
		}

	(int)SendMessage(hCtrl, CB_SETCURSEL, index, 0L);
	return index;
	}


/**************************************************************************
*	function : SaveDatas
*						 Save all the objd dlg control to datas
*	
*
**************************************************************************/
void OBJDCLASS::SaveDatas(HWND hWnd)
	{

	LPDDECDATA    lpData = (LPDDECDATA)AObjLockData(dde->ddeCID, data0_DDECDATA);
	LPDDECOPTIONS lpOptions= (LPDDECOPTIONS)AObjLockData(dde->ddeCID, data1_DDECOPTIONS);

	_fstrncpy(lpData->ServerName, dde->convInfo.ServerName, MAX_NAME);
	_fstrncpy(lpData->Service, dde->convInfo.Service, MAX_NAME);
	_fstrncpy(lpData->Topic, dde->convInfo.Topic, MAX_NAME);
	_fstrncpy(lpData->Item, dde->defItem, MAX_NAME);
	_fstrncpy(lpData->Format, dde->defFormat, MAX_NAME);
	lpData->linkType = dde->defLinkType;
  lpData->objectID = dde->defReceiverOID;
	lpData->ObjectType = dde->defObjectType;
	lpData->linkType = dde->defLinkType;

	// Name of the Object and its type
	ATypeGetName(dde->defObjectType, lpData->typeName, MAXNAME);
	AObjGetName(dde->defReceiverOID, lpData->objectName, MAXNAME);
	_fmemcpy(&(lpOptions->CCFilter), &(dde->CCFilter), sizeof(CONVCONTEXT));
	lpOptions->timeout = dde->timeout;
	lpOptions->advDelay = dde->defAdvDelay;
	_fstrncpy(lpOptions->Country, dde->Country, MAX_NAME);
	_fstrncpy(lpOptions->Language, dde->Language, MAX_NAME);
	strcpy(lpOptions->CodePage, dde->CodePage);

	lpOptions->bDefContext = dde->bDefContext;

	AObjUnlockData(dde->ddeCID, data0_DDECDATA);
	AObjUnlockData(dde->ddeCID, data1_DDECOPTIONS);
	I_UNUSED(hWnd);
	}


//  Methods for the OBJDCLASS class
/****************************************************************************
 *                                                                          *
 *  METHOD     : OBJDCLASS()                                                 *
 *                                                                          *
 *  DESCRIPTION    : Constructor
 *               
 *  RETURNS    : 														                                *
 *                                                                          *
 ****************************************************************************/
OBJDCLASS::OBJDCLASS(HWND hWnd, OBJECTID objID)
	{
	// Init DDE Client Call Back;
	serviceCount = 1;
	topicCount = 1;
	formatCount = 0;
	objTypeCount = 0;;
	objectCount = 0;


	InitializeInstance(FALSE);
	DDEC::serviceListID = AMemAllocate(sizeof(SERVICELIST));

	dde = (DDEC *)new DDEC(objID, TRUE);
 	dde->bDisplayErr = FALSE;
	DDEC::hConvWnd = hWnd;  // set the parent window of this ddec class instance
											// to the dlg window

	// The first item in the service list will be a userdefined string
	SetWindowText(GetDlgItem(hWnd, IDSERVER), dde->convInfo.ServerName);
	SetWindowText(GetDlgItem(hWnd, IDSERVICE),dde->convInfo.Service);
	SetWindowText(GetDlgItem(hWnd, IDTOPIC), dde->convInfo.Topic);
	SetWindowText(GetDlgItem(hWnd, IDITEM),  dde->defItem);

  serviceCount = 1;
	AObjGetDataCount(objID);
	AObjSetDataCount(objID, MAXDDECLIENTDATAS);
	AObjSetRuntimeInfo(objID,(LONG)this);

	}

/****************************************************************************
 *                                                                          *
 *  MEWTHOD    : ~OBJDCLASS()                                                 *
 *                                                                          *
 *  DESCRIPTION    : Destructor
 *               
 *  RETURNS    : 														                                *
 *                                                                          *
 ****************************************************************************/
OBJDCLASS::~OBJDCLASS()
	{
	OBJECTID theObject = dde->ddeCID;
	delete dde;
	dde = NULL;
	AMemFree(DDEC::serviceListID);
	DDEC::serviceListID = 0;
	DDEC::hConvWnd = 0;

	// set the data's contents to NULL
	AObjSetRuntimeInfo(theObject, 0);
	}

/****************************************************************************
 *                                                                          *
 *  METHOD    : SetDDEServiceList()                                                 *
 *                                                                          *
 *  DESCRIPTION    : Set the service list combo box
 *             : bSet -> add to list
 *                       else remove from list
 *               
 *  RETURNS    : 														                                *
 *                                                                          *
 ****************************************************************************/
int OBJDCLASS::SetDDEServiceList(HWND hWnd, BOOL bSet)
	{
	int i;
	HWND hCtrl = GetDlgItem(hWnd, IDSERVICE);
	LPSERVICELIST sList = (LPSERVICELIST)AMemLock(DDEC::serviceListID);
	serviceCount = (int)AMemGetSize(DDEC::serviceListID) / sizeof(SERVICELIST);
  if (bSet) {
	  for (i = 0; i < serviceCount; i++)
		  {
			if ((int)SendMessage(hCtrl, CB_FINDSTRINGEXACT,-1, (LONG)(LPSTR)sList[i].serviceName) == CB_ERR)
				(int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)sList[i].serviceName);
			}
		}
	else {
		(int)SendMessage(hCtrl, CB_RESETCONTENT, 0, 0);
		for (i = 0; i < serviceCount; i++)
		  {
			(int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)sList[i].serviceName);
			}
		}
	if ((int)SendMessage(hCtrl, CB_GETCURSEL, 0, 0L) == CB_ERR)
		(int)SendMessage(hCtrl, CB_SETCURSEL, 0,0);

	AMemUnlock(DDEC::serviceListID);
	return serviceCount;
	}


//  Methods for the OBJDCLASS class
/****************************************************************************
 *                                                                          *
 *  METHOD     : InitControlsFromDatas                                               *
 *                                                                          *
 *  PURPOSE    : Init all the Dialog box controls from the object datas     *         
 *  RETURNS    : 														                                *
 *                                                                          *
 ****************************************************************************/
void OBJDCLASS::InitControlsFromDatas(HWND hWnd)
	{
	int i, idVal;
	LPDDECDATA    lpData = (LPDDECDATA)AObjLockData(dde->ddeCID, data0_DDECDATA);
	LPDDECOPTIONS lpOptions= (LPDDECOPTIONS)AObjLockData(dde->ddeCID, data1_DDECOPTIONS);

	// Build a service list by establishing dde conv
	// and add 1 user define string to the total service count
	serviceCount += dde->BuildServiceList(hWnd);
	SetDDEServiceList(hWnd, TRUE);

	// This should also set the dde fields
  // Server Name
	SetServerName(hWnd, lpData->ServerName);
	SetWindowText(GetDlgItem(hWnd, IDSERVER), lpData->ServerName);

	// Service List --> To build a service list
	ChangeName(hWnd, lpData->Service, IDSERVICE);
	SetUndefItemInCombo(GetDlgItem(hWnd, IDSERVICE), lpData->Service);

	// Topic List
	SetUndefItemInCombo(GetDlgItem(hWnd, IDTOPIC), lpData->Topic);

  // Format list
	i = SetUndefItemInCombo(GetDlgItem(hWnd, IDFORMAT), lpData->Format);
	idVal = IsInFormatList((LPSTR)lpData->Format);
	(int)SendMessage(GetDlgItem(hWnd, IDFORMAT), CB_SETITEMDATA,i, (DWORD)idVal);

  // Item name
	SetUndefItemInCombo(GetDlgItem(hWnd, IDITEM), lpData->Item);

	// Object Type List
	ATypeGetName(lpData->ObjectType, dde->defTypeName, MAXNAME);
	BuildObjTypeList(hWnd, idVal);

	SetComboItem(hWnd, IDOBJTYPE,  dde->defTypeName);
	// If there is objectType Stored in the Datas
	if (AObjGetObjectCount(lpData->ObjectType))  // see if any obj. are available for this TYPE
		BuildObjectList(hWnd, lpData->ObjectType); // dde->defObjectType
  AObjGetName(lpData->objectID, dde->defObjectName, MAXNAME);
	SetComboItem(hWnd, IDOBJECT,  dde->defObjectName);


	lstrcpy(dde->convInfo.ServerName, lpData->ServerName);
	lstrcpy(dde->convInfo.Service, lpData->Service);
	lstrcpy(dde->convInfo.Topic, lpData->Topic);
	lstrcpy(dde->defItem, lpData->Item);
	lstrcpy(dde->defFormat, lpData->Format);
	dde->defLinkType = lpData->linkType;

	// if there is a valid oid in the datas
	if (lpData->objectID) {
		dde->defReceiverOID = lpData->objectID;
    }

	dde->defObjectType = lpData->ObjectType;
	dde->timeout = lpOptions->timeout;
	dde->defAdvDelay = lpOptions->advDelay;
	_fmemcpy(&(dde->CCFilter), &(lpOptions->CCFilter), sizeof(CONVCONTEXT));
	lstrcpy(dde->Country, lpOptions->Country);
  lstrcpy(dde->Language, lpOptions->Language);
  lstrcpy(dde->CodePage, lpOptions->CodePage);

	(int)SendMessage(GetDlgItem(hWnd, IDAUTOLINK), BM_SETCHECK, dde->defLinkType==AUTO_LINK?1:0, 0);
	(int)SendMessage(GetDlgItem(hWnd, IDNOTIFYLINK), BM_SETCHECK, dde->defLinkType==NOTIFY_LINK?1:0, 0);
	(int)SendMessage(GetDlgItem(hWnd, IDMANUALLINK), BM_SETCHECK, dde->defLinkType==MANUAL_LINK?1:0, 0);
	AObjUnlockData(dde->ddeCID, data0_DDECDATA);
	AObjUnlockData(dde->ddeCID, data1_DDECOPTIONS);
	}

/****************************************************************************
 *                                                                          *
 *  METHOD    : SetDDETopicList()                                           *
 *                                                                          *
 *  DESCRIPTION    : Build A DDE Topic List
 *               
 *  RETURNS    : 														                                *
 *                                                                          *
 ****************************************************************************/
int OBJDCLASS::SetDDETopicList(HWND hWnd, MEMBLOCKID dataID)
	{
	LPSTR lpTab;
	LPSTR lpData;
	if (dataID)
		lpData = (LPSTR)AMemLock(dataID);
	else
		lpData = NULL;
	HWND hCtrl = GetDlgItem(hWnd, IDTOPIC);
	
	int i =0;
	topicCount = 1;

	lpTab = lpData;

	// delete all the old Topics
	(int)SendMessage(hCtrl, CB_RESETCONTENT, 0, 0L);

	// if there is a valid string, we atleast have one topic name
	if (lpTab)
		{
		
		i = 1;
		}

	// format of string should be <name1>\t<name2>..
	while (lpTab && ((lpTab = strchr(lpTab, '\t')) != NULL))
		{
		i++;
		lpTab++;

		}

	// topic count is total number of topics plus a user defined topic
	topicCount += i;

	topicList = (LPNAME *)new LPNAME[topicCount];

	i = 0;
	lstrcpy(topicList[i], USER_DEFNAME);
	i++;

	while (lpData && ((lpTab = strchr(lpData, '\t')) != NULL))
		{
		int len = lstrlen(lpData) - lstrlen(lpTab);
		if (len > MAX_NAME)
			len = MAX_NAME;
		_fstrncpy(topicList[i], lpData, len);
		topicList[i][len] = '\0';

		lpData = ++lpTab;

		i++;

		// when it reaches the end of the string
		if (lpTab != strchr(lpData, '\t')) {
			_fstrncpy(topicList[i], lpData, MAX_NAME);
			topicList[i][MAX_NAME - 1] = '\0';
      }
		}
	for (i = 0; i < topicCount; i++)
		{
		(int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)topicList[i]);
		}
	
	if ((int)SendMessage(hCtrl, CB_GETCURSEL, 0, 0L) == CB_ERR)
		{
		char name[MAXNAME];
		(int)SendMessage(hCtrl, CB_SETCURSEL, 0,0);
		// SKJ now set the defitem field of the DDEC objd class MAY 21, 1994
		(int)SendMessage(hCtrl, CB_GETLBTEXT,0, (LONG)(LPSTR)name);
		ChangeName(hWnd, name, IDTOPIC);
		}
	AMemUnlock(dataID);
	return topicCount;
	}




/****************************************************************************
 *                                                                          *
 *  METHOD    : SetDDEItemList()                                                 *
 *                                                                          *
 *  DESCRIPTION    : Build A DDE Item List
 *               
 *  RETURNS    : 														                                *
 *                                                                          *
 ****************************************************************************/
int OBJDCLASS::SetDDEItemList(HWND hWnd, MEMBLOCKID dataID)
	{
	int i =0;
	LPSTR lpData;

	if (dataID)
		lpData = (LPSTR)AMemLock(dataID);
	else
		lpData = NULL;

	LPSTR lpTab;
	HWND hCtrl = GetDlgItem(hWnd, IDITEM);

	lpTab = lpData;
	itemCount = 1; // user defined string
	// delete all the old Items
	(int)SendMessage(hCtrl, CB_RESETCONTENT, 0, 0L);

	// if there is a valid string, we atleast have one item name
	if (lpTab)
		{
		i = 1;
		}

	// format of string should be <name1>\t<name2>..
	while (lpTab && ((lpTab = strchr(lpTab, '\t')) != NULL))
		{
		i++;
		lpTab++;
		}

	// item count is total number of items plus a user defined item
	itemCount += i;
	itemList = new LPNAME[itemCount];
	i = 0;
	lstrcpy(itemList[i], USER_DEFNAME);
	i++;

	while (lpData && ((lpTab = strchr(lpData, '\t')) != NULL))
		{
		int len = lstrlen(lpData) - lstrlen(lpTab);
		if (len > MAX_NAME)
			len = MAX_NAME;
		strncpy(itemList[i], lpData, len);
		itemList[i][len] = '\0';
		lpData = ++lpTab;
		i++;
		// when it reaches the end of the string
		if (lpTab != strchr(lpData, '\t')) {
			_fstrncpy(itemList[i], lpData, MAX_NAME);
			itemList[i][MAX_NAME - 1] = '\0';
			}
		}

	for (i = 0; i < itemCount; i++)
		{
		(int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)itemList[i]);
		}

	if ((int)SendMessage(hCtrl, CB_GETCURSEL, 0, 0L) == CB_ERR)
		{
		char name[MAXNAME];
		(int)SendMessage(hCtrl, CB_SETCURSEL, 0,0);
    // SKJ now set the defitem field of the DDEC objd class MAY 21, 1994
		(int)SendMessage(hCtrl, CB_GETLBTEXT,0, (LONG)(LPSTR)name);
		ChangeName(hWnd, name, IDITEM);
		}

	lpData = (LPSTR)AMemUnlock(dataID);
	return itemCount;
	}



/****************************************************************************
 *                                                                          *
 *  METHOD    : SetDDEFormatList()                                                 *
 *                                                                          *
 *  DESCRIPTION    : Build A DDE Item List
 *               
 *  RETURNS    : 														                                *
 *                                                                          *
 ****************************************************************************/
int OBJDCLASS::SetDDEFormatList(HWND hWnd, MEMBLOCKID dataID )
	{
	int i =0;
	LPSTR lpData;
	if (dataID)
		lpData = (LPSTR)AMemLock(dataID);
	else
		lpData = NULL;

	int index = 0;
	LPSTR lpOld = lpData;
	LPSTR lpTab;
	LPNAME *serFmtList;

	HWND hCtrl = GetDlgItem(hWnd, IDFORMAT);
	formatList = (LPNAME *)new LPNAME[MAXFORMATS];

	formatCount = 0;
	// delete all the old Formats
	(int)SendMessage(hCtrl, CB_RESETCONTENT, 0, 0L);

	if ( lpData)
		{
    lpTab = lpData;
  	if (lpTab)
	  	{
			i = 1;
    	}
		while ((lpTab) && ((lpTab = strchr(lpTab, '\t')) != NULL))
			{
			i++;
			lpTab++;
			}

		formatCount = i;
		serFmtList = (LPNAME *)new LPNAME[formatCount];
		i = 0;

		while (lpData && ((lpTab = strchr(lpData, '\t')) != NULL))
			{
			int len = strlen(lpData) - strlen(lpTab);
			if (len > MAX_NAME)
				len = MAX_NAME;
			strncpy(serFmtList[i], lpData,len);
		 	serFmtList[i][len] = '\0';
			lpData = ++lpTab;
			i++;
			// when it reaches the end of the string
			if (lpTab != strchr(lpData, '\t')) {
				strncpy(serFmtList[i], lpData, MAX_NAME);
				serFmtList[i][MAX_NAME - 1] = '\0';
        }
			}
		}

	if (formatCount == 0)
		{
		// if there are no valid formats, then allow the user to choose from
		// any of the predetermined list of formats.  The error check in this
		// will not be done till runtime
		formatCount = MAXFORMATS;
		for (i = 0; i < MAXFORMATS; i++)
			{
			int curindex;
			strncpy(formatList[i],SFormats[i].name, MAX_NAME);
			formatList[i][MAX_NAME - 1] = '\0';
			curindex = (int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)formatList[i]);
			(int)SendMessage(hCtrl, CB_SETITEMDATA, curindex,SFormats[i].typeID);
			}
		}
	else {
	  index = 0;
		for (i = 0; i < formatCount; i++)
			{
			int idVal;
			if ((idVal = IsInFormatList(serFmtList[i])) != SDDE_NOMATCHINGFORMAT)
			  {int curindex;
				strncpy(formatList[index], serFmtList[i], MAX_NAME);
				formatList[index][MAX_NAME - 1] = '\0';
				curindex = (int)SendMessage(hCtrl, CB_ADDSTRING, 0, (LONG)(LPSTR)formatList[index]);
				(int)SendMessage(hCtrl, CB_SETITEMDATA, curindex,(DWORD)idVal);

				index++;
        }
			}
		formatCount = index;
		}

	if ((int)SendMessage(hCtrl, CB_GETCURSEL, 0, 0L) == CB_ERR)
		{
		char name[MAXNAME];
		(int)SendMessage(hCtrl, CB_SETCURSEL, 0,0);
		(int)SendMessage(hCtrl, CB_GETLBTEXT,0, (LONG)(LPSTR)name);
		ChangeName(hWnd, name, IDFORMAT);
		}

	AMemUnlock(dataID);
	I_UNUSED(lpOld);
	return formatCount;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DoDialog()                                                 *
 *                                                                          *
 *  DESCRIPTION    : Generic dialog invocation routine.  Handles procInstance   *
 *               stuff, and param passing.                 *
 *  RETURNS    : result of dialog procedure.                                *
 *                                                                          *
 ****************************************************************************/
int FAR DoDialog(
  LPCSTR lpTemplateName,
	DLGPROC lpDlgProc,
  OBJECTID objID)
  {
  WORD wRet;
	HWND hwndParent = GetActiveWindow();
	// lpDlgProc = MakeProcInstance(lpDlgProc, hInst);
	wRet = DialogBoxParam(hInst, lpTemplateName, hwndParent, (DLGPROC)lpDlgProc, (long)objID);
	// FreeProcInstance(lpDlgProc);
	return wRet;
	}


/****************************************************************************
 *                                                                          *
 *  METHOD   	:  RunDDEServer()                                             *
 *                                                                          *
 *  DESCRIPTION    : Run a DDE Server
 *
 *  RETURNS    :	procedure.								                                *
 *                                                                          *
 ****************************************************************************/
unsigned int OBJDCLASS::RunDDEServer(HWND hwnd, LPSTR serverName)
	{
	unsigned int rVal;
	if ((rVal = dde->RunServer(NULL)) < 32)
	  {
		dde->MPError(DDEC::hConvWnd, MB_OK, IDS_DDEMLERR, (LPSTR)"Cannot Start Server Application");
		}
	else {
		// If we are successfully able to start a server, check it for def service
		// and build a new Topic and Item list
		// Should I set the current def Strings back into the controls
		// 
		/*
		LPSTR oldService = new char[strlen(dde->convInfo.Service) + 1];
		LPSTR oldTopic = new char[strlen(dde->convInfo.Topic) + 1];
		LPSTR oldItem = new char[strlen(dde->defItem) + 1];
		LPSTR oldFormat = new char[strlen(dde->defFormat) + 1];
		lstrcpy(oldService, dde->convInfo.Service);
		lstrcpy(oldTopic, dde->convInfo.Topic);
		lstrcpy(oldItem, dde->defItem);
		lstrcpy(oldItem, dde->defFormat);
    
		if (GetSystemInfo(hwnd))
			{
			SetUndefItemInCombo(GetDlgItem(hwnd, IDSERVICE), oldService);

			// Topic List
			SetUndefItemInCombo(GetDlgItem(hwnd, IDTOPIC),  oldTopic);
			ChangeName(hwnd, oldTopic,IDTOPIC);

			// Format list
			i = SetUndefItemInCombo(GetDlgItem(hWnd, IDFORMAT), oldFormat);
			idVal = IsInFormatList((LPSTR)oldFormat);
			(int)SendMessage(GetDlgItem(hWnd, IDFORMAT), CB_SETITEMDATA,i, (DWORD)idVal);

  		// Item name
			SetUndefItemInCombo(GetDlgItem(hWnd, IDITEM), oldItem);
			ChangeName(hwnd, oldItem,IDITEM);

			SetComboItem(hWnd, IDOBJTYPE,  	oldTypeName);
			SetComboItem(hWnd, IDOBJECT,  	oldObjectName);


			}
		delete oldService;
		delete oldTopic;
		delete oldItem;
    */
		}
  
	I_UNUSED(serverName);
	I_UNUSED(hwnd);

	return rVal;
  }


/****************************************************************************
 *                                                                          *
 *  METHOD   	:  BuildObjTypeList()                                         *
 *                                                                          *
 *  DESCRIPTION    : Builds a Serius Object Type list from a chosen format	                        *
 *               													                                  *
 *  RETURNS    :														                                *
 *                                                                          *
 ****************************************************************************/
int OBJDCLASS::BuildObjTypeList(HWND hwnd, ATOM formatID)
	{
	int typecount = (int)ATypeGetCount();
	long int i;
	TYPEINFO *typeList = (TYPEINFO *) new TYPEINFO[typecount];
	TYPEID osType;
	HWND hTypeWnd = GetDlgItem(hwnd, IDOBJTYPE);
	int  curindex = 0;

	typecount = 0;
	switch (formatID)
		{
		case TEXT_FORMAT:
			{
			i = AOBJ_GETFIRSTKEY;
			while ((osType = (TYPEID)ATypeGetNext(&i)) != 0L){
      	char name[80];
				ATypeGetName(osType, name, 20);
				if (!AEvtCheckForMethod(osType, AEVENT_TEXTTOOBJECT))
					continue;
				strncpy(typeList[typecount].name, name, MAX_NAME);
				typeList[typecount].name[MAX_NAME - 1] = '\0';
				typeList[typecount].typeID = osType;
				typecount++;
				}
			}
			break;

		case BITMAP_FORMAT:
			{
			i = AOBJ_GETFIRSTKEY;
			while ((osType = (TYPEID)ATypeGetNext(&i))!= 0L) {
      	char name[80];

      	ATypeGetName(osType, name, 20);
				if (!AEvtCheckForMethod(osType, OPICT_EVT_BITMAPTOOBJECT))
					continue;
				strncpy(typeList[typecount].name, name, MAX_NAME);
				typeList[typecount].name[MAX_NAME - 1] = '\0';
				typeList[typecount].typeID = osType;
				typecount++;
				}
			}
      break;

		case METAFILE_FORMAT:
			{
			i = AOBJ_GETFIRSTKEY;
			while ((osType = (TYPEID)ATypeGetNext(&i)) != 0L)
				{
      	char name[80];
				ATypeGetName(osType, name, 20);
				if (!AEvtCheckForMethod(osType, metafileToObject))
					continue;
				strncpy(typeList[typecount].name, name, MAX_NAME);
				typeList[typecount].name[MAX_NAME - 1] = '0';
				typeList[typecount].typeID = osType;
				typecount++;
				}
			}
			break;
    
		case ALMOBJ_FORMAT:
		default:
			{
			while ((osType = ATypeGetNext(&i)) != 0L)
				{
				char name[80];
				int objCount = 0;
        // if no object of this type is instantiated
				if ((objCount = (int)AObjGetObjectCount(osType)) == 0)
					continue;
				ATypeGetName(osType, name, 20);
				strncpy(typeList[typecount].name, name, MAX_NAME);
				typeList[typecount].name[MAX_NAME - 1] = '0';
				typeList[typecount].typeID = osType;
				typecount++;
				I_UNUSED(objCount);
				}
			}
			break;
		}

	SendMessage(hTypeWnd, CB_RESETCONTENT, 0,0);
	if (typecount <= 0)
		EnableWindow(hTypeWnd, FALSE);  // should I return from here
	else
		EnableWindow(hTypeWnd, TRUE);

	int nIndex;
	for (nIndex = 0; nIndex < typecount; nIndex++)
    {
		if (SendMessage(hTypeWnd, CB_FINDSTRINGEXACT,(WPARAM) -1,(LONG)(LPSTR)(typeList[nIndex].name)) == CB_ERR)
      {
			curindex = (int)SendMessage(hTypeWnd, CB_ADDSTRING, 0,(LONG)(LPSTR)(typeList[nIndex].name));
			SendMessage(hTypeWnd, CB_SETITEMDATA, curindex, (long)(typeList[nIndex].typeID));
			}
		}

	curindex = 0;
	SendMessage(hTypeWnd, CB_SETCURSEL, curindex, (LONG)0);
	// Get the TYPE id of the current Type selected
	osType = (TYPEID)SendMessage(hTypeWnd,CB_GETITEMDATA, curindex,0);
	
	// Build the object list from the chosen type
	BuildObjectList(hwnd, osType);
	SetObjType(osType);
  delete typeList;

	return 0;
	}


/****************************************************************************
 *                                                                          *
 *  METHOD   	:  BuildObjectList()                                             *
 *                                                                          *
 *  DESCRIPTION    : Builds a Serius Object  list from a chosen type        *	                        *
 *               													                                  *
 *  RETURNS    :														                                *
 *                                                                          *
 ****************************************************************************/
int OBJDCLASS::BuildObjectList(HWND hwnd, TYPEID osType)
	{                                        	
	int typecount = (int)ATypeGetCount();
	int objCount = (int)AObjGetObjectCount(osType);

	long int i;
	int ctrlID = IDOBJECT;
	HWND hObjWnd = GetDlgItem(hwnd, IDOBJECT);
	int curindex;
	OBJECTID oid = 0;


	// if only type is DDEC object
	if ((typecount <= 1) || (objCount == 0))
		{
		SendDlgItemMessage(hwnd, ctrlID, CB_RESETCONTENT, 0,0);
		SetObject(0); // set the id to be invalid (if there are no objects of a given type)
		EnableWindow(hObjWnd, FALSE );
    return FALSE;
    }
	else
		EnableWindow(hObjWnd, TRUE );

	// Only display objects of the TYPE that the
	// user chooses.
	
	// Only change it if the selection is different from old selection
	// or if there is no current selection
	curindex = (int)SendMessage(hObjWnd, CB_GETCURSEL, 0,0);

	if ((osType != dde->defObjectType) || (curindex == CB_ERR))
		{
		OBJECTID objRec;
		SendDlgItemMessage(hwnd, ctrlID, CB_RESETCONTENT, 0,0);

		dde->defObjectType = osType;
		i = AOBJ_GETFIRSTKEY;


		curindex = 0;
		while ((objRec = (OBJECTID)AObjGetNextObject(osType, &i)) != NULL){
      char objName[MAXNAME];
			AObjGetName(objRec, objName, 20);
			// if ((curindex = (int)SendDlgItemMessage(hwnd, ctrlID, CB_FINDSTRING, curindex -1,(LONG)(LPSTR)objName)) == CB_ERR)
			//  {
			curindex = (int)SendDlgItemMessage(hwnd, ctrlID, CB_ADDSTRING, 0, (LONG)(LPSTR)objName);
			SendDlgItemMessage(hwnd, ctrlID, CB_SETITEMDATA, curindex,(long)objRec);
			// }
			}

		curindex = 0;
		(int)SendMessage(hObjWnd, CB_SETCURSEL, curindex, (LONG)0);
	  // Get the TYPE id of the current Type selected
		oid = (OBJECTID)SendMessage(hObjWnd,CB_GETITEMDATA, curindex,0);

	  SetObject(oid);
	  // Build the object list from the chosen type

		}

	return 0;
	}


/*************************************************************************
*	Function: OpenFileHook
*                                                                        *
*************************************************************************/

static
UINT		ALMCALLBACK OpenFileHook(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	UINT		nRes;

	switch (nMsg)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hDlg, 0);
			break;

		default:
			nRes = 0;
			break;
		}

	I_UNUSED(wParam);
	I_UNUSED(lParam);

	return nRes;
	}

/*************************************************************************
*	Function: BrowseServer                                       					 *
*						Calls CommDlg to use the FILEOPEN function.                  *
*						Lets the user choose the SERVER application they want to run *
*                                                                        *
*************************************************************************/
static char szFile[256] = "\0";
char szFilter[256];

extern "C" int BrowseServer(
	HWND 					hWnd,
	LPSTR         ServerName,
	LPSTR         pathName)
	{
	char 					szDirName[256];
	static char 	szFileTitle[256];
  char          sztmpFile[256];
	int 					rVal = 1;
	OPENFILENAME 	opn;
	
  LPSTR         lpext;
	int           size;

	memset((LPVOID)&opn, 0, sizeof(OPENFILENAME));
	
	lstrcpy(szFilter, "DDE Servers(*.exe) *.exe");
	lstrcpy(sztmpFile, "*.exe");
	
	lpext = strchr(szFilter, ')');
	size = strlen(szFilter);

	lpext[1] = '\0';
	szFilter[size + 1] = '\0';


	szDirName[0] = '\0';

  if (szFile[0] == '\0')
		{
		GetSystemDirectory(szDirName, sizeof(szDirName));
		strcpy(szFile, sztmpFile);
		}
	else
		{
		LPSTR lpOld;
		LPSTR lpTmp = new char[strlen(szFile) + 1];
	
		strcpy(lpTmp, szFile);
		lpOld = lpTmp;
		while ((lpTmp = strchr(lpTmp, '\\')) != NULL)
			{
			lpOld = lpTmp;

			lpTmp++;


			}
		szFile[strlen(szFile) - strlen(lpOld)] = '\0';

		strcpy(szDirName, szFile);
		strcpy(szFile, sztmpFile);

		delete lpTmp;

		}
	opn.lStructSize       = sizeof(OPENFILENAME);
	opn.hwndOwner         = (HWND)hWnd;
	opn.lpstrFilter       = szFilter;
	opn.nFilterIndex      = 1L;
	opn.lpstrFile         = szFile;
	opn.nMaxFile          = (DWORD)sizeof(szFile);
	opn.lpstrFileTitle    = szFileTitle;
	opn.nMaxFileTitle     = MAX_NAME;
	opn.lpstrInitialDir   = szDirName; 
	opn.lpstrTitle        = (LPSTR)"Choose Server";
	opn.Flags             = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY
													| OFN_NOCHANGEDIR |
													OFN_ENABLEHOOK;
	opn.lpfnHook          = OpenFileHook;

	rVal = GetOpenFileName(&opn);

	strncpy(ServerName, szFileTitle, MAX_NAME);
	ServerName[MAX_NAME - 1] = '\0'; // max length
	strncpy(pathName, szFile, MAX_NAME);
	pathName[MAX_NAME - 1] = '\0'; // max length


	return rVal;
	}

/*********************************************************************
*	FUNCTION:  		GetOBJDPtr
*	DESCRIPTION:	This function will create a C++ ptr during INITDIAOG msg
								and stuff it in the object datas.  For subsequent calls
*								it returns this pointer
*
*
**********************************************************************/
LPLONG GetOBJDPtr(HWND hwnd, OBJECTID theObject, WORD msg )
	{
	OBJDCLASS	*objd = NULL;
	if (msg != WM_INITDIALOG)
		{
		objd = (OBJDCLASS *) AObjGetRuntimeInfo(theObject);
		if ((objd == NULL) || (!objd->IsDDE()))
			{
			return NULL;
			}
		}
	else if (msg == WM_INITDIALOG)
		{
		objd = new OBJDCLASS(hwnd , theObject);
		}
	return (LPLONG)objd;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  ClientDlgProc                                                          *
 *                                                                          *
 *  DESCRIPTION    :  Dialog Callback for DDE client OBJD handling                                                          *
 *                                                                          *
 *  RETURNS    :                                                            *
 *			*
 ****************************************************************************/
int ALMCALLBACK ClientDlgProc(
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
				EnablePaste(hwnd);
				return FALSE;
				}
			 }
		case WM_INITDIALOG:
			{
			char title[40];

			InitializeDDE();
			theObject = (OBJECTID) lParam;
			objd = (OBJDCLASS*)GetOBJDPtr(hwnd, theObject ,msg);
			if (!objd)
        break;

			SendDlgItemMessage(hwnd, IDSERVER, EM_LIMITTEXT, MAX_NAME, 0);
			SendDlgItemMessage(hwnd, IDSERVICE, CB_LIMITTEXT, MAX_NAME, 0);
			SendDlgItemMessage(hwnd, IDTOPIC, CB_LIMITTEXT, MAX_NAME, 0);
			SendDlgItemMessage(hwnd, IDFORMAT, CB_LIMITTEXT, MAX_NAME, 0);
			SendDlgItemMessage(hwnd, IDOBJECT, CB_LIMITTEXT, MAX_NAME, 0);
			SendDlgItemMessage(hwnd, IDOBJTYPE, CB_LIMITTEXT, MAX_NAME, 0);

			objd->InitControlsFromDatas(hwnd);
			EnablePaste(hwnd);

			ATypeGetName(OTYPE_DDEC, (LPSTR)title, 32);
			AUtlSetTitleFont(OTYPE_DDEC, title, GetDlgItem(hwnd, IDTITLE));

			// SKJ Nov. 2, 1994 Set Title
			AObjGetName((OBJECTID)lParam, (LPSTR)title, 32);
			SetWindowText(hwnd, (LPSTR)title);


			AUtlCenterDialog(hwnd, 0);

			}
			return TRUE;

		case UM_SETSERVICELIST:  	 // user defined event
															 // posted from DDEML callback at
															 // XTYP_REGISTER/UNREGISTER handling
			{
			objd = (OBJDCLASS*)GetOBJDPtr(hwnd, theObject ,msg);
			if (!objd)
        return FALSE;
			objd->SetDDEServiceList(hwnd, wParam);
			}
			return TRUE;


		case WM_COMMAND:
			objd = (OBJDCLASS*)GetOBJDPtr(hwnd , theObject ,msg);
			if (!objd)
        return FALSE;

			switch (wParam)
				{
				case IDOK:
					{
					// save the info back into datas
					objd->SaveDatas(hwnd);
					delete objd;
					UninitializeDDE();
					EndDialog(hwnd, TRUE);
					return TRUE;
					}

				case IDCANCEL:
					{
					delete objd;
					UninitializeDDE();
					EndDialog(hwnd, FALSE);
					return TRUE;
					}

				case IDPASTELINK:
				  {
					HANDLE hClipData;
          LPSTR psz;
					BOOL bIsData = FALSE;

          if (OpenClipboard(hwnd)) {
						if ((hClipData = GetClipboardData((unsigned int)fmtLink)) != 0){
							if ((psz = (LPSTR)GlobalLock(hClipData)) != NULL) {
								char service[MAXNAME], topic[MAXNAME], item[MAXNAME];
                /*
								* Create a conversation with the link app and
                * begin a request and advise start transaction.
							  */
                
								strncpy(service, psz, MAX_NAME);
								service[MAX_NAME - 1] = '\0';
								objd->ChangeName(hwnd, service, IDSERVICE);
								SetUndefItemInCombo(GetDlgItem(hwnd, IDSERVICE), service);

								strncpy(topic,&psz[_fstrlen(psz) + 1], MAX_NAME);
								topic[MAX_NAME - 1] = '\0';
								objd->ChangeName(hwnd, topic, IDTOPIC);
								SetUndefItemInCombo(GetDlgItem(hwnd, IDTOPIC), topic);
								
								psz += _fstrlen(psz) + 1;
                psz += _fstrlen(psz) + 1;
								strncpy(item, psz,MAX_NAME);
								item[MAX_NAME - 1] = '\0';
								objd->ChangeName(hwnd, psz, IDITEM);
								SetUndefItemInCombo(GetDlgItem(hwnd, IDITEM), item);

								bIsData = TRUE;
							  }
              GlobalUnlock(hClipData);
              }
            }
					CloseClipboard();

					if (!bIsData)
						{
						MessageBox(hwnd, "No Link Information in Clipboard", "DDEC Paste Link", MB_OK);
						}
					bHandled = TRUE;
					break;
          }
				case IDHELP:
					{
					MessageBox(hwnd, "Help Not Implemented", "DDEC HELP", MB_OK);
					bHandled = TRUE;
					}
					break;

				case IDBROWSE: // Browse for Server Application Name
					{
					LPSTR serverName = (LPSTR)new char[MAX_NAME];
					LPSTR pathName = (LPSTR)new char[MAX_NAME];
					if (BrowseServer(hwnd,(LPSTR)serverName, pathName))
						{
						SetWindowText(GetDlgItem(hwnd, IDSERVER), pathName);
						objd->SetServerName(hwnd, pathName);
						}
					bHandled = TRUE;
					}
					break;


			 	case IDSERVER: // Text field to enter Server App name
					{
          // if the server name has changed, then reset the system, topic, item lists
					if ((HIWORD(lParam) == EN_KILLFOCUS) && (bDlgHasFocus == TRUE))
						{
						char name[MAXNAME];
						GetWindowText(GetDlgItem(hwnd,  IDSERVER), name, MAXNAME);
						objd->SetServerName(hwnd, name); // if name is diff from old
						bHandled = TRUE;
						}
					}
          break;

        
				case IDEXECUTE:
					{
					if (objd->RunDDEServer(hwnd, NULL) < 32)
						{
						;
						}
					bHandled = TRUE;
					}
					break;

				// This will simulate a dde connection test
				case IDDDETEST:
					{
					objd->SetDisplayFlag(TRUE); // set bDispError flag
					objd->DDECTest(hwnd);
					objd->SetDisplayFlag(FALSE);
					}
					break;

				case IDTOPIC:
				case IDFORMAT:
				case IDITEM:
				case IDSERVICE:
					{
					
					// if the user has edited the name of the service, then add that as user
					// defined string
					if (HIWORD(lParam) == CBN_KILLFOCUS)
						{

						char name[MAXNAME];
						HWND hCtrl = GetDlgItem(hwnd,  wParam);
						GetDlgItemText(hwnd, wParam,name, MAXNAME);

						// if no matching string is found then add this string as user
						// defined
						SetUndefItemInCombo(hCtrl,name);
						objd->ChangeName(hwnd, name, wParam ); // if name is diff from old
						

						} // if the server name has changed, then reset the system, topic, item lists
          else if (HIWORD(lParam) == CBN_CLOSEUP)
						{
					  char name[MAXNAME];
						HWND hCtrl = GetDlgItem(hwnd,  wParam);

						int index = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);

						(int)SendMessage(hCtrl, CB_GETLBTEXT, index, (LONG) (LPSTR)name);

						objd->ChangeName(hwnd, name, wParam); // if name is diff from old

						}
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
					  char name[MAXNAME];
						HWND hCtrl = GetDlgItem(hwnd,  wParam);
						int index = (int)SendMessage(hCtrl, CB_GETCURSEL,0,0L);
						(int)SendMessage(hCtrl, CB_GETLBTEXT, index, (LONG) (LPSTR)name);

						objd->ChangeName(hwnd, name, wParam); // if name is diff from old

						}
				
 					bHandled = TRUE;
					}
          break;


				case IDAUTOLINK:
				case IDNOTIFYLINK:
				case IDMANUALLINK:
					{
					if (HIWORD(lParam) == BN_CLICKED)
						{
						objd->SetLinkType(wParam); // set the link type

						}
					bHandled = TRUE;
					}
					break;

				case IDOPTIONS:
					{

					DoDialog(MAKEINTRESOURCE(OPTIONS), (DLGPROC)OptionsDlgProc, (OBJECTID)objd);
					break;
				 
					}

				}
			break;  // end of case WM_COMMAND
		}  // end of main Switch
	return bHandled;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
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

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
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

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
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

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
void SetOptions(OBJDCLASS *objd, int ctrlID, long val)
	{
	DDEC *dde = objd->dde;
	switch (ctrlID)
		{
		case IDEF_SECURITY:
			{
      dde->CCFilter.dwSecurity = val;
			}
			break;

		case IDADVDELAY:
			{
			dde->defAdvDelay = val;
			}
			break;

		case IDTIMEOUT:
			{
			dde->timeout = val;
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
		case IDACK:
			dde->bDefAck = (BOOL)val;
    default:

			;

    }
	}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
void SetOptionsStr(OBJDCLASS *objd, int ctrlID, LPSTR lpStr)
	{
	DDEC *dde = objd->dde;
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


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
LPSTR GetOptionsStr(OBJDCLASS *objd, int ctrlID)
	{
	DDEC *dde = objd->dde;
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

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
long int GetOptions(OBJDCLASS *objd, int ctrlID)
	{
  DDEC *dde = objd->dde;
	switch (ctrlID)
		{
		case IDDEFAULT:
			{
			long rVal =  (long)dde->bDefContext;
			return rVal;
			}

		case IDACK: // if a transaction rquires a ACKs
			{
			long rVal =  (long)dde->bDefAck;
			return rVal;
			}

		case IDEF_SECURITY:
			{
      return dde->CCFilter.dwSecurity;
			}
     
		case IDADVDELAY:
			{
			return dde->defAdvDelay;
			}
			
		case IDTIMEOUT:
			{
			return dde->timeout;
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


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
void InitOptions(HWND hWnd, OBJDCLASS *objd)
	{
  BOOL defVal;
	BuildCountry(hWnd, objd);
	BuildLanguage(hWnd, objd);
	BuildCodePage(hWnd, objd);

	SetDlgItemInt(hWnd, IDTIMEOUT, (UINT)GetOptions(objd, IDTIMEOUT), TRUE);
	SetDlgItemInt(hWnd, IDADVDELAY,(UINT)GetOptions(objd, IDADVDELAY), TRUE);

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


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  
 *                                                                          *
 *  DESCRIPTION :   
 *                                                                          *
 *  RETURNS    :                                                            *
 ****************************************************************************/
void SaveOptions(HWND hWnd, OBJDCLASS *objd)
	{
	int val;
	BOOL flag;
	char lpStr[MAXNAME];

	if (objd->dde->bDefContext)
		{
		_fmemset(&(objd->dde->CCFilter), 0, sizeof(CONVCONTEXT));
		GetProfileString("intl", "sCountry", "United States", objd->dde->Country, MAXNAME);
		GetProfileString("intl", "sLanguage", "U.S. English",objd->dde->Language, MAXNAME);

		strcpy(objd->dde->Language, "Modern English");
		strcpy(objd->dde->CodePage,"ANSI");
		objd->dde->CCFilter.iCodePage  = CP_WINANSI;
		objd->dde->CCFilter.wCountryID = GetProfileInt("intl", "iCountry", 1);
		objd->dde->CCFilter.dwLangID = atol(objd->dde->Language);
		objd->dde->CCFilter.cb = sizeof(CONVCONTEXT);
		objd->dde->CCFilter.dwSecurity = 0;

		}
  else {
 	
		SetOptions(objd, IDEF_SECURITY,GetDlgItemInt(hWnd, IDEF_SECURITY, &flag, FALSE));
		SetOptions(objd, IDTIMEOUT, 	GetDlgItemInt(hWnd, IDTIMEOUT, &flag, FALSE ));
		SetOptions(objd, IDADVDELAY, 	GetDlgItemInt(hWnd, IDADVDELAY, &flag, FALSE));
		SetOptions(objd, IDACK,  SendDlgItemMessage(hWnd, IDACK, BM_GETCHECK,0,0));
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
 *  FUNCTION   :     OptionsDialogBox	                                                       *
 *                                                                          *
 *  PURPOSE    :     To get users input on  DDE Conversation Options                                                       *
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
extern "C" int FAR PASCAL _export OptionsDlgProc (
  HWND          hwnd,
  WORD msg,
  WORD wParam,
	LONG          lParam)
	{
		static OBJECTID theObject;
		static OBJDCLASS *objd;

		switch (msg){

				case WM_INITDIALOG:
						{
						char title[42];
						objd = (OBJDCLASS *) lParam;
						InitOptions(hwnd , objd);
						ATypeGetName(OTYPE_DDEC, (LPSTR)title, 32);
						strcat(title, " Options");
						AUtlSetTitleFont(OTYPE_DDEC, title, GetDlgItem(hwnd, IDTITLE));
						AUtlCenterDialog(hwnd, 0);

						// SKJ Nov. 2, 1994 Set Title
						AObjGetName((OBJECTID)objd->dde->ddeCID, (LPSTR)title, 32);
						SetWindowText(hwnd, (LPSTR)title);

            }
            break;

        case WM_COMMAND:
						switch (wParam){
								case IDDEFAULT: // choose the default conv context
									{
									BOOL bChange = FALSE;
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
									I_UNUSED(bChange);
									}

									return FALSE;

								case IDOK:
										SaveOptions(hwnd, objd);
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



///////////////////////////////////////////////////////////////////////
// This function is called to edit the CLIENT OBJECT
//
//
//
//////////////////////////////////////////////////////////////////////
long FAR PASCAL EditClientObject(OBJECTID theObject)
	{
	long rVal = A_OK;
	// DoDialog(MAKEINTRESOURCE(IDDDECLIENT), (FARPROC)ClientDlgProc, theObject);
	int i = DoDialog("DDECLIENT", (DLGPROC)ClientDlgProc, theObject);
	if (i == -1)
		rVal = A_ERROR;
	else if (i)
		rVal = A_MODIFIED;
  
  return rVal;
  }



