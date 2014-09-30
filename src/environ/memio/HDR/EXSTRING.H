#ifndef __EXSTRING_H
#define __EXSTRING_H

union EXESTRING
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

	EXESTRING(void)
		{
		hString.blockIndex = -1;
		}

	EXESTRING(
		void *	objPtr)
		{
		}

	EXESTRING(
		const EXESTRING & srcString);

	EXESTRING(
		const char*		srcString);

	EXESTRING(
		TSTRING	&	srcString);

	~EXESTRING(void);

	EXESTRING & operator =(
		EXESTRING &  srcString);

	EXESTRING & operator =(
		const char*  srcString);

	void InitEXEStringMemory(
		void);

	void FreeEXEStringMemory(
		void);

	int strlen(
		void);

	operator const char*(
		void);

	void ConvertToPointers(
		void);

	};

#endif
