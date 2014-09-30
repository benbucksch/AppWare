#include "almsys.h"

#include "uidarray.h"
#include "compuid.h"
#include "sereclib.h"
#include "serblk.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
UIDARRAY::UIDARRAY(void):SERIUSARRAY()
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
UIDARRAY::UIDARRAY(
	unsigned short	count):SERIUSARRAY(sizeof(COMPUID), count)
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the copy constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
UIDARRAY::UIDARRAY(
	const UIDARRAY & srcArray):
		SERIUSARRAY(sizeof(COMPUID), srcArray.GetCount())
	{

	if (srcArray.GetCount() > 0)
		CopyMemory(this->GetElement(0), srcArray.GetElement(0),
		 this->GetCount()*this->GetSize());

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
UIDARRAY::UIDARRAY(
	unsigned short 	count,
	LPSERIUSBLOCK		arrayBlock):SERIUSARRAY(sizeof(COMPUID), count, arrayBlock)
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
COMPUID & UIDARRAY::operator[](
	unsigned short	index)
	{

	return *((COMPUID *)((LPSERIUSBLOCK)GetObject(blockID))->
	 GetData()+index);

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetElement
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL UIDARRAY::SetElement(
	unsigned short	index,
	COMPUID					uid,
	LPUIDTABLE			uidTable)
	{

	*((COMPUID *)((LPSERIUSBLOCK)uidTable->GetPointer(blockID))->
	 GetData()+index) = uid;

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ConvertToPointers
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void UIDARRAY::ConvertToPointers(
	void)
	{

	SERIUSARRAY::ConvertToPointers();

	}
