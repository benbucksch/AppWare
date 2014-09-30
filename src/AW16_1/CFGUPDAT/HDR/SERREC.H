#ifndef __SERREC_H
#define	__SERREC_H
#include <mem.h>
#include "seruid.h"
#include "recprefx.h"

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
		size_t	 size,
		SERUID	 currentBlock);

	void far * operator new(
		size_t			size,
		void far*		objPtr);

	void operator delete(
		void far *	objptr,
		size_t			size);

	virtual short PostReadInit(
		void);

	};

typedef SERIUSRECORD far*	LPSERIUSRECORD;

#endif
