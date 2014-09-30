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
// AUTHOR:     	Shaili Jain
//
// DESCRIPTION:
//
// CHANGES:     March 22, 1994
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>

#define   MDRTCODE 						1
#include "signal.h"
#include "stdio.h"
#include "string.h"
#include "o_number.h"







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
	SIGS *sigs = NULL;
	if (!bIsStartUp)
		{
		sigs= (SIGS *)AObjGetRuntimeInfo(theObject);
		}
	else // AEVENT_STARTUP
		{
		sigs = new SIGS(theObject);
		AObjGetDataCount(theObject);
		// AObjSetDataCount(theObject, MAXSIGSERVERDATAS);
		AObjSetRuntimeInfo(theObject, (long)sigs);
		}
	return (LPLONG)sigs;
	}



/*************************************************************************
* This is the MDRT routine for the dde client Object
*	This is the common entry point for all the events from the shell
**************************************************************************/
extern "C" void FAR PASCAL _export signalMDRT(
	pAEvtInfo 				theEvent,
	pAObjMessage      system) {
	OBJECTID  theObject = theEvent->targetObject;
	SIGS *sigs;

	switch ((long)theEvent->what)  {
		case AEVENT_STARTUP:
			{
			long i = -1;
			// initialise the SIGS application
			while ((theObject = AObjGetNextObject(OTYPE_SIGS, &i)) != NULL)
				{
				sigs = (SIGS *)GetRunTimePtr(theObject, (theEvent->what==AEVENT_STARTUP));
				}
			}
			break;
    
		case AEVENT_SHUTDOWN:
			{
			long i = -1;
			while ((theObject = AObjGetNextObject(OTYPE_SIGS, &i)) != NULL)
				{
				sigs = (SIGS *)GetRunTimePtr(theObject, (theEvent->what==AEVENT_STARTUP));
				delete sigs;
				}


			break;
			}

		case AEVENT_OBJECTCHANGED:
			{
			OBJECTID sigsObject = 0L;
			long i = -1;
			// check to see if the data source object has changed and then post DdePostAdvise():
			while ((sigsObject = AObjGetNextObject(OTYPE_SIGS, &i)) != NULL)
				{
				sigs = (SIGS *)GetRunTimePtr(sigsObject, (theEvent->what==AEVENT_STARTUP));
				// Enumerate transctions with matching dataSrc OID
				}
			}
			break;
		default:  break;
		}
	I_UNUSED(system);
	}

