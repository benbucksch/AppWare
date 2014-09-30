#ifndef __CHUNKLIB_H__
#define __CHUNKLIB_H__

void CheckBlock(
  long       chunkID,
  void *  theData,
  long       byteCount);

MEMCHUNK * AllocateChunk(
  long       minNumBytes);

void * AllocateBlock(
	long    blockSize,
  long    &chunkID);

void ReleaseBlock(
	long int    chunkID,
	long int    blockLength,
	void *   theData);

void  * ResizeChunk(
  long  chunkID,
  long  newCount);

BOOL ReadChunk(
	HANDLE            hFile,
  MEMCHUNK *  pChunk,
  long           byteCount);

#endif
