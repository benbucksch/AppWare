/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 *
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

/*
 * This file contains the functions necessary for the Login ALM
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
#include "parse.h"

#ifdef __cplusplus

extern "C" {

#endif

#include <dstools.h>

#ifdef __cplusplus

}

#endif

#define MAXMESSAGE					1024

/*
 * ======================================================================
 * Macro Definitions
 * ======================================================================
 */

/* Flows */
#define LOGIN_DS_OK		 	1			/* NDS login done				*/
#define LOGIN_BINDERY_OK	2			/* Bindery login done		*/
#define LOGIN_ERROR			3			/* Error							*/

#define EXE_LOGINSCRIPTS_OK		1
#define EXE_LOGINSCRIPTS_ERROR	2

/* Error Codes */
#define MAXTIME						10			/* Time length						*/
#define OT_TREE_NAME				0x7802	/* Object type of a tree name	*/
#define PROFILE_OBJ_TYPE			0x3501	/* Object type for profile		*/

#define INITIALTOKENCOUNT			180		/* Initial tokens for MX 		*/
#define MAXTOKENCOUNT				250		/* Maximum tokens for MX		*/

/*
 * ======================================================================
 *	LOGIN module prototypes
 * ======================================================================
 */
extern WORD getConnectionToTree(NWCONN_HANDLE *connectionID, char *server,
	char *tree);
extern WORD ConvertDSToBinderyCodes(NWCCODE ccode);
extern WORD DoLogout(BYTE *ServerName, WORD logoutTree);
extern WORD DoScript(BYTE *ScriptFile, WORD scripttype, WORD *exitflag,
	AMEMBLOCKID *Errors, WORD output, PSPACE *p);
extern WORD DoWinScript(BYTE *SFile, WORD *eflag, AMEMBLOCKID *Errs);
extern WORD DoLoginWinScripts (NWCONN_HANDLE ConnID, BYTE *username, WORD stype,
	BYTE *script, BYTE *profile, AMEMBLOCKID *output, PSPACE *pg);

WORD DoLogin(BYTE *ServerName, BYTE *UserName, BYTE *Password,
	WORD LoginType, BYTE *Script, BYTE *Profile, AMEMBLOCKID *Output);

WORD LoginToServer (BYTE *UserName, BYTE *Password,
	WORD *LoginType, NWCONN_HANDLE *ConnectionID);
WORD DoLoginScripts (NWCONN_HANDLE ConnectionID,BYTE *UserName,
	WORD ScriptType, BYTE *InputScript, BYTE *InputProfile, 
	AMEMBLOCKID *output, PSPACE *parsevars);

#if LOGIN IS_DISABLED
/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( Login() )

 NAME       : Login(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION: This function will Login a workstation to the
 		identified server.  Inputs required are a server or tree name, the
		users name.  Optional inputs is a password.

		This function will retrieve all the parameters from the appbuilder
		and then call DoLogin to perform the login.
 
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
		LOGIN_DS_OK		 	1			NDS login done
		LOGIN_BINDERY_OK	2			Bindery login done
		LOGIN_ERROR			3			Error

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK Login (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE	UserName[MAX_DN_BYTES];			 	/* Passed in user name 			*/
		BYTE 	Password[PASSWORD_BUFFER_SIZE];	/* Passed in password			*/
		BYTE 	ServerName[SERVER_NAME_SIZE];		/* Server/tree passed in		*/
		BYTE 	Script[MAX_DN_BYTES];				/* Server/tree passed in		*/
		BYTE 	Profile[MAX_DN_BYTES];				/* Server/tree passed in		*/

		NWDSContextHandle	Context;					/* Current context				*/

		long					inputtype;

		WORD 					DSAuthen;				/* Flag for DS authenticated	*/
		WORD					LoginType;				/* Flag for login type			*/

		NWCCODE				ccode;					/* Return Codes 					*/

		HCURSOR				Oldcursor;
		HCURSOR				Newcursor;

		AMEMBLOCKID			Output;

		primaryConnectionStruct	pc;

		AOBJECTID				oiUserName;				/* User name ALM object			*/
		AOBJECTID				oiPassword;				/* Password ALM object			*/
		AOBJECTID				oiServerName;			/* Server ALM object				*/
		AOBJECTID				oiLoginType;			/* Login Type ALM object		*/
		AOBJECTID				oiScript;				/* Login Script ALM object		*/
		AOBJECTID				oiProfile;				/* Login Profile ALM object 	*/
		AOBJECTID				oiOutput;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = LOGIN_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = LOGIN_DS_OK;
	l->Output = 0;
	l->oiOutput = AObjCreate(OTYPE_LIST);

	/*
	 * Retrieve the server/tree parameter from the Visual Appbuilder.
	 * This field is NOT OPTIONAL.
	 */
	FillMemory(l->ServerName,0,sizeof(l->ServerName));
	l->oiServerName = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (l->oiServerName != 0)
	{
		OTextGetToBuffer(l->oiServerName,(char *)l->ServerName,SERVER_NAME_SIZE - 1);
		NWLSTRUPR((char *)l->ServerName);
	}
	if (*(l->ServerName) == NULL)
	{
		DisplayIDMessage(NO_SERVER,NO_ICON);
		theSystem->message1 = LOGIN_ERROR;
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
	FillMemory(l->UserName,0,sizeof(l->UserName));
	l->oiUserName = AFuncGetTypedParameter(2, OTYPE_TEXT);
	if (l->oiUserName != 0)
	{
		OTextGetToBuffer(l->oiUserName,(char *)l->UserName,MAX_DN_BYTES - 1);
	}
	if (*(l->UserName) == NULL)
	{
		/*
		 * If no user was specified, then if we are authenticated, then
		 * use the current user name as the default.  If we are in
		 * the bindery world, we must have a user name.
		 */
		l->DSAuthen = NWIsDSAuthenticated();
		if (l->DSAuthen == TRUE)
		{
			if ((l->Context = NWDSCreateContext()) ==
				(NWDSContextHandle)ERR_CONTEXT_CREATION)
			{
				DisplayIDMessage(CANNOT_CREATE_CONTEXT,NO_ICON);
				theSystem->message1 = LOGIN_ERROR;
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
				theSystem->message1 = LOGIN_ERROR;
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
		OTextGetToBuffer(l->oiPassword,(char *)l->Password,PASSWORD_BUFFER_SIZE - 1);
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
			theSystem->message1 = LOGIN_ERROR;
			goto Terminate;
		}
	}

	/*
	 * Get the script filename from the AppBuilder.  This field may be blank
	 * because the user may want the defaults.
	 *
	 * This field is OPTIONAL.
	 */
	FillMemory(l->Script,0,sizeof(l->Script));
	l->oiScript = AFuncGetTypedParameter(5, OTYPE_TEXT);
	if (l->oiScript != 0)
	{
		OTextGetToBuffer(l->oiScript,(char *)l->Script,MAX_DN_BYTES - 1);
	}

	/*
	 * Get the profile from the AppBuilder.  This field may be blank
	 * because the user may want the defaults.
	 *
	 * This field is OPTIONAL.
	 */
	FillMemory(l->Profile,0,sizeof(l->Profile));
	l->oiProfile = AFuncGetTypedParameter(6, OTYPE_TEXT);
	if (l->oiProfile != 0)
	{
		OTextGetToBuffer(l->oiProfile,(char *)l->Profile,MAX_DN_BYTES - 1);
	}


	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		theSystem->message1 = LOGIN_ERROR;
		goto Terminate;
	}

	l->Newcursor = LoadCursor(NULL,IDC_WAIT);
	if (l->Newcursor != NULL)
	{
		l->Oldcursor = SetCursor(l->Newcursor);
	}
	l->ccode = DoLogin(l->ServerName,l->UserName,l->Password,l->LoginType,
		l->Script,l->Profile,&l->Output);
	if (l->Newcursor != NULL)
	{
		SetCursor(l->Oldcursor);
	}
	if (l->ccode != DS_SUCCESS && l->ccode != BINDERY_SUCCESS)
	{
		DisplayIDMessage(l->ccode,NO_ICON);
		theSystem->message1 = LOGIN_ERROR;
	}
	else
	{
		theSystem->message1 = LOGIN_DS_OK;
		if (l->ccode == BINDERY_SUCCESS)
		{
			theSystem->message1 = LOGIN_BINDERY_OK;
		}
	}

Terminate:
	if (l != 0)
	{
		FillMemory(l->Password,0,sizeof(l->Password));
		if (l->oiOutput != 0 && l->Output != 0)
		{
			MyOutputList(l->oiOutput,&l->Output,1);
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

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DoLogin() )

 NAME         : DoLogin (pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  : This function will Login a workstation to the
 		identified server.

		This function will first assume that the server passed in is a 
		server and will attempt to attach to it.  If the attach fails, then
		we assume that it was a tree and attempt to attach to a server
		in the tree.

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
		NO_LOGIN_PNW				 Cannot log into PNW


 MODIFICATIONS: February 18, 1994

 END_MANUAL_ENTRY
============================================================================*/
WORD DoLogin (BYTE *ServerName, BYTE *UserName, BYTE *Password,
	WORD LoginType, BYTE *Script, BYTE *Profile, AMEMBLOCKID *Output)
{
	WORD					errcode;						/* return error code				*/
	struct locals
	{
		BYTE 	HoldPassword[PASSWORD_BUFFER_SIZE];	/* Password holder			*/
		BYTE 	TreeName[MAX_TREE_NAME_CHARS+1];	 /* Tree of desired server		*/
		BYTE	UserBuf[MAX_DN_BYTES];				 /* Temporary user name buf  	*/
		BYTE 	curcontext[MAX_DN_BYTES];			 /* Current context buffer		*/
		BYTE	newcontext[MAX_DN_BYTES];			 /* New context buffer			*/
		BYTE	HoldBuf[MAXMESSAGE];
		BYTE	message[MAXMESSAGE];
		BYTE	*attrVal;


		long					graceLoginLimit;
		long					graceLogins;
		uint32				syntaxID;
	
		WORD					ConnStatus;				 /* Connection info				*/
		WORD					junk;						 /* Dummy variable			  	*/

		NWCONN_HANDLE		ConnectionID;			 /* Connection ID for current	*/
		NWCONN_HANDLE		tmpID;					 /* Temporary Connection ID  	*/

		NWDSContextHandle	Context;					 /* Current context				*/

		WORD 					DSConnection;			 /* Flag for DS connection		*/

		NWCCODE				ccode;					 /* Return Codes 					*/

		PSPACE		parsevars;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialization
	 */
	FillMemory(l->HoldPassword,0,sizeof(l->HoldPassword));
	lstrcpy((char *)l->HoldPassword,(char *)Password);
	errcode = DS_SUCCESS;
	l->Context = ERR_CONTEXT_CREATION;
	FillMemory(&l->parsevars,0,sizeof(l->parsevars));

	l->ccode = AttachtoServer(ServerName,&l->ConnectionID);
	if (l->ccode != 0)
	{
		errcode = l->ccode;
		goto Terminate;
	}

	/*
	 * Don't allow logging into Personal NetWare with this utility.
	 */
	DSConnStatus(l->ConnectionID, &l->ConnStatus);
	if ((l->ConnStatus & CONNECTION_PNW) != 0)
	{
		errcode = NO_LOGIN_PNW;
		goto Terminate;
	}

	/*
	 * Logout of all servers.
	 */
	DoLogout((unsigned char *)"",FALSE);

	l->ccode = AttachtoServer(ServerName,&l->ConnectionID);
	if (l->ccode != 0)
	{
		errcode = l->ccode;
		goto Terminate;
	}
	NWSetPrimaryConnectionID(l->ConnectionID);

	/*
	 * Login to the desired server.
	 */
	l->ccode = LoginToServer(UserName,Password,&LoginType,
		&l->ConnectionID);

	/*
	 * Find out of the server is a DS server and save the server's
	 * tree name.
	 */
	l->DSConnection =
		NWIsDSServer(l->ConnectionID, (char *)l->TreeName) ? NDS : BINDERY;
	if ((l->Context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		errcode = CANNOT_CREATE_CONTEXT;
		goto Terminate;
	}

	if (l->ccode == (NWCCODE)ERR_NO_SUCH_ENTRY && l->DSConnection == NDS)
	{
		/*
		 * We must be an unknown user to the desired server.  This
		 * may be correct if we gave a bad user or a password, but
		 * this will also happen if our current context is not 
		 * correct for the user.  Since we know we want to login to
		 * this server, change our current context to the same as the
		 * server and see if the user name is know at that level.
		 */
		if (NWDSGetServerDN(l->Context,l->ConnectionID,(char *)l->newcontext) == 0 &&
			NWDSCanonicalizeName(l->Context,(char *)l->newcontext,(char *)l->newcontext) == 0)
		{
			NWDSCanonicalizeName(l->Context,(char *)UserName,(char *)l->UserBuf);
			DSRemoveNameFromFullName((char *)l->UserBuf,(char *)l->curcontext,NULL,(char *)l->UserBuf);
			DSRemoveNameFromFullName((char *)l->newcontext,(char *)l->newcontext,NULL,NULL);
			/*
			 * Now we have the server and the current context.  If they
			 * happen to be the same, then don't bother trying to login
			 * because we will just try the same user name.  But if the
			 * context is different, then set the current context to the
			 * server's context and attempt another login.
			 */
			if (strcmpi((char *)l->newcontext,(char *)l->curcontext) != 0)
			{
				l->ccode = NWDSSetContext(l->Context,DCK_NAME_CONTEXT,
					l->newcontext);
				lstrcpy((char *)Password,(char *)l->HoldPassword);
				l->ccode = LoginToServer(UserName,Password,&LoginType,
					&l->ConnectionID);
				l->ccode = ConvertDSToBinderyCodes(l->ccode);
			}
		}
	}
	
	if (l->ccode == PASSWORD_HAS_EXPIRED)
	{
		errcode = DS_SUCCESS_PE;
		l->ccode = 0;
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
	
	if (l->ccode != 0 && l->ccode != CONNECTION_LOGGED_IN)
	{
		DoLogout((unsigned char *)"",FALSE);
		errcode = NO_OBJECT_BAD_PASSWORD;
		goto Terminate;
	}
	if (NWGetConnectionHandle(ServerName,GLOBAL,&l->tmpID,&l->junk) == 0)
	{
		l->ConnectionID = l->tmpID;
	}
	if (NWSetPrimaryConnectionID(l->ConnectionID) != 0)
	{
		errcode = BAD_PRIMARY_CONNECTIONID;
		goto Terminate;
	}
	
	FillMemory(&l->parsevars,0,sizeof(l->parsevars));
	DSConnStatus(l->ConnectionID, &l->ConnStatus);
	lstrcpy((char *)l->parsevars.primaryLoginConnection.userName,(char *)UserName);
	lstrcpy((char *)l->parsevars.primaryLoginConnection.serverName,(char *)ServerName);
	lstrcpy((char *)l->parsevars.primaryLoginConnection.password,(char *)Password);
	l->parsevars.primaryLoginConnection.connectionID = l->ConnectionID;
	l->parsevars.primaryLoginConnection.serverVersion = l->DSConnection;
	l->parsevars.primaryLoginConnection.serverLoginType = 
		((l->ConnStatus & CONNECTION_NDS) != 0) ? NDS : BINDERY;

	if (l->parsevars.primaryLoginConnection.serverLoginType == NDS)
	{
		NWDSCanonicalizeName(l->Context,(char *)UserName,
			(char *)l->parsevars.primaryLoginConnection.userName);
	}
	l->ccode = DoLoginWinScripts(l->ConnectionID,UserName,ALL_SCRIPTS,Script,
		Profile,Output,&l->parsevars);

	if (l->parsevars.primaryLoginConnection.serverLoginType != NDS)
	{
		if (errcode == DS_SUCCESS_PE)
		{
			errcode = BINDERY_SUCCESS_PE;
		}
		else
		{
			errcode = BINDERY_SUCCESS;
		}
	}

Terminate:
	if (l != 0)
	{
		FillMemory(Password,0,sizeof(Password));
		FillMemory(l->HoldPassword,0,sizeof(l->HoldPassword));
		if (l->Context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
 			NWDSFreeContext(l->Context);
		}
		UpdateLoginTime(l->parsevars.primaryLoginConnection.connectionID,
		  (char *)	l->parsevars.primaryLoginConnection.userName,&l->parsevars);
		delete l;
	}

	return(errcode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( LoginToServer() )

 NAME         : LoginToServer()

 DESCRIPTION  : Given the server\username and password (and context if
 						Directory services), this code will attempt to attach and 
						login the given username.

 PARAMETERS   :   -> input          <-output 
              -> Server - - - - Name of the server to login to
					-> UserName - - - Name of the user to login as
					-> Password - - - The users password
				 <--> LoginType - - - The login type to try
					<- connectionID - Returns the connection ID of server logged into

 RETURN       : 0 if successful, otherwise the completion code from the call
 						to AttachToServer, or NWGetFileSErverType, or NWDSLogin,
						or NWDSAuthenticate, or NWLoginToFileServer.

 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD LoginToServer (BYTE *UserName, BYTE *Password,
	WORD *LoginType, NWCONN_HANDLE *ConnectionID)
{
	WORD					errcode;
	struct locals
	{
		BYTE 	TreeName[MAX_TREE_NAME_CHARS+1];	 /* Tree of desired server		*/
 		
		NWCCODE				ccode;

		NWDSContextHandle	context;					 /* Current context				*/

		WORD 					Connection;				 /* Flag for DS connection		*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	if ((l->context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		errcode = CANNOT_CREATE_CONTEXT;
		goto Terminate;
	}

	/*
	 * Go and get the type of server.  If the server is a bindery server,
	 * then we can only log into the system in bindery mode.
	 */
	l->Connection = NWIsDSServer(*ConnectionID, (char *)l->TreeName) ? NDS : BINDERY;
	if (l->Connection == BINDERY)
	{
		*LoginType = BINDERY_LOGIN;
	}

	if (*LoginType != BINDERY_LOGIN)
	{
		*LoginType = NDS_LOGIN;
 		l->ccode = NWDSSetConnectionInfo(*ConnectionID, CONVERT_TO_DS);
		l->ccode = NWDSLogin(l->context, 0, (char *)UserName, (char *)Password, ~0);
		errcode = ConvertDSToBinderyCodes(l->ccode);
	}
	else
	{
		l->ccode = NWDSSetConnectionInfo(*ConnectionID,CONVERT_TO_BINDERY);
		if (l->Connection == NDS)
		{
			DSStrupr((char *) Password);
			DSStrupr((char *)UserName);
		}
		else
		{
			NWLSTRUPR((char *)Password);
			NWLSTRUPR((char *)UserName);
		}
		errcode = NWLoginToFileServer(*ConnectionID,(char *)UserName,OT_USER,(char *)Password);
	}

Terminate:
	if (l != 0)
	{
		if (l->context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
 			NWDSFreeContext(l->context);
		}
		delete l;
	}

	return(errcode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DoLoginScripts() )

 NAME         : DoLoginScripts()

 DESCRIPTION  : Given the connection and username and the types of scripts
 		to run, this function will run all the available scripts.

 PARAMETERS   :   -> input          <-output 
 		-> ConnectionID ...... connection to server
		-> UserName........... user name
		-> ScriptType......... type of scripts to run

 RETURN	     	: WORD
 
 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD DoLoginScripts (NWCONN_HANDLE ConnectionID,BYTE *UserName,
	WORD ScriptType, BYTE *InputScript, BYTE *InputProfile, 
	AMEMBLOCKID *output, PSPACE *parsevars)
{
	struct locals
	{
		BYTE	Script[MAX_DN_BYTES];	  			/* Script file name				*/
		BYTE	profileName[MAX_DN_BYTES];
		BYTE	mess[2048];

		NWDSContextHandle	Context;		  			/* Current context				*/
 
		NWCCODE				ccode;

		WORD					exitflag;
 
		uint32				syntaxID;

		unsigned long		objectID;

		BYTE					*profile;
	} *l;


	l = new struct locals;
	if (l == 0)
	{
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->Context = ERR_CONTEXT_CREATION;

	if (*InputScript != 0)
	{
		l->ccode = DoScript(InputScript,0,&l->exitflag,output,1,
			parsevars);
		goto Terminate;
	}

	if ((ScriptType & SYSTEM_SCRIPT) != 0)
	{
		FillMemory(l->Script,0,sizeof(l->Script));
		if (parsevars->primaryLoginConnection.serverLoginType == NDS)
		{
 			if ((l->Context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
			{
				DisplayIDMessage(CANNOT_CREATE_CONTEXT,NO_ICON);
				goto Terminate;
			}
			l->Script[0] = '.';
			NWDSCanonicalizeName(l->Context,(char *)UserName,(char *)&l->Script[1]);
			DSRemoveNameFromFullName((char *)&l->Script[1],(char *)&l->Script[1],NULL,NULL);
		}
		else
		{
			wsprintf((char *)l->Script,DSALMGetMessage(SYSTEM_SCRIPT_FORMAT,(char *)l->mess,sizeof(l->mess)),
				(char *)(parsevars->primaryLoginConnection.serverName));
		}
		if (*(l->Script) != '\0')
		{
			l->ccode = DoScript(l->Script,SYSTEM_SCRIPT,&l->exitflag,
				output,1,parsevars);
			if (l->exitflag == TRUE)
			{
				goto Terminate;
			}
		}
	}

	if ((ScriptType & PROFILE_SCRIPT) != 0)
	{
		FillMemory(l->Script,0,sizeof(l->Script));
		if (InputProfile != NULL && *InputProfile != 0)
		{
			lstrcpy((char *)l->Script,(char *)InputProfile);
		}
		else
		{
			if (parsevars->primaryLoginConnection.serverLoginType == NDS)
			{
				l->ccode = ReadAttribute((char *)UserName,"Profile",(void **)&l->profile,
					&l->syntaxID);
				if (l->ccode == 0)
				{		
					lstrcpy((char *)l->Script,(char *)l->profile);
					free(l->profile);
				}
			}
			else
			{
				GetDefaultProfile(ConnectionID,(char *)UserName,(char *)l->profileName);
				if (NWGetObjectID(ConnectionID,(char *)l->profileName,PROFILE_OBJ_TYPE,
					&l->objectID) == 0)
				{
					wsprintf((char *)l->Script,DSALMGetMessage(USER_SCRIPT_FORMAT,(char *)l->mess,
						sizeof(l->mess)),
						(char *)(parsevars->primaryLoginConnection.serverName),
						NWLongSwap(l->objectID));
				}
			}
		}
		if (*(l->Script) != '\0')
		{
			l->ccode = DoScript(l->Script,PROFILE_SCRIPT,&l->exitflag,
				output,1,parsevars);
			if (l->exitflag == TRUE)
			{
				goto Terminate;
			}
		}
	}

	if ((ScriptType & USER_SCRIPT) != 0)
	{
		if (parsevars->primaryLoginConnection.serverLoginType == NDS)
		{
			lstrcpy((char *)l->Script,(char *)UserName);
		}
		else
		{
			NWGetObjectID(ConnectionID,(char *)UserName,OT_USER,&l->objectID);
			wsprintf((char *)l->Script,DSALMGetMessage(USER_SCRIPT_FORMAT,(char *)l->mess,sizeof(l->mess)),
				(char *)(parsevars->primaryLoginConnection.serverName),NWLongSwap(l->objectID));
		}
		if (*(l->Script) != '\0')
		{
			l->ccode = DoScript(l->Script,USER_SCRIPT,&l->exitflag,output,
				1,parsevars);
			if (l->ccode == (NWCCODE)ERR_NO_SUCH_VALUE ||
				l->ccode == (NWCCODE)ERR_NO_SERVER_FOUND ||
				l->ccode == (NWCCODE)ERR_NO_SUCH_ENTRY ||
				l->ccode == CANNOT_OPEN_SCRIPTFILE)
			{
				FillMemory(l->Script,0,sizeof(l->Script));
				l->ccode = DoScript(l->Script,USER_SCRIPT,&l->exitflag,output,1,
					parsevars);
			}
			if (l->exitflag == TRUE)
			{
				goto Terminate;
			}
		}
	}
	
Terminate:
	if (l != 0)
	{
		if (l->Context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
 			NWDSFreeContext(l->Context);
		}
		delete l;
	}

	return(0);
}

#if LOGIN IS_DISABLED
/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ExecuteLoginScripts() )

 NAME       : ExecuteLoginScripts(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION: This function will Login a workstation to the
 		identified server.  Inputs required are a server or tree name, the
		users name.  Optional inputs is a password.

		This function will retrieve all the parameters from the appbuilder
		and then call DoLogin to perform the login.
 
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
		LOGIN_DS_OK		 	1			NDS login done
		LOGIN_BINDERY_OK	2			Bindery login done
		LOGIN_DIFF_USER	3			Diff user/same tree
		LOGIN_ERROR			4			Error

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK ExecuteLoginScripts (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE	UserName[MAX_DN_BYTES];				 /* Passed in user name 		*/
		BYTE 	Password[PASSWORD_BUFFER_SIZE];	 /* Passed in password			*/
		BYTE 	Script[MAX_DN_BYTES];				 /* Server/tree passed in		*/
		BYTE 	Profile[MAX_DN_BYTES];				 /* Server/tree passed in		*/

		NWCCODE				ccode;					 /* Return Codes 					*/
																								
		AOBJECTID				oiUserName;				 /* User name ALM object		*/
		AOBJECTID				oiPassword;				 /* Password ALM object			*/
		AOBJECTID				oiScript;				 /* Login Script ALM object	*/
		AOBJECTID				oiProfile;				 /* Login Profile ALM object 	*/
		AOBJECTID				oiOutput;				 /* Login output ALM object 	*/

		AMEMBLOCKID			Output;					 /* Output of parsing			*/

		PSPACE		parsevars;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = EXE_LOGINSCRIPTS_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = EXE_LOGINSCRIPTS_OK;
	FillMemory(&l->parsevars,0,sizeof(l->parsevars));

	l->oiOutput = AObjCreate(OTYPE_LIST);
	l->Output = 0;

	/*
	 * Get the specified user name from AppBuilder.  If the user name
	 * is not specified and we are DS authenticated, then default the
	 * user name to the current user's name.  If we are bindery based
	 * then the user name must be specified.
	 *
	 * This field is OPTIONAL.
	 */
	FillMemory(l->UserName,0,sizeof(l->UserName));
	l->oiUserName = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (l->oiUserName != 0)
	{
		OTextGetToBuffer(l->oiUserName,(char *)l->UserName,MAX_DN_BYTES - 1);
	}

	/*
	 * Get the password from the AppBuilder.  This field may be blank
	 * because there may not be a password for this user.
	 *
	 * This field is OPTIONAL.
	 */
	FillMemory(l->Password,0,sizeof(l->Password));
	l->oiPassword = AFuncGetTypedParameter(2, OTYPE_TEXT);
	if (l->oiPassword != 0)
	{
		OTextGetToBuffer(l->oiPassword,(char *)l->Password,
			PASSWORD_BUFFER_SIZE - 1);
	}
	else
	{
		*(l->Password) = 0xFF;
	}

	/*
	 * Get the script filename from the AppBuilder.  This field may be blank
	 * because the user may want the defaults.
	 *
	 * This field is OPTIONAL.
	 */
	FillMemory(l->Script,0,sizeof(l->Script));
	l->oiScript = AFuncGetTypedParameter(3, OTYPE_TEXT);
	if (l->oiScript != 0)
	{
		OTextGetToBuffer(l->oiScript,(char *)l->Script,MAX_DN_BYTES - 1);
	}

	/*
	 * Get the profile from the AppBuilder.  This field may be blank
	 * because the user may want the defaults.
	 *
	 * This field is OPTIONAL.
	 */
	FillMemory(l->Profile,0,sizeof(l->Profile));
	l->oiProfile = AFuncGetTypedParameter(4, OTYPE_TEXT);
	if (l->oiProfile != 0)
	{
		OTextGetToBuffer(l->oiProfile,(char *)l->Profile,MAX_DN_BYTES - 1);
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		theSystem->message1 = EXE_LOGINSCRIPTS_ERROR;
		goto Terminate;
	}

	FillMemory(&l->parsevars,0,sizeof(l->parsevars));
	FillPrimaryConnInfo(&l->parsevars.primaryLoginConnection);
	if (*(l->UserName) != 0)
	{
		lstrcpy((char *)l->parsevars.primaryLoginConnection.userName,
			(char *)l->UserName);
	}
	else
	{
		lstrcpy((char *)l->UserName,(char *)l->parsevars.primaryLoginConnection.userName);
	}
	lstrcpy((char *)l->parsevars.primaryLoginConnection.password,(char *)l->Password);
	l->parsevars.se_perrors = &l->Output;

	l->ccode = DoLoginWinScripts(l->parsevars.primaryLoginConnection.connectionID,
		l->UserName,ALL_SCRIPTS,l->Script,l->Profile,&l->Output,&l->parsevars);

	if (l->ccode != 0)
	{
		DisplayIDMessage(ERR_EXE_LOGINSCRIPTS,NO_ICON);
		theSystem->message1 = EXE_LOGINSCRIPTS_ERROR;
	}
	else
	{
		theSystem->message1 = EXE_LOGINSCRIPTS_OK;
	}

Terminate:
	if (l != 0)
	{
		FillMemory(l->Password,0,sizeof(l->Password));
		if (l->oiOutput != 0 && l->Output != 0)
		{
			MyOutputList(l->oiOutput,&l->Output,1);
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


