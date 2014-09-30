#define I_MAIN
#include "include.h"
#undef I_MAIN
										
extern int _WinAllocFlag;

extern "C" int far pascal LibMain (
	HINSTANCE								hIns,
	WORD 										wDataSeg,
	WORD 										cbHeapSize,
	LPSTR										lpCmdLine)
	{
	I_UNUSED(wDataSeg);I_UNUSED(cbHeapSize);I_UNUSED(lpCmdLine);

	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = (HINSTANCE)hIns;
	
	EDIT_POLY::RegisterWindowClasses();

	return TRUE;
	}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

extern "C" int far pascal _export WEP(
	int											nParameter)
	{
	I_UNUSED(nParameter);

	EDIT_POLY::UnregisterWindowClasses();
	return 1;
  }


