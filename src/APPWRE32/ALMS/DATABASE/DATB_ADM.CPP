/*------------------------------------------------------------------------*\
 | FILE		: DATB_ADM.CPP                                            |
 | DESCRIPTION	: Database object Administration.                         |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include "datb.h"
#include "datb_fnc.h"
#include "dbkey.h"
#include "safemem.h"
#include "memblock.h"
#include "strutl.h"


#define CURRDLLVER	1		// current DLL version

extern int      _WinAllocFlag;
HINSTANCE       hInstance;

/*------------------------------------------------------------------------*\
 | FUNCTION	: GetSerVersion                                           |
 | DESCRIPTION	: Returns Database object's DLL version number.           |
 | Novell, Inc., September 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
long ALMAPI GetSerVersion
( ATYPEID	type			// object type
)
/* Returns Database object's DLL version number. */
{
  return type == OTYPE_DATABASE? CURRDLLVER : 0;
} // GetSerVersion


#define CURRDATBVER	0		// current database object version

class DATB_ADM			/* ---- Database Object Administration -- */
{
public:
  long CheckObjectVersion(AOBJECTID, pAObjMessage);
  /* Checks the object version. */

  long AssignObject(AOBJECTID, pAObjMessage);
  /* Assigns the source object to the destination object at run-time. */

  long CreateObject(AOBJECTID);
  /* Creates a new object at design-time. */

  long ExportObject(AOBJECTID);
  /* Exports the object. */

  long ImportObject(AOBJECTID);
  /* Imports the object. */

  long ValidateObjects(AOBJECTID, pAObjMessage);
  /* Validates all the objects stored in the object's datablocks. */

  long CheckObjects(AOBJECTID);
  /* Checks all the objects stored in the object's datablocks. */

  long EditObject(AOBJECTID);
  /* Edits the object. */
};

/* ---- Database Object administration routines ------------------------- */

/*------------------------------------------------------------------------*\
 | FUNCTION	: ODatbAdministrationRoutine                              |
 | DESCRIPTION	: Database object's administration routines.              |
 | Novell, Inc., September 1, 1992.				       mw |
\*------------------------------------------------------------------------*/
long ALMAPI ODatbAdministrationRoutine
( AOBJECTID	datbObj,		// datb object
  pAObjMessage	theSystem		// system pointer
)
/* Database object's administration routines. */
{ DATB_ADM	adm;			// datb_adm class

  switch (theSystem->message1)
  { case AOBJ_READ:
      return adm.CheckObjectVersion(datbObj, theSystem);
    case AOBJ_ASSIGNOBJECT:
      return adm.AssignObject(datbObj, theSystem);
    case AOBJ_CREATED:
      return adm.CreateObject(datbObj);
    case AOBJ_EXPORTOBJECT:
      return adm.ExportObject(datbObj);
    case AOBJ_IMPORTOBJECT:
      return adm.ImportObject(datbObj);
    case AOBJ_VALIDATEOBJECTIDS:
      return adm.ValidateObjects(datbObj, theSystem);
    case AOBJ_CHECKOBJECT:
      return adm.CheckObjects(datbObj);
    case AOBJ_EDITOBJECT:
      return adm.EditObject(datbObj);
    default:
      break;
  } // switch
  return A_NOTHANDLED;
} // ODatbAdministrationRoutine

long DATB_ADM::CheckObjectVersion /* -- Check Object Version ------------ */
( AOBJECTID	datbObj,		// datb object
  pAObjMessage	theSystem		// system pointer
)
/* Checks the object version. */
{ MemBlock	mb;			// memblock class

  theSystem->message3 = CURRDATBVER;
  if (*(int *) mb.GetPointer(datbObj, DATBCORE) > CURRDATBVER)
  { AObjReportError(datbObj, theSystem->message1, A_ERROR,
		    "Database\tALM_DATB.DLL", CURRDLLVER);
    return A_ERROR;
  } // if
  return A_OK;
} // CheckObjectVersion

long DATB_ADM::AssignObject	/* ---- Assign Object ------------------- */
( AOBJECTID	destObj,		// destination object
  pAObjMessage	theSystem		// system pointer
)
/* Assigns the source object to the destination object at run-time. */
{ AOBJECTID	srcObj;			// source object
  void		*srcData;		// source datablock
  void		*destData;		// destination datablock
  int		dataCount;		// number of datablocks
  long		dataSize;		// datablock size

  int		i;			// counter

  srcObj = (AOBJECTID) theSystem->message3;// source object

  if (srcObj == destObj)
    return A_OK;			// no need to assign
  if (AObjGetType(srcObj) != OTYPE_DATABASE ||
      AObjGetType(destObj) != OTYPE_DATABASE)
    return A_ERROR;			// incompatibale object types

  /* ---- copies datablocks ---- */
  { MemBlock	mb;			// memblock class

    dataCount = (int) AObjGetDataCount(srcObj);
    if (!AObjSetDataCount(destObj, dataCount))
      return A_ERROR;
    for(i = 0; i < dataCount; i++)
    { dataSize = AObjGetDataSize(srcObj, i);
      if (!AObjResizeData(destObj, i, dataSize))
	return A_ERROR;
      if (dataSize)
      {	if ((srcData = (void *) mb.GetPointer(srcObj, i)) == 0)
	  return A_ERROR;
	if ((destData = (void *) mb.GetPointer(destObj, i)) == 0)
	  return A_ERROR;
      } // if
      CopyMemory(destData, srcData, dataSize);
    } // for
  } // scope level-1

  /* ---- inits datablocks private values ---- */
  { MemBlock	mb;			// memblock class
    DatbCore	*datbCore;		// datb core info
    int		fieldCount;		// number of fields
    Field	*field;			// an array of fields

    /* ---- inits datablock DATBCORE ---- */
    if (AObjGetDataSize(destObj, DATBCORE) != sizeof(DatbCore) ||
	(datbCore = (DatbCore *) mb.GetPointer(destObj, DATBCORE)) == 0)
      return A_ERROR;

    datbCore->mode	 = MODE_ALL;	// all mode

    datbCore->foundCount = 0;		// number of record in found list
    datbCore->found	 = 0;           // found list (an array of rec IDs)
    datbCore->foundIdx	 = 0;           // current found index

    datbCore->fileHdl	 = INVALID_HANDLE_VALUE;

    /* ---- inits datablock DATBFIELDS ---- */
    if ((fieldCount = (int) (AObjGetDataSize(destObj, DATBFIELDS) /
			     sizeof(Field))) != 0)
      if ((field = (Field *) mb.GetPointer(destObj, DATBFIELDS)) == 0)
	return A_ERROR;

    for (i = 0; i < fieldCount; i++)
      field[i].keyVal = (char *) MBAlloc(field[i].keySize);
  } // scope level-1
  return A_OK;
} // AssignObject

long DATB_ADM::CreateObject	/* ---- Create Object ------------------- */
( AOBJECTID	datbObj			// datb object
)
/* Creates a new object at design-time. */
{ MemBlock	mb;			// memblock class
  Datb		*datb;			// datb core info

  if ((AObjSetDataCount(datbObj, DATBCOUNT)) == 0)
    return A_ERROR;

  /* ---- datablock DATBCORE ---- */
  if (!AObjResizeData(datbObj, DATBCORE, sizeof(Datb)))
    return A_ERROR;
  if ((datb = (Datb *) mb.GetPointer(datbObj, DATBCORE)) == 0)
    return A_ERROR;

  datb->version = CURRDATBVER;		// current version
  datb->isRunTime = FALSE;		// is run time?

  lstrcpy(datb->defFileExt, DEF_FILEEXT); // default file ext
  datb->recIDObj = 0;			// no record id object

  memset(datb->spares, 0, sizeof(datb->spares)); // inits unused spares

  /* ---- datablock DATBFIELDS ---- */
  if (!AObjResizeData(datbObj, DATBFIELDS, 0)) // no fields
    return A_ERROR;
  return A_OK;
} // CreateObject

long DATB_ADM::ExportObject	/* ---- Export Object ------------------- */
( AOBJECTID	datbObj			// datb object
)
/* Exports the object. */
{ MemBlock	mb;			// memblock class
  Datb		*datb;			// datb core info
  int		fieldCount;		// number of fields
  Field		*field;			// an array of fields
  int		keyCount;		// number of keys
  int		keyIdx;			// key index

  int		i, j;			// counters

  /* ---- datablock DATBCORE ---- */
  if ((datb = (Datb *) mb.GetPointer(datbObj, DATBCORE)) == 0)
    return A_ERROR;

  AUpsfExportField("VERSION", 0, AUPSF_NATV_INT,
    AUPSF_UNIV_INTEGER, &datb->version, sizeof(datb->version));

  AUpsfExportField("FILEEXT", 0, AUPSF_NATV_STRING,
    AUPSF_UNIV_TEXT, datb->defFileExt, lstrlen(datb->defFileExt) + 1);

  AUpsfExportField("RECIDOBJ", 0, AUPSF_NATV_OBJECTREF,
    AUPSF_UNIV_OBJECTREF, &datb->recIDObj, sizeof(datb->recIDObj));

  /* ---- datablock DATBFIELDS ---- */
  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
      return A_ERROR;

  /* ---- field info ---- */
  AUpsfExportField("OBJCNT", 0, AUPSF_NATV_INT,
    AUPSF_UNIV_INTEGER, &fieldCount, sizeof(fieldCount));
  for (keyCount = 0, i = 0; i < fieldCount; i++)
  { AUpsfExportField("OBJID", i, AUPSF_NATV_OBJECTREF,
      AUPSF_UNIV_OBJECTREF, &field[i].obj, sizeof(field[i].obj));

    AUpsfExportField("FIELDNM", i, AUPSF_NATV_STRING,
      AUPSF_UNIV_TEXT, field[i].name, lstrlen(field[i].name));

    if (field[i].keyType != ODATB_KEYQUERY)
      keyCount++;
  } // for

  /* ---- index info ---- */
  AUpsfExportField("INDEXCNT", 0, AUPSF_NATV_INT,
    AUPSF_UNIV_INTEGER, &keyCount, sizeof(keyCount));
  for (i = 0, j = 0, keyIdx = 1; i < fieldCount; i++, keyIdx++)
    if (field[i].keyType != ODATB_KEYQUERY)
    { AUpsfExportField("FIELDNBR", j, AUPSF_NATV_INT,
        AUPSF_UNIV_INTEGER, &keyIdx, sizeof(int));
      AUpsfExportField("KEYTYPE", j, AUPSF_NATV_SHORT,
	AUPSF_UNIV_INTEGER, &field[i].keyType, sizeof(field[i].keyType));
      AUpsfExportField("KEYLEN", j, AUPSF_NATV_SHORT,
	AUPSF_UNIV_INTEGER, &field[i].keySize, sizeof(field[i].keySize));
      j++;
    } // if
  return A_OK;
} // ExportObject

long DATB_ADM::ImportObject	/* ---- Import Object ------------------- */
( AOBJECTID	datbObj			// datb object
)
/* Imports the object. */
{ MemBlock	mb;			// memblock class
  Datb		*datb;			// datb core info
  int		fieldCount;		// number of fields
  Field		*field;			// an array of fields
  DBKey		dbKey;			// dbkey class
  int		keyCount;		// number of keys
  int		keyIdx;			// key index
  short int	keyType;		// key type
  short int	keySize;		// key size

  StrUtl	s;			// strutl class
  char          str[MXSTRSIZE];		// string

  int		i;			// counter

  /* ---- datablock DATBCORE ---- */
  if ((datb = (Datb *) mb.GetPointer(datbObj, DATBCORE)) == 0)
    return A_ERROR;

  AUpsfImportField("VERSION", 0, AUPSF_NATV_INT,
    &datb->version, sizeof(datb->version));

  AUpsfImportField("FILEEXT", 0, AUPSF_NATV_STRING,
    datb->defFileExt, sizeof(datb->defFileExt)); // optional

  if (AUpsfImportField("RECIDOBJ", 0, AUPSF_NATV_OBJECTREF,
	&datb->recIDObj, sizeof(datb->recIDObj)) != AUPSF_NOERROR)
    AUpsfLogComment("Database: Unable to import 'Record ID Object'.");

  /* ---- datablock FIELDS ---- */
  fieldCount = (int) AUpsfFieldCount("OBJID");
  if ((AObjResizeData(datbObj, DATBFIELDS, fieldCount * sizeof(Field))) == 0)
    return A_ERROR;
  if (fieldCount)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
      return A_ERROR;

  /* ---- field info ---- */
  for (i = 0; i < fieldCount; i++)
  { if (AUpsfImportField("OBJID", i, AUPSF_NATV_OBJECTREF,
	  &field[i].obj, sizeof(&field[i].obj)) != AUPSF_NOERROR)
    { AUpsfLogComment("Database: Unable to import an object of "
		      " 'Object in Record'.");
      field[i].obj = 0;
      continue;
    } // if

    if (AUpsfImportField("FIELDNM", i, AUPSF_NATV_STRING,
	  str, sizeof(str)) != AUPSF_NOERROR)
    { AUpsfLogComment("Database: Unable to import a field name of "
		      "'Field Name'.  (Default value has been set.)");
      lstrcpy(field[i].name, DEF_VARNAME);
    } // if
    else
    { s.TrimVar(str);			// trims invalid characters
      if (*str == 0)
	lstrcpy(field[i].name, DEF_VARNAME);
      else
      {	str[MXVARSIZE - 1] = '\0';	// truncates
	lstrcpy(field[i].name, str);
      } // else
    } // else

    /* ---- inits REFOBJ, KEYTYPE, KEYSIZE, and KEYVAL ---- */
    field[i].refObj = 0;
    field[i].keyType = ODATB_KEYQUERY;
    field[i].keySize = dbKey.GetKeySize(field[i].keyType);
  } // for

  /* ---- index info ---- */
  if (AUpsfImportField("INDEXCNT", 0, AUPSF_NATV_INT,
	&keyCount, sizeof(keyCount)) != AUPSF_NOERROR)
  { AUpsfLogComment("Database: Unable to import index field info.");
    keyCount = 0;
  } // if

  for (i = 0; i < keyCount; i++)
  { if (AUpsfImportField("FIELDNBR", i, AUPSF_NATV_INT,
	  &keyIdx, sizeof(int)) != AUPSF_NOERROR)
    { AUpsfLogComment("Database: Unable to import an index field info.");
      continue;
    } // if
    if (keyIdx < 1 || keyIdx > fieldCount)
    { AUpsfLogComment("Database: Unable to import an index field info.");
      continue;
    } // if

    if (AUpsfImportField("KEYTYPE", i, AUPSF_NATV_SHORT,
	  &keyType, sizeof(keyType)) != AUPSF_NOERROR)
      if ((keyType = dbKey.GetKeyType(datbObj, field[keyIdx - 1].obj,
		       field[keyIdx - 1].type)) == ODATB_KEYQUERY)
      { AUpsfLogComment("Database: Unable to import an index field info.");
	continue;
      } // if

    if (AUpsfImportField("KEYLEN", i, AUPSF_NATV_SHORT,
	  &keySize, sizeof(keySize)) != AUPSF_NOERROR)
      keySize = dbKey.GetKeySize(keyType);
    if (keySize < 1)
      keySize = 1;
    else
      if (keySize > ODATB_MXKEYSIZE)
	keySize = ODATB_MXKEYSIZE;

    /* ---- assigns KEYTYPE, and KEYSIZE ---- */
    field[keyIdx - 1].keyType = keyType;
    field[keyIdx - 1].keySize = keySize;
  } // for
  return A_OK;
} // ImportObject

long DATB_ADM::ValidateObjects	/* ---- Validate Objects ---------------- */
( AOBJECTID	datbObj,		// datb object
  pAObjMessage	theSystem		// the system
)
/* Validates all the objects stored in the object's datablocks. */
{ MemBlock	mb;			// memblock class
  Datb		*datb;			// datb core info
  int		fieldCount;		// number of fields
  Field		*field;			// an array of fields

  int		i;			// counter

  /* ---- datablock DATBCORE ---- */
  if ((datb = (Datb *) mb.GetPointer(datbObj, DATBCORE)) == 0)
    return A_ERROR;
  AObjValidateID(&datb->recIDObj, theSystem);

  /* ---- datablock DATBFIELDS ---- */
  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
      return A_ERROR;
  for (i = 0; i < fieldCount; i++)
    AObjValidateID(&field[i].obj, theSystem);
  return A_OK;
} // ValidateObjects

long DATB_ADM::CheckObjects	/* ---- Check Objects ------------------- */
( AOBJECTID	datbObj			// datb object
)
/* Checks all the objects stored in the object's datablocks. */
{ Datb		*datb;			// datb core info
  int		fieldCount;		// number of fields
  Field		*field;			// an array of fields

  int		i, j;			// counters

  { MemBlock	mb;			// memblock class

    /* ---- datablock DATBCORE ---- */
    if ((datb = (Datb *) mb.GetPointer(datbObj, DATBCORE)) == 0)
      return A_ERROR;
    if (datb->recIDObj)
      if (!AObjCheckType(datb->recIDObj, OTYPE_NUMBER))
	datb->recIDObj = 0;

    /* ---- datablock DATBFIELDS ---- */
    fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
                        sizeof(Field));
    field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);
    for (j = 0, i = fieldCount - 1; i >= 0; i--)
      if ((field[i].type = AObjGetType(field[i].obj)) == 0)
      { j++;
	CopyMemory(&field[i], &field[i + 1],
		(fieldCount - i - j) * sizeof(Field));
      } // if
  } // scope level-1
  if (j)
    AObjResizeData(datbObj, DATBFIELDS, (fieldCount - j) * sizeof(Field));
  return A_OK;
} // CheckObjects

long DATB_ADM::EditObject	/* ---- Edit Object --------------------- */
( AOBJECTID	datbObj			// datb object
)
/* Edits the object. */
{
  return DialogBoxParam(hInstance, "Database", GetActiveWindow(),
	   ODatbObjectDialog, (LPARAM) datbObj)? A_MODIFIED : A_OK;
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

