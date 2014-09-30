#include "b_include.h"


///////////////////////

INVISIBLE_VI::INVISIBLE_VI(
	OBJECTID 								oiItem, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status):OpLiteWndItem(oiItem, oiParent, rcItem, status)
	{
	LPBUTTON								but;
	m_state = I_UP;
	m_bTracking = FALSE;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	OWndSetItemTransparent(this, TRUE);
	OWndSetItemEnabled(this, but->IsButtonEnabled() );
	OWndSetItemActivatable(this, FALSE);

	SetAccelKey();

	UnlockButtonObject(Object());
	}

///////////////////////

void ALMAPI INVISIBLE_VI::Destroy()
	{
	delete this;
	}

///////////////////////////

void ALMAPI INVISIBLE_VI::AccelKeyHit(void)
	{
	AEvtPostSignalAtTail(Object(), I_SIG_PUSHED);
	}

///////////////////////

void ALMAPI INVISIBLE_VI::Draw(
	HDC 										hdc, 
	LPRECT 									rcInvalid)
	{
	I_UNUSED(hdc); I_UNUSED(rcInvalid);
	}

///////////////////////

void ALMAPI INVISIBLE_VI::MouseMove(
	POINT* 									where, 
	int 										keysDown)
	{
	I_UNUSED(keysDown);

	if (m_bTracking)
		ChangeState(PointInItem(where) ? I_DOWN: I_UP);
	}

///////////////////////
void ALMAPI INVISIBLE_VI::MouseDown(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPBUTTON								but;

	I_UNUSED(where);I_UNUSED(keysDown);

	if (button == MK_LBUTTON)
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return);

		OWndSetCaptureItem(this);
		m_bTracking = TRUE;
		m_bDoubleClicked = FALSE;
		ChangeState(I_DOWN);
		but->PostTouchedSignal();
		but->BeginTrackingButton(this);
		UnlockButtonObject(Object());
		}
	}

///////////////////////

void ALMAPI INVISIBLE_VI::DoubleClicked(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPBUTTON								but;

	I_UNUSED(where);I_UNUSED(keysDown);

	if (button == MK_LBUTTON)
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return);

		OWndSetCaptureItem(this);
		m_bTracking = TRUE;
		m_bDoubleClicked = TRUE;
		ChangeState(I_DOWN);
		but->PostTouchedSignal();
		but->BeginTrackingButton(this);
		UnlockButtonObject(Object());
		}
	}


///////////////////////

void ALMAPI INVISIBLE_VI::MouseUp(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPBUTTON								but;

	I_UNUSED(keysDown);

	if (button == MK_LBUTTON  &&  m_bTracking)
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return);

		but->EndTrackingButton();
		but->PostMouseUpSignal();	
		
		if (PointInItem(where))
			{
			if (m_bDoubleClicked)
				but->PostDoubleClickedSignal();
			else
				but->PostPushedSignal();
			}
		
		UnlockButtonObject(Object());

		OWndReleaseCapture();
		m_bTracking = FALSE;
		ChangeState(I_UP);
		}
	}

///////////////////////

void ALMAPI INVISIBLE_VI::CancelMode(void)
	{
	LPBUTTON								but;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);
	
	if (but->IsBeingTracked())
		{
		but->EndTrackingButton();
		but->PostMouseUpSignal();	
		}
	
	UnlockButtonObject(Object());

	OWndReleaseCapture();
	m_bTracking = FALSE;
	ChangeState(I_UP);
	}

///////////////////////

BOOL ALMAPI INVISIBLE_VI::AppearanceChanged(
	INVISIBLE_VI *					vi, 
	long 										fwWhatChanged, 
	long										)
	{
	LPBUTTON								but;

	but = LockButtonObject(vi->Object());
	I_ASSERT_POINTER(but, return TRUE);

	if (fwWhatChanged & I_APPEARANCE_CHANGED_ENABLE)
		OWndSetItemEnabled(vi, but->IsButtonEnabled() );
	
	UnlockButtonObject(vi->Object());
	return TRUE;
	}

///////////////////////

BOOL INVISIBLE_VI::ChangeState(
	INVISIBLE_STATE 				newState)
	{
	BOOL										rVal = FALSE;

	if (newState != m_state)
		{
		m_state = newState;
		rVal = TRUE;
		}
	return rVal;
	}

///////////////////////

BOOL INVISIBLE_VI::SetAccelKey()
	{
	LPBUTTON								but;
	BOOL										rVal=FALSE;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return FALSE);

	if (but->GetAccelVKey())
		rVal = OWndSetItemAccelKey(this, but->GetAccelVKey(), but->GetAccelMKey());

	UnlockButtonObject(Object());

	return rVal;
	}

///////////////////////

INVISIBLE_INVERTS_VI::INVISIBLE_INVERTS_VI(
	OBJECTID 								oiItem, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status):INVISIBLE_VI(oiItem, oiParent, rcItem, status)
	{
	}

//////////////

void ALMAPI INVISIBLE_INVERTS_VI::Draw(
	HDC 										hdc, 
	LPRECT 									rcInvalid)
	{
	HDC											hdcParent;

	I_UNUSED(hdc); I_UNUSED(rcInvalid);

	if (m_state == I_DOWN)
		{
		hdcParent = GetDC(m_hwndParent);
		InvertRect(hdcParent, &m_rcItem);
		ReleaseDC(m_hwndParent, hdcParent);
		}

	}

//////////////////

BOOL INVISIBLE_INVERTS_VI::ChangeState(
	INVISIBLE_STATE 				newState)
	{
	BOOL										rVal = FALSE;
	HDC											hdc;

	if (newState != m_state)
		{
		hdc = GetDC(m_hwndParent);
		InvertRect(hdc, &m_rcItem);
		ReleaseDC(m_hwndParent, hdc);
		m_state = newState;
		rVal = TRUE;
		}
	return rVal;
	}