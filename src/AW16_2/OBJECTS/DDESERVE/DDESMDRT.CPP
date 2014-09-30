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
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>

#define   MDRTCODE 						1
#include "ddemlsv.h"
#include "stdio.h"
#include "string.h"
#include "o_number.h"



extern int GetGData(HDDEDATA hData,MEMBLOCKID txtID, int fmt);




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
	DDES *ddeS = NULL;
	if (!bIsStartUp)
		{
		ddeS = (DDES *)AObjGetRuntimeInfo(theObject);
		}
	else // AEVENT_STARTUP
		{
		ddeS = new DDES(theObject);
		AObjGetDataCount(theObject);
		// AObjSetDataCount(theObject, MAXDDESERVERDATAS);
		AObjSetRuntimeInfo(theObject, (long)ddeS);
		}
	return (LPLONG)ddeS;
	}


/******************************************************************************
*
*
*
*******************************************************************************/
BOOL UninitializeDDE()
	{
	DWORD idInst;
	idInst = ATypeGetRuntimeInfo(OTYPE_DDES);
	HWND hwnd = GetActiveWindow();
	DdeNameService(idInst, NULL, NULL, DNS_UNREGISTER);
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




/**********************************************************************************************
* Function to initialise the application as a DDE SERVER with DDEML<
*
*
**********************************************************************************************/
BOOL InitializeDDE()
	{
	DWORD	idInst = DDES_NOTINIT;
	BOOL rBool = FALSE;
	int  rVal;
	if ((rVal = DdeInitialize(&idInst, (PFNCALLBACK)MakeProcInstance((FARPROC)DdeCallback,
								(HINSTANCE)hInst), APPCMD_FILTERINITS, NULL)) == DMLERR_NO_ERROR) {
		rBool = TRUE;
		ATypeSetRuntimeInfo(OTYPE_DDES, (long)idInst);

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
	// RegisterClipboardFormat("Link");

	I_UNUSED(rBool);
	I_UNUSED(rVal);
	I_UNUSED(ddesCount);

	return rBool;
	}




/*************************************************************************
* This is the MDRT routine for the dde client Object
*	This is the common entry point for all the events from the shell
**************************************************************************/
extern "C" void FAR PASCAL _export ddeSMDRT(
	pAEvtInfo 				theEvent,
	pAObjMessage      system) {
	OBJECTID  theObject = theEvent->targetObject;
	DDES *ddeS;

	switch ((long)theEvent->what)  {
		case AEVENT_STARTUP:
			{
			long i = -1;
			BOOL rVal = FALSE;
			// initialise the DDES application
			rVal = InitializeDDE();
			while ((theObject = AObjGetNextObject(OTYPE_DDES, &i)) != NULL)
				{
				ddeS = (DDES *)GetRunTimePtr(theObject, (theEvent->what==AEVENT_STARTUP));
				if (rVal)  {
					HSZ hsz = ddeS->GetddesService().hszService;
					DdeNameService(ddeS->idInst,hsz, NULL, DNS_REGISTER | DNS_FILTERON);
					}
				}
			}
			break;
    
		case AEVENT_SHUTDOWN:
			{
			long i = -1;
			while ((theObject = AObjGetNextObject(OTYPE_DDES, &i)) != NULL)
				{
				ddeS = (DDES *)GetRunTimePtr(theObject, (theEvent->what==AEVENT_STARTUP));
				delete ddeS;
				}

			UninitializeDDE();
			break;
			}

		case AEVENT_OBJECTCHANGED:
			{
			OBJECTID ddesObject = 0L;
			long i = -1;
			// check to see if the data source object has changed and then post DdePostAdvise():
			while ((ddesObject = AObjGetNextObject(OTYPE_DDES, &i)) != NULL)
				{
				XCTIONINFO Xction;
        long 	count = -1;
				ddeS = (DDES *)GetRunTimePtr(ddesObject, (theEvent->what==AEVENT_STARTUP));
				// Enumerate transctions with matching dataSrc OID
				while (ddeS->EnumOIDXction(theObject, &Xction, &count) != DDES_XCTIONNOTFOUND	)
					{
					DdePostAdvise(ddeS->idInst, Xction.hszTopic, Xction.hszItem);
					}
				}
			}
			break;
		default:  break;
		}
	I_UNUSED(system);
	}

