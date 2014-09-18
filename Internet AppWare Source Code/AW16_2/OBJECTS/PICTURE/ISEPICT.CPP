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
#include <o_pict.h>
#include "picture.h"



extern LPLONG GetPictObject(OBJECTID objID);

extern "C" void FAR PASCAL _export IsPictureEmpty(
  pAEvtInfo			theEvent,
	pAObjMessage 					system)
	{
	OBJECTID 						pObject;
	PICTOBJECT           *pO;
	pObject = AFuncGetParameter(1); // the picture object to load New picture into

	system->message1 = 1;

	if (AObjCheckType( pObject, (TYPEID)OTYPE_PICTURE))
		{
		// skj, 3-11-93 init from the datas


		pO =  (PICTOBJECT *) GetPictObject(pObject);

		if (pO->GetImageSize() == 0)
			system->message1 = 1;  // picture is EMPTY
		else
		  system->message1 = 2;
		

  	}
	else
		{
		lstrcpy(system->errorMessage, "Error while loading picture.");
		system->message1 = 0;
		}

	I_UNUSED(theEvent);
	}


