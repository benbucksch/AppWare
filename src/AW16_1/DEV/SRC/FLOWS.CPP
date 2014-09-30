/////////////////////////////////////////////////////////////////////////////
//
//	This file contains the implementations of the member functions of the
//	classes FLOWDESC and FLOWDESCARRAY.
//
/////////////////////////////////////////////////////////////////////////////

#include "windows.h"
#include "flows.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FLOWDESC
//
// CLASS.......: FLOWDESC
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
FLOWDESC::FLOWDESC(void):
	name(),help()
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FLOWDESC
//
// CLASS.......: FLOWDESC
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
FLOWDESC::FLOWDESC(
	LPSTR	theName,
	LPSTR	theHelp):
		name(theName),
		help(theHelp)
	{

	flags = 0x00;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~FLOWDESC
//
// CLASS.......: FLOWDESC
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
FLOWDESC::~FLOWDESC(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator =
//
// CLASS.......: FLOWDESC
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
FLOWDESC far& FLOWDESC::operator =(
	FLOWDESC far&	srcFlow)
	{
	name = srcFlow.name;
	help = srcFlow.help;
	flags = srcFlow.flags;

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ConvertToPointers
//
// CLASS.......: FLOWDESC
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
void FLOWDESC::ConvertToPointers(
	void)
	{

	name.ConvertToPointers();
	help.ConvertToPointers();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FLOWDESCARRAY
//
// CLASS.......: FLOWDESCARRAY
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
FLOWDESCARRAY::FLOWDESCARRAY(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FLOWDESCARRAY
//
// CLASS.......: FLOWDESCARRAY
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
FLOWDESCARRAY::FLOWDESCARRAY(
	unsigned short	count):
		SERIUSARRAY(sizeof(FLOWDESC), count)
	{
	unsigned short i;

	for (i=0; i<count; i++)
		new(GetElement(i)) FLOWDESC();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FLOWDESCARRAY
//
// CLASS.......: FLOWDESCARRAY
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
FLOWDESCARRAY::FLOWDESCARRAY(
	const FLOWDESCARRAY far&	srcArray):
		SERIUSARRAY(sizeof(FLOWDESC), srcArray.GetCount())
	{
	unsigned short i;
	unsigned short count;

	count = srcArray.GetCount();

	for (i=0;	i<count; i++)
		{
		new(GetElement(i)) FLOWDESC();
		(*this)[i] = srcArray[i];
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator []
//
// CLASS.......: FLOWDESCARRAY
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
FLOWDESC far& FLOWDESCARRAY::operator[](
	  unsigned short	index
    ) const
	{
	return *(LPFLOWDESC)GetElement(index);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Insert
//
// CLASS.......: FLOWDESCARRAY
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
BOOL FLOWDESCARRAY::Insert(
	unsigned short	index,
	LPSTR 				 	name,
	LPSTR						help)
	{
	void far *   pFlowDesc;
  BOOL         result = TRUE;

  pFlowDesc = SERIUSARRAY::Insert(index);
  
  if (pFlowDesc)
    new(pFlowDesc) FLOWDESC(name, help);
  else
    result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Delete
//
// CLASS.......: FLOWDESCARRAY
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
BOOL FLOWDESCARRAY::Delete(
	unsigned short index)
	{

	delete (LPFLOWDESC)GetElement(index);

	SERIUSARRAY::Delete(index);

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FLOWDESCARRAY
//
// CLASS.......: FLOWDESCARRAY
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
BOOL FLOWDESCARRAY::Resize(
	unsigned short newCount)
	{
	int i;
	unsigned short	oldCount;
	BOOL						result;

	oldCount = GetCount();

	if (oldCount > newCount)
		{	//delete old strings
		for (i=newCount; i<oldCount; i++)
			delete (LPFLOWDESC)GetElement(i);
		result = SERIUSARRAY::Resize(newCount);
		}
	else if (oldCount < newCount)
		{	//init new strings
    result = SERIUSARRAY::Resize(newCount);
		if (result)
      {
			for (i = oldCount; i < newCount; i++)
				new(GetElement(i)) FLOWDESC();
      }
		}

	result = TRUE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator =
//
// CLASS.......: FLOWDESCARRAY
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
FLOWDESCARRAY far& FLOWDESCARRAY::operator =(
	FLOWDESCARRAY far&	srcArray)
	{
	int i;
	unsigned short	newCount;

	this->Resize(newCount = srcArray.GetCount());

	for (i=0; i<newCount; i++)
		(*this)[i] = srcArray[i];

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ConvertToPointers
//
// CLASS.......: FLOWDESCARRAY
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
void FLOWDESCARRAY::ConvertToPointers(
	void)
	{
	int i;
	unsigned short	count;

	count = GetCount();

	for (i=0; i<count; i++)
		((LPFLOWDESC)GetElement(i))->ConvertToPointers();

	SERIUSARRAY::ConvertToPointers();

	}
