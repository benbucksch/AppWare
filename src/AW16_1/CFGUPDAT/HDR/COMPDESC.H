#ifndef __COMPDESC_H__
#define __COMPDESC_H__

#include "compuid.h"
#include "tstring.h"
#include "dbstring.h"
#include "serrec.h"

class COMPDESC:public SERIUSRECORD
	{

private:
	COMPUID			UID;
	DBSTRING		name;

public:

	COMPDESC(
		void)
		{
		}

	COMPDESC(
		void far*	objPtr):name(objPtr)
		{
		}

	COMPDESC(
		COMPUID	id,
		TSTRING 	name);

	BOOL SetName(
		TSTRING	name);

	TSTRING GetName(
		void);

	LPSTR pGetName(
			void);

	COMPUID GetUID(
		void);
	void SetUID(
		COMPUID);

	void ConvertToPointers(
		void);

	};

typedef COMPDESC far*	LPCOMPDESC;

#endif