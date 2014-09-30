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
//	"dll_def.cpp"
//
// ALM Administration routine for the DLL ALM.
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

#include "dll.h"


////////////////////////////////////////////////////////////////////////////

DLL_OBJECT::DLL_OBJECT(
	AOBJECTID	oiDLL)
{
	long					numOfDataBlks;
	DLL_RETARG*		retArg;

	theObject = oiDLL;
	numOfDataBlks = AObjGetDataCount(theObject);
	if( numOfDataBlks == 0L)
	{
		AObjSetDataCount(theObject, 2);
		if (AObjResizeData(theObject, 0, (long)sizeof(DLL_FUNCNAME)))
		{			// init the DLL name and the DLL function name.
			DllFuncName = (DLL_FUNCNAME*)AObjLockData(theObject, 0L);
			lstrcpy(DllFuncName->DLLName, "");
			lstrcpy(DllFuncName->DLLFunction, "");
			AObjUnlockData(theObject, 0L);
		}

		if (AObjResizeData(theObject, 1, (long)sizeof(DLL_RETARG)))
		{			// init the return value argument.
			retArg = (DLL_RETARG*)AObjLockData(theObject, 1L);
			retArg->theObject = 0L;
			retArg->paramType = -1;
			AObjUnlockData(theObject, 1L);
		}

	}				// if( numOfDataBlks == 0L)...
}


////////////////////////////////////////////////////////////////////////////

DLL_OBJECT::~DLL_OBJECT()
{

}


////////////////////////////////////////////////////////////////////////////
// save the dll name and function.
///////////////////////////////////////////////////////////////////////////
/*
void	DLL_OBJECT::SetDLLName(
	char*	cmdline,
	HWND	hDlg)

{
	int len = lstrlen(cmdline);
	char	temp[DLLNAMESIZE];
	long	index;

	(char*)AObjResizeData(theObject, 0L, len + sizeof(DLL_FUNCNAME));
	(char*)AObjSetData(theObject, 0L, (void*)cmdline, len+1);

	index = SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_GETCURSEL, 0, 0);
	if (index != CB_ERR){
		SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_GETLBTEXT, index, (LONG) &temp);
		len = lstrlen(temp);
//		(char*)AObjResizeData(theObject, 1L, len);
//		(char*)AObjSetData(theObject, 1L, (void*)&temp, len+1);
	}
}
*/
////////////////////////////////////////////////////////////////////////////
// set the DLL name in the dialog.
////////////////////////////////////////////////////
/*
char*	DLL_OBJECT::GetDLLName(
	HWND	hDlg)

{
//	DLL_FUNCNAME*	theDLLName;
//	char	szBuffer[DLLNAMESIZE];

	DLLNamePtr = (char*)AObjLockData(theObject, 0L);
	SetDlgItemText(hDlg, IDD_DLLNAME, (char*)DLLNamePtr);		// set name in dialog.

	openDLLFile(hDlg);		// open the DLL and setup the dll function list.
//	DLLFunction = (char*)AObjLockData(theObject, 1L);
	SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_SELECTSTRING, -1, (LONG) DLLFunctionPtr);

}
*/





////////////////////////////////////////////////////////////////////////////
//	CheckIDs:	Will check all of the object types for the function arguments
//				the function result object.
////////////////////////////////////////////////////////////////////////////

void	DLL_OBJECT::CheckIDs(
	AOBJECTID	oiDLL)
{
	DLL_RETARG*			dataBlockResPtr;
	DLL_ARGUMENTS*	dataBlockPtr;
	long						numArgs;
	int							i;

	theObject = oiDLL;

	//	data 1, result object

	dataBlockResPtr = (DLL_RETARG*)AObjLockData(theObject, 1);
	if (!AObjCheckType(dataBlockResPtr->theObject, 0L)) {
		dataBlockResPtr->theObject = 0L;			// the object was deleted.
//		dataBlockResPtr->paramType = -1;
	}
	AObjUnlockData(theObject,1);

	numArgs = AObjGetDataCount(theObject);
	for (i=2; i<numArgs; ++i) {		// copy data blocks to temp area.
		dataBlockPtr = (DLL_ARGUMENTS*)AObjLockData(theObject, i);
		if (dataBlockPtr->theArgObject != 0L)
			if (!AObjCheckType(dataBlockPtr->theArgObject, 0L))
				dataBlockPtr->theArgObject = 0L;			// object was deleted.
		AObjUnlockData(theObject,i);
	}			// for (i=2; i<numArgs; ++i)...
}


////////////////////////////////////////////////////////////////////////////

void	DLL_OBJECT::Validate(
	pAObjMessage theSystem,
	AOBJECTID	oiDLL)
{
	DLL_RETARG*			dataBlockResPtr;
	DLL_ARGUMENTS*	dataBlockPtr;
	long						numArgs;
	int							i;

	theObject = oiDLL;
	dataBlockResPtr = (DLL_RETARG*)AObjLockData(theObject, 1);
	AObjValidateID(&(dataBlockResPtr->theObject), theSystem);
	AObjUnlockData(theObject,1);

	numArgs = AObjGetDataCount(theObject);
	for (i=2; i<numArgs; ++i) {		// copy data blocks to temp area.
		dataBlockPtr = (DLL_ARGUMENTS*)AObjLockData(theObject, i);
		if (dataBlockPtr->theArgObject != 0L)
			AObjValidateID(&(dataBlockPtr->theArgObject), theSystem);
		AObjUnlockData(theObject,i);
	}			// for (i=2; i<numArgs; ++i)...
}
