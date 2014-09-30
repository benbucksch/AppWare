#include <new.h>

// This structure is used to hold a linked list of unique logfonts in
// the system.
struct SER_FONT {
	LOGFONT logFont;  // logfont structure used by serius environment
	HFONT   hFont;
	int     refCount;
	HANDLE  hMem; // handle for local mem used to store this ser_font
	SER_FONT *next;  // points to next logfont


	};


// This structure is used to hold a linked list of unique logBrushes in
// the system.
struct SER_BRUSH {
	LOGBRUSH logBrush;  // logfont structure used by serius environment
	HBRUSH   hBrush;
	int     refCount;
	HANDLE  hMem; // handle for local mem used to store this ser_brush
	
	SER_BRUSH *next;  // points to next logfont


	};

// this is a base class that defines virtual fucntions to create and delete
// resource handles

class RSRCMGR {
	
  public:
	  void far * operator new(size_t size);

		void operator delete( void far *deadObject,  size_t  t);
	};



// this class is defined to hold a link list of logfonts used in the system
// It defines various operations on the font handles
class FONTMGR:public RSRCMGR{

	SER_FONT *serHdFont; // points to head of the list
	SER_FONT *serFonts;  // points to current serlogFont

  int      numOfFonts;
	int      FontCompare(LOGFONT *lf, int start, int end);
	int      FontHandleCompare(HFONT hFont, int *prevIndex);
	SER_FONT *GetSerFont(int index);

	public:
		FONTMGR();
		~FONTMGR();

	  HFONT  CreateHandle(LOGFONT *lf);
		int    DeleteHandle(HFONT hFont);
		int    NumOfFonts() { return numOfFonts; }
		HFONT  GetFont(int i) { SER_FONT* tmp = GetSerFont(i);
														return tmp->hFont; }


	};


// this class is defined to hold a link list of brushes used in the system
// It defines various operations on the brush handles

class BRUSHMGR:public RSRCMGR {

  SER_BRUSH *serHdBrush;   // hd brush in the list
	SER_BRUSH *serBrushes;   // current brush in the list
	
	int numOfBrushes;
	int BrushCompare(LOGBRUSH *lb);
	int BrushHandleCompare(HBRUSH hBrush, int *prevIndex);
	SER_BRUSH *GetSerBrush(int index);

	public:
	  BRUSHMGR();
    ~BRUSHMGR();

		HBRUSH CreateHandle(LOGBRUSH *lb);
		int    DeleteHandle(HBRUSH hBrush);
		int    NumOfBrushes() { return numOfBrushes; }
		HBRUSH  GetBrush(int i) { SER_BRUSH* tmp = GetSerBrush(i);
														return tmp->hBrush; }



	};


// This is a class that refers to the fontmgr and brushmgr class.
// Serius shell creates an instance of this class at runtime and stores all
// references to its resources in it.

class SER_WINRESOURCES {

	public:
	 
	  void far * operator new(
      size_t size);

	  void operator delete(
      void far *deadObject,
			size_t    t);

	  SER_WINRESOURCES();
		~SER_WINRESOURCES();


	FONTMGR *fontMgr;
	BRUSHMGR *brushMgr;
	};



#ifdef AW_I_DEBUG

extern "C" int ALMAPI oipDeleteAllFonts();
extern "C" int ALMAPI oipCountFonts(HFONT *lf);
extern "C" int ALMAPI oipDeleteAllBrushes();
extern "C" int ALMAPI oipCountBrushes(HBRUSH *lb);

#endif


