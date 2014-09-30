#ifndef __MEMBLK_H__
#define __MEMBLK_H__

#include "classdef.h"
#include "baseblk.h"
#include "rtrec.h"

class MEMBLOCK:public BASEBLOCK
	{

public:

	MEMBLOCK(void);

	MEMBLOCK(
		long int	count);

	~MEMBLOCK(void)
    {
    }

	};


#endif
