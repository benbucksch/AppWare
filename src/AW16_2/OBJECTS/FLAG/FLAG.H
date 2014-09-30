////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
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
//--------------------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//	"flag.h"
//
// Header file for the FLAG C++ class
//
////////////////////////////////////////////////////////////////////////////

#ifndef		__FLAG_H	// prevent multiple includes
#define		__FLAG_H

#ifndef		A_ALM_H
#include	<a_alm.h>	// include <oip.h> if necessary
#endif

#include <string.h>

// FLAGDATA structure //////////////////////////////////////////////////////

struct FLAGDATA
	{
	int		version;
	BOOL	isSet;
	BOOL	interruptChains;
  };

// FLAG class //////////////////////////////////////////////////////////////

class FLAG
	{
	private:
		OBJECTID	theObject;
		FLAGDATA*	fdata;			// data 0
		char*			setText;		// data 1
    char*			clearText;	// data 2

	public:
		FLAG(OBJECTID objID);
		FLAG(OBJECTID objID, BOOL partialBuild);

    // inline methods for the FLAG object
		OBJECTID	ObjectID()	{return theObject;}
		int				Version()		{return fdata->version;}
		BOOL			IsSet()			{return fdata->isSet;}
    BOOL			Interrupt()	{return fdata->interruptChains;}
		char*			GetText()		{return fdata->isSet ? setText : clearText;}
		char*			SetText()		{return setText;}
		char*			ClearText()	{return clearText;}
		void			setSetText(char* theString)		{strcpy(setText, theString);}
		void			setClearText(char* theString)	{strcpy(clearText, theString);}

		// Admn Proc methods
		void			Export();
		void			Import();

		// ObjD Proc methods
		void			InitDlgControls(HWND hDlg);
		void			UpdateObject(HWND hDlg);

		// Function methods
    void			Clear();
		void			Set();
		void			Toggle();
	};

////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef __FLAG_H
