#ifndef __OBJDEF_H__
#define __OBJDEF_H__

#include "memclass.h"
#include "serstrng.h"
#include "serblk.h"

class OBJECTDEF:public MEMORY
	{
public:

	short					fileIndex;
	DWORD					objectID;
	STRING				objectName;
	LPSERIUSBLOCK icon;
//	LPOBJECTDEF 	defRoutine;
//	LPOBJECTADM 	adminRoutine;
	LPSERIUSBLOCK responses;
	LPSERIUSBLOCK objectFlags;
	LPSERIUSBLOCK unused;
	char 					dllName[13];
	HICON					hIcon;

	STRING GetName(void);

	DWORD GetObjectID(
		void);

	HICON	GetIcon(void);

	};

typedef OBJECTDEF far*	LPOBJECTDEF;

#endif
