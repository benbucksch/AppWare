#include "almsys.h"

#include <math.h>
#include "dibitmap.h"

/////////////////////////////////////////////////////////////////
DIBITMAP::DIBITMAP()
	{
  Initialize();
  }

/////////////////////////////////////////////////////////////////
DIBITMAP::DIBITMAP(LPSTR resourceName, HINSTANCE hInst) 
	{
	HRSRC 						rcBitmap;

	Initialize();
	rcBitmap = FindResource(hInst, resourceName, RT_BITMAP);

	I_ASSERT(rcBitmap);

	if(rcBitmap)
		{
		hBmp = LoadResource(hInst, rcBitmap);
		I_ASSERT(hBmp);
		pBI = (LPBITMAPINFOHEADER)LockResource(hBmp);
    
    if(pBI)
    	BuildColorTable();
    else
    	{
      I_ASSERT(FALSE);
      }
		}
	}

////////////////////////////////////////////////////////////////////
DIBITMAP::DIBITMAP(
	HBITMAP bmp,
	HPALETTE hPal)
	{
  BITMAP objInfo;
  int bitCount;
  DWORD sizeColorTable;
  HDC hdc = GetDC(NULL);
  HPALETTE oldPal = NULL;
  
  Initialize();
  
  I_ASSERT(bmp);
  
  //first get the bitmap info
  GetObject(bmp, sizeof(objInfo), &objInfo);
  
  if(hPal)
  	{
  	oldPal = SelectPalette(hdc, hPal, FALSE);
  	RealizePalette(hdc);
    }
    
  bitCount = objInfo.bmPlanes * objInfo.bmBitsPixel;
  sizeColorTable = (1 << bitCount) * sizeof(RGBQUAD);
  
  //fist allocate memory to hold header structure
  pBI = (LPBITMAPINFOHEADER)GlobalAlloc(GPTR, sizeof(BITMAPINFOHEADER) + sizeColorTable);
  
  pBI->biSize = sizeof(BITMAPINFOHEADER);
  pBI->biWidth = objInfo.bmWidth;
  pBI->biHeight = objInfo.bmHeight;
  pBI->biPlanes = 1;
  pBI->biBitCount = (WORD)(objInfo.bmPlanes * objInfo.bmBitsPixel);
  pBI->biCompression = BI_RGB;
  
  //now, set the size we'll need to hold the bits
  GetDIBits(hdc, bmp, 0, objInfo.bmHeight, NULL, (BITMAPINFO far*)pBI, DIB_RGB_COLORS);
  
  //now resize the structure and get the actual bits
  pBI = (LPBITMAPINFOHEADER)GlobalReAlloc(pBI, GlobalSize(pBI) + 
  	GetImageSize(pBI), GMEM_MOVEABLE|GMEM_ZEROINIT);
  
  if(pBI)
  	{
	  //now, get the bit info
  	GetDIBits(hdc, bmp, 0, objInfo.bmHeight, (void*)((char*)pBI + (WORD)pBI->biSize +
  		(WORD)sizeColorTable), (BITMAPINFO far*)pBI, DIB_RGB_COLORS); 
  	
  	BuildColorTable();
		}
  else
    I_ASSERT(FALSE);
  
  if(oldPal)
  	{
  	SelectPalette(hdc, oldPal, FALSE);
    RealizePalette(hdc);
    }
    
  ReleaseDC(NULL, hdc);
  
  }

////////////////////////////////////////////////////////////////////
DIBITMAP::DIBITMAP(LPBITMAPINFO dibInfo)
	{
  DWORD sizeColorTable;
  LPBITMAPINFOHEADER pHdr;
  DWORD copySize;
  
  I_ASSERT(dibInfo);
  Initialize();
  
  pHdr = (LPBITMAPINFOHEADER)dibInfo;
    
  sizeColorTable = CalcColorTableSize(pHdr);
  
  copySize = pHdr->biSize + GetImageSize(pHdr) + 
  	(DWORD)sizeof(RGBQUAD) * sizeColorTable;	
  
  pBI = (LPBITMAPINFOHEADER)GlobalAlloc(GPTR, copySize);
  	
  if(pBI)
  	{
  	CopyMemory(pBI, pHdr, copySize);
    pBI->biSize = sizeof(BITMAPINFOHEADER);	//make sure this field was set
    BuildColorTable();
  	}
  else
  	I_ASSERT(FALSE);  
  }

////////////////////////////////////////////////////////////////////
//expects a valid bitmap file	with the file pointer already positioned.
//Padding is the extra stuff that comes after the end of the bitmap.
DIBITMAP::DIBITMAP(
	HANDLE fileHandle,
	int padding)
  {
	DWORD readLength;
  DWORD cb;
  DWORD currentPos;
  BITMAPFILEHEADER bfh;
  
  Initialize();
    
	//verify file	and move pointer to start of info header
  I_VERIFY(ReadFile(fileHandle, &bfh, sizeof(bfh), &cb, NULL));
  
  //Only check the type because the fileLength may be the data length and
  //not the file length if we wrote the header.
  if(cb == sizeof(bfh) && 
  	bfh.bfType == (('M'<<8) + 'B'))
    {
    //dont count on the bfSize member to get the size since it doesn't
    //always seem to be set up correctly by other apps.
    currentPos = SetFilePointer(fileHandle, 0, NULL,  FILE_CURRENT);
    readLength = SetFilePointer(fileHandle, 0, NULL, FILE_END) - currentPos - padding;
    //seek back to original position.
    SetFilePointer(fileHandle, currentPos, NULL, FILE_BEGIN);

		pBI = (LPBITMAPINFOHEADER)GlobalAlloc(GPTR, readLength);

		if(pBI)
    	{
      I_VERIFY(ReadFile(fileHandle, pBI, readLength, &cb, NULL));
	    if(cb ==	readLength)
  	    {
		    //at this point, we may need to do something stupid like converting
		    //our bitmap to a bitmap info structure from a coreinfo structure.
				if(pBI->biSize == sizeof(BITMAPCOREHEADER))
		    	CoreToInfo();

    	  BuildColorTable();
      	}
    	else
    		{
      	I_ASSERT(FALSE);
      	}
      }
    else
    	I_ASSERT(FALSE);
    }
  else
  	{	
    I_ASSERT(FALSE);
    }
  }

////////////////////////////////////////////////////////////////////
DIBITMAP::~DIBITMAP()
	{
	FreeMemory();
	}

////////////////////////////////////////////////////////////
void DIBITMAP::BuildColorTable()
	{		
	LPRGBQUAD					colTable;
	LPLOGPALETTE			lp;
	LPPALETTEENTRY		lpe;
	int								i;
		
	I_ASSERT(pBI);
  
  if(pBI)
  	{
		colTable = (LPRGBQUAD)((LPSTR)pBI + (unsigned int)pBI->biSize);		
			
		nColorTableSize = CalcColorTableSize(pBI);
      
		lp = (LPLOGPALETTE)GlobalAlloc(GPTR, sizeof(LOGPALETTE) + 
			(sizeof(PALETTEENTRY) * ((unsigned int)nColorTableSize - 1)));

		I_ASSERT(lp);																

		if(lp)
			{
			lp->palVersion = 0x300;
			lp->palNumEntries = (WORD)nColorTableSize;
			lpe = lp->palPalEntry;
		
			for(i=0;i< (unsigned int)nColorTableSize;i++)
				{
				lpe[i].peRed = colTable[i].rgbRed;
				lpe[i].peGreen = colTable[i].rgbGreen;
				lpe[i].peBlue = colTable[i].rgbBlue;
				}		
					
			hPal = CreatePalette(lp);
			
			GlobalFree(lp);
			}
		}
  }


////////////////////////////////////////////////////////////////////
//returns number of colors used
int DIBITMAP::Use(HDC hdc)
	{
	int rVal = 0;

	HPALETTE tempPal;

	if(hPal && (palDC == NULL))
		{
		tempPal = SelectPalette(hdc, hPal, 0);
		if(!oldPal)
			oldPal = tempPal;
		
		RealizePalette(hdc);

		palDC = hdc;
		rVal = TRUE;
		}

	return rVal;
	}

/////////////////////////////////////////////////////////////////
BOOL DIBITMAP::CleanUp()
	{
	BOOL rVal = FALSE;

	if(palDC && hPal)
		{
		SelectPalette(palDC, oldPal, 0);
		RealizePalette(palDC);
		palDC = NULL;
		rVal = TRUE;
		}

	return rVal;
	}
		
//////////////////////////////////////////////////////////////////
BOOL DIBITMAP::DrawIfPaletteChanged(
	HDC hdc,
	int x,
	int y,
	LPRECT bounds)
	{
	return DoDraw(hdc, x, y, FALSE, bounds);
	}

void DIBITMAP::Draw(
	HDC hdc,
	int x,
	int y,
	LPRECT bounds)
	{
	DoDraw(hdc, x, y, TRUE, bounds);
	}

////////////////////////////////////////////////////////////////////////
//if bounds is not NULL, the image is stretched to the bounds
BOOL DIBITMAP::DoDraw(
	HDC hdc,
	int x,
	int y,
	BOOL forceDraw,
	LPRECT bounds)
	{
  void*        pBits;
	BOOL rval = FALSE;

	I_ASSERT(pBI);

	if(pBI)
		{
		if(Use(hdc) > 0 || forceDraw)
			{
	  	pBits = (void*)((char*)pBI + (sizeof(BITMAPINFOHEADER) + (nColorTableSize * sizeof(RGBQUAD))));
		
    	if(!bounds)
      	{
				SetDIBitsToDevice(hdc, x, y, (int)pBI->biWidth, (int)pBI->biHeight, 
					0, 0, 0, (int)pBI->biHeight, pBits, (LPBITMAPINFO)pBI, DIB_RGB_COLORS);
				}
      else
      	{
        StretchDIBits(hdc, x, y,
        	bounds->right - bounds->left + 1,
          bounds->bottom - bounds->top + 1,
          0, 0, (int)pBI->biWidth, (int)pBI->biHeight,
          pBits, (LPBITMAPINFO)pBI, DIB_RGB_COLORS, SRCCOPY);
        }
        
			CleanUp();

			rval = TRUE;
			}
		}

	return rval;
	}
  
//////////////////////////////////////////////////////////////////////////
DIBITMAP& DIBITMAP::operator= (const DIBITMAP& rhs)
	{
  DWORD size;
  
  if(&rhs == this)
  	return *this;
    
  if(pBI)
  	CleanUp();
  
  Initialize();
  																  
  if(rhs.Valid())
  	{
	  if(rhs.pBI)
	  	{
	  
	  	if(rhs.pBI->biCompression == BI_RGB)
	  		{
	   		 size = rhs.GetDataSize();
	        
	      pBI = (LPBITMAPINFOHEADER)GlobalAlloc(GPTR, size);
	      I_ASSERT(pBI);
	      
	      CopyMemory((void*)pBI, (void*)rhs.pBI, size);
	      
	      BuildColorTable();
	    	}
	    else
	    	{
			  I_ASSERT(FALSE);      
	      MessageBeep(MB_ICONEXCLAMATION);
	    	MessageBox(GetActiveWindow(), "AppWare does not support compressed bitmaps.",
	    		"Bitmap Error", MB_TASKMODAL|MB_OK|MB_ICONEXCLAMATION);
        }
	    }
	  else
	  	{
	    I_ASSERT(FALSE);	    
	    }
    }
  
  return *this;
  }


/////////////////////////////////////////////////////////////////////////
void DIBITMAP::Initialize()
	{
	oldPal = NULL;
	palDC = NULL;
	hPal = NULL;
	pBI = NULL;
	}	

//////////////////////////////////////////////////////////////////////
void DIBITMAP::FreeMemory()
	{
  if(pBI && !hBmp)
  	{
    //only free if we weren't pointing to a resource
    GlobalFree(pBI);  	
  	}
  	  
	if(hPal)
		DeleteObject(hPal);
    
	}

/////////////////////////////////////////////////////////////////////////
//idea - adjust the size of the memory by the delta of the colortable
//size + the delta of the header structure size.
//Then, fill in the new header, build the new color table, and slam in
//the data bits.
void DIBITMAP::CoreToInfo()
	{
  BITMAPINFOHEADER info;
  BITMAPCOREHEADER* pBC = (BITMAPCOREHEADER*)pBI;
  long headerDelta;
  long tableDelta;
  DWORD colorTableSize;
  DWORD newMemSize;
	LPRGBQUAD					infoTable;
	RGBTRIPLE*				coreTable;
	int								i;
	LPSTR							infoBits;
  LPSTR							coreBits;
  
  I_ASSERT(pBC);
    
  FillMemory(&info, sizeof(info), 0);
  headerDelta = sizeof(BITMAPINFOHEADER) - sizeof(BITMAPCOREHEADER);
  info.biSize = sizeof(BITMAPINFOHEADER);
  info.biWidth = (long)pBC->bcWidth;
  info.biHeight = (long)pBC->bcHeight;
  info.biPlanes = pBC->bcPlanes;
  info.biBitCount = pBC->bcBitCount;
  info.biCompression = BI_RGB;
  
  colorTableSize = DIBITMAP::CalcColorTableSize(&info);
  tableDelta = ((long)colorTableSize * 
  	((int)sizeof(RGBQUAD) - (int)sizeof(RGBTRIPLE)));
    
  newMemSize = GlobalSize(pBC) + headerDelta + tableDelta;
  pBI = (BITMAPINFOHEADER*)GlobalAlloc(GHND, newMemSize);
  
  I_ASSERT(pBI);
  		
  if(pBI)
  	{
  	CopyMemory(pBI, &info, sizeof(info));
  
  	//build the color table    
		infoTable = (LPRGBQUAD)((LPSTR)pBI + (unsigned int)pBI->biSize);		
		coreTable = (RGBTRIPLE*)((LPSTR)pBC + (unsigned int)pBC->bcSize);
			
		for(i=0;i< (unsigned int)colorTableSize;i++)
			{
			infoTable[i].rgbRed = coreTable[i].rgbtRed;
			infoTable[i].rgbGreen = coreTable[i].rgbtGreen;
			infoTable[i].rgbBlue = coreTable[i].rgbtBlue;
      infoTable[i].rgbReserved = 0;
			}		
		
    infoBits = (LPSTR)&(infoTable[i]);
    coreBits = (LPSTR)&(coreTable[i]);
    
		//now, copy over the bits
		CopyMemory(infoBits, coreBits, newMemSize - pBI->biSize -
			(colorTableSize * sizeof(RGBQUAD)));
		
		GlobalFree(pBC);		 			
 		}
  }

/////////////////////////////////////////////////////////////////////
DWORD DIBITMAP::CalcColorTableSize(
	LPBITMAPINFOHEADER pDibHdr)
  {
  DWORD rVal;
  
  if(pDibHdr->biBitCount == 24)
    rVal = 0;
  else if(pDibHdr->biClrUsed == 0)
  	rVal = 1 << pDibHdr->biBitCount;
  else 
  	rVal = pDibHdr->biClrUsed;
  
  return rVal;
  }
  
//////////////////////////////////////////////////////////////////////
BOOL DIBITMAP::WriteData(
	HANDLE hFile)
	{
  BOOL rVal = TRUE;
  BITMAPFILEHEADER bfh;
  DWORD cb;
  																											
  if(Valid())
  	{
  	bfh.bfType = (('M'<<8) + 'B');
    //divide by 4 because that's the way win32 is.
  	bfh.bfSize = (GetDataSize() + sizeof(bfh)) / 4;
  	bfh.bfReserved1 = 0;
  	bfh.bfReserved2 = 0;
  	bfh.bfOffBits = sizeof(bfh) + pBI->biSize + (sizeof(RGBQUAD) *
  		nColorTableSize);
  	
    I_VERIFY(WriteFile(hFile,&bfh,sizeof(bfh),&cb,NULL));
    if(cb == sizeof(bfh))
    	{
      I_VERIFY(WriteFile(hFile, pBI, GetDataSize(), &cb, NULL));
      if(cb != GetDataSize())	
      	rVal = FALSE;
    	}
   	else
   		{
   		I_ASSERT(FALSE);
   		rVal = FALSE;
   		}  
  	}
  
  return rVal;  
  }
		
///////////////////////////////////////////////////////////////////
//static routine - necessary since the biSizeImage field may not 
// always be set.
DWORD DIBITMAP::GetImageSize(LPBITMAPINFOHEADER infoHdr)
	{
  //bitmaps are DWORD aligned on the scanline
  // >> 3 divides by 8.
  return  ((((infoHdr->biWidth * infoHdr->biBitCount) + 31) 
  	& ~31) >> 3) * infoHdr->biHeight;
  }
