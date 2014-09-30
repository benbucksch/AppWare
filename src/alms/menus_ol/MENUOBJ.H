#ifndef _MENUOBJ_H_
#define _MENUOBJ_H_

#define MENU_VERSION       200
#define MENU_SIGNALSFIXED  103 //version where signals were changed to


#define  MENU_INFO_DATA        0
#define  MENU_TITLE_DATA       1 //keep the same as ITEM_TITLE_DATA (itemobj.h)
#define  MENU_ITEMLIST_DATA    2

#define  MENU_DATAS_CNT        3

#define  MENU_MAX_TEXT        80
#define  ACCEL_MAX_TEXT       25
         
// Defines for signal ids

#define  SIGNAL_ITMCHOSEN 		  0
#define  SIGNAL_NOITEMCHOSEN		1

#define  SIGNAL_OLDITMCHOSEN 1


// Defines for status bits

#define  MENU_NEVER_EDITED    0x00000001
#define  MENU_DISABLED        0x00000002
#define  MENU_POPUP           0x00000004


struct MENUINFO 					// data 0 storage
  {
	int				version;
	WORD			itemCount;
	WORD			popIndex;  		// 0 based; current selection for popup menus
	AOBJECTID	sendText;
	AOBJECTID  sendIndex;
  long    	status;
  };

typedef MENUINFO *  LPMENUINFO;



//structure for item list

struct ITEMENTRY
  {
  AOBJECTID  id;
  short     status;
  UINT      menuItemID;
  };

typedef ITEMENTRY *  LPITEMENTRY;

#define ITEMENTRY_TEXTONLY     0x0001
#define ITEMENTRY_MENUITEM     0x0002
#define ITEMENTRY_MENU         0x0004
#define ITEMENTRY_SEPARATOR    0x0008
#define ITEMENTRY_DELETEENTRY  0x0010
#define ITEMENTRY_DISABLED     0x0020
#define ITEMENTRY_FOREIGN      0x0040
#define ITEMENTRY_FOREIGNMENU  0x0080

//structure for type data


struct MENUTYPEINFO
  {
  UINT  idSeed;
  };

typedef MENUTYPEINFO *  LPMENUTYPEINFO;

#define MENUTYPE_INFO_DATA  0

#endif
