////////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
//  Written by DUNYE ALEXANDER, ECO High Tech Group, March 1994
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//File Name : MovData.h
//
//Description : header file for the Movie Dialog Box.
//    
//////////////////////////////////////////

/////////////////////////////////////////
// Data Blocks Defines
/////////////////////////////////////////

#include <stdio.h>
#include <fcntl.h>
#include <windows.h>

#include <a_alm.h>
#include <op_wnd.h>
#include <o_wnd.h>

#include "qtw.h"
#include "moviedlg.h"

#define MovieDataVersion    1  //december-february 1993/1994
#define NumOfDataBlocks     7  //seven data blocks
#define MovieDev            0
#define MovieRun            1
#define MovieSet            2
#define MovieMooV           3
#define MoviePICT           4
#define MovieFrame          5
#define MovieCallBack       6

////////////////////////////////////////
//Type specific data
#define MINIMUM_VI      10;

/////////////////////////////////////////////////////////
//Object Changed event messages. Set in details field
/////////////////////////////////////////////////////////
#define EvtDefault              0
#define EvtDisplayMovie         1
#define EvtRefocusMovie         2
#define EvtDrawPoster           3
#define EvtDrawMovie            4
#define EvtDrawActBorder        10
#define EvtDrawBadge            11
#define EvtDrawBorder           12
#define EvtDrawLogo             13
#define EvtSetScale             14
#define EvtSetScroll            15
//Movie function category
#define EvtLoadMovie            16
#define EvtPlayMovie            17
#define EvtPlayPreview          18
#define EvtPrerollMovie         19
#define EvtShowPoster           20
#define EvtStopMovie            21
//Movie Edit function category
#define EvtClearSelection       22
#define EvtSetSelection         23
#define EvtSetSelectionTime     24
//MovieMetrics function category
#define EvtGoToTime             40
#define EvtSetLoopValue         41

//MovieController function category
#define EvtCtrlHideSpeaker      50
#define EvtCtrlShowSpeaker      51
#define EvtCtrlHideSteppers     52
#define EvtCtrlShowSteppers     53

#define EvtMuteSound            99
/////////////////////////////////////////

/////////////////////////////////////////
// Signals
/////////////////////////////////////////
#define sChanged         0
#define sDone            1
#define sSingleClick     2
#define sDoubleClick     3
#define sStarted         4
#define sStopped         5
#define sViewChanged     6
#define sActivated       7
#define sDeactivated     8
////////////////////////////////////////

#define OTYPE_MOVIE             ATYPE_MAKEID ('M','o','o','V')
#define AEVT_PLAYMOVIE          AEVENT_MAKEID('m','P','l','a')
#define AEVT_STOPMOVIE          AEVENT_MAKEID('m','S','t','p')
#define AEVT_STARTMOVIE         AEVENT_MAKEID('m','S','t','t')
#define AEVT_OBJECTTOBITMAP     AEVENT_MAKEID('o','t','o','B')
#define AEVT_BITMAPTOOBJECT     AEVENT_MAKEID('b','t','o','O')

#define WM_SETMOVIE             (WM_USER + 1)

#define I_UNUSED(x)             ((void)x)

#define report(where,what,format,num1,num2) \
{\
  FILE *rpt = fopen(where,"a+t");\
  wsprintf(what,format,num1,num2);\
  fwrite(what,sizeof(what),1,rpt);\
  fclose(rpt);\
}
     
//Movie Display Mode
//Not used now. I use Control IDs for display mode. see in moviedlg.h
#define CENTERED   1
#define SIZED      2
#define SCALED     3
#define CROPPED    4

typedef struct {
  long              version;
  MovieFile         mfMovie;       //Movie File contains movie of the object
  char              mfMovieName[255];
  MovieController   mController;//Movie Controller of the Movie Object at Runtime
  AOBJECTID          oiController;
  WORD              DisplayMode;
  BOOL              bShowBorder;
  BOOL              bShowBadge;
  BOOL              bShowActBorder;
  BOOL              bEditable;
  BOOL              bShowLogo;
  BOOL              bAutoStop;
  BOOL              bShowController;
  RECT              rcBorder;
  RECT              rcActBorder;
  /////////////////////////////////////////////////////////////////////////
  //This data is used for design time only
  AOBJECTID          oiSelf;             //ID of the Movie object itself
  BOOL              bShared;            //TRUE if the Object is shared
} MOVIEDEVREC,  *MOVIEDEVPTR;

typedef struct {
  //Movie Runtime Storage
  Movie             mMovie;       //Movie of the Object is contained in the Movie File
  PicHandle         Picture;
  BOOL              bIsMovieDone;
  HANDLE            LastInstance;
  //Object Runtime Storage
  HWND              ActiveInstance;   //Handle to window in wich movie is to be drawn
  BOOL              bIsMoviePlaying;
  TimeValue         LastClickTime;
  LFIXED            lfxRate;
  SFIXED            sfxVolume;
  TimeRecord        trSelBegin;
  TimeRecord        trSelDuration;
  long              LoopValue;
} MOVIERUNREC,  *MOVIERUNPTR;

typedef struct {
  Movie             mMovie;
  MovieFile         mfMovie;
  char              mfMovieName[255];
  MovieController   mController;
  BOOL              bSaved;
  BOOL              bEdit;
} MOVIESETREC,  *LPMOVIESETREC;

typedef struct {
  RECT          SetMovieRect;
  RECT          RunMovieRect;
  RECT          Frame;
  RECT          ActFrame;
  int           HScroll,VScroll;
  double        Scale;
} MOVIEFRAMEREC,  *LPMOVIEFRAMEREC;

class MOVIEVI : public OpHwndWndItem
{
private:
  HWND              hPaintWnd;    // Handle to the window in wich the movie is drawn
  BOOL              isActivated;  // if the movie is activated
  MovieController   mController;
  Movie             mMovie;       // Movie is placed in Visual instance of the Object      
  RECT              mapRect;
  RECT              MovWndRect;
  AMEMBLOCKID        miData;

public:
  MOVIEVI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status);

  void ALMAPI Destroy();
  void ALMAPI DataChanged(long details, long modifiers, long variation);
  void ALMAPI ParentOpened();
  void ALMAPI ParentClosing();
  void Draw(HDC hdc, RECT *invalidRect);
  long ALMAPI Activate(int causedBy, BOOL mustKeep);
  void ALMAPI Deactivated();

  void SetMovieID(AOBJECTID theObject) { m_oiObject = theObject; return;}
  AOBJECTID GetMovieID() { return m_oiObject;}
  void SetMovieController(MovieController MCtrl) { mController = MCtrl; return;}
  MovieController GetMovieController() { return mController;}
  void SetMovie(Movie mMov) {mMovie = mMov; return;}
  Movie GetMovie() {return mMovie;}
  void SetMovieWnd(HWND hwnd) {hPaintWnd = hwnd; return;}
  HWND GetMovieWnd() { return hPaintWnd;}
  void SetActivated(BOOL val) {isActivated = val; return;}
  BOOL IsActivated() {return isActivated;}
  void DrawActBorder(BOOL bShowActBorder);
  void DrawBadge(BOOL bShowBadge);
  void DrawBorder(BOOL bShowBorder);
  void DrawLogo(BOOL bShowLogo);
  void MuteSound(void);
  void SetMovieToController(void);

//Movie function category
  OSErr LoadMovie(const char *Path);
  void PlayMovie();
  void StopMovie();
  void PrerollMovie();
  void PlayPreview();
  void ShowMoviePoster();
  
//Movie Edit function category
  void ClearSelection();
  void SetSelDuration();
  void SetSelectionTime();

//Movie Metrics function category
  void GoToTime();
  void SetLoopValue();

//MovieController methods
  void ShowSpeaker(long details);
  void ShowSteppers(long details);
};

extern "C"
OpWndItemD* ALMAPI  MovieVICreate(
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );
  
