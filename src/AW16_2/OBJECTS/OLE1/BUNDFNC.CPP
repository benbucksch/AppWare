//////////////////////////////////////////////////////////////////////////
//File Name:    BuildName
//              Copyright 1989-92 Serius Corporation
//Author:		Sibai Li
//Date:			June 1, 1992
//Description:
//
//             This function bundles the selected object to
//             the object that name bundle object.
//Input 1:     The Bundled object
//Flow 1:      After bundle the object
//Flow 2:      if an error occurs
//////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <windows.h>
#include <memory.h>
#include <a_alm.h>
#include "intdebug.h"

#include "bundle.h"
void ALMCALLBACK BuildFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID    bObject ;  				//the bundle object's objectID
	LPOBJECTID  bObjPtr ;         //pointer to object IDs for objects participating
	long			  nStartingIndex ;
	long        nBundleDatas ;
	long        objectsNum ;
	MEMBLOCKID  memBlck ;
	LPVOID      memBlockPtr ;
	WORD       t ;

	I_UNUSED(theEvent);

	if((bObject=AFuncGetTypedParameter(1,OTYPE_BUNDLE)) == 0)
		{
		AFuncReportResult(theSystem, 1, "Could't get object while building bundle") ;
		}
	else
	  {
		//nBundleDatas = AObjGetDataCount(bObject) ;
		bObjPtr =(LPOBJECTID) AObjLockData(bObject, 0) ;
		objectsNum = AObjGetDataSize(bObject,0)/sizeof(OBJECTID) ;
		if (bObjPtr)
		  {
			memBlck = AMemAllocate (0) ;
			if (memBlck)
			  {
				for (t=0,nStartingIndex=1; t<objectsNum; t++, nStartingIndex++)
					{
					nBundleDatas = BUNDLEDATAS+nStartingIndex ;
					AObjCopyDatas (bObjPtr[t], memBlck) ;
					memBlockPtr = (LPVOID)AMemLock (memBlck) ;
					if (memBlockPtr)
						{
						AObjSetDataCount (bObject, nBundleDatas) ;
						AObjSetData (bObject, nStartingIndex, memBlockPtr, AMemGetSize(memBlck)) ;
						}
					AMemUnlock (memBlck) ;
					}
				AMemFree (memBlck) ;
				}
			}		  
		AObjUnlockData(bObject, 0) ;
		}
	AFuncReportResult(theSystem, 1 ,"") ;
}
 



