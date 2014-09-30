#include "l_includ.h"



//////////////////////////////////////////////////////////////
//  Function: ListFunction_DeleteAll
//	Description:  Deletes all items in the list
//  Input1: List Object
//  Flow1: Done
//  No Outputs
//////////////////////////////////////////////////////////////
extern "C" void ALMAPI ListFunction_DeleteAll(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList;

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	OListDeleteAll(oiList);

	theSystem->message1 = 1;		//only one exit flow
	}


//////////////////////////////////////////////////////////////
//  Function: ListFunction_ClearSelections
//	Description:  Deselects all selected items in a list object
//  Input1: List Object
//  Flow1: After Clearing
//  No Outputs
//////////////////////////////////////////////////////////////
extern "C" void ALMAPI ListFunction_ClearSelections(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList;

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	OListClearSelections(oiList);

	theSystem->message1 = 1;		//only one exit flow
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
extern "C" void ALMAPI ListFunction_CountElements(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList, oiElements, oiSelections;

	theSystem->message1 = 1;		//only one exit flow

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	oiElements = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiElements, return);

	oiSelections = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiSelections, return);

	ONmbrSetInteger(oiElements, OListCountElements(oiList), FALSE) ;
	ONmbrSetInteger(oiSelections, OListCountSelections(oiList), FALSE) ;

	AFuncSetParameter(1, oiElements);
	AFuncSetParameter(2, oiSelections);
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
extern "C" void ALMAPI ListFunction_DeleteElement(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList, oiRow;
	LONG 										row;

	theSystem->message1 = 1;		//only one exit flow

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	oiRow = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
	if (oiRow)
		{
		row = ONmbrGetInteger(oiRow) - 1 ; // One based to zero based
		OListDeleteElement(oiList, row) ;
		}
	else   	// No index passed in
		OListDeleteSelections(oiList) ;

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
extern "C" void ALMAPI  ListFunction_GetElement(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList, oiRow, oiText;
	LPLIST 									list;
	LPLIST_TABLE 						table;
	LPSTR 									strings, szBuffer;
	int 										row;

	theSystem->message1 = 2;		//default to not found

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	oiRow = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
	I_VERIFY_PARAMETER(oiRow, return);
	row = (int)ONmbrGetInteger(oiRow) - 1 ; // One based to zero based

	oiText = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiText, return);

	list = LockListObject(oiList);
	I_VERIFY_POINTER(list, return);

	if (row >= 0 && row < list->NumItems())
		{
		table = LockListTable(oiList);
		I_VERIFY_POINTER(table, return);

		strings = LockListStrings(oiList);
		I_VERIFY_POINTER(strings, return);

		if ( (szBuffer = GetListString(row, table, strings)) != NULL)
			{
			OTextSetValueFromBuffer(oiText, szBuffer);
			theSystem->message1 = 1;		//found
			}

		UnlockListTable(oiList);
		UnlockListStrings(oiList);
		}

	else if (list->IsListCombobox())
		{
		szBuffer = LockListEdit(oiList);
		if (szBuffer)
			{
			OTextSetValueFromBuffer(oiText, szBuffer);
			theSystem->message1 = 1;		//found
			}
		UnlockListEdit(oiList);
		}

	UnlockListObject(oiList);

	AFuncSetParameter(1, oiText);
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
extern "C" void ALMAPI  ListFunction_GetSelection(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList, oiSelection, oiIndex;
	int 										selection;
	long										index;

	theSystem->message1 = 2;		// selection not found

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	oiIndex = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiIndex, return);

	oiSelection = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	if (oiSelection)
		selection = (int)ONmbrGetInteger(oiSelection) - 1 ; // One based to zero based
	else
		selection = 0 ;
		
  index = OListGetSelection(oiList, selection) ;

	if (index < 0)
		ONmbrSetInteger(oiIndex, 0, FALSE) ;
	else
   	{
		ONmbrSetInteger(oiIndex, index + 1, FALSE) ;
		theSystem->message1 = 1;		// selection found
		}

	AFuncSetParameter(1, oiIndex);
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
extern "C" void ALMAPI  ListFunction_NewElement(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList, oiText, oiRow;
	MEMBLOCKID 							miText;
	LPSTR 									szText;
  LONG 										row ;

	theSystem->message1 = 1 ;		//only one exit flow

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	oiText = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	I_VERIFY_PARAMETER(oiText, return);

	miText = AMemAllocate(0);
	OTextGetToMemblock(oiText, miText) ;
	szText = (LPSTR)AMemLock(miText) ;

	oiRow = AFuncGetTypedParameter(3, OTYPE_NUMBER) ;
	if (oiRow)
		row = (int)ONmbrGetInteger(oiRow) - 1 ; // One based to zero based
	else
		row = OListGetSelection(oiList, 0); // find the first selected row if there is one

	OListNewElement(oiList, szText, row);

	AMemUnlock(miText);
	AMemFree(miText);
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
extern "C" void ALMAPI  ListFunction_SetElement(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList, oiText, oiRow;
	MEMBLOCKID 							miText;
	LPSTR 									szText;
	int 										row;

	theSystem->message1 = 1;		//only one exit flow

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	// Get the text
	oiText = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	I_VERIFY_PARAMETER(oiText, return);

	miText = AMemAllocate(0) ;
	OTextGetToMemblock(oiText, miText);

	szText = (LPSTR)AMemLock(miText);
	I_VERIFY_POINTER(szText, return);

	// Get the row where the text will be set
	oiRow = AFuncGetTypedParameter(3, OTYPE_NUMBER) ;
	if (oiRow)
		row = (int)ONmbrGetInteger(oiRow) - 1 ; // One based to zero based
	else
		row = (int)OListGetSelection(oiList, 0); // find the first selected row if there is one

	OListSetElement(oiList, szText, row) ;

	AMemUnlock(miText);
	AMemFree(miText);
	}

//////////////////////////////////////////////////////////////
//  Function: SetSelectionFunc
//	Description:  Selects an element specified by the row index.
//  Input1: List Object
//  Input2: Number Object - row index.
//  Flow1: After selecting
//  No Output
//////////////////////////////////////////////////////////////
extern "C" void ALMAPI  ListFunction_SetSelection(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList, oiRow;

	theSystem->message1 = 1;		//only one exit flow

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	oiRow = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
	I_VERIFY_PARAMETER(oiRow, return);
	
	OListSetSelection(oiList, ONmbrGetInteger(oiRow) - 1);
	}

//////////////////////////////////////////////////////////////
//  Function: SortListFunc
//	Description:  Sorts a specified list.
//  Input1: List Object
//  Flow1: After sorting
//  No Output
//////////////////////////////////////////////////////////////
extern "C" void ALMAPI  ListFunction_SortList(
	pAEvtInfo, 
	pAObjMessage 						theSystem)
	{
	OBJECTID 								oiList;

	theSystem->message1 = 1;		//only one exit flow

	oiList = AFuncGetTypedParameter(1, OTYPE_LIST);
	I_VERIFY_PARAMETER(oiList, return);

	OListSort(oiList) ;
	}
