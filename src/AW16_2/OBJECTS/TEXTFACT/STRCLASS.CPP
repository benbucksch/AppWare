///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// strclass.hpp
// Robert Lundahl/ Edgar C. Jerez
//
// String Object
//   Append String
//	 Insert String
//   Delete String
//   Get String
//   Count
//
// Start Date 07/01/92
// End   Date 07/01/92
///////////////////////////////////////////////////////////////


#include "tf_incld.h"

///////////////////////////////////////////////////////////////
// Storage Format
//
// count 			total number of strings   WORD
// length                                     WORD
// string ...       sequential strings
//
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// STRING::STRING:
//
///////////////////////////////////////////////////////////////

STRING::STRING ( OBJECTID theobject, LONG thedata )
{
	objid = theobject;
	data  = thedata;

	dataPtr = (BYTE*)AObjLockData(objid, data);
	arrayPtr = NULL;

	if (dataPtr == NULL)
	{
		length = sizeof(Header) - sizeof(char);
		Resize();  // Resize() leaves lock on
		count = head->count = 0;
	}
	else
	{
		head = (Header*)dataPtr;
		length = head->length;
		Offsets( head->count );
	}
}				// STRING::STRING


///////////////////////////////////////////////////////////////
// STRING::~STRING:
//
///////////////////////////////////////////////////////////////

STRING::~STRING ( VOID )
{
	AObjUnlockData(objid, data);
  dataPtr = NULL;  // pointer is invalid, so set it to NULL
}				// STRING::~STRING


///////////////////////////////////////////////////////////////
// STRING::Clear:
//
///////////////////////////////////////////////////////////////

VOID STRING::Clear ( VOID )
{
	length = sizeof(Header) - sizeof(char);
	Resize();
	count = head->count = 0;
}				// STRING::Clear


///////////////////////////////////////////////////////////////
// STRING::Resize:
//
///////////////////////////////////////////////////////////////

VOID STRING::Resize ( VOID )
{
	if (dataPtr)  // data is locked
  	{
		AObjUnlockData(objid, data);
		AObjResizeData(objid, data, length);
		dataPtr = (VOID*) AObjLockData(objid, data);
		head = (Header*)dataPtr;
  	head->length = length;
		}

	else   // data isn't locked, so just resize it.
		{
		AObjResizeData(objid, data, length);
		dataPtr = (VOID*)AObjLockData(objid, data);  // get pointer to change header info
		head = (Header*)dataPtr;
		head->length = length;
//		AObjUnlockData(objid, data);
//		dataPtr = NULL;   // pointer is invalid, so set it to zero
		}

//	head = (Header*)dataPtr;

//	head->length = length;
}				// STRING::Resize...


///////////////////////////////////////////////////////////////
// STRING::SetData:
//
///////////////////////////////////////////////////////////////

VOID STRING::SetData ( OBJECTID theobject, long theindex,
											 void* thedata, long thesize  )
{
	objid = theobject;
	long index = theindex;
	void* newdata  = thedata;
	long size = thesize;

	if (dataPtr)  // data is locked
		AObjUnlockData(objid, index);

	AObjSetData(objid, index, newdata, size);
	dataPtr = (BYTE*)AObjLockData(objid, index);

}				// STRING::SetData...


///////////////////////////////////////////////////////////////
// STRING::Append:
//
///////////////////////////////////////////////////////////////

WORD STRING::Append ( BYTE* lpstr )
{
	WORD		i;
	WORD		oldlen;
	BYTE**		aPtr;

	oldlen = length;
	length += (lstrlen((LPCSTR)lpstr)+ 1);
	Resize();
	Offsets(count);

	aPtr = (BYTE**)new BYTE*[count+1];
	if (aPtr == NULL)
		return -1;

	for (i = 0; i < count; i++)
		aPtr[i] = arrayPtr[i];

	delete arrayPtr;

	arrayPtr = aPtr;
	arrayPtr[count] = ((BYTE*)dataPtr)+oldlen;

	lstrcpy((LPSTR)arrayPtr[count], (LPCSTR)lpstr);

	head->count = ++count;
	return count-1;
}				// STRING::Append...


///////////////////////////////////////////////////////////////
// STRING::MakeWork:
//
///////////////////////////////////////////////////////////////

VOID   STRING::MakeWork ( VOID )
{
	BYTE*		p;
	WORD		i;

	arrayWrk = (BYTE**)new BYTE*[count+2]; 	// 1 for insert 1 for null
	if (arrayWrk == NULL)
	{
		MessageBox(NULL, "STRING: new Failed", NULL, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	p = &head->string;

	for (i = 0; i < count; i++)
	{
		p = arrayPtr[i];
		arrayWrk[i] = new BYTE[lstrlen((LPCSTR)p)+1];
		lstrcpy((LPSTR)arrayWrk[i], (LPCSTR)p);
	}
	arrayWrk[i] = NULL;
}				// STRING::MakeWork...


///////////////////////////////////////////////////////////////
// STRING::VestWork:
//
///////////////////////////////////////////////////////////////

VOID STRING::VestWork ( BYTE* lpstr )
{
	while ( (*vestp++ = *lpstr++) != 0 );
}				// STRING::VestWork ( BYTE* lpstr )...


///////////////////////////////////////////////////////////////
// STRING::VestWork:
//
///////////////////////////////////////////////////////////////

VOID STRING::VestWork ( WORD start, WORD end)
  {
	WORD	i;
	LPSTR	q;


	for (i = start; i < end; i++)
	  {
		if ((q = (LPSTR)arrayWrk[i]) != NULL)
      {
  		while ((*vestp++ = *q++) != 0);
      }
    else
      {
      i = end;
      }
  	}
  }				// STRING::VestWork( WORD start, WORD end)...


///////////////////////////////////////////////////////////////
// STRING::DeleteWork:
//
///////////////////////////////////////////////////////////////

VOID STRING::DeleteWork ( VOID )
{
	BYTE**	pp;

	pp = arrayWrk;

	while (*pp)
	{
		delete*pp;
		pp++;
	}
	delete arrayWrk;
}				// STRING::DeleteWork...


///////////////////////////////////////////////////////////////
// STRING::Offsets:
//
///////////////////////////////////////////////////////////////

VOID STRING::Offsets ( WORD cnt )
{
	WORD	i;
	LPSTR	p;

	count = cnt;

	if (count == 0)
		return;

	// Set Offsets
	if (arrayPtr != NULL)
		delete arrayPtr;

	arrayPtr = (BYTE**)new BYTE*[cnt];
	if (arrayPtr == NULL)
	{
		MessageBox(NULL, "STRING: new Failed", NULL, MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	i = 0;
	p = (LPSTR)&head->string;
	head->count = cnt;

	for(;;)
	{
		(LPSTR)arrayPtr[i++] = p;

		if (i == count)
			break;

		while (*p++);
	}
}				// STRING::Offsets...


///////////////////////////////////////////////////////////////
// STRING::Delete:
//
///////////////////////////////////////////////////////////////

BOOL STRING::Delete ( WORD index )
{
	if (count == 0)
		return 0;

	length -= (lstrlen((LPCSTR)arrayPtr[index])+1);

	MakeWork();
	Resize();

	vestp = &head->string;
	VestWork(0, index);
	VestWork(index+1, count);

	DeleteWork();

	Offsets(--count);
	return 1;
}				// STRING::Delete...



/*
///////////////////////////////////////////////////////////////
// STRING::Insert:
//
///////////////////////////////////////////////////////////////

BOOL STRING::Insert ( BYTE* lpstr, WORD index )
{
	if (index >= count)
		return 0;

	length += lstrlen(lpstr) + 1;

	MakeWork();
	Resize();
	vestp = &head->string;
	VestWork(0, index);
	VestWork(lpstr);
	VestWork(index, count);
	DeleteWork();

	Offsets(++count);
	return 1;
}				// STRING::Insert...
*/
///////////////////////////////////////////////////////////////
// STRING::CopyDataPtr:
//
///////////////////////////////////////////////////////////////

void STRING::CopyDataPtr(OBJECTID dstObj, long index)
	{
	long srcSize	= AObjGetDataSize(objid, index);

	LPVOID	dstPtr;
	LPVOID	srcPtr;

	srcPtr = AObjLockData(objid, index);
	AObjResizeData(dstObj, index, srcSize);
	dstPtr = AObjLockData(dstObj, index);

	if(objid!=NULL && dstPtr!=NULL)
		{
		_fmemcpy(dstPtr, srcPtr, (size_t)srcSize);
		}

	AObjUnlockData(dstObj, index);
	AObjUnlockData(objid, index);
	}				

///////////////////////////////////////////////////////////////
