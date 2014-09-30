#ifndef __MEMBLK_H__
#define __MEMBLK_H__

#include <mem.h>
#include "serblk.h"

class MEMBLOCK:public SERIUSBLOCK
	{

public:

	void far * operator new(
		size_t	size);

	void far * operator new(
		size_t			size,
		void far*		objPtr);

	void operator delete(
		void far *	objPtr,
		size_t			size);

	MEMBLOCK(
		void):
			SERIUSBLOCK()
		{
		}

	MEMBLOCK(
		long int	count):
			SERIUSBLOCK(count)
		{
		}

	~MEMBLOCK(void)
		{
		}

	};

typedef MEMBLOCK far*	LPMEMBLOCK;


#endif
