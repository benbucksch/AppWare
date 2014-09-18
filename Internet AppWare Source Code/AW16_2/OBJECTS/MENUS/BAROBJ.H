#ifndef _BAROBJ_H_
#define _BAROBJ_H_

#define BAR_DATA_CNT 2

#define BAR_INFO_DATA       0
#define BAR_MENULIST_DATA   1
#define BAR_VERSION         103

#define BAR_MAX_TEXT        80

struct BARINFO               	// data 0 storage
  {
	int			version;
  long    status;
	WORD		menuCount;
	WORD		testWinHz;
  HWND    hTestWindow;
  };

typedef BARINFO far*  LPBARINFO;

#define  BAR_DISABLED  0x00000001

struct MENUENTRY
  {
  OBJECTID  id;
  short     status;
  };

typedef MENUENTRY far*   LPMENUENTRY;

#define ITEMENTRY_DELETEENTRY  0x0010

struct BARRUNTIME
  {
  HMENU  hMenu;
  };


#endif
