#ifndef _ATTRIBUTES.H_
#define _ATTRIBUTES.H_

#include <windows.h>
#include <commdlg.h>

#define WINDOW_BACKGROUND			0xFF000000L
#define WINDOW_FOREGROUND			0xFE000000L

typedef UINT (FAR PASCAL *HOOKFUNC)(HWND, UINT, WPARAM, LPARAM);

////////////////////////////

class ATTRIBUTES
	{
	private:
		int		 fontHeight;
		char	 fontFace[LF_FACESIZE];
		int		 fontWeight;				//FW_BOLD, FW_EXTRABOLD....
		BOOL	 italic;
		BOOL	 underlined;
		BOOL	 struckout;
		COLORREF foreground;
		COLORREF background;

	public:
		ATTRIBUTES(BOOL createStuff = TRUE);

		COLORREF GetForeground(){	return foreground == WINDOW_FOREGROUND ? GetSysColor(COLOR_WINDOWTEXT):	foreground; }
		COLORREF GetBackground(){	return background == WINDOW_BACKGROUND ? GetSysColor(COLOR_WINDOW):	background; }

		void SetForeground(COLORREF f){	foreground = f;}
		void SetBackground(COLORREF b){	background = b;}

		int GetFontCellHeight();
		HFONT	CreateTheFont(HDC hdc=NULL, BOOL useOip=FALSE);
		void LogfontToAttributes(LPLOGFONT logFont);
		void AttributesToLogfont(LPLOGFONT logFont);

		void EditFont(HWND owner = NULL,
									LPSTR str="AaBbYyZz123",
									BOOL doColors=TRUE,
									DWORD flags = CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_ENABLETEMPLATE|CF_ENABLEHOOK,
									BOOL doBackground=TRUE);

	};
typedef ATTRIBUTES far * LPATTRIBUTES;

/////////////////////////////////////

class EDIT_FONT_DATA
	{
	public:
		COLORREF		background;
		BOOL				doColors;
		LPSTR				sampleStr;

		EDIT_FONT_DATA(COLORREF b, BOOL d, LPSTR s)
			{
			background = b;
			doColors = d;
			sampleStr = s;
			}
	};
typedef EDIT_FONT_DATA far * LPEDIT_FONT_DATA;

///////////////////////////////////
/*
class COLORHOOKDATA
	{
	public:
		HWND								hWndItem;
		COLORREF far				*colorPtr;
		HBRUSH far 					*brushPtr;

		COLORHOOKDATA(HWND hi, COLORREF far *cPtr, HBRUSH far *bPtr)
			{
			hWndItem = hi;
			colorPtr = cPtr;
			brushPtr = bPtr;
			}
	};
typedef COLORHOOKDATA far * LPCOLORHOOKDATA;
*/
////////////////////////////////////

inline int PixelsToSeventySeconds(
	int			pixels,
	HDC			hdc = NULL)
	{
	int			retVal;

	if (hdc == NULL)
		{
		hdc = CreateDC("Display", NULL, NULL, NULL);
		retVal = MulDiv(pixels, 720, GetDeviceCaps(hdc, LOGPIXELSY) );
		DeleteDC(hdc);
		}
	else
		retVal = MulDiv(pixels, 720, GetDeviceCaps(hdc, LOGPIXELSY) );

	return retVal;
	}


inline int SeventySecondsToPixels(
	int			SeventySeconds,
	HDC			hdc=NULL)
	{
	int			retVal;

	if (hdc==NULL)
		{
		hdc = CreateDC("Display", NULL, NULL, NULL);
		retVal = MulDiv(SeventySeconds, GetDeviceCaps(hdc, LOGPIXELSY), 720);
		DeleteDC(hdc);
		}
	else
		retVal = MulDiv(SeventySeconds, GetDeviceCaps(hdc, LOGPIXELSY), 720);

	return retVal;
	}



////////////////////////////////////////
/// From attributes.cpp  /////
////////////////////////////////
UINT far pascal _export ChooseFontHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
VOID DrawSampleText(HWND hWnd, LPDRAWITEMSTRUCT di);
void DrawBackColorDropDown(HWND hWnd, LPDRAWITEMSTRUCT di);
COLORREF EditColor(HWND parent, COLORREF color);

void	ExportAttributes(ATTRIBUTES* theAttributes);
void	ImportAttributes(ATTRIBUTES* theAttributes);

#endif
