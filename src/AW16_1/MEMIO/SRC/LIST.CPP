#include <memory.h>
#include "almsys.h"

#include "list.h"
#include "listmbr.h"

LIST::LIST()
	{
	head = NULL;
	tail = NULL;
	}

//Add items to the tail
BOOL LIST::Add(
	LPLISTMEMBER memberPtr)
	{

	if(memberPtr)
		{
		if(!head)
			{
			head = memberPtr;
			memberPtr->prevMember = NULL;
			}
		else
			{
			tail->nextMember = memberPtr;
			memberPtr->prevMember = tail;
			}
		tail = memberPtr;
		memberPtr->nextMember = NULL;
		return(TRUE);
		}
	else
		return(FALSE);
	}


//assumes memberPtr is a valid member
BOOL LIST::Remove(
	LPLISTMEMBER memberPtr)
	{
//	LPLISTMEMBER temp;


	//used to check to make sure that memberPtr belonged to this list
	//but stopped that for efficiency reasons.

  //temp = head;
	//while((temp!=NULL) && (temp != memberPtr))
		//temp = temp->nextMember;

  //below, memberPtr is now used instead of temp
	if(memberPtr)
		{
		if(memberPtr == head)
			head = memberPtr->nextMember;
		if(tail == memberPtr)
			tail = memberPtr->prevMember;
		if(memberPtr->prevMember)
			memberPtr->prevMember->nextMember = memberPtr->nextMember;
		if(memberPtr->nextMember)
			memberPtr->nextMember->prevMember = memberPtr->prevMember;

		return TRUE;
		}
	else
		return FALSE;
	}




LPLISTMEMBER LIST::GetNextMember(
	LPLISTMEMBER memberPtr)
	{
	if(!memberPtr)
		return head;
	else
		return memberPtr->nextMember;
	}

LPLISTMEMBER LIST::GetPrevMember(
	LPLISTMEMBER memberPtr)
	{
	if(!memberPtr)
		return tail;
	else
		return memberPtr->prevMember;
	}

BOOL LIST::IsListEmpty()
	{
	if(head)
		return FALSE;
	else
		return TRUE;
	}

unsigned int LIST::GetMemberCount()
	{
	LPLISTMEMBER m = NULL;
	unsigned int count = 0;

	while(NULL != (m = GetNextMember(m)))
		count++;

	return count;
	}

//////////////////////////////////////////////////////////////////////
// STACK Class ///////////////////////////////////////////////////////

BOOL STACK::Push(
	LPLISTMEMBER memberPtr)
	{
  return Add(memberPtr); //add to tail
	}

LPLISTMEMBER STACK::Pop()
	{
	LPLISTMEMBER popped = tail;

	if(popped)
		Remove(popped);

	return popped;
	}

