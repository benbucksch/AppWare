
#if(!defined(__prjwin_h))
#define __prjwin_h

#include "basewin.h"

////////////////////////////////////////////////////////////////////////////

class PROJECTWINDOW:public BASEWINDOW
	{
	private:
		static int 		minProjectHeight;
		static int 		minProjectWidth;
		static BOOL 	registered;
		static char 	className[14];
		int 					projectSubjectWidth;
		BOOL					draggingSubject;
		POINT					oldMouse;
		BOOL 					autoScroll;
		RECT					selectRect;
		BOOL					draggingSelectRect;
		BOOL					vScrollShowing;
		BOOL       		hScrollShowing;
		TSTRING				oldName;

		void PaintWindow();

		void GrowOrShrinkLogBounds();

		void ResizeScrollBars();

		void Register();

		void CreateScrollBars();

		void ScrollProjectWindow(
			int direction);

		void	FigureSelection(
			LPARAM lParam,
			WPARAM	wParam,
			BOOL doubleClick);

		BOOL DragSubject(
			LPARAM 	lParam);

		BOOL EndDrag(
			LPARAM lParam);

		void MoveAllSubjects(
			HDC			hdc,
			LPPOINT offset,
			LPRECT	unionRect);

		void AdjustSelectedSubjectPosition(
			HDC	hdc,
			LPPOINT	offset);

		void AutoScroll(
			int	direction);

		POINT VerifyOffset(
			POINT offset);

		void ResetOldMouse();

		void ArbitraryHScroll(
			WPARAM wParam,
			LPARAM lParam);

		void ArbitraryVScroll(
			WPARAM wParam,
			LPARAM lParam);

		void DrawSelectRect(
			HDC hdc,
			BOOL selectTheSubjects);

		void AutoScrollSelectRect(
			int direction);

		void SelectSubjectsInRect(
			HDC hdc);

		void SnapSubjectsToGrid(
			BOOL snapAll);

		void GreyScrollBar(
			HWND sBar);


	public:
		WNDPROC				oldEditProc;
		HWND					editField;
		SERUID				holdUID;

		PROJECTWINDOW(
			SERUID theProjectUID);

		~PROJECTWINDOW();

		LRESULT WndProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void SetClippingRegion(
			HDC hdc);

		void DrawAllSubjects(
			HDC hdc,
			LPRECT refreshRect);

		static LRESULT CALLBACK EditFieldProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void CloseEditField();

		void ReshapeEditField();

		void SetAllScrollBars();

		void CenterSelection();

		void SelectAllSubjects();

		void DetermineScrollBars();

		void SetScrollBars(
			int command);

		BOOL Busy();

	};


///////////////////////////////////////////////////////////////////////////

#endif


