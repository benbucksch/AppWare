#include "almsys.h"

#include "rectypes.h"
#include "funcdesc.h"
#include "recblk.h"
#include "sereclib.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AFUNCDESC
//
// CLASS.......:  AFUNCDESC
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
AFUNCDESC::AFUNCDESC(
	COMPUID	id,
	TSTRING 	name):
		ACOMPDESC(id, name),
		requiredTypes(),
		help(""),
		entryPoint(""),
		flows((unsigned short)0),
		inputs((unsigned short)0),
		outputs((unsigned short)0)
	{

	typeID_UNUSED = COMPUID_GENERAL;

	dll[0] = '\0';

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetLibName
//
// CLASS.......:  AFUNCDESC
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
BOOL AFUNCDESC::SetLibName(
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
// CLASS.......:  AFUNCDESC
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
LPSERIUSICON	AFUNCDESC::GetIcons(
	void)
	{

	return &icons;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetInputsCount
//
// CLASS.......:  AFUNCDESC
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
int AFUNCDESC::GetInputsCount(
	void)
	{
	return inputs.GetCount();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetOutputsCount
//
// CLASS.......:  AFUNCDESC
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
int AFUNCDESC::GetOutputsCount(
	void)
	{
	return outputs.GetCount();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetFlowsCount
//
// CLASS.......:  AFUNCDESC
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
int AFUNCDESC::GetFlowsCount(
	void)
	{
	return flows.GetCount();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetInput
//
// CLASS.......:  AFUNCDESC
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
LPPARAMDESC AFUNCDESC::GetInput(
	int index)
	{
	return (LPPARAMDESC)inputs.pGetElement((short)index);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetOutput
//
// CLASS.......:  AFUNCDESC
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
LPPARAMDESC AFUNCDESC::GetOutput(
	int index)
	{
	return (LPPARAMDESC)outputs.pGetElement((short)index);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetFlow
//
// CLASS.......:  AFUNCDESC
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
LPFLOWDESC AFUNCDESC::GetFlow(
	int index)
	{
	return (index < flows.GetCount())?
		(LPFLOWDESC)flows.pGetElement((short)index):NULL;
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  AFUNCDESC
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
void AFUNCDESC::ConvertToPointers(
	void)
	{

	ACOMPDESC::ConvertToPointers();
	requiredTypes.ConvertToPointers();
	help.ConvertToPointers();
	entryPoint.ConvertToPointers();
	flows.ConvertToPointers();
	inputs.ConvertToPointers();
	outputs.ConvertToPointers();

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetRequiredType
//
// CLASS.......:  AFUNCDESC
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
void		AFUNCDESC::SetRequiredType(
	ATYPEID		t)
	{
	requiredTypes.SetElement(0, t);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetRequiredType
//
// CLASS.......:  AFUNCDESC
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
ATYPEID	AFUNCDESC::GetRequiredType(
	void)
	{
	return requiredTypes.GetElement(0);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetRequiredType
//
// CLASS.......:  AFUNCDESC
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
ATYPEID	AFUNCDESC::pGetRequiredType(
	void)
	{
	return requiredTypes.pGetElement(0);
	}

