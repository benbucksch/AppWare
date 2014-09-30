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
// FILE:          "balloon.h"
//
// AUTHOR:			Lee Thayer
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef _BALLOON_H
#define _BALLOON_H

#include <windows.h>

#define BALLOON_REQUESTPROMPT	(WM_USER + 101)
#define BALLOON_UPDATEPROMPT	(WM_USER + 102)

#define BALLOON_POINT_X(x)		\
	LOWORD(x)

#define BALLOON_POINT_Y(x)		\
	HIWORD(x)

#ifdef __cplusplus
extern "C" {
#endif

// Exported APIs to be used by balloon clients
int  APIENTRY Balloon_RegisterClient(HWND hwndClient);
int  APIENTRY Balloon_UnregisterClient(HWND hwndClient);
int  APIENTRY Balloon_ShutdownServer();

BOOL APIENTRY Balloon_SuppressPrompts();
BOOL APIENTRY Balloon_ActivatePrompts();

BOOL APIENTRY Balloon_SetPointSize(int nPtSize, BOOL fUpdate);
BOOL APIENTRY Balloon_SetItalics(BOOL fItalic, BOOL fUpdate);
BOOL APIENTRY Balloon_SetUnderline(BOOL fUnderline, BOOL fUpdate);
BOOL APIENTRY Balloon_SetFontWeight(int nWeight, BOOL fUpdate);
BOOL APIENTRY Balloon_SetFont(const char *szFaceName, BOOL fUpdate);
BOOL APIENTRY Balloon_SetTextColor(COLORREF color, BOOL fUpdate);
BOOL APIENTRY Balloon_SetBackgroundColor(COLORREF color, BOOL fUpdate);
BOOL APIENTRY Balloon_SetBorderColor(COLORREF color, BOOL fUpdate);
BOOL APIENTRY Balloon_SetBorderStyle(int nStyle, BOOL fUpdate);
BOOL APIENTRY Balloon_SetBorderWidth(int nWidth, BOOL fUpdate);
BOOL APIENTRY Balloon_SetDelayRate(WORD wRate);

#ifdef __cplusplus
}
#endif


#endif	
