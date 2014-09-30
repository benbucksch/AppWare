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
 | FILE		: DATB_MTH.CPP                                            |
 | DESCRIPTION	: Database Object Methods.                                |
 | COPYRIGHT	: 1993-95 Novell, Inc.                                    |
 | EDIT LOG	:                                                         |
 |   v1.20.00	  mw	23-Mar-1995  Initial Edit.  The result of         |
 |				     splitting datb_fnc.cpp.		  |
\*------------------------------------------------------------------------*/
#include <io.h>
#include <stdio.h>
#include "datb_fnc.h"
#include "memblock.h"
#include "safemem.h"
#include "btree.h"
#include "cmpfunc.h"


void ConcatField		/* ---- Concat Field -------------------- */
( AMEMBLOCKID	recMB,			// record memory block
  AMEMBLOCKID	fieldMB			// field memory block
)
/* Concats the field into the record. */
{ MemBlock	mb;			// memblock class
  long		recSize;		// record size
  char		*rec;			// record

  char		dummy[1] = {0};		// a dummy
  long		fieldSize = 1;		// field size
  char		*field = dummy;		// field
  char		*tempField;		// temporary field

  /* ---- gets field ---- */
  if (fieldMB)
  { tempField = (char *) mb.GetPointer(fieldMB);
    if (tempField)
    { field = tempField;
      fieldSize = lstrlen(field) + 1;
    } // if
  } // if

  /* ---- resizes record for adding field ---- */
  recSize = AMemGetSize(recMB);
  MemResize(recMB, recSize + fieldSize);

  /* ---- adds field to record ---- */
  rec = (char *) mb.GetPointer(recMB);
  memcpy(rec + recSize, field, fieldSize);
} // ConcatField

AMEMBLOCKID BuildRecord		/* ---- Build Record -------------------- */
( long			recPtr,		// record pointer
  DatbCore		datb,		// datb core info
  int			fieldObjCount,	// number of field objects
  int			fieldObjPos[]	// an array of field object positions
)
/* Returns AMEMBLOCKID of '\0'-delimited record string. */
{ AMEMBLOCKID		recMB;		// record memory block
  AMEMBLOCKID		tempMB;		// temporary memory block
  AOBJECTID		tempObj;	// temporary object
  char			*data;		// data block
  char			str[32];	// dummy string

  BTree			file;		// btree class
  long			recOffset;	// record offset
  long			recFieldCount;	// number of record fields
  ObjHdr		objHdr;		// object header
  long			dataSize;	// data size

  int			i, j;		// counters

  recMB = MemAllocate();

  /* ---- reads the record header ---- */
  file.ReadRec(datb.fileHdl, recPtr, 0,
	       sizeof(recFieldCount), (char *) &recFieldCount);

  /* ---- reads the record ---- */
  for (i = 0; i < fieldObjCount; i++)
  { MemBlock		mb;		// memblock class

    /* ---- adds a field ---- */
    if (fieldObjPos[i] <= 0 || fieldObjPos[i] > recFieldCount)
    { /* ---- adds an empty field ---- */
      ConcatField(recMB, 0);
      continue;
    } // if

    /* ---- reads the field header ---- */
    file.ReadRec(datb.fileHdl, recPtr, fieldObjPos[i] * sizeof(long),
		 sizeof(recOffset), (char *) &recOffset);
    file.ReadRec(datb.fileHdl, recPtr, recOffset,
		 sizeof(ObjHdr), (char *) &objHdr);
    recOffset += sizeof(ObjHdr);

    switch (objHdr.objType)
    { case (long)OTYPE_TEXT:
	file.ReadRec(datb.fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	tempMB = mb.Allocate(dataSize);
	data = (char *) mb.GetPointer(tempMB);
	file.ReadRec(datb.fileHdl, recPtr, recOffset, dataSize, data);
	break;
      case (long)OTYPE_NUMBER:
	file.ReadRec(datb.fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	tempMB = mb.Allocate(0);
	tempObj = mb.Create(OTYPE_NUMBER);
	file.ReadRec(datb.fileHdl, recPtr, recOffset, dataSize, str);
	if (-objHdr.dataCount == ODATB_KEYLONGDOUBLE)
	  ONmbrSetReal(tempObj, *(long double *) str, TRUE);
	else
	  ONmbrSetInteger(tempObj, *(long *) str, TRUE);

	AObjConvertToText(tempObj, tempMB);
	break;
      case (long)OTYPE_DATE:
	file.ReadRec(datb.fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	tempMB = mb.Allocate(0);
	tempObj = mb.Create(OTYPE_DATE);
	file.ReadRec(datb.fileHdl, recPtr, recOffset, dataSize, str);
	ODateSetValue(tempObj, *(long *) str);

	AObjConvertToText(tempObj, tempMB);
	break;
      case (long)OTYPE_TIME:
	file.ReadRec(datb.fileHdl, recPtr, recOffset,
		     sizeof(dataSize), (char *) &dataSize);
	recOffset += sizeof(dataSize);

	tempMB = mb.Allocate(0);
	tempObj = mb.Create(OTYPE_TIME);
	file.ReadRec(datb.fileHdl, recPtr, recOffset, dataSize, str);
	OTimeSetValue(tempObj, *(long *) str);

	AObjConvertToText(tempObj, tempMB);
	break;
      default:
	tempMB = mb.Allocate(0);
	tempObj = mb.Create((ATYPEID) objHdr.objType);
	AObjSetDataCount(tempObj, objHdr.dataCount);
	for (j = 0; j < objHdr.dataCount; j++)
	{ /* ---- reads each data block ---- */
	  file.ReadRec(datb.fileHdl, recPtr, recOffset,
		       sizeof(dataSize), (char *) &dataSize);
	  recOffset += sizeof(dataSize);

	  MemResizeObjectData(tempObj, j, dataSize);
	  data = (char *) mb.GetPointer(tempObj, j);
	  file.ReadRec(datb.fileHdl, recPtr, recOffset, dataSize, data);
	  recOffset += dataSize;
	} // for

	AObjConvertToText(tempObj, tempMB);
	break;
    } // switch
    ConcatField(recMB, tempMB);
  } // for
  return recMB;
} // BuildRecord

void ConcatRecID		/* ---- Concatenate Record ID ----------- */
( AMEMBLOCKID		recIDsMB,	// record ids memory block
  long			recID		// record id
)
/* Concats the record id to the record id memory block. */
{ MemBlock		mb;		// memblock class
  long			recIDsMBSize;	// record id memory block size
  char			*recIDs;	// record ids

  recIDsMBSize = AMemGetSize(recIDsMB);
  MemResize(recIDsMB, recIDsMBSize + sizeof(long));
  recIDs = (char *) mb.GetPointer(recIDsMB);

  *(long *)((char *) recIDs + recIDsMBSize) = recID;
} // ConcatRecID

void ConcatRecord		/* ---- Concat Record --------------------- */
( AMEMBLOCKID		destMB,		// destination memory block
  AMEMBLOCKID		srcMB		// source memory block
)
/* Concats source memory block into destination memory block. */
{ MemBlock		mb;		// memblock class
  long			destSize;	// destination size
  char			*dest;		// destination
  long			srcSize;	// source size
  char			*src;		// source

  srcSize = AMemGetSize(srcMB);
  src = (char *) mb.GetPointer(srcMB);

  destSize = AMemGetSize(destMB);
  MemResize(destMB, destSize + srcSize);
  dest = (char *) mb.GetPointer(destMB);

  memcpy((char *) dest + destSize, src, srcSize);
} // ConcatRecord

int GetFieldPos			/* ---- Get Field Position -------------- */
( AOBJECTID	fieldObj,		// the field object
  int		fieldCount,		// number of fields
  Field		*field			// an array of fields
)
/* Returns the field position of the field. */
{ int		i;			// counter

  for (i = 0; i < fieldCount; i++)
    if (fieldObj == field[i].obj)
      return i + 1;
  return 0;
} // GetFieldPos


/*--------------------------------------------------------------------------*\
 | FUNCTION	: ODatbCountRecsFunc                                        |
 | DESCRIPTION	: Returns the number of records in the file, or found list. |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbCountRecsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock		mb;		// memblock class
  AOBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  long			*recCount;	// number of records		(IO:2)

  BTree                 file;           // btree class
  FileHdr               fileHdr;        // file header

  /* ---- INPUT 1 ---- */
  datbObj = theEvent->targetObject;

  /* ---- INPUT 2 ---- */
  recCount = (long *) theEvent->details;

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return,
    "ODatbCountRecsFunc requires Database object.");
  ADBG_ASSERT_E(recCount, return,
    "ODatbCountRecsFunc requires Number of records.");

  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  if (!datbCore->isRunTime)
  { *recCount = -1;
    return;
  } // if

  if (datbCore->mode == MODE_FOUND)
  { *recCount = datbCore->foundCount;
    return;
  } // if
  if (datbCore->fileHdl < 0)		// file is not open
  { *recCount = -1;
    return;
  } // if
  if (!datbCore->isExclusive)		// file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);
  if (!datbCore->isExclusive)		// file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);
  *recCount = fileHdr.Records;
} // ODatbCountRecsFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetFieldIDsFunc                                      |
 | DESCRIPTION  : Returns field OBJECTIDs stored in a memory block.         |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetFieldIDsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock		mb;		// memblock class
  AOBJECTID              datbObj;	// datb object			(I: 1)
  AMEMBLOCKID		fieldObjMB;	// OBJECIDs memory block	(IO:2)
  AOBJECTID		*fieldObj;	// an array of OBJECTIDs
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs

  int			i;		// counter

  /* ---- INPUT 1 ---- */
  datbObj = theEvent->targetObject;

  /* ---- INPUT 2 ---- */
  fieldObjMB = (AMEMBLOCKID) theEvent->details;

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return,
    "ODatbGetFieldIDsFunc requires Database object.");
  ADBG_ASSERT_E(AMemSetSize(fieldObjMB, 0), return,
    "ODatbGetFieldIDsFunc requires Field Object IDs memory block.");

  fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  /* ---- copies OBJECTIDs ---- */
  MemResize(fieldObjMB, fieldCount * sizeof(AOBJECTID));
  fieldObj = (AOBJECTID *) mb.GetPointer(fieldObjMB);

  for (i = 0; i < fieldCount; i++)
    if (AObjCheckType(field[i].obj, field[i].type))
      *fieldObj++ = field[i].obj;
} // ODatbGetFieldIDsFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION	: ODatbGetIndexFieldIDsFunc                                 |
 | DESCRIPTION	: Returns indexed field OBJECTIDs stored in a memory block. |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetIndexFieldIDsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock		mb;		// memblock class
  AOBJECTID		datbObj;	// datb object			(I: 1)
  AMEMBLOCKID		idxObjMB;	// index OBJECTIDs memory block	(IO:2)
  int			idxObjCount;	// number of index OBJECTIDs
  AOBJECTID		*idxObj;	// an array of index OBJECTIDs
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs

  int			i;		// counter

  /* ---- INPUT 1 ---- */
  datbObj = theEvent->targetObject;

  /* ---- INPUT 2 ---- */
  idxObjMB = (AMEMBLOCKID) theEvent->details;

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return,
    "ODatbGetIndexFieldIDsFunc requires Database object.");
  ADBG_ASSERT_E(AMemSetSize(idxObjMB, 0), return,
    "ODatbGetIndexFieldIDsFunc requires Field Object IDs memory block.");

  fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);
  for (idxObjCount = 0, i = 0; i < fieldCount; i++)
    if (field[i].keyType != ODATB_KEYQUERY)
      idxObjCount++;

  /* ---- copies OBJECTIDs ---- */
  MemResize(idxObjMB, idxObjCount * sizeof(AOBJECTID));
  idxObj = (AOBJECTID *) mb.GetPointer(idxObjMB);
  if (idxObjCount)
    for (i = 0; i < fieldCount; i++)
      if (field[i].keyType != ODATB_KEYQUERY &&
	  AObjCheckType(field[i].obj, field[i].type))
	*idxObj++ = field[i].obj;
} // ODatbGetIndexFieldIDsFunc

/*--------------------------------------------------------------------------*\
 | FUNCTION	: ODatbReadRecordsFunc                                      |
 | DESCRIPTION	: Returns the records stored in a memory block.             |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbReadRecordsFunc
( pAEvtInfo		theEvent,	// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock		mb;		// memblock class
  AOBJECTID		datbObj;	// datb object			(I: 1)
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  ODatbReadRecords	*table;		// table			(I: 2)
  int			fieldObjCount;	// number of field objects
  AOBJECTID		*fieldObj;	// field objects
  int			*fieldObjPos;	// field objects' positions

  BTree			file;		// btree class
  int			idx;		// index counter
  int			idxLoc;		// index location
  IdxHdr		idxHdr;		// index header
  long			recID;		// record ID
  long			recPtr;		// record pointer

  long			relIdx;		// relative index
  long			idxPos;		// index position
  long			*recIDs;	// record IDs

  AMEMBLOCKID		recMB;		// record memory block

  long			i, j;		// counters

  int			(*cmp)(char *, char *);	// the compare function

  /* ---- INPUT 1 ---- */
  datbObj = theEvent->targetObject;

  /* ---- INPUT 2 ---- */
  table = (ODatbReadRecords *) theEvent->details;

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return,
    "ODatbReadRecordsFunc requires Database object.");
  ADBG_ASSERT_E(table, return,
    "ODatbReadRecordsFunc requires ODatbReadRecords.");

  /* ---- CHECKS INPUT 1 (Database) ---- */
  if (AObjGetType(datbObj) != OTYPE_DATABASE)
  { table->retCode = -1;		// serious error
    return;
  } // if
  if ((datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE)) == 0)
  { table->retCode = -1;		// serious error
    return;
  } // if
  if (!datbCore->isRunTime || datbCore->fileHdl < 0)
  { table->retCode = -2;		// unexpected error
    return;
  } // if

  if ((fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) /
			   sizeof(Field))) != 0)
    if ((field = (Field *) mb.GetPointer(datbObj, DATBFIELDS)) == 0)
    { table->retCode = -1;		// serious error
      return;
    } // if

  /* ---- CHECKS INPUT 2 (READRECORDS) ---- */
  if (!table)
  { table->retCode = -1;		// serious error
    return;
  } // if
  if (table->recCount <= 0)		// nothing to retrieve
  { table->retCode = -2;		// unexpected error
    return;
  } // if

  /* ---- gets objects' mapping position ---- */
  fieldObjCount = (int) (AMemGetSize(table->fieldObjs) / sizeof(AOBJECTID));
  if (fieldObjCount)
  { if ((fieldObj = (AOBJECTID *) mb.GetPointer(table->fieldObjs)) == 0)
    { table->retCode = -1;		// serious error
      return;
    } // if
    fieldObjPos = (int *) MBAlloc(fieldObjCount * sizeof(*fieldObjPos));
    for (i = 0; i < fieldObjCount; i++)
      fieldObjPos[(int) i] =
	GetFieldPos(fieldObj[(int) i], fieldCount, field);
  } // if
  else
    fieldObjPos = 0;
  relIdx = table->relIdx;

  if (datbCore->mode == MODE_FOUND)
  { /* ---- FOUND MODE ---- */
    switch (table->readType)
    { case SEEK_SET:
	idxPos = relIdx < 0? -relIdx : relIdx;
	break;
      case SEEK_END:
	idxPos = datbCore->foundCount + (relIdx < 0? relIdx : -relIdx);
	break;
      case SEEK_CUR:
      default:
	idxPos = datbCore->foundIdx + (relIdx < 0? relIdx + 1 : relIdx);
	break;
    } // switch
    if (idxPos <= 0)
      idxPos = 1;
    if (idxPos > datbCore->foundCount)
    { table->retCode = -2;		// unexpected error
      return;
    } // if

    if (!datbCore->isExclusive)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    cmp = GetCmpExactFunc(idxHdr.keyType);

    j = 0;
    for (i = (int) (idxPos - 1);
	 i < datbCore->foundCount && j < table->recCount; i++)
      if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *)
	    (datbCore->found + i), cmp)) != 0)
      { /* ---- get record ---- */
	recMB = BuildRecord(recPtr, *datbCore, fieldObjCount, fieldObjPos);
	ConcatRecord(table->recs, recMB);
	ConcatRecID(table->recIDs, *(datbCore->found + 1));
	AMemFree(recMB);
	j++;
      } // if

    if (!datbCore->isExclusive)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
    MBFree(fieldObjPos);
    datbCore->foundIdx = i;
    table->retCode = j;
    table->currRecIdx = j;
    return;
  } // if

  /* ---- ALL MODE ---- */
  if (!datbCore->isExclusive)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);

  if (table->idxObj)
  { /* ---- reads records based on an indexed field ---- */
    idxLoc = 1;
    for (idx = 0; idx < fieldCount && field[idx].obj != table->idxObj; idx++)
      if (field[idx].keyType)
	idxLoc++;
    if (idx == fieldCount || !field[idx].keyType)
    { if (!datbCore->isExclusive)		// the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);
      table->retCode = -2;		// the object is not indexed
      return;
    } // if

    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxLoc);
    switch (table->readType)
    { case SEEK_SET:
	if ((recID = file.LocateFrontRelN(datbCore->fileHdl, &idxHdr,
	               relIdx)) == 0)
	{ if (!datbCore->isExclusive)	// the file is not lock
	    file.UnlockFileHdr(datbCore->fileHdl);
	  table->retCode = 0;		// 0 rows read
	  return;
	} // if
	break;
      case SEEK_END:
	if ((recID = file.LocateRearRelN(datbCore->fileHdl, &idxHdr,
	               relIdx)) == 0)
	{ if (!datbCore->isExclusive)	// the file is not lock
	    file.UnlockFileHdr(datbCore->fileHdl);
	  table->retCode = 0;		// 0 rows read
	  return;
	} // if
	break;
      case SEEK_CUR:
      default:
	if ((recID = file.LocateRelN(datbCore->fileHdl, &idxHdr,
		       field[idx].keyVal, datbCore->recID, relIdx)) == 0)
	{ if (!datbCore->isExclusive)	// the file is not lock
	    file.UnlockFileHdr(datbCore->fileHdl);
	  table->retCode = 0;		// 0 rows read
	  return;
	} // if
	break;
    } // switch
    ConcatRecID(table->recIDs, recID);
    for (i = 1;
	 i < table->recCount && (recID = file.NextRecPtr(datbCore->fileHdl)) != 0;
	 i++)
      ConcatRecID(table->recIDs, recID);
    table->retCode = i;
    recIDs = (long *) mb.GetPointer(table->recIDs);

    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    cmp = GetCmpExactFunc(idxHdr.keyType);
    for (i = 0; i < table->retCode; i++)
    { if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr, (char *)
                      (recIDs + i), cmp)) == 0)
      { /* ---- the file has been corrupted ---- */
	if (!datbCore->isExclusive)
	  file.UnlockFileHdr(datbCore->fileHdl);
	MBFree(fieldObjPos);
	AMemSetSize(table->recIDs, i * sizeof(long));
	table->retCode = i;
	return;
      } //if

      /* ---- gets the record ---- */
      recMB = BuildRecord(recPtr, *datbCore, fieldObjCount, fieldObjPos);
      ConcatRecord(table->recs, recMB);
      AMemFree(recMB);
      if (i == 0)
      { UnlockRecord(datbCore);
	ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);
					// loads record into real objects
	datbCore->recID = *(recIDs + i);
	datbCore->recPtr = recPtr;
	table->currRecIdx = 1;
      } // if
    } // for
  } // if
  else
  { /* ---- reads records based on chronological order ---- */
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    switch (table->readType)
    { case SEEK_SET:
	if ((recPtr = file.LocateFrontRelN(datbCore->fileHdl, &idxHdr,
	                relIdx)) == 0)
	{ if (!datbCore->isExclusive)	// the file is not lock
	    file.UnlockFileHdr(datbCore->fileHdl);
	  MBFree(fieldObjPos);
	  table->retCode = 0;		// 0 rows read
	  return;
	} // if
	break;
      case SEEK_END:
	if ((recPtr = file.LocateRearRelN(datbCore->fileHdl, &idxHdr,
	                relIdx)) == 0)
	{ if (!datbCore->isExclusive)	// the file is not lock
	    file.UnlockFileHdr(datbCore->fileHdl);
	  MBFree(fieldObjPos);
	  table->retCode = 0;		// 0 rows read
	  return;
	} // if
	break;
      case SEEK_CUR:
      default:
	if ((recPtr = file.LocateRelN(datbCore->fileHdl, &idxHdr,
			field[idx].keyVal, datbCore->recID, relIdx)) == 0)
	{ if (!datbCore->isExclusive)	// the file is not lock
	    file.UnlockFileHdr(datbCore->fileHdl);
	  MBFree(fieldObjPos);
	  table->retCode = 0;		// 0 rows read
	  return;
	} // if
	break;
    } // switch
    if (recPtr)
    { UnlockRecord(datbCore);
      ReadRecord(recPtr, datbObj, datbCore, fieldCount, field);
					// loads record into real objects
      datbCore->recID = recID;
      datbCore->recPtr = recPtr;
      table->currRecIdx = 1;
    } // if
    for (i = 0; i < table->recCount && recPtr; i++)
    { /* ---- gets record ---- */
      recMB = BuildRecord(recPtr, *datbCore, fieldObjCount, fieldObjPos);
      ConcatRecord(table->recs, recMB);
      AMemFree(recMB);

      file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
      ConcatRecID(table->recIDs, recID);
      recPtr = file.NextRecPtr(datbCore->fileHdl);
    } // for
  } // else

  if (!datbCore->isExclusive)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);
  MBFree(fieldObjPos);
  table->retCode = i;
} // ODatbReadRecordsFunc

/* ==== Database direct functions ========================================= */

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetFieldObjectIDs                                    |
 | DESCRIPTION  : Retrieves field object IDs to the memory block.           |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetFieldObjectIDs
( AOBJECTID		datbObj,	// I: datb object
  AMEMBLOCKID		fieldObjIDs	// IO: field OBJECTIDs
)
/* Retrieves an array of field object IDs to the memory block. */
{ MemBlock		mb;		// memblock class
  AOBJECTID		*fieldObj;	// an array of OBJECTIDs
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs

  int			i;		// counter

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return,
    "ODatbGetFieldObjectIDs requires Database object.");
  ADBG_ASSERT_E(AMemSetSize(fieldObjIDs, 0), return,
    "ODatbGetFieldObjectIDs requires Field Object IDs memory block.");

  fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  /* ---- copies OBJECTIDs ---- */
  MemResize(fieldObjIDs, fieldCount * sizeof(AOBJECTID));
  fieldObj = (AOBJECTID *) mb.GetPointer(fieldObjIDs);

  for (i = 0; i < fieldCount; i++)
    if (AObjCheckType(field[i].obj, field[i].type))
      *fieldObj++ = field[i].obj;
} // ODatbGetFieldObjectIDs

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetFoundCount                                        |
 | DESCRIPTION  : Returns the number of records in the found list.          |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
long ALMAPI ODatbGetFoundCount
( AOBJECTID		datbObj		// I: datb object
)
/* Returns the number of records in the found list. */
{ MemBlock		mb;		// memblock class
  DatbCore		*datbCore;	// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbGetFoundCount requires Database object.");

  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  if (!datbCore->isRunTime)
    return 0;

  return datbCore->foundCount;
} // ODatbGetFoundCount

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetFoundIndex                                        |
 | DESCRIPTION  : Returns the index (0-origin) of the record in the found   |
 |		  list.  A return value of -1 indicates that the function   |
 |		  cannot find the record.                                   |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
long ALMAPI ODatbGetFoundIndex
( AOBJECTID		datbObj		// I: datb object
)
/* Returns the index (0-origin) of the record in the found list.
   A return value of -1 indicates that the function cannot find the record.
*/
{ MemBlock		mb;		// memblock class
  DatbCore		*datbCore;	// datb core info
  long			*recID;		// an array of record IDs

  long			lI;		// counter

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return -1,
    "ODatbGetFoundIndex requires Database object.");

  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  if (!datbCore->isRunTime || datbCore->fileHdl < 0 ||
      datbCore->recID == 0)		// no current record ID
    return -1;				// cannot find

  recID = datbCore->found;
  for (lI = 0; lI < datbCore->foundCount; lI++)
    if (*recID++ == datbCore->recID)
      return lI;
  return -1;
} // ODatbGetFoundIndex

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetKeyObjectIDs                                      |
 | DESCRIPTION  : Retrieves key object IDs to the memory block.             |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
void ALMAPI ODatbGetKeyObjectIDs
( AOBJECTID		datbObj,	// I: datb object
  AMEMBLOCKID		keyObjIDs	// IO: key OBJECTIDs
)
/* Retrieves an array of key object IDs to the memory block. */
{ MemBlock		mb;		// memblock class
  int			fieldCount;	// number of FIELDs
  Field			*field;		// an array of FIELDs
  int			keyObjCount;	// number of OBJECTIDs
  AOBJECTID		*keyObj;	// an array of OBJECTIDs

  int			i;		// counter

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return,
    "ODatbGetKeyObjectIDs requires Database object.");
  ADBG_ASSERT_E(AMemSetSize(keyObjIDs, 0), return,
    "ODatbGetKeyObjectIDs requires Key Object IDs memory block.");

  fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);
  for (keyObjCount = 0, i = 0; i < fieldCount; i++)
    if (field[i].keyType != ODATB_KEYQUERY)
      keyObjCount++;

  /* ---- copies OBJECTIDs ---- */
  MemResize(keyObjIDs, keyObjCount * sizeof(AOBJECTID));
  keyObj = (AOBJECTID *) mb.GetPointer(keyObjIDs);
  if (keyObjCount)
    for (i = 0; i < fieldCount; i++)
      if (field[i].keyType != ODATB_KEYQUERY &&
	  AObjCheckType(field[i].obj, field[i].type))
	*keyObj++ = field[i].obj;
} // ODatbGetKeyObjectIDs

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbGetRecCount                                          |
 | DESCRIPTION  : Returns the number of records in the file.                |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
long ALMAPI ODatbGetRecCount
( AOBJECTID		datbObj		// I: datb object
)
/* Returns the number of records in the file. */
{ MemBlock		mb;		// memblock class
  DatbCore		*datbCore;	// datb core info

  BTree			file;		// btree class
  FileHdr		fileHdr;	// file header

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return 0,
    "ODatbGetRecCount requires Database object.");

  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  if (!datbCore->isRunTime || datbCore->fileHdl < 0)
    return 0;

  if (!datbCore->isExclusive)		// file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadFileHdr(datbCore->fileHdl, &fileHdr);
  if (!datbCore->isExclusive)		// file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);
  return fileHdr.Records;
} // ODatbGetRecCount

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbIsFileOpen                                           |
 | DESCRIPTION  : Returns TRUE when the file is open.  Otherwise, returns   |
 |		  FALSE.                                                    |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
BOOL ALMAPI ODatbIsFileOpen
( AOBJECTID		datbObj		// I: datb object
)
/* Returns TRUE when the file is open.  Otherwise, returns FALSE. */
{ MemBlock		mb;		// memblock class
  DatbCore		*datbCore;	// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return FALSE,
    "ODatbIsFileOpen requires Database object.");

  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  if (!datbCore->isRunTime || datbCore->fileHdl < 0)
    return FALSE;

  return datbCore->fileHdl < 0? FALSE : TRUE;
} // ODatbIsFileOpen

/*--------------------------------------------------------------------------*\
 | FUNCTION     : ODatbIsRecLock                                            |
 | DESCRIPTION  : Returns TRUE when the record is lock.  Otherwise, returns |
 |		  FALSE.                                                    |
 | Novell, Inc., September 1, 1993.					mw  |
\*--------------------------------------------------------------------------*/
BOOL ALMAPI ODatbIsRecLock
( AOBJECTID		datbObj		// I: datb object
)
/* Returns TRUE when the record is lock.  Otherwise, returns FALSE. */
{ MemBlock		mb;		// memblock class
  DatbCore		*datbCore;	// datb core info

  ADBG_ASSERT_E(AObjCheckType(datbObj, OTYPE_DATABASE), return FALSE,
    "ODatbIsRecLock requires Database object.");

  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  if (!datbCore->isRunTime || datbCore->fileHdl < 0)
    return FALSE;

  return datbCore->isRecLock;
} // ODatbIsRecLock
