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
// FILE:  tm_vi.h
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION: Time object visual instance class definition and method
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

extern "C" LRESULT far pascal _export AppwareTimeObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

class HwndTimeItem:public OpHwndWndItem
{
	private:
		ATTRIBUTES m_attributes;
		LONG       m_selection;	//Current edit selection (block) and cursor position;

	public:
		HwndTimeItem(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI ParentOpened();
		void ALMAPI ParentClosing();
		BOOL ALMAPI GetSelection(LPLONG start, LPLONG count);
		BOOL ALMAPI SetSelection(long start, long count);
		long ALMAPI Activate(int causedBy, BOOL mustKeep);
		void ALMAPI Deactivated();
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void ALMAPI Cut();
    void ALMAPI Copy();
    void ALMAPI Paste();
    void ALMAPI Undo();
  	void ALMAPI Print(HDC hdc, RECT* printRect, int pclPrinter);

    BOOL UpdateIfModified();

		void ChangeFont(LPATTRIBUTES newAttr, long changeFlag);
    void ForceRedraw();

		LRESULT WmChar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmEraseBkgnd(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void MaybePostChanged();
};

class LiteTimeItem:public OpLiteWndItem
{
	private:
		ATTRIBUTES	m_attributes;
		RECT				m_rcLastDrawn; 			//Is invalidated w/TRUE--the whole rect is invalidated w/FALSE
		MEMBLOCKID	m_miLastDrawnString;

	public:
		LiteTimeItem(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);
		~LiteTimeItem();

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI Draw(HDC hdc, LPRECT clipRect);
  	void ALMAPI Print(HDC hdc, RECT* printRect, int pclPrinter);


		void ChangeFont(LPATTRIBUTES newAttr, long changeFlag);
    void ForceRedraw();

};


 
//////////////////////////////////////////////////////////////////
// Prototypes for functions called by OWndCallEachVisualInstance
/////////////////////////////////////////////////////////////////
BOOL far pascal HwndTimeItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag);
BOOL far pascal LiteTimeItemChangeFont(OpWndItemD* lwi, long attrPtr, long changeFlag);
BOOL far pascal HwndTimeItemForceRedraw(OpWndItemD* hwi, long notused, long notusedeither); 
BOOL far pascal LiteTimeItemForceRedraw(OpWndItemD* lwi, long notused, long notusedeither); 
 




