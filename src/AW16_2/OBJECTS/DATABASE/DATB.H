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
 | FILE		: MACRO.H                                                 |
 | DESCRIPTION	: Database Object Include File.                           |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef DATB_H
#define DATB_H

#define	STRICT
#include <windows.h>

#define ASTRICT
#include "a_alm.h"
#include "o_datb.h"
#include "o_text.h"
#include "o_number.h"
#include "o_date.h"
#include "o_time.h"
#include "o_pict.h"


/* ---- miscellaneous values ---- */
#define MXSTRSIZE	256		// maximum string size
#define MXVARSIZE	32		// maximum variable size

#define FILEEXTSIZE	4		// file extention size
#define SPARESSIZE	20		// spares size

/* ---- default values ---- */
#define DEF_FILEEXT	"ODB"		// default file extention
#define DEF_VARNAME	"NewName"	// default variable name
#define DEF_TEXTKEYSIZE	13		// default text key length

/* ---- Database Object signals ----------------------------------------- */
/* None */

/* ---- Database Object datablocks -------------------------------------- */
#define DATBCOUNT		2
#define DATBCORE		0
#define DATBFIELDS		1

struct Datb			/* ---- Database Core Info -------------- */
{ int		version;		// version number 
  BOOL		isRunTime;		// is run time? 

  char		defFileExt[FILEEXTSIZE];// default file extention 
  OBJECTID	recIDObj;		// record id object 

  char		spares[SPARESSIZE];	// unused spares 
};

struct Field			/* ---- Field --------------------------- */
{ OBJECTID	obj;			// object 
  TYPEID	type;			// object type 
  char		name[MXVARSIZE];	// field name 
  OBJECTID	refObj;			// reference object 
  int		keyType;		// key type
  int		keySize;		// key size 
  char		*keyVal;		// key value 
};


extern HINSTANCE	hInstance;
extern char		lpstrFilter[];

#ifdef __cplusplus
extern "C" {				/* Assume C declarations for C++ */
#endif /* __cplusplus */

long ALMAPI GetSerVersion(TYPEID);
/* Returns Database object's DLL version number. */

/* ---- Database object ---- */

long ALMAPI ODatbAdministrationRoutine(OBJECTID, pAObjMessage);
/* Database object's administration routines. */

BOOL ALMAPI ODatbObjectDialog(HWND, UINT, WPARAM, LPARAM);
/* Database object dialog. */

BOOL ALMAPI ODatbPowerSearchDialog(HWND, UINT, WPARAM, LPARAM);
/* Database Power Search dialog. */

void ALMAPI ODatbStartupFunc(pAEvtInfo, pAObjMessage);
/* Database object's constructor. */

void ALMAPI ODatbShutdownFunc(pAEvtInfo, pAObjMessage);
/* Database object's destructor. */

/* ---- Database functions ---- */

void ALMAPI ODatbAddRecordFunc(pAEvtInfo, pAObjMessage);
/* Adds a record to a database file. */

void ALMAPI ODatbAppendFoundIDsFunc(pAEvtInfo, pAObjMessage);
/* Appends a set of record IDs onto the end of the Database's private current
   list of found records.
*/

void ALMAPI ODatbCloseDBFunc(pAEvtInfo, pAObjMessage);
/* Closes a database file. */

void ALMAPI ODatbCopyFoundIDsFunc(pAEvtInfo, pAObjMessage);
/* Retrieves a copy of the Database's found record list.  The list is a Text
   object containing "\r\n" delimited record IDs.
*/

void ALMAPI ODatbCountRecordsFunc(pAEvtInfo, pAObjMessage);
/* Returns the number of records in the database file. */

void ALMAPI ODatbDBFileNameFunc(pAEvtInfo, pAObjMessage);
/* Returns the file name, and path name of the database file. */

void ALMAPI ODatbDeleteRecordFunc(pAEvtInfo, pAObjMessage);
/* Deletes the currently locked record from the database file. */

void ALMAPI ODatbFindExactRecordFunc(pAEvtInfo, pAObjMessage);
/* Finds all records that exactly match the Field Object.  The first found
   record is loaded into the Database's field objects.
*/

void ALMAPI ODatbFindRecordFunc(pAEvtInfo, pAObjMessage);
/* Finds all records that match the Field Object.  The first found record is
   loaded into the Database's field objects.
*/

void ALMAPI ODatbFindRecordIDFunc(pAEvtInfo, pAObjMessage);
/* Finds a particular record given its record ID.  The record is loaded into
   the Database's field objects.
*/

void ALMAPI ODatbGetFoundRecordFunc(pAEvtInfo, pAObjMessage);
/* Loads a found record.  Pass in the index of the found record to load. */

void ALMAPI ODatbGetNextRecordFunc(pAEvtInfo, pAObjMessage);
/* Gets the record that next to the Field Object current record.  If Field
   Object is not passed or sorted, the next chronological order record is
   read.
*/

void ALMAPI ODatbGetPreviousRecordFunc(pAEvtInfo, pAObjMessage);
/* Gets the record that previous to the Field Object current record.  If
   Field Object is not passed or sorted, the previous chronological order
   record is read. */

void ALMAPI ODatbGetRecordFunc(pAEvtInfo, pAObjMessage);
/* Gets a record from the Database.  Pass in the index into the file's
   records, sorted by the Field Object.  If Field Object is not passed or
   sorted, records is returned in order of entry.
*/

void ALMAPI ODatbLoadDBFunc(pAEvtInfo, pAObjMessage);
/* Opens a database file using a pathname instead of the user file dialog. */

void ALMAPI ODatbLockRecordFunc(pAEvtInfo, pAObjMessage);
/* Locks the current record in the Database. */

void ALMAPI ODatbMacroSearchFunc(pAEvtInfo, pAObjMessage);
/* Finds records using complex search instructions. */

void ALMAPI ODatbNewDBFunc(pAEvtInfo, pAObjMessage);
/* Creates a new database file.  If you don't pass a pathname, the create
   file dialog is displayed.
*/

void ALMAPI ODatbOpenDBFunc(pAEvtInfo, pAObjMessage);
/* Opens a database file, using the open file dialog. */

void ALMAPI ODatbPowerSearchFunc(pAEvtInfo, pAObjMessage);
/* Displays the Power Search dialog, as described in the Database
   documentation.
*/

void ALMAPI ODatbReadFoundRecordFunc(pAEvtInfo, pAObjMessage);
/* Loads a found record, without locking the record.  You can call this
   function to load a busy record.
*/

void ALMAPI ODatbReadNextRecordFunc(pAEvtInfo, pAObjMessage);
/* Reads the record that next to the Field Object current record.  If Field
   Object is not passed or sorted, the next chronological order record is
   read.
*/

void ALMAPI ODatbReadPreviousRecordFunc(pAEvtInfo, pAObjMessage);
/* Reads the record that previous to the Field Object current record.  If
   Field Object is not passed or sorted, the previous chronological order
   record is read.
*/

void ALMAPI ODatbReadRecordFunc(pAEvtInfo, pAObjMessage);
/* Reads a record from a Database, without locking the record.  Index applies
   to all records sorted with respect to Field Object.
*/

void ALMAPI ODatbRecordIndexFunc(pAEvtInfo, pAObjMessage);
/* Gets the index in the file of the current record with respect to Field
   Object.
*/

void ALMAPI ODatbRecoverDBFunc(pAEvtInfo, pAObjMessage);
/* Recovers, or reconstructs the database file. */

void ALMAPI ODatbRemoveFoundIDFunc(pAEvtInfo, pAObjMessage);
/* Removes a found ID from the Database's found list. */

void ALMAPI ODatbSetAllModeFunc(pAEvtInfo, pAObjMessage);
/* Sets mode to all mode. */

void ALMAPI ODatbSetFoundModeFunc(pAEvtInfo, pAObjMessage);
/* Sets move to found mode. */

void ALMAPI ODatbSetFoundIDsFunc(pAEvtInfo, pAObjMessage);
/* Resets the Database's record found list.  Pass in a "\r\n" delimited Text
   object containing the record IDs.
*/

void ALMAPI ODatbSortFoundFunc(pAEvtInfo, pAObjMessage);
/* Sorts the list of found records by Field Object. */

void ALMAPI ODatbStartXFunc(pAEvtInfo, pAObjMessage);
/* Establishes exclusive access to the database file. */

void ALMAPI ODatbStopXFunc(pAEvtInfo, pAObjMessage);
/* Stops exclusive access to the database file. */

void ALMAPI ODatbUnlockRecordFunc(pAEvtInfo, pAObjMessage);
/* Unlocks the current lock record. */

void ALMAPI ODatbUpdateRecordFunc(pAEvtInfo, pAObjMessage);
/* Updates the current lock record. */

/* ---- Additional Database functions ---- */

void ALMAPI ODatbCountRecsFunc(pAEvtInfo, pAObjMessage);
/* Returns the number of records in the file or found list. */

void ALMAPI ODatbGetFieldIDsFunc(pAEvtInfo, pAObjMessage);
/* Returns field OBJECTIDs stored in a memory block. */

void ALMAPI ODatbGetIndexFieldIDsFunc(pAEvtInfo, pAObjMessage);
/* Returns indexed field OBJECTIDs stored in a memory block. */

void ALMAPI ODatbReadRecordsFunc(pAEvtInfo, pAObjMessage);
/* Returns the records stored in a memory block. */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // DATB_H 
