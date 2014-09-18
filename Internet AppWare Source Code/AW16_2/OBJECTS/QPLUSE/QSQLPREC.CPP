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
#include "qsqlprec.h"
#include "safemem.h"


BOOL QSqlPrec::Precompile	/* ---- Precompile ---------------------- */
( OBJECTID	qsqlObj,		// qsql object
  char		*sqlStmt,		// sql statement
  char		*message,		// message
  WORD		messageSize,		// message size
  MEMBLOCKID	qeSqlStmtMB		// q+e sql statement memory block
)
/* Returns TRUE, or FALSE, as the result of precompiling the sql statement.
 */
{ MemBlock	mb;			// memblock class
  BOOL		isStmt;			// is statement?

  /* ---- inits private variables ---- */
  qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE);
  varCount = AObjGetDataSize(qsqlObj, QSQLVARS) / sizeof(Var);
  var = (Var *) mb.GetPointer(qsqlObj, QSQLVARS);

  selectMBSize = 0;
  selectMB = 0;
  selColCount = 0;
  selColPtr = 0;

  qeStmtMBSize =0;
  qeStmtMB = qeSqlStmtMB;
  qeStmtLen = 0;
  qeStmtPtr = 0;

  stmtPtr = sqlStmt;
  mssgLen = messageSize - 1;
  mssgPtr = message;

  isStmt = FALSE;
  do
  { /* ---- bind variable(s) ---- */
    if (*stmtPtr == ':')
    { if (!isStmt)
      { Error("A statement is expected", stmtPtr);
	return FALSE;
      } // if
      varKind = BINDVAR;		// bind variables
      if (!Variables())
	return FALSE;
      isStmt = FALSE;
    } // if

    /* ---- select variable(s) ----- */
    if (Object())
    { varKind = SELECTVAR;		// select variables
      if (!Variables())
	return FALSE;
      isStmt = FALSE;
    } // if

    /* ---- statement ---- */
    if (!isStmt && !IsWhiteSpace(*stmtPtr))
      isStmt = TRUE;

    ExpandStmt(1);
    *qeStmtPtr++ = IsWhiteSpace(*stmtPtr)? ' ' : *stmtPtr;
    qeStmtLen++;
  } while (*stmtPtr++ != '\0');

  /* ---- selectMB ---- */
  if (selectMB)
  { AMemUnlock(selectMB);
    MemResize(selectMB, selColCount * sizeof(SelCol));

    qsqlCore->isSelectMB = TRUE;
    qsqlCore->selectMB = selectMB;
  } // if

  /* ---- qeSqlStmtMB ---- */
  if (qeStmtLen == 1)
  { qeStmtPtr--;
    *qeStmtPtr++ = ' ';			// makes it executable
    *qeStmtPtr = '\0';
  } // if
  AMemUnlock(qeSqlStmtMB);
  return TRUE;
} // Precompile


void QSqlPrec::Error		/* ---- Error --------------------------- */
( char		*errMssg,		// error message
  char		*addMssg		// additional message
)
/* Writes error message, and additional message to message. */
{ WORD		errMssgLen;		// error message length
  WORD		addMssgLen;		// additional message length

  WORD		len;			// length

  /* ---- error message ---- */
  *mssgPtr = '\0';
  lstrcat(mssgPtr, "Q+E SQL\r\nExecute: ");
  lstrcat(mssgPtr, errMssg);
  lstrcat(mssgPtr, " at ");

  /* ---- additional message ---- */
  while (IsWhiteSpace(*addMssg))	// ignores white space
    addMssg++;
  if (*addMssg == 0)
  { /* ---- no additional message ----- */
    lstrcat(mssgPtr, "the end of the statement.");
    return;
  } // if

  /* ---- additional message ---- */
  errMssgLen = lstrlen(mssgPtr);
  mssgPtr += errMssgLen;
  addMssgLen = lstrlen(addMssg);

  len = addMssgLen > mssgLen - (errMssgLen + 2)?	
          mssgLen - (errMssgLen + 2) : addMssgLen;

  *mssgPtr++ = '"';			// '"'
  hmemcpy(mssgPtr, addMssg, len);
  mssgPtr += len;
  *mssgPtr++ = '"';			// '"'
  *mssgPtr = '\0';			// 0 terminated string

  if (selectMB)
  { AMemUnlock(selectMB);
    AMemFree(selectMB);
  } // if
  return;
} // Error

BOOL QSqlPrec::ExpandCol	/* ---- Expand Column ------------------- */
( WORD		expCount		// expand count
)
/* Expands column. */
{
  if (selColCount + expCount < selectMBSize)
    return TRUE;

  while (selColCount + expCount > selectMBSize)
    selectMBSize += SELCOLBLOCK;

  if (selColCount)
  { AMemUnlock(selectMB);
    MemResize(selectMB, selectMBSize * sizeof(SelCol));
    if (!(selColPtr = (SelCol *) AMemLock(selectMB) + selColCount))
      return FALSE;
  } //
  else
  { selectMB = MemAllocate(selectMBSize * sizeof(SelCol));
    if (!(selColPtr = (SelCol *) AMemLock(selectMB)))
      return FALSE;
  } // else
  return TRUE;
} // ExpandCol

BOOL QSqlPrec::ExpandStmt	/* ---- Expand Statement ---------------- */
( WORD		expLen			// expand length
)
/* Expands statement. */
{
  if (qeStmtLen + expLen < qeStmtMBSize)
    return TRUE;

  while (qeStmtLen + expLen > qeStmtMBSize)
    qeStmtMBSize += QESTMTBLOCK;

  if (qeStmtLen)
  { AMemUnlock(qeStmtMB);
    MemResize(qeStmtMB, qeStmtMBSize);
    if (!(qeStmtPtr = (char *) AMemLock(qeStmtMB) + qeStmtLen))
      return FALSE;
  } // if
  else
  { MemResize(qeStmtMB, qeStmtMBSize);
    if (!(qeStmtPtr = (char *) AMemLock(qeStmtMB)))
      return FALSE;
  } // else
  return TRUE;
} // ExpandStmt

BOOL QSqlPrec::Object		/* ---- Object -------------------------- */
( void
)
/* OBJECT	: space+ 'into' space+ 'object' space+
		| space+ 'into' space+ 'objects' space+
		| space+ 'using' space+ 'object' space+
		| space+ 'using' space+ 'objects' space+
*/
{ char		*p = stmtPtr;		// pointer

  while (IsWhiteSpace(*p))
    p++;

  if (_fmemicmp(p, "into", 4) == 0)	// 'into'
    p += 4;
  else					// or
    if (_fmemicmp(p, "using", 5) == 0)  // 'using'
      p += 5;
    else
      return FALSE;

  if (!IsWhiteSpace(*p))		// at lease one space
    return FALSE;
  while (IsWhiteSpace(*p))
    p++;

  if (_fmemicmp(p, "object", 6))	// 'object'
    return FALSE;
  p += 6;
  if (*p == 's' || *p == 'S')		// 's'
    p++;

  if (!IsWhiteSpace(*p))		// at lease one space
    return FALSE;
  while (IsWhiteSpace(*p))
    p++;

  stmtPtr = p;				// accepts
  ExpandStmt(1);
  *qeStmtPtr++ = ' ';
  qeStmtLen++;
  return TRUE;
} // Object

BOOL QSqlPrec::Variable		/* ---- Variable ------------------------ */
( void
)
/* VARIABLE	: ':' space* variable_name
 */
{ char		*p = stmtPtr;		// pointer
  char		name[MXVARSIZE];	// name
  Var		huge *varPtr;		// VAR pointer

  char		*str;			// string
  WORD		strLen;			// string length

  WORD		i;			// counter

  if (*p != ':')			// colon
  { Error(":variable is expected", stmtPtr);
    return FALSE;
  } // if
  p++;

  while (IsWhiteSpace(*p))		// space(s)
    p++;

  /* ---- variable name ---- */
  for (i = 0; i < MXVARSIZE - 1 && *p && IsVarChar(*p); i++, p++)
    name[i] = *p;
  name[i] = 0;
  if (*p && IsVarChar(*p))
  { Error("Can't bind variable", stmtPtr);
    return FALSE;
  } // if
  for (i = 0, varPtr =var; i < varCount; i++, varPtr++)
    if (lstrcmpi(varPtr->name, name) == 0)
      break; // for
  if (i == varCount)
  { Error("Can't bind variable", stmtPtr);
    return FALSE;
  } // if

  stmtPtr = p;				// accepts

  if (varKind == SELECTVAR)		// select variable
  { ExpandCol(1);
    selColPtr->varObj = varPtr->obj;
    selColPtr->varType = varPtr->type;

    selColPtr->nameMB = 0;
    selColPtr->bindDataMB = 0;
    selColPtr->dataSizeMB = 0;
    selColPtr->dataMB = 0;
    selColPtr++;
    selColCount++;
    return TRUE;
  } // if

  switch ((long) varPtr->type)		// bind variable
  { case OTYPE_TEXT:
      str = dataCon.TextToQEStr(varPtr->obj);
      strLen = lstrlen(str);
      ExpandStmt(strLen);
      if (qsqlCore->isOEM)
	AnsiToOem(str, qeStmtPtr);
      else
	hmemcpy(qeStmtPtr, str, strLen);
      qeStmtPtr += strLen;
      qeStmtLen += strLen;
      break;
    case OTYPE_NUMBER:
      str = dataCon.NumberToQEStr(varPtr->obj);
      strLen = lstrlen(str);
      ExpandStmt(strLen);
      hmemcpy(qeStmtPtr, str, strLen);
      qeStmtPtr += strLen;
      qeStmtLen += strLen;
      break;
    case OTYPE_DATE:
      str = dataCon.DateToQEStr(varPtr->obj);
      strLen = lstrlen(str);
      ExpandStmt(strLen);
      hmemcpy(qeStmtPtr, str, strLen);
      qeStmtPtr += strLen;
      qeStmtLen += strLen;
      break;
    case OTYPE_TIME:
      str = dataCon.TimeToQEStr(varPtr->obj);
      strLen = lstrlen(str);
      ExpandStmt(strLen);
      hmemcpy(qeStmtPtr, str, strLen);
      qeStmtPtr += strLen;
      qeStmtLen += strLen;
      break;
  } // switch
  return TRUE;
} // Variable

BOOL QSqlPrec::Variables	/* ---- Variables ----------------------- */
( void
)
/* VARIABLES	: VARIABLE space* ',' space* VARIABLES
 *		| VARIABLE
 */
{
  do
  { if (!Variable())			// variable
      return FALSE;

    while (IsWhiteSpace(*stmtPtr))	// space(s)
      *stmtPtr++;

    if (*stmtPtr != ',')		// comma
      break; /* do while */

    *stmtPtr++;
    if (varKind == BINDVAR)		// bind variable
    { ExpandStmt(1);
      *qeStmtPtr++ = ',';
      qeStmtLen++;
    } // if

    while (IsWhiteSpace(*stmtPtr))	// space(s)
      *stmtPtr++;
  } while (*stmtPtr == ':'); /* more variables */
  ExpandStmt(1);
  *qeStmtPtr++ = ' ';
  qeStmtLen++;
  return TRUE;
} // Variables
