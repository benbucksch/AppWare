#ifndef __SJTWIN_H
#define __SJTWIN_H

#include "classdef.h"
#include "compuid.h"
#include "basewin.h"

// masks to mark what kind of drag connections are legal
#define FLOW_IN 		1
#define FLOW_OUT 		2
#define IN_PARAM 		4
#define OUT_PARAM 	8
#define OUT_OBJ   	16

typedef struct ASCROLLINFO
	{
	WORD 	operations;
	POINT scrollAmount;
	} * LPASCROLLINFO;

struct SW_ENTITY
	{
	SERUID uid;
	WORD type;
	};
typedef SW_ENTITY * LPSW_ENTITY;

//Subject Window Entity types
#define ENTITY_PARAM 		1
#define ENTITY_FUNCTION	2
#define ENTITY_GROUP		3
#define ENTITY_COMMENT	4
#define ENTITY_OBJECT		5
#define ENTITY_FLOW			6


class SUBJECTWINDOW:public BASEWINDOW
	{
	friend SUBJECT;

	private:
		static BOOL							registered;
		static char							className[];
		static WORD							sjtWinCount;
		static LPSTR						c_BriefBuffer;
		static LPSTR						c_FullBuffer;
		static HCURSOR					groupResizeCursor;
		static int							minWindowWidth;
		static int							minWindowHeight;
		static char							numObjMessage[];
		static int							numObjMsgWidth;
		static HBRUSH						cornerBrush;
		static WNDPROC					oldStaticWndProc;
				
		POINT										oldMouse;
		POINT										startMouse;
		LIST *								selectedObjects;
		RECT										selectRect;
		int											barOffset;
		FUNCINST *						highlightedFunc;
		int											whereHighlighted;
		int											legalConnections;
		FUNCINST *						flowFromFunc;
		OBJINST *						flowFromObj;
		OBJINST *						highlightedObj;
		SERUID									subjectUID;
		HWND										hScroll;
		HWND  									vScroll;
		LIST *								selectedFlows;
		SERUID									holdFlowUid;
		PARAM *							highlightedParam;
		PARAM *							flowFromParam;
		LIST * 							selectedParams;
		POPUPMENU *          popupPtr;
		LPZLIST									zlistPtr;
		long										newSignal;
		OBJECTSHARE *				objSharePtr;
		LONG										flags;
		int										groupHitArea;
		SERUID									highlightedGroupUID;
		int											groupHighlightIndex;
		LPOBJGROUP							resizeGroupPtr;
		HWND										hWndNumObj;
		HWND										hWndCorner;
		SW_ENTITY								unselectEntity;

		void ChangeFlowSelection(
			HDC hdc,
			WORD wParam,
			SERUID newSelected);

		void ChangeParamSelection(
			HDC hdc,
			WORD wParam,
			SERUID newSelected);


		void UnSelectParams(
			HDC hdc,
			SERUID keepSelected = NULLSERUID);

		BOOL UpdateParamNames(
			SERUID paramUID,
			BOOL isParam,
			LPRECT refreshRect);

		void RecalcFuncBounds();

		PARAM * ParamFromPoint(
			POINT pt);

		void DoSignalPopup();

			void UnSelectFlows(
			HDC hdc);

		WORD Highlight(
			HDC hdc,
			POINT mouse);

		void Register();

		void CreateScrollBars();

		void ResizeScrollBars();

		void CreateStaticWindows();

    void ResizeStaticWindows();

		void OnlyChangeSelection(
			SERUID	newSelection);

		void AnimateFunctionAdd(
			LPFUNCWINOBJ objPtr,
			LPRECT startRect = NULL);

		void FigureSelection(
			const POINT& pt,
			WORD wParam);

		void FigureDoubleClick(
			LPARAM lParam);

		void DrawDragFunctions(
			HDC hdc,
			POINT mouse);

		void SUBJECTWINDOW::MoveAllFunctions(
			HDC 		hdc,
			POINT offset,
			LPFUNCWINOBJ stayPut = NULL);

		void EndDrag(
			WPARAM wParam,
			LPARAM lParam);

		void EraseOldDragStuff(
			HDC hdc);

		BOOL AutoScroll(
			int direction,
			POINT initMouse);

		void DetermineScrollBars();

		void GreyScrollBar(
			HWND sBar);

		void SetScrollBars(
			int command,
			BOOL redraw=TRUE);

		void ScrollSubjectWindow(
			int direction,
			LPPOINT distance = NULL);

		void ArbitraryScroll(
			int direction,
			int	nCode,
			short	nPos);

		BOOL ChangeObjectSelection(
			HDC		hdc,
			SERUID	newlySelected,
			WORD	wParam);

		void UnSelectFunctions(
			HDC hdc,
			SERUID activeGroup = NULLSERUID);

		void ResetOldMouse();

		POINT VerifyOffset(
				POINT offset);

		void PurgeSelectedLists();

		BOOL SelectObjectsInRect(
			HDC hdc,
			BOOL buildZList);

		void DrawSelectRect(
			HDC hdc,
			BOOL selectObjects);

		void NormalizeRect(
			LPRECT normalRect,
			LPRECT sampleRect);

		BOOL SnapObjectToGrid(
			FUNCWINOBJ * func);

		BOOL SnapAllObjectsToGrid();

		void SnapPointToGrid(
			LPPOINT p);

		int SnapToGrid(
			LPPOINT p,
			int width,
			int height);

		void BuildSelectedList();

		void RebuildZlist();

		void CommentDialog(
			COMMENT * commentPtr);

		static BOOL CALLBACK NewComment(
			HWND   hDlg,
			UINT   message,
			WPARAM wParam,
			LPARAM lParam);


		static LPARAM CALLBACK CommentEditProc(
			HWND   hwnd,
			UINT   message,
			WPARAM wParam,
			LPARAM lParam);

		BOOL AllocateBuffers();

		void DestroyBuffers();

		void AdjustLogBounds();

		void CopyAllFunctions(
			POINT offset);

		void CalculateWindowBounds();

		void RedrawDragStuff(
			HDC hdc,
			LPASCROLLINFO scrollInfo);

		void GetBoundsOfIntersection(
			LPRECT intersectRect,
			LPRECT unionRect);

		void HandleGroupObjHit(
			HDC hdc,
			LPOBJGROUP groupPtr,
			LPOBJINST objPtr,
			UINT message,
			WORD		nButtonStates,
			const POINT&	hitPt);


		void HandleMagicKeySelection(
			HDC hdc,
			LPFUNCWINOBJ newSelection,
			WORD wParam);

		void HighlightDropGroup(
			HDC hdc,
			POINT mouse);

		BOOL MoveGroupObjs(
			HDC hdc,
			POINT mouse);

		BOOL SUBJECTWINDOW::UseRButton(
			WPARAM wParam,
			LPARAM lParam);

		BOOL SetSysColors();

		static LPARAM CALLBACK StaticWndProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

	public:

		SUBJECTWINDOW(
			SERUID  theSubjectUID,
			SERUID	theProjectUID);

		~SUBJECTWINDOW();

		LRESULT WndProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void Resize();

		SUBJECT * GetSubjectPtr();

		static SUBJECTWINDOW * GetSubjectWindow(
			HWND hwnd);

		void AddFunction(
			ATYPEID theType,
			POINT newLocation);

	 void AddFunctionExt(
	 	ATYPEID theType, 
	 	LPRECT oldLocation);

		void DrawScreen(
			HDC hdc,
			LPRECT refreshRect,
			LPASCROLLINFO scrollInfo = NULL);

		BOOL Drag(
			WPARAM wParam,
			LPARAM lParam);

		void AddComment();

		void SetAllScrollBars();

		void UpdateParams(
			SERUID updateUID,
			BOOL isFunc,
			LPRECT refreshRect);

		void RefreshClientArea(
			LPRECT area = NULL);

		BOOL DoDelete();

		void UpdateSubjectWindow(
			LPRECT refreshRect = NULL);

		void RebuildlSelectedLists();

		void UnSelectAll();

		void UnSelectAll(
			HDC hdc);

		void HighlightLinkedInputs(
			SERUID outputUID,
			HDC hdc);

		void HighlightLinkedOutput(
			PARAM * inputPtr,
			HDC hdc);

		BOOL WorkBenchObjectsSelected();

		BOOL ItemsSelected();

		BOOL FlowsSelected();

		BOOL GetSelection(
			FUNCWINOBJ * * newSelection,
			FLOW * * flowPtr,
			int * query,
			POINT mouse);

		void AdjustScrollBars();

		BOOL FunctionsSelected();

		BOOL CenterSelection();

		void SelectAllWorksheetItems();

		void OnlyUnselectAll(
			SERUID activeGroup = NULLSERUID);

		void RightButtonHelp(
			HWND hWnd,
			LPARAM lParam);

		BOOL SetPrivateColors();

		void HighlightFunc(
			LPFUNCWINOBJ winObjPtr,
			BOOL select);

		void FlashFunc(
			LPFUNCWINOBJ winObjPtr);

		void FlashParameter(
			LPPARAM paramPtr);

		SERUID GetSubjectUID()
			{
			return subjectUID;
			}

		OBJECTSHARE * GetObjSharePtr()
			{
			return objSharePtr;
			}

		void RebuildSelectedLists();

		void AddObjectToGroup(
    	HDC hdc,
			LPOBJINST objPtr,
			LPRECT cursorRect,
			BOOL newGroup);

		LPZLIST GetZlistPtr()
			{
			return zlistPtr;
			}

		void ClearSelectedChainFlags();

		BOOL EligibleDragObjects();

		LPOBJGROUP GetTargetGroup(
			LPRECT hotSpot);

		BOOL RemoveObjectFromSelectedList(
			SERUID objUID);

		void HighlightObj(
			LPOBJINST objPtr,
			BOOL unselect,
			BOOL refresh = TRUE);

		void DrawNumObjWindow();

		void ScrollWorkbench(
			POINT scrollAmount);

		BOOL GroupsSelected();

		void ChangeActiveGroup(
			HDC hdc,
			LPOBJGROUP newActiveGroup,
			BOOL drawNewGroup,
			BOOL unselectOldGroupObjs);

		void FlashObject(
 			LPOBJINST objPtr);

		void AddUIDToSelectedList(
			SERUID uid);

		BOOL Busy();
};

#endif
