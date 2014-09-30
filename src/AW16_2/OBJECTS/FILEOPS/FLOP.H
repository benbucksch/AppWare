/////////////////////////////////////////////////////////////////////////////
// File Name:  FlOp.H
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:
//
// Description.:  Header file for the File Operation functions.
/////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#define IDS_FILTERSTRING 100
#define IDL_DIRECTORY 100
#define IDL_CHOSEN 101
#define IDD_ADD 102
#define IDD_REMOVE 103
#define IDS_PATH 104
#define IDCB_DRIVES	105

extern HINSTANCE hInstance ;
int sort_function(const void *a, const void *b) ;

extern "C" {
void FAR PASCAL GetFileNamesFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL GetFilesFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL MoveFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL RenameFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL ChooseFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL CopyFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL DeleteFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL DirectoryFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL DiskLabelFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL CreateDirectoryFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL PathNameFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL PutFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL GetCreationInfoFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL GetModificationInfoFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL GetSizeFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void FAR PASCAL GetTypesFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void far pascal _export Flop_ReadIni(pAEvtInfo theEvent, pAObjMessage theSystem);
void far pascal _export Flop_WriteIni(pAEvtInfo theEvent, pAObjMessage theSystem);
}

typedef struct
	{
	char name[13] ;
	} fileName ;

