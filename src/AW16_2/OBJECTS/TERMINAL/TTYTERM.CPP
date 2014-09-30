////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"ttyterm.cpp"
//
// AUTHOR: 
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <ctype.h>
#include "terminal.h"

TTYTerm::TTYTerm(OBJECTID oiObj): RUNTMNL (oiObj)
{
}
TTYTerm::~TTYTerm()
{
}
int far TTYTerm::RedrawTerminal (void)
{
		char *szChars ;

		nNumChars = RUNTMNL::RedrawTerminal() ;
		if (nNumChars <0) return (FALSE) ;

		/*szChars = new char[nNumChars+1];
		nNumChars = comm->GetString (szChars, nNumChars+1) ;
		OemToAnsi (szChars, szChars) ;
		ProcessChars (szChars, nNumChars) ;*/
		//ProcessChars (comm->lpCommMem, nNumChars) ; //change by Feb 16
		ProcessChars (CommGetMem(oiConn), nNumChars) ;

		InvalidateRect (hwnd, NULL, FALSE) ;
		return (TRUE) ;
}
BOOL TTYTerm::ProcessChars (LPSTR szChars, int nNumChars)
{
		int   i ;
    LPSTR lpCurrChar ;

		for(i=0, lpCurrChar=szChars; i<nNumChars; i++,lpCurrChar++)
		{
    		DisplayChar (*lpCurrChar) ;
		}
    return (TRUE) ;
}
void TTYTerm::KeyDown (int keyCode)
{
	switch(keyCode)
		{
		case VK_RETURN:
    	ReturnKey();
			return;

		case VK_BACK:
		case VK_DELETE:
	    PutChar (keyCode) ;
			return ;

		default:
    	break;
		}

	PutChar ((char)keyCode) ;
}