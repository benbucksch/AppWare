

   // Units used for wUnits in DIBPrint().

#define UNITS_INCHES          0        // lpPrintRect in inches
#define UNITS_STRETCHTOPAGE   1        // lpPrintRect n/a -- force to page size
#define UNITS_BESTFIT         2        // lpPrintRect n/a -- force to as large as possible
#define UNITS_SCALE           3        // lpPrintRect.top/left tells the stretch factor for x/y
#define UNITS_PIXELS          4        // lpPrintRect in pixels


   // Error return codes for DIBPrint().  Error are returned in a bitfield,
   //  since more than one error can ocurr during the print.

#define ERR_PRN_NONE             0x00000000   // No error -- everything's A-OK!
#define ERR_PRN_NODIB            0x00000001   // No DIB specified.
#define ERR_PRN_NODC             0x00000002   // Couldn't get printer's DC.
#define ERR_PRN_CANTBAND         0x00000004   // NEXTBAND not supported by printer.
#define ERR_PRN_BANDINFO         0x00000008   // Error on BANDINFO escape.
#define ERR_PRN_SETDIBITSTODEV   0x00000010   // Error in call to SetDIBitsToDevice().
#define ERR_PRN_STRETCHDIBITS    0x00000020   // Error in call to StretchDIBits().
#define ERR_PRN_STARTDOC         0x00000040   // Error in STARTDOC escape.
#define ERR_PRN_SETABORTPROC     0x00000080   // Error in SETABORTPROC escape.
#define ERR_PRN_STARTPAGE        0x00000100   // Error in StartPage().
#define ERR_PRN_NEWFRAME         0x00000200   // Error in NEWFRAME or EndPage().
#define ERR_PRN_ENDDOC           0x00000400   // Error in ENDDOC or EndDoc().
#define ERR_PRN_NEXTBAND         0x00000800   // Error in NEXTBAND.
#define ERR_PRN_NOFNSTARTDOC     0x00001000   // Error finding StartDoc() in GDI.
#define ERR_PRN_NOFNSETABORTPROC 0x00002000   // Error finding SetAbortProc() in GDI.
#define ERR_PRN_NOFNSTARTPAGE    0x00004000   // Error finding StartPage() in GDI.
#define ERR_PRN_NOFNENDPAGE      0x00008000   // Error finding EndPage() in GDI.
#define ERR_PRN_NOFNENDDOC       0x00010000   // Error findind EndDoc() in GDI.


   // String table defines used by PRINT.C.

#define IDS_PRN_NONE              800  // No error -- everything's A-OK!
#define IDS_PRN_NODIB             801  // No DIB specified.
#define IDS_PRN_NODC              802  // Couldn't get printer's DC.
#define IDS_PRN_CANTBAND          803  // NEXTBAND not supported by printer.
#define IDS_PRN_BANDINFO          804  // Error on BANDINFO escape.
#define IDS_PRN_SETDIBITSTODEV    805  // Error in call to SetDIBitsToDevice().
#define IDS_PRN_STRETCHDIBITS     806  // Error in call to StretchDIBits().
#define IDS_PRN_STARTDOC          807  // Error in STARTDOC escape.
#define IDS_PRN_SETABORTPROC      808  // Error in SETABORTPROC escape.
#define IDS_PRN_STARTPAGE         809  // Error in StartPage().
#define IDS_PRN_NEWFRAME          810  // Error in NEWFRAME or EndPage().
#define IDS_PRN_ENDDOC            811  // Error in ENDDOC or EndDoc().
#define IDS_PRN_NEXTBAND          812  // Error in NEXTBAND.
#define IDS_PRN_NOFNSTARTDOC      813  // Error finding StartDoc() in GDI.
#define IDS_PRN_NOFNSETABORTPROC  814  // Error finding SetAbortProc() in GDI.
#define IDS_PRN_NOFNSTARTPAGE     815  // Error finding StartPage() in GDI.
#define IDS_PRN_NOFNENDPAGE       816  // Error finding EndPage() in GDI.
#define IDS_PRN_NOFNENDDOC        817  // Error findind EndDoc() in GDI.


   // Dialog box defines.

#define IDD_PRNPCT            100      // Print Percentage Done in PrintDlg


   // User defined messages.

#define MYWM_CHANGEPCT        WM_USER+1000   // Sent to abort dialog to change % done text


   // Function prototypes for functions called outside PRINT.C.

DWORD DIBPrint (HDC 	 hPrnDC,
								HANDLE hDIB,
                LPRECT lpPrintRect,
                  WORD wUnits,
                 DWORD dwROP,
                  BOOL fBanding,
									BOOL fUse31APIs);

void ShowPrintError (HWND hWnd, DWORD dwError);


#ifdef PICTPRINT
// The following typedef's are for printing functions.  They are defined
//  in PRINT.H (!!!!!!!!!!!!!!!!!!!!!!!?????) included with the 3.1
//  SDK -- as this app is supposed to compile in 3.0, I define them
//  here instead.

typedef struct
   {
   BOOL bGraphics;            // Band includes graphics
   BOOL bText;                // Band includes text.
   RECT GraphicsRect;         // Rectangle to output graphics into.
   }
BANDINFOSTRUCT;



// LPDOCINFO is now defined in 3.1's WINDOWS.H.  We're compiling under
//  both 3.0 and 3.1.  For now, we'll define our own LPDOCINFO here.
//  This is a LESS than adequate solution!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

typedef struct
   {
   short cbSize;
   LPSTR lpszDocName;
   LPSTR lpszOutput;
   }
OURDOCINFO, far * LPOURDOCINFO;



// The following typedef's and string variables are used to link to
//  printing functions on-the-fly in Windows 3.1.  These API are not
//  present in Windows 3.0!  As such, we must use GetModuleHandle() and
//  GetProcAddress() to find and call them.

typedef int (FAR PASCAL *LPSTARTDOC)     (HDC, LPOURDOCINFO);
typedef int (FAR PASCAL *LPSETABORTPROC) (HDC, FARPROC);
typedef int (FAR PASCAL *LPSTARTPAGE)    (HDC);
typedef int (FAR PASCAL *LPENDPAGE)      (HDC);
typedef int (FAR PASCAL *LPENDDOC)       (HDC);


   // The following strings are used to link to function within
   //  GDI on-the-fly.  These functions were added in 3.1.  We
   //  can't call them directly, because that would not allow this
   //  application to run under Windows 3.0.  We, therefore, use
   //  GetModuleHandle()/GetProcAddress() to link to these functions.
   //  See FindGDIFunction() below.

char szGDIModule[]    = "GDI";         // Module name for GDI in Win31.
char szStartDoc[]     = "StartDoc";    // StartDoc() function in GDI.
char szSetAbortProc[] = "SetAbortProc";// SetAbortProc() function in GDI.
char szStartPage[]    = "StartPage";   // StartPage() function in GDI.
char szEndPage[]      = "EndPage";     // EndPage function in GDI.
char szEndDoc[]       = "EndDoc";      // EndDoc function in GDI.




// Globals for this module.

static HWND hDlgAbort    = NULL;        // Handle to abort dialog box.
static char szPrintDlg[] = "PrintDLG";  // Name of Print dialog from .RC
static BOOL bAbort       = FALSE;       // Abort a print operation?
static BOOL gbUseEscapes = TRUE;        // Use Escape() or 3.1 printer APIs?


// Macros

#define ChangePrintPercent(nPct)    SendMessage(hDlgAbort, MYWM_CHANGEPCT, nPct, NULL)
	 // Given a pointer to a DIB header, return TRUE if is a Windows 3.0 style
   //  DIB, false if otherwise (PM style DIB).

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD) (lpbi)) == sizeof (BITMAPINFOHEADER))



// Function prototypes.

BOOL FAR PASCAL __export PrintAbortProc (HDC hDC, short code);
int  FAR PASCAL __export PrintAbortDlg  (HWND hWnd,
                            unsigned msg,
                                WORD wParam,
                                LONG lParam);

DWORD    BandDIBToPrinter    (HDC hPrnDC,
                            LPSTR lpDIBHdr,
                            LPSTR lpBits,
                           LPRECT lpPrintRect);
DWORD    PrintABand          (HDC hDC,
                           LPRECT lpRectOut,
                           LPRECT lpRectClip,
                             BOOL fDoText,
                             BOOL fDoGraphics,
                            LPSTR lpDIBHdr,
                            LPSTR lpDIBBits);
HDC     GetPrinterDC        (void);
void    TranslatePrintRect  (HDC hDC,
                          LPRECT lpPrintRect,
                            WORD wUnits,
                            WORD cxDIB,
                            WORD cyDIB);
DWORD    DoStartDoc          (HDC hPrnDC, LPSTR lpszDocName);
DWORD    DoEndPage           (HDC hPrnDC);
DWORD    DoSetAbortProc      (HDC hPrnDC,
                          FARPROC lpfnAbortProc);
DWORD    DoStartPage         (HDC hPrnDC);
DWORD    DoEndPage           (HDC hPrnDC);
DWORD    DoEndDoc            (HDC hPrnDC);
BOOL     DeviceSupportsEscape(HDC hDC,
                             int nEscapeCode);

#endif
