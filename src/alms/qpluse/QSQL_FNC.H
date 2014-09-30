/*                                                                        *\
    (c) 1993-1995, Novell, Inc.  All rights reserved.

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
 | FILE		: QSQL_FNC.H                                              |
 | DESCRIPTION	: Q+E ODBC SQL Object Functions Include File.		  |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Add Database View.                   |
 |		  mw	08-Jun-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#ifndef QSQL_FNC_H
#define QSQL_FNC_H

#include "qsql.h"
#include "sqllib.h"
#include "op_dview.h"


typedef struct qsqlcore		QsqlCore;
struct qsqlcore			/* ---- ODBC SQL Core info -------------- */
{ int		version;		// version number 

  AOBJECTID	connectObj;		// connection: qcnc object
  AOBJECTID	sqlObj;			// sql statement: text object
  long		rowCount;		// number of rows 
  int		colSprtr;		// column separator 
  BOOL		isOEM;			// is oem character sets? 

  BOOL		isDView;		// is database view?
  void		*objList;		// object list
  long		recCount;		// number of records

  /* ---- run time values ---- */
  qeSTATUS	errorCode;		// error code
  char		*errorMsg;		// error message
  BOOL		isCursor;		// is cursor?
  qeHANDLE      cursor;			// cursor
  BOOL		isSelectMB;		// is select columns memory block
  AMEMBLOCKID	selectMB;		// select columns memory block
  BOOL		isDataBuff;		// is data buffer?
  long		dataBuffRowCount;	// rows can be hold by data buffer
};

/* Var */			/* ---- ODBC SQL Variable --------------- */

typedef struct col		Col;
struct col			/* ---- ODBC SQL Column ----------------- */
{ AOBJECTID	varObj;			// variable's link object
  ATYPEID	varType;		// variable's link object type

  char		*name;			// column name
  int		type;			// data type
  long		size;			// data size
  long		bindDataSize;		// bind data size
  char		*bindData;		// bind data
  HANDLE	dataSizeHdl;		// data size memory handle
  HANDLE	dataHdl;		// data memory handle
};


class QSQL_FNC			/* ---- ODBC SQL Function --------------- */
{
public:
  BOOL ChangeDView(AOBJECTID, QsqlCore *, ODViewColDesc);
  /* Changes database view. */


  BOOL AllocDataBuff(QsqlCore *, long, int, Col *);
  /* Allocates data buffers. */

  void DisposeDataBuff(QsqlCore *, int, Col *);
  /* Disposes data buffers. */

  void Dispose(QsqlCore *);
  /* Disposes column memory blocks. */

  long Fetch(QsqlCore *, long, long = 0);
  /* Returns the number of rows fetched into the data buffers.
     Returns -1, if an error has occured.
  */

  BOOL FetchToColData(QsqlCore *, long, AMEMBLOCKID);
  /* Fetches rows to the column data. */

  BOOL FetchToObject(QsqlCore *, long);
  /* Fetches rows to the objects. */

  BOOL AppendToTable(QsqlCore *, long, AOBJECTID);
  /* Appends rows to the table object. */

  BOOL FetchToTable(QsqlCore *, long, AOBJECTID);
  /* Fetches rows to the table object. */

  BOOL FetchToText(QsqlCore *, long, AOBJECTID);
  /* Fetches rows to the text object. */

  BOOL GetHDBC(AOBJECTID, qeHANDLE *);
  /* Returns true or false as the result of getting the hdbc. */

  BOOL RecordError(QsqlCore *, qeSTATUS);
  /* Returns true or false as the result of recording error. */
};

#endif // QSQL_FNC_H 
