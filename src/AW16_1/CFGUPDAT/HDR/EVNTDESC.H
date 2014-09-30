#ifndef __EVNTDESC_H__
#define __EVNTDESC_H__

#include "compdesc.h"

class EVENTDESC:public COMPDESC
	{
private:
	COMPUID  	typeID;

public:

	EVENTDESC(
		void)
		{
		}

	EVENTDESC(
		void far*	objPtr):COMPDESC(objPtr)
		{
		}

	EVENTDESC(
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

typedef EVENTDESC far*	LPEVENTDESC;

#endif