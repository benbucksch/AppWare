#include "almsys.h"

#include "memblk.h"
#include "system.h"
#include "chunklib.h"
#include "rectypes.h"

#define SEGMENT_SIZE    65535L

#define MAX_IO_SIZE     0X8000

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MEMBLOCK (constructor)
//
// CLASS.......: MEMBLOCK
//
// DESCRIPTION.: This is the default constructor.  No data space is allocated
//               by this constructor.
//
/////////////////////////////////////////////////////////////////////////////
MEMBLOCK::MEMBLOCK(
	void):
    BASEBLOCK(pTheSystem->GetFileObject()->GetMemBlocks(), (long)0)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        MEMBLOCK (constructor)
//
// CLASS.......:        MEMBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                                                              block.
//
// PARAMETERS..:  size - the number of bytes of storage to allocate with the
//                  new block.
//
//
/////////////////////////////////////////////////////////////////////////////
MEMBLOCK::MEMBLOCK(
	long int        size):
    BASEBLOCK(pTheSystem->GetFileObject()->GetMemBlocks(), size)
	{

	}



