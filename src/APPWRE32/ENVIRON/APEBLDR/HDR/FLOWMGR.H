#ifndef __FLOWMGR_H
#define __FLOWMGR_H

#include "classdef.h"
#include "seruid.h"
#include "sereclib.h"
#include "recblk.h"

class FLOWMGR
	{
	private:

		RUNTIMEUID flowList;

		BOOL ImportUpsfFlowData(
			SERUID srcUID,
			SERUID dstUID,
			BOOL srcIsFunc);

	protected:

		RUNTIMEUID GetFlowList()
			{
			return flowList;
			}


	public:

		FLOWMGR();

		FLOWMGR(
			void * thisPtr);

		~FLOWMGR();

		FLOW * AddFlow(
			SERUID destinationUID,
			SERUID sourceUID,
			long   status,
			LONG newSignal,
			int order,
			WORD constructorType);

		void RemoveFlow(
			FLOW * flowPtr);

		void RecalculateFlowPaths();

		void RecalculateFlowPaths(
			LPRECT unionBounds);

		void DrawFlows(
			HDC hdc,
			LPRECT refreshRect,
			LPRECT drawBounds);

		FLOW * FlowFromPoint(
			CONST POINT &pt);

		BOOL AddFlowsInRectToList(
			HDC hdc,
			LPRECT rect,
			LIST * list);

		void AddSelectedFlowsToList(
			LIST * list);

		void AddAllFlowsToList(
			LIST * list);

		void CompileFlows(
			RUNTIMEUID exeFlowList);

		void UpdateUIDs();

		void ClearFlows(
			RUNTIMEUID clipFlowBlock,
			BOOL       sourceSelected,
			BOOL       clearAll);

		void CopyFlows(
			RUNTIMEUID clipFlowBlock,
			BOOL       sourceSelected,
			BOOL       clearAll);

		void DrawSelectedSignals(
			HDC hdc,
			LPRECT refreshRect);

		void DrawUnSelectedSignals(
			HDC hdc,
			LPRECT refreshRect,
			LPRECT screenRect,
			LPZLIST zlistPtr);

		void AddUnSelectedToZlist(
			LPRECT screenRect,
			LPZLIST zlistPtr);

		void ChangeSignalDisplay(
			BOOL showSignalNames);

		BOOL ToggleStops(
			LPRECT bounds);

		BOOL ClearAllStops(
			LPRECT bounds);

		BOOL EnableStops(
			HDC hdc);

		BOOL DisableStops(
			HDC hdc);

		BOOL IsStopSet();

		FLOW * GetDesignFlow(
			LPEXEFLOW exeflow);

		int GetFlowCount();

		void UnselectFlows();

		WORD FlowExportUPSF(
				LPSTR fieldName,
				int * numExported);

		BOOL FlowImportUPSF(
			LPSTR fieldName,
			LPSTR fieldCount,
			SERUID srcUID,
			BOOL srcIsFunc);

		BOOL FlowPostUPSFImport();

		void PostPaste();

		short ValidateFlows(
			BOOL srcIsFunc);

		BOOL HasConnectedFlows()
			{
			return (((LPRECORDBLOCK)GetObject(flowList))->GetCount() > 0)?TRUE:
				FALSE;
			}

		void PushFlows(
    	LPFLOWSTACK flowStack);


		void VerifySignalNames();

		void HandleBadFlows();

		void GetUnionFlowBounds(
			LPRECT bounds);

		void GetBoundsOfIntersectingFlows(
			LPRECT bounds, 
			SERUID uid);
	};

#endif
