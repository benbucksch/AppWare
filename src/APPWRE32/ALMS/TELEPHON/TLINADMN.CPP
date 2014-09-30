/*************************************************************************
**	File Name:
**		TLINADMN.CPP		Copyright 1994 Novell Inc.
**
**	Description:
**      The administration routine for the TLine Window Object.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

#define STRICT
#define ASTRICT

//	Include files

#include <windows.h>
#include <memory.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"
#include "tlinvisu.h"

//	External routines

#ifdef __cplusplus
extern "C" {
#endif

LONG ALMCALLBACK TLineAdmin(OBJECTID oiStp, pAObjMessage theSystem);
BOOL CALLBACK TLineSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif

//	Static routines used by TLineAdmin

static LONG Created 			(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG Awakened			(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG Asleep				(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG Destroyed			(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG Read				(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG PreWrite			(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG PostWrite			(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG AssignObject		(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG ImportObject		(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG ExportObject		(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG QueryProtocol		(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG BuildSignals		(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG CheckObject 		(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG EditObject			(OBJECTID oiTLine, pAObjMessage theSystem);
static LONG ValidateObjectIDs	(OBJECTID oiTLine, pAObjMessage theSystem);

//	Static routines used by TLineSetup (Dialog Box Window)

static BOOL InitDialog(HWND hDlg, OBJECTID oiTLine);
static BOOL UpdateObject(HWND hDlg);

//	Module (STATIC) variables

static char * tlineSignals[] = {
	{"Line Ringing"},
	{"Off Hook"},
	{"On Hook"},
	{"On Hold"},
	{"Retrieved"},
};

//	Routines

/*************************************************************************
**	Function:
**		TLineAdmin				(AppWare Bus)
**
**	Description:
**		The dispatch function for all administrative messages passed to
**		an AppWare TLine Object.
*/

LONG ALMCALLBACK TLineAdmin(OBJECTID oiTLine, pAObjMessage theSystem) {

	switch (theSystem->message1) {
		case AOBJ_CREATED:
			return Created(oiTLine, theSystem);

		case AOBJ_AWAKENED:
			return Awakened(oiTLine, theSystem);

		case AOBJ_ASLEEP:
			return Asleep(oiTLine, theSystem);

		case AOBJ_DESTROYED:
			return Destroyed(oiTLine, theSystem);

		case AOBJ_READ:
			return Read(oiTLine, theSystem);

		case AOBJ_PREWRITE:
			return PreWrite(oiTLine, theSystem);

		case AOBJ_POSTWRITE:
			return PostWrite(oiTLine, theSystem);

		case AOBJ_ASSIGNOBJECT:
			return AssignObject(oiTLine, theSystem);

		case AOBJ_IMPORTOBJECT:
			return ImportObject(oiTLine, theSystem);

		case AOBJ_EXPORTOBJECT:
			return ExportObject(oiTLine, theSystem);

		case AOBJ_QUERYPROTOCOL:
			return QueryProtocol(oiTLine, theSystem);

		case AOBJ_BUILDSIGNALS:
			return BuildSignals(oiTLine, theSystem);

		case AOBJ_CHECKOBJECT:
			return CheckObject(oiTLine, theSystem);

		case AOBJ_EDITOBJECT:
			return EditObject(oiTLine, theSystem);

		case AOBJ_VALIDATEOBJECTIDS:
			return ValidateObjectIDs(oiTLine, theSystem);
	}
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**		Created 				(TLineAdmin)
**
**	Description:
**      Creates a TLine Object by creating and sizing its
**      data block and setting the data block members to their
**		initial default values.
*/

static LONG Created(OBJECTID oiTLine, pAObjMessage theSystem) {
	LPTLINE lpTLine;

    AObjSetDataCount(oiTLine, 1);
	AObjResizeData(oiTLine, TLINE_MAINBLK, sizeof(TLINE));
    lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
    if (lpTLine) {
		_fmemset(lpTLine, 0, sizeof(TLINE));
		lpTLine->fRedLEDActv = TRUE;
		lpTLine->fGreenLEDActv = TRUE;
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return A_OK;
	}
    return AObjReportError(oiTLine, theSystem->message1, A_ERROR,
        "The TLine Object creation failed.", 0);
}

/*************************************************************************
**	Function:
**		Awakened				(TLineAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG Awakened(OBJECTID oiTLine, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**      Asleep                  (TLineAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG Asleep(OBJECTID oiTLine, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**      Destroyed               (TLineAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG Destroyed(OBJECTID oiTLine, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**      Read                    (TLineAdmin)
**
**	Description:
**		Checks for a valid DLL version so that the application will not
**		attempt to use a version that is older than the app itself.
*/

static LONG Read(OBJECTID oiTLine, pAObjMessage theSystem) {
    if (AObjGetDataSize(oiTLine, TLINE_MAINBLK) == sizeof(TLINE))
		return A_OK;
    return AObjReportError(oiTLine, theSystem->message1, A_WARNING,
        "ALM_CSTA.DLL expects a different number of bytes "
        "in the TLine Object data block than is now allocated.", 0);
}


/*************************************************************************
**	Function:
**      PreWrite                (TLineAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG PreWrite(OBJECTID oiTLine, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**      PostWrite               (TLineAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG PostWrite(OBJECTID oiTLine, pAObjMessage theSystem) {
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**      AssignObject            (TLineAdmin)
**
**	Description:
**      Sets the destination TLine Object's data block(s) equal to the
**		source TLine Object's data block(s).
*/

#pragma argsused
static LONG AssignObject(OBJECTID oiTLine, pAObjMessage theSystem) {
	OBJECTID	oiSource = (OBJECTID)theSystem->message3;
    LPTLINE		lpSrcData, lpDstData;

    if (!oiSource)
        return AObjReportError(oiTLine, theSystem->message1, A_WARNING,
			"Invalid TLine source object detected during assignment.", 0);

	lpSrcData = (LPTLINE)AObjLockData(oiSource, TLINE_MAINBLK);
	if (lpSrcData) {
        lpDstData = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
		if (lpDstData) {
            CopyMemory(lpDstData, lpSrcData, sizeof(TLINE));
			AObjUnlockData(oiSource, TLINE_MAINBLK);
            AObjUnlockData(oiTLine, TLINE_MAINBLK);
			return A_OK;
		}
        AObjUnlockData(oiSource, TLINE_MAINBLK);
    }
    return AObjReportError(oiTLine, theSystem->message1, A_WARNING,
        "Invalid TLine Object source or destination data blocks "
		"detected during assignment.", 0);
}

/*************************************************************************
**	Function:
**      ImportObject            (TLineAdmin)
**
**	Description:
**		Imports the TLine Object fields using the UPSF format.
*/

#pragma argsused
static LONG ImportObject(OBJECTID oiTLine, pAObjMessage theSystem) {
	LPTLINE	lpTLine;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		AUpsfImportField("oiPhone", 0, AUPSF_NATV_OBJECTREF, &lpTLine->oiPhone, sizeof(OBJECTID));
		AUpsfImportField("oiOwner", 0, AUPSF_NATV_OBJECTREF, &lpTLine->oiOwner, sizeof(OBJECTID));
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return A_OK;
	}
	return AObjReportError(oiTLine, theSystem->message1, A_ERROR,
		"Import failed to find a data block to import on this object.", 0);
}

/*************************************************************************
**	Function:
**      ExportObject            (TLineAdmin)
**
**	Description:
**		Exports the TLine Object fields using the UPSF format.
*/

#pragma argsused
static LONG ExportObject(OBJECTID oiTLine, pAObjMessage theSystem) {
	LPTLINE	lpTLine;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		AUpsfExportField("oiPhone", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTLine->oiPhone, sizeof(OBJECTID));
		AUpsfExportField("oiOwner", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &lpTLine->oiOwner, sizeof(OBJECTID));
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return A_OK;
	}
	return AObjReportError(oiTLine, theSystem->message1, A_ERROR,
		"Export failed to find a data block to export on this object.", 0);
}

/*************************************************************************
**	Function:
**      QueryProtocol           (TLineAdmin)
**
**	Description:
**      Returns not handled for all but APROT_WND.  For Window Object
**      protocol, returns AP_SUPPORTED and sets the Create function to
**      point to the stoplight creation function.
*/

#pragma argsused
static LONG QueryProtocol(OBJECTID oiTLine, pAObjMessage theSystem) {
    ApQueryInfo  * lpQueryInfo = (ApQueryInfo  *)theSystem->message4;

    if (lpQueryInfo->protocol == APROT_WND) {
		lpQueryInfo->pfnCreate = (APPCFN)TLineCreate;
		return AP_SUPPORTED;
	}
	return A_NOTHANDLED;
}

/*************************************************************************
**	Function:
**      BuildSignals            (TLineAdmin)
**
**	Description:
**		Not handled.
*/

#pragma argsused
static LONG BuildSignals(OBJECTID oiTDevc, pAObjMessage theSystem) {
	AObjSignalRecord *	pSignal;
	MEMBLOCKID				    signalBlock;
	UINT					        nSignals;
	UINT					        i;

	signalBlock = (MEMBLOCKID)theSystem->message2;
	nSignals = sizeof(tlineSignals)/sizeof(tlineSignals[0]);
	AMemSetSize(signalBlock, nSignals * sizeof(AObjSignalRecord));
	pSignal = (AObjSignalRecord *)AMemLock(signalBlock);
	for (i = 0; i < nSignals; i++) {
		pSignal[i].theID = i;
		lstrcpy(pSignal[i].theName, tlineSignals[i]);
	}
	AMemUnlock(signalBlock);
	return A_OK;
}


/*************************************************************************
**	Function:
**      CheckObject             (TLineAdmin)
**
**	Description:
**      Verifies that the TLine Object is ready to be compiled.
*/

#pragma argsused
static LONG CheckObject(OBJECTID oiTLine, pAObjMessage theSystem) {
	LPTLINE	lpTLine;
	LONG	lResult = A_OK;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		if (!lpTLine->oiPhone)
			lResult = AObjReportError(oiTLine, theSystem->message1, A_ERROR,
				"No text object attached to the TLine Object "
				"to hold the PBX Extension for this Line.", 0);
		else if (!lpTLine->oiOwner)
			lResult = AObjReportError(oiTLine, theSystem->message1, A_ERROR,
				"This TLine object has no owning TDevice attached.", 0);
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return lResult;
	}
	return AObjReportError(oiTLine, theSystem->message1, A_ERROR,
		"No data block could be found on this TLine Object during check.", 0);
}

/*************************************************************************
**	Function:
**      EditObject              (TLineAdmin)
**
**	Description:
**		Internally used to edit the data members of the TLine
**		Object at design time.
*/

#pragma argsused
static LONG EditObject(OBJECTID oiTLine, pAObjMessage theSystem) {
	DLGPROC dlgProc;
	BOOL	fResult;

	dlgProc = (DLGPROC)MakeProcInstance((FARPROC)TLineSetup, hInstance);
	fResult = DialogBoxParam(hInstance, (LPCSTR)"TLineObjectEdit",
        GetActiveWindow(), dlgProc, (LPARAM)oiTLine);
	FreeProcInstance((FARPROC)dlgProc);
	return fResult? A_MODIFIED : A_OK;
}

/*************************************************************************
**	Function:
**      TLineSetup			  	(Windows Kernal)
**
**	Description:
**      This is the setup dialog box handler for the TLine Object.
*/

BOOL CALLBACK TLineSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch(msg) {
        case WM_INITDIALOG:
			return InitDialog(hwnd, (OBJECTID)lParam);

		case WM_COMMAND: //!!PORT!!
            switch(wParam) {
                case IDOK:
					return UpdateObject(hwnd);

				case IDCANCEL:
					EndDialog (hwnd, FALSE);
					return TRUE;
			}
            break;
    }
    return FALSE;
}

/*************************************************************************
**	Function:
**		InitDialog				(TLineSetup)
**
**	Description:
**		Initialize the TLine Object's edit dialog box.
*/

static BOOL InitDialog(HWND hDlg, OBJECTID oiTLine) {
	LPTLINE		lpTLine;
	OBJECTID	oiToAdd;
	HWND		hText;
	long		key;
	char		buffer[40];
	int 		objectIndex;

	SetWindowLong(hDlg, DWL_USER, (LONG)oiTLine);
	hText = GetDlgItem(hDlg, IDC_TITLE);
	AUtlSetTitleFont(OTYPE_TLIN, NULL, hText);
	lpTLine = (LPTLINE)AObjLockData(oiTLine, 0);
	if (!lpTLine)
		return TRUE;
	key = AOBJ_GETFIRSTKEY;
	while((oiToAdd = AObjGetNextObject(OTYPE_TEXT, &key)) != 0) {
		AObjGetName(oiToAdd, buffer, 40);
		objectIndex = (int)SendDlgItemMessage(hDlg, IDC_PBXEXTENSION,
			CB_ADDSTRING, 0, (LPARAM)buffer);
		SendDlgItemMessage(hDlg, IDC_PBXEXTENSION, CB_SETITEMDATA,
			objectIndex, (LPARAM)oiToAdd);
		if (oiToAdd == lpTLine->oiPhone)
			SendDlgItemMessage(hDlg, IDC_PBXEXTENSION, CB_SETCURSEL,
				objectIndex, 0);
	}
	SendDlgItemMessage(hDlg, IDC_LINECHAR, WM_SETTEXT, 0,
		(LPARAM)(LPCSTR)lpTLine->szLineChar);
	SendDlgItemMessage(hDlg, IDC_REDLED, BM_SETCHECK,
		lpTLine->fRedLEDActv? TRUE : FALSE, 0L);
	SendDlgItemMessage(hDlg, IDC_GREENLED, BM_SETCHECK,
		lpTLine->fGreenLEDActv? TRUE : FALSE, 0L);
	AObjUnlockData(oiTLine, 0);
	return TRUE;
}

/*************************************************************************
**	Function:
**		UpdateObject			(TLineSetup)
**
**	Description:
**		Updates the TLine Object's data.
*/

static BOOL UpdateObject(HWND hDlg) {
	LPTLINE		lpTLine;
	OBJECTID	oiTLine;
	int			index;

	oiTLine = (OBJECTID)GetWindowLong (hDlg, DWL_USER);
	lpTLine = (LPTLINE)AObjLockData(oiTLine, 0);
	if (!lpTLine)
		EndDialog (hDlg, FALSE);
    else {
		index = (int)SendDlgItemMessage(hDlg, IDC_PBXEXTENSION,
			CB_GETCURSEL, 0, 0L);
		if (index == CB_ERR)
			lpTLine->oiPhone = (OBJECTID)0L;
		else
			lpTLine->oiPhone = (OBJECTID)SendDlgItemMessage(hDlg,
				IDC_PBXEXTENSION, CB_GETITEMDATA, index, 0L);
		lpTLine->fRedLEDActv = (BOOL)SendDlgItemMessage(hDlg,
			IDC_REDLED, BM_GETCHECK, 0, 0L);
		lpTLine->fGreenLEDActv = (BOOL)SendDlgItemMessage(hDlg,
			IDC_GREENLED, BM_GETCHECK, 0, 0L);
		SendDlgItemMessage(hDlg, IDC_LINECHAR, WM_GETTEXT, 2,
			(LPARAM)lpTLine->szLineChar);
		AObjUnlockData(oiTLine, 0);
		EndDialog(hDlg, TRUE);
    }
	return TRUE;
}

/*************************************************************************
**	Function:
**      ValidateObjectIDs       (TLineAdmin)
**
**	Description:
**		Updates the stored Object IDs members.
*/

#pragma argsused
static LONG ValidateObjectIDs(OBJECTID oiTLine, pAObjMessage theSystem) {
	LPTLINE	lpTLine;

	lpTLine = (LPTLINE)AObjLockData(oiTLine, TLINE_MAINBLK);
	if (lpTLine) {
		AObjValidateID(&lpTLine->oiPhone, theSystem);
		AObjValidateID(&lpTLine->oiOwner, theSystem);
		AObjUnlockData(oiTLine, TLINE_MAINBLK);
		return A_OK;
	}
	return AObjReportError(oiTLine, theSystem->message1, A_ERROR,
		"No data block associated with this TLine Object during "
		"data block validation.", 0);
}

