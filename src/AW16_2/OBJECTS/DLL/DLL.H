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
//	"dll_adm.cpp"
//
// Author:	Edgar C. Jerez
//	 Start:		july 28, 1993
//	 Finish:	1994
//
// Description:
//	General header for the DLL object
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DLL_H			// prevent multiple includes.
#define __DLL_H

#ifndef A_ALM_H			// neccesary for data handling.
#include	<a_alm.h>
#include	<a_almutl.h>
#endif

#ifndef __COMMDLG_H			// neccesary for common dialog routines.
#include	<commdlg.h>
#endif


/******************************Resource Constants*************************/

/**************************************************************************
 *	Constants for the DLL resource dialog.
 *************************************************************************/

#define IDD_DLLNAME				101
#define IDD_DLLFUNCNAME		102
#define IDD_ARGUMENTS			103
#define IDD_RETURNVAL			104
#define IDD_BROWSER				105
#define	IDD_DLLCTYPE			110		// radio button, C type function.
#define	IDD_DLLPASCALTYPE	111		// radio button, Pascal type function.

#define dllTypeID      		MAKEID('d','l','l','s')
#define dllEvent 					MAKEID('d','L','l','O')


/**************************************************************************
 *	Constants for the Function Argument dialog.
 *************************************************************************/

#define IDD_ARGUMENTLST		101
#define IDD_INSERT				102
#define IDD_UPDATE				103
#define IDD_DELETE				104
#define IDD_PARAMTYPE			105
#define IDD_OBJECTTYPE		106
#define IDD_OBJECTNAME		107
#define IDD_OBJECTCONST		108
#define IDD_OBJECTVALUE		109
#define	IDD_OBJECTBYREF		112	// radio button, pass argument by reference.
#define	IDD_OBJECTBYVALUE	113	// radio button, pass argument by value.


/**************************************************************************
 *	Constants for the Return Value Dialog Box.
 *************************************************************************/

#define IDD_RETPARAMTYPE	101
#define IDD_RETOBJTYPE		102
#define IDD_RETOBJNAME		103


/**************************************************************************
 *	Data block offsets for the DLL object.
 *************************************************************************/

#define	DATA_DLLDATA			0L
#define NUM_DLL_DATABLKS	2			// minimum data blocks for ALM.
#define DLLNAMEINDEX 			0L		// data block index for DLL name.
#define DLLFUNCNAMEINDEX	0L		// data block index for function name.
#define	DLLRETPARAMINDEX	1L		// Index to data block for return value param.
#define	DLLPARAMINDEX			2L		// Index to start data block for arguments.
#define MAXNUMOFARGS			100		// Max arguments that can be passed to a function.

#define DLLNAMESIZE		 		64
#define FUNCNAMESIZE	 		64
#define CONSTVALLEN				10			// LENGHT OF CONSTANT VALUE, in bytes (long real).


/**************************************************************************
 *	C type supported in the DLL object.
 *************************************************************************/

#define		CTYPE_SHORT				0
#define		CTYPE_USHORT			1
#define		CTYPE_LONG				2
#define		CTYPE_ULONG				3
#define		CTYPE_FLOAT				4
#define		CTYPE_DOUBLE			5
#define		CTYPE_LDOUBLE			6
#define		CTYPE_CHRFARPTR		7
#define		NUMOFTYPES				8


/**************************************************************************
 *	Language types supported in the DLL object.
 *************************************************************************/

#define		C_FUNCTION					0		// C type arguments.
#define		PASCAL_FUNCTION			1		// Pascal type arguments.


/**************************************************************************
 *	standard data type, these are used in the readin of the .dll file.
 *************************************************************************/

typedef	char						CHAR;
typedef	int							INT;
typedef	unsigned short	USHORT;
typedef	unsigned long		ULONG;
typedef int							HFILE;
typedef	CHAR FAR *			LPSZ;
typedef	long *					PTLONG;


#define VERSION_OBJECT		0



/**************************************************************************
 *	DLL_FUNCNAME: contains the DLL and function name and an array
 *	containing the data block index for each argument of the function call.
 *************************************************************************/

struct DLL_FUNCNAME {
	int		version;					// DLL version number.
	char	DLLName[DLLNAMESIZE];			// name of the dll and path.
	char	DLLFunction[DLLNAMESIZE];	// name of the selected function.
	int		languageType;			// 0 - C, 1 - Pascal.
};


/**************************************************************************
 *	DLL_RETARG structure.  Return Argument in a dll function call
 *	will be stored in dll object data block 1. 
 *************************************************************************/

struct DLL_RETARG {
	OBJECTID	theObject;		// object id associated with the c param type.
	int				paramType;		// index to c param type.
};


/**************************************************************************
 *	DLL_ARGUMENTS structure.  Arguments in a dll function call
 *	are stored in dll object data blocks. Each data block starting with
 *	data2 contains a structure of DLL_ARGUMENTS type.  The number of
 *	function arguments can be gotten by CountObjectData-2.
 *************************************************************************/

struct DLL_ARGUMENTS {
	BOOL				constant;					// TRUE -- argument is constant.
	OBJECTID		theArgObject;			// object id associated with the c param type.
	int					paramType;				// index to c param type.
	long double	constVal;					// value for constant.
	BOOL				byValue;					// TRUE - by value; FALSE - by ref.
};


/**************************************************************************
 *	SER_DLL class definition.
 *************************************************************************/

class DLL_OBJECT
{
	private:
		OBJECTID		theObject;

	public:
		DLL_FUNCNAME* DllFuncName;		// dll and function name, in data0.
		DLL_OBJECT(OBJECTID);
		~DLL_OBJECT();

			// inline methods
		int				Version()												{return DllFuncName->version;}
		int				getLanguageType()								{return DllFuncName->languageType;}
		OBJECTID	theObjectID()										{return theObject;}
		void			setVersion(int theVersion)			{DllFuncName->version = theVersion;}
		void			setLanguageType(int theLanType)	{DllFuncName->languageType = theLanType;}
		void			openDLLFile(HWND, char*);
		void			CallDLLFunction();

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
		char					filename[256];
		char					file_title[13];
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
};


/**************************************************************************
 *	Special typedefs for functions returning values, the typedefs will
 *	let the compiler pop the return values of the stack for us.
 *************************************************************************/

typedef float  					(FAR PASCAL*     P_FLOAT)			(void);
typedef double 					(FAR PASCAL*     P_DOUBLE)		(void);
typedef long double			(FAR PASCAL*     P_LONGDOUBLE)(void);
typedef short 					(FAR PASCAL*     P_SHORT)			(void);
typedef unsigned short	(FAR PASCAL*     P_USHORT)		(void);
typedef long					  (FAR PASCAL*     P_LONG)			(void);
typedef unsigned long 	(FAR PASCAL*     P_ULONG)			(void);
typedef char*					 	(FAR PASCAL*     P_TEXTPTR)		(void);

typedef float  					(FAR CDECL*      C_FLOAT)			(void);
typedef double 					(FAR CDECL*      C_DOUBLE)		(void);
typedef long double			(FAR CDECL*      C_LONGDOUBLE)(void);
typedef short 					(FAR CDECL*      C_SHORT)			(void);
typedef unsigned short	(FAR CDECL*      C_USHORT)		(void);
typedef long					  (FAR CDECL*      C_LONG)			(void);
typedef unsigned long 	(FAR CDECL*      C_ULONG)			(void);
typedef char*					 	(FAR CDECL*      C_TEXTPTR)		(void);

#endif			// #ifndef __DLL_H
