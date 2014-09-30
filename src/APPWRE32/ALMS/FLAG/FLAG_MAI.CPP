////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//	"flag_mai.cpp"
//
// The main module for the flag object -- LibMain/*!!PORT!! Replace with DllEntryPoint */ and WEP/*!!PORT!! Replace with DllEntryPoint */
//
//	This module also contains the Admn and ObjD procs.
//
////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<a_alm.h>
#include	<a_almutl.h>
#include	<helpids.h>
#include	<intdebug.h>
#include	"flag_hdr.h"
#include	"o_flag.h"
#include	"flag.h"
#include	"flag_rsc.h"

// global variables ////////////////////////////////////////////////////////

GLOBAL			G;
extern int	_WinAllocFlag;


#ifdef __BORLANDC__
/**********************************************************
 *	Some little magic to fix the load library proble.
 *	flag_HaveWeBeenLoaded
 **********************************************************/

int CALLBACK /*!!PORT!! was _far_ _pascal_*/  flag_HaveWeBeenLoaded()
{
	static BOOL	bLoaded = FALSE;
	BOOL				rVal = bLoaded;

	bLoaded = TRUE;
	return rVal;
}
#endif
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

	G.hInstance = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
#ifdef __BORLANDC__
			if(flag_HaveWeBeenLoaded())	// make sure right libMain is been call.
				break ;	
//			_WinAllocFlag = GMEM_SHARE;	// magical line to make 'new' operator work!?
#endif
			G.cbByteOffset = DWL_USER;
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


// LibMain/*!!PORT!! Replace with DllEntryPoint */ -- DLL entry point //////////////////////////////////////////////
//		this will Initialize some global variables.

//extern "C" int CALLBACK /*!!PORT!! was _far_ _pascal_*/ LibMain/*!!PORT!! Replace with DllEntryPoint */ (
//	HINSTANCE	hInstance,
//	WORD			/*wDataSeg*/,
//	WORD			wHeapSize,
//	LPSTR			/*lpszCmdLine*/)
//
//	{
//	#ifdef __BORLANDC__
//	if(flag_HaveWeBeenLoaded())	// make sure right libMain is been call.
//		return 0;	
//	_WinAllocFlag = GMEM_SHARE;	// magical line to make 'new' operator work!?
//	#endif
//
//	// initialize the necessary global variables
//	G.hInstance = hInstance;
//	G.cbByteOffset = DWL_USER;//serdlgGetByteOffset();
//
//	if(wHeapSize>0)
//		UnlockData(0);
//	return 1;	// the dll was initialized properly
//	}
//
//// WEP/*!!PORT!! Replace with DllEntryPoint */ -- DLL exit point ///////////////////////////////////////////////////
//
//int CALLBACK /*!!PORT!! was _far_ _pascal_*/  WEP/*!!PORT!! Replace with DllEntryPoint */(
//	int	/*nParameter*/)
//
//	{
//	return 1;
//	}

// GetSerVersion ///////////////////////////////////////////////////////////

long CALLBACK GetSerVersion(
	ATYPEID	theType)

	{
	if(theType==OTYPE_FLAG)
		return VERSION_DLL;
	else
		return -1L;
	}

// Flag_Admn ///////////////////////////////////////////////////////////////

LONG CALLBACK Flag_Admn(
	AOBJECTID			theObject,
	pAObjMessage	theSystem)

	{
	switch((int)(theSystem->message1))
		{
		case AOBJ_CREATED:
			{
			FLAG	flag(theObject);
      return (A_OK);
			}

		case AOBJ_ASSIGNOBJECT:
			Flag_doAssign(theObject, (AOBJECTID)theSystem->message3);
			return (A_OK);

		case AOBJ_IMPORTOBJECT:
			{
			FLAG	flag(theObject);
      flag.Import();
			return (A_OK);
			}

		case AOBJ_EXPORTOBJECT:
			{
			FLAG	flag(theObject);
      flag.Export();
			return (A_OK);
			}

		case AOBJ_READ:
			return (Flag_doVersionCheck(theObject, theSystem->message3, theSystem->message4, theSystem->errorMessage));

		default:
			return(Flag_ObjD(theObject,theSystem));
		}
	}

// Flag_doAssign ///////////////////////////////////////////////////////////

void Flag_doAssign(
	AOBJECTID	dstObject,
	AOBJECTID	srcObject)

	{
	FLAG	srcFlag(srcObject, TRUE);	// use partial constructors
	FLAG	dstFlag(dstObject, TRUE);

	if(dstFlag.IsSet() != srcFlag.IsSet())
		dstFlag.Toggle();
	}

// Flag_doVersionCheck /////////////////////////////////////////////////////

LONG Flag_doVersionCheck(
	AOBJECTID	theObject,
	long&			version,
	long&			status,
	char*			/* error */)

	{
	FLAGDATA*	fdata = (FLAGDATA*)AObjLockData(theObject, DATA_FLAGDATA);
	if(fdata && fdata->version<=VERSION_OBJECT)
		{
		version = VERSION_OBJECT;
		status = A_OK;
		AObjUnlockData(theObject, DATA_FLAGDATA);
		return (A_OK);
		}
	else
		{
		version = fdata->version;
		status = A_ERROR;
		AObjReportError(theObject, AOBJ_READ, status, "Flag\tser_flag.dll", version);
    return (A_ERROR);
		}
	}

// Flag_ObjD ///////////////////////////////////////////////////////////////

LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/ 	Flag_ObjD(
	AOBJECTID	theObject,
	pAObjMessage	theSystem)

	{
	switch((int)theSystem->message1)
		{
		case AOBJ_EDITOBJECT:
    	Flag_editObject(theObject);
			return (A_MODIFIED);

		case AOBJ_VALIDATEOBJECTIDS:
			return (A_OK);

		case AOBJ_CHECKOBJECT:
			return (Flag_getParameters(theObject, (AObjInfo*)theSystem->message2));

		case AOBJ_BUILDSIGNALS:
			Flag_buildSketchSignals((AMEMBLOCKID)theSystem->message2);
			return (A_OK);

		default:
			return (A_NOTHANDLED);
		}
	}

// Flag_editObject /////////////////////////////////////////////////////////

void Flag_editObject(
	AOBJECTID	theObject)

	{
	FLAG	flag(theObject);

	// open the dialog
	DialogBoxParam(
		G.hInstance,
		MAKEINTRESOURCE(FLAGDIALOG),
		GetActiveWindow(),
		(DLGPROC)Flag_ObjDDialogProc,
		(LPARAM)&flag);
	}

// Flag_getParameters //////////////////////////////////////////////////////

LONG Flag_getParameters(
	AOBJECTID	flagObj,
	AObjInfo*	infoRecord)

	{
	FLAG	flag(flagObj);
	if(lstrcmpi(flag.SetText(), flag.ClearText()))
		return (A_OK);
	else
		{
		lstrcpy(infoRecord->errorMessage, "The text for the flag clear and flag set states are the same.");
			return (A_WARNING);
		}
	}

// Flag_buildSketchSignals /////////////////////////////////////////////////

void Flag_buildSketchSignals(
	AMEMBLOCKID	theBlock)
{
	AMemSetSize(theBlock, (long)sizeof(AObjSignalRecord)*SIGNAL_NUMSIGNALS);
	pAObjSignalRecord	theSignals = (pAObjSignalRecord)AMemLock(theBlock);

	if(theSignals)
	{
		int	i=0;
		theSignals[i].theID = SIGNAL_SET;
		lstrcpy(theSignals[i].theName, SIGNAME_SET);
		theSignals[++i].theID = SIGNAL_CLEARED;
		lstrcpy(theSignals[i].theName, SIGNAME_CLEARED);
	}
}

// Flag_ObjDDialogProc /////////////////////////////////////////////////////

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flag_ObjDDialogProc(
	HWND		hDlg,
	UINT		uMsg,
	WPARAM	wParam,
	LPARAM	lParam)

	{
	FLAG*	flag;

	switch(uMsg)
		{
		case WM_INITDIALOG:
			{
      // write the object name to the title bar
			char	objName[80];
			char	szType[256];
      flag = (FLAG*)lParam;

			//AUtlSetTitleFont(OTYPE_FLAG, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);

			ATypeGetName(OTYPE_FLAG, szType, sizeof(szType));
			strcat(szType, ": ");
			AObjGetName(flag->ObjectID(), objName, 255);
			strcat(szType, objName);
			SetWindowText(hDlg, szType/*objName*/);
			SetWindowLong(hDlg, DWL_USER, (long)flag);

			// initialize the dialog
			flag->InitDlgControls(hDlg);
			}

		case WM_COMMAND: //!!PORT!!
			switch(LOWORD(wParam))
				{
        // buttons on right side of dialog
				case IDOK:
					flag = (FLAG*)GetWindowLong(hDlg, G.cbByteOffset);
					flag->UpdateObject(hDlg);
          EndDialog(hDlg, TRUE);
					return TRUE;

				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					return TRUE;

				case IDHELP:
					WinHelp(hDlg, "awusflag.hlp", HELP_CONTEXT, HELPID_OBJD);
					return TRUE;

				default:
					break;
				}
			break;

		default:
			break;
		}

  return FALSE;
	}

////////////////////////////////////////////////////////////////////////////
