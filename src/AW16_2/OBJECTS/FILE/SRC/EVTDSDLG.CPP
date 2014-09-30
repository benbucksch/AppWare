#include "almsys.h"
#include <windowsx.h>

#include "evtdsdlg.h"
#include "cfglib.h"
#include "evntdesc.h"
#include "objdesc.h"
#include "sereclib.h"
#include "rectypes.h"


static BOOL CheckNewUID(
  HWND         hDlg,
	LPEVENTDESC	 event,
  COMPUID      originalUID);

BOOL CALLBACK EventDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

#define	MAXNAMECHARS	80
#define	MAXUIDCHARS		4

#define SLE_NAME	   105

#define CB_OBJECT		 125

#define ST_UID       104
#define SLE_UID      104


static HINSTANCE		H_APP_INSTANCE;
static HINSTANCE		H_PREV_INSTANCE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EventDescDlgBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL EventDescDlgBox(
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	HWND					hWndParent,
	SERUID				objUID)
	{
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	result = DialogBoxParam(hAppInstance, "EVENT_DESC_DLG", hWndParent,
													EventDescDlgProc, (LPARAM)GetObject(objUID));
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MessageDescDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EventDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPEVENTDESC	event;
  static COMPUID      originalUID;
	BOOL								msgHandled = TRUE;
	char 								buffer[MAXNAMECHARS+1];
	COMPUID							compUID;
	COMPUID							tempUID;
	int									itemIndex;

	switch (message)
		{
		case WM_INITDIALOG:
			{
			HWND		hName	= GetDlgItem(hDlg, SLE_NAME);
			HWND		hUID	= GetDlgItem(hDlg, SLE_UID);
			HWND		hObj	= GetDlgItem(hDlg, CB_OBJECT);

			event = (LPEVENTDESC)lParam;
			Edit_LimitText(hName, MAXNAMECHARS);
			Edit_LimitText(hUID, MAXUIDCHARS);

			Edit_SetText(hName, (LPCSTR) event->GetName());

			originalUID = event->GetUID();
			CopyMemory(buffer, &originalUID, sizeof(COMPUID));
			buffer[4] = '\0';
			// these two lines swap the outer bytes and inner bytes
			// to reverse the uid character sequence.
			buffer[0]^=buffer[3]^=buffer[0]^=buffer[3];
			buffer[1]^=buffer[2]^=buffer[1]^=buffer[2];
			
			Edit_SetText(hUID, buffer);
			Edit_SetSel(hName, 0, 0);

			cfgInitObjectList(GetDlgItem(hDlg, CB_OBJECT));

			compUID = event->GetTypeID();
			itemIndex = 0;
			for (;;)
				{
				tempUID = ComboBox_GetItemData(hObj, itemIndex);
				if (compUID == tempUID)
					{
					ComboBox_SetCurSel(hObj, itemIndex);
					break;
					}
				else if (tempUID == (COMPUID) CB_ERR)
					break;
				else
					itemIndex++;
				}
			break;
			}

		case WM_COMMAND:
			{
			WORD		wID		= GET_WM_COMMAND_ID(wParam, lParam);

			switch (wID)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					{
					HWND		hObj	= GetDlgItem(hDlg, CB_OBJECT);

					if (!CheckNewUID(hDlg, event, originalUID))
						break;
					Edit_GetText(GetDlgItem(hDlg, SLE_NAME), buffer, MAXNAMECHARS);
					event->SetName(buffer);
					compUID = ComboBox_GetItemData(hObj, ComboBox_GetCurSel(hObj));

					if (compUID != (COMPUID) CB_ERR)
						event->SetTypeID(compUID);
					EndDialog(hDlg, TRUE);
					break;
					}

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


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckNewUID
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL CheckNewUID(
  HWND         hDlg,
	LPEVENTDESC	 event,
  COMPUID      originalUID)
  {
	HWND			hUID	= GetDlgItem(hDlg, SLE_UID);
	char			buffer[5];
	COMPUID		compUID;
  BOOL			result = FALSE;

	Edit_GetText(hUID, buffer, MAXUIDCHARS + 1);

	if (lstrlen(buffer) != MAXUIDCHARS)
		{
		MessageBeep(0);
		MessageBox(hDlg, "An event ID must contain exactly four characters.",
		 "Invalid Event ID", MB_OK);
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
		else if (cfgCheckEventUID(compUID))
			{
			event->SetUID(compUID);
      result = TRUE;
			}
    else
      {
			MessageBeep(0);
			MessageBox(hDlg, "The specified event ID has already been used.",
			 "Duplicate Event ID", MB_OK);
      }
		}

  if (result == FALSE)
    {
		Edit_SetSel(hUID, 0, 32767);
		SetFocus(hUID);
    }

	return result;
	}
