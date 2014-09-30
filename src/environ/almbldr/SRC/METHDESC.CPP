#include "almsys.h"
#include "methdesc.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	METHODDESC
//
// CLASS.......:  METHODDESC
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
METHODDESC::METHODDESC(
	COMPUID	id,
	TSTRING 	name):ACOMPDESC(id, name)
	{

	dllName[0] = '\0';

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	METHODDESC
//
// CLASS.......:  METHODDESC
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
TSTRING METHODDESC::GetFunctionName(
	void)
	{

	return functionName;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	METHODDESC
//
// CLASS.......:  METHODDESC
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
LPSTR METHODDESC::GetDLLName(
	void)
	{

	return dllName;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetTypeID
//
// CLASS.......:  METHODDESC
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
COMPUID METHODDESC::GetTypeID(
	void)
	{

	return  typeID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	METHODDESC
//
// CLASS.......:  METHODDESC
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
BOOL METHODDESC::SetFunctionName(
	LPSTR	newFunctionName)
	{

	functionName = newFunctionName;

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	METHODDESC
//
// CLASS.......:  METHODDESC
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
BOOL METHODDESC::SetDLLName(
	LPSTR newDLLName)
	{

	if (lstrlen(newDLLName) <= DLL_NAME_LENGTH)
		{
		lstrcpy(dllName, newDLLName);
		return TRUE;
		}
	else
		return FALSE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetTypeID
//
// CLASS.......:  METHODDESC
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
BOOL METHODDESC::SetTypeID(
	COMPUID	newTypeID)
	{

	typeID = newTypeID;

	return  TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  METHODDESC
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
void METHODDESC::ConvertToPointers(
	void)
	{

	ACOMPDESC::ConvertToPointers();

	functionName.ConvertToPointers();

	}
