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
 | FILE		: DATB_API.CPP                                            |
 | DESCRIPTION	: Database APIs.                                          |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	22-May-1995  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#define STRICT
#include <windows.h>
#include <stdlib.h>

#include "datb_fnc.h"
#include "memblock.h"
#include "safemem.h"
#include "btree.h"


long ALMAPI ODatbAddRecord	/* ---- ODatbAddRecord ------------------ */
( AOBJECTID	datbObj			// database object
)
/* Adds a record to the database file. */
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info
  long		fieldCount;		// number of fields
  Field		*field;			// an array of fields
  Field		*fieldPtr;		// field pointer

  BTree         file;           	// btree class
  FileHdr       fileHdr;        	// file header
  long		idx;			// index counter
  IdxHdr	idxHdr;         	// index header
  IdxHdr	idxHdr2;		// index header 2
  long          recPtr;         	// record pointer

  /* ---- Database ---- */
  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "Database\r\n"
    "ODatbAddRecord couldn't get the Database input parameter.");

  /* ---- datbcore ---- */
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  ADBG_ASSERT_E(datbCore, return 0,
    "Database\r\n"
    "ODatbAddRecord couldn't get the Database input parameter.");
  ADBG_ASSERT_E(datbCore->isRunTime, return 0,
    "Database\r\n"
    "ODatbAddRecord is not available at design-time.");

  if (datbCore->fileHdl == INVALID_HANDLE_VALUE ||
      datbCore->fileAccessMode == GENERIC_READ)
    /* ---- Flow 2 (Failed) ---- */
    return 2;

  /* ---- field ---- */
  if ((fieldCount = AObjGetDataSize(datbObj, DATBFIELDS) /
		    sizeof(Field)) != 0)
  { field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);
    ADBG_ASSERT_E(field, return 0,
      "Database\r\n"
      "ODatbAddRecord couldn't get the Database input parameter.");
  } // if

  /* ---- Add Record ---- */
  if (!datbCore->isExclusive)
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

  if (!datbCore->isExclusive)
    file.UnlockFileHdr(datbCore->fileHdl);

  datbCore->recID = fileHdr.LastRecordID;
  DBPostEvent(datbCore);

  /* ---- Flow 1 (Added) ---- */
  return 1;
} // ODatbAddRecord

long ALMAPI ODatbAppendFoundIDs	/* ---- ODatbAppendFoundIDs ------------- */
( AOBJECTID	datbObj,		// database object
  AMEMBLOCKID	recIDsMB,		// record IDs memblock
  long		*recIDsCount		// number of record IDs
)
/* Appends new "\r\n" delimited record IDs onto the end of the Database's
   found record IDs.
*/
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info
  char		*recIDsStr;		// record IDs string
  long		*recIDsPtr;		// record IDs pointer
  long		recID;			// record ID

  long		i;			// counter

  /* ---- Database ---- */
  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "Database\r\n"
    "ODatbAppendFoundIDs couldn't get the Database input parameter.");

  /* ---- datbcore ---- */
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  ADBG_ASSERT_E(datbCore, return 0,
    "Database\r\n"
    "ODatbAppendFoundIDs couldn't get the Database input parameter.");
  ADBG_ASSERT_E(datbCore->isRunTime, return 0,
    "Database\r\n"
    "ODatbAppendFoundIDs is not available at design-time.");

  /* ---- Record IDs Memblock ---- */
  recIDsStr = (char *) mb.GetPointer(recIDsMB);
  ADBG_ASSERT_E(recIDsStr, return 0,
    "Database\r\n"
    "ODatbAppendFoundIDs couldn't get the Record IDs input parameter.");

  /* ---- Record IDs Count ---- */
  ADBG_ASSERT_E(recIDsCount, return 0,
    "Database\r\n"
    "ODatbAppendFoundIDs couldn't get the Count output parameter.");

  /* ---- Append Found IDs ---- */
  do
  { /* ---- adds new record IDs to found-list ---- */
    if ((recID = strtol(recIDsStr, &recIDsStr, 10)) > 0)
    { for (i = 0, recIDsPtr = datbCore->found;
	   i < datbCore->foundCount && *recIDsPtr != recID;
	   i++, recIDsPtr++)
	/* do nothing */;
      if (i == datbCore->foundCount)
      { if (datbCore->foundCount % FOUNDBLOCK == 0)
	  datbCore->found = (long *) MBRealloc(datbCore->found,
	    (datbCore->foundCount + FOUNDBLOCK) * sizeof(*datbCore->found));
	*(datbCore->found + datbCore->foundCount) = recID;
	datbCore->foundCount++;
      } // if
    } // if
  } while (*recIDsStr++ == '\r' && *recIDsStr++ == '\n');

  *recIDsCount = datbCore->foundCount;

  /* ---- Flow 1 (Done) ---- */
  return 1;
} // ODatbAppendFoundIDs

long ALMAPI ODatbCloseDatabase	/* ---- ODatbCloseDatabase -------------- */
( AOBJECTID	datbObj			// database object
)
/* Closes a database file. */
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  /* ---- Database ---- */
  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "Database\r\n"
    "ODatbCloseDatabase couldn't get the Database input parameter.");

  /* ---- datbcore ---- */
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  ADBG_ASSERT_E(datbCore, return 0,
    "Database\r\n"
    "ODatbCloseDatabase couldn't get the Database input parameter.");
  ADBG_ASSERT_E(datbCore->isRunTime, return 0,
    "Database\r\n"
    "ODatbCloseDatabase is not available at design-time.");

  /* ---- Closes Database ---- */
  if (!CloseDBFile(datbCore))
    datbCore->fileHdl = INVALID_HANDLE_VALUE;	// closes the file anyway

  /* ---- Flow 1 (Done) ---- */
  return 1;
} // ODatbCloseDatabase

long ALMAPI ODatbCopyFoundIDs	/* ---- ODatbCopyFoundIDs --------------- */
( AOBJECTID	datbObj,		// database object
  AMEMBLOCKID	recIDsMB		// record IDs memblock
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info
  long		dataSize;		// data size
  char		*data;			// "\r\n" delimited str of rec IDs
  int		strLen;         	// string length
  char		str[32];       		 // string
  long		*recIDsPtr;		// record IDs pointer

  long		i;			// counter

  /* ---- Database ---- */
  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "Database\r\n"
    "ODatbCopyFoundIDs couldn't get the Database input parameter.");

  /* ---- datbcore ---- */
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  ADBG_ASSERT_E(datbCore, return 0,
    "Database\r\n"
    "ODatbCopyFoundIDs couldn't get the Database input parameter.");
  ADBG_ASSERT_E(datbCore->isRunTime, return 0,
    "Database\r\n"
    "ODatbCopyFoundIDs is not available at design-time.");

  /* ---- Record IDs Memblock ---- */
  ADBG_ASSERT_E(recIDsMB, return 0,
    "Database\r\n"
    "ODatbCopyFoundIDs couldn't get the Record IDs output parameter.");

  /* ---- Copy Found IDs ---- */

  /* ---- figures out the size ---- */
  if (datbCore->foundCount > 1)		// "\r\n"
    dataSize = 2 * (datbCore->foundCount - 1) + 1;
  else
    dataSize = 1;			// 0 terminated string

  for (i = 0, recIDsPtr = datbCore->found; i < datbCore->foundCount;
       i++, recIDsPtr++)
    dataSize += lstrlen(ltoa(*recIDsPtr, str, 10));

  /* ---- reallocates the size ---- */
  MemResize(recIDsMB, dataSize);
  data = (char *) mb.GetPointer(recIDsMB);

  /* ---- copies record IDs ---- */
  for (i = 0, recIDsPtr = datbCore->found; i < datbCore->foundCount;
       i++, recIDsPtr++)
  { strLen = lstrlen(ltoa(*recIDsPtr, str, 10));
    memcpy(data, str, strLen);
    data = (char *) data + strLen;

    if (i < datbCore->foundCount - 1)	// "\r\n"
    { *data++ = '\r';
      *data++ = '\n';
    } // if
  } // for
  *data = '\0';				// 0 terminated string

  /* ---- Flow 1 (Done) ---- */
  return 1;
} // ODatbCopyFoundIDs

long ALMAPI ODatbCountRecords	/* ---- ODatbCountRecords --------------- */
( AOBJECTID	datbObj,		// database object
  long		*recCount		// number of records
)
/* Returns the number of records in the database file. */
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  BTree		file;			// btree class
  FileHdr	fileHdr;		// file header

  /* ---- Database ---- */
  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "Database\r\n"
    "ODatbCountRecords couldn't get the Database input parameter.");

  /* ---- datbcore ---- */
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  ADBG_ASSERT_E(datbCore, return 0,
    "Database\r\n"
    "ODatbCountRecords couldn't get the Database input parameter.");
  ADBG_ASSERT_E(datbCore->isRunTime, return 0,
    "Database\r\n"
    "ODatbCountRecords is not available at design-time.");

  /* ---- Count Records ---- */
  if (datbCore->fileHdl == INVALID_HANDLE_VALUE)
    *recCount = 0;
  else
  { if (!datbCore->isExclusive)
      file.LockFileHdr(datbCore->fileHdl);

    file.ReadFileHdr(datbCore->fileHdl, &fileHdr);
    *recCount = fileHdr.Records;

    if (!datbCore->isExclusive)
      file.UnlockFileHdr(datbCore->fileHdl);
  } // else

  /* ---- Flow 1 (Done) ---- */
  return 1;			       
} // ODatbCountRecords

long ALMAPI ODatbDatabaseFileName /* --- ODatbDatabaseFileName ---------- */
( AOBJECTID	datbObj,		// database object
  AMEMBLOCKID	fileNameMB,		// file name memblock
  AMEMBLOCKID	pathNameMB		// path name memblock
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;
  (void) fileNameMB;
  (void) pathNameMB;

  return 1;
} // ODatbDatabaseFileName

long ALMAPI ODatbDeleteRecord	/* ---- ODatbDeleteRecord --------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbDeleteRecord

long ALMAPI ODatbFindExactRecord /* --- ODatbFindExactRecord ------------ */
( AOBJECTID	datbObj,		// database object
  AOBJECTID	fieldObj,		// field object
  long		*recCount		// number of records
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;
  (void) fieldObj;
  (void) *recCount;	

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbFindRecord	/* ---- ODatbFindRecord ----------------- */
( AOBJECTID	datbObj,		// database object
  AOBJECTID	fieldObj,		// field object
  long		*recCount		// number of records
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;
  (void) fieldObj;
  (void) *recCount;

  return 1;
} // ODatbFindRecord

long ALMAPI ODatbFindRecordID	/* ---- ODatbFindRecordID --------------- */
( AOBJECTID	datbObj,		// database object
  long		recID			// record ID
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;
  (void) recID;

  return 1;
} // ODatbFindRecordID

long ALMAPI ODatbGetFieldsObjectIDs /*  ODatbGetFieldsObjectIDs --------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbGetFoundIDsCount /* -- ODatbGetFoundIDsCount ----------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbGetFoundIDsIndex /* -- ODatbGetFoundIDsIndex ----------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbGetFoundRecord	/* ---- ODatbGetFoundRecord ------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbGetKeysObjectIDs /* -- ODatbGetKeysObjectIDs ----------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbGetNextRecord	/* ---- ODatbGetNextRecord -------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbGetPreviousRecord /* - ODatbGetPreviousRecord ---------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbGetRecord	/* ---- ODatbGetRecord ------------------ */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbIsDatabaseOpen	/* ---- ODatbIsDatabaseOpen ------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbIsDatabaseOpen

long ALMAPI ODatbIsRecordLock	/* ---- ODatbIsRecordLock --------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbIsRecordLock

long ALMAPI ODatbLoadDatabase	/* ---- ODatbLoadDatabase --------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbLoadDatabase

long ALMAPI ODatbLockRecord	/* ---- ODatbLockRecord ----------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbLockRecord

long ALMAPI ODatbMacroSearch	/* ---- ODatbMacroSearch ---------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbMacroSearch

long ALMAPI ODatbNewDatabase	/* ---- ODatbNewDatabase ---------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbNewDatabase

long ALMAPI ODatbOpenDatabase	/* ---- ODatbOpenDatabase --------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbOpenDatabase

long ALMAPI ODatbPowerSearch	/* ---- ODatbPowerSearch ---------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbPowerSearch

long ALMAPI ODatbReadFoundRecord /* --- ODatbReadFoundRecord ------------ */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbReadFoundRecord

long ALMAPI ODatbReadNextRecord	/* ---- ODatbReadNextRecord ------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbReadNextRecord

long ALMAPI ODatbReadPreviousRecord /*  ODatbReadPreviousRecord --------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbReadPreviousRecord

long ALMAPI ODatbReadRecord	/* ---- ODatbReadRecord ----------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbReadRecord

long ALMAPI ODatbRecordIndex	/* ---- ODatbRecordIndex ---------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbRecordIndex

long ALMAPI ODatbRecoverDatabase /* --- ODatbRecoverDatabase ------------ */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbRemoveFoundID	/* ---- ODatbRemoveFoundID -------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbFindExactRecord

long ALMAPI ODatbSetFoundIDs	/* ---- ODatbSetFoundIDs ---------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbSetFoundIDs

long ALMAPI ODatbSortFound	/* ---- ODatbSortFound ------------------ */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbSortFound

long ALMAPI ODatbStartExclusive	/* ---- ODatbStartExclusive ------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbStartExclusive

long ALMAPI ODatbStopExclusive	/* ---- ODatbStopExclusive -------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbStopExclusive

long ALMAPI ODatbUnlockRecord	/* ---- ODatbUnlockRecord --------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbUnlockRecord

long ALMAPI ODatbUpdateRecord	/* ---- ODatbUpdateRecord --------------- */
( AOBJECTID	datbObj			// database object
)
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbAppendFoundIDs requires Database object.");
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  (void) datbCore;

  return 1;
} // ODatbUpdateRecord

