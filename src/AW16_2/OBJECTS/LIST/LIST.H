#ifndef _LSTHEADER_
#define _LSTHEADER_
#define OTYPE_LIST   MAKETYPEID('L','i','s','t')

extern HANDLE hInstance;
extern FARPROC OldListboxProc ;
extern FARPROC OldComboboxProc ;
extern int listPointerOffset ;
extern int listObjectOffset ;
extern int comboPointerOffset ;
extern int comboObjectOffset ;

#define DBLCLICKSIGNAL 0
#define SINGLECLICKSIGNAL 1
#define CHANGEDSIGNAL 4
#define ENTERKEYSIGNAL 2
#define RETURNKEYSIGNAL 3
#define ACTIVATEDSIGNAL 10
#define DEACTIVATEDSIGNAL 11
#define SELCHANGEDSIGNAL 5
#define EDITEDSIGNAL 6

BOOL GetNextLine(LPSTR str, UINT& start, UINT& end) ;
void PostObjectChanged(OBJECTID theObject, LONG modifiers = 0L) ;
LRESULT far pascal _export Lst_SetupDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define THE_VERSION 2
#define  EXTERNC  extern "C" void far pascal _export
////////////////////////////////
#define LST_APPERANCE_CHANGED		0x00000001L  //Font, selection, etc....
#define LST_ITEMS_CHANGED				0x00000002L
#define LST_EDITCONTROL_CHANGED	0x00000004L

//#define LST_OBJECT_CHANGED_HOW	(AEVT_KILLDUPLICATES|AEVT_KILLMODIFIERS)

#define LF_MULTISELECT			0x00000002L
#define LF_LISTBOX					0x00000004L
#define LF_SIMPLECOMBO 			0x00000008L
#define LF_DROPDOWNCOMBO		0x00000010L
#define LF_DROPDOWNLIST			0x00000020L

#define LF_LIST_STYLES (LF_LISTBOX|LF_SIMPLECOMBO|LF_DROPDOWNCOMBO|LF_DROPDOWNLIST)
#define LF_COMBOS (LF_SIMPLECOMBO|LF_DROPDOWNCOMBO|LF_DROPDOWNLIST)
#define LF_EDITABLE_COMBOS (LF_SIMPLECOMBO|LF_DROPDOWNCOMBO)

#define LIST_INFO				0 		// class LIST
#define TABLE_INFO			1			// array of LIST_TABLE
#define STRINGS_INFO		2			// packed strings
#define EDIT_INFO				3			// The text in the edit of combo boxes.if len=0, selection is not in the edit
#define NUM_LIST_DATAS	4

////////////////////////////////////////////////////////////
//	For every item in the list, there is a table entry of type
//	LIST_TABLE.  This class contains a field, charIdx, which is
//	the length of all strings up until its end.  The starting location
//	for entry[3] is at table[2].charIdx.  The starting location
//	for entry[0] is 0.
////////////////////////////////////////////////////////////
class LIST_TABLE
	{
	public:
		LONG charIdx; // length of all strings in list including this one
		BOOL isSelected;  //TRUE if selected
	};
typedef LIST_TABLE far * LPLIST_TABLE;

//////////////
class LIST
	{
	public:
		int	m_version;
		LONG m_style;
		int	m_nItems;
		int	m_curSelection;
		LONG m_stringsSize;	//number of characters in the string table
		ATTRIBUTES m_attributes;
		OBJECTID m_oiText;	//id of text object to send selection to
		OBJECTID m_oiList;	//id of this object

		LPVOID operator new(size_t size, OBJECTID id, LONG numDatas) ;
		LIST(OBJECTID id, LONG s = LF_LISTBOX) ;
		BOOL addItem(LPSTR str, int idx);
		void RemoveItem(int idx);
		void RemoveSelectedItems();
		void FillListBox(HWND hWnd, BOOL forceLB=FALSE);
		void GetSelections(HWND hWnd);
		void SetSelections(HWND hWnd);
		void SetMultiSelections(HWND hWnd);
		void SetSingleSelections(HWND hWnd);
		void SetComboSelections(HWND hWnd);
		void SelectedString(LPSTR str);

		void SendSelectionToText();
		} ;
typedef LIST far * LPLIST;

///////////////////////////////////
class EDIT_LIST
	{
	public:
//		static FARPROC defFunctionListBox;
//		static FARPROC defFunctionComboBox;

	private:
		LPLIST m_lst;
		HWND m_hWndDlg;
		MEMBLOCKID m_lstMID ;
		MEMBLOCKID m_tableMID ;
		MEMBLOCKID m_stringsMID;
		int	m_curFocus;

	public:
		EDIT_LIST(OBJECTID id);
		~EDIT_LIST();

		void CheckBoxHit(int item, LONG bitMask);
		void RadioButtonClicked(LONG setMask, LONG groupMask);
		void EnableProperControls();
		BOOL AddItem(LPSTR str, int idx);
		BOOL RemoveMultItems(LPINT array, int num);
		BOOL RemoveItem(int idx);
		void Store();
		void InitDialog();
		void FillListBox();
		void FillTextObjectsList();
		void DrawListItem(LPDRAWITEMSTRUCT di);

		friend LRESULT far pascal _export Lst_SetupDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		friend BOOL InitDialog(HWND hWnd, OBJECTID oiList) ;
	};
typedef EDIT_LIST far * LPEDIT_LIST;
/////////////////////////////////////////////////////////
class LIST_VI : public OpHwndWndItem
	{
	protected:
		HFONT m_hfont ;
		HBRUSH m_hbrush ;
		COLORREF m_foreground;
		COLORREF m_background;
	public:
		LRESULT ALMAPI ParentNotification(HWND hwnd, UINT message, UINT wParam, LPARAM lParam) ;
		LRESULT selectionChanged() ;
		void ALMAPI ParentClosing() ;
		void ReturnKeyPressed() ;
		void ALMAPI DataChanged(long details, long modifiers, long variation) ;
		void ALMAPI Destroy() ;
		BOOL ALMAPI MeasureItem(MEASUREITEMSTRUCT* measureItem) ;
		BOOL ALMAPI DrawItem(DRAWITEMSTRUCT* drawItem) ;
		long ALMAPI AdjustNewRect(RECT* newRect, LONG adjustFlag) ;
		void ALMAPI Move(RECT* newRect) ;
		LIST_VI(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special) ;
	};
typedef LIST_VI far * LPLIST_VI;
////////////////////////////
class LIST_VI_LISTBOX : protected LIST_VI
	{
	public:
		void ALMAPI ParentOpened() ;
		LIST_VI_LISTBOX(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special)
			: LIST_VI(oiList, oiParent, rcItem, status, special) {} ;
	};
typedef LIST_VI_LISTBOX far * LPLIST_VI_LISTBOX;

/////////////////////////
class LIST_VI_COMBOBOX : protected LIST_VI
	{
	public:
		void ALMAPI ParentOpened() ;
		LIST_VI_COMBOBOX(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special)
			: LIST_VI(oiList, oiParent, rcItem, status, special) {} ;
	} ;
typedef LIST_VI_COMBOBOX far * LPLIST_VI_COMBOBOX;

/////////////////////////
inline LPSTR GetListString(int idx, LPLIST_TABLE table,	LPSTR strings)
	{
	if (idx < 0)
		return "" ;
	if (idx == 0)
		return strings ;
	else
		return &strings[(int)table[idx-1].charIdx] ;
	}

extern "C" {
LONG FAR PASCAL Administration(OBJECTID oiList, pAObjMessage theSystem);
OpWndItemD* FAR PASCAL CreateListVI(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status, long special) ;
LONG FAR PASCAL NewListboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam) ;
LONG FAR PASCAL NewComboboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam) ;
}
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
/////// From listobjd  /////////////
LONG ValidateIDs(OBJECTID oiList, pAObjMessage theSystem) ;
LONG EvaluateObject(OBJECTID oiList) ;
LONG BuildSignals(OBJECTID oiList, pAObjMessage theSystem) ;

/////// From lst_adm  /////////////

////////  From lst_def  /////////////
EXTERNC ListObjectDef(OBJECTID oiList, pAObjMessage theSystem);

////////  From lst_dlg   //////////////
LONG EditListObject(OBJECTID oiList);

///////  From lst_utl   ///////////////
BOOL insertListString(LPSTR str, int strLength, int idx, int numItems, LPLIST_TABLE table, LPSTR strings, LONG stringsSize);
int deleteListString(int idx, int numItems, LPLIST_TABLE table, LPSTR strings, LONG stringsSize);
void fixTable(LPLIST_TABLE table, LPSTR strings, int numItems);
void SortList(LPLIST_TABLE table, LPSTR strings, int numItems);

///////  From lst_mgr   ////////////////
EXTERNC StartupMeth(pAEvtInfo theEvent, pAObjMessage theSystem);
EXTERNC ShutdownMeth(pAEvtInfo theEvent, pAObjMessage theSystem);
EXTERNC TextToObjectMeth(pAEvtInfo theEvent, pAObjMessage theSystem);
EXTERNC ObjectToTextMeth(pAEvtInfo theEvent, pAObjMessage theSystem);

#endif

