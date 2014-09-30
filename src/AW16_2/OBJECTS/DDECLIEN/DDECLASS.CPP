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
// FILE:      	DDECLASS.cpp
//
// AUTHOR:      Shaili Jain
//
// DESCRIPTION:	Contains routines for handling of DDE interaction with  
//	 						DDEML.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT
#include <string.h>
#include <memory.h>
#include <windows.h>
#include <a_alm.h>
#include "o_pict.h"


/***************************************************************************
 *                                                                         *
 *  MODULE      : dde.c                                                    *
 *                                                                         *
 *  PURPOSE     : Contains routines for handling of DDE interaction with   *
 *                DDEML.                                                   *
 *                                                                         *
 ***************************************************************************/
#include "ddemlcl.h"
#include "runtime.h"
#include "toolhelp.h"
/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)    (((i) + 31) / 32 * 4)


MEMBLOCKID DDEC::serviceListID;
HWND       DDEC::hConvWnd;
extern int GetLinkType(LPSTR linkName);
extern ATOM GetFormatID(char *formatStr);

class OBJDCLASS;

void GetClipboardBMP(HBITMAP hbm,	MEMBLOCKID memID);

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : 						                                                *
 *                                                                          *
 *  PURPOSE    : 																													  *
 *																																				  *
 ****************************************************************************/
int PostAtOnceSignal(OBJECTID theObject, long theSignal)
	{
	AEvtInfo event;
	int debug = 1;
	// Post Object Changed
	event.sourceObject  = theObject;
  event.targetObject  = theObject;
  event.when          = AAppGetTime();
  event.how           = AEVT_ATONCE;
  event.what          = AEVENT_SIGNAL;
	event.details       = theSignal;

	if (debug)
		{
		return AEvtPostSignalAtMark(theObject, theSignal);

		}
  else
	  return AEvtPost(&event);

	}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetHSZName()                                               *
 *                                                                          *
 *  PURPOSE    : Allocates local memory for and retrieves the string form   *
 *               of an HSZ.  Returns a pointer to the local memory or NULL  *
 *               if failure.  The string must be freed via MyFree().        *
 *                                                                          *
 ****************************************************************************/
LPSTR GetHSZName(
	HSZ hsz)
	{
	LPSTR psz;
  WORD cb;
	DWORD idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);
  cb = (WORD)DdeQueryString(idInst, hsz, NULL, 0, 0) + 1;
  psz = MyAlloc(cb);
  DdeQueryString(idInst, hsz, psz, cb, 0);
  return(psz);
	}                                                                         


/****************************************************************************
*  Function:    	RunServer
*  Description:  	This Method will call WinExec to execute a server application
*
*
*****************************************************************************/
UINT  DDEC::RunServer(LPSTR defName)
	{
	unsigned int rVal;
	serverInst = 0;
	if (defName == NULL) {
		rVal = WinExec(convInfo.ServerName, SW_MINIMIZE);
		if (rVal >= 32) // set the hInst of the server
			serverInst = (HINSTANCE)rVal;
    
    }
	else 
		rVal = WinExec(defName, SW_MINIMIZE);
    
  return rVal;

  }



/***********************************************************************************
* StopServer : Send a WM_CLOSE to all top level windows to close the started app
*
*
************************************************************************************/
struct TASKSTRUCT {
	HINSTANCE hServerInst;
  BOOL bFoundTask;
	};



/*************************************************************************
* Enum all windows and find their task.  Then compare the task hInst with
* the hInst of the DDE Server APplication (Passed in as lParam).  If it
* matches, then post a CLOSE msg to the window
*************************************************************************/
BOOL CALLBACK StopTaskProc(HWND hWnd, LONG lpParam)
	{

	TASKSTRUCT *lpTaskStruct = (TASKSTRUCT *) lpParam;
	HTASK			 hCurrentTask =  GetWindowTask(hWnd);
  TASKENTRY	 taskEntry;
	// hCurrentInst = (HINSTANCE) GetWindowWord(hWnd, GWW_HINSTANCE);
	taskEntry.dwSize =  sizeof(TASKENTRY);

	if (TaskFindHandle(&taskEntry, hCurrentTask) == FALSE)
    return TRUE;

	if (taskEntry.hInst == lpTaskStruct->hServerInst)
		{
		// send WM_CLOSE top all top level windows of this task
		SendMessage(hWnd, WM_CLOSE, 0,0);
    lpTaskStruct->bFoundTask = TRUE;
    }
  return TRUE;  
  }
/****************************************************************************
*  Function:    StopServer
*  Description:  This Method will stop execution a server application
*
*
*****************************************************************************/
int DDEC::StopServer()

	{
	TASKENTRY taskEntry;
	BOOL notFound = TRUE;
	int  rVal = DDEC_FAILURE;
	
	taskEntry.dwSize = sizeof(TASKENTRY);
	BOOL bIsTask = TaskFirst(&taskEntry);

	if (taskEntry.hInst == serverInst)
		notFound = FALSE;
	while (bIsTask && notFound)
		{
		bIsTask = TaskNext(&taskEntry);
		if ((bIsTask) && (taskEntry.hInst == serverInst))
			notFound = FALSE;
		}

	if (notFound == FALSE)
		{
		TASKSTRUCT tStruct;
		rVal = DDEC_FAILURE;
		tStruct.hServerInst = serverInst;
		tStruct.bFoundTask = FALSE;
		EnumWindows(StopTaskProc, (LONG)(LONG *)&tStruct);

		if (tStruct.bFoundTask == TRUE)
    	{
			rVal = DDEC_SUCCESS;
			serverInst = 0;
			}
		
		/*
		int wMsg = WM_QUIT; // WM_CLOSE;
		PostAppMessage(taskEntry.hTask, wMsg, 0,0);
		*/
		}

	return rVal;
  }



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : State2String()                                             *
 *                                                                          *
 *  PURPOSE    : converts a conversation state word to a string and         *
 *               returns a pointer to that string.  The string is valid     *
 *               till the next call to this routine.                        *
 *                                                                          *
 ****************************************************************************/
LPSTR State2String(
	WORD state)
	{
  static char *s2s[] = {
        "NULL"             ,
        "Incomplete"       ,
        "Standby"          ,
        "Initiating"       ,
        "ReqSent"          ,
        "DataRcvd"         ,
        "PokeSent"         ,
        "PokeAckRcvd"      ,
        "ExecSent"         ,
        "ExecAckRcvd"      ,
        "AdvSent"          ,
        "UnadvSent"        ,
        "AdvAckRcvd"       ,
        "UnadvAckRcvd"     ,
        "AdvDataSent"      ,
        "AdvDataAckRcvd"   ,
        "?"                ,    // 16
    };

  if (state >= 17)
  	return s2s[17];
  else
  	return s2s[state];
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : Status2String()                                            *
 *                                                                          *
 *  PURPOSE    : Converts a conversation status word to a string and        *
 *               returns a pointer to that string.  The string is valid     *
 *               till the next call to this function.                       *
 *                                                                          *
 ****************************************************************************/
LPSTR Status2String(
  WORD status)
  {
  WORD c, i;
  static char szStatus[6 * 18];
  static struct {
        char *szStatus;
        WORD status;
    } s2s[] = {
        { "Connected"    ,   ST_CONNECTED },
        { "Advise"       ,   ST_ADVISE },
        { "IsLocal"      ,   ST_ISLOCAL },
        { "Blocked"      ,   ST_BLOCKED },
        { "Client"       ,   ST_CLIENT },
        { "Disconnected" ,   ST_TERMINATED },
        { "BlockNext"    ,   ST_BLOCKNEXT },
    };
#define CFLAGS 7
	szStatus[0] = '\0';
  c = 0;
  for (i = 0; i < CFLAGS; i++) {
  	if (status & s2s[i].status) {
    	if (c++)
      	strcat(szStatus, " | ");
        strcat(szStatus, s2s[i].szStatus);
      }
    }
  return szStatus;
#undef CFLAGS
	}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : Type2String()                                              *
 *                                                                          *
 *  PURPOSE    : Converts a wType word and fsOption flags to a string and   *
 *               returns a pointer to that string.  the string is valid     *
 *               until the next call to this function.                      *
 *                                                                          *
 ****************************************************************************/
LPSTR Type2String(
	WORD wType,
	WORD fsOptions)
	{
  static char sz[30];
  static char o2s[] = "^!#$X*<?";
  static char *t2s[] = {
        ""                 ,
        "AdvData"          ,
        "AdvReq"           ,
        "AdvStart"         ,
        "AdvStop"          ,
        "Execute"          ,
        "Connect"          ,
        "ConnectConfirm"   ,
        "XactComplete"    ,
        "Poke"             ,
        "Register"         ,
        "Request"          ,
        "Term"             ,
        "Unregister"       ,
        "WildConnect"      ,
        ""                 ,
    };
  WORD bit, c, i;

  strcpy(sz, t2s[((wType & XTYP_MASK) >> XTYP_SHIFT)]);
  c = strlen(sz);
  sz[c++] = ' ';
  for (i = 0, bit = 1; i < 7; bit = bit << 1, i++) {
  	if (fsOptions & bit)
    	sz[c++] = o2s[i];
    }
  sz[c] = '\0';
  return(sz);
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ProcessTransaction()                                       *
 *                                                                          *
 *  PURPOSE    : Processes synchronous transactions entirely and starts     *
 *               async transactions.  Transaction attempts result in a      *
 *               transaction window being created which displays the state  *
 *               or results of the transaction.  (the callback function     *
 *               updates these windows as it gets calls) Transaction        *
 *               windows stay around until abandoned by the user or until   *
 *               the conversation is disconnected.  Advise Data and Advise  *
 *               Stop transactions are special.  We don't create a new      *
 *               window if the associated advise start transaction window   *
 *               can be found.                                              *
 *                                                                          *
 *  RETURNS    : TRUE  - If successful.                                     *
 *               FALSE - otherwise.
 *  	
	1. XTYP_ADVSTART
	Begins an advise loop. Any number of distinct advise loops
  can exist within a conversation. An application can alter the 
  advise loop type by combining the XTYP_ADVSTART 
  transaction type with one or more of the following flags: 

  Value	Meaning

  XTYPF_NODATA	Instructs the server to notify the 
                client of any data changes without 
                actually sending the data. This 
                flag gives the  client the option of 
                ignoring the notification or 
                requesting the changed data from 
								the server.
  XTYPF_ACKREQ	Instructs the server to wait until the 
                client acknowledges that it 
                received the previous data item 
                before sending the next data item. 
                This flag prevents a fast server 
                from sending data faster than the 
                client can process it. 

	2. XTYP_ADVSTOP	Ends an advise loop. 
	3. XTYP_EXECUTE	Begins an execute transaction. 
	4. XTYP_POKE	Begins a poke transaction. 
	5. XTYP_REQUEST	Begins a request transaction.


 *                                                                          *
 ****************************************************************************/
BOOL DDEC::ProcessTransaction()
  {
	CONVINFO ci;
	BOOL rVal = FALSE;
	DWORD cbData = 0;

	ci.cb = sizeof(CONVINFO);
	DdeQueryConvInfo(convInfo.hConv, (DWORD)QID_SYNC, &ci); // ci.hUser==hConv

	if ((pxact->wType != XTYP_EXECUTE) && (pxact->wType != XTYP_POKE))
		{
		pxact->hDdeData = NULL;
		cbData = 0;
		}
	else
  	cbData = -1;
		 
	/*
	* start transaction with DDEML here
	*/
	pxact->ret = (long)DdeClientTransaction((LPBYTE)pxact->hDdeData, (DWORD)cbData,
						convInfo.hConv, pxact->hszItem, pxact->wFmt,
            pxact->wType,
            pxact->ulTimeout, (LPDWORD)&pxact->Result);

	return rVal;
	}







/****************************************************************************
 *                                                                          *
 *  FUNCTION   : CompleteTransaction()                                      *
 *                                                                          *
 *  PURPOSE    : This handles completed synchronous and asynchronous        *
 *               transactions as well as failed attempted transactions.     *
 *                                                                          *
 *  RETURNS    : TRUE  - If successful.                                     *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
 LPSTR DDEC::CompleteTransaction()
 	{
	LPSTR psz = NULL;

  if (pxact->ret) {
  	/*
    * Successful transaction case
		*/
		if (pxact->wType == XTYP_REQUEST)
			{
      /*
      * Show resulting data
			*/

			// GetTextData((HDDEDATA)pxact->ret, textID);
			psz = NULL;
			// MyFree(psz);
      // Free this string at the receiving end

			/*
      * free returned data since it is displayed.
      */
			DdeFreeDataHandle((HDDEDATA)pxact->ret);

			}
		}
  else
		{
    /*
    * failed - show error result.
		*/
		// SendMessage(hwndInfoCtrl, ICM_SETSTRING, ICSID_LL,
		//      (DWORD)(LPSTR)Error2String(DdeGetLastError(idInst)));

		}
	pxact->fsOptions |= XOPT_COMPLETED;
	return psz;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : AddServiceList();
 *                                                                          *
 *  PURPOSE    : This method will be called from the XTYP_REGISTER handler
 *							 to build a list of all the services currently registered
 *							 DDEML lib. This list is later used during the OBJD dlg
 *                                                                          *
 *  RETURNS    : 
 *               
 *                                                                          *
 ****************************************************************************/
VOID DDEC::AddToServiceList(HSZ hszService,
	HSZ hszInstService )
	{
	if (!serviceListID)
		return;
    
	LPSTR service = GetHSZName(hszService);
	LPSTR instService = GetHSZName(hszInstService);


	int oldSize = (int)AMemGetSize(serviceListID);
	int index = oldSize / sizeof(SERVICELIST);
	AMemSetSize(serviceListID, oldSize + sizeof(SERVICELIST));
	LPSERVICELIST serviceListArr = (LPSERVICELIST)AMemLock(serviceListID);

	strcpy(serviceListArr[index].serverName,service );
	strcat(serviceListArr[index].serverName, ".exe");
	strcpy(serviceListArr[index].serviceName, service);
	strcpy(serviceListArr[index].instServiceName,instService);

	AMemUnlock(serviceListID);
	MyFree(service);
	MyFree(instService);
	


	}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : AddServiceList();
 *                                                                          *
 *  PURPOSE    : This method will be called from the XTYP_REGISTER handler
 *							 to build a list of all the services currently registered
 *							 DDEML lib. This list is later used during the OBJD dlg
 *                                                                          *
 *  RETURNS    : 
 *               
 *                                                                          *
 ****************************************************************************/
int DDEC::BuildServiceList(HWND hWnd)
	{
	CONVINFO ci;
	HCONVLIST hConvList;
	HCONV    hConv = 0;
	int count = 0;
	if (!(hConvList = DdeConnectList(idInst, NULL,NULL, NULL, NULL))) {  
		MPError(hWnd, MB_OK, IDS_DDEMLERR, (LPSTR)Error2String(DdeGetLastError(idInst)));
		return 0;
    }
	while ((hConv = DdeQueryNextServer((HCONVLIST)hConvList, hConv)) != 0) {
		DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, &ci);
		AddToServiceList(ci.hszSvcPartner, ci.hszTopic);
		count++;
		}
	DdeDisconnectList(hConvList);
	return count;
	}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : RemoveServiceList();
 *                                                                          *
 *  PURPOSE    : This method will be called from the XTYP_UNREGISTER handler
 *							 to remove from list of all the services currently registered
 *							 DDEML lib. This list is used during the OBJD dlg
 *                                                                          *
 *  RETURNS    : 
 *               
 *                                                                          *
 ****************************************************************************/
VOID DDEC::RemoveFromServiceList(HSZ hszService,
	HSZ hszInstService )
	{
	int i;
	int removeIndex;

	LPSTR service = GetHSZName(hszService);
	LPSTR instService = GetHSZName(hszInstService);
	int oldSize = (int)AMemGetSize(serviceListID);
	int index = oldSize / sizeof(SERVICELIST);
	LPSERVICELIST serviceListArr = (LPSERVICELIST)AMemLock(serviceListID);

	if (serviceListArr == NULL)
    return;
	for (i = 0; i < index; i++)
		{

		if ( (strcmp(serviceListArr[i].serviceName, service) == 0) &&
			 (strcmp(serviceListArr[i].instServiceName,instService) == 0))
			 removeIndex = i;
		}


	// remove the strings and move rest of the items up
	for (i = removeIndex; i < index - 1;i++)
		{
		strcpy(serviceListArr[i].serverName,serviceListArr[i + 1].serverName);
		strcpy(serviceListArr[i].serviceName, serviceListArr[i + 1].serviceName);
		strcpy(serviceListArr[i].instServiceName,serviceListArr[i + 1].instServiceName);
		}
	AMemSetSize(serviceListID, oldSize - sizeof(SERVICELIST));
	AMemUnlock(serviceListID);
	MyFree(service);
	MyFree(instService);
	}




/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DdeCallback()                                              *
 *                                                                          *
 *  PURPOSE    : This handles all callbacks from the DDEML.  This handles   *
 *               updating of the associated conversation and any special    *
 *               testing cases such as blocking callbacks etc.              *
 *                                                                          *
 *               For the most part, clients only handle advise data and     *
 *               asynchronous transaction completion here.                  *
 *                                                                          *
 *  RETURNS    : Results vary depending on transaction type.                *
 *                                                                          *
 ****************************************************************************/
extern "C" HDDEDATA EXPENTRY _export DdeCallback(
  WORD wType,
	WORD wFmt,
	HCONV hConv,
	HSZ hsz1,
	HSZ hsz2,
	HDDEDATA hData,
	DWORD lData1,
	DWORD lData2)
	{
	CONVINFO ci;
	DDEC *ddec;

	if (hConv)
	  {
    /*
    * update conversation status if it changed.
    */
    MYCONVINFO *pmci;

    ci.cb = sizeof(CONVINFO);
		if (!DdeQueryConvInfo(hConv,(DWORD) QID_SYNC, &ci) || (!ci.hUser)) {
    	/*
      * This conversation does not yet have a corresponding MDI window
      * or is disconnected.
      */
      return 0;
			}
    ddec = (DDEC*)ci.hUser;

		if ((pmci = (MYCONVINFO *)&(ddec->convInfo))  != NULL){
		  if (pmci->ci.wStatus != ci.wStatus ||
        pmci->ci.wConvst != ci.wConvst ||
        pmci->ci.wLastError != ci.wLastError) {
        /*
        * Things have changed, updated the conversation window.
        */
				;
        }
      if (ci.wConvst & ST_INLIST) {
        /*
        * update the associated list window (if any) as well.
				*/
        ;
			 	}
      }
    }

    /*
     * Now we begin sort out what to do.
     */
  switch (wType) {
		case XTYP_REGISTER:
				/*
         * This is where the client would insert code to keep track of
         * what servers are available.  This could cause the initiation
         * of some conversations.
				 */
         {

				 DDEC::AddToServiceList(hsz1, hsz2);

				 // Send a message to the dialog box callback to set the
				 // servicelist in the Services combo-Box
				 // This should be used only during OBJD
				 if (DDEC::hConvWnd)
					 {
					 // wParam = TRUE;
					 SendMessage(DDEC::hConvWnd, UM_SETSERVICELIST, 1,0);
				   }
				 }
				break;
		case XTYP_UNREGISTER:
				 {

         
				 DDEC::RemoveFromServiceList(hsz1, hsz2);
				 // Send a message to the dialog box callback to set the
				 // servicelist in the Services combo-Box
				 // This should be used only during OBJD
				 if (DDEC::hConvWnd)
					 {
           // wparam = FALSE
					 SendMessage(DDEC::hConvWnd, UM_SETSERVICELIST, 0,0);
				   }


         }

		case XTYP_DISCONNECT:
				{
				/*
				if (ddec->fAutoReconnect)
					{
					//
        	// attempt a reconnection
					//
					if ((hConv = DdeReconnect(hConv)) != 0)
						{
						DDEC::AddConv(ci.hszServiceReq, ci.hszTopic, hConv, FALSE);
          	return 0;
						}
					}
				*/

				/*
				* update conv window to show its new state.
      	*/
      	SendMessage((HWND)ci.hUser, UM_DISCONNECTED, 0, 0);
      	return 0;
      	}

		case XTYP_ADVDATA:
				/*
				 * data from an active advise loop (from a server)
				 */
				long sigID;
				if (hData)
					sigID =  SIGDDEC_ADVDATACHANGED;
				else
					sigID =  SIGDDEC_REQDATACHANGED;
				if (ddec->sigQ) {
					ddec->sigQ->AddToQ(hsz2, wFmt, sigID, hData);
					// post a polling event for every time advdata mesg. is received
					AEvtPostStandard(ddec->ddeCID, ddec_PollSigQ, 0, AEVT_ATTAIL);
				  return((HDDEDATA)DDE_FACK);
					}
				return (HDDEDATA)DDE_FNOTPROCESSED;

		case XTYP_XACT_COMPLETE:
        {
        /*
         * An asynchronous transaction has completed.  Show the results.
         *
         * ...unless the XOPT_BLOCKRESULT is chosen.
         */

        ci.cb = sizeof(CONVINFO);
				if (DdeQueryConvInfo(hConv, lData1, &ci))
						if (ddec->pxact->fsOptions & XOPT_BLOCKRESULT)
						  {
              ddec->pxact->fsOptions &= ~XOPT_BLOCKRESULT;
							return (HDDEDATA)(CBR_BLOCK);
							}      

        ddec->pxact->Result = lData2;
				ddec->pxact->ret = (LONG)hData;
				DdeFreeDataHandle((HDDEDATA)ddec->pxact->ret);
				// ddec->CompleteTransaction();
        }
        break;
		}
	return (HDDEDATA)0;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetTextData()                                              *
 *                                                                          *
 *  PURPOSE    : Allocates and returns a pointer to the data contained in   *
 *               hData.  This assumes that hData points to text data and    *
 *               will properly handle huge text data by leaving out the     *
 *               middle of the string and placing the size of the string    *
 *               into this string portion.                                  *
 *                                                                          *
 *  RETURNS    : A pointer to the allocated string.                         *
 *                                                                          *
 ****************************************************************************/
 int GetTextData(
	HDDEDATA hData,
	MEMBLOCKID txtID)
	{
	#define CBBUF  1024

	LPSTR psz;
	DWORD cb = CBBUF;


  if (hData == NULL) {
    return 0;
    }

	// SKJ, why is this crashing July 28, 1993
	// Try putting a static limit to the TEXT
	cb = DdeGetData(hData, NULL, 0, 0);
		
	if (!hData || !cb)
    return NULL;
	cb++;
	AMemSetSize(txtID, cb);
	psz = (LPSTR)AMemLock(txtID);
	DdeGetData(hData, (LPBYTE)psz, cb, 0L);

	psz[(int)cb] = (char)'\0';
	AMemUnlock(txtID);

	#undef CBBUF
	return 1;
	
	}
/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetGData()                                                 *
 *                                                                          *
 *  PURPOSE    : Allocates and returns a pointer to the data contained in   *
 *               hData.  																									  *
 *  RETURNS    : A MEMBLOCKID pointing to data 		                          *
 *                                                                          *
 ****************************************************************************/
 int GetGData(
	HDDEDATA 		hData,
	MEMBLOCKID 	dataMemID,
	int 				wFmt)
	{

	LPSTR psz;
	DWORD cb = 1024;


  if (hData == NULL) {
    return 0;
    }

	if (wFmt == CF_BITMAP)
		{
    HBITMAP hBitmap;
		DdeGetData(hData, (LPBYTE)&hBitmap, sizeof(HBITMAP), 0L);
		// if it is a bitmap then create a new copy of
		// the Bitmap handle because DDEML will free this on returning
  	// from DDE Client CALLBACK
		// Copies all the BITMAP related info and stuffs in
		// the memblock ID --> a DIB bitmap
		GetClipboardBMP((HBITMAP)hBitmap, dataMemID);
		}
  else {
		cb = DdeGetData(hData, NULL, 0, 0);

		if (!hData || !cb)
    	return NULL;

		AMemSetSize(dataMemID, cb);
		psz = (LPSTR)AMemLock(dataMemID);
		DdeGetData(hData, (LPBYTE)psz, cb, 0L);
		AMemUnlock(dataMemID);
		}
	return 1;
	
	}


/*************************************************************************
*	Function: SetALMData
*	This method is called from dde callback and GetData function after a
*	succesful transaction has been completed.  The data received by the client
*	application is passed in which is then set to a SERIUS (ALM) object
*	OBJECTS THAT CURRENTLY SUPPORT DATA IMPORT/EXPORT are:
*			1. Text
*			2. Picture (in BMP format)
*			3. All objects (in ALM format)
*************************************************************************/
int DDEC::SetALMData(XACT * lpXact, HDDEDATA hData, BOOL isHandle , BOOL bDestroyH)
	{
	long dataSize = 0;
	int rVal;
	HANDLE hPData;
	LPSTR pData;
	MEMBLOCKID memID;
	/*
   Show incoming data on corresponding transaction window.
	*/

	if (isHandle)
		{
		
		if (lpXact->wFmt == CF_BITMAP)
			dataSize = sizeof(HBITMAP); // in case of a BITMAP , SKJ Nov.9,1993
    else
			dataSize  = DdeGetData(hData, NULL, 0, 0);
	
		
		hPData = (HANDLE) GlobalAlloc(GHND, (int)dataSize);
		pData = (LPSTR)GlobalLock(hPData);
		DdeGetData(hData, pData, dataSize, 0L);
		}
	else {
		dataSize = AMemGetSize((MEMBLOCKID)hData);
		pData = (LPSTR)AMemLock((MEMBLOCKID)hData);
		// pData = (LPSTR)hData;
    }
	if (dataSize == 0)
		return DDEC_FAILURE;

	switch (lpXact->wFmt)
		{
		case CF_TEXT:
			{
			// append a null char
			//////////////////////////////////////////////
	  	// SKJ , this will set the current data to a TEXT OBJECT, June 16, 1993
			/////////////////////////////////////////////
			MEMBLOCKID textID = AMemAllocate(dataSize + 1);
			LPSTR textStr = (LPSTR)AMemLock(textID);
			_fmemcpy(textStr, (LPSTR)pData, (int)dataSize);
			// lstrcpy(textStr, pData);
			textStr[(int)dataSize] = (char)'\0'; // null terminate the string
			
			if (AObjConvertFromText(lpXact->receiverOID, textID))
				rVal = ALM_IMPORTSUCCESS;
			else
				rVal = ALM_ERRTEXTIMPORT;
			AMemUnlock(textID);
			AMemFree(textID);
 			}
			break;
		case CF_BITMAP:
			{
			AEvtInfo tEvent;
			memID = AMemAllocate(dataSize);
			if (isHandle) {
				LPSTR pictStr;
				GetClipboardBMP((HBITMAP)*pData, memID);
				pictStr = (LPSTR)AMemLock(memID);
				hmemcpy(pictStr, pData, AMemGetSize(memID));
				AMemUnlock(memID);
        }
      else {
				LPSTR pictStr = (LPSTR)AMemLock(memID);
				_fmemcpy(pictStr, pData, (int)dataSize);
        }
			// Get the bitmap data from clipboard
			// Send the received Bitmap to another picture object
			// SKJ -> what is the format of data being received
			// Why am I getting a data block of 28 bytes ?
			tEvent.sourceObject = lpXact->receiverOID;
			tEvent.targetObject = lpXact->receiverOID;
		  tEvent.what = OPICT_EVT_BITMAPTOOBJECT;
		  tEvent.when = 0; //Tickcount;
		  tEvent.how = AEVT_ATONCE | AEVT_DIRECT;
			tEvent.details = (long)memID;
			tEvent.modifiers =  0;

			if (AEvtPost(&tEvent))
        {
				rVal = ALM_IMPORTSUCCESS;
				}
			else
				rVal = ALM_ERRBMPIMPORT;
			AMemUnlock(memID);
			AMemFree(memID);

			}
			break;
		case CF_METAFILEPICT:
			rVal = ALM_ERRMETAIMPORT;

			break;
		default:  // cf_ALMOBJ
			rVal = ALM_ERRALMIMPORT;

			break;
		}


	if (isHandle)
    {
	  GlobalUnlock(hPData);
		GlobalFree(hPData);
		if (bDestroyH)
			DdeFreeDataHandle(hData);

		}
  else
		AMemUnlock((MEMBLOCKID)hData);
	return rVal;
	}




/****************************************************************************
 *                                                                          *
 *  FUNCTION   : MyGetClipboardFormatName()                                 *
 *                                                                          *
 *  PURPOSE    : Properly retrieves the string associated with a clipboard  *
 *               format.  If the format does not have a string associated   *
 *               with it, the string #dddd is returned.                     *
 *                                                                          *
 *  RETURNS    : The number of characters copied into lpstr or 0 on error.  *
 *                                                                          *
 ****************************************************************************/
int MyGetClipboardFormatName(
	WORD fmt,
	LPSTR lpstr,
	int cbMax)
	{
  if (fmt < 0xc000) {
  	// predefined or integer format - just get the atom string
    // wierdly enough, GetClipboardFormatName() doesn't support this.
    return(GlobalGetAtomName(fmt, lpstr, cbMax));
		}
	else {
  	return(GetClipboardFormatName(fmt, lpstr, cbMax));
  	}
	}





/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetFormatName()                                            *
 *                                                                          *
 *  PURPOSE    : allocates and returns a pointer to a string representing   *
 *               a format.  Use MyFree() to free this string.               *
 *                                                                          *
 *  RETURNS    : The number of characters copied into lpstr or 0 on error.  *
 *                                                                          *
 ****************************************************************************/
LPSTR GetFormatName(
WORD wFmt)
	{
  LPSTR psz;
	psz = MyAlloc(20);
  if (wFmt == CF_TEXT) {
			strcpy(psz, "CF_TEXT");
      return psz;
			}
	else if (wFmt == CF_BITMAP)
			{
			strcpy(psz, "CF_BITMAP");
      return psz;
			}
	else if (wFmt == CF_METAFILEPICT)
			{
			strcpy(psz, "CF_METAFILEPICT");
      return psz;
			}
	else
			{
			strcpy(psz, "CF_ALMOBJ");
      return psz;
      }
	}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : Disconnect()                                             *
 *                                                                          *
 *  PURPOSE    : Disconnects the given conversation after updating the      *
 *               associated conversation window.                            *
 *                                                                          *
 *  RETURNS    : TRUE on success, FALSE on failure.                         *
 *                                                                          *
 ****************************************************************************/
BOOL DDEC::Disconnect()
  {

	if (convInfo.hConv)
		convInfo.hConv = (HCONV)DdeDisconnect(convInfo.hConv);


	return convInfo.hConv ? TRUE:FALSE;
  }


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : AddConv()                                                  *
 *                                                                          *
 *                                                                          *
 ****************************************************************************/
HANDLE FAR PASCAL DDEC::AddConv(
	HSZ hszApp,     // these parameters MUST match the MYCONVINFO struct.
  HSZ hszTopic,
	HCONV hConv,
	BOOL fList)
	{
	if (fList) {
	  CONVINFO ci;
    HCONV hConvChild = 0;

    ci.cb = sizeof(CONVINFO);
		while ((hConvChild = DdeQueryNextServer((HCONVLIST)hConv, hConvChild)) != NULL)
		  {
	    if (DdeQueryConvInfo(hConvChild, (DWORD)QID_SYNC, &ci)) {
        AddConv(ci.hszSvcPartner, ci.hszTopic, hConvChild, FALSE);
        }
      }
    }
	I_UNUSED(hszApp);
	I_UNUSED(hszTopic);
	return 0;
	}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetConvListText()                                          *
 *                                                                          *
 *  RETURN     : Returns a ponter to a string containing a list of          *
 *               conversations contained in the given hConvList freeable    *
 *               by MyFree();                                               *
 *                                                                          *
 ****************************************************************************/
LPSTR DDEC::GetConvListText(
  HCONVLIST hConvList)
  {
  HCONV hConv = 0;
  WORD cConv = 0;
  CONVINFO ci;
  WORD cb = 0;
	LPSTR psz, pszStart;

  ci.cb = sizeof(CONVINFO);

  // find out size needed.

	while ((hConv = DdeQueryNextServer(hConvList, hConv)) != NULL)
		{
		if (DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, &ci))
			{
			if (!IsWindow((HWND)ci.hUser))
				{
				if (ci.wStatus & ST_CONNECTED)
					{
        	/*
          * This conversation doesn't have a corresponding
          * MDI window.  This is probably due to a reconnection.
          */
        	AddConv(ci.hszSvcPartner, ci.hszTopic, hConv, FALSE);
					} 
				else
					{
          continue;   // skip this guy - he was closed locally.
          }
        }
      cb += GetWindowTextLength((HWND)ci.hUser);
      if (cConv++)
      	cb += 2;        // room for CRLF
      }
    }

  cb++;                       // for terminator.
  // allocate and fill

	if ((pszStart = MyAlloc(cb)) != NULL)
		{
		psz = pszStart;
		*psz = '\0';
    hConv = 0;
		while ((hConv = DdeQueryNextServer(hConvList, hConv)) != NULL)
			{
	  	if (DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, &ci) &&
      	IsWindow((HWND)ci.hUser)) {
        psz += GetWindowText((HWND)ci.hUser, psz, cb);
        if (--cConv) {
        	*psz++ = '\r';
          *psz++ = '\n';
          }
        }
      }
    }
  return(pszStart);
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetConvInfoText()                                          *
 *                                                                          *
 *  PURPOSE    : Returns a pointer to a string that reflects a              *
 *               conversation's information.  Freeable by MyFree();         *
 *                                                                          *
 ****************************************************************************/
LPSTR DDEC::GetConvInfoText(
	HCONV 	 hConv,
	CONVINFO *pci)
	{
  LPSTR psz;
	LPSTR szApp;
	DWORD idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);
  psz = MyAlloc(300);
  pci->cb = sizeof(CONVINFO);
  if (hConv) {
		if (!DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, (PCONVINFO)pci)) {
            strcpy(psz, "State=Disconnected");
            return(psz);
    }
  	szApp = GetHSZName(pci->hszServiceReq);
				
		wsprintf(psz,
  			"hUser=0x%lx\r\nhConvPartner=0x%lx\r\nhszServiceReq=%s\r\nStatus=%s\r\nState=%s\r\nLastError=%s",
        pci->hUser, pci->hConvPartner, (LPSTR)szApp,
        (LPSTR)Status2String(pci->wStatus),
        (LPSTR)State2String(pci->wConvst),
				(LPSTR)Error2String(pci->wLastError));
		MyFree(szApp);
		}	
	else {
		strcpy(psz, Error2String(DdeGetLastError(idInst)));
		}
  return(psz);
	}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetConvTitleText()                                         *
 *                                                                          *
 *  PURPOSE    : Creates standard window title text based on parameters.    *
 *                                                                          *
 *  RETURNS    : psz freeable by MyFree()                                   *
 *                                                                          *
 ****************************************************************************/
LPSTR DDEC::GetConvTitleText(
	HCONV hConv,
	HSZ hszApp,
	HSZ hszTopic,
	BOOL fList)
	{
  WORD cb;
	LPSTR psz;
	DWORD idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);

  cb = (WORD)DdeQueryString(idInst, hszApp, NULL, 0, 0) +
  			(WORD)DdeQueryString(idInst, hszTopic, (LPSTR)NULL, 0, 0) +
        (fList ? 30 : 20);

	if ((psz = MyAlloc(cb)) != NULL)
		{
  	DdeQueryString(idInst, hszApp, psz, cb, 0);
    strcat(psz, "|");
    DdeQueryString(idInst, hszTopic, &psz[strlen(psz)], cb, 0);
    if (fList)
    	strcat(psz, " - LIST");
      wsprintf(&psz[strlen(psz)], " - (%lx)", hConv);
		}

	return(psz);
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DDEC()							                                        *
 *                                                                          *
 *  PURPOSE    : CONSTRUCTOR																								* 		
 *                                                                          *
 *  RETURNS    :																														* 
 *                                                                          *
 ****************************************************************************/
DDEC::DDEC(OBJECTID objectID, BOOL bOBJD)
	{
	LPDDECDATA    lpData = (LPDDECDATA)AObjLockData(objectID, data0_DDECDATA);
	LPDDECOPTIONS lpOptions= (LPDDECOPTIONS)AObjLockData(objectID, data1_DDECOPTIONS);


	// if it is called from OBJD routines, do no init the field till
	// the controls are all built up and initialised, SKJ July 21, 1993
	if (!bOBJD)
    {
	  lstrcpy(convInfo.ServerName, lpData->ServerName);
	  lstrcpy(convInfo.Service, lpData->Service);
	  lstrcpy(convInfo.Topic, lpData->Topic);
	  lstrcpy(defItem, lpData->Item);
	  lstrcpy(defFormat, lpData->Format);
	  defLinkType = lpData->linkType;
	  defReceiverOID = lpData->objectID;
	  defObjectType = lpData->ObjectType;

	  timeout = lpOptions->timeout;
		defAdvDelay = lpOptions->advDelay;
		bDefAck     = lpOptions->bAck;

	  _fmemcpy(&(CCFilter), &(lpOptions->CCFilter), sizeof(CONVCONTEXT));
	  strcpy(Country, lpOptions->Country);
   	strcpy(Language, lpOptions->Language);
   	strcpy(CodePage, lpOptions->CodePage);
	  strcpy(defTypeName, lpData->typeName);
		strcpy(defObjectName,lpData->objectName);

		sigQ = NULL;
		}
	else {
		_fmemset(this, 0, sizeof(DDEC)); // init all fields to 0
		}

	pxact = (XACT *)new XACT();
	_fmemset(pxact, 0, sizeof(XACT));


	ddeCID = objectID;
	// these fields are not saved in datas
	// are only valid for a given session
	convInfo.hConv = 0;
	numOfTransaction = 0;
	serverInst = 0;     
	pxact->lastUpdatedTime = 0;
	//holds a list of XACT structures for all the transactions
	xactListID = AMemAllocate(0); // holds a list of all conversations
	

	LPSERVICELIST serviceList = (LPSERVICELIST)AMemLock(serviceListID);
	strcpy(serviceList[0].serverName,USER_DEFNAME );
	strcat(serviceList[0].serverName, ".exe");
	strcpy(serviceList[0].serviceName, USER_DEFNAME);
	strcpy(serviceList[0].instServiceName,USER_DEFNAME);
	AMemUnlock(serviceListID);

	AObjUnlockData(objectID, data0_DDECDATA);
	AObjUnlockData(objectID, data1_DDECOPTIONS);
	idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ~DDEC()							                                      *
 *                                                                          *
 *  PURPOSE    : DESTRUCTOR																									* 		
 *                                                                         	*
 *  RETURNS    :																														* 
 *                                                                          *
 ****************************************************************************/
DDEC::~DDEC()
	{
	delete pxact;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : CreateConv()                                               *
 *                                                                          *
 *  PURPOSE    :                                                            *
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
HCONV DDEC::CreateConv(
	HSZ hszApp,
	HSZ hszTopic,
	BOOL fList,
	WORD *pError)
	{
  HCONV hConv;
  CONVINFO ci;

	CONVCONTEXT *cc = NULL;

	if (!bDefContext)
  	{
		cc = (CONVCONTEXT *)new char[sizeof(CONVCONTEXT)];
		_fmemcpy(cc, &CCFilter, sizeof(CONVCONTEXT));
		}
	else
		cc = NULL;
      
  if (fList) {
		hConv = (HCONV)DdeConnectList(idInst, hszApp, hszTopic, NULL, cc);
		}
	else {
		hConv = DdeConnect(idInst, hszApp, hszTopic, cc);
		}

	if (cc)
  	{
		delete cc;
		}

	if (hConv)
		{
		if (fList)
			{
      ci.hszSvcPartner = hszApp;
      ci.hszTopic = hszTopic;
			}
		else
			{
      ci.cb = sizeof(CONVINFO);
			DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, &ci);
			_fmemcpy(&CCFilter, &ci.ConvCtxt, sizeof(CONVCONTEXT));
			}
    AddConv(ci.hszSvcPartner, ci.hszTopic, hConv, fList);
    // HSZs get freed when window dies.
		}
	else
		{
		*pError = DdeGetLastError(idInst);
        
    DdeFreeStringHandle(idInst, hszApp);
    DdeFreeStringHandle(idInst, hszTopic);
		}
	return(hConv);
	}

/*******************************************************
* function: Delay
* Used during ADVDATA msg in the callback.  If there is
* a desired dalay in handling of ADVISE data, this function
* will check the clocktime and return TRUE if the last data update
* occured less then 'advDelay' millisecs ago.  The dde callback
* will ignore processing the ADVDATA if this function returns TRUE
*
********************************************************/
BOOL DDEC::Delay(XACT * lpxact)
  {
	long time = GetCurrentTime();
	if (time < lpxact->lastUpdatedTime + lpxact->advDelay)
		return TRUE;
	else {
		lpxact->lastUpdatedTime = time;
		return FALSE;
		}
  }

/********************************************************
Returns the XACT context of the current transaction
*********************************************************/
XACT *DDEC::GetTransactionContext(int trID)
	{

	XACT *listPtr = (XACT*)AMemLock(xactListID);


	if ((trID >= 0) &&(trID < numOfTransaction))
		{
		AMemUnlock(xactListID);
		return &(listPtr[trID]);

		}
	else {
		AMemUnlock(xactListID);
		return NULL;
		}
	}


/**************************************************************
* FUNCTION: EnumTransactions
* This function will traverse the list of all the current (active/inactive)
*	transactions and check to see if there is a matching item/format transaction
* if bHandle is true, the comparison is done with string handles else it
* is done with string pointers
*
*
***************************************************************/
XACT *DDEC::EnumTransactions(LPBYTE hszItem, BOOL bHandle, ATOM wFmt, int *index)
	{
	XACT *rVal = NULL;
	XACT *Xlist = (XACT *)AMemLock(xactListID);
	if (Xlist == NULL)
		return NULL;
	int i;
	if (*index == numOfTransaction)
		rVal = NULL;
	for (i = *index ; i < numOfTransaction; i++)
		{
		XACT *xact = &Xlist[i];

		if (bHandle)
			{
			HSZ itemH = (HSZ)hszItem;
			if ((DdeCmpStringHandles(xact->hszItem,itemH) == 0) && (xact->wFmt == wFmt))
        {
				*index = i + 1;
        rVal = xact;
			  // return xact;

				}
			}
		else {
			if ((lstrcmp((LPSTR)xact->Item, (LPSTR)hszItem) == (unsigned int)0) &&
					(xact->wFmt == (unsigned int)wFmt))
        {
			  *index = i + 1;
			  rVal = xact;

				}


		  } 
		}
	*index = numOfTransaction;

	AMemUnlock(xactListID);
	return rVal;
	}



/**************************************************************************
FUNCTION: StartConversation
DESC:			This function will start a conversation
					the return value is :
					Flow:             New Conversation established
														Conversation not established
														Conversation already exists
**************************************************************************/
int DDEC::StartConversation()
	{
	int rVal;
	WORD error;
	// To display a message box
	HWND hwnd = GetActiveWindow();
	HSZ  hszApp = DdeCreateStringHandle((DWORD)idInst,(LPSTR)convInfo.Service , (int)0);
	HSZ  hszTopic = DdeCreateStringHandle((DWORD)idInst,convInfo.Topic , (int)0);

	if (convInfo.hConv != 0)
		return DDEC_CONVEREXISTS;

	if (!(convInfo.hConv = CreateConv(hszApp, hszTopic,FALSE, &error)))
		{

		if (bDisplayErr)
			MPError(hwnd, MB_OK, IDS_DDEMLERR, (LPSTR)Error2String(error));
		DdeFreeStringHandle(idInst, hszApp);
		DdeFreeStringHandle(idInst, hszTopic);
		rVal = DDEC_CONVERNOTSTARTED;
		}
	else
		{
    // stuff the ddec ptr in the hUser field of ConvInfo struct 
		DdeSetUserHandle(convInfo.hConv, QID_SYNC, (LONG)this);
		rVal = DDEC_CONVERSTARTED;
		convInfo.status = DDEC_ENABLED;

		}
	
	return rVal;
	}



/***************************************************************************
*	Function:	StopConversation                                               *
*                                                                          *
*	DESC:		  This function will stop the conversation assoc. with this DDEC *
*						Object                                                         *
*                                                                          *
****************************************************************************/
int DDEC::StopConversation()
	{
	int rVal = DDEC_FAILURE;

	if (Disconnect())
    {
		(XACT *)AMemSetSize(xactListID, 0);
		numOfTransaction = 0;
		convInfo.hConv = 0;  // reset this
		rVal = DDEC_SUCCESS;
		}

	return rVal;
	}


/**************************************************************************
*Function:	StopAllConversation
*
*DESC:		  This function will stop all the conversation assoc. with this DDEC
						Object
*
**************************************************************************/
int DDEC::StopAllConversations()
	{
  DDEC* ddec;
	long i;
	OBJECTID theObject;
	while ((theObject = AObjGetNextObject(OTYPE_DDEC, &i)) != NULL)
		{
		ddec = (DDEC *)GetRunTimePtr(theObject, FALSE);
		ddec->Disconnect();
		ddec->convInfo.hConv = 0;
		(XACT *)AMemSetSize(ddec->xactListID, 0);
		}

	return 1;

	}


/***************************************************************************
* Function:  StartTransaction
* Description:
If a client requests an advise loop on a topic/item/format name set for which an advise loop is already
established, the DDEML does not create a duplicate advise loop. Instead, the DDEML alters the 
advise loop flags (XTYPF_ACKREQ and XTYPF_NODATA) to match the latest request.

If the server application specified the CBF_FAIL_ADVISES flag in the DdeInitialize function, this 
transaction is filtered.

****************************************************************************/
int DDEC::StartTransaction(
	LPSTR itemStr,
	LPSTR formatStr,
	LPSTR linkStr,
	long  delay,
	BOOL  bAck,
	OBJECTID oid
	)
  {
	char *lpRequestData = NULL;
	XACT *tmpPxact;
	int i = 0;
	int rVal;
	BOOL bOldTrans = FALSE;

	// init to def parameters first

	lstrcpy(pxact->Item, defItem) ;
	lstrcpy(pxact->Format, defFormat);
	pxact->linkType = defLinkType;
	pxact->fsOptions = DefOptions;
	pxact->ulTimeout = timeout; // DEFTIMEOUT;
	pxact->advDelay = defAdvDelay; // DEFDELAYTIME
	pxact->receiverOID = defReceiverOID;
	pxact->bAck = bAck;

	if ((itemStr) && (strcmp(itemStr, "")  != 0))
		{
		strcpy(pxact->Item, itemStr);
    }

	if (oid)
		{
		pxact->receiverOID = oid;

    }
    
	if ((formatStr)&& (strcmp(formatStr, "")  != 0))
		{
		strcpy(pxact->Format, formatStr);
    }

	if ((linkStr) && (strcmp(linkStr, "")  != 0))
		{
		pxact->linkType = GetLinkType(linkStr);
		}

	if (delay >= 0)
		{
		pxact->advDelay = delay;
		}
	pxact->lastUpdatedTime = GetCurrentTime(); // set the update marker

	pxact->wFmt = GetFormatID(pxact->Format); // CF_TEXT;

	if (pxact->linkType == IDMANUALLINK)
		pxact->wType = XTYP_REQUEST;
	else if (pxact->linkType == IDAUTOLINK)
		pxact->wType = XTYP_ADVSTART;
	else if (pxact->linkType == IDNOTIFYLINK)
		pxact->wType = XACT_NOTIFY; //XTYP_ADVSTART|XTYPF_NODATA

	// SKJ - I cannot '&' wType with XTYPE_ADVSTART because XTYP_REQUEST also
	// sets a common bit (0x0003 and 0x000B)
	if ((pxact->linkType == IDNOTIFYLINK) || (pxact->linkType == IDAUTOLINK))
		{
		if (pxact->bAck)
			pxact->wType |= XTYPF_ACKREQ;
		}

	pxact->hDdeData = 0;
	// now start the transaction.
	// Create a item str and free it only when the transaction is ended.
	// Start the transaction and store the info in a
	// struct somewhere.  If it is a non-Manual transaction then I need to map the transaction
	// to an ID that the RUNTIME class will store
	i = 0;  // start enum from the top of the list
	tmpPxact = EnumTransactions((LPBYTE)pxact->Item,FALSE, pxact->wFmt, &i);
  
	if ((tmpPxact == NULL) || (tmpPxact->status & DDETRANS_STOPPED))// no existing transaction of same type
		{
		if ((tmpPxact) && (tmpPxact->status & DDETRANS_STOPPED))
			{
			// if this transaction was previously stopped and
			// it is not being started again as 'COLD (MANUAL)' then REENABLE it
			// If it is being started again as 'COLD (MANUAL)' then do not
			// change the status bit on it.
		
			if (pxact->wType == XTYP_REQUEST)
        bOldTrans = FALSE;
			else
				bOldTrans = TRUE;
			
			}

		pxact->hszItem = DdeCreateStringHandle(idInst,pxact->Item, NULL);
	  ProcessTransaction();
		}
	else {
	  /*
			if this an advstart type transaction and a transaction of this type
			already exists, then just create a copy of XACT record.  If the
			receiver object of the new transaction is same as some old trnasaction
			then do not even create a new copy XACT record.  Just return the old
			copy
		*/

		pxact->hszItem =  tmpPxact->hszItem;
		do {

		  if (tmpPxact->receiverOID == pxact->receiverOID)
		    {
			  pxact->ret = 0;
				rVal = DDEC_FAILURE; // already exists
				break;     // break out of while loop
				}   
			} while ((tmpPxact = EnumTransactions((LPBYTE)pxact->Item,FALSE, pxact->wFmt, &i)) != NULL);

		if (tmpPxact == NULL)  // if no trans w/ identical oid was found
			{
		
			DdeKeepStringHandle(idInst, pxact->hszItem);
			pxact->ret = 1;
			}
			
    }

	if (pxact->ret)
	  {
		if (!bOldTrans) //if this is not an old STOPPED transaction
      {
	    if ((pxact->linkType == IDNOTIFYLINK) || (pxact->linkType == IDAUTOLINK))
		    {
				int size = (int)AMemGetSize(xactListID);
		 	 	XACT *listPxact = (XACT *)AMemSetSize(xactListID, size + sizeof(XACT));
		 		listPxact = (XACT *)AMemLock(xactListID);
	   		numOfTransaction +=1; // increment the transaction count;
		 		pxact->transID = numOfTransaction - 1; // transaction id is same as the
																						   // the number of transactions - 1
		 		pxact->status = DDETRANS_ENABLED;
				_fmemcpy(&listPxact[numOfTransaction - 1], pxact, sizeof(XACT));
				AMemUnlock(xactListID);

        } 	
			else if (pxact->wType == XTYP_REQUEST)
		 		{
		 		// XTYP_REQUEST
	   		/*
     		* Synchronous transactions are completed already so pass on to
     		* CompleteTransaction right away.
     		*/
     
				pxact->transID = ID_REQUESTTRANS; // transaction id is always -1
				SetALMData(pxact, (HDDEDATA)pxact->ret, TRUE, TRUE); // get almData and destroy data Handle
		 		// pxact->status = DDETRANS_COMPLETED;
		 		}
			} // end of new transaction book-keeping
		else { // if it is an old transaction that has been blocked
			tmpPxact->status = DDETRANS_ENABLED;
      }
			 
	  rVal = DDEC_SUCCESS;
		_fmemcpy(&lastActvdXact, pxact, sizeof(XACT));    // copy the last trans started
	  }
 else {
	 rVal = DDEC_FAILURE;
	 }

 if (lpRequestData)
	 MyFree(lpRequestData);
 return rVal;
 }


/*************************************************************************
* Function: StopTransaction
* Description:  This function will stop the passed in transaction
*
*
**************************************************************************/
int DDEC::StopTransaction(int transactionID)
	{
	int rVal = DDEC_FAILURE;
	int pxactCount = 0;
	XACT *lpXact;
	int index = 0;

	lpXact = (XACT *)GetTransactionContext(transactionID);

	if (lpXact == NULL)   {
		return DDEC_FAILURE;
		}

	// make a copy of the transaction to be stopped
	_fmemcpy(pxact, lpXact, sizeof(XACT));

	// if there is only one transaction of this item,format type then
	// STOP it.  Else only set this transaction's (ref:transactionID)
  // status to STOPPED
	while (EnumTransactions((LPBYTE)pxact->Item, FALSE, pxact->wFmt, &index) != NULL)
	  {
		pxactCount++;
		}

	if (pxactCount == 1)
		{
		pxact->wType = XTYP_ADVSTOP;
		ProcessTransaction();
		}
	else
		{
		pxact->ret = 1;
		}


	lpXact->status = DDETRANS_STOPPED;
  if (pxact->ret)
		{
		_fmemcpy(&lastAbrtdXact, lpXact, sizeof(XACT));
		// decrement the Item string-keep count
		DdeFreeStringHandle(idInst, lpXact->hszItem);
	  rVal = DDEC_SUCCESS;

		}

	return rVal;

	}



/*************************************************************************
* Function: ExecTransaction
* Description:  This function will start a XTYP_EXECUTE trans
*
*
**************************************************************************/
int DDEC::ExecTransaction(LPSTR commStr, LPSTR dataStr)
	{
	int rVal = DDEC_SUCCESS;
	int commLen = lstrlen(commStr);
	int dataLen = lstrlen(dataStr);
	LPSTR lpszOrigTopic = new char[strlen(convInfo.Topic) + 1];
	BOOL 	bNewConv = FALSE;

	strcpy(lpszOrigTopic, convInfo.Topic);

	// If there is no current conv. create a temp SYSTEM topic and start a conversation.
	// DDE standard --> All EXEC are sent under SYSTEM topic
	if (convInfo.hConv == 0)
    {
    strcpy(convInfo.Topic, "SYSTEM");
		rVal = StartConversation();
		if (rVal == DDEC_CONVERNOTSTARTED)
			{
			strcpy(convInfo.Topic, lpszOrigTopic);
			delete lpszOrigTopic;
			return DDEC_FAILURE;
			}
		else
    	bNewConv = TRUE;
		}

	
	LPSTR tmpStr = new char[commLen + dataLen + 2]; // commStr + '\t' + dataStr + '\0'

	pxact->hszItem = NULL; // is ignored for XTYP_EXECUTE msgs
	pxact->wFmt = CF_TEXT;
	pxact->wType = XTYP_EXECUTE;
	pxact->ulTimeout = timeout; // DEFTIMEOUT;
	// data handle to a Null terminated commnad string
	strcpy(tmpStr, commStr);

	if (lstrlen(dataStr) != 0)
    {
	  strcat(tmpStr,"\t");
		strcat(tmpStr, dataStr);
		}
	else { // if there is no data Str then add another \0
		// exec str = commStr\t\0
		strcat(tmpStr,"\t");
		// tmpStr[strlen(tmpStr)] = '\0';
		}
	pxact->hDdeData = DdeCreateDataHandle(idInst, tmpStr,lstrlen(tmpStr) + 1, 0, NULL, CF_TEXT, 0);
	delete tmpStr;

	ProcessTransaction();

	if (pxact->ret)
		{
		// DdeFreeStringHandle(idInst, pxact->hszItem);
		rVal = DDEC_SUCCESS;
		}

  // if a new conversation was started then stop it before exiting
  if (bNewConv)
		StopConversation();

		  
 	strcpy(convInfo.Topic, lpszOrigTopic);
	delete lpszOrigTopic;

	return rVal;
	}


/*************************************************************************
* Function: 		SendData
* Description:  This function will send a dataStr to the server under
*		            the current conversation and itemStr
*
*
**************************************************************************/

int DDEC::SendData(LPSTR itemStr, LPSTR dataStr)
	{
	int rVal = DDEC_FAILURE;
	if (itemStr)
		strcpy(pxact->Item, itemStr);
	else
		strcpy(pxact->Item, defItem);

	pxact->hszItem = DdeCreateStringHandle(idInst,pxact->Item, NULL);

	pxact->linkType = defLinkType;
	pxact->fsOptions = DefOptions;
	pxact->receiverOID = defReceiverOID;
	pxact->bAck = bDefAck;
	// pxact->wFmt = pxact->;
	pxact->wType = XTYP_POKE;
	pxact->ulTimeout = timeout;

	// data handle to a Null terminated commnad string
	pxact->hDdeData = DdeCreateDataHandle(idInst, dataStr, lstrlen(dataStr) + 1, 0, pxact->hszItem, CF_TEXT, 0);

	ProcessTransaction();

	DdeFreeStringHandle(idInst, pxact->hszItem);

	if (pxact->ret)
		{
	  rVal = DDEC_SUCCESS;
		}

	
	return rVal;
	}


/*************************************************************************
* Function:     IsServiceAvailable
* Description:  This function will check to see if a service is available
*
*
**************************************************************************/
int DDEC::IsServiceAvailable(LPSTR service)
	{
	HSZ hszApp = DdeCreateStringHandle((DWORD)idInst,service , 0);

	HCONV hConv;
	WORD  error;
	int rval = DDEC_FAILURE;
	hConv = CreateConv(hszApp, NULL,FALSE, &error);

	if (hConv)
		{
    DdeDisconnect(hConv);
		rval = DDEC_SUCCESS;
		}
	DdeFreeStringHandle(idInst, hszApp);
	return rval;
	}

/*************************************************************************
* Function:     GetData
* Description:  This function will get data assoc. with a warm link
*
*
**************************************************************************/
int DDEC::GetData(int transactionID)
	{
	int rVal = DDEC_FAILURE;

	HDDEDATA hData;
	XACT *tmpPxact =  (XACT *)GetTransactionContext(transactionID);

	if (tmpPxact == NULL)  
		return rVal;
	if ((tmpPxact->status & DDETRANS_STOPPED) || (tmpPxact->status & DDETRANS_BLOCKED))
		{
		return rVal;
		}
	// if the link type is WARM(NOTIFY) link then get Data
  // else return error
	if (tmpPxact->linkType == IDNOTIFYLINK)
		{
		/*
		* XTYPF_NODATA case - request the info.
		*  (we do this asynchronously - from the time that ADVDATA was received in the callback)
 	 	*/
		hData = DdeClientTransaction(NULL, 0L, convInfo.hConv, tmpPxact->hszItem, tmpPxact->wFmt,
						XTYP_REQUEST, timeout, NULL);

		if (hData)
	  	{
			if (SetALMData(tmpPxact, hData, TRUE, FALSE) == ALM_IMPORTSUCCESS) // assign data to SERIUS OBject
				rVal = DDEC_SUCCESS;
			else
				rVal = DDEC_FAILURE; // cannot set data to control

			DdeFreeDataHandle(hData);
			}
  	else
			rVal = DDEC_FAILURE; // no valid data
		}
	else
		rVal = DDEC_FAILURE;
	return rVal;
	}

///////////////////////////////////////////////////////////
// BITMAP FUNCTIONS
//////////////////////////////////////////////////////////
/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibNumColors(VOID FAR * lv)                                *
 *                                                                          *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : The number of colors in the DIB.                           *
 *                                                                          *
 ****************************************************************************/
WORD DibNumColors (VOID far *lv)
  {
  int                 bits;
  LPBITMAPINFOHEADER  lpbi;
  LPBITMAPCOREHEADER  lpbc;

	lpbi = ((LPBITMAPINFOHEADER)lv);
	lpbc = ((LPBITMAPCOREHEADER)lv);

  /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
  */
  if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
  	if (lpbi->biClrUsed != 0)
    	return (WORD)lpbi->biClrUsed;
		bits = lpbi->biBitCount * lpbi->biPlanes;
  	}
  else
		bits = lpbc->bcBitCount * lpbc->bcPlanes; ;

	switch (bits)
		{
  	case 1:
    	return 2;
    case 4:
  		return 16;
    case 8:
    	return 256;
    default:
    	/* A 24 bitcount DIB has no color table */
      return 0;
    }
	}

/****************************************************************************
 *									    																										*
 *  FUNCTION   :  PaletteSize(VOID FAR * lv)				    										*
 *									    																										*
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *		  is of the BITMAPCOREHEADER type, the number of colors is  					*
 *		  multiplied by 3 to give the palette size, otherwise the   					*
 *		  number of colors is multiplied by 4.																*       	
 *									    																										*
 *  RETURNS    :  Palette size in number of bytes.			    								*
 *									    																										*
 ****************************************************************************/
WORD PaletteSize (VOID far *lv)
	{
  LPBITMAPINFOHEADER lpbi;
  WORD	       NumColors;

	lpbi      = (LPBITMAPINFOHEADER)lv;
	NumColors = DibNumColors(lpbi);

  if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
  	return NumColors * sizeof(RGBTRIPLE);
  else
  	return NumColors * sizeof(RGBQUAD);
	}
	


////////////////////////////////////////////////////////
// Copy bmp from CLipboard
////////////////////////////////////////////////////////
void GetClipboardBMP(
	HBITMAP		 hbm,
	MEMBLOCKID memID)
	{
	BITMAP					bmap;
	BYTE huge *     bits;
	// LPSTR  					bits;
	HDC							hDC;
	long 						ImageSize;
	int 						tableSize;
	int 						numColors;
	LPBITMAPINFO    lpbmInfo;

	GetObject(hbm, sizeof(BITMAP), (LPSTR)&bmap);
	int nColorBits = bmap.bmPlanes * bmap.bmBitsPixel;
	numColors = (1 << nColorBits );
	ImageSize = (WIDTHBYTES((DWORD)bmap.bmWidth * nColorBits) * bmap.bmHeight);
	tableSize  = numColors * sizeof(RGBQUAD); //	DibNumColors (lpbi) * sizeof(RGBQUAD);
	lpbmInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO) + (int)tableSize];

	lpbmInfo->bmiHeader.biWidth = bmap.bmWidth;
	lpbmInfo->bmiHeader.biHeight = bmap.bmHeight;
	lpbmInfo->bmiHeader.biPlanes = 1;
	lpbmInfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	lpbmInfo->bmiHeader.biBitCount = bmap.bmPlanes * bmap.bmBitsPixel;
	lpbmInfo->bmiHeader.biCompression = BI_RGB;
	lpbmInfo->bmiHeader.biXPelsPerMeter = 0;
	lpbmInfo->bmiHeader.biYPelsPerMeter = 0;
	lpbmInfo->bmiHeader.biSizeImage  = 0;
	lpbmInfo->bmiHeader.biClrUsed    = 0; // numColors;
	lpbmInfo->bmiHeader.biClrImportant = 0; // numColors;;

	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
			
	GetDIBits(hDC, hbm, 0, bmap.bmHeight, (LPSTR)NULL, lpbmInfo, DIB_RGB_COLORS);

	if (lpbmInfo->bmiHeader.biSizeImage == 0) // SKJ, 3/15/93 Maintenance release
		{
		nColorBits = bmap.bmPlanes * bmap.bmBitsPixel;
		lpbmInfo->bmiHeader.biSizeImage = WIDTHBYTES((DWORD)bmap.bmWidth * nColorBits) * bmap.bmHeight;
		}

	if (lpbmInfo->bmiHeader.biClrUsed == 0)
		lpbmInfo->bmiHeader.biClrUsed = DibNumColors(&(lpbmInfo->bmiHeader));
	// SKJ, the planes info should be gotten from the prev. GetDIBits call
	// 3/15/93
	tableSize = PaletteSize(&(lpbmInfo->bmiHeader));
	ImageSize = lpbmInfo->bmiHeader.biSizeImage;
	AMemSetSize(memID, sizeof(BITMAPINFOHEADER) + tableSize+ ImageSize); // to allow for huge ptr
	bits = (BYTE huge *)AMemLock(memID);
	LPBITMAPINFOHEADER bmpInfoHdr = (LPBITMAPINFOHEADER )bits;
	RGBQUAD *rgbQuad = (RGBQUAD *)((LPBYTE)bmpInfoHdr + sizeof(BITMAPINFOHEADER));
	_fmemcpy((LPSTR)bmpInfoHdr, &(lpbmInfo->bmiHeader), sizeof(BITMAPINFOHEADER));
	_fmemcpy((LPSTR)rgbQuad, lpbmInfo->bmiColors, tableSize);
	bits = (BYTE huge *)((LPBYTE)bmpInfoHdr + sizeof(BITMAPINFOHEADER) + tableSize);
	GetDIBits(hDC, hbm, 0, bmap.bmHeight, (LPSTR)bits, lpbmInfo, DIB_RGB_COLORS);
	AMemUnlock(memID);

	DeleteDC(hDC);

	}


