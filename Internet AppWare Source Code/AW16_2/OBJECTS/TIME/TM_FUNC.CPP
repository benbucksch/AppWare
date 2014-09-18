///////////////////////////////////////////////////////////////
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
// FILE: tm_func.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Time object functions
//
// CHANGES:
//
/////////////////////////////////////////////////////////////*/
#include "stdlib.h"
#include "tm_incld.h"
#include "tm_dialg.h"
#include "tm_utl.h"

extern "C" void ALMAPI TimeFunction_Change(pAEvtInfo theEvent, pAObjMessage	theSystem)
{
	OBJECTID  oiTime, oiNum;
	LPTime		o_time;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiTime = AFuncGetTypedParameter(1, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime, 1, "TimeFunction_Change", return);

	oiNum = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum, 2, "TimeFunction_Change", return);

	o_time = LockTimeObject(oiTime);
	I_ASSERT_TIME(o_time, "TimeFunction_Change", return);

  if (!o_time->IsKeepCurrent())   // if it is not keepcurrent, change the time
		o_time->SetTimeValue(o_time->GetTimeValue()+ONmbrGetInteger(oiNum),TRUE);

	UnlockTimeObject(oiTime);
}

/*****************************************************/
/* Compare Time Function Down to hour and minute     */
/*****************************************************/
extern "C" void ALMAPI TimeFunction_CompareHM(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID  oiTime1, oiTime2;
	LPTime		time1, time2;
	LONG      sec1, sec2;

	I_UNUSED(theEvent);
	theSystem->message1 = 2;		//Default to equal

	oiTime1 = AFuncGetTypedParameter(1, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime1, 1, "TimeFunction_Compare", return);

	oiTime2 = AFuncGetTypedParameter(2, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime2, 2, "TimeFunction_Compare", return);

	time1 = LockTimeObject(oiTime1);
	I_ASSERT_TIME(time1, "TimeFunction_Compare", return);

	time2 = LockTimeObject(oiTime2);
	I_ASSERT_TIME(time2, "TimeFunction_Compare", return);

	sec1 = time1->GetTimeValue();
	sec2 = time2->GetTimeValue();

	sec1 -= (sec1%60); 
	sec2 -= (sec2%60);  //Don't count seconds

	if (sec1 < sec2)	
		theSystem->message1 = 1;
	if (sec1 > sec2)	
		theSystem->message1 = 3;

	UnlockTimeObject(oiTime1);
	UnlockTimeObject(oiTime2);
}

/*****************************************************/
/* Compare Time Function Down to hour, min and sec   */
/*****************************************************/
extern "C" void ALMAPI TimeFunction_CompareHMS(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID  oiTime1, oiTime2;
	LPTime		time1, time2;

	I_UNUSED(theEvent);
	theSystem->message1 = 2;		//Default to equal

	oiTime1 = AFuncGetTypedParameter(1, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime1, 1, "TimeFunction_Compare", return);

	oiTime2 = AFuncGetTypedParameter(2, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime2, 2, "TimeFunction_Compare", return);

	time1 = LockTimeObject(oiTime1);
	I_ASSERT_TIME(time1, "TimeFunction_Compare", return);

	time2 = LockTimeObject(oiTime2);
	I_ASSERT_TIME(time2, "TimeFunction_Compare", return);

	if (time1->GetTimeValue() < time2->GetTimeValue())
		theSystem->message1 = 1;
	if (time1->GetTimeValue() > time2->GetTimeValue())
		theSystem->message1 = 3;

	UnlockTimeObject(oiTime1);
	UnlockTimeObject(oiTime2);
}

/*****************************************************/
/* Current Time Function                             */
/*****************************************************/
extern "C" void ALMAPI TimeFunction_Current(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID  oiOut;
	TIME			currentTime;
	LPTime    timeOut;
	long      time;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiOut = AObjCreate(OTYPE_TIME);
	I_ASSERT_PSUEDO(oiOut, "TimeFunction_Current", return);

	timeOut = LockTimeObject(oiOut);

//	gettime(&currentTime);
//	time = TimeToSec(currentTime);
//	timeOut->SetTimeValue(time, FALSE);
	timeOut->FormatTM();

	UnlockTimeObject(oiOut);

	AFuncSetParameter(1, oiOut);

}																												 

/*****************************************************/
/* Time Difference Function                          */
/*****************************************************/
extern "C" void ALMAPI TimeFunction_Diff(pAEvtInfo theEvent, pAObjMessage theSystem)
{

	OBJECTID 	oiTime1, oiTime2, oiOut;
	LPTime  	time1, time2;
	long      diff;

  I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiTime1 = AFuncGetTypedParameter(1, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime1, 1, "TimeFunction_Diff", return);

	oiTime2 = AFuncGetTypedParameter(2, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime2, 2, "TimeFunction_Diff", return);

	oiOut = AObjCreate(OTYPE_NUMBER);
	I_ASSERT_PSUEDO(oiOut, "TimeFunction_Diff", return);

	time1 = LockTimeObject(oiTime1);
	I_ASSERT_TIME(time1, "TimeFunction_Diff", return);

	time2 = LockTimeObject(oiTime2);
	I_ASSERT_TIME(time2, "TimeFunction_Diff", return);

	diff =  (time2->GetTimeValue()- time1->GetTimeValue());
	ONmbrSetInteger(oiOut, diff, TRUE); 

	UnlockTimeObject(oiTime1);
	UnlockTimeObject(oiTime2);

	AFuncSetParameter(1, oiOut);

}

/*****************************************************/
/* Time Font Size Function                           */
/*****************************************************/
extern "C" void ALMAPI TimeFunction_SetFontSize(pAEvtInfo	theEvent,	pAObjMessage theSystem)
{

	OBJECTID      					oiTime, oiFace, oiSize;
	LPTime			  				  time;
	char										szFace[LF_FACESIZE];
	LPATTRIBUTES						attr;
	long										height;
	long										changeFlag = 0L;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

	oiTime = AFuncGetTypedParameter(1, OTYPE_TIME);
	I_ASSERT_PARAMETER(oiTime, 1, "TimeFunction_SetFontSize", return);
	time = LockTimeObject(oiTime);
	I_ASSERT_TIME(time, "TimeFunction_SetFontSize", return);

	attr = time->GetAttributes();

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
			oiTime,
			time->VisualInstanceIsHwnd() ? HwndTimeItemChangeFont : LiteTimeItemChangeFont,
			(LONG)attr,
			(LONG)changeFlag);
	}

	UnlockTimeObject(oiTime);

}

