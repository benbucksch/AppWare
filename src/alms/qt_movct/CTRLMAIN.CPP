/*//////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
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
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <windowsx.h>

#include <a_alm.h>

#include "qtw.h"
#include "movctrl.h"

#ifdef __BORLANDC__
extern int _WinAllocFlag;
#endif

HINSTANCE   hInstance;
BOOL QTInstall;
//char QTDrive;

char szCtrlVIWndName[]="ControllerVIWindow";
//It is necessary to open movie and file and load movie to draw controller properly
MovieFile mfMovie;

long CALLBACK /*!!PORT!! was _far_ _pascal_*/  CtrlBoxProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);

char FindDrive(const char *envstr, const char *str)
{
  char *ptr, *s;
  char *d;

   /* get the current path environment */
   ptr = GetEnvironmentString(envstr);

   s = strstr(ptr,str);
   if (s == 0)
   {
     return *ptr;
   }

   if (((s-1) == ":") && isalpha((int)s-2))
     return *(s-2);

   d = "C";
   return *d;
}
////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////

int CALLBACK LibMain/*!!PORT!! Replace with DllEntryPoint */(
  HINSTANCE   hIns,
  WORD        wDataSeg,
  WORD        cbHeapSize,
  LPSTR       lpCmdLine)
  {
  I_UNUSED(wDataSeg);
  I_UNUSED(cbHeapSize);
  I_UNUSED(lpCmdLine);

WNDCLASS wndclass;
long lVersion;

#ifdef __BORLANDC__
  _WinAllocFlag = GMEM_SHARE;
#endif

  hInstance = hIns;

  wndclass.style = CS_DBLCLKS;
  wndclass.lpfnWndProc = CtrlBoxProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 256;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = NULL;
  wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
  wndclass.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = "ControlBox";

  if (!RegisterClass(&wndclass))
    return 0;

// Establish links to QuickTime for Windows

//   QTDrive = FindDrive("PATH","\\QTW");

   if (QTInitialize (&lVersion) != QTI_OK)
   {
     QTInstall = FALSE;
   }
   else
     QTInstall = TRUE;

  return TRUE;
  }

int CALLBACK /*!!PORT!! was _far_ _pascal_*/  WEP/*!!PORT!! Replace with DllEntryPoint */(void)
  {
// Cut the connections to QuickTime for Windows
   return 1;
  }
