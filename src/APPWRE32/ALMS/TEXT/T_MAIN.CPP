
#define I_MAIN

#include "t_includ.h"

static HINSTANCE g_hInstResizer = NULL;


void RegisterEditSuperClass();

extern "C" BOOL WINAPI DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved)
	{
	I_UNUSED(pReserved);

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			g_hInstance = hInstDLL;

			RegisterEditSuperClass();
			if (!g_hInstResizer)
				g_hInstResizer = LoadLibrary("RESIZE32.dll");

			break;

		case DLL_PROCESS_DETACH:
			UnregisterClass(g_textObjectClassName, g_hInstance);
			if (g_hInstResizer)
				{
				FreeLibrary(g_hInstResizer);
				g_hInstResizer = NULL;
				}

			break;
		}

	return TRUE;
	}

///////////////////////////////////////////////

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



