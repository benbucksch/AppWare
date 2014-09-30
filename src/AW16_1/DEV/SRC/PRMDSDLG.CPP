#include <memory.h>

#include "almsys.h"
#include "cfglib.h"
#include "prmdsdlg.h"
#include "params.h"
#include "serblk.h"

BOOL I_CALLBACK ParamDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);


#define	MAXNAMECHARS	80

#define BN_INSERT			101
#define BN_REMOVE			119
#define BN_UPDATE			121
#define BN_TEXT			 	130
#define BN_OPTIONAL		129

#define SLE_NAME	   	123

#define CB_OBJECT		 	134

#define LB_PARAMS		 	122

#define GP_PARAMS			128

static HINSTANCE		H_APP_INSTANCE;
static HINSTANCE		H_PREV_INSTANCE;
static short				TYPE_FLAG;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ParamDescDlgBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ParamDescDlgBox(
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	HWND					hWndParent,
	LPSERIUSARRAY	parameters,
	short					typeFlag)
	{
	DLGPROC	 lpProc;
	BOOL		 result;

	TYPE_FLAG = typeFlag;
	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)ParamDescDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "PARAM_DESC_DLG", hWndParent, lpProc,
	 (DWORD)parameters);
	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCurrentParamData
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void SetCurrentParamData(
	HWND							hDlg,
	LPPARAMDESCARRAY	params)
	{
	int			itemIndex;
	HWND		typesList;
	short		i;
	short		count;

	itemIndex = SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0L);

	if (itemIndex == LB_ERR ||
//			((itemIndex+1) == SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT 0, 0)) ||
			itemIndex >= params->GetCount())
		{
		SendDlgItemMessage(hDlg, SLE_NAME, WM_SETTEXT, 0, (long)"");
		SendDlgItemMessage(hDlg, CB_OBJECT, CB_SETCURSEL, 0, 0L);
		SendDlgItemMessage(hDlg, BN_TEXT, BM_SETCHECK, FALSE, 0L);
		SendDlgItemMessage(hDlg, BN_OPTIONAL, BM_SETCHECK, FALSE, 0L);
		}
	else
		{
		SendDlgItemMessage(hDlg, SLE_NAME, WM_SETTEXT, 0,
		 (long)(LPSTR)(*params)[itemIndex].name);

		SendDlgItemMessage(hDlg, BN_TEXT, BM_SETCHECK,
		 ((*params)[itemIndex].flags & PARAM_NOTEMPOBJECTS), 0L);
		SendDlgItemMessage(hDlg, BN_OPTIONAL, BM_SETCHECK,
		 ((*params)[itemIndex].flags & PARAM_OPTIONAL), 0L);

		typesList = GetDlgItem(hDlg, CB_OBJECT);

		count = SendMessage(typesList, CB_GETCOUNT, 0, 0L);

		for (i=0; i<count; i++)
			if ((*params)[itemIndex].type ==
					SendMessage(typesList, CB_GETITEMDATA, i, 0L))
				{
				SendMessage(typesList, CB_SETCURSEL, i, 0L);
				break;
				}

		if (i == count)
			{
			MessageBox(hDlg, "Currently specified parameter type does not exist.",
			 "Type Not Found", MB_OK|MB_ICONEXCLAMATION);
			SendMessage(typesList, CB_SETCURSEL, -1, 0L);
			}

		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDialog
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long InitDialog(
	HWND							hDlg,
	LPPARAMDESCARRAY	params)
	{
	long	 itemID;
	int		 paramCount;
	int		 i;

	if (TYPE_FLAG == DO_INPUTS)
		{
		SendMessage(hDlg, WM_SETTEXT, 0, (long)"Input Parameters");
		SendDlgItemMessage(hDlg, GP_PARAMS, WM_SETTEXT, 0, (long)"Inputs");
		}
	else
		{
		SendMessage(hDlg, WM_SETTEXT, 0, (long)"Output Parameters");
		SendDlgItemMessage(hDlg, GP_PARAMS, WM_SETTEXT, 0, (long)"Outputs");
		}

	SendDlgItemMessage(hDlg, SLE_NAME, EM_LIMITTEXT, MAXNAMECHARS, 0L);

	cfgInitObjectList(GetDlgItem(hDlg, CB_OBJECT), "Untyped");

	paramCount = params->GetCount();
	for (i=0; i<paramCount; i++)
		SendDlgItemMessage(hDlg, LB_PARAMS, LB_ADDSTRING, 0,
		 (long)(LPSTR)(*params)[i].name);

	itemID = SendDlgItemMessage(hDlg, LB_PARAMS, LB_ADDSTRING, 0, (long)"");

	SendDlgItemMessage(hDlg, LB_PARAMS, LB_SETCURSEL, itemID, 0L);

	SetCurrentParamData(hDlg, params);
  
  return itemID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetCurrentParamData
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void GetCurrentParamData(
	HWND							hDlg,
  COMPUID           &typeID,
  unsigned long     &flags,
  LPSTR             szName)
	{
	int 						  typeIndex;
	
	flags = 0x00000000;

	if (0 != SendDlgItemMessage(hDlg, BN_OPTIONAL, BM_GETCHECK, 0, 0L))
		flags |= PARAM_OPTIONAL;

	if (0 != SendDlgItemMessage(hDlg, BN_TEXT, BM_GETCHECK, 0, 0L))
		flags |= PARAM_NOTEMPOBJECTS;

	SendDlgItemMessage(hDlg, SLE_NAME, WM_GETTEXT, MAXNAMECHARS+1,
	 (long)szName);

	typeIndex =  SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETCURSEL, 0, 0L);

	if (typeIndex == CB_ERR)
		typeID = COMPUID_GENERAL;
	else
		typeID =
		 SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETITEMDATA, typeIndex, 0L);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InsertParam
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InsertParam(
	HWND							hDlg,
	LPPARAMDESCARRAY	params)
	{
	unsigned long		flags;
	char						szName[MAXNAMECHARS+1];
	COMPUID					typeID;
	LPPARAMDESC			tempParam;
  long            paramIndex;
  
  GetCurrentParamData(hDlg, typeID, flags, szName);

  paramIndex = SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0L);

	if (paramIndex == LB_ERR) //insert at end of list if nothing selected
		paramIndex =
		 SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT, 0, 0L) - 1;
     
	params->Insert(paramIndex, szName, "", typeID, flags);

	SendDlgItemMessage(hDlg, LB_PARAMS, LB_INSERTSTRING, paramIndex,
	 (long)szName);
   
	SendDlgItemMessage(hDlg, LB_PARAMS, LB_SETCURSEL, paramIndex+1, 0);
  
	if (paramIndex+2 <
			SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT, 0, 0))
		SetCurrentParamData(hDlg, params);
    
	SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, MAKELONG(0, 32767));
	
	SetFocus(GetDlgItem(hDlg, SLE_NAME));
  
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateParam
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateParam(
	HWND							hDlg,
	LPPARAMDESCARRAY	params,
	long              paramIndex,
	BOOL              reselectFlag = TRUE)
	{
	unsigned long		flags = 0x00000000;
	char						szName[MAXNAMECHARS+1];
	COMPUID					typeID;
	LPPARAMDESC			tempParam;

  GetCurrentParamData(hDlg, typeID, flags, szName);
  
	if (paramIndex == LB_ERR ||
		  paramIndex+1 == SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT,0, 0L))
		MessageBeep(0);
	else
		{
		tempParam = (LPPARAMDESC)params->GetElement(paramIndex);
		tempParam->name = szName;
		tempParam->type = typeID;
		tempParam->flags = flags;
		SendDlgItemMessage(hDlg, LB_PARAMS, LB_DELETESTRING, paramIndex, 0L);
		SendDlgItemMessage(hDlg, LB_PARAMS, LB_INSERTSTRING, paramIndex,
		 (long)szName);
		if (reselectFlag == TRUE)
		  SendDlgItemMessage(hDlg, LB_PARAMS, LB_SETCURSEL, paramIndex, 0L);
		}

	SetFocus(GetDlgItem(hDlg, LB_PARAMS));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: QuerySaveChanges
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL QuerySaveChanges(
	HWND	hDlg)
	{
	int 	response;
	BOOL	result;

	response = MessageBox(hDlg, "Save Changes to current parameter?",
	 "Save Changes", MB_YESNO|MB_ICONQUESTION);

	if (response == IDYES)
		result = TRUE;
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DeleteCurrentParam
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DeleteCurrentParam(
	HWND							hDlg,
	LPPARAMDESCARRAY	params)
	{
	int			itemIndex;

	itemIndex = SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0L);

	if (itemIndex == LB_ERR ||
			((itemIndex+1) == SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT, 0, 0)) ||
			itemIndex >= params->GetCount())
		{
		MessageBeep(0);
		}
	else
		{
		if (params->Delete(itemIndex))
		 {
		 SendDlgItemMessage(hDlg, LB_PARAMS, LB_DELETESTRING, itemIndex, 0L);
     SendDlgItemMessage(hDlg, LB_PARAMS, LB_SETCURSEL, itemIndex, 0L);
		 SetCurrentParamData(hDlg, params);
		 }
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ParamDescDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK ParamDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPPARAMDESCARRAY	params;
	static BOOL							paramIsDirty;
  static long             paramIndex;
  long                    newParamIndex;
	BOOL										msgHandled = TRUE;

	switch (message)
		{
		case WM_INITDIALOG:
			params = (LPPARAMDESCARRAY)lParam;
			paramIndex = InitDialog(hDlg, params);
			paramIsDirty = FALSE;
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if ((paramIsDirty == TRUE) &&
					    (paramIndex != -1) && 
					    ((paramIndex+1) != 
					     SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT, 0, 0)) &&
					    QuerySaveChanges(hDlg)
					   )
						UpdateParam(hDlg, params, paramIndex, FALSE);
					EndDialog(hDlg, TRUE);
					break;

				case BN_INSERT:
					InsertParam(hDlg, params);
          paramIndex = 
           SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0L);
					paramIsDirty = FALSE;
					break;

				case BN_REMOVE:
					DeleteCurrentParam(hDlg, params);
					paramIsDirty = FALSE;
					break;

				case BN_UPDATE:
					if (paramIsDirty == TRUE)
						UpdateParam(hDlg, params,
						 SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0));
          paramIndex = 
           SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0L);
					paramIsDirty = FALSE;
					break;

				case BN_OPTIONAL:
				case BN_TEXT:
					if (HIWORD(lParam) == BN_CLICKED)
						{
						paramIsDirty = TRUE;
						if (0 == SendMessage((HWND) LOWORD(lParam), BM_GETCHECK, 0, 0L))
							SendMessage((HWND) LOWORD(lParam), BM_SETCHECK, TRUE, 0L);
						else
							SendMessage((HWND) LOWORD(lParam), BM_SETCHECK, FALSE, 0L);
						}
					break;

				case LB_PARAMS:
					if (HIWORD(lParam) == LBN_SELCHANGE)
						{
						if ((paramIsDirty == TRUE) &&
						    (paramIndex != -1) && 
						    ((paramIndex+1) != 
						     SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT, 0, 0)) &&
						    QuerySaveChanges(hDlg)
						   )
              {
              newParamIndex = 
  	           SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0L);
							UpdateParam(hDlg, params, paramIndex, FALSE);
              if ((newParamIndex+1) ==  
  						     SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCOUNT, 0, 0))
  	            SendDlgItemMessage(hDlg, LB_PARAMS, LB_SETCURSEL, newParamIndex, 0L);
              }
	          paramIndex = 
	           SendDlgItemMessage(hDlg, LB_PARAMS, LB_GETCURSEL, 0, 0L);
            SetCurrentParamData(hDlg, params);
						paramIsDirty = FALSE;
						}
					break;

				case SLE_NAME:
					if (HIWORD(lParam) == EN_CHANGE)
						paramIsDirty = TRUE;
					break;

				case CB_OBJECT:
					if (HIWORD(lParam) == CBN_SELCHANGE)
						paramIsDirty = TRUE;
					break;

				default:
					msgHandled = FALSE;
					break;
				}
			break;

		default:
			msgHandled = FALSE;
			break;

		}

	return msgHandled;

	}

