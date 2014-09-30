///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// listdata.cpp
// Text Builder
// Dialog Box Routines
//
// Robert Lundahl/ Edgar C. Jerez
//
// Start Date 05/28/92
// End   Date 06/../92
///////////////////////////////////////////////////////////////

#include "tf_incld.h"




///////////////////////////////////////////////////////////////
// lISTDATA::LISTDATA: Class Constructor
//
///////////////////////////////////////////////////////////////

LISTDATA::LISTDATA ( VOID )
{
	objid = 0L;
	alias = NULL;
}				// lISTDATA::LISTDATA...


///////////////////////////////////////////////////////////////
// lISTDATA::LISTDATA: Class Constructor
//
///////////////////////////////////////////////////////////////

LISTDATA::LISTDATA ( OBJECTID  id )
{
	if (id != 0L)
	{
		int i;

		string = new BYTE[maxEdit+1];

		AObjGetName((objid=id), (LPSTR)string, maxEdit);

		alias = (BYTE*) new char[lstrlen((LPCSTR)string)+1];
		lstrcpy((LPSTR)alias, (LPCSTR)string);

		// store spaces as underscores in alias name
		// for uniformity with number object
		for (i = 0; alias[i]; i++)
        {
			if (alias[i] == ' ')
				alias[i] = '_';
        }

		delete string;
	}
	else
	{
		objid = 0L;
		alias = NULL;
	}
}				// lISTDATA::LISTDATA...


///////////////////////////////////////////////////////////////
// lISTDATA::LISTDATA: Class Destructor
//
///////////////////////////////////////////////////////////////

LISTDATA::~LISTDATA ( )
{
	if (alias != NULL)
		delete alias;
}				// lISTDATA::~LISTDATA...


///////////////////////////////////////////////////////////////
// lISTDATA::Alias:
//
///////////////////////////////////////////////////////////////

LONG LISTDATA::Alias ( VOID )
{
		return LONG(alias);
}				// lISTDATA::Alias...


///////////////////////////////////////////////////////////////
// lISTDATA::SetAlias:
//
///////////////////////////////////////////////////////////////

VOID LISTDATA::SetAlias ( BYTE* name )
{
	int i;

	if (alias != NULL)
		delete alias;

	alias = (BYTE*)new char[lstrlen((LPCSTR)name)+1];
	lstrcpy((LPSTR)alias, (LPCSTR)name);

	for (i = 0; alias[i]; i++)
    {
		if (alias[i] == ' ')
			alias[i] = '_';
    }
}				// lISTDATA::SetAlias...


///////////////////////////////////////////////////////////////
// lISTDATA::Init:
//
///////////////////////////////////////////////////////////////

VOID LISTDATA::Init ( OBJECTID id, BYTE* name )
{
	objid = id;

	if (alias != NULL)
		delete alias;

	alias = (BYTE*)new char[lstrlen((LPCSTR)name)+1];
	lstrcpy((LPSTR)alias, (LPCSTR)name);
}				// lISTDATA::Init...
