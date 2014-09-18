/******************************************************/
/*	 Time Object Event																*/
/******************************************************/
#include "tm_incld.h"
#include "tm_utl.h"
#include "o_time.h"


extern "C" void far pascal _export TimeEvent_Startup(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID						oiTime;
	LPTime					    time;
	TIME          			systemTime;
  LONG								systemTimeInSec;
	AEvtInfo	     		  event;
	BOOL								keepCurrentExists = FALSE;
	long								cookie = AOBJ_GETFIRSTKEY;


	I_UNUSED(theEvent); 
	I_UNUSED(theSystem);

	gettime(&systemTime);
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

extern "C" void far pascal _export TimeEvent_timeUpdate(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	
	BOOL          keepCurrentExists = FALSE;
  OBJECTID      oiTime;
  LPTime  			time;
  TIME          systemTime, objectTime;
  AEvtInfo   	  event;
	long					cookie = 	AOBJ_GETFIRSTKEY;

	I_UNUSED(theEvent); 
	I_UNUSED(theSystem);

  gettime(&systemTime);

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
	OBJECTID	 		oiTime;
	LPCSTR		 		szSource;
	LPSTR			 		szDest;
	MEMBLOCKID 		miDest;
  int				 		len;

	I_UNUSED(theSystem);
	I_UNUSED(theEvent);

	oiTime = (OBJECTID)theEvent->targetObject;
	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "TimeEvent_ObjectToText", return);

  // make sure the string pointer is valid for temp. objects
  if(time->IsTempObject())
    time->FormatTM();

	szSource = time->GetTimeStringPtr();
	UnlockTimeObject(oiTime);

	len = lstrlen(szSource)+1;

	miDest = (MEMBLOCKID) theEvent->details;
	AMemSetSize(miDest, len);
	szDest = (LPSTR)AMemLock(miDest);
	lstrcpy(szDest, szSource);
  AMemUnlock(miDest);
}


extern "C" void ALMAPI TimeEvent_TextToObject(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{
	LPTime				time;
	OBJECTID			oiTime;
	LPSTR 				szSource;
	MEMBLOCKID		miSource;

	I_UNUSED(theSystem);

	oiTime = (OBJECTID)theEvent->targetObject;
	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "TimeEvent_TextToObject", return);

	miSource = (MEMBLOCKID) theEvent->details;
	szSource = (LPSTR)AMemLock(miSource);
	time->SetTimeValue(szSource, POSTSIGNAL, BLANKOUT);

	UnlockTimeObject(oiTime);
  AMemUnlock(miSource);
}
