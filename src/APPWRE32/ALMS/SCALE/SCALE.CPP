#define 	STRICT
#define  	ASTRICT

#include <windows.h>
#include <windowsx.h>
#include <commDlg.h>
#include <mem.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_number.h>
#include <intutl.h>
#include "scale.h"

Palette PaletteCls::sm_palette =
  { 
  0x0300,
  256,
  {{0xFF, 0xFF, 0xFF, 0}, {0x00, 0x00, 0x00, 0}, {0x77, 0x77, 0x77, 0}, {0xFF, 0x00, 0x00, 0},
   {0x00, 0xFF, 0x00, 0}, {0x00, 0x00, 0xFF, 0}, {0xFF, 0xFF, 0x00, 0}, {0xFF, 0x00, 0xFF, 0},
   {0x00, 0xFF, 0xFF, 0}, {0xBB, 0xBB, 0xBB, 0}, {0xFF, 0x99, 0x00, 0}, {0x66, 0x99, 0x00, 0},
   {0x00, 0x66, 0xFF, 0}, {0xFF, 0xFF, 0x99, 0}, {0xFF, 0x99, 0xFF, 0}, {0x66, 0xFF, 0x99, 0},
   {0x22, 0x22, 0x22, 0}, {0xCC, 0x00, 0x00, 0}, {0x66, 0xCC, 0x00, 0}, {0x66, 0xCC, 0xFF, 0},
   {0xFF, 0xCC, 0x66, 0}, {0xCC, 0x99, 0xFF, 0}, {0xCC, 0xFF, 0xFF, 0}, {0x88, 0x88, 0x88, 0},
   {0xFF, 0x66, 0x33, 0}, {0x00, 0xCC, 0x33, 0}, {0x00, 0x00, 0x77, 0}, {0xFF, 0xFF, 0x66, 0},
   {0xFF, 0x99, 0xCC, 0}, {0x99, 0xFF, 0xCC, 0}, {0x44, 0x44, 0x44, 0}, {0xFF, 0x33, 0x00, 0},
   {0x99, 0xFF, 0x33, 0}, {0x66, 0x99, 0xFF, 0}, {0xFF, 0xCC, 0x00, 0}, {0xCC, 0x00, 0x99, 0},
   {0x99, 0xFF, 0xFF, 0}, {0xAA, 0xAA, 0xAA, 0}, {0xCC, 0x66, 0x00, 0}, {0x66, 0x99, 0x33, 0},
   {0xCC, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0x00, 0}, {0xFF, 0xCC, 0xFF, 0}, {0x99, 0x99, 0xFF, 0},
   {0x55, 0x55, 0x55, 0}, {0xDD, 0x00, 0x00, 0}, {0x00, 0xDD, 0x00, 0}, {0x00, 0x00, 0xBB, 0},
   {0xFF, 0xFF, 0x33, 0}, {0xFF, 0xCC, 0xCC, 0}, {0x33, 0xFF, 0xCC, 0}, {0xEE, 0xEE, 0xEE, 0},
   {0x88, 0x00, 0x00, 0}, {0x00, 0x55, 0x00, 0}, {0x00, 0x00, 0x55, 0}, {0xCC, 0xCC, 0x00, 0},
   {0xCC, 0x33, 0xCC, 0}, {0x33, 0xFF, 0xFF, 0}, {0x11, 0x11, 0x11, 0}, {0xFF, 0x33, 0x66, 0},
   {0x33, 0xCC, 0x33, 0}, {0x00, 0x66, 0xCC, 0}, {0xFF, 0xCC, 0x33, 0}, {0x99, 0x00, 0x66, 0},
   {0x66, 0x99, 0xCC, 0}, {0xDD, 0xDD, 0xDD, 0}, {0xAA, 0x00, 0x00, 0}, {0x00, 0x77, 0x00, 0},
   {0x00, 0x00, 0x88, 0}, {0xCC, 0xFF, 0x33, 0}, {0xCC, 0x00, 0xCC, 0}, {0x99, 0xCC, 0xFF, 0},
   {0xCC, 0xCC, 0xCC, 0}, {0xFF, 0x33, 0x33, 0}, {0x00, 0xCC, 0x00, 0}, {0x33, 0x33, 0xFF, 0},
   {0xCC, 0xCC, 0x33, 0}, {0xFF, 0x00, 0xCC, 0}, {0x33, 0x99, 0xCC, 0}, {0x99, 0x99, 0x99, 0},
   {0xFF, 0x66, 0x00, 0}, {0x66, 0xFF, 0x00, 0}, {0x33, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0x66, 0},
   {0x66, 0x66, 0x99, 0}, {0x66, 0xCC, 0xCC, 0}, {0x66, 0x66, 0x66, 0}, {0x44, 0x00, 0x00, 0},
   {0x00, 0x22, 0x00, 0}, {0x00, 0x00, 0x33, 0}, {0x99, 0x99, 0x00, 0}, {0x99, 0x66, 0x99, 0},
   {0x66, 0x66, 0xCC, 0}, {0x99, 0xCC, 0xCC, 0}, {0xEE, 0x00, 0x00, 0}, {0x00, 0xEE, 0x00, 0},
   {0x00, 0x00, 0xEE, 0}, {0xFF, 0xFF, 0xCC, 0}, {0xFF, 0x33, 0xFF, 0}, {0x00, 0xFF, 0xCC, 0},
   {0x99, 0x99, 0xCC, 0}, {0xFF, 0x00, 0x99, 0}, {0x00, 0x88, 0x00, 0}, {0x66, 0x33, 0xFF, 0},
   {0xCC, 0x99, 0x33, 0}, {0xCC, 0x33, 0xFF, 0}, {0x66, 0xFF, 0xCC, 0}, {0x33, 0x33, 0x33, 0},
   {0xBB, 0x00, 0x00, 0}, {0x00, 0x66, 0x00, 0}, {0x00, 0x00, 0x44, 0}, {0xCC, 0x99, 0x66, 0},
   {0xFF, 0x66, 0x99, 0}, {0x33, 0xCC, 0xFF, 0}, {0x99, 0x99, 0x66, 0}, {0xFF, 0x00, 0x33, 0},
   {0x99, 0xCC, 0x00, 0}, {0x00, 0x00, 0xAA, 0}, {0xCC, 0xFF, 0x99, 0}, {0xCC, 0x33, 0x99, 0},
   {0x66, 0x66, 0xFF, 0}, {0x99, 0x66, 0x66, 0}, {0x55, 0x00, 0x00, 0}, {0x00, 0x11, 0x00, 0},
   {0x33, 0x00, 0x66, 0}, {0xFF, 0xCC, 0x99, 0}, {0xCC, 0x66, 0x99, 0}, {0x00, 0xFF, 0x99, 0},
   {0xCC, 0x99, 0xCC, 0}, {0xFF, 0x00, 0x66, 0}, {0x33, 0xCC, 0x00, 0}, {0x00, 0x00, 0xCC, 0},
   {0x99, 0xFF, 0x00, 0}, {0x99, 0x33, 0x66, 0}, {0x00, 0xCC, 0xFF, 0}, {0x66, 0x99, 0x99, 0},
   {0xCC, 0x00, 0x33, 0}, {0x00, 0x99, 0x00, 0}, {0x66, 0x00, 0xFF, 0}, {0xCC, 0xFF, 0xCC, 0},
   {0xCC, 0x33, 0x66, 0}, {0x99, 0xFF, 0x99, 0}, {0xCC, 0xCC, 0xFF, 0}, {0xCC, 0x33, 0x00, 0},
   {0x00, 0xFF, 0x33, 0}, {0x00, 0x33, 0xFF, 0}, {0xFF, 0x99, 0x66, 0}, {0xCC, 0x00, 0x66, 0},
   {0x66, 0xFF, 0xFF, 0}, {0x33, 0x66, 0x66, 0}, {0x99, 0x00, 0x00, 0}, {0x99, 0xFF, 0x66, 0},
   {0x33, 0x99, 0xFF, 0}, {0xCC, 0xCC, 0x66, 0}, {0xFF, 0x33, 0xCC, 0}, {0x00, 0x99, 0xCC, 0},
   {0xCC, 0x99, 0x99, 0}, {0x77, 0x00, 0x00, 0}, {0x00, 0x33, 0x00, 0}, {0x33, 0x00, 0xCC, 0},
   {0xCC, 0x99, 0x00, 0}, {0xCC, 0x66, 0xCC, 0}, {0x00, 0x99, 0xFF, 0}, {0x66, 0x33, 0x33, 0},
   {0xCC, 0x33, 0x33, 0}, {0x33, 0x66, 0x00, 0}, {0x00, 0x00, 0x22, 0}, {0xCC, 0xCC, 0x99, 0},
   {0xCC, 0x66, 0xFF, 0}, {0x00, 0xCC, 0xCC, 0}, {0x66, 0x66, 0x33, 0}, {0x22, 0x00, 0x00, 0},
   {0x33, 0xFF, 0x00, 0}, {0x00, 0x33, 0x99, 0}, {0x99, 0x66, 0x00, 0}, {0xFF, 0x66, 0xFF, 0},
   {0x33, 0xCC, 0xCC, 0}, {0x99, 0xCC, 0x99, 0}, {0xFF, 0x33, 0x99, 0}, {0x33, 0xFF, 0x66, 0},
   {0x00, 0x00, 0x11, 0}, {0xFF, 0x99, 0x33, 0}, {0x99, 0x33, 0x99, 0}, {0x66, 0xCC, 0x99, 0},
   {0x99, 0xCC, 0x66, 0}, {0x99, 0x00, 0x33, 0}, {0x00, 0x44, 0x00, 0}, {0x33, 0x66, 0xFF, 0},
   {0x99, 0x99, 0x33, 0}, {0xFF, 0x66, 0xCC, 0}, {0x33, 0xFF, 0x99, 0}, {0x33, 0x99, 0x99, 0},
   {0x33, 0x00, 0x00, 0}, {0x00, 0xFF, 0x66, 0}, {0x33, 0x00, 0x99, 0}, {0x99, 0x66, 0x33, 0},
   {0x99, 0x33, 0xCC, 0}, {0x33, 0xCC, 0x99, 0}, {0x99, 0x66, 0xCC, 0}, {0x99, 0x33, 0x00, 0},
   {0x00, 0x99, 0x33, 0}, {0x00, 0x00, 0x99, 0}, {0xCC, 0x66, 0x33, 0}, {0x99, 0x00, 0x99, 0},
   {0x33, 0x66, 0xCC, 0}, {0x33, 0x33, 0x66, 0}, {0xFF, 0x66, 0x66, 0}, {0x00, 0x66, 0x33, 0},
   {0x66, 0x00, 0xCC, 0}, {0x66, 0xFF, 0x33, 0}, {0xFF, 0x99, 0x99, 0}, {0x00, 0x66, 0x66, 0},
   {0x33, 0x99, 0x00, 0}, {0x33, 0x33, 0x99, 0}, {0x66, 0xFF, 0x66, 0}, {0x33, 0x00, 0x33, 0},
   {0xCC, 0x66, 0x66, 0}, {0x00, 0xAA, 0x00, 0}, {0x00, 0x33, 0xCC, 0}, {0x66, 0x33, 0x00, 0},
   {0x33, 0x99, 0x66, 0}, {0x66, 0x00, 0x00, 0}, {0x00, 0xBB, 0x00, 0}, {0x66, 0x00, 0x99, 0},
   {0x33, 0xFF, 0x33, 0}, {0x66, 0x33, 0xCC, 0}, {0x66, 0x00, 0x33, 0}, {0x33, 0x33, 0x00, 0},
   {0x99, 0x00, 0xCC, 0}, {0x00, 0x99, 0x66, 0}, {0x00, 0x00, 0x66, 0}, {0x66, 0xCC, 0x66, 0},
   {0x66, 0x00, 0x66, 0}, {0x99, 0x66, 0xFF, 0}, {0x33, 0x33, 0xCC, 0}, {0x00, 0x99, 0x99, 0},
   {0x99, 0x33, 0x33, 0}, {0x00, 0x00, 0xDD, 0}, {0x99, 0xCC, 0x33, 0}, {0x66, 0x33, 0x66, 0},
   {0x11, 0x00, 0x00, 0}, {0x33, 0x66, 0x99, 0}, {0x66, 0x66, 0x00, 0}, {0x99, 0x00, 0xFF, 0},
   {0x00, 0x33, 0x33, 0}, {0x33, 0x99, 0x33, 0}, {0x99, 0x33, 0xFF, 0}, {0x00, 0xCC, 0x99, 0},
   {0x00, 0x33, 0x66, 0}, {0x66, 0xCC, 0x33, 0}, {0x66, 0x99, 0x66, 0}, {0x66, 0x33, 0x99, 0},
   {0x33, 0xCC, 0x66, 0}, {0x33, 0x66, 0x33, 0}, {0x00, 0xCC, 0x66, 0}, {0x00, 0x66, 0x99, 0}}
  };

HPALETTE PaletteCls::sm_hpal = 0;
////////////////////////////////////////////////////////////////////////////
//
// Method:      PaletteCls::CreatePalette  (static)
//
// Description: This function will create the default palette for the window
//              object.
//
////////////////////////////////////////////////////////////////////////////

void PaletteCls::CreatePalette
  (
  )

  {
  HDC hdc         = CreateDC("DISPLAY", NULL, NULL, NULL);
  int nPalSize    = GetDeviceCaps(hdc, SIZEPALETTE);

  sm_hpal = (nPalSize==256) ? ::CreatePalette((LOGPALETTE*)&sm_palette) : NULL;

  DeleteDC(hdc);
  }

////////////////////////////////////////////////////////////////////////////
//
// Method:      PaletteCls::DestroyPalette  (static)
//
// Description: This function will create the default palette for the window
//              object.
//
////////////////////////////////////////////////////////////////////////////

void PaletteCls::DestroyPalette
  (
  )

  {
  if(sm_hpal)
    {
    DeleteObject(sm_hpal);
    sm_hpal = NULL;
    }
  }

////////////////////////////////////////////////////////////////////////////

BOOL InitLayoutClass (LPSTR lpszClassName)
{
	WNDCLASS wndclass ;

	wndclass.style		     = 0 ;
	wndclass.lpfnWndProc   = (WNDPROC) LayoutWndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = (HINSTANCE)hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = lpszClassName ;

	return RegisterClass(&wndclass) ;
	}
//
//
//
SCALE::SCALE (
	AOBJECTID oiObj)
{
	oiScale = oiObj ;

	if(!oiScale || AObjGetType(oiScale)!=OTYPE_SCAL) return;

	if (AObjGetDataCount(oiObj)<BLOCKNUMS)
		{
    AObjSetDataCount(oiObj, BLOCKNUMS);
		}
	data0	= (DATA0*)AObjLockData(oiObj, BLOCKDATA);
	if (!data0)
		{
		if (AObjResizeData(oiObj, BLOCKDATA, sizeof(DATA0))&&
				(data0 = (DATA0*)AObjLockData(oiObj, BLOCKDATA)) != NULL)
			{
			data0->version     = CURRENTDATAVERSION ;
			data0->min         = NULL ;
			data0->val         = NULL ;
			data0->max         = NULL ;
			data0->minpos.x    = 0  ;
			data0->minpos.y    = 0  ;
			data0->maxpos.x    = 0  ;
			data0->maxpos.y    = 0  ;
			data0->canDrag     = TRUE ;
			data0->stamp       = TRUE;
			data0->wasSetup    = TRUE ;
      data0->canActivate = TRUE ;
			data0->synchronous = FALSE ;
			data0->scalepos.x  = 0 ;
			data0->scalepos.y  = 0 ;
			}
		}
}
SCALE::~SCALE (void)
{
	if (data0)
		AObjUnlockData(oiScale, BLOCKDATA);

	if (scaleData.hScale)
		GlobalFree (scaleData.hScale) ;
	if (indData.hIndicator)
		GlobalFree (indData.hIndicator) ;
}
//
//
//
int CALLBACK /*!!PORT!! was _far_ _pascal_*/  ScaleObjectDlg (
	HWND hDlg,
	WORD message,
	WORD wParam,
	LONG lParam)
{
	//AOBJECTID     oiObj ;
	SCALE*        scale ;

	if (message != WM_INITDIALOG)
		{
		scale = (SCALE*) GetWindowLong(hDlg, DWL_USER); 
		}

	switch (message)
		{
		case WM_INITDIALOG:
			SetWindowLong(hDlg, DWL_USER,  lParam) ; 
			scale = (SCALE*) lParam ;
		 	scale->InitScaleParam(hDlg) ;
			return TRUE;

		case WM_COMMAND: //!!PORT!!
      {
			switch (LOWORD(wParam))
				{
				case S_ButtonP:
					scale->LoadBitmap(hDlg, CLIPBOARD, DIAL) ;
					break;

				case S_ButtonLF:
        	scale->LoadBitmap (hDlg, DISKFILE, DIAL) ;
					break;

				case I_ButtonP:
					scale->LoadBitmap (hDlg, CLIPBOARD, INDICATOR) ;
					break;

				case I_ButtonLF:
					scale->LoadBitmap (hDlg, DISKFILE, INDICATOR) ;
					break;

				case IDOK:
					scale->SaveScaleParam(hDlg) ;
					EndDialog(hDlg, TRUE) ;
					break;

				case IDCANCEL:
					EndDialog(hDlg, FALSE) ;
					break;

			 	}
				break;
			}
		default:
			break;
		}
	return FALSE;
	}
BOOL SCALE::InitScaleParam (HWND hwnd)
{
	char buffer[OBJECTNAMESIZE] ;

	AObjGetName(oiScale, buffer, OBJECTNAMESIZE) ;
	SetWindowText(hwnd, buffer) ;
	AUtlSetTitleFont(OTYPE_SCAL, NULL, GetDlgItem(hwnd, IDD_OBJECT));
	AUtlCenterDialog(hwnd, 0);

	GetBitmapHandle () ;
	mousedown = FALSE ;
	nVscrollPos = nHscrollPos = 0 ;
	nVfirstPixel = 0 ;
	nHfirstPixel = 0 ;

	SetScaleParam(hwnd) ;
	CreateLayoutWindow (hwnd) ;
	return FALSE ;
}
void SCALE::SetScaleParam (HWND hwnd)
{
	AOBJECTID  numObject ;
	int       index ;
	LONG      theGetKey = AOBJ_GETFIRSTKEY ;
	char * text ;
	int       ComboBoxItem ;

	//CheckRadioButton (hwnd, Position_Max, Position_Min, Position_Max) ;
	CheckDlgButton   (hwnd, Check_CanDrag,  data0->canDrag) ;
	CheckDlgButton   (hwnd, Check_Stamp,    data0->stamp ) ;
	CheckDlgButton   (hwnd, Check_Synch,    data0->synchronous) ;
  CheckDlgButton   (hwnd, Check_Activate, data0->canActivate) ;

	text = new char[255] ;
	lstrcpy(text, "(None)") ;
	for (ComboBoxItem=401; ComboBoxItem<=403; ComboBoxItem++)
    {
		//index = SendDlgItemMessage(hwnd, ComboBoxItem, CB_INSERTSTRING, 0, (long)text) ;
		index = (int)ComboBox_InsertString (GetDlgItem(hwnd, ComboBoxItem), 0, text) ;
		ComboBox_SetItemData(GetDlgItem(hwnd, ComboBoxItem), index, 0) ;
		//SendDlgItemMessage(hwnd, ComboBoxItem, CB_SETITEMDATA, index, 0) ;
	} 
	while ((numObject = AObjGetNextObject (OTYPE_NUMBER, &theGetKey)) != 0)
	{
		AObjGetName (numObject, text, 255) ;
		for(ComboBoxItem=401; ComboBoxItem<=403; ComboBoxItem++)
		{
			//index = SendDlgItemMessage (hwnd, ComboBoxItem, CB_INSERTSTRING, -1, (long)text) ;
			//SendDlgItemMessage (hwnd, ComboBoxItem, CB_SETITEMDATA, (WPARAM)index, (LPARAM) numObject) ;
			index = (int)ComboBox_InsertString (GetDlgItem (hwnd, ComboBoxItem), -1, text) ;
			ComboBox_SetItemData (GetDlgItem(hwnd, ComboBoxItem), index, numObject) ;
		}
	}
	delete[] text ;

	SetCurrentDisplay (hwnd, data0->max, Value_Max) ;
	SetCurrentDisplay (hwnd, data0->min, Value_Min) ;
	SetCurrentDisplay (hwnd, data0->val, Value_Val) ;

	/*maxpoint.x = data0->maxpos.x ;
	maxpoint.y = data0->maxpos.y ;
	minpoint.x = data0->minpos.x ;
	minpoint.y = data0->minpos.y ;*/

}

void SCALE::SetCurrentDisplay(HWND hwnd, AOBJECTID numObject, int comboBoxName)
{
	LPSTR theObjectName ;
	int   theIndex ;

	theObjectName = new char[255] ;
	//if (numObject == 0) return ;
	if (numObject ==0)
		lstrcpy(theObjectName, "(None)") ;
	 else
		AObjGetName (numObject, theObjectName, 256) ;
		
	//theIndex = SendDlgItemMessage (hwnd, comboBoxName, CB_FINDSTRING, -1, (long) theObjectName) ;
	//SendDlgItemMessage (hwnd, comboBoxName, CB_SETCURSEL, theIndex, 0) ;
	//SendDlgItemMessage (hwnd, comboBoxName, CB_SETITEMDATA, (WPARAM)theIndex, (LPARAM)numObject) ;
	theIndex = (int)ComboBox_FindString (GetDlgItem(hwnd, comboBoxName), -1, theObjectName) ;
	ComboBox_SetCurSel (GetDlgItem(hwnd, comboBoxName),theIndex) ;
	ComboBox_SetItemData (GetDlgItem(hwnd, comboBoxName), theIndex, numObject) ;

	delete (theObjectName) ;
}
void SCALE::SaveScaleParam (HWND hwnd)
{
	data0->canDrag     = IsDlgButtonChecked (hwnd, Check_CanDrag) ;
	data0->stamp       = IsDlgButtonChecked (hwnd, Check_Stamp) ;
	data0->synchronous = IsDlgButtonChecked (hwnd, Check_Synch) ;
  data0->canActivate = IsDlgButtonChecked (hwnd, Check_Activate) ;
	data0->wasSetup    = TRUE ;
	data0->scalepos.x  = scaleData.rect.left ;
	data0->scalepos.y  = scaleData.rect.top ;
	data0->maxpos.x    = indData.maxRect.left ;
	data0->maxpos.y    = indData.maxRect.top ;
	data0->minpos.x    = indData.minRect.left ;
	data0->minpos.y    = indData.minRect.top ;
	data0->max         = (AOBJECTID) GetObjectID (hwnd, Value_Max) ;
	data0->min         = (AOBJECTID) GetObjectID (hwnd, Value_Min) ;
	data0->val         = (AOBJECTID) GetObjectID (hwnd, Value_Val) ;

	DIBitmapToObject (oiScale, scaleData.hScale, DIAL) ;
  DIBitmapToObject (oiScale, indData.hIndicator, INDICATOR) ;
}

long SCALE::GetObjectID(HWND hwnd, int ButtonID)
{
	int        Index ;
	AOBJECTID   oiNum ;

	//Index = SendDlgItemMessage (hwnd, ButtonID, CB_GETCURSEL, 0, 0L) ;
	Index = (int)ComboBox_GetCurSel (GetDlgItem(hwnd, ButtonID)) ;
	if (Index != CB_ERR)
		oiNum = (AOBJECTID)ComboBox_GetItemData (GetDlgItem(hwnd, ButtonID), Index) ;
	//	oiNum = (AOBJECTID)(SendDlgItemMessage (hwnd, ButtonID, CB_GETITEMDATA, (WPARAM)Index, 0L)) ;
	else
		oiNum = 0 ;
	return ((long)oiNum) ;
}

void SCALE::CreateLayoutWindow (HWND hwnd)
{
	HWND  hOutline ;
	POINT org ;
	int   width ;
  int   height ;

	hOutline = GetDlgItem(hwnd, Obj_Position) ;

	GetWindowRect(hOutline, &layoutRect) ;
	ShowWindow (hOutline, SW_HIDE) ;

	org.x = layoutRect.left ;
	org.y = layoutRect.top  ;
	ScreenToClient(hwnd, &org) ;
	width = layoutRect.right-layoutRect.left,
	height = layoutRect.bottom-layoutRect.top,

	hLayoutW = CreateWindow(
							"PositionWndClass",
						 	NULL,
							WS_CHILD|WS_BORDER|WS_HSCROLL|WS_VSCROLL,
						 	org.x,
						 	org.y,
						 	width,
						 	height,
						 	hwnd,
						 	(HMENU)NULL,
						 	(HINSTANCE)hInstance,
							NULL) ;

	ShowWindow(hLayoutW, SW_SHOW) ;
	UpdateWindow(hLayoutW) ;
}

void SCALE::GetBitmapHandle(void)
{

	scaleData.hScale = ObjectToDIBitmap (oiScale, &scaleData.rect, DIAL) ;
	if (scaleData.hScale)
		{
		OffsetRect (&scaleData.rect, data0->scalepos.x, data0->scalepos.y) ;
		}
	else
		{
		SetRectEmpty (&scaleData.rect) ;
    }
	indData.hIndicator = ObjectToDIBitmap (oiScale, &indData.rect, INDICATOR) ;
	if (indData.hIndicator)
		{
		indData.maxRect=indData.rect ;
		indData.minRect=indData.rect ;
		OffsetRect (&indData.maxRect, data0->maxpos.x, data0->maxpos.y) ;
		OffsetRect (&indData.minRect, data0->minpos.x, data0->minpos.y) ;
		}
	else
		{
		SetRectEmpty (&indData.maxRect) ;
		SetRectEmpty (&indData.minRect) ;
    }                                
	GetScrollRange (scaleData.rect, indData.maxRect, indData.minRect) ;
}
void SCALE::GetScrollRange (RECT rect1, RECT rect2, RECT rect3)
{
	nVscrollMax = max(max(rect1.bottom, rect2.bottom), rect3.bottom) - (layoutRect.right - layoutRect.left) ;
	nHscrollMax = max(max(rect1.right, rect2.right), rect3.right) - (layoutRect.bottom- layoutRect.top) ;
	if (nVscrollMax%5)
	  nVscrollMax = (nVscrollMax/5+1)*5 ;
	if (nHscrollMax%5)
		nHscrollMax = (nHscrollMax/5+1)*5 ;

	if (nVscrollMax<100)
		nVscrollMax = 100 ;
	if (nHscrollMax<100)
		nHscrollMax = 100 ;
}
void SCALE::ScaleSetScrollPos (HWND hwnd)
{
	SetScrollRange (hwnd, SB_VERT, 0, nVscrollMax, FALSE) ;
	SetScrollPos (hwnd, SB_VERT, nVscrollPos, TRUE) ;
	SetScrollRange (hwnd, SB_HORZ, 0, nHscrollMax, FALSE) ;
	SetScrollPos (hwnd, SB_HORZ, nHscrollPos, TRUE) ;
}
void SCALE::DoVscroll (HWND hwnd, WPARAM wParam)
{
	int nVscrollInc ;

	switch (LOWORD(wParam))
 		{
		case SB_TOP:
    		nVscrollInc = -nVscrollPos ;
        break ;

    case SB_BOTTOM:
        nVscrollInc = nVscrollMax - nVscrollPos ;
        break ;

    case SB_LINEUP:
				nVscrollInc = -5 ;
        break ;

    case SB_LINEDOWN:
				nVscrollInc = 5 ;
        break ;

    case SB_PAGEUP:
				nVscrollInc = min (-5, -nVscrollMax ) ;
				break ;

    case SB_PAGEDOWN:
				nVscrollInc = max (5, nVscrollMax ) ; 
        break ;

    case SB_THUMBTRACK:
        nVscrollInc = HIWORD (wParam) - nVscrollPos ;
        break ;

    default:
        nVscrollInc = 0 ;
		}
	nVscrollInc = max (-nVscrollPos,
								min (nVscrollInc, nVscrollMax - nVscrollPos)) ;
	if (nVscrollInc)
  	{
		nVscrollPos += nVscrollInc ;
		nVfirstPixel +=nVscrollInc ;
		ScrollWindow (hwnd, 0, -nVscrollInc, NULL, NULL) ;
    SetScrollPos (hwnd, SB_VERT, nVscrollPos, TRUE) ;
		UpdateWindow (hwnd) ;
		}
}
void SCALE::DoHscroll (HWND hwnd, WPARAM wParam)
{
	int nHscrollInc;

	switch (LOWORD(wParam))
	{
  	case SB_LINEUP:
        nHscrollInc = -5 ;
        break ;

    case SB_LINEDOWN:
        nHscrollInc = 5 ;
        break ;

    case SB_PAGEUP:
        nHscrollInc = min (-5, -nHscrollMax ) ; //-10 ;
        break ;

    case SB_PAGEDOWN:
        nHscrollInc = max (5, nHscrollMax) ; //10 ;
        break ;

    case SB_THUMBPOSITION:
        nHscrollInc = HIWORD (wParam) - nHscrollPos ;
        break ;

    default:
				nHscrollInc = 0 ;
	}
  nHscrollInc = max (-nHscrollPos,
                min (nHscrollInc, nHscrollMax - nHscrollPos)) ;
  if (nHscrollInc)
  	{
		nHscrollPos += nHscrollInc ;
		nHfirstPixel +=nHscrollInc ;
		ScrollWindow (hwnd, -nHscrollInc, 0, NULL, NULL) ;
		SetScrollPos (hwnd, SB_HORZ, nHscrollPos, TRUE) ;
		UpdateWindow (hwnd) ;
		}
} 
long CALLBACK /*!!PORT!! was _far_ _pascal_*/  LayoutWndProc (
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
{
	SCALE      *scale ;

	scale  = (SCALE*) GetWindowLong(GetParent(hwnd), DWL_USER) ;
	if (!scale)
		DefWindowProc (hwnd, message, wParam, lParam)  ;

	switch(message)
		{
		case WM_CREATE:
			scale->ScaleSetScrollPos(hwnd) ;
			return 0 ;

		case WM_PAINT:
   		scale->RepaintBitmap(hwnd) ;
			return 0 ;

		case WM_PALETTECHANGED:
			// if the palette was realized by this window itself, then ignore it
			if ((HWND) wParam == hwnd)
        return 0;
    /* else, fall through */
    case WM_QUERYNEWPALETTE:
      if (PaletteCls::GetPaletteHandle())
				{
				HDC       hdc     = GetDC(hwnd);
        HPALETTE  hpalOld = SelectPalette(hdc, PaletteCls::GetPaletteHandle(), FALSE);
				BOOL      lRet = RealizePalette(hdc);
        SelectPalette(hdc, hpalOld, TRUE);
        RealizePalette(hdc);
        ReleaseDC(hwnd, hdc);

        if(lRet)                            
          InvalidateRect(hwnd, NULL, TRUE);
				return 1;
        }
			return 0;

		case WM_VSCROLL: //!!PORT!!
			scale->DoVscroll (hwnd, wParam) ;
			return 0 ;

	 	case WM_HSCROLL: //!!PORT!!
			scale->DoHscroll (hwnd, wParam) ;
			return 0 ;

		case WM_LBUTTONDOWN:
			if (!scale->MouseDown (hwnd, lParam))
				return 0 ;
			break ;

		case WM_MOUSEMOVE:
			if(!scale->MoveBitmap(hwnd,wParam, lParam) )
				return 0 ;
			break ;

		case WM_LBUTTONUP:
			if (scale->MouseUp(hwnd))
				return 0 ; 
			break ;
	}

	return DefWindowProc (hwnd, message, wParam, lParam)  ;
}
void SCALE::LoadBitmap(HWND hwnd, int LoadMethod, int scaleOrInd)
{
	HANDLE hDIB ;
	POINT  size ;

  
	if (scaleOrInd==DIAL)
		hDIB = scaleData.hScale ;
	else
		hDIB = indData.hIndicator ;

	if (LoadMethod==CLIPBOARD)
		{
		hDIB = LoadBitmapFromClipboard(hwnd, hDIB, &size) ;
		}
	else
		{
		hDIB = LoadBitmapFromFile(hwnd, hDIB, &size) ;//&rect) ;
		}

	if (hDIB)
  	{
		if (scaleOrInd==DIAL)
			{
			scaleData.hScale = hDIB; //hBitmap ;
			scaleData.rect.right  = scaleData.rect.left+size.x ;
			scaleData.rect.bottom = scaleData.rect.top+ size.y ;
			}
		else
			{
			indData.hIndicator = hDIB; //hBitmap ;
			indData.maxRect.right  = indData.maxRect.left+size.x ;
			indData.maxRect.bottom = indData.maxRect.top+size.y ;
			indData.minRect.right  = indData.minRect.left+size.x ;
      indData.minRect.bottom = indData.minRect.top +size.y ;
			}
    }
	InvalidateRect (hLayoutW, NULL, TRUE) ;
	UpdateWindow(hLayoutW) ;
}
void SCALE::RepaintBitmap(HWND hwnd)
{
	HDC hdc ;
	PAINTSTRUCT ps ;
	HPALETTE    hpalOld;

	hdc = BeginPaint (hwnd, &ps) ;
	SetViewportOrgEx(hdc, -nHfirstPixel,-nVfirstPixel, NULL) ;
	if (PaletteCls::GetPaletteHandle ())
	  {
		hpalOld = SelectPalette (hdc, PaletteCls::GetPaletteHandle(), TRUE) ;
		RealizePalette (hdc) ;  
	  }

	if (scaleData.hScale)
		PaintDIBitmap (hdc, scaleData.hScale, &scaleData.rect) ;

	if (indData.hIndicator)
  	{
		PaintDIBitmap (hdc, indData.hIndicator, &indData.minRect) ;
		PaintDIBitmap (hdc, indData.hIndicator, &indData.maxRect) ;
		DrawIndText (hdc, scaleData.rect, indData.maxRect, 0) ;
		DrawIndText (hdc, scaleData.rect, indData.minRect, 1) ;
		}
	if (PaletteCls::GetPaletteHandle())
	  {
	  SelectPalette (hdc, hpalOld, TRUE) ;
		RealizePalette (hdc) ;
	  }

	EndPaint (hwnd, &ps) ;
	GetScrollRange (scaleData.rect, indData.minRect, indData.maxRect) ;
	ScaleSetScrollPos (hwnd) ;
}
BOOL SCALE::MouseDown(HWND hwnd, LPARAM lParam)
{
	RECT rect ;
	POINT offsetMouse ;

	if (mousedown)
		return FALSE ;

	SetCapture(hwnd) ;
	mousedown = TRUE ;

	A_MAKEPOINT(mouse_save,lParam) ;
	offsetMouse.x = mouse_save.x + nHfirstPixel ;
	offsetMouse.y = mouse_save.y + nVfirstPixel ;

	whichpicked = 0 ;
	if(CheckInBitmap (indData.minRect, offsetMouse, &cursor_pos)) 
		{
    rect=indData.minRect ;
		whichpicked = 1 ;
		}
	else if(CheckInBitmap (indData.maxRect, offsetMouse, &cursor_pos)) 
	{
    rect=indData.maxRect ;
		whichpicked = 2 ;
		}
  else if (CheckInBitmap (scaleData.rect, offsetMouse, &cursor_pos)) 
		{
		rect=scaleData.rect ;
		whichpicked = 3 ;
		}
	if (whichpicked)
  	{
		RECT emptyRect ;
		SetRectEmpty(&emptyRect) ;
		OffsetRect (&rect, -nHfirstPixel, -nVfirstPixel) ;
		DrawFrame (hwnd, rect, emptyRect) ;
		}
	return TRUE ;
}
BOOL SCALE::MoveBitmap (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	LPRECT lprect ;

	if (!GetCapture()) return FALSE ;
	if (!mousedown) return FALSE ;
	if(!(wParam&MK_LBUTTON)) return FALSE ;

	if (whichpicked)
		{
		if (whichpicked == 1)
			lprect = &indData.minRect ;
		if (whichpicked == 2)
			lprect = &indData.maxRect ;
		if(whichpicked == 3)
			lprect = &scaleData.rect ;
		MouseMove (hwnd, lParam, lprect) ;
		}
  return TRUE ;
}
void SCALE::MouseMove (HWND hwnd, LONG lParam, LPRECT lprect)
{
	int x, y ;
	//int w, h ;
	int xchg, ychg ;
	//HDC hdc , hdcMem ;
	RECT  newRect ;
	RECT  offRect1,offRect2 ;

	if (hwnd == NULL) return ;

	xchg = LOWORD (lParam) - mouse_save.x ;
	ychg = HIWORD (lParam) - mouse_save.y ;
	if (xchg==0 && ychg==0) return ;

	x = mouse_save.x + xchg - cursor_pos.x -1 + nHfirstPixel ;
	y = mouse_save.y + ychg - cursor_pos.y - 1 + nVfirstPixel ;
		
	if (x < 0) x = 0 ;
	if (y < 0) y = 0 ;
	newRect.left  = x ;
	newRect.top   = y ;
	newRect.right = x + lprect->right - lprect->left ;
	newRect.bottom= y + lprect->bottom - lprect->top ;
	if (((newRect.left-lprect->left)==0) && ((newRect.top-lprect->top)==0)) return ;

	CopyRect (&offRect1, &newRect) ;
	OffsetRect (&offRect1, -nHfirstPixel, -nVfirstPixel) ;
	CopyRect (&offRect2, lprect) ;
	OffsetRect (&offRect2, -nHfirstPixel, -nVfirstPixel) ;
	DrawFrame (hwnd, offRect1, offRect2) ;
	
	*lprect = newRect ;

	return ;
}
BOOL SCALE::MouseUp(HWND hwnd)
{
	ReleaseCapture() ;
	if (mousedown)
  	{
		InvalidateRect (hwnd, NULL, TRUE) ;
		mousedown = FALSE ;
		whichpicked = 0 ;
		}
	return TRUE ;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
AOBJECTID SCALE::GetObject (int Index)
{
	AOBJECTID relObj ;

	if (Index==0)
		relObj = data0->max ;
	if (Index==1)
		relObj = data0->min ;
	if (Index==2)
		relObj = data0->val ;

  return (relObj) ;
}

void SCALE::SetPosition(void) 
{
	AEvtInfo	event;

	event.sourceObject = oiScale;
	event.targetObject = oiScale;
	event.when         = AAppGetTime();
	event.how          = AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL);
	event.what         = AEVENT_OBJECTCHANGED;
	event.modifiers    = 0L ;
	event.details			 = 0L;
	event.variation		 = 0L;
	AEvtPost(&event);
}
