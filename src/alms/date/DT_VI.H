/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:  dt_vi.h
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION: Date object visual instance class definition and method
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

extern "C" LRESULT CALLBACK AppwareDateObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

class HwndDateItem:public OpHwndWndItem
{
	private:
		ATTRIBUTES m_attributes;
		LONG       m_selection;	//Current edit selection (block) and cursor position;

	public:
		HwndDateItem(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI ParentOpened();
		void ALMAPI ParentClosing();
		BOOL ALMAPI GetSelection(int *start, int *count);
		BOOL ALMAPI SetSelection(int start, int count);
		long ALMAPI Activate(int causedBy, BOOL mustKeep);
		void ALMAPI Deactivated();
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void ALMAPI Cut();
    void ALMAPI Copy();
    void ALMAPI Paste();
    void ALMAPI Undo();

    BOOL UpdateIfModified();

		void ChangeFont(LPATTRIBUTES newAttr, long changeFlag);
    void ForceRedraw();

  	void ALMAPI Print(HDC hdc, RECT* printRect, int pclPrinter);

		LRESULT WmChar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmEraseBkgnd(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    MaybePostChanged();
};

class LiteDateItem:public OpLiteWndItem
{
	private:
		ATTRIBUTES	m_attributes;
		RECT				m_rcLastDrawn; 			//Is invalidated w/TRUE--the whole rect is invalidated w/FALSE
		AMEMBLOCKID	m_miLastDrawnString;

	public:
		LiteDateItem(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);
		~LiteDateItem();

		void ALMAPI ParentOpened();
		void ALMAPI ParentClosing();
		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI Draw(HDC hdc, LPRECT clipRect);

		void ChangeFont(LPATTRIBUTES newAttr, long changeFlag);
    void ForceRedraw();

  	void ALMAPI Print(HDC hdc, RECT* printRect, int pclPrinter);

};


 
//////////////////////////////////////////////////////////////////
// Prototypes for functions called by OWndCallEachVisualInstance
/////////////////////////////////////////////////////////////////
BOOL CALLBACK HwndDateItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag);
BOOL CALLBACK LiteDateItemChangeFont(OpWndItemD* lwi, long attrPtr, long changeFlag);
BOOL CALLBACK HwndDateItemForceRedraw(OpWndItemD* hwi, long notused, long notusedeither); 
BOOL CALLBACK LiteDateItemForceRedraw(OpWndItemD* lwi, long notused, long notusedeither); 





