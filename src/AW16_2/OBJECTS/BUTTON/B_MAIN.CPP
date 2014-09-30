#define I_MAIN
#include "b_includ.h"
#undef I_MAIN

extern int _WinAllocFlag;

static void RegisterButtonSuperClass();


extern "C" int far pascal LibMain (
	HINSTANCE								hIns,
	WORD 										wDataSeg,
	WORD 										cbHeapSize,
	LPSTR										lpCmdLine)
	{
	I_UNUSED(wDataSeg);I_UNUSED(cbHeapSize);I_UNUSED(lpCmdLine);

	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = (HINSTANCE)hIns;
	
	RegisterButtonSuperClass();

	return TRUE;
	}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

extern "C" int far pascal _export WEP(
	int											nParameter)
	{
	I_UNUSED(nParameter);

	UnregisterClass(g_szButtonClassName, g_hInstance);
	return 1;
  }


static void RegisterButtonSuperClass()
	{
	WNDCLASS								wc;

	GetClassInfo(NULL, "Button", &wc);

	g_iThisPointerOffset = wc.cbWndExtra;
	g_defButtonProc = wc.lpfnWndProc;

	wc.style |= CS_DBLCLKS;
	wc.cbWndExtra += sizeof(LPVOID);
	wc.lpfnWndProc = AppwareButtonObjectProc;
  wc.hInstance = g_hInstance;
	wc.lpszClassName = g_szButtonClassName;

  RegisterClass(&wc);
	}

