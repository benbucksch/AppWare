/*                                                                        *\
    (c) 1993, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE		: QSQL_RC.H                                               |
 | DESCRIPTION	: Q+E Objects Resource Include File.                      |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit                         |
\*------------------------------------------------------------------------*/
#define STRICT
#include <windows.h>
#include <bwcc.h>


/* ---- Q+E Connection Object dialog box -------------------------------- */
#define IDHELP			998
#define IDD_OBJECT		100

#define IDC_USERNAME		112
#define IDC_PASSWORD		122
#define IDC_PARAMS		132
#define IDD_AUTOCOMMIT		140

/* ---- Q+E SQL Object dialog box --------------------------------------- */
#define IDC_CONNOBJ		212
#define IDC_SQLOBJ		222
#define IDD_VARS		230
#define IDD_OPTIONS		240

/* ---- Variables dialog box -------- */
#define IDL_VARS		312
#define IDD_ADDVAR		320
#define IDD_REMVAR		330
#define IDG_VAR			340
#define IDS_NAME		341
#define IDE_NAME		342
#define IDS_TYPE		351
#define IDC_TYPE		352
#define IDS_OBJ			361
#define IDC_OBJ			362

/* ---- Options dialog box ---------- */
#define IDE_ROWCOUNT		412
#define IDD_TAB			420
#define IDD_COMMA		430
#define IDD_OEM			440
