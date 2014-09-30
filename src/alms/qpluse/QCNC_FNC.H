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
 | FILE		: QCNC_FNC.H                                              |
 | DESCRIPTION	: Q+E ODBC Connection Functions Include File.	          |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Add Database View.	                  |
\*------------------------------------------------------------------------*/
#ifndef QCNC_FNC_H
#define QCNC_FNC_H

#include "qsql.h"
#include "sqllib.h"


typedef struct qcnccore		QcncCore;
struct qcnccore			/* ---- ODBC Connection Core ------------ */
{ int		version;		// version number 

  int		dbmsID;			// dbms id 
  AOBJECTID	usernameObj;		// username: text object 
  AOBJECTID	passwordObj;		// password: text object 
  AOBJECTID	paramsObj;		// parameters: text object 
  BOOL		isAutoCommit;		// is auto commit? 
  BOOL		isDView;		// is database view? 

  /* ---- run time values ---- */
  qeSTATUS	errorCode;		// error code
  char		*errorMsg;		// error message 
  BOOL		isConnected;		// is connected?
  qeHANDLE	hdbc;			// database connection handle
};


class QCNC_FNC			/* ---- ODBC Connection Function -------- */
{
public:
  void CloseCursor(AOBJECTID qcncObj);
  /* Closes the associated cursors. */

  BOOL RecordError(QcncCore *qcncCore, qeSTATUS errorCode);
  /* Returns true or false as the result of recording error. */
};

#endif // QCNC_FNC_H 
