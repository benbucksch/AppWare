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
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
// Object Group:	Essentials
// Object:		  Printer
//
// Filename:	"print.hpp"
//
// Author:	Kevin Smart
//   Start:	July 24, 1992
//   Finish:	July 24, 1992
//
// Description:
//	PRINTER class definition -- PRINTER manages the printer object
//
////////////////////////////////////////////////////////////////////////////

#ifndef	__PRINT_HPP
#define __PRINT_HPP

#ifndef		A_ALM_H			// necessary for "data" handling
#include	<a_alm.h>
#include	<a_almutl.h>
#endif

#ifndef	__COMMDLG_H		// necessary for Common Dialog Routines
#include <commdlg.h>
#endif

#ifndef __PRINT_H		// necessary for LPDEVMODE information
#include <print.h>
#endif


////////////////////////////////////////////////////////////////////////////

#define	PD_PERSISTENTMASK		(PD_COLLATE | PD_PRINTTOFILE | PD_PAGENUMS)
 

////////////////////////////////////////////////////////////////
//	Definitions for the default values of the page settings.
//	If the FromPage and ToPage are set to this values
//	then all of the pages will be printed.
////////////////////////////////////////////////////////////////

#define	DFLT_FROMPAGE		-1
#define	DFLT_TOPAGE			-1


////////////////////////////////////////////////////////////////////////////

struct DATA0
{
	int		ver;
	DWORD	PersistentFlags;
	UINT	FromPage;
	UINT	ToPage;
	BOOL	LandScapeMode;
};


////////////////////////////////////////////////////////////////////////////

class PRINTER
{
  private:
    PRINTDLG		pd;			// PRINTDLG structure for common dialog
												// pd.hDevMode is a handle to data 1
												// pd.hDevNames is a handle to data 2
		DATA0*			data0;	// version, collate, & print to file is in data 0
    OBJECTID		theObject;

  public:
    // constructor & destructor
		PRINTER(OBJECTID, HWND = NULL);
    ~PRINTER();

    // inline methods
		void				SetOwner(HWND hwnd)			{pd.hwndOwner = hwnd;}
    HDC					GetDC()									{return pd.hDC;}
		BOOL				DeleteDC()							{return ::DeleteDC(pd.hDC);}
		OBJECTID		GetObjectID()						{return theObject;}

		BOOL				GetLandScapeMode()			{return data0->LandScapeMode;}

		void			SetPersistentFlags()				{data0->PersistentFlags = PD_PERSISTENTMASK & pd.Flags;}
		void			SetLandScapeMode(BOOL mode)	{data0->LandScapeMode = mode;}

		DWORD			PersistentFlags()				{return data0->PersistentFlags;}
		void			SetMaxPage(UINT max)		{pd.nMaxPage=max;}
    void			SetMinPage(UINT min)		{pd.nMinPage=min;}
    void			SetFromPage(UINT from)	{pd.nFromPage=from;}
		void			SetToPage(UINT to)			{pd.nToPage=to;}
    int				Version()								{return data0->ver;}

		// inline methods included later
    LPDEVMODE		GetDevMode();
    BOOL				RelDevMode();
    LPDEVNAMES	GetDevNames();
    BOOL				RelDevNames();

    // other method prototypes
    static UINT far pascal _export PrintHook(HWND, WORD, WORD, long);
		BOOL	PrintDlg(DWORD = PD_RETURNDC | PD_NOSELECTION | PD_USEDEVMODECOPIES);
    UINT	FromPage();
    UINT	ToPage();
};


////////////////////////////////////////////////////////////////////////////

inline LPDEVMODE	PRINTER::GetDevMode()
{
	if(pd.hDevMode)
		return (LPDEVMODE)GlobalLock(pd.hDevMode);
	else
		return NULL;
}


////////////////////////////////////////////////////////////////////////////

inline BOOL	PRINTER::RelDevMode()
{
	if(pd.hDevMode)
		return GlobalUnlock(pd.hDevMode);
	else
		return NULL;
}


////////////////////////////////////////////////////////////////////////////

inline LPDEVNAMES	PRINTER::GetDevNames()
{
	if(pd.hDevNames)
		return (LPDEVNAMES)GlobalLock(pd.hDevNames);
	else
		return NULL;
}


////////////////////////////////////////////////////////////////////////////

inline BOOL	PRINTER::RelDevNames()
{
	if(pd.hDevNames)
		return GlobalUnlock(pd.hDevNames);
	else
		return NULL;
}

////////////////////////////////////////////////////////////////////////////

#endif	//#ifdef __PRINT_HPP
