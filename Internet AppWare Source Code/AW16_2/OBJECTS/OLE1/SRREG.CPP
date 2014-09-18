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
// FILE:					SRREG.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Provides a high-level interface into the Registration
//								database functions that are used by OLE.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <shellapi.h>
#include "ushort.h"
#include <ole.h>
#include <a_alm.h>
#include <intdebug.h>

#include "srole.h"
#include "srreg.h"

////////////////////////////////////////////////////////////////
//
// FUNCTION:	FillClassList
//
// DESC:			Fills the combobox passed in with class descriptions of
//						OLE servers.
//
////////////////////////////////////////////////////////////////

int			FillClassList(
	HWND		hCombo
	)
	{
	char				szTmp[MAXKEY];
	char				szClass[MAXKEY];
	char				szKey[MAXKEY];
	LONG				lSize;
	int					nKey;
	HKEY				hRoot;

	I_ASSERT(hCombo != NULL);

	SendMessage(hCombo, CB_RESETCONTENT, 0, 0);

	if (RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hRoot) == ERROR_SUCCESS)
		{
		for (nKey = 0; ; nKey++)
			{
			if (RegEnumKey(hRoot, nKey, szClass, MAXKEY) != ERROR_SUCCESS)
				break;

			// check if it has the required info for an OLE server
			wsprintf(szTmp, "%s\\protocol\\StdFileEditing\\server", (LPSTR) szClass);

			lSize = MAXKEY;
			if (RegQueryValue(hRoot, szTmp, szKey, &lSize) == ERROR_SUCCESS)
				{
				// We have a server entry, so get the class description
				lSize = MAXKEY;
				if (RegQueryValue(hRoot, szClass, szKey, &lSize) == ERROR_SUCCESS)
					{
					SendMessage(hCombo, CB_ADDSTRING, 0, (LONG) (LPVOID) szKey);
					}
				}
			}

		RegCloseKey(hRoot);
		}

	return nKey;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ClassFromDescription
//
// DESC:			Returns the class name given the description of the class.
//						FALSE is returned if the class can't be found.
//
////////////////////////////////////////////////////////////////

BOOL	ClassFromDescription(
	LPCSTR	pszDesc,										// in:   description to find
	LPSTR		pszClass,										// out:  class that has the description
	UINT		nBufSize										// in:   sizeof pszClass buffer
	)
	{
	BOOL		fSuccess					= FALSE;
	LONG		lSize;
	HKEY		hRoot;
	char		szClass[MAXKEY];
	int			nKey;

	I_ASSERT(pszDesc != NULL && pszClass != NULL);

	if (RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hRoot) == ERROR_SUCCESS)
		{
		for (nKey = 0; ; nKey++)
			{
			lSize = nBufSize;

			if (RegEnumKey(hRoot, nKey, szClass, MAXKEY) != ERROR_SUCCESS)
				break;												// no more subkeys

			if ((RegQueryValue(hRoot, szClass, pszClass, &lSize) == ERROR_SUCCESS) &&
					(strcmp(pszClass, pszDesc) == 0))
				{
				strncpy(pszClass, szClass, nBufSize);
				fSuccess = TRUE;
				break;
				}
			}

		RegCloseKey(hRoot);
		}

	return fSuccess;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	DescriptionFromClass
//
// DESC:			Returns the class description given the name of the class.
//						FALSE is returned if the class can't be found.
//
////////////////////////////////////////////////////////////////

BOOL	DescriptionFromClass(
	LPCSTR	pszClass,										// in:   class to find
	LPSTR		pszDesc,										// out:  description of class
	UINT		nBufSize										// in:   sizeof pszClass buffer
	)
	{
	BOOL		fSuccess					= FALSE;
	LONG		lSize;
	HKEY		hRoot;

	I_ASSERT(pszDesc != NULL && pszClass != NULL);

	if (RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hRoot) == ERROR_SUCCESS)
		{
		lSize = nBufSize;

		if (RegQueryValue(hRoot, pszClass, pszDesc, &lSize) == ERROR_SUCCESS)
			fSuccess = TRUE;
 
		RegCloseKey(hRoot);
		}

	return fSuccess;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	EnumObjectVerbs
//
// DESC:			Builds a NULL-separated, double NULL-terminated, list
//						of verbs.  The number of verbs within the string is returned.
//
////////////////////////////////////////////////////////////////

int			EnumObjectVerbs(
	LPCSTR	pszClass,										// in:   class whose verbs are enumerated
	LPSTR		pszVerbs,										// out:  <verb0>\0<verb1>\0...\0
	int			nBufSize										// in:   size of pszVerbs
	)
	{
	int			nVerb				= 0;
	LONG		nTmp;
	HKEY		hKey;
	HKEY		hVerbKey;
	char		szTmp[8];

	I_ASSERT(pszClass != NULL && pszVerbs != NULL && nBufSize > 0);

	if (RegOpenKey(HKEY_CLASSES_ROOT, pszClass, &hKey) == ERROR_SUCCESS)
		{
		if (RegOpenKey(hKey, "protocol\\StdFileEditing\\verb", &hVerbKey) ==
					ERROR_SUCCESS)
			{
			for (nVerb = 0; ; nVerb++)
				{
				wsprintf(szTmp, "%d", nVerb);
				nTmp = nBufSize;

				if (RegQueryValue(hVerbKey, szTmp, pszVerbs, &nTmp) != ERROR_SUCCESS)
					break;

				pszVerbs += ((int) nTmp);
				nBufSize -= ((int) nTmp);
				}

			// Add second null terminator
			*pszVerbs = '\0';

			RegCloseKey(hVerbKey);
			}

		RegCloseKey(hKey);
		}

	return nVerb;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SearchVerbs
//
// DESC:			Searches the list of verbs for the given verb.  Non-alphanumeric
//						chars in the list of verbs are ignored (this allows the use
//						of & for underlining in menus).
//
////////////////////////////////////////////////////////////////

int			SearchVerbs(
	LPCSTR	pszClass,										// in:  class whose verbs are search
	LPCSTR	pszVerb											// in:  search for this verb
	)
	{
	BOOL		fFound						= FALSE;
	int			nVerb;
	int			nVerbCount;
	LPSTR		pszVerbs;
	LPSTR		pSrc;
	LPSTR		pDst;
	char		szVerbs[MAXKEY];
	char		szVerb[32];									// enough for one verb?

	I_ASSERT(pszClass != NULL && pszVerb != NULL);

	nVerbCount = EnumObjectVerbs(pszClass, szVerbs, sizeof(szVerbs));
	pszVerbs   = szVerbs;

	for (nVerb = 0; nVerb < nVerbCount; nVerb++)
		{
		strncpy(szVerb, pszVerbs, sizeof(szVerb));		// get working copy of verb

		// Delete any non-alphanumeric chars
		for (pSrc = pDst = szVerb; *pSrc != '\0';)
			{
			if (isalnum(*pSrc) || isspace(*pSrc))
				*(pDst++) = *(pSrc++);
			else
				pSrc++;
			}
		*pDst = '\0';

		if (stricmp(szVerb, pszVerb) == 0)
			{
			fFound = TRUE;
			break;
			}

		// skip to next verb
		pszVerbs += (strlen(pszVerbs) + 1);
		}

	// default to primary verb if not found.
	if (!fFound)
		nVerb = OLEVERB_PRIMARY;

	return nVerb;
	}

