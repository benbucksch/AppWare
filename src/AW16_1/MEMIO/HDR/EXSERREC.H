#ifndef __EXSERREC_H
#define	__EXSERREC_H

#include <memory.h>

#include "seruid.h"
#include "rtuid.h"

class EXESERIUSRECORD
	{
	public:

	EXESERIUSRECORD(
		void);

	~EXESERIUSRECORD(
		void);

	void far * operator new(
		size_t	size);

	void far * operator new(
		size_t	    size,
		RUNTIMEUID  currentBlock);

	void far * operator new(
		size_t			size,
		void far*		objPtr);

	void operator delete(
		void far *	objptr,
		size_t			size);

	virtual void PostReadInit(
		void);

	};

#endif
