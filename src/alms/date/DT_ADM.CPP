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
// DESCRIPTION:	 Date object Administration Routine
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include "dt_incld.h"
#include "dt_adm.h"

extern LONG EditDateObject(AOBJECTID oi);
extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status, long special);

/*****************************************************/
/* Creates and initializes the data structure for a  */
/* Date object when the AOBJ_CREATED meassage is     */
/* received                                          */
/*****************************************************/

extern "C" LONG ALMCALLBACK DateAdminProc(AOBJECTID oiDate,	AObjMessage* theSystem)
{

	switch(theSystem->message1)
	{
		case AOBJ_CREATED:
			return CreateDateObject(oiDate, theSystem);
		case AOBJ_ASSIGNOBJECT:
			return AssignDateObject(oiDate, theSystem);
		case AOBJ_EDITOBJECT:
			return EditDateObject(oiDate);
		case AOBJ_VALIDATEOBJECTIDS:
			return ValidateDateObject(oiDate);
		case AOBJ_READ:
			return ReadObject(oiDate);
		case AOBJ_CHECKOBJECT:
			return CheckObject();
		case AOBJ_BUILDSIGNALS:
			return BuildDateSignal(oiDate, theSystem);
		case AOBJ_QUERYPROTOCOL:
			return QueryProtocol(theSystem);
  	case AOBJ_IMPORTOBJECT:
			return ImportDateObject(oiDate);
		case AOBJ_EXPORTOBJECT:
			return ExportDateObject(oiDate);
			
	}

	return A_NOTHANDLED;
	
}


LONG CreateDateObject(AOBJECTID oiDate, AObjMessage* theSystem)			
{
	LPDate	date;
	BOOL  	bTempObject;

	bTempObject = (theSystem->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE;
	date = new(oiDate) Date(oiDate, bTempObject);  //The locks the data
	delete date;  //Just unlocks the data -- DOES NOT DELETE THE DATAS!!!

	return  A_OK;
}

LONG AssignDateObject(AOBJECTID oiDate, AObjMessage* theSystem)
{
	AOBJECTID				oiSource, oiDest;
  LPDate					dateSrc, dateDst;
	BOOL            bWasChanged;

	oiDest = oiDate;
  oiSource = (AOBJECTID)theSystem->message3;

	dateSrc = LockDateObject(oiSource);
	dateDst = LockDateObject(oiDest);

	bWasChanged = dateDst->SetDateValue(dateSrc->GetDateValue(), POSTSIGNAL);
	if (dateSrc->DisplaysEmpty() != dateDst->DisplaysEmpty())
	{
		dateDst->SetDisplaysEmpty(dateSrc->DisplaysEmpty());

		if (!bWasChanged)
			OWndCallEachVisualInstance(oiDest, dateDst->VisualInstanceIsHwnd()?
																 HwndDateItemForceRedraw : LiteDateItemForceRedraw,
																 0L, 0L);
	}

	UnlockDateObject(oiSource);
	UnlockDateObject(oiDest);

	return A_OK;

}

LONG ValidateDateObject(AOBJECTID oiDate)
{
	LPDate date;

	date = LockDateObject(oiDate);
	date->ValidateObjectIDs(oiDate);
	UnlockDateObject(oiDate);

	return A_OK;
	
}

LONG ReadObject(AOBJECTID oiDate)
{
	LPDate date;

	date = LockDateObject(oiDate);
	new(date) Date(date);
	UnlockDateObject(oiDate);
	
	return A_OK;
}

LONG CheckObject()
{
	return A_OK;
	
}

LONG BuildDateSignal(AOBJECTID oiDate, AObjMessage* theSystem)
{
	LPDate date;

	date = LockDateObject(oiDate);
	date->BuildSignals((AMEMBLOCKID)theSystem->message2);
	UnlockDateObject(oiDate);
	
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
///  UPSF for the Date Object   ///////////
/////////////////////////////////////////////////////////
LONG	ExportDateObject(AOBJECTID	oiDate)
{
	LPDate	date;
	BOOL		editable, frame, current, longdate, shortdate, selectall, displayEmpty, active;
  DATE		theDate;

	date = LockDateObject(oiDate);

	editable = date->IsEditable();
	frame = date->HasFrame();
	current = date->IsKeepCurrent();
	shortdate = date->IsShortDate();
	selectall = date->SelectsAll();
	displayEmpty  = date->DisplaysEmpty();
	active = date->IsActivatable()	;

	theDate = JulianToDate(date->GetDateValue());
	if (shortdate)
			longdate = 0;
	else
			longdate = 1;
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, date->GetDateVer(), sizeof(int));
	AUpsfExportField("EDITABLE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &editable, sizeof(BOOL));
	AUpsfExportField("FRAME", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &frame, sizeof(BOOL));
	AUpsfExportField("CURRENT", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &current, sizeof(BOOL));
	AUpsfExportField("LONGDATE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &longdate, sizeof(BOOL));
	AUpsfExportField("SELECTALL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &selectall, sizeof(BOOL));
	AUpsfExportField("ACTIVATABLE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &active, sizeof(BOOL));
	AUpsfExportField("YEAR", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &theDate.da_year, sizeof(int));
	AUpsfExportField("MONTH", 0L, AUPSF_NATV_CHAR, AUPSF_UNIV_INTEGER, &theDate.da_mon, sizeof(char));
	AUpsfExportField("DAY", 0L, AUPSF_NATV_CHAR, AUPSF_UNIV_INTEGER, &theDate.da_day, sizeof(char));
	AUpsfExportField("DSEMPTY", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &displayEmpty, sizeof(BOOL));

	(date->GetAttributes())->Export();

	UnlockDateObject(oiDate);

	return A_OK;
	
}

////////////////////////////
LONG ImportDateObject(AOBJECTID oiDate)
{
	LPDate 	date; 
	DATE		theDate;
	BOOL		editable, frame, current, longdate, selectall, displayEmpty, active;
	DOS_DATE	dosDate;

	getdate(&dosDate);
	theDate.da_year = dosDate.da_year;
	theDate.da_mon = dosDate.da_mon;
	theDate.da_day = dosDate.da_day;

	date = LockDateObject(oiDate);

	if(AUpsfImportField("EDITABLE", 0L, AUPSF_NATV_BOOL, &editable, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"EDITABLE\" -- using default.");
	else
		date->SetIsEditable(editable);

	if(AUpsfImportField("FRAME", 0L, AUPSF_NATV_BOOL, &frame, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"FRAME\" -- using default.");
	else
		date->SetHasFrame(frame);

	if(AUpsfImportField("CURRENT", 0L, AUPSF_NATV_BOOL, &current, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"CURRENT\" -- using default.");
	else
		date->SetIsKeepCurrent(current);

	if(AUpsfImportField("LONGDATE", 0L, AUPSF_NATV_BOOL, &longdate, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"LONGDATE\" -- using default.");
	else
	{
		if (longdate)
	    date->SetIsShortDate(FALSE);
		else
	    date->SetIsShortDate(TRUE);
	}

	if(AUpsfImportField("DSEMPTY", 0L, AUPSF_NATV_BOOL, &displayEmpty, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"DSEMPTY\" -- using default.");
	else
		date->SetDisplaysEmpty(displayEmpty);

	if(AUpsfImportField("SELECTALL", 0L, AUPSF_NATV_BOOL, &selectall, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"SELECTALL\" -- using default.");
	else
		date->SetSelectsAll(selectall);

	if(AUpsfImportField("ACTIVATABLE", 0L, AUPSF_NATV_BOOL, &active, sizeof(BOOL)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"ACTIVATABLE\" -- using default.");
	else
		date->SetIsActivatable(active);

	if(AUpsfImportField("YEAR", 0L, AUPSF_NATV_INT, &theDate.da_year, sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"YEAR\" -- using current year.");

	if(AUpsfImportField("MONTH", 0L, AUPSF_NATV_CHAR, &theDate.da_mon, sizeof(char)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"MONTH\" -- using current month.");

	if(AUpsfImportField("DAY", 0L, AUPSF_NATV_CHAR, &theDate.da_day, sizeof(char)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"DAY\" -- using current day.");

	(date->GetAttributes())->Import();
	date->FormatDate();

	UnlockDateObject(oiDate);

	return A_OK;
}


/*****************************************************/
/* Create Visual Instance                            */
/*****************************************************/
extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(AOBJECTID	oiItem,	AOBJECTID oiParent, RECT* rcItem, long	status,	long special)
{
	LPDate				date;
  OpWndItemD*		wi;

	I_UNUSED(special);

	date = LockDateObject(oiItem);

//	if (date->VisualInstanceIsHwnd())
	if (date->HasFrame() || date->IsEditable() || date->IsActivatable())
		wi = new HwndDateItem(oiItem, oiParent, rcItem, status);
	else
		wi = new LiteDateItem(oiItem, oiParent, rcItem, status);

	UnlockDateObject(oiItem);

	return wi;
}




































































