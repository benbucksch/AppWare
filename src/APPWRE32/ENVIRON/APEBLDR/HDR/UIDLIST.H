#ifndef __UIDLIST_H
#define __UIDLIST_H

#include "seruid.h"
#include "listmbr.h"
#include "memclass.h"

class UIDLIST:public LISTMEMBER, public MEMORY
	{
	public:
		SERUID uid;

		UIDLIST(SERUID newUID);

	};


#endif
