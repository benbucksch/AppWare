#ifndef __CHUNKLIB_H__
#define __CHUNKLIB_H__

void CheckBlock(
  long       chunkID,
  void far*  theData,
  long       byteCount);

MEMCHUNK far* AllocateChunk(
  long       minNumBytes);

void far* AllocateBlock(
	long    blockSize,
  long    &chunkID);

void ReleaseBlock(
	long int    chunkID,
	long int    blockLength,
	void far*   theData);

void far * ResizeChunk(
  long  chunkID,
  long  newCount);

BOOL ReadChunk(
	int            hFile,
  MEMCHUNK far*  pChunk,
  long           byteCount);

#endif
