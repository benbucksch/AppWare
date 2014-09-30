#include "almsys.h"
#include <windowsx.h>
#include "cfglib.h"
#include "flwdsdlg.h"
#include "flows.h"

BOOL CALLBACK FlowDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

#define	MAXNAMECHARS	80
#define	MAXUIDCHARS		4

#define BN_INSERT			101
#define BN_REMOVE			119
#define BN_UPDATE			121

#define SLE_NAME	   	123

#define LB_FLOWS		 	122


static HINSTANCE		H_APP_INSTANCE;
static HINSTANCE		H_PREV_INSTANCE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FlowDescDlgBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FlowDescDlgBox(
	HINSTANCE				hAppInstance,
	HINSTANCE				hPrevInstance,
	HWND						hWndParent,
	LPFLOWDESCARRAY	flows)
	{
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	result = DialogBoxParam(hAppInstance, "FLOW_DESC_DLG", hWndParent,
										FlowDescDlgProc, (DWORD)flows);
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCurrentFlowData
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void SetCurrentFlowData(
	HWND						hDlg,
	LPFLOWDESCARRAY	flows)
	{
	HWND		hName				= GetDlgItem(hDlg, SLE_NAME);
	int			itemIndex;

	itemIndex = ListBox_GetCurSel(GetDlgItem(hDlg, LB_FLOWS));

	if (itemIndex == LB_ERR ||
			itemIndex >= flows->GetCount())
		{
		Edit_SetText(hName, "");
		}
	else
		{
		Edit_SetText(hName, (LPCSTR)(*flows)[itemIndex].name);
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
	LPFLOWDESCARRAY		flows)
	{
	HWND		hFlows		= GetDlgItem(hDlg, LB_FLOWS);
	long	 itemID;
	int		 flowCount;
	int		 i;

	Edit_LimitText(GetDlgItem(hDlg, SLE_NAME), MAXNAMECHARS);

	flowCount = flows->GetCount();
	for (i=0; i<flowCount; i++)
		ListBox_AddString(hFlows, (LPCSTR)(*flows)[i].name);

	itemID = ListBox_AddString(hFlows, "");
	ListBox_SetCurSel(hFlows, itemID);

	SetCurrentFlowData(hDlg, flows);
  
  return itemID;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetCurrentFlowData
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void GetCurrentFlowData(
	HWND						hDlg,
	LPSTR           szName)
	{
	Edit_GetText(GetDlgItem(hDlg, SLE_NAME), szName, MAXNAMECHARS+1);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InsertFlow
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InsertFlow(
	HWND						hDlg,
	LPFLOWDESCARRAY	flows)
	{
	HWND						hFlows			= GetDlgItem(hDlg, LB_FLOWS);
	HWND						hName				= GetDlgItem(hDlg, SLE_NAME);
	char						szName[MAXNAMECHARS+1];
  long            flowIndex;

  GetCurrentFlowData(hDlg, szName);
  
  flowIndex = ListBox_GetCurSel(hFlows);
  
	if (flowIndex == LB_ERR) //insert at end of list if nothing selected
		flowIndex = ListBox_GetCount(hFlows) - 1;
     
	flows->Insert(flowIndex, szName, "");
  
	ListBox_InsertString(hFlows, flowIndex, szName);

	ListBox_SetCurSel(hFlows, flowIndex + 1);
  
	if (flowIndex+2 < ListBox_GetCount(hFlows))
		SetCurrentFlowData(hDlg, flows);
    
	Edit_SetSel(hName, 0, 32767);
	SetFocus(hName);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateFlow
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void UpdateFlow(
	HWND						hDlg,
	LPFLOWDESCARRAY	flows,
	long            flowIndex,
	BOOL            reselectFlag = TRUE)
	{
	HWND						hFlows		= GetDlgItem(hDlg, LB_FLOWS);
	char						szName[MAXNAMECHARS+1];
	LPFLOWDESC			tempFlow;

  GetCurrentFlowData(hDlg, szName);
  
	if (flowIndex == -1)
	  flowIndex = ListBox_GetCurSel(hFlows);

	if (flowIndex == LB_ERR || flowIndex+1 == ListBox_GetCount(hFlows))
		MessageBeep(0);
	else
		{
		tempFlow = (LPFLOWDESC)flows->GetElement(flowIndex);
		tempFlow->name = szName;
		ListBox_DeleteString(hFlows, flowIndex);
		ListBox_InsertString(hFlows, flowIndex, szName);
		if (reselectFlag == TRUE)
			ListBox_SetCurSel(hFlows, flowIndex);
		}

	SetFocus(hFlows);
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

	response = MessageBox(hDlg, "Save Changes to current flow?",
	 "Save Changes", MB_YESNO|MB_ICONQUESTION);

	if (response == IDYES)
		result = TRUE;
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DeleteCurrentFlow
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DeleteCurrentFlow(
	HWND						hDlg,
	LPFLOWDESCARRAY	flows)
	{
	HWND		hFlows			= GetDlgItem(hDlg, LB_FLOWS);
	int			itemIndex;

	itemIndex = ListBox_GetCurSel(hFlows);

	if (itemIndex == LB_ERR ||
			((itemIndex+1) == ListBox_GetCount(hFlows)) ||
			itemIndex >= flows->GetCount())
		{
		MessageBeep(0);
		}
	else
		{
		if (flows->Delete(itemIndex))
		 {
		 ListBox_DeleteString(hFlows, itemIndex);
		 ListBox_SetCurSel(hFlows, itemIndex);
		 SetCurrentFlowData(hDlg, flows);
		 }
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FlowDescDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK FlowDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPFLOWDESCARRAY flows;
	static BOOL						 flowIsDirty;
  static long            flowIndex;
  long                   newFlowIndex;
	BOOL									 msgHandled = TRUE;


	switch (message)
		{
		case WM_INITDIALOG:
			flows = (LPFLOWDESCARRAY)lParam;
			flowIndex = InitDialog(hDlg, flows);
			flowIsDirty = FALSE;
			break;

		case WM_COMMAND:
			{
			WORD		wID				= GET_WM_COMMAND_ID(wParam, lParam);
			HWND		hFlows		= GetDlgItem(hDlg, LB_FLOWS);
			WORD		wNotify		= GET_WM_COMMAND_CMD(wParam, lParam);

			switch (wID)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if ((flowIsDirty == TRUE) && 
              (flowIndex != -1) &&
              ((flowIndex+1) != ListBox_GetCount(hFlows)) &&
					    QuerySaveChanges(hDlg)
             )
						UpdateFlow(hDlg, flows, flowIndex);
					EndDialog(hDlg, TRUE);
					break;

				case BN_INSERT:
					InsertFlow(hDlg, flows);
          flowIndex = ListBox_GetCurSel(hFlows);
					flowIsDirty = FALSE;
					break;

				case BN_UPDATE:
					if (flowIsDirty == TRUE)
						UpdateFlow(hDlg, flows, ListBox_GetCurSel(hFlows));
          flowIndex = ListBox_GetCurSel(hFlows);
					flowIsDirty = FALSE;
					break;

				case BN_REMOVE:
					DeleteCurrentFlow(hDlg, flows);
					flowIsDirty = FALSE;
					break;

				case LB_FLOWS:
					if (wNotify == LBN_SELCHANGE)
						{
						if ((flowIsDirty == TRUE) && 
                (flowIndex != -1) &&
                ((flowIndex+1) != ListBox_GetCount(hFlows)) &&
						    QuerySaveChanges(hDlg)
               )
              {
              newFlowIndex = ListBox_GetCurSel(hFlows);
							UpdateFlow(hDlg, flows, flowIndex, FALSE);
              if ((newFlowIndex+1) == ListBox_GetCount(hFlows))
								ListBox_SetCurSel(hFlows, newFlowIndex);
              }
					  flowIndex = ListBox_GetCurSel(hFlows);
						SetCurrentFlowData(hDlg, flows);
						flowIsDirty = FALSE;
						}
					break;

				case SLE_NAME:
					if (wNotify == EN_CHANGE)
						flowIsDirty = TRUE;
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
