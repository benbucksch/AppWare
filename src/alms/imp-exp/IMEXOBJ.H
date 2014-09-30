/////////////////////////////////////////////////////////////////////////////
// File Name:  ImExObj.H
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:
//
// Description.:  Header file for the Import/Export object.
// Object Datas
//	Data 0: the ImpExpInfo structure
//	Data 1: the array of object ID participating in import and/or export
//  Data 2: an array of zero terminated strings containing the field names
//			corresponding to the object IDs in Data 1:
//	Data 3: an array of object IDs which will
//			participate in the IMPORT and how those objects will be ordered
//	Data 4: an array of object IDs which will
//			participate in the EXPORT and how those objects will be ordered
/////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#define OTYPE_IMPORTEXPORT   AMAKEID('I','m','e','x')
#define NUMDATAS 5
#define INFO_INDEX 0
#define OBJECT_INDEX 1
#define NAMES_INDEX 2
#define IMP_OBJ_INDEX 3
#define EXP_OBJ_INDEX 4

#define BUFFERSIZE (5*1024)
#define MAXFIELDSIZE 256
#define OBJECTNAMESIZE 32
#define FIELDNAMESIZE 32
#define LEADSIZE 10

#define TF_MASK				0xFFF01FFF  // used to clear the text style flags

#define O_EXPORTING -1
#define O_UNCOMMITTED 0
#define O_IMPORTING 1
/*typedef enum _Direction {
	exporting = -1,  // object committed to exporting text records
	uncommitted,  // the direction will be "uncommitted" until "Export Record" or "Import Record" functions are called
	importing,  // object committed to importing text records
	} Direction ;
*/
typedef struct _RuntimeInfo {
	HWND		StatusDlg ; // Window handle of the Status Dialog box
	BOOL		Abort ; // Set to TRUE if the user presses the CANCEL button
	LPSTR 		lpBuffer ;
	WORD		wMark ;
	int			wBytes ; // used to be WORD
	long		lBlockBytes ;
	BOOL		EndOfFile ;
	HANDLE		hFile ;
	long		lRecords ;
	long		finalStatus ; // used in Export Status Dialog box
	long        currentStatus ; // used in the Export Status Dialog box
	} RuntimeInfo ;

typedef struct _ConfigInfo {
	char 		Delimiter ; // comma or tab
	BOOL		Heading ; // Will the first line of the text file contain field names?
	BOOL		ShowStatus ; // Will a status box be displayed?
	} ConfigInfo ;

// Data 0 of the Import-Export object
typedef struct _ImpExpInfo {
	int			version ; // Current version = 0
	char 	lpstrFile[256] ; // fully qualified filename
	int			nFilterIndex ; // specifies the filter initially used
	char 	lpstrDefExt[5] ; // default file extention used when saving files
	BOOL		Busy ; // Is the object between "Start" and "Stop" functions?
	int			Status ; // exporting, uncommitted, or importing
	long		FileSize ; // used in determining transfer status
	ConfigInfo	ImportConfig ; // Import specific information
	ConfigInfo 	ExportConfig ; // Export specific information
	RuntimeInfo 	*lpRuntimeInfo ; // Runtime specific information
	} ImpExpInfo ;

extern HINSTANCE hInstance ;
extern char lpstrFilter[256] ;

extern "C" {
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/ AdministrationRoutine(AOBJECTID oiImportExport, pAObjMessage  theSystem) ;

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ChooseFileFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ConfigureExportFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ConfigureImportFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ CreateExportFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ExportRecordFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ImportRecordFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ StartImportExportFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ StopImportExportFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ EditObjectDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ ConfigImportDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ ConfigExportDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ StatusDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ EditAddDlgBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ FileNameDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
}
long GetFieldIndex(AOBJECTID oiImportExport, AOBJECTID oiField) ;
void RemoveInvalidObjects( AOBJECTID oiImportExport ) ;
LONG doEditObject( AOBJECTID oiImportExport, pAObjMessage theSystem ) ;
LONG doValidateObjectIDs( AOBJECTID oiImportExport, pAObjMessage theSystem ) ;
LONG doEvaluateObject( AOBJECTID oiImportExport, pAObjMessage theSystem ) ;

