////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"ansiterm.cpp"
//
// AUTHOR: Sibai Li
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <ctype.h>
#include <string.h>
#include "terminal.h"

extern long DispAttributes[] ;

AnsiTerm::AnsiTerm(AOBJECTID oiObj): RUNTMNL (oiObj)
{
	nTermState = CHAR_RECEIVE_STATE ;
}
AnsiTerm::~AnsiTerm()
{
}
int  AnsiTerm::RedrawTerminal (void)
{
	//char *szChars ;

	//nNumChars = RUNTMNL::RedrawTerminal() ;
	nNumChars = lstrlen(CommGetMem(oiConn) );
	if (nNumChars <0) return (FALSE) ;

	//ProcessChars (comm->lpCommMem, nNumChars) ;
	ProcessChars (CommGetMem(oiConn), nNumChars) ;

	InvalidateRect (hwnd, NULL, FALSE) ;
	return (TRUE) ;
}
void  AnsiTerm::PutChar (char c)
{
	char strtemp[2] ;

	strtemp[0] = c ;
	strtemp[1] = '\0' ;

	if (CheckPort())
		{
		//AnsiToOem (strtemp, strtemp) ;
		//comm->PutString(strtemp) ;
		CommPutString (oiConn, (BYTE*)strtemp) ;
		}
	if (bEcho)
		{
		ProcessChars (strtemp, 1) ;
		InvalidateRect (hwnd, NULL, FALSE) ;
    UpdateWindow (hwnd) ;
		}
}
BOOL AnsiTerm::ProcessChars (LPSTR szChars, int nNumChars)
{
	int   i ;
  LPSTR lpCurrChar ;

	for(i=0, lpCurrChar=szChars; i<nNumChars; i++,lpCurrChar++)
		{
		switch (nTermState)
			{
			case CHAR_RECEIVE_STATE:
				CharReceiveState (*lpCurrChar) ;
				break ;

			case ESCAPE_RCVD_STATE:
				EscapeRcvdState (*lpCurrChar) ;
				break ;

			case PARAM_RECEIVE_STATE:
      	ParamReceiveState (*lpCurrChar) ;
				break ;

			default:
				nTermState = CHAR_RECEIVE_STATE ;
        break ;
      }
		}
  return (TRUE) ;
}
void AnsiTerm::CharReceiveState (char cChar)
{
	switch (cChar)
		{
		case VK_ESCAPE:
			nTermState = ESCAPE_RCVD_STATE ;
			break ;

		default:
			DisplayChar (cChar) ;
      break ;
    }
}
void AnsiTerm::EscapeRcvdState (char cChar)
{
	switch (cChar)
		{
		case '[':
			nTermState = PARAM_RECEIVE_STATE ;
			nCurrParam = 0 ;
			nParams[nCurrParam] = -1 ;
			break ;

		default:
			nTermState = CHAR_RECEIVE_STATE ;
      break ;
   	}
}
void AnsiTerm::ParamReceiveState (char cChar)
{
	int      i, nTemp;
  DWORD    dwTemp;
	HBRUSH   hBrush;
	//DWORD    fg, bg ;

   //
   // process based on the character
   //
	switch( cChar )
		{
    case 'H':     //Cursor position
      if ( nParams[ 0 ] == -1 )
				nCurrRow = 0;
      else
			 	nCurrRow = nParams[ 0 ];
      if ( nParams[ 1 ] == -1 )
				nCurrCol = 0;
      else
				nCurrCol = nParams[ 1 ]-1;
			nTermState = CHAR_RECEIVE_STATE;
			MoveToEx (nCurrCol, nCurrRow) ;
      break;

    case 'f':      //horizontal/vertical position
      if ( nParams[ 0 ] == -1 )
				nCurrRow = 0;
      else
				nCurrRow = nParams[ 0 ];
      if ( nParams[ 1 ] == -1 )
				nCurrCol = 0;
      else
				nCurrCol = nParams[ 1 ]-1;
			nTermState = CHAR_RECEIVE_STATE;
			MoveToEx (nCurrCol, nCurrRow) ;
      break;

		case 'A':     //cursor up
      if ( nParams[ 0 ] == -1 )
        nTemp = 1;
      else
        nTemp = nParams[ 0 ];
		 	nCurrRow -= nTemp;
			if ( nCurrRow < 0 )
				nCurrRow = 0;
			nTermState = CHAR_RECEIVE_STATE;
			MoveToEx (nCurrCol, nCurrRow) ;
			break;
                    
    case 'B':    //cursor down
      if ( nParams[ 0 ] == -1 )
        nTemp = 1;
      else
        nTemp = nParams[ 0 ];
		  nCurrRow += nTemp;
			if ( nCurrRow > ( vMaxRows - 2 ) )
				nCurrRow = vMaxRows - 2;
			nTermState = CHAR_RECEIVE_STATE;
			MoveToEx (nCurrCol, nCurrRow) ;
      break;
                         
    case 'C':      //cursor forward
      if ( nParams[ 0 ] == -1 )
        nTemp = 1;
      else
        nTemp = nParams[ 0 ];
		  nCurrCol += nTemp;
			if ( nCurrCol >= ( vMaxCols - 1 )  )
				nCurrCol = vMaxCols - 2;
			nTermState = CHAR_RECEIVE_STATE;
			MoveToEx (nCurrCol, nCurrRow) ;
      break;
                      
    case 'D':     //cursor backward
      if ( nParams[ 0 ] == -1 )
        nTemp = 1;
      else
        nTemp = nParams[ 0 ];
			nCurrCol -= nTemp;
			if ( nCurrCol < 0 )
				nCurrCol = 0;
			nTermState = CHAR_RECEIVE_STATE;
			MoveToEx (nCurrCol, nCurrRow) ;
      break;

    case 's':    //save cursor position
		  nTermState = CHAR_RECEIVE_STATE;
			SaveCursorPos (nCurrCol, nCurrRow) ;
      break;

    case 'u':   // restore cursor position
		  nCurrCol = saveCol;
			nCurrRow = saveRow;
			nTermState = CHAR_RECEIVE_STATE;
			RestoreCursorPos() ;
      break;

    case 'J':  //erase screen
		  if ( nParams[0]==-1||nParams[0]==0)
			  {
				EraseFromCursorToEnd (nCurrCol, nCurrRow) ;
				}
			if ( nParams[0]==1 )
			  {
				EraseFromStartToCursor (nCurrCol, nCurrRow) ;
				}
		  if ( nParams[0]==2 )
			  {
				nCurrRow = nCurrCol = 0;
				EraseScreen (nCurrCol, nCurrRow) ;
        }
			nTermState = CHAR_RECEIVE_STATE;
			break;

		case 'K':  // erase line
		  if (nParams[0]==-1||nParams[0]==0)
			  {
				ClearToEndOfLine (nCurrCol, nCurrRow) ;
				}
		  if (nParams[0]==1)
			  {
				ClearToStartOfLine (nCurrCol, nCurrRow) ;
				}
			if (nParams[0]==2)
			  {
				ClearLine (nCurrCol, nCurrRow) ;
        }
      nTermState = CHAR_RECEIVE_STATE;
			break;

		case 'U':
			nCurrRow = nCurrCol = 0 ;
			EraseScreen (nCurrCol, nCurrRow) ;
			nTermState = CHAR_RECEIVE_STATE ;
			break ;

		case 'L':
		  InsertLine (nCurrCol, nCurrRow) ;
			nTermState = CHAR_RECEIVE_STATE ;	
			break ;

		case 'M':
		  DeleteLine (nCurrCol, nCurrRow) ;
			nTermState = CHAR_RECEIVE_STATE ;
			break ;

		case 'P':
		  DeleteChar (nCurrCol, nCurrRow) ;
			nTermState = CHAR_RECEIVE_STATE ;
			break;

		case 'S':
		  ScrollUp () ;
			nTermState = CHAR_RECEIVE_STATE ;
			break ;

		case 'T':
			ScrollDown () ;
			nTermState = CHAR_RECEIVE_STATE ;
			break ;
				 	 	 	 	 	 	 	
		case 'm':
			{		//Set graphics rendition
			for ( i = 0; i <= nCurrParam; i++ )
			  {
        if ( nParams[ i ] == -1 )
          break;
				switch( nParams[i] )
					{
          case 0: // all attributes off
            dwTextColor = 7;
            dwBkGndColor = 0 ;
            break;

          case 1: // bold on
            dwTextColor = 7;
            dwBkGndColor = 0;
						break;

          case 4: // underscore on
            dwTextColor = 7 ;
            dwBkGndColor = 0 ;
						break;

          case 5: // blink on
            dwTextColor = 7 ;
            dwBkGndColor = 0 ;
            break;

          case 7: // reverse on
						dwTemp = dwTextColor ;
						dwTextColor = dwBkGndColor;
            dwBkGndColor = dwTemp;
            break;

          case 8: // concealed on
						dwTextColor = dwBkGndColor;
            break;

          case 30: //black foreground
						dwTextColor = 0;
            break;

          case 31: // red foreground
						dwTextColor = 4;
            break;

          case 32: // green foreground
						dwTextColor = 2;
            break;

          case 33:// yellow foreground
						dwTextColor = 6;
            break;

          case 34:// blue foreground
						dwTextColor = 1;
            break;

          case 35:// magenta foreground
						dwTextColor = 5;
            break;

          case 36:// cyan foreground
						dwTextColor = 3;
						break;

          case 37:// white foreground
						dwTextColor = 7;
            break;

          case 40:// black background
						dwBkGndColor = 0;
            break;

          case 41: // red background
						dwBkGndColor = 4;
            break;

          case 42: // green background
						dwBkGndColor = 2;
            break;

          case 43: // yellow background
						dwBkGndColor = 6;
            break;

          case 44: //  Blue Background
						dwBkGndColor = 1;
            break;

          case 45:// magenta background
						dwBkGndColor = 5 ;
            break;

          case 46:// cyan background
						dwBkGndColor = 3;
            break;

          case 47: // white background
						dwBkGndColor = 7;
            break;

          default:
            break;
					}
        }

         //
         // reset the current state
				 //
			dwTextColor &=0x0007 ;
			dwBkGndColor &=0x0007 ;																
			SetCurrAttr (dwTextColor, dwBkGndColor) ;
			hBrush = CreateSolidBrush((COLORREF) DispAttributes[dwBkGndColor] );
			if ( hBrush )
				{
				SetClassLong( hwnd, GCL_HBRBACKGROUND, (LONG)hBrush );
        if ( hBkBrush )
          DeleteObject( hBkBrush );
        hBkBrush = hBrush;
				}
			nTermState = CHAR_RECEIVE_STATE;
      }
      break;

		case ';': // go on to the next parameter
    	nCurrParam++;
      nParams[ nCurrParam ] = -1;
			break;

    default: // make sure the character is a digit
			if (isascii(cChar))
				{
				if (isdigit(cChar))
					{
          if ( nParams[ nCurrParam ] == -1 )
            nParams[ nCurrParam ] = 0;
          else
            nParams[ nCurrParam ] *= 10;
          nParams[ nCurrParam ] += (cChar - '0');
					}
				else
					{
          nTermState = CHAR_RECEIVE_STATE;
          }
				 }
		 else
		 	{
      nTermState = CHAR_RECEIVE_STATE;
		 	}
      break;
	 }
}

void AnsiTerm::KeyDown (int keyCode)
{
	switch(keyCode)
		{
		case VK_RETURN:
    	ReturnKey();
			return;

		case VK_TAB:
			TabKey();
			return;

		case VK_BACK:
		case VK_DELETE:
	    PutChar (keyCode) ;
			return ;
		case VK_LEFT:	 PutString ("\33[D") ;	break;
		case VK_RIGHT: PutString ("\33[C") ;	break;
		case VK_UP:		 PutString ("\33[A") ;	break;
		case VK_DOWN:	 PutString ("\33[B") ; 	break;
		case VK_HOME:	 PutString ("\33[H") ; 	break;
		case VK_END:	 PutString ("\33[F") ;  break;
		case VK_PRIOR: PutString ("\33[I") ;	break;
		case VK_NEXT:  PutString ("\33[G") ;	break;
		case VK_INSERT:PutString ("\33[L") ;	break;
		//case VK_F1:  	 PutString ("\33[D") ;	break;
		//case VK_F2:  	 PutString ("\33[D") ;
		//	break;
		//case VK_F3:
		//	PutString ("\33[D") ;
		//	break;
		//case VK_F4:
		//	PutString ("\33[D") ;
		//	break;
		//case VK_F5:
		//	PutString ("\33[D") ;
		//	break;
		//case VK_F6:
		//	PutString ("\33[D") ;
		//	break;
		//case VK_F7:
		//	PutString ("\33[D") ;
		//	break;
		//case VK_F8:
		//	PutString ("\33[D") ;
		//	break;
		//case VK_F9:
		//	PutString ("\33[D") ;
		//	break;
		//case VK_F10:
		//	PutString ("\33[D") ;
		//	break;	
		
		default:
			PutChar ((char)keyCode) ;
    	break; 
		}
	
}
