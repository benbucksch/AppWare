#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "rtuiddf.h"
#include "classdef.h"

class SERIUSARRAY
	{
private:
	unsigned short	elementSize;
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
		void *	objPtr)
		{
		}

	~SERIUSARRAY(void);

	void * Insert(
		unsigned short index);

	BOOL Delete(
		unsigned short	index);

	BOOL Resize(
		unsigned short newCount);

	BOOL Write(
		HANDLE	  hFile,
		DWORD  &writeCount);

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

	void *	GetElement(
		unsigned short index) const;

	void *	pGetElement(
		unsigned short index);

	void ConvertToPointers(
		void);

	};

#endif
