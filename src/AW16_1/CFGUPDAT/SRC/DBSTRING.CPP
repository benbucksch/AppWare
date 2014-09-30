#include <windows.h>
#include <dos.h>
#include "strngstr.h"
#include "dbstring.h"
#include "strspace.h"
#include "system.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator =
//
// CLASS.......:  dbstring
//
// DESCRIPTION.:  assigns one string to another.  Frees the destination
//								string and increments the reference count on the source
//                string.
//
// PARAMETERS..:  srcString - the sources string.
//
/////////////////////////////////////////////////////////////////////////////
DBSTRING far& DBSTRING::operator =(
	DBSTRING far&		srcString)
	{
	LPSTRINGSPACE   stringSpace;

	stringSpace = SYSTEM::GetFileObject()->GetStringSpace();

	if (this->hString.blockIndex != -1)
		stringSpace->
		 StringFree(stringSpace->GetPointer(hString.blockIndex, hString.offset));

	this->hString.blockIndex = srcString.hString.blockIndex;
	this->hString.offset = srcString.hString.offset;

	((LPSTRHEAD)stringSpace->
	 GetPointer(srcString.hString.blockIndex, srcString.hString.offset)
	 - 1)->referenceCount += 1;

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator =
//
// CLASS.......:  dbstring
//
// DESCRIPTION.:  assigns one string to another.  Frees the destination
//								string and increments the reference count on the source
//                string.
//
// PARAMETERS..:  srcString - the sources string.
//
/////////////////////////////////////////////////////////////////////////////
DBSTRING far& DBSTRING::operator =(
	TSTRING far&		srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;

	stringSpace = SYSTEM::GetFileObject()->GetStringSpace();

	if (this->hString.blockIndex != -1)
		stringSpace->
		 StringFree(stringSpace->GetPointer(hString.blockIndex, hString.offset));

	pString =
	 stringSpace->StringAlloc(lstrlen(srcString.pString)+1, tempIndex);

	this->hString.blockIndex = tempIndex;

	lstrcpy(pString, srcString.pString);

	this->hString.offset = FP_OFF(pString);

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator =
//
// CLASS.......:  dbstring
//
// DESCRIPTION.:  assigns one string to another.  Frees the destination
//								string and increments the reference count on the source
//                string.
//
// PARAMETERS..:  srcString - the sources string.
//
/////////////////////////////////////////////////////////////////////////////
DBSTRING far& DBSTRING::operator =(
	LPSTR		srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;

	stringSpace = SYSTEM::GetFileObject()->GetStringSpace();

	if (this->hString.blockIndex != -1)
		stringSpace->
		 StringFree(stringSpace->GetPointer(hString.blockIndex, hString.offset));

	pString = stringSpace->StringAlloc(lstrlen(srcString)+1, tempIndex);

	this->hString.blockIndex = tempIndex;

	lstrcpy(pString, srcString);

	this->hString.offset = FP_OFF(pString);

	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DBSTRING
//
// CLASS.......:	DBSTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
DBSTRING::DBSTRING(
	LPSTR		srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;

	stringSpace = SYSTEM::GetFileObject()->GetStringSpace();

	pString = stringSpace->StringAlloc(lstrlen(srcString)+1, tempIndex);

	this->hString.blockIndex = tempIndex;

	lstrcpy(pString, srcString);

	this->hString.offset = FP_OFF(pString);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DBSTRING
//
// CLASS.......:	DBSTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
DBSTRING::DBSTRING(
	TSTRING	far&	srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;

	stringSpace = SYSTEM::GetFileObject()->GetStringSpace();

	pString = stringSpace->StringAlloc(lstrlen(srcString.pString)+1, tempIndex);

	this->hString.blockIndex = tempIndex;

	lstrcpy(pString, srcString.pString);

	this->hString.offset = FP_OFF(pString);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DBSTRING (copy constructor)
//
// CLASS.......:	DBSTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
DBSTRING::DBSTRING(
	const DBSTRING far& srcString)
	{

	if (srcString.hString.blockIndex != -1)
		((LPSTRHEAD)SYSTEM::GetFileObject()->GetStringSpace()->
		 GetPointer(srcString.hString.blockIndex, srcString.hString.offset)-1)->
		 referenceCount += 1;

	this->hString.blockIndex = srcString.hString.blockIndex;
	this->hString.offset = srcString.hString.offset;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~DBSTRING
//
// CLASS.......:  DBSTRING
//
// DESCRIPTION.:  The destructor for a serius string.
//
/////////////////////////////////////////////////////////////////////////////
DBSTRING::~DBSTRING(
	void)
	{

	if (hString.blockIndex != -1)
		(LPSTR)SYSTEM::GetFileObject()->GetStringSpace()->
		 StringFree(hString.blockIndex, hString.offset);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	strlen
//
// CLASS.......:  DBSTRING
//
// DESCRIPTION.:  returns the length of a database string
//
/////////////////////////////////////////////////////////////////////////////
short DBSTRING::strlen(
	void)
	{
	if (hString.blockIndex == -1)
		return(0);
	else
		return lstrlen((LPSTR)SYSTEM::GetFileObject()->GetStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator char far*()
//
// CLASS.......:  DBSTRING
//
// DESCRIPTION.:  This inline function provides a conversion from a serius
// 								database string to a char far *.  It simply returns a copy
//								of the pointer to the character string that is stored in
//								the serius string record.  Because of this, the returned
//								char far* pointer should not be used to alter the character
// 								string.
/////////////////////////////////////////////////////////////////////////////
DBSTRING::operator char far*(void)
	{

	if (hString.blockIndex == -1)
		return(NULL);
	else
		return (LPSTR)SYSTEM::GetFileObject()->GetStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointer
//
// CLASS.......:  DBSTRING
//
// DESCRIPTION.:  returns the length of a database string
//
/////////////////////////////////////////////////////////////////////////////
void DBSTRING::ConvertToPointers(
	void)
	{
	if (hString.blockIndex == -1)
		pString = NULL;
	else
		pString = (LPSTR)SYSTEM::GetFileObject()->GetStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset);
	}

