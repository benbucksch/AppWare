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
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
//	"scal_evt.cpp"
//
// Event handling functions for the SCALE object
//
////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include	<windows.h>
#include	<stdlib.h>
#include	<a_alm.h>
#include  <o_number.h>
#include	"scale.h"


// Scale_objectChanged //////////////////////////////////////////////////////

void far pascal _export Scale_objectChanged(
	pAEvtInfo	theEvent,
	pAObjMessage	/*theSystem*/)
{
	OBJECTID       targetID = theEvent->targetObject;
	AObjOwnerInfo  ownerInfo;
	SCALE *	       scale ;
	int            i ;

	ownerInfo.key = AOBJ_GETFIRSTKEY;	
	while (AObjGetNextOwner(OTYPE_SCAL, targetID, &ownerInfo))
		{
		scale = (SCALE*) AObjGetRuntimeInfo(ownerInfo.ownerID);
		for (i=0;i<3; i++)
    	{
			if(scale->GetObject(i)==targetID)
				scale->SetPosition() ; //(OBJECTID)ONmbrGetInteger(scale->GetObject(i)),i);
      }
		}
}

// Scale_startupEvent ///////////////////////////////////////////////////////

void far pascal _export Scale_startupEvent(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				/*theSystem*/)
{
	OBJECTID	oiScale;
	long			scaleKey = AOBJ_GETFIRSTKEY;
	SCALE*	  scale;

	while((oiScale = AObjGetNextObject(OTYPE_SCAL, &scaleKey))!=NULL)
		{
		scale = (SCALE*) AObjGetRuntimeInfo(oiScale);
    if (scale != NULL)
		if(scale->GetObject(0))
			{		// register position obj for obj changes
			AObjRegisterOwner(oiScale, scale->GetObject(0), 0L);
			scale->SetPosition() ;//(long) ONmbrGetInteger(scale->GetObject(0)),0);
			}
		if(scale->GetObject(1))
			{
			AObjRegisterOwner(oiScale, scale->GetObject(1), 0L);
			scale->SetPosition() ; //(long) ONmbrGetInteger(scale->GetObject(1)),1);
			}
		if(scale->GetObject(2))
			{
			AObjRegisterOwner(oiScale, scale->GetObject(2), 0L);
			scale->SetPosition() ;//(long) ONmbrGetInteger(scale->GetObject(2)),2);
			}
		}
}

////////////////////////////////////////////////////////////////////////////

