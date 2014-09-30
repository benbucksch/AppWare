//======================================================================
// appmain.cpp
//
//   DLL management routines.
//
// jon 15-Aug-92 [1.0] created.
//======================================================================

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include <intdebug.h>

HANDLE		g_hDLL;

////////////////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	g_hDLL = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
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

