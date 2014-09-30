/////////////////////////////////////////////////////////////////////////////
// File Name:  BundleA.cpp
// 		Copyright 1989-92 Serius Corporation
// Author:  Sibai Li
// Date:  5/15/92
//
// Description.: Object Administration Routine for the Serius Bundle object:
////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <windows.h>
#include <memory.h>
#include <a_alm.h>
#include <intdebug.h>
#include "bundle.h"

#define CURRENTDATAVERSION 0

#ifdef __BORLANDC__
extern int		_WinAllocFlag;
#endif

HINSTANCE hInstance ;
void ExportObject ( OBJECTID ) ;
void ImportObject ( OBJECTID ) ;
void DoAssign (OBJECTID, OBJECTID) ;
void BuildSignal (MEMBLOCKID) ;
LONG VersionCheck(pAObjMessage theSystem) ;
extern "C" long ALMCALLBACK GetSerVersion(TYPEID) ;

long ALMCALLBACK GetSerVersion(TYPEID)
	{
	return 1L ;
	}

LONG ALMCALLBACK BundleAdministration(
	OBJECTID			theObject,
	pAObjMessage	theSystem)
{
	LONG							lRet = A_OK;

	switch (theSystem->message1)
		{
		case AOBJ_CREATED:
			AObjSetDataCount(theObject, 2) ;
			break;

		case AOBJ_ASSIGNOBJECT:
			DoAssign (theObject, (OBJECTID)theSystem->message3) ;
			break;

		case AOBJ_READ :
			lRet = VersionCheck(theSystem) ;
			break ;

		case AOBJ_EXPORTOBJECT:																	
			ExportObject (theObject) ;
			break ;

		case AOBJ_IMPORTOBJECT:
			ImportObject (theObject) ;
			break ;

	  case AOBJ_EDITOBJECT:
		   CheckObjID (theObject) ;
		   DialogBoxParam(hInstance, "bundle", GetActiveWindow(), (DLGPROC)BundleObjectProc, (LPARAM)theObject) ;
			 lRet = A_MODIFIED;
		   break ;

	  case AOBJ_VALIDATEOBJECTIDS:
		   doValidateObjectIDs ( theObject, theSystem) ;
		   break ;

    case AOBJ_CHECKOBJECT:
			CheckObjID (theObject) ;
			break ;

	  case AOBJ_BUILDSIGNALS:
			BuildSignal ((MEMBLOCKID)theSystem->message2) ;
      break;
       
		default:
			lRet = A_NOTHANDLED ;
			break ;
		}

	return lRet;
}

////////////////////////////////////////////////////////////////////////
//  Function:  VersionCheck
//  Description:  Checks the version compatiblity of the Bundle's datas
////////////////////////////////////////////////////////////////////////
LONG VersionCheck(pAObjMessage /*theSystem*/)
	{
	return A_OK;
	}

void ExportObject ( OBJECTID theObject )
{
	LPOBJECTID       objectPtr ;
	int               i ;

    CheckObjID (theObject) ;
	if (!(objectPtr=(LPOBJECTID) AObjLockData (theObject, 0))) return ;

	for(i=0; i<AObjGetDataSize (theObject, 0)/sizeof(OBJECTID); i++)
	{
		AUpsfExportField ("OBJID",    i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &objectPtr[i], sizeof(OBJECTID)) ;
	}

	AObjUnlockData (theObject, 0) ;
	return ;
}
void DoAssign (OBJECTID destObjID, OBJECTID srcObjID)
{
	LPOBJECTID    destObjects ;
	LPOBJECTID    srcObjects ;
	WORD          destSize ;
	WORD          srcSize ;
	WORD          destIndex=0 ;
	WORD          srcIndex=0 ;
	LPVOID        blockPtr ;

	srcObjects = (LPOBJECTID)AObjLockData (srcObjID, 0) ;
	srcSize= AObjGetDataSize (srcObjID, 0)/sizeof(OBJECTID) ;
	if (srcObjects)
	  {
		destObjects = (LPOBJECTID) AObjLockData (destObjID, 0)  ;
		if (destObjects)
		  {
			destSize = AObjGetDataSize (destObjID, 0)/sizeof(OBJECTID) ;
			for (destIndex=0; destIndex<destSize;destIndex++ )
				{
				if (srcIndex==srcSize)
					srcIndex = 0 ;
				blockPtr = (LPVOID) AObjLockData (srcObjID, srcIndex+1) ;
				if (blockPtr)
					{
					AObjRestoreDatas (destObjects[destIndex], blockPtr) ;
					AObjUnlockData (srcObjID, srcIndex+1) ;
					}
				srcIndex++ ;
				}
			AObjUnlockData (destObjID, 0)  ;
			}
		AEvtPostSignalAtTail(destObjID, 0);
		AObjUnlockData (srcObjID, 0) ;
		}
}
void BuildSignal(MEMBLOCKID signalMID) 
{
	pAObjSignalRecord	signal;

	AMemSetSize(signalMID, sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(signalMID);
	signal[0].theID = 0 ;
	lstrcpy(signal[0].theName, "Dispatched");
	AMemUnlock (signalMID) ;
}
//
//
//
void ImportObject ( OBJECTID theObject )
{
	LPOBJECTID       objectPtr ;
	long              Count ;
    int               i ;

	Count = AUpsfFieldCount ("OBJID") ;
	AObjResizeData (theObject, 0, Count* sizeof(OBJECTID)) ;
	objectPtr = (LPOBJECTID) AObjLockData(theObject, 0);

	for (i=0; i<Count; i++)
	{
		if (AUPSF_NOERROR != AUpsfImportField ("OBJID", i, AUPSF_NATV_OBJECTREF, &objectPtr[i], sizeof(OBJECTID)))
		{
			AUpsfLogComment ("Import Error, Defaulting to Object ID NULL.") ;
            objectPtr[i] = NULL ;
        }
	}

	AObjUnlockData(theObject, 0);

	return ;
}

// Every DLL has an entry point LibMain and an exit point WEP.
int CALLBACK LibMain( 
	HINSTANCE hInst, 
	WORD      wDataSeg,
  WORD      wHeapSize,
  LPSTR     lpszCmdLine)
{
	// The startup code for the DLL initializes the local heap (if there is one)
  // with a call to LocalInit which locks the data segment.
	I_UNUSED(wDataSeg);
  I_UNUSED(lpszCmdLine);

#ifdef __BORLANDC__
	_WinAllocFlag = GMEM_SHARE;
#endif

	hInstance = hInst ;
	if ( wHeapSize != 0 )
		UnlockData( 0 );

	return 1;   // Indicate that the DLL was initialized successfully.
	}


int CALLBACK WEP ( int )
	{
	return 1;
	}
