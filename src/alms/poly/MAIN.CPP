#define I_MAIN
#include "include.h"
#undef I_MAIN
										

///////////////////////////////////////////////
///////////////////////////////////////////////

extern "C" BOOL WINAPI DllEntryPoint(
	HINSTANCE								hInstDLL,
	DWORD										dwReason,
	LPVOID									pReserved)
	{
	I_UNUSED(pReserved);

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH: 
			g_hInstance = hInstDLL;
			EDIT_POLY::RegisterWindowClasses();
			break;

		case DLL_PROCESS_DETACH:
			EDIT_POLY::UnregisterWindowClasses();
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		}

	return TRUE;
	}

