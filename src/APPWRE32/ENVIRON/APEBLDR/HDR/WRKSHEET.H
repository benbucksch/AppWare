#ifndef __wrksheet_h
#define __wrksheet_h

#include "navitem.h"
#include "serrec.h"

#define SUBICONWIDTH	32
#define SUBICONHEIGHT	32
#define SUBICONTEXT   18
#define SHADOW				2
#define SUBICONBORDER 8
#define SUBTEXTBORDER 1
#define XMAXPAGE 0x4000
#define YMAXPAGE 0x4000
#define SUBJECTSEPARATION 30
#define SUBJECTTOPSPACING 20

//need to add the SHADOW to the width so that the shadow part of the screen
//will be refreshed when the icon is drawn.
#define SUBJECTWIDTH 	(SUBICONWIDTH + 2*SUBICONBORDER + SHADOW)
#define SUBJECTHEIGHT (SUBICONHEIGHT + 2*SUBICONBORDER + SUBICONTEXT)
#define XSCROLLSTEP		((int)(SUBJECTWIDTH/3))
#define YSCROLLSTEP		((int)(SUBJECTHEIGHT/4))
#define	ID_VSCROLLBAR	200
#define ID_HSCROLLBAR 202
#define PROJECTGRID   16


//WRK masks are shared by both subjects and projects

#define WRK_ICONIC  					0x00000002
#define WRK_MAXIMIZE					0x00000004
#define WRK_GRIDON      			0x00000008

#define PRJ_STOPSDISABLED 		0x00000100
#define PRJ_CANCELCOMPILE 		0x00000200
#define PRJ_IMPORTING					0x00000400

#define SJT_SIGNALNAMES 			0x00000100
#define SJT_SELECTED    			0x00000200
#define SJT_BIGICON     			0x00000400

//used internally by the project's arraange subjects routine
#define SJT_SORTED						0x00004000

///////////////////////////////////////////////////////////////////////////

class WORKSHEET:public NAVIGATORITEM,  public SERIUSRECORD
	{
	private:
		static  unsigned				worksheetCount;
		RECT    								bounds;
		COLORREF								windowColor;
		unsigned int						order;

		void Initialize();

		void UnInitialize();

		void InitConstructor();


	protected:
		static	HICON						subjectIcon;
		static  HICON						projectIcon;
		static  HPEN						shadowPen;
		static 	WINDOWLIST *  indexList;
		long   	status;
		BASEWINDOW *       	windowPtr;

		void ActivateTheWindow();

	public:

		static	HFONT						subFont;
		static  HFONT						numSubFont;
		static	HBITMAP					leftScrollArrow;
		static  HBITMAP					rightScrollArrow;
		static  HBITMAP					upScrollArrow;
		static  HBITMAP					downScrollArrow;

		WORKSHEET();

		WORKSHEET(
			void * thisPtr);

		~WORKSHEET();

        friend AOBJECTID ALMAPI AObjGetNextObject(
  		ATYPEID typeID,
  		LPLONG theGetKey);

		virtual BOOL ShowObjectWindow() = 0;

		void SetWindowColor(
			COLORREF theColor);

		COLORREF GetWindowColor();

		void SetWindowBounds(
			LPRECT theRect);

		void GetWindowBounds(
			LPRECT winBounds);

		virtual void SetName(
			LPCSTR newName)=0;

		virtual LPCSTR GetName()=0;

		void SetWindowPtr(
			BASEWINDOW * theWindow)
			{
			windowPtr = theWindow;
			}

		BASEWINDOW * GetWindowPtr()
			{
			return windowPtr;
			}

		unsigned int GetOrder();

		unsigned int SetOrder(
			unsigned int newOrder);

		void IncrementOrder();

		void ShowItem(void);

		long GetStatus()
			{
			return status;
			}

		static BOOL WorksheetExists()
			{
			return (worksheetCount > 0)?TRUE:FALSE;
			}

		int GetMaxOrder()
			{
			return worksheetCount - 1;
			}

		static HICON GetSubjectIcon()
			{
			return WORKSHEET::subjectIcon;
			}

		static HICON GetProjectIcon()
			{
			return WORKSHEET::projectIcon;
			}

		void SetIconicFlag()
			{
			status|=WRK_ICONIC;
			}

		void ClearIconicFlag()
			{
			status&= ~WRK_ICONIC;
			}

		BOOL TestIconicFlag()
			{
			return (status & WRK_ICONIC)?TRUE:FALSE;
			}

		void SetMaximizeFlag()
			{
			status|=WRK_MAXIMIZE;
			}

		void ClearMaximizeFlag()
			{
			status&= ~WRK_MAXIMIZE;
			}

		BOOL TestMaximizeFlag()
			{
			return (status & WRK_MAXIMIZE)?TRUE:FALSE;
			}
	};


#endif


