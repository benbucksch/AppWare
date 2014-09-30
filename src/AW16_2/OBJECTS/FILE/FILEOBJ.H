/////////////////////////////////////////////////////////////////////////////
// File Name:  FileObj.H
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:
//
// Description.:  Header file for the File object.
// Object Datas
//	Data 0: the FileInfo structure
//	Data 1: the array of object ID participating in import and/or export
/////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <bwcc.h>
#define OTYPE_FILE MAKEID('F','i','l','e')

#define INFO_INDEX 0
#define OBJECT_INDEX 1

#define File	200
#define MAXNAMESIZE 32
#define OBJECTNAMESIZE MAXNAMESIZE

#define IDS_FILTERSTRING 100

// File Selection Dialog Box Constants
//#define IDHELP			998
#define IDD_FNAME   	0x10
#define IDD_FPATH   	0x11
#define IDD_FLIST   	0x12
#define MAXFNAMELENGTH 96

// Object Definition Dialog Box Constants
#define IDL_SELECTED 	104
#define IDL_AVAILABLE	105

#define IDD_ADD			106
#define IDD_REMOVE		107
#define IDD_EDIT		108

#define IDD_TEXT		109
#define IDD_NUMBER		110
#define IDD_PICTURE		111
#define IDD_SOUND		112
#define IDD_OTHER		113

#define IDE_DEFEXT		117

#define IDD_OBJECT		200

typedef struct _FileInfo {
	int			version ; // Current version = 0
	char far	lpstrFile[256] ; // fully qualified filename
	int			nFilterIndex ; // specifies the filter initially used
	char far	lpstrDefExt[4] ; // default file extention used when saving files
	int			hFile ; // file handle of the database file
	WORD      	wFileAttr ; // not currently used
	WORD	  	wStatus ; // not currently used
	} FileInfo ;

extern int	cbOffset ;
extern HANDLE hInstance ;
extern char lpstrFilter[256] ;

extern "C" {
LONG FAR PASCAL AdministrationRoutine(OBJECTID oiFile, pAObjMessage  theSystem) ;

void FAR PASCAL FileNameFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL NewFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL SaveFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL SaveAsFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL LoadFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL OpenFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;

void FAR PASCAL LoadFileMeth(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL SaveFileMeth(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;

BOOL FAR PASCAL EditObjectDialogProc (HWND hDlg, WORD message, WORD wParam, LONG lParam) ;
}

LONG ValidateObjectIDs( OBJECTID oiFile, pAObjMessage theSystem ) ;
LONG EvaluateObject( OBJECTID oiFile, pAObjMessage theSystem ) ;
