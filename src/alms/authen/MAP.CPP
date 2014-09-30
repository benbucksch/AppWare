/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

/*
 * This file contains the functions necessary for the MAP ALM
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
#include <io.h>

#include <a_alm.h>		/* declares AppWare Bus API prototypes			*/
#include <o_text.h> 	/* kdeclares OTYPE_TEXT and text object ALMs	*/
#include <o_flag.h>
#include <o_table.h>
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
#include "resource.h"
#include "alm_auth.h"
#include "errors.h"

#ifdef __cplusplus

extern "C" {

#endif

#include <dstools.h>
#define	WORD	unsigned short

#ifdef __cplusplus

}

#endif

/*
 * ======================================================================
 * Macro Definitions
 * ======================================================================
 */

/* Flows for MapDrive */																			
#define MAPDRIVE_OKAY			1		/* Everything is okay.					*/
#define MAPDRIVE_ERROR			2		/* Error on mapped drive				*/

/* Flows for DeleteMappedDrives */
#define	DELETEMAPDRIVE_OKAY	1		/* Successfully deleted drive			*/
#define DELETEMAPDRIVE_ERROR	2		/* Error deleting mapped drives		*/

/* Flows for GetMappedDrives */
#define	GETMAPDRIVES_OKAY		 1		/* Successfully go mapped drives		*/
#define	GETMAPDRIVES_ERROR	 2		/* Error getting mapped drives  		*/

/* Flows for MapDrives */
#define MAPDRIVES_FOKAY		1		/* Success									*/
#define MAPDRIVES_FERROR		2		/* Bad thing								*/

/* Flows for MapNext */
#define MAPNEXT_OKAY			1		/* Success									*/
#define MAPNEXT_ERROR	  	2		/* Bad thing								*/
#define MAPNEXT_NO_DRV     3		/* No Drive Available      			*/

#define MAXPATHSIZE			256		/* Maximum path size						*/
#define MAXDRIVESIZE		5			/* Maximum character size of drive	*/
#define MAXERRSTRING		1024

#define NOVOLUME				0x0f

/*
 * ======================================================================
 *	MAP module prototypes
 * ======================================================================
 */

WORD DoDeleteDrive(BYTE *DriveName);

/* Static function prototypes */
WORD DoMapdrive(BYTE *DriveName, BYTE *Path, int MapRoot);
static WORD NWToUNCPath(WORD connID, char *path, char *uncPath);
WORD GetVolumeObjectName(NWDSContextHandle context,
	NWCONN_HANDLE connectionID, BYTE *base);
static WORD DriveNumtoDrive(WORD drivenum, BYTE *DriveName);

#if LOGIN IS_DISABLED
/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( MapDrive() )

 NAME       : MapDrive(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  :  This function will take the drive specification and the
 		path given and construct a drive mapping.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = TEXT...required...DriveName
					GET2 = TEXT...required...Path
					GET3 = FLAG...optional...MapRoot, default=FALSE

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = none

 FLOW/RETURN:
		MAPDRIVE_OKAY			= Everything is okay.				
		MAPDRIVE_ERROR			= Something went wrong.

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK MapDrive (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE 	DriveName[MAXDRIVESIZE];			/* Drive to map					*/
		BYTE 	Path[PATH_SIZE];		  				/* Path to map drive to			*/

		int						MapRoot;	  				/* Flag to map root or not		*/
	
		NWCCODE				ccode;	  				/* Return Codes 					*/

		AOBJECTID				oiDrive;	  				/* Server ALM object				*/
		AOBJECTID				oiPath;	  				/* Server ALM object				*/
		AOBJECTID				oiMapRoot; 				/* Server ALM object				*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = MAPDRIVE_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));
	
	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = MAPDRIVE_OKAY;

	/*
	 * Retrieve the drive parameter from the Visual Appbuilder.
	 *
	 * This field is NOT OPTIONAL.
	 */
	FillMemory(l->DriveName,0,sizeof(l->DriveName));
	l->oiDrive = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (l->oiDrive != 0)
	{
		OTextGetToBuffer(l->oiDrive,(char *)l->DriveName,
			sizeof(l->DriveName));
		/*
		 * Get rid of the trailing : if there is one.
		 */
		if (lstrlen((char *)l->DriveName) > 1 &&
			l->DriveName[lstrlen((char *)l->DriveName) - 1] == ':')
		{
			l->DriveName[lstrlen((char *)l->DriveName) - 1] = '\0';
		}
	}
	if (l->oiDrive == 0 || *(l->DriveName) == NULL)
	{
		theSystem->message1 = MAPDRIVE_ERROR;
		DisplayIDMessage(NO_DRIVE_SPECIFIED,NO_ICON);
		goto Terminate;
	}

	/*
	 * Retrieve the path from the appbuilder.
	 *
	 * This field is NOT OPTIONAL.
	 */
	FillMemory(l->Path,0,sizeof(l->Path));
	l->oiPath = AFuncGetTypedParameter(2, OTYPE_TEXT);
	if (l->oiPath != 0)
	{
		OTextGetToBuffer(l->oiPath, (char *)l->Path, sizeof(l->Path));
	}
	if (l->oiPath == 0 || *(l->Path) == NULL)
	{
		theSystem->message1 = MAPDRIVE_ERROR;
		DisplayIDMessage(NO_PATH_SPECIFIED,NO_ICON);
		goto Terminate;
	}
	

	/*
	 * Now get the root flag from the appbuilder.  If this field is
	 * not specified, then default it to FALSE.
	 *
	 * This field is OPTIONAL.
	 */
	l->MapRoot = FALSE;
	l->oiMapRoot = AFuncGetTypedParameter(3, OTYPE_FLAG);
	if (l->oiMapRoot != 0)
	{
		l->MapRoot = (OFlagGet(l->oiMapRoot) != 0) ? TRUE : FALSE;
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		theSystem->message1 = MAPDRIVE_ERROR;
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		goto Terminate;
	}

	l->ccode = DoMapdrive(l->DriveName,l->Path,l->MapRoot);
	if (l->ccode != 0)
	{
		theSystem->message1 = MAPDRIVE_ERROR;
		DisplayIDMessage(l->ccode,NO_ICON);
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return;
}

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( MapNext() )

 NAME       : MapNext(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  :  This function will take the path specification and
 		map it to the next unmapped network drive.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = TEXT...required...Path
					GET2 = FLAG...optional...MapRoot, default=FALSE

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = TEXT ... optional ... drive letter used

 FLOW/RETURN:
 		MAPNEXT_OKAY			= Everything is okay.				
      MAPNEXT_ERROR			= Something went wrong.
      MAPNEXT_NO_DRV       = No drive available.

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK MapNext (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE 	DriveName[MAXDRIVESIZE];			/* Drive to map					*/
		BYTE 	Path[PATH_SIZE];		  				/* Path to map drive to			*/

		int						MapRoot;	  				/* Flag to map root or not		*/
		WORD					drive;	  				/* Drive to look for				*/

		NWCCODE				ccode;

		AOBJECTID				oiPath;	  				/* Server ALM object				*/
		AOBJECTID				oiMapRoot; 				/* Server ALM object				*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = MAPNEXT_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = MAPNEXT_OKAY;

	/*
	 * Retrieve the path from the appbuilder.
	 *
	 * This field is NOT OPTIONAL.
	 */
	FillMemory(l->Path,0,sizeof(l->Path));
	l->oiPath = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (l->oiPath != 0)
	{
		OTextGetToBuffer(l->oiPath,(char *)l->Path,sizeof(l->Path));
	}
	if (l->oiPath == 0 || *(l->Path) == NULL)
	{
		theSystem->message1 = MAPNEXT_ERROR;
		DisplayIDMessage(NO_PATH_SPECIFIED,NO_ICON);
		goto Terminate;
	}

	/*
	 * Now get the root flag from the appbuilder.  If this field is
	 * not specified, then default it to FALSE.
	 *
	 * This field is OPTIONAL.
	 */
	l->MapRoot = FALSE;
	l->oiMapRoot = AFuncGetTypedParameter(2, OTYPE_FLAG);
	if (l->oiMapRoot != 0)
	{
		l->MapRoot = (OFlagGet(l->oiMapRoot) != 0) ? TRUE : FALSE;
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		theSystem->message1 = MAPNEXT_ERROR;
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		goto Terminate;
	}

	l->drive = FirstUnusedDrive();
	if ((int)l->drive == -1)
	{
   	/* added new flow 'No drive' 4/12/95 DLS - SPD 77104 */
		theSystem->message1 = MAPNEXT_NO_DRV;
		DisplayIDMessage(ALL_DRIVES_MAPPED,NO_ICON);
		goto Terminate;
	}

	l->DriveName[0] = l->drive + 'A' - 1;
	l->DriveName[1] = NULL;
	l->ccode = DoMapdrive(l->DriveName,l->Path,l->MapRoot);
	if (l->ccode != 0)
	{
		DisplayIDMessage(l->ccode,NO_ICON);
		theSystem->message1 = MAPNEXT_ERROR;
	}

Terminate:
    /* added 4/12/95 DLS */
	if ( theSystem->message1 == MAPNEXT_OKAY )
	{  // successful execution - return drive letter used
	   AOBJECTID    oiDriveUsed;

      oiDriveUsed = AObjCreate( OTYPE_TEXT );
	   if ( oiDriveUsed != 0 )
	   {
         OTextSetValueFromBuffer( oiDriveUsed, l->DriveName );
         AFuncSetParameter( 1, oiDriveUsed );
	   }
	   else
	      // mapping actually succeeded, but could not return the drive letter used
          theSystem->message1 = MAPNEXT_ERROR;
	}

	if (l != 0)
	{
		delete l;
	}

	return;
}

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( MapDrives() )

 NAME       : MapDrives(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  :  This function will take a three column table input
 		that contains Drive/Path/Root_True_False and create the identified
		drive mappings.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = TABLE...required...Drive mappings, must be a minimum
														of two columns where the first
														column is the drive letter, the
														second is the path and a third
														column may exist with the map
														root flag.

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = LIST...optional...ErrorList with the entry numbers of
														the table that were in error.

 FLOW/RETURN:
		MAPDRIVES_OKAY	  	1		= Everything is okay.				
		MAPDRIVES_FAILURE	2		= Something is wrong

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK MapDrives (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE 	DriveName[MAXDRIVESIZE];  			/* Drive to map					*/
		BYTE 	Path[PATH_SIZE];			  			/* Path to map drive to			*/
		BYTE	errstr[PATH_SIZE];		  			/* Error string					*/
		BYTE	*lpCell;						  			/* Pointer to cell				*/
		BYTE	errmsg[MAXERRSTRING];	  			/* Error message output			*/
		BYTE	*codemsg;					  			/* Error code interpretation	*/
		BYTE	mess[100];

		AMEMBLOCKID			TableCell;	  			/* buffer for input				*/
		AMEMBLOCKID			ErrorList;	  			/* Error output list				*/

		int						MapRoot;		  			/* Flag to map root or not		*/
		long					numrows;		  			/* Number of entrys to do		*/
		int					numcols;		  			/* Number of entrys to do		*/
		WORD					i;				  			/* looping							*/
		WORD					entry;		  			/* Count line number of table	*/
		WORD					rc;			  			/* Function return code			*/

		NWCCODE				ccode;		  			/* Return Codes 					*/

		AOBJECTID				oiTable;		  			/* Server ALM object				*/
		AOBJECTID				oiErrorOutput;			/* Error list ALM object		*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = MAPDRIVES_FERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = MAPDRIVES_FOKAY;
	l->ErrorList = 0;
	l->TableCell = 0;

	/*
	* Create output error list object.
   */
  l->oiErrorOutput = AObjCreate(OTYPE_LIST);

	/*
	 * Retrieve the drive parameter from the Visual Appbuilder.
	 *
	 * This field is NOT OPTIONAL.
	 */
	l->oiTable = AFuncGetTypedParameter(1, OTYPE_TABLE);
	l->numrows = tblGetRowCount(l->oiTable);
	l->numcols = tblGetColumnCount(l->oiTable);
	if (l->oiTable == 0 || l->numrows <= 0)
	{
		theSystem->message1 = MAPDRIVE_ERROR;
		DisplayIDMessage(NO_MAPPINGS_SPECIFIED,NO_ICON);
		goto Terminate;
	}
	if (l->numcols < 2)
	{
		theSystem->message1 = MAPDRIVE_ERROR;
		DisplayIDMessage(BAD_INPUT_TABLE,NO_ICON);
		goto Terminate;
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		theSystem->message1 = MAPDRIVE_ERROR;
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		goto Terminate;
	}

	l->TableCell = AMemAllocate(0);
	l->entry = 1;
	for (l->i = 0; l->i < l->numrows; l->i++)
	{
		tblGetCell(l->oiTable,l->TableCell,0,l->i);
		l->lpCell = (unsigned char *)AMemLock(l->TableCell);
		FillMemory(l->DriveName,0,sizeof(l->DriveName));
		lstrcpyn((char *)l->DriveName,(char *)l->lpCell,sizeof(l->DriveName) - 1);
		AMemUnlock(l->TableCell);

		tblGetCell(l->oiTable,l->TableCell,1,l->i);
		l->lpCell = (unsigned char *)AMemLock(l->TableCell);
		FillMemory(l->Path,0,sizeof(l->Path));
		lstrcpyn((char *)l->Path,(char *)l->lpCell,sizeof(l->Path));
		AMemUnlock(l->TableCell);

		l->MapRoot = FALSE;
		if (l->numcols >= 3)
		{
			tblGetCell(l->oiTable,l->TableCell,2,l->i);
			l->lpCell = (unsigned char *)AMemLock(l->TableCell);
			if (strncmpi((char *)l->lpCell,
				DSALMGetMessage(TRUE_STRING,(char *)l->mess,sizeof(l->mess)),4) == 0)
			{
				l->MapRoot = TRUE;
			}
			AMemUnlock(l->TableCell);
		}

		l->ccode = DoMapdrive(l->DriveName,l->Path,
			l->MapRoot);
		if (l->ccode != 0)
		{
			if (lstrlen((char *)l->DriveName) > 1 &&
				l->DriveName[lstrlen((char *)l->DriveName) - 1] == ':')
			{
				l->DriveName[lstrlen((char *)l->DriveName) - 1] = NULL;
			}
			theSystem->message1 = MAPDRIVES_FERROR;
			FillMemory(l->errmsg,0,sizeof(l->errmsg));
			DSALMGetMessage(MAPDRIVES_ERROR,(char *)l->errmsg,sizeof(l->errmsg));
			l->codemsg = (unsigned char *)DSALMGetMessage(l->ccode,(char *)l->mess,sizeof(l->mess));
			wsprintf((char *)l->errstr,(char *)l->errmsg,(char *)l->DriveName,
				(char *)l->codemsg);
			strcat((char *)l->errstr,DSALMGetMessage(OK_OR_STOP,(char *)l->mess,sizeof(l->mess)));
			l->rc = DisplayTextMessage((char *)l->errstr,
				(MB_ICONINFORMATION | MB_YESNO));
			wsprintf((char *)l->errstr,"%d",l->entry);
			AddToList(&l->ErrorList,l->errstr);
			if (l->rc != IDYES)
			{
				goto Terminate;
			}
		}
		l->entry++;
	}

Terminate:
	if (l != 0)
	{
		if (l->TableCell != 0)
		{
			AMemFree(l->TableCell);
		}
		if (l->oiErrorOutput != 0 && l->ErrorList != 0)
		{
			MyOutputList(l->oiErrorOutput,&l->ErrorList,1);
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
 BEGIN_MANUAL_ENTRY ( DoMapdrive() )

 NAME         : DoMapdrive(BYTE *DriveName, BYTE *Path, int MapRoot)

 DESCRIPTION  :  This function will take the drive specification and the
 		path given and construct a drive mapping.

 PARAMETERS   :   -> input          <-output 
 						-> DriveName = name of drive to map
						->	Path = path to map to
						-> MapRoot = flag to map root or not

 RETURN       :  WORD
		0							= Everything is okay.				
		NODRIVESPECIFIED		= No drive given							
		NOPATHSPECIFIED		= No path given							
		INVALIDDRIVE			= Invalid drive given					
		INVALIDPATH				= Invalid path given					
		NOCONNECTIONTOSERVER	= No connection to desired server	
		VOLUMENOTEXIST			= Volume does not exist				
		NOMAPLOCAL				= Cannot map to local drives			
		CONNECTIONFULL			= No connection table left.			
		UNKNOWNSERVER			= No server found in system			
		NOTLOGGEDIN				= Not logged into server				
		SYSTEM_ERROR			= General system error					
		MEMORY_ERROR			= Could not do a malloc


 END_MANUAL_ENTRY
 ============================================================================
*/
WORD DoMapdrive(BYTE *DriveName, BYTE *Path, int MapRoot)
{
	WORD errcode;

	struct locals
	{
		BYTE	volume[PATH_SIZE]; 					/* Volume of path					*/
		BYTE	dir[PATH_SIZE];			  			/* directory path					*/
		BYTE	server[SERVER_NAME_SIZE]; 			/* Server of path					*/
		BYTE	UNCName[PATH_SIZE + 2];	  			/* UNC path name					*/
		BYTE	wrkPath[PATH_SIZE + 2];	  			/* temporary path variable		*/

		WORD					isObject;	  			/* Is path to an object			*/
		WORD					connStatus;	  			/* Connection info				*/
		WORD					drivenum;	  			/* Given drive number			*/

		NWCONN_HANDLE		ConnectionID; 			/* Connection ID for current	*/

		unsigned short		tempID;		  			/* BAD LIBRARY PARAMETERS		*/

		NWDSContextHandle	Context;		  			/* Current context				*/

		NWCCODE				ccode;		  			/* Return Codes 					*/

		uint32				cxFlags;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	errcode = SUCCESS;

	if (*DriveName == NULL)
	{
		errcode = NO_DRIVE_SPECIFIED;
		goto Terminate;
	}
	/*
	 * Get rid of the trailing : if there is one.
	 */
	if (lstrlen((char *)DriveName) > 1 &&
		DriveName[lstrlen((char *)DriveName) - 1] == ':')
	{
		DriveName[lstrlen((char *)DriveName) - 1] = '\0';
	}
	NWLSTRUPR((char *)DriveName);
	if (*DriveName == '*')
	{
		l->drivenum = 0;
		l->drivenum = atoi((char *)DriveName + 1);
		if (l->drivenum < 1 || l->drivenum > MAXDRIVES)
		{
			errcode = INVALID_DRIVE;
			goto Terminate;
		}
		if (DriveNumtoDrive(l->drivenum,DriveName) != 0)
		{
			errcode = INVALID_DRIVE;
			goto Terminate;
		}
		NWLSTRUPR((char *)DriveName);
	}
	/*
	 * Now validate the letter that was given.
	 * Only accept single character drives (no S1 because we do not do
	 * search drives.
	 */
	l->drivenum = DriveName[0] - 'A' + 1;
	if (l->drivenum > MAXDRIVES || l->drivenum < 1 || lstrlen((char *)DriveName) > 1)
	{
		errcode = INVALID_DRIVE;
		goto Terminate;
	}

	if (*Path == NULL)
	{
		errcode = NO_PATH_SPECIFIED;
		goto Terminate;
	}

	/*
	 * Parse up the specified path into its server, volume, directory
	 * components.
	 */
	FillMemory(l->server,0,sizeof(l->server));
	FillMemory(l->volume,0,sizeof(l->volume));
	FillMemory(l->dir,0,sizeof(l->dir));
	l->isObject = FALSE;
	if (NWIsDSAuthenticated())
	{
		if ((l->Context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{	
			errcode = CANNOT_CREATE_CONTEXT;
			goto Terminate;
		}
		l->ccode = NWDSGetContext(l->Context,DCK_FLAGS,&l->cxFlags);
		l->cxFlags = l->cxFlags | DCV_TYPELESS_NAMES;
		l->ccode = NWDSSetContext(l->Context,DCK_FLAGS,&l->cxFlags);

		l->ccode = NWDSParsePath(l->Context,(char *)Path,(char *)l->server,
			&l->tempID,(char *)l->volume,(char *)l->dir,&l->isObject);
		l->ConnectionID = l->tempID;
		NWDSFreeContext(l->Context);
	}
	else
	{
		NWLSTRUPR((char *)Path);
		l->ccode = NWParsePath((char *)Path,(char *)l->server,&l->ConnectionID,
			(char *)l->volume,(char *)l->dir);
	}
	strcat((char *)l->volume,":");
	/*
	 * Handle possible errors -- no connetion to server, no volume or general
	 * system error.
	 */
	if (l->ccode == NOVOLUME)
	{
		errcode = VOLUME_NOT_EXIST;
		goto Terminate;
	}
	else if (l->ccode == NO_CONNECTION_TO_SERVER)
	{
		errcode = NO_CONNECTION_TOSERVER;
		goto Terminate;
	}
	else if (l->ccode == INVALID_PATH)
	{
		errcode = INVALIDPATH;
		goto Terminate;
	}
	else if (l->ccode != 0)
	{
		errcode = SYSTEM_ERROR;
		goto Terminate;
	}

	/*
	 * To make the path a root mapping, set the volume to include the entire
	 * path so that the call below will map in the VLM to the entire path
	 * and not to just the volume.
	 */
	if (MapRoot == TRUE)
	{
		strcat((char *)l->volume,(char *)l->dir);
		l->dir[0] = NULL;
	}

	/*
	 * If server was identified (because the map is to a network drive) then
	 * attach to the file server and see if a connection can be made.
	 */
	if (*l->server == NULL)
	{
		errcode = NO_MAP_LOCAL;
		goto Terminate;
	}
	NWLSTRUPR((char *)l->server);
	NWLSTRUPR((char *)l->volume);
	NWLSTRUPR((char *)l->dir);
	l->ccode = NWAttachToFileServer((char *)l->server,(WORD)NULL,
		&l->ConnectionID);
	if (l->ccode == CONNECTION_TABLE_FULL)
	{
		errcode = CONNECTION_FULL;
		goto Terminate;
	}
	else if (l->ccode == SERVER_NO_ROUTE || l->ccode == UNKNOWN_FILE_SERVER)
	{
		errcode = UNKNOWN_SERVER;
		goto Terminate;
	}
	else if (l->ccode != 0 && l->ccode != ALREADY_ATTACHED)
	{
		errcode = NO_CONNECTION_TOSERVER;
		goto Terminate;
	}
	else if (l->ccode == 0 || l->ccode == ALREADY_ATTACHED)
	{
		/*
		 * Once a connection is established see if we are logged into
		 * the server.
		 */
		DSConnStatus(l->ConnectionID, &l->connStatus);
		if ((l->connStatus & CONNECTION_LOGGED_IN) == 0)
		{
			errcode = NOT_LOGGED_IN;
			goto Terminate;
		}
	}
	else
	{
		errcode = SYSTEM_ERROR;
		goto Terminate;
	}

	/*
	 * Now turn the given path into a UNCpath and see if it is accessible
	 * to the user.
	 */
	if (*l->dir != NULL)
	{
		FillMemory(l->UNCName,0,sizeof(l->UNCName));
		NWGetFileServerName(l->ConnectionID, (char *)l->server);
		wsprintf((char *)l->wrkPath,"%s/%s/%s",(char *)l->server,(char *)l->volume,
			(char *)l->dir);
		NWToUNCPath(l->ConnectionID,(char *)l->wrkPath,(char *)l->UNCName);
		if (lstrlen((char *)l->UNCName) > 1 &&
			l->UNCName[lstrlen((char *)l->UNCName) - 1] == '\\')
		{
			l->UNCName[lstrlen((char *)l->UNCName) - 1] = '\0';
		}
		if (access((char *)l->UNCName,00) != 0)
		{
			errcode = INVALIDPATH;
			goto Terminate;
		}
	}

	/*
	 * Now see if we are already mapped to this drive letter.  If we are
	 * then we must first delete the drive mapping and then make the new
	 * one.
	 */
	l->ccode = NWGetDrivePath(l->drivenum,(WORD)NULL,NULL,NULL,NULL);
	if (l->ccode == 0)
	{
		l->ccode = NWDeleteDriveBase(l->drivenum, (WORD)NULL);
		if (l->ccode != 0)
		{
			errcode = SYSTEM_ERROR;
			goto Terminate;
		}
	}

	/*
	 * Create the new drive mapping.
	 */
	l->ccode = NWSetDriveBase(l->drivenum,l->ConnectionID,0,
		(char *)l->volume, (WORD)NULL);
	if (l->ccode != 0)
	{
		errcode = SYSTEM_ERROR;
		goto Terminate;
	}

	/*
	 * If a directory was specified, then make that the current working
	 * directory.
	 */
	if (*l->dir != NULL)
	{
		wsprintf((char *)l->wrkPath,"%c:%s",l->drivenum + 'A' - 1,(char *)l->dir);
		chdir((char *)l->wrkPath);
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return(errcode);
}

#if LOGIN IS_DISABLED
/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DeleteMappedDrive() )

 NAME       : DeleteMappedDrive(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  :	This function will delete the drive mapping that is
 		specified in the parameters.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = LIST...required...List of drive mappings to delete

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = LIST...optional...Error list of entries that failed

 FLOW/RETURN:
		DELETEMAPDRIVE_OKAY	= Successfully deleted drive
		DELETEMAPDRIVE_ERROR	= something went wrong

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK DeleteMappedDrive (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE 	DriveName[MAX_DN_BYTES];			/* Drive to map					*/
		BYTE	*DriveList;								/* Pointer to list of drives	*/
		BYTE	*DLptr;
		BYTE	errstr[MAXERRSTRING];				/* Error output string			*/
		BYTE	errmsg[MAXERRSTRING];				/* Temp error string				*/
		BYTE	*codemsg;								/* Code string						*/
		BYTE	mess[100];

		AMEMBLOCKID			ErrorList;				/* Error output list				*/

		WORD					listsize;				/* size of list					*/
		WORD					entry;					/* Item count						*/
		WORD					rc;						/* Function return code			*/

		AOBJECTID				oiDriveList; 			/* Server ALM object				*/
		AOBJECTID				oiErrorOutput;			/* Error list object				*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = DELETEMAPDRIVE_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = DELETEMAPDRIVE_OKAY;
	l->ErrorList = 0;

	/*
   * Create output error list object.
   */
  l->oiErrorOutput = AObjCreate(OTYPE_LIST);

	/*
	 * Retrieve the map parameters from the Visual Appbuilder.  The 
	 * drive and directory paths are NOT optional.  Retrieve their values.
	 */
	FillMemory(l->DriveName,0,sizeof(l->DriveName));
	l->oiDriveList = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (l->oiDriveList != 0)
	{
		l->listsize = OTextGetLength(l->oiDriveList) + 1;
		l->DriveList = l->DLptr = (BYTE *)malloc(l->listsize);
		if (l->DriveList == NULL)
		{
			DisplayIDMessage(MEMORY_ERROR,NO_ICON);
			theSystem->message1 = DELETEMAPDRIVE_ERROR;
			goto Terminate;
		}
		FillMemory(l->DriveList,0,l->listsize);
		OTextGetToBuffer(l->oiDriveList,(char *)l->DriveList,
			l->listsize);
	}
	if (*(l->DriveList) == NULL)
	{
		DisplayIDMessage(NO_MAPPINGS_SPECIFIED,NO_ICON);
		theSystem->message1 = DELETEMAPDRIVE_ERROR;
		goto Terminate;
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		DisplayIDMessage(SYSTEM_ERROR,NO_ICON);
		theSystem->message1 = DELETEMAPDRIVE_ERROR;
		goto Terminate;
	}

	l->DriveList = Gettoken(l->DriveList,l->DriveName);
	l->entry = 1;
	while (*l->DriveName != NULL)
	{
	  	l->rc = DoDeleteDrive(l->DriveName);
		if (l->rc != 0)
		{
			FillMemory(l->errmsg,0,sizeof(l->errmsg));
			DSALMGetMessage(DELETEMAP_ERROR,(char *)l->errmsg,sizeof(l->errmsg));
			l->codemsg = (unsigned char *)DSALMGetMessage(l->rc,(char *)l->mess,sizeof(l->mess));
			wsprintf((char *)l->errstr,(char *)l->errmsg,l->DriveName,
				l->codemsg);
			strcat((char *)l->errstr,DSALMGetMessage(OK_OR_STOP,(char *)l->mess,sizeof(l->mess)));
			theSystem->message1 = DELETEMAPDRIVE_ERROR;
			wsprintf((char *)l->errmsg,"%d",l->entry);
			AddToList(&l->ErrorList,l->errmsg);
			l->rc = DisplayTextMessage((char *)l->errstr,
				(MB_ICONINFORMATION | MB_YESNO));
			if (l->rc != IDYES)
			{
				goto Terminate;
			}
		}
		l->DriveList = Gettoken(l->DriveList,l->DriveName);
		l->entry++;
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
		if (l->DLptr != NULL)
		{
			free(l->DLptr);
		}
		delete l;
	}

	return;
}
#endif

WORD DoDeleteDrive (BYTE *DriveName)
{
	WORD errcode = 0;

	struct locals
	{
		WORD drivenum;
		NWCCODE ccode;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	NWLSTRUPR((char *)DriveName);
	l->drivenum = DriveName[0] - 'A' + 1;
	if (l->drivenum < 1 || l->drivenum > MAXDRIVES)
	{
		errcode = INVALID_DRIVE;
		goto Terminate;
	}
	l->ccode = NWGetDrivePath(l->drivenum,(WORD)NULL,NULL,NULL,NULL);
	if (l->ccode == 0)
	{
		if (NWDeleteDriveBase(l->drivenum, (WORD)NULL) != 0)
		{
			errcode = SYSTEM_ERROR;
			goto Terminate;
		}
	}
	else
	{
		errcode = NO_DRIVE_MAPPED;
		goto Terminate;
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return(errcode);
}

#if LOGIN IS_DISABLED

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( GetMappedDrives() )

 NAME       : GetMappedDrives(pAEvtInfo, pAObjMessage theSystem)

 DESCRIPTION  :  This is an ALM function that will retrieve all of the 
 		currently mapped drives in the system and output them to a table
		object.

 PARAMETERS : -> input   <-output 
              -> pAEvtInfo
              -> pAObjMessage theSystem

 ALM INPUT  : GET1 = ... GETn =
              GET1 = none

 ALM OUTPUT : SET1 = ... SETn =
              SET1 = TABLE...required...Output of drive mappings.  The table
														may be 0 to n columns.  If it is 0
														then this will make it 3, otherwise
														the first 3 columns are modified.
														The first column has the drive
														letter, the second the path, and the
														third the map root flag.

 FLOW/RETURN:
 		GETMAPDRIVES_OKAY		= all okay
		GETMAPDRIVES_ERROR	= something went wrong

 END_MANUAL_ENTRY
 ===========================================================================
*/
void CALLBACK GetMappedDrives (pAEvtInfo, pAObjMessage theSystem)
{
	struct locals
	{
		BYTE	drive;									/* letter to put drive mapping*/
		BYTE	volume[PATH_SIZE];					/* Volume of path					*/
		BYTE	server[SERVER_NAME_SIZE];			/* Server of path					*/
		BYTE	dir[PATH_SIZE];						/* directory path					*/
		BYTE	*p;										/* pointer for traversal		*/
		BYTE	column1[PATH_SIZE];					/* result output string			*/
		BYTE	column2[PATH_SIZE];					/* result output string			*/
		BYTE	column3[PATH_SIZE];					/* result output string			*/
		BYTE	fullpath[PATH_SIZE];					/* result output string			*/
		BYTE	mess[100];

		NWDSContextHandle	Context;					/* Context for system calls	*/
	
		WORD					connStatus;				/* Connection info				*/
		WORD					scope;					/* Internal Novell use			*/
		WORD					isNDS;					/* is drive an NDS drive		*/
		WORD					numcols;					/* Number of columns in table	*/
		WORD					maxConnects;			/* Max connections supported	*/
		WORD					numActive;				/* Number active connections	*/

		ATYPEID				type;						/* Output type						*/

		NWCONN_HANDLE		ConnectionID;	  		/* Connection ID for current	*/
		NWCONN_HANDLE		*connectList;			/* Buffer for connection list	*/
	
		NWCCODE				ccode;			  		/* Return Codes 					*/

		uint32				contextFlags;

		AOBJECTID				oiTable;			  		/* Server ALM object				*/
	} *l;


	l = new struct locals;
	if (l == 0)
	{
		DisplayIDMessage(MEMORY_ERROR,NO_ICON);
		theSystem->message1 = GETMAPDRIVES_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	/*
	 * Initialize the return code to be successful.
	 */
	theSystem->message1 = GETMAPDRIVES_OKAY;

	l->oiTable = AFuncGetOutput(1, &l->type);
	if (l->oiTable == 0 || l->type != OTYPE_TABLE)
	{
		theSystem->message1 = GETMAPDRIVES_ERROR;
		goto Terminate;
	}
	tblSetRowCount( l->oiTable, 0 ); /* clear the table - 4/12/95 DLS - SPD# 95515 */
	l->numcols = tblGetColumnCount(l->oiTable);
	if (l->numcols	== 0)
	{
		tblSetColumnCount(l->oiTable,3);
		l->numcols = 3;
	}
	/*
	 * Fall through and initialize the column titles.  If there are 3 or
	 * more columns, then set all colums, two or more -- just the first 
	 * two, one column, then just do the one.
	 */
	switch (l->numcols)
	{
	default:
	case 3:
		tblColumnTitle(l->oiTable,
			DSALMGetMessage(COLUMN3_HEADER,(char *)l->mess,sizeof(l->mess)),2);

	case 2:
		tblColumnTitle(l->oiTable,
			DSALMGetMessage(COLUMN2_HEADER,(char *)l->mess,sizeof(l->mess)),1);

	case 1:
		tblColumnTitle(l->oiTable,
			DSALMGetMessage(COLUMN1_HEADER,(char *)l->mess,sizeof(l->mess)),0);
	}

	/*
	 * Initialize with network calls.
	 */
	if (NWCallsInit((void  *)NULL,(void  *)NULL) != 0)
	{
		theSystem->message1 = GETMAPDRIVES_ERROR;
		goto Terminate;
	}

	NWGetMaximumConnections(&l->maxConnects);
	l->connectList =
		(NWCONN_HANDLE *)malloc(l->maxConnects * sizeof(NWCONN_HANDLE));
	l->ccode = NWGetConnectionList(0,l->connectList,l->maxConnects,
		&l->numActive);
	if (l->numActive == 0)
	{
		DisplayIDMessage(NO_CONNECTIONS,NO_ICON);
		theSystem->message1 = GETMAPDRIVES_ERROR;
		goto Terminate;
	}
	for (l->drive = 1; l->drive <= MAXDRIVES; l->drive++)
	{
		l->scope = 0;
		l->ccode = NWGetDriveStatus(l->drive,NW_FORMAT_NETWARE,
			&l->scope,&l->ConnectionID,(char *)l->volume,(char *)l->dir,
			(char *)l->fullpath);
		if (l->ccode == 0 && (l->scope & NW_NETWARE_DRIVE) != 0)
		{
			l->ccode = NWGetFileServerName(l->ConnectionID,
				(char *)l->server);
			if (l->ccode != 0)
			{
				theSystem->message1 = GETMAPDRIVES_ERROR;
				goto Terminate;
			}
			DSConnStatus(l->ConnectionID,&l->connStatus);
			l->isNDS =
				((l->connStatus & CONNECTION_NDS) != 0) ? TRUE : FALSE;
			if (l->isNDS == TRUE)
			{
				if ((l->Context = NWDSCreateContext()) ==
					(NWDSContextHandle)ERR_CONTEXT_CREATION)
				{	
					DisplayIDMessage(CANNOT_CREATE_CONTEXT,NO_ICON);
					theSystem->message1 = GETMAPDRIVES_ERROR;
					goto Terminate;
				}
				NWDSGetContext(l->Context,DCK_FLAGS,&l->contextFlags);
				l->contextFlags = l->contextFlags | DCV_TYPELESS_NAMES;
				NWDSSetContext(l->Context,DCK_FLAGS,&l->contextFlags);
				if (GetVolumeObjectName(l->Context,l->ConnectionID,
					l->volume) != 0)
				{
					l->isNDS = FALSE;
				}
				NWDSFreeContext(l->Context);
			}
			l->p = l->dir;
			while ((l->p = (unsigned char *)NWLSTRCHR((char *)l->p,'/')) != NULL)
			{
				*(l->p) = '\\';
			}
			if (l->isNDS == TRUE)
			{
				wsprintf((char *)l->column1,
					DSALMGetMessage(COLUMN1_FORMAT,(char *)l->mess,sizeof(l->mess)),
					(char *)l->drive + 'A' - 1);
				wsprintf((char *)l->column2,
					DSALMGetMessage(COLUMN2_NDSFORMAT,(char *)l->mess,sizeof(l->mess)),
					(char *)l->volume,(char *)l->dir);
			}
			else
			{
				l->p = l->volume;
				while ((l->p = (unsigned char *)NWLSTRCHR((char *)l->p,'/')) != NULL)
				{
					*(l->p) = '\\';
				}
				wsprintf((char *)l->column1,
					DSALMGetMessage(COLUMN1_FORMAT,(char *)l->mess,sizeof(l->mess)),
					l->drive + 'A' - 1);
				wsprintf((char *)l->column2,
					DSALMGetMessage(COLUMN2_FORMAT,(char *)l->mess,sizeof(l->mess)),
					l->server,l->volume,l->dir);
			}
			/*
			 * Assume that the drive is not maproot.  Then look and see
			 * if the volume ends in a :.  If it does then it is not a
			 * maprooted drive, otherwise there will be a directory in the
			 * volume definition.
			 */
			DSALMGetMessage(FALSE_STRING,(char *)l->column3,sizeof(l->column3));
			if (lstrlen((char *)l->volume) > 1 &&
				l->volume[lstrlen((char *)l->volume) - 1] != ':')
			{
				DSALMGetMessage(TRUE_STRING,(char *)l->column3,sizeof(l->column3));
			}
			switch (l->numcols)
			{
			case 1:
	 			AddToTable(l->oiTable,l->column1);
				break;

			case 2:
	 			AddToTable(l->oiTable,l->column1,l->column2);
				break;

			case 3:
	 			AddToTable(l->oiTable,l->column1,l->column2,
					l->column3);
				break;
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
#endif

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( NWToUNCPath() )

 NAME         : NWToUNCPath (WORD connID, char *path, char *uncPath)

 DESCRIPTION  : This function will convert the path into a UNC path with 
 		all of the backslashes in the correct places.

 PARAMETERS   :   -> input          <-output 
                ->  connID = connection
					  ->	path = path to convert
                <-  uncPath = converted path

 RETURN       :  WORD
 		INVALID_PATH = bad path
		return codes from NWParsePath
		return codes from NWGetFileServerName

 END_MANUAL_ENTRY
 ===========================================================================
*/
static WORD NWToUNCPath (WORD connID, char *path, char *uncPath)
{
	WORD errcode;

	struct locals
	{
		BYTE	server[SERVER_NAME_SIZE];
		BYTE	volume[PATH_SIZE];
		BYTE	tempPath[255];
		char 	*tempPtr;

		int	i;

		WORD	ccode;
		WORD	foundServer;               /* set to default - no server     */
		WORD	foundVolume;               /* set to default - no volume     */
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	errcode = 0;
	l->foundServer = 0;
	l->foundVolume = 0;


	/*
	 * if local drive just copy in path to out path
	 */
  lstrcpy(uncPath, path);

  if (connID == 0)
	{
		errcode = INVALIDPATH;
		goto Terminate;
	}

	/*
	 * Go through the path looking for a \ or / before the : so that
	 * we know if a server was in the path.
	 */
  for (l->tempPtr = uncPath, l->i = 0;
		*(l->tempPtr) != ':' && *(l->tempPtr) != NULL; 
      l->tempPtr = NWNEXTCHAR(l->tempPtr), l->i++)
	{
		if (*(l->tempPtr) == '\\' || *(l->tempPtr) == '/')
		{
			/* if '\' before ':' we have a server */
			l->foundServer = 1;
		}
	}

	/*
	 * If we found a server and a : then we also have a volume.
	 */
  if (*(l->tempPtr) == ':' && l->i > 1)
	{
	  /* found a volume */
    l->foundVolume = 1;
	}
  else
	{
	  /* if no volume - no server       */
    l->foundServer = 0;
	}

  if (l->foundServer == 1 || (l->i == 1) || l->foundVolume == 0)
  {
		/*
		 * we have a drive letter, a full NW path or a relative path
		 */
		l->ccode = NWParsePath(path,(char *)l->server,NULL,(char *)l->volume,(char *)l->tempPath);
		if (l->ccode != 0)
		{
			errcode = l->ccode;
			goto Terminate;
		}
		/*
		 * local drive at this point error
		 */
		if (l->volume[1] == '\0')
		{
			errcode = INVALIDPATH;
			goto Terminate;
		}
	}
  else
  {                        
		/*
		 * must have a NW volume path
		 */
		
		/* replace the ':' with '\'       */
		*(l->tempPtr) = '\0';

		lstrcpy((char *)l->volume,uncPath);
		lstrcpy((char *)l->tempPath,(char *)++l->tempPtr);
		l->ccode = NWGetFileServerName(connID,(char *)l->server);
		if (l->ccode != 0)
		{
			errcode = l->ccode;
			goto Terminate;
		}
	}

	/*
	 * Now lets create the UNC path
	 */
	lstrcpy(uncPath, "\\\\");
	if (*(l->server) != '\0')
	{
		strcat(uncPath,(char *)l->server);
		strcat(uncPath,"\\");
	}
	if (*(l->volume) != '\0')
	{
		strcat(uncPath,(char *)l->volume);
		strcat(uncPath,"\\");
	}
	if (*(l->volume) && (*(l->tempPath) != '\\'))
	{
		strcat(uncPath,(char *)l->tempPath);
	}
	else
	{
		strcat(uncPath,(char *)&l->tempPath[1]);
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
 BEGIN_MANUAL_ENTRY ( GetVolumeObjectName() )

 NAME         :	GetVolumeObjectName(NWDSContextHandle context,
							NWCONN_HANDLE connectionID, BYTE *base)

 DESCRIPTION  : Converts an object name into a volume name.  If the volume
 		name is passed in then this will basically return the same thing.

 PARAMETERS   :   -> input          <-output 
                ->  context = context to do business with
					  ->	connectionID = connection id of the server
                <-  base = volume name

 RETURN       :  None

 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD GetVolumeObjectName (NWDSContextHandle context,
	NWCONN_HANDLE connectionID, BYTE *base)
{
	WORD	ccode;

	struct locals
	{
		WORD	volumeNumber;

		BYTE	saveBase[MAX_DN_BYTES];
		char 	*tempPtr;
		char	*saveBaseDir;

		NWVolExtendedInfo entryInfo;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		ccode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	lstrcpy((char *)l->saveBase,(char *)base);

	/*
	 * Go and get rid of the : in the volume name
	 */
	if ((l->tempPtr = NWLstrchr((char *)l->saveBase, ':')) != NULL)
	{
		*(l->tempPtr) = 0;
	}

	/*
	 * Now fetch out the directory information of the path
	 */
	l->saveBaseDir = NWLstrchr((char *)base, ':');
	l->saveBaseDir = strdup((char *)l->saveBaseDir);
	if (l->saveBaseDir == NULL)
	{
		ccode = -1;
		goto Terminate;
	}

	/*
	 * Now go and get the volume number of the object.
	 */
	ccode = NWGetVolumeNumber(connectionID,(char *)l->saveBase,&l->volumeNumber);
	if (ccode == 0)
	{
		/*
		 * Now go and get the volume information
		 */
		if ((ccode = NWGetExtendedVolumeInfo(connectionID,l->volumeNumber,
				&l->entryInfo)) == 0)
		{
			/*
			 * Now go and get the name corresponding to the volume entry
			 * received above.
			 */
			if ((ccode = NWDSMapIDToName(context, connectionID,
					l->entryInfo.DirectoryServicesObjectID,(char *)l->saveBase)) == 0)
			{
				strcat(lstrcpy((char *)base,(char *)l->saveBase),(char *)l->saveBaseDir);
			}
		}
	}

Terminate:
	if (l != 0)
	{
		free(l->saveBaseDir);
		delete l;
	}

	return(ccode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DriveNumtoDrive() )

 NAME         : DriveNumtoDrive (WORD drivenum, BYTE *DriveName)

 DESCRIPTION  : This function will accept a drive number and will convert
 		it to its drive letter equivalent.  This will also make sure that
		the drive is a valid entry.

 PARAMETERS   :   -> input          <-output 
                ->  drivenum		= drive number 1-26
                <-  DriveName	= 'A' - 'Z'

 RETURN       :  WORD
 		-1 = error in drive number
		0	= everything is fine

 END_MANUAL_ENTRY
 ===========================================================================
*/
static WORD DriveNumtoDrive (WORD drivenum, BYTE *DriveName)
{
	WORD errcode = 0;

	struct locals
	{
		WORD			firstdrive;			/* First network drive					*/
		WORD			lastdrive;			/* Last network drive					*/
		WORD			finaldrive;			/* Calculated drive						*/
		REGISTERS	regs;					/* Register for assembly call			*/
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->regs.w.bx = GET_FIRST_AVAIL_DRIVE;
	NWVLMRequest(0,VLM_ID_GENR,GENR_RDI,&l->regs,0);
	l->firstdrive = l->regs.w.bx - 'A' + 1;
	
	l->regs.w.bx = GET_CDS_INFO;
	NWVLMRequest(0,VLM_ID_GENR,GENR_RDI,&l->regs,0);
	l->lastdrive = l->regs.b.ch;
	if (l->lastdrive == 0)
	{
		l->lastdrive = MAXDRIVES;
	}
		
	l->finaldrive = drivenum + l->firstdrive - 1;
	if (l->finaldrive < l->firstdrive || l->finaldrive > l->lastdrive)
	{
		errcode = -1;
		goto Terminate;
	}
	DriveName[0] = 'A' + (l->finaldrive - 1);
	DriveName[1] = NULL;

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return(errcode);
}

