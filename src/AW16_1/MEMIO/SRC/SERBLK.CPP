#include <mem.h>
#include <dos.h>
#include "almsys.h"

#include "serblk.h"
#include "system.h"
#include "chunklib.h"
#include "rectypes.h"

#define SEGMENT_SIZE    65535L

#define MAX_IO_SIZE     0X8000


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSBLOCK (constructor)
//
// CLASS.......: SERIUSBLOCK
//
// DESCRIPTION.: This is the default constructor.  No data space is allocated
//               by this constructor.
//
/////////////////////////////////////////////////////////////////////////////
SERIUSBLOCK::SERIUSBLOCK(
	void):
    BASEBLOCK(pTheSystem->GetFileObject()->GetSeriusBlocks(), (long)0)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        SERIUSBLOCK (constructor)
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                                                              block.
//
// PARAMETERS..:  size - the number of bytes of storage to allocate with the
//                  new block.
//
//
/////////////////////////////////////////////////////////////////////////////
SERIUSBLOCK::SERIUSBLOCK(
	long int        size):
    BASEBLOCK(pTheSystem->GetFileObject()->GetSeriusBlocks(), size)
	{

	}

