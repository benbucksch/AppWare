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
// FILE: 				DDECMDRT.cpp
//
// AUTHOR:      Shaili Jain
//
// DESCRIPTION: MDRT code for DDEC ALM
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT
#define   MDRTCODE 						1
#include "ddemlcl.h"
#include <a_almutl.h>
#include "runtime.h"                                                  
#include "stdio.h"
#include "string.h"
#include "o_number.h"


extern BOOL InitializeDDE();
extern BOOL UninitializeDDE();
extern int GetGData(HDDEDATA hData,MEMBLOCKID txtID, int fmt);

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description:  																															*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
int GetLinkType(LPSTR linkName)
	{

	if ( (strcmpi(linkName, "AUTOMATIC") == 0) ||
			 (strcmpi(linkName,"HOT") == 0))
		return IDAUTOLINK;
	else if ( (strcmpi(linkName, "NOTIFY") == 0) ||
			   (strcmpi(linkName,"WARM") == 0))
		return IDNOTIFYLINK;
	else if ( (strcmpi(linkName, "MANUAL") == 0) ||
			 (strcmpi(linkName,"COLD") == 0))
		return IDMANUALLINK;

	return 0;

	}


/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description:  																															*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
int GetLinkName(int linkType, LPSTR linkName)
	{

	if (linkType == IDAUTOLINK)
		{
		lstrcpy(linkName, "AUTOMATIC");

		}
	else if (linkType == IDNOTIFYLINK)
		{
		lstrcpy(linkName, "NOTIFY");

		}
	else 	if (linkType == IDMANUALLINK)
		{
		lstrcpy(linkName, "MANUAL");

    }

	return 0;

  }


/*********************************************************************
*FUNCTION:  GetRunTimePtr
*        :	This function will create a C++ ptr during startup msg
						and stuff it in the object datas.  For subsequent calls
*						it returns this pointer
*
*
**********************************************************************/

LPLONG GetRunTimePtr(OBJECTID theObject, BOOL bIsStartUp )
	{
	DDEC *ddeC = NULL;
	if (!bIsStartUp)
		{
		ddeC = (DDEC *)AObjGetRuntimeInfo(theObject);
		}
	else // AEVENT_STARTUP
		{
		DDEC::serviceListID = AMemAllocate(sizeof(SERVICELIST));
		ddeC = new DDEC(theObject, FALSE);
		AObjGetDataCount(theObject);
		AObjSetDataCount(theObject, MAXDDECLIENTDATAS);
		AObjSetRuntimeInfo(theObject, (long)ddeC);
		}

	return (LPLONG)ddeC;

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: This function will return the id of the clipboard formats			*
*																																							*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
ATOM GetFormatID(char *formatStr)
	{

	if (lstrcmpi(formatStr, CFTEXT) == 0)
		return CF_TEXT;
	else if (lstrcmpi(formatStr, CFBITMAP) == 0)
		return CF_BITMAP;
	else if (lstrcmpi(formatStr, CFMETAFILE) == 0)
		return CF_METAFILEPICT;
	else 
		return SFormats[3].typeID;  // CF_ALMOBJ

	}




/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: DDEC Signal Queue Class Constructor														*
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
DDECQ::DDECQ()
	{
	int i;
	QITEM * prevItem = NULL;
	for (i = 0; i < MAXQITEMS; i++)
		{
		if (i != 0)
			prevItem = curItem;

		curItem = new QITEM();
		_fmemset(curItem, 0, sizeof(QITEM));
		if (i == 0)
			startItem = curItem;

    if (prevItem)                    // make the prevItem point to this item
			prevItem->nextItem = curItem;  // a circular q
		curItem->bNewSig = FALSE;
		}
	curItem->nextItem = startItem; // last element points to
																 // the first element
	curItem = startItem;
	insertItem = startItem;

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: DDEC Signal Dispatcer																					*
*																																						  *
* Return:      													                                      *
*                                                                             *
******************************************************************************/
BOOL DDEC::DispatchCurSignal()
  {
	int index = 0;
	XACT *lpxact;
	int pxactCount = 0;

	// nothing in the q
	if ((sigQ == NULL) || (sigQ->curItem->bNewSig == FALSE))
		return FALSE;

	while ((lpxact = EnumTransactions((LPBYTE)sigQ->curItem->hsz, TRUE, sigQ->curItem->wFmt, &index)) != NULL)
		{
		pxactCount++;
		/*
    * AdviseStart window is gone, make a new one.
    */
		// if no transaction is found
		if (index == 0)
			return(DDE_FNOTPROCESSED);

		// if this transaction is stopped then do not display data
		if (lpxact->status & DDETRANS_STOPPED)
			{
			continue;
			}

		// did the last update occur less then AdvDelay ago.
		if (Delay(lpxact))
			{
      continue;
			}

		_fmemcpy(&(lastUpdtdXact), lpxact, sizeof(XACT));

		// lpData = (LPSTR) AMemLock(sigQ->curItem->dataID);
		if (sigQ->curItem->sigID == SIGDDEC_ADVDATACHANGED) { // for hot links
			BOOL isHandle = FALSE; // it is a MEMBLOCKID to data buffer
      /*
      * Show incomming data on corresponding transaction window.
			*/
			SetALMData(lpxact, (HDDEDATA)sigQ->curItem->dataID, isHandle , FALSE);
			}
		} // end of while pxact

	AEvtPostSignalAtMark(ddeCID, sigQ->curItem->sigID);

	// Free the associated data context with this signal
	// The data context was built in the DDEC callback when the corresponding
	// ADVDATA msg. was recd.
	sigQ->RemoveCurItem();
	sigQ->curItem = sigQ->curItem->nextItem;

	return TRUE;

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* DESCTRUCTOR for DDEC Signal Queue*																					*
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
DDECQ::~DDECQ()
	{
	int i;
	QITEM *qItem;

	for (i = 0; i < MAXQITEMS; i++)
		{
    if (curItem)
			qItem = curItem->nextItem;
		else
			break;
		RemoveCurItem();  // free mem and string assoc. with this item
		delete curItem;
		curItem = qItem;
    }

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: 
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
DDECQ * DDECQ::BuildQ()
	{	
 	DDECQ * ddecQ = new DDECQ();
	return ddecQ;

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: 
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/

DDECQ::DestroyQ(DDECQ *ddecQ)
	{
	delete ddecQ;
	return 1;

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: 
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
DDECQ::AddToQ(HSZ hsz, int wFmt, long int sigID,HDDEDATA hData)
	{
	DWORD	idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);
	MEMBLOCKID dataID = AMemAllocate(0);

	if (sigID ==  SIGDDEC_ADVDATACHANGED)
		{
		GetGData(hData, dataID, wFmt);
    }
  
	insertItem->sigID = sigID;
	insertItem->bNewSig = TRUE;
	DdeKeepStringHandle(idInst, hsz); // increment usage count on the string
	insertItem->hsz = hsz;
  insertItem->wFmt = wFmt;
	insertItem->dataID = dataID;
	insertItem = insertItem->nextItem; // move the insertion point
	return 1;

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: 
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
DDECQ::RemoveCurItem()
	{
	DWORD	idInst = ATypeGetRuntimeInfo(OTYPE_DDEC);

	int rVal = 0;
  if (curItem->bNewSig == TRUE) // if the signal has never been posted
		{
		curItem->bNewSig = FALSE;
		DdeFreeStringHandle(idInst, curItem->hsz);
		AMemFree(curItem->dataID);
		rVal = 1;
    }
	return rVal;

	}

/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: 
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
QITEM * DDECQ::GetCurItem()
	{
  return curItem;

	}


/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: 
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
DDECQ::SetCurItem(QITEM *item)
	{
  _fmemcpy(curItem, item, sizeof(QITEM));
	return 1;

	}


/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description: 
*
* Return:      													                                      *
*                                                                             *
******************************************************************************/
DDECQ::SetStartItem(QITEM *item)
	{

	_fmemcpy(startItem, item, sizeof(QITEM));
	return 1;

	}


/******************************************************************************
* Function: 		                                                              *
*                                                                             *
* Description:* This is the MDRT routine for the dde client Object						*
*								the common entry point for all the events from the shell      *
* Return:      													                                      *
*                                                                             *
******************************************************************************/
extern "C" void FAR PASCAL _export ddeClientMDRT(
	pAEvtInfo theEvent,
	pAObjMessage      system)
	{
	OBJECTID  theObject = theEvent->targetObject;
	DDEC *ddeC;

	switch ((long)theEvent->what)  {

		case AEVENT_STARTUP:
			{
      long i = -1;
			InitializeDDE();
			
			while ((theObject = AObjGetNextObject(OTYPE_DDEC, &i)) != NULL)
				{
				ddeC = (DDEC *)GetRunTimePtr(theObject, (theEvent->what==AEVENT_STARTUP));
				ddeC->sigQ = DDECQ::BuildQ(); // build a circular signal Queue
				// post a polling event for every instance of ddec object
				AEvtPostStandard(theObject, ddec_PollSigQ, 0, AEVT_ATTAIL);
				}
			}
			break;
    
		case ddec_PollSigQ:
			{// Aug. 5, 1993
			// this event will be used to periodically poll the signal Q for
      // each ddec object (i.e. each conversation) and dispatch a serius signal
			DDEC *ddec = (DDEC *)GetRunTimePtr(theObject, (theEvent->what==AEVENT_STARTUP));
			ddec->DispatchCurSignal();
			}
			break;

		case AEVENT_SHUTDOWN:
			{
			long i = -1;
			while ((theObject = AObjGetNextObject(OTYPE_DDEC, &i)) != NULL)
				{
				ddeC = (DDEC *)GetRunTimePtr(theObject, (theEvent->what==AEVENT_STARTUP));
				ddeC->Disconnect();
				DDECQ::DestroyQ(ddeC->sigQ);
				ddeC->sigQ = NULL;
				delete ddeC;
				}
			// free up any appowned handles
			AMemFree(DDEC::serviceListID);
			DDEC::serviceListID = 0;
			UninitializeDDE();

			break;
      }
		}

	I_UNUSED(system);

	}



