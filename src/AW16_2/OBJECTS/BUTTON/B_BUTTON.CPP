#include "b_include.h"

/////////////////////////////////

LPVOID BUTTON::operator new(
	size_t									size, 
	OBJECTID 								id)
	{
	AObjSetDataCount(id, I_NUM_DATAS);
	AObjResizeData(id, I_DATA_STYLE, size);

	return AObjLockData(id, I_DATA_STYLE);	//Gets unlocked in the constructor
	}

/////////////////////////////////

BUTTON::BUTTON(
	OBJECTID 								id, 
	BOOL 										bIsTemp):	m_attributes(!bIsTemp) //If temp object don't createStuff in attributes
	{
	m_version = I_VERSION;
  m_oiSelf = id;
	*m_szTitle = 0;
	m_style = I_BF_DEFAULT_BUTTON_STYLE;
	m_bBeingTracked = FALSE;
	SetRect(&m_rcTracking, 0, 0, 0, 0);
	m_accelVKey = m_accelMKey = 0;

	AObjUnlockData(id, I_DATA_STYLE);
	}

/////////////////////////////////

BUTTON::BUTTON(	//Unsed to setup the virtual table
	BUTTON far *						ptr):	m_attributes(FALSE) //If setting up vTable don't createStuff in attributes
	{
	I_UNUSED(ptr);
	}

LPVOID BUTTON::operator new(
	size_t									size, 
	BUTTON far 							*ptr)
	{
	I_UNUSED(size);
	return ptr;
	}

/////////////////////////

BOOL BUTTON::SetButtonType(
	OBUTN_TYPE 						type) //return TRUE if the type was changed
	{
	BOOL										rVal=FALSE;

	if (GetButtonType() != type)
		{
		m_style &= ~I_BF_TYPE_BITS;
		switch(type)
			{
			case OBUTN_PUSH: 				m_style |= I_BF_PUSHBUTTON; break;
			case OBUTN_CHECK:				m_style |= I_BF_CHECKBOX; break;
			case OBUTN_RADIO:				m_style |= I_BF_RADIOBUTTON; break;
			case OBUTN_INVISIBLE:		m_style |= I_BF_INVISIBLE; break;
			case OBUTN_BITMAP:			m_style |= I_BF_BITMAP; break;
			}
		rVal = TRUE;
		}

	return rVal;
	}

/////////////////////////////////
OBUTN_TYPE BUTTON::GetButtonType(void)
	{
	OBUTN_TYPE						rVal;

	if (m_style & I_BF_PUSHBUTTON) 
		rVal = OBUTN_PUSH;
	else if (m_style & I_BF_CHECKBOX) 
		rVal =  OBUTN_CHECK;
	else if (m_style & I_BF_RADIOBUTTON) 
		rVal =  OBUTN_RADIO;
	else if (m_style & I_BF_INVISIBLE) 
		rVal =  OBUTN_INVISIBLE;
	else if (m_style & I_BF_BITMAP) 
		rVal =  OBUTN_BITMAP;
	else
		I_ASSERT(0);

	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::IsButtonWindowsType()
	{
	return m_style & I_BF_WINDOWS_BUTTONS ? TRUE : FALSE;
	}

/////////////////////////////////

BOOL BUTTON::SetBitmapStyle(
	BITMAP_STYLE 						style)
	{
	BOOL										rVal=FALSE;

	if (style != GetBitmapStyle())
		{
		m_style &= ~I_BF_BITMAP_STYLE_BITS;
		switch(style)
			{
			case I_CROP:		m_style |= I_BF_BMP_CROP; break;
			case I_STRETCH:	m_style |= I_BF_BMP_STRETCH; break;
			case I_ORIGPROP:m_style |= I_BF_BMP_ORIGPROP; break;
			}
		rVal = TRUE;
		}

	return rVal;
	}				 
					 
					 
/////////////////////////////////

BITMAP_STYLE BUTTON::GetBitmapStyle(void)
	{
	BITMAP_STYLE						rVal;

	if (m_style & I_BF_BMP_CROP)
		rVal = I_CROP;
	else if (m_style & I_BF_BMP_STRETCH)
		rVal = I_STRETCH;
	else
		rVal = I_ORIGPROP;

	return rVal;
	}

/////////////////////////////////


BOOL BUTTON::SetButtonIsDefault(
	BOOL										set)
	{
	BOOL										rVal =FALSE;

	if (set != IsButtonDefault())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_DEFAULT;
		else			m_style &= ~I_BF_DEFAULT;
		}
	return rVal;
	}

/////////////////////////////////
BOOL BUTTON::IsButtonDefault(void)
	{
	return m_style & I_BF_DEFAULT ? TRUE:FALSE;
	}

/////////////////////////////////

BOOL BUTTON::SetButtonIsCancel(
	BOOL 										set)
	{
	BOOL										rVal =FALSE;

	if (set != IsButtonCancel())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_CANCEL;
		else			m_style &= ~I_BF_CANCEL;
		}
	return rVal;
	}

/////////////////////////////////
BOOL BUTTON::IsButtonCancel(void)
	{
	return m_style & I_BF_CANCEL ? TRUE:FALSE;
	}
/////////////////////////////////

BOOL BUTTON::SetButtonSelected(	// Also posts toggled signals and OBJECT_CHANGED
	BOOL 										set)
	{
	BOOL										rVal=FALSE;

	if (DoesButtonSupportSelection()  &&  set != IsButtonSelected())
		{
		rVal = TRUE;
		if (set)	{m_style |= I_BF_SELECTED; PostToggledOnSignal();}
		else			{m_style &= ~I_BF_SELECTED; PostToggledOffSignal();}
		PostObjectChanged();
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::IsButtonSelected(void)
	{
	return DoesButtonSupportSelection() ? (m_style & I_BF_SELECTED ? TRUE:FALSE) : FALSE;
	}

/////////////////////////////////

BOOL BUTTON::DoesButtonSupportSelection()
	{
	return m_style & I_BF_USES_SELECT_BITS ? TRUE:FALSE;
	}

/////////////////////////////////

BOOL BUTTON::SetButtonEnabled(
	BOOL 										set)//Also posts object changed
	{
	BOOL										rVal =FALSE;

	if (set != IsButtonEnabled())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_ENABLED;
		else			m_style &= ~I_BF_ENABLED;
		AppearanceChanged(I_APPEARANCE_CHANGED_ENABLE);
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::IsButtonEnabled(void)
	{
	return m_style & I_BF_ENABLED ? TRUE:FALSE;
	}

/////////////////////////////////

BOOL BUTTON::SetButtonInverts(
	BOOL 										set)
	{
	BOOL										rVal =FALSE;

	if (set != DoesButtonInvert())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_INVERTS;
		else			m_style &= ~I_BF_INVERTS;
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::DoesButtonInvert(void)
	{
	return m_style & I_BF_INVERTS ? TRUE:FALSE;
	}

/////////////////////////////////

BOOL BUTTON::SetButtonIsCapturedByKey(
	BOOL 										set)
	{
	BOOL										rVal =FALSE;

	if (set != IsButtonCapturedByKey())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_CAPTUREDBYKEY;
		else			m_style &= ~I_BF_CAPTUREDBYKEY;
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::IsButtonCapturedByKey(void)
	{
	return m_style & I_BF_CAPTUREDBYKEY ? TRUE:FALSE;
	}

/////////////////////////////////

BOOL BUTTON::SetTitleHasBeenEdited(
	BOOL 										set)
	{
	BOOL										rVal =FALSE;

	if (set != HasTitleBeenEdited())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_TITLEEDITED;
		else			m_style &= ~I_BF_TITLEEDITED;
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::HasTitleBeenEdited()
	{
	return m_style & I_BF_TITLEEDITED ? TRUE:FALSE;
	}

/////////////////////////////////


BOOL BUTTON::MakeSureButtonHasTitle()
	{
	BOOL										rVal=FALSE;
	char										ch[2];

	if (!HasTitleBeenEdited())
		{
		if ( AObjGetName(m_oiSelf, ch, 2)) //Will return 0 if called at runtime
			{
			rVal = TRUE;
			AObjGetName(m_oiSelf, m_szTitle, I_TITLE_LEN);
			}
		}

	return rVal;
	}

/////////////////////////////////


BOOL BUTTON::SetNormalBitmapExists(
	BOOL 										set)
	{
	BOOL										rVal =FALSE;

	if (set != DoesNormalBitmapExist())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_BMP_NORMAL_EXISTS;
		else			m_style &= ~I_BF_BMP_NORMAL_EXISTS;
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::DoesNormalBitmapExist(void)
	{
	return m_style & I_BF_BMP_NORMAL_EXISTS ? TRUE : FALSE;
	}

/////////////////////////////////

BOOL BUTTON::SetFocusedBitmapExists(
	BOOL 										set)
	{
	BOOL										rVal =FALSE;

	if (set != DoesFocusedBitmapExist())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_BMP_FOCUSED_EXISTS;
		else			m_style &= ~I_BF_BMP_FOCUSED_EXISTS;
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::DoesFocusedBitmapExist(void)
	{
	return m_style & I_BF_BMP_FOCUSED_EXISTS ? TRUE : FALSE;
	}


/////////////////////////////////

BOOL BUTTON::SetPressedBitmapExists(
	BOOL 										set)
	{
	BOOL										rVal =FALSE;

	if (set != DoesPressedBitmapExist())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_BMP_PRESSED_EXISTS;
		else			m_style &= ~I_BF_BMP_PRESSED_EXISTS;
		}
	return rVal;
	}

/////////////////////////////////
BOOL BUTTON::DoesPressedBitmapExist(void)
	{
	return m_style & I_BF_BMP_PRESSED_EXISTS ? TRUE : FALSE;
	}


/////////////////////////////////

BOOL BUTTON::SetGrayedBitmapExists(
	BOOL 										set)			
	{
	BOOL										rVal =FALSE;

	if (set != DoesGrayedBitmapExist())
		{
		rVal = TRUE;
		if (set)	m_style |= I_BF_BMP_GRAYED_EXISTS;
		else			m_style &= ~I_BF_BMP_GRAYED_EXISTS;
		}
	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::DoesGrayedBitmapExist(void)
	{
	return m_style & I_BF_BMP_GRAYED_EXISTS ? TRUE : FALSE;
	}

/////////////////////////////////


BOOL BUTTON::CreateBitmapsHandles(
	HPALETTE								hPalette,
	LPRECT									rect, 
	HBITMAP* 								hbmpNormal, 
	HBITMAP* 								hbmpFocused, 
	HBITMAP* 								hbmpPressed, 
	HBITMAP* 								hbmpGrayed)
	{
	BMP_DATA*								bmpData;	 
	POINT										ptSize;

	ptSize.x = rect->right - rect->left;
	ptSize.y = rect->bottom - rect->top;

	if (DoesNormalBitmapExist())
		{
		bmpData = LockNormalBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return FALSE);

		*hbmpNormal = bmpData->CreateMemBitmap(hPalette, &ptSize, GetBitmapStyle());
		UnlockNormalBitmap(m_oiSelf);
		}
	else
		{
		*hbmpNormal = CreateDefaultNormalBitmap();
		}

	if (DoesFocusedBitmapExist())
		{
		bmpData = LockFocusedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return FALSE);

		*hbmpFocused = bmpData->CreateMemBitmap(hPalette, &ptSize, GetBitmapStyle());
		UnlockFocusedBitmap(m_oiSelf);
		}
	else
		{
		*hbmpFocused = CreateDefaultFocusedBitmap(hPalette, *hbmpNormal);
		}

	if (DoesPressedBitmapExist())
		{
		bmpData = LockPressedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return FALSE);

		*hbmpPressed = bmpData->CreateMemBitmap(hPalette, &ptSize, GetBitmapStyle());
		UnlockPressedBitmap(m_oiSelf);
		}
	else
		{
		*hbmpPressed = CreateDefaultPressedBitmap(hPalette, *hbmpNormal);
		}
	
	if (DoesGrayedBitmapExist())
		{
		bmpData = LockGrayedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return FALSE);

		*hbmpGrayed = bmpData->CreateMemBitmap(hPalette, &ptSize, GetBitmapStyle());
		UnlockGrayedBitmap(m_oiSelf);
		}
	else
		{
		*hbmpGrayed =  CreateDefaultGrayedBitmap(hPalette, *hbmpNormal);
		}

	return TRUE;
	}

///////////////////////////////

LONG BUTTON::GetCreateWindowStyleBits()
	{
	LONG      							rVal = WS_CHILD;	
	
	switch(GetButtonType())
		{
		case OBUTN_PUSH:		rVal |= BS_PUSHBUTTON; break;
		case OBUTN_CHECK:		rVal |= BS_AUTOCHECKBOX; break;
		case OBUTN_RADIO:		rVal |= BS_AUTORADIOBUTTON|WS_GROUP;break;	 //WS_GROUP is so MicroSUCKS doesn't uncheck all others in the window
		default:							I_ASSERT(0);
		}

	if (!IsButtonEnabled())				rVal |= WS_DISABLED;

	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::SetControlsText(
	HWND 										hwnd) //returns TRUE if the text was changed
	{
	char										szCurrentText[I_TITLE_LEN+1];
	BOOL										rVal = FALSE;

	GetWindowText(hwnd, szCurrentText, I_TITLE_LEN+1);

	if (lstrcmp(szCurrentText, m_szTitle) != 0)
		{
		SetWindowText(hwnd, m_szTitle);
		rVal = TRUE;
		}

	return rVal;
	}

/////////////////////////////////

BOOL BUTTON::SetCheckState(
	HWND 										hwnd)  //returns TRUE if the check state was changed. 		
	{
	BOOL										bCurrentState;
	BOOL										rVal = FALSE;

	bCurrentState = (BOOL)SendMessage(hwnd, BM_GETCHECK, 0, 0L);
		
	if (bCurrentState != IsButtonSelected())
		{
		SendMessage(hwnd, BM_SETCHECK, IsButtonSelected(), 0L);
		rVal = TRUE;
		}
	return rVal;
	}

/////////////////////////////////		 

void BUTTON::BuildSignals(
	MEMBLOCKID 							miSignalBlock)
	{
	int											signalNum, numSignals;
	pAObjSignalRecord				signal;

	if (GetButtonType() == OBUTN_RADIO  ||  GetButtonType() ==OBUTN_CHECK)
		numSignals = 7;
	else
		numSignals = 5;

	signalNum = 0;

	AMemSetSize(miSignalBlock, numSignals * sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(miSignalBlock);

	signal[signalNum].theID = I_SIG_PUSHED;
	lstrcpy(signal[signalNum].theName, "Pressed");
	signalNum++;

	signal[signalNum].theID = I_SIG_TOUCHED;
	lstrcpy(signal[signalNum].theName, "Touched");
	signalNum++;

	signal[signalNum].theID = I_SIG_ISDOWN;
	lstrcpy(signal[signalNum].theName, "While Down");
	signalNum++;

	if (GetButtonType() == OBUTN_RADIO  ||  GetButtonType() ==OBUTN_CHECK)
	 	{
		signal[signalNum].theID = I_SIG_TOGGLED_ON;
		lstrcpy(signal[signalNum].theName, "Toggled On");
		signalNum++;

		signal[signalNum].theID = I_SIG_TOGGLED_OFF;
		lstrcpy(signal[signalNum].theName, "Toggled Off");
		signalNum++;
		}

	signal[signalNum].theID = I_SIG_MOUSE_UP;
	lstrcpy(signal[signalNum].theName, "Mouse Released");
	signalNum++;

	signal[signalNum].theID = I_SIG_DOUBLECLICK;
	lstrcpy(signal[signalNum].theName, "Double-clicked");
	signalNum++;

	AMemUnlock(miSignalBlock);
	}

/////////////////////////////////		 

OpWndItemD* BUTTON::CreateVisualInstance(
	OBJECTID 								oiItem, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status)
	{
	OpWndItemD*							rVal;

	switch(GetButtonType())
		{
		case OBUTN_PUSH:				rVal = new PUSH_VI(oiItem, oiParent, rcItem, status);				break;
		case OBUTN_CHECK:				rVal = new CHECK_VI(oiItem, oiParent, rcItem, status);			break;
		case OBUTN_RADIO:				rVal = new RADIO_VI(oiItem, oiParent, rcItem, status);			break;
		case OBUTN_INVISIBLE:	
			if (DoesButtonInvert())	rVal = new INVISIBLE_INVERTS_VI(oiItem, oiParent, rcItem, status);	
			else										rVal = new INVISIBLE_VI(oiItem, oiParent, rcItem, status);	
			break;
		case OBUTN_BITMAP:			rVal = new BITMAP_VI(oiItem, oiParent, rcItem, status);			break;
		default:									rVal = NULL; I_ASSERT(0);			break;
		}
	return rVal;
	}

/////////////////////////////////		 

BOOL BUTTON::SetTitle(
	LPCSTR 									szTitle) //Also posts OBJECT_CHANGED
	{
	BOOL										rVal=FALSE;
	
	if (lstrcmp(szTitle, m_szTitle))
		{
		lstrcpy(m_szTitle, szTitle);

		if (IsButtonWindowsType())
			GetAccelKeyFromTitle();

		rVal = TRUE;
		AppearanceChanged(I_APPEARANCE_CHANGED_TITLE);
		}
	return rVal;
	}

/////////////////////////////////		 

void BUTTON::GetAccelKeyFromTitle()
	{
	LPSTR										chp;
  int											num;
	char										chPossible;

	m_accelVKey = m_accelMKey = 0;

	if (strlen(m_szTitle))
		{
		//start at the end of the title. go back until reaching an '&'.  Char in front is the accel key
		//if the are a non-even number of '&' together.
		for(chPossible=0,chp = &m_szTitle[strlen(m_szTitle)-1]; chp >= m_szTitle  &&  !m_accelVKey; chp--)
			{
			num = 0;
			while (*chp == '&' && chp >= m_szTitle)
				{
				chp--;
				num++;
				}
			if (num % 2  &&  isalnum(chPossible)) //non-even number of &'s and alphaNumeric before "&&&&&' started
				{
				m_accelVKey = chPossible;
				m_accelMKey = OWND_ALT;
				}//num % 2...

			chPossible = toupper(*chp);
			}//for

		}//if strlen...

	}

/////////////

BOOL BUTTON::SetAccelKey(
	char 										vKey, 
	char 										mKey)
	{
	BOOL										rVal=FALSE;
	int											i;

	if (m_accelVKey != vKey)
		{
		m_accelVKey = vKey;
		rVal = TRUE;
		}

	if ( IsButtonWindowsType()  &&  vKey)
		mKey = OWND_ALT; //if the button is a push, radio, or check, the modifier key is ALT.

	if (m_accelMKey != mKey)
		{
		m_accelMKey = mKey;
		rVal = TRUE;
		}


	if ( IsButtonWindowsType()  &&  rVal )
		{ //make sure the title to have a '&' in it to cause an underscore.
		vKey = toupper(vKey);
		for (i=0; i<lstrlen(m_szTitle); i++)
			{																																					  
			if ( (toupper(m_szTitle[i]) == vKey)  &&  (i==0 || m_szTitle[i-1] != '&') )
				{ //found where the '&' should go -- at i
				memmove(&m_szTitle[i+1], 	&m_szTitle[i], I_TITLE_LEN - i);
				m_szTitle[i] = '&';
				m_szTitle[I_TITLE_LEN-1]=0;
        break;
				}
			}
		}

	return rVal;
	}

/////////////////////////////////		 

void BUTTON::BeginTrackingButton(
	OpWndItemD* 						wi)
	{
	m_bBeingTracked = TRUE;

	CopyRect(&m_rcTracking, &wi->m_rcItem);
	OWndLPtoDP(wi->m_oiParent, (LPPOINT)&m_rcTracking, 2);
	MapWindowPoints(wi->m_hwndParent, HWND_DESKTOP, (LPPOINT)&m_rcTracking, 2);

	PostIsDownSignal();
	SendTrackingEvent();
	}

/////////////////////////////////		 

void BUTTON::EndTrackingButton(void)
	{
	m_bBeingTracked = FALSE;
 	AEvtPostStandard(m_oiSelf, OBUTN_EVT_TRACKBUTTON, 0, AEVT_ATMARK|AEVT_KILLWHAT|AEVT_KILLSELF|AEVT_KILLALLQUEUES);
 	}

/////////////////////////////////		 

void BUTTON::ProcessTrackingEvent()
	{
	POINT									ptMouse;

	if (m_bBeingTracked)
		{
		GetCursorPos(&ptMouse);

		if (PtInRect(&m_rcTracking, ptMouse))
			PostIsDownSignal();

		SendTrackingEvent();
		}
	}

//////////////////////////////////////

void BUTTON::ObjectToText(
	MEMBLOCKID 							miText)
	{
	LPSTR										szText;

	if (IsButtonSelected())
		{
		AMemSetSize(miText, 2);
		szText = (LPSTR)AMemLock(miText);
		I_ASSERT_POINTER(szText, return);
		*szText='X';*(szText+1)=0;
		AMemUnlock(miText);
		}
	else
		{
		AMemSetSize(miText, 1);
		szText = (LPSTR)AMemLock(miText);
		I_ASSERT_POINTER(szText, return);
		*szText = 0;
		AMemUnlock(miText);
		}
	}

//////////////////////////////////////

void BUTTON::TextToObject(
	MEMBLOCKID 							miText)
	{
	LPSTR										szText;

	szText = (LPSTR)AMemLock(miText);
	I_ASSERT_POINTER(szText, return);

	SetButtonSelected(*szText ? TRUE:FALSE);

	AMemUnlock(miText);
	}

//////////////////////////////////////

void BUTTON::AppearanceChanged(
	long 										fwWhatChanged)
	{

	switch(GetButtonType())
		{
		case OBUTN_PUSH:			OWndCallEachVisualInstance(m_oiSelf, (CALLVIPROC)PUSH_VI::AppearanceChanged, fwWhatChanged, 0L); break;
		case OBUTN_CHECK:			OWndCallEachVisualInstance(m_oiSelf, (CALLVIPROC)CHECK_VI::AppearanceChanged, fwWhatChanged, 0L); break;
		case OBUTN_RADIO:			OWndCallEachVisualInstance(m_oiSelf, (CALLVIPROC)RADIO_VI::AppearanceChanged, fwWhatChanged, 0L); break;
		case OBUTN_BITMAP:		OWndCallEachVisualInstance(m_oiSelf, (CALLVIPROC)BITMAP_VI::AppearanceChanged, fwWhatChanged, 0L); break;
		case OBUTN_INVISIBLE:	OWndCallEachVisualInstance(m_oiSelf, (CALLVIPROC)INVISIBLE_VI::AppearanceChanged, fwWhatChanged, 0L); break;
		}
	}

/////////////////////////////
///    UPSF Support   ///////
/////////////////////////////

static long ExportBool(
	LPSTR										szName,
	LONG										idx,
	BOOL										bValue)
	{
	return AUpsfExportField(szName, idx, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &bValue, sizeof(BOOL));
  }

static long ExportShort(
	LPSTR										szName,
	LONG										idx,
	short										sValue)
	{
	return AUpsfExportField(szName, idx, AUPSF_NATV_SHORT, AUPSF_UNIV_INTEGER, &sValue, sizeof(short));
  }

void LogComment(
	LPSTR										szMember)
	{
	static char							szComment[256];

	wsprintf(szComment, "Error importing \"%s\" -- using default.", szMember);
	AUpsfLogComment(szComment);
	}

/////////////////////////////////
LPSTR szUpsfVersion				= "VERSION";
LPSTR szUpsfType					= "TYPE";
LPSTR szUpsfEnabled				= "ENABLED";
LPSTR szUpsfSelected			= "SELECTED";
LPSTR szUpsfInverts				= "INVERTS";
LPSTR szUpsfPushStyle			= "PBSTYLE";
LPSTR szUpsfAccelVKey			= "ACCLRATR";
LPSTR szUpsfAccelMKey			= "ACCLRATM";
LPSTR szUpsfTitle					= "TITLE";
LPSTR szUpsfBmpStyle			= "NORSTYLE";
LPSTR szUpsfNormalBitmap	= "BMPNORML";
LPSTR szUpsfFocusedBitmap	= "BMPFOCSD";
LPSTR szUpsfPressedBitmap	= "BMPPRSSD";
LPSTR szUpsfGrayedBitmap	= "BMPGRAYD";
/////////////////////////////

long BUTTON::Export()
	{
	LPBMP_DATA							bmpData;

	ExportShort(szUpsfVersion, 0L, m_version);

	switch(GetButtonType())
		{
		case OBUTN_PUSH:					ExportShort(szUpsfType, 0L, 0); break;
		case OBUTN_CHECK:					ExportShort(szUpsfType, 0L, 1); break;
		case OBUTN_RADIO:					ExportShort(szUpsfType, 0L, 2); break;
		case OBUTN_INVISIBLE:			ExportShort(szUpsfType, 0L, 3); break;
		case OBUTN_BITMAP:				ExportShort(szUpsfType, 0L, 4); break;
		}

	if (IsButtonWindowsType()) //to make sure old project have the accelkey stuff set
		GetAccelKeyFromTitle();

	ExportBool(szUpsfEnabled, 		0L, IsButtonEnabled() );
	ExportBool(szUpsfSelected,		0L, IsButtonSelected() );
	ExportBool(szUpsfInverts, 		0L, DoesButtonInvert() );
	ExportShort(szUpsfPushStyle,	0L, IsButtonDefault() ? 1 : IsButtonCancel() ? 2 : 0);

	ExportShort(szUpsfAccelVKey, 	0L, (short)m_accelVKey);
	ExportShort(szUpsfAccelMKey, 	0L, (short)m_accelMKey);
	AUpsfExportField(szUpsfTitle, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, m_szTitle, lstrlen(m_szTitle));
			 
	m_attributes.Export();

	// Now, do the bitmaps

	ExportShort(szUpsfBmpStyle, 	0L, GetBitmapStyle()==I_CROP ? 1 : GetBitmapStyle() == I_STRETCH ? 2 : 4);
			 	
	if (DoesNormalBitmapExist())
		{
		bmpData = LockNormalBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	

		bmpData->Export(szUpsfNormalBitmap);
		UnlockNormalBitmap(m_oiSelf);
		}

	if (DoesFocusedBitmapExist())
		{
		bmpData = LockFocusedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	

		bmpData->Export(szUpsfFocusedBitmap);
		UnlockFocusedBitmap(m_oiSelf);
		}

	if (DoesPressedBitmapExist())
		{
		bmpData = LockPressedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	

		bmpData->Export(szUpsfPressedBitmap);
		UnlockPressedBitmap(m_oiSelf);
		}

	if (DoesGrayedBitmapExist())
		{
		bmpData = LockGrayedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	

		bmpData->Export(szUpsfGrayedBitmap);
		UnlockGrayedBitmap(m_oiSelf);
		}

	return A_OK;
	}

/////////////////////////////

long BUTTON::Import()
	{
	short										s;
	short										mKey, vKey;
	char										szVkey[2];
	BOOL										b;
	long										lBmpSize;
	LPBMP_DATA							bmpData;

	if(AUpsfImportField(szUpsfType, 0L, AUPSF_NATV_SHORT, &s, sizeof(short)) != AUPSF_NOERROR)
		LogComment(szUpsfType);
	else
		{
		switch(s)
			{
			case 0: SetButtonType(OBUTN_PUSH);			break;
			case 1: SetButtonType(OBUTN_CHECK);			break;
			case 2: SetButtonType(OBUTN_RADIO);			break;
			case 3: SetButtonType(OBUTN_INVISIBLE); break;
			case 4: SetButtonType(OBUTN_BITMAP); 		break;
			}
		}

	if(AUpsfImportField(szUpsfEnabled, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfEnabled);
	else
		SetButtonEnabled(b);

	if(AUpsfImportField(szUpsfSelected, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfSelected);
	else
		SetButtonSelected(b);

	if(AUpsfImportField(szUpsfInverts, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfInverts);
	else
		SetButtonInverts(b);
	
	if(AUpsfImportField(szUpsfPushStyle, 0L, AUPSF_NATV_SHORT, &s, sizeof(short)) != AUPSF_NOERROR)
		LogComment(szUpsfPushStyle);
	else
		{
		SetButtonIsDefault(s==1);
		SetButtonIsCancel(s==2);
		}

	if (AUpsfImportField(szUpsfTitle, 0L, AUPSF_NATV_STRING, m_szTitle, I_TITLE_LEN) != AUPSF_NOERROR)
		{
		LogComment(szUpsfTitle);
		SetTitleHasBeenEdited(FALSE);
		}
	else
		SetTitleHasBeenEdited(TRUE);


	if(AUpsfImportField(szUpsfAccelVKey, 0L, AUPSF_NATV_SHORT, &vKey, sizeof(short)) != AUPSF_NOERROR)
		{
		if(AUpsfImportField(szUpsfAccelVKey, 0L, AUPSF_NATV_STRING, szVkey, 2 ) != AUPSF_NOERROR)	//cause Workshop exported it as a string
			{
			LogComment(szUpsfAccelVKey);
			vKey = 0;
			}
		else
			vKey = *szVkey;
		}

	if(AUpsfImportField(szUpsfAccelMKey, 0L, AUPSF_NATV_SHORT, &mKey, sizeof(short)) != AUPSF_NOERROR)
		{
		LogComment(szUpsfAccelMKey);
		mKey = 0;
		}

	SetAccelKey(vKey, mKey);


	m_attributes.Import();

	/// Now, to do the bitmaps

	if(AUpsfImportField(szUpsfBmpStyle, 0L, AUPSF_NATV_SHORT, &s, sizeof(short)) != AUPSF_NOERROR)
		{
		LogComment(szUpsfBmpStyle);
		SetBitmapStyle(I_CROP);
		}
	else
		{
		if (s==1)				SetBitmapStyle(I_CROP);
		else if (s==2)	SetBitmapStyle(I_STRETCH);
		else						SetBitmapStyle(I_ORIGPROP);
		}
					 
	if(AUpsfFieldInfo(szUpsfNormalBitmap, 0L, AUPSF_NATV_BITMAPINFO, NULL, &lBmpSize) == AUPSF_NOERROR)
		{
		AObjResizeData(m_oiSelf, I_DATA_BMP_NORMAL, lBmpSize + sizeof(BITMAPFILEHEADER));
		bmpData = LockNormalBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	
		bmpData->Import(szUpsfNormalBitmap, lBmpSize);
		UnlockNormalBitmap(m_oiSelf);
		SetNormalBitmapExists(TRUE);
		}
	else 
		SetNormalBitmapExists(FALSE);

	if(AUpsfFieldInfo(szUpsfFocusedBitmap, 0L, AUPSF_NATV_BITMAPINFO, NULL, &lBmpSize) == AUPSF_NOERROR)
		{
		AObjResizeData(m_oiSelf, I_DATA_BMP_FOCUSED, lBmpSize + sizeof(BITMAPFILEHEADER));
		bmpData = LockFocusedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	
		bmpData->Import(szUpsfFocusedBitmap, lBmpSize);
		UnlockFocusedBitmap(m_oiSelf);
		SetFocusedBitmapExists(TRUE);
		}
	else 
		SetFocusedBitmapExists(FALSE);

	if(AUpsfFieldInfo(szUpsfPressedBitmap, 0L, AUPSF_NATV_BITMAPINFO, NULL, &lBmpSize) == AUPSF_NOERROR)
		{
		AObjResizeData(m_oiSelf, I_DATA_BMP_PRESSED, lBmpSize + sizeof(BITMAPFILEHEADER));
		bmpData = LockPressedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	
		bmpData->Import(szUpsfPressedBitmap, lBmpSize);
		UnlockPressedBitmap(m_oiSelf);
		SetPressedBitmapExists(TRUE);
		}
	else 
		SetPressedBitmapExists(FALSE);

	if(AUpsfFieldInfo(szUpsfGrayedBitmap, 0L, AUPSF_NATV_BITMAPINFO, NULL, &lBmpSize) == AUPSF_NOERROR)
		{
		AObjResizeData(m_oiSelf, I_DATA_BMP_GRAYED, lBmpSize + sizeof(BITMAPFILEHEADER));
		bmpData = LockGrayedBitmap(m_oiSelf);
		I_ASSERT_POINTER(bmpData, return A_ERROR);	
		bmpData->Import(szUpsfGrayedBitmap, lBmpSize);
		UnlockGrayedBitmap(m_oiSelf);
		SetGrayedBitmapExists(TRUE);
		}
	else 
		SetGrayedBitmapExists(FALSE);

	return A_OK;
	}
/////////////////////////////
