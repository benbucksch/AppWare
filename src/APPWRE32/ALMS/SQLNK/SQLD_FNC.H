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
 | FILE		: SQLD_FNC.H                                              |
 | DESCRIPTION	: SequeLink SQL Functions Include File.	                  |
 | COPYRIGHT	: Novell, Inc.	                                          |  
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#ifndef SQLD_FNC_H
#define SQLD_FNC_H

#include "sqld.h"
#include "sqlnklib.h"


/* ---- miscellaneus values ---- */
#define MXPARAMCOUNT	7		// number of parameters

/* ---- SequeLink SQL Object extended datablocks ------------------------ */
#define SQLDCOUNTEXTEND	(SQLDCOUNT + 1)	// # of extended sqld datablocks
#define SQLDCURSORS	SQLDCOUNT	// sqld cursors

typedef struct sqldcore		SqldCore;
struct sqldcore			/* ---- SequeLink SQL Core Info --------- */
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

  /* ---- run-time values ---- */
  SSPErr	clientCode;		// client error code
  SSPSrvRtn	serverCode;		// server error code
  SSPErr	reasonCode;		// reason error code

  BOOL		isServerRef;		// is server reference number?
  SSPRefNum	serverRef;		// server reference number
  BOOL		isLogonRef;		// is logon reference number?
  SSPRefNum	logonRef;		// logon reference number
  int		dbOverride;		// db override type passed to logon
};

// Variables			/* ---- SequeLink SQL Variables --------- */

typedef struct sqldcursor	SqldCursor;
struct sqldcursor		/* ---- SequeLink SQL Cursor ------------ */
{ SSPRefNum	cursorRef;		// cursor reference number

  BOOL		isBindMB;		// is bind memory block?
  AMEMBLOCKID	bindMB;			// bind columns memory block

  BOOL		isSelectMB;		// is select memory block?
  AMEMBLOCKID	selectMB;		// select columns memory block
};

typedef struct sqldcol		SqldCol;
struct sqldcol			/* ---- SequeLink SQL Column ------------ */
{ AOBJECTID	obj;			// variable's link object
  ATYPEID	objType;		// object type

  AMEMBLOCKID	nameMB;			// column name memory block
  int		type;			// column type
  long		size;			// column size
};


class SQLD_FNC			/* ---- SequeLink SQL Function ---------- */
{
public:
  BOOL Cancel(SqldCore *, SSPRefNum);
  /* Returns true or false as the result of cancelling the cursor. */

  BOOL Close(SqldCore *, SSPRefNum);
  /* Returns true or false as the result of closing the cursor. */

  BOOL Commit(SqldCore *);
  /* Returns true or false as the result of committing the changes. */

  BOOL Disconnect(AOBJECTID, SqldCore *);
  /* Returns true or false as the result of disconnecting the database. */

  BOOL Logoff(AOBJECTID, SqldCore *);
  /* Returns true or false as the result of Logoff from the database. */

  BOOL Rollback(SqldCore *);
  /* Returns true or false as the result of rolling back the changes. */


  void DisposeBindBuff(SqldCursor *);
  /* Disposes bind buffers. */

  void DisposeSelectBuff(SqldCursor *);
  /* Disposes select buffers. */

  void Dispose(SqldCursor *);
  /* Disposes all memory blocks. */


  BOOL DefineBind(SqldCore *, SqldCursor);
  /* Defines bind columns buffers. */

  BOOL DefineSelect(SqldCore *, SqldCursor);
  /* Defines select columns buffers. */


  BOOL BindObjects(SqldCore *, SqldCursor *);
  /* Binds the objects. */

  BOOL FetchToObjects(SqldCore *, SqldCursor, long);
  /* Fetches rows to the objects. */

  BOOL FetchToTable(SqldCore *, SqldCursor, long, AOBJECTID);
  /* Fetches rows to the table object. */

  BOOL FetchToText(SqldCore *, SqldCursor, long, AOBJECTID);
  /* Fetches rows to the text object. */


  void DisposeCursors(AOBJECTID, SqldCore *);
  /* Disposes cursors. */

  SqldCursor *GetCursor(int, SqldCursor *, SSPRefNum);
  /* Returns pointer to the cursor. */
};

#endif // SQLD_FNC_H
