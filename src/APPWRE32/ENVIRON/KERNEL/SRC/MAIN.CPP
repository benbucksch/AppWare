//
// main.cpp -- OIP.DLL main file
//

#include "almsys.h"
#include "intdebug.h"
#include "system.h"
#include "rsrcmgr.h"

class SYSTEM;
SER_WINRESOURCES *SYSTEM::serWResources;

SYSTEMFULL		theSystem;
SYSTEM*				pTheSystem		= &theSystem;

HINSTANCE  hDLL;

#ifdef __CPLUSPLUS
extern "C" {
#endif

//[ DllMain ]=========================================================
//
//====================================================================

BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	hDLL = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			// Instantiate Resource Manager
			// this is a static member of system class and is available till the
			// ser_oip.dll is unloaded i.e till the last ser application is closed

			pTheSystem->serWResources = new SER_WINRESOURCES();
			break;

		case DLL_PROCESS_DETACH:
			delete pTheSystem->serWResources;
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

#ifdef __CPLUSPLUS
}; // extern "C"
#endif
