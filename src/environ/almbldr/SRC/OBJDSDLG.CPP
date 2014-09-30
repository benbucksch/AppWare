#include "almsys.h"
#include <windowsx.h>

#include "objdsdlg.h"
#include "objdesc.h"
#include "iconedit.h"
#include "sereclib.h"
#include "respdlg.h"
#include "sereclib.h"
#include "rectypes.h"
#include "cfglib.h"
#include "fnameobj.h"

BOOL CALLBACK ObjDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

#define	MAXUIDCHARS				4
#define	MAXNAMECHARS			80

#define BN_RESPONSES	101
#define BN_EDITICON		102
#define BN_ICON				103

#define BN_MAYBE      131
#define BN_CANTBE     132
#define BN_MUSTBE     133


#define SLE_UID         104
#define SLE_NAME	   		105
#define SLE_ADMIN_FUNC  126
#define SLE_ADMIN_DLL		123
#define SLE_DEF_FUNC		128
#define SLE_DEF_DLL			124
#define SLE_IMPEX_FUNC	127
#define SLE_IMPEX_DLL		125
#define SLE_HELPFILE    134
#define SLE_HIDDENTYPE	139

#define BN_EDITCOLOR	114
#define BN_EDITBW			115
#define BN_COLORICON	113
#define BN_BWICON			103

static HINSTANCE		H_APP_INSTANCE;
static HINSTANCE		H_PREV_INSTANCE;


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckNewUID
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL CheckNewUID(
  HWND       hDlg,
	LPOBJDESC	 objd,
  COMPUID    originalUID)
  {
	HWND				hUID			= GetDlgItem(hDlg, SLE_UID);
	char				buffer[5];
	COMPUID			compUID;
  BOOL				result = FALSE;

	Edit_GetText(hUID, buffer, MAXUIDCHARS+1);

	if (lstrlen(buffer) != MAXUIDCHARS)
		{
		MessageBeep(0);
		MessageBox(hDlg, "An object ID must contain exactly four characters.",
		 "Invalid Object ID", MB_OK);
		}
	else
		{
		// these two lines swap the outer bytes and inner bytes
		// to reverse the uid character sequence.
		buffer[0]^=buffer[3]^=buffer[0]^=buffer[3];
		buffer[1]^=buffer[2]^=buffer[1]^=buffer[2];

		compUID = *(LPCOMPUID)buffer;

    if (compUID == originalUID)
      result = TRUE;
		else if (cfgCheckObjectUID(compUID))
			{
			objd->SetUID(compUID);
      result = TRUE;
			}
    else
      {
			MessageBeep(0);
			MessageBox(hDlg, "The specified object ID has already been used.",
			 "Duplicate Object ID", MB_OK);
      }
		}

  if (result == FALSE)
    {
		Edit_SetSel(hUID, 0, 32767);
		SetFocus(hUID);
    }

	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ObjDescDlgBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ObjDescDlgBox(
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	HWND					hWndParent,
	SERUID				objUID)
	{
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	result = DialogBoxParam(hAppInstance, "OBJ_DESC_DLG", hWndParent,
										(DLGPROC)ObjDescDlgProc, (DWORD)GetObject(objUID));
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ObjDescDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK ObjDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	static LPOBJDESC				obj;
	static LPRESPONSEARRAY  tempResponses;
	static LPRESPONSEARRAY  saveResponses;
	static LPSERIUSICON			tempIcon;
	static COMPUID          originalUID;

	HWND							hUID					= GetDlgItem(hDlg, SLE_UID);
	HWND							hName					= GetDlgItem(hDlg, SLE_NAME);
	HWND							hAdminDll			= GetDlgItem(hDlg, SLE_ADMIN_DLL);
	HWND							hAdminFunc		= GetDlgItem(hDlg, SLE_ADMIN_FUNC);
	HWND							hDefFunc			= GetDlgItem(hDlg, SLE_DEF_FUNC);
	HWND							hDefDll				= GetDlgItem(hDlg, SLE_DEF_DLL);
	HWND							hImpExDll			= GetDlgItem(hDlg, SLE_IMPEX_DLL);
	HWND							hImpExFunc		= GetDlgItem(hDlg, SLE_IMPEX_FUNC);
	HWND							hHelpFile			= GetDlgItem(hDlg, SLE_HELPFILE);
	BOOL							msgHandled		= TRUE;
	int								nMaxFNameLen	= SFilePath::MaxLength();
	char 							buffer[MAXNAMECHARS+1];
	LPDRAWITEMSTRUCT	drawInfo;

	switch (message)
		{
		case WM_INITDIALOG:
			obj = (LPOBJDESC)lParam;
			saveResponses = new RESPONSEARRAY(obj->responses);
			tempIcon = NULL;

			Edit_LimitText(hUID, MAXUIDCHARS);
			Edit_LimitText(hName, MAXNAMECHARS);
			Edit_SetText(hName, (LPCSTR) obj->GetName());
			Edit_SetSel(hName, 0, 32767);

			Edit_LimitText(hAdminDll, nMaxFNameLen);
			Edit_LimitText(hDefDll, nMaxFNameLen);
			Edit_LimitText(hImpExDll, nMaxFNameLen);

			Edit_SetText(hAdminFunc, (LPSTR) obj->GetFunction(ADMIN_ROUTINE_INDEX));
			Edit_SetText(hAdminDll, (LPSTR) obj->GetDLL(ADMIN_ROUTINE_INDEX));

			Edit_SetText(hDefFunc, (LPSTR) obj->GetFunction(DEF_ROUTINE_INDEX));
			Edit_SetText(hDefDll, (LPSTR) obj->GetDLL(DEF_ROUTINE_INDEX));

			Edit_SetText(hImpExFunc, (LPSTR) obj->GetFunction(IMPEX_ROUTINE_INDEX));
			Edit_SetText(hImpExDll, (LPSTR) obj->GetDLL(IMPEX_ROUTINE_INDEX));

			Edit_SetText(hHelpFile, (LPSTR) obj->GetHelpFileName());

			if (obj->TestMustBePersistentFlag())
				CheckRadioButton(hDlg, BN_MAYBE, BN_MUSTBE, BN_MUSTBE);
			else if (obj->TestCantBePersistentFlag())
				CheckRadioButton(hDlg, BN_MAYBE, BN_MUSTBE, BN_CANTBE);
			else
				CheckRadioButton(hDlg, BN_MAYBE, BN_MUSTBE, BN_MAYBE);

			if (obj->TestHiddenFlag())
				CheckDlgButton(hDlg, SLE_HIDDENTYPE, 1);

			originalUID = obj->GetUID();
			CopyMemory(buffer, &originalUID, sizeof(COMPUID));
			buffer[4] = '\0';
			// these two lines swap the outer bytes and inner bytes
			// to reverse the uid character sequence.
			buffer[0]^=buffer[3]^=buffer[0]^=buffer[3];
			buffer[1]^=buffer[2]^=buffer[1]^=buffer[2];
			Edit_SetText(hUID, buffer);

			Edit_SetSel(hName, 0, 0);
			break;

		case WM_COMMAND:
			{
			WORD			wID		= GET_WM_COMMAND_ID(wParam, lParam);

			switch (wID)
				{
				case IDCANCEL:
					delete saveResponses;
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if (!CheckNewUID(hDlg, obj, originalUID))
						break;

					Edit_GetText(hName, buffer, MAXNAMECHARS+1);
					obj->SetName(buffer);

					Edit_GetText(hAdminFunc, buffer, MAXNAMECHARS+1);
					obj->SetFunction(ADMIN_ROUTINE_INDEX, buffer);
					Edit_GetText(hAdminDll, buffer, MAXNAMECHARS+1);
					obj->SetDLL(ADMIN_ROUTINE_INDEX, buffer);

					Edit_GetText(hDefFunc, buffer, MAXNAMECHARS+1);
					obj->SetFunction(DEF_ROUTINE_INDEX, buffer);
					Edit_GetText(hDefDll, buffer, MAXNAMECHARS+1);
					obj->SetDLL(DEF_ROUTINE_INDEX, buffer);

					Edit_GetText(hImpExFunc, buffer, MAXNAMECHARS+1);
					obj->SetFunction(IMPEX_ROUTINE_INDEX, buffer);
					Edit_GetText(hImpExDll, buffer, MAXNAMECHARS+1);
					obj->SetDLL(IMPEX_ROUTINE_INDEX, buffer);

					Edit_GetText(hHelpFile, buffer, MAXNAMECHARS+1);
					obj->SetHelpFileName(buffer);

					if (IsDlgButtonChecked(hDlg, BN_CANTBE))
						obj->SetCantBePersistentFlag();
					else if (IsDlgButtonChecked(hDlg, BN_MUSTBE))
						obj->SetMustBePersistentFlag();
					else
						obj->SetMayBePersistentFlag();

					if (IsDlgButtonChecked(hDlg, SLE_HIDDENTYPE))
						obj->SetHiddenFlag();
					else
						obj->ClearHiddenFlag();

					obj->responses = *saveResponses;

					if (tempIcon != NULL)
						{
						*obj->GetIcons() = *tempIcon;
						delete tempIcon;
						}

					delete saveResponses;

					EndDialog(hDlg, TRUE);
					break;

				case BN_EDITCOLOR:
					if (tempIcon == NULL)
						{
						tempIcon = new SERIUSICON();
						*tempIcon = *obj->GetIcons();
						}
					ICONEDITOR::Dialog(hDlg, H_APP_INSTANCE, H_PREV_INSTANCE,tempIcon,
					 COLOR16);
					InvalidateRect(GetDlgItem(hDlg, BN_COLORICON), 0, TRUE);
					break;

				case BN_EDITBW:
					if (tempIcon == NULL)
						{
						tempIcon = new SERIUSICON();
						*tempIcon = *obj->GetIcons();
						}
					ICONEDITOR::Dialog(hDlg, H_APP_INSTANCE, H_PREV_INSTANCE,tempIcon,
					 BLACKANDWHITE);
					InvalidateRect(GetDlgItem(hDlg, BN_BWICON), 0, TRUE);
					break;

				case BN_RESPONSES:
					tempResponses = new RESPONSEARRAY(*saveResponses);
					if (ResponsesDlgBox(H_APP_INSTANCE, H_PREV_INSTANCE,
							hDlg, obj->GetUID(), tempResponses))
						*saveResponses = *tempResponses;
					delete tempResponses;
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
					obj->GetIcons()->DrawColor(drawInfo->hDC, drawInfo->rcItem.left,
					 drawInfo->rcItem.top);
				else
					tempIcon->DrawColor(drawInfo->hDC, drawInfo->rcItem.left,
					 drawInfo->rcItem.top);
				}
			else if (drawInfo->CtlID == BN_BWICON)
				{
				if (tempIcon == NULL)
					obj->GetIcons()->DrawBW(drawInfo->hDC, drawInfo->rcItem.left,
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

