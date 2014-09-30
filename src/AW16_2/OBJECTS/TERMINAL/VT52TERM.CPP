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
// FILE:"vt52term.cpp"
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
#include "terminal.h"

VT52Term::VT52Term(OBJECTID oiObj): RUNTMNL (oiObj)
{
		nTermState = CHAR_RECEIVE_STATE ;
}
VT52Term::~VT52Term()
{
}
int far VT52Term::RedrawTerminal (void)
{
	nNumChars = RUNTMNL::RedrawTerminal() ;
	if (nNumChars <0) return (FALSE) ;

	ProcessChars (CommGetMem(oiConn), nNumChars) ;
	return (TRUE) ;
}
BOOL VT52Term::ProcessChars (LPSTR szChars, int nNumChars)
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
				ParamReceiveState(*lpCurrChar) ;
				break ;

			default:
				nTermState = CHAR_RECEIVE_STATE ;
        break ;
      }
		}
  return (TRUE) ;
}
void VT52Term::CharReceiveState (char cChar)
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
void VT52Term::EscapeRcvdState (char cChar)
{
	 //
   // process based on the character
   //
  switch( cChar ) {
    case 'H':     //Cursor position
			nCurrRow = 0;
			nCurrCol = 0;
			nTermState = CHAR_RECEIVE_STATE;
			MoveTo (nCurrCol, nCurrRow) ;
      break;

		case 'A':     //cursor up
    case 'I':
			if ( --nCurrRow < 0 )
				nCurrRow = 0;
			nTermState = CHAR_RECEIVE_STATE;
			MoveTo (nCurrCol, nCurrRow) ;
			break;
                    
    case 'B':    //cursor down
		 	if ( ++nCurrRow > ( vMaxRows - 2 ) )
				nCurrRow = vMaxRows - 2;
			nTermState = CHAR_RECEIVE_STATE;
			MoveTo (nCurrCol, nCurrRow) ;
      break;

    case 'C':      //cursor forward
		 	if ( ++nCurrCol >= ( vMaxCols - 1 )  )
				nCurrCol = vMaxCols - 2;
			nTermState = CHAR_RECEIVE_STATE;
			MoveTo (nCurrCol, nCurrRow) ;
      break;
                      
    case 'D':     //cursor backward
		 	if ( --nCurrCol < 0 )
				nCurrCol = 0;
			nTermState = CHAR_RECEIVE_STATE;
			MoveTo (nCurrCol, nCurrRow) ;
			break;

    case 'J':  //erase screen
		  EraseFromCursorToEnd (nCurrCol, nCurrRow) ;
			nTermState = CHAR_RECEIVE_STATE;
			break;

		case 'K':  // erase line
		  ClearToEndOfLine (nCurrCol, nCurrRow) ;
			nTermState = CHAR_RECEIVE_STATE;
			break;

		case 'Y':
		  nTermState = PARAM_RECEIVE_STATE ;
      nCurrParam = 0 ;
			break ;
				   	
		case 'Z':  //Transmit ESC/Z I am a VT52
		  nTermState = CHAR_RECEIVE_STATE;
			break ;

		default:
		  nTermState = CHAR_RECEIVE_STATE ;
      break ;
	 }
}
void VT52Term::ParamReceiveState(char cChar)
{
	if ( isascii( cChar ) )
		{
		if ( isdigit( cChar ) )
			{
   		nParams[ nCurrParam ] *= 10;
			nParams[ nCurrParam ] += (cChar - '0');
			if (nCurrParam++==1)
				{
				nCurrCol = nParams[0] ;
				nCurrRow = nParams[1] ;
				MoveTo (nCurrCol, nCurrRow) ;
        }
      }
		else
    	nTermState = CHAR_RECEIVE_STATE;
    }
	else
  	nTermState = CHAR_RECEIVE_STATE;
}
void VT52Term::KeyDown (int keyCode)
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

		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
    	ArrowKey(keyCode) ;
			break;

		default:
			PutChar ((char)keyCode) ;
    	return ;
		}
}
void VT52Term::ArrowKey(int KeyCode)
{
	PutChar (27) ; //Esc
	switch(KeyCode)
		{
		case VK_UP:
			PutChar (65) ;
      break ;
		case VK_DOWN:
			PutChar (66) ;
      break ;
		case VK_LEFT:
			PutChar (68) ;
      break ;
		case VK_RIGHT:
			PutChar (69) ;
			break ;
		}
}
