#include  <a_alm.h>
#include 	<op_wnd.h>
#include 	<o_wnd.h>
#include  <intdebug.h>
#include  "sericon.h"
#include 	"iconedit.h"

#define OTYPE_CURSOR								 AMAKEID('A','c','u','r')
#define OCURS_EVT_SETTHECURSOR       AMAKEID('s','C','u','r')
#define OCURS_EVT_STARTANIMATION     AMAKEID('s','A','n','i')
#define OCURS_EVT_KILLANIMATION  		 AMAKEID('k','A','n','i')


//-------------VERSION INFO -------------------
#define DLLVersion      1
#define DATAVersion     1


#define CURSOROBJDATA   0
#define SERIUSICONSDATA 1
#define DATA1           CURSOROBJDATA
#define TMPDATA         2


#define BITMAP_1    1
#define BITMAP_2    2
#define CURSOR_1    3
#define CURSOR_2    4
#define CURSOR_3    5

#define TYPEPOPUP   1
#define OBJECTPOPUP 2
#define FONTPOPUP   3
#define NULL_ID      -1
#define TIMERID     1
#define NOT_SAVED   -1  // used to indicate that new icon has not been created

#define MaxCursors 200
#define HeaderSize 14

#define CursorStartSignal  0
#define CursorStopSignal 1




typedef HCURSOR * LPHCURSORS;
typedef HANDLE  HSICON;
typedef SERIUSICON * LPHSICONS;


typedef struct CURSOROBJ {
	int version;
  int delayTime;
  int FrameCount;
  int nCurrentFrame;
	HANDLE hCursorIDlist;
	AOBJECTID objectID;
	int timerID;
	BOOL bcursorJustSet;

	ICONEDITOR *iconEditor;
	SERIUSICON seriusIcon;

	HSICON    hSIcons;
	HANDLE  lpHCursors[];
	};

typedef CURSOROBJ  *LPCURSOROBJ;

struct ICONDATA {
	BWICON bwIcon;
	HOTSPOT hotSpot;
	}                 ;

typedef ICONDATA * LPICONDATA;

struct TIMERINFO {
	AOBJECTID objectID;
	int      timerID;

	};
typedef TIMERINFO  *LPTIMERINFO;

// last hidden cursor

