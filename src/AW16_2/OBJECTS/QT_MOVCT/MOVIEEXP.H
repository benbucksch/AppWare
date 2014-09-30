/*//////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////*/

#include <windows.h>
#include <qtw.h>
#include <a_alm.h>
#include <op_wnd.h>
#include <o_wnd.h>

#ifdef __cplusplus
extern "C" {
#endif

LONG FAR PASCAL MovieAdministration(OBJECTID oiMovie, pAObjMessage theSystem) ;
BOOL FAR PASCAL EditMovieDialogProc (HWND, UINT, UINT, LONG lParam) ;
OpWndItemD* ALMAPI  AMovieCreate(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );

OpWndItemD* ALMAPI  MovieVICreate(
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );

LONG AssignObject(OBJECTID oiDestination, pAObjMessage theSystem);
void SetMovie(HWND hMovWnd,OBJECTID oiMovie,MovieController mController,Movie mMovie);
void DrawMovie(HWND, /*HDC,*/ OBJECTID, MovieController, Movie, RECT FAR *,WORD DisplayMode);
void DrawActBorder(HWND, BOOL);
void ShowLogo(HWND hwnd, HDC hdc, OBJECTID oiMovie, BOOL bShowLogo, RECT *rcClient,WORD DisplayMode);
int GetMovieFromFile(MovieFile mfMovie,OBJECTID oiMovie);
LONG FAR PASCAL MovWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);
LONG FAR PASCAL MovVIWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);
void ALMAPI Movie_LoadMovie(pAEvtInfo theEvent, pAObjMessage theSystem);
VOID DrawWhiteRect(HDC hdc,int cxClient,int cyClient);
#ifdef __cplusplus
}
#endif
