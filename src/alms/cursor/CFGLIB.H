#ifndef __cfglib_h__
#define __cfglib_h__

#include "objdef.h"
#include "funcdef.h"


BOOL cfgReadConfig(
	void);

LPOBJECTDEF cfgGetNextObjDef(
	short &i);

LPFUNCTIONDEF cfgGetNextFuncDef(
	DWORD objectID,
	short	&i);

#endif

