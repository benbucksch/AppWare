#include "windows.h"
#include "objdesc.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	OBJDESC
//
// CLASS.......:  OBJDESC
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
OBJDESC::OBJDESC(
	COMPUID	id,
	TSTRING 	name):
		COMPDESC(id, name),
		routines(MAX_ROUTINES),
		responses()
	{
  objectFlags = 0;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetIcons
//
// CLASS.......:  OBJDESC
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
LPSERIUSICON	OBJDESC::GetIcons(
	void)
	{

	return &icons;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetFunction
//
// CLASS.......:  OBJDESC
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
LPSTR	OBJDESC::GetFunction(
	short	index)
	{

	return routines.functions[index];

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetFunction
//
// CLASS.......:  OBJDESC
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
LPSTR	OBJDESC::pGetFunction(
	short	index)
	{

	return ((LPDBSTRING)routines.functions.pGetElement(index))->pString;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetDLL
//
// CLASS.......:  OBJDESC
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
LPSTR OBJDESC::GetDLL(
	short index)
	{

	return routines.dlls[index];

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetDLL
//
// CLASS.......:  OBJDESC
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
LPSTR OBJDESC::pGetDLL(
	short index)
	{

	return ((LPDBSTRING)routines.dlls.pGetElement(index))->pString;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetFunction
//
// CLASS.......:  OBJDESC
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
BOOL OBJDESC::SetFunction(
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
// CLASS.......:  OBJDESC
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
BOOL OBJDESC::SetDLL(
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
// CLASS.......:  OBJDESC
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
void OBJDESC::ConvertToPointers(
	void)
	{

	helpFile.ConvertToPointers();
	routines.ConvertToPointers();
	responses.ConvertToPointers();
	COMPDESC::ConvertToPointers();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetResponse
//
// CLASS.......:  OBJDESC
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
LPRESPONSE OBJDESC::pGetResponse(
	COMPUID		eventID)
	{

	return responses.pGetResponse(eventID);

	}
