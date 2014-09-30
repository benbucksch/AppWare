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
 | FILE		: QSQL_ADM.CPP                                            |
 | DESCRIPTION	: Q+E ODBC SQL Object Administration.                     |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Add Database View.                   |
\*------------------------------------------------------------------------*/
#include "qsql_fnc.h"
#include "memblock.h"
#include "safemem.h"


/* LibMain is in QCNC_ADM.CPP */


/* GetSerVersion is in QCNC_ADM.CPP */


#define CURRQSQLVER	0		// current qsql object version

class QSQL_ADM			/* ---- ODBC SQL Object Administration -- */
{
public:
  long CheckObjectVersion(OBJECTID, pAObjMessage);
  /* Checks the object version. */

  long AssignObject(OBJECTID, pAObjMessage);
  /* Assigns the source object to the destination object at run-time. */

  long CheckObjects(OBJECTID);
  /* Checks all the objects stored in the object's datablocks. */

  long ValidateObjects(OBJECTID, pAObjMessage);
  /* Validates all the objects stored in the object's datablocks. */

  long BuildSignals(pAObjMessage);
  /* Builds the object signals. */

  long CreateObject(OBJECTID);
  /* Creates a new object. */

  long ExportObject(OBJECTID);
  /* Exports the object. */

  long ImportObject(OBJECTID);
  /* Imports the object. */

  long EditObject(OBJECTID);
  /* Edits the object. */
};


/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlAdministractionRoutine                             |
 | DESCRIPTION	: ODBC SQL object's administration routines.              |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
long ALMAPI OQsqlAdministrationRoutine
( OBJECTID	qsqlObj,		// qsql object		
  pAObjMessage	theSystem		// system pointer
)
{ QSQL_ADM	adm;			// qsql_adm class

  switch (theSystem->message1)
  { case AOBJ_READ:
      return adm.CheckObjectVersion(qsqlObj, theSystem);
    case AOBJ_ASSIGNOBJECT:
      return adm.AssignObject(qsqlObj, theSystem);
    case AOBJ_CHECKOBJECT:
      return adm.CheckObjects(qsqlObj);
    case AOBJ_VALIDATEOBJECTIDS:
      return adm.ValidateObjects(qsqlObj, theSystem);
    case AOBJ_BUILDSIGNALS:
      return adm.BuildSignals(theSystem);
    case AOBJ_CREATED:
      return adm.CreateObject(qsqlObj);
    case AOBJ_EXPORTOBJECT:
      return adm.ExportObject(qsqlObj);
    case AOBJ_IMPORTOBJECT:
      return adm.ImportObject(qsqlObj);
    case AOBJ_EDITOBJECT:
      return adm.EditObject(qsqlObj);
  } // switch
  return A_NOTHANDLED;
} // OQsqlAdministrationRoutine

long QSQL_ADM::CheckObjectVersion/* --- Check Object -------------------- */
( OBJECTID	qsqlObj,		// qsql object				
  pAObjMessage	theSystem		// system pointer
)
/* Checks the object version. */
{ MemBlock	mb;			// memblock class
	
  theSystem->message3 = CURRQSQLVER;
  if (*(int *) mb.GetPointer(qsqlObj, QSQLCORE) > CURRQSQLVER)
  { AObjReportError(qsqlObj, theSystem->message1, A_ERROR,
		    "ODBC SQL\tALM_QSQL.dll", CURRQSQLVER);
    return A_ERROR;
  } // if
  return A_OK;
} // CheckObjectVersion

long QSQL_ADM::AssignObject	/* ---- Assign Object ------------------- */
( OBJECTID	destObj,		// destination object
  pAObjMessage	theSystem		// system pointer
)
/* Copies the source object to the destination object. */
{ MemBlock	mb;			// memblock class
  QsqlCore	*qsqlCore;		// qsql core info
  OBJECTID	srcObj;			// source object
  long		dataSize;		// datablock size
  void		*srcData;		// source datablock
  void		*destData;		// destination datablock

  WORD		i;			// counter

  srcObj = (OBJECTID) theSystem->message3;
  if (srcObj == destObj ||
      AObjGetType(srcObj) != OTYPE_QSQL ||
      AObjGetType(destObj) != OTYPE_QSQL)	
    return A_ERROR;			// incompatible object types

  /* ---- copies the datablocks ---- */
  if (!AObjSetDataCount(destObj, QSQLCOUNT))
    return A_ERROR;
  for (i = 0; i < QSQLCOUNT; i++)
  { dataSize = AObjGetDataSize(srcObj, i);
    if (!AObjResizeData(destObj, i, dataSize))
      return A_ERROR;
    if (dataSize)
    { if (!(srcData = mb.GetPointer(srcObj, i)) ||
	  !(destData = mb.GetPointer(destObj, i)))
	return A_ERROR;
      hmemcpy(destData, srcData, dataSize);
    } // if
  } // for

  /* ---- inits private values ---- */
  /* ---- datablock QSQLCORE ---- */
  if (!(qsqlCore = (QsqlCore *) mb.GetPointer(destObj, QSQLCORE)))
    return A_ERROR;
  qsqlCore->errorCode = qeSUCCESS;	// no error
  qsqlCore->isCursor = FALSE;		// no cursor
  qsqlCore->isSelectMB = FALSE;		// no select memory block
  qsqlCore->isDataBuff = FALSE;		// no select data buffer
  return A_OK;
} // AssignObject

long QSQL_ADM::CheckObjects	/* ---- Check Objects ------------------- */
( OBJECTID	qsqlObj			// qsql object
)
/* Checks all the objects stored in the object's datablocks. */
{ MemBlock	mb;			// memblock class
  Qsql		*qsql;          	// qsql core info
  WORD		varCount;       	// number of VARs
  Var		*var;           	// an array of VARs
  Var		huge *varPtr;		// VAR pointer

  WORD		i;              	// counter

  /* ---- datablock QSQLCORE ---- */
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);
  if (!AObjCheckType(qsql->connectObj, OTYPE_QCNC))
    qsql->connectObj = 0;
  if (!AObjCheckType(qsql->sqlObj, OTYPE_TEXT))
    qsql->sqlObj = 0;

  /* ---- datablock QSQLVARS ---- */
  varCount = AObjGetDataSize(qsqlObj, QSQLVARS) / sizeof(Var);
  var = (Var *) mb.GetPointer(qsqlObj, QSQLVARS);
  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
    if (!(varPtr->type = AObjGetType(varPtr->obj)))
      varPtr->obj = 0;
  return A_OK;
} // CheckObjects

long QSQL_ADM::ValidateObjects	/* ---- Validate Object IDs ------------- */
( OBJECTID	qsqlObj,		// qsql object
  pAObjMessage	theSystem		// system pointer
)
/* Validates all the objects stored in the object's datablocks. */
{ MemBlock	mb;			// memblock class
  Qsql		*qsql;          	// qsql core info
  WORD		varCount;       	// number of VARs
  Var		*var;           	// an array of VARs
  Var		huge *varPtr;		// VAR pointer			

  WORD		i;              	// counter

  /* ---- datablock QSQLCORE ---- */
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);
  AObjValidateID(&qsql->connectObj, theSystem);
  AObjValidateID(&qsql->sqlObj, theSystem);

  /* ---- datablock QSQLVARS ---- */
  varCount = AObjGetDataSize(qsqlObj, QSQLVARS) / sizeof(Var);
  var = (Var *) mb.GetPointer(qsqlObj, QSQLVARS);
  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
    AObjValidateID(&varPtr->obj, theSystem);
  return A_OK;
} // ValidateObjects

long QSQL_ADM::BuildSignals	/* ---- Build Signals ------------------- */
( pAObjMessage	theSystem		// system pointer
)
/* Builds the object signals. */
{ MemBlock	mb;			// memblock class
  MEMBLOCKID	signalRecMB;		// signal record memory block
  AObjSignalRecord *signalRec;		// signal record 

  char 		*signal[] = {{"If Error"}};
  WORD		signalCount = sizeof(signal) / sizeof(signal[0]);

  WORD		i;			// counter

  signalRecMB = (MEMBLOCKID) theSystem->message2;
  AMemSetSize(signalRecMB, signalCount * sizeof(AObjSignalRecord));
  if (!(signalRec = (AObjSignalRecord *) mb.GetPointer(signalRecMB)))
    return A_ERROR;

  for (i = 0; i < signalCount; i++)
  { signalRec[i].theID = i;
    lstrcpy(signalRec[i].theName, signal[i]);
  } // for
  return A_OK;
} // BuildSignals

long QSQL_ADM::CreateObject	/* ---- Create Object ------------------- */
( OBJECTID	qsqlObj			// qsql object
)
/* Creates a new object. */
{ MemBlock	mb;			// memblock class
  Qsql		*qsql;			// qsql core info

  if (!(AObjSetDataCount(qsqlObj, QSQLCOUNT)))
    return A_ERROR;

  /* ---- datablock QSQLCORE ---- */
  if (!AObjResizeData(qsqlObj, QSQLCORE, sizeof(Qsql)))
    return A_ERROR;
  if (!(qsql = (Qsql *) mb.GetPointer(qsqlObj,QSQLCORE)))
    return A_ERROR;

  qsql->version = CURRQSQLVER;		// current q+e sql object version

  qsql->connectObj = 0;			// unassigned connection's object
  qsql->sqlObj = 0;			// unassigned sql's object

  /* ---- options ---- */
  qsql->rowCount = DEF_ROWCNT;		// default # of rows per fetch
  qsql->colSprtr = DEF_COLSPRTR;	// default column seperator
  qsql->isOEM = FALSE;			// default character sets
  qsql->isDView = FALSE;		// default database view
  qsql->objList = 0;			// empty object list

  memset(qsql->spares, 0, sizeof(qsql->spares));	// inits spares

  /* ---- datablock QSQLVAR ---- */
  if (!AObjResizeData(qsqlObj, QSQLVARS, 0))
    return A_ERROR;
  return A_OK;
} // CreateObject

long QSQL_ADM::ExportObject	/* ---- Export Object ------------------- */
( OBJECTID	qsqlObj			// qsql object
)
/* Exports the object. */
{ MemBlock	mb;			// memblock class
  Qsql		*qsql;			// qsql core info
  WORD		varCount;		// number of VARs
  Var		*var;			// an array of VARs
  Var		huge *varPtr;		// VAR pointer

  WORD		i;			// counter

  /* ---- datablock QSQLCORE ---- */
  if (!(qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE)))
    return A_ERROR;

  AUpsfExportField("VERSION", 0, AUPSF_NATV_INT,
    AUPSF_UNIV_INTEGER, &qsql->version, sizeof(qsql->version));

  AUpsfExportField("CONNECTOBJ", 0, AUPSF_NATV_OBJECTREF,
    AUPSF_UNIV_OBJECTREF, &qsql->connectObj, sizeof(qsql->connectObj));
  AUpsfExportField("SQLOBJ", 0, AUPSF_NATV_OBJECTREF,
    AUPSF_UNIV_OBJECTREF, &qsql->sqlObj, sizeof(qsql->sqlObj));
  AUpsfExportField("ROWCOUNT", 0, AUPSF_NATV_LONG,
    AUPSF_UNIV_INTEGER, &qsql->rowCount, sizeof(qsql->rowCount));
  AUpsfExportField("COLSEPARATOR", 0, AUPSF_NATV_INT,
    AUPSF_UNIV_INTEGER, &qsql->colSprtr, sizeof(qsql->colSprtr));
  AUpsfExportField("ISOEM", 0, AUPSF_NATV_BOOL,
    AUPSF_UNIV_INTEGER, &qsql->isOEM, sizeof(qsql->isOEM));

  /* ---- datablock QSQLVARS ---- */
  varCount = AObjGetDataSize(qsqlObj, QSQLVARS) / sizeof(Var);
  var = (Var *) mb.GetPointer(qsqlObj, QSQLVARS);
  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
  { AUpsfExportField("VARNAME", i, AUPSF_NATV_STRING,
      AUPSF_UNIV_TEXT, varPtr->name, hstrlen(varPtr->name));
    AUpsfExportField("VAROBJ", i, AUPSF_NATV_OBJECTREF,
      AUPSF_UNIV_OBJECTREF, &varPtr->obj, sizeof(varPtr->obj));
  } // for
  return A_OK;
} // ExportObject

long QSQL_ADM::ImportObject	/* ---- Import Object ------------------- */
( OBJECTID	qsqlObj			// qsql object
)
/* Imports the object. */
{ MemBlock	mb;			// memblock class
  Qsql		*qsql;			// qsql core info
  WORD		varCount;		// number of VARs
  Var		*var;			// an array of VARs
  Var		*varPtr;		// VAR pointer

  WORD		i;			// counter

  /* ---- datablock QSQLCORE ---- */
  if (!(qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE)))
    return A_ERROR;

  AUpsfImportField("VERSION", 0, AUPSF_NATV_INT,
    &qsql->version, sizeof(qsql->version));

  if (AUpsfImportField("CONNECTOBJ", 0, AUPSF_NATV_OBJECTREF,
	&qsql->connectObj, sizeof(qsql->connectObj)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC SQL: Unable to import 'Connection Via'.");
  if (AUpsfImportField("SQLOBJ", 0, AUPSF_NATV_OBJECTREF,
	&qsql->sqlObj, sizeof(qsql->sqlObj)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC SQL: Unable to import 'Use Text In'.");
  if (AUpsfImportField("ROWCOUNT", 0, AUPSF_NATV_LONG,
	&qsql->rowCount, sizeof(qsql->rowCount)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC SQL: Unable to import 'Default Max Rows per Fetch'.");
  if (AUpsfImportField("COLSEPARATOR", 0, AUPSF_NATV_INT,
	&qsql->colSprtr, sizeof(qsql->colSprtr)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC SQL: Unable to import 'Column Seperators for ...'.");
  if (AUpsfImportField("ISOEM", 0, AUPSF_NATV_BOOL,
	&qsql->isOEM, sizeof(qsql->isOEM)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC SQL: Unable to import 'OEM Character Sets'.");

  /* ---- datablock QSQLVARS ---- */
  varCount = AUpsfFieldCount("VARNAME");
  if (!AObjResizeData(qsqlObj, QSQLVARS, varCount * sizeof(Var)))
    return A_ERROR;
  if (varCount)
    if (!(var = (Var *) mb.GetPointer(qsqlObj, QSQLVARS)))
      return A_ERROR;
  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
  { if (AUpsfImportField("VARNAME", i, AUPSF_NATV_STRING,
	  varPtr->name, sizeof(varPtr->name)) != AUPSF_NOERROR)
    { hstrcpy(varPtr->name, DEF_VARNAME);
      AUpsfLogComment("ODBC SQL: Unable to import a variable name.");
    } // if
    if (AUpsfImportField("VAROBJ", i, AUPSF_NATV_OBJECTREF,
	  &varPtr->obj, sizeof(varPtr->obj)) != AUPSF_NOERROR)
    { varPtr->obj = 0;
      varPtr->type = 0;
      AUpsfLogComment("ODBC SQL: Unable to import a variable's object.");
    } // if
    else
      varPtr->type = AObjGetType(varPtr->obj);
  } // for
  return A_OK;
} // ImportObject

long QSQL_ADM::EditObject	/* ---- Edit Object --------------------- */
( OBJECTID	qsqlObj			// qsql object
)
/* Edits the object. */
{
  return DialogBoxParam(hInstance, "Qsql", GetActiveWindow(),
	   OQsqlObjectDialog, (LPARAM) qsqlObj)? A_MODIFIED : A_OK;
} // EditObject
