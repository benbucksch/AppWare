#include "tm_incld.h"
#include "tm_dialg.h"
#include "tm_utl.h"

LRESULT far pascal _export TimeEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*****************************************************/

LONG EditTimeObject(OBJECTID  oi)
{
	BOOL	bEdited;

	bEdited = DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(TIME_SETUP_DIALOG),	GetActiveWindow(),	(DLGPROC)EditTime::TimeEditDlg, (LONG) oi);

	if (bEdited)
		return A_MODIFIED;
	else
  	return A_OK;
}

/*****************************************************/
BOOL WINAPI _export EditTime::TimeEditDlg(HWND  hwnd, UINT msg, WPARAM wParam,	LPARAM lParam )
{
	LPEditTime   edittime;
	int          nc;
	char         tmpStr[80];

	switch(msg)
	{
		case WM_INITDIALOG:
			edittime = new EditTime( (OBJECTID) lParam );
			SetWindowLong(hwnd, DWL_USER, (long)edittime);
			edittime->SetWindowHandle(hwnd);
			edittime->InitDialog();
			SetFocus( GetDlgItem(hwnd, ED_TITLE) );
			SendDlgItemMessage(hwnd, ED_TITLE, EM_SETSEL, 0, MAKELONG(0, -1));
			AUtlSetTitleFont(OTYPE_TIME, NULL, GetDlgItem(hwnd, ST_OBJECTNAME) );
			AUtlCenterDialog(hwnd, 0);
			return FALSE;

		case WM_DESTROY:
			edittime = (LPEditTime) GetWindowLong(hwnd, DWL_USER);
			delete edittime;
			return FALSE;

		case WM_COMMAND:
			edittime = (LPEditTime) GetWindowLong(hwnd, DWL_USER);
			nc = HIWORD(lParam);
			switch(wParam)
			{
				case RB_12HOUR:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
					{
						edittime->SetIs24Hour(FALSE);
						edittime->SetIsDefHour(FALSE);
						edittime->EnableProperControls();
          }
					break;

				case RB_24HOUR:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
					{
						edittime->SetIs24Hour(TRUE);
						edittime->SetIsDefHour(FALSE);
						edittime->EnableProperControls();
          }
					break;

				case RB_DEFHOUR:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
					{
						edittime->SetIsDefHour(TRUE);
						edittime->SetIs24Hour(FALSE);
						edittime->EnableProperControls();
          }
					break;


  			case RB_EDITABLE:
					if (nc == BN_CLICKED)
          {
						edittime->SetIsEditable(TRUE);
						edittime->SetIsKeepCurrent(FALSE);
						edittime->EnableProperControls();
          }
					break;

				case RB_KEEPCURRENT:
					if (nc == BN_CLICKED)
          {
						edittime->SetIsKeepCurrent(TRUE);
						edittime->SetIsEditable(FALSE);
						edittime->EnableProperControls();
          }
					break;

				case RB_NEITHER:
					if (nc == BN_CLICKED)
          {
						edittime->SetIsKeepCurrent(FALSE);
						edittime->SetIsEditable(FALSE);
						edittime->EnableProperControls();
          }
					break;

				case CB_SELECTALL:
					if (nc == BN_CLICKED)
         	{
						edittime->SetSelectsAll((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						edittime->EnableProperControls();
          }
					break;

				case CB_SECONDS:
					if (nc == BN_CLICKED)
         	{
						edittime->SetSecond((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						edittime->EnableProperControls();
          }
					break;

				case CB_FRAME:
					if (nc == BN_CLICKED)
          {
						edittime->SetHasFrame((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						edittime->EnableProperControls();
          }
					break;

				case CB_ACTIVATABLE:
					if (nc == BN_CLICKED)
          {
						edittime->SetIsActivatable((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						edittime->EnableProperControls();
          }
					break;

				case CB_DISPLAYEMPTY:
					if (nc == BN_CLICKED)
					{
						edittime->SetDisplaysEmpty((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						edittime->EnableProperControls();
          }
					break;

				case ED_TITLE:
					if (nc == EN_KILLFOCUS)
							edittime->m_time = GetDlgEditTime(hwnd, ED_TITLE, edittime->m_style);
					break;

				case PB_OK:  //cause title and format to get a killfocus..
					SetFocus((HWND)LOWORD(lParam)); 
					edittime->StoreAobj();
					EndDialog(hwnd, TRUE);
					break;

				case PB_CANCEL:
					EndDialog(hwnd, FALSE);
					break;

				case PB_FONTS:
					GetDlgItemText(hwnd, ED_TITLE, tmpStr, sizeof(tmpStr));
					edittime->EditFont(hwnd, tmpStr);
					break;

				case PB_HELP:
					//WinHelp(hwnd, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Text);
					break;
				}
			break;
		}

	return FALSE;
}

void EditTime::EnableProperControls()
{
	char tmpStr[80];

	EnableWindow(GetDlgItem(m_hwndDlg, CB_FRAME), IsActivatable() );
	EnableWindow(GetDlgItem(m_hwndDlg, CB_DISPLAYEMPTY), !IsKeepCurrent() );
	ShowWindow(GetDlgItem(m_hwndDlg, CB_SELECTALL), IsEditable());
	ShowWindow(GetDlgItem(m_hwndDlg, CB_CANACTIVATE), !IsEditable());

	if (Is24Hour())
	{
		FormatTime(m_time, m_style, tmpStr, sizeof(tmpStr) );
		SetDlgItemText(m_hwndDlg, ED_TITLE, tmpStr);
	}
	else if (IsDefHour())
	{
		FormatTime(m_time, m_style, tmpStr, sizeof(tmpStr) );
		SetDlgItemText(m_hwndDlg, ED_TITLE, tmpStr);
	}
	else
	{
		FormatTime(m_time, m_style, tmpStr, sizeof(tmpStr) );
		SetDlgItemText(m_hwndDlg, ED_TITLE, tmpStr);
	}

}

static BOOL IsButtonReallyChecked(
	HWND 										hwndDlg,
	int											idItem)
	{
	if (!IsWindowEnabled(GetDlgItem(hwndDlg, idItem)) )
		return FALSE;

	return (SendMessage(GetDlgItem(hwndDlg, idItem), BM_GETCHECK, 0, 0L) == 1);
	}

void EditTime::StoreAobj()
{
	LPTime timeSrc;
					
	SetSelectsAll(IsButtonReallyChecked(m_hwndDlg, CB_SELECTALL));
	SetSecond(IsButtonReallyChecked(m_hwndDlg, CB_SECONDS));
	SetHasFrame(IsButtonReallyChecked(m_hwndDlg, CB_FRAME));
	SetIsActivatable(IsButtonReallyChecked(m_hwndDlg, CB_ACTIVATABLE));
	SetDisplaysEmpty(IsButtonReallyChecked(m_hwndDlg, CB_DISPLAYEMPTY));
					
	timeSrc = LockTimeObject(m_oiSelf);

	timeSrc->m_version 				= m_version;
	timeSrc->m_oiSelf 				= m_oiSelf;
	timeSrc->m_style 					= m_style;
	timeSrc->m_time           = m_time;
	_fmemcpy(&timeSrc->m_attributes, &m_attributes, sizeof(ATTRIBUTES) );

	timeSrc->FormatTM();

	UnlockTimeObject(m_oiSelf);
}


void EditTime::InitDialog()
{
	char  tmpStr[TM_STRING_LEN];
	char 	objectName[80];
	LONG	time;

	time = 46800L; // 1 pm

	AObjGetName(m_oiSelf, objectName, 80);
	SetWindowText(m_hwndDlg, objectName);

	FormatTime(time, TM_24HOURS, tmpStr, sizeof(tmpStr) );
	strcat(tmpStr, " (24 hour time)");
	SetDlgItemText(m_hwndDlg, RB_24HOUR, tmpStr);

	FormatTime(time, TM_SECONDS, tmpStr, sizeof(tmpStr) );
	strcat(tmpStr, " (12 hour time)");
	SetDlgItemText(m_hwndDlg, RB_12HOUR, tmpStr);

	if (Is24Hour())
		SendDlgItemMessage(m_hwndDlg, RB_24HOUR, BM_SETCHECK, 1, 0L);
	else if (IsDefHour())
		SendDlgItemMessage(m_hwndDlg, RB_DEFHOUR, BM_SETCHECK, 1, 0L);
	else
		SendDlgItemMessage(m_hwndDlg, RB_12HOUR, BM_SETCHECK, 1, 0L);

	if (HasFrame())
		SendDlgItemMessage(m_hwndDlg, CB_FRAME, BM_SETCHECK, 1, 0L);
	if (HasSeconds())
		SendDlgItemMessage(m_hwndDlg, CB_SECONDS, BM_SETCHECK, 1, 0L);
	if (SelectsAll())        		
		SendDlgItemMessage(m_hwndDlg, CB_SELECTALL, BM_SETCHECK, 1, 0L);
	if (IsActivatable())      	
		SendDlgItemMessage(m_hwndDlg, CB_ACTIVATABLE, BM_SETCHECK, 1, 0L);
	if (DisplaysEmpty())				
		SendDlgItemMessage(m_hwndDlg, CB_DISPLAYEMPTY, BM_SETCHECK, 1, 0L);

	if (IsEditable())						
		SendDlgItemMessage(m_hwndDlg, RB_EDITABLE, BM_SETCHECK, 1, 0L);
	else if(IsKeepCurrent())
		SendDlgItemMessage(m_hwndDlg, RB_KEEPCURRENT, BM_SETCHECK, 1, 0L);
	else
		SendDlgItemMessage(m_hwndDlg, RB_NEITHER, BM_SETCHECK, 1, 0L);

	FormatTime(m_time, m_style, tmpStr, sizeof(tmpStr) );
	SetDlgItemText(m_hwndDlg, ED_TITLE, tmpStr);

	strcpy(m_szTime, tmpStr);

	EnableProperControls();
};


EditTime::EditTime(OBJECTID	oi):Time(oi, FALSE)
{
	LPTime timeSrc;

	timeSrc = LockTimeObject(oi);

	m_version	= timeSrc->m_version;
	m_oiSelf = timeSrc->m_oiSelf;
	m_style	= timeSrc->m_style;
	m_time = timeSrc->m_time;	
				
	_fmemcpy(&m_attributes, &timeSrc->m_attributes, sizeof(ATTRIBUTES) );

	UnlockTimeObject(oi);

};

EditTime::~EditTime()
{
}

