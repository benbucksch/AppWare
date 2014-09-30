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
 | FILE		: QSQL.H                                                  |
 | DESCRIPTION	: Q+E ODBC Objects Include File.		          |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Add Database View.                   |
\*------------------------------------------------------------------------*/
#ifndef QSQL_H
#define QSQL_H

#define STRICT
#include <windows.h>

#define ASTRICT
#include "a_alm.h"
#include "o_text.h"
#include "o_number.h"
#include "o_date.h"
#include "o_time.h"


/* ---- miscellaneous values ---- */
#define MXSTRSIZE	256		// maximum string size 
#define MXVARSIZE	32		// maximum variable size 


/* ---- ATYPEID for Q+E ODBC Connection object --------------------------- */
#define OTYPE_QCNC	ATYPE_MAKEID('Q', 'c', 'n', 'c')     

/* ---- default values ---- */
#define DEF_AUTOCOMMIT	TRUE		// default auto commit 
	
/* ---- Q+E ODBC Connection object signals ---- */
#define QCNCIFERROR	0		// if error

/* ---- Q+E ODBC Connection object datablocks ---- */
#define QCNCCOUNT	1		// number of qcnc datablocks 
#define	QCNCCORE	0		// qcnc core info 

/* ---- ODBC Q+E ODBC Connection object core info ---- */
typedef struct qcnc		Qcnc;
struct qcnc			/* ---- ODBC Q+E ODBC Connection object - */
{ int		version;		// version number 

  int		dbmsID;			// dbms id 
  AOBJECTID	usernameObj;		// username: text object 
  AOBJECTID	passwordObj;		// password: text object 
  AOBJECTID	paramsObj;		// parameters: text object 
  BOOL		isAutoCommit;		// is auto commit? 
  BOOL		isDView;		// is database view? 

  char		spares[12];		// unused spares
};


/* ---- ATYPEID for Q+E ODBC SQL object ---------------------------------- */
#define OTYPE_QSQL	ATYPE_MAKEID('Q', 's', 'q', 'l')

/* ---- field separators ---- */
#define COMMA		','		// comma 
#define TAB		'\t'		// tab 

/* ---- default values ---- */
#define DEF_VARNAME	"NewVar"	// default variable name 
#define DEF_ROWCNT	1		// default row count 
#define DEF_COLSPRTR	TAB		// default column separator
#define DEF_OEM		FALSE		// default oem 

/* ---- Q+E ODBC SQL object signals ---- */
#define QSQLIFERROR	0		// if error

/* ---- Q+E ODBC SQL object datablocks ---- */
#define QSQLCOUNT	2		// number of qsql datablocks 
#define QSQLCORE	0		// qsql core info 
#define QSQLVARS	1		// qsql variables 

/* ---- Q+E ODBC SQL object core info ---- */
typedef struct qsql		Qsql;
struct qsql			/* ---- Q+E ODBC SQL object -------- */
{ int		version;		// version number 

  AOBJECTID	connectObj;		// connection: q+e odbc cnction obj 
  AOBJECTID	sqlObj;			// sql: text object 
  long		rowCount;		// number of rows 
  int		colSprtr;		// column seperator 
  BOOL		isOEM;			// is oem character sets? 
  BOOL		isDView;		// is database view?
  void		*objList;		// object list

  char		spares[8];		// unused spares
};

/* ---- Q+E ODBC SQL object Variables ---- */
typedef struct var		Var;
struct var			/* ---- Variable ------------------------ */
{ char		name[MXVARSIZE];	// name 
  AOBJECTID	obj;			// object 
  ATYPEID	type;			// object type 
};


extern HINSTANCE	hInstance;

#ifdef __cplusplus
extern "C" {				// assumes C declarations for C++ 
#endif // __cplusplus 

long ALMAPI GetSerVersion(ATYPEID);
/* Returns Q+E ODBC objects' DLL version number. */

/* ---- Q+E ODBC Connection object -------------------------------------- */
long ALMAPI OQcncAdministrationRoutine(AOBJECTID, pAObjMessage);
/* Q+E ODBC Connection object's administration routines. */

BOOL ALMCALLBACK OQcncObjectDialog(HWND, UINT, WPARAM, LPARAM);
/* Q+E ODBC Connection Object dialog. */

void ALMAPI OQcncStartupMeth(pAEvtInfo, pAObjMessage);
/* Q+E ODBC Connection objects's constructor. */

void ALMAPI OQcncShutdownMeth(pAEvtInfo, pAObjMessage);
/* Q+E ODBC Connection objects's destructor. */

/* ---- The functions ---- */
void ALMAPI OQcncCommitFunc(pAEvtInfo, pAObjMessage);
/* Commits the current transaction.  The current transaction is the set of
   SQL statements executed since the last successful call to either the
   Connect, Commit, or Rollback functions.
*/

void ALMAPI OQcncConnectFunc(pAEvtInfo, pAObjMessage);
/* Establishes a connection between the Q+E ODBC Connection object, and the
   DBMS.
*/

void ALMAPI OQcncDisconnectFunc(pAEvtInfo, pAObjMessage);
/* Breaks the connection between the Q+E ODBC Connection object and the
   DBMS.
*/

void ALMAPI OQcncGetErrorFunc(pAEvtInfo, pAObjMessage);
/* Returns the most recent Q+E ODBC error code, and the associated error
   message.
*/

void ALMAPI OQcncRollbackFunc(pAEvtInfo, pAObjMessage);
/* Rools-back the current transaction.  The current transaction is the set
   of SQL statements executed since the last successful call to either the
   Connect, Commit, or Rollback functions.
*/

void ALMAPI OQcncStatusFunc(pAEvtInfo, pAObjMessage);
/* Returns whether the Q+E ODBC Connection object is currently connected to
   the DBMS.
*/

/* ---- Q+E ODBC SQL object --------------------------------------------- */
long ALMAPI OQsqlAdministrationRoutine(AOBJECTID, pAObjMessage);
/* Q+E ODBC SQL object's administration routines. */

BOOL ALMCALLBACK OQsqlObjectDialog(HWND, UINT, WPARAM, LPARAM);
/* Q+E ODBC SQL object dialog. */

BOOL ALMCALLBACK OQsqlVarsDialog(HWND, UINT, WPARAM, LPARAM);
/* Q+E ODBC SQL variables dialog. */

BOOL ALMCALLBACK OQsqlOptionsDialog(HWND, UINT, WPARAM, LPARAM);
/* Q+E ODBC SQL Options dialog. */

void ALMAPI OQsqlStartupMeth(pAEvtInfo, pAObjMessage);
/* Q+E ODBC SQL objects's constructor. */

void ALMAPI OQsqlShutdownMeth(pAEvtInfo, pAObjMessage);
/* Q+E ODBC SQL objects's destructor. */

/* ---- The functions ---- */
void ALMAPI OQsqlAppendToTableFunc(pAEvtInfo, pAObjMessage);
/* Retrieves rows of SELECT results, and appends them into the specified
   Table object.
*/

void ALMAPI OQsqlDescribeTableFunc(pAEvtInfo, pAObjMessage);
/* Returns a list of column names, a list of the associated column types,
   and a list of the mapped variable types from a given database table.
*/

void ALMAPI OQsqlExecuteFunc(pAEvtInfo, pAObjMessage);
/* Executes the SQL statement associated with the Q+E ODBC SQL object. */

void ALMAPI OQsqlFetchFunc(pAEvtInfo, pAObjMessage);
/* Retrieves rows of SELECT results, and places them into the objects listed
   in the sql statement.
*/

void ALMAPI OQsqlFetchToTableFunc(pAEvtInfo, pAObjMessage);
/* Retrieves rows of SELECT results, and places them into the specified
   Table object.
*/

void ALMAPI OQsqlFetchToTextFunc(pAEvtInfo, pAObjMessage);
/* Retrieves rows of SELECT results, and places them into a Text object. */

void ALMAPI OQsqlFlushFunc(pAEvtInfo, pAObjMessage);
/* Flushes results used by a SQL statement. */

void ALMAPI OQsqlGetErrorFunc(pAEvtInfo, pAObjMessage);
/* Returns the most recent Q+E ODBC error code, and the associated error
   message.
*/

void ALMAPI OQsqlGetTablesFunc(pAEvtInfo, pAObjMessage);
/* Returns a list of table names from a given Q+E ODBC SQL object. */

void ALMAPI OQsqlGetVariablesFunc(pAEvtInfo, pAObjMessage);
/* Returns a list of variable names, and a list of the associated variable
   types from a given Q+E ODBC SQL object.
*/

/* ---- The methods ---- */
void ALMAPI OQsqlDViewChangedMeth(pAEvtInfo, pAObjMessage);
/* Releases column description created by ChangeDView. */

void ALMAPI OQsqlDViewGetDataMeth(pAEvtInfo, pAObjMessage);
/* Returns an array of columns memory block.  Each column memory block is
   '\0'-delimited column data.
*/

void ALMAPI OQsqlDViewGetRowCountMeth(pAEvtInfo, pAObjMessage);
/* Returns the number of rows. */

void ALMAPI OQsqlDViewSetMeth(pAEvtInfo, pAObjMessage);
/* Sets auto query on. */

#ifdef __cplusplus
} // extern "C" 
#endif // __cplusplus 

#endif // QSQL_H 
