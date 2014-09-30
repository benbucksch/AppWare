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
// FILE: tm_class
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Time Class function
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include <windows.h>
 
#include "tm_incld.h"
#include "tm_dialg.h"
#include "malloc.h"

Time::Time(AOBJECTID	id,	BOOL bIsTemp):m_attributes(!bIsTemp) //If temp object, then don't do createStuff in attributes
{
	TIME_DOS 	systemTime;
	TIME		time;

	m_version = VERSION;
	m_oiSelf = id;
	m_style = TM_TIMEDEFAULT;
	SetTempObject(bIsTemp);

	gettime(&systemTime);
	time.ti_min = systemTime.ti_min;
	time.ti_hour = systemTime.ti_hour; 
	time.ti_hund = systemTime.ti_hund;
	time.ti_sec = systemTime.ti_sec;
	
	m_time = TimeToSec(time);
}

LPVOID Time::operator new(size_t size,	AOBJECTID id)
{
	AObjSetDataCount(id, NumData);
	AObjResizeData(id, ZeroBase, size);

	return AObjLockData(id, ZeroBase);
}

Time::Time(Time  *ptr): m_attributes(FALSE)   // Used to setup the virtual table
{
	I_UNUSED(ptr);
}
	
LPVOID Time::operator new(size_t	size,	Time  *ptr)
{
	I_UNUSED(size);
	return ptr;
}

void Time::operator delete(LPVOID	ptr)
{
	AObjUnlockData( ((LPTime)ptr)->m_oiSelf, ZeroBase);
}

/*****************************************************/
/* Create Lite hnd'd item                            */
/*****************************************************/
UINT Time::GetDrawTimeStyle()
{
	UINT	drawFlags = DT_NOPREFIX | DT_SINGLELINE;

	drawFlags |= DT_LEFT;

	return drawFlags;
}

/*****************************************************/
/* Create Window hnd'd item                          */
/*****************************************************/
LONG Time::GetCreateWindowStyle()
{
	LONG style = WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL;

	if (HasFrame())
		style |= WS_BORDER;

	if (!IsEditable())
		style |= ES_READONLY;

	if (IsKeepCurrent())
		style |= ES_READONLY;

	return style;
}

void Time::CopyAttributes(LPATTRIBUTES	attTarget)
{
	CopyMemory(attTarget, &m_attributes, sizeof(ATTRIBUTES));
}

/*****************************************************/
/* Setup event object attributes                     */
/*****************************************************/
void Time::PostChangedSignals()
{
	AEvtInfo event;
 
	event.sourceObject = event.targetObject = m_oiSelf;
	event.when         = 0;
	event.modifiers    = 0L;
	event.variation		 = 0;

	event.what         = AEVENT_OBJECTCHANGED;
	event.details      = 0;
	event.how          = AEvtGetObjectChangedPriority(AEVT_KILLALLQUEUES | AEVT_KILLWHAT | AEVT_KILLSOURCE);
	AEvtPost(&event);		//Post ObjectChanged...

	event.what         = AEVENT_SIGNAL;
	event.details      = TM_SignalChanged;
	event.how          = (AEVT_KILLALLQUEUES | AEVT_KILLWHAT | AEVT_KILLSOURCE | AEVT_KILLDETAILS | AEVT_ATTAIL);
	AEvtPost(&event);		//Posts the changed signal

	//Clear the has changed bit
	SetHasChanged(FALSE);

}

/*****************************************************/
/* Get the value of object                           */
/*****************************************************/
LONG Time::GetTimeValue()
{
	return m_time;
}

/*****************************************************/
/* Returns the date style (ODATE_STYLE_SECONDS     	 */
/* KEEPCURRENT)																			 */
/*****************************************************/
long Time::GetTimeStyle()     
{
	long rVal=FALSE;

	if (HasSeconds())
		rVal |= OTIME_STYLE_SEC;

	if (IsKeepCurrent())
		rVal |= OTIME_STYLE_KEEPCURRENT;

  return rVal;
}

/*****************************************************/
/* Return if the time value has changed              */
/*****************************************************/
BOOL Time::LocalSetTimeValue(long time, BOOL	bPostSignals)//Returns TRUE if the value changed
{
	BOOL	rVal = FALSE;

	if (m_time != time)
	{
		m_time = time;
		SetHasChanged(TRUE);
  	FormatTime(m_time, m_style, m_szTime, TM_STRING_LEN);

		rVal = TRUE;

		if (bPostSignals)
			PostChangedSignals();
	}

	return rVal;
}


/*****************************************************/
/* Return if the date value has changed              */
/*****************************************************/
BOOL Time::SetTimeValue(LPSTR tm,  BOOL	bPostSignals, BOOL bBlankOutIfEmptyString)
{
	long  time;
	BOOL	rVal;
	BOOL 	bNeedsToCallEachVI;
	BOOL	bShouldDisplayEmpty;

	time = ParseTime(tm);
	rVal = LocalSetTimeValue(time, bPostSignals);

	bShouldDisplayEmpty = ((tm[0] == '\0') && bBlankOutIfEmptyString);

	if ((!rVal || !bPostSignals) && bBlankOutIfEmptyString && (bShouldDisplayEmpty != DisplaysEmpty()))
		bNeedsToCallEachVI = TRUE;
	else
		bNeedsToCallEachVI = FALSE;

	SetDisplaysEmpty(bShouldDisplayEmpty);

	if (bNeedsToCallEachVI)
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ?
															 HwndTimeItemForceRedraw : LiteTimeItemForceRedraw,
															 0L, 0L);


	return rVal;
}

/*****************************************************/
/* Return if the date value has changed              */
/*****************************************************/
BOOL Time::SetTimeValue(long time,  BOOL	bPostSignals)
{
	BOOL	rVal;
	TIME  tm;
  BOOL  doSec =((m_style & TM_SECONDS)!=0);
	long  t;
	char  str[TM_STRING_LEN];
	LPSTR cp = str;
	LPSTR tmpStr = cp;


	tm = SecToTime(time);
	if(g_leadingZeroProfileBool)	
		wsprintf(cp, "%02d", tm.ti_hour);
	else												
		wsprintf(cp, "%d", tm.ti_hour);

	cp += lstrlen(cp);
	lstrcat(cp, g_timeSepProfileChar);
	cp += lstrlen(cp);

	wsprintf(cp, "%02d", tm.ti_min);
	cp += lstrlen(cp);

	if (doSec)
	{
		lstrcat(cp, g_timeSepProfileChar);
		cp += lstrlen(cp);
		wsprintf(cp, "%02d", tm.ti_sec);
//		cp += lstrlen(cp);
	}

	t = ParseTime(tmpStr);
	rVal = LocalSetTimeValue(t, bPostSignals);
//	tm = ParseTime(m_szTime);
// 	FormatTime(tm, m_style, m_szTime, TM_STRING_LEN);

	if ((!rVal || !bPostSignals) && DisplaysEmpty())	// No signals were posted, but it should not be blank anymore
	{
		SetDisplaysEmpty(FALSE);
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ? 
															 HwndTimeItemForceRedraw:LiteTimeItemForceRedraw,
															 0L, 0L);

	}
	else
		SetDisplaysEmpty(FALSE);

	return rVal;

}


/*****************************************************/
/* Validate Object ID                                */
/*****************************************************/
void Time::ValidateObjectIDs(AOBJECTID	oiTime)
{
	m_oiSelf = oiTime;
}

/*****************************************************/
/* Build object signals                              */
/*****************************************************/
void Time::BuildSignals(AMEMBLOCKID miSignals)
{
	int											signalNum = 0;
	pAObjSignalRecord				signal;

	AMemSetSize(miSignals, TM_NumSignals * sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(miSignals);

	signal[signalNum].theID = TM_SignalChanged;
	lstrcpy(signal[signalNum++].theName, "Changed");

	if (IsKeepCurrent()) // Build the following signals for the keepcurrent status
	{
		signal[signalNum].theID = TM_SignalSecondChanged;
		lstrcpy(signal[signalNum++].theName, "Second Changed");

		signal[signalNum].theID = TM_SignalMinuteChanged;
		lstrcpy(signal[signalNum++].theName, "Minute Changed");

		signal[signalNum].theID = TM_SignalHourChanged;
		lstrcpy(signal[signalNum++].theName, "Hour Changed");
	}

	if (IsActivatable())
	{
		signal[signalNum].theID = TM_SignalActivated;
		lstrcpy(signal[signalNum++].theName, "Activated");

		signal[signalNum].theID = TM_SignalDeactivated;
		lstrcpy(signal[signalNum++].theName, "Deactivated");
	}

	AMemUnlock(miSignals);
	AMemSetSize(miSignals, signalNum * sizeof(AObjSignalRecord));

}

/*****************************************************/
/* Find out if the object instance has changed       */
/*****************************************************/
BOOL Time::MaybePostChangedStuff(LONG	how) //defaults = AEVT_ATMARK|AEVT_KILLDUPLICATES
{
	BOOL	rVal=FALSE;

	if (HasChanged())
	{
		AEvtPostStandard(m_oiSelf, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how) );
		AEvtPostSignalAtTail(m_oiSelf, TM_SignalChanged);
		SetHasChanged(FALSE);
		rVal = TRUE;
	}

	return rVal;
}

void Time::FormatTM()
{

	FormatTime(m_time, m_style, m_szTime, TM_STRING_LEN);

}



