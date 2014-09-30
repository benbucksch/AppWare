//======================================================================
// appmain.cpp
//
//   DLL management routines.
//
// jon 15-Aug-92 [1.0] created.
//======================================================================

#define STRICT
#define ASTRICT
  #include <windows.h>

 // Borland 3.0 hack
  extern int  _WinAllocFlag;

HANDLE		g_hInstance;

////////////////////////////////////////////////////////////////////////
// LibMain
////////////////////////////////////////////////////////////////////////

         #pragma argsused

extern "C"
         int
         FAR PASCAL

LibMain (HINSTANCE hInst,
         WORD wDataSegment,
         WORD wHeapSize,
         LPSTR lpszCmdLine)
  {
	_WinAllocFlag = GMEM_SHARE;

 // The startup code for the DLL initializes the local heap (if there is one)
 // with a call to LocalInit which locks the data segment.
  if ( wHeapSize != 0 )
	  UnlockData( 0 );

	g_hInstance = hInst;

  return 1;   // Indicate that the DLL was initialized successfully.
}
