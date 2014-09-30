#include "almsys.h"

#include "sjtwin.h"
#include "subject.h"
#include "project.h"                          
#include "mdiclass.h"
#include "fwinobj.h"
#include "objinst.h"
#include "objtype.h"
#include "funcdesc.h"
#include "comment.h"
#include "rectypes.h"
#include "progsys.h"
#include "exetypes.h"
#include "exeobjs.h"
#include "exefunc.h"
#include "cliphead.h"
#include "clipparm.h"
#include "clipfunc.h"
#include "clipflow.h"
#include "clipObj.h"
#include "clipcom.h"
#include "clipgrup.h"
#include "exelibl.h"
#include "response.h"
#include "exeevntl.h"
#include "errors.h"
#include "upsf_env.h"
#include "cfglibp.h"
#include "objgroup.h"
#include "funcinst.h"
#include "navcls.h"
#include "resconst.h"
#include "clipsjt.h"
#include "printer.h"
#include "objdesc.h"
#include "zlist.h"
#include "palette.h"
#include "oiputils.h"
#include "list.h"
#include "uidlist.h"
#include "intdebug.h"
#include "param.h"
#include "upsfname.h"
#include "bitree.h"
#include "apbutil.h"
#include "flowstak.h"
#include "flow.h"

#define SJT_SELECTEDBORDER 2
#define SJT_DEFAULTFUNCSPACE 10
#define SJT_DEFAULTCOMMENT "Couldn't Import Text"
#define SJT_DEFAULTGROUPNAME "????"

LPSERUID	  		SUBJECT::objArray = NULL;
int							SUBJECT::sizeObjArray=0;
LPSERUID	  		SUBJECT::funcArray = NULL;
int							SUBJECT::sizeFuncArray=0;
LPBITREE				SUBJECT::objTree = NULL;
LPBITREE				SUBJECT::funcTree = NULL;


//structure for associating typeID's with an index.
struct IDINDEX:public BITREEMEMBER
	{
	ATYPEID m_typeID;
	int m_index;

	IDINDEX(ATYPEID typeID, int index):m_typeID(typeID), m_index(index)
		{
		}

	int Compare(
		LPBITREEMEMBER node)
		{
		if(m_typeID < ((IDINDEX *)node)->m_typeID)
			return -1;
		else if(m_typeID > ((IDINDEX *)node)->m_typeID)
			return 1;
		else return 0;
		}															 

	int MatchCriteria(long criteria)
	{
		if(m_typeID < criteria)
			return -1;
		else if(m_typeID > criteria)
			return 1;
		else
			return 0;
		}
	};





typedef IDINDEX * LPIDINDEX;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SUBJECT
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  constructor for when a new subject is created
//
// PARAMETERS..:  LPSTR newName:  name of the subject
//                SERUID projectUID
//
/////////////////////////////////////////////////////////////////////////////
SUBJECT::SUBJECT(
	LPCSTR   newName,
	SERUID  projectUID,
	WORD constructorType):
		WORKSHEET()
	{
	RECT clientRect;
	PROJECT * projPtr;

	parentProjectUID = projectUID;

	SetName(newName);
	ConstructorInitialize();
	SetSignalNamesFlag();
	GetClientRect(PROGSYSTEM::hWndClient, &clientRect);
	projPtr = GetParentProject();
	if (!projPtr->TestIconicFlag())
		{
		clientRect.top+= (GetSystemMetrics(SM_CYCAPTION) +
			GetSystemMetrics(SM_CYFRAME)-1);
		}

	SetWindowBounds(&clientRect);
	logBounds.left = XMAXPAGE/2 - GetSystemMetrics(SM_CXSCREEN);
	logBounds.right = logBounds.left + 2 * GetSystemMetrics(SM_CXSCREEN);
	logBounds.top = YMAXPAGE/2 - GetSystemMetrics(SM_CYSCREEN);
	logBounds.bottom = logBounds.top + 2 * GetSystemMetrics(SM_CYSCREEN);
	originOffset.x = logBounds.left;
	originOffset.y = logBounds.top;
	GetSubjectClientBounds(&clientRect);
	projPtr->CalculateSubjectPosition(GetUID(this));
	ClearIconicFlag();
	ClearMaximizeFlag();

	if(PROGSYSTEM::TestSignalNamesPref())
		SetSignalNamesFlag();
	else
		ClearSignalNamesFlag();

	SetGrid(PROGSYSTEM::TestSjtGridPref());

	if(PROGSYSTEM::TestLargeIconsPref())
		SetBigIconFlag();
	else
		ClearBigIconFlag();

	if(constructorType & SJT_NORMALCONSTRUCTOR)
		{
		ClearIconicFlag(); //makw sure window will be shown
		projPtr->OnlyChangeSelection(GetUID(this));
		ShowObjectWindow();
		projPtr->MakeDirty();
		projPtr->ResetLogicalBounds();
		}
	PROGSYSTEM::ClearUndoFlag();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SUBJECT
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  constructor for when a new subject is created from
// 								clipboard
//
//
/////////////////////////////////////////////////////////////////////////////
SUBJECT::SUBJECT(
	LPCSTR   newName,
	SERUID  projectUID,
	LPRECT  winBounds,
	LPRECT  logicalBounds,
	LONG    newStatus,
	POINT   projectOffset,
	POINT   newOriginOffset):
		WORKSHEET()
	{
	POINT projectOriginOffset;

	status = newStatus;
	parentProjectUID = projectUID;
	SetName(newName);
	SetWindowBounds(winBounds);
	logBounds = *logicalBounds;
	ConstructorInitialize();
	projectOriginOffset = GetParentProject()->GetOriginOffset();
	projectOffset.x+=projectOriginOffset.x;
	projectOffset.y+=projectOriginOffset.y;
	SetPositionInProject(projectOffset);
	originOffset = newOriginOffset;
	if(!TestIconicFlag())
		{
		ShowObjectWindow();
		}
	else
		{
		SetOrder(GetMaxOrder());
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SUBJECT
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  Initialization routine
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::ConstructorInitialize()
	{
	PROJECT * projPtr = GetParentProject();

	Initialize();
	nObjectsCount = 0;
	activeGroup = NULLSERUID;
	theObjects = GetUID(new RECORDBLOCK(sizeof(OBJINST), RT_OBJINST));
	theSubTypes = GetUID(new RECORDBLOCK(sizeof(SUBTYPE), RT_SUBTYPE));
  theFunctions = GetUID(new RECORDBLOCK(sizeof(FUNCINST), RT_FUNCINST));
	theComments = GetUID(new RECORDBLOCK(sizeof(COMMENT), RT_COMMENT));
	theObjGroups = GetUID(new RECORDBLOCK(sizeof(OBJGROUP), RT_OBJGROUP));
	MDI::GetNavigatorPtr()->AddItem(parentProjectUID, GetUID(this),
		NULLSERUID, projPtr->fileObjPtr, GetName());
  projPtr->IncrementSubjectCount();

	}


//private routine to be called from constructors and internally
BOOL SUBJECT::DoPrivateColorSet()
	{
	SetWindowColor(PROGSYSTEM::windowColor);
	return TRUE;
	}


BOOL SUBJECT::SetPrivateColors()
	{

	if(!DoPrivateColorSet())
		return FALSE;

	if(windowPtr)
		{
		if(!((LPSUBJECTWINDOW)windowPtr)->SetPrivateColors())
			return FALSE;
		}

	//No errors occured if we get to here.
	return TRUE;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SUBJECT
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  overridded constructor called when the subject has been
//                  read in from the file pointed to by fileHandle.
//
/////////////////////////////////////////////////////////////////////////////
SUBJECT::SUBJECT(
	void * thisPtr):WORKSHEET(thisPtr), name(&name)
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PostReadInit
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short SUBJECT::PostReadInit()
	{
	int i;
	COMMENT * commentPtr;
	FUNCINST * funcPtr;
	short result = PRJ_READNOERROR;
	short tempResult;
	LPSUBTYPE subTypePtr;
	LPOBJGROUP groupPtr;

	Initialize();
	MDI::GetNavigatorPtr()->AddItem(parentProjectUID,
		GetUID(this), NULLSERUID, GetParentProject()->fileObjPtr, GetName());

	i=0;
	while(!(result & PRJ_READERROR) &&
		(groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i))!=0)
		{
		result |= groupPtr->PostReadInit();

		if(groupPtr->GetObjCount() <= 0)
			{
			delete groupPtr;
			i--;
			}
    }

	i=0;
	while(!(result & PRJ_READERROR) &&
		(funcPtr = (FUNCINST *)GetObject(theFunctions, i))!=0)
		{
		//we must temporarily assign the results of PostReadInit to a temp
		//variable so we can tell each time if that particular function was bad.
		tempResult = funcPtr->PostReadInit();
		if(tempResult & PRJ_READBADFUNC)
			{
			funcPtr->HandleBadFlows();
			delete funcPtr;
			i--;
			}
		result|=tempResult;
		}

	i=0;
	while(!(result & PRJ_READERROR) && 
		(commentPtr = (COMMENT *)GetObject(theComments, i))!=0)
		result |= commentPtr->PostReadInit();


	//make sure all subtypes correspond to a real type
	//Do this after validating all objects so that the objects still have
	//access to all type pointers - even the bad ones.  This allows the 
	//objects to decrement the use count if they are deleted.
	i=0;
	while(!(result & PRJ_READERROR) &&
		(subTypePtr = (LPSUBTYPE)GetObject(theSubTypes, i))!=0)
		{
		tempResult = subTypePtr->PostReadInit();
		if(tempResult & PRJ_READBADTYPE)
			{
			//delete the subtype.
			delete subTypePtr;
			i--;
			}
		result |= tempResult;
		}

	//Now go through and validate all parameters
	//validate the params if the result is anything other than no error.
	i=0;
	while(!(result & PRJ_READERROR) &&
		0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		result |= funcPtr->ValidateParams();

	//NOTE: Make sure all invalid types and objects have been removed before
	//telling the objects to check their versions
	//IMPORTANT: make sure this code is called before validating the flows
	//because the objects may depend on having valid versions when they
	//are queried for their signals.
	if (!(result & PRJ_READERROR))
		result |= this->CheckObjectVersions();


	nObjectsCount = (int)((LPRECORDBLOCK)GetObject(theObjects))->GetCount();

	return result;
	}

short SUBJECT::ValidateFlows()
	{
	short result = PRJ_READNOERROR;
	int i;
	LPOBJINST objPtr;
	FUNCINST * funcPtr;

	i=0;
	while(!(result & PRJ_READERROR) &&
		0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
		result |= objPtr->ValidateFlows(FALSE);	//FALSE means an OBJECT

	i=0;
	while(!(result & PRJ_READERROR) &&
		0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		result |= funcPtr->ValidateFlows(TRUE); //TRUE means a function

	CalculateAllFlowPaths();


	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CheckObjectVersions
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  messages defaults to SUB_SEND_AWAKENED | SUB_SEND_READ
//
/////////////////////////////////////////////////////////////////////////////
short SUBJECT::CheckObjectVersions(
	LONG messages)
	{
	int i;
	LPOBJINST pObject;
	short result = PRJ_READNOERROR;

	i=0;
	while(!(result & PRJ_READERROR) &&
		(0 != (pObject = (LPOBJINST)GetObject(theObjects, i))))
		result |= this->GetTypePtr(pObject->GetTypeID())->
										CheckObjectVersion(::GetUID(pObject), messages);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ~SUBJECT
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  destructor: deletes subject window if it is in memory.
//
/////////////////////////////////////////////////////////////////////////////
SUBJECT::~SUBJECT()
	{
	HWND            hwnd;
	PROJECT *		projPtr;

	//it's possible when the subjectwindow is deleted that a window belonging
	//to a different context will become the active window and therefore the
	//context will change. So we need to push the current context on the stack
	//and pop it when were done to ensure that the subject's context will
	//be active throughout the destructor
	//NOTE:: the assumption has been made that the subject's context will be
	//the current context when the destructor is entered.

	PROGSYSTEM::PushAndRegisterContext(pTheSystem->GetFileObject());
	projPtr = GetParentProject();

	if(windowPtr)
		{
		hwnd = windowPtr->GetWindowHandle();
		SendMessage(hwnd,WM_QUERYENDSESSION, 0, 0L);
		SendMessage(PROGSYSTEM::hWndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L);
		SetWindowPtr(NULL);
		}

	MDI::GetNavigatorPtr()->DeleteItem(projPtr->fileObjPtr, GetUID(projPtr),
		GetUID(this), NULLSERUID);

	((LPRECORDBLOCK)GetObject(theObjects))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(theSubTypes))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(theFunctions))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(theComments))->PurgeBlock();

	((LPRECORDBLOCK)GetObject(theObjGroups))->PurgeBlock();

	delete (RECORDBLOCK *)GetObject(theFunctions);
	delete (RECORDBLOCK *)GetObject(theComments);
	delete (RECORDBLOCK *)GetObject(theObjects);
	delete (RECORDBLOCK *)GetObject(theSubTypes);
	delete (RECORDBLOCK *)GetObject(theObjGroups);

	GetParentProject()->DecrementSubjectCount();
	PROGSYSTEM::PopAndRegisterContext();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ShowObjectWindow
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  Creates the subject window if it is not in memory, else
//                ActivateWindow is called which will active the subject
//                window and bring it to the top.
//
// PARAMETERS..:  BOOL:  returns nonzero to show success.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::ShowObjectWindow()
	{
	if(!windowPtr)
		{
		SetWindowPtr(new SUBJECTWINDOW(GetUID(this), parentProjectUID));
		UpdateWindow(windowPtr->GetWindowHandle());
		}
  else
		ActivateTheWindow();

	return (windowPtr != 0)?TRUE:FALSE;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetSubjectRects
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  returns the rectangles enclosing the subject icon, the
//                  text at the bottom of the icon, and the union of the
//                  previous two rectangles.
//
// PARAMETERS..:  HDC hdc:  device context.
//                LPRECT:   will point to the subject rectangle.
//                LPRECT:   will point to the text rectangle.
//                LPRECT:   will point to the union of the two rectangles.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::GetSubjectRects(
	HDC     hdc,
	LPRECT  subRect,
	LPRECT  textRect,
	LPRECT  unionRect)
	{
	SIZE		textSize;
	POINT projectOriginOffset;
	TEXTMETRIC tm;

	projectOriginOffset = GetParentProject()->GetOriginOffset();
	SelectObject(hdc, subFont);
	subRect->left = positionInProject.x- projectOriginOffset.x;
	subRect->top = positionInProject.y- projectOriginOffset.y;
	subRect->right = subRect->left + SUBJECTWIDTH;
	subRect->bottom = subRect->top + SUBJECTHEIGHT;
	if(lstrlen(GetName()) > 0)
		GetTextExtentPoint32(hdc, GetName(), lstrlen(GetName()), &textSize);
	else
		// M is to hold the place so there will be a text rect
		// if the subject has no name
		GetTextExtentPoint32(hdc, "M", 1, &textSize);

	GetTextMetrics(hdc, &tm);
	textRect->left=(subRect->right + subRect->left -(int)(textSize.cx))/2-
		SUBTEXTBORDER;
	textRect->right = textRect->left + (int)(textSize.cx)  + 2*SUBTEXTBORDER;
	textRect->bottom = subRect->bottom;
	textRect->top = textRect->bottom - tm.tmHeight;
	UnionRect(unionRect,subRect,textRect);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SubjectUnionIntersectsRect
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  determines if the union of the subject and text
//                  rectangles intersect the rectangle pointed to by
//                  refreshRect.
//
// PARAMETERS..:  HDC hdc:  device context.
//                LPRECT  subRect:  will point to the subject rectangle.
//                LPRECT  textRect: will point to the text rectangle.
//                LPRECT  refreshRect:  points to the rectangle to which the
//                  subject and text rectangles are being compared.
//
// RETURN VALUE:  BOOL: non-zero if refreshRect is intersected else
//                  zero.
//
/////////////////////////////////////////////////////////////////////////////
BOOL  SUBJECT::SubjectUnionIntersectsRect(
  HDC hdc,
  LPRECT subRect,
  LPRECT textRect,
  LPRECT refreshRect)
  {
  RECT unionRect;
	RECT tempRect;

  GetSubjectRects(hdc,subRect,textRect,&unionRect);
  return IntersectRect(&tempRect,&unionRect, refreshRect);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AdjustSubjectRects
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  Offsets the subject and text rectangles the amount
//                  specified by adjustment.
//
// PARAMETERS..:  LPRECT subRect: points to the subject rectangle.
//                LPRECT textRect:  points to the text rectangle.
//                POINT adjustment: amount to offset the rectangles in the
//                  x and y direction.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::AdjustSubjectRects(
  LPRECT      subRect,
  LPRECT      textRect,
	POINT     adjustment)
  {
    OffsetRect(subRect, -adjustment.x, -adjustment.y);
    OffsetRect(textRect, -adjustment.x, -adjustment.y);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Initialize
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::Initialize()
	{
	fileObjPtr = GetParentProject()->fileObjPtr;
	DoPrivateColorSet();
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DrawASubject
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  draws a subject
//
// PARAMETERS..:  HDC hdc:  device context.
//                LPRECT clientRect:  portion of window which needs to be
//                  drawn.
//                POINT adjust: amounts to adjust the subject and text
//                  rectangles to make them relative to the clientRect.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::DrawASubject(
	HDC           hdc,
	LPRECT        clientRect,
	POINT         drawOffset)
	{
	RECT          subRect;
	RECT          textRect;

	if(SubjectUnionIntersectsRect(hdc, &subRect, &textRect,
		clientRect))
		{
		AdjustSubjectRects(&subRect, &textRect, drawOffset);
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		SetROP2(hdc, R2_COPYPEN);
		Rectangle(hdc, subRect.left, subRect.top, subRect.right - SHADOW,
			subRect.bottom - SUBICONTEXT);

		if(status & SJT_SELECTED)
			{
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			Rectangle(hdc, subRect.left + SJT_SELECTEDBORDER,
				subRect.top + SJT_SELECTEDBORDER,
				subRect.right -SHADOW - SJT_SELECTEDBORDER,
				subRect.bottom - SUBICONTEXT - SJT_SELECTEDBORDER);

			SetTextColor(hdc, PROJECT::highlightColor);
			}
		else
			SetTextColor(hdc, PROJECT::textColor);

		SelectObject(hdc,shadowPen);
		SetROP2(hdc, R2_COPYPEN);
		MoveToEx(hdc,subRect.right-1, subRect.top + SHADOW, NULL);
		LineTo(hdc,subRect.right-1, subRect.bottom - SUBICONTEXT + SHADOW-1);
		LineTo(hdc,subRect.left + SHADOW,
			subRect.bottom - SUBICONTEXT + SHADOW-1);
		DrawIcon(hdc, subRect.left + SUBICONBORDER,
			subRect.top + SUBICONBORDER, WORKSHEET::subjectIcon);
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, subFont);

		//can't use the PROGSYSTEM text color since that color is based on
		//the window background color while the PROJECT text color is based
		//on the desktop color.
		DrawText(hdc,GetName(),
			lstrlen(GetName()), &textRect,
			DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_NOCLIP );
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SubjectIntersectPoint
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  determines if the point is in either the subject or the
//                  text rectangle.
//
// PARAMETERS..:  HDC hdc:  device context.
//                LPPOINT thePoint: the point in question.
//                LPRECT  subRect:  points to the subject rectangle.
//                LPRECT  textRect: points to the text rectangle.
//
// RETURN VALUE:  BOOL: non-zero if point intersects else zero.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::SubjectIntersectPoint(
  HDC     hdc,
  LPPOINT thePoint)
  {
  RECT  subRect;
  RECT  textRect;
  RECT tempRect;

  GetSubjectRects(hdc, &subRect, &textRect,
    &tempRect);
  return (PtInRect(&subRect, *thePoint) ||
    PtInRect(&textRect, *thePoint));
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PointInTextRect
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::PointInTextRect(
  HDC hdc,
  POINT thePoint)
  {
  RECT subRect;
  RECT textRect;
	RECT tempRect;

  GetSubjectRects(hdc, &subRect, &textRect,
		&tempRect);

  return (PtInRect(&textRect, thePoint));

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Move
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  moves a subject relative to the logical bounds.
//
// PARAMETERS..:  HDC hdc:  device context.
//                LPPOINT offset: how  to move.
//                LPRECT  unionRect:  will point to the rectangle
//                  containing the union of where the subject was and where
//                  it was moved to.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::Move(
  HDC     hdc,
  LPPOINT offset,
  LPRECT  unionRect)
  {
  RECT subRect;
	RECT textRect;
  RECT oldUnion;
  RECT newUnion;

  GetSubjectRects(hdc,&subRect, &textRect,&oldUnion);
  positionInProject.x+=(*offset).x;
	positionInProject.y+=(*offset).y;
  GetSubjectRects(hdc,&subRect, &textRect,&newUnion);
  UnionRect(unionRect, &oldUnion, &newUnion);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AdjustToGrid
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  Snaps the subject to the project windows grid.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::AdjustToGrid()
  {
  int offset;
  BOOL adjusted;
  POINT projectLogBounds;
  BOOL logChanged = FALSE;

	projectLogBounds = GetParentProject()->GetLogBounds();
	if(PROGSYSTEM::prjHGrid > 0)
		{
		offset = positionInProject.x % PROGSYSTEM::prjHGrid;
		if(offset)
			{
			adjusted = 1;
			if(offset>5)
				offset = offset - PROGSYSTEM::prjHGrid;
			positionInProject.x-=offset;
			if(positionInProject.x + SUBJECTWIDTH > projectLogBounds.x)
				{
				projectLogBounds.x = positionInProject.x + SUBJECTWIDTH;
				logChanged = TRUE;
				if(projectLogBounds.x > XMAXPAGE)
					{
					projectLogBounds.x = XMAXPAGE;
					positionInProject.x-=PROGSYSTEM::prjHGrid;
					}
				}
			}
		}

	if(PROGSYSTEM::prjVGrid)
		{
		offset = positionInProject.y % PROGSYSTEM::prjVGrid;
		if(offset)
			{
			adjusted = 1;
			if(offset>5)
				offset = offset-PROGSYSTEM::prjVGrid;
			positionInProject.y-=offset;
			if(positionInProject.y + SUBJECTHEIGHT > projectLogBounds.y)
				{
				logChanged = TRUE;
				projectLogBounds.y = positionInProject.y + SUBJECTHEIGHT;
				if(projectLogBounds.y > YMAXPAGE)
					{
					projectLogBounds.y = YMAXPAGE;
					positionInProject.y-=PROGSYSTEM::prjVGrid;
					}
				}
			}
		}

	if(logChanged)
		GetParentProject()->SetLogBounds(projectLogBounds);

	return (adjusted);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SubjectIntersectsRect
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  determines if the subject rectangle or the text rectangle
//                  intersect rectToIntersect.
//
// PARAMETERS..:  HDC hdc:  device context.
//                  LPRECT rectToIntersect: rectangle in question.
//
// RETURN VALUE:  BOOL: non-zero if rectToIntersect was intersected
//                  else zero.
//
/////////////////////////////////////////////////////////////////////////////
BOOL  SUBJECT::SubjectIntersectsRect(
  HDC hdc,
  LPRECT rectToIntersect)
  {
  RECT subRect;
  RECT textRect;
  RECT unionRect;
  RECT tempRect;
  RECT theRect;
  int temp;
  POINT projectOriginOffset;

  projectOriginOffset = GetParentProject()->GetOriginOffset();

  theRect = *rectToIntersect;

  if(theRect.right < theRect.left)
    {
		temp = theRect.right;
    theRect.right = theRect.left;
    theRect.left = temp;
		}
  else if(theRect.right == theRect.left)
    theRect.right = theRect.left+1;

  if(theRect.bottom < theRect.top)
    {
    temp = theRect.bottom;
    theRect.bottom = theRect.top;
    theRect.top = temp;
    }
  else if(theRect.bottom == theRect.top)
    theRect.bottom = theRect.top+1;

  GetSubjectRects(hdc,&subRect,&textRect,&unionRect);

  OffsetRect(&subRect, projectOriginOffset.x, projectOriginOffset.y);

  OffsetRect(&textRect, projectOriginOffset.x, projectOriginOffset.y);

  return (IntersectRect(&tempRect,&subRect, &theRect)
    || IntersectRect(&tempRect, &textRect, &theRect));

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeFullSubjectName
//
// CLASS.......:  SUBJECTWINDOW
//
// DESCRIPTION.:  combines the subject's name with the parent project's name
//                  and puts the result in subjectBuffer
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::MakeFullSubjectName(
  LPSTR subjectBuffer)
	{

  lstrcpy((LPSTR)subjectBuffer, GetParentProject()->GetName());

  lstrcat((LPSTR)subjectBuffer, ": ");

  lstrcat((LPSTR)subjectBuffer, GetName());

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetSubjectWindowName
//
// CLASS.......:  SUBJECTWINDOW
//
// DESCRIPTION.:  Sets the caption text in the subjectWindow to be that of
//                 the parent project's name combined with the subject's name
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::SetSubjectWindowName()
  {
  char subjectBuffer[2 * SUBJECT_NAME_LENGTH];

	if(windowPtr)
    {
    MakeFullSubjectName((LPSTR)subjectBuffer);
		SetWindowText(windowPtr->GetWindowHandle(), (LPSTR)subjectBuffer);
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Increment Index
//
// CLASS.......:  SUBJECTWINDOW
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::IncrementIndex(
	int amount /*set to a default value*/)
	{
	LPFILEOBJECT fileObj;
	unsigned int index;

	if((fileObj = pTheSystem->GetFileObject())!=0)
		{
		index = (unsigned int) WORKSHEET::indexList->GetWindow(fileObj);
		index+=amount;
		WORKSHEET::indexList->RemoveByPtr(fileObj);
		WORKSHEET::indexList->Add((HWND) index, fileObj);
		}
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetIndex
//
// CLASS.......:  SUBJECTWINDOW
//
/////////////////////////////////////////////////////////////////////////////
unsigned int SUBJECT::GetIndex()
  {
	LPFILEOBJECT fileObj;

	if((fileObj = pTheSystem->GetFileObject())!=0)
		return (unsigned int) WORKSHEET::indexList->GetWindow(fileObj);
	else
		return 0;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  IsSelected
//
// CLASS.......:  SUBJECTWINDOW
//
// DESCRIPTION.:  returns true if the subject is selected
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::IsSelected()
  {
  return (status & SJT_SELECTED)?TRUE:FALSE;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetSelected
//
// CLASS.......:  SUBJECTWINDOW
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::SetSelected(
  BOOL set)
  {
  if(set)
    status|=SJT_SELECTED;
  else
    status&=(~SJT_SELECTED);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetMoiuseOffset
//
// CLASS.......:  SUBJECTWINDOW
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::SetMouseOffset(
  POINT newOffset)
	{
  mouseOffset = newOffset;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetMouseOffset
//
// CLASS.......:  SUBJECTWINDOW
//
/////////////////////////////////////////////////////////////////////////////
POINT SUBJECT::GetMouseOffset()
  {
  return mouseOffset;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CalculateMouseOffset
//
// CLASS.......:  SUBJECTWINDOW
//
// DESCRIPTION.:  calculates the difference between the subject's upper left
//                  corner and the mouse.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::CalculateMouseOffset(
  POINT mouse)
  {
  POINT projectOriginOffset;

  projectOriginOffset = GetParentProject()->GetOriginOffset();
  mouseOffset.x = mouse.x - positionInProject.x +
    projectOriginOffset.x;
  mouseOffset.y = mouse.y - positionInProject.y +
    projectOriginOffset.y;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetPositionInProject
//
// CLASS.......:  SUBJECTWINDOW
//
/////////////////////////////////////////////////////////////////////////////
POINT SUBJECT::GetPositionInProject()
  {
  return positionInProject;
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetPositionInProject
//
// CLASS.......:  SUBJECTWINDOW
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::SetPositionInProject(
  POINT newPosition)
  {
  positionInProject = newPosition;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetBoundingRectangle
//
// CLASS.......:  SUBJECTWINDOW
//
// DESCRIPTION.:  returns the rectangle which completely surrounds the
//                  subject rectangle and the text rectangle
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::GetBoundingRectangle(
	HDC hdc,
  LPRECT boundingRect)
	{
  RECT subRect;
  RECT textRect;

	GetSubjectRects(hdc, &subRect, &textRect, boundingRect);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetTextRect
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::GetTextRect(
  HDC hdc,
  LPRECT textRect)
  {
  RECT subRect, tempRect;

  GetSubjectRects(hdc, &subRect, textRect, &tempRect);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddFunction
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
SERUID SUBJECT::AddFunction(
	ATYPEID  theType,
  POINT newLocation)
	{
  FUNCINST * newFunction;

	GetParentProject()->MakeDirty();
	PROGSYSTEM::ClearUndoFlag();
	newFunction = new(theFunctions) FUNCINST(GetUID(this),theType,newLocation);
	return GetUID(newFunction);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddComment
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
SERUID SUBJECT::AddComment(
	POINT newLocation,
	LPCSTR briefC,
	LPCSTR fullC)
	{
	COMMENT * newComment;

	PROGSYSTEM::ClearUndoFlag();
	GetParentProject()->MakeDirty();
  newComment = new(theComments) COMMENT(GetUID(this), newLocation, briefC,
		fullC);
	return GetUID(newComment);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetFunctionList
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
RUNTIMEUID SUBJECT::GetFunctionList()
  {
  return theFunctions;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetCommentList
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
RUNTIMEUID SUBJECT::GetCommentList()
  {
  return theComments;
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CreateObjInst
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  Creates a new OBJINST record in the theObjects serius block
//
// PARAMETERS..:  creationInfo is optional and defaults to NULL
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SERUID SUBJECT::CreateObjInst(
	LPOBJDESC pObjDesc,
	BOOL      aliasFlag,
	SERUID		typeUID,
	pAObjMessage creationInfo)
	{
	SERUID       objectID;
	AObjMessage theSystem;
	ADMNPROC     admnProc;
	LPOBJINST    pObjInst;
	long				 admnResult;

	if(!pObjDesc)
		objectID = NULLSERUID;
	else
  	{
		pObjInst = new(theObjects) OBJINST(pObjDesc, GetUID(this), typeUID);
		objectID = GetUID(pObjInst);

		//we need to make sure the alias has datas in case the alias is a
		//dummy alias.
		if(creationInfo)
			theSystem = *creationInfo;
		else
			{
			theSystem.message2 = 0;
			theSystem.message3 = 0;
			theSystem.message4 = 0;
			}

		theSystem.message1 = AOBJ_CREATED;

		if(aliasFlag)
			pObjInst->SetAliasFlag();

		admnProc = GetTypePtr(pObjDesc->GetUID())->GetAdmnProc();
		admnResult = admnProc(objectID.id, &theSystem);
		if((admnResult != A_OK) && (admnResult != A_NOTHANDLED))
			{
			delete pObjInst;
			objectID = NULLSERUID;
			}


		PROGSYSTEM::ClearUndoFlag();
		}
	return objectID;

	}

//returns the number of objects of the particular subtype left in the subject
//Also calls a routins off the subtype which handles decrementing the
//real type in the project.
long SUBJECT::DecTypeCount(
	long typeID,
	BOOL realObject)
	{
	LPSUBTYPE subTypePtr;
  long result = -1; //result will be set appropriately if no error  occurs

	if(NULL != (subTypePtr = GetSubTypePtr(typeID)))
		{
		if(0 == (result = subTypePtr->DecObjCount(realObject)))
			delete subTypePtr;
		}
	return result;
	}

//Increments the SubType count of all objects of that type - creates a new
//subtype if neccesary.
//Alos call a routine off the subtype which handles incrementing the real
//type in the project.
//Returns the real typeUID or NULLSERUID if the type couldn't be created.
SERUID SUBJECT::IncTypeCount(
	long typeID,
	BOOL realObject)
	{
	LPOBJTYPE realTypePtr;
	LPSUBTYPE subTypePtr;
  SERUID realTypeUID = NULLSERUID;

	//obtain type ptr creates a typePtr if one does not yet exist.
	//This is necessary becuase in the case of upsf, the typeBlock will
	//not yet have been created.
	realTypePtr = GetParentProject()->ObtainTypePtr(typeID);

	if(realTypePtr && !(realTypePtr->TestBadTypeFlag()))
		{
		//make sure the type is a good type
		if(realTypePtr->GetAdmnProc() !=
			NULL)
			{
			subTypePtr = GetSubTypePtr(typeID);

			if(!subTypePtr)
				{
				//create a new subtype
				subTypePtr = new(theSubTypes) SUBTYPE(typeID, GetUID(realTypePtr));
				}

			subTypePtr->IncObjCount(realObject);
	    realTypeUID = GetUID(realTypePtr);
			}
		}

	return realTypeUID;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  IsGridOn
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::IsGridOn()
	{
	return (status & WRK_GRIDON)?TRUE:FALSE;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetGrid
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
VOID SUBJECT::SetGrid(
	BOOL set)
	{
	if(set)
		status|=WRK_GRIDON;
	else
		status&=(~WRK_GRIDON);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetSubjectWindowBounds
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::GetSubjectWindowBounds(
	LPRECT boundsRect)
	{

	GetWindowBounds(boundsRect);
	InflateRect(boundsRect, -GetSystemMetrics(SM_CXFRAME),
		-GetSystemMetrics(SM_CYFRAME));
	boundsRect->top+=GetSystemMetrics(SM_CYCAPTION);
	//convert rectangle into subjectwindow client coordinates
	//by having the upper left corner at 0,0
	OffsetRect(boundsRect, -boundsRect->left, -boundsRect->top);
//	boundsRect->left+=objectAliasWidth;
//  boundsRect->top+=aliasTitleHeight+1;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetSubjectClientBounds
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::GetSubjectClientBounds(
  LPRECT bounds)
  {

	GetSubjectWindowBounds(bounds);
  OffsetRect(bounds, -bounds->left, -bounds->top);
	bounds->right=bounds->right - GetSystemMetrics(SM_CXVSCROLL)+1;
  bounds->bottom=bounds->bottom - GetSystemMetrics(SM_CYHSCROLL)+2;

	}

//fill rect with the client coordinates of the subject window
//converted to logical coordinates.
void SUBJECT::GetSubjectLogicalBounds(
	LPRECT rect)
	{
	GetSubjectClientBounds(rect);
	OffsetRect(rect, originOffset.x, originOffset.y);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Normalize
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::NormalizeClientBounds(
	LPRECT bounds)
  {
  OffsetRect(bounds, -SEPARATOR, 0);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetTypePtr
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
LPOBJTYPE SUBJECT::GetTypePtr(
  long typeID)
	{
	int       i=0;
	LPSUBTYPE subTypePtr;


	while (0 != (subTypePtr = (LPSUBTYPE)GetObject(theSubTypes, i)))
		if (subTypePtr->typeID == typeID)
			return subTypePtr->GetObjTypePtr();

	return NULL;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetDividerRect
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::GetDividerRect(
	LPRECT bounds)
  {

	GetSubjectClientBounds(bounds);
  bounds->right=0;
	bounds->left-=SEPARATOR;
  OffsetRect(bounds,originOffset.x,originOffset.y);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetName
//
// CLASS.......:  SUBJECT
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::SetName(
  LPCSTR newName)
  {
  name = newName;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetName
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
LPCSTR SUBJECT::GetName()
  {
  return name;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Compile
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  1. Convert each design-time object instance into a
//                   runtime object type / object instance structure.
//                2. Tell each function to compile itself.
//
// PARAMETERS..:  Memory address of the runtime header record.
//
// RETURN VALUE:  nada.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::Compile(
	LPEXEHEAD lpExeHead,
	BOOL updateStatus)
	{
	int           i;
	LPOBJINST     pObjInst;
	LPOBJINST     pAliasObjInst;
	FUNCINST * funcPtr;
	RUNTIMEUID    exeFuncList;
	LPEXEOBJTYPE  pExeObjType;
	LPEXEOBJINST  pExeObjInst;
	BOOL          result = TRUE;
  LPOBJTYPE			typePtr;


	// go over the design-time objects

	i = 0;
	while (0 != (pObjInst = (LPOBJINST)GetObject(theObjects, i)))
		{
		pAliasObjInst = 0;
		if (pObjInst->TestAliasFlag())
			{
			//make sure the real object is compiled if it's not a dummy alias
			if(pObjInst->GetSharedObjectUID() != NULLSERUID)
				{
				//we're linked
				pAliasObjInst = pObjInst;
				pObjInst = (LPOBJINST)GetObject(pObjInst->GetSharedObjectUID());
        }
			}
		if (!pObjInst->TestCompiledFlag())
			{
			if (pObjInst->TestPersistentsFlag())
				lpExeHead->SetHasPersistentData();

			typePtr = (LPOBJTYPE)GetObject(pObjInst->GetTypeRecUID());
      I_ASSERT(typePtr);
			pExeObjType = (LPEXEOBJTYPE)GetObject(typePtr->copyUID);
      I_ASSERT(pExeObjType);

			// create the EXEOBJINST for this OBJINST
			pExeObjInst =
				new(pExeObjType->GetExeObjInstList()) EXEOBJINST(
         pExeObjType->GetTypeID(),
				 pObjInst->GetName(), pObjInst->GetObjectStatus(),
				 pObjInst->GetUseCount(), pObjInst, GetUID(pExeObjType));

			pExeObjType->IncrTypeCount();

			// remember where the EXEOBJINST version of this OBJINST is
			pObjInst->copyUID = GetUID(pExeObjInst);

			// compile this object's signal flows
			pObjInst->CompileFlows(pExeObjInst->GetFlowList());
			pObjInst->SetCompiledFlag();
			}
		else
			// get exeobjinst for compiling flows off alias
			pExeObjInst = (LPEXEOBJINST)GetObject(pObjInst->copyUID);

		if (0 != pAliasObjInst)
			{
			// compile the alias object's signal flows
			pAliasObjInst->CompileFlows(pExeObjInst->GetFlowList());
			pAliasObjInst->SetCompiledFlag();
			}

		if(updateStatus)
			PROJECT::IncStatusBar(2);
		}

	// compile the Functions

	i = 0;
	exeFuncList = lpExeHead->GetExeFuncList();
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		funcPtr->copyUID = GetUID(new(exeFuncList) EXEFUNCINST(
			(short)(funcPtr->GetStatus())));
		funcPtr->Compile();
		if(updateStatus)
			PROJECT::IncStatusBar(2);
		}

	// Fill in the persistent data header
	lpExeHead->InitPersistentDataHeader();

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  LinkAliasObject
//
// CLASS.......:  SUBJECT (static)
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short SUBJECT::LinkAliasObject(
	LPOBJINST  pAliasObj,
	BOOL displayErrorsFlag)
	{
	RUNTIMEUID  subjectList;
	RUNTIMEUID  objectList;
	LPSUBJECT   pSubject;
	LPOBJINST   pObjInst;
	int         subjectIndex;
	int         objInstIndex;
	short        result = CHECK_DUMMYALIASWARNING;
	char        buffer[512];

	I_ASSERT(pAliasObj->TestAliasFlag());

	subjectList = PROGSYSTEM::GetCurrentProject()->GetSubjectBlock();

	subjectIndex = 0;
	while(0 != (pSubject = (LPSUBJECT)GetObject(subjectList, subjectIndex)))
		{
		objectList = pSubject->GetObjectList();
		objInstIndex = 0;
		while(0 != (pObjInst = (LPOBJINST)GetObject(objectList, objInstIndex)))
			{
			if (pObjInst->TestShareFlag() &&
					(pAliasObj->GetTypeID() == pObjInst->GetTypeID()) &&
					(!lstrcmp(pAliasObj->GetName(), pObjInst->GetName())))
				{
				pAliasObj->VerifyAliasDatasStatus(GetUID(pObjInst));
				result = CHECK_NOERRORS;
				break;
				}
			}
		if (result == CHECK_NOERRORS)
			break;
		}

	if (result == CHECK_DUMMYALIASWARNING)
		{
		pAliasObj->VerifyAliasDatasStatus(NULLSERUID);
		if (displayErrorsFlag == TRUE)
			{
			wsprintf(buffer,
			 "The alias object \"%s\" in the subject \"%s\" is not connected to a"
			 " shared object.  An alias object must match a shared object in"
			 " type and name in order to connect to the shared object.",
			 (LPSTR)pAliasObj->GetName(), pAliasObj->GetSubjectPtr()->GetName());
			MessageBox(NULL, buffer, "Unlinked Alias",
				MB_ICONEXCLAMATION|MB_OK|MB_TASKMODAL);
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  LinkAllAliasObjects
//
// CLASS.......:  SUBJECT (static)
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short SUBJECT::LinkAllAliasObjects(
	BOOL displayErrorsFlag)
	{
	RUNTIMEUID  objectList;
	LPOBJINST   pObjInst;
	int         objInstIndex;
	short       result = CHECK_NOERRORS;
	short       tempResult;
	int         response;
	char        buffer[512];

	objectList = this->GetObjectList();
	objInstIndex = 0;
	while(0 != (pObjInst = (LPOBJINST)GetObject(objectList, objInstIndex)))
		{
		if (pObjInst->TestAliasFlag())
			{
			tempResult = SUBJECT::LinkAliasObject(pObjInst, FALSE);
			if ((tempResult != CHECK_NOERRORS))
				{
				result |= tempResult;
				if (displayErrorsFlag)
					{
					wsprintf(buffer,
					 "The alias object \"%s\" in the subject \"%s\" is not connected to a"
					 " shared object.  An alias object must match a shared object in"
					 " type and name in order to connect to the shared object."
					 "\r\rContinue?",
					 (LPSTR)pObjInst->GetName(), this->GetName());
					response = MessageBox(NULL, buffer, "Unlinked Alias",
					 MB_ICONEXCLAMATION|MB_YESNO|MB_TASKMODAL);

					if (response == IDYES)
						result |= CHECK_CONTINUE;
					else
						{
						result |= CHECK_CANCEL;
						break;
						}
					}
				else
					result |= CHECK_DUMMYALIASWARNING;
				}
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  UpdateUIDs
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::UpdateUIDs(
	BOOL updateStatus)
	{
	FUNCINST * funcPtr;
	int i=0;
	LPOBJINST pObjInst;

	while (0 != (pObjInst = (LPOBJINST)GetObject(theObjects, i)))
		{
		pObjInst->UpdateUIDs();
		if(updateStatus)
			PROJECT::IncStatusBar();
		}

	i=0;
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		funcPtr->UpdateUIDs();
		if(updateStatus)
			PROJECT::IncStatusBar();
		}
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DoCopy
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::DoCopy(
	BOOL doDelete)
	{
	LPCLIPHEAD headPtr;
	LPCLIPSJT  clipSjt;
	int i;
	LPOBJGROUP groupPtr;
	FUNCINST * funcPtr;
	COMMENT * commentPtr;
	LPFILEOBJECT holdContext;
	BOOL delOp;
	BOOL objectsDeleted = FALSE;
  int groupDeleteCount = 0; 

	headPtr = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
	delOp = headPtr->TestCompleteSubjectFlag();
	clipSjt = new(headPtr->GetSubjectBlock()) CLIPSJT(this);

	// note: algorythm depends upon objects and functions being deleted first.

	MakeClipGroups(clipSjt, delOp, doDelete);
	MakeClipFuncs(clipSjt, delOp);
	MakeClipParams(clipSjt, delOp);
	MakeClipFlows(clipSjt, delOp);
	MakeClipComments(clipSjt, delOp);

	if(doDelete)
		{
		holdContext = PROGSYSTEM::PopAndRegisterContext();

		//delete the groups and at the same time, search through to find what objects need to
		//be deleted.
		i=0;
		while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
			{
			if(groupPtr->DeleteObjects(delOp || groupPtr->IsSelected()))
      	{
      	objectsDeleted = TRUE;
        groupDeleteCount++;
				}

			if(groupPtr->IsSelected() || delOp || (groupPtr->GetObjCount() == 0))
				{
				if(GetUID(groupPtr) == activeGroup)
					activeGroup = NULLSERUID;

				delete groupPtr;
				i--;
        }
			}

		if(groupDeleteCount > 1)
			{
			//more than 1 group was deleted so unselect the objects which would have been
			//selected after the objects were deleted.
      OnlyUnselectGroups(NULLSERUID); 
      }

		i=0;
		while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
			{
			if(funcPtr->IsSelected() || delOp)
				{
        funcPtr->ClearRequiredTypes();
				delete funcPtr;
				i--;
				}
			}

		i=0;
		while(0 != (commentPtr = (COMMENT *)GetObject(theComments, i)))
			{
			if(commentPtr->IsSelected() || delOp)
				{
				delete commentPtr;
				i--;
				}
			}

		//If were a complete subject we don't want to do this since all
		//the window deleting stuff will happen in the destructor
		if(windowPtr && !delOp)
			{

//			((LPSUBJECTWINDOW)windowPtr)->GetObjListPtr()->
//				DoDelete(delOp);
			((LPSUBJECTWINDOW)windowPtr)->DoDelete();

			if(objectsDeleted)
				CalculateObjectFlows();

			((LPSUBJECTWINDOW)windowPtr)->UpdateSubjectWindow();
			}

		//make sure all dummy aliases have datas and all linked aliases are
		//cleared of datas.
		LinkAllAliasObjects(FALSE);

		PROGSYSTEM::PushAndRegisterContext(holdContext);
		}

	return TRUE;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CopyFuncsOnly
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::DeleteFuncsOnly()
	{
  LPCLIPHEAD headPtr;
  LPCLIPSJT  clipSjt;
  int i;
	FUNCINST * funcPtr;
	COMMENT * commentPtr;

	PROGSYSTEM::PurgeDelFileObj();
	PROGSYSTEM::SetDelContext();
	headPtr = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
	headPtr->ClearCompleteSubjectFlag();
	headPtr->SetCutContextID(GetParentProject()->fileObjPtr->GetFileIDStamp());

	clipSjt = new(headPtr->GetSubjectBlock()) CLIPSJT(this);
	MakeClipFuncs(clipSjt, FALSE);
	MakeClipComments(clipSjt, FALSE);
	PROGSYSTEM::PopAndRegisterContext();
	i=0;
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		if(funcPtr->IsSelected())
			{
      funcPtr->ClearRequiredTypes();
			delete funcPtr;
			i--;
			}
		}

	i=0;
	while(0 != (commentPtr = (COMMENT *)GetObject(theComments, i)))
		{
		if(commentPtr->IsSelected())
			{
			delete commentPtr;
			i--;
			}
		}
	if(windowPtr)
		{
		((LPSUBJECTWINDOW)windowPtr)->DoDelete();
		((LPSUBJECTWINDOW)windowPtr)->UpdateSubjectWindow();
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DoPaste
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::DoPaste(
  LPCLIPSJT clipSjt)
  {
	int i;
  LPCLIPPARAM clipParam;
  LPFILEOBJECT holdContext;
  LPCLIPFUNC clipFunc;
  LPCLIPFLOW clipFlow;
	LPCLIPOBJ  clipObj;
	LPCLIPGROUP clipGroup;
	LPOBJINST objPtr;
	LPCLIPCOMMENT clipComment;
  LPCLIPHEAD    clipHead;
	LPLIST		 looseObjList;
	LPINSERTOBJDATA insertMember;
	SERUID typeUID;

	clipHead = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
	if(!clipHead->TestCompleteSubjectFlag())
		{
		holdContext = PROGSYSTEM::PopAndRegisterContext();

		//only want to unselect the active group if its not an undo.
		OnlyUnselectAll(activeGroup);
		//If its an undo, we want to leave the group active but unselect the objects.
		if(!PROGSYSTEM::TestPasteOpFlag() && activeGroup != NULLSERUID)
			GetActiveGroupPtr()->UnselectAllObjects(NULL);
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		}


	holdContext = PROGSYSTEM::PopAndRegisterContext();
	looseObjList = new LIST();
	PROGSYSTEM::PushAndRegisterContext(holdContext);

	//make the obj's from the clipboard obj's.  Any object which does not
	//belong to a group should be placed in a uidlist.
	i=0;
	while(0 != (clipObj = (LPCLIPOBJ)GetObject(clipSjt->objs, i)))
		{
    //get out of clipboard
		holdContext = PROGSYSTEM::PopAndRegisterContext();

		typeUID = IncTypeCount(clipObj->GetTypeID(), clipObj->TestAliasFlag());
		if(NULLSERUID != typeUID.id)
			{
      //go to clipboard context
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			objPtr = clipObj->UpdateEnvObj(this, typeUID);
			//get out of clipboard context
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			nObjectsCount++;
			GetParentProject()->IncObjectsCount();
			if(objPtr->GetGroupUID() != NULLSERUID)
				{
				//we're not linked to any group
				insertMember = new INSERTOBJDATA(GetUID(objPtr));
				looseObjList->Add(insertMember);
				}
			}
    //go back to clipboard
    PROGSYSTEM::PushAndRegisterContext(holdContext);
		}

	//insert the objects in the looseObjList into the appropriate groups
	if(!looseObjList->IsListEmpty())
  	{
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		//PasteInObjects will be responsible for making sure all members are
		//deleted from looseObjList.
		//the last parameter is TRUE if its an undo operation or FALSE if its a paste.
		PasteInObjects(clipHead, looseObjList, !PROGSYSTEM::TestPasteOpFlag());
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		}

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	delete looseObjList;
	PROGSYSTEM::PushAndRegisterContext(holdContext);

	i=0;
	while(0 != (clipGroup = (LPCLIPGROUP)GetObject(clipSjt->groups, i)))
		clipGroup->UpdateEnvGroup(this, clipHead->GetPasteOffset());

	i=0;
	while(0 != (clipFunc = (LPCLIPFUNC)GetObject(clipSjt->funcs, i)))
		{
		clipFunc->UpdateEnvFunc(this, clipHead->GetPasteOffset());
		}

	i=0;
	while(0 != (clipParam = (LPCLIPPARAM)GetObject(clipSjt->params, i)))
		{
		clipParam->UpdateEnvParam();
		}

	i=0;
	while(0 != (clipComment = (LPCLIPCOMMENT)GetObject(clipSjt->comments, i)))
		{
		clipComment->UpdateEnvComment(this, clipHead->GetPasteOffset());
		}

	i=0;
	while(0 != (clipFlow = (LPCLIPFLOW)GetObject(clipSjt->flows, i)))
		clipFlow->UpdateEnvFlow(this);

	return TRUE;

	}


//DO NOT enter in a clipboard context.
BOOL SUBJECT::PasteInObjects(
  LPCLIPHEAD clipHead,
	LPLIST looseObjList,
	BOOL undoOp)
	{
	LPOBJGROUP groupPtr;
	POINT groupLoc;
	POINT offset;
	BOOL result = TRUE; //assume no error
	LPINSERTOBJDATA member;
	LPOBJINST objPtr;
  char nameBuffer[PRJ_NEWGROUPMAXNAME];

	if(!clipHead->TestCompleteSubjectFlag() && !looseObjList->IsListEmpty())
  	{
		if(!undoOp)
			{
			groupPtr = GetActiveGroupPtr();
			if(!groupPtr)
				{
				GetDefaultGroupLocation(&groupLoc);
				offset = clipHead->GetPasteOffset();
				groupLoc.x+=offset.x;
				groupLoc.y+=offset.y;
        GetParentProject()->GetNewGroupName(nameBuffer);
				groupPtr = new(theObjGroups) OBJGROUP(NULLSERUID, groupLoc, GetUID(this),
					nameBuffer);
				}
			groupPtr->SetActiveFlag();
			result = groupPtr->InsertObjects(NULL, looseObjList,  OBJGROUP_DEFAULTINSERTION, FALSE);
			}
		else
			{
			while(NULL != (member = (LPINSERTOBJDATA)looseObjList->GetNextMember(NULL)))
				{
				objPtr = (LPOBJINST)GetObject(member->uid);
				if(objPtr->GetGroupUID() != NULLSERUID)
        	{
					groupPtr = objPtr->GetGroupPtr();
					if(groupPtr->InsertUndoneObject(objPtr) == -1L)
						result = FALSE;
          else
          	groupPtr->SetActiveFlag();
          }
				looseObjList->Remove(member);
        delete member;
        }
      }
		}

	return result;
  }

//should be entered in a normal context
void SUBJECT::ExistingSubjectPostPaste()
	{
	int i;
	LPOBJINST objPtr;

	i=0;
	while(0!=(objPtr = (LPOBJINST)GetObject(theObjects, i)))
		{
		//if the object is an alias and it doesn't have any flows, make sure
    //it's linked.
		if(objPtr->TestAliasFlag() && (objPtr->GetFlowCount() <= 0))
			LinkAliasObject(objPtr, FALSE);
		objPtr->PostPaste();
		}

	if(GetWindowPtr())
		((LPSUBJECTWINDOW)windowPtr)->UpdateSubjectWindow();
  }


//PostPaste should be entered in a clipboard context
void SUBJECT::PostPaste(
	LPCLIPSJT /*clipSjt*/)
	{
	OBJINST * objPtr;
	FUNCINST * funcPtr;
	LPFILEOBJECT holdContext;
	int i;
	BOOL logBoundsChanged;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	i=0;
	while(0!=(objPtr = (LPOBJINST)GetObject(theObjects, i)))
		objPtr->PostPaste();

	LinkAllAliasObjects(FALSE);

	i=0;
	while(0!=(funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		funcPtr->PostPaste();


	logBoundsChanged = CalculateLogBounds();

	CalculateAllFlowPaths();

	if(windowPtr)
		{
		((LPSUBJECTWINDOW)windowPtr)->RebuildSelectedLists();
		if(logBoundsChanged)
			{
			((LPSUBJECTWINDOW)windowPtr)->AdjustScrollBars();
			}
		((LPSUBJECTWINDOW)windowPtr)->UpdateSubjectWindow();
		}

	//restore clipboard context
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeClipFuncs
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::MakeClipFuncs(
  LPCLIPSJT clipSjtPtr,
  BOOL copyAll)
	{
  int i=0;
  FUNCINST * funcPtr;
  LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
    if(funcPtr->IsSelected() || copyAll)
      {
      PROGSYSTEM::PushAndRegisterContext(holdContext);
      funcPtr->copyUID = GetUID(new(clipSjtPtr->funcs) CLIPFUNC(funcPtr,
        clipSjtPtr));
			holdContext = PROGSYSTEM::PopAndRegisterContext();
      }
    }
  PROGSYSTEM::PushAndRegisterContext(holdContext);
  }

//enter in a clipboard context
void SUBJECT::MakeClipGroups(
	LPCLIPSJT clipSjtPtr,
	BOOL copyAll,
	BOOL doDelete)
	{
	int i=0;
	LPOBJGROUP groupPtr;
	LPFILEOBJECT holdContext;
	BOOL entireGroupCopied;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
  //start loop out of the clipboard context
	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		{
		PROGSYSTEM::PushAndRegisterContext(holdContext);
    //in a clipboard context at this point
		//If doDelete is True, that means we're deleting to the undo context so
		//delete entire group if all objects are selected.
		if(groupPtr->IsSelected() || copyAll ||
			(doDelete && (groupPtr->GetObjCount() == groupPtr->GetSelObjCount())))
    	{
			groupPtr->copyUID = GetUID(new(clipSjtPtr->groups) CLIPGROUP(groupPtr));
      entireGroupCopied = TRUE;
			}
		else
    	entireGroupCopied = FALSE;

		MakeClipObjs(clipSjtPtr, groupPtr, entireGroupCopied);

		if(entireGroupCopied)
			((LPCLIPGROUP)GetObject(groupPtr->copyUID))->CreateObjList(groupPtr);

		holdContext = PROGSYSTEM::PopAndRegisterContext();
		//get out of clipboard context to begin loop
    }
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	//leave in the clipboard context
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeClipObjs
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  if copyAll is true, then the entire group is being copied
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::MakeClipObjs(
	LPCLIPSJT clipSjtPtr,
	LPOBJGROUP envGroupPtr,
  BOOL copyAll)
  {
	int i;
  long objCount;
	OBJINST * objPtr;
  LPSERUID dataPtr;
  LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	//out of clipboard context

	dataPtr = envGroupPtr->GetIndexesPtr();
	objCount = envGroupPtr->GetObjCount();

	//update the old indices
	for(i=0;i<objCount;i++)
		{
    objPtr = (LPOBJINST)GetObject(dataPtr[i]);
		objPtr->SetOldIndex(objPtr->GetIndex());
		}

	for(i=0;i<objCount;i++)
		{
		objPtr = (LPOBJINST)GetObject(dataPtr[i]);
		if(objPtr->TestSelectFlag() || copyAll)
      {
			PROGSYSTEM::PushAndRegisterContext(holdContext);
      //back to clipboard context
			objPtr->copyUID = GetUID(new(clipSjtPtr->objs) CLIPOBJ(objPtr, copyAll));

			holdContext = PROGSYSTEM::PopAndRegisterContext();
      //get out fo clipboard context to begin loop
      }
    }
	PROGSYSTEM::PushAndRegisterContext(holdContext);
  //leave in the clipboard context
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeClipComments
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::MakeClipComments(
  LPCLIPSJT clipSjtPtr,
  BOOL copyAll)
  {
  int i=0;
  COMMENT * commentPtr;
  LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
  while(0 != (commentPtr = (COMMENT *)GetObject(theComments, i)))
    {
    if(commentPtr->IsSelected() || copyAll)
      {
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			new(clipSjtPtr->comments) CLIPCOMMENT(commentPtr, clipSjtPtr);
			holdContext = PROGSYSTEM::PopAndRegisterContext();
      }
    }
  PROGSYSTEM::PushAndRegisterContext(holdContext);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeClipParams
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::MakeClipParams(
	LPCLIPSJT clipSjtPtr,
	BOOL copyAll)
	{
	int i;
	FUNCINST * funcPtr;
	LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	//first delete all selected output params
	i=0;
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		if(PROGSYSTEM::TestCopyOpFlag())
			funcPtr->CopyOutputs(clipSjtPtr->params, copyAll);
		else
			funcPtr->ClearOutputs(clipSjtPtr->params, copyAll);
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		}

	//clear all necessary inputs
	i=0;
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		if(PROGSYSTEM::TestCopyOpFlag())
			funcPtr->CopyInputs(clipSjtPtr->params, copyAll);
		else
			funcPtr->ClearInputs(clipSjtPtr->params, copyAll);
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeClipFlows
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  Must enter in a clipboard context.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::MakeClipFlows(
  LPCLIPSJT clipSjtPtr,
  BOOL copyAll)
  {
  int i;
	FUNCINST * funcPtr;
  OBJINST * objPtr;
  LPFILEOBJECT holdContext;
	BOOL objectAlsoSelected;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	i=0;
	while(0 != (objPtr = (OBJINST *)GetObject(theObjects, i)))
		{
		objectAlsoSelected = (objPtr->TestSelectFlag() ||
			objPtr->GetGroupPtr()->IsSelected());

    PROGSYSTEM::PushAndRegisterContext(holdContext);
    if(PROGSYSTEM::TestCopyOpFlag())
			objPtr->CopyFlows(clipSjtPtr->flows, objectAlsoSelected, copyAll);
    else
			objPtr->ClearFlows(clipSjtPtr->flows, objectAlsoSelected, copyAll);

		holdContext = PROGSYSTEM::PopAndRegisterContext();
    }

  i=0;
  while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
    {
    PROGSYSTEM::PushAndRegisterContext(holdContext);
    if(PROGSYSTEM::TestCopyOpFlag())
      funcPtr->CopyFlows(clipSjtPtr->flows, funcPtr->IsSelected(), copyAll);
    else
      funcPtr->ClearFlows(clipSjtPtr->flows, funcPtr->IsSelected(), copyAll);
		holdContext = PROGSYSTEM::PopAndRegisterContext();
    }
  PROGSYSTEM::PushAndRegisterContext(holdContext);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetObjIconHeight
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
int SUBJECT::GetObjIconHeight()
	{
	return (TestBigIconFlag())?36:20;
	}


// FUNCTION....:  UpdateObjIndexes
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::UpdateObjIndexes()
	{
	LPOBJINST temp;
	int i=0;

	while(0 != (temp = (LPOBJINST)GetObject(theObjects, i)))
		temp->SetIndex(temp->GetOldIndex());
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetObjInsertPosition
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
int SUBJECT::GetObjInsertPosition()
	{
	int result=nObjectsCount;
	int i=0;
	LPOBJINST objPtr;

	while(0 != (objPtr=(LPOBJINST)GetObject(theObjects, i)))
		{
		if(objPtr->TestSelectFlag())
			{
			if(objPtr->GetIndex()<result)
				result = objPtr->GetIndex();

			objPtr->ClearSelectFlag();
			}
		}
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  InsertObjByIndex
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
// PARAMETERS..:  index: -index or where to insert the item
//								numInserted: -number objects inserted during the current
//															paste operation.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::InsertObjByIndex(
	OBJINST * objPtr,
	int  &index,
	int numInserted)
	{
	int i=0;
	LPOBJINST temp;

	objPtr->SetIndex(index);
	while(0 != (temp = (LPOBJINST)GetObject(theObjects, i)))
		{
		if(temp !=objPtr)
			{
			if(temp->GetIndex() <index && temp->GetIndex() >=
				index - numInserted)
				{
				//looking at an object which was inserted previously
				if(temp->GetOldIndex() > objPtr->GetOldIndex())
					{
					temp->SetIndex(temp->GetIndex() + 1);
					objPtr->SetIndex(objPtr->GetIndex() - 1);
					}
				}
			else if(temp->GetIndex() >=index)
				temp->SetIndex(temp->GetIndex() + 1);
			}
		}
	index++;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ChangeSignalDisplay
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::ChangeSignalDisplay()
	{
  LPOBJINST objPtr;
  FUNCINST * funcPtr;
  int i;

	if(TestSignalNamesFlag())
		ClearSignalNamesFlag();
	else
		SetSignalNamesFlag();

  i=0;
  while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
    objPtr->ChangeSignalDisplay(TestSignalNamesFlag());

  i=0;
  while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
    funcPtr->ChangeSignalDisplay(TestSignalNamesFlag());

	((LPSUBJECTWINDOW)windowPtr)->UpdateSubjectWindow();

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ObjectsSelected
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:  returns true in any objects, groups, functions or comments are
//								selected.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::ObjectsSelected()
  {
  BOOL result = FALSE;
	BASEWINDOW * winPtr = windowPtr;

  if(winPtr)
    {
		result = ((LPSUBJECTWINDOW)winPtr)->WorkBenchObjectsSelected() ||
			ObjectsInGroupsSelected();

    }
	return result;
	}


BOOL SUBJECT::ObjectsInGroupsSelected()
	{
	BOOL result = FALSE;
	int i=0;
	LPOBJGROUP groupPtr;

	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		if(groupPtr->ObjectsSelected())
			{
			result = TRUE;
			break;
			}

  return result;
  }



BOOL SUBJECT::FunctionsSelected()
	{
	BASEWINDOW * winPtr = windowPtr;

	if(winPtr)
		return ((LPSUBJECTWINDOW)winPtr)->FunctionsSelected();
	else
		return FALSE;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ItemsSelected
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::ItemsSelected()
	{
	BASEWINDOW * winPtr = windowPtr;

  if(winPtr)
		return((LPSUBJECTWINDOW)winPtr)->ItemsSelected();
  else
		return FALSE;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ClearAllFlowStops
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::ClearAllFlowStops()
  {
  LPOBJINST objPtr;
  FUNCINST * funcPtr;
  int i;
	BOOL redraw = FALSE;
	RECT unionRect;
	RECT flowRect;

	SetRectEmpty(&unionRect);

	i=0;
	while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
		{
		if(objPtr->ClearAllStops(&flowRect))
			{
			redraw = TRUE;
			UnionRect(&unionRect, &unionRect, &flowRect);
			}
		}

	i=0;
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		if(funcPtr->ClearAllStops(&flowRect))
			{
			redraw = TRUE;
			UnionRect(&unionRect, &unionRect, &flowRect);
			}
		}

	if(redraw)
		{
		GetParentProject()->MakeDirty();
		
		if(windowPtr)
			((LPSUBJECTWINDOW)windowPtr)->RefreshClientArea(&unionRect);
		}
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ToggleFlowStops
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:	assumes that the subject contains flows to be toggled.
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::ToggleFlowStops()
  {
  LPOBJINST objPtr;
  FUNCINST * funcPtr;
  int i;
	RECT unionRect;
	RECT flowRect;

	SetRectEmpty(&unionRect);

  i=0;
  while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
		if(objPtr->ToggleStops(&flowRect))
			{
			UnionRect(&unionRect, &unionRect, &flowRect);
			}

	i=0;
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		if(funcPtr->ToggleStops(&flowRect))
			{
			UnionRect(&unionRect, &unionRect, &flowRect);
			}

	if(windowPtr)
		{
		((LPSUBJECTWINDOW)windowPtr)->RefreshClientArea(&unionRect);
		}

	GetParentProject()->MakeDirty();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  FlowStopsSet
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::FlowStopsSet()
  {
  BOOL result = FALSE;

  LPOBJINST objPtr;
  FUNCINST * funcPtr;
  int i;

  i=0;
  while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
    if(objPtr->IsStopSet())
			{
      result = TRUE;
      break;
      }

  if(!result)
		{
    i=0;
    while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
      if(funcPtr->IsStopSet())
        {
        result = TRUE;
        break;
				}
    }
  return result;
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  FlowsSelected
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SUBJECT::FlowsSelected()
  {
  BASEWINDOW * winPtr;

	winPtr = windowPtr;

  if(winPtr)
    return
			((LPSUBJECTWINDOW)winPtr)->FlowsSelected();
  else
    return FALSE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetDesignFlow
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FLOW * SUBJECT::GetDesignFlow(
  LPEXEFLOW exeFlow)
  {
  LPOBJINST objPtr;
  FUNCINST * funcPtr;
  FLOW * flowPtr = NULL;
  int i;

  i=0;
  while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
		{
    if((flowPtr = objPtr->GetDesignFlow(exeFlow))!=NULL)
      break;
    }
  if(!flowPtr)
    {
    i=0;
    while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
      {
      if((flowPtr = funcPtr->GetDesignFlow(exeFlow))!=NULL)
        break;
			}
    }

  return flowPtr;

  }

void SUBJECT::CalculateAllFlowPaths()
  {
  int i = 0;
  FUNCINST * funcPtr;

	CalculateObjectFlows();
	while(0 != (funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		funcPtr->RecalculateFlowPaths();
		}
  }

//returns in unionBounds the union of the flowBounds before change with the
//flowBounds after change
void SUBJECT::CalculateObjectFlows(
	LPRECT unionBounds)
	{
	OBJINST * objPtr;
	int i = 0;
	RECT flowBounds;

	SetRectEmpty(unionBounds);
	while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
		{
		objPtr->RecalculateFlowPaths(&flowBounds);
		UnionRect(unionBounds, unionBounds, &flowBounds);
		}
	}

//just recalculates the flowBounds
void SUBJECT::CalculateObjectFlows()
	{
	OBJINST * objPtr;
	int i = 0;

	while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
		{
		objPtr->RecalculateFlowPaths();
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ShowObject
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::ShowObject(
	SERUID  objID)
	{
	RECT objRect;
	LPOBJINST objPtr;

	if(windowPtr && (this == GetParentProject()->GetActiveSubject()))
		{
    ((LPSUBJECTWINDOW)windowPtr)->HighlightObj((LPOBJINST)GetObject(objID), TRUE, TRUE);
		}
	else
		{
		if(!windowPtr)
			{
			objPtr = (LPOBJINST)GetObject(objID);
			objPtr->GetGroupPtr()->GetObjectRect(objPtr->GetIndex(), &objRect);
			MakeSureRectOnScreen(&objRect);
			OnlyUnselectAll();
			objPtr->GetGroupPtr()->SelectObj(NULL, objPtr, 0);
			objPtr->GetGroupPtr()->SetActiveFlag();
			}
		else
			{
			((LPSUBJECTWINDOW)windowPtr)->HighlightObj((LPOBJINST)GetObject(objID), TRUE, FALSE);
			InvalidateRect(windowPtr->GetWindowHandle(), NULL, FALSE);
			}
		}

	ShowObjectWindow();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DoPrint
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::DoPrint(
	LPPRINTER printPtr)
	{
	LPSTR buffer;
	int i=0;
	LPOBJINST objPtr;

	buffer = (LPSTR) GlobalAlloc(GPTR, name.strlen() + 20);

	wsprintf(buffer, "%s [Subject]", (LPCSTR) name);
	printPtr->DoubleLine(0);
	printPtr->PrintLine(0, buffer);

	GlobalFree((HGLOBAL) buffer);

	while((objPtr = (LPOBJINST)GetObject(theObjects, i))!=NULL)
		objPtr->DoPrint(printPtr, 1);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CleanFuncsOfPrint
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::CleanFuncsOfPrint()
	{
	FUNCINST * funcPtr;
	int	i=0;

	while((funcPtr = (FUNCINST *)GetObject(theFunctions, i))!=NULL)
		funcPtr->CleanPrint();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CheckSubject
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
// ASSUMPTIONS.:	All aliases must be linked before this routine is entered
//
/////////////////////////////////////////////////////////////////////////////
short SUBJECT::CheckSubject(
	BOOL updateStatus)
	{
	short objResult = CHECK_NOERRORS;
	short funcResult = CHECK_NOERRORS;
	SUBJECT * holdSubject;
	PROJECT * projectPtr = GetParentProject();

	//Set the active subject to the current subject so OIP will work correctly
	holdSubject = projectPtr->GetActiveSubject();
	projectPtr->SetActiveSubject(this);

	objResult |= CheckObjects(updateStatus, FALSE, FALSE);
	if(objResult & CHECK_CANCEL)
		{
		//restore the old active subject
		projectPtr->SetActiveSubject(holdSubject);
		return objResult;
		}

	funcResult |= CheckFunctions(updateStatus);
	//Make sure the subject was not brought forward to show functions with
	//errors.  If it was not brought forward, restore the old active subject
	if(!(funcResult & CHECK_CANCEL))
		projectPtr->SetActiveSubject(holdSubject);

	return (objResult | funcResult);

	}


//Check for errors should only be called to check an individual subject
void SUBJECT::CheckForErrors()
	{
	short result;
	char errorType[48];
	char buffer[128];
	HCURSOR oldCursor;


	oldCursor  = SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);

	//we no longer need to display warnings about alias objects
	result = LinkAllAliasObjects(FALSE);

	//FALSE means don't update the status bar
	if(!(result & CHECK_CANCEL))
		result |= CheckSubject(FALSE);

	ShowCursor(FALSE);
	SetCursor(oldCursor);

	if(!(result & CHECK_CANCEL))
		{
		if(apb_BuildCheckErrorMessage(result, errorType, sizeof(errorType)))
			{
			wsprintf(buffer, "There were %s found\nin subject: %s",
				errorType, (LPCSTR) name);

			MessageBox(NULL, buffer, "Subject Results", MB_ICONINFORMATION|
				MB_OK|MB_TASKMODAL);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SelectFunction
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SUBJECT::SelectFunction(
	FUNCINST * pFuncInst)
	{
	pFuncInst->SetSelected(TRUE);
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CheckFunctions
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short SUBJECT::CheckFunctions(
	BOOL updateStatus)
	{
	short	result = CHECK_NOERRORS;

	result |= DoFuncCheck(FUNC_CHECK_UNLINKED, updateStatus);
	if(PROGSYSTEM::TestUninitParamsWarningPref() &&
		!(result & CHECK_CANCEL))
		{
		//the following two lines must always be executed in pairs since
		//the first call sets bits and the second call clears bits.
		MarkUninitializedParams(updateStatus);
		result |= DoFuncCheck(FUNC_CHECK_INBEFOREOUT, updateStatus);
		}

	return result;
	}

///////////////////////////////////////////////////////
short SUBJECT::DoFuncCheck(
	short checkType,
	BOOL updateStatus)
	{
	int            i;
	FUNCINST *  funcPtr;
	short          result = CHECK_NOERRORS;
	short          funcResult = FUNC_CHECK_NOERROR;
	short					 tempResult;
	char           buffer[512];
	int            response;
	LPSUBJECTWINDOW winPtr;
	char					 msgTitle[32];

	i = 0;
	while(0 != (funcPtr = (FUNCINST *)GetObject(this->theFunctions, i)))
		{
		if ((tempResult = funcPtr->CheckFunction(checkType)) != FUNC_CHECK_NOERROR)
			{
			if(funcResult == FUNC_CHECK_NOERROR)
				{
				//unselect everything so only functions with errors will be selected.
				OnlyUnselectAll();
				}

			if(tempResult & checkType)
				{
				funcResult |= (tempResult & checkType);
				this->SelectFunction(funcPtr);
				}
			}
		if(updateStatus)
			PROJECT::IncStatusBar();
		}

	if (funcResult != FUNC_CHECK_NOERROR)
		{
		winPtr = (LPSUBJECTWINDOW)windowPtr;
		if(winPtr)
			{
			winPtr->RebuildSelectedLists();
//			winPtr->GetObjListPtr()->UnselectAll();
			InvalidateRect(winPtr->GetWindowHandle(), NULL, FALSE);
			}
		ShowObjectWindow();
		
		//build messages

		wsprintf(buffer,
		 "Selected functions in the subject named \"%s\"", this->GetName());

		if(funcResult == FUNC_CHECK_UNLINKED)
			{
			lstrcat(buffer, " are missing required parameters.");
			lstrcpy(msgTitle, "Function Error");
			result |= CHECK_ERROR;
			}
		else if(funcResult == FUNC_CHECK_INBEFOREOUT)
			{
			lstrcat(buffer, " might have parameters which will be uninitialized"
				" when the function is called.");
			lstrcpy(msgTitle, "Function Warning");
			result |= CHECK_WARNING;
			}
		else
			I_ASSERT(FALSE);

		lstrcat(buffer, "\n\n\rContinue?");

		MessageBeep(MB_ICONEXCLAMATION);

		response = MessageBox(NULL, buffer, msgTitle,
				 MB_ICONEXCLAMATION|MB_OKCANCEL|MB_TASKMODAL);
		
		if (response == IDOK)
			result |= CHECK_CONTINUE;
		else
			result |= CHECK_CANCEL;//CHECK_CONTINUE means errors while error
																		 //CHECK_CANCEL means stop checking.
		}
	return result;
	}
/////////////////////////////////////////////////////
void SUBJECT::MarkUninitializedParams(
	BOOL updateStatus)
	{
	LPOBJINST objPtr;
	LPFUNCINST funcPtr;
	int i;
	LPFLOWSTACK flowStack = new FLOWSTACK();
	LPVOIDSTACK markedStack = new VOIDSTACK();
	LPFLOW flowPtr;
	
	//first treat objects as master flow chains, then
	//treat funcs with no incoming flows as master chains	
	i=0;
	while(0 != (objPtr = (LPOBJINST)GetObject(theObjects, i)))
		{
		objPtr->PushFlows(flowStack);

		while(NULL != (flowPtr = flowStack->Pop()))
			{
			funcPtr = (LPFUNCINST)GetObject(flowPtr->GetDestinationUID());
			I_ASSERT(funcPtr);
			
			if(!funcPtr->TestChainVisitedFlag())
				{
				funcPtr->MarkUninitParams();
				funcPtr->PushFlows(flowStack);
				funcPtr->SetChainVisitedFlag();
				markedStack->Push(funcPtr);
				}
			}

		while(NULL != (funcPtr = (LPFUNCINST)markedStack->Pop()))
			funcPtr->ClearChainVisitedFlag();
		
		if(updateStatus)
			PROJECT::IncStatusBar();	
		}

	i=0;
	while(0 != (funcPtr = (LPFUNCINST)GetObject(theFunctions, i)))
		{
		if(!funcPtr->TestChainVisitedFlag() && !funcPtr->HasIncomingFlows())
			{
			funcPtr->MarkUninitParams();
			funcPtr->PushFlows(flowStack);
			funcPtr->SetChainVisitedFlag();
			markedStack->Push(funcPtr);
			
			while(NULL != (flowPtr = flowStack->Pop()))
				{
				funcPtr = (LPFUNCINST)GetObject(flowPtr->GetDestinationUID());
				I_ASSERT(funcPtr);
			
				if(!funcPtr->TestChainVisitedFlag())
					{
					funcPtr->MarkUninitParams();
					funcPtr->PushFlows(flowStack);
					funcPtr->SetChainVisitedFlag();
					markedStack->Push(funcPtr);
					}

				}

			while(NULL != (funcPtr = (LPFUNCINST)markedStack->Pop()))
				funcPtr->ClearChainVisitedFlag();
			}

		if(updateStatus)
			PROJECT::IncStatusBar();
		}

	while(flowStack->Pop());
	while(markedStack->Pop());

	delete flowStack;
	delete markedStack;
	}


/////////////////////////////////////////////////////
//activeGroup is optional and defaults to NULLSERUID
void SUBJECT::OnlyUnselectAll(
	SERUID activeGroup)
	{
	DoOnlyUnselect(activeGroup);
	}

void SUBJECT::OnlyUnselectGroups(
	SERUID activeGroup)
	{
	LPOBJGROUP groupPtr;
	int i;

	i=0;
	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		{
		if(activeGroup != GetUID(groupPtr))
    	{
			groupPtr->UnselectAllObjects(NULL);
			if(groupPtr->TestActiveFlag())
				groupPtr->ClearActiveFlag();
			}

		if(groupPtr->IsSelected())
			groupPtr->SetSelected(FALSE);

		}
 }

void SUBJECT::DoOnlyUnselect(
	SERUID activeGroup)
	{
	OBJINST * objPtr;
	FUNCWINOBJ * fwinObjPtr;
	int i;

	i=0;
	while(0!=(fwinObjPtr = (FUNCWINOBJ *)GetObject(theFunctions, i)))
		{
		((FUNCINST *)fwinObjPtr)->UnselectParams();
		((FUNCINST *)fwinObjPtr)->UnselectFlows();
		if(fwinObjPtr->IsSelected())
			fwinObjPtr->SetSelected(FALSE);
		}

	i=0;
	while(0!=(fwinObjPtr = (FUNCWINOBJ *)GetObject(theComments, i)))
		{
		if(fwinObjPtr->IsSelected())
			fwinObjPtr->SetSelected(FALSE);
		}


	i=0;
	while(0!=(objPtr = (LPOBJINST)GetObject(theObjects, i)))
		{
		objPtr->UnselectFlows();
		}

	OnlyUnselectGroups(activeGroup);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CheckObjects
//
// CLASS.......:  SUBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short SUBJECT::CheckObjects(
	BOOL updateStatus,
	BOOL selectedOnlyFlag,
	BOOL displayResults)
	{
	int        i;
	LPOBJINST  pObjInst;
	short      result = CHECK_NOERRORS;
	char 				shortMessage[48];
	char				longMessage[128];

  // check each design time object in the subject

  i = 0;
	result |= LinkAllAliasObjects(FALSE);
	while (0 != (pObjInst = (LPOBJINST)GetObject(theObjects, i)))
		{
		pObjInst->ClearCompiledFlag();
		//we need to check an objinst even if its an alias since we still
		//nee to check the signals.
		if (!selectedOnlyFlag || pObjInst->TestSelectFlag())
			{
			result|= pObjInst->CheckObject();
			if (result & CHECK_CANCEL)
				break;
			}
		if(updateStatus)
			PROJECT::IncStatusBar();
		}

	if(displayResults && (!(result & CHECK_CANCEL)))
		{
		if(apb_BuildCheckErrorMessage(result, shortMessage, sizeof(shortMessage)))
			{
			wsprintf(longMessage, "%s were found in the selected objects.", shortMessage);

			MessageBox(NULL, longMessage, "Check Object Result", 
				MB_OK | MB_TASKMODAL | MB_ICONINFORMATION);
			}
		}

	return result;
	}

//adjusts the log bounds to cover all objects and makes sure the objects fit within the
//log bounds.
//For all stuff defaults to FALSE.
BOOL SUBJECT::CalculateLogBounds(
	BOOL forAllStuff)
	{
	FUNCWINOBJ *  fwinObjPtr;
	int              i;
	RECT             bounds;
	POINT            location;
	POINT            holdLocation;
	int              j;
	RUNTIMEUID       objectBlock;
	BOOL             boundsChanged = FALSE;

	for(j=0;j<3;j++)
		{
		if(j==0)
			objectBlock = theFunctions;
		else if(j==1)
			objectBlock = theComments;
		else
    	objectBlock = theObjGroups;

		i=0;
		while(0!=(fwinObjPtr = (FUNCWINOBJ *)GetObject(objectBlock, i)))
			{
			if(fwinObjPtr->IsSelected() || forAllStuff)
				{
				location = fwinObjPtr->GetLocation();
				holdLocation = location;
				fwinObjPtr->GetRect(&bounds);
				if(bounds.left < logBounds.left)
					{
					boundsChanged = TRUE;
					logBounds.left = bounds.left;
					if(logBounds.left < 0)
						{
						location.x-=logBounds.left; //logBounds.left is negative here
						logBounds.left = 0;
						}
					}
				else if(bounds.right > logBounds.right)
					{
					boundsChanged = TRUE;
					logBounds.right = bounds.right;
					if(logBounds.right > XMAXPAGE)
						{
						location.x-=(logBounds.right - XMAXPAGE);
						logBounds.right = XMAXPAGE;
						}
					}

				if(bounds.top < logBounds.top)
					{
					boundsChanged = TRUE;
					logBounds.top = bounds.top;
					if(logBounds.top < 0)
						{
						location.y-=logBounds.top; //logBounds.top is negative here
						logBounds.top = 0;
						}
					}
				else if(bounds.bottom > logBounds.bottom)
					{
					boundsChanged = TRUE;
					logBounds.bottom = bounds.bottom;
					if(logBounds.bottom > YMAXPAGE)
						{
						location.y-=(logBounds.bottom - YMAXPAGE);
						logBounds.bottom = YMAXPAGE;
						}
					}
				if(holdLocation.x != location.x || holdLocation.y != location.y)
					{
					fwinObjPtr->SetLocation(location);
					}
				}
			}
		}
	return boundsChanged;
	}

void SUBJECT::CenterSelection()
	{
 	if(windowPtr)
		((LPSUBJECTWINDOW)windowPtr)->CenterSelection();
	}

void SUBJECT::SelectAll()
	{
	BASEWINDOW * winPtr = windowPtr;
	if(winPtr)
		{
		((LPSUBJECTWINDOW)winPtr)->SelectAllWorksheetItems();
		}
	}


//only check for out of disk space after exporting an object, function or
//comment and when exporting the last field of the subject
WORD SUBJECT::ExportUPSF()
	{
	BOOL tempBool;
	int i;
	LPOBJINST objPtr;
	int exportCount;
	FUNCINST * funcPtr;
	COMMENT * comPtr;
	WORD returnValue = EXPORT_NOERROR;
	long result;
	LPIDINDEX node;
	LPOBJGROUP groupPtr;

	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_NAME, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
		(LPVOID)GetName(), (long)lstrlen(GetName())) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	if(!TestIconicFlag())
		tempBool = TRUE;
	else
		tempBool = FALSE;
	
	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_VISIBLE, 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_POSITION, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&positionInProject.x, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_POSITION, 1L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&positionInProject.y, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	if(IsGridOn())
		tempBool = TRUE;
	else
		tempBool = FALSE;

	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_GRID, 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	if(TestSignalNamesFlag())
		tempBool = TRUE;
	else
		tempBool = FALSE;

	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_SHOWSIGNALS, 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	//the func tree must be build before exporting any objects or
	//functions because flow and parameters depend on the tree being
	//built.
	I_ASSERT(!SUBJECT::funcTree);

	SUBJECT::funcTree = new BITREE();
	
	I_ASSERT(SUBJECT::funcTree);

	i=0;
	while(0!=(funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		node = new IDINDEX(GetUID(funcPtr).id, i-1);

		I_ASSERT(node);

		SUBJECT::funcTree->Add(node);
		}


	exportCount = 0; 
	//export the objects
	if(nObjectsCount > 0)
		{
		//Allocate binary tree to hold all obj id's
		I_ASSERT(!SUBJECT::objTree);
		
		SUBJECT::objTree = new BITREE();

		I_ASSERT(SUBJECT::objTree);

		//build list associating an objects uid with its index
		//The list can be built concurrently with the exporting
		//of the objects since the objects don't need the list.
		i=0;
		while((!(returnValue & EXPORT_OUTOFDISK)) && 
			(0!=(objPtr = (LPOBJINST)GetObject(theObjects, i))))
			{
			node = new IDINDEX(GetUID(objPtr).id, i-1);

			I_ASSERT(node);

			SUBJECT::objTree->Add(node);
		
			if(AUpsfCreateScope(SUBJECT_OBJECTDATA, (long)i-1) == AUPSF_NOERROR)
				{
				exportCount++;
						
				returnValue |= objPtr->ExportUPSF();
				AUpsfCloseScope();
				}
			else
				{
				returnValue|=EXPORT_GENERALERROR;
				I_PRINT("Error creating object scope");
				}
			}
		}

	//export the number of objects exported
	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_OBJECTCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	//export the functions - Make sure the objTree and funcTree is build
	//before exporting any functions.

	i=0;
	exportCount = 0;
	while((0!=(funcPtr = (FUNCINST *)GetObject(theFunctions, i))) &&
		!(returnValue & EXPORT_OUTOFDISK))
		{
		if(AUpsfCreateScope(SUBJECT_FUNCTIONDATA, (long)(i-1)) == AUPSF_NOERROR)
			{
			exportCount++;
			returnValue |= funcPtr->ExportUPSF();
			AUpsfCloseScope();
			}
		else
			{
			returnValue|=EXPORT_GENERALERROR;
			I_PRINT("Error creating functions scope.");
			}
		}

	//export the number of functions exported
	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_FUNCTIONCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);


	//export the groups
	i=0;
	exportCount = 0;
	while(0!=(groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)) &&
		!(returnValue & EXPORT_OUTOFDISK))
		{
		if(AUpsfCreateScope(SUBJECT_GROUPDATA, (long)(i-1)) == AUPSF_NOERROR)
			{
			exportCount++;
			returnValue |= groupPtr->ExportUPSF();
			AUpsfCloseScope();
			}
		else
			{
			returnValue|=EXPORT_GENERALERROR;
			I_PRINT("Error creating group scope.");
			}
		}

	//ALSO - do a final check to see if we've run out of disk space
	I_ALWAYSASSERT(AUpsfExportField(SUBJECT_GROUPCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue |= EXPORT_GENERALERROR);


	//export the comments
	i=0;
	exportCount = 0;
	while(0!=(comPtr = (COMMENT *)GetObject(theComments, i)) &&
		!(returnValue & EXPORT_OUTOFDISK))
		{
		if(AUpsfCreateScope(SUBJECT_COMMENTDATA, (long)(i-1)) == AUPSF_NOERROR)
			{
			exportCount++;
			returnValue |= comPtr->ExportUPSF();
			AUpsfCloseScope();
			}
		else
			{
			returnValue|=EXPORT_GENERALERROR;
			I_PRINT("Error creating group scope.");
			}
		}

	//export the number of Comments exported
	//ALSO - do a final check to see if we've run out of disk space
	result = AUpsfExportField(SUBJECT_COMMENTCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int));

	I_ASSERT(result == AUPSF_NOERROR);
	
	if(result == AUPSF_OUTOFDISKSPACE)
		returnValue |= EXPORT_OUTOFDISK;
	else if(result !=AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;


	//cleanup and initialize everything to empty
	if(SUBJECT::objTree)
		{
		delete SUBJECT::objTree;
		SUBJECT::objTree = NULL;
		}

	if(SUBJECT::funcTree)
		{
		delete SUBJECT::funcTree;
		SUBJECT::funcTree = NULL;
		}

	PROJECT::IncStatusBar();

	return returnValue;
	}

BOOL SUBJECT::ImportUPSF()
	{
	char buffer[128];
	char buffer2[192];
	char extBuffer[8];
	long result;
	BOOL tempBool;
	POINT tempPoint;
	BOOL noError;

	//clear the registered uid's table
	upsfClearObjectIDs();

	//read in as much of the subject name as will fit - hopefully all of it.
	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_NAME, 0L, AUPSF_NATV_STRING,
		(LPVOID)buffer, (long)sizeof(buffer))) != AUPSF_MEMORYERROR, return FALSE);
			
	if(UPSF_GOODSTRINGRESULT(result))
		{
		SetName(buffer);
		MDI::GetNavigatorPtr()->
			SetItemName(GetUID(this), GetParentProject()->fileObjPtr, buffer);
		}
	else
		{
		LoadString(PROGSYSTEM::hInst, STR_SUBJECTNAME, (LPSTR)buffer, sizeof(buffer));
		//Don't worry about creating a default name which is a duplicate of an
		//existing name for the following reasons:
		// (1)	Default subject names are different on the Mac anyway
		// (2) 	It doesn't make sense to check for duplicates before all subjects
		//			are even created.
		// (3)	It would actually be unusual to even use a default name because
		//			that would mean some sort of error occured while reading in the
		//			subject name.
		itoa(SUBJECT::GetIndex(), extBuffer, 10);
		lstrcat(buffer, (LPSTR)extBuffer);
		SUBJECT::IncrementIndex();
 		SetName(buffer);
		apb_LogUpsfError(result, "subject name so using default name");
		}

	wsprintf(buffer2, "%s Importing subject: %s", UPSF_IMPORTPREFIXSTRING, 
		buffer);
		 
	AUpsfLogComment(buffer2);

	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_VISIBLE, 0L, AUPSF_NATV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL))) != AUPSF_MEMORYERROR,
		return FALSE);
	
	if(result == AUPSF_NOERROR)
		{
		if(tempBool>0)
			ClearIconicFlag();
		else
			SetIconicFlag();
		}
	else if(result != AUPSF_MEMORYERROR)
		{
		apb_LogUpsfError(result, "visible field");
		ClearIconicFlag(); //default being iconicized in project window
		}

	//defautls should have already been set up in the SUBJECT constructor

	//postionInProject should already have been set to default
	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_POSITION, 0L, AUPSF_NATV_INT,
		(LPVOID)&tempPoint.x, (long)sizeof(int))) != AUPSF_MEMORYERROR,
		return FALSE);

	if(result == AUPSF_NOERROR)
		{
		I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_POSITION, 1L, AUPSF_NATV_INT,
			(LPVOID)&tempPoint.y, (long)sizeof(int))) != AUPSF_MEMORYERROR,
			return FALSE);

		if(result == AUPSF_NOERROR)
			{
			positionInProject = tempPoint;
			GetParentProject()->AdjustLastSubjectPosition();
			}
		}

	if(result != AUPSF_NOERROR)
		apb_LogUpsfError(result, "position");

	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_GRID, 0L, AUPSF_NATV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL))) != AUPSF_MEMORYERROR,
		return FALSE);
	
	if(result == AUPSF_NOERROR)
		{
		if(tempBool > 0)
			SetGrid(TRUE);
		else
			SetGrid(FALSE);
		}
	else
		apb_LogUpsfError(result, "grid field");

	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_SHOWSIGNALS, 0L, AUPSF_NATV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL))) != AUPSF_MEMORYERROR,
		return FALSE);

	if(result == AUPSF_NOERROR)
		{
		if(tempBool > 0)
			SetSignalNamesFlag();
		else
			ClearSignalNamesFlag();
		}
	else
		apb_LogUpsfError(result, "show signals field");


	//initialize uid arrays to null
	objArray = NULL;
	funcArray = NULL;
	sizeObjArray = sizeFuncArray = 0;


	noError = MakeUPSFObjects();

	if(noError)
		noError=MakeUPSFFunctions();


	//Objects must be imported before functions since the parameters are
	//imported when the functions are imported and output parameters may need
	//to get their names from the objects.

	if(noError)
		noError = ImportUPSFObjects();

	if(noError)
		noError = ImportUPSFFunctions();

	//functions must be imported before groups because if a default
	//big group is imported, it will determine its location based
	//on the function locations.
	if(noError)
		noError = ImportUPSFGroups();

	if(noError)
		noError = ImportUPSFComments();

	//clean up
	if(objArray)
		GlobalFree((HGLOBAL) objArray);
	sizeObjArray = 0;
	objArray = NULL;

	if(funcArray)
		GlobalFree((HGLOBAL) funcArray);
	sizeFuncArray=0;
	funcArray = NULL;

	return noError;
	}

BOOL SUBJECT::MakeUPSFObjects()
	{
	int i;
	LPOBJDESC objDescPtr;
	LPOBJINST objPtr;
	char buffer2[192];
	char buffer[64];
	char typeBuffer[8];
	long result;
	long tempTypeID;
	long tempID;
	int tempInt;
	BOOL isAlias;
	BOOL unknownTypeName;

	//only check against memory error in first assert because we need
	//to log a comment if getting the count field fails for some other
	//reason.
	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_OBJECTCOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&sizeObjArray, (long)sizeof(int))) !=
		AUPSF_MEMORYERROR, return FALSE);

	I_ALWAYSASSERT(result == AUPSF_NOERROR, 
		{apb_LogUpsfError(result, "object count", APB_UPSFFATALERROR);
		return FALSE;});

	if(PROJECT::UpsfPreAppwareImportVersion())
		sizeObjArray++;	//used to be 0 based count.

	I_ASSERT(sizeObjArray >= 0); //shouldn't be negative.

	if(sizeObjArray > 0)
		{
		objArray = (LPSERUID) GlobalAlloc(GPTR, sizeof(SERUID) * sizeObjArray);
		
		I_ALWAYSASSERT(objArray != NULL, return FALSE);

		for(i=0;i<sizeObjArray;i++)
			{
			objArray[i]=NULLSERUID; //initialize to null
			
			I_ALWAYSASSERT((result=AUpsfOpenScope(SUBJECT_OBJECTDATA, (long)i)) !=
				AUPSF_FILEERROR, return FALSE);

			if(result == AUPSF_NOERROR)
				{
				PROJECT::PushScopeStack(ST_QUARTER);
				
				I_ALWAYSASSERT((result = AUpsfImportField(OBJECT_TYPE, 0L, AUPSF_NATV_LONG,
						(LPVOID)&tempTypeID, (long)sizeof(long))) != AUPSF_MEMORYERROR,
						return FALSE);
				
				objDescPtr = cfgGetObjectDesc(tempTypeID);
				
				if(objDescPtr == 0)
					{
					unknownTypeName = FALSE;

					//get name of object type and log comment
					if(PROJECT::UpsfPreAppwareImportVersion())
						{
						I_ALWAYSASSERT((result = AUpsfImportField((LPSTR)"ObjTypeN", 0L, AUPSF_NATV_STRING,
							(LPVOID)buffer, (long)sizeof(buffer))) != AUPSF_MEMORYERROR, 
							return FALSE);
							
						if(UPSF_BADSTRINGRESULT(result))
							unknownTypeName = TRUE;
						}
					else
						{
						if(!PROJECT::GetUpsfObjTypeName(tempTypeID, buffer, sizeof(buffer)))
							unknownTypeName = TRUE;
						}

					if(unknownTypeName)
						lstrcpy(buffer, "?"); 

					MAKE_TYPEID_READABLE(typeBuffer, tempTypeID);

					wsprintf(buffer2, "Could not find object with ATYPEID: %s and TYPE"
						" NAME: %s in your configuration files",
						typeBuffer, buffer);

					AUpsfLogComment(buffer2);
					}	 //if(objDescPtr == 0)
				else
					{
					//determine whether or not object is as alias
					isAlias = FALSE;

					I_ALWAYSASSERT((result = AUpsfImportField(OBJECT_ALIAS, 0L, AUPSF_NATV_INT,
						(LPVOID)&tempInt, (long)sizeof(int))) != AUPSF_MEMORYERROR,
						return FALSE);

					if(result == AUPSF_NOERROR)
						{
						if(tempInt == 2)
							isAlias = TRUE;
						}

					objPtr = NewObjInst(objDescPtr->GetUID(), isAlias, NULLSERUID);

					if(!objPtr)
						{
						wsprintf(buffer2, "Could not create object of type %s.",
							objDescPtr->pGetName());

						AUpsfLogComment(buffer2);
						}
					else
						{
						objArray[i]=GetUID(objPtr);
					
						nObjectsCount++;
					
						GetParentProject()->IncObjectsCount();
					
						if(tempInt == 1)
							objPtr->SetShareFlag();
					
						//read in oldID
						I_ALWAYSASSERT((result = AUpsfImportField(OBJECT_ID, 0L, AUPSF_NATV_LONG,
							(LPVOID)&tempID, (long)sizeof(long))) != AUPSF_MEMORYERROR,
							return FALSE);
						
						if(result == AUPSF_NOERROR)
							upsfRegisterObjectID(GetUID(objPtr).id, tempID);
						else
							apb_LogUpsfError(result, "object's old UID");
						}
					}
				PROJECT::PopScopeStack();
				AUpsfCloseScope();
				}
			else //testing Open scope result
				{
				apb_LogUpsfError(result, "object scope", APB_UPSFOPEN);
				}

			}	//end for loop
		}

	return TRUE;	//no error if we reached here.
	}


BOOL SUBJECT::MakeUPSFFunctions()
	{
	long result;
	int i;
	LPAFUNCDESC funcDescPtr;
	long tempTypeID;
	char buffer[64];
//	char msgBuffer[128];
	char msgBuffer[256];
	char typeBuf[8];
	POINT tempLocation;
	FUNCINST * funcPtr;
	POINT holdLocation;
	BOOL badName;

	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_FUNCTIONCOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&sizeFuncArray, (long)sizeof(int))) !=
		AUPSF_MEMORYERROR, return FALSE);

	I_ALWAYSASSERT(result == AUPSF_NOERROR, 
		{apb_LogUpsfError(result, "function count", APB_UPSFFATALERROR);
		return FALSE;});
	
	if(PROJECT::UpsfPreAppwareImportVersion())
		sizeFuncArray++;	//used to be 0 based count.

	I_ASSERT(sizeFuncArray >= 0); //shouldn't be negative.

	if(sizeFuncArray > 0)
		{
		funcArray = (LPSERUID) GlobalAlloc(GPTR, sizeof(SERUID) * sizeFuncArray);
		
		I_ALWAYSASSERT(funcArray != NULL, return FALSE);
		
		holdLocation.x = originOffset.x;
		holdLocation.y = originOffset.y;
		
		for(i=0;i<sizeFuncArray;i++)
			{
			funcArray[i]=NULLSERUID; //initialize to null
			
			I_ALWAYSASSERT((result = AUpsfOpenScope(SUBJECT_FUNCTIONDATA, (long)i))
				!= AUPSF_FILEERROR, return FALSE);
			
			if(result == AUPSF_NOERROR)
				{
				PROJECT::PushScopeStack(ST_QUARTER);
				
				//create default initialized functions
				I_ALWAYSASSERT((result = AUpsfImportField(FUNCTION_TYPE, 0L, AUPSF_NATV_LONG,
					(LPVOID)&tempTypeID, (long)sizeof(long))) != AUPSF_MEMORYERROR,
					return FALSE);
				
				if(result == AUPSF_NOERROR)
					{
					funcDescPtr = cfgGetFunctionDesc(tempTypeID);
					
					if(funcDescPtr == 0)
						{
						//get name of function type and log comment
						badName = FALSE;

						if(PROJECT::UpsfPreAppwareImportVersion())
							{		
							I_ALWAYSASSERT((result = AUpsfImportField((LPSTR)"FcnTypeN", 0L, AUPSF_NATV_STRING,
								(LPVOID)buffer, (long)sizeof(buffer))) != AUPSF_MEMORYERROR,
								return FALSE);
								
							if(result != AUPSF_NOERROR)
								badName = TRUE;

							}
						else
							{
							if(!PROJECT::GetUpsfFuncTypeName(tempTypeID, buffer, sizeof(buffer)))
								badName = TRUE;
							}

						MAKE_TYPEID_READABLE(typeBuf, tempTypeID);
						
						if(badName)
							lstrcpy(buffer, "?");

						wsprintf(msgBuffer, "Could not find function with ATYPEID: %s	and "
							"TYPE NAME: %s in your configuration files",
							typeBuf, buffer);

						AUpsfLogComment(msgBuffer);

						}	//funcDescPtr == 0
					else
						{

						//get new location
						I_ALWAYSASSERT((result = AUpsfImportField(FUNCTION_LOCATION, 0L, AUPSF_NATV_INT,
							(LPVOID)&tempLocation.x, (long)sizeof(int))) != AUPSF_MEMORYERROR,
							return FALSE);

						if(result == AUPSF_NOERROR)
							{
							I_ALWAYSASSERT((result = AUpsfImportField(FUNCTION_LOCATION, 1L, AUPSF_NATV_INT,
								(LPVOID)&tempLocation.y, (long)sizeof(int))) !=
								AUPSF_MEMORYERROR, return FALSE);
							}

						if(result != AUPSF_NOERROR)
							{
							apb_LogUpsfError(result, "function location so using default location");
							holdLocation.x+=SJT_DEFAULTFUNCSPACE;
							holdLocation.y+=SJT_DEFAULTFUNCSPACE;
							tempLocation=holdLocation;
							}

						//create default function
						funcPtr = new(theFunctions)
							FUNCINST(GetUID(this), tempTypeID, tempLocation);
						
						I_ASSERT(funcPtr);

						funcArray[i]=GetUID(funcPtr);
						}
					
					} //testing import of funcType
				else
					apb_LogUpsfError(result, "function typeID");

				PROJECT::PopScopeStack();
				AUpsfCloseScope();
				}	//test open scope
			else
				apb_LogUpsfError(result, "function data scope", APB_UPSFOPEN);

			} //for loop
		} //sizeFuncArray > 0
	return TRUE; //signal no error
	}


BOOL SUBJECT::ImportUPSFObjects()
	{
	int i;
	LPOBJINST objPtr;
	long result;
	BOOL importResult;

	for(i=0;i<sizeObjArray;i++)
		{
		if(objArray[i] != NULLSERUID)
			{
			I_ALWAYSASSERT((objPtr = (LPOBJINST)GetObject(objArray[i])) != NULL,
				return FALSE);

			I_ALWAYSASSERT((result = AUpsfOpenScope(SUBJECT_OBJECTDATA, (long)i)) !=
				AUPSF_FILEERROR, return FALSE);
			
			if(result == AUPSF_NOERROR)
				{
				PROJECT::PushScopeStack(ST_3QUARTER);
				importResult = objPtr->ImportUPSF();
				PROJECT::PopScopeStack();
				AUpsfCloseScope();
				if(!importResult)
					return FALSE;
				}
			else
				//We are not likely to reach this code since the scope must have been
				//successfully opened once before to even create the object so we
				//don't need to log a very informative message.
				apb_LogUpsfError(result, "information for an object so using default values");
			}
		}
	return TRUE;	//no error if we made it to here.
	}

BOOL SUBJECT::ImportUPSFFunctions()
	{
	int i;
	FUNCINST * funcPtr;
	long result;
	BOOL importResult;

	for(i=0;i<sizeFuncArray;i++)
		{
		if(funcArray[i] != NULLSERUID)
			{
			funcPtr = (FUNCINST *)GetObject(funcArray[i]);
			
			I_ASSERT(funcPtr);

			I_ALWAYSASSERT((result = AUpsfOpenScope(SUBJECT_FUNCTIONDATA, (long)i))
				!= AUPSF_MEMORYERROR, return FALSE);
			
			if(result == AUPSF_NOERROR)
				{
				PROJECT::PushScopeStack(ST_3QUARTER);
				importResult = funcPtr->ImportUPSF();
				
				PROJECT::PopScopeStack();
				AUpsfCloseScope();
				
				if(!importResult)
					return FALSE;
				}
			else
				//We are not likely to reach this code since the scope must have been
				//successfully opened once before to even create the function so we
				//don't need to log a very informative message.
				apb_LogUpsfError(result, "function scope so using"
					" default function information");

			}
		}

	return TRUE; //no error if we reach here
	}


BOOL SUBJECT::ImportUPSFGroups()
	{
	long result;
	int i;
	int groupCount;
	POINT tempLocation;
	POINT holdLocation;
	BOOL createBigGroup = FALSE;
	LPOBJGROUP groupPtr;
	char msgBuffer[128];

	//offset y location so default won't default to same space as functions or
	//groups.
	holdLocation.x = originOffset.x ;
	holdLocation.y = originOffset.y + 2 * SJT_DEFAULTFUNCSPACE;

	//if we're importing a workshop file, then just create 1 big object
	//group, otherwise, import all the groups.
	if(!PROJECT::UpsfPreAppwareImportVersion())
		{
		I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_GROUPCOUNT, 0L,
			AUPSF_NATV_INT, (LPVOID)&groupCount, (long)sizeof(int))) != 
			AUPSF_MEMORYERROR, return FALSE);

		if(result != AUPSF_NOERROR)
			{
			apb_LogUpsfError(result, "subject's group count so creating 1 big group");
			groupCount = 0;
			createBigGroup = TRUE;
			}

		for(i=0;i<groupCount;i++)
			{
			I_ALWAYSASSERT((result = AUpsfOpenScope(SUBJECT_GROUPDATA, (long)i)) !=
				AUPSF_FILEERROR, return FALSE);
			
			if(result == AUPSF_NOERROR)
				{
				PROJECT::PushScopeStack(ST_FULL);
				
				//get new location
				I_ALWAYSASSERT((result = AUpsfImportField(GROUP_LOCATION, 0L, 
					AUPSF_NATV_INT, (LPVOID)&tempLocation.x, (long)sizeof(int))) !=
					AUPSF_MEMORYERROR, return FALSE);
			
				if(result == AUPSF_NOERROR)
					{
					I_ALWAYSASSERT((result = AUpsfImportField(GROUP_LOCATION, 1L, 
						AUPSF_NATV_INT, (LPVOID)&tempLocation.y, (long)sizeof(int))) !=
						AUPSF_MEMORYERROR, return FALSE);
					}

				if(result != AUPSF_NOERROR)
					{
					apb_LogUpsfError(result, "comment location so using"
						" default location");

					tempLocation = holdLocation;

					holdLocation.x+=SJT_DEFAULTFUNCSPACE;
					holdLocation.y+=SJT_DEFAULTFUNCSPACE;
					}

				//create a default group
				groupPtr = new(theObjGroups) OBJGROUP(NULLSERUID, tempLocation, GetUID(this), 
					SJT_DEFAULTGROUPNAME);	
				
				if(!groupPtr->ImportUPSF())
					return FALSE;
				else
					{
					//delete the group if there are no objects
					if(groupPtr->GetObjCount() == 0)
						{
						wsprintf(msgBuffer, "Deleting group: %s because it contained "
							"zero objects.", groupPtr->GetName());

						AUpsfLogComment(msgBuffer);
						delete groupPtr;
						}
					}
				PROJECT::PopScopeStack();
				AUpsfCloseScope();
				
				}//if the scope is good)

			}//for loop

		}
	else	
		createBigGroup = TRUE;

	if(createBigGroup && !CreateBigDefaultGroup(holdLocation))
		return FALSE;

	return TRUE; //no error if made it to here
	}

//location is the desired location but make sure
//it's not too  away from the functions.
BOOL SUBJECT::CreateBigDefaultGroup(
	POINT location)
	{
	char nameBuffer[PRJ_NEWGROUPMAXNAME];
	LPOBJGROUP groupPtr;
	int i=0;
	LPFUNCINST funcPtr;
	RECT funcRect;
	RECT unionRect;
	BOOL rval = TRUE;

	//get union of all function rectangles
	SetRectEmpty(&unionRect);
	while(0 != (funcPtr = (LPFUNCINST)GetObject(theFunctions, i)))
		{
		funcPtr->GetRect(&funcRect);
		UnionRect(&unionRect, &unionRect, &funcRect);
		}

	if(((LPRECORDBLOCK)GetObject(theObjects))->GetCount() > 0)
		{
		GetParentProject()->GetNewGroupName(nameBuffer);

		groupPtr = new(theObjGroups) OBJGROUP(NULLSERUID, location, GetUID(this), 
			nameBuffer);

		I_ASSERT(groupPtr);
			
		//now insert all objects of the subject into the group.	

		rval = groupPtr->InsertAllBlockObjects(theObjects);

		if(!IsRectEmpty(&unionRect))
			groupPtr->MoveNearRectangle(&unionRect);
		
		}
	else
		AUpsfLogComment("Couldn't create big group because subject contained "
			"zero objects.");
				
	return rval;
	}

BOOL SUBJECT::ImportUPSFComments()
	{
	long result;
	int i;
	POINT tempLocation;
	COMMENT * comPtr;
	int commentCount;
	POINT holdLocation;
	BOOL importResult;

	//offset x location so default won't default to same space as functions or
	//groups.
	holdLocation.x = originOffset.x + 4 * SJT_DEFAULTFUNCSPACE;
	holdLocation.y = originOffset.y;
	
	I_ALWAYSASSERT((result = AUpsfImportField(SUBJECT_COMMENTCOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&commentCount, (long)sizeof(int))) != 
		AUPSF_MEMORYERROR, return FALSE);

	if(result != AUPSF_NOERROR)
		{
		apb_LogUpsfError(result, "subject's comment count");
		commentCount = 0;
		}

	for(i=0;i<commentCount;i++)
		{
		I_ALWAYSASSERT((result = AUpsfOpenScope(SUBJECT_COMMENTDATA, (long)i)) !=
			AUPSF_FILEERROR, return FALSE);
		
		if(result == AUPSF_NOERROR)
			{
			PROJECT::PushScopeStack(ST_FULL);
			
			//get new location
			I_ALWAYSASSERT((result = AUpsfImportField(COMMENT_LOCATION, 0L, 
				AUPSF_NATV_INT, (LPVOID)&tempLocation.x, (long)sizeof(int))) !=
				AUPSF_MEMORYERROR, return FALSE);
		
			if(result == AUPSF_NOERROR)
				{
				I_ALWAYSASSERT((result = AUpsfImportField(COMMENT_LOCATION, 1L, 
					AUPSF_NATV_INT, (LPVOID)&tempLocation.y, (long)sizeof(int))) !=
					AUPSF_MEMORYERROR, return FALSE);
				}

			if(result != AUPSF_NOERROR)
				{
				apb_LogUpsfError(result, "comment location so using"
					" default location");

				tempLocation = holdLocation;

				holdLocation.x+=SJT_DEFAULTFUNCSPACE;
				holdLocation.y+=SJT_DEFAULTFUNCSPACE;
				}

			//create default comment
			comPtr = (COMMENT *)GetObject(AddComment(tempLocation,
				(LPSTR)SJT_DEFAULTCOMMENT, (LPSTR)""));

			importResult = comPtr->ImportUPSF();

			PROJECT::PopScopeStack();
			AUpsfCloseScope();

			if(!importResult)
				return FALSE;
			}
		else
			apb_LogUpsfError(result, "comment's data scope", APB_UPSFOPEN);
		}

	return TRUE; //signal no error
	}


BOOL SUBJECT::PostUPSFImport()
	{
	int i;
	LPOBJINST objPtr;
	FUNCINST * funcPtr;

	//There may be cases where the environment is holding onto bogus signal
	//values and the objects will need to change these - do this before calling
	//the objects postUPSFImport routines which simply validate the flows.
	CheckObjectVersions(SUB_SEND_READ);	//don't send awakened
	
	//must be called before calling the object's or function's post import
	//routines.
	LinkAllAliasObjects(FALSE);

	//we need to call the flow post import routines so the flows can properly
	//determine their names and calculate their bounds.
	i=0;
	while(0!=(objPtr = (LPOBJINST)GetObject(theObjects, i)))
		if(!objPtr->PostUPSFImport())
			return FALSE;

	i=0;
	while(0!=(funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		if(!funcPtr->PostUPSFImport())
			return FALSE;


	UPSFAdjustOriginOffset();
	CalculateAllFlowPaths();

	return TRUE;
	}

int SUBJECT::UPSFFuncToIndex(
	SERUID funcUID)
	{
	LPIDINDEX node;

	I_ASSERT(funcTree);

	if(funcTree)
		{
		node = (LPIDINDEX)funcTree->FindNode((long)funcUID.id);

		I_ASSERT(node);

		if(node)
			return node->m_index;
		}

	return SJT_UPSFINDEXERROR;
	}

SERUID SUBJECT::UPSFIndexToFunc(
	int funcIndex)
	{
	if(funcArray && funcIndex >=0 && funcIndex < sizeFuncArray)
		return funcArray[funcIndex];
	else
		return NULLSERUID;
	}

int SUBJECT::UPSFObjToIndex(
	SERUID objUID)
	{
	LPIDINDEX node;

	I_ASSERT(objTree);

	if(objTree)
		{
		node = (LPIDINDEX)objTree->FindNode((long)objUID.id);

		I_ASSERT(node);

		if(node)
			return node->m_index;
		}

	return SJT_UPSFINDEXERROR;
	}

SERUID SUBJECT::UPSFIndexToObj(
	int objIndex)
	{
	if(objArray && objIndex >=0 && objIndex < sizeObjArray)
		return objArray[objIndex];
	else
		return NULLSERUID;
	}

void SUBJECT::UPSFAdjustOriginOffset()
	{
	FUNCINST * funcPtr;
	int i;
	RECT bounds;
	RECT funcRect;
	COMMENT * commentPtr;
	LPOBJGROUP groupPtr;
	int x;

	SetRectEmpty(&bounds);
	i=0;
	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		{
		groupPtr->GetRect(&funcRect);
		UnionRect(&bounds, &bounds, &funcRect);
		}
	
	i=0;
	while(0!=(funcPtr = (FUNCINST *)GetObject(theFunctions, i)))
		{
		funcPtr->GetRect(&funcRect);
		UnionRect(&bounds, &bounds, &funcRect);
		}

	i=0;
	while(0!=(commentPtr = (COMMENT *)GetObject(theComments, i)))
		{
		commentPtr->GetRect(&funcRect);
		UnionRect(&bounds, &bounds, &funcRect);
		}

	if(bounds.left != 0 || bounds.right != 0 || bounds.top != 0
		|| bounds.bottom != 0)
		{
		//offset left corner so stuff isn't so jammed.
		bounds.left -= FUNCSIZE;
		bounds.top -= FUNCSIZE;

		//fit logbounds around functions;
		logBounds = bounds;

		if(logBounds.left <0)
			logBounds.left = 0;

		originOffset.x = bounds.left;

		x=GetSystemMetrics(SM_CXSCREEN);

		logBounds.right+=x;
	
		if(logBounds.right < logBounds.left + x)
			logBounds.right = logBounds.left + 2 * x;

		if(logBounds.right > XMAXPAGE)
			logBounds.right = XMAXPAGE;

		if(logBounds.top < 0)
			logBounds.top = 0;

		originOffset.y = bounds.top;
		
		if(originOffset.y < 0)
			originOffset.y = 0;
		
		x=GetSystemMetrics(SM_CYSCREEN);

		logBounds.bottom+=x;
		
		if(logBounds.bottom < logBounds.top + x)
			logBounds.bottom = logBounds.top + 2 * x;
		
		if(logBounds.bottom > YMAXPAGE)
			logBounds.bottom = YMAXPAGE;
																				 
		//now calculate the log bounds to make sure that all stuff fits the
		//log bounds in case the log bounds was adjusted.
		CalculateLogBounds(TRUE);
		}
	}

//----------------------------------------------------------------------
//   SUBJECT::RenameAliases
//
// Renames all aliases (in this subject) of 'theMaster' with the
// specified 'newName'.
//----------------------------------------------------------------------
BOOL SUBJECT::RenameAliases(
	LPOBJINST  theMaster,
	LPCSTR      newName)
	{
	int       i;
	LPOBJINST objectNow;
	HDC hdc;

	if(windowPtr)
		{
		hdc = GetDC(windowPtr->GetWindowHandle());
		}
	else
  	hdc = NULL;


	i = 0;
	while (objectNow = (LPOBJINST)GetObject(theObjects, i), objectNow)
		{
		if (objectNow->TestAliasFlag()
						&&
				objectNow->GetTypeID() == theMaster->GetTypeID()
						&&
				!lstrcmp(objectNow->GetName(), theMaster->GetName()))
			{
			objectNow->UpdateName(newName);

			if(!this->RenameConnectedParams(objectNow) && hdc)
				objectNow->GetGroupPtr()->DrawObject(hdc, objectNow, TRUE);

			}
		}

	if(hdc)
		ReleaseDC(windowPtr->GetWindowHandle(), hdc);

	return TRUE;
	}


//----------------------------------------------------------------------
//   SUBJECT::IsAliased
//
// Returns TRUE if the specified master is aliased in this subject.
//----------------------------------------------------------------------
BOOL SUBJECT::IsAliased(
	LPOBJINST  theMaster)
	{
	int       i=0;
	LPOBJINST objectNow;

	while (objectNow = (LPOBJINST)GetObject(theObjects, i), objectNow)
		{
		if (objectNow->TestAliasFlag()
						&&
				objectNow->GetTypeID() == theMaster->GetTypeID()
						&&
				!lstrcmp(objectNow->GetName(), theMaster->GetName()))
			{
			return TRUE;
			}
		}

	return FALSE;
	}

//----------------------------------------------------------------------
//   SUBJECT::RenameConnectedParams
//
// Renames all paramete+rs that are connected to 'theObject'.
//Returns TRUE if the screen is refreshed.
//----------------------------------------------------------------------
BOOL SUBJECT::RenameConnectedParams(
	LPOBJINST  theObject)
	{
	FUNCINST * temp;
	int           i=0;
	SERUID        uidNow;
	BOOL					redraw = FALSE;
	RECT					rt;
	RECT					refreshRect;

	uidNow = GetUID(theObject);

	SetRectEmpty(&refreshRect);
	while((temp = (FUNCINST *)GetObject(theFunctions, i))!=NULL)
		{
		if(temp->UpdateParamNames(uidNow, FALSE, &rt))
			{
			UnionRect(&refreshRect, &refreshRect, &rt);
			redraw = TRUE;
			}
		}

		//see if a window exists and if so refresh the client area

		if (redraw && windowPtr)
			{
			((LPSUBJECTWINDOW)windowPtr)->RefreshClientArea(&refreshRect);
			}

		return redraw;
	}


void SUBJECT::RedrawLinkedAliases(
	LPOBJINST sharedObjPtr,
	BOOL      doUpdate)
	{
	//We only need to redraw the aliases if the window is shown.
	if(IsAliased(sharedObjPtr) && windowPtr)
		{
		InvalidateRect(windowPtr->GetWindowHandle(),NULL, FALSE);

		if(doUpdate)
			UpdateWindow(windowPtr->GetWindowHandle());
		}

	}


void SUBJECT::RedrawAllFunctions()
	{
	if(windowPtr)
		((LPSUBJECTWINDOW)windowPtr)->RefreshClientArea();
	}

void SUBJECT::HighlightObj(
	LPOBJINST objPtr)
	{
	I_ASSERT(objPtr);

	ShowObject(GetUID(objPtr));

	I_ASSERT(windowPtr);

	((LPSUBJECTWINDOW)windowPtr)->FlashObject(objPtr);
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SUBJECT::HighlightWorkbenchItem
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////																	 
void SUBJECT::HighlightWorkbenchItem(
	LPFUNCWINOBJ itemPtr,
	BOOL flash,
	BOOL select)
	{
	RECT objRect;
	HDC hdc;

	I_ASSERT(itemPtr);

	if(windowPtr && (this == GetParentProject()->GetActiveSubject()))
		{
    ((LPSUBJECTWINDOW)windowPtr)->HighlightFunc(itemPtr, select);
		}
	else
		{
		itemPtr->GetRect(&objRect);
		MakeSureRectOnScreen(&objRect);
		OnlyUnselectAll();
		if(select)
			{
			itemPtr->SetSelected(TRUE);
			if(itemPtr->TestObjGroupFlag())
				ResetActiveGroup(GetUID(itemPtr));
			}

		if(windowPtr)
			{
			hdc = GetDC(windowPtr->GetWindowHandle());
			SetWindowOrgEx(hdc, originOffset.x, originOffset.y, NULL);
			((LPSUBJECTWINDOW)windowPtr)->RebuildSelectedLists();
			((LPSUBJECTWINDOW)windowPtr)->SetAllScrollBars();	
			InvalidateRect(windowPtr->GetWindowHandle(), NULL, FALSE);
			ReleaseDC(windowPtr->GetWindowHandle(), hdc);
			}
		}

	ShowObjectWindow();
	
	if(flash && windowPtr)
		((LPSUBJECTWINDOW)windowPtr)->FlashFunc(itemPtr);

	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SUBJECT::HighlightParameter
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SUBJECT::HighlightParameter(
	LPPARAM paramPtr)
	{
	HDC hdc;
	LPFUNCINST funcPtr = (LPFUNCINST)GetObject(paramPtr->ownerFunc);

	I_ASSERT(funcPtr);

	//false, false means don't flash and don't select.
	HighlightWorkbenchItem(funcPtr, FALSE, FALSE);

	if(paramPtr->TestLinkFlag())
		paramPtr->SetSelected(TRUE);

	if(windowPtr)
		{
		//only draw the parameter if its selection has changed
		if(paramPtr->IsSelected())
			{
			hdc = GetDC(windowPtr->GetWindowHandle());
			//TRUE means to erase the old
			paramPtr->Draw(hdc, TRUE);
			ReleaseDC(windowPtr->GetWindowHandle(), hdc);
		 	}

		((LPSUBJECTWINDOW)windowPtr)->FlashParameter(paramPtr);
		((LPSUBJECTWINDOW)windowPtr)->AddUIDToSelectedList(GetUID(paramPtr));
		}

	} 

																			 

/////////////////////////////////////////////////////////////
// FUNCTION:			SUBJECT::ObjectsToExistingGroup
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SUBJECT::ObjectsToExistingGroup(
	LPOBJGROUP dstGroup,
	int insertIndex,
	BOOL moveFunctions)
	{
	LPLIST dragObjects;
	BOOL result;
	LPLIST groupList;
	
	groupList = new LIST();
	dragObjects = new LIST();
	BuildDragObjList(groupList, dragObjects);

	result = dstGroup->InsertObjects(groupList, dragObjects, insertIndex, moveFunctions);
	
	//both lists should have been purged in the InsertObjects routine.
	delete dragObjects;
	delete groupList;

	return result;
	}

//Returns TRUE for success or FALSE if an error occurrs.
BOOL SUBJECT::ObjectsToNewGroup(
	POINT mouse,
	BOOL moveFunctions,
	LPSERUID  newGroupUID)
	{
	BOOL result;
	RECT cursorRect;
	LPOBJGROUP groupPtr;
	char nameBuffer[PRJ_NEWGROUPMAXNAME];

  GetParentProject()->GetNewGroupName(nameBuffer);
	groupPtr = new(theObjGroups) OBJGROUP(NULLSERUID, mouse, GetUID(this),
		nameBuffer);

  *newGroupUID = GetUID(groupPtr);

	result = ObjectsToExistingGroup(groupPtr, 0, 	moveFunctions);

	if(windowPtr)
		{
		//animate the group addition;
		MDI::GetPalettePtr()->GetCursorRect(&cursorRect, mouse);
		((LPSUBJECTWINDOW)windowPtr)->AnimateFunctionAdd(groupPtr, &cursorRect);
		}

	return result;
	}

BOOL SUBJECT::AddAliasObjects(
	LPLIST aliasList)
	{
	BOOL result;
	LPOBJINST objPtr;
  LPOBJINST aliasPtr;
	LPUIDLIST member;
	LPINSERTOBJDATA insertData;
	LPLIST insertObjList;
	LPOBJGROUP groupPtr;
	POINT groupLoc;
	char nameBuffer[PRJ_NEWGROUPMAXNAME];
	BOOL logBoundsChanged;

  insertObjList = new LIST();
	//Go through the alias list and create an alias object for each member of the list and
	//pack the uids of the alias objects in a dragObjList.
	member = NULL;
	while(NULL != (member = (LPUIDLIST)aliasList->GetNextMember(member)))
		{
		objPtr = (LPOBJINST)GetObject(member->uid);
		//pass in TRUE to create an alias
		aliasPtr = NewObjInst(objPtr->GetTypeID(), TRUE);

		//the pointer may have changed because of the new alias which was
		//created so get the object pointer again.
		objPtr = (LPOBJINST)GetObject(member->uid);

		aliasPtr->UpdateName(objPtr->GetName());
		insertData = new INSERTOBJDATA(GetUID(aliasPtr));
		insertObjList->Add(insertData);
		nObjectsCount++;
		GetParentProject()->IncObjectsCount();
		}

	//Get the active group or create a new group if necessary
	if(NULL == (groupPtr = GetActiveGroupPtr()))
		{
		GetDefaultGroupLocation((LPPOINT)&groupLoc);
    GetParentProject()->GetNewGroupName(nameBuffer);
		groupPtr = new(theObjGroups) OBJGROUP(NULLSERUID, groupLoc, GetUID(this),
		nameBuffer);
    }

	OnlyUnselectAll(GetUID(groupPtr));

	//Insert the alias objects
	result = groupPtr->InsertObjects(NULL, insertObjList,  OBJGROUP_DEFAULTINSERTION, FALSE);
	groupPtr->CalculateObjFlows();

	if(!result)
		{
		MessageBox(NULL, "An error ocurred while moveing the objects to make room for"
			" the aliases.  To prevent project corruption, AppWare is being closed "
			"without saving the changes since the last time you saved. Sorry!",
			"Fatal Error", MB_ICONEXCLAMATION|MB_TASKMODAL|MB_OK);
		exit(1);
		}

	groupPtr->SetActiveFlag();
	
	//pass in TRUE so it's caluculated for all stuff
	logBoundsChanged = CalculateLogBounds(TRUE);

	if(windowPtr)
		{
		((LPSUBJECTWINDOW)windowPtr)->RebuildSelectedLists();
		
		if(logBoundsChanged)
			((LPSUBJECTWINDOW)windowPtr)->AdjustScrollBars();
		
		((LPSUBJECTWINDOW)windowPtr)->RefreshClientArea();
    }
	delete insertObjList;

	GetParentProject()->UpdateObjCountBoxes();
	
	return result;
	}

//used for creating objects from another objects OBJD.
AOBJECTID SUBJECT::CreateDynamicObject(
	ATYPEID typeID,
	APROTOCOLID protocolID)
	{
	AObjMessage system;
	LPOBJGROUP groupPtr;
	LPOBJINST objPtr;
	char buffer[192];
	char typeChar[5];
	AOBJECTID newObjID;
	POINT groupLoc;
  char nameBuffer[PRJ_NEWGROUPMAXNAME];

  system.message1 = 0;
	system.message2 = protocolID;
	system.message3 = 0;
	system.message4 = 0;

  //FALSE means don't create an alias
	objPtr = NewObjInst(typeID, FALSE, &system);

	if(objPtr)
  	{
		//Get the active group or create a new group if necessary
		if(NULL == (groupPtr = GetActiveGroupPtr()))
			{
			GetDefaultGroupLocation((LPPOINT)&groupLoc);
			GetParentProject()->GetNewGroupName(nameBuffer);
			groupPtr = new(theObjGroups) OBJGROUP(NULLSERUID, groupLoc, GetUID(this),
				nameBuffer);
			}

		OnlyUnselectAll(GetUID(groupPtr));

		groupPtr->InsertObject(GetUID(objPtr), OBJGROUP_AFTERSELECTEDINSERTION);

		groupPtr->CalculateObjFlows();

		groupPtr->SetActiveFlag();

		nObjectsCount++;
		GetParentProject()->IncObjectsCount();
		
		if(windowPtr)
			{
			((LPSUBJECTWINDOW)windowPtr)->RebuildSelectedLists();
			((LPSUBJECTWINDOW)windowPtr)->RefreshClientArea();
			}

		GetParentProject()->UpdateObjCountBoxes();

		newObjID = GetUID(objPtr).id;
		}
	else
		{
		MAKE_TYPEID_READABLE(typeChar, typeID);
		wsprintf((LPSTR)buffer,  (LPSTR)"An object of type %s could not be created"
			" because no matching ALM could be found in the configuration information.",
			typeChar);

		MessageBox(NULL, (LPSTR)buffer, (LPSTR)"Object Creation Error",
			MB_ICONEXCLAMATION|MB_TASKMODAL|MB_OK);

    newObjID = 0;
		}

	return newObjID;
	}

BOOL SUBJECT::DestroyObject(
	LPOBJINST objPtr)
	{
	BOOL result;
	long beforeCount;
  long afterCount;
  LPOBJGROUP groupPtr;

	if(objPtr)
		{
    groupPtr = objPtr->GetGroupPtr();
    beforeCount = groupPtr->GetObjCount();
      
		if(0 == (afterCount = groupPtr->DestroyObject(objPtr)))
			{
			delete groupPtr;
			if(windowPtr)
      	((LPSUBJECTWINDOW)windowPtr)->RebuildSelectedLists();
			}

		//success if the counts are different because that means the object
    //was successfully removed.
    result = (beforeCount != afterCount)?TRUE:FALSE;

    if(result && windowPtr)
			((LPSUBJECTWINDOW)windowPtr)->UpdateSubjectWindow();
		}
	else
  	result = FALSE;

	return result;
	}

BOOL SUBJECT::RenameObject(
	LPOBJINST objPtr,
	LPCSTR newName)
	{
	BOOL result;

	if(objPtr)
		{
		result = objPtr->GetGroupPtr()->RenameObject(objPtr, newName);
		}
	else
  	result = FALSE;

	return result;
	}

void SUBJECT::BuildDragObjList(
	LPLIST groupList,
	LPLIST dragObjects)
	{
	LPUIDLIST member;
	LPINSERTOBJDATA dragData;
	LPOBJINST objPtr;
	int selCount;
	int selIndex;
  LPOBJGROUP groupPtr;
	int i=0;

	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		{
		if(groupPtr->OKToDragObjects())
			{
			member = new UIDLIST(GetUID(groupPtr));
			groupList->Add(member);

			//now get all of the group's selected objects and insert them into the group
			selCount = 0;
			selIndex = 0;
			while(0 != (objPtr = groupPtr->GetNextSelObj((int *)&selIndex, (int *)&selCount)))
				{
				dragData = new INSERTOBJDATA(GetUID(objPtr));
				objPtr->GetGroupPtr()->GetNippleTip((LPPOINT)&dragData->oldNipple, objPtr);
				dragObjects->Add(dragData);
				}
			}
  	}
	}


//if dragAndDrop if FALSE, the object is added to the active group. If there is not
//active group, then a new group is created at a default location.
//if dragAndDrop is TRUE, the location is used to insert the object, otherwise, the
//location is assumed to be the cursor location at the time the object is to be
//added.
//NOTE: added means to add the object to the active group either at the end or above
//      the topmost selected item.
BOOL SUBJECT::AddObjInst(
	LPOBJDESC objDesc,
	BOOL aliasFlag,
	LPPOINT location,
	LPCSTR objName,
	BOOL dragAndDrop,
	LPRECT fromRect) //from rect can only be used when we're not doing dragAndDrop.
	{
	LPOBJGROUP groupPtr=NULL;
	HDC hdc;
	HWND hWnd;
	LPOBJINST objInstPtr;
	BOOL doPop=FALSE;
	BOOL result = FALSE;
	POINT newLocation;
	RECT cursorRect;
	char nameBuffer[PRJ_NEWGROUPMAXNAME];

	if(pTheSystem->GetFileObject()->GetFileIDStamp() != 
		fileObjPtr->GetFileIDStamp())
		{
		PROGSYSTEM::PushAndRegisterContext(fileObjPtr);
		doPop = TRUE;
		}

	//Create the object
	objInstPtr = NewObjInst(objDesc->GetUID(), aliasFlag);
	if(objInstPtr)
		{
		nObjectsCount++;
		GetParentProject()->IncObjectsCount();

		if(objName)
			objInstPtr->UpdateName(objName);

		if(windowPtr)
			{
      //convet the location to logical coordinates
			hWnd = windowPtr->GetWindowHandle();
			hdc = GetDC(hWnd);
			newLocation = *location;
			//Figure rectangle of icon being dropped
			ScreenToClient(hWnd, (LPPOINT)&newLocation);
			DPtoLP(hdc, &newLocation, 1);
			MDI::GetPalettePtr()->GetCursorRect(&cursorRect, newLocation);


			if(dragAndDrop)
				groupPtr = ((LPSUBJECTWINDOW)windowPtr)->GetTargetGroup(&cursorRect);			
			else
				{
				//try to get the active group
				if(activeGroup != NULLSERUID)
					{
					groupPtr = (LPOBJGROUP)GetObject(activeGroup);
					
					if(fromRect)
						{
						cursorRect = *fromRect; //cursor rect is used in animating
						ScreenToClient(hWnd, (LPPOINT) &cursorRect.left);
						ScreenToClient(hWnd, (LPPOINT) &cursorRect.right);
						DPtoLP(hdc, (LPPOINT) &cursorRect, 2);
						}
					}
        }
			
			
			}

		if(groupPtr)
			{
			//If selected, the selection must be erased before the object is added
			//or the groups bounds will change and the selection rect won't be drawn
      //properly.
			if(windowPtr && groupPtr->IsSelected())
					groupPtr->EraseSelection(hdc);

			if(dragAndDrop)
				groupPtr->AddObject(GetUID(objInstPtr), &newLocation);
			else
      	{
				groupPtr->InsertObject(GetUID(objInstPtr), OBJGROUP_BOTTOMINSERTION);
				}

			groupPtr->CalculateObjFlows();
			if(windowPtr)
        //FALSE means we didn't create a new group
				((LPSUBJECTWINDOW)windowPtr)->AddObjectToGroup(hdc, objInstPtr, &cursorRect,
					FALSE);
      }
		else
		 	{
			//We need to create a new group
			if(!dragAndDrop)
				//the location points to where the cursor is but we don't get care since
				//we're doing a default add - this means it was most likely added be
        //double clicking in the palette.
				GetDefaultGroupLocation((LPPOINT)&newLocation);

			GetParentProject()->GetNewGroupName(nameBuffer);
			new(theObjGroups) OBJGROUP(GetUID(objInstPtr),
				newLocation, GetUID(this), nameBuffer);

			if(windowPtr)
				{
				//TRUE means we created a new group
				((LPSUBJECTWINDOW)windowPtr)->AddObjectToGroup(hdc, objInstPtr, &cursorRect,
					TRUE);
				}
      }

		if(windowPtr)
			ReleaseDC(hWnd, hdc);

		if(CalculateLogBounds(TRUE) && windowPtr)
			((LPSUBJECTWINDOW)windowPtr)->AdjustScrollBars();
		
		GetParentProject()->UpdateObjCountBoxes();

		result = TRUE;
		}


	if(doPop)
		PROGSYSTEM::PopAndRegisterContext();

	return result;
	}

RUNTIMEUID SUBJECT::GetObjGroupList()
	{
	return theObjGroups;
	}


SERUID SUBJECT::CopyGroup(
	LPOBJGROUP groupPtr,
	POINT newLocation)
	{
	LPOBJGROUP newGroup;

	//store the groupPtr UID because the pointer may change when the
  //new group is created.
	SERUID groupUID = GetUID(groupPtr);

	//place members of the group in locals because the groupPtr may change
	//before the parameters of the OBJGROUP constructor can be resolved.
	int baseStatus = groupPtr->GetStatus();
	long groupStatus = groupPtr->GetGroupStatus();
	int width = groupPtr->GetWidth();
	int count = groupPtr->GetObjCount();
	TSTRING groupName = groupPtr->GetName();
  LPGROUPINFO infoPtr = groupPtr->GetInfoPtr();

	newGroup = new(theObjGroups) OBJGROUP(GetUID(this),
		newLocation,
		baseStatus,
		groupStatus,
		width,
		count,
		groupName,
		infoPtr);

	groupPtr = (LPOBJGROUP)GetObject(groupUID);
	newGroup->CopyObjects(groupPtr, theObjects);

	return GetUID(newGroup);
  }

////////////////////////////////////////////////////////////////////////
// SUBJECT::NewObjInst
//
// Returns a pointer to a newly created OBJINST for the serius object
// described by 'pObjDef'.
// Returns NULL if there is an error, probably because the ADMN or OBJD proc
// could not be located.
////////////////////////////////////////////////////////////////////////
LPOBJINST SUBJECT::NewObjInst(
	ATYPEID 		typeID,
	BOOL      aliasFlag,
	pAObjMessage creationInfo) //creationInfo is optional and defaults to NULL
	{
	LPOBJINST objPtr = NULL;
	BOOL      doPop;
	LPOBJDESC	pObjDef;
	SERUID  newObjInstUID;
	SERUID  typeUID;
	char buffer[256];
  char typeBuf[5];

	if(pTheSystem->GetFileObject()->GetFileIDStamp() == 
		fileObjPtr->GetFileIDStamp())
		doPop = FALSE;
	else
		{
		PROGSYSTEM::PushAndRegisterContext(fileObjPtr);
		doPop = TRUE;
		}

	pObjDef = cfgGetObjectDesc(typeID);
	// Try to increment the count on the appropriate object type
	// IncTypeCount expects TRUE for the last parameter if the object is a
	// real object so pass !aliasFlag
	if(pObjDef && (((typeUID = IncTypeCount(pObjDef->GetUID(), !aliasFlag)))
  	!= NULLSERUID))
		{
		newObjInstUID = CreateObjInst(pObjDef, aliasFlag, typeUID, creationInfo);
		if(newObjInstUID.id != NULLSERUID)
			{
			objPtr = (LPOBJINST)GetObject(newObjInstUID);
			if(pObjDef->TestMustBePersistentFlag())
				objPtr->SetPersistentsFlag();
			GetParentProject()->MakeDirty();
			}
		else
			{
			//print an error message
			MAKE_TYPEID_READABLE(typeBuf, typeID);

			wsprintf(buffer, "Cannot create an object of type [ %s ] because an error"
				" occurred in the object's create routine.", typeBuf);

			MessageBox(NULL, buffer, "Object Creation Error",
				MB_TASKMODAL|MB_ICONEXCLAMATION|MB_OK);
      }
		}
	else
		{
		//print an error message
		MAKE_TYPEID_READABLE(typeBuf, typeID);

		if(pObjDef)
			wsprintf(buffer, "Cannot create an object of type [ %s ] because the "
				"ADMN procedure address could not be obtained from the ALM.", typeBuf);
		else
			wsprintf(buffer, "Cannot create an object of type [ %s ] because the "
				"configuration information is not available.", typeBuf);

		MessageBox(NULL, buffer, "Object Creation Error",
			MB_TASKMODAL|MB_ICONEXCLAMATION|MB_OK);
		}

	if(doPop)
		PROGSYSTEM::PopAndRegisterContext();

	return objPtr;
	}

LPSUBTYPE SUBJECT::GetSubTypePtr(
	ATYPEID typeID)
	{
	int i=0;
	LPSUBTYPE subTypePtr;

	while(0 != (subTypePtr = (LPSUBTYPE)GetObject(theSubTypes, i)))
		{
		if(typeID == subTypePtr->typeID)
			return subTypePtr;;

		}
	return NULL;
	}

#define SUBJECT_GROUP_OFFSET 10
void SUBJECT::GetDefaultGroupLocation(
	LPPOINT pt)
	{
	POINT base;
	RECT bounds;

	pt->x = originOffset.x + FUNCSIZE;
	pt->y =	originOffset.y + FUNCSIZE;

	base = *pt;	//used as the starting point for diaganal offsets
	
	//now make sure the group caption is not exactly ontop of another
	//groups caption.

	while(DuplicateCaptionLocation(*pt))
		{
		//try offsetting diaganally
		pt->x+=SUBJECT_GROUP_OFFSET;
		pt->y+=SUBJECT_GROUP_OFFSET;

		//make sure not extended out of screen view	if the window is visible,
		//otherwise, make sure not to extend out of the logical bounds
		if(GetWindowPtr())
			GetSubjectLogicalBounds(&bounds);
		else //make sure not extended out of logical bounds
			bounds = logBounds;

		if(pt->x >= bounds.right || pt->y >= bounds.bottom)
			{
			base.x += SUBJECT_GROUP_OFFSET;
			*pt = base;

			if(pt->x >= bounds.right)
				{
				//just shove the thing in the center of the bounds and to heck with
				//it.

				pt->x = (bounds.right + bounds.left)/2;
				pt->y = (bounds.bottom + bounds.top)/2;

				break;
				}
			}
		}
	}

BOOL SUBJECT::DuplicateCaptionLocation(
	POINT origin)
	{
	int i=0;
	LPOBJGROUP groupPtr;

	while(NULL != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		{
		if(groupPtr->GetLocation().x == origin.x && 
			groupPtr->GetLocation().y == origin.y)
			return TRUE;
		}

	return FALSE;
	}
		 

void SUBJECT::ClearAllChainFlags()
	{
	LPFUNCINST funcPtr;
	int i=0;

	while(0 != (funcPtr = (LPFUNCINST)GetObject(theFunctions, i)))
		if(funcPtr->TestChainVisitedFlag())
			funcPtr->ClearChainVisitedFlag();

	}

BOOL SUBJECT::GroupsSelected()
	{
	BOOL result;
	int i;
	LPOBJGROUP groupPtr;

	if(windowPtr)
		result = ((LPSUBJECTWINDOW)windowPtr)->GroupsSelected(); //uses selected list
	else
		{
		result = FALSE;
		i=0;
		while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
			{
			if(groupPtr->IsSelected())
				{
				result = TRUE;
				break;
				}
			}
		}

	return result;
	}

void SUBJECT::SetSelectedGroupsIconSize(
	WORD newSize)
	{
	LPOBJGROUP groupPtr;
	int i=0;

	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		{
		if(groupPtr->IsSelected())
    	{
			groupPtr->SetGroupIconSize(newSize);
			groupPtr->CalculateObjFlows();
			}
		}

	if(windowPtr)
		((LPSUBJECTWINDOW)windowPtr)->RefreshClientArea();


	}

void SUBJECT::MakeObjectsPersistent()
	{
	int i=0;
	LPOBJGROUP groupPtr;

	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		groupPtr->MakeObjectsPersistent();

	GetParentProject()->RedrawAllLinkedAliases(this);
	}

void SUBJECT::MakeObjectsNonPersistent()
	{
	int i=0;
	LPOBJGROUP groupPtr;

	while(0 != (groupPtr = (LPOBJGROUP)GetObject(theObjGroups, i)))
		groupPtr->MakeObjectsNonPersistent();

	GetParentProject()->RedrawAllLinkedAliases(this);
	}

//returns TRUE if the rect was already on the screen
BOOL SUBJECT::MakeSureRectOnScreen(
	LPRECT rt)
	{
	RECT logRect;
  BOOL onScreen;

	GetSubjectLogicalBounds(&logRect);
  //release the DC in the if statements
	if(PtInRect(&logRect, (POINT)*(LPPOINT)&(rt->left)) &&
		PtInRect(&logRect, (POINT)*(LPPOINT)&(rt->right)))
		{
		onScreen = TRUE;
		}
	else
		{
    onScreen = FALSE;
		//center the object on the screen
		originOffset.x = ((rt->right + rt->left)/2 - (logRect.right - logRect.left)/2);
		originOffset.y = ((rt->bottom + rt->top)/2 - (logRect.bottom - logRect.top)/2);
		FitLogBoundsToClientBounds();
		}

	return onScreen;
	}


void SUBJECT::FitLogBoundsToClientBounds()
	{
  RECT client;

	GetSubjectLogicalBounds(&client);

	if(client.right > logBounds.right)
		{
		logBounds.right = client.right;
		if(logBounds.right > XMAXPAGE)
			{
			logBounds.right = XMAXPAGE;
			originOffset.x-=(client.right - logBounds.right);
			}
		}
	else if(client.left < logBounds.left)
		{
		logBounds.left = client.left;
		if(logBounds.left < 0)
			{
			logBounds.left = 0;
			originOffset.x+=(logBounds.left - client.left);
			}
		}
	if(client.bottom > logBounds.bottom)
		{
		logBounds.bottom = client.bottom;
		if(logBounds.bottom > YMAXPAGE)
			{
			logBounds.bottom = YMAXPAGE;
			originOffset.y-=(client.bottom - logBounds.bottom);
			}
		}
	else if(client.top < logBounds.top)
		{
		logBounds.top = client.top;
		if(logBounds.top < 0)
			{
			logBounds.top = 0;
			originOffset.y += (logBounds.top - client.top);
			}
		}
  }
////////////////////////////////////////////////////////////
//if the client bounds are outside of the log bounds, shove
//them back in without resizing.  Care should be taken when
//calling this routine since it assumes the client bounds will
//fit in the log bounds.
void SUBJECT::AdjustClientToLogBounds()
	{
	RECT client;

	GetSubjectLogicalBounds(&client);

	if(client.left < logBounds.left)
		originOffset.x+= (logBounds.left - client.left);
	else if(client.right > logBounds.right)
		originOffset.x-=(client.right - logBounds.right);

	if(client.top < logBounds.top)
		originOffset.y+=(logBounds.top - client.top);
	else if(client.bottom > logBounds.bottom)
		originOffset.y-=(client.bottom - logBounds.bottom);

	}


/////////////////////////////////////////////////////////////
// FUNCTION:			SUBJECT::ResetActiveGroup
//						 
// PARAMETERS:
//
// DESCRIPTION:		resets the active group to 'newActiveGroup'
//								without doing any redrawing.
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SUBJECT::ResetActiveGroup(
	SERUID newActiveGroup)
	{
	if(newActiveGroup != activeGroup)
		{
		if(activeGroup != NULLSERUID)
			GetActiveGroupPtr()->ClearActiveFlag();

		((LPOBJGROUP)GetObject(newActiveGroup))->SetActiveFlag();
		activeGroup = newActiveGroup;
		}
	} 

