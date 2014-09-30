#ifndef __PARMDESC_H__
#define __PARMDESC_H__

#include "classdef.h"
#include "compuid.h"
#include "dbstring.h"
#include "array.h"

#define	PARAM_OPTIONAL		         0X0001
#define	PARAM_NOTEMPOBJECTS        0X0002

struct APARAMDESC
	{
	DBSTRING		name;
	DBSTRING		help;
	COMPUID			type;
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

	APARAMDESC(void);

	APARAMDESC(
		LPSTR 		theName,
		LPSTR			theType,
		COMPUID   theTypeID,
		DWORD			flags);

	~APARAMDESC(void);

	APARAMDESC & operator =(
		APARAMDESC & 	srcParam);

	LPCSTR pGetName(
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

class PARAMDESCARRAY:public SERIUSARRAY
	{

public:

	PARAMDESCARRAY(void);

	PARAMDESCARRAY(
		unsigned short	count);

	PARAMDESCARRAY(
		const PARAMDESCARRAY &	srcArray);

	PARAMDESCARRAY(
		void *	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	APARAMDESC & operator[](
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

	PARAMDESCARRAY & operator =(
		PARAMDESCARRAY &	srcArray);

	void ConvertToPointers(
		void);

	};

#endif
