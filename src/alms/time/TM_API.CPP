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
// FILE: tm_api.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Time object APIs Function
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "tm_incld.h"

extern "C" OTIME ALMAPI OTimeStringToTime(LPSTR szBuffer)
{
	return ParseTime(szBuffer);
}

extern "C" OTIME ALMAPI	OTimeGetValue(AOBJECTID oiTime)
{
	LPTime	time;
  long		rVal;

	I_ASSERT_TYPEID(oiTime, OTYPE_TIME, "OTimeGetValue", return 0);

	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "OTimeGetValue", return 0);

  rVal = time->GetTimeValue();

	UnlockTimeObject(oiTime);

	return rVal;

}

extern "C" int ALMAPI	OTimeConvertToString(AOBJECTID	oiTime,	LPSTR	szBuffer,	short	bufferLen)
{
	LPTime		time;
  	int			rVal;

	I_ASSERT_TYPEID(oiTime, OTYPE_TIME, "OTimeConvertToString", return 0);

	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "OTimeConvertToString", return 0);

	time->FormatTM();

	lstrcpyn(szBuffer, time->GetTimeStringPtr(), bufferLen);
	szBuffer[bufferLen-1] = '\0';
	rVal = lstrlen(szBuffer);

	UnlockTimeObject(oiTime);

  return rVal;

}

extern "C" BOOL ALMAPI	OTimeConvertFromString(AOBJECTID	oiTime,	LPSTR	szBuffer)
{
	LPTime 		time;
  BOOL	 		rVal;

	I_ASSERT_TYPEID(oiTime, OTYPE_TIME, "OTimeConvertFromString", return 0);

	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "OTimeConvertFromString", return 0);

	rVal = time->SetTimeValue(szBuffer, POSTSIGNAL, BLANKOUT);

  UnlockTimeObject(oiTime);

	return rVal;

}

extern "C" int ALMAPI	OTimeFormatTime(AOBJECTID oiTime, LONG	theTime, LPSTR buffer, short bufferLen)
{
	LPTime		time;
	char      tmpStr[TM_STRING_LEN];
  	int			rVal;

	I_ASSERT_TYPEID(oiTime, OTYPE_TIME, "OTimeFormatTime", return 0);

	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "OTimeFormatTime", return 0);

	FormatTime(theTime, time->GetTimeStyle(), tmpStr, TM_STRING_LEN);

	lstrcpyn(buffer, tmpStr, bufferLen);
	buffer[bufferLen-1] = '\0';

	rVal = lstrlen(buffer);

	UnlockTimeObject(oiTime);

  return rVal;

}


extern "C" BOOL ALMAPI	OTimeSetValue(AOBJECTID 	oiTime,	long theTime)
{
	LPTime 		time;
	BOOL		 	rVal;

	I_ASSERT_TYPEID(oiTime, OTYPE_TIME, "OTimeSetValue", return 0);

	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "OTimeSetValue", return 0);

	rVal = time->SetTimeValue( (long double)theTime, POSTSIGNAL);

	UnlockTimeObject(oiTime);
  return rVal;

}

#pragma argsused
extern "C" void ALMAPI OTimeToHMS(OTIME theTime,	pOTIMEHMS	hmsTime)
{
	TIME	time;


	time = SecToTime(theTime);

	hmsTime->min = time.ti_min;
	hmsTime->hour = time.ti_hour;
	hmsTime->sec = time.ti_sec;

}

extern "C" OTIME ALMAPI OTimeFromHMS(pOTIMEHMS	hmsTime)
{
	TIME   time;

	time.ti_min = hmsTime->min;
	time.ti_hour = hmsTime->hour;
	time.ti_sec = hmsTime->sec;

	return TimeToSec(time);
	
}
	
extern "C" long ALMAPI OTimeGetStyle(AOBJECTID	oiTime)
{
	LPTime	time;
	long		rVal;

	I_ASSERT_TYPEID(oiTime, OTYPE_TIME, "OTimeGetTimeStyle", return 0);

	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "OTimeGetTimeStyle", return 0);

	rVal = time->GetTimeStyle();

	UnlockTimeObject(oiTime);

	return rVal;

}

/*returns OTIME_<HOUR,MIN,SEC>_SUPLIED	*/
extern "C" long ALMAPI OTimeParseString(	LPSTR	szString)
{

	long   whatsupplied;
															
	ParseTimeString(szString, &whatsupplied); 

	return whatsupplied;

}



