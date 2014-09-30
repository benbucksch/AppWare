#if(!defined(__basewin_h))
#define __basewin_h

#include "classdef.h"
#include "memclass.h"
#include "sereclib.h"

#define GOUP 		0x1
#define GODOWN 	0x2
#define GOLEFT	0x4
#define GORIGHT	0x8

#define VRANGE 	0x1
#define VPOS		0x2
#define HRANGE	0x4
#define HPOS		0x8

#define BSW_SUBJECT 0x00000001
#define BSW_PROJECT 0x00000002


///////////////////////////////////////////////////////////////////////////

class BASEWINDOW:public MEMORY
	{
	private:
		static int 							windowCount;
		SERUID                  projectUID;

		void Initialize();

		void DeInitialize();

	protected:
		static HFONT						numberSubFont;
		static HRGN							clipRegion;
		HWND 			hScroll;
		HWND 			vScroll;
		HWND 			hWnd;
		long			status;

		void SetSubjectFlag()
			{
			status &= ~BSW_PROJECT;
			status |= BSW_SUBJECT;
			}

		void SetProjectFlag()
			{
			status &= ~BSW_SUBJECT;
			status |= BSW_PROJECT;
			}

	static LPARAM CALLBACK BASEWINDOW::BaseWndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);

	public:
		LPFILEOBJECT 						fileObjPtr;
		static WINDOWLIST *	list;

		BASEWINDOW(
			SERUID theProjectUID);

		virtual ~BASEWINDOW();

		virtual long WndProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam)=0;

		HWND GetWindowHandle();

		PROJECT * GetProjectPtr()
			{
			return (PROJECT *)GetObject(projectUID);
			}

		void SetWindowHandle(
			HWND newHandle);

		BOOL Show(
			int ShowCmd);

		BOOL Hide(
			void);

		void Update(
			void);

		BOOL TestSubjectFlag()
			{
			return (status & BSW_SUBJECT)?TRUE:FALSE;
			}

		BOOL TestProjectFlag()
			{
			return (status & BSW_PROJECT)?TRUE:FALSE;
			}

		static BASEWINDOW * BASEWINDOW::GetMouseWindow(
			POINT pt);
	
		virtual BOOL Busy() = 0;
	};

#endif


