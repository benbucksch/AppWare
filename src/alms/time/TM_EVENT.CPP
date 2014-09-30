/******************************************************/
/*	 Time Object Event																*/
/******************************************************/
#include "tm_incld.h"
#include "tm_utl.h"
#include "o_time.h"


extern "C" void CALLBACK TimeEvent_Startup(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	AOBJECTID						oiTime;
	LPTime					    time;
	TIME          			systemTime;
	TIME_DOS						dosTime;
  LONG								systemTimeInSec;
	AEvtInfo	     		  event;
	BOOL								keepCurrentExists = FALSE;
	long								cookie = AOBJ_GETFIRSTKEY;


	I_UNUSED(theEvent); 
	I_UNUSED(theSystem);

	gettime(&dosTime);
	systemTime.ti_min = dosTime.ti_min;
	systemTime.ti_hour = dosTime.ti_hour; 
	systemTime.ti_hund = dosTime.ti_hund;
	systemTime.ti_sec = dosTime.ti_sec;


	systemTimeInSec = TimeToSec(systemTime);

	while ( (oiTime = AObjGetNextObject(OTYPE_TIME, &cookie)) != 0)
	{
		time = LockTimeObject(oiTime);

		if (time->IsDefHour())			// if it is default
		{
			if (g_is24hourProfileBool)
				time->SetIs24Hour(TRUE);									 // set up 24 hour
		}

		if (time->IsKeepCurrent())
		{
			time->SetTimeValue(systemTimeInSec, TRUE);
			keepCurrentExists = TRUE;
		}
		UnlockTimeObject(oiTime);
	}

	if (keepCurrentExists)
  {
		event.sourceObject = 0;
    event.targetObject = 0;
    event.when         = AAppGetTime();
    event.how          = AEVT_ATTAIL | AEVT_KILLSIMILARS;
    event.what         = OTIME_EVT_TIMEUPDATE;
    event.details      = 0L;
		event.modifiers    = 0L;
    AEvtPost(&event);
	}

}

extern "C" void CALLBACK TimeEvent_timeUpdate(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	
	BOOL          keepCurrentExists = FALSE;
  AOBJECTID      oiTime;
  LPTime  			time;
  TIME          systemTime, objectTime;
  TIME_DOS		dosTime;
  AEvtInfo   	  event;
	long					cookie = 	AOBJ_GETFIRSTKEY;

	I_UNUSED(theEvent); 
	I_UNUSED(theSystem);

  gettime(&dosTime);
	systemTime.ti_min = dosTime.ti_min;
	systemTime.ti_hour = dosTime.ti_hour; 
	systemTime.ti_hund = dosTime.ti_hund;
	systemTime.ti_sec = dosTime.ti_sec;

	while ((oiTime = AObjGetNextObject(OTYPE_TIME, &cookie)) != 0)
  {
   	time = LockTimeObject(oiTime);
    if (time->IsKeepCurrent())
    {
      keepCurrentExists = TRUE;
      objectTime = SecToTime(time->GetTimeValue());
      if (memcmp(&objectTime, &systemTime, sizeof(TIME)))
      {
				if (systemTime.ti_hour != objectTime.ti_hour)	
					AEvtPostSignalAtTail(oiTime, TM_SignalHourChanged);

				if (systemTime.ti_min != objectTime.ti_min)		
					AEvtPostSignalAtTail(oiTime, TM_SignalMinuteChanged);

				if (systemTime.ti_sec != objectTime.ti_sec)		
					AEvtPostSignalAtTail(oiTime, TM_SignalSecondChanged);
		  }
        //changedSignal is posted by the method that listens for ObjectChanged
					time->SetTimeValue(TimeToSec(systemTime), TRUE); // Get the time value and post the changed signal
    }
		UnlockTimeObject(oiTime);
  }

  if (keepCurrentExists)
  {
    event.sourceObject = 0;
    event.targetObject = 0;
    event.when         = AAppGetTime() + 60;
    event.how          = AEVT_ATTAIL;
    event.what         = OTIME_EVT_TIMEUPDATE;
    event.details      = 0L;
    event.modifiers    = 0L;
    AEvtPost(&event);
  }
}


extern "C" void ALMAPI TimeEvent_ObjectToText(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{
  LPTime		 		time;
	AOBJECTID	 		oiTime;
	LPCSTR		 		szSource;
	LPSTR			 		szDest;
	AMEMBLOCKID 		miDest;
  int				 		len;

	I_UNUSED(theSystem);
	I_UNUSED(theEvent);

	oiTime = (AOBJECTID)theEvent->targetObject;
	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "TimeEvent_ObjectToText", return);

  // make sure the string pointer is valid for temp. objects
  if(time->IsTempObject())
    time->FormatTM();

	szSource = time->GetTimeStringPtr();
	UnlockTimeObject(oiTime);

	len = lstrlen(szSource)+1;

	miDest = (AMEMBLOCKID) theEvent->details;
	AMemSetSize(miDest, len);
	szDest = (LPSTR)AMemLock(miDest);
	lstrcpy(szDest, szSource);
  AMemUnlock(miDest);
}


extern "C" void ALMAPI TimeEvent_TextToObject(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{
	LPTime				time;
	AOBJECTID			oiTime;
	LPSTR 				szSource;
	AMEMBLOCKID		miSource;

	I_UNUSED(theSystem);

	oiTime = (AOBJECTID)theEvent->targetObject;
	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "TimeEvent_TextToObject", return);

	miSource = (AMEMBLOCKID) theEvent->details;
	szSource = (LPSTR)AMemLock(miSource);
	time->SetTimeValue(szSource, POSTSIGNAL, BLANKOUT);

	UnlockTimeObject(oiTime);
  AMemUnlock(miSource);
}
