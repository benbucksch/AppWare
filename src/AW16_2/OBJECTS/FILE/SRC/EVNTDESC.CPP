#include "almsys.h"
#include "evntdesc.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	EVENTDESC
//
// CLASS.......:  EVENTDESC
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
EVENTDESC::EVENTDESC(
	COMPUID	id,
	TSTRING 	name):COMPDESC(id, name)
	{

	typeID = COMPUID_GENERAL;

	}

