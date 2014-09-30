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
 | FILE		: VRFILE.CPP                                              |
 | DESCRIPTION	: Variable Records-length File Manager.  A file           |
 |		  management for MS-Windows program to access multi-user  |
 |		  accessed file at file-server.                           |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#include <windows.h>
#include <dos.h>
#include <sys\locking.h>
#include <io.h>
#include <errno.h>
#include <a_alm.h>

#include "vrfile.h"
#include "safemem.h"


long VRFile::RECSIZE[] = {    1L,     2L,     4L,     8L,    16L,    32L,
     64L,   128L,   256L,   512L,  1024L,  2048L,  4096L,  8192L, 16384L,
  32768L, 65536L};

void VRFile::AddList		/* ---- Add List -------------------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  long		recPtr,			// record pointer
  int		recSize			// record size
)
/* Adds free record RECPTR to the front of FILEHDR.FreeRecordList[RECSIZE]. of
   file associated with handle FILEHDL.
*/
{ VRFreeRecHdr	freeRecHdr;		// free record header

  if (fileHdr.freeRecList[recSize])	// not an empty list
    WriteFreeRecHdrPrev(fileHdl, fileHdr.freeRecList[recSize], &recPtr);
  freeRecHdr.lock = 0;
  freeRecHdr.type = RF_FREE;
  freeRecHdr.size = (char) recSize;
  freeRecHdr.prev = 0;
  freeRecHdr.next = fileHdr.freeRecList[recSize];
  WriteFreeRecHdr(fileHdl, recPtr, &freeRecHdr);
  fileHdr.freeRecList[recSize] = recPtr;
} // AddList

long VRFile::DelFreeRec		/* ---- Delete Free Record Start ---------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr		// file header
)
/* Returns record pointer to the record removed from FILEHDR.freeRecStart of
   file associated with handle FILEHDL.
*/
{ long		recPtr;			// record pointer
  VRFreeRecHdr	freeRecHdr;		// free record header

  recPtr = fileHdr.nextAvailRec + sizeof(Lock) + sizeof(VRRecHdr);
  freeRecHdr.lock = 0;
  WriteFreeRecHdr(fileHdl, recPtr, &freeRecHdr);
  fileHdr.nextAvailRec += RECSIZE[MAXRECSIZE];
  return recPtr;
} // DelFreeRec

long VRFile::DelList		/* ---- Delete List ----------------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  int		recSize			// record size
)
/* Returns record pointer to the record removed from
   fileHdr.freeRecHdr[RECSIZE] of file associated with handle FILEHDL.
*/
{ long		freeRecPtr;		// free record pointer
  VRFreeRecHdr	freeRecHdr;		// free record header

  if ((freeRecPtr = fileHdr.freeRecList[recSize]) == 0)	// an empty list
    return 0;
  ReadFreeRecHdr(fileHdl, freeRecPtr, &freeRecHdr);
  fileHdr.freeRecList[recSize] = freeRecHdr.next;
  return freeRecPtr;
} // DelList

void VRFile::Error		/* ---- Error ----------------------------- */
( int		errNo			// global variable errno
)
/* Reports an error messages. */
{ switch (errNo)
  { case EACCES :
      MessageBox(GetActiveWindow(), "File access denied.  "
	"You may switch to Windows to fix the file before continuing.",
	"File Error", MB_ICONSTOP | MB_OK);
      break;
    case EBADF  : //printf("Bad file number\n"); break;
///    case EINVACC: //printf("Invalid access code\n"); break;   /// can't find definition
    case EINVAL : //printf("Invalid argument\n"); break;
    case EMFILE : //printf("Too many open files\n"); break;
    case ENOENT : //printf("No such file or directory\n"); break;
    default     :
      MessageBox(GetActiveWindow(), "File access denied.  "
	"You may switch to Windows to fix the file before continuing.",
	"File Error", MB_ICONSTOP | MB_OK);
      break;
  } // switch
} // Error

int VRFile::GetBestRecSize	/* ---- Get Best Record Size -------------- */
( unsigned	recLen			// record length
)
/* Returns the smallest integer i: 4..16, such that RECSIZE[i] >= RECLEN. */
{
  return GetBestRecSizeSearch(recLen, 4, 16);
} // GetBestRecSize

int VRFile::GetBestRecSizeSearch/* ---- Get Best Record Size Search ------- */
( unsigned	recLen,			// record length
  int		low,			// from index
  int		high			// to index
)
/* Returns the index of RECSIZE[LOW..HIGH], where RECSIZE[index] >= RECLEN. */
{
  if (low > high)
    return low;    // found
  return(RECSIZE[(low + high) >> 1] > recLen?
	   GetBestRecSizeSearch(recLen, low, ((low + high) >> 1) - 1) :
	   GetBestRecSizeSearch(recLen, ((low + high) >> 1) + 1, high));
} // GetBestRecSizeSearch

long VRFile::GetBuddyRecPtr	/* ---- Get Buddy Record Pointer ---------- */
( long		recStart,		// record start
  long		recPtr,			// record pointer
  int		recSize			// record size
)
/* Returns buddy record pointer of RECPTR of size RECSIZE. */
{ long		ptr;			// pointer

  // first record starts at RECSTART + sizeof(Lock) + sizeof(VRRecHdr) from
  // file beginning, each chunk is RECSIZE[MAXRECSIZE]
  ptr = (recPtr - recStart - sizeof(Lock) - sizeof(VRRecHdr)) %
	  RECSIZE[MAXRECSIZE];
  return ptr % RECSIZE[recSize + 1]?
	   recPtr - RECSIZE[recSize] : recPtr + RECSIZE[recSize];
} // GetBuddyRecPtr

long VRFile::LiberateRec	/* ---- Liberate Record ------------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  long		recPtr,			// record pointer
  VRRecHdr	&recHdr			// record header
)
/* Returns record pointer to the new size liberated record. */
{ long		budPtr;			// buddy record pointer
  VRFreeRecHdr	budHdr;			// buddy record header
  int		isRead;			// is read?

  if (recHdr.size < MAXRECSIZE)
  { budPtr = GetBuddyRecPtr(fileHdr.recStart, recPtr, recHdr.size);
    isRead = ReadFreeRecHdr2(fileHdl, budPtr, &budHdr);
    if (isRead && budHdr.type == RF_FREE && budHdr.size == recHdr.size)
    { /* liberates record with its buddy by deleting nodes from the list */
      if (fileHdr.freeRecList[recHdr.size] == budPtr)
	fileHdr.freeRecList[recHdr.size] = budHdr.next;	// front first block
      else
	WriteFreeRecHdrNext(fileHdl, budHdr.prev, &budHdr.next);
      if (budHdr.next)
	WriteFreeRecHdrPrev(fileHdl, budHdr.next, &budHdr.prev);
      recHdr.size++;
      if (budPtr < recPtr)    // little buddy
	return budPtr;
    } // if
  } // if
  return recPtr;
} // LiberateRec

void VRFile::ReadFreeRecHdr	/* ---- Read Free Record Header ----------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  VRFreeRecHdr	*freeRecHdr		// free record header
)
/* Reads free record header FREERECHDR of RECPTR from file associated with
   handle FILEHDL.
*/
{ DWORD		bytesCount;		// number of bytes

  Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr));

  while (!ReadFile(fileHdl, freeRecHdr, sizeof(VRFreeRecHdr),
		   &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr));
  } // while
} // ReadFreeRecHdr

BOOL VRFile::ReadFreeRecHdr2	/* ---- Read Free Record Header 2 --------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  VRFreeRecHdr	*freeRecHdr		// free record header
)
/* Reads free record header FREERECHDR of RECPTR from file associated with
   handle FILEHDL.
*/
{ DWORD		bytesCount;

  Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr));
  if (!ReadFile(fileHdl, freeRecHdr, sizeof(VRFreeRecHdr),
		&bytesCount, NULL))
    return FALSE;	// assumes the record is lock by other user
  return TRUE;
} // ReadFreeRecHdr

void VRFile::ReadRecHdr		/* ---- Read Record Header ---------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  VRRecHdr	*recHdr			// record header
)
/* Reads record header RECHDR of RECPTR from file associated with handle
   FILEHDL.
*/
{ DWORD		bytesCount;		// number of bytes

  Seek(fileHdl, recPtr - sizeof(VRRecHdr));
  while (!ReadFile(fileHdl, recHdr, sizeof(VRRecHdr),
		   &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr - sizeof(VRRecHdr));
  } // while
} // ReadRecHdr

void VRFile::Seek		/* ---- Seek ------------------------------ */
( HANDLE	fileHdl,		// file handle
  long		offset			// offset from file beginning
)
/* Sets file pointer to new position OFFSET bytes beyond the file location
   from file beginning of file associated with handle FILEHDL.
*/
{
  while (SetFilePointer(fileHdl, offset, NULL, FILE_BEGIN) == 0xFFFFFFFF)
    Error(errno);
} // Seek

void VRFile::SplitRec		/* ---- Split Record ---------------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  long		recPtr,			// record pointer
  VRRecHdr	&recHdr			// record header
)
/* Splits the upper half of the record RECPTR without checking the record
   boundary of file associated with handle FILEHDL.
*/
{
  recHdr.size--;
  recPtr += RECSIZE[recHdr.size];
  AddList(fileHdl, fileHdr, recPtr, recHdr.size);
} // SplitRec

void VRFile::WriteFreeRecHdr	/* ---- Write Free Record Header ---------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  VRFreeRecHdr  *freeRecHdr		// free record header
)
/* Writes free record header FREERECHDR of RECPTR into file associated with
   handle FILEHDL.
*/
{ DWORD		bytesCount;		// number of bytes

  Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr));
  while (!WriteFile(fileHdl, freeRecHdr, sizeof(VRFreeRecHdr),
		    &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr));
  } // while
} // WriteFreeRecHdr

void VRFile::WriteFreeRecHdrNext/* ---- Write Free Record Header Next ----- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  long		*next			// next record pointer
)
/* Writes free record header NEXT field of RECPTR into file associated with
   handle FILEHDL.
*/
{ DWORD		bytesCount;		// number of bytes

  Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr) + posofVRFreeRecHdrnext);
  while (WriteFile(fileHdl, next, sizeof(*next),
		   &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr) + posofVRFreeRecHdrnext);
  } // if
}  // WriteFreeRecHdrNext

void VRFile::WriteFreeRecHdrPrev/* ---- Write Free Record Header Previous - */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  long		*prev			// previous record pointer
)
/* Writes free record header PREV field of RECPTR into file associated with
   handle FILEHDL.
*/
{ DWORD		bytesCount;		// number of bytes

  Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr) + posofVRFreeRecHdrprev);
  while (!WriteFile(fileHdl, prev, sizeof(*prev), &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr - sizeof(VRFreeRecHdr) + posofVRFreeRecHdrprev);
  } // while
} // WriteFreeRecHdrPrev

void VRFile::WriteRecHdr	/* ---- Write Record Header --------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  VRRecHdr	*recHdr			// record header
)
/* Writes record header RECHDR of RECPTR into file associated with handle
   FILEHDL.
*/
{ DWORD		bytesCount;		// number of bytes

  Seek(fileHdl, recPtr - sizeof(VRRecHdr));
  while (!WriteFile(fileHdl, (char *) recHdr, sizeof(VRRecHdr),
		    &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr - sizeof(VRRecHdr));
  } // while
} // WriteRecHdr


void VRFile::ReadRec		/* ---- Read Record ----------------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  void		*recBuf,		// record buffer
  unsigned	recLen			// record length
)
/* Reads record RECPTR of length RECLEN into buffer RECBUF from file
   associated with handle FILEHDL.
*/
{ void		*ptr;			// pointer
  DWORD		bytesCount;		// number of bytes

  if (recLen == 0)			// nothing to read
    return;

  Seek(fileHdl, recPtr);
  if ((long) LOWORD((long) recBuf) + (long) recLen > 0xFFFF)
  { ptr = MemAlloc(recLen);

    while (!ReadFile(fileHdl, ptr, recLen, &bytesCount, NULL))
    { Error(errno);
      Seek(fileHdl, recPtr);
    } // while

    CopyMemory(recBuf, ptr, recLen);
    MemFree(ptr);
    return;
  } // if
  while (!ReadFile(fileHdl, recBuf, recLen, &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr);
  } // while
} // ReadRec

void VRFile::WriteRec		/* ---- Write Record ---------------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  void		*recBuf,		// record buffer
  unsigned	recLen			// record length
)
/* Writes record RECPTR of length RECLEN from buffer RECBUF into file
   associated with handle FILEHDL.
*/
{ void		*ptr;			// pointer
  DWORD		bytesCount;		// number of bytes

  if (recLen == 0)			// nothing to write
    return;

  Seek(fileHdl, recPtr);
  if ((long) LOWORD((long) recBuf) + (long) recLen > 0xFFFF)
  { ptr = MemAlloc(recLen);
    CopyMemory(ptr, recBuf, recLen);

    while (!ReadFile(fileHdl, ptr, recLen, &bytesCount, NULL))
    { Error(errno);
      Seek(fileHdl, recPtr);
    } // while

    MemFree(ptr);
    return;
  } // if
  while (!WriteFile(fileHdl, recBuf, recLen, &bytesCount, NULL))
  { Error(errno);
    Seek(fileHdl, recPtr);
  } // while
} // WriteRec


long VRFile::AllocRec		/* ---- Allocate Record ------------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  long		recLen,			// record length
  char		recType			// record type (default = RF_RECORD)
)
/* Returns record pointer to the newly allocated record type RECTYPE of
   length RECLEN from header FILEHDR of file associated with handle FILEHDL.
*/
{ long		recPtr;			// record pointer
  VRRecHdr	recHdr;			// record header
  int		recSize;		// record size
  int		i;			// loop counter

  recHdr.len = recLen;
  recLen += sizeof(Lock) + sizeof(VRRecHdr);
  recHdr.next = recLen <= RECSIZE[MAXRECSIZE]?
		  0 : AllocRec(fileHdl, fileHdr, recLen - RECSIZE[MAXRECSIZE]);
  recSize = recLen >= RECSIZE[MAXRECSIZE]?
	      MAXRECSIZE : GetBestRecSize((unsigned) recLen);
  /* allocates record from FreeRecordList of size at least equal to recSize */
  for (i = recSize; i <= MAXRECSIZE &&
		    !(recPtr = DelList(fileHdl, fileHdr, i)); i++);
  if (recPtr == 0)    // free record not found
  { /* allocates record from RecordDataStart of size MAXRECSIZE */
    i = MAXRECSIZE;
    recPtr = DelFreeRec(fileHdl, fileHdr);
  } // if
  recHdr.size = (char) i;
  recHdr.type = recType;
  for (; i > recSize; i--)    // the record is too large
    SplitRec(fileHdl, fileHdr, recPtr, recHdr);    // splits the upper half
  WriteRecHdr(fileHdl, recPtr, &recHdr);
  return recPtr;
} // AllocRec

void VRFile::FreeRec		/* ---- Free Record ----------------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  long		recPtr			// record pointer
)
/* Deallocates record RECPTR to header FILEHDR of file associated with
   handle FILEHDL.
*/
{ VRRecHdr	recHdr;			// record header
  int		recSize;		// record size
  long		recNext;		// record next pointer

  while (recPtr)
  { ReadRecHdr(fileHdl, recPtr, &recHdr);
    recNext = recHdr.next;
    do
    { recSize = recHdr.size;
      recPtr = LiberateRec(fileHdl, fileHdr, recPtr, recHdr);
    } while (recHdr.size > recSize);
    AddList(fileHdl, fileHdr, recPtr, recHdr.size);
    recPtr = recNext;
  } // while
} // FreeRec

void VRFile::LockFileHdr	/* ---- Lock File Header ------------------ */
( HANDLE	fileHdl			// file handle
)
/* Locks file header of file associated with handle FILEHDL. */
{
  while (!LockFile(fileHdl, posofFileHdr, 0, sizeof(FileHdr), 0));
} // LockFileHdr

BOOL VRFile::LockRec		/* ---- Lock Record ---------------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr			// record pointer
)
/* Returns TRUE or FALSE, as the result of locking record RECPTR of file
   associated with handle FILEHDL.
*/
{
  return !LockFile(fileHdl, recPtr - sizeof(Lock) - sizeof(VRRecHdr), 0,
	           sizeof(Lock), 0) == 0? TRUE : FALSE;
} // LockRec

void VRFile::LockRecHdr		/* ---- Lock Record Header ---------------- */
( HANDLE	fileHdl,		// file handle
  long   	recPtr			// record pointer
)
/* Locks record header of RECPTR of file associated with handle FILEHDL. */
{
  while (!LockFile(fileHdl, recPtr - sizeof(VRRecHdr), 0,
                   sizeof(VRRecHdr), 0));
} // LockRecHdr

void VRFile::ReadBTreeHdr	/* ---- Read BTree Header ----------------- */
( HANDLE	fileHdl,		// file handle
  void		*idxHdr,		// index header
  long		idxHdrNo		// index header number (default = 0)
)
/* Reads BTree header TREEHDRBUF of number TREEHDRNO from file associated
   with handle FILEHDL.
*/
{
  ReadRec(fileHdl, posofIdxHdr + idxHdrNo * sizeof(IdxHdr), idxHdr,
	  sizeof(IdxHdr));
} // ReadBTreeHdr

void VRFile::ReadFileHdr	/* ---- Read File Header ------------------ */
( HANDLE	fileHdl,		// file handle
  FileHdr	*fileHdr		// file header
)
/* Reads file header FILEHDR from file associated with handle FILEHDL. */
{
  ReadRec(fileHdl, posofFileHdr, fileHdr, sizeof(FileHdr));
} // ReadFileHdr

void VRFile::ReadRec		/* ---- Read Record ----------------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  long		recOffset,		// record offset
  long		bytes,			// number of bytes to read
  char		*recBuf			// record buffer
)
/* Reads record RECPTR from offset RECOFFSET of length RECLEN to
   buffer RECBUF from file associated with FILEHDL.
*/
{ VRRecHdr	recHdr;			// record header
  long		recLen;			// record length
  long		bufOffset;		// buffer offset

  if (bytes < 1)    // nothing to read
    return;
  ReadRecHdr(fileHdl, recPtr, &recHdr);
  if (recOffset >= recHdr.len)    // nothing to read
    return;
  recLen = RECSIZE[recHdr.size] - sizeof(VRRecHdr) - sizeof(Lock);
  while (recOffset > recLen - 1)
  { /* navigates thru chaining */
    recOffset -= recLen;
    if ((recPtr = recHdr.next) == 0)    // something must be wrong
      return;
    ReadRecHdr(fileHdl, recPtr, &recHdr);
    recLen = RECSIZE[recHdr.size] - sizeof(VRRecHdr) - sizeof(Lock);
  } // while
  if (recLen - recOffset >= bytes)
  { ReadRec(fileHdl, recPtr + recOffset, recBuf, (unsigned) bytes);
    return;
  } // if
  /* chaining reads */
  ReadRec(fileHdl, recPtr + recOffset, recBuf,
	  (unsigned) (recLen - recOffset));
  bufOffset = recLen - recOffset;    // number of bytes have read
  bytes -= bufOffset;    // decrements buffer offset
  while ((recPtr = recHdr.next) != 0 && bytes > 0)
  { ReadRecHdr(fileHdl, recPtr, &recHdr);
    recLen = RECSIZE[recHdr.size] - sizeof(VRRecHdr) - sizeof(Lock);
    if (recLen >= bytes)
    { ReadRec(fileHdl, recPtr, &recBuf[bufOffset], (unsigned) bytes);
      return;
    } // if
    /* chaining */
    ReadRec(fileHdl, recPtr, &recBuf[bufOffset], (unsigned) recLen);
    bufOffset += recLen;
    bytes -= recLen;
  } // while
} // ReadRec

long VRFile::ReallocRec		/* ---- Realloc Record -------------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  long		recPtr,			// record pointer
  long		newRecLen		// new record length
)
/* Returns record pointer of the reallocated record of size NEWRECLEN based
   on record RECPTR from file header FILEHDR of file associated with handle
   FILEHDL.  It doesn't copy the contents to the new location.
*/
{
  FreeRec(fileHdl, fileHdr, recPtr);
  return AllocRec(fileHdl, fileHdr, newRecLen);
} // ReallocRec

void VRFile::UnlockFileHdr	/* ---- Unlock File Header ---------------- */
( HANDLE	fileHdl			// file handle
)
/* Unlocks file header of file associated with handle FILEHDL. */
{
  UnlockFile(fileHdl, posofFileHdr, 0, sizeof(FileHdr), 0);
} // UnlockFileHdr

void VRFile::UnlockRec		/* ---- Unlock Record --------------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr			// record pointer
)
/* Unlocks record RECPTR of file associated with handle FILEHDL. */
{
  UnlockFile(fileHdl, recPtr - sizeof(Lock) - sizeof(VRRecHdr), 0,
	     sizeof(Lock), 0);
} // UnlockRec

void VRFile::UnlockRecHdr	/* ---- Unlock Record Header -------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr			// record pointer
)
/* Unlocks record header of RECPTR of file associated with handle FILEHDL. */
{
  UnlockFile(fileHdl, recPtr - sizeof(VRRecHdr), 0, sizeof(VRRecHdr), 0);
} // UnlockRecHdr

void VRFile::WriteBTreeHdr	/* ---- Write BTree Header ---------------- */
( HANDLE	fileHdl,		// file handle
  void		*idxHdr,		// index header
  long		idxHdrNo		// index header number
)
/* Writes BTree header TREEHDRBUF of number TREEHDRNO into file associated
   with handle FILEHDL.
*/
{
  WriteRec(fileHdl, posofIdxHdr + idxHdrNo * sizeof(IdxHdr), idxHdr,
	   sizeof(IdxHdr));
} // WriteBTreeHdr

void VRFile::WriteFileHdr	/* ---- Write File Header ----------------- */
( HANDLE	fileHdl,		// file handle
  FileHdr	*fileHdr		// file header
)
/* Writes file header FILEHDR into file associated with handle FILEHDL. */
{
  WriteRec(fileHdl, posofFileHdr, fileHdr, sizeof(FileHdr));
} // WriteFileHdr

void VRFile::WriteRec		/* ---- Write Record ---------------------- */
( HANDLE	fileHdl,		// file handle
  long		recPtr,			// record pointer
  long		recOffset,		// record offset
  long		bytes,			// number of bytes to write
  char		*recBuf			// record buffer
)
/* Writes record RECPTR from offset RECOFFSET of length RECLEN from
   buffer RECBUF into file associated with FILEHDL.
*/
{ VRRecHdr	recHdr;			// record header
  long		recLen;			// record length
  long		bufOffset;		// buffer offset

  if (bytes < 1)    // nothing to write
    return;
  ReadRecHdr(fileHdl, recPtr, &recHdr);
  if (recOffset >= recHdr.len)    // nothing to write
    return;
  recLen = RECSIZE[recHdr.size] - sizeof(VRRecHdr) - sizeof(Lock);
  while (recOffset > recLen - 1)
  { /* navigates thru chaining */
    recOffset -= recLen;
    if ((recPtr = recHdr.next) == 0)    // something is wrong
      return;
    ReadRecHdr(fileHdl, recPtr, &recHdr);
    recLen = RECSIZE[recHdr.size] - sizeof(VRRecHdr) - sizeof(Lock);
  } // while
  if (recLen - recOffset >= bytes)
  { WriteRec(fileHdl, recPtr + recOffset, recBuf, (unsigned) bytes);
    return;
  } // if
  /* chaining writes */
  WriteRec(fileHdl, recPtr + recOffset, recBuf,
	   (unsigned) (recLen - recOffset));
  bufOffset = recLen - recOffset;    // number of bytes have written
  bytes -= bufOffset;    // decrements buffer offset
  while ((recPtr = recHdr.next) != 0 && bytes > 0)
  { ReadRecHdr(fileHdl, recPtr, &recHdr);
    recLen = RECSIZE[recHdr.size] - sizeof(VRRecHdr) - sizeof(Lock);
    if (recLen >= bytes)
    { WriteRec(fileHdl, recPtr, &recBuf[bufOffset], (unsigned) bytes);
      return;
    } // if
    /* chaining */
    WriteRec(fileHdl, recPtr, &recBuf[bufOffset], (unsigned) recLen);
    bufOffset += recLen;
    bytes -= recLen;
  } // while
} // WriteRec

long VRFile::GetNextRecPtr	/* ---- Get Next Record Pointer ----------- */
( HANDLE	fileHdl,		// file handle
  long		fileLen,		// file length
  long		recStart,		// record start
  long		currRecPtr		// current record pointer
)
/* Returns next sequential record pointer if found, else returns 0. */
{ VRRecHdr		recHdr;		// record header

  if (currRecPtr == 0)
    currRecPtr = recStart + sizeof(Lock) + sizeof(VRRecHdr);
  else
  { ReadRecHdr(fileHdl, currRecPtr, &recHdr);
    currRecPtr += RECSIZE[recHdr.size];
  } // else
  while (currRecPtr < fileLen &&
	 !IsGoodRecPtr(fileHdl, fileLen, recStart, currRecPtr))
  { ReadRecHdr(fileHdl, currRecPtr, &recHdr);
    if (recHdr.size < 4 || recHdr.size > 16 ||
	(currRecPtr - recStart - sizeof(Lock) - sizeof(VRRecHdr)) % 32 ||
	((currRecPtr - recStart - sizeof(Lock) - sizeof(VRRecHdr)) %
	 RECSIZE[MAXRECSIZE]) + RECSIZE[recHdr.size] > RECSIZE[MAXRECSIZE])
      currRecPtr = currRecPtr + RECSIZE[MAXRECSIZE] -
	((currRecPtr - recStart - sizeof(Lock) - sizeof(VRRecHdr)) %
	 RECSIZE[MAXRECSIZE]);
    else
      currRecPtr += RECSIZE[recHdr.size];
  } // while
  return currRecPtr < fileLen? currRecPtr : 0;
} // GetNextRecPtr

BOOL VRFile::IsGoodRecPtr	/* ---- Is Good Record Pointer ------------ */
( HANDLE	fileHdl,		// file handle
  long		fileLen,		// file length
  long		recStart,		// record start
  long		recPtr			// record pointer
)
/* Returns TRUE if record RECPTR is a good record pointer, else
   returns FALSE.
*/
{ VRRecHdr		recHdr;		// record header

  if (recPtr >= fileLen ||
      recPtr < recStart ||
      (recPtr - recStart - sizeof(Lock) - sizeof(VRRecHdr)) % 32)
    return FALSE;

  ReadRecHdr(fileHdl, recPtr, &recHdr);
  if (recHdr.type != RF_REC ||
      recHdr.size < 4 ||
      recHdr.size > 16 ||
      recPtr + recHdr.len > fileLen)
    return FALSE;

  if (recHdr.len > RECSIZE[recHdr.size] - sizeof(Lock) - sizeof(VRRecHdr))
  { if ((recPtr - recStart - sizeof(Lock) - sizeof(VRRecHdr)) %
	  RECSIZE[MAXRECSIZE])
      return FALSE;
    return recHdr.next && recHdr.size == MAXRECSIZE?
	     IsGoodRecPtr(fileHdl, fileLen, recStart, recHdr.next) : FALSE;
  } // if
  return recHdr.next? FALSE : TRUE;
} // IsGoodRecPtr
