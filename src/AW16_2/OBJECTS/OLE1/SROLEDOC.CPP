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
// FILE:					SROLEDOC.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		OLE Document-level functions
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <windows.h>
#include "ushort.h"
#include <ole.h>
#include <a_alm.h>
#include <intdebug.h>

#include "srole.h"
#include "ole_obj.h"
#include "srolemod.h"
#include "sroleapp.h"
#include "sroledoc.h"
#include "sroleobj.h"

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc constructor
//
// DESC:			Initializes a new client doc instance.
//
////////////////////////////////////////////////////////////////

SROleClientDoc::SROleClientDoc(
	SROleClientApp*		pApp								// in:  parent document
	) :
		m_OwnerApp(pApp),
		m_LibHandle(NULL),
		m_lNextObjName(0),
		m_lBusyCount(0),
		m_aDocName(NULL),
		m_fInitOk(FALSE)
	{
	m_hCommWindow = OLEMODULE->NewCommWindow();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc destructor
//
// DESC:			Cleans up a client doc instance.
//
////////////////////////////////////////////////////////////////

SROleClientDoc::~SROleClientDoc(
	VOID
	)
	{
	DestroyWindow(m_hCommWindow);
	m_hCommWindow = NULL;

	if (m_aDocName != NULL)
		{
		DeleteAtom(m_aDocName);
		m_aDocName = NULL;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::Register
//
// DESC:			Registers the client document with the OLE libraries.  The
//						document name is remembered.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientDoc::Register(
	LPSTR				pszDocFile									// in:  name of document
	)
	{
	OLESTATUS		oStatus;

	I_ASSERT(pszDocFile != NULL);

	m_aDocName = AddAtom(pszDocFile);

	oStatus = OleRegisterClientDoc(m_OwnerApp->AppName(), pszDocFile,
														0, &m_LibHandle);

	// We shouldn't have to wait
	I_ASSERT(oStatus != OLE_WAIT_FOR_RELEASE);

	return (oStatus == OLE_OK);
	}
	
////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::Revoke
//
// DESC:			Revokes access to the document.  The document object should
//						be deleted after this.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientDoc::Revoke(
	VOID
	)
	{
	OLESTATUS			oStatus;

	// We must first release all objects in the document
	ReleaseObjects();

	DeleteObjects();

	// Now revoke access to this document
	oStatus = OleRevokeClientDoc(LibHandle());

	// We're not supposed to wait for this one
	I_ASSERT(oStatus != OLE_WAIT_FOR_RELEASE);

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::Save
//
// DESC:			Saves document and all objects 
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientDoc::Save(
	VOID
	)
	{
	LONG							lEnumKey		= AOBJ_GETFIRSTKEY;
	BOOL							fPrompted		= FALSE;
	BOOL							fUpdate			= TRUE;						// really update open objs
	BOOL							fOpen;
	OBJECTID					oId;
	SROleClientObj*		pObj;
	OLESTATUS					oStatus;

	// First prompt for update on any open objects

	while ((oId = AObjGetNextObject(OTYPE_OLE1CLIENT, &lEnumKey)) != 0)
		{
		pObj = GetObjById(oId);

		if (pObj)
			{
			fOpen = pObj->ObjectIsOpen();

			if (fOpen && !fPrompted)
				{
				// Prompt with "Contains open objects.  Update open objects?"
				// set fUpdate accordingly
				if (MessageBox(GetActiveWindow(),
													"Document contains open embedded objects."
													"  Update before saving?", "OLE",
													MB_YESNO | MB_TASKMODAL) == IDYES)
					fUpdate = TRUE;

				fPrompted = TRUE;
				}

			if (pObj->ObjectChanged() || (fOpen && fUpdate))
				{
				pObj->UpdateObj();
				pObj->Save();
				}
			}
		}

	//!! We really should flush the Windows' queue at this point since
	// some OLE_CHANGED messages might have come through during the
	// UpdateObj().  These will still be in the queue.  These messages don't
	// grab data.

	// All objects should be saved into memory by this time.  Just
	// save the client doc.

	oStatus = OleSavedClientDoc(LibHandle());

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::CreateObjById
//
// DESC:			Creates a new SROleClientObj* and associates it with the
//						OBJECTID.
//
////////////////////////////////////////////////////////////////

SROleClientObj*		SROleClientDoc::CreateObjById(
	OBJECTID					oObject
	)
	{
	SROleClientObj*		pObj;

	pObj = new SROleClientObj(this, oObject);

	if (pObj != NULL)
		AddObjectId(oObject, pObj);

	return pObj;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::GetObjById
//
// DESC:			Retrieves the SROleClientObj* by OBJECTID.
//
////////////////////////////////////////////////////////////////

SROleClientObj*		SROleClientDoc::GetObjById(
	OBJECTID			oObject
	)
	{
	SROleClientObj*		pObj;
	LONG							lObj;

	lObj = m_IdToObj.FindL((LONG) oObject);

	if (lObj != 0)
		{
		pObj = (SROleClientObj*) (VOID*) lObj;

		I_ASSERT(pObj->ObjectId() == oObject);
		}
	else
		pObj = NULL;

	return pObj;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::AddObjectId
//
// DESC:			Associates the OBJECTID with the SROleClientObj* object.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientDoc::AddObjectId(
	OBJECTID					oId,
	SROleClientObj*		pObject
	)
	{
	I_ASSERT(pObject != NULL);

	m_IdToObj.Add((LONG) oId, (LONG) (VOID*) pObject);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::DeleteObjectId
//
// DESC:			Removes the association between OBJECTID and SROleClientObj*.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientDoc::DeleteObjectId(
	OBJECTID		oId
	)
	{
	I_ASSERT(oId != 0);

	m_IdToObj.RemoveL((LONG) oId);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::DeleteObjectId
//
// DESC:			Removes the association between OBJECTID and SROleClientObj*.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientDoc::DeleteObjectId(
	SROleClientObj*		pObj
	)
	{
	I_ASSERT(pObj != NULL);

	m_IdToObj.RemoveR((LONG) (VOID*) pObj);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::InitObjects
//
// DESC:			Enumerates all objects in the document and initializes
//						them by creating an SROleClientObj for each one.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientDoc::InitObjects(
	VOID
	)
	{
	LONG					lEnumKey	= AOBJ_GETFIRSTKEY;
	OBJECTID			oObject;

	m_fInitOk = TRUE;

	while ((oObject = AObjGetNextObject(OTYPE_OLE1CLIENT, &lEnumKey)) != 0)
		{
		if (CreateObjById(oObject) == NULL)
			{
			m_fInitOk = FALSE;
			break;
			}
		}

	return m_fInitOk;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::LoadObjects
//
// DESC:			Loads all objects within the document into memory.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientDoc::LoadObjects(
	VOID
	)
	{
	LONG							lEnumKey		= AOBJ_GETFIRSTKEY;
	OBJECTID					oId;
	SROleClientObj*		pObj;
	SROLEObject*			psrObject;

	I_ASSERT(m_fInitOk == TRUE);

	// This should be called after Init(), and only if Init() succeeds.
	// It assumes that there is a SROleClientObj* object for each AppWare object.

	// During the load of the objects, the SROleClientObj* might be deleted
	// if there is an error in the Load().  This implies two things:  1)
	// we need to be careful in our enumeration, and 2) routines need to
	// be able to correctly respond to a AppWare OLE Object without a
	// SROleClientObj* associated with it.

	while ((oId = AObjGetNextObject(OTYPE_OLE1CLIENT, &lEnumKey)) != 0)
		{
		pObj = GetObjById(oId);

		I_ASSERT(pObj != NULL);				// during the load, it should be 1-to-1

		psrObject = (SROLEObject*) AObjGetRuntimeInfo(oId);

		if (psrObject->RuntimeInfo() & ORI_HASDATA)
			pObj->Load();
		}

	return TRUE;				//!! why always TRUE?
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::ResetMenus
//
// DESC:			Resets all menus referenced by the OLE objects to
//						a disabled "&Object..." item.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientDoc::ResetMenus(
	VOID
	)
	{
	LONG							lEnumKey		= AOBJ_GETFIRSTKEY;
	OBJECTID					oId;
	SROleClientObj*		pObj;
	SROLEObject*			psrObject;

	I_ASSERT(m_fInitOk == TRUE);

	// This should be called after Init(), and only if Init() succeeds.
	// It assumes that there is a SROleClientObj* object for each AppWare object.

	while ((oId = AObjGetNextObject(OTYPE_OLE1CLIENT, &lEnumKey)) != 0)
		{
		pObj = GetObjById(oId);

		psrObject = (SROLEObject*) AObjGetRuntimeInfo(oId);

		if (psrObject->MenuObject() != NULL)
			pObj->ResetMenu(psrObject->MenuObject(),
											psrObject->MenuInsertPos());
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::ReleaseObjects
//
// DESC:			Releases all objects from memory.  They can be accessed
//						again by loading them again.  This should be done during
//						shutdown of document.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientDoc::ReleaseObjects(
	VOID
	)
	{
	LONG							lEnumKey		= AOBJ_GETFIRSTKEY;
	OBJECTID					oId;
	SROleClientObj*		pObj;

	while ((oId = AObjGetNextObject(OTYPE_OLE1CLIENT, &lEnumKey)) != 0)
		{
		pObj = GetObjById(oId);

		if (pObj)
			pObj->Release();
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::DeleteObjects
//
// DESC:			Deletes all objects from memory.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientDoc::DeleteObjects(
	VOID
	)
	{
	LONG							lEnumKey		= AOBJ_GETFIRSTKEY;
	OBJECTID					oId;
	SROleClientObj*		pObj;

	while ((oId = AObjGetNextObject(OTYPE_OLE1CLIENT, &lEnumKey)) != 0)
		{
		pObj = GetObjById(oId);

		if (pObj)
			delete pObj;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::DeleteObj
//
// DESC:			Deletes the given object from the document.  All storage
//						is reclaimed at this point.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientDoc::DeleteObj(
	SROleClientObj*		pObject
	)
	{
	BOOL							fDeleted	= FALSE;

	I_ASSERT(pObject != NULL);

	if (m_IdToObj.FindR((LONG) (VOID*) pObject) != 0)
		{
		// Remove from our list
		m_IdToObj.RemoveR((LONG) (VOID*) pObject);

		// Kill OLE object
		pObject->Delete();

		// Kill SROleClientObj
		delete pObject;

		fDeleted = TRUE;
		}

	return fDeleted;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientDoc::Wait
//
// DESC:			Waits for either the given object to become non-busy, or
//						until the busy object count goes to zero.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientDoc::Wait(
	BOOL*							pFlag,					// wait until this becomes FALSE
	SROleClientObj*		pObject					// = NULL; if non-NULL, wait on this object, else
																		//  else wait for all objects
	)
	{
	BOOL							fContinue;
	MSG								msg;

	(*pFlag) = TRUE;

	for (;;)
		{
		if (pObject != NULL)					// wait on a particular object
			fContinue = ((*pFlag) == TRUE ||
										OleQueryReleaseStatus(pObject->OleObject()) == OLE_BUSY);
		else
			fContinue = (m_lBusyCount > 0);
	
		if (fContinue)
			{
			if (GetMessage(&msg, NULL, NULL, NULL))
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
		else
			break;
		}
	}
