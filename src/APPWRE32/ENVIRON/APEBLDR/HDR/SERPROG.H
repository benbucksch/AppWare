#if(!defined(__serprog_h))
#define __serprog_h

int  APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
//BOOL InitApplication(HANDLE);
//BOOL InitInstance(HANDLE, int);
long CALLBACK FrameWndProc(HWND, unsigned, WORD, LONG);
BOOL CALLBACK About(HWND, unsigned, WORD, LONG);
BOOL CALLBACK CloseEnumProc(HWND, LONG);

#endif
