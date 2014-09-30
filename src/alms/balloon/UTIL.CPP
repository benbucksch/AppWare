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
// FILE:          "util.cpp"
//
// AUTHOR:        Lee E. Thayer
//
// DESCRIPTION:   Utility routines for balloon support
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>

#include "server.h"



//****************************************************************
//
//	Balloon Class Access Routines:
//
//		GetBalloon
//		StoreBalloon - 	Stores the class pointer in the balloon 
//						window's extra bytes area.  Doing this, if
//						we have the balloon's window handle, we can
//						always retrieve the balloon class and 
//						access its functions and its public and 
//						protected data (but not private data).
//
//****************************************************************
Balloon* GetBalloon(HWND hwnd)
{
	return((Balloon*)GetWindowLong(hwnd, 0));
}


void StoreBalloon(HWND hwnd, Balloon *balloonPtr)
{
	SetWindowLong(hwnd, 0, (LONG)balloonPtr);
}

