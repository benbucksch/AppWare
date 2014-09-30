#include "almsys.h"

#include "uidstack.h"
#include "memblk.h"
#include "rtreclib.h"

UIDSTACK::UIDSTACK(
    short   initialSize,
    short   growthSize)
    {
    LPMEMBLOCK   pBlock;

    pBlock = new MEMBLOCK(initialSize*sizeof(SERUID));

    this->blockID  = pBlock->GetUID();
    this->dataPointer = (LPSERUID)pBlock->GetData();

    this->initialSize = initialSize;
    this->growthSize  = growthSize;

    this->topIndex = -1;
    this->maxIndex = initialSize - 1;

    this->dataPointer = (LPSERUID)pBlock->GetData();

    }


void UIDSTACK::Push(
  SERUID  item)
  {
  this->topIndex++;

  if (this->topIndex > this->maxIndex)
    {
    LPMEMBLOCK pBlock = (LPMEMBLOCK)GetObject(blockID);
    pBlock->ResizeBlock(growthSize*sizeof(SERUID));
    this->dataPointer = (LPSERUID)pBlock->GetData();
    }

  this->dataPointer[this->topIndex] = item;

  }


SERUID UIDSTACK::Pop(
  void)
  {

  if (this->topIndex < -1)
    return 0;

  return this->dataPointer[--this->topIndex];

  }
