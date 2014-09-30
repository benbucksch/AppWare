#include "almsys.h"

#include "tstring.h"
#include "dbstring.h"
#include "system.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator =
//
// CLASS.......:  STRING
//
// DESCRIPTION.:  assigns one string to another.  Frees the destination
//								string and increments the reference count on the source
//                string.
//
// PARAMETERS..:  srcString - the sources string.
//
/////////////////////////////////////////////////////////////////////////////
TSTRING & TSTRING::operator =(
	const TSTRING &		srcString)
	{
	LPSTRINGSPACE   stringSpace;

	stringSpace = pTheSystem->GetTempStringSpace();

	if (this->blockIndex != -1)
		stringSpace->StringFree(this->blockIndex, this->offset);

	this->blockIndex = srcString.blockIndex;
	this->offset = srcString.offset;

	stringSpace->IncrementRefCount(srcString.blockIndex, srcString.offset);
	 
	return *this;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	TSTRING
//
// CLASS.......:	TSTRING
//
// DESCRIPTION.:  constructor for creating a temp string from a database
//								string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
TSTRING::TSTRING(
	DBSTRING	srcString)
	{
	LPCSTR	pSrcString	= srcString;
	LPSTR	pString;

	if (pSrcString)
		{
		pString = pTheSystem->GetTempStringSpace()->
		 StringAlloc(lstrlen(pSrcString)+1, this->blockIndex, this->offset);
		lstrcpy(pString, pSrcString);
		}
	else
		this->blockIndex = -1;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	TSTRING
//
// CLASS.......:	TSTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
TSTRING::TSTRING(
	LPCSTR		srcString)
	{
	char*		pString;

	if (srcString)
		{
		pString = pTheSystem->GetTempStringSpace()->
		 StringAlloc(lstrlen(srcString)+1, this->blockIndex, this->offset);
		lstrcpy(pString, srcString);
		}
	else
		this->blockIndex = -1;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	TSTRING (copy constructor)
//
// CLASS.......:	TSTRING
//
// DESCRIPTION.:  constructor for creating a serius string from a null
//								terminated character string.
//
// PARAMETERS..:	srcString - the sources character string.
//
/////////////////////////////////////////////////////////////////////////////
TSTRING::TSTRING(
	const TSTRING & srcString)
	{

	if (srcString.blockIndex != -1)
    pTheSystem->GetTempStringSpace()->
     IncrementRefCount(srcString.blockIndex, srcString.offset);	 

	this->blockIndex = srcString.blockIndex;
	this->offset = srcString.offset;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	TSTRING
//
// CLASS.......:	TSTRING
//
// DESCRIPTION.:  Creates string by reading it from a file.  The string is
//								read from the current location in the file.  The characters
//                of the string are assumed to be preceded by the count of
//                the number of characters in the string including the
//								terminating null character.  The file must also contain
//                the terminting null character.
//
// PARAMETERS..:  hFile - the handle of the file from which the string is to
//									be read.
//
/////////////////////////////////////////////////////////////////////////////
TSTRING::TSTRING(
	HANDLE		hFile)
	{
	short	size;
	DWORD readSize;
	LPSTR	pString;

	ReadFile(hFile, (LPSTR)&size, sizeof(short), &readSize, NULL);
	if (sizeof(short) != readSize)
		{
		MessageBox(NULL, "Error in reading String size", "File I/O Error", MB_OK|MB_TASKMODAL);
		}
	else
		{
		pString = pTheSystem->GetTempStringSpace()->
		 StringAlloc(size, this->blockIndex, this->offset);
		ReadFile(hFile, pString, size, &readSize, NULL);
		if (size != (short)readSize)
			MessageBox(NULL, "Error in read string from file.",
								 "File I/O Error", MB_OK|MB_TASKMODAL);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~TSTRING
//
// CLASS.......:  TSTRING
//
// DESCRIPTION.:  The destructor for a serius string.
//
/////////////////////////////////////////////////////////////////////////////
TSTRING::~TSTRING(
	void)
	{

	if (this->blockIndex != -1)
		pTheSystem->GetTempStringSpace()->StringFree(this->blockIndex, this->offset);

	this->blockIndex = -1;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	strlen
//
// CLASS.......:  TSTRING
//
// DESCRIPTION.:  This function returns the length of the string in bytes
/////////////////////////////////////////////////////////////////////////////
int TSTRING::strlen(
	void)
	{

	if (this->blockIndex == -1)
		return(0);
	else
		return lstrlen((LPSTR)pTheSystem->GetTempStringSpace()->
		 GetPointer(this->blockIndex, this->offset));
	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator  == 
//
// CLASS.......:  TSTRING
//
// DESCRIPTION.:  This function does a case sensitive compare  
//                and returns TRUE if the strings are equal
/////////////////////////////////////////////////////////////////////////////
BOOL TSTRING::operator ==(
  TSTRING  & str2)
  {
	LPSTRINGSPACE	 pStringSpace;
  LPSTR          pString1;
  LPSTR          pString2;
  BOOL           result;
	
  if ((this->blockIndex == -1) && (str2.blockIndex == -1))
    result = TRUE;
  else if ((this->blockIndex == -1) || (str2.blockIndex == -1))
    result = FALSE;
  else
    {
	  pStringSpace = pTheSystem->GetTempStringSpace();
    pString1 = pStringSpace->GetPointer(this->blockIndex, this->offset);
    pString2 = pStringSpace->GetPointer(str2.blockIndex, str2.offset);
    result = (0 == lstrcmp(pString1, pString2));
    }
  
  return result;    
      
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator  == 
//
// CLASS.......:  TSTRING
//
// DESCRIPTION.:  This inline function does a case sensitive compare and returns
//                TRUE if the strings are NOT equal
/////////////////////////////////////////////////////////////////////////////
BOOL TSTRING::operator !=(
  TSTRING  & str2)
  {
	LPSTRINGSPACE	 pStringSpace;
  LPSTR          pString1;
  LPSTR          pString2;
  BOOL           result;
	
  if ((this->blockIndex == -1) && (str2.blockIndex == -1))
    result = FALSE;
  else if ((this->blockIndex == -1) || (str2.blockIndex == -1))
    result = TRUE;
  else
    {
	  pStringSpace = pTheSystem->GetTempStringSpace();
    pString1 = pStringSpace->GetPointer(this->blockIndex, this->offset);
    pString2 = pStringSpace->GetPointer(str2.blockIndex, str2.offset);
    result = (0 != lstrcmp(pString1, pString2));
    }
  
  return result;    
      
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator  char*
//
// CLASS.......:  TSTRING
//
// DESCRIPTION.:  This inline function provides a conversion from a serius 
//                string to a char  *.  It simply returns a copy of the pointer
//                to the character string that is stored in the serius string 
//                record.  Because of this, the the returned char * pointer 
//                should not be used to alter the character string.
/////////////////////////////////////////////////////////////////////////////
TSTRING::operator const char *()
	{

	if (this->blockIndex == -1)
		return(NULL);
	else
		return (LPSTR)pTheSystem->GetTempStringSpace()->
		 GetPointer(this->blockIndex, this->offset);

	};
