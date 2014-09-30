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
 | FILE		: DATB_FNC.H                                              |
 | DESCRIPTION	: Database Object Functions Include File.                 |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef DATB_FNC_H
#define DATB_FNC_H

#include "datb.h"


/* ---- miscellaneous values ---- */
#define PATHNAMESIZE	80
#define FOUNDBLOCK	64

/* ---- default values ---- */
#define DEF_FILENAME	"untitled"

/* ---- database mode ---- */
#define MODE_ALL	0
#define MODE_FOUND	1

struct DatbCore			/* ---- Database Core Info -------------- */
{ /* ---- design-time (public) attributes ------------------------------- */
  int		version;		// version number 
  BOOL		isRunTime;		// is run time? 

  char		defFileExt[FILEEXTSIZE];// default file extention 
  OBJECTID	recIDObj;		// record id object 

  char		spares[SPARESSIZE];	// unused spares 

  /* ---- run-time attributes ------------------------------------------- */
  /* ---- protected attributes ---- */
  int		filterIndex;		// filter index
  char		pathName[PATHNAMESIZE];	// path name

  /* ---- private attributes ---- */
  int		mode;			// MODE_ALL or MODE_FOUND

  long		foundCount;		// number of records in found list
  long		*found;			// found list (an array of rec ids)
  long		foundIdx;		// current found index

  int		fileHdl;		// file handle
  int		accessMode;		// ACCESS_SHARED or ACCESS_EXCLUSIVE
  long		recPtr;			// current record pointer
  long		recID;			// current record id
  BOOL		isRecLock;		// is current record lock?

  int		fileAccessMode;		// file access mode
};

/* Field */			/* ---- Database Field ------------------ */


struct	ObjHdr			/* ---- Object Header ------------------- */
{ long		objType;		// object type
  long		dataCount;              // data count
};


BOOL CreateFile(char *, int, int, Field *);
/* Returns TRUE if Object Database File is created, else returns FALSE. */

BOOL OpenFile(DatbCore *, int, Field *);
/* Returns TRUE if the Object Database File is opened, else returns FALSE.
*/

BOOL CloseFile(DatbCore *);
/* Returns TRUE if the current file is close, else returns FALSE. */

void DBPostEvent(DatbCore *);
/* Database Function posts an event. */

void DBReportResult(BOOL, pAEvtInfo, pAObjMessage, int = 1, char * = NULL);
/* Database object reports result. */

BOOL LockRecord(long, DatbCore *);
/* Returns TRUE if the current record is lock, else returns FALSE. */

void UnlockRecord(DatbCore *);
/* Unlocks the current record, if any. */

OBJECTID ReadObject(int, long, int);
/* Returns the newly read object from the record, or return 0. */

void ReadRecord(long, OBJECTID, DatbCore *, int, Field *);
/* Reads the record into the objects, and loads the current keys. */

void WriteRecord(int, long, int, Field *);
/* Writes the record from the objects. */

void WriteRecord(long, OBJECTID, DatbCore *, int, Field *);
/* Writes the record from the objects, and loads the current keys. */

#endif /* DATB_FNC.H */
