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
 | FILE		: QCNC_ADM.CPP                                            |
 | DESCRIPTION	: Q+E ODBC Connection Object Administration.              |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Add Database View.                   |
 |		  mw	08-Jun-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#include "qcnc_fnc.h"
#include "memblock.h"
#include "safemem.h"


extern int	_WinAllocFlag;
HINSTANCE	hInstance;
			

#define CURRDLLVER	1		// current dll version

/*------------------------------------------------------------------------*\
 | FUNCTION	: GetSerVersion                                           |
 | DESCRIPTION	: Returns ODBC objects' DLL version number.               |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
long ALMAPI GetSerVersion
( ATYPEID	type			// object type
)
{
  return type == OTYPE_QCNC || type == OTYPE_QSQL? CURRDLLVER : 0;
} // GetSerVersion


#define CURRQCNCVER	0		// current qcnc object version

class QCNC_ADM			/* ---- ODBC Connection Object Admin ---- */
{
public:
  long CheckObjectVersion(AOBJECTID, pAObjMessage);
  /* Checks the object version. */

  long AssignObject(AOBJECTID, pAObjMessage);
  /* Assigns the source object to the destination object at run-time. */

  long CheckObjects(AOBJECTID);
  /* Checks all the objects stored in the object's datablocks. */

  long ValidateObjects(AOBJECTID, pAObjMessage);
  /* Validates all the objects stored in the object's datablocks. */

  long BuildSignals(pAObjMessage);
  /* Builds the object signals. */

  long CreateObject(AOBJECTID);
  /* Creates a new object. */

  long ExportObject(AOBJECTID);
  /* Exports the object. */

  long ImportObject(AOBJECTID);
  /* Imports the object. */

  long EditObject(AOBJECTID);
  /* Edits the object. */
};


/*------------------------------------------------------------------------*\
 | FUNCTION	: OQcncAdministractionRoutine                             |
 | DESCRIPTION	: ODBC Connection object's administration routines.       |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
long ALMAPI OQcncAdministrationRoutine
( AOBJECTID	qcncObj,		// qcnc object
  pAObjMessage	theSystem		// system pointer
)
{ QCNC_ADM	adm;			// qcnc_adm class

  switch (theSystem->message1)
  { case AOBJ_READ:
      return adm.CheckObjectVersion(qcncObj, theSystem);
    case AOBJ_ASSIGNOBJECT:
      return adm.AssignObject(qcncObj, theSystem);
    case AOBJ_CHECKOBJECT:
      return adm.CheckObjects(qcncObj);
    case AOBJ_VALIDATEOBJECTIDS:
      return adm.ValidateObjects(qcncObj, theSystem);
    case AOBJ_BUILDSIGNALS:
      return adm.BuildSignals(theSystem);
    case AOBJ_CREATED:
      return adm.CreateObject(qcncObj);
    case AOBJ_EXPORTOBJECT:
      return adm.ExportObject(qcncObj);
    case AOBJ_IMPORTOBJECT:
      return adm.ImportObject(qcncObj);
    case AOBJ_EDITOBJECT:
      return adm.EditObject(qcncObj);
  } // switch
  return A_NOTHANDLED;
} // OQcncAdministrationRoutine

long QCNC_ADM::CheckObjectVersion/* --- Check Object Version ------------ */
( AOBJECTID	qcncObj,		// qcnc object
  pAObjMessage	theSystem		// system pointer
)
/* Checks the version of the object. */
{ MemBlock	mb;			// memblock class

  theSystem->message3 = CURRQCNCVER;
  if (*(int *) mb.GetPointer(qcncObj, QCNCCORE) > CURRQCNCVER)
  { AObjReportError(qcncObj, theSystem->message1, A_ERROR,
		    "ODBC Connection\tALM_QSQL.DLL", CURRQCNCVER);
    return A_ERROR;
  } // if
  return A_OK;
} // CheckObjectVersion

long QCNC_ADM::AssignObject	/* ---- Assign Object ------------------- */
( AOBJECTID	destObj,		// destination object
  pAObjMessage	theSystem		// system pointer
)
/* Assigns the source object to the destination object at run-time. */
{ MemBlock	mb;			// memblock class
  QcncCore	*qcncCore;		// qcnc core info
  AOBJECTID	srcObj;			// source object
  int		dataSize;		// datablock size
  void		*srcData;		// source datablock
  void		*destData;		// destination datablock

  int		i;			// counter

  srcObj = (AOBJECTID) theSystem->message3;
  if (srcObj == destObj ||
      AObjGetType(srcObj) != OTYPE_QCNC ||
      AObjGetType(destObj) != OTYPE_QCNC)
    return A_ERROR;			// incompatible object types

  /* ---- copies the datablocks ---- */
  if (!AObjSetDataCount(destObj, QCNCCOUNT))
    return A_ERROR;
  for(i = 0; i < QCNCCOUNT; i++)
  { dataSize = AObjGetDataSize(srcObj, i);
    if (!AObjResizeData(destObj, i, dataSize))
      return A_ERROR;
    if (dataSize)
    { if ((srcData = mb.GetPointer(srcObj, i)) == 0||
	  (destData = mb.GetPointer(destObj, i)) == 0)
	return A_ERROR;
      CopyMemory(destData, srcData, dataSize);
    } // if
  } // for

  /* ---- inits private values ---- */
  /* ---- datablock QCNCCORE ---- */
  if ((qcncCore = (QcncCore *) mb.GetPointer(destObj, QCNCCORE)) == 0)
    return A_ERROR;
  qcncCore->errorCode = qeSUCCESS;	// no error
  qcncCore->isConnected = FALSE;	// not connected
  return A_OK;
} // AssignObject

long QCNC_ADM::CheckObjects	/* ---- Check Objects ------------------- */
( AOBJECTID	qcncObj			// qcnc object
)
/* Checks all the objects stored in the object's datablocks. */
{ MemBlock	mb;			// memblock class
  Qcnc		*qcnc;			// qcnc core info

  /* ---- datablock QCNCCORE ---- */
  if ((qcnc = (Qcnc *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
    return A_ERROR;
  if (!AObjCheckType(qcnc->usernameObj, OTYPE_TEXT))
    qcnc->usernameObj = 0;
  if (!AObjCheckType(qcnc->passwordObj, OTYPE_TEXT))
    qcnc->passwordObj = 0;
  if (!AObjCheckType(qcnc->paramsObj, OTYPE_TEXT))
    qcnc->paramsObj = 0;
  return A_OK;
} // CheckObjects

long QCNC_ADM::ValidateObjects	/* ---- Validate Objects ---------------- */
( AOBJECTID	qcncObj,		// qcnc object
  pAObjMessage	theSystem		// system pointer
)
/* Validates all the objects stored in the object's datablocks. */
{ MemBlock	mb;			// memblock class
  Qcnc		*qcnc;			// qcnc core info

  /* ---- datablock QCNCCORE ---- */
  if ((qcnc = (Qcnc *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
    return A_ERROR;
  AObjValidateID(&qcnc->usernameObj, theSystem);
  AObjValidateID(&qcnc->passwordObj, theSystem);
  AObjValidateID(&qcnc->paramsObj, theSystem);
  return A_OK;
} // ValidateObjects

long QCNC_ADM::BuildSignals	/* ---- Build Signals ------------------- */
( pAObjMessage	theSystem		// system pointer
)
/* Builds the object signals. */
{ MemBlock	mb;			// memblock class
  AMEMBLOCKID	signalRecMB;		// signal record memory block
  AObjSignalRecord *signalRec;		// signal record 

  char 		*signal[] = {{"If Error"}};
  int		signalCount = sizeof(signal) / sizeof(signal[0]);

  int		i;			// counter

  signalRecMB = (AMEMBLOCKID) theSystem->message2;
  AMemSetSize(signalRecMB, signalCount * sizeof(AObjSignalRecord));
  if ((signalRec = (AObjSignalRecord *) mb.GetPointer(signalRecMB)) == 0)
    return A_ERROR;

  for (i = 0; i < signalCount; i++)
  { signalRec[i].theID = i;
    lstrcpy(signalRec[i].theName, signal[i]);
  } // for
  return A_OK;
} // BuildSignals

long QCNC_ADM::CreateObject	/* ---- Create Object ------------------- */
( AOBJECTID	qcncObj			// qcnc object
)
/* Creates a new object. */
{ MemBlock	mb;			// memblock class
  Qcnc		*qcnc;			// qcnc core info

  if (!(AObjSetDataCount(qcncObj, QCNCCOUNT)))
    return A_ERROR;

  /* ---- datablock OCNCCORE ---- */
  if (!AObjResizeData(qcncObj, QCNCCORE, sizeof(Qcnc)))
    return A_ERROR;
  if ((qcnc = (Qcnc *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
    return A_ERROR;

  qcnc->version = CURRQCNCVER;		// current q+e connection object ver

  qcnc->dbmsID = 0;			// dbms id
  qcnc->usernameObj = 0;		// unassigned username's object
  qcnc->passwordObj = 0;		// unassigned password's object
  qcnc->paramsObj = 0;			// unassigned parameter's object
  qcnc->isAutoCommit = DEF_AUTOCOMMIT;	// default auto commit
  qcnc->isDView = FALSE;		// default database view

  FillMemory(qcnc->spares, 0, sizeof(qcnc->spares));	// inits spares
  return A_OK;
} // CreateObject

long QCNC_ADM::ExportObject	/* ---- Export Object ------------------- */
( AOBJECTID	qcncObj			// qcnc object
)
/* Exports the object. */
{ MemBlock	mb;			// memblock class
  Qcnc		*qcnc;			// qcnc core info

  /* ---- datablock QCNCCORE ---- */
  if ((qcnc = (Qcnc *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
    return A_ERROR;

  AUpsfExportField("VERSION", 0, AUPSF_NATV_INT,
    AUPSF_UNIV_INTEGER, &qcnc->version, sizeof(qcnc->version));

  AUpsfExportField("DBMSID", 0, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
    &qcnc->dbmsID, sizeof(qcnc->dbmsID));
  AUpsfExportField("USERNAMEOBJ", 0, AUPSF_NATV_OBJECTREF,
    AUPSF_UNIV_OBJECTREF, &qcnc->usernameObj, sizeof(qcnc->usernameObj));
  AUpsfExportField("PASSWORDOBJ", 0, AUPSF_NATV_OBJECTREF,
    AUPSF_UNIV_OBJECTREF, &qcnc->passwordObj, sizeof(qcnc->passwordObj));
  AUpsfExportField("PARAMSOBJ", 0, AUPSF_NATV_OBJECTREF,
    AUPSF_UNIV_OBJECTREF, &qcnc->paramsObj, sizeof(qcnc->paramsObj));
  AUpsfExportField("ISAUTOCOMMIT", 0, AUPSF_NATV_BOOL,
    AUPSF_UNIV_INTEGER, &qcnc->isAutoCommit, sizeof(qcnc->isAutoCommit));
  return A_OK;
} // ExportObject

long QCNC_ADM::ImportObject	/* ---- Import Object ------------------- */
( AOBJECTID	qcncObj			// qcnc object
)
/* Imports the object. */
{ MemBlock	mb;			// memblock class
  Qcnc		*qcnc;			// qcnc core info

  /* ---- datablock QCNCCORE ---- */
  if ((qcnc = (Qcnc *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
    return A_ERROR;

  AUpsfImportField("VERSION", 0, AUPSF_NATV_INT,
    &qcnc->version, sizeof(qcnc->version));

  if (AUpsfImportField("DBMSID", 0, AUPSF_NATV_INT,
	&qcnc->dbmsID, sizeof(qcnc->dbmsID)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC Connection: Unable to import 'Product Name'.");
  if (AUpsfImportField("USERNAMEOBJ", 0, AUPSF_NATV_OBJECTREF,
	&qcnc->usernameObj, sizeof(qcnc->usernameObj)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC Connection: Unable to import 'Username'.");
  if (AUpsfImportField("PASSWORDOBJ", 0, AUPSF_NATV_OBJECTREF,
	&qcnc->passwordObj, sizeof(qcnc->passwordObj)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC Connection: Unable to import 'Password'.");
  if (AUpsfImportField("PARAMSOBJ", 0, AUPSF_NATV_OBJECTREF,
	&qcnc->paramsObj, sizeof(qcnc->paramsObj)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC Connection: Unable to import 'Other Parameters'.");
  if (AUpsfImportField("ISAUTOCOMMIT", 0, AUPSF_NATV_BOOL,
	&qcnc->isAutoCommit, sizeof(qcnc->isAutoCommit)) != AUPSF_NOERROR)
    AUpsfLogComment("ODBC Connection: Unable to import 'Auto Commit'.");
  return A_OK;
} // ImportObject

long QCNC_ADM::EditObject	/* ---- Edit Object --------------------- */
( AOBJECTID	qcncObj			// qcnc object
)
/* Edits the object. */
{
  return DialogBoxParam(hInstance, "Qcnc", GetActiveWindow(),
	   OQcncObjectDialog, (LPARAM) qcncObj)? A_MODIFIED : A_OK;
} // EditObject


extern "C" BOOL WINAPI DllEntryPoint
( HINSTANCE	hInstDLL,
  DWORD		dwReason,
  LPVOID	pReserved
)
/* DLL entry point */
{
  (void) pReserved;

  switch(dwReason)
  { case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
      hInstance = hInstDLL;
      break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
  } // switch
  return TRUE;
} // DLLEntryPoint

