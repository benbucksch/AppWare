#ifndef N_DATA_H
#define N_DATA_H

#ifdef I_MAIN
LPSTR								g_szEmpty = "\0";
#else
extern LPSTR				g_szEmpty;
#endif


#define I_THE_VERSION								201

/////////////////////////////////////////////
///  Number Object Signals
/////////////////////////////////////////////
#define I_SIG_CHANGED								0L
#define I_SIG_ACTIVATED     				1L
#define I_SIG_DEACTIVATED   				2L
#define I_SIG_RANGEERROR   					3L
#define I_SIG_EDITED								4L
#define I_NUM_SIGNALS								5L
//////////////////////////////////////////
///  Data Block Defines
/////////////////////////////////////////

#define I_DATA_STYLE				        0L
#define I_NUM_DATAS    							1L

//////////////////////////////////////////
// Number style flags /////
//////////////////////////////////////////
#define I_NF_JUST_LEFT	        0x00000001L
#define I_NF_JUST_RIGHT   	    0x00000004L
#define I_NF_FRAME	        		0x00000400L
#define I_NF_EDITABLE   	      0x00000800L
#define I_NF_INUMBER          	0x00004000L
#define I_NF_RNUMBER	          0x00008000L
#define I_NF_RANGE_ERR					0x00800000L
#define I_NF_SELECTALL					0x01000000L
#define I_NF_DISPLAYEMPTY				0x08000000L
#define I_NF_ACTIVATABLE				0x10000000L
#define I_NF_HASCHANGED					0x04000000L  //set if the value has changed while editing. if true, objectchanged will be sent on <Enter> or deactivate
#define I_NF_TEMPOBJECT					0x02000000L

#define I_NF_JUSTIFY_BITS   (I_NF_JUST_LEFT | I_NF_JUST_RIGHT)
#define I_NF_NUMTYPE_BITS		(I_NF_INUMBER | I_NF_RNUMBER )
#define I_NF_DEFNUMBERSTYLE	(I_NF_SELECTALL | I_NF_JUST_LEFT | I_NF_FRAME | I_NF_EDITABLE | I_NF_INUMBER)
#define I_NF_HWND_STYLEBITS (I_NF_EDITABLE | I_NF_ACTIVATABLE | I_NF_FRAME)

//////////////////////////////////
#define I_FORMAT_STRING_LENGTH	63
#define I_STRING_LEN						35

//////////////////////////////////
#define I_SMALLEST_LONG	 ((long double)-LONG_MAX)
#define I_LARGEST_LONG	 ((long double)LONG_MAX)
///////////////////////////////////////////	 

typedef enum {POST_SIGNALS=TRUE,					DONT_POST_SIGNALS=FALSE}					I_POST_TYPE;
typedef enum {ROUND_IF_INT=TRUE,					DONT_ROUND_IF_INT=FALSE}					I_ROUND_TYPE;
typedef enum {CHANGE_IF_PSEUDO=TRUE,			DONT_CHANGE_IF_PSEUDO=FALSE}			I_CHANGE_TYPE;						
typedef enum {BLANK_IF_EMPTY_STRING=TRUE, DONT_BLANK_IF_EMPTY_STRING=FALSE}	I_BLANK_TYPE;

///////////////////////////////////////////

struct NFORM
	{
	int						formatIdx;
	char					forceDecimal;
	char					forceSign;
	char				 	usesPercent;
	char					widthWhole;
	char					widthFloat;
  char					unused[6];
	};
typedef NFORM far * LPNFORM;

struct NFORM_INFO
	{
	NFORM		positive;
	NFORM		negitive;
	NFORM		zero;
  };
typedef NFORM_INFO far * LPNFORM_INFO;

////////////////////////////

class EDIT_NUMBER;
class NUMBER
	{
	protected:
		short								m_version;
    OBJECTID						m_oiSelf;
		LONG          			m_style;
		LONG								m_wholeValue;
    long double					m_floatValue;
		ATTRIBUTES    			m_attributes;   //Font, foreground, background
		char								m_szFormat[I_FORMAT_STRING_LENGTH];
		NFORM_INFO					m_formatInfo;
		char								m_szNumber[I_STRING_LEN];


	private:
		NUMBER();
		LPVOID operator new(size_t size) {return NULL;}

  public:
		NUMBER(NUMBER far *ptr); //Unsed to setup the virtual table
		LPVOID operator new(size_t size, NUMBER far *ptr); //Unsed to setup the virtual table

		NUMBER(OBJECTID id, BOOL bIsTemp);
		LPVOID operator new(size_t size, OBJECTID id);
		void operator delete(LPVOID	ptr);

		inline BOOL JustifiesLeft()			{return m_style & I_NF_JUST_LEFT ? TRUE:FALSE;}
		inline BOOL JustifiesRight()				{return m_style & I_NF_JUST_RIGHT ? TRUE:FALSE;}
		inline BOOL IsEditable()						{return m_style & I_NF_EDITABLE ? TRUE:FALSE;}
		inline BOOL IsActivatable()					{return m_style & (I_NF_ACTIVATABLE|I_NF_EDITABLE) ? TRUE:FALSE;}
		inline BOOL SelectsAll()						{return m_style & I_NF_SELECTALL ? TRUE:FALSE;}
		inline I_BLANK_TYPE DisplaysEmpty()	{return m_style & I_NF_DISPLAYEMPTY ? BLANK_IF_EMPTY_STRING : DONT_BLANK_IF_EMPTY_STRING;}
		inline BOOL HasFrame()							{return m_style & I_NF_FRAME ? TRUE:FALSE;}
		inline BOOL IsInteger()							{return m_style & I_NF_INUMBER ? TRUE:FALSE;}
		inline BOOL IsReal()								{return m_style & I_NF_RNUMBER ? TRUE:FALSE;}
		inline BOOL HadRangeError()					{return m_style & I_NF_RANGE_ERR ? TRUE: FALSE;}
		inline BOOL HasChanged()						{return m_style & I_NF_HASCHANGED ? TRUE:FALSE;}
		inline BOOL	IsTempObject()					{return m_style & I_NF_TEMPOBJECT ? TRUE:FALSE;}
		inline BOOL VisualInstanceIsHwnd() 	{return m_style & I_NF_HWND_STYLEBITS ? TRUE:FALSE;}

		inline void SetJustifiesLeft()				 				{m_style &= ~I_NF_JUSTIFY_BITS; m_style |= I_NF_JUST_LEFT;}
		inline void SetJustifiesRight()				 				{m_style &= ~I_NF_JUSTIFY_BITS; m_style |= I_NF_JUST_RIGHT;}
		inline void SetIsEditable(BOOL set)		 				{if (set) m_style |= I_NF_EDITABLE; else m_style &= ~I_NF_EDITABLE;}
		inline void SetIsActivatable(BOOL set) 				{if (set) m_style |= I_NF_ACTIVATABLE; else m_style &= ~I_NF_ACTIVATABLE;}
		inline void SetSelectsAll(BOOL set)		 				{if (set) m_style |= I_NF_SELECTALL; else m_style &= ~I_NF_SELECTALL;}
		inline void SetDisplaysEmpty(I_BLANK_TYPE set){if (set==BLANK_IF_EMPTY_STRING) m_style |= I_NF_DISPLAYEMPTY; else m_style &= ~I_NF_DISPLAYEMPTY;}
		inline void SetHasFrame(BOOL set)			 				{if (set) m_style |= I_NF_FRAME; else m_style &= ~I_NF_FRAME;}
		inline void SetIsInteger()				 		 				{m_style &= ~I_NF_NUMTYPE_BITS; m_style |= I_NF_INUMBER;}
		inline void SetIsReal()				 		 		 				{m_style &= ~I_NF_NUMTYPE_BITS; m_style |= I_NF_RNUMBER;}
		inline void SetHasChanged(BOOL set)		 				{if (set) m_style |= I_NF_HASCHANGED; else m_style &= ~I_NF_HASCHANGED;}
		inline void SetTempObject(BOOL set)		 				{if (set) m_style |= I_NF_TEMPOBJECT; else m_style &= ~I_NF_TEMPOBJECT;}
		void SetRangeError(BOOL set, I_POST_TYPE fPostSignals);

		UINT GetDrawTextStyle();
		LONG GetCreateWindowStyle();

		void CopyAttributes(LPATTRIBUTES attTarget);

		long GetWholeValue(long minVal=LONG_MIN, long maxVal=LONG_MAX); 
		long double GetFloatValue();

    ////////////////////  These functions need to set the HAS_CHANGED bit !!!!!!!!!!!!!!!!
		private:
		BOOL LocalSetValue(long double flValue, I_POST_TYPE fPostSignals, I_ROUND_TYPE fRoundIfInteger);
		public:
		BOOL SetValue(long double flValue, I_POST_TYPE fPostSignals, I_ROUND_TYPE fRoundIfInteger);
		BOOL SetValue(long lValue, I_POST_TYPE fPostSignals);
		BOOL SetValue(LPCSTR str, I_POST_TYPE fPostSignals, I_CHANGE_TYPE fChangeTypeIfPseudo, I_BLANK_TYPE fBlankOutIfEmptyString);
		///////////////////////////////////////////////////////////////////////////////////////
    void FormatNumber();

		inline const char far *GetStringPtr(){return DisplaysEmpty() ? g_szEmpty : m_szNumber;}
		virtual LPSTR GetFormatPtr();
		virtual LPNFORM_INFO GetFormatInfoPtr();


		void EditFont(HWND owner = NULL, LPSTR str="AaBbYyZz123", BOOL bDoBackground = TRUE)	
			{m_attributes.AttrEditFont(owner, 
																	str, 
																	"Edit Atributes", 
																	TRUE, 
																	CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_ENABLETEMPLATE|CF_ENABLEHOOK, 
																	bDoBackground);
			}

		inline LPATTRIBUTES GetAttributes()		{return &m_attributes;}

		long GetType();    //returns ONMBR_INTEGER, ONMBR_REAL
		long GetStatus();  //returns ONMBR_RANGE_ERR, ONMBR_HAS_CHANGED

		void ValidateObjectIDs(OBJECTID oiNumber);
		void BuildSignals(MEMBLOCKID miSignals);
		long DoVersionCheck(void);


		BOOL MaybePostChangedStuff();

		long Export();
		long Import();

		friend EDIT_NUMBER;

	};
typedef NUMBER far * LPNUMBER;

////////////////////////////


class EDIT_NUMBER:public NUMBER
  {
  private:
		HWND            			m_hwndDlg;
		char									m_szFormatInteger[I_FORMAT_STRING_LENGTH];
		NFORM_INFO						m_formatInfoInteger;
		char									m_szFormatReal[I_FORMAT_STRING_LENGTH];
		NFORM_INFO						m_formatInfoReal;

  private:
		LPVOID operator new(size_t size, OBJECTID id) {return NULL;}

	public:
		LPVOID operator new(size_t size) {return ::new char[size];}
		void operator delete(LPVOID	ptr) {::delete(ptr);}

		inline const char far *GetStringPtr(){return m_szNumber;}

		virtual inline char far *GetFormatPtr(){ if (IsReal()) return m_szFormatReal; else return m_szFormatInteger;}
		virtual inline LPNFORM_INFO GetFormatInfoPtr(){ if (IsReal()) return &m_formatInfoReal; else return &m_formatInfoInteger;}

		EDIT_NUMBER(OBJECTID oi);
		~EDIT_NUMBER();
		inline void SetWindowHandle(HWND hwnd) {m_hwndDlg = hwnd;}
		void EnableProperControls();
		void Store();
		void InitDialog();
	};
typedef EDIT_NUMBER far * LPEDIT_NUMBER;



#endif
