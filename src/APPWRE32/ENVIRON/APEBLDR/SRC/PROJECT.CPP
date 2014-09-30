#include "almsys.h"

#include "resconst.h"
#include "prjwin.h"
#include "memlib.h"
#include "project.h"
#include "subject.h"
#include "mdiclass.h"
#include "progsys.h"
#include "rectypes.h"
#include "cliphead.h"
#include "oipexe.h"
#include "clipobj.h"
#include "exetypes.h"
#include "printer.h"
#include "errors.h"
#include "tempobj.h"
#include "upsf_env.h"
#include "navcls.h"
#include "compstat.h"
#include "sjtwin.h"
#include "funcinst.h"
#include "clipsjt.h"
#include "prtlabel.h"
#include "objtype.h"
#include "oiputils.h"
#include "cfglibp.h"
#include "srchmgr.h"
#include "upsfname.h"
#include "apbutil.h"
#include "time.h"
#include "unitree.h"
#include "listmbr.h"
#include "fnameobj.h"
#include "utility.h"
#include "splshmgr.h"
#include "exelibl.h"
#include "response.h"
#include "exeevntl.h"
#include "objdesc.h"

#ifdef AW_I_TIME
#include "mmsystem.h"
#endif

//the following file should only be included in the demo version
#ifdef DEMO
#include "demo.h"
#endif

// exeXXX.h files for compile-time ordering of lists

#include "exeevntl.h"
#include "exelibl.h"
#include "exebeadl.h"

#define PRJ_MICROAPP_FILENAME  "aw32.dat"

#ifndef CRLF

  #define CRLF "\r\n"

#endif

#define INITSUBJECTINDEX 1
#define PRJ_PREEXTENSION 8

#define PRJ_EXPORTPREP				0.04
#define PRJ_TYPELISTS					0.08
#define PRJ_HALFTYPELISTS			(PRJ_TYPELISTS / 2)

#define PRJ_SJTCOMPWEIGHT			3L
#define PRJ_OBJCOMPWEIGHT			6L
#define PRJ_FUNCCOMPWEIGHT		6L

#define PRJ_PRJCOMPTIME				0.05
#define PRJ_PREPCOMPTIME			0.02
#define PRJ_COMPILETIMESLICE				0.95
#define PRJ_CREATEAPPTIMESLICE			(1.0 - PRJ_COMPILETIMESLICE)

#define PRJ_SJTSORTTOLERANCE 16 // 1/2 of the SUBJECTICONHEIGHT

#define MAX_INITIAL_PRJ_HEIGHT 200

unsigned int PROJECT::projectIndex = 0;
int	PROJECT::labelIndent = 0;
PRINTLABEL * PROJECT::labelPtr;
SCOPESTACK *	PROJECT::scopeStackPtr;
COMPILESTATUS * PROJECT::compStatPtr;
int PROJECT::projectCount = 0;
COLORREF PROJECT::textColor;

//dragPen must be set to null as future comparison depends on it.
HPEN PROJECT::dragPen = NULL;
COLORREF PROJECT::highlightColor=NULL;
COLORREF PROJECT::windowBkgrndColor=NULL;

LPUNIQUETREE PROJECT::upsfObjTypeTree=NULL;
LPUNIQUETREE PROJECT::upsfFuncTypeTree=NULL;

int PROJECT::upsfImportVersion;


////////////////////////////////////////////////////////////////////////////
// Structures required for replacing the icon in the shell executable.
///////////////////////////////////////////////////////////////////////////

#define RTYPE_ICON 0x0003

#define NEWHEADERPOS 0x0000003C
#define IDERR_READFAIL -1

BOOL supplantIcon(
	HANDLE fileHandle,
	long Pos,
	SERIUSICON serIcon);

typedef struct {
	unsigned short	signature;
	unsigned short	bLP;
	unsigned short	pages;
	unsigned short	relocation;
	unsigned short	headerSize;
	unsigned short	minParagraphs;
	unsigned short	maxParagraphs;
	unsigned short	SS;
	unsigned short	SP;
	unsigned short	checkSum;
	unsigned short	IP;
	unsigned short	CS;
	unsigned short	lpRelocation;
	unsigned short	overlay;
	unsigned short	reservedWords;
	LONG		posNewHdr;
}	OLDHDR;


typedef struct {
	WORD 	signature;
	char  verNumber;
	char	lingRevNumber;
	WORD	entryTable;
	WORD  bytesInEntryTable;
	LONG	checkSum;
	WORD  flag;
	WORD	autoData;
	WORD	heap;
	WORD	stack;
	LONG	CSIP;
	LONG	SSSP;
	WORD	segCount;
	WORD  moduleEntries;
	WORD	nonResNameTable;
	WORD	offsetSegTable;
	WORD  offsetResTable;
	WORD  offsetResNameTable;
	WORD	offsetModRefTable;
	WORD	offsetImpNameTable;
	LONG	offsetNonResNameTable;
	WORD	moveableEntries;
	WORD	segAlignShiftCount;
	WORD	resSegCount;
	BYTE	exeType;
	BYTE	otherFlags;
	WORD	offsetGang;
	WORD	gangLength;
	WORD	swapArea;
	WORD	expVersion;
} NEWHDR;

typedef struct {
	WORD	type;
	WORD	count;
	LONG	Proc;
} RESTYPEINFO;

typedef struct {
	unsigned short	offset;
	unsigned short  length;
	unsigned short	flags;
	unsigned short	ID;
	unsigned short	handle;
	unsigned short	usage;
} RESNAMEINFO;

typedef RESNAMEINFO * LPRESNAMEINFO;

typedef struct {
	RESTYPEINFO	rt;
	RESNAMEINFO rn[1];
} RESTABLE;

struct ICOFILEHDR
	{
	WORD	icoReserved;
	WORD	icoResourceType;
	WORD	icoResourceCount;
	};

struct ICODIRENTRY
	{
	BYTE		width;
	BYTE		height;
	BYTE		colorCount;
	BYTE		reserved1;
	WORD		reserved2;
	WORD		reserved3;
	DWORD		icoDIBSize;
	DWORD		icoDIBOffset;
	};


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:PROJECT
//
// CLASS.......:PROJECT
//
// DESCRIPTION.:Constructor- register the class if necessary, perform
//							all initialization, and create a PROJECTWINDOW. Constructor
//							which is used for a new project.
//
// PARAMETERS..:Application instance, prvious instance, MDI frame window
//							handle, MDI client window handle.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
PROJECT::PROJECT(
	char constructorType):WORKSHEET()
	{
	RECT clientRect;
	SERUID dummyUID;
	LPSUBJECT subjectPtr;
  int i;

	nObjectsCount = 0;
	ClearIconicFlag();
	ClearMaximizeFlag();
	activeSubject=NULL;

	fileObjPtr = pTheSystem->GetFileObject();
	//push the projects context and register the context with oip
	PROGSYSTEM::PushAndRegisterContext(fileObjPtr);
	fileObjPtr->SetHeadRec(GetUID(this));
	fileObjPtr->SetSaveAsFlag();
	SetPath(".");
	WORKSHEET::indexList->Add((HWND) INITSUBJECTINDEX, fileObjPtr);
	subjectBlock = GetUID(new RECORDBLOCK(sizeof(SUBJECT), RT_SUBJECT));
	typeBlock = GetUID(new RECORDBLOCK(sizeof(OBJTYPE), RT_OBJTYPE));

	//SetSysColors must be called before setting the private colors
	if(!projectCount)
		PROJECT::SetSysColors();
	DoPrivateColorSet();

	//SetSysColors must be called before incrementing project count
	projectCount++;

	GetClientRect(PROGSYSTEM::hWndClient, (LPRECT)&clientRect);
	clientRect.bottom = 4 * clientRect.bottom / 9;

	//Make sure the project isn't unneccessarily big on high resolution
	//screens
	if(clientRect.bottom > MAX_INITIAL_PRJ_HEIGHT)
		clientRect.bottom = MAX_INITIAL_PRJ_HEIGHT;

	SetWindowBounds(&clientRect);
	subjectCount = 0;

	lastSubjectPosition.x = 0;
	lastSubjectPosition.y = SUBJECTTOPSPACING;

	originOffset.x =	0;
	originOffset.y =	0;

	logBounds.x= GetSystemMetrics(SM_CXSCREEN);
	logBounds.y = GetSystemMetrics(SM_CYSCREEN);

	SetGrid(PROGSYSTEM::TestPrjGridPref());

	//initialize to 1 even for UPSF constructor incase 
	//default groups have to be created.
	newGroupIndex = 1;

	if(constructorType & PRJ_NORMAL_CONSTRUCTOR)
		{
		ClearIconicFlag(); //make sure the window will be shown

		BuildTypeBlock();
		ShowObjectWindow();
		MDI::GetNavigatorPtr()->AddItem(GetUID(this), NULLSERUID, NULLSERUID,
			fileObjPtr, GetName());
		MakeClean();
		}
	else if(constructorType & PRJ_UPSF_CONSTRUCTOR)
		{
		//turn off alias warnings so message boxes don't appear.
		PROGSYSTEM::SetNoAliasWarningsFlag();
		//importing the project should also add the name to the navigator
		
		SetUpsfImportFlag();
		if(ImportUPSF())
			{
      ClearUpsfImportFlag();
      
			UpdateTypeBlock();

			ShowAllWindows();
			//only want to pop the context since we don't want to register
			//another context with oip since that would have been done
			//when the project or subject windows were activated.				 
			PROGSYSTEM::ClearNoAliasWarningsFlag();
			MakeDirty();

			//reset the group index to the total number of groups + 1.
			newGroupIndex = 1;
			i=0;
			while(0 != (subjectPtr = (LPSUBJECT)GetObject(subjectBlock, i)))
				newGroupIndex+=subjectPtr->GetGroupCount();
			}
		else
			{
      ClearUpsfImportFlag();
			if(scopeStackPtr)
				{
				scopeStackPtr->ErrorCleanup();
				delete scopeStackPtr;
				}
			PROGSYSTEM::SetImportErrorFlag();
			//an error ocurred so restore the context to its previous state
			PROGSYSTEM::PopAndRegisterContext();
			PROGSYSTEM::ClearNoAliasWarningsFlag();
			return; //an error was encountered in ImportUPSF
			}
		}

	PROGSYSTEM::PopAndRegisterContext();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:PROJECT
//
// CLASS.......:PROJECT
//
// DESCRIPTION.:	overridded constructor which is called when a project
//								is opened up from disk.
//
// PARAMETERS..:  fileHandle is the file from which to read the data.
//								keepName will be TRUE if the project is to retain its name
//								If keepName is false, the project will take on a generic
//								name.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

PROJECT::PROJECT(
	void * thisPtr):
		WORKSHEET(thisPtr),
		theIcon(&theIcon)
	{

	fileObjPtr = pTheSystem->GetFileObject();
	WORKSHEET::indexList->Add((HWND) INITSUBJECTINDEX, fileObjPtr);
	activeSubject=NULL;

	//The system colors must be set before setting the private colors
	if(!projectCount)
		PROJECT::SetSysColors();
	DoPrivateColorSet();

	//SetSysColors must be called before incrementing the project count
	projectCount++;
	MDI::GetNavigatorPtr()->AddItem(GetUID(this), NULLSERUID, NULLSERUID,
		fileObjPtr, GetName());

	}

//Private routine to be called from constructors and from
//SetPrivateColors.
BOOL PROJECT::DoPrivateColorSet()
	{

	SetWindowColor(PROJECT::windowBkgrndColor);

	//signal no error
	return TRUE;
	}

//Static routine
BOOL PROJECT::SetSysColors()
	{
	//IMPORTANT NOTE:  The project maintains it's own colors since it was
	//originally thought that the project window background would be a
	//different color than the subject.  However, it has since been decided
	//that they will be the same.
	//Still, I prefer the idea of different colors so I have kept the  code
	//to make the change easy if we ever decide to change.


	//if the dragPen has already been created, then delete to make way for
	//the new.
	if(projectCount && dragPen)
		{
		DeleteObject(dragPen);
		//dragPen must be set to Null so it can be recreated
		dragPen = NULL;
		}

	//set the drag color to black which will be OK as
	//as long as a NOTXOR op is used
	//only want to create the pen if its NULL
	//All the colors created in this section are static and only need to
	//be created once.
	if(!dragPen)
		{
		windowBkgrndColor = GetSysColor(COLOR_WINDOW);

		dragPen = CreatePen(PS_DOT, 1, RGB(0,0,0));

		highlightColor = GetSysColor(COLOR_HIGHLIGHT);
		//Make sure that both the highlight color and window color are not the
		//same color since sometimes the highlight color will be drawn as a
		//copy op.
		if(highlightColor ==  windowBkgrndColor)
			{
			if(windowBkgrndColor != RGB(0,0,0))
				highlightColor = RGB(0,0,0);
			else
				//set highlight to light gray since we already know the window is
				//black.  NOTE: we don't make it white since white drawn on black
				//with a NOTXOR op will result in black!
				highlightColor = RGB(192,192,192);
			}
		//now choose colors to match the background

		textColor = GetSysColor(COLOR_WINDOWTEXT);
		if(textColor == windowBkgrndColor)
			{
			//the text color is the same as the window color
			//Either set the text color to black or white - whatever the background
			//isn't.
			if(textColor != RGB(0,0,0))
				textColor = RGB(0,0,0);
			else
				textColor = RGB(255,255,255);
			}
		}

	//no errors if we make it to here
	return TRUE;
	}

//routine to be called externally
BOOL PROJECT::SetPrivateColors()
	{
	SUBJECT * subjectPtr;
	int i;

	if(!DoPrivateColorSet())
		return FALSE;

	i=0;
	while(0 != (subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(!subjectPtr->SetPrivateColors())
			return FALSE;
		}

	if(GetWindowPtr())
		InvalidateRect(GetWindowPtr()->GetWindowHandle(), NULL, FALSE);

	return TRUE;
	}

short PROJECT::PostReadInit()
	{
	short result = PRJ_READNOERROR;
	SUBJECT * subjectPtr;
	int i;
	LPOBJTYPE typePtr;

	i=0;
	while(!(result & PRJ_READERROR) &&
		(typePtr = (LPOBJTYPE)GetObject(typeBlock, i))!=0)
		{
		//don't delete the types if they're bad because validating
		//the sub types depend on their corresponding real types being
		//there.
		result |= typePtr->PostReadInit();
		}

	UpdateTypeBlock();

  i=0;
	while(!(result & PRJ_READERROR) &&
		(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i))!=0)
		result |= subjectPtr->PostReadInit();

	//Now that all types have their OBJD and ADMN addresses, validate the
	//flows.  All types must have their OBJD's and ADMN's first so that the
	//objects and functions can obtain their signal blocks.
	//Count the number of objects as well
	nObjectsCount=0;
	i=0;
	while(!(result & PRJ_READERROR) &&
		(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i))!=0)
		{
		nObjectsCount+=subjectPtr->GetObjectCount();
		result |= subjectPtr->ValidateFlows();
		}
	if(!(result & PRJ_READERROR))
		ShowAllWindows();

	if(result == PRJ_READNOERROR)
		MakeClean();
	else
		MakeDirty();

	return result;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~PROJECT
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  class destructor.  Delete all subjects and delete the
//								PROJECTWINDOW if it is currently created.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
PROJECT::~PROJECT()
	{
	LONG		lTmp			= 0;
	HCURSOR oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

	ShowCursor(TRUE);

	SendMessageToObject(AOBJ_ASLEEP, lTmp, lTmp, lTmp);

	WORKSHEET::indexList->RemoveByPtr(fileObjPtr);

  ((LPRECORDBLOCK)GetObject(subjectBlock))->PurgeBlock();
	((LPRECORDBLOCK)GetObject(typeBlock))->PurgeBlock();

	MDI::GetNavigatorPtr()->DeleteItem(fileObjPtr, GetUID(this), NULLSERUID,
		NULLSERUID);

	delete (RECORDBLOCK *)GetObject(GetSubjectBlock());
	delete (RECORDBLOCK *)GetObject(typeBlock);

	ShowCursor(FALSE);
	SetCursor(oldCursor);
	projectCount--;
	if(!projectCount)
		{
		DeleteObject(dragPen);
		//dragPen must be set to Null so it can be recreated
		dragPen = NULL;
		}

	MDI::GetSearchPtr()->ReportProjectClosed(fileObjPtr);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ShowObjectWindow
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  creates a PROJECTWINDOW if the PROJECTWINDO is not
//								currently in memory. Otherwise ActivateTheWindow is
//								called.
//
// PARAMETERS..:
//
// RETURN VALUE: 1 to show the function succeeded.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::ShowObjectWindow()
	{
	if(!GetWindowPtr())
		SetWindowPtr(new PROJECTWINDOW(GetUID(this)));
  else
		ActivateTheWindow();
	return TRUE;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetClientBounds
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Returns the client bounds of the ProjectWindow.
//								the client bounds exclude the area taken by the scroll
//								bars.
//
// PARAMETERS..:  LPRECT winBounds: points to rectangle to fill with the
//								client bounds info.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::GetClientBounds(LPRECT winBounds)
{
	RECT bounds;

	GetWindowBounds(&bounds);
	winBounds->left =0;
	winBounds->right = bounds.right - bounds.left -
		GetSystemMetrics(SM_CXVSCROLL)-
		2*GetSystemMetrics(SM_CXFRAME) + 1;
	winBounds->top = 0;
	winBounds->bottom = bounds.bottom - bounds.top-
		GetSystemMetrics(SM_CYHSCROLL) -
		2 *	GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYCAPTION)+2;
}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OnlyChangeSelection
//
// CLASS.......: PROJECT
//
// DESCRIPTION.: changes which subject is selected.  Does not redraw.
//
// PARAMETERS..: SERUID newSelection: subject which will be set to selected.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::OnlyChangeSelection(
	SERUID	newSelection)
	{
	SUBJECT * temp;
	SUBJECT * subjectPtr;
	int		i=0;

	subjectPtr = (SUBJECT *)GetObject(newSelection);
	while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(temp->IsSelected())
			temp->SetSelected(FALSE);
		}
		subjectPtr->SetSelected(TRUE);
	}

BOOL PROJECT::OnlySnapSelected(
	HDC hdc,
	LPRECT refreshRect)
	{
	BOOL result = FALSE;
	BOOL snapped = FALSE;
	SUBJECT * temp;
	int i=0;
	RECT bounds;

	SetRectEmpty(refreshRect);
	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
			if(temp->IsSelected())
				{
				temp->GetBoundingRectangle(hdc,&bounds);
				UnionRect(refreshRect, refreshRect, &bounds);
				snapped |= temp->AdjustToGrid();
				if(snapped)
					{
					result|=snapped;
				temp->GetBoundingRectangle(hdc,&bounds);
				UnionRect(refreshRect, refreshRect, &bounds);
					}
				}
		}
	return result;
	}




/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	UnSelectSubjects.
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Unselect all the Subjects and redraw.
//
// PARAMETERS..:  HDC hdc: device context.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::UnSelectSubjects(
	HDC hdc)
	{
	RECT	clientRect;
	POINT	offset;
	SUBJECT * temp;
	int		i=0;
	BASEWINDOW * theWindow;

  theWindow = GetWindowPtr();
	if(theWindow)
		((PROJECTWINDOW *)theWindow)->SetClippingRegion(hdc);

	offset.x = 0;
	offset.y = 0;
	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(temp->IsSelected())
			{
			temp->SetSelected(FALSE);
			if(theWindow)
				{
				GetClientBounds(&clientRect);
				temp->DrawASubject(hdc,
					&clientRect, offset);
				}
			}
		}
	}

void PROJECT::OnlyUnselectSubjects(
	HDC hdc,
	LPRECT refreshRect)
	{
	int i=0;
	SUBJECT * temp;
	RECT bounds;

	SetRectEmpty(refreshRect);
	while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(temp->IsSelected())
			{
			temp->SetSelected(FALSE);
			temp->GetBoundingRectangle(hdc, &bounds);
			UnionRect(refreshRect, refreshRect, &bounds);
			}
		}
	}

void PROJECT::ClearSubjectSelections()
	{
	int i=0;
	SUBJECT * temp;

	while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
		if(temp->IsSelected())
			temp->SetSelected(FALSE);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ChangeSelection
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  changes the current selection.  If the shift key is being
//								held down, the new selection is added and the already
//								selected objects remain selected.
//
// PARAMETERS..:  HDC hdc: device context.
//								SERUID newlySelected: the newly selected subject.
//								WORD wParam: from windows to tell if the shift key is
//								held down.
//
// RETURN VALUE:  nonzero if selection changed else zero.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::ChangeSelection(
	HDC		hdc,
	SERUID	newlySelected,
	WPARAM	wKeyStates)
	{
	RECT bounds;
	RECT unionRect;
	SUBJECT * newSelection;
	BOOL redraw = FALSE;

	newSelection = (SUBJECT *)GetObject(newlySelected);
	SetRectEmpty(&unionRect);
	if(!newSelection->IsSelected())
		{
		if(!(wKeyStates & MK_SHIFT))
			OnlyUnselectSubjects(hdc, &unionRect);
		newSelection->SetSelected(TRUE);
		newSelection->GetBoundingRectangle(hdc, &bounds);
		UnionRect(&unionRect, &unionRect, &bounds);
		redraw = TRUE;
		}
	else if(wKeyStates & MK_SHIFT)
		{
		newSelection->SetSelected(FALSE);
		newSelection->GetBoundingRectangle(hdc,&unionRect);
		redraw = TRUE;
		}

	if(GridIsOn())
		if(OnlySnapSelected(hdc, &bounds))
			{
			UnionRect(&unionRect, &unionRect, &bounds);
			redraw = TRUE;
			}

	if(redraw && GetWindowPtr())
		{
		((PROJECTWINDOW *)GetWindowPtr())->DrawAllSubjects(hdc,
			&unionRect);
		}


	return FALSE;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AdjustLastSubjectPosition
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Adjusts the postion of where the next subject int the
//								widow is to be placed.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void	PROJECT::AdjustLastSubjectPosition()
	{
	SUBJECT * temp;
	POINT lowPt;
	int		i=0;
	POINT positionInProject;

	lowPt.x = 0;
	lowPt.y = SUBJECTTOPSPACING;
	while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
		{
		positionInProject = temp->GetPositionInProject();
		if(positionInProject.y > SUBJECTHEIGHT + lowPt.y)
			{
			lowPt.x = positionInProject.x + SUBJECTWIDTH;
			lowPt.y = positionInProject.y;
			}
		else if(positionInProject.y + SUBJECTHEIGHT > lowPt.y)
			if(lowPt.x < positionInProject.x)
				{
				lowPt.x = positionInProject.x + SUBJECTWIDTH;
				if(positionInProject.y > lowPt.y)
					lowPt.y = positionInProject.y;
				}
		}
		lastSubjectPosition = lowPt;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CalculateSubjectPosition
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Computes the position in the project window of a new
//								subject based on the lastSubjectPosition.
//
// PARAMETERS..:  SERUID theSubject: the new subject to be placed.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void	PROJECT::CalculateSubjectPosition(
	SERUID	theSubject)
	{
	RECT	winBounds;
	POINT temp;
	SUBJECT * subjectPtr;
	POINT newPositionInProject;

	GetClientBounds(&winBounds);
	temp.x = lastSubjectPosition.x+SUBJECTSEPARATION;
	temp.y = lastSubjectPosition.y;

	//Figure new last subject position
	//First try fitting the subject in the logical bounds, if that doesn't work
	//adjust the logical bounds downward and if that doesn't work, adjust
	//the logical bounds to the right
	if(temp.x + SUBJECTWIDTH >= logBounds.x)
		if(temp.y + SUBJECTTOPSPACING + SUBJECTHEIGHT < logBounds.y)
			{
			temp.x = SUBJECTSEPARATION;
			temp.y+= (SUBJECTTOPSPACING + SUBJECTHEIGHT);
			originOffset.x=0;
			}
		else if(temp.y + SUBJECTSEPARATION + SUBJECTHEIGHT < YMAXPAGE)
			{
			temp.x = SUBJECTSEPARATION;
			temp.y += (SUBJECTTOPSPACING + SUBJECTHEIGHT);
			logBounds.y = temp.y+1;
			originOffset.x=0;
			}
		else if(temp.x + SUBJECTWIDTH < XMAXPAGE)
			{
			logBounds.x = temp.x+1;
			//start placing back at the top
			temp.y = SUBJECTTOPSPACING;
			}
		else
			{
			temp.x-=1;
			temp.y-=1;
			}
	subjectPtr = (SUBJECT *)GetObject(theSubject);
	subjectPtr->SetPositionInProject(temp);

	//if necessary snap to grid
	if(status & WRK_GRIDON)
		{
		subjectPtr->AdjustToGrid();
		}

	newPositionInProject = subjectPtr->GetPositionInProject();
	lastSubjectPosition.x = newPositionInProject.x+SUBJECTWIDTH;
	lastSubjectPosition.y = newPositionInProject.y;

	temp.x = winBounds.right - winBounds.left +1;
	temp.y = winBounds.bottom - winBounds.top +1;


	//if the new subjects position is outside the window's viewing area,
	//	ajust the originOffset so the subject will be inside the viewing area.
	if(originOffset.x + temp.x < newPositionInProject.x + SUBJECTWIDTH)
		originOffset.x = newPositionInProject.x + SUBJECTWIDTH - temp.x +1;
	if(originOffset.y + temp.y < newPositionInProject.y + SUBJECTHEIGHT)
		originOffset.y = newPositionInProject.y + SUBJECTHEIGHT -
			temp.y +1;

	if(GetWindowPtr())
		InvalidateRect(GetWindowPtr()->GetWindowHandle(), NULL, TRUE);

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CalculateSelectedSubjectOffset
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  determines the offset from where the mouse was clicked
//								for all selected subjects.  This is useful for makeing
//								sure all subjects are moved relative to their offset
//								from the mouse, and that all subjects keep their same
//								relation to each other and to the mouse cursor.
//
// PARAMETERS..:  POINT mouse: the mouse location.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::CalculateSelectedSubjectOffset(
	POINT mouse)
	{
	SUBJECT * temp;
	int i=0;

	while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
		if(temp->IsSelected())
			temp->CalculateMouseOffset(mouse);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AdjustLogBounds
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  If a selected subject is dragged outside the current
//								logical bounds for the window, the logical bounds are
//								adjusted to hold the subject.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::AdjustLogBounds()
	{
	SUBJECT * temp;
	int i=0;
	POINT tempPositionInProject;

	while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(temp->IsSelected())
			{
			tempPositionInProject = temp->GetPositionInProject();
			if(tempPositionInProject.x + SUBJECTWIDTH > logBounds.x)
				{
				logBounds.x = tempPositionInProject.x + SUBJECTWIDTH;
				if(logBounds.x > XMAXPAGE)
					{
					logBounds.x = XMAXPAGE;
					tempPositionInProject.x = XMAXPAGE - SUBJECTWIDTH;
					}
				}
			if(tempPositionInProject.y + SUBJECTHEIGHT > logBounds.y)
				{
				logBounds.y = tempPositionInProject.y + SUBJECTHEIGHT;
				if(logBounds.y > YMAXPAGE)
					{
					logBounds.y = YMAXPAGE;
					tempPositionInProject.y = YMAXPAGE - SUBJECTHEIGHT;
					}
				}
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ArrangeSubjects
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  auto arranges all subjects atarting with the upper left
//								corner of the logical bounds.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::ArrangeSubjects()
	{
	SUBJECT * temp;
	int i,j,k;
	HDC hdc;
	RECT clientRect;
	BASEWINDOW * theWindow;
	SUBJECT * *	subjectArray;
	BOOL doSort = FALSE;
	SUBJECT * nextSubject;
	int bucket;
	POINT nextPositionInPrj;
	POINT tempPositionInPrj;


	subjectArray = (SUBJECT**) GlobalAlloc(GPTR, subjectCount *	sizeof(SUBJECT * *));

	if(subjectArray)
		doSort = TRUE;

	lastSubjectPosition.x = 0;
	lastSubjectPosition.y = SUBJECTTOPSPACING;
	originOffset.x = 0;
	originOffset.y = 0;
	theWindow = GetWindowPtr();
	if(theWindow)
		GetClientBounds(&clientRect);

	if(doSort)
		{
		//Vertical order takes precedence over horizontal order.
		//Therefore, if a subject has a lower vertical coordinate, it will come
		//before subjects with higher vertical coordinates and lower horizontal
		//coordinates.
		//Use a simple bucket sort. Go through the subjects finding the first
		//subject and put it in bucket 1 and so on.
		//We don't really need a fast sort here.

		//Clear the sorted flags
		i=0;
		while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
			temp->ClearSortedFlag();

		//Go through the list searching for the next subject and place in
		//the appropriate slot in the array.  Set the sorted flag on the
		//subject after placing in the array.
		//We need 1 iteration of the for loop for each subject
		//bucket is the index of the current bucket we're trying to fill.
		bucket = 0;
		for(j=0;j<subjectCount;j++)
			{
			k=0;
			nextSubject=NULL;
			//Get first unsorted subject;
			while(0!= (temp = (SUBJECT *)GetObject(subjectBlock, k)))
				if(!temp->TestSortedFlag())
					{
					//we found the first unsorted subject so break;
					nextSubject = temp;
					nextPositionInPrj = temp->GetPositionInProject();
					break;
					}

			//now compare the found subject with the following unsorted subjects
			//until we have the next subject to place in the window
			while(0!=(temp = (SUBJECT *)GetObject(subjectBlock, k)))
				{
				if(!temp->TestSortedFlag())
					{
					tempPositionInPrj = temp->GetPositionInProject();
					if(tempPositionInPrj.y < (nextPositionInPrj.y -
						PRJ_SJTSORTTOLERANCE) ||
						(tempPositionInPrj.y >= (nextPositionInPrj.y -
						PRJ_SJTSORTTOLERANCE) && tempPositionInPrj.y <=
						(nextPositionInPrj.y + PRJ_SJTSORTTOLERANCE) &&
						tempPositionInPrj.x < nextPositionInPrj.x))
						{
						nextPositionInPrj = tempPositionInPrj;
						nextSubject = temp;
						}
					}
				}

			//now place the subject in the array and set the sorted flag and
			subjectArray[bucket] = nextSubject;
			nextSubject->SetSortedFlag();
			bucket++;
			}

		//reset the logical bounds to the visible screen size
		if(theWindow)
			{
			logBounds.x = clientRect.right;
			logBounds.y = clientRect.bottom;
			}
		//Now calculate the new positions of the subjects according to the order
		//in the array
		for(i=0;i<subjectCount;i++)
			{
			if(subjectArray[i] != 0)
				{
				CalculateSubjectPosition(GetUID(subjectArray[i]));
				}
			}

		GlobalFree((HGLOBAL) subjectArray);
		}
	else 	//we couldn't create the array so arrange according to the order
				//of the subjects in the subjectBlock
		{
		i=0;
		while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
			{
			CalculateSubjectPosition(GetUID(temp));
			}
		}
	if(theWindow)
		{
		hdc = GetDC(theWindow->GetWindowHandle());
		((PROJECTWINDOW *)theWindow)->DrawAllSubjects(hdc, &clientRect);
		ReleaseDC(theWindow->GetWindowHandle(), hdc);
		}
	ResetLogicalBounds();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SnapLogBounds
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Adjusts the rightmost and bottom logical bounds so
//								that they do not extend past the right and bottom edges
//								respectively of the bounding rectangle for the subjects.
//								This prevents the user from being abled to scroll out into
//								the boondocks while searching for a subject that is not
//                there.
//
// PARAMETERS..:  int howToSnap:  VRANGE tells to adjust the vertical
//																logical bounds.
//																HRANGE tells to adjust the horizontal
//																logical bounds.
//
// RETURN VALUE:                  unsigned int:  tells inwhich direction the
//																logical bounds was actually snapped.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
unsigned int PROJECT::SnapLogBounds(unsigned int howToSnap)
	{
	RECT boundingRect;
	RECT unionRect;
	RECT holdUnion;
	RECT winRect;

	SUBJECT * temp;
	int i=0;
	unsigned adjust=0;
	HDC hdc;

	hdc = CreateDC("DISPLAY",NULL,NULL,NULL);
	SetRectEmpty(&boundingRect);
	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
			temp->GetBoundingRectangle(hdc, &unionRect);
			holdUnion = boundingRect;
			UnionRect(&boundingRect, &holdUnion,
				&unionRect);
		}
	DeleteDC(hdc);
	OffsetRect(&boundingRect, originOffset.x, originOffset.y);
	GetClientBounds(&winRect);
	if(howToSnap & HRANGE)
		if(boundingRect.right < logBounds.x)
			{
			adjust |= HRANGE;
			logBounds.x = boundingRect.right;
			if(logBounds.x < originOffset.x + winRect.right)
				{
				logBounds.x = originOffset.x + winRect.right;
				}
			}
	if(howToSnap & VRANGE)
		if(boundingRect.bottom < logBounds.y)
			{
			adjust |=VRANGE;
			logBounds.y = boundingRect.bottom;
			if(logBounds.y < originOffset.y + winRect.bottom)
				{
				logBounds.y = originOffset.y + winRect.bottom;
				}
			}
	return(adjust);
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::MakeBackUp
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PROJECT::MakeBackUp(
	LPCSTR oldFile)
	{
	SFilePath	newFile;
	BOOL result = TRUE;
	SFormattedMsg	msg;
	DWORD attrib;

	if(PROGSYSTEM::TestMakeBackUpsPref())
		{
		newFile = oldFile;
		newFile.SetExtension(PROGSYS_BACKUPEXTENSION);

		if(lstrcmp(oldFile, newFile))
			{
			//make sure original not read only
			attrib = GetFileAttributes(oldFile);

			if((attrib == 0xFFFFFFFF) || (attrib & FILE_ATTRIBUTE_READONLY))
				{
				msg.Format("Unable to save.  The Project file is read only.");
				
				MessageBeep(0);	
				MessageBox(NULL, msg, "File Error", MB_TASKMODAL |
					MB_ICONEXCLAMATION | MB_OK);

				result = FALSE;
				}
			else
				{
				DeleteFile(newFile);	
				if (!MoveFile(oldFile, newFile))
					{
					msg.Format("Error creating backup file.");
					
					switch(GetLastError() & 0xFFFF)
						{
						case ERROR_FILE_NOT_FOUND:
						case ERROR_PATH_NOT_FOUND:
							msg.AppendFmt("  No such File or Directory");
							break;

						case ERROR_ACCESS_DENIED:
							msg.AppendFmt("  Permission Denied");
							break;

						default:
							msg.AppendFmt("  Unknown error");
							break;
						}

					msg.AppendFmt("\r\nSave Anyway?");

					MessageBeep(0);
					if(MessageBox(NULL, msg, "File Error", MB_TASKMODAL |
						MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
						result = FALSE;

					}
				else
					{
					//sync up file attributes
					SetFileAttributes(oldFile, attrib);

					//mark for backup
					SetFileAttributes(newFile, FILE_ATTRIBUTE_ARCHIVE);
					}
				}
			}
		else
			{
			msg.Format("The file being saved cannot have the same name as"
				" the backup file, '%s'", (const char*) newFile);
			result = FALSE;
			MessageBeep(0);
			MessageBox(NULL, msg, "File Error", MB_TASKMODAL |
				MB_ICONEXCLAMATION | MB_OK);
			}
		}

	return result;
	} 

																			 

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Save
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  called when save is chosen from the menu.  Calls
//								SaveToDisk unless the subject is not named, then calls
//								SaveAs.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::Save()
	{
	HANDLE fileHandle;
	SFilePath		completePath;
	BOOL result = FALSE;

	if(fileObjPtr->TestSaveAsFlag())
		{
		if(SaveAs())
			{
			result = TRUE;
			fileObjPtr->ClearSaveAsFlag();
			}
		}
	else
		{
		completePath = (LPCSTR)fileObjPtr->GetFilePath();
		completePath.AddToEnd(GetName());
				
		if(MakeBackUp(completePath))
			{
			fileHandle = CreateFile(completePath, GENERIC_WRITE, 0, NULL,
												CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

			I_ASSERT(fileHandle != INVALID_HANDLE_VALUE);
		
			if(SaveToDisk(fileHandle))
				result = TRUE;
			}
		}
	return result;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SaveToDisk
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  If the project is marked as dirty, it is saved to disk and
//								marked as clean.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::SaveToDisk(HANDLE fileHandle)
	{
	LONG		lTmp;
	LONG		lTmp1;
	HCURSOR oldCursor;

	if(fileHandle != INVALID_HANDLE_VALUE)
		{
		PurgeBadUnusedTypes();

		oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

		lTmp  = 0;
		lTmp1 = AOBJ_PROJECTSAVE;
		SendMessageToObject(AOBJ_PREWRITE, lTmp, lTmp, lTmp1);

		ShowCursor(TRUE);
		OrderWindows();
		MakeClean();
		SetFilePointer(fileHandle, 0L, NULL, FILE_BEGIN);
		fileObjPtr->Write(fileHandle);
		CloseHandle(fileHandle);

		// Reinitialize variables since an object could have changed them
		lTmp  = 0;
		lTmp1 = AOBJ_PROJECTSAVE;
		SendMessageToObject(AOBJ_POSTWRITE, lTmp, lTmp, lTmp1);

		SetCursor(NULL);						// remove cursor from screen
		SetCursor(oldCursor);
		return TRUE;
		}
	else
		return FALSE;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SaveAs
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  prompts the user for a name for the project.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::SaveAs()
	{
	static char filters[]	= {"AppWare Projects(*."PROJECT_EXTENSION")""\0*."
													PROJECT_EXTENSION"\0"};
	int						i					= 0;
	BOOL					result		= FALSE;
	HANDLE				fileHandle;
	SFilePath			filePath;
	SFilePath			fileName;
	SUBJECT*			temp;
	BASEWINDOW*		theWindow;
	OPENFILENAME	ofn;

	filePath = fileObjPtr->GetFilePath();
	filePath.AddToEnd(GetName());

	InitOpenFileName(&ofn, PROGSYSTEM::hWndFrame, filters, filePath,
									"Save File As",
									OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
									OFN_PATHMUSTEXIST | OFN_SHOWHELP,
									PROJECT_EXTENSION, TRUE);

	if (GetSaveFileName(&ofn))
		{
		fileHandle = CreateFile(filePath, GENERIC_WRITE, 0, NULL,
													CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		I_ASSERT(fileHandle != INVALID_HANDLE_VALUE);

		filePath.GetFullName(fileName);
		filePath.RemoveFromEnd();

		SetName(fileName);
		SetPath(filePath);

		if(SaveToDisk(fileHandle) && (0 != (theWindow = GetWindowPtr())))
			{
			SetWindowText(theWindow->GetWindowHandle(), GetName());
			fileObjPtr->ClearSaveAsFlag();
			if (MDI::GetNavigatorPtr())
				MDI::GetNavigatorPtr()->
				 SetItemName(GetUID(this), this->fileObjPtr, GetName());
			while(0 != (temp=(SUBJECT *)GetObject(subjectBlock, i)))
				{
				temp->SetSubjectWindowName();
				}
			result = TRUE;
			}
		}
	return result;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SaveAsExecutable
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  prompts the user for a name for the project.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::SaveAsExecutable()
	{
	static char    filters[]={"AppWare Apps. (*.exe)\0*.exe\0"};

	HANDLE					hAppFile;
	SFilePath				filePath;
	SFilePath				fileName;
	BOOL						result = TRUE;
	unsigned long		exeStructOffset;
	HCURSOR					oldCursor;
	long						compTime;
	HANDLE					hShellFile;
  long						lTmp;
	DWORD						nWrite;
	OPENFILENAME		ofn;
	char						szMessage[256];
	SFormattedMsg		longMsg;

	//check to make sure that almkrnl.dat exists
  //First try looking in the same directory as the exe
  GetModuleFileName(PROGSYSTEM::hInst, szMessage, sizeof(szMessage));
  filePath = szMessage;
  filePath.SetFullName(PRJ_MICROAPP_FILENAME);
  hShellFile = CreateFile(filePath, GENERIC_READ, 0, NULL,
  	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    	
	if (hShellFile == INVALID_HANDLE_VALUE)
		{
    //Now try looking in the working directory.
	 	hShellFile = CreateFile(PRJ_MICROAPP_FILENAME, GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
 	
  	if(hShellFile == INVALID_HANDLE_VALUE)
    	{
			MessageBox(NULL, "Could not open " PRJ_MICROAPP_FILENAME,
		 	"Create Application Error", MB_OK|MB_TASKMODAL);
		
			return TRUE; //we already displayed an error so return true
			}
		}

	filePath = fileObjPtr->GetFilePath();
	filePath.AddToEnd(GetName());
	filePath.SetExtension("EXE");

	InitOpenFileName(&ofn, PROGSYSTEM::hWndFrame, filters, filePath,
									"Save Executable As:",
									OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST |
									OFN_SHOWHELP,
									"EXE", TRUE);

	if (GetSaveFileName(&ofn))
		{
		hAppFile = CreateFile(filePath, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL,
													CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		I_ASSERT(hAppFile != INVALID_HANDLE_VALUE);

		filePath.GetFullName(fileName);
		filePath.RemoveFromEnd();

		oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);

		LoadString(PROGSYSTEM::hInst, STR_COMPILING, szMessage,
			sizeof(szMessage)-1);
		//NULL means use default caption
		compStatPtr = new COMPILESTATUS(PROGSYSTEM::hWndFrame, szMessage,
			GetName(), 1, 0, NULL, COMPSTAT_COMPILE);

		compTime = GetCompileTime();
		compTime = (long)((double)compTime * (1+PRJ_PREPCOMPTIME +
			PRJ_PRJCOMPTIME + PRJ_CREATEAPPTIMESLICE));
		compStatPtr->ChangeSize(compTime);
		compStatPtr->UpdateStatus((long)((double)compTime * PRJ_PREPCOMPTIME));

		I_ASSERT(PROGSYSTEM::TestCreateAppFlag() == FALSE);

		//must be set before compiling
		PROGSYSTEM::SetCreateAppFlag();

		if(Compile(compTime, PRJ_COMPILETIMESLICE, filePath))
			{
			//prepare objects for writing
			PROGSYSTEM::SetUpCompileContext();
			PROGSYSTEM::RegisterNewContext();
			
			ForceMessageToAllExeObjects(AOBJ_PREWRITE, lTmp, lTmp, lTmp);

			// Go back to design-time
			PROGSYSTEM::ClearCompileInfo();
			PROGSYSTEM::RegisterNewContext();

			LoadString(PROGSYSTEM::hInst, STR_CREATING, szMessage,
				sizeof(szMessage)-1);
			compStatPtr->ChangeName(szMessage, fileName);
			if(0 != WriteShellExe(hAppFile, hShellFile))
				{
				// Replace icon before tacking on project structures
				I_VERIFY(CloseHandle(hAppFile));

				//reconstruct path before reopening file.
				filePath.AddToEnd(fileName);
				ReplaceIconInEXE(filePath);

		 		hAppFile = CreateFile(filePath, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL,
			 									OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

				I_ASSERT(hAppFile != INVALID_HANDLE_VALUE);

				SetFilePointer(hAppFile, 0, NULL, FILE_END);
				exeStructOffset = GetFileSize(hAppFile, NULL);

				//.3 means update 30% or creates time slice
				compStatPtr->UpdateStatus((long)((double)compTime *
					PRJ_CREATEAPPTIMESLICE * .3));

				fileObjPtr->ExeWrite(hAppFile);

				compStatPtr->UpdateStatus((long)((double)compTime *
					PRJ_CREATEAPPTIMESLICE * .3));

				WriteFile(hAppFile, &exeStructOffset, sizeof(exeStructOffset), &nWrite, NULL);

				//put objects to sleep
				PROGSYSTEM::SetUpCompileContext();
				PROGSYSTEM::RegisterNewContext();
				
				ForceMessageToAllExeObjects(AOBJ_POSTWRITE, lTmp, lTmp, lTmp);
				ForceMessageToAllExeObjects(AOBJ_ASLEEP, lTmp, lTmp, lTmp);

				// Go back to design-time
				PROGSYSTEM::ClearCompileInfo();
				PROGSYSTEM::RegisterNewContext();

				CleanCompile();
				result = TRUE;
				compStatPtr->FillToEnd();
				compStatPtr->ShowFinish();

				longMsg.Format("Application %s successfully created.",
									(const char*) fileName);

				//Inform the user that we were successful in creating the app.
				MessageBox(NULL, longMsg,
					"Success!", MB_OK|MB_TASKMODAL|MB_ICONINFORMATION);
				}
			else
				{
				//an error occured while trying to write the exeshell

				PROGSYSTEM::SetUpCompileContext();
				PROGSYSTEM::RegisterNewContext();
				
				ForceMessageToAllExeObjects(AOBJ_POSTWRITE, lTmp, lTmp, lTmp);
				ForceMessageToAllExeObjects(AOBJ_ASLEEP, lTmp, lTmp, lTmp);

				// Go back to design-time
				PROGSYSTEM::ClearCompileInfo();
				PROGSYSTEM::RegisterNewContext();

				CleanCompile();
				result = FALSE;
				}
			}
		else
			result = FALSE;

		PROGSYSTEM::ClearCreateAppFlag();

		CloseHandle(hAppFile);
		ShowCursor(FALSE);
		SetCursor(oldCursor);
		delete compStatPtr;
		compStatPtr = NULL;
		}

	CloseHandle(hShellFile);
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: WriteShellExe
//
// CLASS.......: PROJECT
//
/////////////////////////////////////////////////////////////////////////////

#define SHELL_MAXBLOCKWRITE 0x8000L

unsigned long PROJECT::WriteShellExe(
	HANDLE  hAppFile,
	HANDLE  hShellFile)
	{
	void  *    pBuffer;
  long          shellSize = 0;
	DWORD					cb;

	if (hShellFile != INVALID_HANDLE_VALUE)
		{
    shellSize = GetFileSize(hShellFile, NULL);
    pBuffer = GlobalAlloc(GPTR, shellSize);
    ReadFile(hShellFile, pBuffer, shellSize, &cb, NULL);
    I_ASSERT(cb == shellSize);
    WriteFile(hAppFile, pBuffer, shellSize, &cb, NULL);
    I_ASSERT(cb == shellSize);
    GlobalFree((HGLOBAL) pBuffer);
    }
  else
  	I_ASSERT(FALSE);

  return shellSize;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ReplaceIconInEXE
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Replaces the ICON res. in a W3.x Exe file with
//                a SERIUS ICON
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: The W3.x exe has two 32*32 icons (Color and BW)
//               The Format of the exe file is as documented in MSJ journal Sept/Oct.'91
//
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

#define N_COLORS			16
#define N_XORSIZE			(32 * 32 / 2)
#define N_ANDSIZE			(32 * 32 / 8)

typedef struct
	{
	RGBQUAD							rgbColors[N_COLORS];
	char								bXorMask[N_XORSIZE];
	char								bAndMask[N_ANDSIZE];
	}	ICONRES;

//This entire routine needs work
BOOL  PROJECT::ReplaceIconInEXE(
	LPCSTR					pszExeFile
	)
	{
	BOOL						fReplaced		= FALSE;

		ICONRES			iconRes;

		CopyMemory(iconRes.rgbColors, theIcon.colorIcon.imageColors,
								sizeof(iconRes.rgbColors));
	 	CopyMemory(iconRes.bXorMask, theIcon.colorIcon.imageArray,
		 						sizeof(iconRes.bXorMask));
		CopyMemory(iconRes.bAndMask, theIcon.colorIcon.maskArray,
								sizeof(iconRes.bAndMask));
		
      
      
	  HANDLE hFile;
    HANDLE hFileMapping;
    LPVOID lpFileBase;
    PIMAGE_DOS_HEADER dosHeader;
  
    hFile = CreateFile(pszExeFile, GENERIC_READ|GENERIC_WRITE, 
    	FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                    
    if ( hFile != INVALID_HANDLE_VALUE )
			{    
    	hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
    	if ( hFileMapping == 0 )
    		{   
    		I_ASSERT(FALSE);
    		CloseHandle(hFile);
				}
			else
				{    
    		lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, 0);
    		if ( lpFileBase == 0 )
    			{
        	CloseHandle(hFileMapping);
        	CloseHandle(hFile);
    			}
      	else
      		{
        	//we have a successfull file mapping.
    			dosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
    			if(dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
          	{
          	//Replace the icon
          	apb_ReplaceIconResource(lpFileBase, "ShellIcon",
          		&iconRes, sizeof(iconRes));
          	}
    			UnmapViewOfFile(lpFileBase);
    			CloseHandle(hFileMapping);
    			CloseHandle(hFile);
        	}
    		}	    
			}
    else
    	I_ASSERT(FALSE);

	return fReplaced;
	}


BOOL PROJECT::ImportUPSF()
	{
	char buffer[128];
	char holdBuffer[128];
	LONG bufSize;
	ATYPEID universalType;
	long result;
	int fieldCount;
	BOOL visible;
	SUBJECT * subjectPtr;
	int i;
	SUBJECT * holdSubject;
	long bmpSize;
	long maskSize;
	int newOrder;
	SFormattedMsg msgBuffer;
	char timeBuffer[32];
	char dateBuffer[32];
	LCID	lcid;

	//WARNING:  scopeStackPtr MUST be initialized to NULL
	scopeStackPtr = NULL;

	lcid = GetUserDefaultLCID();
	GetTimeFormat(lcid, 0, NULL, NULL, timeBuffer, sizeof(dateBuffer));
	GetDateFormat(lcid, 0, NULL, NULL, dateBuffer, sizeof(dateBuffer));

	//time stamp the log file
	msgBuffer.Format("date: %s     time: %s", dateBuffer, timeBuffer);
	AUpsfLogComment(msgBuffer);

	//read in name
	I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_NAME, 0L, AUPSF_NATV_STRING,
		(LPVOID)buffer, (long)sizeof(buffer))) != AUPSF_MEMORYERROR,
		return FALSE);
		
	if(UPSF_GOODSTRINGRESULT(result))
		{
		//First place a copy of the name in holdBuffer;
		lstrcpy(holdBuffer, buffer);

//!! Need to validate actual name

		if(lstrcmp(holdBuffer, buffer))
			{
			//the name was changed
			msgBuffer.Format("The imported project's name was "
				"changed from %s to %s to meet DOS file requirements.",
				holdBuffer, buffer);
			AUpsfLogComment(msgBuffer);
			}
		SetName(buffer);
		}
	else
		{
		//get default name and log reason for using default name
		PROJECT::MakeProjectName(buffer, PROJECT::GetIndex()+1);
		while(MDI::DuplicateProjectName((LPSTR)buffer))
			{
			//increment the index and get a new project name and continue
			//the loop until a default name is found that doesn't already
			//exist.
			PROJECT::IncIndex();
			PROJECT::MakeProjectName(buffer, PROJECT::GetIndex()+1);
      }
		PROJECT::IncIndex();
		SetName(buffer);
		apb_LogUpsfError(result, "projects name so using default");
		}

	//label the log file with the project's name
	msgBuffer.Format("Importing Project: %s", buffer);
	AUpsfLogComment(msgBuffer);

	//create a status bar for importing
	//0.02 is the starting percentage of the status bar
	//0.00 this field not used to set to NULL
	scopeStackPtr = new SCOPESTACK(PROGSYSTEM::hWndFrame,
		"Importing Project:", (LPSTR)GetName(), "AppWare Importer",
		0.02, 0.00, COMPSTAT_UPSF);

	//Add the name to the navigator
	MDI::GetNavigatorPtr()->AddItem(GetUID(this), NULLSERUID, NULLSERUID,
		fileObjPtr, GetName());

	I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_WINVERSION, 0L,
		AUPSF_NATV_INT, (LPVOID)&PROJECT::upsfImportVersion, (long)sizeof(int)))
		!=  AUPSF_MEMORYERROR, return FALSE);

	if(result != AUPSF_NOERROR)
		{
		PROJECT::upsfImportVersion = UPSF_WORKSHOP_VERSION;
		}

	if(PROJECT::upsfImportVersion != UPSF_APPWARE_VERSION)
		{
		//we're either importing a workshop file or a Mac file
		//If we're imporing a Mac file, see if it's a Appware Mac
		//file by testing for the presence of the PROJECT_OBJTYPECOUNT field.
		I_ALWAYSASSERT((result = AUpsfFieldInfo(PROJECT_OBJTYPECOUNT, 0L, AUPSF_NATV_INT,
		(LPTYPEID)&universalType, (LPLONG)&bufSize)) != AUPSF_MEMORYERROR,
		return FALSE);

		if(result == AUPSF_NOERROR)
			PROJECT::upsfImportVersion = UPSF_APPWARE_VERSION;

		}

	if(PROJECT::UpsfPreAppwareImportVersion())
		AUpsfLogComment("Importing a Serius Workshop upsf file.");

	I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_VISIBLE, 0L, 
		AUPSF_NATV_BOOL, (LPVOID)&visible, (long)sizeof(BOOL))) != AUPSF_MEMORYERROR,
		return FALSE);
	
	if(result == AUPSF_NOERROR)
		{
		if(visible == 0)
			SetIconicFlag();
		else
			ClearIconicFlag();
		}
	else
		{
		ClearIconicFlag();
		apb_LogUpsfError(result, "visible field");
		}

	//import the objTypeLists and funcTypeLists before importing the
	//subjects.
	if(!PROJECT::UpsfPreAppwareImportVersion())
		{
		CreateUpsfObjTypeTree();
		CreateUpsfFuncTypeTree();

		if(!ImportUpsfObjTypeNames())
			return FALSE;
			
		if(!ImportUpsfFuncTypeNames())
			return FALSE;
		}


	I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_SUBJECTCOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&fieldCount, (long)sizeof(int))) 
		!= AUPSF_MEMORYERROR, return FALSE);

	if(result != AUPSF_NOERROR)
		{
		fieldCount = 0;
		apb_LogUpsfError(result, "project's subject Count");
		}
	else if(PROJECT::UpsfPreAppwareImportVersion())
		{
		//in workshop, the count was zero based (0 meant 1 subject)
		fieldCount++;
		}

	newOrder=0;
	for(i=0;i<fieldCount;i++)
		{
		//read in subjects here

		I_ALWAYSASSERT((result = AUpsfOpenScope(PROJECT_SUBJECTDATA, (long)i)) 
			!= AUPSF_FILEERROR, return FALSE);
		
		if(result == AUPSF_NOERROR)
			{
			//create default subject with a "" name since it will receive a
			//name in the subject's ImportUPSF routine.
 			subjectPtr = new(subjectBlock) SUBJECT("", GetUID(this),
				SJT_UPSFCONSTRUCTOR);
			
			PROJECT::PushScopeStack(ST_9TENTHS);
			//temporarily set the active subject to the currenly importing subject
			//so the oip routines will work.
			//WARNING: the active subject must be restored.
			holdSubject = activeSubject;
			activeSubject = subjectPtr;
			
			if(!subjectPtr->ImportUPSF())
				{
				activeSubject = holdSubject;
				AUpsfCloseScope();
				return FALSE;
				}
			
			activeSubject = holdSubject;
			
			if(!subjectPtr->TestIconicFlag())
				{
				subjectPtr->SetOrder(newOrder);
				newOrder++;
				}
			
			PROJECT::PopScopeStack();
			AUpsfCloseScope();
			}
		else
			{
			apb_LogUpsfError(result, "subject data scope", APB_UPSFOPEN);
			}
		}
	SetOrder(newOrder);


	//import color bitmap
	I_ALWAYSASSERT((result = AUpsfFieldInfo(PROJECT_COLORICON, 0L, AUPSF_NATV_BITMAPINFO,
		(LPTYPEID)&universalType, (LPLONG)&bmpSize)) != AUPSF_MEMORYERROR,
		return FALSE);

	if(result == AUPSF_NOERROR)
		{
		I_ALWAYSASSERT((result = AUpsfFieldInfo(PROJECT_COLORMASK, 0L, AUPSF_NATV_BITMAPINFO,
		(LPTYPEID)&universalType, (LPLONG)&maskSize)) != AUPSF_MEMORYERROR,
		return FALSE);
		
		if(result == AUPSF_NOERROR)
			{
			//make sure size of color bitmap matches windows format
			if((bmpSize == (long)(IMAGEARRAYSIZE + 16*sizeof(RGBQUAD) +
				sizeof(BITMAPINFOHEADER))) && maskSize == ((long)(MASKARRAYSIZE +
				2*sizeof(RGBQUAD) + sizeof(BITMAPINFOHEADER))))
				{
				I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_COLORICON, 0L, AUPSF_NATV_BITMAPINFO,
					(LPVOID)&(theIcon.colorIcon.imageHdr), bmpSize)) != AUPSF_MEMORYERROR,
					return FALSE);

				if(result == AUPSF_NOERROR)
					{
					I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_COLORMASK, 0L, AUPSF_NATV_BITMAPINFO,
						(LPVOID)&(theIcon.colorIcon.maskHdr), maskSize)) != AUPSF_MEMORYERROR,
						return FALSE);
					}
				}
			else
				AUpsfLogComment("Could not import the project's color icon "
					"since the size did not match the Window's format.");

			}
		else
			apb_LogUpsfError(result, "the projects color icon");
		}
	else
		apb_LogUpsfError(result, "the project's color icon mask");

	//import black and white bitmap
	I_ALWAYSASSERT((result = AUpsfFieldInfo(PROJECT_BWICON, 0L, AUPSF_NATV_BITMAPINFO,
		(LPTYPEID)&universalType, (LPLONG)&bmpSize)) != AUPSF_MEMORYERROR,
		return FALSE);

	if(result == AUPSF_NOERROR)
		{
		I_ALWAYSASSERT((result = AUpsfFieldInfo(PROJECT_BWMASK, 0L, AUPSF_NATV_BITMAPINFO,
		(LPTYPEID)&universalType, (LPLONG)&maskSize)) != AUPSF_MEMORYERROR,
		return FALSE);

		if(result == AUPSF_NOERROR)
			{
			//make sure size of black and white bitmap matches windows format
			if((bmpSize == (long)(MASKARRAYSIZE + 2*sizeof(RGBQUAD) +
				sizeof(BITMAPINFOHEADER))) && maskSize == ((long)(MASKARRAYSIZE +
				2*sizeof(RGBQUAD) + sizeof(BITMAPINFOHEADER))))
				{
				I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_BWICON, 0L, AUPSF_NATV_BITMAPINFO,
					(LPVOID)&(theIcon.bwIcon.imageHdr), bmpSize)) != AUPSF_MEMORYERROR,
					return FALSE);

				if(result == AUPSF_NOERROR)
					{
					I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_BWMASK, 0L, AUPSF_NATV_BITMAPINFO,
						(LPVOID)&(theIcon.bwIcon.maskHdr), maskSize)) != AUPSF_MEMORYERROR,
						return FALSE);
					}
				}
			else
				AUpsfLogComment("Could not import the project's black and "
					"white icon since the size did not match the Window's format.");

			}
		else
			apb_LogUpsfError(result, "the project's black and white icon");
		}
	else
		apb_LogUpsfError(result, "the project's black and white icon mask");

	//Call post import routines
	i=0;
	while(0!=(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		//open the subject scope again so we can push on the scope for
		//the status bar
		I_ALWAYSASSERT((result = AUpsfOpenScope(PROJECT_SUBJECTDATA, (long)i-1)) 
			!= AUPSF_FILEERROR, return FALSE);
		
		if(result == AUPSF_NOERROR)
			PROJECT::PushScopeStack(ST_1TENTH);

		if(!subjectPtr->PostUPSFImport())
			return FALSE;

		if(result == AUPSF_NOERROR)
			{
			PROJECT::PopScopeStack();
			AUpsfCloseScope();
			}
		}

	if(!PROJECT::UpsfPreAppwareImportVersion())
		{
		DestroyUpsfObjTypeTree();
		DestroyUpsfFuncTypeTree();
		}
	
	delete scopeStackPtr;
	//now set the scopeStackPtr to NULL since it no longer exits
	scopeStackPtr = NULL;
	return TRUE;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::ImportUpsfObjTypeNames
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// ASSUMPTIONS:		THE upsfObjTypeTree must be created before 
//								this routine is entered!!!
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PROJECT::ImportUpsfObjTypeNames()
	{
	long result;
	int fieldCount;
	int i;
	char buffer[128];
	long typeID;

	//there should be lists to import	so anything other than AUPSF_NOERROR
	//is unexpected.
	I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_OBJTYPECOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&fieldCount, (long)sizeof(int))) == 
		AUPSF_NOERROR, return FALSE);
		
	if(result != AUPSF_NOERROR)
		apb_LogUpsfError(result, "the object type count or type list");
	else
		{
		for(i=0;i<fieldCount;i++)
			{
			I_ALWAYSASSERT((result = AUpsfOpenScope(PROJECT_OBJTYPENAME, (long)i))
				!= AUPSF_FILEERROR, return FALSE);

			if(result == AUPSF_NOERROR)
				{
				PROJECT::PushScopeStack(ST_FULL);
				
				//import the type name
				I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_NAMEFIELD, 0L,
					AUPSF_NATV_STRING, (LPVOID)buffer, (long)sizeof(buffer))) !=
					AUPSF_MEMORYERROR, return FALSE);

				if(UPSF_GOODSTRINGRESULT(result))
					{
					//import the typeID
					I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_TYPEIDFIELD,
						0L, AUPSF_NATV_LONG, (LPVOID)&typeID, (long)sizeof(long))) !=
						AUPSF_MEMORYERROR, return FALSE);
					}

				I_ASSERT(result == AUPSF_NOERROR);

				if(result != AUPSF_NOERROR)
					apb_LogUpsfError(result, "an objType/name pair");
				else
					AddUpsfObjType(typeID, buffer);

				PROJECT::PopScopeStack();
				AUpsfCloseScope();
				}
			else
				apb_LogUpsfError(result, "objTypeName scope", APB_UPSFOPEN); 
			}
		}

	return TRUE; //no fatal error if we made it to here.
	}			

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::ImportUpsfFuncTypeNames
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// ASSUMPTIONS:		The upsfFuncTypeTree must be created before
//								this routine is entered.
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PROJECT::ImportUpsfFuncTypeNames()
	{
	long result;
	int fieldCount;
	int i;
	char buffer[128];
	long typeID;

	//there should be lists to import	so anything other than AUPSF_NOERROR
	//is unexpected.
	I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_FUNCTYPECOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&fieldCount, (long)sizeof(int))) == 
		AUPSF_NOERROR, return FALSE);
		
	if(result != AUPSF_NOERROR)
		apb_LogUpsfError(result, "the function type count of type list");
	else
		{
		for(i=0;i<fieldCount;i++)
			{
			I_ALWAYSASSERT((result = AUpsfOpenScope(PROJECT_FUNCTYPENAME, (long)i))
				!= AUPSF_FILEERROR, return FALSE);
	
			if(result == AUPSF_NOERROR)
				{
				PROJECT::PushScopeStack(ST_FULL);

				//import the type name
				I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_NAMEFIELD, 0L,
					AUPSF_NATV_STRING, (LPVOID)buffer, (long)sizeof(buffer))) !=
					AUPSF_MEMORYERROR, return FALSE);

				if(UPSF_GOODSTRINGRESULT(result))
					{
					//import the typeID
					I_ALWAYSASSERT((result = AUpsfImportField(PROJECT_TYPEIDFIELD,
						0L, AUPSF_NATV_LONG, (LPVOID)&typeID, (long)sizeof(long))) !=
						AUPSF_MEMORYERROR, return FALSE);
					}

				I_ASSERT(result == AUPSF_NOERROR);

				if(result != AUPSF_NOERROR)
					apb_LogUpsfError(result, "a funcType/name pair");
				else
					AddUpsfFuncType(typeID, buffer);

				PROJECT::PopScopeStack();
				AUpsfCloseScope();
				}
			else
				apb_LogUpsfError(result, "funcTypeName scope", APB_UPSFOPEN); 
			}
		}

	return TRUE; //no fatal error if we made it to here.
	}			


//NOTE: only worry about checking for out of disk space when returning
//from exporting a subject and when closing the export file - this will keep
//the code simpler and calls to UPSF even when we're already out of disk
//space won't hurt anything.
WORD PROJECT::ExportUPSF()
	{
	static	char filters[]={"AppWare UPSF files(*."UPSF_EXTENSION")""\0*."
												UPSF_EXTENSION"\0"};
	SFilePath		filePath;
	int i;
	SUBJECT * temp;
	long result;
	BOOL visible;
	int count;
	SUBJECT * holdSubject;
	HCURSOR oldCursor;
	long totalSize;
	WORD returnValue = EXPORT_NOERROR;
	LPUPSFTYPE node;
	LPOBJTYPE typePtr;
	long tempLong;
	OPENFILENAME	ofn;

	compStatPtr = NULL; //WARNING: must be initialized to NULL
	oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);

	filePath = fileObjPtr->GetFilePath();
	filePath.AddToEnd(GetName());
	filePath.SetExtension(UPSF_EXTENSION);

	InitOpenFileName(&ofn, PROGSYSTEM::hWndFrame, filters, filePath,
									"Export Project As:",
									OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST |
									OFN_SHOWHELP | OFN_NOCHANGEDIR,
									UPSF_EXTENSION, TRUE);

	if (GetSaveFileName(&ofn))
		{
		//Create status bar with bar with 0 init points
		compStatPtr = new COMPILESTATUS(PROGSYSTEM::hWndFrame,
			"Exporting Project:",
			GetName(),
			1L,
			0L,
			"AppWare Exporter", COMPSTAT_UPSF);
		
		CreateUpsfFuncTypeTree();
		
		totalSize = GetExportTime();

		compStatPtr->ChangeSize(totalSize);
		
		compStatPtr->UpdateStatus((long)((double)totalSize * PRJ_EXPORTPREP));
		
		result = upsfInitExport(filePath);
	
		I_ASSERT(result == AUPSF_NOERROR);
		
		if(result == AUPSF_NOERROR)
			{
			I_ALWAYSASSERT(AUpsfExportField(PROJECT_NAME, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
				GetName(), (long)lstrlen(GetName())) == AUPSF_NOERROR, 
				returnValue |= EXPORT_GENERALERROR);
			
			//export current version
			i = UPSF_APPWARE_VERSION;
			I_ALWAYSASSERT(AUpsfExportField(PROJECT_WINVERSION, 0L, AUPSF_NATV_INT,
				AUPSF_UNIV_INTEGER, (LPVOID)&i, (long)sizeof(int)) == AUPSF_NOERROR,
				returnValue |= EXPORT_GENERALERROR);
			
			
			//can't have an invisible project window so if project window
			//is iconic, then set visible to FALSE;
			if(TestIconicFlag())
				visible=FALSE;
			else
				visible=TRUE;
			
			I_ALWAYSASSERT(AUpsfExportField(PROJECT_VISIBLE, 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL,
				(LPVOID)&visible, (long)sizeof(BOOL)) == AUPSF_NOERROR,
				returnValue |= EXPORT_GENERALERROR);

			i=0;
			while(!(returnValue & EXPORT_OUTOFDISK) &&
				0!=(temp = (SUBJECT *)GetObject(subjectBlock, i)))
				{
				result = AUpsfCreateScope(PROJECT_SUBJECTDATA, (long)(i-1));
				
				I_ASSERT(result == AUPSF_NOERROR);
				
				if(result == AUPSF_NOERROR)
					{
					//temporarily set the active subject so oip routines will work
					//WARNING: The active subject must be restored
					holdSubject = activeSubject;
					activeSubject = temp;
					returnValue |= temp->ExportUPSF();
					activeSubject = holdSubject;
					AUpsfCloseScope();
					}
				else if(result == AUPSF_OUTOFDISKSPACE)
					returnValue |= EXPORT_OUTOFDISK;
				else
					returnValue |= EXPORT_GENERALERROR;
				}

			//decrease i since i is 1 more than the number exported.
			i--;

			I_ALWAYSASSERT(AUpsfExportField(PROJECT_SUBJECTCOUNT, 0L, AUPSF_NATV_INT,
				AUPSF_UNIV_INTEGER, (LPVOID)&i, (long)sizeof(int)) == 
				AUPSF_NOERROR,
				returnValue|=EXPORT_GENERALERROR);

			//export the objtype list
			//Depends on the totalObjCount being the count of the number of
			//objects of that type in the project.  If this count ever changes,
			//we need to build the upsfObjTypeTree and export that tree.
			i=0;
			count=0;
			while(0 != (typePtr = (LPOBJTYPE)GetObject(typeBlock, i)))
				{
				if(typePtr->GetTotalObjCount() > 0)
					{
					result = AUpsfCreateScope(PROJECT_OBJTYPENAME, count);
						
					//make sure the scope is created before incrementing the count.
					count++;
					
					I_ASSERT(result == AUPSF_NOERROR);

					if(result == AUPSF_NOERROR)
						{
						I_ALWAYSASSERT(AUpsfExportField(PROJECT_NAMEFIELD, 0L, AUPSF_NATV_STRING,
							AUPSF_UNIV_TEXT, typePtr->GetTypeName(), 
							(long)lstrlen(typePtr->GetTypeName())) ==  AUPSF_NOERROR,
							returnValue|= EXPORT_GENERALERROR);

						tempLong = typePtr->GetTypeID();

						I_ALWAYSASSERT(AUpsfExportField(PROJECT_TYPEIDFIELD, 0L, AUPSF_NATV_LONG,
							AUPSF_UNIV_INTEGER, (LPVOID)&tempLong, (long)sizeof(long)) ==
							AUPSF_NOERROR, returnValue |= EXPORT_GENERALERROR);

						AUpsfCloseScope();
						}
					else
						{
						returnValue|= EXPORT_GENERALERROR;
						}
					}
				}

			I_ALWAYSASSERT(AUpsfExportField(PROJECT_OBJTYPECOUNT, 0L, AUPSF_NATV_INT,
				AUPSF_UNIV_INTEGER, (LPVOID)&count, (long)sizeof(int)) ==
				AUPSF_NOERROR, returnValue |= EXPORT_GENERALERROR);

			compStatPtr->UpdateStatus((long)((double)totalSize * PRJ_HALFTYPELISTS));
		
			//export the functype list
			UpsfFuncTypePrepareForGetting();

			node=NULL;
			i=0;
			while(NULL != (node = GetNextUpsfFuncType(node)))
				{
				result = AUpsfCreateScope(PROJECT_FUNCTYPENAME, i);
				
				I_ASSERT(result == AUPSF_NOERROR);

				if(result == AUPSF_NOERROR)
					{
					//make sure the scope is opened before incrementing i.
					i++;

					I_ALWAYSASSERT(AUpsfExportField(PROJECT_NAMEFIELD, 0L, AUPSF_NATV_STRING,
						AUPSF_UNIV_TEXT, (LPCSTR)node->typeName, 
						(long)lstrlen((LPCSTR)node->typeName)) ==  AUPSF_NOERROR,
						returnValue|= EXPORT_GENERALERROR);

					I_ALWAYSASSERT(AUpsfExportField(PROJECT_TYPEIDFIELD, 0L, AUPSF_NATV_LONG,
						AUPSF_UNIV_INTEGER, (LPVOID)&node->typeID, (long)sizeof(long)) ==
						AUPSF_NOERROR, returnValue |= EXPORT_GENERALERROR);

					AUpsfCloseScope();
					}
				else
					returnValue |= EXPORT_GENERALERROR;
				}

			I_ALWAYSASSERT(AUpsfExportField(PROJECT_FUNCTYPECOUNT, 0L, AUPSF_NATV_INT,
				AUPSF_UNIV_INTEGER, (LPVOID)&i, (long)sizeof(int)) ==
				AUPSF_NOERROR,
				returnValue |= EXPORT_GENERALERROR);

			compStatPtr->UpdateStatus((long)((double)totalSize * PRJ_HALFTYPELISTS));

			//Export the Color Icon bitmap
			I_ALWAYSASSERT(AUpsfExportField(PROJECT_COLORICON, 0L, AUPSF_NATV_BITMAPINFO,
				AUPSF_UNIV_BITMAP, (LPVOID)&(theIcon.colorIcon.imageHdr),
				(long)(IMAGEARRAYSIZE + sizeof(BITMAPINFOHEADER) +
				16 * sizeof(RGBQUAD))) == AUPSF_NOERROR,
				returnValue |= EXPORT_GENERALERROR);

			//Export the Color Icon Mask
			I_ALWAYSASSERT(AUpsfExportField(PROJECT_COLORMASK, 0L, AUPSF_NATV_BITMAPINFO,
				AUPSF_UNIV_BITMAP, (LPVOID)&(theIcon.colorIcon.maskHdr),
				(long)(MASKARRAYSIZE + sizeof(BITMAPINFOHEADER) +
				2 * sizeof(RGBQUAD))) == AUPSF_NOERROR,
				returnValue |= EXPORT_GENERALERROR);

			//Export the Black and White Icon bitmap
			I_ALWAYSASSERT(AUpsfExportField(PROJECT_BWICON, 0L, AUPSF_NATV_BITMAPINFO,
				AUPSF_UNIV_BITMAP, (LPVOID)&(theIcon.bwIcon.imageHdr),
				(long)(MASKARRAYSIZE + sizeof(BITMAPINFOHEADER) +
				2 * sizeof(RGBQUAD))) == AUPSF_NOERROR,
				returnValue |= EXPORT_GENERALERROR);

			//Export the Black and White Icon Mask
			I_ALWAYSASSERT(AUpsfExportField(PROJECT_BWMASK, 0L, AUPSF_NATV_BITMAPINFO,
				AUPSF_UNIV_BITMAP, (LPVOID)&(theIcon.bwIcon.maskHdr),
				(long)(MASKARRAYSIZE + sizeof(BITMAPINFOHEADER) +
				2 * sizeof(RGBQUAD))) == AUPSF_NOERROR,
				returnValue |= EXPORT_GENERALERROR);

			result = upsfCloseExport();
			
			I_ASSERT(result == AUPSF_NOERROR);

			if(result == AUPSF_OUTOFDISKSPACE)
				returnValue |= EXPORT_OUTOFDISK;
			else if(result != AUPSF_NOERROR)
				returnValue |= EXPORT_GENERALERROR;

			}
		else if(result == AUPSF_OUTOFDISKSPACE)
			{
			returnValue |= EXPORT_OUTOFDISK;
			}
		else
			{
			returnValue |= EXPORT_GENERALERROR;
			}
		}

	DestroyUpsfFuncTypeTree();

	ShowCursor(FALSE);
	SetCursor(oldCursor);

	//If we're out of disk space then put up an error message.
	//Only show the finish of the stat bar if we're not out of disk space
	//since it looks stupid if we put it up when we ran out of space.
	if(returnValue & EXPORT_OUTOFDISK)
		MessageBox(GetActiveWindow(), "There was not enough disk space to "
			"export the entire project.  The partially exported file is therefore"
			"  invalid and has been removed from the disk.",
			"UPSF Export Error", MB_ICONEXCLAMATION|MB_OK|MB_TASKMODAL);
	else if(compStatPtr)
		{
		compStatPtr->FillToEnd();
		compStatPtr->ShowFinish();
		}

	//there was some general error but the file may still be good.
	if(returnValue == EXPORT_GENERALERROR)
		MessageBox(NULL, "Errors occured while exporting the project. "
			"It still may be possible to import the file.  However, the data "
			"where errors ocurred will be lost and default values will be used.",
			"UPSF Export Error", MB_ICONEXCLAMATION|MB_OK|MB_TASKMODAL);

	if(compStatPtr)
		{
		delete compStatPtr;
		compStatPtr = NULL;
		}

	return returnValue;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::AddUpsfObjType
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PROJECT::AddUpsfObjType(
	ATYPEID typeID,
	LPCSTR typeName)
	{
	LPUPSFTYPE node;
	BOOL l_rval = FALSE;

	I_ASSERT(PROJECT::upsfObjTypeTree);

	if(PROJECT::upsfObjTypeTree != NULL)
		{
		node = new UPSFTYPE(typeID, typeName);
		l_rval = PROJECT::upsfObjTypeTree->Add(node);
		}
		
	return l_rval;	
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::GetUpsfObjTypeName
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PROJECT::GetUpsfObjTypeName(
	long typeID, 
	LPSTR buffer, 
	int bufSize)
	{
	LPUPSFTYPE node;
	BOOL rval = FALSE;

	I_ASSERT(PROJECT::upsfObjTypeTree);
	
	if(PROJECT::upsfObjTypeTree != NULL)
		{
		node = (LPUPSFTYPE)upsfObjTypeTree->FindNode(typeID);
		
		I_ASSERT(node);

		if(node)
			{
			lstrcpyn(buffer, node->typeName, bufSize);
			//for safety, make sure the string is null terminated.
			buffer[bufSize-1] = '\0';
			rval = TRUE;
			}
		}

	return rval;
	}
																			 

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::AddUpsfFuncType
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PROJECT::AddUpsfFuncType(
	ATYPEID typeID,
	LPCSTR typeName)
	{
	LPUPSFTYPE node;
	BOOL l_rval = FALSE;

	I_ASSERT(PROJECT::upsfFuncTypeTree);

	if(PROJECT::upsfFuncTypeTree != NULL)
		{
		node = new UPSFTYPE(typeID, typeName);
		l_rval = PROJECT::upsfFuncTypeTree->Add(node);
		}
		
	return l_rval;	
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::GetUpsfFuncTypeName
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PROJECT::GetUpsfFuncTypeName(
	long typeID, 
	LPSTR buffer, 
	int bufSize)
	{
	LPUPSFTYPE node;
	BOOL rval = FALSE;

	I_ASSERT(PROJECT::upsfFuncTypeTree);
	
	if(PROJECT::upsfFuncTypeTree != NULL)
		{
		node = (LPUPSFTYPE)upsfFuncTypeTree->FindNode(typeID);
		
		I_ASSERT(node);

		if(node)
			{
			lstrcpyn(buffer, node->typeName, bufSize);
			//for safety, make sure the string is null terminated.
			buffer[bufSize-1] = '\0';
			rval = TRUE;
			}
		}

	return rval;
	}
																			 

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::GetNextUpsfObjType
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LPUPSFTYPE PROJECT::GetNextUpsfObjType(
	LPUPSFTYPE node)
	{
	I_ASSERT(PROJECT::upsfObjTypeTree);

	return (LPUPSFTYPE)PROJECT::upsfObjTypeTree->GetNextSortedMember(node);
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::UpsfObjTypePrepareForGetting
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void PROJECT::UpsfObjTypePrepareForGetting()
	{
	I_ASSERT(PROJECT::upsfObjTypeTree);

	//sort preOrder so the bitree can be output in such a way
	//that it can be rebuilt into a reasonable binary tree.
	PROJECT::upsfObjTypeTree->SortPreOrder();
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::GetNextUpsfFuncType
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LPUPSFTYPE PROJECT::GetNextUpsfFuncType(
	LPUPSFTYPE node)
	{
	I_ASSERT(PROJECT::upsfFuncTypeTree);

	return (LPUPSFTYPE)PROJECT::upsfFuncTypeTree->GetNextSortedMember(node);
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::UpsfFUncTypePrepareForGetting
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void PROJECT::UpsfFuncTypePrepareForGetting()
	{
	I_ASSERT(PROJECT::upsfFuncTypeTree);

	//sort preOrder so the bitree can be output in such a way
	//that it can be rebuilt into a reasonable binary tree.
	PROJECT::upsfFuncTypeTree->SortPreOrder();
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PROJECT::GetExportTime
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
long PROJECT::GetExportTime()
	{
	long totalSize = 0;
	int i=0;
	LPSUBJECT temp;

	//determine information for status bar;
	while(0!=(temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		totalSize+=(long)((LPRECORDBLOCK)GetObject(temp->GetObjectList()))->
			GetCount();
		totalSize+=(long)((LPRECORDBLOCK)GetObject(temp->GetFunctionList()))->
			GetCount();
		totalSize+=(long)((LPRECORDBLOCK)GetObject(temp->GetCommentList()))->
			GetCount();
		totalSize+=(long)((LPRECORDBLOCK)GetObject(temp->GetObjGroupList()))->
			GetCount();
		}
	totalSize+=(long)i; //add in number of subjects
	totalSize++; //add in 1 for the project
	totalSize = (long)((double)totalSize * (1.0 + PRJ_EXPORTPREP +
		PRJ_TYPELISTS));
	
	return totalSize;	
	} 

																			 

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OrderWindows
//
// CLASS.......: PROJECT
//
// DESCRIPTION.: used to order the windows of a project as to their
//								 layering on the screen.  The topmost window has an order
//								 value of 0.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::OrderWindows()
	{
	WORD order = 0;
	BASEWINDOW * winPtr;
	HWND currentHwnd = GetTopWindow(PROGSYSTEM::hWndClient);

	//order all subjects
	while(currentHwnd != NULL)
		{
		if(((winPtr = (BASEWINDOW *)
			BASEWINDOW::list->GetObject(currentHwnd))!=NULL) &&
			(winPtr->fileObjPtr->GetFileIDStamp() == fileObjPtr->GetFileIDStamp()))
			{
			if(winPtr->TestSubjectFlag())
				{
				((SUBJECTWINDOW *)winPtr)->GetSubjectPtr()->SetOrder(order);
				order++;
				}
			}
		currentHwnd = GetNextWindow(currentHwnd, GW_HWNDNEXT);
		}
	//force the project window to be on the bottom by Ed's request
	SetOrder(order);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ShowAllWindows
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Displays all windows of a project on the screen
//									maintaining their layered order.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: Should only be called by the project constructor after a
//							 	project has been opened (loaded from file).
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::ShowAllWindows()
	{
	WORKSHEET * * arrayPtr;
	SUBJECT * temp;
	int		i;
	int j;
	BOOL pleaseContinue = TRUE;
	BOOL orderChanged = FALSE;
	int visibleCount = 0;

	//allocate array to hold pointers to all subjects and to the project
	arrayPtr = (WORKSHEET**) GlobalAlloc(GPTR, (subjectCount + 1) *
		sizeof(WORKSHEET *));

	//array acts like a bucket sort
	//the array is filled with pointers to every subject as well as the project
	//so the windows will be able to be displayed with in the same layered
	//order as they were saved

	//Set the visibleCount
	i=0;
	while(0!=(temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(!temp->TestIconicFlag())
			visibleCount++;
		}
	
	//project window always needs to be shown	
	visibleCount++;

	i=0;
	while(pleaseContinue && (0 != (temp=(SUBJECT *)GetObject(subjectBlock, i))))
		{
		if(!temp->TestIconicFlag()) //if window is visible it has some order
			{
			if(temp->GetOrder() > visibleCount-1)
				temp->SetOrder(visibleCount-1);
			if(arrayPtr[temp->GetOrder()] == NULL)
				arrayPtr[temp->GetOrder()] = (WORKSHEET *)temp;
			else
				{
				j = visibleCount-1;
				while(pleaseContinue && (arrayPtr[j] != NULL))
					{
					j--;
					if(j<0)
						pleaseContinue = FALSE;
					}
				if(pleaseContinue)
					{
					arrayPtr[j] = (WORKSHEET *)temp;
					temp->SetOrder(j);
					orderChanged = TRUE;
					}
				}
			}
		}
	if(i != subjectCount+1)
		pleaseContinue = FALSE;

	if(pleaseContinue)
		{
		if(GetOrder()>visibleCount-1)
			SetOrder(visibleCount-1);
		if(arrayPtr[GetOrder()] == NULL)
			arrayPtr[GetOrder()] = (WORKSHEET *)this;
		else
			{
			j=visibleCount-1;
			while(pleaseContinue && (arrayPtr[j] != NULL))
				{
				j--;
				if(j<0)
					pleaseContinue = FALSE;
				}
			if(pleaseContinue)
				{
				arrayPtr[j] = (WORKSHEET *)this;
				this->SetOrder(j);
				orderChanged = TRUE;
				}
			}
		}

	if(orderChanged)
		MessageBox(NULL, "Proper layering order of project and\r"
			"subject windows could not be determined.\r"
			"Default order will be used.",
		"Unable to layer windows", MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);

	if(pleaseContinue)
		{
		for(i=visibleCount-1;i>=0; i--)
			arrayPtr[i]->ShowObjectWindow();
		}
	else
		MessageBox(NULL,
			"A fatal error has occured while trying\r"
			"to display project and subject windows.\r"
			"AppWare should be shut down and run again.",
			"Fatal Error Showing Windows",
			MB_SYSTEMMODAL|MB_OK|MB_ICONEXCLAMATION);

	GlobalFree((HGLOBAL) arrayPtr);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MakeClean
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Marks a project as being clean, or in otherwords, no
//								changes need to be saved.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::MakeClean()
	{
	fileObjPtr->ClearDirtyFlag();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MakeDirty
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Marks a project as being dirty (changes have been made)
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::MakeDirty()
	{
	fileObjPtr->SetDirtyFlag();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ProjectIsDirty
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  returns true if the project needs to be saved
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::ProjectIsDirty()
	{
	return fileObjPtr->TestDirtyFlag();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetLastSubjectPosition
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  returns the lastSubjectPosition point.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
POINT PROJECT::GetLastSubjectPosition()
	{
	return lastSubjectPosition;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetPath
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  sets the projects path to whatever is pointed to by
//									by thePath.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::SetPath(
	LPCSTR thePath)
	{
	fileObjPtr->SetFilePath(thePath);
	};


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetPath
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  returns the projects path.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPCSTR PROJECT::GetPath()
	{
	return fileObjPtr->GetFilePath();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetIconBlock
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  returns the SERIUSBLOCK pointer which contains the icon
//									data.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SERIUSICON * PROJECT::GetIconBlock()
	{
	return &theIcon;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GridIsOn
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  returns TRUE if the project grid is on else returns FALSE.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::GridIsOn()
	{
	return (status & WRK_GRIDON)?TRUE:FALSE;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetGrid
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Sets the project's grid variable to whatever value is
//									contained in the parameter set.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::SetGrid(
	BOOL set)
	{
	if(set)
		status |= WRK_GRIDON;
	else
		status &= ~WRK_GRIDON;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetOriginOffset
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::SetOriginOffset(
	POINT newOffset)
	{
	originOffset = newOffset;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetOriginOffset
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
POINT PROJECT::GetOriginOffset()
	{
	return originOffset;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetLogBounds
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::SetLogBounds(
	POINT newBounds)
	{
	logBounds = newBounds;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetLogBounds
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
POINT PROJECT::GetLogBounds()
	{
	return logBounds;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  IncrementSubjectCount
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::IncrementSubjectCount()
	{
	subjectCount++;
	}

void PROJECT::DecrementSubjectCount()
	{
	subjectCount--;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetSubjectCount
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
unsigned int PROJECT::GetSubjectCount()
	{
	return subjectCount;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeProjectName
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  makes a generic project name and places it in buffer.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECT::MakeProjectName(
	LPSTR buffer,
	unsigned int index)
	{
	char nameBuffer[16];

	if(index < 10)
		LoadString(PROGSYSTEM::hInst, STR_PROJECTNAME, nameBuffer, 10);
	else if(index < 100)
		LoadString(PROGSYSTEM::hInst, STR_SHORTPROJECTNAME, nameBuffer, 10);
	else
		LoadString(PROGSYSTEM::hInst, STR_SHORTESTPROJECTNAME, nameBuffer, 10);

	wsprintf(buffer,"%s%u", nameBuffer, index);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetIndex
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  returns the projectIndex.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
unsigned int PROJECT::GetIndex()
	{
	return projectIndex;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  VerifyFile
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  checks the file to see if its a project file.
//
// PARAMETERS..:
//
// RETURN VALUE:  TRUE if the file is a project file
//
// ASSUMPTIONS.:  fileHandle must be a valid, open file. I'm assuming that
//								if the file is big enough to hold a project and if the
//								dirty field has exactly the value TRUE, the file must be
//								a project.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL  PROJECT::VerifyFile(
	HANDLE fileHandle,
	LPCSTR fileName)
	{
	DWORD					status;
	FILEIDINFO		idInfo;
	SFormattedMsg buffer;
	DWORD					nRead;

	if(!ReadFile(fileHandle, &idInfo, sizeof(FILEIDINFO), &nRead, NULL) ||
  	(nRead != sizeof(FILEIDINFO)))
		status = FILESTAT_SHORT;
	else if (lstrcmp(idInfo.idString, ID_STRING ) != 0)
		status = FILESTAT_NOTSERIUSFILE;
	else if (!(idInfo.fileType & FILETYPE_PROJECT))
		status = FILESTAT_WRONGTYPE;
	else if (idInfo.databaseVersion  != DATABASE_VERSION)
		status = FILESTAT_OLDDB;
	else
		status = FILESTAT_OK;

	SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN);

	if (status & FILESTAT_OK)
		return TRUE;
	else
		{
		buffer.Format("%s is not a valid Project file.", fileName);
		MessageBeep(0);
		MessageBox(NULL, buffer,
		 "Open File Error", MB_OK|MB_ICONEXCLAMATION | MB_TASKMODAL);
		return FALSE;
		}

	}


void PROJECT::SetName(
	LPCSTR newName)
	{
	fileObjPtr->SetName(newName);
	}

LPCSTR PROJECT::GetName()
	{
	return fileObjPtr->GetName();
	}

class LIBNAME:public UNITREEMEMBER
	{
	public:
		TSTRING name;

		int Compare(
			LPBITREEMEMBER node)
			{
			//do a non-case sensitive search
			return lstrcmpi(name, ((LIBNAME *)node)->name);
			}

	};

typedef LIBNAME * LPLIBNAME;

class STACKNAME:public LISTMEMBER
	{
	public:
		TSTRING name;

	};

typedef STACKNAME * LPSTACKNAME;

//WARNING: compStatPtr must be valid when this routine is entered
BOOL PROJECT::Compile(
	long   timePoints,
	double timeRatio,
  LPCSTR  masterPath)
	{
	SUBJECT     * temp;
	LPEXEHEAD   lpExeHead;
	int         i;
	SUBJECT * holdSubject;
	LPEXEEVENTLIST  exeEventList;
	LPEXEEVENT      exeEvent;
	LPEXELIBLIST    exeLibList;
	LPEXELIB        exeLib;
	SFilePath				packingPath;
	char      packingLine    [400];
	HANDLE    hPackingFile;
	AMEMBLOCKID bufID;
	LPSTR      bufPtr;
	int 	numDlls;
	int				j;
	LPUNIQUETREE   libTree;
	LPLIBNAME libName;
	AObjMessage sysRec;
	LPEXEOBJTYPE exeType;
	pImplicit_Link linkPtr;
	int numLinks;
	char nameBuffer[256];
	STACK * libStack;
	LPSTACKNAME stackName;
	char namePath[MAX_PATH];
	HMODULE hMod;
	BOOL unknownExtension = FALSE;
	long lTmp;
	DWORD		nWrite;
 
	if(0 == ((LPRECORDBLOCK)GetObject(subjectBlock))->GetCount())
		{
		MessageBox(NULL, "An empty project can not be compiled.",
			"Compilation Warning", MB_OK|MB_TASKMODAL|MB_ICONINFORMATION);
		return FALSE;
		}
    
  #ifdef AW_I_TIME
  DWORD compTime=0;
  DWORD checkTime = timeGetTime();
	#endif
  
	// TRUE means to update the status bar
	if (!this->CheckProject(TRUE))
		return FALSE;

	#ifdef AW_I_TIME
  checkTime = timeGetTime() - checkTime;
  compTime = timeGetTime();
	#endif
	
	fileObjPtr->SetTempObjects(new FASTRECMGR( 64, sizeof(TEMPOBJECT)));

	fileObjPtr->SetHeadExeBlock(GetUID(
		new EXERECORDBLOCK(sizeof(EXEHEAD), RT_EXEHEADREC)));

	lpExeHead = new(fileObjPtr->GetHeadExeBlock()) EXEHEAD();
	fileObjPtr->SetHeadExeRec(GetUID(lpExeHead));

	PROGSYSTEM::SetUpCompileContext();
	PROGSYSTEM::RegisterNewContext();

	if(!BuildExeTypeList())
  	return FALSE;

	//prepare the objects for copying
	lTmp  = 0;
	SendMessageToObject(AOBJ_PREWRITE, lTmp, lTmp, lTmp);

	// Compile each subject
	i=0;
	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		//make the subject being compiled the active subject so oip routines
		//will work consistently
		holdSubject = activeSubject;
		activeSubject = temp;
		//TRUE means to update the status bar
		temp->Compile(lpExeHead, TRUE);
		IncStatusBar();
		activeSubject = holdSubject;
		}

	//clean the objects from copying
	lTmp  = 0;
	SendMessageToObject(AOBJ_POSTWRITE, lTmp, lTmp, lTmp);


	// Pack down (and sort, as appropriate) all of the runtime lists

	exeEventList = lpExeHead->GetExeEventList();
	exeEventList->PackAndSortNodes();
	for (i = 0 ; 0 != (exeEvent = (LPEXEEVENT)exeEventList->IdxGetNode(i)) ; i++)
		{
		exeEvent->GetResponseList()->PackAndSortNodes();
		}

	exeLibList = lpExeHead->GetExeLibList();
	exeLibList->PackAndSortNodes();

	// Derive path to the packing list file, extension is .REQ
	if (!masterPath)
		{
		// They didn't specify a master path, so we won't build a packing list
		hPackingFile = INVALID_HANDLE_VALUE;
		}
	else
		{
		packingPath = masterPath;
		packingPath.SetExtension("REQ");

		// Open the packing list file
		hPackingFile = CreateFile(packingPath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
												CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		}

	//.3 means update 30% of projects time
	compStatPtr->UpdateStatus((long)((double)timePoints * timeRatio *
		PRJ_PRJCOMPTIME * .3));


	if (hPackingFile != INVALID_HANDLE_VALUE)
		{
		// Make a pretty time stamp
		time_t    aTimeT;
		tm       *aTm;
		time(&aTimeT);
		aTm = localtime(&aTimeT);

		// Write out a header
		lstrcpy(packingLine, asctime(aTm));
		// Smash the newline asctime sticks on
		packingLine[lstrlen(packingLine)-1] = '\0';
		lstrcat(packingLine, CRLF CRLF "Required DLLs for ");
		lstrcat(packingLine, masterPath);
		lstrcat(packingLine, CRLF CRLF);
		WriteFile(hPackingFile, packingLine, lstrlen(packingLine), &nWrite, NULL);
		}

	if (hPackingFile != INVALID_HANDLE_VALUE)
		{
		libTree = new UNIQUETREE();
		libStack = new STACK();

		I_ASSERT(libTree);

		bufID = AMemAllocate(0);

		I_ASSERT(bufID != 0);

		for (i = 0 ; 0 != (exeLib = (LPEXELIB)exeLibList->IdxGetNode(i)) ; i++)
			{
			//push the initial Dll onto the stack
			stackName = new STACKNAME();
			stackName->name = exeLib->GetName();
			libStack->Push(stackName);

			//loop until the stack is empty
			while(NULL != (stackName = (LPSTACKNAME)libStack->Pop()))
				{
				//see if the dll is already added to the tree
				//If it's not already added, extract all dependent dll's
				libName = new LIBNAME();
				libName->name = stackName->name;
				
				//popped off so delete
				delete stackName;

				if(libTree->Add(libName))
					{
					lstrcpy(nameBuffer, libName->name);

					//check if the file extension is unknown
					if(nameBuffer[lstrlen(nameBuffer) - 2] == '*')
						{
						//guess that their is a dll of this name
						nameBuffer[lstrlen(nameBuffer) - 1] = '\0';
						lstrcat(nameBuffer, ".dll");
						}

					numDlls = apb_ExtractDllLibs(nameBuffer, bufID);
					I_ASSERT(AMemGetSize(bufID) / MAX_PATH == numDlls);

					if(numDlls > 0)
						{
						bufPtr = (LPSTR)AMemLock(bufID);

						//push all extracted dll's onto the stack
						for(j=0;j<numDlls;j++)
							{
							stackName = new STACKNAME(); 
							lstrcpy(nameBuffer, bufPtr);

							hMod = GetModuleHandle(nameBuffer);
							
							if ((hMod != NULL) &&
									(GetModuleFileName(hMod, namePath, sizeof(namePath)-1)))
								{
								SFilePath		path(namePath);

								path.GetFullName(nameBuffer, sizeof(nameBuffer));
								}
							else
								{
								unknownExtension = TRUE;
								lstrcat(nameBuffer, "*");
								}
								
							stackName->name = nameBuffer;
							libStack->Add(stackName);
							bufPtr += MAX_PATH;
							}
						}
					AMemSetSize(bufID, 0);
					}
				}

			exeLib->GetProcList()->PackAndSortNodes();
			}

		//make sure the almkrnl is added
		libName = new LIBNAME();
		libName->name = "almkrnl.dll";
		libTree->Add(libName);

		//now query all types for their implicit links
		i=0;
		while(0 != (exeType = (LPEXEOBJTYPE)GetObject(lpExeHead->GetExeObjTypeList(), i)))
			{
			sysRec.message1 = AOBJ_REPORTREQUIREDFILES;
			sysRec.message2 = bufID;
			sysRec.message3 = AOBJ_INT_REPORTREQUIREDFILES;
			sysRec.message4 = 0;
			sysRec.errorMessage[0] = '\0';

			exeType->GetAdmnProc()(0L, &sysRec);
			}

		//now go through the memBlock and add all fileNames to the libTree
		if(AMemGetSize(bufID) > 0)
			{
			linkPtr = (pImplicit_Link)AMemLock(bufID);
			I_ASSERT(linkPtr);

			numLinks = (int)AMemGetSize(bufID) / sizeof(Implicit_Link);
			
			for(i=0;i<numLinks;i++)
				{
				libName = new LIBNAME();
				libName->name = (const char*) linkPtr->fileName;
				libTree->Add(libName);

				// WARNING!!  Since we are going to throw away the memory
				// after this operation, we need to call the destructor of
				// the filename.  DON'T DO ANYTHING ELSE TO THIS linkPtr!!
				linkPtr->fileName.SFilePath::~SFilePath();

				linkPtr++;
				}
			}

		libTree->SortInOrder();

		libName = NULL;

		while(NULL != (libName = (LPLIBNAME)libTree->GetNextSortedMember(libName)))
			{
			lstrcpy(packingLine, "    ");
			lstrcat(packingLine, libName->name);
			lstrcat(packingLine, CRLF);
			WriteFile(hPackingFile, packingLine, lstrlen(packingLine), &nWrite, NULL);
			}

		if(unknownExtension)
			{
			lstrcpy(packingLine, CRLF CRLF "* Unknown File Extension");
			WriteFile(hPackingFile, packingLine, lstrlen(packingLine), &nWrite, NULL);
			}

		AMemUnlock(bufID);
		AMemFree(bufID);
		delete libTree;
		delete libStack;
		}

	// Close off the packing list
	if (hPackingFile != INVALID_HANDLE_VALUE)
		{
		CloseHandle(hPackingFile);
		}

	lpExeHead->GetExeBeadList()->PackAndSortNodes();

	//.4 means update 40% of projects time
	compStatPtr->UpdateStatus((long)((double)timePoints * timeRatio *
		PRJ_PRJCOMPTIME * .4));
    
  //awaken the objects
  ForceMessageToAllExeObjects(AOBJ_AWAKENED, lTmp, lTmp, lTmp);

	// Validate all the object IDs
	i=0;
	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		//passing in TRUE means to update the status bar
		temp->UpdateUIDs(TRUE);
		IncStatusBar();
		}

	// Go back to design-time
	PROGSYSTEM::ClearCompileInfo();

	PROGSYSTEM::RegisterNewContext();

	//.3 means update 30% of projects time
	compStatPtr->UpdateStatus((long)((double)timePoints * timeRatio *
		PRJ_PRJCOMPTIME * .3));
    
  #ifdef AW_I_TIME
  char msg[256];
  compTime = timeGetTime() - compTime;
  wsprintf(msg, "Error Checking Time: %u\n\r", checkTime);
  OutputDebugString(msg);
  wsprintf(msg, "Compile Time: %u\n\r", compTime);
  OutputDebugString(msg);
  #endif  
    
	return TRUE;
	}

/////////////////////////////////////////////////////////////////
BOOL PROJECT::BuildExeTypeList()
	{
  LPOBJTYPE typePtr;
  LPEXEHEAD pExeHead;
  int i=0;
  BOOL rVal = TRUE;
  
  //first make sure number and text will be added.
  typePtr = GetTypePtr(OTYPE_TEXT);
  if(typePtr && !typePtr->GetRequiredCount())
  	typePtr->IncRequiredCount();
    
  typePtr = GetTypePtr(OTYPE_NUMBER);
  if(typePtr && !typePtr->GetRequiredCount())
  	typePtr->IncRequiredCount();
    
	pExeHead = (LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec());
  while(NULL != (typePtr = (LPOBJTYPE)GetObject(typeBlock, i)))
  	{
  	if(typePtr->GetRequiredCount() || typePtr->GetTotalObjCount())
    	{
      if(!AddExeObjType(typePtr, pExeHead))
      	{
        rVal = FALSE;
        break;
        }
      }
  	}
  
  return rVal;    
  }

/////////////////////////////////////////////////////////////////
//This routine does not check to make sure the type doesn't already
//exist.
BOOL PROJECT::AddExeObjType(
	LPOBJTYPE pObjType,
	LPEXEHEAD pExeHead)
	{
	LPOBJDESC       pObjDesc;
	LPEXEOBJTYPE    pExeObjType;
	RUNTIMEUID      exeObjTypeList;
	LPRESPONSEARRAY responseArray;
	short           responseCount;
	LPRESPONSE      pResponse;
	AEVENTID         eventID;
	LPEXEEVENT      pExeEvent;
	BOOL            success = FALSE;
	char						buffer[256];
  char 						typeName[8];
  ATYPEID					tempID;
  
	exeObjTypeList = pExeHead->GetExeObjTypeList();

	// get the design-time type struct, build a runtime version

	pObjDesc = pObjType->GetObjDesc();

	if (pObjDesc != 0)
		{
		pExeObjType = new(exeObjTypeList) EXEOBJTYPE(pObjType, 
			pObjType->GetRealObjCount());
		pObjType->copyUID = GetUID(pExeObjType);
    
		 // Let the library list know which DLL/entryPoint pair should be
		 // associated with this OBJTYPE's admin routine at runtime

		EXELIBLIST::BindUIDToExeProc(GetUID(pExeObjType), ADMIN_ROUTINE_INDEX,
		 pObjDesc->pGetDLL(ADMIN_ROUTINE_INDEX),
		 pObjDesc->pGetFunction(ADMIN_ROUTINE_INDEX));

	 // Let the library list know the DLL/entryPoint pairs that are
	 // associated with this OBJTYPE's events at runtime

		responseArray = pObjDesc->GetResponseArray();
		responseCount = responseArray->GetCount();

		for (int rdx = 0; rdx < responseCount; rdx++)
			{
			pResponse = (LPRESPONSE)responseArray->pGetElement((short)rdx);
			eventID = pResponse->GetEventID();
			pExeEvent = pExeHead->GetExeEventList()->GetNodeFor(eventID);
			pExeEvent->GetResponseList()->AddResponseFor(pObjType->GetTypeID());

			EXELIBLIST::BindEventToExeProc(eventID, pObjType->GetTypeID(),
			 pResponse->GetDll(), pResponse->pGetFunction());
			}
		
		success=TRUE;
		}
	else
  	{
    tempID = pObjType->GetTypeID();
		MAKE_TYPEID_READABLE(typeName, tempID);

    wsprintf(
    	buffer, 
    	"An Object of type [%s] is required but could not be found in your"
      " .DLL files.  The project may not run as expected."
    	"  Continue comilation?", 
      typeName);
    
    MessageBeep(MB_ICONEXCLAMATION);  
    if(MessageBox(GetActiveWindow(), buffer,
    	"Warning", MB_ICONEXCLAMATION|MB_TASKMODAL|MB_OKCANCEL) ==
    	IDOK)
    	success = TRUE;    
    }
    
	return success;
	}


//////////////////////////////////////////////////////////////////
void PROJECT::DoUndo()
	{
	BOOL redraw=FALSE;
	HCURSOR oldCursor;

	if(PROGSYSTEM::TestUndoFlag())
		{
		if(PROGSYSTEM::TestDeleteOpFlag() || PROGSYSTEM::TestClearOpFlag())
			{
			oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			PROGSYSTEM::SetDelContext();
			redraw = Paste();
			PROGSYSTEM::PopAndRegisterContext();
			PROGSYSTEM::SetUndoDeleteOpFlag();
			ShowCursor(FALSE);
			SetCursor(oldCursor);
			}
		else if(PROGSYSTEM::TestCutOpFlag())
			{
			oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			PROGSYSTEM::SetDelContext();
			redraw = Paste();
			PROGSYSTEM::PopAndRegisterContext();
			PROGSYSTEM::SetPasteOpFlag();
			ShowCursor(FALSE);
			SetCursor(oldCursor);
			}
		else if(PROGSYSTEM::TestPasteOpFlag())
			{
			DoCut();
			}
		else if(PROGSYSTEM::TestUndoDeleteOpFlag())
			{
			DoDelete();
			}
		else if(PROGSYSTEM::TestCopyFuncOpFlag())
			{
			if(PROGSYSTEM::GetUndoSjtUID() != NULLSERUID)
				{
				oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
				ShowCursor(TRUE);
				((SUBJECT *)GetObject(PROGSYSTEM::GetUndoSjtUID()))
					->DeleteFuncsOnly();
				PROGSYSTEM::SetUndoCopyFuncOpFlag();
				ShowCursor(FALSE);
				SetCursor(oldCursor);
				}
			}
		else if(PROGSYSTEM::TestUndoCopyFuncFlag())
			{
			oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			PROGSYSTEM::SetDelContext();
			redraw = Paste();
			PROGSYSTEM::PopAndRegisterContext();
			PROGSYSTEM::SetCopyFuncOpFlag();
			ShowCursor(FALSE);
			SetCursor(oldCursor);
			}
		if(redraw)
			{
			AdjustLastSubjectPosition();
			InvalidateRect(GetWindowPtr()->GetWindowHandle(), NULL, TRUE);
			UpdateWindow(GetWindowPtr()->GetWindowHandle());
			}
		PROGSYSTEM::SetUndoOpFlag();
		}
	}


void PROJECT::DoPaste()
	{
	BOOL redraw;
	HCURSOR oldCursor;
	LPCLIPHEAD clipHead;

	oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);
	PROGSYSTEM::ClearUndoOpFlag();
	PROGSYSTEM::SetClipContext();
	PROGSYSTEM::SetPasteOpFlag();
	redraw = Paste();
	clipHead =
		(LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
	clipHead->IncPasteOffset();
	PROGSYSTEM::PopAndRegisterContext();
	ShowCursor(FALSE);
	SetCursor(oldCursor);
	if(redraw)
		{
		AdjustLastSubjectPosition();
		AdjustLogBounds();
		((PROJECTWINDOW *)GetWindowPtr())->SetAllScrollBars();
		InvalidateRect(GetWindowPtr()->GetWindowHandle(), NULL, TRUE);
		UpdateWindow(GetWindowPtr()->GetWindowHandle());
		}
	PROGSYSTEM::SetUndoFlag();
	MakeDirty();
	}


//Paste should be entered in a clipboard context
BOOL PROJECT::Paste()
	{
	LPFILEOBJECT pasteFileObj, holdContext;
	LPCLIPHEAD clipHead;
	LPCLIPSJT  clipSjt;
	int i,j;
	SUBJECT * subjectPtr;
	LPCLIPOBJ clipObj;
	LPOBJINST objPtr;
	BOOL result = FALSE;
	SUBJECT * holdSubject;
  SUBJECT * originalActiveSubject = activeSubject;
	int numOldSubjects;

	pasteFileObj = pTheSystem->GetFileObject();
	clipHead = (LPCLIPHEAD)GetObject(pasteFileObj->GetHeadRec());

	//get the number of oldSubjects;
	holdContext = PROGSYSTEM::PopAndRegisterContext();
	numOldSubjects = (int)((LPRECORDBLOCK)GetObject(subjectBlock))->GetCount();
	PROGSYSTEM::PushAndRegisterContext(holdContext);

	if((activeSubject == NULL && clipHead->TestCompleteSubjectFlag())
		|| (activeSubject && !clipHead->TestCompleteSubjectFlag()))
		{

#ifdef DEMO
		//if its a complete subject then make sure we won't have too many
		//subjects.
		if(clipHead->TestCompleteSubjectFlag())
			{
			if(subjectCount +
				((LPRECORDBLOCK)GetObject(clipHead->GetSubjectBlock()))
				->GetCount() > DEMO_MAXSUBJECTS)
				{
				demo_SubjectMessage();
				return result;
				}
			else
				{
				int numObjs = nObjectsCount;
				LPCLIPSJT clipSjt;
				int i=0;

				while(NULL != (clipSjt = (LPCLIPSJT)GetObject(clipHead->GetSubjectBlock(), i)))
					numObjs += ((LPRECORDBLOCK)GetObject(clipSjt->objs))->GetCount();

				if(numObjs > DEMO_MAXPROJECTOBJECTS)
					{
					demo_ObjectMessage();
					return result;
					}
				}

			//NOTE: I'm assuming that anything on the clipboard was placed there
			//from the demo version.  Therefore, a complete subject on the
			//clipboard cannot contain more than the alloted number of subjects.
			//If in future versions the clipboard could somehow have more than
			//the alloted number of objects while a demo version is running, then
			//we need to check each subject on the clipboard.  That check should
			//occur in this block of code.
			}
		else
			{
			//if complete subjects are not being pasted then make sure that
			//the subject receiving objects will not have too many objects.
			i=0;
			clipSjt = ((LPCLIPSJT)GetObject(clipHead->GetSubjectBlock(), i));
			if(clipSjt)
				{
				j=((LPRECORDBLOCK)GetObject(clipSjt->objs))->GetCount();
				if(j + activeSubject->GetObjectCount() > DEMO_MAXOBJECTS ||
					j + GetObjectCount() > DEMO_MAXPROJECTOBJECTS)
					{
					demo_ObjectMessage();
					return result;
					}
				}
			}

#endif

		PROGSYSTEM::SetNoAliasWarningsFlag();
		PROGSYSTEM::SetUndoPrjID(fileObjPtr->GetFileIDStamp());
		if(activeSubject == NULL)
			{
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			ClearSubjectSelections();
			result = TRUE;
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			PROGSYSTEM::ClearUndoSjtUID();
			}
		else
			PROGSYSTEM::SetUndoSjtUID(GetUID(activeSubject));
    i=0;
		while(0 != (clipSjt=(LPCLIPSJT)GetObject(clipHead->GetSubjectBlock(), i)))
			{
			//set the subjectPtr to where we want to paste
			if(originalActiveSubject)
				subjectPtr = originalActiveSubject;
      else
				subjectPtr = clipSjt->UpdateEnvSubject(subjectBlock,
					clipHead->GetPasteOffset());

			//we should be in a clipboard context at this point
			subjectPtr->DoPaste(clipSjt);
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			clipSjt->copyUID = GetUID(subjectPtr);
			PROGSYSTEM::PushAndRegisterContext(holdContext);

			//now validate the object id's

			//set the active subject to the current subject so oip will work
			//properly during the validate routines.
			holdSubject = activeSubject;
			activeSubject = subjectPtr;

			//Pasting from one project to another is a special case:
			//if the objects are being pasted into a different project,
			//then call the PostValidateProjectUIDs to zero out all ids
			//being held by an object which are not on the clipboard and validate
			//the ids at the same time;

			if(clipHead->GetCutContextID() != fileObjPtr->GetFileIDStamp())
				{
				j=0;
				while(0 != (clipObj = (LPCLIPOBJ)GetObject(clipSjt->objs, j)))
					{
					holdContext = PROGSYSTEM::PopAndRegisterContext();
					objPtr = (OBJINST *)GetObject(clipObj->copyUID);
					objPtr->PreValidateProjectUIDs(holdContext, clipSjt->objs);
					PROGSYSTEM::PushAndRegisterContext(holdContext);
					}
				}
			else
				{
				//If were undoing some action then send validate messages to all
				//objects within the subject - otherwise, only send messages to
				//the objects which were on the clipboard.
				//The current context should be the delete context if were undoing
				//some action.
				
				I_ASSERT(PROGSYSTEM::GetDelContext());

				if(PROGSYSTEM::GetDelContext()->GetFileIDStamp() == 
					pTheSystem->GetFileObject()->GetFileIDStamp())
					{
					j=0;
					while(0 != (clipObj = (LPCLIPOBJ)GetObject(clipSjt->objs, j)))
						{
						holdContext = PROGSYSTEM::PopAndRegisterContext();
						objPtr = (OBJINST *)GetObject(clipObj->copyUID);
						objPtr->ValidateAllUIDs(subjectPtr->GetObjectList());
						PROGSYSTEM::PushAndRegisterContext(holdContext);
						}
					}
				else
					{
					j=0;
					while(0 != (clipObj = (LPCLIPOBJ)GetObject(clipSjt->objs, j)))
						{
						holdContext = PROGSYSTEM::PopAndRegisterContext();
						objPtr = (OBJINST *)GetObject(clipObj->copyUID);
						PROGSYSTEM::PushAndRegisterContext(holdContext);
						objPtr->ValidatePastedUIDs(clipSjt->objs);
						}
					}
				}

			//now do post validate stuff so objects which were pasted and
			//are holding onto other objects id's can correctly check if
			//those id's are bogus or valid - this is only within a subject.

			if(clipHead->GetCutContextID() == fileObjPtr->GetFileIDStamp())
				{
			 j=0;
			 while(0 != (clipObj = (LPCLIPOBJ)GetObject(clipSjt->objs, j)))
				 {
				 holdContext = PROGSYSTEM::PopAndRegisterContext();
				 objPtr = (OBJINST *)GetObject(clipObj->copyUID);
				 objPtr->PostValidateSubjectUIDs();
				 PROGSYSTEM::PushAndRegisterContext(holdContext);
				 }
				}
  	  //restore the active subject
	    activeSubject = holdSubject;
			}

		//Do post paste stuff here
		//Post paste allows all object aliases to correctly link themselves
		//so the signal coming out of aliases will display properly

		//first do the post paste for the existing subjects if complete subjects
    //are being pasted in
		if(clipHead->TestCompleteSubjectFlag())
    	{
    	holdContext = PROGSYSTEM::PopAndRegisterContext();
			int i=0;
			while((i < numOldSubjects) && (0 != (subjectPtr = (SUBJECT *)GetObject(
				subjectBlock, i))))
				{
        subjectPtr->ExistingSubjectPostPaste();
				}
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			}

		int i=0;
		while(0!=(clipSjt=(LPCLIPSJT)GetObject(clipHead->GetSubjectBlock(), i)))
			{
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			subjectPtr = (SUBJECT *)GetObject(clipSjt->copyUID);
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			subjectPtr->PostPaste(clipSjt);
			}

		//Bring project window to top if necessary
		if(clipHead->TestCompleteSubjectFlag())
			{
			holdContext = PROGSYSTEM::PopAndRegisterContext();
			BringWindowToTop(GetWindowPtr()->GetWindowHandle());
			PROGSYSTEM::PushAndRegisterContext(holdContext);
			}
		holdContext = PROGSYSTEM::PopAndRegisterContext();
		UpdateObjCountBoxes();
		PROGSYSTEM::PushAndRegisterContext(holdContext);
		PROGSYSTEM::ClearNoAliasWarningsFlag();
		}
	return result;
	}

void PROJECT::DoCut()
	{
	HCURSOR oldCursor;

	if(DeleteSharedObjects())
		{
		oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);
		PROGSYSTEM::ClearUndoOpFlag();
		PROGSYSTEM::SetUndoPrjID(fileObjPtr->GetFileIDStamp());
		if(GetActiveSubject())
			{
			DoPartialCut();
			PROGSYSTEM::SetUndoSjtUID(GetUID(GetActiveSubject()));
			}
		else
			{
			DoCompleteCut();
			PROGSYSTEM::ClearUndoSjtUID();
			}
		MakeDirty();
		ShowCursor(FALSE);
		SetCursor(oldCursor);
		}
	PROGSYSTEM::SetUndoFlag();
	}

//DoDelete not associated with a menu item so do check to make sure that
//something can be deleted
void PROJECT::DoDelete()
	{
	HCURSOR oldCursor;
	SUBJECT * subjectPtr = GetActiveSubject();

	if((!subjectPtr && SubjectsSelected()) || (subjectPtr &&
		(subjectPtr->ItemsSelected() || subjectPtr->ObjectsSelected())))
		{
		if(DeleteSharedObjects())
			{
			oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			PROGSYSTEM::ClearUndoOpFlag();
			PROGSYSTEM::SetDeleteOpFlag();
			PROGSYSTEM::SetUndoPrjID(fileObjPtr->GetFileIDStamp());
			if(GetActiveSubject())
				{
				DoPartialDelete();
				PROGSYSTEM::SetUndoSjtUID(GetUID(GetActiveSubject()));
				}
			else
				{
				DoCompleteDelete();
				PROGSYSTEM::ClearUndoSjtUID();
				}
			MakeDirty();
			ShowCursor(FALSE);
			SetCursor(oldCursor);
			PROGSYSTEM::SetUndoFlag();
			}
		}
	}

void PROJECT::DoCopy()
	{
	HCURSOR oldCursor;

	oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);
	PROGSYSTEM::ClearUndoOpFlag();
	PROGSYSTEM::SetUndoPrjID(fileObjPtr->GetFileIDStamp());
	if(GetActiveSubject())
		{
		DoPartialCopy();
		PROGSYSTEM::SetUndoSjtUID(GetUID(GetActiveSubject()));
		}
	else
		{
		DoCompleteCopy();
		PROGSYSTEM::ClearUndoSjtUID();
		}
	ShowCursor(FALSE);
	SetCursor(oldCursor);
	}

void PROJECT::DoClear()
	{
	HCURSOR oldCursor;

	if(DeleteSharedObjects())
		{
		oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);
		PROGSYSTEM::ClearUndoOpFlag();
		PROGSYSTEM::SetClearOpFlag();
		PROGSYSTEM::SetUndoPrjID(fileObjPtr->GetFileIDStamp());
		if(GetActiveSubject())
			{
			DoPartialDelete();
			PROGSYSTEM::SetUndoSjtUID(GetUID(GetActiveSubject()));
			}
		else
			{
			DoCompleteDelete();
			PROGSYSTEM::ClearUndoSjtUID();
			}
		MakeDirty();
		ShowCursor(FALSE);
		SetCursor(oldCursor);
		PROGSYSTEM::SetUndoFlag();
		}
	}


void PROJECT::DoPartialCut()
	{
	DoPartialCopy();
	PROGSYSTEM::SetDeleteOpFlag();
	DoPartialDelete();
	PROGSYSTEM::SetCutOpFlag();
	}

void PROJECT::DoCompleteCut()
	{
	DoCompleteCopy();
	PROGSYSTEM::SetDeleteOpFlag();
	DoCompleteDelete();
	PROGSYSTEM::SetCutOpFlag();
	}

void PROJECT::DoPartialCopy()
	{
	SUBJECT * subjectPtr;
	LPCLIPHEAD headPtr;

	subjectPtr = GetActiveSubject();

	if(subjectPtr)
		{
		PROGSYSTEM::SetCopyOpFlag();
		PROGSYSTEM::PurgeClipFileObj();
		PROGSYSTEM::SetClipContext();
		headPtr = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
		headPtr->ClearCompleteSubjectFlag();
		headPtr->ClearPasteOffset();
		headPtr->IncPasteOffset();
		headPtr->SetCutContextID(fileObjPtr->GetFileIDStamp());
		subjectPtr->DoCopy(FALSE);
		PROGSYSTEM::PopAndRegisterContext();
		}
	}

void PROJECT::DoCompleteCopy()
	{
	SUBJECT * subjectPtr;
	LPCLIPHEAD headPtr;
	int i=0;

	PROGSYSTEM::PushAndRegisterContext(fileObjPtr);
	PROGSYSTEM::PurgeClipFileObj();
	PROGSYSTEM::SetCopyOpFlag();
	PROGSYSTEM::SetClipContext();
	headPtr = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
	headPtr->SetCompleteSubjectFlag();
	headPtr->ClearPasteOffset();
	headPtr->IncPasteOffset();
	headPtr->SetCutContextID(fileObjPtr->GetFileIDStamp());
	PROGSYSTEM::PopAndRegisterContext();
	while(0 != (subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(subjectPtr->IsSelected())
			{
			PROGSYSTEM::SetClipContext();
			subjectPtr->DoCopy(FALSE);
			PROGSYSTEM::PopAndRegisterContext();
			}
		}
	PROGSYSTEM::PopAndRegisterContext();

	}


void PROJECT::DoPartialDelete()
	{
	SUBJECT * subjectPtr;
	LPCLIPHEAD headPtr;

	if(0 != (subjectPtr = GetActiveSubject()))
		{
		PROGSYSTEM::PurgeDelFileObj();
		PROGSYSTEM::SetDelContext();
		headPtr = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
		headPtr->ClearCompleteSubjectFlag();
		headPtr->ClearPasteOffset();
		headPtr->SetCutContextID(fileObjPtr->GetFileIDStamp());
		subjectPtr->DoCopy(TRUE);
		PROGSYSTEM::PopAndRegisterContext();
		UpdateObjCountBoxes();
		subjectPtr->LinkAllAliasObjects(FALSE);
		}
	}

void PROJECT::DoCompleteDelete()
	{
	SUBJECT * subjectPtr;
	LPCLIPHEAD headPtr;
	int i=0;
	BOOL redraw=FALSE;

	PROGSYSTEM::PushAndRegisterContext(fileObjPtr);
	PROGSYSTEM::PurgeDelFileObj();
	PROGSYSTEM::SetDelContext();
	headPtr = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
	headPtr->SetCompleteSubjectFlag();
	headPtr->ClearPasteOffset();
	headPtr->SetCutContextID(fileObjPtr->GetFileIDStamp());
	PROGSYSTEM::PopAndRegisterContext();
	while(0 != (subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(subjectPtr->IsSelected())
			{
			PROGSYSTEM::SetDelContext();
			subjectPtr->DoCopy(TRUE);
			PROGSYSTEM::PopAndRegisterContext();
			delete subjectPtr;
			i--;
			redraw=TRUE;
			}
		}

	LinkAllAliases(FALSE);
	if(redraw)
		{
		AdjustLastSubjectPosition();
		SnapLogBounds(HRANGE|VRANGE);
		((PROJECTWINDOW *)GetWindowPtr())->SetAllScrollBars();
		InvalidateRect(GetWindowPtr()->GetWindowHandle(), NULL, TRUE);
		UpdateWindow(GetWindowPtr()->GetWindowHandle());
		}
	UpdateObjCountBoxes();
	PROGSYSTEM::PopAndRegisterContext();
	}


void PROJECT::CleanCompile()
	{
	LPEXEHEAD   lpExeHead;
	RUNTIMEUID  list;

	lpExeHead = (LPEXEHEAD)GetObject(fileObjPtr->GetHeadExeRec());

	list = lpExeHead->GetExeFuncList();

	((LPEXERECORDBLOCK)GetObject(list))->PurgeBlock();

	list = lpExeHead->GetExeObjTypeList();

	((LPEXERECORDBLOCK)GetObject(list))->PurgeBlock();

	delete lpExeHead;

	delete (LPEXERECORDBLOCK)GetObject(fileObjPtr->GetHeadExeBlock());

	delete pTheSystem->GetFileObject()->GetTempObjects();

	pTheSystem->GetFileObject()->GetExeRecordBlocks()->PurgeList();
	pTheSystem->GetFileObject()->GetExeSeriusBlocks()->PurgeList();

	}

BOOL PROJECT::SubjectsSelected()
	{
	SUBJECT * temp;
	int i=0;
	int result = FALSE;

	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(temp->IsSelected())
			{
			result = TRUE;
			break;
			}
		}

	return result;
	}

BOOL PROJECT::FlowStopsSet()
	{
	SUBJECT * temp;
	int i=0;
	int result = FALSE;

	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(temp->FlowStopsSet())
			{
			result = TRUE;
			break;
			}
		}

	return result;

	}

FLOW * PROJECT::GetDesignFlow(
	LPEXEFLOW exeFlow)
	{
	SUBJECT * temp;
	int i=0;
	FLOW * flowPtr=NULL;

	while(0 != (temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if((flowPtr = temp->GetDesignFlow(exeFlow))!=NULL)
			break;
		}
	return flowPtr;
	}

void PROJECT::DoPrint()
	{
	PRINTER * printPtr;
	BOOL printResult;
	SUBJECT * subjectPtr;
	int i;
	char buffer[40];
	int copies;
	int j;

	printPtr = new PRINTER(PROGSYSTEM::hWndFrame);
	printResult = printPtr->PrintDlg();
	if(printResult > 0)
		{
		copies = printPtr->GetNumCopies();
		for(j=0;j<copies;j++)
			{
			labelPtr = new PRINTLABEL();
			i=0;
			if(printPtr->StartJob(GetName()))
				{
				LCID	lcid		= GetUserDefaultLCID();
				char	szDate[64];
				char	szTime[64];

				GetTimeFormat(lcid, 0, NULL, NULL, szDate, sizeof(szDate));
				GetDateFormat(lcid, 0, NULL, NULL, szTime, sizeof(szTime));

				wsprintf(buffer, "%s - %s %s", GetName(), szDate, szTime);

				printPtr->CenterLine(buffer);
				printPtr->SkipLine();
				while(0 != (subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
					subjectPtr->DoPrint(printPtr);
				printPtr->EndJob();
				}
				delete labelPtr;
			}
		}
	delete printPtr;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CheckProject
//
// CLASS.......:  PROJECT
//
// DESCRIPTION.:  Checks each subject in the project.  Returns false if any
//                errors are discovered.  This routine should be called
//                prior to any compilation.
//
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECT::CheckProject(
	BOOL updateStatus)
	{
	short       subjectResult;

	subjectResult = DoChecking(updateStatus);

	if(subjectResult & CHECK_CANCEL)
		{
		SetCancelCompileFlag();
		return FALSE;
		}

	if(subjectResult & CHECK_ERROR)
		return FALSE;

	if(subjectResult & CHECK_DUMMYALIASWARNING)
		{
		if(IDNO == MessageBox(NULL, "There were Unlinked Alias Objects found in"
			" the project.\nContinue compiling?", "Warning",
			MB_ICONEXCLAMATION|MB_YESNO|MB_TASKMODAL))
			{
			SetCancelCompileFlag();
			return FALSE;
			}
    }
	if(subjectResult & CHECK_WARNING)
		{
		if(IDNO == MessageBox(NULL, "There are warnings in the project.\n"
			"Continue compiling?", "Warning",
			MB_ICONEXCLAMATION|MB_YESNO|MB_TASKMODAL))
			{
			SetCancelCompileFlag();
			return FALSE;
			}
		}

	return TRUE;
	}

short PROJECT::DoChecking(
	BOOL updateStatus)
	{
	SUBJECT     * pSubject;
	int         i;
	short       subjectResult;

	ClearCancelCompileFlag();
	//FALSE means don't display warnings
	subjectResult = LinkAllAliases(FALSE);
	if(!(subjectResult & CHECK_CANCEL))
		{
		i=0;
		while(0 != (pSubject = (SUBJECT *)GetObject(subjectBlock, i)))
			{
			//The activeSubject will be set to the subject being currently checked
			//in the CheckSubject routine.
			subjectResult|=pSubject->CheckSubject(updateStatus);
			if(updateStatus)
				PROJECT::IncStatusBar();
			if(subjectResult & CHECK_CANCEL)
				break;
			}
		}
	return subjectResult;
	}

void PROJECT::CheckProjectForErrors()
	{
	short result;
	HCURSOR oldCursor;
	char buffer[128];
	char errorType[48];

	oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);

	//FALSE means don't update the status bar
	result = DoChecking(FALSE);
	
	ShowCursor(FALSE);
	SetCursor(oldCursor);

	if(!(result & CHECK_CANCEL))
		{
		if(apb_BuildCheckErrorMessage(result, errorType, sizeof(errorType)))
			{

			wsprintf(buffer, "There were %s found\nin project: %s",
				errorType, GetName());

			MessageBox(NULL, buffer, "Project Results", MB_ICONINFORMATION|
				MB_OK|MB_TASKMODAL);

			}
		}
	}

BOOL PROJECT::DeleteSharedObjects()
	{
	SUBJECT *  subjectPtr;
	BOOL          sharedObjects = FALSE;
	RUNTIMEUID    objectList;
	LPOBJINST     objPtr;
	int           i;
	int           j;

	if((subjectPtr = GetActiveSubject())!=NULL)
		{
		j=0;
		objectList = subjectPtr->GetObjectList();
		while(!sharedObjects && ((objPtr = (LPOBJINST)GetObject(objectList, j))
			!=NULL))
			sharedObjects = (objPtr->TestSelectFlag() &&
				objPtr->TestShareFlag());
		}
	else
		{
		i=0;
		while(!sharedObjects && ((subjectPtr = (SUBJECT *)GetObject(
			subjectBlock, i))!=NULL))
			{
			if(subjectPtr->IsSelected())
				{
				j=0;
				objectList = subjectPtr->GetObjectList();
				while(!sharedObjects && ((objPtr = (LPOBJINST)GetObject(objectList, j))
					!=NULL))
					sharedObjects = objPtr->TestShareFlag();
				}
			}
		}
	if(sharedObjects && IDCANCEL == MessageBox(NULL,
		"You have selected one or more Shared Objects\n"
		"to be deleted or cut from the workbench.\n"
		"If you choose to continue, all aliased objects\n"
		"linked to the shared objects will lose their links.",
		"Warning:", MB_OKCANCEL|MB_ICONINFORMATION))
		return FALSE;

	else
		return TRUE;
	}

LPOBJTYPE PROJECT::GetTypePtr(
	ATYPEID typeID)
	{	
	LPOBJTYPE typePtr = GetGoodOrBadTypePtr(typeID);
	
	return (!typePtr || typePtr->TestBadTypeFlag())?NULL:typePtr;
	}

LPOBJTYPE PROJECT::GetGoodOrBadTypePtr(
	ATYPEID typeID)
	{
	LPOBJTYPE typePtr;
	int i=0;

	while(0 !=  (typePtr = (LPOBJTYPE)GetObject(typeBlock, i)))
		if(typePtr->GetTypeID() == typeID)
			break;

	return typePtr;;
	}		 



void PROJECT::ClearAllStops()
	{
	SUBJECT * subjectPtr;
	int i;

	i=0;
	while((0!=(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i))))
		{
		subjectPtr->ClearAllFlowStops();
		}
	}

void PROJECT::SetFlowStops(
	BOOL enable)
	{
	SUBJECT *  subjectPtr;
	FUNCINST * funcPtr;
	LPOBJINST     objPtr;
	RUNTIMEUID    objectBlock;
	BOOL          redraw;
	int           i;
	int           j;
	HDC 					hdc;

	i=0;
	while((0!=(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i))))
		{
		if(subjectPtr->GetWindowPtr())
			hdc = GetDC(subjectPtr->GetWindowPtr()->GetWindowHandle());
		else
			hdc = NULL;

		redraw=FALSE;
		j=0;
		objectBlock = subjectPtr->GetObjectList();
		while((0!=(objPtr = (LPOBJINST)GetObject(objectBlock, j))))
			{
			if(enable)
				redraw|=objPtr->EnableStops(hdc);
			else
				redraw|=objPtr->DisableStops(hdc);
			}
		j=0;
		objectBlock = subjectPtr->GetFunctionList();
		while((0!=(funcPtr = (FUNCINST *)GetObject(objectBlock, j))))
			{
			if(enable)
				redraw|=funcPtr->EnableStops(hdc);
			else
				redraw|=funcPtr->DisableStops(hdc);
			}

		if(hdc)
			ReleaseDC(subjectPtr->GetWindowPtr()->GetWindowHandle(), hdc);
		}

	if(redraw)
		MakeDirty();
	}


void PROJECT::CenterSelection()
	{
	if(GetWindowPtr())
		((PROJECTWINDOW *)GetWindowPtr())->CenterSelection();
	}

void PROJECT::SelectAll()
	{
	if(GetActiveSubject())
		{
		GetActiveSubject()->SelectAll();
		}
	else
		{
		if(GetWindowPtr())
			((PROJECTWINDOW *)GetWindowPtr())->SelectAllSubjects();
		}
	}


void PROJECT::UpdateObjCountBoxes()
	{
	SUBJECT * subjectPtr;
	int i=0;

	while(0!=(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		subjectPtr->UpdateCountBox();
		}
	}

void PROJECT::PushScopeStack(
	double percent)
	{
	if(scopeStackPtr)
		scopeStackPtr->PushScope(percent);
	}

void PROJECT::PopScopeStack()
	{
	if(scopeStackPtr)
		scopeStackPtr->PopScope(TRUE);
	}

void PROJECT::IncStatusBar(
	long amount /* defaults to 1*/)
	{
	if(compStatPtr)
		compStatPtr->UpdateStatus(amount);
	}

long PROJECT::GetCompileTime()
	{
	long totalSize;
	int i;
	SUBJECT * temp;

	totalSize=0;
	i=0;
	while(0!=(temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		totalSize+=(PRJ_OBJCOMPWEIGHT * (long)((LPRECORDBLOCK)
			GetObject(temp->GetObjectList()))->GetCount());
		
		totalSize+= (PRJ_FUNCCOMPWEIGHT* (long)((LPRECORDBLOCK)
			GetObject(temp->GetFunctionList()))->GetCount());
		}
	totalSize+=((long)i * PRJ_SJTCOMPWEIGHT); //add in number of subjects
	totalSize++; 	//add 1 for the number of projects to make sure the time
								//is at least 1.
	totalSize = (long)((double)totalSize * (1.0 + PRJ_PRJCOMPTIME));
	return totalSize;
	}

//----------------------------------------------------------------------
//   PROJECT::RenameAliases
//
// Requests all subjects to rename aliases of 'masterObject'.
//----------------------------------------------------------------------
BOOL PROJECT::RenameAliases(
  LPOBJINST  masterObject,
  LPCSTR      newName)
  {
	LPSUBJECT  subjectNow, holdSubject;
  HWND       holdFocus;
  int        i;

  holdFocus = GetFocus();

	// Go over each subject to fix alias names
	i = 0;
	while(subjectNow = (SUBJECT *)GetObject(subjectBlock, i), subjectNow)
		{
		// Make this subject the active subject so oip routines
		// will work right.
		holdSubject = activeSubject;
		activeSubject = subjectNow;

    subjectNow->RenameAliases(masterObject, newName);

		activeSubject = holdSubject;
		}

  if (holdFocus)
    SetFocus(holdFocus);

  return TRUE;
  }

//----------------------------------------------------------------------
//   PROJECT::IsAliased
//
// Returns TRUE if the specified object is aliased in any subject.
//----------------------------------------------------------------------
BOOL PROJECT::IsAliased(
  LPOBJINST  masterObject)
  {
	LPSUBJECT  subjectNow, holdSubject;
  HWND       holdFocus;
  int        i;
  BOOL       ans;

  holdFocus = GetFocus();

	holdSubject = activeSubject;

	// Go over each subject
	i = 0;
	while(subjectNow = (SUBJECT *)GetObject(subjectBlock, i), subjectNow)
		{
		// Make this subject the active one so OIP routines work right
		activeSubject = subjectNow;

    if (subjectNow->IsAliased(masterObject))
      {
      ans = TRUE;
      goto Bye;
      }
		}

  ans = FALSE;

Bye:

  activeSubject = holdSubject;

  if (holdFocus)
    SetFocus(holdFocus);

  return ans;
  }


void PROJECT::UnselectEverything()
	{
  SUBJECT * subjectPtr;
	int i=0;

  while(0!=(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
  	{
    if(subjectPtr->IsSelected())
    	subjectPtr->SetSelected(FALSE);

    subjectPtr->OnlyUnselectAll();
		}
	}

// causes redraw of all alias objects of same name as the sharedObjPtr
// upon redraw, the objInst will automatically link to the shared object
void PROJECT::RedrawLinkedAliases(
	LPOBJINST sharedObjPtr)
	{
	SUBJECT * subjectPtr;
	SUBJECT * sharedObjSubject;
	int i;

	sharedObjSubject = sharedObjPtr->GetSubjectPtr();
	i=0;
	while(0 != (subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		//the subject containing the shared object will automatically update upon
		//redraw after the persistence was changed
		if(subjectPtr != sharedObjSubject)
			//TRUE means update the objList
			subjectPtr->RedrawLinkedAliases(sharedObjPtr, TRUE);
		}
	}

// causes redraw of all alias objects of same name as the shareed objects
// shared objects contained in subjectPtr.
// upon redraw, the objInst will automatically link to the shared object
void PROJECT::RedrawAllLinkedAliases(
	LPSUBJECT subjectPtr)
	{
	SUBJECT *  tempSubject;
	LPOBJINST     sharedObjPtr;
	int           i,j;
	RUNTIMEUID    objectBlock;

	objectBlock = subjectPtr->GetObjectList();
	i=0;

	while(0 != (sharedObjPtr = (LPOBJINST)GetObject(objectBlock, i)))
		{
		if(sharedObjPtr->TestShareFlag())
    	{
			j=0;
			while(0 != (tempSubject = (SUBJECT *)GetObject(subjectBlock, j)))
				{
				//FALSE means don't redraw but invalidate
				tempSubject->RedrawLinkedAliases(sharedObjPtr, FALSE);
				}
			}
    }

	//cause all appropriate subjects to redraw
	j=0;
	while(0 != (tempSubject = (SUBJECT *)GetObject(subjectBlock, j)))
		{
		if(subjectPtr->GetWindowPtr())
			UpdateWindow(GetWindowPtr()->GetWindowHandle());
		}
	}

BOOL PROJECT::DuplicateSubjectName(
	LPCSTR name)
	{
	BOOL result = FALSE;
	int i=0;
	SUBJECT * subjectPtr;

	while(!result &&
		0 != (subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		result = (0 == lstrcmp(name, subjectPtr->GetName()));
		}

	return result;
	}

short PROJECT::LinkAllAliases(
	BOOL warnings)
	{
	int i;
	SUBJECT * subjectPtr;
	short result = CHECK_NOERRORS;
	i=0;

	while(0!=(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		result|=subjectPtr->LinkAllAliasObjects(warnings);
		if(result & CHECK_CANCEL)
			break;
		}
	return result;

	}

void PROJECT::RedrawAllFunctions()
	{
	int i=0;
	SUBJECT * subjectPtr;

	while(0 != (subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		subjectPtr->RedrawAllFunctions();
	}

void PROJECT::ResetLogicalBounds()
	{
	PROJECTWINDOW * prjWinPtr;

	prjWinPtr = (PROJECTWINDOW *)GetWindowPtr();
	AdjustLogBounds();
	SnapLogBounds(HRANGE|VRANGE);
	if(prjWinPtr)
		{
		prjWinPtr->DetermineScrollBars();
		prjWinPtr->SetScrollBars(HRANGE|HPOS|VRANGE|VPOS);
		}
	}

//Creates a new type record if one does not already exist
//Returns the uid of the type record
//showErrors is optional and defaults to TRUE
SERUID PROJECT::CreateObjType(
	long      typeID,
	ADMNPROC	admnProc)
	{
	LPOBJTYPE newType;
	SERUID typeUID=NULLSERUID;

	newType = GetGoodOrBadTypePtr(typeID);

	if(newType)
		{
		if(!newType->TestBadTypeFlag())
			typeUID = GetUID(newType);
		}
	else
		typeUID = NewType(typeID, admnProc);

	return typeUID;
	}

//creates a typePtr if one does not yet exist
LPOBJTYPE PROJECT::ObtainTypePtr(
	ATYPEID typeID)
	{
	LPOBJTYPE typePtr = GetGoodOrBadTypePtr(typeID);
	LPADMNNODE node;
	SERUID newUID;

	if(typePtr == NULL)
		{
		node = pTheSystem->GetMasterTypeList()->GetNode(typeID);
		if(node)
			{
			newUID = NewType(typeID, node->m_admnProc);
			if(newUID != NULLSERUID)
				typePtr = (LPOBJTYPE)GetObject(newUID);
			}
		}
		
	return (!typePtr || typePtr->TestBadTypeFlag())?NULL:typePtr; 
	}

//creates a type without Loading the DLL
SERUID PROJECT::NewType(
	long typeID,
	ADMNPROC admnProc)
	{
	LPOBJTYPE objType;
	SERUID typeUID = NULLSERUID;
	LPOBJDESC objDesc;

	objDesc = cfgGetObjectDesc(typeID);
	if(objDesc)
  	{
  	objType = new(typeBlock) OBJTYPE(typeID, objDesc);
		objType->SetAdmnProc(admnProc);
    typeUID = GetUID(objType);
		}

	return typeUID;
	}

//should be called whenever a new project is created.
//fills the type block with type records for all possible types.
//returns false if a type can't be created.
BOOL PROJECT::BuildTypeBlock()
	{
	LPADMNNODE node;
	BOOL result = TRUE;//assume all types can be created.

	node=NULL;
	while(NULL != (node = pTheSystem->GetMasterTypeList()->GetNextNode(node)))
		{
		if(NewType(node->m_typeID, node->m_admnProc) == NULLSERUID)
			result = FALSE;
		}

  return result;
	}

//should be called when a project is opened to fill the type block with
//all possible types which aren't already in the project.
BOOL PROJECT::UpdateTypeBlock()
	{
	LPADMNNODE node;

	BOOL result = TRUE; //assume all types can be created

	node=0;
	while(NULL != (node = pTheSystem->GetMasterTypeList()->GetNextNode(node)))
		{
		//Call create objType here because it creates the type only
		//if one does not exist.
		if(CreateObjType(node->m_typeID, node->m_admnProc) == NULLSERUID)
			result = FALSE;
		}

	return result;
	}


//builds a list of unused types and deletes the types without freeing the libraries.
//returns a pointer to the list
//Don't purge out the bad types becuase if we UPSF a project back in, it
//may recreate the types. It's better to have bad types hanging around.
LPLIST PROJECT::BuildUselessTypeList()
	{
	LPTYPELISTMEMBER member;
	LPLIST list;
	int i;
	LPOBJTYPE objType;

	list = new LIST();

	i=0;
	while(0 != (objType = (LPOBJTYPE)GetObject(typeBlock, i)))
		{
		if(objType->GetTotalObjCount() == 0)
			{
			//useless so add to list
			member = new TYPELISTMEMBER(objType->GetTypeID());

			list->Add(member);

			delete objType;
			i--;
      }
		}

  return list;
	}


//restores the type records based on the info in list.
//The list is deleted.
void PROJECT::RestoreUselessTypeList(
	LPLIST list)
	{
	LPTYPELISTMEMBER member;
	SERUID objTypeUID;
	LPADMNNODE nodePtr;

	while(NULL != (member = (LPTYPELISTMEMBER)list->GetNextMember(NULL)))
		{
		nodePtr = pTheSystem->GetMasterTypeList()->GetNode(member->typeID);

		I_ASSERT(nodePtr);

		objTypeUID = NewType(member->typeID, nodePtr->m_admnProc);

		if(objTypeUID == NULLSERUID)
			{
			MessageBox(NULL, "A type record has been lost. To prevent project corruption"
				" AppWare is being terminated without saving your current changes."
				"  Sorry!", "Fatal Error", MB_ICONEXCLAMATION|MB_TASKMODAL|MB_OK);
			FatalExit(0);
			}
    list->Remove(member);
    delete member;
		}

	delete list;
	}

//should be called when an object is created from an OBJD
AOBJECTID PROJECT::CreateDynamicObject(
	ATYPEID typeID,
	APROTOCOLID protocolID)
	{
  AOBJECTID objID = 0;
	LPSUBJECT subjectPtr=GetActiveSubject();

	if(subjectPtr)
		objID = subjectPtr->CreateDynamicObject(typeID, protocolID);

	return objID;
	}

BOOL PROJECT::DestroyObject(
	AOBJECTID objID)
	{
	BOOL result = FALSE;
	LPOBJINST objPtr;

	if(GetUIDType((SERUID)objID) == RT_OBJINST)
  	{
		objPtr = (LPOBJINST)GetObject((SERUID)objID);
		if(objPtr)
			//we have a valid object
			result = objPtr->GetSubjectPtr()->DestroyObject(objPtr);
      
		}

	ADBG_ASSERT_E(result == TRUE,;,"Invalid Object ID");


  return FALSE;
	}


BOOL PROJECT::RenameObject(
	AOBJECTID objID,
	LPCSTR newName)
	{
	BOOL result = FALSE;
	LPOBJINST objPtr;

	if(newName != NULL)
  	{
		if(GetUIDType((SERUID)objID) == RT_OBJINST)
  		{
			objPtr = (LPOBJINST)GetObject((SERUID)objID);
			if(objPtr)
				//we have a valid object
				result = objPtr->GetSubjectPtr()->RenameObject(objPtr, newName);
      
			}
		ADBG_ASSERT_E(result == TRUE,;,"Invalid Object ID");
		}
	else
  	ADBG_PRINT_E("Invalid string passed in for the new name.");

	return FALSE;
	}


void PROJECT::GetNewGroupName(
	LPSTR nameBuffer)
	{
	wsprintf(nameBuffer, "Group%u", newGroupIndex);
	newGroupIndex++;
  }


//this routine now just purges a type if it's bad and not marked as
//being a required type.
void PROJECT::PurgeBadUnusedTypes()
	{
	LPOBJTYPE objType;
	int i=0;

	while(0 != (objType = (LPOBJTYPE)GetObject(typeBlock, i)))
		{
		if(objType->TestBadTypeFlag() && !objType->GetRequiredCount())
			{
			delete objType;
			i--;
			}
		}

	}

////////////////////////////////////////////////////////////////
void PROJECT::ImportSplashScreen()
	{
  SPLASHMGR* splashMgrPtr = new SPLASHMGR();
  
  if(splashMgrPtr->ImportSplashScreen(fileObjPtr->GetSplashPtr(), fileObjPtr->GetSplashTimeDelay()))
  	{
    fileObjPtr->SetSplashBits();
    fileObjPtr->SetSplashTimeDelay(splashMgrPtr->GetDelayTime());
    MakeDirty();
    }
    
  delete splashMgrPtr;
  }

/////////////////////////////////////////////////////////////////
//Trys to create the type if one doesn't already exist
BOOL PROJECT::MarkRequiredType(
	ATYPEID typeID)
  {
  BOOL rVal;
  LPOBJTYPE typePtr = ObtainTypePtr(typeID);
  
  if(typePtr)
  	{
  	typePtr->IncRequiredCount();
  	rVal = TRUE;
    }
  else
  	{
  	rVal = FALSE;
  	}
  
  return rVal;
  }

/////////////////////////////////////////////////////////////////
BOOL PROJECT::ClearRequiredType(
	ATYPEID typeID)
  {
  BOOL rVal;
  LPOBJTYPE typePtr = GetTypePtr(typeID);
  
  if(typePtr)
  	{
    typePtr->DecRequiredCount();
    rVal = TRUE;
   	}
  else
  	rVal=FALSE;
    
  return rVal;
  }