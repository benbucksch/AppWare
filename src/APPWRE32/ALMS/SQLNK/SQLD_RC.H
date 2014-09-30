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
 | FILE		: SQLD_RC.H                                               |
 | DESCRIPTION	: SequeLink SQL Object Resource Include File.             |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#define STRICT
#include <windows.h>

#include <bwcc.h>


/* ---- SequeLink SQL Object dialog box --------------------------------- */
#define IDHELP			998
#define IDD_OBJECT		100

#define IDC_CONNSET		112
#define IDD_CONNSET		120
#define IDD_VARS		130
#define IDD_OPTIONS		140

/* ---- Connection Preference dialog box ---- */
#define IDD_ADD			210
#define IDD_DEL			220
#define IDC_SERVER		242
#define IDC_PROTOCOL		252
#define IDE_LINKPARAM		262
#define IDC_DATABASE		272
#define IDE_DBNAME		282
#define IDE_USERID		292

/* ---- Variables dialog box ---------------- */
#define IDL_VARS		312
#define IDD_ADDVAR		320
#define IDD_REMVAR		330
#define IDG_VAR			340
#define IDS_NAME		341
#define IDE_NAME		342
#define IDS_TYPE		351
#define IDC_TYPE		352
#define IDS_SIZE		361
#define IDE_SIZE		362
#define IDG_OBJLINK		370
#define IDS_OBJTYPE		381
#define IDC_OBJTYPE		382
#define IDS_OBJ			391
#define IDC_OBJ			392

/* ---- Options dialog box ------------------ */
#define IDE_ROWCOUNT		412
#define IDD_TAB			420
#define IDD_COMMA		430
#define IDD_MANUAL		440
#define IDD_ROLLBACK		450
#define IDD_COMMIT		460

/* ---- Substring dialog box ---------------- */
#define IDS_SUBSTR		511
#define IDE_SUBSTR		512
