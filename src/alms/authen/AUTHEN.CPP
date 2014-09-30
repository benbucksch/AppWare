/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 *
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

/*
 * This file contains the functions necessary for the Authenticate ALM
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

/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

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
#include <o_flag.h>
#include <o_number.h>

#define NWWIN
#include <constant.h>
#include <nwcalls.h>
#include <nwnet.h>
#include <nwintcon.h>
#include <dbcsmac.h>
#include <nwundoc.h>
#include <nwintern.h>
#include "utils.h"
#include "alm_authen.h"
#include "errors.h"
#include "resource.h"
#include "mx.h"
#include "parseut.h"
#include "authwin.h"
#include "msupport.h"

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
#define AUTHENTICATION_DS_OK		 		1		/* NDS login done				*/
#define AUTHENTICATION_DS_OK_PE			2
#define AUTHENTICATION_BINDERY_OK		3		/* Bindery login done		*/
#define AUTHENTICATION_BINDERY_OK_PE	4		/* Bindery login done		*/
#define AUTHENTICATION_DIFF_USER		5	 	/* Diff user/same tree		*/
#define AUTHENTICATION_ERROR				6	 	/* Error							*/

/* Error Codes */
#define MAXTIME						10			/* Time length						*/
#define MAXMESSAGE					1024

/*
 * ======================================================================
 *	AUTHENTICATION module prototypes
 * ======================================================================
 */

/* Function prototypes */
WORD DoAuthenticate(BYTE *ServerName, BYTE *UserName, BYTE *Password,
	WORD LoginType);
WORD ConvertDSToBinderyCodes(NWCCODE ccode);

/* Static function prototypes */
static WORD DSGetCurrentTreeName(BYTE *treeName);
static void DisconnectDSServers(NWCONN_HANDLE pConnID);

/*
 * ========================================================================
 * Global Variables
 * ========================================================================
 */
#if LOGIN IS_DISABLED
extern int _WinAllocFlag;
HINSTANCE hInstance;


extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	hInstance = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
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



/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( (Authenticate) )

 NAME       : Authenticate(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION: This function will authenticate a workstation to the
 		identified server.  Inputs required are a server or tree name, the
		users name.  Optional inputs is a password.

		This function will retrieve all the parameters from the appbuilder
		and then call DoAuthenticate to perform the authentication.
 
 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = TEXT....required..ServerName
					GET2 = TEXT....optional..UserName..default=current user
					GET3 = TEXT....optional..Password..default=empty
					GET4 = NUMBER..optional..Login Type..default=0

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = none

 FLOW/RETURN:
		AUTHENTICATION_DS_OK		 	1			NDS login done
		AUTHENTICATION_BINDERY_OK	2			Bindery login done
		AUTHENTICATION_DIFF_USER	3			Diff user/same tree
		AUTHENTICATION_ERROR			4			Error

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK Authenticate (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE	UserName[MAX_DN_BYTES];			  	/* Passed in user name 			*/
		BYTE 	Password[PASSWORD_BUFFER_SIZE]; 	/* Passed in password			*/
		BYTE 	ServerName[SERVER_NAME_SIZE];	  	/* Server/tree passed in		*/

		NWDSContextHandle	Context;				  	/* Current context				*/

		long					inputtype;				/* Login type input				*/

		WORD 					DSAuthen;			  	/* Flag for DS authenticated	*/
		WORD					LoginType;			  	/* Flag for login type			*/

		primaryConnectionStruct	pc;

		NWCCODE				ccode;				  	/* Return Codes 					*/
														  
		AOBJECTID				oiUserName;		  		/* User name ALM object			*/
		AOBJECTID				oiPassword;		  		/* Password ALM object			*/
		AOBJECTID				oiServerName;	  		/* Server ALM object				*/
		AOBJECTID				oiLoginType;	  		/* Login Type ALM object		*/
	} *l;

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = AUTHENTICATION_DS_OK;
	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = AUTHENTICATION_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Retrieve the server/tree parameter from the Visual Appbuilder.
	 * This field is NOT OPTIONAL.
	 */
	FillMemory(l->ServerName,0,sizeof(l->ServerName));
	l->oiServerName = AFuncGetTypedParameter(1, OTYPE_TEXT);
	OTextGetToBuffer(l->oiServerName,(char *)l->ServerName,SERVER_NAME_SIZE);
	NWLSTRUPR((char *)l->ServerName);
	if (l->oiServerName == 0 || *(l->ServerName) == NULL)
	{
		DisplayIDMessage(NO_SERVER,NO_ICON);
		theSystem->message1 = AUTHENTICATION_ERROR;
		goto Terminate;
	}
	
	/*
	 * Get the specified user name from AppBuilder.  If the user name
	 * is not specified and we are DS authenticated, then default the
	 * user name to the current user's name.  If we are bindery based
	 * then the user name must be specified.
	 *
	 * This field is OPTIONAL.
	 */
	l->oiUserName = AFuncGetTypedParameter(2, OTYPE_TEXT);
	OTextGetToBuffer(l->oiUserName,(char *)l->UserName,MAX_DN_BYTES);
	if (l->oiUserName == 0 || *(l->UserName) == NULL)
	{
		/*
		 * If no user was specified, then if we are authenticated, then
		 * use the current user name as the default.  If we are in
		 * the bindery world, we must have a user name.
		 */
		l->DSAuthen = NWIsDSAuthenticated();
		if (l->DSAuthen == TRUE)
		{
			if ((l->Context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
			{
				DisplayIDMessage(CANNOT_CREATE_CONTEXT,NO_ICON);
				theSystem->message1 = AUTHENTICATION_ERROR;
				goto Terminate;
			}
			NWDSWhoAmI(l->Context,(char *)l->UserName);
			NWDSFreeContext(l->Context);
		}
		else
		{
			FillMemory(&l->pc,0,sizeof(l->pc));
			FillPrimaryConnInfo(&l->pc);
			if (l->pc.userName[0] == '\0')
			{
				DisplayIDMessage(NO_USER,NO_ICON);
				theSystem->message1 = AUTHENTICATION_ERROR;
				goto Terminate;
			}
			lstrcpy((char *)l->UserName,(char *)l->pc.userName);
		}
	}

	/*
	 * Get the password from the AppBuilder.  This field may be blank
	 * because there may not be a password for this user.
	 *
	 * This field is OPTIONAL.
	 */
	FillMemory(l->Password,0,sizeof(l->Password));
	l->oiPassword = AFuncGetTypedParameter(3, OTYPE_TEXT);
	if (l->oiPassword != 0)
	{
		OTextGetToBuffer(l->oiPassword,(char *)l->Password,PASSWORD_BUFFER_SIZE);
	}

	/*
	 * Get the login type from the AppBuilder.  This field may be blank
	 * because the user may want the default.
	 *
	 * This field is OPTIONAL.
	 */
	l->LoginType = DEFAULT_LOGIN;
	l->oiLoginType = AFuncGetTypedParameter(4, OTYPE_NUMBER);
	if (l->oiLoginType != 0)
	{
		l->inputtype = ONmbrGetInteger(l->oiLoginType);
		if (l->inputtype == DEFAULT_LOGIN)
		{
			l->LoginType = DEFAULT_LOGIN;
		}
		else if (l->inputtype == NDS_LOGIN)
		{
			l->LoginType = NDS_LOGIN;
		}
		else if (l->inputtype == BINDERY_LOGIN)
		{
			l->LoginType = BINDERY_LOGIN;
		}
		else
		{
			DisplayIDMessage(INVALID_LOGIN_TYPE,NO_ICON);
			theSystem->message1 = AUTHENTICATION_ERROR;
			goto Terminate;
		}
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		theSystem->message1 = AUTHENTICATION_ERROR;
		goto Terminate;
	}

	l->ccode = DoAuthenticate(l->ServerName,l->UserName,l->Password,
		l->LoginType);
	if (l->ccode != DS_SUCCESS && l->ccode != BINDERY_SUCCESS &&
		l->ccode != DS_SUCCESS_PE && l->ccode != BINDERY_SUCCESS_PE)
	{
		if (l->ccode == DIFF_USER_SAME_TREE)
		{
			theSystem->message1 = AUTHENTICATION_DIFF_USER;
		}
		else
		{
			DisplayIDMessage(l->ccode,NO_ICON);
			theSystem->message1 = AUTHENTICATION_ERROR;
		}
	}
	else
	{
		switch (l->ccode)
		{
		case DS_SUCCESS:
			theSystem->message1 = AUTHENTICATION_DS_OK;
			break;

		case DS_SUCCESS_PE:
			theSystem->message1 = AUTHENTICATION_DS_OK_PE;
			break;

		case BINDERY_SUCCESS:
			theSystem->message1 = AUTHENTICATION_BINDERY_OK;
			break;

		case BINDERY_SUCCESS_PE:
			theSystem->message1 = AUTHENTICATION_BINDERY_OK_PE;
			break;

		default:
			DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
			theSystem->message1 = AUTHENTICATION_ERROR;
			goto Terminate;
		}
	}

Terminate:
	if (l != 0)
	{
		FillMemory(l->Password,0,sizeof(l->Password));
		delete l;
	}
	
	return;
}
#endif

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DSGetCurrentTreeName() )

 NAME         : DSGetCurrentTreeName(NWCONN_HANDLE *connID, char *server,
 							char *treeName)

 DESCRIPTION  : This function will traverse through the active connection
 		list and find the first NDS server that we are logged into.  Then
		it will return the connection ID, the server name and the tree name
		of that connection.  Since NDS is only to allow a single NDS tree
		connection, this should be our current tree.

 PARAMETERS   :   -> input          <-output 
                <-  connID = connection id of server in current tree
					  <-	server = name of server in current tree
					  <-	treeName = name of the current tree

 RETURN       :  WORD
 		0 = everything is fine
 		failures of NWGetConnectionList
		ERR_NOT_ENOUGH_MEMORY = malloc failure
	   ERR_CONTEXT_CREATION = error creating context
		ERR_NO_SERVER_FOUND = No NDS servers found in connection list

 MODIFICATIONS: February 18, 1994

 END_MANUAL_ENTRY
===========================================================================*/
static WORD DSGetCurrentTreeName (BYTE *treeName)
{
	WORD	errcode;							/* Return code from function			*/

	struct locals
	{
		WORD	i;									/* Loop counter							*/

		NWCONN_HANDLE	*connectList;		/* Buffer for connection list			*/

		REGISTERS	regs;						/* Register for assembly call			*/

		char	tree[1024];	/* tree name */

		NWDSContextHandle	context;			/* DS context handle */
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	errcode = 0;
	l->context = ERR_CONTEXT_CREATION;
	l->connectList = NULL;

	if (NWIsDSAuthenticated() == 0)
	{
		errcode = ERR_NO_SERVER_FOUND;
		goto Terminate;
	}

	l->regs.w.di = FP_OFF((void  *)l->tree);
	l->regs.w.es = FP_SEG((void  *)l->tree);
	l->regs.w.bx = 0;
	l->regs.w.dx = 0x32;
	if (NWVLMRequest(0,VLM_ID_NWP,15,&l->regs,0) != 0 || *(l->tree) == NULL)
	{
		errcode = ERR_NO_SERVER_FOUND;
		goto Terminate;
	}
	if (treeName != 0)
	{
		l->tree[MAX_TREE_NAME_CHARS] = 0;
	
		/*
		 * trim off trailing underscores
		 */
		for (l->i = MAX_TREE_NAME_CHARS - 1; l->tree[l->i] == '_'; l->i--)
		{
			l->tree[l->i] = 0;
		}
		lstrcpy((char *)treeName,(char *)l->tree);
	}
	
Terminate:
	if (l != 0)
	{
		if (l->context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			NWDSFreeContext(l->context);
		}
		if (l->connectList != NULL)
		{
			free(l->connectList);
		}
		delete l;
	}

	return(errcode);
}

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DoAuthenticate() )

 NAME         : DoAuthenticate (pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  : This function will authenticate a workstation to the
 		identified server.

		This function will first assume that the server passed in is a 
		server and will attempt to attach to it.  If the attach fails, then
		we assume that it was a tree and attempt to attach to a server
		in the tree.

		Once we have an attachment, then we see if the server is a bindery
		or NDS server.  If it is an NDS server, then we see if we are 
		already authenticated.  If we are, then we are done.  If we are not
		authenticated, then we check to see if the server is in the same
		tree as we are in -- if so, then if the user name given is our
		user name we just authenticate to the server, otherwise we fail
		because we only allow one user authenticated to a tree for this
		workstation  If the desired server is not in the same tree, then
		we will automatically preform a bindery attachment.

 PARAMETERS   :   -> input          <-output 
 						->	ServerName	= name of server or tree
						->	UserName		= user name
						->	Password		= password for login

 RETURN       :  WORD

		DS_SUCCESS					 Everything is okay
		BINDERY_SUCCESS			 Everything is okay
		NO_SERVER					 No server/tree passed in
		NO_USER						 No user name passed in
		NO_SERVER_OR_TREE			 Server/tree not found
		CONNECTION_FULL 			 Connection table full
		SYSTEM_ERROR				 General system error
		DIFF_USER_SAME_TREE 		 Same tree but diff userid
		NO_OBJECT_BAD_PASSWORD	 Object not found in tree
		AUTHENTICATION_DENIED	 Login is denied


 MODIFICATIONS: February 18, 1994

 END_MANUAL_ENTRY
============================================================================*/
WORD DoAuthenticate (BYTE *ServerName, BYTE *UserName, BYTE *Password,
	WORD LoginType)
{
	WORD					errcode;							/* return error code			*/

	struct locals
	{
		BYTE 	HoldPassword[PASSWORD_BUFFER_SIZE];	/* Password holder			*/
		BYTE 	TreeName[MAX_TREE_NAME_CHARS+1];		/* Tree of desired server	*/
		BYTE 	CurTree[MAX_TREE_NAME_CHARS+1];		/* Current tree				*/
		BYTE 	CurServer[SERVER_NAME_SIZE]; 			/* Current server				*/
		BYTE 	CurUserName[MAX_DN_BYTES];				/* Current user name			*/
		BYTE	UserBuf[MAX_DN_BYTES];					/* Temporary user name buf	*/
		BYTE 	curcontext[MAX_DN_BYTES];				/* Current context buffer	*/
		BYTE	newcontext[MAX_DN_BYTES];				/* New context buffer		*/
		BYTE	ObjName[MAX_DN_BYTES];					/* Object name bindery user*/
		BYTE	LogTime[MAXTIME];							/* Bindery login time		*/
		BYTE	HoldBuf[MAXMESSAGE];
		BYTE	message[MAXMESSAGE];
		BYTE	*attrVal;
		BYTE	mseg;

		Restrictions		restrict;

		NWOBJ_TYPE			ObjType;						/* Bindery object type		*/

		NWOBJ_ID				ObjId;						/* Bindery object id			*/

		WORD					ConnStatus;					/* Connection info			*/

		NWCONN_HANDLE		DSConnectionID;			/* ConnectionID DS Server	*/
		NWCONN_HANDLE		ConnectionID;				/* Connection ID current	*/

		NWCONN_NUM			ConnectionNum;				/* Connection number			*/

		NWDSContextHandle	Context;						/* Current context			*/

		WORD 					DSConnection;				/* Flag for DS connection	*/
		WORD 					DSAuthen;					/* Flag DS authenticated	*/

		int					i;								/* Looping variable			*/
		long					graceLoginLimit;
		long					graceLogins;
		
		NWCCODE				ccode;						/* Return Codes 				*/

		uint32				syntaxID;
	} *l;

	/*
	 * Initialization
	 */
	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	FillMemory(l->HoldPassword,0,sizeof(l->HoldPassword));
	lstrcpy((char *)l->HoldPassword,(char *)Password);
	errcode = DS_SUCCESS;

	l->ccode = AttachtoServer(ServerName,&l->ConnectionID);
	if (l->ccode != 0)
	{
		errcode = l->ccode;
		goto Terminate;
	}

	/*
	 * Find out of the server is a DS server and save the server's
	 * tree name.
	 */
	l->DSConnection =
		NWIsDSServer(l->ConnectionID,(char *)l->TreeName) ? NDS : BINDERY;

	/*
	 * trim off trailing underscores in tree name
	 */
	l->TreeName[MAX_TREE_NAME_CHARS] = 0;
	for (l->i = MAX_TREE_NAME_CHARS - 1;l->TreeName[l->i] == '_' && l->i >= 0;
		l->i--)
	{
		l->TreeName[l->i] = 0;
	}

	/*
	 * If the connection is a NDS connection and we are authenticated, then
	 * we need to check to see if we are connecting in the same tree.  If
	 * we are connecting into a server in the same tree -- then make sure
	 * that we are the same user.  If we are attempting to authenticate to
	 * the same tree as a different user -- then there is an error.
	 *
	 * If we are connecting to a different tree then we are currently in,
	 * then we need to perform a bindery connection to that server in 
	 * another tree.
	 *
	 * If we are not an NDS connection, then obviously we need to perform
	 * a bindery connection.
	 */
	if (l->DSConnection == NDS && LoginType != BINDERY_LOGIN)
	{
		l->DSAuthen = NWIsDSAuthenticated();
		if (l->DSAuthen == TRUE)
		{
			/*
			 * Get the current tree.  If there is not one or it is different
			 * then the tree we want, go and do a bindery connection.
			 */
			l->ccode = DSGetCurrentTreeName(l->CurTree);
			if (l->ccode == SUCCESS)
			{
				/*
				 * If we already have a BINDERY connection to the server
				 * we are trying to attach to in NDS mode, then blow away
				 * the BINDERY connection so we can re-establish an NDS
				 * connection.
				 */
				DSConnStatus(l->ConnectionID, &l->ConnStatus);
				if ((l->ConnStatus & CONNECTION_NDS) == 0 &&
					(l->ConnStatus & CONNECTION_LOGGED_IN))
				{
					if (NWLogoutFromFileServer(l->ConnectionID) != 0)
					{
						errcode = SYSTEM_ERROR;
						goto Terminate;
					}
				}
				/*
				 * If we are going to a server in the same tree, then make sure
				 * that we are going as the same user we are now.
				 */
				if (strcmpi((char *)l->CurTree,(char *)l->TreeName) == 0)
				{
					if ((l->Context = NWDSCreateContext()) ==	(NWDSContextHandle)ERR_CONTEXT_CREATION)
					{
						errcode = CANNOT_CREATE_CONTEXT;
						goto Terminate;
					}
					NWDSWhoAmI(l->Context,(char *)l->CurUserName);
					NWDSCanonicalizeName(l->Context,(char *)UserName,(char *)l->UserBuf);
					NWDSAbbreviateName(l->Context,(char *)l->UserBuf,(char *)UserName);
					NWDSFreeContext(l->Context);
					/*
					 * We are not going as the same user -- so bomb out!
					 * If we want default attachment, then we will logout as
					 * the current user and attempt to attach as the new user
					 * in bindery mode.
					 */
					if (strcmpi((char *)UserName,(char *)l->CurUserName) != 0)
					{
#if 0
Remove this special treatment for the DEFAULT_LOGIN and
make it respond like everything else -- different user but same tree.

						if (LoginType == DEFAULT_LOGIN)
						{
							if ((l->ccode = 
								NWLogoutFromFileServer(l->ConnectionID)) != 0)
							{
								errcode = SYSTEM_ERROR;
								goto Terminate;
							}
							LoginType = BINDERY_LOGIN;
						}
						else
#endif
						{
							errcode = DIFF_USER_SAME_TREE;
							goto Terminate;
						}
					}
					else
					{
						/*
						 * If the connection is a bindery connection, then logout
						 * of the server, so that we can authenticate below in
						 * NDS mode.
						 */
						DSConnStatus(l->ConnectionID, &l->ConnStatus);
						if ((l->ConnStatus & CONNECTION_NDS) == 0)
						{
							if ((l->ccode =
								NWLogoutFromFileServer(l->ConnectionID)) != 0)
							{
								errcode = SYSTEM_ERROR;
								goto Terminate;
							}
						}

						/*
						 * Authenticate to the desired server
						 */
						l->ccode = NWDSAuthenticate(l->ConnectionID,
							(unsigned long)NULL,NULL);
						if (l->ccode != 0)
						{
							errcode = AUTHENTICATION_DENIED;
						}
						else
						{
							errcode = DS_SUCCESS;
						}
						goto Terminate;
					}
				}
				else
				{
					if (LoginType == DEFAULT_LOGIN)
					{
						LoginType = BINDERY_LOGIN;
					}
				}
			}
			else
			{
				/* 
				 * Pretend that we are not authenticated so that we will do
				 * a first time DS login below.  This condition can only arise
				 * if we have been authenticated and then changed our
				 * connections to bindery connections so that there is no
				 * current DS connection (but we are still authenticated).
				 * We do not have to worry about being logged into the server
				 * as two different users, because the NDSLogin() below will
				 * overwrite the old user with this new user.
				 */
				l->DSAuthen = FALSE;
			}
		}

		/*
		 * We must have an NDS connection, but we have never authenticated
		 * to a tree in the system.  Therefore, we must set up the first
		 * NDS connection.
		 */
		if (l->DSAuthen == FALSE && LoginType != BINDERY_LOGIN)
		{
			/*
			 * If we are connected in a bindery mode, then log out of
			 * the file server, so that we can go into NDS mode below.
			 */
			DSConnStatus(l->ConnectionID, &l->ConnStatus);
			if ((l->ConnStatus & CONNECTION_NDS) == 0)
			{
				if ((l->ccode =
					NWLogoutFromFileServer(l->ConnectionID)) != 0)
				{
					errcode = SYSTEM_ERROR;
					goto Terminate;
				}
			}

			if ((l->Context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
			{
				DisplayIDMessage(CANNOT_CREATE_CONTEXT,NO_ICON);
				errcode = CANNOT_CREATE_CONTEXT;
				goto Terminate;
			}
			NWDSSetConnectionInfo(l->ConnectionID,CONVERT_TO_DS);
			/*
			 * Make sure that our preferred tree is the same as the tree
			 * of the desired server.  Following the setup of the preferred
			 * tree we must DISCONNECT ANY DS SERVERS from us (we know that
			 * we are not logged into them or we would have run the code
			 * above -- so they are just attached).  The DS servers must be
			 * disconnected in order for the preferred tree name to really
			 * take effect in the VLMs.
			 */
			NWSetPreferredDSTree(lstrlen((char *)l->TreeName),l->TreeName);
			DisconnectDSServers(l->ConnectionID);

			lstrcpy((char *)Password,(char *)l->HoldPassword);
			l->ccode = NWDSLogin(l->Context,0,(char *)UserName,(char *)Password,~0);
			l->ccode = ConvertDSToBinderyCodes(l->ccode);
			if (l->ccode == 0)
			{
				errcode = DS_SUCCESS;
				l->ccode = NWDSAuthenticate(l->ConnectionID,
					(unsigned long)NULL,NULL);
				if (l->ccode != 0)
				{
					errcode = AUTHENTICATION_DENIED;
				}
			}
			else if (l->ccode == PASSWORD_HAS_EXPIRED)
			{
				errcode = DS_SUCCESS;
				l->ccode = NWDSAuthenticate(l->ConnectionID,
					(unsigned long)NULL,NULL);
				if (l->ccode != 0)
				{
					errcode = AUTHENTICATION_DENIED;
				}

				l->graceLoginLimit = l->graceLogins = -1L;
				l->ccode = ReadAttribute((char *)UserName,"Login Grace Limit",
					(void **)&l->attrVal,&l->syntaxID);
				if (l->ccode == 0)
				{
					l->graceLoginLimit = (long)*l->attrVal;
				}
				if (l->attrVal != 0)
				{
					free(l->attrVal);
				}
				if (l->graceLoginLimit != -1)
				{
					l->ccode = ReadAttribute((char *)UserName,"Login Grace Remaining",
						(void **)&l->attrVal,&l->syntaxID);
					if (l->ccode == 0)
					{
						l->graceLogins = (long)*l->attrVal;
					}
					if (l->attrVal != 0)
					{
						free(l->attrVal);
					}
				}

				if (l->graceLoginLimit != -1)
				{
					DSALMGetMessage(PWD_EXPIRED,(char *)l->HoldBuf,sizeof(l->HoldBuf));
					wsprintf((char *)l->message,(char *)l->HoldBuf,(char *)l->graceLogins);
					DisplayTextMessage((char *)l->message,NO_ICON);
				}
			}
			else if (l->ccode == (NWCCODE)ERR_NO_SUCH_ENTRY)
			{
				/*
				 * We must be an unknown user to the desired server.  This
				 * may be correct if we gave a bad user or a password, but
				 * this will also happen if our current context is not 
				 * correct for the user.  Since we know we want to login to
				 * this server, change our current context to the same as the
				 * server and see if the user name is know at that level.
				 */
				errcode = NO_OBJECT_BAD_PASSWORD;
				if (NWDSGetServerDN(l->Context,l->ConnectionID,(char *)l->newcontext) == 0
					&& NWDSCanonicalizeName(l->Context,(char *)l->newcontext,
					(char *)l->newcontext) == 0)
				{
					NWDSCanonicalizeName(l->Context,(char *)UserName,(char *)l->UserBuf);
					DSRemoveNameFromFullName((char *)l->UserBuf,(char *)l->curcontext,
						NULL,(char *)l->UserBuf);
					DSRemoveNameFromFullName((char *)l->newcontext,(char *)l->newcontext,
						NULL,NULL);
					/*
					 * Now we have the server and the current context.  If they
					 * happen to be the same, then don't bother trying to login
					 * because we will just try the same user name.  But if the
					 * context is different, then set the current context to the
					 * server's context and attempt another login.
					 */
					if (strcmpi((char *)l->newcontext,(char *)l->curcontext) != 0)
					{
						l->ccode = NWDSSetContext(l->Context,
							DCK_NAME_CONTEXT,l->newcontext);

						lstrcpy((char *)Password,(char *)l->HoldPassword);
						l->ccode = NWDSLogin(l->Context,0,(char *)l->UserBuf,(char *)Password,~0);
						l->ccode = ConvertDSToBinderyCodes(l->ccode);
						/*
						 * If we got logged in then authenticate to the server,
						 * otherwise restore the context to the original context.
						 */
						if (l->ccode == 0)
						{
							errcode = DS_SUCCESS;
							l->ccode = NWDSAuthenticate(l->ConnectionID,
								(unsigned long)NULL,NULL);
							if (l->ccode != 0)
							{
								errcode = AUTHENTICATION_DENIED;
							}
						}
						else if (l->ccode == PASSWORD_HAS_EXPIRED)
						{
							errcode = DS_SUCCESS_PE;
							l->ccode = NWDSAuthenticate(l->ConnectionID,
								(unsigned long)NULL,NULL);
							if (l->ccode != 0)
							{
								errcode = AUTHENTICATION_DENIED;
							}

							l->graceLoginLimit = l->graceLogins = -1L;
							NWDSCanonicalizeName(l->Context,(char *)l->UserBuf,(char *)l->HoldBuf);
							wsprintf((char *)l->UserBuf,".%s",(char *)l->HoldBuf);
							l->ccode = ReadAttribute((char *)l->UserBuf,"Login Grace Limit",
								(void **)&l->attrVal,&l->syntaxID);
							if (l->ccode == 0)
							{
								l->graceLoginLimit = (long)*l->attrVal;
							}
							if (l->attrVal != 0)
							{
								free(l->attrVal);
							}
							if (l->graceLoginLimit != -1)
							{
								l->ccode = ReadAttribute((char *)l->UserBuf,"Login Grace Remaining",
									(void **)&l->attrVal,&l->syntaxID);
								if (l->ccode == 0)
								{
									l->graceLogins = (long)*l->attrVal;
								}
								if (l->attrVal != 0)
								{
									free(l->attrVal);
								}
							}

							if (l->graceLoginLimit != -1)
							{
								DSALMGetMessage(PWD_EXPIRED,(char *)l->HoldBuf,sizeof(l->HoldBuf));
								wsprintf((char *)l->message,(char *)l->HoldBuf,(char *)l->graceLogins);
								DisplayTextMessage((char *)l->message,NO_ICON);
							}
						}
					}
				}
			}
			else
			{
				/*
				 * We were denied the login.
				 */
				errcode = AUTHENTICATION_DENIED;
			}
			NWDSFreeContext(l->Context);
			goto Terminate;
		}
	}

	/*
	 * If we wanted a NDS attachment and we are here it means that the
	 * desired server is a bindery server or we somehow got out of the
	 * code above without terminating, so error out.
	 */
	if (l->DSConnection == BINDERY && LoginType == NDS_LOGIN)
	{
		errcode = DS_TO_BINDERY_SERVER;
		goto Terminate;
	}
	else if (LoginType == NDS_LOGIN)
	{
		errcode = AUTHENTICATION_DENIED;
		goto Terminate;
	}
	
	/*
	 * Retrieve status of the current connection.
	 */
	DSConnStatus(l->ConnectionID, &l->ConnStatus);

	/*
	 * We are going to authenticate in bindery mode, so remove any
	 * context stuff from the user name and abbreviate to just the
	 * CN (object name).
	 */
	DSRemoveNameFromFullName((char *)UserName,(char *)l->curcontext,(char *)UserName,NULL);


	/*
	 * If we are already logged into the desired server, then return
	 * that the authentication was okay, if appropriate.
	 */
	if ((l->ConnStatus & CONNECTION_LOGGED_IN) != 0)
	{
		/*
		 * Go and get the id of the currently logged in user
		 */
		if (NWGetConnectionNumber(l->ConnectionID,&l->ConnectionNum) != 0)
		{
			errcode = SYSTEM_ERROR;
			goto Terminate;
		}
		if (NWGetConnectionInformation(l->ConnectionID,l->ConnectionNum,
			(char *)l->ObjName,&l->ObjType,&l->ObjId,l->LogTime) != 0)
		{
			errcode = SYSTEM_ERROR;
			goto Terminate;
		}
		/*
		 * If we are not the same user as currently logged in, then drop
		 * the connection and fall down below to perform a bindery connection.
		 *
		 * If we are the same user and we are already in bindery then 
		 * we have a success, otherwise drop the DS connection and fall down
		 * below to do a bindery attachment.
		 */
		if (strcmpi((char *)UserName,(char *)l->ObjName) != 0)
		{
			if (NWLogoutFromFileServer(l->ConnectionID) != 0)
			{
				errcode = SYSTEM_ERROR;
				goto Terminate;
			}
		}
		else
		{
			if ((l->ConnStatus & CONNECTION_NDS) == 0)
			{
				errcode = BINDERY_SUCCESS;
				goto Terminate;
			}
			if (NWLogoutFromFileServer(l->ConnectionID) != 0)
			{
				errcode = SYSTEM_ERROR;
				goto Terminate;
			}
		}
	}
	
	/*
	 * Now let's do a bindery connection.  We have to because it is either
	 * another tree or the server is not a DS server.
	 */

	NWDSSetConnectionInfo(l->ConnectionID,CONVERT_TO_BINDERY);

	/*
	 * Bindery connections to NDS servers requires a different capitalization
	 * scheme then 3x servers, so capitalize the username and password
	 * appropriately.
	 */
	if (l->DSConnection == NDS)
	{
		DSStrupr((char *)Password);
		DSStrupr((char *)UserName);
	}
	else
	{
		NWLSTRUPR((char *)Password);
		NWLSTRUPR((char *)UserName);
	}
	l->ccode = NWLoginToFileServer(l->ConnectionID,(char *)UserName,OT_USER,(char *)Password);
	if (l->ccode == 0)
	{
		errcode = BINDERY_SUCCESS;
	}
	else if (l->ccode == NO_SUCH_OBJECT)
	{
		errcode = NO_OBJECT_BAD_PASSWORD;
	}
	else if (l->ccode == PASSWORD_HAS_EXPIRED)
	{
		errcode = BINDERY_SUCCESS_PE;
		l->graceLoginLimit = l->graceLogins = -1L;
		l->ccode = NWReadPropertyValue(l->ConnectionID,(char *)UserName,OT_USER,
			"LOGIN_CONTROL",1,(BYTE *)&l->restrict,&l->mseg,(BYTE *)NULL);
		if (strcmpi((char *)UserName,"SUPERVISOR") != 0)
		{
	 		if (l->restrict.passwordGraceLogins != 0xff)
			{
				DSALMGetMessage(PWD_EXPIRED,(char *)l->HoldBuf,sizeof(l->HoldBuf));
				wsprintf((char *)l->message,(char *)l->HoldBuf,l->restrict.passwordGraceLogins);
				DisplayTextMessage((char *)l->message,NO_ICON);
			}
		}
	}
	else
	{
		/* Denied access */
		errcode = AUTHENTICATION_DENIED;
	}

Terminate:
	FillMemory(Password,0,sizeof(Password));
	if (l != 0)
	{
		FillMemory(l->HoldPassword,0,sizeof(l->HoldPassword));
		delete l;
	}

	return(errcode);
}

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ConvertDSToBinderyCodes() )

 NAME         : ConvertDSToBinderyCodes(NWCCODE ccode)

 DESCRIPTION  : This will take the DS return codes and turn them into
 		the equivalent Bindery codes. This makes the control flow easier
		in the main code -- only having to check for a single return code
		rather than all the Bindery and NDS codes.

 PARAMETERS   :   -> input          <-output 
                ->  ccode = code to convert

 RETURN       :  WORD
 		converted code or the same code passed in if not converted

 MODIFICATIONS: February 18, 1994

 END_MANUAL_ENTRY
============================================================================*/
WORD ConvertDSToBinderyCodes (NWCCODE ccode)
{
 	NWCCODE	newccode;

	switch (ccode)
	{
		case DSERR_BINDERY_LOCKED:							/* FF02 */
			newccode = SUPERVISOR_HAS_DISABLED_LOGIN;	/* 89FE */
			break;
	
		case DSERR_DUPLICATE_PASSWORD:
			newccode = PASSWORD_NOT_UNIQUE;
			break;

		case DSERR_PASSWORD_TOO_SHORT:					/* FF28 */
			newccode = PASSWORD_TOO_SHORT;				/* 89D8 */
			break;
		
		case DSERR_BAD_PASSWORD:							/* ff22 */
			newccode = PASSWORD_HAS_EXPIRED_NO_GRACE;	/* 89DE */
			break;
		
		case DSERR_PASSWORD_EXPIRED:						/* FF21 */
			newccode = PASSWORD_HAS_EXPIRED;				/* 89DF */
			break;
		
		case DSERR_NO_LOGIN_CONN_AVAILABLE:
			newccode = LOGIN_DENIED_NO_CONNECTION;
			break;
		
		case DSERR_NO_ACCOUNT_BALANCE:
			newccode = LOGIN_DENIED_NO_ACCOUNT_BALANCE;
			break;
		
		case DSERR_CREDIT_LIMIT_EXCEEDED:
			newccode = LOGIN_DENIED_NO_CREDIT;
			break;
		
		case DSERR_LOG_ACCOUNT_EXPIRED:					/* FF24 */
			newccode = ACCOUNT_DISABLED;					/* 89DC */
			break;
		
		case DSERR_LOGIN_LOCKOUT:							/* FF3B */
			newccode =  INTRUDER_DETECTION_LOCK;		/* 89C5 */
			break;
		
		case ERR_FAILED_AUTHENTICATION:					/* fd63 */
			newccode = NO_SUCH_OBJECT_OR_BAD_PASSWORD;/* 89FF */
			break;
		
		case DSERR_BAD_LOGIN_TIME:							/* ff26 */
			newccode = UNAUTHORIZED_LOGIN_TIME;			/* 89da */
			break;
		
		case DSERR_MAXIMUM_LOGINS_EXCEEDED:				/* ff27 */
			newccode = ERR_MAXIMUM_LOGINS_EXCEEDED;	/* 89d9 */
			break;

		/*
		 * ERR_NO_SUCH_VALUE can be returned from DS when the user name
		 * exists as another object type, which is the same as if it does
		 * not exist
		 */
		case ERR_NO_SUCH_VALUE:								/* fda6 */
			newccode = ERR_NO_SUCH_ENTRY;					/* fda7 */
			break;

		default:
			newccode = ccode;
	}

	return(newccode);
}

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DisconnectDSServers() )

 NAME         : DisconnectDSServers(NWCONN_HANDLE pConnID)

 DESCRIPTION  : This function will go through the active connection table
 		and tear down all connections to DS servers where we are not logged
		in.  This is basically to remove all attached DS servers.  The 
		connection handle passed in is protected from being dropped.

		This is typically necessary if the user desires to change the 
		preferred DS tree.  Apparently this will not take effect unless all
		attached DS servers are deleted.

 PARAMETERS   :   -> input          <-output 
                ->  pConnID = connection to maintain

 RETURN       :  VOID

 MODIFICATIONS: February 18, 1994

 END_MANUAL_ENTRY
============================================================================*/
static void DisconnectDSServers (NWCONN_HANDLE pConnID)
{
	struct locals
	{
		NWLPT	pindex; 						/* Printer index							*/

		NWCCODE	ccode;	 				/* Preset for netx.com to not error */

		WORD	maxConnects;				/* Maximum connections supported		*/
		WORD	numActive;					/* Number of active connections		*/
		WORD	connType;					/* Connection status						*/

		NWCONN_HANDLE	*connectList;	/* Buffer for connection list			*/

		NWCAPTURE_FLAGS1 capFlagsRW;	/* Capture flags							*/
		NWCAPTURE_FLAGS2 capFlagsRO;	/* Capture flags							*/
 
		NWDSContextHandle	context;		/* Current context						*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->ccode = 0;

	/*
	 * End capture all printers that are assigned to any DS servers,
	 * since we will be blowing them away soon.
	 */
	for (l->pindex = 1; l->pindex < MAXPRINTERS; l->pindex++)
	{
		/*
		 * check to see if there is a connection ID for a printer
		 */
		if (NWGetCaptureFlags(l->pindex,&l->capFlagsRW,&l->capFlagsRO) == 0 &&
			l->capFlagsRO.connID != 0 && l->capFlagsRO.connID != pConnID)
		{
			DSConnStatus(l->capFlagsRO.connID,&l->connType);
			if ((l->connType & CONNECTION_NDS) != 0)
			{
				NWEndCapture(l->pindex);
			}
		}
	}

	/*
	 * No go through and delete all of the DS connections that we are not
	 * currently logged into.  Be sure and not delete our current connection
	 * that was passed in.
	 */
	do
	{
		/*
		 * Allocate space for the maximum number of connections
 		 */
		NWGetMaximumConnections(&l->maxConnects);
		l->connectList =
			(NWCONN_HANDLE *)malloc(l->maxConnects * sizeof(NWCONN_HANDLE));
		l->ccode = NWGetConnectionList(0,l->connectList,l->maxConnects,
			&l->numActive);
		while (l->numActive > 0)
		{
			l->numActive--;
			DSConnStatus(l->connectList[l->numActive],&l->connType);
			/*
			 * NDS connections hang around so look for nds connections
			 * not logged in or authenticated and free the connection
			 * then return
			 */
			if (l->connectList[l->numActive] != pConnID &&
				(l->connType & CONNECTION_NDS) != 0 &&
	 			(l->connType & CONNECTION_LOGGED_IN) == 0)
			{
 				NWFreeConnectionSlot(l->connectList[l->numActive],
					SYSTEM_DISCONNECT);
			}
		}
		free(l->connectList);
	} while (l->ccode == CONNECT_LIST_OVERFLOW);

	/*
	 * If we are authenticated then we need to log out of a tree -- since
	 * we should only be calling this function if we are eliminating all
	 * DS connections.
	 */
	if (NWIsDSAuthenticated())
	{
 		if ((l->context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			goto Terminate;
		}
		NWDSLogout(l->context);
 		NWDSFreeContext(l->context);
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return;
}

