#ifndef __SERREC_H
#define	__SERREC_H

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

	void  * operator new(
		size_t	size);

	void  * operator new(
		size_t	     size,
		RUNTIMEUID	 currentBlock);

	void  * operator new(
		size_t			size,
		void *		objPtr);

	void operator delete(
		void  *	objptr,
		size_t			size);

	virtual short PostReadInit(
		void);

	};

#endif
