#ifndef __TSTRING_H
#define __TSTRING_H

#include "strspace.h"

class TSTRING
	{
	friend union DBSTRING;
	friend union EXESTRING;

private:

	LPSTR								pString;					 //a pointer to the null terminated
																				 // character string.
public:

	TSTRING(void)
		{
		pString = NULL;
		};

	TSTRING(
		const TSTRING far& srcString);

	TSTRING(
		char far*		srcString);

	TSTRING(
		DBSTRING	srcString);

	TSTRING(
		int	hFile);

	~TSTRING(void);

	TSTRING far& operator =(
		const TSTRING far&		srcString);

/////////////////////////////////////////////////////////////////////////////
// This inline function returns the length of the string in bytes
/////////////////////////////////////////////////////////////////////////////
	short strlen(
		void)
		{
		if (pString == NULL)
			return(-1);
		else
			return lstrlen(pString);
		};

/////////////////////////////////////////////////////////////////////////////
// This inline function does a case sensitive compare  and returns TRUE if
// the strings are equal
/////////////////////////////////////////////////////////////////////////////
  BOOL operator ==(
    TSTRING far & str2)
    {
    if ((this->pString == 0) || (str2.pString == 0))
      return 0;
    else
      return (0 == lstrcmp(this->pString, str2.pString));
    }

/////////////////////////////////////////////////////////////////////////////
// This inline function does a case sensitive compare and returns TRUE if the
// strings are NOT equal
/////////////////////////////////////////////////////////////////////////////
  BOOL operator !=(
    TSTRING far & str2)
    {
    if ((this->pString == 0) || (str2.pString == 0))
      return 1;
    else
      return (0 != lstrcmp(this->pString, str2.pString));
    }

/////////////////////////////////////////////////////////////////////////////
// This inline function provides a conversion from a serius string to a
// char far *.  It simply returns a copy of the pointer to the character
// string that is stored in the serius string record.  Because of this, the
// the returned char far* pointer should not be used to alter the character
// string.
/////////////////////////////////////////////////////////////////////////////
	operator char far*()
		{

		return pString;

		};

	};

typedef TSTRING far*		LPTSTRING;

#endif
