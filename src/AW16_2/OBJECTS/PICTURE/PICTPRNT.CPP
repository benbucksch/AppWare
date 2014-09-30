/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
// Date:		Aug.28, 1993
//
// AUTHOR:Shailendra Jain
//
// DESCRIPTION: PICTURE OBJECT
			File:  PRINT.C

   Purpose:  Routines called to print a DIB.  Uses banding or lets GDI
             do the banding.  Works with 3.0 (i.e. via escapes) or 3.1
             (i.e. via printing APIs).

 	Functions:  DIBPrint
             BandDIBToPrinter
             PrintABand
             DeviceSupportsEscape
             TranslatePrintRect
             GetPrinterDC
             PrintAbortProc
             PrintAbortDlg
             DoStartDoc
             DoSetAbortProc
             DoStartPage
             DoEndPage
             DoEndDoc
             FindGDIFunction
             ShowPrintError

	Comments:	 Code from VC++ Sample disk.  Problem in printing to PS printers.
  					 This is a recommended patch by MS 

   History:   Date      Reason
             6/ 1/91    Created

//////////////////////////////////////////////////////////////*/



// #include "master.h"
#define STRICT
#define ASTRICT
#define ADGB_DEBUG
#define PICTPRINT

#include "a_alm.h"
#include "windows.h"
#include <math.h>
#include <mem.h>
#include "stdlib.h"
#include "picture.h"
#include "print.h"


#undef PICTPRINT

extern HINSTANCE               hInst;




//---------------------------------------------------------------------
//
// Function:   DIBNumColors
//
// Purpose:    Given a pointer to a DIB, returns a number of colors in
//             the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD DIBNumColors (LPSTR lpbi)
{
   WORD wBitCount;


      // If this is a Windows style DIB, the number of colors in the
      //  color table can be less than the number of bits per pixel
      //  allows for (i.e. lpbi->biClrUsed can be set to some value).
      //  If this is the case, return the appropriate value.

   if (IS_WIN30_DIB (lpbi))
      {
      DWORD dwClrUsed;

      dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

      if (dwClrUsed)
         return (WORD) dwClrUsed;
      }


      // Calculate the number of colors in the color table based on
      //  the number of bits per pixel for the DIB.

   if (IS_WIN30_DIB (lpbi))
      wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER) lpbi)->bcBitCount;

   switch (wBitCount)
      {
      case 1:
         return 2;

      case 4:
         return 16;

      case 8:
         return 256;

      default:
         return 0;
      }
}



//---------------------------------------------------------------------
//
// Function:   PaletteSize
//
// Purpose:    Given a pointer to a DIB, returns number of bytes
//             in the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD PaletteSize (LPSTR lpbi)
{
   if (IS_WIN30_DIB (lpbi))
      return (DIBNumColors (lpbi) * sizeof (RGBQUAD));
   else
      return (DIBNumColors (lpbi) * sizeof (RGBTRIPLE));
}





extern LPSTR FindDIBBits (LPSTR lpbi);



//---------------------------------------------------------------------
//
// Function:   DIBHeight
//
// Purpose:    Given a pointer to a DIB, returns its height.  Note
//             that it returns a DWORD (since a Win30 DIB can have
//             a DWORD in its height field), but under Win30, the
//             high order word isn't used!
//
// Parms:      lpDIB == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

DWORD DIBHeight (LPSTR lpDIB)
{
   LPBITMAPINFOHEADER lpbmi;
   LPBITMAPCOREHEADER lpbmc;

   lpbmi = (LPBITMAPINFOHEADER) lpDIB;
   lpbmc = (LPBITMAPCOREHEADER) lpDIB;

   if (lpbmi->biSize == sizeof (BITMAPINFOHEADER))
      return lpbmi->biHeight;
   else
      return (DWORD) lpbmc->bcHeight;
}


//---------------------------------------------------------------------
//
// Function:   DIBWidth
//
// Purpose:    Given a pointer to a DIB, returns its width.  Note
//             that it returns a DWORD (since a Win30 DIB can have
//             a DWORD in its width field), but under Win30, the
//             high order word isn't used!
//
// Parms:      lpDIB == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

DWORD DIBWidth (LPSTR lpDIB)
{
   LPBITMAPINFOHEADER lpbmi;
   LPBITMAPCOREHEADER lpbmc;

   lpbmi = (LPBITMAPINFOHEADER) lpDIB;
   lpbmc = (LPBITMAPCOREHEADER) lpDIB;

   if (lpbmi->biSize == sizeof (BITMAPINFOHEADER))
      return lpbmi->biWidth;
   else
      return (DWORD) lpbmc->bcWidth;
}







//---------------------------------------------------------------------
//
// Function:   DIBPrint
//
// Purpose:    This routine drives the printing operation.  It has the code
//             to handle both banding and non-banding printers.  A banding
//             printer can be distinguished by the GetDeviceCaps() API (see
//             the code below.  On banding devices, must repeatedly call the
//             NEXTBAND escape to get the next banding rectangle to print
//             into.  If the device supports the BANDINFO escape, it should
//             be used to determine whether the band "wants" text or
//             graphics (or both).  On non-banding devices, we can ignore
//             all this and call PrintPage() on the entire page!
//
// Parms:      hDIB        == Handle to global memory with a DIB spec in it.
//                              can be either a Win30 DIB or an OS/2 PM DIB.
//             lpPrintRect == Rect to print (decoded based on next parm)
//             wUnits      == Units lpPrintRect is in (see
//                              TranslatePrintRect()).
//             dwROP       == Raster operation to use.
//!!!!!!!!!!!!!!!!!!!!dwROP isn't used !!!!!!!!!!!!!!!!!!!!!!
//             fBanding    == TRUE when want to do banding (use NEXTBAND).
//
// Returns:   Encoded error value -- bitwise combination of ERR_PRN_*
//             in PRINT.H.  More than one error can be returned --
//             the application can parse the bits in the DWORD returned,
//             or call ShowPrintError() to display all the errors
//             that ocurred.
//
// History:   Date      Reason
//             6/01/91  Created
//            10/26/91  Added error return codes.
//                      Use DeviceSupportsEscape() instead
//                        of QUERYESCSUPPORT.
//            10/29/91  Added the fUse31APIs flag.
//            11/14/91  Added more error checking.
//                      Added lpDocName as a parameter.
//
//---------------------------------------------------------------------

DWORD DIBPrint (HDC hPrnDC,
								HANDLE hDIB,
                LPRECT lpPrintRect,
                  WORD wUnits,
                 DWORD dwROP,
                  BOOL fBanding,
									BOOL fUse31APIs)
	{
	 RECT           rect;
	 LPSTR          lpDIBHdr, lpBits;


      // Do some setup (like getting pointers to the DIB and its header,
			//  and a printer DC).  Also, set the global gbUseEscapes to force
      //  using printer escapes or the 3.1 printing API.

   if (!hDIB)
			return 0;

   gbUseEscapes = !fUse31APIs;
	 lpDIBHdr     = (LPSTR)GlobalLock (hDIB);
   lpBits       = FindDIBBits (lpDIBHdr);

	 if (hPrnDC)
      {
      SetStretchBltMode (hPrnDC, COLORONCOLOR);
      TranslatePrintRect (hPrnDC,
                          lpPrintRect,
                          wUnits,
                          (WORD) DIBWidth (lpDIBHdr),
                          (WORD) DIBHeight (lpDIBHdr));


         // Initialize the abort procedure.  Then STARTDOC.

			bAbort      = FALSE;

	
      if (fBanding)
				 BandDIBToPrinter (hPrnDC, lpDIBHdr, lpBits, lpPrintRect);
      else
         {
            // When not doing banding, call PrintABand() to dump the
            //  entire page to the printer in one shot (i.e. give it
						//  a band that covers the entire printing rectangle,
						//  and tell it to print graphics and text).

				 rect = *lpPrintRect;

				PrintABand (hPrnDC,
															lpPrintRect,
															&rect,
															TRUE,
															TRUE,
															lpDIBHdr,
															lpBits);


				 // Non-banding devices need the NEWFRAME or EndPage() call.

				 }



		 }

				 // All done, clean up.


	 GlobalUnlock (hDIB);
	 I_UNUSED(dwROP);
	 return 1;
}



//---------------------------------------------------------------------
//
// Function:   BandDIBToPrinter
//
// Purpose:    Repeatedly call the NEXTBAND escape to get the next
//             banding rectangle to print into.  If the device supports
//             the BANDINFO escape, use it to determine whether the band
//             wants text or graphics (or both).  For each band, call
//             PrintABand() to do the actual output.
//
// Parms:      hPrnDC   == DC to printer.
//             lpDIBHdr == Ptr to DIB header (BITMAPINFOHEADER or
//                         BITMAPCOREHEADER)
//             lpBits   == Ptr to DIB's bitmap bits.
//
// Returns:    WORD -- One (or more) of the printer errors defined as
//             ERR_PRN_* in PRINT.H.
//
//             ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//            10/26/91  Chopped out of DIBPrint().
//                      Use DeviceSupportsEscape() instead of
//                        QUERYESCSUPPORT.
//            11/14/91  Added Error return codes ERR_PRN_BANDINFO
//                        and errors from PrintABand.
//            01/22/91  Fixed NEXTBAND error return check (was checking
//                        if != 0, now check if > 0).
//
//---------------------------------------------------------------------

DWORD BandDIBToPrinter (HDC hPrnDC,
                      LPSTR lpDIBHdr,
                      LPSTR lpBits,
                     LPRECT lpPrintRect)
{
   BANDINFOSTRUCT bi;
   BOOL           bBandInfoDevice;
   RECT           rect;
   DWORD          dwError = ERR_PRN_NONE;
   int            nEscRet;


      // All printers should support the NEXTBAND escape -- we'll
      //  check here, just in case, though!

   if (!DeviceSupportsEscape (hPrnDC, NEXTBAND))
      return ERR_PRN_CANTBAND;


      // Check if device supports the BANDINFO escape.  Then setup
      //  the BANDINFOSTRUCT (we'll use the values we put into it
      //  here later even if the device doesn't support BANDINFO).

   bBandInfoDevice = DeviceSupportsEscape (hPrnDC, BANDINFO);
   bi.bGraphics    = TRUE;
   bi.bText        = TRUE;
   bi.GraphicsRect = *lpPrintRect;


      // Enter the banding loop.  For each band, call BANDINFO if
      //  appropriate.  Then call PrintABand() to do the actual
      //  output.  Terminate loop when NEXTBAND returns an empty rect.

   while (((nEscRet = Escape (hPrnDC, NEXTBAND, NULL, NULL, (LPSTR) &rect)) > 0) &&
         !IsRectEmpty (&rect))
      {
      if (bBandInfoDevice)
         if (!Escape (hPrnDC,
                      BANDINFO,
                      sizeof (BANDINFOSTRUCT),
                      (LPSTR) &bi,
                      (LPSTR) &bi))
            dwError |= ERR_PRN_BANDINFO;

      dwError |= PrintABand (hPrnDC,
                             lpPrintRect,
                             &rect,
                             bi.bText,
                             bi.bGraphics,
                             lpDIBHdr,
                             lpBits);
      }

   if (nEscRet <= 0)
      dwError |= ERR_PRN_NEXTBAND;

   return dwError;
}




//---------------------------------------------------------------------
//
// Function:   PrintABand
//
// Purpose:    This routine does ALL output to the printer.  It is driven by
//             BandDIBToPrinter().  It is called for both banding and non-
//             banding printing devices.  lpClipRect contains the rectangular
//             area we should do our output into (i.e. we should clip our
//             output to this area).  The flags fDoText and fDoGraphics
//             should be set appropriately (if we want any text output to
//             the rectangle, set fDoText to true).  Normally these flags
//             are returned on banding devices which support the BANDINFO
//             escape.  On non-banding devices, all output goes to the
//             entire page, so this routine is passes a rectangle for
//             the entire page, and fDoText = fDoGraphics = TRUE.
//
//             This routine is also responsible for doing stretching of
//             the DIB.  As such, the lpRectOut parameter points to a
//             rectangle on the printed page where the entire DIB will
//             fit -- the DIB is stretched appropriately to fit in this
//             rectangle.
//
//             After printing a band, updates the print % shown in the
//             abort dialog box.
//
// Parms:      hDC         == DC to do output into.
//             lpRectOut   == Rectangle on DC DIB should fit in.
//             lpRectClip  == Rectangle to output during THIS call.
//             fDoText     == Output text into this rectangle (unused by DIBView)?
//             fDoGraphics == Output graphics into this rectangle?
//             lpDIBHdr    == Pointer to DIB's header (either a
//                              BITMAPINFOHEADER or a BITMAPCOREHEADER)
//             lpDIBBits   == Pointer to the DIB's bitmap bits.
//
// Returns:    One or more of the ERR_PRN_* errors in PRINT.H (or'd
//             together. ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

DWORD PrintABand (HDC hDC,
               LPRECT lpRectOut,
               LPRECT lpRectClip,
                 BOOL fDoText,
                 BOOL fDoGraphics,
                LPSTR lpDIBHdr,
                LPSTR lpDIBBits)
{
   int    nxLogPix, nyLogPix;
   RECT   rect;
   double dblXScaling, dblYScaling;
   DWORD  dwError = ERR_PRN_NONE;


   if (fDoGraphics)
      {
      nxLogPix = GetDeviceCaps (hDC, LOGPIXELSX);
      nyLogPix = GetDeviceCaps (hDC, LOGPIXELSY);

      dblXScaling = ((double) lpRectOut->right - lpRectOut->left) /
                     DIBWidth (lpDIBHdr);
      dblYScaling = ((double) lpRectOut->bottom - lpRectOut->top) /
                     DIBHeight (lpDIBHdr);


         // Now we set up a temporary rectangle -- this rectangle
         //  holds the coordinates on the paper where our bitmap
         //  WILL be output.  We can intersect this rectangle with
         //  the lpClipRect to see what we NEED to output to this
         //  band.  Then, we determine the coordinates in the DIB
         //  to which this rectangle corresponds (using dbl?Scaling).

      IntersectRect (&rect, lpRectOut, lpRectClip);

      if (!IsRectEmpty (&rect))
         {
         RECT rectIn;
         int  nPct;

         rectIn.left   = (int) ((rect.left - lpRectOut->left) /
                                 dblXScaling + 0.5);
         rectIn.top    = (int) ((rect.top  - lpRectOut->top) /
                                 dblYScaling + 0.5);
         rectIn.right  = (int) (rectIn.left + (rect.right  - rect.left) /
                                 dblXScaling + 0.5);
         rectIn.bottom = (int) (rectIn.top  +  (rect.bottom - rect.top) /
                                 dblYScaling + 0.5);

            // Could just always call StretchDIBits() below, but
            //  we want to give SetDIBitsToDevice() a work out, too!

         if ((rect.right - rect.left == rectIn.right - rectIn.left) &&
             (rect.bottom - rect.top == rectIn.bottom - rectIn.top))
            {
            if (!SetDIBitsToDevice (hDC,                            // DestDC
                                    rect.left,                      // DestX
                                    rect.top,                  // DestY
                                    rect.right - rect.left,    // DestWidth
                                    rect.bottom - rect.top,    // DestHeight
                                    rectIn.left,               // SrcX
                                    (int) DIBHeight (lpDIBHdr)-// SrcY
                                       rectIn.top -
                                       (rectIn.bottom - rectIn.top),
                                    0,                         // nStartScan
                                    (int) DIBHeight (lpDIBHdr),// nNumScans
                                    lpDIBBits,                 // lpBits
                                    (LPBITMAPINFO) lpDIBHdr,   // lpBitInfo
                                    DIB_RGB_COLORS))           // wUsage
               dwError |= ERR_PRN_SETDIBITSTODEV;
            }
         else
            {
            if (!StretchDIBits (hDC,                           // DestDC
                                rect.left,                     // DestX
                                rect.top,                      // DestY
                                rect.right - rect.left,        // DestWidth
                                rect.bottom - rect.top,        // DestHeight
                                rectIn.left,                   // SrcX
                                (int) DIBHeight (lpDIBHdr) -   // SrcY
                                   rectIn.top -
                                   (rectIn.bottom - rectIn.top),
                                rectIn.right - rectIn.left,    // SrcWidth
                                rectIn.bottom - rectIn.top,    // SrcHeight
                                lpDIBBits,                     // lpBits
                                (LPBITMAPINFO) lpDIBHdr,       // lpBitInfo
                                DIB_RGB_COLORS,                // wUsage
                                SRCCOPY))                      // dwROP
               dwError |= ERR_PRN_STRETCHDIBITS;
            }


            // Change percentage of print shown in abort dialog.

         nPct = MulDiv (rect.bottom,
                        100,
                        lpRectOut->bottom);
         ChangePrintPercent (nPct);
         }
      }

	 I_UNUSED(nyLogPix);
	 I_UNUSED(nxLogPix);
	 I_UNUSED(fDoText);

   return dwError;
}



//---------------------------------------------------------------------
//
// Function:   DeviceSupportsEscape
//
// Purpose:    Uses QUERYESCSUPPORT to see if the given device
//             supports the given escape code.
//
// Parms:      hDC         == Device to check if escape is supported on.
//             nEscapeCode == Escape code to check for.
//
// History:   Date      Reason
//            10/26/91  Created
//
//---------------------------------------------------------------------

BOOL DeviceSupportsEscape (HDC hDC, int nEscapeCode)
{
   return Escape(hDC, QUERYESCSUPPORT, sizeof(int), (LPSTR) &nEscapeCode, NULL);
}



//---------------------------------------------------------------------
//
// Function:   TranslatePrintRect
//
// Purpose:    Given a rectangle and what units that rectangle is in,
//             translate the rectangle to the appropriate value in
//             device units.
//
// Parms:      hDC         == DC translation is relative to.
//             lpPrintRect == Pointer to rectangle to translate.
//             wUnits      == Units lpPrintRect is in:
//                              UNITS_INCHES == Units are in inches, stretch
//                                                DIB to this size on page.
//                              UNITS_STRETCHTOPAGE == lpPrintRect doesn't
//                                                matter, stretch DIB to
//                                                fill the entire page.
//                              UNITS_BESTFIT == lpPrintRect doesn't matter,
//                                                stretch DIB as much as
//                                                possible horizontally,
//                                                and preserve its aspect
//                                                ratio vertically.
//                              UNITS_SCALE == lpPrintRect->top is factor to
//                                                stretch vertically.
//                                                lpPrintRect->left is
//                                                factor to stretch horiz.
//                              UNITS_PIXELS == lpPrintRect is in pixels.
//             cxDIB       == DIB's width.
//             cyDIB       == DIB's height.
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

void TranslatePrintRect (HDC hDC,
                      LPRECT lpPrintRect,
                        WORD wUnits,
                        WORD cxDIB,
                        WORD cyDIB)
{
   int cxPage, cyPage, cxInch, cyInch;

   if (!hDC)
      return;

   cxPage = GetDeviceCaps (hDC, HORZRES);
   cyPage = GetDeviceCaps (hDC, VERTRES);
   cxInch = GetDeviceCaps (hDC, LOGPIXELSX);
   cyInch = GetDeviceCaps (hDC, LOGPIXELSY);

   switch (wUnits)
      {
         // lpPrintRect contains units in inches.  Convert to pixels.

      case UNITS_INCHES:
         lpPrintRect->top    *= cyInch;
         lpPrintRect->left   *= cxInch;
         lpPrintRect->bottom *= cyInch;
         lpPrintRect->right  *= cxInch;
         break;


         // lpPrintRect contains no pertinent info -- create a rectangle
         //  which covers the entire printing page.

      case UNITS_STRETCHTOPAGE:
         lpPrintRect->top    = 0;
         lpPrintRect->left   = 0;
         lpPrintRect->bottom = cyPage;
         lpPrintRect->right  = cxPage;
         break;


         // lpPrintRect contains no pertinent info -- create a rectangle
         //  which preserves the DIB's aspect ratio, and fills the page
         //  horizontally.  NOTE:  Assumes DIB is 1 to 1 aspect ratio,
         //  could use biXPelsPerMeter in a DIB to munge these values
         //  for non 1 to 1 aspect ratio DIBs (I've never seen such
         //  a beast, though)!

      case UNITS_BESTFIT:
         lpPrintRect->top    = 0;
         lpPrintRect->left   = 0;
         lpPrintRect->bottom = (int)(((double) cyDIB * cyPage * cyInch) /
                                     ((double) cxDIB * cxInch));
         lpPrintRect->right  = cxPage;
         break;



         // lpPrintRect's top/left contain multipliers to multiply the
         //  DIB's height/width by.

      case UNITS_SCALE:
         {
         int cxMult, cyMult;

         cxMult              = lpPrintRect->left;
         cyMult              = lpPrintRect->top;
         lpPrintRect->top    = 0;
         lpPrintRect->left   = 0;
         lpPrintRect->bottom = cyDIB * cyMult;
         lpPrintRect->right  = cxDIB * cxMult;
         }


         // lpPrintRect already contains device units, don't touch it.

      case UNITS_PIXELS:
      default:
         // Don't touch the units...
         break;
			}

      I_UNUSED(szPrintDlg);
}








