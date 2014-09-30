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
  WORD	    count;
  BOOL	    autoUpdate;
  OBJECTID	resultID;
  };

////////////////////////////////////////////////////////////////////////////

class CALCULATION {
  private:
		OBJECTID		theObject;

		OBJECTID*		variableOBJIDs;  // list of object variables
    char*				calcExpression;	// expression string
    CALCHEADER*	calcHeader;	// number of variables, autoupdate, result object ID
    STRING*			variableNames; // pointer to string of aliases

	public:
		CALCULATION(OBJECTID objID, BOOL wantExpression);
		CALCULATION(OBJECTID objID);
		~CALCULATION();

		BOOL	SetExpression( char*, long);
		const char*	Expression()					{return calcExpression;}
    OBJECTID		Result()							{return calcHeader->resultID;}
		BOOL AutoUpdate() {
			if(calcHeader == NULL)
				return FALSE ;
			else
				return calcHeader->autoUpdate ;
			}

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

		void				DoAssign(OBJECTID, OBJECTID);
		void				ImportObject();
		void				ExportObject();
		void				CheckIDs();
		void				Validate(pAObjMessage);
		void				DataObjectDelete(OBJECTID*, int);
		BOOL				ValidExpression(char* errString);

    BOOL				InVarList(OBJECTID);

		BOOL        GetNextVariable(LPOBJECTID next, LPLONG cookie);

		STRING*			GetVariableNames();
		OBJECTID*		GetData0();
		CALCHEADER*	GetData3();

		OBJECTID 		TheObject()           {return theObject;}

		char* 			Get(WORD index);  // return pointer to string

		BOOL        ResizeData(long index);
		void 				CopyDataPtr	(OBJECTID, OBJECTID, long);

  };

////////////////////////////////////////////////////////////////////////////
