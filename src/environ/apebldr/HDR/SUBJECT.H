#if(!defined(__subject_h))
	#define __subject_h

#include "classdef.h"
#include "wrksheet.h"
#include "dbstring.h"
#include "sereclib.h"
#include "recblk.h"
#include "uidlist.h"
#include "sjtwin.h"
#include "bitree.h"

#define SEPARATOR 8
#define FLOWTIPADJUST 2

#define SJT_UPSFINDEXERROR -1

#define SJT_UPSFCONSTRUCTOR 	0x0001
#define SJT_NORMALCONSTRUCTOR 0x0002

#define SUB_SEND_AWAKENED				0x00000001L
#define SUB_SEND_READ						0x00000002L

class INSERTOBJDATA:public UIDLIST
	{
	public:
		INSERTOBJDATA(
			SERUID newUID):UIDLIST(newUID)
			{
			}

		POINT oldNipple;
	};

typedef INSERTOBJDATA * LPINSERTOBJDATA;

class SUBJECT:public WORKSHEET{

	friend COMMENT;
	friend SUBJECTWINDOW;
	friend FUNCINST;

        friend long ALMAPI AObjGetObjectCount(
		ATYPEID typeID);

        friend long ALMAPI ATypeGetCount();

        friend LONG ALMAPI AObjEdit(
		AOBJECTID theObject);

        friend BOOL ALMAPI AObjExport(
		AOBJECTID theObject);

        friend BOOL ALMAPI AObjImport(
		AOBJECTID theObject);

        friend AOBJECTID ALMAPI AObjGetNextObject(
		ATYPEID typeID,
		LPLONG theGetKey);

        friend ATYPEID ALMAPI ATypeGetNext(
		LPLONG theGetKey);

        friend long ALMAPI ATypeGetName(
		ATYPEID theType,
		LPSTR  theName,
		long	 nameSize);

        friend BOOL ALMAPI AEvtCheckForMethod (
		ATYPEID  theType,
		AEVENTID selector);

        friend BOOL ALMAPI AEvtCallMethod (
		AOBJECTID sourceObject,
		AOBJECTID targetObject,
		ATYPEID   targetType,
		AEVENTID  selector,
		LONG     methodParam);

	//a helper routine contained in oipobjs.h
	friend  LPOBJTYPE GetSubjectTypePtr(
		ATYPEID typeID);


	private:
		static LPSERUID	  objArray;
		static int				sizeObjArray;
		static LPSERUID		funcArray;
		static int				sizeFuncArray;
		static LPBITREE		objTree;
		static LPBITREE		funcTree;

		int								nObjectsCount;
		POINT							positionInProject;
		SERUID			 			parentProjectUID;
		POINT							mouseOffset;
		POINT							originOffset;
		RUNTIMEUID			 	theFunctions;
		RUNTIMEUID				theObjects;
		RUNTIMEUID				theSubTypes;
		RUNTIMEUID				theComments;
		RUNTIMEUID				theObjGroups;
		RECT							logBounds;
		DBSTRING					name;
		SERUID						activeGroup;

		void GetSubjectRects(
			HDC			hdc,
			LPRECT	subRect,
			LPRECT	textRect,
			LPRECT	unionRect);

		void AdjustSubjectRects(
			LPRECT	subRect,
			LPRECT	textRect,
			POINT	adjustment);

		BOOL  SubjectUnionIntersectsRect(
			HDC			hdc,
			LPRECT 	subRect,
			LPRECT 	textRect,
			LPRECT 	refreshRect);

		void Initialize();

		void MakeClipGroups(
			LPCLIPSJT clipSjtPtr,
			BOOL copyAll,
			BOOL doDelete);

		void MakeClipFlows(
			LPCLIPSJT clipSjtPtr,
			BOOL copyAll);

		void MakeClipParams(
			LPCLIPSJT clipSjtPtr,
			BOOL copyAll);

		void MakeClipObjs(
			LPCLIPSJT clipSjtPtr,
			LPOBJGROUP envGroupPtr,
			BOOL copyAll);

		void MakeClipFuncs(
			LPCLIPSJT clipSjtPtr,
			BOOL copyAll);

		void MakeClipComments(
			LPCLIPSJT clipSjtPtr,
			BOOL copyAll);

		void InsertObjByIndex(
			OBJINST * objPtr,
			int  &index,
			int numInserted);

		void UpdateObjIndexes();

		int GetObjInsertPosition();


		static void SelectFunction(
			FUNCINST  *  pFuncInst);

		void DoOnlyUnselect(
			SERUID activeGroup);

		BOOL DoPrivateColorSet();

		short CheckObjectVersions(
			long messages = (SUB_SEND_AWAKENED | SUB_SEND_READ));

		BOOL PasteInObjects(
			LPCLIPHEAD clipHead,
			LPLIST looseObjList,
			BOOL undoOp);

		BOOL CreateBigDefaultGroup(
			POINT location);

		short DoFuncCheck(
			short checkType,
			BOOL updateStatus);

		void MarkUninitializedParams(
			BOOL updateStatus);


	public:

		LPFILEOBJECT fileObjPtr;

		SUBJECT(
			LPCSTR  newName,
			SERUID projectUID,
			WORD constructorType);

		SUBJECT(
			LPCSTR   newName,
			SERUID  projectUID,
			LPRECT  winBounds,
			LPRECT 	logicalBounds,
			LONG 		newStatus,
			POINT   projectOffset,
			POINT   newOriginOffset);

		SUBJECT(
			void * thisPtr);

		void ConstructorInitialize();

		short PostReadInit();

		~SUBJECT();

		BOOL ShowObjectWindow();

		void DrawASubject(
			HDC			hdc,
			LPRECT	clientRect,
			POINT		drawOffset);

		BOOL SubjectIntersectPoint(
			HDC			hdc,
			LPPOINT	thePoint);

		void Move(
			HDC			hdc,
			LPPOINT offset,
			LPRECT 	unionRect);

		BOOL SubjectIntersectsRect(
			HDC hdc,
			LPRECT rectToIntersect);

		BOOL AdjustToGrid();

		void MakeFullSubjectName(
			LPSTR subjectBuffer);

		void SetSubjectWindowName();

		static unsigned int GetIndex();

		PROJECT * GetParentProject()
			{
			return (PROJECT *)GetObject(parentProjectUID);
			}

		BOOL IsSelected();

		void SetSelected(
			BOOL set);

		void SetMouseOffset(
			POINT newOffset);

		POINT GetMouseOffset();

		void GetBoundingRectangle(
			HDC hdc,
			LPRECT boundingRect);

		void SetPositionInProject(
			POINT newPosition);

		POINT GetPositionInProject();

		void CalculateMouseOffset(
			POINT mouse);

		static void IncrementIndex(
			int amount = 1);

		SERUID AddFunction(
			ATYPEID theType,
			POINT newLocation);

		RUNTIMEUID GetFunctionList();

		RUNTIMEUID GetCommentList();

		SERUID CreateObjInst(
			LPOBJDESC	pObjDesc,
			BOOL      aliasFlag,
			SERUID 		typeUID,
			pAObjMessage creationInfo=NULL);

		BOOL AddObjInst(
			LPOBJDESC objDesc,
			BOOL aliasFlag,
			LPPOINT location,
			LPCSTR objName,
			BOOL dragAndDrop,
			LPRECT fromRect = NULL);

		OBJINST * NewObjInst(
			ATYPEID typeID,
			BOOL aliasFlag,
			pAObjMessage creationInfo=NULL);

		BOOL IsGridOn();

		void SetGrid(
			BOOL set);

	 void SetBigIconFlag()
		{
		status|=SJT_BIGICON;
		}

	 void ClearBigIconFlag()
		{
		status&=(~SJT_BIGICON);
		}

	 BOOL TestBigIconFlag()
		{
		return(status & SJT_BIGICON);
		}

		SERUID AddComment(
			POINT newLocation,
			LPCSTR briefC,
			LPCSTR fullC);

		void GetSubjectWindowBounds(
			LPRECT boundsRect);

		void GetSubjectClientBounds(
			LPRECT bounds);

		RUNTIMEUID GetObjectList()		{return theObjects;}

		RUNTIMEUID GetSubTypesList()	{return theSubTypes;}

		RUNTIMEUID GetObjGroupList();

		LPOBJTYPE GetTypePtr(
			long typeID);

		SERUID IncTypeCount(
			long typeID,
			BOOL realObject);

		long DecTypeCount(
			long typeID,
			BOOL realObject);

		BOOL PointInTextRect(
			HDC hdc,
			POINT thePoint);

		void GetTextRect(
			HDC hdc,
			LPRECT textRect);

		void GetDividerRect(
			LPRECT bounds);

		void NormalizeClientBounds(
			LPRECT bounds);

		LPCSTR GetName();

		void SetName(
			LPCSTR newName);

		BOOL Compile(
			LPEXEHEAD lpExeHead,
			BOOL updateStatus);

		void UpdateUIDs(
			BOOL updateStatus);

		BOOL DoCopy(
			BOOL doDelete);

		void DeleteFuncsOnly();

		BOOL DoPaste(
			LPCLIPSJT clipSjt);

		POINT GetOriginOffset()
			{
			return originOffset;
			}

		void SetOriginOffset(
			POINT newOffset)
			{
			originOffset = newOffset;
			}

		int GetObjIconHeight();

		void GetLogBounds(
			LPRECT rect)
			{
			*rect = logBounds;
			}

		static short LinkAliasObject(
			OBJINST *  pAliasObj,
			BOOL          displayErrorsFlag);

		void ShowObject(
			SERUID  objID);

		short LinkAllAliasObjects(
			BOOL displayErrorsFlag);

		void UnSelectSubjectWindow();

		void SetSignalNamesFlag()
			{
			status|=SJT_SIGNALNAMES;
			}

		BOOL TestSignalNamesFlag()
			{
			return (status & SJT_SIGNALNAMES)?TRUE:FALSE;
			}

		void ClearSignalNamesFlag()
			{
			status&=(~SJT_SIGNALNAMES);
			}

		void SetSortedFlag()
			{
			status|=SJT_SORTED;
			}

		void ClearSortedFlag()
			{
			status&=(~SJT_SORTED);
			}

		BOOL TestSortedFlag()
			{
			return (status & SJT_SORTED);
			}

		void ChangeSignalDisplay();

		BOOL ObjectsSelected();

		BOOL ItemsSelected();

		BOOL FunctionsSelected();

		void ClearAllFlowStops();

		void ToggleFlowStops();

		BOOL FlowStopsSet();

		BOOL FlowsSelected();

		FLOW * GetDesignFlow(
			LPEXEFLOW exeFlow);

		void CalculateObjectFlows(
			LPRECT unionBounds);

		void CalculateObjectFlows();

		void CalculateAllFlowPaths();

		void ShowObject(
			OBJINST * objPtr);

		void DoPrint(
			LPPRINTER printPtr);

		void CleanFuncsOfPrint();

		short CheckSubject(
			BOOL updateStatus);

		short CheckFunctions(
			BOOL updateStatus);

		void OnlyUnselectAll(
			SERUID activeGroup = NULLSERUID);

		void OnlyUnselectGroups(
			SERUID activeGroup);

		short CheckObjects(
			BOOL updateStatus,
			BOOL selectedOnlyFlag,
			BOOL displayResult);

		int GetObjectCount()
			{
			return nObjectsCount;
			}

		void DecObjectsCount()
			{
			nObjectsCount--;
			}

		void IncObjectCount()
			{
			nObjectsCount++;
			}

		void UpdateCountBox()
			{
			if(windowPtr)
				((LPSUBJECTWINDOW)windowPtr)->DrawNumObjWindow();
      }

		void CheckForErrors();

		BOOL CalculateLogBounds(
			BOOL forAllStuff = FALSE);

		void CenterSelection();

		int GetFunctionCount()
			{
			return (int)(((RECORDBLOCK *)GetObject(theFunctions))->GetCount());
			}

		int GetCommentCount()
			{
			return (int)(((RECORDBLOCK *)GetObject(theComments))->GetCount());
			}

		void SelectAll();

		WORD ExportUPSF();

		BOOL ImportUPSF();

		int UPSFFuncToIndex(
			SERUID funcUID);

		SERUID UPSFIndexToFunc(
			int funcIndex);

		int UPSFObjToIndex(
			SERUID objUID);

		SERUID UPSFIndexToObj(
			int objIndex);

		BOOL MakeUPSFObjects();

		BOOL MakeUPSFFunctions();

		BOOL ImportUPSFObjects();

		BOOL ImportUPSFFunctions();

		BOOL PostUPSFImport();

		BOOL ImportUPSFComments();

		BOOL ImportUPSFGroups();

		void UPSFAdjustOriginOffset();

		void PostPaste(
			LPCLIPSJT clipSjt);

    BOOL RenameAliases(
			LPOBJINST  theMaster,
      LPCSTR      newName);

    BOOL IsAliased(
			LPOBJINST  theMaster);

    BOOL RenameConnectedParams(
			LPOBJINST  theObject);

		void RedrawLinkedAliases(
			LPOBJINST sharedObjPtr,
			BOOL      doUpdate);

		BOOL SetPrivateColors();

		void RedrawAllFunctions();

		void HighlightObj(
			LPOBJINST objPtr);

		void HighlightWorkbenchItem(
			LPFUNCWINOBJ itemPtr,
			BOOL flash,
			BOOL select);

		void HighlightParameter(
			LPPARAM paramPtr);

		void GetSubjectLogicalBounds(
			LPRECT rect);

		short ValidateFlows();


		LPSUBTYPE GetSubTypePtr(
			ATYPEID typeID);

		void SetActiveGroup(
			SERUID group)
			{
			activeGroup = group;
			}

		SERUID GetActiveGroup()
			{
			return activeGroup;
			}

		void GetDefaultGroupLocation(
			LPPOINT pt);

		LPOBJGROUP GetActiveGroupPtr()
			{
			return (LPOBJGROUP) ((activeGroup != NULLSERUID)?GetObject(activeGroup):
				NULL);
			}

		BOOL ObjectsToExistingGroup(
			LPOBJGROUP dstGroup,
			int insertIndex,
			BOOL moveFunctions);

		BOOL ObjectsToNewGroup(
			POINT mouse,
			BOOL moveFunctions,
			LPSERUID  newGroupUID);

		void SUBJECT::BuildDragObjList(
			LPLIST groupList,
			LPLIST dragObjects);

		void ClearAllChainFlags();


		BOOL AddAliasObjects(
			LPLIST aliasList);

		AOBJECTID CreateDynamicObject(
			ATYPEID typeID,
			APROTOCOLID protocolID);

		BOOL GroupsSelected();

		void SetSelectedGroupsIconSize(
			WORD newSize);

		BOOL ObjectsInGroupsSelected();

		void MakeObjectsPersistent();

		void MakeObjectsNonPersistent();

		BOOL MakeSureRectOnScreen(
			LPRECT rt);

		void FitLogBoundsToClientBounds();

		BOOL DestroyObject(
			LPOBJINST objPtr);

		BOOL RenameObject(
			LPOBJINST objPtr,
			LPCSTR newName);

		SERUID CopyGroup(
			LPOBJGROUP groupPtr,
			POINT newLocation);

		int GetGroupCount()
			{
			return (int)((LPRECORDBLOCK)GetObject(theObjGroups))->GetCount();
      }
	
		void ResetActiveGroup(
			SERUID newActiveGroup);

		void ExistingSubjectPostPaste();

		BOOL DuplicateCaptionLocation(
			POINT origin);

		void AdjustClientToLogBounds();

};

////////////////////////////////////////////////////////////////////////////

#endif
