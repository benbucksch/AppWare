#ifndef __OBJGROUP_H
#define __OBJGROUP_H

#include "classdef.h"
#include "fwinobj.h"
#include "dbstring.h"
#include "subject.h"
#include "memclass.h"
#include "serblk.h"

#define OBJGROUP_LARGEICONS 				0x00000001
#define OBJGROUP_ACTIVE							0x00000004
#define OBJGROUP_EDITFIELD					0x00000008

#define OBJGROUP_NOHIT							    0x0000
#define OBJGROUP_CAPTIONHIT 						0x0001
#define OBJGROUP_LEFTBORDERHIT 		  		0x0002
#define OBJGROUP_RIGHTBORDERHIT		  		0x0004
#define OBJGROUP_ICONHIT								0x0008
#define OBJGROUP_TEXTHIT								0x0010
#define OBJGROUP_NIPPLEHIT							0x0020
#define OBJGROUP_DRAGBORDERHIT					0x0040

#define OBJGROUP_RESIZEBORDERHIT (OBJGROUP_LEFTBORDERHIT | OBJGROUP_RIGHTBORDERHIT)
#define OBJGROUP_BORDERHIT (OBJGROUP_RESIZEBORDERHIT | OBJGROUP_DRAGBORDERHIT)

#define OBJGROUP_LARGEICONSIZE 32
#define OBJGROUP_SMALLICONSIZE 16

#define OBJGROUP_DRAWBOX	 			0x0001
#define OBJGROUP_NORMALNIPPLE 	0x0002
#define OBJGROUP_INVERTNIPPLE		0x0004
#define OBJGROUP_ERASEBOX				0x0008

//make sure that the numbers are significanlty large so
//that if the insert index is -1, meaning at the top, it's
//not confused to be some special type of insertion
#define OBJGROUP_BOTTOMINSERTION 				-10
#define OBJGROUP_DEFAULTINSERTION 			-11
#define OBJGROUP_AFTERSELECTEDINSERTION -12

#define OBJGROUP_TOTALREFRESH 		0x0001
#define OBJGROUP_PARTIALREFRESH		0x0002
#define OBJGROUP_NOREFRESH				0x0004

#define OBJGROUP_NONESELECTED -1 	//must be -1 so that incrementing
																	//will get to zero.

//no object has yet been clicked upon
#define OBJGROUP_NOMARKSET	OBJGROUP_NONESELECTED

#define OBJGROUP_NAMEEDIT 1000
#define OBJGROUP_MAXNAMELENGTH 64

#define OBJGROUP_SETSMALLICONS 1
#define OBJGROUP_SETLARGEICONS 2


class OBJECTTRAVERSEKEY
	{
	friend OBJGROUP;

	private:
		LPSERUID indexesPtr;
		int index;

	public:
		OBJECTTRAVERSEKEY()
			{
      indexesPtr = 0;
			}
	};

typedef OBJECTTRAVERSEKEY * LPOBJECTTRAVERSEKEY;


//storage class for info only needed during design time
class GROUPINFO:public MEMORY
	{
	friend OBJGROUP;
	friend CLIPGROUP;

	private:

		GROUPINFO():mark(OBJGROUP_NOMARKSET)
			{
			}

		int mark; //the index of the last item clicked on


	};

typedef GROUPINFO * LPGROUPINFO;

class OBJGROUP:public FUNCWINOBJ
	{
	friend AOBJECTID ALMAPI	AObjGetNextObject(
		ATYPEID typeID,
		LPLONG theGetKey);

	private:
		static long groupCount;
		static HPEN insertMarkPen;

		static HFONT small_normalFont;
		static HFONT small_sharedFont;
		static HFONT small_aliasFont;
		static HFONT large_normalFont;
		static HFONT large_sharedFont;
		static HFONT large_aliasFont;

		static HFONT small_persistentNormalFont;
		static HFONT small_persistentSharedFont;
		static HFONT small_persistentAliasFont;
		static HFONT large_persistentNormalFont;
		static HFONT large_persistentSharedFont;
		static HFONT large_persistentAliasFont;

		static HFONT captionFont;
		static HBITMAP nippleNormal;
		static HBITMAP nippleInverted;
		static HBITMAP nippleMask;
		static HBITMAP selRectPattern;
		static HBITMAP nippleSelected;
		static HBRUSH	 selRectBrush;
		static HWND editField;
		static WNDPROC oldEditProc;
		static int largeSlot;
		static int smallSlot;
		static int largeVBorder;
		static int smallVBorder;
		static int smallTextBorder;
		static int largeTextBorder;
		static SERUID editObject;
		static char nameBuffer[];
		static WNDPROC oldObjValueProc;

		RUNTIMEUID  objIndexes;
		RECT	      objBounds;
		long	      groupStatus;
		DBSTRING    groupName;
				
		int objCount;

		int selObjCount;	 //the number of selected objects

		////////////////////////////////////
		
		LPGROUPINFO groupInfo;
		LOGFONT			futureFont;

		void DrawSelection(
			HDC hdc);

		static BOOL Initialize();

		static BOOL UnInitialize();

		void DrawCaption(
			HDC hdc);

		void GetNipplePoint(
			LPPOINT nip,
			LPOBJINST objPtr);

		LPOBJINST ObjectFromY(
			int y);

		int DoInsert(
			SERUID objUID,
			int index);

		int GetNewObjIndex(
			int y);


		void SelectObjRange(
			HDC hdc,
			int start,
			int end,
			WORD refresh=OBJGROUP_TOTALREFRESH);

		void DrawAllObjects(
			HDC hdc,
			LPRECT drawRect,
			BOOL drawOffScreen);

		void VerifyInsertIndex(
			int * index);

		void DrawSelectionRect(
			HDC hdc);

		int GetCaptionHeight();

		void DrawTheSelectionRects(
			HDC hdc,
			LPPOINT offset = NULL);

		static BOOL CALLBACK ChangeGroupName(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		BOOL ChangeSharedState(
			BOOL makeShared);

		void DoGroupResize(
			POINT offset,
			WORD dragBorder);

		BOOL DoObjectNameChange(
			LPOBJINST objPtr,
			LPCSTR newName);

		void ObjectValueDialog(
			LPCSTR valueString);

		static BOOL CALLBACK ObjValueDlgProc(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);


		static LRESULT CALLBACK ObjValueEditProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void SizeGroupToFitObject();

	public:

		OBJGROUP(
			SERUID objUID,
			POINT  location,
			SERUID ownerSubject,
			LPCSTR	newName);

		OBJGROUP(
			SERUID ownerSubject,
			POINT newLocation,
			long baseStatus,
			long groupStats,
			int width,
			int objectCount,
			LPCSTR newName,
			LPGROUPINFO infoPtr);

			OBJGROUP(
    	void * thisPtr);

		~OBJGROUP();

		void Draw(
			HDC hdc);

		BOOL Draw(
			HDC hdc,
			LPRECT drawRect)
			{
			return Draw(hdc, drawRect, TRUE); //default to drawing offscreen
			}

		BOOL Draw(
			HDC hdc,
			LPRECT drawRect,
			BOOL drawOffScreen);

		BOOL DrawGroupBody(
			HDC hdc,
			LPRECT drawRect,
			BOOL drawOffScreen);

		void GetRect(
			LPRECT rect);

		void GetTotalBounds(
			LPRECT bounds);

		void DrawDrag(
			HDC hdc,
			POINT offset);

		void Move(
			POINT offset);

		void SetLocation(
			POINT p);


		BOOL AddObjectToZlist(
			LPRECT area,
			LPZLIST zlistPtr);

		LPOBJINST NewObjInst(
			ATYPEID 		typeID,
			LPCSTR		objName,
			BOOL      aliasFlag);


		BOOL TestLargeIconsFlag()
			{
			return (groupStatus & OBJGROUP_LARGEICONS)?TRUE:FALSE;
			}

		void SetLargeIconsFlag()
			{
			groupStatus |= OBJGROUP_LARGEICONS;
			}

		void ClearLargeIconsFlag()
			{
			groupStatus &= ~OBJGROUP_LARGEICONS;
			}

		void SetEditFieldFlag()
			{
			groupStatus |= OBJGROUP_EDITFIELD;
			}

		void ClearEditFieldFlag()
			{
			groupStatus &= ~OBJGROUP_EDITFIELD;
			}

		BOOL TestEditFieldFlag()
			{
			return (groupStatus & OBJGROUP_EDITFIELD)?TRUE:FALSE;
			}

		static BOOL SetSysColors();

		void CalculateBounds(
			BOOL sizeToHoldObject = FALSE);

		int AddObject(
			SERUID objUID,
			LPPOINT locationPtr = NULL);

		void DrawObject(
			LPOBJINST objPtr,
			BOOL drawOffscreen = TRUE);

		void DrawObject(
			HDC hdc,
			LPOBJINST objPtr,
			BOOL drawOffScreen = TRUE);

		void DrawObject(
			HDC hdc,
			LPOBJINST objPtr,
			LPRECT bounds,
			BOOL drawOffScreen = TRUE);

		WORD GetHitArea(
		 POINT mouse,
		 LPOBJINST * objPtr);

		WORD OnResizeBorder(
			POINT mouse);

		LPOBJINST ObjectFromPoint(
			POINT pt);

		int GetIconSize()
			{
			return (TestLargeIconsFlag())?OBJGROUP_LARGEICONSIZE:
				OBJGROUP_SMALLICONSIZE;
			}

		void DrawObjectBox(
			HDC hdc,
			LPOBJINST objPtr);

		BOOL OnNipple(
			POINT pt);

		int GetSlotHeight();

		void GetNippleTip(
			LPPOINT nip,
			LPOBJINST objPtr);

		void DrawNipple(
			HDC hdc,
			POINT nip,
			BOOL invert,
			BOOL selected);

		void HighlightConnection(
			HDC hdc,
			WORD operations,
			LPOBJINST objPtr);

		BOOL EditObject(
			LPOBJINST objPtr);

		void SetActiveFlag()
			{
			groupStatus |= OBJGROUP_ACTIVE;
			GetSubjectPtr()->SetActiveGroup(GetUID(this));
			}

		void ClearActiveFlag()
			{
			LPSUBJECT subjectPtr = GetSubjectPtr();

			groupStatus &= ~OBJGROUP_ACTIVE;
			//because of extended selection, it's possible to have more groups appear active
			//than the actual active group.  So make sure we're the actual active group before
			//clearing the subject's active group.
			if(subjectPtr->GetActiveGroup() == GetUID(this))
				GetSubjectPtr()->SetActiveGroup(NULLSERUID);
			}

		BOOL TestActiveFlag()
			{
			return (groupStatus & OBJGROUP_ACTIVE)?TRUE:FALSE;
			}

		void UnSelect(
			HDC hdc);

		void Select(
			HDC hdc);


		void RectSelect(
			HDC hdc);

		void RectUnSelect(
			HDC hdc);

		BOOL DropScoredHit(
			LPRECT cursorRect);

		BOOL ObjectIntersectsRect(
			LPRECT bigRect);

		void AnimateObjectAdd(
			HDC hdc,
			LPOBJINST objPtr,
			LPRECT cursorRect);


		void SelectObj(
			HDC hdc,
			LPOBJINST objPtr,
			WPARAM wParam); // SKJ

		BOOL UnselectObjects(
			HDC hdc,
			BOOL redraw,
			SERUID staySelectedUID);

		void UnselectAllObjects(
			HDC hdc)
			{
			UnselectObjects(hdc, (hdc == NULL)?FALSE:TRUE, NULLSERUID);
      }

		void GetObjectRect(
			int index,
			LPRECT rt);

		void ClearActiveState(
			HDC hdc);

		void SetActiveState(
			HDC hdc);

		int GetSelObjCount()
			{
			return selObjCount;
			}

		void IncSelObjCount()
			{
			selObjCount++;
			}

		int GetObjCount()
			{
			return objCount;
			}

		void HandleKeyDown(
			WPARAM wParam,
			LPARAM lParam);

		void HandleCharMessage(
			WPARAM wParam,
			LPARAM lParam);

		LPOBJINST ObjectFromIndex(
			int index);

		int GetTopSelectedIndex();

		void GetObjTextRect(
			LPRECT const objRect,
			LPRECT textRect);

		static LRESULT CALLBACK EditProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		HWND CreateEditField(
			LPOBJINST objPtr);

		BOOL EditObjTitle(
			LPOBJINST objPtr,
			UINT message,
			WPARAM wParam,
			LPARAM lParam,
			BOOL selectText,
			BOOL drawAll);

		HFONT GetObjFont(
			LPOBJINST objPtr);

		int GetVBorder()
			{
			return (groupStatus & OBJGROUP_LARGEICONS)?largeVBorder:smallVBorder;
			}

		int GetTextBorder()
			{
			return (groupStatus & OBJGROUP_LARGEICONS)?largeTextBorder:smallTextBorder;
			}

		LPOBJINST GetEditObject();

		BOOL KillEditField(
			BOOL redrawObj,
			BOOL setObjName,
			BOOL killObjSelection);

		void InvalidateObjRect(
			LPOBJINST objPtr);

		void ValidateObjRect(
			LPOBJINST objPtr);

		void SelectObjChain(
			HDC hdc,
			LPOBJINST objPtr,
			LPLIST selectedFuncs,
			LPLIST selectedFlows,
			WPARAM wParam,
			BOOL drawAll);


		void SelectAllObjChains(
			HDC hdc,
			LPLIST selectedFuncs,
			LPLIST selectedFlows,
			BOOL drawAll);


		BOOL HandleInteriorDrag(
			HDC hdc,
			WPARAM wParam,
			POINT mouse);

		int InsertObject(
			SERUID objUID,
			int index);

		void UnSelectAndLeaveActive(
			HDC hdc);

		void EraseSelection(
			HDC hdc);

		void DrawObjectDrag(
			HDC hdc,
			POINT offset);

		int GetHighlightIndex(
			POINT mouse);

		void DrawInsertMark(
			HDC hdc,
			int index);


		LPOBJINST GetNextSelObj(
			int * selIndex,
			int * selCount);

		void ResetObjIndexes();

		int OnlyRemoveObject(
			LPOBJINST objPtr);

		BOOL InsertObjects(
			LPLIST groupList,
			LPLIST dragObjects,
			int insertIndex,
			BOOL	moveFunctions);

		int RemoveAllSelObjects(
			BOOL deleteObj,
			int * insertIndex=NULL);

		BOOL EditGroupName(
			HDC hdc);

		BOOL ShareObjects()
			{
			return ChangeSharedState(TRUE);
  		}

		BOOL UnShareObjects()
			{
			return ChangeSharedState(FALSE);
			}

		int GetMark()
			{
			return groupInfo->mark;
			}

		int GetWidth()
			{
			return objBounds.right - objBounds.left + 1;
			}

		RUNTIMEUID GetObjBlock()
			{
			return objIndexes;
			}

		LPSERUID GetIndexesPtr()
			{
			return (LPSERUID)((LPSERIUSBLOCK)GetObject(objIndexes))->GetData();
      }
    
		void DeleteObjInst(
			OBJINST * pObjInst);

		int DeleteObjects(
			BOOL deleteAll);


		LPCSTR GetName()
			{
			return (LPCSTR)groupName;
			}

		long GetGroupStatus()
			{
			return groupStatus;
			}

		BOOL ObjectsSelected()
			{
			return (selObjCount > 0)?TRUE:FALSE;
			}

		short PostReadInit();

		void CalculateObjFlows();

		int InsertUndoneObject(
			LPOBJINST objPtr);


		void DrawBorderDrag(
			HDC hdc,
			POINT offset,
			WORD dragBorder);

		int GetMinGroupWidth();

		void ResizeGroupHorizontally(
			POINT offset,
			POINT adjustedOffset,
			WORD dragBorder);

		void HandleSelectionForArrowKeys(
			HDC hdc,
			LPOBJINST objPtr,
			BOOL unselect);

		void SetGroupIconSize(
			WORD newSize);

		void MakeObjectsPersistent();

		void MakeObjectsNonPersistent();

		int DestroyObject(
			LPOBJINST objPtr);


		BOOL RenameObject(
			LPOBJINST objPtr,
			LPCSTR newName);

		LPGROUPINFO GetInfoPtr()
			{
			return groupInfo;
			}

		BOOL CopyObjects(
			LPOBJGROUP sourceGroup,
			RUNTIMEUID objectBlock);


		void InitializeTraverseKey(
			LPOBJECTTRAVERSEKEY key)
			{
			key->index = 0;
			key->indexesPtr = GetIndexesPtr();
			}

		LPOBJINST GetNextObject(
			LPOBJECTTRAVERSEKEY key)
			{
			ADBG_ASSERT_E(key->indexesPtr != 0, return 0, "Unitialized key passed in to"
				" OBJGROUP::GetNextObject()");

			if((key->index >= 0) && (key->index < objCount) && key->indexesPtr)
				return (LPOBJINST)GetObject(key->indexesPtr[key->index++]);
			else
				{
				key->indexesPtr = 0;
				return 0;
        }
      }

		BOOL OKToDragObjects()
			{
			return ((selObjCount > 0) && (!IsSelected()))?TRUE:FALSE;
			}

		void GetSelectedRect(
			LPRECT bounds);

		WORD ExportUPSF();

		BOOL ImportUPSF();

		BOOL InsertAllBlockObjects(
			RUNTIMEUID objectBlock);

		void MoveNearRectangle(
			LPRECT rt);

		void GetObjectBox(
			LPRECT box,
			LPOBJINST objPtr);

	};

#endif
