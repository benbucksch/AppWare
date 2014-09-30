/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					SHAPEUTL.H
//
// AUTHOR:				Scott McCarty (extracted from old Window Object code)
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SHAPEUTL_H
#define SHAPEUTL_H

#define DOT_SIZE    6

void  MakeRealRect(LPRECT target, LPRECT source);
void  DrawFrame(HDC hdc, RECT r);
BOOL  PtOnLine(RECT rcLine, POINT pt, int leeway);
BOOL  RectAndLineIntersect(RECT rect, RECT line, int leeway);
void  CreateCenteredGrabHandle(LPRECT lprcHandle, const POINT* lpptRef, const POINT* lpptSize);
void  CreateGrabHandle(int nGrabHandle, LPRECT lprcHandle, const RECT* lprcItem, const POINT* lpptSize);
BOOL  PointInEllipse(LPPOINT lppt, LPRECT lprcEllipse);

#endif	// !SHAPEUTL_H
