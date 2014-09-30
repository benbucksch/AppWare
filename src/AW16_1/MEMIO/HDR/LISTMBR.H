#ifndef LISTMBR_H
#define LISTMBR_H

#include "classdef.h"

class LISTMEMBER
	{
	friend LIST;

	private:
		LISTMEMBER far* nextMember;
		LISTMEMBER far* prevMember;
	};

#endif
