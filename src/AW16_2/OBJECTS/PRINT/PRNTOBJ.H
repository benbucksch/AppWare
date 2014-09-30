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
// Filename:	"prntobj.h"
//
// Author:	Kevin Smart
//   Start:	June 24, 1992
//   Finish:	July xx, 1992
//
// Description:
//	This is a header file containing the resource constants
//	and the global class container definition.
//
////////////////////////////////////////////////////////////////////////////

//////////////////////Global Container Definition///////////////////////////

class	GLOBAL {
	public :
	  HINSTANCE	hInst;
	  BOOL			bUserAbort;
		HWND			hPrintDlg;
	};

///////////////////////////////Resource Constants///////////////////////////

#define	IDD_MESSAGE				101
#define IDD_PRINTERINFO		120

/////////////////////////////////Other Constants ///////////////////////////

#define VERSION_OBJECT		0
#define VERSION_DLL				1L

///////////////////////////Function Prototypes//////////////////////////////

extern "C" {
  // Functions in "print_ar.cpp"
	LONG far pascal _export	PrintAdmn				(OBJECTID, pAObjMessage);
	long far pascal _export	GetSerVersion		(TYPEID);

	// Functions in "print_fn.cpp"
  void far pascal _export	PrintDialog			(pAEvtInfo, pAObjMessage);
  void far pascal _export	PrintSetup			(pAEvtInfo, pAObjMessage);
  void far pascal _export	PrintObject			(pAEvtInfo, pAObjMessage);
  void far pascal _export	PrintObjectNow	(pAEvtInfo, pAObjMessage);

  // Functions in "print_md.cpp"
  void far pascal _export	ShutDown				(pAEvtInfo, pAObjMessage);
  }

// other exported functions
BOOL far pascal _export		AbortProc	(HDC, short);
BOOL far pascal _export		PrintProc	(HWND, WORD, WORD, LONG);

// special typedefs
typedef UINT (CALLBACK* HOOKFUNC)(HWND, UINT, WPARAM, LPARAM);

////////////////////////////////////////////////////////////////////////////
