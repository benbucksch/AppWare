/*************************************************************************
**	File Name:
**		TDEVADMN.CPP	Copyright 1994 Novell Inc.
**
**	Description:
**		The administration routine for the AppWare TDev Telephony
**		Services Device Object.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

//	Include files

#include <windows.h>
#include <malloc.h>
#include <memory.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"

//	External routines

#ifdef __cplusplus
extern "C" {
#endif

LONG ALMCALLBACK TDevAdmin(OBJECTID oiTDevc, pAObjMessage theSystem);
BOOL CALLBACK TDeviceSetup(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif

//	Static routines used by TDEVAdmin

static LONG Created 		    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG Awakened		    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG Asleep			    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG Destroyed		    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG PreRead 		    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG Read  				    (OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG PostRead		    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG PreWrite		    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG Write 				    (OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG PostWrite		    	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG AssignObject	  	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG ImportObject	  	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG ExportObject	  	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG QueryProtocol	  	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG BuildSignals	  	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG CheckObject 	  	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG EditObject		  	(OBJECTID oiTDevc, pAObjMessage theSystem);
static LONG ValidateObjectIDs	(OBJECTID oiTDevc, pAObjMessage theSystem);

//	Static routines used by EditObject

static BOOL InitDialog			  (HWND hDlg, OBJECTID oiTDevc);
static void MoveLineObjects   (HWND hDlg, UINT iDst, UINT iSrc);
static BOOL UpdateObject		  (HWND hDlg);

//	Module (STATIC) variables

static char * tdevSignals[] = {
	{"Startup"},
	{"Call Clear"},
	{"Conferenced"},
	{"Connection Clear"},
	{"Phone Ringing"},
	{"Diverted"},
	{"Established"},
	{"Failed"},
	{"Held"},
	{"Network Reached"},
	{"Originated"},
	{"Queued"},
	{"Retrieved"},
	{"Service Initiated"},
	{"Transferred"},
	{"Do Not Disturb On"},
	{"Do Not Disturb Off"},
	{"Forward Immediate"},
	{"Forward Busy"},
	{"Forward No Answer"},
	{"Forward Busy Internal"},
	{"Forward Busy External"},
	{"Forward No Answer Internal"},
	{"Forward No Answer External"},
	{"Forwarding Off"},
	{"Message Waiting On"},
	{"Message Waiting Off"},
};

//	Routines

/*************************************************************************
**	Function:
**		TDevAdmin				(AppWare Bus)
**
**	Description:
**		The dispatch function for all administrative messages passed to
**		an AppWare TDevice (AT&T-Novell) Telephony Device Object.
*/

LONG ALMCALLBACK TDevAdmin(OBJECTID oiTDevc, pAObjMessage theSystem) {

	switch (theSystem->message1) {
		case AOBJ_CREATED:
			return Created(oiTDevc, theSystem);

		case AOBJ_AWAKENED:
			return Awakened(oiTDevc, theSystem);

		case AOBJ_ASLEEP:
			return Asleep(oiTDevc, theSystem);

		case AOBJ_DESTROYED:
			return Destroyed(oiTDevc, theSystem);

///		case AOBJ_PREREAD:
///			return PreRead(oiTDevc, theSystem);

		case AOBJ_READ:
			return Read(oiTDevc, theSystem);

///		case AOBJ_POSTREAD:
///			return PostRead(oiTDevc, theSystem);

		case AOBJ_PREWRITE:
			return PreWrite(oiTDevc, theSystem);

///		case AOBJ_WRITE:
///			return Write(oiTDevc, theSystem);

		case AOBJ_POSTWRITE:
			return PostWrite(oiTDevc, theSystem);

		case AOBJ_ASSIGNOBJECT:
			return AssignObject(oiTDevc, theSystem);

		case AOBJ_IMPORTOBJECT:
			return ImportObject(oiTDevc, theSystem);

		case AOBJ_EXPORTOBJECT:
			return ExportObject(oiTDevc, theSystem);

		case AOBJ_QUERYPROTOCOL:
			return QueryProtocol(oiTDevc, theSystem);

		case AOBJ_BUILDSIGNALS:
			return BuildSignals(oiTDevc, theSystem);

		case AOBJ_CHECKOBJECT:
			return CheckObject(oiTDevc, theSystem);

		case AOBJ_EDITOBJECT:
			return EditObject(oiTDevc, theSystem);

		case AOBJ_VALIDATEOBJECTIDS:
			return ValidateObjectIDs(oiTDevc, theSystem);
	}
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		Created 				(TDevAdmin)
**
**	Description:
**		Creates a Telephony Device object by creating and sizing its
**		single data block and setting the datablock members to their
**		initial default values.
*/

static LONG Created(OBJECTID oiTDevc, pAObjMessage theSystem) {
	LPTDEVICE	lpTDevice;

	AObjSetDataCount(oiTDevc, 1);
	AObjResizeData(oiTDevc, TDEVC_MAINBLK, sizeof(TDEVICE));
	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		_fmemset(lpTDevice, 0, sizeof(TDEVICE));
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return A_OK;
	}
	return AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
		"The TDevice Object creation failed.", 0);
}

/*************************************************************************
**	Function:
**		Awakened				(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG Awakened(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		Asleep					(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG Asleep(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		Destroyed				(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG Destroyed(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		PreRead 				(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG PreRead(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		Read					(TDevAdmin)
**
**	Description:
**		Checks for a valid DLL version so that the application will not
**		attempt to use a version that is older than the app itself.
*/

static LONG Read(OBJECTID oiTDevc, pAObjMessage theSystem) {
	if (AObjGetDataSize(oiTDevc, TDEVC_MAINBLK) == sizeof(TDEVICE))
		return A_OK;
	return AObjReportError(oiTDevc, theSystem->message1, A_WARNING,
		"ALM_CSTA.DLL expects a different number of bytes "
		"in the TDevice Object data block than is now allocated.", 0);
}

/*************************************************************************
**	Function:
**		PostRead				(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG PostRead(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		PreWrite				(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG PreWrite(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		Write					(TDevAdmin)
**
**	Description:
*/

#pragma argsused
static LONG Write(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		PostWrite				(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG PostWrite(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		AssignObject			(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG AssignObject(OBJECTID oiTDevc, pAObjMessage theSystem) {
	OBJECTID	oiSource;
	LPTDEVICE	lpSrcData;
	LPTDEVICE	lpDstData;

	oiSource = (OBJECTID)theSystem->message3;
	if (!oiSource)
		return AObjReportError(oiTDevc, theSystem->message1, A_WARNING,
			"Invalid TDevice source object detected during assignment.", 0);
	lpSrcData = (LPTDEVICE)AObjLockData(oiSource, TDEVC_MAINBLK);
	if (lpSrcData) {
		lpDstData = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
		if (lpDstData) {
			CopyMemory(lpDstData, lpSrcData, sizeof(TDEVICE));
			AObjUnlockData(oiSource, TDEVC_MAINBLK);
			AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
			return A_OK;
		}
	}
	return AObjReportError(oiTDevc, theSystem->message1, A_WARNING,
		"Invalid TDevice Object source or destination data blocks "
		"detected during assignment.", 0);
}

/*************************************************************************
**	Function:
**		ImportObject			(TDevAdmin)
**
**	Description:
**		Imports the TDevice object fields using the UPSF format.
*/

static LONG ImportObject(OBJECTID oiTDevc, pAObjMessage theSystem) {
	LPTDEVICE	lpTDevice;
	char		szFieldName[10];
	UINT		i;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		AUpsfImportField("oiSendQSize", 0, AUPSF_NATV_OBJECTREF, &lpTDevice->oiSendQSize, sizeof(OBJECTID));
		AUpsfImportField("oiRecvQSize", 0, AUPSF_NATV_OBJECTREF, &lpTDevice->oiRecvQSize, sizeof(OBJECTID));
		AUpsfImportField("oiServerID", 0, AUPSF_NATV_OBJECTREF, &lpTDevice->oiServerID, sizeof(OBJECTID));
		AUpsfImportField("oiLoginID", 0, AUPSF_NATV_OBJECTREF, &lpTDevice->oiLoginID, sizeof(OBJECTID));
		AUpsfImportField("oiPassword", 0, AUPSF_NATV_OBJECTREF, &lpTDevice->oiPassword, sizeof(OBJECTID));
		AUpsfImportField("oiMonDev", 0, AUPSF_NATV_OBJECTREF, &lpTDevice->oiMonDev, sizeof(OBJECTID));
		for (i = 0; i < TDEVC_MAXLINES; i++) {
			wsprintf(szFieldName, "oiLine%03d", i);
			AUpsfImportField(szFieldName, 0, AUPSF_NATV_OBJECTREF, &lpTDevice->oiLines[i], sizeof(OBJECTID));
		}
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return A_OK;
	}
	return AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
		"Import failed to find a data block to import on this object.", 0);
}

/*************************************************************************
**	Function:
**		ExportObject			(TDevAdmin)
**
**	Description:
**		Exports the TDevice object fields using the UPSF format.
*/

static LONG ExportObject(OBJECTID oiTDevc, pAObjMessage theSystem) {
	LPTDEVICE	lpTDevice;
	char		szFieldName[10];
	UINT		i;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		AUpsfExportField("oiSendQSize", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTDevice->oiSendQSize, sizeof(OBJECTID));
		AUpsfExportField("oiRecvQSize", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTDevice->oiRecvQSize, sizeof(OBJECTID));
		AUpsfExportField("oiServerID", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTDevice->oiServerID, sizeof(OBJECTID));
		AUpsfExportField("oiLoginID", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTDevice->oiLoginID, sizeof(OBJECTID));
		AUpsfExportField("oiPassword", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTDevice->oiPassword, sizeof(OBJECTID));
		AUpsfExportField("oiMonDev", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTDevice->oiMonDev, sizeof(OBJECTID));
		for (i = 0; i < TDEVC_MAXLINES; i++) {
			wsprintf(szFieldName, "oiLine%03d", i);
			AUpsfExportField(szFieldName, 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTDevice->oiLines[i], sizeof(OBJECTID));
		}
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return A_OK;
	}
	return AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
		"Export failed to find a data block to export on this object.", 0);
}

/*************************************************************************
**	Function:
**		QueryProtocol			(TDevAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG QueryProtocol(OBJECTID oiTDevc, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		BuildSignals			(TDevAdmin)
**
**	Description:
**		Builds an array of TDevice Object signal names.
*/

#pragma argsused
static LONG BuildSignals(OBJECTID oiTDevc, pAObjMessage theSystem) {
	AObjSignalRecord *	pSignal;
	MEMBLOCKID				signalBlock;
	UINT					nSignals;
	UINT					i;

	signalBlock = (MEMBLOCKID)theSystem->message2;
	nSignals = sizeof(tdevSignals)/sizeof(tdevSignals[0]);
	AMemSetSize(signalBlock, nSignals * sizeof(AObjSignalRecord));
	pSignal = (AObjSignalRecord *)AMemLock(signalBlock);
	for (i = 0; i < nSignals; i++) {
		pSignal[i].theID = i;
		lstrcpy(pSignal[i].theName, tdevSignals[i]);
	}
	AMemUnlock(signalBlock);
	return A_OK;
}

/*************************************************************************
**	Function:
**		CheckObject 			(TDevAdmin)
**
**	Description:
**		Verifies that the TDevice Object is ready to be compiled.
*/

static LONG CheckObject(OBJECTID oiTDevc, pAObjMessage theSystem) {
	LPTDEVICE	lpTDevice;
	LONG		lResult = A_OK;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		if (!lpTDevice->oiSendQSize)
			lResult = AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
				"No number object attached to the TDevice Object "
				"to hold the Send Queue Size.", 0);
		else if (!lpTDevice->oiRecvQSize)
			lResult = AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
				"No number object attached to the TDevice Object "
				"to hold the Receive Queue Size.", 0);
		else if (!lpTDevice->oiServerID)
			lResult = AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
				"No text object attached to the TDevice Object "
				"to hold the telephony server name.", 0);
		else if (!lpTDevice->oiLoginID)
			lResult = AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
				"No text object attached to the TDevice Object "
				"to hold the user login name.", 0);
		else if (!lpTDevice->oiPassword)
			lResult = AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
				"No text object attached to the TDevice Object "
				"to hold the user password.", 0);
		else if (!lpTDevice->oiMonDev)
			lResult = AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
				"No text object attached to the TDevice Object "
				"to hold the Monitor Device PBX Extension.", 0);
		else if (!lpTDevice->lNumLines)
			lResult = AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
				"No Line objects assigned to this TDevice Object.", 0);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return lResult;
	}
	return AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
		"No data block could be found on this TDevice Object during check.", 0);
}

/*************************************************************************
**	Function:
**		EditObject				(TDevAdmin)
**
**	Description:
**		Internally used to edit the data members of the TDevice Object
**		at design time.
*/

#pragma argsused
static LONG EditObject(OBJECTID oiTDevc, pAObjMessage theSystem) {
	DLGPROC dlgProc;
	BOOL	fResult;

	dlgProc = (DLGPROC)MakeProcInstance((FARPROC)TDeviceSetup, hInstance);
	fResult = DialogBoxParam(hInstance, "TDevObjectEdit",
		GetActiveWindow(), dlgProc, (LPARAM)oiTDevc);
	FreeProcInstance((FARPROC)dlgProc);
	return fResult? A_MODIFIED : A_OK;
}

/*************************************************************************
**	Function:
**		TDeviceSetup			(Windows Kernal)
**
**	Description:
**		Used to edit the data members of the TDevice Object at
**		design time.
*/

BOOL CALLBACK TDeviceSetup(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case WM_INITDIALOG:
			return InitDialog(hDlg, (OBJECTID)lParam);

		case WM_COMMAND: //!!PORT!!
			switch(wParam) {
				case IDC_ADDLINES:
					MoveLineObjects(hDlg, IDC_OWNEDLIST, IDC_AVAILLIST);
					break;

				case IDC_REMOVELINES:
					MoveLineObjects(hDlg, IDC_AVAILLIST, IDC_OWNEDLIST);
					break;

				case IDOK:
					return UpdateObject(hDlg);

				case IDCANCEL:
					EndDialog (hDlg, FALSE);
					return TRUE;
			}
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		InitDialog				(TDeviceSetup)
**
**	Description:
**		Initialize the TDevice Object's edit dialog box.
*/

static BOOL InitDialog(HWND hDlg, OBJECTID oiTDevc) {
	LPTDEVICE	lpTDevice;
	OBJECTID	oiToAdd;
	HWND		hText;
	long		key;
	char		buffer[40];
	int 		objectIndex;
	int 		iDst;
	int 		i;

	SetWindowLong(hDlg, DWL_USER, (LONG)oiTDevc);
	hText = GetDlgItem(hDlg, IDC_OBJECT);
	AUtlSetTitleFont(OTYPE_TDEV, NULL, hText);
	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (!lpTDevice)
		return TRUE;

	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_SERVERID,
			CB_ADDSTRING, (WPARAM)0, (LPARAM)buffer);
		SendDlgItemMessage(hDlg, IDC_SERVERID, CB_SETITEMDATA,
			(WPARAM)objectIndex, (LPARAM)oiToAdd);
		if (oiToAdd == lpTDevice->oiServerID)
			SendDlgItemMessage(hDlg, IDC_SERVERID, CB_SETCURSEL,
				(WPARAM)objectIndex, (LPARAM)0);
	}
	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_LOGINID,
			CB_ADDSTRING, (WPARAM)0, (LPARAM)buffer);
		SendDlgItemMessage(hDlg, IDC_LOGINID, CB_SETITEMDATA,
			(WPARAM)objectIndex, (LPARAM)oiToAdd);
		if (oiToAdd == lpTDevice->oiLoginID)
			SendDlgItemMessage(hDlg, IDC_LOGINID, CB_SETCURSEL,
				(WPARAM)objectIndex, (LPARAM)0);
	}
	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_PASSWORD,
			CB_ADDSTRING, (WPARAM)0, (LPARAM)buffer);
		SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_SETITEMDATA,
			(WPARAM)objectIndex, (LPARAM)oiToAdd);
		if (oiToAdd == lpTDevice->oiPassword)
			SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_SETCURSEL,
				(WPARAM)objectIndex, (LPARAM)0);
	}
	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_MONDEV,
			CB_ADDSTRING, (WPARAM)0, (LPARAM)buffer);
		SendDlgItemMessage(hDlg, IDC_MONDEV, CB_SETITEMDATA,
			(WPARAM)objectIndex, (LPARAM)oiToAdd);
		if (oiToAdd == lpTDevice->oiMonDev)
			SendDlgItemMessage(hDlg, IDC_MONDEV, CB_SETCURSEL,
				(WPARAM)objectIndex, (LPARAM)0);
	}
	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_NUMBER, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_SENDQSIZE,
			CB_ADDSTRING, (WPARAM)0, (LPARAM)buffer);
		SendDlgItemMessage(hDlg, IDC_SENDQSIZE, CB_SETITEMDATA,
			(WPARAM)objectIndex, (LPARAM)oiToAdd);
		if (oiToAdd == lpTDevice->oiSendQSize)
			SendDlgItemMessage(hDlg, IDC_SENDQSIZE, CB_SETCURSEL,
				(WPARAM)objectIndex, (LPARAM)0);
	}
	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_NUMBER, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_RECVQSIZE,
			CB_ADDSTRING, (WPARAM)0, (LPARAM)buffer);
		SendDlgItemMessage(hDlg, IDC_RECVQSIZE, CB_SETITEMDATA,
			(WPARAM)objectIndex, (LPARAM)oiToAdd);
		if (oiToAdd == lpTDevice->oiRecvQSize)
			SendDlgItemMessage(hDlg, IDC_RECVQSIZE, CB_SETCURSEL,
				(WPARAM)objectIndex, (LPARAM)0);
	}
	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_TLIN, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		iDst = IDC_AVAILLIST;
		for (i = 0; i < lpTDevice->lNumLines; i++)
			if (oiToAdd == lpTDevice->oiLines[i]) {
				iDst = IDC_OWNEDLIST;
				break;
			}
		objectIndex = (int)SendDlgItemMessage(hDlg, iDst, LB_ADDSTRING,
			(WPARAM)0, (LPARAM)(LPCSTR)buffer);
		SendDlgItemMessage(hDlg, iDst, LB_SETITEMDATA,
			(WPARAM)objectIndex, (LPARAM)oiToAdd);
	}
	AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	return TRUE;
}

/*************************************************************************
**	Function:
**		MoveLineObjects 		(TDeviceSetup)
**
**	Description:
**		Moves selected objects from the Source List Box to Destination
**		List Box.
*/

static void MoveLineObjects(HWND hDlg, UINT iDst, UINT iSrc) {
	int 		count;
	int 		i;
	int 		index;
	int 		items[TDEVC_MAXLINES];
	char		buffer[40];
	OBJECTID	oiToMove;

	count = (int)SendDlgItemMessage(hDlg, iSrc, LB_GETSELCOUNT, 0, 0L);
	if (count) {
		SendDlgItemMessage(hDlg, iSrc, LB_GETSELITEMS,
			(WPARAM)count, (LPARAM)(int *)items);
		for (i = count - 1; i >= 0; i--) {
			SendDlgItemMessage(hDlg, iSrc, LB_GETTEXT,
				(WPARAM)items[i], (LPARAM)(LPCSTR)buffer);
			oiToMove = (OBJECTID)SendDlgItemMessage(hDlg, iSrc,
				LB_GETITEMDATA, (WPARAM)items[i], (LPARAM)0);
			SendDlgItemMessage(hDlg, iSrc, LB_DELETESTRING,
				(WPARAM)items[i], (LPARAM)0);
			index = (int)SendDlgItemMessage(hDlg, iDst, LB_INSERTSTRING,
				(WPARAM)0, (LPARAM)(LPCSTR)buffer);
			SendDlgItemMessage(hDlg, iDst, LB_SETITEMDATA,
				(WPARAM)index, (LPARAM)oiToMove);
		}
	}
	return;
}

/*************************************************************************
**	Function:
**		UpdateObject			(TDeviceSetup)
**
**	Description:
**		Updates the TDevice Object's data.
*/

static BOOL UpdateObject(HWND hDlg) {
	LPTDEVICE	lpTDevice;
    LPTLINE		lpTLine;
	OBJECTID	oiTDevc;
	int 		index;

	oiTDevc = (OBJECTID)GetWindowLong(hDlg, DWL_USER);
	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (!lpTDevice) {
		EndDialog (hDlg, FALSE);
		return TRUE;
	}
	index = (int)SendDlgItemMessage(hDlg, IDC_SERVERID, CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	if (index == CB_ERR)
		lpTDevice->oiServerID = 0;
	else
		lpTDevice->oiServerID = (OBJECTID)SendDlgItemMessage(hDlg,
			IDC_SERVERID, CB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
	index = (int)SendDlgItemMessage(hDlg, IDC_LOGINID, CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	if (index == CB_ERR)
		lpTDevice->oiLoginID = 0;
	else
		lpTDevice->oiLoginID = (OBJECTID)SendDlgItemMessage(hDlg,
			IDC_LOGINID, CB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
	index = (int)SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	if (index == CB_ERR)
		lpTDevice->oiPassword = 0;
	else
		lpTDevice->oiPassword = (OBJECTID)SendDlgItemMessage(hDlg,
			IDC_PASSWORD, CB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
	index = (int)SendDlgItemMessage(hDlg, IDC_MONDEV, CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	if (index == CB_ERR)
		lpTDevice->oiMonDev = 0;
	else
		lpTDevice->oiMonDev = (OBJECTID)SendDlgItemMessage(hDlg,
			IDC_MONDEV, CB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
	index = (int)SendDlgItemMessage(hDlg, IDC_SENDQSIZE, CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	if (index == CB_ERR)
		lpTDevice->oiSendQSize = 0;
	else
		lpTDevice->oiSendQSize = (OBJECTID)SendDlgItemMessage(hDlg,
			IDC_SENDQSIZE, CB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
	index = (int)SendDlgItemMessage(hDlg, IDC_RECVQSIZE, CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	if (index == CB_ERR)
		lpTDevice->oiRecvQSize = 0;
	else
		lpTDevice->oiRecvQSize = (OBJECTID)SendDlgItemMessage(hDlg,
			IDC_RECVQSIZE, CB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
	lpTDevice->lNumLines = (int)SendDlgItemMessage(hDlg, IDC_OWNEDLIST,
		LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
	for (index = 0; index < lpTDevice->lNumLines; index++) {
		lpTDevice->oiLines[index] = (OBJECTID)SendDlgItemMessage(hDlg,
			IDC_OWNEDLIST, LB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
		lpTLine = (LPTLINE)AObjLockData(lpTDevice->oiLines[index], TLINE_MAINBLK);
		if (lpTLine) {
        	lpTLine->oiOwner = oiTDevc;
        	AObjUnlockData(lpTDevice->oiLines[index], TLINE_MAINBLK);
        }
    }
	for (; index < TDEVC_MAXLINES; index++)
		lpTDevice->oiLines[index] = (OBJECTID)0;

	AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	EndDialog (hDlg, TRUE);
	return TRUE;
}

/*************************************************************************
**	Function:
**		ValidateObjectIDs		(TDevAdmin)
**
**	Description:
**		Updates the stored Object IDs members.
*/

static LONG ValidateObjectIDs(OBJECTID oiTDevc, pAObjMessage theSystem) {
	LPTDEVICE	lpTDevice;
	int 		i;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		AObjValidateID(&lpTDevice->oiSendQSize, theSystem);
		AObjValidateID(&lpTDevice->oiRecvQSize, theSystem);
		AObjValidateID(&lpTDevice->oiServerID, theSystem);
		AObjValidateID(&lpTDevice->oiLoginID, theSystem);
		AObjValidateID(&lpTDevice->oiPassword, theSystem);
		AObjValidateID(&lpTDevice->oiMonDev, theSystem);
		for (i = 0; i < lpTDevice->lNumLines; i++)
			AObjValidateID(&lpTDevice->oiLines[i], theSystem);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return A_OK;
	}
	return AObjReportError(oiTDevc, theSystem->message1, A_ERROR,
		"No data block associated with this TDevice Object during "
		"data block validation.", 0);
}

