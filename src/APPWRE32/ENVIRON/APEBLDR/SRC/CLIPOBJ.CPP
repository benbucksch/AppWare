#include "almsys.h"

#include "clipobj.h"
#include "seruid.h"
#include "progsys.h"
#include "fileobj.h"
#include "subject.h"
#include "objdesc.h"
#include "objtype.h"
#include "intdebug.h"
#include "stdlib.h"

//only copy datas if the object is not an alias, if it is
//an alias, it will be read in with default datas
CLIPOBJ::CLIPOBJ(
	LPOBJINST objPtr,
	BOOL groupOnClipBoard):
    OBJINST(objPtr->GetTypeID(), "", objPtr->GetObjectStatus(), 0, objPtr->objDesc->GetUID())
	{
	LPVOID dataPtr;
	LONG dataCount;
	LONG dataSize;
	LPFILEOBJECT holdContext;
	char buffer[64];
	LONG i;
	ADMNPROC admnProc;
	AObjMessage theSystem;
	long admnResult;

	oldItemIndex = objPtr->GetOldIndex();

	status = objPtr->GetStatus();

  //copy name from the source object

	holdContext = PROGSYSTEM::PopAndRegisterContext();
  //out of clipboard context
	oldUID = GetUID(objPtr);

	I_ASSERT(lstrlen(objPtr->GetName()) < sizeof(buffer));

	lstrcpy(buffer, (LPSTR)objPtr->GetName());

	PROGSYSTEM::PushAndRegisterContext(holdContext);
  //back to clipboard context
	UpdateName(buffer);

	if(!objPtr->TestAliasFlag())
		{
		holdContext = PROGSYSTEM::PopAndRegisterContext();

		dataCount = objPtr->GetDatasCount();
	
		theSystem.message1 = AOBJ_PREWRITE;
		theSystem.message2 = 0;
		theSystem.message3 = 0;
		theSystem.message4 = 0;

		admnProc = objPtr->GetSubjectPtr()->GetTypePtr(typeID)->GetAdmnProc();
		I_ASSERT(admnProc != NULL);

		admnResult = admnProc(oldUID.id, &theSystem);

		I_ASSERT(admnResult != A_ERROR);

		PROGSYSTEM::PushAndRegisterContext(holdContext);
		
		//Set the data count - avoids memory fragmentation caused by 
		//calling AddData for every data
		this->SetDatasCount(dataCount);

    //copy the source object datas
		for(i=0;i<dataCount;i++)
			{
			holdContext = PROGSYSTEM::PopAndRegisterContext();
      //out of clipboard context
			dataPtr = objPtr->LockDataPtr(i);
			dataSize = objPtr->GetDataSize(i);
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			//back in clipboard context
			this->SetData(i, dataPtr, dataSize);

			//get out of clipboard
			holdContext = PROGSYSTEM::PopAndRegisterContext();
      
      if(dataPtr)
				objPtr->UnlockDataPtr(i);
				
			//go back to clipboard
			PROGSYSTEM::PushAndRegisterContext(holdContext);	
			}

		holdContext = PROGSYSTEM::PopAndRegisterContext();

		theSystem.message1 = AOBJ_POSTWRITE;
		theSystem.message2 = 0;
		theSystem.message3 = 0;
		theSystem.message4 = 0;

		admnProc = objPtr->GetSubjectPtr()->GetTypePtr(typeID)->GetAdmnProc();
		I_ASSERT(admnProc != NULL);

		admnResult = admnProc(oldUID.id, &theSystem);

		I_ASSERT(admnResult != A_ERROR);

		PROGSYSTEM::PushAndRegisterContext(holdContext);

		}

	if(groupOnClipBoard)
		groupUID = NULLSERUID; 	//the group knows who we are so set to NULL to signal that
														//we belong to a clipboard group.
	else
  	{
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		//let the group know where it came from for undoing purposes.
		groupUID = objPtr->GetGroupUID();
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		}
  }

LPOBJINST CLIPOBJ::UpdateEnvObj(
	SUBJECT * envSubjectPtr,
	SERUID typeUID)
	{
	LPOBJINST objPtr;
	LPFILEOBJECT holdContext;
	SERUID subjectUID;
	char buffer[64];
	LPVOID dataPtr;
	LONG dataCount;
	LONG dataSize;
	LONG i;
	ADMNPROC admnProc;
	AObjMessage theSystem;
  SERUID envObjUID;
	long admnResult;

	I_ASSERT(lstrlen(GetName()) < sizeof(buffer));
	lstrcpy(buffer, (LPSTR)GetName());
	
	dataCount = this->GetDatasCount();
	holdContext = PROGSYSTEM::PopAndRegisterContext();
	subjectUID = GetUID(envSubjectPtr);

	objPtr = new(envSubjectPtr->GetObjectList())
			OBJINST(typeID, (LPSTR)buffer, objectStatus, subjectUID, typeUID);

	I_ASSERT(objPtr);
	
	envObjUID = GetUID(objPtr);

	if(TestSelectFlag())
		objPtr->SetSelectFlag();
	objPtr->SetOldIndex(oldItemIndex);
	objPtr->SetOldUID(oldUID);
  objPtr->SetGroupUID(groupUID);
	copyUID = GetUID(objPtr);
	PROGSYSTEM::PushAndRegisterContext(holdContext);

	//in a clipboard context at this point
	if(!this->TestAliasFlag())
		{
		//set the data size to avoid memory fragmentation caused by
		//AddData
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		objPtr->SetDatasCount(dataCount);
		PROGSYSTEM::PushAndRegisterContext(holdContext);

		//begin loop in a clipboard context
		for(i=0;i<dataCount;i++)
			{
			dataPtr = this->LockDataPtr(i);
			dataSize = this->GetDataSize(i);

			//out of clipboard context
			holdContext = PROGSYSTEM::PopAndRegisterContext();
		
			objPtr->SetData(i, dataPtr, dataSize);
		
			//back to a clipboard context
			PROGSYSTEM::PushAndRegisterContext(holdContext);

      if(dataPtr)
				this->UnlockDataPtr(i);
			}
		}
	else
		SetSharedObjectUID(NULLSERUID);
																					
	//Fix for sending an awaken message to an object which is pasted in.
	//Muse be send after the data's are copied.

  //get out of clipboard context
	holdContext = PROGSYSTEM::PopAndRegisterContext();

	if(TestAliasFlag())
		{
		//create with datas so the linkaliasobject routines will be in sync
		theSystem.message1 = AOBJ_CREATED;
		}
	else
		theSystem.message1 = AOBJ_AWAKENED;
	
	theSystem.message2 = 0;
	theSystem.message3 = 0;
	theSystem.message4 = 0;

	admnProc = envSubjectPtr->GetTypePtr(typeID)->GetAdmnProc();
	admnResult = admnProc(envObjUID.id, &theSystem);

	if(admnResult == A_ERROR)
		{
		MessageBeep(0);

		MessageBox(NULL, "An object was copied but not created. To prevent data "
			"corruption, AppWare is being terminated.", "Fatal Copy Error",
			MB_TASKMODAL|MB_ICONEXCLAMATION|MB_OK);

		exit(1);
		}

	//go back to clipboard context
	PROGSYSTEM::PushAndRegisterContext(holdContext);

	//End fix

	return objPtr;
	}
