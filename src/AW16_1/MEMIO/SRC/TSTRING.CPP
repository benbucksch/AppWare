#include <dos.h>
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
TSTRING far& TSTRING::operator =(
	const TSTRING far&		srcString)
	{

	if (this->pString != NULL)
		pTheSystem->GetTempStringSpace()->StringFree(pString);

	((LPSTRHEAD)srcString.pString - 1)->referenceCount += 1;

	this->pString = srcString.pString;

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
	LPSTR	pSrcString;
	short	blockIndex;

  pSrcString = srcString;
	if (pSrcString)
		{
		pString = pTheSystem->GetTempStringSpace()->StringAlloc(lstrlen(pSrcString)+1, blockIndex);
		lstrcpy(pString, pSrcString);
		}
	else
		pString = NULL;

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
	LPSTR		srcString)
	{
	short	blockIndex;

	if (srcString)
		{
		pString = pTheSystem->GetTempStringSpace()->
		 StringAlloc(lstrlen(srcString)+1, blockIndex);
		lstrcpy(pString, srcString);
		}
	else
		pString = NULL;

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
	const TSTRING far& srcString)
	{

	if (srcString.pString)
		((LPSTRHEAD)srcString.pString - 1)->referenceCount += 1;

	this->pString = srcString.pString;

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
	int		hFile)
	{
	short	size;
	short 	blockIndex;

	if (sizeof(short) != _lread(hFile, (LPSTR)&size, sizeof(short)))
		{
		MessageBox(NULL, "Error in reading String size", "File I/O Error", MB_OK|MB_TASKMODAL);
		}
	else
		{
		pString = pTheSystem->GetTempStringSpace()->StringAlloc(size, blockIndex);
		if (size != _lread(hFile, pString, size))
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

	if (pString != NULL)
		pTheSystem->GetTempStringSpace()->StringFree(pString);

	pString = NULL;

	}

