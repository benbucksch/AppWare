#ifndef __FLOWS_H
#define __FLOWS_H

#include "array.h"
#include "dbstring.h"

struct FLOWDESC
	{
	DBSTRING		name;
	DBSTRING		help;
	DWORD				flags;

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

	FLOWDESC(void);

	FLOWDESC(
		LPSTR	name,
		LPSTR	help);

	~FLOWDESC(void);

	FLOWDESC & operator =(
		FLOWDESC &		srcFlow);

	void ConvertToPointers(
		void);

	};



class FLOWDESCARRAY:public SERIUSARRAY
	{

public:

	FLOWDESCARRAY(void);

	FLOWDESCARRAY(
		unsigned short	count);

	FLOWDESCARRAY(
		const FLOWDESCARRAY &	srcArray);

	FLOWDESCARRAY(
		void *	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	FLOWDESC & operator[](
		unsigned short	index
    ) const;

	BOOL Resize(
		unsigned short newCount);

	BOOL Insert(
		unsigned short	index,
		LPSTR 				 	name,
		LPSTR						help);

	BOOL Delete(
		unsigned short index);

	FLOWDESCARRAY & operator =(
		FLOWDESCARRAY &	srcArray);

	void ConvertToPointers(
		void);

	};

#endif
