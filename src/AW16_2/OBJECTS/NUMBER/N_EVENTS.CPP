
#include "n_includ.h"


/////////////////////////////////////

extern "C" void ALMAPI NmbrEvent_ObjectToText(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
  LPNUMBER								num;
	OBJECTID								oiNumber;
	LPCSTR										szSource;
	LPSTR										szDest;
	MEMBLOCKID							miDest;
  int											len;

	I_UNUSED(theSystem);

	oiNumber = (OBJECTID)theEvent->targetObject;
	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "NmbrEvent_ObjectToText", return);

	szSource = num->GetStringPtr();
	UnlockNumberObject(oiNumber);

	len = lstrlen(szSource)+1;

	miDest = (MEMBLOCKID) theEvent->details;
	AMemSetSize(miDest, len);
	szDest = (LPSTR)AMemLock(miDest);
	lstrcpy(szDest, szSource);
  AMemUnlock(miDest);
	}

///////////////////////////

extern "C" void ALMAPI NmbrEvent_TextToObject(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	LPNUMBER								num;
	OBJECTID								oiNumber;
	LPCSTR									szSource;
	MEMBLOCKID							miSource;

	I_UNUSED(theSystem);

	oiNumber = (OBJECTID)theEvent->targetObject;
	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "NmbrEvent_TextToObject", return);

	miSource = (MEMBLOCKID) theEvent->details;
	szSource = (LPCSTR)AMemLock(miSource);
	num->SetValue(szSource, POST_SIGNALS, CHANGE_IF_PSEUDO, BLANK_IF_EMPTY_STRING);

	UnlockNumberObject(oiNumber);
  AMemUnlock(miSource);
	}

///////////////////////////////////////////////////
