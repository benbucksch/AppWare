#ifndef __NEWCOMP_H
#define __NEWCOMP_H

#include "tstring.h"
#include "compuid.h"

struct	NEWCOMPDATA
	{
	TSTRING	compName;
	COMPUID compUID;
	};

typedef NEWCOMPDATA *	LPNEWCOMPDATA;

BOOL NewCompDlgBox(
	HINSTANCE			hAppInstance,
	HWND					hWndParent,
	TSTRING				defaultName,
	TSTRING				typeName,
	long					compType,
	LPNEWCOMPDATA	compData);

#endif
