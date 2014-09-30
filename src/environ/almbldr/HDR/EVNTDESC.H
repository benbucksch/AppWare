#ifndef __EVNTDESC_H__
#define __EVNTDESC_H__

#include "compuid.h"
#include "compdesc.h"

class AEVENTDESC:public ACOMPDESC
	{
private:
	COMPUID  	typeID;

public:

	AEVENTDESC(
		void)
		{
		}

	AEVENTDESC(
		void *	objPtr):ACOMPDESC(objPtr)
		{
		}

	AEVENTDESC(
		COMPUID	id,
		TSTRING 	name);

	BOOL SetTypeID(
		COMPUID	newTypeID)
		{
		typeID = newTypeID;
		return TRUE;
		}

	COMPUID	GetTypeID(
		void)
		{
		return typeID;
		}

	};

#endif
