#include "almsys.h"

#include "clipfunc.h"
#include "seruid.h"
#include "progsys.h"
#include "funcinst.h"
#include "subject.h"
#include "clipsjt.h"
#include "sereclib.h"


CLIPFUNC::CLIPFUNC(
	FUNCINST * funcPtr,
	LPCLIPSJT subjectPtr)
	{
	POINT location;

	location = funcPtr->GetLocation();
	offset.x = location.x - subjectPtr->originOffset.x;
	offset.y = location.y - subjectPtr->originOffset.y;
	status = funcPtr->GetStatus();
	typeID = funcPtr->GetTypeID();
	}


void CLIPFUNC::UpdateEnvFunc(
	SUBJECT * envSubjectPtr,
	POINT pasteOffset)
	{
	FUNCINST * funcPtr;
	LPFILEOBJECT holdContext;
	POINT location;
	POINT originOffset = envSubjectPtr->GetOriginOffset();
	SERUID subjectUID;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	subjectUID = GetUID(envSubjectPtr);
	location.x = originOffset.x + offset.x + pasteOffset.x;
	location.y = originOffset.y + offset.y + pasteOffset.y;
	funcPtr = new(envSubjectPtr->GetFunctionList())
		FUNCINST(subjectUID, typeID, location);
	copyUID = GetUID(funcPtr);
	if(status& FWO_SELECTED)
		funcPtr->SetSelected(TRUE);
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}
