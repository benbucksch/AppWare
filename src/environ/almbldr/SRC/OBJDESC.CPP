#include "almsys.h"
#include "objdesc.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AOBJDESC
//
// CLASS.......:  AOBJDESC
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
AOBJDESC::AOBJDESC(
	COMPUID	id,
	TSTRING 	name):
		ACOMPDESC(id, name),
		routines(MAX_ROUTINES),
		responses()
	{
  objectFlags = 0;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetFunction
//
// CLASS.......:  AOBJDESC
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
LPCSTR	AOBJDESC::GetFunction(
	short	index)
	{

	return routines.functions[index];

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetFunction
//
// CLASS.......:  AOBJDESC
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
LPCSTR	AOBJDESC::pGetFunction(
	short	index)
	{

	return ((LPDBSTRING)routines.functions.pGetElement(index))->pString;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetDLL
//
// CLASS.......:  AOBJDESC
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
LPCSTR AOBJDESC::GetDLL(
	short index)
	{

	return routines.dlls[index];

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetDLL
//
// CLASS.......:  AOBJDESC
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
LPCSTR AOBJDESC::pGetDLL(
	short index)
	{

	return ((LPDBSTRING)routines.dlls.pGetElement(index))->pString;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetFunction
//
// CLASS.......:  AOBJDESC
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
BOOL AOBJDESC::SetFunction(
	short index,
	LPSTR newFunction)
	{
	BOOL result = TRUE;

	if (index < MAX_ROUTINES)
		routines.functions[index] = newFunction;
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetDLL
//
// CLASS.......:  AOBJDESC
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
BOOL AOBJDESC::SetDLL(
	short	index,
	LPSTR newDLL)
	{
	BOOL result = TRUE;

	if (index < MAX_ROUTINES)
		routines.dlls[index] = newDLL;
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  AOBJDESC
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
void AOBJDESC::ConvertToPointers(
	void)
	{

	helpFile.ConvertToPointers();
	routines.ConvertToPointers();
	responses.ConvertToPointers();
	ACOMPDESC::ConvertToPointers();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetResponse
//
// CLASS.......:  AOBJDESC
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
LPRESPONSE AOBJDESC::pGetResponse(
	COMPUID		eventID)
	{

	return responses.pGetResponse(eventID);

	}
