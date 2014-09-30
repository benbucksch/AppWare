#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "classdef.h"
#include "compuid.h"
#include "dbstring.h"
#include "array.h"

#define MAXDLLCHARS 12

class RESPONSE
	{
private:
	COMPUID		eventID;
	char			dll[MAXDLLCHARS+1];
	DBSTRING	function;

public:

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

	RESPONSE(void);

	RESPONSE(
		COMPUID	eventID,
		LPSTR		dllName,
		LPSTR		functionName);

	RESPONSE(
		void *	objPtr):
			function(&this->function)
		{
		}

	RESPONSE(
		RESPONSE & srcResponse);

	RESPONSE & operator =(
		RESPONSE &		srcResponse);

	COMPUID GetEventID(
		void)
		{
		return eventID;
		}

	LPCSTR GetFunction(
		void)
		{
		return function;
		}

	LPSTR pGetFunction(
		void)
		{
		return function.pString;
		}

	LPSTR GetDll(
		void)
		{
		return dll;
		}

	BOOL SetDll(
		LPSTR	pDll);

	BOOL SetFunction(
		LPSTR pFunction)
		{
		function = pFunction;
		return TRUE;
		}

	BOOL SetEventID(
		COMPUID id)
		{
		eventID = id;
		return TRUE;
		}

	void  ConvertToPointers(
		void);

	};


class RESPONSEARRAY:public SERIUSARRAY
	{

public:

	RESPONSEARRAY(void);

	RESPONSEARRAY(
		unsigned short	count);

	RESPONSEARRAY(
		const RESPONSEARRAY &	srcArray);

	RESPONSEARRAY(
		void *	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	RESPONSE & operator[](
		unsigned short	index
    ) const;

	BOOL Resize(
		unsigned short newCount);

	LPRESPONSE Insert(
	unsigned short	index,
	COMPUID					eventID,
	LPSTR 				 	dll,
	LPSTR						function);

	BOOL Delete(
		unsigned short index);

	RESPONSEARRAY & operator =(
		RESPONSEARRAY &	srcArray);

	LPRESPONSE pGetResponse(
		COMPUID		eventID);

	void ConvertToPointers(
		void);

	};

#endif
