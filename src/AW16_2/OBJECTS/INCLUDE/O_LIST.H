/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_LIST_H
#define O_LIST_H

/* ***********************************************************************
 * TYPEID for List Object
 * *********************************************************************** */

#define OTYPE_LIST                  MAKETYPEID('L','i','s','t')

/* ***********************************************************************
 * Constants for List Object
 * *********************************************************************** */

#define OLISTINVALIDLISTOBJECT      (   -1L)
#define OLISTROWOUTOFRANGE          (   -2L)
#define OLISTSELECTIONOUTOFRANGE    (   -3L)
#define OLISTMAXINDEX               (65534L)

/* ***********************************************************************
 * Function Prototypes for List Object
 * *********************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif
LONG ALMAPI OListDeleteAll(OBJECTID oiList) ;
LONG ALMAPI OListClearSelections(OBJECTID oiList) ;
LONG ALMAPI OListCountElements(OBJECTID oiList) ;
LONG ALMAPI OListCountSelections(OBJECTID oiList) ;
LONG ALMAPI OListDeleteElement(OBJECTID oiList, LONG index) ;
LONG ALMAPI OListDeleteSelections(OBJECTID oiList) ;
LONG ALMAPI OListGetElementLength(OBJECTID oiList, LONG row) ;
LONG ALMAPI OListGetElement(OBJECTID oiList, LONG row, LPSTR szElement, UINT nSize) ;
LONG ALMAPI OListGetSelection(OBJECTID oiList, LONG itemIndex) ;
LONG ALMAPI OListNewElement(OBJECTID oiList, LPSTR itemText, LONG startIndex) ;
LONG ALMAPI OListSetElement(OBJECTID oiList, LPSTR itemText, LONG itemIndex) ;
void ALMAPI OListSetEditText(OBJECTID oiList, LPSTR lpText) ;
LONG ALMAPI OListSetSelection(OBJECTID oiList, LONG itemText) ;
LONG ALMAPI OListSort(OBJECTID oiList) ;
#ifdef __cplusplus
}
#endif

#endif  /* !O_LIST_H */
