#include "almsys.h"
#include "system.h"
#include "rsrcmgr.h"
#include "a_almutl.h"


/************************************************************************/
/*  The following functions implement the resource manager
		May 3, 1993
************************************************************************/

SER_WINRESOURCES::SER_WINRESOURCES()

	{
	fontMgr = new FONTMGR();
	brushMgr = new BRUSHMGR();
	}

SER_WINRESOURCES::~SER_WINRESOURCES()
	{
	delete fontMgr;
	delete brushMgr;
	}

int FONTMGR::FontHandleCompare(
	HFONT	hFont,
	int*	prevIndex)
	{
	int rVal = -1;
	int i;
	SER_FONT *curFont = serHdFont;

	for (i = 0; i < numOfFonts; i++)
		{
		if (curFont->hFont == hFont)
      {
			rVal = i;
			*prevIndex = i - 1;

			break;
			}
		else
			curFont = curFont->next;
		}

	return rVal;
	}

int FONTMGR::FontCompare(
	LOGFONT	*lf,
	int			fromFont,
	int			toFont)
	{
	int i;
	LOGFONT oldlf;
	int rVal = -1;
	SER_FONT *curFont = serHdFont;

	for (i = fromFont; i < toFont; i++)
		{
		oldlf = curFont->logFont;
		
		if ((lf->lfHeight == oldlf.lfHeight)
			&& (lf->lfWidth == oldlf.lfWidth)
			&& (lf->lfEscapement == oldlf.lfEscapement)
			&& (lf->lfOrientation == oldlf.lfOrientation)
			&& (lf->lfWeight == oldlf.lfWeight)
			&& (lf->lfItalic == oldlf.lfItalic)
			&& (lf->lfUnderline == oldlf.lfUnderline)
			&& (lf->lfStrikeOut == oldlf.lfStrikeOut)
			&& (lf->lfCharSet == oldlf.lfCharSet)
			&& (lf->lfOutPrecision == oldlf.lfOutPrecision)
			&& (lf->lfClipPrecision == oldlf.lfClipPrecision)
			&& (lf->lfQuality == oldlf.lfQuality)
			&& (lf->lfPitchAndFamily == oldlf.lfPitchAndFamily)
			&& (lstrcmp(lf->lfFaceName ,oldlf.lfFaceName)== 0)
			)
      {
			return i;
      }

		curFont = curFont->next;
		}
  return rVal;
	}

FONTMGR::FONTMGR()
	{
	serHdFont = NULL;
	serFonts = NULL;
	numOfFonts = 0;
	}


FONTMGR::~FONTMGR()
	{
	int i;
  HANDLE hMem;

	SER_FONT *curFont = serHdFont;

	for (i = 0; i < numOfFonts; i++)
		{
		/*
		#ifdef DEBUG
			char outStr[40];
			wsprintf(outStr, "Handle deleted %d", curFont->hFont);
			MessageBox(NULL, "CLEAN UP ", outStr , MB_OK);
		#endif */

		DeleteObject(curFont->hFont);
		
		hMem  = curFont->hMem;
		curFont = curFont->next;

		LocalUnlock(hMem);
    LocalFree(hMem);
		}
  
	numOfFonts = 0;
	}

HFONT FONTMGR::CreateHandle(
	LOGFONT 	*lf)
	{
	HFONT  rFontVal;
	int    index;
	SER_FONT *curFont;
	if ((index = FontCompare(lf, 0, numOfFonts)) < 0)
		{
    // Create a new ser_font block
		
	  HANDLE	hMem = LocalAlloc(LMEM_ZEROINIT, sizeof(SER_FONT));
		SER_FONT *newFont =  (SER_FONT *)LocalLock(hMem);

		CopyMemory(&(newFont->logFont), lf, sizeof(LOGFONT));

		newFont->hFont = CreateFontIndirect(lf);
		newFont->refCount = 1;
		newFont->hMem = hMem;
		newFont->next = NULL;

		// add it to the link list
		if (serHdFont == NULL)
			{
			serHdFont = newFont;
			serHdFont->next = NULL;
			serFonts = serHdFont;
			}
		else
			{
			serFonts->next = newFont;
			serFonts= newFont;
			serFonts->next = NULL;
			}

		rFontVal = serFonts->hFont;
		numOfFonts++;
    }
	else
		{
		curFont = GetSerFont(index);

		rFontVal = curFont->hFont;
		curFont->refCount++; //increase the ref count
		}

  return rFontVal;
	}

short FONTMGR::DeleteHandle(
	HFONT	hFont)
	{
	int index;
	int prevIndex;
	short rVal;
	SER_FONT *curFont;
	if ((index = FontHandleCompare(hFont, (int *)&prevIndex)) < 0)
		{
		// ERROR -> this handle was not created by OIP
		rVal = AUTL_HANDLENOTCREATED;
    }
	else
		{
		SER_FONT *prevFont;

		prevFont =  GetSerFont(index - 1);

		curFont = GetSerFont(index);

		curFont->refCount--;
		// if still being refered
		if (curFont->refCount == 0)
		  {
			if (numOfFonts == 1)
		   	{
				serHdFont = NULL; // set the hd of the list to null
				}
			else if (index == numOfFonts - 1) //if end of list
				{
				prevFont->next = NULL;
				}
			else if (index == 0) // first font
				{
				serHdFont = curFont->next;
				}
			else
				{ // else normal case
				prevFont->next = curFont->next;
				}
			DeleteObject(curFont->hFont);

			LocalUnlock(curFont->hMem);
			LocalFree(curFont->hMem);

			rVal = AUTL_HANDLEDELETED;
			numOfFonts--;

      serFonts = GetSerFont(numOfFonts - 1);// init the tail
			}
		else
			rVal = AUTL_HANDLEINUSE;
		}

	return rVal;
	}

/**********************************************************************
// SER_BRUSH class

***********************************************************************/

int BRUSHMGR::BrushHandleCompare(
	HBRUSH	hBrush,
	int*		prevIndex)
	{
	int rVal = -1;
	int i;
	SER_BRUSH *tmpSBrush = serHdBrush;

	for (i = 0; i < numOfBrushes; i++)
		{
		if (tmpSBrush->hBrush == hBrush)
      {
			rVal = i;
			*prevIndex = i - 1;

			break;
			}
		tmpSBrush = tmpSBrush->next;
		}

	return rVal;
	}

int BRUSHMGR::BrushCompare(
	LOGBRUSH	*lb)
	{
	int i;
	LOGBRUSH oldlb;
	int rVal = -1;
	SER_BRUSH *curBrush = serHdBrush;

	for (i = 0; i < numOfBrushes; i++)
		{
		oldlb = curBrush->logBrush;
		if ((oldlb.lbStyle == lb->lbStyle) &&
				(oldlb.lbColor == lb->lbColor) &&
				(oldlb.lbHatch == lb->lbHatch))
			return i;

		curBrush = curBrush->next;
		}
  return rVal;
	}

BRUSHMGR::BRUSHMGR()
	{
	serHdBrush = NULL;
	serBrushes = NULL;
	numOfBrushes = 0;

	return;
	}

BRUSHMGR::~BRUSHMGR()
	{
	int i;
	SER_BRUSH *curBrush = serHdBrush;
	HANDLE hMem; 
	for (i = 0; i < numOfBrushes; i++)
		{
		/*
		#ifdef DEBUG
			char outStr[40];
			wsprintf(outStr, "Handle deleted %d", curBrush->hBrush);
			MessageBox(NULL, "CLEAN UP ", outStr , MB_OK);
		#endif */

		DeleteObject(curBrush->hBrush);

    hMem = curBrush->hMem;
		curBrush = curBrush->next;
		LocalUnlock(hMem);
    LocalFree(hMem);

		}
  
	numOfBrushes = 0;
	}




HBRUSH BRUSHMGR::CreateHandle(
	LOGBRUSH	*lb)
	{
	HBRUSH rBrushVal;
	int    index;
	SER_BRUSH *curBrush;

	if ((index = BrushCompare(lb)) < 0)
		{
		// Create a new ser_font block
	
		HANDLE	hMem = LocalAlloc(LMEM_ZEROINIT, sizeof(SER_BRUSH));
		SER_BRUSH *newBrush = (SER_BRUSH*) LocalLock(hMem);

		CopyMemory(&(newBrush->logBrush), lb, sizeof(LOGBRUSH));

		newBrush->hBrush = CreateBrushIndirect(lb);
		newBrush->refCount = 1;
		newBrush->hMem = hMem;
		newBrush->next = NULL;

		// add it to the link list
		if (serHdBrush == NULL)
			{
			serHdBrush = newBrush;
			serHdBrush->next = NULL;
			serBrushes = serHdBrush;
			}
		else
			{
			serBrushes->next= newBrush;
		  serBrushes= newBrush;
			serBrushes->next = NULL;
			}

		rBrushVal = serBrushes->hBrush;
		numOfBrushes++;
    }
	else
		{
		curBrush = GetSerBrush(index);

		rBrushVal = curBrush->hBrush;
		curBrush->refCount++; //increase the ref count
		}

	return rBrushVal;
	}


SER_BRUSH *BRUSHMGR::GetSerBrush(
	int	index)
	{
	SER_BRUSH *curBrush = serHdBrush;
  int i;
	for (i = 0; i < index; i++) 
		curBrush = curBrush->next;

  return curBrush;
	}

SER_FONT* FONTMGR::GetSerFont(
	int	index)
	{
	SER_FONT *curFont = serHdFont;
	int i;
	for (i = 0; i < index; i++)
		curFont = curFont->next;

	return curFont;
	}

short BRUSHMGR::DeleteHandle(
	HBRUSH	hBrush)
	{
	int index;
	int prevIndex;
	short rVal;
	SER_BRUSH *curBrush;
	SER_BRUSH *prevBrush;

	if ((index = BrushHandleCompare(hBrush, (int *)&prevIndex)) < 0)
		{
		// ERROR -> this handle was not created by OIP
		rVal = AUTL_HANDLENOTCREATED;
    }
	else
		{
		prevBrush =  GetSerBrush(index - 1);

		curBrush = GetSerBrush(index);

		curBrush->refCount--;
		// if still being refered
		if (curBrush->refCount == 0)
		  {
			if (numOfBrushes == 1)
		   	{
        serHdBrush = NULL;
				}
			else if (index == numOfBrushes - 1) //if end of list
				{
				prevBrush->next = NULL;
				}
			else if (index == 0) // first font
				{
				serHdBrush = curBrush->next;
				}
			else
				{ // else normal case
				prevBrush->next = curBrush->next;
				}
			DeleteObject(curBrush->hBrush);

			LocalUnlock(curBrush->hMem);
			LocalFree(curBrush->hMem);

			rVal = AUTL_HANDLEDELETED;
			numOfBrushes--;

			serBrushes = GetSerBrush(numOfBrushes - 1);// init the tail

			}
		else
			rVal = AUTL_HANDLEINUSE;
		}

	return rVal;
	}




extern "C" HFONT ALMAPI  AUtlCreateFontIndirect(
	LOGFONT	*lf)
	{
	HFONT hFont = pTheSystem->serWResources->fontMgr->CreateHandle(lf);
  return hFont;
	}


extern "C" HFONT ALMAPI  AUtlCreateFont(
	 int lfHeight,
   int lfWidth,
   int lfEscapement,
   int lfOrientation,
   int lfWeight,
   DWORD  lfItalic,
   DWORD  lfUnderline,
   DWORD  lfStrikeOut,
   DWORD  lfCharSet,
   DWORD  lfOutPrecision,
   DWORD  lfClipPrecision,
   DWORD  lfQuality,
   DWORD  lfPitchAndFamily,
	 LPCSTR lfFaceName)
	 {
	 LOGFONT lf;

	 lf.lfHeight = lfHeight;
	 lf.lfWidth = lfWidth;
	 lf.lfEscapement = lfEscapement;
	 lf.lfOrientation = lfOrientation;
	 lf.lfWeight = lfWeight;
	 lf.lfItalic = (BYTE) lfItalic;
	 lf.lfUnderline = (BYTE) lfUnderline;
	 lf.lfStrikeOut = (BYTE) lfStrikeOut;
	 lf.lfCharSet = (BYTE) lfCharSet;
	 lf.lfOutPrecision = (BYTE) lfOutPrecision;
	 lf.lfClipPrecision = (BYTE) lfClipPrecision;
	 lf.lfQuality = (BYTE) lfQuality;
	 lf.lfPitchAndFamily = (BYTE) lfPitchAndFamily;
	 lstrcpy(lf.lfFaceName, lfFaceName);

	 HFONT hFont = pTheSystem->serWResources->fontMgr->CreateHandle((LOGFONT *)&lf);

   return hFont;
	 }

extern "C" short ALMAPI AUtlDeleteFont(
	HFONT	hFont)
	{
	short rVal = pTheSystem->serWResources->fontMgr->DeleteHandle(hFont);
  return rVal;
	}

/**********************************************************************
/// BRUSH Interface
***********************************************************************/
extern "C" HBRUSH  ALMAPI  AUtlCreateSolidBrush(
	COLORREF	clRef)
	{
	LOGBRUSH lb;

	lb.lbStyle = BS_SOLID;
	lb.lbColor = clRef;
	lb.lbHatch = 0; // does not matter for solid brushes;

	HBRUSH hBrush= pTheSystem->serWResources->brushMgr->CreateHandle(&lb);

  return hBrush;
	}


extern "C" HBRUSH  ALMAPI  AUtlCreateBrushIndirect(
	LOGBRUSH *lb)
	{
	HBRUSH hBrush= pTheSystem->serWResources->brushMgr->CreateHandle(lb);

  return hBrush;
	}


extern "C" short ALMAPI AUtlDeleteBrush(
	HBRUSH	hBrush)
	{
	short rVal = pTheSystem->serWResources->brushMgr->DeleteHandle(hBrush);
  return rVal;
	}

