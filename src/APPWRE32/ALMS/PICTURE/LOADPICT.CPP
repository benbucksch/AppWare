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
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT

#include "a_alm.h"
#include "windows.h"
#include "picture.h"
#include "string.h"
#include "stdlib.h"
#include "o_wnd.h"
#include "op_wnd.h"
#include "o_text.h"



extern LPLONG GetPictObject(AOBJECTID objID);
// function used to call the commdlg FILE-OPEN dlg
// to get the name of bmp/meta file to load into the
// picture object

/* extern "C" int InitializeFOpen( PICTOBJECT *pO,
	HWND ghWnd); */



/**********************************************************************
* LoadPicture
* Load a new image into the Picture
*
*
***********************************************************************/
extern "C" void WINAPI loadPict(
	pAEvtInfo							theEvent,
	pAObjMessage 					system)
	{
	AOBJECTID 						pObject, dObject;
	LPSTR 							lpext;
	AEvtInfo 						tEvent;
	PICTOBJECT 					*pO;
	char 								fName[MAXNAME];
	int 								rVal;

	pObject = AFuncGetParameter(1); // the picture object to load New picture into

	dObject = NULLOBJECTID;
	system->message1 = 1;

	if (AObjCheckType( pObject, (ATYPEID)OTYPE_PICTURE))
		{
		// skj, 3-11-93 init from the datas
		pO =  (PICTOBJECT *)GetPictObject(pObject);
		// if a filename is supplied as input then load that file into the
		// pict object else open up a file-open dlg (W3.1 - commdlg)

		if ((dObject = AFuncGetTypedParameter(2, OTYPE_TEXT)) != 0L)
			{
			OTextGetToBuffer(dObject, fName, MAXNAME);
	
			// tPtr = (LPTEXTOBJECT)AObjLockData(dObject, 1);
			// CopyMemory(fName, tPtr,MAXNAME); //  AObjGetDataSize(dObject, 1));
			
			// AObjUnlockData(dObject, 1);

			}
		else
			{
			fName[0] = '\0';
			}

		// check the extension of the file chosen
		// if ext = .bmp then pictType = BITMAPPICT
		// if ext = .wmf then pictType = METAPICT
		lpext = strchr(fName, '.');
		if ((lpext == NULL) ||
			((lstrcmpi(lpext, ".bmp") != 0) &&  (lstrcmpi(lpext, ".wmf") != 0)))
		  {
			// Oct. 15, 1993 SKJ do not use NULL,
			// for task-modal dialog box
			rVal = pO->InitializeFOpen(GetActiveWindow());
      }
		else
			{
			if ((lstrcmpi(lpext, ".bmp") == 0))
				pO->SetPictType(BITMAPPICT);
			else
				pO->SetPictType(METAPICT);

			if (pO->GetPictType() == BITMAPPICT)
				{
				HANDLE 	FILE_HANDLE = CreateFile(fName, GENERIC_READ, 0, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

				if (FILE_HANDLE != INVALID_HANDLE_VALUE) 
					{
					rVal = FALSE;
					lstrcpy(system->errorMessage, "Wrong File Name. Picture Not Loaded.");
          }
				else 
					{
					
					// Save the BMP into the Object Datas
					pO->SaveBMP(SAVE_FROM_FILE, FILE_HANDLE);

		
					CloseHandle(FILE_HANDLE);
					rVal = TRUE;
					}
				}
			else
				{
				// Save the MetaFile into the Object datas

				pO->SaveMetaData(SAVE_FROM_FILE, fName );

		
				pO->SetbSave(TRUE);
				rVal = TRUE;
				}
			}

		if (rVal <= 0)
			{
			// lstrcpy(system->errorMessage, "Picture Not Loaded.");
			system->message1 = 2;
			}
		else
			{
		
			long int lval = 0L;
		
			pO->SetbImageSave(TRUE);  // ImageSaved
			pO->SetbClipboard(FALSE);

			pO->SetbSave(TRUE);

			tEvent.sourceObject = pObject;
		  tEvent.targetObject = pObject;
	   	tEvent.what = AEVENT_OBJECTCHANGED;
		  tEvent.when = 0; //Tickcount;
			tEvent.how = AEvtGetObjectChangedPriority(AEVT_ATTAIL | AEVT_KILLDUPLICATES);
			tEvent.details = RECREATEHANDLE;
			tEvent.modifiers =  RECREATEHANDLE;  // recreate all picture handles
			AEvtPost(&tEvent);

			// Save the runtime data back into the datas before assigning
			pO->SaveRuntimeData();

			I_UNUSED(lval);
			}
		}
	else
		{
		lstrcpy(system->errorMessage, "Error while loading picture.");
		system->message1 = 2;
		}
	I_UNUSED(theEvent);
	}

/**********************************************************************
* ClearPicture
* Clear the picture datas
* June 20, 1994
*
***********************************************************************/
extern "C" void WINAPI  ClearPict(
	pAEvtInfo							theEvent,
	pAObjMessage 					system)
	{
	AOBJECTID 						pObject;
	AEvtInfo 						tEvent;
	PICTOBJECT 					*pO;
	int 								rVal = 0;

	pObject = AFuncGetParameter(1); // the picture object to load New picture into
	system->message1 = 1;

	if (AObjCheckType( pObject, (ATYPEID)OTYPE_PICTURE))
		{
		// skj, 3-11-93 init from the datas
		pO =  (PICTOBJECT *)GetPictObject(pObject);
		if (pO)
			{
			pO->SetImageSize(0);
			rVal = TRUE;
			}

		if (rVal <= 0)
			{
			system->message1 = 2;
			}
		else
			{
			long int lval = 0L;
			pO->SetbImageSave(TRUE);  // ImageSaved
			pO->SetbClipboard(FALSE);
			pO->SetbSave(TRUE);
			tEvent.sourceObject = pObject;
		  tEvent.targetObject = pObject;
	   	tEvent.what = AEVENT_OBJECTCHANGED;
		  tEvent.when = 0; //Tickcount;
			tEvent.how = AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
			tEvent.details = RECREATEHANDLE;
			tEvent.modifiers =  RECREATEHANDLE;  // recreate all picture handles
			AEvtPost(&tEvent);
			// Save the runtime data back into the datas before assigning
			pO->SaveRuntimeData();
			I_UNUSED(lval);
			}
		}
	else
		{
		lstrcpy(system->errorMessage, "Error while Clearing picture.");
		system->message1 = 2;
		}
	I_UNUSED(theEvent);
	}
