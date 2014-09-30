//Utility routines which should have no need of any class information or
//information other than what is passed in.

#include "almsys.h"
#include <windowsx.h>
#include "apbutil.h"
#include "intdebug.h"
#include "errors.h"
#include "mmsystem.h"


#define APBUTIL_TIMERRESOLUTION 	1

#define APBUTIL_TOTALFLASHTIME 		200
#define APBUTIL_FLASHINTERVAL			50


HCURSOR oldZZZCursor = LoadCursor(NULL, IDC_ARROW);

//Ides is to transfrom the top-left and bottom-right
void AnimateRectToRect(
	HDC hdc,
	LPRECT src,
	LPRECT dst,
	HPEN pen,
	double steps,
	DWORD delay)
	{
	double lxChange;
	double lyChange;
	double rxChange;
	double ryChange;
	int stop;
//	DWORD time, oldTime;

	double left,right,top,bottom;

	int  oldDC = SaveDC(hdc);

	lxChange = ((double)(dst->left - src->left))/steps;
	lyChange = ((double)(dst->top - src->top))/steps;
	rxChange = ((double)(dst->right - src->right))/steps;
	ryChange = ((double)(dst->bottom - src->bottom))/steps;

	left = src->left;
	right = src->right;
	top = src->top;
	bottom = src->bottom;

	stop = (int)steps;

	SelectObject(hdc, pen);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	SetROP2(hdc, R2_NOTXORPEN);
  
  GdiSetBatchLimit(1); //disable

	while(stop >= 0)
		{

		//calculate if it's not the first pass
		if(stop != (int)steps)
			{
			//calculate the new rectangle size
			left+=lxChange;
			top+=lyChange;
			right+=rxChange;
			bottom+=ryChange;
			}

		//draw the new
		Rectangle(hdc, (int)left, (int)top, (int)right, (int)bottom);

		//delay before erasing

		SleepEx(delay, FALSE);
    
		//erase the old
		Rectangle(hdc, (int)left, (int)top, (int)right, (int)bottom);
		stop--;
		}

	GdiSetBatchLimit(0); //set default
  
	RestoreDC(hdc, oldDC);
	}

void apb_ShowWaitCursor()
	{
	oldZZZCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
  ShowCursor(TRUE);
	}

void apb_RestoreOldCursor()
	{
	ShowCursor(FALSE);
  SetCursor(oldZZZCursor);
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			apb_FlashRectangle
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void apb_FlashRectangle(
	HDC hdc,
	LPRECT rc)
	{
	DWORD time, startTime;
	WORD parity = 0;

  timeBeginPeriod(APBUTIL_TIMERRESOLUTION);
	startTime = timeGetTime();
	
	GdiSetBatchLimit(1); //disable

	do
		{
		//invert the rect
		BitBlt(hdc,rc->left,
			rc->top,
			rc->right-rc->left,
			rc->bottom - rc->top,
			NULL,
			0,0,
			DSTINVERT);
		parity^=0x0001; //change the parity from even to odd or vice-versa
		//waste time in while loop
		
		SleepEx(APBUTIL_FLASHINTERVAL, FALSE);
    time = timeGetTime();
		}
	//only exit when times up and the parity is even to make sure we're in the
	//same state as when we entered.
	while (!(time - startTime > APBUTIL_TOTALFLASHTIME && !(parity%2)));
  
 	GdiSetBatchLimit(0); //set default
 
  timeEndPeriod(APBUTIL_TIMERRESOLUTION);	
	}


//info defaults to APB_NOINFO
long apb_LogUpsfError(
	long upsfError,
	LPSTR comment,
	WORD info)
	{
	
	struct MSGMEMBER
		{
		long    code;
		LPSTR  message;
		};

	static LPSTR  defaultMsg = "(unknown error code).";
	static MSGMEMBER msg[] =
		{
			{	0L,		"No"	},
			{ -1L,	"File"},
			{ -2L,	"Memory"},
			{	-3L,	"File Closed"},
			{	-4L,	"Can't Init Export"},
			{	-5L,	"Export"},
			{	-6L,	"NoT Exporting"},
			{	-7L,	"Can't Convert"},
			{	-8L,	"Size"},
			{	-9L,	"Invalid Object"},
			{	-10L,	"Can't Init Export"},
			{	-11L,	"Import"},
			{	-12L,	"Not Importing"},
			{	-13L,	"Scope"},
			{	-14L, "Scope Doesn't Exist"},
			{	-15L,	"Field Doesn't Exist"},
			{	-15L, "Log"},
			{	-16L, "Out Of Disk Space"},
		};

	const int numMsgs = sizeof(msg) / sizeof(msg[0]);

	char buffer[256];
	char infoMsg[16];
	LPSTR errorMsg;
	char prefixMsg[16];
	int i;

	if(info & APB_UPSFOPEN)
		lstrcpy(infoMsg, "Opening");
	else 
		lstrcpy(infoMsg, "Importing");

	if(info & APB_UPSFFATALERROR)
		lstrcpy(prefixMsg, "Fatal: ");
	else
		prefixMsg[0] = '\0';

	errorMsg = defaultMsg;
	for (i = 0 ; i < numMsgs ; i++)
		{
		if (upsfError == msg[i].code)
			{
			errorMsg = msg[i].message;
			break;
			}
		}

	if((lstrlen(errorMsg) + lstrlen(comment))	< (sizeof(buffer) - 32))
		wsprintf(buffer, "[ %s ] %sError %s %s.", 
			errorMsg, prefixMsg, infoMsg, comment);
	else
		wsprintf(buffer, "[ %s ] %sError -- <log error as well - comment "
			"too long to log>." ,errorMsg, prefixMsg);

	return AUpsfLogComment(buffer);
	}


BOOL apb_BuildCheckErrorMessage(
	short code,
	LPSTR buffer,
	int bufLength)
	{
	BOOL rVal = FALSE;
	BOOL built = FALSE;
	static char joinText[] = {" and "};
	static char joinSize = sizeof(joinText);

	if(!(code & CHECK_CANCEL))
		{
		rVal = TRUE;

		if(code == CHECK_NOERRORS)
			lstrcpyn(buffer, "No Errors", bufLength);
		else 
			{
			if(code & CHECK_ERROR)
				{
				lstrcpyn(buffer, "Errors", bufLength);
				buffer[bufLength-1]= '\0';
				built = TRUE;
				}
			if(code & CHECK_WARNING)
				{
				if(built)
					{
					//8 is the size of "Warnings"
					if(lstrlen(buffer) + joinSize + 8 <= bufLength)
						{
						lstrcat(buffer,	joinText);
						lstrcat(buffer, "Warnings");
						}
					}
				else
					{
					built = TRUE;
					lstrcpyn(buffer, "Warnings", bufLength);
					buffer[bufLength-1] = '\0';
					}
				}
			if(code & CHECK_DUMMYALIASWARNING)
				{
				if(built)
					{
					//13 is the size of "Dummy Aliases"
					if(lstrlen(buffer) + joinSize + 13 <= bufLength)
						{
						lstrcat(buffer,	joinText);
						lstrcat(buffer, "Dummy Aliases");
						}
					}
				else
					{
					built = TRUE;
					lstrcpyn(buffer, "Dummy Aliases", bufLength);
					buffer[bufLength-1] = '\0';
					}
				}
			if(!built)
				{
				rVal = FALSE;
				buffer[0] = '\0';
				I_ASSERT(FALSE);
				}
			}
							
		}

	return rVal;
	}

////////////////////////////////////////////////////////////////
//Remember, LineTo draws a line up to but not including the end point. 
///////////////////////////////////////////////////////////
void apb_DoFrameDraw(
	HDC hdc,
	LPRECT bounds,
	HPEN frame,
	HPEN highlight,
	HPEN shadow)
	{
	HPEN oldPen;
	
	//first draw black border but don't draw the very corner pixels.
	oldPen = SelectPen(hdc, frame);
  
  I_VERIFY(MoveToEx(hdc, bounds->left+1, bounds->top, NULL));
  LineTo(hdc, bounds->right, bounds->top);
  
  I_VERIFY(MoveToEx(hdc, bounds->right, bounds->top+1, NULL));
  LineTo(hdc, bounds->right, bounds->bottom);
  
  I_VERIFY(MoveToEx(hdc, bounds->right-1, bounds->bottom, NULL));
  LineTo(hdc, bounds->left, bounds->bottom);
  
 	I_VERIFY(MoveToEx(hdc, bounds->left, bounds->bottom-1, NULL));
  LineTo(hdc, bounds->left, bounds->top);
  
  //draw highlight
  SelectObject(hdc, highlight);
  I_VERIFY(MoveToEx(hdc, bounds->left+1, bounds->bottom-2, NULL));
  LineTo(hdc, bounds->left+1, bounds->top+1);
  LineTo(hdc, bounds->right-1, bounds->top+1);
  
  I_VERIFY(MoveToEx(hdc, bounds->left+2, bounds->bottom-3, NULL));
  LineTo(hdc, bounds->left+2, bounds->top+2);
  LineTo(hdc, bounds->right-2, bounds->top+2); 
  
  //draw shadow
  SelectObject(hdc, shadow);
  I_VERIFY(MoveToEx(hdc, bounds->right-1, bounds->top+1,NULL));
  LineTo(hdc, bounds->right-1, bounds->bottom-1);
  LineTo(hdc, bounds->left, bounds->bottom-1);
  
  I_VERIFY(MoveToEx(hdc, bounds->right-2, bounds->top+2, NULL));
  LineTo(hdc, bounds->right-2, bounds->bottom-2);
  LineTo(hdc, bounds->left+1, bounds->bottom-2);
	                                                
	
	SelectPen(hdc, oldPen);	
	}
////////////////////////////////////////////////////////////
void apb_DoThinFrameDraw(
	HDC hdc,
	LPRECT bounds,
	HPEN highlight,
	HPEN shadow)
	{
	HPEN oldPen;
	
	//DrawShadow
	oldPen = SelectPen(hdc, shadow);
  
  I_VERIFY(MoveToEx(hdc, bounds->left, bounds->top, NULL));
  LineTo(hdc, bounds->right, bounds->top);
  LineTo(hdc, bounds->right, bounds->bottom);
  
  //draw highlight
  SelectObject(hdc, highlight);
  I_VERIFY(MoveToEx(hdc, bounds->right-1, bounds->bottom, NULL));
  LineTo(hdc, bounds->left, bounds->bottom);
  LineTo(hdc, bounds->left, bounds->top);
  
	SelectPen(hdc, oldPen);	
	
	}




//Stuff for extracting .LIBs used from a .DLL
#define	IDERR_SUCCESS						0
#define	IDERR_OPENFAIL					1
#define	IDERR_READFAIL					2
#define	IDERR_FILETYPEBAD				3
#define	IDERR_EXETYPEBAD				4
#define	IDERR_WINVERSIONBAD			5
#define	IDERR_RESNAMETBLBAD			6
#define	IDERR_NONRESNAMETBLBAD	7

#define	WINDOWSEXE							2

typedef DWORD	NT_SIGNATURE_STRUCT;

typedef struct {
	char	nameLen;								// length of the dll name.
	char	funcName[MAX_PATH];		// dll name.		//!! change to variable length
} DLLNAME;

//this list will probably grow
static char	*dllsOmmitted[] = {
																{ "USER32" }, 
																{ "GDI32" },
																{ "NETAPI32" },
																{ "KERNEL32" },
																{ "COMDLG32" } };

static const int 	numOfOmmittedDlls = sizeof(dllsOmmitted) / sizeof(dllsOmmitted[0]);


////////////////////////////////////////////////////////
// readDLLOldHeader: This function reads the old header
//		from a DLL (.EXE) file, checks to make sure that
//		it is a valid header, and saves the position of
//		the files new header.
//
//				hFile					dll file handle.
//				lFileLen			length of file.
//				pPosNewHdr		pointer to new header.
//				
////////////////////////////////////////////////////////

static WORD ReadDLLOldHeader(
	HANDLE	hFile,
	DWORD	lFileLen,
	DWORD*	plPosNewHdr)
{
	DWORD		lPos;
	DWORD 	cb;
	IMAGE_DOS_HEADER	ehOldHeader;
	WORD		wResult;

	wResult = IDERR_SUCCESS;
  lPos = SetFilePointer(hFile, 0L, NULL, FILE_BEGIN);
	if (lPos == (DWORD)-1 || lPos != 0)
		wResult = IDERR_READFAIL;
    
	if (wResult == IDERR_SUCCESS)
	{
  if(ReadFile(hFile, (LPVOID)&ehOldHeader,	(DWORD)sizeof(ehOldHeader),
    	&cb, NULL))
    {
		if (cb != (DWORD)sizeof(ehOldHeader))
			wResult = IDERR_READFAIL;
		else if (ehOldHeader.e_magic != IMAGE_DOS_SIGNATURE)
			wResult = IDERR_FILETYPEBAD;
		else if (ehOldHeader.e_lfanew < sizeof(ehOldHeader))
			wResult = IDERR_EXETYPEBAD;
		else if (ehOldHeader.e_lfanew > lFileLen)
			wResult = IDERR_EXETYPEBAD;
		else
			*plPosNewHdr = ehOldHeader.e_lfanew;    
    }
  else
  	{
  	I_ASSERT(FALSE);
  	wResult = IDERR_READFAIL;
  	}    		
  	
	}
	return wResult;
}

///////////////////////////////////////////////////////////////
static DWORD GetImageDirectoryOffset(HANDLE hFile, 
	DWORD imageDir,
  PIMAGE_FILE_HEADER phf,
	PIMAGE_OPTIONAL_HEADER poh,
	DWORD secOffset)
	{
  DWORD offset = (DWORD)-1;
  IMAGE_SECTION_HEADER ish;
  DWORD vaImageDir;
  DWORD cb;
  DWORD lPos;
  IMAGE_DATA_DIRECTORY idd;
  int i;
  
  if(imageDir < poh->NumberOfRvaAndSizes)
  	{
    //seek to the location of the imageDirectory
    //the section offset points to just after all the image directories.
    lPos = SetFilePointer(hFile, 
    	secOffset - (sizeof(IMAGE_DATA_DIRECTORY) *
    	(IMAGE_NUMBEROF_DIRECTORY_ENTRIES - imageDir)),
      NULL, FILE_BEGIN);
      
    if(lPos == (DWORD)-1)
    	{
      I_ASSERT(FALSE);
      }
    //load in the data directory
    else if(!ReadFile(hFile, (LPVOID)&idd, (DWORD)sizeof(idd),
    	&cb, NULL) || (cb != (DWORD)sizeof(idd)))
      {
      I_ASSERT(FALSE);
      }
    else
      {
      vaImageDir = idd.VirtualAddress;
      
      //locate the section containing the image directory.
      i=0;
      while(i++	< phf->NumberOfSections)
      	{
        //seek to the section header offset
        lPos = SetFilePointer(hFile, secOffset, NULL, FILE_BEGIN);
        if(lPos == (DWORD)-1 || lPos != secOffset)
        	{
          I_ASSERT(FALSE);
          break;
          }
        //read in the section header
        else if(!ReadFile(hFile, (LPVOID)&ish, (DWORD)sizeof(ish),
        	&cb, NULL) || (cb != (DWORD)sizeof(ish)))
          {
          I_ASSERT(FALSE);
          break;
          }
        else
        	{
					if(ish.VirtualAddress <= (DWORD)vaImageDir &&
	    			ish.VirtualAddress + ish.SizeOfRawData > (DWORD)vaImageDir)
	    			{
	    			break;
            }
           
          secOffset+= (DWORD)sizeof(ish);                    
          }                  
        } //end while loop
        
	   	if (i <= phf->NumberOfSections)
	   		{
	    	/* return image import directory offset */
	    	offset =  vaImageDir - ish.VirtualAddress + ish.PointerToRawData;       
	      }
	    }    
    }
    
  return offset;
  }
  
/* function gets the function header for a section identified by name */
static BOOL GetSectionHdrByName (
  HANDLE		     hFile,
  PIMAGE_SECTION_HEADER psh,
  char* sectionName,
  PIMAGE_FILE_HEADER pfh,
  DWORD secOffset)
	{
  IMAGE_SECTION_HEADER	ish;
  int	i;
	DWORD lPos;
  DWORD cb;
  BOOL rVal = FALSE;

	/* find the section by name */
	for (i=0; i<pfh->NumberOfSections; i++)
	  {
    //seek to the section header offset
    lPos = SetFilePointer(hFile, secOffset, NULL, FILE_BEGIN);
    if(lPos == (DWORD)-1 || lPos != secOffset)
    	{
      I_ASSERT(FALSE);
      }
    else if(!ReadFile(hFile, &ish, (DWORD)sizeof(ish), &cb, NULL) ||
    	(cb != (DWORD)sizeof(ish)))
    	{
      I_ASSERT(FALSE);
      }
    else
    	{
      //we read in the section header
  	  if (!strcmp ((char*)&ish.Name, sectionName))
				{
				/* copy data to header */
				*psh = ish;
				rVal = TRUE;
				break;
				}
	    }
		
		secOffset+=(DWORD)sizeof(ish);
		}

	return rVal;
	}


////////////////////////////////////////////////////////////////
static WORD ReadPortableHeader(
	HANDLE	hFile,
	DWORD	fileLen,
	DWORD	posNewHdr,
	DWORD* pImageDirOffset,
	PIMAGE_SECTION_HEADER psh)

	{
	DWORD		lPos;
	DWORD 	cb;
	WORD		wResult;
  NT_SIGNATURE_STRUCT sig;
  IMAGE_OPTIONAL_HEADER  optFileHeader;
  IMAGE_FILE_HEADER fileHeader;
  DWORD secOffset;

	wResult = IDERR_SUCCESS;
	//verify that we're an NT file
	lPos = SetFilePointer(hFile, posNewHdr, NULL, FILE_BEGIN);
	if (lPos == (DWORD)-1 || lPos != posNewHdr || lPos > fileLen)
		wResult = IDERR_READFAIL;
  //read in the signature
	else if(ReadFile(hFile, (LPVOID)&sig,	(DWORD)sizeof(sig),
    &cb, NULL))
		{
		if (cb != sizeof(sig))
			wResult = IDERR_READFAIL;
		else if (sig != IMAGE_NT_SIGNATURE)
			wResult = IDERR_FILETYPEBAD;
		//read in the file header
		else if(ReadFile(hFile, (LPVOID)&fileHeader,	(DWORD)sizeof(fileHeader),
  		&cb, NULL))
			{
			if (cb != sizeof(fileHeader))
				{
				wResult = IDERR_READFAIL;
        I_ASSERT(FALSE);
        }
      //read in the optional header
      else if(ReadFile(hFile, (LPVOID)&optFileHeader,	(DWORD)sizeof(optFileHeader),
  			&cb, NULL))
				{
				if (cb != sizeof(optFileHeader))
					wResult = IDERR_READFAIL;
      	else
    			{
					secOffset = posNewHdr + sizeof(IMAGE_FILE_HEADER) +
						sizeof(IMAGE_NT_SIGNATURE) + sizeof(IMAGE_OPTIONAL_HEADER);
          //get the directory offset for the import data
      		*pImageDirOffset = GetImageDirectoryOffset(hFile, 
      			IMAGE_DIRECTORY_ENTRY_IMPORT, &fileHeader, &optFileHeader,
            secOffset);
            
          if(*pImageDirOffset == (DWORD)-1)
          	{
            I_ASSERT(FALSE);
            wResult = IDERR_READFAIL;
            }
          else
          	{
          	//get the image section header
            if(!GetSectionHdrByName(hFile, psh, ".idata",
            	&fileHeader, secOffset))
              {
              I_ASSERT(FALSE);
              wResult = IDERR_READFAIL;
              }
          	}
      		}
        }
      else
      	{
        I_ASSERT(FALSE);
        wResult = IDERR_READFAIL;
        }
     	}
  	else
  		{
    	I_ASSERT(FALSE);
    	wResult = IDERR_READFAIL;
    	}
    }
  else
  	{
  	I_ASSERT(FALSE);
  	wResult = IDERR_READFAIL;
  	}
  	  
	return wResult;
	}


////////////////////////////////////////////////////////
// openDLLFile: open the dll and extract the imported
//		functions from the imported name table.
// input:
//		bufferID			memBlockId for dll name buffer.
//		DLLNamePtr		ptr to dll name and path.
////////////////////////////////////////////////////////

int	apb_ExtractDllLibs(
	char* DLLNamePtr,
	AMEMBLOCKID	bufferID)
	{
	HANDLE		hFile;
	WORD			wResult;
	DWORD			lPos;
	DWORD			lPosNewHdr;
	DWORD			lFileLen;
	DWORD			imageDirOffset;
  DWORD			cb;
  IMAGE_SECTION_HEADER ish;
	DLLNAME		dllNameRec;							// name of the function record.
	BOOL			readNames, dontAddIt;
	int				i;
	char*			memBlkPtr;
	int				namesFound = 0;
  IMAGE_IMPORT_DESCRIPTOR iid;
  DWORD nameOffset;
  DWORD nameAmount;
  DWORD importDirOffset;

	wResult = IDERR_SUCCESS;
	if ((hFile = CreateFile(DLLNamePtr,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
    == INVALID_HANDLE_VALUE)
		{	// open file.
		wResult = IDERR_OPENFAIL;
		}
	if (wResult == IDERR_SUCCESS)
		if ((lFileLen = SetFilePointer(hFile, 0L, NULL,SEEK_END)) == (DWORD)-1)	// get file length.
			wResult = IDERR_READFAIL;
	
	#ifdef ADBG_DEBUG
//  DWORD lastError;

  if(wResult != IDERR_SUCCESS)
  	{
  		GetLastError();
    }
  #endif

	if (wResult == IDERR_SUCCESS)	// read old header and get pos of new header.
		wResult = ReadDLLOldHeader(hFile, lFileLen, &lPosNewHdr);

	if (wResult == IDERR_SUCCESS)		// get pos of imported table from new header.
		wResult = ReadPortableHeader(hFile, lFileLen, lPosNewHdr, &imageDirOffset, &ish);

	if (wResult == IDERR_SUCCESS)
		{
		readNames = TRUE;
		
		importDirOffset = imageDirOffset;
				
		// read in the imported names.
		while (readNames == TRUE) 
			{
      //seek to the import descriptor
      lPos = SetFilePointer(hFile, imageDirOffset, NULL, FILE_BEGIN);
      if(lPos == (DWORD)-1 || lPos > lFileLen)
      	{
        I_ASSERT(FALSE);
       	readNames = FALSE;
        }
			//read in the image import descriptor
      else if(!ReadFile(hFile, &iid, (DWORD)sizeof(iid), &cb, NULL) ||
      	(cb != (DWORD)sizeof(iid)))
        {
        I_ASSERT(FALSE);
        readNames = FALSE;
        }
      else
      	{
      	if(!iid.Name)
        	{
          readNames = FALSE;
          }
        else
        	{
					nameOffset = importDirOffset + iid.Name - ish.VirtualAddress;
          nameAmount = MAX_PATH;		// change to variable size name
                    
          if(nameOffset + nameAmount > lFileLen)
          	nameAmount = lFileLen - nameOffset;
          
          //seek to the name
          lPos = SetFilePointer(hFile, nameOffset, NULL, FILE_BEGIN);
          if(lPos == (DWORD)-1 || lPos > lFileLen)
          	{
            I_ASSERT(FALSE);
            readNames = FALSE;
            }
          //read in the name
          else if(!ReadFile(hFile, &dllNameRec.funcName, nameAmount, &cb,
          	NULL) || (cb != nameAmount))
          	{
          	I_ASSERT(FALSE);
          	readNames = FALSE;
          	}
         	else
         		{
         		//NULL terminate the name just in case
            dllNameRec.funcName[MAX_PATH - 1] = '\0';
         		dllNameRec.nameLen = (char)lstrlen(dllNameRec.funcName);
         		
            //see if the name is on the list not to add
						dontAddIt = FALSE;
						for (i=0; i<numOfOmmittedDlls; i++)
							{
							if (strcmpi(dllNameRec.funcName,dllsOmmitted[i]) == 0)
								{
								dontAddIt = TRUE;
								break;	//matched so stop iterating through list
								}
							}

						if (dontAddIt == FALSE) 
							{			// add it to buffer...
							AMemSetSize(bufferID, AMemGetSize(bufferID) + MAX_PATH);
							memBlkPtr = (char*)AMemLock(bufferID)	+ (int)AMemGetSize(bufferID) - MAX_PATH;
							CopyMemory(memBlkPtr, &(dllNameRec.funcName), MAX_PATH);
							AMemUnlock(bufferID);
							namesFound++;
							}			// name added to buffer.						
         		}
         	}
        }
      
      imageDirOffset += (DWORD)sizeof(iid);
      } //end while 
		}
    
	if (hFile != INVALID_HANDLE_VALUE)
		I_VERIFY(CloseHandle(hFile));

	return namesFound;
	}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
#define RC_MASK (0x80000000L)

struct IconHeader {
     WORD     reserved;     // Currently zero
     WORD     type;         // 1 for icons
     WORD     count;       // Number of components
     };
  

struct ResourceDirectory {
     BYTE     bWidth;
     BYTE     bHeight;
     BYTE     bColorCount;
     BYTE     bReserved;
     WORD     wPlanes;
     WORD     wBitCount;
     DWORD    lBytesInRes;
     WORD     wNameOrdinal;     // Points to component
     };

////////////////////////////////////////////////////////////
BOOL ReplaceIconData(
	BYTE* header,
	int resSize,
	LPVOID srcData,
	int dataSize)
  {
  BOOL rVal = FALSE;
  BYTE* pRawData;
  BITMAPINFOHEADER* pInfo = (BITMAPINFOHEADER*)header;
  
  if((resSize - sizeof(BITMAPINFOHEADER)) == dataSize)
  	{
    if(resSize == (pInfo->biSizeImage + 
    	((1 << (pInfo->biPlanes * pInfo->biBitCount)) * sizeof(RGBQUAD)) +
    	pInfo->biSize))
      {
      pRawData = header + sizeof(BITMAPINFOHEADER);
      CopyMemory(pRawData, srcData, dataSize);
      }
    else
    	I_ASSERT(FALSE);
    }
  else
  	I_ASSERT(FALSE);
    
  return rVal;
  }

/////////////////////////////////////////////////////////////
int GetIconOrdinal(
	BYTE* header)
  {
  int rVal = -1;
  ResourceDirectory* prd;
  int i;
  
  I_ASSERT(header);
  
  IconHeader* pIcon = (IconHeader*)header;
  
  if(pIcon->type == 1)
  	{
    if(pIcon->count == 2)
    	{
    	prd = (ResourceDirectory*)(header + sizeof(IconHeader));
    
    	for(i=0;i<pIcon->count;i++)
      	{
    		if(prd->bColorCount == 16)
      		{
          rVal = prd->wNameOrdinal;
        	break;
        	}
        }
    	}
    else
    	{
      I_ASSERT(FALSE); //code can't currently handle this case
      }      
    }
  else
  	I_ASSERT(FALSE);
  
  return rVal;
  }


//////////////////////////////////////////////////////////
PIMAGE_RESOURCE_DIRECTORY_ENTRY	GetRDE(
	PIMAGE_RESOURCE_DIRECTORY ird,
  LPCSTR resName,
  int resID,
 	BYTE* root)
  {
	PIMAGE_RESOURCE_DIRECTORY_ENTRY irde;
  PIMAGE_RESOURCE_DIRECTORY_ENTRY rVal = NULL;
  int i;
  int totalEntries;
  char buffer[64];
  PIMAGE_RESOURCE_DIR_STRING_U	pDirString;
  int count;
  
  //search for the resource by name
  //set i to total number of directory entries
  totalEntries = ird->NumberOfIdEntries + ird->NumberOfNamedEntries;
  
  I_ASSERT(totalEntries);
  
  //set a pointer to the first directory entry;
	irde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
		((BYTE*)ird + sizeof(IMAGE_RESOURCE_DIRECTORY));
  
	//search for the icon entry
	for(i=0;i<totalEntries;i++)
		{
	  if(irde->Name & RC_MASK)
	  	{
      if(resName)
      	{
		    //search for the icon section by name
		    pDirString = (PIMAGE_RESOURCE_DIR_STRING_U)(root + (irde->Name & ~RC_MASK)); 
		    	    
		    count =WideCharToMultiByte(CP_ACP, NULL,pDirString->NameString, 
		    	pDirString->Length, buffer, sizeof(buffer), NULL, NULL);
          
        I_ASSERT(count && count < sizeof(buffer));
        
        if(count >= sizeof(buffer))
        	count = sizeof(buffer) - 1;
          
        buffer[count] = '\0';  //Null terminate the string
		      
		    if(!lstrcmpi(buffer, resName))
		    	break;
        }
	    }
		else if(!resName)	//if resName is null then resId is assumed valid
	 		{
	 		//search for it by id
	 		if(irde->Name == resID)
	  		{
	    	break;
	  	  }          
	 		}   
	  
	  irde++;
	  }
    
  if(i < totalEntries)
  	rVal = irde;
    
  return rVal;
  }

//////////////////////////////////////////////////////////////////////
PIMAGE_RESOURCE_DATA_ENTRY GetDataDirectory(
	BYTE* rootNode,
	PIMAGE_RESOURCE_DIRECTORY ird,
  int type,
	LPCSTR resName,
	int resID)
	{
	PIMAGE_RESOURCE_DIRECTORY_ENTRY	irde;
  PIMAGE_RESOURCE_DATA_ENTRY rVal = NULL;
  
  //First look for the type directory
  irde = GetRDE(ird, NULL, type, rootNode);
    
  if(irde)
  	{
    if(irde->OffsetToData & RC_MASK)
    	{
      //more subdirectories
      ird = (PIMAGE_RESOURCE_DIRECTORY)
      	(rootNode + (irde->OffsetToData & ~RC_MASK));
        
      //search for the resource name
      irde = GetRDE(ird, resName, resID, rootNode);
      
      if(irde)
      	{
      	if(irde->OffsetToData & RC_MASK)
      		{
          //we need to map to the Language entry
          ird = (PIMAGE_RESOURCE_DIRECTORY)
          	(rootNode + (irde->OffsetToData & ~RC_MASK));
            
          irde = GetRDE(ird, NULL, MAKELANGID(LANG_NEUTRAL,
          	SUBLANG_NEUTRAL), rootNode);
          }
      	
      	if(irde && (!(irde->OffsetToData & RC_MASK)))
        	{												                               
          rVal = 	(PIMAGE_RESOURCE_DATA_ENTRY)
          	(rootNode + irde->OffsetToData);
          }
        else
        	I_ASSERT(FALSE); 
      	}
     	else
     		I_ASSERT(FALSE); 
      }
    else
    	I_ASSERT(FALSE); //unexpected
		}
  else
  	I_ASSERT(FALSE);

	return rVal;
  }

///////////////////////////////////////////////////////////////////////
//currently, type is expected to be an integer identifier of a resource
BOOL apb_ReplaceIconResource(
	LPVOID fileBase,
	LPCSTR resName,
	LPVOID rawData,
	int dataSize)
  {
	PIMAGE_FILE_HEADER fileHeader;
	PIMAGE_OPTIONAL_HEADER optionalHeader;
  PIMAGE_DATA_DIRECTORY dataDir;
  DWORD* sigPtr;
  PIMAGE_RESOURCE_DIRECTORY ird;
  BOOL result = FALSE;
 	PIMAGE_SECTION_HEADER psh;
  PIMAGE_RESOURCE_DATA_ENTRY irData;
  BYTE* rootNode;
  int i;
  BYTE* resData;
  
  I_ASSERT(fileBase);
  
	sigPtr =  (DWORD*)(((BYTE*)fileBase + ((PIMAGE_DOS_HEADER)fileBase)->e_lfanew));
	fileHeader = (PIMAGE_FILE_HEADER)((BYTE*)sigPtr + sizeof(IMAGE_NT_SIGNATURE)); 
  
  //verify an NT file
  if(*sigPtr == IMAGE_NT_SIGNATURE)
  	{
    optionalHeader = (PIMAGE_OPTIONAL_HEADER)((BYTE*)fileHeader + sizeof(IMAGE_FILE_HEADER));
    
    //get pointer to the beginning of the sections.
    psh = (PIMAGE_SECTION_HEADER)((BYTE*)optionalHeader + sizeof(IMAGE_OPTIONAL_HEADER));
    
    //get pointer to the resource data directory.
    dataDir =  (PIMAGE_DATA_DIRECTORY)((BYTE*)psh - (sizeof(IMAGE_DATA_DIRECTORY) * 
    	(IMAGE_NUMBEROF_DIRECTORY_ENTRIES - IMAGE_DIRECTORY_ENTRY_RESOURCE)));
    
    //locate the section containing the image directory.
    i=0;
    while(i++	< fileHeader->NumberOfSections)
    	{
			if(psh->VirtualAddress <= dataDir->VirtualAddress &&
  			psh->VirtualAddress + psh->SizeOfRawData > dataDir->VirtualAddress)
  			{
  			break;
        }
         
       psh++;                                      
      } //end while loop
      
   	if (i <= fileHeader->NumberOfSections)
   		{
      //first associate an ordinal with the icon
      
    	// get a pointer to the resource directory header 
    	ird = (PIMAGE_RESOURCE_DIRECTORY)
    		((BYTE*)fileBase + dataDir->VirtualAddress - psh->VirtualAddress +
    		psh->PointerToRawData); 
      
      rootNode = (BYTE*)ird;
      
    	irData = GetDataDirectory(rootNode, ird, (int)RT_GROUP_ICON,
      	resName, 0);
      	 
    	if(irData)
      	{
				resData = ((BYTE*)fileBase + irData->OffsetToData - psh->VirtualAddress +
					psh->PointerToRawData);
  
        i = GetIconOrdinal(resData);
        
        if(i>0)
        	{
        	//now actually replace the icon
          ird = (PIMAGE_RESOURCE_DIRECTORY)rootNode;
          
          //get the data directory of the icon based on its oridinal
          irData = GetDataDirectory(rootNode, ird, (int)RT_ICON, NULL, i);
        	
          if(irData)
          	{
						resData = ((BYTE*)fileBase + irData->OffsetToData - psh->VirtualAddress +
							psh->PointerToRawData);
              
            ReplaceIconData(resData, irData->Size, rawData, dataSize);
            
            }
          else
          	I_ASSERT(FALSE);
        	}
       	else
       		I_ASSERT(FALSE);     
    		}
      else
      	I_ASSERT(FALSE);
    	}
    else
    	I_ASSERT(FALSE);
    }
  else
  	ADBG_PRINT_E("Not an NT file.");
    
  return result;
  }


