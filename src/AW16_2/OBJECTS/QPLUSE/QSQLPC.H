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
 | FILE		: QSQLPREC.H                                              |
 | DESCRIPTION	: SQL Statement Precompiler Include File.	          |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef QSQLPREC_H
#define QSQLPREC_H

#include "qsql_fnc.h"
#include "strutl.h"
#include "qsqldc.h"
#include "memblock.h"


#define BINDVAR		1		// bind variable
#define SELECTVAR	2		// select variable

#define SELCOLBLOCK	8		// select column block
#define QESTMTBLOCK	128		// q+e statement block

class QSqlPrec : protected StrUtl/* --- QSql Precompiler ---------------- */
{ MemBlock	mb;			// memblock class
  QsqlCore	*qsqlCore;		// qsql core info
  WORD		varCount;		// number of VARs
  Var		*var;			// an array of VARs

  WORD		selectMBSize;		// select memory block size
  MEMBLOCKID	selectMB;		// select memory block
  WORD		selColCount;		// number of SELECTCOLs
  Col		huge *selColPtr;	// SELCOL pointer

  WORD		qeStmtMBSize;		// q+e statement memory block size
  MEMBLOCKID	qeStmtMB;		// q+e statement memory block
  WORD		qeStmtLen;		// q+e statement length 
  char		*qeStmtPtr;		// q+e statement pointer

  char		*stmtPtr;		// statement pointer
  WORD		mssgLen;		// message length
  char		*mssgPtr;		// message pointer

  BOOL		varKind;		// variable kind
  QSqlDatC	dataCon;		// qsqldatc class

  void Error(char *, char *);
  /* Writes error message, and additional message to message. */

  BOOL ExpandStmt(WORD);
  /* Expands statement. */

  BOOL ExpandCol(WORD);
  /* Expands column. */

  BOOL Object(void);
  /* OBJECT	: space+ 'into' space+ 'object' space+
   *		| space+ 'into' space+ 'objects' space+
   *		| space+ 'using' space+ 'object' space+
   *		| space+ 'using' space+ 'objects' space+
   */

  BOOL Variable(void);
  /* VARIALBE	: ':' variable_name SPACE
   */

  BOOL Variables(void);
  /* VARIABLES	: VARIABLE ',' VARIABLES
   *		| VARIABLE
   */

public:
  BOOL Precompile(OBJECTID, char *, char *, WORD, MEMBLOCKID);
  /* Returns TRUE or FALSE, as the result of precompiling the sql statement.
   */
};

#endif /* QSQLPREC_H */
