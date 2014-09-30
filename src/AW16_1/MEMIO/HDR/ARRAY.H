#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <memory.h>
#include "rtuiddf.h"
#include "classdef.h"

class SERIUSARRAY
	{
private:
	size_t					elementSize;
	unsigned short	elementCount;

protected:
	RUNTIMEUID			blockID;

public:

	SERIUSARRAY(void);

	SERIUSARRAY(
		short						size,
		unsigned short  count = 0);

	SERIUSARRAY(
		short						size,
		unsigned short  count,
		LPSERIUSBLOCK		blockID);

	SERIUSARRAY(
		void far*	objPtr)
		{
		}

	~SERIUSARRAY(void);

	void far* Insert(
		unsigned short index);

	BOOL Delete(
		unsigned short	index);

	BOOL Resize(
		unsigned short newCount);

	BOOL Write(
		int	  hFile,
		WORD  &writeCount);

	unsigned short GetCount(
		void) const
		{
		return elementCount;
		}

	size_t GetSize(
		void)
		{
		return elementSize;
		}

	void far*	GetElement(
		unsigned short index) const;

	void far*	pGetElement(
		unsigned short index);

	void ConvertToPointers(
		void);

	};

#endif
