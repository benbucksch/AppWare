#ifndef B_DATA_H
#define B_DATA_H

// TRACK BUTTON EVENT -- used to post I_SIG_TOUCHED
#define OBUTN_EVT_TRACKBUTTON AEVENT_MAKEID('t','r','k','B')

extern long CheckButtonObjectVersion(AOBJECTID	oiButton);


//////////////////////////////////
#define I_VERSION 			200
//////////////////////////////////

////////////////////////////////////////////
/////// Button Object Signals    ///////////
////////////////////////////////////////////
#define I_SIG_PUSHED					0
#define I_SIG_TOUCHED					1
#define I_SIG_ISDOWN					2
#define I_SIG_TOGGLED_OFF			3
#define I_SIG_TOGGLED_ON			4
#define I_SIG_MOUSE_UP				5
#define I_SIG_DOUBLECLICK			6
#define I_NUM_SIGNALS					7

////////////////////////////////////////////
/////// Button Style Flags       ///////////
////////////////////////////////////////////
#define I_BF_PUSHBUTTON					0x00000001L
#define I_BF_CHECKBOX   	      0x00000002L
#define I_BF_RADIOBUTTON  	    0x00000004L
#define I_BF_INVISIBLE      	  0x00000008L
#define I_BF_BITMAP           	0x00000010L
#define I_BF_NORMAL	           	0x00000020L
#define I_BF_DEFAULT	          0x00000040L
#define I_BF_CANCEL   	        0x00000080L
#define I_BF_ENABLED    	      0x00000100L
#define I_BF_SELECTED	     	    0x00000200L
#define I_BF_INVERTS        	  0x00000400L
#define I_BF_HASALTKEY        	0x00000800L
#define I_BF_INVERTED	         	0x00001000L
#define I_BF_CAPTUREDBYKEY 	    0x00002000L
#define I_BF_TITLEEDITED				0x00004000L
//for bitmap buttons
#define I_BF_BMP_CROP						0x00010000L
#define I_BF_BMP_STRETCH				0x00020000L
#define I_BF_BMP_ORIGPROP				0x00040000L
//what bitmaps have been defined by the user
#define I_BF_BMP_NORMAL_EXISTS	0x00080000L
#define I_BF_BMP_FOCUSED_EXISTS	0x00100000L
#define I_BF_BMP_PRESSED_EXISTS	0x00200000L
#define I_BF_BMP_GRAYED_EXISTS	0x00400000L

#define I_BF_BMP_STYLES	(I_BF_BMP_CROP | I_BF_BMP_STRETCH | I_BF_BMP_ORIGPROP)
#define I_BF_BMP_EXISTS (BF_BMP_NORMAL_EXISTS	| BF_BMP_FOCUSED_EXISTS	| BF_BMP_PRESSED_EXISTS | BF_BMP_GRAYED_EXISTS)

#define I_BF_TYPE_BITS (I_BF_PUSHBUTTON | I_BF_CHECKBOX | I_BF_RADIOBUTTON | I_BF_INVISIBLE | I_BF_BITMAP)
#define I_BF_PUSH_STYLE_BITS (I_BF_NORMAL | I_BF_DEFAULT | I_BF_CANCEL)
#define I_BF_BITMAP_STYLE_BITS (I_BF_BMP_CROP | I_BF_BMP_STRETCH | I_BF_BMP_ORIGPROP)
#define I_BF_USES_SELECT_BITS	(I_BF_CHECKBOX | I_BF_RADIOBUTTON)
#define I_BF_USES_ALTKEY_BITS (I_BF_PUSHBUTTON | I_BF_CHECKBOX | I_BF_RADIOBUTTON)
#define I_BF_WINDOWS_BUTTONS (I_BF_PUSHBUTTON | I_BF_CHECKBOX | I_BF_RADIOBUTTON)
#define I_BF_DEFAULT_BUTTON_STYLE	(I_BF_PUSHBUTTON | I_BF_NORMAL | I_BF_ENABLED)
	
//////////////////////////////////////////
//////  Button Object Datas        ///////
//////////////////////////////////////////
#define I_DATA_STYLE					0L
#define I_DATA_BMP_NORMAL			1L
#define I_DATA_BMP_FOCUSED		2L
#define I_DATA_BMP_PRESSED		3L
#define I_DATA_BMP_GRAYED			4L
#define	I_NUM_DATAS						5L

//////////////////////////////////////////
#define I_TITLE_LEN						31

enum BITMAP_STATE {I_NORMAL, I_FOCUSED, I_PRESSED, I_GRAYED};
enum BITMAP_STYLE {I_CROP, I_STRETCH, I_ORIGPROP, I_UNDERTIMINED};
enum INVISIBLE_STATE {I_UP, I_DOWN};

//////// Visual Appearance Changed Bits //////
#define I_APPEARANCE_CHANGED_TITLE		0x00000001
#define I_APPEARANCE_CHANGED_ENABLE		0x00000002


/// Default size for a bitmap button with no bitmap
#define I_DEFAULT_BM_WIDTH		64
#define I_DEFAULT_BM_HEIGHT		64


///////////////////////////////////////////////////////////////////////////////
//  This class is what is stored in I_DATA_BMP_NORMAL	- I_DATA_BMP_GRAYED		///
//  It is the bitmaps the bitmap button uses.
////////////////////////////////////////////////////////////////////////////////
class BMP_DATA
	{
	public:
		BITMAPFILEHEADER			m_bmpFH;
		BITMAPINFOHEADER			m_bmpIH;
		BYTE									m_rgbQuadAndBits[];

	public:
		BOOL Draw(HDC hdc, LPRECT rect, BITMAP_STYLE bmpStyle);
		void GetSize(LPPOINT ptSize, BITMAP_STYLE bmpStyle);
		HBITMAP CreateMemBitmap(HPALETTE hPalette, LPPOINT ptSize, BITMAP_STYLE bmpStyle);

		WORD GetPaletteForBitmap(AMEMBLOCKID miLogPalette);
		HPALETTE CreatePaletteForBitmap();
		LONG Export(LPSTR szFieldName);
		LONG Import(LPSTR szFieldName, LONG lBmpSize);
	};
typedef BMP_DATA  * LPBMP_DATA;

//////////////////////////////////////////////////////
// This class is the button object style and etc..
// It is stored in I_DATA_STYLE
///////////////////////////////////////////////////////
class EDIT_BUTTON;

class BUTTON 
	{
	protected:
		short			m_version;
		AOBJECTID		m_oiSelf;
		char			m_szTitle[I_TITLE_LEN+1];
		long			m_style;
		char			m_bBeingTracked;
		RECT			m_rcTracking;   			// in screen coords
		ATTRIBUTES  m_attributes;   			//Font, foreground, background
		char			m_accelVKey;  	 			//used for bitmap & invisible buttons
		char			m_accelMKey;  	 			//used for bitmap & invisible buttons

	private:
		BUTTON(){}
		LPVOID operator new(size_t size) {return NULL;}

	public:
		LPVOID operator new(size_t size, AOBJECTID oi);//Locks the data	
		BUTTON(AOBJECTID id, BOOL bIsTemp); //Unlocks the data
		
		BUTTON(BUTTON  *ptr);//Unsed to setup the virtual table
		LPVOID operator new(size_t size, BUTTON  *ptr);

		BOOL SetButtonType(OBUTN_TYPE type); //return TRUE if the type was changed
		OBUTN_TYPE GetButtonType(void);
		BOOL IsButtonWindowsType(void);

		BOOL SetBitmapStyle(BITMAP_STYLE style);
		BITMAP_STYLE GetBitmapStyle(void);
		
		BOOL SetButtonIsDefault(BOOL set);
		BOOL IsButtonDefault(void);

		BOOL SetButtonIsCancel(BOOL set);
		BOOL IsButtonCancel(void);

		BOOL SetButtonSelected(BOOL set);// Also posts toggled signals and OBJECT_CHANGED
		BOOL IsButtonSelected(void);
		BOOL DoesButtonSupportSelection(void);

		BOOL SetButtonEnabled(BOOL set); 
		BOOL IsButtonEnabled(void);

		BOOL SetButtonInverts(BOOL set);
		BOOL DoesButtonInvert(void);

		BOOL SetButtonIsCapturedByKey(BOOL set);
		BOOL IsButtonCapturedByKey(void);

		BOOL SetTitleHasBeenEdited(BOOL set);
		BOOL HasTitleBeenEdited();
		BOOL MakeSureButtonHasTitle();

		BOOL SetNormalBitmapExists(BOOL set);
		BOOL DoesNormalBitmapExist(void);

		BOOL SetFocusedBitmapExists(BOOL set);
		BOOL DoesFocusedBitmapExist(void);

		BOOL SetPressedBitmapExists(BOOL set);
		BOOL DoesPressedBitmapExist(void);

		BOOL SetGrayedBitmapExists(BOOL set);
		BOOL DoesGrayedBitmapExist(void);

		BOOL CreateBitmapsHandles(HPALETTE hPalette, LPRECT rect, HBITMAP* hbmpNormal, HBITMAP* hbmpFocused, HBITMAP* hbmpPressed, HBITMAP* hbmpGrayed);

		LONG GetCreateWindowStyleBits();	 

		BOOL SetControlsText(HWND hwnd); //returns TRUE if the text was changed
		BOOL SetCheckState(HWND hwnd);  //returns TRUE if the check state was changed. 		

		long ValidateObjectIDs(AOBJECTID oi){m_oiSelf = oi; return A_OK;}
		void BuildSignals(AMEMBLOCKID miSignalBlock);
		OpWndItemD* CreateVisualInstance(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

		inline LPCSTR GetTitle() {return (LPCSTR)m_szTitle;}
		BOOL SetTitle(LPCSTR szTitle); //Also posts OBJECT_CHANGED
		void GetAccelKeyFromTitle();

		inline void CopyAttributes(LPATTRIBUTES attTarget){CopyMemory(attTarget, &m_attributes, sizeof(ATTRIBUTES));}

		BOOL SetAccelKey(char vKey, char mKey);
		inline char GetAccelVKey(void){return m_accelVKey;}
		inline char GetAccelMKey(void){return m_accelMKey;}

		void BeginTrackingButton(OpWndItemD* wi);
		inline BOOL IsBeingTracked(void) {return m_bBeingTracked?TRUE:FALSE;}
		void EndTrackingButton(void);
		inline void SendTrackingEvent(void) {AEvtPostStandard(m_oiSelf, OBUTN_EVT_TRACKBUTTON, 0, AEVT_ATTAIL);}
		void ProcessTrackingEvent();

		void ObjectToText(AMEMBLOCKID miText);
		void TextToObject(AMEMBLOCKID miText);

		void AppearanceChanged(long fwWhatChanged);

		inline void PostObjectChanged(void)				
			{AEvtPostStandard(m_oiSelf, AEVENT_OBJECTCHANGED, 0, AEVT_ATMARK);}

		inline void PostPushedSignal(void) 				
			{AEvtPostSignalAtTail(m_oiSelf,I_SIG_PUSHED); }

		inline void PostDoubleClickedSignal(void) 
			{AEvtPostSignalAtTail(m_oiSelf,I_SIG_DOUBLECLICK); }

		inline void PostTouchedSignal(void)				
			{AEvtPostSignalAtTail(m_oiSelf,I_SIG_TOUCHED); }

		inline void PostIsDownSignal(void)				
			{AEvtPostSignalAtTail(m_oiSelf,I_SIG_ISDOWN); }

		inline void PostToggledOnSignal(void)			
			{AEvtPostSignalAtTail(m_oiSelf,I_SIG_TOGGLED_ON); }

		inline void PostToggledOffSignal(void)		
			{AEvtPostSignalAtTail(m_oiSelf,I_SIG_TOGGLED_OFF); }

		inline void PostMouseUpSignal(void)				
			{AEvtPostSignalAtTail(m_oiSelf,I_SIG_MOUSE_UP); }

		long Export();
		long Import();

		friend EDIT_BUTTON;
		friend long CheckButtonObjectVersion(AOBJECTID	oiButton);
		};
typedef BUTTON  * LPBUTTON;
																
/////////////////////////////////////////////////////////
// This Class controls the editing of the button object
/////////////////////////////////////////////////////////
class EDIT_BUTTON:public BUTTON
	{
	private:
		AMEMBLOCKID						m_miNormal, m_miFocused, m_miPressed, m_miGrayed;
		BITMAP_STATE 					m_currentState;  //The current bitmap that is being edited
		HPALETTE							m_hPalette;

  private:
		LPVOID operator new(size_t size, AOBJECTID id) {return NULL;}

	public:
		LPVOID operator new(size_t size) {return ::new char[size];}
		void operator delete(LPVOID	ptr) {::delete(ptr);}

	public:
		EDIT_BUTTON(AOBJECTID id);
		~EDIT_BUTTON();

		BOOL Store(void);

		void SetTitle(HWND hwnd, int controlID) {GetDlgItemText(hwnd, controlID, m_szTitle, I_TITLE_LEN);}

		void InitButtonDialog(HWND hwnd);
		void InitBitmapDialog(HWND hwnd);


		void RadioButtonClicked(HWND hwnd, LONG setMask, LONG groupMask);
		void CheckBoxHit(HWND hwnd, int item, LONG  bitMask);
		void ShowProperControls(HWND hwnd);

	  void Draw(LPDRAWITEMSTRUCT di);
	  void SetDialogButtonTitles(HWND hwnd, BOOL allowAccels);

		void EditFont(HWND hwndOwner);


		BOOL SetCurrentState(BITMAP_STATE state);
		BITMAP_STATE GetCurrentState(void);

		void CreateDefaultBitmaps();  //Also sets m_hPalette

		BOOL LoadBitmap(void);			//from a file	-- Sets m_whatExists and creates default bitmaps
		BOOL LoadBitmap(HWND hWnd);	//from the clipboard -- Sets m_whatExists and creates default bitmaps
		void BitmapLoaded(void); 		//Called when a bitmap is loaded.  Set m_whatExists
		void BitmapDeleted(void);		//Called when a bitmap is deleted.  Set m_whatExists
		void SetBitmapMemUsage(HWND hwnd);	//Sets the static text indicating the mem usage for bitmap buttons

		AMEMBLOCKID GetCurrentBitmapMemblock(void);  //returns miNormal, m_miFocused, m_miPressed, m_miGrayed depending on m_currentState

		inline HPALETTE GetPaletteHandle(void) {return m_hPalette;}

	};

typedef EDIT_BUTTON  * LPEDIT_BUTTON;

/////////////////////////////////////////
// Utility Functions in b_editu.cpp   //
///////////////////////////////////////




#endif //B_DATA_H

