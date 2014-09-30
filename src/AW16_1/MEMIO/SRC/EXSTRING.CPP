#include <dos.h>
#include "almsys.h"

#include "strngstr.h"
#include "exstring.h"
#include "strspace.h"
#include "system.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator = (take 1)
//
// CLASS.......:  EXESTRING
//
// DESCRIPTION.:  assigns one EXESTRING to another.  Frees the destination
//								EXESTRING and increments the reference count on the source
//                EXESTRING.
//
// PARAMETERS..:  srcString - the source EXESTRING.
//
/////////////////////////////////////////////////////////////////////////////
EXESTRING far& EXESTRING::operator =(
	EXESTRING far&		srcString)
	{
	LPSTRINGSPACE   stringSpace;

	stringSpace = pTheSystem->GetFileObject()->GetExeStringSpace();

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
// FUNCTION....:	operator = (take 2)
//
// CLASS.......:  EXESTRING
//
// DESCRIPTION.:  assigns a C string to an EXESTRING.
//
// PARAMETERS..:  srcString - the source C string.
//
/////////////////////////////////////////////////////////////////////////////
EXESTRING far& EXESTRING::operator =(
  LPSTR  srcString)
  {
	LPSTRINGSPACE   stringSpace;
  LPSTR           pString;
  short           tempIndex;

  stringSpace = pTheSystem->GetFileObject()->GetExeStringSpace();

  if (this->hString.blockIndex != -1)
    {
    stringSpace->StringFree(
      stringSpace->GetPointer(
        hString.blockIndex, hString.offset));
    }

  pString = stringSpace->StringAlloc(lstrlen(srcString)+1, tempIndex);
  lstrcpy(pString, srcString);

  this->hString.blockIndex = tempIndex;
  this->hString.offset = FP_OFF(pString);

  return *this;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	EXESTRING
//
// CLASS.......:	EXESTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
EXESTRING::EXESTRING(
	LPSTR		srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;

	stringSpace = pTheSystem->GetFileObject()->GetExeStringSpace();

	pString = stringSpace->StringAlloc(lstrlen(srcString)+1, tempIndex);

	this->hString.blockIndex = tempIndex;

	lstrcpy(pString, srcString);

	this->hString.offset = FP_OFF(pString);

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	EXESTRING
//
// CLASS.......:	EXESTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
EXESTRING::EXESTRING(
	TSTRING	far&	srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;

	stringSpace = pTheSystem->GetFileObject()->GetExeStringSpace();

	pString = stringSpace->StringAlloc(lstrlen(srcString.pString)+1, tempIndex);

	this->hString.blockIndex = tempIndex;

	lstrcpy(pString, srcString.pString);

	this->hString.offset = FP_OFF(pString);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	EXESTRING (copy constructor)
//
// CLASS.......:	EXESTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
EXESTRING::EXESTRING(
	const EXESTRING far& srcString)
	{

	if (srcString.hString.blockIndex != -1)
		((LPSTRHEAD)pTheSystem->GetFileObject()->GetExeStringSpace()->
		 GetPointer(srcString.hString.blockIndex, srcString.hString.offset)-1)->
		 referenceCount += 1;

	this->hString.blockIndex = srcString.hString.blockIndex;
	this->hString.offset = srcString.hString.offset;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~EXESTRING
//
// CLASS.......:  EXESTRING
//
// DESCRIPTION.:  The destructor for a serius string.
//
/////////////////////////////////////////////////////////////////////////////
EXESTRING::~EXESTRING(
	void)
	{

	if (hString.blockIndex != -1)
		(LPSTR)pTheSystem->GetFileObject()->GetExeStringSpace()->
		 StringFree(hString.blockIndex, hString.offset);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	strlen
//
// CLASS.......:  EXESTRING
//
// DESCRIPTION.:  returns the length of a database string
//
/////////////////////////////////////////////////////////////////////////////
short EXESTRING::strlen(
	void)
	{
	if (hString.blockIndex == -1)
		return(0);
	else
		return lstrlen((LPSTR)pTheSystem->GetFileObject()->GetExeStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator char far*()
//
// CLASS.......:  EXESTRING
//
// DESCRIPTION.:  This inline function provides a conversion from a serius
// 								database string to a char far *.  It simply returns a copy
//								of the pointer to the character string that is stored in
//								the serius string record.  Because of this, the returned
//								char far* pointer should not be used to alter the character
// 								string.
/////////////////////////////////////////////////////////////////////////////
EXESTRING::operator char*(void)
	{

	if (hString.blockIndex == -1)
		return(NULL);
	else
		return (LPSTR)pTheSystem->GetFileObject()->GetExeStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointer
//
// CLASS.......:  EXESTRING
//
// DESCRIPTION.:  returns the length of a database string
//
/////////////////////////////////////////////////////////////////////////////
void EXESTRING::ConvertToPointers(
	void)
	{
	if (hString.blockIndex == -1)
		pString = NULL;
	else
		pString = (LPSTR)pTheSystem->GetFileObject()->GetExeStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset);
	}

