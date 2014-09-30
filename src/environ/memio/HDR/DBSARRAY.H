#ifndef __DBSTRINGARRAY__
#define __DBSTRINGARRAY__

#include "array.h"

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
		void *	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	DBSTRING & operator[](
		unsigned short	index
    ) const;

	BOOL Resize(
		unsigned short newCount);

	DBSTRINGARRAY & operator =(
		DBSTRINGARRAY &	srcArray);

	void ConvertToPointers(
		void);

	};

#endif
