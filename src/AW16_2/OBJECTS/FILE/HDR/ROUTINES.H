#ifndef __ROUTINES_H__
#define __ROUTINES_H__

#include "dbsarray.h"

class ROUTINES
	{
public:
	DBSTRINGARRAY		dlls;
	DBSTRINGARRAY	 	functions;

	ROUTINES();

	ROUTINES(
		short	count);

	ROUTINES(
		void *	objPtr):
			dlls(objPtr),
			functions(objPtr)
		{
		}

	BOOL Update(
		LPSTR  dll,
		LPSTR	 function,
		short 	 index);

	int Add(
		LPSTR		dll,
		LPSTR		function);

	int Delete(
		short	index);

	void ConvertToPointers(
		void);

	};


#endif
