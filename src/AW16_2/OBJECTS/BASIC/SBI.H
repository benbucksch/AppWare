/* $Header:   V:/WIN/DEV/SBI/SBI.H_V   1.27   18 Aug 1994 15:14:24   sbrennan  $ */
//
//	File:		sbi.h
//	Author:	Stephen C. Brennan
//	Date:		22-Dec-93
//
//	Description:	this file contains the SBI interface
//
//
#ifndef __SBI_H__
#define __SBI_H__
#ifndef SBL_H
#include "sbl.h"
#endif

	//
	// SBI error returns

#define SBI_E_BASE		-3000
#define SBI_E_INC			-1
#define SBI_E_ERROR(x)	(SBI_E_BASE+(x)*SBI_E_INC)

#define SBI_NOERROR		0						// no error
	// returned from SBITerm:
	//		SBI_E_CANCEL	- user canceled terminate request
	//		SBI_E_RETRY		- SBI busy and attempting to shut down - try
	//							  SBITerm again later (see sbi/g/generic.c)
#define SBI_E_CANCEL		SBI_E_ERROR(1)
#define SBI_E_RETRY		SBI_E_ERROR(2)
#define SBI_E_INITSIZE	SBI_E_ERROR(3)	// init structure is the wrong size
#define SBI_E_HANDLE		SBI_E_ERROR(4)	// invalid SBI handle
#define SBI_E_PARENT		SBI_E_ERROR(5)	// bad parent window
#define SBI_E_COMMAND	SBI_E_ERROR(6)	// invalid command
#define SBI_E_BUSY		SBI_E_ERROR(7)	// SBI is running a program
#define SBI_E_SIGNATURE	SBI_E_ERROR(8)	// bad signature match for SBICall or SBIRun
#define SBI_E_NOCOMPILE	SBI_E_ERROR(9)	// could not compile requested module

#define SBI_E_MIN			SBI_E_NOCOMPILE
#define SBI_E_MAX			SBI_E_CANCEL
#ifdef __cplusplus
extern "C" {
#endif
	//
	// typedefs
	//

//----------------------------------------------------------------
//
// initialization data structure - SBI_INIT
//
typedef SBL_ULONG HSBI;
typedef HSBI SBL_FAR *LPHSBI;

typedef SBL_ULONG SBI_TRIGGER;
typedef SBI_TRIGGER SBL_FAR *SBI_LPTRIGGER;

typedef SBL_PVOID (DLLENTRYPTR SBI_LPGETOBJ)(HSBI hSbi, char *szModule);
typedef enum {
	SBI_NOTIFY_CLOSE,
} SBI_NOTIFY;

typedef SBL_STATUS (DLLENTRYPTR SBI_LPNOTIFY)(HSBI hSbi, SBI_NOTIFY Reason, SBL_PVOID pData);

#define SBI_MAX_DIR 100
typedef struct {
	char 			szDir[SBI_MAX_DIR];
	char			szWndName[50];
		//
		// window position
		//
	RECT			SBIRect;
		//
		// parent window handle
		//
	HWND			hWndParent;
	SBL_SHORT	sMSTimeSlice;
	SBL_WORD		wfRunTime;
#define SBI_RTF_RTC				0x01
#define SBI_RTF_DIR				0x02
#define SBI_RTF_MODAL			0x04
#define SBI_RTF_UNLOADHMD		0x08
	SBL_BYTE		cFrameType;
#define SBI_FT_POPUP				0x00
#define SBI_FT_CONTROL			0x01
	SBL_BYTE		cfFrameOpts;
#define SBI_FO_ICONS				0x01
#define SBI_FO_MENU				0x02
		//
		// callbacks
		//
	SBI_LPGETOBJ	GetObj;
		//
		//	count of thread and instance longs
		//
	SBL_WORD			cThreadLongs;
	SBL_WORD			cInstanceLongs;
		//
		// binary and include paths
		//
	SBL_LPSTR		pszBinaryPath;
	SBL_LPSTR		pszIncludePath;
		//
		// notification callback
		//
	SBI_LPNOTIFY	pNotifyProc;
} SBI_INIT;
typedef SBI_INIT SBL_FAR *SBI_LPINIT;
	//
	// interface
	//

	//
	// return default configuration
	//
SBL_STATUS	DLLENTRY SBIDefaults(SBI_LPINIT pInit, SBL_WORD InitSize);
	//
	// diagnostic routine for trying different SBI configuration options
	//
HSBI DLLENTRY SBIOptions(HSBI hCurSBI, SBI_LPINIT pInit, SBL_WORD InitSize);
	//
	//	display SBL splash screen
	//
SBL_STATUS DLLENTRY SBISplash(HSBI hSBI);
	//
	// create an instance of the SBI interface -- eventually, pInit
	// will point to a configuration structure.  For now, pass NULL.
	//	Returns NULL if error
	//
SBL_STATUS	DLLENTRY SBIInit(SBI_LPINIT pInit, SBL_WORD InitSize, LPHSBI phSBI);
	//
	// destroy an instance of the SBI interface.  Returns non-zero if error
	//
SBL_STATUS		DLLENTRY SBITerm(HSBI hSBI);
	//
	// resize control window
	//
SBL_STATUS		DLLENTRY SBIResize(HSBI hSBI, LPRECT pRect);
	//
	//	change parent of SBI control
	//
SBL_STATUS DLLENTRY SBIParent(HSBI hSBI, HWND hWnd);
	//
	//	set focus to the editor
	//
SBL_STATUS DLLENTRY SBIFocus(HSBI hSBI);
	//
	//	return handle to SBL associated with an instance of SBI
	//
HSBL				DLLENTRY SBIhSbl(HSBI hSBI);
	//
	//	This function must be called between in the embedding application's
	// message loop:
	//
   //	while (GetMessage(&msg, NULL, NULL, NULL)) {
	//	 	if(!SBIPreTrans(&msg)) {
   //	 		TranslateMessage(&msg);    /* Translates virtual key codes       */
   //	 		DispatchMessage(&msg);     /* Dispatches message to window       */
	//		}
   //	 }
	//
	//	it returns TRUE if SBI consumed the message, FALSE if the application
	// should continue processing the message
	//
SBL_BOOL			DLLENTRY SBIPreTrans(MSG SBL_FAR * pMsg);
	//
	// run a macro
	//
	//		hSBI			handle to SBI instance
	//		wFlags		flags for this run
	//							SBI_F_DEBUG		stop at first line of called procedure
	//							SBI_F_BP			enable breakpoints
	//							SBI_F_CMD		first input parameter is pointer to
	//												string to set into SBL command line
	//						
	//		pFileName	name of file containing macro to run (currently must
	//						be a source file)
	//		pSubName		name of procedure to run.  If NULL is passed, run
	//						procedure called "main".
	//		pSig			pointer to array of SBL_DATATYPE's which describe the
	//						signature of the function to call - NULL for any
	//						signature
	//		pTrig			pointer to a trigger.  Triggers are used by SBI to
	//						cache invocation information.
	//		ppArgs		pointer to arguments to be passed to macros as per
	//						arguments to SblCallProc
	//
	//
#define SBI_F_DEBUG		0x0001
#define SBI_F_BP			0x0002
#define SBI_F_CMD			0x0004
#define SBI_F_LOADONLY	0x0080
SBL_STATUS		DLLENTRY SBIRun(
								HSBI	hSBI,					// instance handle
								SBL_WORD wFlags,			// flags word
								SBL_LPSTR pFileName,		// pointer to macro file
								SBL_LPSTR pSubName,		// pointer to sub (NULL for main)
								SBL_LPDATATYPE pSig,		// datatypes array for signature
								SBI_LPTRIGGER pTrig,		// pointer to cache handle
								SBL_PVOID SBL_FAR *ppArgs);
SBL_STATUS		DLLENTRY SBIRunObject(
								HSBI	hSBI,					// instance handle
								SBL_WORD wFlags,			// flags word
								SBL_LPSTR pFileName,		// pointer to macro file
								SBL_LPSTR pSubName,		// pointer to sub (NULL for main)
								SBL_LPDATATYPE pSig,		// datatypes array for signature
								SBI_LPTRIGGER pTrig,		// pointer to cache handle
								SBL_PVOID SBL_FAR *ppArgs,
								SBL_PVOID me);
	//
	//	edit a macro
	//		hSBI			handle to SBI instance
	//		pFileName	name of file to edit
	//
SBL_STATUS		DLLENTRY SBIEdit(HSBI hSBI, SBL_LPSTR pFileName);
	//
	//	edit a macro with associated class
	//		hSBI			handle to SBI instance
	//		pFileName	name of file to edit
	//		Class			datatype of class to associated with this module
	//
SBL_STATUS		DLLENTRY SBIEditClass(HSBI hSBI, SBL_LPSTR pFileName,
													SBL_DATATYPE Class);
	//
	//	fire up the SBI dialog box to edit, run or create a new macro
	//
SBL_STATUS		DLLENTRY SBIMacros(HSBI hSBI);
	//
	// run a sbl macro
	//
	//		hSBI			handle to SBI instance
	//		wFlags		flags for this run
	//							SBI_F_DEBUG		stop at first line of called procedure
	//							SBI_F_BP			enable breakpoints
	//							SBI_F_CMD		first input parameter is pointer to
	//												string to set into SBL command line
	//						
	//		pFileName	name of file containing macro to run (currently must
	//						be a source file)
	//		pSubName		name of procedure to run.  If NULL is passed, run
	//						procedure called "main".
	//		pSig			pointer to array of SBL_DATATYPE's which describe the
	//						signature of the function to call - NULL for any
	//						signature
	//		pTrig			pointer to a trigger.  Triggers are used by SBI to
	//						cache invocation information.
	//		pRet			pointer to area in which to place return value
	//		...			additional arguments to be passed to BASIC routine
	//	
SBL_STATUS		SBL_CDECL SBL_FAR SBICall(
						HSBI hSbi,					// instance handle
						SBL_WORD wFlags,			// flags word
						SBL_LPSTR pFileName,		// pointer to macro file
						SBL_LPSTR pSubName,		// pointer to sub (NULL for main)
						SBL_LPDATATYPE pSig,		// datatypes array for signature
						SBI_LPTRIGGER pTrig,		// pointer to cache handle
						SBL_PVOID pRet,			// pointer to return value
						...);	 						// additional arguments
SBL_STATUS		SBL_CDECL SBL_FAR SBICallObject(
						HSBI hSbi,					// instance handle
						SBL_WORD wFlags,			// flags word
						SBL_LPSTR pFileName,		// pointer to macro file
						SBL_LPSTR pSubName,		// pointer to sub (NULL for main)
						SBL_LPDATATYPE pSig,		// datatypes array for signature
						SBI_LPTRIGGER pTrig,		// pointer to cache handle
						SBL_PVOID pRet,			// pointer to return value
						SBL_PVOID me,				// "me" object for call proc object
						...);	 						// additional arguments
	//*******************
	// command interface
	//*******************

	//
	//	command ID's
	//
typedef enum {
	SBI_CMD_FILENEW = 0,
	SBI_CMD_FILEOPEN,
	SBI_CMD_FILESAVE,
	SBI_CMD_FILESAVEAS,
	SBI_CMD_FILEPRINT,
	SBI_CMD_EDITUNDO,
	SBI_CMD_EDITCUT,
	SBI_CMD_EDITCOPY,
	SBI_CMD_EDITPASTE,
	SBI_CMD_EDITCLEAR,
	SBI_CMD_EDITSELECT,
	SBI_CMD_EDITSETFONT,
	SBI_CMD_SEARCHFIND,
	SBI_CMD_SEARCHNEXT,
	SBI_CMD_SEARCHPREV,
	SBI_CMD_COMPILE,
	SBI_CMD_EXECUTE,
	SBI_CMD_CONSOLE,
	SBI_CMD_DUMPCOMP,
	SBI_CMD_STEPINTO,
	SBI_CMD_STEPOVER,
	SBI_CMD_IDLE,
	SBI_CMD_SUSPEND,
	SBI_CMD_STEPUP,
	SBI_CMD_LOGFILE,
	SBI_CMD_VARS,
	SBI_CMD_NEXTERROR,
	SBI_CMD_PREVERROR,
	SBI_CMD_ANIMATE,
	SBI_CMD_FASTER,
	SBI_CMD_SLOWER,
	SBI_CMD_BREAKPOINT,
	SBI_CMD_EXECUNTIL,
	SBI_CMD_HELPSBL,
	SBI_CMD_HELPABOUT,
	SBI_CMD_HELPCONTENTS,
	SBI_CMD_HELPCONTEXT,
	SBI_CMD_SEARCHREPLACE,
	SBI_CMD_EDITDIALOG,
	SBI_CMD_CLEARBPS
} SBI_CMD;
	//
	// SBI state
	//
typedef enum {
	SBI_STATE_ERROR,			// some error in sbi state (invalid handle)
	SBI_STATE_EDITING,		// editing macro (idle)
	SBI_STATE_RUNNING,		// running a macro
	SBI_STATE_DEBUGGING		// debugging a macro
} SBI_STATE;

	//
	// execute a command -- return TRUE if command executed, FALSE
	//	if command is disabled
	//
SBL_STATUS DLLENTRY SBICmdExec(HSBI hSBI, SBI_CMD Cmd);
	//
	// return state of a command as a set of bits
	//		MF_ENABLED, MF_GRAYED, MF_DISABLED, MF_CHECKED, MF_UNCHECKED
	//
UINT DLLENTRY SBICmdQuery(HSBI hSBI, SBI_CMD Cmd);
	//
	//	return current state of SBI
	//
SBI_STATE DLLENTRY SBIState(HSBI hSBI);
	//
	// return error text for error
	//
SBL_STATUS DLLENTRY SBIGetErrorText(
		SBL_STATUS errNumber,				// error number
		SBL_LPSTR pszErrBuf,					// buffer to populate with error text
		SBL_WORD cbErrBuf );					// size of buffer
	//
	//	manipulate the edit control
	//
typedef enum {
	SBIEC_GETSEL,			// wP, lP ignored, LOWORD(result) is offset of first character
									// in the selection, HIWORD(result) is offset of last character
									// in the selection
	SBIEC_SETSEL,			// wP ignored, LOWORD(lP) is offset of first character in the
									// selection, HIWORD(lP) is offset of last character in the selection
	SBIEC_GETLINE,			// wP contains line to retrieve, lP points to buffer in which to
									// return the line
	SBIEC_GETLINECOUNT,	// wP, lP ignored.  Result contains count of lines
	SBIEC_LINEINDEX,		// lP ignored.  wP line to use, -1 for current line.  Result contains the character
									// offset of the line
	SBIEC_LINELENGTH,		// lP ignored.  wP character index of line whose length to query.  Result contains
									// length of specified line
	SBIEC_REPLACESEL,		// wP ignored.  lP points to NULL terminated string used to replace the 
									// current selection
	SBIEC_LINEFROMCHAR,	// lP character index.  Returns number of line that contains character at
									// character index
	SBIEC_GOTOPROC,		// Find start of procedure/sub of a given name.  lP
									// points to null terminated string containing
									// the name of the procedure.  Returns TRUE if
									// found, FALSE if not.
	SBIEC_APPENDCONSOLE,	//	Append string to console window.  lP points to
									// NULL terminated string
} SBI_EDIT_COMMAND;

SBL_ULONG DLLENTRY SBIEditControl(HSBI hSBI,
											 SBI_EDIT_COMMAND Cmd,
											 SBL_WORD wP,
											 SBL_LONG lP);

	//
	// return the text of the last message displayed at the bottom
	// of the SBI window
	//
SBL_STATUS DLLENTRY SBIGetLastMessage(HSBI hSBI,
												  SBL_LPSTR szBuf,
												  SBL_WORD cbBuf);
#ifdef __cplusplus
}
#endif
#endif

