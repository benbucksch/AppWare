#include "almsys.h"
#include "evntdesc.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AEVENTDESC
//
// CLASS.......:  AEVENTDESC
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
AEVENTDESC::AEVENTDESC(
	COMPUID	id,
	TSTRING 	name):ACOMPDESC(id, name)
	{

	typeID = COMPUID_GENERAL;

	}

