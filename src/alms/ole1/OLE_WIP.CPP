/*//////////////////////////////////////////////////////////////
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
// FILE:					OLE_WIP.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Contains implementation of Window Item Protocol for OLE
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
#include <op_wnd.h>
#include <o_wnd.h>

#include "srole.h"
#include "srolefun.h"
#include "sroleapp.h"
#include "sroledoc.h"
#include "sroleobj.h"
#include "ole_obj.h"
#include "ole_wip.h"

////////////////////////////////////////////////////////////////
//
// FUNCTION:		OLEObjectVICreate
//
// DESCR:				Creates a new visual instance of an OLE object.
//
////////////////////////////////////////////////////////////////

extern "C"
OpWndItemD* ALMAPI		OLEObjectVICreate(
	AOBJECTID							oiItem,
	AOBJECTID							oiParent,
	RECT*									prcItem,
	long									lStatus,
	AMEMBLOCKID						mbExtra
	)
	{
	I_UNUSED(mbExtra);

	return new OLEObjectVI(oiItem, oiParent, prcItem, lStatus);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::OLEObjectVI
//
// DESCR:			Initializes a new OLE object visual instance
//
////////////////////////////////////////////////////////////////

OLEObjectVI::OLEObjectVI(
	AOBJECTID		oiItem,
	AOBJECTID		oiParent,
	RECT*				prcItem,
	LONG				lStatus
	) :
		OpLiteWndItem(oiItem, oiParent, prcItem, lStatus)
	{
	I_ASSERT(oiItem != NULL);

	OWndSetItemActivatable(this, TRUE);
	OWndSetItemTransparent(this, TRUE);
	OWndSetItemEnabled(this, TRUE);

	m_pObject = (SROLEObject*) AObjGetRuntimeInfo(oiItem);

	I_ASSERT(m_pObject != NULL);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::~OLEObjectVI
//
// DESCR:			Cleans up an OLE object visual instance.
//
////////////////////////////////////////////////////////////////

OLEObjectVI::~OLEObjectVI(
	void
	)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::EnumProc
//
// DESCR:			Tests if the item is in the active window.  If it is,
//						then it activates that item and terminates the iteration.
//
////////////////////////////////////////////////////////////////

BOOL ALMAPI	OLEObjectVI::EnumProc(
	OpWndItemD*			pItem,
	LONG						lData1,
	LONG						lData2
	)
	{
	BOOL						fContinue;

	I_UNUSED(lData1);
	I_UNUSED(lData2);

	if (pItem == OWndGetActiveItem())
		{
		fContinue = FALSE;
		}
	else if (pItem->m_oiParent == OWndGetActiveWindowObject())
		{
		OWndSetActiveItem(pItem);
		fContinue = FALSE;
		}
	else
		{
		fContinue = TRUE;
		}

	return fContinue;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::SetItemActive
//
// DESCR:			Sets the active item to the given one.  If NULL is
//						passed in, then the visual instances are searched
//						to find one in the active window.
//
////////////////////////////////////////////////////////////////

void	OLEObjectVI::SetItemActive(
	OLEObjectVI*		pItem,
	AOBJECTID				oiObject
	)
	{
	if (pItem)
		{
		OWndSetActiveItem(pItem);
		}
	else
		{
		I_ASSERT(oiObject != NULL);

		OWndCallEachVisualInstance(oiObject, EnumProc, 0, 0);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::Destroy
//
// DESCR:			Destroys a visual instance of an ole object.
//
////////////////////////////////////////////////////////////////

void ALMAPI		OLEObjectVI::Destroy(
	void
	)
	{
	delete this;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::DrawFocus
//
// DESCR:			Draws a focus rectangle around the visual instance.
//
////////////////////////////////////////////////////////////////

void	OLEObjectVI::DrawFocus(
	HDC			hDC				// = NULL
	)
	{
	BOOL		fGetDC		= (hDC == NULL);
	RECT		rcFocus;
	POINT		ptLBRT[2];

	if (fGetDC)
		hDC = GetDC(m_hwndParent);

	if (hDC != NULL)
		{
		// Get to logical to subtract one pixel, then go back
		ptLBRT[0].x = m_rcItem.left;
		ptLBRT[0].y = m_rcItem.bottom;
		ptLBRT[1].x = m_rcItem.right;
		ptLBRT[1].y = m_rcItem.top;
		OWndLPtoDP(m_oiParent, ptLBRT, sizeof(ptLBRT) / sizeof(POINT));

		ptLBRT[0].x += 1;
		ptLBRT[0].y -= 1;
		ptLBRT[1].x -= 1;
		ptLBRT[1].y += 1;
		OWndDPtoLP(m_oiParent, ptLBRT, sizeof(ptLBRT) / sizeof(POINT));

		rcFocus.left   = ptLBRT[0].x + 1;
		rcFocus.bottom = ptLBRT[0].y - 1;
		rcFocus.right  = ptLBRT[1].x - 1;
		rcFocus.top    = ptLBRT[1].y + 1;

		DrawFocusRect(hDC, &rcFocus);

		if (fGetDC)
			ReleaseDC(m_hwndParent, hDC);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::ObjectBounds
//
// DESCR:			Returns the graphical bounds of the object in OWND units
//
////////////////////////////////////////////////////////////////

VOID	OLEObjectVI::ObjectBounds(
	LPRECT		prcBounds
	)
	{
	SROleClientDoc*				pDoc	= SRCurrentDoc();
	SROleClientObj*				pObj;

	I_ASSERT(pDoc != NULL);

	pObj = pDoc->GetObjById(m_pObject->ObjectId());

	if (pObj != NULL)
		{
		pObj->ObjectBounds(prcBounds);

		// now convert to OWND units.  OLE keeps bounds in .01mm units, and
		// OWND keeps them in .1mm units, so we just divide by 10.
		prcBounds->left   /= 10;
		prcBounds->bottom /= 10;
		prcBounds->right  /= 10;
		prcBounds->top    /= 10;

		// now "swap" top and bottom (in himetric, positive y is up)
		prcBounds->bottom *= -1;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::DataChanged
//
// DESCR:			Forces a redraw of the visual instance.
//
////////////////////////////////////////////////////////////////

void ALMAPI		OLEObjectVI::DataChanged(
	LONG							lDetails,
	LONG							lModifiers,
	LONG							lVariation
	)
	{
	RECT									rcNew;

	I_UNUSED(lDetails);
	I_UNUSED(lModifiers);
	I_UNUSED(lVariation);

	OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);

	if (m_pObject->Options() & SOLE_SVRRESIZE)
		{
		ObjectBounds(&rcNew);

		// object is anchored at top left
		m_rcItem.right  = m_rcItem.left + (rcNew.right  - rcNew.left);
		m_rcItem.bottom = m_rcItem.top  + (rcNew.bottom - rcNew.top);

		OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::Paste
//
// DESCR:			Pastes an OLE object from the clipboard.
//
////////////////////////////////////////////////////////////////

void ALMAPI		OLEObjectVI::Paste(
	void
	)
	{
	SROleClientDoc*				pDoc	= SRCurrentDoc();
	SROleClientApp*				pApp;
	SROleClientObj*				pObj;

	pApp = (SROleClientApp*) ATypeGetRuntimeInfo(OTYPE_OLE1CLIENT);

	if (pApp->OLEObjOnClipboard())
		{
		pObj = pDoc->GetObjById(m_pObject->ObjectId());

		if (pObj)
			pDoc->DeleteObj(pObj);

		pObj = pDoc->CreateObjById(m_pObject->ObjectId());

		if (pObj)
			pObj->CreateFromClip();
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::Activate
//
// DESCR:			Forces the OLE menu to be updated with the verbs of the
//						object.
//
////////////////////////////////////////////////////////////////

LONG ALMAPI		OLEObjectVI::Activate(
	int							nCausedBy,
	BOOL						fMustKeep
	)
	{
	LONG									nAct	= OWND_ACTIVATION_PASS;
	SROleClientDoc*				pDoc	= SRCurrentDoc();
	SROleClientObj*				pObj;

	I_UNUSED(nCausedBy);
	I_UNUSED(fMustKeep);

	if (pDoc != NULL)
		{
		pObj = pDoc->GetObjById(m_pObject->ObjectId());

		if (pObj != NULL && pObj->OleObject() != NULL)
			{
			DrawFocus();

			// We need to delay the actual activation stuff since we will
			// get a deactivate before we are really activated.
			m_pObject->PostObjectEvent(AEVT_G_ACTIVATE);

			nAct = OWND_ACTIVATION_KEPT;
			}
		}

	return nAct;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::Deactivated
//
// DESCR:			Restores the OLE menu to the default state.
//
////////////////////////////////////////////////////////////////

void ALMAPI		OLEObjectVI::Deactivated(
	void
	)
	{
	SROleClientDoc*				pDoc	= SRCurrentDoc();
	SROleClientObj*				pObj;

	if (pDoc != NULL)
		{
		pObj = pDoc->GetObjById(m_pObject->ObjectId());

		if (pObj != NULL && pObj->OleObject() != NULL)
			{
			DrawFocus();

			if (m_pObject->MenuObject() != NULL)
				pObj->ResetMenu(m_pObject->MenuObject(),
												m_pObject->MenuInsertPos());
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::ScreenLocationChanged
//
// DESCR:			Refreshes the OLE object by redrawing.  This is necessary
//						for those OLE objects that are sensitive to screen
//						location (e.g., Video for Windows).
//
////////////////////////////////////////////////////////////////

void ALMAPI		OLEObjectVI::ScreenLocationChanged(
	int						causedBy
	)
	{
	I_UNUSED(causedBy);

	// We need to erase the background here, because some OLE objects do not
	// completely fill the space (e.g,. MS Word Art).  This will cause flashing
	// on the objects.
	OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::Draw
//
// DESCR:			Draws the OLE object to the best of its ability.  If there
//						is no (real) OLE object, then the window background is displayed.
//
////////////////////////////////////////////////////////////////

void ALMAPI		OLEObjectVI::Draw(
	HDC							hDC,
	RECT*						prcInvalid
	)
	{
	SROleClientDoc*				pDoc	= SRCurrentDoc();
	SROleClientObj*				pObj;

	I_UNUSED(prcInvalid);
	I_ASSERT(m_pObject != NULL);

	if (!m_pObject->Draw(hDC, &m_rcItem))		// wasn't drawn
		{
		OpLiteWndItem::Draw(hDC, prcInvalid);
		}

	if (pDoc != NULL)
		{
		pObj = pDoc->GetObjById(m_pObject->ObjectId());

		if ((pObj != NULL) &&
				(pObj->OleObject() != NULL) &&
				(OWndGetActiveItem() == this) &&
				OWndGetHandle(m_oiParent) == ::GetActiveWindow())
			{
			DrawFocus(hDC);
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		OLEObjectVI::DoubleClicked
//
// DESCR:			Activates the default verb of the OLE object.
//
////////////////////////////////////////////////////////////////

void ALMAPI		OLEObjectVI::DoubleClicked(
	POINT*					pptWhere,
	int							nKeysDown,
	int							nButton
	)
	{
	SROleClientDoc*				pDoc	= SRCurrentDoc();
	SROleClientObj*				pObj;

	I_UNUSED(pptWhere);
	I_UNUSED(nKeysDown);

	if (nButton == MK_LBUTTON)
		{
		pObj = pDoc->GetObjById(m_pObject->ObjectId());

		if (pObj != NULL && pObj->OleObject() != NULL)
			{
			SetItemActive(this, NULL);
			pObj->Activate(OLEVERB_PRIMARY);
			}
		else
			{
			if (pObj == NULL)
				pObj = pDoc->CreateObjById(m_pObject->ObjectId());

			if (pObj != NULL)
				pObj->Create();
			}
		}
	}
