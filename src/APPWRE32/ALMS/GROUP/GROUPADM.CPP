/////////////////////////////////////////////////////////////////////////////
// File Name:  Groupadm.cpp
// 		Copyright 1989-92 Serius Corporation
// Author:  Sibai Li
// Date:  5/15/92
//
// Description.: Object Administration Routine for the Serius Group object:
////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>
#include "groupobj.h"

#define CURRENTDATAVERSION 0

extern "C" long ALMCALLBACK GetSerVersion(ATYPEID) ;
LONG VersionCheck(pAObjMessage theSystem) ;

#ifdef __BORLANDC__
extern int		_WinAllocFlag;
#endif

HINSTANCE hInstance ;
void ExportObject ( AOBJECTID ) ;
void ImportObject ( AOBJECTID ) ;

long ALMCALLBACK GetSerVersion(ATYPEID)
	{
	return 1L ;
	}

LONG ALMCALLBACK GroupAdministration(AOBJECTID theObject, pAObjMessage theSystem)
	{
	LONG				lRet				= A_OK;
	AOBJECTID    sObject ;

	switch (theSystem->message1)
		{
		case AOBJ_CREATED:
			AObjSetDataCount(theObject, 1) ;
			break;

		case AOBJ_ASSIGNOBJECT:
			sObject = (AOBJECTID) theSystem->message3 ;
			AObjResizeData(theObject, 0, AObjGetDataSize(sObject,0)) ;
			AObjSetData(theObject, 0, AObjLockData(sObject,0),
																AObjGetDataSize(sObject,0)) ;
			AObjUnlockData(sObject, 0);
			break;

		case AOBJ_READ :
			lRet = VersionCheck(theSystem) ;
			break ;

		case AOBJ_IMPORTOBJECT:
			ImportObject (theObject) ;
			break ;

		case AOBJ_EXPORTOBJECT:
			ExportObject (theObject) ;
			break ;

		case AOBJ_EDITOBJECT:
			CheckObjID (theObject) ;
			DialogBoxParam(hInstance, "group",GetActiveWindow(), (DLGPROC)GroupObjectProc, (LPARAM)theObject) ;
			lRet = A_MODIFIED;
			break ;

		case AOBJ_VALIDATEOBJECTIDS:
			doValidateObjectIDs ( theObject, theSystem) ;
			break ;

		case AOBJ_CHECKOBJECT:
			CheckObjID(theObject) ;
			break ;

		default:
			lRet = A_NOTHANDLED ;
			break ;
		}

	return lRet;
	}

////////////////////////////////////////////////////////////////////////
//  Function:  VersionCheck
//  Description:  Checks the version compatiblity of the Groups's datas
////////////////////////////////////////////////////////////////////////
LONG VersionCheck(pAObjMessage theSystem)
	{
	I_UNUSED(theSystem) ;
	return A_OK;
	}

void ExportObject ( AOBJECTID theObject )
{

	LPOBJECTID       objectPtr ;
    int               i ;

    CheckObjID(theObject) ;
	if (!(objectPtr=(LPOBJECTID) AObjLockData (theObject, 0))) return ;
	for(i=0; i<AObjGetDataSize (theObject, 0)/sizeof(AOBJECTID); i++)
	{
		AUpsfExportField ("OBJID", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &objectPtr[i], sizeof(AOBJECTID)) ;
	}

	AObjUnlockData(theObject, 0);
	return ;
}

void ImportObject ( AOBJECTID theObject )
{
	LPOBJECTID      objectPtr ;
    long             Count ;
	int              i ;

	Count = AUpsfFieldCount ("OBJID") ;
	AObjResizeData (theObject, 0, Count* sizeof(AOBJECTID)) ;
	objectPtr = (LPOBJECTID) AObjLockData (theObject, 0) ;
	for (i=0; i<Count; i++)
	{
		if (AUPSF_NOERROR != AUpsfImportField ("OBJID", i, AUPSF_NATV_OBJECTREF, &objectPtr[i], sizeof(AOBJECTID)))
		{
			AUpsfLogComment ("Import Error, Defaulting to Object ID NULL.") ;
			objectPtr[i] = NULL ;
        }
	}

	AObjUnlockData (theObject, 0) ;
    return ;
}
///////////////////////////////////////////////////////////////////////////
//DllEntryPoint
///////////////////////////////////////////////////////////////////////////
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
			break;

		case DLL_PROCESS_DETACH:
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



// Every DLL has an entry point LibMain/*!!PORT!! Replace with DllEntryPoint */ and an exit point WEP/*!!PORT!! Replace with DllEntryPoint */.
//int CALLBACK /*!!PORT!! was _far_ _pascal_*/ LibMain/*!!PORT!! Replace with DllEntryPoint */
//	( 
//	HINSTANCE hInst, 
//	WORD wDataSegment,
//	WORD wHeapSize, 
//	LPSTR lpszCmdLine 
//	)
//	{
//	I_UNUSED(wDataSegment);
//	I_UNUSED(lpszCmdLine);
//
//	// The startup code for the DLL initializes the local heap (if there is one)
//	// with a call to LocalInit which locks the data segment.
//
//#ifdef __BORLANDC__
//	_WinAllocFlag = GMEM_SHARE;
//#endif
//
//	hInstance = hInst ;
//	if ( wHeapSize != 0 )
//		UnlockData( 0 );
//
//	return 1;   // Indicate that the DLL was initialized successfully.
//	}


//int CALLBACK /*!!PORT!! was _far_ _pascal_*/ WEP/*!!PORT!! Replace with DllEntryPoint */ 
//	( 
//	int bSystemExit 
//	)
//	{
//	I_UNUSED(bSystemExit);
//
//	return 1;
//	}
