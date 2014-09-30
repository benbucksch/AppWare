//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//////////////////////////////////////////////////////////////

#define		STRICT
#define		ASTRICT

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "a_alm.h"
#include "intdebug.h"
#include "rad.h"
#include "o_button.h"
#include "globals.h"

extern int Object_in_items_array(
	OBJECTID      object,
  LPOBJECTID    items,
	int           numItems);

extern "C" void pascal _export Rad_objectChanged(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	LPOBJECTID							items;
	LONG                		magicCookie = AOBJ_GETFIRSTKEY;
	OBJECTID								oiRadio;
	int											numItems, i;

	I_UNUSED(theSystem);

	if ( AObjGetType(theEvent->sourceObject) == OTYPE_BUTTON   &&  OButnIsSelected(theEvent->sourceObject))
    {
		while ( (oiRadio = AObjGetNextObject(OTYPE_RADIOGROUP, &magicCookie)) != 0)
			{
			numItems = (int)AObjGetDataSize(oiRadio, RADIO_ITEMS_INFO)/sizeof(OBJECTID);
			items = (LPOBJECTID)AObjLockData(oiRadio, RADIO_ITEMS_INFO);

			if (Object_in_items_array(theEvent->sourceObject, items, numItems) != -1)
				{
				for(i=0; i<numItems; i++)
					{
					if (items[i] != theEvent->sourceObject)
						OButnSetSelected(items[i], FALSE);
					}

				}
			AObjUnlockData(oiRadio, RADIO_ITEMS_INFO);
			}
		}

	}



