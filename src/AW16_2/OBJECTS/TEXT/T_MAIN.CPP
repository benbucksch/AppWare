
#define I_MAIN

#include "t_includ.h"

extern int _WinAllocFlag;
static HINSTANCE g_hInstResizer = NULL;


void RegisterEditSuperClass();

extern "C" int far pascal LibMain (
	HINSTANCE								hIns,
	WORD 										wDataSeg,
	WORD 										cbHeapSize,
	LPSTR										lpCmdLine)
	{
	I_UNUSED(wDataSeg);
	I_UNUSED(cbHeapSize);
	I_UNUSED(lpCmdLine);

	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = (HINSTANCE)hIns;

	RegisterEditSuperClass();

	if (!g_hInstResizer)
		g_hInstResizer = LoadLibrary("resizer.dll");


	return TRUE;
	}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

extern "C" int far pascal _export WEP(
	int											nParameter)
	{
	I_UNUSED(nParameter);

	UnregisterClass(g_textObjectClassName, g_hInstance);

	if (g_hInstResizer)
		{
		FreeLibrary(g_hInstResizer);
		g_hInstResizer = NULL;
		}


	return 1;
  }


static void RegisterEditSuperClass()
	{
	WNDCLASS								wc;

	GetClassInfo(NULL, "Edit", &wc);

	g_iThisPointerOffset = wc.cbWndExtra;
	g_defEditProc = wc.lpfnWndProc;

	wc.cbWndExtra += sizeof(LPVOID);
	wc.lpfnWndProc = AppwareTextObjectProc;
  wc.hInstance = g_hInstance;
	wc.lpszClassName = g_textObjectClassName;

  RegisterClass(&wc);
	}

