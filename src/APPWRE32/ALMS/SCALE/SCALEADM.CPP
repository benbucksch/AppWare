//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
//
//Function:    ScaleAdm.cpp
//Description: scale administration program
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////
#define 	STRICT
#define  	ASTRICT

#include <windows.h>
#include <string.h>
#include <mem.h>
#include <a_alm.h>
#include "intdebug.h"
#include "scale.h"
#define CURRENTVERSION 0

HINSTANCE hInstance ;
HBITMAP   hBORDER ;

long VersionCheck(AOBJECTID, pAObjMessage) ;
extern BOOL InitLayoutClass (LPSTR lpszClassName);
void ImportObject ( AOBJECTID ) ;
void ExportObject ( AOBJECTID ) ;
void DoValidateObjID (AOBJECTID, pAObjMessage) ;
void CreateSignalSet(pAObjMessage) ;
void DoCheckObject (AOBJECTID) ;
extern OpWndItemD* ALMAPI vScaleCreate(AOBJECTID,AOBJECTID,RECT*,long,long) ;

long CALLBACK /*!!PORT!! was _far_ _pascal_*/  ScaleAdministration (
	AOBJECTID 		 theObject,
	pAObjMessage theSystem)
{
	long          rVal = A_OK;
	SCALE        *scale ;

	switch (theSystem->message1)
		{
		case AOBJ_AWAKENED:
    	scale = new SCALE (theObject) ;
			AObjSetRuntimeInfo(theObject, (long) scale);  //Maybe have some problems
			rVal = A_CONTINUE;
			break;

		case AOBJ_ASLEEP:
			scale =(SCALE*) AObjGetRuntimeInfo (theObject) ;
			if (scale)
				delete scale ;
			break ;

		case AOBJ_CREATED :
			scale = new SCALE(theObject) ;
			AObjSetRuntimeInfo(theObject, (long)scale) ;
			//delete scale ;
			break ;

		case AOBJ_READ :
			VersionCheck(theObject, theSystem) ;
			break ;

		case AOBJ_ASSIGNOBJECT:
			break ;

		case AOBJ_IMPORTOBJECT:
			ImportObject(theObject) ;
      break ;

		case AOBJ_EXPORTOBJECT:
			ExportObject(theObject) ;
			break ;

		// the following events have been defined for the APPWARE environment
		// Aug. 25, 1993
		case AOBJ_QUERYPROTOCOL:
    	{
			ApQueryInfo *pQueryInfo = (ApQueryInfo *) theSystem->message4;
			if (pQueryInfo->protocol == APROT_WND)  // if the window protocol is supported
				{
				 //theSystem->message3 = APROT_STATIC;
				 pQueryInfo->pfnCreate = (APPCFN) vScaleCreate; // create function for
				 rVal = AP_SUPPORTED;
				}
			break;
			}

		case AOBJ_EDITOBJECT:
      int ok;
			//scale = new SCALE(theObject) ;
			scale =(SCALE*) AObjGetRuntimeInfo (theObject) ;
			ok = DialogBoxParam(hInstance, "scale", GetActiveWindow(), (DLGPROC)ScaleObjectDlg, (LPARAM)scale) ;
			//delete scale ;
			if (ok)
				rVal = A_MODIFIED;
			else if (ok == -1)
        rVal = A_ERROR;
			break ;

		case AOBJ_VALIDATEOBJECTIDS:
    	DoValidateObjID (theObject, theSystem) ;
			break ;

		case AOBJ_BUILDSIGNALS:
    	CreateSignalSet(theSystem) ;
			break;

		case AOBJ_CHECKOBJECT :
			DoCheckObject (theObject) ;
			break ;
		}
	return rVal;

	}

////////////////////////////////////////////////////////////////////////
//  Function:  VersionCheck
//  Description:  Checks the version compatiblity of the Scale's datas
////////////////////////////////////////////////////////////////////////
long VersionCheck(AOBJECTID oiObj, pAObjMessage theSystem)
{
	long   rVal = A_OK;
	DATA0* data0 ;

	data0 = (DATA0*)AObjLockData (oiObj, BLOCKDATA);
	if (data0)
		{
	 	theSystem->message3 = CURRENTVERSION ;
		if (data0->version > CURRENTVERSION)
			{ AObjReportError(oiObj, theSystem->message1, A_ERROR,
					"Scale\tALM_SCAL.DLL", 1);
    	rVal = A_ERROR;
			}
    AObjUnlockData (oiObj, BLOCKDATA) ;
		}
         
	return rVal;
}
//
//
//
void DoValidateObjID (
	AOBJECTID oiObj,
	pAObjMessage theSystem)
{
	SCALE* scale ;
	scale = (SCALE*)AObjGetRuntimeInfo (oiObj) ;
	if (scale)
	  {
		AObjValidateID (&scale->data0->min, theSystem) ;
		AObjValidateID (&scale->data0->max, theSystem) ;
		AObjValidateID (&scale->data0->val, theSystem) ;
	  }
}
void CreateSignalSet(
	pAObjMessage theSystem)
{
	pAObjSignalRecord signal ;
	AMEMBLOCKID        signalMID = (AMEMBLOCKID)theSystem->message2;

	if (signalMID)
  	{
		AMemSetSize(signalMID, 3 * sizeof(AObjSignalRecord));
		signal = (pAObjSignalRecord)AMemLock(signalMID);

		signal[0].theID = 1 ; //0 ;
		signal[1].theID = 2 ;//UserChanged ; //1 ;
		signal[2].theID = 3 ; //AfterSynch-dragging ; //2;
		lstrcpy(signal[0].theName, "Changed");
		lstrcpy(signal[1].theName, "User Changed");
		lstrcpy(signal[2].theName, "After Synch-dragging");
		AMemUnlock(signalMID);
		}
}
void DoCheckObject (AOBJECTID oiObj)
{
	DATA0* data0 ;

	data0 = (DATA0*)AObjLockData (oiObj, BLOCKDATA);
	if (data0)
		{             
		if (data0->min==NULL)
			{
			AObjReportError	(oiObj,
											 AOBJ_CHECKOBJECT,
											 A_ERROR,
											 "There isn't a Number object connected to the minimum value of scale.",
											 0);
			}		 
		if (data0->max == NULL)
		  {
			AObjReportError	(oiObj,
											 AOBJ_CHECKOBJECT,
											 A_ERROR,
											 "There isn't a Number object connected to the maximum value of scale.",
											 0);
	    }
		if (data0->val == NULL)
		  {
			AObjReportError	(oiObj,
											 AOBJ_CHECKOBJECT,
											 A_ERROR,
											 "There isn't a Number object connected to the value of scale.",
											 0);
			}
		AObjUnlockData (oiObj, 0);
		}
}
//
//
//
void ImportObject (
	AOBJECTID theObject)
{
	DATA0 *data0 ;

	data0 = (DATA0*)AObjLockData (theObject, BLOCKDATA) ;
	if (data0)
		{
		if (AUPSF_NOERROR != AUpsfImportField ("VERSION", 0, AUPSF_NATV_OBJECTREF, &data0->version, sizeof(int)))
  		{
			AUpsfLogComment ("Cannot import version, defaulting to version 0") ;
      data0->version = 0 ;
			}
  	if (AUPSF_NOERROR != AUpsfImportField ("MINMUM", 0, AUPSF_NATV_OBJECTREF, &data0->min, sizeof(AOBJECTID)))
  		{
			AUpsfLogComment ("Cannot import minmum, defaulting to minmum 0") ;
      data0->min = 0 ;
			}
  	if (AUPSF_NOERROR != AUpsfImportField ("VALUE", 0, AUPSF_NATV_OBJECTREF, &data0->val, sizeof(AOBJECTID)))
  		{
			AUpsfLogComment ("Cannot import value, defaulting to value 0") ;
      data0->val = 0 ;
			}
    if (AUPSF_NOERROR != AUpsfImportField ("MAXMUM", 0, AUPSF_NATV_OBJECTREF, &data0->max, sizeof(AOBJECTID)))
    	{
			AUpsfLogComment ("Cannot import maxmum, defaulting to maxmum 0") ;
      data0->max = 0 ;
    	}
    
    if (AUPSF_NOERROR != AUpsfImportField ("MINPOS", 0, AUPSF_NATV_POINT, &data0->minpos, sizeof(POINT)))
    	{
			AUpsfLogComment ("Cannot import minpos!") ;
      data0->minpos.x = 0 ;
      data0->minpos.y = 0 ;
    	}

    if (AUPSF_NOERROR != AUpsfImportField ("MAXPOS", 0, AUPSF_NATV_POINT, &data0->maxpos, sizeof(POINT)))
    	{
			AUpsfLogComment ("Cannot export value!") ;
      data0->maxpos.x = 0 ;
      data0->maxpos.y = 0 ;
    	}

    if (AUPSF_NOERROR != AUpsfImportField ("SCLPOS", 0, AUPSF_NATV_POINT, &data0->scalepos, sizeof(POINT)))
    	{
			AUpsfLogComment ("Cannot export value!") ;
    	data0->scalepos.x = 0 ;
    	data0->scalepos.y = 0 ;
    	}

    if (AUPSF_NOERROR != AUpsfImportField ("CANDRAG", 0, AUPSF_NATV_BOOL, &data0->canDrag, sizeof(BOOL)))
    	{
			AUpsfLogComment ("Cannot import CanDrag!") ;
      data0->canDrag = TRUE ;
			}

    if (AUPSF_NOERROR != AUpsfImportField ("STAMP", 0, AUPSF_NATV_BOOL, &data0->stamp, sizeof(BOOL)))
    	{
			AUpsfLogComment ("Cannot import stamp!") ;
      data0->stamp = TRUE ;
			}

    if (AUPSF_NOERROR != AUpsfImportField ("CANACTIVATE", 0, AUPSF_NATV_BOOL, &data0->canActivate, sizeof(BOOL)))
    	{
			AUpsfLogComment ("Cannot import can Activate!") ;
      data0->canActivate = TRUE ;
    	}
    if (AUPSF_NOERROR != AUpsfImportField ("WASSETUP", 0, AUPSF_NATV_BOOL, &data0->wasSetup, sizeof(BOOL)))
    	{
			AUpsfLogComment ("Cannot import wassetup!") ;
      data0->wasSetup = FALSE ;
    	}
    if (AUPSF_NOERROR != AUpsfImportField ("SYNCH", 0, AUPSF_NATV_BOOL, &data0->synchronous, sizeof(BOOL)))
    	{
			AUpsfLogComment ("Cannot import synchronous!") ;
      data0->synchronous = FALSE ;
			}
    AObjUnlockData (theObject, BLOCKDATA) ;
		}

	long ret ;
	long bmpsize ;
	LPBITMAPINFOHEADER lpbmpInfo ;
	ret = AUpsfFieldInfo ("INDBMP", 0, AUPSF_NATV_BITMAPINFO, NULL, (LPLONG)&bmpsize) ;
	if (ret==AUPSF_NOERROR)
		{
		AMEMBLOCKID theBlock = AMemAllocate (bmpsize) ;
		lpbmpInfo = (LPBITMAPINFOHEADER) AMemLock (theBlock) ;
		if (lpbmpInfo)
			{
			ret =AUpsfImportField ("INDBMP", 0, AUPSF_NATV_BITMAPINFO, (LPVOID)lpbmpInfo, bmpsize) ;
			if (ret!=AUPSF_NOERROR)
				{
				AUpsfLogComment ("Error importing indicator bitmap!") ;
				}
			AObjSetData(theObject, INDICATOR, lpbmpInfo, bmpsize) ;
    	AMemFree (theBlock) ;
			}
		}
	ret = AUpsfFieldInfo ("SCALEBMP", 0, AUPSF_NATV_BITMAPINFO, NULL, (LPLONG)&bmpsize) ;
	if (ret==AUPSF_NOERROR)
		{
		AMEMBLOCKID theBlock = AMemAllocate (bmpsize) ;
		lpbmpInfo = (LPBITMAPINFOHEADER) AMemLock (theBlock) ;
		if (lpbmpInfo)
			{
			ret =AUpsfImportField ("SCALEBMP", 0, AUPSF_NATV_BITMAPINFO, (LPVOID)lpbmpInfo, bmpsize) ;
			if (ret!=AUPSF_NOERROR)
				{
				AUpsfLogComment ("Error importing scale bitmap!") ;
				}
			AObjSetData(theObject, DIAL, lpbmpInfo, bmpsize) ;
    	AMemFree (theBlock) ;
			}
		}
}

void ExportObject (AOBJECTID theObject)
{
	DATA0 *data0 ;

	data0 =(DATA0*) AObjLockData (theObject, BLOCKDATA) ;
	if (data0)
  	{
	  AUpsfExportField ("MINMUM", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->min, sizeof(AOBJECTID)) ;
  	AUpsfExportField ("VALUE",  0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->val, sizeof(AOBJECTID)) ;
  	AUpsfExportField ("MAXMUM", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data0->max, sizeof(AOBJECTID)) ;
	  AUpsfExportField ("MINPOS", 0, AUPSF_NATV_POINT, AUPSF_UNIV_POINT, &data0->minpos, sizeof(POINT)) ;
  	AUpsfExportField ("MAXPOS", 0, AUPSF_NATV_POINT, AUPSF_UNIV_POINT, &data0->maxpos, sizeof(POINT)) ;
  	AUpsfExportField ("SCLPOS", 0, AUPSF_NATV_POINT, AUPSF_UNIV_POINT, &data0->scalepos, sizeof(POINT)) ;
	  AUpsfExportField ("CANDRAG", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->canDrag, sizeof(BOOL)) ;
  	AUpsfExportField ("STAMP",   0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->stamp, sizeof(BOOL)) ;
  	AUpsfExportField ("WASSETUP",0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->wasSetup,sizeof(BOOL)) ;
  	AUpsfExportField ("SYNCH",   0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->synchronous, sizeof(BOOL)) ;
		AUpsfExportField ("CANACTIVATE", 0, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &data0->canActivate, sizeof(BOOL)) ;
    AObjUnlockData(theObject, BLOCKDATA) ;
		}

	long size = AObjGetDataSize (theObject, INDICATOR) ;
	LPBITMAPINFOHEADER lpbmp = (LPBITMAPINFOHEADER) AObjLockData (theObject, INDICATOR) ;
	if (lpbmp)AUpsfExportField ("INDBMP",0, AUPSF_NATV_BITMAPINFO, AUPSF_UNIV_BITMAP, lpbmp, size) ;
	AObjUnlockData (theObject, INDICATOR) ;
	size = AObjGetDataSize (theObject, DIAL) ;
	lpbmp = (LPBITMAPINFOHEADER) AObjLockData (theObject, DIAL) ;
	if (lpbmp)AUpsfExportField ("SCALEBMP",0, AUPSF_NATV_BITMAPINFO, AUPSF_UNIV_BITMAP, lpbmp, size) ;
	AObjUnlockData (theObject, DIAL) ;
}
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
			hBORDER = LoadBitmap(hInstance, "scale");
			PaletteCls::CreatePalette () ;
 			if (InitLayoutClass("PositionWndClass") == 0)
				break ;
			break;

		case DLL_PROCESS_DETACH:
			DeleteObject(hBORDER); // free the border bitmap
	    PaletteCls::DestroyPalette();
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
//int CALLBACK /*!!PORT!! was _far_ _pascal_*/ LibMain/*!!PORT!! Replace with DllEntryPoint */(
//  HINSTANCE hInst,
//  WORD      wDataSegment,
//  WORD      wHeapSize,
//  LPSTR     lpszCmdLine)
//{
	// The startup code for the DLL initializes the local heap (if there is one)
  // with a call to LocalInit which locks the data segment.
//  _WinAllocFlag = GMEM_SHARE ;

//  hInstance = hInst ;
//	hBORDER = LoadBitmap(hInstance, "scale");
//	PaletteCls::CreatePalette () ;
//  if ( wHeapSize != 0 )
//    UnlockData( 0 );

//	if (InitLayoutClass("PositionWndClass") == 0)
//		return 0;

//	I_UNUSED(wDataSegment);
//	I_UNUSED(lpszCmdLine);

//	return 1;   // Indicate that the DLL was initialized successfully.
//}

//int CALLBACK /*!!PORT!! was _far_ _pascal_*/ WEP/*!!PORT!! Replace with DllEntryPoint */ ( int bSystemExit )
//{
//	I_UNUSED (bSystemExit) ;
//	DeleteObject(hBORDER); // free the border bitmap

//  PaletteCls::DestroyPalette();
//  return 1;
//}



