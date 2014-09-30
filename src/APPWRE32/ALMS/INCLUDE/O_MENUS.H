/* ***********************************************************************
 *
 * Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.
 *
 * THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
 * TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
 * COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
 * CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 * TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
 * NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
 * AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
 * LIABILITY.
 *
 * *********************************************************************** */

#ifndef O_MENUS_H
#define O_MENUS_H

/* ***********************************************************************
 * TYPEIDs for Menu ALM
 * *********************************************************************** */

#define OTYPE_MENUBAR                   ATYPE_MAKEID('M','b','a','r')
#define OTYPE_MENU                      ATYPE_MAKEID('M','e','n','u')
#define OTYPE_MENUITEM                  ATYPE_MAKEID('M','i','t','m')

/* ***********************************************************************
 * EVENTIDs for Menu ALM
 * *********************************************************************** */

#define OEVENT_GETMENUHANDLE            AEVENT_MAKEID('m','g','m','H')
#define OEVENT_RELEASEMENUHANDLE        AEVENT_MAKEID('m','r','m','H')
#define OEVENT_HANDLECOMMAND            AEVENT_MAKEID('m','c','m','D')
#define OEVENT_HANDLESELECT             AEVENT_MAKEID('m','s','e','L')

/* Sent to owners of foreign menu items when the foreign item is selected */
#define OEVENT_MENUITEMCHOSEN           AEVENT_MAKEID('m','c','h','S')

/* structure for passing parameters with an event */
typedef struct tagMENUSPARAMBLOCK
  {
  AOBJECTID  objID;
  HMENU     hMenu;
  DWORD      itemID;
  } MENUSPARAMBLOCK, far *LPMENUSPARAMBLOCK;


/* structure for item list element. Used by OMenuAddItems */
#define OMENU_MAXITEMNAME   80
#define OMENU_DISABLED      0x00001
typedef struct tagITEMSPEC
  {
  DWORD  itemID;
  WORD  status;
  char  itemText[OMENU_MAXITEMNAME+1];
  } ITEMSPEC, far* LPITEMSPEC;


/* ***********************************************************************
 * Function prototypes for Menu ALM
 * *********************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif

BOOL ALMAPI OMenuIsPopup(
  AOBJECTID    objID);

BOOL ALMAPI OMenuIsEnabled(
  AOBJECTID    objID);

short ALMAPI OMenuGetTitle(
  AOBJECTID    objID,
  LPSTR       buffer,
  short       bufferSize);

BOOL ALMAPI OMenuAddItems(
  AOBJECTID    ownerID,
  AOBJECTID    menuID,
  AMEMBLOCKID  itemList,
  short       position);

BOOL ALMAPI OMenuDeleteItems(
  AOBJECTID    menuID,
  AMEMBLOCKID  itemList);

AOBJECTID ALMAPI OMenuAddSubmenu(
  AOBJECTID    ownerID,
  AOBJECTID    menuID,
  AMEMBLOCKID  itemList,
  short       position);

BOOL ALMAPI OMenuDeleteSubmenu(
  AOBJECTID    menuID,
  AOBJECTID    submenuID);

short ALMAPI OMenuDeletePosition(
  AOBJECTID    menuID,
  short       position,
  short       count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ifndef O_MENUS_H */
