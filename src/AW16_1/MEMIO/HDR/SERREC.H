#ifndef __SERREC_H
#define	__SERREC_H

#include <memory.h>
#include "seruid.h"
#include "rtuid.h"

class SERIUSRECORD
	{
	public:
	SERUID copyUID;

	SERIUSRECORD(
		void);

	~SERIUSRECORD(
		void);

	void far * operator new(
		size_t	size);

	void far * operator new(
		size_t	     size,
		RUNTIMEUID	 currentBlock);

	void far * operator new(
		size_t			size,
		void far*		objPtr);

	void operator delete(
		void far *	objptr,
		size_t			size);

	virtual short PostReadInit(
		void);

	};

#endif
