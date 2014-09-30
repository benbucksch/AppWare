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
// FILE:					SROLEAPP.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <string.h>
#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>

#include "srole.h"
#include "sroledoc.h"
#include "sroleapp.h"

//////////////// Globals

extern char				g_szStdFileEditing[];

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientApp constructor
//
// DESC:			Initializes a new SROleClientApp.
//
////////////////////////////////////////////////////////////////

SROleClientApp::SROleClientApp(
	LPSTR					pszAppName							// in:  unique name for application
	) :
		m_cfNative(NULL),
		m_cfOwnerLink(NULL),
		m_cfObjectLink(NULL),
		m_Document(NULL)
	{
	I_ASSERT(pszAppName != NULL);
	I_ASSERT(sizeof(m_szAppName) > 1);

	lstrcpyn(m_szAppName, pszAppName, sizeof(m_szAppName) - 1);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientApp destructor
//
// DESC:			Deletes the object.
//
////////////////////////////////////////////////////////////////

SROleClientApp::~SROleClientApp(
	VOID
	)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientApp::Init
//
// DESC:			Initializes the object.  FALSE is returned if there is an
//						error during initialization.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientApp::Init(
	VOID
	)
	{
	m_cfNative     = RegisterClipboardFormat("Native");
	m_cfOwnerLink  = RegisterClipboardFormat("OwnerLink");
	m_cfObjectLink = RegisterClipboardFormat("ObjectLink");

	return (m_cfNative     != NULL &&
					m_cfOwnerLink  != NULL &&
					m_cfObjectLink != NULL);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientApp::Cleanup
//
// DESC:			Cleans up before deletion.
//
////////////////////////////////////////////////////////////////

VOID	SROleClientApp::Cleanup(
	VOID
	)
	{
	// Revoke document

	I_ASSERT(m_Document != NULL);

	m_Document->Save();						// grab data from open objects
	m_Document->Revoke();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientApp::AddDoc
//
// DESC:			Adds a document to the application.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientApp::AddDoc(
	SROleClientDoc*		pDoc								// in:  document to add
	)
	{
	I_ASSERT(pDoc != NULL);
	I_ASSERT(m_Document == NULL);

	m_Document = pDoc;

	return TRUE;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROleClientApp::OLEObjOnClipboard
//
// DESC:			Returns TRUE if an OLE object is on the clipboard.
//
////////////////////////////////////////////////////////////////

BOOL	SROleClientApp::OLEObjOnClipboard(
	VOID
	)
	{
	return (OleQueryCreateFromClip(g_szStdFileEditing, olerender_draw, 0) == OLE_OK);
	}

