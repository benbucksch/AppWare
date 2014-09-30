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
// FILE: dt_class
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Date Class function
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include <windows.h>
 
#include "dt_incld.h"
#include "dt_dialg.h"


Date::Date(AOBJECTID	id,	BOOL bIsTemp):m_attributes(!bIsTemp) //If temp object, then don't do createStuff in attributes
{
	DATE 		systemDate;
	DOS_DATE	dosDate;

	m_version = VERSION;
	m_oiSelf = id;
	m_style = DT_DATEDEFAULT;
	SetTempObject(bIsTemp);

	if (!bIsTemp)
	{
		getdate(&dosDate);
		systemDate.da_year = dosDate.da_year;
		systemDate.da_mon = dosDate.da_mon;
		systemDate.da_day = dosDate.da_day;

		m_date = DateToJulian(systemDate);
		FormatDate();
	}

}

LPVOID Date::operator new(size_t size,	AOBJECTID id)
{
	AObjSetDataCount(id, NumData);
	AObjResizeData(id, ZeroBase, size);

	return AObjLockData(id, ZeroBase);
}

Date::Date(Date  *ptr):m_attributes(FALSE)   // Used to setup the virtual table
{
	I_UNUSED(ptr);
}
	
LPVOID Date::operator new(size_t	size,	Date  *ptr)
{
	I_UNUSED(size);
	return ptr;
}

void Date::operator delete(LPVOID	ptr)
{
	AObjUnlockData( ((LPDate)ptr)->m_oiSelf, ZeroBase);
}

/*****************************************************/
/* Create Lite hnd'd item                            */
/*****************************************************/
UINT Date::GetDrawDateStyle()
{
	UINT	drawFlags = DT_NOPREFIX | DT_SINGLELINE;

	drawFlags |= DT_LEFT;

	return drawFlags;
}

/*****************************************************/
/* Create Window hnd'd item                          */
/*****************************************************/
LONG Date::GetCreateWindowStyle()
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

void Date::CopyAttributes(LPATTRIBUTES	attTarget)
{
	CopyMemory(attTarget, &m_attributes, sizeof(ATTRIBUTES));
}

/*****************************************************/
/* Setup event object attributes                     */
/*****************************************************/
void Date::PostChangedSignals()
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
	event.details      = DT_SignalChanged;
	event.how          = (AEVT_KILLDUPLICATES | AEVT_ATTAIL | AEVT_KILLWHAT | AEVT_KILLSOURCE | AEVT_KILLDETAILS);

	AEvtPost(&event);		//Posts the changed signal

	//Clear the has changed bit
	SetHasChanged(FALSE);

}

/*****************************************************/
/* Get the value of object                           */
/*****************************************************/
LONG Date::GetDateValue()
{
	return m_date;
}

/*****************************************************/
/* Returns the date style (ODATE_STYLE_SHORT, LONG	 */
/* KEEPCURRENT)																			 */
/*****************************************************/
long Date::GetDateStyle()     
{
	long rVal=FALSE;

	if (IsKeepCurrent())
		rVal |= ODATE_STYLE_KEEPCURRENT;

	if (IsShortDate())
		rVal |= ODATE_STYLE_SHORT;
	else
		rVal |= ODATE_STYLE_LONG;

	  return rVal;
}


/*****************************************************/
/* Return if the date value has changed              */
/*****************************************************/
BOOL Date::LocalSetDateValue(long julian, BOOL	bPostSignals)//Returns TRUE if the value changed
{
	BOOL	rVal = FALSE;
//	const long maxJulian = (long)((30000*146097+1)/4) - 578041L;
	const long maxJulian = 11383592L;
	const long minJulian = -578041L;

	// check the julian date is in the range of 19th century
	if (julian <= maxJulian && julian >= minJulian)
	{
		if (m_date != julian)
		{
			m_date = julian;
			SetHasChanged(TRUE);
			FormatDate();
			rVal = TRUE;

			if (bPostSignals)
				PostChangedSignals();
		}
	}

	return rVal;
}

/*****************************************************/
/* Return if the date value has changed              */
/*****************************************************/
BOOL Date::SetDateValue(DATE dt, BOOL bPostSignals)
{
	BOOL rVal;
	long dateValue;


	dateValue = DateToJulian(dt);	// get the julian value for the date

	rVal = LocalSetDateValue(dateValue, bPostSignals);

	if ((!rVal || !bPostSignals) && DisplaysEmpty())	// No signals were posted, but it should not be blank anymore
	{
		SetDisplaysEmpty(FALSE);
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ? 
															 HwndDateItemForceRedraw:LiteDateItemForceRedraw,
															 0L, 0L);
	}
	else
		SetDisplaysEmpty(FALSE);

	return rVal;
}

BOOL Date::SetDateValue(long date, BOOL bPostSignals)
{
	BOOL rVal;	

	rVal = LocalSetDateValue(date, bPostSignals);

	if ((!rVal || !bPostSignals) && DisplaysEmpty())	// No signals were posted, but it should not be blank anymore
	{
		SetDisplaysEmpty(FALSE);
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ? 
															 HwndDateItemForceRedraw:LiteDateItemForceRedraw,
															 0L, 0L);
	}
	else
		SetDisplaysEmpty(FALSE);

	return rVal;
}

/*****************************************************/
/* Return if the date value(string) has changed      */
/*****************************************************/
BOOL Date::SetDateValue(LPSTR str, BOOL	bPostSignals, BOOL bBlankOutIfEmptyString)
{
	int  					month;
	BOOL 					rVal;
	BOOL 					islongFlag=FALSE;
	LONG 					dateValue;
	BOOL 					bNeedsToCallEachVI;
	BOOL					bShouldDisplayEmpty;

	if (IsTempObject())
	{
		for(month=0; month<12; month++)
			if ( SearchForSubstring(str, g_sMonthNames[month]))
    		islongFlag = TRUE;
	}
	else
	{
		if (!IsShortDate())
			islongFlag = TRUE;
	}


	if (islongFlag)
		dateValue = ParseLongDate(str);
	else
		dateValue = ParseShortDate(str);

	rVal = LocalSetDateValue(dateValue, bPostSignals);

	bShouldDisplayEmpty = ((str[0] == '\0') && bBlankOutIfEmptyString);

	if ((!rVal || !bPostSignals) && bBlankOutIfEmptyString && (bShouldDisplayEmpty != DisplaysEmpty()))
		bNeedsToCallEachVI = TRUE;
	else
		bNeedsToCallEachVI = FALSE;

	SetDisplaysEmpty(bShouldDisplayEmpty);

	if (bNeedsToCallEachVI)
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ?
															 HwndDateItemForceRedraw : LiteDateItemForceRedraw,
															 0L, 0L);

	return rVal;
}

/*****************************************************/
/* Format date                                       */
/*****************************************************/
void Date::FormatDate()
{

	if (IsShortDate())
		FormatShortDate(m_date, m_szDate, DT_STRING_LEN);
	else
		FormatLongDate(m_date,  m_szDate, DT_STRING_LEN);


}


/*****************************************************/
/* Validate Object ID                                */
/*****************************************************/
void Date::ValidateObjectIDs(AOBJECTID	oiDate)
{
	m_oiSelf = oiDate;
}

/*****************************************************/
/* Build object signals                              */
/*****************************************************/
void Date::BuildSignals(AMEMBLOCKID miSignals)
{
	int											signalNum = 0;
	pAObjSignalRecord				signal;

	AMemSetSize(miSignals, DT_NumSignals * sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(miSignals);

	signal[signalNum].theID = DT_SignalChanged;
	lstrcpy(signal[signalNum++].theName, "Changed");

	if (IsKeepCurrent()) // Build the following signals for the keepcurrent status
	{
		signal[signalNum].theID = DT_SignalDayChanged;
		lstrcpy(signal[signalNum++].theName, "Day Changed");

		signal[signalNum].theID = DT_SignalMonthChanged;
		lstrcpy(signal[signalNum++].theName, "Month Changed");

		signal[signalNum].theID = DT_SignalYearChanged;
		lstrcpy(signal[signalNum++].theName, "Year Changed");
	}

	if (IsActivatable())
	{
		signal[signalNum].theID = DT_SignalActivated;
		lstrcpy(signal[signalNum++].theName, "Activated");

		signal[signalNum].theID = DT_SignalDeactivated;
		lstrcpy(signal[signalNum++].theName, "Deactivated");
	}

	AMemUnlock(miSignals);
	AMemSetSize(miSignals, signalNum * sizeof(AObjSignalRecord));

}


/*****************************************************/
/* Find out if the object instance has changed       */
/*****************************************************/
BOOL Date::MaybePostChangedStuff(LONG	how) //defaults = AEVT_ATMARK|AEVT_KILLDUPLICATES
{
	BOOL	rVal=FALSE;

	if (HasChanged())
	{
		AEvtPostStandard(m_oiSelf, AEVENT_OBJECTCHANGED, 0, AEvtGetObjectChangedPriority(how) );
		AEvtPostSignalAtTail(m_oiSelf, DT_SignalChanged);
		SetHasChanged(FALSE);
		rVal = TRUE;
	}

	return rVal;
}




