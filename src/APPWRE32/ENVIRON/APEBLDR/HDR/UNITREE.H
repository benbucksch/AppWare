#ifndef __UNITREE_H
#define __UNITREE_H

#include "bitree.h"

class UNIQUETREE:public BITREE
	{
	private:

	 	LPBITREEMEMBER * GetInsertPosition(
			LPBITREEMEMBER child);

	};

typedef BITREEMEMBER UNITREEMEMBER;
typedef LPBITREEMEMBER LPUNITREEMEMBER;

#endif
