/*//////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
//  Written by DUNYE ALEXANDER, ECO High Tech Group, March 1994
//
//  Fixes:
//	ecj01		fix for the qtw path not defined problem.
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT

#include <windows.h>
#include <windowsx.h>

#include <a_alm.h>

#include "qtw.h"
#include "movieexp.h"
#include "moviedat.h"

#ifdef __BORLANDC__
extern int _WinAllocFlag;
#endif

HINSTANCE   hInstance;
BOOL QTInstall;

char szMovWndName[]="MovieWindow";
char szMovVIWndName[]="MovieVIWindow";
char szMovFrameWndName[]="MovieFrameWindow";

//////////////////////////////////////////////////////////////////////////////////
//  Function:
//    MovWndProc
//  Description:
//    Procedure of the Movie Window that must be created in the Movie Dialog Box
//////////////////////////////////////////////////////////////////////////////////
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/  MovWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);

//////////////////////////////////////////////////////////////////////////////////
//  Function:
//    MovVIWndProc
//  Description:
//    Procedure of the Movie Window that must be created in the Movie Dialog Box
//////////////////////////////////////////////////////////////////////////////////
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/  MovVIWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);

//////////////////////////////////////////////////////////////////////////////////
//  Function:
//    MovFrameWndProc
//  Description:
//    Procedure of the Movie Window that must be created in the Movie Dialog Box
//////////////////////////////////////////////////////////////////////////////////
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/  MovFrameWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);

int CALLBACK LibMain/*!!PORT!! Replace with DllEntryPoint */(
  HINSTANCE   hIns,
  WORD        wDataSeg,
  WORD        cbHeapSize,
  LPSTR       lpCmdLine)
  {
  I_UNUSED(wDataSeg);
  I_UNUSED(cbHeapSize);
  I_UNUSED(lpCmdLine);

#ifdef __BORLANDC__
  _WinAllocFlag = GMEM_SHARE;
#endif

  long lVersion;

  WNDCLASS wndclass;

// Establish links to QuickTime for Windows
   if (((UINT)QTInitialize (&lVersion) != QTI_OK) || (lVersion < 0x00001000L))
   {
     QTInstall = FALSE;
/* ecj01 : this is a temp fix, to avoid hanging or killing the system when the path for qtw is not defined. */
     MessageBox (GetActiveWindow(), 
                 "QuickTime failed to initialize.  Either you do not have the correct "
                 "version of QuickTime or the executables are not in your path "
                 "(usually c:\\qtw\\bin).  The QuickTime Movie ALM cannot be loaded.",
                 "Error", MB_ICONEXCLAMATION);
     return 0;  // stop loading the DLL.
/* ecj01 */
   }
   else
     QTInstall = TRUE;                     

  hInstance = hIns;

  wndclass.style = CS_DBLCLKS;
  wndclass.lpfnWndProc = MovWndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = sizeof(long);
  wndclass.hInstance = hInstance;
  wndclass.hIcon = NULL;
  wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
  wndclass.hbrBackground =(HBRUSH) (COLOR_WINDOW + 1);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = szMovWndName;

  if(!RegisterClass(&wndclass))
    return FALSE;

  wndclass.lpfnWndProc = MovVIWndProc;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = NULL;
  wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
  wndclass.hbrBackground =NULL;
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = szMovVIWndName;

  if(!RegisterClass(&wndclass))
    return FALSE;

  wndclass.style = CS_DBLCLKS;
  wndclass.lpfnWndProc = MovFrameWndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = sizeof(long);
  wndclass.hInstance = hInstance;
  wndclass.hIcon = NULL;
  wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
  wndclass.hbrBackground =(HBRUSH) (COLOR_WINDOW + 1);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = szMovFrameWndName;
  if(!RegisterClass(&wndclass))
    return FALSE;

  return TRUE;
  }

extern "C"
int CALLBACK /*!!PORT!! was _far_ _pascal_*/  WEP/*!!PORT!! Replace with DllEntryPoint */(int nParameter)
  {
// Cut the connections to QuickTime for Windows

   I_UNUSED(nParameter);

   if (QTInstall)
   {
     ExitMovies ();
     QTTerminate ();
     QTInstall = FALSE;
   }
   return 1;
  }
