/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

 /*
 * This file contains the functions necessary for the Script parsing ALM
 * functions.
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
#include <io.h>
#include <dos.h>
#include <dir.h>
#include <setjmp.h>
#include <stdarg.h>

#include <a_alm.h>		/* declares AppWare Bus API prototypes			*/
#include <o_text.h> 	/* kdeclares OTYPE_TEXT and text object ALMs	*/
#include <o_flag.h>
#include <o_list.h>

#define NWWIN
#include <constant.h>
#include <nwcalls.h>
#include <nwnet.h>
#include <nwintcon.h>
#include <dbcsmac.h>
#include <nwlocale.h>
#include <nwintern.h>
#include <nwundoc.h>
#define SYNCPASS_INTERNAL
#include "mx.h"
#include "utils.h"
#include "errors.h"
#include "alm_auth.h"
#include "resource.h"
#include "parse.h"
#include "cmdline.h"
#include "msupport.h"
#include "cmds.h"
#include "parseut.h"
#include "authwin.h"
#include "inerrors.h"

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

#define MAXPATHSIZE			256		/* Maximum path size						*/
#define MAXERRSTRING		1024		/* Maximum error string length		*/
#define MAX_STRING			256		/* Maximum script line 254				*/
#define MAX_SCRIPT_ARGS	40
#define MAX_NESTED_IF_DEPTH	16

#define DEFAULT_SCRIPT_ERROR	"Script error;>>%s<< file %s, line %d: %s"
#define BADOUTPUTMSG			"Bad message to ScriptOutput"
#define CANNOTGETMESSAGE		"Cannot get message %d"
#define DEFAULT_SCRIPT_QUESTION		"?"

/*
 * ======================================================================
 *	SCRIPT module prototypes
 * ======================================================================
 */
extern WORD DoScript(BYTE *ScriptFile, WORD scripttype, WORD *exitflag,
	AMEMBLOCKID *Errors, WORD output, PSPACE *p);
extern WORD DoMapdrive(BYTE *DriveName, BYTE *Path, int MapRoot);

/* Static function prototypes */
void ScriptError (WORD error, PSPACE *p, ...);

/*
 * ======================================================================
 *	Globals to file
 * ======================================================================
 */
static char *DefaultScript[] =
{
	"MAP DISPLAY OFF",
	"MAP ERRORS OFF",
	"MAP *1:=%FILE_SERVER/SYS:; *1:=%FILE_SERVER/SYS:%LOGIN_NAME",
	"IF \"%1\"=\"SUPERVISOR\" || \"%1\"=\"ADMIN\" THEN BEGIN",
	"	MAP *1:=%FILE_SERVER/SYS:SYSTEM",
	"END",
	"MAP INS S1:=%FILE_SERVER/SYS:PUBLIC",
	"MAP INS S2:=%FILE_SERVER/SYS:PUBLIC/%MACHINE/%OS/%OS_VERSION",
	"MAP DISPLAY ON",
	"MAP"
};
#define DEFAULTSCRIPTSIZE sizeof(DefaultScript)/sizeof(DefaultScript[0])

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( SynchTime() )

 MODULE       : init.c

 GLOBALS USED : 


 SYNTAX       : SynchTime(void)

 PARAMETERS   :   -> input          <-output 


 RETURN       : None


 DESCRIPTION  : Synchronizes the workstation time with the server.



 ALGORITHM    : Get Server Date & Time
 						Set Workstation Date & Time



 END_MANUAL_ENTRY
****************************************************************************/
void SynchTime(PSPACE *p)
{

struct dosdate_t wsdate;
struct dostime_t wstime;
	
NWCONN_HANDLE connID;

BYTE time[7]; 
MYDATETIME now;
int ccode;

	/* sync workstation time with server time */
	if ((ccode = NWGetPrimaryConnectionID(&connID)) != 0)
	{
		ScriptError(SCRIPT_UNEXPECTED_INTERNAL_ERROR,p,SCRIPTSCPP+1,ccode);
	}
	else if ((ccode = NWGetFileServerDateAndTime(connID, time)) != 0)
	{
		ScriptError(SCRIPT_UNEXPECTED_INTERNAL_ERROR,p,SCRIPTSCPP+2,ccode);
	}
	DosGetDateTime(&now);
	now.year = time[0] + (time[0] > 82? 1900: 2000);
	now.month = time[1];
	now.day = time[2];
	now.hours = time[3];
	now.minutes = time[4];
	now.seconds = time[5];

	wsdate.month = now.month;
	wsdate.day = now.day;
	wsdate.year = now.year;
	wstime.hour = now.hours;
	wstime.minute = now.minutes;
	wstime.second = now.seconds;
	wstime.hsecond = 0;
	if ((_dos_setdate(&wsdate) != 0) || (_dos_settime(&wstime) != 0))
	{
		ScriptError(SCRIPT_DATE_TIME,p);
	}
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CompressLine() )

 MODULE       : script.c

 GLOBALS USED : 


 SYNTAX       : CompressLine(char *line)

 PARAMETERS   :   -> input          <-output 
              -> line- - - - A line to convert (compress).


 RETURN       : None


 DESCRIPTION  : removes any excess white space and converts line
						to upper case except within quotes or after a '#'.


 ALGORITHM    : 



 END_MANUAL_ENTRY
****************************************************************************/
static int CompressLine (char *line)
{
	char 				*src;
	char				*dest;
	char				temp[MAX_STRING + 1];

	register int	firstChar = 1;

	dest = temp;
	*dest = ' ';
	for (src = line; *src != 0; src = NWNextChar(src))
	{
		switch (NWCharVal(src))
		{
		case ' ':
		case '\t':
			if (NWISALNUM(*dest) || *dest == '.' || *dest == ':')
			{
				dest = NWNEXTCHAR(dest);
				*dest = ' ';
			}
			break;

		case '"':
			if (*dest != ' ')
			{
				dest = NWNEXTCHAR(dest);
			}
			CopyMemory(dest, src, NWNUMBEROFBYTES(src));
			dest = NWNEXTCHAR(dest);
			src = NWNEXTCHAR(src);
			while (*src != 0 && *src != '"')
			{
				CopyMemory(dest, src, NWNUMBEROFBYTES(src));
				dest = NWNEXTCHAR(dest);
				src = NWNEXTCHAR(src);
				if (*(NWPREVCHAR(line, src)) == '\\' && *src != 0)
				{
					CopyMemory(dest, src, NWNUMBEROFBYTES(src));
					dest = NWNEXTCHAR(dest);
					src = NWNEXTCHAR(src);
				}
			}
			/*
			 * does line end without ending quote
			 */
			if (*src == 0)
			{
				/*
				 * do not fail if REM or REMARK or ;
				 */
				char *p;
				p = line;
				/*
				 * skip space and tabs
				 */
				while (*p == ' ' || *p == '\t')
				{
					p++;
				}
				if (strnicmp(p,"REM",3) && *p != ';')
				{
					return(0);
				}
				/* continue if REMark statement */
			}
			CopyMemory(dest, src, NWNUMBEROFBYTES(src));
			break;

		case '#':
			if (firstChar)
			{
				if (*dest != ' ')
				{
					dest = NWNEXTCHAR(dest);
				}
				CopyMemory(dest, src, NWNUMBEROFBYTES(src));
				src = NWNEXTCHAR(src);
				while (*(NWNEXTCHAR(src)) != 0)
				{
					dest = NWNEXTCHAR(dest);
					CopyMemory(dest, src, NWNUMBEROFBYTES(src));
					src = NWNEXTCHAR(src);
				}
				dest = NWNEXTCHAR(dest);
				CopyMemory(dest, src, NWNUMBEROFBYTES(src));
				break;
			}

		default:
			firstChar = 0;
			/*
			 * begins with _ is ok
			 */
			if (NWISALNUM(*src) || *src == '_' || *src == '%')
			{
				dest = NWNEXTCHAR(dest);
				CopyMemory(dest, src, NWNUMBEROFBYTES(src));
				*dest = NWTOUPPER(*dest);
			}
			else if (*dest == ' ')
			{
				CopyMemory(dest, src, NWNUMBEROFBYTES(src));
			}
			else
			{
				dest = NWNEXTCHAR(dest);
				CopyMemory(dest, src, NWNUMBEROFBYTES(src));
			}
		}
	}
	*++dest = 0;
	lstrcpy(line, temp[0] == ' ' ? temp + 1 : temp);
	return(1);
}

void WinListOutput (BYTE *output, PSPACE *p)
{
//	SendDlgItemMessage(p->ScriptWin,SCRIPT_DIALOG_BOX,EM_REPLACESEL,
//		0,(LPARAM)((LPCSTR)output));

	Edit_ReplaceSel(GetDlgItem(p->ScriptWin, SCRIPT_DIALOG_BOX), ((LPCSTR)output));

	UpdateWindow(p->ScriptWin);
}

void ScriptError (WORD error, PSPACE *p, ...)
{
	va_list		ap;

	char			origmsg[2048];
	char			erroutput[2048];
	char			errmsg[2048];

	va_start(ap,p);

	if (DSALMGetMessage(error,origmsg,sizeof(origmsg)) == NULL)
	{
		wsprintf(origmsg,CANNOTGETMESSAGE,error);
	}
	vwsprintf(errmsg,origmsg,ap);

	if (*(p->scripterror) == NULL)
	{
		DSALMGetMessage(SCRIPT_GEN_ERROR,(char *)p->scripterror,sizeof(p->scripterror));
		if (*(p->scripterror) == NULL)
		{
			lstrcpy((char *)p->scripterror,DEFAULT_SCRIPT_ERROR);
		}
	}
	if (*(p->scriptquestion) == NULL)
	{
		DSALMGetMessage(SCRIPT_GEN_QUESTION,(char *)p->scriptquestion,
			sizeof(p->scriptquestion));
		if (*(p->scriptquestion) == NULL)
		{
			lstrcpy((char *)p->scriptquestion,DEFAULT_SCRIPT_QUESTION);
		}
	}
	if (*(p->scriptfile) != NULL)
	{
		wsprintf(erroutput,(char *)p->scripterror,errmsg,p->scriptfile,
			p->ScriptlineNumber,p->originalLine);
	}
	else
	{
		wsprintf(erroutput,(char *)p->scripterror,errmsg,(char *)p->scriptquestion,0,
		  (char *)p->scriptquestion);
	}

#if LOGIN IS_DISABLED && LOGOUT IS_DISABLED

	if (p->se_perrors != 0)
	{
		AddToList(p->se_perrors,(unsigned char *)erroutput);
	}

#endif

	if (p->WinOutput == 1)
	{
		WinListOutput((unsigned char *)erroutput,p);
	}

	va_end(ap);
}

void ScriptOutput (BYTE *output, PSPACE *p, ...)
{
	va_list		ap;
	char			message[2048];

	va_start(ap,p);
	if (output != NULL)
	{
		vwsprintf(message,(char *)output,ap);
	}
	else
	{
		lstrcpy(message,BADOUTPUTMSG);
	}

#if LOGIN IS_DISABLED && LOGOUT IS_DISABLED

	if (p->se_perrors != 0)
	{
		AddToList(p->se_perrors,(unsigned char *)message);
	}

#endif

	if (p->WinOutput == 1)
	{
		WinListOutput((unsigned char *)message,p);
	}
	va_end(ap);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ExecuteNonFlowCommand() )

 MODULE       : script.c

 GLOBALS USED : context - - - Context handle recieved from CreateContext
					  parsedServer, parseUser, parsedString, parsedName, 
 					  parsedCondition, errorLevel, executeDefaultScript
					  pcCompatibles, shiftArgs

 SYNTAX       : ExecuteNonFlowCommand(char *originalLine)

 PARAMETERS   :   -> input          <-output 
              -> originalLine- The original line read from the login script. 


 RETURN       : None


 DESCRIPTION  : 	After the parser has successfully parsed a command in the
 					login script, this function will switch on the command found
					and execute the appropriate function.  Each case should be
					self explanitory

 ALGORITHM    : switch(parsedCommand)
 						case:
						case:
						default:

 END_MANUAL_ENTRY
****************************************************************************/
static int ExecuteNonFlowCommand (char *originalLine, PSPACE *p,
	AMEMBLOCKID *Errors, WORD *exitflag)
{
	switch(p->parsedCmdType)
	{
		case CMD_NOTHING:
		case CMD_CLEARSCREEN:
		case CMD_LABEL:
			break;
		case CMD_ATTACH:
			p->errorLevel = AttachCommand(p->parsedServer, p->parsedUser, p->parsedString,p);
			break;
		case CMD_BREAK:
			BreakCommand(p->parsedCondition);
			break;
		case CMD_COMSPEC:
			p->errorLevel = ComSpecCommand(p->parsedName, p->parsedCondition, originalLine,p);
			break;
		case CMD_CONTEXT:
			ChangeContext(p->parsedString,p);
			break;
		case CMD_DISPLAY:
			p->errorLevel = DisplayFileCommand(p->parsedName, p->parsedCondition, originalLine, p);
			break;
		case CMD_DISPLAY_LTIME:
			p->errorLevel = ShowLastLoginTime(p);
			break;
		case CMD_DOSBREAK:
			DOSBreakCommand(p->parsedCondition);
			break;
		case CMD_INCLUDE:
			{
			char *newName;
			p->errorLevel = 1;
			NWDSContextHandle	context;						/* Current context				*/

		if ((context = NWDSCreateContext()) != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{

				if((newName = strdup(p->parsedName)) == NULL)
				{
					ScriptError(MEMORY_ERROR,p);
					ScriptError(SCRIPT_CANT_INCLUDE,p,p->parsedName);
					break;
				}

				p->errorLevel = DoScript((unsigned char *)p->parsedName, FILENAME_SCRIPT, exitflag,
					Errors, p->WinOutput,p);

				free(newName);
				NWDSFreeContext(context);
		}
				if(p->errorLevel == SCRIPT_PROCESSING_ERROR)
					return(0);

				break;
		}
		case CMD_SET:
			p->errorLevel = DOSSetCommand(p->parsedName, p->parsedString, p->parsedCondition,p);
			break;
		case CMD_DOSVERIFY:
			setverify(p->parsedCondition);
			break;
		case CMD_DRIVE:
			p->errorLevel = DriveCommand(p->parsedString[0],p);
			break;
		case CMD_EXIT:
			ExitCommand(p->parsedString,p);
			return(0);
		case CMD_EXTERNAL:
			p->errorLevel = ExternalCommand(p->parsedName, p->parsedString,p);
			break;
		case CMD_FIRE:
			FirePhasersCommand(p->parsedString[0] == 0? 1: p->parsedString[0]);
			break;
		case CMD_MACHINE:
			p->errorLevel = MachineNameCommand(p->parsedString,p);
			break;
		case CMD_MAP:
			p->errorLevel = MapCommand(p->parsedString,p);
			break;
		case CMD_NO_DEFAULT:
			p->executeDefaultScript = 0;
			break;
		case CMD_PAUSE:
			p->errorLevel = PauseCommand(p);
			break;
		case CMD_COMPATIBLE:
			p->pcCompatible = 1;
			break;
		case CMD_SCRIPT_SERVER:
			p->errorLevel = HomeServer(p->parsedServer, p->parsedUser,p);
			break;
		case CMD_SET_TIME:
			p->synchTimeFlag = p->parsedCondition;
			break;
		case CMD_SHIFT:
			p->shiftArgs += (char)p->parsedCondition;
			break;
		case CMD_WRITE:
			ScriptOutput((unsigned char *)p->parsedString,p);
			break;
		case CMD_UNKNOWN:
			ScriptError(SCRIPT_UNKNOWN_COMMAND,p);
			break;
		default:
			ScriptError(SCRIPT_BAD_FLOW_COMMAND,p,originalLine);
			break;
	}
	return(1);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DoGoto() )

 MODULE       : script.c

 GLOBALS USED : p->parsedName - - - This is the label to search for.


 SYNTAX       : DoGoto(arg1, arg2)

 PARAMETERS   :   -> input          <-output 
              -> fp- - - - - - - The current script file pointer stream.
					-> originalLine- - The original line buffer (Used to get next 
												line)
					-> expandedLine- - The expandedLine buffer (Used to expand 
												originalLine).

 RETURN       : The line number that the label specified was found on.


 DESCRIPTION  : Starts at the beginning of the file and seeks for the first
 						occurence of a specified label.

 ALGORITHM    :  Get the label.
 						Go to the beginning of the file.
						Search for the label recieved.
						return line number recieved.


 END_MANUAL_ENTRY
****************************************************************************/
static WORD DoGoto(FILE *fp, char *originalLine, char *expandedLine, PSPACE *p)
{
	char label[MAX_STRING];
	WORD lineNumber;

	lstrcpy(label, p->parsedName);
	fseek(fp, 0L, SEEK_SET);
	lineNumber = 0;
	while (fgets(originalLine, MAX_STRING - 1, fp))
	{
		lineNumber++;
		strcat(originalLine, "\n");
		ExpandCmdLineArgs(expandedLine, originalLine,p);
		if (CompressLine(expandedLine)
		  && ParseLine(EXPANDIDENTIFIERS, expandedLine,p)
		  && ParseLine(SCRIPTCOMMAND, p->parsedString,p)
		  && p->parsedCmdType == CMD_LABEL
		  && lstrcmp(label, p->parsedName) == 0)
		{
			return(lineNumber);
		}
	}
	ScriptError(SCRIPT_NO_LABEL,p, label);
	return(lineNumber);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( InitNetworkIdentifiers() )

 MODULE       : init.c

 GLOBALS USED : 


 SYNTAX       : InitNetworkIdentifiers(WORD connectionID)

 PARAMETERS   :   -> input          <-output 
              -> connectionID- - - ConnectionID of the server that all data
													for the login script should be
					-> objectName- - - - The userName (objectName) used to login.

 RETURN       : None


 DESCRIPTION  : Initializes identifiers for the login script.  This code 
 						will only work once the user has logged in.  It is specific 
						to the login script, but some of the global variables may be 
						used elsewhere. It Gets the following information:

							- Primary Connection ID 	(primaryConnectionID)
							- Server Connection Number (station)
							- File Server Name			(serverName)
							- Type of Object				(type)
							- Object ID number			(userObjectID)
							- InterNetwork Address 		(netAddress)
							- The user name				(userName)
							- Full name of the user 	(lastName)

			NOTE: If user has no full name an UNKNOWN_NAME string is used.

 ALGORITHM    : Make calls to get the various information (see description).
 						If we are logging into a 3.2 server get the home server, 
						and language.

 END_MANUAL_ENTRY
****************************************************************************/
void InitNetworkIdentifiers(NWCONN_HANDLE *connectionID, char *objectName, PSPACE *p)
{
WORD	type;
int	ccode;
char	value[128],
		*attrName1,
		*ptr,
		language[128],
		shellContext[MAX_DN_CHARS],
		buffer[MAX_DN_CHARS];		/* working variable to hold typed full name */

uint32		syntaxID;
CI_List_T	*attrName2,
				*attrPtr;
NWDSContextHandle	context;						/* Current context				*/

	/*
		fullName = full login or context name
		lastName = full name (bindery) or surname (last name)
		loginName = UserName up to 8 characters
	*/

	context = ERR_CONTEXT_CREATION;
	ccode = NWGetConnectionNumber(*connectionID, &p->station);
	if (ccode != 0)
	{
		ScriptError(SCRIPT_UNEXPECTED_INTERNAL_ERROR,p,SCRIPTSCPP+3,ccode);
	}
	NWGetFileServerName(*connectionID, p->serverName);

	if(p->NDSorBindery == BINDERY)
	{
		if ((ccode = NWGetConnectionInformation(*connectionID, p->station, p->loginName, &type, &p->userObjectID, NULL)) != 0
			|| (ccode = NWGetInternetAddress(*connectionID, p->station, p->netAddress))	!= 0)
		{
			ScriptError(SCRIPT_CANT_GET_CONNECTION_INFO,p, ccode);
		}
		p->userObjectID = NWLongSwap(p->userObjectID);

		if (NWReadPropertyValue(*connectionID, p->loginName, type,
				"IDENTIFICATION", 1, (BYTE *)value, NULL, NULL) == 0)
		{
			value[sizeof(value) - 1] = 0;
			lstrcpy(p->lastName, value);
			lstrcpy(p->fullName, value);	/* SPD 62182 KPW 3-29-94 */
		}
		else
		{
			DSALMGetMessage(E_UNKNOWN_NAME,p->lastName,sizeof(p->lastName));
			DSALMGetMessage(E_UNKNOWN_NAME,p->fullName,sizeof(p->fullName));
		}
	}

	if(p->NDSorBindery == NDS)
	{
		ccode = NWGetInternetAddress(*connectionID, p->station, p->netAddress);

		if ((context = NWDSCreateContext()) != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			/* USERID */
			if((ccode = GetUserName(context, 0, TRUE, buffer)) >= 0)
			{
				NWDSMapNameToID(context, *connectionID, buffer, &p->userObjectID);
				p->userObjectID = NWLongSwap(p->userObjectID);
			}

			FillMemory(p->loginName,0,9);											/* set to null */
			NWDSCanonicalizeName(context, buffer, buffer); 			/* Need RDN */
			NWDSRemoveAllTypes(context, buffer, buffer);				/* it may come through with types */
			DSRemoveNameFromFullName(buffer, NULL, buffer, NULL);
			NWSTRNCPY(p->loginName,buffer,8);								/* copy only 8 chars */

			/* change spaces to under score "_" KPW 04-15-94
				This is needed since LOGIN_NAME is used to map drives
			*/
			ptr = p->loginName;
			do
			{
				if (*ptr == ' ')
					*ptr = '_';
				ptr = NWNEXTCHAR(ptr);
			}while (*ptr);

			/* Set shell context to objects container if no shell context */
			NWGetDefaultNameContext(MAX_DN_CHARS, (unsigned char *)shellContext);
			if(*shellContext == (char)NULL)
			{
				shellContext[0]='.';		/* anchor to root (KPW 03-03-93) */
				NWDSCanonicalizeName(context, buffer, &shellContext[1]);
				DSRemoveNameFromFullName(&shellContext[1], &shellContext[1], NULL, NULL);		/* get container of user */
				ChangeContext(shellContext,p);
			}

			/* LAST_NAME */
			if(!ReadAttribute(objectName, "full name", (void **)&attrName1, &syntaxID))
			{
				lstrcpy(p->lastName, attrName1);			/* Global Full Name Identifier */
				if (attrName1)
					free(attrName1);
			}
			else
			{
				DSALMGetMessage(E_UNKNOWN_NAME,p->fullName,sizeof(p->fullName));
			}
	
			/* LAST_NAME */
			if(!ReadAttribute(objectName, "Surname", (void **)&attrName1, &syntaxID))
			{
				lstrcpy(p->lastName, attrName1);
				if (attrName1)
					free(attrName1);
			}
			else
			{
				DSALMGetMessage(E_UNKNOWN_NAME,p->lastName,sizeof(p->lastName));
			}

			/* make Message or Home server the primary connection */
			if(!ReadAttribute(objectName, "Message Server", (void **)&attrName1, &syntaxID))
			{
				/*                       DN       , cont, w/o type    w type */
				NWDSCanonicalizeName(context, attrName1, buffer);
				DSRemoveNameFromFullName(buffer, NULL,shellContext,NULL);
				if (shellContext[0] != '\0')	/* there is a home server */
				{
					/* if the server is different than the one that is our primary,
						change the primary server */
					if (strcmpi((char *)p->primaryLoginConnection.serverName,shellContext) != 0)
					{
						int	drive;
						/*         server        user */
						HomeServer(shellContext, objectName,p);
	
						if (*connectionID != p->primaryLoginConnection.connectionID)
						{
							/* 12-15-93 Free the connection slot and force a new
								monitored connection, hopefully, to the home server */
		 					NWFreeConnectionSlot(*connectionID,SYSTEM_DISCONNECT);
	
							/* change the working connection ID to the new one. */
							*connectionID = p->primaryLoginConnection.connectionID;

							/* We need to remap the current drive to sys:login on the
								home server so that maps to vol: will get to the proper
								server */
							drive = _getdrive()+'A'-1;				/* get the current drive */
							strcat(shellContext,"/sys:login");	/* get server/sys:login */
							value[0] = drive;
							value[1] = 0;
							DoMapdrive((unsigned char *)value, (unsigned char *)shellContext, TRUE);
						}
					}
				}
				if (attrName1)
					free(attrName1);
			}
		}

		/* set NWLANGUAGE environment variable */
		if(!ReadAttribute(objectName, "Language", (void **)&attrName2, &syntaxID))
		{
			for(attrPtr = attrName2; attrPtr != NULL; attrPtr=attrPtr->next)
			{
				if(attrPtr == attrName2)
					lstrcpy(language, attrPtr->s);
				else
				{
					if(lstrlen(language) + lstrlen(attrPtr->s) +1 < 127)
					{
						strcat(language, ",");
						strcat(language, attrPtr->s);
					}
				}
			}
			DOSSetCommand("NWLANGUAGE", language, 1,p);
			if (attrName2)
				free(attrName2);
		}
	}										 

Terminate:
	if (context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		NWDSFreeContext(context);
	}
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( GetNovellDOSVer() )

 MODULE       : init.c

 GLOBALS USED : 


 SYNTAX       : GetNovellDOSVer()

 PARAMETERS   :   -> input          <-output 


 RETURN       : None


 DESCRIPTION  : Use int 21 to look for versions of DRDOS and Novell DOS

 ALGORITHM    : 

 END_MANUAL_ENTRY
****************************************************************************/
void GetNovellDOSVer(PSPACE *p)
{
	union REGS inregs, outregs;

	/* now look for Novell DOS  and change the version */
	inregs.x.ax = 0x4452;		/* get version number */
	intdos(&inregs,&outregs);
	if (outregs.x.cflag == 0)
	{
		/* save default DR-DOS values then change for Novell DOS */
		lstrcpy(p->os,"DRDOS");
		wsprintf(p->osVersion,"V3.31");

		/* look at dx for minor version when released */
		if (outregs.x.ax > 0x1070 && outregs.x.dx > 0x00)	/* 0x1071 0x00 is DRDOS 6-3-93 update */
		{
			lstrcpy(p->os,"NWDOS");
			if (outregs.x.ax == 1071)
				wsprintf(p->osVersion,"V7.00b");						/* 7.00 beta release */
			else if (outregs.x.ax >= 1072)
				wsprintf(p->osVersion,"V7.%02d",outregs.h.dl);	/* V7.xx release, dx is minor version */
		}
	}

} /* end of GetNovellDOSVer */


/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( InitWorkstationData() )

 MODULE       : init.c

 GLOBALS USED : 


 SYNTAX       : InitWorkstationData(arg1, arg2)

 PARAMETERS   :   -> input          <-output 
              -> arg1 
              <- arg2 


 RETURN       : None


 DESCRIPTION  :	Initializes workstation data.
						Information received include:
						 OS, OS_VERSION, MACHINE_NAME, SHELL_VERSION or REQUESTER_VERSION

			NOTE: this data must be global so that the M-Code parser can access it.

 ALGORITHM    : Get the information from shell or DOS


 END_MANUAL_ENTRY
****************************************************************************/

static void InitWorkstationData(PSPACE *p)
{

	REGISTERS regs;
	char data[40], *tmp;

	regs.w.ax = 0xEA01;	/* Shell workstation environment string */
	regs.w.bx = 0;
	regs.w.di = FP_OFF((void  *)data);
	regs.w.es = FP_SEG((void  *)data);
	NWShellRequest(&regs, USE_ES); //USE_DS | USE_ES);
	lstrcpy(p->os, data);					/* OS - Must be global for M-Code parser */
	tmp = data + lstrlen(data) + 1;
	if (*tmp == 0)
		ScriptError(SCRIPT_NO_SHELL,p);
	else
		lstrcpy(p->osVersion, tmp);		/* OsVersion - Must be global for M-Code parser */

	GetNovellDOSVer(p);	/* use the INT 21 4452 to look for novell dos */
	tmp += lstrlen(tmp) + 1;
	lstrcpy(p->machine, tmp);  			/* Machine   - Must be global for M-Code parser */
	tmp += lstrlen(tmp) + 1;
	lstrcpy(p->sMachine, tmp);			/* SMachine  - Must be global for M-Code parser */
	wsprintf(p->shellVersion,"V%d.%02d%c",regs.w.bx >> 8,regs.w.bx & 0x00ff,
		(regs.w.cx & 0x00ff) + 'A');	/* Shell Version - Must be global for M-Code parser */
	
	/* Get the DOS Requester version here */
	regs.w.bx = GEN_VER;
	NWVLMRequest(0, VLM_ID_GENR, VLM_GEN, &regs, 0); //USE_DS | USE_ES);
	wsprintf(p->requesterVersion,"V%d.%02d",regs.w.bx, regs.w.cx);	/* Shell Version - Must be global for M-Code parser */
}

void InitParseData (PSPACE *p)
{
	int i;

	int monthNames[] =
	{
		MON_JANUARY, MON_FEBRUARY, MON_MARCH, MON_APRIL, MON_MAY,
		MON_JUNE, MON_JULY, MON_AUGUST, MON_SEPTEMBER, MON_OCTOBER,
		MON_NOVEMBER, MON_DECEMBER,
	};
	int dayNames[] =
	{
		DAY_SUN, DAY_MON, DAY_TUES, DAY_WED, DAY_THURS,
		DAY_FRI, DAY_SAT
	};
	
	p->emptystring = "";
	for (i = 0; i < 255; i++)
	{
		p->tokenString[i] = p->emptystring;
	}
	for (i = 0; i < sizeof(monthNames)/sizeof(monthNames[0]); i++)
	{
		p->monthNames[i] = monthNames[i];
	}
	for (i = 0; i < sizeof(dayNames)/sizeof(dayNames[0]); i++)
	{
		p->dayNames[i] = dayNames[i];
	}
	FillMemory(p->scriptfile,0,sizeof(p->scriptfile));
	FillMemory(p->scripterror,0,sizeof(p->scripterror));
	FillMemory(p->scriptquestion,0,sizeof(p->scriptquestion));
	p->ScriptlineNumber = 0;
	p->se_perrors = 0;
}

WORD ProcessScript (FILE *fp, BYTE *FileName, AMEMBLOCKID *Errors,
	WORD *exitflag, PSPACE *p)
{
	WORD			errcode;
	struct locals
	{
		signed char ifConditions[MAX_NESTED_IF_DEPTH];

		int 			ifLevel;

		int 			linenum;

		WORD 			len;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));
	errcode = SCRIPT_PROCESSING_OKAY;

	*exitflag = 0;
	if (p->nesting <= 1)
	{
		InitNetworkIdentifiers(&p->primaryLoginConnection.connectionID,
			(char *)p->primaryLoginConnection.userName,p);
		InitWorkstationData(p);
		InitParseData(p);
	}

	p->synchTimeFlag = 0;
	p->ScriptlineNumber = l->linenum = 0;
	p->se_perrors = Errors;
	lstrcpy((char *)p->scriptfile,(char *)FileName);

	/*
	 * Define space for MX parser
	 */
	if (p->nesting <= 1)
	{
		DefineTokenTable(&p->mxg,INITIALTOKENCOUNT,MAXTOKENCOUNT);
	}


	l->ifConditions[l->ifLevel = 0] = 1;
	for (;;)
	{
		p->expandedLine[0] = 0;
		/*
		 * Do we need to run the default login script?
		 */
		if (fp == NULL)
		{
			l->linenum++;
			p->ScriptlineNumber = l->linenum;
			/*
			 * We figure the end of the DefaultScript by figuring the sizeof
			 * the string array.
			 */
			if (p->ScriptlineNumber > DEFAULTSCRIPTSIZE)
			{
				break;
			}
			lstrcpy(p->originalLine, DefaultScript[p->ScriptlineNumber - 1]);
		}
		else
		{
			if (fgets(p->originalLine, MAX_STRING - 1, fp) == NULL)		
			{
				break;
			}
			l->linenum++;
			p->ScriptlineNumber = l->linenum;
			l->len = lstrlen(p->originalLine);

			if (l->len >= MAX_STRING - 2)
			{
				/* scroll error */
				ScriptError(SCRIPT_LINE_TOO_LONG,p);
				continue;
			}
			/*
			 * Remove any trailing \r\n from the string.
			 */
			while (l->len > 0 &&
				(p->originalLine[l->len - 1] == (BYTE)0x0d ||
				 p->originalLine[l->len - 1] == (BYTE)0x0a) )
			{
				p->originalLine[l->len - 1] = 0;
				l->len--;
			}
		}

		/*
		 * If the line is blank then skip it;
		 */
		if (p->originalLine[0] == 0)
		{
			continue;
		}

		/*
		 * Expand the original (%1 %2) to usable scriptCommand
		 */ 
		ExpandCmdLineArgs(p->expandedLine, p->originalLine,p);
		
		/*
		 * Removes excess white space, and uppercases command
		 */
		if (!CompressLine(p->expandedLine))
		{
			ScriptError(SCRIPT_NO_END_QUOTE,p);
			continue;
		}

		/*
		 * This obtains identifiers. i.e. IF %HOUR24, will get the time
		 * in HOUR24 (military time)
		 */
		if (!ParseLine(EXPANDIDENTIFIERS, p->expandedLine, p))
		{
			ScriptError(SCRIPT_NO_PARSE,p);
			continue;
		}

		/*
		 * Removes excess white space, and uppercases command
		 */
		if (!CompressLine(p->expandedLine))
		{
			ScriptError(SCRIPT_NO_END_QUOTE,p);
			continue;
		}
	
		/*
		 * Parse the script Command
		 */
		if (!ParseLine(SCRIPTCOMMAND, p->parsedString, p))
		{
			ScriptError(SCRIPT_NO_PARSE,p);
			continue;
		}

		switch (p->parsedCmdType)
		{
			/*
			 * If the ifConditions ifLevel is 1, Set it to 0
			 * (Don't exececute the else) or Vice-Versa
			 */ 
			case CMD_ELSE:
				/*
				 * Look at CMD_IFDO for an understanding of -1
				 */
				if (l->ifConditions[l->ifLevel] != -1)
				{
					l->ifConditions[l->ifLevel]=
						(char)(l->ifConditions[l->ifLevel] == (char)1 ? 0 : 1);
				}
				break;

			/*
			 * End of If statement go back a level
			 */
			case CMD_END:
				if (l->ifLevel > 0)			
				{
					l->ifLevel--;
				}
				break;

			/*
			 * Goto the label specified
			 */
			case CMD_GOTO:
				if ((p->ScriptlineNumber = DoGoto(fp, p->originalLine, p->expandedLine,p)) == 0)
				{
					ScriptError(SCRIPT_NO_GOTO,p);
					errcode = SCRIPT_PROCESSING_ERROR;
					goto Terminate;
				}
				break;

			case CMD_IF:
				/*
				 * If if level Doesn't equal 1, or bad parse command -
				 * break out of switch()
				 */
				if (l->ifConditions[l->ifLevel] != 1 || !p->parsedCondition)
				{
					break;
				}
				/*
				 * Else, Parse the command
				 */
				if (!ParseLine(SCRIPTCOMMAND, p->parsedString,p))
				{
					ScriptError(SCRIPT_NO_PARSE,p);
					continue;
				}
				/*
				 * Goto a label
				 */
				if (p->parsedCmdType == CMD_GOTO)
				{
					/*
					 * If error going to label, leave with error
					 */
					if ((p->ScriptlineNumber = DoGoto(fp, p->originalLine, p->expandedLine,p)) == 0)
					{
						ScriptError(SCRIPT_NO_GOTO,p);
						errcode = SCRIPT_PROCESSING_ERROR;
						goto Terminate;
					}
				}
				/*
				 * Flow cmds are IF, ELSE, GOTO, etc. They deal with
				 * flow of the login script
				 */
				else if ((errcode = ExecuteNonFlowCommand(p->originalLine,p,Errors,exitflag)) != 0)
				{
					ScriptError(errcode,p);
					errcode = SCRIPT_PROCESSING_ERROR;
					goto Terminate;
				}
				break;

			case CMD_IFDO:
				/*
				 * If user tries to go to many levels of IF's
				 */
				if (l->ifLevel >= sizeof(l->ifConditions) - 1)
				{
					ScriptError(SCRIPT_IF_NESTED,p);
					errcode = SCRIPT_PROCESSING_ERROR;
					goto Terminate;
				}
				/*
				 * If previous if is 0, then do not execute this if (-1)
				 * or any else's that follow
				 *
				 * Otherwise, set it to 0 if p->parsedCondition =0, a 1
				 * if p->parsedCondition is anything else
				 */
				l->ifConditions[l->ifLevel+1] = (char) 
					(l->ifConditions[l->ifLevel] != 1 ? -1: (p->parsedCondition ? 1: 0));

				/*
				 * Go to next l->ifLevel
				 */
				l->ifLevel++;
				break;

			default:
				/*
				 * If we should execute and there is an error executing
				 * command, Error and exit
				 */
				if (l->ifConditions[l->ifLevel] == 1 &&
					((errcode = ExecuteNonFlowCommand(p->originalLine,p,Errors,exitflag)) == 0))
				{
					ScriptError(errcode,p);
					errcode = SCRIPT_PROCESSING_ERROR;
					goto Terminate;
				}
		}
		FillMemory(p->expandedLine, 0, MAX_STRING);
		FillMemory(p->originalLine, 0, MAX_STRING);  
	}

Terminate:
	if (l != 0)
	{
		if (p->scripterror != NULL)
		{
			*(p->scripterror) = 0;
		}
		if (p->doExit != 0)
		{
			*exitflag = 1;
		}
		FillMemory(p->scriptfile,0,sizeof(p->scriptfile));
		delete l;
	}
	if (p->synchTimeFlag == 1)
	{
		SynchTime(p);
	}
	return(errcode);
}

