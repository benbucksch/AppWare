////////////////////////////////////////////////////////////////////////
// oipmem.cpp -- memXXX functions provided by OIP
//
// jon 16-Aug-92 documented
////////////////////////////////////////////////////////////////////////

	#include "almsys.h"

  #include "memblk.h"
  #include "sereclib.h"

extern "C" {

//----------------------------------------------------------------------
//   AMemAllocate
//----------------------------------------------------------------------
MEMBLOCKID ALMAPI	AMemAllocate(
  LONG        numBytes)
  {
	pTheSystem->GetCurrentContext();
	return GetUID(new MEMBLOCK(numBytes)).id;
	}


//----------------------------------------------------------------------
//   AMemFree
//----------------------------------------------------------------------
BOOL ALMAPI	AMemFree(
	MEMBLOCKID  theBlock)
	{
	LPMEMBLOCK  pBlock;
	BOOL        result = TRUE;

	pTheSystem->GetCurrentContext();

	pBlock = (LPMEMBLOCK)GetObject((RUNTIMEUID)theBlock);
	if (pBlock)
		delete pBlock;
	else
		result = FALSE;

	return result;
	}


//----------------------------------------------------------------------
//   AMemLock
//----------------------------------------------------------------------
LPVOID ALMAPI	AMemLock(
	MEMBLOCKID  theBlock)
	{
	LPMEMBLOCK  pBlock;
	LPVOID      pData;

	pBlock = (LPMEMBLOCK)GetObject((RUNTIMEUID)theBlock);
	if (pBlock)
		pData = pBlock->GetData();
	else
		pData = NULL;

	return pData;
	}


//----------------------------------------------------------------------
//   AMemUnlock
//----------------------------------------------------------------------
BOOL ALMAPI	 AMemUnlock(
	MEMBLOCKID ) // theBlock)
	{

	pTheSystem->GetCurrentContext();

	return TRUE;

	}

//----------------------------------------------------------------------
//   AMemGetSize
//----------------------------------------------------------------------
LONG ALMAPI	AMemGetSize(
	MEMBLOCKID  theBlock)
	{
	LPMEMBLOCK  pBlock;
	LONG        size;

	pTheSystem->GetCurrentContext();

	pBlock = (LPMEMBLOCK)GetObject((RUNTIMEUID)theBlock);
	if (pBlock)
		size = pBlock->GetByteCount();
	else
		size = -1;

	return size;
	}


//----------------------------------------------------------------------
//   AMemSetSize
//----------------------------------------------------------------------
BOOL ALMAPI	AMemSetSize(
	MEMBLOCKID  theBlock,
	LONG        numBytes)
	{
	LPMEMBLOCK  pBlock;
	BOOL        result = FALSE;

	pTheSystem->GetCurrentContext();

	pBlock = (LPMEMBLOCK)GetObject((RUNTIMEUID)theBlock);
	if (pBlock)
		{
		pBlock->ResizeBlock(numBytes - pBlock->GetByteCount());
		result = pBlock->ValidBlock();
		}
	return result;
	}

} // extern "C"
