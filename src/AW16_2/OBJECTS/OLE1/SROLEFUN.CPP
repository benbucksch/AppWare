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
// FILE:					SROLEFUN.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Miscellaneous support functions.
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
#include <o_text.h>
#include <o_number.h>

#include "srole.h"
#include "srolemod.h"
#include "sroleapp.h"
#include "srole.h"
#include "srolefun.h"

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SRCurrentDoc
//
// DESC:			Returns the current doc, NULL if there is an error.
//
////////////////////////////////////////////////////////////////

SROleClientDoc*		SRCurrentDoc(
	VOID
	)
	{
	SROleClientApp*		pApp;
	SROleClientDoc*		pDoc;

	pApp = (SROleClientApp*) ATypeGetRuntimeInfo(OTYPE_OLE1CLIENT);

	if (pApp)
		pDoc = pApp->GetDoc();
	else
		pDoc = NULL;

	return pDoc;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SRGetText
//
// DESC:			Returns the text of a OTYPE_TEXT object.
//
////////////////////////////////////////////////////////////////

VOID		SRGetText(
	OBJECTID		oText,
	LPSTR				pszBuf,
	int					nBufSize
	)
	{
	I_ASSERT(pszBuf != NULL);
	I_ASSERT(AObjGetType(oText) == OTYPE_TEXT);

	OTextGetToBuffer(oText, pszBuf, nBufSize);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SRGetNumber
//
// DESC:			Returns the text of a OTYPE_TEXT object.
//
////////////////////////////////////////////////////////////////

LONG		SRGetNumber(
	OBJECTID		oNumber
	)
	{
	return ONmbrGetInteger(oNumber);
	}
