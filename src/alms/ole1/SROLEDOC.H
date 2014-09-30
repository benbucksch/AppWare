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
// FILE:					SROLEDOC.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Defines an OLE object holder.  This holder is generally
//								known as a document.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SROLEDOC_H
#define SROLEDOC_H

#include "ushort.h"
#include <ole.h>
#include "srassoc.h"

////////////////////////////////////////////////////////////////
//
// CLASS:		SROleClientDoc
//
// DESC:		Manages OLE documents.  This is a runtime-only object that
//					is used to manage collections of OLE objects (real OLE objects,
//					compared to AppWare OLE Object).
//
////////////////////////////////////////////////////////////////

class SROleClientDoc
	{
	private:
		SROleClientApp*					m_OwnerApp;						// who owns this doc
		LHCLIENTDOC							m_LibHandle;					// OLE-supplied id
		LONG										m_lNextObjName;				// unique within document
		HWND										m_hCommWindow;				// simple window to receive messages
		LONG										m_lBusyCount;					// # of objects with OLE_WAIT_FOR...
		ATOM										m_aDocName;						// usually filepath
		SRAssoc									m_IdToObj;						// associates OBJID to SROleClientObj*
		BOOL										m_fInitOk;						// initialization was OK

	public:
														SROleClientDoc(SROleClientApp*);
													 ~SROleClientDoc(VOID);

		BOOL										Register(LPSTR);			// done during bootup
		BOOL										Revoke(VOID);					// done during shutdown; release all objects
		BOOL										Save(VOID);						// saves doc & all objects

		SROleClientObj*					CreateObjById(AOBJECTID);
		SROleClientObj*					GetObjById(AOBJECTID);

		VOID										AddObjectId(AOBJECTID, SROleClientObj*);
		VOID										DeleteObjectId(AOBJECTID);
		VOID										DeleteObjectId(SROleClientObj*);

		BOOL										InitObjects(VOID);		// initializes all objects
		BOOL										LoadObjects(VOID);		// loads all objects
		VOID										ResetMenus(VOID);			// resets all menus
		VOID										ReleaseObjects(VOID);	// releases all objects
		VOID										DeleteObjects(VOID);

		BOOL										DeleteObj(SROleClientObj*);

		VOID										UpdateLinks(VOID);

		HWND										HwndDoc(VOID)					{return m_hCommWindow;}
		LHCLIENTDOC							LibHandle(VOID)				{return m_LibHandle;}
		LONG										NextObjName(VOID)			{return m_lNextObjName++;}
		SROleClientApp*					OwnerApp(VOID)				{return m_OwnerApp;}

		LONG										BusyCount(VOID)				{return m_lBusyCount;}
		LONG										AddBusy(VOID)					{return ++m_lBusyCount;}
		LONG										DelBusy(VOID)					{return --m_lBusyCount;}

		VOID										Wait(BOOL*, SROleClientObj* = NULL);
	};



#endif	// !SROLEDOC_H

