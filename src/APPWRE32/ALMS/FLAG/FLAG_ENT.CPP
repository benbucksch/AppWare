////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//	"flag_ent.cpp"
//
// Flag Object function and event entry points
//
// FUNCTIONS
//	1.	Flag_ClearFlag
//	2.	Flag_IsFlagSet
//	3.	Flag_SetFlag
//	4.	Flag_ToggleFlag
//
// EVENT RESPONSES
//	1.	AEVENT_OBJECTTOTEXT	----	Flag_objectToText
//	2.	AEVENT_TEXTTOOBJECT	----	Flag_textToObject
//
////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<a_alm.h>
#include	"o_text.h"
#include	<op_wnd.h>
#include	<o_wnd.h>
//#include <string.h>
// #define OTYPE_TEXT											AMAKEID('T','e','x','t')

// #include	<wip.h>
#include	"flag_hdr.h"
#include	"o_flag.h"

#include	"flag.h"

// Flag_ClearFlag //////////////////////////////////////////////////////////
//	Input		1:	Flag
//	Flow		1:	After clearing

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flag_ClearFlag(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)

	{
	AOBJECTID	flagObj = AFuncGetTypedParameter(1L, OTYPE_FLAG);
	if(!flagObj)
  	{
		AFuncReportResult(theSystem, 1L, "The input to the Clear Flag function must be a Flag");
		return;
		}

	FLAG			flag(flagObj, TRUE);	// use the partial constructor
  flag.Clear();

	AFuncReportResult(theSystem, 1L, NULL);
	}

// Flag_IsFlagSet //////////////////////////////////////////////////////////
//	Input		1:	Flag
//	Output	1:	Text -- optional
//	Flow		1:	Flag Set
//	Flow		2:	Flag Clear

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flag_IsFlagSet(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)

	{
	AOBJECTID	flagObj = AFuncGetTypedParameter(1L, OTYPE_FLAG);
	if(!flagObj)
  	{
		AFuncReportResult(theSystem, 1L, "The input to the Is Flag Set function must be a Flag");
		return;
		}

	FLAG			flag(flagObj);

	AOBJECTID	textObj = AObjCreate(OTYPE_TEXT);
	OTextSetValueFromBuffer(textObj, flag.GetText());
//ecj	Text_TextToObject(textObj, flag.GetText());


	AFuncSetParameter(1L, textObj);

	if(flag.IsSet())
		AFuncReportResult(theSystem, 1L, NULL);
	else
		AFuncReportResult(theSystem, 2L, NULL);
	}

// Flag_SetFlag ////////////////////////////////////////////////////////////
//	Input		1:	Flag
//	Flow		1:	After setting

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flag_SetFlag(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)

	{
	AOBJECTID	flagObj = AFuncGetTypedParameter(1L, OTYPE_FLAG);
	if(!flagObj)
  	{
		AFuncReportResult(theSystem, 1L, "The input to the Set Flag function must be a Flag");
		return;
		}

	FLAG			flag(flagObj, TRUE);	// use the partial constructor
  flag.Set();

	AFuncReportResult(theSystem, 1L, NULL);
	}

// Flag_ToggleFlag /////////////////////////////////////////////////////////
//	Input		1:	Flag
//	Output	1:	Text -- optional
//	Flow		1:	After toggling

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flag_ToggleFlag(
	pAEvtInfo	/*theEvent*/,
	pAObjMessage				theSystem)

	{
	AOBJECTID	flagObj = AFuncGetTypedParameter(1L, OTYPE_FLAG);
	if(!flagObj)
  	{
		AFuncReportResult(theSystem, 1L, "The input to the Toggle Flag function must be a Flag");
		return;
		}

	FLAG			flag(flagObj);
  flag.Toggle();

	AOBJECTID	textObj = AObjCreate(OTYPE_TEXT);
	OTextSetValueFromBuffer(textObj, flag.GetText());
//	Text_TextToObject(textObj, flag.GetText());

	AFuncSetParameter(1L, textObj);

	AFuncReportResult(theSystem, 1L, NULL);
	}

// Flag_objectToText ///////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flag_objectToText(
	pAEvtInfo	theEvent,
	pAObjMessage				/*theSystem*/)

	{
	AOBJECTID		flagObj = theEvent->targetObject;
	AMEMBLOCKID	theBlock = (AMEMBLOCKID)theEvent->details;
	FLAG				flag(flagObj);

	AMemSetSize(theBlock, (long)lstrlen(flag.GetText()) + 1L);
	lstrcpy((char*)AMemLock(theBlock), flag.GetText()); 
	}

// Flag_textToObject ///////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Flag_textToObject(
	pAEvtInfo	theEvent,
	pAObjMessage				/*theSystem*/)

	{
	char	*FlagText[] = {{"TRUE"}, {"FALSE"}, {"YES"}, {"NO"}, {"SET"},
								{"CLEAR"}, {"T"}, {"F"}, {"Y"}, {"N"}, {"1"}, {"0"}};
	int 	numOfFlagText = sizeof(FlagText) / sizeof(FlagText[0]);
	AOBJECTID		flagObj = theEvent->targetObject;
	AMEMBLOCKID	theBlock = (AMEMBLOCKID)theEvent->details;
	char*				text = (char*)AMemLock(theBlock);
	FLAG				flag(flagObj);
	int					i;
	BOOL				foundIt=FALSE;

	for (i=0; i<numOfFlagText; i=i+2)			// is it a set strint?
		if (strcmpi(text,FlagText[i]) == 0) {
			flag.setSetText(FlagText[i]);
			flag.setClearText(FlagText[i+1]);
			foundIt = TRUE;
			break;
		}
	if (!foundIt)
		for (i=1; i<numOfFlagText; i=i+2)			// must or should be a clear string.
			if (strcmpi(text,FlagText[i]) == 0) {
				flag.setSetText(FlagText[i-1]);
				flag.setClearText(FlagText[i]);
				break;
			}

	if(!lstrcmpi(text, flag.SetText()))				// set the flag accordingly...
  	flag.Set();
	else
  	flag.Clear();
	}

////////////////////////////////////////////////////////////////////////////
