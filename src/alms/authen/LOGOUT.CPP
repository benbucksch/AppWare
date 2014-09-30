/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

 /*
 * This file contains the functions necessary for the Logout ALM
 * function.
 */

/*
 * ========================================================================
 *	Include files
 * ========================================================================
 */
#include "features.h"
#ifdef _Windows
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <dos.h>
#include <dir.h>
#include <setjmp.h>

#include <a_alm.h>		/* declares AppWare Bus API prototypes			*/
#include <o_text.h> 	/* kdeclares OTYPE_TEXT and text object ALMs	*/
#include <o_flag.h>
#include <o_list.h>

#define NWWIN
#include <constant.h>
#include <nwcalls.h>
#include <nwnet.h>
#include <nwintcon.h>
#include <nwlocale.h>
#include <dbcsmac.h>
#include <nwundoc.h>
#include <nwintern.h>
#include "utils.h"
#include "errors.h"
#include "alm_auth.h"
#include "resource.h"
#include "authwin.h"

#ifdef __cplusplus

extern "C" {

#endif

#include <dstools.h>
#define	WORD	unsigned short
#include <nwtools.h>

#ifdef __cplusplus

}

#endif

/*
 * ======================================================================
 * Macro Definitions
 * ======================================================================
 */

/* Flows */																			
#define LOGOUT_OKAY			1			/* Everything is okay.					*/
#define LOGOUT_FAILURE		2			/* Failure to logout.					*/

#define MAXPATHSIZE			256		/* Maximum path size						*/
#define MAXERRSTRING		1024		/* Maximum error string length		*/

/*
 * ======================================================================
 *	LOGOUT module prototypes
 * ======================================================================
 */
WORD DoLogout(BYTE *ServerName, WORD logoutTree);

/* Static function prototypes */
static WORD LogoutServer(BYTE *server,NWCONN_HANDLE lastconn,WORD lastTree);
static WORD LogoutAll(NWCONN_HANDLE lastconn,WORD lastTree);
static void EndCapConnID(NWCONN_HANDLE connID);
static void CheckCurrentDrive();
static WORD LogoutOne(NWCONN_HANDLE connectID, NWCONN_HANDLE lastconnID,
	WORD haveServer, WORD lastTree);

#if LOGIN IS_DISABLED
#if LOGOUT IS_ENABLED

extern int _WinAllocFlag;
HINSTANCE hInstance;

WORD LogoutDialogBox (LOUTEXTRA *p);
BOOL CALLBACK LogoutDialogBoxProc (HWND hDlg,UINT message,UINT wParam,
		LONG lParam);

#pragma argsused
int  WinMain (HINSTANCE hinst, HINSTANCE previnst, LPSTR lpCmdLine, int nCmdShow)
{
	struct locals
	{
		BYTE 	ServerName[MAX_DN_BYTES];				/* Server/tree passed in		*/
		BYTE	*ServerList;								/* List of servers 				*/
		BYTE	errstr[MAXERRSTRING];					/* Error string					*/
		BYTE	errmsg[MAXERRSTRING];					/* Error message pointer		*/
		BYTE	*codemsg;									/* Error message pointer		*/
		BYTE	mess[100];

		HCURSOR				Oldcursor;					/* Save original cursor			*/
		HCURSOR				Newcursor;					/* Value for new cursor			*/

		WORD					haveServer;					/* Server list given				*/
		WORD					entry;						/* item with error				*/
		WORD					rc;							/* return codes					*/

		NWCCODE				ccode;						/* Return Codes 					*/

		LOUTEXTRA			extra;
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

	l->extra.logoutTree = FALSE;
	l->ServerList = l->extra.SList;
	FillMemory(l->extra.SList,0,sizeof(l->extra.SList));
	LogoutDialogBox(&l->extra);

	if (l->extra.winreturn != IDOK)
	{
		goto Terminate;
	}

	l->haveServer = (*(l->ServerList) == NULL) ? FALSE : TRUE;

	if (l->extra.logoutTree == TRUE && l->haveServer == TRUE)
	{
		DisplayIDMessage(LOGOUT_TREE_W_SERVER,NO_ICON);
		goto Terminate;
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		goto Terminate;
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

	/*
	 * If a server list was specified, then go through the list and logout
	 * of those servers, otherwise logout of all servers.
	 */
	if (l->haveServer == TRUE)
	{
		l->entry = 1;
		l->ServerList = Gettoken(l->ServerList,l->ServerName);
		while (*(l->ServerName) != NULL)
		{
			NWLSTRUPR(l->ServerName);
			l->ccode = DoLogout(l->ServerName,l->extra.logoutTree);
			if (l->ccode != SUCCESS)
			{
				FillMemory(l->errmsg,0,sizeof(l->errmsg));
				DSALMGetMessage(LOGOUT_ERROR,l->errmsg,sizeof(l->errmsg));
				l->codemsg = DSALMGetMessage(l->ccode,l->mess,sizeof(l->mess));
				sprintf(l->errstr,l->errmsg,l->ServerName,l->codemsg);
				strcat(l->errstr,DSALMGetMessage(OK_OR_STOP,l->mess,sizeof(l->mess)));
			  	l->rc = DisplayTextMessage(l->errstr,
					(MB_ICONINFORMATION | MB_YESNO));
				if (l->rc != IDYES)
				{
					goto Terminate;
				}
			}
			l->ServerList = Gettoken(l->ServerList,l->ServerName);
		}
	}
	else
	{
		l->ccode = DoLogout("",l->extra.logoutTree);
		if (l->ccode != SUCCESS)
		{
			FillMemory(l->errmsg,0,sizeof(l->errmsg));
			DSALMGetMessage(LOGOUT_ERROR,l->errmsg,sizeof(l->errmsg));
			l->codemsg = DSALMGetMessage(l->ccode,l->mess,sizeof(l->mess));
			sprintf(l->errstr,l->errmsg,l->ServerName,l->codemsg);
			DisplayTextMessage(l->errstr,NO_ICON);
		}
	}

Terminate:
	if (l != 0)
	{
		if (l->Newcursor != NULL)
		{
			SetCursor(l->Oldcursor);
		}
		delete l;
	}

	return(0);
}

WORD LogoutDialogBox (LOUTEXTRA *p)
{
	HWND hwnd;
	WORD ReturnValue = -1;

	hwnd = GetActiveWindow();
	
	ReturnValue = DialogBoxParam(hInstance,(LPSTR)"LOGOUT_DIALOG",hwnd,
		LogoutDialogBoxProc, (DWORD)p);

	return(ReturnValue);
}

#pragma argsused
BOOL CALLBACK LogoutDialogBoxProc (HWND hDlg,UINT message,UINT wParam,
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

	LOUTEXTRA			*p;

	BYTE					mess[100];


	switch (message)
	{
	case WM_INITDIALOG:
		p = (LOUTEXTRA *)lParam;
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
// 		SendDlgItemMessage(hDlg,LOGOUT_DIALOG_VERSION,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)DSALMGetMessage(VERSION_STRING,mess,sizeof(mess))));

 		Static_SetText(GetDlgItem(hDlg,LOGOUT_DIALOG_VERSION),
 			((LPCSTR)DSALMGetMessage(VERSION_STRING,mess,sizeof(mess))));

//		SendDlgItemMessage(hDlg,LOGOUT_DIALOG_SERVER,WM_SETTEXT,0,
//			(LPARAM)((LPCSTR)p->SList));

		Static_SetText(GetDlgItem(hDlg, LOGOUT_DIALOG_SERVER), ((LPCSTR)p->SList));

//		SendDlgItemMessage(hDlg,LOGOUT_TREE_BUTTON,BM_SETCHECK,0,
//			(LPARAM)((LPCSTR)p->logoutTree));

		Button_SetCheck(GetDlgItem(hDlg, LOGOUT_TREE_BUTTON), 0);


//		SendDlgItemMessage(hDlg,LOGOUT_SERVER_BUTTON,BM_SETCHECK,1,
//			(LPARAM)((LPCSTR)p->logoutTree));

		Button_SetCheck(GetDlgItem(hDlg, LOGOUT_SERVER_BUTTON), 1);
		break;

	case WM_COMMAND: 
	{
		WORD  id = GET_WM_COMMAND_ID(wParam, lParam);

		p = (LOUTEXTRA *)GetWindowLong(hDlg,DWL_USER);
//		switch (LOWORD(wParam))

		switch (id)
		{
		case IDOK:
			p->winreturn = IDOK;
			GetDlgItemText(hDlg,LOGOUT_DIALOG_SERVER,(LPSTR)p->SList,
				MAX_SERVER_LIST);

			p->logoutTree = DEFAULT_LOGOUTTREE;

//			rc = SendDlgItemMessage(hDlg,LOGOUT_TREE_BUTTON,BM_GETCHECK,0,0);
			rc = Button_GetCheck(GetDlgItem(hDlg, LOGOUT_TREE_BUTTON));

			if (rc != 0)
			{
				p->logoutTree = TRUE;
			}
			else
			{
				p->logoutTree = FALSE;
			}
//			EndDialog(hDlg,LOWORD(wParam));
			EndDialog(hDlg, id);
	
			break;

		case IDCANCEL:
			p = (LOUTEXTRA *)GetWindowLong(hDlg,DWL_USER);
			p->winreturn = IDCANCEL;
			FillMemory(p->SList,0,sizeof(p->SList));
			p->logoutTree = DEFAULT_LOGOUTTREE;
//			EndDialog(hDlg,LOWORD(wParam));
			EndDialog(hDlg, id);
			break;

		case LOGOUT_TREE_BUTTON:
		{
			WORD cmd = GET_WM_COMMAND_CMD(wParam, lParam);

//			switch (HIWORD(lParam))
			switch (cmd)
			{
			case BN_CLICKED:
	  			GetDlgItemText(hDlg,LOGOUT_DIALOG_SERVER,(LPSTR)p->SList,
					MAX_SERVER_LIST);

//				SendDlgItemMessage(hDlg,LOGOUT_DIALOG_SERVER,WM_SETTEXT,0,
//					(LPARAM)((LPCSTR)""));

				Static_SetText(GetDlgItem(hDlg, LOGOUT_DIALOG_SERVER), ((LPCSTR)""));

				EnableWindow(GetDlgItem(hDlg,LOGOUT_DIALOG_SERVER),FALSE);	
				UpdateWindow(hDlg);
				break;
			}
			break;
		}

		case LOGOUT_SERVER_BUTTON:
		{
			WORD cmd = GET_WM_COMMAND_CMD(wParam, lParam);

//			switch (HIWORD(lParam))
			switch (cmd)
			{
			case BN_CLICKED:

//				SendDlgItemMessage(hDlg,LOGOUT_DIALOG_SERVER,WM_SETTEXT,0,
//					(LPARAM)((LPCSTR)p->SList));

				Static_SetText(GetDlgItem(hDlg, LOGOUT_DIALOG_SERVER), ((LPCSTR)p->SList));


				EnableWindow(GetDlgItem(hDlg,LOGOUT_DIALOG_SERVER),TRUE);	
				UpdateWindow(hDlg);
				break;
			}
			break;
		}
		break;
		}

	default:
		return(FALSE);
	}
	}

	return(TRUE);
}

#else

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( Logout() )

 NAME       : Logout(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  :  This function will log the user out of the Novell Network.
 		The function will look for two parameters.  The first being the name
		of the server that the user wishes to log out of, and the second being
		a flag signalling whether the user wishes to log out of the NDS tree
		or all servers.  If the flag is set to TRUE, meaning that only NDS
		disconnects are desired, then the server parameter is ignored and the 
		code will search for the DS connection and drop it.  If the flag is
		set to FALSE, meaning that all types of connections are dropped, then
		the code will look at the server identified.  If the server is specified
		then that connection is dropped.  If the server value is empty, then 
		all connections are dropped.
		
		If the flag is unspecified it will default to FALSE -- dropping all
		connections.  The server parameter may contain wildcard characters.

		If the user does not have a connection to the desired server/tree
		then this function still reports that the logout was successful.
												  
		When the user logs out of the entire system then the logout must
		drop the default connection last.  This is because the system
		wishes to record the logout times to the user records in the 
		bindery/tree in the default connection.  If we drop this
		connection before we are through then the system will re-establish the
		connection in order to record the last disconnect time.  We save this
		extra time by not disconnecting the default connection until
		the very end.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = TEXT...optional...ServerList, default=all
					GET2 = FLAG...optional...LogoutTree, default=FALSE

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = LIST...optional...Errorlist output of line entry
														failures

 FLOW/RETURN:
		LOGOUT_OKAY			1 = Everything is okay.
		LOGOUT_FAILURE		2 = Problem logging out

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK Logout (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE 	ServerName[MAX_DN_BYTES];				/* Server/tree passed in		*/
		BYTE	*ServerList;								/* List of servers 				*/
		BYTE	*SLptr;
		BYTE	errstr[MAXERRSTRING];					/* Error string					*/
		BYTE	errmsg[MAXERRSTRING];					/* Error message pointer		*/
		BYTE	*codemsg;									/* Error message pointer		*/
		BYTE	mess[100];

		HCURSOR				Oldcursor;					/* Save original cursor			*/
		HCURSOR				Newcursor;					/* Value for new cursor			*/

		AMEMBLOCKID			ErrorList;					/* Error list for output		*/

		UINT					listsize;					/* Size of server list			*/

		WORD					haveServer;					/* Server list given				*/
		WORD					logoutTree;					/* Server is really a tree		*/
		WORD					entry;						/* item with error				*/
		WORD					rc;							/* return codes					*/

		NWCCODE				ccode;						/* Return Codes 					*/

		AOBJECTID				oiServerList;				/* Server ALM object				*/
		AOBJECTID				oiLogoutTree;				/* Server ALM object				*/
		AOBJECTID				oiErrorOutput;				/* Server ALM object				*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = LOGOUT_FAILURE;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = LOGOUT_OKAY;
	l->ErrorList = 0;

  /*
   * Create output error list object.
   */
  l->oiErrorOutput = AObjCreate(OTYPE_LIST);

	/*
	 * Retrieve the logout parameters from the Visual Appbuilder.  The 
	 * server/tree and the flag are both optional.  Retrieve their values
	 * and set the defaults if necessary.
	 */
	l->haveServer = FALSE;
	l->oiServerList = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (l->oiServerList != 0)
	{
		l->listsize = OTextGetLength(l->oiServerList) + 1;
		l->ServerList = l->SLptr = (BYTE *)malloc(l->listsize);
		if (l->ServerList == NULL)
		{
			DisplayIDMessage(MEMORY_ERROR,NO_ICON);
			theSystem->message1 = LOGOUT_FAILURE;
			goto Terminate;
		}
		FillMemory(l->ServerList,0,l->listsize);
		OTextGetToBuffer(l->oiServerList,(char *)l->ServerList,l->listsize);
		l->haveServer = (*(l->ServerList) == NULL) ? FALSE : TRUE;
	}

	l->logoutTree = FALSE;
	l->oiLogoutTree = AFuncGetTypedParameter(2, OTYPE_FLAG);
	if (l->oiLogoutTree != 0)
	{
		l->logoutTree = (OFlagGet(l->oiLogoutTree) != 0) ? TRUE : FALSE;
	}

	if (l->logoutTree == TRUE && l->haveServer == TRUE)
	{
		DisplayIDMessage(LOGOUT_TREE_W_SERVER,NO_ICON);
		theSystem->message1 = LOGOUT_FAILURE;
		goto Terminate;
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		theSystem->message1 = LOGOUT_FAILURE;
		goto Terminate;
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

	/*
	 * If a server list was specified, then go through the list and logout
	 * of those servers, otherwise logout of all servers.
	 */
	if (l->haveServer == TRUE)
	{
		l->entry = 1;
		l->ServerList = Gettoken(l->ServerList,l->ServerName);
		while (*(l->ServerName) != NULL)
		{
			NWLSTRUPR((char *)l->ServerName);
			l->ccode = DoLogout(l->ServerName,l->logoutTree);
			if (l->ccode != SUCCESS)
			{
				theSystem->message1 = LOGOUT_FAILURE;
				FillMemory(l->errmsg,0,sizeof(l->errmsg));
				DSALMGetMessage(LOGOUT_ERROR,(char *)l->errmsg,sizeof(l->errmsg));
				l->codemsg = (unsigned char *)DSALMGetMessage(l->ccode,(char *)l->mess,sizeof(l->mess));
				sprintf((char *)l->errstr,(char *)l->errmsg,l->ServerName,l->codemsg);
				strcat((char *)l->errstr,DSALMGetMessage(OK_OR_STOP,(char *)l->mess,sizeof(l->mess)));
				sprintf((char *)l->errmsg,"%d",l->entry);
				AddToList(&l->ErrorList,l->errmsg);
				l->rc = DisplayTextMessage((char *)l->errstr,
					(MB_ICONINFORMATION | MB_YESNO));
				if (l->rc != IDYES)
				{
					goto Terminate;
				}
			}
			l->ServerList = Gettoken(l->ServerList,l->ServerName);
			l->entry++;
		}
	}
	else
	{
		l->ccode = DoLogout((unsigned char *)"",l->logoutTree);
		if (l->ccode != SUCCESS)
		{
			FillMemory(l->errmsg,0,sizeof(l->errmsg));
			DSALMGetMessage(LOGOUT_ERROR,(char *)l->errmsg,sizeof(l->errmsg));
			l->codemsg = (unsigned char *)DSALMGetMessage(l->ccode,(char *)l->mess,sizeof(l->mess));
			sprintf((char *)l->errstr,(char *)l->errmsg,l->ServerName,l->codemsg);
			DisplayTextMessage((char *)l->errstr,NO_ICON);
			theSystem->message1 = LOGOUT_FAILURE;
		}
	}

Terminate:
	if (l != 0)
	{
		if (l->oiErrorOutput != 0 && l->ErrorList != 0)
		{
			MyOutputList(l->oiErrorOutput,&l->ErrorList,1);
		}
		else
		{
			AObjDestroy(l->oiErrorOutput);
		}
		if (l->Newcursor != NULL)
		{
			SetCursor(l->Oldcursor);
		}
		if (l->SLptr != NULL)
		{
			free(l->SLptr);
		}
		delete l;
	}

	return;
}
#endif
#endif

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DoLogout() )

 NAME         : DoLogout (BYTE *ServerName, WORD logoutTree)

 DESCRIPTION  : This will log the user out of the specified server.  If the
 		logoutTree flag is TRUE, then the logout will not happen unless the
		server is a DS server.

 PARAMETERS   :   -> input          <-output 
                ->  ServerName	= server to logout from
					  ->	logoutTree	= flag to log DS only

 RETURN       :  WORD
 		NO_DEFAULT_CONN 	= No default connection
		SYSTEM_ERROR	= General system error
		MEMORY_ERROR	= Could not allocate memory

 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD DoLogout (BYTE *ServerName, WORD logoutTree)
{
	WORD					errcode;						/* Return Codes 					*/
	struct locals
	{
		WORD					connStatus;		  		/* Connection info				*/
		WORD					MaxConns;		  		/* Maximum connections			*/
		WORD 					DSAuthen;		  		/* Flag for DS authenticated	*/
		WORD					numActive;		  		/* Number of active connects	*/
		WORD					completelyLoggedOut;	/* All connections dropped?	*/
		WORD					completelyLoggedOutDS;/* All DS connections dropped?*/
		WORD					haveServer;				/* Server list given				*/

		NWCONN_HANDLE		DefaultConnectionID;	/* Default connection ID		*/
		NWCONN_HANDLE		LastConnectionID;		/* Last connection to drop		*/
		NWCONN_HANDLE		*connectList;			/* Pointer to list of connects*/

		NWDSContextHandle	Context;					/* Current context				*/

		int					i;							/* Looping variable				*/

		NWCCODE				ccode;					/* Return Codes 					*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->numActive = 0;

	errcode = SUCCESS;
	l->connectList = NULL;
	l->haveServer = (*ServerName == NULL) ? FALSE : TRUE;

	/*
	 * Retrieve the default connection.  This connection represents the 
	 * connection where the current directory is located.  Assume that
	 * this is the last connection we wish to drop.
	 */
	if (NWGetDefaultConnectionID(&l->DefaultConnectionID) != 0)
	{
		errcode = NO_DEFAULT_CONN;
		goto Terminate;
	}
	l->LastConnectionID = l->DefaultConnectionID;

	/*
	 * Look and see if we have a DS connection.  If we do, then go and get
	 * the monitored connection and make this the last connection to drop.
	 */
	l->DSAuthen = FALSE;
	if (NWIsDSAuthenticated())
	{
		NWDSGetMonitoredConnection(&l->LastConnectionID);
		l->DefaultConnectionID = l->LastConnectionID;
		l->DSAuthen = TRUE;
	}

	/*
	 * For an extra check to see if we have only one connection, if we
	 * do then make that the last connection to drop.
	 */
	NWGetMaximumConnections(&l->MaxConns);
	l->connectList =
		(NWCONN_HANDLE *)malloc(l->MaxConns * sizeof(NWCONN_HANDLE));
	if (l->connectList == NULL)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	l->ccode = NWGetConnectionList(ALL_SESSIONS,l->connectList,
		l->MaxConns,&l->numActive);
	if (l->ccode != 0)
	{
		errcode = SYSTEM_ERROR;
		goto Terminate;
	}
	if (l->numActive == 1)
	{
		l->LastConnectionID = l->connectList[0];
	}

	/*
	 * Now start logging out of servers.  If the user has specified a
	 * server then logout of that server, otherwise log out of all servers.
	 */
	if (l->haveServer == TRUE)
	{
		l->ccode = LogoutServer(ServerName,l->LastConnectionID,logoutTree);
		if (l->ccode != SUCCESS)
		{
			errcode = l->ccode;
			goto Terminate;
		}
	}
	else
	{
		l->ccode = LogoutAll(l->LastConnectionID,logoutTree);
		if (l->ccode != SUCCESS)
		{
			errcode = l->ccode;
			goto Terminate;
		}
	}

	/*
	 * Retrieve the new connection list after logging out of all desired
	 * servers.
	 */
	l->ccode = NWGetConnectionList(ALL_SESSIONS,l->connectList,l->MaxConns,
		&l->numActive);
	if (l->ccode != 0)
	{
		errcode = SYSTEM_ERROR;
		goto Terminate;
	}

	/*
	 * Go through the current connection list and see if we have disconnected
	 * from all servers and if we have disconnected from all DS servers.
	 */
	l->completelyLoggedOut = l->completelyLoggedOutDS = TRUE;
	for (l->i = 0; l->i < l->numActive; l->i++)
	{
		DSConnStatus(l->connectList[l->i], &l->connStatus);

		if ((l->connStatus & CONNECTION_LOGGED_IN) != 0)
		{
			if ((l->connStatus & CONNECTION_NDS) != 0)
			{
				l->completelyLoggedOutDS = FALSE;
			}
			else
			{
				l->completelyLoggedOut = FALSE;
			}
		}
	}

	/*
	 * If we have completely logged out of all DS servers and we were
	 * DS authenticated, then perform a NWDSLogout to disconnection from
	 * the tree.
	 */
	if (l->completelyLoggedOutDS == TRUE && l->DSAuthen == TRUE)
	{
		if ((l->Context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			errcode = MEMORY_ERROR;
			goto Terminate;
		}
		NWDSLogout(l->Context);
		NWDSFreeContext(l->Context);
		l->DSAuthen = FALSE;
	}

	/*
	 * If we have completely logged out of all servers or we have been
	 * given the option of logging out of all servers, then perform the
	 * ultimate in disconnect -- perform an int 21h to the system that
	 * will truely drop all connections.
	 */
	if (l->completelyLoggedOut == TRUE || l->haveServer == FALSE)
	{
		/*
		 * If we were told to disconnection from all servers regardless
		 * of whether they were DS or bindery servers, then perform the
		 * ultimate disconnect.  If we were to logout of all connections, then
		 * the last connection id to drop would have already been logged
		 * out, but the connection would still be up -- this should drop
		 * this connection as well.
		 */
		if (logoutTree == FALSE)
		{
			// THIS MUST BE PORTED!! WIN95 doesn't suppor direct DOS
			// calls.  We need to figure out the equivalent of
			// a 0d7h.
/*			
			asm {
				mov	ah,0d7h
				int	21h
			}
*/
		}
	}

	/*
	 * Check to see if we current drive is invalid -- if it is then change
	 * to a different drive.
	 */
	CheckCurrentDrive();

	
Terminate:
	if (l != 0)
	{
		if (l->connectList != NULL)
		{
			free(l->connectList);
		}
		delete l;
	}

	return(errcode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LogoutServer() )

 NAME         : LogoutServer (BYTE *server, NWCONN_HANDLE lastconn,
							WORD logoutTree)

 DESCRIPTION  : This function will logout of the servers specified.  The
 		server may be a simple server name or may contain wildcard characters.

		If the server is matched with a current connection, then if the
		connection is not the last connection and if the connection matches
		the logoutTree request (FALSE = all, TRUE = DS only) then the
		connection is dropped.

 PARAMETERS   :   -> input          <-output 
                ->  server = name of server to disconnection from
					  ->	lastconn = last connection to drop
					  ->	logoutTree = flag to logout of DS objects only

 RETURN       :  WORD
 		MEMORY_ERROR	= could not perform a malloc
		SYSTEM_ERROR	= errors from NWGetConnectionList
		return codes from LogoutOne

 END_MANUAL_ENTRY
 ===========================================================================
 */
static WORD LogoutServer (BYTE *server, NWCONN_HANDLE lastconn,
	WORD logoutTree)
{
	WORD	errcode;								/* Function return code					*/

	struct locals
	{
		NWCONN_HANDLE	connectID;			/* Connection id of server				*/
		NWCONN_HANDLE	*connectList;		/* List of connections					*/
																									
		NWCCODE	ccode;						/* Return codes							*/

		WORD	i;							 		/* looping index							*/
		WORD	doLastConnection;		 		/* Flag for last connection			*/
		WORD	match;					 		/* Flag for wild card matching		*/
		WORD	maxconns;				 		/* Count of maximum connections		*/
		WORD	numActive;				 		/* Number of current connections		*/

		BYTE	testServer[SERVER_NAME_SIZE];	/* Server name for connection		*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->ccode = 0;


	/*
	 * Initialization
	 */
	errcode = SUCCESS;
	l->connectList = NULL;

	/*
	 * If no wild characters are specified in the server, then go and get
	 * the connection ID of the server and logout.
	 *
	 * If wild characters are specified, then go through the connection list
	 * and for each matching server, logout of that server.
	 */
	if (IsWild(server) == FALSE)
	{
		l->ccode = NWGetConnectionID(server,(WORD)NULL,&l->connectID,(WORD)NULL);
		if (l->ccode != 0)
		{
			errcode = SYSTEM_ERROR;
			goto Terminate;
		}
		/*
		 * If we have a connection to the server, logout
		 */
		errcode = LogoutOne(l->connectID,lastconn,TRUE,logoutTree);
	}
	else
	{
		/*
		 * Get a list of all the connections in use
		 */
		NWGetMaximumConnections(&l->maxconns);
		l->connectList =
			(NWCONN_HANDLE *)malloc(l->maxconns * sizeof(NWCONN_HANDLE));
		if (l->connectList == NULL)
		{
			errcode = MEMORY_ERROR;
			goto Terminate;
		}
		if ((l->ccode = NWGetConnectionList(ALL_SESSIONS,l->connectList,
			l->maxconns,&l->numActive)) != 0)
		{
			errcode = SYSTEM_ERROR;
			goto Terminate;
		}

		l->doLastConnection = FALSE;

		/*
		 * Logout from every connection that maps to the selected server.
		 * Make sure that the last connection to be dropped is dropped
		 * last and not when matched.
		 */
		for (l->i = 0 ; l->i < l->numActive; l->i++)
		{
			if ((l->ccode = NWGetFileServerName(l->connectList[l->i],
				(char *)l->testServer)) != 0)
			{
				errcode = SYSTEM_ERROR;
				goto Terminate;
			}

			/*
			 * Check for a match in the current connection table.
			 */
			l->match = WildMatch(server,l->testServer);

			/*
			 * If a match was made, then logout of that server unless it was
			 * the last connection.  If it is the last connection then flag it
			 * to be done later.
			 */
			if (l->match == TRUE)
			{
				if (l->connectList[l->i] != lastconn)
				{
					errcode = LogoutOne(l->connectList[l->i],lastconn,TRUE,
						logoutTree);
					if (errcode != SUCCESS)
					{
						DisplayIDMessage(errcode,NO_ICON);
					}
				}
				else
				{
					l->doLastConnection = TRUE;
				}
			}
		}
		
		/*
		 * If it was flagged to do the last connection, then drop it now.
		 */
		if (l->doLastConnection == TRUE)
		{
			errcode = LogoutOne(lastconn,lastconn,TRUE,logoutTree);
		}
	}

Terminate:
	if (l != 0)
	{
		if (l->connectList != NULL)
		{
			free(l->connectList);
		}
		delete l;
	}

	return(errcode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LogoutAll() )

 NAME         : LogoutAll(NWCONN_HANDLE lastconn, WORD logoutTree)

 DESCRIPTION  : This function will traverse through the entire connection
 		list and logout of all servers.  This will also make sure that the
		last connection is dropped at the end.

 PARAMETERS   :   -> input          <-output 
                ->  lastconn = last connection to drop
					  ->	logoutTree = flag to logout of DS only or all servers

 RETURN       :  WORD
		MEMORY_ERROR	= could not malloc enough memory
		SYSTEM_ERROR	= failures from NWGetConnectionList
		return codes from LogoutOne

 END_MANUAL_ENTRY
 ===========================================================================
*/
static WORD LogoutAll (NWCONN_HANDLE lastconn, WORD logoutTree)
{
	WORD	errcode;

	struct locals
	{
		WORD	i;						/* Looping index							*/
		WORD	ccode;				/* Error code								*/
		WORD	maxconns;			/* Maximum workstation connections	*/
		WORD	numActive;			/* Number active connections			*/

		NWCONN_HANDLE	*connectList;	/* Connection list pointer		*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));


	errcode = SUCCESS;
	l->connectList = NULL;

	/*
	 * Get all the current connections.
	 */
	NWGetMaximumConnections(&l->maxconns);
	l->connectList =
		(NWCONN_HANDLE *)malloc(l->maxconns * sizeof(NWCONN_HANDLE));
	if (l->connectList == NULL)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}

	l->ccode = NWGetConnectionList(ALL_SESSIONS,l->connectList,l->maxconns,
		&l->numActive);
	if (l->ccode != 0)
	{
		errcode = SYSTEM_ERROR;
		goto Terminate;
	}
	if (l->numActive == 0)
	{
		errcode = SUCCESS;
		goto Terminate;
	}

	/*
	 * Check each connection.  If it is not the last connection then decide
	 * if we should logout and detach from it
	*/
	for (l->i = 0 ; l->i < l->numActive ; l->i++)
	{
		if (l->connectList[l->i] != lastconn)
		{
			errcode = LogoutOne(l->connectList[l->i],lastconn,FALSE,logoutTree);
			if (errcode != SUCCESS)
			{
				DisplayIDMessage(errcode,NO_ICON);
			}
		}
	}

	/*
	 * Drop the last connection.
	 */
	errcode = LogoutOne(lastconn,lastconn,FALSE,logoutTree);

	/*
	 * Logout should clean up the banner since there is no user name now.
	 */
	NWSetBannerUserName("");

Terminate:
	if (l != 0)
	{
		if (l->connectList != NULL)
		{
			free(l->connectList);
		}
		delete l;
	}

	return(errcode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LogoutOne() )

 NAME         : LogoutOne (NWCONN_HANDLE connectID, NWCONN_HANDLE lastconnID,
							WORD haveServer, WORD logoutTree)

 DESCRIPTION  : This will log the user out of the desired file server.  If
 		the file server specified matches with the logoutTree flag.  Also if it
		is not the last connection, then this will also detach from the file
		server and free the connection slot.

 PARAMETERS   :   -> input          <-output 
                ->  connectID = connection to drop
					  ->	lastconnID = last connection to drop
					  ->	haveServer = flag is server was specified or all
					  ->	logoutTree = flag to log out of DS only

 RETURN       :  WORD
 		SYSTEM_ERROR	= failures from NWFreeConnectionSlot
 		SYSTEM_ERROR	= failures from NWLogoutFromFileServer
 		SYSTEM_ERROR	= failures from NWDetachFromFileServer

 END_MANUAL_ENTRY
 ===========================================================================
*/
static WORD LogoutOne (NWCONN_HANDLE connectID, NWCONN_HANDLE lastconnID,
	WORD haveServer, WORD logoutTree)
{
	WORD	errcode;

	struct locals
	{
		WORD	ccode;				  	/* Error code										*/
		WORD	lastConnect;			/* Is this the last active connection?		*/
		WORD	connStatus;			  	/* connection status information				*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->lastConnect = FALSE;

	errcode = SUCCESS;

	/*
	 * get the connection status
	 */
	DSConnStatus(connectID,&l->connStatus);

	/*
	 * If we are logging out of a tree, then if the connection is
	 * not an NDS connection, then leave it alone.  We keep bindery
	 * connections around.
	 */
	if (logoutTree == TRUE && (l->connStatus & CONNECTION_NDS) == 0)
	{
		errcode = SUCCESS;
		goto Terminate;
	}

	/*
	 * NDS connections hang around so look for nds connections not logged in
	 * or authenticated and free the connection then return
	 */
	if (connectID != lastconnID && (l->connStatus & CONNECTION_NDS) != 0 &&
		(l->connStatus & CONNECTION_LOGGED_IN) == 0)
	{
	 	l->ccode = NWFreeConnectionSlot(connectID,SYSTEM_DISCONNECT);
		if (l->ccode != 0)
		{
			errcode = SYSTEM_ERROR;
		}
		else
		{
			errcode = SUCCESS;
		}
		goto Terminate;
	}

	/*
	 * is this the last one
	 */
	if (connectID == lastconnID)
	{
		l->lastConnect = TRUE;
	}

	/*
	 * If we were logged in, then logout
	 */
	if ((l->connStatus & CONNECTION_LOGGED_IN) != 0)
	{
		/*
		 * end capture for this connection
		 */
		EndCapConnID(connectID);
	
		if ((l->ccode = NWLogoutFromFileServer(connectID)) != 0)
		{
			errcode = SYSTEM_ERROR;
			goto Terminate;
		}

		/*
		 * If this is not the last connection then if we were given
		 * a server and it is an NDS server then disconnect a little
		 * differently then if it is a bindery connection.
		 */
		if (l->lastConnect == FALSE)
		{
			if (haveServer && (l->connStatus & CONNECTION_NDS) != 0)
			{
				l->ccode = NWFreeConnectionSlot(connectID,SYSTEM_DISCONNECT);
			}
			else
			{
				l->ccode = _NWFreeConnectionSlot(connectID,SYSTEM_DISCONNECT);
			}
			if (l->ccode != 0)
			{
				errcode = SYSTEM_ERROR;
			}
			else
			{
				errcode = SUCCESS;
			}
			goto Terminate;
		}
	}	
	else 
	{
		/*
		 * If we were not logged in and this is not the last
		 * connection, detach
		 */
		if (l->lastConnect == FALSE)
		{
			if ((l->ccode = NWDetachFromFileServer(connectID)) != 0)
			{
				errcode = SYSTEM_ERROR;
				goto Terminate;
			}
		}
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return(errcode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( EndCapConnID() )

 NAME         : EndCapConnID(NWCONN_HANDLE connID)

 DESCRIPTION  : This function will loop through all the printer definitions
 		and end any capture statements that are done on those printers.

 PARAMETERS   :   -> input          <-output 
                ->  connID = connection to end capture for

 RETURN       :  None

 END_MANUAL_ENTRY
 ===========================================================================
*/
static void EndCapConnID (NWCONN_HANDLE connID)
{
	struct locals
	{
		BYTE		printerNumber;

 		NWCAPTURE_FLAGS1 capFlagsRW;
	 	NWCAPTURE_FLAGS2 capFlagsRO;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	for (l->printerNumber = 1; l->printerNumber < MAXPRINTERS;
		l->printerNumber++)
	{
		/*
		 * check to see if there is a connection ID
		 */
		if ((NWGetCaptureFlags(l->printerNumber,&l->capFlagsRW,&l->capFlagsRO)
			== 0)	&& (l->capFlagsRO.connID == connID))
		{
			NWEndCapture(l->printerNumber);
		}
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return;
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CheckCurrentdrive() )

 NAME         : CheckCurrentdrive()

 DESCRIPTION  : This will check the current drive and see if it is now an
 		invalid network drive.  If it is then this will change the curretn
		drive to a valid one.

 RETURN       :  None

 END_MANUAL_ENTRY
 ===========================================================================
*/
static void CheckCurrentDrive()
{
	struct locals
	{
		NWCCODE			ccode;			/* return codes							*/

		unsigned int	drive;			/* looping index for drives			*/
		unsigned int	maxdrives;		/* dummy for call to setdrive			*/

		WORD				driveStatus;	/* drive status for each drive		*/
		WORD				netdrive;		/* flags signalling a network drive	*/

		NWCONN_HANDLE	connectionID;	/* temporary connection id				*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	_dos_getdrive(&l->drive);

	l->ccode = NWGetDriveStatus(l->drive,NW_FORMAT_NETWARE,&l->driveStatus,
		&l->connectionID,NULL,NULL,NULL);

	l->netdrive = (NW_NETWORK_DRIVE | NW_LITE_DRIVE | NW_NETWARE_DRIVE);

	if (l->ccode == NW_INVALID_DRIVE ||	l->driveStatus == NW_UNMAPPED_DRIVE)
	{
		for (l->drive = 1; l->drive <= MAXDRIVES; l->drive++)
		{
			l->ccode = NWGetDriveStatus(l->drive, NW_FORMAT_NETWARE,
				&l->driveStatus,&l->connectionID,NULL,NULL,NULL);

			if (l->ccode != NW_INVALID_DRIVE &&
				(l->driveStatus & l->netdrive) != 0)
			{
				_dos_setdrive(l->drive,&l->maxdrives);
				goto Terminate;
			}
		}
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return;
}

