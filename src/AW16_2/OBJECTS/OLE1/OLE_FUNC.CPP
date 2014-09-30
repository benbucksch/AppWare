/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					OLE_FUNC.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Implements the AppWare OLE Object Functions.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>
#include <o_text.h>
#include <o_number.h>

#include "srole.h"
#include "srolemod.h"
#include "sroleapp.h"
#include "sroledoc.h"
#include "sroleobj.h"
#include "srolefun.h"
#include "srreg.h"
#include "ole_func.h"
#include "ole_obj.h"
#include "ole_wip.h"

////////////////////////////////////////////////////////////////
//
// FUNCTION:	GetObject
//
// DESC:			Gets the OLE1CLIENT object from the given parameter #.
//
////////////////////////////////////////////////////////////////

BOOL	GetObject(
	LONG							lIndex,					// in:   parameter index
	SROleClientObj*&	pObj						// out:  object in that parameter
	)
	{
	SROleClientDoc*		pDoc	= SRCurrentDoc();
	OBJECTID					oId;

	pObj = NULL;

	oId = AFuncGetTypedParameter(lIndex, OTYPE_OLE1CLIENT);

	if (oId != 0 && pDoc != NULL)
		pObj = pDoc->GetObjById(oId);

	return (pObj != NULL);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	CheckOleObject
//
// DESC:			Verifies the existence, validity, and state of the
//						object.
//
//						In Param:		OLE Object
//						Out Param:	none
//						Flows:			Invalid
//												Inactive
//												Active
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		CheckOleObject(
	pAEvtInfo						theEvent,
	pAObjMessage				theSystem
	)
	{
	SROleClientObj*		pObj;

	I_UNUSED(theEvent);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	if (GetObject(OLEF_CHK_IN_OLEOBJECT, pObj))
		{
		if (pObj->OleObject() == NULL)
			theSystem->message1 = OLEF_CHK_FLOW_ERROR;				// no object
		else if (pObj->ObjectIsOpen())
			theSystem->message1 = OLEF_CHK_FLOW_ACTIVE;
		else
			theSystem->message1 = OLEF_CHK_FLOW_INACTIVE;
		}

	else
		{
		// invalid object
		theSystem->message1 = OLEF_CHK_FLOW_ERROR;
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	CreateOleObject
//
// DESC:			Activates the server for the OLE object and creates
//						a new instance of the object.
//
//						In Param:		OLE Object
//												Text Object:  OLE Object Type (optional)
//												Text Object:  OLE link file (optional)
//												Text Object:  OLE link item (optional)
//						Out Param:	none
//						Flows:			After Creation
//												Error in Creation
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		CreateOleObject(
	pAEvtInfo						theEvent,
	pAObjMessage				theSystem
	)
	{
	BOOL							fOk					= FALSE;
	SROleClientObj*		pObj				= NULL;
	SROleClientDoc*		pDoc				= SRCurrentDoc();
	OBJECTID					oType;
	OBJECTID					oLinkFile;
	OBJECTID					oLinkItem;
	OBJECTID					oId;
	BOOL							fLinkedObject;
	char							szType[MAXKEY];
	char							szLinkFile[_MAX_PATH];
	char							szLinkItem[_MAX_PATH];
	char							szClass[MAXKEY];

	I_UNUSED(theEvent);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	// assume error since we only have one good path
	theSystem->message1 = OLEF_CRE_FLOW_ERROR;

	oId       = AFuncGetTypedParameter(OLEF_CRE_IN_OLEOBJECT, OTYPE_OLE1CLIENT);
	oType     = AFuncGetTypedParameter(OLEF_CRE_IN_TYPE,     OTYPE_TEXT);
	oLinkFile = AFuncGetTypedParameter(OLEF_CRE_IN_LINKFILE, OTYPE_TEXT);
	oLinkItem = AFuncGetTypedParameter(OLEF_CRE_IN_LINKITEM, OTYPE_TEXT);

	if (oId != 0)
		{
		SROLEObject*		pObject	 = (SROLEObject*) AObjGetRuntimeInfo(oId);

		if (pDoc != NULL)
			pObj = pDoc->GetObjById(oId);

		// At this point:  if pObj == NULL, or pObj->OleObject() == NULL, then
		// we don't have an object, else we need to delete it first.  Although
		// we could reuse an existing SROleClientObj, it's probably safer
		// to delete it.

		if (pObj != NULL)
			pObj->OwnerDoc()->DeleteObj(pObj);

		// We better not have one now!
		I_ASSERT(GetObject(OLEF_CRE_IN_OLEOBJECT, pObj) == FALSE);

		// Get defaults for those options not passed in.
		if (oType == 0)
			pObject->ObjectType(szType, sizeof(szType));
		else
			SRGetText(oType, szType, sizeof(szType));

		if (szType[0] != '\0' &&
				ClassFromDescription(szType, szClass, sizeof(szClass)))
			{
			if (oLinkFile != 0)															// type is linked
				{
				fLinkedObject = TRUE;
				SRGetText(oLinkFile, szLinkFile, sizeof(szLinkFile));

				if (oLinkItem != NULL)
					SRGetText(oLinkItem, szLinkItem, sizeof(szLinkItem));
				else
					szLinkItem[0] = '\0';
				}
			else
				{
				fLinkedObject = (pObject->Options() & SOLE_LINKED);

				if (fLinkedObject)
					{
					pObject->LinkFile(szLinkFile, sizeof(szLinkFile));
					pObject->LinkItem(szLinkItem, sizeof(szLinkItem));
					}
				}
			}
		else
			pDoc = NULL;																		// force failure below

		// Create a new SROleClientObj* and OLE object
		if (pDoc != NULL)
			{
			pObj = pDoc->CreateObjById(oId);

			if (fLinkedObject)
				fOk = pObj->CreateLink(szClass, szLinkFile, szLinkItem);
			else
				fOk = pObj->Create(szClass);

			if (fOk)
				theSystem->message1 = OLEF_CRE_FLOW_AFTER;
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	DeleteOleObject
//
// DESC:			Deletes the OLE object.
//
//						In Param:		OLE Object
//						Out Param:	none
//						Flows:			After Deletion
//												No Object
//												Error in Delete
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		DeleteOleObject(
	pAEvtInfo						theEvent,
	pAObjMessage				theSystem
	)
	{
	SROleClientObj*		pObj;
	SROleClientDoc*		pDoc;
	OBJECTID					oiObj;

	I_UNUSED(theEvent);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	if (GetObject(OLEF_DEL_IN_OLEOBJECT, pObj))
		{
		if (pObj->OleObject() == NULL)
			theSystem->message1 = OLEF_DEL_FLOW_NOOBJECT;
		else
			{
			// delete the object by asking the document to delete it.
			//!! Should we allow option to prompt user for deletion?

			pDoc  = pObj->OwnerDoc();
			oiObj = pObj->ObjectId();

			if (pDoc->DeleteObj(pObj))
				{
				pDoc->CreateObjById(oiObj);

				theSystem->message1 = OLEF_DEL_FLOW_AFTER;
				}
			else
				theSystem->message1 = OLEF_DEL_FLOW_ERROR;
			}
		}

	else
		theSystem->message1 = OLEF_DEL_FLOW_ERROR;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ActivateOleObject
//
// DESC:			Invokes the given verb.
//
//						In Param:		OLE Object
//												TNDT Object:  Verb (optional)
//						Out Param:	none
//						Flows:			After Activation
//												No Object
//												Error in Activation
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		ActivateOleObject(
	pAEvtInfo						theEvent,
	pAObjMessage				theSystem
	)
	{
	BOOL							fContinue				= FALSE;
	char							szClass[MAXKEY];
	char							szVerb[MAXKEY];
	SROleClientObj*		pObj;
	OBJECTID					oId;
	TYPEID						tId;
	WORD							wVerb;

	I_UNUSED(theEvent);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	theSystem->message1 = OLEF_ACT_FLOW_ERROR;

	if (GetObject(OLEF_ACT_IN_OLEOBJECT, pObj))
		{
		if (pObj->OleObject() == NULL)
			theSystem->message1 = OLEF_ACT_FLOW_NOOBJECT;

		else
			{
			fContinue = TRUE;
			oId       = AFuncGetParameter(OLEF_ACT_IN_VERB);
			tId       = AObjGetType(oId);

			if (tId == OTYPE_TEXT)
				{
				SRGetText(oId, szVerb, sizeof(szVerb));
				pObj->ObjectClass(szClass, sizeof(szClass));

				// Search verb list for match
				wVerb = SearchVerbs(szClass, szVerb);
				}
			else if (tId == OTYPE_NUMBER)
				wVerb = (WORD) SRGetNumber(oId);
			else
				wVerb = OLEVERB_PRIMARY;
				
			}
		}

	if (fContinue)
		{
		// activate the window that contains the VI of the object.
		OLEObjectVI::SetItemActive(NULL, pObj->ObjectId());

		if (pObj->Activate(wVerb))
			theSystem->message1 = OLEF_ACT_FLOW_AFTER;
		}
	}
