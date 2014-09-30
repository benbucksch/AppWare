////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "wlay_rsc.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   WINDOW LAYOUT RESOURCE FILE HEADER
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef   WLAY_RSC_H
#define   WLAY_RSC_H

// DEFINE FOR EASY USE OF GETTING RESOURCE STRINGS /////////////////////////
#ifndef ID
#ifndef RCINVOKED
#define ID(x)       MAKEINTRESOURCE(x)
#else
#define ID(x)       x
#endif
#endif

// CONSTANTS FOR THE RESOURCES /////////////////////////////////////////////

#define FONT_AW   FONT 8, "MS Sans Serif"

// constants for dialogs, menus, accelerators, cursors, icons, etc.
#define MENU_LAYOUT                   4000

#define ACCEL_LAYOUT                  4010

#define CUR_NMBR_12                   4020
#define CUR_NMBR_21                   4021
#define CUR_TABTOOL                   4022
#define CUR_GROUPTOOL                 4023
#define CUR_LOCKTOOL                  4024
#define CUR_VISBTOOL                  4025
#define CUR_ANCHOR                    4026
#define IDC_VSIZEBAR                  4027

#define ICON_LAYOUT                   4030

#define DLG_ATTACH                    4040
#define DLG_GRID                      4041
#define DLG_PREFERENCES               4042
#define DLG_ITEMSIZES                 4043
#define DLG_RENAMEOBJ                 4044
#define DLG_WNDSIZES                  4045
#define DLG_WNDSTYLE                  4046

// version resources must be 1
#define VER_LAYOUT                    1

// general defines
#define IDOK                          1
#define IDCANCEL                      2
#define IDHELP                        998

// constants for DLG_ATTACH --- start with 100
#define BMP_NORMAL                    1000
#define BMP_PUSHED                    3000

#define GB_SAMPLE                     100
#define GB_ATTACHSTATES               101

#define PB_CURRENTATTACH              110

#define BMP_STARTWINDOW               120

#define BMP_OOOO                      130
#define BMP_OOOX                      131
#define BMP_OOXO                      132
#define BMP_OOXX                      133
#define BMP_OXOO                      134   // unused
#define BMP_OXOX                      135
#define BMP_OXXO                      136   // unused
#define BMP_OXXX                      137
#define BMP_XOOO                      138   // unused
#define BMP_XOOX                      139   // unused
#define BMP_XOXO                      140
#define BMP_XOXX                      141
#define BMP_XXOO                      142   // unused
#define BMP_XXOX                      143   // unused
#define BMP_XXXO                      144   // unused
#define BMP_XXXX                      145

#define BMP_UP_STARTWINDOW            1120  //(BMP_STARTWINDOW + BMP_NORMAL)

#define BMP_UP_OOOO                   1130  //(BMP_OOOO + BMP_NORMAL)
#define BMP_UP_OOOX                   1131  //(BMP_OOOX + BMP_NORMAL)
#define BMP_UP_OOXO                   1132  //(BMP_OOXO + BMP_NORMAL)
#define BMP_UP_OOXX                   1133  //(BMP_OOXX + BMP_NORMAL)
#define BMP_UP_OXOO                   1134  //(BMP_OXOO + BMP_NORMAL)   // unused
#define BMP_UP_OXOX                   1135  //(BMP_OXOX + BMP_NORMAL)
#define BMP_UP_OXXO                   1136  //(BMP_OXXO + BMP_NORMAL)   // unused
#define BMP_UP_OXXX                   1137  //(BMP_OXXX + BMP_NORMAL)
#define BMP_UP_XOOO                   1138  //(BMP_XOOO + BMP_NORMAL)   // unused
#define BMP_UP_XOOX                   1139  //(BMP_XOOX + BMP_NORMAL)   // unused
#define BMP_UP_XOXO                   1140  //(BMP_XOXO + BMP_NORMAL)
#define BMP_UP_XOXX                   1141  //(BMP_XOXX + BMP_NORMAL)
#define BMP_UP_XXOO                   1142  //(BMP_XXOO + BMP_NORMAL)   // unused
#define BMP_UP_XXOX                   1143  //(BMP_XXOX + BMP_NORMAL)   // unused
#define BMP_UP_XXXO                   1144  //(BMP_XXXO + BMP_NORMAL)   // unused
#define BMP_UP_XXXX                   1145  //(BMP_XXXX + BMP_NORMAL)

#define BMP_DOWN_OOOO                 3130  //(BMP_OOOO + BMP_PUSHED)
#define BMP_DOWN_OOOX                 3131  //(BMP_OOOX + BMP_PUSHED)
#define BMP_DOWN_OOXO                 3132  //(BMP_OOXO + BMP_PUSHED)
#define BMP_DOWN_OOXX                 3133  //(BMP_OOXX + BMP_PUSHED)
#define BMP_DOWN_OXOO                 3134  //(BMP_OXOO + BMP_PUSHED)   // unused
#define BMP_DOWN_OXOX                 3135  //(BMP_OXOX + BMP_PUSHED)
#define BMP_DOWN_OXXO                 3136  //(BMP_OXXO + BMP_PUSHED)   // unused
#define BMP_DOWN_OXXX                 3137  //(BMP_OXXX + BMP_PUSHED)
#define BMP_DOWN_XOOO                 3138  //(BMP_XOOO + BMP_PUSHED)   // unused
#define BMP_DOWN_XOOX                 3139  //(BMP_XOOX + BMP_PUSHED)   // unused
#define BMP_DOWN_XOXO                 3140  //(BMP_XOXO + BMP_PUSHED)
#define BMP_DOWN_XOXX                 3141  //(BMP_XOXX + BMP_PUSHED)
#define BMP_DOWN_XXOO                 3142  //(BMP_XXOO + BMP_PUSHED)   // unused
#define BMP_DOWN_XXOX                 3143  //(BMP_XXOX + BMP_PUSHED)   // unused
#define BMP_DOWN_XXXO                 3144  //(BMP_XXXO + BMP_PUSHED)   // unused
#define BMP_DOWN_XXXX                 3145  //(BMP_XXXX + BMP_PUSHED)

// constants for DLG_GRID --- start with 200
#define ST_GRIDWIDTH                  200
#define ST_GRIDHEIGHT                 201

#define ED_GRIDWIDTH                  210
#define ED_GRIDHEIGHT                 211

#define XB_SHOWGRID                   220
#define XB_SNAPTOGRID                 221
#define IDC_GROUPBOX15	202
#define IDC_GROUPBOX16	203

#define PB_RESETGRID                  230

// constants for DLG_ITEMSIZES --- start with 300

#define ED_HPOSITION                  320
#define ED_WIDTH                      321
#define ED_VPOSITION                  322
#define ED_HEIGHT                     323
#define IDC_GROUPBOX13	324
#define IDC_GROUPBOX14	325

#define ST_HPOSITION                  330
#define ST_WIDTH                      331
#define ST_VPOSITION                  332
#define ST_HEIGHT                     333

// constants for DLG_RENAMEOBJ --- start with 400

#define ED_OBJECTNAME                 402

// constants for DLG_PREFERENCES --- start with 500
#define XB_PRSTWNDPOSITIONS           500
#define XB_REQFULLENCLOSURE           501


#define RB_SELECTIONTOOL              520
#define RB_PRSTTOOL                   521
#define RB_FULLINFOMODE               522
#define RB_WYSIWYGMODE                523
#define RB_PRSTMODE                   524
#define RB_INCHES                     525
#define RB_MILLIMETERS                526
#define RB_POINTS                     527
#define IDC_GROUPBOX9	502
#define IDC_GROUPBOX10	503
#define IDC_GROUPBOX11	504
#define IDC_GROUPBOX12	505

// constants for DLG_WNDSTYLE
#define CB_MENUBAR                    600

#define ED_WINDOWTITLE                610

#define XB_TITLEBAR                   620
#define XB_SYSTEMMENU                 621
#define XB_MAXIMIZEBUTTON             622
#define XB_MINIMIZEBUTTON             623
#define XB_HORIZONTALSCROLL           624
#define XB_VERTICALSCROLL             625
#define XB_RESIZEBORDER               626
#define XB_DIALOGFRAME                627
#define XB_MODAL                      628
#define XB_ALWAYSONTOP                629
#define XB_INITIALLYVISIBLE           630

#define ST_MENUBAR                    640
#define ST_WINDOWTITLE                641
#define ST_SAMPLEWINDOW               642
#define IDC_GROUPBOX2	603
#define XB_QUITONCLOSE	632


// constants for DLG_WNDSIZES

#define ST_LEFT                       710
#define ST_TOP                        711
#define ST_RIGHT                      712
#define ST_BOTTOM                     713
#define ST_MINHEIGHT                  714
#define ST_MINWIDTH                   715
#define ST_MAXHEIGHT                  716
#define ST_MAXWIDTH                   717
#define ST_SCROLLHEIGHT               718
#define ST_SCROLLWIDTH                719

#define ED_LEFT                       730
#define ED_TOP                        731
#define ED_BOTTOM                     732
#define ED_RIGHT                      733
#define ED_MINHEIGHT                  734
#define ED_MINWIDTH                   735
#define ED_MAXHEIGHT                  736
#define ED_MAXWIDTH                   737
#define ED_SCROLLHEIGHT               738
#define ED_SCROLLWIDTH                739

#define RB_LIMITED                    750
#define RB_UNLIMITED                  751

#define IDC_GROUPBOX4	704
#define IDC_GROUPBOX5	705
#define IDC_GROUPBOX6	706
#define IDC_GROUPBOX7	707
#define IDC_GROUPBOX8	708

// constants for MENU_LAYOUT
// file menu
#define IDM_SAVE                      100
#define IDM_REVERT                    101
#define IDM_EXIT                      102

// edit menu
//#define IDM_UNDO                      200
//#define IDM_REDO                      201
//#define IDM_CUT                       202
//#define IDM_COPY                      203
//#define IDM_PASTE                     204
#define IDM_DELETE                    205
#define IDM_SELECTALL                 206
#define IDM_DUPLICATE                 207
#define IDM_WNDATTRIBUTES             208
//#define IDM_WNDBACKGROUND           209
#define IDM_WNDSIZES                  210
#define IDM_LAYPREFERENCES            211
#define IDM_DELETEOBJECT              212
#define IDM_OBJECTNAME                213


// view menu
#define IDM_DETAILS                   300
#define IDM_SHOWGRID				  301
#define IDM_SNAPGRID				  302
#define IDM_GRID                      303


// items menu
#define IDM_SENDTOBACK                400
#define IDM_BRINGTOFRONT              401
#define IDM_GROUP                     402
#define IDM_UNGROUP                   403
#define IDM_EDITITEM                  404
#define IDM_SIZE                      405


// alignment menu
#define IDM_ALIGNLEFT                 500
#define IDM_ALIGNTOP                  501
#define IDM_ALIGNRIGHT                502
#define IDM_ALIGNBOTTOM               503
#define IDM_CENTERHORZ                504
#define IDM_CENTERVERT                505
#define IDM_CENTERGROUPHORZ           506
#define IDM_CENTERGROUPVERT           507
#define IDM_GROWWIDEST		 		  508
#define IDM_SHRINKTHINNEST  		  509
#define IDM_GROWTALLEST     		  510
#define IDM_SHRINKSHORTEST  		  511
#define IDM_SPACEHORZ                 512
#define IDM_SPACEVERT                 513

//#define IDM_ATTACH                    446


// tools menu
#define IDM_SELECTION                 600
#define IDM_REORDER                   601
#define IDM_TABSTOPS                  602
#define IDM_GROUPSTOPS                603
#define IDM_VISIBILITY                604
#define IDM_LOCK                      605
#define IDM_ANCHOR                    606
#define IDM_HOLLOWOVALS				  607
#define IDM_FILLEDROUNDEDBOXES	      608
#define IDM_HOLLOWROUNDEDBOXES		  609
#define IDM_FILLEDBOXES				  610
#define IDM_HOLLOWBOXES				  611
#define IDM_FILLEDOVALS				  612
#define IDM_LINES  					  613
#define IDM_FONT   					  614
#define IDM_PATTERN					  615


// palettes menu
#define IDM_RESETPOSITIONS	     	  700
#define IDM_HIDEALL                   701
#define IDM_SHOWALL                   702
#define IDM_ALIGNMENT                 703
#define IDM_NAME                      704
#define IDM_OBJECTLIST                705
#define IDM_SIZES                     706
#define IDM_USERPALETTE               707


// help menu
#define IDM_HELPCONTENTS			  800
#define IDM_HELPMENUS   			  801
#define IDM_HELPPALETTES			  802
#define IDM_QUICKTIPS				  803



// constants for the STRINGTABLE
#define ERROR_NOPROTSUPPORT           1000
#define ERROR_INVALIDHWND             1001

// Standard tool bitmaps.  For coding ease, these should match
// OSHP_STDTOOL_*
#define STDTOOL_ANCHOR                1
#define STDTOOL_ARROW                 2
#define STDTOOL_BACKGROUND            3
#define STDTOOL_GROUPSTOP             4
#define STDTOOL_LOCK                  5
#define STDTOOL_REORDER               6
#define STDTOOL_SELECTALL             7
#define STDTOOL_TABSTOP               8
#define STDTOOL_VISIBILITY            9

// Alignment tools bitmaps
#define ALIGNLEFT           100
#define ALIGNRIGHT          101
#define ALIGNTOP            102
#define ALIGNBOTTOM         103
#define CENTERHORZ          104
#define CENTERVERT          105
#define CENTERGROUPHORZ     106
#define CENTERGROUPVERT	    107
#define GROWWIDE            108
#define GROWTALL            109
#define SHRINKTHIN          110
#define SHRINKSHORT         111
#define SPACEVERT           112
#define SPACEHORZ           113
//#define GRID                114
//#define MOVESIZE            115

////////////////////////////////////////////////////////////////////////////

// Temporary

#define TMP_SRCCURSOR                 101
#define TMP_DSTCURSOR                 102
#define TMP_INVCURSOR                 103

#endif  // #ifndef  WLAY_RSC_H
