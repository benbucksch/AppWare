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

/*********************************************************************
 * This module contains an implementation of a sophisticated DDE server                                        
 * that uses enumerable topics and items.  It does not take
 * full advantage of appowned data handles (when fAppowned is set) to                       
 * minimize the need for repeated rendering of data when shared with
 * multiple clients.
 *
 * The server supports full system topic information plus help and non
 * system topic item enumeration for the benefit of browsing clients
 * that are wondering what's around.
 *
 * This server can be made secure by altering the conversation context
 * filter.
 *
 * This server can appear to support alternate codepages and languages
 * by altering the conversation context filter.   On Windows this is
 * pretty much moot since there is not yet a clearly defined way of
 * doing international communication and because the atom manager restricts
 * what topic and item strings can be used on the system.
 **********************************************************************/

#define STRICT
#define ASTRICT
#define	DDESMODULE

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_wnd.h>
#include <op_wnd.h>
#include "o_pict.h"

#include "ddemlsv.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <o_text.h>


/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)    (((i) + 31) / 32 * 4)


extern LPLONG GetRunTimePtr(OBJECTID theObject, BOOL bIsStartUp );
extern ATOM   CF_ALMOBJ;

WORD DDES::cServers = 0;

CONVCONTEXT CCFilter = { sizeof(CONVCONTEXT), 0, 0, CP_WINANSI, 0L, 0L };

FORMATINFO aFormats[CFORMATS] = {
	{ 1, 	"TEXT" },      // exception!  predefined format, note:
                        // we use the standard imposed by EXCEL of not including
                        // the "CF_" and use all UPPERCASE.
  { 2, 	"BITMAP"  },
	{ 3, 	"METAFILE"},
	{ 4,	"ALMOBJ"}
  };



  /*
   *          "System" Topic and Item tables supported under all services provided by APPWARE DDE Server application. 
   */

	/*   HSZ    PROCEDURE       PSZ        */

	SYSITEMLIST SystemTopicItemList[CSYSTEMITEMS] = {
		{ 0, TopicListXfer,  SZDDESYS_ITEM_TOPICS   }, 			// Names of topics supported under this service
		{ 0, SysItemListXfer,SZDDESYS_ITEM_SYSITEMS },		// Mames of items supported under the system topic
		{ 0, SysFormatsXfer, SZDDESYS_ITEM_FORMATS  },    	// Names of formats supported under this service
		{ 0, HelpXfer,       SZDDESYS_ITEM_HELP			},      // Help supported under this service
		{ 0, AllItemsXfer,   SZDDE_ITEM_ITEMLIST		},     	// List of all nonSystem topic items supported under this service
		{ 0, SignalsXfer,    SZDDES_SIGNALLIST			},      // List of all signals supported under this service
		};




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
	




void Delay(
	DWORD delay,
	BOOL fModal)
	{
  MSG msg;
  delay = GetCurrentTime() + delay;
  while (GetCurrentTime() < delay) {
  	if (fModal && PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)) {
    	TranslateMessage(&msg);
      DispatchMessage(&msg);
      }
    }
	}




/******************************************************************************
 *
 * This function verifies that the incomming conversation context fits the
 * server's context filter's requirements.
 *
******************************************************************************/
BOOL DDES::ValidateContext(
	PCONVCONTEXT pCC)
	{
  // make sure our CCFilter allows it...mock security, language support
  // old DDE app client case...pCC == NULL
  if (pCC == NULL &&
  	CCFilter.dwSecurity == 0 &&      // were nonsecure
    CCFilter.iCodePage == CP_WINANSI // were normal cp
		)
		{
    return(TRUE);
    }

  if (pCC &&
  	pCC->wFlags == CCFilter.wFlags && // no special flags needed
    pCC->iCodePage == CCFilter.iCodePage && // codepages match
    pCC->dwSecurity == CCFilter.dwSecurity) { // security passes
    // dont care about language and country.
    return(TRUE);
    }
	return(FALSE);  // disallow no match
	}

/*****************************************************************************
* Method: SignalExists  
*         Check to see if the signal already exists
*        
*
*****************************************************************************/
BOOL DDES::SignalExists(LPSTR sigName)
	{
	int i = 0;
	for (i = 0; i < nTopicSignals; i++)
		{
		if (strcmp(topicSignals[i].name, sigName) == 0)
      return TRUE;
		}

  return FALSE;

	}

/*****************************************************************************
* Method: AdjustTopicSignals  
*         Set the proper SIG ID from datas or create new ID
*        
*
*****************************************************************************/
BOOL DDES::AdjustTopicSignalIDs()
	{
	int i = 0;
	int j = 0;
	LPDDESDATA   lpData = (LPDDESDATA)AObjLockData(ddesOID, data0_DDESDATA);
	LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(ddesOID, data3_DEFVALS);
	BOOL 		foundSig  = FALSE;
	
	for (i = 0; i < nTopicSignals; i++)
		{
    foundSig = FALSE;
		for (j = 0; j < lpData->nTopicSignals; j++)
      {
			if (strcmp(topicSignals[i].name, lpData->signals[j].name) == 0)
				{
				topicSignals[i].sigID = lpData->signals[j].sigID;
				topicSignals[i+1].sigID = lpData->signals[j + 1].sigID;
				// topicSignals[i+2].sigID = lpData->signals[j + 2].sigID;
				// topicSignals[i+3].sigID = lpData->signals[j+ 3].sigID;
				i += 1;
        foundSig = TRUE;
				break;   // break out of for j= 0 ...
				}
			}

		if (foundSig == FALSE) {
			topicSignals[i].sigID = lpDefData->maxSignalID++;
			}

		}

	maxSignalID = lpDefData->maxSignalID;
	lpData->nTopicSignals = nTopicSignals;
	AObjUnlockData(ddesOID, data0_DDESDATA);
	AObjUnlockData(ddesOID, data3_DEFVALS);
	return nTopicSignals;

	}


/*****************************************************************************
* Method: AdjustUserSignal
*         This method will write a user-defined signal to our datas (if it already does not exist)
*         
*
*****************************************************************************/
int DDES::AdjustUserSignalIDs()
	{

	int i = 0;
	int j = 0;
	LPDDESDATA   lpData = (LPDDESDATA)AObjLockData(ddesOID, data0_DDESDATA);
	LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(ddesOID, data3_DEFVALS);
	BOOL 		foundSig  = FALSE;
	
	for (i = 0; i < nUserSignals; i++)
		{
    foundSig = FALSE;
		for (j = 0; j < lpData->nUserSignals; j++)
      {
			if (strcmp(userSignals[i].name, lpData->signals[j + lpData->nTopicSignals].name) == 0)
				{
				userSignals[i].sigID = lpData->signals[j + lpData->nTopicSignals].sigID;
				foundSig = TRUE;
				break;
				}
			}
		if (foundSig == FALSE)
			userSignals[i].sigID = lpDefData->maxSignalID++;
		}

	maxSignalID = lpDefData->maxSignalID;
	lpData->nUserSignals = nUserSignals;
	AObjUnlockData(ddesOID, data0_DDESDATA);
	AObjUnlockData(ddesOID, data3_DEFVALS);
	return nTopicSignals;
	}



/*****************************************************************************
* Method: AddTopicSignal  
*         This method will add a topic based signal to our list
*         We do not need to tie a rcvr OID to this signal
*
*****************************************************************************/
int DDES::AddTopicSignal(LPSTR sigName, BOOL isTopic)
	{
  int i; //  j;
	SIGNALINFO *sigs = NULL;


  if (isTopic)
    {
		sigs = (SIGNALINFO *)new char[(nTopicSignals + MAX_TOPICSIGNAL) * sizeof(SIGNALINFO)];
    if (topicSignals)
      _fmemcpy(sigs, topicSignals,(nTopicSignals) * sizeof(SIGNALINFO));

		for (i =0; i < MAX_TOPICSIGNAL; i++)
      {
      strcpy(sigs[nTopicSignals].name, sigName);
			sigs[nTopicSignals].sigID = maxSignalID; // 1 based signals
			maxSignalID++;
      nTopicSignals++;
      }
    }
  else {

    sigs = (SIGNALINFO *)new char[(nTopicSignals + MAX_ITEMSIGNAL) * sizeof(SIGNALINFO)];
    if (topicSignals)
      _fmemcpy(sigs, topicSignals,(nTopicSignals) * sizeof(SIGNALINFO));
		for (i =  0; i <  MAX_ITEMSIGNAL; i++)
      {
      
      strcpy(sigs[nTopicSignals].name, sigName);
			sigs[nTopicSignals].sigID = 	maxSignalID++;; // 1 based signals
      nTopicSignals++;
      }

    }
 

	delete topicSignals;
  topicSignals = sigs; // save new signal list
  return nTopicSignals;
	}


/*****************************************************************************
* Method: SaveSiganlsToData
*         This method will build an array of signals based on the available topice
*         /items and user defined signals and save to Datas
*
*****************************************************************************/
int DDES::SaveSignalsToData()
  {
	int i = 0;
	LPDDESDATA   lpData = (LPDDESDATA)AObjLockData(ddesOID, data0_DDESDATA);
	int nOldTopicSignals = lpData->nTopicSignals;

	AdjustTopicSignalIDs(); // for old signals use old ID from Datas

	lpData->nTopicSignals = nTopicSignals;
  _fmemcpy((lpData->signals), topicSignals, (nTopicSignals) * sizeof(SIGNALINFO));


	// at objd time the DDES structure only consists of the User defined signals
	// Save if the number of new topics has changed or if user-defined signals have been modified.
	if (bSave & DDES_SAVESIGS)
		{
		AdjustUserSignalIDs(); // for old signals use old ID from Datas
		lpData->nUserSignals = nUserSignals;
    _fmemcpy((lpData->signals + nTopicSignals), userSignals, (nUserSignals) * sizeof(SIGNALINFO));
		}
	else 	if (nOldTopicSignals != nTopicSignals) // if new topics have been added/deleted, append the user-signals again
  	{
		lpData->nUserSignals = nUserSignals;
    _fmemcpy((lpData->signals + nTopicSignals), userSignals, (nUserSignals) * sizeof(SIGNALINFO));
		}

  AObjUnlockData(ddesOID, data0_DDESDATA);
	delete userSignals; // delete the userSignals Structure (Allocated during DDES()
  return TRUE;

	}



/*****************************************************************************
* Method: BuildSignalsFromData
*         This method will build an array of signals from the Datas
*
*
*****************************************************************************/
int DDES::BuildSignalsFromData()
  {
  LPDDESDATA    lpData     = (LPDDESDATA)AObjLockData(ddesOID, data0_DDESDATA);

  if (nUserSignals)
    {
    userSignals = new SIGNALINFO[nUserSignals]; // array of signals
		_fmemcpy(userSignals, &(lpData->signals[nTopicSignals]), nUserSignals * sizeof(SIGNALINFO));
    }
  else
    userSignals = NULL;


  if (nTopicSignals)
    {
    topicSignals = new SIGNALINFO[nTopicSignals]; // array of signals
    _fmemcpy(topicSignals, lpData->signals, nTopicSignals * sizeof(SIGNALINFO));
    }
  else
    topicSignals = NULL;

	AObjUnlockData(ddesOID, data0_DDESDATA);
  return nUserSignals + nTopicSignals;
	}


/************************************************************************
*
*  DESCRIPTION:	Called during AOBJ_CHECKOBJECT and WM_INITDIALOG in SignalDlgProc
*	             	Check to see if the signals are tied to valid oids
*
*************************************************************************/
BOOL DDES::CheckSignals()
	{
	int 	i;
	BOOL bError = FALSE;
	SIGNALINFO *newSigs = NULL;
	int nSigs = 0;
	// Remove Signals that are tied to delete signals
	for (i = 0; i < nUserSignals; i++)
		{
		if (AObjCheckType(userSignals[i].rcvrOID, 0) == TRUE)
			{
			nSigs ++;
			SIGNALINFO *tmpSigs = new SIGNALINFO[nSigs]; // increment signal count and reallocate
			_fmemcpy(tmpSigs, newSigs, (nSigs -1));
			delete newSigs;  // delete oldSigs
			_fmemcpy((LPSTR)&(tmpSigs[nSigs - 1]), (LPSTR)&(userSignals[i]), sizeof(SIGNALINFO));
			newSigs  = tmpSigs;
			}
		}

	// if some signals are deleted
	if (nSigs != 	nUserSignals)
		{
		nUserSignals = nSigs;
		delete userSignals;
		userSignals = newSigs;
    bError = TRUE;
		}

	return bError;
	}


/***********************************************************
DDES class implementaion

************************************************************/
DDES::DDES(OBJECTID dObject)
  {
  int i, j;
  LPDDESDATA    lpData     = (LPDDESDATA)AObjLockData(dObject, data0_DDESDATA);
	LPDDESTOPICS  lpTopicsData = (LPDDESTOPICS)AObjLockData(dObject, data1_DDESTOPICS);
  LPDDESITEMS   lpItemsData = (LPDDESITEMS)AObjLockData(dObject, data2_DDESITEMS);
	LPDEFOBJDDATA lpDefData  =  (LPDEFOBJDDATA)AObjLockData(dObject, data3_DEFVALS);
	DDESTOPICLIST *lpTopicList = NULL;
	//Get the help text for this service
	LPSTR 				lpHelpData = (LPSTR) AObjLockData(dObject, data4_SYSHELP);
	helpMemID =   AMemAllocate(AObjGetDataSize(dObject,data4_SYSHELP));
	LPSTR lpHelp = (LPSTR)AMemLock(helpMemID);
	if (lpHelp)
		{
		lstrcpy(lpHelp, lpHelpData);
		AObjUnlockData(dObject, data4_SYSHELP);
		}
	ddesOID       = dObject;
  // get ddeinitialize Instance ID (init during STARTUP)
	idInst				= ATypeGetRuntimeInfo(OTYPE_DDES);
	fAppowned			= 0;
	RenderDelay		= 0;
	count					= 0;

  bSave         = 0;
  // init from data0 -- DDESDATA
  //hszService = 0;
  nUserSignals =  lpData->nUserSignals;
  nTopicSignals =   lpData->nTopicSignals;
	// init from data 0
  ddesService.nTopics = lpData->nTopics; // number of total services provided by this object

  BuildSignalsFromData();

	
  strncpy(ddesService.Service,lpData->Service, MAXNAME);

  // memblock to hold list of transaction structures
  XMemID = AMemAllocate(0);
  nXctions = 0;
  // init from data 1
  // allocate space to hold all topics and their items
  ddesService.topicListID = AMemAllocate(ddesService.nTopics * sizeof(DDESTOPICLIST));
  lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);

  if (lpTopicList)
    _fmemset(lpTopicList, 0, ddesService.nTopics * sizeof(DDESTOPICLIST));

  // init from data1 and data2
  // the ordering of topic names in data1 is same as the ordering of their items
  // in data2 i.e. lpTopicsData[i].topicName == lpItemList[i].TopicName
  int curItemIndex = 0;
  for (i = 0; i < lpData->nTopics; i++)
    {
    lpTopicList[i].nItems =  lpTopicsData->topicList[i].nItems;
    strncpy(lpTopicList[i].pszTopic, lpTopicsData->topicList[i].name, MAXNAME);

    // init from data2
    DDESITEMLIST *lpItemList;
    lpTopicList[i].itemListID = AMemAllocate(lpTopicsData->topicList[i].nItems * sizeof(DDESITEMLIST));
    lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[i].itemListID);

    if (lpItemList)
      _fmemset(lpItemList, 0,lpTopicsData->topicList[i].nItems * sizeof(DDESITEMLIST));

    for (j = 0; j <lpTopicList[i].nItems ; j++)
      {
			strncpy(lpItemList[j].pszItem, lpItemsData->itemList[j + curItemIndex].Item, MAXNAME);
      lpItemList[j].dataSrcOID = (OBJECTID)lpItemsData->itemList[j + curItemIndex].dataSrcOID;
      lpItemList[j].format = lpItemsData->itemList[j + curItemIndex].format; // formatInfo

      // lpItemList[j].objectType = (LONG)AObjGetType(lpItemList[j].dataSrcOID);
      }
    curItemIndex += j; // the next item comes from this index --> keep adding to the last topic's index position
		}


  strcpy(defTopic, lpDefData->defTopic);
  strcpy(defItem, lpDefData->defItem);
  strcpy(defFormat.name, lpDefData->defFormat.name);
  defFormat.atom = lpDefData->defFormat.atom;

  defUpdateTime = lpDefData->defUpdateTime;
	defDataSrcOID = lpDefData->defDataSrcOID;
	maxSignalID   = lpDefData->maxSignalID ; // SKJ Mar. 7, 1994



	_fmemcpy(&(CCFilter), &(lpData->CCFilter), sizeof(CONVCONTEXT));
	bDefContext = lpData->bDefContext;
	strcpy(Country, lpData->Country);
  strcpy(Language, lpData->Language);
	strcpy(CodePage, lpData->CodePage);

  AMemUnlock(ddesService.topicListID);

  // At runtime create a string handle for the service string that can lie around till we
  // shut down this string handle will be used for all ddeml func. calls
	if (idInst != DDES_NOTINIT)
    {
    Createhsz();
    }

	hConv = 0; // init the conv fiedl to NULL
  AObjUnlockData(dObject, data0_DDESDATA);
  AObjUnlockData(dObject, data1_DDESTOPICS);
  AObjUnlockData(dObject, data2_DDESITEMS);
  AObjUnlockData(dObject, data3_DEFVALS);

	}


/**************************************************************************
* function : ~DDES
* Description: Destructor for DDES class.  If the bSave flag is set, the fields
* of this instance are written out the DDES object Datas
* bSave = TRUE --> user has chosen 'OK' @OBJD
*         FALSE -> otherwise
***************************************************************************/
DDES::~DDES()
  {
  int i,j;
  LPDDESTOPICS  lpTopicsData;
  LPDDESITEMS   lpItemsData;
	long          itemsDataSize = AObjGetDataSize(ddesOID, data2_DDESITEMS);
  DDESTOPICLIST *lpTopicList = NULL;
	int           oldItemIndex = 0;

  if (bSave & DDES_SAVEOBJD) // only after OK'd OBJD edit
		{
		LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(ddesOID, data3_DEFVALS);

		AObjResizeData(ddesOID, data0_DDESDATA, sizeof(DDESDATA) + (nUserSignals + nTopicSignals) * sizeof(SIGNALINFO));
    // Save the signals to Datas
    SaveSignalsToData();

    LPDDESDATA   lpData = (LPDDESDATA)AObjLockData(ddesOID, data0_DDESDATA);
    lpData->ddesOID = ddesOID;
    // save into data0 -- DDESDATA
		// hszService = 0;

    lpData->nUserSignals =  nUserSignals;
		lpData->nTopicSignals =   nTopicSignals;

		// init from data 0
    lpData->nTopics = ddesService.nTopics; // number of total services provided by this object
		strncpy(lpData->Service, ddesService.Service, MAXNAME);

    // init data 1
    // allocate space to hold all topics and their items
    AObjResizeData(ddesOID, data1_DDESTOPICS, sizeof(DDESTOPICDATA) + ddesService.nTopics * sizeof(TOPICINFO));
    lpTopicsData = (LPDDESTOPICS)AObjLockData(ddesOID, data1_DDESTOPICS);
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);

    // init from data1 and data2
    // the ordering of topic names in data1 is same as the ordering of their items
    // in data2 i.e. lpTopicsData[i].topicName == lpItemList[i].TopicName
		oldItemIndex = 0;
		itemsDataSize = 0; // start from SYSTEM item
    for (i = 0; i < lpData->nTopics; i++)
      {   
      lpTopicsData->topicList[i].nItems = lpTopicList[i].nItems;
      strncpy(lpTopicsData->topicList[i].name, lpTopicList[i].pszTopic, MAXNAME);
			// init from data2
			DDESITEMLIST *lpItemList;
      // resize ItemsData to fit new list of Items (per Topic)
			itemsDataSize += lpTopicsData->topicList[i].nItems * sizeof(ITEMSINFO);
      AObjResizeData(ddesOID, data2_DDESITEMS, itemsDataSize);
      lpItemsData  = (LPDDESITEMS)AObjLockData(ddesOID, data2_DDESITEMS);
      lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[i].itemListID);

      for (j = 0; j <lpTopicList[i].nItems; j++)
        {
        strncpy(lpItemsData->itemList[j + oldItemIndex].Topic, lpTopicList[i].pszTopic, MAXNAME);
        strncpy(lpItemsData->itemList[j + oldItemIndex].Item, lpItemList[j].pszItem, MAXNAME);
        lpItemsData->itemList[j + oldItemIndex].dataSrcOID = (OBJECTID)lpItemList[j].dataSrcOID;
        lpItemsData->itemList[j + oldItemIndex].format = lpItemList[j].format;        // formatInfo
        }
			oldItemIndex += j; // Store the next item at this location
												 // The next item is saved at this index --> keep adding to the last topic's index position


      AObjUnlockData(ddesOID, data2_DDESITEMS);
      AMemUnlock(lpTopicList[i].itemListID);
      }
    strcpy(lpDefData->defTopic, defTopic);
    strcpy(lpDefData->defItem,  defItem);
    strcpy(lpDefData->defFormat.name,defFormat.name);
    lpDefData->defFormat.atom = defFormat.atom;
		lpDefData->defUpdateTime = defUpdateTime;
    lpDefData->defDataSrcOID = defDataSrcOID;
		lpDefData->maxSignalID  = 	maxSignalID;  // SKJ Mar. 7, 1994

    AMemUnlock(ddesService.topicListID);

		_fmemcpy(&(lpData->CCFilter), &(CCFilter), sizeof(CONVCONTEXT));
		_fstrncpy(lpData->Country, Country, MAXNAME);
		_fstrncpy(lpData->Language, Language, MAXNAME);
		_fstrncpy(lpData->CodePage, CodePage, MAXNAME);

		lpData->bDefContext = bDefContext;

		AObjUnlockData(ddesOID, data0_DDESDATA);
		AObjUnlockData(ddesOID, data1_DDESTOPICS);
		AObjUnlockData(ddesOID, data3_DEFVALS);

		// Save the Help files
		//Get the help text for this service
		long helpSize = AMemGetSize(helpMemID);
		AObjResizeData(ddesOID, data4_SYSHELP, helpSize);
		LPSTR 				lpHelpData = (LPSTR) AObjLockData(ddesOID, data4_SYSHELP);
		LPSTR 				lpHelp = (LPSTR)AMemLock(helpMemID);
		if (lpHelp)
			{
			lstrcpy(lpHelpData, lpHelp);
			}
		if (lpHelpData)
			AObjUnlockData(ddesOID, data4_SYSHELP);
		AMemUnlock(helpMemID);
		AMemFree(helpMemID);
		} // eo-bSave

	// At runtime destroy the string handle for the service string
	if (idInst != DDES_NOTINIT)   //check if to see if it is runtime
		{
		if (hConv)
			{
			DdeDisconnect(hConv); // disconnect the current conversation
			}
		Destroyhsz();
    }
    
	// free MEM BLOCKS
	lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);

  for (i = 0; i < ddesService.nTopics; i++)
    AMemFree(lpTopicList[i].itemListID);
  AMemUnlock(ddesService.topicListID);
	AMemFree(ddesService.topicListID);
	}




/*****************************************************************************
* GetALMData --> get the data from the src oid
*
*
*****************************************************************************/
BOOL GetALMData(OBJECTID itemID,
  int wFmt,
  MEMBLOCKID imemID)
  {
  BOOL  rVal= FALSE;
  switch (wFmt) {
    case CF_TEXT:
      if (itemID)
        {
        rVal = AObjConvertToText(itemID, imemID);
        }
      break;
    case CF_BITMAP:
      {
      LONG lHandle = 0;
      HANDLE hPict;
			OPictGetHandle(itemID, &lHandle );
      hPict = (HANDLE)lHandle;
      AMemSetSize(imemID, sizeof(HANDLE));
      LPHANDLE lpPtr = (LPHANDLE)AMemLock(imemID);
      *lpPtr = hPict;
      AMemUnlock(imemID);

      }
      break;
    case CF_METAFILEPICT:
    default:
      rVal = FALSE;
      break;
    }
  return rVal;
  }



/***************************** Private Function ****************************\
*  This creates often used global hszs from standard global strings.
*  It also fills the hsz fields of the topic and item tables.
*
\***************************************************************************/
void DDES::Createhsz()
  {
  WORD iTopic, iItem;
  ddesService.hszService = DdeCreateStringHandle(idInst, ddesService.Service, NULL);
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
  for (iTopic = 0; iTopic < ddesService.nTopics; iTopic++)
    {
    lpTopicList[iTopic].hszTopic = DdeCreateStringHandle(idInst, lpTopicList[iTopic].pszTopic, NULL);
    DDESITEMLIST *lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[iTopic].itemListID);
    for (iItem = 0; iItem < lpTopicList[iTopic].nItems; iItem++)
      {
      lpItemList[iItem].hszItem =
          DdeCreateStringHandle(idInst, lpItemList[iItem].pszItem, NULL);
      }
    AMemUnlock(lpTopicList[iTopic].itemListID);
    }
  AMemUnlock(ddesService.topicListID);
  }

/***************************** Private Function ****************************\
*  This destroys often used global hszs from standard global strings.
*
\***************************************************************************/
void DDES::Destroyhsz()
  {
  WORD iTopic, iItem;
  DdeFreeStringHandle(idInst, ddesService.hszService);
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
  for (iTopic = 0; iTopic < ddesService.nTopics; iTopic++)
    {
    DdeFreeStringHandle(idInst, lpTopicList[iTopic].hszTopic);
    DDESITEMLIST *lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[iTopic].itemListID);

    for (iItem = 0; iItem < lpTopicList[iTopic].nItems; iItem++)
      {
			DdeFreeStringHandle(idInst,   lpItemList[iItem].hszItem);
      }
    AMemUnlock(lpTopicList[iTopic].itemListID);
    }
  AMemUnlock(ddesService.topicListID);
  }

/***********************************************************************
*
*
*
************************************************************************/
HSZ DDES::GethszTopic(int index)
  {
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
  HSZ rVal = lpTopicList[index].hszTopic;
  AMemUnlock(ddesService.topicListID);
  return rVal;
	}

/***********************************************************************
*
* Check to see if the topic is defined
*
************************************************************************/
int DDES::IsSamehszTopic(HSZ hszTopic)
  {
  int i = 0;
  int curTopicIndex = DDES_TOPICNOTFOUND;
	ADBG_ASSERT_E(ddesService.topicListID, return DDES_TOPICNOTFOUND,"Error in DDES context during server callback");
	if (ddesService.topicListID == 0) // This should never happen, but it is a good error check
		return DDES_TOPICNOTFOUND;
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);

  for (i = 0; i < ddesService.nTopics; i++)
    {
    if (DdeCmpStringHandles(lpTopicList[i].hszTopic, hszTopic) == 0)
      {
      curTopicIndex = i;
      break;
			}
    }

  AMemUnlock(ddesService.topicListID);
  return curTopicIndex;
  }

/***********************************************************************
*
* 
*
************************************************************************/
int DDES::IsNewTopic(LPSTR Topic)
  {
  int i = 0;
  int curTopicIndex = DDES_TOPICNOTFOUND;
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);

  for (i = 0; i < ddesService.nTopics; i++)
    {

    if (strcmp(lpTopicList[i].pszTopic, Topic) == 0)
      {
      curTopicIndex = i;
      break;
      }
    }

  AMemUnlock(ddesService.topicListID);
  return curTopicIndex;
  }

int DDES::IsNewSignal(SIGNALINFO defSig)
  {
  int i = 0;
  int curSigIndex = DDES_SIGNALNOTFOUND;

  for (i = 0; i < nUserSignals; i++)
    {

		if (strcmp(userSignals[i].name, defSig.name) == 0)
      {
      curSigIndex = i;
      break;
      }
    }
  return curSigIndex;
  }




/************************************************************************************
* Check to see if the oid is tied to any service:topic:item
*
*************************************************************************************/
BOOL DDES::GetConvInfoFromOID(MEMBLOCKID topicID, MEMBLOCKID itemID, OBJECTID oid)
	{
  BOOL bRVal = FALSE;
	DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
	int 	topicIndex = 0;
	for (topicIndex = 0; topicIndex < ddesService.nTopics; topicIndex++)
		{
		DDESITEMLIST *lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[topicIndex].itemListID);
		int curItemIndex = DDES_ITEMNOTFOUND;
  	int   j = 0;
  	for (j = 0; j <lpTopicList[topicIndex].nItems; j++)
    	{
			if (oid == lpItemList[j].dataSrcOID)
				{
      	curItemIndex = j;
				break;
      	}
			}

		if (curItemIndex != DDES_ITEMNOTFOUND)
			{
			AMemSetSize(itemID, strlen(lpItemList[curItemIndex].pszItem) + 1);
			LPSTR lpStr = (LPSTR)AMemLock(itemID);
			strcpy(lpStr,lpItemList[j].pszItem);
			AMemUnlock(itemID);

			
			AMemSetSize(topicID, strlen(lpTopicList[topicIndex].pszTopic) + 1);
			lpStr = (LPSTR)AMemLock(topicID);
			strcpy(lpStr,lpTopicList[topicIndex].pszTopic);
			AMemUnlock(topicID);
			bRVal =TRUE;
			}

		AMemUnlock(lpTopicList[topicIndex].itemListID);
		if (curItemIndex != DDES_ITEMNOTFOUND)
    	break;
		}

	AMemUnlock(ddesService.topicListID);
  return bRVal;
  }


/************************************************************************************
* Check to see if the item is under a given topic
*
*************************************************************************************/

int DDES::IsSamehszItem(int topicIndex, HSZ hszItem, WORD wFormat, OBJECTID *oid)
  {
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
  DDESITEMLIST *lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[topicIndex].itemListID);
  int curItemIndex = DDES_ITEMNOTFOUND;
  int   j = 0;
  for (j = 0; j <lpTopicList[topicIndex].nItems; j++)
    {
    char lpBuffer[100];
    int cmpVal = DdeCmpStringHandles(lpItemList[j].hszItem, hszItem);
    DdeQueryString(idInst,hszItem, lpBuffer, 100, CP_WINANSI);
    DdeQueryString(idInst,lpItemList[j].hszItem, lpBuffer, 100, CP_WINANSI);

    if ((cmpVal == 0) && (lpItemList[j].format.atom == wFormat))
      {
			curItemIndex = j;
      *oid = lpItemList[j].dataSrcOID;
      break;
      }
    }
  AMemUnlock(lpTopicList[topicIndex].itemListID);
  AMemUnlock(ddesService.topicListID);
  return curItemIndex;
  }

/******************************************************************************************
* This function is called durin XTYP_CONFIRM_CONNECT.  We will set the user handle to contain
* the C++ object
*
******************************************************************************************/
DDES *DDES::SetContext(
  HCONV hConv,
  HSZ   hszService,
  HSZ   hszTopic,
  WORD  wFormat,
  WORD  wType)
  {
  DDES  *ddes = NULL;
  long i = -1;
  BOOL rVal = FALSE;
  OBJECTID  theObject;
  int topicIndex;
  while ((theObject = AObjGetNextObject(OTYPE_DDES, &i)) != NULL)
    {
    ddes = (DDES *)GetRunTimePtr(theObject, FALSE);
    if (DdeCmpStringHandles(ddes->ddesService.hszService, hszService) == 0)
      {
      if ( (topicIndex = ddes->IsNewTopic(ddes->defTopic)) != DDES_TOPICNOTFOUND) // -1
        {
				// stuff the ddes ptr in the hUser field of ConvInfo struct
				DdeSetUserHandle(hConv, QID_SYNC, (LONG)ddes);

				ddes->hConv = hConv;  // save the conversation handle
				// Post a signal for all requested conv. Conv. Established
				ddes->PostSignal(hszTopic, NULL, wFormat, wType);

        break;
        }
      }
    }
  I_UNUSED(rVal);
  I_UNUSED(topicIndex);
  I_UNUSED(hszTopic);
  I_UNUSED(wFormat);
  I_UNUSED(wType);
  return ddes;
  }



/***********************************************************************
*
* 
*
************************************************************************/
DDES *DDES::GetContextFromhConv(
  HCONV hConv)
  {
  DDES *ddes = NULL;
  CONVINFO convInfo;
  long i = -1;
  OBJECTID theObject;
  BOOL bFound  = FALSE;
  DdeQueryConvInfo(hConv, QID_SYNC, &convInfo);
  ddes = (DDES *)convInfo.hUser;

  // if the hconv context is null --> it has not been set yet
  if (ddes == NULL)
    {
    return ddes;
    }

  // This is a dumb check to see if we are getting one of the desired C++ objects
  while ((theObject = AObjGetNextObject(OTYPE_DDES, &i)) != NULL)
    {
		if (ddes == (DDES *)GetRunTimePtr(theObject, FALSE))
      {
      bFound = TRUE;
      break;
      }
    }

  if (bFound == FALSE) {
		ddes = NULL;
		ADBG_PRINT_W("ERROR in getting user handle from CONV");
    }

  return (DDES *)ddes;
  }

/***********************************************************************
*
* 
*
************************************************************************/
DDES *GetContextFromhConvTopic(
  HCONV hConv)
  {
  DDES *ddeS = NULL;
  CONVINFO convInfo;
  long i = -1;
  OBJECTID theObject;
  BOOL bFound  = FALSE;
  DdeQueryConvInfo(hConv, QID_SYNC, &convInfo);
  ddeS = (DDES *)convInfo.hUser;
  // This is a dumb check to see if we are getting one of the desired C++ objects
  while ((theObject = AObjGetNextObject(OTYPE_DDES, &i)) != NULL)
    {
    ddeS = (DDES *)AObjGetRuntimeInfo(theObject);

    if (DdeCmpStringHandles(ddeS->GetddesService().hszService, convInfo.hszServiceReq) == 0)
      {
      bFound = TRUE;
      break;
      }
		}

  if (bFound == FALSE) {
    ddeS = NULL;
		ADBG_PRINT_W("ERROR in getting user handle from CONV");
		}
  return (DDES *)ddeS;
  }


/***********************************************************************
*	FUNCTION:			RemoveSignalL
*	DESCRIPTION:	Delete a signal from ddes' signal list.  Delete the old list.
*								Recreate a new list.
*
************************************************************************/
BOOL DDES::RemoveSignal(int index)
  {
  int i;

  if ((index >= nUserSignals) || (index < 0))
    return FALSE;

  SIGNALINFO *tmpSignal = new SIGNALINFO[nUserSignals - 1];
  for (i = 0; i < index; i++)
    tmpSignal[i] = userSignals[i];

  for (i = index + 1; i < nUserSignals; i++)
    tmpSignal[i -1] = userSignals[i];

  nUserSignals --;
  delete userSignals;
  userSignals = tmpSignal;
  return TRUE;
  }

/***********************************************************************
*
* 
*
************************************************************************/
int DDES::RemoveItem(int delTopicIndex, int delItemIndex)
  {
  int j = 0;
  int i = 0;
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
  DDESITEMLIST *lpItemList;
  int nItems = lpTopicList[delTopicIndex].nItems;
  AMemUnlock(ddesService.topicListID);

  // if this is the only item under this topic or if user wants to delete the entire topic
  if ((delItemIndex == DDES_REMOVEALLITEMS) || (nItems == 1))
    {
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
    long topicSize = AMemGetSize(ddesService.topicListID);
    MEMBLOCKID tmpID = AMemAllocate(topicSize - sizeof(DDESTOPICLIST));
    DDESTOPICLIST *newList = (DDESTOPICLIST *)AMemLock(tmpID);
    for (j = 0; j < ddesService.nTopics; j++)
      {
      if (j != delTopicIndex)
        {
        _fmemcpy((LPSTR)&newList[i], (LPSTR)&lpTopicList[j], sizeof(DDESTOPICLIST));
        i++;
        }
      else
        {
        AMemFree(lpTopicList[j].itemListID); //delTopicIndex
        }
      }
    ddesService.nTopics --;  // reduce number of topics 
    AMemUnlock(ddesService.topicListID);
    AMemUnlock(tmpID);
    AMemFree(ddesService.topicListID);
    ddesService.topicListID = tmpID;
    }
  else {
    lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
    lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[delTopicIndex].itemListID);

    long itemSize = AMemGetSize(lpTopicList[delTopicIndex].itemListID);
		MEMBLOCKID tmpID = AMemAllocate(itemSize - sizeof(DDESITEMLIST));
    DDESITEMLIST *newList = (DDESITEMLIST *)AMemLock(tmpID);

    for (j = 0; j <lpTopicList[delTopicIndex].nItems; j++)
      {
      if (j != delItemIndex)
        {
        _fmemcpy((LPSTR)&newList[i], (LPSTR)&lpItemList[j], sizeof(DDESITEMLIST));
        i++;
        }
      }
    lpTopicList[delTopicIndex].nItems--;

    AMemUnlock(lpTopicList[delTopicIndex].itemListID);
    AMemFree(lpTopicList[delTopicIndex].itemListID);
    lpTopicList[delTopicIndex].itemListID = tmpID;
    AMemUnlock(tmpID);
    AMemUnlock(ddesService.topicListID);
    }

  return 1;
  }

/***********************************************************************
*
* 
*
************************************************************************/
DDES *GetNextDDES(long *count)
  {
  DDES *ddeS = NULL;
  OBJECTID theObject = AObjGetNextObject(OTYPE_DDES, count);
  if (theObject)
    {
    ddeS = (DDES *)GetRunTimePtr(theObject, FALSE);
    }
  return ddeS;
  }

/***********************************************************************
*
* 
*
************************************************************************/
DDESTOPICLIST *DDES::GetTopicList()
  {
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
  return lpTopicList;
  }

/***********************************************************************
*
* 
*
************************************************************************/
BOOL DDES::UnGetTopicList()
  {
  return AMemUnlock(ddesService.topicListID);
  }

/***********************************************************************
*
* 
*
************************************************************************/
DDESITEMLIST *DDES::GetItemList(int topicIndex)
  {
  DDESTOPICLIST *lpTopicList = (DDESTOPICLIST *)AMemLock(ddesService.topicListID);
  DDESITEMLIST *lpItemList = (DDESITEMLIST *)AMemLock(lpTopicList[topicIndex].itemListID);
  AMemUnlock(ddesService.topicListID);
  return lpItemList;
  }


/************************************************************************************
* Enum. all the active transaction that have a matching oid
*
*
************************************************************************************/
int DDES::EnumOIDXction(
	OBJECTID    oid,
  XCTIONINFO  *Xction,
  long        *count)
  {
  XCTIONINFO *lpXction =  (XCTIONINFO *)AMemLock(XMemID);
  long rVal = DDES_XCTIONNOTFOUND;
  int i = 0;
	for (i = (int)*count + 1; i < nXctions; i++)
    {
    *count = i;
		if ((lpXction[i].isValid) && (lpXction[i].srcOID == oid))
      {
      *Xction =  lpXction[i];
      rVal = i;
      break;
      }
    }
  AMemUnlock(XMemID);

	return (int)rVal;
  }




/*********************************************************************************************
*
*
*
**********************************************************************************************/

BOOL DDES::RemoveAllXctions(
  HCONV hConv
  )
  {
	int i = -1;
	long index = -1;
  BOOL bFound = FALSE;
  OBJECTID  theObject;
	DDES   *ddes = NULL;
	int 		nXctionCount = 0;

  while ((theObject = AObjGetNextObject(OTYPE_DDES, &index)) != NULL)
    {
		ddes = (DDES *)GetRunTimePtr(theObject, FALSE);
		XCTIONINFO *lpXction = (XCTIONINFO *)AMemLock(ddes->XMemID);
		for (i = 0; i < ddes->nXctions; i++)
      {
      if (lpXction[i].hConv == hConv)
        {
        // Need to delete the Xction record --- SKJ (??)
        lpXction[i].isValid = FALSE; // transacion is ON
        bFound = TRUE;
				}
			}
		nXctionCount += ddes->nXctions;
		if (bFound == TRUE)
      break;
    AMemUnlock(ddes->XMemID);
		}
	// if there are > 0 transactions and no matching transactions are found
	ADBG_ASSERT_E((nXctionCount > 0)?bFound:!bFound, ; , "ERROR in Removing TRANSACTION");
	return bFound;
  }


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////
int DDES::AddXction(PXFERINFO pXferInfo, OBJECTID oid)
  {
  // int   i = IsSamehszTopic(pXferInfo->hszTopic);
  // int    j = IsSamehszItem(i, pXferInfo->hszItem, pXferInfo->wFmt, &oid);
	ADBG_ASSERT_E(oid, return -1, "Error in Finding Matching Transaction");
	if (oid == 0)
		return -1;

  AMemSetSize(XMemID, AMemGetSize(XMemID) + sizeof(XCTIONINFO));
	XCTIONINFO *lpXction = (XCTIONINFO*)AMemLock(XMemID);
	lpXction[nXctions].hConv =  pXferInfo->hConv;
	lpXction[nXctions].hszTopic = pXferInfo->hszTopic;
	lpXction[nXctions].hszItem = pXferInfo->hszItem;
	lpXction[nXctions].srcOID = oid;
  lpXction[nXctions].isValid = TRUE; // transacion is ON
  lpXction[nXctions].ddes    = this; // keep the ddes context around for zpeed purposes
                                     // Also during certain transactions (EXECUTE) the user data
                                     // of a conv. handle does not seem to have the 'ddes' ptr
                                     // WHO CHANGES THIS?
	AMemUnlock(XMemID);
  return ++(nXctions);

  }



/*********************************************************************************************
*	Function:		PostSignal
*             Post a Signal based on transaction type
*
**********************************************************************************************/
long DDES::PostSignal(
	HSZ  	hszTopic,
	HSZ		hszItem,
	int		iFmt,
	int  	wType)
	{
	WORD i;
	LPDDESDATA lpData = (LPDDESDATA)AObjLockData(ddesOID, data0_DDESDATA);
	LPSTR pszTopic 	= 	new char[MAXNAME];
	LPSTR pszItem 	= 	new char[MAXNAME];
	LPSTR	itemString		=   new char[MAXNAME * 3];
	int index = DDES_TOPICNOTFOUND;
	long signalID = 0;
	int	offset = 0;
	DdeQueryString(idInst, hszTopic, (LPSTR) pszTopic, MAXNAME, CP_WINANSI);
	DdeQueryString(idInst, hszItem, (LPSTR) pszItem, MAXNAME, CP_WINANSI);

	sprintf(itemString, "%s:%s:%s", pszTopic, pszItem, 	aFormats[iFmt].name);

	for (i = 0; i < lpData->nTopicSignals; i++)
    {
    // for every topic add the following signals
    //      Conv. <topic> requested
    //      Conv. <topic> established
		//      Conv. <topic> stopped
		//			transaction requested
    //			data updated

		// if it is a topic name then the next 3 signals are
		if (strcmp(pszTopic, lpData->signals[i].name) == 0)
    	{
			index = i;
			break;
			}
		else 		if (strcmp(itemString, lpData->signals[i].name) == 0)
    	{ // if it is a topic:item:format string
			index = i;
			break;
			}
		}

	if (index != DDES_TOPICNOTFOUND)
		{
		switch (wType)
			{
			case XTYP_CONNECT:  														// conv. requested
			case XTYP_WILDCONNECT:
				{
        // do not post any signal for a Conv. request
				// offset = I_REQUESTCONV;
				break;
        }
			case XTYP_CONNECT_CONFIRM:
				offset = I_ESTABCONVSIG;
				break;

			case XTYP_DISCONNECT:
				offset = I_STOPCONVSIG;
				break;

			case XTYP_POKE:
				offset = I_NOSIGNAL;
				break;

			case XTYP_ADVSTART:
				{
				offset = I_REQUESTTRANSSIG;   // transaction on ITEM requested
				break;
				}
			case XTYP_REQUEST:
			case XTYP_ADVREQ:
				{
				offset = I_UPDATETRANSSIG;  	// ITEM data updated
				}
				break;
			default:
				break;
			}

		if (offset != I_NOSIGNAL)
			{
			signalID = lpData->signals[index + offset].sigID;  // transaction requested
			AEvtPostSignalAtMark(ddesOID, signalID);
      }

		}

	AObjUnlockData(ddesOID, data0_DDESDATA);
	delete pszTopic;
	delete pszItem;
	delete itemString;


	return signalID;
	}
////////////////////////////////////////////////////////////////////////////////////////////
// GetXctionData
// Get The Data assoc. with this transaction i.e. from the data Src ALM
//
////////////////////////////////////////////////////////////////////////////////////////////

HDDEDATA DDES::GetXctionData(
	PXFERINFO pXferInfo,
	OBJECTID  oid,
	WORD iFmt)
		{
		HDDEDATA hData;

		// Post SIgnals
		PostSignal(pXferInfo->hszTopic, pXferInfo->hszItem, iFmt, pXferInfo->wType);

		switch (pXferInfo->wType) {
			case XTYP_POKE:
				{
        // return the data passed in by the client
				return pXferInfo->hData; // SKJ , Feb. 18, 1995
				}

			case XTYP_REQUEST:
				// pszComment = "Count data requested.";
			case XTYP_ADVREQ:
				{
				BOOL fAppOwned = FALSE;
				MEMBLOCKID dataID = AMemAllocate(0);
				LPSTR      dataPtr;
				// Delay(RenderDelay, FALSE);
				GetALMData(oid, pXferInfo->wFmt, dataID);
				dataPtr = (LPSTR)AMemLock(dataID);
				hData = DdeCreateDataHandle(idInst, dataPtr, AMemGetSize(dataID), 0, pXferInfo->hszItem,
										pXferInfo->wFmt, fAppowned ? HDATA_APPOWNED : 0);
				AMemUnlock(dataID);

				I_UNUSED(fAppOwned);

				return(hData);
				}

			case XTYP_ADVSTART: {
				AddXction(pXferInfo, oid);
      	return (HDDEDATA)(1);
     	 }
    	case XTYP_ADVSTOP: {
      	// RemoveXction(pXferInfo->hConv);
				break;
       	}
			}

		I_UNUSED(iFmt);
  	return(0);
  	}

/***********************************************************************
*
* 
*
************************************************************************/
HDDEDATA ReturnData(WORD wType, HDDEDATA hDataRet)
  {
  /*
  * The table functions return a boolean or data.
  * It gets translated here.
  */
  switch (wType & XCLASS_MASK)
    {
    case XCLASS_DATA:
      return(hDataRet);
    case XCLASS_FLAGS:
      return (HDDEDATA) (hDataRet ? DDE_FACK : DDE_FNOTPROCESSED);
    case XCLASS_BOOL:
      return (HDDEDATA) (TRUE);
    default: // XCLASS_NOTIFICATION
      return(0);
    }
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

/*************************************************************************
*	Function: SetALMData
*	This method is called from dde callback and GetXctionData function while
*	processing the XTYP_POKE transaction (Dens Data function)
* The data sent by the client
*	application is passed in which is then set to a SERIUS (ALM) object
*	OBJECTS THAT CURRENTLY SUPPORT DATA IMPORT/EXPORT are:
*			1. Text
*************************************************************************/
int DDES::SetALMData(  PXFERINFO  lpXact,
  OBJECTID receiverOID, BOOL isHandle , BOOL bDestroyH)
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

		if (lpXact->wFmt == CF_TEXT)
			{
			dataSize  = DdeGetData(lpXact->hData, NULL, 0, 0);

			}
    else
			return DDES_FAILURE;
		
		hPData = (HANDLE) GlobalAlloc(GHND, (int)dataSize);
		pData = (LPSTR)GlobalLock(hPData);
		DdeGetData(lpXact->hData, pData, dataSize, 0L);

		}
	else {
		return DDES_FAILURE; 	// pData = (LPSTR)hData;
		}
	if (dataSize == 0)
		return DDES_FAILURE;

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

			if (AObjConvertFromText(receiverOID, textID))
				rVal = DDES_SUCCESS;
			else
				rVal = DDES_FAILURE;
			AMemUnlock(textID);
			AMemFree(textID);
			}
			break;

		case CF_BITMAP:
		case CF_METAFILEPICT:
			rVal =  DDES_FAILURE;
			break;

		default:  // cf_ALMOBJ
			rVal = DDES_FAILURE;

			break;
		}


	if (isHandle)
		{
	  GlobalUnlock(hPData);
		GlobalFree(hPData);
		if (bDestroyH)
			DdeFreeDataHandle(lpXact->hData);

		}

	return rVal;
	}

	/***********************************************************************************
*
*
*
***********************************************************************************/
HDDEDATA DDES::HandleTransactions(
  PXFERINFO pXferInfo,
  WORD  iFmt)
	{
	HDDEDATA hDataRet = 0;
  DDESTOPICLIST *lpTopicList;
  int   cItems;
  int   j;
  int    i = IsSamehszTopic(pXferInfo->hszTopic);
  OBJECTID  oid;
  // for (i = 0; i < CTOPICS; i++) \
  // {
  // if (DdeCmpStringHandles(topicList[i].hszTopic, hszTopic) == 0)
  //  {
  if (i != DDES_TOPICNOTFOUND)
    {
    /*
    * connections must be on a topic we support.
    */
    
    if (pXferInfo->wType == XTYP_CONNECT)
      {
      return (HDDEDATA)(ValidateContext((PCONVCONTEXT)pXferInfo->lData1));
      }

    lpTopicList = GetTopicList();
    cItems = lpTopicList[i].nItems;
    UnGetTopicList();

    j = IsSamehszItem(i, pXferInfo->hszItem, pXferInfo->wFmt, &oid);
    if (j != DDES_ITEMNOTFOUND)
      {
      /*
      * Make call to worker function here...
      */
			if (i == INDEX_SYSTEMTOPIC) // if it is the SYSTEM topic which is always at index '0'
				{
				// Call default functions to return data under predefined SYSTEM topic
				hDataRet = (*(SystemTopicItemList[j].npfnCallback))(pXferInfo, (DDES *)this, (int)j);

				}
			else
				{
				hDataRet = GetXctionData((PXFERINFO )pXferInfo, oid, iFmt);
				if (pXferInfo->wType == XTYP_POKE)
					SetALMData((PXFERINFO )pXferInfo,oid, TRUE , FALSE);
		  	 return ReturnData(pXferInfo->wType, hDataRet);
				}
			}
		}
  I_UNUSED(cItems);

  return hDataRet;
  }

/*****************************************************************************************************
* Find any dde server alm with a matching service
*
*
*****************************************************************************************************/
int IsSamehszService(
  HSZ hszService)
  {
  long count = -1;
  DDES *ddeS = NULL;
  while ((ddeS = GetNextDDES(&count)) != NULL)
    {
    if (DdeCmpStringHandles(ddeS->GetddesService().hszService, hszService) == 0)
      return (int)count;
    }
  return DDES_TOPICNOTFOUND;
  }

/***********************************************************************
* Exec Command format: <SIGNAL> '\t' <DATA> \0
* 
*
************************************************************************/
long DDES::GetSignal(
  LPSTR szExec)
  {
  int i = 0;
  long rVal = -1;
	szExec[MAX_EXEC - 1] = '\0';
  /* strtol converts string to long integer  */
	LPSTR lpTmp = strchr(szExec, '\t');
  int n = strlen(szExec) - strlen(lpTmp); // length of signal id string
  LPSTR lpData = &szExec[n + 1];
  szExec[n] = '\0';
  for (i = 0; i < nUserSignals; i++)
    {
    // compare the passed signal string and get its signalID
		if (strcmp(szExec, userSignals[i].name) == 0)
      {
			rVal = userSignals[i].sigID;
			OTextSetValueFromBuffer(userSignals[i].rcvrOID, lpData);
			break;
      }
    }

  I_UNUSED(lpData);
  return (long)rVal;
  }
  
/**************************************************************************
*Method:      HandleExec
*  
*Description: Processes the XTYP_EXECUTE
*
**************************************************************************/
BOOL DDES::HandleExec(
  HDDEDATA hData)
	{
		// Format is assumed to be CF_TEXT.
	long 	signalID = -1L;
	long 	size = DdeGetData(hData, NULL, MAX_EXEC, 0);

	char	*szExec = new char[MAX_EXEC];
  // Format is assumed to be CF_TEXT.
	DdeGetData(hData, (LPBYTE)szExec, MAX_EXEC, 0);
	if ((signalID = GetSignal(szExec)) > 0)
		{ 
		AEvtPostSignalAtMark(ddesOID, signalID);
		}
	I_UNUSED(size);
	delete szExec;
  return TRUE;
  }

/***************************** Public  Function ****************************\
*
* This function is called by the DDE manager DLL and passes control onto
* the apropriate function pointed to by the global topic and item arrays.
* It handles all DDE interaction generated by external events.
*
\***************************************************************************/
extern "C" HDDEDATA EXPENTRY _export DdeCallback(
	WORD wType,
	WORD wFmt,
  HCONV hConv,
  HSZ hszTopic,
  HSZ hszItem,
  HDDEDATA hData,
  DWORD lData1,
  DWORD lData2)
  {
  int i, j;
  long count = -1;
	WORD iFmt;
  HDDEDATA hDataRet;
  DDES     *ddeS = NULL;
	DDESTOPICLIST *lpTopicList = NULL;
	DWORD	idInst = ATypeGetRuntimeInfo(OTYPE_DDES);

  // when another application is unregistering  
  if (wType == XTYP_UNREGISTER)
    return 0;

  // Bet the proper C++ context from the hConv handle -- SKJ
  if (hConv) {
    ddeS = (DDES *)DDES::GetContextFromhConv(hConv);
    if ((wType == XTYP_EXECUTE) && (ddeS == NULL))
      {
			ADBG_ASSERT_E(ddeS, ;,"DDES == NULL during XTYP_EXEC");
      ddeS = GetContextFromhConvTopic(hConv);
			// this should not happen
      if (ddeS == NULL)
        return DDE_FNOTPROCESSED;
      }
    }

  /*
  * Block this callback if its blockable and we are supposed to.
  */
  /* if (fBlockNextCB && !(wType & XTYPF_NOBLOCK)) {
    fBlockNextCB = FALSE;
    fAllEnabled = FALSE;
    return (HDDEDATA)(CBR_BLOCK);
		}
	*/

	/*
	* Block this callback if its associated with a conversation and we
	* are supposed to.
	*/

	/*
	if (fTermNextCB && hConv) {
		fTermNextCB = FALSE;
		DdeDisconnect(hConv);
		wType = XTYP_DISCONNECT;
		}
	*/

	/*
	* Keep a count of connections
	*/
	if (wType == XTYP_CONNECT_CONFIRM) {
		DDES::SetContext(hConv,  hszItem, hszTopic,wFmt, wType);
		DDES::cServers++;
		return(0);
		}
	if (wType == XTYP_DISCONNECT) {
		DDES::cServers--;
		ADBG_ASSERT_E(ddeS, return 0;,"DDES == NULL during XTYP_DISCONNECT");
		if (ddeS == NULL) // SKJ Aug.3, 1994
			return 0;
		ddeS->hConv = 0;
		// SKJ, remove all transaction under this Conversation
		ddeS->RemoveAllXctions(hConv);
		ddeS->PostSignal(hszTopic, NULL, wFmt, wType);
		return(0);
		}


	/*
	* only allow transactions on the formats we support if they have a format.
	*/
	if (wFmt) {
		for (iFmt = 0; iFmt < CFORMATS; iFmt++) {
			if (wFmt == aFormats[iFmt].atom)
				break;
			}
		if (iFmt == CFORMATS)
			return(0);          // illegal format - ignore now.
		}

	/*
	* Executes are allowed only on the system topic.  This is a general
	* convention, not a requirement.
	*
	* Any executes received result in the execute text being shown in
	* the server client area.  No real action is taken.
	*/
	if (wType == XTYP_EXECUTE) {
		if (ddeS->IsSamehszTopic(hszTopic) != DDES_TOPICNOTFOUND) { // == topicList[0].hszTopic) {    // must be on system topic
			hDataRet = (HDDEDATA)ddeS->HandleExec(hData);
			hDataRet = (HDDEDATA)TRUE;
			return ReturnData(wType, hDataRet);
			}
		else {
			char errStr[40];
			sprintf(errStr, "Error during type:%d transaction", wType);
			I_PRINT(errStr);
			// ADBG_ASSERT_E(FALSE ,;, errStr);
			// pszComment = "Execute received on non-system topic - ignored";
			return(0);
			}
		}

	/*
	* Process wild initiates here
	*/
	if (wType == XTYP_WILDCONNECT) \
		{
		HSZ ahsz[(CTOPICS + 1) * 2];
		/*
		* He wants a hsz list of all our available app/topic pairs
		* that conform to hszTopic and hszItem(App).
		*/
		// Check the Service name
		if ((hszItem != NULL) &&  (IsSamehszService(hszItem) == DDES_TOPICNOTFOUND))
			{
			// we only support the hszAppName service
			return(0);
			}
		// scan the topic table and create hsz pairs
		j = 0;
		if (ddeS) // if we have a valid service and topic name
			{
			if (!ddeS->ValidateContext((PCONVCONTEXT)lData1)) {
				return(FALSE);
				}

			for (i = 0; i < ddeS->GetddesService().nTopics; i++) {
				if (hszTopic == NULL || (i == ddeS->IsSamehszTopic(hszTopic))) //  hszTopic == topicList[i].hszTopic) {
					{
					ahsz[j++] = ddeS->GetddesService().hszService;
					ahsz[j++] = ddeS->GethszTopic(i); //hszTopic;
					// post a signal for all wild connect topics
					ddeS->PostSignal(ddeS->GethszTopic(i), NULL, 0,XTYP_CONNECT);
					}
				}
			}
		else
			{ // loop thro' all available services and topics
			count = -1;
			while ((ddeS = GetNextDDES(&count)) != NULL)
				{
				if (!ddeS->ValidateContext((PCONVCONTEXT)lData1)) {
					return(FALSE);
					}

				for (i = 0; i < ddeS->GetddesService().nTopics; i++) {
					if ((hszTopic == NULL) || (i == ddeS->IsSamehszTopic(hszTopic))) //  hszTopic == topicList[i].hszTopic) {
						{
						ahsz[j++] = ddeS->GetddesService().hszService;
						ahsz[j++] = ddeS->GethszTopic(i); //hszTopic;
						// post a signal for all wild connect topics
						ddeS->PostSignal(ddeS->GethszTopic(i), NULL, 0,XTYP_CONNECT);

						}
					}
				}
			} // eo else
		// cap off the list with 0s
		ahsz[j++] = ahsz[j++] = 0L;
		// send it back
		return(DdeCreateDataHandle(idInst, (LPBYTE)&ahsz[0], sizeof(HSZ) * j, 0L, 0, wFmt, 0));
		}

	/*
	* Check our hsz tables and send to the apropriate proc. to process.
	* We use DdeCmpStringHandles() which is the portable case-insensitive
	* method of comparing string handles.  (this is a macro on windows so
	* there is no real speed hit.)  On WINDOWS, HSZs are case-insensitive
	* anyway, but this may not be the case on other platforms.
	*/
	count = -1;
	XFERINFO XferInfo;
	XferInfo.wType =  wType;
	XferInfo.wFmt =   wFmt;
	XferInfo.hConv =  hConv;
	XferInfo.hszTopic = hszTopic;
	XferInfo.hszItem = hszItem;
	XferInfo.hData = hData;
	XferInfo.lData1 = lData1;
	XferInfo.lData2 = lData2;

	if (ddeS)
		{

    hDataRet = ddeS->HandleTransactions(&XferInfo,iFmt);
    return hDataRet;
    }
  else {
    count = -1;
    while ((ddeS = GetNextDDES(&count)) != NULL)
      {
      hDataRet = ddeS->HandleTransactions(&XferInfo, iFmt);
      if (hDataRet) {
        return hDataRet;
        }
      }
    }

  /*
  * anything else fails - DDEML is designed so that a 0 return is ALWAYS ok.
  */
  I_UNUSED(ddeS);
	I_UNUSED(lpTopicList);

  return(0);
  }





/***************************** Private Function ****************************\
* This passes out a standard tab-delimited list of topic names for this
* application.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
\***************************************************************************/
HDDEDATA TopicListXfer(
  PXFERINFO pXferInfo,
  DDES  *ddes,
  int   itemIndex)
  {
    WORD cbAlloc, i;
    LPSTR pszTopicList;
    HDDEDATA hData = 0;
    DDESTOPICLIST *lpTopicList = ddes->GetTopicList();

    if (pXferInfo->wType == XTYP_ADVSTART)
        return (HDDEDATA)(TRUE);

    if (pXferInfo->wType != XTYP_REQUEST &&
            pXferInfo->wType != XTYP_ADVREQ)
        return(0);
    /*
     * construct the list of topics we have
     */
    cbAlloc = 0;
    for (i = 0; i < ddes->GetddesService().nTopics; i++)
        cbAlloc += lstrlen(lpTopicList[i].pszTopic) + 1;  // 1 for tab

		if (cbAlloc)
      {
			// allocate a data handle big enough for the list.
    	hData = DdeCreateDataHandle(ddes->idInst, NULL, 0, cbAlloc, pXferInfo->hszItem,
            pXferInfo->wFmt, 0);
    	pszTopicList = (LPSTR)DdeAccessData(hData, NULL);
    	if (pszTopicList)
      	{
      	for (i = 0; i < ddes->GetddesService().nTopics; i++) {
        	_fstrcpy(pszTopicList, lpTopicList[i].pszTopic);
        	pszTopicList += strlen(lpTopicList[i].pszTopic);
        	*pszTopicList++ = '\t';
        	}
      	*--pszTopicList = '\0';
				DdeUnaccessData(hData);
        }
			}
		ddes->UnGetTopicList(); // release lock on topicList
		I_UNUSED(itemIndex);
		return(hData);
		}




/***************************** Private Function ****************************\
* This passes out a standard tab-delimited list of item names for the
* specified topic.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
\***************************************************************************/
HDDEDATA SysItemListXfer(
  PXFERINFO pXferInfo,
  DDES 	*ddes,
	int  	itemIndex)
	{
	WORD 			size, i, nItems;
	LPSTR 		pszT;
	HDDEDATA 	hData = 0;

  if (pXferInfo->wType == XTYP_ADVSTART)
  	return (HDDEDATA)(TRUE);

  if (pXferInfo->wType != XTYP_REQUEST &&
    pXferInfo->wType != XTYP_ADVREQ)
		return(0);
  /*
  * construct the list of items we support for this topic - this supports
  * more than the minimum standard which would support SysItems only on
  * the system topic.
  */
	LPDDESTOPICS  lpTopicsData = (LPDDESTOPICS)AObjLockData(ddes->ddesOID, data1_DDESTOPICS);
	LPDDESITEMS   lpItemsData = (LPDDESITEMS)AObjLockData(ddes->ddesOID, data2_DDESITEMS);

	// locate the requested topic item table
	nItems = lpTopicsData->topicList[INDEX_SYSTEMTOPIC].nItems; //  items under SYSTEM topic (0 index)
	size = 0;

	for (i = 0; i < nItems; i++)
		{
		size += strlen(lpItemsData->itemList[i].Item) + 1; // null terminated
		}

	size += 1; // extra null at the end
	hData = DdeCreateDataHandle(ddes->idInst, NULL, (DWORD)size,
  		0L, pXferInfo->hszItem, pXferInfo->wFmt, 0);

	pszT = (LPSTR)DdeAccessData(hData, NULL);
  size = 0;
	for (i = 0; i < nItems; i++)
		{
		int slen = strlen(lpItemsData->itemList[i].Item);
		strncpy((LPSTR)(pszT + size), (LPSTR)lpItemsData->itemList[i].Item, slen);
		size += slen;
		pszT[size] = '\t';
		size += 1;
		}
	if (nItems)  // if at least 1 or item
		pszT[size] = '\0';
	DdeUnaccessData(hData);

	AObjUnlockData(ddes->ddesOID, data2_DDESITEMS);
	AObjUnlockData(ddes->ddesOID, data1_DDESTOPICS);

  I_UNUSED(ddes);
	I_UNUSED(itemIndex);
	return(hData);
	}


/***************************** Private Function ****************************\
* Gives out a 0 terminated array of dde format numbers supported by this app.
*
* This support is required for other apps to be able to
* find out about us.  This kind of support should be in every DDE
* application.
*
\***************************************************************************/
HDDEDATA SysFormatsXfer(
  PXFERINFO pXferInfo,
  DDES *ddes,
  int  itemIndex)
  {
  int i, cb;
  LPSTR psz, pszT;
  HDDEDATA hData = 0;

  if (pXferInfo->wType == XTYP_ADVSTART)
  	return (HDDEDATA)(TRUE);

  if (pXferInfo->wType != XTYP_REQUEST &&
  	pXferInfo->wType != XTYP_ADVREQ)
    return(0);

  for (i = 0, cb = 0; i < CFORMATS; i++)
  	cb += strlen(aFormats[i].name) + 1;

  hData = DdeCreateDataHandle(ddes->idInst, NULL, (DWORD)cb,
            0L, pXferInfo->hszItem, pXferInfo->wFmt, 0);
	psz = pszT = (LPSTR)DdeAccessData(hData, NULL);
	for (i = 0; i < CFORMATS; i++)
		{
    _fstrcpy(pszT, aFormats[i].name);
    pszT += _fstrlen(pszT);
    *pszT++ = '\t';
		}
	*(--pszT) = '\0';
	DdeUnaccessData(hData);

  I_UNUSED(psz);
  I_UNUSED(ddes);
  I_UNUSED(itemIndex);
  return(hData);
  }




/***********************************************************************
*
* 
*
************************************************************************/

HDDEDATA HelpXfer(
  PXFERINFO pXferInfo,
  DDES  *ddes,
  int   itemIndex)
	{

  LPSTR psz, pszT;
  HDDEDATA hData = 0;

	if (pXferInfo->wType == XTYP_ADVSTART)
  	return (HDDEDATA)(TRUE);

  if (pXferInfo->wType != XTYP_REQUEST &&
  		pXferInfo->wType != XTYP_ADVREQ)
    return(0);

	DWORD size = AMemGetSize(ddes->helpMemID);

	if (size)
	  { 
		LPSTR lpHelp = (LPSTR) AMemLock(ddes->helpMemID);
		hData = DdeCreateDataHandle(ddes->idInst, NULL, (DWORD)size,
  		0L, pXferInfo->hszItem, pXferInfo->wFmt, 0);
		psz = pszT = (LPSTR)DdeAccessData(hData, NULL);
		_fstrcpy(pszT, lpHelp);
		*(--pszT) = '\0';
		DdeUnaccessData(hData);
		if (lpHelp)
			AMemUnlock(ddes->helpMemID);

		}



  I_UNUSED(psz);
	I_UNUSED(itemIndex);
	I_UNUSED(ddes);
	return(hData);

	}


/***********************************************************************
*
* 
*
************************************************************************/

HDDEDATA SignalsXfer(
  PXFERINFO pXferInfo,
  DDES  *ddes,
  int   itemIndex)
  {
	LPSTR theSignals;
	int nlength = 0;
	int i = 0;
	HDDEDATA hData = 0;

	if (pXferInfo->wType == XTYP_ADVSTART)
  	return (HDDEDATA)(TRUE);

  if (pXferInfo->wType != XTYP_REQUEST &&
  		pXferInfo->wType != XTYP_ADVREQ)
		return(0);

	LPDDESDATA lpData = (LPDDESDATA)AObjLockData(ddes->ddesOID, data0_DDESDATA);

	nlength = 0;

	// find length of all user defined signals
	// format signal1\tsignal2\tsignal3\0\0
	for (i = 0; i < lpData->nUserSignals; i++)
		{
		nlength += strlen(lpData->signals[i + lpData->nTopicSignals].name) + 1;
		}

  nlength +=1 ; // extra null at the end
	hData = DdeCreateDataHandle(ddes->idInst, NULL, (DWORD)nlength,
  		0L, pXferInfo->hszItem, pXferInfo->wFmt, 0);
	theSignals = (LPSTR)DdeAccessData(hData, NULL);

	nlength = 0;
	for (i = 0; i < lpData->nUserSignals; i++)
		{
		sprintf((LPSTR)&theSignals[nlength], "%s", (LPSTR) &(lpData->signals[i + lpData->nTopicSignals].name));
		nlength = strlen(lpData->signals[i + lpData->nTopicSignals].name);
		theSignals[nlength] = '\t'; // pad the end with a tab
		theSignals[nlength + 1] = '\0';
		}

	// pad 2 nulls at the end
	if (nlength >= 2)
    {
		theSignals[nlength] = '\0';
		theSignals[nlength + 1] = '\0';
		}
	DdeUnaccessData(hData);

	I_UNUSED(ddes);
  I_UNUSED(pXferInfo);
  I_UNUSED(itemIndex);

	AObjUnlockData(ddes->ddesOID, data0_DDESDATA);
	return(hData);

  }

/***********************************************************************
*
* 
*
************************************************************************/

HDDEDATA AllItemsXfer(
  PXFERINFO pXferInfo,
  DDES  *ddes,
  int   itemIndex)
  {
	int i;
	LPSTR pszT;
  HDDEDATA hData = 0;

	if (pXferInfo->wType == XTYP_ADVSTART)
  	return (HDDEDATA)(TRUE);

  if (pXferInfo->wType != XTYP_REQUEST &&
  		pXferInfo->wType != XTYP_ADVREQ)
    return(0);

	LPDDESITEMS   lpItemsData = (LPDDESITEMS)AObjLockData(ddes->ddesOID, data2_DDESITEMS);
	DWORD size = AObjGetDataSize(ddes->ddesOID, data2_DDESITEMS);
	int nItems = (int) (size / sizeof(ITEMSINFO));

  size = 0;
	for (i = 0; i < nItems; i++)
		{
		size += strlen(lpItemsData->itemList[i].Item) + 1; // null terminated
		}

	size += 1; // extra null at the end
	hData = DdeCreateDataHandle(ddes->idInst, NULL, (DWORD)size,
  		0L, pXferInfo->hszItem, pXferInfo->wFmt, 0);

	pszT = (LPSTR)DdeAccessData(hData, NULL);
  size = 0;
	for (i = 0; i < nItems; i++)
		{
		int slen = strlen(lpItemsData->itemList[i].Item);
		strncpy((LPSTR)(pszT + (int)size), (LPSTR)lpItemsData->itemList[i].Item, slen);
		size += slen;
		pszT[(int)size] = '\t';
		size += 1;
		}

	if (nItems)  // if at least 1 or item
		pszT[(int)size] = '\0';
	DdeUnaccessData(hData);

	AObjUnlockData(ddes->ddesOID, data2_DDESITEMS);

	I_UNUSED(itemIndex);
	I_UNUSED(ddes);
	I_UNUSED(pXferInfo);
  I_UNUSED(itemIndex);

	return(hData);
	}


