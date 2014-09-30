//////////////////////////////////////////////////////
//File Name : MovData.h
//
//Description : header file for the Movie Dialog Box.
//    
//////////////////////////////////////////

/////////////////////////////////////////
// Data Blocks Defines
/////////////////////////////////////////

#include <windows.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <op_wnd.h>
#include <o_wnd.h>

#include "qtw.h"

////////////////////////////////////////

#define OTYPE_CONTROLLER MAKETYPEID('M','C','t','l')

#define I_UNUSED(x)                ((void)x)

//Controller Dialog Box Definitions
#define CONTROLLER_DIALOG     100
#define PB_OK            IDOK
#define PB_CANCEL        IDCANCEL
#define IDHELP           998
#define PB_HELP          IDHELP   

#define CB_MOVIEASSIGNED       1000
#define CB_SHOWSPEAKER         1001
#define CB_ALLOWSELECTIONS     1002
#define CB_SHOWSTEPPERS        1003
#define CB_DRAWINCOLOR         1004

//Static Controls
#define ST_MCTRLNAME          1005
#define ST_MCTRLRECT          1006
#define CntrlIconID           1007
#define MCTRLDATABLOCKS        2

//Controller Signals
#define sVolumeClicked         0
#define sActionClicked         1
#define sScrollAreaClicked     2
#define sStepperClicked        3

//Controller Data Blocks
#define MCtrlDev  0
#define MCtrlRun  1

#define ID_MYTIMER 1;
/*
#define Speaker  1;
#define PlayArea 2;
#define AreaSel  3;
#define Steppers 4;
*/

typedef struct {
  int         virsion;
  OBJECTID    oiMovie;
  BOOL        bSpeaker;
  BOOL        bStepper;
  BOOL        bSelectable;
  BOOL        bColor;
  BOOL        canActivate;
// Data used at design time
  OBJECTID    oiMovieShared;       //ID of the original Movie Object
  BOOL        bMovieAlias;         //TRUE if assigned Movie is an alias object
} MCTRLDEV, FAR *LPMCTRLDEV;

typedef struct {
  TimeRecord ThumbTime;
  TimeRecord SelStart;
  TimeRecord SelLength;
  BOOL       ShiftWasDown;
  BOOL       Unused;

} MCTRLRUN, FAR *LPMCTRLRUN;

class MCONTROLLERVI : public OpHwndWndItem
{
private:
  HWND                  hPaintWnd;    //Handle to the window in wich the controller is drawn
  HWND                  hMovWnd;      //Handle to the window in wich the movie is drawn
  BOOL                  isActivated;  //if the movie is activated
  MovieController       mController;
  Movie                 mMovie;
  OBJECTID              oiMovie;
  OBJECTID              oiMovieController;
  RECT                  mapRect;
  RECT                  rcMovie;
  RECT                  MCtrlRect;

public:
  MCONTROLLERVI(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status);
  ~MCONTROLLERVI();
  void ALMAPI Destroy();
  void ALMAPI DataChanged(long details, long modifiers, long variation);
  void ALMAPI ParentOpened();
  void ALMAPI ParentClosing();
  long ALMAPI Activate(int causedBy, BOOL mustKeep);
  void ALMAPI Deactivated();

////////////////////////////////////////////////
  MovieController GetMovieController() {return mController;}
  void SetMovieController(MovieController Ctrl) {mController = Ctrl;}
  Movie GetControllerMovie() {return mMovie;}
  void SetControllerMovie(Movie mMov) {mMovie = mMov;}
  OBJECTID GetControllerMovieID() {return oiMovie;}
  void SetControllerMovieID(OBJECTID id) {oiMovie = id;}
};

extern "C"
OpWndItemD* ALMAPI  MCtrlCreate(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );

extern "C"
OpWndItemD* ALMAPI  MCtrlVICreate(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );

#ifdef __cplusplus
extern "C" {
#endif
LONG FAR PASCAL CtrlAdministration(OBJECTID oiMovie, pAObjMessage theSystem) ;
BOOL FAR PASCAL EditCtrlDlgProc (HWND, UINT, UINT, LONG lParam) ;
long FAR PASCAL CtrlBoxProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);
LONG AssignObject(OBJECTID oiDestination, pAObjMessage theSystem);
#ifdef __cplusplus
}
#endif
