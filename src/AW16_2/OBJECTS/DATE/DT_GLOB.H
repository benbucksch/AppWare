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
// FILE: dt_glob.h
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Global variables used by Date object
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#ifndef DT_GLOB_H
#define DT_GLOB_H

#define BLANKOUT     TRUE
#define POSTSIGNAL   TRUE
#define DONTPOSTSGN  FALSE
#define DONTBLANKOUT FALSE

#define DAY_MONTH_STRING_LEN		20 // Max. no of the month string

#ifdef  DT_MAIN
HINSTANCE				g_hInstance;
char						g_sDateProfileString[20];
char						g_lDateProfileString[100];
char            g_lMonthNames[12][DAY_MONTH_STRING_LEN];
char            g_sMonthNames[12][DAY_MONTH_STRING_LEN];
char            g_lDayNames[7][DAY_MONTH_STRING_LEN];
char            g_sDayNames[7][DAY_MONTH_STRING_LEN];
WNDPROC					g_defEditProc = NULL;
int 						g_iThisPointerOffset;
LPSTR			 		  g_dateObjectClassName = "AppwareDate";
LPSTR           g_szEmpty="\0";
#else
extern HINSTANCE g_hInstance;
extern char			 g_sDateProfileString[];
extern char			 g_lDateProfileString[];
extern char      g_lMonthNames[][DAY_MONTH_STRING_LEN];
extern char      g_sMonthNames[][DAY_MONTH_STRING_LEN];
extern char      g_lDayNames[][DAY_MONTH_STRING_LEN];
extern char      g_sDayNames[][DAY_MONTH_STRING_LEN];
extern LPSTR		 g_dateObjectClassName;
extern WNDPROC	 g_defEditProc;
extern int 			 g_iThisPointerOffset;
extern LPSTR     g_szEmpty;

#endif

#endif