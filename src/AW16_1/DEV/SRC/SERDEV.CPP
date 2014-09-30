#include "almsys.h"
#include "devsys.h"

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
int PASCAL WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			cmdShow)
	{
	int result;

	result = DEVSYSTEM::Initialize(hInstance, hPrevInstance, lpCmdLine,
	 cmdShow);

	DEVSYSTEM::Cleanup();

	return result;

	}
