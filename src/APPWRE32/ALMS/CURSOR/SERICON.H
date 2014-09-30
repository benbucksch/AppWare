#ifndef __SERICON_H__
#define __SERICON_H__


#define MASKARRAYSIZE	(32*32/8)
#define IMAGEARRAYSIZE (32*32/2)



struct CURSORENTRY {

  BYTE curWidth;
  BYTE curHeight;
  WORD ColorCount;
  WORD wBitsPerPel;
  DWORD dwBytesInRes;
  WORD wOrdinalNumber;

  };


struct CURSORDIRECTORY {
  WORD curReserved ;
  WORD curResourceType;
  WORD curResourceCount;
  CURSORENTRY  *CursorEntry;

  };

struct HOTSPOT {
  int  xHotSpot;
  int  yHotSpot;


  };
struct CURSORIMAGE {

  HOTSPOT 									HotSpot;
  BITMAPINFOHEADER 					dibInfo;
  RGBQUAD 									imageColors[2];
  BYTE    									bXorMask[MASKARRAYSIZE];
  BYTE    									bAndMask[MASKARRAYSIZE];
  };


struct BWCURSOR {

	CURSORDIRECTORY         cursorHdr;
	CURSORIMAGE         		cursorImage;
	};


typedef  BWCURSOR * 		LPBWCURSOR;
typedef  CURSORENTRY * LPRESOURCEDIR;

struct COLORICON
	{
	BITMAPINFOHEADER				imageHdr;						//these two fields must remain
	RGBQUAD									imageColors[16];    //adjacent and in this order
	BYTE										imageArray[IMAGEARRAYSIZE];
	BITMAPINFOHEADER				maskHdr;						//these two fields must remain
	RGBQUAD									maskColors[2];    	//adjacent and in this order
	BYTE										maskArray[MASKARRAYSIZE];
	};

typedef COLORICON * 	LPCOLORICON;

struct BWICON
	{
	BITMAPINFOHEADER				imageHdr;						//these two fields must remain
	RGBQUAD									imageColors[2];    	//adjacent and in this order
	BYTE										imageArray[MASKARRAYSIZE];
	BITMAPINFOHEADER				maskHdr;						//these two fields must remain
	RGBQUAD									maskColors[2];    	//adjacent and in this order
	BYTE    								maskArray[MASKARRAYSIZE];
	};

typedef BWICON *				LPBWICON;

class SERIUSICON
	{
private:

	void InitPixels(
		int  			x,
		int  			y);

public:

	COLORICON								colorIcon;
	BWICON									bwIcon;

	HANDLE                  hrsrcDir;

	BWCURSOR                bwCursor;

	SERIUSICON(
		void);

	SERIUSICON::SERIUSICON(
		HANDLE	fileHandle,
		short	colorCount);

	SERIUSICON(
		void *	objPtr)
		{
		}

	void DrawColor(
		HDC	hDC,
		int	x,
		int y);

	void DrawBW(
		HDC	hDC,
		int	x,
		int y);

	};

typedef SERIUSICON *	LPSERIUSICON;

#endif
