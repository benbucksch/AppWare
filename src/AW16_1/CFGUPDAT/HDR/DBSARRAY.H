#ifndef __DBSTRINGARRAY__
#define __DBSTRINGARRAY__
#include "array.h"
#include "dbstring.h"

class DBSTRINGARRAY:public SERIUSARRAY
	{

public:

	DBSTRINGARRAY(void);

	DBSTRINGARRAY(
		unsigned short	count);

	DBSTRINGARRAY(
		unsigned short 	count,
		SERUID	arrayBlockID);

	DBSTRINGARRAY(
		void far*	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	DBSTRING far& operator[](
		unsigned short	index
    ) const;

	BOOL Resize(
		unsigned short newCount);

	DBSTRINGARRAY far& operator =(
		DBSTRINGARRAY far&	srcArray);

	void ConvertToPointers(
		void);

	};

#endif