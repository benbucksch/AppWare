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
 | FILE		: VRFILE.H                                                |
 | DESCRIPTION	: Variable Records-length File Manager Include File.  A   |
 |		  file management for MS-Windows program to access multi- |
 |		  user accessed file at file-server.                      |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#ifndef VRFILE_H
#define VRFILE_H

#include "index.h"


#define RF_FREE		7		// free record-flag
#define RF_NODE		255		// node record-flag
#define RF_REC		26		// record record-flag

#define MAXRECSIZE	16		// maximum number of record sizes

struct Lock			/* ---- Lock ---------------------------- */
{ short int	lock;			// lock
};

struct FileID			/* ---- File ID ------------------------- */
{ char		fileID[22];		// "Object Database File\32\0"
  long		version;		// version of the database object
};

#define posofFileHdr	(sizeof(FileID) + sizeof(Lock))
					// position of FileHdr

struct FileHdr			/* ---- File Header --------------------- */
{ long		recStart;		// the beginning of file mngmt area
  long		nextAvailRec;		// next 64K avail record to allocate
  long		freeRecList[MAXRECSIZE + 1];
					// a list of allocated free records
  long		LastRecordID;		// last record ID assigned
  long		Records;		// number of records in database file
  long		Indexes;		// number of indexes
  long		Fields;			// number of fields when file created
  char		spare[8];		// spare for future compatibility
};

struct VRRecHdr			/* ---- Var Recs-len Record Header ------ */
{ char		type;			// record type: RF_REC or RF_NODE
  char		size;			// size of record in RECSIZE[size]
  long		len;			// actual record length
  long		next;			// next record pointer
};

#define posofVRFreeRecHdrprev  4	// position of prev in VRFreeRecHdr
#define posofVRFreeRecHdrnext  8	// position of next in VRFreeRecHdr

struct VRFreeRecHdr		/* ---- Var Recs-len Free Record Header -- */
{ short int	lock;			// lock = 0 in this version
  char		type;			// record type: RF_FREE
  char		size;			// size of record in RECSIZE[size]
  long		prev;			// previous free record header
  long		next;			// next free record header
};

class VRFile			/* ---- Var Recs-len File manager ------- */
{ static long	RECSIZE[MAXRECSIZE + 1];// array of record sizes

  void	AddList(HANDLE, FileHdr &, long, int);
  /* Adds free record RECPTR to the front of FILEHDR.freeRecList[RECSIZE]. of
     file associated with handle FILEHDL.
  */

  long	DelFreeRec(HANDLE, FileHdr &);
  /* Returns record pointer to the record removed from FILEHDR.freeRecStart of
     file associated with FILEHDL.
  */

  long	DelList(HANDLE, FileHdr &, int);
  /* Returns record pointer to the record removed from
     fileHdr.freeRecHdr[RECSIZE] of file associated with handle FILEHDL.
  */

  void	Error(int);
  /* Reports error messages. */

  int	GetBestRecSize(unsigned);
  /* Returns the smallest integer i: 4..16, such that RECSIZE[i] >= RECLEN. */

  int	GetBestRecSizeSearch(unsigned, int, int);
  /* Returns the idx of RECSIZE[LOW..HIGH], where RECSIZE[idx] >= RECLEN. */

  long	GetBuddyRecPtr(long, long, int);
  /* Returns buddy record pointer of RECPTR of size RECSIZE. */

  BOOL	IsGoodRecPtr(HANDLE, long, long, long);
  /* Returns TRUE if record RECPTR is a good record pointer, else
     returns FALSE.
  */

  long	LiberateRec(HANDLE, FileHdr &, long, VRRecHdr &);
  /* Returns record pointer to the new size liberated record. */

  void	ReadFreeRecHdr(HANDLE, long, VRFreeRecHdr *);
  /* Reads free record header FREERECHDR of RECPTR from file associated with
     handle FILEHDL.
  */

  BOOL	ReadFreeRecHdr2(HANDLE, long, VRFreeRecHdr *);
  /* Reads free record header FREERECHDR of RECPTR from file associated with
     handle FILEHDL.
  */

  void	ReadRecHdr(HANDLE, long, VRRecHdr *);
  /* Reads record header RECHDR of RECPTR from file associated with handle
     FILEHDL.
  */

  void	Seek(HANDLE, long);
  /* Sets file pointer to new position OFFSET bytes beyond the file location
     from file beginning of file associated with handle FILEHDL.
  */

  void	SplitRec(HANDLE, FileHdr &, long, VRRecHdr &);
  /* Splits the upper half of the record RECPTR without checking the record
     boundary of file associated with handle FILEHDL.
  */

  void	WriteFreeRecHdr(HANDLE, long, VRFreeRecHdr *);
  /* Writes free record header FREERECHDR of RECPTR into file associated with
     handle FILEHDL.
  */

  void  WriteFreeRecHdrNext(HANDLE, long, long *);
  /* Writes free record header NEXT field of RECPTR into file associated with
     handle FILEHDL.
  */

  void	WriteFreeRecHdrPrev(HANDLE, long, long *);
  /* Writes free record header PREV field of RECPTR into file associated with
     handle FILEHDL.
  */

  void	WriteRecHdr(HANDLE, long, VRRecHdr *);
  /* Writes record header RECHDR of RECPTR into file associated with handle
     FILEHDL.
  */

protected:
  void ReadRec(HANDLE, long, void *, unsigned);
  /* Reads record RECPTR of length RECLEN from file associated with handle
     FILEHDL into buffer RECBUF.
  */

  void WriteRec(HANDLE, long, void *, unsigned);
  /* Writes record RECPTR of length RECLEN from buffer RECBUF into file
     associated with handle FILEHDL.
  */

public:
  long AllocRec(HANDLE, FileHdr &, long, char = RF_REC);
  /* Returns record pointer to the newly allocated record type TYPE of length
     RECLEN from header FILEHDR of file associated with handle FILEHDL.
  */

  void FreeRec(HANDLE, FileHdr &, long);
  /* Deallocates record RECPTR to header FILEHDR of file associated with
     handle FILEHDL.
  */

  long GetNextRecPtr(HANDLE, long, long, long = 0);
  /* Returns next sequential record pointer if found, else returns 0. */

  void LockFileHdr(HANDLE);
  /* Locks file header of file associated with handle FILEHDL. */

  BOOL	LockRec(HANDLE, long);
  /* Returns TRUE or FALSE, as the result of locking record RECPTR of file
     associated with handle FILEHDL.
  */

  void LockRecHdr(HANDLE, long);
  /* Locks record header of RECPTR of file associated with handle FILEHDL. */

  void ReadBTreeHdr(HANDLE, void *, long = 0);
  /* Reads BTree header TREEHDRBUF of number TREEHDRNO from file associated
     with handle FILEHDL.
  */

  void ReadFileHdr(HANDLE, FileHdr *);
  /* Reads file header FILEHDR from file associated with handle FILEHDL. */

  void ReadRec(HANDLE, long, long, long, char *);
  /* Reads record RECPTR from offset RECOFFSET of length RECLEN to
     buffer RECBUF from file associated with FILEHDL.
  */

  long ReallocRec(HANDLE, FileHdr &, long, long);
  /* Returns record pointer of the reallocated record of size NEWRECLEN based
     on record RECPTR from file header FILEHDR of file associated with handle
     FILEHDL.
  */

  void UnlockFileHdr(HANDLE);
  /* Unlocks file header of file associated with handle FILEHDL. */

  void UnlockRec(HANDLE, long);
  /* Unlocks record RECPTR of file associated with handle FILEHDL. */

  void UnlockRecHdr(HANDLE, long);
  /* Unlocks record header of RECPTR of file associated with handle FILEHDL. */

  void WriteBTreeHdr(HANDLE, void *, long = 0);
  /* Writes BTree header TREEHDRBUF of number TREEHDRNO into file associated
     with handle FILEHDL.
  */

  void WriteFileHdr(HANDLE, FileHdr *);
  /* Writes file header FILEHDR into file associated with handle FILEHDL. */

  void WriteRec(HANDLE, long, long, long, char *);
  /* Writes record RECPTR from offset RECOFFSET of length RECLEN from
     buffer RECBUF into file associated with FILEHDL.
  */
};

#endif // VRFILE_H

