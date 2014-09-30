#ifndef LISTMBR_H
#define LISTMBR_H

#include "memclass.h"

class LIST;

class LISTMEMBER:public MEMORY
	{
	friend LIST;

	private:
		LISTMEMBER far* nextMember;
		LISTMEMBER far* prevMember;
	};

typedef LISTMEMBER far* LPLISTMEMBER;

#endif