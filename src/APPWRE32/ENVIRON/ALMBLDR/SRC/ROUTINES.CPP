#include "almsys.h"
#include "routines.h"
#include "dbstring.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ROUTINES
//
// CLASS.......:  ROUTINES
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
ROUTINES::ROUTINES():dlls(),functions()
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ROUTINES
//
// CLASS.......:  ROUTINES
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
ROUTINES::ROUTINES(
	short	count):dlls(count),functions(count)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Update
//
// CLASS.......:  ROUTINES
//
// DESCRIPTION.:  Updates an existing ROUTINES string with a new value
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL ROUTINES::Update(
	LPSTR  dll,
	LPSTR	 function,
	short 	 index)
	{
	BOOL result = TRUE;

	if ((index < dlls.GetCount()) && (index >= 0))
		{
		dlls[index] = dll;
		functions[index] = function;
		}
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Add
//
// CLASS.......:  ROUTINES
//
// DESCRIPTION.:	Adds a new ROUTINES string to the end of the array
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int ROUTINES::Add(
	LPSTR  dll,
	LPSTR	 function)
	{
	short newCount;

	newCount = (short)(dlls.GetCount() + 1);

	dlls.Resize(newCount);
	functions.Resize(newCount);

	dlls[(short)(newCount-1)] = dll;
	functions[(short)(newCount-1)] = function;

	return (int)newCount;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Delete
//
// CLASS.......:  ROUTINES
//
// DESCRIPTION.:	Deletes an existing ROUTINES string
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int ROUTINES::Delete(
	short	index)
	{
	BOOL result = TRUE;

	if ((index < dlls.GetCount()) && (index >= 0))
		{
		dlls.Delete(index);
		functions.Delete(index);
		}
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  ROUTINES
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
void ROUTINES::ConvertToPointers(
	void)
	{

	dlls.ConvertToPointers();
	functions.ConvertToPointers();

	}
