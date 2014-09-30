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
//	"SCRP_ADMIN.cpp"
//
// ALM Administration routine for the Script ALM.
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

#include "scrp_bas.h"


/*******************************************************************
 *	Local function prototypes
 ******************************************************************/

LONG	scrp_doAssign(OBJECTID	dstObject,OBJECTID	srcObject);
LONG	scrp_doVersionCheck(OBJECTID theObject,long& version, long& status, char* error);
LONG	scrp_editObject(OBJECTID theObject);
BOOL	InitDialog(HWND, long) ;
LONG	ExportScrpObject(OBJECTID theObject);
LONG	ImportScrpObject(OBJECTID theObject);


/*******************************************************************
 *	External function prototypes, and vars.
 ******************************************************************/

extern LONG Compile_Script(OBJECTID);
extern HINSTANCE	hInstance;


/*******************************************************************
 *	SCRP_ADMIN: Administration routine for the scrp ALM.
 ******************************************************************/

extern "C" LONG far pascal _export SCRP_ADMIN(
	OBJECTID	 		theObject,
	pAObjMessage	theSystem)
{
	long				rVal = A_NOTHANDLED;
	SCRP_OBJECT*	theScrpObj;

	switch(theSystem->message1) {
		case AOBJ_AWAKENED:
			AObjSetRuntimeInfo(theObject, (long) new SCRP_OBJECT(theObject));
			rVal = A_CONTINUE;
			break;

		case AOBJ_CREATED:		// create object and initialize it.
			AObjSetRuntimeInfo(theObject, (long) new SCRP_OBJECT(theObject));
			rVal = A_OK;
			break;

		case AOBJ_ASLEEP:
			break;

		case AOBJ_DESTROYED:
			break;

		case AOBJ_ASSIGNOBJECT:			// assign the scrp name and function.
			rVal = scrp_doAssign(theObject, (OBJECTID)theSystem->message3);
			break;

		case AOBJ_IMPORTOBJECT:
			rVal = ImportScrpObject(theObject);
			break;

		case AOBJ_EXPORTOBJECT:
			rVal = ExportScrpObject(theObject);
			break;

		case AOBJ_READ:
			rVal = scrp_doVersionCheck(theObject, theSystem->message3, theSystem->message4, theSystem->errorMessage);
			break;

		case AOBJ_QUERYPROTOCOL:
			break;

		case AOBJ_EDITOBJECT:		// display the scrp dialog.
			theScrpObj = (SCRP_OBJECT*)AObjGetRuntimeInfo(theObject);
			I_ASSERT(theScrpObj);
			theScrpObj->CheckIDs(theObject);
			rVal = scrp_editObject(theObject);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			theScrpObj = (SCRP_OBJECT*)AObjGetRuntimeInfo(theObject);
			I_ASSERT(theScrpObj);
			theScrpObj->Validate(theSystem, theObject);
			rVal = A_OK;
			break;

		case AOBJ_CHECKOBJECT:
			rVal = Compile_Script(theObject);	// go compile all the scripta.
			if (rVal == A_ERROR) {
				AObjReportError(theObject, AOBJ_CHECKOBJECT, rVal,
					"An error took place during compilation of the script.",0);
			}
			break;

		case AOBJ_BUILDSIGNALS:
			break;

		case AOBJ_PREWRITE:
			break;

		case AOBJ_POSTWRITE:
			break;

		case AOBJ_REPORTREQUIREDFILES:			// compile the scripts
//			rVal = Compile_Scripts();	// go compile all the scripta.
//			Compile_Script(theObject);	// go compile all the scripta.
			break;

		default:
//			MessageBox(NULL, "Unknown Message in scrp Admin", "Error", MB_OK);
			break;
	}
	return rVal;
}


/*******************************************************************
 *	scrp_doAssign: assign script ALM to another ALM.
 ******************************************************************/

LONG scrp_doAssign(
	OBJECTID	dstObject,
	OBJECTID	srcObject)
{
	AEvtInfo  event;
  LONG			retValue = A_OK;
	SCRP_OBJECT*	scrpObj;

	scrpObj = (SCRP_OBJECT*)AObjGetRuntimeInfo((OBJECTID)srcObject);
	scrpObj->scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(srcObject, 0L);
	if(scrpObj->scrpFuncName && (scrpObj->Version() <= VERSION_OBJECT)) {
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
 *	scrp_doVersionCheck:
 ******************************************************************/

LONG scrp_doVersionCheck(
	OBJECTID	theObject,
	long&			version,
	long&			status,
	char*			error)
{
	I_UNUSED(theObject);
	I_UNUSED(version);
	I_UNUSED(status);
	I_UNUSED(error);
//	SCRP_OBJECT*	scrpObj;
	LONG	retVal = A_OK;
/*
	scrpObj = (SCRP_OBJECT*)AObjGetRuntimeInfo((OBJECTID)theObject);
	scrpObj->scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(theObject, 0L);
	if(scrpObj->scrpFuncName && scrpObj->Version()<=VERSION_OBJECT) {
		version = VERSION_OBJECT;
		status = A_OK;
//    return (A_OK);
	}
	else {
		version = scrpObj->Version();
		status = A_ERROR;
		AObjReportError(theObject, AOBJ_READ, status, "script ALM\talm_bas.dll", version);
    retVal = A_ERROR;
	}
	AObjUnlockData(theObject, 0L);
*/
	return (retVal);
}			// scrp_doVersionCheck(...


/*******************************************************************
 *	scrp_editObject:
 ******************************************************************/

LONG scrp_editObject(
	OBJECTID	theObject)
{
	LONG				theResult;

	if (DialogBoxParam(hInstance, "ARGUMENTS", GetActiveWindow(),
							(DLGPROC)EditArgumentDialogProc, (long)theObject))


//	if(DialogBoxParam(hInstance, "scrp", GetActiveWindow(),
//			(DLGPROC)EditObjectDialogProc, (long)theObject))
		theResult = A_MODIFIED;
	else
		theResult = A_OK;
	return theResult;
}			// scrp_editObject(...


/*******************************************************************
 *	ExportScrpObject: UPSF, export to other platform.
 ******************************************************************/

LONG	ExportScrpObject(
	OBJECTID 			theObject)
{
	I_UNUSED(theObject);

	return(A_OK);
}


/*******************************************************************
 *	ImportScrpObject: UPSF, import to other platform.
 ******************************************************************/

LONG	ImportScrpObject(
	OBJECTID 			theObject)
{
	I_UNUSED(theObject);

	return(A_OK);
}

