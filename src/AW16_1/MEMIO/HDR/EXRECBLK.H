#ifndef __EXRECBLK_H__
#define __EXRECBLK_H__

#include <memory.h>
#include "seruid.h"
#include "classdef.h"
#include "baserecb.h"


class EXERECORDBLOCK:public BASERECORDBLOCK
	{


public:

	EXERECORDBLOCK(void);

	EXERECORDBLOCK(
		long int  recordSize,
		long int	type);

  EXERECORDBLOCK(
	  long int  	recordSize,
	  long int		type,
    long int    recordCount);

	};

#endif
