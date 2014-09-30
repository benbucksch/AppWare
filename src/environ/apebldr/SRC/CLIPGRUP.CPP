#include "almsys.h"

#include "clipgrup.h"
#include "objinst.h"
#include "seruid.h"
#include "progsys.h"
#include "fileobj.h"
#include "subject.h"
#include "objgroup.h"
#include "serblk.h"
#include "clipobj.h"

//enter in a clipboard context
CLIPGROUP::CLIPGROUP(
	LPOBJGROUP groupPtr)
	{
	LPFILEOBJECT holdContext;
	LPSTR buffer;
	int nameLength;
	POINT originOffset;
  POINT oldLocation;

	baseStatus = groupPtr->GetStatus();

	//copy name from the source object to buffer

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	//back to the project's context

	nameLength = lstrlen(groupPtr->GetName());
	buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, nameLength+1);
	lstrcpy(buffer, groupPtr->GetName());

  //figure the offset from the group to the upper left corner of the window.
	originOffset = groupPtr->GetSubjectPtr()->GetOriginOffset();
	oldLocation = groupPtr->GetLocation();
	offset.x = oldLocation.x - originOffset.x;
	offset.y = oldLocation.y - originOffset.y;

	PROGSYSTEM::PushAndRegisterContext(holdContext);
	//back to clipboard context

	name= buffer;
	GlobalFree((HGLOBAL)buffer);

	objCount = groupPtr->GetObjCount();
	groupStatus = groupPtr->GetGroupStatus();
	baseStatus = groupPtr->GetStatus();
	groupInfo = new GROUPINFO();
	groupInfo->mark = groupPtr->GetMark();
	width = groupPtr->GetWidth();
	objIndexes = GetUID(new SERIUSBLOCK((long)0));
	}

//should enter in a clipboard context.
LPOBJGROUP CLIPGROUP::UpdateEnvGroup(
	SUBJECT * envSubjectPtr,
	POINT pasteOffset)
	{
	LPOBJGROUP groupPtr;
	LPFILEOBJECT holdContext;
	SERUID subjectUID;
	LPSTR buffer;
	LPSERUID envDataPtr;
	LPSERUID clipDataPtr;
	LPCLIPOBJ clipObjPtr;
  LPOBJINST envObjPtr;
	int i;
	POINT newLocation;
  POINT originOffset;

	buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, name.strlen()+1);
	lstrcpy(buffer, name);
	holdContext = PROGSYSTEM::PopAndRegisterContext();
	//out of clipboard context

	subjectUID = GetUID(envSubjectPtr);

	//figure the new location based on an offset from the upper left corner
	//of the window.
  originOffset = envSubjectPtr->GetOriginOffset();
	newLocation.x = originOffset.x + offset.x + pasteOffset.x;
	newLocation.y = originOffset.y + offset.y + pasteOffset.y;

	groupPtr = new(envSubjectPtr->GetObjGroupList())
			OBJGROUP(subjectUID, newLocation, baseStatus, groupStatus, width,
				objCount, buffer, groupInfo);

	//if the group was on the clipboard as active, make it the active group of
	//the subject.
	if(groupPtr->TestActiveFlag())
		envSubjectPtr->ResetActiveGroup(GetUID(groupPtr));
	
	copyUID = GetUID(groupPtr);

	envDataPtr = groupPtr->GetIndexesPtr();

	PROGSYSTEM::PushAndRegisterContext(holdContext);

	clipDataPtr = (LPSERUID)((LPSERIUSBLOCK)GetObject(objIndexes))->GetData();

	//construct the environment group's objlist.
  //begin loop in a clipboard context
	for(i=0;i<objCount;i++)
		{
		clipObjPtr = (LPCLIPOBJ)GetObject(clipDataPtr[i]);
    envDataPtr[i] = clipObjPtr->copyUID;

		holdContext = PROGSYSTEM::PopAndRegisterContext();
    //out of clipboard stuff so set some members of the environment object
    envObjPtr = (LPOBJINST)GetObject(envDataPtr[i]);
		if(envObjPtr->TestSelectFlag())
			groupPtr->IncSelObjCount();

		envObjPtr->SetGroupUID(GetUID(groupPtr));

		envObjPtr->SetIndex(i);

		//go back to clipboard context to begin loop
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		}

	GlobalFree((HGLOBAL)buffer);
	return groupPtr;
	}

//creates a list of clipObjs based on the env group's list of env objs.
void CLIPGROUP::CreateObjList(
	LPOBJGROUP groupPtr)
	{
	LPSERUID dataPtr;
	LPFILEOBJECT holdContext;
	int i;
	LPSERUID envDataPtr;
  LPOBJINST envObj;

  ((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(sizeof(SERUID) * objCount);
	dataPtr = (LPSERUID)((LPSERIUSBLOCK)GetObject(objIndexes))->GetData();

	holdContext = PROGSYSTEM::PopAndRegisterContext();
  //out of clipboard context here
	envDataPtr = (LPSERUID)((LPSERIUSBLOCK)GetObject(groupPtr->GetObjBlock()))->GetData();
	for(i=0;i<objCount;i++)
		{
		envObj = (LPOBJINST)GetObject(envDataPtr[i]);
		dataPtr[i] = envObj->copyUID;
		}

	PROGSYSTEM::PushAndRegisterContext(holdContext);
	//leave in clipboard context
	}
