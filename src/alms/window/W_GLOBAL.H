////////////////////////////////
// This file contains the global variables
// used by the window object

///////////////////////////////

#ifndef W_GLOBAL_H
#define W_GLOBAL_H

#ifdef I_MAIN
HINSTANCE g_hInstance;
LPSTR g_windowObjectClassName = "AppwareWND";

LPSTR g_szUpsf_version           = "VERSION";
LPSTR g_szUpsf_title             = "TITLE";
LPSTR g_szUpsf_titlebar          = "TITLEBAR";
LPSTR g_szUpsf_sysmenu           = "SYSMENU";
LPSTR g_szUpsf_maxbutton         = "MAXBUTT";
LPSTR g_szUpsf_minbutton         = "MINBUTT";
LPSTR g_szUpsf_horzscroll        = "HSCROLL";
LPSTR g_szUpsf_vertscroll        = "VSCROLL";
LPSTR g_szUpsf_resize            = "RESIZE";
LPSTR g_szUpsf_dialogframe       = "DFRAME";
LPSTR g_szUpsf_modal             = "MODAL";
LPSTR g_szUpsf_ontop             = "ONTOP";
LPSTR g_szUpsf_initvisible       = "INITVIS";
LPSTR g_szUpsf_drawsmooth        = "DRSMOOTH";
LPSTR g_szUpsf_closeonexit       = "CLSEXIT";
LPSTR g_szUpsf_initrect          = "INITRECT";
LPSTR g_szUpsf_limitsize         = "LIMITSIZ";
LPSTR g_szUpsf_maxsize           = "MAXSIZE";
LPSTR g_szUpsf_minsize           = "MINSIZE";
LPSTR g_szUpsf_scrollsize        = "SCRLSIZE";
LPSTR g_szUpsf_menuid            = "MENUID";
LPSTR g_szUpsf_nextsignal        = "NEXTSIG";
LPSTR g_szUpsf_bg_pattern        = "BG-PATRN";
LPSTR g_szUpsf_bg_fgcolor        = "BG-FGCOL";
LPSTR g_szUpsf_bg_fgdefault      = "BG-FGDEF";
LPSTR g_szUpsf_bg_bgcolor        = "BG-BGCOL";
LPSTR g_szUpsf_bg_bgdefault      = "BG-BGDEF";

LPSTR g_szUpsf_item_scope        = "ITEMSCOP";
LPSTR g_szUpsf_item_id           = "ITEMID";
LPSTR g_szUpsf_item_shape        = "ISSHAPE";
LPSTR g_szUpsf_item_group        = "ISGROUP";
LPSTR g_szUpsf_item_typeid       = "SHPLIBTP";
LPSTR g_szUpsf_item_rect         = "RECT";
LPSTR g_szUpsf_item_visible      = "VISIBLE";
LPSTR g_szUpsf_item_tabstop      = "TABSTOP";
LPSTR g_szUpsf_item_groupstop    = "GRPSTOP";
LPSTR g_szUpsf_item_locked       = "LOCKED";
LPSTR g_szUpsf_item_growsflag    = "GROWS";
LPSTR g_szUpsf_item_signalid     = "SIGNALID";
LPSTR g_szUpsf_item_extrascope   = "XDATSCOP";
LPSTR g_szUpsf_shape_version     = "VERSION";  // Shape library version
LPSTR g_szUpsf_shape_type        = "TYPE";     // Shape type
LPSTR g_szUpsf_shape_dll         = "ShapeDLL"; // Shape dll name

#else

extern HINSTANCE g_hInstance;
extern LPSTR g_windowObjectClassName;

extern LPSTR g_szUpsf_version;
extern LPSTR g_szUpsf_title;
extern LPSTR g_szUpsf_titlebar;
extern LPSTR g_szUpsf_sysmenu;
extern LPSTR g_szUpsf_maxbutton;
extern LPSTR g_szUpsf_minbutton;
extern LPSTR g_szUpsf_horzscroll;
extern LPSTR g_szUpsf_vertscroll;
extern LPSTR g_szUpsf_resize;
extern LPSTR g_szUpsf_dialogframe;
extern LPSTR g_szUpsf_modal;
extern LPSTR g_szUpsf_ontop;
extern LPSTR g_szUpsf_initvisible;
extern LPSTR g_szUpsf_drawsmooth;
extern LPSTR g_szUpsf_closeonexit;
extern LPSTR g_szUpsf_initrect;
extern LPSTR g_szUpsf_limitsize;
extern LPSTR g_szUpsf_maxsize;
extern LPSTR g_szUpsf_minsize;
extern LPSTR g_szUpsf_scrollsize;
extern LPSTR g_szUpsf_menuid;
extern LPSTR g_szUpsf_nextsignal;
extern LPSTR g_szUpsf_bg_pattern;
extern LPSTR g_szUpsf_bg_fgcolor;
extern LPSTR g_szUpsf_bg_fgdefault;
extern LPSTR g_szUpsf_bg_bgcolor;
extern LPSTR g_szUpsf_bg_bgdefault;

extern LPSTR g_szUpsf_item_scope;
extern LPSTR g_szUpsf_item_id;
extern LPSTR g_szUpsf_item_shape;
extern LPSTR g_szUpsf_item_group;
extern LPSTR g_szUpsf_item_typeid;
extern LPSTR g_szUpsf_item_rect;
extern LPSTR g_szUpsf_item_visible;
extern LPSTR g_szUpsf_item_tabstop;
extern LPSTR g_szUpsf_item_groupstop;
extern LPSTR g_szUpsf_item_locked;
extern LPSTR g_szUpsf_item_growsflag;
extern LPSTR g_szUpsf_item_signalid;
extern LPSTR g_szUpsf_item_extrascope;
extern LPSTR g_szUpsf_shape_version;
extern LPSTR g_szUpsf_shape_type;
extern LPSTR g_szUpsf_shape_dll;
#endif


#endif

