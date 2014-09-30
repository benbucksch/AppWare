
#define I_MAIN

#include "n_includ.h"

void RegisterEditSuperClass();


extern "C" BOOL WINAPI DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved)
	{
	char										profileStr[2];

	I_UNUSED(pReserved);

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			g_hInstance = hInstDLL;

			RegisterEditSuperClass();

			*g_szNumberParts = 0;
			LoadString(g_hInstance, I_NUMBER_PARTS, g_szNumberParts, 100);

			GetProfileString("intl", "sCurrency", "$", profileStr, sizeof(profileStr) );
			g_chCurrency = *profileStr;

			GetProfileString("intl", "sThousand", ",", profileStr, sizeof(profileStr) );
			g_chThouSep = *profileStr;

			GetProfileString("intl", "sDecimal", ".", profileStr, sizeof(profileStr) );
			g_chDecSep = *profileStr;

			ValidateNumberFormatString(g_szIntegerNumberFormat, &g_integerNumberFormatInfo, TRUE);
			ValidateNumberFormatString(g_szRealNumberFormat, &g_realNumberFormatInfo, FALSE);
			break;

		case DLL_PROCESS_DETACH:
			UnregisterClass(g_numberObjectClassName, g_hInstance);
			break;
		}

	return TRUE;
	}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////


static void RegisterEditSuperClass()
	{
	WNDCLASS								wc;

	GetClassInfo(NULL, "Edit", &wc);

	g_iThisPointerOffset = wc.cbWndExtra;
	g_defEditProc = wc.lpfnWndProc;

	wc.cbWndExtra += sizeof(LPVOID);
	wc.lpfnWndProc = AppwareNumberObjectProc;
  wc.hInstance = g_hInstance;
	wc.lpszClassName = g_numberObjectClassName;

  RegisterClass(&wc);
	}


