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

#define MAINMODULE 1

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>


#include "resource.h"
#include <string.h>
#include <memory.h>                                                        
#include "stdlib.h"
#include "stdio.h"
#include <mem.h>
#include "ddemlsv.h"
#define VERSIONOK     1
#define VERSIONERR    2

unsigned int         fmtLink;

ATOM    CF_ALMOBJ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ InitializeApplication();
extern long CALLBACK /*!!PORT!! was _far_ _pascal_*/ EditServerObject(AOBJECTID theObject);
// typedef DDESDATA     *LPDDESDATA;
// typedef DDESOPTIONS *LPDDESOPTIONS;
extern int _WinAllocFlag;

/****************************************************************************
Function: Update Object
Desc:     This function will check the version on the DDES object
          July 1993 - version 1

****************************************************************************/
int UpdateObject(AOBJECTID source, AOBJECTID dest)
  {

  LPDDESDATA    lpSrcData = (LPDDESDATA)AObjLockData(source, data0_DDESDATA);
  LPDDESDATA    lpDestData = (LPDDESDATA)AObjLockData(dest, data0_DDESDATA);
  int rVal;

  if ((lpSrcData->version == DDES_DATAVERSION) && (lpSrcData->version == lpDestData->version))
    rVal =  VERSIONOK;
  else
    rVal =  VERSIONERR;

  AObjUnlockData(source, data0_DDESDATA);
  AObjUnlockData(dest, data0_DDESDATA);
  return rVal;
  }

/***************************************************************************************
* check the datas to see if the input string is a topic name
*
***************************************************************************************/
BOOL isTopic(AOBJECTID dObject, LPSTR topicName)
  {
  int i;
  int rVal  = DDES_TOPICNOTFOUND;
  LPDDESTOPICS  lpTopicsData = (LPDDESTOPICS)AObjLockData(dObject, data1_DDESTOPICS);
  LPDDESDATA    lpData     = (LPDDESDATA)AObjLockData(dObject, data0_DDESDATA);

  for (i = 0; i < lpData->nTopics; i++)
    {
    if (lstrcmp(lpTopicsData->topicList[i].name, topicName) == 0)
      {
      rVal = i;
      break;
      }
    }
  AObjUnlockData(dObject, data1_DDESTOPICS);
  AObjUnlockData(dObject, data0_DDESDATA);

  return rVal;

  }


/*****************************************************************************************
* FUNCTION:   CreateSystemTopic
*
*
*
*****************************************************************************************/
void CreateSystemTopic(AOBJECTID dObject)
  {
  int j;
  // add a new 'system' topic
  AObjResizeData(dObject, data1_DDESTOPICS, sizeof(DDESTOPICDATA) + 1 * sizeof(TOPICINFO));
  LPDDESDATA lpData = (LPDDESDATA)AObjLockData(dObject, data0_DDESDATA);
  LPDDESTOPICS lpTopicsData = (LPDDESTOPICS)AObjLockData(dObject, data1_DDESTOPICS);
  // resize ItemsData to fit new list of Items (per Topic)
  long itemsDataSize = CSYSTEMITEMS * sizeof(ITEMSINFO);
  AObjResizeData(dObject, data2_DDESITEMS, itemsDataSize);
  LPDDESITEMS lpItemsData =  (LPDDESITEMS)AObjLockData(dObject, data2_DDESITEMS);

  // init from data1 and data2
  // the ordering of topic names in data1 is same as the ordering of their items
  // in data2 i.e. lpTopicsData[i].topicName == lpItemList[i].TopicName
  lpData->nTopics = 1; // system topic

  lpTopicsData->topicList[0].nItems  = CSYSTEMITEMS; // items unsder the system topic
  lstrcpy(lpTopicsData->topicList[0].name, SZDDESYS_TOPIC);
  for (j = 0; j < lpTopicsData->topicList[0].nItems; j++)
    {
    lstrcpyn(lpItemsData->itemList[j].Topic,SZDDESYS_TOPIC, MAXNAME);
    lpItemsData->itemList[j].dataSrcOID = 0;
    lpItemsData->itemList[j].format = aFormats[0]; // formatInfo TEXT
    }

  // list of all the topics under this service
  lstrcpyn(lpItemsData->itemList[0].Item, SZDDESYS_ITEM_TOPICS, MAXNAME);
  // list of all the items under system topic
  lstrcpyn(lpItemsData->itemList[1].Item, SZDDESYS_ITEM_SYSITEMS, MAXNAME);
  // list of all the formats supported under this service
  lstrcpyn(lpItemsData->itemList[2].Item, SZDDESYS_ITEM_FORMATS, MAXNAME);
  // Help on this service
  lstrcpyn(lpItemsData->itemList[3].Item, SZDDESYS_ITEM_HELP, MAXNAME);
  // list of all the topic:items (topic:items/ttopic:items)
  lstrcpyn(lpItemsData->itemList[4].Item, SZDDE_ITEM_ITEMLIST, MAXNAME);
  // list of all the signals supported under this service
  lstrcpyn(lpItemsData->itemList[5].Item, SZDDES_SIGNALLIST, MAXNAME);

  AObjUnlockData(dObject, data0_DDESDATA);
  AObjUnlockData(dObject, data1_DDESTOPICS);
  AObjUnlockData(dObject, data2_DDESITEMS);
  }
/*****************************************************************************************
* FUNCTION:   BUILDALLSIGNALS
*             Handle AOBJ_BUILDSIGNAL
*
*
*****************************************************************************************/
int BuildAllSignals(AMEMBLOCKID sigsBlockID, AOBJECTID theObject)
  {
  int i,j;
	pAObjSignalRecord theSigs;

  // Check old signals before building signals
	DDES 		*ddes = new DDES(theObject);
	ddes->CheckSignals();
	ddes->SetSave(DDES_SAVESIGS);
	// ddes->SaveSignalsToData();  // SKJ why do I need this here
	delete ddes;

  LPDDESDATA lpData = (LPDDESDATA)AObjLockData(theObject, data0_DDESDATA);
  int nSignals = lpData->nTopicSignals + lpData->nUserSignals;
  AMemSetSize(sigsBlockID, (nSignals)  * sizeof(AObjSignalRecord));
  theSigs = (pAObjSignalRecord)(LPSTR)AMemLock(sigsBlockID);
  for (i = 0; i < lpData->nTopicSignals; i++)
    {
    // for every topic add the following signals
    //      Conv. <topic> requested
    //      Conv. <topic> established
    //      Conv. <topic> stopped
    // if it is a topic name then the next 3 signals are
    if (isTopic(lpData->ddesOID, lpData->signals[i].name) != DDES_TOPICNOTFOUND)
			{
      // there are only 2 per Topic Signal 
      for (j = 0; j < MAX_TOPICSIGNAL; j++)
        {
        
        switch (j)
          {
					case I_ESTABCONVSIG: sprintf(theSigs[i + j].theName, "Conv. Established (%s)", lpData->signals[i + j].name);
                      break;
          case I_STOPCONVSIG: sprintf(theSigs[i + j].theName, "Conv. Terminated (%s)", lpData->signals[i + j].name);
                      break;
          }
        theSigs[i + j].theID = lpData->signals[i + j].sigID;
        }
      i +=1; 
      }
    else
      {
      // for every new topic:item:format add the following signals
      //      <topic:item:format> data requested
      //      <topic:item:format> data updated

      for (j = 0; j < MAX_ITEMSIGNAL; j++)
        {
        
        switch (j)
          {
          case I_REQUESTTRANSSIG: sprintf(theSigs[i + j].theName, "Item Requested (%s)", lpData->signals[i + j].name);
            break;
          case I_UPDATETRANSSIG: sprintf(theSigs[i + j].theName, "Item Updated (%s)", lpData->signals[i+ j].name);
            break;
          }
        theSigs[i + j].theID = lpData->signals[i + j].sigID;
        }
			i += 1;
      }
    }

  for (i = 0; i < lpData->nUserSignals; i++)
    {
    sprintf(theSigs[i + lpData->nTopicSignals].theName, "User Defined:%s", lpData->signals[i + lpData->nTopicSignals].name);
    theSigs[i + lpData->nTopicSignals].theID = lpData->signals[i + lpData->nTopicSignals].sigID;
    }

	AObjUnlockData(theObject, data0_DDESDATA);
	AMemUnlock(sigsBlockID);
  return 1;
	}



/*********************************************************************************************
*
* ADMIN ROUTINE:	DuplicateServiceName
*
*	DESCRIPTION:		This fucntion checks if a service name (in a ddes object) is duplicated in
*									any other DDES object.  If so it returns an ERROR.  It is called during the
									handling of the AOBJ_CHECKOBJECT
*
**********************************************************************************************/

BOOL IsDuplicateServiceName(
	AOBJECTID theObject,
	LPSTR lpszService)    // if the service name is same as an existing service, do not exit
	{
	long index = -1;
	AOBJECTID  oid;
	BOOL bError = FALSE;

	while ((oid = AObjGetNextObject(OTYPE_DDES, &index)) != 0L)
		{
		if (oid != theObject)
			{
			LPDDESDATA   	lpData     = (LPDDESDATA)AObjLockData(oid, data0_DDESDATA);
			if (lstrcmp(lpszService, lpData->Service) == 0)
				{
				bError = TRUE;
				break;
				}
			AObjUnlockData(oid, data0_DDESDATA);
      }
		}
	return bError;
	}



/*********************************************************************************************
*
* ADMIN ROUTINE:	UpdateVersion
*
*	DESCRIPTION:		This function updates the version of DDES object that is being read in
*									(for backward compatability)
*
**********************************************************************************************/
long 	 UpdateVersion(
	AOBJECTID dObject)
	{
	long rVal = A_OK;
	LPDDESDATA lpData = (LPDDESDATA)AObjLockData(dObject, data0_DDESDATA);
	switch (lpData->version)
  	{
		case DDES_DATAVERSION_1:
				{
				lpData->version = DDES_DATAVERSION;
				AObjResizeData(dObject, data3_DEFVALS, sizeof(DEFOBJDDATA));
				LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(dObject, data3_DEFVALS);
				// number of user signals + number of topic/item signals;
				lpDefData->maxSignalID = lpData->nUserSignals + lpData->nTopicSignals;
				AObjUnlockData(dObject, data3_DEFVALS);
				}
				break;
		default:
    	rVal = A_ERROR;

		}

	AObjUnlockData(dObject, data0_DDESDATA);
  return rVal;

  }

/*********************************************************************************************
*
* ADMIN ROUTINE:	ddeSADMN
*
*	DESCRIPTION:		This function is the ADMN routine for DDE Server Object
*
**********************************************************************************************/
extern "C" LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/  ddeSADMN(
	AOBJECTID          theObject,
  pAObjMessage      system)
  {

  AOBJECTID   	 			sObject = 0;
  AOBJECTID    			dObject = 0;
  AEvtInfo 					tEvent;
	char              *errorString = new char[255];
	long    					rVal = A_OK;

  errorString[0] 		= '\0';

  switch (system->message1)
    {
    case  AOBJ_CREATED:
      {
      ddesCount++;
      dObject = (AOBJECTID)theObject;
      AObjSetDataCount(dObject, MAXDDESERVERDATAS);
      AObjResizeData(dObject, data0_DDESDATA, sizeof(DDESDATA));
      AObjResizeData(dObject, data1_DDESTOPICS, sizeof(DDESTOPICDATA));
      AObjResizeData(dObject, data2_DDESITEMS, sizeof(DDESITEMDATA));
			AObjResizeData(dObject, data3_DEFVALS, sizeof(DEFOBJDDATA));

			LPDDESDATA lpData = (LPDDESDATA)AObjLockData(dObject, data0_DDESDATA);
			LPDDESTOPICS lpTopics = (LPDDESTOPICS)AObjLockData(dObject, data1_DDESTOPICS);
      LPDDESITEMS lpItems =  (LPDDESITEMS)AObjLockData(dObject, data2_DDESITEMS);
      LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(dObject, data3_DEFVALS);

			_fmemset(lpData, 0, sizeof(DDESDATA));

			lpData->version = DDES_DATAVERSION;
      lstrcpy(lpData->Service,"");

			lpData->ddesOID = dObject;
			lpData->idInst = DDES_NOTINIT;
      lpData->nUserSignals = 0;
			lpData->nTopicSignals = 0;
			lpData->nTopics = 0;

      _fmemset(lpTopics, 0, sizeof(DDESTOPICDATA));
      _fmemset(lpItems, 0, sizeof(DDESITEMDATA));
			// Init the Def Values and use them to init the OBJD Controls
      lpDefData->defTopic[0] = '\0';
      lpDefData->defItem[0] = '\0';
      lpDefData->defFormat.name[0] = '\0';
      lpDefData->defFormat.atom = 0;
      lpDefData->defUpdateTime = 100;
			lpDefData->defDataSrcOID = 0;
			lpDefData->maxSignalID = 1;

			// lpTopics->nItems = 0;

			// Set the default CCFilter (CONV. context)
			_fmemset(&(lpData->CCFilter), 0, sizeof(CONVCONTEXT));
			GetProfileString("intl", "sCountry", "United States", lpData->Country, MAXNAME);
			GetProfileString("intl", "sLanguage", "U.S. English",lpData->Language, MAXNAME);
			lstrcpy(lpData->Language, "Modern English");
			lstrcpy(lpData->CodePage,"ANSI");
			lpData->CCFilter.iCodePage  = CP_WINANSI;
			lpData->CCFilter.wCountryID = GetProfileInt("intl", "iCountry", 1);
			lpData->CCFilter.dwLangID = atol(lpData->Language);
			lpData->CCFilter.cb = sizeof(CONVCONTEXT);
			lpData->CCFilter.dwSecurity = 0;

			if (system->message4 == AOBJ_TEMPORARY)
        {
        ;
				}
      AObjUnlockData(dObject, data0_DDESDATA);
			AObjUnlockData(dObject, data1_DDESTOPICS);
      AObjUnlockData(dObject, data2_DDESITEMS);
      AObjUnlockData(dObject, data3_DEFVALS);

			// for every ddes object there is a default topic "SYSTEM" which is used by the clients to query for
      // different info. on the server.
      // Build default "SYSTEM" topic and its items
      CreateSystemTopic(dObject);
			}
      break;

    case AOBJ_ASSIGNOBJECT:
      {
      int i;
      dObject = (AOBJECTID)(theObject);    // destination
      sObject = (AOBJECTID)(system->message3);    // source
			if (sObject == dObject)
        break;

      if (UpdateObject(sObject, dObject) != VERSIONOK)
        {
        lstrcpy(system->errorMessage, "AOBJ_ASSIGNOBJECT failed : DDE CLIENT/tDDES");
        return A_ERROR;
        }

			for (i = 0; i < MAXDDESERVERDATAS ; i++)
        {
        LPSTR lpSrc = (LPSTR)AObjLockData(sObject, i);
        long size = AObjGetDataSize(sObject, i);
        AObjResizeData(dObject, i, size);
        LPSTR lpDst = (LPSTR)AObjLockData(dObject, i);
        if (sObject && dObject)
          {
          CopyMemory(lpDst, lpSrc, size);
          AObjUnlockData(sObject, i);
          AObjUnlockData(dObject, i);
          }
        }

			tEvent.sourceObject = dObject;
      tEvent.targetObject = dObject;
      tEvent.what = AEVENT_OBJECTCHANGED;
      tEvent.when = 0; //Tickcount;
      tEvent.how = AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
      tEvent.details = 0;
      tEvent.modifiers = 0; 

      AEvtPost(&tEvent);
			}
      break;

    // added for data and dll version check 4/13/93
    case AOBJ_READ:
			{

			dObject = (AOBJECTID)theObject;
			LPDDESDATA   	lpData     = (LPDDESDATA)AObjLockData(theObject, data0_DDESDATA);
			long 	version = lpData->version;
			AObjUnlockData(theObject, data0_DDESDATA);
			if ( version != DDES_DATAVERSION)
				{
				UpdateVersion(dObject);
				}
			system->message4 = A_OK;
      system->message3 = (LONG)DDES_DATAVERSION; // return current data Version
      }
      break;

		// Currently there is no IMP/EXP supported for this object.  There is no equivalent object
    // on other platforms (MAC etc)
    case AOBJ_IMPORTOBJECT:
			break;

    case AOBJ_EXPORTOBJECT:
			break;

    case AOBJ_EDITOBJECT:
      {
			ATypeSetRuntimeInfo(OTYPE_DDES, (LONG)DDES_NOTINIT);   // no dde initialisation
			AObjSetRuntimeInfo(theObject, 0L); // init the data to 0
			rVal = EditServerObject(theObject);

      }
      break;


		case AOBJ_CHECKOBJECT: {
			int i, j;
			BOOL bError = FALSE;
			LPDDESDATA   	lpData     = (LPDDESDATA)AObjLockData(theObject, data0_DDESDATA);
			LPDDESITEMS  	lpItemsData = (LPDDESITEMS)AObjLockData(theObject, data2_DDESITEMS);
			char				  *errorMsg 	= new char[MAXNAME];

			if (lstrcmp(lpData->Service, "") == 0)
				{
				lstrcpy(errorMsg, "Invalid Service Name in DDE Server Object");
				bError = TRUE;
				}
			else 	if (IsDuplicateServiceName(theObject, lpData->Service) == TRUE)   // if the service name is same as an existing service, do not exit
				{
			 	lstrcpy(errorMsg, "Duplicate Service Name in DDE Server Objects");
				bError = TRUE;
				}
			else
				{
				int nSize = (int)AObjGetDataSize(theObject, data2_DDESITEMS);
				nSize = nSize/sizeof(ITEMSINFO); // number of items
				for (j = CSYSTEMITEMS; j < nSize; j++)
					{
					if (AObjCheckType(lpItemsData->itemList[j].dataSrcOID, 0) == FALSE)
						{
						bError = TRUE;
						lpItemsData->itemList[j].dataSrcOID= INVALIDOID;
						lstrcpy(errorMsg, "The DDE Server object should be connected to a valid data source object");
						break;
						}
					}

				if (bError == FALSE)
					{ // no Error
					for (i = 0; i < lpData->nUserSignals ; i++)
          	{
						if (AObjCheckType(lpData->signals[lpData->nTopicSignals + i].rcvrOID, 0) == FALSE)
            	{
							bError = TRUE;
							lstrcpy(errorMsg, "Error in DDE Server Signals");
							// set the rVal = A_VALIDATESIGNALS
							break;
							}
            }
					}
				}

			AObjUnlockData(theObject, data0_DDESDATA);
      AObjUnlockData(theObject, data2_DDESITEMS);
			if (bError) {
				rVal = A_ERROR;
				AObjReportError	(	theObject,
													AOBJ_CHECKOBJECT,
													A_ERROR,
                          errorMsg,
													0);
				}
			}
      break;
      
    case AOBJ_VALIDATEOBJECTIDS:
      {
      int i, j;
      LPDDESDATA   lpData     = (LPDDESDATA)AObjLockData(theObject, data0_DDESDATA);
      LPDDESTOPICS lpTopicsData = (LPDDESTOPICS)AObjLockData(theObject, data1_DDESTOPICS);
      LPDDESITEMS  lpItemsData = (LPDDESITEMS)AObjLockData(theObject, data2_DDESITEMS);
			int oldIndex = 0;
      for (i = 0; i < lpData->nTopics; i++)
        {
				for (j = oldIndex; j <lpTopicsData->topicList[i].nItems + oldIndex; j++)
					{
          AObjValidateID( &(AOBJECTID)lpItemsData->itemList[j].dataSrcOID, system);
					}
				oldIndex = j;
				}

			// normalize signal receiver OID for runtime
			for (i = 0; i < lpData->nUserSignals; i++)
				{
				AObjValidateID( &(AOBJECTID)lpData->signals[i + lpData->nTopicSignals].rcvrOID, system);
				}

			AObjUnlockData(theObject, data0_DDESDATA);
			AObjUnlockData(theObject, data1_DDESTOPICS);
      AObjUnlockData(theObject, data2_DDESITEMS);

      break;
			}

    case AOBJ_BUILDSIGNALS:
      {
      BuildAllSignals((AMEMBLOCKID)system->message2, theObject);
			break;
			}

		default:
			rVal = A_NOTHANDLED;
			break;

		}

	delete errorString;
	return rVal;
  }




/******************************************************************************
*
*  Every DLL has an entry point LibMain/*!!PORT!! Replace with DllEntryPoint */ and an exit point WEP/*!!PORT!! Replace with DllEntryPoint */.
*
*******************************************************************************/
#pragma argsused
extern "C" int CALLBACK /*!!PORT!! was _far_ _pascal_*/ LibMain/*!!PORT!! Replace with DllEntryPoint */(
  HINSTANCE     hInstance,
  WORD          wDataSegment,
  WORD          wHeapSize,
  LPSTR         lpszCmdLine)
  {
  _WinAllocFlag = GMEM_SHARE;

  // The startup code for the DLL initializes the local heap (if there is one)
  // with a call to LocalInit which locks the data segment.
  if ( wHeapSize != 0 )
    UnlockData( 0 );

  hInst = hInstance;

  if (!InitializeApplication()) /* Initialize shared things */
    return (FALSE);              /* Exits if unable to initialize     */

  /* Perform initializations that apply to a specific instance */



  
  return 1;   // Indicate that the DLL was initialized successfully.
  }

/***************************************************************************
*
*
*
****************************************************************************/
int CALLBACK /*!!PORT!! was _far_ _pascal_*/ WEP/*!!PORT!! Replace with DllEntryPoint */ ( int bSystemExit )
  {
  I_UNUSED(bSystemExit);
  return 1;
  }




/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitializeApplication ()                                   *
 *                                                                          *
 *  PURPOSE    : Registers the Link clipboard format                        *
 *                                                                          *
 *  RETURNS    : TRUE  - If successful.                                     *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ InitializeApplication()
  {
		fmtLink = RegisterClipboardFormat("Link");

		if (!fmtLink)
        return FALSE;
    lstrcpy(aFormats[0].name, CFTEXT);
    aFormats[0].atom = CF_TEXT;

    lstrcpy(aFormats[1].name, CFBITMAP);
    aFormats[1].atom = CF_BITMAP;

    lstrcpy(aFormats[2].name,CFMETAFILE);
    aFormats[2].atom = CF_METAFILEPICT;

    lstrcpy(aFormats[3].name, CFALMOBJ);
    // build the atom at
    aFormats[3].atom = RegisterClipboardFormat("CF_ALMOBJ");
    CF_ALMOBJ = aFormats[3].atom;
    return TRUE;

  }



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitializeInstance ()                                      *
 *                                                                          *
 *  PURPOSE    : Performs a per-instance initialization of Client.          *
 *                                                                       *
 *  RETURNS    : TRUE  - If initialization was successful.                  *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ InitializeInstance(
  WORD nCmdShow)
  {
  I_UNUSED(nCmdShow);
  return TRUE;
  }





/****************************************************************************

    FUNCTION:  InitObjectInstance()

    PURPOSE:  Saves instance handle and creates main window

    COMMENTS:

        This function is called at initialization time for every instance of
        this application.  This function performs initialization tasks that
        cannot be shared by multiple instances.

        In this case, we save the instance handle in a static variable and
        create and display the main program window.

****************************************************************************/

BOOL InitObjectInstance()
  {
	/* Save the instance handle in static variable, which will be used in  */
  /* many subsequence calls from this application to Windows.            */


	return (FALSE);
 
  }




