#define		STRICT
#define		ASTRICT

#include <stddef.h>
#include <windows.h>
#include "heap.h"
// #include "memlib.h"
#include "memclass.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:  MEMORY
//
// DESCRIPTION.:  overwritten new operator.  Allocates memory from
//									the memlib procedure Allocate.
//
// PARAMETERS..:  size_t size:  amount of memory to allocate.
//
// RETURN VALUE:  void far*:	pointer to memory which was allocated.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far* MEMORY::operator new(size_t size)
	{
	 	// removed ref. to allocate(..) in memlib.cpp
   	// SKJ Oct.14, 1993
		return MemAlloc((unsigned)size);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator delete
//
// CLASS.......:  MEMORY
//
// DESCRIPTION.:  overwritten delete operator which calls memlib routine
//									Deallocate.
//
// PARAMETERS..:  void far* p:  pointer to memory to free.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void MEMORY:: operator  delete(void far* p)
	{
			// REmoved ref. to 	Deallocate((LPSTR)p);
			MemFree((LPSTR)p);
	}



void far* MEMORY::operator new(
	size_t size,
	int fileHandle)
	{
	void far* tempPtr;
	tempPtr = (void far *)MemAlloc((unsigned)size); // Allocate
	_llseek(fileHandle, 0L, 0);
	_lread(fileHandle, (LPSTR)tempPtr, size);
	return tempPtr;
	}

