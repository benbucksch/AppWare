#ifndef L_DATA_H
#define L_DATA_H

/////////////////////////////////////////
///  List Object Style Flags   //////////
/////////////////////////////////////////

#define I_LF_MULTISELECT			0x00000002L
#define I_LF_LISTBOX					0x00000004L
#define I_LF_SIMPLECOMBO 			0x00000008L
#define I_LF_DROPDOWNCOMBO		0x00000010L
#define I_LF_DROPDOWNLIST			0x00000020L

#define I_LF_LIST_STYLES (I_LF_LISTBOX|I_LF_SIMPLECOMBO|I_LF_DROPDOWNCOMBO|I_LF_DROPDOWNLIST)
#define I_LF_COMBOS (I_LF_SIMPLECOMBO|I_LF_DROPDOWNCOMBO|I_LF_DROPDOWNLIST)
#define I_LF_EDITABLE_COMBOS (I_LF_SIMPLECOMBO|I_LF_DROPDOWNCOMBO)

//////////////////////////////////////////
///  List Object Data Blocks   ///////////
//////////////////////////////////////////
#define I_DATA_STYLE					0L
#define I_DATA_TABLE					1L
#define I_DATA_STRINGS				2L
#define I_DATA_EDIT						3L
#define I_NUM_DATAS						4L

//////////////////////////////////////////
///  List Object Signals       ///////////
//////////////////////////////////////////
#define I_SIG_DOUBLECLICKED					 0
#define I_SIG_SINGLECLICKED					 1
#define I_SIG_CHANGED								 4
#define I_SIG_ENTERKEY							 2
#define I_SIG_RETURNKEY							 3
#define I_SIG_ACTIVATED							 10
#define I_SIG_DEACTIVATED						 11
#define I_SIG_SELECTION_CHANGED			 5
#define I_SIG_EDITED								 6
#define I_SIG_DOWN						 			 7
#define I_SIG_UP						 				 8

#define I_NUM_BASE_SIGNALS					 6


///////////////////////////////////////////

enum OLIST_TYPE {OLIST_LISTBOX, OLIST_SIMPLECOMBO, OLIST_DROPDOWNCOMBO, OLIST_DROPDOWNLIST, OLIST_UNKNOWN=-1}; 

extern long CheckListObjectVersion(OBJECTID oiList);

#define I_VERSION				100


////////////////////////////////////////////////////////////
//	For every item in the list, there is a table entry of type
//	LIST_TABLE.  This struct contains a field, lCharIdx, which is
//	the length of all strings up until its end.  The starting location
//	for entry[3] is at table[2].lCharIdx.  The starting location
//	for entry[0] is 0.
////////////////////////////////////////////////////////////
struct LIST_TABLE
	{
	LONG 										lCharIdx; 		// length of all strings in list including this one
	BOOL 										bIsSelected;  //TRUE if selected
	};
typedef LIST_TABLE far * LPLIST_TABLE;


///////////////////////////////////////////
class EDIT_LIST;

class LIST
	{
	private:
		short									m_version;
    OBJECTID							m_oiSelf;
		LONG 									m_style;
		unsigned short				m_nItems;
		unsigned short				m_curSelection;
		long									m_lStringsSize;	//number of characters in the string table
		ATTRIBUTES 						m_attributes;
		OBJECTID 							m_oiText;				//text object to send selection to

	public:
		LPVOID operator new(size_t size, OBJECTID id);
		LIST(OBJECTID id, LONG s = I_LF_LISTBOX);

		LIST(LIST far *ptr);//Unsed to setup the virtual table
		LPVOID operator new(size_t size, LIST far *ptr);


		inline short GetVersion(void) {return m_version;}
		inline unsigned short NumItems(void) {return m_nItems;}

		BOOL SetDoesMultipleSelections(BOOL set);
		BOOL DoesMultipleSelections(void);

		OLIST_TYPE GetType(void);
		BOOL SetType(OLIST_TYPE type);
		inline BOOL IsListCombobox() {return (GetType()==OLIST_SIMPLECOMBO || GetType()==OLIST_DROPDOWNCOMBO || GetType()==OLIST_DROPDOWNLIST);}

		BOOL addItem(LPSTR str, int idx, int cbStr=-1);
		BOOL RemoveItem(int idxItem);			//return TRUE if the removing cause the selections to change
		BOOL RemoveSelectedItems(void);
		BOOL RemoveAllItems(void);

		BOOL FillListBox(HWND hWnd, BOOL forceLB=FALSE/*used during editing*/);

		BOOL SelectedString(LPSTR str);	//return TRUE if selection changed

		void SendSelectionsToText(void);

		inline void CopyAttributes(LPATTRIBUTES attTarget){_fmemcpy(attTarget, &m_attributes, sizeof(ATTRIBUTES));}
				
		void EditFont(HWND owner = NULL,
									LPSTR str="AaBbYyZz123",
									BOOL doColors=TRUE,
									DWORD flags = CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_ENABLETEMPLATE|CF_ENABLEHOOK,
									BOOL doBackground=TRUE)	{m_attributes.AttrEditFont(owner, str, "Edit Atributes", doColors, flags, doBackground);}

		inline LPATTRIBUTES GetAttributes()		{return &m_attributes;}


		void ObjectToText(MEMBLOCKID miText);
		void TextToObject(MEMBLOCKID miText);

		void ItemsChanged(void);
		void AppearanceChanged(HWND hwnd);
		void EditControlChanged(HWND hwnd);

		BOOL Copy(OBJECTID oiSource);
		LONG Import(void);
		LONG Export(void);
		LONG ValidateObjectIDs(pAObjMessage theSystem);
		LONG CheckObject(void);
		LONG BuildSignals(pAObjMessage theSystem);


		friend EDIT_LIST;
		friend long CheckListObjectVersion(OBJECTID oiList);
		};
typedef LIST far * LPLIST;


///////////////////////////////////
class EDIT_LIST
	{
	private:
		LPLIST 										m_list;
		HWND 											m_hWndDlg;
		MEMBLOCKID 								m_miList;
		MEMBLOCKID 								m_miTable;
		MEMBLOCKID 								m_miStrings;

	public:
		EDIT_LIST(OBJECTID id, HWND hwnd);
		~EDIT_LIST(void);

		void CheckBoxHit(int item, LONG bitMask);
		void RadioButtonClicked(LONG setMask, LONG groupMask);
		void EnableProperControls(void);
		BOOL AddItem(LPSTR str, int idx);
		BOOL RemoveMultItems(LPINT array, int num);
		BOOL RemoveItem(int idx);
		void Store(void);
		void InitDialog(void);
		void FillListBox(void);
		void FillTextObjectsList(void);
		void DrawListItem(LPDRAWITEMSTRUCT di);
		inline void SetTextObject(OBJECTID id) {m_list->m_oiText  = id;}
		inline void EditFont() {m_list->m_attributes.AttrEditFont(m_hWndDlg);}
	};
typedef EDIT_LIST far * LPEDIT_LIST;


#endif
