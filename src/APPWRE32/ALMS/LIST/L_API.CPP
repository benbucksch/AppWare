#include "l_includ.h"

//////////////////////////////////////////////////////////////
//  Function: OListDeleteAll
//	Description: Deletes all items in a list object
//	Return:  TRUE if the list was cleared
//		OLISTINVALIDLISTOBJECT if oiList was not a List Object
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListDeleteAll(
	AOBJECTID 								oiList)
	{
	LPLIST 									list;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	list->RemoveAllItems();
	
	ResizeListEdit(oiList, 0L);
	list->EditControlChanged(NULL);

	UnlockListObject(oiList);
		
	return TRUE;
	}

//////////////////////////////////////////////////////////////
//  Function: OListClearSelections
//	Description: Deselects all selected items in a list object
//	Return:  TRUE if the list object's selections where cleared
//		OLISTINVALIDLISTOBJECT if oiList was not a List Object
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListClearSelections(
	AOBJECTID 								oiList)
	{
  LONG 										cleared=0L;
	LPLIST 									list;
	LPLIST_TABLE 						table;
	unsigned short					i;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	if (list->NumItems())
		{
		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);

		for (i=0; i < list->NumItems(); i++)
	   	{
			if (table[i].bIsSelected)
				{
				cleared++;
				table[i].bIsSelected = FALSE;
				}
			}
		UnlockListTable(oiList);
		}

	if (cleared)
		{
		list->SendSelectionsToText();
		list->AppearanceChanged(NULL);
		AEvtPostSignalAtTail(oiList, I_SIG_SELECTION_CHANGED);
		}

	UnlockListObject(oiList);
		
	return cleared;
	}


//////////////////////////////////////////////////////////////
//  Function: OListCountElements
//	Description: Returns a count of all the items in the list object
//	Return:  A count of the items in the list object or
//		OLISTINVALIDLISTOBJECT oiList is not a valid list object
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListCountElements(
	AOBJECTID 								oiList)
	{
	LPLIST 									list;
	LONG 										count;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	count = list->NumItems();
	
	UnlockListObject(oiList);

	return count;
  }


//////////////////////////////////////////////////////////////
//  Function: OListCountSelections
//	Description: Returns a count of all the selected items in the list object
//	Return:  A count of the selected items in the list object or
//		OLISTINVALIDLISTOBJECT oiList is not a valid list object
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListCountSelections(
	AOBJECTID 								oiList)
	{
	LPLIST_TABLE 						table;
	LONG 										selections=0L, count;
	int											i;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	if (OListCountElements(oiList))
		{
		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);

		for (i=0, count = OListCountElements(oiList); i < count; i++)
			{
			if (table[i].bIsSelected)
				selections++;
			}

		UnlockListTable(oiList);
		}

	return selections;
	}


//////////////////////////////////////////////////////////////
//  Function: OListDeleteElement
//	Description: Deletes the list element at a specified index
//		The index is zero based.
//	Return:  OLISTINVALIDLISTOBJECT if the list object is invalid or
//      OLISTROWOUTOFRANGE if the row index is out of range or
//		TRUE otherwise
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListDeleteElement(
	AOBJECTID 								oiList, 
	LONG 										index)
	{
	LPLIST 									list;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	if (index < 0 || index >= OListCountElements(oiList))
		return OLISTROWOUTOFRANGE;

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	if (list->RemoveItem((int)index))
		{
		list->SendSelectionsToText();
		AEvtPostSignalAtTail(oiList, I_SIG_SELECTION_CHANGED);
		}
	list->ItemsChanged();

	ResizeListEdit(oiList, 0L);
	list->EditControlChanged(NULL);

	UnlockListObject(oiList);

  return TRUE;
	}


//////////////////////////////////////////////////////////////
//  Function: OListDeleteSelections
//	Description: Deletes all selected elements in the list
//	Return:  OLISTINVALIDLISTOBJECT if the list object is invalid or
//		TRUE otherwise
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListDeleteSelections(AOBJECTID oiList)
	{
	LPLIST 									list;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	if (SizeofListEdit(oiList))
		{
		ResizeListEdit(oiList, 0L);
		list->EditControlChanged(NULL);
		}

	list->RemoveSelectedItems();

	UnlockListObject(oiList);

  return TRUE;
	}


//////////////////////////////////////////////////////////////
//  Function: OListGetElementLength
//	Description: Retrieves the string length of the element at the specified index
//  LONG OListGetElementLength(AOBJECTID oiList, LONG row);
//  	oiList is the object ID of the list object
//		row is the zero based index of the row in the list
//	The return value is
//		the length of the string at the specified row
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTROWOUTOFRANGE if the row is out of range
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListGetElementLength(
	AOBJECTID 								oiList, 
	LONG 										row)
	{
	LPLIST									list;
	LPLIST_TABLE 						table;
	LPSTR 									strings;
	LPSTR 									szBuffer;
	LONG 										rVal = OLISTROWOUTOFRANGE;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	if (row >= 0 && row < list->NumItems())
		{
		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);

		strings = LockListStrings(oiList);
		I_VERIFY_POINTER(strings, return OLISTINVALIDLISTOBJECT);

		szBuffer = GetListString((int)row, table, strings);
		rVal = lstrlen(szBuffer);

		UnlockListStrings(oiList);
		UnlockListTable(oiList);
		}
	else if (list->IsListCombobox()  &&   SizeofListEdit(oiList))
		{
		szBuffer = LockListEdit(oiList);
		I_VERIFY_POINTER(szBuffer, return 0);
		rVal = lstrlen(szBuffer);
		UnlockListEdit(oiList);
		}

	UnlockListObject(oiList);

	return rVal;
	}


//////////////////////////////////////////////////////////////
//  Function: OListGetElement
//	Description: Retrieves the text of the element at the specified index
//  LONG OListGetElement(AOBJECTID oiList, LONG row,  char* szElement, UINT nSize);
//  	oiList is the object ID of the list object
//		row is the zero based index of the row in the list
//		szElement is a pointer to a string buffer
//		nSize is the size of the string buffer
//	The return value is
//		the actual length of the string at the specified row
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTROWOUTOFRANGE if the row is out of range
//	Comments:
//		If szElement is NULL or nSize is 0, this function will still return
//		the actual length of the string at the specified row
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListGetElement(	
	AOBJECTID 								oiList, 
	LONG 										row, 
	LPSTR 									szElement, 
	UINT 										nSize)
	{
	LPLIST 									list;
	LPLIST_TABLE 						table;
	LPSTR 									strings, szBuffer;
	LONG 										lReturn = OLISTROWOUTOFRANGE;  //default if the row is not found

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);
	I_ALWAYSASSERT(nSize >0  &&  szElement, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	if (row >= 0 && row < list->NumItems())
		{
		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);

		strings = LockListStrings(oiList);
		I_VERIFY_POINTER(strings, return OLISTINVALIDLISTOBJECT);

		szBuffer = GetListString((int)row, table, strings);
		if (szBuffer)
			{
			lstrcpyn(szElement, szBuffer, nSize);
			szBuffer[nSize-1]=0;
			lReturn = lstrlen(szBuffer);
			}
		UnlockListTable(oiList);
		UnlockListStrings(oiList);
		}
	else if (list->IsListCombobox()  &&   SizeofListEdit(oiList))
		{
		szBuffer = LockListEdit(oiList);
		I_VERIFY_POINTER(szBuffer, return 0);
		lstrcpyn(szElement, szBuffer, nSize);
		szBuffer[nSize-1]=0;
		lReturn = lstrlen(szBuffer);
		UnlockListEdit(oiList);
		}

	UnlockListObject(oiList);
	return lReturn;
	}


//////////////////////////////////////////////////////////////
//  Function: OListGetSelection
//	Description: Gets the row number of the selected item
//	LONG OListGetSelection(AOBJECTID oiList, LONG itemIndex)
//		oiList is the object ID of the list object
//		itemIndex is the zero based index of the selected item
//			i.e. 0 specifies the first selected item
//	The return value is
//		the zero based index of the selected element in the list object
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTSELECTIONOUTOFRANGE if the itemIndex is out of range
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListGetSelection(
	AOBJECTID 								oiList, 
	LONG 										itemIndex)
	{
	LPLIST 									list;
	LPLIST_TABLE 						table;
	LONG 										lReturn;
	int 										index, i;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	if (list->NumItems())
		{
		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);

		for (lReturn = OLISTSELECTIONOUTOFRANGE, index=i=0; i < list->NumItems() && lReturn==OLISTSELECTIONOUTOFRANGE; i++)
			{
			if (table[i].bIsSelected)
				{
				if (index == itemIndex)
	        lReturn = i;
				else
					index++;
				}
			}

		UnlockListTable(oiList);
		}

	UnlockListObject(oiList);
	return lReturn;
	}


//////////////////////////////////////////////////////////////
//  Function: OListNewElement
//	Description: Inserts one or more new elements in a list at
//		a specified position.  
//		
//	LONG OListNewElement(AOBJECTID oiList, LPSTR itemText, LONG startingIndex)
//		oiList is the object ID of the list object
//		itemText contains the text of the element to be added.  To insert
//			several elements at a time, you pass a Text object containing
//			multiple, return-delimited entries, one for each element.
//		startIndex is the zero based index where the element is to be
//			inserted.  If the startingIndex is negitive, the list object appends 
//			the element(s) to the end of the list.
//	The return value is
//		the number of elements added to the list or
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied 
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListNewElement(
	AOBJECTID 							oiList, 
	LPSTR 									itemText, 
	LONG 										startIndex)
	{
	LPLIST									list;
	long										nListItems, index, end, l;
	BOOL										bChanged=FALSE;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);
	I_ALWAYSASSERT(itemText, return 0);

	nListItems = OListCountElements(oiList);

	if (startIndex < 0L || startIndex > nListItems)
		startIndex = nListItems; // insert the new element at the end of the list

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	for (index = startIndex, end =l= 0L; GetNextLine(itemText, &l, &end); l = end, index++)
		{
		list->addItem(&itemText[l], index, end-l);
		bChanged = TRUE;
		}

	if (bChanged)
		list->ItemsChanged();

	UnlockListObject(oiList);

  return (index - startIndex);
	}


//////////////////////////////////////////////////////////////
//  Function: OListSetElement
//	Description:  Replaces the text of the indexed element with itemText.
//
//	LONG OListSetElement(AOBJECTID oiList, LPSTR itemText, LONG itemIndex)
//		oiList is the object ID of the list object
//		itemText points to the replacement text.  
//		itemIndex is the zero based index where the element text is to be replaced.  
//    if itemIndex is less than zero, the text is added to the combo edit-control part
//	The return value is
//		TRUE if successful
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTSELECTIONOUTOFRANGE if the itemIndex is out of range
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListSetElement(
	AOBJECTID 								oiList, 
	LPSTR 									itemText, 
	LONG 										itemIndex)
	{
 	LPLIST 									list;
	LPLIST_TABLE 						table;
	LONG 										nListItems;
	UINT 										row;
	BOOL 										bIsSelected;
	LONG										rVal = OLISTSELECTIONOUTOFRANGE;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);
	I_ALWAYSASSERT(itemText, return FALSE);

	nListItems = OListCountElements(oiList);
	I_ALWAYSASSERT(itemIndex <= nListItems, return OLISTSELECTIONOUTOFRANGE);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);
	
	if (itemIndex < 0)
		{
		if (list->IsListCombobox())
			{
			OListSetEditText(oiList, itemText);
			rVal = TRUE;
			}
		}
	else
		{
		row = (UINT) itemIndex;

		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);
		bIsSelected = table[row].bIsSelected;
		UnlockListTable(oiList);

		list->RemoveItem(row);			//remove the old
		list->addItem(itemText, row);		//add the new

		table = LockListTable(oiList);
		table[row].bIsSelected = bIsSelected;	
		UnlockListTable(oiList);
		
		if (bIsSelected)
			list->SendSelectionsToText();	//if we changeda selected item
		list->ItemsChanged();
		rVal = TRUE;
		}
	
	UnlockListObject(oiList);

  return rVal;
	}


//////////////////////////////////////////////////////////////
//  Function: OListSetEditText
//	Description:  Sets the text of the combo box edit control.
//
//	LONG OListSetEditText(AOBJECTID oiList, LPSTR itemText)
//		oiList is the object ID of the list object
//		itemText points to the new edit text.  
//////////////////////////////////////////////////////////////
extern "C" void ALMAPI OListSetEditText(
	AOBJECTID 								oiList, 
	LPSTR 									lpText)
	{
	LPLIST									list;
	LPSTR 									editBoxText;
	
	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return);
	I_ALWAYSASSERT(lpText, return);

	ResizeListEdit(oiList, lstrlen(lpText)+1);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return);

	ResizeListEdit(oiList, lstrlen(lpText)+1);
	editBoxText = LockListEdit(oiList);
	I_VERIFY_POINTER(editBoxText, return);

	lstrcpy(editBoxText, lpText);
	list->EditControlChanged(NULL);

	UnlockListEdit(oiList);
	UnlockListObject(oiList);
	}

//////////////////////////////////////////////////////////////
//  Function: OListSetSelection
//	Description:  Selects an element specified by the row index.
//
//	LONG OListSetSelection(AOBJECTID oiList, LONG itemIndex)
//		oiList is the object ID of the list object
//		itemIndex is the zero based index of the element to be selected.
//	The return value is
//		TRUE if successful
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTSELECTIONOUTOFRANGE if the itemIndex is out of range
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListSetSelection(
	AOBJECTID 								oiList, 
	LONG 										itemIndex)
	{
	LONG 										nListItems;
	LPLIST 									list;
	LPLIST_TABLE 						table;
	int 										i;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	nListItems = OListCountElements(oiList);

	I_ALWAYSASSERT(itemIndex >= 0L, return OLISTSELECTIONOUTOFRANGE);
	I_ALWAYSASSERT(itemIndex <= nListItems, return OLISTSELECTIONOUTOFRANGE);
	I_ALWAYSASSERT(itemIndex < OLISTMAXINDEX, return OLISTSELECTIONOUTOFRANGE);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);
	
	table = LockListTable(oiList);
	I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);

	if (!table[(UINT)itemIndex].bIsSelected)
		{

		if (!list->DoesMultipleSelections())
			{
			for(i = 0; i < nListItems; i++)
				table[i].bIsSelected = FALSE;
			}

		table[(UINT)itemIndex].bIsSelected = TRUE; //Select it

		list->SendSelectionsToText();
		list->AppearanceChanged(NULL);
		AEvtPostSignalAtTail(oiList, I_SIG_SELECTION_CHANGED);
		}
	
	UnlockListObject(oiList);
	UnlockListTable(oiList);
  
  return TRUE;
  }


//////////////////////////////////////////////////////////////
//  Function: OListSort
//	Description:  Sorts a list.
//
//	LONG OListSort(AOBJECTID oiList)
//		oiList is the object ID of the list object
//	The return value is
//		TRUE if successful
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI OListSort(
	AOBJECTID 								oiList)
	{
	LPLIST 									list;
	LPLIST_TABLE 						table;
	LPSTR 									strings;
	BOOL										bSomethingSelected=FALSE;
	int											i;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return OLISTINVALIDLISTOBJECT);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return OLISTINVALIDLISTOBJECT);

	if (list->NumItems())
		{
		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return OLISTINVALIDLISTOBJECT);

		strings = LockListStrings(oiList);
		I_VERIFY_POINTER(strings, return OLISTINVALIDLISTOBJECT);

		for(i=0; i<list->NumItems() &&  !bSomethingSelected; i++)
			bSomethingSelected = table[i].bIsSelected;

		SortList(table, strings, list->NumItems());
		if (bSomethingSelected)
			{
			list->SendSelectionsToText();
			AEvtPostSignalAtTail(oiList, I_SIG_SELECTION_CHANGED);
			}

		list->ItemsChanged();

		UnlockListTable(oiList);
		UnlockListStrings(oiList);
		}

	UnlockListObject(oiList);
  return TRUE;
	}
