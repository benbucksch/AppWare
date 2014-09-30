#ifndef __FONT_H
#define __FONT_H

#include <commdlg.h>
#include "memclass.h"

typedef UINT (CALLBACK *FONTHOOK)(HWND, UINT, WPARAM, LPARAM);

class FONT:public MEMORY
	{
	private:
		CHOOSEFONT cf;
		LOGFONT lf;


	public:
		FONT(
			HWND owner,
			LPLOGFONT lfInit);

		~FONT();

	BOOL ChooseFont(
		LPLOGFONT lfResult);


	static int XPixelsTo72nds(
		HDC hdc,
		int pixels)
		{
		return MulDiv(pixels, 72, GetDeviceCaps(hdc, LOGPIXELSX));
		}

	static int X72ndsToPixels(
		HDC hdc,
		int seventy2nds)
		{
		return MulDiv(seventy2nds, GetDeviceCaps(hdc, LOGPIXELSX), 72);
		}
	static int YPixelsTo72nds(
		HDC hdc,
		int pixels)
		{
		return MulDiv(pixels, 72, GetDeviceCaps(hdc, LOGPIXELSY));
		}

	static int Y72ndsToPixels(
		HDC hdc,
		int seventy2nds)
		{
		return MulDiv(seventy2nds, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		}

	static VOID DrawSampleText(
		HWND								hWnd,
		LPDRAWITEMSTRUCT		di);

	static UINT CALLBACK ChooseFontHook(
		HWND							hWnd,
		UINT							msg,
		WPARAM  					wParam,
		LPARAM  					lParam);
	};

#endif
