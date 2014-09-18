/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE: dt_event
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Date object events
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "dt_incld.h"
#include "dt_class.h"
#include "o_date.h"


/******************************************************/
/* SetValue for all KeepCurrent dates and post 				*/
/* DateChanged event																	*/
/******************************************************/
extern "C" void CALLBACK DateEvent_Startup(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID						oiDate;
	LPDate					    date;
	DATE          			systemDate;
  LONG								julianSystemDate;
	AEvtInfo	     		  event;
	BOOL								keepCurrentExists = FALSE;
	long								cookie = AOBJ_GETFIRSTKEY;
	DOS_DATE				dosDate;


	I_UNUSED(theEvent); 
	I_UNUSED(theSystem);

	getdate(&dosDate);
	systemDate.da_year = dosDate.da_year;
	systemDate.da_mon = dosDate.da_mon;
	systemDate.da_day = dosDate.da_day;


	julianSystemDate = DateToJulian(systemDate);

	while ( (oiDate = AObjGetNextObject(OTYPE_DATE, &cookie)) != 0)
	{
		date = LockDateObject(oiDate);
		if (date->IsKeepCurrent())
		{
			date->SetDateValue(julianSystemDate, TRUE);
			keepCurrentExists = TRUE;
		}
		UnlockDateObject(oiDate);
	}

	if (keepCurrentExists)
  {
		event.sourceObject = 0;
    event.targetObject = 0;
    event.when         = JulianToTickCount(DateToJulian(systemDate)+1);
    event.how          = AEVT_ATTAIL | AEVT_KILLSIMILARS | AEVT_KILLALLQUEUES;
    event.what         = ODATE_EVT_DATEUPDATE;
//    event.what         = dateUpdate;
    event.details      = 0L;
		event.modifiers    = 0L;
    AEvtPost(&event);
	}

}

extern "C" void CALLBACK DateEvent_dateUpdate(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	
	BOOL          keepCurrentExists = FALSE;
  OBJECTID      oiDate;
  LPDate  			date;
  DATE          systemDate, objectDate;
  AEvtInfo   	  event;
	long					cookie = 	AOBJ_GETFIRSTKEY;
	DOS_DATE	dosDate;


	I_UNUSED(theEvent); 
	I_UNUSED(theSystem);

  getdate(&dosDate);
  systemDate.da_year = dosDate.da_year;
  systemDate.da_mon = dosDate.da_mon;
  systemDate.da_day = dosDate.da_day;


	while ((oiDate = AObjGetNextObject(OTYPE_DATE, &cookie)) != 0)
  {
   	date = LockDateObject(oiDate);
    if (date->IsKeepCurrent())
    {
      keepCurrentExists = TRUE;
      objectDate = JulianToDate(date->GetDateValue());
      if (memcmp(&objectDate, &systemDate, sizeof(DATE)))
      {
        //changedSignal is posted by the method that listens for ObjectChanged
          if (systemDate.da_year != objectDate.da_year)
						AEvtPostSignalAtTail(oiDate, DT_SignalYearChanged);

          if (systemDate.da_mon != objectDate.da_mon)
						AEvtPostSignalAtTail(oiDate, DT_SignalMonthChanged);

          if (systemDate.da_day != objectDate.da_day)
						AEvtPostSignalAtTail(oiDate, DT_SignalDayChanged);
			}

      //changedSignal is posted by the method that listens for ObjectChanged
					date->SetDateValue(DateToJulian(systemDate), TRUE); // Get the date value and post the changed signal
		}
		UnlockDateObject(oiDate);
  }

  if (keepCurrentExists)
  {
    event.sourceObject = 0;
    event.targetObject = 0;
    event.when         = JulianToTickCount(DateToJulian(systemDate)+1);
    event.how          = AEVT_ATTAIL | AEVT_KILLSIMILARS | AEVT_KILLALLQUEUES;
    event.what         = ODATE_EVT_DATEUPDATE;

//    event.what         = dateUpdate;
    event.details      = 0L;
    event.modifiers    = 0L;
    AEvtPost(&event);
  }
}


extern "C" void ALMAPI DateEvent_ObjectToText(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{
  LPDate		 		date;
	OBJECTID	 		oiDate;
	LPCSTR		 		szSource;
	LPSTR			 		szDest;
	MEMBLOCKID 		miDest;
  int				 		len;

	I_UNUSED(theSystem);
	I_UNUSED(theEvent);

	oiDate = (OBJECTID)theEvent->targetObject;
	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "DateEvent_ObjectToText", return);

  // make sure the string pointer is valid for temp. objects
  if(date->IsTempObject())
    date->FormatDate();

	szSource = date->GetDateStringPtr();
	UnlockDateObject(oiDate);

	len = lstrlen(szSource)+1;

	miDest = (MEMBLOCKID) theEvent->details;
	AMemSetSize(miDest, len);
	szDest = (LPSTR)AMemLock(miDest);
	lstrcpy(szDest, szSource);
  AMemUnlock(miDest);
}


extern "C" void ALMAPI DateEvent_TextToObject(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{
	LPDate				date;
	OBJECTID			oiDate;
	LPSTR 				szSource;
	MEMBLOCKID		miSource;

	I_UNUSED(theSystem);

	oiDate = (OBJECTID)theEvent->targetObject;
	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "DateEvent_TextToObject", return);

	miSource = (MEMBLOCKID) theEvent->details;
	szSource = (LPSTR)AMemLock(miSource);
	date->SetDateValue(szSource, POSTSIGNAL, BLANKOUT);

	UnlockDateObject(oiDate);
  AMemUnlock(miSource);
}
