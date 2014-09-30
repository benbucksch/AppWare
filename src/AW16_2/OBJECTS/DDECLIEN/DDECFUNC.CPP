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
// FILE: 						DDECFUNC.cpp
//
// AUTHOR:          Shaili Jain
//
// DESCRIPTION:     DDEC Appware Functions
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT


#include "ddemlcl.h"
#include "runtime.h"
#include "o_text.h"
#include "o_number.h"
#include "o_flag.h"


extern int GetLinkName(int linkType, LPSTR linkName);

/***************************************************************************
* DDEC FUNCTION:  StartServer
* Input: 					DDEC object
* Ouput:					N/A
* Flow:						Server Started
									Server not Started
									Server is already running
* Description:  This function will call WinExec() on the server application name
* that is connected to this client object.  If no server application name is
* available, this function will assume the server name to same as the service
* name and try to start it up.
****************************************************************************/


extern "C" void FAR PASCAL _export StartServer(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	// the return value is :
	//     1. DDEC_SERVERSTARTED = 1
	//     2. DDEC_SERNOTSTARTED = 2
	//     3. DDEC_SERVERALREADYSTARTED = 3

	DDEC *ddec =     (DDEC *)GetRunTimePtr(ddecID, FALSE);
	int rVal = 1;

	if (ddec->serverInst == 0)
    {
		rVal = ddec->RunServer(NULL);
		}
	else {
    rVal = 16; // can only start one inst. of the server per ddec object
    }
	if (rVal < 32)
	  {
		if (rVal == 16)
		  // Attempt was made to load a second instance of an executable file
			// containing multiple data segments that were not marked read-only.
		  rVal = DDEC_SERVERALREADYSTARTED;
		else
		  rVal = DDEC_SERVERNOTSTARTED;		
		}
	else
	  rVal = DDEC_SERVERSTARTED;
	system->message1 = rVal;
	
	I_UNUSED(theEvent);
	}


/***************************************************************************
* DDEC FUNCTION:			Stop Server:
* Input Parameters:  	DDE Client object
* Flow:    						Server stopped
* 										Cannot Stop Server
* Description:  Terminates the specific server (if it is running).  This function can only be called if a call
* to Start Server has been made previously.  If no server name is available, it will use the service name as 
* the server name.
****************************************************************************/
extern "C" void FAR PASCAL _export StopServer(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{

	int rVal = DDEC_FAILURE;
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, FALSE);

	if (ddec->serverInst)
		rVal = ddec->StopServer();

	system->message1 = rVal;
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:			Start Conversation:
* Input Parameters:  	DDE Client object
																	
*	Flow:               New Conversation established
										Conversation not established
										Conversation already exists
*	Description:
Establishes a conversation with the service:topic (as specified in the Client object).  
The default conversation context options (language, security, keyboard code page) are used.
If it fails it may be due to the following reasons:
          1.  No matching service currently registered with DDEML lib 
					2.  No matching topic is available from the service.
p.s. If this function is called more than once on a client object, it is a no-op.
Only one conversation can be established per serius dde client object.

****************************************************************************/
extern "C" void FAR PASCAL _export StartConversation(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	int rVal = 0;
	OBJECTID 	ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC 			*ddec 					=     (DDEC *)GetRunTimePtr(ddecID, 0);
	rVal = ddec->StartConversation();
  if (rVal == DDEC_SUCCESS)
		AEvtPostSignalAtMark(ddec->ddeCID, SIGDDEC_CONVERSSTARTED);
	system->message1 = rVal;
	I_UNUSED(theEvent);
	I_UNUSED(system);
	}


/***************************************************************************
* DDEC FUNCTION:		 Stop Conversation:
* Input Parameters:  DDE Client object
*	Flow:							 Stopped Conversation
*									 Cannot Stop Conversation
*	This function will disconnect this conversation (as identified by the DDE client object) if it has been 
*	previously established.
****************************************************************************/
extern "C" void FAR PASCAL _export StopConversation(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	int rval;
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC 			*ddec 					=     (DDEC *)GetRunTimePtr(ddecID, 0);


	/* the return value is :


	* Flow:    				 Stopped Conversation
									 Cannot Stop Conversation
	*/
	rval = ddec->StopConversation();
	if (rval == DDEC_SUCCESS)
		AEvtPostSignalAtMark(ddec->ddeCID, SIGDDEC_CONVERSSTOPPED);
	system->message1 = rval;
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:   	 Stop All Conversations:
* Flow:							 Stopped All Conversation
*										 Cannot Stop All Conversation
*	This function will disconnect all conversations that have been established by the
*	application.
****************************************************************************/
extern "C" void FAR PASCAL _export StopAllConversations(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{


	/* the return value is :


	* Flow:             Stopped All Conversation
									  Cannot Stop All Conversation

	*/
	DDEC::StopAllConversations();
	system->message1 =1 ;
	I_UNUSED(theEvent);
	}



/***************************************************************************
* DDEC FUNCTION:			Start Transaction (service:topic:Item)
* Input Parameters:  	A DDE client object.
										Item Name (optional)
										Serius object (optional)
										format name
										link type name
* Flow:								Transaction Started
										Cannot Start Transaction
*	Output:							Transaction ID
* Description: A transaction with the input Item name is established.  If no Item name is input, it will use 
* the default item name as specified in the DDE client object.  This  transaction will be attached to the 
* passed in serius object or use the default serius object. The format of the transaction
* will either be passed in or we will use the default format.  Similarly either the passed
* in linktype will be used or the default will be used.  The transaction will
* use the default timeout and advise data delay options (from DDE client object).
* If this transaction was of auto/notify type, this function will return an transaction identifier that can be 
* later used as input to other functions.  In a 'manual' type transaction this id will always be 0, and cannot 
* be further used.  The function can fail for the following reasons:
	1. Conversation not established
	2. Wrong Item name
	3. Wrong format 
	4. No Input Item name or object (and no default item/object name).
     
* p.s. If a client requests an advise loop on a topic/item/format name set for which an advise loop is already
* established, the DDEML does not create a duplicate advise loop. Instead, the DDEML alters the 
* advise loop flags (XTYPF_ACKREQ and XTYPF_NODATA) to match the latest request.
****************************************************************************/
extern "C" void FAR PASCAL _export StartTransaction(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	int rVal;
	DDEC *ddec;
	int 	signal;
	long transID = -1; // -1 is error transID
	long delay = 0;
	OBJECTID returnID;
	BOOL   bAck;
	LPSTR  itemStr = NULL;
	LPSTR  formatStr = NULL;
	LPSTR    linkTypeStr = NULL;
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	OBJECTID itemID = AFuncGetTypedParameter(2, OTYPE_TEXT);
	OBJECTID formatID = AFuncGetTypedParameter(3, OTYPE_TEXT);
	OBJECTID linkTypeID = AFuncGetTypedParameter(4, OTYPE_TEXT);
	OBJECTID delayID    = AFuncGetTypedParameter(5, OTYPE_NUMBER);
	OBJECTID bAckID = AFuncGetTypedParameter(6, OTYPE_FLAG);
	OBJECTID objectID = AFuncGetParameter(7);

	MEMBLOCKID imemID = AMemAllocate(0);
	MEMBLOCKID fmemID = AMemAllocate(0);
	MEMBLOCKID lmemID = AMemAllocate(0);
  
	ddec = (DDEC *)GetRunTimePtr(ddecID, FALSE);

	// if there is no conversation or if the conv. is suspended
	if ((!ddec->convInfo.hConv) || (ddec->convInfo.status & DDEC_SUSPENDED))
		{
		system->message1 = 2;
		return;
		}
	// usurp the default setting if valid parameters are passed
	if (itemID)
		{
		// itemStr;
		AObjConvertToText(itemID, imemID);
		itemStr = (LPSTR)AMemLock(imemID);
		}

	if (formatID)
		{
		
		AObjConvertToText(formatID, fmemID);
		formatStr = (LPSTR)AMemLock(fmemID);

		}

	if (linkTypeID)
		{
		AObjConvertToText(linkTypeID, lmemID);
		linkTypeStr = (LPSTR)AMemLock(lmemID);

		}

	if (delayID)
		{

		delay = ONmbrGetInteger(delayID);
		}
  
	if (bAckID)
		{
    // SKJ waiting for FLAG api
		bAck = OFlagGet(bAckID);
		}
	else
		bAck = ddec->bDefAck;

	rVal = ddec->StartTransaction(itemStr,formatStr, linkTypeStr, delay, bAck, objectID );

	if (rVal == DDEC_SUCCESS)
		{
		if (ddec->pxact->linkType == IDMANUALLINK)
		  signal = SIGDDEC_MANTRANSCOMPLETED;
		else
			signal = SIGDDEC_AUTOTRANSSTARTED;

		AEvtPostSignalAtMark(ddec->ddeCID, signal);
		transID = (long)ddec->pxact->transID;
			
		system->message1 = 1;
		}
	else
	  {   
		system->message1 = 2;
    }
    

	returnID = AObjCreate(OTYPE_NUMBER);
	ONmbrSetInteger(returnID, transID, TRUE);
	// ONmbrSetInteger(returnID, (long)transID);
	AFuncSetParameter(1, returnID);

	if (itemID)
		{
		AMemUnlock(imemID);
		}
	if (formatID)
		{
		AMemUnlock(fmemID);
		}
	if (linkTypeID)
		{
		AMemUnlock(lmemID);
		}


	AMemFree(imemID);
	AMemFree(fmemID);
	AMemFree(lmemID);
	I_UNUSED(theEvent);
	}



/***************************************************************************
* DDEC FUNCTION:			Stop Transaction
* Input Parameters:  	A DDE client object.
											Transaction ID
* Flow:								Transaction Stopped
											Cannot Stop Trnsaction
* Description: The transaction identified by the transactionID is stopped.  The
*	transaction being stopped should be of auto or notify type.
****************************************************************************/

extern "C" void FAR PASCAL _export StopTransaction(
  pAEvtInfo			theEvent,
	pAObjMessage	system)
	{
 	/* the return value is :
	* Flow:             Transaction Stopped
										Cannot Stop Transaction
	*/
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID,0);
	OBJECTID tID = AFuncGetParameter(2);
	long trID = (long int)ONmbrGetInteger(tID);

	/* the return value is :
	* * Flow:    						Server stopped = DDEC_SUCCESS
	* 										Cannot Stop Server = DDEC_FAILURE

	*/

	system->message1 = ddec->StopTransaction((int)trID);

	if (system->message1 == DDEC_SUCCESS)
		AEvtPostSignalAtMark(ddec->ddeCID, SIGDDEC_AUTOTRANSSTOPPED);
	I_UNUSED(theEvent);
	}


/***************************************************************************
* DDEC FUNCTION:			IsServiceAvailble
* Input Parameters:  	A DDE client object.
* Flow:               Service Is Available
									  	Service Not Available
*	Description: This function will check to see if the input dde client
						object's service is available
****************************************************************************/

extern "C" void FAR PASCAL _export IsServiceAvailable(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);

	system->message1 = ddec->IsServiceAvailable(ddec->convInfo.Service);
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:			GetData
* Input Parameters:  	A DDE client object.
							      	transactionID
* Flow:               After getting data
											Cannot get data
*	Description: 				This function will probe the server for data over warm
											link
**************************************************************************/

extern "C" void FAR PASCAL _export GetData(
  pAEvtInfo						theEvent,
	pAObjMessage 			 	system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec      =     (DDEC *)GetRunTimePtr(ddecID, 0);
	OBJECTID tID    = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	long trID;

  if (tID)
		trID = (long int)ONmbrGetInteger(tID);
	else
		trID = ddec->lastUpdtdXact.transID;

	system->message1 = ddec->GetData((int)trID);

	I_UNUSED(theEvent);
	}




/***************************************************************************
* DDEC FUNCTION:			SendData
* Input Parameters:  	A DDE client object.
											transactionID
											data String
* Flow:               After sending data
											Cannot send data
*	Description: 			 	This function will send data from client to server
**************************************************************************/

extern "C" void FAR PASCAL _export SendData(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	int rVal = DDEC_FAILURE;
	MEMBLOCKID imemID = AMemAllocate(0);
	MEMBLOCKID dmemID = AMemAllocate(0);

	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
  LPSTR itemStr;
	OBJECTID itemID = AFuncGetParameter(2);

	if (itemID)
    {
		AObjConvertToText(itemID, imemID);
		itemStr = (LPSTR)AMemLock(imemID);
		}
	else
		{
		AMemSetSize(imemID, lstrlen(ddec->defItem) + 1);
		itemStr = (LPSTR)AMemLock(imemID);
		lstrcpy(itemStr, ddec->defItem);
		}



	OBJECTID dataID = AFuncGetParameter(3);

	if (dataID)
    {
		AObjConvertToText(dataID, dmemID);
		}
	else { // use the default receiver Object
		AObjConvertToText(ddec->defReceiverOID, dmemID);

		}

	LPSTR    dataStr =  (LPSTR)AMemLock(dmemID);
  if (ddec->convInfo.hConv)
		rVal = ddec->SendData(itemStr, dataStr);

	system->message1 = rVal;
  if (imemID)
		AMemUnlock(imemID);
	if (dmemID)
		AMemUnlock(dmemID);

	AMemFree(imemID);
	AMemFree(dmemID);
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:			SuspendConversation
* Input Parameters:  	A DDE client object.
* Flow:               After suspending
											Cannot suspend
Description: 					This function will suspend this dde object's conversation
**************************************************************************/

extern "C" void FAR PASCAL _export SuspendConversation(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{

	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
	system->message1 = DDEC_FAILURE;

	if (DdeEnableCallback(ddec->idInst, ddec->convInfo.hConv, EC_DISABLE))
    {
		ddec->convInfo.status = DDEC_SUSPENDED;
		system->message1 = DDEC_SUCCESS;
		}
	I_UNUSED(theEvent);
	}



/***************************************************************************
* DDEC FUNCTION:			ResumeConversation
* Input Parameters:  	A DDE client object.
* Flow:               After resuming
											Cannot resume
Description: 					This function will resume this dde object's conversation
**************************************************************************/

extern "C" void FAR PASCAL _export ResumeConversation(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
	system->message1 = DDEC_FAILURE;


	if (DdeEnableCallback(ddec->idInst, ddec->convInfo.hConv, EC_ENABLEALL))
    {
		ddec->convInfo.status = DDEC_ENABLED;
		system->message1 = DDEC_SUCCESS;
		}
	I_UNUSED(theEvent);
	}


/*** TO BE CONFIGURED SKJ July 28 1993***/
/***************************************************************************
* DDEC FUNCTION:			SuspendAllConversation

* Flow:               After suspending
											Cannot suspend
Description: 					This function will suspend all this dde conversation
**************************************************************************/

extern "C" void FAR PASCAL _export SuspendAllConversations(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{

	DWORD 	idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);
	system->message1 = DDEC_FAILURE;
	if (DdeEnableCallback(idInst, NULL, EC_DISABLE))
    {
		system->message1 = DDEC_SUCCESS;
		}
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:			ResumeAllConversation

* Flow:               After Resuminng
											Cannot resume
*	Description: 					This function will resume all this dde conversation
**************************************************************************/

extern "C" void FAR PASCAL _export ResumeAllConversations(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	DWORD idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);
	system->message1 = DDEC_FAILURE;
	if (DdeEnableCallback(idInst, NULL, EC_ENABLEALL))
    {
		system->message1 = DDEC_SUCCESS;
		}
	I_UNUSED(theEvent);
	}



/***************************************************************************
* DDEC FUNCTION:			SetTransactionOptions
* Input Parameters:  	A DDE client object.
											synchronous timeout
											advise data delay
                    	bSendAck
* Flow:               After setting

Description: 					This function will set this dde object's transactions
**************************************************************************/
extern "C" void FAR PASCAL _export SetTransactionsOptions(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
  // synchronous time out
	OBJECTID timeoutID = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	// adv data delay
	OBJECTID delayID = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	// ack flag
  // waiting for flag ALM
	OBJECTID bAckID = AFuncGetTypedParameter(4, OTYPE_FLAG);
	BOOL bAck = ddec->bDefAck;
	long timeout = ddec->timeout;
	long delay   = ddec->defAdvDelay;

	if (bAckID)
		{
		//SKJ waiting for flag ALM     Sept.19,1993
		bAck = OFlagGet(bAckID);
		}

	if (delayID)
		{
		delay = ONmbrGetInteger(delayID);
		}
	if (timeoutID)
		{
		timeout = ONmbrGetInteger(timeoutID);
		}
	ddec->bDefAck = bAck;
	ddec->timeout = timeout;
	ddec->defAdvDelay = delay;
	system->message1 = DDEC_SUCCESS;
	I_UNUSED(theEvent);
	}


/***************************************************************************
* DDEC FUNCTION:			ExecuteServerCommand
* Input Parameters:  	A DDE client object.
											Server Command
											data String
* Flow:               After setting

Description: 					This function will send an execute command to the server
*											format:  "<Command>\t<dats String>"
**************************************************************************/
extern "C" void FAR PASCAL _export ExecuteServerCommand(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	MEMBLOCKID cmemID  = AMemAllocate(0);
	MEMBLOCKID dmemID  = AMemAllocate(1);
	OBJECTID 	 ddecID 	 = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *		 ddec				 = (DDEC *)GetRunTimePtr(ddecID, 0);
	OBJECTID 	 commandID = AFuncGetTypedParameter(2, OTYPE_TEXT);
	OBJECTID 	 dataID 	 = AFuncGetTypedParameter(3, OTYPE_TEXT);
	LPSTR      commandStr, dataStr;


	if (ddec == NULL)
    {
		system->message1 = 2; 
		}
  else {
		AObjConvertToText(commandID, cmemID);
		commandStr = (LPSTR)AMemLock(cmemID);

		if (dataID)
      {
		  AObjConvertToText(dataID, dmemID);
			dataStr = (LPSTR)AMemLock(dmemID);
			}
		else{
			dataStr = (LPSTR)AMemLock(dmemID);
			dataStr[0]= '\0';
      }
		system->message1 = ddec->ExecTransaction(commandStr, dataStr);
		AMemUnlock(cmemID);
		AMemUnlock(dmemID);

		}

	AMemFree(cmemID);
	AMemFree(dmemID);
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:			GetConversationInfo
* Input Parameters:  	A DDE client object.
* Flow:               After getting
											cannot Get
Output:								Service Name
											Topic Name
                    	bIsConnected
Description: 					This function will get the attributes of this conv
**************************************************************************/
extern "C" void FAR PASCAL _export GetConversationInfo(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);

	OBJECTID serverRVal =AObjCreate(OTYPE_TEXT);
	OBJECTID bConvRVal =  AObjCreate(OTYPE_FLAG);
	OBJECTID serviceRVal = AObjCreate(OTYPE_TEXT);
	OBJECTID topicRVal = AObjCreate(OTYPE_TEXT);

	MEMBLOCKID textID = AMemAllocate(lstrlen(ddec->convInfo.ServerName) + 1);
	LPSTR txtStr = (LPSTR)AMemLock(textID);
	lstrcpy(txtStr,ddec->convInfo.ServerName);
	AObjConvertFromText(serverRVal, textID); // return Item Name
	AMemUnlock(textID);

	textID = AMemAllocate(lstrlen(ddec->convInfo.Service) + 1);
	txtStr = (LPSTR)AMemLock(textID);
	lstrcpy(txtStr,ddec->convInfo.Service);
	AObjConvertFromText(serviceRVal, textID); // return Item Name
	AMemUnlock(textID);

	AMemSetSize(textID, lstrlen(ddec->convInfo.Topic) + 1);
	txtStr = (LPSTR)AMemLock(textID);
	lstrcpy(txtStr, ddec->convInfo.Topic);
	AObjConvertFromText(topicRVal, textID); // return Item Name
	AMemUnlock(textID);

	//SKJ waiting for flag ALM     Sept.19,1993
	OFlagSet(bConvRVal, (BOOL)(ddec->convInfo.hConv != 0));

	AFuncSetParameter(1, serverRVal);
	AFuncSetParameter(2, serviceRVal);
	AFuncSetParameter(3, topicRVal);
	AFuncSetParameter(4, bConvRVal);
	system->message1 = DDEC_SUCCESS;
	I_UNUSED(theEvent);
	}


/***************************************************************************
* DDEC FUNCTION:			GetTransactionInfo
* Input Parameters:  	A DDE client object.
											TransactionID
* Flow:               After setting
											cannot Set
Output:								Item Name
											Format Name
                    	Serius Object
Description: 					This function will get the attributes of thid transaction
**************************************************************************/
extern "C" void FAR PASCAL _export GetTransactionInfo(
	pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
	OBJECTID trOID = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	long trID = (long int)ONmbrGetInteger(trOID);
	XACT *lpxact = ddec->GetTransactionContext((int)trID);

	OBJECTID itemRVal = AObjCreate(OTYPE_TEXT);
	OBJECTID formatRVal = AObjCreate(OTYPE_TEXT);

	OBJECTID linkRVal =  AObjCreate(OTYPE_TEXT);
	OBJECTID delayRVal = AObjCreate(OTYPE_NUMBER);
	//SKJ waiting for flag ALM     Sept.19,1993
	OBJECTID ackRVal  = AObjCreate(OTYPE_FLAG);


	if (lpxact == NULL)
		{
		system->message1 = DDEC_FAILURE;
		}
  else {
		MEMBLOCKID textID = AMemAllocate(lstrlen(lpxact->Item) + 1);
		LPSTR txtStr = (LPSTR)AMemLock(textID);
		lstrcpy(txtStr, lpxact->Item);
		AObjConvertFromText(itemRVal, textID); // return Item Name
		AMemUnlock(textID);

		AMemSetSize(textID, lstrlen(lpxact->Format) + 1);
		txtStr = (LPSTR)AMemLock(textID);
		lstrcpy(txtStr, lpxact->Format);
		AObjConvertFromText(formatRVal, textID); // return Item Name
		AMemUnlock(textID);

		LPSTR linkName = new char[20];
		GetLinkName(lpxact->linkType, linkName);
		AMemSetSize(textID, lstrlen(linkName) + 1);
		txtStr = (LPSTR)AMemLock(textID);
		lstrcpy(txtStr,linkName);
		AObjConvertFromText(linkRVal, textID); // return Item Name
		AMemUnlock(textID);

		
		ONmbrSetInteger(delayRVal , lpxact->advDelay, TRUE); // return Item Name

		//SKJ waiting for flag ALM     Sept.19,1993
		OFlagSet(ackRVal, lpxact->bAck);

		system->message1 = DDEC_SUCCESS;
		}
	AFuncSetParameter(1, itemRVal);
	AFuncSetParameter(2, formatRVal);
	AFuncSetParameter(3, linkRVal);
	AFuncSetParameter(4, delayRVal);
	AFuncSetParameter(5, ackRVal);
	if (lpxact)
		AFuncSetParameter(6, lpxact->receiverOID);
	else
		AFuncSetParameter(6, ddec->defReceiverOID);
	I_UNUSED(theEvent);
	}


/***************************************************************************
* DDEC FUNCTION:			LastNewTrans
* Input Parameters:  	A DDE client object.
* Flow:               After setting
											cannot Set
Output:						  	Transaction ID
Description: 					This function will get the id of last transaction that
											was started
**************************************************************************/
extern "C" void FAR PASCAL _export GetLastNewTrans(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
	OBJECTID oidRVal = AObjCreate(OTYPE_NUMBER);
	long id = -1;
	if (ddec->numOfTransaction != 0) {
		id = (long)ddec->lastActvdXact.transID;
		system->message1 = DDEC_SUCCESS;
		}
	else
		{
		system->message1 = DDEC_FAILURE;
		}

	ONmbrSetInteger(oidRVal, (long)id, TRUE);
	AFuncSetParameter(1, oidRVal);
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:			GetAbortedTransaction
* Input Parameters:  	A DDE client object.
* Flow:               After getting
											cannot get
Output:						  	Transaction ID
Description: 					This function will get the id of last stopped transaction
**************************************************************************/
extern "C" void FAR PASCAL _export GetAbortedTransaction(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
	OBJECTID oidRVal = AObjCreate(OTYPE_NUMBER);
	long id = -1;
	system->message1 = DDEC_SUCCESS;

	if (ddec->numOfTransaction != 0) {
		id = (long)ddec->lastAbrtdXact.transID;
		ONmbrSetInteger(oidRVal, (long)id, TRUE);
		system->message1 = DDEC_SUCCESS;
		}
	else
		{
		system->message1 = DDEC_FAILURE;
		}

	ONmbrSetInteger(oidRVal, (long)id, TRUE);
	
	AFuncSetParameter(1, oidRVal);
	I_UNUSED(theEvent);
	}

/***************************************************************************
* DDEC FUNCTION:			LastUpdatedTransaction
* Flow:               After getting
											cannot get
Output:             	DDE Object
											Transaction ID
Description: 					This function will get the dde client object and the
											transaction ID of the last transaction that was handled
											in the Callback
**************************************************************************/
extern "C" void FAR PASCAL _export GetLastUpdatedTrans(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID ddecID = AFuncGetTypedParameter(1, OTYPE_DDEC);
	DDEC *ddec=     (DDEC *)GetRunTimePtr(ddecID, 0);
	OBJECTID oidRVal = AObjCreate(OTYPE_NUMBER);
  long id  = -1;
	system->message1 = DDEC_SUCCESS;

	if (ddec->numOfTransaction != 0) {
		id = (long)ddec->lastUpdtdXact.transID;
		system->message1 = DDEC_SUCCESS;
		}
	else
		{
		system->message1 = DDEC_FAILURE;
		}
	ONmbrSetInteger(oidRVal, (long)id, TRUE);
	AFuncSetParameter(1, oidRVal);
	I_UNUSED(theEvent);
	}

