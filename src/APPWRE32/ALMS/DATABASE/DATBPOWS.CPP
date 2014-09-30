/*                                                                        *\
    (c) 1993-1995, Novell, Inc.  All rights reserved.

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
 | FILE		: DATBPOWS.CPP                                            |
 | DESCRIPTION	: Database Power Search.                                  |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                         |
\*------------------------------------------------------------------------*/
#include "datb_fnc.h"
#include "datb_rc.h"
#include "macroexe.h"
#include "dbkey.h"
#include "memblock.h"
#include "safemem.h"
#include "helpids.h"


class DATBPOWS			/* ---- Database Power Search ----------- */
{
public:
  BOOL InitPowerSearchDialog(HWND hDlg, AOBJECTID datbObj);
  /* Inits Power Search dialog box. */

  BOOL UpdatePowerSearch(HWND hDlg);
  /* Updates Power Search. */

  BOOL AvailableField(HWND hDlg, LONG lParam);
  /* Available field. */

  BOOL FieldSearch(HWND hDlg);
  /* Field Search group box items. */

  BOOL EditSignificant(HWND hDlg);
  /* Edits significant. */

  BOOL EditRange(HWND hDlg);
  /* Edits range. */

  BOOL EditKeyVal1(HWND hDlg, LONG lParam);
  /* Edits key value 1. */

  BOOL EditKeyVal2(HWND hDlg, LONG lParam);
  /* Edits key value 2. */

  BOOL UpdateRecordFound(HWND hDlg, long foundCount);
  /* Updates record found. */

  BOOL BuildCode(HWND hDlg, int *codeCount, Macro **code);
  /* Builds macro code. */

  BOOL Find(HWND hDlg);
  /* Finds record ids to replace the record ids in found-list. */

  BOOL Append(HWND hDlg);
  /* Appends the newly search record ids to found-list. */

  BOOL Intersect(HWND hDlg);
  /* Intersects the newly search record ids with found-list. */

  BOOL Omit(HWND hDlg);
  /* Omits the newly search record ids from found-list. */
};

/* ---- Database Power Search dialog box -------------------------------- */

BOOL ALMAPI ODatbPowerSearchDialog/* - Power Search Dialog -------------- */
( HWND			hDlg,		// window handle
  UINT			message,	// message
  WPARAM		wParam,		// message
  LPARAM		lParam		// message
)
/* Database Power Search dialog box. */
{ DATBPOWS		pows;		// datbpows class

  switch (message)
  { case WM_INITDIALOG:
      return pows.InitPowerSearchDialog(hDlg, (AOBJECTID) lParam);
    case WM_COMMAND: //!!PORT!!
      switch (wParam)
      {	case IDHELP:
	  return WinHelp(hDlg, "awusdatb.hlp", HELP_CONTEXT, HELPID_OBJD + 1);
	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  pows.UpdatePowerSearch(hDlg);
	  return FALSE;
	case IDOK:
	  EndDialog(hDlg, TRUE);
	  pows.UpdatePowerSearch(hDlg);
	  return TRUE;

	case IDL_KEYS:
	  return pows.AvailableField(hDlg, lParam);
	case IDD_SIGNIFICANT:
	  return pows.EditSignificant(hDlg);
	case IDD_RANGE:
	  return pows.EditRange(hDlg);
	case IDE_KEY1:
	  return pows.EditKeyVal1(hDlg, lParam);
	case IDE_KEY2:
	  return pows.EditKeyVal2(hDlg, lParam);

	case IDD_FIND:
	  return pows.Find(hDlg);
	case IDD_APPEND:
	  return pows.Append(hDlg);
	case IDD_INTERSECT:
	  return pows.Intersect(hDlg);
	case IDD_OMIT:
	  return pows.Omit(hDlg);
      } // switch
  } // switch
  return FALSE;
}  // ODatbPowerSearchDialog

BOOL DATBPOWS::InitPowerSearchDialog/* Init Power Search Dialog --------- */
( HWND		hDlg,			// window handle
  AOBJECTID	datbObj			// datb object
)
/* Inits Power Search dialog box. */
{ MemBlock	mb;			// memblock class
  DatbCore	*datbCore;		// datb core info
  int		fieldCount;		// number of FIELDs
  Field		*field;			// an array of FIELDs
  Macro		*macroPtr;		// macro pointer
  DBKey		dbKey;			// dbkey class
  short		keyType;		// key type

  int		tab = 6;
  char		name[MXVARSIZE + 4];	// name

  int		i, j;			// counters

  SetWindowLong(hDlg, DWL_USER, (long) datbObj);	// save the object id

  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  /* ---- inits Indexed Fields ---- */
  SendDlgItemMessage(hDlg, IDL_KEYS, LB_SETTABSTOPS, 1, (long) &tab);
  for (i = 0; i < fieldCount; i++)
  { if (!field[i].keyType)
      if ((keyType = dbKey.GetSearchKeyType(datbObj, field[i].obj)) == 0)
	continue; // for

    /* ---- inits searchable field ---- */
    macroPtr = (Macro *) MemAlloc(sizeof(Macro));
    macroPtr->oprtr = NOCOMMAND;
    macroPtr->field = i + 1;
    if (field[i].keyType)
    { macroPtr->isIndex = TRUE;
      macroPtr->keyType = field[i].keyType;
      macroPtr->keySize = field[i].keySize;
    } // if
    else
    { macroPtr->isIndex = FALSE;
      macroPtr->keyType = keyType;
      macroPtr->keySize = (short)(keyType == ODATB_KEYTEXT?
			    ODATB_MXKEYSIZE : dbKey.GetKeySize(keyType));
    } // else
    macroPtr->isRange = FALSE;
    macroPtr->keyVal1 = (char *) MemAlloc(macroPtr->keySize);
    macroPtr->keyVal2 = (char *) MemAlloc(macroPtr->keySize);
    switch (macroPtr->keyType)
    { case ODATB_KEYTEXT:
	*macroPtr->keyVal1 = 0;
	*macroPtr->keyVal2 = 0;
	break;
      case ODATB_KEYLONG:
      case ODATB_KEYDATE:
      case ODATB_KEYTIME:
	*(long *) macroPtr->keyVal1 = 0;
	*(long *) macroPtr->keyVal2 = 0;
	break;
      case ODATB_KEYLONGDOUBLE:
	*(long double *) macroPtr->keyVal1 = 0;
	*(long double *) macroPtr->keyVal2 = 0;
	break;
    } // switch

    lstrcpy(name, " \t");
    if (macroPtr->isIndex)
      lstrcat(name, field[i].name);
    else
    { lstrcat(name, "<");
      lstrcat(name, field[i].name);
      lstrcat(name, ">");
    } // else
    j = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_INSERTSTRING,
				 -1, (long) name);
    SendDlgItemMessage(hDlg, IDL_KEYS, LB_SETITEMDATA, j, (long) macroPtr);
  } // for
  if (!SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCOUNT, 0, 0))
    MessageBox(hDlg, "No searchable fields available", NULL, MB_OK);
  else
  { SendDlgItemMessage(hDlg, IDL_KEYS, LB_SETCURSEL, 0, 0);
    FieldSearch(hDlg);
  } // else
  UpdateRecordFound(hDlg, datbCore->foundCount);
  return TRUE;
} // InitPowerSearchDialog

BOOL DATBPOWS::UpdatePowerSearch/* ---- Update Power Search --------------- */
( HWND			hDlg		// window handle
)
/* Updates Power Search. */
{ int			itemCount;	// number of items
  Macro			*macroPtr;	// macro pointer

  int			i;		// counter

  itemCount = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCOUNT, 0, 0);
  for (i = 0; i < itemCount; i++)
  { macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					    i, 0);
    MemFree(macroPtr->keyVal1);
    MemFree(macroPtr->keyVal2);
    MemFree(macroPtr);
  } // for
  return TRUE;
} // UpdatePowerSearch

BOOL DATBPOWS::AvailableField	/* ---- Available Field ------------------- */
( HWND			hDlg,		// window handle
  LONG			lParam		// message
)
/* Available field. */
{
  if (HIWORD(lParam) != LBN_SELCHANGE)
    return FALSE;

  FieldSearch(hDlg);
  return TRUE;
} // AvailableField

BOOL DATBPOWS::FieldSearch	/* ---- Field Search ---------------------- */
( HWND			hDlg		// window handle
)
/* Field Search group box items. */
{ Macro			*macroPtr;	// macro pointer
  DBKey			dbKey;		// dbkey class
  char			str[MXSTRSIZE];	// string

  int			i;		// counter

  i = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCURSEL, 0, 0);
  macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					  i, 0);
  /* ---- OPRTR ---- */
  CheckDlgButton(hDlg, IDD_SIGNIFICANT, macroPtr->oprtr);
  switch (macroPtr->keyType)
  { case ODATB_KEYTEXT:
      SetDlgItemText(hDlg, IDD_RANGE, "Text Range");
      break;
    case ODATB_KEYLONG:
      SetDlgItemText(hDlg, IDD_RANGE, "Integer Range");
      break;
    case ODATB_KEYLONGDOUBLE:
      SetDlgItemText(hDlg, IDD_RANGE, "Real Range");
      break;
    case ODATB_KEYTIME:
      SetDlgItemText(hDlg, IDD_RANGE, "Time Range");
      break;
    case ODATB_KEYDATE:
      SetDlgItemText(hDlg, IDD_RANGE, "Date Range");
      break;
  } // switch

  if (macroPtr->oprtr)
  { /* ---- enables ISRANGE ---- */
    EnableWindow(GetDlgItem(hDlg, IDD_RANGE), TRUE);
    CheckDlgButton(hDlg, IDD_RANGE, macroPtr->isRange);

    /* ---- enables KEYVAL1 ---- */
    dbKey.KeyValToStr(macroPtr->keyVal1, macroPtr->keyType,
		      macroPtr->keySize, str, sizeof(str));
    EnableWindow(GetDlgItem(hDlg, IDS_KEY1), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDE_KEY1), TRUE);
    SetDlgItemText(hDlg, IDE_KEY1, str);

    if (macroPtr->isRange)
    { /* ---- enables KEYVAL2 ---- */
      dbKey.KeyValToStr(macroPtr->keyVal2, macroPtr->keyType,
			macroPtr->keySize, str, sizeof(str));
      EnableWindow(GetDlgItem(hDlg, IDS_KEY2), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDE_KEY2), TRUE);
      SetDlgItemText(hDlg, IDE_KEY2, str);
    } // if
    else
    { /* ---- disables KEYVAL2 ---- */
      EnableWindow(GetDlgItem(hDlg, IDS_KEY2), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDE_KEY2), FALSE);
      SetDlgItemText(hDlg, IDE_KEY2, "");
    } // else
  } // if
  else
  { /* ---- disables ISRANGE ---- */
    EnableWindow(GetDlgItem(hDlg, IDD_RANGE), FALSE);
    CheckDlgButton(hDlg, IDD_RANGE, FALSE);

    /* ---- disables KEYVAL1 ---- */
    EnableWindow(GetDlgItem(hDlg, IDS_KEY1), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDE_KEY1), FALSE);
    SetDlgItemText(hDlg, IDE_KEY1, "");

    /* ---- disables KEYVAL2 ---- */
    EnableWindow(GetDlgItem(hDlg, IDS_KEY2), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDE_KEY2), FALSE);
    SetDlgItemText(hDlg, IDE_KEY2, "");
  } // else
  return TRUE;
} // FieldSearch

BOOL DATBPOWS::EditSignificant	/* ---- Edit Significant ------------------ */
( HWND			hDlg		// window handle
)
/* Edits significant. */
{ Macro			*macroPtr;	// macro pointer
  char			name[MXVARSIZE + 4];	// name

  int			i;		// counter

  i = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCURSEL, 0, 0);
  macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					  i, 0);
  macroPtr->oprtr = !macroPtr->oprtr;
  FieldSearch(hDlg);

  /* ---- edits Indexed Fields ---- */
  SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETTEXT, i, (long) name);
  *name = macroPtr->oprtr? '*' : ' ';
  SendDlgItemMessage(hDlg, IDL_KEYS, LB_DELETESTRING, i, 0);
  SendDlgItemMessage(hDlg, IDL_KEYS, LB_INSERTSTRING, i, (long) name);
  SendDlgItemMessage(hDlg, IDL_KEYS, LB_SETITEMDATA, i, (long) macroPtr);
  SendDlgItemMessage(hDlg, IDL_KEYS, LB_SETCURSEL, i, 0);
  return TRUE;
} // EditSignificant

BOOL DATBPOWS::EditRange	/* ---- Edit Range ------------------------ */
( HWND			hDlg		// window handle
)
/* Edits range. */
{ Macro			*macroPtr;	// macro pointer

  int			i;		// counter

  i = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCURSEL, 0, 0);
  macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					  i, 0);
  macroPtr->isRange = !macroPtr->isRange;
  FieldSearch(hDlg);
  return TRUE;
} // EditRange

BOOL DATBPOWS::EditKeyVal1	/* ---- Edit Key Value 1 ------------------ */
( HWND			hDlg,		// window handle
  LONG			lParam		// message
)
/* Edits key value 1. */
{ Macro			*macroPtr;	// macro pointer
  DBKey			dbKey;		// dbkey class
  char			str[MXSTRSIZE];	// string

  int			i;		// counter

  if (HIWORD(lParam) != EN_KILLFOCUS)
    return FALSE;

  i = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCURSEL, 0, 0);
  macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					  i, 0);
  GetDlgItemText(hDlg, IDE_KEY1, str, sizeof(str));
  dbKey.KeyValFromStr(macroPtr->keyVal1, macroPtr->keyType,
		      macroPtr->keySize, str, sizeof(str));
  SetDlgItemText(hDlg, IDE_KEY1, macroPtr->keyVal1);
  return TRUE;
} // EditKeyVal1

BOOL DATBPOWS::EditKeyVal2	/* ---- Edit Key Value 2 ------------------ */
( HWND			hDlg,		// window handle
  LONG			lParam		// message
)
/* Edits key value 2. */
{ Macro			*macroPtr;	// macro pointer
  DBKey			dbKey;		// dbkey class
  char			str[MXSTRSIZE];	// string

  int			i;		// counter

  if (HIWORD(lParam) != EN_KILLFOCUS)
    return FALSE;

  i = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCURSEL, 0, 0);
  macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					  i, 0);
  GetDlgItemText(hDlg, IDE_KEY2, str, sizeof(str));
  dbKey.KeyValFromStr(macroPtr->keyVal2, macroPtr->keyType,
		      macroPtr->keySize, str, sizeof(str));
  SetDlgItemText(hDlg, IDE_KEY2, macroPtr->keyVal2);
  return TRUE;
} // EditKeyVal2

BOOL DATBPOWS::UpdateRecordFound/* ---- Update Record Found --------------- */
( HWND			hDlg,		// window handle
  long			foundCount	// number of record ids in found-list
)
/* Updates record found. */
{ char			str[32];	// string

  wsprintf(str, "%ld Records Found", foundCount);
  SetDlgItemText(hDlg, IDS_FOUND, str);
  return TRUE;
} // UpdateRecordFound

BOOL DATBPOWS::BuildCode	/* ---- Build Code ------------------------ */
( HWND			hDlg,		// window handle
  int			*codeCount,	// number of MACROs
  Macro			**code		// an array of MACROs
)
/* Builds code. */
{ int			itemCount;	// number of items
  Macro			*macroPtr;	// macro pointer
  int			macroCodeCount;	// number of MACROs
  Macro			*macroCode;	// an array of MACROs

  int			i;		// counter

  itemCount = (int) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETCOUNT, 0, 0);
  macroCodeCount = 0;
  for (i = 0; i < itemCount; i++)
  { macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					    i, 0);
    if (macroPtr->oprtr)
      macroCodeCount++;
  } // for
  macroCode = (Macro *) MemAlloc(macroCodeCount * sizeof(Macro));
  macroCodeCount = 0;
  for (i = 0; i < itemCount; i++)
  { macroPtr = (Macro *) SendDlgItemMessage(hDlg, IDL_KEYS, LB_GETITEMDATA,
					    i, 0);
    if (macroPtr->oprtr)
    { memcpy(&macroCode[macroCodeCount], macroPtr, sizeof(Macro));
      macroCodeCount++;
    } // if
  } // if
  *codeCount = macroCodeCount;
  *code = macroCode;
  return TRUE;
} // BuildCode

BOOL DATBPOWS::Find		/* ---- Find ------------------------------ */
( HWND			hDlg		// window handle
)
/* Finds record ids to replace the record ids in found-list. */
{ MacroExe		exe;		// macroexe class
  MemBlock		mb;		// memblock class
  AOBJECTID		datbObj;	// datb object
  DatbCore		*datbCore;	// datb core info
  int			fieldCount;	// number of fields
  Field			*field;		// an array of fields
  int			codeCount;	// number of MACROs
  Macro			*code;		// an array of MACROs

  HCURSOR		cursor = LoadCursor(NULL, IDC_WAIT);
  cursor = SetCursor(cursor);

  datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  fieldCount = AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field);
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  BuildCode(hDlg, &codeCount, &code);
  if (datbCore->found)
    MBFree(datbCore->found);
  exe.Search(datbObj, datbCore, fieldCount, field, codeCount, code,
             &datbCore->foundCount, &datbCore->found);
  MemFree(code);

  UpdateRecordFound(hDlg, datbCore->foundCount);
  datbCore->foundIdx = 0;
  SetCursor(cursor);
  return TRUE;
} // Find

BOOL DATBPOWS::Append		/* ---- Append ---------------------------- */
( HWND			hDlg		// window handle
)
/* Appends the newly search record ids to found-list. */
{ MacroExe		exe;		// macroexe class
  MemBlock		mb;		// memblock class
  AOBJECTID		datbObj;	// datb object
  DatbCore		*datbCore;	// datb core info
  WORD			fieldCount;	// number of fields
  Field			*field;		// an array of fields
  int			codeCount;	// number of MACROs
  Macro			*code;		// an array of MACROs
  long			listCount;	// number of items in temporary list
  long			*list;		// temporary list

  HCURSOR		cursor = LoadCursor(NULL, IDC_WAIT);
  cursor = SetCursor(cursor);

  datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  fieldCount = (WORD) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  BuildCode(hDlg, &codeCount, &code);
  exe.Search(datbObj, datbCore, fieldCount, field, codeCount, code,
             &listCount, &list);
  exe.Merge(&datbCore->found, &datbCore->foundCount, list, listCount);
  MBFree(list);
  MemFree(code);

  UpdateRecordFound(hDlg, datbCore->foundCount);
  datbCore->foundIdx = 0;
  SetCursor(cursor);
  return TRUE;
} // Append

BOOL DATBPOWS::Intersect	/* ---- Intersect ------------------------- */
( HWND			hDlg		// window handle
)
/* Intersects the newly search record ids with found-list. */
{ MacroExe		exe;		// macroexe class
  MemBlock		mb;		// memblock class
  AOBJECTID		datbObj;	// datb object
  DatbCore		*datbCore;	// datb core info
  WORD			fieldCount;	// number of fields
  Field			*field;		// an array of fields
  int			codeCount;	// number of MACROs
  Macro			*code;		// an array of MACROs
  long			listCount;	// number of items in temporary list
  long			*list;		// temporary list

  HCURSOR		cursor = LoadCursor(NULL, IDC_WAIT);
  cursor = SetCursor(cursor);

  datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  fieldCount = (WORD) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  BuildCode(hDlg, &codeCount, &code);
  exe.Search(datbObj, datbCore, fieldCount, field, codeCount, code,
             &listCount, &list);
  exe.Require(&datbCore->found, &datbCore->foundCount, list, listCount);
  MBFree(list);
  MemFree(code);

  UpdateRecordFound(hDlg, datbCore->foundCount);
  datbCore->foundIdx = 0;
  SetCursor(cursor);
  return TRUE;
} // Intersect

BOOL DATBPOWS::Omit		/* ---- Omit ------------------------------ */
( HWND			hDlg		// window handle
)
/* Omits the newly search record ids from found-list. */
{ MacroExe		exe;		// macroexe class
  MemBlock		mb;		// memblock class
  AOBJECTID		datbObj;	// datb object
  DatbCore		*datbCore;	// datb core info
  WORD			fieldCount;	// number of fields
  Field			*field;		// an array of fields
  int			codeCount;	// number of MACROs
  Macro			*code;		// an array of MACROs
  long			listCount;	// number of items in temporary list
  long			*list;		// temporary list

  HCURSOR		cursor = LoadCursor(NULL, IDC_WAIT);
  cursor = SetCursor(cursor);

  datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);
  datbCore = (DatbCore *) mb.GetPointer(datbObj, DATBCORE);
  fieldCount = (WORD) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  BuildCode(hDlg, &codeCount, &code);
  exe.Search(datbObj, datbCore, fieldCount, field, codeCount, code,
             &listCount, &list);
  exe.Exclude(&datbCore->found, &datbCore->foundCount, list, listCount);
  MBFree(list);
  MemFree(code);

  UpdateRecordFound(hDlg, datbCore->foundCount);
  datbCore->foundIdx = 0;
  SetCursor(cursor);
  return TRUE;
} // Omit
