#ifndef FLOW_H
#define FLOW_H

#include "seruid.h"
#include "flowflgs.h"
#include "serrec.h"
#include "classdef.h"
#include "dbstring.h"
#include "sereclib.h"
#include "progsys.h"

#define NO_ORDER  -1
//#define NO_SIGNAL -1
#define DEFAULT_SIGNAL 0

//used in the static routines which bring up the popup.
//There flags whether to get the signals from an object or function
#define FLOW_FROMOBJECT 	1
#define FLOW_FROMFUNCTION 2


#define FLOW_NORMALCONSTRUCTOR 	0
#define FLOW_PASTECONSTRUCTOR 	1
#define FLOW_UPSFCONSTRUCTOR 		2
class FLOW:public SERIUSRECORD
	{
	friend BOOL APIENTRY OKludgeReportChangedSignals(
			AOBJECTID objID,
			long oldSignal,
			long newSignal);

	private:

		static int flowCount;
		static HPEN flowPen;
		static HFONT font;
		static HBITMAP redStopSign;
		static HBITMAP greenStopSign;
		static HBITMAP disabledStopSign;
		static HBITMAP redSignMask;
		static HBITMAP greenSignMask;
		static RECT animateRect;
		static RECT dstRect;
		static POPUPMENU * staticPopupPtr;

		SERUID flowToUID;
		SERUID sourceUID;
		LONG			signal;
		RECT		signalRect;
		POINT		controlPt1;
		POINT		controlPt2;
		long		 status;
		RECT		flowBounds;
		COLORREF		signalColor;
		DBSTRING name;
		int order;

		void CalculateSignalRect(
			POINT midPoint);

		void SetSignalText();

		static BOOL Initialize();

		static void Uninitialize();

		void AnimateStop(
			SUBJECT * subjectPtr);

		void GetVisualName(
			LPTSTRING visName);

	public:

		FLOW(
			SERUID destinationUID,
			SERUID sourceUID,
			long   newStatus,
			LONG   newSignal,
			WORD 	 constructorType);

		FLOW(
			void * thisPtr);

		~FLOW();

		void Draw(
			HDC hdc,
			LPRECT drawBounds);

		void CalculatePath();

		void ChangeSignal(
			long newSignal);

		BOOL IsSelected();

		void SetSelected(
			BOOL set);

		BOOL PointInFlow(
			POINT pt)
			{
			return PtInRect((LPRECT)&signalRect, pt);
			}

		void ShiftSignalRect(
			int amount);

		void SetDraw(
			BOOL set);

		SERUID GetDestinationUID()
			{
			return flowToUID;
			}

		SERUID GetSourceUID()
			{
			return sourceUID;
			}

		void DrawSignalRect(
			HDC hdc,
			BOOL offScreen = FALSE);

		void GetSignalRect(
			LPRECT rect)
			{
			*rect = signalRect;
			}

		BOOL SignalInRect(
			LPRECT rect)
			{
			RECT temp;

			return IntersectRect((LPRECT)&temp, rect, (LPRECT)&signalRect);
			}

		void BringUpMenu(
			HWND parent);

		long GetStatus()
			{
			return status;
			}

		LONG GetSignal()
			{
			return signal;
			}


		BOOL FlowInRect(
			LPRECT rect)
			{
			RECT bounds;
			return (IntersectRect((LPRECT)&bounds, (LPRECT)&flowBounds, rect) ||
				IntersectRect((LPRECT)&bounds, (LPRECT)&signalRect, rect));
			}

		void UpdateUIDs();

		void SetOrder(int
			newOrder)
			{
			order = newOrder;
			}

		void DecOrder()
			{
			order--;
			}

		void IncOrder()
			{
			order++;
			}

		int GetOrder()
			{
			return order;
			}

		BOOL TestSrcObjFlag()
			{
			return (status & FL_OBJSOURCE)?TRUE:FALSE;
			}

		void ChangeSignalDisplay(
			BOOL showSignalNames)
			{
			if(showSignalNames)
				status|=FL_SIGNALNAME;
			else
				status&=~FL_SIGNALNAME;

			ChangeSignal(signal);
			}

		void SetStop(
			LPRECT bounds);

		void SetStopBits();

		void ClearStop(
			LPRECT bounds);

		void ClearStopBits();

		BOOL TestStopFlag()
			{
			return (status & FL_STOP)?TRUE:FALSE;
			}

		void ToggleStop(
			LPRECT bounds)
			{
			if(TestStopFlag())
				ClearStop(bounds);
			else
				SetStop(bounds);
			}

		void DisableStop();

		void EnableStop();

		void SetStoppedAtFlowFlag()
			{
			status|=FL_STOPPED;
			}

		void ClearStoppedAtFlowFlag()
			{
			status&=~FL_STOPPED;
			}

		void ClearStoppedFlow();

		void DoStopAnimation(
			HDC hdc,
			int loopNum);

		void FlashFlow();

		void SetPrintFlag()
			{
			status |= FL_PRINT;
			}

		BOOL TestPrintFlag()
			{
			return (status & FL_PRINT)?TRUE:FALSE;
			}

		void ClearPrintFlag()
			{
			status &= ~FL_PRINT;
			}

		void SetClosePrintFlag()
			{
			status |= FL_CLOSEPRINT;
			}

		BOOL TestClosePrintFlag()
			{
			return (status & FL_CLOSEPRINT)?TRUE:FALSE;
			}

		void ClearClosePrintFlag()
			{
			status &= ~FL_CLOSEPRINT;
			}

		void ClearPrintingFlags()
			{
			ClearClosePrintFlag();
			ClearPrintFlag();
			}

		void DoPrint(
			LPPRINTER printPtr,
			PRINTFLOWSTACK * flowStackPtr,
			int indent);

		void BuildLabels(
			PRINTFLOWSTACK * flowStackPtr);

		void GetFlowBounds(
			LPRECT bounds)
			{
			UnionRect(bounds, &flowBounds, &signalRect);

			//compensate for the double width pen used to draw flow lines
			InflateRect(bounds, 1, 1);
			}

		WORD ExportUPSF();

		BOOL PostUPSFImport();

		BOOL TestSignalNotLinkedFlag()
			{
			return (status & FL_SIGNALNOTLINKED)?TRUE:FALSE;
			}

		void SetSignalNotLinkedFlag()
			{
			status|=FL_SIGNALNOTLINKED;
			}

		void ClearSignalNotLinkedFlag()
			{
			status&= ~FL_SIGNALNOTLINKED;
			}

		void PostPaste();

		BOOL AddFlowToZlist(
			LPRECT area,
			LPZLIST zlistPtr);

		LPCSTR GetName()
			{
			return name;
			}

		static BOOL SetSysColors();

		static BOOL ChooseInitialSignal(
			SERUID fillerUID,
			SERUID fillerType,
			POINT screenLocation);

		static long GetInitialSignal();

		short ValidateFlow();

		LPFUNCINST GetDestinationFunc()
			{
			return (LPFUNCINST)GetObject(flowToUID);
			};

		void SelectChain(
			HDC hdc,
			LIST * selectedFuncs,
			LIST * selectedFlows);

		void UpdateSignalIntegrity();

		BOOL ImportUPSF();

		void SetLinkToExeFlag()
			{
			status |= FL_LINKTOEXEFLOW;
			}

		void ClearLinkToExeFlag()
			{
			status &= ~FL_LINKTOEXEFLOW;
			}

		BOOL TestLinkToExeFlag()
			{
			return (PROGSYSTEM::TestRunTimeFlag() && (status & FL_LINKTOEXEFLOW))?
				TRUE:FALSE;
			}
	};


#endif
