#include "almsys.h"
#include <windowsx.h>

#include "fundsdlg.h"
#include "funcdesc.h"
#include "cfglib.h"
#include "objdesc.h"
#include "iconedit.h"
#include "fhelpdlg.h"
#include "prmdsdlg.h"
#include "flwdsdlg.h"
#include "sereclib.h"
#include "rectypes.h"
#include "fnameobj.h"

/////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////

BOOL CALLBACK FuncDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

/////////////////////////////////////////////////////////////////////////

#define	MAXNAMECHARS	80
#define	MAXUIDCHARS		4

#define BN_INPUTS			110
#define BN_OUTPUTS		112
#define BN_FLOWS			122

#define BN_EDITCOLOR	108
#define BN_EDITBW			109
#define BN_COLORICON	106
#define BN_BWICON			107

#define SLE_NAME	   		 102
#define SLE_DLL			 		 103
#define SLE_ENTRYPOINT   104
#define SLE_UID          101
#define SLE_HELPFILENAME 105

#define SLE_REQTYPE				150

static HINSTANCE	H_APP_INSTANCE;
static HINSTANCE	H_PREV_INSTANCE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FuncDescDlgBox
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL FuncDescDlgBox(
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	HWND					hWndParent,
	SERUID				objUID)
	{
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	result = DialogBoxParam(hAppInstance, "FUNC_DESC_DLG", hWndParent,
								FuncDescDlgProc, (DWORD)GetObject(objUID));
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitializeDialog
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
static void InitializeDialog(
	HWND		 		hDlg,
	LPFUNCDESC	func)
	{
	HWND					hName						= GetDlgItem(hDlg, SLE_NAME);
	HWND					hDLL						= GetDlgItem(hDlg, SLE_DLL);
	HWND					hEntry					= GetDlgItem(hDlg, SLE_ENTRYPOINT);
	HWND					hUID						= GetDlgItem(hDlg, SLE_UID);
	HWND					hReqCombo				= GetDlgItem(hDlg, SLE_REQTYPE);
	int						nMaxDllChars		= SFilePath::MaxLength();
	char 					uidBuffer[5];
	COMPUID				compUID;
	TYPEID				tRequired;
	int						nCount;

	Edit_LimitText(hName, MAXNAMECHARS);
	Edit_LimitText(hDLL, nMaxDllChars);
	Edit_LimitText(hEntry, MAXNAMECHARS);
	Edit_LimitText(hUID, MAXUIDCHARS);

	Edit_SetText(hName, (LPCSTR) func->GetName());
	Edit_SetText(hDLL, (LPSTR) func->GetLibName());
	Edit_SetText(hEntry, (LPSTR) func->GetEntryPointName());
	Edit_SetText(GetDlgItem(hDlg, SLE_HELPFILENAME), (LPCSTR) func->GetHelp());

	//initialize the UID field

	compUID = func->GetUID();
	CopyMemory(uidBuffer, &compUID, sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0] ^= uidBuffer[3] ^= uidBuffer[0] ^= uidBuffer[3];
	uidBuffer[1] ^= uidBuffer[2] ^= uidBuffer[1] ^= uidBuffer[2];

	Edit_SetText(hUID, uidBuffer);
	Edit_SetSel(hName, 0, 32767);

	//-------------- Required type initialization

	cfgInitObjectList(hReqCombo, "<None>");

	tRequired = func->GetRequiredType();

	// Search and select
	nCount = ComboBox_GetCount(hReqCombo);

	while (nCount > 0)
		{
		if (tRequired == ComboBox_GetItemData(hReqCombo, nCount - 1))
			{
			ComboBox_SetCurSel(hReqCombo, nCount - 1);
			break;
			}

		nCount--;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckNewUid
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL CheckNewUID(
  HWND        hDlg,
	LPFUNCDESC	func)
  {
	HWND				hUID			= GetDlgItem(hDlg, SLE_UID);
	char     buffer[5];
	COMPUID  compUID;
  BOOL     result = FALSE;

	Edit_GetText(hUID, buffer, MAXUIDCHARS+1);

	if (lstrlen(buffer) != MAXUIDCHARS)
		{
		Edit_SetSel(hUID, 0, 32767);
		SetFocus(hUID);
		MessageBeep(0);
		MessageBox(hDlg,
		"A function ID must contain exactly four Characters.",
		"Invalid function ID", MB_OK);
		return FALSE;
		}
	else
		{
		// these two lines swap the outer bytes and inner bytes
		// to reverse the uid character sequence.
		buffer[0]^=buffer[3]^=buffer[0]^=buffer[3];
		buffer[1]^=buffer[2]^=buffer[1]^=buffer[2];
		compUID = *(LPCOMPUID)buffer;

    if (compUID == func->GetUID())
      result = TRUE;
		else if (cfgCheckFunctionUID(compUID))
			{
			func->SetUID(compUID);
      result = TRUE;
			}
    else
			{
			Edit_SetSel(hUID, 0, 32767);
			SetFocus(hUID);
			MessageBeep(0);
			MessageBox(hDlg, "The specified function ID has already been used.",
			 "Duplicate Function ID", MB_OK);
      }

		}

	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetRequiredType
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void		GetRequiredType(
  HWND					hDlg,
	LPFUNCDESC		func)
  {
	HWND					hCombo			= GetDlgItem(hDlg, SLE_REQTYPE);
	TYPEID				tNewType;
	int						nSelected;

	nSelected = ComboBox_GetCurSel(hCombo);

	if (nSelected != CB_ERR)
		{
		tNewType = (TYPEID) ComboBox_GetItemData(hCombo, nSelected);
		func->SetRequiredType(tNewType);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FuncDescDlgProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK FuncDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	static LPFUNCDESC					func;
	static LPPARAMDESCARRAY		tempInputs;
	static LPPARAMDESCARRAY		tempOutputs;
	static LPFLOWDESCARRAY		tempFlows;
	static LPPARAMDESCARRAY		saveInputs;
	static LPPARAMDESCARRAY		saveOutputs;
	static LPFLOWDESCARRAY		saveFlows;
	static LPSERIUSICON			 	tempIcon;

	BOOL							msgHandled = TRUE;
	char 							buffer[MAXNAMECHARS+1];
	LPDRAWITEMSTRUCT	drawInfo;

	switch (message)
		{
		case WM_INITDIALOG:
			func = (LPFUNCDESC)lParam;
			saveInputs = new PARAMDESCARRAY(*func->GetInputs());
			saveOutputs = new PARAMDESCARRAY(*func->GetOutputs());
			saveFlows = new FLOWDESCARRAY(*func->GetFlows());
			tempIcon = NULL;
			InitializeDialog(hDlg, func);
			break;

		case WM_COMMAND:
			{
			WORD		wID		= GET_WM_COMMAND_ID(wParam, lParam);

			switch (wID)
				{
				case IDCANCEL:
					if (tempIcon != NULL)
						delete tempIcon;
					delete saveInputs;
					delete saveOutputs;
					delete saveFlows;

					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if (!CheckNewUID(hDlg, func))
						break;

					GetDlgItemText(hDlg, SLE_NAME, buffer, MAXNAMECHARS+1);
					func->SetName(buffer);

					GetDlgItemText(hDlg, SLE_DLL, buffer, SFilePath::MaxLength());
					func->SetLibName(buffer);

					GetDlgItemText(hDlg, SLE_ENTRYPOINT, buffer, MAXNAMECHARS+1);
					func->SetEntryPointName(buffer);

					GetDlgItemText(hDlg, SLE_HELPFILENAME, buffer, MAXNAMECHARS+1);
					func->SetHelp(buffer);

					GetRequiredType(hDlg, func);

					if (tempIcon != NULL)
						{
						*func->GetIcons() = *tempIcon;
						delete tempIcon;
						}

					func->SetInputs(saveInputs);
					delete saveInputs;

					func->SetOutputs(saveOutputs);
					delete saveOutputs;

					func->SetFlows(saveFlows);
					delete saveFlows;

					EndDialog(hDlg, TRUE);
					break;

				case BN_EDITCOLOR:
					if (tempIcon == NULL)
						{
						tempIcon = new SERIUSICON();
						*tempIcon = *func->GetIcons();
						}

					ICONEDITOR::Dialog(hDlg, H_APP_INSTANCE, H_PREV_INSTANCE,tempIcon,
					 COLOR16);
					InvalidateRect(GetDlgItem(hDlg, BN_COLORICON), 0, TRUE);
					break;

				case BN_EDITBW:
					if (tempIcon == NULL)
						{
						tempIcon = new SERIUSICON();
						*tempIcon = *func->GetIcons();
						}
					ICONEDITOR::Dialog(hDlg, H_APP_INSTANCE, H_PREV_INSTANCE,tempIcon,
					 BLACKANDWHITE);
					InvalidateRect(GetDlgItem(hDlg, BN_BWICON), 0, TRUE);
					break;

				case BN_INPUTS:
					tempInputs = new PARAMDESCARRAY(*saveInputs);
					if (ParamDescDlgBox(H_APP_INSTANCE, H_PREV_INSTANCE,
							hDlg, tempInputs, DO_INPUTS))
						*saveInputs = *tempInputs;
					delete tempInputs;
					break;

				case BN_OUTPUTS:
					tempOutputs = new PARAMDESCARRAY(*saveOutputs);
					if (ParamDescDlgBox(H_APP_INSTANCE, H_PREV_INSTANCE,
							hDlg, tempOutputs, DO_OUTPUTS))
						*saveOutputs = *tempOutputs;
					delete tempOutputs;
					break;

				case BN_FLOWS:
					tempFlows = new FLOWDESCARRAY(*saveFlows);
					if (FlowDescDlgBox(H_APP_INSTANCE, H_PREV_INSTANCE,
							hDlg, tempFlows))
						*saveFlows = *tempFlows;
					delete tempFlows;
					break;

				default:
					msgHandled = FALSE;
					break;
				}
			break;
			}

		case WM_DRAWITEM:
			drawInfo = (LPDRAWITEMSTRUCT)lParam;
			if (drawInfo->CtlID == BN_COLORICON)
				{
				if (tempIcon == NULL)
					func->GetIcons()->DrawColor(drawInfo->hDC, drawInfo->rcItem.left,
					 drawInfo->rcItem.top);
				else
					tempIcon->DrawColor(drawInfo->hDC, drawInfo->rcItem.left,
					 drawInfo->rcItem.top);
				}
			else if (drawInfo->CtlID == BN_BWICON)
				{
				if (tempIcon == NULL)
					func->GetIcons()->DrawBW(drawInfo->hDC, drawInfo->rcItem.left,
					 drawInfo->rcItem.top);
				else
					tempIcon->DrawBW(drawInfo->hDC, drawInfo->rcItem.left,
					 drawInfo->rcItem.top);
				}
			break;

		default:
			msgHandled = FALSE;
			break;

		}

	return msgHandled;

	}

