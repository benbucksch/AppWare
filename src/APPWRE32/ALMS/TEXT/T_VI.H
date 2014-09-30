
extern "C" LRESULT  pascal  AppwareTextObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


class HwndTextItem:public OpHwndWndItem
	{
	private:
		ATTRIBUTES						m_attributes;
		int										m_iSelectionStart;	//Current edit selection start
		int										m_iSelectionEnd;		//Current edit selection end
		LONG									m_whenChangeLastProcessed;

	public:
		HwndTextItem(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI ParentOpened();
		void ALMAPI ParentClosing();
		BOOL ALMAPI GetSelection(LPINT start, LPINT end);
		BOOL ALMAPI SetSelection(int start, int end);
		long ALMAPI Activate(int causedBy, BOOL mustKeep);
		void ALMAPI Deactivated();
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  	void ALMAPI Print(HDC hdc, RECT* printRect, int pclPrinter);

		void ALMAPI Cut();
		void ALMAPI Copy();
		void ALMAPI Paste();
		void ALMAPI Undo();

		BOOL UpdateIfModified();

		void SetTabStops();
		void ReleaseTabStops();
		void SetEditLimit();

		void ChangeFont(LPATTRIBUTES newAttr, long changeFlag);

		LRESULT WmSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmEraseBkgnd(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WmChar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	};

class LiteTextItemBase:public OpLiteWndItem
	{
	protected:
		ATTRIBUTES						m_attributes;
		RECT									m_rcLastDrawn; 			//Is invalidated w/TRUE--the whole rect is invalidated w/FALSE
		AMEMBLOCKID						m_miLastDrawnString;

	public:
		LiteTextItemBase(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);
		~LiteTextItemBase();

		void ALMAPI Destroy();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
  	void ALMAPI Print(HDC hdc, RECT* printRect, int pclPrinter);

		void DrawTheText(HDC hdc, LPRECT clipRect, BOOL bIndent);
		void ChangeFont(LPATTRIBUTES newAttr, long changeFlag);
	};

class LiteTextItemTransparent:public LiteTextItemBase
	{
	public:
		LiteTextItemTransparent(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);
		void ALMAPI Draw(HDC hdc, LPRECT clipRect);
	};

class LiteTextItemFramed:public LiteTextItemBase
	{
	public:
		LiteTextItemFramed(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);
		void ALMAPI Draw(HDC hdc, LPRECT clipRect);
	};

//////////////////////////////////////////////////////////////////
// Prototypes for functions called by OWndCallEachVisualInstance
/////////////////////////////////////////////////////////////////
BOOL  pascal HwndTextItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag);
BOOL  pascal LiteTextItemChangeFont(OpWndItemD* lwi, long attrPtr, long changeFlag);


