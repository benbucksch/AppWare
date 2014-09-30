#include "n_includ.h"			   
#include "n_dialog.h"
#include "helpids.h"

LRESULT CALLBACK NumberEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL IsButtonReallyChecked(HWND hwndDlg, int idItem);


//////////////////////////////////////////////////

LONG EditNumberObject(
	AOBJECTID            oi)
	{
	BOOL								bEdited;

	bEdited = DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(NUMBER_SETUP_DIALOG),	GetActiveWindow(),	(DLGPROC)NumberEditDlg, (LONG) oi);

	if (bEdited)
		return A_MODIFIED;
	else
  	return A_OK;
	}

///////////////////////////////////////////////////////

LRESULT CALLBACK NumberEditDlg(
	HWND                    hwnd,
	UINT                    msg,
	WPARAM                  wParam,
	LPARAM                  lParam )
	{
	LPEDIT_NUMBER           en;
	WORD                    wNotifyCode, wControlID;

	switch(msg)
		{
		case WM_INITDIALOG:
			en = new EDIT_NUMBER( (AOBJECTID) lParam );
			SetWindowLong(hwnd, DWL_USER, (long)en);
			en->SetWindowHandle(hwnd);
			en->InitDialog();
			SetFocus( GetDlgItem(hwnd, ED_TITLE) );
			SendDlgItemMessage(hwnd, ED_TITLE, EM_SETSEL, 0, -1);
//			AUtlSetTitleFont(OTYPE_NUMBER, NULL, GetDlgItem(hwnd, ST_OBJECTNAME) );
			AUtlCenterDialog(hwnd, 0);
			return FALSE;

		case WM_DESTROY:
			en = (LPEDIT_NUMBER) GetWindowLong(hwnd, DWL_USER);
			delete en;
			return FALSE;

		case WM_COMMAND: 
			en = (LPEDIT_NUMBER) GetWindowLong(hwnd, DWL_USER);
			wNotifyCode = HIWORD(wParam);
			wControlID = LOWORD(wParam);
			switch(wControlID)
				{
				case RB_INUMBER:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
						en->SetIsInteger();
						ValidateNumberFormatString(en->GetFormatPtr(), en->GetFormatInfoPtr(), en->IsInteger());
						SetDlgItemText(hwnd, ED_FORMAT, en->GetFormatPtr());
						en->FormatNumber();
						SetDlgItemText(hwnd, ED_TITLE, en->GetStringPtr());
						}
					break;

				case RB_RNUMBER:			
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
						en->SetIsReal();
						ValidateNumberFormatString(en->GetFormatPtr(), en->GetFormatInfoPtr(), en->IsInteger());
						SetDlgItemText(hwnd, ED_FORMAT, en->GetFormatPtr());
						en->FormatNumber();
						SetDlgItemText(hwnd, ED_TITLE, en->GetStringPtr());
						}
					break;

				case RB_LEFT:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						en->SetJustifiesLeft();
					break;
				case RB_RIGHT:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						en->SetJustifiesRight();
					break;
				case CB_SELECTALL:
					if (wNotifyCode == BN_CLICKED)
						en->SetSelectsAll((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
					break;

				case CB_FRAME:
					if (wNotifyCode == BN_CLICKED)
						en->SetHasFrame((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
					break;

				case RB_NOTACTIVATE:
					if (wNotifyCode == BN_CLICKED)
          	{
						en->SetIsActivatable(FALSE);
						en->SetIsEditable(FALSE);
						EnableWindow(GetDlgItem(hwnd, CB_FRAME), FALSE);
						EnableWindow(GetDlgItem(hwnd, CB_SELECTALL), FALSE);
            }
					break;

				case RB_ACTIVATEONLY:
					if (wNotifyCode == BN_CLICKED)
          	{
						en->SetIsActivatable(TRUE);
						en->SetIsEditable(FALSE);
						EnableWindow(GetDlgItem(hwnd, CB_FRAME), TRUE);
						EnableWindow(GetDlgItem(hwnd, CB_SELECTALL), FALSE);
            }
					break;


				case RB_ACTIVATEEDIT:
					if (wNotifyCode == BN_CLICKED)
          	{
						en->SetIsEditable(TRUE);
						en->SetIsActivatable(TRUE);
						EnableWindow(GetDlgItem(hwnd, CB_FRAME), TRUE);
						EnableWindow(GetDlgItem(hwnd, CB_SELECTALL), TRUE);
            }
					break;

				case CB_DISPLAYEMPTY:
					if (wNotifyCode == BN_CLICKED)
						en->SetDisplaysEmpty( SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) ? BLANK_IF_EMPTY_STRING : DONT_BLANK_IF_EMPTY_STRING);
					break;

				case ED_TITLE:
					if (wNotifyCode == EN_KILLFOCUS)
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
					if (wNotifyCode == EN_KILLFOCUS)
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
						IsButtonReallyChecked(hwnd, RB_ACTIVATEEDIT) || 
						IsButtonReallyChecked(hwnd, RB_ACTIVATEONLY) || 
						IsButtonReallyChecked(hwnd, CB_FRAME) );
					break;

				case PB_HELP:
					WinHelp(hwnd, "awusnumbr.hlp", HELP_CONTEXT, HELPID_OBJD);
					break;
				}
			break;
		}

	return FALSE;
	}


static BOOL IsButtonReallyChecked(
	HWND 										hwndDlg,
	int											idItem)
	{
//	if (!IsWindowEnabled(GetDlgItem(hwndDlg, idItem)) )
//		return FALSE;

	return (SendMessage(GetDlgItem(hwndDlg, idItem), BM_GETCHECK, 0, 0L) == 1);
	}

///////////////////

void EDIT_NUMBER::Store()
	{
	LPNUMBER								numSource;

	if(IsButtonReallyChecked(m_hwndDlg, CB_FRAME))
	{
		if(IsWindowEnabled(GetDlgItem(m_hwndDlg, CB_FRAME)))
		{
			fDisabledFrame = FALSE;
			SetHasFrame(TRUE);
		}
		else
		{
			fDisabledFrame = TRUE;
			SetHasFrame(FALSE);
		}
	}
	else
	{
		fDisabledFrame = FALSE;		
		SetHasFrame(FALSE);
	}

	// Activatable
	if(IsButtonReallyChecked(m_hwndDlg, RB_ACTIVATEEDIT))
	{
		SetIsActivatable(TRUE);
		SetIsEditable(TRUE);
	}
	else if (IsButtonReallyChecked(m_hwndDlg, RB_ACTIVATEONLY))
	{
		SetIsActivatable(TRUE);
		SetIsEditable(FALSE);
	}
    else
	{
		SetIsActivatable(FALSE);
		SetIsEditable(FALSE);
	}

	SetDisplaysEmpty(IsButtonReallyChecked(m_hwndDlg, CB_DISPLAYEMPTY) ? BLANK_IF_EMPTY_STRING : DONT_BLANK_IF_EMPTY_STRING);

	numSource = LockNumberObject(m_oiSelf);

	numSource->m_version 				= m_version;
	numSource->m_oiSelf 				= m_oiSelf;
	numSource->m_style 					= m_style;

	if (IsReal())
		numSource->m_floatValue 	= m_floatValue;
	else
		numSource->m_wholeValue		= m_wholeValue;

	CopyMemory(&numSource->m_attributes, &m_attributes, sizeof(ATTRIBUTES) );

	if (IsReal())
		{
		lstrcpy(numSource->m_szFormat, m_szFormatReal);
		CopyMemory(&numSource->m_formatInfo, &m_formatInfoReal, sizeof(NFORM_INFO));
		}
	else
		{
		lstrcpy(numSource->m_szFormat, m_szFormatInteger);
		CopyMemory(&numSource->m_formatInfo, &m_formatInfoInteger, sizeof(NFORM_INFO));
		}
	
	numSource->FormatNumber();

	UnlockNumberObject(m_oiSelf);
	}

//////////////////////////

void EDIT_NUMBER::InitDialog()
	{
	char	objectName[80];
	char    szTitle[256];



	ATypeGetName(OTYPE_NUMBER, szTitle, sizeof(szTitle));
	strcat(szTitle, ": ");

	AObjGetName(m_oiSelf, objectName, 80);
	strcat(szTitle, objectName);

	SetWindowText(m_hwndDlg, szTitle);

	if (JustifiesLeft())        SendDlgItemMessage(m_hwndDlg, RB_LEFT, BM_SETCHECK, 1, 0L);
	else                                  SendDlgItemMessage(m_hwndDlg, RB_RIGHT, BM_SETCHECK, 1, 0L);

	if (IsInteger())        		SendDlgItemMessage(m_hwndDlg, RB_INUMBER, BM_SETCHECK, 1, 0L);
	else                                  SendDlgItemMessage(m_hwndDlg, RB_RNUMBER, BM_SETCHECK, 1, 0L);

	if (HasFrame() || fDisabledFrame)        			
		SendDlgItemMessage(m_hwndDlg, CB_FRAME, BM_SETCHECK, 1, 0L);

	if (SelectsAll())        		
		SendDlgItemMessage(m_hwndDlg, CB_SELECTALL, BM_SETCHECK, 1, 0L);

 	if (IsActivatable())
	{
		if (IsEditable())		
			SendDlgItemMessage(m_hwndDlg, RB_ACTIVATEEDIT, BM_SETCHECK, 1, 0L);
		else
		{
			SendDlgItemMessage(m_hwndDlg, RB_ACTIVATEONLY, BM_SETCHECK, 1, 0L);
			EnableWindow(GetDlgItem(m_hwndDlg, CB_SELECTALL), FALSE);
		}
	}
	else
	{
		SendDlgItemMessage(m_hwndDlg, RB_NOTACTIVATE, BM_SETCHECK, 1, 0L);
		EnableWindow(GetDlgItem(m_hwndDlg, CB_FRAME), FALSE);
		EnableWindow(GetDlgItem(m_hwndDlg, CB_SELECTALL), FALSE);
	}

	if (DisplaysEmpty())				
		SendDlgItemMessage(m_hwndDlg, CB_DISPLAYEMPTY, BM_SETCHECK, 1, 0L);

	ValidateNumberFormatString(GetFormatPtr(), GetFormatInfoPtr(), IsInteger());
	SetDlgItemText(m_hwndDlg, ED_FORMAT, GetFormatPtr());
	FormatNumber();
	SetDlgItemText(m_hwndDlg, ED_TITLE, GetStringPtr());
	}


///////////////////////////

EDIT_NUMBER::EDIT_NUMBER(
	AOBJECTID								oi):NUMBER(oi, FALSE)
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

	CopyMemory(&m_attributes, &numSource->m_attributes, sizeof(ATTRIBUTES) );

	if (IsReal())
		{
    //Use the current format for the real
		lstrcpy(m_szFormatReal, numSource->m_szFormat);
		CopyMemory(&m_formatInfoReal, &numSource->m_formatInfo, sizeof(NFORM_INFO));
		//And the global for the integer
   	lstrcpy(m_szFormatInteger, g_szIntegerNumberFormat);
		CopyMemory(&m_formatInfoInteger, &g_integerNumberFormatInfo, sizeof(NFORM_INFO));
		}
	else
		{
    //Use the current format for the integer
		lstrcpy(m_szFormatInteger, numSource->m_szFormat);
		CopyMemory(&m_formatInfoInteger, &numSource->m_formatInfo, sizeof(NFORM_INFO));
    //Use the global for the real
		lstrcpy(m_szFormatReal, g_szRealNumberFormat);
		CopyMemory(&m_formatInfoReal, &g_realNumberFormatInfo, sizeof(NFORM_INFO));
    }

	if (IsEditable())					//Editable implies activatable
		SetIsActivatable(TRUE); //If made not editable, it will still be activatable


	UnlockNumberObject(oi);
	};

//////////////////////////

EDIT_NUMBER::~EDIT_NUMBER()
	{
	}

