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
// FILE:					SROLE.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Common definitions for the AppWare OLE Object module
//								classes.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SROLE_H
#define SROLE_H

////////////////////////////////////////////////////////////////
//
// Forward class declarations for header inclusion.

class SROleClientApp;
class SROleClientDoc;
class SROleClientObj;
class SROleStream;

////////////////////////////////////////////////////////////////
//
// Constants

const int OC_MAXNAME		= 32;				// maximum length of OLE object name

////////////////////////////////////////////////////////////////
//
// User messages.  These are posted to the OleCommWindow only.

#define WM_OLECLIENT			(WM_USER + 1)
#define OCB_DELETE				100
#define OCB_CHANGED				101
#define OCB_SAVED					102
#define OCB_CLOSED				103

////////////////////////////////////////////////////////////////
//
// Signals from an OLE Object to function chains.

typedef enum
	{
	SE_CREATED					= 0,					// sent during creation
	SE_DELETED,												// sent after deletion
	SE_ACTIVATED,											// sent after activation
	SE_DEACTIVATED,										// sent after deactivation
	SE_SIZECHANGE											// sent if size changes
	}	SOLE_EVENT;

#define SOLE_EVENT_LAST		(SE_SIZECHANGE + 1)

// ****************************************************************

#define OTYPE_OLE1CLIENT			ATYPE_MAKEID('O', 'l', 'e', '1')

#endif	// !SROLE_H

