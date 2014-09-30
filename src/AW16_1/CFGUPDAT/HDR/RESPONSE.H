#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "compuid.h"
#include "seruid.h"
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

	RESPONSE(void);

	RESPONSE(
		COMPUID	eventID,
		LPSTR		dllName,
		LPSTR		functionName);

	RESPONSE(
		void far*	objPtr):
			function(&this->function)
		{
		}

	RESPONSE(
		RESPONSE far& srcResponse);

	RESPONSE far& operator =(
		RESPONSE far&		srcResponse);

	COMPUID GetEventID(
		void)
		{
		return eventID;
		}

	LPSTR GetFunction(
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

typedef RESPONSE far*	LPRESPONSE;


class RESPONSEARRAY:public SERIUSARRAY
	{

public:

	RESPONSEARRAY(void);

	RESPONSEARRAY(
		unsigned short	count);

	RESPONSEARRAY(
		const RESPONSEARRAY far&	srcArray);

	RESPONSEARRAY(
		void far*	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	RESPONSE far& operator[](
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

	RESPONSEARRAY far& operator =(
		RESPONSEARRAY far&	srcArray);

	LPRESPONSE pGetResponse(
		COMPUID		eventID);

	void ConvertToPointers(
		void);

	};

typedef RESPONSEARRAY far*	LPRESPONSEARRAY;


#endif