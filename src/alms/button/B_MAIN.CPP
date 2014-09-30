#define I_MAIN
#include "b_includ.h"
#undef I_MAIN

extern int _WinAllocFlag;

static void RegisterButtonSuperClass();

extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	g_hInstance = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			RegisterButtonSuperClass();
			break;

		case DLL_PROCESS_DETACH:
			UnregisterClass(g_szButtonClassName, g_hInstance);
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
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

