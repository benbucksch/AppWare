/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
// REVIEWER: Scott McCarty, Kevin Smart
//					 Nov. 11, 1993
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT

#include "a_alm.h"

#include "picture.h"
#include "stdlib.h"
#include <mem.h>
#include "math.h"
#include "heap.h"

#define MS_BMTYPE                       0x4d42

extern int FAR PASCAL PictureDlgProc(HWND hDlg,unsigned message, WORD wParam,LONG lParam);
extern HPALETTE CreateDIBPaletteHandle(
							BITMAPINFOHEADER *lpInfo,
							MEMBLOCKID memID
							);
extern void DestroyPalette(HPALETTE hpal);
extern HINSTANCE		hInst;
extern void InitPictObject(OBJECTID objID);
extern LPLONG GetPictObject(OBJECTID objID);
extern int DeletePHandle(PICTOBJECT *po);
extern OpWndItemD* ALMAPI vPictureCreate(
						OBJECTID		oiItem,
						OBJECTID		oiParent,
						RECT*				rcItem,
						long				status,
						long				special
						);


extern "C" {

long UpdateObject(OBJECTID dObject)
	{
	LPSTR  pictData =   (LPSTR)AObjLockData(dObject, PICTDATA0);
	long int dataVersion = (LONG)((PICTDATA *)pictData)->version;
	long rVal;

	AObjUnlockData(dObject, PICTDATA0);

	switch (dataVersion)
	  {
		case DATAVersion:
			{

      }
			rVal = A_OK;
			break;
		case 1: // if version is '1'
    
		default: // else must be an uninitialized version #
		  {
			PICTDATAv1 *pdataV1 = (PICTDATAv1 *)pictData; 
			AObjResizeData(dObject, PICTDATA0, sizeof(PICTDATA));
			PICTDATA   *pdataCur = (PICTDATA *)AObjLockData(dObject, PICTDATA0);

			pdataCur->version    = DATAVersion   ;  // version # 8/20  1
			pdataCur->pictType   = pdataV1->pictType;   //indicates whether the picture is bitmap or a metafile
			pdataCur->pictStyle  = pdataV1->pictStyle;  // Crop, fitToScreen, OrigProp etc
			pdataCur->bSelectable= pdataV1->bSelectable;
			pdataCur->bClipboard = pdataV1->bClipboard; // If the picture was pasted from Clipboard
			pdataCur->bImageSaved= pdataV1->bImageSaved; // if a picture is loaded
			pdataCur->height     = pdataV1->height;
			pdataCur->width      =  pdataV1->width;;
			if (pdataCur->version != DATAVersion)  // is not current
				pdataCur->bFrame = FALSE;
			rVal = A_OK;
			AObjUnlockData(dObject, PICTDATA0);
			break;
		 	}
			
		} // break switch statement

  return rVal;    
	}

/***************************************************************************
*	Function:		ExportObject
*	Description: This function exports the picture object
* UPSF
***************************************************************************/
extern "C" void ExportPicture(
	OBJECTID objectID)
	{
	BYTE huge	*lpImage = (BYTE huge *) AObjLockData(objectID, BMPMETADATA);
	PICTDATA	*pictData = (PICTDATA *)AObjLockData(objectID, PICTDATA0);

	if (!AUpsfCreateScope(PICTDATASCOPE, 0))
		{
		AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &pictData->version,sizeof(int) );
		AUpsfExportField("TYPE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &pictData->pictType,sizeof(BOOL) );
		AUpsfExportField("STYLE", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &pictData->pictStyle,sizeof(WORD) );
		AUpsfExportField("SELECTABLE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &pictData->bSelectable,sizeof(BOOL) );

		AUpsfExportField("BCLIPBOARD", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &pictData->bClipboard,sizeof(BOOL) );
		// added for version 2.0
		AUpsfExportField("FRAME", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &pictData->bFrame,sizeof(BOOL) );

		AUpsfCloseScope();
		}

	if (pictData->pictType == BITMAPPICT)
		{
    // Offset into the datas to point at the BITMAPINFOHEADER, followed by the
		// rgbQuadInfo and the bits
		BMP huge *curPict = (BMP FAR *)(lpImage);
		long size = AObjGetDataSize(objectID, BMPMETADATA);

		if (size != 0) {// if there is an image
			size -= sizeof(BITMAPFILEHEADER);
			AUpsfExportField("PICTURE", 0L, AUPSF_NATV_BITMAPINFO, AUPSF_UNIV_BITMAP, &curPict->bmpInfoHdr,size );
			}

		ADBG_ASSERT_E(size != 0, return; , "Error Exporting Empty Picture");
		}
	else // if META FILE Data
		{
		// Export the metafile as a metafile and as a bitmap
		// for Native Import, UPSF will give me back a stream of bytes representing the METAFILE
		// for NOn-Native Import, UPSF will give me back a BITMAP
		RECT 	rect;
		HWND hitemWnd = GetActiveWindow();
		HDC hDC;
		HANDLE hPicture = NULL;
		PICTOBJECT *pObject = (PICTOBJECT *)GetPictObject(objectID); // new PICTOBJECT((OBJECTID) objectID, NULL);
		// set the flag so as to create a new handle 3/26/93
		pObject->SetbImageSave(TRUE); // pictData.bImageSaved = TRUE;
		hDC = GetDC(hitemWnd);
		GetClientRect(hitemWnd, &rect);
		// Init the global Palette Handle
		HPALETTE hPal = CreateDIBPaletteHandle(&(pObject->bmpInfoHdr), pObject->memID);

		hPicture = pObject->CreatePictH(hDC,&rect, hPal);
    DestroyPalette(hPal);
		ReleaseDC(hitemWnd, hDC);

		if (hPicture)
    	{
			// For native export, export out the METAFILE data
			AUpsfExportField("METAFILE", 0L, AUPSF_NATV_METAFILE, AUPSF_NATV_METAFILE, lpImage, pObject->GetImageSize());
			// pass a HMETAFILE handle to the AUpsfExportField routine
			// to export a metafile as bitmap
			AUpsfExportField("PICTURE", 0L, AUPSF_NATV_METAFILE, UNIV_METAFILE, (LPVOID)&(hPicture), sizeof(HANDLE));
			pObject->DeletePHandle(hPicture);

      }
		}

  if (lpImage)
		AObjUnlockData(objectID, BMPMETADATA);

  if (pictData)
		AObjUnlockData(objectID, PICTDATA0);

	}


/***************************************************************************
*	Function:		ImportObject
*	Description: This function Imports the picture object
* UPSF
***************************************************************************/

void ImportObject(
	OBJECTID objectID)
	{
	long 			size = 0;
	AObjResizeData(objectID, BMPMETADATA, size);
	BYTE huge *lpImage = (BYTE huge *)AObjLockData(objectID, BMPMETADATA);
	PICTDATA  *pictData = (PICTDATA *)AObjLockData(objectID, PICTDATA0);
	TYPEID 		univType;

	if (!AUpsfOpenScope(PICTDATASCOPE, 0))
		{
		//	importing versions is only for cross-version compatibility.. DO NOT DIRECTLY IMPORT!!!
		// 	if (AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &pictData->version,sizeof(int) ) != AUPSF_NOERROR)
		// 	AUpsfLogComment("Defaulting to Version 0");
		//	version 2.0 change
		pictData->bFrame = TRUE;
		AUpsfImportField("TYPE", 0L, AUPSF_NATV_BOOL, &pictData->pictType,sizeof(BOOL) );
		AUpsfImportField("STYLE", 0L, AUPSF_NATV_INT,  &pictData->pictStyle,sizeof(WORD) );
		AUpsfImportField("SELECTABLE", 0L, AUPSF_NATV_BOOL,  &pictData->bSelectable,sizeof(BOOL) );
		AUpsfImportField("FRAME", 0L, AUPSF_NATV_BOOL,  &pictData->bFrame,sizeof(BOOL) );
		AUpsfImportField("BCLIPBOARD", 0L, AUPSF_NATV_BOOL, &pictData->bClipboard,sizeof(BOOL) );
		pictData->bImageSaved = TRUE;
		pictData->height = 0;
		pictData->width = 0;
		AUpsfCloseScope();
		}

	if (pictData->pictType == BITMAPPICT)
		{
		// Offset into the datas to point at the BITMAPINFOHEADER, followed by the
    // rgbQuadInfo and the bits
		BMP huge *curPict;
		BITMAPFILEHEADER bmpFileHdr;
		BITMAPINFOHEADER bInfo;
		int tableSize;

		univType = (TYPEID)AUPSF_UNIV_BITMAP;
		if(!AUpsfFieldInfo("PICTURE", 0L, AUPSF_NATV_BITMAPINFO, (LONG *)&univType, (LPLONG)&size))
			{ // only import if successful....
			if (lpImage)
				AObjUnlockData(objectID, BMPMETADATA);
			(BYTE huge *)AObjResizeData(objectID, BMPMETADATA, size + sizeof(BITMAPFILEHEADER));
			lpImage =(BYTE huge *)AObjLockData(objectID, BMPMETADATA);
			curPict  = (BMP FAR *)(lpImage);
			AUpsfImportField("PICTURE", 0L, AUPSF_NATV_BITMAPINFO, (LPSTR)&curPict->bmpInfoHdr,size );

			bmpFileHdr.bfType = MS_BMTYPE ; // atoi("BM"); SKJ Nov. 4, 1994
			bmpFileHdr.bfSize = sizeof(BITMAPFILEHEADER) + size;
			bmpFileHdr.bfReserved1 = 0;
			bmpFileHdr.bfReserved2 = 0;

			_fmemcpy((LPSTR)&bInfo, (LPSTR)&curPict->bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			tableSize = pow(2, bInfo.biBitCount) * sizeof(RGBQUAD);
			bmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize;
			_fmemcpy(&curPict->bmpFileHdr, &bmpFileHdr, sizeof(BITMAPFILEHEADER));
			}
		else
			pictData->bImageSaved = FALSE;

		}
	else // if META FILE Data
		{

		TYPEID natvType = (TYPEID)AUPSF_NATV_METAFILE;
		TYPEID univType = (TYPEID)UNIV_METAFILE;

    // First ty NATV to NATV import
		if(!AUpsfFieldInfo("METAFILE", 0L, AUPSF_NATV_METAFILE, (long *)&natvType, (LPLONG)&size))
			{
			BYTE huge *curPict;

      if (lpImage)
				AObjUnlockData(objectID, BMPMETADATA);
			AObjResizeData(objectID, BMPMETADATA, size);
			lpImage = (BYTE huge*)AObjLockData(objectID, BMPMETADATA);
			curPict = (BYTE huge*)(lpImage);
			AUpsfImportField("METAFILE", 0L, AUPSF_NATV_METAFILE, curPict,size );

			} // else try UNIV format of a metafile (which is a BITMAP format)
		else if (!AUpsfFieldInfo("PICTURE", 0L, UNIV_METAFILE, (long *)&univType, (LPLONG)&size))
			{

			// Offset into the datas to point at the BITMAPINFOHEADER, followed by the
      // rgbQuadInfo and the bits
	   	BMP huge *curPict;
	  	BITMAPFILEHEADER bmpFileHdr;
	  	BITMAPINFOHEADER bInfo;
			int tableSize;

      if (lpImage)
				AObjUnlockData(objectID, BMPMETADATA);
			AObjResizeData(objectID, BMPMETADATA, size + sizeof(BITMAPFILEHEADER) ); // SKJ Nov. 9, 1993
			lpImage = (BYTE huge*)AObjLockData(objectID, BMPMETADATA);
			curPict  = (BMP FAR *)(lpImage);

			AUpsfImportField("PICTURE", 0L, UNIV_METAFILE, (LPSTR)&curPict->bmpInfoHdr, size );
			bmpFileHdr.bfType = MS_BMTYPE; // atoi("BM"); SKJ Nov. 4, 1994
			bmpFileHdr.bfSize = sizeof(BITMAPFILEHEADER) + size;
			bmpFileHdr.bfReserved1 = 0;
			bmpFileHdr.bfReserved2 = 0;

			_fmemcpy((LPSTR)&bInfo, (LPSTR)&curPict->bmpInfoHdr, sizeof(BITMAPINFOHEADER));
			tableSize = pow(2, bInfo.biBitCount) * sizeof(RGBQUAD);
			bmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tableSize;
			_fmemcpy(&curPict->bmpFileHdr, &bmpFileHdr, sizeof(BITMAPFILEHEADER));

			pictData->pictType = BITMAPPICT;

			}
		}

	if (lpImage)
		AObjUnlockData(objectID, BMPMETADATA);

	if (pictData)
		AObjUnlockData(objectID, PICTDATA0);


	// SKJ - May 10, 1994
	// Reinit the C++ class with the newly imported data.
	// The objectID's C++ wrapper has already been created during the AOBJ_CREATE which is
  // recd. before the AOBJ_IMPORT event
	PICTOBJECT *pictObject = (PICTOBJECT *)GetPictObject(objectID);
	// Init the C++ object with the new datas
  if (pictObject)
		pictObject->InitObjFieldsFromDatas();

	}



/*********************************************************************
* PICTURE ADMN PROCEDURE
* ADMINISTRATION Routine for picture object
*
*
*********************************************************************/
long FAR PASCAL _export admnProc(
	OBJECTID 					theObject,
	pAObjMessage 			system)

	{
	OBJECTID 					sObject, dObject;
	AEvtInfo 					tEvent;
	char 							errorString[255];
	MEMBLOCKID 				sigsBlockID;
	long rVal 				= A_OK;

	pAObjSignalRecord		theSigs;
	errorString[0] = '\0';


	switch (system->message1)
		{
		case  AOBJ_CREATED:
			{
		 	PICTDATA FAR *lpPictData;
			dObject = (OBJECTID)theObject;
			AObjSetDataCount(dObject, OBJECTINSTDATA);
			AObjResizeData(dObject, PICTDATA0, sizeof(PICTDATA));

			lpPictData = (PICTDATA FAR *)AObjLockData(dObject, PICTDATA0);
			lpPictData->version = DATAVersion;
			lpPictData->pictType = BITMAPPICT;    // indicates whether the picture is bitmap or a metafile
     	lpPictData->pictStyle = CROPID;  // Crop, fitToScreen, OrigProp etc
			lpPictData->bSelectable = FALSE;
			lpPictData->bClipboard = FALSE; // If the picture was pasted from Clipboard
     	lpPictData->height = 0;
     	lpPictData->width = 0;
			// added for maintenance release 3/23/93
			lpPictData->bFrame = TRUE;
			AObjUnlockData(dObject, PICTDATA0);

			AObjResizeData(dObject, BMPMETADATA, 0); // data1
			AObjResizeData(dObject, 2, sizeof(long));

			// May 20, 1993
			// to properly handle creation of C++ obj instance for temporary
			// objects that are created during runtime but do not go thro' the
			// normal AEVENT_STARTUP msg handling
			// need to init the PICT_OBJECT ptr;
			if (system->message4 == AOBJ_TEMPORARY)
				{
				InitPictObject(dObject);
				}
			}
		case AOBJ_AWAKENED:
			{
			PICTOBJECT *pObject;
			pObject = new PICTOBJECT((OBJECTID)theObject, NULL);
			AObjSetRuntimeInfo(theObject, (LONG)pObject);
			}
			if (system->message1 == AOBJ_AWAKENED)
        rVal = A_CONTINUE;
			break;
		case AOBJ_ASLEEP:
		case AOBJ_DESTROYED:
			{
			if (theObject > 0) // not a temp object
				{
				PICTOBJECT *pObject = (PICTOBJECT *) AObjGetRuntimeInfo(theObject);
				delete pObject;
				}
			if (system->message1 == AOBJ_ASLEEP)
				rVal = A_CONTINUE;
			}
      break;
    // added for data and dll version check 4/13/93
		case AOBJ_READ:
			{
			if (UpdateObject(theObject) == A_OK)
				{
				// system->message4 = A_OK;
				rVal = A_OK;
				system->message3 = (LONG)DATAVersion; // return current data Version
				}
			else
				{
				// system->message4 = versionError;
				rVal =  A_ERROR;
				system->message3 = (LONG)DATAVersion; // return current data Version
				AObjReportError		(theObject,
				 									 AOBJ_READ,
													 A_ERROR,
													 "Incompatible Version",
													 0);
				}
			}
			break;

	 	case AOBJ_ASSIGNOBJECT:
		 	{
			int i;
			PICTOBJECT *destObject = NULL;
			PICTOBJECT *sourceObject = NULL;
			dObject = (OBJECTID)(theObject);    // destination
			sObject = (OBJECTID)(system->message3);    // source
			if (sObject == dObject)
				break;

			if (UpdateObject(sObject) != A_OK)
				{
				lstrcpy(system->errorMessage, "AOBJ_ASSIGNOBJECT failed : PICTURE/tSER_PICT");
				return A_ERROR;
				}
      if (sObject > 0) {
				sourceObject = (PICTOBJECT *)GetPictObject(sObject); // AObjLockData(sObject, STOREDPICTOBJ);;
				// Save the runtime data back into the datas before assigning
				sourceObject->SaveRuntimeData();

				}
			// destination object needs to deleted and recreated on the next
			// entry to MDRT
      if (dObject > 0) {
				destObject =  (PICTOBJECT *)GetPictObject(dObject); //AObjLockData(dObject, STOREDPICTOBJ);
      	}
			for (i = 0; i < OBJECTINSTDATA; i++)
				{
				LPSTR lpSrc = (LPSTR)AObjLockData(sObject, i);
				long size = AObjGetDataSize(sObject, i);
				AObjResizeData(dObject, i, size);
				LPSTR lpDst =	(LPSTR)AObjLockData(dObject, i);
				hmemcpy(lpDst, lpSrc, size);
        if (lpSrc)
					AObjUnlockData(sObject, i);
        if (lpDst)
					AObjUnlockData(dObject, i);
				}
			if (destObject != NULL)
				{
				// Init the C++ object with the new datas
				destObject->InitObjFieldsFromDatas();
        }
			tEvent.sourceObject = dObject;
		 	tEvent.targetObject = dObject;
			tEvent.what = AEVENT_OBJECTCHANGED;
     	tEvent.when = 0; //Tickcount;
			tEvent.how = AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
			tEvent.details = RECREATEHANDLE;
			tEvent.modifiers =  RECREATEHANDLE;  // recreate all picture handles

     	//tEvent.private= 0;

     	AEvtPost(&tEvent);
     	}
		 	break;

		case AOBJ_IMPORTOBJECT:
    	{
			ImportObject(theObject);
      }
			break;

		case AOBJ_EXPORTOBJECT:
    	{
			ExportPicture(theObject);
      }
			break;

		// the following events have been defined for the APPWARE environment
		// Aug. 25, 1993
		case AOBJ_QUERYPROTOCOL:
    	{
			ApQueryInfo *pQueryInfo = (ApQueryInfo *) system->message4;

			if (pQueryInfo->protocol == APROT_WND)  // if the window protocol is supported
				{
				// system->message3 = APROT_STATIC;
				pQueryInfo->pfnCreate = (APPCFN) vPictureCreate; // create function for
				rVal = AP_SUPPORTED;
				}
			break;
			}
		case AOBJ_CHECKOBJECT:{
			// AObjInfo *objInfoRec = (AObjInfo *)system->message2;
			// objInfoRec->errorCode = objectHealthy;
			rVal = A_OK;
			// I_UNUSED(objInfoRec);
			}
			break;

	
		case AOBJ_EDITOBJECT:{
			HWND 			hWnd = GetActiveWindow();
			int i = DialogBoxParam(hInst, "PICTDLG", hWnd, (DLGPROC)PictureDlgProc, (LONG)theObject);
			if (i == -1)
				rVal= A_ERROR;
			else if (i == 1)
				rVal = A_MODIFIED;
			else rVal = A_OK;
			}
			break;

		case AOBJ_BUILDSIGNALS:{
			PICTDATA FAR 	*lpData;
			sigsBlockID = (MEMBLOCKID)system->message2;

			lpData = (PICTDATA FAR*)AObjLockData(theObject, PICTDATA0);
			if (!lpData->bSelectable)
				{
				AMemSetSize(sigsBlockID, 3 * sizeof(AObjSignalRecord));
				theSigs = (pAObjSignalRecord)(LPSTR)AMemLock(sigsBlockID);
				theSigs[0].theID = PICTCHANGEDSIGNAL;
				lstrcpy(theSigs[0].theName, "Changed");
				theSigs[1].theID = PICTCLICKEDSIGNAL;
				lstrcpy(theSigs[1].theName, "Clicked");
				theSigs[2].theID = PICTDBLCLICKEDSIGNAL;
				lstrcpy(theSigs[2].theName, "Double Clicked");

				AMemUnlock(sigsBlockID);

				}
			else
				{
				AMemSetSize(sigsBlockID, 5 * sizeof(AObjSignalRecord));
				theSigs = (pAObjSignalRecord)(LPSTR)AMemLock(sigsBlockID);

				theSigs[0].theID = PICTCHANGEDSIGNAL;
				lstrcpy(theSigs[0].theName, "Changed");
				theSigs[1].theID = PICTCLICKEDSIGNAL;
				lstrcpy(theSigs[1].theName, "Clicked");
				theSigs[2].theID = PICTACTIVATEDSIGNAL;
				lstrcpy(theSigs[2].theName, "Activated");
				theSigs[3].theID = PICTDEACTIVATEDSIGNAL;
				lstrcpy(theSigs[3].theName, "Deactivated");
				theSigs[4].theID = PICTDBLCLICKEDSIGNAL;
				lstrcpy(theSigs[4].theName, "Double Clicked");

				AMemUnlock(sigsBlockID);

				}
			AObjUnlockData(theObject, PICTDATA0);
			}
			break;

		case AOBJ_VALIDATEOBJECTIDS:
				break;


		default: break;
		}
	return rVal;
	}


} // end....extern "C"







// this function the dllVersion of this Object. It is
// used by the shell to check for incompatibilty
long _export GetSerVersion(
	TYPEID objectType)
	{


	if (objectType == (TYPEID)OTYPE_PICTURE)
		return DLLVersion;
	else
		return -1L;


  }


/**************************************************************************
* Function: vPictureCreate
* Description: A pointer to this function is passed back to the Window	Object
*							 The Window Object calls this for every v-instance of the picture
*							 item.  The constructor to PICT_VINST is called to create and init
*							 a visual instance of Picture's c++ class.  The methods in this
*							 class provide the implentation for the Window Item Protocol
*
**************************************************************************/
OpWndItemD* ALMAPI vPictureCreate(
	OBJECTID		oiItem,
	OBJECTID		oiParent,
	RECT*				rcItem,
	long				status,
	long				special
	)
	{

	I_UNUSED(special);
	return new PICTURE_VINST(oiItem, oiParent, rcItem, status);
	}

// ****************************************************************
//
// Overridden operator new and delete so we don't eat up selectors
// with small C++ objects.
//
// ****************************************************************

void*   operator new(
  size_t    s
  )
  {
  return MemAlloc(s);
  }

void    operator delete(
  void*     p
  )
  {
  if (p)
    MemFree(p);
  }

