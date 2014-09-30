#include "b_include.h"

///////////////////////////

BOOL ALMAPI	OButnIsSelected(
	OBJECTID 								oiButton)
	{
	LPBUTTON								but;
	BOOL										rVal;

	I_VERIFY_TYPEID(oiButton, OTYPE_BUTTON, return FALSE);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return FALSE);

	rVal = but->IsButtonSelected();

	UnlockButtonObject(oiButton);

	return rVal;
	}

///////////////////////////

BOOL ALMAPI OButnSetSelected(
	OBJECTID 								oiButton, 
	BOOL 										bSelected)
	{
	LPBUTTON								but;
	BOOL										rVal;

	I_VERIFY_TYPEID(oiButton, OTYPE_BUTTON, return FALSE);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return FALSE);

	rVal = but->SetButtonSelected(bSelected);

	UnlockButtonObject(oiButton);

	return rVal;
	}

///////////////////////////

OBUTN_TYPE ALMAPI	OButnGetType(
	OBJECTID 								oiButton)
	{
	LPBUTTON								but;
	OBUTN_TYPE							rVal;

	I_VERIFY_TYPEID(oiButton, OTYPE_BUTTON, return OBUTN_UNKNOWN);

	but = LockButtonObject(oiButton);
	I_VERIFY_POINTER(but, return OBUTN_UNKNOWN);

	rVal = but->GetButtonType();

	UnlockButtonObject(oiButton);

	return rVal;
	}


