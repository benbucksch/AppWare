#include "almsys.h"

#include "mem.h"
#include "array.h"
#include "serblk.h"
#include "rtreclib.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
/////////////////////////////////////////////////////////////////////////////
SERIUSARRAY::SERIUSARRAY(
	void)
	{

	blockID.id = -1;
	elementCount = 0;
	elementSize = 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
SERIUSARRAY::SERIUSARRAY(
	short						size,
	unsigned short  count)
	{

	if (count == 0)
		blockID = -1;
	else
		blockID = GetUID((LPSERIUSBLOCK)new SERIUSBLOCK(size*count));

	elementSize = size;
	elementCount = count;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
SERIUSARRAY::SERIUSARRAY(
	short						size,
	unsigned short  count,
	LPSERIUSBLOCK		pBlock)
	{

	pBlock->
	 ResizeBlock((count*size) - pBlock->GetByteCount());
	this->blockID = GetUID(pBlock);
	elementSize = size;
	elementCount = count;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
SERIUSARRAY::~SERIUSARRAY(
	void)
	{

	if (blockID.id != -1)
		delete (LPSERIUSBLOCK)GetObject(blockID);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Resize
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSARRAY::Resize(
	unsigned short newCount)
	{
	LPSERIUSBLOCK  theBlock;

	if (blockID.id != -1)
		{
		theBlock = 	(LPSERIUSBLOCK)GetObject(blockID);
		theBlock->ResizeBlock((newCount*elementSize) - theBlock->GetByteCount());
		}
	else
		blockID = GetUID(new SERIUSBLOCK(newCount*elementSize));

	elementCount = newCount;

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Insert
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void far* SERIUSARRAY::Insert(
	unsigned short	index)
	{
	void far* 		 	result;
	LPSERIUSBLOCK	 	pBlock;
	LPSTR						pData;
	int 						i;

	if (elementSize == 0)
		result = 0;
	else if (blockID == -1)
		{
		pBlock = new SERIUSBLOCK(elementSize);
		blockID = GetUID(pBlock);
		elementCount = 1;
		result = pBlock->GetData();
		}
	else
		{
		pBlock = (LPSERIUSBLOCK)GetObject(blockID);
		result = pBlock->ResizeBlock(elementSize);
		elementCount += 1;
		if (index < elementCount-1)
			{
			pData =  (LPSTR)pBlock->GetData();
			result = pData + (elementSize*index);
			for (i=elementCount-1; i>index; i--)
				_fmemcpy(&pData[i*elementSize],
				 &pData[(i-1)*elementSize], elementSize);
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Delete
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSARRAY::Delete(
	unsigned short	index)
	{
	BOOL result = TRUE;

	if ((index >= elementCount) || (blockID == -1))
		result = FALSE;
	else
		{
		((LPSERIUSBLOCK)GetObject(blockID))->
		 DeleteSubBlock(index*elementSize + 1, elementSize);
		elementCount--;
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Write
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSARRAY::Write(
	int   hFile,
	WORD  &writeCount)
	{

	if (blockID == -1)
		return FALSE;
	else
		return ((LPSERIUSBLOCK)GetObject(blockID))->Write(hFile, writeCount);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetElement
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void far*	SERIUSARRAY::GetElement(
	unsigned short index) const
	{

	if (index >= elementCount)
		return 0;
	else if (blockID.id != -1)
		return (LPSTR)((LPSERIUSBLOCK)GetObject(blockID))->GetData() +
		 index*elementSize;
	else
		return 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: pGetElement
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void far*	SERIUSARRAY::pGetElement(
	unsigned short index)
	{

	if (blockID.ptr != 0)
		return
		 (LPSTR)((LPSERIUSBLOCK)blockID.ptr)->GetData() + index*elementSize;
	else
		return 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ConvertToPointers
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SERIUSARRAY::ConvertToPointers(
	void)
	{

	if (blockID.id != -1)
		blockID.ptr = GetObject(blockID);
	else
		blockID.ptr = 0;

	}

