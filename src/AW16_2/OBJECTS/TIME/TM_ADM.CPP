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
//
//--------------------------------------------------------------
// FILE: dt_adm.cpp	
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	 Time object Administration Routine
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "tm_incld.h"
#include "tm_adm.h"

extern LONG EditTimeObject(OBJECTID oi);
extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status, long special);

/*****************************************************/
/* Creates and initializes the data structire for a  */
/* Time object when the AOBJ_CREATED meassage is     */
/* received                                          */
/*****************************************************/

extern "C" LONG ALMCALLBACK TimeAdminProc(OBJECTID oiTime,	AObjMessage* theSystem)
{

	switch(theSystem->message1)
	{
		case AOBJ_CREATED:
			return CreateTimeObject(oiTime, theSystem);
		case AOBJ_ASSIGNOBJECT:
			return AssignTimeObject(oiTime, theSystem);
		case AOBJ_EDITOBJECT:
			return EditTimeObject(oiTime);
		case AOBJ_VALIDATEOBJECTIDS:
			return ValidateTimeObject(oiTime);
		case AOBJ_READ:
			return ReadObject(oiTime);
		case AOBJ_CHECKOBJECT:
			return CheckObject();
		case AOBJ_BUILDSIGNALS:
			return BuildTimeSignal(oiTime, theSystem);
		case AOBJ_QUERYPROTOCOL:
			return QueryProtocol(theSystem);
  	case AOBJ_IMPORTOBJECT:
			return ImportTimeObject(oiTime);
		case AOBJ_EXPORTOBJECT:
			return ExportTimeObject(oiTime);

	}

	return A_NOTHANDLED;
	
}

LONG CreateTimeObject(OBJECTID oiTime, AObjMessage* theSystem)			
{
	LPTime	time;
	BOOL  	bTempObject;

	bTempObject = (theSystem->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE;
	time = new(oiTime) Time(oiTime, bTempObject);  //The locks the data
	delete time;  //Just unlocks the data -- DOES NOT DELETE THE DATAS!!!

	return  A_OK;
}

LONG AssignTimeObject(OBJECTID oiTime, AObjMessage* theSystem)
{
	OBJECTID				oiSource, oiDest;
  LPTime					timeSrc, timeDst;
	BOOL            bWasChanged;


	oiDest = oiTime;
  oiSource = (OBJECTID)theSystem->message3;

	timeSrc = LockTimeObject(oiSource);
	timeDst = LockTimeObject(oiDest);

	bWasChanged = timeDst->SetTimeValue(timeSrc->GetTimeValue(), POSTSIGNAL);
	if (timeSrc->DisplaysEmpty() != timeDst->DisplaysEmpty())
	{
		timeDst->SetDisplaysEmpty(timeSrc->DisplaysEmpty());

		if (!bWasChanged)
			OWndCallEachVisualInstance(oiDest, timeDst->VisualInstanceIsHwnd()?
																 HwndTimeItemForceRedraw : LiteTimeItemForceRedraw,
																 0L, 0L);
	}

	UnlockTimeObject(oiSource);
	UnlockTimeObject(oiDest);

	return A_OK;

}

LONG ValidateTimeObject(OBJECTID oiTime)
{
	LPTime time;

	time = LockTimeObject(oiTime);
	time->ValidateObjectIDs(oiTime);
	UnlockTimeObject(oiTime);

	return A_OK;
	
}

LONG ReadObject(OBJECTID oiTime)
{
	LPTime time;

	time = LockTimeObject(oiTime);
	new(time) Time(time);
	UnlockTimeObject(oiTime);
	
	return A_OK;
}

LONG CheckObject()
{
	return A_OK;
	
}

LONG BuildTimeSignal(OBJECTID oiTime, AObjMessage* theSystem)
{
	LPTime time;

	time = LockTimeObject(oiTime);
	time->BuildSignals((MEMBLOCKID)theSystem->message2);
	UnlockTimeObject(oiTime);
	
	return A_OK;
}

LONG QueryProtocol(AObjMessage* theSystem)
{
 	pApQueryInfo		pQueryInfo;
	LONG						rVal = A_NOTHANDLED;

	pQueryInfo = (ApQueryInfo*) theSystem->message4;
	if (pQueryInfo->protocol == APROT_WND)
	{
		rVal = AP_SUPPORTED;
		pQueryInfo->pfnCreate = (APPCFN) CreateVisualInstance;
	}
  
  return rVal;
}

/////////////////////////////////////////////////////////
///  UPSF for the Time Object   ///////////
/////////////////////////////////////////////////////////
LONG	ExportTimeObject(OBJECTID	oiTime)
{
	LPTime	time;
	int     style;
	BOOL		editable, frame, current, selectall, displaySec, displayEmpty;
  TIME		theTime;

	time = LockTimeObject(oiTime);

	editable = time->IsEditable();
	frame = time->HasFrame();
	current = time->IsKeepCurrent();
	selectall = time->SelectsAll();
	displaySec = time->HasSeconds();
	displayEmpty  = time->DisplaysEmpty();

	if (time->IsDefHour())
		style = 0;
	else if (time->Is24Hour())
		style = 2;
	else
		style = 1;			   // default to 12-hour time 

	theTime = SecToTime(time->GetTimeValue());

	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, time->GetTimeVer(), sizeof(int));
	AUpsfExportField("EDITABLE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &editable, sizeof(BOOL));
	AUpsfExportField("FRAME", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &frame, sizeof(BOOL));
	AUpsfExportField("CURRENT", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &current, sizeof(BOOL));
	AUpsfExportField("SELECTALL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &selectall, sizeof(BOOL));
	AUpsfExportField("STYLE", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &style, sizeof(int));
	AUpsfExportField("DSPLYSEC", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &displaySec, sizeof(BOOL));
	AUpsfExportField("DSPEMPTY", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &displayEmpty, sizeof(BOOL));
	AUpsfExportField("HOURS", 0L, AUPSF_NATV_UCHAR, AUPSF_UNIV_INTEGER, &theTime.ti_hour, sizeof(char));
	AUpsfExportField("MINUTES", 0L, AUPSF_NATV_UCHAR, AUPSF_UNIV_INTEGER, &theTime.ti_min, sizeof(char));
	AUpsfExportField("SECONDS", 0L, AUPSF_NATV_UCHAR, AUPSF_UNIV_INTEGER, &theTime.ti_sec, sizeof(char));

	(time->GetAttributes())->Export();

	UnlockTimeObject(oiTime);

	return A_OK;
	
}

////////////////////////////
LONG ImportTimeObject(OBJECTID oiTime)
{
	LPTime 	time; 
	TIME		theTime;
	BOOL		editable, frame, current, selectall, displaySec, displayEmpty;
	int     style;

	gettime(&theTime);
	time = LockTimeObject(oiTime);

	if(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, time->GetTimeVer(), sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing m_version -- using default.");

	if(AUpsfImportField("EDITABLE", 0L, AUPSF_NATV_BOOL, &editable, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"EDITABLE\" -- using default.");
	else
		time->SetIsEditable(editable);

	if(AUpsfImportField("FRAME", 0L, AUPSF_NATV_BOOL, &frame, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"FRAME\" -- using default.");
	else
		time->SetHasFrame(frame);

	if(AUpsfImportField("CURRENT", 0L, AUPSF_NATV_BOOL, &current, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"CURRENT\" -- using default.");
	else
		time->SetIsKeepCurrent(current);

	if(AUpsfImportField("DSPLYSEC", 0L, AUPSF_NATV_BOOL, &displaySec, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"DSPLSEC\" -- using default.");
	else
		time->SetSecond(displaySec);

	if(AUpsfImportField("DSEMPTY", 0L, AUPSF_NATV_BOOL, &displayEmpty, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"DSEMPTY\" -- using default.");
	else
		time->SetDisplaysEmpty(displayEmpty);

	if(AUpsfImportField("SELECTALL", 0L, AUPSF_NATV_BOOL, &selectall, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"SELECTALL\" -- using default.");
	else
		time->SetSelectsAll(selectall);

	if(AUpsfImportField("STYLE", 0L, AUPSF_NATV_INT, &style, sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"STYLE\" -- using default.");
	else
	{
		if (style == 2)
			time->SetIs24Hour(TRUE);
		else if (style == 0)
			time->SetIsDefHour(TRUE);
		else if (style == 1)
			time->SetIs12Hour(FALSE);
	}

	if(AUpsfImportField("HOURS", 0L, AUPSF_NATV_INT, &theTime.ti_hour, sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"HOURS\" -- using current hour.");

	if(AUpsfImportField("MINUTES", 0L, AUPSF_NATV_CHAR, &theTime.ti_min, sizeof(char)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"MINUTES\" -- using current minute.");

	if(AUpsfImportField("SECONDS", 0L, AUPSF_NATV_CHAR, &theTime.ti_sec, sizeof(char)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"SECONDS\" -- using current second.");

	(time->GetAttributes())->Import();
 	time->FormatTM();

	UnlockTimeObject(oiTime);

	return A_OK;
}


/*****************************************************/
/* Create Visual Instance                            */
/*****************************************************/
extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(OBJECTID	oiItem,	OBJECTID oiParent, RECT* rcItem, long	status,	long special)
{
	LPTime				time;
  OpWndItemD*		wi;

	I_UNUSED(special);

	time = LockTimeObject(oiItem);

	if (time->HasFrame() || time->IsEditable() || time->IsActivatable())
		wi = new HwndTimeItem(oiItem, oiParent, rcItem, status);
	else
		wi = new LiteTimeItem(oiItem, oiParent, rcItem, status);

	UnlockTimeObject(oiItem);

	return wi;
}




































































