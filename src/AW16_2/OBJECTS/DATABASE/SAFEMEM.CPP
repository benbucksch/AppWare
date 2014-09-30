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
 | FILE		: SAFEMEM.CPP                                             |
 | DESCRIPTION	: Memory Functions with Warning Messages.                 |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#define STRICT
#include <alloc.h>

#include "safemem.h"


int hmemcmp			/* ---- Huge Memory Compare ------------- */
( const void	_huge *s1,		// source 1
  const void	_huge *s2,		// source 2
  long		n			// number of bytes
)
/* Compares the first n bytes of the block s1 and s2 as unsigned chars. */
{ BYTE		huge *s1Ptr = (BYTE huge *) s1;
  BYTE		huge *s2Ptr = (BYTE huge *) s2;
  WORD		s1Offset;		// source 1 offset
  WORD		s2Offset;		// source 2 offset
  DWORD		count;			// number of bytes to compare
  int		result;			// result of comparison

  while (n > 0)
  { s1Offset = LOWORD((DWORD) s1Ptr);
    s2Offset = LOWORD((DWORD) s2Ptr);
    count = 0x00010000L - (s1Offset > s2Offset? s1Offset : s2Offset);
    if (count > n)
      count = n;
    if (count > 0x0000FFFF)
      count = 0x0000FFFF;

    result = _fmemcmp(s1Ptr, s2Ptr, (unsigned) count);
    if (result)
      return result;			// s1 is not the same as s2

    s1Ptr += count;
    s2Ptr += count;
    n -= count;
  } // while
  return 0;				// s1 is the same as s2
} // hmemcmp

#if (WINVER < 0x030a)

void hmemcpy			/* ---- Huge Memory Copy ---------------- */
( void		_huge *dest,		// destination
  const void	_huge *src,		// source
  long		n			// number of bytes
)
/* Copies a block of n bytes from src to dest. */
{ BYTE		huge *srcPtr = (BYTE huge *) src;
  BYTE		huge *destPtr = (BYTE huge *) dest;
  WORD		srcOffset;		// source offset
  WORD		destOffset;		// destination offset
  DWORD		count;			// number of bytes to copy

  while (n > 0)
  { srcOffset = LOWORD((DWORD) srcPtr);
    destOffset = LOWORD((DWORD) destPtr);
    count = 0x00010000L - (srcOffset > destOffset? srcOffset : destOffset);
    if (count > n)
      count = n;
    if (count > 0x0000FFFF)
      count = 0x0000FFFF;

    _fmemcpy(destPtr, srcPtr, (unsigned) count);

    srcPtr += count;
    destPtr += count;
    n -= count;
  } // while
} // hmemcpy

#endif /* (WINVER < 0x030a) */

void hmemmove			/* ---- Huge Memory Move ---------------- */
( void		_huge *dest,		// destination
  const void	_huge *src,		// source
  long		n			// number of bytes
)
/* Copies a block of n bytes from src to dest.  Even when the source and
   destination blocks overlap, bytes in the overlapping locations are
   copied correctly.
*/
{ BYTE		huge *srcPtr = (BYTE huge *) src;
  BYTE		huge *destPtr = (BYTE huge *) dest;
  WORD		srcOffset;		// source offset
  WORD		destOffset;		// destination offset
  DWORD		count;			// number of bytes to copy

  while (n > 0)
  { srcOffset = LOWORD((DWORD) srcPtr);
    destOffset = LOWORD((DWORD) destPtr);
    count = 0x00010000L - (srcOffset > destOffset? srcOffset : destOffset);
    if (count > n)
      count = n;
    if (count > 0x0000FFFF)
      count = 0x0000FFFF;

    memmove(destPtr, srcPtr, (unsigned) count);

    srcPtr += count;
    destPtr += count;
    n -= count;
  } // while
} // hmemmove

void *hmemptr			/* ---- Huge Memory Pointer ------------- */
( void		_huge *s,		// source pointer
  long		idx,			// index
  unsigned	width			// width
)
/* Returns a pointer to the item. */
{ BYTE		huge *sPtr = (BYTE huge *) s;
  DWORD		count;			// number of bytes to skip
  long		n = idx * width;	// number of bytes

  while (n > 0)
  { count = 0x00010000L - LOWORD((DWORD) sPtr);
    if (count > n)
      count = n;
    if (count > 0x0000FFFF)
      count = 0x0000FFFF;

    sPtr += count;
    n -= count;
  } // while
  return sPtr;
} // hmemptr

void hmemset			/* ---- Huge Memory Set ----------------- */
( void		_huge *s,		// source pointer
  int		c,			// character
  long		n			// number of bytes
)
/* Sets the first n bytes of the array s to the character c. */
{ BYTE		huge *sPtr = (BYTE huge *) s;
  DWORD		count;			// number of bytes to set

  while (n > 0)
  { count = 0x00010000L - LOWORD((DWORD) sPtr);
    if (count > n)
      count = n;
    if (count > 0x0000FFFF)
      count = 0x0000FFFF;

    _fmemset(sPtr, c, (unsigned) count);

    sPtr += count;
    n -= count;
  } // while
} // hmemset


void hstrcat			/* ---- huge string copy ---------------- */
( char		_huge *dest,		// destination string
  const char	_huge *src		// source string
)
/* Appends a copy of src to the end of dest. */
{ 
  while (*dest != '\0')
    dest++;
  while ((*dest++ = *src++) != '\0')
    /* Do Nothing */;
} // hstrcat

int hstrcmp			/* ---- huge string compare ------------- */
( const char	_huge *s1,		// string 1
  const char	_huge *s2		// string 2
)
/* Returns a value that is < 0 if s1 < s2, 0 if s1 = s2, > 0 if s1 > s2. */ 
{
  for (; *s1 == *s2 && *s1 != '\0'; s1++, s2++)
    /* Do Nothing */;
  return *s1 - *s2;
} // hstrcmp
        							
void hstrcpy			/* ---- huge string copy ---------------- */
( char		_huge *dest,		// destination string
  char		_huge *src		// source string
)
/* Copies string src to dest. */
{ 
  while ((*dest++ = *src++) != '\0')
    /* Do Nothing */;
} // hstrcpy

long hstrlen			/* ---- huge string length -------------- */
( const char	_huge *s		// string		
)
/* Returns the length of string s. */
{ long		i;			// counter

  for (i = 0; *s++ != '\0'; i++)
    /* Do Nothing */;
  return i;
} // hstrlen


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
( OBJECTID	obj,			// object
  long		dataCount		// number of data
)
/* Sets the number of the datablocks for the specified object. */
{
  while (!AObjSetDataCount(obj, dataCount))
    MemError();
} // MemSetObjectDataCount

void MemResizeObjectData	/* ---- Memory Resize Object Data ------- */
( OBJECTID	obj,			// object
  long		dataIdx,		// data index
  long		dataSize		// data size
)
/* Resizes the specified datablock. */
{
  while (!AObjResizeData(obj, dataIdx, dataSize))
    MemError();
} // MemResizeObjectData


MEMBLOCKID MemAllocate		/* ---- Memory Allocate ----------------- */
( long		bytes			// number of bytes
)
/* Returns the MEMBLOCKID of the allocated memory block of size BYTES. */
{ MEMBLOCKID	memBlock;		// memory block

  while ((memBlock = AMemAllocate(bytes)) == 0)
    MemError();
  return memBlock;
} // MemAllocate

void MemResize			/* ---- Memory Resize ------------------- */
( MEMBLOCKID	memBlock,		// memory block
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


/* MEMBLOCKID memory 

   WARNING!:	Don't mix the use of the following functions with MemAlloc,
		MemRealloc, MemFree, GMAlloc, GMRealloc, GMFree, or GMSize.
*/

void *MBAlloc			/* ---- Memory Block Allocate ----------- */
( long		byteCount		// number of bytes
)
/* Returns the pointer to the newly allocated memory block. */
{ MEMBLOCKID	memBlock;		// memory block
  MEMBLOCKID	*ptr;			// pointer

  if (byteCount <= 0)
    return 0;
  while ((memBlock = AMemAllocate(sizeof(MEMBLOCKID) + byteCount)) == 0)
    MemError();

  ptr = (MEMBLOCKID *) AMemLock(memBlock);
  *ptr = memBlock;
  return(ptr + 1);
} // MBAlloc

void *MBRealloc			/* ---- Memory Block Reallocate --------- */
( void		*ptr,			// pointer
  long		newByteCount		// number of new bytes
)
/* Returns the pointer to the reallocated memory block. */
{ MEMBLOCKID	memBlock;		// memory block

  if (newByteCount <= 0)
  { MBFree(ptr);
    return 0;
  } // if
  if (!ptr)
    return MBAlloc(newByteCount);

  memBlock = *((MEMBLOCKID *) ptr - 1);
  AMemUnlock(memBlock);
  while (!AMemSetSize(memBlock, sizeof(MEMBLOCKID) + newByteCount))
    MemError();
  return((MEMBLOCKID *) AMemLock(memBlock) + 1);
} // MBRealloc

void MBFree			/* ---- Memory Block Free --------------- */
( void		*ptr			// pointer
)
/* Discards the allocated memory block. */
{ MEMBLOCKID	memBlock;		// memory block

  if (ptr)
  { memBlock = *((MEMBLOCKID *) ptr - 1);
    AMemUnlock(memBlock);
    AMemFree(memBlock);
  } // if
} // MBFree

long MBGetSize			/* ---- Memory Block Get Size ----------- */
( void		*ptr			// pointer
)
/* Returns the size of the memory block. */
{
  return ptr? AMemGetSize(*((MEMBLOCKID *) ptr - 1)) : 0;
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
  while ((gmHdl = GlobalAlloc(GMEM_MOVEABLE, sizeof(HANDLE) +
					     byteCount)) == 0)
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
