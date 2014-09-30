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
 | FILE		: SQLDPC.H                                                |
 | DESCRIPTION	: SequeLink SQL Statement Precompiler Include File.       |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#ifndef SQLDPC_H
#define SQLDPC_H

#include "sqld_fnc.h"
#include "strutl.h"


#define BINDVAR		1		// bind variable
#define SELECTVAR	2		// select variable

#define COLBLOCK	8		// column block
#define STMTBLOCK	128		// statement block

class SqldPC : protected StrUtl	/* ---- SequeLink SQL Precompiler ------- */
{ SqldCore	*sqldCore;		// sqld core info
  int		varCount;		// number of variables
  Var		*var;			// an array of variables

  int		selectMBSize;		// select memory block size
  AMEMBLOCKID	selectMB;		// select memory block
  int		selColCount;		// number of select columns
  SqldCol	*selColPtr;	// select column pointer

  int		bindMBSize;		// bind memory block size
  AMEMBLOCKID	bindMB;			// bind memory block
  int		bindColCount;		// number of bind columns
  SqldCol	*bindColPtr;	// bind column pointer

  int		sqldStmtMBSize;		// sqld statement memory block size
  AMEMBLOCKID	sqldStmtMB;		// sqld statement memory block
  int		sqldStmtLen;		// sqld statement length
  char		*sqldStmtPtr;		// sqld statement pointer

  char		*stmtPtr;		// statement pointer
  int		mssgLen;		// message length
  char		*mssgPtr;		// message pointer

  int		varKind;		// variable kind

  void Error(char *errMssg, char *addMssg);
  /* Writes error message, and additional message to message. */

  BOOL ExpandBindCol(int);
  /* Expands bind column. */

  BOOL ExpandSelCol(int);
  /* Expands select column. */

  BOOL ExpandStmt(int);
  /* Expands statement. */


  BOOL Object(void);
  /* OBJECT	: 'into' space+ 'object' space+
		| 'into' space+ 'objects' space+
		| 'using' space+ 'object' space+
		| 'using' space+ 'objects' space+
  */

  BOOL Variable(void);
  /* VARIABLE	: ':' variable_name
  */

  BOOL Variables(void);
  /* VARIABLES	: VARIABLE ',' VARIABLES
		| VARIABLE ','  	
		| VARIABLE
  */

public:
  BOOL Precompile(AOBJECTID sqldObj, SqldCursor *cursor, char *stmt,
		  char *message, unsigned messageSize,
		  AMEMBLOCKID sqlStmtMB);
  /* Returns true or false, as the result of precompiling the statement. */
}; 

#endif /* SQLDPC_H */
