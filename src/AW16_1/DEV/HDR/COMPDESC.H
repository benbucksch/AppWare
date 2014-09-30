#ifndef __COMPDESC_H__
#define __COMPDESC_H__

#include "serrec.h"
#include "tstring.h"
#include "compuid.h"
#include "dbstring.h"

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

#endif
