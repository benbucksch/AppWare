#include <memory.h>
#include "almsys.h"

#include "objdsdlg.h"
#include "objdesc.h"
#include "iconedit.h"
#include "sereclib.h"
#include "respdlg.h"
#include "sereclib.h"
#include "rectypes.h"
#include "cfglib.h"

BOOL I_CALLBACK ObjDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

#define	MAXUIDCHARS				4
#define	MAXNAMECHARS			80
#define MAXFILENAMECHARS	12

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
	char    buffer[5];
	COMPUID compUID;
  BOOL    result = FALSE;

	GetDlgItemText(hDlg, SLE_UID, buffer, MAXUIDCHARS+1);

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
		SendDlgItemMessage(hDlg, SLE_UID, EM_SETSEL, 0, MAKELONG(0, 32767));
		SetFocus(GetDlgItem(hDlg, SLE_UID));
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
	DLGPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)ObjDescDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "OBJ_DESC_DLG", hWndParent, lpProc,
	 (DWORD)GetObject(objUID));
	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ObjDescDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK ObjDescDlgProc(
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

	BOOL							msgHandled = TRUE;
	char 							buffer[MAXNAMECHARS+1];
	LPDRAWITEMSTRUCT	drawInfo;

	switch (message)
		{
		case WM_INITDIALOG:
			obj = (LPOBJDESC)lParam;
			saveResponses = new RESPONSEARRAY(obj->responses);
			tempIcon = NULL;

			SendDlgItemMessage(hDlg, SLE_UID, EM_LIMITTEXT, MAXUIDCHARS, 0L);
			SendDlgItemMessage(hDlg, SLE_NAME, EM_LIMITTEXT, MAXNAMECHARS, 0L);
			SetDlgItemText(hDlg, SLE_NAME, obj->GetName());
			SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, MAKELONG(0, 32767));

			SendDlgItemMessage(hDlg, SLE_ADMIN_DLL, EM_LIMITTEXT,
			 MAXFILENAMECHARS, 0L);
			SendDlgItemMessage(hDlg, SLE_DEF_DLL, EM_LIMITTEXT,
			 MAXFILENAMECHARS, 0L);
			SendDlgItemMessage(hDlg, SLE_IMPEX_DLL, EM_LIMITTEXT,
			 MAXFILENAMECHARS, 0L);

			SetDlgItemText(hDlg, SLE_ADMIN_FUNC,
			 obj->GetFunction(ADMIN_ROUTINE_INDEX));
			SetDlgItemText(hDlg, SLE_ADMIN_DLL,
			 obj->GetDLL(ADMIN_ROUTINE_INDEX));

			SetDlgItemText(hDlg, SLE_DEF_FUNC,
			 obj->GetFunction(DEF_ROUTINE_INDEX));
			SetDlgItemText(hDlg, SLE_DEF_DLL,
			 obj->GetDLL(DEF_ROUTINE_INDEX));

			SetDlgItemText(hDlg, SLE_IMPEX_FUNC,
			 obj->GetFunction(IMPEX_ROUTINE_INDEX));
			SetDlgItemText(hDlg, SLE_IMPEX_DLL,
			 obj->GetDLL(IMPEX_ROUTINE_INDEX));

			SetDlgItemText(hDlg, SLE_HELPFILE, obj->GetHelpFileName());

			if (obj->TestMustBePersistentFlag())
				CheckRadioButton(hDlg, BN_MAYBE, BN_MUSTBE, BN_MUSTBE);
			else if (obj->TestCantBePersistentFlag())
				CheckRadioButton(hDlg, BN_MAYBE, BN_MUSTBE, BN_CANTBE);
			else
				CheckRadioButton(hDlg, BN_MAYBE, BN_MUSTBE, BN_MAYBE);

			if (obj->TestHiddenFlag())
				CheckDlgButton(hDlg, SLE_HIDDENTYPE, 1);

			originalUID = obj->GetUID();
			_fmemcpy(buffer, &originalUID, sizeof(COMPUID));
			buffer[4] = '\0';
			// these two lines swap the outer bytes and inner bytes
			// to reverse the uid character sequence.
			buffer[0]^=buffer[3]^=buffer[0]^=buffer[3];
			buffer[1]^=buffer[2]^=buffer[1]^=buffer[2];
			SetDlgItemText(hDlg, SLE_UID, buffer);

			SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, 0L);
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					delete saveResponses;
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:

					if (!CheckNewUID(hDlg, obj, originalUID))
						break;

					GetDlgItemText(hDlg, SLE_NAME, buffer, MAXNAMECHARS+1);
					obj->SetName(buffer);

					GetDlgItemText(hDlg, SLE_ADMIN_FUNC, buffer, MAXNAMECHARS+1);
					 obj->SetFunction(ADMIN_ROUTINE_INDEX, buffer);
					GetDlgItemText(hDlg, SLE_ADMIN_DLL, buffer, MAXNAMECHARS+1);
					 obj->SetDLL(ADMIN_ROUTINE_INDEX, buffer);

					GetDlgItemText(hDlg, SLE_DEF_FUNC, buffer, MAXNAMECHARS+1);
					 obj->SetFunction(DEF_ROUTINE_INDEX, buffer);
					GetDlgItemText(hDlg, SLE_DEF_DLL, buffer, MAXNAMECHARS+1);
					 obj->SetDLL(DEF_ROUTINE_INDEX, buffer);

					GetDlgItemText(hDlg, SLE_IMPEX_FUNC, buffer, MAXNAMECHARS+1);
					 obj->SetFunction(IMPEX_ROUTINE_INDEX, buffer);
					GetDlgItemText(hDlg, SLE_IMPEX_DLL, buffer, MAXNAMECHARS+1);
					 obj->SetDLL(IMPEX_ROUTINE_INDEX, buffer);

					GetDlgItemText(hDlg, SLE_HELPFILE, buffer, MAXNAMECHARS+1);
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

