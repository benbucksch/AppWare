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
//--------------------------------------------------------------
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////

#define		STRICT
#define		ASTRICT

////////////////////////////////////////////////////////
/////////  Radio Group Object Adm Routines    ////////
////////////////////////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <string.h>
#include <a_alm.h>
#include <intdebug.h>

#include "rad.h"
#include "o_button.h"
#include "globals.h"

void	ExportObject(OBJECTID theObject);
void	ImportObject(OBJECTID theObject);

char szErrorString[256];

#pragma warn -aus
long pascal _export RadioObjectAdm(
	OBJECTID			theObject,
	pAObjMessage	theSystem)
	{
	LPOBJECTID			items;
	int							i, numItems, numSelected;
	long 						rVal = A_OK;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED:
			AObjSetDataCount((OBJECTID)theObject, NUM_RADIO_DATAS);
			break;

		case AOBJ_EXPORTOBJECT:
			ExportObject((OBJECTID)theObject);
			break;

		case AOBJ_IMPORTOBJECT:
			ImportObject((OBJECTID)theObject);
			break;

		case AOBJ_READ:
			theSystem->message4 = A_OK; // versionOK;
			theSystem->message3 = 100;
			break;

		/**** Moved from Object Def (rad_def.cpp)Code **********/
		case AOBJ_EDITOBJECT:
			rVal = EditRadioObject(theObject);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			items = (LPOBJECTID)AObjLockData(theObject, RADIO_ITEMS_INFO);
			numItems = (int) AObjGetDataSize(theObject, RADIO_ITEMS_INFO)/sizeof(OBJECTID);
			i = 0;
			while(i < numItems)
				{
				AObjValidateID(&items[i], theSystem);
				if (!items[i])
					{
					numItems--;
					EraseItemFromArray(i, sizeof(OBJECTID), numItems, items);
					}
				else
					i++;
				}
      if (items)
				AObjUnlockData(theObject, RADIO_ITEMS_INFO);
			AObjSetData(theObject, RADIO_ITEMS_INFO, items, sizeof(OBJECTID)*numItems);
			break;

		case AOBJ_CHECKOBJECT:
			items = (LPOBJECTID)AObjLockData(theObject, RADIO_ITEMS_INFO);
			numItems = (int) AObjGetDataSize(theObject, RADIO_ITEMS_INFO)/sizeof(OBJECTID);
			i = numSelected = 0;
			while(i < numItems)
				{
				if (!AObjCheckType(items[i], OTYPE_BUTTON))
					{
					numItems--;
					EraseItemFromArray(i, sizeof(OBJECTID), numItems, items);
					}
				else
					{
					if (OButnIsSelected(items[i]))
						numSelected++;	
					i++;
					}
				}

			if (items)
				AObjUnlockData(theObject, RADIO_ITEMS_INFO);

			AObjSetData(theObject, RADIO_ITEMS_INFO, items, sizeof(OBJECTID)*numItems);
			if (numItems >1 &&  numSelected == 1)
				rVal = A_OK;
			else
				{
				rVal = A_WARNING;
				strcpy(szErrorString, "The radio group '");
				AObjGetName(theObject, &szErrorString[strlen(szErrorString)], 32);
				strcat(szErrorString, "' contains ");
				if (numItems < 2)
					strcat(szErrorString, "less than two buttons.");
				else if (!numSelected)
					strcat(szErrorString, "no selected button.");
				else
					strcat(szErrorString, "more than one selected button.");
				AObjReportError(theObject, AOBJ_CHECKOBJECT, rVal, szErrorString, 0);
				}
		 	break;

		case AOBJ_BUILDSIGNALS:
			break;
		default:
			rVal = A_NOTHANDLED;
			break;

		}
	return rVal;
	}

void	ExportObject(
	OBJECTID theObject)
	{
	OBJECTID* oid = (OBJECTID*)AObjLockData(theObject, RADIO_ITEMS_INFO);
	long      IDs = AObjGetDataSize(theObject, RADIO_ITEMS_INFO)/sizeof(OBJECTID);

	for(long i=0L; i<IDs; i++, oid++)
		AUpsfExportField("RAD_OBJS", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, oid, sizeof(OBJECTID));
  if (oid)
		AObjUnlockData(theObject, RADIO_ITEMS_INFO);
	}

void	ImportObject(
	OBJECTID theObject)
	{
	long			IDs = AUpsfFieldCount("RAD_OBJS");
	AObjResizeData(theObject, RADIO_ITEMS_INFO, IDs*sizeof(OBJECTID));
	OBJECTID*	oid = (OBJECTID*)AObjLockData(theObject, RADIO_ITEMS_INFO);
	long			errorCount = 0L;

	for(long i=0L; i<IDs; i++, oid++)
		if(AUpsfImportField("RAD_OBJS", i, AUPSF_NATV_OBJECTREF, oid, sizeof(OBJECTID)))
			{
			AUpsfLogComment("Error importing one of the radio buttons for the Radio Group Object.");
			errorCount++;
      if (oid)
				AObjUnlockData(theObject, RADIO_ITEMS_INFO);
			AObjResizeData(theObject, RADIO_ITEMS_INFO, (IDs-errorCount)*sizeof(OBJECTID));
			oid = (OBJECTID *)AObjLockData(theObject, RADIO_ITEMS_INFO);

			oid = (OBJECTID *)((long)oid + (i-errorCount));
			}
  if (oid)
		AObjUnlockData(theObject, RADIO_ITEMS_INFO);

	}

#pragma warn .aus

