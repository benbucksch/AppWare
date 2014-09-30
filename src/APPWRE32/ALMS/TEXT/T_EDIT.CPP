
#include "t_includ.h"
#include "t_dialog.h"
#include "helpids.h"

LRESULT  pascal  TextEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL IsButtonReallyChecked(HWND hwndDlg, int idItem);

//////////////////////////////////////////////////

LONG EditTextObject(
	AOBJECTID            oi)
	{
	BOOL								bEdited;

	bEdited = DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(TEXT_SETUP_DIALOG),	GetActiveWindow(),	(DLGPROC)TextEditDlg, (LONG) oi);

	if (bEdited)
		return A_MODIFIED;
	else
  	return A_OK;
	}

///////////////////////////////////////////////////////

LRESULT  pascal  TextEditDlg(
	HWND                    hwnd,
	UINT                    msg,
	WPARAM                  wParam,
	LPARAM                  lParam )
	{
	LPEDIT_TEXT             et;
	WORD                    wNotifyCode, wControlID;
	BOOL										bTranslated;

	switch(msg)
		{
		case WM_INITDIALOG:
			et = new EDIT_TEXT( (AOBJECTID) lParam );
			SetWindowLong(hwnd, DWL_USER, (long)et);
			et->m_hwndDlg = hwnd;
			et->InitDialog();
			SetFocus( GetDlgItem(hwnd, ED_TITLE) );
			SendDlgItemMessage(hwnd, ED_TITLE, EM_SETSEL, 0, -1); 
			//AUtlSetTitleFont(OTYPE_TEXT, NULL, GetDlgItem(hwnd, ST_OBJECTNAME) );
			AUtlCenterDialog(hwnd, 0);
			return FALSE;

		case WM_DESTROY:
			et = (LPEDIT_TEXT) GetWindowLong(hwnd, DWL_USER);
			delete et;
			return FALSE;

		case WM_COMMAND:
			et = (LPEDIT_TEXT) GetWindowLong(hwnd, DWL_USER);
			wNotifyCode = HIWORD(wParam);
			wControlID = LOWORD(wParam);
			switch(wControlID)
				{
				case RB_LEFT:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
            et->m_txtPtr->SetJustifiesLeft();
						et->EnableProperControls();
            }
					break;
				case RB_CENTER:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
            et->m_txtPtr->SetJustifiesCenter();
						et->EnableProperControls();
            }
					break;
				case RB_RIGHT:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
            et->m_txtPtr->SetJustifiesRight();
						et->EnableProperControls();
            }
					break;

				case RB_UPPERCASE:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
            et->m_txtPtr->SetUppercase();
						et->EnableProperControls();
            }
					break;

				case RB_LOWERCASE:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
						et->m_txtPtr->SetLowercase();
						et->EnableProperControls();
            }
					break;

				case RB_CASEINSENSITIVE:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
            et->m_txtPtr->SetIsCaseInsensitive();
						et->EnableProperControls();
            }
					break;

				case RB_MULTILINE:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
            et->m_txtPtr->SetIsMultiline();
						et->EnableProperControls();
            }
					break;
				case RB_SINGLELINE:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
						et->m_txtPtr->SetIsSingleline();
						et->EnableProperControls();
            }
					break;
				case RB_STATIC:
					if (wNotifyCode == BN_CLICKED && SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L))
						{
						et->m_txtPtr->SetIsStatic();
						et->EnableProperControls();
            }
					break;

				case CB_USESENTER:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetUsesEnter((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case CB_USESTAB:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetUsesTab((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case CB_SELECTALL:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetSelectsAll((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;
				
				case CB_KEEPSELECTION:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetKeepsSelection((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case CB_HORZSCROLL:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetHasHScroll((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case CB_VERTSCROLL:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetHasVScroll((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case CB_FRAME:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetHasFrame((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case CB_EDITABLE:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetIsEditable((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case CB_PASSWORD:
					if (wNotifyCode == BN_CLICKED)
          	{
						et->m_txtPtr->SetIsPassword((BOOL)SendDlgItemMessage(hwnd, wControlID, BM_GETCHECK, 0, 0L) );
						et->EnableProperControls();
            }
					break;

				case ED_TABSIZE:
					if (wNotifyCode == EN_KILLFOCUS)
          	{
						et->m_txtPtr->SetTabSize(GetDlgItemInt(hwnd, ED_TABSIZE, &bTranslated, FALSE) );
						SetDlgItemInt(hwnd, ED_TABSIZE, et->m_txtPtr->GetTabSize(), FALSE);
						}
					break;

				case ED_LIMIT:
					if (wNotifyCode == EN_KILLFOCUS)
						{
						et->m_txtPtr->SetMaxNumChars(GetDlgItemInt(hwnd, ED_LIMIT, &bTranslated, FALSE) );
						SetDlgItemInt(hwnd, ED_LIMIT, et->m_txtPtr->GetMaxNumChars(), FALSE);
						SendDlgItemMessage(hwnd, ED_TITLE, EM_LIMITTEXT, et->m_txtPtr->GetMaxNumChars(), 0L);
						GetDlgItemText(hwnd, ED_TITLE, et->m_szTitle, et->m_txtPtr->GetMaxNumChars());
						SetDlgItemText(hwnd, ED_TITLE, et->m_szTitle);
						}
					break;

				case ED_TITLE:
					if (wNotifyCode == EN_KILLFOCUS)
						GetDlgItemText(hwnd, ED_TITLE, et->m_szTitle, 32000);
					break;

				case PB_OK:
					SetFocus(hwnd); //cause title and format to get a killfocus..
					et->Store();
					EndDialog(hwnd, TRUE);
					break;

				case PB_CANCEL:
					EndDialog(hwnd, FALSE);
					break;

				case PB_FONTS:
					GetDlgItemText(hwnd, ED_TITLE, et->m_szTitle, et->m_txtPtr->GetMaxNumChars() );
					et->m_txtPtr->EditFont(hwnd, *(et->m_szTitle) ? et->m_szTitle : "AaBbYyZz123", 
						!et->m_txtPtr->IsStatic()  ||  et->m_txtPtr->HasFrame() );
					break;

				case PB_HELP:
					WinHelp(hwnd, "alm_text.hlp", HELP_CONTEXT, HELPID_OBJD);
					break;
				}
			break;
		}

	return FALSE;
	}


///////////////////////////////////////

void EDIT_TEXT::EnableProperControls()
	{
	char										szFrameName[255];

	/*
	ShowWindow(GetDlgItem(m_hwndDlg, CB_EDITABLE	), 		m_txtPtr->IsStatic() ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(m_hwndDlg, CB_USESENTER), 		m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(m_hwndDlg, CB_USESTAB	), 			m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(m_hwndDlg, CB_VERTSCROLL), 		m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(m_hwndDlg, CB_HORZSCROLL), 		m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(m_hwndDlg, CB_SELECTALL), 		m_txtPtr->IsStatic() ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(m_hwndDlg, CB_KEEPSELECTION), m_txtPtr->IsStatic() ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(m_hwndDlg, RB_LEFT  ), 				m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(m_hwndDlg, RB_CENTER), 				m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(m_hwndDlg, RB_RIGHT ), 				m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(m_hwndDlg, ST_JUSTIFICATION), m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(m_hwndDlg, CB_PASSWORD), 			m_txtPtr->IsSingleline() ? SW_SHOW : SW_HIDE);
	*/
	EnableWindow(GetDlgItem(m_hwndDlg, CB_EDITABLE	), 		m_txtPtr->IsStatic() ? SW_HIDE : SW_SHOW);
	EnableWindow(GetDlgItem(m_hwndDlg, CB_USESENTER), 		m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	EnableWindow(GetDlgItem(m_hwndDlg, CB_USESTAB	), 			m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	EnableWindow(GetDlgItem(m_hwndDlg, CB_VERTSCROLL), 		m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	EnableWindow(GetDlgItem(m_hwndDlg, CB_HORZSCROLL), 		m_txtPtr->IsMultiline() ? SW_SHOW : SW_HIDE);
	EnableWindow(GetDlgItem(m_hwndDlg, CB_SELECTALL), 		m_txtPtr->IsStatic() ? SW_HIDE : SW_SHOW);
	EnableWindow(GetDlgItem(m_hwndDlg, CB_KEEPSELECTION), m_txtPtr->IsStatic() ? SW_HIDE : SW_SHOW);
	EnableWindow(GetDlgItem(m_hwndDlg, RB_LEFT  ), 				m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	EnableWindow(GetDlgItem(m_hwndDlg, RB_CENTER), 				m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	EnableWindow(GetDlgItem(m_hwndDlg, RB_RIGHT ), 				m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	EnableWindow(GetDlgItem(m_hwndDlg, ST_JUSTIFICATION), m_txtPtr->IsSingleline() ? SW_HIDE : SW_SHOW);
	EnableWindow(GetDlgItem(m_hwndDlg, CB_PASSWORD), 			m_txtPtr->IsSingleline() ? SW_SHOW : SW_HIDE);

	LoadString(g_hInstance, m_txtPtr->IsStatic() ? STR_FRAMEANDBACK : STR_FRAME, szFrameName, sizeof(szFrameName) );
	SetWindowText(GetDlgItem(m_hwndDlg, CB_FRAME), szFrameName);
																							
	}

///////////////////

static BOOL IsButtonReallyChecked(
	HWND 										hwndDlg,
	int											idItem)
	{
	if (!IsWindowEnabled(GetDlgItem(hwndDlg, idItem)) )
		return FALSE;

	if (!IsWindowVisible(GetDlgItem(hwndDlg, idItem)) )
		return FALSE;

	return (SendMessage(GetDlgItem(hwndDlg, idItem), BM_GETCHECK, 0, 0L) == 1);
	}

///////////////////

void EDIT_TEXT::Store()
	{
	UINT						 				len;
	LPTEXTOBJECT									txt;
  LPSTR										string;

	m_txtPtr->SetUsesEnter(IsButtonReallyChecked(m_hwndDlg, CB_USESENTER) );
	m_txtPtr->SetUsesTab(IsButtonReallyChecked(m_hwndDlg, CB_USESTAB) );
	m_txtPtr->SetSelectsAll(IsButtonReallyChecked(m_hwndDlg, CB_SELECTALL) );
	m_txtPtr->SetKeepsSelection(IsButtonReallyChecked(m_hwndDlg, CB_KEEPSELECTION) );
	m_txtPtr->SetHasHScroll(IsButtonReallyChecked(m_hwndDlg, CB_HORZSCROLL) );
	m_txtPtr->SetHasVScroll(IsButtonReallyChecked(m_hwndDlg, CB_VERTSCROLL) );
	m_txtPtr->SetHasFrame(IsButtonReallyChecked(m_hwndDlg, CB_FRAME) );
	m_txtPtr->SetIsEditable(IsButtonReallyChecked(m_hwndDlg, CB_EDITABLE) );
	m_txtPtr->SetIsPassword(IsButtonReallyChecked(m_hwndDlg, CB_PASSWORD) );

	txt = LockTextStyle(m_oiStore);
	CopyMemory(txt, m_txtPtr, sizeof(TEXTOBJECT) );
	UnlockTextStyle(m_oiStore);

	len = (UINT)SendDlgItemMessage(m_hwndDlg, ED_TITLE, WM_GETTEXTLENGTH, 0, 0L);
	len = len < m_txtPtr->GetMaxNumChars() ? len : m_txtPtr->GetMaxNumChars();
	len++; //for the null byte

	ResizeTextString(m_oiStore, len);
	string = LockTextString(m_oiStore);
	GetDlgItemText(m_hwndDlg, ED_TITLE, string, len);
	UnlockTextString(m_oiStore);
	}

//////////////////////////

void EDIT_TEXT::InitDialog()
	{
	/*char		 								objectName[80];*/
	char		szType[256];
	char		szName[256];
  LPSTR										string;

	ATypeGetName(OTYPE_TEXT, szType, sizeof(szType));
	strcat(szType, ": ");
	
	AObjGetName(m_oiStore, szName/*objectName*/, sizeof(szName)/*80*/);
	strcat(szType, szName);
	SetWindowText(m_hwndDlg, szType/*objectName*/);

	if (m_txtPtr->JustifiesLeft())        SendDlgItemMessage(m_hwndDlg, RB_LEFT, BM_SETCHECK, 1, 0L);
	else if (m_txtPtr->JustifiesCenter()) SendDlgItemMessage(m_hwndDlg, RB_CENTER, BM_SETCHECK, 1, 0L);
	else                                  SendDlgItemMessage(m_hwndDlg, RB_RIGHT, BM_SETCHECK, 1, 0L);

	if (m_txtPtr->IsUppercase())        	SendDlgItemMessage(m_hwndDlg, RB_UPPERCASE, BM_SETCHECK, 1, 0L);
	else if (m_txtPtr->IsLowercase())   	SendDlgItemMessage(m_hwndDlg, RB_LOWERCASE, BM_SETCHECK, 1, 0L);
	else                                  SendDlgItemMessage(m_hwndDlg, RB_CASEINSENSITIVE, BM_SETCHECK, 1, 0L);

	if (m_txtPtr->IsMultiline())   				SendDlgItemMessage(m_hwndDlg, RB_MULTILINE, BM_SETCHECK, 1, 0L);
	else if (m_txtPtr->IsSingleline())  	SendDlgItemMessage(m_hwndDlg, RB_SINGLELINE, BM_SETCHECK, 1, 0L);
	else 														  	  SendDlgItemMessage(m_hwndDlg, RB_STATIC, BM_SETCHECK, 1, 0L);

	if (m_txtPtr->UsesEnter())     				SendDlgItemMessage(m_hwndDlg, CB_USESENTER, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->UsesTab())     					SendDlgItemMessage(m_hwndDlg, CB_USESTAB, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->HasHScroll())          	SendDlgItemMessage(m_hwndDlg, CB_HORZSCROLL, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->HasVScroll())          	SendDlgItemMessage(m_hwndDlg, CB_VERTSCROLL, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->HasFrame())        			SendDlgItemMessage(m_hwndDlg, CB_FRAME, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->SelectsAll())        		SendDlgItemMessage(m_hwndDlg, CB_SELECTALL, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->KeepsSelection())     	SendDlgItemMessage(m_hwndDlg, CB_KEEPSELECTION, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->IsEditable())						SendDlgItemMessage(m_hwndDlg, CB_EDITABLE, BM_SETCHECK, 1, 0L);
	if (m_txtPtr->IsPassword())						SendDlgItemMessage(m_hwndDlg, CB_PASSWORD, BM_SETCHECK, 1, 0L);

	SetDlgItemInt(m_hwndDlg, ED_LIMIT, m_txtPtr->GetMaxNumChars(), FALSE);
	SetDlgItemInt(m_hwndDlg, ED_TABSIZE, m_txtPtr->GetTabSize(), FALSE);
	SendDlgItemMessage(m_hwndDlg, ED_TITLE, EM_LIMITTEXT, m_txtPtr->GetMaxNumChars(), 0L);

	string = LockTextString(m_oiStore);
	SetDlgItemText(m_hwndDlg, ED_TITLE, string);
	UnlockTextString(m_oiStore);

	EnableProperControls();
	}


///////////////////////////

EDIT_TEXT::EDIT_TEXT(
	AOBJECTID								oi)
	{
	LPTEXTOBJECT									txtSource;
  LPSTR										string;

	m_miTxt = AMemAllocate(sizeof(TEXTOBJECT) );
	m_txtPtr = (LPTEXTOBJECT) AMemLock(m_miTxt);  //Leave unlocked untill destructor

	txtSource = LockTextStyle(oi);
	CopyMemory(m_txtPtr, txtSource, sizeof(TEXTOBJECT) );
	UnlockTextStyle(oi);


	m_miTitle = AMemAllocate(32001);
	m_szTitle = (LPSTR)AMemLock(m_miTitle); //Leave unlocked untill destructor

	string = LockTextString(oi);
	lstrcpy(m_szTitle, string);
  UnlockTextString(oi);

	m_oiStore = oi;
	};

//////////////////////////

EDIT_TEXT::~EDIT_TEXT()
	{
	AMemUnlock(m_miTxt);
	AMemUnlock(m_miTitle);

	AMemFree(m_miTxt);
	AMemFree(m_miTitle);
	}



