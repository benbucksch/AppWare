/////////////////////////////////////////////////////////////////////////////
// File Name:  BrwsAdmn.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  7/30/92
//
// Description.: Event Administration Routine for the Serius Browser object:
////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <mem.h>
#include <a_alm.h>
#include <op_wnd.h>
#include <o_datb.h>
#include "browser.h"
#define CURRENTDATAVERSION 0

extern "C" long FAR PASCAL _export GetSerVersion(TYPEID) ;
LONG CreateObject(OBJECTID oiBrowser) ;
LONG VersionCheck(OBJECTID oiBrowser, pAObjMessage theSystem) ;
LONG AssignObject(OBJECTID oiSource, OBJECTID oiDestination) ;
LONG ImportObject(OBJECTID oiBrowser) ;
LONG ExportObject(OBJECTID oiBrowser) ;
LONG QueryProtocol(ApQueryInfo* lpQueryInfo) ;

HANDLE hInstance ;
HBITMAP hBitmap ;

long FAR PASCAL _export GetSerVersion(TYPEID)
	{
	return 1L ;
	}

LONG FAR PASCAL _export AdministrationRoutine(OBJECTID oiBrowser, pAObjMessage theSystem)
	{
	switch (theSystem->message1)
		{
		case AOBJ_CREATED:
			return CreateObject(oiBrowser) ;

		case AOBJ_ASSIGNOBJECT:
			return AssignObject(oiBrowser, (OBJECTID)theSystem->message2) ;

		case AOBJ_READ :
			return VersionCheck(oiBrowser, theSystem) ;

		case AOBJ_IMPORTOBJECT :
			return ImportObject(oiBrowser) ;

		case AOBJ_EXPORTOBJECT :
			return ExportObject(oiBrowser) ;

		case AOBJ_EDITOBJECT:
			return EditObject(oiBrowser) ;

		case AOBJ_VALIDATEOBJECTIDS :
			return ValidateObjectIDs(oiBrowser, theSystem) ;

		case AOBJ_BUILDSIGNALS:
			return BuildSignals(theSystem) ;

		case AOBJ_CHECKOBJECT:
			return EvaluateObject(oiBrowser, theSystem) ;

		case AOBJ_QUERYPROTOCOL:
			return QueryProtocol((ApQueryInfo*)theSystem->message4) ;
		}
	return A_NOTHANDLED ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  QueryProtocol
//  Description:  Indicates whether or not the supplied protocol is
//		supported by this object.
////////////////////////////////////////////////////////////////////////
LONG QueryProtocol(ApQueryInfo* lpQueryInfo)
	{
	if (lpQueryInfo->protocol == APROT_WND)
		{
		lpQueryInfo->pfnCreate = (APPCFN) BrowserCreate ;
		return AP_SUPPORTED;
		}
	return A_NOTHANDLED ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  VersionCheck
//  Description:  Checks the version compatiblity of the Browser's datas
////////////////////////////////////////////////////////////////////////
LONG VersionCheck(OBJECTID oiBrowser, pAObjMessage theSystem)
	{
	BrowserInfo far* lpBrowserInfo ;
	long version;

	OKludgeReportChangedSignals(oiBrowser, 11, 4) ; // the Before Clearing Fields
	OKludgeReportChangedSignals(oiBrowser, 31, 24) ; // the After Clearing Fields
	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	version = lpBrowserInfo->version ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	theSystem->message3 = CURRENTDATAVERSION ;
	if(version > CURRENTDATAVERSION)
		{
		AObjReportError(oiBrowser, theSystem->message1, A_ERROR, "Browser\tALM_BRWS.DLL", 0);
		return A_ERROR ;
		}
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  CreateObject
//  Description:  Creates a Browser object with default values
////////////////////////////////////////////////////////////////////////
LONG CreateObject(OBJECTID oiBrowser)
	{
	BrowserInfo far* lpBrowserInfo ;

	if (!AObjSetDataCount(oiBrowser, 1))
		{
		ADBG_PRINT_E("AObjSetDataCount failed creating a Browser object");
		return A_ERROR;
		}

	AObjResizeData(oiBrowser, INFO_INDEX, sizeof(BrowserInfo)) ;
	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;

	if (lpBrowserInfo == NULL)
		{
		ADBG_PRINT_E("AObjLockData failed creating a Browser object");
		return A_ERROR;
		}

	lpBrowserInfo->version = 0 ;
	lpBrowserInfo->oiDatabase = 0 ;
	lpBrowserInfo->oiField = 0 ;
	lpBrowserInfo->file = TRUE ; // only significant at run time
	lpBrowserInfo->cancel = FALSE ;

	AObjUnlockData(oiBrowser, INFO_INDEX) ;

	return A_OK ;
	}
////////////////////////////////////////////////////////////////////////
//  Function:  AssignObject
//  Description:  Creates a Browser object with default values
////////////////////////////////////////////////////////////////////////
LONG AssignObject(OBJECTID oiSource, OBJECTID oiDestination)
	{
	BrowserInfo far* lpDestinationInfo ;
	BrowserInfo far* lpSourceInfo ;

	lpSourceInfo = (BrowserInfo far*)AObjLockData(oiSource, INFO_INDEX) ;
	lpDestinationInfo = (BrowserInfo far*)AObjLockData(oiDestination, INFO_INDEX) ;
	_fmemcpy (lpDestinationInfo, lpSourceInfo, sizeof(BrowserInfo) );
	AObjUnlockData(oiSource, INFO_INDEX) ;
	AObjUnlockData(oiDestination, INFO_INDEX) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  VerifyDatabaseAndIndex
//  Description:  Verify the existence of the Database and Index objects.
////////////////////////////////////////////////////////////////////////
void VerifyDatabaseAndField(BrowserInfo far *lpBrowserInfo)
	{
	if (!AObjCheckType(lpBrowserInfo->oiDatabase, OTYPE_DATABASE))
		{
		lpBrowserInfo->oiDatabase = 0 ;
		lpBrowserInfo->oiField = 0 ;
		return ;
		}
	MEMBLOCKID keyObjectIDs = AMemAllocate(0) ;
	ODatbGetKeyObjectIDs(lpBrowserInfo->oiDatabase, keyObjectIDs) ;
	OBJECTID far* lpKeys = (OBJECTID far*)AMemLock(keyObjectIDs) ;
	int nKeys = (int)AMemGetSize(keyObjectIDs) / sizeof(OBJECTID) ;
	for (int i = 0 ; i < nKeys; i++)
		{
		if (lpKeys[i] == lpBrowserInfo->oiField)
			break ;
		}
	if (i == nKeys)
		lpBrowserInfo->oiField = 0 ;
	AMemUnlock(keyObjectIDs) ;
	AMemFree(keyObjectIDs) ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  ExportObject
//  Description:  Exports the Browser object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ExportObject(OBJECTID oiBrowser)
	{
	BrowserInfo far *lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &lpBrowserInfo->version, sizeof(int)) ;
	VerifyDatabaseAndField(lpBrowserInfo) ;
	AUpsfExportField("DATABASE", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpBrowserInfo->oiDatabase, sizeof(OBJECTID)) ;
	AUpsfExportField("FIELD", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpBrowserInfo->oiField, sizeof(OBJECTID)) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  ImportObject
//  Description:  Imports the Browser object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ImportObject(OBJECTID oiBrowser)
	{
	BrowserInfo far* lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
	if(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &lpBrowserInfo->version, sizeof(int)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to version 0.") ;
		lpBrowserInfo->version = 0 ;
		}
	if(AUpsfImportField("DATABASE", 0L, AUPSF_NATV_OBJECTREF, &lpBrowserInfo->oiDatabase, sizeof(OBJECTID)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to no Database object connected to browser.") ;
		lpBrowserInfo->oiDatabase = 0L ;
		}
	if(AUpsfImportField("FIELD", 0L, AUPSF_NATV_OBJECTREF, &lpBrowserInfo->oiField, sizeof(OBJECTID)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to 'Active Item' for database indexing.") ;
		lpBrowserInfo->oiField = 0L ;
		}
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return A_OK ;
	}

///////////////////////////////////////////////////////////////////////
// FUNCTION:  LibMain
///////////////////////////////////////////////////////////////////////
int FAR PASCAL LibMain( HANDLE hInst, WORD, WORD wHeapSize, LPSTR)
	{
	_WinAllocFlag = GMEM_SHARE ;
	hInstance = hInst ;
	hBitmap = LoadBitmap(hInstance, "Browser") ;
	if ( wHeapSize != 0 )
		UnlockData( 0 ) ;
	return 1;   // Indicate that the DLL was initialized successfully.
	}

///////////////////////////////////////////////////////////////////////
// FUNCTION:  WEP
///////////////////////////////////////////////////////////////////////
int FAR PASCAL WEP ( int )
	{
	DeleteObject(hBitmap);
	return 1;
	}
