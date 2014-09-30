#include "windows.h"
#include "almsys.h"
#include "typeary.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	TYPEIDARRAY
//
// CLASS.......:  TYPEIDARRAY
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
TYPEIDARRAY::TYPEIDARRAY()
	: SERIUSARRAY(sizeof(TYPEID))
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	TYPEIDARRAY
//
// CLASS.......:  TYPEIDARRAY
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
TYPEIDARRAY::TYPEIDARRAY(
	const TYPEIDARRAY far&	srcAry):
		SERIUSARRAY(sizeof(TYPEID), srcAry.GetCount())
	{
	int i;
	unsigned short count;
	TYPEID*		pType;

	count = GetCount();

	for (i = 0; i < count; i++)
		{
		pType  = (TYPEID*) GetElement(i);
		*pType = *((TYPEID*) srcAry.GetElement(i));
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetElement
//
// CLASS.......:  TYPEIDARRAY
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

TYPEID	TYPEIDARRAY::GetElement(
	short		nIdx
	) const
	{
	TYPEID		tType;
	TYPEID*		pType;

	pType = (TYPEID*) SERIUSARRAY::GetElement(nIdx);

	if (pType != NULL)
		tType = *pType;
	else
		tType = NULL;

	return tType;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	pGetElement
//
// CLASS.......:  TYPEIDARRAY
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

TYPEID	TYPEIDARRAY::pGetElement(
	short		nIdx
	) const
	{
	TYPEID		tType;
	TYPEID*		pType;

	pType = (TYPEID*) SERIUSARRAY::pGetElement(nIdx);

	if (pType != NULL)
		tType = *pType;
	else
		tType = NULL;

	return tType;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetElement
//
// CLASS.......:  TYPEIDARRAY
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

void	TYPEIDARRAY::SetElement(
	short		nIdx,
	TYPEID	tType
	)
	{
	TYPEID*		pType;

	if (GetCount() < nIdx + 1)
		Resize(nIdx + 1);

	if (GetCount() > nIdx)
		{
		pType = (TYPEID*) SERIUSARRAY::GetElement(nIdx);
		*pType = tType;
		}
	}
