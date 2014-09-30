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
//	"scrp_def.cpp"
//
// ALM Administration routine for the scrp ALM.
//
////////////////////////////////////////////////////////////////////////////

#define ASTRICT
#define STRICT

#include <windows.h>
#include <stdio.h>
#include <mem.h>
#include <string.h>
#include <bwcc.h>
#include <helpids.h>

#include	<a_alm.h>
#include	<a_almutl.h>
#include	<op_wnd.h>
#include	<o_wnd.h>

#include "scrp_bas.h"


////////////////////////////////////////////////////////////////////////////

SCRP_OBJECT::SCRP_OBJECT(
	OBJECTID	oiScrp)
{
	long					numOfDataBlks;
	SCRP_ARGUMENTS*		retArg;

	theObject = oiScrp;
	numOfDataBlks = AObjGetDataCount(theObject);
	if( numOfDataBlks == 0L)
	{
		AObjSetDataCount(theObject, MIN_DATABLKS);	// info block and return parameter.
		if (AObjResizeData(theObject, SCRP_INFO_DBINDEX, (long)sizeof(SCRP_FUNCNAME)))
		{			// init the scrp name and the scrp function name.
			scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(theObject, SCRP_INFO_DBINDEX);
			scrpFuncName->version = 1;
//			scrpFuncName->numOfArguments = 0;
			scrpFuncName->numOfErrorsInLog = 0;
			scrpFuncName->debuggEnable = FALSE;
			scrpFuncName->globalScript = FALSE;
			scrpFuncName->function = TRUE	;
			scrpFuncName->structure = FALSE;
			scrpFuncName->variables = FALSE;
			strcpy(&scrpFuncName->scrpFunction[0], "<Undefined>");			// no ALM name...
		}

		if (AObjResizeData(theObject, SCRP_PARAM_DBINDEX, (long)sizeof(SCRP_ARGUMENTS)))
		{			// init the return value argument.
			retArg = (SCRP_ARGUMENTS*)AObjLockData(theObject, SCRP_PARAM_DBINDEX);
			retArg->constant = FALSE;
			retArg->theArgObject = 0L;
			strcpy(&retArg->theArgObjectName[0], "<Undefined>");			// no ALM name...
			retArg->paramType = -1;
			retArg->constVal = 0;
			retArg->byValue = FALSE;
			AObjUnlockData(theObject, SCRP_PARAM_DBINDEX);
		}

	}				// if( numOfDataBlks == 0L)...
}


////////////////////////////////////////////////////////////////////////////

SCRP_OBJECT::~SCRP_OBJECT()
{

}


////////////////////////////////////////////////////////////////////////////
// save the scrp name and function.
///////////////////////////////////////////////////////////////////////////
/*
void	SCRP_OBJECT::SetScrpName(
	char*	cmdline,
	HWND	hDlg)

{
	int len = strlen(cmdline);
	char	temp[SCRPNAMESIZE];
	long	index;

	(char*)AObjResizeData(theObject, 0L, len + sizeof(SCRP_FUNCNAME));
	(char*)AObjSetData(theObject, 0L, (void*)cmdline, len+1);

	index = SendDlgItemMessage(hDlg, IDD_SCRPFUNCNAME, CB_GETCURSEL, 0, 0);
	if (index != CB_ERR){
		SendDlgItemMessage(hDlg, IDD_SCRPFUNCNAME, CB_GETLBTEXT, index, (LONG) &temp);
		len = strlen(temp);
//		(char*)AObjResizeData(theObject, 1L, len);
//		(char*)AObjSetData(theObject, 1L, (void*)&temp, len+1);
	}
}
*/
////////////////////////////////////////////////////////////////////////////
// set the scrp name in the dialog.
////////////////////////////////////////////////////
/*
char*	SCRP_OBJECT::GetScrpName(
	HWND	hDlg)

{
//	SCRP_FUNCNAME*	theScrpName;
//	char	szBuffer[SCRPNAMESIZE];

	scrpNamePtr = (char*)AObjLockData(theObject, 0L);
	SetDlgItemText(hDlg, IDD_SCRPNAME, (char*)scrpNamePtr);		// set name in dialog.

	openScrpFile(hDlg);		// open the scrp and setup the scrp function list.
//	scrpFunction = (char*)AObjLockData(theObject, 1L);
	SendDlgItemMessage(hDlg, IDD_SCRPFUNCNAME, CB_SELECTSTRING, -1, (LONG) scrpFunctionPtr);

}
*/





////////////////////////////////////////////////////////////////////////////
//	CheckIDs:	Will check all of the object types for the function arguments
//				the function result object.
////////////////////////////////////////////////////////////////////////////

void	SCRP_OBJECT::CheckIDs(
	OBJECTID	oiScrp)
{
	SCRP_ARGUMENTS*	dataBlockPtr;
	long						numArgs;
	int							i;

	theObject = oiScrp;
	numArgs = AObjGetDataCount(theObject);
	for (i=SCRP_PARAM_DBINDEX; i<numArgs; ++i) {		// copy data blocks to temp area.
		dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObject, i);
		if (dataBlockPtr->theArgObject != 0L)
			if (!AObjCheckType(dataBlockPtr->theArgObject, 0L)) {
				dataBlockPtr->theArgObject = 0L;			// object was deleted.
				strcpy(&dataBlockPtr->theArgObjectName[0], "<Undefined>");			// no ALM name...
			}			// if (!AObjCheckType(dataBlockPtr->theArgObject, 0L...
		AObjUnlockData(theObject,i);
	}			// for (i=1; i<numArgs; ++i)...
}


////////////////////////////////////////////////////////////////////////////

void	SCRP_OBJECT::Validate(
	pAObjMessage theSystem,
	OBJECTID	oiScrp)
{
	SCRP_ARGUMENTS*	dataBlockPtr;
	long						numArgs;
	int							i;

	theObject = oiScrp;
	numArgs = AObjGetDataCount(theObject);
	for (i=SCRP_PARAM_DBINDEX; i<numArgs; ++i) {		// copy data blocks to temp area.
		dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObject, i);
		if (dataBlockPtr->theArgObject != 0L)
			AObjValidateID(&(dataBlockPtr->theArgObject), theSystem);
		AObjUnlockData(theObject,i);
	}			// for (i=1; i<numArgs; ++i)...
}
