#include "almsys.h"

#include "mdiclass.h"
#include "memlib.h"
#include "cfglib.h"
#include "progsys.h"
#include "system.h"
#include "oipexe.h"

class SYSTEM;
SER_WINRESOURCES *SYSTEM::serWResources;

SYSTEMMIN			theSystem;
SYSTEM*				pTheSystem		= &theSystem;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	WinMain
//
// CLASS.......:  None
//
// DESCRIPTION.:  entry point into program.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			CmdShow)
	{
	int result;
	MDI * mdiPtr;

	//must be initialized before any function calls are made
	PROGSYSTEM::hInst = hInstance;
	PROGSYSTEM::hPrevInst = hPrevInstance;
	PROGSYSTEM::lpCmdParam = lpCmdLine;
	PROGSYSTEM::CmdShow = CmdShow;

	PROGSYSTEM::ShowSplashScreen();

	InitializeBlocks();

	pTheSystem->Initialize();

	cfgReadConfigs();

	// Initialize Programmer system
	result = PROGSYSTEM::InitSystem();

	if(result)
		{
		mdiPtr = new MDI();

		result = mdiPtr->MainLoop();

		delete mdiPtr;
		}


	cfgCleanUp();

	PROGSYSTEM::CleanUpSystem();

	pTheSystem->CleanUp();

	exeUnregisterContext();

	return result;

	}
