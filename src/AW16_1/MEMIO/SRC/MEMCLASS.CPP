#include <stddef.h>
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
// RETURN VALUE:  void far*:	pointer to memory which was allocated.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far* MEMORY::operator new(size_t size)
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
			Deallocate((LPSTR)p);
	}


void far* MEMORY::operator new(
	size_t size,
	int fileHandle)
	{
	void far* tempPtr;

	tempPtr = Allocate((unsigned)size);
	_llseek(fileHandle, 0L, 0);
	_lread(fileHandle, (LPSTR)tempPtr, size);

	return tempPtr;
	}
