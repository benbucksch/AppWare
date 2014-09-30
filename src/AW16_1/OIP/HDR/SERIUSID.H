#ifndef _SERIUSID_
#define _SERIUSID_

/////////////////////
// Object Type IDs //
/////////////////////

//#define OTYPE_WINDOW										MAKEID('W','i','n','o')
#define OTYPE_MENUBAR										MAKEID('M','b','a','r')
#define OTYPE_MENU											MAKEID('M','e','n','u')
#define OTYPE_MENUITEM									MAKEID('M','i','t','m')
//#define OTYPE_BUTTON										MAKEID('B','u','t','n')
//#define OTYPE_TEXT											MAKEID('T','e','x','t')
//#define OTYPE_NUMBER										MAKEID('N','m','b','r')
#define OTYPE_LIST											MAKEID('L','i','s','t')
#define OTYPE_CONTROL										MAKEID('C','n','t','l')
//#define OTYPE_PICTURE										MAKEID('P','i','c','t')
//#define OTYPE_BUNDLE										MAKEID('B','n','d','l')
#define OTYPE_CD												MAKEID('C','d','a','u')
#define OTYPE_OIPFILE										MAKEID('F','i','l','e')
#define OTYPE_NETWORK										MAKEID('W','k','s','t')
//#define OTYPE_DATE											MAKEID('D','a','t','e')
//#define OTYPE_TIME											MAKEID('T','i','m','e')
//#define OTYPE_LAUNCHER									MAKEID('A','p','p','l')
//#define OTYPE_PRINTER										MAKEID('P','r','n','t')
#define OTYPE_SOUND											MAKEID('S','o','n','d')
//#define OTYPE_DATABASE									MAKEID('D','a','t','b')
#define OTYPE_SCANNER										MAKEID('S','c','a','n')
#define OTYPE_PAINTER										MAKEID('P','n','t','r')
#define OTYPE_PALETTE										MAKEID('P','l','t','t')
#define OTYPE_ANIMATOR									MAKEID('A','n','i','m')
#define OTYPE_WORDPUBLISHER							MAKEID('W','p','u','b')
#define OTYPE_SPREADSHEET								MAKEID('S','p','r','s')
#define OTYPE_SCROLLPANEL								MAKEID('S','c','p','l')
#define OTYPE_MAGICBUTTON								MAKEID('M','b','u','t')
#define OTYPE_CONNECTION								MAKEID('C','o','n','n')
#define OTYPE_TERMINAL									MAKEID('T','e','r','m')
#define OTYPE_FILETRANSFER							MAKEID('F','t','n','s')
#define OTYPE_EXPRESSION								MAKEID('C','a','l','c')
//#define OTYPE_BROWSER										MAKEID('B','r','w','s')
#define OTYPE_DIAL											MAKEID('S','c','a','l')
#define OTYPE_PAGE											MAKEID('P','a','g','e')
//#define OTYPE_LOOP											MAKEID('L','o','o','p')
#define OTYPE_GRAPH											MAKEID('G','r','p','h')
#define OTYPE_RADIOGROUP								MAKEID('R','d','g','p')
//#define OTYPE_SUB												MAKEID('S','u','b','r')
//#define OTYPE_GROUP											MAKEID('G','r','u','p')
//#define OTYPE_NOTIFY										MAKEID('N','t','f','y')
#define OTYPE_REPORT										MAKEID('R','p','r','t')
#define OTYPE_TEXTMAKER									MAKEID('T','f','c','t')
#define OTYPE_TABLE											MAKEID('T','a','b','l')
#define OTYPE_SYSEX											MAKEID('M','i','d','i')
#define OTYPE_XCMD											MAKEID('X','c','m','d')
#define OTYPE_IMPORTEXPORT							MAKEID('I','m','e','x')
#define OTYPE_NUMBERSPEAK								MAKEID('N','s','p','k')
#define OTYPE_MOVIE											MAKEID('M','o','o','V')
#define OTYPE_CONTROLLER								MAKEID('M','C','t','l')
#define OTYPE_CURSOR										MAKEID('A','c','u','r')
#define OTYPE_SQLNK											MAKEID('S','q','l','d')


// Move the following #defines into the appropriate object's o_<obj>.h file

//////////////////////
//// Menu Event IDs //
//////////////////////
//
//#define OMENU_EVT_DRAWMBAR							MAKEID('d','r','m','B')
//#define OMENU_EVT_INSTALLMENUBAR				MAKEID('i','n','m','B')
//#define OMENU_EVT_INSTALLMENU						MAKEID('i','n','m','N')
//#define OMENU_EVT_REMOVEMENU						MAKEID('r','m','m','N')
//#define OMENU_EVT_CLEARMBAR							MAKEID('c','l','m','B')
//#define OMENU_EVT_DISABLEMENUBAR				MAKEID('d','s','m','B')
//#define OMENU_EVT_DISABLEMENU						MAKEID('d','s','m','N')
//#define OMENU_EVT_ENABLEMENUBAR					MAKEID('e','n','m','B')
//#define OMENU_EVT_ENABLEMENU						MAKEID('e','n','m','N')
//#define OMENU_EVT_HIDEMENUBAR						MAKEID('h','d','m','B')
//#define OMENU_EVT_SHOWMENUBAR						MAKEID('s','h','m','B')
//#define OMENU_EVT_KILLHILITES						MAKEID('k','m','h','L')
//#define OMENU_EVT_MENUCHOSEN						MAKEID('m','n','c','H')
//#define OMENU_EVT_SETMENUITEM						MAKEID('s','i','t','M')
//#define OMENU_EVT_MENUITEMCHOSEN				MAKEID('m','i','c','H')
//#define OMENU_EVT_TOGGLEMENUITEMON			MAKEID('t','m','i','N')
//#define OMENU_EVT_TOGGLEMENUITEMOFF			MAKEID('t','m','i','F')
//
////////////////////////
//// Window Event IDs //
////////////////////////
//
//#define OWND_EVT_WINDOWSTARTUP					MAKEID('s','t','r','W')
//#define OWND_EVT_TRACKWINDOWSCROLL			MAKEID('s','c','l','W')
//#define OWND_EVT_OPENWINDOWOBJECT				MAKEID('o','p','n','W')
//#define OWND_EVT_CLOSEWINDOWOBJECT			MAKEID('c','l','s','W')
//#define OWND_EVT_CENTERWINDOWOBJECT			MAKEID('c','e','n','W')
//#define OWND_EVT_MOVEWINDOWOBJECT				MAKEID('m','v','e','W')
//#define OWND_EVT_SIZEWINDOWOBJECT				MAKEID('s','i','z','W')
//#define OWND_EVT_DISPOSEWINDOWEVENT			MAKEID('d','s','p','W')
//#define OWND_EVT_ZOOMWINDOWOBJECT				MAKEID('z','o','m','W')
//#define OWND_EVT_UPDATEWINDITEMINFO			MAKEID('u','p','d','I')
//#define OWND_EVT_DRAWWINDITEM						MAKEID('d','r','w','I')
//#define OWND_EVT_ENDUPDATEWINDITEMS			MAKEID('e','n','d','U')
//#define OWND_EVT_WINDITEMHIT						MAKEID('i','h','i','T')
//#define OWND_EVT_WINDMOUSEUP						MAKEID('w','m','s','U')
//#define OWND_EVT_WINDITEMDOUBLECLICK		MAKEID('i','d','b','L')
//#define OWND_EVT_WINDITEMKEY						MAKEID('i','k','e','Y')
//#define OWND_EVT_WINDITEMKEYUP					MAKEID('i','k','y','U')
//#define OWND_EVT_ACTIVATEWINDITEM				MAKEID('i','a','c','T')
//#define OWND_EVT_DEACTIVATEWINDITEM			MAKEID('i','d','c','T')
//#define OWND_EVT_HIDEWINDITEM						MAKEID('h','i','d','I')
//#define OWND_EVT_SHOWWINDITEM						MAKEID('s','h','o','I')
//#define OWND_EVT_DISPOSEWINDITEM				MAKEID('d','s','p','I')
//#define OWND_EVT_RESUMEWINDITEM					MAKEID('i','r','s','M')
//#define OWND_EVT_SUSPENDWINDITEM				MAKEID('i','s','p','D')
//#define OWND_EVT_CONFIRMWINDITEM				MAKEID('i','c','n','F')
//#define OWND_EVT_CMDKEYWINDITEM					MAKEID('i','c','m','K')
//#define OWND_EVT_MOVEWINDITEM						MAKEID('m','o','v','I')
//#define OWND_EVT_ACTIVATEITEM						MAKEID('a','c','t','I')
//#define OWND_EVT_EDITUNDO								MAKEID('i','u','d','O')
//#define OWND_EVT_EDITCUT								MAKEID('i','c','u','T')
//#define OWND_EVT_EDITCOPY								MAKEID('i','c','p','Y')
//#define OWND_EVT_EDITPASTE							MAKEID('i','p','s','T')
//#define OWND_EVT_WINDOWFOCUS						MAKEID('w','i','n','F')
//#define OWND_EVT_WINDOWIDLE							MAKEID('w','i','n','I')
//#define OWND_EVT_WINDITEMDRAWROUTINE		MAKEID('w','i','d','r')
//#define OWND_EVT_WINDITEMPRINTROUTINE		MAKEID('w','i','p','r')
//#define OWND_EVT_GETWINDOWATTRIBUTES		MAKEID('g','a','t','t')
//#define OWND_EVT_DRAWWINDCONTROLITEM		MAKEID('d','w','c','I')
//#define OWND_EVT_WINDMENU								MAKEID('w','m','n','U')
//#define OWND_EVT_GETMENUHANDLE					MAKEID('g','m','n','H')
//#define OWND_EVT_WINDISPOPUP						MAKEID('w','m','n','P')
//#define OWND_EVT_MENUDESTROY						MAKEID('m','n','u','D')
//#define OWND_EVT_GETACCELKEYS						MAKEID('g','a','c','L')
//#define OWND_EVT_ACCELKEYHIT						MAKEID('a','c','c','L')
//#define OWND_EVT_WINDCOMMAND						MAKEID('w','c','m','D')
//#define OWND_EVT_WINDENTERTHIEF					MAKEID('w','e','n','T')
//#define OWND_EVT_WINDITEMHSCROLL				MAKEID('w','i','h','S')
//#define OWND_EVT_WINDITEMVSCROLL				MAKEID('w','i','v','S')
//#define OWND_EVT_WINDITEMMOUSEMOVE			MAKEID('w','i','m','M')
//#define OWND_EVT_WINDITEMMOUSEENTRY			MAKEID('w','i','m','E')
//#define OWND_EVT_ITEMGETSELECTION				MAKEID('i','g','s','L')
//#define OWND_EVT_ITEMSETSELECTION				MAKEID('i','s','s','L')
//
//////////////////////
//// Text Event IDs //
//////////////////////
//
//#define OTEXT_EVT_TEXTEDITED						MAKEID('t','x','t','E')
//#define OTEXT_EVT_ACTIVETEIDLE					MAKEID('i','i','d','L')
//
//////////////////////
//// Date Event IDs //
//////////////////////
//
//#define ODATE_EVT_DATEUPDATE						MAKEID('d','t','u','P')
//
///////////////////////
////	Time Event IDs //
///////////////////////
//
//#define OTIME_EVT_TIMEUPDATE						MAKEID('t','m','u','P')
//
/////////////////////////
///// Button Event IDs //
/////////////////////////
//
//#define OBUTN_EVT_TRACKBUTTON						MAKEID('t','r','k','B')
//
/////////////////////////
///// Cursor Event IDs //
/////////////////////////
//
//#define OCURS_EVT_SETTHECURSOR					MAKEID('s','C','u','r')
//#define OCURS_EVT_STARTANIMATION				MAKEID('s','A','n','i')
//#define OCURS_EVT_KILLANIMATION					MAKEID('k','A','n','i')


#endif
