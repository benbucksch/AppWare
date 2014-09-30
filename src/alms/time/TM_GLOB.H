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
// FILE: tm_glob.h
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Global variables used by Time object
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#ifndef TM_GLOB_H
#define TM_GLOB_H

#define BLANKOUT     TRUE
#define POSTSIGNAL   TRUE
#define DONTPOSTSGN  FALSE
#define DONTBLANKOUT FALSE

#ifdef  TM_MAIN
HINSTANCE				  g_hInstance;
char						  g_pmProfileSting[20];
char						  g_amProfileSting[20];
char						  g_timeSepProfileChar[2];
BOOL						  g_is24hourProfileBool;
BOOL						  g_leadingZeroProfileBool;
WNDPROC					  g_defEditProc = NULL;
int 						  g_iThisPointerOffset;
LPSTR			 		    g_timeObjectClassName = "AppwareTime";
LPSTR             g_szEmpty="\0";
#else
extern HINSTANCE  g_hInstance;
extern char			  g_pmProfileSting[];
extern char			  g_amProfileSting[];
extern char			  g_timeSepProfileChar[];
extern BOOL			  g_is24hourProfileBool;
extern BOOL			  g_leadingZeroProfileBool;
extern LPSTR		  g_timeObjectClassName;
extern WNDPROC	  g_defEditProc;
extern int 			  g_iThisPointerOffset;
extern LPSTR      g_szEmpty;

#endif

#endif
