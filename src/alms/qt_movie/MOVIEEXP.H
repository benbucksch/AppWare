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

#include <a_alm.h>
#include <op_wnd.h>
#include <o_wnd.h>

#include "qtw.h"

#ifdef __cplusplus
extern "C" {
#endif

LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/ MovieAdministration(AOBJECTID oiMovie, pAObjMessage theSystem) ;
BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ EditMovieDialogProc (HWND, UINT, UINT, LONG lParam) ;
OpWndItemD* ALMAPI  AMovieCreate(
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );

OpWndItemD* ALMAPI  MovieVICreate(
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        special
  );

LONG AssignObject(AOBJECTID oiDestination, pAObjMessage theSystem);
void SetMovie(HWND hMovWnd,AOBJECTID oiMovie,MovieController mController,Movie mMovie);
void DrawMovie(HWND, /*HDC,*/ AOBJECTID, MovieController, Movie, RECT  *,WORD DisplayMode);
void DrawActBorder(HWND, BOOL);
void ShowLogo(HWND hwnd, HDC hdc, AOBJECTID oiMovie, BOOL bShowLogo, RECT *rcClient,WORD DisplayMode);
int GetMovieFromFile(MovieFile mfMovie,AOBJECTID oiMovie);
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/ MovWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);
LONG CALLBACK /*!!PORT!! was _far_ _pascal_*/ MovVIWndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);
void ALMAPI Movie_LoadMovie(pAEvtInfo theEvent, pAObjMessage theSystem);
VOID DrawWhiteRect(HDC hdc,int cxClient,int cyClient);
#ifdef __cplusplus
}
#endif
