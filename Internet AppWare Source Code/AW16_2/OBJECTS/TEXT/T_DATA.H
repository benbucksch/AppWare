#ifndef T_DATA_H
#define T_DATA_H

#define I_THE_VERSION								201

/////////////////////////////////////////////
///  Text Object Signals
/////////////////////////////////////////////
#define I_SIG_CHANGED								0L
#define I_SIG_EDITED        				1L
#define I_SIG_ACTIVATED     				2L
#define I_SIG_DEACTIVATED   				3L
#define I_SIG_LENGTHERROR   				4L
#define I_SIG_ENTERKEY      				5L
#define I_NUM_SIGNALS								6L

//////////////////////////////////////////
///  Data Block Defines
/////////////////////////////////////////

#define I_DATA_STYLE				        0L
#define I_DATA_STRING								1L
#define I_NUM_DATAS    							2L

//////////////////////////////////////////
// Text style flags /////
//////////////////////////////////////////
#define I_TF_JUST_LEFT	        0x00000001L
#define I_TF_JUST_CENTER	      0x00000002L
#define I_TF_JUST_RIGHT   	    0x00000004L
#define I_TF_MULTILINE      	  0x00000008L
#define I_TF_EDITABLE   	      0x00000010L
#define I_TF_SELECTALL					0x00000040L
#define I_TF_UPPERCASE        	0x00000100L
#define I_TF_LOWERCASE	        0x00000200L
#define I_TF_USESENTER					0x00000400L
#define I_TF_USESTAB						0x00000800L
#define I_TF_HORZBAR        	  0x00001000L
#define I_TF_VERTBAR          	0x00002000L
#define I_TF_FRAME	        		0x00004000L
#define I_TF_PASSWORD        		0x00008000L
#define I_TF_SINGLELINE					0x00010000L
#define I_TF_STATIC							0x00020000L
#define I_TF_KEEP_SELECTION	0x00040000l

#define I_TF_JUSTIFY_BITS   (I_TF_JUST_LEFT | I_TF_JUST_CENTER | I_TF_JUST_RIGHT)
#define I_TF_CASE_BITS   		(I_TF_UPPERCASE | I_TF_LOWERCASE)
#define I_TF_DEFTEXTSTYLE   (I_TF_SELECTALL | I_TF_JUST_LEFT | I_TF_MULTILINE | I_TF_FRAME | I_TF_EDITABLE)
#define I_TF_TYPE_BITS			(I_TF_MULTILINE | I_TF_SINGLELINE | I_TF_STATIC )

///////////////////////////////////////////

#define SPACE		0x20
#define TAB '\t'
#define CR '\r'
#define LF '\n'


class EDIT_TEXT;

////////////////////////////

class TEXT
	{
	private:
		short								m_version;
    OBJECTID						m_oiSelf;
		LONG          			m_style;
		unsigned short			m_limit;        //Max number of characters
		unsigned short 			m_horzLimit;    //Horz scroll range in pixels
		ATTRIBUTES    			m_attributes;   //Font, foreground, background
		unsigned short			m_tabSize;

	private:
		TEXT();

  public:

		TEXT(OBJECTID id, BOOL bIsTemp);
		LPVOID operator new(size_t size, OBJECTID id);
		void operator delete(LPVOID	ptr);

		inline BOOL JustifiesLeft()			{return m_style & I_TF_JUST_LEFT ? TRUE:FALSE;}
		inline BOOL JustifiesCenter()		{return m_style & I_TF_JUST_CENTER ? TRUE:FALSE;}
		inline BOOL JustifiesRight()		{return m_style & I_TF_JUST_RIGHT ? TRUE:FALSE;}
		inline BOOL IsMultiline()				{return m_style & I_TF_MULTILINE ? TRUE:FALSE;}
		inline BOOL IsSingleline()			{return m_style & I_TF_SINGLELINE ? TRUE:FALSE;}
		inline BOOL IsStatic()					{return m_style & I_TF_STATIC ? TRUE:FALSE;}
		inline BOOL IsEditable()				{return m_style & I_TF_EDITABLE ? TRUE:FALSE;}
		inline BOOL SelectsAll()				{return m_style & I_TF_SELECTALL ? TRUE:FALSE;}
		inline BOOL KeepsSelection()		{return m_style & I_TF_KEEP_SELECTION ? TRUE:FALSE;}
		inline BOOL IsUppercase()				{return m_style & I_TF_UPPERCASE ? TRUE:FALSE;}
		inline BOOL IsLowercase()				{return m_style & I_TF_LOWERCASE ? TRUE:FALSE;}
		inline BOOL UsesEnter()					{return m_style & I_TF_USESENTER ? TRUE:FALSE;}
		inline BOOL UsesTab()						{return m_style & I_TF_USESTAB ? TRUE:FALSE;}
		inline BOOL HasHScroll()				{return m_style & I_TF_HORZBAR ? TRUE:FALSE;}
		inline BOOL HasVScroll()				{return m_style & I_TF_VERTBAR ? TRUE:FALSE;}
		inline BOOL HasFrame()					{return m_style & I_TF_FRAME ? TRUE:FALSE;}
		inline BOOL IsPassword()				{return m_style & I_TF_PASSWORD ? TRUE:FALSE;}

		inline void SetJustifiesLeft()				 {m_style &= ~I_TF_JUSTIFY_BITS; m_style |= I_TF_JUST_LEFT;}
		inline void SetJustifiesCenter()			 {m_style &= ~I_TF_JUSTIFY_BITS; m_style |= I_TF_JUST_CENTER;}
		inline void SetJustifiesRight()				 {m_style &= ~I_TF_JUSTIFY_BITS; m_style |= I_TF_JUST_RIGHT;}
		inline void SetIsMultiline()	 				 {m_style &= ~I_TF_TYPE_BITS; m_style |= I_TF_MULTILINE;}
		inline void SetIsSingleline()	 				 {m_style &= ~I_TF_TYPE_BITS; m_style |= I_TF_SINGLELINE;}
		inline void SetIsStatic()	 				 		 {m_style &= ~I_TF_TYPE_BITS; m_style |= I_TF_STATIC;}
		inline void SetIsEditable(BOOL set)		 {if (set) m_style |= I_TF_EDITABLE; else m_style &= ~I_TF_EDITABLE;}
		inline void SetSelectsAll(BOOL set)		 {if (set) m_style |= I_TF_SELECTALL; else m_style &= ~I_TF_SELECTALL;}
		inline void SetKeepsSelection(BOOL set)		 {if (set) m_style |= I_TF_KEEP_SELECTION; else m_style &= ~I_TF_KEEP_SELECTION;}
		inline void SetUppercase()						 {m_style &= ~I_TF_CASE_BITS; m_style |= I_TF_UPPERCASE;}
		inline void SetLowercase()		 				 {m_style &= ~I_TF_CASE_BITS; m_style |= I_TF_LOWERCASE;}
		inline void SetIsCaseInsensitive()		 {m_style &= ~I_TF_CASE_BITS; }
		inline void SetUsesEnter(BOOL set)		 {if (set) m_style |= I_TF_USESENTER; else m_style &= ~I_TF_USESENTER;}
		inline void SetUsesTab(BOOL set)			 {if (set) m_style |= I_TF_USESTAB; else m_style &= ~I_TF_USESTAB;}
		inline void SetHasHScroll(BOOL set)		 {if (set) m_style |= I_TF_HORZBAR; else m_style &= ~I_TF_HORZBAR;}
		inline void SetHasVScroll(BOOL set)		 {if (set) m_style |= I_TF_VERTBAR; else m_style &= ~I_TF_VERTBAR;}
		inline void SetHasFrame(BOOL set)			 {if (set) m_style |= I_TF_FRAME; else m_style &= ~I_TF_FRAME;}
		inline void SetIsPassword(BOOL set)		 {if (set) m_style |= I_TF_PASSWORD; else m_style &= ~I_TF_PASSWORD;}


		UINT GetDrawTextStyle();
		LONG GetCreateWindowStyle();

		void CopyAttributes(LPATTRIBUTES attTarget);

		inline UINT GetMaxNumChars() {return m_limit;}
		inline void SetMaxNumChars(UINT i) {m_limit = i > 0 ? (i < 32000 ? i : 32000) : 32000;}

		inline UINT GetHorzScrollLimit() {return m_horzLimit;}
		inline void SetHorzScrollLimit(UINT i) {m_horzLimit = i;}

		inline UINT GetTabSize() {return m_tabSize;}
		inline void SetTabSize(UINT i) {m_tabSize = i;}

		
		void EditFont(HWND owner = NULL, LPSTR str="AaBbYyZz123", BOOL bDoBackground = TRUE)	
			{m_attributes.AttrEditFont(owner, 
																	str, 
																	"Edit Atributes", 
																	TRUE, 
																	CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_ENABLETEMPLATE|CF_ENABLEHOOK, 
																	bDoBackground);
			}

		inline LPATTRIBUTES GetAttributes()		{return &m_attributes;}

		void ValidateObjectIDs(OBJECTID oiText);
		void BuildSignals(MEMBLOCKID miSignals);
		long DoVersionCheck(void);

		long Export();
		long Import();

		void Print(LPPIDR_BLOCK pidr);


	};
typedef TEXT far * LPTEXT;

////////////////////////////


class TEXTRUNTIME
	{
	private:
		char									m_szFileFilter[128];
		HwndTextItem*					m_tiLastActive;

	public:
		TEXTRUNTIME();
		~TEXTRUNTIME();

		inline HwndTextItem* GetLastActive() {return m_tiLastActive;}
		inline void SetLastActive(HwndTextItem* ti) {m_tiLastActive = ti;}

		inline LPSTR GetFileFilter()					{return m_szFileFilter;}
		inline void SetFileFilter(LPSTR sz)		{strncpy(m_szFileFilter, sz, 128);}
	};
typedef TEXTRUNTIME far * LPTEXTRUNTIME;

////////////////////////////////

class EDIT_TEXT
  {
  public:
		LPTEXT				    		m_txtPtr;    				    //The style stuff
		MEMBLOCKID						m_miTxt, m_miTitle;			//Mem block ids of the txo, and title
		LPSTR 								m_szTitle;      				//The characters
		HWND            			m_hwndDlg;
		OBJECTID							m_oiStore;

		EDIT_TEXT(OBJECTID oi);
		~EDIT_TEXT();
		void EnableProperControls();
		void Store();
		void InitDialog();
	};
typedef EDIT_TEXT far * LPEDIT_TEXT;



#endif
