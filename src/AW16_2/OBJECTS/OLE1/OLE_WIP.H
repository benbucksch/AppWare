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
// FILE:					OLE_WIP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef OLE_WIP_H
#define OLE_WIP_H

//////////////// Local includes

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

////////////////////////////////////////////////////////////////
//
// CLASS:		OLEObjectVI
//
// DESCR:		Implements the lite-item Window item protocol.
//
////////////////////////////////////////////////////////////////

class OLEObjectVI : public OpLiteWndItem
	{
	private:
		SROLEObject*				m_pObject;							// handle to C++ wrapper

											 ~OLEObjectVI(void);

		static BOOL ALMAPI	EnumProc(OpWndItemD*, LONG, LONG);

		void							DrawFocus(HDC = NULL);

	public:
												OLEObjectVI(OBJECTID	oiItem,
																	OBJECTID	oiParent,
																	RECT*			prcItem,
																	LONG			lStatus);

	
		static void				SetItemActive(OLEObjectVI*, OBJECTID);

		VOID							ObjectBounds(LPRECT);

		void ALMAPI				Destroy(void);				// MUST override Destroy!

		void ALMAPI				DataChanged(long details, long modifiers, long variation);
		void ALMAPI				Paste(void);
		long ALMAPI				Activate(int nCausedBy, BOOL fMustKeep);
		void ALMAPI				Deactivated(void);
		void ALMAPI				ScreenLocationChanged(int causedBy);
		void ALMAPI				Draw(HDC hDC, RECT* prcInvalid);
		void ALMAPI				DoubleClicked(POINT* pptWhere, int nKeysDown, int nButton);
	};

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OLEObjectVICreate
//
////////////////////////////////////////////////////////////////

extern "C"
OpWndItemD* ALMAPI	OLEObjectVICreate(
	OBJECTID		oiItem,
	OBJECTID		oiParent,
	RECT*				prcItem,
	long				lStatus,
	MEMBLOCKID	mbExtraData
	);

#endif	// !OLE_WIP_H
