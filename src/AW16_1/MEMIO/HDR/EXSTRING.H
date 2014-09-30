#ifndef __EXSTRING_H
#define __EXSTRING_H

#include <memory.h>

union EXESTRING
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

	EXESTRING(void)
		{
		hString.blockIndex = -1;
		}

	EXESTRING(
		void far*	objPtr)
		{
		}

	EXESTRING(
		const EXESTRING far& srcString);

	EXESTRING(
		char far*		srcString);

	EXESTRING(
		TSTRING	far&	srcString);

	EXESTRING(
		int	hFile);

	~EXESTRING(void);

	EXESTRING far& operator =(
		EXESTRING far&  srcString);

	EXESTRING far& operator =(
		LPSTR  srcString);

	void InitEXEStringMemory(
		void);

	void FreeEXEStringMemory(
		void);

	short strlen(
		void);

	operator char far*(
		void);

	void ConvertToPointers(
		void);

	};

#endif
