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

#define OTYPE_MENUBAR                   MAKETYPEID('M','b','a','r')
#define OTYPE_MENU                      MAKETYPEID('M','e','n','u')
#define OTYPE_MENUITEM                  MAKETYPEID('M','i','t','m')

/* ***********************************************************************
 * EVENTIDs for Menu ALM
 * *********************************************************************** */

#define OEVENT_GETMENUHANDLE            MAKEEVENTID('m','g','m','H')
#define OEVENT_RELEASEMENUHANDLE        MAKEEVENTID('m','r','m','H')
#define OEVENT_HANDLECOMMAND            MAKEEVENTID('m','c','m','D')
#define OEVENT_HANDLESELECT             MAKEEVENTID('m','s','e','L')

/* Sent to owners of foreign menu items when the foreign item is selected */
#define OEVENT_MENUITEMCHOSEN           MAKEEVENTID('m','c','h','S')

/* structure for passing parameters with an event */
typedef struct tagMENUSPARAMBLOCK
  {
  OBJECTID  objID;
  HMENU     hMenu;
  WORD      itemID;
  } MENUSPARAMBLOCK, far *LPMENUSPARAMBLOCK;


/* structure for item list element. Used by OMenuAddItems */
#define OMENU_MAXITEMNAME   80
#define OMENU_DISABLED      0x00001
typedef struct tagITEMSPEC
  {
  WORD  itemID;
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
  OBJECTID    objID);

BOOL ALMAPI OMenuIsEnabled(
  OBJECTID    objID);

short ALMAPI OMenuGetTitle(
  OBJECTID    objID,
  LPSTR       buffer,
  short       bufferSize);

BOOL ALMAPI OMenuAddItems(
  OBJECTID    ownerID,
  OBJECTID    menuID,
  MEMBLOCKID  itemList,
  short       position);

BOOL ALMAPI OMenuDeleteItems(
  OBJECTID    menuID,
  MEMBLOCKID  itemList);

OBJECTID ALMAPI OMenuAddSubmenu(
  OBJECTID    ownerID,
  OBJECTID    menuID,
  MEMBLOCKID  itemList,
  short       position);

BOOL ALMAPI OMenuDeleteSubmenu(
  OBJECTID    menuID,
  OBJECTID    submenuID);

short ALMAPI OMenuDeletePosition(
  OBJECTID    menuID,
  short       position,
  short       count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ifndef O_MENUS_H */
