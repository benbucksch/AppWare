#include <almsys.h>
#include <memory.h>
#include "cfglib.h"
#include "flwdsdlg.h"
#include "flows.h"

BOOL I_CALLBACK FlowDescDlgProc(
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
	DLGPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)FlowDescDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "FLOW_DESC_DLG", hWndParent, lpProc,
	 (DWORD)flows);
	FreeProcInstance((FARPROC) lpProc);

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
	int			itemIndex;

	itemIndex = SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);

	if (itemIndex == LB_ERR ||
//			((itemIndex+1) == SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT 0, 0)) ||
			itemIndex >= flows->GetCount())
		{
		SendDlgItemMessage(hDlg, SLE_NAME, WM_SETTEXT, 0, (long)"");
		}
	else
		{
		SendDlgItemMessage(hDlg, SLE_NAME, WM_SETTEXT, 0,
		 (long)(LPSTR)(*flows)[itemIndex].name);
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
	long	 itemID;
	int		 flowCount;
	int		 i;

	SendDlgItemMessage(hDlg, SLE_NAME, EM_LIMITTEXT, MAXNAMECHARS, 0L);

	flowCount = flows->GetCount();
	for (i=0; i<flowCount; i++)
		SendDlgItemMessage(hDlg, LB_FLOWS, LB_ADDSTRING, 0,
		 (long)(LPSTR)(*flows)[i].name);

	itemID = SendDlgItemMessage(hDlg, LB_FLOWS, LB_ADDSTRING, 0, (long)"");

	SendDlgItemMessage(hDlg, LB_FLOWS, LB_SETCURSEL, itemID, 0L);

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

	SendDlgItemMessage(hDlg, SLE_NAME, WM_GETTEXT, MAXNAMECHARS+1,
	 (long)szName);

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
	char						szName[MAXNAMECHARS+1];
  long            flowIndex;

  GetCurrentFlowData(hDlg, szName);
  
  flowIndex = SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);
  
	if (flowIndex == LB_ERR) //insert at end of list if nothing selected
		flowIndex =
		 SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT, 0, 0L) - 1;
     
	flows->Insert(flowIndex, szName, "");
  
	SendDlgItemMessage(hDlg, LB_FLOWS, LB_INSERTSTRING, flowIndex,
	 (long)szName);
   
	SendDlgItemMessage(hDlg, LB_FLOWS, LB_SETCURSEL, flowIndex+1, 0);
  
	if (flowIndex+2 <
			SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT, 0, 0))
		SetCurrentFlowData(hDlg, flows);
    
	SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, MAKELONG(0, 32767));
  
	SetFocus(GetDlgItem(hDlg, SLE_NAME));

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
	char						szName[MAXNAMECHARS+1];
	LPFLOWDESC			tempFlow;

  GetCurrentFlowData(hDlg, szName);
  
	if (flowIndex == -1)
	  flowIndex = SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);

	if (flowIndex == LB_ERR ||
			flowIndex+1 == SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT,
			0, 0L))
		MessageBeep(0);
	else
		{
		tempFlow = (LPFLOWDESC)flows->GetElement(flowIndex);
		tempFlow->name = szName;
		SendDlgItemMessage(hDlg, LB_FLOWS, LB_DELETESTRING, flowIndex, 0L);
		SendDlgItemMessage(hDlg, LB_FLOWS, LB_INSERTSTRING, flowIndex,
		 (long)szName);
		if (reselectFlag == TRUE)
  		SendDlgItemMessage(hDlg, LB_FLOWS, LB_SETCURSEL, flowIndex, 0L);
		}

	SetFocus(GetDlgItem(hDlg, LB_FLOWS));

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
	int			itemIndex;

	itemIndex = SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);

	if (itemIndex == LB_ERR ||
			((itemIndex+1) == SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT, 0, 0)) ||
			itemIndex >= flows->GetCount())
		{
		MessageBeep(0);
		}
	else
		{
		if (flows->Delete(itemIndex))
		 {
		 SendDlgItemMessage(hDlg, LB_FLOWS, LB_DELETESTRING, itemIndex, 0L);
     SendDlgItemMessage(hDlg, LB_FLOWS, LB_SETCURSEL, itemIndex, 0L);
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

BOOL I_CALLBACK FlowDescDlgProc(
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
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if ((flowIsDirty == TRUE) && 
              (flowIndex != -1) &&
              ((flowIndex+1) !=  
					     SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT, 0, 0)) &&
					    QuerySaveChanges(hDlg)
             )
						UpdateFlow(hDlg, flows, flowIndex);
					EndDialog(hDlg, TRUE);
					break;

				case BN_INSERT:
					InsertFlow(hDlg, flows);
          flowIndex = 
           SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);
					flowIsDirty = FALSE;
					break;

				case BN_UPDATE:
					if (flowIsDirty == TRUE)
						UpdateFlow(hDlg, flows,
 						 SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L));
          flowIndex = 
           SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);
					flowIsDirty = FALSE;
					break;

				case BN_REMOVE:
					DeleteCurrentFlow(hDlg, flows);
					flowIsDirty = FALSE;
					break;

				case LB_FLOWS:
					if (HIWORD(lParam) == LBN_SELCHANGE)
						{
						if ((flowIsDirty == TRUE) && 
                (flowIndex != -1) &&
                ((flowIndex+1) !=  
						     SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT, 0, 0)) &&
						    QuerySaveChanges(hDlg)
               )
              {
              newFlowIndex =  
               SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);
							UpdateFlow(hDlg, flows, flowIndex, FALSE);
              if ((newFlowIndex+1) ==  
  						     SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCOUNT, 0, 0))
  	            SendDlgItemMessage(hDlg, LB_FLOWS, LB_SETCURSEL, newFlowIndex, 0L);
              }
					  flowIndex = 
					   SendDlgItemMessage(hDlg, LB_FLOWS, LB_GETCURSEL, 0, 0L);
						SetCurrentFlowData(hDlg, flows);
						flowIsDirty = FALSE;
						}
					break;

				case SLE_NAME:
					if (HIWORD(lParam) == EN_CHANGE)
						flowIsDirty = TRUE;
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
