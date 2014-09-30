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
 | FILE		: SAFEMEM.CPP                                             |
 | DESCRIPTION	: Memory Functions with Warning Messages.                 |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#define STRICT
#include <alloc.h>

#include "safemem.h"


void MemError			/* ---- Memory Error -------------------- */
( void
)
/* Memory Error. */
{
  MessageBox(GetActiveWindow(), "Insufficient memory.  "
    "You may switch to Windows to free unused memory before continuing.",
    "Memory Error", MB_ICONSTOP | MB_OK);
} // MemError


void MemSetObjectDataCount	/* ---- Memory Set Object Data Count ---- */
( AOBJECTID	obj,			// object
  long		dataCount		// number of data
)
/* Sets the number of the datablocks for the specified object. */
{
  while (!AObjSetDataCount(obj, dataCount))
    MemError();
} // MemSetObjectDataCount

void MemResizeObjectData	/* ---- Memory Resize Object Data ------- */
( AOBJECTID	obj,			// object
  long		dataIdx,		// data index
  long		dataSize		// data size
)
/* Resizes the specified datablock. */
{
  while (!AObjResizeData(obj, dataIdx, dataSize))
    MemError();
} // MemResizeObjectData


AMEMBLOCKID MemAllocate		/* ---- Memory Allocate ----------------- */
( long		bytes			// number of bytes
)
/* Returns the AMEMBLOCKID of the allocated memory block of size BYTES. */
{ AMEMBLOCKID	memBlock;		// memory block

  while ((memBlock = AMemAllocate(bytes)) == 0)
    MemError();
  return memBlock;
} // MemAllocate

void MemResize			/* ---- Memory Resize ------------------- */
( AMEMBLOCKID	memBlock,		// memory block
  long		bytes			// number of bytes
)
/* Resizes the specifed memory block. */
{
  while (!AMemSetSize(memBlock, bytes))
    MemError();
} // MemResize


/* Global memory */

HANDLE GAlloc			/* ---- Global Allocate ----------------- */
( long		byteCount		// number of bytes
)
/* Returns the handle to the newly allocated global memory block. */
{ HANDLE	gmHdl;			// global memory handle

  if (byteCount <= 0)
    return 0;
  while ((gmHdl = GlobalAlloc(GMEM_MOVEABLE, byteCount)) == 0)
    MemError();
  return gmHdl;
} // GAlloc

HANDLE GRealloc			/* ---- Global Reallocate --------------- */
( HANDLE	gmHdl,			// global memory handle
  long		newByteCount		// number of new bytes
)
/* Returns the handle to the reallocated global memory block. */
{ HANDLE	gm2Hdl;			// global memory handle

  if (newByteCount <= 0)
  { GFree(gmHdl);
    return 0;
  } // if
  if (!gmHdl)
    return GAlloc(newByteCount);

  while ((gm2Hdl = GlobalReAlloc(gmHdl, newByteCount, GMEM_MOVEABLE)) == 0)
    MemError();
  return gm2Hdl;
} // GRealloc

void GFree			/* ---- Global Free --------------------- */
( HANDLE	gmHdl			// global memory handle
)
/* Discards the allocated global memory block. */
{
  if (gmHdl)
    GlobalFree(gmHdl);
} // GFree


/* Simple memory

   WARNING!:    Don't mix the use of the following functions with MBAlloc,
		MBRealloc, MBFree, MBSize, GMAlloc, GMRealloc, GMFree, or
		GMSize.
*/

void *MemAlloc			/* ---- Memory Allocate ----------------- */
( size_t	byteCount           	// number of bytes
)
/* Returns the pointer to the newly allocated memory block. */
{ void		*ptr;			// pointer

  if (byteCount <= 0)
    return 0;
  while ((ptr = malloc(byteCount)) == 0)
    MemError();
  return ptr;
} // MemAlloc

void *MemRealloc		/* ---- Memory Reallocate --------------- */
( void		*ptr,			// pointer
  size_t	newByteCount		// number of new bytes
)
/* Returns the pointer to the reallocated memory block. */
{ void		*ptr2;			// pointer 2

  if (newByteCount <= 0)
  { MemFree(ptr);
    return 0;
  } // if
  if (!ptr)
    return MemAlloc(newByteCount);

  while ((ptr2 = realloc(ptr, newByteCount)) == 0)
    MemError();
  return ptr2;
} // MemRealloc

void MemFree			/* ---- Memory Free --------------------- */
( void		*ptr			// pointer
)
/* Discards the allocated memory block. */
{
  if (ptr)
    free(ptr);
} // MemFree


/* AMEMBLOCKID memory

   WARNING!:	Don't mix the use of the following functions with MemAlloc,
		MemRealloc, MemFree, GMAlloc, GMRealloc, GMFree, or GMSize.
*/

void *MBAlloc			/* ---- Memory Block Allocate ----------- */
( long		byteCount		// number of bytes
)
/* Returns the pointer to the newly allocated memory block. */
{ AMEMBLOCKID	memBlock;		// memory block
  AMEMBLOCKID	*ptr;			// pointer

  if (byteCount <= 0)
    return 0;
  while ((memBlock = AMemAllocate(sizeof(AMEMBLOCKID) + byteCount)) == 0)
    MemError();

  ptr = (AMEMBLOCKID *) AMemLock(memBlock);
  *ptr = memBlock;
  return(ptr + 1);
} // MBAlloc

void *MBRealloc			/* ---- Memory Block Reallocate --------- */
( void		*ptr,			// pointer
  long		newByteCount		// number of new bytes
)
/* Returns the pointer to the reallocated memory block. */
{ AMEMBLOCKID	memBlock;		// memory block

  if (newByteCount <= 0)
  { MBFree(ptr);
    return 0;
  } // if
  if (!ptr)
    return MBAlloc(newByteCount);

  memBlock = *((AMEMBLOCKID *) ptr - 1);
  AMemUnlock(memBlock);
  while (!AMemSetSize(memBlock, sizeof(AMEMBLOCKID) + newByteCount))
    MemError();
  return((AMEMBLOCKID *) AMemLock(memBlock) + 1);
} // MBRealloc

void MBFree			/* ---- Memory Block Free --------------- */
( void		*ptr			// pointer
)
/* Discards the allocated memory block. */
{ AMEMBLOCKID	memBlock;		// memory block

  if (ptr)
  { memBlock = *((AMEMBLOCKID *) ptr - 1);
    AMemUnlock(memBlock);
    AMemFree(memBlock);
  } // if
} // MBFree

long MBGetSize			/* ---- Memory Block Get Size ----------- */
( void		*ptr			// pointer
)
/* Returns the size of the memory block. */
{
  return ptr? AMemGetSize(*((AMEMBLOCKID *) ptr - 1)) : 0;
} // MBGetSize


/* Global memory

   WARNING!:	Don't mix the use of the following functions with MemAlloc,
		MemRealloc, MemFree, MBAlloc, MBRealloc, MBFree, or MBSize.
*/

void *GMAlloc			/* ---- Global Memory Allocate ---------- */
( long		byteCount		// number of bytes
)
/* Returns the pointer to the newly allocated memory block. */
{ HANDLE	gmHdl;			// global memory handle
  HANDLE	*ptr;			// pointer

  if (byteCount <= 0)
    return 0;
  while ((gmHdl = GlobalAlloc(GMEM_MOVEABLE, sizeof(HANDLE) + byteCount)) ==
	 0)
    MemError();

  ptr = (HANDLE *) GlobalLock(gmHdl);
  *ptr = gmHdl;
  return(ptr + 1);
} // GMAlloc

void *GMRealloc			/* ---- Global Memory Reallocate -------- */
( void		*ptr,			// pointer
  long		newByteCount		// number of new bytes
)
/* Returns the pointer to the reallocated memory block. */
{ HANDLE	gmHdl;			// global memory handle
  HANDLE	gm2Hdl;			// global memory handle

  if (newByteCount <= 0)
  { GMFree(ptr);
    return 0;
  } // if
  if (!ptr)
    return GMAlloc(newByteCount);

  gmHdl = *((HANDLE *) ptr - 1);
  GlobalUnlock(gmHdl);
  while ((gm2Hdl = GlobalReAlloc(gmHdl, sizeof(HANDLE) + newByteCount,
				 GMEM_MOVEABLE)) == 0)
    MemError();
  ptr = GlobalLock(gm2Hdl);
  *(HANDLE *) ptr = gm2Hdl;
  return((HANDLE *) ptr + 1);
} // GMRealloc

void GMFree			/* ---- Global Memory Free -------------- */
( void		*ptr			// pointer
)
/* Discards the allocated memory block. */
{ HANDLE	gmHdl;			// global memory handle

  if (ptr)
  { gmHdl = *((HANDLE *) ptr - 1);
    GlobalUnlock(gmHdl);
    GlobalFree(gmHdl);
  } // if
} // GMFree

long GMSize			/* ---- Global Memory Size -------------- */
( void		*ptr			// pointer
)
/* Returns the size of the memory block. */
{
  return ptr? GlobalSize(*((HANDLE *) ptr - 1)) : 0;
} // GMSize
