#define OTYPE_RADIOGROUP 				MAKETYPEID('R','d','g','p')

#ifdef _MAIN_MODULE
HANDLE					hInstance = (HINSTANCE)-1;
HANDLE					hPrevInstance = (HINSTANCE)-1;
int							nCmdShow = -1;
HANDLE  				userDS = 0;
unsigned	windowsVersion;
int Object_in_items_array(
  OBJECTID      object,
  LPOBJECTID    items,
	int           numItems);
BOOL EraseItemFromArray(
  int     idx,
  int     size,
  int     max,
	LPVOID  array);
////////////////////////

void PostObjectChanged(
  OBJECTID     theObject,
	LONG         modifiers = 0L, //defaults to 0
	LONG				 how 			 = 0L, //defaults to 0
	LONG 				 time 		 = 0L);//defaults to 0

#else
extern HANDLE		hInstance;
extern HANDLE		hPrevInstance;
extern int			nCmdShow;
extern HANDLE		userDS;
extern unsigned	windowsVersion;
extern int Object_in_items_array(
  OBJECTID      object,
  LPOBJECTID    items,
	int           numItems);
extern BOOL EraseItemFromArray(
  int     idx,
  int     size,
  int     max,
	LPVOID  array);
extern void PostObjectChanged(
  OBJECTID     theObject,
	LONG         modifiers = 0L, //defaults to 0
	LONG				 how			 = 0L,       //defaults to 0
	LONG 				 time			 = 0L);      //defaults to 0

#endif

