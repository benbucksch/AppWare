#include <mem.h>
#include <dos.h>
#include "almsys.h"
#include "intalm.h"
#include "system.h"
#include "exrecblk.h"
#include "rectypes.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	EXERECORDBLOCK (constructor)
//
// CLASS.......:	EXERECORDBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//								block.
//
// PARAMETERS..:	recordSize - the size in bytes of the records to be stored
//									in the serius block.  If recordSize is zero, count is
//									the number of bytes to be stored in the block, otherwise
//									count is the number of records to be stored in the block.
//
//								type - the type constant for the record type.
//
// 							  count - the number of records/bytes of storage to allocate
//									with the new block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
EXERECORDBLOCK::EXERECORDBLOCK(
	long int  	recordSize,
	long int		type):
    BASERECORDBLOCK(pTheSystem->GetFileObject()->GetExeRecordBlocks(),
                    recordSize, type)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	EXERECORDBLOCK (constructor)
//
// CLASS.......:	EXERECORDBLOCK
//
// DESCRIPTION.:  This is the constructor used to preallocate space for a
//                specified number of records.
//
// PARAMETERS..:	recordSize - the size in bytes of the records to be stored
//									in the serius block.  If recordSize is zero, count is
//									the number of bytes to be stored in the block, otherwise
//									count is the number of records to be stored in the block.
//
//								type - the type constant for the record type.
//
// 							  count - the number of records to allocate storage for.
//
/////////////////////////////////////////////////////////////////////////////
EXERECORDBLOCK::EXERECORDBLOCK(
	long int  	      recordSize,
	long int		      type,
  long int          recordCount):
    BASERECORDBLOCK(pTheSystem->GetFileObject()->GetExeRecordBlocks(),
                    recordSize, type, recordCount)
	{

  }

