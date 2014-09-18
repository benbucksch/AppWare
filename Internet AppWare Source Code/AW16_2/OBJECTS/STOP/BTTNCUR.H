/*
 * BTTNCUR.H
 *
 * Public include file for the Button Images and Cursor DLL, including
 * structures, definitions, and function prototypes.
 *
 * Copyright (c)1992 Microsoft Corporation, All Rights Reserved.
 */

#ifdef __cplusplus
extern "C"
    {
#endif


//Standard image bitmap
#define IDB_STANDARDIMAGES          400

//Image indices inside the standard bitmap.
#define TOOLIMAGE_MIN               0
#define TOOLIMAGE_EDITCUT           0
#define TOOLIMAGE_EDITCOPY          1
#define TOOLIMAGE_EDITPASTE         2
#define TOOLIMAGE_FILENEW           3
#define TOOLIMAGE_FILEOPEN          4
#define TOOLIMAGE_FILESAVE          5
#define TOOLIMAGE_FILEPRINT         6
#define TOOLIMAGE_HELP              7
#define TOOLIMAGE_HELPCONTEXT       8
#define TOOLIMAGE_MAX               8


//Additional Standard Cursors as defined in the UI Design Guide.
#define IDC_NEWUICURSORMIN          500
#define IDC_RIGHTARROW              500
#define IDC_CONTEXTHELP             501
#define IDC_MAGNIFY                 502
#define IDC_NODROP                  503
#define IDC_TABLETOP                504
#define IDC_HSIZEBAR                505
#define IDC_VSIZEBAR                506
#define IDC_HSPLITBAR               507
#define IDC_VSPLITBAR               508
#define IDC_SMALLARROWS             509
#define IDC_LARGEARROWS             510
#define IDC_HARROWS                 511
#define IDC_VARROWS                 512
#define IDC_NESWARROWS              513
#define IDC_NWSEARROWS              514
#define IDC_NEWUICURSORMAX          514



//Standard sizes for toolbar buttons and bitmaps
#define TOOLBUTTON_STDWIDTH         24
#define TOOLBUTTON_STDHEIGHT        22
#define TOOLBUTTON_STDIMAGEWIDTH    16
#define TOOLBUTTON_STDIMAGEHEIGHT   15



/*
 * Values for button display states.  Each value is mutually
 * exclusive and contains one or more grouping bits.  Each group
 * represents buttons that share some sub-state in common.
 *
 * The actual state values are split into a command group and an
 * attribute group.  Up, mouse down, and disabled states are identical,
 * but only attributes can have down, down disabled, and indeterminate
 * states.
 *
 * BUTTONGROUP_BLANK is defined so an application can draw only the button
 * without an image in the up, down, mouse down, or indeterminate
 * state, that is, BUTTONGROUP_BLANK is inclusive with BUTTONGROUP_DOWN
 * and BUTTONGROUP_LIGHTFACE.
 */


#define BUTTONGROUP_DOWN                0x0001
#define BUTTONGROUP_ACTIVE              0x0002
#define BUTTONGROUP_DISABLED            0x0004
#define BUTTONGROUP_LIGHTFACE           0x0008
#define BUTTONGROUP_BLANK               0x0010


//Command buttons only
#define COMMANDBUTTON_UP                (BUTTONGROUP_ACTIVE)
#define COMMANDBUTTON_MOUSEDOWN         (BUTTONGROUP_ACTIVE | BUTTONGROUP_DOWN)
#define COMMANDBUTTON_DISABLED          (BUTTONGROUP_DISABLED)

//Attribute buttons only
#define ATTRIBUTEBUTTON_UP              (BUTTONGROUP_ACTIVE)
#define ATTRIBUTEBUTTON_MOUSEDOWN       (BUTTONGROUP_ACTIVE | BUTTONGROUP_DOWN)
#define ATTRIBUTEBUTTON_DISABLED        (BUTTONGROUP_DISABLED)
#define ATTRIBUTEBUTTON_DOWN            (BUTTONGROUP_ACTIVE | BUTTONGROUP_DOWN | BUTTONGROUP_LIGHTFACE)
#define ATTRIBUTEBUTTON_INDETERMINATE   (BUTTONGROUP_ACTIVE | BUTTONGROUP_LIGHTFACE)
#define ATTRIBUTEBUTTON_DOWNDISABLED    (BUTTONGROUP_DISABLED | BUTTONGROUP_DOWN | BUTTONGROUP_LIGHTFACE)

//Blank buttons only
#define BLANKBUTTON_UP                  (BUTTONGROUP_ACTIVE | BUTTONGROUP_BLANK)
#define BLANKBUTTON_DOWN                (BUTTONGROUP_ACTIVE | BUTTONGROUP_BLANK | BUTTONGROUP_DOWN | BUTTONGROUP_LIGHTFACE)
#define BLANKBUTTON_MOUSEDOWN           (BUTTONGROUP_ACTIVE | BUTTONGROUP_BLANK | BUTTONGROUP_DOWN)
#define BLANKBUTTON_INDETERMINATE       (BUTTONGROUP_ACTIVE | BUTTONGROUP_BLANK | BUTTONGROUP_LIGHTFACE)


//Public functions in BTTNCUR.DLL
HCURSOR WINAPI UICursorLoad(UINT);
BOOL    WINAPI UIToolButtonDraw(HDC, int, int, int, int, HBITMAP, int, int, int, UINT);


//Added because this code is no longer in a DLL
BOOL           FAR PASCAL ToolButtonInit(void);
void           FAR PASCAL ToolButtonFree(void);


#ifdef __cplusplus
    }
#endif
