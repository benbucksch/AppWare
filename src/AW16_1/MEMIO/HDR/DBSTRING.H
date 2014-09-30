#ifndef __DBSTRING_H
#define __DBSTRING_H

#include <memory.h>
#include "tstring.h"

union DBSTRING
	{
	struct
		{
		short	blockIndex;
		WORD	offset;
		} 								hString;

	LPSTR								pString;


	void far * operator new(
		size_t	size)
		{
		return NULL;
		}

	void far * operator new(
		size_t			size,
		void far*		objPtr)
		{
		return objPtr;
		}

	void operator delete(
		void far *	objptr)
		{
		}

	DBSTRING(void)
		{
		hString.blockIndex = -1;
		}

	DBSTRING(
		void far*	objPtr)
		{
		}

	DBSTRING(
		const DBSTRING far& srcString);

	DBSTRING(
		char far*		srcString);

	DBSTRING(
		TSTRING	far&	srcString);

	DBSTRING(
		int	hFile);

	~DBSTRING(void);

	DBSTRING far& operator =(
		DBSTRING far&		srcString);

	DBSTRING far& operator =(
		TSTRING far&		srcString);

	DBSTRING far& operator =(
		LPSTR		srcString);

	void InitDBStringMemory(
		void);

	void FreeDBStringMemory(
		void);

	short strlen(
		void);

	operator char* (
		void);

	LPSTR	pGetPointer(
		void)
		{
		return pString;
		}

	void ConvertToPointers(
		void);

	};

#endif
