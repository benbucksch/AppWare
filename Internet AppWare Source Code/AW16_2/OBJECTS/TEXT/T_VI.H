
extern "C" LRESULT far pascal _export AppwareTextObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


class HwndTextItem:public OpHwndWndItem
	{
	private:
		ATTRIBUTES						m_attributes;
		HGLOBAL								m_hEditDS;
		LONG      						m_selection;	//Current edit selection (block) and cursor position;
		LONG									m_whenChangeLastProcessed;

	public:
		HwndTextItem(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);

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

		HINSTANCE GetHInstanceToUse();
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
		MEMBLOCKID						m_miLastDrawnString;

	public:
		LiteTextItemBase(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);
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
		LiteTextItemTransparent(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);
		void ALMAPI Draw(HDC hdc, LPRECT clipRect);
	};

class LiteTextItemFramed:public LiteTextItemBase
	{
	public:
		LiteTextItemFramed(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);
		void ALMAPI Draw(HDC hdc, LPRECT clipRect);
	};

//////////////////////////////////////////////////////////////////
// Prototypes for functions called by OWndCallEachVisualInstance
/////////////////////////////////////////////////////////////////
BOOL far pascal HwndTextItemChangeFont(OpWndItemD* hwi, long attrPtr, long changeFlag);
BOOL far pascal LiteTextItemChangeFont(OpWndItemD* lwi, long attrPtr, long changeFlag);


