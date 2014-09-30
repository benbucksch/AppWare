//////////////////////////////////////////////////////////////
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
// FILE:					SROLEOBJ.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Manages Windows' OLE Objects.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <string.h>
#include <windows.h>
#include "ushort.h"
#include <ole.h>
#include <a_alm.h>
#include <intdebug.h>
#include <o_menus.h>

#include "srole.h"
#include "ole_obj.h"
#include "ole_data.h"
#include "srreg.h"
#include "srolemod.h"
#include "sroleapp.h"
#include "sroledoc.h"
#include "sroleobj.h"
#include "srolstrm.h"

//////////////// Globals

char										g_szStdFileEditing[]	= "StdFileEditing";

//////////////// Local statics

int ALMCALLBACK _export	SROleClientObjCallback(
	LPOLECLIENT						pClient,
	OLE_NOTIFICATION			nNotification,
	LPOLEOBJECT						pObject
	);

static OLECLIENTVTBL		vtblClient					=		{
																									SROleClientObjCallback
																								};

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SROleClientObjCallback
//
// DESC:			This is the main callback function for getting notifications
//						from the OLE libraries.  This is really a pass-through to the
//						method on the C++ client object.
//
////////////////////////////////////////////////////////////////

int ALMCALLBACK _export	SROleClientObjCallback(
	LPOLECLIENT						pClient,
	OLE_NOTIFICATION			nNotification,
	LPOLEOBJECT						pObject
	)
	{
	SROleClientObj*				pSRObject;

	I_UNUSED(pObject);

	I_ASSERT(pClient != NULL);
	I_ASSERT(pObject != NULL);

	pSRObject = ((SROleClientOleObj*) pClient)->m_Owner;

	// This should be the same as what we are associated with (otherwise we
	// shouldn't have received the callback)
	I_ASSERT(pSRObject->OleObject() == pObject);

	return pSRObject->Callback(nNotification, FALSE);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj constructor
//
// DESC:			Initializes a new SROleClientObj:  sets up the link between
//						the OLECLIENT and this C++ instance; fills in the vtbl for
//						OLECLIENT.
//
////////////////////////////////////////////////////////////////

SROleClientObj::SROleClientObj(
	SROleClientDoc*				pOwner,					// document that contains object
	OBJECTID							oId
	) :
		m_pOleObject(NULL),
		m_OwnerDoc(pOwner),
		m_ObjectId(oId),
		m_mbMenus(NULL),
		m_LastActiveHwnd(NULL),
		m_fValidSize(FALSE),
		m_fOpen(FALSE),
		m_fChanged(FALSE),
		m_fWaiting(FALSE),
		m_fInCreate(FALSE),
		m_fExists(FALSE),
		m_fNameSet(FALSE),
		m_aClass(NULL),
		m_aFile(NULL)
	{
	I_ASSERT(pOwner != NULL);

	m_OleObj.m_ClientVtbl = &vtblClient;
	m_OleObj.m_Owner      = this;					// establish link back to this object
	m_OleObj.m_hWnd       = m_OwnerDoc->HwndDoc();

	m_lObjName            = m_OwnerDoc->NextObjName();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj destructor
//
// DESC:			Cleans up the object.  Assumes that the Windows OLE object
//						has been released or deleted.
//
////////////////////////////////////////////////////////////////

SROleClientObj::~SROleClientObj(
	VOID
	)
	{
	I_ASSERT(m_pOleObject == NULL);

	if (m_aClass != NULL)
		{
		DeleteAtom(m_aClass);
		m_aClass = NULL;
		}

	if (m_aFile != NULL)
		{
		DeleteAtom(m_aFile);
		m_aFile = NULL;
		}

	if (m_mbMenus != NULL)
		AMemFree(m_mbMenus);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::PostCallback
//
// DESC:			Posts the given message to the dummy window.  The message
//						will eventually make it back to the Callback method.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::PostCallback(
	WORD		wMsg
	)
	{
	PostMessage(m_OwnerDoc->HwndDoc(), WM_OLECLIENT, wMsg, (LONG) (VOID*) this);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::PostEvent
//
// DESC:			Posts the given AppWare Event to the system queue.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::PostEvent(
	SOLE_EVENT		sEvent
	)
	{
	AEvtPostSignalAtTail(ObjectId(), (LONG) OLEMODULE->OleEventType(sEvent));
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::SetNames
//
// DESC:			Performs a SetHostNames() if it hasn't been done already.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::SetNames(
	VOID
	)
	{
	OLESTATUS					oStatus;

	I_ASSERT(OleObject() != NULL);

	if (!m_fNameSet)
		{
		LPCSTR					pszAppName		= m_OwnerDoc->OwnerApp()->AppName();

		// The last two parameters are:  AppName, Document.  Since
		// we equate them, we just deal with AppName.
		oStatus = OleSetHostNames(OleObject(), pszAppName, pszAppName);

		WAIT_IF_WFR(oStatus, this, FALSE);

		m_fNameSet = TRUE;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Callback
//
// DESC:			Performs callback semantics.  This is called both directly
//						by the callback function, as well as by the posted message (the
//						messages are posted here).
//
////////////////////////////////////////////////////////////////

int	SROleClientObj::Callback(
	OLE_NOTIFICATION		nNotification,
	BOOL								fPosted
	)
	{
	int									nRet		= 0;

	I_UNUSED(fPosted);

	switch (nNotification)
		{
		case OLE_CHANGED:								// not sent for embedded objects
			m_fExists  = TRUE;						// we must exist for it to be changed
			m_fChanged = TRUE;

			PostCallback(OCB_CHANGED);
//			ADBG_PRINT_I("OLE_CHANGED");
			break;

		case OLE_SAVED:
			m_fExists = TRUE;						// we have data, so don't delete on OLE_CLOSED

			// OleCreateLinkFromFile() sends an OLE_SAVED during the create, but
			// we fail on the update in OleQuerySize() (it returns OLE_ERROR_BLANK).
			// We will do the update after the successful create.
			if (!m_fInCreate)
				PostCallback(OCB_SAVED);
//			ADBG_PRINT_I("OLE_SAVED");
			break;

		case OLE_CLOSED:							// not sent for linked objects ??????!!
			if (!m_fExists)
				// delete object since server never supplied data
				PostCallback(OCB_DELETE);
			else
				PostCallback(OCB_CLOSED);

			SetFocus(LastActiveHwnd());
//			ADBG_PRINT_I("OLE_CLOSED");
			break;

		case OLE_RENAMED:							// nop for us
//			ADBG_PRINT_I("OLE_RENAMED");
			break;

		case OLE_QUERY_PAINT:
			nRet = TRUE;								// continue painting
//			ADBG_PRINT_I("OLE_QUERY_PAINT");
			break;

		case OLE_RELEASE:
			m_fWaiting = FALSE;
			m_OwnerDoc->DelBusy();

			//!! The following assumes that we never get the OLE_CLOSED on the
			//    (incorrectly) created object.  If we do get OLE_CLOSED, then
			//    we will delete the object twice.
			if (m_fInCreate && OleQueryReleaseError(OleObject()) != OLE_OK)
				PostCallback(OCB_DELETE);

//			ADBG_PRINT_I("OLE_RELEASE");
			break;

		case OLE_QUERY_RETRY:
			nRet = TRUE;								// continue trying to talk to server
//			ADBG_PRINT_I("OLE_QUERY_RETRY");
			break;


		// The following cases are for the PostMessage()s that were posted from here
		case OCB_DELETE:
			{
			SROleClientDoc*		pDoc	= m_OwnerDoc;
			OBJECTID					oiObj	= ObjectId();

			// Delete 'this' since we created but never saved the object
			pDoc->DeleteObj(this);
			pDoc->CreateObjById(oiObj);
//			ADBG_PRINT_I("+OCB_DELETE");
			break;
			}

		case OCB_CHANGED:						// only linked objects receive this
			UpdateInfo();
			PostObjectChanged();
//			ADBG_PRINT_I("+OCB_CHANGED");
			break;

		case OCB_SAVED:							// only embedded objects receive this
		case OCB_CLOSED:
			UpdateInfo();
			PostObjectChanged();
			Save();

			if (nNotification == OCB_CLOSED)
				{
				PostEvent(SE_DEACTIVATED);
//				ADBG_PRINT_I("+OCB_CLOSED");
				}
			else
				{
//				ADBG_PRINT_I("+OCD_SAVED");
				}

			break;

		default:
			I_ASSERT(0);
			break;
		}

	return nRet;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::ObjectClass
//
// DESC:			Fills in the buffer with the ASCII object class.  This is
//						retrieved via the atom.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::ObjectClass(
	LPSTR		pBuf,											// fill in here
	int			nSize											// buf is this size
	)
	{
	I_ASSERT(pBuf != NULL && nSize > 0);

	GetAtomName(m_aClass, pBuf, nSize);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::ObjectIsOpen
//
// DESC:			Returns TRUE if the object is in the active state.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::ObjectIsOpen(
	)
	{
	BOOL					fIsOpen;

	if (OleObject())
		fIsOpen = (OleQueryOpen(OleObject()) == OLE_OK);
	else
		fIsOpen = FALSE;

	return fIsOpen;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::ObjectName
//
// DESC:			Fills in the buffer with the ASCII object name.  The format
//						of the name is:  #xxxxx   where 'xxxxx' is replaced by the
//						unique id of the object within the document.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::ObjectName(
	LPSTR		pBuf,											// fill in here
	int			nSize											// buf is this size
	)
	{
	char		szName[OC_MAXNAME];

	I_ASSERT(pBuf != NULL);
	I_ASSERT(nSize >= 0);
	I_ASSERT(sizeof(szName) > sizeof("#99999"));		// max string we generate

	wsprintf(szName, "#%05ld", m_lObjName);

	strncpy(pBuf, szName, nSize - 1);
	pBuf[strlen(szName)] = '\0';
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::ObjectBounds
//
// DESC:			Returns the bounds of the object in MM_HIMETRIC units
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::ObjectBounds(
	LPRECT		prcBounds
	)
	{
	I_ASSERT(prcBounds);

	if (OleObject() != NULL)			// we have valid bounds
		{
		*prcBounds = m_rcSize;
		}
	else
		{
		prcBounds->left     =
			prcBounds->right  =
			prcBounds->top    = 
			prcBounds->bottom = 0;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Load
//
// DESC:			Creates a new object by loading it from AppWare memory.
//
//						If FALSE is returned, then the object will have been deleted
//						from the document, and the C++ object will be destroyed.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::Load(
	VOID
	)
	{
	OLESTATUS			oStatus;
	char					szName[OC_MAXNAME];
	WORD					cfFormat;
	HANDLE				hData;
	LPSTR					pData;
	SROleStream		srStream(this, TRUE);

	if (srStream.Init(0))
		{
		ObjectName(szName, sizeof(szName));

		oStatus = OleLoadFromStream(srStream.OleStream(), g_szStdFileEditing,
																(LPOLECLIENT) &m_OleObj,
																m_OwnerDoc->LibHandle(), szName, &m_pOleObject);

		WAIT_IF_WFR(oStatus, this, FALSE);

		srStream.Term();
		}
	else
		oStatus = OLE_ERROR_GENERIC;			// stream init failed for some reason.

	if (oStatus != OLE_OK)
		{
		// Failure:  delete object
		m_OwnerDoc->DeleteObj(this);
		}

	else																// success
		{
		m_fExists = TRUE;

		I_ASSERT(OleObject() != NULL);

		// Grab type/open information about the object
		UpdateInfo();

		// Now look for either ownerlink or objectlink formats.  Although this
		// could be done in a more general place, we only need it during the
		// load.
		cfFormat = 0;

		for (;;)
			{
			cfFormat = OleEnumFormats(OleObject(), cfFormat);

			if (cfFormat == NULL)
				break;

			if (cfFormat == m_OwnerDoc->OwnerApp()->OwnerLinkCF())
				break;
			else if (cfFormat == m_OwnerDoc->OwnerApp()->ObjectLinkCF())
				break;
			}

		I_ASSERT(cfFormat != NULL);					// serious problems if neither one exists

		// Now get the class of the object with OwnerLink information
		oStatus = OleGetData(OleObject(), cfFormat,	&hData);

		I_ASSERT(oStatus == OLE_OK || oStatus == OLE_WARN_DELETE_DATA);

		// Format of data is: <class>\0<doc>\0<item>\0\0
		if ((hData != NULL) && (pData = (LPSTR) GlobalLock(hData)) != NULL)
			{
			I_ASSERT(m_aClass == NULL);
			m_aClass = AddAtom(pData);

			if (m_lObjectType == OT_LINK)
				{
				pData += (strlen(pData) + 1);

				I_ASSERT(m_aFile == NULL);
				m_aFile = AddAtom(pData);

				UpdateObj();
				}

			GlobalUnlock(hData);
			}

		if (oStatus == OLE_WARN_DELETE_DATA)
			GlobalFree(hData);
		
		PostObjectChanged();
		}

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Save
//
// DESC:			Saves an existing object to AppWare memory.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::Save(
	VOID
	)
	{
	BOOL						fSaved;
	SROleStream			srStream(this, FALSE);
	OLESTATUS				oStatus;
	DWORD						dwSize;

	oStatus = OleQuerySize(OleObject(), &dwSize);

	I_ASSERT(oStatus == OLE_OK);

	if (srStream.Init(dwSize))
		{
		oStatus = OleSaveToStream(OleObject(), srStream.OleStream());

		srStream.Term();
		}

	if ((fSaved = (oStatus == OLE_OK)) == TRUE)
		{
		SROLEObject*		psrObject		= (SROLEObject*) AObjGetRuntimeInfo(ObjectId());

		psrObject->SetRuntimeInfo(ORI_HASDATA);

		m_fChanged = FALSE;
		}

	return fSaved;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Create
//
// DESC:			Creates a new object of the class stored in the config
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::Create(
	VOID
	)
	{
	BOOL					fCreated								= FALSE;
	SROLEObject*	pObject;
	OLECFGDATA*		pCfg;
	LPCSTR				pszType;
	char					szClass[MAXKEY];

	pObject = (SROLEObject*) AObjGetRuntimeInfo(m_ObjectId);

	pCfg = new (pObject) OLECFGDATA(pObject);

	if (pCfg)
		{
		pszType = pCfg->ObjectType();

		if (pszType != NULL &&
			ClassFromDescription(pszType, szClass, sizeof(szClass)))
			{
			fCreated = Create(szClass);
			}

		pCfg->Destroy(pObject);
		}

	return fCreated;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::CreateFromClip
//
// DESC:			Creates a new object from the clipboard contents.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::CreateFromClip(
	VOID
	)
	{
	BOOL					fCreated								= FALSE;
	OLESTATUS			oStatus;
	char					szName[OC_MAXNAME];
	HANDLE				hData;
	LPSTR					pData;

	I_ASSERT(OleObject() == NULL);

	if (OleQueryCreateFromClip(g_szStdFileEditing, olerender_draw, 0) == OLE_OK &&
			OpenClipboard(m_OwnerDoc->HwndDoc()))
		{
		ObjectName(szName, sizeof(szName));

		m_fInCreate = TRUE;

		// we have an ole object on the clipboard
		oStatus = OleCreateFromClip(g_szStdFileEditing,
																(LPOLECLIENT) &m_OleObj,
																m_OwnerDoc->LibHandle(), szName, &m_pOleObject,
																olerender_draw, 0);

		WAIT_IF_WFR(oStatus, this, FALSE);

		if (oStatus == OLE_OK)
			{
			PostEvent(SE_CREATED);
			PostEvent(SE_ACTIVATED);

			// Now get the class of the object with OwnerLink information
			oStatus = OleGetData(OleObject(),
										m_OwnerDoc->OwnerApp()->OwnerLinkCF(), &hData);

			I_ASSERT(oStatus == OLE_OK || oStatus == OLE_WARN_DELETE_DATA);

			// Format of data is: <class>\0<doc>\0<item>\0\0
			if ((hData != NULL) && (pData = (LPSTR) GlobalLock(hData)) != NULL)
				{
				I_ASSERT(m_aClass == NULL);
				m_aClass = AddAtom(pData);
				GlobalUnlock(hData);
				}

			if (oStatus == OLE_WARN_DELETE_DATA)
				GlobalFree(hData);

			UpdateInfo();
			PostObjectChanged();
			SetNames();
			}

		m_fInCreate = FALSE;

		CloseClipboard();
		}

	return fCreated;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Create
//
// DESC:			Creates a new object of the given class.  The creation will
//						bring up the object's editor automatically.
//
//						If FALSE is returned, then the object will have been deleted
//						from the document, and the C++ object will be destroyed.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::Create(
	LPCSTR					pszClass
	)
	{
	OLESTATUS				oStatus;
	char						szName[OC_MAXNAME];

	// Remember this window so we can set focus back to this when the
	// activation is done.  Do this BEFORE we create the object.
	m_LastActiveHwnd = GetFocus();

	ObjectName(szName, sizeof(szName));

	// If we get an OLE_RELEASE during the create, then we need to delete
	// the object.  The OLE_RELEASE code will check for this flag
	m_fInCreate = TRUE;

	oStatus = OleCreate(g_szStdFileEditing, (LPOLECLIENT) &m_OleObj, pszClass,
												m_OwnerDoc->LibHandle(), szName, &m_pOleObject,
												olerender_draw, 0);

	WAIT_IF_WFR(oStatus, this, FALSE);

	if (oStatus == OLE_OK)
		{
		PostEvent(SE_CREATED);
		PostEvent(SE_ACTIVATED);

		m_aClass  = AddAtom(pszClass);
		UpdateInfo();
		PostObjectChanged();
		SetNames();
		}

	m_fInCreate = FALSE;

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::CreateLink
//
// DESC:			Creates a new linked object of the given class.  The object's
//						server will not be shown for editing purposes, leaving the
//						object in the closed state.
//
//						If FALSE is returned, then the object will have been deleted
//						from the document, and the C++ object will be destroyed.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::CreateLink(
	LPCSTR					pszClass,					// class of object to create
	LPCSTR					pszFile,					// link to this file
	LPCSTR					pszItem						// and this item within file
	)
	{
	OLESTATUS				oStatus;
	char						szName[OC_MAXNAME];

	ObjectName(szName, sizeof(szName));

	// If we get an OLE_RELEASE during the create, then we need to delete
	// the object.  The OLE_RELEASE code will check for this flag
	m_fInCreate = TRUE;

	oStatus = OleCreateLinkFromFile(g_szStdFileEditing, (LPOLECLIENT) &m_OleObj,
															pszClass, pszFile, pszItem, 
															m_OwnerDoc->LibHandle(), szName, &m_pOleObject,
															olerender_draw, 0);

	WAIT_IF_WFR(oStatus, this, FALSE);

	if (oStatus == OLE_OK)
		{
		PostEvent(SE_CREATED);
		// We don't do SE_ACTIVATED here since we don't go into the object editor

		m_fExists = TRUE;
		m_aClass  = AddAtom(pszClass);
		m_aFile   = AddAtom(pszFile);

		// Perform these actions here since we disabled the update on the
		// OLE_SAVED message (since OleQuerySize() fails then)...
		UpdateInfo();
		PostObjectChanged();
		Save();
		}

	m_fInCreate = FALSE;

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::PostObjectChanged
//
// DESC:			Posts and object changed event.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::PostObjectChanged(
	VOID
	)
	{
	AEvtPostStandard(ObjectId(), AEVENT_OBJECTCHANGED, 0,
										AEvtGetObjectChangedPriority(AEVT_ATTAIL |
																								AEVT_KILLDUPLICATES));
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::UpdateInfo
//
// DESC:			Updates cached information about an object.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::UpdateInfo(
	VOID
	)
	{
	OLESTATUS				oStatus;
	RECT						rcNewBounds;

	I_ASSERT(OleObject() != NULL);

	oStatus = OleQueryType(OleObject(), &m_lObjectType);
	I_ASSERT(oStatus == OLE_OK);

	oStatus = OleQueryBounds(OleObject(), &rcNewBounds);
	// might not be okay here because the object is null??

	// The following is a baaaad place to put this check.  The check for new
	// size should really be somewhere else, but every caller of this would
	// have to change...  sigh...
	if (oStatus == OLE_OK)
		{
		if (m_fValidSize &&
				(
					(m_rcSize.left   != rcNewBounds.left)   ||
					(m_rcSize.bottom != rcNewBounds.bottom) ||
					(m_rcSize.top    != rcNewBounds.top)    ||
					(m_rcSize.right  != rcNewBounds.right)
				))
			{
			PostEvent(SE_SIZECHANGE);
			}

		m_rcSize = rcNewBounds;
		m_fValidSize = TRUE;
		}

	if (m_lObjectType == OT_LINK)
		{
		oStatus = OleGetLinkUpdateOptions(OleObject(), &m_LinkUpdateOpt);
		I_ASSERT(oStatus == OLE_OK);
		}

	m_fOpen = (OleQueryOpen(OleObject()) == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::UpdateObj
//
// DESC:			Updates an OLE object.  Updating the object will get
//						new presentation and native data.  It does NOT do a save
//						of the object.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientObj::UpdateObj(
	VOID
	)
	{
	OLESTATUS				oStatus;

	I_ASSERT(OleObject() != NULL);

	oStatus = OleUpdate(OleObject());

	WAIT_IF_WFR(oStatus, this, FALSE);

	UpdateInfo();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Close
//
// DESC:			Closes the open object, thereby terminating the connection
//						with the server.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::Close(
	VOID
	)
	{
	OLESTATUS		oStatus;

	I_ASSERT(OleObject() != NULL);
	I_ASSERT(ObjectIsOpen());

	oStatus = OleClose(OleObject());

	WAIT_IF_WFR(oStatus, this, FALSE);

	UpdateInfo();

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Release
//
// DESC:			Releases the OLE object from memory (closing it if it was
//						open).  A LoadFromStream() can be done later.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::Release(
	VOID
	)
	{
	OLESTATUS		oStatus;
	BOOL				fReleased;

	if (OleObject())
		{
		oStatus = OleRelease(OleObject());

		WAIT_IF_WFR(oStatus, this, FALSE);

		m_pOleObject = NULL;						// since it was released from memory

		fReleased = (oStatus == OLE_OK);
		}
	else
		{
		fReleased = FALSE;
		}

	return fReleased;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Delete
//
// DESC:			Releases and then deletes the OLE object.  This should be done
//						when the object is deleted from the document.  This call should
//						be followed by a delete of this class instance.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientObj::Delete(
	VOID
	)
	{
	SROLEObject*	psrObject				= (SROLEObject*) AObjGetRuntimeInfo(ObjectId());
	OLESTATUS			oStatus;

	if (OleObject())
		{
		oStatus = OleDelete(OleObject());

		WAIT_IF_WFR(oStatus, this, FALSE);

		m_pOleObject = NULL;						// since it was released from memory

		PostEvent(SE_DELETED);
		PostObjectChanged();

		psrObject->PostObjectEvent(AEVT_G_RESETMENU);
		}
	else
		{
		oStatus = OLE_OK;
		}

	psrObject->DeleteData();

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Activate
//
// DESC:			Activates the given object.
//
////////////////////////////////////////////////////////////////

BOOL		SROleClientObj::Activate(
	short				nVerb										// in:  activate with this verb
	)
	{
	OLESTATUS		oStatus;

	I_ASSERT(OleObject() != NULL);

	// Remember this window so we can set focus back to this when the
	// activation is done.
	m_LastActiveHwnd = GetFocus();

	SetNames();

	// The last two parameters (HWND, LPRECT) are not used, so I pass NULL...
	oStatus = OleActivate(OleObject(), nVerb, TRUE, TRUE, NULL, NULL);

	WAIT_IF_WFR(oStatus, this, FALSE);

	m_fOpen = (OleQueryOpen(OleObject()) == OLE_OK);

	if (oStatus == OLE_OK)
		PostEvent(SE_ACTIVATED);

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::ActivateByMenu
//
// DESC:			Activates the given object's verb that is specified by
//						the menu pick.
//
////////////////////////////////////////////////////////////////

BOOL		SROleClientObj::ActivateByMenu(
	short							nMenuId
	)
	{
	short							nVerb;
	short							nVerbStart;
	short							nVerbStop;
	ITEMSPEC*					pMenus;

	ADBG_ASSERT_E(nMenuId > 0, return FALSE, "Invalid menu id for ole activation");
	I_ASSERT(m_nVerbs == 0 || m_mbMenus != NULL);

	if (m_nVerbs > 0 && ((pMenus = (ITEMSPEC*) AMemLock(m_mbMenus)) != NULL))
		{
		if (m_nVerbs > 1)
			{
			nVerbStart = 1;
			nVerbStop  = m_nVerbs + 1;
			}
		else
			{
			nVerbStart = 0;
			nVerbStop  = 1;
			}

		for (nVerb = nVerbStart; nVerb < nVerbStop; nVerb++)
			{
			if (pMenus[nVerb].itemID == nMenuId)
				break;
			}

		nVerb -= nVerbStart;

		AMemUnlock(m_mbMenus);

		// If we didn't find it, then activate with the primary verb
		if (nVerb >= m_nVerbs)
			{
			I_ASSERT(0);
			nVerb = 0;
			}
		}
	else
		nVerb = 0;

	return Activate(nVerb);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Draw
//
// DESC:			Draws the given object.
//
////////////////////////////////////////////////////////////////

BOOL		SROleClientObj::Draw(
	HDC				hDC,										// in:  draw to this context
	LPRECT		pRect										// in:  draw within this area of hDC
	)
	{
	OLESTATUS		oStatus;

	I_ASSERT(hDC != NULL);
	I_ASSERT(pRect != NULL);

	if (OleObject() != NULL)
		oStatus = OleDraw(OleObject(), hDC, pRect, NULL, NULL);
	else
		oStatus = OLE_ERROR_GENERIC;

	return (oStatus == OLE_OK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::ChangeMenu
//
// DESC:			Modifies the menu to contain the verbs of the object.
//						This is done the way the style guide suggests.
//
////////////////////////////////////////////////////////////////

BOOL		SROleClientObj::ChangeMenu(
	OBJECTID	oiMenu,									// in:  owner menu of menuitem to change
	short			nMenuItemIdx						// in:  index of menuitem within menu
	)
	{
	BOOL			fChanged					= FALSE;
	BOOL			fSubmenu;
	int				nVerbCount;
	int				nItem;
	char			szVerbs[MAXKEY];
	char			szClass[MAXKEY];
	char			szDesc[MAXKEY];
	LPSTR			pszVerbs;
	ITEMSPEC*	pMenus;

	I_ASSERT(oiMenu != NULL);

	ObjectClass(szClass, sizeof(szClass));

	nVerbCount = EnumObjectVerbs(szClass, szVerbs, sizeof(szVerbs));

	if (DescriptionFromClass(szClass, szDesc, sizeof(szDesc)))
		{
		// Always delete the menu and recreate it.  This will NOT cause
		// problems if we have never created the menu, because the menu
		// system will NOT delete non-foreign menus.
		OMenuDeletePosition(oiMenu, nMenuItemIdx, 1);

		// delete old information
		if (m_mbMenus != NULL)
			{
			AMemFree(m_mbMenus);
			m_mbMenus = NULL;
			m_nVerbs  = 0;
			}

		if (nVerbCount < 2)
			{
			// make the menuitem say "Edit <class> Object..."
			fSubmenu = FALSE;

			m_mbMenus = AMemAllocate(sizeof(ITEMSPEC) * 1);

			if ((m_mbMenus != NULL) &&
					((pMenus = (ITEMSPEC*) AMemLock(m_mbMenus)) != NULL))
				{
				if (nVerbCount < 1)
					strcpy(szVerbs, "Edit");

				wsprintf(pMenus[0].itemText, "%s %s &Object", szVerbs, szDesc);
				pMenus[0].status = 0x0;

				AMemUnlock(m_mbMenus);
				}
			}
		else
			{
			// make the popup say "<class> Object" and the menu items under it
			// contain the verbs
			fSubmenu = TRUE;

			m_mbMenus = AMemAllocate(sizeof(ITEMSPEC) * (nVerbCount + 1));

			if ((m_mbMenus != NULL) &&
					((pMenus = (ITEMSPEC*) AMemLock(m_mbMenus)) != NULL))
				{
				wsprintf(pMenus[0].itemText, "%s &Object", szDesc);
				pMenus[0].status = 0x0;

				pszVerbs = szVerbs;

				for (nItem = 1; nItem <= nVerbCount; nItem++)
					{
					pMenus[nItem].status = 0x0;
					strcpy(pMenus[nItem].itemText, pszVerbs);
					pszVerbs += (strlen(pszVerbs) + 1);
					}

				AMemUnlock(m_mbMenus);
				}
			}

		if (m_mbMenus != NULL)
			{
			m_nVerbs = nVerbCount;

			if (fSubmenu)
				fChanged = (OMenuAddSubmenu(m_ObjectId, oiMenu,
																		m_mbMenus, nMenuItemIdx) != NULL);
			else
				fChanged = OMenuAddItems(m_ObjectId, oiMenu,
																		m_mbMenus, nMenuItemIdx);

#ifdef AW_I_DEBUG
			pMenus = (ITEMSPEC*) AMemLock(m_mbMenus);
			if (nVerbCount > 1)
				{
				for (nItem = 1; nItem <= nVerbCount; nItem++)
					{
					ADBG_ASSERT_E(pMenus[nItem].itemID != 0,, "Invalid menu id in OLE menu");
					}
				}
			else
				{
				ADBG_ASSERT_E(pMenus[0].itemID != 0,, "Invalid menu id in OLE menu");
				}
			AMemUnlock(m_mbMenus);
#endif
			}
		}

	return fChanged;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::ResetMenu
//
// DESC:			Clears out any verbs for the OLE object from the menu.
//
////////////////////////////////////////////////////////////////

BOOL		SROleClientObj::ResetMenu(
	OBJECTID	oiMenu,									// in:  owner menu of menuitem to change
	short			nMenuItemIdx						// in:  index of menuitem within menu
	)
	{
	BOOL				fReset;
	MEMBLOCKID	mbMenus;
	ITEMSPEC*		pMenu;

	I_ASSERT(oiMenu != NULL);

	fReset = OMenuDeletePosition(oiMenu, nMenuItemIdx, 1);

	if (m_mbMenus != NULL)
		{
		AMemFree(m_mbMenus);
		m_mbMenus = NULL;
		m_nVerbs  = 0;
		}

	mbMenus = AMemAllocate(sizeof(ITEMSPEC));

	if ((mbMenus != NULL) &&
			((pMenu = (ITEMSPEC*) AMemLock(mbMenus)) != NULL))
		{
		strcpy(pMenu->itemText, "&Object");
		pMenu->status = OMENU_DISABLED;

		fReset = OMenuAddItems(m_ObjectId, oiMenu, mbMenus, nMenuItemIdx);
		AMemUnlock(mbMenus);
		}

	if (mbMenus != NULL)
		AMemFree(mbMenus);

	return fReset;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientObj::Wait
//
// DESC:			Waits for the object to receive OLE_RELEASE
//
////////////////////////////////////////////////////////////////

OLESTATUS		SROleClientObj::Wait(
	BOOL				fDontBlock						// TRUE => return without waiting
	)
	{
	// Bump up busy count since OLE_RELEASE decrements it
	m_OwnerDoc->AddBusy();

	// In some cases, we don't want to wait immediately.  Instead we want to
	// group together many Wait()s and wait for them all.  Releasing all
	// objects is an example.  While it certainly could be done one at a time,
	// we want to allow 'block' operations
	if (!fDontBlock)
		{
		// Wait on this particular object
		m_OwnerDoc->Wait(&m_fWaiting, this);
		}

	// Return the real status of the operation
	return OleQueryReleaseError(OleObject());
	}

