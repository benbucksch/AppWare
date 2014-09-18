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
 | FILE		: QSQL_DEF.H                                              |
 | DESCRIPTION	: Q+E SQL Object Definition.                              |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit                         |
\*------------------------------------------------------------------------*/
#include "qsql.h"
#include "qsql_rc.h"
#include "strutl.h"
#include "memblock.h"
#include "safemem.h"
#include "a_almutl.h"
#include "helpids.h"


#define	EXTNAMESIZE	(MXVARSIZE * 2 + 4)

class QSQL_DEF                  /* ---- Q+E SQL Object Definition ------- */
{
public:
  /* ---- Q+E SQL Object dialog ----------------------------------------- */

  BOOL InitObjectDialog(HWND hDlg, OBJECTID qsqlObj);
  /* Inits Q+E SQL Object dialog. */

  BOOL UpdateObject(HWND hDlg);
  /* Updates object. */

  BOOL EditVars(HWND hDlg);
  /* Edits variables. */

  BOOL EditOptions(HWND hDlg);
  /* Edits options. */

  /* ---- Variables dialog ---------------------------------------------- */

  BOOL InitVarsDialog(HWND hDlg, OBJECTID qsqlObj);
  /* Inits variables. */

  BOOL InitObj(HWND hDlg, TYPEID type);
  /* Inits objects. */

  BOOL UpdateVars(HWND hDlg);
  /* Updates variables. */

  BOOL VarsList(HWND hDlg, LPARAM lParam);
  /* Variables list. */

  BOOL AddVar(HWND hDlg);
  /* Adds a new variable. */

  BOOL RemVar(HWND hDlg);
  /* Removes variable(s). */

  BOOL DisableSelVar(HWND hDlg);
  /* Disables Selected Variable group box. */

  BOOL EnableSelVar(HWND hDlg);
  /* Enables Selected Variable group box. */

  BOOL SelName(HWND hDlg, char *name);
  /* Selects name. */

  BOOL EditName(HWND hDlg, LPARAM lParam);
  /* Edits name. */

  BOOL SelType(HWND hDlg, OBJECTID obj);
  /* Selects type. */

  BOOL EditType(HWND hDlg, LPARAM lParam);
  /* Edits type. */

  BOOL SelObj(HWND hDlg, OBJECTID obj);
  /* Selects object. */

  BOOL EditObj(HWND hDlg, LPARAM lParam);
  /* Edits object. */

  BOOL EditObjectDialog(HWND hDlg);
  /* Edits object dialog. */

  /* ---- Options dialog ------------------------------------------------ */

  BOOL InitOptionsDialog(HWND hDlg, OBJECTID qsqlObj);
  /* Inits Options dialog. */

  BOOL UpdateOptions(HWND hDlg);
  /* Updates options. */

  BOOL EditRowCount(HWND hDlg, LPARAM lParam);
  /* Edits row count. */
};


/* ---- Q+E SQL Object dialog ------------------------------------------- */

BOOL ALMCALLBACK OQsqlObjectDialog/* --- Edit Object Dialog ------------- */
( HWND			hDlg,		// window handle
  UINT			message,	// message
  WPARAM		wParam,		// message
  LPARAM		lParam		// message
)
/* Q+E SQL Object dialog. */
{ QSQL_DEF		def;		// qsql_def class

  switch (message)
  { case WM_INITDIALOG:
      return def.InitObjectDialog(hDlg, (OBJECTID) lParam);
    case WM_COMMAND:
      switch (wParam)
      {	case IDHELP:
          #ifdef HELPID_OBJD_Qsql
	    return WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT,
			   HELPID_OBJD_Qsql);
	  #else
	    return FALSE;
          #endif
	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  return TRUE;
	case IDOK:
	  EndDialog(hDlg, TRUE);
	  return def.UpdateObject(hDlg);

	case IDD_VARS:
	  return def.EditVars(hDlg);
	case IDD_OPTIONS:
	  return def.EditOptions(hDlg);
	default:
          return FALSE;
      } // switch
    default:
      return FALSE;
  } // switch
} // EditObject

BOOL QSQL_DEF::InitObjectDialog	/* ---- Init Object Dialog -------------- */
( HWND                  hDlg,           // window handle
  OBJECTID              qsqlObj         // qsql object
)
/* Inits Q+E SQL Object dialog. */
{ MemBlock		mb;		// memblock class
  Qsql			*qsql;		// qsql class
  char			str[MXSTRSIZE];	// string

  long			objIdx;		// object index
  OBJECTID		obj;		// object
  char			name[MXVARSIZE];// name

  WORD			i;		// counter
  			
  SetWindowLong(hDlg, DWL_USER, (DWORD) qsqlObj);	// saves object id
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);

  /* ---- inits qsql object type name, and qsql object name ---- */
  AUtlSetTitleFont(OTYPE_QSQL, NULL, GetDlgItem(hDlg, IDD_OBJECT));
	AUtlCenterDialog(hDlg, 0);
  AObjGetName(qsqlObj, str, sizeof(str));
  SetWindowText(hDlg, str);

  /* ---- inits connection ---- */
  i = SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_INSERTSTRING,
                         -1, (DWORD) "(None)");
  SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_SETITEMDATA, i, 0);
  if (!qsql->connectObj ||
      !AObjCheckType(qsql->connectObj, OTYPE_QCNC))
    SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_SETCURSEL, i, 0);

  objIdx = AOBJ_GETFIRSTKEY;
  while((obj = AObjGetNextObject(OTYPE_QCNC, &objIdx)) != 0)
  { AObjGetName(obj, name, sizeof(name));
    i = SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_INSERTSTRING,
			   -1, (DWORD) name);
    SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_SETITEMDATA, i, (DWORD) obj);
    if (qsql->connectObj == obj)
      SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_SETCURSEL, i, 0);
  } // while

  /* ---- inits sql statement ---- */
  i = SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_INSERTSTRING,
                         -1, (DWORD) "(None)");
  SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_SETITEMDATA, i, 0);
  if (!qsql->sqlObj ||
      !AObjCheckType(qsql->sqlObj, OTYPE_TEXT))
    SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_SETCURSEL, i, 0);

  objIdx = AOBJ_GETFIRSTKEY;
  while((obj = AObjGetNextObject(OTYPE_TEXT, &objIdx)) != 0)
  { AObjGetName(obj, name, sizeof(name));
    i = SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_INSERTSTRING,
                           -1, (DWORD) name);
    SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_SETITEMDATA, i, (DWORD) obj);
    if (qsql->sqlObj == obj)
      SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_SETCURSEL, i, 0);
  } // while
  return TRUE;
} // InitObject

BOOL QSQL_DEF::UpdateObject	/* ---- Update Object Dialog ------------ */
( HWND                  hDlg            // window handle
)
/* Updates object dialog. */
{ MemBlock		mb;		// memblock class
  OBJECTID              qsqlObj;        // qsql object
  Qsql                  *qsql;          // qsql core info

  WORD			i;              // counter

  qsqlObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);	// gets object id
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);

  /* ----  updates connection ---- */
  i = SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_GETCURSEL, 0, 0);
  qsql->connectObj = (OBJECTID)
    SendDlgItemMessage(hDlg, IDC_CONNOBJ, CB_GETITEMDATA, i, 0);

  /* ---- updates sql statement ---- */
  i = SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_GETCURSEL, 0, 0);
  qsql->sqlObj = (OBJECTID)
    SendDlgItemMessage(hDlg, IDC_SQLOBJ, CB_GETITEMDATA, i, 0);
  return TRUE;
} // UpdateObject

BOOL QSQL_DEF::EditVars		/* ---- Edit Variables ------------------ */
( HWND                  hDlg		// window handle
)
/* Edits variables. */
{ OBJECTID		qsqlObj;	// qsql object

  qsqlObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);	// gets object id
  return DialogBoxParam(hInstance, "QsqlVars", hDlg,
	   OQsqlVarsDialog, (LPARAM) qsqlObj)? TRUE : FALSE;
} // EditVars

BOOL QSQL_DEF::EditOptions	/* ---- Edit Options -------------------- */
( HWND                  hDlg		// window handle
)
/* Edits options. */
{ OBJECTID		qsqlObj;	// qsql object

  qsqlObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);	// gets object id
  return DialogBoxParam(hInstance, "QsqlOptions", hDlg,
	   OQsqlOptionsDialog, (LPARAM) qsqlObj)? TRUE : FALSE;
} // EditOptions

/* ---- Variables dialog ------------------------------------------------ */

BOOL ALMCALLBACK OQsqlVarsDialog/* ---- Edit Variables Dialog ----------- */
( HWND                  hDlg,           // window handle
  UINT                  message,        // message
  WPARAM                wParam,		// message
  LPARAM                lParam		// message			
)
/* Edits Variables dialog. */
{ QSQL_DEF		def;		// qsql_def class

  switch (message)
  { case WM_INITDIALOG:
      return def.InitVarsDialog(hDlg, (OBJECTID) lParam);
    case WM_COMMAND:
      switch (wParam)
      {	case IDHELP:
          #ifdef HELPID_OBJD_QsqlVars
            return WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT,
			   HELPID_OBJD_QsqlVars);
          #else
	    return FALSE;
          #endif
	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  return FALSE;
        case IDOK:
	  EndDialog(hDlg, TRUE);
	  return def.UpdateVars(hDlg);

	case IDL_VARS:
	  return def.VarsList(hDlg, lParam);
	case IDD_ADDVAR:
	  return def.AddVar(hDlg);
	case IDD_REMVAR:
	  return def.RemVar(hDlg);
	case IDE_NAME:
	  return def.EditName(hDlg, lParam);
	case IDC_TYPE:
	  return def.EditType(hDlg, lParam);
	case IDC_OBJ:
	  return def.EditObj(hDlg, lParam);
	default:
          return FALSE;
      } // switch
    default:
      return FALSE;
  } // switch
} // EditVarsDialog

BOOL QSQL_DEF::InitVarsDialog	/* ---- Init Variables Dialog ----------- */
( HWND                  hDlg,           // window handle
  OBJECTID              qsqlObj         // qsql object
)
/* Inits Variables group box. */
{ MemBlock		mb;		// memblock class
  WORD                  varCount;       // number of VARs
  Var                   huge *varPtr;   // VAR pointer
  int			extNameLen;		// extended name length
  char			extName[EXTNAMESIZE];	// extended name

  long			typeIdx;	// type index
  TYPEID		type;		// type
  char			name[MXVARSIZE];// name

  WORD			i;              // counter

	AUtlCenterDialog(hDlg, 0);
  SetWindowLong(hDlg, DWL_USER, (DWORD) qsqlObj);	// saves object id
  varCount = (WORD) (AObjGetDataSize(qsqlObj, QSQLVARS) / sizeof(Var));
  varPtr = (Var *) mb.GetPointer(qsqlObj, QSQLVARS);

  /* ---- inits variable ---- */
  for (i = 0; i < varCount; i++, varPtr++)
  { if (varPtr->obj && !AObjCheckType(varPtr->obj, 0))
      varPtr->obj = 0;

    extNameLen = (int) hstrlen(varPtr->name);
    hmemcpy(extName, varPtr->name, extNameLen);
    if (varPtr->obj)
    { lstrcpy(&extName[extNameLen], ": <");
      ATypeGetName(varPtr->type, &extName[extNameLen + 3], sizeof(name));
      lstrcat(&extName[extNameLen], ">");
    } // if
    else
      lstrcpy(&extName[extNameLen], ": <(None)>");
    SendDlgItemMessage(hDlg, IDL_VARS, LB_INSERTSTRING, i, (DWORD) extName);
    SendDlgItemMessage(hDlg, IDL_VARS, LB_SETITEMDATA,
                       i, (DWORD) varPtr->obj);
  } // for
  /* inserts an empty mask */
  SendDlgItemMessage(hDlg, IDL_VARS, LB_INSERTSTRING, i, (DWORD) "");
  SendDlgItemMessage(hDlg, IDL_VARS, LB_SETITEMDATA, i, 0);

  /* ---- inits type ---- */ 
  i = SendDlgItemMessage(hDlg, IDC_TYPE, CB_INSERTSTRING,
                         -1, (DWORD) "(None)");
  SendDlgItemMessage(hDlg, IDC_TYPE, CB_SETITEMDATA, i, 0);
  typeIdx = AOBJ_GETFIRSTKEY;
  while ((type = ATypeGetNext(&typeIdx)) != 0)
    if (type == OTYPE_TEXT || type == OTYPE_NUMBER ||
	type == OTYPE_DATE || type == OTYPE_TIME)
    { ATypeGetName(type, name, sizeof(name));
      i = SendDlgItemMessage(hDlg, IDC_TYPE, CB_INSERTSTRING,
                             -1, (DWORD) name);
      SendDlgItemMessage(hDlg, IDC_TYPE, CB_SETITEMDATA, i, (DWORD) type);
    } // if
  return TRUE;
} // InitVars

BOOL QSQL_DEF::InitObj		/* ---- Init Object --------------------- */
( HWND                  hDlg,           // window handle
  TYPEID                type		// type
)
/* Inits object. */
{ long			objIdx;		// object index
  OBJECTID              obj;            // object
  char                  name[MXVARSIZE];// name

  WORD			i;              // counter

  if (!type)
  { EnableWindow(GetDlgItem(hDlg, IDS_OBJ), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_OBJ), FALSE);
    SendDlgItemMessage(hDlg, IDC_OBJ, CB_SETCURSEL, -1, 0);
    return FALSE;
  } // if
  EnableWindow(GetDlgItem(hDlg, IDS_OBJ), TRUE);
  EnableWindow(GetDlgItem(hDlg, IDC_OBJ), TRUE);
  if (SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETCOUNT, 0, 0) > 0)
  { obj = (OBJECTID) SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETITEMDATA,
					0, 0);
    if (type == AObjGetType(obj))	// it has been inited
    { if (SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETCURSEL, 0, 0) == CB_ERR)
        SendDlgItemMessage(hDlg, IDC_OBJ, CB_SETCURSEL, 0, 0);
      return FALSE;
    } // if
  } // if

  SendDlgItemMessage(hDlg, IDC_OBJ, CB_RESETCONTENT, 0, 0);
  objIdx = AOBJ_GETFIRSTKEY;
  while ((obj = AObjGetNextObject(type, &objIdx)) != 0)
  { AObjGetName(obj, name, sizeof(name));
    i = SendDlgItemMessage(hDlg, IDC_OBJ, CB_INSERTSTRING,
			   -1, (DWORD) name);
    SendDlgItemMessage(hDlg, IDC_OBJ, CB_SETITEMDATA, i, (DWORD) obj);
  } // while
  SendDlgItemMessage(hDlg, IDC_OBJ, CB_SETCURSEL, 0, 0);
  return TRUE;
} // InitObj

BOOL QSQL_DEF::UpdateVars	/* ---- Update Variables ---------------- */
( HWND                  hDlg            // window handle
)
/* Updates variables. */
{ MemBlock		mb;		// memblock class
  OBJECTID              qsqlObj;        // qsql object
  WORD                  varCount;       // number of VARs
  Var                   huge *varPtr;   // VAR pointer
  char			huge *namePtr;	// name pointer	
  char			extName[EXTNAMESIZE];	// extended name
  char			*extNamePtr;		// extended name pointer

  WORD			i;              // counter

  qsqlObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);	// gets object id	

  varCount = SendDlgItemMessage(hDlg, IDL_VARS, LB_GETCOUNT, 0, 0) - 1;
  MemResizeObjectData(qsqlObj, QSQLVARS, varCount * sizeof(Var));
  varPtr = (Var *) mb.GetPointer(qsqlObj, QSQLVARS);

  /* ---- updates variables ---- */
  for (i = 0; i < varCount; i++, varPtr++)
  { namePtr = varPtr->name;
    SendDlgItemMessage(hDlg, IDL_VARS, LB_GETTEXT, i, (DWORD) extName);
    extNamePtr = extName;
    while (*extNamePtr != ':')
      *namePtr++ = *extNamePtr++;
    *namePtr = 0;

    varPtr->obj = (OBJECTID)
      SendDlgItemMessage(hDlg, IDL_VARS, LB_GETITEMDATA, i, 0);
    varPtr->type = AObjGetType(varPtr->obj);
  } // for
  return TRUE;
} // UpdateVars

BOOL QSQL_DEF::VarsList		/* ---- Variables List ------------------ */
( HWND                  hDlg,           // window handle
  LPARAM		lParam		// message
)
/* Variables list. */
{ WORD                  selItemCount;   // number of selected items

  switch (HIWORD(lParam))
  { case LBN_SELCHANGE:
      selItemCount = SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELCOUNT,
					0, 0);
      if (selItemCount)
	if (SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSEL, (WORD)
	      SendDlgItemMessage(hDlg, IDL_VARS, LB_GETCOUNT, 0, 0) - 1, 0))
	  selItemCount--;		// ignores mask item

      if (selItemCount)
      { EnableWindow(GetDlgItem(hDlg, IDD_REMVAR), TRUE);
	if (selItemCount == 1)
	  EnableSelVar(hDlg);
	else
	  DisableSelVar(hDlg);
      } // if
      else
      { EnableWindow(GetDlgItem(hDlg, IDD_REMVAR), FALSE);
	DisableSelVar(hDlg);
      } // else
      return TRUE;
    case LBN_DBLCLK:
      selItemCount = SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELCOUNT,
                                        0, 0);
      if (selItemCount)
	if (SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSEL, (WORD) 
	      SendDlgItemMessage(hDlg, IDL_VARS, LB_GETCOUNT, 0, 0) - 1, 0))
          selItemCount--;               // ignores mask item

      if (selItemCount == 1)
        return EditObjectDialog(hDlg);
      return FALSE;
    default:
      return FALSE;
  } // switch
} // VarList

BOOL QSQL_DEF::AddVar		/* ---- Add Variable -------------------- */
( HWND                  hDlg            // window handle
)
/* Adds a new variable. */
{ WORD                  topIdx;         // the first visible item

  WORD			i;		// counter

  /* ---- finds the place to insert ---- */
  if (!SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELITEMS, 1, (DWORD) &i))
    i = (DWORD) SendDlgItemMessage(hDlg, IDL_VARS, LB_GETCOUNT, 0, 0) - 1;

  /* ---- inserts a new variable ---- */
  SendDlgItemMessage(hDlg, IDL_VARS, LB_INSERTSTRING,
                     i, (DWORD) "NewVar: <(None)>");
  SendDlgItemMessage(hDlg, IDL_VARS, LB_SETITEMDATA, i, 0);

  /* ---- makes the newly added variable visible ---- */
  topIdx = SendDlgItemMessage(hDlg, IDL_VARS, LB_GETTOPINDEX, 0, 0);
  if (i < topIdx)
    SendDlgItemMessage(hDlg, IDL_VARS, LB_SETTOPINDEX, i, 0);
  else
    if (i > topIdx + 9)
      SendDlgItemMessage(hDlg, IDL_VARS, LB_SETTOPINDEX, i - 9, 0);
  return TRUE;
} // AddVar

BOOL QSQL_DEF::RemVar		/* ---- Remove Variable ----------------- */
( HWND                  hDlg            // window handle
)
/* Removes variable(s). */
{ WORD                  selItemCount;   // number of selected items
  WORD                  *selItem;	// an array of selected items
  WORD			huge *selItemPtr;// selected item pointer

  WORD			i;		// counter

  EnableWindow(GetDlgItem(hDlg, IDD_REMVAR), FALSE);

  /* ---- finds the place(s) to delete ---- */
  selItemCount = SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELCOUNT, 0, 0);
  if (SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSEL, (WORD)
        SendDlgItemMessage(hDlg, IDL_VARS, LB_GETCOUNT, 0, 0) - 1, 0))
    selItemCount--;                     // ignores the mask item
  selItem = (WORD *) MemAlloc(selItemCount * sizeof(*selItem));
  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELITEMS,
		     selItemCount, (DWORD) selItem);

  /* ---- deletes the variable(s) ---- */
  for (selItemPtr = (WORD huge *) selItem + (selItemCount - 1), 
       i = 0; i < selItemCount; i++,
       selItemPtr--)
    SendDlgItemMessage(hDlg, IDL_VARS, LB_DELETESTRING, *selItemPtr, 0);

  MemFree(selItem);
  if (selItemCount == 1)
    DisableSelVar(hDlg);
  SetFocus(GetDlgItem(hDlg, IDL_VARS));	// keeps the focus
  return TRUE;
} // RemVar

BOOL QSQL_DEF::DisableSelVar	/* ---- Disable Selected Variable ------- */
( HWND                  hDlg            // window handle
)
/* Disables Selected Variable group box. */
{
  EnableWindow(GetDlgItem(hDlg, IDG_VAR), FALSE);

  EnableWindow(GetDlgItem(hDlg, IDS_NAME), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDE_NAME), FALSE);
  SetDlgItemText(hDlg, IDE_NAME, "");

  EnableWindow(GetDlgItem(hDlg, IDS_TYPE), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDC_TYPE), FALSE);
  SendDlgItemMessage(hDlg, IDC_TYPE, CB_SETCURSEL, -1, 0);

  EnableWindow(GetDlgItem(hDlg, IDS_OBJ), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDC_OBJ), FALSE);
  SendDlgItemMessage(hDlg, IDC_OBJ, CB_SETCURSEL, -1, 0);
  return TRUE;
} // DisableSelVar

BOOL QSQL_DEF::EnableSelVar	/* ---- Enable Selected Variable -------- */
( HWND                  hDlg            // window handle
)
/* Enables Selected Variable group box. */
{ OBJECTID              obj;            // object
  char                  extName[EXTNAMESIZE];	// extended name
  char			*extNamePtr;		// extended name ptr

  WORD			i;		// counter

  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELITEMS, 1, (DWORD) &i);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETTEXT, i, (DWORD) extName);
  obj = (OBJECTID) SendDlgItemMessage(hDlg, IDL_VARS, LB_GETITEMDATA, i, 0);

  EnableWindow(GetDlgItem(hDlg, IDG_VAR), TRUE);
  /* ---- trims type name ---- */
  for (extNamePtr = extName; *extNamePtr != ':'; extNamePtr++)
    /* Do Nothing */;
  *extNamePtr = 0;
  SelName(hDlg, extName);
  SelType(hDlg, obj);
  SelObj(hDlg, obj);
  return TRUE;
} // EnableSelVar

BOOL QSQL_DEF::SelName		/* ---- Select Name --------------------- */
( HWND                  hDlg,           // window handle
  char                  *name           // name
)
/* Selects name. */
{
  EnableWindow(GetDlgItem(hDlg, IDS_NAME), TRUE);
  EnableWindow(GetDlgItem(hDlg, IDE_NAME), TRUE);
  SetDlgItemText(hDlg, IDE_NAME, name);
  return TRUE;
} // SelName

BOOL QSQL_DEF::EditName		/* ---- Edit Name ----------------------- */
( HWND                  hDlg,           // window handle
  LPARAM                lParam		// message
)
/* Edits Name. */
{ OBJECTID              obj;            // object
  char			extName[EXTNAMESIZE];	// extended name
  char			*extNamePtr;		// extended name pointer
  StrUtl		s;		// strutl class
  char                  str[MXSTRSIZE];	// string

  WORD			i;		// counter

  if (HIWORD(lParam) != EN_KILLFOCUS)
    return FALSE;

  GetDlgItemText(hDlg, IDE_NAME, str, sizeof(str));
  s.TrimVar(str);			// trims invalid characters
  if (*str == 0)
    lstrcpy(str, "NewVar");		// doesn't allow empty name
  else
    str[MXVARSIZE - 1] = 0;		// truncates
  SetDlgItemText(hDlg, IDE_NAME, str);

  /* ---- update variables ---- */
  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELITEMS, 1, (DWORD) &i);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETTEXT, i, (DWORD) extName);
  obj = (OBJECTID) SendDlgItemMessage(hDlg, IDL_VARS, LB_GETITEMDATA, i, 0);
  for (extNamePtr = extName; *extNamePtr != ':'; extNamePtr++)
    /* Do Nothing */;
  lstrcat(str, extNamePtr);

  SendDlgItemMessage(hDlg, IDL_VARS, LB_DELETESTRING, i, 0);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_INSERTSTRING, i, (DWORD) str);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_SETITEMDATA, i, (DWORD) obj);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_SETSEL, TRUE, i);
  return TRUE;
} // EditName

BOOL QSQL_DEF::SelType		/* ---- Select Type --------------------- */
( HWND                  hDlg,           // window handle
  OBJECTID              obj             // object
)
/* Selects type. */
{ WORD                  typeCount;      // number of types
  TYPEID                type;           // type

  WORD                  i;              // counter

  EnableWindow(GetDlgItem(hDlg, IDS_TYPE), TRUE);
  EnableWindow(GetDlgItem(hDlg, IDC_TYPE), TRUE);

  type = AObjGetType(obj);
  typeCount = (WORD) SendDlgItemMessage(hDlg, IDC_TYPE, CB_GETCOUNT, 0, 0);
  for (i = 0; i < typeCount; i++)
    if (type == (TYPEID) SendDlgItemMessage(hDlg, IDC_TYPE, CB_GETITEMDATA,
                                            i, 0))
    { SendDlgItemMessage(hDlg, IDC_TYPE, CB_SETCURSEL, i, 0);
      break;
    } // if
  return TRUE;
} // SelType

BOOL QSQL_DEF::EditType		/* ---- Edit Type ----------------------- */
( HWND                  hDlg,           // window handle
  LPARAM                lParam		// message
)
/* Edits type. */
{ TYPEID                type;           // type
  OBJECTID		obj;		// object
  int			extNameLen;		// extended name length
  char			extName[EXTNAMESIZE];	// extended name

  WORD			i;		// counter		       

  if (HIWORD(lParam) != CBN_SELCHANGE)
    return FALSE;

  i = (WORD) SendDlgItemMessage(hDlg, IDC_TYPE, CB_GETCURSEL, 0, 0);
  type = (TYPEID) SendDlgItemMessage(hDlg, IDC_TYPE, CB_GETITEMDATA, i, 0);

  /* ---- updates variable ---- */
  InitObj(hDlg, type);
  extNameLen = GetDlgItemText(hDlg, IDE_NAME, extName, sizeof(extName));
  if (type)
  { i = (WORD) SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETCURSEL, 0, 0);
    obj = (OBJECTID) SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETITEMDATA,
                                        i, 0);
    lstrcpy(&extName[extNameLen], ": <");
    extNameLen += 3;
    extNameLen += GetDlgItemText(hDlg, IDC_TYPE, &extName[extNameLen],
                                 MXVARSIZE);
    lstrcpy(&extName[extNameLen], ">");
  } // if
  else
  { obj = 0;
    lstrcpy(&extName[extNameLen], ": <(None)>");
  } // else   

  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELITEMS, 1, (DWORD) &i);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_DELETESTRING, i, 0);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_INSERTSTRING, i, (DWORD) extName);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_SETITEMDATA, i, (DWORD) obj);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_SETSEL, TRUE, i);
  return TRUE;
} // EditType

BOOL QSQL_DEF::SelObj		/* ---- Select Object ------------------- */
( HWND                  hDlg,           // window handle
  OBJECTID              obj             // object
)
/* Selects object. */
{ WORD			objCount;       // number of objects

  WORD			i;              // counter

  InitObj(hDlg, AObjGetType(obj));
  objCount = (WORD) SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETCOUNT, 0, 0);
  for (i = 0; i < objCount; i++)
    if (obj == (OBJECTID) SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETITEMDATA,
                                             i, 0))
    { SendDlgItemMessage(hDlg, IDC_OBJ, CB_SETCURSEL, i, 0);
      break;
    } // if
  return TRUE;
} // SelObj

BOOL QSQL_DEF::EditObj		/* ---- Edit Object --------------------- */
( HWND                  hDlg,           // window handle
  LPARAM                lParam		// message
)
/* Edits object. */
{ OBJECTID              obj;            // object

  WORD			i;		// counter

  if (HIWORD(lParam) != CBN_SELCHANGE)
    return FALSE;

  i = SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETCURSEL, 0, 0);
  obj = (OBJECTID) SendDlgItemMessage(hDlg, IDC_OBJ, CB_GETITEMDATA, i, 0);

  /* ---- updates variable ---- */
  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELITEMS, 1, (DWORD) &i);
  SendDlgItemMessage(hDlg, IDL_VARS, LB_SETITEMDATA, i, (DWORD) obj);
  return TRUE;
} // EditObj

BOOL QSQL_DEF::EditObjectDialog	/* ---- Edit Object Dialog -------------- */
( HWND                  hDlg            // window handle
)
/* Edits object dialog. */
{ OBJECTID		obj;		// object

  WORD                  i;              // counter

  SendDlgItemMessage(hDlg, IDL_VARS, LB_GETSELITEMS, 1, (DWORD) &i);
  obj = (OBJECTID) SendDlgItemMessage(hDlg, IDL_VARS, LB_GETITEMDATA, i, 0);
  if (obj)
  { AObjEdit(obj);
    return TRUE;
  } // if
  return FALSE;
} // EditObject

/* ---- Options dialog -------------------------------------------------- */

BOOL ALMCALLBACK OQsqlOptionsDialog/* - Edit Options Dialog ------------- */
( HWND			hDlg,		// window handle
  UINT			message,	// message
  WPARAM		wParam,		// message
  LPARAM		lParam		// message
)
/* Edits Options dialog. */
{ QSQL_DEF		def;		// qsql_def class

  switch (message)
  { case WM_INITDIALOG:
      return def.InitOptionsDialog(hDlg, (OBJECTID) lParam);
    case WM_COMMAND:
      switch (wParam)
      { case IDHELP:
	  #ifdef HELPID_OBJD_QsqlOptions
	    return WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT,
                           HELPID_OBJD_QsqlOptions);
          #else
	    return FALSE;
          #endif
	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  return FALSE;
        case IDOK:
	  EndDialog(hDlg, TRUE);
	  return def.UpdateOptions(hDlg);

	case IDE_ROWCOUNT:
	  return def.EditRowCount(hDlg, lParam);
	case IDD_TAB:
	case IDD_COMMA:
	  CheckRadioButton(hDlg, IDD_TAB, IDD_COMMA, wParam);
          return TRUE;
	case IDD_OEM:
	  CheckDlgButton(hDlg, IDD_OEM, !IsDlgButtonChecked(hDlg, IDD_OEM));
	  return TRUE;
	default:
          return FALSE;
      } // switch
    default:
      return FALSE;
  } // switch
} // OQsqlOptionsDialog

BOOL QSQL_DEF::InitOptionsDialog/* ---- Init Options -------------------- */
( HWND			hDlg,		// window handle
  OBJECTID		qsqlObj		// qsql object
)
/* Inits options. */
{ MemBlock		mb;		// memblock class
  Qsql			*qsql;		// qsql core info

	AUtlCenterDialog(hDlg, 0);
  SetWindowLong(hDlg, DWL_USER, (DWORD) qsqlObj);	// saves object id
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);

  /* ---- inits row count ---- */
  SetDlgItemInt(hDlg, IDE_ROWCOUNT, (WORD) qsql->rowCount, FALSE);

  /* ---- inits row separator ---- */
  switch (qsql->colSprtr)
  { case TAB:
      CheckRadioButton(hDlg, IDD_TAB, IDD_COMMA, IDD_TAB);
      break;
    case COMMA:
      CheckRadioButton(hDlg, IDD_TAB, IDD_COMMA, IDD_COMMA);
      break;
    /* default: Fall Through */
  } // switch

  /* ---- inits oem ---- */
  CheckDlgButton(hDlg, IDD_OEM, qsql->isOEM);
  return TRUE;
} // InitOptions

BOOL QSQL_DEF::UpdateOptions	/* ---- Update Options ------------------ */
( HWND                  hDlg            // window handle
)
/* Updates options. */
{ MemBlock		mb;		// memblock class
  OBJECTID              qsqlObj;        // qsql object
  Qsql                  *qsql;          // qsql core info

  qsqlObj = (OBJECTID) GetWindowLong(hDlg, DWL_USER);	// gets object id    
  qsql = (Qsql *) mb.GetPointer(qsqlObj, QSQLCORE);

  /* ---- updates row count ---- */
  qsql->rowCount = GetDlgItemInt(hDlg, IDE_ROWCOUNT, NULL, FALSE);

  /* ---- updates column separator ---- */
  if (IsDlgButtonChecked(hDlg, IDD_TAB))
    qsql->colSprtr = TAB;
  else
    if (IsDlgButtonChecked(hDlg, IDD_COMMA))
      qsql->colSprtr = COMMA;
    /* else Fall Through */

  /* ---- updates oem ---- */
  qsql->isOEM = IsDlgButtonChecked(hDlg, IDD_OEM);
  return TRUE;
} // UpdateOptions

BOOL QSQL_DEF::EditRowCount	/* ---- Edit Row Count ------------------ */
( HWND                  hDlg,           // window handle
  LPARAM                lParam		// message
)
/* Edits row count. */
{ WORD			rowCount;       // number of rows

  if (HIWORD(lParam) != EN_KILLFOCUS)
    return FALSE;

  rowCount = GetDlgItemInt(hDlg, IDE_ROWCOUNT, NULL, FALSE);
  if (rowCount == 0)
    rowCount = 1;
  SetDlgItemInt(hDlg, IDE_ROWCOUNT, rowCount, FALSE);
  return TRUE;
} // EditRowCount
