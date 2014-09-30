#include "almsys.h"

#include "clipcom.h"
#include "seruid.h"
#include "progsys.h"
#include "funcmask.h"
#include "comment.h"
#include "clipsjt.h"
#include "subject.h"
#include "sereclib.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CLIPCOMMENT constructor
//
// CLASS.......:  CLIPCOMMENT
//
// DESCRIPTION.:  Initializes a comment for the clipboard context using
//								data from the design comment pointed to by commentPtr
//
/////////////////////////////////////////////////////////////////////////////
CLIPCOMMENT::CLIPCOMMENT(
	COMMENT * commentPtr,
	LPCLIPSJT subjectPtr)
	{
	POINT location;
	int nameLength;
	LPSTR buffer, buffer2;
	LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	location = commentPtr->GetLocation();
	offset.x = location.x - subjectPtr->originOffset.x;
	offset.y = location.y - subjectPtr->originOffset.y;
	status = commentPtr->GetStatus();
	nameLength = commentPtr->briefComment.strlen();
	buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, nameLength+1);
	lstrcpy(buffer, commentPtr->briefComment);;
	nameLength = commentPtr->fullComment.strlen();
	buffer2 = (LPSTR)GlobalAlloc( GMEM_ZEROINIT, nameLength+1);
	lstrcpy(buffer2, commentPtr->fullComment);
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	name = buffer;
	body = buffer2;
	GlobalFree((HGLOBAL)buffer);
	GlobalFree((HGLOBAL)buffer2);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  UpdateEnvComment
//
// CLASS.......:  CLIPCOMMENT
//
// DESCRIPTION.:  Adds a comment to the design subject pointed to by
//								enbSubjectPtr and fills it with the data in the
//								clipboard comment.
//
/////////////////////////////////////////////////////////////////////////////
void CLIPCOMMENT::UpdateEnvComment(
	SUBJECT * envSubjectPtr,
	POINT pasteOffset)
	{
	COMMENT * commentPtr;
	LPFILEOBJECT holdContext;
	POINT location;
	POINT originOffset = envSubjectPtr->GetOriginOffset();
	SERUID subjectUID;
	int nameLength;
	LPSTR buffer, buffer2;


	nameLength = name.strlen();
	buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, nameLength+1);
	lstrcpy(buffer, name);;
	nameLength = body.strlen();
	buffer2 = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, nameLength+1);
	lstrcpy(buffer2, body);

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	subjectUID = GetUID(envSubjectPtr);
	location.x = originOffset.x + offset.x + pasteOffset.x;
	location.y = originOffset.y + offset.y + pasteOffset.y;
	commentPtr = new(envSubjectPtr->GetCommentList())
		COMMENT(subjectUID,location,buffer,buffer2);
	if(status& FWO_SELECTED)
		commentPtr->SetSelected(TRUE);
	PROGSYSTEM::PushAndRegisterContext(holdContext);
  
  GlobalFree((HGLOBAL)buffer);
  GlobalFree((HGLOBAL)buffer2);
	}
