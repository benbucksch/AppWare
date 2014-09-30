#include "n_includ.h"
#include "n_dialog.h"

LRESULT far pascal _export NumberEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL IsButtonReallyChecked(HWND hwndDlg, int idItem);


//////////////////////////////////////////////////

LONG EditNumberObject(
	OBJECTID            oi)
	{
	BOOL								bEdited;

	bEdited = DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(NUMBER_SETUP_DIALOG),	GetActiveWindow(),	(DLGPROC)NumberEditDlg, (LONG) oi);

	if (bEdited)
		return A_MODIFIED;
	else
  	return A_OK;
	}

///////////////////////////////////////////////////////

LRESULT far pascal _export NumberEditDlg(
	HWND                    hwnd,
	UINT                    msg,
	WPARAM                  wParam,
	LPARAM                  lParam )
	{
	LPEDIT_NUMBER           en;
	int                     nc;

	switch(msg)
		{
		case WM_INITDIALOG:
			en = new EDIT_NUMBER( (OBJECTID) lParam );
			SetWindowLong(hwnd, DWL_USER, (long)en);
			en->SetWindowHandle(hwnd);
			en->InitDialog();
			SetFocus( GetDlgItem(hwnd, ED_TITLE) );
			SendDlgItemMessage(hwnd, ED_TITLE, EM_SETSEL, 0, MAKELONG(0, -1));
			AUtlSetTitleFont(OTYPE_NUMBER, NULL, GetDlgItem(hwnd, ST_OBJECTNAME) );
			AUtlCenterDialog(hwnd, 0);
			return FALSE;

		case WM_DESTROY:
			en = (LPEDIT_NUMBER) GetWindowLong(hwnd, DWL_USER);
			delete en;
			return FALSE;

		case WM_COMMAND:
			en = (LPEDIT_NUMBER) GetWindowLong(hwnd, DWL_USER);
			nc = HIWORD(lParam);
			switch(wParam)
				{
				case RB_INUMBER:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
						{
						en->SetIsInteger();
						ValidateNumberFormatString(en->GetFormatPtr(), en->GetFormatInfoPtr(), en->IsInteger());
						SetDlgItemText(hwnd, ED_FORMAT, en->GetFormatPtr());
						en->FormatNumber();
						SetDlgItemText(hwnd, ED_TITLE, en->GetStringPtr());
						}
					break;

				case RB_RNUMBER:			
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
						{
						en->SetIsReal();
						ValidateNumberFormatString(en->GetFormatPtr(), en->GetFormatInfoPtr(), en->IsInteger());
						SetDlgItemText(hwnd, ED_FORMAT, en->GetFormatPtr());
						en->FormatNumber();
						SetDlgItemText(hwnd, ED_TITLE, en->GetStringPtr());
						}
					break;

				case RB_LEFT:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
						{
						en->SetJustifiesLeft();
						en->EnableProperControls();
            }
					break;
				case RB_RIGHT:
					if (nc == BN_CLICKED && SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
						{
						en->SetJustifiesRight();
						en->EnableProperControls();
            }
					break;
				case CB_SELECTALL:
					if (nc == BN_CLICKED)
          	{
						en->SetSelectsAll((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						en->EnableProperControls();
            }
					break;

				case CB_FRAME:
					if (nc == BN_CLICKED)
          	{
						en->SetHasFrame((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						en->EnableProperControls();
            }
					break;

				case CB_EDITABLE:
					if (nc == BN_CLICKED)
          	{
						en->SetIsEditable((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						en->EnableProperControls();
            }
					break;

				case CB_ACTIVATABLE:
					if (nc == BN_CLICKED)
          	{
						en->SetIsActivatable((BOOL)SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) );
						en->EnableProperControls();
            }
					break;

				case CB_DISPLAYEMPTY:
					if (nc == BN_CLICKED)
          	{
						en->SetDisplaysEmpty( SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L) ? BLANK_IF_EMPTY_STRING : DONT_BLANK_IF_EMPTY_STRING);
						en->EnableProperControls();
            }
					break;

				case ED_TITLE:
					if (nc == EN_KILLFOCUS)
          	{
						if (en->IsInteger() )
							en->SetValue( (long double)GetDlgEditFormatedInteger(hwnd, ED_TITLE, en->GetFormatPtr()), DONT_POST_SIGNALS, DONT_ROUND_IF_INT);
						else
							en->SetValue( GetDlgEditFormatedReal(hwnd, ED_TITLE, en->GetFormatPtr()), DONT_POST_SIGNALS, DONT_ROUND_IF_INT);

						en->FormatNumber();
						SetDlgItemText(hwnd, ED_TITLE, en->GetStringPtr());
						}
					break;

				case ED_FORMAT:
					if (nc == EN_KILLFOCUS)
						{
						GetDlgItemText(hwnd, ED_FORMAT, en->GetFormatPtr(), I_FORMAT_STRING_LENGTH);
						ValidateNumberFormatString(en->GetFormatPtr(), en->GetFormatInfoPtr(), en->IsInteger());
						SetDlgItemText(hwnd, ED_FORMAT, en->GetFormatPtr());
						en->FormatNumber();
						SetDlgItemText(hwnd, ED_TITLE, en->GetStringPtr());
						}
					break;

				case PB_OK:
					SetFocus(hwnd); //cause title and format to get a killfocus..
					en->Store();
					EndDialog(hwnd, TRUE);
					break;

				case PB_CANCEL:
					EndDialog(hwnd, FALSE);
					break;

				case PB_FONTS:
					en->EditFont(hwnd, "1,234,567.890", 
						IsButtonReallyChecked(hwnd, CB_EDITABLE) || 
						IsButtonReallyChecked(hwnd, CB_ACTIVATABLE) || 
						IsButtonReallyChecked(hwnd, CB_FRAME) );
					break;

				case PB_HELP:
					//WinHelp(hwnd, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Text);
					break;
				}
			break;
		}

	return FALSE;
	}


///////////////////////////////////////

static void EnableWindowAndSet(
	HWND										hwndControl,
	BOOL										bEnable,
	BOOL										bSet)
	{
	if (!bEnable)
		bSet = FALSE;

	EnableWindow(hwndControl, bEnable);
 	SendMessage(hwndControl, BM_SETCHECK, (WPARAM)bSet, 0L);
}

///////////////////////////////////////

void EDIT_NUMBER::EnableProperControls()
	{

	EnableWindowAndSet(GetDlgItem(m_hwndDlg, CB_FRAME), IsActivatable(), HasFrame() );
	ShowWindow(GetDlgItem(m_hwndDlg, CB_SELECTALL), IsEditable());
	ShowWindow(GetDlgItem(m_hwndDlg, CB_ACTIVATABLE), !IsEditable());
	}

///////////////////

static BOOL IsButtonReallyChecked(
	HWND 										hwndDlg,
	int											idItem)
	{
	if (!IsWindowEnabled(GetDlgItem(hwndDlg, idItem)) )
		return FALSE;

	return (SendMessage(GetDlgItem(hwndDlg, idItem), BM_GETCHECK, 0, 0L) == 1);
	}

///////////////////

void EDIT_NUMBER::Store()
	{
	LPNUMBER								numSource;

	SetHasFrame(IsButtonReallyChecked(m_hwndDlg, CB_FRAME));
	SetIsEditable(IsButtonReallyChecked(m_hwndDlg, CB_EDITABLE));
	SetIsActivatable(IsButtonReallyChecked(m_hwndDlg, CB_ACTIVATABLE));
	SetDisplaysEmpty(IsButtonReallyChecked(m_hwndDlg, CB_DISPLAYEMPTY) ? BLANK_IF_EMPTY_STRING : DONT_BLANK_IF_EMPTY_STRING);

	numSource = LockNumberObject(m_oiSelf);

	numSource->m_version 				= m_version;
	numSource->m_oiSelf 				= m_oiSelf;
	numSource->m_style 					= m_style;

	if (IsReal())
		numSource->m_floatValue 	= m_floatValue;
	else
		numSource->m_wholeValue		= m_wholeValue;

	_fmemcpy(&numSource->m_attributes, &m_attributes, sizeof(ATTRIBUTES) );

	if (IsReal())
		{
		lstrcpy(numSource->m_szFormat, m_szFormatReal);
		_fmemcpy(&numSource->m_formatInfo, &m_formatInfoReal, sizeof(NFORM_INFO));
		}
	else
		{
		lstrcpy(numSource->m_szFormat, m_szFormatInteger);
		_fmemcpy(&numSource->m_formatInfo, &m_formatInfoInteger, sizeof(NFORM_INFO));
		}
	
	numSource->FormatNumber();

	UnlockNumberObject(m_oiSelf);
	}

//////////////////////////

void EDIT_NUMBER::InitDialog()
	{
	char		 								objectName[80];

	AObjGetName(m_oiSelf, objectName, 80);
	SetWindowText(m_hwndDlg, objectName);

	if (JustifiesLeft())        SendDlgItemMessage(m_hwndDlg, RB_LEFT, BM_SETCHECK, 1, 0L);
	else                                  SendDlgItemMessage(m_hwndDlg, RB_RIGHT, BM_SETCHECK, 1, 0L);

	if (IsInteger())        		SendDlgItemMessage(m_hwndDlg, RB_INUMBER, BM_SETCHECK, 1, 0L);
	else                                  SendDlgItemMessage(m_hwndDlg, RB_RNUMBER, BM_SETCHECK, 1, 0L);

	if (HasFrame())        			SendDlgItemMessage(m_hwndDlg, CB_FRAME, BM_SETCHECK, 1, 0L);
	if (SelectsAll())        		SendDlgItemMessage(m_hwndDlg, CB_SELECTALL, BM_SETCHECK, 1, 0L);
	if (IsActivatable())      	SendDlgItemMessage(m_hwndDlg, CB_ACTIVATABLE, BM_SETCHECK, 1, 0L);
	if (IsEditable())						SendDlgItemMessage(m_hwndDlg, CB_EDITABLE, BM_SETCHECK, 1, 0L);
	if (DisplaysEmpty())				SendDlgItemMessage(m_hwndDlg, CB_DISPLAYEMPTY, BM_SETCHECK, 1, 0L);

	ValidateNumberFormatString(GetFormatPtr(), GetFormatInfoPtr(), IsInteger());
	SetDlgItemText(m_hwndDlg, ED_FORMAT, GetFormatPtr());
	FormatNumber();
	SetDlgItemText(m_hwndDlg, ED_TITLE, GetStringPtr());

	EnableProperControls();
	}


///////////////////////////

EDIT_NUMBER::EDIT_NUMBER(
	OBJECTID								oi):NUMBER(oi, FALSE)
	{
	LPNUMBER								numSource;

	numSource = LockNumberObject(oi);

	m_version 				= numSource->m_version;
	m_oiSelf 					= numSource->m_oiSelf;
	m_style 					= numSource->m_style;
	if (numSource->IsReal())
		m_floatValue 		= numSource->m_floatValue;
	else
		m_wholeValue 		= numSource->m_wholeValue;

	_fmemcpy(&m_attributes, &numSource->m_attributes, sizeof(ATTRIBUTES) );

	if (IsReal())
		{
    //Use the current format for the real
		lstrcpy(m_szFormatReal, numSource->m_szFormat);
		_fmemcpy(&m_formatInfoReal, &numSource->m_formatInfo, sizeof(NFORM_INFO));
		//And the global for the integer
   	lstrcpy(m_szFormatInteger, g_szIntegerNumberFormat);
		_fmemcpy(&m_formatInfoInteger, &g_integerNumberFormatInfo, sizeof(NFORM_INFO));
		}
	else
		{
    //Use the current format for the integer
		lstrcpy(m_szFormatInteger, numSource->m_szFormat);
		_fmemcpy(&m_formatInfoInteger, &numSource->m_formatInfo, sizeof(NFORM_INFO));
    //Use the global for the real
		lstrcpy(m_szFormatReal, g_szRealNumberFormat);
		_fmemcpy(&m_formatInfoReal, &g_realNumberFormatInfo, sizeof(NFORM_INFO));
    }

	if (IsEditable())					//Editable implies activatable
		SetIsActivatable(TRUE); //If made not editable, it will still be activatable


	UnlockNumberObject(oi);
	};

//////////////////////////

EDIT_NUMBER::~EDIT_NUMBER()
	{
	}

