/*                                                                        *\
    (c) 1993-1995, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE		: QCNC_DEF.H                                              |
 | DESCRIPTION	: Q+E ODBC Connection Object Definition.                  |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	08-Jun-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#include "qsql.h"
#include "qsql_rc.h"
#include "memblock.h"
#include "a_almutl.h"
#include "helpids.h"


class QCNC_DEF                 /* ---- ODBC Connection Object Definition  */
{
public:
  BOOL InitObjectDialog(HWND hDlg, AOBJECTID qcncObj);
  /* Inits ODBC Connection Object dialog. */

  BOOL UpdateObject(HWND hDlg);
  /* Updates ODBC Connection Object. */
};


BOOL ALMCALLBACK OQcncObjectDialog /* - Edit Object Dialog -------------- */
( HWND		hDlg,			// window handle
  UINT		message,		// message
  WPARAM	wParam,			// message
  LPARAM	lParam			// message
)
/* ODBC Connection Object dialog. */
{ QCNC_DEF	def;			// datb_def class
  WORD		wId;			// identifier

  switch (message)
  { case WM_INITDIALOG:
      return def.InitObjectDialog(hDlg, (AOBJECTID) lParam);
    case WM_COMMAND:
      wId = LOWORD(wParam);
      switch (wId)
      {	case IDHELP:
	  #ifdef HELPID_OBJD_Qcnc
	    return WinHelp(hDlg, "appware.hlp", HELP_CONTEXT,
			   HELPID_OBJD_Qcnc);
	  #else
	    return FALSE;
	  #endif
	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  return TRUE;
	case IDOK:
	  EndDialog(hDlg, TRUE);
	  return def.UpdateObject(hDlg);

	case IDD_AUTOCOMMIT:
	  CheckDlgButton(hDlg, IDD_AUTOCOMMIT,
			 IsDlgButtonChecked(hDlg, IDD_AUTOCOMMIT));
	  return TRUE;
      } // switch
  } // switch
  return FALSE;
} // OQcncObjectDialog

BOOL QCNC_DEF::InitObjectDialog	/* ---- Init Object Dialog -------------- */
( HWND		hDlg,			// window handle
  AOBJECTID	qcncObj			// qcnc object
)
/* Inits ODBC Connection Object dialog. */
{ MemBlock	mb;			// memblock class
  Qcnc		*qcnc;			// qcnc core info
  char		str[MXSTRSIZE];		// string

  long		objIdx;			// object index
  AOBJECTID	obj;			// object
  char		name[MXVARSIZE];	// name

  int		i;			// counter

  SetWindowLong(hDlg, DWL_USER, (LONG) qcncObj);	// saves object id
  qcnc = (Qcnc *) mb.GetPointer(qcncObj, QCNCCORE);

  /* ---- inits qcnc object type name, and qcnc object name ---- */
  AUtlSetTitleFont(OTYPE_QCNC, NULL, GetDlgItem(hDlg, IDD_OBJECT));
	AUtlCenterDialog(hDlg, 0);
  AObjGetName(qcncObj, str, sizeof(str));
  SetWindowText(hDlg, str);

  /* ---- inits username, password, and other parameters ---- */
  i = SendDlgItemMessage(hDlg, IDC_USERNAME, CB_INSERTSTRING,
			 -1, (LPARAM) "(None)");
  SendDlgItemMessage(hDlg, IDC_USERNAME, CB_SETITEMDATA, i, 0);
  if (!qcnc->usernameObj || !AObjCheckType(qcnc->usernameObj, OTYPE_TEXT))
    SendDlgItemMessage(hDlg, IDC_USERNAME, CB_SETCURSEL, i, 0);

  i = SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_INSERTSTRING,
			 -1, (LPARAM) "(None)");
  SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_SETITEMDATA, i, 0);
  if (!qcnc->passwordObj || !AObjCheckType(qcnc->passwordObj, OTYPE_TEXT))
    SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_SETCURSEL, i, 0);

  i = SendDlgItemMessage(hDlg, IDC_PARAMS, CB_INSERTSTRING,
			 -1, (LPARAM) "(None)");
  SendDlgItemMessage(hDlg, IDC_PARAMS, CB_SETITEMDATA, i, 0);
  if (!qcnc->paramsObj ||
      !AObjCheckType(qcnc->paramsObj, OTYPE_TEXT))
    SendDlgItemMessage(hDlg, IDC_PARAMS, CB_SETCURSEL, i, 0);

  objIdx = AOBJ_GETFIRSTKEY;
  while((obj = AObjGetNextObject(OTYPE_TEXT, &objIdx)) != 0)
  { AObjGetName(obj, name, sizeof(name));

    i = SendDlgItemMessage(hDlg, IDC_USERNAME, CB_INSERTSTRING,
			   -1, (LPARAM) name);
    SendDlgItemMessage(hDlg, IDC_USERNAME, CB_SETITEMDATA, i, (LPARAM) obj);
    if (qcnc->usernameObj == obj)
      SendDlgItemMessage(hDlg, IDC_USERNAME, CB_SETCURSEL, i, 0);

    i = SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_INSERTSTRING,
			   -1, (LPARAM) name);
    SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_SETITEMDATA, i, (LPARAM) obj);
    if (qcnc->passwordObj == obj)
      SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_SETCURSEL, i, 0);

    i = SendDlgItemMessage(hDlg, IDC_PARAMS, CB_INSERTSTRING,
			   -1, (LPARAM) name);
    SendDlgItemMessage(hDlg, IDC_PARAMS, CB_SETITEMDATA, i, (LPARAM) obj);
    if (qcnc->paramsObj == obj)
      SendDlgItemMessage(hDlg, IDC_PARAMS, CB_SETCURSEL, i, 0);
  } // while

  /* ---- inits auto commit ---- */
  CheckDlgButton(hDlg, IDD_AUTOCOMMIT, qcnc->isAutoCommit);
  return TRUE;
} // InitObjectDialog

BOOL QCNC_DEF::UpdateObject	/* ---- Update Object ------------------- */
( HWND		hDlg			// window handle
)
/* Updates ODBC Connection Object. */
{ MemBlock	mb;			// memblock class
  AOBJECTID	qcncObj;		// qcnc object
  Qcnc		*qcnc;			// qcnc core info

  int		i;			// counter

  qcncObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);	// gets object id
  qcnc = (Qcnc *) mb.GetPointer(qcncObj, QCNCCORE);

  /* ---- updates username ---- */
  i = SendDlgItemMessage(hDlg, IDC_USERNAME, CB_GETCURSEL, 0, 0);
  qcnc->usernameObj = (AOBJECTID)
    SendDlgItemMessage(hDlg, IDC_USERNAME, CB_GETITEMDATA, i, 0);

  /* ---- updates password ---- */
  i = SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_GETCURSEL, 0, 0);
  qcnc->passwordObj = (AOBJECTID)
    SendDlgItemMessage(hDlg, IDC_PASSWORD, CB_GETITEMDATA, i, 0);

  /* ---- updates other parameters ---- */
  i = SendDlgItemMessage(hDlg, IDC_PARAMS, CB_GETCURSEL, 0, 0);
  qcnc->paramsObj = (AOBJECTID)
    SendDlgItemMessage(hDlg, IDC_PARAMS, CB_GETITEMDATA, i, 0);

  /* ---- updates auto commit ---- */
  qcnc->isAutoCommit = IsDlgButtonChecked(hDlg, IDD_AUTOCOMMIT);
  return TRUE;
} // UpdateObject
