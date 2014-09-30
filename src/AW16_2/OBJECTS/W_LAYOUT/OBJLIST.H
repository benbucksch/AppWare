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
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_H
#define OBJLIST_H

// required includes ///////////////////////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

// forward class definition ////////////////////////////////////////////////

class Layout;

// defines for sizes ///////////////////////////////////////////////////////

#define ALIGNMENT_WIDTH       49
#define TOOL_WIDTH            73
#define TOOL_HEIGHT           193
#define OBJLIST_WIDTH         (ALIGNMENT_WIDTH+TOOL_WIDTH)
#define OBJLIST_HEIGHT        (GetSystemMetrics(SM_CYSCREEN)-TOOL_HEIGHT-GetSystemMetrics(SM_CYCAPTION))
#define DROP_OFFSETX          15
#define DROP_OFFSETY          15

#define ITEM_DEFAULT_WIDTH    300
#define ITEM_DEFAULT_HEIGHT   100

// function prototypes for Object List interface //////////////////////////

BOOL    RegisterObjectPalette();
HWND    InitObjectListPalette(HWND hwndOwner, Layout* pLayout, BOOL fPrstPos);
void    SetObjectListDragTarget(HWND hDragTarget);
void    SetObjectListDragIgnore(HWND hDragIgnore);
void    RemoveObjectListDragIgnore(HWND hDragIgnore);
void    DestroyObjectListWindow(BOOL fPrstPos);
void    EnableObjectListWindow(BOOL fEnable);
void		ReloadObjectListWindow(void);

////////////////////////////////////////////////////////////////////////////

#endif  // !OBJLIST_H
