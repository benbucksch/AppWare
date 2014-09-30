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
#define PAUTHENTICATION_DS_OK			1	/* NDS login done.				*/
#define PAUTHENTICATION_DS_OK_PE		2	/* NDS login done.				*/
#define PAUTHENTICATION_BINDERY_OK		3	 /* Bindery login done.			*/
#define PAUTHENTICATION_BINDERY_OK_PE	4	 /* Bindery login done.			*/
#define PAUTHENTICATION_CANCEL			5	 /* User cancelled request		*/
#define PAUTHENTICATION_DIFF_USER		6	 /* Diff user/same tree			*/
#define PAUTHENTICATION_ERROR			7	 /* Error								*/

/*
 * ======================================================================
 *	PROMPTEDAUTHENTICATE module prototypes
 * ======================================================================
 */

/* Function prototypes */
extern WORD DoAuthenticate(BYTE *ServerName, BYTE *UserName, BYTE *Password,
	WORD Type);
//WORD CALLBACK AuthenDialogBox(AWNDEXTRA *p);
WORD AuthenDialogBox(AWNDEXTRA *p);
BOOL CALLBACK AuthenDialogBoxProc(HWND hDlg,UINT message,UINT wParam,
		LONG lParam);
WORD DoAuthenDialog(BYTE *server, BYTE *user, BYTE *pwd, WORD type,
	WORD display);

/*
 * ==========================================================================
 * Global Variables
 * ==========================================================================
 */
extern HINSTANCE hInstance;

#if LOGIN IS_DISABLED

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( PromptedAuthenticate() )

 NAME       : PromptedAuthenticate(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  : This function will authenticate a workstation to the
 		identified server.  This function has no inputs, but will instead
		pop up a windows dialog box to prompt the user for their tree/server,
		user name and password.
		
		This function will after getting the user's input call DoAuthenticate
		to perform the authentication.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = none

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = none

 FLOW/RETURN:
		PAUTHENTICATION_DS_OK		  1	Everything is okay
		PAUTHENTICATION_BINDERY_OK	  2	Everything is okay
		PAUTHENTICATION_DIFF_USER	  3	Different user but same tree
		PAUTHENTICATION_CANCEL		  4	User cancelled out
		PAUTHENTICATION_ERROR		  5	General system error

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK PromptedAuthenticate (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		WORD					rc;							/* Return code	  				*/
		BYTE					curuser[MAX_DN_BYTES];	/* Current user  				*/
		NWDSContextHandle	Context;						/* Current context			*/
		primaryConnectionStruct	pc;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = PAUTHENTICATION_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->curuser[0] = '\0';
	if (NWIsDSAuthenticated())
	{
		if ((l->Context = NWDSCreateContext()) != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			NWDSWhoAmI(l->Context,(char *)l->curuser);
			NWDSRemoveAllTypes(l->Context,(char *)l->curuser,(char *)l->curuser);
			NWDSFreeContext(l->Context);
		}
	}
	else
	{
		FillMemory(&l->pc,0,sizeof(l->pc));
		FillPrimaryConnInfo(&l->pc);
		if (l->pc.userName[0] != '\0')
		{
			lstrcpy((char *)l->curuser,(char *)l->pc.userName);
		}
	}
	
	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = PAUTHENTICATION_DS_OK;

	l->rc = DoAuthenDialog(NULL, l->curuser, NULL, DEFAULT_LOGIN, 1);
	switch (l->rc)
	{
	case PAUTHEN_CANCEL:
		theSystem->message1 = PAUTHENTICATION_CANCEL;
		break;

	case PAUTHEN_DIFF_USER:
		theSystem->message1 = PAUTHENTICATION_DIFF_USER;
		break;

	case PAUTHEN_BINDERY_OK:
		theSystem->message1 = PAUTHENTICATION_BINDERY_OK;
		break;

	case PAUTHEN_BINDERY_OK_PE:
		theSystem->message1 = PAUTHENTICATION_BINDERY_OK_PE;
		break;

	case PAUTHEN_DS_OK:
		theSystem->message1 = PAUTHENTICATION_DS_OK;
		break;

	case PAUTHEN_DS_OK_PE:
		theSystem->message1 = PAUTHENTICATION_DS_OK_PE;
		break;

	default:
		theSystem->message1 = PAUTHENTICATION_ERROR;
		break;
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return;
}
#endif

WORD DoAuthenDialog (BYTE *server, BYTE *user, BYTE *pwd, WORD type,
	WORD display)
{
	WORD 					errcode;

	struct locals
	{
		HCURSOR				Oldcursor;				/* Save original cursor			*/
		HCURSOR				Newcursor;				/* Value for new cursor			*/

		NWCCODE				ccode;

		NWDSContextHandle	Context;					/* Current context				*/

		AWNDEXTRA 			extra;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	FillMemory(l->extra.ServerName,0,sizeof(l->extra.ServerName));
	FillMemory(l->extra.UserName,0,sizeof(l->extra.UserName));
	FillMemory(l->extra.Password,0,sizeof(l->extra.Password));

	l->extra.LoginType = type;
	if (server != 0 && *server != 0)
	{
		lstrcpy((char *)l->extra.ServerName,(char *)server);
	}
	if (user != 0 && *user != 0)
	{
		lstrcpy((char *)l->extra.UserName,(char *)user);
	}
	if (pwd != 0 && (unsigned int)*pwd != 0xFF)
	{
		lstrcpy((char *)l->extra.Password,(char *)pwd);
	}
	else
	{
		*(l->extra.Password) = 0xff;
	}

	if (*(l->extra.ServerName) == NULL ||
		*(l->extra.UserName) == NULL    ||
		(unsigned int)*(l->extra.Password) == 0xFF)
	{
		AuthenDialogBox(&l->extra);
		if (server != 0)
		{
			lstrcpy((char *)server,(char *)l->extra.ServerName);
		}
		if (user != 0)
		{
			lstrcpy((char *)user,(char *)l->extra.UserName);
		}
		if (pwd != 0)
		{
			lstrcpy((char *)pwd,(char *)l->extra.Password);
		}
	}
	if (*(l->extra.ServerName) == NULL &&
			*(l->extra.UserName) == NULL &&
			*(l->extra.Password) == NULL)
	{
		errcode = PAUTHEN_CANCEL;
		goto Terminate;
	}
	if (*(l->extra.ServerName) != NULL && *(l->extra.UserName) == NULL)
	{
		if (NWIsDSAuthenticated())
		{
			if ((l->Context = NWDSCreateContext()) != (NWDSContextHandle)ERR_CONTEXT_CREATION)
			{
				NWDSWhoAmI(l->Context,(char *)l->extra.UserName);
				NWDSRemoveAllTypes(l->Context,(char *)l->extra.UserName,(char *)l->extra.UserName);
				NWDSFreeContext(l->Context);
			}
		}
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
	l->ccode = DoAuthenticate(l->extra.ServerName,l->extra.UserName,
		l->extra.Password,l->extra.LoginType);
	if (l->Newcursor != NULL)
	{
		SetCursor(l->Oldcursor);
	}
	if (l->ccode != DS_SUCCESS && l->ccode != BINDERY_SUCCESS &&
		l->ccode != DS_SUCCESS_PE && l->ccode != BINDERY_SUCCESS_PE)
	{
		if (l->ccode == DIFF_USER_SAME_TREE)
		{
			errcode = PAUTHEN_DIFF_USER;
		}
		else
		{
			if (display == 1)
			{
				DisplayIDMessage(l->ccode,NO_ICON);
			}
			errcode = PAUTHEN_ERROR;
		}
		goto Terminate;
	}
	else
	{
		switch (l->ccode)
		{
		case DS_SUCCESS:
			errcode = PAUTHEN_DS_OK;
			break;

		case DS_SUCCESS_PE:
			errcode = PAUTHEN_DS_OK_PE;
			break;

		case BINDERY_SUCCESS:
			errcode = PAUTHEN_BINDERY_OK;
			break;

		case BINDERY_SUCCESS_PE:
			errcode = PAUTHEN_BINDERY_OK_PE;
			break;

		default:
			DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
			errcode = PAUTHEN_ERROR;
			goto Terminate;
		}
	}

Terminate:
	if (l != 0)
	{
		FillMemory(l->extra.Password,0,sizeof(l->extra.Password));
		delete l;
	}

	return(errcode);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AuthenDialogBox() )

 NAME        : AuthenDialogBox(HINSTANCE hInst)

 DESCRIPTION : Initializes the authenticate dialog box procedure

 SYNOPSIS    : 1) Make a proc instance of the dialog box proc
               2) Create the dialog box
               3) After returning from the proc, free the proc

 PARAMETERS  : -> input   <- output
               -> hInst (the application handle)

 RETURN      : This procedure returns the value returned by the dialog proc

 END_MANUAL_ENTRY
*****************************************************************************/
//WORD CALLBACK AuthenDialogBox (AWNDEXTRA *p)
WORD AuthenDialogBox (AWNDEXTRA *p)
{
	HWND hwnd;
	WORD ReturnValue = -1;


	hwnd = GetActiveWindow();
	
	ReturnValue = DialogBoxParam(hInstance,(LPSTR)"AUTHEN_DIALOG",hwnd,
		AuthenDialogBoxProc, (DWORD)p);

	return(ReturnValue);
}

/*****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AuthenDialogBoxProc() )

 NAME        : AuthenDialogBoxProc(HWND hDlg,UINT message,UINT wParam,LONG lParam)

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
BOOL CALLBACK AuthenDialogBoxProc (HWND hDlg,UINT message,UINT wParam,
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

	AWNDEXTRA			*p;

#if 0
	BYTE					mess[100];
#endif

	switch (message)
	{
	case WM_INITDIALOG:
		p = (AWNDEXTRA *)lParam;
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
#if 0
//		SendDlgItemMessage(hDlg,AUTHEN_DIALOG_VERSION,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)DSALMGetMessage(VERSION_STRING,mess,sizeof(mess))));

		Static_SetText(GetDlgItem(hDlg, AUTHEN_DIALOG_VERSION), 
			((LPCSTR)DSALMGetMessage(VERSION_STRING,mess,sizeof(mess))));
#else
//		SendDlgItemMessage(hDlg,AUTHEN_DIALOG_VERSION,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)""));

		Static_SetText(GetDlgItem(hDlg, AUTHEN_DIALOG_VERSION), ((LPCSTR)""));
#endif

//		SendDlgItemMessage(hDlg,AUTHEN_DIALOG_USER,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)p->UserName));

		Static_SetText(GetDlgItem(hDlg, AUTHEN_DIALOG_USER), ((LPCSTR)p->UserName));


//		SendDlgItemMessage(hDlg,AUTHEN_DIALOG_SERVER,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)p->ServerName));

		Static_SetText(GetDlgItem(hDlg, AUTHEN_DIALOG_SERVER), ((LPCSTR)p->ServerName));


//		SendDlgItemMessage(hDlg,AUTHEN_DEFAULT_BUTTON,BM_SETCHECK,1,
//			(LPARAM)((LPCSTR)p->UserName));

		Button_SetCheck(GetDlgItem(hDlg, AUTHEN_DEFAULT_BUTTON), 1);

		break;

	case WM_COMMAND: 
	{
		WORD id = GET_WM_COMMAND_ID(wParam, lParam);

		p = (AWNDEXTRA *)GetWindowLong(hDlg,DWL_USER);
//		switch (LOWORD(wParam))
		switch (id)
		{
		case IDOK:
			GetDlgItemText(hDlg,AUTHEN_DIALOG_SERVER,(LPSTR)p->ServerName,
				sizeof(p->ServerName));
			GetDlgItemText(hDlg,AUTHEN_DIALOG_USER,(LPSTR)p->UserName,
				sizeof(p->UserName));
			GetDlgItemText(hDlg,AUTHEN_DIALOG_PASSWORD,(LPSTR)p->Password,
				sizeof(p->Password));

			p->LoginType = DEFAULT_LOGIN;
//			rc = SendDlgItemMessage(hDlg,AUTHEN_DEFAULT_BUTTON,BM_GETCHECK,0,0);
			rc = Button_GetCheck(GetDlgItem(hDlg, AUTHEN_DEFAULT_BUTTON));

			if (rc != 0)
			{
				p->LoginType = DEFAULT_LOGIN;
			}
			else
			{
//				rc = SendDlgItemMessage(hDlg,AUTHEN_NDSONLY_BUTTON,BM_GETCHECK,
//					0,0);
				rc = Button_GetCheck(GetDlgItem(hDlg, AUTHEN_NDSONLY_BUTTON));
  
				if (rc != 0)
				{
					p->LoginType = NDS_LOGIN;
				}
				else
				{
//					rc = SendDlgItemMessage(hDlg,AUTHEN_BINDERYONLY_BUTTON,
//						BM_GETCHECK,0,0);
					rc = Static_GetCheck(GetDlgItem(hDlg, AUTHEN_BINDERYONLY_BUTTON));

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
			p = (AWNDEXTRA *)GetWindowLong(hDlg,DWL_USER);
			FillMemory(p->ServerName,0,sizeof(p->ServerName));
			FillMemory(p->UserName,0,sizeof(p->UserName));
			FillMemory(p->Password,0,sizeof(p->Password));
			p->LoginType = DEFAULT_LOGIN;
//			EndDialog(hDlg, id);
			break;
		}
		break;

	}
	default:
		return(FALSE);
	}

	return(TRUE);
}
