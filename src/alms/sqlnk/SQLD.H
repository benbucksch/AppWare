/*                                                                        *\
    (c) 1993, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE		: SQLD.H                                                  |
 | DESCRIPTION	: SequeLink SQL Include File.	                          |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef SQLD_H
#define SQLD_H

#define STRICT
#include <windows.h>

#define ASTRICT
#include "a_alm.h"
#include "o_text.h"
#include "o_number.h"
#include "o_date.h"
#include "o_time.h"
#include "o_table.h"


/* ---- miscellaneus values ---- */
#define MXSTRSIZE	256		// maximum string size
#define MXVARSIZE	32		// maximum variable size 		


/* ---- ATYPEID for SequeLink SQL Object --------------------------------- */
#define OTYPE_SQLD	ATYPE_MAKEID('S', 'q', 'l', 'd')

/* ---- field separators ---- */
#define COMMA		','		// comma 
#define TAB		'\t'		// tab 

/* ---- commit and rollback options ---- */
#define MANUAL		0
#define AUTOCOMMIT	1
#define AUTOROLLBACK	2

/* ---- default values ---- */
#define DEF_ROWCNT	1		// default row count
#define DEF_FIELDSPRTR	TAB		// default field separator 
#define DEF_AUTOCOMMIT	MANUAL		// default auto commit options
#define DEF_VARNAME	"NewVar"	// default variable name
#define DEF_VARSIZE	20		// default variable size

/* ---- Server Types ---- */
#define SV_AS400	1
#define SV_MVS		2
#define SV_OS2ROUTER	3
#define SV_OS2		4
#define SV_VAX		5
#define SV_UNIX		6

/* ---- Network Protocol Types ---- */
#define NP_DECNET	3
#define NP_APPC		4
#define NP_TCPIP	5
#define NP_NETBIOS	6

/* ---- Database Types ---- */
#define DB_AS400	1
#define DB_DB2		2
#define DB_INFORMIX	3
#define DB_INGRES	4
#define DB_INTERBASE	5
#define DB_ORACLE	6
#define DB_OS2DBM	7
#define DB_RDB		8
#define DB_SYBASE	9

/* ---- SequeLink SQL Object signals ------------------------------------ */
#define SQLDIFERROR	0		// if error 

/* ---- SequeLink SQL Object datablocks --------------------------------- */
#define SQLDCOUNT	2		// number of sqld datablocks
#define	SQLDCORE	0		// sqld core info
#define SQLDVARS	1		// sqld variables

typedef struct sqld		Sqld;
struct sqld			/* ---- SequeLink SQL Object ------------ */
{ int		version;		// version number

  /* ---- connection parameters ----- */
  int		serverType;		// server type
  int		protocolType;		// protocol type
  char		linkParam[MXSTRSIZE];	// link parameter
  int		databaseType;		// database type
  char		logon1[MXSTRSIZE];	// 1st logon parameter
  char		logon2[MXSTRSIZE];	// 2nd logon parameter

  long		rowCount;		// number of rows
  int		colSprtr;		// column separator
  int		mrcType;		// manual, rollback, or commit

  char		spares[16];		// unused spares
};

typedef struct var		Var;
struct var			/* ---- Variable ------------------------ */
{ char		name[MXVARSIZE];	// name
  int		type;			// variable type
  long		size;			// variable size
  				
  AOBJECTID	obj;			// object
  ATYPEID	objType;		// object type
};


extern HINSTANCE	hInstance;
 	
#ifdef __cplusplus
extern "C" {				// assumes C declarations for C++ 	
#endif

long ALMAPI OSqldGetVersion(ATYPEID);
/* Returns SequeLink SQL object's DLL version number. */

/* ---- SequeLink SQL Object -------------------------------------------- */
long ALMAPI OSqldAdministrationRoutine(AOBJECTID, pAObjMessage);
/* SequeLink SQL object's administration routines. */

BOOL ALMCALLBACK OSqldObjectDialog(HWND, UINT, WPARAM, LPARAM);
/* SequeLink SQL object dialog. */

BOOL ALMCALLBACK OSqldConnSetsDialog(HWND, UINT, WPARAM, LPARAM);
/* SequeLink SQL connection settings dialog.*/

BOOL ALMCALLBACK OSqldVarsDialog(HWND, UINT, WPARAM, LPARAM);
/* SequeLink SQL variables dialog.*/

BOOL ALMCALLBACK OSqldOptionsDialog(HWND, UINT, WPARAM, LPARAM);
/* SequeLink SQL options dialog. */

BOOL ALMCALLBACK OSqldSubstrDialog(HWND, UINT, WPARAM, LPARAM);
/* SequeLink SQL substring dialog. */

void ALMAPI OSqldStartupMeth(pAEvtInfo, pAObjMessage);
/* SequeLink SQL object's constructor. */

void ALMAPI OSqldShutdownMeth(pAEvtInfo, pAObjMessage);
/* SequeLink SQL object's destructor. */

/* ---- The functions ---- */

int ALMCALLBACK OSqldCallbackBind(void **);
/* Callback bind function. */

int ALMCALLBACK OSqldCallbackSelect(void **);
/* Callback select function. */

void ALMAPI OSqldBuildQueryFunc(pAEvtInfo, pAObjMessage);
/* Builds a query from an SQL script and send it to the database for
   processing.
*/

void ALMAPI OSqldCommitFunc(pAEvtInfo, pAObjMessage);
/* Commits changes made to the database. */

void ALMAPI OSqldConnectFunc(pAEvtInfo, pAObjMessage);
/* Establishes a connection to a database server. */

void ALMAPI OSqldDescribeTableFunc(pAEvtInfo, pAObjMessage);
/* Returns a list of column names and types for a given database table. */

void ALMAPI OSqldDisconnectFunc(pAEvtInfo, pAObjMessage);
/* Breaks the connection with a database server and releases its resources.*/

void ALMAPI OSqldDisposeQueryFunc(pAEvtInfo, pAObjMessage);
/* Releases resources used by an SQL query. */

void ALMAPI OSqldExecuteQueryFunc(pAEvtInfo, pAObjMessage);
/* Execute a SQL query. */

void ALMAPI OSqldFetchFunc(pAEvtInfo, pAObjMessage);
/* Retrieves data from the database.  Places results into the objects
   defined in the script.
*/

void ALMAPI OSqldFetchToTableFunc(pAEvtInfo, pAObjMessage);
/* Retrieves data from the database.  Places results into the table
   object.
*/

void ALMAPI OSqldFetchToTextFunc(pAEvtInfo, pAObjMessage);
/* Retrieves data from the database.  Places results into the text
   object.  Seperators are user-definable.
*/

void ALMAPI OSqldGetErrorFunc(pAEvtInfo, pAObjMessage);
/* Returns an error code and the associated error message for the most
   recent SequeLink error.
*/

void ALMAPI OSqldGetStatusFunc(pAEvtInfo, pAObjMessage);
/* Returns the status of the connection. */

void ALMAPI OSqldGetVariablesFunc(pAEvtInfo, pAObjMessage);
/* Returns a list of variable names and types defined at design-time. */

void ALMAPI OSqldLogonFunc(pAEvtInfo, pAObjMessage);
/* Logons to a database. */

void ALMAPI OSqldLogoffFunc(pAEvtInfo, pAObjMessage);
/* Logoffs of a database. */

void ALMAPI OSqldRollbackFunc(pAEvtInfo, pAObjMessage);
/* Rollbacks changes made to the database. */

void ALMAPI OSqldStopFetchFunc(pAEvtInfo, pAObjMessage);
/* Releases resources allocated for a fetch operation. */

#ifdef __cplusplus
} // extern "C" 
#endif // __cplusplus 

#endif // SQLD_H
