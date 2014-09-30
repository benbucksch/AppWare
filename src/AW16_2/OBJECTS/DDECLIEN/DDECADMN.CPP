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
// FILE:       		ddesAdmn.cpp
//
// AUTHOR:     		Shaili Jain
//
// DESCRIPTION:   Handles all the ADMN events
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT


#define MAINMODULE 1

#include "resource.h"
#include <string.h>
#include <memory.h>                                                        
#include "stdlib.h"
#include <mem.h>
#include "ddemlcl.h"
#include  "runtime.h"
#include 	"o_text.h"
#include 	"o_number.h"


#define VERSIONOK 	 	1
#define VERSIONERR		2

typedef DDECDATA 		*LPDDECDATA;
typedef DDECOPTIONS *LPDDECOPTIONS;
extern int _WinAllocFlag;
extern long FAR PASCAL EditClientObject(OBJECTID theObject);

/****************************************************************************
*	Function: Update Object
*	Desc:     This function will check the version on the DDEC object
*						July 1993 - version 1
*
****************************************************************************/
long UpdateObject(OBJECTID source, OBJECTID dest)
	{
	long  rVal = A_OK;
	LPDDECDATA    lpSrcData = (LPDDECDATA)AObjLockData(source, data0_DDECDATA); 
	LPDDECDATA    lpDestData = (LPDDECDATA)AObjLockData(dest, data0_DDECDATA);

	if ((lpSrcData->version == DDEC_CURRENTDATAVERSION) && (lpSrcData->version == lpDestData->version))
		rVal = A_OK;
	else
		rVal = A_ERROR;
	AObjUnlockData(source, data0_DDECDATA);
	AObjUnlockData(dest, data0_DDECDATA);
	return rVal;
  }

/*****************************************************************************
*	FUNCTION: ADMN PROC for DDEC Client Object
* 					Entry points for Appware Bus
*
*****************************************************************************/
extern "C" long FAR PASCAL _export ddeClientADMN(
	OBJECTID					theObject,
	pAObjMessage 			system)
	{
	long							rVal = A_OK;
	OBJECTID 					sObject, dObject;
  AEvtInfo 					tEvent;
	char 							errorString[255];
	errorString[0] = 	'\0';

	switch (system->message1)
		{

		case  AOBJ_CREATED:  // Create a new instance of DDEC
			{
	
			dObject = (OBJECTID)theObject;
			AObjSetDataCount(dObject, MAXDDECLIENTDATAS);
			AObjResizeData(dObject, data0_DDECDATA, sizeof(DDECDATA));
			LPDDECDATA lpData = (LPDDECDATA)AObjLockData(dObject, data0_DDECDATA);
			AObjResizeData(dObject, data1_DDECOPTIONS, sizeof(DDECOPTIONS));
			LPDDECOPTIONS lpOptions = (LPDDECOPTIONS)AObjLockData(dObject, data1_DDECOPTIONS);

			lpData->version = DDEC_CURRENTDATAVERSION;
			strcpy(lpData->ServerName, USER_DEFNAME);
			strcpy(lpData->Service, USER_DEFNAME);
			strcpy(lpData->Topic, USER_DEFNAME);
			strcpy(lpData->Item, USER_DEFNAME);
			strcpy(lpData->Format, CFTEXT);
			lpData->ObjectType = OTYPE_TEXT; // default type
			lpData->objectID = 0;
			lpData->linkType = AUTO_LINK; // default link
			ATypeGetName(lpData->ObjectType, lpData->typeName, MAXNAME);
			lpData->objectName[0] = '\0';

			// AObjGetName(lpData->objectID, lpData->objectName, MAXNAME);


			lpData->bDisplayErr = FALSE;
			_fmemset(&(lpOptions->CCFilter), 0, sizeof(CONVCONTEXT));
			GetProfileString("intl", "sCountry", "United States", lpOptions->Country, MAXNAME);
			GetProfileString("intl", "sLanguage", "U.S. English",lpOptions->Language, MAXNAME);

			strcpy(lpOptions->Language, "Modern English");
			strcpy(lpOptions->CodePage,"ANSI");
			lpOptions->CCFilter.iCodePage  = CP_WINANSI;
			lpOptions->CCFilter.wCountryID = GetProfileInt("intl", "iCountry", 1);
			lpOptions->CCFilter.dwLangID = atol(lpOptions->Language);
			lpOptions->CCFilter.cb = sizeof(CONVCONTEXT);
			lpOptions->CCFilter.dwSecurity = 0;
			

			lpOptions->timeout = MIN_SYNCTIMEOUT;
			lpOptions->advDelay = MIN_SYNCTIMEOUT;
			if (system->message4 == AOBJ_TEMPORARY)
				{
				;
				}

			AObjUnlockData(dObject, data0_DDECDATA);
			AObjUnlockData(dObject, data1_DDECOPTIONS);

				
		 	}
			break;

		case AOBJ_ASSIGNOBJECT:
			{
			int i;
			// SKJ, param were swtiched for ASSIGN
			dObject = (OBJECTID)(theObject);    // source
			sObject = (OBJECTID)(system->message3);    // destination

			if (sObject == dObject)
				break;
			if (UpdateObject(sObject, dObject) != A_OK)
				{
				lstrcpy(system->errorMessage, "AOBJ_ASSIGNOBJECT failed : DDE CLIENT/tDDEC");
				return A_ERROR;
				}

			for (i = 0; i < MAXDDECLIENTDATAS ; i++)
				{
			 	LPSTR lpSrc = (LPSTR)AObjLockData(sObject, i);
				long size = AObjGetDataSize(sObject, i);
				AObjResizeData(dObject, i, size);
				LPSTR lpDst = (LPSTR)AObjLockData(dObject, i);
				hmemcpy(lpDst, lpSrc, size);
				AObjUnlockData(sObject, i);
				AObjUnlockData(dObject,i);
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
			dObject = (OBJECTID)theObject;
			system->message4 = A_OK;
			system->message3 = (LONG)DDEC_CURRENTDATAVERSION; // return current data Version
			}
			break;

	 	case AOBJ_IMPORTOBJECT:  // no equivalent alm on mac
			break;

		case AOBJ_EXPORTOBJECT:
			break;

		case AOBJ_EDITOBJECT:
			{
			rVal = EditClientObject(theObject);
			}
			break;

		case AOBJ_CHECKOBJECT: {
			// AObjInfo *objInfoRec = (AObjInfo *)theObject;
			BOOL bError = FALSE;
			LPDDECDATA lpData = (LPDDECDATA)AObjLockData(theObject, data0_DDECDATA);

			if ((lpData->objectID <= INVALIDID) || !(AObjCheckType(lpData->objectID, 0))) {
				bError = TRUE;
				lpData->objectID = INVALIDID;
				}
			if (bError) {
        rVal = A_ERROR;
				// objInfoRec->errorCode = objectCritical;
				AObjReportError	(	theObject,
													AOBJ_CHECKOBJECT,
													A_ERROR,
													"The DDEC client object should be connected to a valid object",
													0);
				break;
				}
			AObjUnlockData(theObject, data0_DDECDATA);
			// objInfoRec->errorCode = objectHealthy;
			}
			break;

		case AOBJ_VALIDATEOBJECTIDS: {
			LPDDECDATA lpData = (LPDDECDATA)AObjLockData(theObject, data0_DDECDATA);
			AObjValidateID( &(lpData->objectID), system);
			AObjUnlockData(theObject, data0_DDECDATA);
			}
			break;

		case AOBJ_BUILDSIGNALS:
			{
			int i;
			MEMBLOCKID sigsBlockID = (MEMBLOCKID)system->message2;
			pAObjSignalRecord theSigs;
			LPDDECDATA lpData = (LPDDECDATA)AObjLockData(theObject, data0_DDECDATA);

			AMemSetSize(sigsBlockID, MAX_SIGNALS * sizeof(AObjSignalRecord));
			theSigs = (pAObjSignalRecord)(LPSTR)AMemLock(sigsBlockID);

			i = 0;
			theSigs[i].theID = SIGDDEC_CONVERSSTARTED;
			lstrcpy(theSigs[i].theName, "Conversation Started");

			i++;
			theSigs[i].theID = SIGDDEC_CONVERSSTOPPED;
			lstrcpy(theSigs[i].theName, "Conversation Terminated");

			i++;
			theSigs[i].theID = SIGDDEC_AUTOTRANSSTARTED;
			lstrcpy(theSigs[i].theName, "Auto/Notify Transaction Started");

			i++;
			theSigs[i].theID = SIGDDEC_AUTOTRANSSTOPPED;
			lstrcpy(theSigs[i].theName, "Auto/Notify Transaction Stopped");

			i++;
			theSigs[i].theID = SIGDDEC_MANTRANSCOMPLETED;
			lstrcpy(theSigs[i].theName, "Manual Transaction Completed");

			i++;
			theSigs[i].theID = SIGDDEC_ADVDATACHANGED;
			lstrcpy(theSigs[i].theName, "AutoLink-Data Changed");

			i++;
			theSigs[i].theID = SIGDDEC_REQDATACHANGED;
			lstrcpy(theSigs[i].theName, "NotifyLink-Data Changed");
			AObjUnlockData(theObject, data0_DDECDATA);
			AMemUnlock(sigsBlockID);
			I_UNUSED(lpData);
			}
		  break;

		default:
	 		rVal = A_NOTHANDLED;
			break;
		}

	return rVal;
	}




/****************************************************************************
*	Function: LibMain
*	Desc:     Every DLL has an entry point LibMain and an exit point WEP.
*						July 1993 - version 1
*
****************************************************************************/
#pragma argsused
extern "C" int FAR PASCAL _export LibMain(
	HINSTANCE 		hInstance,
	WORD 					wDataSegment,
	WORD 					wHeapSize,
	LPSTR 				lpszCmdLine)
	{
  _WinAllocFlag = GMEM_SHARE;

	// The startup code for the DLL initializes the local heap (if there is one)
	// with a call to LocalInit which locks the data segment.
	if ( wHeapSize != 0 )
		UnlockData( 0 );

	hInst = hInstance;

	// Register Formats
	InitializeFormats();

	return 1;   // Indicate that the DLL was initialized successfully.
	}

/***************************************************************************
* FUNCTION:	WEP
*
*
****************************************************************************/
int FAR PASCAL WEP ( int bSystemExit )
	{
	I_UNUSED(bSystemExit);
	return 1;
	}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitializeFormats ()                                   		*	
 *                                                                          *
 *  PURPOSE    : Registers the Link clipboard format                   			*
 *                                                                          *
 *  RETURNS    : TRUE  - If successful.                                     *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL FAR PASCAL InitializeFormats()
  {
	fmtLink = RegisterClipboardFormat("Link");

  if (!fmtLink)
  	return FALSE;
	strcpy(SFormats[0].name, CFTEXT);
	SFormats[0].typeID = TEXT_FORMAT;

	strcpy(SFormats[1].name, CFBITMAP);
	SFormats[1].typeID = BITMAP_FORMAT;

	strcpy(SFormats[2].name, CFMETAFILE);
	SFormats[2].typeID = METAFILE_FORMAT;

	strcpy(SFormats[3].name, CFALMOBJ);
	// build the atom at
	SFormats[3].typeID = RegisterClipboardFormat("CF_ALMOBJ");

  return TRUE;
	}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitializeInstance ()                                      *
 *                                                                          *
 *  PURPOSE    : Performs a per-instance initialization of Client.          *
 *              																												 		*
 *  RETURNS    : TRUE  - If initialization was successful.                  *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL FAR PASCAL InitializeInstance(
  WORD nCmdShow)
	{
	I_UNUSED(nCmdShow);
	return TRUE;
  }




