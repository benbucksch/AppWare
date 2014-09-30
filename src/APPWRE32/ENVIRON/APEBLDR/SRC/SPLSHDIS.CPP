//////////////////////////////////////////////////////////////////////////
// For each module that this source lives in, we want to have a unique
// class name for the window class.  However, we can not keep a static
// boolean variable to tell us whether or not we have registered the class
// for the following reason:  The class belongs to the app, and if the app
// terminates, the class is unregistered.  However, if another app is
// started up before the dll is closed, the flag will save that the 
// class is registered.
// The static variable SPD_CLASS_NAME will be the base name while
// the static varibale SPD_APP_CLASS_NAME will contain the modified name
// unique to whatever module started up the app.
#include "almsys.h"

#include <windowsx.h>
#include "splshdis.h"
#include "intdebug.h"

WINDOWLIST* SPLASHDISPLAY::winList = NULL;

#define SPLASHDISPLAY_CLASS_BASE_NAME "SPLASHDISPLAY_Window"
static char SPD_CLASS_NAME[] = SPLASHDISPLAY_CLASS_BASE_NAME;
//at least length of SPLASHDISPLAY_CLASS_NAME plus SPLASH_FILE_LENGTH
static char SPD_APP_CLASS_NAME[sizeof(SPD_CLASS_NAME) + 
	SPLASH_FILE_LENGTH] = SPLASHDISPLAY_CLASS_BASE_NAME;

#define SPLASH_TAG AMAKEID('S','P','L','S')
#define SPLASH_DEFAULT_TIME 2

struct SPLASHDISPLAY_FOOTER
	{
  long timeDelay;
	long tag;
  long offset;
  };
  
///////////////////////////////////////////////////////////////
//don't create the window list in the constructor but create
//it from the window proc.  This will insure that the module
//creating the window will have access to the correct window list.
SPLASHDISPLAY::SPLASHDISPLAY():hWndSplash(NULL)
	{
	splashPtr = new DIBITMAP();
  timeDelay = SPLASH_DEFAULT_TIME;
	}


///////////////////////////////////////////////////////////////
SPLASHDISPLAY::~SPLASHDISPLAY()
	{
  I_ASSERT(!winList);
  
  delete splashPtr;
  }

///////////////////////////////////////////////////////////////
void SPLASHDISPLAY::ShowSplashScreen()
	{
	WNDCLASS wc;
	RECT clientRect;
	char buffer[256];
  HINSTANCE hInst;
  char* namePtr;
  
  hInst = GetModuleHandle(NULL);

	if(!hWndSplash)
  	{
		if(splashPtr->Valid())
	  	{
	    if(!GetClassInfo(hInst, SPD_APP_CLASS_NAME, &wc))
				{
        //modify the class name in case it's already been registered 
        //from a different module.
        lstrcpy(SPD_APP_CLASS_NAME, SPD_CLASS_NAME);
        
				GetModuleFileName(hInst, buffer, sizeof(buffer)-1);
        
        //add on the last 13 characters of the module name
        if(lstrlen(buffer) < 13)
        	namePtr = buffer;
        else
        	namePtr = (buffer + lstrlen(buffer) - 13);
          
        lstrcat(SPD_APP_CLASS_NAME, namePtr);
        	        
				wc.style 					= 0;
				wc.lpfnWndProc 		= &SPLASHDISPLAY::WndProc;
				wc.cbClsExtra 		= 0;
				wc.cbWndExtra 		= 0;
				wc.hInstance 			= hInst;
				wc.hIcon 					= NULL;
				wc.hCursor 				= LoadCursor(NULL, IDC_WAIT);
				wc.hbrBackground	= GetStockBrush(NULL_BRUSH);
				wc.lpszMenuName 	= NULL;
				wc.lpszClassName 	= SPD_APP_CLASS_NAME;

				RegisterClass((LPWNDCLASS)&wc);
				}

			clientRect.left = (GetSystemMetrics(SM_CXSCREEN) - splashPtr->Width())/2;
			clientRect.top =	(GetSystemMetrics(SM_CYSCREEN)- splashPtr->Height())/2;
			clientRect.right = clientRect.left + splashPtr->Width();
			clientRect.bottom = clientRect.top + splashPtr->Height();
			AdjustWindowRect((LPRECT)&clientRect, WS_POPUP, FALSE);

			hWndSplash = CreateWindow(SPD_APP_CLASS_NAME,(LPSTR)"",
				WS_POPUP|WS_VISIBLE,
				clientRect.left,
				clientRect.top,
				clientRect.right - clientRect.left,
				clientRect.bottom - clientRect.top,
				NULL,
				NULL,
				hInst,
				this);
        
			UpdateWindow(hWndSplash);
		
    	startTime = GetTickCount();
	   	}
    }
  else
  	I_ASSERT(FALSE);

	}

///////////////////////////////////////////////////////////////
void SPLASHDISPLAY::HideSplashScreen()
	{
	if(hWndSplash)
  	{
    while((GetTickCount() - startTime) < (GetTimeDelay() * 1000))
    	{
      Sleep((GetTimeDelay() * 1000) - (GetTickCount() - startTime));
      }
      
		DestroyWindow(hWndSplash);
		}
    
	hWndSplash = NULL;
	}

////////////////////////////////////////////////////////////////
LRESULT SPLASHDISPLAY::SplashWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	HDC hdc;
	PAINTSTRUCT ps;
	BOOL palChanged;
  
  
	switch(message)
		{
		case WM_PAINT:
			I_ASSERT(splashPtr);
			
			hdc = BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
			splashPtr->Draw(hdc, 0, 0);
			EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
			return 0;

		case WM_PALETTECHANGED:
			if(((HWND) wParam) == hwnd)
				break;
			//else fall though
		case WM_QUERYNEWPALETTE:
    	I_ASSERT(splashPtr);
			hdc = GetDC(hwnd);
			palChanged = splashPtr->DrawIfPaletteChanged(hdc, 0, 0);
			ReleaseDC(hwnd, hdc);
			
			if(palChanged)
				return TRUE;

			break;


		}


	return DefWindowProc(hwnd, message, wParam, lParam);
	}

/////////////////////////////////////////////////////////////////////////
//return FALSE if write error occurrs.
BOOL SPLASHDISPLAY::Write(
	HANDLE hFile)
	{
  BOOL rVal = TRUE;
  DWORD dataStart;
  SPLASHDISPLAY_FOOTER sf;
  DWORD cb;
  
  if(splashPtr && splashPtr->Valid())
  	{
    dataStart = GetFileSize(hFile, NULL);
    
  	rVal = splashPtr->WriteData(hFile);
		if(rVal)
    	{
      sf.timeDelay = GetTimeDelay();
      sf.offset = GetFileSize(hFile, NULL) - dataStart + sizeof(sf);
      sf.tag = SPLASH_TAG;
			//seek to end of file to write tag to beginning of data
      SetFilePointer(hFile, 0, NULL, FILE_END);
      
      I_VERIFY(WriteFile(hFile, &sf, sizeof(sf), &cb, NULL)); 
      if(cb != sizeof(sf))
      	{
      	rVal = FALSE;
      	} 
      }
		
		}
	return rVal;
  }

/////////////////////////////////////////////////////////////////////////
//footerOffset=0 is the offset from the end of the file where the footer
//begins.  Allows for additional info at the end of the file 
BOOL SPLASHDISPLAY::Read(HANDLE hFile, int footerOffset)
	{
  SPLASHDISPLAY_FOOTER sf;
  BOOL rVal = FALSE;
  DWORD cb;
  
  //first read in the footer.
  //seek to the end of file - sizeof(footer)-footerOffset
  SetFilePointer(hFile, -(int)sizeof(sf)-footerOffset, NULL, FILE_END); 
  
  //read the footer
  I_VERIFY(ReadFile(hFile, &sf, sizeof(sf), &cb, NULL));
  if(cb == sizeof(sf))
  	{
  	if(sf.tag == SPLASH_TAG)
    	{
      SetTimeDelay((WORD)sf.timeDelay);
      
      //seek to start of bitmap
      SetFilePointer(hFile, -sf.offset-footerOffset, NULL,FILE_END);
      
  		if(splashPtr)
    		delete splashPtr;
    
    	//sizeof(sf) + footerOffset is the padding after the bitmap
  		splashPtr = new DIBITMAP(hFile, sizeof(sf) + footerOffset);
  
  		rVal = splashPtr->Valid();
      }
  	}
    
  return rVal;
	}

///////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SPLASHDISPLAY::WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	SPLASHDISPLAY*				 		splashDisPtr;
	LPCREATESTRUCT 						cs;

  if((message == WM_CREATE) && (!SPLASHDISPLAY::winList))
  	winList = new WINDOWLIST();
	
	if (SPLASHDISPLAY::winList)
		{
		splashDisPtr = (SPLASHDISPLAY*)(SPLASHDISPLAY::winList->GetObject(hWnd));
		if (!splashDisPtr)
			{
			if (message == WM_CREATE)
				{
				cs = (LPCREATESTRUCT)lParam;
				SPLASHDISPLAY::winList->Add(hWnd, splashDisPtr =
					((SPLASHDISPLAY*)cs->lpCreateParams));
				return splashDisPtr->SplashWndProc(hWnd, message, wParam, lParam);
				}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		else
			{
			if(message == WM_DESTROY)
      	{
				SPLASHDISPLAY::winList->Remove(hWnd);
        if(SPLASHDISPLAY::winList->IsListEmpty())
        	{
          delete SPLASHDISPLAY::winList;
          SPLASHDISPLAY::winList = NULL;
          }
        }
			return splashDisPtr->SplashWndProc(hWnd, message, wParam, lParam);
			}
		}
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

