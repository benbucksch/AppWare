/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */
#include "windows.h"
#include "features.h"
#include "parse.h"
#include "resource.h"

	/* VARIABLES */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *	 Global Variables for the PARSER
 *		We would makes this local if the parser could handle that.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

char	scriptuserName[MAX_DN_BYTES];
NWCONN_HANDLE	scriptconnID;
primaryConnectionStruct	primaryLoginConnection;

/* parser ouput */
char	parsedString[MAX_STRING],
		parsedName[MAX_STRING],
		parsedUser[MAX_DN_BYTES],
		parsedServer[48],						/* explicit server name */
		parsedTree[33],				 		/* explicit tree name */
		parsedProfile[MAX_DN_BYTES],
		currTree[33]={0},						/* currently logged tree name */
		*lowerCommandLine,
		swapPath[128] = {0};

int	parsedCondition,
		parsedPhysical,
		parsedDrive=0,
		parsedRoot,
		doExit = 0,
		executeDefaultScript =1,
		NDSorBindery = 0,						/* this type of login */
		synchTimeFlag = 1;

BYTE 	clearScreen = FALSE,
			noAttach=FALSE,
			noScript = FALSE,						/* attach or /ns command */
			NDSPresent = FALSE,			 		/* can NDS be present, is vlm loaded */
			attachNDS = NDS,						/* what type of attach, NDS or BINDERY */
			noSwap = FALSE,						/* allow swapping */
			swap = TRUE;							/* memory swap on external commands */

	/* Initializers */
char	explicitLoginScript[MAX_STRING],
		machine[20],
		osVersion[20],
		os[20],
		requesterVersion[20],
		requesterContext[MAX_DN_BYTES] = {0},
		loginContext[MAX_DN_BYTES] = {0},
		leftStr[MAX_STRING],
		shellVersion[20],
		sMachine[20];
	
	/* identifiers */
char	fullName[512],
		lastName[128],
		loginName[9],
		serverName[48],
		aliasContext[2] = {"N\0"};

DWORD	userObjectID;
WORD	errorLevel;
int	daysLeft;

COMMANDTYPE parsedCmdType;


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Local Data (to parse.c and msupport.c)
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
char	ident[MAX_DN_BYTES],
		*tmp;
	
/* variable cmd line args start with 2, server is 1, user is 0 */
int argNumber = 2;
	
/* identifier variables */
MYDATETIME now;
unsigned int 	station;
BYTE 	netAddress[10];			/* should be 10 but there is a bug in the lib */
char mailStatus[80];	/* Leave room for expansion */
	
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Local Tables 
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
	
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

