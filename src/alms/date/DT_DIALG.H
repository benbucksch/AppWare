#ifndef DT_DIALOG_H
#define DT_DIALOG_H


/// Define for dialog ID's ////

#define DATE_SETUP_DIALOG		100

#define ST_OBJECTNAME				1001  //The name of the object type being edited

/// Defines for controls ////
#define ED_TITLE						1100
#define PB_OK               1
#define PB_CANCEL           2
#define PB_HELP							3

#define CB_EDITABLE					1301
#define CB_FRAME		        1302
#define RB_LEFT             1305
#define RB_RIGHT            1307
#define PB_FONTS						1312
#define PB_COLOR						1313
#define CB_SELECTALL				1316
#define CB_ACTIVATABLE			1319
#define CB_CANACTIVATE      1319
#define CB_DISPLAYEMPTY			1320

#define RB_SHORTDATE				1420
#define RB_LONGDATE					1421
#define RB_KEEPCURRENT 			1422
#define RB_EDITABLE					1423
#define RB_NEITHER					1424

/////////////////////////////////
// STRINGTABLE Defines  ////////
////////////////////////////////
#define ST_SIZETOBIG				100
#define LONGMONTH_NAMES			200 //Janunary+0, February+1...December+11
#define SHORTMONTH_NAMES 		212 //Jan+0, Feb+1...Dec+11
#define LONGDAY_NAMES				224 //Sunday+0 Monday+1...Saturday+6
#define SHORTDAY_NAMES			236 //Sun+0 Mon+1...Sat+6

#endif

