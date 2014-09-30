/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_LIST_H
#define O_LIST_H

/* ***********************************************************************
 * ATYPEID for List Object
 * *********************************************************************** */

#define OTYPE_LIST                  ATYPE_MAKEID('L','i','s','t')

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
LONG ALMAPI OListDeleteAll(AOBJECTID oiList) ;
LONG ALMAPI OListClearSelections(AOBJECTID oiList) ;
LONG ALMAPI OListCountElements(AOBJECTID oiList) ;
LONG ALMAPI OListCountSelections(AOBJECTID oiList) ;
LONG ALMAPI OListDeleteElement(AOBJECTID oiList, LONG index) ;
LONG ALMAPI OListDeleteSelections(AOBJECTID oiList) ;
LONG ALMAPI OListGetElementLength(AOBJECTID oiList, LONG row) ;
LONG ALMAPI OListGetElement(AOBJECTID oiList, LONG row, LPSTR szElement, UINT nSize) ;
LONG ALMAPI OListGetSelection(AOBJECTID oiList, LONG itemIndex) ;
LONG ALMAPI OListNewElement(AOBJECTID oiList, LPSTR itemText, LONG startIndex) ;
LONG ALMAPI OListSetElement(AOBJECTID oiList, LPSTR itemText, LONG itemIndex) ;
void ALMAPI OListSetEditText(AOBJECTID oiList, LPSTR lpText) ;
LONG ALMAPI OListSetSelection(AOBJECTID oiList, LONG itemText) ;
LONG ALMAPI OListSort(AOBJECTID oiList) ;
#ifdef __cplusplus
}
#endif

#endif  /* !O_LIST_H */
