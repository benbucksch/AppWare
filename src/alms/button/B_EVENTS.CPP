#include "b_includ.h"
#include "o_datb.h"

extern "C" void ALMAPI ButtonEvent_ObjectToText(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	LPBUTTON								but;

  I_UNUSED(theSystem);

	but = LockButtonObject(theEvent->targetObject);
	I_ASSERT_POINTER(but, return);

	but->ObjectToText((AMEMBLOCKID)theEvent->details);

	UnlockButtonObject(theEvent->targetObject);
	}

///////////////////////////

extern "C" void ALMAPI ButtonEvent_TextToObject(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	LPBUTTON								but;

	I_UNUSED(theSystem);

	but = LockButtonObject(theEvent->targetObject);
	I_ASSERT_POINTER(but, return);

	but->TextToObject((AMEMBLOCKID)theEvent->details);

	UnlockButtonObject(theEvent->targetObject);
	}

//////////////////////////////////////////////

extern "C" void ALMAPI ButtonEvent_SendKey(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	LPBUTTON								but;
	ODatbKey *						pKey = (ODatbKey *)theEvent->details;
  LONG										ln;

	I_UNUSED(theSystem);

	but = LockButtonObject(theEvent->targetObject);
	I_ASSERT_POINTER(but, return);

	switch (pKey->type)
		{
		case ODATB_KEYQUERY:
			if (but->DoesButtonSupportSelection() )
				pKey->type = ODATB_KEYLONG;
			else
				pKey->type =  ODATB_KEYTEXT;
			break;

		case ODATB_KEYTEXT:
			lstrcpyn(pKey->key, but->GetTitle(), max(ODATB_MXKEYSIZE, I_TITLE_LEN));
			pKey->key[max(ODATB_MXKEYSIZE, I_TITLE_LEN)] = '\0';
			break;

		case ODATB_KEYLONG:
			ln = but->IsButtonSelected();
			CopyMemory(pKey->key, &ln, sizeof(LONG));
			break;
		}

	UnlockButtonObject(theEvent->targetObject);
	return;
	}

////////////////////////////////////////////// 

extern "C" void ALMAPI ButtonEvent_TrackButton(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	LPBUTTON								but;

	I_UNUSED(theSystem);

	but = LockButtonObject(theEvent->targetObject);
	I_ASSERT_POINTER(but, return);

	but->ProcessTrackingEvent();

	UnlockButtonObject(theEvent->targetObject);
	return;
	}
