/*--------------------------------------------------------------------------*\
 | FILE		: DATB_RC.H                                                 |
 | DESCRIPTION	: Database Object Resource Include File.                    |
 | COPYRIGHT	: Novell, Inc.                                              |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                           |
\*--------------------------------------------------------------------------*/
#define STRICT
#include <windows.h>
#include <bwcc.h>


/* ---- Database Object dialog box ---------------------------------------- */
#define IDHELP			998
#define IDD_OBJECT		100
#define IDS_FILTERSTRING	101

#define IDD_TEXT		111
#define IDD_NUMBER		112
#define IDD_PICTURE		113
#define IDD_DATE		114
#define IDD_TIME		115
#define IDD_OTHER		116

#define IDL_SELECTED		122

#define IDD_ADD			131
#define IDD_REMOVE		132
#define IDD_EDIT		133

#define IDL_AVAILABLE		142

#define IDG_FIELD		150
#define IDD_SIGNIFICANT		151
#define IDS_FIELDNAME		161
#define IDE_FIELDNAME		162
#define IDS_KEYTYPE		171
#define IDC_KEYTYPE		172
#define IDS_CHARS		181
#define IDE_CHARS		182

#define IDE_EXTENSION		192
#define IDC_RECORDID		202

/* ---- Power search dialog box ------------------------------------------- */
#define IDD_FIND		211
#define IDD_APPEND		212
#define IDD_INTERSECT		213
#define IDD_OMIT		214
#define IDL_KEYS		215

#define IDD_RANGE		418
#define IDS_KEY1		419
#define IDE_KEY1		420
#define IDS_KEY2		421
#define IDE_KEY2		422
#define IDS_FOUND		423

#define IDD_INDEXED		424
