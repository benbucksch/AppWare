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
// FILE:					SROLEMOD.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Manages module-level information.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>

#include "srole.h"
#include "sroleapp.h"
#include "srolefun.h"
#include "srolemod.h"
#include "sroleobj.h"

////////////////////////////////////////////////////////////////
//
// Static member initialization
//
////////////////////////////////////////////////////////////////

LPSTR					SROleModule::s_szCommClass			= "OleCommWindow";
SROleModule*	SROleModule::s_OleModule				= NULL;
BOOL					SROleModule::s_fRegistered			= FALSE;
HINSTANCE			SROleModule::s_hInstance				= NULL;
EVENTID				SROleModule::s_OLEEventTypes[]	= {
														MAKEEVENTID('o', 'l', 'e', 'C'),	// SE_CREATED
														MAKEEVENTID('o', 'l', 'e', 'D'),	// SE_DELETED
														MAKEEVENTID('o', 'l', 'e', 'A'),	// SE_ACTIVATED
														MAKEEVENTID('o', 'l', 'e', 'E'),	// SE_DEACTIVATED
														MAKEEVENTID('o', 'l', 'e', 'S')	// SE_SIZECHANGE
																								};

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SROlePostWndProc
//
// DESC:			This wndproc controls an invisible window that is used
//						to post OLECLIENT messages to.
//
////////////////////////////////////////////////////////////////

//!! This doesn't seem to belong at a module-level, but the window class
//   registration does belong at a module level--what's a mother to do?

LRESULT ALMCALLBACK	_export SROlePostWndProc(
	HWND		hWnd,
	UINT		uMsg,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	if (uMsg == WM_OLECLIENT)
		{
		// wParam == notification code; lParam == object

		SROleClientObj*		pObject		= (SROleClientObj*) (VOID*) lParam;

		pObject->Callback((OLE_NOTIFICATION) wParam, TRUE);
		}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleModule constructor
//
// DESC:			
//
////////////////////////////////////////////////////////////////

SROleModule::SROleModule(
	VOID
	)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleModule destructor
//
// DESC:			
//
////////////////////////////////////////////////////////////////

SROleModule::~SROleModule(
	VOID
	)
	{
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SROleModule::RegisterComm
//
// DESC:			Registers the window class that is used to post OLE messages to.
//
////////////////////////////////////////////////////////////////

BOOL	SROleModule::RegisterComm(
	VOID
	)
	{
	if (!s_fRegistered)
		{
		WNDCLASS			wndClass;

		wndClass.style				 = NULL;
		wndClass.lpfnWndProc	 = SROlePostWndProc;
		wndClass.cbClsExtra		 = 0;
		wndClass.cbWndExtra		 = 0;
		wndClass.hInstance		 = Instance();
		wndClass.hIcon				 = NULL;
		wndClass.hCursor			 = NULL;
		wndClass.hbrBackground = NULL;
		wndClass.lpszMenuName	 = NULL;
		wndClass.lpszClassName = s_szCommClass;

		s_fRegistered = RegisterClass(&wndClass);
		}

	return s_fRegistered;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SROleModule::UnregisterComm
//
// DESC:			Unregisters the window class that is used to post OLE messages to.
//
////////////////////////////////////////////////////////////////

VOID	SROleModule::UnregisterComm(
	VOID
	)
	{
	if (s_fRegistered)
		{
		s_fRegistered = !UnregisterClass(s_szCommClass, Instance());
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleModule::Init
//
// DESC:			Creates the single instance of SROleModule.
//
////////////////////////////////////////////////////////////////

BOOL	SROleModule::Init(
	HINSTANCE				hInst
	)
	{
	I_ASSERT(s_OleModule == NULL);

	s_hInstance = hInst;

	if (RegisterComm())
		s_OleModule = new SROleModule();

	return (s_OleModule != NULL);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleModule::Cleanup
//
// DESC:			Destroys the single instance of SROleModule
//
////////////////////////////////////////////////////////////////

VOID	SROleModule::Cleanup(
	VOID
	)
	{
	I_ASSERT(s_OleModule != NULL);

	delete s_OleModule;

	s_OleModule = NULL;

	UnregisterComm();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleModule::NewCommWindow
//
// DESC:			Creates a new communication window.
//
////////////////////////////////////////////////////////////////

HWND	SROleModule::NewCommWindow(
	VOID
	)
	{
	return CreateWindow(s_szCommClass, NULL, 0, -1, -1, 0, 0, NULL, NULL,
													Instance(), NULL);
	}

