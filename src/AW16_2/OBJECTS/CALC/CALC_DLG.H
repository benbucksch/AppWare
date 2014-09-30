////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"calc_dlg.h"
//
// Author:	Kevin Smart
//   Start:	July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	CALCULATION class header for object management
//
////////////////////////////////////////////////////////////////////////////

BOOL far pascal _export	CalcDlg(HWND, UINT, UINT, LONG);
long far pascal _export	NewEditControlProc(HWND, UINT, UINT, LONG);

/////////////////////////////Resource Constants/////////////////////////////

#define IDD_EVALUATE				200
#define IDHELP							998

#define	IDD_AUTO_UPDATE			100

#define	IDD_VARIABLES				110
#define	IDD_OBJECTS					111

#define	IDD_ALIAS						120
#define	IDD_ALIAS_TITLE			121
#define	IDD_RESULT					122

#define	IDD_ADD							130
#define	IDD_REMOVE					131
#define	IDD_EDIT						132

#define	IDD_EXPRESSION			140

#define IDD_OBJECT					201

////////////////////////////////////////////////////////////////////////////
/* Add this later
class   CALC_DLG 
  {
  public:
    void				InitDlgControls(HWND hDlg);
		void				FillResult(HWND hDialog, OBJECTID);
		void				FillObjects(HWND hDialog);
		void				FillVariables(HWND hDialog);

		void				UpdateAddButton(HWND hDialog);
		void				UpdateEditButton(HWND hDialog);
		void				UpdateRemoveButton(HWND hDialog);
		void				UpdateAliasControl(HWND hDialog);
		BOOL				CheckForErrors(HWND hDialog, BOOL);
		void				OBJD_Evaluate(HWND);
		void				UpdateObject(HWND, BOOL = TRUE);

		void				DeleteListData(HWND hDialog);
		void				Add(HWND hDialog);
		void				Remove(HWND hDialog);
		void				Edit(HWND hDialog);
		void				Alias(HWND hDialog);
  };
*/  