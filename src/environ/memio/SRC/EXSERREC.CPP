#include "almsys.h"

#include "exrecblk.h"
#include "uidtable.h"
#include "exserrec.h"
#include "sereclib.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	EXESERIUSRECORD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void  * EXESERIUSRECORD::operator new(
	size_t				/*size*/)
	{

	return NULL;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator new
//
// CLASS.......:EXESERIUSRECORD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void  * EXESERIUSRECORD::operator new(
	size_t	    size,
	RUNTIMEUID	blockUID)
	{

	return ((LPEXERECORDBLOCK)GetObject(blockUID))->AddRecord(size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator new
//
// CLASS.......:EXESERIUSRECORD
//
// DESCRIPTION.:	This version of the new operator is used to initialize
//								the virtual table pointers of a record read from disk.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void  * EXESERIUSRECORD::operator new(
	size_t			/*size*/,
	void *		objPtr)
	{

	return (objPtr);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator delete
//
// CLASS.......:EXESERIUSRECORD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void EXESERIUSRECORD::operator delete(
	void  *	objPtr,
	size_t			size)
	{
	((LPEXERECORDBLOCK)GetObject(((RECORDPREFIX *)objPtr - 1)->recBlockUID))
  ->DeleteRecord(objPtr, size);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~EXESERIUSRECORD (destructor)
//
// CLASS.......: EXESERIUSRECORD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

EXESERIUSRECORD::~EXESERIUSRECORD(void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EXESERIUSRECORD (constructor)
//
// CLASS.......: EXESERIUSRECORD
//
// DESCRIPTION.: default constructor required for derived classes.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

EXESERIUSRECORD::EXESERIUSRECORD(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EXESERIUSRECORD (constructor)
//
// CLASS.......: PostReadInit
//
// DESCRIPTION.: default constructor required for derived classes.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void EXESERIUSRECORD::PostReadInit(
	void)
	{

	}
