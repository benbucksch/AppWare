#include "windows.h"
#include "rectypes.h"
#include "funcdesc.h"
#include "recblk.h"
#include "sereclib.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FUNCDESC
//
// CLASS.......:  FUNCDESC
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
FUNCDESC::FUNCDESC(
	COMPUID	id,
	TSTRING 	name):
		COMPDESC(id, name),
		syntax(),
		help(""),
		entryPoint(""),
		flows((unsigned short)0),
		inputs((unsigned short)0),
		outputs((unsigned short)0)
	{

	typeID = COMPUID_GENERAL;

	dll[0] = '\0';

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetLibName
//
// CLASS.......:  FUNCDESC
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
BOOL FUNCDESC::SetLibName(
	LPSTR	pName)
	{

	if (lstrlen(pName) <= MAXDLLCHARS)
		{
		lstrcpy(dll, pName);
		return TRUE;
		}
	else
		return FALSE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetIcons
//
// CLASS.......:  FUNCDESC
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
LPSERIUSICON	FUNCDESC::GetIcons(
	void)
	{

	return &icons;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetInputsCount
//
// CLASS.......:  FUNCDESC
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
int FUNCDESC::GetInputsCount(
	void)
	{
	return inputs.GetCount();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetOutputsCount
//
// CLASS.......:  FUNCDESC
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
int FUNCDESC::GetOutputsCount(
	void)
	{
	return outputs.GetCount();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetFlowsCount
//
// CLASS.......:  FUNCDESC
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
int FUNCDESC::GetFlowsCount(
	void)
	{
	return flows.GetCount();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetInput
//
// CLASS.......:  FUNCDESC
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
LPPARAMDESC FUNCDESC::GetInput(
	int index)
	{
	return (LPPARAMDESC)inputs.pGetElement(index);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetOutput
//
// CLASS.......:  FUNCDESC
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
LPPARAMDESC FUNCDESC::GetOutput(
	int index)
	{
	return (LPPARAMDESC)outputs.pGetElement(index);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetFlow
//
// CLASS.......:  FUNCDESC
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
LPFLOWDESC FUNCDESC::GetFlow(
	int index)
	{
	return (index < flows.GetCount())?
		(LPFLOWDESC)flows.pGetElement(index):NULL;
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  FUNCDESC
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
void FUNCDESC::ConvertToPointers(
	void)
	{

	COMPDESC::ConvertToPointers();
	syntax.ConvertToPointers();
	help.ConvertToPointers();
	entryPoint.ConvertToPointers();
	flows.ConvertToPointers();
	inputs.ConvertToPointers();
	outputs.ConvertToPointers();

	}