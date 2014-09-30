#include "almsys.h"

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
DBSTRING & DBSTRING::operator =(
	DBSTRING &		srcString)
	{
	LPSTRINGSPACE   stringSpace;

	stringSpace = pTheSystem->GetFileObject()->GetStringSpace();

	if (this->hString.blockIndex != -1)
		stringSpace->StringFree(hString.blockIndex, hString.offset);

	this->hString.blockIndex = srcString.hString.blockIndex;
	this->hString.offset = srcString.hString.offset;

	stringSpace->
	 IncrementRefCount(srcString.hString.blockIndex, srcString.hString.offset);

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
DBSTRING & DBSTRING::operator =(
	TSTRING &		srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;
	WORD					tempOffset;
  

	stringSpace = pTheSystem->GetFileObject()->GetStringSpace();

	if (this->hString.blockIndex != -1)
		stringSpace->
		 StringFree(hString.blockIndex, hString.offset);

	pString =
	 stringSpace->StringAlloc(srcString.strlen()+1, tempIndex, tempOffset);

	this->hString.blockIndex = tempIndex;
  this->hString.offset = tempOffset;

	lstrcpy(pString, srcString);

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
DBSTRING & DBSTRING::operator =(
	LPCSTR		srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;
  WORD          tempOffset;

	stringSpace = pTheSystem->GetFileObject()->GetStringSpace();

	if (this->hString.blockIndex != -1)
		stringSpace->
		 StringFree(hString.blockIndex, hString.offset);

	pString = 
	 stringSpace->StringAlloc(lstrlen(srcString)+1, tempIndex, tempOffset);

	this->hString.blockIndex = tempIndex;
  this->hString.offset = tempOffset;
  
	if (pString != NULL)
	  lstrcpy(pString, srcString);
  
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
	LPCSTR		srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	short					tempIndex;
  WORD          tempOffset;

	stringSpace = pTheSystem->GetFileObject()->GetStringSpace();

	pString = 
	 stringSpace->StringAlloc(lstrlen(srcString)+1, tempIndex, tempOffset);

	this->hString.blockIndex = tempIndex;
  this->hString.offset = tempOffset;

	if (pString != NULL)
	  lstrcpy(pString, srcString);

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
	TSTRING	&	srcString)
	{
	LPSTRINGSPACE	stringSpace;
	LPSTR					pString;
	
	stringSpace = pTheSystem->GetFileObject()->GetStringSpace();

	pString = stringSpace->StringAlloc(srcString.strlen()+1, 
	 this->hString.blockIndex, this->hString.offset);

  if (pString != NULL)
    lstrcpy(pString, srcString);
    
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
	const DBSTRING & srcString)
	{

	if (srcString.hString.blockIndex != -1)
		pTheSystem->GetFileObject()->GetStringSpace()->
		 IncrementRefCount(srcString.hString.blockIndex, srcString.hString.offset);

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
		(LPSTR)pTheSystem->GetFileObject()->GetStringSpace()->
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
		return (short)(lstrlen((LPSTR)pTheSystem->GetFileObject()->GetStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset)));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator char *()
//
// CLASS.......:  DBSTRING
//
// DESCRIPTION.:  This inline function provides a conversion from a serius
// 								database string to a char  *.  It simply returns a copy
//								of the pointer to the character string that is stored in
//								the serius string record.  Because of this, the returned
//								char * pointer should not be used to alter the character
// 								string.
/////////////////////////////////////////////////////////////////////////////
DBSTRING::operator const char*(void)
	{

	if (hString.blockIndex == -1)
		return(NULL);
	else
		return (LPSTR)pTheSystem->GetFileObject()->GetStringSpace()->
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
		pString = (LPSTR)pTheSystem->GetFileObject()->GetStringSpace()->
		 GetPointer(hString.blockIndex, hString.offset);
	}

