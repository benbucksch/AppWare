#ifndef __UIDARRAY__
#define __UIDARRAY__

#include "array.h"
#include "compuid.h"

class UIDARRAY:public SERIUSARRAY
	{

public:

	UIDARRAY(void);

	UIDARRAY(
		unsigned short	count);

	UIDARRAY(
		void *	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	UIDARRAY(
		const UIDARRAY & srcArray);

	UIDARRAY(
		unsigned short 	count,
		LPSERIUSBLOCK		arrayBlock);

	COMPUID & operator[](
		unsigned short	index);

	BOOL SetElement(
		unsigned short	index,
		COMPUID					uid,
		LPUIDTABLE			uidTable);

	void ConvertToPointers(
		void);

	};

#endif
