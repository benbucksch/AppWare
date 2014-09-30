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

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"strclass.cpp"
//
// Author:	Robert Lundahl---modified by Kevin Smart
//   Start:		July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	String Class to manage the list of objects
//
////////////////////////////////////////////////////////////////////////////

#include <a_alm.h>
#include "strclass.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////

STRING::STRING(AOBJECTID theObject, long theData)

{
  objid = theObject;
  data  = theData;

  arrayPtr = NULL;

  dataPtr = (char*)AObjLockData(objid, data);

  if(dataPtr==NULL) {
    length = sizeof(Header) - sizeof(char);
    Resize();					// resize will leave it locked

    count = head->count = 0;
    }
  else {
    head = (Header*)dataPtr;

    length = head->length;

    Offsets( head->count );
    }
}

////////////////////////////////////////////////////////////////////////////

STRING::~STRING()

{
	if (dataPtr)
		AObjUnlockData(objid, data);

  delete arrayPtr;
}

////////////////////////////////////////////////////////////////////////////

void	STRING::Clear()

{
  length = sizeof(Header) - sizeof(char);
  Resize();
  count = head->count = 0;
}

////////////////////////////////////////////////////////////////////////////

WORD	STRING::Append(char* lpstr)

{
  WORD		i;
  int			oldlen;
  char**	aPtr;

  oldlen = length;
  length += (lstrlen(lpstr)+ 1);
  Resize();
  Offsets(count);

	aPtr = (char**)new char*[(int)count+1];
  if(aPtr==NULL) {
    return -1;
    }

  for (i=0; i<count; i++) {
    aPtr[i] = arrayPtr[i];
    }

  delete arrayPtr;

  arrayPtr = aPtr;
  arrayPtr[(int)count] = ((char*)dataPtr)+oldlen;

  lstrcpy(arrayPtr[(int)count], lpstr);

  head->count = ++count;
  return (WORD)(count-1);
}

////////////////////////////////////////////////////////////////////////////

BOOL	STRING::Insert(char* lpstr, WORD index)

{
  if(index>=count) {
    return 0;
    }

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
}

////////////////////////////////////////////////////////////////////////////

BOOL	STRING::Delete(WORD index)

{
  if(count==0) {
    return 0;
    }

  length = length - (lstrlen(arrayPtr[index])+1);

  MakeWork();
  Resize();

  vestp = &head->string;
  VestWork(0, index);
  VestWork(index+1, count);

  DeleteWork();

  Offsets(--count);
  return 1;
}

////////////////////////////////////////////////////////////////////////////

void	STRING::MakeWork()
{
  char*		p;
  WORD		i;

	arrayWrk = (char**)new char*[(int)count+2]; 	// 1 for insert 1 for null
  if(arrayWrk==NULL) {
    MessageBox(NULL, "STRING: new Failed", NULL, MB_OK | MB_ICONEXCLAMATION);
    return;
    }

//  p = &head->string;

  for(i=0; i<count; i++) {
    p = arrayPtr[i];
    arrayWrk[i] = new char[lstrlen(p)+1];
    lstrcpy(arrayWrk[i], p);
    }
  arrayWrk[i] = NULL;
}

////////////////////////////////////////////////////////////////////////////

void	STRING::VestWork(char* lpstr)

{
  while ((*vestp++ = *lpstr++)!=0);
}

////////////////////////////////////////////////////////////////////////////

void	STRING::VestWork(int start, int end)

{
  int		i;
  LPSTR	q;

  for(i=start; i<end; i++) {
    q = arrayWrk[i];

    while ((*vestp++ = *q++)!=0);
    }
}

////////////////////////////////////////////////////////////////////////////

void	STRING::DeleteWork()

{
  char**	pp;

  pp = arrayWrk;

  while (*pp) {
    delete*pp;
    pp++;
    }
  delete arrayWrk;
}

////////////////////////////////////////////////////////////////////////////

void	STRING::Offsets(int cnt)

{
  WORD	i;
  LPSTR	p;

  count = cnt;

  if(count==0) {
    return;
    }

  // Set Offsets
  if(arrayPtr != NULL) {
    delete arrayPtr;
    }

  arrayPtr = (char**)new char*[cnt];
  if(arrayPtr==NULL) {
    MessageBox(NULL, "STRING: new Failed", NULL, MB_OK | MB_ICONEXCLAMATION );
    return;
    }

  i = 0;
  p = &head->string;
  head->count = cnt;

  for(;;) {
    arrayPtr[i] = p;
    i++;

    if(i==count) {
      break;
      }

    while (*p++);
    }
}

////////////////////////////////////////////////////////////////////////////

void	STRING::Resize()

{
	if (dataPtr)
		AObjUnlockData(objid, data);

  AObjResizeData(objid, data, length);

  dataPtr = (void*)AObjLockData(objid, data);

	ADBG_ASSERT_E(dataPtr != NULL, return, "STRING::Resize failed");

  head = (Header*)dataPtr;

  head->length = length;
}

////////////////////////////////////////////////////////////////////////////

void STRING::CopyDataPtr(AOBJECTID dstObj, long index)

{
	
  long 		srcSize    = AObjGetDataSize(objid, index);

	LPVOID 	dstPtr;
	LPVOID 	srcPtr;

  srcPtr = AObjLockData(objid, index);
	AObjResizeData(dstObj, index, srcSize);
	dstPtr = AObjLockData(dstObj, index);

  if(objid!=NULL && dstPtr!=NULL) {
		CopyMemory(dstPtr, srcPtr, (size_t)srcSize);
    }

	AObjUnlockData(dstObj, index);
	AObjUnlockData(objid, index);
}

////////////////////////////////////////////////////////////////////////////
