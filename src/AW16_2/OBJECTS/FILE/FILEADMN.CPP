/////////////////////////////////////////////////////////////////////////////
// File Name:  FileAdmn.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  4/22/92
//
// Description.: Event Administration Routine for the Serius File object:
////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <mem.h>
#include <a_alm.h>
#include <o_text.h>
#include "fileobj.h"

#define CURRENTDATAVERSION 0

extern "C" long FAR PASCAL _export GetSerVersion(TYPEID) ;
LONG VersionCheck(OBJECTID oiFile, pAObjMessage theSystem) ;
LONG CreateObject(OBJECTID oiFile) ;
LONG ImportObject(OBJECTID oiFile) ;
LONG ExportObject(OBJECTID oiFile) ;

char lpstrFilter[256] ;
HANDLE hInstance ;
extern int _WinAllocFlag;

LONG FAR PASCAL _export AdministrationRoutine(OBJECTID oiFile, pAObjMessage theSystem)
	{
	switch (theSystem->message1)
		{
		case AOBJ_CREATED:
			return CreateObject(oiFile) ;

		case AOBJ_ASSIGNOBJECT: // AOBJ_ASSIGNOBJECT doesn't do anything in the MAC code
			return A_NOTHANDLED ;  // because it doesn't make sence to store object IDs in a database

		case AOBJ_READ :
			return VersionCheck(oiFile, theSystem) ;

		case AOBJ_IMPORTOBJECT :
			return ImportObject(oiFile) ;

		case AOBJ_EXPORTOBJECT :
			return ExportObject(oiFile) ;

		case AOBJ_EDITOBJECT:
			return DialogBoxParam( hInstance, "ObjectEdit", GetActiveWindow(),
					(FARPROC)EditObjectDialogProc, oiFile);

		case AOBJ_VALIDATEOBJECTIDS :
			return ValidateObjectIDs(oiFile, theSystem) ;

		case AOBJ_CHECKOBJECT :
			return EvaluateObject(oiFile, theSystem) ;
		}
	return A_NOTHANDLED ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  VersionCheck
//  Description:  Checks the version compatiblity of the Files's datas
////////////////////////////////////////////////////////////////////////
LONG VersionCheck(OBJECTID oiFile, pAObjMessage theSystem)
	{
	FileInfo far* lpFileInfo ;

	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, 0) ;
	theSystem->message3 = CURRENTDATAVERSION ;
	AObjUnlockData(oiFile, 0) ;
	if(lpFileInfo->version > CURRENTDATAVERSION)
		{
		AObjReportError(oiFile, theSystem->message1, A_ERROR, "File\tALM_FILE.DLL", 0);
		return A_ERROR ;
		}
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  CreateObject
//  Description:  Creates a File object with default values
////////////////////////////////////////////////////////////////////////
LONG CreateObject(OBJECTID oiFile)
	{
	FileInfo far* lpFileInfo ;

	if (!AObjSetDataCount(oiFile, 2))
		{
		MessageBox(NULL, "AObjSetDataCount failed", NULL, MB_OK) ;
		return A_ERROR ;
		}
	if (!AObjResizeData(oiFile, INFO_INDEX, sizeof(FileInfo)))
		{
		MessageBox(NULL, "AObjResizeData failed", NULL, MB_OK) ;
		return A_ERROR ;
		}
	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ;
	lpFileInfo->version = 0 ;
	lstrcpy(lpFileInfo->lpstrFile, "NONAME") ; // default file name
	lstrcpy(lpFileInfo->lpstrDefExt, "ALM") ; // default file extension
	lpFileInfo->nFilterIndex = 0 ;
	lpFileInfo->hFile = 0 ;
	lpFileInfo->wFileAttr = 0 ; // default DOS file attributes
	lpFileInfo->wStatus = 0 ;  // ?
	AObjUnlockData(oiFile, INFO_INDEX) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  ExportObject
//  Description:  Exports the File object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ExportObject(OBJECTID oiFile)
	{
	FileInfo far *lpFileInfo ;
	OBJECTID far *lpObjects ;

	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ;
	AUpsfExportField("EXTENSION", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, lpFileInfo->lpstrDefExt, lstrlen(lpFileInfo->lpstrDefExt)) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	lpObjects = (OBJECTID far*)AObjLockData( oiFile, OBJECT_INDEX ) ;
	int nObjects = (int)AObjGetDataSize (oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ;
	int nObjectsRemaining = nObjects ;
	for (int i = nObjects-1; i >= 0; i--)
		{
		if (!AObjCheckType(lpObjects[i], 0))
			{
			nObjectsRemaining-- ;
			for (int j = i; j < nObjectsRemaining; j++)
				{
				lpObjects[j] = lpObjects[j+1] ; // shift objects
				}
			}
		}
	AObjUnlockData( oiFile, OBJECT_INDEX ) ;
	AObjResizeData(oiFile, OBJECT_INDEX, nObjectsRemaining*sizeof(OBJECTID)) ;
	lpObjects = (OBJECTID far*)AObjLockData( oiFile, OBJECT_INDEX ) ;

	for (i = 0; i < nObjectsRemaining; i++)
		{
		AUpsfExportField("OBJECTID", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) ;
		}

	AObjUnlockData( oiFile, OBJECT_INDEX ) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  ImportObject
//  Description:  Imports the File object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ImportObject(OBJECTID oiFile)
	{
	FileInfo far* lpFileInfo ;
	OBJECTID far *lpObjects ;

	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ;
	if(AUpsfImportField("EXTENSION", 0L, AUPSF_NATV_STRING, lpFileInfo->lpstrDefExt, 4) != AUPSF_NOERROR)
		AUpsfLogComment("Using 'ALM' for default file extension.") ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	int nObjects = (int)AUpsfFieldCount("OBJECTID") ;
	AObjResizeData(oiFile, OBJECT_INDEX, nObjects*sizeof(OBJECTID)) ;
	lpObjects = (OBJECTID far*)AObjLockData(oiFile, OBJECT_INDEX) ;
	for (int i = 0; i < nObjects; i++)
		{
		if(AUpsfImportField("OBJECTID", i, AUPSF_NATV_OBJECTREF, &lpObjects[i], sizeof(OBJECTID)) != AUPSF_NOERROR)
			{
			AUpsfLogComment("An invalid object ID was transfered.") ;
			lpObjects[i] = 0L ;
			}
		}
	AObjUnlockData(oiFile, OBJECT_INDEX) ;
	return A_OK ;
	}

// Every DLL has an entry point LibMain and an exit point WEP.
int FAR PASCAL LibMain( HANDLE hInst, WORD,
								   WORD wHeapSize, LPSTR )
	{
	// The startup code for the DLL initializes the local heap (if there is one)
	// with a call to LocalInit which locks the data segment.
	unsigned int cbString ; // size of filter string (in bytes)
	char chReplace ;

	_WinAllocFlag = GMEM_SHARE ;
	hInstance = hInst ;
	if ( wHeapSize != 0 )
		UnlockData( 0 ) ;

	cbString = LoadString (hInstance, IDS_FILTERSTRING, lpstrFilter, sizeof(lpstrFilter)) ;
	chReplace = lpstrFilter[cbString - 1]; // retrieve wildcard character
	for (int i = 0; lpstrFilter[i] != '\0'; i++)
		if (lpstrFilter[i] == chReplace)
			lpstrFilter[i] = '\0' ;

	return 1;   // Indicate that the DLL was initialized successfully.
	}

// Turn off warning: Parameter '' is never used
#pragma argsused

int FAR PASCAL WEP ( int bSystemExit )
	{
	return 1;
	}
