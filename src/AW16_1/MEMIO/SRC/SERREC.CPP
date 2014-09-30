#include "almsys.h"

#include "serblk.h"
#include "baserecb.h"
#include "uidtable.h"
#include "serrec.h"
#include "baserecb.h"
#include "rtreclib.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	SERIUSRECORD
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
void far * SERIUSRECORD::operator new(
	size_t  /*size*/)
	{

	return NULL;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator new
//
// CLASS.......:SERIUSRECORD
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
void far * SERIUSRECORD::operator new(
	size_t	    size,
	RUNTIMEUID	blockUID)
	{

	return ((LPBASERECORDBLOCK)GetObject(blockUID))->AddRecord(size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator new
//
// CLASS.......:SERIUSRECORD
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
void far * SERIUSRECORD::operator new(
	size_t			/*size*/,
	void far*		objPtr)
	{

	return (objPtr);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator delete
//
// CLASS.......:SERIUSRECORD
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
void SERIUSRECORD::operator delete(
	void far *	objPtr,
	size_t			size)
	{

	((LPBASERECORDBLOCK)GetObject(((RECORDPREFIX far*)objPtr - 1)->recBlockUID))->
	 DeleteRecord(objPtr, size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~SERIUSRECORD (destructor)
//
// CLASS.......: SERIUSRECORD
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

SERIUSRECORD::~SERIUSRECORD(void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSRECORD (constructor)
//
// CLASS.......: SERIUSRECORD
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

SERIUSRECORD::SERIUSRECORD(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSRECORD (constructor)
//
// CLASS.......: PostReadInit
//
// DESCRIPTION.: default constructor required for derived classes.
//
// PARAMETERS..:
//
// RETURN VALUE: 0 corresponds to a value defined in project.h which signals
//								that no error occurred.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
short SERIUSRECORD::PostReadInit(
	void)
	{
	return 0;
	}