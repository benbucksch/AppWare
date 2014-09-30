#include <windows.h>
#include <bwcc.h>
#include "o_wnd.h"

#define IDD_OBJECT   	600
//#define borderID      601  // id for border bitmap handle
//#define borderID1     602
#define Scale		    	100
#define Obj_Scale     101
#define Obj_Indicator 102
#define Obj_Position	103
#define S_ButtonP     201
#define S_ButtonLF		202
#define I_ButtonP		  203
#define I_ButtonLF		204
//#define Position_Max 	301
//#define Position_Min	302
#define Value_Max		  401
#define Value_Min		  402
#define Value_Val		  403
#define Check_CanDrag	501
#define Check_Stamp		502
#define Check_Synch		503
#define Check_Activate  504

#define MAXREAD  		    32767
#define OBJECTNAMESIZE  32
#define ADDONE          1
#define CURRENTDATAVERSION 0

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define OTYPE_SCAL			MAKETYPEID('S','c','a','l')
#define	SERIUS_HELPFILE						"serhelp.hlp"
#define	HELPID_OBJD_Scal					10000

#define BLOCKDATA 0
#define DIAL      1
#define INDICATOR 2
#define BLOCKNUMS 3
#define CLIPBOARD 0
#define DISKFILE  1

struct DATA0
{
	int      version ;
	OBJECTID min ;
	OBJECTID max ;
	OBJECTID val ;
	BOOL     canDrag ;
	BOOL     stamp ;
	BOOL     wasSetup ;
	BOOL     synchronous ;
	POINT    scalepos ;
	POINT    minpos ;
	POINT    maxpos ;
  BOOL     canActivate ;
} ;

struct SCALEDATA
{
	RECT    rect ;
	HANDLE  hScale ;
  HBITMAP hbkgd ;
} ;

struct INDDATA
{
	RECT    rect ;
	RECT    minRect ;
	RECT    maxRect ;
	HANDLE  hIndicator ;
	HBITMAP hmaxbkgd ;
	HBITMAP hminbkgd ;
} ;
// palette structure ///////////////////////////////////////////////////////

struct  Palette
  {
  WORD          palVersion;
  WORD          palNumEntries;
  PALETTEENTRY  palPalEntry[256];
  };
class PaletteCls 
{
  private:
    static Palette      sm_palette;   // default 256 color palette
  protected:
    static HPALETTE     sm_hpal;      // handle to default palette
  public:
    static inline HPALETTE  GetPaletteHandle()    {return sm_hpal;}
    static void             CreatePalette();
    static void             DestroyPalette();
};
class SCALE
{
	public:
		OBJECTID  oiScale ;
		DATA0*    data0 ;
		SCALEDATA scaleData ;
		INDDATA   indData ;
    HWND      hLayoutW ;
		RECT      layoutRect ;
		BOOL      sync  ;
		int       whichpicked ;
		BOOL      mousedown ;
		POINT     mouse_save ;
		POINT     cursor_pos ;
    int       nVfirstPixel ;
    int       nHfirstPixel ;
    int       nVscrollMax ;
    int       nVscrollMin ;
    int       nVscrollPos ;
    int       nHscrollMax ;
    int       nHscrollMin ;
    int       nHscrollPos ;

		SCALE (OBJECTID) ;
 		~SCALE(void) ;
		BOOL InitScaleParam (HWND) ;
		void SetScaleParam (HWND) ;
		void SetCurrentDisplay(HWND, OBJECTID, int) ;
		void SaveScaleParam (HWND) ;
		long GetObjectID(HWND, int) ;
		void CreateLayoutWindow (HWND) ;
		void GetBitmapHandle(void) ;
		void LoadBitmap(HWND, int, int) ;
		void RepaintBitmap (HWND) ;
		BOOL MouseDown(HWND hwnd, LPARAM lParam) ;
		BOOL MoveBitmap (HWND hwnd, WPARAM wParam, LPARAM lParam) ;
		void MouseMove (HWND hwnd, LONG lParam, LPRECT lprect) ;
		BOOL MouseUp(HWND hwnd) ;
		HANDLE BitmapToDIB (HBITMAP hBitmap) ;
		BOOL CanActivate (void){return (data0->canActivate);};
   	OBJECTID GetObject (int) ;
		void SetPosition (void) ; //OBJECTID, int) ;
    void GetScrollRange (RECT, RECT, RECT) ;
    void ScaleSetScrollPos (HWND) ;
    void DoVscroll (HWND, WPARAM, LPARAM) ;
    void DoHscroll (HWND, WPARAM, LPARAM) ;
		friend int FAR PASCAL _export ScaleObjectDlg(HWND, WORD, WORD, LONG) ;
} ;
long far PASCAL _export LayoutWndProc (HWND, UINT, WPARAM, LPARAM) ;
class VI_SCALE:public OpLiteWndItem {
	public:
	SCALE         *scale;
  RECT          VRect ;
  RECT          sRect ;
  RECT          minRect ;
  RECT          maxRect ;
  POINT         minpos ;
  POINT         maxpos ;
  POINT         scalepos ;
	double long   max ;
	double long   min ;
	double long   val ;
	long          Width ;
	long          Height ;
  BOOL          KeyHit ;
  HBITMAP       hBackground ;
  HBITMAP       hMaskBitmap ;
  HBITMAP       hInvertBitmap ;

	VI_SCALE (OBJECTID oid,OBJECTID parentID,RECT* rcItem, long statusFlag);
 ~VI_SCALE (void);

	void ALMAPI    MouseDown(POINT *where,int       keysDown,int            button);
	void ALMAPI    MouseUp(POINT           *where,int       keysDown,int            button);
	void ALMAPI    MouseMove(POINT *where,int      keysDown);
	void ALMAPI    Destroy(void);
	long ALMAPI    Activate(int causedBy, BOOL mustKeep);
	void ALMAPI    Deactivated(void);
	void ALMAPI    Draw(HDC hItemDC, RECT* invalidRect);
	void ALMAPI    DataChanged(long details,long modifiers,	long variation) ;
	void ALMAPI    KeyDown(int vKey,char ascii,long keyData);
	long ALMAPI    AdjustNewRect(LPRECT lpRect, LONG flag);
	void ALMAPI    Move(LPRECT lpRect) ;
  void SetDefaultRectValue (void) ;
	void DrawActiveItem(HDC) ;//id) ;
	void ValidateValues (void) ;
	double long PositionToValue (POINT thePosition ) ;
	POINT ValueToPosition (double long val) ;
	void RepaintScale(HDC, BOOL) ;
	void RedrawIndicator(HWND, RECT) ;
	void PaintIntersect (HDC hdc, RECT newRect, RECT rect) ;
	void PaintIndicator (HDC hdc, RECT rect) ;
	void SaveBackground(HDC hdc, RECT ) ;
	//void RestoreBackground (HDC hdc, RECT) ;
} ;

extern int _WinAllocFlag ;
extern HINSTANCE hInstance ;
extern HBITMAP   hBORDER ;

extern "C"
{
	long far pascal _export ScaleAdministration (OBJECTID , pAObjMessage) ;
	void far pascal _export Scale_objectChanged(pAEvtInfo, pAObjMessage) ;
	void far pascal _export Scale_startupEvent(pAEvtInfo, pAObjMessage) ;
}

extern WORD DIBNumColors (LPSTR) ;
extern WORD PaletteSize (LPSTR) ;
extern LPSTR FindDIBBits (LPSTR) ;
extern void InitBitmapInfoHeader (LPBITMAPINFOHEADER, DWORD, DWORD, int) ;
extern HANDLE BitmapToDIB (HBITMAP hBitmap, LPPOINT) ;
extern HANDLE LoadBitmapFromClipboard(HWND hwnd, HANDLE, LPPOINT);
extern BOOL OpenDlg (HWND, char * fileName) ;
extern HANDLE LoadBitmapFromFile(HWND, HANDLE, LPPOINT) ;
extern HANDLE ReadDIB(char*, LPPOINT) ;
extern void PaintDIBitmap(HDC hdc, HANDLE hDIB, LPRECT) ;
extern DWORD PASCAL lread (int fh, VOID far *pv, DWORD ul) ;
extern HANDLE ObjectToDIBitmap (OBJECTID oiObj, LPRECT lprect, int ScaleOrInd) ;
extern void DIBitmapToObject (OBJECTID oiObj, HANDLE hDIB, int ScaleOrInd) ;
extern BOOL CheckInBitmap (RECT rect, POINT mousepos, LPPOINT mouseInBitmap) ;
extern void DrawFrame(HWND hwnd, RECT frameRect, RECT) ;
extern void DrawIndText (HDC, RECT, RECT iRect,int status) ;
extern BOOL CorrectRect(LPRECT newRect, LPRECT layRect);
extern double long GetValue (OBJECTID theObject) ;
extern void SetValue (OBJECTID numObject, long double theValue) ;
extern HBITMAP CreateInvertBitmap(HANDLE, HBITMAP) ;
extern HBITMAP CreateMaskBitmap (HANDLE) ;
extern void DoTransparent (HDC hdc, HBITMAP hMaskBitmap, HBITMAP hInvertBitmap, LPRECT lpRect) ;