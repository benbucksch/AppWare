#ifndef __SHELLSYS_H__
#define __SHELLSYS_H__

#include "system.h"

class SHELLSYSTEM:public SYSTEM
	{
public:
	static HINSTANCE	 hInstance;
	static HINSTANCE	 hPrevInstance;
	static LPSTR   cmdLine;

	static int Initialize(
		HINSTANCE	hInst,
		HINSTANCE	hPrevInstance,
		LPSTR		lpCmdLine,
		int			CmdShow);

	static void Cleanup(
	 void);

	};

#endif
