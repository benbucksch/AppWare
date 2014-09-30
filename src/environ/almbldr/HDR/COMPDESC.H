#ifndef __COMPDESC_H__
#define __COMPDESC_H__

#include "serrec.h"
#include "tstring.h"
#include "compuid.h"
#include "dbstring.h"

class ACOMPDESC:public SERIUSRECORD
	{

private:
	COMPUID			UID;
	DBSTRING		name;

public:

	ACOMPDESC(
		void)
		{
		}

	ACOMPDESC(
		void *	objPtr):name(objPtr)
		{
		}

	ACOMPDESC(
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
