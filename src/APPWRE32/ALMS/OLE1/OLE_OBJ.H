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
// FILE:					OLE_OBJ.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		AppWare OLE Object datablock definitions.  A C++ wrapper
//								object around the datablocks is defined.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef OLE_OBJ_H
#define OLE_OBJ_H

#include <stdlib.h>					// for size_t declaration

////////////////////////////////////////////////////////////////
//
// CLASS:		SROLEObject
//
// DESC:		Implements a small wrapper around the AppWare OLE Object.
//					It is to be used to manipulate the internal data in a somewhat
//					opaque way for an INSTANTIATED AppWare OLE Object.  Object-type
//					behavior is not handled here (e.g. StartupEvent, ShutdownEvent).
//
////////////////////////////////////////////////////////////////

// These options are valid for the Options() method
typedef unsigned short	OLEOPTIONS;
const OLEOPTIONS				SOLE_EMBEDDED		= 0x1;			// object is embedded
const OLEOPTIONS				SOLE_LINKED			= 0x2;			// object is linked
const OLEOPTIONS				SOLE_SVRRESIZE	= 0x4;			// server can resize Object

#define OLEOPTIONSALL		( \
													SOLE_EMBEDDED | \
													SOLE_LINKED   | \
													SOLE_SVRRESIZE  \
												)


// These options are valid for the m_Rstatus field defined in SROLEObject
typedef unsigned short	OLERUNINFO;
const OLERUNINFO				ORI_VALID			= 0x1;			// has real OLE data
const OLERUNINFO				ORI_HASDATA		= 0x2;			// has real cfg info


// Internal-only event id
#define AOLE_EVT_GENERAL			AEVENT_MAKEID('o','l','e','I')

// These are valid for PostObjectEvent
#define AEVT_G_ACTIVATE					1
#define AEVT_G_RESETMENU				2

class SROLEObject
	{
	private:
		AOBJECTID									m_Object;
		OLERUNINFO								m_Rstatus;									// runtime info

		static BOOL ALMCALLBACK			DlgProc(HWND, UINT, WPARAM, LPARAM);

		BOOL											DlgInit(HWND);
		VOID											DlgGetData(HWND);
		VOID											DlgLoadMenuObjects(HWND);
		VOID											CopyData(AOBJECTID, LONG);
		VOID											Init(VOID);

	public:
															SROLEObject(AOBJECTID o);
														 ~SROLEObject(VOID)								{}

		AOBJECTID									ObjectId(VOID)									{return m_Object;}

		OLERUNINFO								RuntimeInfo(VOID)								{return m_Rstatus;}
		VOID											SetRuntimeInfo(OLERUNINFO s)		{m_Rstatus |=  s;}
		VOID											ClrRuntimeInfo(OLERUNINFO s)		{m_Rstatus &= ~s;}

		VOID											BuildSignals(AMEMBLOCKID);
		VOID											Create(SROleClientDoc*, BOOL);
		VOID											Assign(SROleClientDoc*, AOBJECTID);
		LONG											Export(void);
		LONG											Import(void);
		BOOL											Edit(VOID);
		LONG											CheckState(VOID);
		VOID											ValidateIds(AObjMessage*);
		static LONG								QueryProtocol(ApQueryInfo*);
		VOID											PreWrite(VOID);

		VOID											DeleteData(VOID);

		VOID											ObjectType(LPSTR, int);
		VOID											LinkFile(LPSTR, int);
		VOID											LinkItem(LPSTR, int);
		OLEOPTIONS								Options(VOID);
		AOBJECTID									MenuObject(VOID);
		short											MenuInsertPos(VOID);

		VOID*											CreateDataBlock(DWORD);			// performs lock also
		VOID*											LockDataBlock(VOID);
		VOID											UnlockDataBlock(VOID);

		// General event handling
		VOID											ObjectEvent(AEvtInfo*);
		VOID											PostObjectEvent(LONG lDetails);

		// Methods to support WIP
		BOOL											Draw(HDC, RECT*);
		VOID											Activate(VOID);
	};

////////////////////////////////////////////////////////////////
//
// AppWare Object-required functions.  These are the Admin, Definition, and
// event handlers for the OLE Object.
//
////////////////////////////////////////////////////////////////

extern "C"
{
// Admin routine
LONG ALMCALLBACK		OleAdmin(AOBJECTID, pAObjMessage);

// Event handlers
VOID ALMCALLBACK		OleStartupEvent(pAEvtInfo, pAObjMessage);
VOID ALMCALLBACK		OleShutdownEvent(pAEvtInfo, pAObjMessage);
VOID ALMCALLBACK		OleMenuEvent(pAEvtInfo, pAObjMessage);

// General internal-only event handler
VOID ALMCALLBACK		OleGenEvent(pAEvtInfo, pAObjMessage);
}

#endif	// !OLE_OBJ_H
