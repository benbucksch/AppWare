#include "b_includ.h"

extern LONG EditButtonObject(AOBJECTID oi);
extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status, long special);

/////////////////////////////

extern "C" LONG ALMCALLBACK ButtonAdminProc(
	AOBJECTID 								oiButton,
	AObjMessage*					  theSystem)
  {
	LPBUTTON								but, butSource, butDest;
  AOBJECTID								oiSource, oiDest;
	pApQueryInfo						pQueryInfo;
	LONG										rVal = A_NOTHANDLED;
	BOOL										bTempObject;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED:
			bTempObject = (theSystem->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE;
			new(oiButton) BUTTON(oiButton, bTempObject);
			rVal = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			oiDest = oiButton;
      oiSource = (AOBJECTID)theSystem->message3;
			butSource = LockButtonObject(oiSource);
			butDest = LockButtonObject(oiDest);

			butDest->SetTitle(butSource->GetTitle());
			butDest->SetButtonSelected(butSource->IsButtonSelected());

			UnlockButtonObject(oiSource);
			UnlockButtonObject(oiDest);

			rVal = A_OK;
			break;

		case AOBJ_EDITOBJECT:
			rVal = EditButtonObject(oiButton);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			but = LockButtonObject(oiButton);
			rVal = but->ValidateObjectIDs(oiButton);
			UnlockButtonObject(oiButton);
			break;

		case AOBJ_AWAKENED:
			//but = LockButtonObject(oiButton);
			//but->m_bBeingTracked = FALSE;
			//UnlockButtonObject(oiButton);
			//rVal = A_OK;
			//break;

		case AOBJ_READ:
		//Setup the virtual table
			rVal = CheckButtonObjectVersion(oiButton);

			but = LockButtonObject(oiButton);
			new(but) BUTTON(but);
			UnlockButtonObject(oiButton);
      break;

		case AOBJ_CHECKOBJECT:
			but = LockButtonObject(oiButton);
			but->MakeSureButtonHasTitle();
			UnlockButtonObject(oiButton);
			rVal = A_OK;
			break;

		case AOBJ_BUILDSIGNALS:
			but = LockButtonObject(oiButton);
			but->BuildSignals((AMEMBLOCKID)theSystem->message2);
			UnlockButtonObject(oiButton);
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
			but = LockButtonObject(oiButton);
			rVal = but->Import();
			UnlockButtonObject(oiButton);
			break;

		case AOBJ_EXPORTOBJECT:
			but = LockButtonObject(oiButton);
			rVal = but->Export();
			UnlockButtonObject(oiButton);
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
	LPBUTTON								but;
  OpWndItemD*							wi;

	I_UNUSED(special);

	but  = LockButtonObject(oiItem);
	I_ASSERT_POINTER(but, return NULL);

	wi = but->CreateVisualInstance(oiItem, oiParent, rcItem, status);
	UnlockButtonObject(oiItem);

	return wi;
	}


