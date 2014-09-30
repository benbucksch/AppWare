/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

/*
 * ========================================================================
 *	Include files
 * ========================================================================
 */
#include "features.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <setjmp.h>

#include <a_alm.h>		/* declares AppWare Bus API prototypes			*/
#include <o_text.h> 	/* kdeclares OTYPE_TEXT and text object ALMs	*/
#include <o_list.h>

#define NWWIN
#include <constant.h>
#include <nwcalls.h>
#include <nwnet.h>
#include <nwintcon.h>
#include <dbcsmac.h>
#include <nwundoc.h>
#include "utils.h"
#include "alm_auth.h"
#include "errors.h"
#include "resource.h"
#include "parse.h"
#include "authwin.h"

#ifdef __cplusplus

extern "C" {

#endif

#include <dstools.h>

#ifdef __cplusplus

}

#endif

/*
 * ======================================================================
 * Macro Definitions
 * ======================================================================
 */

/*
 * ======================================================================
 *	PromptedLogin module prototypes
 * ======================================================================
 */

/* Function prototypes */
extern WORD DoScript(BYTE *ScriptFile, WORD scripttype, WORD *exitflag,
	AMEMBLOCKID *Errors, WORD output, PSPACE *p);
extern WORD DoLoginScripts (NWCONN_HANDLE ConnectionID,BYTE *UserName,
	WORD ScriptType, BYTE *InputScript, BYTE *InputProfile, 
	AMEMBLOCKID *output, PSPACE *parsevars);

WORD ScriptDialogBox(SWNDEXTRA *p);
BOOL CALLBACK ScriptDialogBoxProc(HWND hDlg,UINT message,UINT wParam,
		LONG lParam);

WORD LoginScriptDialogBox(SWNDEXTRA *p);
BOOL CALLBACK LoginScriptDialogBoxProc(HWND hDlg,UINT message,
		UINT wParam,LONG lParam);

WORD PauseDialogBox(HWND ScriptWin);
BOOL CALLBACK PauseDialogBoxProc (HWND hDlg,UINT message,UINT wParam,
		LONG lParam);

/*
 * ==========================================================================
 * Global Variables
 * ==========================================================================
 */
extern HINSTANCE hInstance;

WORD DoWinScript (BYTE *SFile, WORD *eflag, AMEMBLOCKID *Errs, PSPACE *pg)
{
	SWNDEXTRA *extra;
	WORD doreturn;

	extra = new SWNDEXTRA;

	pg->WinOutput = 1;
	lstrcpy((char *)extra->ScriptFile,(char *)SFile);
	extra->exitflag = *eflag;
	extra->Errors = Errs;
	extra->pglobals = pg;

	ScriptDialogBox(extra);

	*eflag = extra->exitflag;
	doreturn = extra->doreturn;

	delete extra;
	return(doreturn);
}

WORD DoLoginWinScripts (NWCONN_HANDLE ConnID, BYTE *username, WORD stype,
	BYTE *script, BYTE *profile, AMEMBLOCKID *output, PSPACE *pg)
{
	WORD	doreturn;
	SWNDEXTRA *extra;

	extra = new SWNDEXTRA;

	extra->doreturn = 0;
	pg->WinOutput = 1;
	extra->ScriptconnID = ConnID;
	lstrcpy((char *)extra->ScriptUserName,(char *)username);
	extra->Scripttype = stype;
	lstrcpy((char *)extra->ScriptInputScript,(char *)script);
	lstrcpy((char *)extra->ScriptInputProfile,(char *)profile);
	extra->pglobals = pg;
	extra->Errors = output;

	LoginScriptDialogBox(extra);

	doreturn = extra->doreturn;

	delete extra;
	return(doreturn);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ScriptDialogBox() )

 NAME        : ScriptDialogBox(HINSTANCE hInst)

 DESCRIPTION : Initializes the Script dialog box procedure

 SYNOPSIS    : 1) Make a proc instance of the dialog box proc
               2) Create the dialog box
               3) After returning from the proc, free the proc

 PARAMETERS  : -> input   <- output
               -> hInst (the application handle)

 RETURN      : This procedure returns the value returned by the dialog proc

 END_MANUAL_ENTRY
*****************************************************************************/
WORD ScriptDialogBox (SWNDEXTRA *p)
{
	HWND hwnd;
	WORD ReturnValue = -1;

	hwnd = GetActiveWindow();
	
	ReturnValue = DialogBoxParam(hInstance,(LPSTR)"SCRIPT_DIALOG",hwnd,
		ScriptDialogBoxProc, (DWORD)p);

	return(ReturnValue);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ScriptDialogBoxProc() )

 NAME        : ScriptDialogBoxProc(HWND hDlg,UINT message,UINT wParam,LONG lParam)

 DESCRIPTION : The dialog box callback procedure.  This fills the user
 		information into the dialog box and allows the user to edit these
		fields.

 PARAMETERS  : -> input   <- output
               -> hDlg
               -> message
               -> wParam
               -> lParam

 RETURN      : TRUE or FALSE, depending on mouse clicks, etc.

 END_MANUAL_ENTRY
*****************************************************************************/
#pragma argsused
BOOL CALLBACK ScriptDialogBoxProc (HWND hDlg,UINT message,UINT wParam,
		LONG lParam)
{

	int					wWidth;
	int					wHeight;
	int					sWidth;
	int					sHeight;
	int					xPos;
	int					yPos;

	RECT					screenrect;
	RECT					ftlrect;

	WINDOWPLACEMENT	wndPlace;

	SWNDEXTRA			*p;

	switch (message)
	{
	case WM_INITDIALOG:
		p = (SWNDEXTRA *)lParam;
		p->pglobals->ScriptWin = hDlg;
		SetWindowLong(hDlg,DWL_USER,(long)p);

		GetWindowRect(GetDesktopWindow(),&screenrect);
		GetWindowRect(hDlg,&ftlrect);
		wWidth = ftlrect.right - ftlrect.left;
		wHeight = ftlrect.bottom - ftlrect.top;
		sWidth = screenrect.right - screenrect.left;
		sHeight = screenrect.bottom - screenrect.top;
		xPos = (sWidth / 2) - (wWidth / 2);
		yPos = (sHeight / 2) - (wHeight / 2);
		wndPlace.length = sizeof(WINDOWPLACEMENT);
		wndPlace.flags = WPF_SETMINPOSITION;
		wndPlace.showCmd = SW_SHOW;
		wndPlace.ptMinPosition.x = xPos;
		wndPlace.ptMinPosition.y = yPos;
		wndPlace.ptMaxPosition.x = 0;
		wndPlace.ptMaxPosition.y = 0;
		SetRect(&wndPlace.rcNormalPosition,xPos,yPos,xPos + wWidth,
			yPos + wHeight);
		SetWindowPlacement(hDlg,&wndPlace);
		EnableWindow(GetDlgItem(hDlg,IDOK),FALSE);	
		PostMessage(hDlg,SCRIPT_DOPROCESS,0,0);
		break;

	case SCRIPT_DOPROCESS:
		p = (SWNDEXTRA *)GetWindowLong(hDlg,DWL_USER);
		p->doreturn = DoScript(p->ScriptFile,0,&p->exitflag,p->Errors,
			p->pglobals->WinOutput,p->pglobals);
		EnableWindow(GetDlgItem(hDlg,IDOK),TRUE);	
		break;

	case WM_COMMAND:
	{
		WORD id = GET_WM_COMMAND_ID(wParam, lParam);
//		switch (LOWORD(wParam))
		switch (id)
		{
		case IDOK:
//			EndDialog(hDlg,LOWORD(wParam));
			EndDialog(hDlg, id);
			break;
		}
		break;
	}

	default:
		return(FALSE);
	}
	return(TRUE);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LoginScriptDialogBox() )

 NAME        : LoginScriptDialogBox(HINSTANCE hInst)

 DESCRIPTION : Initializes the Script dialog box procedure

 SYNOPSIS    : 1) Make a proc instance of the dialog box proc
               2) Create the dialog box
               3) After returning from the proc, free the proc

 PARAMETERS  : -> input   <- output
               -> hInst (the application handle)

 RETURN      : This procedure returns the value returned by the dialog proc

 END_MANUAL_ENTRY
*****************************************************************************/
WORD LoginScriptDialogBox (SWNDEXTRA *p)
{
	HWND hwnd;
	WORD ReturnValue = -1;

	hwnd = GetActiveWindow();

	ReturnValue = DialogBoxParam(hInstance,(LPSTR)"SCRIPT_DIALOG",hwnd,
		LoginScriptDialogBoxProc, (DWORD)p);

	return(ReturnValue);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LoginScriptDialogBoxProc() )

 NAME        : LoginScriptDialogBoxProc(HWND hDlg,UINT message,UINT wParam,LONG lParam)

 DESCRIPTION : The dialog box callback procedure.  This fills the user
 		information into the dialog box and allows the user to edit these
		fields.

 PARAMETERS  : -> input   <- output
               -> hDlg
               -> message
               -> wParam
               -> lParam

 RETURN      : TRUE or FALSE, depending on mouse clicks, etc.

 END_MANUAL_ENTRY
*****************************************************************************/
#pragma argsused
BOOL CALLBACK LoginScriptDialogBoxProc (HWND hDlg,UINT message,
		UINT wParam,LONG lParam)
{
	int					wWidth;
	int					wHeight;
	int					sWidth;
	int					sHeight;
	int					xPos;
	int					yPos;

	RECT					screenrect;
	RECT					ftlrect;

	WINDOWPLACEMENT	wndPlace;

	SWNDEXTRA			*p;

	switch (message)
	{
	case WM_INITDIALOG:
		p = (SWNDEXTRA *)lParam;
		p->pglobals->ScriptWin = hDlg;
		SetWindowLong(hDlg,DWL_USER,(long)p);

		GetWindowRect(GetDesktopWindow(),&screenrect);
		GetWindowRect(hDlg,&ftlrect);
		wWidth = ftlrect.right - ftlrect.left;
		wHeight = ftlrect.bottom - ftlrect.top;
		sWidth = screenrect.right - screenrect.left;
		sHeight = screenrect.bottom - screenrect.top;
		xPos = (sWidth / 2) - (wWidth / 2);
		yPos = (sHeight / 2) - (wHeight / 2);
		wndPlace.length = sizeof(WINDOWPLACEMENT);
		wndPlace.flags = WPF_SETMINPOSITION;
		wndPlace.showCmd = SW_SHOW;
		wndPlace.ptMinPosition.x = xPos;
		wndPlace.ptMinPosition.y = yPos;
		wndPlace.ptMaxPosition.x = 0;
		wndPlace.ptMaxPosition.y = 0;
		SetRect(&wndPlace.rcNormalPosition,xPos,yPos,xPos + wWidth,
			yPos + wHeight);
		SetWindowPlacement(hDlg,&wndPlace);
		EnableWindow(GetDlgItem(hDlg,IDOK),FALSE);	
		PostMessage(hDlg,SCRIPT_DOPROCESS,0,0);
		break;

	case SCRIPT_DOPROCESS:
		p = (SWNDEXTRA *)GetWindowLong(hDlg,DWL_USER);
		p->doreturn = DoLoginScripts(p->ScriptconnID,p->ScriptUserName,
			p->Scripttype,p->ScriptInputScript,p->ScriptInputProfile,
			p->Errors,p->pglobals);
		EnableWindow(GetDlgItem(hDlg,IDOK),TRUE);	
		break;

	case WM_COMMAND: 
	{
		WORD id = GET_WM_COMMAND_ID(wParam, lParam);

//		switch (LOWORD(wParam))
		switch (id)
		{
		case IDOK:
//			EndDialog(hDlg,LOWORD(wParam));
			EndDialog(hDlg, id);
			break;
		}
		break;
	}

	default:
		return(FALSE);
	}
	return(TRUE);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( PauseDialogBox(ScriptWin) )

 NAME        : PauseDialogBox(HINSTANCE hInst)

 DESCRIPTION : Initializes the Script dialog box procedure

 SYNOPSIS    : 1) Make a proc instance of the dialog box proc
               2) Create the dialog box
               3) After returning from the proc, free the proc

 PARAMETERS  : -> input   <- output
               -> hInst (the application handle)

 RETURN      : This procedure returns the value returned by the dialog proc

 END_MANUAL_ENTRY
*****************************************************************************/
WORD PauseDialogBox (HWND ScriptWin)
{
	HWND hwnd;
	WORD ReturnValue = -1;

	hwnd = GetActiveWindow();
	
	ReturnValue = DialogBoxParam(hInstance,(LPSTR)"PAUSE_DIALOG",hwnd,
		PauseDialogBoxProc, (long)ScriptWin);

	return(ReturnValue);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( PauseDialogBoxProc() )

 NAME        : PauseDialogBoxProc(HWND hDlg,UINT message,UINT wParam,LONG lParam)

 DESCRIPTION : The dialog box callback procedure.  This fills the user
 		information into the dialog box and allows the user to edit these
		fields.

 PARAMETERS  : -> input   <- output
               -> hDlg
               -> message
               -> wParam
               -> lParam

 RETURN      : TRUE or FALSE, depending on mouse clicks, etc.

 END_MANUAL_ENTRY
*****************************************************************************/
#pragma argsused
BOOL CALLBACK PauseDialogBoxProc (HWND hDlg,UINT message,UINT wParam,
		LONG lParam)
{
	int					wWidth;
	int					wHeight;

	RECT					drect;
	RECT					prect;

	WINDOWPLACEMENT	wndPlace;

	HWND					ScriptWin;

	switch (message)
	{
	case WM_INITDIALOG:
		ScriptWin = (HWND)lParam;
		GetWindowRect(GetDlgItem(ScriptWin,SCRIPT_DIALOG_BOX),&drect);
		GetWindowRect(hDlg,&prect);
		wWidth = prect.right - prect.left;
		wHeight = prect.bottom - prect.top;
		wndPlace.length = sizeof(WINDOWPLACEMENT);
		wndPlace.flags = WPF_SETMINPOSITION;
		wndPlace.showCmd = SW_SHOW;
		wndPlace.ptMinPosition.x = drect.left;
		wndPlace.ptMinPosition.y = drect.bottom;
		wndPlace.ptMaxPosition.x = 0;
		wndPlace.ptMaxPosition.y = 0;
		SetRect(&wndPlace.rcNormalPosition,drect.left, drect.bottom + 2,
			drect.left + wWidth, drect.bottom + 2 + wHeight);
		SetWindowPlacement(hDlg,&wndPlace);
		break;

	case WM_COMMAND: 
	{
		WORD id = GET_WM_COMMAND_ID(wParam, lParam);

//		switch (LOWORD(wParam))
		switch (id)
		{
		case IDCONTINUE:
//			EndDialog(hDlg,LOWORD(wParam));
			EndDialog(hDlg, id);
			break;
		}
		break;
	}

	default:
		return(FALSE);
	}
	return(TRUE);
}


