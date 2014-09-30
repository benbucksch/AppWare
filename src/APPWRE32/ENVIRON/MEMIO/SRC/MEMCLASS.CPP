#include "almsys.h"

#include "memlib.h"
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
// RETURN VALUE:  void *:	pointer to memory which was allocated.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void * MEMORY::operator new(size_t size)
	{
		return Allocate((unsigned)size);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator delete
//
// CLASS.......:  MEMORY
//
// DESCRIPTION.:  overwritten delete operator which calls memlib routine
//									Deallocate.
//
// PARAMETERS..:  void * p:  pointer to memory to free.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void MEMORY:: operator  delete(void * p)
	{
			Deallocate((LPSTR)p);
	}


void * MEMORY::operator new(
	size_t size,
	HANDLE fileHandle)
	{
	void * tempPtr;
	DWORD  sizeRead;

	tempPtr = Allocate((unsigned)size);
	SetFilePointer(fileHandle, 0L, NULL, FILE_BEGIN); // SKJ --SetFilePointer(fileHandle, 0L, 0);
	ReadFile(fileHandle, (LPSTR)tempPtr, size, &sizeRead, NULL); // ReadFile(fileHandle, (LPSTR)tempPtr, size);

	return tempPtr;
	}
