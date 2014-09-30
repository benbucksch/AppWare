#if(!defined __memlib_h)
#define __memlib_h

void far* Allocate(
	unsigned size);

BOOL Deallocate(
	void far* recordPtr);

void InitializeBlocks();

void CleanUpBlocks();

void far * GetGlobalBlockArrayAddr();

void SetGlobalBlockArrayAddr(
  void far * theAddr);

unsigned GetGlobalBlockArraySize();

void SetGlobalBlockArraySize(
  unsigned theSize);

#endif
