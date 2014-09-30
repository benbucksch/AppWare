#ifndef __PARMDESC_H__
#define __PARMDESC_H__

#include "dbstring.h"
#include "compuid.h"
#include "array.h"

#define	PARAM_OPTIONAL		         0X0001
#define	PARAM_NOTEMPOBJECTS        0X0002

struct PARAMDESC
	{
	DBSTRING		name;
	DBSTRING		help;
	COMPUID			type;
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

	PARAMDESC(void);

	PARAMDESC(
		LPSTR 		theName,
		LPSTR			theType,
		COMPUID   theTypeID,
		DWORD			flags);

	~PARAMDESC(void);

	PARAMDESC far& operator =(
		PARAMDESC far& 	srcParam);

	LPSTR pGetName(
		void)
		{
		return name.pGetPointer();
		}

	void ConvertToPointers(
		void);

	BOOL TestNoTempObjectsFlag()
		{
		return (flags & PARAM_NOTEMPOBJECTS)?TRUE:FALSE;
		}

	BOOL TestOptionalFlag()
		{
		return (flags & PARAM_OPTIONAL)?TRUE:FALSE;
		}

	};

typedef PARAMDESC far*	LPPARAMDESC;

class PARAMDESCARRAY:public SERIUSARRAY
	{

public:

	PARAMDESCARRAY(void);

	PARAMDESCARRAY(
		unsigned short	count);

	PARAMDESCARRAY(
		const PARAMDESCARRAY far&	srcArray);

	PARAMDESCARRAY(
		void far*	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	PARAMDESC far& operator[](
		unsigned short	index
    ) const;

	BOOL Resize(
		unsigned short newCount);

	BOOL Insert(
		unsigned short	index,
		LPSTR 				 	name,
		LPSTR						help,
		COMPUID   			typeID,
		DWORD						flags);

	BOOL Delete(
		unsigned short index);

	PARAMDESCARRAY far& operator =(
		PARAMDESCARRAY far&	srcArray);

	void ConvertToPointers(
		void);

	};

typedef PARAMDESCARRAY far*	LPPARAMDESCARRAY;

#endif