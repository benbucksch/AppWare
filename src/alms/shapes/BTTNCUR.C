/*
 * BTTNCUR.C
 *
 * Public functions to generate different states of toolbar buttons from
 * a single bitmap.  States are normal, pressed, checked, and disabled.
 *
 * Copyright (c)1992 Microsoft Corporation, All Rights Reserved.
 */


#include <windows.h>
#include "bttncur.h"
#include "bttncuri.h"


//Cache GDI objects to speed drawing.
static HDC     hDCGlyphs    = NULL;
static HDC     hDCMono      = NULL;
static HBRUSH  hBrushDither = NULL;

//Standard images to use in case caller doesn't provide them
static HBITMAP hBmpStandardImages=NULL;



/*
 * Colors often referenced.  We actually hardcode these values because
 * the bitmap manipulations we do in this DLL use raster operations
 * that depend on the bitmaps and colors being made of grays, without
 * any other colors.  So against my better judgement of always allowing
 * color customization, we have to hardcode these for the purposes
 * of this DLL.  And as a user who usually picks color issues, I don't
 * mind this forced color here.
 */
static COLORREF rgbFace    =RGB(192, 192, 192);     //Light gray
static COLORREF rgbShadow  =RGB(128, 128, 128);     //Dark gray
static COLORREF rgbHilight =RGB(255, 255, 255);     //White
static COLORREF rgbFrame   =RGB(0, 0, 0);           //Black



/*
 * Mapping from image identifier to button type (command/attribute).
 * Version 1.00 of this DLL has no attribute images defined, so
 * the code will only support three states for each command
 * button.  Any state is, however, valid for an application
 * defined image.
 */

UINT mpButtonType[TOOLIMAGE_MAX-TOOLIMAGE_MIN+1]=
        {
        BUTTONTYPE_COMMAND, BUTTONTYPE_COMMAND, BUTTONTYPE_COMMAND,
        BUTTONTYPE_COMMAND, BUTTONTYPE_COMMAND, BUTTONTYPE_COMMAND,
        BUTTONTYPE_COMMAND, BUTTONTYPE_COMMAND, BUTTONTYPE_COMMAND
        };




/*
 * LibMain
 *
 * Purpose:
 *  DLL-specific entry point called from LibEntry.  Initializes
 *  global variables and loads standard image bitmaps.
 *
 * Parameters:
 *  hInstance       HANDLE instance of the DLL.
 *  wDataSeg        WORD segment selector of the DLL's data segment.
 *  wHeapSize       WORD byte count of the heap.
 *  lpCmdLine       LPSTR to command line used to start the module.
 *
 * Return Value:
 *  HANDLE          Instance handle of the DLL.
 */

#if 0
HANDLE FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg
                          , WORD cbHeapSize, LPSTR lpCmdLine)
    {
    //Attempt to load our standard image resource.
    hBmpStandardImages=LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_STANDARDIMAGES));

    if (NULL==hBmpStandardImages)
        return NULL;

    //Perform global initialization.
    if (ToolButtonInit())
        {
        CursorsCache(hInstance);

        if (0!=cbHeapSize)
            UnlockData(0);

        return hInstance;
        }

    return (HANDLE)NULL;
    }
#endif


/*
 * WEP
 *
 * Purpose:
 *  Required DLL Exit function.  Does nothing.
 *
 * Parameters:
 *  bSystemExit     BOOL indicating if the system is being shut
 *                  down or the DLL has just been unloaded.
 *
 * Return Value:
 *  void
 *
 */

#if 0
void FAR PASCAL WEP(int bSystemExit)
    {
    /*
     * **Developers:  Note that WEP is called AFTER Windows does any
     *                automatic task cleanup.  You may see warnings for
     *                that two DCs, a bitmap, and a brush, were not
     *                deleted before task termination.  THIS IS NOT A
     *                PROBLEM WITH THIS CODE AND IT IS NOT A BUG.  This
     *                WEP function is properly called and performs the
     *                cleanup as appropriate.  The fact that Windows is
     *                calling WEP after checking task cleanup is not
     *                something we can control.  Just to prove it, the
     *                OutputDebugStrings in this and ToolButtonFree
     *                show that the code is exercised.
     */

   #ifdef DEBUG
    OutputDebugString("WEP Entry\r\n");
   #endif

    CursorsFree();
    ToolButtonFree();

   #ifdef DEBUG
    OutputDebugString("WEP Exit\r\n");
   #endif
    return;
    }

#endif







/*
 * ToolButtonInit
 * Internal
 *
 * Purpose:
 *  Initializes GDI objects for drawing images through UIToolButtonDraw.
 *  If the function fails, the function has already performed proper
 *  cleanup.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if initialization succeeded.  FALSE otherwise.
 */

#if 0
BOOL NEAR PASCAL ToolButtonInit(void)
#else
BOOL FAR PASCAL ToolButtonInit(void)
#endif
    {
    HBITMAP     hBmpGray;

    rgbFrame=GetSysColor(COLOR_WINDOWFRAME);
    hDCGlyphs=CreateCompatibleDC(NULL);

    //Mono DC and Bitmap for disabled image - bitmap always selected into DC
    hDCMono =CreateCompatibleDC(NULL);

    //Create a PatBlt brush based on face and highlight colors.
    hBmpGray=HDitherBitmapCreate(rgbFace, rgbHilight);

    if (NULL!=hBmpGray)
        {
        hBrushDither=CreatePatternBrush(hBmpGray);
        DeleteObject(hBmpGray);
        }

    if (NULL==hDCGlyphs || NULL==hDCMono || NULL==hBrushDither)
        {
        //On failure, cleanup whatever might have been allocated.
        ToolButtonFree();
        return FALSE;
        }

    return TRUE;
    }





/*
 * ToolButtonFree
 * Internal
 *
 * Purpose:
 *  Free all GDI allocations made during initialization.  Note that the
 *  DEBUG output included here shows that WEP is called and cleanup actually
 *  occurs.  However, if you watch debug output in DBWIN or on a terminal,
 *  the debugging version of Windows does automatic app cleanup before WEP
 *  is called, leading some to believe that this code is buggy.  The
 *  debug output below shows that we do perform all necessary cleanup.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

#if 0
void NEAR PASCAL ToolButtonFree(void)
#else
void FAR PASCAL ToolButtonFree(void)
#endif
    {
    if (NULL!=hDCMono)
        {
        DeleteDC(hDCMono);
       #ifdef DEBUG
        OutputDebugString("ToolButtonFree:  hDCMono deleted.\r\n");
       #endif
        }

    hDCMono=NULL;


    if (NULL!=hDCGlyphs)
        {
        DeleteDC(hDCGlyphs);
       #ifdef DEBUG
        OutputDebugString("ToolButtonFree:  hDCGlyphs deleted.\r\n");
       #endif
        }

    hDCGlyphs=NULL;


    if (NULL!=hBrushDither)
        {
        DeleteObject(hBrushDither);
       #ifdef DEBUG
        OutputDebugString("ToolButtonFree:  hBrushDither deleted.\r\n");
       #endif
        }

    hBrushDither=NULL;


    if (NULL!=hBmpStandardImages)
        {
        DeleteObject(hBmpStandardImages);
       #ifdef DEBUG
        OutputDebugString("ToolButtonFree:  hBmpStandardImages deleted.\r\n");
       #endif
        }

    hBmpStandardImages=NULL;

    return;
    }






/*
 * HDitherBitmapCreate
 * Internal
 *
 * Purpose:
 *  Creates and returns a handle to an 8*8 monochrome pattern bitmap
 *  for creating a pattern brush.  The button face and highlight colors
 *  are used to indicate the resulting colors of a PatBlt using this
 *  brush.
 *
 * Parameters:
 *  rgbFace         COLORREF of the button face color.
 *  rgbHi           COLORREF of the button highlight color.
 *
 * Return Value:
 *  HBITMAP         Handle to the dither bitmap.
 */

static HBITMAP NEAR PASCAL HDitherBitmapCreate(COLORREF rgbFace, COLORREF rgbHi)
    {
    struct  //BITMAPINFO with 16 colors
        {
        BITMAPINFOHEADER    bmiHeader;
        RGBQUAD             bmiColors[16];
        } bmi;

    DWORD       patGray[8];
    HDC         hDC;
    HBITMAP     hBmp;

    /*
     * We're going to create an 8*8 brush for PatBlt using the
     * button face color and button highlight color.  We use this
     * brush to affect the pressed state and the disabled state.
     */
    bmi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth        = 8;
    bmi.bmiHeader.biHeight       = 8;
    bmi.bmiHeader.biPlanes       = 1;
    bmi.bmiHeader.biBitCount     = 1;
    bmi.bmiHeader.biCompression  = BI_RGB;
    bmi.bmiHeader.biSizeImage    = 0;
    bmi.bmiHeader.biXPelsPerMeter= 0;
    bmi.bmiHeader.biYPelsPerMeter= 0;
    bmi.bmiHeader.biClrUsed      = 0;
    bmi.bmiHeader.biClrImportant = 0;

    bmi.bmiColors[0].rgbBlue     = GetBValue(rgbFace);
    bmi.bmiColors[0].rgbGreen    = GetGValue(rgbFace);
    bmi.bmiColors[0].rgbRed      = GetRValue(rgbFace);
    bmi.bmiColors[0].rgbReserved = 0;

    bmi.bmiColors[1].rgbBlue     = GetBValue(rgbHi);
    bmi.bmiColors[1].rgbGreen    = GetGValue(rgbHi);
    bmi.bmiColors[1].rgbRed      = GetRValue(rgbHi);
    bmi.bmiColors[1].rgbReserved = 0;

    //Create the byte array for CreateDIBitmap.
    patGray[6]=patGray[4]=patGray[2]=patGray[0]=0x5555AAAAL;
    patGray[7]=patGray[5]=patGray[3]=patGray[1]=0xAAAA5555L;

    hDC=GetDC(NULL);
    hBmp=CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT, patGray
                        , (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);

    ReleaseDC(NULL, hDC);
    return hBmp;
    }






/*
 * UIToolButtonDraw
 * Public API
 *
 * Purpose:
 *  Draws the complete image of a toolbar-style button with a given
 *  image in the center and in a specific state.  The button is drawn
 *  on a specified hDC at a given location, so this function is useful
 *  on standard owner-draw buttons as well as on toolbar controls that
 *  have only one window but show images of multiple buttons.
 *
 * Parameters:
 *  hDC             HDC on which to draw.
 *  x, y            int coordinates at which to draw.
 *  dx, dy          int dimensions of the *button*, not necessarily the image.
 *  hBmp            HBITMAP from which to draw the image.
 *  bmx, bmy        int dimensions of each bitmap in hBmp.  If hBmp is NULL
 *                  then these are forced to the standard sizes.
 *  iImage          int index to the image to draw in the button
 *  uState          UINT containing the state index for the button.
 *
 * Return Value:
 *  BOOL            TRUE if drawing succeeded, FALSE otherwise meaning that
 *                  hDC is NULL or hBmp is NULL and iImage is not a valid
 *                  index for a standard image.
 */

BOOL WINAPI UIToolButtonDraw(HDC hDC, int x, int y, int dx, int dy
                             , HBITMAP hBmp, int bmx, int bmy
                             , int iImage, UINT uState)
    {
    int         iSaveDC;
    int         xOffsetGlyph, yOffsetGlyph;
    int         j;
    HBRUSH      hBR;
    HGDIOBJ     hObj;
    HBITMAP     hBmpT=NULL;
    HBITMAP     hBmpMono;
    HBITMAP     hBmpMonoOrg;

    if (NULL==hDC)
        return FALSE;

    /*
     * If we're given no image bitmap, then use the standard and validate the
     * image index.  We also enforce the standard bitmap size and the size of
     * the button (as requested by User Interface designers).
     */
    if (NULL==hBmp && !(uState & BUTTONGROUP_BLANK))
        {
        hBmp=hBmpStandardImages;
        bmx=TOOLBUTTON_STDIMAGEWIDTH;       //Force bitmap dimensions
        bmy=TOOLBUTTON_STDIMAGEHEIGHT;

        dx=TOOLBUTTON_STDWIDTH;             //Force button dimensions
        dy=TOOLBUTTON_STDHEIGHT;

        if (iImage > TOOLIMAGE_MAX)
            return FALSE;

        /*
         * If we are using a standard command button, verify that the state
         * does not contain the LIGHTFACE group which only applies to
         * attribute buttons.
         */
        if (BUTTONTYPE_COMMAND==mpButtonType[iImage]
            && (uState & BUTTONGROUP_LIGHTFACE))
            return FALSE;
        }

    //Save the DC state before we munge on it.
    iSaveDC=SaveDC(hDC);

    //Create a dithered bitmap.
    hBmpMono =CreateBitmap(dx-2, dy-2, 1, 1, NULL);

    if (NULL==hBmpMono)
        return FALSE;

    hBmpMonoOrg =(HBITMAP)SelectObject(hDCMono,  hBmpMono);


    //Draw the border around the button.
    PatB(hDC, x+1,    y,      dx-2, 1,    rgbFrame);
    PatB(hDC, x+1,    y+dy-1, dx-2, 1,    rgbFrame);
    PatB(hDC, x,      y+1,    1,    dy-2, rgbFrame);
    PatB(hDC, x+dx-1, y+1,    1,    dy-2, rgbFrame);


    //Shift coordinates to account for the border we just drew
    x++;
    y++;
    dx-=2;
    dy-=2;

    //Paint the interior grey as a default.
    PatB(hDC, x, y, dx, dy, rgbFace);

    /*
     * Determine the offset necessary to center the image but also reflect
     * the pushed-in state, which means just adding 1 to the up state.
     */

    xOffsetGlyph=(dx-bmx) >> 1;
    yOffsetGlyph=(dy-bmy) >> 1;

    /*
     * Draw shadows and highlights.  The DOWN grouping that contains
     * down, mouse down, and down disabled are drawn depressed.  Up,
     * indeterminate, and disabled are drawn up.
     */

    if (uState & BUTTONGROUP_DOWN)
        {
        PatB(hDC, x, y, 1,  dy, rgbShadow);
        PatB(hDC, x, y, dx, 1,  rgbShadow);

        //For any depressed button, add one to the offsets.
        xOffsetGlyph+=1;
        yOffsetGlyph+=1;
        }
    else
        {
        //Normal button look.
        PatB(hDC, x, y, 1,    dy-1, rgbHilight);
        PatB(hDC, x, y, dx-1, 1,    rgbHilight);

        PatB(hDC, x+dx-1, y,      1,  dy, rgbShadow);
        PatB(hDC, x,      y+dy-1, dx, 1,  rgbShadow);

        PatB(hDC, x+1+dx-3, y+1,    1,    dy-2, rgbShadow);
        PatB(hDC, x+1,      y+dy-2, dx-2, 1,    rgbShadow);
        }



    //Select the given image bitmap into the glyph DC before calling MaskCreate
    if (NULL!=hBmp)
        hBmpT=(HBITMAP)SelectObject(hDCGlyphs, hBmp);

    /*
     * Draw the face on the button.  If we have an up or [mouse]down
     * button then we can just draw it as-is.  For indeterminate,
     * disabled, or down disabled we have to gray the image and possibly
     * add a white shadow to it (disabled/down disabled).
     *
     * Also note that for the intermediate state we first draw the normal
     * up state, then proceed to add disabling looking highlights.
     */

    //Up, mouse down, down, indeterminate
    if ((uState & BUTTONGROUP_ACTIVE) && !(uState & BUTTONGROUP_BLANK))
        {
        //The image is black--not disabling or indeterminate.
        BitBlt(hDC, x+xOffsetGlyph, y+yOffsetGlyph, bmx, bmy
               , hDCGlyphs, iImage*bmx, 0, SRCCOPY);
        }


    if ((uState & BUTTONGROUP_DISABLED || ATTRIBUTEBUTTON_INDETERMINATE==uState)
        && !(uState & BUTTONGROUP_BLANK))
        {
        //Grayed state (up or down, no difference)
        MaskCreate(iImage, dx, dy, bmx, bmy, xOffsetGlyph, yOffsetGlyph, 0);

        SetTextColor(hDC, 0L);                     //0's in mono -> 0
        SetBkColor(hDC, (COLORREF)0x00FFFFFFL);     //1's in mono -> 1

        //If we're disabled, up or down, draw the highlighted shadow.
        if (uState & BUTTONGROUP_DISABLED)
            {
            hBR=CreateSolidBrush(rgbHilight);
            if (NULL!=hBR)
                {
                hObj=SelectObject(hDC, hBR);
                if (NULL!=hObj)
                    {
                    //Draw hilight color where we have 0's in the mask
                    BitBlt(hDC, x+1, y+1, dx-2, dy-2, hDCMono, 0, 0, ROP_PSDPxax);
                    SelectObject(hDC, hObj);
                    }
                DeleteObject(hBR);
                }
            }

        //Draw the gray image.
        hBR=CreateSolidBrush(rgbShadow);

        if (NULL!=hBR)
            {
            hObj=SelectObject(hDC, hBR);
            if (NULL!=hObj)
                {
                //Draw the shadow color where we have 0's in the mask
                BitBlt(hDC, x, y, dx-2, dy-2, hDCMono, 0, 0, ROP_PSDPxax);
                SelectObject(hDC, hObj);
                }

            DeleteObject(hBR);
            }
        }

    //If the button is selected do the dither brush avoiding the glyph
    if (uState & BUTTONGROUP_LIGHTFACE)
        {
        hObj=SelectObject(hDC, hBrushDither);

        if (NULL!=hObj)
            {
            /*
             * The mask we create now determines where the dithering
             * ends up.  In the down disabled state, we have to preserve
             * the highlighted shadow, so the mask we create must have
             * two masks of the original glyph, one of them offset by
             * one pixel in both x & y.  For the indeterminate state,
             * we have to mask all highlighted areas.  The state passed
             * to MaskCreate matters here (we've used zero before).
             */
            MaskCreate(iImage, dx, dy, bmx, bmy
                       , xOffsetGlyph-1, yOffsetGlyph-1, uState);


            SetTextColor(hDC, 0L);                      //0 -> 0
            SetBkColor(hDC, (COLORREF)0x00FFFFFFL);      //1 -> 1

            /*
             * Only draw the dither brush where the mask is 1's.  For
             * the indeterminate state we have to not overdraw the
             * shadow highlight so we use dx-3, dy-3 instead of dx-1
             * and dy-1.  We do this whether or not we're blank.
             */
            j=(ATTRIBUTEBUTTON_INDETERMINATE==uState
               || BLANKBUTTON_INDETERMINATE==uState) ? 3 : 1;

            BitBlt(hDC, x+1, y+1, dx-j, dy-j, hDCMono, 0, 0, ROP_DSPDxax);
            SelectObject(hDC, hObj);
            }
        }

    //Cleanup hDCGlyphs:  Must do AFTER calling MaskCreate
    if (NULL!=hBmpT)
        SelectObject(hDCGlyphs, hBmpT);

    SelectObject(hDCMono,   hBmpMonoOrg);
    DeleteObject(hBmpMono);

    //Restore everything in the DC.
    RestoreDC(hDC, iSaveDC);
    return TRUE;
    }







/*
 * PatB
 * Internal
 *
 * Purpose:
 *  A more convenient PatBlt operation for drawing button borders and
 *  highlights.
 *
 * Parameters:
 *  hDC             HDC on which to paint.
 *  x, y            int coordinates at which to paint.
 *  dx, dy          int dimensions of rectangle to paint.
 *  rgb             COLORREF to use as the background color.
 *
 * Return Value:
 *  None
 */

static void NEAR PASCAL PatB(HDC hDC, int x, int y, int dx, int dy, COLORREF rgb)
    {
    RECT        rc;

    SetBkColor(hDC, rgb);
    SetRect(&rc, x, y, x+dx, y+dy);
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    }




/*
 * MaskCreate
 * Internal
 *
 * Purpose:
 *  Creates a monochrome mask bitmap of the given image at the given offset
 *  in the global hDCMono.  Anywhere in the image that you have COLOR_BTNFACE
 *  or COLOR_HILIGHT you'll get 1's.  Anywhere else will be 0's
 *
 * Parameters:
 *  iImage          UINT index of the image for which to create a mask.
 *  dx, dy          int dimensions of the button.
 *  bmx, bmy        int dimensions of the bitmap to use.
 *  xOffset         int offset for x inside hDCMono where we paint.
 *  yOffset         int offset for y inside hDCMono where we paint.
 *  uState          UINT state of the image.  Special cases are made
 *                  for ATTRIBUTEBUTTON_DOWNDISABLED and
 *                  ATTRIBUTEBUTTON_INDETERMINATE.  In any case where you
 *                  do not want a special case, pass zero here, regardless
 *                  of the true button state.
 *
 * Return Value:
 *  None
 */

static void NEAR PASCAL MaskCreate(UINT iImage, int dx, int dy, int bmx, int bmy
                                   ,int xOffset, int yOffset, UINT uState)
    {
    //Initalize whole area with zeros
    PatBlt(hDCMono, 0, 0, dx, dy, WHITENESS);

    if (uState & BUTTONGROUP_BLANK)
        return;

    //Convert face colored pixels to 1's.
    SetBkColor(hDCGlyphs, rgbFace);
    BitBlt(hDCMono, xOffset, yOffset, bmx, bmy, hDCGlyphs, iImage*bmx, 0, SRCCOPY);

    //In the indeterminate state, don't turn highlight's to 1's. Leave black.
    if (ATTRIBUTEBUTTON_INDETERMINATE!=uState)
        {
        //Convert highlight colored pixels to 1's and OR them with the previous.
        SetBkColor(hDCGlyphs, rgbHilight);
        BitBlt(hDCMono, xOffset, yOffset, bmx, bmy, hDCGlyphs, iImage*bmx, 0, SRCPAINT);
        }

    /*
     * For the down disabled state, AND this same mask with itself at an
     * offset of 1, which accounts for the highlight shadow.
     */
    if (ATTRIBUTEBUTTON_DOWNDISABLED==uState)
        BitBlt(hDCMono, 1, 1, dx-1, dy-1, hDCMono,  0, 0, SRCAND);

    return;
    }
