#include "almsys.h"
#include <windowsx.h>

#include "cfglib.h"
#include "prmdsdlg.h"
#include "params.h"
#include "serblk.h"

BOOL CALLBACK ParamDescDlgProc(
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
	BOOL		 result;

	TYPE_FLAG = typeFlag;
	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	result = DialogBoxParam(hAppInstance, "PARAM_DESC_DLG", hWndParent,
												(DLGPROC)ParamDescDlgProc, (DWORD)parameters);
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
	HWND		hParams			= GetDlgItem(hDlg, LB_PARAMS);
	HWND		hName				= GetDlgItem(hDlg, SLE_NAME);
	HWND		typesList		= GetDlgItem(hDlg, CB_OBJECT);
	HWND		hText				= GetDlgItem(hDlg, BN_TEXT);
	HWND		hOptional		= GetDlgItem(hDlg, BN_OPTIONAL);
	int			itemIndex;
	short		i;
	int			count;

	itemIndex = ListBox_GetCurSel(hParams);

	if (itemIndex == LB_ERR ||
//			((itemIndex+1) == ListBox_GetCount(hParams)) ||
			itemIndex >= params->GetCount())
		{
		Edit_SetText(hName, "");
		ComboBox_SetCurSel(typesList, 0);
		Button_SetCheck(hText, FALSE);
		Button_SetCheck(hOptional, FALSE);
		}
	else
		{
		Edit_SetText(hName, (LPCSTR)(*params)[itemIndex].name);

		Button_SetCheck(hText, ((*params)[itemIndex].flags & PARAM_NOTEMPOBJECTS));
		Button_SetCheck(hOptional, ((*params)[itemIndex].flags & PARAM_OPTIONAL));

		count = ComboBox_GetCount(typesList);

		for (i=0; i<count; i++)
			if ((*params)[itemIndex].type == ComboBox_GetItemData(typesList, i))
				{
				ComboBox_SetCurSel(typesList, i);
				break;
				}

		if (i == count)
			{
			MessageBox(hDlg, "Currently specified parameter type does not exist.",
			 "Type Not Found", MB_OK|MB_ICONEXCLAMATION);
			ComboBox_SetCurSel(typesList, -1);
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
	HWND		hParams		= GetDlgItem(hDlg, GP_PARAMS);
	HWND		hlbParams	= GetDlgItem(hDlg, LB_PARAMS);
	long	 itemID;
	int		 paramCount;
	int		 i;

	if (TYPE_FLAG == DO_INPUTS)
		{
		SetWindowText(hDlg, "Input Parameters");
		Button_SetText(hParams, "Inputs");
		}
	else
		{
		SetWindowText(hDlg, "Output Parameters");
		Button_SetText(hParams, "Outputs");
		}

	Edit_LimitText(GetDlgItem(hDlg, SLE_NAME), MAXNAMECHARS);

	cfgInitObjectList(GetDlgItem(hDlg, CB_OBJECT), "Untyped");

	paramCount = params->GetCount();
	for (i=0; i<paramCount; i++)
		ListBox_AddString(hlbParams, (LPCSTR)(*params)[i].name);

	itemID = ListBox_AddString(hlbParams, "");

	ListBox_SetCurSel(hlbParams, itemID);

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
	HWND							hObject		= GetDlgItem(hDlg, CB_OBJECT);
	int 						  typeIndex;
	
	flags = 0x0;

	if (0 != Button_GetCheck(GetDlgItem(hDlg, BN_OPTIONAL)))
		flags |= PARAM_OPTIONAL;

	if (0 != Button_GetCheck(GetDlgItem(hDlg, BN_TEXT)))
		flags |= PARAM_NOTEMPOBJECTS;

	Edit_GetText(GetDlgItem(hDlg, SLE_NAME), szName, MAXNAMECHARS+1);

	typeIndex =  ComboBox_GetCurSel(hObject);

	if (typeIndex == CB_ERR)
		typeID = COMPUID_GENERAL;
	else
		typeID = ComboBox_GetItemData(hObject, typeIndex);
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
	HWND						hParams			= GetDlgItem(hDlg, LB_PARAMS);
	HWND						hName				= GetDlgItem(hDlg, SLE_NAME);
	unsigned long		flags;
	char						szName[MAXNAMECHARS+1];
	COMPUID					typeID;
  long            paramIndex;
  
  GetCurrentParamData(hDlg, typeID, flags, szName);

  paramIndex = ListBox_GetCurSel(hParams);

	if (paramIndex == LB_ERR) //insert at end of list if nothing selected
		paramIndex = ListBox_GetCount(hParams) - 1;
     
	params->Insert(paramIndex, szName, "", typeID, flags);

	ListBox_InsertString(hParams, paramIndex,	szName);
   
	ListBox_SetCurSel(hParams, paramIndex+1);
  
	if (paramIndex+2 < ListBox_GetCount(hParams))
		SetCurrentParamData(hDlg, params);
    
	Edit_SetSel(hName, 0, 32767);
	
	SetFocus(hName);
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
	HWND							hParams						= GetDlgItem(hDlg, LB_PARAMS);
	unsigned long		flags = 0x00000000;
	char						szName[MAXNAMECHARS+1];
	COMPUID					typeID;
	LPPARAMDESC			tempParam;

  GetCurrentParamData(hDlg, typeID, flags, szName);
  
	if (paramIndex == LB_ERR || paramIndex+1 == ListBox_GetCount(hParams))
		MessageBeep(0);
	else
		{
		tempParam = (LPPARAMDESC)params->GetElement(paramIndex);
		tempParam->name = szName;
		tempParam->type = typeID;
		tempParam->flags = flags;
		ListBox_DeleteString(hParams, paramIndex);
		ListBox_InsertString(hParams, paramIndex, szName);
		if (reselectFlag == TRUE)
		  ListBox_SetCurSel(hParams, paramIndex);
		}

	SetFocus(hParams);
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
	HWND						hParams		= GetDlgItem(hDlg, LB_PARAMS);
	int			itemIndex;

	itemIndex = ListBox_GetCurSel(hParams);

	if (itemIndex == LB_ERR ||
			((itemIndex+1) == ListBox_GetCount(hParams)) ||
			itemIndex >= params->GetCount())
		{
		MessageBeep(0);
		}
	else
		{
		if (params->Delete(itemIndex))
			{
			ListBox_DeleteString(hParams, itemIndex);
			ListBox_SetCurSel(hParams, itemIndex);
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

BOOL CALLBACK ParamDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPPARAMDESCARRAY	params;
	static BOOL							paramIsDirty;
  static long             paramIndex;
	HWND										hParams					= GetDlgItem(hDlg, LB_PARAMS);
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
			{
			WORD			wID				= GET_WM_COMMAND_ID(wParam, lParam);
			WORD			wNotify		= GET_WM_COMMAND_CMD(wParam, lParam);
			HWND			hCtl			= GET_WM_COMMAND_HWND(wParam, lParam);

			switch (wID)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if ((paramIsDirty == TRUE) &&
					    (paramIndex != -1) && 
					    ((paramIndex+1) != 
					     ListBox_GetCount(hParams)) &&
					    QuerySaveChanges(hDlg)
					   )
						UpdateParam(hDlg, params, paramIndex, FALSE);
					EndDialog(hDlg, TRUE);
					break;

				case BN_INSERT:
					InsertParam(hDlg, params);
          paramIndex = ListBox_GetCurSel(hParams);
					paramIsDirty = FALSE;
					break;

				case BN_REMOVE:
					DeleteCurrentParam(hDlg, params);
					paramIsDirty = FALSE;
					break;

				case BN_UPDATE:
					if (paramIsDirty == TRUE)
						UpdateParam(hDlg, params, ListBox_GetCurSel(hParams));
          paramIndex = ListBox_GetCurSel(hParams);
					paramIsDirty = FALSE;
					break;

				case BN_OPTIONAL:
				case BN_TEXT:
					if (wNotify == BN_CLICKED)
						{
						paramIsDirty = TRUE;
						if (0 == Button_GetCheck(hCtl))
							Button_SetCheck(hCtl, TRUE);
						else
							Button_SetCheck(hCtl, FALSE);
						}
					break;

				case LB_PARAMS:
					if (wNotify == LBN_SELCHANGE)
						{
						if ((paramIsDirty == TRUE) &&
						    (paramIndex != -1) && 
						    ((paramIndex+1) != 
						     ListBox_GetCount(hParams)) &&
						    QuerySaveChanges(hDlg)
						   )
              {
              newParamIndex = ListBox_GetCurSel(hParams);
							UpdateParam(hDlg, params, paramIndex, FALSE);
              if ((newParamIndex+1) == ListBox_GetCount(hParams))
  	            ListBox_SetCurSel(hParams, newParamIndex);
              }
	          paramIndex = ListBox_GetCurSel(hParams);
            SetCurrentParamData(hDlg, params);
						paramIsDirty = FALSE;
						}
					break;

				case SLE_NAME:
					if (wNotify == EN_CHANGE)
						paramIsDirty = TRUE;
					break;

				case CB_OBJECT:
					if (wNotify == CBN_SELCHANGE)
						paramIsDirty = TRUE;
					break;

				default:
					msgHandled = FALSE;
					break;
				}
			break;
			}

		default:
			msgHandled = FALSE;
			break;

		}

	return msgHandled;

	}
