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
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ GetFileNamesFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ GetFilesFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ MoveFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ RenameFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ChooseFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ CopyFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ DeleteFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ DirectoryFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ DiskLabelFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ CreateDirectoryFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ PathNameFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ PutFileFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ GetCreationInfoFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ GetModificationInfoFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ GetSizeFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ GetTypesFunc(pAEvtInfo  theEvent, pAObjMessage  theSystem) ;
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flop_ReadIni(pAEvtInfo theEvent, pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flop_WriteIni(pAEvtInfo theEvent, pAObjMessage theSystem);
}

typedef struct
	{
	char name[13] ;
	} fileName ;

