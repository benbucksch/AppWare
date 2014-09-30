///////////////////////////////////////////////////////////////
//
//	(c) 1993, 1994 Novell, Inc.  All rights reserved.
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
//	"scrp_bas.h"
//
// Author:	Edgar C. Jerez
//	 Start:		july 28, 1993
//	 Finish:	1994
//
// Description:
//	General header for the scrp object
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SCRP_H			// prevent multiple includes.
#define __SCRP_H

#ifndef A_ALM_H			// neccesary for data handling.
#include	<a_alm.h>
#include	<a_almutl.h>
#endif

#include	"sbi.h"
#include	"sbl.h"

#ifndef __COMMDLG_H			// neccesary for common dialog routines.
#include	<commdlg.h>
#endif


/******************************Resource Constants*************************/

/**************************************************************************
 *	Constants for the scrp resource dialog.
 *************************************************************************/

//#define IDD_SCRPNAME				101
//#define IDD_SCRPFUNCNAME		102
//#define IDD_ARGUMENTS			103
//#define IDD_RETURNVAL			115
//#define IDD_BROWSER				105
//#define	IDD_SCRPCTYPE			110		// radio button, C type function.
//#define	IDD_SCRPPASCALTYPE	111		// radio button, Pascal type function.

//#define scrpTypeID      		MAKEID('s','c','r','p')
#define scrpTypeID      		MAKEID('s','e','c','j')
#define scrpEvent 				MAKEID('S','c','R','p')


/**************************************************************************
 *	Constants for the Function Argument dialog.
 *************************************************************************/

#define IDD_ARGUMENTLST		101
#define IDD_INSERT			102
#define IDD_DEBUGGER			103	// check box, set if debugger is enabled.
#define IDD_DELETE			104
#define IDD_GLOBALVARS		105	// set if script is global environment.
#define IDD_STRUCTURE		106	// User define structure.
#define IDD_SCRIPT			108	// Script function.
#define IDD_VARIABLES		109	// Variables defenition.
#define IDD_OPENSCRIPT		112	// open script button.
#define IDD_ERRORLOG			113	// Compiler error log button.
//#define IDD_UPDATE			103
//#define IDD_PARAMTYPE		105
//#define IDD_OBJECTTYPE		106
//#define IDD_OBJECTNAME		107
//#define IDD_OBJECTVALUE		109


/**************************************************************************
 *	Constants for the Return Value Dialog Box.
 *************************************************************************/

#define IDD_RETPARAMTYPE	101
#define IDD_RETOBJTYPE		102
#define IDD_RETOBJNAME		103
#define IDD_OBJECTCONST		108
#define IDD_OBJECTVALUE		109


/**************************************************************************
 *	Constants for the Function Argument dialog.
 *************************************************************************/

#define	IDD_ERROR_LIST		101


/**************************************************************************
 *	Data block offsets for the scrp object.
 *************************************************************************/

//#define SCRPFUNCNAMEINDEX		0L		// data block index for function name.
//#define DATA_SCRPDATA			0L
#define MIN_DATABLKS				4		// minimum data blocks for ALM.
#define MAX_NUM_OFARGS			100	// Max arguments that can be passed to a function.
#define SCRP_INFO_DBINDEX		0L		// data block index for scrp info.
#define ERROR_LOG_DBINDEX		1L		// Index to data block for return value param.
#define SCRP_TEXT_DBINDEX		2L		// Data block index to script.
#define SCRP_PARAM_DBINDEX		3L		// Index to start data block for arguments.
#define MAX_ERRORS				30		// Max errors in compile log.

#define SCRPRETPARAMDLGINDEX	0L		// Index to return value param in param block.
#define SCRPNAMESIZE		 		64
#define FUNCNAMESIZE	 			64
#define CONSTVALLEN				10		// LENGHT OF CONSTANT VALUE, in bytes (long real).


/**************************************************************************
 *	C type supported in the scrp object.
 *************************************************************************/

#define		BASTYPE_INTEGER		0			// equivalent to short in C.
//#define		CTYPE_USHORT			1
#define		BASTYPE_LONG			1			// equivalent to long in C.
//#define		CTYPE_ULONG				3
#define		BASTYPE_SINGLE			2			// equivalent to float in C.
#define		BASTYPE_DOUBLE			3			// equivalent to double in C.
#define		BASTYPE_CURRENCY		4			// equivalent to fixed in C.
#define		BASTYPE_FIXEDSTR		5			//
#define		BASTYPE_DYNAMICSTR	6			//
#define		NUMOFTYPES				7


/**************************************************************************
 *	Language types supported in the scrp object.
 *************************************************************************/

#define		C_FUNCTION					0		// C type arguments.
#define		PASCAL_FUNCTION			1		// Pascal type arguments.


/**************************************************************************
 *	standard data type, these are used in the readin of the .scrp file.
 *************************************************************************/

typedef	char				CHAR;
typedef	int				INT;
typedef	unsigned short	USHORT;
typedef	unsigned long	ULONG;
typedef	int				HFILE;
typedef	CHAR FAR *		LPSZ;
typedef	long *			PTLONG;


#define VERSION_OBJECT		0



/**************************************************************************
 *	SCRP_FUNCNAME: contains the scrp and function name and info about
 *		the script arguments, script and compiler error log.
 *************************************************************************/

struct SCRP_FUNCNAME {
	int	version;									// scrp version number.
	int	numOfErrorsInLog;						// Number of errors in log file
	char	scrpFunction[SBL_CCH_MAX_NAME];	// Function name.
	BOOL	debuggEnable;							// TRUE - invoke debugger.
	BOOL	globalScript;							// TRUE - global variables.
	BOOL	function;								// TRUE - run a function.
	BOOL	structure;								// TRUE - define global structure.
	BOOL	variables;								// TRUE - define global varables.
//	char	scrpName[SCRPNAMESIZE];			// name of the scrp and path.
//	int		languageType;			// 0 - C, 1 - Pascal.
//	int		numOfArguments;		// Number of arguments.
//	int		scriptDBIndex;			// Data block index to start of script.
};


/**************************************************************************
 *	SCRP_RETARG structure.  Return Argument in a scrp function call
 *	will be stored in scrp object data block 1.
 *************************************************************************/
/*
struct SCRP_RETARG {
	OBJECTID	theObject;		// object id associated with the c param type.
	int				paramType;		// index to c param type.
};
*/

/**************************************************************************
 *	SCRP_ARGUMENTS structure.  Arguments in a scrp function call
 *	are stored in scrp object data blocks. Each data block starting with
 *	data2 contains a structure of SCRP_ARGUMENTS type.  The number of
 *	function arguments can be gotten by CountObjectData-2.
 *************************************************************************/

struct SCRP_ARGUMENTS {
	BOOL			constant;				// TRUE -- argument is constant.
	OBJECTID		theArgObject;			// object id associated with the c param type.
	int			paramType;				// index to c param type.
	long double	constVal;				// value for constant.
	BOOL			byValue;					// TRUE - by value; FALSE - by ref.
	char			theArgObjectName[SBL_CCH_MAX_NAME];	// Appware name of the argument.
};


/**************************************************************************
 *	COMPILER_ERROR_LOG structure.  Log of errors, from previous compile.
 *		The error log is saved in DataBlock 1.
 *************************************************************************/

struct COMPILER_ERROR_LOG {
	SBL_WORD		severity;		// Code indicating severity of error.
	SBL_STATUS	err;				// SBL error or warning code.
   SBL_SHORT	lineNumber;		// line number in file.
};


/**************************************************************************
 *	SER_SCRP class definition.
 *************************************************************************/

class SCRP_OBJECT
{
	private:
		OBJECTID		theObject;

	public:
		SCRP_FUNCNAME* scrpFuncName;		// scrp and function name, in data0.
		SCRP_OBJECT(OBJECTID);
		~SCRP_OBJECT();

			// inline methods
		int			Version()								{return scrpFuncName->version;}
//		int			getLanguageType()						{return scrpFuncName->languageType;}
		OBJECTID		theObjectID()							{return theObject;}
		void			setVersion(int theVersion)			{scrpFuncName->version = theVersion;}
//		void			setLanguageType(int theLanType)	{scrpFuncName->languageType = theLanType;}
		void			openScrpFile(HWND, char*);
		void			CallScrpFunction();

		void		UpdateData(HWND);
		void		ExportObject();
		void		ImportObject();
		void		CheckIDs(OBJECTID);
		void		Validate(pAObjMessage, OBJECTID);
};


/**************************************************************************
 *	OPENDLG class definition for the browser dialog.
 *************************************************************************/

class OPENDLG {
	private:
		static char*	filter;
		static char*	title;	  
		char				filename[256];
		char				file_title[13];
		OPENFILENAME	ofn;

	public:
		OPENDLG(HWND = NULL);
		int		GetFileOffset()				{return ofn.nFileOffset;}
		void	SetFlags(DWORD Flags)	{ofn.Flags = Flags;}
		char*	GetName()							{return filename;}
		void	SetInitDir(char* p)		{ofn.lpstrInitialDir = p;}
		BOOL	FileDlg(char* = NULL);
};


/**************************************************************************
 *	Exported Function Prototypes.
 *************************************************************************/

extern "C" {
	BOOL FAR PASCAL EditObjectDialogProc (HWND hDlg, WORD message, WORD wParam, LONG lParam);
	BOOL FAR PASCAL EditArgumentDialogProc (HWND hDlg, WORD message, WORD wParam, LONG lParam);
	BOOL FAR PASCAL EditReturnValDialogProc (HWND hDlg, WORD message, WORD wParam, LONG lParam);
	BOOL FAR PASCAL errorLogDialogProc (HWND hDlg, WORD message,WORD wParam, LONG lParam);
};


/**************************************************************************
 *	Special typedefs for functions returning values, the typedefs will
 *	let the compiler pop the return values of the stack for us.
 *************************************************************************/

typedef float				(FAR PASCAL* P_FLOAT)		(void);
typedef double 			(FAR PASCAL* P_DOUBLE)		(void);
typedef long double		(FAR PASCAL* P_LONGDOUBLE)	(void);
typedef short 				(FAR PASCAL* P_SHORT)		(void);
typedef unsigned short	(FAR PASCAL* P_USHORT)		(void);
typedef long				(FAR PASCAL* P_LONG)			(void);
typedef unsigned long 	(FAR PASCAL* P_ULONG)		(void);
typedef char*				(FAR PASCAL* P_TEXTPTR)		(void);

typedef float  			(FAR CDECL* C_FLOAT)			(void);
typedef double 			(FAR CDECL* C_DOUBLE)		(void);
typedef long double		(FAR CDECL* C_LONGDOUBLE)	(void);
typedef short 				(FAR CDECL* C_SHORT)			(void);
typedef unsigned short	(FAR CDECL* C_USHORT)		(void);
typedef long				(FAR CDECL* C_LONG)			(void);
typedef unsigned long 	(FAR CDECL* C_ULONG)			(void);
typedef char*				(FAR CDECL* C_TEXTPTR)		(void);


#endif			// #ifndef __SCRP_H


