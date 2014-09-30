/*                                                                          *\
    Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.

    THIS WORK IS SUBJECT TO U.S.  AND  INTERNATIONAL  COPYRIGHT  LAWS  AND
    TREATIES.  NO PART OF THIS WORK MAY  BE  USED,  PRACTICED,  PERFORMED,
    COPIED,   DISTRIBUTED,   REVISED,   MODIFIED,   TRANSLATED,  ABRIDGED,
    CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,  TRANSFORMED
    OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY  USE
    OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD  SUBJECT  THE
    PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
\*                                                                          */

/*--------------------------------------------------------------------------*\
 | FILE         : O_DATB.H                                                  |
 | DESCRIPTION  : AppWare Database Object Library Include File.             |
 | COPYRIGHT    : 1993-1994 Novell, Inc.                                    |
\*--------------------------------------------------------------------------*/
#ifndef O_DATB_H
#define O_DATB_H

/* ---- ATYPEID for Database Object ---- */
#define OTYPE_DATABASE          ATYPE_MAKEID('D','a','t','b')

/* ---- AEVENTID for Database Object ---- */
#define ODATB_EVT_GETKEY        AEVENT_MAKEID('d','b','g','t')
#define ODATB_EVT_SENDKEY       AEVENT_MAKEID('d','b','g','t')


/* ---- KEY TYPES ---- */
#define ODATB_KEYQUERY          0x0000  /* query key / no key */
#define ODATB_KEYTEXT           0x0001  /* text key */
#define ODATB_KEYLONG           0x0002  /* long key */
#define ODATB_KEYLONGDOUBLE     0x0004  /* long double key */
#define ODATB_KEYDATE           0x0008  /* date key */
#define ODATB_KEYTIME           0x0010  /* time key */

/* ---- KEY SIZE ---- */
#define ODATB_MXKEYSIZE         236     /* max 0 terminated key size */
#define ODATB_MXKEYLEN          235     /* max 0 terminated key length */

/* ---- KEY Struct for Database Object ---- */
typedef struct {
  short int     type;                   /* key type */
  char          key[ODATB_MXKEYSIZE];   /* key buffer */
} ODatbKey, far *pODatbKey;


/* ---- READ TYPES ---- */
#define ODATB_READSET           0       /* reads from the first record */
#define ODATB_READCUR           1       /* reads from the current record */
#define ODATB_READEND           2       /* reads from the last record */

/* ---- ERROR CODE ---- */
#define ODATB_ERRORSERIOUS      -1      /* serious */
#define ODATB_ERRORUNEXPECTED   -2      /* unexpected */

/* ---- READ RECORDS Struct ---- */
typedef struct {
  AMEMBLOCKID    fieldObjs;              /* I: the field objects requested */
  AOBJECTID      idxObj;                 /* I: the indexed field object */
  long          relIdx;                 /* I: relative index to current rec */
  int           readType;               /* I: READ TYPES */
  long          recCount;               /* I: number of records requested */
  AMEMBLOCKID    recs;                   /* O: '\t' and '\0' delimited recs */
  AMEMBLOCKID    recIDs;                 /* O: record IDs */
  long          retCode;                /* O: # of rows copied / ERROR CODE */
  long          currRecIdx;             /* O: current rec idx (found mode) */
} ODatbReadRecords, far *pODatbReadRecords;


/* ---- METHOD PARAMS Struct ---- */
typedef struct {
  AOBJECTID      obj[3];                 /* IO: objects */
  int           flowIdx;                /* O:  flow index */
} ODatbMethodParams, far *pODatbMethodParams;


#ifdef __cplusplus
extern "C" {                            /* Assume C declarations for C++ */
#endif /* __cplusplus */

/* ==== Database direct functions ========================================= */

void ALMAPI ODatbGetFieldObjectIDs (
    AOBJECTID            datbObj,                /* I: Database object */
    AMEMBLOCKID          fieldObjIDs);           /* IO: Field OBJECTIDs */
/* Retrieves field object IDs to the memory block. */

long ALMAPI ODatbGetFoundCount (
    AOBJECTID            datbObj);               /* I: Database object */
/* Returns the number of records in the found list. */

long ALMAPI ODatbGetFoundIndex (
    AOBJECTID            datbObj);               /* I: Database object */
/* Returns the index (0-origin) of the record in the found list.
   A return value of -1 indicates that the function cannot find the record.
*/

void ALMAPI ODatbGetKeyObjectIDs (
    AOBJECTID            datbObj,                /* I: Database object */
    AMEMBLOCKID          keyObjIDs);             /* IO: Key OBJECTIDs */
/* Retrieves key object IDs to the memory block. */

long ALMAPI ODatbGetRecCount (
    AOBJECTID            datbObj);               /* I: Database object */
/* Returns the number of records in the file. */

BOOL ALMAPI ODatbIsFileOpen (
    AOBJECTID            datbObj);               /* I: Database object */
/* Returns TRUE when the file is open.  Otherwise, returns FALSE. */

BOOL ALMAPI ODatbIsRecLock (
    AOBJECTID            datbObj);               /* I: Database object */
/* Returns TRUE when the record is lock.  Otherwise, returns FALSE. */


/* ==== Database APIs =================================================== */

long ALMAPI ODatbAddRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbAppendFoundIDs (
    AOBJECTID            datbObj,                /* I: Database object */
    AMEMBLOCKID          recIDsMB,               /* I: Record IDs memblock */
    long                *recIDCount);           /* O: Number of record IDs */

long ALMAPI ODatbCloseDatabase (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbCopyFoundIDs (
    AOBJECTID            datbObj,                /* I: Database object */
    AMEMBLOCKID          recIDsMB);              /* O: Record IDs memblock */

long ALMAPI ODatbCountRecords (
    AOBJECTID            datbObj,                /* I: Database object */
    long                *recCount);             /* O: Number of records */

long ALMAPI ODatbDatabaseFileName (
    AOBJECTID            datbObj,                /* I: Database object */
    AMEMBLOCKID          fileNameMB,             /* O: File name memblock */
    AMEMBLOCKID          pathNameMB);            /* O: Path name memblock */

long ALMAPI ODatbDeleteRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbFindExactRecord (
    AOBJECTID            datbObj,                /* I: Database object */
    AOBJECTID            fieldObj,               /* I: Field object */
    long                *recCount);             /* O: # of records found */

long ALMAPI ODatbFindRecord (
    AOBJECTID            datbObj,                /* I: Database object */
    AOBJECTID            fieldObj,               /* I: Field object */
    long                *recCount);             /* O: # of records found */

long ALMAPI ODatbFindRecordID (
    AOBJECTID            datbObj,                /* I: Database object */
    long                recID);                 /* I: Record ID */



long ALMAPI ODatbGetFieldsObjectIDs (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbGetFoundIDsCount (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbGetFoundIDsIndex (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbGetFoundRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbGetKeysObjectIDs (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbGetNextRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbGetPreviousRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbGetRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbIsDatabaseOpen (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbIsRecordLock (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbLoadDatabase (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbLockRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbMacroSearch (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbNewDatabase (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbOpenDatabase (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbPowerSearch (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbReadFoundRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbReadNextRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbReadPreviousRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbReadRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbRecordIndex (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbRecoverDatabase (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbRemoveFoundID (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbSetFoundIDs (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbSortFound (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbStartExclusive (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbStopExclusive (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbUnlockRecord (
    AOBJECTID            datbObj);               /* I: Database object */

long ALMAPI ODatbUpdateRecord (
    AOBJECTID            datbObj);               /* I: Database object */


/* ==== Database methods ================================================ */

/*--------------------------------------------------------------------------*\
  ODatbAddRecordFunc adds a record to a database file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
	    O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbAppendFoundIDsFunc appends a set of record IDs onto the end of the
    Database's private current list of found records.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
	    I: obj[1] = Record IDs
            O: obj[2] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbCloseDBFunc closes a database file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbCopyFoundIDsFunc retrieves a copy of the Database's found record
    list.  The list is a Text object containing "\r\n" delimited record IDs.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: obj[1] = Record IDs
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbCountRecordsFunc returns the number of records in the database file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: obj[1] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbDBFileNameFunc returns the file name, and path name of the database
    file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: obj[1] = File Name
            O: obj[2] = Path Name
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbDeleteRecordFunc deletes the currently locked record from the
    database file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbFindExactRecordFunc finds all records that exactly match the Field
    Object.  The first found record is loaded into the Database's field
    objects.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = File Object
            O: obj[2] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbFindRecordFunc finds all records that match the Field Object.  The
    first found record is loaded into the Database's field objects.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = File Object
            O: obj[2] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbFindRecordIDFunc finds a particular record given its record ID.  The
    record is loaded into the Database's field objects.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = ID (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbGetFoundRecordFunc loads a found record.  Pass in the Index of the
    found record to load.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Index (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbGetNextRecordFunc gets the record that next to the Field Object
    current record.  If Field Object is not passed or sorted, the next
    chronological order record is read.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object (Opt)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbGetPreviousRecordFunc gets the record that previous to the Field
    Object current record.  If Field Object is not passed or sorted, the
    previous chronological order record is read.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object (Opt)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbGetRecordFunc gets a record from the Database.  Pass in the Index
    into the file's records, sorted by Field Object.  If Field Object is not
    passed or sorted, records are returned in order of entry.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object (Opt)
            I: obj[2] = Index (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbLoadDBFunc opens a database file using a pathname instead of the
    user file dialog.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Pathname
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbLockRecordFunc locks the current record in the Database.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbMacroSearchFunc finds records using complex search instructions.
    See Database documentation.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Instructions
            O: obj[2] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbNewDBFunc creates a new database file.  If you don't pass a pathname,
    the create file dialog is displayed.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Pathname
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbOpenDBFunc opens a database file, using the open file dialog.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbPowerSearchFunc displays the Power Search dialog, as described in
    the Database documentation.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: obj[1] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbReadFoundRecordFunc loads a found record, without locking the record.
    You can call this function to load a busy record.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Index (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbReadNextRecordFunc reads the record that next to the Field Object
    current record.  If Field Object is not passed or sorted, the next
    chronological order record is read.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object (Opt)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbReadPreviousRecordFunc reads the record that previous to the Field
    Object current record.  If Field Object is not passed or sorted, the
    previous chronological order record is read.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object (Opt)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbReadRecordFunc reads a record from a Database, without locking the
    record.  Index applies to all records sorted with respect to Field
    Object.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object (Opt)
            I: obj[2] = Index (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbRecordIndexFunc gets the index in the file of the current record
    with respect to Field Object.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object (Opt)
            O: obj[2] = Index (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbRecoverDBFunc recovers, or reconstructs the database file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Pathname (Opt)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbRemoveFoundIDFunc removes a found ID from the Database's found list.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Index (long)
            O: obj[2] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbSetAllModeFunc sets mode to all mode.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbSetFoundIDsFunc resets the Database's record found list.  Pass in
    a "\r\n" delimited Text object containing the record IDs.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Record IDs
            O: obj[2] = Count (long)
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbSetFoundModeFunc sets mode to found mode.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details      I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbSortFoundFunc sorts the list of found records by Field Object.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            I: obj[1] = Field Object
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbStartXFunc establishes exclusive access to the database file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbStopXFunc stops exclusive access to the database file.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbUnlockRecordFunc unlocks the current lock record.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbUpdateRecordFunc updates the current lock record.
  The parameters are:
    pODatbMethodParams  pAEvtInfo->details  I: obj[0] = Database
            O: flowIdx = Flows
\*--------------------------------------------------------------------------*/

/* ==== Additonal Database methods ======================================== */

/*--------------------------------------------------------------------------*\
  ODatbCountRecsFunc returns the number of records in file, or found list.
  The parameters are:
    AOBJECTID            pAEvtInfo->targetObject I: Database object
    long *              pAEvtInfo->details      IO: Number of records
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbGetFieldIDsFunc returns field OBJECTIDs stored in a memory block.
  The parameters are:
    AOBJECTID            pAEvtInfo->targetObject I: Database object
    AMEMBLOCKID          pAEvtInfo->details      IO: Field OBJECTIDs
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbGetIndexFieldIDsFunc returns indexed field OBJECTIDs stored in a
    memory block.
  The parameters are:
    AOBJECTID            pAEvtInfo->targetObject I: Database object
    AMEMBLOCKID          pAEvtInfo->details      IO: Indexed field OBJECTIDs
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
  ODatbReadRecordsFunc returns the records stored in a memory block.
  The parameters are:
    AOBJECTID            pAEvtInfo->targetObject I: Database object
    pODatbReadRecord    pAEvtInfo->details      IO: Read records parameters
\*--------------------------------------------------------------------------*/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* O_DATB_H */
