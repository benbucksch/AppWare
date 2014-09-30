#define		STRICT
#define		ASTRICT

#include "windows.h"
#include "winlist.h"
#include "windows.h"
// #include "memlib.h"


typedef struct WLEtag{
	HWND WindowHandle;
	void * objectPtr;
	WLEtag *	nextPtr;
}WindowListElement;

typedef WindowListElement * WLEptr;


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	WindowList
//
// CLASS.......:	WindowList
//
// DESCRIPTION.:	Constructor.  Initializes head and tail instance variables
//								to NULL.
//
/////////////////////////////////////////////////////////////////////////////

WINDOWLIST::WINDOWLIST()
	{
	head = NULL;
	tail = NULL;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	IsListEmpty
//
// CLASS.......:  WindowList
//
// DESCRIPTION.:  returns true if the list is empty else returns false.
//
// RETURN VALUE:  zero for false or non-zero for true.
//
/////////////////////////////////////////////////////////////////////////////

WINDOWLIST::IsListEmpty()
	{
	if (head)
		return(FALSE);
	else
		return(TRUE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Add
//
// CLASS.......:  WindowList
//
// DESCRIPTION.:  Adds the window handle 'hwnd' and the associated object
//								pointer 'theObject' to the last element in the list.
//								(WLEptr)head and (WLEptr)tail are set to their new respected values.
//
// PARAMETERS..:  theObject- type void *, its actually the
//									this pointer of the object
//
// RETURN VALUE:  non-zero if the pointer and handle were added else zero.
//
// ASSUMPTIONS.:  when called, the pointer which theObject receives must be
//	              casted to a void *
//
// SIDE-EFFECTS:  allocates Local memory to hold the list elements.
/////////////////////////////////////////////////////////////////////////////

BOOL WINDOWLIST::Add(
	HWND hwnd,
	void * theObject)
	{
	WLEptr data;

	// removed ref to Allocate((unsigned)sizeof(WindowListElement));
  // SKJ Oct.14
//	data =(WLEptr)MemAlloc((unsigned)sizeof(WindowListElement));
	data = new WindowListElement;

	if	(data)
		{
		if	(!(WLEptr)head)
			head = data;
		else
			{
			((WLEptr)tail)->nextPtr = data;
			}
		(WLEptr)tail = data;
		data->WindowHandle = hwnd;
		data->objectPtr = theObject;
		data->nextPtr = NULL;
		return(TRUE);
		}
	else
		return(FALSE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Remove
//
// CLASS.......:  WindowList
//
// DESCRIPTION.:  Removes the window and its associated object pointer from
//								the list.
//
// PARAMETERS..:  hwnd- the handle of the window to remove.
//
// RETURN VALUE:  non-zero if the window was removed else zero.
//
// SIDE-EFFECTS:	the Local memory which was allocated is now freed.
/////////////////////////////////////////////////////////////////////////////

BOOL WINDOWLIST::Remove(
	HWND hwnd)
	{
	WLEptr 	parent, child;

	if(IsListEmpty())
		return(FALSE);
	else
		{
		parent = (WLEptr)head;
		if(parent->WindowHandle == hwnd)
			{
			head = parent->nextPtr;

//			MemFree((LPSTR)parent); // Deallocate
			delete[] parent;

			return(TRUE);
			}
		else
			{
			child = parent->nextPtr;
			while(child)
				{
				if(child->WindowHandle == hwnd)
					{
					if(tail == child)
						tail = parent;
					parent->nextPtr = child->nextPtr;
//					MemFree((LPSTR)child);
					delete[] child;


					// Deallocate((LPSTR)child);
					return(TRUE);
					}
				else
					{
					parent = child;
					child = parent->nextPtr;
					}
				}
			return(FALSE);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetObject
//
// CLASS.......:  WindowList
//
// DESCRIPTION.:  returns the object pointer which is associated to the
//								window handle hwnd.  The pointer is returned as a void *
//
// PARAMETERS..:  hwnd-	handle to the window
//
// RETURN VALUE:  void * or NULL if the window is not in the
//								list.
/////////////////////////////////////////////////////////////////////////////

void * WINDOWLIST::GetObject(
	HWND hwnd)
	{
	WLEptr temp;

	temp = (WLEptr)head;
	while	(temp)
		{
		if	(temp->WindowHandle == hwnd)
			return temp->objectPtr;
		temp = temp->nextPtr;
		}
	return(NULL);
	}


/*
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetWindow
//
// CLASS.......:  WindowList
//
// DESCRIPTION.:  returns the window handle which is associated with the
//								object pointer theObject.
//
// PARAMETERS..:  theObject-	pointer to an object
//
// RETURN VALUE:  HWND which is a valid window handle or NULL if the object
//								was not in the list
//
/////////////////////////////////////////////////////////////////////////////

HWND WINDOWLIST::GetWindow(
	void * theObject)
	{
	WLEptr temp;

	temp = (WLEptr)head;
	while	(temp)
		{
		if	(temp->objectPtr == theObject)
			return  temp->WindowHandle;
		temp = temp->nextPtr;
		}
	return(NULL);
	}
*/

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetNextObject
//
// CLASS.......:  WindowList
//
// DESCRIPTION.:  returns a pointer to the object which is next in the list
//									following the object pointed to by 'theObject'.
//								if 'theObject' is NULL then the first object is returned.
//								if 'theObject' is not in the list or no objects follow,
//								NULL is returned.
//
// PARAMETERS..:	theObject- pointer to an object
//
// RETURN VALUE:  LPSTR- points to the object which is next in the list or
//								NULL is returned if no objects follow.
/////////////////////////////////////////////////////////////////////////////
/*
void * WINDOWLIST::GetNextObject(
	void * theObject)
	{
	WLEptr temp;

	temp = (WLEptr)head;
	if(!theObject && temp)
		return temp->objectPtr;
	else
		{
		while	(temp)
			{
			if	(temp->objectPtr == theObject)
				if	(temp->nextPtr)
					return	temp->nextPtr->objectPtr;
				else
					return NULL;
			temp = temp->nextPtr;
			}
		return(NULL);
		}
	}
*/

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetNextWindow
//
// CLASS.......:  WindowList
//
// DESCRIPTION.:  returns the handle of the window in the list which
//								immediately follows the window 'hwnd'.
//
// PARAMETERS..:  hwnd- of type HWND.  Handle to a window in the list.
//
// RETURN VALUE:  HWND if the next window exists in the list else NULL.
/////////////////////////////////////////////////////////////////////////////

/*
HWND WINDOWLIST::GetNextWindow(
	HWND hwnd)
	{
	WLEptr temp;

	temp = (WLEptr)head;
	if(!hwnd && temp)
		return temp->WindowHandle;
	else
		{
		while	(temp)
			{
			if	(temp->WindowHandle == hwnd)
				if	(temp->nextPtr)
					return temp->nextPtr->WindowHandle;
				else
					return NULL;
			temp = temp->nextPtr;
			}
		return(NULL);
		}
	}
*/

