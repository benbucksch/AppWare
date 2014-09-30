#ifndef __EXSERREC_H
#define	__EXSERREC_H

#include "seruid.h"
#include "rtuid.h"

class EXESERIUSRECORD
	{
	public:

	EXESERIUSRECORD(
		void);

	~EXESERIUSRECORD(
		void);

	void  * operator new(
		size_t	size);

	void  * operator new(
		size_t	    size,
		RUNTIMEUID  currentBlock);

	void  * operator new(
		size_t			size,
		void *		objPtr);

	void operator delete(
		void  *	objptr,
		size_t			size);

	virtual void PostReadInit(
		void);

	};

#endif
