
#ifndef __EDCFGDLG_H
#define __EDCFGDLG_H

#include "classdef.h"

struct	EDITCONFIGDATA
	{
	LPFILEOBJECT	fileObject;
	LPMDIAPP			mdiApp;
	};

typedef EDITCONFIGDATA FAR*	LPEDITCONFIGDATA;

BOOL EditConfigDlgBox(
	HINSTANCE					hAppInstance,
	HWND							hWndParent,
	LPEDITCONFIGDATA	editConfigData);

#endif
