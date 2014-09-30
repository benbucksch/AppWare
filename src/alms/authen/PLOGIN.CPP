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

/* Flows */
#define PLOGIN_DS_OK			1		/* NDS login done.				*/
#define PLOGIN_DS_OK_PE		2		/* NDS login done.				*/
#define PLOGIN_BINDERY_OK		3		/* Bindery login done.			*/
#define PLOGIN_BINDERY_OK_PE	4		/* Bindery login done.			*/
#define PLOGIN_CANCEL			5		/* User cancelled request		*/
#define PLOGIN_ERROR			6		/* Error								*/

/*
 * ======================================================================
 *	PromptedLogin module prototypes
 * ======================================================================
 */

/* Function prototypes */
extern WORD DoLogin(BYTE *ServerName, BYTE *UserName, BYTE *Password,
	WORD LoginType, BYTE *Script, BYTE *Profile, AMEMBLOCKID *Output);

WORD LoginDialogBox(LWNDEXTRA *p);
BOOL CALLBACK LoginDialogBoxProc(HWND hDlg,UINT message,UINT wParam,
		LONG lParam);

#if LOGIN IS_DISABLED
extern HINSTANCE hInstance;
#endif

#if LOGIN IS_ENABLED
extern int _WinAllocFlag;
HINSTANCE hInstance;

#pragma argsused
int  WinMain (HINSTANCE hinst, HINSTANCE previnst, LPSTR lpCmdLine, int nCmdShow)
{
	struct locals
	{														
		HCURSOR				Oldcursor;		 		/* Save original cursor			*/
		HCURSOR				Newcursor;				/* Value for new cursor			*/

		BYTE					curuser[MAX_DN_BYTES];/* Current user					*/
		BYTE					mess[2048];

		NWDSContextHandle	Context;		  			/* Current context				*/

		NWCCODE				ccode;		  			/* Return Codes 					*/

		LWNDEXTRA			extra;

		NWCONN_HANDLE		connID;

		primaryConnectionStruct	pc;

	} *l;

#ifdef __BORLANDC__

	/*------ Used by BC31 compilers to turn off memory suballocation ------*/
	_WinAllocFlag = GMEM_SHARE;

#endif


	if (previnst != 0)
	{
		return(FALSE);
	}
	hInstance = hinst;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	FillMemory(l->extra.ServerName,0,sizeof(l->extra.ServerName));
	FillMemory(l->extra.UserName,0,sizeof(l->extra.UserName));
	FillMemory(l->extra.Password,0,sizeof(l->extra.Password));
	lstrcpy((char *)l->extra.Script,DSALMGetMessage(SCRIPT_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess)));
	lstrcpy((char *)l->extra.Profile,DSALMGetMessage(PROFILE_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess)));

	/*
	 * If we are DS authenticated, then default the
	 * user name to the current user's name.
	 */
	if (NWIsDSAuthenticated())
	{
		if ((l->Context = NWDSCreateContext()) != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			NWDSWhoAmI(l->Context,(char *)l->curuser);
			NWDSRemoveAllTypes(l->Context,(char *)l->curuser,(char *)l->curuser);
			lstrcpy((char *)l->extra.UserName,(char *)l->curuser);
			NWDSFreeContext(l->Context);
		}
	}
	else
	{
		FillMemory(&l->pc,0,sizeof(l->pc));
		FillPrimaryConnInfo(&l->pc);
		if (l->pc.userName[0] != '\0')
		{
			lstrcpy((char *)l->extra.UserName,(char *)l->pc.userName);
		}
	}
	if (NWGetPreferredDSServer(&l->connID) != 0)
	{
		l->connID = 0;
		NWGetPrimaryConnectionID(&l->connID);
		if (l->connID == 0)
		{
			if (NWGetDefaultConnectionID(&l->connID) == 0)
			{
				NWSetPrimaryConnectionID(l->connID);
			}
		}
	}
	if (l->connID != 0)
	{
		if (NWGetFileServerName(l->connID,(char *)l->extra.ServerName) != 0)
		{
			FillMemory(l->extra.ServerName,0,sizeof(l->extra.ServerName));
		}
	}

	LoginDialogBox(&l->extra);
	if (*(l->extra.ServerName) == NULL &&
			*(l->extra.UserName) == NULL &&
			*(l->extra.Password) == NULL)
	{
		goto Terminate;
	}
	if (*(l->extra.ServerName) != NULL && *(l->extra.UserName) == NULL)
	{
		lstrcpy((char *)l->extra.UserName,(char *)l->curuser);
	}
	if (strcmpi((char *)l->extra.Script,DSALMGetMessage(SCRIPT_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess))) == 0)
	{
		*(l->extra.Script) = NULL;
	}
	if (strcmpi((char *)l->extra.Profile,DSALMGetMessage(PROFILE_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess))) == 0)
	{
		*(l->extra.Profile) = NULL;
	}

	/*
	 * Switch to an hour-glass cursor and then do the authentication.  Then
	 * after coming back restore the cursor.
	 */
	l->Newcursor = LoadCursor(NULL,IDC_WAIT);
	if (l->Newcursor != NULL)
	{
		l->Oldcursor = SetCursor(l->Newcursor);
	}

	l->ccode = DoLogin(l->extra.ServerName,l->extra.UserName,l->extra.Password,
		l->extra.LoginType,l->extra.Script,l->extra.Profile,NULL);

	if (l->Newcursor != NULL)
	{
		SetCursor(l->Oldcursor);
	}
	if (l->ccode != DS_SUCCESS && l->ccode != BINDERY_SUCCESS)
	{
		DisplayIDMessage(l->ccode,NO_ICON);
		goto Terminate;
	}

Terminate:
	if (l != 0)
	{
		FillMemory(l->extra.Password,0,sizeof(l->extra.Password));
		delete l;
	}

	return(0);
}

#else

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( PromptedLogin() )

 NAME       : PromptedLogin(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  : This function will login a workstation to the
 		identified server.  This function has no inputs, but will instead
		pop up a windows dialog box to prompt the user for their tree/server,
		user name and password.
		
 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = none

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = none

 FLOW/RETURN:
		PLOGIN_DS_OK		  1	Everything is okay
		PLOGIN_BINDERY_OK	  2	Everything is okay
		PLOGIN_DIFF_USER	  3	Different user but same tree
		PLOGIN_CANCEL		  4	User cancelled out
		PLOGIN_ERROR		  5	General system error

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK PromptedLogin (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{														
		HCURSOR				Oldcursor;		 		/* Save original cursor			*/
		HCURSOR				Newcursor;				/* Value for new cursor			*/

		BYTE					curuser[MAX_DN_BYTES];/* Current user					*/
		BYTE					mess[2048];

		NWDSContextHandle	Context;		  			/* Current context				*/

		NWCCODE				ccode;		  			/* Return Codes 					*/

		LWNDEXTRA			extra;

		AMEMBLOCKID			output;

		AOBJECTID				oiOutput;

		NWCONN_HANDLE		connID;

		primaryConnectionStruct	pc;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = PLOGIN_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = PLOGIN_DS_OK;

	FillMemory(l->extra.ServerName,0,sizeof(l->extra.ServerName));
	FillMemory(l->extra.UserName,0,sizeof(l->extra.UserName));
	FillMemory(l->extra.Password,0,sizeof(l->extra.Password));
	lstrcpy((char *)l->extra.Script,DSALMGetMessage(SCRIPT_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess)));
	lstrcpy((char *)l->extra.Profile,DSALMGetMessage(PROFILE_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess)));
	l->output = 0;
	l->oiOutput = AObjCreate(OTYPE_LIST);

	/*
	 * If we are DS authenticated, then default the
	 * user name to the current user's name.
	 */
	if (NWIsDSAuthenticated())
	{
		if ((l->Context = NWDSCreateContext()) != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			NWDSWhoAmI(l->Context,(char *)l->curuser);
			NWDSRemoveAllTypes(l->Context,(char *)l->curuser,(char *)l->curuser);
			lstrcpy((char *)l->extra.UserName,(char *)l->curuser);
			NWDSFreeContext(l->Context);
		}
	}
	else
	{
		FillMemory(&l->pc,0,sizeof(l->pc));
		FillPrimaryConnInfo(&l->pc);
		if (l->pc.userName[0] != '\0')
		{
			lstrcpy((char *)l->extra.UserName,(char *)l->pc.userName);
		}
	}

	if (NWGetPreferredDSServer(&l->connID) != 0)
	{
		l->connID = 0;
		NWGetPrimaryConnectionID(&l->connID);
		if (l->connID == 0)
		{
			if (NWGetDefaultConnectionID(&l->connID) == 0)
			{
				NWSetPrimaryConnectionID(l->connID);
			}
		}
	}
	if (l->connID != 0)
	{
		if (NWGetFileServerName(l->connID,(char *)l->extra.ServerName) != 0)
		{
			FillMemory(l->extra.ServerName,0,sizeof(l->extra.ServerName));
		}
	}

	LoginDialogBox(&l->extra);
	if (*(l->extra.ServerName) == NULL &&
			*(l->extra.UserName) == NULL &&
			*(l->extra.Password) == NULL)
	{
		theSystem->message1 = PLOGIN_CANCEL;
		goto Terminate;
	}
	if (*(l->extra.ServerName) != NULL && *(l->extra.UserName) == NULL)
	{
		lstrcpy((char *)l->extra.UserName,(char *)l->curuser);
	}
	if (strcmpi((char *)l->extra.Script,DSALMGetMessage(SCRIPT_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess))) == 0)
	{
		*(l->extra.Script) = NULL;
	}
	if (strcmpi((char *)l->extra.Profile,DSALMGetMessage(PROFILE_PROMPTED_DEFAULT,(char *)l->mess,sizeof(l->mess))) == 0)
	{
		*(l->extra.Profile) = NULL;
	}

	/*
	 * Switch to an hour-glass cursor and then do the authentication.  Then
	 * after coming back restore the cursor.
	 */
	l->Newcursor = LoadCursor(NULL,IDC_WAIT);
	if (l->Newcursor != NULL)
	{
		l->Oldcursor = SetCursor(l->Newcursor);
	}

	l->ccode = DoLogin(l->extra.ServerName,l->extra.UserName,l->extra.Password,
		l->extra.LoginType,l->extra.Script,l->extra.Profile,&l->output);

	if (l->Newcursor != NULL)
	{
		SetCursor(l->Oldcursor);
	}
	if (l->ccode != DS_SUCCESS && l->ccode != BINDERY_SUCCESS)
	{
		DisplayIDMessage(l->ccode,NO_ICON);
		theSystem->message1 = PLOGIN_ERROR;
		goto Terminate;
	}
	else
	{
		theSystem->message1 = PLOGIN_DS_OK;
		if (l->ccode == BINDERY_SUCCESS)
		{
			theSystem->message1 = PLOGIN_BINDERY_OK;
		}
	}

Terminate:
	if (l != 0)
	{
		FillMemory(l->extra.Password,0,sizeof(l->extra.Password));
		if (l->oiOutput != 0 && l->output != 0)
		{
			MyOutputList(l->oiOutput,&l->output,1);
		}
		else
		{
			AObjDestroy(l->oiOutput);
		}
		delete l;
	}

	return;
}
#endif


/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LoginDialogBox() )

 NAME        : LoginDialogBox(HINSTANCE hInst)

 DESCRIPTION : Initializes the login dialog box procedure

 SYNOPSIS    : 1) Make a proc instance of the dialog box proc
               2) Create the dialog box
               3) After returning from the proc, free the proc

 PARAMETERS  : -> input   <- output
               -> hInst (the application handle)

 RETURN      : This procedure returns the value returned by the dialog proc

 END_MANUAL_ENTRY
*****************************************************************************/
WORD LoginDialogBox (LWNDEXTRA *p)
{
	HWND hwnd;
	WORD ReturnValue = -1;

	hwnd = GetActiveWindow();
	
	ReturnValue = DialogBoxParam(hInstance,(LPSTR)"LOGIN_DIALOG",hwnd,
		LoginDialogBoxProc, (DWORD)p);

	return(ReturnValue);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LoginDialogBoxProc() )

 NAME        : LoginDialogBoxProc(HWND hDlg,UINT message,UINT wParam,LONG lParam)

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
BOOL CALLBACK LoginDialogBoxProc (HWND hDlg,UINT message,UINT wParam,
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

	DWORD 				rc;

	LWNDEXTRA			*p;

	BYTE					mess[100];


	switch (message)
	{
	case WM_INITDIALOG:
		p = (LWNDEXTRA *)lParam;
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

// 		SendDlgItemMessage(hDlg,LOGIN_DIALOG_VERSION,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)DSALMGetMessage(VERSION_STRING,(char *)mess,sizeof(mess))));

 		Static_SetText(GetDlgItem(hDlg, LOGIN_DIALOG_VERSION),
			((LPCSTR)DSALMGetMessage(VERSION_STRING,(char *)mess,sizeof(mess))));

//		SendDlgItemMessage(hDlg,LOGIN_DIALOG_USER,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)p->UserName));

		Static_SetText(GetDlgItem(hDlg, LOGIN_DIALOG_USER), ((LPCSTR)p->UserName));

//		SendDlgItemMessage(hDlg,LOGIN_DIALOG_SERVER,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)p->ServerName));

		Static_SetText(GetDlgItem(hDlg, LOGIN_DIALOG_SERVER), ((LPCSTR)p->ServerName));

//		SendDlgItemMessage(hDlg,LOGIN_DEFAULT_BUTTON,BM_SETCHECK,1,
//			(LPARAM)((LPCSTR)p->UserName));

		Button_SetCheck(GetDlgItem(hDlg, LOGIN_DEFAULT_BUTTON), 1);

//		SendDlgItemMessage(hDlg,LOGIN_DIALOG_SCRIPT,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)p->Script));

		Static_SetText(GetDlgItem(hDlg, LOGIN_DIALOG_SCRIPT), ((LPCSTR)p->Script));

//		SendDlgItemMessage(hDlg,LOGIN_DIALOG_PROFILE,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)p->Profile));

		Static_SetText(GetDlgItem(hDlg, LOGIN_DIALOG_PROFILE), ((LPCSTR)p->Profile));
		break;

	case WM_COMMAND: 
	{
		WORD id = GET_WM_COMMAND_ID(wParam, lParam);

		p = (LWNDEXTRA *)GetWindowLong(hDlg,DWL_USER);
//		switch (LOWORD(wParam))
		switch (id)
		{
		case IDOK:
			GetDlgItemText(hDlg,LOGIN_DIALOG_SERVER,(LPSTR)p->ServerName,
				MAX_DN_BYTES);
			GetDlgItemText(hDlg,LOGIN_DIALOG_USER,(LPSTR)p->UserName,
				MAX_DN_BYTES);
			GetDlgItemText(hDlg,LOGIN_DIALOG_PASSWORD,(LPSTR)p->Password,
				PASSWORD_BUFFER_SIZE);
			GetDlgItemText(hDlg,LOGIN_DIALOG_SCRIPT,(LPSTR)p->Script,
				MAX_DN_BYTES);
			GetDlgItemText(hDlg,LOGIN_DIALOG_PROFILE,(LPSTR)p->Profile,
				MAX_DN_BYTES);

			p->LoginType = DEFAULT_LOGIN;

//			rc = SendDlgItemMessage(hDlg,LOGIN_DEFAULT_BUTTON,BM_GETCHECK,0,0);
			rc = Button_GetCheck(GetDlgItem(hDlg, LOGIN_DEFAULT_BUTTON));

			if (rc != 0)
			{
				p->LoginType = DEFAULT_LOGIN;
			}
			else
			{
//				rc = SendDlgItemMessage(hDlg,LOGIN_NDSONLY_BUTTON,BM_GETCHECK,
//					0,0);

				rc = Button_GetCheck(GetDlgItem(hDlg, LOGIN_NDSONLY_BUTTON));

				if (rc != 0)
				{
					p->LoginType = NDS_LOGIN;
				}
				else
				{
//					rc = SendDlgItemMessage(hDlg,LOGIN_BINDERYONLY_BUTTON,
//						BM_GETCHECK,0,0);

					rc = Button_GetCheck(GetDlgItem(hDlg, LOGIN_BINDERYONLY_BUTTON));,

					if (rc != 0)
					{
						p->LoginType = BINDERY_LOGIN;
					}
				}
			}
//			EndDialog(hDlg,LOWORD(wParam));

			EndDialog(hDlg, id);
	
			break;

		case IDCANCEL:
			p = (LWNDEXTRA *)GetWindowLong(hDlg,DWL_USER);
			FillMemory(p->ServerName,0,sizeof(p->ServerName));
			FillMemory(p->UserName,0,sizeof(p->UserName));
			FillMemory(p->Password,0,sizeof(p->Password));
			p->LoginType = DEFAULT_LOGIN;
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
