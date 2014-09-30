#define TM_MAIN

#include "tm_incld.h"

extern int _WinAllocFlag;

void RegisterEditSuperClass();


extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			g_hInstance = (HINSTANCE)hInstDLL;
	
			RegisterEditSuperClass();

			GetProfileString("intl", "s2359", "PM", g_pmProfileSting, sizeof(g_pmProfileSting) );
			GetProfileString("intl", "s1159", "AM", g_amProfileSting, sizeof(g_amProfileSting) );

			GetProfileString("intl", "sTime", ":", g_timeSepProfileChar, sizeof(g_timeSepProfileChar) );

			g_leadingZeroProfileBool = (BOOL)GetProfileInt("intl", "iTLZero", TRUE);
			g_is24hourProfileBool = (BOOL)GetProfileInt("intl", "iTime", 0);
			break;

		case DLL_PROCESS_DETACH:
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


extern "C" int CALLBACK /*!!PORT!! was _far_ _pascal_*/  WEP/*!!PORT!! Replace with DllEntryPoint */(int	nParameter)
{
	I_UNUSED(nParameter);

	UnregisterClass(g_timeObjectClassName, g_hInstance);
	return 1;
}


static void RegisterEditSuperClass()
{
	WNDCLASS	wc;

	GetClassInfo(NULL, "Edit", &wc);

	g_iThisPointerOffset = wc.cbWndExtra;
	g_defEditProc = wc.lpfnWndProc;

	wc.cbWndExtra += sizeof(LPVOID);
	wc.lpfnWndProc = AppwareTimeObjectProc;
  wc.hInstance = g_hInstance;
	wc.lpszClassName = g_timeObjectClassName;

  RegisterClass(&wc);
}

