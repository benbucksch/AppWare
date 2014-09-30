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
 | FILE		: QCNC_FNC.CPP                                            |
 | DESCRIPTION	: Q+E ODBC Connection Object's Functions.	          |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Add Database View.		          |
 |		  mw	08-Jun-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#include "qcnc_fnc.h"
#include "qsql_fnc.h"
#include "memblock.h"
#include "safemem.h"


void QCNC_FNC::CloseCursor	/* ---- Close Cursor -------------------- */
( AOBJECTID	qcncObj 		// qcnc object
)
/* Closes the associated SQL cursors. */
{ MemBlock	mb;			// memblock class
  AOBJECTID	qsqlObj;		// qsql object
  QsqlCore	*qsqlCore;		// qsql core info

  QSQL_FNC	fnc;			// qsql_fnc class
  long		objIdx;         	// object index
  qeSTATUS	errorCode;		// error code

  objIdx = AOBJ_GETFIRSTKEY;
  while((qsqlObj = AObjGetNextObject(OTYPE_QSQL, &objIdx)) != 0)
  { qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE);
    if (qsqlCore && qsqlCore->connectObj == qcncObj && qsqlCore->isCursor)
    { errorCode = (*qe_EndSQL)(qsqlCore->cursor);
      if (fnc.RecordError(qsqlCore, errorCode))
	AEvtPostSignalAtMark(qsqlObj, QSQLIFERROR);
      else
      { qsqlCore->isCursor = FALSE;
	fnc.Dispose(qsqlCore);
      } // else
    } // if
  } // while
} // CloseCursor

BOOL QCNC_FNC::RecordError	/* ---- Record Error -------------------- */
( QcncCore	*qcncCore,		// qcnc core info
  qeSTATUS	errorCode		// error code
)
/* Returns true or false as the result of recording error. */
{
  if (errorCode == qeSUCCESS)
  { /* ---- no error ---- */
    if (qcncCore->errorCode != qeSUCCESS)
    { /* ---- erases the previous error message ---- */
      qcncCore->errorCode = qeSUCCESS;
      MBFree(qcncCore->errorMsg);
    } // if
    return FALSE;
  } // if

  if (qcncCore->errorCode == qeSUCCESS)
    /* ---- creates buffer for the current error message ---- */
    qcncCore->errorMsg = (char *) MBAlloc(qeMAX_ERR_MSG_LEN);
  (*qe_ErrMsgBuf)(qcncCore->errorMsg);
  qcncCore->errorCode = errorCode;
  return TRUE;
} // RecordError


/*------------------------------------------------------------------------*\
 | METHOD       : OQcncStartupMeth                                        |
 | DESCRIPTION  : ODBC Connection object's constructor.                   |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncStartupMeth
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  long          objIdx;         	// object index
  AOBJECTID      qcncObj;		// qcnc object
  QcncCore      *qcncCore;		// qcnc core info

  objIdx = AOBJ_GETFIRSTKEY;
  while((qcncObj = AObjGetNextObject(OTYPE_QCNC, &objIdx)) != 0)
  { /* ---- inits resources ---- */
    MemResizeObjectData(qcncObj, QCNCCORE, sizeof(QcncCore));
    qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE);
    qcncCore->errorCode = qeSUCCESS;
    qcncCore->isConnected = FALSE;
  } // while

  if (!sqlLib.LoadLib())
    lstrcpy(theSystem->errorMessage,
	    "ODBC Connection: Couldn't find the Q+E ODBC DLL.");
  else
    (*qe_LibInit)();
} // OQcncStartupMeth

/*------------------------------------------------------------------------*\
 | METHOD       : OQcncShutdownMeth                                       |
 | DESCRIPTION  : ODBC Connection object's destructor.                    |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncShutdownMeth
( pAEvtInfo,				// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock	mb;			// memblock class
  long          objIdx;         	// object index
  AOBJECTID      qcncObj;		// qcnc object
  QcncCore      *qcncCore;      	// qcnc core info

  QCNC_FNC	fnc;			// qcnc_fnc class
  qeSTATUS	errorCode;		// error code		

  objIdx = AOBJ_GETFIRSTKEY;
  while((qcncObj = AObjGetNextObject(OTYPE_QCNC, &objIdx)) != 0)
  { /* ---- releases resources ---- */
    qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE);
    if (qcncCore->isConnected)
    { fnc.CloseCursor(qcncObj);		// closes the qsql cursors

      errorCode = (*qe_Disconnect)(qcncCore->hdbc);
      if (fnc.RecordError(qcncCore, errorCode))
	AEvtPostSignalAtMark(qcncObj, QCNCIFERROR);
      else
        qcncCore->isConnected = FALSE;
    } // if
    if (qcncCore->errorCode != qeSUCCESS)
    { qcncCore->errorCode = qeSUCCESS;
      MBFree(qcncCore->errorMsg);
    } // if
  } // while

  if (sqlLib.IsLoadLib())
    (*qe_LibTerm)();
} // OQcncShutdownMeth

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQcncCommitFunc                                         |
 | DESCRIPTION  : Commits the current transaction.  The current           |
 |		  transaction is the set of the SQL statements executed   |
 |		  since the last successful call to either the Connect,   |
 |		  Commit, or Rollback functions.                          |
 | INPUT 1      : ODBC Connection: ODBC CONNECTION -- The ODBC Connection |
 |                object to commit the changes for.                       |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : Commited.                                               |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncCommitFunc
( pAEvtInfo,				// event pointer
  pAObjMessage  theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qcncObj;        	// qcnc object                (I: 1)
  QcncCore      *qcncCore;      	// qcnc core info

  QCNC_FNC	fnc;			// qcnc_fnc class
  qeSTATUS	errorCode;		// error code

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
	"Commit: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qcncObj = AFuncGetTypedParameter(1, OTYPE_QCNC);
  if ((qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Commit: Couldn't get the ODBC Connection input parameter.");
    return;
  } // if

  if (!qcncCore->isConnected)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC Connection\r\n"
      "Commit: The database wasn't connected.");
    return;
  } // if

  if (!qcncCore->isAutoCommit)
  { errorCode = (*qe_Commit)(qcncCore->hdbc);
    if (fnc.RecordError(qcncCore, errorCode))
    { AEvtPostSignalAtTail(qcncObj, QCNCIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if

    errorCode = (*qe_BeginTran)(qcncCore->hdbc);
    if (fnc.RecordError(qcncCore, errorCode))
      AEvtPostSignalAtMark(qcncObj, QCNCIFERROR);
  } // if

  /* ---- FLOW 1 (Commited) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQcncCommitFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQcncConnectFunc                                        |
 | DESCRIPTION  : Establishes a connection between the Q+E Connection     |
 |		  object, and the DBMS.                                   |
 | INPUT 1      : ODBC Connection: ODBC CONNECTION -- The ODBC Connection |
 |                object to use to make connection.                       |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : Connected.                                              |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncConnectFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qcncObj;        	// ocnc object                (I: 1)
  QcncCore      *qcncCore;      	// ocnc core info
  int		usernameLen;		// username length
  int		passwordLen;		// password length
  int		paramsLen;		// parameters length

  QCNC_FNC	fnc;			// qcnc_fnc class
  int		cncStrLen;		// connect string length
  char		*cncStr;		// connection string
  char		*cncStr2;		// connection string 2
  qeSTATUS	errorCode;		// error code

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
	"Connect: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qcncObj = AFuncGetTypedParameter(1, OTYPE_QCNC);
  if ((qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Connect: Couldn't get the ODBC Connection input parameter.");
    return;
  } // if

  /* ---- calculates connection string size ---- */
  cncStrLen = 1;			// 0 terminated string

  /* ---- username ---- */
  if (qcncCore->usernameObj)
  { usernameLen = OTextGetLength(qcncCore->usernameObj);
    cncStrLen += 4 + usernameLen;	// "UID="
  } // if

  /* ---- password ---- */
  if (qcncCore->passwordObj)
  { passwordLen = OTextGetLength(qcncCore->passwordObj);
    if (qcncCore->usernameObj)
      cncStrLen += 5 + passwordLen;	// ";PWD="
    else
      cncStrLen += 4 + passwordLen;	// "PWD="
  } // if

  /* ---- other params ---- */
  if (qcncCore->paramsObj)
  { paramsLen = OTextGetLength(qcncCore->paramsObj);
    if (qcncCore->usernameObj || qcncCore->passwordObj)
      cncStrLen += 1 + paramsLen;	// ";"
    else
      cncStrLen += paramsLen;
  } // if

  /* ---- writes connection string ---- */
  cncStr = cncStr2 = (char *) mb.Alloc(cncStrLen);

  /* ---- username ---- */
  if (qcncCore->usernameObj)
  { CopyMemory(cncStr2, "UID=", 4);
    cncStr2 += 4;
    OTextGetToBuffer(qcncCore->usernameObj, cncStr2, usernameLen + 1);
    cncStr2 += usernameLen;
  } // if

  /* ---- password ---- */
  if (qcncCore->passwordObj)
  { if (qcncCore->usernameObj)
    { CopyMemory(cncStr2, ";PWD=", 5);
      cncStr2 += 5;
    } // if
    else
    { CopyMemory(cncStr2, "PWD=", 4);
      cncStr2 += 4;
    } // else
    OTextGetToBuffer(qcncCore->passwordObj, cncStr2, passwordLen + 1);
    cncStr2 += passwordLen;
  } // if

  /* ---- other params ---- */
  if (qcncCore->paramsObj)
  { if (qcncCore->usernameObj || qcncCore->passwordObj)
    { CopyMemory(cncStr2, ";", 1);
      cncStr2 += 1;
    } // if
    OTextGetToBuffer(qcncCore->paramsObj, cncStr2, paramsLen + 1);
    cncStr2 += paramsLen;
  } // if

  *cncStr2 = '\0';			// 0 terminated string

  if (qcncCore->isConnected)
  { /* ---- closes cursors ---- */
    fnc.CloseCursor(qcncObj);		// closes the sql cursors

    errorCode = (*qe_Disconnect)(qcncCore->hdbc);
    if (fnc.RecordError(qcncCore, errorCode))
    { AEvtPostSignalAtTail(qcncObj, QCNCIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if
    qcncCore->isConnected = FALSE;
  } // if

  if (!(qcncCore->hdbc = (*qe_Connect)(cncStr)))
  { errorCode = (*qe_Err)();
    if (fnc.RecordError(qcncCore, errorCode))
      AEvtPostSignalAtTail(qcncObj, QCNCIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, ""); 
    return;
  } // if
  qcncCore->isConnected = TRUE;

  if (!qcncCore->isAutoCommit)		// not auto commit
  { errorCode = (*qe_BeginTran)(qcncCore->hdbc);
    if (fnc.RecordError(qcncCore, errorCode))
      AEvtPostSignalAtMark(qcncObj, QCNCIFERROR);
  } // if

  if (qcncCore->isDView)		// is database view
  { errorCode = (*qe_SetSelectOptions)(qcncCore->hdbc, qeFETCH_ANY_DIR);
    if (fnc.RecordError(qcncCore, errorCode))
      AEvtPostSignalAtMark(qcncObj, QCNCIFERROR);
  } // if

  /* ---- FLOW 1 (Connected) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQcncConnectFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQcncDisconnectFunc                                     |
 | DESCRIPTION  : Breaks the connection between the ODBC Connection       |
 |		  object, and the DBMS.                                   |
 | INPUT 1      : ODBC Connection: ODBC CONNECTION -- The ODBC Connection |
 |                Connection object to use to disconnect.                 |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : Disconnected.                                           |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., November 1, 1993.                                     mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncDisconnectFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qcncObj;        	// qcnc object		      (I: 1)
  QcncCore      *qcncCore;      	// qcnc core info

  QCNC_FNC	fnc;			// qcnc_fnc class
  qeSTATUS	errorCode;		// error code

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
	"Disconnect: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qcncObj = AFuncGetTypedParameter(1, OTYPE_QCNC);
  if ((qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Disconnect: Couldn't get the ODBC Connection input parameter.");
    return;
  } // if

  if (!qcncCore->isConnected)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC Connection\r\n"
      "Disconnect: The database wasn't connected.");
    return;
  } // if

  fnc.CloseCursor(qcncObj);		// closes the sql cursors
  errorCode = (*qe_Disconnect)(qcncCore->hdbc);
  if (fnc.RecordError(qcncCore, errorCode))
  { AEvtPostSignalAtTail(qcncObj, QCNCIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if
  qcncCore->isConnected = FALSE;

  /* ---- FLOW 1 (Disconnected) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQcncDisconnectFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQcncGetErrorFunc                                       |
 | DESCRIPTION  : Returns the most recent ODBC error code, and the        |
 |		  associated error message.                               |
 | INPUT 1      : ODBC Connection: ODBC CONNECTION -- The ODBC Connection |
 |                object to get the error from.                           |
 | OUTPUT 1(OPT): Error Number: NUMBER -- The returned ODBC error code.   |
 | OUTPUT 2(OPT): Error Message: TEXT -- The returned ODBC error message. |
 | FLOW 1       : Fetched.                                                |
 | FLOW 2       : No Error.                                               |
 | Novell, Inc., November 1, 1993.                                     mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncGetErrorFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qcncObj;        	// qcnc object                (I: 1)
  QcncCore      *qcncCore;      	// qcnc core info
  AOBJECTID      errorNmbrObj;    	// number object              [O: 1]
  AOBJECTID      errorMsgObj;    	// text object                [O: 2]

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
	"Get Error: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qcncObj = AFuncGetTypedParameter(1, OTYPE_QCNC);
  if ((qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Get Error: Couldn't get the ODBC Connection input parameter.");
    return;
  } // if

  if (qcncCore->errorCode == qeSUCCESS)
  { /* ---- FLOW 2 (No Error) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((errorNmbrObj = AObjCreate(OTYPE_NUMBER)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Get Error: Couldn't create the Error Code output parameter.");
    return;
  } // if
  ONmbrSetInteger(errorNmbrObj, qcncCore->errorCode, TRUE);
  AFuncSetParameter(1, errorNmbrObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if ((errorMsgObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Get Error: Couldn't create the Error Message output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(errorMsgObj, qcncCore->errorMsg);
  AFuncSetParameter(2, errorMsgObj);

  /* ---- FLOW 1 (Fetched) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQcncGetErrorFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQcncRollbackFunc                                       |
 | DESCRIPTION  : Rolls back the current transaction.  The current        |
 |                transaction is the set of SQL statements executed since |
 |		  the last successful call to either the Connect, Commit, |
 |		  or Rollback functions.                                  |
 | INPUT 1      : ODBC Connection: ODBC CONNECTION -- The ODBC Connection |
 |                object to roll back the changes for.                    |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : Rolled Back.                                            |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., November 1, 1993.                                     mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncRollbackFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qcncObj;        	// qcnc object                (I: 1)
  QcncCore      *qcncCore;      	// qcnc core info

  QCNC_FNC	fnc;			// qcnc_fnc class
  qeSTATUS	errorCode;		// error code

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
	"Rollback: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qcncObj = AFuncGetTypedParameter(1, OTYPE_QCNC);
  if ((qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Rollback: Couldn't get the ODBC Connection input parameter.");
    return;
  } // if

  if (!qcncCore->isConnected)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC Connection\r\n"
      "Rollback: The database wasn't connected.");
    return;
  } // if

  if (!qcncCore->isAutoCommit)
  { errorCode = (*qe_Rollback)(qcncCore->hdbc);
    if (fnc.RecordError(qcncCore, errorCode))
    { AEvtPostSignalAtTail(qcncObj, QCNCIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if

    errorCode = (*qe_BeginTran)(qcncCore->hdbc);
    if (fnc.RecordError(qcncCore, errorCode))
      AEvtPostSignalAtMark(qcncObj, QCNCIFERROR);
  } // if

  /* ---- FLOW 1 (Rolled Back) ---- */
  AFuncReportResult(theSystem, 1, "");   
} // OQcncRollbackFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQcncStatusFunc                                         |
 | DESCRIPTION  : Returns whether the ODBC Connection object is currently |
 |                connected to the DBMS.                                  |
 | INPUT 1      : ODBC Connection: ODBC CONNECTION -- The ODBC Connection |
 |		  object to get the status from.                          |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : Connected.                                              |
 | FLOW 2       : Not Connected.                                          |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQcncStatusFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qcncObj;        	// qcnc object		      (I: 1)
  QcncCore      *qcncCore;      	// qcnc core info

  /* ---- FUNCTION INPUT 1 ---- */
  qcncObj = AFuncGetTypedParameter(1, OTYPE_QCNC);
  if ((qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC Connection\r\n"
      "Status: Couldn't get the ODBC Connection input parameter.");
    return;
  } // if

  /* ---- FLOW 1 (Connected), OR FLOW 2 (Not Connected) ---- */
  AFuncReportResult(theSystem, 1 + (qcncCore->isConnected == FALSE), "");
} // OQcncStatusFunc
