////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          memobj.cpp
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_number.h>
#include "memobj.h"

////////////////

#define	DEF_ALLOC_COUNT		100					// 100 blocks default
#define	DEF_FREE_COUNT		1						// free one block at a time
#define DEF_DATA_ALLOC		1						// allocate one block at a time
#define DEF_ALLOC_SIZE		-10					// each block has 10 bytes more than previous
#define DEF_ALLOC_FILL		-1					// fill each block with index
#define ALLOC_FOREVER			-1					// allocate until failure
#define FILL_WITHINDEX		-1					// fill with index of block

template <class T>
inline T min(T a, T b)		{return (a < b) ? a : b;}

////////////////////////////////////////////////////////////////
//
//
//
//
//
////////////////////////////////////////////////////////////////

void	FreeAllMemory(
	AOBJECTID				oiMem,
	MemTestStatus*	pStatus
	)
	{
	I_ASSERT(pStatus->nDataCount > 0);

	AObjSetDataCount(oiMem, 1);

	I_ASSERT(AObjGetDataCount(oiMem) == 1);

	pStatus->nDataCount = 0;
	}

////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////

long	GetAllocSize(
	MemTestStatus*		pStatus,
	long							nCurrentIndex
	)
	{
	long							nAlloc;

	if (pStatus->nAllocSize < 0)
		nAlloc = (- pStatus->nAllocSize) * (nCurrentIndex + 1);
	else
		nAlloc = pStatus->nAllocSize;

	return nAlloc;
	}

////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////

long	GetFillValue(
	MemTestStatus*		pStatus,
	long							nCurrentIndex
	)
	{
	long							nFill;

	if (pStatus->fFillIndex)
		nFill = nCurrentIndex;
	else
		nFill = pStatus->nFillValue;

	return nFill;
	}

////////////////////////////////////////////////////////////////
//
// MemTestAllocate:  Allocates datablocks.  The parameters are:
//
//			- Count (number):  # of data blocks to allocate; -1 => until failure
//			- DataAlloc(number): # of datablocks to alloc at one time
//			- Size (number):  size of each datablock; < 0 => grow by that amount each time
//			- Fill (number):  value to put in each block; -1 => fill with index
//
////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK	 MemTestAllocate(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
	{
	AOBJECTID					oiMem;
	AOBJECTID					oiCount;
	AOBJECTID					oiDataAlloc;
	AOBJECTID					oiSize;
	AOBJECTID					oiFill;
	LONG							nCount;
	LONG							nDataAlloc;
	LONG							nSize;
	LONG							nFill;
	MemTestStatus*		pStatus;
	LONG							nCurrentIndex;
	LONG							nAlloc;
	BOOL							fSuccess;
	void*							pData;

	(void) theEvent;

	oiMem = AFuncGetTypedParameter(1, OTYPE_MEMTEST);

	I_ASSERT(AObjGetDataCount(oiMem) == 1);
	I_ASSERT(AObjGetDataSize(oiMem, 0) == sizeof(MemTestStatus));

	pStatus = (MemTestStatus*) AObjLockData(oiMem, 0);
	I_ASSERT(pStatus != NULL);

	if (pStatus->nDataCount > 0)
		FreeAllMemory(oiMem, pStatus);

	oiCount     = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	oiDataAlloc = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	oiSize      = AFuncGetTypedParameter(4, OTYPE_NUMBER);
	oiFill      = AFuncGetTypedParameter(5, OTYPE_NUMBER);

	if (oiCount != NULL)
		nCount = ONmbrGetInteger(oiCount);
	else
		nCount = DEF_ALLOC_COUNT;

	if (oiDataAlloc != NULL)
		nDataAlloc = ONmbrGetInteger(oiDataAlloc);
	else
		nDataAlloc = DEF_DATA_ALLOC;

	if (oiSize != NULL)
		nSize = ONmbrGetInteger(oiSize);
	else
		nSize = DEF_ALLOC_SIZE;

	if (oiFill != NULL)
		nFill = ONmbrGetInteger(oiFill);
	else
		nFill = DEF_ALLOC_FILL;

	if ((nCount > 0 || nCount == ALLOC_FOREVER) &&
			(nSize != 0))
		{
		I_ASSERT(pStatus->nDataCount == 0);
		pStatus->nTargetCount   = nCount;
		pStatus->nDataAllocSize = nDataAlloc;
		pStatus->fFillForever   = (nCount == ALLOC_FOREVER);
		pStatus->nAllocSize     = nSize;
		pStatus->fFillIndex     = (nFill == FILL_WITHINDEX);
		pStatus->nFillValue     = nFill;

		nCurrentIndex = 0;

		for (;;)
			{
			// Stop if we aren't filling until failure, and we 
			if ((!pStatus->fFillForever) &&
					(nCurrentIndex >= pStatus->nTargetCount))
				break;

			// Figure out size of current data block to allocate
			nAlloc = GetAllocSize(pStatus, nCurrentIndex);

			// bump up the number of data blocks if too few
			if (nCurrentIndex + 1 >= AObjGetDataCount(oiMem))
				{
				pStatus->nDataCount = min(pStatus->nDataCount + pStatus->nDataAllocSize,
																			pStatus->nTargetCount);
				fSuccess = AObjSetDataCount(oiMem, pStatus->nDataCount + 1);
				I_ASSERT((fSuccess == FALSE) ||
									(AObjGetDataCount(oiMem) == pStatus->nDataCount + 1));
				}
			else
				fSuccess = TRUE;

			if (!fSuccess)
				break;

			// Actually allocate the data for the data block
			fSuccess = AObjResizeData(oiMem, nCurrentIndex + 1, nAlloc);
			I_ASSERT((fSuccess == FALSE) ||
								(AObjGetDataSize(oiMem, nCurrentIndex + 1) == nAlloc));

			if (!fSuccess)
				break;

			// Now fill it.
			pData = AObjLockData(oiMem, nCurrentIndex + 1);
			I_ASSERT(pData != NULL);

			nFill = GetFillValue(pStatus, nCurrentIndex);
		
			for (;;)
				{
				if (nAlloc < sizeof(short))
					break;

				*((short*) pData) = (short) nFill;
				pData = (void*) ((char*) pData + sizeof(short));
				nAlloc -= sizeof(short);
				}

			AObjUnlockData(oiMem, nCurrentIndex + 1);

			nCurrentIndex++;
			}
		}

	AObjUnlockData(oiMem, 0);

  AFuncReportResult(theSystem, 1, "");
	}

////////////////////////////////////////////////////////////////
//
// MemTestVerify:  Verifies the integrity of the memtest object.
//
////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK	 MemTestVerify(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
	{
	AOBJECTID					oiMem;
	MemTestStatus*		pStatus;
	LONG							nCurrentIndex;
	LONG							nFill;
	LONG							nAlloc;
	void*							pData;

	(void) theEvent;

	oiMem = AFuncGetTypedParameter(1, OTYPE_MEMTEST);

	I_ASSERT(AObjGetDataCount(oiMem) >= 1);
	I_ASSERT(AObjGetDataSize(oiMem, 0) == sizeof(MemTestStatus));

	pStatus = (MemTestStatus*) AObjLockData(oiMem, 0);
	I_ASSERT(pStatus != NULL);

	I_ASSERT(AObjGetDataCount(oiMem) == pStatus->nDataCount + 1);
	
	nCurrentIndex = 0;

	for (;;)
		{
		if (nCurrentIndex >= pStatus->nDataCount)
			break;

		// Figure out size of current data block to allocate
		nAlloc = GetAllocSize(pStatus, nCurrentIndex);

		I_ASSERT(AObjGetDataSize(oiMem, nCurrentIndex + 1) == nAlloc);

		pData = AObjLockData(oiMem, nCurrentIndex + 1);

		I_ASSERT(pData != NULL);

		nFill = GetFillValue(pStatus, nCurrentIndex);

		for (;;)
			{
			if (nAlloc < sizeof(short))
				break;

			I_ASSERT(*((short*) pData) == (short) nFill);
			pData = (void*) ((char*) pData + sizeof(short));
			nAlloc -= sizeof(short);
			}

		AObjUnlockData(oiMem, nCurrentIndex + 1);

		nCurrentIndex++;
		}

	AObjUnlockData(oiMem, 0);

  AFuncReportResult(theSystem, 1, "");
	}

////////////////////////////////////////////////////////////////
//
// MemTestFree:  frees blocks allocated by MemTestAllocate
//
//			- Count (number):  # of data blocks to free; -1 => all data blocks
//
////////////////////////////////////////////////////////////////

extern "C"
void ALMCALLBACK	 MemTestFree(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
	{
	AOBJECTID					oiMem;
	AOBJECTID					oiCount;
	LONG							nCount;
	MemTestStatus*		pStatus;
	LONG							nDataCount;

	oiMem = AFuncGetTypedParameter(1, OTYPE_MEMTEST);

	MemTestVerify(theEvent, theSystem);

	oiCount = AFuncGetTypedParameter(2, OTYPE_NUMBER);

	if (oiCount != NULL)
		nCount = ONmbrGetInteger(oiCount);
	else
		nCount = DEF_FREE_COUNT;

	I_ASSERT(AObjGetDataSize(oiMem, 0) == sizeof(MemTestStatus));

	pStatus = (MemTestStatus*) AObjLockData(oiMem, 0);
	I_ASSERT(pStatus != NULL);

	for (;;)
		{
		if (pStatus->nDataCount <= 1)
			break;

		nDataCount = AObjGetDataCount(oiMem) - nCount;
		if (nDataCount < 1)
			nDataCount = 1;

		AObjSetDataCount(oiMem, nDataCount);
		I_ASSERT(AObjGetDataCount(oiMem) == nDataCount);
		pStatus->nDataCount = nDataCount;
		}

	AObjUnlockData(oiMem, 0);

  AFuncReportResult(theSystem, 1, "");
	}
