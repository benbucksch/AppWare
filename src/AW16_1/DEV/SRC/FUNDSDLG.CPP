#include <memory.h>
#include "almsys.h"

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

/////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK FuncDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

/////////////////////////////////////////////////////////////////////////

#define	MAXNAMECHARS	80
#define MAXDLLCHARS		12
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
	DLGPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)FuncDescDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "FUNC_DESC_DLG", hWndParent, lpProc,
	 (DWORD)GetObject(objUID));
	FreeProcInstance((FARPROC) lpProc);

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
	char 					uidBuffer[5];
	COMPUID				compUID;
	TYPEID				tRequired;
	HWND					hReqCombo;
	int						nCount;

	SendDlgItemMessage(hDlg, SLE_NAME, EM_LIMITTEXT, MAXNAMECHARS, 0L);
	SendDlgItemMessage(hDlg, SLE_DLL, EM_LIMITTEXT, MAXDLLCHARS, 0L);
	SendDlgItemMessage(hDlg, SLE_ENTRYPOINT, EM_LIMITTEXT, MAXNAMECHARS, 0L);
	SendDlgItemMessage(hDlg, SLE_UID, EM_LIMITTEXT, MAXUIDCHARS, 0L);

	SetDlgItemText(hDlg, SLE_NAME, func->GetName());
	SetDlgItemText(hDlg, SLE_DLL, func->GetLibName());
	SetDlgItemText(hDlg, SLE_ENTRYPOINT, func->GetEntryPointName());
	SetDlgItemText(hDlg, SLE_HELPFILENAME, func->GetHelp());

	//initialize the UID field

	compUID = func->GetUID();
	_fmemcpy(uidBuffer, &compUID, sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0] ^= uidBuffer[3] ^= uidBuffer[0] ^= uidBuffer[3];
	uidBuffer[1] ^= uidBuffer[2] ^= uidBuffer[1] ^= uidBuffer[2];

	// SetDlgItemText(hDlg, ST_UID, uidBuffer);
	SetDlgItemText(hDlg, SLE_UID, uidBuffer);

	SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, MAKELONG(0, 32767));

	//-------------- Required type initialization

	hReqCombo = GetDlgItem(hDlg, SLE_REQTYPE);
	cfgInitObjectList(hReqCombo, "<None>");

	tRequired = func->GetRequiredType();

	// Search and select
	nCount = (int) SendMessage(hReqCombo, CB_GETCOUNT, 0, 0);

	while (nCount > 0)
		{
		if (tRequired ==
					(TYPEID) SendMessage(hReqCombo, CB_GETITEMDATA, nCount - 1, 0))
			{
			SendMessage(hReqCombo, CB_SETCURSEL, nCount - 1, 0);
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
	char     buffer[5];
	COMPUID  compUID;
  BOOL     result = FALSE;

	GetDlgItemText(hDlg, SLE_UID, buffer, MAXUIDCHARS+1);

	if (lstrlen(buffer) != MAXUIDCHARS)
		{
		SendDlgItemMessage(hDlg, SLE_UID, EM_SETSEL, 0,
						 MAKELONG(0, 32767));
		SetFocus(GetDlgItem(hDlg, SLE_UID));
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
			SendDlgItemMessage(hDlg, SLE_UID, EM_SETSEL, 0,
			 MAKELONG(0, 32767));
			SetFocus(GetDlgItem(hDlg, SLE_UID));
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
	TYPEID				tNewType;
	HWND					hCombo;
	int						nSelected;

	hCombo = GetDlgItem(hDlg, SLE_REQTYPE);

	nSelected = (int) SendMessage(hCombo, CB_GETCURSEL, 0, 0);

	if (nSelected != CB_ERR)
		{
		tNewType = (TYPEID) SendMessage(hCombo, CB_GETITEMDATA, nSelected, 0);
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
BOOL I_CALLBACK FuncDescDlgProc(
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
			switch (wParam)
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

					GetDlgItemText(hDlg, SLE_DLL, buffer, MAXDLLCHARS+1);
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

