#ifndef __SERICON_H__
#define __SERICON_H__

#include "memclass.h"
#include "system.h"

#define MASKARRAYSIZE	(32*32/8)
#define IMAGEARRAYSIZE (32*32/2)

struct COLORICON
	{
	BITMAPINFOHEADER				imageHdr;						//these two fields must remain
	RGBQUAD									imageColors[16];    //adjacent and in this order
	BYTE										imageArray[IMAGEARRAYSIZE];
	BITMAPINFOHEADER				maskHdr;						//these two fields must remain
	RGBQUAD									maskColors[2];    	//adjacent and in this order
	BYTE										maskArray[MASKARRAYSIZE];
	};

typedef COLORICON far* 	LPCOLORICON;

struct BWICON
	{
	BITMAPINFOHEADER				imageHdr;						//these two fields must remain
	RGBQUAD									imageColors[2];    	//adjacent and in this order
	BYTE										imageArray[MASKARRAYSIZE];
	BITMAPINFOHEADER				maskHdr;						//these two fields must remain
	RGBQUAD									maskColors[2];    	//adjacent and in this order
	BYTE										maskArray[MASKARRAYSIZE];
	};

typedef BWICON far*		LPBWICON;

class SERIUSICON:public MEMORY
	{
private:

	void InitPixels(
		short			x,
		short			y);

public:

	COLORICON		colorIcon;
	BWICON			bwIcon;

	SERIUSICON(
		void);

	SERIUSICON::SERIUSICON(
		int		fileHandle,
		short	colorCount);

	SERIUSICON(
		void far*	objPtr)
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

	void Draw(
		HDC	hDC,
		int	x,
		int y)
		{
		if (pTheSystem->GetNumColors() > 4)
			DrawColor(hDC, x, y);
		else
			DrawBW(hDC, x, y);
		};

	void DrawColorImage(
		HDC hDC,
		int x,
		int y);

	void DrawColorMask(
		HDC hDC,
		int x,
		int y);

	void DrawBWImage(
		HDC hDC,
		int x,
		int y);

	void DrawBWMask(
		HDC hDC,
		int x,
		int y);

	void DrawMask(
		HDC hDC,
		int x,
		int y)
		{
		if(pTheSystem->GetNumColors() > 4)
			DrawColorMask(hDC, x, y);
		else
			DrawBWMask(hDC, x, y);
		};

	void DrawImage(
		HDC hDC,
		int x,
		int y)
		{
		if(pTheSystem->GetNumColors() > 4)
			DrawColorImage(hDC, x, y);
		else
			DrawBWImage(hDC, x, y);
		};

	BOOL Write(
		int  hFile);

	int GetIconSize()
		{
		return 32;  //must be the same value as the define ICONSIZE in the .cpp
		}

	};

#endif
