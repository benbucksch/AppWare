/*--------------------------------------------------------------------------*\
 | FILE         : DATB_DEF.CPP                                              |
 | DESCRIPTION  : Database Object Definition.                               |
 | COPYRIGHT    : 1993 Novell, Inc.                                         |
 | EDIT LOG     :                                                           |
 |   v1.00.00     mw    01-Sep-1993  Initial Edit                           |
\*--------------------------------------------------------------------------*/
#include "datb.h"
#include "datb_rc.h"
#include "dbkey.h"
#include "memblock.h"
#include "safemem.h"
#include "strutl.h"
#include "a_almutl.h"
#include "helpids.h"


class DATB_DEF                  /* ---- Database Object Definition -------- */
{
public:
  BOOL InitObjectDialog(HWND hDlg, OBJECTID datbObj);
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

  BOOL AvailableObject(HWND hDlg, LONG lParam);
  /* Available Object. */

  BOOL SelectedObject(HWND hDlg, LPARAM lParam);
  /* Selected Object. */

  BOOL AddObject(HWND hDlg);
  /* Adds objects from Available Objects to Objects in Record list. */

  BOOL RemoveObject(HWND hDlg);
  /* Removes objects from Objects in Record. */

  BOOL EditObject(HWND hDlg);
  /* Edits object in Objects in Record. */

  BOOL EditFieldName(HWND hDlg, LPARAM lParam);
  /* Edits Field Name. */

  BOOL EditIndexed(HWND hDlg);
  /* Edits Indexed. */

  BOOL EditKeyType(HWND hDlg, LPARAM lParam);
  /* Edits Key Type. */

  BOOL EditCharacters(HWND hDlg, LPARAM lParam);
  /* Edits Characters. */

  BOOL EditDefFileExt(HWND hDlg, LPARAM lParam);
  /* Edits Default File Extension. */

  BOOL EditRecIDObj(HWND hDlg, LPARAM lParam);
  /* Edits Record ID Object. */
};

/* ---- Database Object dialog box ---------------------------------------- */

BOOL ALMAPI ODatbObjectDialog   /* ---- Database Object Dialog ------------ */
( HWND                  hDlg,           // window handle
  UINT                  message,        // message
  WPARAM                wParam,         // message
  LPARAM                lParam          // message
)
/* Database Object dialog box. */
{ DATB_DEF              def;            // datb_def class

  switch (message)
  { case WM_INITDIALOG:
      return def.InitObjectDialog(hDlg, (OBJECTID) lParam);
    case WM_COMMAND:
      switch (wParam)
      { case IDHELP:
	  return WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Datb);
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
          return def.InitAvailableObject(hDlg, wParam);
        case IDL_AVAILABLE:
          return def.AvailableObject(hDlg, lParam);
        case IDL_SELECTED:
          return def.SelectedObject(hDlg, lParam);
	case IDD_ADD:
	  return def.AddObject(hDlg);
	case IDD_REMOVE:
	  return def.RemoveObject(hDlg);
	case IDD_EDIT:
	  return def.EditObject(hDlg);
	case IDE_FIELDNAME:
	  return def.EditFieldName(hDlg, lParam);
	case IDD_INDEXED:
	  return def.EditIndexed(hDlg);
	case IDC_KEYTYPE:
	  return def.EditKeyType(hDlg, lParam);
        case IDE_CHARS:
          return def.EditCharacters(hDlg, lParam);
        case IDE_EXTENSION:
          return def.EditDefFileExt(hDlg, lParam);
        case IDC_RECORDID:
          return def.EditRecIDObj(hDlg, lParam);
      } // switch
  } // switch
  return FALSE;
} // DatbObjectDialog

BOOL DATB_DEF::InitObjectDialog /* ---- Init Object Dialog ---------------- */
( HWND                  hDlg,           // window handle
  OBJECTID              datbObj         // datb object
)
/* Inits Database Object dialog box. */
{ MemBlock              mb;             // memblock class
  char                  str[MXSTRSIZE];	// string
  Datb                  *datb;          // datb core info
  char			name[MXVARSIZE];// name
  int                   fieldCount;     // number of FIELDs
  Field                 huge *field;    // an array of FIELDs
  Field                 *fieldPtr;      // field pointer

  long                  objIdx;         // object index
  OBJECTID              obj;            // object

  int                   i, j;           // counters

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
      hmemcpy(fieldPtr, &field[i], sizeof(Field));

      j = (int) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING,
                                   -1, (long) name);
      SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA,
                         j, (long) fieldPtr);
    } // if
  /* inserts an empty mask */
  j = (int) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_INSERTSTRING,
			       -1, (long) "");
  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_SETITEMDATA, j, 0);

  /* ---- inits Default File Extension ---- */
  SetDlgItemText(hDlg, IDE_EXTENSION, datb->defFileExt);

  /* ---- inits Record ID Object ---- */
  i = (int) SendDlgItemMessage(hDlg, IDC_RECORDID, CB_INSERTSTRING,
                               -1, (long) "(None)");
  SendDlgItemMessage(hDlg, IDC_RECORDID, CB_SETITEMDATA, i, 0);
  if (!datb->recIDObj)
    SendDlgItemMessage(hDlg, IDC_RECORDID, CB_SETCURSEL, i, 0);

  objIdx = AOBJ_GETFIRSTKEY;
  while((obj = AObjGetNextObject(OTYPE_NUMBER, &objIdx)) != 0)
  { AObjGetName(obj, name, sizeof(name));

    i = (int) SendDlgItemMessage(hDlg, IDC_RECORDID, CB_INSERTSTRING,
				 -1, (long) name);
    SendDlgItemMessage(hDlg, IDC_RECORDID, CB_SETITEMDATA, i, (long) obj);
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
  OBJECTID              datbObj;        // datb object
  Datb                  *datb;          // datb core info
  int                   fieldCount;     // number of FIELDs
  Field                 huge *field;    // an array of FIELDs
  Field                 *fieldPtr;      // field pointer

  int                   i;              // counter

  datbObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);
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
  datb->recIDObj = (OBJECTID)
    SendDlgItemMessage(hDlg, IDC_RECORDID, CB_GETITEMDATA, i, 0);

  /* ---- updates FIELDs ---- */
  for (i = 0; i < fieldCount; i++)
  { fieldPtr = (Field *)
      SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA, i, 0);
    hmemcpy(&field[i], fieldPtr, sizeof(Field));
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
  OBJECTID              datbObj;        // datb object
  DBKey                 dbKey;          // dbkey class

  int                   i;              // counter

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
                                       i, 0);
  if (field->keyType == ODATB_KEYQUERY)
  { datbObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);
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
( HWND                  hDlg,           // window handle
  WORD                  buttonID        // button id
)
/* Inits Available Objects list box. */
{ char                  name[MXVARSIZE];// name
  long                  typeIdx;        // type index
  TYPEID                type;           // type
  long                  objIdx;         // object index
  OBJECTID              obj;            // object

  int                   i;              // counter

  CheckRadioButton (hDlg, IDD_TEXT, IDD_OTHER, buttonID);

  EnableWindow(GetDlgItem(hDlg, IDD_ADD), FALSE);

  SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_RESETCONTENT, 0, 0);
  switch (buttonID)
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
					 -1, (long) name);
	    SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA,
			       i, (long) obj);
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
                                 -1, (long) name);
    SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_SETITEMDATA, i, (long) obj);
  } // while
  return TRUE;
} // InitAvailableObject

BOOL DATB_DEF::AvailableObject  /* ---- Available Object ------------------ */
( HWND                  hDlg,           // window handle
  LONG                  lParam          // message
)
/* Available Object. */
{
  switch (HIWORD(lParam))
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
( HWND                  hDlg,           // window handle
  LONG                  lParam          // message
)
/* Selected Object. */
{ int                   selItemCount;   // number of selected items

  switch (HIWORD(lParam))
  { case LBN_SELCHANGE:
      selItemCount = (int)
        SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELCOUNT, 0, 0);
      if (selItemCount)
        if (SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSEL, (int)
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
        if (SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSEL, (int)
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
{ OBJECTID              datbObj;        // datbObj
  DBKey                 dbKey;          // dbkey class

  int                   itemCount;      // number of items
  int                   itemIdx;        // item index
  int                   topIdx;         // top index

  int                   selItemCount;   // number of selected items
  int                   *selItem;       // an array of selected items
  OBJECTID              obj;            // object
  Field                 *field;         // field

  int                   i;              // counter

  datbObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);

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
  { obj = (OBJECTID) SendDlgItemMessage(hDlg, IDL_AVAILABLE, LB_GETITEMDATA,
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
( HWND                  hDlg,           // window handle
  LPARAM                lParam          // message
)
/* Edits Field Name. */
{ Field                 *field;         // field

  StrUtl                s;              // strutl class
  char                  str[MXSTRSIZE]; // string

  int                   i;              // counter

  if (HIWORD(lParam) != EN_KILLFOCUS)
    return FALSE;

  GetDlgItemText(hDlg, IDE_FIELDNAME, str, sizeof(str));
  s.TrimVar(str);                       // trims invalid characters
  if (*str == 0)
    lstrcpy(str, DEF_VARNAME);          // doesn't allow empty name
  else
    str[sizeof(str) - 1] = 0;           // truncates to MXVARLEN
  SetDlgItemText(hDlg, IDE_FIELDNAME, str);

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
				       i, 0);
  lstrcpy(field->name, str);
  return TRUE;
} // EditFieldName

BOOL DATB_DEF::EditIndexed      /* ---- Edit Indexed ---------------------- */
( HWND                  hDlg            // window handle
)
/* Edits Indexed. */
{ OBJECTID              datbObj;        // datb object
  DBKey                 dbKey;          // dbkey class
  BOOL                  isChecked;      // is checked?
  Field                 *field;         // field

  int                   i;              // counter

  isChecked = !IsDlgButtonChecked(hDlg, IDD_INDEXED);
  CheckDlgButton(hDlg, IDD_INDEXED, isChecked);

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
                                       i, 0);
  if (isChecked)
  { datbObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);
    field->keyType = dbKey.GetKeyType(datbObj, field->obj, field->type);
  } // if
  else
    field->keyType = ODATB_KEYQUERY;
  field->keySize = dbKey.GetKeySize(field->keyType);
  EnableField(hDlg);
  return TRUE;
} // EditIndexed

BOOL DATB_DEF::EditKeyType      /* ---- Edit Key Type --------------------- */
( HWND                  hDlg,           // window handle
  LONG                  lParam          // message
)
/* Edits Key Type. */
{ Field                 *field;         // field

  int                   i;              // counter

  if (HIWORD(lParam) != CBN_SELCHANGE)
    return FALSE;

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
                                       i, 0);
  i = (int) SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_GETCURSEL, 0, 0);
  field->keyType = (int)
    SendDlgItemMessage(hDlg, IDC_KEYTYPE, CB_GETITEMDATA, i, 0);
  return TRUE;
} // EditKeyType

BOOL DATB_DEF::EditCharacters   /* ---- Edit Characters ------------------- */
( HWND                  hDlg,           // window handle
  LONG                  lParam          // message
)
/* Edits Characters. */
{ Field                 *field;         // field

  int                   i;              // counter

  if (HIWORD(lParam) != EN_KILLFOCUS)
    return FALSE;

  SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETSELITEMS, 1, (long) &i);
  field = (Field *) SendDlgItemMessage(hDlg, IDL_SELECTED, LB_GETITEMDATA,
                                       i, 0);
  field->keySize = GetDlgItemInt(hDlg, IDE_CHARS, NULL, FALSE) + 1;
  if (field->keySize > ODATB_MXKEYSIZE)
    field->keySize = ODATB_MXKEYSIZE;
  SetDlgItemInt(hDlg, IDE_CHARS, field->keySize - 1, FALSE);
  return TRUE;
} // EditCharacters

BOOL DATB_DEF::EditDefFileExt   /* ---- Edit Default File Extension ------- */
( HWND                  hDlg,           // window handle
  LONG                  lParam          // message
)
/* Edits Default File Extension. */
{ char                  s[FILEEXTSIZE]; // buffer

  if (HIWORD(lParam) != EN_KILLFOCUS)
    return FALSE;

  GetDlgItemText(hDlg, IDE_EXTENSION, s, sizeof(s));
  SetDlgItemText(hDlg, IDE_EXTENSION, s);
  return TRUE;
} // EditDefFileExt

BOOL DATB_DEF::EditRecIDObj     /* ---- Edit Record ID Object ------------- */
( HWND                  hDlg,           // window handle
  LONG                  lParam          // message
)
/* Edits Record ID Object. */
{
  if (HIWORD(lParam) != CBN_SELCHANGE)
    return FALSE;

  (void) hDlg;
  return TRUE;
} // Edit RecIDObj
