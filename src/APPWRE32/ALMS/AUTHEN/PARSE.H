#ifndef __PARSE_H_
#define	__PARSE_H_

#include <nwcaldef.h>
#include <nwdsdefs.h>
#include <a_alm.h>
#include "mx.h"

#define MAX_STRING				256
#define MAX_SCRIPT_ARGS		40
#define NO_EXIT_COMMAND		0
#define EXIT_WITH_STRING		1
#define EXIT_NO_STRING			2

#define BINDERY					0
#define NDS						1

typedef enum PARSEFLAG
{
	SCRIPTCOMMAND = 'a', EXPANDIDENTIFIERS, MAPCOMMAND
} PARSEFLAG;

typedef enum COMMANDTYPE
{
	CMD_ATTACH, CMD_BREAK, CMD_CLEARSCREEN, CMD_COMSPEC, CMD_CONTEXT, 
	CMD_COMPATIBLE, CMD_DISPLAY, CMD_DISPLAY_LTIME, CMD_DOSBREAK, CMD_DOSVERIFY, 
	CMD_DRIVE, CMD_ELSE, CMD_END, CMD_EXIT, CMD_EXTERNAL, CMD_FIRE, CMD_GOTO, 
	CMD_IFDO, CMD_IF, CMD_INCLUDE, CMD_LABEL, CMD_MACHINE, CMD_MAP, CMD_NOTHING, 
	CMD_NO_DEFAULT, CMD_PAUSE,	CMD_SCRIPT_SERVER, CMD_SET, CMD_SET_TIME, 
	CMD_SHIFT, CMD_UNKNOWN, CMD_WRITE, MAP_ADD_DRIVE,	MAP_ADD_DRIVE_TO_SDRIVE, 
	MAP_ADD_SEARCH, MAP_ADD_SEARCH_TO_SDRIVE, MAP_CHANGE_SEARCH, 
	MAP_CHANGE_DRIVE_TO_SDRIVE, MAP_CHANGE_INS_DRIVE_TO_SDRIVE,	MAP_DEL_DRIVE, 
	MAP_DEL_SEARCH, MAP_DISPLAY_ALL_DRIVES, MAP_DISPLAY_ON, MAP_DISPLAY_OFF, 
	MAP_DISPLAY_DRIVE, MAP_DISPLAY_SEARCH, MAP_ERRORS_ON, MAP_ERRORS_OFF, 
	MAP_INS_SEARCH, MAP_INS_SEARCH_TO_SDRIVE, MAP_NEXT_DRIVE
} COMMANDTYPE;

enum RELTYPE {REL_U, REL_NE, REL_E, REL_GE, REL_LE, REL_G, REL_L};

typedef struct _MYDATETIME
{
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
	unsigned char hundredths;
	unsigned char day;
	unsigned char month;
	unsigned short year;
	short mytimezone;
	unsigned char weekday;
} MYDATETIME;

typedef struct primaryConnectionStruct
{
	BYTE	serverName[MAX_DN_BYTES];
	BYTE	userName[MAX_DN_BYTES];
	BYTE  password[MAX_DN_BYTES];
	NWCONN_HANDLE	connectionID;
	int 	serverVersion;
	int	serverLoginType;
}primaryConnectionStruct;

#define MAX_SCRIPT_ARGS      40

/* VARIABLES */
typedef struct pspace
{
	char *cmdLineArgs[MAX_SCRIPT_ARGS];
	char shiftArgs;

	NWCONN_HANDLE	scriptconnID;
	char	scriptuserName[MAX_DN_BYTES];
	char	ident[MAX_DN_BYTES];
	char	*tmp;
	int	daysLeft;
	char leftStr[MAX_STRING];


	BYTE	netAddress[10];
	unsigned int	station;


	char	parsedString[MAX_STRING],
					parsedName[MAX_STRING],
					parsedUser[MAX_DN_BYTES],
					fullName[512],
					parsedServer[48],
					parsedTree[33],
					currTree[33],
					parsedProfile[MAX_DN_BYTES],
					explicitLoginScript[MAX_STRING],

					machine[20],
					osVersion[20],
					os[20],
					requesterVersion[20],
					requesterContext[MAX_DN_BYTES],
					loginContext[MAX_DN_BYTES],
					shellVersion[20],
					sMachine[20],
					lastName[128],
					loginName[9],
					serverName[48],
					aliasContext[2],
					*lowerCommandLine,
					swapPath[128];

	int	executeDefaultScript,
					parsedCondition,
					parsedPhysical,
					parsedDrive,
					doExit,
					parsedRoot,
					synchTimeFlag,
					NDSorBindery;

	BYTE	clearScreen,
					noAttach,
					noScript,
					NDSPresent,
					attachNDS,
					noSwap,
					swap;

	WORD	errorLevel;
	DWORD	userObjectID;

	COMMANDTYPE parsedCmdType;
	int argNumber;
	int dayNames[7];
	char mailStatus[80];	/* Leave room for expansion */
	int monthNames[12];

	MYDATETIME now;

	char 	*exitString;

	int	pcCompatible;

	char	*tokenString[255];
	char	operation;
	char *emptystring;
	primaryConnectionStruct primaryLoginConnection;

	char	expandedLine[MAX_STRING];
	char	originalLine[MAX_STRING];

	int WinOutput;
	
	BYTE            scriptfile[MAX_STRING];
	BYTE            scripterror[MAX_STRING];
	BYTE            scriptquestion[MAX_STRING];
	AMEMBLOCKID      *se_perrors;
	WORD         ScriptlineNumber;
	HWND				ScriptWin;

	int			nesting;
	MXGLOBALS	mxg;
} PSPACE;

#endif
