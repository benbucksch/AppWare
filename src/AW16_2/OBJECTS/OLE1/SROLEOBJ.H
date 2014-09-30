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
// FILE:					SROLEOBJ.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Definitions for the SROleClientObj.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SROLEOBJ_H
#define SROLEOBJ_H

#include "ushort.h"
#include <ole.h>

////////////////////////////////////////////////////////////////
//
// #define's
//
////////////////////////////////////////////////////////////////

// Wait if status is OLE_WAIT_FOR_RELEASE
#define WAIT_IF_WFR(os,ob,bl)						\
		{																		\
		if ((os) == OLE_WAIT_FOR_RELEASE)		\
			os = (ob)->Wait(bl);							\
		}

////////////////////////////////////////////////////////////////
//
// STRUCT:	SROleClientOleObj
//
// DESC:		Equivalent to an expanded OLECLIENT structure.  It is used
//					to communicate with the OLE libraries.
//
////////////////////////////////////////////////////////////////

struct SROleClientOleObj
	{
	LPOLECLIENTVTBL			m_ClientVtbl;				// This MUST be the first field
	SROleClientObj*			m_Owner;						// Points back to object that handles
																					//  everything
	HWND								m_hWnd;							// window to post notifications to
	};

////////////////////////////////////////////////////////////////
//
// CLASS:			SROleClientObj
//
// DESC:			Manages an OLE object.  The use-model is:  a new instance of
//						the class is created; then the OLE object itself is either
//						created or loaded from a stream.  This object sits between the
//						AppWare OLE Object and the Windows OLE object.  It exists for
//						the lifetime of the AppWare OLE Object.
//
////////////////////////////////////////////////////////////////

class SROleClientObj
	{
	private:
		SROleClientOleObj				m_OleObj;					// used for communication from libraries
		LPOLEOBJECT							m_pOleObject;			// used for communication to libraries
		SROleClientDoc*					m_OwnerDoc;				// owner document
		OBJECTID								m_ObjectId;				// AppWare object ID

		MEMBLOCKID							m_mbMenus;				// menus with ids
		short										m_nVerbs;					// # of items in above menu
		HWND										m_LastActiveHwnd;	// used when an object closes

		RECT										m_rcSize;					// size in HIMETRIC units
		BOOL										m_fValidSize;			// m_rcSize is valid
		LONG										m_lObjectType;		// OT_LINK, OT_EMBEDDED
		OLEOPT_UPDATE						m_LinkUpdateOpt;	// currently always oleupdate_onsave
		BOOL										m_fOpen;					// currently activated
		BOOL										m_fChanged;				// changed since last save
		BOOL										m_fWaiting;				// waiting for OLE_RELEASE
		BOOL										m_fInCreate;			// TRUE => in middle of create
		BOOL										m_fExists;				// TRUE => OLE data exists
		BOOL										m_fNameSet;				// have we done OleSetHostNames()?
		LONG										m_lObjName;				// unique within document
		ATOM										m_aClass;					// class of object
		ATOM										m_aFile;					// for linked objects:  has filename

		// Methods
		VOID										SetNames(VOID);		// calls SetHostNames() if not already done
		VOID										PostCallback(WORD);
		VOID										PostEvent(SOLE_EVENT);

	public:
														SROleClientObj(SROleClientDoc*, OBJECTID);
													 ~SROleClientObj(VOID);

		int											Callback(OLE_NOTIFICATION, BOOL);

		LPOLEOBJECT							OleObject(VOID)				{return m_pOleObject;}
		SROleClientDoc*					OwnerDoc(VOID)				{return m_OwnerDoc;}
		OBJECTID								ObjectId(VOID)				{return m_ObjectId;}
		LONG										ObjectType(VOID)			{return m_lObjectType;}
		VOID										ObjectClass(LPSTR, int);
		BOOL										ObjectIsOpen(VOID);
		BOOL										ObjectChanged(VOID)		{return m_fChanged;}
		LONG										ObjectName(VOID)			{return m_lObjName;}
		VOID										ObjectName(LPSTR, int);
		VOID										ObjectBounds(LPRECT);
		HWND										LastActiveHwnd(VOID)	{return m_LastActiveHwnd;}

		BOOL										Create(VOID);						// creates remembered type
		BOOL										CreateFromClip(VOID);		// from clipboard
		BOOL										Create(LPCSTR);					// creates predefined type
		BOOL										CreateLink(LPCSTR, LPCSTR, LPCSTR);	// from file,item
		BOOL										Load(VOID);							// datablocks => objdata
		BOOL										Save(VOID);							// objdata => datablocks

		VOID										PostObjectChanged(VOID);// posts and ObjectChanged event
		VOID										UpdateInfo(VOID);				// grabs all info about object
		VOID										UpdateObj(VOID);				// updates from client libs

		BOOL										Close(VOID);						// terminates connection with server
		BOOL										Release(VOID);					// releases from memory
		BOOL										Delete(VOID);						// releases and deletes;
																										//  should be followed by 'delete pobj;'

		BOOL										Activate(short nVerb);			// activates with given verb
		BOOL										ActivateByMenu(short);			// activates by menu id
		BOOL										Draw(HDC, LPRECT);					// draws the object
		BOOL										ChangeMenu(OBJECTID, short);// updates menu with verbs
		BOOL										ResetMenu(OBJECTID, short);	// undoes changes from ChangeMenu()

		OLESTATUS								Wait(BOOL fDontBlock);	// wait until object is released
	};

#endif	// !SROLEOBJ_H
