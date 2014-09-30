/*                                                                        *\
    (c) 1993-95, Novell, Inc.  All rights reserved.

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
 | FILE		: DATB_FNC.CPP                                            |
 | DESCRIPTION	: Database Object Functions and Methods.                  |
 | COPYRIGHT	: 1993-95 Novell, Inc.                                    |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit.                        |
 |   v1.20.00	  mw	23-Mar-1995  Spill part of them into datb_mth.cpp.|
\*------------------------------------------------------------------------*/
#define STRICT
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <dos.h>
#include <errno.h>
#include <commdlg.h>

#include "datb_fnc.h"
#include "memblock.h"
#include "safemem.h"
#include "strutl.h"
#include "dbkey.h"
#include "btree.h"
#include "cmpfunc.h"
#include "macrotrn.h"
#include "macroexe.h"


char lpstrFilter[] = "ODB Files (*.ODB)\0*.odb\0All Files (*.*)\0*.*\0\0";

long GetFieldSize(OBJECTID, TYPEID);
/* Returns the size of field.
   Note: Each field has field type and the number of data, and
	 each data has data size and the data itself.
*/

long GetRecordSize(int, Field *);
/* Returns the size of record.
   Note: Each record has the number of field, the offset of each field, and
	 the field.
*/

long GetRecordSize(int, OBJECTID *);
/* Returns the size of record.
   Note: Each record has the number of field, the offset of each field, and
	 the field.
*/


long GetFieldSize		/* ---- Get Field Size -------------------- */
( OBJECTID		object,		// object
  TYPEID		type		// type
)
/* Returns the size of field.
   Note: Each field has field type and the number of data, and
	 each data has data size and the data itself.
*/
{ long			fieldSize;	// field size
  int			dataCount;	// number of fields

  int			i;		// counter

  switch ((long) type)
  { case OTYPE_TEXT:
      return 3 * sizeof(long) + OTextGetLength(object) + 1;
    case OTYPE_NUMBER:
      return 3 * sizeof(long) + (ONmbrGetType(object) == ONMBR_INTEGER?
				   sizeof(long) : sizeof(long double));
    case OTYPE_DATE:
      return 3 * sizeof(long) + sizeof(long);
    case OTYPE_TIME:
      return 3 * sizeof(long) + sizeof(long);
    default:
      break;
  } // switch
  dataCount = (int) AObjGetDataCount(object);
  fieldSize = (2 + dataCount) * sizeof(long);
  for (i = 0; i < dataCount; i++)
    fieldSize += AObjGetDataSize(object, i);
  return fieldSize;
} // GetFieldSize

long GetRecordSize		/* ---- Get Record Size ------------------- */
( int		fieldCount,		// number of fields
  Field		*field			// an array of FIELDs
)
/* Returns the size of record.
   Note: Each record has the number of field, the offset of each field, and
	 the field.
*/
{ long		recSize;		// record size

  int		i;			// counter

  recSize = (1 + fieldCount) * sizeof(long);
  for (i = 0; i < fieldCount; i++)
    recSize += GetFieldSize(field[i].obj, field[i].type);
  return recSize;
} // GetRecordSize

long GetRecordSize		/* ---- Get Record Size ------------------- */
( int		fieldCount,		// number of fields
  OBJECTID	*fieldObj		// an array of OBJECTIDs
)
/* Returns the size of record.
   Note: Each record has the number of field, the offset of each field, and
	 the field.
*/
{ long		recSize;		// record size

  int		i;			// counter

  recSize = (1 + fieldCount) * sizeof(long);
  for (i = 0; i < fieldCount; i++)
    recSize += GetFieldSize(fieldObj[i], AObjGetType(fieldObj[i]));
  return recSize;
} // GetRecordSize

BOOL CreateFile			/* ---- Create File ----------------------- */
( char			*fileName,	// file name
  int			fileVer,	// file version
  int			fieldCount,	// number of FIELDs
  Field			*field		// an array of FIELDs
)
/* Returns TRUE if Object Database File is created, else returns FALSE. */
{ int			idxCount;	// number of index field
  long			*objType;	// an array of TYPEIDs

  int			fileHdl;	// file handle
  OFSTRUCT		lpReOpenBuff;	// OpenFile OFSTRUCT
  FileID		fileID;		// file ID
  Lock			lock;		// lock
  FileHdr		fileHdr;	// file header
  IdxHdr		idxHdr;		// index header

  int			i;		// counter

  /* ---- calculates number of indexes ---- */
  for (idxCount = 0, i = 0; i < fieldCount; i++)
    if (field[i].keyType)
      idxCount++;

  fileHdl = OpenFile(fileName, &lpReOpenBuff, OF_CREATE | OF_SHARE_EXCLUSIVE);
  if (fileHdl == -1)
    return FALSE;

  /* ---- un-inits file id ---- */
  hmemset(&fileID, 0, sizeof(FileID));
  if (_lwrite(fileHdl, (char *) &fileID, sizeof(FileID)) == 65535U)
  { _lclose(fileHdl);
    return FALSE;
  } // if

  /* ---- inits the lock bytes ---- */
  hmemset(&lock, 0, sizeof(Lock));
  if (_lwrite(fileHdl, &lock, sizeof(Lock)) == 65535U)
  { _lclose(fileHdl);
    return FALSE;
  } // if

  /* ---- inits file header ---- */
  hmemset(&fileHdr, 0, sizeof(FileHdr));
  fileHdr.Fields = fieldCount;
  fileHdr.Indexes = idxCount;
  fileHdr.recStart = sizeof(FileID) + sizeof(Lock) + sizeof(FileHdr) +
    (1 + idxCount) * sizeof(IdxHdr) + fieldCount * sizeof(long);
  fileHdr.recStart = fileHdr.nextAvailRec = (fileHdr.recStart % 512) == 0?
    (fileHdr.recStart / 512) * 512 : (fileHdr.recStart / 512 + 1) * 512;
  if (_lwrite(fileHdl, (char *) &fileHdr, sizeof(FileHdr)) == 65535U)
  { _lclose(fileHdl);
    return FALSE;
  } // if

  /* ---- inits index header ---- */
  idxHdr.root = idxHdr.leftLeaf = idxHdr.rightLeaf = 0;
  idxHdr.field = 0;
  idxHdr.keyType = ODATB_KEYLONG;
  idxHdr.keyLen = sizeof(long);
  if (_lwrite(fileHdl, (char *) &idxHdr, sizeof(IdxHdr)) == 65535U)
  { _lclose(fileHdl);
    return FALSE;
  } // if
  for (i = 0; i < fieldCount; i++)
    if (field[i].keyType)
    { idxHdr.field = i + 1;
      idxHdr.keyType = field[i].keyType;
      idxHdr.keyLen = field[i].keySize;
      if (_lwrite(fileHdl, (char *) &idxHdr, sizeof(IdxHdr)) == 65535U)
      { _lclose(fileHdl);
	return FALSE;
      } // if
    } // if

  /* ---- inits object types ---- */
  if (fieldCount)
  { objType = (long *) MBAlloc(fieldCount * sizeof(*objType));
    for (i = 0; i < fieldCount; i++)
      objType[i] = (long) field[i].type;
    if (_lwrite(fileHdl, (char *) objType, fieldCount * sizeof(*objType)) ==
	65535U)
    { _lclose(fileHdl);
      return FALSE;
    } // if
    MBFree(objType);
  } // if

  /* ---- inits file ID ---- */
  _llseek(fileHdl, 0, 0);
  lstrcpy(fileID.fileID, "Object Database File\32\0");
  fileID.version = fileVer;
  if (_lwrite(fileHdl, (char *) &fileID, sizeof(FileID)) == 65535U)
  { _lclose(fileHdl);
    return FALSE;
  } // if
  return _lclose(fileHdl) == -1? FALSE : TRUE;
} // CreateFile

BOOL OpenFile			/* ---- Open File ------------------------- */
( DatbCore		*datbCore,	// datb core info
  int			fieldCount,	// number of FIELDs
  Field			*field		// an array of FIELDs
)
/* Returns TRUE if the Object Database File is opened, else returns FALSE. */
{ int			idxCount;	// number of indexes
  long			*objType;	// an array of TYPEIDs

  BTree			file;		// btree class
  FileID		fileID;		// file id
  FileHdr		fileHdr;	// file header
  IdxHdr		idxHdr;		// index header
  char			error[MXSTRSIZE];// error message

  int			i;		// counter

  /* ---- calculates number of indexes ---- */
  for (idxCount = 0, i = 0; i < fieldCount; i++)
    if (field[i].keyType)
      idxCount++;

  /* ---- opens file ---- */
  if ((datbCore->fileHdl = _lopen(datbCore->pathName, OF_READWRITE |
                                  OF_SHARE_DENY_NONE)) == -1)
    if ((datbCore->fileHdl = _lopen(datbCore->pathName, OF_READ |
                                    OF_SHARE_DENY_NONE)) == -1)
    { datbCore->fileAccessMode  = 0;                               
      return FALSE;
    } // if
    else
      datbCore->fileAccessMode = OF_READ;
  else
    datbCore->fileAccessMode = OF_READWRITE;

  if (!file.IsShare(datbCore->fileHdl))
  { _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "SHARE.EXE has to be be loaded before running this application.",
      "Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if

  /* ---- verifies Object Database File ---- */
  if (_lread(datbCore->fileHdl, (char *) &fileID, sizeof(fileID)) == 65535U)
  { _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "This is not an Object Database File.",
      "Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if
  if (lstrcmp(fileID.fileID, "Object Database File\32\0") &&
      lstrcmp(fileID.fileID, "Serius Database File\32\0"))
  { _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "This is not an Object Database File.",
      "Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if

  /* ---- verifies version of Object Database File ---- */
  if (fileID.version != datbCore->version)
  { _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "Invalid version of Object Database File.",
      "Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if

  /* ---- verifies number of indexes in the file to database object ---- */
  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);
  if (fileHdr.Indexes != idxCount)
  { _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "The number of index key fields don't match.",
      "Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if

  /* ---- verifies indexes in file to database object ---- */
  if (_lread(datbCore->fileHdl, (char *) &idxHdr, sizeof(IdxHdr)) == 65535U)
  { _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "Couldn't open this database file.",
      "File Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if
  if (idxHdr.field != 0 ||
      idxHdr.keyType != ODATB_KEYLONG || idxHdr.keyLen != sizeof(long))
  { _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "Couldn't open this database file.",
      "File Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if
  for (i = 0; i < idxCount; i++)
  { if (_lread(datbCore->fileHdl, (char *) &idxHdr, sizeof(IdxHdr)) == 65535U)
    { _lclose(datbCore->fileHdl);
      datbCore->fileHdl = -1;
      MessageBox(GetActiveWindow(), "Database: "
	"Couldn't open this database file.",
	"File Error", MB_ICONINFORMATION | MB_OK);
      return FALSE;
    } // if
    if (idxHdr.field < 1 || idxHdr.field > fieldCount + 1)
    { _lclose(datbCore->fileHdl);
      datbCore->fileHdl = -1;
      MessageBox(GetActiveWindow(), "Database: "
	"Couldn't open this database file.",
	"Error", MB_ICONINFORMATION | MB_OK);
      return FALSE;
    } // if

    if (idxHdr.keyType != field[(int) (idxHdr.field - 1)].keyType)
    { _lclose(datbCore->fileHdl);
      datbCore->fileHdl = -1;
      MessageBox(GetActiveWindow(), "Database: "
	"Index key types don't match.",
	"Error", MB_ICONINFORMATION | MB_OK);
      return FALSE;
    } // if
    if (idxHdr.keyLen != field[(int) (idxHdr.field - 1)].keySize)
    { _lclose(datbCore->fileHdl);
      datbCore->fileHdl = -1;
      MessageBox(GetActiveWindow(), "Database: "
	"Index key lengths don't match.",
	"Error", MB_ICONINFORMATION | MB_OK);
      return FALSE;
    } // if
  } // for

  /* ---- verifies number of fields in file to database object ---- */
  if (fileHdr.Fields != fieldCount)
  { wsprintf(error, "Database: The file was initially created for %ld fields."
      "  This database object has %d fields", fileHdr.Fields, fieldCount);
    MessageBox(GetActiveWindow(), error,
      "Warning", MB_ICONINFORMATION | MB_OK);
    if (fileHdr.Fields > fieldCount)
      fileHdr.Fields = fieldCount;
  } // if

  /* ---- verifies field types in file to database object ---- */
  objType = (long *) MBAlloc(fileHdr.Fields * sizeof(*objType));
  if ((_lread(datbCore->fileHdl, (char *) objType,
	      (int) fileHdr.Fields * sizeof(*objType))) == 65535U)
  { MBFree(objType);
    _lclose(datbCore->fileHdl);
    datbCore->fileHdl = -1;
    MessageBox(GetActiveWindow(), "Database: "
      "Couldn't open this databasse file.",
      "File Error", MB_ICONINFORMATION | MB_OK);
    return FALSE;
  } // if
  for (i = 0 ; i < fileHdr.Fields; i++)
    if (objType[i] != (long) field[i].type)
    { wsprintf(error, "Database: Object types in Field %d do not match",
	       i + 1);
      MessageBox(GetActiveWindow(), error,
	"Warning", MB_ICONINFORMATION | MB_OK);
    } // if
  MBFree(objType);

  /* ---- inits values ---- */
  datbCore->accessMode = FALSE;
  datbCore->recPtr = 0;
  datbCore->recID = 0;
  datbCore->isRecLock = FALSE;
  return TRUE;
} // OpenFile

BOOL CloseFile			/* ---- Close File ------------------------ */
( DatbCore		*datbCore	// datb core info
)
/* Returns TRUE if the current file is close, else returns FALSE. */
{
  if (datbCore->fileHdl > -1)
  { UnlockRecord(datbCore);

    if (_lclose(datbCore->fileHdl) == -1)
      return FALSE;
    datbCore->fileHdl = -1;
  } // if
  return TRUE;
} // CloseFile

void DBPostEvent		/* ---- Database Post Event --------------- */
( DatbCore		*datbCore	// database info
)
/* Database Function posts an event. */
{
  if (datbCore->recIDObj)
  { ONmbrSetInteger(datbCore->recIDObj, datbCore->recID, TRUE);
    AEvtPostStandard(datbCore->recIDObj, AEVENT_OBJECTCHANGED,
		     0, AEVT_KILLDUPLICATES);
  } // if
} // DBPostEvent

void DBReportResult		/* ---- Database Report Result ------------ */
( BOOL			isFunction,	// is function?
  pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem,	// system pointer
  int			flowNo,		// flow number
  char			*report		// report
)
/* Database object reports result. */
{
  if (!isFunction)
  { ((ODatbMethodParams *) theEvent->details)->flowIdx = flowNo;

    ADBG_ASSERT_E(flowNo, return, report);
  } // if
  else
    AFuncReportResult(theSystem, flowNo, report);
} // DBReportResult

BOOL LockRecord			/* ---- Lock Record ----------------------- */
( long			recPtr,		// record pointer
  DatbCore		*datbCore	// datb core info
)
/* Returns TRUE if record RECORDPTR is lock, else returns FALSE. */
{ BTree			file;		// btree class

  if (datbCore->isRecLock && datbCore->recPtr == recPtr)
    return TRUE;
  if (!datbCore->accessMode)		// the file is not lock
  { if (file.LockRec(datbCore->fileHdl, recPtr) == FALSE)
      return FALSE;
    if (datbCore->isRecLock)
      file.UnlockRec(datbCore->fileHdl, datbCore->recPtr); // UnlockRecord
  } // if
  datbCore->isRecLock = TRUE;
  datbCore->recPtr = recPtr;
  return TRUE;
} // LockRecord

BOOL LockRecord			/* ---- Lock Record ----------------------- */
( DatbCore		*datbCore	// datb core info
)
/* Returns TRUE or FALSE, as the result of locking the current record. */
{ BTree			file;		// btree class

  if (datbCore->isRecLock)
    return TRUE;
  return datbCore->isRecLock = datbCore->accessMode?
	   TRUE : file.LockRec(datbCore->fileHdl, datbCore->recPtr);
} // LockRecord

void UnlockRecord		/* ---- Unlock Record --------------------- */
( DatbCore		*datbCore	// datb core info
)
/* Unlocks the current record. */
{ BTree			file;		// btree class

  if (datbCore->isRecLock)
  { if (!datbCore->accessMode)		// the file is not lock
      file.UnlockRec(datbCore->fileHdl, datbCore->recPtr);
    datbCore->isRecLock = FALSE;
  } // if
} // UnlockRecord

OBJECTID ReadObject		/* ---- Read Object ----------------------- */
( int			fileHdl,	// file handle
  long			recPtr,		// record pointer
  int			objPos		// object position (0-origin)
)
/* Returns the newly read object in record, or return 0. */
{ OBJECTID		obj;		// object

  MemBlock		mb;		// memblock class
  MEMBLOCKID		tempMemBlock;	// temporary memory block
  char			*data;		// data
  char			str[32];	// string

  BTree			file;		// btree class
  long			recOffset;	// record offset
  long			recFieldCount;	// number of record fields
  ObjHdr		objHdr;		// object header
  long			dataSize;	// data block size

  int			i;		// counter

  /* ---- reads the record header ---- */
  file.ReadRec(fileHdl, recPtr, 0,
	       sizeof(recFieldCount), (char *) &recFieldCount);
  if (objPos >= recFieldCount)
    return 0;

  /* ---- reads the field header ---- */
  file.ReadRec(fileHdl, recPtr, sizeof(recFieldCount) + objPos *
	       sizeof(recOffset), sizeof(recOffset), (char *) &recOffset);
  file.ReadRec(fileHdl, recPtr, recOffset, sizeof(objHdr), (char *) &objHdr);
  recOffset += sizeof(objHdr);

  /* ---- creates the object ---- */
  if ((obj = AObjCreate((TYPEID) objHdr.objType)) == 0)
    return 0;

  /* ---- reads the object ---- */
  switch (objHdr.objType)
  { case OTYPE_TEXT:
      file.ReadRec(fileHdl, recPtr, recOffset,
		   sizeof(dataSize), (char *) &dataSize);
      recOffset += sizeof(dataSize);

      tempMemBlock = mb.Allocate(dataSize);
      data = (char *) mb.GetPointer(tempMemBlock);
      file.ReadRec(fileHdl, recPtr, recOffset, dataSize, data);

      AObjConvertFromText(obj, tempMemBlock);
      break;
    case OTYPE_NUMBER:
      file.ReadRec(fileHdl, recPtr, recOffset,
		   sizeof(dataSize), (char *) &dataSize);
      recOffset += sizeof(dataSize);

      file.ReadRec(fileHdl, recPtr, recOffset, dataSize, str);
      if (-objHdr.dataCount == ODATB_KEYLONGDOUBLE)
	ONmbrSetReal(obj, *(long double *) str, TRUE);
      else				// always converts long to long double
	ONmbrSetReal(obj, (long double) (*(long *) str), TRUE);
      break;
    case OTYPE_DATE:
      file.ReadRec(fileHdl, recPtr, recOffset,
		   sizeof(dataSize), (char *) &dataSize);
      recOffset += sizeof(dataSize);

      file.ReadRec(fileHdl, recPtr, recOffset, dataSize, str);
      ODateSetValue(obj, *(long *) str);
      break;
    case OTYPE_TIME:
      file.ReadRec(fileHdl, recPtr, recOffset,
		   sizeof(dataSize), (char *) &dataSize);
      recOffset += sizeof(dataSize);

      file.ReadRec(fileHdl, recPtr, recOffset, dataSize, str);
      OTimeSetValue(obj, *(long *) str);
      break;
    default:
      MemSetObjectDataCount(obj, objHdr.dataCount);

      for (i = 0; i < objHdr.dataCount; i++)
      { /* ---- reads each datablock ---- */
	file.ReadRec(fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	MemResizeObjectData(obj, i, dataSize);
	data = (char *) mb.GetPointer(obj, i);
	file.ReadRec(fileHdl, recPtr, recOffset, dataSize, data);
	recOffset += dataSize;
      } // for
      break;
  } // switch
  return obj;
} // ReadObject

void ReadRecord			/* ---- Read Record ----------------------- */
( long			recPtr,		// record pointer
  OBJECTID		datbObj,	// datb object
  DatbCore		*datbCore,	// datb core info
  int			fieldCount,	// number of FIELDs
  Field			*field		// an array of FIELDs
)
/* Reads the record into the objects, and loads the current keys. */
{ MEMBLOCKID		tempMemBlock;	// temporary memory block
  OBJECTID		tempObj;	// temporary object
  char			*data;		// data block
  char			str[32];	// string
  long			longVal;	// long value
  long double		longDblVal;	// long double value

  BTree			file;		// btree class
  long			recOffset;	// record offset
  long			recFieldCount;	// number of record fields
  ObjHdr		objHdr;	// object header
  long			dataSize;	// data block size
  DBKey			dbKey;		// DBKey class

  int			i, j;		// counters

  /* ---- reads the record header ---- */
  file.ReadRec(datbCore->fileHdl, recPtr, 0,
	       sizeof(recFieldCount), (char *) &recFieldCount);
  if (fieldCount > recFieldCount)
    fieldCount = (int) recFieldCount;

  /* ---- reads the record ---- */
  for (i = 0; i < fieldCount; i++)
  { MemBlock		mb;		// memblock class

    /* ---- reads each field header ---- */
    file.ReadRec(datbCore->fileHdl, recPtr, (i + 1) * sizeof(long),
		 sizeof(recOffset), (char *) &recOffset);
    file.ReadRec(datbCore->fileHdl, recPtr, recOffset,
		 sizeof(objHdr), (char *) &objHdr);
    recOffset += sizeof(ObjHdr);

    /* ---- reads each field ---- */
    switch (objHdr.objType)
    { case OTYPE_TEXT:
	file.ReadRec(datbCore->fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	tempMemBlock = mb.Allocate(dataSize);
	data = (char *) mb.GetPointer(tempMemBlock);
	file.ReadRec(datbCore->fileHdl, recPtr, recOffset, dataSize, data);
	if (field[i].keyType)
	  /* ---- loads the current key ---- */
	  if (dataSize <= field[i].keySize)
	    hmemcpy(field[i].keyVal, data, dataSize);
	  else
	  { hmemcpy(field[i].keyVal, data, field[i].keySize - 1);
	    field[i].keyVal[field[i].keySize - 1] = 0;	// truncates
	  } // else

	AObjConvertFromText(field[i].obj, tempMemBlock);
	break;
      case OTYPE_NUMBER:
	if ((tempObj = mb.Create(OTYPE_NUMBER)) == 0)
	  break;                        // skips

	file.ReadRec(datbCore->fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	file.ReadRec(datbCore->fileHdl, recPtr, recOffset, dataSize, str);
	if (-objHdr.dataCount == ODATB_KEYLONGDOUBLE)
	{ ONmbrSetReal(tempObj, *(long double *) str, TRUE);
	  switch (field[i].keyType)
	  { /* ---- loads the current key ---- */
	    case ODATB_KEYLONG:
	      longVal = (long) *(long double *) str;    // converts value
	      hmemcpy(field[i].keyVal, &longVal, field[i].keySize);
	      break;
	    case ODATB_KEYLONGDOUBLE:
	      hmemcpy(field[i].keyVal, str, field[i].keySize);
	      break;
	  } // switch
	} // if
	else
	{ ONmbrSetInteger(tempObj, *(long *) str, TRUE);
	  switch (field[i].keyType)
	  { /* ---- loads the current key ---- */
	    case ODATB_KEYLONG:
	      hmemcpy(field[i].keyVal, str, field[i].keySize);
	      break;
	    case ODATB_KEYLONGDOUBLE:
	      longDblVal = *(long *) str;		// converts value
	      hmemcpy(field[i].keyVal, &longDblVal, field[i].keySize);
	      break;
	  } // switch
	} // else

	AObjAssign(tempObj, field[i].obj);
	break;
      case OTYPE_DATE:
	if ((tempObj = mb.Create(OTYPE_DATE)) == 0)
	  break;			// skips

	file.ReadRec(datbCore->fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	file.ReadRec(datbCore->fileHdl, recPtr, recOffset, dataSize, str);
	ODateSetValue(tempObj, *(long *) str);
	if (field[i].keyType)
	  /* ---- loads the current key ---- */
	  hmemcpy(field[i].keyVal, str, field[i].keySize);

	AObjAssign(tempObj, field[i].obj);
	break;
      case OTYPE_TIME:
	if ((tempObj = mb.Create(OTYPE_TIME)) == 0)
	  break;			// skips

	file.ReadRec(datbCore->fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	file.ReadRec(datbCore->fileHdl, recPtr, recOffset, dataSize, str);
	OTimeSetValue(tempObj, *(long *) str);
	if (field[i].keyType)
	  /* ---- loads the current key ---- */
	  hmemcpy(field[i].keyVal, str, field[i].keySize);

	AObjAssign(tempObj, field[i].obj);
	break;
      default:
	if ((tempObj = mb.Create((TYPEID) objHdr.objType)) == 0)
	  break;			// skips
	MemSetObjectDataCount(tempObj, objHdr.dataCount);

	for (j = 0; j < objHdr.dataCount; j++)
	{ /* ---- reads each datablock ---- */
	  file.ReadRec(datbCore->fileHdl, recPtr, recOffset,
		       sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  MemResizeObjectData(tempObj, j, dataSize);
	  data = (char *) mb.GetPointer(tempObj, j);
	  file.ReadRec(datbCore->fileHdl, recPtr, recOffset, dataSize, data);
	  recOffset += dataSize;
	} // for
	if (field[i].keyType)
	{ /* ---- loads the current key ---- */
	  data = (char *) dbKey.GetKey(datbObj, field[i].obj,
				       field[i].keyType, field[i].keySize);
	  hmemcpy(field[i].keyVal, data, field[i].keySize);
	} // if

	AObjAssign(tempObj, field[i].obj);
	break;
    } // switch
  } // for
} // ReadRecord

void WriteRecord		/* ---- Write Record ---------------------- */
( long			recPtr,		// record pointer
  OBJECTID		datbObj,	// datb object
  DatbCore		*datbCore,	// datb core info
  int			fieldCount,	// number of FIELDs
  Field			*field		// an array of FIELDs
)
/* Writes the record from the objects, and loads the current keys. */
{ MEMBLOCKID		tempMemBlock;	// temp memory block
  char			*data;		// data block
  long			longVal;	// long value
  long double		longDblVal;	// long double value
  long			dateVal;	// date value
  long			timeVal;	// time value

  BTree			file;		// btree class
  long			recOffset;	// record offset
  long			recFieldCount;	// number of record fields
  long			fieldOffset;	// field offset
  ObjHdr		fieldHdr;	// field header
  long			dataSize;	// data size
  DBKey			dbKey;		// dbkey class

  int			i, j;		// counters

  /* ---- writes the record header ---- */
  recFieldCount = fieldCount;
  file.WriteRec(datbCore->fileHdl, recPtr, 0,
		sizeof(recFieldCount), (char *) &recFieldCount);

  recOffset = sizeof(recFieldCount);
  fieldOffset = sizeof(recFieldCount) + recFieldCount * sizeof(fieldOffset);
  for (i = 0; i < fieldCount; i++)
  { /* ---- writes each field offset ---- */
    file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		  sizeof(fieldOffset), (char *) &fieldOffset);
    recOffset += sizeof(fieldOffset);
    fieldOffset += GetFieldSize(field[i].obj, field[i].type);
  } // for

  /* ---- writes the record ---- */
  for (i = 0; i < fieldCount; i++)
  { MemBlock		mb;		// memblock class

    /* ---- writes each field ---- */
    fieldHdr.objType = (long) field[i].type;

    switch ((long) fieldHdr.objType)
    { case OTYPE_TEXT:
	fieldHdr.dataCount = -ODATB_KEYTEXT;
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	tempMemBlock = mb.Allocate();
	dataSize = OTextGetToMemblock(field[i].obj, tempMemBlock);
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	data = (char *) mb.GetPointer(tempMemBlock);
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset, dataSize, data);
	recOffset += dataSize;
	if (field[i].keyType)
	  /* ---- loads the current key ---- */
	  if (dataSize <= field[i].keySize)
	    hmemcpy(field[i].keyVal, data, dataSize);
	  else
	  { hmemcpy(field[i].keyVal, data, field[i].keySize - 1);
	    field[i].keyVal[field[i].keySize - 1] = 0;	// truncates
	  } // else
	break;
      case OTYPE_NUMBER:
	fieldHdr.dataCount = ONmbrGetType(field[i].obj) == ONMBR_REAL?
			       -ODATB_KEYLONGDOUBLE : -ODATB_KEYLONG;
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	if (ONmbrGetType(field[i].obj) == ONMBR_REAL)
	{ ONmbrGetReal(field[i].obj, &longDblVal);
	  dataSize = sizeof(longDblVal);
	  file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
			sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
			sizeof(longDblVal), (char *) &longDblVal);
	  recOffset += sizeof(longDblVal);

	  switch (field[i].keyType)
	  { /* ---- loads the current key ---- */
	    case ODATB_KEYLONG:
	      longVal = (long) longDblVal;		// converts value
	      hmemcpy(field[i].keyVal, &longVal, field[i].keySize);
	      break;
	    case ODATB_KEYLONGDOUBLE:
	      hmemcpy(field[i].keyVal, &longDblVal, field[i].keySize);
	      break;
	  } // switch
	} // if
	else
	{ longVal = ONmbrGetInteger(field[i].obj);
	  dataSize = sizeof(longVal);
	  file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
			sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
			sizeof(longVal), (char *) &longVal);
	  recOffset += sizeof(longVal);

	  switch (field[i].keyType)
	  { /* ---- loads the current key ---- */
	    case ODATB_KEYLONG:
	      hmemcpy(field[i].keyVal, &longVal, field[i].keySize);
	      break;
	    case ODATB_KEYLONGDOUBLE:
	      longDblVal = longVal;                     // converts value
	      hmemcpy(field[i].keyVal, &longDblVal, field[i].keySize);
	      break;
	  } // switch
	} // else
	break;
      case OTYPE_DATE:
	fieldHdr.dataCount = -ODATB_KEYDATE;
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	dateVal = ODateGetValue(field[i].obj);
	dataSize = sizeof(dateVal);
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(dateVal), (char *) &dateVal);
	recOffset += sizeof(dateVal);

	if (field[i].keyType)
	  /* ---- loads the current key ---- */
	  hmemcpy(field[i].keyVal, &dateVal, field[i].keySize);
	break;
      case OTYPE_TIME:
	fieldHdr.dataCount = -ODATB_KEYTIME;
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	timeVal = OTimeGetValue(field[i].obj);
	dataSize = sizeof(timeVal);
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(timeVal), (char *) &timeVal);
	recOffset += sizeof(timeVal);

	if (field[i].keyType)
	  /* ---- loads the current key ---- */
	  hmemcpy(field[i].keyVal, &timeVal, field[i].keySize);
	break;
      default:
	fieldHdr.dataCount = AObjGetDataCount(field[i].obj);
	file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	for (j = 0; j < fieldHdr.dataCount; j++)
	{ /* ---- writes each datablock ---- */
	  dataSize = AObjGetDataSize(field[i].obj, j);
	  file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
			sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  file.WriteRec(datbCore->fileHdl, recPtr, recOffset,
			dataSize, (char *) mb.GetPointer(field[i].obj, j));
	  recOffset += dataSize;
	} // for

	if (field[i].keyType)
	{ /* ---- loads the current key ---- */
	  data = (char *) dbKey.GetKey(datbObj, field[i].obj,
				       field[i].keyType, field[i].keySize);
	  hmemcpy(field[i].keyVal, data, field[i].keySize);
	} // if
	break;
    } // switch
  } // for
} // WriteRecord

void WriteRecord		/* ---- Write Record ---------------------- */
( int		fileHdl,		// file handle
  long		recPtr,			// record pointer
  int		fieldCount,		// number of FIELDs
  OBJECTID	*fieldObj		// an array of OBJECTIDs
)
/* Writes the record from the objects. */
{ MEMBLOCKID	tempMemBlock;		// temporary memory block
  char		*data;			// data block
  long		longVal;		// long value
  long double	longDblVal;		// long double value
  long		dateVal;		// date value
  long		timeVal;		// time value

  BTree		file;			// btree class
  long		recOffset;		// record offset
  long		recFieldCount;		// number of record fields
  long		fieldOffset;		// field offset
  ObjHdr	fieldHdr;		// field header
  long		dataSize;		// data size

  int		i, j;			// counters

  /* ---- writes the record header ---- */
  recFieldCount = fieldCount;
  file.WriteRec(fileHdl, recPtr, 0,
		sizeof(recFieldCount), (char *) &recFieldCount);

  recOffset = sizeof(recFieldCount);
  fieldOffset = sizeof(recFieldCount) + recFieldCount * sizeof(fieldOffset);
  for (i = 0; i < fieldCount; i++)
  { /* ---- writes each field offset ---- */
    file.WriteRec(fileHdl, recPtr, recOffset,
		  sizeof(fieldOffset), (char *) &fieldOffset);
    recOffset += sizeof(fieldOffset);
    fieldOffset += GetFieldSize(fieldObj[i], AObjGetType(fieldObj[i]));
  } // for

  /* ---- writes the record ---- */
  for (i = 0; i < fieldCount; i++)
  { MemBlock		mb;		// memblock class

    /* ---- writes each field ---- */
    fieldHdr.objType = (long) AObjGetType(fieldObj[i]);

    switch (fieldHdr.objType)
    { case OTYPE_TEXT:
	fieldHdr.dataCount = -ODATB_KEYTEXT;
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	tempMemBlock = mb.Allocate();
	dataSize = OTextGetToMemblock(fieldObj[i], tempMemBlock);
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	data = (char *) mb.GetPointer(tempMemBlock);
	file.WriteRec(fileHdl, recPtr, recOffset, dataSize, data);
	recOffset += dataSize;
	break;
      case OTYPE_NUMBER:
	fieldHdr.dataCount = ONmbrGetType(fieldObj[i]) & ONMBR_REAL?
			       -ODATB_KEYLONGDOUBLE : -ODATB_KEYLONG;
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	if (ONmbrGetType(fieldObj[i]) == ONMBR_REAL)
	{ ONmbrGetReal(fieldObj[i], &longDblVal);
	  dataSize = sizeof(longDblVal);
	  file.WriteRec(fileHdl, recPtr, recOffset,
			sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  file.WriteRec(fileHdl, recPtr, recOffset,
			dataSize, (char *) &longDblVal);
	  recOffset += sizeof(longDblVal);
	} // if
	else
	{ longVal = ONmbrGetInteger(fieldObj[i]);
	  dataSize = sizeof(longVal);
	  file.WriteRec(fileHdl, recPtr, recOffset,
			sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  file.WriteRec(fileHdl, recPtr, recOffset,
			dataSize, (char *) &longVal);
	  recOffset += sizeof(longVal);
	} // else
	break;
      case OTYPE_DATE:
	fieldHdr.dataCount = -ODATB_KEYDATE;
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	dateVal = ODateGetValue(fieldObj[i]);
	dataSize = sizeof(dateVal);
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	file.WriteRec(fileHdl, recPtr, recOffset,
		      dataSize, (char *) &dateVal);
	recOffset += sizeof(dateVal);
	break;
      case OTYPE_TIME:
	fieldHdr.dataCount = -ODATB_KEYTIME;
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	timeVal = OTimeGetValue(fieldObj[i]);
	dataSize = sizeof(timeVal);
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	file.WriteRec(fileHdl, recPtr, recOffset,
		      dataSize, (char *) &timeVal);
	recOffset += sizeof(timeVal);
	break;
      default:
	fieldHdr.dataCount = AObjGetDataCount(fieldObj[i]);
	file.WriteRec(fileHdl, recPtr, recOffset,
		      sizeof(fieldHdr), (char *) &fieldHdr);
	recOffset += sizeof(fieldHdr);

	for (j = 0; j < fieldHdr.dataCount; j++)
	{ /* ---- writes each datablock ---- */
	  dataSize = AObjGetDataSize(fieldObj[i], j);
	  file.WriteRec(fileHdl, recPtr, recOffset,
			sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  file.WriteRec(fileHdl, recPtr, recOffset,
			dataSize, (char *) mb.GetPointer(fieldObj[i], j));
	  recOffset += dataSize;
	} // for
	break;
    } // switch
  } // for
} // WriteRecord


/*--------------------------------------------------------------------------*\
 | FUNCTION	: ODatbStartupFunc                                          |
 | DESCRIPTION	: Database object's constructor.                            |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbStartupFunc
( pAEvtInfo,				// the event
  pAObjMessage				// the system
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  long			objIdx;		// object index

  int			i;		// counter

  objIdx = AOBJ_GETFIRSTKEY;
  while((datbObj = AObjGetNextObject(OTYPE_DATABASE, &objIdx)) != 0)
  { /* ---- datablock DATBCORE ---- */
    MemResizeObjectData(datbObj, DATBCORE, sizeof(DatbCore));
    datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);

    datbCore->isRunTime = TRUE;

    /* inits protected value */
    datbCore->filterIndex = 0;
    lstrcpy(datbCore->pathName, DEF_FILENAME);

    /* inits private value */
    datbCore->mode = MODE_ALL;		// default value

    datbCore->foundCount = 0;		// # of records in found list
    datbCore->found = 0;
    datbCore->foundIdx = 0;

    datbCore->fileHdl = -1;		// file is not open

    /* ---- datablock DATBFIELDS ---- */
    fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
    field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);
    for (i = 0; i < fieldCount; i++)
      if (field[i].keyType)
	field[i].keyVal = (char *) MBAlloc(field[i].keySize);
  } // while
  SetHandleCount(255);
} // ODatbStartupFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION	: ODatbShutdownFunc                                         |
 | DESCRIPTION	: Database object's destructor.                             |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbShutdownFunc
( pAEvtInfo,				// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  long			objIdx;		// object index

  int			i;		// counter

  *theSystem->errorMessage = 0;
  objIdx = AOBJ_GETFIRSTKEY;
  while((datbObj = AObjGetNextObject(OTYPE_DATABASE, &objIdx)) != 0)
  { /* ---- datablock DATBCORE ---- */
    datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);

    if (datbCore->found)
      /* deletes private found-list */
      MBFree(datbCore->found);

    if (!CloseFile(datbCore))
      if (lstrlen(theSystem->errorMessage) +
	  lstrlen("Database: The database file might have been damaged.\r\n")
	  < sizeof(theSystem->errorMessage))
	lstrcat(theSystem->errorMessage,
		"Database: The database file might have been damaged.\r\n");

    /* ---- datablock DATBFIELDS ---- */
    fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
    field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);
    for (i = 0; i < fieldCount; i++)
      if (field[i].keyType)
	MBFree(field[i].keyVal);
  } // while
} // ODatbShutdownFunc

/* ---- Database Functions ---------------------------------------------- */

/*------------------------------------------------------------------------*\
 | FUNCTION	: ODatbAddRecordFunc                                      |
 | DESCRIPTION	: Adds a record to the database file.                     |
 | INPUT 1	: Database.                                               |
 | OUTPUT	: None.                                                   |
 | FLOW 1	: After adding.                                           |
 | FLOW 2	: If error.                                               |
 | Novell, Inc., September 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI ODatbAddRecordFunc
( pAEvtInfo	theEvent,		// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  OBJECTID	datbObj;		// datb object		      (I: 1)
  DatbCore	*datbCore;		// datb core info
  WORD		fieldCount;		// number of fields
  Field		*field;			// an array of fields
  Field		huge *fieldPtr;		// field pointer

  BTree         file;           	// btree class
  FileHdr       fileHdr;        	// file header
  WORD		idx;			// index counter
  IdxHdr	idxHdr;         	// index header
  IdxHdr	idxHdr2;		// index header 2
  long          recPtr;         	// record pointer

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Add Record: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Add Record: Couldn't get the Database input parameter.");
    return;
  } // if
  if (datbCore->fileHdl < 0 || datbCore->fileAccessMode == OF_READ)
  { /* ---- FLOW 2 (If error) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	
    return;
  } // if

  if ((fieldCount = (WORD) (AObjGetDataSize(datbObj, DATBFIELDS) /
			    sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Add Record: Couldn't get the Database input parameter.");
      return;
    } // if

  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);
  fileHdr.LastRecordID++;

  recPtr = file.AllocRec(datbCore->fileHdl, fileHdr,
                         GetRecordSize(fieldCount, field));
  WriteRecord(recPtr, datbObj, datbCore, fieldCount, field);

  /* ---- inserts record id index ---- */
  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  idxHdr2 = idxHdr;
  file.InsertKey(datbCore->fileHdl, fileHdr, idxHdr,
		 (char *) &fileHdr.LastRecordID, recPtr);
  if (memcmp(&idxHdr, &idxHdr2, sizeof(IdxHdr)))
    file.WriteBTreeHdr(datbCore->fileHdl, &idxHdr);

  for (fieldPtr = field, idx = 1; idx <= fileHdr.Indexes; idx++)
  { /* ---- adds field indexes ---- */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idx);
    idxHdr2 = idxHdr;
    file.InsertKey(datbCore->fileHdl, fileHdr, idxHdr,
      (fieldPtr + idxHdr.field - 1)->keyVal, fileHdr.LastRecordID);
    if (memcmp(&idxHdr, &idxHdr2, sizeof(IdxHdr)))
      file.WriteBTreeHdr(datbCore->fileHdl, &idxHdr, idx);
  } // for

  LockRecord(recPtr, datbCore);

  fileHdr.Records++;
  file.WriteFileHdr(datbCore->fileHdl, &fileHdr);
  if (!datbCore->accessMode)            // the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  datbCore->recID = fileHdr.LastRecordID;
  DBPostEvent(datbCore);

  /* ---- FLOW 1 (After adding) ---- */
  DBReportResult(isFunction, theEvent, theSystem);     
} // ODatbAddRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbAppendFoundIDsFunc                                   |
 | DESCRIPTION  : Appends new "\r\n" delimited record IDs onto the end of   |
 |                the Database's private current list of found records.     |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Record IDs                                                |
 | OUTPUT 1     : Count                                                     |
 | FLOW 1       : After appending                                           |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbAppendFoundIDsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  OBJECTID              recIDObj;	// text object			(I: 2)
  MEMBLOCKID		recIDMB;	// record id memory block
  char                  *recIDStr;	// "\r\n" delimited string of recIDs
  OBJECTID		countObj;	// nmbr object			(O: 1)

  long			*recIDPtr;	// record id pointer
  long                  recID;          // record id

  long                  lI;              // counter

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    recIDObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Append Found IDs: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    recIDObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
    if (AObjGetType(recIDObj) != OTYPE_TEXT)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Append Found IDs: Couldn't get the Record IDs input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Append Found IDs: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nAppend Found IDs: Shouldn't be called at design-time.");

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  recIDMB = mb.Allocate();
  OTextGetToMemblock(recIDObj, recIDMB);
  if ((recIDStr = (char *) mb.GetPointer(recIDMB)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Append Found IDs: Couldn't get the Record IDs input parameter.");
    return;
  } // if

  do
  { /* adds new record IDs to found-list */
    if ((recID = strtol(recIDStr, &recIDStr, 10)) > 0)
    { for (lI = 0, recIDPtr = datbCore->found;
	   lI < datbCore->foundCount && *recIDPtr != recID;
	   lI++, recIDPtr++)
	/* Do Nothing */;
      if (lI == datbCore->foundCount)
      { if (datbCore->foundCount % FOUNDBLOCK == 0)
	  datbCore->found = (long *) MBRealloc(datbCore->found,
	    (datbCore->foundCount + FOUNDBLOCK) * sizeof(*datbCore->found));
	*(long *) hmemptr(datbCore->found, datbCore->foundCount,
			  sizeof(*datbCore->found)) = recID;
	datbCore->foundCount++;
      } // if
    } // if
  } while (*recIDStr++ == '\r' && *recIDStr++ == '\n');

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Append Found IDs: Couldn't create the Count output parameter.");
      return;
    } // if
    ONmbrSetInteger(countObj, datbCore->foundCount, TRUE);
    AFuncSetParameter(1, countObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[2] =
      (OBJECTID) datbCore->foundCount;
  } // else
  DBReportResult(isFunction, theEvent, theSystem);	// After appending
} // ODatbAppendFoundIDsFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbCloseDBFunc                                          |
 | DESCRIPTION  : Closes a database file.                                   |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After closing                                             |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbCloseDBFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;        // datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Close Database: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Close Database: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nClose Database: Shouldn't be called at design-time.");

  if (!CloseFile(datbCore))
  { datbCore->fileHdl = -1;		// closes the file anyway

    DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Close Database: The database file might have been damaged.");
    return;
  } // if
  DBReportResult(isFunction, theEvent, theSystem);	// After closing
} // ODatbCloseDBFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbCopyFoundIDsFunc                                     |
 | DESCRIPTION  : Retrieves a copy of the Database's found record list.     |
 |		  The list is a Text object containing "\r\n" delimited     |
 |		  record IDs.                                               |
 | INPUT 1      : Database                                                  |
 | OUTPUT 1     : Record IDs                                                |
 | FLOW 1       : After retrieving                                          |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbCopyFoundIDsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  OBJECTID              recIDObj;	// text object			(O: 1)

  MEMBLOCKID		tempMB;		// temporary memory block
  long			dataSize;	// data size
  char			*data;		// "\r\n" delimited string of rec ids
  int                   strLen;         // string length
  char                  str[32];        // string
  long			*recIDPtr;	// record id pointer

  long                  lI;             // counter

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Copy Found IDs: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Copy Found IDs: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nCopy Found IDs: Shouldn't be called at design-time.");

  /* ---- figures out the size ---- */
  dataSize = 1;				// 0 terminator
  for (lI = 0, recIDPtr = datbCore->found; lI < datbCore->foundCount;
       lI++, recIDPtr++)
  { dataSize += lstrlen(ltoa(*recIDPtr, str, 10));
    if (lI < datbCore->foundCount - 1)
      dataSize += 2;			// "\r\n" deliminator
  } // for

  /* ---- allocates the size ---- */
  tempMB = mb.Allocate(dataSize);
  data = (char *) mb.GetPointer(tempMB);

  /* ---- copies record id ---- */
  for (lI = 0, recIDPtr = datbCore->found; lI < datbCore->foundCount;
       lI++, recIDPtr++)
  { strLen = lstrlen(ltoa(*recIDPtr, str, 10));
    hmemcpy(data, str, strLen);
    data = (char *) data + strLen;
    if (lI < datbCore->foundCount - 1)	// "\r\n" deliminator
    { *data++ = '\r';
      *data++ = '\n';
    } // if
  } // for
  *data = 0;				// 0 terminator

  if ((recIDObj = AObjCreate(OTYPE_TEXT)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Copy Found IDs: Couldn't create the Record IDs output parameter.");
    return;
  } // if
  OTextSetValueFromMemblock(recIDObj, tempMB);

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1, and 2 ---- */
    AFuncSetParameter(1, recIDObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1, and 2 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[1] = recIDObj;
  } // else
  DBReportResult(isFunction, theEvent, theSystem);	// After retrieving
} // ODatbCopyFoundIDsFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbCountRecordsFunc                                     |
 | DESCRIPTION  : Returns the number of records in the database file.       |
 | INPUT 1      : Database                                                  |
 | OUTPUT 1     : Count                                                     |
 | FLOW 1       : After returning                                           |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbCountRecordsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  OBJECTID		countObj;	// nmbr object			(O: 1)

  BTree                 file;           // btree class
  FileHdr               fileHdr;        // file header

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Count Records: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Count Records: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nCount Records: Shouldn't be called at design-time.");

  if (datbCore->fileHdl < 0)
    fileHdr.Records = 0;
  else
  { if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);

    file.ReadFileHdr(datbCore->fileHdl, &fileHdr);

    if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
  } // else

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Count Records: Couldn't create the Count output parameter.");
      return;
    } // if
    ONmbrSetInteger(countObj, fileHdr.Records, TRUE);
    AFuncSetParameter(1, countObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[1] =
      (OBJECTID) fileHdr.Records;
  } // else
  DBReportResult(isFunction, theEvent, theSystem);	// After returning
} // ODatbCountRecordsFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbDBFileNameFunc                                       |
 | DESCRIPTION  : Returns the file name, and path name of the database      |
 |		  file.                                                     |
 | INPUT 1      : Database                                                  |
 | OUTPUT 1     : File Name                                                 |
 | OUTPUT 2     : Path Name                                                 |
 | FLOW 1       : After getting                                             |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbDBFileNameFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  OBJECTID              fileNameObj;	// text object			(O: 1)
  OBJECTID              pathNameObj;	// text object			(O: 2)

  char			str[_MAX_PATH];	// string
  char                  drive[_MAX_DRIVE];	// drive name
  char                  dir[_MAX_DIR];		// directory name
  char                  file[_MAX_FNAME];	// file name
  char                  ext[_MAX_EXT];		// file extension name

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"DB File Name: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "DB File Name: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nDB File Name: Shouldn't be called at design-time.");

  _splitpath(datbCore->pathName, drive, dir, file, ext);

  if ((fileNameObj = AObjCreate(OTYPE_TEXT)) == 0 ||
      (pathNameObj = AObjCreate(OTYPE_TEXT)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "DB File Name: Couldn't create the File Name output parameter.");
    return;
  } // if
  lstrcpy(str, file);
  lstrcat(str, ext);
  OTextSetValueFromBuffer(fileNameObj, str);
  lstrcpy(str, drive);
  lstrcat(str, dir);
  OTextSetValueFromBuffer(pathNameObj, str);

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1, and 2 ---- */
    AFuncSetParameter(1, fileNameObj);
    AFuncSetParameter(2, pathNameObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1, and 2 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[1] = fileNameObj;
    ((ODatbMethodParams *) theEvent->details)->obj[2] = pathNameObj;
  } // else
  DBReportResult(isFunction, theEvent, theSystem);	// After getting
} // ODatbDBFileNameFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbDeleteRecordFunc                                     |
 | DESCRIPTION  : Deletes the current locked record from the database file. |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After deleting                                            |
 | FLOW 2       : If can't delete                                           |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbDeleteRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field                 *field;		// an array of FIELDs

  BTree                 file;           // btree class
  FileHdr               fileHdr;        // file header
  int			idx;		// index counter
  IdxHdr                idxHdr;         // index header
  IdxHdr		idxHdr2;	// index header 2		

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Delete Record: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Delete Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nDelete Record: Shouldn't be called at design-time.");

  fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  if (fieldCount)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Delete Record: Couldn't get the Database input parameter.");
      return;
    } // if

  if (datbCore->fileHdl < 0 || datbCore->fileAccessMode == OF_READ ||
      !datbCore->isRecLock)
  { DBReportResult(isFunction, theEvent, theSystem, 2);	// If can't delete
    return;
  } // if

  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);

  UnlockRecord(datbCore);

  /* deletes record id index */
  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  idxHdr2 = idxHdr;
  file.DeleteKey(datbCore->fileHdl, fileHdr, idxHdr,
		 (char *) &datbCore->recID, datbCore->recPtr);
  if (memcmp(&idxHdr, &idxHdr2, sizeof(IdxHdr)))
    file.WriteBTreeHdr(datbCore->fileHdl, &idxHdr);

  for (idx = 1; idx <= fileHdr.Indexes; idx++)
  { /* deletes field indexes */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idx);
    idxHdr2 = idxHdr;
    file.DeleteKey(datbCore->fileHdl, fileHdr, idxHdr,
		   field[(int) (idxHdr.field - 1)].keyVal, datbCore->recID);
    if (memcmp(&idxHdr, &idxHdr2, sizeof(IdxHdr)))
      file.WriteBTreeHdr(datbCore->fileHdl, &idxHdr, idx);
  } // for

  file.FreeRec(datbCore->fileHdl, fileHdr, datbCore->recPtr);

  fileHdr.Records--;
  file.WriteFileHdr(datbCore->fileHdl, &fileHdr);
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);
  DBReportResult(isFunction, theEvent, theSystem);	// After deleting
} // ODatbDeleteRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbFindRecordFunc                                       |
 | DESCRIPTION  : Finds all records that match the Field Object.  The first |
 |		  found record is loaded into the Database's field objects. |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Field Object                                              |
 | OUTPUT 1     : Count                                                     |
 | FLOW 1       : After finding                                             |
 | FLOW 2       : If none found                                             |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbFindRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info
  int                   fieldCount;     // number of FIELDs
  Field                 *field;		// an array of FIELDs
  OBJECTID              fieldObj;	// field object			(I: 2)
  OBJECTID		countObj;	// nmbr object			(O: 1)
  int			flowNo;		// flow number

  int			keyType;	// key type
  DBKey		        key;		// key
  char			*keyPtr;	// key pointer
  DBKey			currKey;	// DBKey class
  char			*currKeyPtr;	// current key pointer
  OBJECTID		currObj;	// current object

  BTree			file;		// btree class
  int			idx;		// index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // btree index header
  long			recID;		// record id
  long                  recPtr;         // record pointer
  int                   (*cmp)(char *, char *);	// the compare function

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nFind Record: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (!fieldObj)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Record: Couldn't get the Field Object input parameter.");
    return;
  } // if

  idxPos = 1;
  for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
    if (field[idx].keyType)
      idxPos++;
  if (idx == fieldCount)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Record: The Field Object is not in the record.");
    return;
  } // if
  if (!field[idx].keyType &&
      (keyType = key.GetSearchKeyType(datbObj, field[idx].obj)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Record: The Field Object is not searchable.");
    return;
  } // if

  if (datbCore->fileHdl < 0)
  { /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if (datbCore->found)
  { /* ---- removes found list ---- */
    MBFree(datbCore->found);
    datbCore->found = 0;
    datbCore->foundCount = 0;
    datbCore->foundIdx = 0;
  } // if

  if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);

  if (field[idx].keyType)
  { /* ---- searchs record-IDs based on index tree ---- */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    keyPtr = (char *) key.GetKey(datbObj, field[idx].obj,
				 idxHdr.keyType, idxHdr.keyLen);
    cmp = GetCmpPartFunc(idxHdr.keyType);

    recID = file.LocateKey(datbCore->fileHdl, &idxHdr, keyPtr, cmp);
    if (recID)
    { /* ---- builds found list ---- */
      do
      { if (datbCore->foundCount % FOUNDBLOCK == 0)
	  datbCore->found = (long *) MBRealloc(datbCore->found,
	    (datbCore->foundCount + FOUNDBLOCK) * sizeof(*datbCore->found));

	*(long *) hmemptr(datbCore->found, datbCore->foundCount,
			  sizeof(*datbCore->found)) = recID;
	datbCore->foundCount++;

	recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, keyPtr, cmp);
      } while (recID);

      /* finds the first record ID in record ID index. */
      file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
      if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *)
            datbCore->found, GetCmpExactFunc(idxHdr.keyType))) == 0)
      { if (!datbCore->accessMode)	// the file is not lock
	  file.UnlockFileHdr(datbCore->fileHdl);

	DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	  "Find Record: The file has been corrupted.");
	return;
      } // if
      flowNo = LockRecord(recPtr, datbCore)? 1 : 3;
    } // if
    else
      flowNo = 2;
  } // if
  else
  { cmp = GetCmpPartFunc(keyType);

    /* ---- searchs record-IDs based on record-ID tree ---- */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    keyPtr = (char *) key.GetKey(datbObj, field[idx].obj,
				 keyType, ODATB_MXKEYLEN);

    recPtr = file.FirstKey(datbCore->fileHdl, &idxHdr);
    while (recPtr)
    { /* ---- builds found list ---- */
      if ((currObj = ReadObject(datbCore->fileHdl, recPtr, idx)) != 0)
      {	currKeyPtr = (char *) currKey.GetKey(datbObj, currObj, keyType,
					     ODATB_MXKEYLEN);
	if ((*cmp)(keyPtr, currKeyPtr) == 0)
	{ file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
	  if (datbCore->foundCount % FOUNDBLOCK == 0)
	    datbCore->found = (long *) MBRealloc(datbCore->found,
	      (datbCore->foundCount + FOUNDBLOCK) * sizeof(*datbCore->found));

	  *(long *) hmemptr(datbCore->found, datbCore->foundCount,
			    sizeof(*datbCore->found)) = recID;
	  datbCore->foundCount++;
	} // if
	AObjDestroy(currObj);
      } // if
      recPtr = file.NextRecPtr(datbCore->fileHdl);
    } // while

    if (datbCore->foundCount)
    { if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *)
           datbCore->found, GetCmpExactFunc(idxHdr.keyType))) == 0)
      { if (!datbCore->accessMode)	// the file is not lock
	  file.UnlockFileHdr(datbCore->fileHdl);

	DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	  "Find Record: The file has been corrupted.");
	return;
      } // if
      flowNo = LockRecord(recPtr, datbCore)? 1 : 3;
    } // if
    else
      flowNo = 2;
  } // else

  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  if (flowNo == 1)
  { ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);
    datbCore->recID = *datbCore->found;
    DBPostEvent(datbCore);
  } // if

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Record: Couldn't create the Count output parameter.");
    } // if
    ONmbrSetInteger(countObj, datbCore->foundCount, TRUE);
    AFuncSetParameter(1, countObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[2] =
      (OBJECTID) datbCore->foundCount;
  } // else
  DBReportResult(isFunction, theEvent, theSystem, flowNo);// After finding
} // ODatbFindRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbFindExactRecordFunc                                  |
 | DESCRIPTION  : Finds all records that exactly match the field object.    |
 |                The first found record is loaded into the the database's  |
 |                fields objects.                                           |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Field Object                                              |
 | OUTPUT 1     : Count                                                     |
 | FLOW 1       : After finding                                             |
 | FLOW 2       : If none found                                             |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbFindExactRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage          theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info
  int                   fieldCount;     // number of FIELDs
  Field                 *field;		// an array of FIELDs
  OBJECTID              fieldObj;	// field object			(I: 2)
  OBJECTID		countObj;	// nmbr object			(O: 1)
  int			flowNo;		// flow number

  int			keyType;	// key type
  DBKey		        key;		// key
  char			*keyPtr;	// key pointer
  DBKey			currKey;	// DBKey class
  char			*currKeyPtr;	// current key pointer
  OBJECTID		currObj;	// current object

  BTree			file;		// btree class
  int			idx;		// index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // btree index header
  long			recID;		// record id
  long                  recPtr;         // record pointer
  int                   (*cmp)(char *, char *);	// the compare function

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Exact Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Exact Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nFind Exact Record: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Exact Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (!fieldObj)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Exact Record: Couldn't get the Field Object input parameter.");
    return;
  } // if

  idxPos = 1;
  for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
    if (field[idx].keyType)
      idxPos++;
  if (idx == fieldCount)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Exact Record: The Field Object is not in the record.");
    return;
  } // if
  if (!field[idx].keyType &&
      (keyType = key.GetSearchKeyType(datbObj, field[idx].obj)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Exact Record: The Field Object is not searchable.");
    return;
  } // if

  if (datbCore->fileHdl < 0)
  { /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if (datbCore->found)
  { /* ---- removes found list ---- */
    MBFree(datbCore->found);
    datbCore->found = 0;
    datbCore->foundCount = 0;
    datbCore->foundIdx = 0;
  } // if

  if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);

  if (field[idx].keyType)
  { /* ---- searchs record-IDs based on index tree ---- */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    keyPtr = (char *) key.GetKey(datbObj, field[idx].obj,
				 idxHdr.keyType, idxHdr.keyLen);
    cmp = GetCmpExactFunc(idxHdr.keyType);

    recID = file.LocateKey(datbCore->fileHdl, &idxHdr, keyPtr, cmp);
    if (recID)
    { /* ---- builds found list ---- */
      do
      { if (datbCore->foundCount % FOUNDBLOCK == 0)
	  datbCore->found = (long *) MBRealloc(datbCore->found,
	    (datbCore->foundCount + FOUNDBLOCK) * sizeof(*datbCore->found));

	*(long *) hmemptr(datbCore->found, datbCore->foundCount,
			  sizeof(*datbCore->found)) = recID;
	datbCore->foundCount++;

	recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, keyPtr, cmp);
      } while (recID);

      /* find the first record ID in record ID index. */
      file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
      if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *)
            datbCore->found, GetCmpExactFunc(idxHdr.keyType))) == 0)
      { if (!datbCore->accessMode)	// the file is not lock
	  file.UnlockFileHdr(datbCore->fileHdl);

	DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	  "Find Exact Record: The file has been corrupted.");
	return;
      } // if
      flowNo = LockRecord(recPtr, datbCore)? 1 : 3;
    } // if
    else
      flowNo = 2;
  } // if
  else
  { cmp = GetCmpExactFunc(keyType);

    /* ---- searchs record-IDs based on record-ID tree ---- */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    keyPtr = (char *) key.GetKey(datbObj, field[idx].obj,
				 keyType, ODATB_MXKEYLEN);

    recPtr = file.FirstKey(datbCore->fileHdl, &idxHdr);
    while (recPtr)
    { /* ---- builds found list ---- */
      if ((currObj = ReadObject(datbCore->fileHdl, recPtr, idx)) != 0)
      { currKeyPtr = (char *) currKey.GetKey(datbObj, currObj,
					     keyType, ODATB_MXKEYLEN);
	if ((*cmp)(keyPtr, currKeyPtr) == 0)
	{ file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
	  if (datbCore->foundCount % FOUNDBLOCK == 0)
	    datbCore->found = (long *) MBRealloc(datbCore->found,
	      (datbCore->foundCount + FOUNDBLOCK) * sizeof(*datbCore->found));

	  *(long *) hmemptr(datbCore->found, datbCore->foundCount,
			    sizeof(*datbCore->found)) = recID;
	  datbCore->foundCount++;
	} // if
	AObjDestroy(currObj);
      } // if
      recPtr = file.NextRecPtr(datbCore->fileHdl);
    } // while

    if (datbCore->foundCount)
    { if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *)
            datbCore->found, GetCmpExactFunc(idxHdr.keyType))) == 0)
      { if (!datbCore->accessMode)	// the file is not lock
	  file.UnlockFileHdr(datbCore->fileHdl);

	DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	  "Find Exact Record: The file has been corrupted.");
	return;
      } // if
      flowNo = LockRecord(recPtr, datbCore)? 1 : 3;
    } // if
    else
      flowNo = 2;
  } // else

  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  if (flowNo == 1)
  { ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);
    datbCore->recID = *datbCore->found;
    DBPostEvent(datbCore);
  } // if

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Exact Record: Couldn't create the Count output parameter.");
    } // if
    ONmbrSetInteger(countObj, datbCore->foundCount, TRUE);
    AFuncSetParameter(1, countObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[2] =
      (OBJECTID) datbCore->foundCount;
  } // else
  DBReportResult(isFunction, theEvent, theSystem, flowNo);// After finding
} // ODatbFindExactRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbFindRecordIDFunc                                     |
 | DESCRIPTION  : Finds a particular record given its record ID.  The       |
 |		  record is loaded into the Database's field objects.       |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : ID                                                        |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After finding                                             |
 | FLOW 2       : If not found                                              |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., Sepetember 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbFindRecordIDFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;        // datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info
  int			fieldCount;     // number of FIELDs
  Field			*field;         // an array of FIELDs
  OBJECTID              recIDObj;	// nmbr object			(I: 2)
  long                  recID;          // record id

  BTree                 file;           // btree class
  IdxHdr                idxHdr;         // index header
  long			recPtr;		// record pointer

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    recIDObj = AFuncGetTypedParameter(2, OTYPE_NUMBER);
    recID = ONmbrGetInteger(recIDObj);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Record ID: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    recID = (long) ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Record ID: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nFind Record ID: Shouldn't be called at design-time.");

  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Find Record ID: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Record ID) ---- */
  if (recID < 1)
  { /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	// invalid record id
    return;
  } // if

  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *)
        &recID, GetCmpExactFunc(idxHdr.keyType))) == 0)
  {  /* ---- FLOW 2 ---- */
     if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    DBReportResult(isFunction, theEvent, theSystem, 2);// If not found
    return;
  } // if

  if (!LockRecord(recPtr, datbCore))
  { /* ---- FLOW 3 ---- */
    if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    DBReportResult(isFunction, theEvent, theSystem, 3);	// If busy
    return;
  } // if
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);

  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);	// After finding
} // ODatbFindRecordIDFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetFoundRecordFunc                                   |
 | DESCRIPTION  : Loads a found record.  Pass in the Index of the found     |
 |		  record to load.                                           |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Index                                                     |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After getting                                             |
 | FLOW 2       : If error                                                  |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc. September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetFoundRecordFunc
( pAEvtInfo             theEvent,	// event pointer
  pAObjMessage          theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;        // datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info
  int                   fieldCount;     // number of FIELDs
  Field                 *field;         // an array of FIELDs
  OBJECTID              indexObj;	// nmbr object			(I: 2)
  long                  index;		// found-list index number

  BTree                 file;           // btree class
  IdxHdr                idxHdr;         // index header
  long                  recPtr;         // record pointer
  long			recID;		// record id

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    indexObj = AFuncGetTypedParameter(2, OTYPE_NUMBER);
    index = ONmbrGetInteger(indexObj);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Found Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    index = (long) ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Get Found Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nGet Found Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Found Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (index < 1 || index > datbCore->foundCount)
  { /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	// idx is out of range
    return;
  } // if
  recID = *(long *) hmemptr(datbCore->found, index - 1,
			    sizeof(*datbCore->found));

  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	(int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
  { /* ---- FLOW 2 ---- */
    if (!datbCore->accessMode)          // the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    DBReportResult(isFunction, theEvent, theSystem, 2);	// not found
    return;
  } // if

  if (!LockRecord(recPtr, datbCore))
  { /* ---- FLOW 3 ---- */
    if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    DBReportResult(isFunction, theEvent, theSystem, 3);	// If busy
    return;
  } // if
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);

  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);	// After getting
} // ODatbGetFoundRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetNextRecordFunc                                    |
 | DESCRIPTION  : Gets the record that next to the Field Object current     |
 |		  record.  If Field Object is not passed, the next          |
 |		  chronological order record is read.                       |
 | INPUT 1      : Database                                                  |
 | INPUT 2(Opt)	: Field Object                                              |
 | OUPUT        : None                                                      |
 | FLOW 1       : After getting                                             |
 | FLOW 2       : If error                                                  |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetNextRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  OBJECTID              fieldObj;	// field object			[I: 2]

  BTree                 file;           // btree class
  int                   idx;            // index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // index header
  long                  recID;          // record id
  long                  recPtr;         // record pointer

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Next Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Get Next Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nGet Next Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Next Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (fieldObj)
  { idxPos = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
      if (field[idx].keyType)
	idxPos++;
    if (idx == fieldCount)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Next Record: The Field Object is not in the record.");
      return;
    } // if
    if (!field[idx].keyType)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Next Record: The Field Object is not indexed.");
      return;
    } // if
  } // if

  if (!datbCore->accessMode)            // the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  if (fieldObj)
  { /* locates the current key associated with the current record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    if ((recID = file.LocateNext(datbCore->fileHdl, &idxHdr,
	  field[idx].keyVal, datbCore->recID)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
      return;
    } // if

    /* locates the record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	  (int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Next Record: The database file has been corrupted.");
      return;
    } // if
  } // if
  else
  { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateNext(datbCore->fileHdl, &idxHdr,
	  (char *) &datbCore->recID, datbCore->recPtr)) == 0)
    { if (!datbCore->accessMode)          // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
      return;
    } // if
    recID = 0;
    file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
    if (!recID)
    { if (!datbCore->accessMode)	// the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Next Record: The database file has been corrupted.");
      return;
    } // if
  } // else

  if (!LockRecord(recPtr, datbCore))
  { if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    DBReportResult(isFunction, theEvent, theSystem, 3);	// If busy
    return;
  } // if
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);

  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbGetNextRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetPreviousRecordFunc                                |
 | DESCRIPTION  : Gets the record that previous to the Field Object current |
 |		  record.  If Field Object is not passed, the previous      |
 |		  chronological order record is read.                       |
 | INPUT 1      : Database                                                  |
 | INPUT 2(Opt)	: Field object                                              |
 | OUPUT        : None                                                      |
 | FLOW 1       : After getting                                             |
 | FLOW 2       : If error                                                  |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetPreviousRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// and array of FIELDs
  OBJECTID		fieldObj;	// field object			[I: 2]

  BTree                 file;           // btree class
  int                   idx;            // index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // index header
  long                  recID;          // record id
  long                  recPtr;         // record pointer

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Previous Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Get Previous Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nGet Previous Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Previous Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (fieldObj)
  { idxPos = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
      if (field[idx].keyType)
	idxPos++;
    if (idx == fieldCount)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Previous Record: The Field Object is not in the record.");
      return;
    } // if
    if (!field[idx].keyType)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Previous Record: The Field Object is not indexed.");
      return;
    } // if
  } // if

  if (!datbCore->accessMode)            // the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  if (fieldObj)
  { /* locates the current key associated with the current record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    if ((recID = file.LocatePrev(datbCore->fileHdl, &idxHdr,
	  field[idx].keyVal, datbCore->recID)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if

    /* locates the record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	  (int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Previous Record: The database file has been corrupted.");
      return;
    } // if
  } // if
  else
  { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocatePrev(datbCore->fileHdl, &idxHdr,
	  (char *) &datbCore->recID, datbCore->recPtr)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if
    recID = 0;
    file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
    if (!recID)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Previous Record: The database file has been corrupted.");
      return;
    } // if
  } // else

  if (!LockRecord(recPtr, datbCore))
  { if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    DBReportResult(isFunction, theEvent, theSystem, 3);	// If busy
    return;
  } // if
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);

  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);	// After getting
} // ODatbGetPreviousRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetRecordFunc                                        |
 | DESCRIPTION  : Gets a record from a database file.  Index applies to the |
 |                records sorted with respect to the indexed Field Object.  |
 |		  If the indexed Field Object is not passed, Index applies  |
 |		  to the records sorted with respect to the chrnological    |
 |		  order.                                                    |
 | INPUT 1      : Database                                                  |
 | INPUT 2(Opt)	: Field Object                                              |
 | INPUT 3      : Index                                                     |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After getting                                             |
 | FLOW 2       : If error                                                  |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., October 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  OBJECTID              fieldObj;	// field object			[I: 2]
  OBJECTID              recIdxObj;	// nmbr object			(I: 3)
  long                  recIdx;         // record index

  BTree                 file;           // btree class
  FileHdr               fileHdr;        // file header
  IdxHdr                idxHdr;         // index header
  int			idx;		// index counter
  int			idxPos;		// index position
  long                  recPtr;         // record pointer
  long                  recID;          // record id

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);

    /* ---- FUNCTION INPUT 3 ---- */
    recIdxObj = AFuncGetTypedParameter(3, OTYPE_NUMBER);
    recIdx = ONmbrGetInteger(recIdxObj);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];

    /* ---- METHOD INPUT 3 ---- */
    recIdx = (long) ((ODatbMethodParams *) theEvent->details)->obj[2];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Get Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nGet Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (fieldObj)
  { idxPos = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
      if (field[idx].keyType)
	idxPos++;
    if (idx == fieldCount)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: The Field Object is not in the record.");
      return;
    } // if
    if (!field[idx].keyType)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: The Field Object is not indexed.");
      return;
    } // if
  } // if

  /* ---- CHECKS INPUT 3 (Index) ---- */
  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);
  if (recIdx < 1 || recIdx > fileHdr.Records)
  { if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
    return;
  } // if

  if (fieldObj)
  { /* locates record id on the index btree */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    if ((recID = file.LocateRecIdx(datbCore->fileHdl, &idxHdr, recIdx)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: The database file has been corrupted.");
      return;
    } // if

    /* locates record pointer on id index btree */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	  (int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
    { if (!datbCore->accessMode)
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: The database file has been corrupted.");
      return;
    } // if
  } // if
  else
  { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateRecIdx(datbCore->fileHdl, &idxHdr, recIdx)) == 0)
    { if (!datbCore->accessMode)
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: The database file has been corrupted.");
      return;
    } // if
    recID = 0;
    file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
    if (recID == 0)
    { if (!datbCore->accessMode)
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Get Record: The database file has been corrupted.");
      return;
    } // if
  } // else

  if (!LockRecord(recPtr, datbCore))
  { if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    /* ---- FLOW 3 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 3);	// If busy
    return;
  } // if

  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);

  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbGetRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbLoadDBFunc                                           |
 | DESCRIPTION  : Opens a database file using a pathname.                   |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Pathname                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After opening                                             |
 | FLOW 2       : If can't open                                             |
 | Novell, Inc., October 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbLoadDBFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore              *datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  OBJECTID              pathNameObj;    // text object			(I: 2)
  MEMBLOCKID		pathNameMB;	// pathname memory block
  char                  *pathName;      // path name

  struct fcb            blk;            // file control block

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    pathNameObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Load Database: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    pathNameObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
    if (AObjGetType(pathNameObj) != OTYPE_TEXT)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Load Database: Couldn't get the Pathname input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Load Database: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nLoad Database: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Load Database: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Pathname) ---- */
  pathNameMB = mb.Allocate();
  OTextGetToMemblock(pathNameObj, pathNameMB);
  if ((pathName = (char *) mb.GetPointer(pathNameMB)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Load Database: Couldn't get the Pathname input parameter.");
    return;
  } // if

  if (!CloseFile(datbCore))
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Load Database: Couldn't close the current database file.");
    return;
  } // if

  if (lstrlen(pathName) + 1 > sizeof(datbCore->pathName))
  { hmemcpy(datbCore->pathName, pathName, sizeof(datbCore->pathName) - 1);
    datbCore->pathName[sizeof(datbCore->pathName) - 1] = 0;
  } // if
  else
    lstrcpy(datbCore->pathName, pathName);

  if (!parsfnm(datbCore->pathName, &blk, 1))
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if (!OpenFile(datbCore, fieldCount, field))
    DBReportResult(isFunction, theEvent, theSystem, 2);	// If can't open
  else
  { if (datbCore->fileAccessMode == OF_READ)
      DBReportResult(isFunction, theEvent, theSystem, 3); //read only open
    else
      DBReportResult(isFunction, theEvent, theSystem);	// good read/write open
  }  
} // ODatbLoadDBFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbLockRecordFunc                                       |
 | DESCRIPTION  : Locks the current record.                                 |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After locking                                             |
 | FLOW 2       : If error						    |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbLockRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;	// datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info

  BTree                 file;           // btree class
  IdxHdr                idxHdr;         // index header

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Lock Record: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Lock Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nLock Record: Shouldn't be called at design-time.");

  if (datbCore->fileHdl < 0 || datbCore->recID == 0 || datbCore->recPtr == 0)
  { /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
    return;
  } // if

  if (!datbCore->isRecLock)
  { if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);

    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if (!file.LocateKey(datbCore->fileHdl, &idxHdr,
			(char *) &datbCore->recID, datbCore->recPtr,
			GetCmpExactFunc(idxHdr.keyType)))
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      /* ---- FLOW 2 ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if

    if (!LockRecord(datbCore->recPtr, datbCore))
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      /* ---- FLOW 2 ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if
  } // if
  DBReportResult(isFunction, theEvent, theSystem);	// After locking
} // ODatbLockRecordFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : ODatbMacroSearchFunc                                    |
 | DESCRIPTION  : Finds records using complex search instructions.        |
 | INPUT 1      : DATABASE: Database -- Find records in this Database.    |
 | INPUT 2      : TEXT: Instructions -- Find records according to this    |
 |		  instructions.                                           |
 | OUTPUT 1     : NUMBER: Count -- The number of found records.           |
 | FLOW 1       : Found.                                                  |
 | FLOW 2	: None found.		                                  |
 | FLOW 3       : If error.                                               |
 | Novell, Inc., September 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI ODatbMacroSearchFunc
( pAEvtInfo	theEvent,		// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  OBJECTID	datbObj;		// datb object		      (I: 1)
  DatbCore	*datbCore;		// datb core info
  WORD		fieldCount;		// number of fields
  Field		*field;			// and array of fields
  OBJECTID      instObj;		// text object		      (I: 2)
  MEMBLOCKID	instMB;			// instruction memory block
  char          *inst;          	// instruction
  OBJECTID	countObj;		// number object	      (O: 1)

  MacroTrn	trn;			// macrotrn class
  MacroExe	exe;			// macroexe class
  WORD		macroCodeCount;		// number of macro code
  Macro		*macroCode;		// an array of macro code
  char		errMsg[MXSTRSIZE];	// error message

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    instObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Macro Search: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    instObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
    if (AObjGetType(instObj) != OTYPE_TEXT)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Macro Search: Couldn't get the Instructions input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Macro Search: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nLock Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  fieldCount = (WORD) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  if (fieldCount)
  if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Macro Search: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 ---- */
  instMB = mb.Allocate();
  OTextGetToMemblock(instObj, instMB);
  if ((inst = (char *) mb.GetPointer(instMB)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Macro Search: Couldn't get the Instructions input parameter.");
    return;
  } // if

  macroCodeCount = 0;
  macroCode = (Macro *) MBAlloc(0);
  if (!trn.Translate(datbObj, inst, errMsg, &macroCodeCount, &macroCode))
  { /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, errMsg);
    return;
  } // if
  exe.Execute(datbObj, datbCore, fieldCount, field,
              macroCodeCount, macroCode);
  MBFree(macroCode);

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Macro Search: Couldn't create the Count output parameter.");
      return;
    } // if
    ONmbrSetInteger(countObj, datbCore->foundCount, TRUE);
    AFuncSetParameter(1, countObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[2] =
      (OBJECTID) datbCore->foundCount;
  } // else

  if (!datbCore->foundCount)
    /* ---- FLOW 2 (None Found) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 3);
  else
    /* ---- FLOW 1 (Found) ---- */
    DBReportResult(isFunction, theEvent, theSystem);
} // ODatbMacroSearchFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbNewDBFunc                                            |
 | DESCRIPTION  : Creates a new Object Database file.                       |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Pathname                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After creating                                            |
 | FLOW 2       : If canceled                                               |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbNewDBFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  OBJECTID              pathNameObj;	// text object			(I: 2)
  MEMBLOCKID		pathNameMB;	// pathname memory block
  char                  *pathName;	// pathname

  struct fcb            blk;            // file control block
  int                   fileHdl;	// file handle
  OPENFILENAME          ofn;		// openfilename struct
  char                  lpstrCustomFilter[40];	// custom filter
  char			lpstrFile[MXSTRSIZE];	// file name

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    pathNameObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"New Database: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    pathNameObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "New Database: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nNew Database: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"New Database: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Pathname) ---- */
  if (pathNameObj)
  { pathNameMB = mb.Allocate();
    OTextGetToMemblock(pathNameObj, pathNameMB);
    if ((pathName = (char *) mb.GetPointer(pathNameMB)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"New Database: Couldn't get the Pathname input parameter.");
      return;
    } // if
  } // if

  if (pathNameObj)
  { if (!parsfnm(pathName, &blk, 1))
    { /* ---- FLOW 2 ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);
      return;
    } // if

    if (_dos_creatnew(pathName, _A_NORMAL, &fileHdl))
      switch (errno)
      { /* ---- FLOW 2 ---- */
	case EEXIST:
	  DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
	    "New Database: File already exists.");
	  return;
	case ENOENT:
	  #ifdef UNDEF
	  DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
	    "New Database: Pathname not found.");
	  return;
	  #endif /* UNDEF */
	default:
	  DBReportResult(isFunction, theEvent, theSystem, 2);
	  return;
      } // switch
    if (_lclose(fileHdl) == -1)
    { /* ---- FLOW 2 ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);
      return;
    } // if
    if (!CreateFile(pathName, datbCore->version, fieldCount, field))
    { /* ---- FLOW 2 ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);
      return;
    } // if
  } // if
  else
  { /* ---- GetSaveFileName dialog box ---- */
    hmemset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFilter = lpstrFilter;
    memset(lpstrCustomFilter, '\0', sizeof(lpstrCustomFilter));
    wsprintf(lpstrCustomFilter, "Database Files (*.%s)",
	     datbCore->defFileExt);
    wsprintf(lpstrCustomFilter + 1 + lstrlen(lpstrCustomFilter), "*.%s",
	     datbCore->defFileExt);
    ofn.lpstrCustomFilter = lpstrCustomFilter;
    ofn.nMaxCustFilter = sizeof(lpstrCustomFilter);
    ofn.nFilterIndex = datbCore->filterIndex;
    lstrcpy(lpstrFile, DEF_FILENAME);
    ofn.lpstrFile = lpstrFile;
    ofn.nMaxFile = sizeof(lpstrFile);
    ofn.lpstrTitle = "Create New Database File";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
		OFN_SHAREAWARE;
    ofn.lpstrDefExt = datbCore->defFileExt;
    do
    { if (!GetSaveFileName(&ofn))
      { datbCore->filterIndex = (int) ofn.nFilterIndex;

	/* ---- FLOW 2 ---- */
	DBReportResult(isFunction, theEvent, theSystem, 2);	// If canceled
	return;
      } // if
      datbCore->filterIndex = (int) ofn.nFilterIndex;
    } while (!CreateFile(lpstrFile, datbCore->version, fieldCount, field));
    pathName = lpstrFile;
  } // else

  if (!CloseFile(datbCore))
  { DBReportResult(isFunction, theEvent, theSystem, 1, "Database\r\n"
      "New Database: Couldn't close the current database file.");
    return;
  } // if

  if (lstrlen(pathName) + 1 > sizeof(datbCore->pathName))
  { hmemcpy(datbCore->pathName, pathName, sizeof(datbCore->pathName) - 1);
    datbCore->pathName[sizeof(datbCore->pathName) - 1] = 0;
  } // if
  else
    lstrcpy(datbCore->pathName, pathName);

  if (!OpenFile(datbCore, fieldCount, field))
  { DBReportResult(isFunction, theEvent, theSystem, 1, "Database\r\n"
      "New Database: Couldn't open the newly created database file.");
    return;
  } // if
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbNewDBFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbOpenDBFunc                                           |
 | DESCRIPTION  : Opens a database file using file dialog.                  |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After opening                                             |
 | FLOW 2       : If canceled                                               |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbOpenDBFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs

  OPENFILENAME          ofn;		// openfilename struc
  char                  lpstrCustomFilter[40];	// custom filter
  char			lpstrFile[MXSTRSIZE];	// file name

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Open Database: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Open Database: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nOpen Database: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Open Database: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- GetOpenFileName dialog box ---- */
  hmemset(&ofn, 0, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = GetActiveWindow();
  ofn.lpstrFilter = lpstrFilter;
  memset(lpstrCustomFilter, '\0', sizeof(lpstrCustomFilter));
  wsprintf(lpstrCustomFilter, "Database Files (*.%s)",
	   datbCore->defFileExt);
  wsprintf(lpstrCustomFilter + 1 + lstrlen(lpstrCustomFilter), "*.%s",
	   datbCore->defFileExt);
  ofn.lpstrCustomFilter = lpstrCustomFilter;
  ofn.nMaxCustFilter = sizeof(lpstrCustomFilter);
  ofn.nFilterIndex = datbCore->filterIndex;
  lstrcpy(lpstrFile, datbCore->pathName);
  ofn.lpstrFile = lpstrFile;
  ofn.nMaxFile = sizeof(lpstrFile);
  ofn.lpstrTitle = "Open Database File";
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
	      OFN_SHAREAWARE;
  ofn.lpstrDefExt = datbCore->defFileExt;
  do
  { if (!GetOpenFileName(&ofn))
    { datbCore->filterIndex = (int) ofn.nFilterIndex;

      /* ---- FLOW 2 ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);// If canceled
      return;
    } // if
    datbCore->filterIndex = (int) ofn.nFilterIndex;

    if (!CloseFile(datbCore))
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Open Database: Couldn't close the current database file.");
      return;
    } // if

    if (lstrlen(lpstrFile) + 1 > sizeof(datbCore->pathName))
    { hmemcpy(datbCore->pathName, lpstrFile, sizeof(datbCore->pathName) - 1);
      datbCore->pathName[sizeof(datbCore->pathName) - 1] = 0;
    } // if
    else
      lstrcpy(datbCore->pathName, lpstrFile);
  } while (!OpenFile(datbCore, fieldCount, field));

  if (datbCore->fileAccessMode == OF_READ)
    DBReportResult(isFunction, theEvent, theSystem, 3); //read only open
  else
    DBReportResult(isFunction, theEvent, theSystem);	// good read/write open
    
} // ODatbOpenDBFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbPowerSearchFunc                                      |
 | DESCRIPTION  : Finds records on the basis of the multiple search         |
 |                criteria specified by Power Search dialog.                |
 | INPUT 1      : Database                                                  |
 | OUTPUT 1     : Count                                                     |
 | FLOW 1       : Found                                                     |
 | FLOW 2	: None found						    |	
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbPowerSearchFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  OBJECTID		countObj;	// nmbr object			(O: 1)

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Power Search: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  { MemBlock		mb;		// memblock class

    if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Power Search: Couldn't get the Database input parameter.");
      return;
    } // if
    ADBG_ASSERT_E(datbCore->isRunTime,
      ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
      "Database\r\nPower Search: Shouldn't be called at design-time.");
    if (datbCore->fileHdl < 0)
    { DBReportResult(isFunction, theEvent, theSystem, 2);
      return;
    } // if
  } // scope level-1

  DialogBoxParam(hInstance, "PowerSearch", GetActiveWindow(),
		 ODatbPowerSearchDialog, (LPARAM) datbObj);

  { MemBlock		mb;		// memblock class

    if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Power Search: Couldn't get the Database input parameter.");
      return;
    } // if

    if (isFunction)
    { /* ---- FUNCTION OUTPUT 1 ---- */
      if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
      { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	  "Power Search: Couldn't create the Count output parameter.");
	return;
      } // if
      ONmbrSetInteger(countObj, datbCore->foundCount, TRUE);
      AFuncSetParameter(1, countObj);
    } // if
    else
    { /* ---- METHOD OUTPUT 1 ---- */
      ((ODatbMethodParams *) theEvent->details)->obj[1] =
	(OBJECTID) datbCore->foundCount;
    } // else
  } // scope level-1

  if (!datbCore->foundCount)
    /* ---- FLOW 2 (None Found) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);
  else
    /* ---- FLOW 1 (Found) ---- */
    DBReportResult(isFunction, theEvent, theSystem);
} // ODatbPowerSearchFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbReadFoundRecordFunc                                  |
 | DESCRIPTION  : Reads a found record, without locking it.                 |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Index                                                     |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After reading                                             |
 | FLOW 2       : If error                                                  |
 | Novell, Inc., October 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbReadFoundRecordFunc
( pAEvtInfo		theEvent,	// evnet pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;        // datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info
  int                   fieldCount;     // number of FIELDs
  Field                 *field;         // an array of FIELDs
  OBJECTID              foundIdxObj;	// nmbr object        		(I: 2)
  long                  foundIdx;       // found index

  BTree                 file;           // btree class
  IdxHdr                idxHdr;         // btree index header
  long                  recPtr;         // record pointer
  long			recID;		// record id

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    foundIdxObj = AFuncGetTypedParameter(2, OTYPE_NUMBER);
    foundIdx = ONmbrGetInteger(foundIdxObj);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Found Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    foundIdx = (long) ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Read Found Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nRead Found Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Found Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Index) ---- */
  if (foundIdx < 1 || foundIdx > datbCore->foundCount)
  { /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
    return;
  } // if
  recID = *(long *) hmemptr(datbCore->found, foundIdx - 1,
			    sizeof(*datbCore->found));

  if (!datbCore->accessMode)            // the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	(int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
  { if (!datbCore->accessMode)            // the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
    return;
  } // if

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);
  UnlockRecord(datbCore);

  if (!datbCore->accessMode)              // the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  datbCore->recPtr = recPtr;
  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbReadFoundRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbReadNextRecordFunc                                   |
 | DESCRIPTION  : Reads the record that next to the Field Object current    |
 |		  record.  If Field Object is not passed, the next          |
 |		  chronological order record is read.                       |
 | INPUT 1      : Database                                                  |
 | INPUT 2(Opt)	: Field Object                                              |
 | OUPUT        : None                                                      |
 | FLOW 1       : After reading                                             |
 | FLOW 2       : If error                                                  |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbReadNextRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// and array of FIELDs
  OBJECTID		fieldObj;	// field object			[I: 2]

  BTree                 file;           // btree class
  int                   idx;            // index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // index header
  long                  recID;          // record id
  long                  recPtr;         // record pointer

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Next Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Read Next Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nRead Next Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Next Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (fieldObj)
  { idxPos = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
      if (field[idx].keyType)
	idxPos++;
    if (idx == fieldCount)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Next Record: The Field Object is not in the record.");
      return;
    } // if
    if (!field[idx].keyType)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Next Record: The Field Object is not indexed.");
      return;
    } // if
  } // if

  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  if (fieldObj)
  { /* locates the current key associated with the current record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    if ((recID = file.LocateNext(datbCore->fileHdl, &idxHdr,
	  field[idx].keyVal, datbCore->recID)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if

    /* locates the record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	  (int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Next Record: The database file has been corrupted.");
      return;
    } // if
  } // if
  else
  { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateNext(datbCore->fileHdl, &idxHdr,
	  (char *) &datbCore->recID, datbCore->recPtr)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if
    recID = 0;
    file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
    if (recID == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Next Record: The database file has been corrupted.");
      return;
    } // if
  } // else

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);
  UnlockRecord(datbCore);

  if (!datbCore->accessMode)            // the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  datbCore->recPtr = recPtr;
  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);	// After reading
} // ODatbReadNextRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbReadPreviousRecordFunc                               |
 | DESCRIPTION  : Reads the record that previous to the Field Object        |
 |		  current record.  If Field Object is not passed, the       |
 |		  previous chronological order record is read.              |
 | INPUT 1      : Database                                                  |
 | INPUT 2(Opt)	: Field object                                              |
 | OUPUT        : None                                                      |
 | FLOW 1       : After reading                                             |
 | FLOW 2       : If error                                                  |
 | FLOW 3       : If busy                                                   |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbReadPreviousRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;        // datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info
  int                   fieldCount;     // number of FIELDs
  Field                 *field;         // and array of FIELDs
  OBJECTID              fieldObj;	// field object			[I: 2]

  BTree                 file;           // btree class
  int                   idx;            // index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // index header
  long                  recID;          // record id
  long                  recPtr;         // record pointer

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Previous Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Read Previous Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nRead Previous Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Previous Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (fieldObj)
  { idxPos = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
      if (field[idx].keyType)
	idxPos++;
    if (idx == fieldCount)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Previous Record: The Field Object is not in the record.");
      return;
    } // if
    if (!field[idx].keyType)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Previous Record: The Field Object is not indexed.");
      return;
    } // if
  } // if

  if (!datbCore->accessMode)            // the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  if (fieldObj)
  { /* locates the current key associated with the current record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    if ((recID = file.LocatePrev(datbCore->fileHdl, &idxHdr,
	  field[idx].keyVal, datbCore->recID)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if

    /* locates the record id */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	  (int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Previous Record: The database file has been corrupted.");
      return;
    } // if
  } // if
  else
  { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocatePrev(datbCore->fileHdl, &idxHdr,
	  (char *) &datbCore->recID, datbCore->recPtr)) == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 2);// If error
      return;
    } // if
    recID = 0;
    file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
    if (recID == 0)
    { if (!datbCore->accessMode)        // the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Previous Record: The database file has been corrupted.");
      return;
    } // if
  } // else

  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);
  UnlockRecord(datbCore);

  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  datbCore->recPtr = recPtr;
  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);	// After reading
} // ODatbReadPreviousRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbReadRecordFunc                                       |
 | DESCRIPTION  : Reads a record from a database file, without locking it.  |
 |		  Index applies to the records sorted with respect to the   |
 |		  indexed Field Object.  If the indexed Field Object is not |
 |		  passed, Index applies to the records sorted with respect  |
 |		  to the chronological order.                               |
 | INPUT 1      : Database                                                  |
 | INPUT 2(Opt)	: Field Object                                              |
 | INPUT 3      : Index                                                     |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After reading                                             |
 | FLOW 2       : If error                                                  |
 | Novell, Inc., October 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbReadRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  OBJECTID              fieldObj;	// field object			[I: 2]
  OBJECTID              recIdxObj;	// nmbr object			(I: 3)
  long                  recIdx;         // record index

  BTree                 file;           // btree class
  FileHdr		fileHdr;	// file header
  IdxHdr		idxHdr;		// index header
  int                   idx;            // index counter
  int			idxPos;		// index positon
  long                  recPtr;         // record pointer
  long                  recID;          // record id

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);

    /* ---- FUNCTION INPUT 3 ---- */
    recIdxObj = AFuncGetTypedParameter(3, OTYPE_NUMBER);
    recIdx = ONmbrGetInteger(recIdxObj);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];

    /* ---- METHOD INPUT 3 ---- */
    recIdx = (long) ((ODatbMethodParams *) theEvent->details)->obj[2];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Read Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nRead Record: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (fieldObj)
  { idxPos = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
      if (field[idx].keyType)
	idxPos++;
    if (idx == fieldCount)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: The Field Object is not in the record.");
      return;
    } // if
    if (!field[idx].keyType)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: The Field Object is not indexed.");
      return;
    } // if
  } // if

  /* ---- CHECKS INPUT 3 (Index) ---- */
  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);
  if (recIdx < 1 || recIdx > fileHdr.Records)
  { if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);

    /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
    return;
  } // if

  if (fieldObj)
  { /* locates record id on the index btree */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    if ((recID = file.LocateRecIdx(datbCore->fileHdl, &idxHdr, recIdx)) == 0)
    { if (!datbCore->accessMode)	// the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: The database file has been corrupted.");
      return;
    } // if

    /* locates recort pointer on record id index btree */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *) &recID,
	  (int (*)(char *, char *)) GetCmpExactFunc(idxHdr.keyType))) == 0)
    { if (!datbCore->accessMode)
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: The database file has been corrupted.");
      return;
    } // if
  } // if
  else
  { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    if ((recPtr = file.LocateRecIdx(datbCore->fileHdl, &idxHdr, recIdx)) == 0)
    { if (!datbCore->accessMode)
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: The database file has been corrupted.");
      return;
    } // if
    recID = 0;
    file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
    if (recID == 0)
    { if (!datbCore->accessMode)
	file.UnlockFileHdr(datbCore->fileHdl);

      DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Read Record: The database file has been corrupted.");
      return;
    } // if
  } // else

  UnlockRecord(datbCore);
  ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);

  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  datbCore->recPtr = recPtr;
  datbCore->recID = recID;
  DBPostEvent(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);	// After reading
} // ODatbReadRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbRecordIndexFunc                                      |
 | DESCRIPTION  : Returns the index of the current record with respect to   |
 |		  Field Object.  If Field Object is not passed, the index   |
 |		  is respect to order of entry.                             |
 | INPUT 1      : Database                                                  |
 | INPUT 2(Opt)	: Field Object                                              |
 | OUTPUT 1     : Index                                                     |
 | FLOW 1       : Done                                                      |
 | Novell, Inc., October 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbRecordIndexFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  OBJECTID              fieldObj;	// field object			(I: 2)
  OBJECTID		recIdxObj;	// nmbr object			(O: 1)
  long			recIdx;		// record index

  BTree                 file;           // btree class
  int			idx;		// index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // index header

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Record Index: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Record Index: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nRecord Index: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Record Index: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (fieldObj)
  { idxPos = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
      if (field[idx].keyType)
	idxPos++;
    if (idx == fieldCount)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Record Index: The Field Object is not in the record.");
      return;
    } // if
    if (!field[idx].keyType)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Record Index: The Field Object is not indexed.");
      return;
    } // if
  } // if

  if (datbCore->fileHdl < 0 || datbCore->recID == 0 || datbCore->recPtr == 0)
    recIdx = 0;
  else
  { if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);

    if (fieldObj)
    { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
      recIdx= file.LocateRecID(datbCore->fileHdl, &idxHdr, datbCore->recID);
    } // if
    else
    { file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
      recIdx = file.LocateRecID0(datbCore->fileHdl, &idxHdr, datbCore->recID);
    } // if

    if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
  } // else

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((recIdxObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Record Index: Couldn't create the Index output parameter.");
      return;
    } // if
    ONmbrSetInteger(recIdxObj, recIdx, TRUE);
    AFuncSetParameter(1, recIdxObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[2] = (OBJECTID) recIdx;
  } // else
  DBReportResult(isFunction, theEvent, theSystem);	// After returning
} // ODatbRecordIndexFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbRemoveFoundIDFunc                                    |
 | DESCRIPTION  : Removes a found record ID from the Database's found list. |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Index                                                     |
 | OUTPUT 1     : Count                                                     |
 | FLOW 1       : After removing                                            |
 | Novell, Inc., October 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbRemoveFoundIDFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  OBJECTID              foundIdxObj;	// nmbr object			(I: 2)
  long			foundIdx;	// found index
  OBJECTID		countObj;	// nmbr object			(O: 1)

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    foundIdxObj = AFuncGetTypedParameter(2, OTYPE_NUMBER);
    foundIdx = ONmbrGetInteger(foundIdxObj);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Remove Found ID: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    foundIdx = (long) ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Remove Found ID: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nRemove Found ID: Shouldn't be called at design-time.");

  if (foundIdx > 0 && foundIdx <= datbCore->foundCount)
  { if (datbCore->foundIdx == foundIdx)
      datbCore->foundIdx--;
    hmemcpy(hmemptr(datbCore->found, foundIdx - 1, sizeof(*datbCore->found)),
	    hmemptr(datbCore->found, foundIdx, sizeof(*datbCore->found)),
	    (datbCore->foundCount - foundIdx) * sizeof(*datbCore->found));
    datbCore->foundCount--;
  } // if

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Count Records: Couldn't create the Count output parameter.");
      return;
    } // if
    ONmbrSetInteger(countObj, datbCore->foundCount, TRUE);
    AFuncSetParameter(1, countObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[1] =
      (OBJECTID) datbCore->foundCount;
  } // else
  DBReportResult(isFunction, theEvent, theSystem);	// After removing
} // ODatbRemoveFoundIDFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbSetAllModeFunc                                       |
 | DESCRIPTION  : Sets mode to all mode.                                    |
 | INPUT 1      : Database                                                  |
 | FLOW 1       : Done                                                      |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbSetAllModeFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Set All Mode: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Set All Mode: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nSet All Mode: Shouldn't be called at design-time.");

  if (datbCore->mode != MODE_ALL)
  { datbCore->mode = MODE_ALL;

    theEvent->sourceObject = theEvent->targetObject = datbObj;
    theEvent->when = AAppGetTime();
    theEvent->how = AEVT_KILLDUPLICATES | AEVT_ATTAIL;
    theEvent->what = AEVENT_OBJECTCHANGED;
    AEvtPost(theEvent);
  } // if
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbSetAllModeFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbSetFoundIDsFunc                                      |
 | DESCRIPTION  : Replaces the Database's found-list with a "\r\n"          |
 |                delimited string of record IDs.                           |
 | INPUT 1      : Database object.                                          |
 | INPUT 2      : A text object containing the record IDs to be retrieved.  |
 | OUTPUT 1     : A number object containing the number of found IDs.       |
 | FLOW 1       : Done.                                                     |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbSetFoundIDsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;	// datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info
  OBJECTID              recIDsObj;	// text object			(I: 2)
  char                  *recIDStr;	// "\r\n" delimited string of recIDs
  OBJECTID		countObj;	// nmbr object			(O: 1)

  MEMBLOCKID		tempMB;		// temporary memory block
  long			*recIDPtr;	// record id pointer
  long                  recID;          // record id

  long			lI;		// counter

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    recIDsObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Set Found IDs: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    recIDsObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
    if (AObjGetType(recIDsObj) != OTYPE_TEXT)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Set Found IDs: Couldn't get the Record IDs input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Find Record ID: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nSet Found ID: Shouldn't be called at design-time.");

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (!recIDsObj)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Set Found IDs: Couldn't get the Record IDs input parameter.");
    return;
  } // if
  tempMB = mb.Allocate();
  OTextGetToMemblock(recIDsObj, tempMB);
  if ((recIDStr = (char *) mb.GetPointer(tempMB)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Set Found IDs: Couldn't get the Record IDs input parameter.");
    return;
  } // if

  /* rebuilds found-list */
  if (datbCore->found)
  { MBFree(datbCore->found);
    datbCore->found = 0;
    datbCore->foundCount = 0;
    datbCore->foundIdx = 0;
  } // if

  do
  { /* adds new record IDs to found-list */
    if ((recID = strtol(recIDStr, &recIDStr, 10)) > 0)
    { for (lI = 0, recIDPtr = datbCore->found;
	   lI < datbCore->foundCount && *recIDPtr != recID;
	   lI++, recIDPtr++)
	/* Do Nothing */;
      if (lI == datbCore->foundCount)
      { if (datbCore->foundCount % FOUNDBLOCK == 0)
	  datbCore->found = (long *) MBRealloc(datbCore->found,
	    (datbCore->foundCount + FOUNDBLOCK) * sizeof(*datbCore->found));
	*(long *) hmemptr(datbCore->found, datbCore->foundCount,
			  sizeof(*datbCore->found)) = recID;
	datbCore->foundCount++;
      } // if
    } // if
  } while (*recIDStr++ == '\r' && *recIDStr++ == '\n');

  if (isFunction)
  { /* ---- FUNCTION OUTPUT 1 ---- */
    if ((countObj = AObjCreate(OTYPE_NUMBER)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Set Found IDs: Couldn't create the Count output parameter.");
      return;
    } // if
    ONmbrSetInteger(countObj, datbCore->foundCount, TRUE);
    AFuncSetParameter(1, countObj);
  } // if
  else
  { /* ---- METHOD OUTPUT 1 ---- */
    ((ODatbMethodParams *) theEvent->details)->obj[2] =
      (OBJECTID) datbCore->foundCount;
  } // else
  DBReportResult(isFunction, theEvent, theSystem);	// After setting
} // ODatbSetFoundIDsFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbSetFoundModeFunc                                     |
 | DESCRIPTION  : Sets mode to found mode.                                  |
 | INPUT 1      : Database                                                  |
 | FLOW 1       : Done                                                      |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbSetFoundModeFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Set Found Mode: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Set Found Mode: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nSet Found Mode: Shouldn't be called at design-time.");

  if (datbCore->mode != MODE_FOUND)
  { datbCore->mode = MODE_FOUND;

    theEvent->sourceObject = theEvent->targetObject = datbObj;
    theEvent->when = AAppGetTime();
    theEvent->how = AEVT_KILLDUPLICATES | AEVT_ATTAIL;
    theEvent->what = AEVENT_OBJECTCHANGED;
    AEvtPost(theEvent);
  } // if
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbSetFoundModeFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbSortFoundFunc                                        |
 | DESCRIPTION  : Sorts the list of records by Field Object.                |
 | INPUT 1      : Database                                                  |
 | INPUT 2      : Field Object                                              |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After sorting                                             |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbSortFoundFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID              datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field                 *field;         // an array of FIELDs
  OBJECTID              fieldObj;	// field object			(I: 2)

  BTree                 file;           // btree class
  int			idx;		// index counter
  int			idxPos;		// index position
  IdxHdr                idxHdr;         // index header
  long                  recID;          // record id
  long			*currRecID;	// current record id
  long			*fromRecID;	// from record id
  long			currIdx;	// current index
  long                  fromIdx;        // from index
  long                  toIdx;          // to index

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    fieldObj = AFuncGetParameter(2);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Sort Found: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    fieldObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Sort Found: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nSort Found: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Sort Found: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Field Object) ---- */
  if (!fieldObj)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Sort Found: Couldn't get the Field Object input parameter.");
    return;
  } // if

  idxPos = 1;
  for (idx = 0; idx < fieldCount && field[idx].obj != fieldObj; idx++)
    if (field[idx].keyType)
      idxPos++;
  if (idx == fieldCount)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Sort Found: The Field Object is not in the record.");
    return;
  } // if
  if (!field[idx].keyType)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Sort Found: The Field Object is not indexed.");
    return;
  } // if

  if ((datbCore->fileHdl < 0) == 0)
  { if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);

    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxPos);
    fromIdx = 0;
    toIdx = datbCore->foundCount - 1;
    recID = file.FirstKey(datbCore->fileHdl, &idxHdr);
    while (recID && fromIdx != toIdx)
    { for (currIdx = fromIdx, currRecID = (long *)
	     hmemptr(datbCore->found, currIdx, sizeof(*datbCore->found));
	   currIdx <= toIdx && *currRecID != recID;
	   currIdx++, currRecID++)
	/* Do Nothing */;
      if (currIdx <= toIdx)
      { if (currIdx != fromIdx)
	{ /* ---- swaps ---- */
	  fromRecID = (long *) hmemptr(datbCore->found, fromIdx,
				       sizeof(long));
	  *currRecID = *fromRecID;
	  *fromRecID = recID;
	} // if
	fromIdx++;
      } // if
      recID = file.NextRecPtr(datbCore->fileHdl);
    } // while

    if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
  } // if
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbSortFoundFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbStartXFunc                                           |
 | DESCRIPTION  : Establishes exclusive access to the database file.        |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After starting                                            |
 | FLOW 2       : If error                                                  |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbStartXFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Start Exclusive: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Start Exclusive: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nStart Exclusive: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if (!datbCore->accessMode)
  { UnlockRecord(datbCore);

    if (_lclose(datbCore->fileHdl) == -1)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Start Exclusive: The database file might have been damaged.");
      return;
    } // if

    if ((datbCore->fileHdl = _lopen(datbCore->pathName,
	   OF_SHARE_EXCLUSIVE | datbCore->fileAccessMode)) == -1)
    { if ((datbCore->fileHdl = _lopen(datbCore->pathName,
	     OF_SHARE_DENY_NONE | datbCore->fileAccessMode)) == -1)
      {	if (datbCore->found)
	{ /* deletes private found-list */
	  MBFree(datbCore->found);
	  datbCore->found = 0;
	  datbCore->foundCount = 0;
	  datbCore->foundIdx = 0;
	} // if

	DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
	  "Start Exclusive: The file might have been exclusive locked "
	  "by other user.  The file is closed.");
	return;
      } // if

      DBReportResult(isFunction, theEvent, theSystem, 2);
      return;
    } // if

    datbCore->accessMode = TRUE;
  } // if
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbStartXFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbStopXFunc                                            |
 | DESCRIPTION  : Stops exclusive access to the current database file.      |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : Done                                                      |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbStopXFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage          theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Stop Exclusive: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Stop Exclusive: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nStop Exclusive: Shouldn't be called at design-time.");
  if (datbCore->fileHdl < 0)
  { DBReportResult(isFunction, theEvent, theSystem, 2);
    return;
  } // if

  if (datbCore->accessMode)
  { UnlockRecord(datbCore);

    if (_lclose(datbCore->fileHdl) == -1)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Start Exclusive: The database file might have been damaged.");
      return;
    } // if

    if ((datbCore->fileHdl = _lopen(datbCore->pathName,
	   OF_SHARE_DENY_NONE | datbCore->fileAccessMode)) == -1)
    { if (datbCore->found)
      { /* deletes private found-list */
	MBFree(datbCore->found);
	datbCore->found = 0;
	datbCore->foundCount = 0;
	datbCore->foundIdx = 0;
      } // if

      DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
	"Start Exclusive: The file might have been exclusive locked "
	"by other user.  The file is closed.");
      return;
    } // if

    datbCore->accessMode = FALSE;
  } // if
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbStopXFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbUnlockRecordFunc                                     |
 | DESCRIPTION  : Unlocks the current locked record.                        |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After unlocking                                           |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbUnlockRecordFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore              *datbCore;      // datb core info

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Unlock Record: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Unlock Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nUnlock Record: Shouldn't be called at design-time.");

  if ((datbCore->fileHdl < 0) == 0)
    UnlockRecord(datbCore);
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbUnlockRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbUpdateRecordFunc                                     |
 | DESCRIPTION  : Updates the current locked record in the database file.   |
 | INPUT 1      : Database                                                  |
 | OUTPUT       : None                                                      |
 | FLOW 1       : After updating                                            |
 | FLOW 2       : If error                                                  |
 | Novell, Inc., September 2, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbUpdateRecordFunc
( pAEvtInfo		theEvent,	// event poiter
  pAObjMessage		theSystem	// system pointer
)
{ MemBlock		mb;		// memblock class
  OBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs

  DBKey			dbKey;		// dbkey class
  char			*key;		// key
  BTree                 file;           // btree class
  FileHdr               fileHdr;        // file header
  int                   idx;            // index counter
  IdxHdr                idxHdr;         // index header
  IdxHdr		idxHdr2;	// index header 2		
  long                  recPtr;         // record pointer

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Update Record: Couldn't get the Database input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Update Record: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nUpdate Record: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Update Record: Couldn't get the Database input parameter.");
      return;
    } // if

  if (datbCore->fileHdl < 0 || datbCore->fileAccessMode == OF_READ ||
      !datbCore->isRecLock)
  { /* ---- FLOW 2 ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2);	// If error
    return;
  } // if

  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);

  for (idx = 1; idx <= fileHdr.Indexes; idx++)
  { /* updates field indexes */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idx);
    idxHdr2 = idxHdr;
    key = (char *) dbKey.GetKey(datbObj, field[(int) (idxHdr.field - 1)].obj,
				idxHdr.keyType, idxHdr.keyLen);
    if (CmpExact(field[(int) (idxHdr.field - 1)].keyVal, key, idxHdr.keyType))
    { file.DeleteKey(datbCore->fileHdl, fileHdr, idxHdr,
		     field[(int) (idxHdr.field - 1)].keyVal, datbCore->recID);
      file.InsertKey(datbCore->fileHdl, fileHdr, idxHdr,
		     key, datbCore->recID);
      if (memcmp(&idxHdr, &idxHdr2, sizeof(IdxHdr)))
        file.WriteBTreeHdr(datbCore->fileHdl, &idxHdr, idx);
    } // if
  } // for

  UnlockRecord(datbCore);
  recPtr = file.ReallocRec(datbCore->fileHdl, fileHdr, datbCore->recPtr,
			   GetRecordSize(fieldCount, field));
  WriteRecord(recPtr, datbObj, datbCore, fieldCount, field);

  /* updates record id field index */
  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  idxHdr2 = idxHdr;
  file.DeleteKey(datbCore->fileHdl, fileHdr, idxHdr,
		 (char *) &datbCore->recID, datbCore->recPtr);
  file.InsertKey(datbCore->fileHdl, fileHdr, idxHdr,
		 (char *) &datbCore->recID, recPtr);
  if (memcmp(&idxHdr, &idxHdr2, sizeof(IdxHdr)))
    file.WriteBTreeHdr(datbCore->fileHdl, &idxHdr);

  LockRecord(recPtr, datbCore);

  file.WriteFileHdr(datbCore->fileHdl, &fileHdr);
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  datbCore->recPtr = recPtr;
  DBReportResult(isFunction, theEvent, theSystem);	// After updating
} // ODatbUpdateRecordFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION	: ODatbRecoverDBFunc                                        |
 | DESCRIPTION	: Recovers a damaged Object Database File.                  |
 | INPUT 1	: Database                                                  |
 | INPUT 2(Opt.): Pathname                                                  |
 | FLOW 1	: Recovered.                                                |
 | FLOW 2	: Failed.                                                   |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbRecoverDBFunc
( pAEvtInfo	theEvent,		// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class		
  OBJECTID	datbObj;		// datb object			(I: 1)
  DatbCore	*datbCore;		// datb core info
  int		fieldCount;		// number of FIELDs
  Field		*field;			// an array of FIELDs
  OBJECTID	pathNameObj;		// text object			(I: 2)
  MEMBLOCKID	pathNameMB;		// pathname memory block
  char		*pathName;		// pathname

  struct fcb	blk;			// file control block
  OPENFILENAME	ofn;			// openfilename struc
  char		lpstrCustomFilter[40];	// custom filter
  char		lpstrFile[MXSTRSIZE];	// file name

  char          drivename[_MAX_DRIVE + _MAX_DIR];
  					// drive name
  char          dirname[_MAX_DIR];	// directory name
  char          filename[_MAX_FNAME];	// file name
  char          extname[_MAX_EXT];	// file extension name

  int		fileHdl;		// file handle
  long		recPtr;			// record pointer
  int		fieldTypeCount;		// number of field TYPEIDs
  TYPEID	*fieldType;		// an array of field TYPEIDs
  long		firstRecPtr;		// first record pointer
  long		fileLen;		// file length

  int		*fieldMapPos;		// an array of field mapping positions
  OBJECTID	*fieldObj;		// an array of field OBJECTIDs

  OBJECTID	tempObj;		// temporary object
  TYPEID	tempTypeID;		// temporary type ID
  char		*tempName;		// temporary file name
  int		tempHdl;		// temporary file handle
  long		tempRecPtr;		// temporary record pointer

  BTree		file;			// btree class
  FileID	fileID;			// file ID
  FileHdr	fileHdr;		// file header
  IdxHdr	idxHdr;			// index header
  IdxHdr	idxHdr2;		// index header 2					

  DBKey		dbKey;			// DBKey class
  char		*key;			// key

  int		i, j;			// counters

  BOOL isFunction = theEvent->what == AEVENT_CALLFUNCTION? TRUE : FALSE;
  if (isFunction)
  { /* ---- FUNCTION INPUT 1 ---- */
    datbObj = AFuncGetTypedParameter(1, OTYPE_DATABASE);

    /* ---- FUNCTION INPUT 2 ---- */
    pathNameObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
  } // if
  else
  { /* ---- METHOD INPUT 1 ---- */
    datbObj = ((ODatbMethodParams *) theEvent->details)->obj[0];
    if (AObjGetType(datbObj) != OTYPE_DATABASE)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Load Database: Couldn't get the Database input parameter.");
      return;
    } // if

    /* ---- METHOD INPUT 2 ---- */
    pathNameObj = ((ODatbMethodParams *) theEvent->details)->obj[1];
    if (AObjGetType(pathNameObj) != OTYPE_TEXT)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Load Database: Couldn't get the Pathname input parameter.");
      return;
    } // if
  } // else

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Recover Database: Couldn't get the Database input parameter.");
    return;
  } // if
  ADBG_ASSERT_E(datbCore->isRunTime,
    ((ODatbMethodParams *) theEvent->details)->flowIdx = 0; return,
    "Database\r\nRecover Database: Shouldn't be called at design-time.");

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Recover Database: Couldn't get the Database input parameter.");
      return;
    } // if

  /* ---- CHECKS INPUT 2 (Pathname) ---- */
  if (pathNameObj)
  { pathNameMB = mb.Allocate();
    OTextGetToMemblock(pathNameObj, pathNameMB);
    if ((pathName = (char *) mb.GetPointer(pathNameMB)) == 0)
    { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
	"Recover Database: Couldn't get the Pathname input parameter.");
      return;
    } // if
  } // if

  if (pathNameObj)
  { if (!parsfnm(pathName, &blk, 1))
    { /* ---- FLOW 2 (Failed) ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);
      return;
    } // if
  } // if
  else
  { /* ---- GetOpenFileName dialog box ---- */
    hmemset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFilter = lpstrFilter;
    memset(lpstrCustomFilter, '\0', sizeof(lpstrCustomFilter));
    wsprintf(lpstrCustomFilter, "Database Files (*.%s)",
	     datbCore->defFileExt);
    wsprintf(lpstrCustomFilter + 1 + lstrlen(lpstrCustomFilter), "*.%s",
	     datbCore->defFileExt);
    ofn.lpstrCustomFilter = lpstrCustomFilter;
    ofn.nMaxCustFilter = sizeof(lpstrCustomFilter);
    ofn.nFilterIndex = datbCore->filterIndex;
    lstrcpy(lpstrFile, DEF_FILENAME);
    ofn.lpstrFile = lpstrFile;
    ofn.nMaxFile = sizeof(lpstrFile);
    ofn.lpstrTitle = "Recover Database File";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
		OFN_SHAREAWARE;
    ofn.lpstrDefExt = datbCore->defFileExt;
    if (!GetOpenFileName(&ofn))
    { datbCore->filterIndex = (int) ofn.nFilterIndex;

      /* ---- FLOW 2 (Failed) ---- */
      DBReportResult(isFunction, theEvent, theSystem, 2);
      return;
    } // if
    datbCore->filterIndex = (int) ofn.nFilterIndex;
    pathName = lpstrFile;
  } // else

  if (!CloseFile(datbCore))
  { DBReportResult(isFunction, theEvent, theSystem, 0, "Database\r\n"
      "Recover Database: Couldn't close the current database file.");
    return;
  } // if

  if (lstrlen(pathName) + 1 > sizeof(datbCore->pathName))
  { hmemcpy(datbCore->pathName, pathName, sizeof(datbCore->pathName) - 1);
    datbCore->pathName[sizeof(datbCore->pathName) - 1] = '\0';
  } // if
  else
    lstrcpy(datbCore->pathName, pathName);

  if ((fileHdl = _lopen(datbCore->pathName, OF_READ | OF_SHARE_EXCLUSIVE))
	== -1)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- verifies Object Database File ---- */
  if (_lread(fileHdl, (char *) &fileID, sizeof(fileID)) == 65535U)
  { /* ---- FLOW 2 (Failed) ---- */
    _lclose(fileHdl);
    DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
      "Recover Database: This is not an Object Database File.");
    return;
  } // if
  if (lstrcmp(fileID.fileID, "Object Database File\32\0") != 0 &&
      lstrcmp(fileID.fileID, "Serius Database File\32\0") != 0)
  { _lclose(fileHdl);

    /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
      "Recover Database: This is not an Object Database File.");
    return;
  } // if

  /* ---- verifies the version of Object Database File ---- */
  if (fileID.version != datbCore->version)
  { _lclose(fileHdl);

    /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
      "Recover Database: This is not an Object Database File.");
    return;
  } // if

  /* ---- gets field TYPEIDs ---- */
  file.ReadFileHdr(fileHdl, &fileHdr);
  fieldTypeCount = (int) fileHdr.Fields;
  fieldType = (TYPEID *) MBAlloc(fileHdr.Fields * sizeof(TYPEID));
  _llseek(fileHdl, sizeof(FileID) + sizeof(Lock) + sizeof(FileHdr) +
		   (1 + fileHdr.Indexes) * sizeof(IdxHdr), 0);
  if ((_lread(fileHdl, (char *) fieldType, fieldTypeCount * sizeof(TYPEID)))
	== 65535U)
  { MBFree(fieldType);
    _lclose(fileHdl);

    /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
      "Recover Database: Couldn't recover this database file.");
    return;
  } // if

  /* ---- gets file size and file offset ---- */
  firstRecPtr = fileHdr.recStart;
  if ((fileLen = filelength(fileHdl)) == -1)
  { MBFree(fieldType);
    _lclose(fileHdl);

    /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
      "Recover Database: Couldn't recover this database file.");
    return;
  } // if

  /* ---- gets mapping positions and the fields objects ---- */
  fieldMapPos = (int *) MBAlloc(fieldCount * sizeof(*fieldMapPos));
  fieldObj = (OBJECTID *) MBAlloc(fieldCount * sizeof(*fieldObj));
  for (i = 0; i < fieldCount; i++)
  { tempTypeID = field[i].type;
    for (j = 0; j < fieldTypeCount; j++)
      if (fieldType[j] && fieldType[j] == tempTypeID)	// found
      { fieldMapPos[i] = j;				// field j
	fieldObj[i] = AObjCreate(tempTypeID);		// pseudo object
	if (tempTypeID == OTYPE_NUMBER)
	  ONmbrSetType(fieldObj[i], ONmbrGetType(field[i].obj));

	fieldType[j] = 0;
	break;
      } // if
    if (j == fieldTypeCount)				// not found
    { fieldMapPos[i] = -1;				// not in the record
      fieldObj[i] = field[i].obj;			// real object
    } // if
  } // for
  MBFree(fieldType);

  /* ---- creates temporary file with prefix ODBFL ---- */
  _splitpath(datbCore->pathName, drivename, dirname, filename, extname);
  lstrcat(drivename, dirname);
  tempName = tempnam(drivename, "ODBFL");
  if (!CreateFile(tempName, datbCore->version, fieldCount, field))
  { free(tempName);
    for (i = 0; i < fieldCount; i++)
      if (fieldMapPos[i] >= 0)
	AObjDestroy(fieldObj[i]);	// pseudo object
    MBFree(fieldMapPos);
    MBFree(fieldObj);
    _lclose(fileHdl);

    /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
      "Recover Database: Couldn't create a temporary file.");
    return;
  } // if
  if ((tempHdl = _lopen(tempName, OF_READWRITE | OF_SHARE_EXCLUSIVE)) == -1)
  { free(tempName);
    for (i = 0; i < fieldCount; i++)
      if (fieldMapPos[i] >= 0)
	AObjDestroy(fieldObj[i]);	// pseudo object
    MBFree(fieldMapPos);
    MBFree(fieldObj);
    _lclose(fileHdl);

    /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, "Database\r\n"
      "Recover Database: Couldn't create a temporary file.");
    return;
  } // if
  file.ReadFileHdr(tempHdl, &fileHdr);

  recPtr = 0;
  while ((recPtr = file.GetNextRecPtr(fileHdl, fileLen, firstRecPtr, recPtr))
	   != 0)
  { /* ---- reads source file ---- */
    for (i = 0; i < fieldCount; i++)
      if (fieldMapPos[i] >= 0)
      {	/* ---- reads pseudo objects ---- */
	if ((tempObj = ReadObject(fileHdl, recPtr, fieldMapPos[i])) == 0)
	  tempObj = AObjCreate(AObjGetType(fieldObj[i]));
	AObjAssign(tempObj, fieldObj[i]);
	AObjDestroy(tempObj);
      } // if

    /* ---- writes destination file ---- */
    fileHdr.Records++;
    fileHdr.LastRecordID++;

    tempRecPtr = file.AllocRec(tempHdl, fileHdr,
			       GetRecordSize(fieldCount, fieldObj));
    WriteRecord(tempHdl, tempRecPtr, fieldCount, fieldObj);

    /* ---- inserts record ID index ---- */
    file.ReadBTreeHdr(tempHdl, &idxHdr);
    idxHdr2 = idxHdr;
    file.InsertKey(tempHdl, fileHdr, idxHdr,
		   (char *) &fileHdr.LastRecordID, tempRecPtr);
    if (memcmp(&idxHdr, &idxHdr2, sizeof(idxHdr)))
      file.WriteBTreeHdr(tempHdl, &idxHdr);

    /* ---- inserts field indexes ---- */
    for (j = 0, i = 0; i < fieldCount; i++)
      if (field[i].keyType)
      { j++;
	file.ReadBTreeHdr(tempHdl, &idxHdr, j);
        idxHdr2 = idxHdr;
	key = (char *) dbKey.GetKey(datbObj, fieldObj[i],
				    field[i].keyType, field[i].keySize);
	file.InsertKey(tempHdl, fileHdr, idxHdr, key, fileHdr.LastRecordID);
        if (memcmp(&idxHdr, &idxHdr2, sizeof(idxHdr)))
	  file.WriteBTreeHdr(tempHdl, &idxHdr, j);
      } // if
  } // while

  file.WriteFileHdr(tempHdl, &fileHdr);

  for (i = 0; i < fieldCount; i++)
    if (fieldMapPos[i] >= 0)
      AObjDestroy(fieldObj[i]);		// pseudo object
  MBFree(fieldMapPos);
  MBFree(fieldObj);

  _lclose(fileHdl);
  if (remove(datbCore->pathName))
  { free(tempName);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  _lclose(tempHdl);
  if (rename(tempName, datbCore->pathName))
  { free(tempName);

    /* ---- FLOW 2 (Failed) ---- */
    DBReportResult(isFunction, theEvent, theSystem, 2, "");
    return;
  } // if
  free(tempName);
  DBReportResult(isFunction, theEvent, theSystem);
} // ODatbRecoverDBFunc
