/////////////////////////////////////////////////////////////////////////////
//
//	This file contains the implementations of the member functions of the
//	classes PARAMDESC and PARAMDESCARRAY.
//
/////////////////////////////////////////////////////////////////////////////

#include "almsys.h"
#include "params.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: PARAMDESC
//
// CLASS.......: PARAMDESC
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
PARAMDESC::PARAMDESC(void):
	name(),help()
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: PARAMDESC
//
// CLASS.......: PARAMDESC
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
PARAMDESC::PARAMDESC(
	LPSTR			theName,
	LPSTR			theHelp,
	COMPUID		theTypeID,
	DWORD			theFlags):
		name(theName),
		help(theHelp)
	{

	flags = theFlags;
	type = theTypeID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~PARAMDESC
//
// CLASS.......: PARAMDESC
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
PARAMDESC::~PARAMDESC(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator =
//
// CLASS.......: PARAMDESC
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
PARAMDESC & PARAMDESC::operator =(
	PARAMDESC &	srcFlow)
	{
	name = srcFlow.name;
	help = srcFlow.help;
	flags = srcFlow.flags;
	type = srcFlow.type;

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator =
//
// CLASS.......: PARAMDESC
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
void PARAMDESC::ConvertToPointers(
	void)
	{

	name.ConvertToPointers();
	help.ConvertToPointers();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: PARAMDESCARRAY
//
// CLASS.......: PARAMDESCARRAY
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
PARAMDESCARRAY::PARAMDESCARRAY(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: PARAMDESCARRAY
//
// CLASS.......: PARAMDESCARRAY
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
PARAMDESCARRAY::PARAMDESCARRAY(
	unsigned short	count):
		SERIUSARRAY(sizeof(PARAMDESC), count)
	{
	unsigned short i;

	for (i=0; i<count; i++)
		new(GetElement(i)) PARAMDESC();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: PARAMDESCARRAY
//
// CLASS.......: PARAMDESCARRAY
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
PARAMDESCARRAY::PARAMDESCARRAY(
	const PARAMDESCARRAY &	srcArray):
		SERIUSARRAY(sizeof(PARAMDESC), srcArray.GetCount())
	{
	unsigned short i;
	unsigned short count;

	count = srcArray.GetCount();

	for (i=0;	i<count; i++)
		{
		new(GetElement(i)) PARAMDESC("", "", COMPUID_GENERAL, 0);
		(*this)[i] = srcArray[i];
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator []
//
// CLASS.......: PARAMDESCARRAY
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
PARAMDESC & PARAMDESCARRAY::operator[](
	  unsigned short	index
    ) const
	{

	return *(LPPARAMDESC)GetElement(index);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Resize
//
// CLASS.......: PARAMDESCARRAY
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
BOOL PARAMDESCARRAY::Resize(
	unsigned short newCount)
	{
	short	i;
	unsigned short	oldCount;
	BOOL						result;

	oldCount = GetCount();

	if (oldCount > newCount)
		{	//delete old strings
		for (i=newCount; i<oldCount; i++)
			delete (LPPARAMDESC)GetElement(i);
		result = SERIUSARRAY::Resize(newCount);
		}
	else if (oldCount < newCount)
		{	//init new strings
    result = SERIUSARRAY::Resize(newCount);
		if (result)
			for (i=oldCount; i<newCount; i++)
				new(GetElement(i)) PARAMDESC("", "", COMPUID_GENERAL, 0);
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Insert
//
// CLASS.......: PARAMDESCARRAY
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
BOOL PARAMDESCARRAY::Insert(
	unsigned short	index,
	LPSTR 				 	name,
	LPSTR						help,
	COMPUID   			typeID,
	DWORD						flags)
	{
	void  *   pParamDesc;
  BOOL         result = TRUE;

  pParamDesc = SERIUSARRAY::Insert(index);
  
  if (pParamDesc)
    new(pParamDesc) PARAMDESC(name, help, typeID, flags);
  else
    result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: PARAMDESCARRAY
//
// CLASS.......: PARAMDESCARRAY
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
BOOL PARAMDESCARRAY::Delete(
	unsigned short index)
	{

	delete (LPPARAMDESC)GetElement(index);

	SERIUSARRAY::Delete(index);

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator =
//
// CLASS.......: PARAMDESCARRAY
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
PARAMDESCARRAY & PARAMDESCARRAY::operator =(
	PARAMDESCARRAY &	srcArray)
	{
	short	i;
	unsigned short	newCount;

	this->Resize(newCount = srcArray.GetCount());

	for (i=0; i<newCount; i++)
		(*this)[i] = srcArray[i];

	return *this;

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ConvertToPointers
//
// CLASS.......: PARAMDESCARRAY
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
void PARAMDESCARRAY::ConvertToPointers(
	void)
	{
	short	i;
	unsigned short	count;

	count = GetCount();

	for (i=0; i<count; i++)
		((LPPARAMDESC)GetElement(i))->ConvertToPointers();

	SERIUSARRAY::ConvertToPointers();

	}

