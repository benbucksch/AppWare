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
 | FILE		: QSQLAPI.CPP                                             |
 | DESCRIPTION	: Q+E ODBC SQL Method Callers and Methods.                |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	28-Apr-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include "qsql_fnc.h"
#include "memblock.h"
#include "safemem.h"


BOOL ChangeDView		/* ---- Change Database View ------------ */
( OBJECTID	qsqlObj,		// qsql object
  ODViewColDesc	colDesc			// column description
)
/* Changes database view. */
{ ODViewColDesc	*colDescPtr;		// column description pointer

  colDescPtr = (ODViewColDesc *) MBAlloc(sizeof(ODViewColDesc));
  *colDescPtr = colDesc;   	

  if (!AEvtPostStandard(qsqlObj, AEVENT_DVIEWCHANGED, (long) colDescPtr,
			AEVT_ATMARK | AEVT_SELF))
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
  QsqlCore	*qsql;			// qsql core info
  ODViewColData	*colData;		// column data

  QSQL_FNC	fnc;			// qsql_fnc class
   
  /* ---- INPUT 1 ---- */
  qsqlObj = theEvent->targetObject;
  if (!AObjCheckType(qsqlObj, OTYPE_QSQL))
    return;
  qsql = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE);
  ADBG_ASSERT_E(qsql, return, "ODBC SQL\n"
    "AEVENT_DVIEWGETDATA: Couldn't get the ODBC SQL input parameter.");

  /* ---- INPUT 2 ---- */
  if (!(colData = (ODViewColData *) theEvent->details))
    return;

  if (colData->recNmbr < 1 || !fnc.Fetch(*qsql, *colData))
  { MemResize(colData->colDataMB, 0);
    colData->recCount = 0;
  } // if
} // OQsqlDViewGetDataMeth

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

  /* ---- INPUT 1 ---- */
  qsqlObj = theEvent->targetObject;
  if (!AObjCheckType(qsqlObj, OTYPE_QSQL))
    return;
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);
  ADBG_ASSERT_E(qsql, return, "ODBC SQL\n"
    "AEVENT_DVIEWSET: Couldn't get the ODBC SQL input parameter.");

  qsql->isDView = TRUE;

  /* ---- sets database view on at the connection object ---- */
  if (!qsql->connectObj)
    return;
  qcnc = (Qcnc *) mb.GetPointer(qsql->connectObj, QCNCCORE);
  ADBG_ASSERT_E(qcnc, return, "ODBC SQL\n"
    "AEVENT_DVIEWSET: Couldn't get the ODBC SQL input parameter.");

  qcnc->isDView = TRUE;
} // OQsqlDViewSetMeth
