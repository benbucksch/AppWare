#include "windows.h"
#include "response.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	RESPONSE
//
// CLASS.......:  RESPONSE
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
RESPONSE::RESPONSE():
		function(),
		eventID((long)NULLCOMPUID)
	{

	dll[0] = '\0';

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	RESPONSE
//
// CLASS.......:  RESPONSE
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
RESPONSE::RESPONSE(
	COMPUID	event,
	LPSTR		dllName,
	LPSTR		functionName):
		function(functionName),
		eventID(event)
	{

	if (lstrlen(dllName) <= MAXDLLCHARS)
		lstrcpy(dll, dllName);
	else
		dll[0] = '\0';

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	RESPONSE
//
// CLASS.......:  RESPONSE
//
// DESCRIPTION.:	Copy Constructor
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
RESPONSE::RESPONSE(
	RESPONSE far& srcResponse):
		function(srcResponse.function),
		eventID(srcResponse.eventID)
	{

	lstrcpy(dll, srcResponse.dll);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	RESPONSE
//
// CLASS.......:  RESPONSE
//
// DESCRIPTION.:	Assignment Operator
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
RESPONSE far& RESPONSE::operator =(
	RESPONSE far&		srcResponse)
	{

	eventID = srcResponse.eventID;

	function = srcResponse.function;

	lstrcpy(dll, srcResponse.dll);

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetDll
//
// CLASS.......:  RESPONSE
//
// DESCRIPTION.:	Deletes an existing RESPONSE string
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL RESPONSE::SetDll(
	LPSTR	pDll)
	{

	if (lstrlen(pDll) <= MAXDLLCHARS)
		{
		lstrcpy(dll, pDll);
		return TRUE;
		}
	else
		return FALSE;

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  RESPONSE
//
// DESCRIPTION.:	Deletes an existing RESPONSE string
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void RESPONSE::ConvertToPointers(
	void)
	{

	function.ConvertToPointers();

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RESPONSEARRAY
//
// CLASS.......: RESPONSEARRAY
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
RESPONSEARRAY::RESPONSEARRAY(
	void):
		SERIUSARRAY(sizeof(RESPONSE), 0)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RESPONSEARRAY
//
// CLASS.......: RESPONSEARRAY
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
RESPONSEARRAY::RESPONSEARRAY(
	unsigned short	count):
		SERIUSARRAY(sizeof(RESPONSE), count)
	{
	unsigned short i;

	for (i=0; i<count; i++)
		new(GetElement(i)) RESPONSE();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RESPONSEARRAY
//
// CLASS.......: RESPONSEARRAY
//
// DESCRIPTION.: Copy constructor
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
RESPONSEARRAY::RESPONSEARRAY(
	const RESPONSEARRAY far&	srcArray):
		SERIUSARRAY(sizeof(RESPONSE), srcArray.GetCount())
	{
	unsigned short i;
	unsigned short count;

	count = srcArray.GetCount();

	for (i=0;	i<count; i++)
		new(GetElement(i)) RESPONSE(srcArray[i]);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator []
//
// CLASS.......: RESPONSEARRAY
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
RESPONSE far& RESPONSEARRAY::operator[](
	  unsigned short	index
    ) const
	{

	return *(LPRESPONSE)GetElement(index);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Insert
//
// CLASS.......: RESPONSEARRAY
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
LPRESPONSE RESPONSEARRAY::Insert(
	unsigned short	index,
	COMPUID					eventID,
	LPSTR 				 	dll,
	LPSTR						function)
	{
	void far*	 pNewResponse;

	pNewResponse = SERIUSARRAY::Insert(index);

	if (pNewResponse)
		new(pNewResponse) RESPONSE(eventID, dll, function);

	return (LPRESPONSE)pNewResponse;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Delete
//
// CLASS.......: RESPONSEARRAY
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
BOOL RESPONSEARRAY::Delete(
	unsigned short index)
	{

	delete (LPRESPONSE)GetElement(index);

	SERIUSARRAY::Delete(index);

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RESPONSEARRAY
//
// CLASS.......: RESPONSEARRAY
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
BOOL RESPONSEARRAY::Resize(
	unsigned short newCount)
	{
	int i;
	unsigned short	oldCount;
	BOOL						result;

	oldCount = GetCount();

	if (oldCount > newCount)
		{	//delete old strings
		for (i=newCount; i<oldCount; i++)
			delete (LPRESPONSE)GetElement(i);
		result = SERIUSARRAY::Resize(newCount);
		}
	else if (oldCount < newCount)
		{	//init new strings
    result = SERIUSARRAY::Resize(newCount);
		if (result)
			for (i=oldCount; i<newCount; i++)
				new(GetElement(i)) RESPONSE();
		}

	result = TRUE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: operator =
//
// CLASS.......: RESPONSEARRAY
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
RESPONSEARRAY far& RESPONSEARRAY::operator =(
	RESPONSEARRAY far&	srcArray)
	{
	int i;
	unsigned short	newCount;

	this->Resize(newCount = srcArray.GetCount());

	for (i=0; i<newCount; i++)
		(*this)[i] = srcArray[i];

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: pGetResponse
//
// CLASS.......: RESPONSEARRAY
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
LPRESPONSE RESPONSEARRAY::pGetResponse(
	COMPUID		eventID)
	{
	int i;
	unsigned short	count;
	LPRESPONSE			pResponse;

	count = GetCount();

	for (i=0; i<count; i++)
		{
		pResponse = (LPRESPONSE)pGetElement(i);
		if (pResponse->GetEventID() == eventID)
			break;
		}

	if (i == count)
		pResponse = 0;

	return pResponse;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ConvertToPointers
//
// CLASS.......: RESPONSEARRAY
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
void RESPONSEARRAY::ConvertToPointers(
	void)
	{
	int i;
	unsigned short	count;

	count = GetCount();

	for (i=0; i<count; i++)
		((LPRESPONSE)GetElement(i))->ConvertToPointers();

	SERIUSARRAY::ConvertToPointers();

	}
