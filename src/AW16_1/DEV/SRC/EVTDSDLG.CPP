#include <memory.h>
#include "almsys.h"

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

BOOL I_CALLBACK EventDescDlgProc(
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
	DLGPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)EventDescDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "EVENT_DESC_DLG", hWndParent, lpProc,
	 (LPARAM)GetObject(objUID));
	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MessageDescDlgProc
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK EventDescDlgProc(
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
			event = (LPEVENTDESC)lParam;
			SendDlgItemMessage(hDlg, SLE_NAME, EM_LIMITTEXT, MAXNAMECHARS, 0L);

			SendDlgItemMessage(hDlg, SLE_UID, EM_LIMITTEXT, MAXUIDCHARS, 0L);
			SetDlgItemText(hDlg, SLE_NAME, event->GetName());

			originalUID = event->GetUID();
			_fmemcpy(buffer, &originalUID, sizeof(COMPUID));
			buffer[4] = '\0';
			// these two lines swap the outer bytes and inner bytes
			// to reverse the uid character sequence.
			buffer[0]^=buffer[3]^=buffer[0]^=buffer[3];
			buffer[1]^=buffer[2]^=buffer[1]^=buffer[2];
			// SetDlgItemText(hDlg, ST_UID, buffer);
			SetDlgItemText(hDlg, SLE_UID, buffer);


			SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, 0L);

			cfgInitObjectList(GetDlgItem(hDlg, CB_OBJECT));

			compUID = event->GetTypeID();
			itemIndex = 0;
			for (;;)
				{
				tempUID =
				 SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETITEMDATA, itemIndex, 0L);
				if (compUID == tempUID)
					{
					SendDlgItemMessage(hDlg, CB_OBJECT, CB_SETCURSEL, itemIndex, 0L);
					break;
					}
				else if (tempUID == CB_ERR)
					break;
				else
					itemIndex++;
				}
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if (!CheckNewUID(hDlg, event, originalUID))
						break;
					GetDlgItemText(hDlg, SLE_NAME, buffer, MAXNAMECHARS);
					event->SetName(buffer);
					compUID = SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETITEMDATA,
					 SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETCURSEL, 0, 0L), 0L);
					if (compUID != CB_ERR)
						event->SetTypeID(compUID);
					EndDialog(hDlg, TRUE);
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
	char     buffer[5];
	COMPUID  compUID;
  BOOL     result = FALSE;

	GetDlgItemText(hDlg, SLE_UID, buffer, MAXUIDCHARS+1);

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
		SendDlgItemMessage(hDlg, SLE_UID, EM_SETSEL, 0, MAKELONG(0, 32767));
		SetFocus(GetDlgItem(hDlg, SLE_UID));
    }

	return result;

	}
