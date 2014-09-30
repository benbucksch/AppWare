/*--------------------------------------------------------------------------*/
/* VRFile.H: Variable Records-length File manager.                          */
/* A file management for MS-Windows program to access multi-user accessed   */
/* file at file-server.                                                     */
/* Serius Corporation, June 17, 1992                                        */
/*--------------------------------------------------------------------------*/
#ifndef _VRFILE_H_
#define _VRFILE_H_

#include "index.h"

#define RF_FREE		7		// free record-flag
#define RF_NODE		255		// node record-flag
#define RF_REC		26		// record record-flag

#define MAXRECSIZE	16		// maximum number of record sizes

struct Lock			// Lock
{ int		lock;			// lock
};

struct FileID			// File ID
{ char		fileID[22];		// "Serius Database File\32\0"
  long		version;		// version of the database object
};

#define posofFileHdr	(sizeof(FileID) + sizeof(Lock))	// position of FileHdr
struct FileHdr			// File Header
{ long		recStart;		// the beginning of file mngmt area
  long		nextAvailRec;		// next 64K avail record to allocate
  long		freeRecList[MAXRECSIZE + 1];	// list of alloc. free records
  long		LastRecordID;		// last record ID assigned
  long		Records;		// number of records in database file
  long		Indexes;		// number of indexes
  long		Fields;			// number of fields when file created
  char		spare[8];		// spare for future compatibility
};

struct VRRecHdr			// Variable Records-length Record Header
{ char		type;			// record type: RF_REC or RF_NODE
  char		size;			// size of record in RECSIZE[size]
  long		len;			// actual record length
  long		next;			// next record pointer
};

#define posofVRFreeRecHdrprev  4	// position of prev in VRFreeRecHdr
#define posofVRFreeRecHdrnext  8	// position of next in VRFreeRecHdr
struct VRFreeRecHdr		// Variable Records-length Free Record Header
{ int		lock;			// lock = 0 in this version
  char		type;			// record type: RF_FREE
  char		size;			// size of record in RECSIZE[size]
  long		prev;			// previous free record header
  long		next;			// next free record header
};

class VRFile			// Variable Records-length File manager
{ static long	RECSIZE[MAXRECSIZE + 1];// array of record sizes

  void	AddList(int, FileHdr &, long, int);
  /* Adds free record RECPTR to the front of FILEHDR.freeRecList[RECSIZE]. of
     file associated with handle FILEHDL.
  */

  long	DelFreeRec(int, FileHdr &);
  /* Returns record pointer to the record removed from FILEHDR.freeRecStart of
     file associated with FILEHDL.
  */

  long	DelList(int, FileHdr &, int);
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

  BOOL	IsGoodRecPtr(int, long, long, long);
  /* Returns TRUE if record RECPTR is a good record pointer, else
     returns FALSE.
  */

  long	LiberateRec(int, FileHdr &, long, VRRecHdr &);
  /* Returns record pointer to the new size liberated record. */

  void	ReadFreeRecHdr(int, long, VRFreeRecHdr *);
  /* Reads free record header FREERECHDR of RECPTR from file associated with
     handle FILEHDL.
  */

  BOOL	ReadFreeRecHdr2(int, long, VRFreeRecHdr *);
  /* Reads free record header FREERECHDR of RECPTR from file associated with
     handle FILEHDL.
  */

  void	ReadRecHdr(int, long, VRRecHdr *);
  /* Reads record header RECHDR of RECPTR from file associated with handle
     FILEHDL.
  */

  void	Seek(int, long);
  /* Sets file pointer to new position OFFSET bytes beyond the file location
     from file beginning of file associated with handle FILEHDL.
  */

  void	SplitRec(int, FileHdr &, long, VRRecHdr &);
  /* Splits the upper half of the record RECPTR without checking the record
     boundary of file associated with handle FILEHDL.
  */

  void	WriteFreeRecHdr(int, long, VRFreeRecHdr *);
  /* Writes free record header FREERECHDR of RECPTR into file associated with
     handle FILEHDL.
  */

  void  WriteFreeRecHdrNext(int, long, long *);
  /* Writes free record header NEXT field of RECPTR into file associated with
     handle FILEHDL.
  */

  void	WriteFreeRecHdrPrev(int, long, long *);
  /* Writes free record header PREV field of RECPTR into file associated with
     handle FILEHDL.
  */

  void	WriteRecHdr(int, long, VRRecHdr *);
  /* Writes record header RECHDR of RECPTR into file associated with handle
     FILEHDL.
  */

protected:
  void ReadRec(int, long, void huge *, unsigned);
  /* Reads record RECPTR of length RECLEN from file associated with handle
     FILEHDL into buffer RECBUF.
  */

  void WriteRec(int, long, void huge *, unsigned);
  /* Writes record RECPTR of length RECLEN from buffer RECBUF into file
     associated with handle FILEHDL.
  */

public:
  long AllocRec(int, FileHdr &, long, char = RF_REC);
  /* Returns record pointer to the newly allocated record type TYPE of length
     RECLEN from header FILEHDR of file associated with handle FILEHDL.
  */

  void FreeRec(int, FileHdr &, long);
  /* Deallocates record RECPTR to header FILEHDR of file associated with
     handle FILEHDL.
  */

  long GetNextRecPtr(int, long, long, long = 0);
  /* Returns next sequential record pointer if found, else returns 0. */

  BOOL	IsShare(int);
  /* Returns TRUE or FALSE whether SHARE.EXE is loaded or not loaded. */

  void LockFileHdr(int);
  /* Locks file header of file associated with handle FILEHDL. */

  BOOL	LockRec(int, long);
  /* Returns TRUE or FALSE, as the result of locking record RECPTR of file
     associated with handle FILEHDL.
  */

  void LockRecHdr(int, long);
  /* Locks record header of RECPTR of file associated with handle FILEHDL. */

  void ReadBTreeHdr(int, void *, long = 0);
  /* Reads BTree header TREEHDRBUF of number TREEHDRNO from file associated
     with handle FILEHDL.
  */

  void ReadFileHdr(int, FileHdr *);
  /* Reads file header FILEHDR from file associated with handle FILEHDL. */

  void ReadRec(int, long, long, long, char huge *);
  /* Reads record RECPTR from offset RECOFFSET of length RECLEN to
     buffer RECBUF from file associated with FILEHDL.
  */

  long ReallocRec(int, FileHdr	&, long, long);
  /* Returns record pointer of the reallocated record of size NEWRECLEN based
     on record RECPTR from file header FILEHDR of file associated with handle
     FILEHDL.
  */

  void UnlockFileHdr(int);
  /* Unlocks file header of file associated with handle FILEHDL. */

  void UnlockRec(int, long);
  /* Unlocks record RECPTR of file associated with handle FILEHDL. */

  void UnlockRecHdr(int, long);
  /* Unlocks record header of RECPTR of file associated with handle FILEHDL. */

  void WriteBTreeHdr(int, void *, long = 0);
  /* Writes BTree header TREEHDRBUF of number TREEHDRNO into file associated
     with handle FILEHDL.
  */

  void WriteFileHdr(int, FileHdr *);
  /* Writes file header FILEHDR into file associated with handle FILEHDL. */

  void WriteRec(int, long, long, long, char huge *);
  /* Writes record RECPTR from offset RECOFFSET of length RECLEN from
     buffer RECBUF into file associated with FILEHDL.
  */
};

#endif
