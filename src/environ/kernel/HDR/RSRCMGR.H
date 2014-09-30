#ifndef RSRCMGR_H
#define RSRCMGR_H

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
		short    DeleteHandle(HFONT hFont);
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
		short  DeleteHandle(HBRUSH hBrush);
		int    NumOfBrushes() { return numOfBrushes; }
		HBRUSH  GetBrush(int i) { SER_BRUSH* tmp = GetSerBrush(i);
														return tmp->hBrush; }



	};


// This is a class that refers to the fontmgr and brushmgr class.
// Serius shell creates an instance of this class at runtime and stores all
// references to its resources in it.

class SER_WINRESOURCES {

	public:
	 
	  SER_WINRESOURCES();
		~SER_WINRESOURCES();


	FONTMGR *fontMgr;
	BRUSHMGR *brushMgr;
	};



#endif
