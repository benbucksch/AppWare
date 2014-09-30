#include "dt_incld.h"
#include "dt_dialg.h"

extern "C" ODATE ALMAPI	ODateStringToDate(LPSTR	szBuffer)
{
	BOOL					isLongDate = FALSE;
	int						month;

	for(month=0; month<12; month++)
		if ( SearchForSubstring(szBuffer, g_sMonthNames[month]))
    	isLongDate = TRUE;

	if (isLongDate)
		return ParseLongDate(szBuffer);

	return ParseShortDate(szBuffer);
}

extern "C" int  ALMAPI	ODateConvertToString(AOBJECTID	oiDate,	LPSTR	szBuffer,	short	bufferLen)
{
	LPDate		date;
  	int			rVal;

	I_ASSERT_TYPEID(oiDate, OTYPE_DATE, "ODateConvertToString", return 0);

	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "ODateConvertToString", return 0);

	date->FormatDate();
	lstrcpyn(szBuffer, date->GetDateStringPtr(), bufferLen);
	szBuffer[bufferLen-1] = '\0';
	rVal = lstrlen(szBuffer);

	UnlockDateObject(oiDate);

  return rVal;

}

extern "C" BOOL ALMAPI	ODateConvertFromString(AOBJECTID	oiDate,	LPSTR	szBuffer)
{
	LPDate 		date;
  BOOL	 		rVal;

	I_ASSERT_TYPEID(oiDate, OTYPE_DATE, "ODateConvertFromString", return 0);

	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "ODateConvertFromString", return 0);

	rVal = date->SetDateValue(szBuffer, POSTSIGNAL, BLANKOUT);

  UnlockDateObject(oiDate);

	return rVal;

}

extern "C" int ALMAPI	ODateFormatDate(AOBJECTID oiDate, LONG	theDate, LPSTR buffer, short bufferLen)
{
	LPDate		date;
	char      tmpStr[DT_STRING_LEN];
	int			rVal;

	I_ASSERT_TYPEID(oiDate, OTYPE_DATE, "ODateFormatDate", return 0);

	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "ODateFormatDate", return 0);

	if (date->IsShortDate())
		FormatShortDate(theDate, tmpStr, DT_STRING_LEN);
	else
		FormatLongDate(theDate, tmpStr, DT_STRING_LEN);

	lstrcpyn(buffer, tmpStr, bufferLen);
	buffer[bufferLen-1] = '\0';

	rVal = lstrlen(buffer);

	UnlockDateObject(oiDate);

  return rVal;

}

extern "C" ODATE ALMAPI	ODateGetValue(AOBJECTID oiDate)
{
	LPDate	date;
  long		rVal;

	I_ASSERT_TYPEID(oiDate, OTYPE_DATE, "ODateGetValue", return 0);

	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "ODateGetValue", return 0);

  rVal = date->GetDateValue();

	UnlockDateObject(oiDate);

	return rVal;

}

extern "C" BOOL ALMAPI	ODateSetValue(AOBJECTID 	oiDate,	long theDate)
{
	LPDate 		date;
	BOOL		 	rVal;

	I_ASSERT_TYPEID(oiDate, OTYPE_DATE, "ODateSetValue", return 0);

	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "ODateSetValue", return 0);

	rVal = date->SetDateValue( (long double)theDate, POSTSIGNAL);

	UnlockDateObject(oiDate);
  return rVal;

}

extern "C" void ALMAPI ODateToYMD(ODATE theDate,	pODATEYMD	ymdDate)
{
	DATE 		date;

	date = JulianToDate(theDate);

	ymdDate->year = date.da_year;
	ymdDate->month = date.da_mon;
	ymdDate->day = date.da_day;
	ymdDate->dayOfWeek = DayOfTheWeek(theDate);

}

extern "C" ODATE ALMAPI ODateFromYMD(pODATEYMD	ymdDate)
{
	DATE   date;

	date.da_year = ymdDate->year;
	date.da_mon = ymdDate->month;
	date.da_day = ymdDate->day;

	return DateToJulian(date);
	
}

/*returns ODATE_STYLE_<SHORT,LONG,KEEPCURRENT>*/
extern "C" long ALMAPI ODateGetStyle(AOBJECTID	oiDate)
{
	LPDate	date;
	long		rVal;

	I_ASSERT_TYPEID(oiDate, OTYPE_DATE, "ODateGetDateStyle", return 0);

	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "ODateGetDateStyle", return 0);

	rVal = date->GetDateStyle();

	UnlockDateObject(oiDate);

	return rVal;

}

/*returns ODATE_<MONTH,DAY,YEAR>_SUPLIED	*/
extern "C" long ALMAPI ODateParseString(LPSTR 	szString)
{
	long   whatsupplied;
															
	ParseDate(szString, &whatsupplied); 

	return whatsupplied;
	
}





