#include "almsys.h"

#include "exserblk.h"
#include "system.h"
#include "chunklib.h"
#include "rectypes.h"

#define SEGMENT_SIZE    65535L

#define MAX_IO_SIZE     0X8000

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EXESERIUSBLOCK (constructor)
//
// CLASS.......: EXESERIUSBLOCK
//
// DESCRIPTION.: This is the default constructor.  No data space is allocated
//               by this constructor.
//
/////////////////////////////////////////////////////////////////////////////
EXESERIUSBLOCK::EXESERIUSBLOCK(
	void):
    BASEBLOCK(pTheSystem->GetFileObject()->GetExeSeriusBlocks(), (long)0)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  EXESERIUSBLOCK (constructor)
//
// CLASS.......:  EXESERIUSBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                                                              block.
//
// PARAMETERS..:  size - the number of bytes of storage to allocate with the
//                  new block.
//
//
/////////////////////////////////////////////////////////////////////////////
EXESERIUSBLOCK::EXESERIUSBLOCK(
	long int        size):
    BASEBLOCK(pTheSystem->GetFileObject()->GetExeSeriusBlocks(), size)
	{

	}



