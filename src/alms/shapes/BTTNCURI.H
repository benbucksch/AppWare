/*
 * BTTNCURI.H
 *
 * Private include file for the Button Images and Cursors DLL.
 *
 * Copyright (c)1992 Microsoft Corporation, All Rights Reserved.
 */

#ifdef __cplusplus
extern "C"
    {
#endif

//Function prototypes.

//BTTNCUR.C
#if 0
HANDLE         FAR  PASCAL LibMain(HANDLE, WORD, WORD, LPSTR);
void           FAR  PASCAL WEP(int);
BOOL           NEAR PASCAL ToolButtonInit(void);
void           NEAR PASCAL ToolButtonFree(void);
#endif
static HBITMAP NEAR PASCAL HDitherBitmapCreate(COLORREF, COLORREF);
static void    NEAR PASCAL PatB(HDC, int, int, int, int, COLORREF);
static void    NEAR PASCAL MaskCreate(UINT, int, int, int, int, int, int, UINT);

//CURSORS.C
#if 0
void                PASCAL CursorsCache(HINSTANCE);
void                PASCAL CursorsFree(void);
#endif



/*
 * Wierd Wild Wooly Waster (raster) Ops for special bltting.  See the
 * Windows SDK reference on Raster Operation Codes for explanation of
 * these.  The DSPDxax and PSDPxax is a reverse-polish notation for
 * operations where D==Destination, S==Source, P==Patterm, a==AND,
 * x==XOR.  Both of these codes are actually described in Programming
 * Windows by Charles Petzold, Second Edition, pages 622-624.
 */
#define ROP_DSPDxax  0x00E20746L
#define ROP_PSDPxax  0x00B8074AL



/*
 * Button types, used internally to distinguish command buttons from
 * attribute buttons to enforce three-state or six-state possibilities.
 * Command buttons can only have three states (up, mouse down, disabled)
 * while attribute buttons add (down, down disabled, and indeterminate).
 */

#define BUTTONTYPE_COMMAND      0
#define BUTTONTYPE_ATTRIBUTE    1


#ifdef __cplusplus
    }
#endif
