#ifndef DATA_H
#define DATA_H

#define I_VERSION  100
#define OTYPE_POLYBUTTON ATYPE_MAKEID('P','o','l','y')


typedef unsigned short USHORT;

//////////////////////////////////
///// Polybutton Signals    //////
//////////////////////////////////
#define I_SIG_CLICKED					0
#define I_SIG_SET							1
#define I_SIG_CLEARED					2
#define I_NUM_BASE_SIGNALS		3
///
#define I_SIG_CLICKED_OFFSET	0
#define I_SIG_SET_OFFSET			1
#define I_SIG_CLEARED_OFFSET	2

//////Data blocks//////////////////
#define I_DATA_POLY					0
#define I_DATA_NORMAL				1
#define I_DATA_HIGHLIGHT		2
#define I_NUM_DATAS					3

////////////////////////// 
#define I_BUTTON_NAME_LENGTH		((USHORT)33)
#define I_UNDEFINED_INDEX				0xFFFF

////////////////////////// 
#define I_INCREMENT_NEXT_UID 		10
#define I_UID_INITIAL_VALUE			100
#define I_MAX_POINTS_AT_EDIT		500

////////////////////////// 

enum REGION_TYPE	{I_RECT=0, I_ELLIPSE, I_POLYGON};
enum COMBINE_MODE	{I_COMBINE_INITIAL=0, I_COMBINE_AND, I_COMBINE_DIFF, I_COMBINE_OR, I_COMBINE_XOR}; 
enum BUTTON_TYPES {I_PUSH=0, I_RADIO, I_CHECK, I_TRANSPARENT, I_UNKNOWN_TYPE=-1};
enum TOOL_TYPE		{I_TOOL_ARROW=0, I_TOOL_RECT, I_TOOL_ELLIPSE, I_TOOL_POLYGON, I_TOOL_FREEHAND};
enum WHICH_IMAGE	{I_IMAGE_NORMAL=0, I_IMAGE_HIGHLIGHT};

class EDIT_POLY;
class POLY;
class BUTTON;
class REGION;
///////////////////////////////////////////////
//	class REGION -- This class describes a region
//	A region is made of up multiple points
class REGION	
	{
	private:
		REGION_TYPE							m_type;
		COMBINE_MODE						m_combineMode; 
		USHORT									m_usNumPoints;
		POINT										m_aPoints[];

	private:
		REGION(); 								// no default constructor 
		LPVOID operator new (size_t ){I_ASSERT(0);return NULL;} 	// or new operator        

	public:
		LPVOID operator new (size_t ,LPVOID ptr) {return ptr;}
		REGION(REGION_TYPE type, COMBINE_MODE mode, LPPOINT pPoints, USHORT usNumPoints);

		size_t GetSize(void);

		int GetNativeCombineMode(void);
		REGION  * CombineRegion(HRGN  * pHrgnPrev); //return the next REGION

		void OffsetRegion(int dx, int dy);

		void FrameRegion(HDC hdc);

		long Export();
		long Import();


	};
typedef REGION  * LPREGION;


///////////////////////////////////////////////
//	class BUTTON -- This class describes a button
//	A button is made of up multiple regions
class BUTTON
	{
	private:
		char										m_szButtonName[I_BUTTON_NAME_LENGTH];
		BUTTON_TYPES						m_buttonType;
		BOOL										m_bSet;
		HRGN										m_hrgn;
		LONG										m_uidSignal;
		USHORT									m_usNumRegions;
		char										m_aRegions[];

	private:
		LPVOID operator new (size_t){ I_ASSERT(0); return NULL;}	 //no default new operator

	public:
		LPVOID operator new (size_t , size_t sizeComplete);
		LPVOID operator new (size_t , LPVOID pTheMem) { return pTheMem;}
		BUTTON(BUTTON  * pButtonSource);
		BUTTON(LONG uidSignal, TOOL_TYPE toolType, LPPOINT pPoints, USHORT usNumPoints);

		size_t GetSize(void);

		BUTTON_TYPES GetType(void) {return m_buttonType;}
		void SetType(BUTTON_TYPES type) {m_buttonType = type;}

		BOOL GetName(LPSTR szBuffer, USHORT usBufferLen);
		LPCSTR GetName(void) { return m_szButtonName;}
		BOOL SetName(LPCSTR szNewName);
		BOOL IsThisTheName(LPCSTR szCompareName);

		BOOL PointInButton(LPPOINT pPoint);

		void FrameButton(HDC hdc, LPPOINT pPtOffset);
		void InvertButton(HDC hdc, LPPOINT pPtOffset);
		void InvalidateButton(HWND hwnd, BOOL bErase, LPPOINT pPtOffset);

		void OffsetButton(int dx, int dy);

		USHORT NumberOfSignals(void);
		void GetSignals(pAObjSignalRecord pSignals);

		void Clicked(AOBJECTID oiPoly, HWND hwndParent, LPPOINT pPtOffset);
		BOOL Set(AOBJECTID oiPoly, HWND hwndParent, LPPOINT pPtOffset);
		BOOL Clear(AOBJECTID oiPoly, HWND hwndParent, LPPOINT pPtOffset);
		BOOL IsSet(){return m_bSet;}

		BUTTON  *CreateRegionHandle(void);
		BOOL DestroyRegionHandle(void);
		HRGN GetRegionHandle(void) {return m_hrgn;}

		BOOL Configure(USHORT usIndexNumber);

		long Export();
		long Import();

	};
typedef BUTTON  * LPBUTTON;

// 	class used when configuring a button 
class CONFIGBUTTON
	{
	private:
		LPBUTTON								m_pButtonOrig;
		char										m_szButtonName[I_BUTTON_NAME_LENGTH];
		BUTTON_TYPES						m_buttonType;
		USHORT									m_usIndex;

	public:
		CONFIGBUTTON(LPBUTTON bButtonOrig, USHORT usIndex);  

		void Store(void);									 
																			
		BUTTON_TYPES GetType() {return m_buttonType;}
		void SetType(BUTTON_TYPES type) { m_buttonType = type; }
																			 
		LPCSTR GetName(void) { return m_szButtonName;}
		void SetName(LPCSTR szNewName) {lstrcpyn(m_szButtonName, szNewName, I_BUTTON_NAME_LENGTH);m_szButtonName[I_BUTTON_NAME_LENGTH-1] = 0;}
	
		static LRESULT CALLBACK /*!!PORT!! was _far_ _pascal_*/  ButtonConfigureDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	};
typedef CONFIGBUTTON  * LPCONFIGBUTTON;


///////////////////////////////////////////////
//	class POLY -- This class describes a Polybutton
//	A POLY is made of up multiple BUTTONS
//  This is in data #0 
class POLY
	{
	private:
		USHORT									m_version;
		AOBJECTID								m_oiSelf;
		HBITMAP									m_hBmpNormal;
		HBITMAP									m_hBmpHighlight;
		USHORT									m_usBitmapHandleUseCount;
		USHORT									m_usButtonUseCount;
		USHORT									m_usLastButtonClicked;
    unsigned long   				m_uidNext;				//Gets incremented by I_INCREMENT_NEXT_UID (which is 5)
		AMEMBLOCKID							m_miButtonPointers;
		LPBUTTON  *					m_ptrButtons; //array of pointers to futher on down where the buttons are
		USHORT									m_usNumButtons;
		char										m_aButtons[];

	private:
		//called ONLY by the other Destroy/CreateXX methods	and DoAssign and Edit
		BOOL IgnoreCount_CreateBitmapHandles(void);	
		BOOL IgnoreCount_DestroyBitmapHandles(void);
		BOOL IgnoreCount_CreateButtons(void);				
		BOOL IgnoreCount_DestroyButtons(void);

		LPVOID operator new(size_t size){ I_ASSERT(0); return NULL;} // So it can't called 
																								 
	public:
		LPVOID operator new (size_t size, size_t sizeReal, AOBJECTID oiPoly);	//locks the data
		POLY(AOBJECTID oiPoly, LPBUTTON  * ppButtons, USHORT usNumButtons, unsigned long uidNext, BOOL bZeroCounters); //unlocks it

		static LONG DoAssign(AOBJECTID oiDest, AOBJECTID oiSource);
		static LONG Edit(AOBJECTID oiEdit);
		static LONG POLY::Read(AOBJECTID oiPoly);

		size_t GetSize(void);

		long ValidateObjectIDs(AOBJECTID oi){m_oiSelf = oi; return A_OK;}

		LONG BuildSignals(AMEMBLOCKID miSignals);

		BOOL CreateBitmapHandles(void);
		BOOL DestroyBitmapHandles(void);

		BOOL CreateButtons(void);
		BOOL DestroyButtons(void);

		USHORT PointInWhichButton(LPPOINT ptOffset);
		BOOL InvalidateButton(USHORT usIndex, HWND hwnd, BOOL bErase, LPPOINT pPtOffset);
		BOOL FrameButton(USHORT usIndex, HDC hdc, LPPOINT pPtOffset);

		inline HBITMAP GetNormalBitmap(void) {return m_hBmpNormal;}	 
		inline HBITMAP GetHighlightBitmap(void) {return m_hBmpHighlight;}

		inline USHORT GetNumberOfButtons(void) {return m_usNumButtons;}

		BOOL AssignImagesToPictures(AOBJECTID oiNormalPicture, AOBJECTID oiHighlightPicture);
		BOOL AssignPicturesToImages(AOBJECTID oiNormalPicture, AOBJECTID oiHighlightPicture);

		BOOL GetLastClickedButton(USHORT  *pusIndex, LPSTR szName, USHORT usBufferLen);
		BOOL GetButtonIndexByName(USHORT  *pusIndex, LPSTR szName);
		BOOL GetButtonNameByIndex(USHORT usIndex, LPSTR szName, USHORT usBufferLen);
		 
		BUTTON_TYPES GetButtonType(USHORT usIndex);

		HRGN GetButtonRegionHandle(USHORT usIndex);

		BOOL ButtonClicked(USHORT usIndex, HWND hwndParent, LPPOINT pPtOffset);

		BOOL SetButton(USHORT usIndex, HWND hwndParent=NULL, LPPOINT pPtOffset=NULL);
		BOOL SetButton(LPSTR szName);
	
		BOOL ClearButton(USHORT usIndex, HWND hwndParent=NULL, LPPOINT pPtOffset=NULL);
		BOOL ClearButton(LPSTR szName);

		BOOL IsButtonSet(USHORT usIndex, BOOL  *bSet);
		BOOL IsButtonSet(LPSTR szName, BOOL  *pbSet);

		BOOL SetButtonName(USHORT usIndex, LPSTR szNewName);
		BOOL SetButtonName(LPSTR szOldName, LPSTR szNewName);

		void GetImageSize(LPPOINT ptSize);

		static LONG	Import(AOBJECTID oiPoly);
		long Export();

		friend EDIT_POLY;
	};
typedef POLY  * LPPOLY;

/////////////////////////////////////////////////

class EDIT_POLY
	{
	private:
		AOBJECTID							m_oiStore;
		AMEMBLOCKID						m_miNormal;
		AMEMBLOCKID						m_miHighlight;
		HBITMAP								m_hBmpNormal;
		HBITMAP								m_hBmpHighlight;
		HPALETTE							m_hPaletteNormal;
		HPALETTE							m_hPaletteHighlight;
		POINT									m_ptBitmapSize;
    unsigned long   			m_uidNext; 
		USHORT								m_usButtonSelected;  
		USHORT								m_usNumButtons;
		AMEMBLOCKID						m_miButtonsPointer;
		USHORT								m_usNumPoints;
		POINT									m_ptTracking[I_MAX_POINTS_AT_EDIT];
		POINT									m_ptOffset;
		BOOL									m_bTracking;
		TOOL_TYPE							m_currentTool;
		HWND									m_hwndDialog;
		HWND									m_hwndDraw;
		WHICH_IMAGE						m_whichImage;

	public:

		EDIT_POLY(AOBJECTID oiPoly, HWND hwnd);
		~EDIT_POLY();

		BOOL CreateBitmapHandles(void);
		void DeleteBitmapHandles(void);

		HPALETTE GetCurrentPaletteHandle(void) {return (m_whichImage == I_IMAGE_NORMAL ? m_hPaletteNormal:m_hPaletteHighlight);}

		//These MouseXXX methods are used to track the mouse when drawing
		//regions on the picture.
		void MouseDown(LPPOINT ptWindow, WPARAM keysdown);
		void MouseMove(LPPOINT ptWindow, WPARAM keysdown);
		void MouseUp(LPPOINT ptWindow, WPARAM keysdown);
		void MouseDoubleClick(LPPOINT ptWindow, WPARAM keysdown);

		void DrawTrackingPoints(HDC hdc, BOOL bAll);

		USHORT PointInWhichButton(LPPOINT ptWindow);
		USHORT SetCurrentSelectedButton(USHORT us);

		inline TOOL_TYPE GetCurrentTool(void) {return m_currentTool;}
		void SetCurrentTool(TOOL_TYPE tt);

		void InitDialog(void);
		void EnableProperControls(void); //paste and configure buttons
		void Store(void);
		void DialogResized(void);		

		BOOL SaveSizeInIniFile();
		BOOL ReadSizeFromIniFile();
	
		void SetWhichImage(WHICH_IMAGE w);

		//The next two method load/paste into the WHICH_IMAGE (norm/high)
		BOOL Load(void); 
		BOOL Paste(void);

		void SetSizes();

		void WmPaintImage(HDC hdc);		

		void WmVScroll(WORD wScrollCode, int iScrollPos);
		void WmHScroll(WORD wScrollCode, int iScrollPos);

		BOOL ConfigureButton(void) { return ConfigureButton(m_usButtonSelected);}
		BOOL ConfigureButton(USHORT usIndex);
		BOOL OffsetButton(HDC hdc, USHORT usIndex, int dx, int dy);
		BOOL FrameButton(USHORT usIndex, HDC hdc);
		BOOL InvalidateButton(USHORT usIndex, BOOL bErase);

		BOOL CreateNewButton(void); //using current_tool and the points
		BOOL DeleteSelectedButton(void);

		//button use methods. Used to lock, unlock, and resize the array pointers to buttons 
		LPBUTTON  * BeginUsingButtons(void);
		BOOL EndUsingButtons(void);
		BOOL IncrementNumButtons(void);
		USHORT NumButtons(void) { return m_usNumButtons;}
		
		static BOOL RegisterWindowClasses(void);
		static BOOL UnregisterWindowClasses(void);
		static LRESULT CALLBACK /*!!PORT!! was _far_ _pascal_*/  PolybuttonDrawWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK /*!!PORT!! was _far_ _pascal_*/  PolybuttonEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
typedef EDIT_POLY  * LPEDIT_POLY;




//////////////////////////////////////////////////////////////////////////////////////
// This class is what is stored datas #1 and #2 for the normal and highlight images //
// It is the bitmaps that the polybutton uses.                                      //
//////////////////////////////////////////////////////////////////////////////////////
class BMP_DATA
	{
  public:
		BITMAPFILEHEADER			m_bmpFH;
		BITMAPINFOHEADER			m_bmpIH;
		BYTE									m_rgbQuadAndBits[];


	public:
		HBITMAP CreateMemBitmap(HPALETTE hPalette);
		HPALETTE CreatePaletteForBitmap();
		LONG Export(LPSTR szFieldName);
		LONG Import(LPSTR szFieldName, LONG lBmpSize);
	};
typedef BMP_DATA  * LPBMP_DATA;



#endif //DATA_H
