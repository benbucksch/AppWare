///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// event.cpp
// Text Builder
// Text Builder Events
//
// Robert Lundahl/ Edgar C. Jerez
//
// Start Date 07/01/92
// End   Date 12/18/92
///////////////////////////////////////////////////////////////

#include "tf_incld.h"


void RegisterObjects ( );
void ChildCheck ( pAObjMessage systemPtr, AOBJECTID objid );
void DoObjectChanged ( pAObjMessage systemPtr, AOBJECTID sourceObject, AOBJECTID targetObject );
void TranslateChar ( TOKEN* tkp, LPSTR output );
void TranslateLiteral ( TOKEN* tkp, LPSTR output, AOBJECTID objid );
WORD TranslateObject ( TOKEN* tkp, LPSTR output, AOBJECTID objid );
void EventBuildText ( AOBJECTID tfObject, pAObjMessage systemPtr );


extern GLOBAL vG;       // Global Container Class

extern "C"
{
	void CALLBACK Event ( pAEvtInfo eventPtr, pAObjMessage systemPtr );
}


///////////////////////////////////////////////////////////////
// Event:
//
///////////////////////////////////////////////////////////////

void CALLBACK Event ( pAEvtInfo eventPtr, pAObjMessage systemPtr )
{
	if (eventPtr->what == AEVENT_OBJECTCHANGED)
	  {
		ChildCheck(systemPtr, eventPtr->targetObject);
		AFuncReportResult(systemPtr, 0L, NULL);
		}

	else if (eventPtr->what == AEVENT_STARTUP)
		{
		RegisterObjects();
		AFuncReportResult(systemPtr, 0L, NULL);
	  }
}				// Event...


///////////////////////////////////////////////////////////////
// RegisterObjects:
//
///////////////////////////////////////////////////////////////

void RegisterObjects()

{
	AOBJECTID	tfID;
  long			getkey = AOBJ_GETFIRSTKEY;

  // register all objects that we are interested in change....
	while(0L != (tfID = AObjGetNextObject(OTYPE_TEXTMAKER, &getkey)))
		{
		AOBJECTID*	ownedID = (AOBJECTID*)AObjLockData(tfID, 0L);

		while (*ownedID)
			{
			if (*ownedID)
				AObjRegisterOwner(tfID, *ownedID, 0L);

			ownedID++;
			}

		AObjUnlockData(tfID, 0L);

		DATAFOUR*	data4 =  (DATAFOUR*)AObjLockData(tfID, 4L);

		if (data4->autoUpdate)
			{
				AObjMessage	system;
				EventBuildText(tfID, &system);
				AEvtPostSignalAtMark(tfID, SIGNAL_EXPUPDATE);
			}

		AObjUnlockData(tfID, 4L);

		}
}				// RegisterObjects...


///////////////////////////////////////////////////////////////
// ChildCheck:
//
///////////////////////////////////////////////////////////////

void ChildCheck ( pAObjMessage systemPtr, AOBJECTID targetID )
{
	AObjOwnerInfo	ownerInfo;
	ownerInfo.key = -1;

	while(AObjGetNextOwner(OTYPE_TEXTMAKER, targetID, &ownerInfo))
		DoObjectChanged(systemPtr, targetID, ownerInfo.ownerID);
}				// ChildCheck...


///////////////////////////////////////////////////////////////
// DoObjectChanged:
//
///////////////////////////////////////////////////////////////

void DoObjectChanged ( pAObjMessage systemPtr, AOBJECTID sourceObject, AOBJECTID targetObject )
{
	BOOL			isObject = FALSE;
	TOKEN*    data2;
	DATAFOUR* data4;

	AEvtPostSignalAtMark(targetObject, SIGNAL_VARCHANGED);

	data2 = (TOKEN*)AObjLockData(targetObject, 2L);

	while (data2->type != STOP)
	{
		if (data2->type == OBJECT)
		{
			if (data2->multi == sourceObject)
			{
				isObject = TRUE;
				break;
			}
		}
		data2++;
	}

	if (isObject)	// object changed in this object - invoke send text
	{
		data4 =  (DATAFOUR*)AObjLockData(targetObject, 4L);

		if (data4->autoUpdate)
		{
			EventBuildText(targetObject, systemPtr);
			AEvtPostSignalAtMark(targetObject, SIGNAL_EXPUPDATE);
		}
		AObjUnlockData(targetObject, 4L);
	}
	AObjUnlockData(targetObject, 2L);
}				// DoObjectChanged...


///////////////////////////////////////////////////////////////
// EventBuildText:
//
///////////////////////////////////////////////////////////////

void EventBuildText ( AOBJECTID tfObject, pAObjMessage systemPtr )
{
	BOOL				ret;
	LPSTR				output;
	TOKEN*			data2;
	DATAFOUR*		data4;
	AMEMBLOCKID	memblk;

	output = (LPSTR)new BYTE[maxEdit*10+1];
	if (output == NULL)
	{
		AFuncReportResult(systemPtr, 0L, "Memory Allocation Failed");
		return;
	}

	output[0] = 0;

	data2 = (TOKEN*)AObjLockData(tfObject, 2L);
	if (data2 == NULL)
	{
		AFuncReportResult(systemPtr, 0, "Data 2 Undefined: Text Builder Build Text");
		return;
	}

	while (data2->type != STOP)
	{
		switch (data2->type)
		{
		case CHARACTER:
			TranslateChar(data2, output);
			break;

		case LITERAL:
			TranslateLiteral(data2, output, tfObject);
			break;

		case OBJECT:
			TranslateObject(data2, output, tfObject);
			break;
		}
		data2++;
	}

	AObjUnlockData(tfObject, 2L);

	data4 = (DATAFOUR*)AObjLockData(tfObject, 4L);
	if (data4 == NULL)
	{
		AFuncReportResult(systemPtr, 0, "Data 4 Undefined: Text Builder Build Text");
		return;
	}

	if (data4->resultId == 0L)
	{
		AFuncReportResult(systemPtr, 0, "Text Builder:\nResult Object Was Not Specified;\nText Build Aborted");
		AObjUnlockData(tfObject, 4L);
		return;
	}

	memblk = AMemAllocate(lstrlen(output)+1);
	if (memblk == 0L)
  	{
		AObjUnlockData(tfObject, 4L);
		return;
		}

	lstrcpy((LPSTR)AMemLock(memblk), output);

	ret = AObjConvertFromText(data4->resultId, memblk);
	if (!ret)
	{
		AMemFree(memblk);
		AFuncReportResult(systemPtr, 0, "OIP Call Method Failed: Text Builder");
		AObjUnlockData(tfObject, 4L);
		return;
	}
    AMemFree(memblk);

	AFuncReportResult(systemPtr, 0L, NULL);
	delete output;
	AObjUnlockData(tfObject, 4L);
}				// EventBuildText...
