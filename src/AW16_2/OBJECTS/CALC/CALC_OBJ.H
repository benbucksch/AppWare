////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"calc_obj.h"
//
// Author:	Kevin Smart
//   Start:		July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	General header file for the calculation object
//
////////////////////////////////////////////////////////////////////////////


#ifndef CALC_OBJ_H
#define CALC_OBJ_H


#define OTYPE_EXPRESSION								MAKEID('C','a','l','c')
#define 	MAX_EDIT	255
#define		DATACOUNT	4

/////////////////////////////Global Container///////////////////////////////
// This data is global to each task

struct	TASKGLOBALDATA {
	HTASK			hTask;
	BOOL			FPEerror;
	BOOL			MATHerror;
	BOOL			parseError;
	char			error[255];
	FARPROC		lpfnOldProc;
	};

////////////////////////////////////////////////////////////////////////////
// This data is global to the DLL

class GLOBAL {
	private:
		HGLOBAL	hGlobalList;
		void*		pGlobalList;
		long		gCount;

    // private methods
		TASKGLOBALDATA*	GetGlobalData();
		TASKGLOBALDATA*	AddGlobalData();
		TASKGLOBALDATA*	GlobalData();

	public:
		HINSTANCE		hInst;
///    FARPROC			lpfnOldProc;

		// constructor & destructor
		GLOBAL();
		~GLOBAL();

		// methods
		void				DeleteGlobalData();
		FARPROC			OldProc();
		void				SetOldProc(FARPROC oldProc);
		BOOL				ParseError();
		void				SetParseError(BOOL error);
		char*				Error();
		void				SetError(char* error);
		BOOL				FPEerror();
		void				SetFPEerror(BOOL error);
		BOOL				MATHerror();
		void				SetMATHerror(BOOL error);
	};


/////////////////////////Other Function Prototypes//////////////////////////

BOOL	evaluate	(const char*, long double&, char*, OBJECTID,
								 long double* = NULL, BOOL eval = TRUE);

BOOL	EvalExpression	(OBJECTID, char*);


// version constants ///////////////////////////////////////////////////////

#define	VERSION_OBJECT	0
#define	VERSION_DLL			1L

/////////////////////////////////Signals////////////////////////////////////

#define	calcObjectUpdated		0
#define	calcVariableChanged	1
#define	calcExpressionError	2

#define	SIGNALCOUNT					3
#define	OBJECTNAMESIZE			32

#endif
