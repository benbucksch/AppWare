///////////////////////////////////////
//  LISTFUNC.CPP
//  DATE:  9/16/93
//  Author:  Paul Ruben
//  Description:  Appware Functions for the List Object
///////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <op_wnd.h>
#include <o_text.h>
#include <o_number.h>
#include <o_list.h>
#include "attribut.h"
#include "list.h"

//////////////////////////////////////////////////////////////
//  Function: ClearSelectionsFunc
//	Description:  Deselects all selected items in a list object
//  Input1: List Object
//  Flow1: After Clearing
//  No Outputs
//////////////////////////////////////////////////////////////
EXTERNC ClearSelectionsFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST) ;
	OListClearSelections(oiList) ;
	theSystem->message1 = 1;		//only one exit flow
	}

//////////////////////////////////////////////////////////////
//  Function: OListClearSelections
//	Description: Deselects all selected items in a list object
//	Return:  TRUE if the list object's selections where cleared
//		OLISTINVALIDLISTOBJECT if oiList was not a List Object
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListClearSelections(OBJECTID oiList)
	{
	if (AObjGetType(oiList) != OTYPE_LIST) // verify that oiList is the correct object type ;
		return OLISTINVALIDLISTOBJECT ;

    LONG cleared = 0 ;
	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);
	if (table && list)
		{
		for (int i = 0; i < list->m_nItems; i++)
        	{
			if (table[i].isSelected)
            	cleared++ ;
			table[i].isSelected = FALSE;
			}
		list->SendSelectionToText();
		PostObjectChanged(oiList, MAKELONG(LST_APPERANCE_CHANGED, 0));
		}
	AObjUnlockData(oiList, TABLE_INFO);
	AObjUnlockData(oiList, LIST_INFO);
    return cleared ;
	}

//////////////////////////////////////////////////////////////
//  Function: CountElementsFunc
//	Description:  Reports the total number of elements and the number
//		of selected elements in a list.
//  Input1: List Object
//  Flow1: After Returning
//  Output1 (Optional): Number Object - number of items in list
//  Output2 (Optional): Number Object - number of selected items in list
//////////////////////////////////////////////////////////////
EXTERNC CountElementsFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST);

	OBJECTID oiElements = AObjCreate(OTYPE_NUMBER);
	ONmbrSetInteger(oiElements, OListCountElements(oiList), FALSE) ;
	AFuncSetParameter(1, oiElements);

	OBJECTID oiSelections = AObjCreate(OTYPE_NUMBER);
	ONmbrSetInteger(oiSelections, OListCountSelections(oiList), FALSE) ;
	AFuncSetParameter(2, oiSelections);

	theSystem->message1 = 1;		//only one exit flow
	}
//////////////////////////////////////////////////////////////
//  Function: OListCountElements
//	Description: Returns a count of all the items in the list object
//	Return:  A count of the items in the list object or
//		OLISTINVALIDLISTOBJECT oiList is not a valid list object
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListCountElements(OBJECTID oiList)
	{
	if (AObjGetType(oiList) != OTYPE_LIST) // verify that oiList is the correct object type ;
		return OLISTINVALIDLISTOBJECT ;

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	if (list == NULL)
		return OLISTINVALIDLISTOBJECT ;

	LONG count = list->m_nItems ;
	AObjUnlockData(oiList, LIST_INFO);
	return count ;
    }
//////////////////////////////////////////////////////////////
//  Function: OListCountSelections
//	Description: Returns a count of all the selected items in the list object
//	Return:  A count of the selected items in the list object or
//		OLISTINVALIDLISTOBJECT oiList is not a valid list object
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListCountSelections(OBJECTID oiList)
	{
	if (AObjGetType(oiList) != OTYPE_LIST) // verify that oiList is the correct object type ;
		return OLISTINVALIDLISTOBJECT ;

	LONG selections = 0 ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);
	if (table)
    	{
		LONG count = OListCountElements(oiList) ;
		for (LONG i = 0 ; i < count; i++)
			{
			if (table[i].isSelected)
				selections++;
			}
		AObjUnlockData(oiList, TABLE_INFO);
		}
	return selections ;
    }
//////////////////////////////////////////////////////////////
//  Function: DeleteElementFunc
//	Description:  Deletes an element specified by index from a list.
//		If no row index is passed, all selected elements are deleted.
//  Input1: List Object
//  Input2 (Optional): Number Object - index of item to be deleted.
//  Flow1: After Deleting
//  No outputs.
//////////////////////////////////////////////////////////////
EXTERNC DeleteElementFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	OBJECTID oiRow = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
	if (oiRow)
		{
		LONG row = ONmbrGetInteger(oiRow) - 1 ; // One based to zero based
		OListDeleteElement(oiList, row) ;
		}
	else   	// No index passed in
		OListDeleteSelections(oiList) ;

	theSystem->message1 = 1;		//only one exit flow
	}
//////////////////////////////////////////////////////////////
//  Function: OListDeleteElement
//	Description: Deletes the list element at a specified index
//		The index is zero based.
//	Return:  OLISTINVALIDLISTOBJECT if the list object is invalid or
//      OLISTROWOUTOFRANGE if the row index is out of range or
//		TRUE otherwise
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListDeleteElement(OBJECTID oiList, LONG index)
	{
	if (AObjGetType(oiList) != OTYPE_LIST) // verify that oiList is the correct object type ;
		return OLISTINVALIDLISTOBJECT ;

	if (index < 0 && index >= OListCountElements(oiList))
		return OLISTROWOUTOFRANGE ;

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	list->RemoveItem(index);
	AObjUnlockData(oiList, LIST_INFO);

	AObjResizeData(oiList, EDIT_INFO, 0);  //Clear the edit box
	list->SendSelectionToText();
	PostObjectChanged(oiList, MAKELONG(LST_ITEMS_CHANGED, 0)) ;
    return TRUE ;
	}
//////////////////////////////////////////////////////////////
//  Function: OListDeleteSelections
//	Description: Deletes all selected elements in the list
//	Return:  OLISTINVALIDLISTOBJECT if the list object is invalid or
//		TRUE otherwise
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListDeleteSelections(OBJECTID oiList)
	{
	if (AObjGetType(oiList) != OTYPE_LIST) // verify that oiList is the correct object type ;
		return OLISTINVALIDLISTOBJECT ;

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	list->RemoveSelectedItems();
	AObjUnlockData(oiList, LIST_INFO);

	AObjResizeData(oiList, EDIT_INFO, 0);  //Clear the edit box
	list->SendSelectionToText();
	PostObjectChanged(oiList, MAKELONG(LST_ITEMS_CHANGED, 0)) ;
	return TRUE ;
	}
//////////////////////////////////////////////////////////////
//  Function: GetElementFunc
//	Description: Retrieves the text of the element at the specified index
//  Input1: List Object
//  Input2: Number Object - index of item to be retrieved.
//  Flow1: After getting the element text.
//  Flow2: If the element cannot be found.
//  Output1: Text Object - retrieved text.
//////////////////////////////////////////////////////////////
EXTERNC GetElementFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(oiList, STRINGS_INFO);

	OBJECTID oiRow = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
	int row = (int)ONmbrGetInteger(oiRow) - 1 ; // One based to zero based

	theSystem->message1 = 2;		//default to not found
	if (table && list && strings)
		{
		LPSTR lpBuffer = NULL ;
		if (row >= 0 && row < list->m_nItems)
			lpBuffer = GetListString(row, table, strings);
		else if (list->m_style & LF_COMBOS)
			lpBuffer = (LPSTR)AObjLockData(oiList, EDIT_INFO);

		if (lpBuffer)
			{
			OBJECTID oiText = AObjCreate(OTYPE_TEXT);
			OTextSetValueFromBuffer(oiText, lpBuffer) ;
			AFuncSetParameter(1, oiText);
			theSystem->message1 = 1;		//found
			}
		}
	if (strings)
		AObjUnlockData(oiList, STRINGS_INFO);
    if (table)
		AObjUnlockData(oiList, TABLE_INFO);
    if (list)
		AObjUnlockData(oiList, LIST_INFO);
	}
//////////////////////////////////////////////////////////////
//  Function: OListGetElementLength
//	Description: Retrieves the string length of the element at the specified index
//  LONG OListGetElementLength(OBJECTID oiList, LONG row) ;
//  	oiList is the object ID of the list object
//		row is the zero based index of the row in the list
//	The return value is
//		the length of the string at the specified row
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTROWOUTOFRANGE if the row is out of range
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListGetElementLength(OBJECTID oiList, LONG row)
	{
	if (AObjGetType(oiList) != OTYPE_LIST)
		return OLISTINVALIDLISTOBJECT ;

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(oiList, STRINGS_INFO);

	LONG lReturn = OLISTROWOUTOFRANGE ;  //default if the row is not found
	if (table && list && strings)
		{
		LPSTR lpBuffer = NULL ;
		if (row >= 0 && row < list->m_nItems)
			lpBuffer = GetListString(row, table, strings);
		else if (list->m_style & LF_COMBOS)
			lpBuffer = (LPSTR)AObjLockData(oiList, EDIT_INFO);

		if (lpBuffer)
			lReturn = lstrlen(lpBuffer) ;
		}
    if (strings)
		AObjUnlockData(oiList, STRINGS_INFO);
    if (table)
		AObjUnlockData(oiList, TABLE_INFO);
    if (list)
		AObjUnlockData(oiList, LIST_INFO);
	return lReturn ;
	}
//////////////////////////////////////////////////////////////
//  Function: OListGetElement
//	Description: Retrieves the text of the element at the specified index
//  LONG OListGetElement(OBJECTID oiList, LONG row, FAR char* szElement, UINT nSize) ;
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
LONG FAR PASCAL _export OListGetElement(OBJECTID oiList, LONG row, LPSTR szElement, UINT nSize)
	{
	if (AObjGetType(oiList) != OTYPE_LIST)
		return OLISTINVALIDLISTOBJECT ;

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(oiList, STRINGS_INFO);

	LONG lReturn = OLISTROWOUTOFRANGE ;  //default if the row is not found
	if (table && list && strings)
		{
		LPSTR lpBuffer = NULL ;
		if (row >= 0 && row < list->m_nItems)
			lpBuffer = GetListString(row, table, strings);
		else if (list->m_style & LF_COMBOS)
			lpBuffer = (LPSTR)AObjLockData(oiList, EDIT_INFO);

		if (lpBuffer)
        	{
			lReturn = lstrlen(lpBuffer) ;
			if (szElement && (nSize > 0))
				{
				lstrcpyn(szElement, lpBuffer, nSize) ;
				}
            }
		}
	if (strings)
		AObjUnlockData(oiList, STRINGS_INFO);
    if (table)
		AObjUnlockData(oiList, TABLE_INFO);
    if (list)
		AObjUnlockData(oiList, LIST_INFO);
	return lReturn ;
	}
//////////////////////////////////////////////////////////////
//  Function: GetSelectionFunc
//	Description: Gets the row number of the selected item
//  Input1: List Object
//  Input2 (Optional): Number Object - nth selected item.
//  Flow1: After returning.
//  Flow2: If the index is out of range.
//  Output1: Number Object - index of the selection.
//////////////////////////////////////////////////////////////
EXTERNC GetSelectionFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST);

	int selection = 0 ;
	OBJECTID oiSelection = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	if (oiSelection)
		selection = (int)ONmbrGetInteger(oiSelection) - 1 ; // One based to zero based

    LONG index = OListGetSelection(oiList, selection) ;
	OBJECTID oiIndex = AObjCreate(OTYPE_NUMBER);
	ONmbrSetInteger(oiIndex, index + 1, FALSE) ;

	if (index < 0)
    	{
		ONmbrSetInteger(oiIndex, 0, FALSE) ;
		theSystem->message1 = 2;		// selection not found
        }
	else
    	{
		ONmbrSetInteger(oiIndex, index + 1, FALSE) ;
		theSystem->message1 = 1;		// selection found
		}

	AFuncSetParameter(1, oiIndex);
	}
//////////////////////////////////////////////////////////////
//  Function: OListGetSelection
//	Description: Gets the row number of the selected item
//	LONG OListGetSelection(OBJECTID oiList, LONG itemIndex)
//		oiList is the object ID of the list object
//		itemIndex is the zero based index of the selected item
//			i.e. 0 specifies the first selected item
//	The return value is
//		the zero based index of the selected element in the list object
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTSELECTIONOUTOFRANGE if the itemIndex is out of range
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListGetSelection(OBJECTID oiList, LONG itemIndex)
	{
	if (AObjGetType(oiList) != OTYPE_LIST)
    	return OLISTINVALIDLISTOBJECT ;

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);

	LONG lReturn = OLISTSELECTIONOUTOFRANGE ;  //default if the selection is not found
	if (table && list)
		{
		int index = 0 ;
		for (int i = 0; i < list->m_nItems; i++)
			{
			if (table[i].isSelected)
				{
				if (index == itemIndex)
					break ;
				index++ ;
				}
			}
		if (i != list->m_nItems)
			{
            lReturn = i ;
			}
		}
	AObjUnlockData(oiList, TABLE_INFO);
	AObjUnlockData(oiList, LIST_INFO);
	return lReturn ;
	}
//////////////////////////////////////////////////////////////
//  Function: NewElementFunc
//	Description: Inserts one or more new elements in a list at
//		a specified position.  To insert a single element, you pass
//		a text object containing the text of that element.  To insert
//		several elements at a time, you pass a Text object containing
//		multiple, return-delimited entries, one for each element.
//		If no index is given, the list object inserts the element(s) at
//		the current selection or at the end of the list if no items are
//		selected.
//  Input1: List Object
//	Input2: Text Object - new element(s) text
//  Input3 (Optional): Number Object - row index.
//  Flow1: After inserting
//  No Output
//////////////////////////////////////////////////////////////
EXTERNC NewElementFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	theSystem->message1 = 1 ;		//only one exit flow
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST) ;

	OBJECTID oiText = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	MEMBLOCKID memText = AMemAllocate(0) ;
	OTextGetToMemblock(oiText, memText) ;
	LPSTR lpText = (LPSTR)AMemLock(memText) ;

	OBJECTID oiRow = AFuncGetTypedParameter(3, OTYPE_NUMBER) ;
    LONG row ;
	if (oiRow)
		{
		row = (int)ONmbrGetInteger(oiRow) - 1 ; // One based to zero based
		}
	else
		{
		// find the first selected row if there is one
		row = OListGetSelection(oiList, 0) ;
		}

	OListNewElement(oiList, lpText, row) ;

	AMemUnlock(memText) ;
	AMemFree(memText) ;
	}
//////////////////////////////////////////////////////////////
//  Function: OListNewElement
//	Description: Inserts one or more new elements in a list at
//		a specified position.  
//		
//	LONG OListNewElement(OBJECTID oiList, LPSTR itemText, LONG startingIndex)
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
LONG FAR PASCAL _export OListNewElement(OBJECTID oiList, LPSTR itemText, LONG startIndex)
	{
	if (AObjGetType(oiList) != OTYPE_LIST)
    	return OLISTINVALIDLISTOBJECT ;

	if (itemText == NULL)
		return 0 ;

	LONG nListItems = OListCountElements(oiList) ;
	if (startIndex < 0 || startIndex > nListItems)
		startIndex = nListItems ; // insert the new element at the end of the list

	LONG index = startIndex ;
	UINT end = 0 ;
	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	for (UINT i = 0; GetNextLine(itemText, i, end); i = end+1, index++)
		{
		if (itemText[end] == 0)
			{
			list->addItem(&itemText[i], index);
			break ;
			}
		else
			{
			itemText[end] = 0 ;
			list->addItem(&itemText[i], index);
			}
		if (end > 32767)
			break ;
		}
	AObjUnlockData(oiList, LIST_INFO) ;
	PostObjectChanged(oiList, MAKELONG(LST_ITEMS_CHANGED, 0));
    return (index - startIndex) ;
	}

//////////////////////////////////////////////////////////////
//  Function: SetElementFunc
//	Description:  Replaces the text of a specified element with new text.
//		If a row index is not provided then the text in the first
//		selected element is set.  If no element is selected and no row
//		index is provided then this function does nothing.
//  Input1: List Object
//	Input2: Text Object - replacement text
//  Input3 (Optional): Number Object - row index.
//  Flow1: After setting
//  No Output
//////////////////////////////////////////////////////////////
EXTERNC SetElementFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	theSystem->message1 = 1;		//only one exit flow
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
//	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;

	// Get the text
	OBJECTID oiText = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	MEMBLOCKID memText = AMemAllocate(0) ;
	OTextGetToMemblock(oiText, memText) ;
	LPSTR lpText = (LPSTR)AMemLock(memText) ;

	// Get the row where the text will be set
	OBJECTID oiRow = AFuncGetTypedParameter(3, OTYPE_NUMBER) ;
	int row ;
	if (oiRow)
		row = (int)ONmbrGetInteger(oiRow) - 1 ; // One based to zero based
	else
		{
		// find the first selected row if there is one
		row = OListGetSelection(oiList, 0) ;
		}

	OListSetElement(oiList, lpText, row) ;

	AMemUnlock(memText) ;
	AMemFree(memText) ;
	return ;
	}
//////////////////////////////////////////////////////////////
//  Function: OListSetElement
//	Description:  Replaces the text of the indexed element with itemText.
//
//	LONG OListSetElement(OBJECTID oiList, LPSTR itemText, LONG itemIndex)
//		oiList is the object ID of the list object
//		itemText points to the replacement text.  
//		itemIndex is the zero based index where the element text is to be replaced.  
//	The return value is
//		TRUE if successful
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTSELECTIONOUTOFRANGE if the itemIndex is out of range
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListSetElement(OBJECTID oiList, LPSTR itemText, LONG itemIndex)
	{
	if (AObjGetType(oiList) != OTYPE_LIST)
    	return OLISTINVALIDLISTOBJECT ;

	if (itemText == NULL)
		return FALSE ;

	LONG nListItems = OListCountElements(oiList) ;

 	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	if (list->m_style & LF_COMBOS)
		OListSetEditText(oiList, itemText) ;
	AObjUnlockData(oiList, LIST_INFO) ;

	if (itemIndex < 0 || itemIndex > nListItems)
		return OLISTSELECTIONOUTOFRANGE ;
	// if row is within the range of the list

	if (itemIndex > OLISTMAXINDEX)
		return OLISTSELECTIONOUTOFRANGE ; // until we move to a 32-bit platform or modify our code we are limited to 64K rows

	UINT row = (UINT) itemIndex ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);  AObjLockData(oiList, LIST_INFO) ;
	BOOL isSelected = table[row].isSelected;

	list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	list->RemoveItem(row);			//remove the old
	list->addItem(itemText, row);		//add the new
	table[row].isSelected = isSelected;
	list->SendSelectionToText();
	AObjUnlockData(oiList, LIST_INFO) ;

	AObjUnlockData(oiList, TABLE_INFO);
	PostObjectChanged(oiList, MAKELONG(LST_ITEMS_CHANGED, 0));
    return TRUE ;
	}
//////////////////////////////////////////////////////////////
//  Function: OListSetEditText
//	Description:  Sets the text of the combo box edit control.
//
//	LONG OListSetEditText(OBJECTID oiList, LPSTR itemText)
//		oiList is the object ID of the list object
//		itemText points to the new edit text.  
//	The return value is
//		the number of elements added to the list or
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied 
//////////////////////////////////////////////////////////////
void FAR PASCAL _export OListSetEditText(OBJECTID oiList, LPSTR lpText)
	{
	if (AObjGetType(oiList) != OTYPE_LIST || lpText == NULL)
		return ;

	AObjResizeData(oiList, EDIT_INFO, lstrlen(lpText)+1);
	LPSTR editBoxText = (LPSTR)AObjLockData(oiList, EDIT_INFO);
	lstrcpy(editBoxText, lpText);
	AObjUnlockData(oiList, EDIT_INFO);
	PostObjectChanged(oiList, MAKELONG(LST_EDITCONTROL_CHANGED, 0));
	}
//////////////////////////////////////////////////////////////
//  Function: SetSelectionFunc
//	Description:  Selects an element specified by the row index.
//  Input1: List Object
//  Input2: Number Object - row index.
//  Flow1: After selecting
//  No Output
//////////////////////////////////////////////////////////////
EXTERNC SetSelectionFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	OBJECTID oiRow = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
	LONG row = ONmbrGetInteger(oiRow) - 1 ;

	OListSetSelection(oiList, row) ;

	theSystem->message1 = 1;		//only one exit flow
	}
//////////////////////////////////////////////////////////////
//  Function: OListSetSelection
//	Description:  Selects an element specified by the row index.
//
//	LONG OListSetSelection(OBJECTID oiList, LONG itemIndex)
//		oiList is the object ID of the list object
//		itemIndex is the zero based index of the element to be selected.
//	The return value is
//		TRUE if successful
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//		OLISTSELECTIONOUTOFRANGE if the itemIndex is out of range
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListSetSelection(OBJECTID oiList, LONG itemIndex)
	{
	if (AObjGetType(oiList) != OTYPE_LIST)
    	return OLISTINVALIDLISTOBJECT ;

	LONG nListItems = OListCountElements(oiList) ;

	if (itemIndex < 0 || itemIndex > nListItems)
		return OLISTSELECTIONOUTOFRANGE ;
	// if row is within the range of the list

	if (itemIndex > OLISTMAXINDEX)
		return OLISTSELECTIONOUTOFRANGE ; // until we move to a 32-bit platform or modify our code we are limited to 64K rows

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);

	if (list && table)
		{
		if (!(list->m_style & LF_MULTISELECT))
			for(int i = 0; i < nListItems; i++)	//clear the old ones
				table[i].isSelected = FALSE ;

		table[(UINT)itemIndex].isSelected = TRUE ;
		list->SendSelectionToText();
		PostObjectChanged(oiList, MAKELONG(LST_APPERANCE_CHANGED, 0));
		}

	AObjUnlockData(oiList, TABLE_INFO);
	AObjUnlockData(oiList, LIST_INFO) ;
    return TRUE ;
    }
//////////////////////////////////////////////////////////////
//  Function: SortListFunc
//	Description:  Sorts a specified list.
//  Input1: List Object
//  Flow1: After sorting
//  No Output
//////////////////////////////////////////////////////////////
EXTERNC SortListFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	OListSort(oiList) ;
	theSystem->message1 = 1;		//only one exit flow
	}
//////////////////////////////////////////////////////////////
//  Function: OListSort
//	Description:  Sorts a list.
//
//	LONG OListSort(OBJECTID oiList)
//		oiList is the object ID of the list object
//	The return value is
//		TRUE if successful
//		OLISTINVALIDLISTOBJECT if an invalid list object is supplied
//////////////////////////////////////////////////////////////
LONG FAR PASCAL _export OListSort(OBJECTID oiList)
	{
	if (AObjGetType(oiList) != OTYPE_LIST)
    	return OLISTINVALIDLISTOBJECT ;

	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(oiList, STRINGS_INFO);
	if (list && table && strings)
		{
		SortList(table, strings, list->m_nItems);
		list->SendSelectionToText();
		PostObjectChanged(oiList, MAKELONG(LST_ITEMS_CHANGED, 0));
		}
	AObjUnlockData(oiList, STRINGS_INFO) ;
	AObjUnlockData(oiList, TABLE_INFO) ;
	AObjUnlockData(oiList, LIST_INFO) ;
    return TRUE ;
	}
////////////////////////////////////////
////////////////////////////////////////
BOOL GetNextLine(LPSTR str, UINT& start, UINT& end)
	{
	if (!str || !*str)
		return FALSE ;

	while (str[start] == '\r' || str[start] == '\n')
		start++ ;
	end = start ;

	if(str[start] == 0)
		return FALSE ;

	while ((str[end] != '\r') && (str[end] != '\n') && (str[end] != 0))
		end++ ;
	return TRUE ;
	}
