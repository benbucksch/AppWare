#include "b_includ.h"

extern "C" void ALMAPI ButtonFunction_Clear(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	OBJECTID								oiButton;
	LPBUTTON								but;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonSelected(FALSE);

	UnlockButtonObject(oiButton);
	}

////////////////////////////

extern "C" void ALMAPI ButtonFunction_Set(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	OBJECTID								oiButton;
	LPBUTTON								but;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonSelected(TRUE);

	UnlockButtonObject(oiButton);
	}

////////////////////////////

extern "C" void ALMAPI ButtonFunction_Disable(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	OBJECTID								oiButton;
	LPBUTTON								but;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonEnabled(FALSE);

	UnlockButtonObject(oiButton);
	}

////////////////////////////

extern "C" void ALMAPI ButtonFunction_Enable(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	OBJECTID								oiButton;
	LPBUTTON								but;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonEnabled(TRUE);

	UnlockButtonObject(oiButton);
	}


////////////////////////////

extern "C" void ALMAPI ButtonFunction_GetTitle(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	OBJECTID								oiButton, oiText;
	LPBUTTON								but;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	oiText = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiText, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	OTextSetValueFromBuffer(oiText, (LPSTR)but->GetTitle());

	UnlockButtonObject(oiButton);

	AFuncSetParameter(1, oiText);
	}


////////////////////////////

extern "C" void ALMAPI ButtonFunction_SetTitle(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	OBJECTID								oiButton, oiText;
	LPBUTTON								but;
	char										szTitle[I_TITLE_LEN+1];

	I_UNUSED(theEvent);

	theSystem->message1 = 1; 	//Only one exit flow

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	oiText = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText, return);

	OTextGetToBuffer(oiText, szTitle, I_TITLE_LEN+1);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetTitle(szTitle);

	UnlockButtonObject(oiButton);
	}

////////////////////////////

extern "C" void ALMAPI ButtonFunction_IsSet(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	OBJECTID								oiButton;
	LPBUTTON								but;

	I_UNUSED(theEvent);

	theSystem->message1 = 2; // default to not set

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	if (but->IsButtonSelected())
		theSystem->message1 = 1; // set

	UnlockButtonObject(oiButton);
	}

////////////////////////////



