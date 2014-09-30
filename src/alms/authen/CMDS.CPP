/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */
#include "features.h"
#include <windows.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <setjmp.h>
#include <errno.h>		/* E2BIG, ENOENT, EXDEV, ENOEXEC, ENOMEM */
#include <fcntl.h>		/* O_BINARY, O_TEXT, O_RDONLY, O_RDWR */
#include <direct.h>		/* getcwd */
#include <io.h>			/* open, setmode  */
#include <malloc.h>		/* free */
#include <process.h>		/* spawnlp, P_WAIT, errno */
#include <time.h>			/* struct tm */
#include <mmsystem.h>

/* Novell header files */
#include <nwlocale.h>	/* NWLStrftime */
#include <nwserver.h>	/* _NWGetFileServerType */
#include <nwbindry.h>	/* NWReadProperty, NWWriteProperty */
#include <nwerror.h>		/* NO_SUCH_PROPERTY, NO_SUCH_SEGMENT */
#include <dbcsmac.h>		/* NWLSTRUPR */
#include <constant.h>	/* SERVER_NAME_SIZE, USER_NAME_SIZE */
#include <nwundoc.h>		/* REGISTERS, USE_DS, USE_ES, LMemCpy */
#include <nwdpath.h>		/* NWParsePath */
#include <nwconnec.h>	/* NWGetConnectionID */
#include <nwdsdefs.h>	/* DS_CONTAINER_ENTRY */
#include <nwndscon.h>	/* NWSetDefaultNameContext */
#include <nwdsmisc.h>	/* NWIsDSServer */
#include <nwdserr.h>
#include <nwdsname.h>	/* NWDSCanonicalizeName */
#include <nwdsdsa.h>		/* NWReadObjectInfo */
#include <nwvol.h>
#include "parse.h"
#include "cmds.h"
#include <a_alm.h>
#include "alm_auth.h"
#include "errors.h"
#include "resource.h"
#include "utils.h"
#include "msupport.h"
#include "parse.h"
#include "inerrors.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <dstools.h>

#ifdef __cplusplus
}
#endif


//#define MK_FP(seg, ofs)	((void  *) (((unsigned long)(seg) << 16) | (unsigned)(ofs)))
//#undef FP_OFF
//#define FP_OFF(fp)	((unsigned)((unsigned long)(fp)))
//#undef FP_SEG
//#define FP_SEG(fp)	((unsigned)((unsigned long)(fp) >> 16))

#define USE_MAX_MEM -1

#define STDOUT_HANDLE		1

#ifdef DOS
	#define	KEYBOARD_BUFFER_START	0x0080
	#define	KEYBOARD_BUFFER_END		0x0082
	#define 	BIOSDATASEG	0x40

	/* offset in the keyboard buffer to the next character to be read */
	#define 	BUFFERHEAD	0x1A

	/* offset in the keyboard buffer of the last key to be read */
	#define 	BUFFERTAIL	0x1C
#endif

extern WORD DoAuthenDialog(BYTE *server, BYTE *user, BYTE *pwd, WORD type,
	WORD display);
extern WORD DoDeleteDrive(BYTE *DriveName);
extern void ScriptError(WORD error,PSPACE *p,...);
extern void ScriptOutput(BYTE *output,PSPACE *p,...);
extern WORD DoMapdrive(BYTE *DriveName, BYTE *Path, int MapRoot);
extern WORD GetVolumeObjectName(NWDSContextHandle context,
	NWCONN_HANDLE connectionID, BYTE *base);
//extern WORD CALLBACK PauseDialogBox(HWND ScriptWin);
extern WORD PauseDialogBox(HWND ScriptWin);


WORD CallNWParsePath(char *path, char *server, NWCONN_HANDLE *connectionID,
							char *volume, char *dir, int physical, int *isObject);

	/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CallNWParsePath() )

 MODULE       : mapcmd.c

 GLOBALS USED : 


 SYNTAX       : CallNWParsePath()

 PARAMETERS   :   -> input          <-output 
					char *path				Path to parse
					char *server			return server name
					WORD *connectionID	return connection id to server
					char *volume			return volume name
					char *dir				return directory name
					int physical			path passed is server/volume and not
												NDS object name
					int *isObject			path was indeed an NDS object

 RETURN       :Returns any completion code of NWcall.
					Returns Sever name.
					Returns ConnectionID.
					Returns volume.
					Returns the directory name.

 DESCRIPTION  :calls either NWDSParsePath or NWParsePath

 ALGORITHM    :1) if not physical and authenticated to a tree Calls DS parse
						path. Otherwise, calls the bindery based ParsePath.  


 END_MANUAL_ENTRY
****************************************************************************/

/* same for map and login */
	
WORD CallNWParsePath(char *path, char *server, NWCONN_HANDLE *connectionID,
					char *volume, char *dir, int physical, int *isObject, PSPACE *p)
{
	NWDSContextHandle	context;						/* Current context				*/
	WORD errcode = -1;

	if ((context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		ScriptError(CANNOT_CREATE_CONTEXT,p);
		errcode = -1;
		goto Error1;
	}
	
	*isObject = FALSE;	/* default to no object. NWParsePath does not set */

	/* If user did not specify a physical volume and authenticated to DS */
	if(!physical && NWIsDSAuthenticated())
		errcode = NWDSParsePath(context, path, server, (unsigned short *)connectionID, volume, dir, (unsigned short *)isObject);
	else							/* If user wants to make to physical volume */
		errcode = NWParsePath(path, server, connectionID, volume, dir);
	
	NWDSFreeContext(context);

Error1:
	return(errcode);

} /* end of CallNWParsePath */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( BreakHandler() )

 MODULE       : init.c

 GLOBALS USED : 


 SYNTAX       : BreakHandler(void)

 PARAMETERS   :   -> input          <-output 

 RETURN       : None


 DESCRIPTION  : The CTL-C and CTL-BREAK handlers will call this if SetBreak
 						is turned on. (see BreakCommand)

 ALGORITHM    : Exit(-1);



 END_MANUAL_ENTRY
****************************************************************************/
#pragma argsused
static void BreakHandler (int flag)
{
	exit(-1);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( BreakCommand() )

 MODULE       : init.c

 GLOBALS USED : 


 SYNTAX       : BreakCommand(int onOff)

 PARAMETERS   :   -> input          <-output 
 					-> onOff - - - Flag indicating wether to turn on or off the
										CTL-C and CTL-BREAK keys.

 RETURN       : None


 DESCRIPTION  : Turns on or off the CTL-C and CTL-BREAK handling routines.

 ALGORITHM    : If user wants to set break on
 						Set address to our CTL-BREAK\CTL-C break handler
					  Else
					  	Turn of all CTL-BREAK\CTL-C checking.

 END_MANUAL_ENTRY
****************************************************************************/
void BreakCommand(int onOff)
{
	signal(SIGINT, onOff ? BreakHandler : (void (*)(int))SIG_IGN);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AttachCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : AttachCommand(char *server, char *user, char *pwd)

 PARAMETERS   :   -> input          <-output 
              -> server- - -  The name of the server to attach to.
					-> user- - - -  The name of the user to attach as.
					-> pwd - - - -  The password of the user, can be a pointer to
											NULL if the user needs to type one in.

 RETURN       : None


 DESCRIPTION  : If the server name is not specified we prompt for the server
 						and username.
					  Then attach to the server specified.
					  Then we reset the primary connection so that we will continue
						  to read the correct login script.

 ALGORITHM    : If server not passed
 						Get server and userName
					  Try to attach
					  reset primary connection.


 END_MANUAL_ENTRY
****************************************************************************/
int	AttachCommand(char *server, char *user, char *pwd, PSPACE *p)
{
	BYTE	password[PASSWORD_BUFFER_SIZE];
	BYTE	mess[100];
	char	buf[MAX_DN_BYTES];
	WORD 	ccode;
	unsigned int pwdi;

	pwdi = (*pwd & 0xff);
	/* user was not entered. User user from primary */
	if (*user == 0)
	{
		if (p->NDSorBindery == NDS)
		{
			lstrcpy(user,(char *)p->primaryLoginConnection.userName);
		}
		DSRemoveNameFromFullName((char *)p->primaryLoginConnection.userName,NULL,user,NULL);	/* change full DN to just user name */
	}
	
	if (pwdi != 0xff)				/* was a password entered */
	{
		lstrcpy((char *)password, pwd);	/* Set the global password */
 		FillMemory(pwd, 0,2);			/* clear the parsed password */
	}
	else if (p->primaryLoginConnection.serverLoginType == NDS)
	{
		/* NDS Connection */
		DSRemoveNameFromFullName((char *)p->primaryLoginConnection.userName,NULL,buf,NULL);
		if (strcmpi(buf,user) == 0)	/* same user name */
		{
			lstrcpy((char *)password,(char *)p->primaryLoginConnection.password);
		}
		else
		{
			password[0] = (char)0xff;		/* do not use password if primary login is to DS */
		}
	}
	else
	{
		/* bindery connection */
		if (strcmpi((char *)p->primaryLoginConnection.userName,user) == 0)	/* same user name */
		{
			lstrcpy((char *)password,(char *)p->primaryLoginConnection.password);
		}
		else
		{
			password[0] = (char)0xff;		/* do not use password if primary login is to DS */
		}
	}

	ccode = DoAuthenDialog((unsigned char *)server,(unsigned char *)user,password,DEFAULT_LOGIN,0);
	FillMemory(password,0,sizeof(password));
	if (ccode != PAUTHEN_DS_OK && ccode != PAUTHEN_BINDERY_OK)
	{
		*password = 0xFF;
		ccode = DoAuthenDialog((unsigned char *)server,(unsigned char *)user,password,DEFAULT_LOGIN,1);
		FillMemory(password,0,sizeof(password));
	}
	if (ccode != PAUTHEN_DS_OK && ccode != PAUTHEN_BINDERY_OK)
	{
		ScriptError(ccode,p,server);
	}
	else
	{
		ScriptOutput((unsigned char *)DSALMGetMessage(ccode,(char *)mess,sizeof(mess)),p,(char *)server);
		ccode = 0;
	}

	return((int)ccode);
}


/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( HomeServer() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : HomeServer(char *server, char *user)

 PARAMETERS   :   -> input          <-output 
              -> serverName - - - Name of the server to make as the primary.
					-> userName - - - - Name of the user to attach as, or NULL if
												unknown userName.


 RETURN       : None


 DESCRIPTION  : Connects to the script server specified in the login script.
 						and makes it the primary server.  All future implicit 
						references to the default server will go to this server.


 ALGORITHM    : 	GetConnected
 						Attach and login
						Set as primary server



 END_MANUAL_ENTRY
****************************************************************************/
int HomeServer(char *server, char *user, PSPACE *p)
{
int 	ccode;
NWCONN_HANDLE 	connectionID;
WORD			isDSServer;
char	treeName[33] = {0};
BYTE	pwd[PASSWORD_BUFFER_SIZE];		/* Entered password   	*/
BYTE mess[100];

	/* if the same server as primary, just return */
	if (strcmpi((char *)p->primaryLoginConnection.serverName,server) == 0)
	{
		return(0);
	}

	pwd[0] = 0xFF;
	ccode = DoAuthenDialog((unsigned char *)server,(unsigned char *)user,pwd,DEFAULT_LOGIN,1);

	if (ccode == PAUTHEN_DS_OK || ccode == PAUTHEN_BINDERY_OK)
	{
		NWGetConnectionIDFromName(lstrlen(server),(unsigned char *)server,&connectionID);

		isDSServer = NWIsDSServer(connectionID,treeName);
		if (p->NDSorBindery == BINDERY)
		{
			FillMemory(p->primaryLoginConnection.password,0,
				lstrlen((char *)p->primaryLoginConnection.password));

			lstrcpy((char *)p->primaryLoginConnection.userName,user);
			lstrcpy((char *)p->primaryLoginConnection.password,(char *)pwd);
		}

		lstrcpy((char *)p->primaryLoginConnection.serverName,server);

		p->primaryLoginConnection.connectionID = connectionID;
		p->primaryLoginConnection.serverVersion = isDSServer ? NDS:BINDERY;
		p->primaryLoginConnection.serverLoginType =
			p->primaryLoginConnection.serverVersion;

		NWSetPrimaryConnectionID(connectionID);	/* reset the primary connection id */

		ScriptOutput((unsigned char *)DSALMGetMessage(SCRIPT_ATTACHED,(char *)mess,sizeof(mess)),p,server);
		ccode = 0;
	}
	FillMemory(pwd,0,sizeof(pwd));
	return(ccode);
} 

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ChangeContext() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : ChangeContext(char *contextString)

 PARAMETERS   :   -> input          <-output 
              -> contextString - - - - The context string in the login script


 RETURN       : None


 DESCRIPTION  : 



 ALGORITHM    : 



 END_MANUAL_ENTRY
****************************************************************************/
int ChangeContext(char *contextString, PSPACE *p)
{
	NWDSContextHandle	context;						/* Current context				*/
NWCCODE 	ccode;
char	buffer[MAX_DN_BYTES] = {0};

Object_Info_T	objectInfo;

	if ((context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		ScriptError(CANNOT_CREATE_CONTEXT,p);
		goto Error1;
	}
			/*	Canonicalize the name to get object types in the context. */
	if ((ccode = NWDSCanonicalizeName(context, contextString, buffer)) != 0)
	{
		ScriptError(SCRIPT_CANON_CONTEXT,p, contextString);
		goto Error0;
	}


	/*	Set the context handle to the one selected to see if it fails
	 *	Do this to syntax check the new context. 
	 */
	lstrcpy(contextString, buffer);
	if ((ccode = NWDSSetContext(context,DCK_NAME_CONTEXT, contextString)) != 0)
	{
		ScriptError(SCRIPT_CONTEXT_INVALID,p,contextString);
		goto Error0;
	}

	/*	Set the current context to the root and try to read the object
	 *	we are trying to set to.
	 */
	if((ccode = NWDSSetContext(context, DCK_NAME_CONTEXT, DS_ROOT_NAME))!=0)
 	{
		ScriptError(SCRIPT_READ_OBJECTINFO,p,contextString);
		goto Error0;
 	}

	/***	NOTE:
			We use NWDSReadObjectInfo since it is free and will read any object
			in the tree. NWDSRead will only read if you have rights and seems
			to be too restrictive.
			Due to the case where you have rights in one context and not in the
			one above, this call is used so that cx .. can be used to navigate
			the tree. 
	*/

		/*	If the object does not exist or it is not a container, error ***/
 	ccode = NWDSReadObjectInfo(context, contextString, buffer, &objectInfo);
 	if (ccode || !(objectInfo.objectFlags & DS_CONTAINER_ENTRY) )
 	{
		ScriptError(SCRIPT_READ_OBJECTINFO,p,contextString);
		goto Error0;
 	}

	if(((ccode = NWDSSetContext(context, DCK_NAME_CONTEXT, contextString)) != 0) ||
		((ccode = NWSetDefaultNameContext(lstrlen((char *)contextString)+1, 
				(unsigned char *)contextString)) != 0))
	{
		ScriptError(SCRIPT_CANNOT_SET_CONTEXT,p,contextString);
	}
	else
	{
		ScriptError(SCRIPT_CONTEXT_CHANGED,p,contextString);
	}

Error0:
	NWDSFreeContext(context);

Error1:
	return(ccode);
} 


/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ComSpecCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : ComSpecCommand(arg1, arg2)

 PARAMETERS   :   -> input          <-output 
              -> spec- - - - - The path to set comspec to.
					-> drive - - - - The drive that command.com is on.
					-> originalLine- The original script line (for error reporting).


 RETURN       : None


 DESCRIPTION  : Takes the comspec command and converts it to a SET command.



 ALGORITHM    : If no drive specified find one.
 					   Then see if it exists in the search path
						If it doesn't exist 
							print an error
						If it does 
							call the SETCommand


 END_MANUAL_ENTRY
****************************************************************************/
#pragma argsused
int ComSpecCommand(char *spec, WORD drive, char *originalLine, PSPACE *p)
{
	ScriptError(SCRIPT_NOT_SUPPORTED_COMSPEC,p);
	return(0);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DisplayFileCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : DisplayFileCommand(char *name, int binaryMode)

 PARAMETERS   :   -> input          <-output 
 					-> name- - - - - - The path name of a file to display on the screen
					-> binaryMode- - - The mode to open the file in (TEXT or BINARY)


 RETURN       : None


 DESCRIPTION  : Displays a specified file on the screen.



 ALGORITHM    : Parse the path to see if it is valid.
 						If it is attempt to open file
						read the file and display contents on the screen.

 END_MANUAL_ENTRY
****************************************************************************/
int DisplayFileCommand(char *name, int binaryMode, char *originalLine, PSPACE *p)
{
char	server[48],
		volume[48],
		dir[256],
		buffer[512];
int 	fileHandle,
		bytesRead,
		mode,
		isObject = FALSE;
WORD 	ccode;
NWCONN_HANDLE connectionID;

	ccode = CallNWParsePath(name, server, &connectionID, volume, 
					buffer, 0, &isObject, p);
	if (ccode != 0)
	{
		ScriptError(SCRIPT_PARSE_PATH,p, originalLine);
		return(ccode);
	}
	if (connectionID == 0)
	{
		if(buffer[0] == '\\')
			wsprintf(dir, "%s:%s", volume, buffer);
		else
			wsprintf(dir, "%s:\\%s", volume, buffer);
	}
	else
		wsprintf(dir, "\\\\%s\\%s\\%s", server, volume, buffer);
	mode = binaryMode? O_BINARY: O_TEXT;
	if ((fileHandle = open(dir, O_RDONLY|mode)) == -1)
	{
		return(-1);
	}

	bytesRead = read(fileHandle, buffer, sizeof(buffer) - 1);
  while (bytesRead > 0)
	{
		buffer[bytesRead] = 0;
		ScriptOutput((unsigned char *)buffer,p);
		bytesRead = read(fileHandle, buffer, sizeof(buffer) - 1);
	} 

	close(fileHandle);
	ScriptOutput((unsigned char *)"\r\n",p);
	return(0);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ShowLastLoginTime() )

 MODULE       : cmds.c

 GLOBALS USED : The STRUCTURE primaryLoginConnection 


 SYNTAX       : ShowLastLoginTime(arg1, arg2)

 PARAMETERS   :   -> input          <-output 

 RETURN       : Always returns 0


 DESCRIPTION  : Reads the bindery property MISC_LOGIN_INFO to get the last
 						login time and date.  Then, prints the time and date to
						the screen.  If the MISC_LOGIN_INFO property does not exist
						for that user this function will simply return

 ALGORITHM    : Read property of primary connection and primary user name.
 					  If successful - Print date and time to the screen
					  else - return.

 END_MANUAL_ENTRY
****************************************************************************/
int ShowLastLoginTime(PSPACE *p)
{
WORD		ccode;
BYTE		buffer[128];
BYTE 		*date;
uint32	syntaxID;
struct tm timeptr,*timeptrDS;
BYTE mess[100];
char	user[MAX_DN_BYTES];

	if(p->NDSorBindery == NDS)
	{
		DSRemoveNameFromFullName((char *)p->primaryLoginConnection.userName,NULL,
			user,NULL);
		ccode = ReadAttribute(user,"Last Login Time",(void **)&date,&syntaxID);
	}
	else
	{
		date = buffer;
		ccode = NWReadPropertyValue(p->primaryLoginConnection.connectionID, 
						(char *)p->primaryLoginConnection.userName, OT_USER, 
						"MISC_LOGIN_INFO", (BYTE)1, date, NULL , NULL);
	}
	switch(ccode) 
	{
		case 0: 
			if(p->NDSorBindery == NDS)
			{
				timeptrDS = localtime((time_t *)date);
				NWLSTRFTIME((char *)buffer,128,"%c",timeptrDS);
			}
			else
			{
				timeptr.tm_min	 = date[4];
				timeptr.tm_hour = date[3];
				timeptr.tm_mday = date[2];
				timeptr.tm_mon	 = date[1] - 1;
				timeptr.tm_year = date[0];
				NWLSTRFTIME((char *)buffer,128,"%c",&timeptr);
			}

			ScriptOutput((unsigned char *)DSALMGetMessage(DISPLAY_LAST_LOGIN_TIME,(char *)mess,sizeof(mess)),p,buffer);

		case NO_SUCH_PROPERTY: 
			break;
		case NO_SUCH_SEGMENT:
		default:
			ScriptError(SCRIPT_UNEXPECTED_INTERNAL_ERROR,p,CMDSCPP+1,ccode);
			break;
	}
	if(p->NDSorBindery == NDS)
		if (date)
			free(date);
	return(0);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DosBreakCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : DOSBreakCommand(int on)

 PARAMETERS   :   -> input          <-output 
              -> on- - - Wether to turn on or off the DOS Break Command

 RETURN       : None


 DESCRIPTION  : Calls the shell to turn enable or disable the DOS CTL-C 
 						and CTL-BREAK handlers.

 ALGORITHM    : (see discription).



 END_MANUAL_ENTRY
****************************************************************************/
#pragma argsused
void DOSBreakCommand(int on)
{
#if 0
	REGISTERS regs;

	regs.w.ax = 0x3301;				/* DOS call */
	regs.w.dx = on ? 1:0;			/* DOS set break function */
	NWShellRequest(&regs, USE_DS | USE_ES | USE_DOS);		
#endif
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DOSSetCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : DOSSetCommand(char *name, char *value, int masterEnvFlag)

 PARAMETERS   :   -> input          <-output 
 					-> name- - - - - - The name of the environment variable that
										needs to be set (e.g. PATH= )
					-> value - - - - - The Value to set the environment variable
										to (e.g. c:\test;F:.;G:.)
					-> masterEnvFlag - A flag indicating wether this variable
										should be placed in the master environment
										or just the local (Login's environment).  
										Normally, this will be set to TRUE (1).


 RETURN       : None


 DESCRIPTION  : Does basically a DOS SET command.  It updates LOGIN's
 						environment (and the master environment if the flag
						is set).



 ALGORITHM    : Allocate memory for putenv call
 						Put it into LOGIN's environment
					  If masterFlag is TRUE
					  	set in master environment
					  If this is comspec
					  	Set the DOS internal comspec pointer

 END_MANUAL_ENTRY
****************************************************************************/
int DOSSetCommand(char *name, char *value, int masterEnvFlag, PSPACE *p)
{
int	i, ccode = 0;
char 	*temp;

	i = lstrlen(name) + lstrlen(value) + 3;		/* Add three for = and double null at end of environment */

	/* ***** DO NOT FREE TEMP.
		putenv and GetEnvironmentString uses a pointer to the value and does not actually
		store the value. If it is freed, GetEnvironmentString will fail since it will not be
		pointing to a string.
	*/

	if ((temp = (char *)malloc(i)) == NULL)
	{
		ScriptError(MEMORY_ERROR,p);
		return(1);
	}
	else
	{
		lstrcpy(temp,name); 	/* use lstrcpy/strcat so \ is not treated as an escape char KPW 6-23-93 */
		strcat(temp,"=");
		strcat(temp,value);
		if (putenv(temp) != 0)
		{
			ScriptError(SCRIPT_PUTENV,p,temp);
			ccode = 2;
		}
	}
	masterEnvFlag = masterEnvFlag;
	return(ccode);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DriveCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : DriveCommand(int letter)

 PARAMETERS   :   -> input          <-output 
              -> letter - - - A drive letter to set the current drive to.


 RETURN       : None


 DESCRIPTION  : Changes the current drive to the letter passed in.



 ALGORITHM    : (see description)



 END_MANUAL_ENTRY
****************************************************************************/
int DriveCommand(int letter, PSPACE *p)
{
int 	ccode;

	if ((ccode = _chdrive(letter - 'A' + 1)) != 0)
	{
		ScriptError(SCRIPT_CANT_SET_DRIVE,p, letter);
	}
	return(ccode);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ExitCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : ExitCommand(char *string)

 PARAMETERS   :   -> input          <-output 
              -> string - - - A command to execute upon exit


 RETURN       : None


 DESCRIPTION  : Copies a string into a buffer that will be put into the 
 						keyboard buffer upon exit.
					Upon exit this command will be executed.

		NOTE: The exit string code is VERY DOS SPECIFIC but is still compiled for
				OS/2.  It should never be executed under OS/2 because the
				machine type should never be IBM_PC.


 ALGORITHM    : Check to see if the exit string is support.  (ie. that this
 							machine is pcCompatible, and is an IBM_PC)
					  Make sure string is not too long for a command to DOS
					  Copy the string into a buffer that will be executed when
					  	LOGIN exits.



 END_MANUAL_ENTRY
****************************************************************************/

void ExitCommand(char *string,PSPACE *p)
{
	if (string[0] == 0)
	{
		p->doExit = EXIT_NO_STRING;
		return;
	}
	else
	{
		ScriptError(SCRIPT_NOT_SUPPORTED_EXIT,p);
	}
}


/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ExternalCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : ExternalCommand(char *name, char *args)

 PARAMETERS   :   -> input          <-output 
              -> name- - - - The external command to execute 
              -> args- - - - Any arguments the command needs


 RETURN       : None

 DESCRIPTION  : Spawns command.com and executes the passed in command

 ALGORITHM    : Checks for the LOGOUT command (We do not want to logout
 							of the server we are reading scripts from).
					  Spawn the command passed in.

 END_MANUAL_ENTRY
****************************************************************************/
#pragma argsused
int ExternalCommand(char *name, char *args, PSPACE *p)
{
	char buf[500];
	int rc,fd = 0;

//	wsprintf(buf,"%s %s >c:\\tmp\\_lgnscpt.out",name,args);
	wsprintf(buf,"%s %s",name,args);
rc = WinExec(buf,SW_SHOWNORMAL);
fd = fd;
#if 0
	fd = CreateFile("c:\\windows\\_lgnscpt.bat",0);
	rc = 0;
	if (fd != -1)
	{
		if (WriteFile(fd,buf,lstrlen(buf)) > 0)
		{
			CloseHandle(fd);
//			rc = WinExec("_lgnscpt.pif",SW_SHOWMINIMIZED);
			rc = WinExec("_lgnscpt.pif",SW_SHOWNORMAL);
		}
		else
		{
			CloseHandle(fd);
		}
	}
#endif

	if (rc <= 32)
	{
		ScriptError(SCRIPT_NOT_SUPPORTED_EXTERNAL_COMMAND,p);
	}
	else
	{
//			DisplayFileCommand("c:\\tmp\\_lgnscpt.out",FALSE,"", p);
//			unlink("c:\\windows\\_lgnscpt.bat");
//			unlink("c:\\tmp\\_lgnscpt.out");
	}
	return(0);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( FirePhasersCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : FirePhasersCommand(int count)

 PARAMETERS   :   -> input          <-output 
              -> count - - - Number of times to beep the speaker


 RETURN       : None


 DESCRIPTION  : Beeps the speaker like a phaser



 ALGORITHM    : Beep the speaker the amount of times passed in.



 END_MANUAL_ENTRY
****************************************************************************/
void FirePhasersCommand(int count)
{
	if (count <= 0)				/* If count is Zero or negative Return */
	{
		return;
	}
#if 0
	while (count--)
	{
		MessageBeep(0);
	}
#else
	while (count--)
	{
		sndPlaySound("phasers.wav",SND_SYNC);
	}
#endif
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( MachineNameCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : MachineNameCommand(char *name)

 PARAMETERS   :   -> input          <-output 
              -> name- - - - The name the user wants to change the machine
											name to.

 RETURN       : The return code from a call to the shell to set the machine 
 						name


 DESCRIPTION  :  Sets the machine name to the name passed in 



 ALGORITHM    : if DOS is 3.1x or greater,
 						call the shell to set the machine name to the name passed.



 END_MANUAL_ENTRY
****************************************************************************/
int MachineNameCommand(char *name,PSPACE *p)
{
	int 	position, retCode = 0;
	char 	machineName[16];

	if ( (_osmajor == 3 && _osminor >= 1) || _osmajor >= 4)
	{
		REGISTERS regs;
		FillMemory(machineName, '\0', 15);
		for (position = 0; position < 15 && name[position] != '\0'; position++)
			machineName[position] = name[position];
		regs.w.ax = 0x5E01;			/* DOS 3.1+ Set Machine Name */
		regs.w.cx = 0x0100;
		regs.w.dx = FP_OFF(machineName);
		regs.w.ds = FP_SEG((char  *)machineName);
		NWShellRequest(&regs, 0);
		// USE_DS | USE_ES | USE_DOS);
		retCode = regs.w.flags & 1? -1: 0;
	}
	else
		ScriptError(SCRIPT_MUST_BE_DOS31,p, "MACHINE NAME");
	return(retCode);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( PauseCommand() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : PauseCommand(void)

 PARAMETERS   :   -> input          <-output 
                None


 RETURN       : The key pressed


 DESCRIPTION  : Prints a pause message and waits for a key press



 ALGORITHM    : (see description)



 END_MANUAL_ENTRY
****************************************************************************/

int PauseCommand(PSPACE *p)
{
	MessageBeep(0);
	PauseDialogBox(p->ScriptWin);
	return(0);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CheckMailbox() )

 MODULE       : cmds.c

 GLOBALS USED : 


 SYNTAX       : CheckMailbox(char *mailStatus)

 PARAMETERS   :   -> input          <-output 
 					-> mailStatus - - - - A string indicating whether mail is 
													waiting or not.  

 RETURN       : None


 DESCRIPTION  : Checks to see if new mail has been sent to the user



 ALGORITHM    : Finds a file under the users mail directory that has
 						been unopened by the user.
					  If it finds one then fills the mailstatus buffer with YES
					  Else it fills the buffer with no.
					  If we are on OS/2 it fills the mailstatus buffer with NOT
					  	Supported under OS/2

 END_MANUAL_ENTRY
****************************************************************************/
void CheckMailbox(PSPACE *p)
{
	char 	mailPath[31];
	unsigned ccode = 0;
	struct find_t c_file;

	wsprintf(mailPath,"SYS:MAIL\\%lX\\????????.I??\0",p->userObjectID);
	ccode = FindFirstFile(mailPath, _A_NORMAL, &c_file);

	if (ccode == 0)
		DSALMGetMessage(ANSWER_YES,p->mailStatus,sizeof(p->mailStatus));
	else
		DSALMGetMessage(ANSWER_NO,p->mailStatus,sizeof(p->mailStatus));
}
/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DisplayDrive() )

 MODULE       : mapcmd.c

 GLOBALS USED : 


 SYNTAX       : DisplayDrive()

 PARAMETERS   :   -> input          <-output 
					int letter		A drive letter passed.

 RETURN       :No return codes

 DESCRIPTION  :Displays the mapping (i.e. Volume object name:directory or 
					server\volume:directory) associated with a drive.
				 	It will first try to display the Volume object name:directory
					(Under directory services)	unless it cannot be found 
					or the physical parameter flag is set.

 ALGORITHM    : 

 END_MANUAL_ENTRY
****************************************************************************/

void DisplayDrive(int letter,PSPACE *p)
{
WORD	ccode,
		scope;

NWCONN_HANDLE		connectionID;

char 	curDir[PATH_SIZE],
		server[SERVER_NAME_SIZE],
		base[MAX_DN_BYTES],
		mess[100]; 
WORD	NDSConnection;

	NWDSContextHandle	context;						/* Current context				*/

char  *tempPtr;

	if ((context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		ScriptError(CANNOT_CREATE_CONTEXT,p);
		goto Error1;
	}
	
	ccode = NWGetDriveStatus(letter-'A'+1,NW_FORMAT_NETWARE, &scope, &connectionID, base, curDir, NULL);

	switch (ccode)
	{
		case 0:
			/* If drive info available, get the server name */
			if (ccode || (ccode = NWGetFileServerName(connectionID, server)) != 0)
			{
				ScriptError(SYSTEM_ERROR,p);
				goto Error2;
			}

			/* if the connection is a NDS connection, try to get the volume
				object name and then append the current directory onto it.
				This code will only get the object pointed to by the object id
				stamped on the volume. If that object is deleted or lost for some
				reason the object name will not be available. The use may also
				map to another object name, not the original, or a directory
				map object and all we can get back is the original object name.
			*/
			if ((NDSConnection = isNDSConnection(connectionID)) != 0)
			{
				if (GetVolumeObjectName(context,connectionID,(unsigned char *)base))
					/* cannot get object name, show as physical server/vol */
					NDSConnection = FALSE;
			}

			tempPtr = curDir;
			while((tempPtr = NWLSTRCHR(tempPtr, '/')) != NULL)
				*tempPtr = '\\';

			if (NDSConnection)
			{
				/* show the NDS object name with current directory */
				ScriptOutput((BYTE *)DSALMGetMessage(FULL_DS_MAPPED_PATH,(char *)mess,sizeof(mess)),p,(char)letter,base,curDir);
			}
			else
			{
				/* show as server/vol bindery type mapping */
				tempPtr = base;
				while((tempPtr = NWLSTRCHR(tempPtr, '/')) != NULL)
					*tempPtr = '\\';
	
				ScriptOutput((unsigned char *)DSALMGetMessage(FULL_MAPPED_PATH,(char *)mess,sizeof(mess)),p, letter, server, base, curDir);
			}
			break;

		case NW_INVALID_DRIVE:
			ScriptError(DISK_UNDEFINED,p,letter);
			break;
  }

Error2:
	
	NWDSFreeContext(context);

Error1:
} /* end of DisplayDrive */


/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DisplayAllDrives() )

 MODULE       : mapcmd.c

 GLOBALS USED : 


 SYNTAX       : DisplayAllDrives()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: Nothing is passed in.

 RETURN       : None
 *  On Exit: Nothing is returned.

 DESCRIPTION  : 
 *	Function: Goes through all drives and displays the mapping associated
 *					with them.

 ALGORITHM    : 

 END_MANUAL_ENTRY
****************************************************************************/

void DisplayAllDrives(PSPACE *p)
{
WORD	prevCcode=0,
		scope,
		i,
		singularMessage;

NWCONN_HANDLE	connectionID;

char	buffer[128],mess[100];

	*buffer = 0;
	for (i = 'A'; i <= 'Z'; i++)
	{
		NWGetDriveStatus(i-'A'+1,NW_FORMAT_DRIVE, &scope, &connectionID, NULL, NULL, NULL);

		/* must be a valid drive to proceed
			1. local drives not remapped
			2. not a search drive
		*/
		if ( scope & NW_LOCAL_DRIVE && !(scope & NW_NETWARE_DRIVE)
			|| scope
 			)
		{
			/* if a local drive, add to local drive string */
			if(  scope & NW_LOCAL_DRIVE &&			/* If local drive */
				!(scope & NW_NETWARE_DRIVE) )
			{
				if((scope & NW_LOCAL_DRIVE) &&
					(prevCcode & NW_LOCAL_DRIVE) )	/* If previous drive was a local drive */
				{
					if (*buffer)
						strcat(buffer, ",");				/* Append a ',' and drive to message */
					strcat(buffer, &(char)i);			/* Single Byte drive letters */
					singularMessage = FALSE;
				}
				else											/* If previous drive was not a local drive start a new buffer of drives */
				{
					lstrcpy(buffer, &(char)i);			/* Single Byte drive letters */
					singularMessage = TRUE;
				}
			}
			else
			{
				if(*buffer)									/* Do we need to print the local drives? */
				{
					ScriptOutput((unsigned char *)DSALMGetMessage((singularMessage ? SINGLE_MAP_TO_LOCAL_DISK
						: MAPPINGS_TO_LOCAL_DISK),(char *)mess,sizeof(mess)), p,buffer);				/* Drives A, B, C map to a local disk */
					*buffer = 0;
				}
				DisplayDrive(i,p);							/* Display non-local drive */
			}
			prevCcode = scope;							/* Save the previous code so that we can see if it was local or not */
		}
	}
	if(prevCcode & NW_LOCAL_DRIVE)					/* If we did not find any network drives after the local drives, print out the local drives */
	{
		ScriptOutput((unsigned char *)DSALMGetMessage((singularMessage ? SINGLE_MAP_TO_LOCAL_DISK
				: MAPPINGS_TO_LOCAL_DISK),mess,sizeof(mess)),p, buffer);	/* Drives A, B, C map to a local disk */
	}
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( MapCommand() )

 MODULE       : mapcmd.c

 GLOBALS USED : 


 SYNTAX       : MapCommand()

 PARAMETERS   :   -> input          <-output 
 *  On Entry: The command line string is passed in.


 RETURN       : None
 *  On Exit : Displays any errors and returns the completion code.


 DESCRIPTION  : 
 *  Function: Duplicates the string.
 *				Executes the first command up to the semi-colon.
 *					(NOTE that a user can type in multiple map commands on the
 *					 command line by seperating the commands by a semi-colon.
 *					 For, example map f:=h:;s16:=server\sys:test;next server\sys:).
 *				If the user only typed map, then display mappings.
 *				Otherwise, send command line to parser and execute the appropriate
 *					function.  The parser will set the parsedCmdType variable.

 ALGORITHM    : 

 END_MANUAL_ENTRY
****************************************************************************/
	
int MapCommand(char *cmdString, PSPACE *p)
{
int	ccode = 0;
static int displayMappings = 1,displayErrors = 1;
char 	*mapOpLine,
		*nextOp,
		*thisOp;
char drivestr[3];

	if ((mapOpLine = strdup(cmdString)) == NULL)			/* Duplicate the command line */
	{
		ScriptError(MEMORY_ERROR,p);
		return(-1);
	}

	/* remove trailing spaces and ; (spd 24150 KPW 11-12-92) */
	thisOp = &mapOpLine[lstrlen(mapOpLine)-1];		/* get pointer to end */
	while(lstrlen(mapOpLine) > 0 && (*thisOp == ' ' || *thisOp == ';'))
	{
		*thisOp = '\0';
		NWPREVCHAR(mapOpLine,thisOp);
	}

	nextOp = thisOp;
	for (thisOp = mapOpLine; thisOp != NULL; thisOp = nextOp)	/* While we have more operations, see next instruction */
	{
		if (NWIsDSAuthenticated())
		{
			p->attachNDS = NDS;										/* reset for each map command */
		}

		if ((nextOp = NWLSTRCHR(thisOp, ';')) != NULL)	/* Check for more then one operation, i.e. map f:=v:;w:=y:, maps f to v: and w to y: */
		{
			*nextOp++ = 0;
		}
		if (*thisOp == 0)											/* If operation == NULL */
		{
			DisplayAllDrives(p);
		}
		else if (!ParseLine(MAPCOMMAND, thisOp,p))		  	/* Else parse the command line */
		{
			ScriptError(SCRIPT_NO_PARSE,p);
		}
		else
		{
			ccode = 0;
			switch(p->parsedCmdType)
			{
				case MAP_DISPLAY_ON:
					displayMappings = 1;
					break;

				case MAP_DISPLAY_OFF:
					displayMappings = 0;
					break;
				
				case MAP_ERRORS_ON:
					displayErrors = 1;
					break;
				
				case MAP_ERRORS_OFF:
					displayErrors = 0;
					break;

				case MAP_NEXT_DRIVE:
					/*	The user wants to map his first unused network drive
						to the path.  Find the first unused drive for MAP_ADD_DRIVE 
						to map for us and	fall through to the next case.
					*/
					if (p->parsedDrive == 0)
					{
						p->parsedDrive = FirstUnusedDrive();
						if (p->parsedDrive == -1 && displayErrors)
						{
							ScriptError(ALL_DRIVES_MAPPED,p);
						   break;
						}
						p->parsedDrive += 'A' - 1;
					}
					drivestr[0] = (char)p->parsedDrive;
					drivestr[1] = 0;
					ccode = DoMapdrive((BYTE *)drivestr,(BYTE *)p->parsedName,(int)p->parsedRoot);
					if (displayMappings)
					{
						DisplayDrive(p->parsedDrive,p);
					}
					break;
					
				case MAP_ADD_DRIVE:
					drivestr[0] = (char)p->parsedDrive;
					drivestr[1] = 0;
					ccode = DoMapdrive((BYTE *)drivestr,(BYTE *)p->parsedName,(int)p->parsedRoot);
					if (displayMappings)
					{
						DisplayDrive(p->parsedDrive,p);
					}
					break;
				
				case MAP_DISPLAY_DRIVE:
					DisplayDrive(p->parsedCondition,p);
					break;
				
				case MAP_DEL_DRIVE:
					drivestr[0] = (char)p->parsedDrive;
					drivestr[1] = 0;
					ccode = DoDeleteDrive((unsigned char *)drivestr);
					break;

				case MAP_DISPLAY_SEARCH:
				case MAP_CHANGE_SEARCH:
				case MAP_CHANGE_DRIVE_TO_SDRIVE:
				case MAP_CHANGE_INS_DRIVE_TO_SDRIVE:
					ScriptError(SCRIPT_NOT_SUPPORTED_SEARCH_DRIVE,p);
					break;

				case MAP_INS_SEARCH:
				case MAP_ADD_SEARCH:
					if (p->parsedDrive == 0)
					{
						p->parsedDrive = LastUnusedDrive();
						if (p->parsedDrive == -1 && displayErrors)
						{
							ScriptError(ALL_DRIVES_MAPPED,p);
						   break;
						}
						p->parsedDrive += 'A' - 1;
					}
					drivestr[0] = (char)p->parsedDrive;
					drivestr[1] = 0;
					ccode = DoMapdrive((BYTE *)drivestr,(BYTE *)p->parsedName,(int)p->parsedRoot);
					if (displayMappings)
					{
						DisplayDrive(p->parsedDrive,p);
					}
					break;

				case MAP_DEL_SEARCH:
				case MAP_INS_SEARCH_TO_SDRIVE:
				case MAP_ADD_SEARCH_TO_SDRIVE:
				case MAP_ADD_DRIVE_TO_SDRIVE:
					ScriptError(SCRIPT_NOT_SUPPORTED_SEARCH_DRIVE,p);
					break;
			}
			p->parsedDrive = 0;		/* Clear out drive letter so that we start clean next time through */
		}
	}
	free(mapOpLine);
	return(ccode);
} /* end of MapCommand */


