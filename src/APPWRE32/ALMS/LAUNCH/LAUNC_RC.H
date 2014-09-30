////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "launc_rc.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Launcher object dialog and resource constants.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAUNC_RC_H
#define LAUNC_RC_H

// Make sure dependencies are included /////////////////////////////////////

#ifndef WINDOWS_H
#include <windows.h>
#ifndef WINDOWS_H
#define WINDOWS_H
#endif // #ifndef WINDOWS_H
#endif // #ifndef WINDOWS_H

// EditLaunch structure ////////////////////////////////////////////////////

#ifndef RCINVOKED
class Launcher;

struct EditLauncher
  {
  Launcher* pLaunch;
  BOOL      fRuntime;
  };

BOOL CALLBACK  LauncherEditDlg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

// useful macros ///////////////////////////////////////////////////////////
  
#ifndef RCINVOKED
#define ID(x)               MAKEINTRESOURCE(x)
#else
#define ID(x)               x
#endif

// Resource constants //////////////////////////////////////////////////////

// icons
#define ICON_LAUNCHER       1000

// dialogs
#define DLG_DESIGNLAUNCH    1100
#define DLG_RUNTIMELAUNCH   1101

// edit controls
#define ED_CMDLINE          101
#define ED_STARTDIR         102

// check boxes
#define XB_RUNMIN           103

// static text
#define ST_OBJECT           201

// push buttons
#define PB_BROWSE           110
#define IDHELP              998

// string resources
#define STR_UNKNOWNERROR    1

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LAUNC_RC_H
