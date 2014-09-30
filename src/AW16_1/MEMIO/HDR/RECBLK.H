#ifndef __RECBLK_H__
#define __RECBLK_H__

#include <memory.h>
#include "seruid.h"
#include "classdef.h"
#include "baserecb.h"


class RECORDBLOCK:public BASERECORDBLOCK
	{

public:

	RECORDBLOCK(void);

	RECORDBLOCK(
		long int  recordSize,
		long int	type);

  RECORDBLOCK(
	  long int  	recordSize,
	  long int		type,
    long int    recordCount);
	};

#endif
