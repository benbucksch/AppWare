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
// FILE:					SRREG.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Function prototypes for registration utility routines.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SRREG_H
#define SRREG_H

////////////////////////////////////////////////////////////////
//
// Function declarations.
//   In general, string parameters that are to be filled in should be
//   of length MAXKEY.
//
////////////////////////////////////////////////////////////////

const int							MAXKEY			= 256;

int			FillClassList(
	HWND		hCombo
	);

BOOL		ClassFromDescription(
	LPCSTR	pszDesc,										// in:   description to find
	LPSTR		pszClass,										// out:  class that has the description
	UINT		nBufSize										// in:   sizeof pszClass buffer
	);

BOOL		DescriptionFromClass(
	LPCSTR	pszClass,										// in:   class to find
	LPSTR		pszDesc,										// out:  description of class
	UINT		nBufSize										// in:   sizeof pszClass buffer
	);

int			EnumObjectVerbs(
	LPCSTR	pszClass,										// in:   class whose verbs are enumerated
	LPSTR		pszVerbs,										// out:  <verb0>\0<verb1>\0...\0
	int			nBufSize										// in:   size of pszVerbs
	);

int			SearchVerbs(
	LPCSTR	pszClass,										// in:  class of object
	LPCSTR	pszVerb											// in:  text of verb
	);

#endif	// !SRREG_H


