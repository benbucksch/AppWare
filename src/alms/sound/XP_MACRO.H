////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          XP_MACRO.H
//
// AUTHOR:        Kevin Smart
//
// DESCRIPTION:   Contains macros for Win16 and Win32 development
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef XP_MACRO_H
#define XP_MACRO_H

#if !defined(__FLAT__)

#define COORDINATE                              int
#define COPYMEMORY                              memcpy
#define FILLMEMORY(p, by, len)                  memset(p, by, len)
#define EXPORT                                  _export
#define SetBrushOrgEx(hdc, nX, nY, pt)          SetBrushOrg(hdc, nX, nY)
#define SetItemCtlID(hwndCtl, id)               SetWindowWord((HWND)(hwndCtl), GWW_ID, (WORD)(id))
#define GetItemCtlID(hwndCtl)                   (int)GetWindowWord((HWND)(hwndCtl), GWW_ID)
#define EnumThreadWindows                       EnumTaskWindows
#define GetCurrentThreadId                      GetCurrentTask
#define POINTSTOPOINT(pt, lParam)               pt = MAKEPOINT(lParam)
#define GetCurrentProcessId                     (DWORD)GetCurrentTask
#define READFILE(lLen, hfile, p, l)             lLen = _hread(hfile, p, l)
#define WRITEFILE(lLen, hfile, p, l)            lLen = _hwrite(hfile, p, l)
#define SETFILEPOINTER(hfile, l, f)             _llseek(hfile, l, f)
#define CLOSEFILEHANDLE(hfile)                  _lclose(hfile)

#else

#if defined(EXPORT)
#undef EXPORT
#endif  // #if defined(EXPORT)

#define COORDINATE                              LONG
#define COPYMEMORY                              CopyMemory
#define FILLMEMORY(p, by, len)                  FillMemory(p, len, by)
#define EXPORT
#define SetItemCtlID(hwndCtl, id)               SetWindowLong((HWND)(hwndCtl), GWL_ID, (LONG)(id))
#define GetItemCtlID(hwndCtl)                   (int)GetWindowLong((HWND)(hwndCtl), GWL_ID)
#define OWndLayGetMenubarID(oiWnd)              OWndLayGetMenuBarID(oiWnd)
#define READFILE(lLen, hfile, p, l)             ReadFile((HANDLE)hfile, (LPVOID)p, l, (LPDWORD)(&lLen), NULL)
#define WRITEFILE(lLen, hfile, p, l)            WriteFile((HANDLE)hfile, (LPCVOID)p, l, (LPDWORD)(&lLen), NULL)
#define SETFILEPOINTER(hfile, l, f)             SetFilePointer((HANDLE)hfile, l, NULL, f)
#define CLOSEFILEHANDLE(hfile)                  CloseHandle((HANDLE)hfile)

#endif  // #if !defined(__FLAT__)

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef XP_MACRO_H
