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
		int	count);

	ROUTINES(
		void far*	objPtr):
			dlls(objPtr),
			functions(objPtr)
		{
		}

	BOOL Update(
		LPSTR  dll,
		LPSTR	 function,
		int 	 index);

	int Add(
		LPSTR		dll,
		LPSTR		function);

	int Delete(
		int	index);

	void ConvertToPointers(
		void);

	};


#endif
