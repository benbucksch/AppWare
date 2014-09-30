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
	: SERIUSARRAY(sizeof(ATYPEID))
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
	const TYPEIDARRAY &	srcAry):
		SERIUSARRAY(sizeof(ATYPEID), srcAry.GetCount())
	{
	short	i;
	unsigned short count;
	ATYPEID*		pType;

	count = GetCount();

	for (i = 0; i < count; i++)
		{
		pType  = (ATYPEID*) GetElement(i);
		*pType = *((ATYPEID*) srcAry.GetElement(i));
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

ATYPEID	TYPEIDARRAY::GetElement(
	short		nIdx
	) const
	{
	ATYPEID		tType;
	ATYPEID*		pType;

	pType = (ATYPEID*) SERIUSARRAY::GetElement(nIdx);

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

ATYPEID	TYPEIDARRAY::pGetElement(
	short		nIdx
	) const
	{
	ATYPEID		tType;
	ATYPEID*		pType;

	//stupic cast because of someone only partially impelmenting 
	//const correctness.
	pType = (ATYPEID*) ((TYPEIDARRAY*)this)->SERIUSARRAY::pGetElement(nIdx);

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
	ATYPEID	tType
	)
	{
	ATYPEID*		pType;

	if (GetCount() < nIdx + 1)
		Resize((short)(nIdx + 1));

	if (GetCount() > nIdx)
		{
		pType = (ATYPEID*) SERIUSARRAY::GetElement(nIdx);
		*pType = tType;
		}
	}
