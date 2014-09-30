#if(!defined __memlib_h)
#define __memlib_h

void * Allocate(
	unsigned size);

BOOL Deallocate(
	void * recordPtr);

BOOL InitializeBlocks();

void CleanUpBlocks();

void * GetGlobalBlockArrayAddr();

void SetGlobalBlockArrayAddr(
	void * theAddr);

unsigned GetGlobalBlockArraySize();

void SetGlobalBlockArraySize(
  unsigned theSize);

#endif
