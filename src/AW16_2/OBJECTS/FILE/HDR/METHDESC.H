#ifndef __METHODDESC_H__
#define __METHODDESC_H__

#include "compdesc.h"
#include "compuid.h"
#include "tstring.h"

#define DLL_NAME_LENGTH 12

struct METHODDESC:public COMPDESC
	{
private:
	short 		fileIndex;
	COMPUID  	typeID;
	char			dllName[DLL_NAME_LENGTH + 1];
	DBSTRING  functionName;

public:

	METHODDESC(
		void)
		{
		}

	METHODDESC(
		void *	objPtr):
			COMPDESC(objPtr),
			functionName(objPtr)
		{
		}

	METHODDESC(
		COMPUID	id,
		TSTRING 	name);

	TSTRING GetFunctionName(
		void);

	LPSTR GetDLLName(
		void);

	COMPUID	GetTypeID(
		void);

	BOOL SetFunctionName(
		LPSTR	newFunctionName);

	BOOL SetDLLName(
		LPSTR newDLLName);

	BOOL SetTypeID(
		COMPUID	newTypeID);

	void ConvertToPointers(
		void);

	};

#endif
