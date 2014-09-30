#ifndef __EXSERBLK_H__
#define __EXSERBLK_H__

#include "classdef.h"
#include "baseblk.h"
#include "rtrec.h"

class EXESERIUSBLOCK:public BASEBLOCK
	{

public:

	EXESERIUSBLOCK(void);

	EXESERIUSBLOCK(
		long int	count);

	~EXESERIUSBLOCK(void)
    {
    }

	};


#endif
