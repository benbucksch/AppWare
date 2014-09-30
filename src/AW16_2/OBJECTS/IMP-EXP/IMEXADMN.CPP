/////////////////////////////////////////////////////////////////////////////
// File Name:  ImExAdmn.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:
//
// Description.: Event Administration Routine for the Serius Import/Export object:
////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <mem.h>
#include <dos.h>
#include <a_alm.h>
#include "imexobj.h"
#include "imexdlg.h"

#define CURRENTDATAVERSION 0

extern "C" long FAR PASCAL _export GetSerVersion(TYPEID) ;
LONG VersionCheck(OBJECTID oiImportExport, pAObjMessage theSystem) ;
LONG CreateObject(OBJECTID oiImportExport) ;
LONG AssignObject(OBJECTID oiImportExport, pAObjMessage theSystem) ;
LONG ImportObject(OBJECTID oiImportExport) ;
LONG ExportObject(OBJECTID oiImportExport) ;

HANDLE hInstance ;
char lpstrFilter[256] ;
extern int _WinAllocFlag;

long FAR PASCAL _export GetSerVersion(TYPEID)
	{
	return 1L ;
	}

LONG FAR PASCAL _export AdministrationRoutine(OBJECTID oiImportExport, pAObjMessage theSystem)
	{
	switch (theSystem->message1)
		{
		case AOBJ_CREATED:
			return CreateObject(oiImportExport) ;

		case AOBJ_ASSIGNOBJECT:
			return AssignObject(oiImportExport, theSystem) ;

		case AOBJ_READ :     // Sent when an Info object should load its data
			return VersionCheck(oiImportExport, theSystem) ;

		case AOBJ_IMPORTOBJECT :
			return ImportObject(oiImportExport) ;

		case AOBJ_EXPORTOBJECT :
			return ExportObject(oiImportExport) ;

		case AOBJ_EDITOBJECT:
			return doEditObject(oiImportExport, theSystem) ;

		case AOBJ_VALIDATEOBJECTIDS :
			return doValidateObjectIDs(oiImportExport, theSystem) ;

		case AOBJ_CHECKOBJECT :
			return doEvaluateObject(oiImportExport, theSystem) ;

		case AOBJ_AWAKENED :  // Sent when a project is loaded
//		case AOBJ_PREREAD :   // Sent when an Info object's data is
							  //   read from a project or application file
//		case AOBJ_POSTREAD :  // Sent after all objects' data are loaded.
		case AOBJ_PREWRITE :  // Sent when a project is saved
//		case AOBJ_WRITE :     // Sent when an Info object's data is
							  //   written to a project or application file
		case AOBJ_POSTWRITE : // Sent after a project is saved
		case AOBJ_DESTROYED : // Sent before an Info object is destroyed
		case AOBJ_ASLEEP :	  // Sent when a project is closed
		case AOBJ_QUERYPROTOCOL :
			return A_NOTHANDLED ;
		}
	return A_NOTHANDLED ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  VersionCheck
//  Description:  Checks the version compatiblity of the Import/Export's datas
////////////////////////////////////////////////////////////////////////
LONG VersionCheck(OBJECTID oiImportExport, pAObjMessage theSystem)
	{
	ImpExpInfo far* lpImpExpInfo ;
	LONG objectVersion ;

	lpImpExpInfo = (ImpExpInfo far*)AObjLockData(oiImportExport, 0) ;
	objectVersion = lpImpExpInfo->version ;
	AObjUnlockData(oiImportExport, 0) ;
	theSystem->message3 = CURRENTDATAVERSION ;
	if(objectVersion > CURRENTDATAVERSION)
		{
		AObjReportError(oiImportExport, theSystem->message1, A_ERROR, "Import Export\tSER_IMEX.DLL", 0);
		return A_ERROR ;
		}
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  CreateObject
//  Description:  Creates a Info object with default values
////////////////////////////////////////////////////////////////////////
LONG CreateObject(OBJECTID oiImportExport)
	{
	ImpExpInfo far *lpImpExpInfo ;
	char far* lpFieldNames ;

	if(!AObjSetDataCount(oiImportExport, NUMDATAS))
		{
		ADBG_PRINT_E("AObjSetDataCount failed") ;
		return A_ERROR ;
		}
	AObjResizeData(oiImportExport, 0, sizeof(ImpExpInfo)) ;
	lpImpExpInfo = (ImpExpInfo far *)AObjLockData(oiImportExport, 0) ;
	if(lpImpExpInfo == NULL)
		{
		ADBG_PRINT_E("AObjResizeData failed") ;
		return A_ERROR ;
		}
	lpImpExpInfo->version = 0 ;
	lstrcpy(lpImpExpInfo->lpstrFile,"untitled") ; // default file name
	lpImpExpInfo->nFilterIndex = 1 ;
	lstrcpy(lpImpExpInfo->lpstrDefExt,"dat") ; // default extension
	lpImpExpInfo->Busy = FALSE ;
	lpImpExpInfo->Status = O_UNCOMMITTED ;
	lpImpExpInfo->FileSize = 0 ;
	lpImpExpInfo->ImportConfig.Delimiter = '\t' ;
	lpImpExpInfo->ImportConfig.Heading = TRUE ;
	lpImpExpInfo->ImportConfig.ShowStatus = TRUE ;

	lpImpExpInfo->ExportConfig.Delimiter = '\t' ;
	lpImpExpInfo->ExportConfig.Heading = TRUE ;
	lpImpExpInfo->ExportConfig.ShowStatus = TRUE ;

	lpImpExpInfo->lpRuntimeInfo = NULL ;
	AObjUnlockData(oiImportExport, 0) ;

	AObjResizeData(oiImportExport, NAMES_INDEX, 1) ;
	lpFieldNames = (char far *)AObjLockData(oiImportExport, NAMES_INDEX) ;
	if(lpFieldNames == NULL)
		{
		ADBG_PRINT_E("AObjResizeData failed") ;
		return A_ERROR ;
		}
	lpFieldNames[0] = 0 ;
	AObjUnlockData(oiImportExport, NAMES_INDEX) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  AssignObject
//  Description:  Copies datas from one Info object to another with default values
////////////////////////////////////////////////////////////////////////
LONG AssignObject(OBJECTID oiImportExport, pAObjMessage theSystem)
	{
	OBJECTID oiDestination ;
	LPVOID lpDestination, lpImportExport ;
	int size, i ;

	oiImportExport = (OBJECTID) theSystem->message2 ; // ImportExport object ID
	oiDestination = (OBJECTID) theSystem->message3 ; // destination object ID
	if (AObjGetType(oiImportExport) != AObjGetType(oiDestination))
		return A_ERROR ;

	for (i = 0; i < NUMDATAS; i++)
		{
		size = (int)AObjGetDataSize(oiImportExport, i) ;
		AObjResizeData(oiDestination, i, size) ;
		if (size > 0)
			{
			lpImportExport = AObjLockData(oiImportExport, i) ;
			lpDestination = AObjLockData(oiDestination, i) ;
			if (lpDestination && lpImportExport)  // if neither pointer is NULL
				_fmemcpy(lpDestination, lpImportExport, size) ;
			AObjUnlockData(oiImportExport, i) ;
			AObjUnlockData(oiDestination, i) ;
			}
		}
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  ExportObject
//  Description:  Exports the Info object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ExportObject(OBJECTID oiImportExport)
	{
	ImpExpInfo far *lpImpExpInfo ;
	char far* lpFieldNames ;
	OBJECTID far *lpObjects ;
	int nObjects, i, j, length ;

	RemoveInvalidObjects( oiImportExport ) ;
	lpImpExpInfo = (ImpExpInfo far*)AObjLockData(oiImportExport, INFO_INDEX) ;
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &lpImpExpInfo->version, sizeof(int)) ;
	AUpsfExportField("EXTENSION", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, lpImpExpInfo->lpstrDefExt, lstrlen(lpImpExpInfo->lpstrDefExt)) ;

	AUpsfExportField("IMPDELIMITER", 0L, AUPSF_NATV_CHAR, AUPSF_UNIV_NUMBER,	&lpImpExpInfo->ImportConfig.Delimiter, sizeof(char)) ;
	AUpsfExportField("IMPHEADING", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &lpImpExpInfo->ImportConfig.Heading, sizeof(BOOL)) ;
	AUpsfExportField("IMPSTATUS", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &lpImpExpInfo->ImportConfig.ShowStatus, sizeof(BOOL)) ;

	AUpsfExportField("EXPDELIMITER", 0L, AUPSF_NATV_CHAR, AUPSF_UNIV_NUMBER,	&lpImpExpInfo->ExportConfig.Delimiter, sizeof(char)) ;
	AUpsfExportField("EXPHEADING", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &lpImpExpInfo->ExportConfig.Heading, sizeof(BOOL)) ;
	AUpsfExportField("EXPSTATUS", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &lpImpExpInfo->ExportConfig.ShowStatus, sizeof(BOOL)) ;
	AObjUnlockData(oiImportExport, INFO_INDEX) ;

	lpObjects = (OBJECTID far*)AObjLockData( oiImportExport, OBJECT_INDEX ) ;
	lpFieldNames = (char far*)AObjLockData( oiImportExport, NAMES_INDEX ) ;
	nObjects = (int)AObjGetDataSize (oiImportExport, OBJECT_INDEX) / sizeof(OBJECTID) ;
	for (i = 0, j = 0; i < nObjects; i++)
		{
		AUpsfExportField("OBJECTID", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) ;
		length = lstrlen(&lpFieldNames[j]) ;
		AUpsfExportField("NAMES", i, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, &lpFieldNames[j], length) ;
		j += length + 1 ;
		}
	if (lpFieldNames != NULL)
		AObjUnlockData( oiImportExport, NAMES_INDEX ) ;
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, OBJECT_INDEX ) ;

	lpObjects = (OBJECTID far*)AObjLockData( oiImportExport, IMP_OBJ_INDEX ) ;
	nObjects = (int)AObjGetDataSize (oiImportExport, IMP_OBJ_INDEX) / sizeof(OBJECTID) ;
	for (i = 0, j = 0; i < nObjects; i++)
		{
		AUpsfExportField("IMPOBJECTID", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) ;
		}
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, IMP_OBJ_INDEX ) ;

	lpObjects = (OBJECTID far*)AObjLockData( oiImportExport, EXP_OBJ_INDEX ) ;
	nObjects = (int)AObjGetDataSize (oiImportExport, EXP_OBJ_INDEX) / sizeof(OBJECTID) ;
	for (i = 0, j = 0; i < nObjects; i++)
		{
		AUpsfExportField("EXPOBJECTID", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) ;
		}
	if (lpObjects != NULL)
		AObjUnlockData( oiImportExport, EXP_OBJ_INDEX ) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  ImportObject
//  Description:  Imports the Info object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ImportObject(OBJECTID oiImportExport)
	{
	ImpExpInfo far *lpImpExpInfo ;
	char far* lpFieldNames ;
	char buffer[FIELDNAMESIZE] ;
	OBJECTID far *lpObjects ;
	int nObjects, i, j, length ;

	lpImpExpInfo = (ImpExpInfo far*)AObjLockData(oiImportExport, INFO_INDEX) ;
	if(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &lpImpExpInfo->version, sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to version 0.") ;
	if(AUpsfImportField("EXTENSION", 0L, AUPSF_NATV_STRING, lpImpExpInfo->lpstrDefExt, 4) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to 'OIP' for file extension.") ;

	if(AUpsfImportField("IMPDELIMITER", 0L, AUPSF_NATV_CHAR, &lpImpExpInfo->ImportConfig.Delimiter, sizeof(char)) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to tab delimiter for import.") ;
	if(AUpsfImportField("IMPHEADING", 0L, AUPSF_NATV_BOOL, &lpImpExpInfo->ImportConfig.Heading, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to 'Heading ON' for import.") ;
	if(AUpsfImportField("IMPSTATUS", 0L, AUPSF_NATV_BOOL, &lpImpExpInfo->ImportConfig.ShowStatus, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to 'Status ON' for import.") ;

	if(AUpsfImportField("EXPDELIMITER", 0L, AUPSF_NATV_CHAR, &lpImpExpInfo->ImportConfig.Delimiter, sizeof(char)) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to tab delimiter for export.") ;
	if(AUpsfImportField("EXPHEADING", 0L, AUPSF_NATV_BOOL, &lpImpExpInfo->ImportConfig.Heading, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to 'Heading ON' for export.") ;
	if(AUpsfImportField("EXPSTATUS", 0L, AUPSF_NATV_BOOL, &lpImpExpInfo->ImportConfig.ShowStatus, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Defaulting to 'Status ON' for export.") ;
	AObjUnlockData(oiImportExport, INFO_INDEX) ;

	nObjects = (int)AUpsfFieldCount("OBJECTID") ;
	AObjResizeData(oiImportExport, OBJECT_INDEX, nObjects*sizeof(OBJECTID)) ;
	lpObjects = (OBJECTID far*)AObjLockData(oiImportExport, OBJECT_INDEX) ;
	for (i = 0, j = 0; i < nObjects; i++)
		{
		if(AUpsfImportField("OBJECTID", i, AUPSF_NATV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) != AUPSF_NOERROR)
			{
			AUpsfLogComment("An invalid object ID was transfered to the Import/Export object list.") ;
			lpObjects[i] = 0L ;
			}
		if(AUpsfImportField("NAMES", i, AUPSF_NATV_STRING, buffer, FIELDNAMESIZE) != AUPSF_NOERROR)
			{
			AUpsfLogComment("No field name was transfered with an object in the Import/Export object list.") ;
			lstrcpy(buffer, "No Field Name") ;
			}
		length = lstrlen(buffer) + 1 ;
		AObjResizeData( oiImportExport, NAMES_INDEX, j+length ) ;
		lpFieldNames = (char far*)AObjLockData( oiImportExport, NAMES_INDEX) ;
		lstrcpy(&lpFieldNames[j], buffer) ;
		AObjUnlockData( oiImportExport, NAMES_INDEX) ;
		j += length ;
		}
	if (lpObjects != NULL)
		AObjUnlockData(oiImportExport, OBJECT_INDEX) ;

	nObjects = (int)AUpsfFieldCount("IMPOBJECTID") ;
	AObjResizeData(oiImportExport, IMP_OBJ_INDEX, nObjects*sizeof(OBJECTID)) ;
	lpObjects = (OBJECTID far*)AObjLockData(oiImportExport, IMP_OBJ_INDEX) ;
	for (i = 0; i < nObjects; i++)
		{
		if(AUpsfImportField("IMPOBJECTID", i, AUPSF_NATV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) != AUPSF_NOERROR)
			{
			AUpsfLogComment("An invalid object ID was transfered to the Import object list.") ;
			lpObjects[i] = 0L ;
			}
		}
	if (lpObjects != NULL)
		AObjUnlockData(oiImportExport, IMP_OBJ_INDEX) ;

	nObjects = (int)AUpsfFieldCount("EXPOBJECTID") ;
	AObjResizeData(oiImportExport, EXP_OBJ_INDEX, nObjects*sizeof(OBJECTID)) ;
	lpObjects = (OBJECTID far*)AObjLockData(oiImportExport, EXP_OBJ_INDEX) ;
	for (i = 0; i < nObjects; i++)
		{
		if(AUpsfImportField("EXPOBJECTID", i, AUPSF_NATV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) != AUPSF_NOERROR)
			{
			AUpsfLogComment("An invalid object ID was transfered to the Export object list.") ;
			lpObjects[i] = 0L ;
			}
		}
	if (lpObjects != NULL)
		AObjUnlockData(oiImportExport, EXP_OBJ_INDEX) ;
	return A_OK ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  LibMain
/////////////////////////////////////////////////////////////////////////////
int FAR PASCAL LibMain( HANDLE hInst, WORD, WORD wHeapSize, LPSTR )
	{
	_WinAllocFlag = GMEM_SHARE ;
	hInstance = hInst ;
	if ( wHeapSize != 0 )
		UnlockData( 0 );

	int cbString = LoadString (hInstance, IDS_FILTERSTRING, lpstrFilter, sizeof(lpstrFilter)) ;
	char chReplace = lpstrFilter[cbString - 1]; // retrieve wildcard character
	for (int i = 0; lpstrFilter[i] != '\0'; i++)
		if (lpstrFilter[i] == chReplace)
			lpstrFilter[i] = '\0' ;

	return 1;   // Indicates that the DLL was initialized successfully.
	}
