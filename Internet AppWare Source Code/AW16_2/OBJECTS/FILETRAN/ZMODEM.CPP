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
// FILE:"zmodem.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT																					  

#include <string.h>
#include <mem.h>
#include "filetrns.h"
#include "zmmacro.h"
#include "crc.h"
#include "zmodem.h"

extern void ActiveIdleEvent (OBJECTID oiFT) ;
//* Contains:
//*      public:
//*         ZMODEMCLASS::ZMODEMCLASS()
//*         ZMODEMCLASS::~ZMODEMCLASS()
//*         ZMODEMCLASS::receivefile()
//*         ZMODEMCLASS::sendfile()
//*         ZMODEMCLASS::sendfilelist()
//*         ZMODEMCLASS::SetRecurseFlag()
//*         ZMODEMCLASS::SetStripPathOption()
//*         ZMODEMCLASS::SetCrashRecovery()
//*
//*
///////////////////////////////////////////////////////////////////////////
//Filename: ZMODEMCLASS::ZMODEMCLASS
//describtion: Construction
///////////////////////////////////////////////////////////////////////////
ZMODEMCLASS::ZMODEMCLASS (
	OBJECTID oiFlTr, 
	char * fileName, 
	BOOL sORr
	):FTCLASS (oiFlTr, fileName, sORr)
{
	FILETRANSFER fltr(oiFT) ;
	WORD  namesize = 0;		
	char* p=fileName;

	crc_mode = 1;  //Get CRC check.
	Crc16 = new ChkCrc16() ;
	Crc32 = new ChkCrc32() ;
  
	lErrorCount  = 0 ;
	block_number = 0 ;
	block_size = 0 ;
	nbytes = 0 ;
	filecount = 0 ;

	filename = NULL ;
	if (sORr)
	  {
		while(strlen(p))
			{
			namesize +=strlen(p)+1 ;
			p +=strlen(p)+1 ; 
			filecount++ ;
			}
		filename = new char[namesize+1] ;
		memcpy (filename, fileName, namesize) ;
		}
	else
	  {
		filename = new char[strlen(fileName+1)] ;
		strcpy (filename, fileName) ;
		}
 	AObjSetRuntimeInfo (oiFT, (long)this) ;
}
//*
//* ZMODEMCLASS::~ZMODEMCLASS( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This destructor will delete allocated parts of the Zmodem class.
//*
ZMODEMCLASS::~ZMODEMCLASS(void)
{
	if (Crc16) delete Crc16 ;
	if (Crc32) delete Crc32 ;
	if (filename) delete[] filename ;
	delete this ;
}

BOOL ZMODEMCLASS::SystemIdle () 
{
	MSG  msg ;
	HWND hwnd = NULL ;
	BOOL bVal = TRUE ; //SUCCESSFUL

	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	  {
		if (msg.message==WM_QUIT||msg.message==WM_CLOSE)
			bVal = FALSE ; //STOPIDLE
		if (msg.message==WM_SYSCOMMAND&&msg.wParam==SC_CLOSE)
			bVal = FALSE ;
			
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	  }
	return(bVal);
}
int ZMODEMCLASS::ReadChar (DWORD timeout)
{
	int rChar ;
	
	DWORD endtime = GetTickCount() + timeout ;
	for( ; GetTickCount() < endtime ; )
	  {
		if (CommRxCount (oiConn)>0)     
			{
			rChar = CommGetChar (oiConn) ; 
			if (rChar>-1)															 
	   		break;
			}
		if (GetTickCount()> endtime)
			{
			rChar = FT_TIMEOUT ;
			break;
			}
		if (!SystemIdle())
			{
			rChar = FT_ERROR ;
			break;
			}
		if (CheckAbort())
			{
			rChar = FT_ERROR ;
			break;
			}
	  }

	return(rChar);
}
int ZMODEMCLASS::WritePort (char c)
{
	int rVal = FT_ERROR ;
	DWORD endtime = GetTickCount()+ 30000 ;

	for (;;)
	  {
		if (CommPutChar (oiConn, c)==1)
			{
			rVal = FT_SUCCESS ; 
			break;
			}
		if (!SystemIdle())	
			break;
		if (CheckAbort())
			break;
			
		if (GetTickCount()>endtime)
			break;
		}  
	return(rVal);
}
int ZMODEMCLASS::WriteToBuffer(const char *buffer, int length )
{
	BYTE *p = (BYTE*)buffer;
  int total_written =0;  
  int count_written =0;
  long endtime = GetTickCount() + 60000L ;
   
  if(CheckAbort())
    return(FT_ERROR);

 	for( ; ; ) 
 		{
    //*
    //* Try to write the buffer then update counters an pointers.
    //*
    count_written = 0;
		count_written=CommPutBuffer(oiConn, p, length);
		total_written += count_written;
    length -= count_written;

		if (length==0)
			return(FT_SUCCESS);
			
		if (GetTickCount()>endtime)
			return (FT_ERROR) ;

		if (!SystemIdle())
			return(FT_ERROR);
	 }		
}
int ZMODEMCLASS::UpdateStateWindow (int error, int blockno, long blocksize, char *name)
{
	BOOL rVal = FT_SUCCESS ;

	if (Status)
  	{
		if(blockno) lpStatus->blockno = blockno ;
		lpStatus->blocksize = blocksize ;
		lpStatus->error = error ;
		if (name)
			{
			lpStatus->writeName = TRUE ;
			strcpy (lpStatus->filename, name) ;
			}  
		lpStatus->UpdateStatusWindow () ;
		lpStatus->writeName = FALSE ;
  	}
	return(rVal);

}
void ZMODEMCLASS::CloseStateWindow(void)
{
	if (Status&&lpStatus)
		DestroyWindow (lpStatus->hStatus) ;
}
BOOL ZMODEMCLASS::CheckAbort(void)
{
	BOOL rVal = FALSE ;
	if (Status&&lpStatus)
		if (lpStatus->transferStop) rVal = TRUE ;
	else
		if (transferCancelled) rVal = TRUE ;	//Check Abort when there is no the satus bar
	return(rVal);
}
void ZMODEMCLASS::StartProtocol (OBJECTID oiFT, int sendOrReceive)
{
	int rVal ;
	if (sendOrReceive==SENDFILE)
    {
		rVal = Initiation (TRUE) ;
		if (rVal==FT_SUCCESS)
		  _SendFile() ;
		}
	else
	  {
	  rVal = Initiation (FALSE) ;
		_ReceiveFile () ;
		}

	ActiveIdleEvent (oiFT) ;
	if (rVal!=FT_SUCCESS)
		AEvtPostSignalAtMark(oiFT, 6) ;
}
int ZMODEMCLASS::Initiation (BOOL bBar)
{
	int rVal = FT_SUCCESS ;
  
	if (Status)
		{
		if (!lpStatus->InitStatusWindow (2, bBar, "ZMODEM", filename, filesize)) 
			rVal = FT_LOCALCANCEL ;
		}
	if (lpBuffer==NULL)
		rVal = FT_ERROR ;

  CommSetXONXOFF (oiConn, 0) ;
	return rVal ;
}

