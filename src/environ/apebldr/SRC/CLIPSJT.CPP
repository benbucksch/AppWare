#include "almsys.h"

#include "clipsjt.h"
#include "recblk.h"
#include "sereclib.h"
#include "rectypes.h"
#include "subject.h"
#include "fileobj.h"
#include "progsys.h"
#include "project.h"
#include "clipparm.h"
#include "clipflow.h"
#include "clipobj.h"
#include "clipfunc.h"
#include "clipcom.h"
#include "clipgrup.h"

CLIPSJT::CLIPSJT(
	SUBJECT * subjectPtr)
	{
	POINT prjOriginOffset;
	POINT position;
	LPFILEOBJECT holdContext;
	LPSTR buffer;
	int length;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	prjOriginOffset = subjectPtr->GetParentProject()->GetOriginOffset();
	position = subjectPtr->GetPositionInProject();
	subjectPtr->GetWindowBounds((LPRECT)&bounds);
	projectOffset.x = position.x - prjOriginOffset.x;
	projectOffset.y = position.y - prjOriginOffset.y;
	originOffset = subjectPtr->GetOriginOffset();
	subjectPtr->GetWindowBounds((LPRECT)&bounds);
	subjectPtr->GetLogBounds((LPRECT)&logBounds);
	length = lstrlen((LPSTR)subjectPtr->GetName()) + 1;
	buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, length);
	lstrcpy(buffer, subjectPtr->GetName());
	status = subjectPtr->GetStatus();

	pTheSystem->PushContext(holdContext);
	params = GetUID(new RECORDBLOCK(sizeof(CLIPPARAM), RT_CLIPPARAM));
	funcs = GetUID(new RECORDBLOCK(sizeof(CLIPFUNC), RT_CLIPFUNC));
	flows = GetUID(new RECORDBLOCK(sizeof(CLIPFLOW), RT_CLIPFLOW));
	groups = GetUID(new RECORDBLOCK(sizeof(CLIPGROUP), RT_CLIPGROUP));
	//Must user RT_OBJINST type so memio stuff will recognize the block
	//as a block containing objects
	objs = GetUID(new RECORDBLOCK(sizeof(CLIPOBJ), RT_OBJINST));
	comments = GetUID(new RECORDBLOCK(sizeof(CLIPCOMMENT), RT_CLIPCOMMENT));
	name = buffer;
	GlobalFree(buffer);

	}

CLIPSJT::~CLIPSJT()
	{
	((LPRECORDBLOCK)GetObject(params))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(objs))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(funcs))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(flows))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(comments))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(groups))->PurgeBlock();

	delete (RECORDBLOCK *)GetObject(params);
	delete (RECORDBLOCK *)GetObject(funcs);
	delete (RECORDBLOCK *)GetObject(flows);
	delete (RECORDBLOCK *)GetObject(objs);
	delete (RECORDBLOCK *)GetObject(comments);
	delete (RECORDBLOCK *)GetObject(groups);
	}

SUBJECT * CLIPSJT::UpdateEnvSubject(
	RUNTIMEUID subjectBlock,
	POINT      pasteOffset)
	{
	LPFILEOBJECT holdContext;
	SUBJECT * subjectPtr;
	LPSTR buffer;
	POINT tempPrjOffset;

	buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, name.strlen()+1);
	lstrcpy(buffer, name);

	holdContext = PROGSYSTEM::PopAndRegisterContext();

	tempPrjOffset = projectOffset;
	tempPrjOffset.x += pasteOffset.x;
	tempPrjOffset.y += pasteOffset.y;

	subjectPtr = new(subjectBlock) SUBJECT(buffer, GetUID(
		PROGSYSTEM::GetCurrentProject()), (LPRECT)&bounds, (LPRECT)&logBounds,
		status, tempPrjOffset, originOffset);

	GlobalFree(buffer);

	PROGSYSTEM::PushAndRegisterContext(holdContext);
	return subjectPtr;
	}
