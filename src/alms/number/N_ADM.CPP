#include "n_includ.h"

extern LONG EditNumberObject(AOBJECTID oi);
extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status, long special);

//////////////////////////////////////////////////
//////////////////////////////////////////////////

extern "C"  LONG ALMCALLBACK NumAdminProc(
	AOBJECTID 								oiNumber,
	AObjMessage*					  theSystem)
  {
	LPNUMBER								num, numSource, numDest;
  AOBJECTID								oiSource, oiDest;
	pApQueryInfo						pQueryInfo;
	LONG										rVal = A_NOTHANDLED;
	BOOL										bTempObject;
	BOOL										bWasChanged;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED:
			bTempObject = (theSystem->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE;
			num = new(oiNumber) NUMBER(oiNumber, bTempObject);  //The locks the data
			delete num;  //Just unlocks the data -- DOES NOT DELETE THE DATAS!!!
			rVal = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			oiDest = oiNumber;
      oiSource = (AOBJECTID)theSystem->message3;
			numSource = LockNumberObject(oiSource);
			numDest = LockNumberObject(oiDest);

			if (numSource->HadRangeError())
				{
				bWasChanged = numDest->SetValue(HUGE_VAL, POST_SIGNALS, ROUND_IF_INT);
				numDest->SetRangeError(TRUE, POST_SIGNALS);
				}
			else
				{
				if (numSource->IsReal())
					bWasChanged = numDest->SetValue(numSource->GetFloatValue(), POST_SIGNALS, ROUND_IF_INT);
				else
					bWasChanged = numDest->SetValue(numSource->GetWholeValue(), POST_SIGNALS);
				}

			if (numSource->DisplaysEmpty() != numDest->DisplaysEmpty() )
				{
				numDest->SetDisplaysEmpty(numSource->DisplaysEmpty());
				if (!bWasChanged)
					OWndCallEachVisualInstance(oiDest, (CALLVIPROC)(numDest->VisualInstanceIsHwnd() ? HwndNumberItemForceRedraw : LiteNumberItemForceRedraw), 0L,	0L);
				}

			UnlockNumberObject(oiSource);
			UnlockNumberObject(oiDest);

			rVal = A_OK;
			break;

		case AOBJ_EDITOBJECT:
			rVal = EditNumberObject(oiNumber);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			num = LockNumberObject(oiNumber);
			num->ValidateObjectIDs(oiNumber);
			UnlockNumberObject(oiNumber);
			rVal = A_OK;
			break;

		case AOBJ_READ:
		//Setup the virtual table
			num = LockNumberObject(oiNumber);
			new(num) NUMBER(num);
			rVal = num->DoVersionCheck();
			UnlockNumberObject(oiNumber);
      break;

		case AOBJ_CHECKOBJECT:
			rVal = A_OK;
			break;

		case AOBJ_BUILDSIGNALS:
			num = LockNumberObject(oiNumber);
			num->BuildSignals((AMEMBLOCKID)theSystem->message2);
			UnlockNumberObject(oiNumber);
			rVal = A_OK;
			break;

		case AOBJ_QUERYPROTOCOL:
			pQueryInfo = (ApQueryInfo*) theSystem->message4;
			if (pQueryInfo->protocol == APROT_WND)
				{
				rVal = AP_SUPPORTED;
				pQueryInfo->pfnCreate = (APPCFN) CreateVisualInstance;
				}
			break;

		case AOBJ_IMPORTOBJECT:
			num = LockNumberObject(oiNumber);
			rVal = num->Import();
			UnlockNumberObject(oiNumber);
			break;

		case AOBJ_EXPORTOBJECT:
			num = LockNumberObject(oiNumber);
			rVal = num->Export();
			UnlockNumberObject(oiNumber);
			break;

		}

  return rVal;
	}


////////////////////////////////////////////

extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(
	AOBJECTID								oiItem,
	AOBJECTID								oiParent,
	RECT*										rcItem,
	long										status,
	long										special)
	{
	LPNUMBER								num;
  OpWndItemD*							wi;

	I_UNUSED(special);

	num  = LockNumberObject(oiItem);

	if (num->VisualInstanceIsHwnd())
		wi = new HwndNumberItem(oiItem, oiParent, rcItem, status);
	else
		wi = new LiteNumberItem(oiItem, oiParent, rcItem, status);

	UnlockNumberObject(oiItem);

	return wi;
	}

