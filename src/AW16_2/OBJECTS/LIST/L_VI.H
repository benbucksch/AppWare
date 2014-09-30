#ifndef L_VI_H
#define L_VI_H


LONG far pascal _export NewListboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);
LONG far pascal _export NewComboboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);
LONG far pascal _export NewComboboxEditProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);
LONG far pascal _export NewComboboxListProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);

BOOL ALMAPI ListViAppearanceChanged(OpWndItemD* wi, long hwnd, long);
BOOL ALMAPI ListViEditControlChanged(OpWndItemD* wi, long hwnd, long);

OpWndItemD* ALMAPI CreateListVI(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special);


///////////

class LIST_VI : public OpHwndWndItem
	{
	protected:
		ATTRIBUTES								m_attributes;
		POINT											m_ptLastClick;
		DWORD											m_lTimeLastClick;

	public:
		LIST_VI(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special);
		void ALMAPI ParentClosing();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI Destroy();
		BOOL ALMAPI MeasureItem(MEASUREITEMSTRUCT* measureItem);
		BOOL ALMAPI DrawItem(DRAWITEMSTRUCT* drawItem);
		long ALMAPI AdjustNewRect(RECT* newRect, LONG adjustFlag);

		void AppearanceChanged(HWND hwnd);
		void EditControlChanged(HWND hwnd);

		virtual LONG SetSelections()=0L;
		virtual void ReturnKeyPressed();

		void SendClickSignal(POINT pt);

	};
typedef LIST_VI far * LPLIST_VI;

////////////////////////////

class LIST_VI_LISTBOX : public LIST_VI
	{
	public:
		LIST_VI_LISTBOX(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special);
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);
		void ALMAPI ParentOpened();

		LRESULT SelectionsMaybeChanged();
		BOOL GetSelections();	 //returns TRUE if the selections changed
		LONG SetSelections();
	};
typedef LIST_VI_LISTBOX far * LPLIST_VI_LISTBOX;

/////////////////////////

class LIST_VI_COMBOBOX : public LIST_VI
	{
	public:
		LIST_VI_COMBOBOX(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special);
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);
		void ALMAPI ParentOpened();

		LRESULT SelectionsMaybeChanged();
		BOOL GetSelections();	 //returns TRUE if the selections changed
		LONG SetSelections();
		void SetEditControlText();

		void ReturnKeyPressed();
		void SubclassComboboxPieces(HWND hwndChild);

	};
typedef LIST_VI_COMBOBOX far * LPLIST_VI_COMBOBOX;

#endif
