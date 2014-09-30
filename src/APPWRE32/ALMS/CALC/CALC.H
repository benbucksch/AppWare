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
// Filename:	"calc.h"
//
// Author:	Kevin Smart
//   Start:	July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	CALCULATION class header for object management
//
////////////////////////////////////////////////////////////////////////////

struct CALCHEADER {
  int		    ver;
//  WORD	    count;
  UINT	    count;
  BOOL	    autoUpdate;
  AOBJECTID	resultID;
  };

////////////////////////////////////////////////////////////////////////////

class CALCULATION {
  private:
		AOBJECTID		theObject;

		AOBJECTID*		variableOBJIDs;  // list of object variables
    char*				calcExpression;	// expression string
    CALCHEADER*	calcHeader;	// number of variables, autoupdate, result object ID
    STRING*			variableNames; // pointer to string of aliases

	public:
		CALCULATION(AOBJECTID objID, BOOL wantExpression);
		CALCULATION(AOBJECTID objID);
		~CALCULATION();

		BOOL	SetExpression( char*, long);
		const char*	Expression()					{return calcExpression;}
    AOBJECTID		Result()							{return calcHeader->resultID;}
		BOOL AutoUpdate() {
			if(calcHeader == NULL)
				return FALSE ;
			else
				return calcHeader->autoUpdate ;
			}

    void				InitDlgControls(HWND hDlg);
		void				FillResult(HWND hDialog, AOBJECTID);
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

		void				DoAssign(AOBJECTID, AOBJECTID);
		void				ImportObject();
		void				ExportObject();
		void				CheckIDs();
		void				Validate(pAObjMessage);
		void				DataObjectDelete(AOBJECTID*, int);
		BOOL				ValidExpression(char* errString);

    BOOL				InVarList(AOBJECTID);

		BOOL        GetNextVariable(LPOBJECTID next, LPLONG cookie);

		STRING*			GetVariableNames();
		AOBJECTID*		GetData0();
		CALCHEADER*	GetData3();

		AOBJECTID 		TheObject()           {return theObject;}

		char* 			Get(WORD index);  // return pointer to string

		BOOL        ResizeData(long index);
		void 				CopyDataPtr	(AOBJECTID, AOBJECTID, long);

  };

////////////////////////////////////////////////////////////////////////////
