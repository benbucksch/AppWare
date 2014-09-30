//
// main.cpp -- OIP.DLL main file
//

#include <windows.h>

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

//defined in serdlg.cpp
extern int	cbOffset ;

//used for serdlg.cpp
#define ADDITIONALBYTES 4

#pragma argsused
BOOL FAR PASCAL _export LibMain

    ( HANDLE      hInstance,
      WORD        dataSegment,
      WORD        heapSize,
      LPSTR       lpszCmdLine )

  {
 // thanks Borland...
  _WinAllocFlag = GMEM_SHARE;

//  hDLL = hInstance;
  if (heapSize > 0)
    UnlockData(0);

  return TRUE;

  }

//[ WEP ]=============================================================
//
// Every Windows DLL has a WEP (Windows Exit Procedure).  It's best
// to do as little as possible in the WEP...
//
//====================================================================
#pragma argsused
BOOL FAR PASCAL _export WEP(BOOL isSystemExit)
	{

  return TRUE;

  }

#ifdef __CPLUSPLUS
}; // extern "C"
#endif
