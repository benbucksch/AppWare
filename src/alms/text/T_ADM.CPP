#include "t_includ.h"

extern LONG EditTextObject(AOBJECTID oi);
extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status, long special);

//////////////////////////////////////////////////
//////////////////////////////////////////////////

extern "C" LONG ALMCALLBACK TxtAdminProc(
	AOBJECTID 								oiText,
	AObjMessage*					  theSystem)
  {
	LPTEXTOBJECT									txt;
	LPSTR										szSource;
  AOBJECTID								oiSource, oiDest;
	pApQueryInfo						pQueryInfo;
	LONG										rVal = A_NOTHANDLED;
	BOOL										bTempObject;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED:
			bTempObject = (theSystem->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE;
			txt = new(oiText) TEXTOBJECT(oiText, bTempObject);  //The locks the data
			delete txt;  //Just unlocks the data -- DOES NOT DELETE THE DATAS!!!
			rVal = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			oiDest = oiText;
      oiSource = (AOBJECTID)theSystem->message3;
			szSource = LockTextString(oiSource);
			I_ASSERT_STRING(szSource, "AOBJ_ASSIGNOBJECT", break);
			OTextSetValueFromBuffer(oiDest, szSource);
			UnlockTextString(oiSource);
			rVal = A_OK;
			break;

		case AOBJ_EDITOBJECT:
			rVal = EditTextObject(oiText);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			txt = LockTextStyle(oiText);
			txt->ValidateObjectIDs(oiText);
			UnlockTextStyle(oiText);
			rVal = A_OK;
			break;

		case AOBJ_READ:
			txt = LockTextStyle(oiText);
			rVal = txt->DoVersionCheck();
			UnlockTextStyle(oiText);
			break;

		case AOBJ_CHECKOBJECT:
			rVal = A_OK;
			break;

		case AOBJ_BUILDSIGNALS:
			txt = LockTextStyle(oiText);
			txt->BuildSignals((AMEMBLOCKID)theSystem->message2);
			UnlockTextStyle(oiText);
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
			txt = LockTextStyle(oiText);
			rVal = txt->Import();
			UnlockTextStyle(oiText);
			break;

		case AOBJ_EXPORTOBJECT:
			txt = LockTextStyle(oiText);
			rVal = txt->Export();
			UnlockTextStyle(oiText);
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
	LPTEXTOBJECT									txt;
  OpWndItemD*							wi;

	I_UNUSED(special);

	txt = LockTextStyle(oiItem);

	if (txt->IsStatic())
		{
		if (txt->HasFrame())
			wi = new LiteTextItemFramed(oiItem, oiParent, rcItem, status);
		else
			wi = new LiteTextItemTransparent(oiItem, oiParent, rcItem, status);
		}
	else
		wi = new HwndTextItem(oiItem, oiParent, rcItem, status);

	UnlockTextStyle(oiItem);

	return wi;
	}


