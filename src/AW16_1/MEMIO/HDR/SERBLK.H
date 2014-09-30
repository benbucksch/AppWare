#ifndef __SERBLK_H__
#define __SERBLK_H__

#include <memory.h>
#include "baseblk.h"

class SERIUSBLOCK:public BASEBLOCK
	{

public:

	SERIUSBLOCK(void);

	SERIUSBLOCK(
		long int	count);

	~SERIUSBLOCK(void)
    {
    }

	};


#endif
