#ifndef __DIBITMAP_H
#define __DIBITMAP_H

#include "memclass.h"
#include "intdebug.h"

class DIBITMAP:public MEMORY
	{
	private:
		HPALETTE hPal;
		HPALETTE oldPal;
		HDC palDC;
		HGLOBAL  hBmp;
  	long	nColorTableSize;
  	LPBITMAPINFOHEADER pBI;

		BOOL DoDraw(
			HDC	hdc,
			int x,
			int y,
			BOOL forceDraw,
			LPRECT bounds);

		void BuildColorTable();

		void Initialize();

		void FreeMemory();

		DWORD CalcColorTableSize(
			LPBITMAPINFOHEADER pDibHdr);

		void CoreToInfo();

		static DWORD GetImageSize(LPBITMAPINFOHEADER infoHdr);
	
	
	public:
		DIBITMAP();

		DIBITMAP(
			HBITMAP bmp,
			HPALETTE hPal);
		
		DIBITMAP(
			LPSTR resourceName,
			HINSTANCE hInst);

		DIBITMAP(
			LPBITMAPINFO dibInfo);

		DIBITMAP(
			HANDLE fileHandle,
			int padding);

		~DIBITMAP();

		BOOL Valid() const
			{
			return (pBI && hPal)?TRUE:FALSE;
			}

		int Use(HDC hdc);
					
		BOOL CleanUp();
		
		void Draw(
			HDC hdc,
			int x,
			int y,
			LPRECT bounds=NULL);

		BOOL DrawIfPaletteChanged(
			HDC hdc,
			int x,
			int y,
			LPRECT bounds=NULL);
			
		int Width()
			{
			I_ASSERT(pBI);
			
			return (pBI != NULL)?(int)pBI->biWidth:0;
			}
			
		int Height()
			{
			I_ASSERT(pBI);
			
			return (pBI != NULL)?(int)pBI->biHeight:0;
			}
			
		DIBITMAP& operator= (const DIBITMAP& rhs);
 
		DWORD GetDataSize()	const
			{
			return (!Valid())?0:pBI->biSize + GetImageSize(pBI) +
				(sizeof(RGBQUAD) * nColorTableSize);
			}

		BOOL WriteData(
			HANDLE file);

	};

#endif
