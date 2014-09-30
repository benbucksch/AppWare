#ifndef __PARAM_H
#define __PARAM_H

#include "tstring.h"
#include "compuid.h"
#include "parmmask.h"
#include "cfglibp.h"
#include "serrec.h"
#include "objdesc.h"

#define MAX_PARAM_NAME 256
#define PAR_BORDER 2

class PARAM:public SERIUSRECORD, public PARAMMASK
	{
	private:

		static int 		paramCount;
		static HFONT 	requiredFont;
		static HFONT 	optionalFont;
		static HFONT  boldRequired;
		static HFONT  boldOptional;
		static char   dlgBuffer[MAX_PARAM_NAME];
		static HBITMAP grayInArrow;
		static HBITMAP blackInArrow;
		static HBITMAP arrowMask;

		RECT					location;
		LPPARAMDESC   paramDesc;
		int						ordinal;
		int						connectCount;

		static BOOL Initialize();

		static void Uninitialize();

		void AdjustWidth();

		void SetFont(
			HDC hdc);

		void DoErase(
			HDC hdc);

		void DetermineTextValidity();

		static BOOL ValidInputOutputConnection(
			LPPARAM input,
			LPPARAM output,
			BOOL ignoreConnectStatus);

	public:
		SERUID ownerFunc;

		PARAM(
			LPPARAMDESC descPtr,
			int   info,
			SERUID theOwnerFunc,
			int count,
			int position,
			BOOL figureBounds = TRUE);

		PARAM(
			void * thisPtr);

		~PARAM();

		TSTRING MakeName(
			HDC hdc,
			BOOL truncate);

		int GetOrdinal()
			{
			return ordinal;
			}

		LPPARAMDESC GetParamDesc()
			{
			return paramDesc;
			}

		void ClearInfo();

		void HardClearInfo();

		void Draw(
			HDC hdc,
			BOOL erase);

		BOOL IsInput()
			{
			return (status & PAR_INPUT)?TRUE:FALSE;
			}

		BOOL IsSelected()
			{
			return (status & PAR_SELECTED)?TRUE:FALSE;
			}

		void SetSelected(
			BOOL set);

		BOOL PointInParam(
			POINT pt)
			{
			return PtInRect((LPRECT)&location, pt);
			}

		void AddInfo(
			long info);

		void AddLinkUID(
			SERUID dataUID);

		void GetRect(
			LPRECT rect)
			{
			*rect = location;
			}

		void CalculateRect(
			POINT refPt,
			int count);

		void ReCalculateRect(
			POINT offset)
			{
			OffsetRect((LPRECT)&location, offset.x, offset.y);
			}

		void DrawHighlight(
			HDC hdc);

		void SetName(
			LPCSTR newName)
			{
			name = newName;
			AdjustWidth();
			}

		BOOL ChangeName(
			HINSTANCE hInst,
			HWND ownerWindow);

		BOOL EnterConstant(
			HINSTANCE hInst,
			HWND ownerWindow);

		static BOOL CALLBACK GetParamName(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static BOOL CALLBACK GetConstant(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		BOOL TestCanBeConstantFlag()
			{
			return((!(TestObjConnectFlag() || TestParamConnectFlag())) &&
				(status & PAR_CANBECONSTANT))?TRUE:FALSE;
			}

		void Erase(
			HDC hdc);

		BOOL ParamInRect(
			LPRECT rect)
			{
			RECT temp;

			return IntersectRect((LPRECT)&temp, (LPRECT)&location, rect);
			}

		void UpdateUIDs();

		short UpdateParamDesc(
                        LPAFUNCDESC funcDesc);

		void SetStatus(
			long newStatus)
			{
			status = newStatus;
			}

		int GetConnectCount()
			{
			return connectCount;
			}

		BOOL TestTextFlag()
			{
			return (status & PAR_TEXTLEGAL)?TRUE:FALSE;
			}

		void DoPrint(
			LPPRINTER printPtr,
			int indent);

		LPCSTR GetName()
			{
			return name;
			}

		void UpdateLocation(
			int position,
			LPFUNCINST funcPtr);

		WORD ExportUPSF();

		BOOL TestBadParamFlag()
			{
			return (status & PAR_BADPARAM)?TRUE:FALSE;
			}

		void SetBadParamFlag()
			{
			connectCount = 0;
			ClearInfo();
			status |= PAR_BADPARAM;
			}

		void ClearBadParamFlag()
			{
			status &= ~PAR_BADPARAM;
			}

		static BOOL SetSysColors();

		BOOL TestTempObjFlag()
			{
			return (status & PAR_TEMPOBJ)?TRUE:FALSE;
			}

		SERUID PopUpParamList(
			HWND subjectWindow,
			RUNTIMEUID objectBlock);

			PARAM & PARAM::operator =(
				const PARAM &		srcParam);

			BOOL Equivalent(
				const LPPARAM paramPtr);

		BOOL ConnectionLegal(
			LPOBJINST objPtr,
			LPPARAM paramPtr,
			BOOL ignoreConnectStatus);
		
		//overload the PM_SLOTFILLED flag since it is only used at runtime
		BOOL TestInitializedInputFlag()
			{
			return (status & PM_SLOTFILLED);
			}

		void SetInitializedInputFlag()
			{
			status |= PM_SLOTFILLED;
			}

		void ClearInitializedInputFlag()
			{
			status &= ~PM_SLOTFILLED;
			}
      
    void SetTypeNeedsAddingFlag()
    	{
      status |= PAR_TYPENEEDSADDING;
      }
      
    void ClearTypeNeedsAddingFlag()
    	{
      status &= ~PAR_TYPENEEDSADDING;
      }
      
    BOOL TestTypeNeedsAddingFlag()
    	{
      return (status & PAR_TYPENEEDSADDING)?TRUE:FALSE;
      }

		};



#endif
