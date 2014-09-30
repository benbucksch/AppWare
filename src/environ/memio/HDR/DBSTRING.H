#ifndef __DBSTRING_H
#define __DBSTRING_H

#include "tstring.h"

union DBSTRING
	{
	struct
		{
		short	blockIndex;
		WORD	offset;
		} 								hString;

	char*								pString;


	void  * operator new(
		size_t	size)
		{
		return NULL;
		}

	void  * operator new(
		size_t			size,
		void *		objPtr)
		{
		return objPtr;
		}

	void operator delete(
		void  *	objptr)
		{
		}

	DBSTRING(void)
		{
		hString.blockIndex = -1;
		}

	DBSTRING(
		void *	objPtr)
		{
		}

	DBSTRING(
		const DBSTRING & srcString);

	DBSTRING(
		const char *		srcString);

	DBSTRING(
		TSTRING	&	srcString);

	~DBSTRING(void);

	DBSTRING & operator =(
		DBSTRING &		srcString);

	DBSTRING & operator =(
		TSTRING &		srcString);

	DBSTRING & operator =(
		const char*		srcString);

	void InitDBStringMemory(
		void);

	void FreeDBStringMemory(
		void);

	short strlen(
		void);

	operator const char* (
		void);

	const char*	pGetPointer(
		void)
		{
		return pString;
		}

	void ConvertToPointers(
		void);

	};

#endif
