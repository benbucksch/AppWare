#ifndef __FUNCDEF_H__
#define __FUNCDEF_H__

#include "memclass.h"
#include "serstrng.h"
#include "serblk.h"

class FUNCTIONDEF:public MEMORY
	{
public:

	short 				fileIndex;
	DWORD					typeID;
	DWORD					functionID;
	DWORD 				methodID;
	STRING				functionName;
	STRING				syntax;
	LPSERIUSBLOCK icon;
	STRING				comment;
	LPSERIUSBLOCK flows;
	LPSERIUSBLOCK	inputs;
	LPSERIUSBLOCK outputs;
	DWORD					flags;
	LPSERIUSBLOCK unused;
	HICON					hIcon;

	STRING GetName(void);

	HICON	GetIcon(void);

	};

typedef FUNCTIONDEF far* LPFUNCTIONDEF;

#endif
