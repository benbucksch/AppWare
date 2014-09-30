///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// function.cpp
// Text Builder
// Text Builder Functions
//
// Robert Lundahl/ Edgar C. Jerez
//
// Start Date 07/01/92
// End   Date 07/03/92
///////////////////////////////////////////////////////////////

#include "tf_incld.h"


void Parse ( AOBJECTID objid, BYTE* string );
long GetLength ( AOBJECTID tfObject );
void TranslateChar ( TOKEN* tkp, LPSTR output );
void TranslateLiteral ( TOKEN* tkp, LPSTR output, AOBJECTID objid );
WORD TranslateObject ( TOKEN* tkp, LPSTR output, AOBJECTID objid );

extern GLOBAL vG;       // Global Container Class

extern "C"
{
	void CALLBACK AutoUpdateOff ( pAEvtInfo evt, pAObjMessage sys );
	void CALLBACK AutoUpdateOn ( pAEvtInfo evt, pAObjMessage sys );
	void CALLBACK BuildText ( pAEvtInfo evt, pAObjMessage sys );
	void CALLBACK SetExpress ( pAEvtInfo evt, pAObjMessage sys );
}


///////////////////////////////////////////////////////////////
// AutoUpdateOff: Param 1 Text Builder Object.
//
///////////////////////////////////////////////////////////////

void CALLBACK AutoUpdateOff ( pAEvtInfo, pAObjMessage sys )
{
	AOBJECTID theObject = AFuncGetTypedParameter(1, OTYPE_TEXTMAKER);
  if (theObject == NULL)
  {
	  AFuncReportResult(sys, 1, "Bad Object Handle: Text Builder Build Text");
		return;
	}

	DATAFOUR*	data4 = (DATAFOUR*)AObjLockData(theObject, 4L);
	if(data4)
	{
		data4->autoUpdate = FALSE;
    AFuncReportResult(sys, 1, NULL);
	}
	else
  {
		AFuncReportResult(sys, 1L, "Unable to set auto update due to a memory error.");
	}
	AObjUnlockData(theObject, 4L);

}				// AutoUpdateOff...


///////////////////////////////////////////////////////////////
// AutoUpdateOn: Param 1 Text Builder Object
//
///////////////////////////////////////////////////////////////

void CALLBACK AutoUpdateOn ( pAEvtInfo, pAObjMessage sys )
{
	AOBJECTID theObject = AFuncGetTypedParameter(1, OTYPE_TEXTMAKER);
  if (theObject == NULL)
  {
	  AFuncReportResult(sys, 1, "Bad Object Handle: Text Builder Build Text");
		return;
  }

	DATAFOUR*	data4 = (DATAFOUR*)AObjLockData(theObject, 4L);
	if(data4)
	{
		AOBJECTID* oid = (AOBJECTID*)AObjLockData(theObject, 0L);
    BOOL			changeOK = TRUE;
		while(*oid)
		{
			if(*oid==data4->resultId)
			{
				changeOK = FALSE;
				AFuncReportResult(sys, 1L, "Text Builder:  Auto Update On\r\rUnable to start auto updating because the result object is in the variable list.");
				break;
			}
		oid++;
		}

		AObjUnlockData(theObject, 0L);

		if(changeOK)
    {
			data4->autoUpdate = TRUE;
			AFuncReportResult(sys, 1, NULL);
    }
	AObjUnlockData(theObject, 4L);
	}
	else
  {
		AFuncReportResult(sys, 1L, "Unable to set auto update due to a memory error.");
	}

}				// AutoUpdateOn(...


///////////////////////////////////////////////////////////////
// BuildText: Param 1 Text Builder Object.
//
///////////////////////////////////////////////////////////////

void CALLBACK BuildText ( pAEvtInfo, pAObjMessage sys )
{
  BOOL      	ret;
  long				length;
  AOBJECTID  	tfObject;
	TOKEN*  		data2;
	DATAFOUR*   data4;
	AMEMBLOCKID  memblk;
  char*				memblkPtr;

	tfObject = AFuncGetTypedParameter(1, OTYPE_TEXTMAKER);
  if (tfObject == NULL)
  {
	  AFuncReportResult(sys, 1, "Bad Object Handle: Text Builder Build Text");
		return;
  }

	length = GetLength(tfObject);
	if (length > 0x7FFF)
	{
		AFuncReportResult(sys, 1, "Text Build Length Exceeds Maximum Limit");
		return;
  }

	memblk = AMemAllocate(length+100);
  if (memblk == 0L)
		return;

	memblkPtr = (LPSTR)AMemLock(memblk);
    memblkPtr[0] = 0;

	data2 = (TOKEN*)AObjLockData(tfObject, 2L);
  if (data2 == NULL)
  {
    AFuncReportResult(sys, 1, "Data 2 Undefined: Text Builder Build Text");
		AMemFree(memblk);
		return;
  }

  while (data2->type != STOP)
  {
    switch (data2->type)
    {
    case CHARACTER:
      TranslateChar(data2, memblkPtr);
      break;

    case LITERAL:
      TranslateLiteral(data2, memblkPtr, tfObject);
      break;

		case OBJECT:
      ret = TranslateObject(data2, memblkPtr, tfObject);
      if (!ret)
			{
				AFuncReportResult(sys, 1, "OIP Call Method Failed: Text Builder");
       	AEvtPostSignalAtMark(tfObject, SIGNAL_ERROR);
				AMemFree(memblk);
				AObjUnlockData(tfObject, 2L);
				return;
      }
			break;
    }
    data2++;
	}

	AObjUnlockData(tfObject, 2L);

	data4 = (DATAFOUR*)AObjLockData(tfObject, 4L);
  if (data4 == NULL)
  {
    AFuncReportResult(sys, 1, "Text Builder:\nData 4 Undefined In Build Text");
		AMemFree(memblk);
		return;
	}

	if (data4->resultId == 0L)
	{
		AMemFree(memblk);
		AFuncReportResult(sys, 1L, NULL);
		AEvtPostSignalAtMark(tfObject, SIGNAL_EXPUPDATE);
    AObjUnlockData(tfObject, 4L);
		return;
	}

	ret = AObjConvertFromText(data4->resultId, memblk);
	if (!ret)
	{
    AMemFree(memblk);
    AFuncReportResult(sys, 1, "OIP Call Method Failed: Text Builder");
		AObjUnlockData(tfObject, 4L);
		return;
  }

	AMemFree(memblk);
	AFuncReportResult(sys, 1L, NULL);
	AEvtPostSignalAtMark(tfObject, SIGNAL_EXPUPDATE);
	AObjUnlockData(tfObject, 4L);
}				// BuildText(...


///////////////////////////////////////////////////////////////
// GetLength:
//
///////////////////////////////////////////////////////////////

long GetLength ( AOBJECTID tfObject )
{
	BOOL				ret;
  long				length = 0;
	TOKEN* 			data2;
	AMEMBLOCKID  memblk;

	data2 = (TOKEN*)AObjLockData(tfObject, 2L);

	while (data2->type != STOP)
  {
		switch (data2->type)
		{
    case CHARACTER:
      length += data2->length;
      break;

    case LITERAL:
			length += data2->length;
      break;

		case OBJECT:
			// allocate at zero length first then let AEVENT_OBJECTTOTEXT resize
			memblk = AMemAllocate(0L);
			if (memblk == 0L)
      {
				AObjUnlockData(tfObject, 2L);
				return 0;
      }

			ret = AEvtCallMethod(tfObject, data2->multi, 0L, AEVENT_OBJECTTOTEXT, memblk);
	    if (!ret)
      {
				AMemFree(memblk);
				break;
      }

			length += (UINT)lstrlen( (LPSTR)AMemLock(memblk) );
			AMemFree(memblk);
			break;
    }
  	data2++;
	}
	AObjUnlockData(tfObject, 2L);
  return length;
}				// GetLength(...


///////////////////////////////////////////////////////////////
// TranslateChar:
//
///////////////////////////////////////////////////////////////

void TranslateChar ( TOKEN* tkp, LPSTR output )
{
  long    s[10];

  s[0] = tkp->multi;
  s[1] = 0L;

	lstrcat(output, (LPCSTR)s);
}				// TranslateChar...


///////////////////////////////////////////////////////////////
// TranslateLiteral:
//
///////////////////////////////////////////////////////////////

void TranslateLiteral ( TOKEN* tkp, LPSTR output, AOBJECTID objid )
{
	STRING* strPtr;

  strPtr = new STRING (objid, 3L);
  if (strPtr == NULL)
    return;

	lstrcat( output, (LPCSTR)strPtr->Get(UINT(tkp->multi)) );
  delete strPtr;
}				// TranslateLiteral...


///////////////////////////////////////////////////////////////
// TranslateObject:
//
///////////////////////////////////////////////////////////////

WORD TranslateObject ( TOKEN* tkp, LPSTR output, AOBJECTID objid )
{
  BOOL        ret;
  AMEMBLOCKID  memblk;

	// allocate at zero length first then let AEVENT_OBJECTTOTEXT resize
	memblk = AMemAllocate(0L);
  if (memblk == 0L)
		return 0;

	ret = AEvtCallMethod(objid, tkp->multi, 0L, AEVENT_OBJECTTOTEXT, memblk);
  if (!ret)
		return 0;

	lstrcat(output, (LPSTR)AMemLock(memblk));
	AMemFree(memblk);

  return 1;
}				// TranslateObject...


///////////////////////////////////////////////////////////////
// SetExpress:
//
// Param 1 Text Object
// Param 2 Text Builder Object
//
///////////////////////////////////////////////////////////////

void CALLBACK SetExpress ( pAEvtInfo, pAObjMessage sys )
{
	AOBJECTID  textObject;
	AOBJECTID	tfObject;
	LPSTR     textObjectData1;
  TEXTFACT*	tf;

	tfObject = AFuncGetTypedParameter(1, OTYPE_TEXTMAKER);
  if (tfObject == NULL)
  {
    AFuncReportResult(sys, 1, "Bad Object Handle: Text Builder Set Instructions");
    return;
  }

  textObject = AFuncGetTypedParameter(2, OTYPE_TEXT);
  if (textObject == NULL)
  {
    AFuncReportResult(sys, 1, "Bad Object Handle: Text Builder Set Instructions");
    return;
  }

	textObjectData1 = (LPSTR)AObjLockData(textObject, 1L);
	if (textObjectData1 == NULL)
	{
		AFuncReportResult(sys, 1, "Data 1 Undefined: Text Builder Set Instructions");
    return;
  }

	tf = (TEXTFACT*)AObjGetRuntimeInfo(tfObject);
	tf->Parse((char*)textObjectData1, sys);
	AObjUnlockData(textObject, 1L);

	if(vG.parseError)
  {
		AEvtPostSignalAtMark(tfObject, SIGNAL_ERROR);
		return;
	}

	AEvtPostStandard(tfObject, AEVENT_OBJECTCHANGED, 0L, AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
  AFuncReportResult(sys, 1L, NULL);
}				// SetExpress...




