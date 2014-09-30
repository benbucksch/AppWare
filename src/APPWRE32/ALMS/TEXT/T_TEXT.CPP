
#include "t_includ.h"
#include <stdlib.h>
#include "o_upsf.h"


TEXTOBJECT::TEXTOBJECT(
	AOBJECTID								id, 
	BOOL 										bIsTemp):	m_attributes(!bIsTemp) //If temp object don't createStuff in attributes
	{
	m_version = I_THE_VERSION;
  m_oiSelf = id;
	m_style = I_TF_DEFTEXTSTYLE;
	m_uLimit = 32000; 
	m_uHorzLimit = 500;
	m_iTabSize = 8;
	}

LPVOID TEXTOBJECT::operator new(
	size_t 									size,
	AOBJECTID 								id)
	{
  LPSTR									  str;
	AObjSetDataCount(id, I_NUM_DATAS);
	AObjResizeData(id, I_DATA_STYLE, size);

	//Put an empty string in the second data block
	ResizeTextString(id, 1);
	str = (LPSTR)AObjLockData(id, I_DATA_STRING);
	*str = 0;
	AObjUnlockData(id, I_DATA_STRING);

	return AObjLockData(id, I_DATA_STYLE);
	}

////////////////////////

void TEXTOBJECT::operator delete(
	LPVOID								ptr)
	{
	AObjUnlockData( ((LPTEXTOBJECT)ptr)->m_oiSelf, I_DATA_STYLE);
	}

////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////

UINT TEXTOBJECT::GetDrawTextStyle()
	{
  UINT 															drawFlags = DT_NOPREFIX | DT_EXPANDTABS;


	if (IsMultiline() || IsStatic())	drawFlags |= DT_WORDBREAK;
	else															drawFlags |= DT_SINGLELINE;

	if (JustifiesLeft()) 							drawFlags |= DT_LEFT;
	else if (JustifiesCenter())				drawFlags |= DT_CENTER;
	else															drawFlags |= DT_RIGHT;

	return drawFlags;
	}

///////////////////////////////////////////

LONG TEXTOBJECT::GetCreateWindowStyle()
	{
	LONG      							s = WS_CHILD;

	if (IsMultiline())
  	{
																	s |= ES_MULTILINE;
																	s |= ES_AUTOVSCROLL;
		if (HasVScroll())      				s |= WS_VSCROLL;
		if (HasHScroll())        			s |= (WS_HSCROLL | ES_AUTOHSCROLL);
		}
	else														s |= ES_AUTOHSCROLL;

	if (JustifiesCenter())  				s |= ES_CENTER;
	else if (JustifiesRight())			s |= ES_RIGHT;
	else														s |= ES_LEFT;

	if (IsUppercase())      				s |= ES_UPPERCASE;
	else if (IsLowercase())  		 		s |= ES_LOWERCASE;

	if (HasFrame())									s |= WS_BORDER;

	if (!IsEditable())							s |= ES_READONLY;

	if (IsPassword())								s |= ES_PASSWORD;

	if (KeepsSelection())						s |= ES_NOHIDESEL;

	return s;
	}

///////////////////////////

void TEXTOBJECT::CopyAttributes(
	LPATTRIBUTES						attTarget)
	{
	CopyMemory(attTarget, &m_attributes, sizeof(ATTRIBUTES));
  }

//////////////////////////////////

void TEXTOBJECT::ValidateObjectIDs(
	AOBJECTID 								oiNew)
	{
  m_oiSelf = oiNew;
	}

////////////////////////////////////

void TEXTOBJECT::BuildSignals(
	AMEMBLOCKID 							miSignals)
	{
	int											signalNum = 0;
	pAObjSignalRecord				signal;

	AMemSetSize(miSignals, I_NUM_SIGNALS * sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(miSignals);

	signal[signalNum].theID = I_SIG_CHANGED;
	lstrcpy(signal[signalNum++].theName, "Changed");

	if (IsEditable())
		{
		signal[signalNum].theID = I_SIG_EDITED;
		lstrcpy(signal[signalNum++].theName, "Edited");

		signal[signalNum].theID = I_SIG_ACTIVATED;
		lstrcpy(signal[signalNum++].theName, "Activated");

		signal[signalNum].theID = I_SIG_DEACTIVATED;
		lstrcpy(signal[signalNum++].theName, "Deactivated");
  	}

	signal[signalNum].theID = I_SIG_LENGTHERROR;
	lstrcpy(signal[signalNum++].theName, "Too long");

	if (IsEditable())
		{
		signal[signalNum].theID = I_SIG_ENTERKEY;
		lstrcpy(signal[signalNum++].theName, "Enter key");
   	}

	AMemSetSize(miSignals, signalNum * sizeof(AObjSignalRecord));
	AMemUnlock(miSignals);
	}


///////////////////////////////////////////

#define I_TF_HWND_STYLEBITS_FOR_200  0x7030 //(I_TF_EDITABLE | I_TF_ACTIVATABLE | I_TF_FRAME | I_TF_HORZBAR | I_TF_VERTBAR)

long TEXTOBJECT::DoVersionCheck()
	{
	long										rVal = A_OK;

	if (m_version == 200)
		{//up it from when we didn't have the static style
		if (!(m_style & I_TF_HWND_STYLEBITS_FOR_200) )
			SetIsStatic();
		else if (!(m_style & I_TF_MULTILINE))
			SetIsSingleline();
		else
			SetIsMultiline();
		m_version = 201;
		rVal = A_MODIFIED;
		}
	else if (m_version > 201)
		rVal = A_ERROR;

	return rVal;
	}


//////////////////////////////////////////
////  UPSF Support      //////////////////
//////////////////////////////////////////

LPSTR szUpsfVersion						= "VERSION";
LPSTR szUpsfSelectAll					= "SLECTALL";
LPSTR szUpsfJustify						= "JUSTIFY";
LPSTR szUpsfCaseLimit					= "CASELIMT";
LPSTR szUpsfEditable					= "EDITABLE";
LPSTR szUpsfVScroll						= "VSCROLL";
LPSTR szUpsfHScroll						= "HSCROLL";
LPSTR szUpsfFrame							= "FRAME";
LPSTR szUpsfSingleline				= "SINGLELINE";
LPSTR szUpsfMultiline					= "MULTILINE";
LPSTR szUpsfStatic						= "STATIC";
LPSTR szUpsfUseEnter					= "USEENTER";
LPSTR szUpsfUseTabs						= "USETABS";
LPSTR szUpsfTabWidth					= "TABWIDTH";
LPSTR szUpsfLimit							= "LIMIT";
LPSTR szUpsfText							= "TEXT";
LPSTR szUpsfKeepsSelection		= "KEEPSEL";

void LogError(
	LPSTR										szMember)
	{
	static char							szComment[256];

	wsprintf(szComment, "Error importing \"%s\" -- using default.", szMember);
	AUpsfLogComment(szComment);
	}

void LogInfo(
	LPSTR										szMember)
	{
	static char							szComment[256];

	wsprintf(szComment, "Info \"%s\" field missing -- using default.", szMember);
	AUpsfLogComment(szComment);
	}

///////////////////////

long TEXTOBJECT::Export()
	{
	LPSTR					szText;

	ExportShort(szUpsfVersion,				0L, m_version);
	ExportBool(szUpsfSelectAll,				0L, SelectsAll());
	ExportShort(szUpsfJustify,				0L, JustifiesLeft() ? (short)0 : JustifiesCenter() ? (short)1 : (short)2);
	ExportShort(szUpsfCaseLimit,			0L, IsLowercase() ? (short)2 : IsUppercase() ? (short)1 : (short)0);
	ExportBool(szUpsfEditable,				0L, IsEditable());
	ExportBool(szUpsfVScroll,					0L, HasVScroll());
	ExportBool(szUpsfHScroll,					0L, HasHScroll());
	ExportBool(szUpsfFrame,						0L, HasFrame());
	ExportBool(szUpsfSingleline,			0L, IsSingleline() );
	ExportBool(szUpsfMultiline,				0L, IsMultiline() );
	ExportBool(szUpsfStatic,	 				0L, IsStatic() );
	ExportBool(szUpsfUseEnter,				0L, UsesEnter());
	ExportBool(szUpsfUseTabs,					0L, UsesTab());
	ExportInt(szUpsfTabWidth ,				0L, m_iTabSize);
	ExportUInt(szUpsfLimit,						0L, m_uLimit);
	ExportBool(szUpsfKeepsSelection,	0L, KeepsSelection() );
	
	(GetAttributes())->Export();

	szText = LockTextString(m_oiSelf);
	I_ASSERT_STRING(szText, "TEXT::Export", return A_ERROR);

	AUpsfExportField(szUpsfText,	0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, szText, lstrlen(szText));

	return A_OK;
	}
									 
///////////////////////

long TEXTOBJECT::Import()
	{
	BOOL										bool;
	short										s;
	unsigned long						ulTextLen;
	LPSTR										szText;


	if( ImportBool(szUpsfSelectAll, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfSelectAll);
	else
		SetSelectsAll(bool);

	if(ImportShort(szUpsfJustify, 0L, &s) != AUPSF_NOERROR)
		LogError(szUpsfJustify);
	else
		{
		if (s==0)	SetJustifiesLeft();
		else if (s==1) SetJustifiesCenter();
		else SetJustifiesRight();
		}
		
	if(ImportShort(szUpsfCaseLimit, 0L, &s) != AUPSF_NOERROR)
		LogError(szUpsfCaseLimit);
	else
		{
		if (s==2)	SetLowercase();
		else if (s==1) SetUppercase();
		}
		
	if(ImportBool(szUpsfEditable, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfEditable);
	else
		SetIsEditable(bool);

	if(ImportBool(szUpsfVScroll, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfVScroll);
	else
		SetHasVScroll(bool);

	if(ImportBool(szUpsfHScroll, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfHScroll);
	else
		SetHasHScroll(bool);

	if(ImportBool(szUpsfFrame, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfFrame);
	else
		SetHasFrame(bool);

	//get the control type
	if(ImportBool(szUpsfStatic, 0L, &bool) != AUPSF_NOERROR)
		{//Don't know if it is static
		LogInfo(szUpsfStatic);
		if (!HasFrame() && !IsEditable() && !HasVScroll())
			SetIsStatic();
		}
	else
		{
		if (bool)
			SetIsStatic();
		}

	if(ImportBool(szUpsfSingleline, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfSingleline);
	else
		{
		if (bool  &&  !IsStatic()) //cause singleline could be true from the old way
			SetIsSingleline();			//singleline was all we used to export
		}

	if(ImportBool(szUpsfMultiline, 0L, &bool) != AUPSF_NOERROR)
		LogInfo(szUpsfMultiline);
	else
		{
		if (bool)
			SetIsMultiline();
		}

	if(ImportBool(szUpsfUseEnter, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfUseEnter);
	else
		SetUsesEnter(bool);

	if(ImportBool(szUpsfUseTabs, 0L, &bool) != AUPSF_NOERROR)
		LogError(szUpsfUseTabs);
	else
		SetUsesTab(bool);

	if(ImportInt(szUpsfTabWidth, 0L, &m_iTabSize) != AUPSF_NOERROR)
		LogError(szUpsfTabWidth);

	if(ImportUInt(szUpsfLimit, 0L, &m_uLimit) != AUPSF_NOERROR)
		LogError(szUpsfLimit);

	if(ImportBool(szUpsfKeepsSelection, 0L, &bool) != AUPSF_NOERROR)
		LogInfo(szUpsfKeepsSelection);
	else
		SetKeepsSelection(bool);


	(GetAttributes())->Import();

	if(AUpsfFieldInfo(szUpsfText, 0L, AUPSF_NATV_STRING, NULL, (long *)&ulTextLen) == AUPSF_NOERROR)
		{
		ulTextLen = min(ulTextLen, m_uLimit+1L);
		ResizeTextString(m_oiSelf, ulTextLen);
		szText = LockTextString(m_oiSelf);
		I_ASSERT_STRING(szText, "TEXTOBJECT::Import", return A_ERROR);

		*szText = 0; // initialize to nothing.

		// ignore possible errors on importing the text -- text could be too long
		AUpsfImportField(szUpsfText, 0L, AUPSF_NATV_STRING, szText, ulTextLen);
		szText[ulTextLen-1] = 0;
		UnlockTextString(m_oiSelf);
		}
	else
		LogError(szUpsfText);

	return A_OK;
	}
	
////////////////////////////////////////////////////////
/// Printer Support   //////////////////////////////////
////////////////////////////////////////////////////////
#define I_MAR_LEFT		0.5	//margins in inches
#define I_MAR_TOP			0.5
#define I_MAR_RIGHT		0.5
#define I_MAR_BOTTOM	0.5


static BOOL EjectPage(
	HDC										hdc,
	LPSTR									szError)
	{
	BOOL									rVal;

	switch( Escape(hdc, NEWFRAME, NULL, NULL, NULL) )
		{
		case SP_APPABORT:
			lstrcpy(szError, "Print job was terminated because the application's Abort function returned zero.");
			rVal = FALSE;
			break;

		case SP_OUTOFDISK:
			lstrcpy(szError, "Not enough disk space is currently available for print spooling.");
			rVal = FALSE;
			break;

		case SP_OUTOFMEMORY:
			lstrcpy(szError, "Not enough memory is available for print spooling.");
			rVal = FALSE;
			break;

		case SP_USERABORT:
			lstrcpy(szError, "User terminated the job through Print Manager.");
			rVal = FALSE;
			break;

		case SP_ERROR:
			lstrcpy(szError, "General printing error.");
			rVal = FALSE;
			break;

		default :
			rVal = TRUE;
			break;
		}
	return rVal;
	}

//////

WORD BreakLine(  //Returns horz the extent of the text
	HDC											hdc,
	LPSTR										str,
	int											tabWidth,
	int											width,
	int  *								ptrNumCharsToPrint,
	int  *								ptrNumCharsToIncBy)
	{
	int											maxNumChars, i;
  DWORD										extent;
	BOOL										bFoundBreak;

	bFoundBreak = FALSE;

	for(i=0; !bFoundBreak && i <= lstrlen(str); i++)
		{
		extent = GetTabbedTextExtent(hdc, str, i+1, 1, &tabWidth);
		if (LOWORD(extent) > width)
			break; //we've gone too 

		if (str[i] == CR  &&  str[i+1] == LF)
			{  //hit a <CR><LF> combo
			bFoundBreak = TRUE;
			*ptrNumCharsToPrint = i;
			*ptrNumCharsToIncBy = i+2;
			}
		else if (str[i] == 0)
			{
			bFoundBreak = TRUE;
			*ptrNumCharsToPrint = i;
			*ptrNumCharsToIncBy = i;
			}
		}

	maxNumChars = i-1;

	//If there was no <CR>LF> in time, backup to break the word.
	for( ; !bFoundBreak && i>0; i--)
		{
		if (isspace(str[i]))
			{
			bFoundBreak = TRUE;
			*ptrNumCharsToIncBy = i+1;
			*ptrNumCharsToPrint = i;
			}
		}

	if (!bFoundBreak)
		*ptrNumCharsToPrint = *ptrNumCharsToIncBy = maxNumChars; 

	extent = GetTabbedTextExtent(hdc, str, *ptrNumCharsToPrint, 1, &tabWidth);

	return LOWORD(extent);
	}

////

void TEXTOBJECT::Print(
  LPPIDR_BLOCK						pidr)
	{
	LPSTR										szText;
	ATTRIBUTES							attPrinter(&m_attributes);
  POINT										ptDim, ptOffset;
	RECT										rcTemp;
	int											width, height;
	HFONT										hFontOld;
	COLORREF								fore, oldFore, back, oldBack;
  TEXTMETRIC							tm;
  int											yChar;
  char 										szBuffer[128];
	int											currPage, lineNumber, nLinesPerPage;
  LPSTR										chp;
	BOOL										bIsColor;
	int											iTabWidth;
	int											xPos, yPos;
	int											extent, numCharsToPrint, numCharsToIncBy;

	szText = LockTextString(m_oiSelf);
	I_ASSERT_STRING(szText, "TEXTOBJECT::Print", return);

  // set the margins
  if(Escape(pidr->hDC, GETPHYSPAGESIZE, NULL, NULL, &ptDim) > 0) 
  	{
    if(Escape(pidr->hDC, GETPRINTINGOFFSET, NULL, NULL, &ptOffset)>0) 
    	{
      rcTemp.left	  = pidr->DPI.x * I_MAR_LEFT - ptOffset.x;
      rcTemp.top	  = pidr->DPI.y * I_MAR_TOP  - ptOffset.y;
      rcTemp.right  = ptDim.x - pidr->DPI.x* I_MAR_RIGHT  - ptOffset.x;
      rcTemp.bottom = ptDim.y - pidr->DPI.y* I_MAR_BOTTOM - ptOffset.y;

      // reassign the rectangle as necessary
      if(pidr->rect.left   < rcTemp.left)   pidr->rect.left   = rcTemp.left;
      if(pidr->rect.top    < rcTemp.top)    pidr->rect.top    = rcTemp.top;
      if(pidr->rect.right  > rcTemp.right)  pidr->rect.right  = rcTemp.right;
      if(pidr->rect.bottom > rcTemp.bottom) pidr->rect.bottom = rcTemp.bottom;
      }
    }

  width  = pidr->rect.right  - pidr->rect.left;
  height = pidr->rect.bottom - pidr->rect.top;

	//Setup the dc
	attPrinter.AttrCreateFont(pidr->hDC);
	hFontOld = attPrinter.AttrSelectFont(pidr->hDC);

	bIsColor = GetDeviceCaps(pidr->hDC, NUMCOLORS) > 2;
	fore = bIsColor ? attPrinter.AttrGetForeground() : RGB(0,0,0); //black
	back = bIsColor ? attPrinter.AttrGetBackground() : RGB(255,255,255);//white

	oldFore = SetTextColor(pidr->hDC, fore);
	oldBack = SetBkColor(pidr->hDC, back);

  // Get the char height, number of lines per page, & num chars per line
  GetTextMetrics(pidr->hDC, &tm);

  // Get the char height, number of lines per page, & tab width
  yChar = tm.tmHeight + tm.tmExternalLeading;
  nLinesPerPage = (height) / yChar;
	iTabWidth = m_iTabSize * tm.tmAveCharWidth;

	/////////

	currPage = 0;
	lineNumber = nLinesPerPage + 1;
  chp = szText;

	yPos = pidr->rect.top +tm.tmExternalLeading; 

	while (*chp)
		{
    if(lineNumber > nLinesPerPage) 
    	{
      currPage++;
			yPos = pidr->rect.top +tm.tmExternalLeading; 

      // quit if all the requested pages have been printed
      if(!pidr->numPages && currPage > pidr->toPage)
      	break;

      // eject page and update window if printing
			if(!pidr->numPages &&
				 currPage>pidr->fromPage && currPage<=pidr->toPage)
				{
				if (!EjectPage(pidr->hDC, pidr->error))
					break; //failed

				// re-initialize the font and colors
				attPrinter.AttrSelectFont(pidr->hDC);
				SetTextColor(pidr->hDC, fore);
				SetBkColor(pidr->hDC, back);
				}

			// update window
			if(!pidr->numPages) 
				{
				if(currPage >= pidr->fromPage  &&  currPage <= pidr->toPage)
					wsprintf(szBuffer, "Printing page #%i.", currPage);
				else
					wsprintf(szBuffer, "Preparing Print Job.");
				SendMessage(pidr->hMsgWnd, WM_SETTEXT, 0, (long)szBuffer);
				}

			// reset line number for next Page
      lineNumber=1;
      }

	 extent = BreakLine(pidr->hDC, chp, iTabWidth, width, &numCharsToPrint, &numCharsToIncBy);
	 if(!pidr->numPages)
	 	{
		if (JustifiesLeft())
			xPos = pidr->rect.left;
		else if (JustifiesRight())
			xPos = pidr->rect.left + width - extent;
		else //center
		  xPos = pidr->rect.left + (width - extent)/2;

   TabbedTextOut(pidr->hDC, xPos, yPos, chp, numCharsToPrint, 1, &iTabWidth, xPos);
		}
	 chp += numCharsToIncBy;
   yPos += yChar;
   lineNumber++;
	 }

	//Done
	EjectPage(pidr->hDC, pidr->error);

	// restore font and colors to what they originally were
	SetBkColor(pidr->hDC, oldBack);
	SetTextColor(pidr->hDC, oldFore);
	SelectObject(pidr->hDC, hFontOld);
	attPrinter.AttrDeleteFont();

  if(pidr->numPages) 
  	{
    // return number of possible pages to print in toPage
    pidr->toPage = (short)currPage;
    }

	UnlockTextString(m_oiSelf);
	}


