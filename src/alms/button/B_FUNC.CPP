
#include "b_includ.h"



//******************************************************************
//
// Func:	ButtonFunction_Clear
//
// Desc:	Removes selection mark from a specified 
//			radio or checkbox button.
//
//******************************************************************
extern "C" void ALMAPI 
ButtonFunction_Clear(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID	oiButton;
	LPBUTTON	but;


	I_UNUSED(theEvent);

	theSystem->message1 = 1;		// Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonSelected(FALSE);

	UnlockButtonObject(oiButton);
}



//******************************************************************
//
// Func:	ButtonFunction_Set
//
// Desc:	Sets a radio or checkbox button.
//
//******************************************************************
extern "C" void ALMAPI 
ButtonFunction_Set(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID	oiButton;
	LPBUTTON	but;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonSelected(TRUE);

	UnlockButtonObject(oiButton);
}


//******************************************************************
//
// Func:	ButtonFunction_IsSet
//
// Desc:	Check a specified radio or checkbox button for 
//			the selection mark and issue signal indicating
//			whether the mark is present.
//
//******************************************************************
extern "C" void ALMAPI 
ButtonFunction_IsSet(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID	oiButton;
	LPBUTTON	but;


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



//******************************************************************
//
// Func:	ButtonFunction_Disable
//
// Desc:	Disables or "grays-out" a specified button.
//			
//
//******************************************************************
extern "C" void ALMAPI 
ButtonFunction_Disable(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID	oiButton;
	LPBUTTON	but;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonEnabled(FALSE);

	UnlockButtonObject(oiButton);
}


//******************************************************************
//
// Func:	ButtonFunction_Enable
//
// Desc:	Enables a button.
//			
//
//******************************************************************
extern "C" void ALMAPI 
ButtonFunction_Enable(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID	oiButton;
	LPBUTTON	but;


	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one exit flow	

	oiButton = AFuncGetTypedParameter(1, OTYPE_BUTTON);
	I_VERIFY_PARAMETER(oiButton, return);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return);

	but->SetButtonEnabled(TRUE);

	UnlockButtonObject(oiButton);
}



//******************************************************************
//
// Func:	ButtonFunction_GetTitle
//
// Desc:	Reports the title of a specified button.
//			
//
//******************************************************************
extern "C" void ALMAPI 
ButtonFunction_GetTitle(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID	oiButton, oiText;
	LPBUTTON	but;


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



//******************************************************************
//
// Func:	ButtonFunction_SetTitle
//
// Desc:	Renames a button.
//			
//
//******************************************************************
extern "C" void ALMAPI 
ButtonFunction_SetTitle(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID	oiButton, oiText;
	LPBUTTON	but;
	char		szTitle[I_TITLE_LEN+1];


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






