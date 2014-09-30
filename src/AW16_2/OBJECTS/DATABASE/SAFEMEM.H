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
 | FILE		: SAFEMEM.H                                               |
 | DESCRIPTION	: Memory Functions with Warning Messages Include File.    |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#ifndef SAFEMEM_H
#define SAFEMEM_H

#define STRICT
#include <memory.h>

#define ASTRICT
#include "a_alm.h"


int hmemcmp(const void _huge *s1, const void _huge *s2, long n);
/* Compares the first n bytes of the block s1 and s2 as unsigned chars. */

#if (WINVER < 0x030a)

void hmemcpy(void _huge *dest, const void _huge *src, long n);
/* Copies a block of n bytes from src to dest. */

#endif /* (WINVER < 0x030a) */

void hmemmove(void _huge *dest, const void _huge *src, long n);
/* Copies a block of n bytes from src to dest.  Even when the source and
   destination blocks overlap, bytes in the overlapping locations are
   copied correctly.
*/

void *hmemptr(void _huge *s, long idx, unsigned width);
/* Returns the pointer to the item. */

void hmemset(void _huge *s, int c, long n);
/* Sets the first n bytes of the array s to the character c. */


void hstrcat(char _huge *dest, const char _huge *src);
/* Appends a copy of src to the end of dest. */

int hstrcmp(const char _huge *s1, const char _huge *s2);
/* Returns a value that is < 0 if s1 < s2, 0 if s1 = s2, > 0 if s1 > s2. */ 
        							
void hstrcpy(char _huge *dest, char _huge *src);
/* Copies string src to dest. */

long hstrlen(const char _huge *s);
/* Returns the length of string s. */


void MemError(void);
/* Memory Error. */


void MemSetObjectDataCount(OBJECTID obj, long dataCount);
/* Sets the number of the datablocks for the specified object. */

void MemResizeObjectData(OBJECTID obj, long dataIdx, long dataSize);
/* Resizes the specified datablock. */


MEMBLOCKID MemAllocate(long bytes = 0);
/* Returns the MEMBLOCKID of the allocated memory block of size BYTES. */

void MemResize(MEMBLOCKID memBlock, long bytes);
/* Resizes the specified memory block. */


/* Global memory */

HANDLE GAlloc(long byteCount);
/* Returns the handle to the newly allocated global memory block. */

HANDLE GRealloc(HANDLE gmHdl, long newByteCount);
/* Returns the handle to the reallocated global memory block. */

void GFree(HANDLE gmHdl);
/* Discards the allocated global memory block. */


/* Simple memory

   WARNING!:    Don't mix the use of the following functions with MBAlloc,
		MBRealloc, MBFree, MBSize, GMAlloc, GMRealloc, GMFree, or
		GMSize.
*/

void *MemAlloc(size_t byteCount);
/* Returns the pointer to the newly allocated memory block. */

void *MemRealloc(void *ptr, size_t newByteCount);
/* Returns the pointer to the reallocated memory block. */

void MemFree(void *ptr);
/* Discards the allocated memory block. */


/* MEMBLOCKID memory 

   WARNING!:	Don't mix the use of the following functions with MemAlloc,
		MemRealloc, MemFree, GMAlloc, GMRealloc, GMFree, or GMSize.
*/

void *MBAlloc(long byteCount);
/* Returns the pointer to the newly allocated memory block. */

void *MBRealloc(void *ptr, long newByteCount);
/* Returns the pointer to the reallocated memory block. */

void MBFree(void *ptr);
/* Discards the allocated memory block. */

long MBSize(void *ptr);
/* Returns the size of the memory block. */


/* Global memory

   WARNING!:	Don't mix the use of the following functions with MemAlloc,
		MemRealloc, MemFree, MBAlloc, MBRealloc, MBFree, or MBSize.
*/

void *GMAlloc(long byteCount);
/* Returns the pointer to the newly allocated memory block. */

void *GMRealloc(void *ptr, long newByteCount);
/* Returns the pointer to the reallocated memory block. */

void GMFree(void *ptr);
/* Discards the allocated memory block. */

long GMSize(void *ptr);
/* Returns the size of the memory block. */

#endif /* SAFEMEM_H */
