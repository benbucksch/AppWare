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
// FILE: dt_func.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Date object functions
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "stdlib.h"
#include "dt_incld.h"
#include "dt_dialg.h"

#define OTYPE_ANYTHING		0

#define SHORTDATE	        1
#define LONGDATE					0

/*****************************************************/
/* Change Date Function                              */
/*****************************************************/
extern "C" void ALMAPI DateFunction_Change(pAEvtInfo theEvent, pAObjMessage	theSystem)
{
	OBJECTID  oiDate, oiDays;
	LPDate		o_date;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiDate = AFuncGetTypedParameter(1, OTYPE_DATE);
	I_ASSERT_PARAMETER(oiDate, 1, "DateFunction_Change", return);

	o_date = LockDateObject(oiDate);
	I_ASSERT_DATE(o_date, "DateFunction_Change", return);

	oiDays = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiDays, 2, "DateFunction_Change", return);

  if (!o_date->IsKeepCurrent())   // if it is not keepcurrent, change the date
		o_date->SetDateValue(o_date->GetDateValue()+ONmbrGetInteger(oiDays),TRUE);

	UnlockDateObject(oiDate);
}

/*****************************************************/
/* Compare Date Function                             */
/*****************************************************/
extern "C" void ALMAPI DateFunction_Compare(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID  oiDate1, oiDate2;
	LPDate		date1, date2;

	I_UNUSED(theEvent);
	theSystem->message1 = 2;		//Default to equal

	oiDate1 = AFuncGetTypedParameter(1, OTYPE_DATE);
	I_ASSERT_PARAMETER(oiDate1, 1, "DateFunction_Compare", return);

	oiDate2 = AFuncGetTypedParameter(2, OTYPE_DATE);
	I_ASSERT_PARAMETER(oiDate2, 2, "DateFunction_Compare", return);

	date1 = LockDateObject(oiDate1);
	I_ASSERT_DATE(date1, "DateFunction_Compare", return);

	date2 = LockDateObject(oiDate2);
	I_ASSERT_DATE(date2, "DateFunction_Compare", return);

	if (date1->GetDateValue() < date2->GetDateValue())
		theSystem->message1 = 1;
	if (date1->GetDateValue() > date2->GetDateValue())
		theSystem->message1 = 3;

	UnlockDateObject(oiDate1);
	UnlockDateObject(oiDate2);
}

/*****************************************************/
/* Current Date Function                             */
/*****************************************************/
extern "C" void ALMAPI DateFunction_Current(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID  oiOut;
	DATE			currentDate;
	LPDate    dateOut;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiOut = AObjCreate(OTYPE_DATE);
	I_ASSERT_PSUEDO(oiOut, "DateFunction_Current", return);

	dateOut = LockDateObject(oiOut);

	getdate(&currentDate);
	dateOut->SetDateValue(currentDate, FALSE);

	UnlockDateObject(oiOut);
	AFuncSetParameter(1, oiOut);
}																												 

/*****************************************************/
/* Date Difference Function                          */
/*****************************************************/
extern "C" void ALMAPI DateFunction_Diff(pAEvtInfo theEvent, pAObjMessage theSystem)
{

	OBJECTID 	oiDate1, oiDate2, oiOut;
	LPDate  	date1, date2;
	long      diff;

  I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiDate1 = AFuncGetTypedParameter(1, OTYPE_DATE);
	I_ASSERT_PARAMETER(oiDate1, 1, "DateFunction_Diff", return);

	oiDate2 = AFuncGetTypedParameter(2, OTYPE_DATE);
	I_ASSERT_PARAMETER(oiDate2, 2, "DateFunction_Diff", return);

	oiOut = AObjCreate(OTYPE_NUMBER);
	I_ASSERT_PSUEDO(oiOut, "DateFunction_Diff", return);

	date1 = LockDateObject(oiDate1);
	I_ASSERT_DATE(date1, "DateFunction_Diff", return);

	date2 = LockDateObject(oiDate2);
	I_ASSERT_DATE(date2, "DateFunction_Diff", return);

	diff =  (date2->GetDateValue()- date1->GetDateValue());
	ONmbrSetInteger(oiOut, diff, TRUE); 

	UnlockDateObject(oiDate1);
	UnlockDateObject(oiDate2);

	AFuncSetParameter(1, oiOut);

}

/*****************************************************/
/* Date Font Size Function                           */
/*****************************************************/
extern "C" void ALMAPI DateFunction_SetFontSize(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{

	OBJECTID      					oiDate, oiFace, oiSize;
	LPDate			  				  date;
	char										szFace[LF_FACESIZE];
	LPATTRIBUTES						attr;
	long										height;
	long										changeFlag = 0L;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

	oiDate = AFuncGetTypedParameter(1, OTYPE_DATE);
	I_ASSERT_PARAMETER(oiDate, 1, "DateFunction_SetFontSize", return);
	date = LockDateObject(oiDate);
	I_ASSERT_DATE(date, "DateFunction_SetFontSize", return);

	attr = date->GetAttributes();

	oiFace = AFuncGetTypedParameter(2, OTYPE_TEXT);
	if (oiFace)
	{
		OTextGetToBuffer(oiFace, szFace, sizeof(szFace));
		changeFlag |= ATTR_CHANGE_FACE;
	}
	else
		szFace[0] = '\0';

	oiSize = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	if (oiSize)
	{
		height =  ONmbrGetInteger(oiSize);
		height *= 127L; //convert from points to TMM
		height /= 36L;
		changeFlag |= ATTR_CHANGE_HEIGHT;
  }
	else 
		height=0L;

	if (attr->AttrChangeFont(NULL, NULL, changeFlag, szFace, (int)height) )
	{
		OWndCallEachVisualInstance(
			oiDate,
			date->VisualInstanceIsHwnd() ? HwndDateItemChangeFont : LiteDateItemChangeFont,
			(LONG)attr,
			(LONG)changeFlag);
	}

	UnlockDateObject(oiDate);

}

void GetDateNames(unsigned long	julianDate, OBJECTID oiDayOfWeek, OBJECTID oiDayOfMonth, OBJECTID oiMonth, OBJECTID	oiYear, BOOL shortNames)
{
	char					dayOfWeek[15],  month[15], dayOfMonth[15], year[15];
	DATE					date;
	LPSTR					cp;
	BOOL 					leadingZero = FALSE;
  int						i;

	date = JulianToDate(julianDate);

	if (shortNames)
		lstrcpy(dayOfWeek, g_sDayNames[DayOfTheWeek(julianDate)]);
	else
		lstrcpy(dayOfWeek, g_lDayNames[DayOfTheWeek(julianDate)]);


	// Figure out if leading zeros belong in the dayOfMonth
	cp = g_lDateProfileString;
	while(*cp)
	{
		if ((*cp|0x20) == 'd')
   	{
			for(i=0; *cp == 'd'; cp++, i++);
			if (i==2) leadingZero = TRUE;
		}
    cp++;
	}
	if (leadingZero)
		sprintf(dayOfMonth, "%02d", date.da_day);
	else
		sprintf(dayOfMonth, "%d", date.da_day);

	if (shortNames)
		lstrcpy(month, g_sMonthNames[date.da_mon-1]);
	else
		lstrcpy(month, g_lMonthNames[date.da_mon-1]);

	if (shortNames) 
	{
		date.da_year = date.da_year % 100;
		sprintf(year, "%02d", date.da_year);
	}
	else
		sprintf(year, "%d", date.da_year);

	OTextSetValueFromBuffer(oiDayOfWeek, dayOfWeek);
	OTextSetValueFromBuffer(oiMonth, month);
	OTextSetValueFromBuffer(oiDayOfMonth, dayOfMonth);
	OTextSetValueFromBuffer(oiYear, year);

}
	
void GetDateNumbers(unsigned long	julianDate, OBJECTID	oiDayOfWeek, OBJECTID oiDayOfMonth, OBJECTID oiMonth, OBJECTID oiYear)
{
 	DATE					date;
 	
 	date = JulianToDate(julianDate);

	ONmbrSetInteger(oiDayOfWeek, (long) DayOfTheWeek(julianDate)+1, TRUE); 
	ONmbrSetInteger(oiDayOfMonth, (long) date.da_day, TRUE); 
	ONmbrSetInteger(oiMonth, (long) date.da_mon, TRUE); 
	ONmbrSetInteger(oiYear, (long) date.da_year, TRUE); 
}

extern "C" void ALMAPI DateFunction_Parse(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{
	OBJECTID         	oiDate, oiText; // inputs
	OBJECTID					oiDayOfWeek, oiDayOfMonth, oiMonth, oiYear; // outputs
	LPDate						datei;
	char							texti[2];
	int								type = 0; // 0=numbers, 1=long names, 2=short names

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiDate = AFuncGetTypedParameter(1, OTYPE_DATE);
	I_ASSERT_PARAMETER(oiDate, 1, "DateFunction_Parse", return);

	oiText = AFuncGetTypedParameter(2, OTYPE_TEXT);

	if (oiText)
	{
		I_ASSERT_PARAMETER(oiText, 2, "DateFunction_Parse", return);
		OTextGetToBuffer(oiText, texti, sizeof(texti));

		if ((*texti) == 'l' || (*texti) == 'L')		 //  match the first char 
			type = 1;	 // long date name
		else if ((*texti) == 's' || (*texti) == 'S')
			type = 2;	// short date name
		
	}

	datei = LockDateObject(oiDate);
	if (!type) // default number type 
	{
		oiDayOfWeek = AObjCreate(OTYPE_NUMBER);
		I_ASSERT_PSUEDO(oiDayOfWeek, "DateFunction_Parse", return);

		oiMonth = AObjCreate(OTYPE_NUMBER);
		I_ASSERT_PSUEDO(oiMonth, "DateFunction_Parse", return);


		oiDayOfMonth = AObjCreate(OTYPE_NUMBER); 
		I_ASSERT_PSUEDO(oiDayOfMonth, "DateFunction_Parse", return);

		oiYear = AObjCreate(OTYPE_NUMBER); 
		I_ASSERT_PSUEDO(oiYear, "DateFunction_Parse", return);

		GetDateNumbers(datei->GetDateValue(), oiDayOfWeek, oiDayOfMonth, oiMonth, oiYear);
	}
	else
	{
		oiDayOfWeek = AObjCreate(OTYPE_TEXT);
		I_ASSERT_PSUEDO(oiDayOfWeek, "DateFunction_Parse", return);

		oiMonth = AObjCreate(OTYPE_TEXT);
		I_ASSERT_PSUEDO(oiMonth, "DateFunction_Parse", return);

		oiDayOfMonth = AObjCreate(OTYPE_TEXT); 
		I_ASSERT_PSUEDO(oiDayOfMonth, "DateFunction_Parse", return);

		oiYear = AObjCreate(OTYPE_TEXT); 
		I_ASSERT_PSUEDO(oiYear, "DateFunction_Parse", return);

		if (type == 1) // long date
			GetDateNames(datei->GetDateValue(), oiDayOfWeek, oiDayOfMonth, oiMonth, oiYear, LONGDATE);
		else if (type == 2)
			GetDateNames(datei->GetDateValue(), oiDayOfWeek, oiDayOfMonth, oiMonth, oiYear, SHORTDATE);
  } 


	UnlockDateObject(oiDate);

	AFuncSetParameter(1, oiDayOfWeek);
	AFuncSetParameter(2, oiDayOfMonth);
	AFuncSetParameter(3, oiMonth);
	AFuncSetParameter(4, oiYear);

}








