#ifndef L_VI_H
#define L_VI_H


extern "C" LRESULT  pascal  NewListboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);
extern "C" LRESULT  pascal  NewComboboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);
extern "C" LRESULT  pascal  NewComboboxEditProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);
extern "C" LRESULT  pascal  NewComboboxListProc (HWND hWnd, UINT message, UINT wParam, LONG lParam);

BOOL ALMAPI ListViAppearanceChanged(OpWndItemD* wi, long hwnd, long);
BOOL ALMAPI ListViEditControlChanged(OpWndItemD* wi, long hwnd, long);

OpWndItemD* ALMAPI CreateListVI(AOBJECTID oiList, AOBJECTID oiParent, RECT* rcItem, long status, long special);


///////////

class LIST_VI : public OpHwndWndItem
	{
	protected:
		ATTRIBUTES								m_attributes;
		POINT											m_ptLastClick;
		DWORD											m_lTimeLastClick;

	public:
		LIST_VI(AOBJECTID oiList, AOBJECTID oiParent, RECT* rcItem, long status, long special);
		void ALMAPI ParentClosing();
		void ALMAPI DataChanged(long details, long modifiers, long variation);
		void ALMAPI Destroy();
		BOOL ALMAPI MeasureItem(MEASUREITEMSTRUCT* measureItem);
		BOOL ALMAPI DrawItem(DRAWITEMSTRUCT* drawItem);
		long ALMAPI AdjustNewRect(RECT* newRect, LONG adjustFlag);

		void AppearanceChanged(HWND hwnd);
		void EditControlChanged(HWND hwnd);

		virtual LONG SetSelections()=0;
		virtual void ReturnKeyPressed();

		void SendClickSignal(POINT pt);

	};
typedef LIST_VI  * LPLIST_VI;

////////////////////////////

class LIST_VI_LISTBOX : public LIST_VI
	{
	public:
		LIST_VI_LISTBOX(AOBJECTID oiList, AOBJECTID oiParent, RECT* rcItem, long status, long special);
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);
		void ALMAPI ParentOpened();

		LRESULT SelectionsMaybeChanged();
		BOOL GetSelections();	 //returns TRUE if the selections changed
		LONG SetSelections();
	};
typedef LIST_VI_LISTBOX  * LPLIST_VI_LISTBOX;

/////////////////////////

class LIST_VI_COMBOBOX : public LIST_VI
	{
	public:
		LIST_VI_COMBOBOX(AOBJECTID oiList, AOBJECTID oiParent, RECT* rcItem, long status, long special);
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, UINT wParam, LPARAM lParam);
		void ALMAPI ParentOpened();

		LRESULT SelectionsMaybeChanged();
		BOOL GetSelections();	 //returns TRUE if the selections changed
		LONG SetSelections();
		void SetEditControlText();

		void ReturnKeyPressed();
		void SubclassComboboxPieces(HWND hwndChild);

	};
typedef LIST_VI_COMBOBOX  * LPLIST_VI_COMBOBOX;

#endif
