
extern "C" LRESULT CALLBACK AppwareNumberObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



class HwndNumberItem:public OpHwndWndItem
	{
	private:
		ATTRIBUTES						m_attributes;
		int										m_iSelectionStart;	//Current edit selection start
		int										m_iSelectionEnd;		//Current edit selection end

	public:
		HwndNumberItem(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI ParentOpened();
		void ALMAPI ParentClosing();
		BOOL ALMAPI GetSelection(LPINT start, LPINT count);
		BOOL ALMAPI SetSelection(int start, int count);
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
		LiteNumberItem(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);
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
BOOL ALMAPI HwndNumberItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag);
BOOL ALMAPI LiteNumberItemChangeFont(OpWndItemD* lwi, long attrPtr, long changeFlag);
BOOL ALMAPI HwndNumberItemForceRedraw(OpWndItemD* hwi, long notused, long notusedEither);
BOOL ALMAPI LiteNumberItemForceRedraw(OpWndItemD* lwi, long notused, long notusedEither);

 




