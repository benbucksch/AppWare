///////////////////////////////////////////////////////////////
//
//	(c) 1993, 1994 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
//	"dll_adm.cpp"
//
// ALM Administration routine for the DLL ALM.
//
////////////////////////////////////////////////////////////////////////////

#define ASTRICT
#define STRICT

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

#include	<a_alm.h>
#include	<a_almutl.h>
#include	<op_wnd.h>
#include	<o_wnd.h>
#include	<intdebug.h>

#include "dll.h"


/*******************************************************************
 *	Local function prototypes
 ******************************************************************/

LONG	DLL_doAssign(AOBJECTID	dstObject,AOBJECTID	srcObject);
LONG	DLL_doVersionCheck(AOBJECTID theObject,long& version, long& status, char* error);
LONG	DLL_editObject(AOBJECTID theObject);
BOOL	InitDialog(HWND, long) ;
LONG	ExportDllObject(AOBJECTID theObject);
LONG	ImportDllObject(AOBJECTID theObject);

extern HINSTANCE	hInstance;


/*******************************************************************
 *	DLL_Admin: Administration routine for the DLL ALM.
 ******************************************************************/

extern "C" LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/  DLL_Admin(
	AOBJECTID	 		theObject,
	pAObjMessage	theSystem)
{
	long				rVal = A_NOTHANDLED;
	DLL_OBJECT*	theDLLObj;

	switch(theSystem->message1) {
		case AOBJ_AWAKENED:
			AObjSetRuntimeInfo(theObject, (long) new DLL_OBJECT(theObject));
			rVal = A_CONTINUE;
			break;

		case AOBJ_CREATED:		// create object and initialize it.
			AObjSetRuntimeInfo(theObject, (long) new DLL_OBJECT(theObject));
			rVal = A_OK;
			break;

		case AOBJ_ASLEEP:
			break;

		case AOBJ_DESTROYED:
			break;

		case AOBJ_ASSIGNOBJECT:			// assign the dll name and function.
			rVal = DLL_doAssign(theObject, (AOBJECTID)theSystem->message3);
			break;

		case AOBJ_IMPORTOBJECT:
			rVal = ImportDllObject(theObject);
			break;

		case AOBJ_EXPORTOBJECT:
			rVal = ExportDllObject(theObject);
			break;

		case AOBJ_READ:
			rVal = DLL_doVersionCheck(theObject, theSystem->message3, theSystem->message4, theSystem->errorMessage);    
			break;

		case AOBJ_QUERYPROTOCOL:
			break;

		case AOBJ_EDITOBJECT:		// display the dll dialog.
			theDLLObj = (DLL_OBJECT*)AObjGetRuntimeInfo(theObject);
			I_ASSERT(theDLLObj);
			theDLLObj->CheckIDs(theObject);
			rVal = DLL_editObject(theObject);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			theDLLObj = (DLL_OBJECT*)AObjGetRuntimeInfo(theObject);
			I_ASSERT(theDLLObj);
			theDLLObj->Validate(theSystem, theObject);
			rVal = A_OK;
			break;

		case AOBJ_CHECKOBJECT:
			break;

		case AOBJ_BUILDSIGNALS:
			break;

		case AOBJ_PREWRITE:
			break;

		case AOBJ_POSTWRITE:
			break;

		default:
//			MessageBox(NULL, "Unknown Message in DLL Admin", "Error", MB_OK);
			break;
	}
	return rVal;
}


/*******************************************************************
 *	DLL_doAssign: assign DLL ALM to another ALM.
 ******************************************************************/

LONG DLL_doAssign(
	AOBJECTID	dstObject,
	AOBJECTID	srcObject)
{
	AEvtInfo  event;
  LONG			retValue = A_OK;
	DLL_OBJECT*	DLLObj;

	DLLObj = (DLL_OBJECT*)AObjGetRuntimeInfo((AOBJECTID)srcObject);
	DLLObj->DllFuncName = (DLL_FUNCNAME*)AObjLockData(srcObject, 0L);
	if(DLLObj->DllFuncName && (DLLObj->Version() <= VERSION_OBJECT)) {
		//the only version to worry about is 0...
		if(AObjAssign(srcObject, dstObject)) {
			event.sourceObject = event.targetObject = srcObject;
			event.when         = AAppGetTime();
			event.how          = AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
			event.what         = AEVENT_OBJECTCHANGED;
			AEvtPost(&event);
		}
		else
		retValue = A_ERROR;
	}
	else
		retValue = A_ERROR;
	AObjUnlockData(srcObject, 0L);
	return (retValue);
}			// Sbar_doAssign(...


/*******************************************************************
 *	DLL_doVersionCheck:
 ******************************************************************/

LONG DLL_doVersionCheck(
	AOBJECTID	theObject,
	long&			version,
	long&			status,
	char*			error)
{
	I_UNUSED(theObject);
	I_UNUSED(version);
	I_UNUSED(status);
	I_UNUSED(error);
//	DLL_OBJECT*	DLLObj;
	LONG	retVal = A_OK;
/*
	DLLObj = (DLL_OBJECT*)AObjGetRuntimeInfo((AOBJECTID)theObject);
	DLLObj->DllFuncName = (DLL_FUNCNAME*)AObjLockData(theObject, 0L);
	if(DLLObj->DllFuncName && DLLObj->Version()<=VERSION_OBJECT) {
		version = VERSION_OBJECT;
		status = A_OK;
//    return (A_OK);
	}
	else {
		version = DLLObj->Version();
		status = A_ERROR;
		AObjReportError(theObject, AOBJ_READ, status, "DLL ALM\talm_dll.dll", version);
    retVal = A_ERROR;
	}
	AObjUnlockData(theObject, 0L);
*/
	return (retVal);
}			// DLL_doVersionCheck(...


/*******************************************************************
 *	DLL_editObject:
 ******************************************************************/

LONG DLL_editObject(
	AOBJECTID	theObject)
{
	LONG				theResult;

	if(DialogBoxParam(hInstance, "DLL", GetActiveWindow(),
			(DLGPROC)EditObjectDialogProc, (long)theObject))
		theResult = A_MODIFIED;
	else
		theResult = A_OK;
	return theResult;
}			// DLL_editObject(...


/*******************************************************************
 *	ExportDllObject: UPSF, export to other platform.
 ******************************************************************/

LONG	ExportDllObject(
	AOBJECTID 			theObject)
{
	I_UNUSED(theObject);

	return(A_OK);
}


/*******************************************************************
 *	ImportDllObject: UPSF, import to other platform.
 ******************************************************************/

LONG	ImportDllObject(
	AOBJECTID 			theObject)
{
	I_UNUSED(theObject);

	return(A_OK);
}

