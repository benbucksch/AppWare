#ifndef __FLOWS_H
#define __FLOWS_H

#include "dbstring.h"
#include "array.h"

struct FLOWDESC
	{
	DBSTRING		name;
	DBSTRING		help;
	DWORD				flags;

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

	FLOWDESC(void);

	FLOWDESC(
		LPSTR	name,
		LPSTR	help);

	~FLOWDESC(void);

	FLOWDESC far& operator =(
		FLOWDESC far&		srcFlow);

	void ConvertToPointers(
		void);

	};

typedef FLOWDESC far*		LPFLOWDESC;



class FLOWDESCARRAY:public SERIUSARRAY
	{

public:

	FLOWDESCARRAY(void);

	FLOWDESCARRAY(
		unsigned short	count);

	FLOWDESCARRAY(
		const FLOWDESCARRAY far&	srcArray);

	FLOWDESCARRAY(
		void far*	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	FLOWDESC far& operator[](
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

	FLOWDESCARRAY far& operator =(
		FLOWDESCARRAY far&	srcArray);

	void ConvertToPointers(
		void);

	};

typedef FLOWDESCARRAY far*	LPFLOWDESCARRAY;

#endif
