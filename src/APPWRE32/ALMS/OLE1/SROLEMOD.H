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
// FILE:					SROLEMOD.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Manages AppWare OLE Object module-level information.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SROLEMOD_H
#define SROLEMOD_H

////////////////////////////////////////////////////////////////
//
// CLASS:		SROleModule
//
// DESC:		Contains module-level information.  There is exactly one
//					instance of this class during runtime.
//
////////////////////////////////////////////////////////////////

class SROleModule
	{
	private:
		static SROleModule*		s_OleModule;
		static BOOL						s_fRegistered;							// Comm window class was registered
		static LPSTR					s_szCommClass;							// classname of Comm window
		static HINSTANCE			s_hInstance;								// instance of DLL
		static AEVENTID				s_OLEEventTypes[SOLE_EVENT_LAST];

													SROleModule(VOID);
												 ~SROleModule(VOID);

		static BOOL						RegisterComm(VOID);					// registers Comm wnd class
		static VOID						UnregisterComm(VOID);				// unregisters Comm wnd class

	public:
		static BOOL						Init(HINSTANCE);
		static VOID						Cleanup(VOID);
		static SROleModule*		OleModule(VOID)							{return s_OleModule;}
		static HINSTANCE			Instance(VOID)							{return s_hInstance;}

		HWND									NewCommWindow(VOID);

		AEVENTID								OleEventType(SOLE_EVENT s)	{return s_OLEEventTypes[s];}
	};


// Declare the single instance of this class.  (!!is this the best way?)
#define OLEMODULE					(SROleModule::OleModule())

#endif	// !SROLEMOD_H
