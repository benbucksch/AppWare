//
// main.cpp -- OIP.DLL main file
//

#include "almsys.h"
#include "intdebug.h"

#ifdef __BORLANDC__
#include <bwcc.h>
#endif

#include "system.h"
#include "rsrcmgr.h"
#include "heap.h"

class SYSTEM;
SER_WINRESOURCES *SYSTEM::serWResources;

SYSTEMFULL		theSystem;
SYSTEM*				pTheSystem		= &theSystem;

HANDLE  hDLL;

extern int _WinAllocFlag;

#ifdef __CPLUSPLUS
extern "C" {
#endif

//[ LibMain ]=========================================================
//
// Every Windows DLL has a LibMain entry point.  To work with the
// proto-shell, the Object Type retains the handle to the instance
// of the DLL.
//
//====================================================================

BOOL FAR PASCAL LibMain

    ( HINSTANCE   hInstance,
      WORD        dataSegment,
      WORD        heapSize,
      LPSTR       lpszCmdLine )

  {
	I_UNUSED(dataSegment);
	I_UNUSED(lpszCmdLine);

#ifdef __BORLANDC__
 // thanks Borland...
  _WinAllocFlag = GMEM_SHARE;
#endif

  hDLL = hInstance;
  if (heapSize > 0)
    UnlockData(0);


	// Instantiate REsource Manager
	// this is a static member of system class and is available till the
	// ser_oip.dll is unloaded i.e till the last ser application is closed

	pTheSystem->serWResources = new SER_WINRESOURCES();

#ifdef __BORLANDC__
	// Initialize the Borland custom controls
	BWCCGetVersion();
#endif

  return TRUE;

  }

//[ WEP ]=============================================================
//
// Every Windows DLL has a WEP (Windows Exit Procedure).  It's best
// to do as little as possible in the WEP...
//
//====================================================================
BOOL FAR PASCAL _export WEP(BOOL isSystemExit)
	{
	I_UNUSED(isSystemExit);

	delete pTheSystem->serWResources;

  return TRUE;
  }

#ifdef __CPLUSPLUS
}; // extern "C"
#endif

////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////

void*	operator new(
	size_t	s
	)
	{
	return MemAlloc(s);
	}

void	operator delete(
	void*		p
	)
	{
	MemFree(p);
	}
