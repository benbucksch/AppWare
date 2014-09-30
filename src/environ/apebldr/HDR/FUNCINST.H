#ifndef FUNCINST_H
#define FUNCINST_H

#include "classdef.h"
#include "funcmask.h"
#include "flowmgr.h"
#include "fwinobj.h"
#include "funcdesc.h"

#define FUNCSIZE 32
#define FN_DRAWBRACKET 0xffffffffL

///////////////////////////////////////////////
//error masks for CheckFunction
#define FUNC_CHECK_NOERROR			0x0000
#define FUNC_CHECK_UNLINKED			0x0001
#define FUNC_CHECK_INBEFOREOUT	0x0002

class FUNCINST:public FUNCWINOBJ, public FLOWMGR
	{
	friend PARAM;

	private:
		static int 			funcCount;
		static HBITMAP 	blackFuncArrow;
		static HBITMAP  arrowMask;
		static HBITMAP  selectionBmp;
		static HBITMAP 	shadeMask;
		static HBITMAP  shadeBmp;
		static HBITMAP  textBmp;
		static HFONT		funcFont;

		BOOL							alias;
		COMPUID 					typeID;
		LPAFUNCDESC 				infoPtr;
		int 							inFlowFlag;
		int 							outFlowFlag;
		SERUID 						subjectUID;
		RUNTIMEUID				inputParams;
		RUNTIMEUID 				outputParams;
		RECT 							boundsRect;

		void DrawSelection(
			HDC hdc);

		void DrawSelectionRect(
			HDC hdc);

		static BOOL Initialize();

		static void UnInitialize();

		void ReCalculateParams(
			POINT offset);

		void GetParams(
			BOOL figureBounds = TRUE);

	void PushPrintFlows(
		PRINTFLOWSTACK * stackPtr,
		int indent);

		BOOL ImportUpsfInputParamData(
			int index);

		BOOL ImportUpsfOutputParamData(
			int index);

		BOOL RecreateParams();

		void MarkInputsAfterOutput(
			LPPARAM outputPtr);

		void MaybeMarkInput(
			SERUID outputUID);

		void ParamNotFoundMessage(
			LPPARAM paramPtr,
			BOOL upsfing);

		void RequiredTypeNotFoundMessage(
			BOOL upsfing);
      
	public:

		FUNCINST(
			SERUID theSubjectUID,
			ATYPEID theType,
			POINT newLocation);

		FUNCINST(
			void * thisPtr);

		~FUNCINST();

		void Draw(
			HDC hdc);

		void DrawDrag(
			HDC hdc,
			POINT offset);

		void GetRect(
			LPRECT bounds)
			{
			*bounds = boundsRect;
			}

		virtual void GetTotalBounds(
			LPRECT bounds);
		
		void GetUnionIncomingFlows(
			LPRECT bounds);

		void GetSelectedRect(
			LPRECT bounds);

		void Move(
			POINT offset);

		int PointInFlowArea(
			POINT pt);

		void DrawHighlight(
			HDC hdc,
			int where);

		POINT PointToArrowTip(
			POINT mouse);

		POINT GetArrowTip(
			int whichTip);

		void IncFlowCount(
			int which);

		void DecFlowCount(
			int which);

		PARAM * ParamFromPoint(
			POINT pt);

		void CalculateBounds();

		BOOL PtInSelectionArea(
			POINT mouse);

		BOOL UpdateParamNames(
			SERUID paramUID,
			BOOL isParam,
			LPRECT refreshRect);

		void AddSelectedParamsToList(
			LIST * listPtr);

		BOOL AddParamsInRectToList(
			HDC hdc,
			LPRECT rect,
			LIST * list);

		BOOL ObjectIntersectsRect(
			LPRECT bigRect);


		RUNTIMEUID GetInputParams()
			{
			return inputParams;
			}

		RUNTIMEUID GetOutputParams()
			{
			return outputParams;
			}

		void Compile();

		LPAFUNCDESC GetDescPtr()
			{
			return infoPtr;
			}

		short PostReadInit();

		void UpdateUIDs();

		void ClearInputs(
			RUNTIMEUID clipParamBlock,
			BOOL       clearAll);

		void ClearOutputs(
			RUNTIMEUID clipParamBlock,
			BOOL       clearAll);

		void CopyInputs(
			RUNTIMEUID clipParamBlock,
			BOOL       copyAll);

		void CopyOutputs(
			RUNTIMEUID clipParamBlock,
			BOOL       copyAll);

		PARAM * GetParam(
			int index);

		ATYPEID GetTypeID()
			{
			return typeID;
			}

		void AddLinkedInputsToList(
			HDC hdc,
			LIST * list,
			SERUID outputUID);

		void SetPrintVisitedFlag()
			{
			status|=FN_PRINTVISITED;
			}

		BOOL TestPrintVisitedFlag()
			{
			return (status & FN_PRINTVISITED)?TRUE:FALSE;
			}

		void ClearPrintVisitedFlag()
			{
			status&= ~FN_PRINTVISITED;
			}

		void SetPrintLabelFlag()
			{
			status|=FN_PRINTLABEL;
			}

		BOOL TestPrintLabelFlag()
			{
			return (status & FN_PRINTLABEL)?TRUE:FALSE;
			}

		void ClearPrintLabelFlag()
			{
			status&= ~FN_PRINTLABEL;
			}

		void SetLabelVisitedFlag()
			{
			status|=FN_LABELVISITED;
			}

		BOOL TestLabelVisitedFlag()
			{
			return (status & FN_LABELVISITED)?TRUE:FALSE;
			}

		void ClearLabelVisitedFlag()
			{
			status&= ~FN_LABELVISITED;
			}

		void DoPrint(
			LPPRINTER printPtr,
			PRINTFLOWSTACK * stackPtr,
			int indent);

		void CleanPrint();

		void BuildLabels(
			PRINTFLOWSTACK * stackPtr);

		static void PrintBracket(
			LPPRINTER printPtrj,
			int indent);

		short CheckFunction(
			short checkType);

		void SetLocation(POINT p);

		void UnselectParams();

		WORD ExportUPSF();

		BOOL ImportUPSF();

		BOOL AddObjectToZlist(
			LPRECT area,
			LPZLIST zlistPtr);

		LPSTR pGetHelpFileName(
			void)
			{
			return infoPtr->pGetHelpFileName();
			}

		void MarkAllParamsBad();

		short ValidateParams();

		static BOOL SetSysColors();

		BOOL HasSpaces(
			LPSTR theString);

		void GetMaxScreenSize(
			LPRECT bounds);

		//use the labelVisited mask since it's not in use at this time.
		void SetChainVisitedFlag()
			{
			SetLabelVisitedFlag();
			}

		void ClearChainVisitedFlag()
			{
			ClearLabelVisitedFlag();
			}

		BOOL TestChainVisitedFlag()
			{
			return TestLabelVisitedFlag();
			}

		void PushAndSelectFuncChain(
			HDC hdc,
			LPLIST selectedFuncs,
			LPLIST selectedFlows,
			LPFLOWSTACK flowStack);

		void SelectFuncChain(
			HDC hdc,
			LIST * selectedFuncs,
			LIST * selectedFlows);

		BOOL MoveIfSelected(
			POINT offset,
			LPFLOWSTACK flowStack);

		LPPARAM GetNextParam(
			int * indexPtr);

		BOOL PostUPSFImport();

		void GetHighlightBox(
			LPRECT box,
			int where);

		void GetUnionBoundsAndBox(
			LPRECT bounds,
			int where);

		BOOL SupportsOutFlows()
			{
			return (status & FN_FLOWOUT)?TRUE:FALSE;
			}

		BOOL HasOutputParams()
			{
			return (status & FN_OUTPUTS);
			}

		void MarkUninitParams();

		BOOL HasIncomingFlows()
			{
			return (inFlowFlag > 0);
			}

    void SetTypeNeedsAddingFlag()
    	{
      status |= FN_TYPENEEDSADDING;
      }
      
    void ClearTypeNeedsAddingFlag()
    	{
      status &= ~FN_TYPENEEDSADDING;
      }
      
    BOOL TestTypeNeedsAddingFlag()
    	{
      return (status & FN_TYPENEEDSADDING)?TRUE:FALSE;
      }

		void MarkRequiredTypes(
			BOOL upsfing);
    
		void ClearRequiredTypes();
    
	};

#endif
