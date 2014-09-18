
extern "C" LRESULT far pascal _export AppwareNumberObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



class HwndNumberItem:public OpHwndWndItem
	{
	private:
		ATTRIBUTES						m_attributes;
		LONG      						m_selection;	//Current edit selection (block) and cursor position;


	public:
		HwndNumberItem(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI ParentOpened();
		void ALMAPI ParentClosing();
		BOOL ALMAPI GetSelection(LPLONG start, LPLONG count);
		BOOL ALMAPI SetSelection(long start, long count);
		long ALMAPI Activate(int causedBy, BOOL mustKeep);
		void ALMAPI Deactivated();
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  	void ALMAPI Print(HDC hdc, RECT* printRect, int pclPrinter);

		void ALMAPI Cut();
		void ALMAPI Copy();
		void ALMAPI Paste();
		void ALMAPI Undo();

		BOOL UpdateIfModified();

		void ChangeFont(LPATTRIBUTES newAttr, long changeFlag);
		void ForceRedraw();

		LRESULT WmChar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmEraseBkgnd(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

class LiteNumberItem:public OpLiteWndItem
	{
	private:
		ATTRIBUTES						m_attributes;
		RECT									m_rcLastDrawn; 			//Is invalidated w/TRUE--the whole rect is invalidated w/FALSE
		char									m_szLastDrawn[I_STRING_LEN];

	public:
		LiteNumberItem(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);
		~LiteNumberItem();

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
BOOL far pascal HwndNumberItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag);
BOOL far pascal LiteNumberItemChangeFont(OpWndItemD* lwi, long attrPtr, long changeFlag);
BOOL far pascal HwndNumberItemForceRedraw(OpWndItemD* hwi, long notused, long notusedEither);
BOOL far pascal LiteNumberItemForceRedraw(OpWndItemD* lwi, long notused, long notusedEither);

 




