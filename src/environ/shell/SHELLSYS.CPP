#include "almsys.h"
#include "shellsys.h"


///////////////////////////////////////////////////////////////////////////
//	Allocate and initialize SHELLSYSTEM class static data  members
///////////////////////////////////////////////////////////////////////////

SYSTEMMIN		theSystem;
SYSTEM*			pTheSystem		= &theSystem;

HINSTANCE	 SHELLSYSTEM::hInstance;
HINSTANCE	 SHELLSYSTEM::hPrevInstance;
LPSTR			 SHELLSYSTEM::cmdLine;


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Initialize
//
// CLASS.......: 	SHELLSYSTEM	(static)
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int SHELLSYSTEM::Initialize(
	HINSTANCE	hInst,
	HINSTANCE	/*hPrevInstance*/,
	LPSTR		lpCmdLine,
	int			/*cmdShow*/)
	{
	cmdLine = lpCmdLine;
	pTheSystem->Initialize();
	hInstance = hInst;

  return 0;

	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Cleanup
//
// CLASS.......: 	SHELLSYSTEM	(static)
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void SHELLSYSTEM::Cleanup(
	void)
	{

	pTheSystem->CleanUp();

	};

