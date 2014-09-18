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
 | FILE		: QSQL_MTH.CPP                                            |
 | DESCRIPTION	: Q+E ODBC SQL Method Callers and Methods.                |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	28-Apr-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include "qsql_fnc.h"
#include "queue.h"
#include "memblock.h"
#include "safemem.h"


BOOL QSQL_FNC::ChangeDView	/* ---- Change Database View ------------ */
( OBJECTID	qsqlObj,		// qsql object
  QsqlCore	*qsqlCore,		// qsql core info
  ODViewColDesc	colDesc			// column description
)
/* Changes database view. */
{ ODViewColDesc	*colDescPtr;		// column description pointer
  Queue		*src;			// queue class
  Queue		*dest;			// queue class		
  OBJECTID	obj;			// object

  colDescPtr = (ODViewColDesc *) MBAlloc(sizeof(ODViewColDesc));
  *colDescPtr = colDesc;   	

  /* ---- informs all the subscribed objects ---- */
  src = (Queue *) qsqlCore->objList;
  while (!(dest = new Queue))
    MemError();
  while ((obj = (OBJECTID) src->Deque()) != 0)
  { AEvtPostStandard(obj, AEVENT_DVIEWCHANGED, (long) colDescPtr,
		     AEVT_ATMARK | AEVT_DIRECT);
    dest->Enque((void *) obj);
  } // while
  delete src;
  qsqlCore->objList = (void *) dest;

  if (!AEvtPostStandard(qsqlObj, AEVENT_DVIEWCHANGED, (long) colDescPtr,
			AEVT_ATMARK | AEVT_DIRECT))
  { /* ---- releases column description ---- */
    MBFree(colDescPtr->colNames);
    MBFree(colDescPtr);
  } // if
  return TRUE;
} // ChangeDView


/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlDViewChangedMeth                                   |
 | DESCRIPTION	: Releases column description created by ChangeDView.     |
 | Novell, Inc., April 28, 2994.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlDViewChangedMeth
( pAEvtInfo	theEvent,		// event pointer
  pAObjMessage				// system pointer
)
{
  /* ---- INPUT 1 ---- */
  if (!AObjCheckType(theEvent->targetObject, OTYPE_QSQL))
    return;

  /* ---- INPUT 2 ---- */
  if (!theEvent->details)
    return;

  MBFree(((ODViewColDesc *) theEvent->details)->colNames);
  MBFree((void *) theEvent->details);
  theEvent->details = 0;
} // OQsqlDViewChangedMeth

/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlDViewGetDataMeth                                   |
 | DESCRIPTION	: Returns an array of columns memory block.  Each column  |
 |		  memory block is "\0"-deliminated column data.           |
 | Novell, Inc., April 28, 1994.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlDViewGetDataMeth
( pAEvtInfo	theEvent,		// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock	mb;			// memblock class
  OBJECTID	qsqlObj;		// qsql object		     	
  QsqlCore	*qsqlCore;		// qsql core info
  ODViewColData	*colData;		// column data

  QSQL_FNC	fnc;			// qsql_fnc class
   
  /* ---- INPUT 1 ---- */
  qsqlObj = theEvent->targetObject;
  if (!AObjCheckType(qsqlObj, OTYPE_QSQL))
    return;
  qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE);
  ADBG_ASSERT_E(qsqlCore, return, "ODBC SQL\n"
    "AEVENT_DVIEWGETDATA: Couldn't get the ODBC SQL input parameter.");
  if (!qsqlCore->isDView)
    return;

  /* ---- INPUT 2 ---- */
  if (!(colData = (ODViewColData *) theEvent->details))
    return;
  if (colData->recNmbr < 1 || colData->recCount < 0)
  { colData->retCode = -1;
    return;
  } // if
  if (!qsqlCore->isCursor)
  { colData->retCode = DVIEWNOSTMT;
    return;
  } // if

  colData->retCode = fnc.Fetch(qsqlCore, colData->recCount,
                               colData->recNmbr);
  if (colData->retCode == -1)
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);
    return;
  } // if
  if (colData->retCode > 0)
    fnc.FetchToColData(qsqlCore, colData->retCode, colData->colDataMB);
} // OQsqlDViewGetDataMeth

/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlDViewGetRowCountMeth                               |
 | DESCRIPTION	: Returns the number of rows.                             |
 | Novell, Inc., April 28, 1994.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlDViewGetRowCountMeth
( pAEvtInfo	theEvent,		// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock	mb;			// memblock class
  OBJECTID	qsqlObj;		// qsql object		     	
  QsqlCore	*qsqlCore;		// qsql core info
   
  /* ---- INPUT 1 ---- */
  qsqlObj = theEvent->targetObject;
  if (!AObjCheckType(qsqlObj, OTYPE_QSQL))
    return;
  qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE);
  ADBG_ASSERT_E(qsqlCore, return, "ODBC SQL\n"
    "AEVENT_DVIEWGETDATA: Couldn't get the ODBC SQL input parameter.");

  /* ---- INPUT 2 ---- */
  if (!theEvent->details)
    return;

  *(long *) theEvent->details = qsqlCore->recCount;
} // OQsqlDViewGetRowCountMeth

/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlDViewSetMeth                                       |
 | DESCRIPTION	: Sets auto query on.                                     |
 | Novell, Inc., April 28, 1994.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlDViewSetMeth
( pAEvtInfo	theEvent,		// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock	mb;			// memblock class
  OBJECTID	qsqlObj;		// qsql object
  Qsql		*qsql;			// qsql core info
  Qcnc		*qcnc;			// qcnc core info
  Queue		*q;			// queue class

  /* ---- INPUT 1 ---- */
  qsqlObj = theEvent->targetObject;
  if (!AObjCheckType(qsqlObj, OTYPE_QSQL))
    return;
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);
  ADBG_ASSERT_E(qsql, return, "ODBC SQL\n"
    "AEVENT_DVIEWSET: Couldn't get the ODBC SQL input parameter.");

  /* ---- INPUT 2 ---- */
  if (!theEvent->sourceObject ||
      AObjCheckType(theEvent->sourceObject, OTYPE_QSQL))
    return;
         
  /* ---- sets database view on at the connection object ---- */
  if (!qsql->connectObj)
    return;
  qcnc = (Qcnc *) mb.GetPointer(qsql->connectObj, QCNCCORE);
  ADBG_ASSERT_E(qcnc, return, "ODBC SQL\n"
    "AEVENT_DVIEWSET: Couldn't get the ODBC SQL input parameter.");

  qcnc->isDView = TRUE;

  /* ---- stores the subsrcibed object ---- */
  if (!qsql->objList)
  { while (!(q = new Queue)) 
      MemError();
    qsql->objList = (void *) q;
  } // if
  else
    q = (Queue *) qsql->objList;
  q->Enque((void *) theEvent->sourceObject);
       
  qsql->isDView = TRUE;
} // OQsqlDViewSetMeth
