#ifndef _ITEMOBJ_H_
#define _ITEMOBJ_H_

#define ITEM_VERSION  103 // new version as of 2-26-93 TJW
#define ITEM_SIGNALSFIXED 103 //version where signals were changed to
// Signal Identifiers

#define SIGNAL_ITMCHOSEN 		0
#define SIGNAL_ITMTOGON			1
#define SIGNAL_ITMTOGOFF		2

// old signal identifiers that did not match Mac.

#define SIGNAL_OLDITMCHOSEN 1
#define SIGNAL_OLDITMTOGON  2
#define SIGNAL_OLDITMTOGOFF 3


//detail field valued for item objectchange events.

#define ITEMCHANGED_DISABLE
#define ITEMCHANGED_ENABLE
#define ITEMCHANGED_ON
#define ITEMCHANGED_OFF


//Item status values

#define ITEM_NEVER_EDITED    0x00000001
#define ITEM_DISABLED        0x00000002
#define ITEM_TOGGLED_ON      0x00000004
#define ITEM_TOGGLE_TYPE     0x00000008
#define ITEM_SHOW_CHECKMARK   0x00000010

#define ITEM_MAX_TEXT        80

struct ITEMINFO       			// data 0 storage
  {
	int 		  version;
  long      status;
	char		  accelKey;		    // windows virtual key code
  char      accelModifier;  // OWND_SHIFT|OWND_CTRL
  short     itemID;
  };

typedef ITEMINFO far*  LPITEMINFO;

#define  ITEM_INFO_DATA          0
#define  ITEM_TITLE_DATA         1  //keep the same as MENU_TITLE_DATA (menuobj.h)
#define  ITEM_TOGGLE_TITLE_DATA  2

#define  ITEM_DATAS_CNT          3



#endif