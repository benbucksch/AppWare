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

#define SYNCPASS_INTERNAL
#include <dstools.h>

#include "mx.h"
#include "utils.h"
#include "errors.h"
#include "alm_auth.h"
#include "resource.h"
#include "parse.h"
#include "parseut.h"
#include "cmds.h"

#ifdef __cplusplus

extern "C" {

#endif

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
#define SCRIPT_OKAY			1			/* Everything is okay.					*/
#define SCRIPT_FAILURE		2			/* Failure to parse script.			*/

#define MAXPATHSIZE			256		/* Maximum path size						*/
#define MAXERRSTRING		1024		/* Maximum error string length		*/
#define MAX_STRING			256		/* Maximum script line 254				*/
#define MAX_SCRIPT_ARGS	40
#define MAX_NESTED_IF_DEPTH	16

/*
 * ======================================================================
 *	SCRIPT module prototypes
 * ======================================================================
 */

/* Static function prototypes */
WORD DoScript(BYTE *ScriptFile, WORD scripttype, WORD *exitflag,
	AMEMBLOCKID *Errors, WORD output, PSPACE *p);

extern WORD ProcessScript (FILE *fp, BYTE *FileName, AMEMBLOCKID *Errors,
	WORD *exitflag, PSPACE *p);
extern void ScriptError(WORD error, PSPACE *p,...);
extern WORD DoMapdrive(BYTE *DriveName, BYTE *Path, int MapRoot);
extern WORD DoWinScript(BYTE *SFile, WORD *eflag, AMEMBLOCKID *Errs,PSPACE *pg);

#if LOGIN IS_DISABLED
/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ExecuteScript() )

 NAME       : ExecuteScript(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  :  This function will retrieve from the Visual Appbuilder the
 		file name of the script to parse and then call the function to actually
		parse the login script.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = TEXT...required...ScriptFile

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = LIST...optional...Errorlist output of line entry
														failures

 FLOW/RETURN:
		SCRIPT_OKAY			1 = Everything is okay.
		SCRIPT_FAILURE		2 = Problem executing login script

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK ExecuteScript (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE 	*ScriptFile;							/* Script File passed in		*/
		BYTE	errstr[MAXERRSTRING];				/* Error string					*/
		BYTE	errmsg[MAXERRSTRING];				/* Error message pointer		*/
		BYTE	*codemsg;								/* Error message pointer		*/
		BYTE	mess[2048];

		AMEMBLOCKID			ErrorList;				/* Error list for output		*/

		WORD					ExitValue;				/* Exit value for output		*/
		WORD					filenamesize;			/* Size of the filename			*/

		NWCCODE				ccode;					/* Return Codes 					*/

		AOBJECTID				oiScriptFile;			/* Server ALM object				*/
		AOBJECTID				oiErrorOutput;			/* Server ALM object				*/
		AOBJECTID				oiExitFlag;				/* Server ALM object				*/

		PSPACE				parsevars;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = SCRIPT_FAILURE;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = SCRIPT_OKAY;
	FillMemory(&l->parsevars,0,sizeof(l->parsevars));
	l->ErrorList = 0;
	l->ExitValue = 0;

  /*
   * Create output error list object and exitflag.
   */
  l->oiErrorOutput = AObjCreate(OTYPE_LIST);
	l->oiExitFlag = AObjCreate(OTYPE_FLAG);

	/*
	 * Retrieve the logout parameters from the Visual Appbuilder.  The 
	 * server/tree and the flag are both optional.  Retrieve their values
	 * and set the defaults if necessary.
	 */
	l->oiScriptFile = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (l->oiScriptFile != 0)
	{
		l->filenamesize = OTextGetLength(l->oiScriptFile) + 1;
		l->ScriptFile = (BYTE *)malloc(l->filenamesize);
		if (l->ScriptFile == NULL)
		{
			DisplayIDMessage(MEMORY_ERROR,NO_ICON);
			theSystem->message1 = SCRIPT_FAILURE;
			goto Terminate;
		}
		FillMemory(l->ScriptFile,0,l->filenamesize);
		OTextGetToBuffer(l->oiScriptFile, (char *)l->ScriptFile,l->filenamesize);
	}
	if (l->oiScriptFile == 0 || *(l->ScriptFile) == '\0')
	{
		DisplayIDMessage(NO_SCRIPT_SPECIFIED,NO_ICON);
		theSystem->message1 = SCRIPT_FAILURE;
		goto Terminate;
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		theSystem->message1 = SCRIPT_FAILURE;
		goto Terminate;
	}

	FillMemory(&l->parsevars,0,sizeof(l->parsevars));
	FillPrimaryConnInfo (&l->parsevars.primaryLoginConnection);
	l->ccode = DoWinScript(l->ScriptFile,&l->ExitValue,
		&l->ErrorList,&l->parsevars);
#if 0
	if (l->ccode != SUCCESS)
	{
		FillMemory(l->errmsg,0,sizeof(l->errmsg));
		lstrcpyn(l->errmsg,DSALMGetMessage(SCRIPT_ERROR,l->mess,sizeof(l->mess)),
			sizeof(l->errmsg));
		l->codemsg = DSALMGetMessage(l->ccode,l->mess,sizeof(l->mess));
		wsprintf(l->errstr,l->errmsg,l->ScriptFile,l->codemsg);
		DisplayTextMessage(l->errstr,NO_ICON);
		theSystem->message1 = SCRIPT_FAILURE;
	}
#else
	if (l->ccode != SCRIPT_PROCESSING_OKAY)
	{
		theSystem->message1 = SCRIPT_FAILURE;
	}
#endif

Terminate:
	if (l != 0)
	{
		if (l->oiExitFlag != 0)
		{
			OFlagSet(l->oiExitFlag,l->ExitValue);
			AFuncSetParameter(1,l->oiExitFlag);
		}
		if (l->ScriptFile != NULL)
		{
			free(l->ScriptFile);
		}
		if (l->oiErrorOutput != 0 && l->ErrorList != 0)
		{
			MyOutputList(l->oiErrorOutput,&l->ErrorList,2);
		}
		else
		{
			AObjDestroy(l->oiErrorOutput);
		}
		delete l;
	}

	return;
}
#endif

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DoScript() )

 NAME         : DoScript (BYTE *ScriptFile)

 DESCRIPTION  : This will parse the login script file passed in and execute
 		the requested functions.
 
 PARAMETERS   :   -> input          <-output 
                ->  ScriptFile	= server to logout from

 RETURN       :  WORD
		SYSTEM_ERROR	= General system error
		MEMORY_ERROR	= Could not allocate memory

 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD DoScript (BYTE *ScriptFile, WORD scripttype, WORD *exitflag,
	AMEMBLOCKID *Errors, WORD output, PSPACE *p)
{
	WORD			errcode;							/* Return code from function	*/
	struct locals
	{
		NWDSContextHandle	Context;	  			/* Current context				*/

		WORD			DSAuthen;

		BYTE			*attrName;
		BYTE			DefaultScriptName[MAX_STRING];
		BYTE			mess[2048];

		uint32		syntaxID;

		FILE			*fp;

		int			nwdsHandle;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->fp = NULL;
	l->nwdsHandle = -1;

	p->nesting++;
	if (p->nesting <= 1)
	{
		MXglobalsInit(&p->mxg);
	}
	FillMemory(p->scriptfile,0,sizeof(p->scriptfile));
	FillMemory(p->scripterror,0,sizeof(p->scripterror));
	FillMemory(p->scriptquestion,0,sizeof(p->scriptquestion));
	p->ScriptlineNumber = 0;
	p->se_perrors = 0;

	p->WinOutput = output;
	lstrcpy((char *)p->scriptfile,(char *)ScriptFile);
	if (!NWIsDSAuthenticated())
	{
		p->NDSorBindery = BINDERY;
		if (NWGetPrimaryConnectionID(&p->scriptconnID) != 0)
		{
			if (NWGetDefaultConnectionID(&p->scriptconnID) != 0)
			{
				errcode = SCRIPT_NO_VALID_CONNECTION;
				ScriptError(errcode,p);
				goto Terminate;
			}
		}
	}
	else
	{
		p->NDSorBindery = NDS;
	}
	errcode = SCRIPT_PROCESSING_OKAY;
	*exitflag = FALSE;
	l->attrName = 0;
	lstrcpy((char *)l->DefaultScriptName,(char *)DSALMGetMessage(DEFAULT_SCRIPTNAME,(char *)l->mess,sizeof(l->mess)));
	if ((l->Context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		errcode = CANNOT_CREATE_CONTEXT;
		ScriptError(errcode,p);
		goto Terminate;
	}
	
	/*
	 * If we are authenticated and the script file does not have the
	 * drive: front-end, then assume that the script filename that was
	 * given is a DS object and attempt to read that objects attribute.
	 */
	l->DSAuthen = NWIsDSAuthenticated();
	if (l->DSAuthen == TRUE && ScriptFile != NULL && ScriptFile[0] != NULL &&
		ScriptFile[1] != ':' &&
		(ReadAttribute((char *)ScriptFile,"Login Script",(void **)&l->attrName,
			&l->syntaxID) == 0) || scripttype == PROFILE_SCRIPT)
	{
		/*
		 * Malloced by ReadAttribute, but we don't care, so free it.
		 */
		if (l->attrName != 0)
		{
			free(l->attrName);
		}


		if (NWDSOpenStream(l->Context,(char *)ScriptFile,"Login Script",
			DS_READ_STREAM, &l->nwdsHandle) == 0)
		{
			if ((l->fp = fdopen(l->nwdsHandle, "r+t")) != NULL)
			{
				errcode = ProcessScript(l->fp,ScriptFile,Errors,exitflag,p);
				close(l->nwdsHandle);
			}
			else
			{
				errcode = CANNOT_OPEN_SCRIPTFILE;
				close(l->nwdsHandle);
				ScriptError(errcode,p,ScriptFile);
				goto Terminate;
			}
		}
		else
		{
			if ((l->fp = fopen((char *)ScriptFile, "r")) != NULL)
			{
				errcode = ProcessScript(l->fp,ScriptFile,Errors,exitflag,p);
			}
			else
			{
				errcode = CANNOT_OPEN_SCRIPTFILE;
				ScriptError(errcode,p,ScriptFile);
				goto Terminate;
			}
		}
	}
	else if (ScriptFile != NULL && *ScriptFile != NULL)
	{
		if ((l->fp = fopen((char *)ScriptFile,"r")) != NULL)
		{
			errcode = ProcessScript(l->fp,ScriptFile,Errors,exitflag,p);
		}
		else
		{
			errcode = CANNOT_OPEN_SCRIPTFILE;
			ScriptError(errcode,p,ScriptFile);
			goto Terminate;
		}
	}
	else
	{
		lstrcpy((char *)ScriptFile,(char *)l->DefaultScriptName);
		errcode = ProcessScript((FILE *)NULL,ScriptFile,
			Errors,exitflag,p);
	}
	
Terminate:
	if (l != 0)
	{
		if (l->fp != NULL)
		{
			fclose(l->fp);
		}
		if (l->Context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			NWDSFreeContext(l->Context);
		}
		delete l;
	}
	p->nesting--;
	if (p->nesting <= 0)
	{
		MXglobalsFree(&p->mxg);
	}
	
	return(errcode);
}
