
#define ASTRICT
#define STRICT

#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <mem.h>
#include <string.h>

#include "a_alm.h"
#include "a_almutl.h"
#include "intdebug.h"
#include "o_wnd.h"

#define I_GETFONTHEIGHTASCENT		1
#define I_GETFONTHEIGHTCELL			2
#define I_GETFONTHEIGHTFULL			3

#define I_WINDOW_BACKGROUND			0xFF000000L
#define I_WINDOW_FOREGROUND			0xFE000000L

#include "attribut.h"

////////////////////
// Converts Decipoints to TMM (10ths of a milimeter)
// The output is negitive, because CreateFont use a negitive height to specify char height
// as opposed to cell height.
// The input can be positive
////////////////////
int ConvertDecipointsToTmm(
	int											decipoints)
	{
	double									d;
	
	// Unit cancelization
	//
	// 	|decipoints * tmm |  / decipoints
	// 	|						 inch	| /		inch
	//

	d =	(double)(abs(decipoints));
	d *= OWND_TMM_PER_INCH;
	d /= 720.0;/*decipoints/inch*/
	d += 0.5;	//to round

	return (int)-d;
	}

////////////////////
// Converts TMM (10ths of a milimeter) to Decipoints
// Also, for the sake of the Mac, makes sure Decipoints is a multiple of 10
// The output is positive.  The input may be negitive
////////////////////
int ConvertTmmToDecipoints(
	int											tmm)
	{
	double									d;
	int											i;

	// Unit cancelization
	//
	// tmm * decipoints  * inch
	//			  inch			    tmm
	//
	
	d = (double)(abs(tmm));
	d *= 720.0;	 //decipoints per inch
	d *= OWND_INCH_PER_TMM;

	i = (int)d;
	i += 5;
	i /=10;	 //to make sure its a rounded multiple of 10
	i *= 10;

	return i;
	}




///////////////////////////////////////////////
///  Attribute functions
///////////////////////////////////////////////

ATTRIBUTES::ATTRIBUTES(
	BOOL										createStuff) //Defaults to TRUE--FALSE is used by tmp objects
	{
	TEXTMETRIC   						tm;
	HDC											hdc;
	double									flFontAscent;

	if (createStuff)
  	{
		hdc = CreateDC("Display", NULL, NULL, NULL);
		GetTextMetrics(hdc, &tm);
		GetTextFace(hdc, LF_FACESIZE, m_szFace);

		flFontAscent = (double)tm.tmAscent/*pixels*/ * OWND_TMM_PER_INCH / (double)GetDeviceCaps(hdc, LOGPIXELSY);
		flFontAscent += 0.5; //round

		DeleteDC(hdc);

		m_fontHeightInTMM	= (int)-flFontAscent;
		m_fontWeight			= tm.tmWeight;
		m_bItalic					= tm.tmItalic;
		m_bUnderlined			= tm.tmUnderlined;
		m_bStruckout			= tm.tmStruckOut;

		m_clrForeground		= I_WINDOW_FOREGROUND;
		m_clrBackground		= I_WINDOW_BACKGROUND;
	
		I_ASSERT(m_fontHeightInTMM < 0);
		}

	m_hfont = NULL;
	m_hbrush = NULL;
	}

/////////////

ATTRIBUTES::ATTRIBUTES(
	ATTRIBUTES  *				attSource)
	{
	m_hfont 							= NULL;
	m_hbrush 							= NULL;
	m_fontHeightInTMM 		= attSource->m_fontHeightInTMM;
	m_fontWeight					= attSource->m_fontWeight;
	m_bItalic							=	attSource->m_bItalic;					
	m_bUnderlined					=	attSource->m_bUnderlined;			
	m_bStruckout					=	attSource->m_bStruckout;			
	m_clrForeground				=	attSource->m_clrForeground;		
	m_clrBackground				=	attSource->m_clrBackground;
	lstrcpy(m_szFace, attSource->m_szFace);

	I_ASSERT(m_fontHeightInTMM < 0);
	}

/////////////

ATTRIBUTES::~ATTRIBUTES()
	{
	ADBG_EVAL
		(
		if (m_hfont)
			ADbgPrint(ADBG_WARNING, 0, "Undeleted attribute font handle");
		if (m_hbrush)
			ADbgPrint(ADBG_WARNING, 0, "Undeleted attribute brush handle");
		)
	}


////////////
void ATTRIBUTES::AttrInitialize()
	{
	m_hfont=NULL;
	m_hbrush=NULL;
	}

////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

BOOL ATTRIBUTES::AttrCreateFont(
	HDC											hdc)
	{
	m_hfont = OWndCreateFontTmm(hdc, m_fontHeightInTMM, 0, 0, 0, 
			m_fontWeight, m_bItalic, m_bUnderlined, m_bStruckout,
			1, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,
			m_szFace);

  return m_hfont ? TRUE : FALSE;
	}

///////////////////////////

BOOL ATTRIBUTES::AttrDeleteFont()
	{
	BOOL										rVal;
  if (m_hfont)
		{
		OWndDeleteFont(m_hfont);
	  m_hfont=0;
		rVal = TRUE;
		}
	else
		rVal = FALSE;

  return rVal;
  }

/////////////

HFONT ATTRIBUTES::AttrGetFontHandle()
	{
	return m_hfont;
	}

///////////////

HFONT ATTRIBUTES::AttrSelectFont(
	HDC 										hdc)
	{
	return (HFONT)SelectObject(hdc, m_hfont);
	}

///////////////

BOOL ATTRIBUTES::AttrSetFont(
	HWND										hWnd)
	{
	BOOL										rVal; 

	if ( (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0L) != m_hfont)
		{
		SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hfont, MAKELONG(TRUE,0) );
		rVal = TRUE;
		}
	else
		rVal = FALSE;

	return rVal;
	}

///////////////////////////	 

BOOL ATTRIBUTES::AttrChangeFont(
	HDC 										hdcTarget, 
	HWND 										hWndSelectedInto, 
	LONG 										fChange, 
	LPSTR 									szFace, 				//default=NULL
	int 										iHeight, 					//default=0
	int 										iWeight, 				//default=0
	BYTE 										bItalic, 				//default=FALSE
	BYTE 										bUnderlined, 		//default=FALSE
	BYTE 										bStruckout)			//default=FALSE
	{
	HFONT										hOldFont;
	BOOL										rVal=FALSE; //return true if a change was made

	if ( (fChange & ATTR_CHANGE_FACE) && lstrcmp(m_szFace, szFace) )
		{
		lstrcpy(m_szFace, szFace);
    rVal = TRUE;
		}
	
	if ( (fChange & ATTR_CHANGE_HEIGHT) && -m_fontHeightInTMM != iHeight)
		{
		m_fontHeightInTMM = -iHeight;
		if(m_fontHeightInTMM < 0)
			m_fontHeightInTMM = -m_fontHeightInTMM;
//		I_ASSERT(m_fontHeightInTMM < 0);
    rVal = TRUE;
		}

	if ( (fChange & ATTR_CHANGE_WEIGHT) && m_fontWeight != iWeight)
		{
		m_fontWeight = iWeight;
		rVal = TRUE;
		}

	if ( (fChange & ATTR_CHANGE_ITALIC) && m_bItalic != bItalic)
		{
		m_bItalic = bItalic;
		rVal = TRUE;
		}

	if ( (fChange & ATTR_CHANGE_UNDERLINED) && m_bUnderlined != bUnderlined)
		{
		m_bUnderlined = bUnderlined;
		rVal = TRUE;
		}

	if ( (fChange & ATTR_CHANGE_STRUCKOUT) && m_bStruckout != bStruckout)
		{
		m_bStruckout = bStruckout;
		rVal = TRUE;
		}
	
	//Now that we have changed all the members, we can change the handle if necessary

	if (m_hfont && rVal) //If we have a font handle, and something changed
		{
		hOldFont = m_hfont;
		AttrCreateFont(hdcTarget); //Create the new one.
		if (m_hfont == hOldFont)
			rVal = FALSE;
		else if (hWndSelectedInto)
			SendMessage(hWndSelectedInto, WM_SETFONT, (WPARAM)m_hfont, MAKELONG(TRUE, 0) );

		OWndDeleteFont(hOldFont); //Delete the old font (even if same,to decrement to lock count)
		}
	
	return rVal;
	}

///////

BOOL ATTRIBUTES::AttrChangeFontIndirect(
	HDC 										hdcTarget, 
	HWND 										hWndSelectedInto, 
	long 										fChange, 
	ATTRIBUTES  *				attr)
	{
	return AttrChangeFont(hdcTarget, hWndSelectedInto, fChange, 
													attr->m_szFace,
													attr->m_fontHeightInTMM,
													attr->m_fontWeight,
													attr->m_bItalic,
													attr->m_bUnderlined,
													attr->m_bStruckout);
	}

/////////////////////////////////

int ATTRIBUTES::AttrGetFontHeight(
	int											opCode)
	{
	HDC											hdc;
	HFONT										hFont;
	TEXTMETRIC    					tm;
  int											rVal;

	hdc = CreateDC("Display", NULL, NULL, NULL);

	hFont = OWndCreateFontTmm(hdc, m_fontHeightInTMM, 0, 0, 0, 
			m_fontWeight, m_bItalic, m_bUnderlined, m_bStruckout,
			1, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,
			m_szFace);

	hFont = (HFONT)SelectObject(hdc, hFont);
	GetTextMetrics(hdc, &tm);
	hFont = (HFONT)SelectObject(hdc, hFont);
	OWndDeleteFont(hFont);
	DeleteDC(hdc);

	switch(opCode)
		{
		case I_GETFONTHEIGHTCELL:
			rVal = tm.tmHeight;
      break;
		case I_GETFONTHEIGHTFULL:
			rVal = tm.tmHeight + tm.tmExternalLeading;
      break;
		case I_GETFONTHEIGHTASCENT:
    default:
			rVal = tm.tmAscent;
      break;
		}

  return rVal;
	}


int ATTRIBUTES::AttrGetFontHeightAscent()
	{
	return AttrGetFontHeight(I_GETFONTHEIGHTASCENT);
	}

int ATTRIBUTES::AttrGetFontCellHeight()
	{
	return AttrGetFontHeight(I_GETFONTHEIGHTCELL);
	}

int ATTRIBUTES::AttrGetFontFullHeight()
	{
	return AttrGetFontHeight(I_GETFONTHEIGHTFULL);
	}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

BOOL ATTRIBUTES::AttrCreateBrush()
	{
	m_hbrush = AUtlCreateSolidBrush(AttrGetBackground());

	return m_hbrush ? TRUE : FALSE;
	}

////////

BOOL ATTRIBUTES::AttrDeleteBrush()
	{
	BOOL										rVal;

	if (m_hbrush)
		{
		AUtlDeleteBrush(m_hbrush);
		m_hbrush = 0;
	  rVal = TRUE;
		}
	else
		rVal = FALSE;

	return rVal;
  }

/////////////////

HBRUSH ATTRIBUTES::AttrGetBrushHandle()
	{
	return m_hbrush;
	}

///////////////////

HBRUSH ATTRIBUTES::AttrSelectBrush(
	HDC 										hdc)
	{
	return (HBRUSH)SelectObject(hdc, m_hbrush);
	}
///////////////////

BOOL ATTRIBUTES::AttrUpdateBrushDueToWinIniChanged() //WM_WININICHANGED
	{
	BOOL										rVal;

	//If the brush hasn't been created, we don't need to change it
	if (m_hbrush  &&  m_clrBackground == I_WINDOW_BACKGROUND)
  	{
		AUtlDeleteBrush(m_hbrush);
		AUtlCreateSolidBrush(AttrGetBackground());
		rVal = TRUE;
		}
	else
		rVal = FALSE;
	
	return rVal;
	}

////////////////////////////////////////

COLORREF ATTRIBUTES::AttrGetForeground()
	{
	return m_clrForeground == I_WINDOW_FOREGROUND ? GetSysColor(COLOR_WINDOWTEXT):	m_clrForeground;
	}

//////

void ATTRIBUTES::AttrSetForeground(
	COLORREF 								f)
	{
	m_clrForeground = f;
	}

//////


COLORREF ATTRIBUTES::AttrGetBackground()
	{
	return m_clrBackground == I_WINDOW_BACKGROUND ? GetSysColor(COLOR_WINDOW):	m_clrBackground;
	}

//////

void ATTRIBUTES::AttrSetBackground(
	COLORREF 								b)
	{
	m_clrBackground = b;
	}

/////////////////////////////////////////////
/////////////////////////////////////////////

void ATTRIBUTES::AttrLogfontToAttributes(  //private function
	LPLOGFONT								logFont)
	{
	HDC											hdc; 
	double									flFontHeightInTMM;

	hdc = CreateDC("Display", NULL, NULL, NULL);

	flFontHeightInTMM = (double)logFont->lfHeight/*pixels*/ * OWND_TMM_PER_INCH / (double)GetDeviceCaps(hdc, LOGPIXELSY)/*pixels per inch*/;
	flFontHeightInTMM -= 0.5; //round
	DeleteDC(hdc);

	m_fontHeightInTMM	= (int) flFontHeightInTMM;
	m_fontWeight	= logFont->lfWeight;
	m_bItalic			= logFont->lfItalic;
	m_bUnderlined	= logFont->lfUnderline;
	m_bStruckout	= logFont->lfStrikeOut;
	lstrcpy(m_szFace, logFont->lfFaceName);
	
	I_ASSERT(m_fontHeightInTMM < 0);
	}

/////////////////////////////////////////

void ATTRIBUTES::AttrAttributesToLogfont(  //private function
	LPLOGFONT								logFont)
	{
	HDC											hdc; 
	double									flFontHeightInPixels;

	I_ASSERT(m_fontHeightInTMM < 0);

	hdc = CreateDC("Display", NULL, NULL, NULL);
	flFontHeightInPixels = (double)m_fontHeightInTMM/*tmm*/ * OWND_INCH_PER_TMM * (double)GetDeviceCaps(hdc, LOGPIXELSY)/*pixels per inch*/;
	flFontHeightInPixels -= 0.5;
	DeleteDC(hdc);

	FillMemory(logFont, sizeof(LOGFONT), 0);

	logFont->lfHeight			= (int)flFontHeightInPixels;
	logFont->lfWeight			= m_fontWeight;
	logFont->lfItalic			= m_bItalic;
	logFont->lfUnderline	= m_bUnderlined;
	logFont->lfStrikeOut	= m_bStruckout;
	lstrcpy(logFont->lfFaceName, m_szFace);
	}

/////////////////////////////////

void ATTRIBUTES::AttrEditFont(
	HWND          	  			owner,					//Default - NULL
	LPSTR 									szSample,        //Default - "AaBbYyZz123"
	LPSTR										szTitle,				//Default =  "Edit Attributes"
	BOOL 										doColors,       //Default - TRUE
	DWORD	          				flags,          //Default - CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT
	BOOL 										doBackground)   //Default - TRUE
	{
	LOGFONT									logFont;
	int											deciPoint;
	HDC											hdcPrinter = NULL;
	char										printer[64];
  LPSTR										device, driver, output;

	if(flags & CF_PRINTERFONTS)
  	{
		GetProfileString("windows", "device", "", printer, 64);
		if ((device = strtok(printer, "," )) != 0
    	    && (driver = strtok(NULL, ", ")) !=0
					&& (output = strtok(NULL, ", ")) !=0 )
			hdcPrinter = CreateDC(driver, device, output, NULL);
		else
			flags &= ~CF_PRINTERFONTS;
		}


	AttrAttributesToLogfont(&logFont);

	AUtlDlgEditFont(
		owner,
		szSample,
		szTitle,
		&logFont,
		&deciPoint,
		doColors,
		&m_clrForeground,
		doBackground ? &m_clrBackground : NULL,
		TRUE,
		flags,
		((flags & CF_PRINTERFONTS) && hdcPrinter) ? TRUE : FALSE,
		hdcPrinter);

	AttrLogfontToAttributes(&logFont);

	// clean up the dc for the printer
	if(hdcPrinter)
		DeleteDC(hdcPrinter);
	}


//////////////////////////////////////////
//          UPSF Support           ///////
//////////////////////////////////////////

LPSTR szUpsfAttribScope	 = "ATTRIBUT";
LPSTR szUpsfFontName		 = "FONTNAME";
LPSTR szUpsfPointSize		 = "POINTSZ";
LPSTR szUpsfForeColor		 = "FGCOLOR";
LPSTR szUpsfBackColor		 = "BGCOLOR";
LPSTR szUpsfBold				 = "BOLD";
LPSTR szUpsfItalic			 = "ITALIC";
LPSTR szUpsfStikeOut		 = "STRIKOUT";
LPSTR szUpsfUnderline		 = "UNDRLINE";

static void LogComment(
	LPSTR										szField)
	{
	char										tmpStr[255];

	wsprintf(tmpStr, "Error importing attributes field %s -- using default", szField);
	AUpsfLogComment(tmpStr);
	}

///////////////

BOOL ATTRIBUTES::Export()
	{
	int											iSize = ConvertTmmToDecipoints(m_fontHeightInTMM);
	BYTE										bBold = (BYTE)(m_fontWeight >= FW_SEMIBOLD);

	I_ASSERT(m_fontHeightInTMM < 0);

	AUpsfCreateScope(szUpsfAttribScope,	0L);
	AUpsfExportField(szUpsfFontName, 		0L, AUPSF_NATV_FONT,			AUPSF_UNIV_FONT, 		m_szFace, lstrlen(m_szFace));
	AUpsfExportField(szUpsfPointSize, 	0L, AUPSF_NATV_INT, 			AUPSF_UNIV_INTEGER, &iSize, sizeof(int));
	
	if (m_clrForeground != I_WINDOW_FOREGROUND)
		AUpsfExportField(szUpsfForeColor, 	0L, AUPSF_NATV_RGBCOLOR,	AUPSF_UNIV_COLOR, 	&m_clrForeground, sizeof(COLORREF));
	
	if (m_clrBackground != I_WINDOW_BACKGROUND)
		AUpsfExportField(szUpsfBackColor, 	0L, AUPSF_NATV_RGBCOLOR,	AUPSF_UNIV_COLOR, 	&m_clrBackground, sizeof(COLORREF));
	
	AUpsfExportField(szUpsfBold, 				0L, AUPSF_NATV_BYTE,			AUPSF_UNIV_INTEGER, 	&bBold, sizeof(BYTE));
	AUpsfExportField(szUpsfItalic, 			0L, AUPSF_NATV_BYTE,			AUPSF_UNIV_INTEGER,  &m_bItalic, sizeof(BYTE));
	AUpsfExportField(szUpsfStikeOut, 		0L, AUPSF_NATV_BYTE,			AUPSF_UNIV_INTEGER,  &m_bStruckout, sizeof(BYTE));
	AUpsfExportField(szUpsfUnderline, 	0L, AUPSF_NATV_BYTE,			AUPSF_UNIV_INTEGER,  &m_bUnderlined, sizeof(BYTE));
	AUpsfCloseScope();

	return TRUE;
	}

//////////////////////////////////////////

BOOL ATTRIBUTES::Import(void)
	{
	int											iSize;
	BYTE										bBold;

	if(AUpsfOpenScope(szUpsfAttribScope, 0L) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Unable to import the attributes.");
    return FALSE;
		}

	if(AUpsfImportField(szUpsfFontName, 0L, AUPSF_NATV_FONT, m_szFace, LF_FACESIZE) != AUPSF_NOERROR)
		{
		LogComment(szUpsfFontName);
    lstrcpy(m_szFace, "System");
		}

	if(AUpsfImportField(szUpsfPointSize, 	0L, AUPSF_NATV_INT, &iSize, sizeof(int)) != AUPSF_NOERROR)
		{
		LogComment(szUpsfPointSize);
    iSize = 100;
		}
	m_fontHeightInTMM = ConvertDecipointsToTmm(iSize);
	I_ASSERT(m_fontHeightInTMM < 0);

	if(AUpsfImportField(szUpsfForeColor, 	0L, AUPSF_NATV_RGBCOLOR, &m_clrForeground, sizeof(COLORREF)) != AUPSF_NOERROR)
    m_clrForeground = I_WINDOW_FOREGROUND;

	if(AUpsfImportField(szUpsfBackColor, 	0L, AUPSF_NATV_RGBCOLOR, &m_clrBackground, sizeof(COLORREF)) != AUPSF_NOERROR)
    m_clrBackground = I_WINDOW_BACKGROUND;
	
	if(AUpsfImportField(szUpsfBold, 				0L, AUPSF_NATV_BYTE, &bBold, sizeof(BYTE)) != AUPSF_NOERROR)
		{
		LogComment(szUpsfBold);
    bBold = FALSE;
		}
	m_fontWeight = bBold ? FW_BOLD : FW_NORMAL;

	if(AUpsfImportField(szUpsfItalic, 			0L, AUPSF_NATV_BYTE,		&m_bItalic, sizeof(BYTE)) != AUPSF_NOERROR)
		LogComment(szUpsfItalic);

	if(AUpsfImportField(szUpsfStikeOut, 		0L, AUPSF_NATV_BYTE,		&m_bStruckout, sizeof(BYTE)) != AUPSF_NOERROR)
		LogComment(szUpsfStikeOut);

	if(AUpsfImportField(szUpsfUnderline, 	0L, AUPSF_NATV_BYTE,			&m_bUnderlined, sizeof(BYTE)) != AUPSF_NOERROR)
		LogComment(szUpsfUnderline);

	AUpsfCloseScope();

	return TRUE;
	}

//////////////////////////////////////////
