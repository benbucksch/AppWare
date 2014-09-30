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
#define IDL_SELECTED	104
#define IDD_ADD	106
#define IDD_REMOVE	107
#define IDD_EDIT	108
#define IDL_AVAILABLE	105
#define IDE_DEFEXT	117
#define IDC_GROUPBOX1	112
#define STRICT
#define ASTRICT

#include <bwcc.h>
#define OTYPE_FILE AMAKEID('F','i','l','e')

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


#define IDD_TEXT		109
#define IDD_NUMBER		110
#define IDD_PICTURE		111
#define IDD_SOUND		112
#define IDD_OTHER		113



typedef struct _FileInfo {
	int       version ;        // Current version = 0
	char 	  lpstrFile[256] ; // fully qualified filename
	int	   	  nFilterIndex ;   // specifies the filter initially used
	char 	  lpstrDefExt[4] ; // default file extention used when saving files
	HANDLE	  hFile ;          // file handle of the database file
	int       iFileAttr ;       // not currently used
	int		  iStatus ;        // not currently used
	} FileInfo ;

extern int	cbOffset ;
extern HINSTANCE hInstance ;
extern char lpstrFilter[256] ;

extern "C" {
LONG CALLBACK AdministrationRoutine(AOBJECTID oiFile, pAObjMessage  theSystem) ;

void CALLBACK FileNameFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK NewFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK SaveFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK SaveAsFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK LoadFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK OpenFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;

void CALLBACK LoadFileMeth(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK SaveFileMeth(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
BOOL CALLBACK EditObjectDialogProc (HWND, UINT, WPARAM, LPARAM) ;
}
LONG ValidateObjectIDs( AOBJECTID oiFile, pAObjMessage theSystem ) ;
LONG EvaluateObject( AOBJECTID oiFile, pAObjMessage theSystem ) ;
