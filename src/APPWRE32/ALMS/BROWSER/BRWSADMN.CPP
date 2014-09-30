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
#include <intdebug.h>
#include <op_wnd.h>
#include <o_datb.h>
#include "browser.h"
#define CURRENTDATAVERSION 0

extern "C" long CALLBACK /*!!PORT!! was _far_ _pascal_*/  GetSerVersion(ATYPEID) ;
LONG CreateObject(AOBJECTID oiBrowser) ;
LONG VersionCheck(AOBJECTID oiBrowser, pAObjMessage theSystem) ;
LONG AssignObject(AOBJECTID oiSource, AOBJECTID oiDestination) ;
LONG ImportObject(AOBJECTID oiBrowser) ;
LONG ExportObject(AOBJECTID oiBrowser) ;
LONG QueryProtocol(ApQueryInfo* lpQueryInfo) ;

HINSTANCE hInstance ;
HBITMAP hBitmap ;

long CALLBACK /*!!PORT!! was _far_ _pascal_*/  GetSerVersion(ATYPEID)
	{
	return 1L ;
	}

LONG CALLBACK  AdministrationRoutine(AOBJECTID oiBrowser, pAObjMessage theSystem)
	{
	switch (theSystem->message1)
		{
		case AOBJ_CREATED:
			return CreateObject(oiBrowser) ;

		case AOBJ_ASSIGNOBJECT:
			return AssignObject(oiBrowser, (AOBJECTID)theSystem->message2) ;

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
LONG VersionCheck(AOBJECTID oiBrowser, pAObjMessage theSystem)
	{
	BrowserInfo * lpBrowserInfo ;
	long version;

//	OKludgeReportChangedSignals(oiBrowser, 11, 4) ; // the Before Clearing Fields
//	OKludgeReportChangedSignals(oiBrowser, 31, 24) ; // the After Clearing Fields
	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
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
LONG CreateObject(AOBJECTID oiBrowser)
	{
	BrowserInfo * lpBrowserInfo ;

	if (!AObjSetDataCount(oiBrowser, 1))
		{
		ADBG_PRINT_E("AObjSetDataCount failed creating a Browser object");
		return A_ERROR;
		}

	AObjResizeData(oiBrowser, INFO_INDEX, sizeof(BrowserInfo)) ;
	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;

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
LONG AssignObject(AOBJECTID oiSource, AOBJECTID oiDestination)
	{
	BrowserInfo * lpDestinationInfo ;
	BrowserInfo * lpSourceInfo ;

	lpSourceInfo = (BrowserInfo *)AObjLockData(oiSource, INFO_INDEX) ;
	lpDestinationInfo = (BrowserInfo *)AObjLockData(oiDestination, INFO_INDEX) ;
	CopyMemory (lpDestinationInfo, lpSourceInfo, sizeof(BrowserInfo) );
	AObjUnlockData(oiSource, INFO_INDEX) ;
	AObjUnlockData(oiDestination, INFO_INDEX) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  VerifyDatabaseAndIndex
//  Description:  Verify the existence of the Database and Index objects.
////////////////////////////////////////////////////////////////////////
void VerifyDatabaseAndField(BrowserInfo  *lpBrowserInfo)
	{
	if (!AObjCheckType(lpBrowserInfo->oiDatabase, OTYPE_DATABASE))
		{
		lpBrowserInfo->oiDatabase = 0 ;
		lpBrowserInfo->oiField = 0 ;
		return ;
		}
	AMEMBLOCKID keyObjectIDs = AMemAllocate(0) ;
	ODatbGetKeyObjectIDs(lpBrowserInfo->oiDatabase, keyObjectIDs) ;
	AOBJECTID * lpKeys = (AOBJECTID *)AMemLock(keyObjectIDs) ;
	int nKeys = (int)AMemGetSize(keyObjectIDs) / sizeof(AOBJECTID) ;
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
LONG ExportObject(AOBJECTID oiBrowser)
	{
	BrowserInfo  *lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &lpBrowserInfo->version, sizeof(int)) ;
	VerifyDatabaseAndField(lpBrowserInfo) ;
	AUpsfExportField("DATABASE", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpBrowserInfo->oiDatabase, sizeof(AOBJECTID)) ;
	AUpsfExportField("FIELD", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpBrowserInfo->oiField, sizeof(AOBJECTID)) ;
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return A_OK ;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  ImportObject
//  Description:  Imports the Browser object fields using the UPSF format.
////////////////////////////////////////////////////////////////////////
LONG ImportObject(AOBJECTID oiBrowser)
	{
	BrowserInfo * lpBrowserInfo ;

	lpBrowserInfo = (BrowserInfo *)AObjLockData(oiBrowser, INFO_INDEX) ;
	if(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &lpBrowserInfo->version, sizeof(int)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to version 0.") ;
		lpBrowserInfo->version = 0 ;
		}
	if(AUpsfImportField("DATABASE", 0L, AUPSF_NATV_OBJECTREF, &lpBrowserInfo->oiDatabase, sizeof(AOBJECTID)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to no Database object connected to browser.") ;
		lpBrowserInfo->oiDatabase = 0L ;
		}
	if(AUpsfImportField("FIELD", 0L, AUPSF_NATV_OBJECTREF, &lpBrowserInfo->oiField, sizeof(AOBJECTID)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to 'Active Item' for database indexing.") ;
		lpBrowserInfo->oiField = 0L ;
		}
	AObjUnlockData(oiBrowser, INFO_INDEX) ;
	return A_OK ;
	}


///////////////////////////////////////////////////////////////////////
// FUNCTION:  LibMain/*!!PORT!! Replace with DllEntryPoint */
///////////////////////////////////////////////////////////////////////
extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	hInstance = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			hBitmap = LoadBitmap(hInstance, "Browser") ;
			break;

		case DLL_PROCESS_DETACH:
			DeleteObject(hBitmap);
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}

//int CALLBACK /*!!PORT!! was _far_ _pascal_*/ LibMain/*!!PORT!! Replace with DllEntryPoint */( HANDLE hInst, WORD, WORD wHeapSize, LPSTR)
//	{
//	_WinAllocFlag = GMEM_SHARE ;
//	hInstance = hInst ;
//	hBitmap = LoadBitmap(hInstance, "Browser") ;
//	if ( wHeapSize != 0 )
//		UnlockData( 0 ) ;
//	return 1;   // Indicate that the DLL was initialized successfully.
//	}
//
///////////////////////////////////////////////////////////////////////
// FUNCTION:  WEP/*!!PORT!! Replace with DllEntryPoint */
///////////////////////////////////////////////////////////////////////
//int CALLBACK /*!!PORT!! was _far_ _pascal_*/ WEP/*!!PORT!! Replace with DllEntryPoint */ ( int )
//	{
//	DeleteObject(hBitmap);
//	return 1;
//	}
