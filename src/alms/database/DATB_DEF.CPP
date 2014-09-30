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
 | FILE         : DATB_DEF.CPP                                            |
 | DESCRIPTION  : Database Object Definition.                             |
 | COPYRIGHT    : 1993-1995 Novell, Inc.                                  |
 | EDIT LOG     :                                                         |
 |   v1.00.00     mw    04-Jan-1994  Initial Edit                         |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#include "datb.h"
#include "datb_rc.h"
#include "dbkey.h"
#include "memblock.h"
#include "safemem.h"
#include "strutl.h"

#include "a_almutl.h"
#include "helpids.h"


class DATB_DEF                  /* ---- Database Object Definition ------ */
{
public:
  BOOL InitObjectDialog(HWND hDlg, AOBJECTID datbObj);
  /* Inits Database Object dialog box. */

  BOOL CancelUpdateObject(HWND hDlg);
  /* No Updates Database Object. */

  BOOL UpdateObject(HWND hDlg);
  /* Updates Database Object. */

  BOOL DisableField(HWND hDlg);
  /* Disables Sortable Field group box. */

  BOOL EnableField(HWND hDlg);
  /* Enables Sortable Field group box. */

  BOOL InitAvailableObject(HWND hDlg, WORD buttonID);
  /* Inits Available Objects list box. */

  BOOL AvailableObject(HWND hDlg, WORD wCmd);
  /* Available Object. */

  BOOL SelectedObject(HWND hDlg, WORD wCmd);
  /* Selected Object. */

  BOOL AddObject(HWND hDlg);
  /* Adds objects from Available Objects to Objects in Record list. */

  BOOL RemoveObject(HWND hDlg);
  /* Removes objects from Objects in Record. */

  BOOL EditObject(HWND hDlg);
  /* Edits object in Objects in Record. */

  BOOL EditFieldName(HWND hDlg, WORD wCmd);
  /* Edits Field Name. */

  BOOL EditIndexed(HWND hDlg);
  /* Edits Indexed. */

  BOOL EditKeyType(HWND hDlg, WORD wCmd);
  /* Edits Key Type. */

  BOOL EditCharacters(HWND hDlg, WORD wCmd);
  /* Edits Characters. */

  BOOL EditDefFileExt(HWND hDlg, WORD wCmd);
  /* Edits Default File Extension. */

  BOOL EditRecIDObj(HWND hDlg, WORD wCmd);
  /* Edits Record ID Object. */
};

/* ---- Database Object dialog box ---------------------------------------- */

BOOL ALMAPI ODatbObjectDialog   /* ---- Database Object Dialog ------------ */
( HWND		hDlg,			// window handle
  UINT		message,		// message
  WPARAM	wParam,			// message
  LPARAM	lParam			// message
)
/* Database Object dialog box. */
{ DATB_DEF	def;			// datb_def class
  WORD		wCmd;			// command
  WORD		wId;			// identifier

  switch (message)
  { case WM_INITDIALOG:
      return def.InitObjectDialog(hDlg, (AOBJECTID) lParam);
    case WM_COMMAND:
      wCmd = HIWORD(wParam);
      wId = LOWORD(wParam);
      switch (wId)
      { case IDHELP:
	  return WinHelp(hDlg, "awusdatb.hlp", HELP_CONTEXT, HELPID_OBJD);
	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  return def.CancelUpdateObject(hDlg);
	case IDOK:
	  EndDialog(hDlg, TRUE);
	  return def.UpdateObject(hDlg);

	case IDD_TEXT:
	case IDD_NUMBER:
	case IDD_DATE:
	case IDD_TIME:
	case IDD_PICTURE:
	case IDD_OTHER:
	  return def.InitAvailableObject(hDlg, wId);
	case IDL_AVAILABLE:
	  return def.AvailableObject(hDlg, wCmd);
	case IDL_SELECTED:
	  return def.SelectedObject(hDlg, wCmd);
	case IDD_ADD:
	  return def.AddObject(hDlg);
	case IDD_REMOVE:
	  return def.RemoveObject(hDlg);
	case IDD_EDIT:
	  return def.EditObject(hDlg);
	case IDE_FIELDNAME:
	  return def.EditFieldName(hDlg, wCmd);
	case IDD_INDEXED:
	  return def.EditIndexed(hDlg);
	case IDC_KEYTYPE:
	  return def.EditKeyType(hDlg, wCmd);
	case IDE_CHARS:
	  return def.EditCharacters(hDlg, wCmd);
	case IDE_EXTENSION:
	  return def.EditDefFileExt(hDlg, wCmd);
	case IDC_RECORDID:
	  return def.EditRecIDObj(hDlg, wCmd);
      } // switch
  } // switch
  return FALSE;
} // DatbObjectDialog

BOOL DATB_DEF::InitObjectDialog /* ---- Init Object Dialog ---------------- */
( HWND                  hDlg,           // window handle
  AOBJECTID              datbObj         // datb object
)
/* Inits Database Object dialog box. */
{ MemBlock              mb;             // memblock class
  char                  str[MXSTRSIZE];	// string
  Datb                  *datb;          // datb core info
  char			name[MXVARSIZE];// name
  int                   fieldCount;     // number of FIELDs
  Field                 *field;		// an array of FIELDs
  Field                 *fieldPtr;      // field pointer

  long                  objIdx;         // object index
  AOBJECTID              obj;            // object

  long                   i, j;           // counters

  SetWindowLong(hDlg, DWL_USER, (long) datbObj);        // save the object ID
  datb = (Datb *) mb.GetPointer(datbObj, DATBCORE);
  fieldCount = (int) (AObjGetDataSize(datbObj, DATBFIELDS) / sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  /* ---- inits datb object type name, and datb object name ---- */
  AUtlSetTitleFont(OTYPE_DATABASE, NULL, GetDlgItem(hDlg, IDD_OBJECT));
	AUtlCenterDialog(hDlg, 0);
  AObjGetName(datbObj, str, sizeof(str));
  SetWindowText(hDlg, str);

  /* ---- inits Available Objects ---- */
  InitAvailableObject(hDlg, IDD_TEXT);

  /* ---- inits Objects in Record ---- */
  for (i = 0; i < fieldCount; i++)
    if ((field[i].type = AObjGetType(field[i].obj)) != 0)
    { AObjGetName(field[i].obj, name, sizeof(name));
      fieldPtr = (Field *) MemAlloc(sizeof(Field));
      CopyMemory(fieldPtr, &field[i], sizeof(Field));

      j = SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING,
			     -1, (LPARAM) name);
      SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA,
			 j, (LPARAM) fieldPtr);
    } // if
  /* inserts an empty mask */
  j = SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING,
			 -1, (LPARAM) "");
  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, j, 0);

  /* ---- inits Default File Extension ---- */
  SetDlgItemText(hDlg, IDE_EXTENSION, datb->defFileExt);

  /* ---- inits Record ID Object ---- */
  i = SendDlgItemMessage(hDlg, IDC_RECORDID, CB_INSERTSTRING,
			 -1, (LPARAM) "(None)");
  SendDlgItemMessage(hDlg, IDC_RECORDID, CB_SETITEMDATA, i, 0);
  if (!datb->recIDObj)
    SendDlgItemMessage(hDlg, IDC_RECORDID, CB_SETCURSEL, i, 0);

  objIdx = AOBJ_GETFIRSTKEY;
  while((obj = AObjGetNextObject(OTYPE_NUMBER, &objIdx)) != 0)
  { AObjGetName(obj, name, sizeof(name));

    i = SendDlgItemMessage(hDlg, IDC_RECORDID, CB_INSERTSTRING,
			   -1, (LPARAM) name);
    SendDlgItemMessage(hDlg, IDC_RECORDID, CB_SETITEMDATA, i, (LPARAM) obj);
    if (datb->recIDObj == obj)
      SendDlgItemMessage(hDlg, IDC_RECORDID, CB_SETCURSEL, i, 0);
  } // while
  return TRUE;
} // InitObjectDialog

BOOL DATB_DEF::CancelUpdateObject/* --- No Update Object ------------------ */
( HWND                  hDlg            // window handle
)
/* Cancels Update Database Object. */
{ int                   fieldCount;     // number of FIELDs
  Field                 *fieldPtr;      // field pointer

  int                   i;              // counter

  fieldCount = (int) (SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETCOUNT,
					 0, 0) - 1);	// ignores mask item
  for (i = 0; i < fieldCount; i++)
  { fieldPtr = (Field *)
      SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, i, 0);
    MemFree(fieldPtr);
  } // for
  return TRUE;
} // CancelUpdateObject

BOOL DATB_DEF::UpdateObject     /* ---- Update Object --------------------- */
( HWND                  hDlg            // window handle
)
/* Updates Database Object. */
{ MemBlock              mb;             // memblock class
  AOBJECTID              datbObj;        // datb object
  Datb                  *datb;          // datb core info
  int                   fieldCount;     // number of FIELDs
  Field                 *field;		// an array of FIELDs
  Field                 *fieldPtr;      // field pointer

  int                   i;              // counter

  datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);
  datb = (Datb *) mb.GetPointer(datbObj, DATBCORE);
  fieldCount = (int) (SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETCOUNT,
					 0, 0) - 1);	// ignores mask item
  MemResizeObjectData(datbObj, DATBFIELDS, fieldCount * sizeof(Field));
  field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);

  /* ---- updates Default File Extension ---- */
  GetDlgItemText(hDlg, IDE_EXTENSION,
		 datb->defFileExt, sizeof(datb->defFileExt));

  /* ---- updates Record ID Object ----- */
  i = (int) SendDlgItemMessage(hDlg, IDC_RECORDID, CB_GETCURSEL, 0, 0);
  datb->recIDObj = (AOBJECTID)
    SendDlgItemMessage(hDlg, IDC_RECORDID, CB_GETITEMDATA, i, 0);

  /* ---- updates FIELDs ---- */
  for (i = 0; i < fieldCount; i++)
  { fieldPtr = (Field *)
      SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, i, 0);
    CopyMemory(&field[i], fieldPtr, sizeof(Field));
    MemFree(fieldPtr);
  } // for
  return TRUE;
} // UpdateObject

BOOL DATB_DEF::DisableField     /* ---- Disable Field --------------------- */
( HWND                  hDlg            // window handle
)
/* Disables Sortable Field group box. */
{
  EnableWindow(GetDlgItem(hDlg, IDG_FIELD), FALSE);

  /* ---- Field Name ---- */
  EnableWindow(GetDlgItem(hDlg, IDS_FIELDNAME), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDE_FIELDNAME), FALSE);
  SetDlgItemText(hDlg, IDE_FIELDNAME, "");

  /* ---- Indexed ---- */
  EnableWindow(GetDlgItem(hDlg, IDD_INDEXED), FALSE);
  CheckDlgButton(hDlg, IDD_INDEXED, FALSE);

  /* ---- Key Type ---- */
  EnableWindow(GetDlgItem(hDlg, IDS_KEYTYPE), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDC_KEYTYPE), FALSE);
  SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETCURSEL, -1, 0);

  /* ---- Characters ---- */
  EnableWindow(GetDlgItem(hDlg, IDS_CHARS), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDE_CHARS), FALSE);
  SetDlgItemText(hDlg, IDE_CHARS, "");
  return TRUE;
} // DisableField

BOOL DATB_DEF::EnableField      /* ---- Enable Field ---------------------- */
( HWND                  hDlg            // window handle
)
/* Enables Sortable Field group box. */
{ Field                 *field;         // field
  AOBJECTID              datbObj;        // datb object
  DBKey                 dbKey;          // dbkey class

  int                   i;              // counter

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
				       i, 0);
  if (field->keyType == ODATB_KEYQUERY)
  { datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);
    if (dbKey.GetKeyType(datbObj, field->obj, field->type) == ODATB_KEYQUERY)
    { /* ---- Sortable Field ---- */
      EnableWindow(GetDlgItem(hDlg, IDG_FIELD), FALSE);

      /* ---- Field Name ---- */
      EnableWindow(GetDlgItem(hDlg, IDS_FIELDNAME), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDE_FIELDNAME), FALSE);
      SetDlgItemText(hDlg, IDE_FIELDNAME, "");

      /* ---- Indexed ---- */
      EnableWindow(GetDlgItem(hDlg, IDD_INDEXED), FALSE);
    } // if
    else
    { /* ---- Sortable Field ---- */
      EnableWindow(GetDlgItem(hDlg, IDG_FIELD), TRUE);

      /* ---- Field Name ---- */
      EnableWindow(GetDlgItem(hDlg, IDS_FIELDNAME), TRUE);
      EnableWindow(GetDlgItem(hDlg, IDE_FIELDNAME), TRUE);
      SetDlgItemText(hDlg, IDE_FIELDNAME, field->name);

      /* ---- Indexed ---- */
      EnableWindow(GetDlgItem(hDlg, IDD_INDEXED), TRUE);
    } // else
    CheckDlgButton(hDlg, IDD_INDEXED, FALSE);

    /* ---- Key Type ---- */
    EnableWindow(GetDlgItem(hDlg, IDS_KEYTYPE), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_KEYTYPE), FALSE);
    SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETCURSEL, -1, 0);

    /* ---- Characters ---- */
    EnableWindow(GetDlgItem(hDlg, IDS_CHARS), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDE_CHARS), FALSE);
    SetDlgItemText(hDlg, IDE_CHARS, "");

    return TRUE;
  } // if

  /* ---- Sortable Field ---- */
  EnableWindow(GetDlgItem(hDlg, IDG_FIELD), TRUE);

  /* ---- Field Name ---- */
  EnableWindow(GetDlgItem(hDlg, IDS_FIELDNAME), TRUE);
  EnableWindow(GetDlgItem(hDlg, IDE_FIELDNAME), TRUE);
  SetDlgItemText(hDlg, IDE_FIELDNAME, field->name);

  /* ---- Indexed ---- */
  EnableWindow(GetDlgItem(hDlg, IDD_INDEXED), TRUE);
  CheckDlgButton(hDlg, IDD_INDEXED, TRUE);

  /* ---- Key Type ---- */
  EnableWindow(GetDlgItem(hDlg, IDS_KEYTYPE), TRUE);
  EnableWindow(GetDlgItem(hDlg, IDC_KEYTYPE), TRUE);
  SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_RESETCONTENT, 0, 0);
  switch (field->keyType)
  { case ODATB_KEYTEXT:
      i = (int) SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_INSERTSTRING,
				   -1, (long) "Text");
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETITEMDATA,
			 i, field->keyType);
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETCURSEL, i, 0);
      break;
    case ODATB_KEYLONG:
    case ODATB_KEYLONGDOUBLE:
      i = (int) SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_INSERTSTRING,
				   -1, (long) "Integer");
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETITEMDATA,
                         i, ODATB_KEYLONG);
      if (field->keyType == ODATB_KEYLONG)
	SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETCURSEL, i, 0);

      i = (int) SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_INSERTSTRING,
				   -1, (long) "Real");
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETITEMDATA,
			 i, ODATB_KEYLONGDOUBLE);
      if (field->keyType == ODATB_KEYLONGDOUBLE)
	SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETCURSEL, i, 0);
      break;
    case ODATB_KEYDATE:
      i = (int) SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_INSERTSTRING,
				   -1, (long) "Date");
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETITEMDATA,
			 i, field->keyType);
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETCURSEL, i, 0);
      break;
    case ODATB_KEYTIME:
      i = (int) SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_INSERTSTRING,
				   -1, (long) "Time");
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETITEMDATA,
			 i, field->keyType);
      SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_SETCURSEL, i, 0);
      break;
  } // switch

  /* ---- Characters ---- */
  if (field->keyType == ODATB_KEYTEXT)
  { EnableWindow(GetDlgItem(hDlg, IDS_CHARS), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDE_CHARS), TRUE);
    SetDlgItemInt(hDlg, IDE_CHARS, field->keySize - 1, FALSE);
  } // if
  else
  { EnableWindow(GetDlgItem(hDlg, IDS_CHARS), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDE_CHARS), FALSE);
    SetDlgItemText(hDlg, IDE_CHARS, "");
  } // else
  return TRUE;
} // EnableField

BOOL DATB_DEF::InitAvailableObject/* -- Init Available Object ------------- */
( HWND		hDlg,			// window handle
  WORD		wButtonID		// button id
)
/* Inits Available Objects list box. */
{ char		name[MXVARSIZE];	// name
  long		typeIdx;		// type index
  ATYPEID	type;			// type
  long		objIdx;			// object index
  AOBJECTID	obj;			// object

  int		i;			// counter

  CheckRadioButton (hDlg, IDD_TEXT, IDD_OTHER, wButtonID);

  EnableWindow(GetDlgItem(hDlg, IDD_ADD), FALSE);

  SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_RESETCONTENT, 0, 0);
  switch (wButtonID)
  { case IDD_OTHER:
      typeIdx = AOBJ_GETFIRSTKEY;
      while ((type = ATypeGetNext (&typeIdx)) != 0)
	if (!(type == OTYPE_TEXT     || type == OTYPE_NUMBER   ||
	      type == OTYPE_DATE     || type == OTYPE_TIME     ||
	      type == OTYPE_PICTURE  || type == OTYPE_DATABASE))
	{ objIdx = AOBJ_GETFIRSTKEY;
	  while((obj = AObjGetNextObject(type, &objIdx)) != 0)
	  { AObjGetName(obj, name, sizeof(name));
	    i = (int) SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_INSERTSTRING,
					 -1, (LPARAM) name);
	    SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA,
			       i, (LPARAM) obj);
	  } // while
	} // if
      return TRUE;
    case IDD_TEXT:
      type = OTYPE_TEXT;
      break;
    case IDD_NUMBER:
      type = OTYPE_NUMBER;
      break;
    case IDD_DATE:
      type = OTYPE_DATE;
      break;
    case IDD_TIME:
      type = OTYPE_TIME;
      break;
    case IDD_PICTURE:
      type = OTYPE_PICTURE;
      break;
    default:
      return FALSE;
  } // switch
  objIdx = AOBJ_GETFIRSTKEY;
  while((obj = AObjGetNextObject(type, &objIdx)) != 0)
  { AObjGetName(obj, name, sizeof(name));
    i = (int) SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_INSERTSTRING,
				 -1, (LPARAM) name);
    SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, i, (LPARAM) obj);
  } // while
  return TRUE;
} // InitAvailableObject

BOOL DATB_DEF::AvailableObject  /* ---- Available Object ------------------ */
( HWND		hDlg,			// window handle
  WORD		cCmd			// command
)
/* Available Object. */
{
  switch (cCmd)
  { case LBN_SELCHANGE:
      if (SendDlgItemMessage (hDlg, IDL_AVAILABLE, LB_GETSELCOUNT, 0, 0))
	EnableWindow(GetDlgItem(hDlg, IDD_ADD), TRUE);
      else
	EnableWindow(GetDlgItem(hDlg, IDD_ADD), FALSE);
      return TRUE;
    case LBN_DBLCLK:
      AddObject(hDlg);
      return TRUE;
  } // switch
  return FALSE;
} // AvailableObject

BOOL DATB_DEF::SelectedObject   /* ---- Selected Object ------------------- */
( HWND		hDlg,			// window handle
  WORD		cCmd			// command
)
/* Selected Object. */
{ int		selItemCount;		// number of selected items

  switch (cCmd)
  { case LBN_SELCHANGE:
      selItemCount = (int)
	SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0);
      if (selItemCount)
	if (SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSEL, (LPARAM)
	    SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0) - 1, 0))
	  selItemCount--;               // ignores the mask item

      if (selItemCount)
	EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), TRUE);
      else
	EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), FALSE);

      if (selItemCount == 1)
      { EnableWindow(GetDlgItem(hDlg, IDD_EDIT), TRUE);
	EnableField(hDlg);
      } // if
      else
      { EnableWindow(GetDlgItem(hDlg, IDD_EDIT), FALSE);
	DisableField(hDlg);
      } // else
      return TRUE;
    case LBN_DBLCLK:
      selItemCount = (int)
	SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0);
      if (selItemCount)
	if (SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSEL, (LPARAM)
	    SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0) - 1, 0))
	  selItemCount--;               // ignores the mask item

      if (selItemCount == 1)
	EditObject(hDlg);
      return TRUE;
  } // switch
  return FALSE;
} // SelectedObject

BOOL DATB_DEF::AddObject        /* ---- Add Object ------------------------ */
( HWND                  hDlg            // window handle
)
/* Adds objects from Available Objects to Objects in Record. */
{ AOBJECTID              datbObj;        // datbObj
  DBKey                 dbKey;          // dbkey class

  int                   itemCount;      // number of items
  int                   itemIdx;        // item index
  int                   topIdx;         // top index

  int                   selItemCount;   // number of selected items
  int                   *selItem;       // an array of selected items
  AOBJECTID              obj;            // object
  Field                 *field;         // field

  int                   i;              // counter

  datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);

  /* ---- finds the place to insert ---- */
  itemCount = (int) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0);
  if (!SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS,
                          1, (long) &itemIdx))
    itemIdx = itemCount - 1;

  /* ---- finds items to insert ---- */
  selItemCount = (int)
    SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_GETSELCOUNT, 0, 0);
  selItem = (int *) MemAlloc(selItemCount * sizeof(int));
  SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_GETSELITEMS,
                     selItemCount, (long) selItem);

  /* ---- insert items ---- */
  for (i = selItemCount - 1; i >= 0; i--)
  { obj = (AOBJECTID) SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_GETITEMDATA,
                                        selItem[i], 0);

    /* inits field */
    field = (Field *) MemAlloc(sizeof(Field));
    field->obj = obj;
    field->type = AObjGetType(obj);
    AObjGetName(obj, field->name, sizeof(field->name));
    if (itemCount == 1 && i == 0)
      field->keyType = dbKey.GetKeyType(datbObj, field->obj, field->type);
    else
      field->keyType = ODATB_KEYQUERY;
    field->keySize = dbKey.GetKeySize(field->keyType);
    field->refObj = 0;

    SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING,
                       itemIdx, (long) field->name);
    SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA,
                       itemIdx, (long) field);
  } // for
  MemFree(selItem);

  /* ---- makes the newly added items visible ---- */
  topIdx = (int) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETTOPINDEX, 0, 0);
  if (itemIdx < topIdx)
  { if (selItemCount > 9)
      itemIdx += selItemCount - 9;
    SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETTOPINDEX, itemIdx, 0);
  } // if
  else
    if (itemIdx + selItemCount > topIdx + 9)
      SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETTOPINDEX,
                         itemIdx + selItemCount - 9, 0);
  return TRUE;
} // AddObject

BOOL DATB_DEF::RemoveObject     /* ---- Remove Object --------------------- */
( HWND                  hDlg            // window handle
)
/* Removes objects from Objects in Record. */
{ int                   selItemCount;   // number of selected items
  int                   *selItem;       // an array of selected items
  Field                 *field;         // field

  int                   i;              // counter

  EnableWindow(GetDlgItem(hDlg, IDD_REMOVE), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDD_EDIT), FALSE);

  /* ---- finds the places to delete ---- */
  selItemCount = (int)
    SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0);
  if (SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSEL, (int)
        SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETCOUNT, 0, 0) - 1, 0))
    selItemCount--;                     // ignores the mask item
  selItem = (int *) MemAlloc(selItemCount * sizeof(int));
  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS,
                     selItemCount, (long) selItem);

  /* ---- deletes the variables ---- */
  for (i = selItemCount - 1; i >= 0; i--)
  { field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
                                         selItem[i], 0);
    MemFree(field);
    SendDlgItemMessage(hDlg, IDL_SELECTED, LB_DELETESTRING, selItem[i], 0);
  } // for

  MemFree(selItem);
  if (selItemCount == 1)
    DisableField(hDlg);
  SetFocus(GetDlgItem(hDlg, IDL_SELECTED));     // keeps the focus
  return TRUE;
} // RemoveObject

BOOL DATB_DEF::EditObject       /* ---- Edit Object ----------------------- */
( HWND                  hDlg            // window handle
)
/* Edits object in Objects in Record. */
{ Field                 *field;         // field

  int                   i;              // counter

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
                                       i, 0);
  AObjEdit(field->obj);
  return TRUE;
} // EditObject

BOOL DATB_DEF::EditFieldName    /* ---- Edit Field Name ------------------- */
( HWND		hDlg,			// window handle
  WORD		wCmd			// command
)
/* Edits Field Name. */
{ Field		*field;			// field
  StrUtl	s;			// strutl class
  char		str[MXSTRSIZE];		// string

  int		i;			// counter

  if (wCmd != EN_KILLFOCUS)
    return FALSE;

  GetDlgItemText(hDlg, IDE_FIELDNAME, str, sizeof(str));
  s.TrimVar(str);                       // trims invalid characters
  if (*str == 0)
    lstrcpy(str, DEF_VARNAME);          // doesn't allow empty name
  else
    str[sizeof(str) - 1] = 0;           // truncates to MXVARLEN
  SetDlgItemText(hDlg, IDE_FIELDNAME, str);

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (LPARAM) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
				       i, 0);
  lstrcpy(field->name, str);
  return TRUE;
} // EditFieldName

BOOL DATB_DEF::EditIndexed      /* ---- Edit Indexed ---------------------- */
( HWND		hDlg			// window handle
)
/* Edits Indexed. */
{ AOBJECTID	datbObj;		// datb object
  DBKey		dbKey;			// dbkey class
  BOOL		isChecked;		// is checked?
  Field		*field;			// field

  int		i;			// counter

  isChecked = IsDlgButtonChecked(hDlg, IDD_INDEXED);
  CheckDlgButton(hDlg, IDD_INDEXED, isChecked);

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (LPARAM) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
				       i, 0);
  if (isChecked)
  { datbObj = (AOBJECTID) GetWindowLong(hDlg, DWL_USER);
    field->keyType = dbKey.GetKeyType(datbObj, field->obj, field->type);
  } // if
  else
    field->keyType = ODATB_KEYQUERY;
  field->keySize = dbKey.GetKeySize(field->keyType);
  EnableField(hDlg);
  return TRUE;
} // EditIndexed

BOOL DATB_DEF::EditKeyType      /* ---- Edit Key Type --------------------- */
( HWND		hDlg,			// window handle
  WORD		wCmd			// command
)
/* Edits Key Type. */
{ Field		*field;			// field

  int		i;			// counter

  if (wCmd != CBN_SELCHANGE)
    return FALSE;

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (LPARAM) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
				       i, 0);
  i = (int) SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_GETCURSEL, 0, 0);
  field->keyType = (short int)
    SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_GETITEMDATA, i, 0);
  return TRUE;
} // EditKeyType

BOOL DATB_DEF::EditCharacters   /* ---- Edit Characters ------------------- */
( HWND		hDlg,			// window handle
  WORD		wCmd			// command
)
/* Edits Characters. */
{ Field		*field;			// field

  int		i;			// counter

  if (wCmd != EN_KILLFOCUS)
    return FALSE;

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (LPARAM) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
				       i, 0);
  field->keySize = (short int)
		     (GetDlgItemInt(hDlg, IDE_CHARS, NULL, FALSE) + 1);
  if (field->keySize > ODATB_MXKEYSIZE)
    field->keySize = ODATB_MXKEYSIZE;
  SetDlgItemInt(hDlg, IDE_CHARS, field->keySize - 1, FALSE);
  return TRUE;
} // EditCharacters

BOOL DATB_DEF::EditDefFileExt   /* ---- Edit Default File Extension ------- */
( HWND		hDlg,			// window handle
  WORD		wCmd			// command
)
/* Edits Default File Extension. */
{ char		s[FILEEXTSIZE];		// buffer

  if (wCmd != EN_KILLFOCUS)
    return FALSE;

  GetDlgItemText(hDlg, IDE_EXTENSION, s, sizeof(s));
  SetDlgItemText(hDlg, IDE_EXTENSION, s);
  return TRUE;
} // EditDefFileExt

BOOL DATB_DEF::EditRecIDObj     /* ---- Edit Record ID Object ------------- */
( HWND		hDlg,           	// window handle
  WORD		wCmd			// command
)
/* Edits Record ID Object. */
{
  if (wCmd != CBN_SELCHANGE)
    return FALSE;

  (void) hDlg;
  return TRUE;
} // EditRecIDObj
