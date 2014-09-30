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
// AUTHOR:   	Shaili Jain
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
#include <memory.h>
#include "signal.h"
#define VERSIONOK     1
#define VERSIONERR    2

unsigned int         fmtLink;

BOOL CALLBACK InitializeApplication();
extern int CALLBACK EditServerObject(AOBJECTID theObject);
extern int _WinAllocFlag;

/***************************************************************************
*	Function:		ExportObject
*	Description: This function exports the signal object
* UPSF
***************************************************************************/
void ExportObject(
	AOBJECTID theObject)
	{
	int i;
	LPSIGSDATA lpData = (LPSIGSDATA)AObjLockData(theObject, data0_SIGSDATA);
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &lpData->version,sizeof(long) );
	AUpsfExportField("ISALPHA", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &lpData->isAlphaSignal,sizeof(BOOL));

	for (i=0; i<lpData->nUserSignals; i++)
		{
		if(AUpsfCreateScope(SIGNALSCOPE, (long)i)==AUPSF_NOERROR)
			{
			AUpsfExportField("NAME", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_STRING, lpData->signals[i].name, lstrlen(lpData->signals[i].name));
			AUpsfExportField("ID", 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &lpData->signals[i].sigID, sizeof(long));
			AUpsfExportField("ISALPHASIG", 0L, AUPSF_NATV_BYTE, AUPSF_UNIV_INTEGER, &lpData->signals[i].bisAlphaSignal,sizeof(BYTE));
			}
		AUpsfCloseScope();
		}                             

	AObjUnlockData(theObject, data0_SIGSDATA);
	}


/***************************************************************************
*	Function:		ImportObject
*	Description: This function Imports the signal object
* UPSF
***************************************************************************/

void ImportObject(
	AOBJECTID objectID)
	{
//	long 			size = 0;
//	ATYPEID 		univType;
	int 			i;
	int numSignals = (int)AUpsfFieldCount(SIGNALSCOPE);
	AObjResizeData(objectID,data0_SIGSDATA, sizeof(SIGSDATA) + (numSignals * sizeof(SIGNALINFO)) );
	LPSIGSDATA lpData = (LPSIGSDATA)AObjLockData(objectID, data0_SIGSDATA);

	lpData->sigsOID = objectID;
	AUpsfImportField("VERSION", 0L, AUPSF_NATV_LONG, &lpData->version,sizeof(long) );
	if (AUpsfImportField("ISALPHA", 0L, AUPSF_NATV_BOOL, &lpData->isAlphaSignal,sizeof(int) ) != 0L)
		lpData->isAlphaSignal = TRUE;
	lpData->nUserSignals = (int)AUpsfFieldCount(SIGNALSCOPE);

	for (i=0; i<lpData->nUserSignals; i++)
		{
		if(AUpsfOpenScope(SIGNALSCOPE, (long)i)==AUPSF_NOERROR)
			{
			long natvType;
			long size;
			if(!AUpsfFieldInfo("NAME", 0L, AUPSF_NATV_STRING, (long *)&natvType, (LPLONG)&size))
				AUpsfImportField("NAME", 0L, AUPSF_NATV_STRING, lpData->signals[i].name, size);
			else
				lpData->signals[i].name[0] = '\0';

			AUpsfImportField("ID", 0L, AUPSF_NATV_LONG, &lpData->signals[i].sigID, sizeof(long));
			if (AUpsfImportField("ISALPHASIG", 0L, AUPSF_NATV_BYTE, &lpData->signals[i].bisAlphaSignal,sizeof(BYTE)) != 0)
				lpData->signals[i].bisAlphaSignal = TRUE;

			AUpsfCloseScope();
			}
		}

	AObjResizeData(objectID,data1_DEFVALS, sizeof(DEFOBJDDATA));
	LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(objectID, data1_DEFVALS);

	if (lpData->nUserSignals)
  	{
		lstrcpy(lpDefData->defSignal.name, lpData->signals[0].name);
		lpDefData->defSignal.sigID = lpData->signals[0].sigID;
		lpDefData->defSignal.bisAlphaSignal = lpData->signals[0].bisAlphaSignal;
		lpDefData->maxSignalID = lpData->signals[lpData->nUserSignals - 1].sigID + 1; // 1 and 2 are used by default, and any
		}
  else {
		lpDefData->defSignal.name[0] = '\0';
		lpDefData->defSignal.sigID = 0;
		lpDefData->defSignal.bisAlphaSignal = 0;
		lpDefData->maxSignalID = 3; // 1 and 2 are used by default, and any
		}
	AObjUnlockData(objectID, data1_DEFVALS);

	AObjUnlockData(objectID, data0_SIGSDATA);

	}





/****************************************************************************
Function: Update Object
Desc:     This function will check the version on the sig object
          July 1993 - version 1

****************************************************************************/
int UpdateObject(AOBJECTID source, AOBJECTID dest)
	{
	I_UNUSED(source);
	I_UNUSED(dest);

	return 1;
	}


/*****************************************************************************************
* FUNCTION:   BUILDALLSIGNALS
*             Handle AOBJ_BUILDSIGNAL
*
*
*****************************************************************************************/
int BuildAllSignals(AMEMBLOCKID sigsBlockID, AOBJECTID theObject)
	{
	int i;
	pAObjSignalRecord theSigs;

  // Check old signals before building signals
	SIGS 		*sig = new SIGS(theObject);
	sig->CheckSignals();
	sig->SetSave(SIGS_SAVESIGS);
	delete sig;

	LPSIGSDATA lpData = (LPSIGSDATA)AObjLockData(theObject, data0_SIGSDATA);
	int nSignals = lpData->nUserSignals + 2; // 'default' and 'any'
  AMemSetSize(sigsBlockID, (nSignals)  * sizeof(AObjSignalRecord));
	theSigs = (pAObjSignalRecord)(LPSTR)AMemLock(sigsBlockID);

	wsprintf(theSigs[0].theName, "%s", SIGSZ_DEFAULT);
	theSigs[0].theID = ISIG_DEFAULT;

	wsprintf(theSigs[1].theName, "%s", SIGSZ_ANY);
	theSigs[1].theID = ISIG_ANY;

	for (i = 0; i < lpData->nUserSignals ; i++)
    {
		wsprintf(theSigs[i + 2].theName, "%s", lpData->signals[i].name);
		theSigs[i + 2].theID = lpData->signals[i].sigID;
		}

	AObjUnlockData(theObject, data0_SIGSDATA);
	AMemUnlock(sigsBlockID);
  return 1;
	}




/*********************************************************************************************
*
* ADMIN ROUTINE:	sigADMN
*
*	DESCRIPTION:		This function is the ADMN routine for DDE Server Object
*
**********************************************************************************************/
extern "C" LONG CALLBACK signalADMN(
	AOBJECTID          theObject,
  pAObjMessage      system)
  {

  AOBJECTID   	 			sObject;
  AOBJECTID    			dObject;
  AEvtInfo 					tEvent;
	char              *errorString = new char[255];
	long    					rVal = A_OK;

  errorString[0] 		= '\0';

  switch (system->message1)
    {
    case  AOBJ_CREATED:
      {
			// sigsCount++;
      dObject = (AOBJECTID)theObject;
			AObjSetDataCount(dObject, MAXSIGNALDATAS);
			AObjResizeData(dObject, data0_SIGSDATA, sizeof(SIGSDATA));
			AObjResizeData(dObject, data1_DEFVALS, sizeof(DEFOBJDDATA));

			LPSIGSDATA lpData = (LPSIGSDATA)AObjLockData(dObject, data0_SIGSDATA);
			LPDEFOBJDDATA lpDefData =  (LPDEFOBJDDATA)AObjLockData(dObject, data1_DEFVALS);

			lpDefData->defSignal.name[0] = '\0';
			lpDefData->defSignal.sigID = 0;
			lpDefData->defSignal.bisAlphaSignal = 0;
		  lpDefData->maxSignalID = 3; // 1 and 2 are used by default, and any

			memset(lpData, 0, sizeof(SIGSDATA));
			lpData->version = SIGS_DATAVERSION;
			lpData->nUserSignals = 0;
			lpData->isAlphaSignal = FALSE;
		
			if (system->message4 == AOBJ_TEMPORARY)
        {
        ;
				}
			AObjUnlockData(dObject, data0_SIGSDATA);
			AObjUnlockData(dObject, data1_DEFVALS);
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
				lstrcpy(system->errorMessage, "AOBJ_ASSIGNOBJECT failed : SIGNAL ALM:/tsig");
        return A_ERROR;
        }

			for (i = 0; i < MAXSIGNALDATAS ; i++)
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
			AObjLockData(dObject, data0_SIGSDATA);

			AObjUnlockData(dObject, data0_SIGSDATA);
			system->message4 = A_OK;
			system->message3 = (LONG)SIGS_DATAVERSION; // return current data Version
      }
      break;

		// Currently there is no IMP/EXP supported for this object.  There is no equivalent object
    // on other platforms (MAC etc)
		case AOBJ_IMPORTOBJECT:
    	ImportObject(theObject);
			break;

		case AOBJ_EXPORTOBJECT:
			ExportObject(theObject);
			break;

    case AOBJ_EDITOBJECT:
			{

			int flag = EditServerObject(theObject);
			if (flag == -1)
				rVal = A_ERROR;
			else if (flag == 1)
				rVal = A_MODIFIED;
			else rVal = A_OK;

      }
      break;

		case AOBJ_CHECKOBJECT: {
			}
      break;
      
    case AOBJ_VALIDATEOBJECTIDS:
      {
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



extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	hInst = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}

