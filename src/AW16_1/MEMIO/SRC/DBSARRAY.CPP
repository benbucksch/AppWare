#include "almsys.h"

#include "dbsarray.h"
#include "sereclib.h"
#include "dbstring.h"
#include "serblk.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DBSTRINGARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
DBSTRINGARRAY::DBSTRINGARRAY(
  void):
    SERIUSARRAY(sizeof(DBSTRING), 0)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DBSTRINGARRAY
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
DBSTRINGARRAY::DBSTRINGARRAY(
	unsigned short count):SERIUSARRAY(sizeof(DBSTRING), count)
	{
	int i;

	for (i=0; i<count; i++)
		{
		new(GetElement(i)) DBSTRING();
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Resize
//
// CLASS.......: SERIUSARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL DBSTRINGARRAY::Resize(
	unsigned short newCount)
	{
	int i;
	unsigned short	oldCount;
	BOOL						result;

	oldCount = GetCount();

	if (oldCount > newCount)
		{	//delete old strings
		for (i=newCount; i<oldCount; i++)
			delete (LPDBSTRING)GetElement(i);
		result = SERIUSARRAY::Resize(newCount);
		}
	else if (oldCount < newCount)
		{	//init new strings
    result = SERIUSARRAY::Resize(newCount);
		if (result)
      {
			for (i=oldCount; i<newCount; i++)
				new(GetElement(i)) DBSTRING();
      }
		}

	result = TRUE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator []
//
// CLASS.......: DBSTRINGARRAY
//
// DESCRIPTION.: This is the default constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
DBSTRING far& DBSTRINGARRAY::operator[](
	  unsigned short	index
    ) const
	{

	return *((DBSTRING far*)((LPSERIUSBLOCK)GetObject(blockID))->
	 GetData()+index);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator =
//
// CLASS.......: DBSTRINGARRAY
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
DBSTRINGARRAY far& DBSTRINGARRAY::operator =(
	DBSTRINGARRAY far&	srcArray)
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
// CLASS.......: SERIUSARRAY
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
void DBSTRINGARRAY::ConvertToPointers(
	void)
	{
	unsigned short	count;
	int i;

	count = GetCount();

	for (i=0; i<count; i++)
		((LPDBSTRING)GetElement(i))->ConvertToPointers();

	SERIUSARRAY::ConvertToPointers();

	}

